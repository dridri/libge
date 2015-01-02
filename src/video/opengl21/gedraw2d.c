 /*
	The Gamma Engine Library is a multiplatform library made to make games
	Copyright (C) 2012  Aubry Adrien (dridri85)

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "../../ge_internal.h"
#include "ge_viddrv.h"
extern ge_Shader* ge_current_shader;

void geDrawLineScreen(int x0, int y0, int x1, int y1, u32 color){
	geDrawLineScreenDepth(x0, y0, 0, x1, y1, 0, color);
}

void geDrawLineScreenFade(int x0, int y0, int x1, int y1, u32 color0, u32 color1){
	geDrawLineScreenFadeDepth(x0, y0, 0, x1, y1, 0, color0, color1);
}

void geFillRectScreen(int x, int y, int width, int height, u32 color){
	geFillRectScreenDepth(x, y, 0, width, height, color);
}

void geDrawLineScreenDepth(int x0, int y0, int z0, int x1, int y1, int z1, u32 color){
	geDrawLineScreenFadeDepth(x0, y0, z0, x1, y1, z1, color, color);
}

void geDrawLineScreenFadeDepth(int x0, int y0, int z0, int x1, int y1, int z1, u32 color0, u32 color1){
	if(abs(z0) > 2048 || abs(z1) > 2048){
		return;
	}
	x0 += libge_context->draw_off_x;
	x1 += libge_context->draw_off_x;
	y0 += libge_context->draw_off_y;
	y1 += libge_context->draw_off_y;

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	if(!ge_current_shader){
		geShaderUse(_ge_GetVideoContext()->shader2d);
	}
	if(ge_current_shader == _ge_GetVideoContext()->shader2d){
		geShaderUniform1f(_ge_GetVideoContext()->loc_textured, 0.0);
	}

	glBegin(GL_LINES);
		glColor4ub(R(color0), G(color0), B(color0), A(color0));
		glTexCoord3f(-1.0, -1.0, -1.0);
		glVertex3f(x0, y0, z0+libge_context->img_stack[z0+2048]);
		glColor4ub(R(color1), G(color1), B(color1), A(color1));
		glVertex3f(x1, y1, z1+libge_context->img_stack[z1+2048]);
	glEnd();

	libge_context->img_stack[z0+2048] += 0.001;
	libge_context->img_stack[z1+2048] += 0.001;
	glEnable(GL_TEXTURE_2D);
}

void geFillRectScreenDepth(int x, int y, int z, int width, int height, u32 color){
	if(abs(z) > 2048){
		return;
	}
	x += libge_context->draw_off_x;
	y += libge_context->draw_off_y;
	
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	if(!ge_current_shader){
		geShaderUse(_ge_GetVideoContext()->shader2d);
	}
	if(ge_current_shader == _ge_GetVideoContext()->shader2d){
		geShaderUniform1f(_ge_GetVideoContext()->loc_textured, 0.0);
	}

	glBegin(GL_QUADS);
		glColor4ub(R(color), G(color), B(color), A(color));
		glTexCoord3f(-1.0, -1.0, -1.0);
		glVertex3f(x, y, z+libge_context->img_stack[z+2048]);
		glVertex3f(x+width, y, z+libge_context->img_stack[z+2048]);
		glVertex3f(x+width, y+height, z+libge_context->img_stack[z+2048]);
		glVertex3f(x, y+height, z+libge_context->img_stack[z+2048]);
	glEnd();

	libge_context->img_stack[z+2048] += 0.001;
	glEnable(GL_TEXTURE_2D);
}

void geBlitImage(int x, int y, ge_Image* img, int _sx, int _sy, int width, int height, int flags){
	geBlitImageDepthStretched(x, y, 0, img, _sx, _sy, width, height, width, height, flags);
}

void geBlitImageStretched(int x, int y, ge_Image* img, int _sx, int _sy, int ex, int ey, int width, int height, int flags){
	geBlitImageDepthStretched(x, y, 0, img, _sx, _sy, ex, ey, width, height, flags);
}

void geBlitImageRotated(int x, int y, ge_Image* img, int _sx, int _sy, int ex, int ey, float angle, int flags){
	geBlitImageDepthRotated(x, y, 0, img, _sx, _sy, ex, ey, angle, flags);
}

void geBlitImageDepthRotated(int x, int y, int z, ge_Image* img, int _sx, int _sy, int ex, int ey, float angle, int flags){
	if(!img)return;
	if((t_ptr)img==0xBAADF00D)return;
	if(!img->id)return;
	if(abs(z) > 2048){
		return;
	}
	if(!(flags & GE_BLIT_NOOFFSET)){
		x += libge_context->draw_off_x;
		y += libge_context->draw_off_y;
	}

	if(flags & GE_BLIT_NOALPHA){
		glDisable(GL_ALPHA_TEST);
		glDisable(GL_BLEND);
	}else{
		glEnable(GL_ALPHA_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	float texMaxX = img->u;
	float texMaxY = img->v;
	float sx = _sx*texMaxX/img->width;
	float sy = _sy*texMaxY/img->height;
	texMaxX = ex*texMaxX/img->width;
	texMaxY = ey*texMaxY/img->height;
	float width = ex;
	float height = ey;
	width *= 0.5f;
	height *= 0.5f;

	float Cos = geCos(angle);
	float Sin = geSin(-angle);

	float sw = Sin*width;
	float sh = Sin*height;
	float cw = Cos*width;
	float ch = Cos*height;

	int mw = 0;
	int mh = 0;
	if(!(flags & GE_BLIT_CENTERED)){
		mw = (ex-x) / 2;
		mh = (ey-y) / 2;
		mw += (int)((x - cw - sh) - (x - cw + sh));
		mh += (int)((y - sw + ch) - (y - sw - ch));
	}
	x += mw;
	y += mh;
	
	if(!ge_current_shader){
		geShaderUse(_ge_GetVideoContext()->shader2d);
	}
	if(ge_current_shader == _ge_GetVideoContext()->shader2d){
		geShaderUniform1f(_ge_GetVideoContext()->loc_textured, 1.0);
		glUniform1f(_ge_GetVideoContext()->shader2d->loc_time, ((float)geGetTick()) / 1000.0);
		glUniform1f(_ge_GetVideoContext()->shader2d->loc_ratio, ((float)libge_context->width) / ((float)libge_context->height));
	}
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, img->id);

	glBegin(GL_TRIANGLE_FAN);
		glColor4ub(R(img->color), G(img->color), B(img->color), A(img->color));
		glTexCoord2f(sx, sy+texMaxY);		 glVertex3f(x - cw - sh, y - sw + ch, (float)z+libge_context->img_stack[z+2048]);
		glTexCoord2f(sx, sy);				 glVertex3f(x - cw + sh, y - sw - ch, (float)z+libge_context->img_stack[z+2048]);
		glTexCoord2f(sx+texMaxX, sy);		 glVertex3f(x + cw + sh, y + sw - ch, (float)z+libge_context->img_stack[z+2048]);
		glTexCoord2f(sx+texMaxX, sy+texMaxY);glVertex3f(x + cw - sh, y + sw + ch, (float)z+libge_context->img_stack[z+2048]);
	glEnd();
	
	libge_context->img_stack[z+2048] += 0.001;
}

void geBlitImageDepthStretched(int x, int y, int z, ge_Image* img, int _sx, int _sy, int ex, int ey, int width, int height, int flags){
	if(!img)return;
	if((t_ptr)img==0xBAADF00D)return;
	if(!img->id)return;
	if(abs(z) > 2048){
		return;
	}
	x += libge_context->draw_off_x;
	y += libge_context->draw_off_y;

	if(flags & GE_BLIT_CENTERED){
		x -= width / 2;
		y -= height / 2;
	}
	
	if(x > libge_context->width || x+width < 0 || y > libge_context->height || y+height < 0){
		return;
	}

	if(flags & GE_BLIT_NOALPHA){
		glDisable(GL_ALPHA_TEST);
		glDisable(GL_BLEND);
	}else{
		glEnable(GL_ALPHA_TEST);
		glEnable(GL_BLEND);
		int b_src = libge_context->blend_src;
		int b_dst = libge_context->blend_dst;
		b_src = (b_src == GE_DEFAULT) ? GL_SRC_ALPHA : b_src;
		b_dst = (b_dst == GE_DEFAULT) ? GL_ONE_MINUS_SRC_ALPHA : b_dst;
		glBlendFunc(b_src, b_dst);
	}

	float texMaxX = img->u;
	float texMaxY = img->v;
	float sx = _sx*texMaxX/img->width;
	float sy = _sy*texMaxY/img->height;
	texMaxX = ex*texMaxX/img->width;
	texMaxY = ey*texMaxY/img->height;

	int tex_mode = GL_TEXTURE_2D;
	
	if(!ge_current_shader){
		geShaderUse(_ge_GetVideoContext()->shader2d);
	}
	if(ge_current_shader == _ge_GetVideoContext()->shader2d){
		geShaderUniform1f(_ge_GetVideoContext()->loc_textured, 1.0);
		glUniform1f(ge_current_shader->loc_time, ((float)geGetTick()) / 1000.0);
		glUniform1f(ge_current_shader->loc_ratio, ((float)libge_context->width) / ((float)libge_context->height));
	}
	
	glEnable(tex_mode);
	glBindTexture(tex_mode, img->id);
	
	if(flags & GE_BLIT_VFLIP){
		glBegin(GL_QUADS);
			glColor4ub(R(img->color), G(img->color), B(img->color), A(img->color));
	
			glTexCoord2f(sx, sy+texMaxY);
			glVertex3f(x, y, z+libge_context->img_stack[z+2048]);

			glTexCoord2f(sx+texMaxX, sy+texMaxY);
			glVertex3f(x+width, y, z+libge_context->img_stack[z+2048]);

			glTexCoord2f(sx+texMaxX, sy);
			glVertex3f(x+width, y+height, z+libge_context->img_stack[z+2048]);

			glTexCoord2f(sx, sy);
			glVertex3f(x, y+height, z+libge_context->img_stack[z+2048]);
		glEnd();
	}else{
		glBegin(GL_QUADS);
			glColor4ub(R(img->color), G(img->color), B(img->color), A(img->color));
	
			glTexCoord2f(sx, sy);
			glVertex3f(x, y, z+libge_context->img_stack[z+2048]);

			glTexCoord2f(sx+texMaxX, sy);
			glVertex3f(x+width, y, z+libge_context->img_stack[z+2048]);

			glTexCoord2f(sx+texMaxX, sy+texMaxY);
			glVertex3f(x+width, y+height, z+libge_context->img_stack[z+2048]);

			glTexCoord2f(sx, sy+texMaxY);
			glVertex3f(x, y+height, z+libge_context->img_stack[z+2048]);
		glEnd();
	}

	libge_context->img_stack[z+2048] += 0.001;
}

void geBlitImageDepthStretchedRotated(int x, int y, int z, ge_Image* img, int _sx, int _sy, int ex, int ey, int width, int height, float angle, int flags){
	if(!img)return;
	if((t_ptr)img==0xBAADF00D)return;
	if(!img->id)return;
	if(abs(z) > 2048){
		return;
	}
	if(!(flags & GE_BLIT_NOOFFSET)){
		x += libge_context->draw_off_x;
		y += libge_context->draw_off_y;
	}

	if(flags & GE_BLIT_NOALPHA){
		glDisable(GL_ALPHA_TEST);
		glDisable(GL_BLEND);
	}else{
		glEnable(GL_ALPHA_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	float texMaxX = img->u;
	float texMaxY = img->v;
	float sx = _sx*texMaxX/img->width;
	float sy = _sy*texMaxY/img->height;
	texMaxX = ex*texMaxX/img->width;
	texMaxY = ey*texMaxY/img->height;

	float Cos = geCos(angle);
	float Sin = geSin(-angle);

	float sw = Sin*width*0.5;
	float sh = Sin*height*0.5;
	float cw = Cos*width*0.5;
	float ch = Cos*height*0.5;

	int mw = 0;
	int mh = 0;
	if(!(flags & GE_BLIT_CENTERED)){
		mw = (width-x) / 2;
		mh = (height-y) / 2;
		mw += (int)((x - cw - sh) - (x - cw + sh));
		mh += (int)((y - sw + ch) - (y - sw - ch));
	}
	x += mw;
	y += mh;
	
	if(!ge_current_shader){
		geShaderUse(_ge_GetVideoContext()->shader2d);
	}
	if(ge_current_shader == _ge_GetVideoContext()->shader2d){
		geShaderUniform1f(_ge_GetVideoContext()->loc_textured, 1.0);
		glUniform1f(_ge_GetVideoContext()->shader2d->loc_time, ((float)geGetTick()) / 1000.0);
		glUniform1f(_ge_GetVideoContext()->shader2d->loc_ratio, ((float)libge_context->width) / ((float)libge_context->height));
	}
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, img->id);

	glBegin(GL_TRIANGLE_FAN);
		glColor4ub(R(img->color), G(img->color), B(img->color), A(img->color));
		glTexCoord2f(sx, sy+texMaxY);		 glVertex3f(x - cw - sh, y - sw + ch, (float)z+libge_context->img_stack[z+2048]);
		glTexCoord2f(sx, sy);				 glVertex3f(x - cw + sh, y - sw - ch, (float)z+libge_context->img_stack[z+2048]);
		glTexCoord2f(sx+texMaxX, sy);		 glVertex3f(x + cw + sh, y + sw - ch, (float)z+libge_context->img_stack[z+2048]);
		glTexCoord2f(sx+texMaxX, sy+texMaxY);glVertex3f(x + cw - sh, y + sw + ch, (float)z+libge_context->img_stack[z+2048]);
	glEnd();
	
	libge_context->img_stack[z+2048] += 0.001;
}

void geRenderFontOutline(int x, int y, ge_Font* font, u32 color, u32 outlineColor, const char* text, int len){
	int i;
	int b_x = x;
	y += font->size;
	int z = 0;
	
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, font->texture->id);
	if(!ge_current_shader){
		geShaderUse(_ge_GetVideoContext()->shader2d);
	}
	if(ge_current_shader == _ge_GetVideoContext()->shader2d){
		geShaderUniform1f(_ge_GetVideoContext()->loc_textured, 1.0);
	}

	glBegin(GL_QUADS);

	float rx = font->texture->u / font->texture->width;
	float ry = font->texture->v / font->texture->height;
		
	for(i=0; i<len; i++){
		if(text[i] == '\n'){
			x = b_x;
			y += font->size;
			continue;
		}

		float sx = ((float)font->positions[(u8)text[i]].x) * rx;
		float sy = ((float)font->positions[(u8)text[i]].y) * ry;
		float texMaxX = ((float)font->positions[(u8)text[i]].w) * rx;
		float texMaxY = ((float)font->positions[(u8)text[i]].h) * ry;
		float width = font->positions[(u8)text[i]].w;
		float height = font->positions[(u8)text[i]].h;
		float fy = (float)y - font->positions[(u8)text[i]].posY;

		if(A(outlineColor) != 0){
			float ofs = 0.06;
			float scale = 1.1;

			glColor4ub(R(outlineColor), G(outlineColor), B(outlineColor), A(outlineColor));
			glTexCoord2f(sx, sy);
			glVertex3f(x-width*ofs, fy-font->positions[(u8)text[i]].posY*ofs, z+libge_context->img_stack[z+2048]);

			glTexCoord2f(sx+texMaxX, sy);
			glVertex3f(x+width*scale, fy-font->positions[(u8)text[i]].posY*ofs, z+libge_context->img_stack[z+2048]);

			glTexCoord2f(sx+texMaxX, sy+texMaxY);
			glVertex3f(x+width*scale, fy+height*scale, z+libge_context->img_stack[z+2048]);

			glTexCoord2f(sx, sy+texMaxY);
			glVertex3f(x-width*ofs, fy+height*scale, z+libge_context->img_stack[z+2048]);
		}
		
		glColor4ub(R(color), G(color), B(color), A(color));

		glTexCoord2f(sx, sy);
		glVertex3f(x, fy, z+libge_context->img_stack[z+2048]);

		glTexCoord2f(sx+texMaxX, sy);
		glVertex3f(x+width, fy, z+libge_context->img_stack[z+2048]);

		glTexCoord2f(sx+texMaxX, sy+texMaxY);
		glVertex3f(x+width, fy+height, z+libge_context->img_stack[z+2048]);

		glTexCoord2f(sx, sy+texMaxY);
		glVertex3f(x, fy+height, z+libge_context->img_stack[z+2048]);

		x += font->positions[(u8)text[i]].advX;
	}
	glEnd();

	libge_context->img_stack[z+2048] += 0.001;
}

void geRenderFont(int x, int y, ge_Font* font, u32 color, const char* text, int len){
	geRenderFontOutline(x, y, font, color, 0x00000000, text, len);
}
/*
void geRenderFont(int x, int y, ge_Font* font, u32 color, const char* text, int len){
	int i;
	int b_x = x;
	y += font->size;
	int z = 0;
	
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, font->texture->id);
	if(!ge_current_shader){
		geShaderUse(_ge_GetVideoContext()->shader2d);
	}
	if(ge_current_shader == _ge_GetVideoContext()->shader2d){
		geShaderUniform1f(_ge_GetVideoContext()->loc_textured, 1.0);
	}

	glBegin(GL_QUADS);
	glColor4ub(R(color), G(color), B(color), A(color));

	float rx = font->texture->u / font->texture->width;
	float ry = font->texture->v / font->texture->height;
		
	for(i=0; i<len; i++){
		if(text[i] == '\n'){
			x = b_x;
			y += font->size;
			continue;
		}

		float sx = ((float)font->positions[(u8)text[i]].x) * rx;
		float sy = ((float)font->positions[(u8)text[i]].y) * ry;
		float texMaxX = ((float)font->positions[(u8)text[i]].w) * rx;
		float texMaxY = ((float)font->positions[(u8)text[i]].h) * ry;
		float width = font->positions[(u8)text[i]].w;
		float height = font->positions[(u8)text[i]].h;
		float fy = (float)y - font->positions[(u8)text[i]].posY;

		glTexCoord2f(sx, sy);
		glVertex3f(x, fy, z+libge_context->img_stack[z+2048]);

		glTexCoord2f(sx+texMaxX, sy);
		glVertex3f(x+width, fy, z+libge_context->img_stack[z+2048]);

		glTexCoord2f(sx+texMaxX, sy+texMaxY);
		glVertex3f(x+width, fy+height, z+libge_context->img_stack[z+2048]);

		glTexCoord2f(sx, sy+texMaxY);
		glVertex3f(x, fy+height, z+libge_context->img_stack[z+2048]);

		x += font->positions[(u8)text[i]].advX;
	}
	glEnd();

	libge_context->img_stack[z+2048] += 0.001;
}
*/
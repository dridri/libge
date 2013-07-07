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

static ge_Vertex* _ge_2d_vertices = NULL;
static u32 _ge_2d_vertices_vbo = -1;
static int _ge_2d_vertices_i = 0;
static int _ge_2d_vertices_draw_mode = GL_TRIANGLES;
#define GE_2D_VERTICES_MAX 128*6

static void InitDraw(ge_Image* tex, int mode){
	_ge_2d_vertices_draw_mode = mode;
	
	if(!ge_current_shader){
		geShaderUse(_ge_GetVideoContext()->shader2d);
	}
	if(ge_current_shader == _ge_GetVideoContext()->shader2d){
		geShaderUniform1f(_ge_GetVideoContext()->loc_textured, 1.0);
		glUniform1f(_ge_GetVideoContext()->shader2d->loc_time, ((float)geGetTick()) / 1000.0);
		glUniform1f(_ge_GetVideoContext()->shader2d->loc_ratio, ((float)libge_context->width) / ((float)libge_context->height));
	}

	geUpdateMatrix();
	if(tex && tex->id){
		glActiveTexture(GL_TEXTURE0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, tex->id);
		if(ge_current_shader == _ge_GetVideoContext()->shader2d){
			geShaderUniform1f(_ge_GetVideoContext()->loc_textured, 1.0);
		}
	}else{
		if(ge_current_shader == _ge_GetVideoContext()->shader2d){
			geShaderUniform1f(_ge_GetVideoContext()->loc_textured, 0.0);
		}
	}
	if(!_ge_2d_vertices){
		_ge_2d_vertices = (ge_Vertex*)geMalloc(sizeof(ge_Vertex)*GE_2D_VERTICES_MAX);
		glGenBuffers(1, &_ge_2d_vertices_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _ge_2d_vertices_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(ge_Vertex)*GE_2D_VERTICES_MAX, NULL, GL_STREAM_DRAW);
	}
	glBindBuffer(GL_ARRAY_BUFFER, _ge_2d_vertices_vbo);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(ge_Vertex), BUFFER_OFFSET(0));
	glVertexAttribPointer(1, 4, GL_FLOAT, false, sizeof(ge_Vertex), BUFFER_OFFSET(12));
	glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(ge_Vertex), BUFFER_OFFSET(28));
	glVertexAttribPointer(3, 3, GL_FLOAT, false, sizeof(ge_Vertex), BUFFER_OFFSET(40));
	_ge_2d_vertices_i = 0;
}

static ge_Vertex* ReserveVertices(int n){
	ge_Vertex* ret = NULL;
	if((_ge_2d_vertices_i+n) >= GE_2D_VERTICES_MAX){
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(ge_Vertex)*_ge_2d_vertices_i, _ge_2d_vertices);
		glDrawArrays(_ge_2d_vertices_draw_mode, 0, _ge_2d_vertices_i);
		_ge_2d_vertices_i = 0;
		ret = _ge_2d_vertices;
	}else{
		ret = &_ge_2d_vertices[_ge_2d_vertices_i];
		_ge_2d_vertices_i += n;
	}
	return ret;
}

float geGetTickFloat();
static float _t = 0.0;
static void TermDraw(){
	float t = geGetTickFloat() - 3000.0;
	t = (float)geGetTick();
	if(ge_current_shader == _ge_GetVideoContext()->shader2d){
		glUniform1f(_ge_GetVideoContext()->shader2d->loc_time, t);
//		glUniform1f(_ge_GetVideoContext()->shader2d->loc_time, _t);
	}
	_t += 0.01;
	if(_ge_2d_vertices_i > 0){
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(ge_Vertex)*_ge_2d_vertices_i, _ge_2d_vertices);
		glDrawArrays(_ge_2d_vertices_draw_mode, 0, _ge_2d_vertices_i);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

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

//	geShaderUse(NULL);
	if(!ge_current_shader){
		geShaderUse(_ge_GetVideoContext()->shader2d);
	}
	geShaderUniform1f(_ge_GetVideoContext()->loc_textured, 0.0);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	
	InitDraw(NULL, GL_LINES);
	ge_Vertex* vertex = ReserveVertices(2);
	vertex[0].u = vertex[0].v = vertex[0].w = -1.0;
	vertex[0].x = x0;
	vertex[0].y = y0;
	vertex[0].z = z0+libge_context->img_stack[z0+2048];
	SET_COLOR(vertex[0].color, color0);
	vertex[1].u = vertex[1].v = vertex[1].w = -1.0;
	vertex[1].x = x1;
	vertex[1].y = y1;
	vertex[1].z = z1+libge_context->img_stack[z1+2048];
	SET_COLOR(vertex[1].color, color1);
	TermDraw();
	/*
	glBegin(GL_LINES);
		glColor4ub(R(color0), G(color0), B(color0), A(color0));
		glTexCoord3f(-1.0, -1.0, -1.0);
		glVertex3f(x0, y0, z0+libge_context->img_stack[z0+2048]);
		glColor4ub(R(color1), G(color1), B(color1), A(color1));
		glVertex3f(x1, y1, z1+libge_context->img_stack[z1+2048]);
	glEnd();
	*/
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
	
//	geShaderUse(NULL);
	if(!ge_current_shader){
		geShaderUse(_ge_GetVideoContext()->shader2d);
	}
	geShaderUniform1f(_ge_GetVideoContext()->loc_textured, 0.0);
	glDisable(GL_TEXTURE_2D);
	/*
	glBegin(GL_QUADS);
		glColor4ub(R(color), G(color), B(color), A(color));
		glTexCoord3f(-1.0, -1.0, -1.0);
		glVertex3f(x, y, z+libge_context->img_stack[z+2048]);
		glVertex3f(x+width, y, z+libge_context->img_stack[z+2048]);
		glVertex3f(x+width, y+height, z+libge_context->img_stack[z+2048]);
		glVertex3f(x, y+height, z+libge_context->img_stack[z+2048]);
	glEnd();
	*/
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
	if((unsigned long)img==0xBAADF00D)return;
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

/*
	float texMaxX = img->u;
	float texMaxY = img->v;
	float sx = _sx*texMaxX/img->textureWidth;
	float sy = _sy*texMaxY/img->textureHeight;
	texMaxX = ex*texMaxX/img->textureWidth;
	texMaxY = ey*texMaxY/img->textureHeight;
*/
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
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, img->id);
	geShaderUniform1f(_ge_GetVideoContext()->loc_textured, 1.0);

	
	ge_Vertex vertices[6] = {
		{ sx, sy, 0.0, { Rf(img->color), Gf(img->color), Bf(img->color), Af(img->color) }, 0.0, 0.0, 0.0, x - cw + sh, y - sw - ch, z+libge_context->img_stack[z+2048], { 0.0 } },
		{ sx+texMaxX, sy, 0.0, { Rf(img->color), Gf(img->color), Bf(img->color), Af(img->color) }, 0.0, 0.0, 0.0, x + cw + sh, y + sw - ch, z+libge_context->img_stack[z+2048], { 0.0 } },
		{ sx+texMaxX, sy+texMaxY, 0.0, { Rf(img->color), Gf(img->color), Bf(img->color), Af(img->color) }, 0.0, 0.0, 0.0, x + cw - sh, y + sw + ch, z+libge_context->img_stack[z+2048], { 0.0 } },
		
		{ sx, sy, 0.0, { Rf(img->color), Gf(img->color), Bf(img->color), Af(img->color) }, 0.0, 0.0, 0.0, x - cw + sh, y - sw - ch, z+libge_context->img_stack[z+2048], { 0.0 } },
		{ sx+texMaxX, sy+texMaxY, 0.0, { Rf(img->color), Gf(img->color), Bf(img->color), Af(img->color) }, 0.0, 0.0, 0.0, x + cw - sh, y + sw + ch, z+libge_context->img_stack[z+2048], { 0.0 } },
		{ sx, sy+texMaxY, 0.0, { Rf(img->color), Gf(img->color), Bf(img->color), Af(img->color) }, 0.0, 0.0, 0.0, x - cw - sh, y - sw + ch, z+libge_context->img_stack[z+2048], { 0.0 } }
	};
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(ge_Vertex), &vertices[0].u);
	glVertexAttribPointer(1, 4, GL_FLOAT, false, sizeof(ge_Vertex), &vertices[0].color[0]); //3*4 => size(u,v,w)
	glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(ge_Vertex), &vertices[0].nx); //3*4 + 4*4 => size(u,v,w,color[4])
	glVertexAttribPointer(3, 3, GL_FLOAT, false, sizeof(ge_Vertex), &vertices[0].x); //3*4 + 4*4 + 3*4 => size(u,v,w,color[4],nx,ny,nz)
	glDrawArrays(GL_TRIANGLES, 0, 6);
	/*
	glBegin(GL_TRIANGLE_FAN);
		glColor4ub(R(img->color), G(img->color), B(img->color), A(img->color));
		glTexCoord2f(sx, sy+texMaxY);		 glVertex3f(x - cw - sh, y - sw + ch, (float)z+libge_context->img_stack[z+2048]);
		glTexCoord2f(sx, sy);				 glVertex3f(x - cw + sh, y - sw - ch, (float)z+libge_context->img_stack[z+2048]);
		glTexCoord2f(sx+texMaxX, sy);		 glVertex3f(x + cw + sh, y + sw - ch, (float)z+libge_context->img_stack[z+2048]);
		glTexCoord2f(sx+texMaxX, sy+texMaxY);glVertex3f(x + cw - sh, y + sw + ch, (float)z+libge_context->img_stack[z+2048]);
	glEnd();
	*/
	libge_context->img_stack[z+2048] += 0.001;
}


void geBlitImageDepthStretched(int x, int y, int z, ge_Image* img, int _sx, int _sy, int ex, int ey, int width, int height, int flags){
	if(!img)return;
	if((unsigned long)img==0xBAADF00D)return;
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
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	

	float texMaxX = img->u;
	float texMaxY = img->v;
	float sx = _sx*texMaxX/img->width;
	float sy = _sy*texMaxY/img->height;
	texMaxX = ex*texMaxX/img->width;
	texMaxY = ey*texMaxY/img->height;

	if(flags & GE_BLIT_VFLIP){
		texMaxY = -texMaxY;
	}

	InitDraw(img, GL_TRIANGLES);

	ge_Vertex* vertex = ReserveVertices(6);
	vertex[0].u = sx;
	vertex[0].v = sy;
	vertex[0].x = x;
	vertex[0].y = y;
	vertex[0].z = z     ;//+libge_context->img_stack[z+2048];

	vertex[1].u = sx+texMaxX;
	vertex[1].v = sy;
	vertex[1].x = x+width;
	vertex[1].y = y;
	vertex[1].z = z     ;//+libge_context->img_stack[z+2048];
	
	vertex[2].u = sx+texMaxX;
	vertex[2].v = sy+texMaxY;
	vertex[2].x = x+width;
	vertex[2].y = y+height;
	vertex[2].z = z     ;//+libge_context->img_stack[z+2048];
	
	vertex[3].u = sx;
	vertex[3].v = sy;
	vertex[3].x = x;
	vertex[3].y = y;
	vertex[3].z = z     ;//+libge_context->img_stack[z+2048];
	
	vertex[4].u = sx+texMaxX;
	vertex[4].v = sy+texMaxY;
	vertex[4].x = x+width;
	vertex[4].y = y+height;
	vertex[4].z = z     ;//+libge_context->img_stack[z+2048];
	
	vertex[5].u = sx;
	vertex[5].v = sy+texMaxY;
	vertex[5].x = x;
	vertex[5].y = y+height;
	vertex[5].z = z     ;//+libge_context->img_stack[z+2048];

	vertex[0].color[0] = vertex[1].color[0] = vertex[2].color[0] = vertex[3].color[0] = vertex[4].color[0] = vertex[5].color[0] = Rf(img->color);
	vertex[0].color[1] = vertex[1].color[1] = vertex[2].color[1] = vertex[3].color[1] = vertex[4].color[1] = vertex[5].color[1] = Gf(img->color);
	vertex[0].color[2] = vertex[1].color[2] = vertex[2].color[2] = vertex[3].color[2] = vertex[4].color[2] = vertex[5].color[2] = Bf(img->color);
	vertex[0].color[3] = vertex[1].color[3] = vertex[2].color[3] = vertex[3].color[3] = vertex[4].color[3] = vertex[5].color[3] = Af(img->color);

	TermDraw();

	libge_context->img_stack[z+2048] += 0.001;
}

void geRenderFont(int x, int y, ge_Font* font, u32 color, const char* text, int len){
	int i, j;
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
	geShaderUniform1f(_ge_GetVideoContext()->loc_textured, 1.0);


	float rx = font->texture->u / font->texture->width;
	float ry = font->texture->v / font->texture->height;
	
	ge_Vertex* vertex = NULL;
	InitDraw(font->texture, GL_TRIANGLES);

	for(i=0, j=0; i<len; i++){
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

		vertex = ReserveVertices(6);

		vertex[0].u = sx;
		vertex[0].v = sy;
		vertex[0].x = x;
		vertex[0].y = fy;
		vertex[0].z = z+libge_context->img_stack[z+2048];

		vertex[1].u = sx+texMaxX;
		vertex[1].v = sy;
		vertex[1].x = x+width;
		vertex[1].y = fy;
		vertex[1].z = z+libge_context->img_stack[z+2048];

		vertex[2].u = sx+texMaxX;
		vertex[2].v = sy+texMaxY;
		vertex[2].x = x+width;
		vertex[2].y = fy+height;
		vertex[2].z = z+libge_context->img_stack[z+2048];
		
		vertex[3].u = sx;
		vertex[3].v = sy;
		vertex[3].x = x;
		vertex[3].y = fy;
		vertex[3].z = z+libge_context->img_stack[z+2048];

		vertex[4].u = sx+texMaxX;
		vertex[4].v = sy+texMaxY;
		vertex[4].x = x+width;
		vertex[4].y = fy+height;
		vertex[4].z = z+libge_context->img_stack[z+2048];

		vertex[5].u = sx;
		vertex[5].v = sy+texMaxY;
		vertex[5].x = x;
		vertex[5].y = fy+height;
		vertex[5].z = z+libge_context->img_stack[z+2048];

		vertex[0].color[0] = vertex[1].color[0] = vertex[2].color[0] = vertex[3].color[0] = vertex[4].color[0] = vertex[5].color[0] = Rf(color);
		vertex[0].color[1] = vertex[1].color[1] = vertex[2].color[1] = vertex[3].color[1] = vertex[4].color[1] = vertex[5].color[1] = Gf(color);
		vertex[0].color[2] = vertex[1].color[2] = vertex[2].color[2] = vertex[3].color[2] = vertex[4].color[2] = vertex[5].color[2] = Bf(color);
		vertex[0].color[3] = vertex[1].color[3] = vertex[2].color[3] = vertex[3].color[3] = vertex[4].color[3] = vertex[5].color[3] = Af(color);

		x += font->positions[(u8)text[i]].advX;
	}

	libge_context->img_stack[z+2048] += 0.001;

	TermDraw();
}

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

typedef struct Vertex {
	u32 color;
	short x, y, z;
} Vertex;

typedef struct TextureVertex {
	short u, v;
	u32 color;
	short x, y, z;
} TextureVertex;

typedef struct Vertexf {
	u32 color;
	float x, y, z;
} Vertexf;

typedef struct TextureVertexf {
	float u, v;
	u32 color;
	float x, y, z;
} TextureVertexf;

static int Exp(int val){
	u32 i;
	asm("clz %0, %1\n" : "=r"(i):"r"(val&0x3FF));
	return 31-i;
}

extern void* ge_psp_texture_2d_bound;

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
	geSendCommandi(GE_TEXTURE_2D, false);

	Vertexf* vertices = (Vertexf*)geGetMemory(2 * sizeof(Vertexf));
	vertices[0].color = color0;
	vertices[0].x = x0;
	vertices[0].y = y0;
	vertices[0].z = z0;
	vertices[1].color = color1;
	vertices[1].x = x1;
	vertices[1].y = y1;
	vertices[1].z = z1;

	geDrawArrayFull(GE_LINES, GE_COLOR_8888|GE_VERTEX_32BITF|GE_TRANSFORM_3D, vertices, 2);
//	geDrawSync();
	geSendCommandi(GE_TEXTURE_2D, true);
}

void geFillRectScreenDepth(int x, int y, int z, int width, int height, u32 color){
	geSendCommandi(GE_TEXTURE_2D, false);
	Vertexf* vertices = (Vertexf*)geGetMemory(2 * sizeof(Vertexf));
	vertices[0].color = color;
	vertices[0].x = x;
	vertices[0].y = y;
	vertices[0].z = z;
	vertices[1].color = color;
	vertices[1].x = x + width; 
	vertices[1].y = y + height;
	vertices[1].z = z;
	geDrawArrayFull(GE_SPRITES, GE_COLOR_8888|GE_VERTEX_32BITF|GE_TRANSFORM_3D, vertices, 2);
//	geDrawSync();
	geSendCommandi(GE_TEXTURE_2D, true);
}

void geBlitImage(int x, int y, ge_Image* img, int sx, int sy, int width, int height, int flags){
	geBlitImageDepthStretched(x, y, 0, img, sx, sy, width, height, width, height, flags);
}

void geBlitImageStretched(int x, int y, ge_Image* img, int sx, int sy, int ex, int ey, int width, int height, int flags){
	if(!img)return;
	if((u32)img==0xBAADF00D)return;
	if(ex <= 0)return;
	if(ey <= 0)return;
	if(width <= 0)return;
	if(height <= 0)return;
	if((libge_context->drawing_mode & GE_DRAWING_2D_DEPTH)){
		geBlitImageDepthStretched(x, y, 0, img, sx, sy, ex, ey, width, height, flags);
		return;
	}
//	gePrintDebug(0x100, "geBlitImageStretched(%d, %d, 0x%08X, %d, %d, %d, %d, %d, %d, 0x%08X)\n", x, y, (u32)img, sx, sy, ex, ey, width, height, flags);

	x += libge_context->draw_off_x;
	y += libge_context->draw_off_y;
	if(flags & GE_BLIT_CENTERED){
		x -= width / 2;
		y -= height / 2;
	}

	ge_psp_texture_2d_bound = NULL;

	geSendCommandi(GE_ALPHA_TEST, !(flags & GE_BLIT_NOALPHA));
	geSendCommandi(GE_BLEND, !(flags & GE_BLIT_NOALPHA));
	geSendCommandi(CMD_TEX_MIPMAP_DRAW_MODE, (0 << 16) | (0 << 8) | (img->flags&GE_IMAGE_SWIZZLED));

	//TexImage
	geSendCommandi(CMD_TEX_MIPMAP0, ((u32)img->data) & 0xffffff);
	geSendCommandi(CMD_TEX_STRIDE0, ((((u32)img->data) >> 8) & 0x0f0000)|img->textureWidth);
//	geSendCommandi(CMD_TEX_SIZE0, (Exp(img->textureHeight) << 8) | (Exp(img->textureWidth)));
	geSendCommandi(CMD_TEX_SIZE0, (Exp(geGetNextPower2(img->height)) << 8) | (Exp(img->textureWidth)));

	//TexScale
	geSendCommandf(CMD_TEX_SCALE_U, 1.0f / ((float)img->textureWidth));
	geSendCommandf(CMD_TEX_SCALE_V, 1.0f / ((float)img->textureHeight));

	geSendCommandf(CMD_TEX_FLUSH, 0.0f);

	int j = 0;
	int sc = ex/width;
	while (j < width) {
		TextureVertex* vertices = (TextureVertex*)geGetMemory(2 * sizeof(TextureVertex));
		int sliceWidth = 64;
		if (j + sliceWidth > width) sliceWidth = width - j;
		vertices[0].u = sx + j*sc;
		vertices[0].v = sy;
		vertices[0].x = x + j;
		vertices[0].y = y;
		vertices[1].u = sx + (j+sliceWidth)*sc;
		vertices[1].v = sy + ey;
		vertices[1].x = x + j + sliceWidth;
		vertices[1].y = y + height;
		vertices[0].color = vertices[1].color = img->color;
		geSendCommandi(CMD_DRAW_MODE, GE_COLOR_8888 | GE_TEXTURE_16BIT | GE_VERTEX_16BIT | GE_TRANSFORM_2D);
		geSendCommandi(CMD_VERTICES_POINTER, ((unsigned int)vertices) & 0xffffff);
		geSendCommandi(CMD_DRAW_TYPE_VCOUNT, (GE_SPRITES << 16)|2);
		j += sliceWidth;
	}
}

void geBlitImageRotated(int x, int y, ge_Image* img, int sx, int sy, int ex, int ey, float angle, int flags){
	if(!img)return;
	if((u32)img==0xBAADF00D)return;
	if(!img->id)return;
	if(ex <= 0)return;
	if(ey <= 0)return;
	if((libge_context->drawing_mode & GE_DRAWING_2D_DEPTH)){
		geBlitImageDepthRotated(x, y, 0, img, sx, sy, ex, ey, angle, flags);
		return;
	}

	if(!(flags & GE_BLIT_NOOFFSET)){
		x += libge_context->draw_off_x;
		y += libge_context->draw_off_y;
	}

	ge_psp_texture_2d_bound = NULL;
	
	geSendCommandi(GE_ALPHA_TEST, !(flags & GE_BLIT_NOALPHA));
	geSendCommandi(GE_BLEND, !(flags & GE_BLIT_NOALPHA));
	geSendCommandi(CMD_TEX_MIPMAP_DRAW_MODE, (0 << 16) | (0 << 8) | (img->flags&GE_IMAGE_SWIZZLED));

	//TexImage
	geSendCommandi(CMD_TEX_MIPMAP0, ((u32)img->data) & 0xffffff);
	geSendCommandi(CMD_TEX_STRIDE0, ((((u32)img->data) >> 8) & 0x0f0000)|img->textureWidth);
//	geSendCommandi(CMD_TEX_SIZE0, (Exp(img->textureHeight) << 8) | (Exp(img->textureWidth)));
	geSendCommandi(CMD_TEX_SIZE0, (Exp(geGetNextPower2(img->height)) << 8) | (Exp(img->textureWidth)));

	//TexScale
	geSendCommandf(CMD_TEX_SCALE_U, 1.0f / ((float)img->textureWidth));
	geSendCommandf(CMD_TEX_SCALE_V, 1.0f / ((float)img->textureHeight));

	geSendCommandf(CMD_TEX_FLUSH, 0.0f);

	int mw = 0;
	int mh = 0;
	if(!(flags & GE_BLIT_CENTERED)){
		mw = (ex) / 2;
		mh = (ey) / 2;
	}

	float cos = geCos(angle);
	float sin = geSin(angle);
	int width = ex;
	int height = ey;
	width *= 0.5f;
	height *= 0.5f;

	float sw = sin*(float)width;
	float sh = sin*(float)height;
	float cw = cos*(float)width;
	float ch = cos*(float)height;

	x += mw;
	y += mh;

	TextureVertex* vertices = (TextureVertex*)geGetMemory(6 * sizeof(TextureVertex));

	vertices[0].u = sx;
	vertices[0].v = sy;
	vertices[0].x = (int)((float)x) - cw - sh;
	vertices[0].y = (int)((float)y) + sw - ch;
	vertices[0].z = 0;

	vertices[1].u = sx + ex;
	vertices[1].v = sy + ey;
	vertices[1].x = (int)((float)x) + cw + sh;
	vertices[1].y = (int)((float)y) - sw + ch;
	vertices[1].z = 0;

	vertices[2].u = sx + ex;
	vertices[2].v = sy;
	vertices[2].x = (int)((float)x) + cw - sh;
	vertices[2].y = (int)((float)y) - sw - ch;
	vertices[2].z = 0;


	vertices[3].u = sx;
	vertices[3].v = sy;
	vertices[3].x = (int)((float)x) - cw - sh;
	vertices[3].y = (int)((float)y) + sw - ch;
	vertices[3].z = 0;

	vertices[4].u = sx;
	vertices[4].v = sy + ey;
	vertices[4].x = (int)((float)x) - cw + sh;
	vertices[4].y = (int)((float)y) + sw + ch;
	vertices[4].z = 0;

	vertices[5].u = sx + ex;
	vertices[5].v = sy + ey;
	vertices[5].x = (int)((float)x) + cw + sh;
	vertices[5].y = (int)((float)y) - sw + ch;
	vertices[5].z = 0;

	vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = vertices[4].color = vertices[5].color = img->color;

	geSendCommandi(CMD_DRAW_MODE, GE_COLOR_8888 | GE_TEXTURE_16BIT | GE_VERTEX_16BIT | GE_TRANSFORM_2D);
	geSendCommandi(CMD_VERTICES_POINTER, ((unsigned int)vertices) & 0xffffff);
	geSendCommandi(CMD_DRAW_TYPE_VCOUNT, (GE_TRIANGLES << 16)|6);
}

void geBlitImageDepthRotated(int x, int y, int z, ge_Image* img, int sx, int sy, int ex, int ey, float angle, int flags){
	if(!img)return;
	if((u32)img==0xBAADF00D)return;
	if(!img->id)return;
	if(ex <= 0)return;
	if(ey <= 0)return;
	if(z > 2048 || z < -2048)return;
	if(!(libge_context->drawing_mode & GE_DRAWING_2D_DEPTH)){
		geBlitImageRotated(x, y, img, sx, sy, ex, ey, angle, flags);
		return;
	}

	if(!(flags & GE_BLIT_NOOFFSET)){
		x += libge_context->draw_off_x;
		y += libge_context->draw_off_y;
	}

	ge_psp_texture_2d_bound = NULL;
	
	geSendCommandi(GE_ALPHA_TEST, !(flags & GE_BLIT_NOALPHA));
	geSendCommandi(GE_BLEND, !(flags & GE_BLIT_NOALPHA));
	geSendCommandi(CMD_TEX_MIPMAP_DRAW_MODE, (0 << 16) | (0 << 8) | (img->flags&GE_IMAGE_SWIZZLED));

	//TexImage
	geSendCommandi(CMD_TEX_MIPMAP0, ((u32)img->data) & 0xffffff);
	geSendCommandi(CMD_TEX_STRIDE0, ((((u32)img->data) >> 8) & 0x0f0000)|img->textureWidth);
//	geSendCommandi(CMD_TEX_SIZE0, (Exp(img->textureHeight) << 8) | (Exp(img->textureWidth)));
	geSendCommandi(CMD_TEX_SIZE0, (Exp(geGetNextPower2(img->height)) << 8) | (Exp(img->textureWidth)));

	//TexScale
	geSendCommandf(CMD_TEX_SCALE_U, 1.0f / ((float)img->textureWidth));
	geSendCommandf(CMD_TEX_SCALE_V, 1.0f / ((float)img->textureHeight));

	geSendCommandf(CMD_TEX_FLUSH, 0.0f);

	int mw = 0;
	int mh = 0;
	if(!(flags & GE_BLIT_CENTERED)){
		mw = (ex) / 2;
		mh = (ey) / 2;
	}

	float cos = geCos(angle);
	float sin = geSin(angle);
	int width = ex;
	int height = ey;
	width *= 0.5f;
	height *= 0.5f;

	float sw = sin*(float)width;
	float sh = sin*(float)height;
	float cw = cos*(float)width;
	float ch = cos*(float)height;

	x += mw;
	y += mh;

	TextureVertexf* vertices = (TextureVertexf*)geGetMemory(6 * sizeof(TextureVertexf));

	vertices[0].u = sx;
	vertices[0].v = sy;
	vertices[0].x = (int)((float)x) - cw - sh;
	vertices[0].y = (int)((float)y) + sw - ch;
	vertices[0].z = z+libge_context->img_stack[z+2048];

	vertices[1].u = sx + ex;
	vertices[1].v = sy + ey;
	vertices[1].x = (int)((float)x) + cw + sh;
	vertices[1].y = (int)((float)y) - sw + ch;
	vertices[1].z = z+libge_context->img_stack[z+2048];

	vertices[2].u = sx + ex;
	vertices[2].v = sy;
	vertices[2].x = (int)((float)x) + cw - sh;
	vertices[2].y = (int)((float)y) - sw - ch;
	vertices[2].z = z+libge_context->img_stack[z+2048];


	vertices[3].u = sx;
	vertices[3].v = sy;
	vertices[3].x = (int)((float)x) - cw - sh;
	vertices[3].y = (int)((float)y) + sw - ch;
	vertices[3].z = z+libge_context->img_stack[z+2048];

	vertices[4].u = sx;
	vertices[4].v = sy + ey;
	vertices[4].x = (int)((float)x) - cw + sh;
	vertices[4].y = (int)((float)y) + sw + ch;
	vertices[4].z = z+libge_context->img_stack[z+2048];

	vertices[5].u = sx + ex;
	vertices[5].v = sy + ey;
	vertices[5].x = (int)((float)x) + cw + sh;
	vertices[5].y = (int)((float)y) - sw + ch;
	vertices[5].z = z+libge_context->img_stack[z+2048];

	vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = vertices[4].color = vertices[5].color = img->color;

	geSendCommandi(CMD_DRAW_MODE, GE_COLOR_8888 | GE_TEXTURE_32BITF | GE_VERTEX_32BITF | GE_TRANSFORM_3D);
	geSendCommandi(CMD_VERTICES_POINTER, ((unsigned int)vertices) & 0xffffff);
	geSendCommandi(CMD_DRAW_TYPE_VCOUNT, (GE_TRIANGLES << 16)|6);

	libge_context->img_stack[z+2048] += 0.001;
}

void geBlitImageDepthStretched(int x, int y, int z, ge_Image* img, int sx, int sy, int ex, int ey, int width, int height, int flags){
	if(!img)return;
	if((u32)img==0xBAADF00D)return;
	if(z > 2048 || z < -2048)return;
	if(ex <= 0)return;
	if(ey <= 0)return;
	if(width <= 0)return;
	if(height <= 0)return;
	if(!(libge_context->drawing_mode & GE_DRAWING_2D_DEPTH)){
		geBlitImageStretched(x, y, img, sx, sy, ex, ey, width, height, flags);
		return;
	}

	x += libge_context->draw_off_x;
	y += libge_context->draw_off_y;
	if(flags & GE_BLIT_CENTERED){
		x -= width / 2;
		y -= height / 2;
	}

	ge_psp_texture_2d_bound = NULL;
	
	sceKernelDcacheWritebackInvalidateAll();
	
	geSendCommandi(GE_ALPHA_TEST, !(flags & GE_BLIT_NOALPHA));
	geSendCommandi(GE_BLEND, !(flags & GE_BLIT_NOALPHA));

	geSendCommandi(CMD_TEX_MIPMAP_DRAW_MODE, (0 << 16) | (0 << 8) | (img->flags&GE_IMAGE_SWIZZLED));
	//TexImage
	geSendCommandi(CMD_TEX_MIPMAP0, ((u32)img->data) & 0xffffff);
	geSendCommandi(CMD_TEX_STRIDE0, ((((u32)img->data) >> 8) & 0x0f0000)|img->textureWidth);
//	geSendCommandi(CMD_TEX_SIZE0, (Exp(img->textureHeight) << 8) | (Exp(img->textureWidth)));
	geSendCommandi(CMD_TEX_SIZE0, (Exp(geGetNextPower2(img->height)) << 8) | (Exp(img->textureWidth)));
	//TexScale
	geSendCommandf(CMD_TEX_SCALE_U, 1.0f / ((float)img->textureWidth));
	geSendCommandf(CMD_TEX_SCALE_V, 1.0f / ((float)img->textureHeight));
	//Flush
	geSendCommandf(CMD_TEX_FLUSH, 0.0f);

	int j = 0;
	while (j < width) {
		TextureVertexf* vertices = (TextureVertexf*)geGetMemory(2 * sizeof(TextureVertexf));
		int sliceWidth = 64;
		if (j + sliceWidth > width) sliceWidth = width - j;
		vertices[0].u = sx + j;
		vertices[0].v = sy;
		vertices[0].x = x + j;
		vertices[0].y = y;
		vertices[1].u = sx + j + sliceWidth;
		vertices[1].v = sy + height;
		vertices[1].x = x + j + sliceWidth;
		vertices[1].y = y + height;
		vertices[0].z = z+libge_context->img_stack[z+2048];
		vertices[1].z = z+libge_context->img_stack[z+2048];
		vertices[0].color = vertices[1].color = img->color;
		geSendCommandi(CMD_DRAW_MODE, GE_COLOR_8888 | GE_TEXTURE_32BITF | GE_VERTEX_32BITF | GE_TRANSFORM_3D);
		geSendCommandi(CMD_VERTICES_POINTER, ((unsigned int)vertices) & 0xffffff);
		geSendCommandi(CMD_DRAW_TYPE_VCOUNT, (GE_SPRITES << 16)|2);
		j += sliceWidth;
	}

	libge_context->img_stack[z+2048] += 0.001;
}

void geRenderFont(int x, int y, ge_Font* font, u32 color, const char* text, int len){
	int i, j;
	int b_x = x;
	y += font->size;
	int z = 0;

	ge_psp_texture_2d_bound = NULL;
	
	sceKernelDcacheWritebackInvalidateAll();
	
	geSendCommandi(GE_ALPHA_TEST, true);
	geSendCommandi(GE_BLEND, true);
	
	geSendCommandi(CMD_TEX_MIPMAP_DRAW_MODE, (0 << 16) | (0 << 8) | (font->texture->flags&GE_IMAGE_SWIZZLED));
	//TexImage
	geSendCommandi(CMD_TEX_MIPMAP0, ((u32)font->texture->data) & 0xffffff);
	geSendCommandi(CMD_TEX_STRIDE0, ((((u32)font->texture->data) >> 8) & 0x0f0000)|font->texture->textureWidth);
//	geSendCommandi(CMD_TEX_SIZE0, (Exp(font->texture->textureHeight) << 8) | (Exp(font->texture->textureWidth)));
	geSendCommandi(CMD_TEX_SIZE0, (Exp(geGetNextPower2(font->texture->height)) << 8) | (Exp(font->texture->textureWidth)));
	//TexScale
	geSendCommandf(CMD_TEX_SCALE_U, 1.0f / ((float)font->texture->textureWidth));
	geSendCommandf(CMD_TEX_SCALE_V, 1.0f / ((float)font->texture->textureHeight));
	//Flush
	geSendCommandf(CMD_TEX_FLUSH, 0.0f);
		
	TextureVertexf* vertices = (TextureVertexf*)geGetMemory(2 * len * sizeof(TextureVertexf));
	for(i=0, j=0; i<len; i++){
		if(text[i] == '\n'){
			x = b_x;
			y += font->size;
			continue;
		}

		float sx = (float)font->positions[(u8)text[i]].x;
		float sy = (float)font->positions[(u8)text[i]].y;
		float texMaxX = (float)font->positions[(u8)text[i]].w;
		float texMaxY = (float)font->positions[(u8)text[i]].h;
		float width = font->positions[(u8)text[i]].w;
		float height = font->positions[(u8)text[i]].h;
		float fy = (float)y - font->positions[(u8)text[i]].posY;
		
		vertices[i*2 + 0].u = sx;
		vertices[i*2 + 0].v = sy;
		vertices[i*2 + 0].x = x;
		vertices[i*2 + 0].y = fy;
		vertices[i*2 + 0].z = z+libge_context->img_stack[z+2048];
		vertices[i*2 + 0].color = color;
		
		vertices[i*2 + 1].u = sx + texMaxX;
		vertices[i*2 + 1].v = sy + texMaxY;
		vertices[i*2 + 1].x = x + width;
		vertices[i*2 + 1].y = fy + height;
		vertices[i*2 + 1].z = z+libge_context->img_stack[z+2048];
		vertices[i*2 + 1].color = color;

		x += font->positions[(u8)text[i]].advX;
		j++;
	}
	geSendCommandi(CMD_DRAW_MODE, GE_COLOR_8888 | GE_TEXTURE_32BITF | GE_VERTEX_32BITF | GE_TRANSFORM_3D);
	geSendCommandi(CMD_VERTICES_POINTER, ((unsigned int)vertices) & 0xffffff);
	geSendCommandi(CMD_DRAW_TYPE_VCOUNT, (GE_SPRITES << 16) | (2 * j));

	libge_context->img_stack[z+2048] += 0.001;
}

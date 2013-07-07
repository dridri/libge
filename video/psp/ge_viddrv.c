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

#define GE_TEXTURE_MIN_FILTER 0xF001
#define GE_TEXTURE_MAG_FILTER 0xF002
#define GE_TEXTURE_WRAP_S 0xF003
#define GE_TEXTURE_WRAP_T 0xF004
#define GE_TEXTURE_SCALE_U 0xF005
#define GE_TEXTURE_SCALE_V 0xF006

static int n_ge_textures = 0;
static ge_Image** ge_textures = NULL;

typedef struct ge_psp_texture_unit {
	bool used;
	int min_filter;
	int mag_filter;
	int wrap_s;
	int wrap_t;
	float u, v;
	int nMipmaps;
	bool swizzled;
	int width[8];
	int height[8];
	int hwWidth[8];
	int hwHeight[8];
	u32* data[8];
} ge_psp_texture_unit;

ge_psp_texture_unit** ge_psp_textures = NULL;
int ge_psp_textures_count = 0;
ge_psp_texture_unit* ge_psp_texture_2d_bound = NULL;

void swizzle_fast(u8* out, ge_File* in, unsigned int width, unsigned int height);

static int Exp(int val){
	u32 i;
	asm("clz %0, %1\n" : "=r"(i):"r"(val&0x3FF));
	return 31-i;
}

static int min(int a, int b){
	return a < b ? a : b;
}

static int max(int a, int b){
	return a > b ? a : b;
}

static void ge_psp_GenTextures(int n, u32* ids){
	int i;
	ge_psp_textures = (ge_psp_texture_unit**)geRealloc(ge_psp_textures, sizeof(ge_psp_texture_unit*) * (ge_psp_textures_count + n));
	for(i=0; i<n; i++){
		ge_psp_textures[ge_psp_textures_count + i] = (ge_psp_texture_unit*)geMalloc(sizeof(ge_psp_texture_unit));
		ge_psp_textures[ge_psp_textures_count + i]->u = 1.0;
		ge_psp_textures[ge_psp_textures_count + i]->v = 1.0;
		ids[i] = (u32)ge_psp_textures[ge_psp_textures_count + i];
	}
	ge_psp_textures_count += n;
}

static void ge_psp_DeleteTextures(int n, u32* ids){
	int i, j;
	for(i=0; i<n; i++){
		if(ids[i]){
			ge_psp_texture_unit* tex = (ge_psp_texture_unit*)ids[i];
			for(j=0; j<tex->nMipmaps; j++){
				geFree(tex->data[j]);
			}
			geFree(tex);
		}
	}
}

static void ge_psp_BindTexture(int target, u32 id){
	int i = 0;
	if(target == GE_TEXTURE_2D){
		/*
		if(ge_psp_texture_2d_bound == (ge_psp_texture_unit*)id){
			return;
		}
		*/
		ge_psp_texture_2d_bound = (ge_psp_texture_unit*)id;
	}

//	gePrintDebug(0x100, "ge_psp_BindTexture(0x%08X)\n", id);
	if(id != 0){
		ge_psp_texture_unit* tex = (ge_psp_texture_unit*)id;
//		gePrintDebug(0x100, "  ge_psp_BindTexture:: %d * %d : 0x%08X\n", tex->hwWidth[0], tex->hwHeight[0], tex->data[0]);
	
		geSendCommandi(CMD_TEX_MIPMAP_DRAW_MODE, (tex->nMipmaps << 16) | (0 << 8) | tex->swizzled);

		for(i=0; i<=tex->nMipmaps; i++){
			geSendCommandi(CMD_TEX_MIPMAP0 + i, ((u32)tex->data[i]) & 0xffffff);
			geSendCommandi(CMD_TEX_STRIDE0 + i, ((((u32)tex->data[i]) >> 8) & 0x0f0000) | tex->hwWidth[i]);
			geSendCommandi(CMD_TEX_SIZE0 + i, (Exp(tex->hwHeight[i]) << 8) | Exp(tex->hwWidth[i]));
		}
	
		// Filters
		geSendCommandi(CMD_TEX_WRAP, ((tex->wrap_t & GE_IMAGE_CLAMPY) << 8) | (tex->wrap_s & GE_IMAGE_CLAMPX));
		geSendCommandi(CMD_TEX_FILTER, (tex->mag_filter << 8) | tex->min_filter);

		// TexScale
		geSendCommandf(CMD_TEX_SCALE_U, tex->u);
		geSendCommandf(CMD_TEX_SCALE_V, tex->v);

		// Flush
		geSendCommandf(CMD_TEX_FLUSH, 0.0f);
	}
}

static void ge_psp_TexImage2D(int target, int miplevel, bool swizzle, int width, int height, void* data){
	ge_psp_texture_unit* tex = NULL;
	if(target == GE_TEXTURE_2D){
		tex = ge_psp_texture_2d_bound;
	}
	if(!tex){
		return;
	}

	int hwWidth = geGetNextPower2(width);
	int hwHeight = geGetNextPower2(height);

	if(data && swizzle){
		int size = hwWidth * height * 4;
		ge_File* in_file = NULL;
		if(!libge_context->ram_extended){
			in_file = geFileBuffer(data, size, GE_FILE_MODE_READWRITE);
		}else{
			in_file = geFileOpen("ms0:/libge_temp_file", GE_FILE_MODE_CLEAR_RW);
			geFileWrite(in_file, data, size);
			geFileRewind(in_file);
		}
		swizzle_fast((u8*)data, in_file, hwWidth * 4, height);
		geFileClose(in_file);
		tex->swizzled = true;
	}

	tex->nMipmaps = max(miplevel, tex->nMipmaps);
	tex->width[miplevel] = width;
	tex->height[miplevel] = height;
	tex->hwWidth[miplevel] = hwWidth;
	tex->hwHeight[miplevel] = hwHeight;
	tex->data[miplevel] = (u32*)data;
}

static void ge_psp_TexParameteri(int target, int param, int val){
	ge_psp_texture_unit* tex = NULL;
	if(target == GE_TEXTURE_2D){
		tex = ge_psp_texture_2d_bound;
	}
	if(!tex){
		return;
	}

	switch(param){
		case GE_TEXTURE_MIN_FILTER :
			tex->min_filter = val;
			break;
		case GE_TEXTURE_MAG_FILTER :
			tex->mag_filter = val;
			break;
		case GE_TEXTURE_WRAP_S :
			tex->wrap_s = val;
			break;
		case GE_TEXTURE_WRAP_T :
			tex->wrap_t = val;
			break;
		default :
			break;
	}
}

static void ge_psp_TexParameterf(int target, int param, float val){
	ge_psp_texture_unit* tex = NULL;
	if(target == GE_TEXTURE_2D){
		tex = ge_psp_texture_2d_bound;
	}
	if(!tex){
		return;
	}

	switch(param){
		case GE_TEXTURE_SCALE_U :
			tex->u = val;
			break;
		case GE_TEXTURE_SCALE_V :
			tex->v = val;
			break;
		default :
			break;
	}
}

static u32 takePixelAt(const u32* data, float _x, float _y, float _w, float _h, float coeff){
	_x *= coeff;
	_y *= coeff;
	_h *= coeff;
	_w *= coeff;
	int x = (int)_x;
	int y = (int)_y;
	int w = (int)_w;
	int h = (int)_h;
	if(x <= 0){
		x = 0;
	}
	if(y <= 0){
		y = 0;
	}
	if(x >= w){
		x -= (x-w)+1;
	}
	if(y >= h){
		y -= (y-h)+1;
	}
	return data[x + y*w];
}

static u32 mixColors(u32 c1, u32 c2){
	u8 R1=R(c1);	u8 G1=G(c1);	u8 B1=B(c1);	u8 A1=A(c1);
	u8 R2=R(c2);	u8 G2=G(c2);	u8 B2=B(c2);	u8 A2=A(c2);
	u8 r = (R1 + R2) /2;
	u8 g = (G1 + G2) /2;
	u8 b = (B1 + B2) /2;
	u8 a = (A1 + A2) /2;
	r = sqrtf((R1*R1 + R2*R2)/2);
	g = sqrtf((G1*G1 + G2*G2)/2);
	b = sqrtf((B1*B1 + B2*B2)/2);
	a = sqrtf((A1*A1 + A2*A2)/2);
	return RGBA(r, g, b, a);
}

static void MakeSubImage(u32* dest, u32* from, int fw, int fh, int algo, float scale){
	int x, y;
	int w = fw/scale;
	int h = fh/scale;
	for(y=0; y<h; y++){
		for(x=0; x<w; x++){
			if(algo == 10){
				dest[x+y*w] = mixColors(takePixelAt(from, x, y, w, h, scale), takePixelAt(from, x+1, y+1, w, h, scale));
				dest[x+y*w] = mixColors(dest[x+y*w], takePixelAt(from, x-1, y-1, w, h, scale));
			}
			if(algo == 2){
				dest[x+y*w] = mixColors(takePixelAt(from, x, y, w, h, scale), takePixelAt(from, x+1, y+1, w, h, scale));
				dest[x+y*w] = mixColors(dest[x+y*w], takePixelAt(from, x-1, y-1, w, h, scale));
			}
			if(algo == 1 || algo == 11){
				u32 lr = mixColors(takePixelAt(from, x-1.0, y, w, h, scale), takePixelAt(from, x+1.0, y, w, h, scale));
				u32 tb = mixColors(takePixelAt(from, x, y-1.0, w, h, scale), takePixelAt(from, x, y+1.0, w, h, scale));
				u32 tl_br = mixColors(takePixelAt(from, x-1.0, y-1.0, w, h, scale), takePixelAt(from, x+1.0, y+1.0, w, h, scale));
				u32 tr_bl = mixColors(takePixelAt(from, x+1.0, y-1.0, w, h, scale), takePixelAt(from, x-1.0, y+1.0, w, h, scale));
				u32 c1 = mixColors(lr, tb);
				u32 c2 = mixColors(tl_br, tr_bl);
				dest[x+y*w] = mixColors(takePixelAt(from, x, y, w, h, scale), c1);
				dest[x+y*w] = mixColors(dest[x+y*w], c2);
			//	dest[x+y*w] = multColor(dest[x+y*w], 1.0);
			}
		}
	}
}

void PassMipmap(ge_Image* image, int max, int algo){
	if(!max)return;
	gePrintDebug(0x100, "Making mipmaps at %d level\n", max);
	int i = 0;
	int w = image->textureWidth;
	int h = image->textureHeight;
	u32* mipmaps[8];
	mipmaps[0] = image->data;
	
	for(i=0; i<max; i++){
		w /= 2;
		h /= 2;
		if(w == 0 || h == 0){
			break;
		}
		u32* mipmap = (u32*)geMalloc(sizeof(u32) * geGetNextPower2(w) * h);
		mipmaps[i+1] = mipmap;
		if(algo == 10 || algo == 11){
			MakeSubImage(mipmaps[i+1], mipmaps[0], image->textureWidth, image->textureHeight, algo, (float)image->textureWidth/(float)w);
			/*
			int x, y;
			for(y=0; y<h; y++){
				for(x=0; x<w; x++){
					if(i == 0){
						mipmaps[i+1][y*w + x] = RGBA(255, 0, 0, 255); //RED
					}
					if(i == 1){
						mipmaps[i+1][y*w + x] = RGBA(255, 255, 0, 255); //YELLOW
					}
					if(i == 2){
						mipmaps[i+1][y*w + x] = RGBA(0, 255, 0, 255); //GREEN
					}
					if(i == 3){
						mipmaps[i+1][y*w + x] = RGBA(0, 255, 255, 255); //CYAN
					}
					if(i == 4){
						mipmaps[i+1][y*w + x] = RGBA(0, 0, 255, 255); //BLUE
					}
				}
			}
			*/
		}else{
			MakeSubImage(mipmaps[i+1], mipmaps[i], w*2, h*2, algo, 2.0);
		}
		ge_psp_TexImage2D(GE_TEXTURE_2D, i + 1, true, w, h, mipmap);
		libge_context->gpumem += w*h*sizeof(u32);
	}
}

void geUpdateImage(ge_Image* image){
	gePrintDebug(0x100, "geUpdateImage(0x%08X)\n", image);
	if(!image)return;
	gePrintDebug(0x100, "geUpdateImage 1\n");

	int mipmap_detail = max(0, (Exp(min(image->textureWidth, image->textureHeight)) * !(image->flags & GE_IMAGE_NO_MIPMAPS)) >> 1);
	//mipmap_detail = 0;
	gePrintDebug(0x100, "geUpdateImage 2\n");

	image->u = (float)image->width / (float)image->textureWidth;
	image->v = (float)image->height / (float)image->textureHeight;
	gePrintDebug(0x100, "geUpdateImage 3\n");

	if(!image->id){
		ge_psp_GenTextures(1, &image->id);
		gePrintDebug(0x100, "Generated id : 0x%08X\n", image->id);
	}
	gePrintDebug(0x100, "geUpdateImage 4\n");
	
	ge_psp_BindTexture(GE_TEXTURE_2D, image->id);

	gePrintDebug(0x100, "Texture %X (%d x %d | %X) mip detail : %d\n", image->id, image->width, image->height, image->flags, mipmap_detail);
	if(mipmap_detail){
		if(image->flags & GE_IMAGE_BUMP){
			// Unsupported
		}else{
			PassMipmap(image, mipmap_detail, 11);
		}
		ge_psp_TexParameteri(GE_TEXTURE_2D, GE_TEXTURE_MIN_FILTER, GE_LINEAR_MIPMAP_LINEAR);
		ge_psp_TexParameteri(GE_TEXTURE_2D, GE_TEXTURE_MAG_FILTER, GE_LINEAR_MIPMAP_LINEAR);
	}else{
		ge_psp_TexParameteri(GE_TEXTURE_2D, GE_TEXTURE_MIN_FILTER, GE_LINEAR);
		ge_psp_TexParameteri(GE_TEXTURE_2D, GE_TEXTURE_MAG_FILTER, GE_LINEAR);
	}
	ge_psp_TexParameteri(GE_TEXTURE_2D, GE_TEXTURE_WRAP_S, GE_REPEAT);
    ge_psp_TexParameteri(GE_TEXTURE_2D, GE_TEXTURE_WRAP_T, GE_REPEAT);
	
	ge_psp_TexImage2D(GE_TEXTURE_2D, 0, true, image->textureWidth, image->textureHeight, image->data);
	image->flags |= GE_IMAGE_SWIZZLED;
	ge_psp_BindTexture(GE_TEXTURE_2D, 0);
}

void geDeleteImage(ge_Image* image){
	ge_psp_DeleteTextures(1, &image->id);
}

void geTextureImage(int unit, ge_Image* img){
//	gePrintDebug(0x100, "geTextureImage(%d, 0x%08X) [0x%08X]\n", unit, img, img ? img->id : NULL);
	if(unit > 0 || !img){
		geDisable(GE_TEXTURE_2D);
		return;
	}
	geEnable(GE_TEXTURE_2D);
	ge_psp_BindTexture(GE_TEXTURE_2D, img->id);
}

void geTextureWrap(ge_Image* tex, int wrapx, int wrapy){
	tex->flags = (tex->flags | (GE_IMAGE_CLAMPX * wrapx)) & ~(GE_IMAGE_CLAMPX * !wrapx);
	tex->flags = (tex->flags | (GE_IMAGE_CLAMPY * wrapy)) & ~(GE_IMAGE_CLAMPY * !wrapy);
}

void geTextureMode(ge_Image* img, int mode){
	ge_psp_BindTexture(GE_TEXTURE_2D, img->id);
	ge_psp_TexParameteri(GE_TEXTURE_2D, GE_TEXTURE_MIN_FILTER, mode);
	ge_psp_TexParameteri(GE_TEXTURE_2D, GE_TEXTURE_MAG_FILTER, mode);
}

void geTextureScale(ge_Image* img, float u, float v){
	if(!img || !img->id){
		return;
	}
	ge_psp_BindTexture(GE_TEXTURE_2D, img->id);
	ge_psp_TexParameterf(GE_TEXTURE_2D, GE_TEXTURE_SCALE_U, u);
	ge_psp_TexParameterf(GE_TEXTURE_2D, GE_TEXTURE_SCALE_V, v);
}

void geTextureCopyBuffer(ge_Image* target, int ofsx, int ofsy, int sx, int sy, int w, int h){
	LibGE_PspContext* context = (LibGE_PspContext*)libge_context->syscontext;

	geSendCommandi(CMD_TEX_COPY_SRC, ((unsigned int)context->draw_buffer.frame_buffer) & 0xffffff);
	geSendCommandi(CMD_TEX_COPY_SRC_STRIDE, ((((unsigned int)context->draw_buffer.frame_buffer) & 0xff000000) >> 8) | context->draw_buffer.frame_width);
	geSendCommandi(CMD_TRANSFER_SRC_OFFSET, (sy << 10) | sx);
	geSendCommandi(CMD_TEX_COPY_DST, ((unsigned int)target->data) & 0xffffff);
	geSendCommandi(CMD_TEX_COPY_DST_STRIDE, ((((unsigned int)target->data) & 0xff000000) >> 8) | target->textureWidth);
	geSendCommandi(CMD_TRANSFER_DST_OFFSET, (ofsx << 10) | ofsy);
	geSendCommandi(CMD_TRANSFER_SIZE, ((h-1) << 10) | (w-1));
	//geSendCommandi(234, (psm ^ 0x03) ? 0 : 1);
	geSendCommandi(CMD_TRANSFER_KICK, 1);
}

void geAllocateSurface(ge_Image* image, int width, int height){
	image->width = width;
	image->height = height;
	image->textureWidth = geGetNextPower2(width);
	//image->textureHeight = geGetNextPower2(height);
	image->textureHeight = height;
	if(height != geGetNextPower2(height)){
		image->textureHeight = 16 * ((image->textureHeight / 16) + 1);
	}

	image->data = (u32*)geMalloc(image->textureWidth * image->textureHeight * sizeof(u32));
}

ge_Image* geAddTexture(ge_Image* tex){
	gePrintDebug(0x100, "geAddTexture(0x%08X)...", (u32)tex);
	//Compare
	ge_Image* ret = tex;
	bool already_exists = false;
	int i = 0;
	for(i=0; i<n_ge_textures; i++){
		if(!ge_textures[i])continue;
		if( !memcmp(&tex->width, &ge_textures[i]->width, sizeof(int)*4) ){  //sizeof(int)*4 for size and 2^n size
			int sz = sizeof(u32) * min(tex->textureWidth*tex->textureHeight, ge_textures[i]->textureWidth*ge_textures[i]->textureHeight);
			if( !memcmp(tex->data, ge_textures[i]->data, sz) ){
				//It's the same
				already_exists = true;
				break;
			}
		}
	}
	if(!already_exists){
		n_ge_textures++;
		ge_textures = (ge_Image**)geRealloc(ge_textures, sizeof(ge_Image)*n_ge_textures);
		ge_textures[n_ge_textures-1] = tex;
	}else{
		ret = ge_textures[i];
		geFreeImage(tex);
	}
	gePrintDebug(0x100, "Ok\n");
	return ret;
}

void geFramebufferUse(ge_Framebuffer* fbo){
}

void swizzle_fast(u8* out, ge_File* in, unsigned int width, unsigned int height){
	unsigned int blockx, blocky;
	unsigned int j;
 
	unsigned int width_blocks = (width / 16);
	unsigned int height_blocks = (height / 8);
 
	unsigned int src_pitch = (width-16)/4;
	unsigned int src_row = width * 8;
 
	u8* _ysrc = (u8*)geMalloc(src_row);
	u8* ysrc = _ysrc;
	u32* dst = (u32*)out;
 
	for(blocky = 0; blocky < height_blocks; ++blocky){
		geFileRead(in, ysrc, src_row);
		const u8* xsrc = ysrc;
		for(blockx = 0; blockx < width_blocks; ++blockx){
			const u32* src = (u32*)xsrc;
			for(j = 0; j < 8; ++j){
				*(dst++) = *(src++);
				*(dst++) = *(src++);
				*(dst++) = *(src++);
				*(dst++) = *(src++);
				src += src_pitch;
			}
			xsrc += 16;
		}
	}
	geFree(_ysrc);
}

void geSwizzle(ge_Image* img){
//	if(!(img->flags & GE_IMAGE_SWIZZLED)){
		int size = img->textureWidth * img->textureHeight * 4;
		ge_File* in_file = NULL;

		if(!libge_context->ram_extended){
			in_file = geFileBuffer(img->data, size, GE_FILE_MODE_READWRITE);
		}else{
			in_file = geFileOpen("ms0:/libge_temp_file", GE_FILE_MODE_CLEAR_RW);
			geFileWrite(in_file, img->data, size);
			geFileRewind(in_file);
		}

		swizzle_fast((u8*)img->data, in_file, img->textureWidth * 4, img->textureHeight);

		img->flags |= GE_IMAGE_SWIZZLED;
		geFileClose(in_file);
//	}
}

void unswizzle_fast(u8* out, ge_File* in, unsigned int width, unsigned int height){
	unsigned int blockx, blocky;
	unsigned int j;
 
	unsigned int width_blocks = (width / 16);
	unsigned int height_blocks = (height / 8);
 
	unsigned int src_pitch = (width-16)/4;
	unsigned int src_row = width * 8;
 
	u8* ydst = out;
	int read_size = sizeof(u32) * 4 * 8 * width_blocks;
	u32* _src = (u32*)geMalloc(read_size);
 
	for(blocky = 0; blocky < height_blocks; ++blocky){
		u8* xdst = ydst;
		u32* src = _src;
		geFileRead(in, src, read_size);
		for(blockx = 0; blockx < width_blocks; ++blockx){
			u32* dst = (u32*)xdst;
			for(j = 0; j < 8; ++j){
				*(dst++) = *(src++);
				*(dst++) = *(src++);
				*(dst++) = *(src++);
				*(dst++) = *(src++);
				dst += src_pitch;
			}
			xdst += 16;
	  }
	  ydst += src_row;
	}
	geFree(_src);
}

void geUnswizzle(ge_Image* img){
	if(img->flags & GE_IMAGE_SWIZZLED){
		int size = img->textureWidth * img->textureHeight * 4;
		ge_File* in_file = NULL;

		if(!libge_context->ram_extended){
			in_file = geFileBuffer(img->data, size, GE_FILE_MODE_READWRITE);
		}else{
			in_file = geFileOpen("ms0:/libge_temp_file", GE_FILE_MODE_CLEAR_RW);
			geFileWrite(in_file, img->data, size);
			geFileRewind(in_file);
		}

		unswizzle_fast((u8*)img->data, in_file, (img->textureWidth * 4), img->textureHeight);

		img->flags &= ~GE_IMAGE_SWIZZLED;
		geFileClose(in_file);
	}
}

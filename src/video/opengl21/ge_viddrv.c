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
#include <math.h>

static int n_ge_textures = 0;
static ge_Image** ge_textures = { NULL };

#define load_func(name) \
	name = geglImportFunction(#name); \
	gePrintDebug(0x100, #name ": 0x%08lX\n", (t_ptr)name)

#define load_func2(name, str) \
	name = geglImportFunction(str); \
	gePrintDebug(0x100, #name ": 0x%08lX\n", (t_ptr)name)

void geInitVideo(){
/*
	load_func(glClear);
	load_func(glClearColor);
	load_func(glGetError);
	load_func(glEnable);
	load_func(glDisable);
	load_func(glViewport);
	load_func(glScissor);
	load_func(glFrontFace);
	load_func(glCullFace);
	load_func(glPolygonMode);
	load_func(glLineWidth);

	load_func(glDepthFunc);
	load_func(glDepthMask);
	load_func(glDepthRange);
	load_func(glClearDepth);
	load_func(glColorMask);

	load_func(glGenTextures);
	load_func(glDeleteTextures);
	load_func(glBindTexture);
	load_func(glTexImage2D);
	load_func(glTexSubImage2D);
	load_func(glCopyTexSubImage2D);
	load_func(glTexImage3D);
	load_func(glTexSubImage3D);
	load_func(glTexParameteri);
	load_func(glTexParameterf);

	load_func(glDrawArrays);
	load_func(glDrawElements);

	load_func(glBlendFunc);
	load_func(glBlendEquation);
	load_func(glBlendColor);
*/
#ifndef GL_GLEXT_PROTOTYPES
#ifdef WIN32
	load_func(glBlendEquation);
	load_func(glTexImage3D);
	load_func(glTexSubImage3D);
	load_func(glTexImage2DMultisample);
#endif
	load_func(glActiveTexture);

	load_func(glGenBuffers);
	load_func(glDeleteBuffers);
	load_func(glBindBuffer);
	load_func(glBufferData);
	load_func(glBufferSubData);
	load_func(glGetBufferParameteriv);

	load_func(glGenFramebuffers);
	load_func(glDeleteFramebuffers);
	load_func(glBindFramebuffer);
	load_func(glFramebufferTexture);
	load_func(glFramebufferTexture2D);
	load_func(glFramebufferTexture3D);
	load_func(glFramebufferTextureLayer);
	load_func(glDrawBuffers);
	load_func(glBlitFramebuffer);

	load_func(glGenRenderbuffers);
	load_func(glBindRenderbuffer);
	load_func(glRenderbufferStorageMultisample);
	load_func(glFramebufferRenderbuffer);

	load_func(glPatchParameteri);
#endif
}

void InitMipmaps(ge_Image* image, int max){
	if(!max)return;
	int i = 0;
	int w=image->textureWidth, h=image->textureHeight;

	for(i=0; i<max; i++){
		w /= 2;
		h /= 2;
		glTexImage2D(GL_TEXTURE_2D, i+1, GL_RGBA/*4*/, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		libge_context->gpumem += w*h*sizeof(u32);
	}
}

u32 takePixelAt(const u32* data, float _x, float _y, float _w, float _h, float coeff){
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

u32 multColor(u32 c, float f){
	float r = (float)R(c)*f;
	float g = (float)G(c)*f;
	float b = (float)B(c)*f;
	float a = (float)A(c)*f;
	if(r>255.0)r=255.0;
	if(g>255.0)g=255.0;
	if(b>255.0)b=255.0;
	if(a>255.0)a=255.0;
	return RGBA((u8)r, (u8)g, (u8)b, (u8)a);
}

u32 mixColors(u32 c1, u32 c2){
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

u32 mixColors3(u32 c1, u32 c2, u32 c3){
	u8 R1=R(c1);	u8 G1=G(c1);	u8 B1=B(c1);	u8 A1=A(c1);
	u8 R2=R(c2);	u8 G2=G(c2);	u8 B2=B(c2);	u8 A2=A(c2);
	u8 R3=R(c3);	u8 G3=G(c3);	u8 B3=B(c3);	u8 A3=A(c3);
	u8 r = (R1 + R2 + R3) /3;
	u8 g = (G1 + G2 + G3) /3;
	u8 b = (B1 + B2 + B3) /3;
	u8 a = (A1 + A2 + A3) /3;
	return RGBA(r, g, b, a);
}

u32 mixColors4(u32 c1, u32 c2, u32 c3, u32 c4){
	u8 R1=R(c1);	u8 G1=G(c1);	u8 B1=B(c1);	u8 A1=A(c1);
	u8 R2=R(c2);	u8 G2=G(c2);	u8 B2=B(c2);	u8 A2=A(c2);
	u8 R3=R(c3);	u8 G3=G(c3);	u8 B3=B(c3);	u8 A3=A(c3);
	u8 R4=R(c4);	u8 G4=G(c4);	u8 B4=B(c4);	u8 A4=A(c4);
	u8 r = (R1 + R2 + R3 + R4) /4;
	u8 g = (G1 + G2 + G3 + G4) /4;
	u8 b = (B1 + B2 + B3 + B4) /4;
	u8 a = (A1 + A2 + A3 + A4) /4;
	return RGBA(r, g, b, a);
}

void MakeSubImage(u32* dest, u32* from, int fw, int fh, int algo, float scale){
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
			if(algo == 1){
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
	int w=image->textureWidth, h=image->textureHeight;
//	u32* mipmap = (u32*)geMalloc(sizeof(u32) * w * h);
//	memcpy(mipmap, image->data, sizeof(u32)*w*h);
	u32* mipmaps[16];
	mipmaps[0] = image->data;

	
	for(i=0; i<max; i++){
		w /= 2;
		h /= 2;
		u32* mipmap = (u32*)geMalloc(sizeof(u32) * image->textureWidth * image->textureHeight);
		mipmaps[i+1] = mipmap;
		if(algo == 10){
			MakeSubImage(mipmaps[i+1], mipmaps[0], image->textureWidth, image->textureHeight, algo, (float)image->textureWidth/(float)w);
		}else{
			MakeSubImage(mipmaps[i+1], mipmaps[i], w*2, h*2, algo, 2.0);
		}
		glTexSubImage2D(GL_TEXTURE_2D, i+1, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, mipmap);
		libge_context->gpumem += w*h*sizeof(u32);
		if(i>0)geFree(mipmaps[i]);
	}
	geFree(mipmaps[i]);
}

void UpdateGlTexture(ge_Image* image, int mipmapping){
	if(!image)return;

//	int mipmap_detail = image->textureWidth / 256;
	int mipmap_detail = 8;
	if(!mipmapping || image->flags&GE_IMAGE_NO_MIPMAPS || image->textureWidth<512){
		mipmap_detail = 0;
	}

	int tex_mode = GL_TEXTURE_2D;
	image->u = (float)image->width / (float)image->textureWidth;
	image->v = (float)image->height / (float)image->textureHeight;

	if(!image->id){
		glGenTextures(1, &image->id);
		glBindTexture(tex_mode, image->id);
		glTexImage2D(tex_mode, 0, GL_RGBA/*4*/, image->textureWidth, image->textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		libge_context->gpumem += image->textureWidth*image->textureHeight*sizeof(u32);
	//	InitMipmaps(image, mipmap_detail);
	}
	
	glBindTexture(tex_mode, image->id);
	glTexSubImage2D(tex_mode, 0, 0, 0, image->textureWidth, image->textureHeight, GL_RGBA, GL_UNSIGNED_BYTE, image->data);

	if(mipmap_detail){
		if(image->flags & GE_IMAGE_BUMP){
			PassMipmap(image, mipmap_detail, 1);
		}else{
			PassMipmap(image, mipmap_detail, 1);
		}
		glTexParameterf(tex_mode, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(tex_mode, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(tex_mode, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(tex_mode, GL_TEXTURE_MAX_LEVEL, mipmap_detail-1);
		glTexParameterf(tex_mode, GL_TEXTURE_MIN_LOD, -2.0);
		glTexParameterf(tex_mode, GL_TEXTURE_MAX_LOD, mipmap_detail-1);
	}else{
		glTexParameterf(tex_mode, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(tex_mode, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	glTexParameterf(tex_mode, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(tex_mode, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(tex_mode, 0);
}

void UpdateGlTexture3D(ge_Image3D* image){
	if(!image)return;
	
	printf("UpdateGlTexture3D 1\n");
	image->u = (float)image->width / (float)image->textureWidth;
	image->v = (float)image->height / (float)image->textureHeight;
	printf("UpdateGlTexture3D 2\n");

	if(!image->id){
		glGenTextures(1, &image->id);
		glBindTexture(GL_TEXTURE_3D, image->id);
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA/*4*/, image->textureWidth, image->textureHeight, image->textureDepth, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		libge_context->gpumem += image->textureWidth*image->textureHeight*sizeof(u32);
	//	InitMipmaps(image, mipmap_detail);
	}
	
	printf("UpdateGlTexture3D 3\n");
	glBindTexture(GL_TEXTURE_3D, image->id);
	glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, image->textureWidth, image->textureHeight, image->textureDepth, GL_RGBA, GL_UNSIGNED_BYTE, image->data);
	printf("UpdateGlTexture3D 4\n");

	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_3D, 0);
	printf("UpdateGlTexture3D 5\n");
}

#ifndef max
static int max(int a, int b){
	if(a > b){
		return a;
	}
	return b;
}
#endif

void geAllocateSurface(ge_Image* image, int width, int height){
	image->width = width;
	image->height = height;
	int p2w = geGetNextPower2(width);
	int p2h = geGetNextPower2(height);
	bool ok = false;

	if(width != height){
		if(geCheckExtensionAvailable("GL_ARB_texture_rectangle")){
			if((p2w != width || p2h != height) && geCheckExtensionAvailable("GL_ARB_texture_non_power_of_two")){
				gePrintDebug(0x100, "Allocating rectangle NPOT texture\n");
				image->flags |= GE_IMAGE_RECTANGLE;
				image->textureWidth = width;
				image->textureHeight = height;
				ok = true;
			}else if(p2w == width || p2h == height){
				gePrintDebug(0x100, "Allocating rectangle texture\n");
				image->flags |= GE_IMAGE_RECTANGLE;
				image->textureWidth = width;
				image->textureHeight = height;
				ok = true;
			}
		}
	}else{
		if(p2w != width && geCheckExtensionAvailable("GL_ARB_texture_non_power_of_two")){
			gePrintDebug(0x100, "Allocating NPOT texture\n");
			image->textureWidth = width;
			image->textureHeight = height;
			ok = true;
		}
	}
	if(!ok){
		image->textureWidth = geGetNextPower2(width);
		image->textureHeight = geGetNextPower2(height);
		int framesz = max(image->textureWidth, image->textureHeight);
		image->textureWidth = framesz;
		image->textureHeight = framesz;
	}

	image->data = (u32*)geMalloc(image->textureWidth * image->textureHeight * sizeof(u32));
/*
	image->width = width;
	image->height = height;
	if(((width != height) || (width==height && width!=geGetNextPower2(width))) && geCheckExtensionAvailable("GL_ARB_texture_non_power_of_two") && geCheckExtensionAvailable("GL_ARB_texture_rectangle")){
//	if(0){
		gePrintDebug(0x100, "Allocating non power of two texture\n");
		image->flags |= GE_IMAGE_RECTANGLE;
		image->textureWidth = width;
		image->textureHeight = height;
	}else{
		image->textureWidth = geGetNextPower2(width);
		image->textureHeight = geGetNextPower2(height);

		int framesz = max(image->textureWidth, image->textureHeight);
		image->textureWidth = framesz;
		image->textureHeight = framesz;
	}
	image->data = (u32*)geMalloc(image->textureWidth * image->textureHeight * sizeof(u32));
*/
}

ge_Image* geAddTexture(ge_Image* tex){
	gePrintDebug(0x100, "geAddTexture(0x%08lX)...", (t_ptr)tex);
	//Compare
	ge_Image* ret = tex;
	bool already_exists = false;
	int i = 0;
	for(i=0; i<n_ge_textures; i++){
		if(!ge_textures[i])continue;
		if( !memcmp(&tex->width, &ge_textures[i]->width, sizeof(int)*4) ){  //sizeof(int)*4 for size and 2^n size
			if( !memcmp(tex->data, ge_textures[i]->data, sizeof(u32)*tex->textureWidth*tex->textureHeight) ){
				//It's the same
				already_exists = true;
				break;
			}
		}
	}
	if(!already_exists){
		n_ge_textures++;
		if(!ge_textures){
			ge_textures = (ge_Image**)geMalloc(sizeof(ge_Image)*n_ge_textures);
		}else{
			ge_textures = (ge_Image**)geRealloc(ge_textures, sizeof(ge_Image)*n_ge_textures);
		}
		ge_textures[n_ge_textures-1] = tex;
		geUpdateImage(tex);
	}else{
		ret = ge_textures[i];
		glDeleteTextures(1, &tex->id);
		libge_context->gpumem -= tex->textureWidth*tex->textureHeight*sizeof(u32);
		geFreeImage(tex);
	}
	gePrintDebug(0x100, "Ok\n");
	return ret;
}

void geUpdateImage(ge_Image* img){
	UpdateGlTexture(img, 0);
}

void geDeleteImage(ge_Image* img){
	glDeleteTextures(1, &img->id);
	libge_context->gpumem -= img->textureWidth*img->textureHeight*sizeof(u32);
}

void geTextureImage(int unit, ge_Image* img){
	if(img){
		glActiveTexture(GL_TEXTURE0+unit);
		if(img->flags & GE_IMAGE_3D){
			glBindTexture(GL_TEXTURE_3D, img->id);
			glEnable(GL_TEXTURE_3D);
		}else{
			glBindTexture(GL_TEXTURE_2D, img->id);
			glEnable(GL_TEXTURE_2D);
		}
	}else{
		glActiveTexture(GL_TEXTURE0+unit);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
	}
}

void geTextureMode(ge_Image* img, int mode){
	glBindTexture(GL_TEXTURE_2D, img->id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mode);
}

void geTextureWrap(ge_Image* img, int wrapx, int wrapy){
	glBindTexture(GL_TEXTURE_2D, img->id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapx);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapy);
}

ge_Framebuffer* geCreateFramebuffer(int width, int height){
	ge_Framebuffer* fbo = (ge_Framebuffer*)geMalloc(sizeof(ge_Framebuffer));
	fbo->texture = geCreateSurface(width, height, 0xFF000000);
	fbo->depth = geCreateSurface(width, height, 0xFF000000);
	fbo->samples = 1;

    glGenFramebuffers(1, (GLuint*)&fbo->id);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo->id);
	
	glBindTexture(GL_TEXTURE_2D, fbo->texture->id);
//	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
//	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo->texture->id, 0);

	glBindTexture(GL_TEXTURE_2D, fbo->depth->id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fbo->depth->id, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return fbo;
}


ge_Framebuffer* geCreateFramebufferMultisample(int width, int height, int samples){
	if(samples <= 0){
		return NULL;
	}
	if(samples == 1){
		return geCreateFramebuffer(width, height);
	}

	ge_Framebuffer* fbo = (ge_Framebuffer*)geMalloc(sizeof(ge_Framebuffer));
	fbo->texture = geCreateSurface(width, height, 0xFF000000);
	fbo->depth = geCreateSurface(width, height, 0xFF000000);
	fbo->samples = samples;
	

    glGenFramebuffers(1, (GLuint*)&fbo->id);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo->id);

	u32 colorBuffer, depthBuffer;
	// Multi sample colorbuffer
	glGenRenderbuffers(1, &colorBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, colorBuffer);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_RGBA8, width, height);
	// Multi sample depth buffer
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH_COMPONENT, width, height);
	// Attach them
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorBuffer);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);




    glGenFramebuffers(1, (GLuint*)&fbo->ms_id);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo->ms_id);
	glBindTexture(GL_TEXTURE_2D, fbo->texture->id);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo->texture->id, 0);

	glBindTexture(GL_TEXTURE_2D, fbo->depth->id);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fbo->depth->id, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return fbo;
}

void geFreeFramebuffer(ge_Framebuffer* fbo){
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	geFreeImage(fbo->texture);
	geFreeImage(fbo->depth);
	glDeleteFramebuffers(1, (GLuint*)&fbo->id);
	geFree(fbo);
}

u32 geFramebufferReadPixel(ge_Framebuffer* fbo, int x, int y){
	u32 px;
	glBindFramebuffer(GL_FRAMEBUFFER, fbo->id);
	glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &px);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return px;
}

void geFramebufferUse(ge_Framebuffer* fbo){
	static ge_Framebuffer* _ge_current_fbo = NULL;
	if(_ge_current_fbo && _ge_current_fbo != fbo && _ge_current_fbo->samples > 1){
		glBindFramebuffer(GL_READ_FRAMEBUFFER, _ge_current_fbo->id);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _ge_current_fbo->ms_id);
		glBlitFramebuffer(0, 0, _ge_current_fbo->texture->width, _ge_current_fbo->texture->height, 0, 0, _ge_current_fbo->texture->width, _ge_current_fbo->texture->height, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	}
	if(fbo){
		glBindFramebuffer(GL_FRAMEBUFFER, fbo->id);
	}else{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	_ge_current_fbo = fbo;
}

void geSetupMultipleRenderTargets(int n, ge_Framebuffer** fbs){
	GLenum buffers[4];
	int i, j=0;
	for(i=0; i<n; i++){
		buffers[i] = GL_FRONT_LEFT;
		if(fbs[i] && fbs[i]->texture){
		//	glDrawBuffer(GL_NONE);
		//	glReadBuffer(GL_NONE);
		//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
			buffers[i] = GL_COLOR_ATTACHMENT0+i;
			j++;
		}
	}
	glDrawBuffers(n, buffers);
}

void geTextureCopyBuffer(ge_Image* target, int ofsx, int ofsy, int sx, int sy, int w, int h){
	glBindTexture(GL_TEXTURE_2D, target->id);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, ofsx, ofsy, sx, sy, w, h);
}

static char _extensions[8192] = "";
void ListGlExtensions(){
#ifdef WIN32
	PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB;
	wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");
#ifdef TARGET_qt
	char* sysgl_ext = "";
//	char* sysgl_ext = (char*)wglGetExtensionsStringARB(((LibGE_QtContext*)libge_context->syscontext)->hDC);
#else
	char* sysgl_ext = (char*)wglGetExtensionsStringARB(((LibGE_WinlowContext*)libge_context->syscontext)->hDC);
#endif
#else //Unix
	char* sysgl_ext = "";
#endif
	char* gl_ext = (char*)glGetString(GL_EXTENSIONS);

	sprintf(_extensions, "%s %s", sysgl_ext, gl_ext);
	int i, j;
	for(i=0; i<strlen(_extensions); i++){
		if(i == 0){
			for(j=strlen(_extensions); j>i; j--){
				_extensions[j] = _extensions[j-1];
			}
			_extensions[i] = '\t';
		}
		if(_extensions[i] == ' '){
			for(j=strlen(_extensions); j>i; j--){
				_extensions[j] = _extensions[j-1];
			}
			_extensions[i] = '\n';
			_extensions[i+1] = '\t';
			i++;
		}
	}
	gePrintDebug(0x100, "Extensions :\n%s\n", _extensions);
}

int geCheckExtensionAvailable(const char* name){
	if(strlen(_extensions) <= 0){
		ListGlExtensions();
	}
	if(strstr(_extensions, name)){
		return true;
	}
	return false;
}


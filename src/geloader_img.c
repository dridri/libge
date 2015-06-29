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

#include "ge_internal.h"
void UpdateGlTexture(ge_Image* image, int);
void geAllocateSurface(ge_Image* image, int width, int height);

typedef struct _ge_file_buffer_t {
  char name[256];
  unsigned char *data;
  long length;
  long offset;
} _ge_file_buffer_t;

ge_Image* geLoadBmp(ge_File* file);
ge_Image* geLoadPng(ge_File* file);
ge_Image* geLoadPngSize(ge_File* file, int pref_w, int pref_h);
ge_Image* geLoadJpeg(ge_File* file);
ge_Image* geLoadTga(ge_File* file);
ge_Image* geLoadXPM(ge_File* file);

static ge_Image* LoadCorrespondingType(const char* file, int pref_w, int pref_h);

ge_Image* geLoadImageResizedOptions(const char* file, int pref_w, int pref_h, int flags){
	ge_Image* image = LoadCorrespondingType(file, pref_w, pref_h);
	if(image){
		image->flags = flags;
		if(flags != -1)geUpdateImage(image);
	}
	return image;
}

ge_Image* geLoadImageResized(const char* file, int pref_w, int pref_h){
	return geLoadImageResizedOptions(file, pref_w, pref_h, 0);
}

ge_Image* geLoadImageOptions(const char* file, int flags){
	return geLoadImageResizedOptions(file, 0, 0, flags);
}

ge_Image* geLoadImage(const char* file){
	return geLoadImageOptions(file, 0);
}

void geReleaseImage(ge_Image* img){
	if(img->data)geFree(img->data);
	img->data = NULL;
}

void geFreeImage(ge_Image* img){
	if((t_ptr)img==0xBAADF00D)return;
	if(!img)return;

	geDeleteImage(img);
	if(img->data)geFree(img->data);
	geFree(img);
}

static ge_Image* LoadCorrespondingType(const char* file, int pref_w, int pref_h){
	char buffer[64] = "";
	char buffer2[32] = "";
	ge_File* fp = NULL;

	char test_resource[256] = "";
	int file_len = strlen(file);
	strncpy(test_resource, file, file_len<22?file_len:22); //_ge_resource_header_0x  len=22
	if(!strcmp(test_resource, "_ge_resource_header_0x")){
		void* adress = NULL;
		sscanf(file, "_ge_resource_header_0x%p", &adress);
		ge_Resource* resource = (ge_Resource*)adress;
		fp = geFileBuffer(NULL, resource->size, GE_FILE_MODE_READ | GE_FILE_MODE_BINARY);
		fp->buffer = resource->buffer;
	}else{
		fp = geFileOpen(file, GE_FILE_MODE_READ | GE_FILE_MODE_BINARY);
		if(!fp)return NULL;
	}
	geFileRead(fp, buffer, 64);
	geFileSeek(fp, -32, GE_FILE_SEEK_END);
	geFileRead(fp, buffer2, 32);
	geFileRewind(fp);

	ge_Image* img = NULL;
#ifndef LIBGE_MINI
	if(!strncmp(buffer, "BM", 2)){
		img = geLoadBmp(fp);
	}else
	if(!strncmp(&buffer[1], "PNG", 3)){
		img = geLoadPngSize(fp, pref_w, pref_h);
	}else
	if(!strncmp(&buffer[6], "JFIF", 4) || !strncmp(&buffer[6], "Exif", 4)){
		img = geLoadJpeg(fp);
	}else
	if(strstr(buffer2+14, "TRUEVISION-XFILE")){
		img = geLoadTga(fp);
	}
	if(strstr(buffer, "/* XPM */")){
		img = geLoadXPM(fp);
	}
#endif
	fp->buffer = NULL;
	geFileClose(fp);
	if(img){
		img->color = 0xffffffff;
	}
	return img;
}

ge_Image* geCreateSurface(short width, short height, u32 color){
	ge_Image* image = (ge_Image*)geMalloc(sizeof(ge_Image));
	if(!image)return NULL;
	
	geAllocateSurface(image, width, height);
	if(!image->data)return NULL;

	if(color)geFillRectImage(0, 0, width, height, color, image);
	image->color = 0xffffffff;
	geUpdateImage(image);

	return image;
}

ge_Image3D* geCreateSurface3D(short width, short height, short depth, u32 color){
	ge_Image3D* image = (ge_Image3D*)geMalloc(sizeof(ge_Image3D));
	if(!image)return NULL;
	memset(image, 0, sizeof(ge_Image3D));

	image->flags = GE_IMAGE_3D;
	image->width = width;
	image->height = height;
	image->depth = depth;
	image->textureWidth = geGetNextPower2(width);
	image->textureHeight = geGetNextPower2(height);
	image->textureDepth = geGetNextPower2(depth);
	image->data = (u32*)geMalloc(image->textureWidth * image->textureHeight * image->textureDepth * sizeof(u32));
	if(!image->data)return NULL;
	
	if(color){
		int skipX = image->textureWidth - width;
		int skipY = image->textureHeight - height;
		int X, Y, Z;
		u32* data = image->data;
		for(Z = 0; Z < depth; Z++, data += skipY){
			for(Y = 0; Y < height; Y++, data += skipX){
				for(X = 0; X < width; X++, data++){
					*data = color;
				}
			}
		}
	}

	image->color = 0xffffffff;
	return image;
}

ge_Image* geCreateSurfaceFromSurface3D(ge_Image3D* image, int depth, int alloc_new_buffer){
	ge_Image* image2d = (ge_Image*)geMalloc(sizeof(ge_Image));
	if(!image)return NULL;
	if(!image2d)return NULL;
	memset(image2d, 0, sizeof(ge_Image));

	image2d->color = image->color;
	image2d->flags = image->flags - GE_IMAGE_3D;
	image2d->width = image->width;
	image2d->height = image->height;
	image2d->textureWidth = image->textureWidth;
	image2d->textureHeight = image->textureHeight;
//	image2d->id = image->id;

	if(alloc_new_buffer){
		image2d->data = (u32*)geMalloc(image->textureWidth * image->textureHeight * sizeof(u32));
		if(!image2d->data)return NULL;
		memcpy(image2d->data, &image->data[depth*image->textureDepth], image->textureWidth * image->textureHeight * sizeof(u32));
	}else{
		image2d->data = &image->data[depth*image->textureDepth];
	}

	geUpdateImage(image2d);
	return image2d;
}

ge_Image* geAnimateImage(ge_Image* _img, int n, float t){
	_ge_ImageAnimated* img = (_ge_ImageAnimated*)geMalloc(sizeof(_ge_ImageAnimated));
	memcpy(img, _img, sizeof(ge_Image));
	img->flags |= GE_IMAGE_ANIMATED;
	img->realHeight = img->height;
	img->height = img->realHeight / n;
	img->v = img->v / n;
	img->nImages = n;
	img->frameTime = t;
	img->_ge_n = 0;
	img->_ge_t = 0.0f;
	return (ge_Image*)img;
	
}

static ge_Image _ge_image3d_to_image2d;
ge_Image* geSurfaceFromSurface3D(ge_Image3D* image, int depth){
	_ge_image3d_to_image2d.color = image->color;
	_ge_image3d_to_image2d.data = &image->data[depth*image->textureDepth];
	_ge_image3d_to_image2d.flags = image->flags;
	_ge_image3d_to_image2d.width = image->width;
	_ge_image3d_to_image2d.height = image->height;
	_ge_image3d_to_image2d.textureWidth = image->textureWidth;
	_ge_image3d_to_image2d.textureHeight = image->textureHeight;
	_ge_image3d_to_image2d.id = image->id;
	return &_ge_image3d_to_image2d;
}

ge_Image* geDuplicateImage(ge_Image* source){
	ge_Image* dest = geCreateSurface(source->width, source->height, 0x00000000);

	u32* sv = dest->data;
	memcpy(dest, source, sizeof(ge_Image));

	dest->data = sv;
	memcpy(dest->data, source->data, sizeof(u32)*dest->textureWidth*dest->textureHeight);

	dest->id = 0x0;
	dest->flags = source->flags;
//	geUpdateImage(dest);
	return dest;
}

int geGetNextPower2(int width){
	int b = width;
	int n;
	for (n = 0; b != 0; n++) b >>= 1;
	b = 1 << n;
	if (b == 2 * width) b >>= 1;
	return b;
}

void geFlipImageVertical(ge_Image* img){
	int mi_height = (int)img->height / 2;

	int size = img->textureWidth*img->height;
	int line_size = 4*img->textureWidth;
	u8* buffer = (u8*)geMalloc(line_size);
	int y, i=0, j=size-img->textureWidth;
	for(y=0; y<mi_height; y++){
		memcpy(buffer, &img->data[i], line_size);
		memcpy(&img->data[i], &img->data[j], line_size);
		memcpy(&img->data[j], buffer, line_size);

		i += img->textureWidth;
		j -= img->textureWidth;
		if(i>=((size*4)-line_size))break;
	}
	geUpdateImage(img);
}

ge_Image* geResizeImage(ge_Image* src, int new_width, int new_height, int method){
	ge_Image* dst = geCreateSurface(new_width, new_height, 0x00000000);
	if(!dst)return NULL;

	float ratio_w = (float)new_width / (float)src->width;
	float ratio_h = (float)new_height / (float)src->height;
	gePrintDebug(0x100, "geResizeImage: ratio_w: %f = %d / %d\n", ratio_w, new_width, src->width);
	gePrintDebug(0x100, "geResizeImage: ratio_h: %f = %d / %d\n", ratio_h, new_height, src->height);
//	int i=0, j=0;
	int x=0, y=0;
//	int line_j=0, line_y=0;
//	int jp=0, jump=0;

	for(y=0; y<dst->height; y++){
		for(x=0; x<dst->width; x++){
			if(method == GE_NEAREST){
				dst->data[x+y*dst->textureWidth] = src->data[((int)((float)x/ratio_w)) + ((int)((float)y/ratio_h))*src->textureWidth];
			}
			if(method == GE_LINEAR){
				int x2 = ( (int)( (float)(x+1)/ratio_w ) < src->width ) ? (x+1) : x;
				int y2 = ( (int)( (float)(y+1)/ratio_h ) < src->height ) ? (y+1) : y;
				int xn2 = ((x-1)>0)?(x-1):x;
				int yn2 = ((y-1)>0)?(y-1):y;
				u32 c0 = src->data[((int)((float)x/ratio_w)) + ((int)((float)y/ratio_h))*src->textureWidth];
				u32 c1 = src->data[((int)((float)x2/ratio_w)) + ((int)((float)y/ratio_h))*src->textureWidth];
				u32 c2 = src->data[((int)((float)x/ratio_w)) + ((int)((float)y2/ratio_h))*src->textureWidth];
				u32 c3 = src->data[((int)((float)x2/ratio_w)) + ((int)((float)y2/ratio_h))*src->textureWidth];
				u32 c4 = src->data[((int)((float)xn2/ratio_w)) + ((int)((float)y/ratio_h))*src->textureWidth];
				u32 c5 = src->data[((int)((float)x/ratio_w)) + ((int)((float)yn2/ratio_h))*src->textureWidth];
				u32 c6 = src->data[((int)((float)xn2/ratio_w)) + ((int)((float)yn2/ratio_h))*src->textureWidth];
				u32 c7 = src->data[((int)((float)xn2/ratio_w)) + ((int)((float)y2/ratio_h))*src->textureWidth];
				u32 c01 = geMixColors(c0, c1, 0.5);
				u32 c23 = geMixColors(c2, c3, 0.5);
				u32 c45 = geMixColors(c4, c5, 0.5);
				u32 c67 = geMixColors(c6, c7, 0.5);
				u32 c0123 = geMixColors(c01, c23, 0.5);
				u32 c4567 = geMixColors(c45, c67, 0.5);
				dst->data[x+y*dst->textureWidth] = geMixColors(c0123, c4567, 0.5);
			}
		}
	}
/*
	for(j=0; j<src->height; j++){
		y = (int)(j* ratio_h);
		for(i=0; i<src->width; i++){
			x = (int)(i* ratio_w);
			jump = x - i;
			gePrintDebug(0x100, "new position of %d/%d ==> %d/%d width ratio %f/%f	jump: %d\n", i, j, x, y, ratio_w, ratio_h, jump);
		//	char s[128]; gets(s);
			for(jp=0; jp<jump; jp++){
				dst->data[x+y*dst->textureWidth+jp] = src->data[i+j*src->textureWidth];
			}
		}
		line_j += src->textureWidth;
		line_y += y* dst->textureWidth;
		jump = 0;
	}
*/
	geUpdateImage(dst);
	return dst;
}

void lQ_effect(ge_Image* img){
	int cswap = 0;
	int size = 4 * img->textureWidth * img->textureHeight;
	u8* buffer = (u8*)img->data;
	for(cswap = 0; cswap < size; cswap += 4){
		buffer[cswap+4] ^= buffer[cswap+6];
		buffer[cswap+1] ^= buffer[cswap+4];
		buffer[cswap+4] ^= buffer[cswap+1];
	//	buffer[cswap+4] ^= buffer[cswap+1] ^= buffer[cswap+4] ^= buffer[cswap+6];
	}
	geUpdateImage(img);
}

void geImageEffect(ge_Image* img, int effect){
	switch(effect){
		case GE_EFFECT_ANTI_GRADATION:
			lQ_effect(img);
			break;
		default:
			return;
	}
}
/*
R  G  B		R  G  B		R  G  B
00 ff 02	  ff 04 ff	  ff ff ff
R  G  B  A	R  G  B  A	R  G  B  A
00 ff 02 ff  ff 04 ff ff  ff ff ff ff
A  B  G  R	A  B  G  R	A  B  G  R
ff 02 ff 00  ff ff 04 ff  ff ff ff ff
*/

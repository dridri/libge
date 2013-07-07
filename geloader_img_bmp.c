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

#include "libge.h"
#define getIntBuffer(b,a) ((int)((u8)b[a] | (u16)(b[a+1]<<8) | (u32)(b[a+2]<<16) | (u32)(b[a+3]<<24)))
#define getShortBuffer(b,a) ((short)((u8)b[a] | (u16)(b[a+1]<<8)))
void CreateGlTexture(ge_Image* image);

bool LoadBmp_RGB(ge_File* fp, u8* header, ge_Image* image, int width, int height, int bpp);
bool LoadBmp_Palette(ge_File* fp, u8* header, ge_Image* image, int width, int height, int bpp);



ge_Image* geLoadBmp(ge_File* fp){
	geFileSeek(fp, 0, GE_FILE_SEEK_END);
	int file_size = geFileTell(fp);
	geFileSeek(fp, 0, GE_FILE_SEEK_SET);

	ge_Image* image = NULL;

	int width=0, height=0, bpp=0;
	bool inversed = true;

	u8 file_header[14] = { 0 };
	geFileRead(fp, file_header, 14);

	if(getIntBuffer(file_header, 2) != file_size){
		goto error;
	}

	int data_ofs = getIntBuffer(file_header, 10);

	u8 header[40] = { 0 };
	geFileRead(fp, header, 40);

	int compression = getShortBuffer(header, 18);
	int Planes = 0;
	if(compression > 3){//OS2 Type
		width = getShortBuffer(header, 4);
		height = getShortBuffer(header, 6);
		Planes = getShortBuffer(header, 8);
		bpp = getShortBuffer(header, 10);
		compression = 0;
		header[18] = 3;
	}else{				//Windows type
		width = getIntBuffer(header, 4);
		height = getIntBuffer(header, 8);
		Planes = getShortBuffer(header, 12);
		bpp = getShortBuffer(header, 14);
		header[18] = 4;
	}
	if((Planes!=1) || (bpp<8)){
		goto error;
	}
	if(height < 0){
		height = -height;
		inversed = false;
	}

	geFileRewind(fp);
	bool ok = false;
	image = geCreateSurface(width, height, 0x00000000);

	header[19] = (u8)data_ofs;
	if(compression == 0){ //Non-compressed
		if(bpp <= 8){
			ok = LoadBmp_Palette(fp, header, image, width, height, bpp);
		}
		if(bpp >= 16){
			geFileSeek(fp, data_ofs, GE_FILE_SEEK_SET);
			ok = LoadBmp_RGB(fp, header, image, width, height, bpp);
		}
	}
	if(!ok)goto error;


	if(inversed)geFlipImageVertical(image);
	return image;
error:
	if(image)geFreeImage(image);
	return NULL;
}

bool LoadBmp_RGB(ge_File* fp, u8* header, ge_Image* image, int width, int height, int bpp){
	int i, j, src=0, dst=0;
	int jump = bpp/8;

	u8* buffer = (u8*)geMalloc(width*jump);

	for(j=0; j<height; j++, dst+=(image->textureWidth-width)){
		geFileRead(fp, buffer, width*jump);
		src = 0;
		for(i=0; i<width; i++, src+=jump, dst++){
			if(bpp == 16){
				u16 color = getShortBuffer(buffer, src);
				u8 r = (u8)(((color & 0x7C00) >> 10)<<3);
				u8 g = (u8)(((color & 0x03E0) >> 5)<<3);
				u8 b = (u8)((color & 0x001F)<<3);
				image->data[dst] = RGBA(r, g, b, 255);
			}
			if(bpp == 24){
				image->data[dst] = RGBA(buffer[src+2], buffer[src+1], buffer[src], 255);
			}
			if(bpp == 32){
				image->data[dst] = RGBA(buffer[src+2], buffer[src+1], buffer[src], buffer[src+3]);
			//	gePrintDebug(0x100, "pixel(%dx%d)= %d %d %d %d\n", i, j, R(image->data[dst]), G(image->data[dst]), B(image->data[dst]), A(image->data[dst]));
			}
		}
	}

	geFree(buffer);
	return true;
}

bool LoadBmp_Palette(ge_File* fp, u8* header, ge_Image* image, int width, int height, int bpp){
	int palette_bpp = header[18];
	int palette_sz = (1<<bpp) *palette_bpp;

	u8* palette = (u8*)geMalloc(palette_sz);
	geFileRead(fp, palette, palette_sz);
	int i, j, dst=0;

	u8* buffer = (u8*)geMalloc(width);

	for(j=0; j<height; j++, dst+=(image->textureWidth-width)){
		geFileRead(fp, buffer, width);
		for(i=0; i<width; i++, dst++){
			u8 color = buffer[50+palette_sz/*+j*width*/+i];
			if(bpp == 8){
				image->data[dst] = RGB(palette[color*palette_bpp+2], palette[color*palette_bpp+1], palette[color*palette_bpp]);
			}
		}
	}

	return true;
}

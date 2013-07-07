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
void CreateGlTexture(ge_Image* image);
bool LoadTGAUncompressed_RGB(ge_File* fTGA, u8* header, ge_Image* image, int width, int height, int bpp);
bool LoadTGAUncompressed_Palette(ge_File* fTGA, u8* header, ge_Image* image, int width, int height, int bpp);
bool LoadTGAUncompressed_Gray(ge_File* fTGA, u8* header, ge_Image* image, int width, int height, int bpp);
bool LoadTGACompressed_RGB(ge_File* fTGA, u8* header, ge_Image* image, int width, int height, int bpp);
bool LoadTGACompressed_Palette(ge_File* fTGA, u8* header, ge_Image* image, int width, int height, int bpp);
bool LoadTGACompressed_Gray(ge_File* fTGA, u8* header, ge_Image* image, int width, int height, int bpp);

ge_Image* geLoadTga(ge_File* fTGA){
	u8 header[20] = { 0 };
	geFileRead(fTGA, header, 18);

	int width  = header[13] * 256 + header[12];
	int height = header[15] * 256 + header[14];
	int bpp	= header[16] / 8;

	if((width <= 0) || (height <= 0)){
		geFileClose(fTGA);
		return NULL;
	}

	ge_Image* image = geCreateSurface(width, height, 0x00000000);

	bool ok = false;
	if(header[2] == 1){
		ok = LoadTGAUncompressed_Palette(fTGA, header, image, width, height, bpp);
	}else
	if(header[2] == 2){
		ok = LoadTGAUncompressed_RGB(fTGA, header, image, width, height, bpp);
	}else
	if(header[2] == 3){
		ok = LoadTGAUncompressed_Gray(fTGA, header, image, width, height, bpp);
	}else
	if(header[2] == 9){
		ok = LoadTGACompressed_Palette(fTGA, header, image, width, height, bpp);
	}else
	if(header[2] == 10){
		ok = LoadTGACompressed_RGB(fTGA, header, image, width, height, bpp);
	}else
	if(header[2] == 11){
		ok = LoadTGACompressed_Gray(fTGA, header, image, width, height, bpp);
	}
	if(!ok){
		geFileClose(fTGA);
		geFreeImage(image);
		return NULL;
	}

	geFlipImageVertical(image);
	return image;
}

bool LoadTGAUncompressed_RGB(ge_File* fTGA, u8* header, ge_Image* image, int width, int height, int bpp){
	int size = (bpp/8) * width * height;
	u8* buffer	= (u8*)geMalloc((bpp>16?(bpp/8):3) * width * height);

	geFileRead(fTGA, buffer, size);

	if(bpp >= 3){
		// Byte Swapping Optimized By Steve Thomas
		int cswap = 0;
		for(cswap = 0; cswap < size; cswap += bpp){
			buffer[cswap] ^= buffer[cswap+2];
			buffer[cswap+2] ^= buffer[cswap];
			buffer[cswap] ^= buffer[cswap+2];
		}
	}

	if(bpp == 1){
		memcpy(image->data, buffer, size);
		u8* buf2 = (u8*)image->data;
		int s, d;
		for(s=0, d=0; s<size; s++, d+=3){
			buffer[d] = buf2[s];
			buffer[d+1] = buf2[s];
			buffer[d+2] = buf2[s];
		}
		bpp = 3;
		size = bpp * width * height;
		memset(image->data, 0, sizeof(u32)*image->textureWidth*image->textureHeight);
	}else
	if(bpp == 2){
		memcpy(image->data, buffer, size);
		u8* buf2 = (u8*)image->data;
		int s, d;
		for(s=0, d=0; s<size; s+=2, d+=3){
			u16 color = buf2[s] + (buf2[s+1]<<8);
			buffer[d] = (u8)(((color & 0x7C00) >> 10)<<3);
			buffer[d+1] = (u8)(((color & 0x03E0) >> 5)<<3);
			buffer[d+2] = (u8)((color & 0x001F)<<3);
		}
		bpp = 3;
		size = bpp * width * height;
		memset(image->data, 0, sizeof(u32)*image->textureWidth*image->textureHeight);
	}

	int i, j;
	int src = 0;
	int dst = 0;
	for(j=0; j<height; j++, dst+=(image->textureWidth-width), src+=0){
		for(i=0; i<width && src<size; i++,dst++,src+=bpp){
			if(bpp == 4){
				image->data[dst] = RGBA(buffer[src], buffer[src+1], buffer[src+2], buffer[src+3]);
			}
			if(bpp == 3){
				image->data[dst] = RGBA(buffer[src], buffer[src+1], buffer[src+2], 255);
			}
		}
	}
	
	geFree(buffer);
	return true;
}

bool LoadTGAUncompressed_Palette(ge_File* fTGA, u8* header, ge_Image* image, int width, int height, int bpp){
	int palette_bpp = header[7]/8;
	int palette_count = header[5] + (header[6]<<8);

	u8* palette = (u8*)geMalloc(palette_count*palette_bpp);
	geFileRead(fTGA, palette, palette_count*palette_bpp);

	u8* buffer = (u8*)geMalloc(width*height);
	geFileRead(fTGA, buffer, width*height);

	int i, j, src=0, dst=0;
	for(j=0; j<height; j++, dst+=(image->textureWidth-width)){
		for(i=0; i<width; i++, dst++, src++){
			if(palette_bpp == 4){
				image->data[dst] = RGBA(palette[buffer[src]*4+2], palette[buffer[src]*4+1], palette[buffer[src]*4], palette[buffer[src]*4+3]);
			}
			if(palette_bpp == 3){
				image->data[dst] = RGB(palette[buffer[src]*3+2], palette[buffer[src]*3+1], palette[buffer[src]*3]);
			}
			if(palette_bpp == 2){
				u8 r = (u8)(((palette[buffer[src]*2+2] & 0x7C00) >> 10)<<3);
				u8 g = (u8)(((palette[buffer[src]*2+1] & 0x03E0) >> 5)<<3);
				u8 b = (u8)((palette[buffer[src]*2] & 0x001F)<<3);
				image->data[dst] = RGB(r, g, b);
			}
		}
	}
	
	geFree(palette);
	geFree(buffer);
	return true;
}

bool LoadTGAUncompressed_Gray(ge_File* fTGA, u8* header, ge_Image* image, int width, int height, int bpp){
	u8* buffer = (u8*)geMalloc(width*height);
	geFileRead(fTGA, buffer, width*height);

	int i, j, src=0, dst=0, jmp=bpp;
	for(j=0; j<height; j++, dst+=(image->textureWidth-width)){
		for(i=0; i<width; i++, dst++, src+=jmp){
			if(bpp == 1){
				image->data[dst] = RGBA(buffer[src], buffer[src], buffer[src], buffer[src]);
			}
			if(bpp == 2){
				image->data[dst] = RGBA(buffer[src], buffer[src], buffer[src], buffer[src+1]);
			}
		}
	}

	geFree(buffer);
	return true;
}

bool LoadTGACompressed_RGB(ge_File* fTGA, u8* header, ge_Image* image, int width, int height, int bpp){
	int i, size;
	u8 rgb[4];
	u8 packet_header;
	int jmp = (bpp<3? 3 : bpp);
	u16 color = 0x0;

	u8* buffer = (u8*)geMalloc(width*height*jmp);
	u8* ptr = buffer;

	while (ptr < buffer + (width * height) * jmp){
		packet_header = (u8)geFileGetChar(fTGA);
		size = 1 + (packet_header & 0x7f);

		if (packet_header & 0x80){
			if(bpp >= 3){
				geFileRead(fTGA, rgb, bpp);
			}else if(bpp == 2){
				color = (u16)((u8)(geFileGetChar(fTGA)) + (u16)(geFileGetChar(fTGA)<<8));
			}

			for (i = 0; i < size; ++i, ptr += jmp){
				if(bpp == 2){
					ptr[0] = (u8)(((color & 0x7C00) >> 10) << 3);
					ptr[1] = (u8)(((color & 0x03E0) >>  5) << 3);
					ptr[2] = (u8)(((color & 0x001F) >>  0) << 3);
				}
				if(bpp >= 3){
					ptr[0] = rgb[2];
					ptr[1] = rgb[1];
					ptr[2] = rgb[0];
					if(bpp==4)ptr[3] = rgb[3];
				}
			}
		}else{
			for (i = 0; i < size; ++i, ptr += jmp){
				if(bpp == 2){
					color = (u16)((u8)(geFileGetChar(fTGA)) + (u16)(geFileGetChar(fTGA)<<8));
					ptr[0] = (u8)(((color & 0x7C00) >> 10) << 3);
					ptr[1] = (u8)(((color & 0x03E0) >>  5) << 3);
					ptr[2] = (u8)(((color & 0x001F) >>  0) << 3);
				}
				if(bpp >= 3){
					ptr[2] = (u8)geFileGetChar(fTGA);
					ptr[1] = (u8)geFileGetChar(fTGA);
					ptr[0] = (u8)geFileGetChar(fTGA);
					if(bpp==4)ptr[3] = (u8)geFileGetChar(fTGA);
				}
			}
		}
	}

	int j;
	int src = 0;
	int dst = 0;
	for(j=0; j<height; j++, dst+=(image->textureWidth-width)){
		for(i=0; i<width; i++,dst++,src+=jmp){
			if(bpp == 4){
				image->data[dst] = RGBA(buffer[src], buffer[src+1], buffer[src+2], buffer[src+3]);
			}
			if(bpp==2 || bpp==3){
				image->data[dst] = RGB(buffer[src], buffer[src+1], buffer[src+2]);
			}
		}
	}

	geFree(buffer);
	return true;
}

bool LoadTGACompressed_Palette(ge_File* fTGA, u8* header, ge_Image* image, int width, int height, int bpp){
	int i, size;
	u8 index;
	u8 packet_header;

	int palette_bpp = header[7]/8;
	int palette_count = header[5] + (header[6]<<8);
	u8* palette = (u8*)geMalloc(palette_count*palette_bpp);
	geFileRead(fTGA, palette, palette_count*palette_bpp);

	u8* buffer = (u8*)geMalloc(width*height*3);
	u8* ptr = buffer;

	while (ptr < buffer + (width * height) * 3){
		packet_header = (u8)geFileGetChar(fTGA);
		size = 1 + (packet_header & 0x7f);

		if (packet_header & 0x80){
			index = (u8)geFileGetChar(fTGA);

			for (i = 0; i < size; ++i, ptr += 3){
				ptr[0] = palette[index*3 +2];
				ptr[1] = palette[index*3 +1];
				ptr[2] = palette[index*3 +0];
			}
		}else{
			for (i = 0; i < size; ++i, ptr += 3){
				index = (u8)geFileGetChar(fTGA);
				ptr[0] = palette[index*3 +2];
				ptr[1] = palette[index*3 +1];
				ptr[2] = palette[index*3 +0];
			}
		}
	}

	int j;
	int src = 0;
	int dst = 0;
	for(j=0; j<height; j++, dst+=(image->textureWidth-width)){
		for(i=0; i<width; i++,dst++,src+=3){
			image->data[dst] = RGB(buffer[src], buffer[src+1], buffer[src+2]);
		}
	}

	geFree(palette);
	geFree(buffer);
	return true;
}

bool LoadTGACompressed_Gray(ge_File* fTGA, u8* header, ge_Image* image, int width, int height, int bpp){
	int i, size;
	u8 color, alpha=0;
	u8 packet_header;

	u8* buffer = (u8*)geMalloc(width*height);
	u8* ptr = buffer;

	while (ptr < buffer + (width * height) * bpp){
		packet_header = (u8)geFileGetChar(fTGA);
		size = 1 + (packet_header & 0x7f);

		if (packet_header & 0x80){
			color = (u8)geFileGetChar(fTGA);
			if(bpp==2)alpha = (u8)geFileGetChar(fTGA);

			for (i = 0; i < size; ++i, ptr+=bpp){
				ptr[0] = color;
				if(bpp==2)ptr[1] = alpha;
			}
		}else{
			for (i = 0; i < size; ++i, ptr+=bpp){
				color = (u8)geFileGetChar(fTGA);
				if(bpp==2)alpha = (u8)geFileGetChar(fTGA);
				ptr[0] = color;
				if(bpp==2)ptr[1] = alpha;
			}
		}
	}

	int j;
	int src = 0;
	int dst = 0;
	for(j=0; j<height; j++, dst+=(image->textureWidth-width)){
		for(i=0; i<width; i++,dst++,src+=bpp){
			if(bpp == 2){
				image->data[dst] = RGBA(buffer[src], buffer[src], buffer[src], buffer[src + 1]);
			}else{
				image->data[dst] = RGBA(buffer[src], buffer[src], buffer[src], 255);
			}
		}
	}

	geFree(buffer);
	return true;
}

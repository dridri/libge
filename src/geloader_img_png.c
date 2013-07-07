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
#include <png.h>

static void png_read_from_geFile(png_structp png_ptr, png_bytep data, png_size_t length);
/*
png_voidp ge_png_malloc(png_structp png_ptr, png_size_t size){
	// There is a memory leak in libpng, using self-made malloc crashes because libpng makes buffer overflow on top of address
	return geMalloc(size);
}
void ge_png_free(png_structp png_ptr, png_voidp data){
	geFree(data);
}
*/

typedef struct _ge_png_alloc {
	t_ptr addr;
	t_ptr real_addr;
	t_ptr real_size;
} _ge_png_alloc;
_ge_png_alloc png_allocs[2048];
int png_allocs_i = 0;

png_voidp ge_png_malloc(png_structp png_ptr, png_size_t size){
	t_ptr* ptr = (t_ptr*)geMalloc(size);
	png_allocs[png_allocs_i].addr = (t_ptr)ptr;
	png_allocs[png_allocs_i].real_addr = ptr[-2];
	png_allocs[png_allocs_i].real_size = ptr[-1];
	png_allocs_i++;
	return (void*)ptr;
}
void ge_png_free(png_structp png_ptr, png_voidp data){
	int i;
	for(i=0; i<png_allocs_i; i++){
		if(png_allocs[i].addr == (t_ptr)data){
			((t_ptr*)data)[-2] = png_allocs[i].real_addr;
			((t_ptr*)data)[-1] = png_allocs[i].real_size;
			geFree(data);
			break;
		}
	}
}

ge_Image* geLoadPngSize(ge_File* file, int pref_w, int pref_h){
	png_allocs_i = 0;

	png_structp png_ptr;
	png_infop info_ptr;
	png_uint_32 width, height;
	int w, h;
	int bit_depth, color_type, interlace_type, x, y;
	u32* line;

	u8 magic[8] = { 0x0 };
	geFileRead(file, magic, 8);
	geFileRewind(file);
#ifdef png_check_sig
	if(!png_check_sig(magic, 8)){
		return NULL;
	}
#endif

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp*)file->filename, NULL, NULL);
	if (png_ptr == NULL) {
		return NULL;
	}
	png_set_mem_fn(png_ptr, NULL, ge_png_malloc, ge_png_free);
	png_set_error_fn(png_ptr, (png_voidp) NULL, (png_error_ptr) NULL, NULL);
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return NULL;
	}

	png_set_read_fn(png_ptr, (png_voidp*)file, png_read_from_geFile);

	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);
	w = (pref_w > 0) ? pref_w : width;
	h = (pref_h > 0) ? pref_h : height;
	ge_Image* image = geCreateSurface(w, h, 0x00000000);
	if(!image || !image->data){
		return NULL;
	}
	png_set_strip_16(png_ptr);
	png_set_packing(png_ptr);
	if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png_ptr);
//	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_gray_1_2_4_to_8(png_ptr);
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);
	png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);

	line = (u32*) geMalloc(width * sizeof(u32));
	if (!line) {
		geFree(image->data);
		geFree(image);
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return NULL;
	}

	for (y = 0; y < height; y++) {
		png_read_row(png_ptr, (u8*) line, NULL);
		for (x = 0; x < width; x++) {
			u32 color = line[x];
			int x2, y2;
			for(y2=(y*h/height); y2<((y+1)*h/height); y2++){
				for(x2=(x*w/width); x2<((x+1)*w/width); x2++){
					image->data[x2 + y2 * image->textureWidth] = color;
				}
			}
			//image->data[(x * w / width) + (y * h / height) * image->textureWidth] = color;
		}
	}

	geFree(line);
	png_read_end(png_ptr, info_ptr);
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
/*
	if(w > width || h > height){
		for(y=1; y<h-1; y++){
			for(x=1; x<w-1; x++){
				u32 c1 = image->data[(x - 1) + (y - 1) * image->textureWidth];
				u32 c2 = image->data[(x - 0) + (y - 1) * image->textureWidth];
				u32 c3 = image->data[(x + 1) + (y - 1) * image->textureWidth];
				u32 c4 = image->data[(x - 1) + (y - 0) * image->textureWidth];
				u32 c5 = image->data[(x - 0) + (y - 0) * image->textureWidth];
				u32 c6 = image->data[(x + 1) + (y - 0) * image->textureWidth];
				u32 c7 = image->data[(x - 1) + (y + 1) * image->textureWidth];
				u32 c8 = image->data[(x - 0) + (y + 1) * image->textureWidth];
				u32 c9 = image->data[(x + 1) + (y + 1) * image->textureWidth];
				u32 c12 = geMixColors(c1, c2, 0.5);
				u32 c34 = geMixColors(c1, c2, 0.5);
				u32 c56 = geMixColors(c1, c2, 0.5);
				u32 c78 = geMixColors(c1, c2, 0.5);
				image->data[x + y * image->textureWidth] = geMixColors(c5, geMixColors(geMixColors(c12, c34, 0.5), geMixColors(c56, c78, 0.5), 0.5), 0.5);
			}
		}
	}
*/
	return image;
}

static void png_read_from_geFile(png_structp png_ptr, png_bytep data, png_size_t length){
	ge_File* file = (ge_File*)png_get_io_ptr(png_ptr);
	geFileRead(file, data, length);
}

ge_Image* geLoadPng(ge_File* file){
	return geLoadPngSize(file, 0, 0);
}


/*

ge_Image* geLoadPng(ge_File* file){
	png_allocs_i = 0;

	png_structp png_ptr;
	png_infop info_ptr;
	png_uint_32 width, height;
	int bit_depth, color_type, interlace_type, x, y;
	u32* line;

	u8 magic[8] = { 0x0 };
	geFileRead(file, magic, 8);
	geFileRewind(file);
#ifdef png_check_sig
	if(!png_check_sig(magic, 8)){
		return NULL;
	}
#endif

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp*)file->filename, NULL, NULL);
	if (png_ptr == NULL) {
		return NULL;
	}
	png_set_mem_fn(png_ptr, NULL, ge_png_malloc, ge_png_free);
	png_set_error_fn(png_ptr, (png_voidp) NULL, (png_error_ptr) NULL, NULL);
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return NULL;
	}

	png_set_read_fn(png_ptr, (png_voidp*)file, png_read_from_geFile);

	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);
	ge_Image* image = geCreateSurface(width, height, 0x00000000);
	if(!image || !image->data){
		return NULL;
	}
	png_set_strip_16(png_ptr);
	png_set_packing(png_ptr);
	if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png_ptr);
//	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_gray_1_2_4_to_8(png_ptr);
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);
	png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);

	line = (u32*) geMalloc(width * 4);
	if (!line) {
		geFree(image->data);
		geFree(image);
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return NULL;
	}
	for (y = 0; y < height; y++) {
		png_read_row(png_ptr, (u8*) line, NULL);
		for (x = 0; x < width; x++) {
			u32 color = line[x];
			image->data[x + y * image->textureWidth] = color;
		}
	}
	geFree(line);
	png_read_end(png_ptr, info_ptr);
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

	return image;
}
*/

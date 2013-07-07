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
#include <jpeglib.h>
#include <jerror.h>

typedef struct ge_jpeg_src {
	struct jpeg_source_mgr jsrc;

	ge_File* file;
	u8* buffer;
	int start_of_file;
} ge_jpeg_src;

#define INPUT_BUF_SIZE 4096

void ge_init_jpeg_src(struct jpeg_decompress_struct* dinfo, ge_File* file);
static void init_source(j_decompress_ptr cinfo);
static boolean file_fill_input_buffer(j_decompress_ptr cinfo);
static void skip_input_data (j_decompress_ptr cinfo, long num_bytes);
static void term_source(j_decompress_ptr cinfo);

ge_Image* geLoadJpeg(ge_File* file){
	struct jpeg_decompress_struct dinfo;

	struct jpeg_error_mgr jerr;

	dinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&dinfo);

	ge_init_jpeg_src(&dinfo, file);

	jpeg_read_header(&dinfo, TRUE);
	int width = dinfo.image_width;
	int height = dinfo.image_height;
	jpeg_start_decompress(&dinfo);

	ge_Image* image = geCreateSurface(width, height, 0x00000000);

	if (!image) {
		jpeg_destroy_decompress(&dinfo);
		return NULL;
	}

	u8* line = (u8*) geMalloc(width * 3);
	if (!line) {
		jpeg_destroy_decompress(&dinfo);
		return NULL;
	}
	if (dinfo.jpeg_color_space == JCS_GRAYSCALE) {
		while (dinfo.output_scanline < dinfo.output_height) {
			int y = dinfo.output_scanline;
			jpeg_read_scanlines(&dinfo, &line, 1);
			int x;
			for (x = 0; x < width; x++) {
				u32 c = line[x];
				image->data[x + image->textureWidth * y] = c | (c << 8) | (c << 16) | 0xff000000;
			}
		}
	} else {
		while (dinfo.output_scanline < dinfo.output_height) {
			int y = dinfo.output_scanline;
			jpeg_read_scanlines(&dinfo, &line, 1);
			u8* linePointer = line;
			int x;
			for (x = 0; x < width; x++) {
				u32 c = *(linePointer++);
				c |= (*(linePointer++)) << 8;
				c |= (*(linePointer++)) << 16;
				image->data[x + image->textureWidth * y] = c | 0xff000000;
			}
		}
	}
	jpeg_finish_decompress(&dinfo);
	jpeg_destroy_decompress(&dinfo);
	geFree(((ge_jpeg_src*)dinfo.src)->buffer);
	geFree(dinfo.src);
	geFree(line);

	return image;
}

void ge_init_jpeg_src(struct jpeg_decompress_struct* dinfo, ge_File* file){
	dinfo->src = (struct jpeg_source_mgr*)geMalloc(sizeof(ge_jpeg_src));

	ge_jpeg_src* src = (ge_jpeg_src*)dinfo->src;
	src->file = file;
	src->buffer = (u8*)geMalloc(INPUT_BUF_SIZE);
	src->start_of_file = true;

	src->jsrc.next_input_byte = NULL;
	src->jsrc.bytes_in_buffer = 0;
	src->jsrc.init_source = init_source;
	src->jsrc.fill_input_buffer = file_fill_input_buffer;
	src->jsrc.skip_input_data = skip_input_data;
	src->jsrc.resync_to_restart = jpeg_resync_to_restart;
	src->jsrc.term_source = term_source;
}

static void init_source(j_decompress_ptr cinfo){
}

static boolean file_fill_input_buffer(j_decompress_ptr cinfo){
	ge_jpeg_src* src = (ge_jpeg_src*)cinfo->src;

	int nbytes = geFileRead(src->file, src->buffer, INPUT_BUF_SIZE);

	if(nbytes <= 0){
		if(src->start_of_file){ //It's an error
			return false;
		}
		WARNMS(cinfo, JWRN_JPEG_EOF);
		/* Insert a fake EOI marker */
		src->buffer[0] = (JOCTET) 0xFF;
		src->buffer[1] = (JOCTET) JPEG_EOI;
		nbytes = 2;
	}

	src->jsrc.next_input_byte = src->buffer;
	src->jsrc.bytes_in_buffer = nbytes;
	src->start_of_file = false;

	return true;
}
/*
static int mem_fill_input_buffer(j_decompress_ptr cinfo){
	JOCTET eoi_buffer[2] = { 0xFF, JPEG_EOI };
	struct jpeg_source_mgr *jsrc = cinfo->src;

	// Create a fake EOI marker
	jsrc->next_input_byte = eoi_buffer;
	jsrc->bytes_in_buffer = 2;

	return true;
}
*/
static void skip_input_data (j_decompress_ptr cinfo, long num_bytes){
	struct jpeg_source_mgr *jsrc = cinfo->src;

	if(num_bytes > 0){
		while (num_bytes > (long)jsrc->bytes_in_buffer){
			num_bytes -= (long)jsrc->bytes_in_buffer;
			file_fill_input_buffer(cinfo);
		}
		jsrc->next_input_byte += num_bytes;
		jsrc->bytes_in_buffer -= num_bytes;
	}
}

static void term_source(j_decompress_ptr cinfo){
}

#include "ge_internal.h"
#include <ft2build.h>
#include <freetype2/freetype/freetype.h>
#include <freetype2/freetype/ftglyph.h>
void CreateGlTexture(ge_Image* image);
#ifndef min
# define min(a, b) (a < b ? a : b)
#endif
FT_Library ft_library;
static void fontPrintTextImage(FT_Bitmap* bitmap, int x, int y, u32 color, ge_Image* dest);
void TransformUnicodeChar(long* chr);

FT_GlyphSlot slot;
ge_Font* geLoadFontTTF(ge_File* fontFile){
	FT_Error initError;

	initError = FT_Init_FreeType(&ft_library);
  	if(initError)return 0;

	ge_Font* font = (ge_Font*) geMalloc(sizeof(ge_Font));

	geFileSeek(fontFile, 0, GE_FILE_SEEK_END);
	int filesize = geFileTell(fontFile);
	u8* fontData = (u8*)geMalloc(filesize);
	if(!fontData){
		geFileClose(fontFile);
		geFree(font);
		return NULL;
	}
	geFileRewind(fontFile);
	geFileRead(fontFile, fontData, filesize);

	FT_Face face;
	int error = FT_New_Memory_Face(ft_library, fontData, filesize, 0, &face);
	if(error){
		geFree(font);
		geFree(fontData);
		return NULL;
	}

	font->face = face;
	font->data = fontData;
	font->texture = NULL;
//	strncpy(font->name, fontFile->filename, 256);

	FT_Select_Charmap(((FT_Face)font->face), FT_ENCODING_UNICODE);
	/*
	gePrintDebug(0x100, "Font1: 0x%08X\n", (u32)font);
	gePrintDebug(0x100, "Font1 face0: 0x%08X\n", (u32)face);
	gePrintDebug(0x100, "Font1 face: 0x%08X\n", (u32)font->face);
	gePrintDebug(0x100, "Font1 glyph: 0x%08X\n", (u32)((FT_Face)font->face)->glyph);
	*/
	font->type = GE_FONT_TTF;
	return font;
}
void FontSetSizeTTF(ge_Font* font, int px){
	FT_Set_Pixel_Sizes((FT_Face)font->face, 0, px);
}

static void fontPrintTextImpl2(FT_Bitmap* bitmap, int xofs, int yofs, u32 color, u32* framebuffer, int width, int height, int lineSize){
	int x, y;

	u8* line = bitmap->buffer;
	u32* fbLine = framebuffer + xofs + yofs * lineSize;
	for (y = 0; y < bitmap->rows; y++){
		u8* column = line;
		u32* fbColumn = fbLine;
		for (x = 0; x < bitmap->width; x++){
			if (x + xofs < width && x + xofs >= 0 && y + yofs < height && y + yofs >= 0){
				u8 val = *column;
				*fbColumn = RGBA(255, 255, 255, val);
			}
			column++;
			fbColumn++;
		}
		line += bitmap->pitch;
		fbLine += lineSize;
	}
}

void geCreateFontTtf(ge_Font* font){
	if(font->size <= 0)return;
	FT_GlyphSlot slot = ((FT_Face)font->face)->glyph;
	u16 n;
	u16 chr = 0;
	int x = 0;
	int y = font->size;
	bool first_null_char = true;

	int total_width = 0;

	for(n=0; n<256; n++){
		chr = n;
		FT_UInt glyph_index = FT_Get_Char_Index((FT_Face)font->face, chr);
		int error = FT_Load_Glyph((FT_Face)font->face, glyph_index, FT_LOAD_DEFAULT);
		if (error) continue;
		error = FT_Render_Glyph(((FT_Face)font->face)->glyph, ft_render_mode_normal);
		if (error) continue;
		if(!glyph_index && !first_null_char){
			continue;
		}
		total_width += slot->bitmap.width;
		first_null_char = false;
	}

	if(font->texture)geFreeImage(font->texture);

#ifdef PSP
	ge_Image* dest = geCreateSurface(512, 512, 0x00000000);
#else
//	ge_Image* dest = geCreateSurface(1024, 1024, 0x00000000);
	ge_Image* dest = geCreateSurface(font->size*16*1.5, font->size*16*1.5, 0x00000000);
#endif

	font->texture = dest;
	first_null_char = true;

	for(n=0; n<256; n++){
		chr = n;
		FT_UInt glyph_index = FT_Get_Char_Index((FT_Face)font->face, chr);
		int error = FT_Load_Glyph((FT_Face)font->face, glyph_index, FT_LOAD_DEFAULT);
		if (error) continue;
		error = FT_Render_Glyph(((FT_Face)font->face)->glyph, ft_render_mode_normal);
		if (error) continue;

		if(x+(slot->advance.x>>6) > dest->width){
			x = 0;
			y += font->size * 1.5;
		}

		font->positions[n].x = x;
		if(font->positions[n].x<0)font->positions[n].x=0;
		font->positions[n].y = y - slot->bitmap_top;
		font->positions[n].w = slot->bitmap.width + slot->bitmap_left;
		font->positions[n].h = slot->bitmap.rows;
		font->positions[n].advX = slot->advance.x >> 6;
		font->positions[n].posY = slot->bitmap_top;
		if(!glyph_index && !first_null_char){
			continue;
		}

//		fontPrintTextImage(&slot->bitmap, x + slot->bitmap_left, y - slot->bitmap_top, 0xffffffff, dest);
		fontPrintTextImpl2(&slot->bitmap, x + slot->bitmap_left, y - slot->bitmap_top, 0xffffffff, dest->data, dest->width, dest->height, dest->textureWidth);

		/*
		int jump = slot->bitmap.width;
		int jump2 = slot->advance.x >> 6;
		if(jump2 > jump){
			jump = jump2;
		}
		jump = slot->bitmap.width + (slot->advance.x >> 6);
		x += jump;
		*/
	//	x += slot->advance.x >> 6;
		x += font->size;

	//	y += slot->advance.y >> 6;
		first_null_char = false;
	}
//	geSwizzle(dest);
	geUpdateImage(dest);
}

void FontPrintImageTtf_short(ge_Font* font, int x, int y, const unsigned short* text, u32 color, ge_Image* image){
	FT_GlyphSlot slot = ((FT_Face)font->face)->glyph;

	int b_x = x;
	int n = 0;
	long ch = 0;

	for (n = 0; text[n] != 0x0; n++) {
	//	gePrintDebug(0x100, "char: %d\n", n);
		ch = (u16)text[n];
		if(ch == '\n'){
			y += font->size;
			x = b_x;
			continue;
		}
		if(ch >= 0x80){
			TransformUnicodeChar(&ch);
		}

		FT_UInt glyph_index = FT_Get_Char_Index(((FT_Face)font->face), ch);
		FT_Load_Glyph(((FT_Face)font->face), glyph_index, FT_LOAD_DEFAULT);

		FT_Glyph glyph;
		FT_Get_Glyph(((FT_Face)font->face)->glyph, &glyph);

		FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
		slot = ((FT_Face)font->face)->glyph;
		
		FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;
		FT_Bitmap bitmap = bitmap_glyph->bitmap;

		fontPrintTextImage(&bitmap, x+bitmap_glyph->left, y+font->size-bitmap_glyph->top, color, image);

		x += slot->advance.x >> 6;
	}
	geUpdateImage(image);
}

void FontPrintScreenTtf_short(ge_Font* font, int x, int y, const unsigned short* text, u32 color){
	FT_GlyphSlot slot = ((FT_Face)font->face)->glyph;
	ge_Image* image = libge_context->fontbuf;
	memset(image->data, 0x0, image->textureWidth*image->textureHeight*sizeof(u32));

	int b_x = x;
	int n = 0;
	long ch = 0;
	
	for (n = 0; text[n] != 0x0; n++) {
	//	gePrintDebug(0x100, "char: %d\n", n);
		ch = (u16)text[n];
		if(ch == 0x0){
			break;
		}
		if(ch == '\n'){
			y += font->size;
			x = b_x;
			continue;
		}
		if(ch >= 0x80){
		//	TransformUnicodeChar(&ch);
		}

		FT_UInt glyph_index = FT_Get_Char_Index(((FT_Face)font->face), ch);
		FT_Load_Glyph(((FT_Face)font->face), glyph_index, FT_LOAD_DEFAULT);

		FT_Glyph glyph;
		FT_Get_Glyph(((FT_Face)font->face)->glyph, &glyph);

		FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
		slot = ((FT_Face)font->face)->glyph;
		
		FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;
		FT_Bitmap bitmap = bitmap_glyph->bitmap;

		fontPrintTextImage(&bitmap, x+bitmap_glyph->left, y+font->size-bitmap_glyph->top, color, image);

		x += slot->advance.x >> 6;
	}
	
	geUpdateImage(image);
	geBlitImage(0, 0, image, 0, 0, image->width, image->height, 0);
}

void geFontPrintImageTtf(ge_Font* font, int x, int y, const char* text, u32 color, ge_Image* image){
}

void FontMeasureText_short(ge_Font* font, const unsigned short* text, int* width, int* height){
	int mx = 0;
	int x = 0;
	int y = 0;
	int n = 0;
	long ch = 0;
	
	for (n = 0; ; n++) {
	//	gePrintDebug(0x100, "char: %d\n", n);
		ch = (u16)text[n];
		if(ch == 0x0){
			if(x > mx){
				mx = x;
			}
			break;
		}
		if(ch == '\n'){
			y += font->size;
			if(x > mx){
				mx = x;
			}
			x = 0;
			continue;
		}
		if(ch >= 0x80){
		//	TransformUnicodeChar(&ch);
		}

		FT_UInt glyph_index = FT_Get_Char_Index(((FT_Face)font->face), ch);
		FT_Load_Glyph(((FT_Face)font->face), glyph_index, FT_LOAD_DEFAULT);

		FT_Glyph glyph;
		FT_Get_Glyph(((FT_Face)font->face)->glyph, &glyph);

		FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
		slot = ((FT_Face)font->face)->glyph;

		x += slot->advance.x >> 6;
	}

	*width = mx;
	*height = y + font->size + (font->size * 0.4);
}

static void fontPrintTextImpl(FT_Bitmap* bitmap, int xofs, int yofs, u32 text_color, u32* framebuffer, int width, int height, int lineSize){
	int x, y;
	
	u8* line = bitmap->buffer;
	u32* fbLine = framebuffer + xofs + yofs * lineSize;
	for (y = 0; y < bitmap->rows; y++) {
		u8* column = line;
		u32* fbColumn = fbLine;
		for (x = 0; x < bitmap->width; x++) {
			if (x + xofs < width && x + xofs >= 0 && y + yofs < height && y + yofs >= 0) {
				u8 val = *column;
				u32 color = *fbColumn;

				float f_val = ((float)val) / 255.0;
				f_val *= Af(text_color);

				float r = min(1.0, Rf(text_color) * f_val + Rf(color) * (1.0 - f_val));
				float g = min(1.0, Gf(text_color) * f_val + Gf(color) * (1.0 - f_val));
				float b = min(1.0, Bf(text_color) * f_val + Bf(color) * (1.0 - f_val));
				float a = min(1.0, Af(color) + f_val);

				*fbColumn = RGBAf(r, g, b, a);
			}
			column++;
			fbColumn++;
		}
		line += bitmap->pitch;
		fbLine += lineSize;
	}

//	gePrintDebug(0x100, "fontPrintTextImpl(bitmap, %d, %d, 0x%8.8X, framebuffer, %d, %d, %d)\n", xofs, yofs, color, width, height, lineSize);
	/*
	u8* line = bitmap->buffer;
	u32* fbLine = framebuffer + xofs + yofs * lineSize;
	for (y = 0; y < bitmap->rows; y++) {
		u8* column = line;
		u32* fbColumn = fbLine;
		for (x = 0; x < bitmap->width; x++) {
			if (x + xofs < width && x + xofs >= 0 && y + yofs < height && y + yofs >= 0) {
				u8 val = *column;
				color = *fbColumn;
				u8 r = color & 0xff;
				u8 g = (color >> 8) & 0xff;
				u8 b = (color >> 16) & 0xff;
				u8 a = (color >> 24) & 0xff;
				r = rf * val / 255 + (255 - val) * r / 255;
				g = gf * val / 255 + (255 - val) * g / 255;
				b = bf * val / 255 + (255 - val) * b / 255;
				a = af * val / 255 + (255 - val) * a / 255;
				*fbColumn = RGBA(r, g, b, a);
			//	*fbColumn = RGBA(255, 255, 255, val);
			}
			column++;
			fbColumn++;
		}
		line += bitmap->pitch;
		fbLine += lineSize;
	}
	*/
}

static void fontPrintTextImage(FT_Bitmap* bitmap, int x, int y, u32 color, ge_Image* dest){
	fontPrintTextImpl(bitmap, x, y, color, dest->data, dest->width, dest->height, dest->textureWidth);
}

void TransformUnicodeChar(long* chr){
	if(*chr == 0x80){
		*chr = 0x20AC;
		return;
	}
}

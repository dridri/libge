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

#ifndef _GE_FONT_H_
#define _GE_FONT_H_


#include "getypes.h"
//#define GE_FONT_PGF 0
#define GE_FONT_TTF 1
#define GE_FONT_SYS 0xFF
#define GE_FONT_ENCODING_UTF8 0x08
#define GE_FONT_ENCODING_UTF16 0x16

#define GE_FONT_ALIGN_LEFT 0x0
#define GE_FONT_ALIGN_CENTER 0x1
#define GE_FONT_ALIGN_RIGHT 0x2
#define GE_FONT_ALIGN_SCROLL_LEFT 0x04
#define GE_FONT_ALIGN_SCROLL_RIGHT 0x08

/*
ge_Font* geLoadIntraFont(const char* file);
void geIntraFontSize(ge_Font* font, int size);
void geIntraFontShadowColor(ge_Font* font, u32 scolor);
void geIntraFontPrintScreen(int x, int y, ge_Font* font, const char* text, u32 color);
void geFreeIntraFont(ge_Font* font);
void geFontPrintScreenScroll(int x, int y, ge_Font* font, const char* text, u32 color, int* _ptr);
*/

LIBGE_API ge_Font* geLoadFont(const char* file);
LIBGE_API void geFreeFont(ge_Font* font);
LIBGE_API void geFontSize(ge_Font* font, int px);
LIBGE_API void geFontEncoding(ge_Font* font, int encod);
LIBGE_API void geFontAlignment(ge_Font* font, u32 align);

LIBGE_API void geFontPrintScreen(int x, int y, ge_Font* font, const char* text, u32 color);
LIBGE_API void geFontPrintScreenUnicode(int x, int y, ge_Font* font, const unsigned short* text, u32 color);
LIBGE_API void geFontPrintScreenOutline(int x, int y, ge_Font* font, const char* text, u32 color, u32 outlineColor);

LIBGE_API void geFontPrintImage(int x, int y, ge_Font* font, const char* text, u32 color, ge_Image* image);
LIBGE_API void geFontPrintImageUnicode(int x, int y, ge_Font* font, const unsigned short* text, u32 color, ge_Image* image);

LIBGE_API void geFontPrintfScreen(int x, int y, ge_Font* font, u32 color, const char* fmt, ...);
LIBGE_API void geFontPrintfImage(int x, int y, ge_Font* font, u32 color, ge_Image* image, const char* fmt, ...);

LIBGE_API ge_Image* geFontMakeImage(ge_Font* font, const char* text, u32 color);
LIBGE_API ge_Image* geFontMakeImageUnicode(ge_Font* font, const unsigned short* text, u32 color);

LIBGE_API void geFontMeasureTextUnicode(ge_Font* font, const unsigned short* text, int* width, int* height);
LIBGE_API void geFontMeasureText(ge_Font* font, const char* text, int* width, int* height);


#endif

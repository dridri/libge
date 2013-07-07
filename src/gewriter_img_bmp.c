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

bool SaveBmp_RGB(ge_File* fp, u8* header, ge_Image* image, int width, int height, int bpp);
//bool SaveBmp_Palette(ge_File* fp, u8* header, ge_Image* image, int width, int height, int bpp);



void geSaveBmp(ge_File* fp, ge_Image* img){
}

bool SaveBmp_RGB(ge_File* fp, u8* header, ge_Image* image, int width, int height, int bpp){
	return true;
}

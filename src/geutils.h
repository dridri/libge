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

#ifndef _H_GE_UTILS_
#define _H_GE_UTILS_

#include "getypes.h"

#define GE_COMPRESSED 0x00010000

#define GE_COMPRESSION_ZLIB 0
#define GE_COMPRESSION_GZIP 1

LIBGE_API ge_Buffer* geDecompress(void* in, int insz, int mode);
LIBGE_API ge_Buffer* geCompress(void* in, int insz, int mode);

#endif // _H_GE_UTILS_

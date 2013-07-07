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

#ifndef _H_GE_SYS_
#define _H_GE_SYS_

#include "getypes.h"


typedef struct ge_Buffer {
	void* ptr;
	u32 size;
	u32 cursor;
} ge_Buffer;

LIBGE_API int geBufferRead(ge_Buffer* buffer, void* data, int len);
LIBGE_API int geBufferWrite(ge_Buffer* buffer, void* data, int len);
LIBGE_API void geBufferEncryptIndex(ge_Buffer* buffer, u8* key, int key_size, int key_index);
LIBGE_API void geBufferDecryptIndex(ge_Buffer* buffer, u8* key, int key_size, int key_index);
LIBGE_API void geBufferEncrypt(ge_Buffer* buffer, u8* key, int key_size);
LIBGE_API void geBufferDecrypt(ge_Buffer* buffer, u8* key, int key_size);
LIBGE_API void gePrintBuffer(void* buf, int buflen);



LIBGE_API int geGetParamString(const char* buffer, const char* attrib, char* dest, int max);
LIBGE_API int geGetStringList(const char* buffer, char** dest, int max_len, int max_dest);
LIBGE_API int geGetParamStringMulti(const char* buffer, char* separators, char** dest, int max_len, int max_dest);

LIBGE_API bool geGetParamInt(const char* buffer, const char* attrib, int* dest);
LIBGE_API int geGetParamIntMulti(const char* buffer, const char* attrib, int* dest, int num);

LIBGE_API bool geGetParamDouble(const char* buffer, const char* attrib, double* dest);

LIBGE_API bool geGetParamFloat(const char* buffer, const char* attrib, float* dest);
LIBGE_API int geGetParamFloatMulti(const char* buffer, const char* attrib, float* dest, int num);

LIBGE_API int geGetNumCPU();

typedef struct ge_Date {
	unsigned short year;
	unsigned short month;
	unsigned short day;
	unsigned short hour;
	unsigned short minute;
	unsigned short second;
} ge_Date;

#endif

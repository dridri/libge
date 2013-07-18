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

#ifndef __GE_H_FILE__
#define __GE_H_FILE__

#include "getypes.h"
#include "gesys.h"

#define GE_FILE_MODE_READ 0x2// r
#define GE_FILE_MODE_WRITE 0x4// w
#define GE_FILE_MODE_APPEND 0x6// a
#define GE_FILE_MODE_READWRITE 0x8// r+
#define GE_FILE_MODE_CLEAR_RW 0x10// w+
#define GE_FILE_MODE_APPEND_RW 0x20// a+
#define GE_FILE_MODE_TEXT 0x40
#define GE_FILE_MODE_BINARY 0x80 // b
#define GE_FILE_MODE_ENCRYTPTED 0x1000
#define GE_FILE_MODE_VIRTUAL 0x2000

#define GE_FILE_TYPE_NORMAL 0x00
#define GE_FILE_TYPE_BUFFER 0x01
#define GE_FILE_TYPE_PART	0x02

#define GE_FILE_SEEK_SET 0
#define GE_FILE_SEEK_CUR 1
#define GE_FILE_SEEK_END 2

typedef struct ge_File {
	char filename[2048];
	int mode;
	int type;
	void* fd;
	u8* buffer;
	void* desc;
} ge_File;

typedef struct ge_Stat {
	int isDir;
	int size;
	ge_Date ctime;
	ge_Date atime;
	ge_Date mtime;
} ge_Stat;

typedef struct ge_Dir {
	char dirname[2048];
	void* fd;
} ge_Dir;
typedef struct ge_Dirent {
	char name[256];
	ge_Stat stat;
} ge_Dirent;

LIBGE_API char* geFileFromBuffer(void* buf, int size);
LIBGE_API char* gePathFromFile(ge_File* file);
LIBGE_API bool geFileExists(const char* filename);

LIBGE_API ge_File* geFileOpen(const char* filename, int mode);
LIBGE_API ge_File* geFileOpenFd(int fd, int mode);
LIBGE_API ge_File* geFileBuffer(const void* buf, int size, int mode);
LIBGE_API ge_File* geFilePart(const char* filename, const char* name);
LIBGE_API void geFileClose(ge_File* file);
LIBGE_API void geFileSetKey(ge_File* file, u8* key, int len);
LIBGE_API void geFileOffset(ge_File* file, int offset);

LIBGE_API void geFileRewind(ge_File* file);
LIBGE_API void geFileSeek(ge_File* file, int offset, int origin);
LIBGE_API int geFileTell(ge_File* file);

LIBGE_API char geFileGetChar(ge_File* file);
LIBGE_API int geFileGets(ge_File* file, char* buffer, int max);
LIBGE_API int geFileRead(ge_File* file, void* buffer, int size);
LIBGE_API int geFileWrite(ge_File* file, void* buffer, int size);


LIBGE_API ge_Dir* geDirOpen(const char* dirname);
LIBGE_API int geDirReadNext(ge_Dir* dir, ge_Dirent* entry);
LIBGE_API char** geDirList(ge_Dir* dir, int* count);
LIBGE_API void geDirClose(ge_Dir* dir);
LIBGE_API void geDirRewind(ge_Dir* dir);


LIBGE_API void geHomePath(char* s, int max);

#endif //__GE_H_FILE__

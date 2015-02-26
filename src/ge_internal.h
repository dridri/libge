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

#ifndef _GE_INTERNAL_H_
#define _GE_INTERNAL_H_

#include "libge.h"

//#define PLATFORM psp

#ifdef PLATFORM_psp
#include "psplibs/valloc/valloc.h"
#include "psplibs/vfpu/pspvfpu.h"
#include "system/psp/ge_context.h"
#endif
#ifdef PLATFORM_win_low
#include "system/win_low/ge_context.h"
#endif
#ifdef PLATFORM_win
#include "system/win/ge_context.h"
#endif
#ifdef PLATFORM_mac
#include "system/mac/ge_context.h"
#endif
#ifdef PLATFORM_linux_low
#include "system/linux_low/ge_context.h"
#endif
#ifdef PLATFORM_linux
#include "system/linux/ge_context.h"
#endif
#ifdef PLATFORM_android
#include "system/android/ge_context.h"
#endif
#ifdef PLATFORM_ps3
#include "system/ps3/ge_context.h"
#endif
#ifdef PLATFORM_psv
#include "system/psv/ge_context.h"
#endif
#ifdef PLATFORM_qt
#include "system/qt/ge_context.h"
#endif

#ifndef CONTEXT_FILE
	extern LibGE_Context* libge_context;
	#ifdef PSP
		extern u32 ge_edram_address;
	#endif
#endif

#define set_color(c, r,g,b,a)c[0]=r;c[1]=g;c[2]=b;c[3]=a

#if (defined(PLATFORM_win) || defined(PLATFORM_win_low))
#if (ARCH == 64)
#define GE_BLOB(x) _binary_##x
#else
#define GE_BLOB(x) binary_##x
#endif
#else
#define GE_BLOB(x) _binary_##x
#endif

void ge_Splashscreen();
void geGraphicsInit();
void geInitFps();
void ge_Fps_Routine();
void geDebugTerm();

void ge_PauseAllThreads();
void ge_ResumeAllThreads();

void geMatrixLocations();
void CalculateModelMatrices();
ge_Image* geAddTexture(ge_Image* tex);
void geSceneSetup(ge_Scene* scene);
char* _ge_BuildPath(const char* path, const char* filename);

typedef struct ge_type32_struct {
	u32 type;
} ge_type32_struct;

typedef struct ge_file_desc {
	int start;
	int offset;
	int size;
	int fake_offset;
} ge_file_desc;

typedef struct ge_data_file {
	u8 mini_magic;
	char magic[31];
	u8 version_min;
	u8 version_maj;
	u8 blank[10];
	u16 encrypted;
	u16 key_len;
	u32 num_files;
} __attribute__((packed)) ge_data_file;

typedef struct ge_geo_vertex {
	float u, v, w;
	u32 color;
	float nx, ny, nz;
	float x, y, z;
} __attribute__((packed)) ge_geo_vertex;

typedef struct ge_geo_texture {
	s32 id;
	s32 flags;
	u32 color;
	s32 width;
	s32 height;
	s32 datasize;
} __attribute__((packed)) ge_geo_texture;

typedef struct ge_geo_object {
	char name[64];
	float matrix[16];
	u32 diffuse;
	u32 ambient;
	u32 specular;
	float bump_force;
	s32 nTextures;
	s32 nVertices;
	s32 texturesOfs;
	s32 verticesOfs;
	s32 verticesSize;
} __attribute__((packed)) ge_geo_object;

typedef struct ge_geo_header {
	u32 magic;
	u16 version_min;
	u16 version_maj;
	s32 nObjs;
	u16 compressed;
	u8 blank[18];
} __attribute__((packed)) ge_geo_header;



void* geSysFileOpen(const char* filename, int mode);
void* geSysFileOpenFd(int fd, int mode);
void geSysFileClose(void* file);
int geSysFileSeek(void* file, int offset, int origin);
int geSysFileTell(void* file);
char geSysFileGetc(void* file);
char* geSysFileGets(void* file, char* buffer, int max);
int geSysFileRead(void* file, void* buffer, int size);
int geSysFileWrite(void* file, void* buffer, int size);

void* geSysDopen(const char* file);
void geSysDclose(void* fd);
int geSysDread(void* fd, char* path, ge_Dirent* entry);


#endif

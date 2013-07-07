/*
	The Game Engine Library is a multiplatform library made to make games
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

#include "ge_viddrv.h"

typedef struct LibGE_DisplayList {
	u32* start;
	u32* current;
	u32* last;
	u32* defaultList;
} LibGE_DisplayList;

typedef struct LibGE_DrawBuffer {
	int pixel_size;
	int frame_width;
	u32* frame_buffer;
	u32* disp_buffer;
	u32* depth_buffer;
	int depth_width;
	int width;
	int height;
} LibGE_DrawBuffer;

typedef struct LibGE_ClearVertex {
	u32 color;
	short x,y,z;
	short pad;
} LibGE_ClearVertex;

typedef struct LibGE_PspContext {
	struct pspvfpu_context* vfpu_context;
	LibGE_DrawBuffer draw_buffer;
	LibGE_DisplayList dList;
	u32 ge_edram_address;
	int ge_list_executed[2];
	LibGE_DisplayList* clear_list;
	LibGE_ClearVertex* clear_vertices;
	ge_Image* cursor;
} LibGE_PspContext;

void geInitVideo();
int SystemSwapBuffers();
void geKeyMoveCursor(int x, int y);
void geCursorRender();

void geCreateContext();
void geSendCommandi(int cmd, int args);
void geSendCommandf(int cmd, float args);
inline void geListSendCommandi(LibGE_DisplayList* dList, int cmd, int args);
inline void geListSendCommandf(LibGE_DisplayList* dList, int cmd, float args);
void* geGetMemory(int size);
void* geListGetMemory(LibGE_DisplayList* dList, int size);
void geDrawSync();


void geMakeClearList();


void geSetupFunc(void (*func)());
void geIndirectListMode(int m);
u32* geGetIndirectList(int* sz);
void geSendList(LibGE_DisplayList* list);
void geCallList(LibGE_DisplayList* list);

void geDrawBuffer(int psm, void* pointer, int frame_width);
void geDisplayBuffer(void* pointer, int width, int height);
void geDepthBuffer(void* pointer, int frame_width);
void geScissor(int x, int y, int ex, int ey);
void geSetDither(const ScePspIMatrix4* matrix);

void geEnable(int what);
void geDisable(int what);
void geAmbientColor(u32 color);

void geOffset(u32 x, u32 y);
void geViewport(int x, int y, int width, int height);
void geDepthRange(int near, int far);
void geDepthFunc(int func);
void geFrontFace(int order);
void geShadeModel(int mode);

void geDrawType(int type);
void geDrawMode(int mode);
void geDrawArrayPointer(void* vertices);
void geDrawIndicesPointer(void* indices);
void geDrawCount(int count);
void geDraw();
void geDrawArray(int type, void* vertices, int count);
void geDrawArrayFull(int type, int mode, void* vertices, int count);

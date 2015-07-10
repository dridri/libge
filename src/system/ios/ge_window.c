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

#include "../../ge_internal.h"
#include <stdlib.h>

void iOSOpenWindow(int* width, int* height, int flags);
void iOSSwapBuffer(void);
void iOSSetWarpMode(int en);
void iOSGetMousePos(int*x, int* y);

bool initializing = false;
static int cbuffer = 0;
static int last_pressed = 0;

void _ge_exit(){
	LibGE_iOSContext* context = (LibGE_iOSContext*)libge_context->syscontext;
}

int geCreateMainWindow(const char* title, int Width, int Height, int flags){
	initializing = true;
	int width = Width;
	int height = Height;

	iOSOpenWindow(&width, &height, flags);

	gePrintDebug(0x100, "Current OpenGL version: %s\n", (const char*)glGetString(GL_VERSION));
	geInitVideo();
	geInitShaders();
	geGraphicsInit();
	geDrawingMode(GE_DRAWING_MODE_2D);

	atexit(_ge_exit);

	initializing = false;
	return 0;
}

void geSetIcon(ge_Image* icon){
}

void geFullscreen(bool fullscreen, int width, int height){
}

void CloseFullScreen(){
}

void geWaitVsync(int enabled){
}

void _ge_mac_resize(int w, int h){
	libge_context->width = w;
	libge_context->height = h;
	libge_context->projection_matrix[0] = (float)0xFFFFFFFF;
	geGraphicsInit();
	geDrawingMode(libge_context->drawing_mode | 0xF0000000);
}

int iOSSwapBuffers(){
	int x = 0;
	int y = 0;
	iOSSwapBuffer();
	iOSGetMousePos(&x, &y);
	libge_context->mouse_x = x;
	libge_context->mouse_y = y;
	return cbuffer ^= 1;
}

void geCursorPosition(int* x, int* y){
	*x = libge_context->mouse_x;
	*y = libge_context->mouse_y;
}

void geCursorWarp(int* x, int* y){
}

void geCursorRoundMode(bool active){
	libge_context->mouse_round = active;
	iOSSetWarpMode(active);
	if(active){
		LibGE_iOSContext* context = (LibGE_iOSContext*)libge_context->syscontext;
	}
}

int iOSGetLastPressed(){
	int ret = last_pressed;
	last_pressed = 0;
	return ret;
}

void geQuit(){
	exit(0);
}

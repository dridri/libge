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

void MacOpenWindow(int* width, int* height, int flags);
void MacSwapBuffer(void);
void MacSetWarpMode(int en);
void MacGetMousePos(int*x, int* y);

bool initializing = false;
static int cbuffer = 0;
extern int _ge_mac_mouse_warp_x;
extern int _ge_mac_mouse_warp_y;
static int last_pressed = 0;

void _ge_exit(){
	LibGE_MacContext* context = (LibGE_MacContext*)libge_context->syscontext;
}

int geCreateMainWindow(const char* title, int Width, int Height, int flags){
	initializing = true;
	int width = Width;
	int height = Height;

	MacOpenWindow(&width, &height, flags);

	libge_context->width = width;
	libge_context->height = height;

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

int MacSwapBuffers(){
	MacSwapBuffer();
	MacGetMousePos(&libge_context->mouse_x, &libge_context->mouse_y);
	return cbuffer ^= 1;
}

void geCursorPosition(int* x, int* y){
	*x = libge_context->mouse_x;
	*y = libge_context->mouse_y;
}

void geCursorWarp(int* x, int* y){
	*x = _ge_mac_mouse_warp_x;
	*y = _ge_mac_mouse_warp_y;
}

void geCursorRoundMode(bool active){
	libge_context->mouse_round = active;
	MacSetWarpMode(active);
	if(active){
		LibGE_MacContext* context = (LibGE_MacContext*)libge_context->syscontext;
	}
}

int MacGetLastPressed(){
	int ret = last_pressed;
	last_pressed = 0;
	return ret;
}

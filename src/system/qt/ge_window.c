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

void LinuxInit();
void CloseFullScreen();

bool initializing = false;

extern void QtInit(LibGE_QtContext* context);

void _ge_exit(){
}

int geCreateMainWindow(const char* title, int Width, int Height, int flags){
	initializing = true;

	LibGE_QtContext* context = (LibGE_QtContext*)geMalloc(sizeof(LibGE_QtContext));
	libge_context->syscontext = (unsigned long)context;

	libge_context->width = Width;
	libge_context->height = Height;
	context->fs = flags & GE_WINDOW_FULLSCREEN;

	int nSamples = 1;
	if(flags & GE_WINDOW_MSAA2X){
		nSamples = 2;
	}
	if(flags & GE_WINDOW_MSAA4X){
		nSamples = 4;
	}
	if(flags & GE_WINDOW_MSAA8X){
		nSamples = 8;
	}

	QtInit(context);

	geWaitVsync(true);

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

void geCursorPosition(int* x, int* y){
	*x = libge_context->mouse_x;
	*y = libge_context->mouse_y;
}

void geCursorWarp(int* x, int* y){
}

void geCursorRoundMode(bool active){
	libge_context->mouse_round = active;
	if(active){
		//TODO
	}
}

void geCursorVisible(bool visible){
	LibGE_QtContext* context = (LibGE_QtContext*)libge_context->syscontext;
	if(!visible){
	}else{
	}
}

void LinuxGetPressedKeys(u8* k){
//	memcpy(k, keys_pressed, GE_KEYS_COUNT*sizeof(u8));
}

int LinuxGetLastPressed(){
	return 0;
}

void geQuit(){
	exit(0);
}

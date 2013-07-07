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

#include <windows.h>
#include "../../video/opengl21/ge_viddrv.h"

typedef struct LibGE_WinlowContext {
	int flags;
	int maxw, maxh;
	int winx, winy;
	HWND window;
	HDC hDC;
	HGLRC hRC;
	HINSTANCE instance;
	HINSTANCE hOpenGL;
} LibGE_WinlowContext;

void geInitVideo();
int geInitShaders();
int SystemSwapBuffers();
void WindowsGetPressedKeys(unsigned char* k);

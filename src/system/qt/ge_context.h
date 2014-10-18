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

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/extensions/xf86vmode.h>
#include <GL/glx.h>
#ifdef LIBGE_OPENGL30
#include "../../video/opengl30/ge_viddrv.h"
#else
#include "../../video/opengl20/ge_viddrv.h"
#endif

typedef struct LibGE_QtContext {
	int flags;
	int maxw, maxh;
	int x, y;
	int depth;

	bool fs;
	void* qt;
	void* widget;
} LibGE_QtContext;

void geInitVideo();
int geInitShaders();
int SystemSwapBuffers();
void QtGetPressedKeys(unsigned char* k);

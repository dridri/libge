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

#ifndef _GE_H_CONTEXT_
#define _GE_H_CONTEXT_

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <android/native_activity.h>
#include <android_native_app_glue.h>
#include "../../video/android/ge_viddrv.h"

typedef struct LibGE_AndroidContext {
	int flags;
	int width;
	int height;
	int depth;
	ge_Image* cursor;
	
	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
	EGLConfig config;
	struct android_app* state;
} LibGE_AndroidContext;

void geInitVideo();
int geInitShaders();
int SystemSwapBuffers();
void AndroidReadKeys(ge_Keys* keys);

LibGE_AndroidContext* _ge_GetAndroidContext();

#endif // _GE_H_CONTEXT_

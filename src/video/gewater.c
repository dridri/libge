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

#include "../ge_internal.h"
#include <stdio.h>
#include <stdlib.h>
float* ge_GetVerticalView();

static void ge_MakeReflection(ge_Water* water, ge_Camera* cam, ge_Image* texture, void (*cb)(void*), void* cb_data){
	u32 last_mode = geClearMode(GE_CLEAR_DEPTH_BUFFER | GE_CLEAR_COLOR_BUFFER);
	geClearScreen();

	geMatrixMode(GE_MATRIX_VIEW);
	geLoadIdentity();
	geLookAt(cam->x,cam->y,cam->z, cam->cX,cam->cY,cam->cZ);
	geTranslate(0.0, 0.0, 2.0 * water->z); //z=2*water.z
	geScale(1.0, 1.0, -1.0);

//	geClipPlane(0.0, 0.0, 1.0, 0.0); //w=-water.z
	glFrontFace(GL_CCW);
	if(cb){
		cb(cb_data);
	}
	glFrontFace(GL_CW);


	geTextureImage(0, texture);
	geTextureCopyBuffer(texture, 0, 0, 0, 0, 512, 512);
	geTextureImage(0, NULL);

	geClearMode(last_mode);
}

static void ge_MakeRefraction(ge_Water* water, ge_Camera* cam, ge_Image* texture, void (*cb)(void*), void* cb_data){
	u32 last_mode = geClearMode(GE_CLEAR_DEPTH_BUFFER);
	geClearScreen();

	geMatrixMode(GE_MATRIX_VIEW);
	geLoadIdentity();
	geLookAt(cam->x,cam->y,cam->z, cam->cX,cam->cY,cam->cZ);
	geTranslate(0.0, 0.0, 0.0);
	geScale(1.0, 1.0, 1.0);

//	geClipPlane(0.0, 0.0, -1.0, 0.0); //w=water.z
	if(cb){
		cb(cb_data);
	}
	
	geTextureImage(0, texture);
	geTextureCopyBuffer(texture, 0, 0, 0, 0, 512, 512);
	geTextureImage(0, NULL);

	geClearMode(last_mode);
}

void geWaterInit(ge_Renderer* render, float z){
	ge_Water* water = (ge_Water*)geMalloc(sizeof(ge_Water));
	water->type = GE_RENDERER_WATER;
	render->extension = water;
	
	water->loc_normalMapPos = geShaderUniformID(render->shader, "normalMapPos");
	water->loc_random = geShaderUniformID(render->shader, "random");
	water->normalMapPos[0] = water->normalMapPos[1] = water->normalMapPos[2] = 1.0;
	water->tex0 = geCreateSurface(512, 512, 0x00000000);
	water->tex3 = geCreateSurface(512, 512, 0x00000000);
	geTextureWrap(water->tex0, GE_CLAMP, GE_CLAMP);
	geTextureWrap(water->tex3, GE_CLAMP, GE_CLAMP);
	water->initialized = false;
	water->z = z;
}

void geWaterRender(ge_Renderer* render, ge_Camera* cam, void (*cb)(void*), void* cb_data){
	int i;
	ge_Water* water = (ge_Water*)render->extension;
	if(!water || water->type != GE_RENDERER_WATER || !cam){
		return;
	}
	if(!water->initialized){
		water->initialized = true;
		for(i=0; i<render->nObjs; i++){
			render->objs[i]->material.textures[0] = water->tex0;
			render->objs[i]->material.textures[3] = water->tex3;
		}
	}

	render->enabled = false;
	u32 last_mode = geClearMode(GE_CLEAR_DEPTH_BUFFER);
	geViewport(0, 0, 512, 512);
	geScissor(0, 0, 512, 512);
	
	ge_MakeReflection(water, cam, water->tex0, cb, cb_data);
	ge_MakeRefraction(water, cam, water->tex3, cb, cb_data);
	geClipPlane(0.0, 0.0, 0.0, 0.0);

	/*
	water->normalMapPos[0] += 0.0001;
	if(water->normalMapPos[0] > 1.0){
		water->normalMapPos[0] = 0.0;
	}
	water->normalMapPos[1] += 0.0002;
	if(water->normalMapPos[1] > 1.0){
		water->normalMapPos[1] = 0.0;
	}
	*/

	water->normalMapPos[2] += 0.5;
	if(water->normalMapPos[2] > M_PI){
		water->normalMapPos[2] = -M_PI;
	}
	
	float random = ((float)(rand() % 1000)) / 1000.0;
	geShaderUse(render->shader);
	geShaderUniform3f(water->loc_normalMapPos, water->normalMapPos[0], water->normalMapPos[1], water->normalMapPos[2]);
	geShaderUniform1f(water->loc_random, random);

	geViewport(0, 0, geGetContext()->width, geGetContext()->height);
	geScissor(0, 0, geGetContext()->width, geGetContext()->height);
	render->enabled = true;
	geClearScreen();
	geClearMode(last_mode);
}

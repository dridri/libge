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

#include <libge/libge.h>

int main(int argc, char** argv){
	geInit();
	geDebugMode(GE_DEBUG_ALL);

	geSplashscreenEnable(false);
	geCreateMainWindow("LibGE 3D", 1280, 720, 0);
	geCursorRoundMode(true);
//	geCursorVisible(false);
	geWaitVsync(true);

	ge_Keys* keys = geCreateKeys();
	ge_Font* font = geLoadFont("");
	geFontSize(font, 20);

	ge_Camera* cam = geCreateCamera();
	geCameraSetMaximumAngles(cam, -89, 89, GE_CAMERA_VERTICAL);
	cam->z = 1.8;

	ge_Object* obj = geLoadObject("cube.obj");

	ge_Shader* shader = geCreateShader();
	geShaderLoadVertexSource(shader, "default_shaders/generic_light.vert");
	geShaderLoadFragmentSource(shader, "default_shaders/generic_light.frag");

	ge_Renderer* renderer = geCreateRenderer(shader);
	geRendererLinkObject(renderer, obj);
	geRendererCreateContext(NULL, renderer);

	while(1){
		geReadKeys(keys);

		if(keys->pressed['Z']){
			geCameraMoveForward(cam, 10.0);
		}
		if(keys->pressed['S']){
			geCameraMoveBack(cam, 10.0);
		}
		if(keys->pressed['Q']){
			geCameraMoveLeft(cam, 10.0);
		}
		if(keys->pressed['D']){
			geCameraMoveRight(cam, 10.0);
		}
		if(keys->pressed[GEK_PRIOR]){
			cam->z += 0.1;
		}
		if(keys->pressed[GEK_NEXT]){
			cam->z -= 0.1;
		}
		if(keys->pressed[GEK_ESCAPE]){
			break;
		}

		geCameraRotateWithMouse(cam, 0.1);
		geCameraLook(cam);
		
		geDrawingMode(GE_DRAWING_MODE_3D);
		geClearScreen();
		geRenderObjects(renderer);
	
		geDrawingMode(GE_DRAWING_MODE_2D);
		geFontPrintfScreen(0, 0, font, 0xFFFFFFFF, "FPS: %d\nCamera: x, y, z : %f %f %f", geFps(), cam->x, cam->y, cam->z);
		
		geSwapBuffers();
	}

	return 0;
}

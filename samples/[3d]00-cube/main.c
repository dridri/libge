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
	geCreateMainWindow("LibGE Win32 low profile", 512, 512, GE_WINDOW_RESIZABLE);
	geWaitVsync(true);

	ge_Keys* keys = geCreateKeys();
	ge_Font* font = geLoadFont("data/arial.ttf");
	geFontSize(font, 20);

	ge_Camera* cam = geCreateCamera();
	cam->x = 3.0;
	cam->y = 3.0;
	cam->z = 3.0;
	cam->cX = 0.0;
	cam->cY = 0.0;
	cam->cZ = 0.0;

	ge_Scene* scene = geLoadScene("scene/scene.ge3d");

	while(1){
		geReadKeys(keys);
		if(keys->pressed[GEVK_BACK]){
			break;
		}
		
		geDrawingMode(GE_DRAWING_MODE_3D);
		geClearScreen();
	
		geObjectMatrixRotate(scene->renderers[0].objs[0], 0.01, 0.02, 0.0);
		geCameraLook(cam);
		geSceneDraw(scene);
	
	
		geDrawingMode(GE_DRAWING_MODE_2D);
		geFontPrintfScreen(0, 0, font, 0xFFFFFFFF, "FPS: %d", geFps());
		
		geSwapBuffers();
	}

	return 0;
}

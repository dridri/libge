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
	
	char buf[256] = "";
	char buf2[256] = "";
	char win_title[64] = "";
	int win_res[2] = { 0, 0 };
	int win_mode = 0;
	char f_image[256] = "";
	char f_font[256] = "";
	int font_size = 0;

	ge_File* conf = geFileOpen("conf.ini", GE_FILE_MODE_READ);
	while(geFileGets(conf, buf, 256)){
		geGetParamIntMulti(buf, "resolution", win_res, 2);
		geGetParamInt(buf, "font_size", &font_size);
		geGetParamString(buf, "title", win_title, 64);
		geGetParamString(buf, "image", f_image, 256);
		geGetParamString(buf, "font ", f_font, 256);
		if(geGetParamString(buf, "mode", buf2, 256)){
			if(strstr(buf2, "fullscreen")){
				win_mode |= GE_WINDOW_FULLSCREEN;
			}
			if(strstr(buf2, "resizable")){
				win_mode |= GE_WINDOW_RESIZABLE;
			}
		}
	}
	geFileClose(conf);
	printf("\"%s\", %d, %d, %08X\n", win_title, win_res[0], win_res[1], win_mode);
	printf("image \"%s\"\n", f_image);
	printf("font \"%s\" [%d]\n", f_font, font_size);

	geCreateMainWindow(win_title, win_res[0], win_res[1], win_mode);
	geDrawingMode(GE_DRAWING_MODE_2D);
	
	geClearColor(RGB(100,100,100));
	ge_Image* img = geLoadImage(f_image);
	ge_Font* font = geLoadFont(f_font);
	geFontSize(font, font_size);

	ge_Keys* keys = geCreateKeys();

	while(1){
		geReadKeys(keys);
		if(geKeysToggled(keys, GEVK_BACK)){
			break;
		}
		geClearScreen();
	
		geDrawImage(0, 0, img);
	
		char str[32]; sprintf(str, "FPS: %d", geFps());
		geFontPrintScreen(0, 0, font, str, 0xFFFFFFFF);
	
		geSwapBuffers();
	}

	return 0;
}

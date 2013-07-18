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
	geCreateMainWindow("LibGE", 1024, 768, 0);

	// Load Font for GUI style
	ge_Font* font = geLoadFont("data/arial.ttf");

	// Create window, default black GUI is loaded
	ge_GuiWindow* win = geGuiCreateWindow("Hello GUI", 640, 480, 0);
	// Set font for current style
	geGuiStyleFont(win->style, font, 20);

	// Create 2 buttons
	ge_GuiButton* btn = geGuiCreateButton("Ok", 200, 25);
	ge_GuiButton* btn2 = geGuiCreateButton("Cancel", 200, 25);

	// Create an input box
	char str[256] = "";
	ge_GuiInputBox* input = geGuiCreateInputBox(200, 22, str, 256);

	// Create a progress bar
	ge_GuiProgressBar* pbar = geGuiCreateProgressBar(400, 30, 500);

	// Link objects to window
	geGuiWindowLinkObject(win, win->width/2-btn->width/2, 100, btn, 0);
	geGuiWindowLinkObject(win, win->width/2-btn2->width/2, 130, btn2, 0);
	geGuiWindowLinkObject(win, 20, 20, input, 0);
	geGuiWindowLinkObject(win, 20, win->height-50, pbar, 0);

	while(1){
		geClearScreen();

		// Increment progressBar value
		pbar->status++;
		
		if(btn->pressed){
			pbar->visible = false;
		}
		if(btn2->pressed){
			pbar->visible = true;
		}

		// SwapBuffers automatically draws GUI windows
		geSwapBuffers();
    }

    geQuit();
    return 0;
}

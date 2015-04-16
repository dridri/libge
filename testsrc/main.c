/*
 * This file is for testing purpose only
 * Please consider to compile LibGE as a library instead
*/


#include "libge.h"

void SSAO();

void anim(){
	ge_Image* img = geLoadImage("testsrc/data/ship.png");
	img = geAnimateImage(img, 4, 0.125f);

	ge_Font* font = geLoadFont("");
	geFontSize(font, 14);

	ge_Joystick* js = geJoystickOpen(geJoystickUpdateList(NULL));

	while(1){
		geJoystickRead(js);
		geJoystickDebugPrint(js, 0);

		geClearScreen();
		geDrawImage(100, 100, img);
		geFontPrintfScreen(0, 0, font, RGBA(255, 255, 255, 255), "FPS : %d", geFps());
		geSwapBuffers();
	}
}

int main(int argc, char** argv){
	SSAO();

	geDebugMode(GE_DEBUG_ALL);
	geInit();
	geCreateMainWindow("LibGE !", 1024, 640, GE_WINDOW_RESIZABLE);
	geWaitVsync(false);

	anim();

	ge_LuaScript* script = geLoadLuaScript("testsrc/test.lua");
	printf("\n\n\n");
	geLuaScriptStart(script, GE_LUA_EXECUTION_MODE_NORMAL);
	printf("\n\n\n");

	geClearColor(RGBA(40, 40, 40, 255));
	ge_Font* font = geLoadFont("");
	geFontSize(font, 14);


	while(1){
		geClearScreen();
		geFontPrintfScreen(0, 0, font, RGBA(255, 255, 255, 255), "FPS : %d", geFps());
		geSwapBuffers();
	}

	geQuit();
	return 0;
}

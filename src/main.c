#include "ge_internal.h"

int main(int argc, char** argv){
	geInit();
	geCreateMainWindow("LibGE !", 1024, 640, GE_WINDOW_RESIZABLE);
	geWaitVsync(false);

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

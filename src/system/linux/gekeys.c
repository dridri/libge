#include "libge.h"


ge_Keys* geCreateKeys(){
	ge_Keys* keys = (ge_Keys*)geMalloc(sizeof(ge_Keys));
	return keys;
}

#if(defined(WIN32))


bool* geGetKeysTable(){
//	return WindowsGetKeysTable();
	return NULL; //Not yet implemented
}
bool geReadKeys(ge_Keys* keys){
	WindowsGetPressedKeys(keys->pressed);
	return true;
}
void geSetKeyRepeat(bool enabled){
//	WindowsSetKeyRepeat(enabled);
}


#elif(defined(PSP))
#include <pspctrl.h>

static bool key_reapet = true;
bool geReadKeys(ge_Keys* keys){
	SceCtrlData pad;

	sceCtrlPeekBufferPositive(&pad, 1);
	memset(&keys->pressed, 0, sizeof(keys->pressed));
	keys->pressed[GEK_CROSS] = pad.Buttons & PSP_CTRL_CROSS;
	keys->pressed[GEK_CIRCLE] = pad.Buttons & PSP_CTRL_CIRCLE;
	keys->pressed[GEK_TRIANGLE] = pad.Buttons & PSP_CTRL_TRIANGLE;
	keys->pressed[GEK_SQUARE] = pad.Buttons & PSP_CTRL_SQUARE;
	keys->pressed[GEK_UP] = pad.Buttons & PSP_CTRL_UP;
	keys->pressed[GEK_RIGHT] = pad.Buttons & PSP_CTRL_RIGHT;
	keys->pressed[GEK_DOWN] = pad.Buttons & PSP_CTRL_DOWN;
	keys->pressed[GEK_LEFT] = pad.Buttons & PSP_CTRL_LEFT;
	keys->pressed[GEK_LTRIGGER] = pad.Buttons & PSP_CTRL_LTRIGGER;
	keys->pressed[GEK_RTRIGGER] = pad.Buttons & PSP_CTRL_RTRIGGER;
	keys->pressed[GEK_START] = pad.Buttons & PSP_CTRL_START;
	keys->pressed[GEK_SELECT] = pad.Buttons & PSP_CTRL_SELECT;
	keys->pressed[GEK_HOME] = pad.Buttons & PSP_CTRL_HOME;
	keys->pressed[GEK_HOLD] = pad.Buttons & PSP_CTRL_HOLD;
	keys->pressed[GEK_NOTE] = pad.Buttons & PSP_CTRL_NOTE;
	keys->pressed[GEK_SCREEN] = pad.Buttons & PSP_CTRL_SCREEN;
	keys->pressed[GEK_VOLUP] = pad.Buttons & PSP_CTRL_VOLUP;
	keys->pressed[GEK_VOLDOWN] = pad.Buttons & PSP_CTRL_VOLDOWN;
	keys->pressed[GEK_WLAN_UP] = pad.Buttons & PSP_CTRL_WLAN_UP;
	keys->pressed[GEK_REMOTE] = pad.Buttons & PSP_CTRL_REMOTE;
	keys->pressed[GEK_DISC] = pad.Buttons & PSP_CTRL_DISC;
	keys->pressed[GEK_MS] = pad.Buttons & PSP_CTRL_MS;

	sceCtrlPeekBufferNegative(&pad, 1);
	memset(&keys->released, 0, sizeof(keys->released));
	keys->released[GEK_CROSS] = pad.Buttons & PSP_CTRL_CROSS;
	keys->released[GEK_CIRCLE] = pad.Buttons & PSP_CTRL_CIRCLE;
	keys->released[GEK_TRIANGLE] = pad.Buttons & PSP_CTRL_TRIANGLE;
	keys->released[GEK_SQUARE] = pad.Buttons & PSP_CTRL_SQUARE;
	keys->released[GEK_UP] = pad.Buttons & PSP_CTRL_UP;
	keys->released[GEK_RIGHT] = pad.Buttons & PSP_CTRL_RIGHT;
	keys->released[GEK_DOWN] = pad.Buttons & PSP_CTRL_DOWN;
	keys->released[GEK_LEFT] = pad.Buttons & PSP_CTRL_LEFT;
	keys->released[GEK_LTRIGGER] = pad.Buttons & PSP_CTRL_LTRIGGER;
	keys->released[GEK_RTRIGGER] = pad.Buttons & PSP_CTRL_RTRIGGER;
	keys->released[GEK_START] = pad.Buttons & PSP_CTRL_START;
	keys->released[GEK_SELECT] = pad.Buttons & PSP_CTRL_SELECT;
	keys->released[GEK_HOME] = pad.Buttons & PSP_CTRL_HOME;
	keys->released[GEK_HOLD] = pad.Buttons & PSP_CTRL_HOLD;
	keys->released[GEK_NOTE] = pad.Buttons & PSP_CTRL_NOTE;
	keys->released[GEK_SCREEN] = pad.Buttons & PSP_CTRL_SCREEN;
	keys->released[GEK_VOLUP] = pad.Buttons & PSP_CTRL_VOLUP;
	keys->released[GEK_VOLDOWN] = pad.Buttons & PSP_CTRL_VOLDOWN;
	keys->released[GEK_WLAN_UP] = pad.Buttons & PSP_CTRL_WLAN_UP;
	keys->released[GEK_REMOTE] = pad.Buttons & PSP_CTRL_REMOTE;
	keys->released[GEK_DISC] = pad.Buttons & PSP_CTRL_DISC;
	keys->released[GEK_MS] = pad.Buttons & PSP_CTRL_MS;

	return true;
}
void geSetKeyRepeat(bool enabled){
	key_reapet = enabled;
}


#else


bool* geGetKeysTable(){
	return X11GetKeysTable();
}
void geReadKeys(bool* k){
	X11GetKeys(k);
}
void geSetKeyRepeat(bool enabled){
	X11SetKeyRepeat(enabled);
}


#endif

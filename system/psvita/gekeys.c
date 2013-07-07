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

#include "../../ge_internal.h"
#include <pspctrl.h>

ge_Keys* geCreateKeys(){
//	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
	ge_Keys* keys = (ge_Keys*)geMalloc(sizeof(ge_Keys));
	geKeysAssign(keys, GEK_CROSS, GEVK_ENTER);
	geKeysAssign(keys, GEK_CIRCLE, GEVK_BACK);
	geKeysAssign(keys, GEK_UP, GEVK_UP);
	geKeysAssign(keys, GEK_DOWN, GEVK_DOWN);
	geKeysAssign(keys, GEK_LEFT, GEVK_LEFT);
	geKeysAssign(keys, GEK_RIGHT, GEVK_RIGHT);
	geKeysAssign(keys, GEK_SQUARE, GEVK_ACTION1);
	geKeysAssign(keys, GEK_TRIANGLE, GEVK_ACTION2);
	geKeysAssign(keys, GEK_LTRIGGER, GEVK_ACTION3);
	geKeysAssign(keys, GEK_RTRIGGER, GEVK_ACTION4);
	geKeysAssign(keys, GEK_START, GEVK_ACTION5);
	geKeysAssign(keys, GEK_SELECT, GEVK_ACTION6);
	return keys;
}

void geKeysAssign(ge_Keys* keys, int real, int vir){
	keys->assigns[vir - GE_KEYS_COUNT] = real;
}

static bool key_reapet = true;
bool geReadKeys(ge_Keys* keys){
	memcpy(keys->last, keys->pressed, GE_KEYS_COUNT+32);

	SceCtrlData pad;
	sceCtrlPeekBufferPositive(&pad, 1);
	/*
	keys->pressed[GEK_CROSS] = (pad.Buttons & PSP_CTRL_CROSS)? 1 : 0;
	keys->pressed[GEK_CIRCLE] = (pad.Buttons & PSP_CTRL_CIRCLE)? 1 : 0;
	keys->pressed[GEK_TRIANGLE] = (pad.Buttons & PSP_CTRL_TRIANGLE)? 1 : 0;
	keys->pressed[GEK_SQUARE] = (pad.Buttons & PSP_CTRL_SQUARE)? 1 : 0;
	keys->pressed[GEK_UP] = (pad.Buttons & PSP_CTRL_UP)? 1 : 0;
	keys->pressed[GEK_RIGHT] = (pad.Buttons & PSP_CTRL_RIGHT)? 1 : 0;
	keys->pressed[GEK_DOWN] = (pad.Buttons & PSP_CTRL_DOWN)? 1 : 0;
	keys->pressed[GEK_LEFT] = (pad.Buttons & PSP_CTRL_LEFT)? 1 : 0;
	keys->pressed[GEK_LTRIGGER] = (pad.Buttons & PSP_CTRL_LTRIGGER)? 1 : 0;
	keys->pressed[GEK_RTRIGGER] = (pad.Buttons & PSP_CTRL_RTRIGGER)? 1 : 0;
	keys->pressed[GEK_START] = (pad.Buttons & PSP_CTRL_START)? 1 : 0;
	keys->pressed[GEK_SELECT] = (pad.Buttons & PSP_CTRL_SELECT)? 1 : 0;
	keys->pressed[GEK_HOME] = (pad.Buttons & PSP_CTRL_HOME)? 1 : 0;
	keys->pressed[GEK_HOLD] = (pad.Buttons & PSP_CTRL_HOLD)? 1 : 0;
	keys->pressed[GEK_NOTE] = (pad.Buttons & PSP_CTRL_NOTE)? 1 : 0;
	keys->pressed[GEK_SCREEN] = (pad.Buttons & PSP_CTRL_SCREEN)? 1 : 0;
	keys->pressed[GEK_VOLUP] = (pad.Buttons & PSP_CTRL_VOLUP)? 1 : 0;
	keys->pressed[GEK_VOLDOWN] = (pad.Buttons & PSP_CTRL_VOLDOWN)? 1 : 0;
	keys->pressed[GEK_WLAN_UP] = (pad.Buttons & PSP_CTRL_WLAN_UP)? 1 : 0;
	keys->pressed[GEK_REMOTE] = (pad.Buttons & PSP_CTRL_REMOTE)? 1 : 0;
	keys->pressed[GEK_DISC] = (pad.Buttons & PSP_CTRL_DISC)? 1 : 0;
	keys->pressed[GEK_MS] = (pad.Buttons & PSP_CTRL_MS)? 1 : 0;
	*/
	keys->pressed[GEVK_ENTER] = keys->pressed[keys->assigns[GEVK_ENTER - GE_KEYS_COUNT]];
	keys->pressed[GEVK_BACK] = keys->pressed[keys->assigns[GEVK_BACK - GE_KEYS_COUNT]];
	keys->pressed[GEVK_UP] = keys->pressed[keys->assigns[GEVK_UP - GE_KEYS_COUNT]];
	keys->pressed[GEVK_DOWN] = keys->pressed[keys->assigns[GEVK_DOWN - GE_KEYS_COUNT]];
	keys->pressed[GEVK_LEFT] = keys->pressed[keys->assigns[GEVK_LEFT - GE_KEYS_COUNT]];
	keys->pressed[GEVK_RIGHT] = keys->pressed[keys->assigns[GEVK_RIGHT - GE_KEYS_COUNT]];
	keys->pressed[GEVK_ACTION1] = keys->pressed[keys->assigns[GEVK_ACTION1 - GE_KEYS_COUNT]];
	keys->pressed[GEVK_ACTION2] = keys->pressed[keys->assigns[GEVK_ACTION2 - GE_KEYS_COUNT]];
	keys->pressed[GEVK_ACTION3] = keys->pressed[keys->assigns[GEVK_ACTION3 - GE_KEYS_COUNT]];
	keys->pressed[GEVK_ACTION4] = keys->pressed[keys->assigns[GEVK_ACTION4 - GE_KEYS_COUNT]];
	keys->pressed[GEVK_ACTION5] = keys->pressed[keys->assigns[GEVK_ACTION5 - GE_KEYS_COUNT]];
	keys->pressed[GEVK_ACTION6] = keys->pressed[keys->assigns[GEVK_ACTION6 - GE_KEYS_COUNT]];
	keys->pressed[GEVK_ACTION7] = keys->pressed[keys->assigns[GEVK_ACTION7 - GE_KEYS_COUNT]];
	keys->pressed[GEVK_ACTION8] = keys->pressed[keys->assigns[GEVK_ACTION8 - GE_KEYS_COUNT]];
	keys->pressed[GEVK_ACTION9] = keys->pressed[keys->assigns[GEVK_ACTION9 - GE_KEYS_COUNT]];
	keys->pressed[GEVK_ACTION10] = keys->pressed[keys->assigns[GEVK_ACTION10 - GE_KEYS_COUNT]];
	keys->pressed[GEVK_ACTION11] = keys->pressed[keys->assigns[GEVK_ACTION11 - GE_KEYS_COUNT]];
	keys->pressed[GEVK_ACTION12] = keys->pressed[keys->assigns[GEVK_ACTION12 - GE_KEYS_COUNT]];
	keys->pressed[GEVK_ACTION13] = keys->pressed[keys->assigns[GEVK_ACTION13 - GE_KEYS_COUNT]];
	keys->pressed[GEVK_ACTION14] = keys->pressed[keys->assigns[GEVK_ACTION14 - GE_KEYS_COUNT]];
	keys->pressed[GEVK_ACTION15] = keys->pressed[keys->assigns[GEVK_ACTION15 - GE_KEYS_COUNT]];
	keys->pressed[GEVK_ACTION16] = keys->pressed[keys->assigns[GEVK_ACTION16 - GE_KEYS_COUNT]];
	keys->pressed[GEVK_ACTION17] = keys->pressed[keys->assigns[GEVK_ACTION17 - GE_KEYS_COUNT]];
	keys->pressed[GEVK_ACTION18] = keys->pressed[keys->assigns[GEVK_ACTION18 - GE_KEYS_COUNT]];
	keys->pressed[GEVK_ACTION19] = keys->pressed[keys->assigns[GEVK_ACTION19 - GE_KEYS_COUNT]];
	keys->pressed[GEVK_ACTION20] = keys->pressed[keys->assigns[GEVK_ACTION20 - GE_KEYS_COUNT]];
	keys->pressed[GEVK_ACTION21] = keys->pressed[keys->assigns[GEVK_ACTION21 - GE_KEYS_COUNT]];
	keys->pressed[GEVK_ACTION22] = keys->pressed[keys->assigns[GEVK_ACTION22 - GE_KEYS_COUNT]];
	keys->pressed[GEVK_ACTION23] = keys->pressed[keys->assigns[GEVK_ACTION23 - GE_KEYS_COUNT]];
	keys->pressed[GEVK_ACTION24] = keys->pressed[keys->assigns[GEVK_ACTION24 - GE_KEYS_COUNT]];
	keys->pressed[GEVK_ACTION25] = keys->pressed[keys->assigns[GEVK_ACTION25 - GE_KEYS_COUNT]];
	keys->pressed[GEVK_ACTION26] = keys->pressed[keys->assigns[GEVK_ACTION26 - GE_KEYS_COUNT]];

	return true;
}

int geKeysToggled(ge_Keys* keys, int key){
	return ( keys->pressed[key] && (!keys->last[key]) );
}

int geKeysUnToggled(ge_Keys* keys, int key){
	return ( (!keys->pressed[key]) && keys->last[key] );
}

void geSetKeyRepeat(bool enabled){
//	key_reapet = enabled;
}

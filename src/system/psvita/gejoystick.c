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

const ge_JoystickController* geJoystickUpdateList(int* num){
	return NULL;
}

ge_Joystick* geJoystickOpen(const ge_JoystickController* controller){
	return NULL;
}

static _ge_JoystickReadEvent(int joystick_fd, _ge_jsEvent* js){
	return -1;
}

void geJoystickClose(ge_Joystick* js){
}

static void geJoystickReadAxisXbox360(ge_Joystick* js, _ge_jsEvent* jse){
}

static void geJoystickReadAxisDS3(ge_Joystick* js, _ge_jsEvent* jse){
}

void geJoystickRead(ge_Joystick* js){
}

void geJoystickDebugPrint(ge_Joystick* js, int num){
	gePrintDebug(0, "JOYSTICK %d :\n", num);
	gePrintDebug(0, "    Axis Left     : %5d %5d\n", js->stick1_x, js->stick1_y);
	gePrintDebug(0, "    Axis Right    : %5d %5d\n", js->stick2_x, js->stick2_y);
	gePrintDebug(0, "    Trigger Left  : %5d\n", js->trigger_l);
	gePrintDebug(0, "    Trigger Right : %5d\n", js->trigger_r);
	gePrintDebug(0, "    Buttons       :\n");
	gePrintDebug(0, "        [0] = %d     [1] = %d     [2] = %d     [3] = %d\n", js->buttons[0], js->buttons[1], js->buttons[2], js->buttons[3]);
	gePrintDebug(0, "        [4] = %d     [5] = %d     [6] = %d     [7] = %d\n", js->buttons[4], js->buttons[5], js->buttons[6], js->buttons[7]);
	gePrintDebug(0, "        [8] = %d     [9] = %d    [10] = %d    [11] = %d\n", js->buttons[8], js->buttons[9], js->buttons[10], js->buttons[11]);
	gePrintDebug(0, "       [12] = %d    [13] = %d    [14] = %d    [15] = %d\n", js->buttons[12], js->buttons[13], js->buttons[14], js->buttons[15]);
}

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

#include "getypes.h"

#define GE_JOYSTICK_DS3 0xD53
#define GE_JOYSTICK_DS4 0xD54
#define GE_JOYSTICK_XBOX360 0x360

typedef struct ge_JoystickController {
	char* name;
	char* device;
	u32 driver;
	u8 nButtons;
	u8 nAxes;
	u32 type;
	u8 swaps[32];
}ge_JoystickController;

typedef struct ge_Joystick {
	void* handler;
	const ge_JoystickController* controller;
	u8 buttons[32];
	u8 swaps[32];
	s16 stick1_x;
	s16 stick1_y;
	s16 stick2_x;
	s16 stick2_y;
	s16 trigger_l;
	s16 trigger_r;
} ge_Joystick;

const ge_JoystickController* geJoystickUpdateList(int* num);
ge_Joystick* geJoystickOpen(const ge_JoystickController* controller);
bool geJoystickRead(ge_Joystick* js);
void geJoystickForceFeedback(ge_Joystick* js, bool enable);
void geJoystickClose(ge_Joystick* js);
void geJoystickDebugPrint(ge_Joystick* js, int num);

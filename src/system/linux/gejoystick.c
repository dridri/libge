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
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <linux/joystick.h>
#include <ctype.h>

typedef struct _ge_jsEvent {
	unsigned int time;
	short value;
	unsigned char type;
	unsigned char number;
} _ge_jsEvent;

static ge_JoystickController* _ge_JoystickList = 0;

const ge_JoystickController* geJoystickUpdateList(int* num){
	int i, j, k, fd;
	char tmp[256];

	if(!_ge_JoystickList){
		_ge_JoystickList = (ge_JoystickController*)geMalloc(sizeof(ge_JoystickController) * 17);
		for(i=0; i<17; i++){
			_ge_JoystickList[i].name = (char*)geMalloc(256);
			_ge_JoystickList[i].device = (char*)geMalloc(256);
		}
	}

	for(i=0, j=0; i<16; i++){
		sprintf(tmp, "/dev/input/js%d", i);
		if(access(tmp, F_OK) != -1){
			sprintf(_ge_JoystickList[j].device, "/dev/input/js%d", i);
			fd = open(_ge_JoystickList[j].device, O_RDWR | O_NONBLOCK);
			if(fd >= 0){
				ioctl(fd, JSIOCGNAME(256), _ge_JoystickList[j].name);
				ioctl(fd, JSIOCGVERSION, &_ge_JoystickList[j].driver);
				ioctl(fd, JSIOCGBUTTONS, &_ge_JoystickList[j].nButtons);
				ioctl(fd, JSIOCGAXES, &_ge_JoystickList[j].nAxes);
				close(fd);
				strcpy(tmp, _ge_JoystickList[j].name);
				char* p = tmp;
				for ( ; *p; ++p) *p = tolower(*p);
				if(strstr(tmp, "playstation") && strstr(tmp, "3")){
					_ge_JoystickList[j].type = GE_JOYSTICK_DS3;
				}else if(strstr(tmp, "gasia") && strstr(tmp, "ps(r)")){
					_ge_JoystickList[j].type = GE_JOYSTICK_DS3;
				}else if(strstr(tmp, "xbox") && strstr(tmp, "360")){
					_ge_JoystickList[j].type = GE_JOYSTICK_XBOX360;
				}else if(strstr(tmp, "sony") && strstr(tmp, "wireless") && strstr(tmp, "controller")){
					_ge_JoystickList[j].type = GE_JOYSTICK_DS4;
				}
			}
			if(_ge_JoystickList[j].type == GE_JOYSTICK_DS3){
				// Avoid collisions
				for(k=0; k<32; k++){
					_ge_JoystickList[j].swaps[k] = 31;
				}
				_ge_JoystickList[j].swaps[14] = 0; // X
				_ge_JoystickList[j].swaps[13] = 1; // O
				_ge_JoystickList[j].swaps[15] = 2; // []
				_ge_JoystickList[j].swaps[12] = 3; // /\

				_ge_JoystickList[j].swaps[4] = 13; // UP
				_ge_JoystickList[j].swaps[5] = 12; // RIGHT
				_ge_JoystickList[j].swaps[6] = 14; // DOWN
				_ge_JoystickList[j].swaps[7] = 11; // LEFT

				_ge_JoystickList[j].swaps[3] = 7; // START
				_ge_JoystickList[j].swaps[0] = 6; // SELECT

				_ge_JoystickList[j].swaps[10] = 4; // L1
				_ge_JoystickList[j].swaps[11] = 5; // R1
				_ge_JoystickList[j].swaps[1] = 9; // L3
				_ge_JoystickList[j].swaps[2] = 10; // R3
			}else if(_ge_JoystickList[j].type == GE_JOYSTICK_DS4){
				// Avoid collisions
				for(k=0; k<32; k++){
					_ge_JoystickList[j].swaps[k] = 31;
				}
				_ge_JoystickList[j].swaps[1] = 0; // X
				_ge_JoystickList[j].swaps[2] = 1; // O
				_ge_JoystickList[j].swaps[0] = 2; // []
				_ge_JoystickList[j].swaps[3] = 3; // /\

				_ge_JoystickList[j].swaps[9] = 7; // START
				_ge_JoystickList[j].swaps[8] = 6; // SELECT

				_ge_JoystickList[j].swaps[4] = 4; // L1
				_ge_JoystickList[j].swaps[5] = 5; // R1
				_ge_JoystickList[j].swaps[10] = 9; // L3
				_ge_JoystickList[j].swaps[11] = 10; // R3
			}else{
				for(k=0; k<32; k++){
					_ge_JoystickList[j].swaps[k] = k;
				}
			}

			j++;
		}
	}

	if(num){
		*num = j;
	}

	return (const ge_JoystickController*)_ge_JoystickList;
}

ge_Joystick* geJoystickOpen(const ge_JoystickController* controller){
	int i;
	int joystick_fd = open(controller->device, O_RDWR | O_NONBLOCK); /* read write for force feedback? */
	if (joystick_fd < 0)
		return NULL;

	/* maybe ioctls to interrogate features here? */

	ge_Joystick* ret = (ge_Joystick*)geMalloc(sizeof(ge_Joystick));
	ret->handler = (void*)(uint64_t)joystick_fd;
	ret->controller = controller;
	for(i=0; i<32; i++){
		ret->swaps[i] = i;
	}
	return ret;
}

int _ge_JoystickReadEvent(int joystick_fd, _ge_jsEvent* js){
	int bytes;
	bytes = read(joystick_fd, js, sizeof(*js));

	if (bytes == -1)
		return 0;

	if (bytes == sizeof(*js))
		return 1;

	printf("Unexpected bytes from joystick: %d\n", bytes);

	return -1;
}

void geJoystickClose(ge_Joystick* js){
	int joystick_fd = (int)(uint64_t)js->handler;
	close(joystick_fd);
}

static void geJoystickReadAxisXbox360(ge_Joystick* js, _ge_jsEvent* jse){
	switch (jse->number) {
		case 0: js->stick1_x = jse->value;
			break;
		case 1: js->stick1_y = jse->value;
			break;
		case 3: js->stick2_x = jse->value;
			break;
		case 4: js->stick2_y = jse->value;
			break;
		case 2: js->trigger_l = jse->value;
			break;
		case 5: js->trigger_r = jse->value;
			break;
		default:
			break;
	}
}

static void geJoystickReadAxisDS3(ge_Joystick* js, _ge_jsEvent* jse){
	switch (jse->number) {
		case 0: js->stick1_x = jse->value;
			break;
		case 1: js->stick1_y = jse->value;
			break;
		case 2: js->stick2_x = jse->value;
			break;
		case 3: js->stick2_y = jse->value;
			break;
		case 12: js->trigger_l = jse->value;
			break;
		case 13: js->trigger_r = jse->value;
			break;
		default:
			break;
	}
}

static void geJoystickReadAxisDS4(ge_Joystick* js, _ge_jsEvent* jse){
	switch (jse->number) {
		case 0: js->stick1_x = jse->value;
			break;
		case 1: js->stick1_y = jse->value;
			break;
		case 2: js->stick2_x = jse->value;
			break;
		case 5: js->stick2_y = jse->value;
			break;
		case 3: js->trigger_l = jse->value;
			break;
		case 4: js->trigger_r = jse->value;
			break;
		case 7:
			if(jse->value < 0){
				js->buttons[js->swaps[13]] = 1;
				js->buttons[js->swaps[14]] = 0;
			}else if(jse->value > 0){
				js->buttons[js->swaps[13]] = 0;
				js->buttons[js->swaps[14]] = 1;
			}else{
				js->buttons[js->swaps[13]] = 0;
				js->buttons[js->swaps[14]] = 0;
			}
			break;
		case 6:
			if(jse->value < 0){
				js->buttons[js->swaps[11]] = 1;
				js->buttons[js->swaps[12]] = 0;
			}else if(jse->value > 0){
				js->buttons[js->swaps[11]] = 0;
				js->buttons[js->swaps[12]] = 1;
			}else{
				js->buttons[js->swaps[11]] = 0;
				js->buttons[js->swaps[12]] = 0;
			}
			break;
		default:
			break;
	}
}

bool geJoystickRead(ge_Joystick* js){
	int rc;
	bool ret = false;
	_ge_jsEvent jse;
	int joystick_fd = (int)(uint64_t)js->handler;
	if (joystick_fd < 0)
		return false;

	while ((rc = _ge_JoystickReadEvent(joystick_fd, &jse) == 1)) {
		jse.type &= ~JS_EVENT_INIT;
		if (jse.type == JS_EVENT_AXIS) {
			if(js->controller->type == GE_JOYSTICK_XBOX360){
				geJoystickReadAxisXbox360(js, &jse);
			}
			if(js->controller->type == GE_JOYSTICK_DS3){
				geJoystickReadAxisDS3(js, &jse);
			}
			if(js->controller->type == GE_JOYSTICK_DS4){
				geJoystickReadAxisDS4(js, &jse);
			}
		} else if (jse.type == JS_EVENT_BUTTON) {
			if (jse.number < 32) {
				js->buttons[js->swaps[js->controller->swaps[jse.number]]] = jse.value;
			}
		}
		ret = true;
	}

	return ret;
}

void geJoystickForceFeedback(ge_Joystick* js, bool enable){
	struct ff_effect ff;
	struct input_event play;

	ff.type = FF_RUMBLE;
	ff.id = -1;
	ff.u.rumble.strong_magnitude = (unsigned short)(1.0 * 65535);
	ff.u.rumble.weak_magnitude = (unsigned short)(1.0 * 65535);
	ff.replay.length = 5;
	ff.replay.delay = 0;

	if (ioctl((int)(uint64_t)js->handler, EVIOCSFF, &ff) != -1) {
	}

	/* play the effect */
	play.type = EV_FF;
	play.code = ff.id;
	play.value = 1;
	write((int)(uint64_t)js->handler, (const void*)&play, sizeof(play));
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

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

#ifndef __GE_KEYS__
#define __GE_KEYS__

#ifndef WIN32
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if(defined(WIN32))
//Windows Keys
#define GEK_LBUTTON		1
#define GEK_RBUTTON		2
#define GEK_MBUTTON		4
#define GEK_MWHEELUP	5
#define GEK_MWHEELDOWN	6
#define GEK_BACK		8
#define GEK_TAB			9
#define GEK_CLEAR		12
#define GEK_ENTER		13
#define GEK_SHIFT		16
#define GEK_CONTROL		17
#define GEK_MENU		18
#define GEK_PAUSE		19
//#define GEK_KANA			0x15
//#define GEK_HANGEUL		0x15
//#define GEK_HANGUL		0x15
//#define GEK_JUNJA			0x17
//#define GEK_FINAL			0x18
//#define GEK_HANJA			0x19
//#define GEK_KANJI			0x19
#define GEK_ESCAPE		0x1B
//#define GEK_CONVERT		0x1C
//#define GEK_NONCONVERT	0x1D
//#define GEK_ACCEPT		0x1E
//#define GEK_MODECHANGE	0x1F
#define GEK_SPACE		32
#define GEK_PRIOR		33
#define GEK_NEXT		34
#define GEK_END			35
#define GEK_HOME		36
#define GEK_LEFT		37
#define GEK_UP			38
#define GEK_RIGHT		39
#define GEK_DOWN		40
#define GEK_SELECT		41
#define GEK_PRINT		42
#define GEK_SNAPSHOT	44
#define GEK_INSERT		45
#define GEK_DELETE		46
#define GEK_HELP		47
#define GEK_LSUPER		0x5B
#define GEK_RSUPER		0x5C
#define GEK_NUMPAD0		0x60
#define GEK_NUMPAD1		0x61
#define GEK_NUMPAD2		0x62
#define GEK_NUMPAD3		0x63
#define GEK_NUMPAD4		0x64
#define GEK_NUMPAD5		0x65
#define GEK_NUMPAD6		0x66
#define GEK_NUMPAD7		0x67
#define GEK_NUMPAD8		0x68
#define GEK_NUMPAD9		0x69
#define GEK_MULTIPLY	0x6A
#define GEK_ADD			0x6B
#define GEK_SEPARATOR	0x6C
#define GEK_SUBTRACT	0x6D
#define GEK_DECIMAL		0x6E
#define GEK_DIVIDE		0x6F
#define GEK_F1			0x70
#define GEK_F2			0x71
#define GEK_F3			0x72
#define GEK_F4			0x73
#define GEK_F5			0x74
#define GEK_F6			0x75
#define GEK_F7			0x76
#define GEK_F8			0x77
#define GEK_F9			0x78
#define GEK_F10			0x79
#define GEK_F11			0x7A
#define GEK_F12			0x7B
#define GEK_NUMLOCK		0x90
#define GEK_SCROLL		0x91
//Add GEK_SHIFTLOCK

#define GE_KEYS_COUNT 0xff

typedef struct ge_KeysExtension {
	int size;
	int type;
} ge_KeysExtension;

#elif(defined(PSP))

#include <pspctrl.h>

#define GEK_CROSS		0x00
#define GEK_CIRCLE		0x01
#define GEK_TRIANGLE	0x02
#define GEK_SQUARE		0x03
#define GEK_UP			0x04
#define GEK_RIGHT		0x05
#define GEK_DOWN		0x06
#define GEK_LEFT		0x07
#define GEK_LTRIGGER	0x08
#define GEK_RTRIGGER	0x09
#define GEK_START		0x0A
#define GEK_SELECT		0x0B
#define GEK_HOME		0xA0
#define GEK_HOLD		0xA1
#define GEK_NOTE		0xA2
#define GEK_SCREEN		0xA3
#define GEK_VOLUP		0xA4
#define GEK_VOLDOWN		0xA5
#define GEK_WLAN_UP		0xA6
#define GEK_REMOTE		0xA7
#define GEK_DISC		0xA8
#define GEK_MS			0xA9

#define GEK_LBUTTON		GEK_CROSS
#define GEK_MWHEELUP	0xD0
#define GEK_MWHEELDOWN	0xD1

#define GE_KEYS_COUNT	255

#define geGetKeysTable() ;

#elif(defined(PSVITA))

#define GEK_SELECT		(1<<0)
#define GEK_START		(1<<3)
#define GEK_UP			(1<<4)
#define GEK_RIGHT		(1<<5)
#define GEK_DOWN		(1<<6)
#define GEK_LEFT		(1<<7)
#define GEK_LTRIGGER	(1<<8)
#define GEK_RTRIGGER	(1<<9)
#define GEK_TRIANGLE	(1<<12)
#define GEK_CIRCLE		(1<<13)
#define GEK_CROSS		(1<<14)
#define GEK_SQUARE		(1<<15)
#define GEK_INTERCEPTED	(1<<16)
#define GEK_HOLD		(1<<17)

#define GE_KEYS_COUNT	14

#elif(defined(PSP))

#include <pspctrl.h>

#define GEK_CROSS		0x00
#define GEK_CIRCLE		0x01
#define GEK_TRIANGLE	0x02
#define GEK_SQUARE		0x03
#define GEK_UP			0x04
#define GEK_RIGHT		0x05
#define GEK_DOWN		0x06
#define GEK_LEFT		0x07
#define GEK_LTRIGGER	0x08
#define GEK_RTRIGGER	0x09
#define GEK_START		0x0A
#define GEK_SELECT		0x0B
#define GEK_HOME		0xA0
#define GEK_HOLD		0xA1
#define GEK_NOTE		0xA2
#define GEK_SCREEN		0xA3
#define GEK_VOLUP		0xA4
#define GEK_VOLDOWN		0xA5
#define GEK_WLAN_UP		0xA6
#define GEK_REMOTE		0xA7
#define GEK_DISC		0xA8
#define GEK_MS			0xA9

#define GE_KEYS_COUNT	22

#define geGetKeysTable() ;

#elif(defined(PSVITA))

#define GEK_SELECT		(1<<0)
#define GEK_START		(1<<3)
#define GEK_UP			(1<<4)
#define GEK_RIGHT		(1<<5)
#define GEK_DOWN		(1<<6)
#define GEK_LEFT		(1<<7)
#define GEK_LTRIGGER	(1<<8)
#define GEK_RTRIGGER	(1<<9)
#define GEK_TRIANGLE	(1<<12)
#define GEK_CIRCLE		(1<<13)
#define GEK_CROSS		(1<<14)
#define GEK_SQUARE		(1<<15)
#define GEK_INTERCEPTED	(1<<16)
#define GEK_HOLD		(1<<17)

#define GE_KEYS_COUNT	14

#elif(defined(PLATFORM_android))


#define GEK_LBUTTON		1
#define GEK_RBUTTON		2
#define GEK_MBUTTON		4
#define GEK_MWHEELUP	5
#define GEK_MWHEELDOWN	6
#define GEK_SHIFT		59
#define GEK_SPACE		62
#define GEK_ESCAPE		0
#define GEK_CONTROL		0
#define GEK_F11			0

#define GE_KEYS_COUNT	255

#elif(defined(PLATFORM_mac))

#define GEK_LBUTTON		1
#define GEK_MBUTTON		2
#define GEK_RBUTTON		3
#define GEK_MWHEELUP	4
#define GEK_MWHEELDOWN	5

#define GEK_ENTER		0
#define GEK_BACK		0
#define GEK_UP			0
#define GEK_DOWN		0
#define GEK_LEFT		0
#define GEK_RIGHT		0
#define GEK_SPACE		62

#define GE_KEYS_COUNT 0xff

#else //Assume Linux
//#elif(defined(LINUX))

#include <X11/keysym.h>

//X11 Keys
#define GEK_LBUTTON		1
#define GEK_MBUTTON		2
#define GEK_RBUTTON		3
#define GEK_MWHEELUP	4
#define GEK_MWHEELDOWN	5
#define GEK_BACK		XK_BackSpace-0xff00
#define GEK_TAB			XK_Tab-0xff00
#define GEK_CLEAR		XK_Clear-0xff00
#define GEK_ENTER		XK_Return-0xff00
#define GEK_LSHIFT		XK_Shift_L-0xff00
#define GEK_RSHIFT		XK_Shift_R-0xff00
#define GEK_SHIFT		GEK_LSHIFT
#define GEK_LALT		XK_Alt_L-0xff00
#define GEK_RALT		XK_Alt_R-0xff00
#define GEK_ALT			GEK_LALT
#define GEK_LCONTROL	XK_Control_L-0xff00
#define GEK_RCONTROL	XK_Control_R-0xff00
#define GEK_CONTROL		GEK_LCONTROL
#define GEK_MENU		XK_Menu-0xff00
#define GEK_PAUSE		XK_Pause-0xff00
#define GEK_ESCAPE		XK_Escape-0xff00
#define GEK_SPACE		XK_space
#define GEK_PRIOR		XK_Prior-0xff00
#define GEK_NEXT		XK_Next-0xff00
#define GEK_END			XK_End-0xff00
#define GEK_HOME		XK_Home-0xff00
#define GEK_LEFT		XK_Left-0xff00
#define GEK_UP			XK_Up-0xff00
#define GEK_RIGHT		XK_Right-0xff00
#define GEK_DOWN		XK_Down-0xff00
#define GEK_SELECT		XK_Select-0xff00
#define GEK_PRINT		XK_Print-0xff00
//#define GEK_SNAPSHOT	0xfd1d
#define GEK_INSERT		XK_Insert-0xff00
#define GEK_DELETE		XK_Delete-0xff00
#define GEK_HELP		XK_Help-0xff00
#define GEK_LSUPER		XK_Super_L-0xff00
#define GEK_RSUPER		XK_Super_R-0xff00
#define GEK_NUMPAD0		XK_KP_0-0xff00
#define GEK_NUMPAD1		XK_KP_1-0xff00
#define GEK_NUMPAD2		XK_KP_2-0xff00
#define GEK_NUMPAD3		XK_KP_3-0xff00
#define GEK_NUMPAD4		XK_KP_4-0xff00
#define GEK_NUMPAD5		XK_KP_5-0xff00
#define GEK_NUMPAD6		XK_KP_6-0xff00
#define GEK_NUMPAD7		XK_KP_7-0xff00
#define GEK_NUMPAD8		XK_KP_8-0xff00
#define GEK_NUMPAD9		XK_KP_9-0xff00
#define GEK_MULTIPLY	XK_KP_Multiply-0xff00
#define GEK_ADD			XK_KP_Add-0xff00
#define GEK_SEPARATOR	XK_KP_Separator-0xff00
#define GEK_SUBTRACT	XK_KP_Subtract-0xff00
#define GEK_DECIMAL		XK_KP_Decimal-0xff00
#define GEK_DIVIDE		XK_KP_Divide-0xff00
#define GEK_F1			XK_F1-0xff00
#define GEK_F2			XK_F2-0xff00
#define GEK_F3			XK_F3-0xff00
#define GEK_F4			XK_F4-0xff00
#define GEK_F5			XK_F5-0xff00
#define GEK_F6			XK_F6-0xff00
#define GEK_F7			XK_F7-0xff00
#define GEK_F8			XK_F8-0xff00
#define GEK_F9			XK_F9-0xff00
#define GEK_F10			XK_F10-0xff00
#define GEK_F11			XK_F11-0xff00
#define GEK_F12			XK_F12-0xff00
#define GEK_NUMLOCK		XK_Num_Lock-0xff00
#define GEK_SCROLL		XK_Scroll_Lock-0xff00
#define GEK_SHIFTLOCK	XK_Shift_Lock-0xff00

#define GE_KEYS_COUNT 0xff

#endif

#define GE_ALL_KEYS 0xffffffff
#define GEVK_ENTER		GE_KEYS_COUNT+0
#define GEVK_BACK		GE_KEYS_COUNT+1
#define GEVK_UP			GE_KEYS_COUNT+2
#define GEVK_DOWN		GE_KEYS_COUNT+3
#define GEVK_LEFT		GE_KEYS_COUNT+4
#define GEVK_RIGHT		GE_KEYS_COUNT+5
#define GEVK_ACTION1	GE_KEYS_COUNT+6
#define GEVK_ACTION2	GE_KEYS_COUNT+7
#define GEVK_ACTION3	GE_KEYS_COUNT+8
#define GEVK_ACTION4	GE_KEYS_COUNT+9
#define GEVK_ACTION5	GE_KEYS_COUNT+10
#define GEVK_ACTION6	GE_KEYS_COUNT+11
#define GEVK_ACTION7	GE_KEYS_COUNT+12
#define GEVK_ACTION8	GE_KEYS_COUNT+13
#define GEVK_ACTION9	GE_KEYS_COUNT+14
#define GEVK_ACTION10	GE_KEYS_COUNT+15
#define GEVK_ACTION11	GE_KEYS_COUNT+16
#define GEVK_ACTION12	GE_KEYS_COUNT+17
#define GEVK_ACTION13	GE_KEYS_COUNT+18
#define GEVK_ACTION14	GE_KEYS_COUNT+19
#define GEVK_ACTION15	GE_KEYS_COUNT+20
#define GEVK_ACTION16	GE_KEYS_COUNT+21
#define GEVK_ACTION17	GE_KEYS_COUNT+22
#define GEVK_ACTION18	GE_KEYS_COUNT+23
#define GEVK_ACTION19	GE_KEYS_COUNT+24
#define GEVK_ACTION20	GE_KEYS_COUNT+25
#define GEVK_ACTION21	GE_KEYS_COUNT+26
#define GEVK_ACTION22	GE_KEYS_COUNT+27
#define GEVK_ACTION23	GE_KEYS_COUNT+28
#define GEVK_ACTION24	GE_KEYS_COUNT+29
#define GEVK_ACTION25	GE_KEYS_COUNT+30
#define GEVK_ACTION26	GE_KEYS_COUNT+31



#define GEK_TOUCH_FRONT 0
#define GEK_TOUCH_BACK 1

typedef struct ge_KeysTouchReport {
	u8 used;
	u8 pressed;
	u16 force;
	u16 x;
	u16 y;
} ge_KeysTouchReport;

typedef struct ge_KeysTouch {
	int maxReports;
	ge_KeysTouchReport reports[16];
} ge_KeysTouch;

typedef struct ge_Keys {
	u8 pressed[GE_KEYS_COUNT+32];
	u8 last[GE_KEYS_COUNT+32];
	u8 assigns[32];
	ge_KeysTouch* touch;
//	ge_KeysJoystick* joysticks[2];
} ge_Keys;

LIBGE_API ge_Keys* geCreateKeys();
LIBGE_API void geKeysAssign(ge_Keys* keys, int real, int vir);
LIBGE_API bool geReadKeys(ge_Keys* keys);
LIBGE_API int geKeysToggled(ge_Keys* keys, int key);
LIBGE_API int geKeysUnToggled(ge_Keys* keys, int key);
LIBGE_API void geSetKeyRepeat(bool enabled);

#ifdef __cplusplus
}
#endif
#endif

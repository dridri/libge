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
#include <psputility.h>
#include <pspnet.h>
#include <pspnet_inet.h>
#include <pspnet_apctl.h>

static bool _ge_socket_initialized = false;
static bool _ge_socket_net_initialized = false;


static char* _ge_apctrl_states[7] = {
	"DISCONNECTED",
	"EAP_AUTH",
	"GETTING_IP",
	"GOT_IP",
	"JOINING",
	"KEY_EXCHANGE",
	"SCANNING"
};

void pspNetHandler(int oldState, int newState, int event, int error, void* pArg){
	gePrintDebug(0x100, "Net changed state from '%s' to '%s'\n", _ge_apctrl_states[oldState], _ge_apctrl_states[newState]);
	if(newState == PSP_NET_APCTL_STATE_DISCONNECTED){
		_ge_socket_net_initialized = false;
	}
}

void pspNetDialog(int language, int buttonSwap){
	if(!_ge_socket_net_initialized){
		sceUtilityLoadNetModule(PSP_NET_MODULE_COMMON);
		sceUtilityLoadNetModule(PSP_NET_MODULE_INET);
	
		sceNetInit(128*1024, 42, 4*1024, 42, 4*1024);
		sceNetInetInit();
		sceNetApctlInit(0x8000, 48);
		sceNetApctlAddHandler(&pspNetHandler, NULL);

		_ge_socket_net_initialized = true;
	}

   	pspUtilityNetconfData data;
	memset(&data, 0, sizeof(data));
	data.base.size = sizeof(data);
	data.base.language = language;
	data.base.buttonSwap = buttonSwap;
	data.base.graphicsThread = 17;
	data.base.accessThread = 19;
	data.base.fontThread = 18;
	data.base.soundThread = 16;
	data.action = PSP_NETCONF_ACTION_CONNECTAP;

	struct pspUtilityNetconfAdhoc adhocparam;
	memset(&adhocparam, 0, sizeof(adhocparam));
	data.adhocparam = &adhocparam;

	sceUtilityNetconfInitStart(&data);

	int done = 0;
	while(!done){
		geClearScreen();

		switch(sceUtilityNetconfGetStatus()){
			case PSP_UTILITY_DIALOG_NONE:
				break;

			case PSP_UTILITY_DIALOG_VISIBLE:
				sceUtilityNetconfUpdate(1);
				break;

			case PSP_UTILITY_DIALOG_QUIT:
				sceUtilityNetconfShutdownStart();
				break;

			case PSP_UTILITY_DIALOG_FINISHED:
				done = 1;
				break;
			default:
				break;
		}

		sceDisplayWaitVblankStart();
		geSwapBuffers();
	}
}

void geInitSocket(){
	if(!_ge_socket_initialized){
		_ge_socket_initialized = true;
		int mode = geDrawingMode(GE_DRAWING_MODE_2D);
		pspNetDialog(12, 1);
		geDrawingMode(mode);
	}
}

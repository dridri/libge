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
int LinuxGetLastPressed();

static bool _ge_linuxkeyboard_active = false;
static char* _ge_linuxkeyboard_out = NULL;
static int _ge_linuxkeyboard_max = 0;
static int _ge_linuxkeyboard_i = 0;
static int _ge_linuxkeyboard_maxlines = 0;
static int _ge_linuxkeyboard_nlines = 0;
static int _ge_linuxkeyboard_lastk = -1;
static int _ge_linuxkeyboard_lasttime = 0;
static bool _ge_linuxkeyboard_firstk = true;
//static int _ge_linuxkeyboard_timer = 0;

void LinuxKeyboardInit(){
	_ge_linuxkeyboard_out = NULL;
	_ge_linuxkeyboard_max = 0;
	_ge_linuxkeyboard_i = 0;
	_ge_linuxkeyboard_maxlines = 0;
	_ge_linuxkeyboard_nlines = 0;
	_ge_linuxkeyboard_lastk = -1;
	_ge_linuxkeyboard_lasttime = 0;
	_ge_linuxkeyboard_active = true;
}

void LinuxKeyboardFinished(){
	_ge_linuxkeyboard_out[_ge_linuxkeyboard_i] = 0x0;
	LinuxKeyboardInit();
	_ge_linuxkeyboard_active = false;
}

void LinuxKeyboardDescrition(const char* desc){
	if(!_ge_linuxkeyboard_active){
		return;
	}
}

void LinuxKeyboardOutput(char* ptr, int max_len, int maxlines){
	if(!_ge_linuxkeyboard_active){
		return;
	}
	_ge_linuxkeyboard_out = ptr;
	_ge_linuxkeyboard_max = max_len;
	_ge_linuxkeyboard_maxlines = maxlines - 1;
	_ge_linuxkeyboard_i = strlen(ptr);
}

void LinuxKeyboardColor(u32 color){
	if(!_ge_linuxkeyboard_active){
		return;
	}
}

int LinuxKeyboardIndex(){
	return _ge_linuxkeyboard_i;
}

void istrcpy(char* dst, char* src){
	int i;
	int len = strlen(src) - 1;
	for(i=len; i>=0; i--){
		dst[i] = src[i];
	}
}

int LinuxKeyboardUpdate(){
	if(!_ge_linuxkeyboard_active){
		return -1;
	}
	int k = LinuxGetLastPressed();
	if(k == 0){
		if(libge_context->ge_keys->pressed[GEK_LEFT]){
			k = 0xFF00 + GEK_LEFT;
		}
		if(libge_context->ge_keys->pressed[GEK_RIGHT]){
			k = 0xFF00 + GEK_RIGHT;
		}
		if(libge_context->ge_keys->pressed[GEK_DELETE]){
			k = 0xFF00 + GEK_DELETE;
		}
	}
	if(k == 127){
		k = 0xFF00 + GEK_DELETE;
	}

	if(k == _ge_linuxkeyboard_lastk){
		int waittime = 500;
		if(_ge_linuxkeyboard_firstk == false){
			waittime = 50;
		}
		if(geGetTick() - _ge_linuxkeyboard_lasttime < waittime){
			return 0;
		}else{
			_ge_linuxkeyboard_firstk = false;
		}
	}else{
		_ge_linuxkeyboard_firstk = true;
	}

	if(_ge_linuxkeyboard_out != NULL){
		if(k == GEK_BACK){
			if(_ge_linuxkeyboard_i > 0){
				strcpy(&_ge_linuxkeyboard_out[_ge_linuxkeyboard_i-1], &_ge_linuxkeyboard_out[_ge_linuxkeyboard_i]);
				_ge_linuxkeyboard_i--;
			}
		}else if(k == (0xFF00 + GEK_DELETE)){
			printf("pwet\n");
			if(_ge_linuxkeyboard_out[_ge_linuxkeyboard_i] != 0x0){
				strcpy(&_ge_linuxkeyboard_out[_ge_linuxkeyboard_i], &_ge_linuxkeyboard_out[_ge_linuxkeyboard_i+1]);
			}
		}else if(k == (0xFF00 + GEK_LEFT)){
			if(_ge_linuxkeyboard_i > 0){
				_ge_linuxkeyboard_i--;
			}
		}else if(k == (0xFF00 + GEK_RIGHT)){
			if(_ge_linuxkeyboard_out[_ge_linuxkeyboard_i] != 0x0 && _ge_linuxkeyboard_i+1 < _ge_linuxkeyboard_max){
				_ge_linuxkeyboard_i++;
			}
		}else if(k != 0 && _ge_linuxkeyboard_i+1 < _ge_linuxkeyboard_max){
			int c = k;
			if(k == GEK_ENTER){
				if(_ge_linuxkeyboard_nlines < _ge_linuxkeyboard_maxlines){
					c = '\n';
					_ge_linuxkeyboard_nlines++;
				}else{
					c = 0x0;
				}
			}
			if(c != 0x0){
				istrcpy(&_ge_linuxkeyboard_out[_ge_linuxkeyboard_i+1], &_ge_linuxkeyboard_out[_ge_linuxkeyboard_i]);
				_ge_linuxkeyboard_out[_ge_linuxkeyboard_i] = c;
				_ge_linuxkeyboard_i++;
			}
		}
		/*
		if(_ge_linuxkeyboard_i < _ge_linuxkeyboard_max){
			if(geGetTick() - _ge_linuxkeyboard_timer < 500){
				_ge_linuxkeyboard_out[_ge_linuxkeyboard_i] = '_';
			}else if(geGetTick() - _ge_linuxkeyboard_timer < 1000){
				_ge_linuxkeyboard_out[_ge_linuxkeyboard_i] = 0x00;
			}else{
				_ge_linuxkeyboard_timer = geGetTick();
			}
		}
		*/
	}
	
	_ge_linuxkeyboard_lasttime = geGetTick();
	_ge_linuxkeyboard_lastk = k;
	return 0;
}
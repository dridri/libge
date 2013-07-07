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
int WindowsGetLastPressed();

static bool _ge_winkeyboard_active = false;
static char* _ge_winkeyboard_out = NULL;
static int _ge_winkeyboard_max = 0;
static int _ge_winkeyboard_i = 0;
static int _ge_winkeyboard_maxlines = 0;
static int _ge_winkeyboard_nlines = 0;
static int _ge_winkeyboard_lastk = -1;
static int _ge_winkeyboard_lasttime = 0;
static bool _ge_winkeyboard_firstk = true;
//static int _ge_winkeyboard_timer = 0;

void WindowsKeyboardInit(){
	_ge_winkeyboard_out = NULL;
	_ge_winkeyboard_max = 0;
	_ge_winkeyboard_i = 0;
	_ge_winkeyboard_maxlines = 0;
	_ge_winkeyboard_nlines = 0;
	_ge_winkeyboard_lastk = -1;
	_ge_winkeyboard_lasttime = 0;
	_ge_winkeyboard_active = true;
}

void WindowsKeyboardFinished(){
	_ge_winkeyboard_out[_ge_winkeyboard_i] = 0x0;
	WindowsKeyboardInit();
	_ge_winkeyboard_active = false;
}

void WindowsKeyboardDescrition(const char* desc){
	if(!_ge_winkeyboard_active){
		return;
	}
}

void WindowsKeyboardOutput(char* ptr, int max_len, int maxlines){
	if(!_ge_winkeyboard_active){
		return;
	}
	_ge_winkeyboard_out = ptr;
	_ge_winkeyboard_max = max_len;
	_ge_winkeyboard_maxlines = maxlines - 1;
	_ge_winkeyboard_i = strlen(ptr);
}

void WindowsKeyboardColor(u32 color){
	if(!_ge_winkeyboard_active){
		return;
	}
}

int WindowsKeyboardIndex(){
	return _ge_winkeyboard_i;
}

void istrcpy(char* dst, char* src){
	int i;
	int len = strlen(src) - 1;
	for(i=len; i>=0; i--){
		dst[i] = src[i];
	}
}

int WindowsKeyboardUpdate(){
	if(!_ge_winkeyboard_active){
		return -1;
	}
	int k = WindowsGetLastPressed();
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

	if(k == _ge_winkeyboard_lastk){
		int waittime = 400;
		if(_ge_winkeyboard_firstk == false){
			waittime = 40;
		}
		if(geGetTick() - _ge_winkeyboard_lasttime < waittime){
			return 0;
		}else{
			_ge_winkeyboard_firstk = false;
		}
	}else{
		_ge_winkeyboard_firstk = true;
	}

	if(_ge_winkeyboard_out != NULL){
		if(k == GEK_BACK){
			if(_ge_winkeyboard_i > 0){
				strcpy(&_ge_winkeyboard_out[_ge_winkeyboard_i-1], &_ge_winkeyboard_out[_ge_winkeyboard_i]);
				_ge_winkeyboard_i--;
			}
		}else if(k == (0xFF00 + GEK_DELETE)){
			if(_ge_winkeyboard_out[_ge_winkeyboard_i] != 0x0){
				strcpy(&_ge_winkeyboard_out[_ge_winkeyboard_i], &_ge_winkeyboard_out[_ge_winkeyboard_i+1]);
			}
		}else if(k == (0xFF00 + GEK_LEFT)){
			if(_ge_winkeyboard_i > 0){
				_ge_winkeyboard_i--;
			}
		}else if(k == (0xFF00 + GEK_RIGHT)){
			if(_ge_winkeyboard_out[_ge_winkeyboard_i] != 0x0 && _ge_winkeyboard_i+1 < _ge_winkeyboard_max){
				_ge_winkeyboard_i++;
			}
		}else if(k != 0 && _ge_winkeyboard_i+1 < _ge_winkeyboard_max){
			int c = k;
			if(k == GEK_ENTER){
				if(_ge_winkeyboard_nlines < _ge_winkeyboard_maxlines){
					c = '\n';
					_ge_winkeyboard_nlines++;
				}else{
					c = 0x0;
				}
			}
			if(c != 0x0){
				istrcpy(&_ge_winkeyboard_out[_ge_winkeyboard_i+1], &_ge_winkeyboard_out[_ge_winkeyboard_i]);
				_ge_winkeyboard_out[_ge_winkeyboard_i] = c;
				_ge_winkeyboard_i++;
			}
		}
		/*
		if(_ge_winkeyboard_i < _ge_winkeyboard_max){
			if(geGetTick() - _ge_winkeyboard_timer < 500){
				_ge_winkeyboard_out[_ge_winkeyboard_i] = '_';
			}else if(geGetTick() - _ge_winkeyboard_timer < 1000){
				_ge_winkeyboard_out[_ge_winkeyboard_i] = 0x00;
			}else{
				_ge_winkeyboard_timer = geGetTick();
			}
		}
		*/
	}
	
	_ge_winkeyboard_lasttime = geGetTick();
	_ge_winkeyboard_lastk = k;
	return 0;
}
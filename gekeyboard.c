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

#include "ge_internal.h"

void (*KeyboardInit)() = NULL;
void (*KeyboardFinished)() = NULL;
void (*KeyboardDescrition)(const char* desc) = NULL;
void (*KeyboardOutput)(char* ptr, int max_len, int maxlines) = NULL;
void (*KeyboardColor)(u32 color) = NULL;
int (*KeyboardUpdate)() = NULL;
int (*KeyboardIndex)() = NULL;

void geKeyboardDefine(void* init, void* finished, void* desc, void* output, void* color, void* update, void* index){
	KeyboardInit = (void(*)())init;
	KeyboardFinished = (void(*)())finished;
	KeyboardDescrition = (void(*)(const char*))desc;
	KeyboardOutput = (void(*)(char*,int,int))output;
	KeyboardColor = (void(*)(u32))color;
	KeyboardUpdate = (int(*)())update;
	KeyboardIndex = (int(*)())index;
}

void geKeyboardInit(){
	if(KeyboardInit){
		KeyboardInit();
	}
}

void geKeyboardFinished(){
	if(KeyboardFinished){
		KeyboardFinished();
	}
}

void geKeyboardDescrition(const char* desc){
	if(KeyboardDescrition){
		KeyboardDescrition(desc);
	}
}

void geKeyboardOutput(char* ptr, int max_len, int maxlines){
	if(KeyboardOutput){
		KeyboardOutput(ptr, max_len, maxlines);
	}
}

void geKeyboardColor(u32 color){
	if(KeyboardColor){
		KeyboardColor(color);
	}
}

int geKeyboardUpdate(){
	if(KeyboardUpdate){
		return KeyboardUpdate();
	}
	return -1;
}

int geKeyboardIndex(){
	if(KeyboardIndex){
		return KeyboardIndex();
	}
	return -1;
}

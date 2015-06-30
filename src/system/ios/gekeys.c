/*
	The Gamma Engine Library is a multiplatform library made to make games
	Copyright (C) 2013  Aubry Adrien (dridri85)

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

ge_Keys* geCreateKeys(){
	ge_Keys* keys = (ge_Keys*)geMalloc(sizeof(ge_Keys));
	return keys;
}

void geKeysAssign(ge_Keys* keys, int real, int vir){
	keys->assigns[vir - GE_KEYS_COUNT] = real;
}

bool geReadKeys(ge_Keys* keys){
	memcpy(keys->last, keys->pressed, GE_KEYS_COUNT+32);
	iOSGetPressedKeys(keys->pressed);
	return true;
}

int geKeysToggled(ge_Keys* keys, int key){
	return ( keys->pressed[key] && (!keys->last[key]) );
}

int geKeysUnToggled(ge_Keys* keys, int key){
	return ( (!keys->pressed[key]) && keys->last[key] );
}

void geSetKeyRepeat(bool enabled){
//	WindowsSetKeyRepeat(enabled);
}
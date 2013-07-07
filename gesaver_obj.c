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

#define GE_TYPE_OBJ 1
#define GE_TYPE_DAE 2
#define GE_TYPE_GEO 4

void geSaveGeoList(ge_Object** objs, int count, const char* file, int flags);

void geSaveObject(ge_Object* obj, int type, const char* file){
	if(type & GE_TYPE_OBJ){
		return;
	}
	if(type & GE_TYPE_DAE){
		return;
	}
	if(type & GE_TYPE_GEO){
		geSaveGeoList(&obj, 1, file, type & 0x00FF0000);
	}
}

void geSaveObjectsList(ge_Object** objs, int count, int type, const char* file){
	if(type & GE_TYPE_OBJ){
	}else
	if(type & GE_TYPE_DAE){
	}else
	if(type & GE_TYPE_GEO){
		printf("GEO\n");
		geSaveGeoList(objs, count, file, type & 0x00FF0000);
	}
}

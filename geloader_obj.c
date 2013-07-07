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
int ObjGetType(const char* file);

void geObjectInit(ge_Object* obj);

ge_Object* geLoadObj(const char* file, const char* obj_name);
void geLoadObjListMulti(const char* file, const char** obj_list, int count, ge_Object** objs);
int geObjsCountInFile(const char* file);

ge_Object* geLoadDae(const char* file, const char* obj_name);
void geLoadDaeListMulti(const char* file, const char** obj_list, int count, ge_Object** objs, ge_Animator* animator);
int geDaeCountInFile(const char* file);
int geDaeCountInFileSubs(const char* file, const char** obj_list, int count);

ge_Object* geLoadGeo(const char* file, const char* obj_name);
void geLoadGeoListMulti(const char* file, const char** obj_list, int count, ge_Object** objs);
int geGeoCountInFile(const char* file);

ge_Object* geLoadObject(const char* file){
	int type = ObjGetType(file);

	ge_Object* obj = NULL;
	if(type == GE_TYPE_OBJ){
		obj = geLoadObj(file, "");
	}
	if(type == GE_TYPE_DAE){
		obj = geLoadDae(file, "");
	}
	if(type == GE_TYPE_GEO){
		obj = geLoadGeo(file, "");
	}

	geObjectInit(obj);
	return obj;
}

ge_Object* geLoadObjectName(const char* file, const char* name){
	int type = ObjGetType(file);

	ge_Object* obj = NULL;
	if(type == GE_TYPE_OBJ){
		obj = geLoadObj(file, name);
	}
	if(type == GE_TYPE_DAE){
		obj = geLoadDae(file, name);
	}
	if(type == GE_TYPE_GEO){
		obj = geLoadGeo(file, name);
	}

	geObjectInit(obj);
	return obj;
}

void geLoadObjectsList(const char* file, const char** list, int count, ge_Object** objs, ge_Animator* anim){
	int type = ObjGetType(file);
	gePrintDebug(0x100, "\"%s\" type: %d\n", file, type);
	if(type == GE_TYPE_OBJ){
		geLoadObjListMulti(file, list, count, objs);
	}
	if(type == GE_TYPE_DAE){
		geLoadDaeListMulti(file, list, count, objs, anim);
	}
	if(type == GE_TYPE_GEO){
		geLoadGeoListMulti(file, list, count, objs);
	}
}

int geObjectsCountInFile(const char* file){
	int type = ObjGetType(file);
	int count = 0;
	if(type == GE_TYPE_OBJ){
		count = geObjsCountInFile(file);
	}
	if(type == GE_TYPE_DAE){
		count = geDaeCountInFile(file);
	}
	if(type == GE_TYPE_GEO){
		count = geGeoCountInFile(file);
	}
	return count;
}

int geObjectsCountInFileSubs(const char* file, const char** list, int list_count){
	int type = ObjGetType(file);
	int count = 0;
	if(type == GE_TYPE_OBJ){
		count = list_count;
	}
	if(type == GE_TYPE_DAE){
		count = geDaeCountInFileSubs(file, list, list_count);
	}
	if(type == GE_TYPE_GEO){
		count = list_count;
	}
	return count;
}

void geObjectInit(ge_Object* obj){
}

void geFreeObject(ge_Object* obj){
	geFree(obj->verts);
	geFree(obj);
}

int ObjGetType(const char* file){
	char buffer[128] = "";
	ge_File* fp = geFileOpen(file, GE_FILE_MODE_READ | GE_FILE_MODE_BINARY);
	if(!fp)return -1;
	int type = -1;

	geFileGets(fp, buffer, 128);
	void* b = (void*)buffer;
	u32 magic = ((u32*)b)[0];

	printf("geSysFileGets 3 (\"%s\")\n", buffer);
	if(strstr(buffer, "OBJ")){
		type = GE_TYPE_OBJ;
	}
	if(strstr(buffer, "Obj")){
		type = GE_TYPE_OBJ;
	}
	if(strstr(buffer, "obj")){
		type = GE_TYPE_OBJ;
	}
	if(strstr(buffer, "xml")){
		geFileGets(fp, buffer, 128);
		printf("geSysFileGets 3 (\"%s\")\n", buffer);
		if(strstr(buffer, "<COLLADA")){
			type = GE_TYPE_DAE;
		}
	}
	if(magic == 0x4F4547FE){
		type = GE_TYPE_GEO;
	}
	
	geFileClose(fp);
	return type;
}

bool findInList(const char** list, int count, const char* w){
	if(!list || count <= 0){
		return true;
	}
	int i = 0;
	for(i=0; i<count; i++){
		if(!strcmp(list[i],w)){
			return true;
		}
	}
	return false;
}

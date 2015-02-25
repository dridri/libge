 /*
	The Gamma Engine Library is a useful library to make games easily
	Copyright (C) 2010  Aubry Adrien (dridri85)

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

#ifndef __GE_LUA_H__
#define __GE_LUA_H__

#include "getypes.h"
#include <lua5.2/lua.h>

typedef struct ge_LuaScript {
	char root[2048];
	char file[2048];
	char* data;
	int buf_size;

	int mode;
	ge_Thread* thread;
	lua_State *L;
	char str_error[1024];
} ge_LuaScript;

LIBGE_API void geSetLuaError(ge_LuaScript* script, const char* err, ...);
LIBGE_API char* geGetLuaError(ge_LuaScript* script);

LIBGE_API ge_LuaScript* geLoadLuaScript(const char* file);
LIBGE_API void geLuaScriptStart(ge_LuaScript* script, int mode);
LIBGE_API void geLuaDoString(ge_LuaScript* script, const char* buf);
LIBGE_API void geLuaDoFile(ge_LuaScript* script, const char* file);
LIBGE_API void geLuaCallFunction(ge_LuaScript* script, const char* funcname, const char* fmt, ...);

LIBGE_API void geLuaSetConstantInteger(ge_LuaScript* script, const char* name, int v);
LIBGE_API void geLuaSetConstantFloat(ge_LuaScript* script, const char* name, double v);
LIBGE_API void geLuaSetConstantString(ge_LuaScript* script, const char* name, char* v);
LIBGE_API void geLuaSetConstantPointer(ge_LuaScript* script, const char* name, void* v);

LIBGE_API void geLuaAddFunction(ge_LuaScript* script, void* ptr, const char* funcname, void* udata);
LIBGE_API int geLuaArgumentInteger(ge_LuaScript* script, int i);
LIBGE_API double geLuaArgumentNumber(ge_LuaScript* script, int i);
LIBGE_API const char* geLuaArgumentString(ge_LuaScript* script, int i);
LIBGE_API void* geLuaArgumentPointer(ge_LuaScript* script, int i);
LIBGE_API void geLuaReturnInteger(ge_LuaScript* script, int value);


LIBGE_API void geLuaArgumentTableOpen(ge_LuaScript* script, int i);
LIBGE_API void geLuaTableClose(ge_LuaScript* script);
LIBGE_API void geLuaTableOpen(ge_LuaScript* script, const char* name, bool global);
LIBGE_API void geLuaArgumentTableClose(ge_LuaScript* script, int i);
LIBGE_API void geLuaTableVariableIndex(ge_LuaScript* script, int index);
LIBGE_API void geLuaTableVariableName(ge_LuaScript* script, const char* name);
LIBGE_API int geLuaTableVariableInteger(ge_LuaScript* script);
LIBGE_API double geLuaTableVariableFloat(ge_LuaScript* script);
LIBGE_API bool geLuaTableVariableBoolean(ge_LuaScript* script);
LIBGE_API const char* geLuaTableVariableString(ge_LuaScript* script);
LIBGE_API void* geLuaTableVariablePointer(ge_LuaScript* script);
LIBGE_API int geLuaTableVariableIntegerByName(ge_LuaScript* script, const char* name);
LIBGE_API double geLuaTableVariableFloatByName(ge_LuaScript* script, const char* name);
LIBGE_API bool geLuaTableVariableBooleanByName(ge_LuaScript* script, const char* name);
LIBGE_API const char* geLuaTableVariableStringByName(ge_LuaScript* script, const char* name);
LIBGE_API void* geLuaTableVariablePointerByName(ge_LuaScript* script, const char* name);
LIBGE_API int geLuaTableVariableIntegerByIndex(ge_LuaScript* script, int index);
LIBGE_API double geLuaTableVariableFloatByIndex(ge_LuaScript* script, int index);
LIBGE_API const char* geLuaTableVariableStringByIndex(ge_LuaScript* script, int index);
LIBGE_API void* geLuaTableVariablePointerByIndex(ge_LuaScript* script, int index);

LIBGE_API void geLuaGetVariableInteger(ge_LuaScript* script, const char* name, int* ret);
LIBGE_API void geLuaGetVariableString(ge_LuaScript* script, const char* name, char* ret);
LIBGE_API void geLuaGetTableInteger(ge_LuaScript* script, const char* name, int i, int* ret);
LIBGE_API void geLuaGetTableString(ge_LuaScript* script, const char* name, int i, char* ret);
LIBGE_API void geLuaGetDoubleTableInteger(ge_LuaScript* script, const char* name, int i, int j, int* ret);


#endif //__GE_LUA_H__

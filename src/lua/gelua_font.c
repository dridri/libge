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

#include "ge_lua.c"

UserdataStubs(Font, ge_Font*);

static const luaL_Reg Font_meta[];

static int Font_print(lua_State* L);
static int Font_release(lua_State* L);
static int Font_measureString(lua_State* L);

static int Font_load(lua_State *L){
//	lua_lock(L);
	int argc = lua_gettop(L); 

	lua_createtable(L, 0, 5);

	if (argc != 1){
		return luaL_error(L, "Argument error: geFont.Load(filename) takes only one argument.");
	}
//	lua_gc(L, LUA_GCCOLLECT, 0);
	
	char file[2048] = "";
	if(luaL_checkstring(L, 1)[0] == '/'){
		strcpy(file, luaL_checkstring(L, 1));
	}else if(luaL_checkstring(L, 1)[0]){
		char tmp[2048] = "";
		strncpy(tmp, luaL_checkstring(L, 1), 2048);
		ge_LuaScript* script = ge_ScriptFromState(L);
		sprintf(file, "%s%s%s", script->root, script->root[0] ? "/" : "", tmp);
	}

	ge_Font* fnt = geLoadFont(file);
	*pushNewFont(L) = fnt;
	lua_setfield(L, -2, "fnt");

// 	lua_pushinteger(L, fnt->size);
// 	lua_setfield(L, -2, "size");

	*pushNewColor(L) = RGBA(255, 255, 255, 255);
	lua_setfield(L, -2, "color");

	lua_pushcfunction(L, Font_print);
	lua_setfield(L, -2, "print");

	lua_pushcfunction(L, Font_measureString);
	lua_setfield(L, -2, "measureString");

	lua_pushcfunction(L, Font_release);
	lua_setfield(L, -2, "Release");

	lua_createtable(L, 0, 0);
	luaL_setfuncs(L, Font_meta, 0);
	lua_setmetatable(L, -2);
//	lua_unlock(L);

	return 1;
}

static int Font_measureString(lua_State* L){
	int argc = lua_gettop(L);

	lua_getfield(L, 1, "fnt");
	lua_pushstring(L, "fnt");
	lua_gettable(L, 1);
	ge_Font* fnt = *toFont(L, -1);

	if(!fnt){
		return luaL_error(L, "Error: geFont.measureString() must be with a colon");
	}
	argc--;
	if(argc != 1){
		return luaL_error(L, "Error: geFont.measureString(string) must take only one arguments.");
	}
/*
	lua_getfield(L, 1, "size");
	lua_pushstring(L, "size");
	lua_gettable(L, 1);
	int sz = lua_tointeger(L, -1);

	if(sz != fnt->size){
		geFontSize(fnt, sz);
	}*/

	int w = 0;
	int h = 0;
	geFontMeasureText(fnt, luaL_checkstring(L, 2), &w, &h);

	lua_pushinteger(L, w);
	lua_pushinteger(L, h);

	return 2;
}

static int Font_print(lua_State* L){
	int argc = lua_gettop(L);

	lua_getfield(L, 1, "fnt");
	lua_pushstring(L, "fnt");
	lua_gettable(L, 1);
	ge_Font* fnt = *toFont(L, -1);

	if(!fnt){
		return luaL_error(L, "Error: geFont.print() must be with a colon");
	}
	argc--;
	if(argc != 3 && argc != 4){
		return luaL_error(L, "Error: geFont.print(x, y, string, [color]) must take 3 or 4 arguments.");
	}

	u32 color = 0;
	if(argc == 4){
		color = *toColor(L, 5);
	}else{
		lua_getfield(L, 1, "color");
		lua_pushstring(L, "color");
		lua_gettable(L, 1);
		color = *toColor(L, -1);
	}

// 	lua_getfield(L, 1, "size");
// 	lua_pushstring(L, "size");
// 	lua_gettable(L, 1);
// 	int sz = lua_tointeger(L, -1);
// 
// 	if(sz != fnt->size || fnt->texture == NULL){
// 		geFontSize(fnt, sz);
// 		fnt->size = sz;
// 		lua_pushinteger(L, fnt->size);
// 		lua_setfield(L, 1, "size");
// 	}

	lua_settop(L, argc + 1);

	geFontPrintScreen(luaL_checkint(L, 2), luaL_checkint(L, 3), fnt, luaL_checkstring(L, 4), color);

	return 1;
}

static int Font_release(lua_State* L){
	int argc = lua_gettop(L);

	lua_getfield(L, 1, "fnt");
	lua_pushstring(L, "fnt");
	lua_gettable(L, 1);
	ge_Font* fnt = *toFont(L, -1);

	if(!fnt){
		return luaL_error(L, "Error: geFont:release() must be with a colon");
	}

	geReleaseFont(fnt);

	return 1;
}

static int Font_index(lua_State* L){
	int argc = lua_gettop(L);

	lua_getfield(L, 1, "fnt");
	lua_pushstring(L, "fnt");
	lua_gettable(L, 1);
	ge_Font* fnt = *toFont(L, -1);

	const char* key = luaL_checkstring(L, 2);
// 	printf("LUA::Font_index key = \"%s\"\n", key);

	if(!strcmp(key, "size")){
		lua_pushinteger(L, fnt->size);
	}else{
		lua_pushvalue(L, 2);
		lua_rawget(L, 1);
	}

	return 1;
}

static int Font_newIndex(lua_State* L){
	int argc = lua_gettop(L);

	lua_getfield(L, 1, "fnt");
	lua_pushstring(L, "fnt");
	lua_gettable(L, 1);
	ge_Font* fnt = *toFont(L, -1);

	const char* key = luaL_checkstring(L, 2);
// 	printf("LUA::Font_newIndex key = \"%s\"\n", key);

	if(!strcmp(key, "size")){
		geFontSize(fnt, luaL_checkint(L, 3));
	}else{
		lua_pushvalue(L, 2);
		lua_pushvalue(L, 3);
		lua_rawset(L, 1);
	}

	return 1;
}

static const luaL_Reg Font_methods[] = {
	{ "load", Font_load },
	{ "print", Font_print },
	{ "measureString", Font_measureString },
	{ "Release", Font_release },
	{ NULL, NULL }
};

static const luaL_Reg Font_meta[] = {
	{ "__index", Font_index },
	{ "__newindex", Font_newIndex },
	{ NULL, NULL }
};

UserdataRegister(Font, Font_methods, Font_meta)

int geLuaInit_font(lua_State* L){
	Font_register(L);
	return 0;
}

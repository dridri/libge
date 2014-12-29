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

static int dofile(lua_State *L){
	int argc = lua_gettop(L);
	if(argc != 1) return luaL_error(L, "Argument error: dofile(file) takes one argument.");

	const char* file = luaL_checkstring(L, 1);
	ge_File* fp = geFileOpen(file, GE_FILE_MODE_READ | GE_FILE_MODE_BINARY);

	geFileSeek(fp, 0, GE_FILE_SEEK_END);
	size_t sz = geFileTell(fp);
	char* buf = (char*)geMalloc(sz + 1);

	geFileRewind(fp);
	geFileRead(fp, buf, sz);

	geFileClose(fp);

	luaL_dostring(L, buf);

	geFree(buf);
	return 0;
}

static int createMainWindow(lua_State *L){
	int argc = lua_gettop(L);
	if(argc != 4){
		return luaL_error(L, "Argument error: ge.CreateMainWindow(\"title\", width, height, flags) takes 4 argument.");
	}

	const char* title = luaL_checkstring(L, 1);
	int width = luaL_checkint(L, 2);
	int height = luaL_checkint(L, 3);
	int flags = luaL_checkint(L, 4);
	geCreateMainWindow(title, width, height, flags);
	return 0;
}

static int drawingMode(lua_State *L){
	geDrawingMode(GE_DRAWING_MODE_2D);
	return 0;
}

static int clearScreen(lua_State *L){
	geClearScreen();
	return 0;
}

static int swapBuffers(lua_State *L){
	geSwapBuffers();
	return 0;
}

static int clearMode(lua_State *L){
	int argc = lua_gettop(L);
	if(argc != 1) return luaL_error(L, "Argument error: screen.ClearMode(mode) takes one argument.");

	geClearMode(luaL_checkint(L, 1));

	return 0;
}

static int clearColor(lua_State *L){
	int argc = lua_gettop(L);
	if(argc != 1) return luaL_error(L, "Argument error: screen.ClearColor(color) takes one argument.");

	geClearColor(*(u32*)lua_touserdata(L, 1));

	return 0;
}

static int fps(lua_State *L){
	lua_pushinteger(L, geFps());
	return 1;
}

static int debugPrint(lua_State *L){
	int argc = lua_gettop(L);
	if(argc != 2) return luaL_error(L, "Argument error: geDebugPrint(mode, string) takes two argument.");

	gePrintDebug(luaL_checkinteger(L, 1), "%s\n", luaL_checkstring(L, 2));

	return 1;
}

static const luaL_Reg f_ge[] = {
	{"geCreateMainWindow", createMainWindow},
	{"geClearScreen", clearScreen},
	{"geClearColor", clearColor},
	{"geClearMode", clearMode},
	{"geSwapBuffers", swapBuffers},
	{"geFps", fps},
	{"geDrawingMode", drawingMode},
	{"geDebugPrint", debugPrint},
	{"dofile", dofile},
	{0,0}
};

int geLuaInit_ge(lua_State* L){
//	luaL_openlib(L, "ge", f_ge, 0);
//	_ge_register_lua_lib(L, "", f_ge, NULL);
	_ge_register_lua_global_functions(L, f_ge);
	
	lua_setconst(L, GE_WINDOW_FULLSCREEN);
	lua_setconst(L, GE_DRAWING_MODE_2D);
	lua_setconst(L, GE_DRAWING_MODE_3D);
	lua_setconst(L, GE_DRAWING_2D_DEPTH);
	lua_setconst(L, GE_CLEAR_COLOR_BUFFER);
	lua_setconst(L, GE_CLEAR_DEPTH_BUFFER);
	lua_setconst(L, GE_BLIT_CENTERED);

	return 0;
}

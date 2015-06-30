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

UserdataPrototypes(Font, ge_Font*);

static int dofile(lua_State *L){
	int argc = lua_gettop(L);
	if(argc != 1) return luaL_error(L, "Argument error: dofile(file) takes one argument.");

	const char* file = luaL_checkstring(L, 1);

	geLuaDoFile(ge_ScriptFromState(L), file);
/*
	ge_File* fp = geFileOpen(file, GE_FILE_MODE_READ | GE_FILE_MODE_BINARY);

	geFileSeek(fp, 0, GE_FILE_SEEK_END);
	size_t sz = geFileTell(fp);
	char* buf = (char*)geMalloc(sz + 1);

	geFileRewind(fp);
	geFileRead(fp, buf, sz);

	geFileClose(fp);

	luaL_dostring(L, buf);

	geFree(buf);
*/
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

static int getTick(lua_State *L){
	int argc = lua_gettop(L);
	if(argc != 0) return luaL_error(L, "Argument error: getTicks() takes no argument.");

	lua_pushnumber(L, geGetTickFloat());

	return 1;
}

static int ge_sleep(lua_State *L){
	int argc = lua_gettop(L);
	if(argc != 1) return luaL_error(L, "Argument error: geSleep(ms) takes one argument.");

	geSleep(luaL_checkinteger(L, 1));

	return 1;
}

static int textInput(lua_State *L){
	int argc = lua_gettop(L);
	if(argc != 3 && argc != 5) return luaL_error(L, "Argument error: geTextInput(title, initial_text, font[, draw_callback, callback_data]) takes two or four argument.");

	lua_getfield(L, 3, "fnt");
	ge_Font* font = *toFont(L, -1);

#if (defined(PLATFORM_android) || defined(PLATFORM_ios))
	ge_GuiWindow* win = NULL;
	bool done = true;
//TODO
#else
	ge_GuiWindow* win = geGuiCreateWindow(luaL_checkstring(L, 1), geGetContext()->width * 0.9, geGetContext()->width * 0.3, 0);
	geGuiStyleFont(win->style, font, font->size);
	ge_GuiInputBox* input = geGuiCreateInputBox(win->width * 0.9, font->size * 2.0, luaL_checkstring(L, 2), 32);
	ge_GuiButton* ok = geGuiCreateButton("Ok", win->width * 0.4, win->height * 0.3);
	ge_GuiButton* cancel = geGuiCreateButton("Cancel", win->width * 0.4, win->height * 0.3);
	geGuiWindowLinkObject(win, -input->width / 2, -input->height * 1.0, input, GE_GUI_ALIGNX_CENTER | GE_GUI_ALIGNY_CENTER);
	geGuiWindowLinkObject(win, -input->width / 2, input->height * 0.4, ok, GE_GUI_ALIGNX_CENTER | GE_GUI_ALIGNY_CENTER);
	geGuiWindowLinkObject(win, -input->width / 2, input->height * 0.4, cancel, GE_GUI_ALIGNX_RIGHT | GE_GUI_ALIGNY_CENTER);
	bool first_focus = false;
	bool done = false;
#endif

	float dt=0.0f, t = geGetTick() / 1000.0f;

	while(win->visible){
		geClearScreen();

		lua_pushvalue(L, 4);
		lua_pushvalue(L, 5);
		lua_pushnumber(L, t);
		lua_pushnumber(L, dt);
		lua_call(L, 3, 0);

		geSwapBuffers();

#if (defined(PLATFORM_android) || defined(PLATFORM_ios))
//TODO
#else
		if(!first_focus){
			geGuiGiveFocus(input);
			first_focus = true;
		}
		if(ok->pressed){
			done = true;
			win->visible = false;
		}
		if(cancel->pressed){
			win->visible = false;
		}
#endif

		dt = geGetTick() / 1000.0f - t;
		t = geGetTick() / 1000.0f;
	}

	if(done){
	#if (defined(PLATFORM_android) || defined(PLATFORM_ios))
		//TODO
		lua_pushstring(L, luaL_checkstring(L, 2));
	#else
		lua_pushstring(L, input->text);
	#endif
	}else{
		lua_pushstring(L, luaL_checkstring(L, 2));
	}


	return 1;
}

static int lineWidth(lua_State* L){
	int argc = lua_gettop(L);

	if(argc != 1) return luaL_error(L, "Argument error: geLineWidth(ms) takes one argument.");

	glLineWidth(luaL_checkinteger(L, 1));

	return 1;
}

static int offset(lua_State* L){
	int argc = lua_gettop(L);

	if(argc != 2) return luaL_error(L, "Argument error: geOffset(x, y) takes two arguments.");

	geDrawOffset(luaL_checknumber(L, 1), luaL_checknumber(L, 2));

	return 1;
}

static int showAds(lua_State* L){
	int argc = lua_gettop(L);

	if(argc != 1) return luaL_error(L, "Argument error: geShowAds(type) must take one argument.");

	geShowAds(luaL_checkint(L, 1));

	return 1;
}

static int memoryUsage(lua_State* L){
	int argc = lua_gettop(L);

	
	lua_createtable(L, 0, 0);
	lua_pushinteger(L, geGetContext()->mem);
	lua_setfield(L, -2, "ram");
	lua_pushinteger(L, geGetContext()->gpumem);
	lua_setfield(L, -2, "vram");

	return 1;
}


static const luaL_Reg f_ge[] = {
	{"geCreateMainWindow", createMainWindow},
	{"geClearScreen", clearScreen},
	{"geClearColor", clearColor},
	{"geClearMode", clearMode},
	{"geDrawingMode", drawingMode},
	{"geSwapBuffers", swapBuffers},
	{"geFps", fps},
	{"geMemoryUsage", memoryUsage},

	{"geLineWidth", lineWidth},
	{"geDrawOffset", offset},

	{"geTextInput", textInput},
	{"geShowAds", showAds},

	{"geDebugPrint", debugPrint},
	{"geGetTick", getTick},
	{"geSleep", ge_sleep},

	{"dofile", dofile},
	{0,0}
};

int geLuaInit_ge(lua_State* L){
//	luaL_openlib(L, "ge", f_ge, 0);
//	_ge_register_lua_lib(L, "", f_ge, NULL);
	_ge_register_lua_global_functions(L, f_ge);

	lua_setconst(L, GE_CLAMP);
	lua_setconst(L, GE_REPEAT);
	
	lua_setconst(L, GE_WINDOW_FULLSCREEN);
	lua_setconst(L, GE_DRAWING_MODE_2D);
	lua_setconst(L, GE_DRAWING_MODE_3D);
	lua_setconst(L, GE_DRAWING_2D_DEPTH);
	lua_setconst(L, GE_CLEAR_COLOR_BUFFER);
	lua_setconst(L, GE_CLEAR_DEPTH_BUFFER);
	lua_setconst(L, GE_BLIT_CENTERED);

	lua_setconst(L, GE_NEAREST);
	lua_setconst(L, GE_LINEAR);

	lua_setconst(L, GE_TRIANGLES);
	lua_setconst(L, GE_LINES);
	lua_setconst(L, GE_LINE_STRIP);

	return 0;
}

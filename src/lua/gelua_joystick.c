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

UserdataStubs(Joystick, ge_Joystick*);

static int Joystick_close(lua_State* L);
static int Joystick_read(lua_State* L);
static int Joystick_tostring(lua_State* L);

static int Joystick_number(lua_State* L){
	int argc = lua_gettop(L);

	int n = 0;
	geJoystickUpdateList(&n);
	lua_pushinteger(L, n);

	return 1;
}

static int Joystick_open(lua_State *L){
	int argc = lua_gettop(L); 

	if (argc != 1){
		return luaL_error(L, "Argument error: geJoystick.open(n) takes only one argument.");
	}

	int n = luaL_checkint(L, 1);
	ge_Joystick* js = geJoystickOpen(&geJoystickUpdateList(NULL)[n]);

	lua_createtable(L, 0, 5);

	lua_createtable(L, 0, 0);
	lua_pushcfunction(L, Joystick_tostring);
	lua_setfield(L, -2, "__tostring");
	lua_setmetatable(L, -2);

	lua_pushcfunction(L, Joystick_read);
	lua_setfield(L, -2, "read");

	lua_pushcfunction(L, Joystick_close);
	lua_setfield(L, -2, "close");

	*pushNewJoystick(L) = js;
	lua_setfield(L, -2, "handler");

	lua_createtable(L, 0, 32);
	lua_setfield(L, -2, "buttons");

	lua_pushinteger(L, 0);
	lua_setfield(L, -2, "triggerL");

	lua_pushinteger(L, 0);
	lua_setfield(L, -2, "triggerR");

	lua_createtable(L, 0, 2);
	lua_pushinteger(L, 0);
	lua_setfield(L, -2, "x");
	lua_pushinteger(L, 0);
	lua_setfield(L, -2, "y");
	lua_setfield(L, -2, "stick1");

	lua_createtable(L, 0, 2);
	lua_pushinteger(L, 0);
	lua_setfield(L, -2, "x");
	lua_pushinteger(L, 0);
	lua_setfield(L, -2, "y");
	lua_setfield(L, -2, "stick2");

//	lua_unlock(L);

	return 1;
}

static int Joystick_close(lua_State *L){
	int argc = lua_gettop(L); 

	// TODO

	return 1;
}

static int Joystick_read(lua_State* L){
	int i;
	int argc = lua_gettop(L);

	lua_getfield(L, 1, "handler");
// 	lua_pushstring(L, "handler");
// 	lua_gettable(L, 1);
	ge_Joystick* js = *toJoystick(L, -1);

	if(!js){
		return luaL_error(L, "Error: geJoystick.read() must be with a colon");
	}
	argc--;
	if(argc != 0){
		return luaL_error(L, "Error: geJoystick.read() must not take arguments.");
	}

	bool ret = geJoystickRead(js);

	lua_pushinteger(L, js->trigger_l);
	lua_setfield(L, 1, "triggerL");
	lua_pushinteger(L, js->trigger_r);
	lua_setfield(L, 1, "triggerR");

	lua_getfield(L, 1, "stick1");
	lua_pushinteger(L, js->stick1_x);
	lua_setfield(L, -2, "x");
	lua_pushinteger(L, js->stick1_y);
	lua_setfield(L, -2, "y");

	lua_getfield(L, 1, "stick2");
	lua_pushinteger(L, js->stick2_x);
	lua_setfield(L, -2, "x");
	lua_pushinteger(L, js->stick2_y);
	lua_setfield(L, -2, "y");

	lua_getfield(L, 1, "buttons");
	for(i=0; i<32; i++){
		lua_pushinteger(L, i);
		lua_pushinteger(L, js->buttons[i]);
		lua_settable(L, -3);
	}

	lua_pushboolean(L, ret);

	return 1;
}

static int Joystick_tostring(lua_State* L){
	int argc = lua_gettop(L);

	lua_getfield(L, 1, "handler");
	ge_Joystick* js = *toJoystick(L, -1);

	if(!js){
		return luaL_error(L, "Error: geJoystick.__tostring() must be with a colon");
	}

	lua_pushstring(L, js->controller->name);

	return 1;
}

static const luaL_Reg Joystick_methods[] = {
	{ "getNumber", Joystick_number },
	{ "open", Joystick_open },
	{ "close", Joystick_close },
	{ "read", Joystick_read },
	{ NULL, NULL }
};

static const luaL_Reg Joystick_meta[] = {
	{ "__tostring", Joystick_tostring },
	{ NULL, NULL }
};

UserdataRegister(Joystick, Joystick_methods, Joystick_meta)

int geLuaInit_joystick(lua_State* L){
	Joystick_register(L);
	return 0;
}

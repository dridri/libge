/*
	The Gamma Engine Library is a multiplatform library made to make games
	Copyright (C) 2015  Aubry Adrien (dridri85)

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

UserdataStubs(Socket, ge_Socket*);

static int Socket_new(lua_State *L){
	int argc = lua_gettop(L);

	if (argc != 2 && argc != 1){
		return luaL_error(L, "Argument error: geSocket.new([hostname, ]port) takes one or two arguments.");
	}
	lua_gc(L, LUA_GCCOLLECT, 0);

	if(argc == 1){
		*pushNewSocket(L) = geCreateSocket( GE_SOCKET_TYPE_SERVER, "0.0.0.0", (int)luaL_checknumber(L, 1), GE_PORT_TYPE_TCP );
	}else{
		*pushNewSocket(L) = geCreateSocket( GE_SOCKET_TYPE_CLIENT, luaL_checkstring(L, 1), (int)luaL_checknumber(L, 2), GE_PORT_TYPE_TCP );
	}

	return 1;
}

static int Socket_connect(lua_State *L){
	int argc = lua_gettop(L);
	ge_Socket* sock = selfSocket(L, &argc);

	if (!sock || argc != 0){
		return luaL_error(L, "Argument error: geSocket:connect() must be called with a colon and takes no argument");
	}

	lua_pushinteger(L, geSocketConnect(sock));

	return 1;
}

static int Socket_send(lua_State *L){
	int argc = lua_gettop(L);
	ge_Socket* sock = selfSocket(L, &argc);

	if (!sock || (argc != 1 && argc != 2)){
		return luaL_error(L, "Argument error: geSocket:send( string OR ( array[, size] ) ) must be called with a colon and takes one or two arguments");
	}

	if(lua_isstring(L, 1)){
		size_t len = 0;
		const char* str = luaL_checklstring(L, 1, &len);
		int ret = geSocketSend(sock, (char*)str, len);
		lua_pushinteger(L, ret);
	}else{
		lua_gettable(L, 1);

		int i, len = ( argc == 2 ? (int)luaL_checknumber(L, 2) : lua_rawlen(L, -1) );
		unsigned char* buf = (unsigned char*)geMalloc(len);

		for(i=0; i<len; i++){
			lua_rawgeti(L, -1, i);
			buf[i] = lua_tonumber(L, -1);
			printf("buf[%d/%d] = 0x%02X\n", i, len, buf[i]);
		}

		int ret = geSocketSend(sock, buf, len);
		geFree(buf);
	}

	return 1;
}

static int Socket_receive(lua_State *L){
	int argc = lua_gettop(L);
	ge_Socket* sock = selfSocket(L, &argc);

	char buf[16384] = "";
	int ret = geSocketReceive(sock, buf, sizeof(buf));
	buf[ret] = 0x0;
	lua_pushstring(L, buf);

	return 1;
}

static int Socket_close(lua_State *L){
	int argc = lua_gettop(L);
	ge_Socket* sock = selfSocket(L, &argc);

	if (!sock || argc != 0){
		return luaL_error(L, "Argument error: geSocket:connect() must be called with a colon and takes no argument");
	}

	geSocketShutdown(sock);
	geSocketClose(sock);
	geFree(sock);

	return 1;
}

static const luaL_Reg Socket_methods[] = {
	{ "new", Socket_new },
	{ "connect", Socket_connect },
	{ "send", Socket_send },
	{ "receive", Socket_receive },
	{ "close", Socket_close },
	{ NULL, NULL }
};

static const luaL_Reg Socket_meta[] = {
	{ NULL, NULL }
};

UserdataRegister(Socket, Socket_methods, Socket_meta)

int geLuaInit_socket(lua_State* L){
	Socket_register(L);
	return 0;
}

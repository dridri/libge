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

UserdataStubs(File, ge_File*);

static int File_open(lua_State* L);
static int File_seek(lua_State* L);
static int File_tell(lua_State* L);
static int File_read(lua_State* L);
static int File_write(lua_State* L);
static int File_close(lua_State* L);

static int File_open(lua_State *L){
	int argc = lua_gettop(L); 

	if (argc != 2){
		return luaL_error(L, "Argument error: geFile.open(filename, mode) takes two argument.");
	}
	lua_gc(L, LUA_GCCOLLECT, 0);
	
	char file[2048] = "";
	if(luaL_checkstring(L, 1)[0] == '/'){
		strcpy(file, luaL_checkstring(L, 1));
	}else if(luaL_checkstring(L, 1)[0]){
		char tmp[2048] = "";
		strncpy(tmp, luaL_checkstring(L, 1), 2048);
		ge_LuaScript* script = ge_ScriptFromState(L);
		sprintf(file, "%s%s%s", script->root, script->root[0] ? "/" : "", tmp);
	}

	const char* mode = luaL_checkstring(L, 2);
	int fmode = GE_FILE_MODE_BINARY;
	if(strchr(mode, 'r') && strchr(mode, 'w') && strchr(mode, 'a')){
		fmode |= GE_FILE_MODE_APPEND_RW;
	}else if(strchr(mode, 'w') && strchr(mode, 'a')){
		fmode |= GE_FILE_MODE_APPEND;
	}else if(strchr(mode, 'w')){
		fmode |= GE_FILE_MODE_WRITE;
	}else if(strchr(mode, 'a')){
		fmode |= GE_FILE_MODE_APPEND;
	}else if(strchr(mode, 'r')){
		fmode |= GE_FILE_MODE_READ;
	}

	ge_File* fp = geFileOpen(file, fmode);
	if(strchr(mode, 'b')){
		fp->mode |= 0x8000;
	}
	*pushNewFile(L) = fp;

	return 1;
}

static int File_seek(lua_State* L){
	int argc = lua_gettop(L);

	ge_File* fp = selfFile(L, &argc);

	if(!fp){
		return luaL_error(L, "Error: geFile:seek() must be with a colon");
	}
	if(argc != 2){
		return luaL_error(L, "Error: geFile:seek(offset, origin) must take two arguments.");
	}

	geFileSeek(fp, luaL_checkinteger(L, 1), luaL_checkinteger(L, 2));

	return 1;
}

static int File_tell(lua_State* L){
	int argc = lua_gettop(L);

	ge_File* fp = selfFile(L, &argc);

	if(!fp){
		return luaL_error(L, "Error: geFile:seek() must be with a colon");
	}
	if(argc != 0){
		return luaL_error(L, "Error: geFile:seek() takes no arguments.");
	}

	lua_pushinteger(L, geFileTell(fp));

	return 1;
}

static int File_read(lua_State* L){
	int argc = lua_gettop(L);

	ge_File* fp = selfFile(L, &argc);

	if(!fp){
		return luaL_error(L, "Error: geFile:read() must be with a colon");
	}
	if(argc != 0){
		return luaL_error(L, "Error: geFile:read() takes no arguments.");
	}

	char* line = (char*)geMalloc(2048);
	geFileGets(fp, line, 2048);
	lua_pushstring(L, line);
	geFree(line);

	return 1;
}

static int File_write(lua_State* L){
	int argc = lua_gettop(L);

	ge_File* fp = selfFile(L, &argc);

	if(!fp){
		return luaL_error(L, "Error: geFile:write(value) must be with a colon");
	}
	if(argc != 1){
		return luaL_error(L, "Error: geFile:write(value) takes one argument.");
	}

	size_t len = 0;
	const char* data = lua_tolstring(L, 1, &len);
	geFileWrite(fp, (char*)data, len);
	if(!(fp->mode & 0x8000)){
		geFileWrite(fp, "\n", 1);
	}

	return 1;
}

static int File_close(lua_State* L){
	int argc = lua_gettop(L);

	ge_File* fp = selfFile(L, &argc);

	if(!fp){
		return luaL_error(L, "Error: geFile:close() must be with a colon");
	}
	if(argc != 0){
		return luaL_error(L, "Error: geFile:close() takes no arguments.");
	}

	geFileClose(fp);

	return 1;
}

static const luaL_Reg File_methods[] = {
	{ "open", File_open },
	{ "seek", File_seek },
	{ "tell", File_tell },
	{ "read", File_read },
	{ "write", File_write },
	{ "close", File_close },
	{ NULL, NULL }
};

static const luaL_Reg File_meta[] = {
	{ NULL, NULL }
};

UserdataRegister(File, File_methods, File_meta)

int geLuaInit_file(lua_State* L){
	File_register(L);
	return 0;
}

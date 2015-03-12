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

UserdataStubs(Shader, ge_Shader*);

void getFile(lua_State *L, char* file, const char* in){
	if(in[0] == '/'){
		strcpy(file, in);
	}else if(in[0]){
		char tmp[2048] = "";
		strncpy(tmp, in, 2048);
		ge_LuaScript* script = ge_ScriptFromState(L);
		sprintf(file, "%s%s%s", script->root, script->root[0] ? "/" : "", tmp);
	}
}

static int Shader_load(lua_State *L){
	int argc = lua_gettop(L); 

	if (argc != 2){
		return luaL_error(L, "Argument error: geShader.load(vertex_shader_filename, fragment_shader_filename) takes two argument.");
	}
	lua_gc(L, LUA_GCCOLLECT, 0);
	
	char fv[2048] = "";
	char ff[2048] = "";
	getFile(L, fv, luaL_checkstring(L, 1));
	getFile(L, ff, luaL_checkstring(L, 2));

	ge_Shader* shader = geCreateShader();
	geShaderLoadVertexSource(shader, fv);
	geShaderLoadFragmentSource(shader, ff);

	*pushNewShader(L) = shader;

	return 1;
}

static int Shader_use(lua_State* L){
	int argc = lua_gettop(L);

	if(argc < 1){
		return luaL_error(L, "Error: geShader:use() must be with a colon");
	}

	ge_Shader* shader = selfShader(L, &argc);
	geShaderUse(shader);
	lua_pop(L, -2);

	return 1;
}

static int Shader_uniformID(lua_State* L){
	int argc = lua_gettop(L);
	ge_Shader* shader = selfShader(L, &argc);

	if(!shader){
		return luaL_error(L, "Error: geShader.uniformID(str) must be with a colon");
	}
	if(argc < 1){
		return luaL_error(L, "Error: geShader.uniformID(str) takes shader variable name as argument");
	}

	lua_pushinteger(L, geShaderUniformID(shader, luaL_checkstring(L, 1)));

	return 1;
}

static int Shader_uniform1f(lua_State* L){
	int argc = lua_gettop(L);
	ge_Shader* shader = selfShader(L, &argc);

	if(!shader){
		return luaL_error(L, "Error: geShader.uniformID(id, value) must be with a colon");
	}
	if(argc < 2){
		return luaL_error(L, "Error: geShader.uniformID(id, value) takes shader variable id and value as arguments");
	}

	ge_Shader* last = geShaderUse(shader);
	geShaderUniform1f(luaL_checkinteger(L, 1), luaL_checknumber(L, 2));
	geShaderUse(last);

	return 1;
}

static int Shader_uniform2f(lua_State* L){
	int argc = lua_gettop(L);
	ge_Shader* shader = selfShader(L, &argc);

	if(!shader){
		return luaL_error(L, "Error: geShader.uniformID(id, value) must be with a colon");
	}
	if(argc < 3){
		return luaL_error(L, "Error: geShader.uniformID(id, value) takes shader variable id and value as arguments");
	}

	ge_Shader* last = geShaderUse(shader);
	geShaderUniform2f(luaL_checkinteger(L, 1), luaL_checknumber(L, 2), luaL_checknumber(L, 3));
	geShaderUse(last);

	return 1;
}

static int Shader_uniform3f(lua_State* L){
	int argc = lua_gettop(L);
	ge_Shader* shader = selfShader(L, &argc);

	if(!shader){
		return luaL_error(L, "Error: geShader.uniformID(id, value) must be with a colon");
	}
	if(argc < 4){
		return luaL_error(L, "Error: geShader.uniformID(id, value) takes shader variable id and value as arguments");
	}

	ge_Shader* last = geShaderUse(shader);
	geShaderUniform3f(luaL_checkinteger(L, 1), luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4));
	geShaderUse(last);

	return 1;
}

static int Shader_uniform4f(lua_State* L){
	int argc = lua_gettop(L);
	ge_Shader* shader = selfShader(L, &argc);

	if(!shader){
		return luaL_error(L, "Error: geShader.uniformID(id, value) must be with a colon");
	}
	if(argc < 5){
		return luaL_error(L, "Error: geShader.uniformID(id, value) takes shader variable id and value as arguments");
	}

	ge_Shader* last = geShaderUse(shader);
	geShaderUniform4f(luaL_checkinteger(L, 1), luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4), luaL_checknumber(L, 5));
	geShaderUse(last);

	return 1;
}

static int Shader_getDefault(lua_State* L){
	int argc = lua_gettop(L);

	*pushNewShader(L) = NULL;

	return 1;
}

static const luaL_Reg Shader_methods[] = {
	{ "load", Shader_load },
	{ "use", Shader_use },
	{ "uniformID", Shader_uniformID },
	{ "uniform1f", Shader_uniform1f },
	{ "uniform2f", Shader_uniform2f },
	{ "uniform3f", Shader_uniform3f },
	{ "uniform4f", Shader_uniform4f },
	{ "getDefaultShader", Shader_getDefault },
	{ NULL, NULL }
};

static const luaL_Reg Shader_meta[] = {
	{ NULL, NULL }
};

UserdataRegister(Shader, Shader_methods, Shader_meta)

int geLuaInit_shader(lua_State* L){
	Shader_register(L);
	return 0;
}

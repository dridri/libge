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

UserdataStubs(Renderer, ge_Renderer*);
UserdataPrototypes(Shader, ge_Shader*);
UserdataPrototypes(VertexArray, ge_Vertex*);
UserdataPrototypes(IndiceArray, u32*);

void alloc_Renderer(lua_State* L, ge_Renderer* render, int array_idx);

static int Renderer_new(lua_State *L){
	int argc = lua_gettop(L); 

	if (argc != 2){
		return luaL_error(L, "Argument error: geRenderer.new(geShader, geVertexArray) must take two arguments.");
	}
	lua_gc(L, LUA_GCCOLLECT, 0);

	ge_Shader* shader = *toShader(L, 1);
	lua_pushstring(L, "array");
	lua_gettable(L, 2);
	ge_Vertex* array = *toVertexArray(L, -1);

	lua_pushstring(L, "size");
	lua_gettable(L, 2);
	int size = luaL_checkinteger(L, -1);

	ge_Renderer* render = geCreateRenderer(shader);
	alloc_Renderer(L, render, 2);

	render->depth_enabled = false;
	render->depth_mask = false;
	render->verts = array;
	render->nVerts = size;
	geRendererCreateContext(NULL, render);

	return 1;
}

static int Renderer_update(lua_State *L){
	int argc = lua_gettop(L); 

	if (argc != 1){
		return luaL_error(L, "Argument error: geRenderer:update() must not take arguments.");
	}

	lua_pushstring(L, "renderer");
	lua_gettable(L, 1);
	ge_Renderer* render = *toRenderer(L, -1);

	geRendererUpdateContext(NULL, render);

	return 1;
}

static int Renderer_draw(lua_State *L){
	int argc = lua_gettop(L); 

	if (argc != 2 && argc != 3){
		return luaL_error(L, "Argument error: geRenderer:draw(mode, [indices]) must take one or two arguments.");
	}

	lua_pushstring(L, "renderer");
	lua_gettable(L, 1);
	ge_Renderer* render = *toRenderer(L, -1);

	lua_pushstring(L, "array");
	lua_gettable(L, 1);
	lua_pushstring(L, "size");
	lua_gettable(L, -2);
	int size = luaL_checkinteger(L, -1);

	geRendererUse(render);
	geUpdateMatrix();

	if(argc == 3){
		lua_pushstring(L, "array");
		lua_gettable(L, 3);
		u32* array = *toIndiceArray(L, -1);
		lua_pushstring(L, "size");
		lua_gettable(L, 3);
		int count = luaL_checkinteger(L, -1);
		glDrawElements(luaL_checkinteger(L, 2), count, GL_UNSIGNED_INT, array);
	}else{
		geDrawArray(luaL_checkinteger(L, 2), 0, size);
	}
	geShaderUse(0);

	return 1;
}

static const luaL_Reg Renderer_methods[] = {
	{ "new", Renderer_new },
	{ "update", Renderer_update },
	{ "draw", Renderer_draw },
	{ NULL, NULL }
};

static const luaL_Reg Renderer_meta[] = {
	{ NULL, NULL }
};

UserdataRegister(Renderer, Renderer_methods, Renderer_meta)

void alloc_Renderer(lua_State* L, ge_Renderer* render, int array_idx){
	lua_createtable(L, 0, 0);
	luaL_setfuncs(L, Renderer_methods, 0);
	*pushNewRenderer(L) = render;
	lua_setfield(L, -2, "renderer");
	lua_pushvalue(L, array_idx);
	lua_setfield(L, -2, "array");
	lua_createtable(L, 0, 0);
	luaL_setfuncs(L, Renderer_meta, 0);
	lua_setmetatable(L, -2);
}

int geLuaInit_renderer(lua_State* L){
	Renderer_register(L);
	return 0;
}

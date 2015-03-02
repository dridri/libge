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

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

UserdataStubs(VertexArray, ge_Vertex*);
UserdataStubs(Vertex, ge_Vertex*);
UserdataStubs(IndiceArray, u32*);

void alloc_Vertex(lua_State* L, ge_Vertex* vertex);
void alloc_VertexArray(lua_State* L, ge_Vertex* array, int size);
void alloc_IndiceArray(lua_State* L, u32* array, int size);

#define ARRAY_STUB(name, type, luatype) \
UserdataStubs(name, type*); \
void alloc_##name(lua_State* L, type* array, int size); \
static int name##_new(lua_State *L){ \
	int argc = lua_gettop(L);  \
 \
	if (argc != 1){ \
		return luaL_error(L, "Argument error: ge##name##.new(size) must take one argument."); \
	} \
	lua_gc(L, LUA_GCCOLLECT, 0); \
 \
	int size = luaL_checkint(L, 1); \
	if(size <= 0){ \
		return luaL_error(L, "Argument error: ge##name##.new(size) : size must be greater than 0"); \
	} \
 \
	type* array = (type*)geMalloc(sizeof(type) * size); \
	alloc_##name(L, array, size); \
 \
	return 1; \
} \
 \
static int name##_index(lua_State* L){ \
	int argc = lua_gettop(L); \
 \
	lua_pushstring(L, "array"); \
	lua_gettable(L, 1); \
	type* array = *to##name(L, -1); \
 \
	int i = lua_tointeger(L, 2); \
	if(i >= 0){ \
		lua_push##luatype(L, array[i]); \
	}else{ \
		lua_pushnil(L); \
	} \
 \
	return 1; \
} \
 \
static int name##_newIndex(lua_State* L){ \
	int argc = lua_gettop(L); \
 \
	lua_pushstring(L, "array"); \
	lua_gettable(L, 1); \
	type* array = *to##name(L, -1); \
 \
	int i = lua_tointeger(L, 2); \
	if(i >= 0){ \
		array[i] = lua_to##luatype(L, 3); \
	} \
 \
	return 1; \
} \
 \
static int name##_propagate(lua_State* L){ \
	int argc = lua_gettop(L); \
 \
	lua_pushstring(L, "array"); \
	lua_gettable(L, 1); \
	type* array = *to##name(L, -1); \
	lua_pushstring(L, "size"); \
	lua_gettable(L, 1); \
	int size = lua_tointeger(L, -1); \
 \
	int line = lua_to##luatype(L, 2); \
	type propagation = lua_to##luatype(L, 3); \
	type reduction = lua_to##luatype(L, 4); \
	type clamp_min = lua_to##luatype(L, 5); \
	type clamp_max = lua_to##luatype(L, 6); \
float reductor = 1.0; \
	int i, i2; \
	for(i=0, i2=size-1; i<size && i2>=0; i++, i2--){ \
		array[i] = max(clamp_min, min(clamp_max, array[i] + reduction)); \
 \
		if((i + 1) % line > i % line && array[i] > array[i + 1]){ \
			array[i + 1] = max(clamp_min, min(clamp_max, array[i + 1] + (array[i] - array[i + 1]) * propagation)); \
		} \
		if(i + line < size && array[i] > array[i + line]){ \
			array[i + line] = max(clamp_min, min(clamp_max, array[i + line] + (array[i] - array[i + line]) * propagation)); \
		} \
		if((i - 1) % line >= 0 && (i - 1) % line < i % line && array[i] > array[i - 1]){ \
			array[i - 1] = max(clamp_min, min(clamp_max, array[i - 1] + (array[i] - array[i - 1]) * propagation * reductor)); \
		} \
		if(i - line > 0 && array[i] > array[i - line]){ \
			array[i - line] = max(clamp_min, min(clamp_max, array[i - line] + (array[i] - array[i - line]) * propagation * reductor)); \
		} \
		if((i2 + 1) % line > i2 % line && array[i2] > array[i2 + 1]){ \
			array[i2 + 1] = max(clamp_min, min(clamp_max, array[i2 + 1] + (array[i2] - array[i2 + 1]) * propagation)); \
		} \
		if(i2 + line < size && array[i2] > array[i2 + line]){ \
			array[i2 + line] = max(clamp_min, min(clamp_max, array[i2 + line] + (array[i2] - array[i2 + line]) * propagation)); \
		} \
		if((i2 - 1) % line >= 0 && (i2 - 1) % line < i2 % line > 0 && array[i2] > array[i2 - 1]){ \
			array[i2 - 1] = max(clamp_min, min(clamp_max, array[i2 - 1] + (array[i2] - array[i2 - 1]) * propagation * reductor)); \
		} \
		if(i2 - line > 0 && array[i2] > array[i2 - line]){ \
			array[i2 - line] = max(clamp_min, min(clamp_max, array[i2 - line] + (array[i2] - array[i2 - line]) * propagation * reductor)); \
		} \
	} \
 \
	return 1; \
} \
 \
static const luaL_Reg name##_methods[] = { \
	{ "new", name##_new }, \
	{ "propagate", name##_propagate }, \
	{ NULL, NULL } \
}; \
 \
static const luaL_Reg name##_meta[] = { \
	{ "__index", name##_index }, \
	{ "__newindex", name##_newIndex }, \
	{ NULL, NULL } \
}; \
 \
void alloc_##name(lua_State* L, type* array, int size){ \
	lua_createtable(L, 0, 0); \
	luaL_setfuncs(L, name##_methods, 0); \
	*pushNew##name(L) = array; \
	lua_setfield(L, -2, "array"); \
	lua_pushinteger(L, size); \
	lua_setfield(L, -2, "size"); \
	lua_createtable(L, 0, 0); \
	luaL_setfuncs(L, name##_meta, 0); \
	lua_setmetatable(L, -2); \
} \
UserdataRegister(name, name##_methods, name##_meta)

ARRAY_STUB(FloatArray, float, number);
ARRAY_STUB(IntegerArray, int, integer);

static int VertexArray_new(lua_State *L){
	int argc = lua_gettop(L); 

	if (argc != 1){
		return luaL_error(L, "Argument error: geVertexArray.new(size) must take one argument.");
	}
	lua_gc(L, LUA_GCCOLLECT, 0);

	int size = luaL_checkint(L, 1);
	if(size <= 0){
		return luaL_error(L, "Argument error: geVertexArray.new(size) : size must be greater than 0");
	}

	ge_Vertex* array = (ge_Vertex*)geMalloc(sizeof(ge_Vertex) * size);
	alloc_VertexArray(L, array, size);

	return 1;
}

static int VertexArray_setColor(lua_State* L){
	int argc = lua_gettop(L);

	lua_pushstring(L, "array");
	lua_gettable(L, 1);
	ge_Vertex* array = *toVertexArray(L, -1);

	int i = lua_tointeger(L, 2);
	float r = lua_tonumber(L, 3);
	float g = lua_tonumber(L, 4);
	float b = lua_tonumber(L, 5);
	float a = lua_tonumber(L, 6);

	array[i].color[0] = r;
	array[i].color[1] = g;
	array[i].color[2] = b;
	array[i].color[3] = a;

	return 1;
}

static int VertexArray_index(lua_State* L){
	int argc = lua_gettop(L);

	lua_pushstring(L, "array");
	lua_gettable(L, 1);
	ge_Vertex* array = *toVertexArray(L, -1);

	int i = luaL_checkint(L, 2);
//	printf("VertexArray_index(%d)\n", i);
	if(i >= 0 /* ??TODO?? check overflow ? */){
		alloc_Vertex(L, &array[i]);
	}else{
		lua_pushnil(L);
	}

	return 1;
}

static int VertexArray_newIndex(lua_State* L){
	int argc = lua_gettop(L);

	lua_pushstring(L, "array");
	lua_gettable(L, 1);
	ge_Vertex* array = *toVertexArray(L, -1);
//	printf("array : %p\n", array);

	int i = luaL_checkint(L, 2);
//	printf("VertexArray_newIndex(%d)\n", i);
	if(i >= 0 /* ??TODO?? check overflow ? */){
		//TODO
	}

	return 1;
}

static int VertexArray_fromArray(lua_State* L){
	int argc = lua_gettop(L);

	lua_pushstring(L, "array");
	lua_gettable(L, 1);
	ge_Vertex* array = *toVertexArray(L, -1);
	lua_pushstring(L, "size");
	lua_gettable(L, 1);
	int real_size = lua_tointeger(L, -1);

	const char* target = lua_tolstring(L, 2, NULL);
	int offset = lua_tointeger(L, 4);
	int size = lua_tointeger(L, 5);

	lua_pushstring(L, "array");
	lua_gettable(L, 3);
	float* values = *toFloatArray(L, -1);

	if(!strcmp(target, "r")){
		for(; offset<size; offset++){
//			lua_rawgeti(L, 3, offset);
//			array[offset].color[0] = lua_tonumber(L, -1);
//			lua_pop(L, 1);
			array[offset].color[0] = values[offset];
		}
	}

	return 1;
}

static int IndiceArray_new(lua_State *L){
	int argc = lua_gettop(L); 

	if (argc != 1){
		return luaL_error(L, "Argument error: geIndiceArray.new(size) must take one argument.");
	}
	lua_gc(L, LUA_GCCOLLECT, 0);

	int size = luaL_checkint(L, 1);
	if(size <= 0){
		return luaL_error(L, "Argument error: geIndiceArray.new(size) : size must be greater than 0");
	}

	u32* array = (u32*)geMalloc(sizeof(u32) * size);
	alloc_IndiceArray(L, array, size);

	return 1;
}

static int IndiceArray_index(lua_State* L){
	int argc = lua_gettop(L);

	lua_pushstring(L, "array");
	lua_gettable(L, 1);
	u32* array = *toIndiceArray(L, -1);

	int i = luaL_checkint(L, 2);
//	printf("VertexArray_index(%d)\n", i);
	if(i >= 0 /* ??TODO?? check overflow ? */){
		lua_pushunsigned(L, array[i]);
	}else{
		lua_pushnil(L);
	}

	return 1;
}

static int IndiceArray_newIndex(lua_State* L){
	int argc = lua_gettop(L);

	lua_pushstring(L, "array");
	lua_gettable(L, 1);
	u32* array = *toIndiceArray(L, -1);

	int i = luaL_checkint(L, 2);
//	printf("IndiceArray_newIndex(%d)\n", i);
	if(i >= 0 /* ??TODO?? check overflow ? */){
		array[i] = luaL_checkunsigned(L, 3);
	}

	return 1;
}

static int Vertex_index(lua_State* L){
	int argc = lua_gettop(L);

	lua_pushstring(L, "vertex");
	lua_gettable(L, 1);
	ge_Vertex* vertex = *toVertexArray(L, -1);

	const char* key = luaL_checkstring(L, 2);
//	printf("Vertex_index(%s)\n", key);

	//TODO

	return 1;
}

static int Vertex_newIndex(lua_State* L){
	int argc = lua_gettop(L);

	lua_pushstring(L, "vertex");
	lua_gettable(L, 1);
	ge_Vertex* vertex = *toVertexArray(L, -1);

	const char* key = luaL_checkstring(L, 2);
//	printf("Vertex_newIndex(%s)\n", key);

	if(!strcmp(key, "r")){
		vertex->color[0] = lua_tonumber(L, 3);
		return 1;
	}
	if(!strcmp(key, "g")){
		vertex->color[1] = lua_tonumber(L, 3);
		return 1;
	}
	if(!strcmp(key, "b")){
		vertex->color[2] = lua_tonumber(L, 3);
		return 1;
	}
	if(!strcmp(key, "a")){
		vertex->color[3] = lua_tonumber(L, 3);
		return 1;
	}
	if(!strcmp(key, "color")){
		u32 color = luaL_checkunsigned(L, 3);
		SET_COLOR(vertex->color, color);
		return 1;
	}
	if(!strcmp(key, "x")){
		vertex->x = lua_tonumber(L, 3);
		return 1;
	}
	if(!strcmp(key, "y")){
		vertex->y = lua_tonumber(L, 3);
		return 1;
		return 1;
	}
	if(!strcmp(key, "z")){
		vertex->z = lua_tonumber(L, 3);
		return 1;
		return 1;
	}
	if(!strcmp(key, "u")){
		vertex->u = lua_tonumber(L, 3);
		return 1;
		return 1;
	}
	if(!strcmp(key, "v")){
		vertex->v = lua_tonumber(L, 3);
		return 1;
	}
	if(!strcmp(key, "w")){
		vertex->w = lua_tonumber(L, 3);
		return 1;
	}
	if(!strcmp(key, "nx")){
		vertex->nx = lua_tonumber(L, 3);
		return 1;
	}
	if(!strcmp(key, "ny")){
		vertex->ny = lua_tonumber(L, 3);
		return 1;
	}
	if(!strcmp(key, "nz")){
		vertex->nz = lua_tonumber(L, 3);
		return 1;
	}

	return 1;
}

static const luaL_Reg VertexArray_methods[] = {
	{ "new", VertexArray_new },
	{ "setColor", VertexArray_setColor },
	{ "fromArray", VertexArray_fromArray },
	{ NULL, NULL }
};

static const luaL_Reg VertexArray_meta[] = {
	{ "__index", VertexArray_index },
	{ "__newindex", VertexArray_newIndex },
	{ NULL, NULL }
};

static const luaL_Reg IndiceArray_methods[] = {
	{ "new", IndiceArray_new },
	{ NULL, NULL }
};

static const luaL_Reg IndiceArray_meta[] = {
	{ "__index", IndiceArray_index },
	{ "__newindex", IndiceArray_newIndex },
	{ NULL, NULL }
};

static const luaL_Reg Vertex_methods[] = {
//	{ "create", Vertex_create },
	{ NULL, NULL }
};

static const luaL_Reg Vertex_meta[] = {
	{ "__index", Vertex_index },
	{ "__newindex", Vertex_newIndex },
	{ NULL, NULL }
};

UserdataRegister(VertexArray, VertexArray_methods, VertexArray_meta)
UserdataRegister(IndiceArray, IndiceArray_methods, IndiceArray_meta)
UserdataRegister(Vertex, Vertex_methods, Vertex_meta)

void alloc_Vertex(lua_State* L, ge_Vertex* vertex){
	lua_createtable(L, 0, 0);
	luaL_setfuncs(L, Vertex_methods, 0);
	*pushNewVertex(L) = vertex;
	lua_setfield(L, -2, "vertex");
	lua_createtable(L, 0, 0);
	luaL_setfuncs(L, Vertex_meta, 0);
	lua_setmetatable(L, -2);
}

void alloc_VertexArray(lua_State* L, ge_Vertex* array, int size){
	lua_createtable(L, 0, 0);
	luaL_setfuncs(L, VertexArray_methods, 0);
	*pushNewVertexArray(L) = array;
	lua_setfield(L, -2, "array");
	lua_pushinteger(L, size);
	lua_setfield(L, -2, "size");
	lua_createtable(L, 0, 0);
	luaL_setfuncs(L, VertexArray_meta, 0);
	lua_setmetatable(L, -2);
}

void alloc_IndiceArray(lua_State* L, u32* array, int size){
	lua_createtable(L, 0, 0);
	luaL_setfuncs(L, IndiceArray_methods, 0);
	*pushNewIndiceArray(L) = array;
	lua_setfield(L, -2, "array");
	lua_pushinteger(L, size);
	lua_setfield(L, -2, "size");
	lua_createtable(L, 0, 0);
	luaL_setfuncs(L, IndiceArray_meta, 0);
	lua_setmetatable(L, -2);
}

int geLuaInit_vertex(lua_State* L){
	IndiceArray_register(L);
	VertexArray_register(L);
	Vertex_register(L);
	FloatArray_register(L);
	IntegerArray_register(L);
	return 0;
}

#define LUA_COMPAT_MODULE
#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>
#include <lua5.2/lauxlib.h>
#include "../ge_internal.h"

#define lua_setconst(_l, name) { lua_pushnumber(_l, name); lua_setglobal(_l, #name); }
void _ge_register_lua_lib(lua_State* L, const char* name, const luaL_Reg* funcs, const luaL_Reg* metas);
void _ge_register_lua_global_functions(lua_State* L, const luaL_Reg* funcs);
ge_LuaScript* ge_ScriptFromState(lua_State* L);
int geLuaInitGraphics(lua_State* L);

#define UserdataPrototypes(HANDLE, DATATYPE) \
DATATYPE *to##HANDLE (lua_State *L, int index); \
DATATYPE self##HANDLE(lua_State *L, int* argc); \
DATATYPE* pushNew##HANDLE(lua_State *L); \
void push##HANDLE(lua_State *L, DATATYPE* data); \
int HANDLE##_register(lua_State *L);

#define UserdataStubs(HANDLE, DATATYPE) \
DATATYPE *to##HANDLE (lua_State *L, int index){ \
  DATATYPE* handle  = (DATATYPE*)lua_touserdata(L, index); \
  return handle; \
} \
\
DATATYPE* pushNew##HANDLE(lua_State *L) { \
	DATATYPE* newvalue = (DATATYPE*)lua_newuserdata(L, sizeof(DATATYPE)); \
	luaL_getmetatable(L, "ge"#HANDLE); \
	lua_setmetatable(L, -2); \
	return newvalue; \
} \
\
void push##HANDLE(lua_State *L, DATATYPE* data){ \
	lua_pushlightuserdata(L, (void*)data); \
}\
\
DATATYPE self##HANDLE(lua_State *L, int* argc){ \
	DATATYPE* ret = NULL; \
	int type = lua_type(L, 1); \
	if (type == LUA_TTABLE){ \
		lua_remove(L, 1); \
	}else if (type == LUA_TUSERDATA){ \
		ret = to##HANDLE(L, 1); \
		lua_remove(L, 1); \
	} \
	if(ret){ \
		if(argc)(*argc)--; \
		return *ret; \
	} \
	return (DATATYPE)0; \
} \
\
DATATYPE* selfPtr##HANDLE(lua_State *L, int* argc){ \
	DATATYPE* ret = NULL; \
	int type = lua_type(L, 1); \
	if (type == LUA_TTABLE){ \
		lua_remove(L, 1); \
	}else if (type == LUA_TUSERDATA){ \
		if(argc)(*argc)--; \
		ret = to##HANDLE(L, 1); \
		lua_remove(L, 1); \
	} \
	return ret; \
}


#define UserdataRegister(HANDLE, METHODS, METAMETHODS) \
int HANDLE##_register(lua_State *L){ \
	luaL_openlib(L, "ge"#HANDLE, METHODS, 0); \
	\
	luaL_newmetatable(L, "ge"#HANDLE); \
	\
	luaL_openlib(L, 0, METAMETHODS, 0); \
	lua_pushliteral(L, "__index"); \
	lua_pushvalue(L, -3); \
	lua_rawset(L, -3);  \
	lua_pushliteral(L, "__metatable"); \
	lua_pushvalue(L, -3); \
	lua_rawset(L, -3); \
	\
	lua_pop(L, 1); \
	return 1; \
}

/*
#define UserdataRegister(HANDLE, METHODS, METAMETHODS) \
int HANDLE##_register(lua_State *L){ \
	luaL_newmetatable(L, "ge"#HANDLE);  \
	lua_pushliteral(L, "__index"); \
	lua_pushvalue(L, -2);  \
	lua_rawset(L, -3);  \
	\
	luaL_openlib(L, 0, METAMETHODS, 0); \
	luaL_openlib(L, "ge"#HANDLE, METHODS, 0); \
	\
	lua_pushstring(L, "ge"#HANDLE); \
	lua_gettable(L, LUA_RIDX_GLOBALS); \
	luaL_getmetatable(L, "ge"#HANDLE); \
	lua_setmetatable(L, -2); \
	return 1; \
}
*/

/*
#define SETDEST \
	ge_Image *dest = NULL; \
	{ \
		int type = lua_type(L, 1); \
		if (type == LUA_TTABLE) lua_remove(L, 1); \
		else if (type == LUA_TUSERDATA) { \
			dest = *toImage(L, 1); \
			lua_remove(L, 1); \
		} else return luaL_error(L, "Method must be called with a colon!"); \
	}


#define SETFONTDEST \
	ge_Font *dest = NULL; \
	{ \
		int type = lua_type(L, 1); \
		if (type == LUA_TTABLE) lua_remove(L, 1); \
		else if (type == LUA_TUSERDATA) { \
			dest = *toFont(L, 1); \
			lua_remove(L, 1); \
		} else return luaL_error(L, "Method must be called with a colon!"); \
	}
*/


UserdataPrototypes(Color, u32);
UserdataPrototypes(Image, ge_Image*);
UserdataPrototypes(Font, ge_Font*);

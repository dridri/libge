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

void* luaMalloc(void* ud, void* ptr, size_t osize, size_t nsize);
void geLuaStacktrace(ge_LuaScript* script, int pmode);

int geLuaInit_ge(lua_State* L);
int geLuaInit_screen(lua_State* L);
int geLuaInit_file(lua_State* L);
int geLuaInit_image(lua_State* L);
int geLuaInit_font(lua_State* L);
int geLuaInit_shader(lua_State* L);
int geLuaInit_vertex(lua_State* L);
int geLuaInit_renderer(lua_State* L);
int geLuaInit_thread(lua_State* L);
int geLuaInit_joystick(lua_State* L);

ge_LuaScript* scripts[32] = { NULL };

ge_LuaScript* ge_ScriptFromState(lua_State* L){
	int i = 0;
	for(i=0; i<32; i++){
		if(scripts[i]->L == L){
			return scripts[i];
		}
	}
	return NULL;
}

void geSetLuaError(ge_LuaScript* script, const char* err, ...){
	va_list	opt;
	char buffer[2048];
	va_start(opt, err);
	vsnprintf( buffer, (size_t) sizeof(buffer), err, opt);
	memset(script->str_error, 0, 1024);
	strcpy(script->str_error, buffer);
}

char* geGetLuaError(ge_LuaScript* script){
	return script->str_error;
}

int geInitLua(){
	return 0;
}

void geLuaGetVariableInteger(ge_LuaScript* script, const char* name, int* ret){
	lua_settop(script->L, 0);
	lua_getglobal(script->L, name);
	if (lua_isnumber(script->L, 1)){
		*ret = lua_tonumber(script->L,1);
	}
	lua_pop(script->L, 1);
}

void geLuaGetVariableString(ge_LuaScript* script, const char* name, char* ret){
	lua_settop(script->L, 0);
	lua_getglobal(script->L, name);
	if (lua_isstring(script->L, 1)){
		strcpy(ret, lua_tostring(script->L,1));
	}
	lua_pop(script->L, 1);
}

void geLuaSetConstantInteger(ge_LuaScript* script, const char* name, int v){
	lua_pushnumber(script->L, v);
	lua_setglobal(script->L, name);
}

void geLuaSetConstantFloat(ge_LuaScript* script, const char* name, double v){
	lua_pushnumber(script->L, v);
	lua_setglobal(script->L, name);
}

void geLuaSetConstantString(ge_LuaScript* script, const char* name, char* v){
	lua_pushstring(script->L, v);
	lua_setglobal(script->L, name);
}

void geLuaSetConstantPointer(ge_LuaScript* script, const char* name, void* v){
	lua_pushlightuserdata(script->L, v);
	lua_setglobal(script->L, name);
}

typedef struct _ge_lua_userfunction {
	char* name;
	void* udata;
	void* ptr;
} _ge_lua_userfunction;
_ge_lua_userfunction* ufcts = NULL;
int nUfcts = 0;

int _ge_lua_userfunction_handler(lua_State* L){
	const char* name = lua_tostring(L, lua_upvalueindex(1));
	int i = 0;
	for(i=0; i<nUfcts; i++){
		if(!strcmp(name, ufcts[i].name)){
			break;
		}
	}
	if(i >= nUfcts){
		return luaL_error(L, "C Function '%s' not found\n", name);
	}

	int (*fct)(ge_LuaScript*,void*) = (int(*)(ge_LuaScript*,void*))ufcts[i].ptr;
	return fct(ge_ScriptFromState(L), ufcts[i].udata);
}

void geLuaAddFunction(ge_LuaScript* script, void* ptr, const char* funcname, void* udata){
	lua_pushstring(script->L, funcname);
//	lua_pushcfunction(script->L, _ge_lua_userfunction_handler);
	lua_pushcclosure(script->L, _ge_lua_userfunction_handler, 1);
	lua_setglobal(script->L, funcname);

	ufcts = (_ge_lua_userfunction*)geRealloc(ufcts, sizeof(_ge_lua_userfunction)*(nUfcts+1));
	ufcts[nUfcts].name = (char*)geMalloc(strlen(funcname)+1);
	strcpy(ufcts[nUfcts].name, funcname);
	ufcts[nUfcts].udata = udata;
	ufcts[nUfcts].ptr = ptr;
	nUfcts++;
}

int geLuaArgumentInteger(ge_LuaScript* script, int i){
	return luaL_checkint(script->L, i);
}

double geLuaArgumentNumber(ge_LuaScript* script, int i){
	return lua_tonumber(script->L, i);
}

const char* geLuaArgumentString(ge_LuaScript* script, int i){
	return lua_tostring(script->L, i);
}

void* geLuaArgumentPointer(ge_LuaScript* script, int i){
	void* ret = lua_touserdata(script->L, i);
	if(ret){
		return *((void**)ret);
	}
	return NULL;
}

static int table_index = 0;
static int cSubTable = -1;
void geLuaArgumentTableOpen(ge_LuaScript* script, int i){
	table_index = -(lua_gettop(script->L)-i);
	cSubTable = 0;
}

void geLuaTableClose(ge_LuaScript* script){
	lua_pop(script->L, 1); // geLuaOpenTable::getglobal/getfield
	cSubTable--;
}

void geLuaTableOpen(ge_LuaScript* script, const char* name, bool global){
	if(global){
		lua_settop(script->L, 0);
		lua_getglobal(script->L, name);
		cSubTable = 0;
		table_index = 0;
	}else{
		lua_getfield(script->L, -1+table_index, name);
		cSubTable++;
	}
}

void geLuaArgumentTableClose(ge_LuaScript* script, int i){
	lua_pop(script->L, 2);
}

void geLuaTableVariableIndex(ge_LuaScript* script, int index){
	if(cSubTable < 0){
		gePrintDebug(0x102, "geLuaTable* : No opened table\n");
		return;
	}
	if(cSubTable > 0){
		lua_pushnumber(script->L, index);
	}else{
	//	lua_pushnumber(script->L, index-table_index);
		lua_pushnumber(script->L, index);
	}
}

void geLuaTableVariableName(ge_LuaScript* script, const char* name){
	if(cSubTable < 0){
		gePrintDebug(0x102, "geLuaTable* : No opened table\n");
		return;
	}
	lua_pushstring(script->L, name);
}

int geLuaTableVariableInteger(ge_LuaScript* script){
	if(cSubTable < 0){
		gePrintDebug(0x102, "geLuaTable* : No opened table\n");
		return 0;
	}
	if(cSubTable > 0){
		lua_gettable(script->L, -2);
	}else{
		lua_gettable(script->L, -2+table_index);
	}
	int ret = lua_tointeger(script->L, -1);
	lua_pop(script->L, 1);
	return ret;
}

double geLuaTableVariableFloat(ge_LuaScript* script){
	if(cSubTable < 0){
		gePrintDebug(0x102, "geLuaTable* : No opened table\n");
		return 0;
	}
	if(cSubTable > 0){
		lua_gettable(script->L, -2);
	}else{
		lua_gettable(script->L, -2+table_index);
	}
	double ret = lua_tonumber(script->L, -1);
	lua_pop(script->L, 1);
	return ret;
}

bool geLuaTableVariableBoolean(ge_LuaScript* script){
	if(cSubTable < 0){
		gePrintDebug(0x102, "geLuaTable* : No opened table\n");
		return 0;
	}
	if(cSubTable > 0){
		lua_gettable(script->L, -2);
	}else{
		lua_gettable(script->L, -2+table_index);
	}
	bool ret = lua_toboolean(script->L, -1);
	lua_pop(script->L, 1);
	return ret;
}

const char* geLuaTableVariableString(ge_LuaScript* script){
	if(cSubTable < 0){
		gePrintDebug(0x102, "geLuaTable* : No opened table\n");
		return 0;
	}
	if(cSubTable > 0){
		lua_gettable(script->L, -2);
	}else{
		lua_gettable(script->L, -2+table_index);
	}
	const char* ret = lua_tostring(script->L, -1);
	lua_pop(script->L, 1);
	return ret;
}

void* geLuaTableVariablePointer(ge_LuaScript* script){
	if(cSubTable < 0){
		gePrintDebug(0x102, "geLuaTable* : No opened table\n");
		return NULL;
	}
	if(cSubTable > 0){
		lua_gettable(script->L, -2);
	}else{
		lua_gettable(script->L, -2+table_index);
	}
	void* ret = lua_touserdata(script->L, -1);
	lua_pop(script->L, 1);
	if(ret){
		return *((void**)ret);
	}
	return NULL;
}

int geLuaTableVariableIntegerByName(ge_LuaScript* script, const char* name){
	geLuaTableVariableName(script, name);
	return geLuaTableVariableInteger(script);
}
double geLuaTableVariableFloatByName(ge_LuaScript* script, const char* name){
	geLuaTableVariableName(script, name);
	return geLuaTableVariableFloat(script);
}
bool geLuaTableVariableBooleanByName(ge_LuaScript* script, const char* name){
	geLuaTableVariableName(script, name);
	return geLuaTableVariableBoolean(script);
}
const char* geLuaTableVariableStringByName(ge_LuaScript* script, const char* name){
	geLuaTableVariableName(script, name);
	return geLuaTableVariableString(script);
}
void* geLuaTableVariablePointerByName(ge_LuaScript* script, const char* name){
	geLuaTableVariableName(script, name);
	return geLuaTableVariablePointer(script);
}
int geLuaTableVariableIntegerByIndex(ge_LuaScript* script, int index){
	geLuaTableVariableIndex(script, index);
	return geLuaTableVariableInteger(script);
}
double geLuaTableVariableFloatByIndex(ge_LuaScript* script, int index){
	geLuaTableVariableIndex(script, index);
	return geLuaTableVariableFloat(script);
}
void* geLuaTableVariablePointerByIndex(ge_LuaScript* script, int index){
	geLuaTableVariableIndex(script, index);
	return geLuaTableVariablePointer(script);
}
const char* geLuaTableVariableStringByIndex(ge_LuaScript* script, int index){
	geLuaTableVariableIndex(script, index);
	return geLuaTableVariableString(script);
}
void geLuaReturnInteger(ge_LuaScript* script, int value){
	lua_pushinteger(script->L, value);
}

void geLuaCallFunction(ge_LuaScript* script, const char* funcname, const char* fmt, ...){
	va_list opt;
	va_start(opt, fmt);

	char dbg_str[1024] = "";

	int nRets = 0;
	int nArgs = 0;
	int i = 0;
	int lfmt = strlen(fmt);

	if(strchr(fmt, '=')){
		// Jump return args
		for(i=0; i<lfmt && fmt[i] != '='; i++){
			if(fmt[i] == 'i' || fmt[i] == 'f' || fmt[i] == 'd' || fmt[i] == 's'){
				sprintf(dbg_str, "%s%c%s", dbg_str, fmt[i], (i + 1 < lfmt && fmt[i + 1] != '=') ? ", " : "");
				va_arg(opt, void*);
				nRets++;
			}
		}
		i = strchr(fmt, '=') - fmt + 1;
	}

	sprintf(dbg_str, "%s%s%s(", dbg_str, strchr(fmt, '=') ? " = " : "", funcname);

	if(!strchr(funcname, '.') && !strchr(funcname, ':')){
		lua_getglobal(script->L, funcname);
	}else{
		char tmp[128];
		int i, j, k;
		bool method = false;
		for(i=0, j=0, k=0; funcname[i]; i++){
			if(funcname[i] == '.' || funcname[i] == ':'){
				tmp[j] = 0;
				if(k == 0){
					lua_getglobal(script->L, tmp);
				}else{
					lua_getfield(script->L, -1, tmp);
				//	lua_remove(script->L, -2);
				}
				memset(tmp, 0, sizeof(tmp));
				j = 0;
				k++;
				method = (funcname[i] == ':');
			}else{
				tmp[j] = funcname[i];
				j++;
			}
		}
		tmp[j] = 0;
		lua_getfield(script->L, -1, tmp);
	//	lua_remove(script->L, -2);
		if(method){
			lua_pushvalue(script->L, -2);
			nArgs++;
		}
	}

	for(; i<lfmt; i++){
		if(fmt[i] == 'i'){
			int n = va_arg(opt, int);
			sprintf(dbg_str, "%s%d", dbg_str, n);
			lua_pushinteger(script->L, n);
			nArgs++;
		}
		if(fmt[i] == 'f' || fmt[i] == 'd'){
			double d = va_arg(opt, double);
			sprintf(dbg_str, "%s%f", dbg_str, d);
			lua_pushnumber(script->L, d);
			nArgs++;
		}
		if(fmt[i] == 's'){
			char* s = va_arg(opt, char*);
			if(strlen(s) > 16){
				char tmp[32] = "\"";
				strncat(tmp, s, 16);
				strcat(tmp, "...\"");
				sprintf(dbg_str, "%s%s", dbg_str, tmp);
			}else{
				sprintf(dbg_str, "%s%s", dbg_str, s);
			}
			lua_pushstring(script->L, s);
			nArgs++;
		}
		if(i + 1 < lfmt && (fmt[i] == 'i' || fmt[i] == 'f' || fmt[i] == 'd' || fmt[i] == 's')){
			sprintf(dbg_str, "%s, ", dbg_str);
		}
	}
	va_end(opt);

	sprintf(dbg_str, "%s)", dbg_str);

// 	gePrintDebug(0x100, "Calling LUA (0x%08X) function \"%s\" with %d arguments and %d returns\n", script->L, funcname, nArgs, nRets);
// 	gePrintDebug(0x100, "Calling LUA (context: 0x%08X) function %s\n", script->L, dbg_str);
	int ret = lua_pcall(script->L, nArgs, nRets, 0);

	if(ret != 0){
		geSetLuaError(script, lua_tostring(script->L, -1));
		geLuaStacktrace(script, 0x102);
		gePrintDebug(0x102, "Lua: %s\n", script->str_error);
	}

	if(strchr(fmt, '=')){
		int n = 1;
		va_start(opt, fmt);
		for(i=0; i<lfmt && fmt[i] != '='; i++){
			if(fmt[i] == 'i'){
				*va_arg(opt, int*) = lua_tointeger(script->L, n);
				n++;
			}
			if(fmt[i] == 'f'){
				*va_arg(opt, float*) = (float)lua_tonumber(script->L, n);
				n++;
			}
			if(fmt[i] == 'd'){
				*va_arg(opt, double*) = lua_tonumber(script->L, n);
				n++;
			}
			if(fmt[i] == 's'){
				strcpy(va_arg(opt, char*), lua_tostring(script->L, n));
				n++;
			}
		}
	}
	lua_pop(script->L, -1); //lua_getglobal
}

ge_LuaScript* geLoadLuaScript(const char* file){
	ge_LuaScript* script = (ge_LuaScript*)geMalloc(sizeof(ge_LuaScript));
	if(!script)return NULL;

	if(file && file[0]){
		ge_File* fp = geFileOpen(file, GE_FILE_MODE_READ |GE_FILE_MODE_BINARY);
		if(!fp){
			geFree(script);
			return NULL;
		}
		geFileSeek(fp, 0, GE_FILE_SEEK_END);
		script->buf_size = geFileTell(fp);
		geFileRewind(fp);

		script->data = (char*)geMalloc(script->buf_size);
		if(!script->data){
			geFree(script);
			geFileClose(fp);
			return NULL;
		}
		memset(script->data, 0, script->buf_size);
		geFileRead(fp, script->data, script->buf_size);

		int i = 0;
		for(i=0; i<32; i++){
			if(!scripts[i]){
				scripts[i] = script;
				break;
			}
		}
		
		strcpy(script->file, file);
		strcpy(script->root, file);
		if(strchr(script->root, '/')){
			strrchr(script->root, '/')[0] = 0x0;
		}else{
			memset(script->root, 0x0, sizeof(script->root));
		}

		geFileClose(fp);
	}

#ifdef LIBGE_LUAJIT
	script->L = lj_state_newstate(luaMalloc, NULL);
#else
	script->L = lua_newstate(luaMalloc, NULL);
#endif
	luaL_openlibs(script->L);
	geLuaInit_ge(script->L);
	geLuaInit_screen(script->L);
	geLuaInit_file(script->L);
	geLuaInit_image(script->L);
	geLuaInit_font(script->L);
	geLuaInit_shader(script->L);
	geLuaInit_vertex(script->L);
	geLuaInit_renderer(script->L);
	geLuaInit_thread(script->L);
	geLuaInit_joystick(script->L);

	if(file && file[0] && script->data && script->buf_size > 0){
		int s = luaL_loadbuffer(script->L, script->data, script->buf_size, NULL);
		if(s != 0){
			geSetLuaError(script, lua_tostring(script->L, -1));
			lua_pop(script->L, 1);
		}
	}

	return script;
}

void geLuaDoString(ge_LuaScript* script, const char* buf){
	luaL_loadstring(script->L, buf);
	int ret = lua_pcall(script->L, 0, LUA_MULTRET, 0);

	if(ret != 0){
		geSetLuaError(script, lua_tostring(script->L, -1));
		gePrintDebug(0x102, "geLuaDoString: %s\n", script->str_error);
	}
}

void geLuaDoFile(ge_LuaScript* script, const char* file){
	ge_File* fp = geFileOpen(file, GE_FILE_MODE_READ | GE_FILE_MODE_BINARY);

	if(!fp){
		geSetLuaError(script, "File not found");
		return;
	}

	geFileSeek(fp, 0, GE_FILE_SEEK_END);
	size_t sz = geFileTell(fp);
	char* buf = (char*)geMalloc(sz + 1);

	geFileRewind(fp);
	geFileRead(fp, buf, sz);

	geFileClose(fp);

	luaL_loadstring(script->L, buf);
	int ret = lua_pcall(script->L, 0, LUA_MULTRET, 0);
	if(ret != 0){
		geSetLuaError(script, lua_tostring(script->L, -1));
		gePrintDebug(0x102, "geLuaDoFile ['%s']: %s\n", file, script->str_error);
	}

	geFree(buf);
}

void geLuaStacktrace(ge_LuaScript* script, int pmode){
	lua_Debug entry;
	int depth = 0;

	bool crit = geDebugCritical(false);
	gePrintDebug(pmode, "LUA Stack trace :\n");

	while (lua_getstack(script->L, depth, &entry))
	{
		int status = lua_getinfo(script->L, "Sln", &entry);

		gePrintDebug(pmode, "%s(%d): %s\n", entry.short_src, entry.currentline, entry.name ? entry.name : "?");
		depth++;
	}

	geDebugCritical(crit);
}


void* luaMalloc(void* ud, void* ptr, size_t osize, size_t nsize){
	if(nsize == 0){
		geFree(ptr);
		return NULL;
	}
	return geRealloc(ptr, nsize);
}

void _ge_register_lua_lib(lua_State* L, const char* name, const luaL_Reg* funcs, const luaL_Reg* metas){
	int nFuncs = 0;
	while(funcs[nFuncs++].func != 0);
	lua_createtable(L, 0, nFuncs);
    luaL_setfuncs(L, funcs, 0);
	if(metas){
		luaL_newmetatable(L, name);
		luaL_setfuncs(L, metas, 0);
		lua_setmetatable(L, -2);
	}
    lua_setglobal(L, name);
	lua_pop(L, 1);
}

void _ge_register_lua_global_functions(lua_State* L, const luaL_Reg* funcs){
	luaL_Reg* fct;
	for(fct=(luaL_Reg*)funcs; fct->func != NULL; fct++){
		lua_pushcfunction(L, fct->func);
		lua_setglobal(L, fct->name);
	}
}


int _ge_lua_start_script(int args, void* argp){
	ge_LuaScript* script = (ge_LuaScript*)((t_ptr*)argp)[0];
	int ret = 0;

	ret = lua_pcall(script->L, 0, LUA_MULTRET, 0);

	if(ret != 0){
		geSetLuaError(script, lua_tostring(script->L, -1));
		geLuaStacktrace(script, 0x102);
		gePrintDebug(0x102, "Lua_start: %s\n", script->str_error);
	}

	return 0;
}

void geLuaScriptStart(ge_LuaScript* script, int mode){
	script->mode = mode;
	t_ptr argp[2] = { (t_ptr)script, 0x0 };

	if(mode & GE_LUA_EXECUTION_MODE_NORMAL){
		_ge_lua_start_script(sizeof(argp), argp);
	}else
	if(mode & GE_LUA_EXECUTION_MODE_PARALLEL){
		int flags = GE_THREAD_PRIORITY_NORMAL;
		if(mode & GE_THREAD_SHARED_GRAPHIC_CONTEXT){
			flags |= GE_THREAD_SHARED_GRAPHIC_CONTEXT;
		}
		ge_Thread* thread = geCreateThread("lua_script_thread", _ge_lua_start_script, flags);
		script->thread = thread;
		geThreadStart(thread, sizeof(argp), argp);
	}
}

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

#include <pthread.h>
#include "ge_lua.c"

typedef struct ge_LuaThread {
	ge_LuaScript* script;
	ge_Thread* thread;
	lua_State* L;
	char* entry;
	int nArgs;
	char** args;
	bool started;
} ge_LuaThread;

UserdataStubs(Thread, ge_LuaThread*);

extern ge_LuaScript* scripts[32];
static int locked[32] = { 0 };

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_t self_thread = 0;

void ge_LuaLock(lua_State* L){
// 	printf("ge_LuaLock()\n");

	int i = 0;
	for(i=0; i<32; i++){
		if(scripts[i]->L == L){
			break;
		}
	}
	if(i >= 32){
		gePrintDebug(0x102, "ge_LuaLock: unknown script\n");
	}

//	if(locked[i] == 0){
		locked[i] = 1;
		pthread_mutex_lock(&mutex);
//	}
}

void ge_LuaUnlock(lua_State* L){
// 	printf("ge_LuaUnlock()\n");

	int i = 0;
	for(i=0; i<32; i++){
		if(scripts[i]->L == L){
			break;
		}
	}
	if(i >= 32){
		gePrintDebug(0x102, "ge_LuaUnlock: unknown script\n");
	}

//	if(locked[i] == 1){
		pthread_mutex_unlock(&mutex);
		locked[i] = 0;
//	}
}

static void push_value(ge_LuaThread* thread, const char* name){
	ge_LuaScript* script = thread->script;
	if(!strchr(name, '.')){
		lua_getglobal(script->L, name);
	}else{
		char tmp[128];
		int i, j, k;
		memset(tmp, 0, sizeof(tmp));
		for(i=0, j=0, k=0; name[i]; i++){
			if(name[i] == '.'){
				tmp[j] = 0;
				if(k == 0){
					lua_getglobal(script->L, tmp);
				}else{
					lua_getfield(script->L, -1, tmp);
					lua_remove(script->L, -2);
				}
				memset(tmp, 0, sizeof(tmp));
				j = 0;
				k++;
			}else{
				tmp[j] = name[i];
				j++;
			}
		}
		tmp[j] = 0;
		lua_getfield(script->L, -1, tmp);
		lua_remove(script->L, -2);
	}
}

static int ge_LuaThreadEntry(int args, void* argp){
	ge_LuaThread* thread = (ge_LuaThread*)argp;
	ge_LuaScript* script = thread->script;

	printf("ge_LuaThreadEntry() [entry=%s] [L=%p]\n", thread->entry, thread->L);
//	lua_lock(script->L);
	printf("lock ok\n");

	int i;
	char dbg_str[1024] = "";
	sprintf(dbg_str, "%s%s(", dbg_str, thread->entry);

	push_value(thread, thread->entry);
	printf("top : %d\n", lua_gettop(script->L));
	for(i=0; i<thread->nArgs; i++){
		push_value(thread, thread->args[i]);
		sprintf(dbg_str, "%s%s%s", dbg_str, i>0?", ":"", thread->args[i]);
	}
	printf("top : %d\n", lua_gettop(script->L));

	thread->started = true;

	sprintf(dbg_str, "%s)", dbg_str);

//	lua_unlock(script->L);
	gePrintDebug(0x100, "ge_LuaThreadEntry[%p] : Calling function %s\n", script->L, dbg_str);

	lua_call(script->L, thread->nArgs, 0);
/*
	int ret = lua_pcall(script->L, thread->nArgs, 0, 0);
	if(ret != 0){
		geSetLuaError(script, lua_tostring(script->L, -1));
		gePrintDebug(0x102, "Lua: %s\n", script->str_error);
	}
*/
	return 0;
}

void* luaMalloc(void* ud, void* ptr, size_t osize, size_t nsize);
static int Thread_new(lua_State *L){
	int argc = lua_gettop(L);

	if (argc != 2){
		return luaL_error(L, "Argument error: geThread.new(name, entry) must take two arguments.");
	}

	ge_LuaThread* thread = (ge_LuaThread*)geMalloc(sizeof(ge_LuaThread));
	thread->thread = geCreateThread(luaL_checkstring(L, 1), ge_LuaThreadEntry, 0);
 	thread->L = lua_newthread(L);
//	thread->L = lua_newstate(luaMalloc, NULL);

	thread->entry = strdup(luaL_checkstring(L, 2));
	*pushNewThread(L) = thread;

	thread->script = (ge_LuaScript*)geMalloc(sizeof(ge_LuaScript));
//	memcpy(thread->script, ge_ScriptFromState(L), sizeof(ge_LuaScript));
	thread->script->thread = thread->thread;
	thread->script->L = thread->L;

	int i = 0;
	for(i=0; i<32; i++){
		if(!scripts[i]){
			scripts[i] = thread->script;
			break;
		}
	}

	return 1;
}

static int Thread_start(lua_State* L){
	int argc = lua_gettop(L);

	if(argc < 1){
		return luaL_error(L, "Error: geThread:start(...) must be with a colon");
	}

	ge_LuaThread* thread = selfThread(L, &argc);

	int i;
	thread->nArgs = argc;
	thread->args = (char**)malloc(sizeof(char*) * argc);
	printf("argc : %d\n", argc);
	for(i=0; i<argc; i++){
		thread->args[i] = strdup(luaL_checkstring(L, i + 1));
	}

	geThreadStart(thread->thread, sizeof(ge_LuaThread*), thread);
	while(thread->started)geSleep(1);

	return 1;
}

static const luaL_Reg Thread_methods[] = {
	{ "new", Thread_new },
	{ "start", Thread_start },
	{ NULL, NULL }
};

static const luaL_Reg Thread_meta[] = {
	{ NULL, NULL }
};

UserdataRegister(Thread, Thread_methods, Thread_meta)

int geLuaInit_thread(lua_State* L){
	Thread_register(L);
	return 0;
}

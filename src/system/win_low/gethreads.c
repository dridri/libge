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

#include "../../ge_internal.h"
#include <windows.h>

static ge_Thread* threads[32] = { NULL };
bool RegisterThread(ge_Thread* thread);
ge_Thread* getThreadByHandle(u32 handle);
ge_Thread* getThreadById(u32 id);

ge_Thread* geCreateThread(const char* name, void* func, int flags){
	ge_Thread* thread = (ge_Thread*)geMalloc(sizeof(ge_Thread));
	if(!thread)return NULL;

	strcpy(thread->name, name);
	thread->priority = flags & 64;
	thread->flags = flags & 0xFFFFFF80;
	thread->func = func;

	if(thread->flags & GE_THREAD_SHARED_GRAPHIC_CONTEXT){
		HGLRC hRC = wglCreateContext(((LibGE_WinlowContext*)libge_context->syscontext)->hDC);
		wglShareLists(((LibGE_WinlowContext*)libge_context->syscontext)->hRC, hRC);
		thread->graphic_ctx = (t_ptr)hRC;
	}else
	if(thread->flags & GE_THREAD_GRAPHIC_CONTEXT){
		HGLRC hRC = wglCreateContext(((LibGE_WinlowContext*)libge_context->syscontext)->hDC);
		thread->graphic_ctx = (t_ptr)hRC;
	}

	RegisterThread(thread);

	return thread;
}

void geFreeThread(ge_Thread* thread){
	int i = 0;
	for(i=0; i<64; i++){
		if(threads[i] == thread){
			threads[i] = NULL;
		}
	}
	TerminateThread((HANDLE)thread->handle, 0);
	geFree(thread);
}

int thread_start_point(void* p){
	t_ptr* argp = (t_ptr*)p;
	int (*_ge_func)(int args, void* argp) = (int(*)(int,void*))argp[0];
	ge_Thread* thread = (ge_Thread*)argp[1];
	int args = argp[2];
	void* real_argp = (void*)argp[3];
	
	if(thread->flags & GE_THREAD_SHARED_GRAPHIC_CONTEXT || thread->flags & GE_THREAD_GRAPHIC_CONTEXT){
		wglMakeCurrent(((LibGE_WinlowContext*)libge_context->syscontext)->hDC, (HGLRC)thread->graphic_ctx);
	}
	
	int ret = _ge_func(args, real_argp);
	
	if(thread->flags & GE_THREAD_SHARED_GRAPHIC_CONTEXT || thread->flags & GE_THREAD_GRAPHIC_CONTEXT){
		wglMakeCurrent(NULL, NULL);
	}

	thread->handle = 0;
	thread->id = 0;
	
	ExitThread(ret);
	return ret;
}

void geThreadStart(ge_Thread* thread, int args, void* argp){
	t_ptr* p = (t_ptr*)malloc(sizeof(t_ptr)*4);
	p[0] = (t_ptr)thread->func;
	p[1] = (t_ptr)thread;
	p[2] = args;
	p[3] = (t_ptr)argp;

	if(thread->handle){
		CloseHandle((HANDLE)thread->handle);
	}

	LPDWORD Id = 0;
	HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_start_point, (LPVOID)p, 0, Id);
	thread->handle = (t_ptr)hThread;
	thread->id = (t_ptr)Id;

	int pr = THREAD_PRIORITY_NORMAL;
	if(thread->priority == GE_THREAD_PRIORITY_TIME_CRITICAL){
		pr = THREAD_PRIORITY_TIME_CRITICAL;
	}
	if(thread->priority == GE_THREAD_PRIORITY_LOWEST){
		pr = THREAD_PRIORITY_LOWEST;
	}
	SetThreadPriority(hThread, pr);
}

void geThreadResume(ge_Thread* thread){
	ResumeThread((HANDLE)thread->handle);
}

void geThreadSuspend(ge_Thread* thread){
	SuspendThread((HANDLE)thread->handle);
}

void geThreadSleep(){
	SuspendThread(GetCurrentThread());
}

void geThreadExit(int code){
	ge_Thread* thread = NULL;
	thread = getThreadById(GetCurrentThreadId());

	if(thread){
		thread->exit_code = code;
	}

	ExitThread(0);
}

void geThreadTerminate(ge_Thread* thread){
	TerminateThread((HANDLE)thread->handle, 0);
}

bool RegisterThread(ge_Thread* thread){
	int i = 0;
	ge_Thread* ret = NULL;
	for(i=0; i<32; i++){
		if(!threads[i]){
			threads[i] = thread;
			ret = threads[i];
			break;
		}
	}
	return ret;
}

ge_Thread* getThreadByHandle(u32 handle){
	int i = 0;
	for(i=0; i<32; i++){
		if(threads[i] && threads[i]->handle == handle){
			return threads[i];
		}
	}
	return NULL;
}

ge_Thread* getThreadById(u32 id){
	int i = 0;
	for(i=0; i<32; i++){
		if(threads[i] && threads[i]->id == id){
			return threads[i];
		}
	}
	return NULL;
}

void ge_PauseAllThreads(){
	int i = 0;
	for(i=0; i<32; i++){
		if(threads[i] && threads[i]->id != GetCurrentThreadId()){
			geThreadSuspend(threads[i]);
		}
	}
}

void ge_ResumeAllThreads(){
	int i = 0;
	for(i=0; i<32; i++){
		if(threads[i] && threads[i]->id != GetCurrentThreadId()){
			geThreadResume(threads[i]);
		}
	}
}

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
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
static void SIGUSR1Handler(int errno);
static void SIGUSR2Handler(int errno);

static ge_Thread* threads[32];
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
		LibGE_LinuxContext* context = (LibGE_LinuxContext*)libge_context->syscontext;
		thread->graphic_ctx = (t_ptr)glXCreateContext(context->dpy, context->vi, context->ctx, true);
	}else
	if(thread->flags & GE_THREAD_GRAPHIC_CONTEXT){
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
	geThreadTerminate(thread);
	geFree(thread);
}

int thread_start_point(void* p){
	unsigned long* argp = (unsigned long*)p;
	int (*_ge_func)(int args, void* argp) = (int(*)(int,void*))argp[0];
	ge_Thread* thread = (ge_Thread*)argp[1];
	int args = argp[2];
	void* real_argp = (void*)argp[3];
	
	if(thread->flags & GE_THREAD_SHARED_GRAPHIC_CONTEXT || thread->flags & GE_THREAD_GRAPHIC_CONTEXT){
		LibGE_LinuxContext* context = (LibGE_LinuxContext*)libge_context->syscontext;
		glXMakeCurrent(context->dpy, context->win, (GLXContext)thread->graphic_ctx);
	}

	signal(SIGUSR1, (void*)&SIGUSR1Handler);
	signal(SIGUSR2, (void*)&SIGUSR2Handler);
	
	int ret = _ge_func(args, real_argp);
	
	if(thread->flags & GE_THREAD_SHARED_GRAPHIC_CONTEXT || thread->flags & GE_THREAD_GRAPHIC_CONTEXT){
		//release context
	}

	thread->handle = 0;
	thread->id = 0;

	free(p);
	int ret2 = 0;
	pthread_exit(&ret2);
	return ret;
}

void geThreadStart(ge_Thread* thread, int args, void* argp){
	unsigned long* p = (unsigned long*)malloc(sizeof(unsigned long)*4);
	p[0] = (unsigned long)thread->func;
	p[1] = (unsigned long)thread;
	p[2] = args;
	p[3] = (unsigned long)argp;

	pthread_t hThread;
	pthread_attr_t attr;

//	void* stack = malloc(32 * 1024 * 1024);
//	pthread_attr_init(&attr);
//	pthread_attr_setstack(&attr, &stack, 32 * 1024 * 1024);

	pthread_create(&hThread, NULL, (void*)&thread_start_point, p);
	thread->handle = (unsigned long)hThread;
	thread->id = (unsigned long)hThread;
}

void geThreadResume(ge_Thread* thread){
	thread->paused = false;
	pthread_kill((pthread_t)thread->handle, SIGCONT);
}

void geThreadSuspend(ge_Thread* thread){
	thread->paused = true;

	int ret = 0;
	ret = pthread_kill((pthread_t)thread->handle, SIGUSR1);
}

void geThreadSleep(){
	geThreadSuspend(getThreadByHandle(pthread_self()));
}

void geThreadExit(int code){
	ge_Thread* thread = NULL;
	thread = getThreadById(pthread_self());

	if(thread)thread->exit_code=code;

	int ret = 0;
	pthread_exit(&ret);
	if(thread)thread->returned=ret;
}

void geThreadTerminate(ge_Thread* thread){
	pthread_kill((pthread_t)thread->handle, SIGUSR2);
}

static void SIGUSR1Handler(int errno){
	ge_Thread* thread = getThreadByHandle(pthread_self());
	while(thread->paused){
		usleep(100000);
	}
}

static void SIGUSR2Handler(int errno){
	geThreadExit(0);
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
		if(threads[i] && threads[i]->handle != pthread_self()){
			geThreadSuspend(threads[i]);
		}
	}
}

void ge_ResumeAllThreads(){
	int i = 0;
	for(i=0; i<32; i++){
		if(threads[i] && threads[i]->handle != pthread_self()){
			geThreadResume(threads[i]);
		}
	}
}

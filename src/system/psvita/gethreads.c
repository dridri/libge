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

static ge_Thread* threads[32];
bool RegisterThread(ge_Thread* thread);
ge_Thread* getThreadByHandle(u32 handle);
ge_Thread* getThreadById(u32 id);

ge_Thread* geCreateThread(const char* name, void* func, int prio){
	ge_Thread* thread = (ge_Thread*)geMalloc(sizeof(ge_Thread));
	if(!thread)return NULL;

	strcpy(thread->name, name);
	thread->priority = prio;
	thread->func = func;
	thread->stackSize = 0x2000;

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
	sceKernelTerminateThread(thread->handle);
	sceKernelDeleteThread(thread->handle);
	geFree(thread);
}

int thread_start_point(void* p){
	u32* argp = (u32*)p;
	int (*_ge_func)(int args, void* argp) = (int(*)(int,void*))argp[0];
	int args = argp[1];

	void* real_argp = &argp[2];

	return _ge_func(args, real_argp);
}

void geThreadStart(ge_Thread* thread, int args, void* argp){
	int pri = 30;
	if(thread->priority == GE_THREAD_PRIORITY_TIME_CRITICAL){
		pri = 18;
	}
	thread->handle = sceKernelCreateThread(thread->name, (SceKernelThreadEntry)thread->func, pri, thread->stackSize, PSP_THREAD_ATTR_USER | PSP_THREAD_ATTR_VFPU, NULL);
	thread->id = 0;
	sceKernelStartThread(thread->handle, args, argp);
}

void geThreadResume(ge_Thread* thread){
	sceKernelResumeThread(thread->handle);
}

void geThreadSuspend(ge_Thread* thread){
	sceKernelSuspendThread(thread->handle);
}

void geThreadSleep(){
	sceKernelSleepThread();
}

void geThreadExit(int code){
	ge_Thread* thread = getThreadByHandle(sceKernelGetThreadId());
	geFree(thread);
	sceKernelExitThread(code);
}

void geThreadTerminate(ge_Thread* thread){
	if(!thread)return;
	sceKernelTerminateThread(thread->handle);
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
		if(threads[i] && threads[i]->id != sceKernelGetThreadId()){
			geThreadSuspend(threads[i]);
		}
	}
}

void ge_ResumeAllThreads(){
	int i = 0;
	for(i=0; i<32; i++){
		if(threads[i] && threads[i]->id != sceKernelGetThreadId()){
			geThreadResume(threads[i]);
		}
	}
}

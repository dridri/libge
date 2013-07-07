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

#include <malloc.h>
#include <string.h>
#include "../../ge_internal.h"

#ifndef ALIGN
	#define ALIGN(x, align) (((x)+((align)-1))&~((align)-1))
#endif

#define gePrintDebug(...) ;

static t_ptr AllocMemBlock(int size){
	void* ret = malloc(size);
	if((t_ptr)ret <= 0){
		gePrintDebug(0x102, "malloc(%d) returned %p !\n", size, ret);
	}

	return (t_ptr)ret;
}

void* geMemalign(int size, int align){
	int block_sz = sizeof(t_ptr)*2;
	t_ptr addr = AllocMemBlock(size+align);
	t_ptr* var = (t_ptr*)(ALIGN(addr+block_sz, align) - block_sz);
	var[0] = addr; //Store the addr in the first int
	var[1] = size; //Store the size on the 2nd int
	memset((void*)(t_ptr)&var[2], 0x0, size);
	if(libge_context){
		libge_context->mem += size;
		libge_context->allocs++;
	}
	return (void*)(t_ptr)&var[2]; //Then return block +2 (addr + size)
}

void* geMalloc(int size){
	gePrintDebug(0x100, "geMalloc(%d)\n", size);
	if(size <= 0){
		return NULL;
	}
	return geMemalign(size, 16);
}

void geFree(void* data){
	gePrintDebug(0x100, "geFree(0x%08lX)\n", (t_ptr)data);
//	printf("geFree(0x%08X)\n", (u32)data);
	if(data != NULL && data != (void*)0xDEADBEEF && data != (void*)0xBAADF00D){
		t_ptr* var = (t_ptr*)data;
		t_ptr addr = var[-2]; // Get the addr
		t_ptr size = var[-1]; // Get the size
//		printf("  addr: 0x%08X\n", (u32)addr);
		free((void*)addr);

		libge_context->mem -= size;
		libge_context->frees++;
	}
}

void* geRealloc(void* last, int size){
//	gePrintDebug(0x100, "geRealloc(0x%08lX, %d)\n", (t_ptr)last, size);
	if(size <= 0){
		geFree(last);
		return NULL;
	}
	if(last == NULL || last == (void*)0xDEADBEEF || last == (void*)0xBAADF00D){
		return geMalloc(size);
	}

	t_ptr last_size = ((t_ptr*)last)[-1];
	
	int block_sz = sizeof(t_ptr)*2;
	t_ptr addr = AllocMemBlock(size+16+16);
	t_ptr* var = (t_ptr*)(ALIGN(addr+16, 16) - block_sz);
	var[0] = addr; //Store the addr in the first int
	var[1] = size; //Store the size on the 2nd int
	void* new_ptr = (void*)&var[2];
	memset(new_ptr, 0x0, size);

	int sz_copy = last_size<size?last_size:size;
	memcpy(new_ptr, last, sz_copy); //Copy the old data

	var = (t_ptr*)last;
	free((void*)var[-2]);

	libge_context->mem -= last_size;
	libge_context->mem += size;
	
	return new_ptr; //Then return the new pointer
}

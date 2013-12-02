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

#include <string.h>
#include "../../ge_internal.h"

#ifndef ALIGN
	#define ALIGN(x, align) (((x)+((align)-1))&~((align)-1))
#endif

static unsigned long AllocMemBlock(int size){
	return (unsigned long)malloc(size);
}

void* geMemalign(int size, int align){
	int block_sz = sizeof(unsigned long)*2;
	unsigned long addr = AllocMemBlock(size+(align>block_sz?align:block_sz)+align);
	unsigned long* var = (unsigned long*)(ALIGN(addr+(align>block_sz?align:block_sz), align) - block_sz);
	var[0] = addr; //Store the addr in the first int
	var[1] = size; //Store the size on the 2nd int
	memset((void*)(unsigned long)&var[2], 0x0, size);
	if(libge_context){
		libge_context->mem += size;
		libge_context->allocs++;
	}
	return (void*)(unsigned long)&var[2]; //Then return block +2 (addr + size)
}

void* geMalloc(int size){
	if(size <= 0){
		return NULL;
	}
	return geMemalign(size, 16);
}

void geFree(void* data){
//	printf("geFree(0x%08X)\n", (u32)data);
	if(data != NULL && data != (void*)0xDEADBEEF && data != (void*)0xBAADF00D){
		unsigned long* var = (unsigned long*)data;
		unsigned long addr = var[-2]; // Get the addr
		unsigned long size = var[-1]; // Get the size
//		printf("  addr: 0x%08X\n", (u32)addr);
		free((void*)addr);

		libge_context->mem -= size;
		libge_context->frees++;
	}
}

void* geRealloc(void* last, int size){
	if(size <= 0){
		geFree(last);
		return NULL;
	}
	if(last == NULL || last == (void*)0xDEADBEEF || last == (void*)0xBAADF00D){
		return geMalloc(size);
	}
	unsigned long last_size = ((unsigned long*)last)[-1];
	
	unsigned long addr = AllocMemBlock(size+8+16);
	unsigned long* var = (unsigned long*)(ALIGN(addr+8, 16) - 8);
	var[0] = addr; //Store the addr in the first int
	var[1] = size; //Store the size on the 2nd int
	void* new_ptr = (void*)&var[2];
	memset((void*)(unsigned long)&var[2], 0x0, size);

	int sz_copy = last_size<size?last_size:size;
	memcpy(new_ptr, last, sz_copy); //Copy the old data

	var = (unsigned long*)last;
	addr = var[-2]; // Get the addr
	free((void*)addr);

	libge_context->mem -= last_size;
	libge_context->mem += size;

	return new_ptr; //Then return the new pointer
}

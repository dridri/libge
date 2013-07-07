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

#ifndef ALIGN
	#define ALIGN(x, align) (((x)+((align)-1))&~((align)-1))
#endif

size_t geMemBlockSize(void* ptr){
	if(ptr != NULL && ptr != (void*)0xDEADBEEF && ptr != (void*)0xBAADF00D){
		return ((t_ptr*)ptr)[-1];
	}
	return 0;
}

static u32 AllocMemBlock(int size, u32* addr){
	u32 id = sceKernelAllocPartitionMemory(2, "alloc", PSP_SMEM_Low, size, NULL);
	if((int)id < 0){
		id = sceKernelAllocPartitionMemory(2, "alloc", PSP_SMEM_High, size, NULL);
	}
	if((int)id < 0){
		gePrintDebug(0x102, "Memory allocation failed ! (sz=%d)\n", size);
	}
	*addr = (u32)sceKernelGetBlockHeadAddr(id);
	return id;
}

void* geMemalign(int size, int align){
	gePrintDebug(0x100, "geMemalign(%d, %d)\n", size, align);
	/*
	u32 addr = 0x0;
	u32 id = AllocMemBlock(size+8+align, &addr);
//	u32* var = (u32*)((addr & (0xFFFFFFFF-(u32)(align-1))) + align - 8);
	u32* var = (u32*)(ALIGN(addr, align) - 8);
	var[0] = id; //Store the ID in the first byte
	var[1] = size; //Store the size on the 2nd byte
	gePrintDebug(0x100, "FreeMem After Memalign: %d KB\n\n", sceKernelTotalFreeMemSize() / 1024);
	memset((void*)(u32)&var[2], 0x0, size);
	libge_context->allocs++;
	libge_context->mem += size + 8;
	return (void*)(u32)&var[2]; //Then return block +2 (id + size)
	*/
	
	int block_sz = sizeof(t_ptr)*2;
	t_ptr addr = 0;
	u32 id = AllocMemBlock(size+align+block_sz, &addr);
	t_ptr* var = (t_ptr*)(ALIGN(addr+block_sz, align) - block_sz);
	var[0] = id; //Store the ID in the first int
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
	/*
	n_alloc++;
	gePrintDebug(0x100, "geMalloc(%d)\n", size);

	u32 addr = 0x0;
	u32 id = AllocMemBlock(size+8, &addr);
	u32* var = (u32*)(ALIGN(addr, 16) - 8);
	var[0] = id; //Store the ID in the first byte
	var[1] = size; //Store the size on the 2nd byte
	gePrintDebug(0x100, "FreeMem After Alloc: %d KB\n\n", sceKernelTotalFreeMemSize() / 1024);
	memset((void*)(u32)&var[2], 0x0, size);
	return (void*)(u32)&var[2]; //Then return block +2 (id + size)
	*/
}

void geFree(void* data){
	gePrintDebug(0x100, "geFree(0x%08lX)\n", (t_ptr)data);
	if(data != NULL && data != (void*)0xDEADBEEF && data != (void*)0xBAADF00D){
		t_ptr* var = (t_ptr*)data;
		u32 id = var[-2]; // Get the ID
		t_ptr size = var[-1]; // Get the size
		sceKernelFreePartitionMemory(id);

		libge_context->mem -= size;
		libge_context->frees++;
	}
}

void* geRealloc(void* last, int size){
	gePrintDebug(0x100, "geRealloc(0x%p, %d)\n", (t_ptr)last, size);
	if(size <= 0){
		geFree(last);
		return NULL;
	}
	if(last == NULL || last == (void*)0xDEADBEEF || last == (void*)0xBAADF00D){
		return geMalloc(size);
	}

	t_ptr last_size = ((t_ptr*)last)[-1];
	
	int block_sz = sizeof(t_ptr)*2;
	t_ptr addr = 0;
	u32 id = AllocMemBlock(size+16+block_sz, &addr);
	t_ptr* var = (t_ptr*)(ALIGN(addr+block_sz, 16) - block_sz);
	var[0] = id; //Store the ID in the first int
	var[1] = size; //Store the size on the 2nd int
	void* new_ptr = (void*)&var[2];
	memset(new_ptr, 0x0, size);

	int sz_copy = last_size < size ? last_size : size;
	memcpy(new_ptr, last, sz_copy); //Copy the old data

	sceKernelFreePartitionMemory((u32)((t_ptr*)last)[-2]);

	libge_context->mem -= last_size;
	libge_context->mem += size;
	
	return new_ptr; //Then return the new pointer



	/*
	gePrintDebug(0x100, "geRealloc(0x%08X, %d)\n", (u32)last, size);
	u32 old_size = ((u32*)last)[-1]; //The size is stored in the 2nd byte

	u32* new_ptr = (u32*)geMalloc(size); //Do a malloc with new size
	memcpy(new_ptr, last, old_size); //Copy the old data
	geFree(last); //free the last block
	gePrintDebug(0x100, "FreeMem After Realloc: %d KB\n\n", sceKernelTotalFreeMemSize() / 1024);
	libge_context->mem -= old_size;
	libge_context->mem += size;
	return new_ptr; //Then return the new pointer
	*/
}

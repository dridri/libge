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

static int n_alloc = 0;
static int n_free = 0;

static u32 AllocMemBlock(int size, u32* addr){
	u32 id = sceKernelAllocMemBlock("GE_alloc", SCE_KERNEL_MEMBLOCK_TYPE_USER_RW, size, NULL);
	sceKernelGetMemBlockBase(id, addr);
	return id;
}

void* geMemalign(int size, int align){
	n_alloc++;
	gePrintDebug(0x100, "geMemalign(%d, %d)\n", size, align);

	u32 addr = 0x0;
	u32 id = AllocMemBlock(size+8+align, &addr);
//	u32* var = (u32*)((addr & (0xFFFFFFFF-(u32)(align-1))) + align - 8);
	u32* var = (u32*)(ALIGN(addr, align) - 8);
	var[0] = id; //Store the ID in the first byte
	var[1] = size; //Store the size on the 2nd byte
	gePrintDebug(0x100, "FreeMem After Memalign: %d KB\n\n", sceKernelTotalFreeMemSize() / 1024);
	memset((void*)(u32)&var[2], 0x0, size);
	return (void*)(u32)&var[2]; //Then return block +2 (id + size)
}

void* geMalloc(int size){
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
}

void geFree(void* data){
	n_free++;
	gePrintDebug(0x100, "geFree(0x%08X)\n", (u32)data);
	
	u32* var = (u32*)data;
	u32 id = var[-2]; // Get the ID
	int ret = sceKernelFreeMemBlock(id); //We can free the data
	gePrintDebug(0x100, "FreeMem After Free: %d KB\n\n", sceKernelTotalFreeMemSize() / 1024);
}

void* geRealloc(void* last, int size){
	gePrintDebug(0x100, "geRealloc(0x%08X, %d)\n", (u32)last, size);
	u32 old_size = ((u32*)last)[-1]; //The size is stored in the 2nd byte

	u32* new_ptr = (u32*)geMalloc(size); //Do a malloc with new size
	memcpy(new_ptr, last, old_size); //Copy the old data
	geFree(last); //free the last block
	gePrintDebug(0x100, "FreeMem After Realloc: %d KB\n\n", sceKernelTotalFreeMemSize() / 1024);
	return new_ptr; //Then return the new pointer
}

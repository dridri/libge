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
#include <psprtc.h>

u32 geGetTick(){
	u64 tick = 0;
	sceRtcGetCurrentTick(&tick);
//	return (u32)tick;
	return (u32)tick / 1000;
}

int geGetTickResolution(){
//	return 1000000;
	return 1000;
}

void geSleep(int msec){
	sceKernelDelayThread(msec*1000);
}

void geUSleep(int usec){
	sceKernelDelayThread(usec);
}

int geGetNumCPU(){
	return 1;
}

u32 geSysFileOpen(const char* filename, int mode){
	int fd_mode = 0;
	
	if(mode & GE_FILE_MODE_READ){
		fd_mode = PSP_O_RDONLY;
	}else
	if(mode & GE_FILE_MODE_WRITE){
		fd_mode = PSP_O_WRONLY | PSP_O_CREAT;
	}else
	if(mode & GE_FILE_MODE_APPEND){
		fd_mode = PSP_O_WRONLY | PSP_O_APPEND | PSP_O_CREAT;
	}else
	if(mode & GE_FILE_MODE_READWRITE){
		fd_mode = PSP_O_RDWR;
	}else
	if(mode & GE_FILE_MODE_CLEAR_RW){
		fd_mode = PSP_O_RDWR | PSP_O_CREAT | PSP_O_TRUNC;
	}else
	if(mode & GE_FILE_MODE_APPEND_RW){
		fd_mode = PSP_O_RDWR | PSP_O_APPEND | PSP_O_CREAT;
	}
	
	return sceIoOpen(filename, fd_mode, 0777);
}

void geSysFileClose(u32 file){
	sceIoClose(file);
}

int geSysFileSeek(u32 file, int offset, int origin){
	sceIoLseek32(file, offset, origin);
}

int geSysFileTell(u32 file){
	sceIoLseek32(file, 0, PSP_SEEK_CUR);
}

char geSysFileGetc(u32 file){
	char c = 0x0;
	sceIoRead(file, &c, sizeof(char));
	return c;
}

char* geSysFileGets(u32 file, char* buffer, int max){
	sceIoRead(file, buffer, max);
	int i = 0;
	for(i=0; (i+1)<max; i++){
		if(buffer[i] == '\n'){
			buffer[i+1] = 0x0;
			break;
		}
	}
	buffer[max-1] = 0x0;
}

int geSysFileRead(u32 file, void* buffer, int size){
	sceIoRead(file, buffer, size);
}

int geSysFileWrite(u32 file, void* buffer, int size){
	sceIoWrite(file, buffer, size);
}

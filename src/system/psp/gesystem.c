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
	return (u32)(tick / (u64)1000);
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

void* geSysFileOpen(const char* filename, int mode){
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
	
	return (void*)sceIoOpen(filename, fd_mode, 0777);
}

void* geSysFileOpenFd(int fd, int mode){
	return NULL;
}

void geSysFileClose(void* file){
	sceIoClose((u32)file);
}

int geSysFileSeek(void* file, int offset, int origin){
//	return sceIoLseek32((u32)file, offset, origin);
	return sceIoLseek((u32)file, offset, origin);
}

int geSysFileTell(void* file){
//	return sceIoLseek32((u32)file, 0, PSP_SEEK_CUR);
	return sceIoLseek((u32)file, 0, PSP_SEEK_CUR);
}

char geSysFileGetc(void* file){
	char c = 0x0;
	sceIoRead((u32)file, &c, sizeof(char));
	return c;
}

char* geSysFileGets(void* file, char* buffer, int max){
	/*
	sceIoRead((u32)file, buffer, max);
	int i = 0;
	for(i=0; (i+1)<max; i++){
		if(buffer[i] == '\n'){
			buffer[i+1] = 0x0;
			break;
		}
	}
	buffer[max-1] = 0x0;
	*/
	int ret = sceIoRead((u32)file, buffer, max);
	buffer[max-1] = 0x0;
	int i;
	for(i=0; (i+1)<ret; i++){
		if(buffer[i] == '\n' || (buffer[i]==0x0d && buffer[i+1]==0x0a)){
			if((i+2)<strlen(buffer)){
				buffer[i+1] = 0x0;
			}else{
				buffer[i+2] = 0x0;
			}
			if(buffer[i]==0x0d){
				buffer[i] = '\n';
				sceIoLseek32((u32)file, -(ret-strlen(buffer))+1, SEEK_CUR);
			}else{
				sceIoLseek32((u32)file, -(ret-strlen(buffer)), SEEK_CUR);
			}
			break;
		}
	}
	if(ret <= 0){
		return NULL;
	}
	return buffer;
}

int geSysFileRead(void* file, void* buffer, int size){
	return sceIoRead((u32)file, buffer, size);
}

int geSysFileWrite(void* file, void* buffer, int size){
	return sceIoWrite((u32)file, buffer, size);
}

void* geSysDopen(const char* file){
	return (void*)sceIoDopen(file);
}

void geSysDclose(void* fd){
	sceIoDclose((SceUID)fd);
}

int geSysDread(void* fd, char* path, ge_Dirent* entry){
	if(!fd || !entry){
		return -1;
	}
	SceIoStat st;
	SceIoDirent e;
	if(sceIoDread((SceUID)fd, &e) <= 0){
		return -2;
	}

	strncpy(entry->name, e.d_name, 256);
	
	char fullpath[2048+256] = "";
	sprintf(fullpath, "%s/%s", path, e.d_name);
	sceIoGetstat(fullpath, &st);
	entry->stat.isDir = (st.st_mode & FIO_SO_IFDIR) ? 1 : 0;
	entry->stat.size = st.st_size;
	memcpy(&entry->stat.ctime, &st.st_ctime, sizeof(ge_Date));
	memcpy(&entry->stat.atime, &st.st_atime, sizeof(ge_Date));
	memcpy(&entry->stat.mtime, &st.st_mtime, sizeof(ge_Date));

	return 1;
}

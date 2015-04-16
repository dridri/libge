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
#include <dirent.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <time.h>


void LinuxKeyboardInit();
void LinuxKeyboardFinished();
void LinuxKeyboardDescrition(const char* desc);
void LinuxKeyboardOutput(char* ptr, int max_len);
void LinuxKeyboardColor(u32 color);
int LinuxKeyboardUpdate();
int LinuxKeyboardIndex();

void LinuxInit(){
	geKeyboardDefine(LinuxKeyboardInit, LinuxKeyboardFinished, LinuxKeyboardDescrition, LinuxKeyboardOutput, LinuxKeyboardColor, LinuxKeyboardUpdate, LinuxKeyboardIndex);
}

void* geglImportFunction(const char* func_name){
	void* func = NULL;
	//glXGetProcAddress
	func = glXGetProcAddressARB((GLubyte*)func_name);
	return func;
}

int LinuxSwapBuffers();
int SystemSwapBuffers(){
	return LinuxSwapBuffers();
}

void geDebugOut(char* buff, int bufsz){
	if(bufsz <= 0){
		return;
	}
//	buff[strlen(buff)-1] = 0x0;
//	puts(buff);
	printf("%s", buff);
}

u32 geGetTick(){
/*
	struct timeval cTime;
	gettimeofday(&cTime, 0);
	return (cTime.tv_sec * 1000) + (cTime.tv_usec / 1000);
*/
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	return now.tv_sec*1000 + now.tv_nsec/1000000;
}

float geGetTickFloat(){
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
// 	clock_gettime(CLOCK_REALTIME_HR, &now);
	float ret = (float)now.tv_sec;
	u32 ms = now.tv_nsec/1000000;
	ret += ((float)ms) / 1000.0;
	return ret;
}

int geGetTickResolution(){
	return 1000;
}

void geSleep(int msec){
	usleep(msec * 1000);
}

void geUSleep(int usec){
	usleep(usec);
}

int geGetNumCPU(){
	return sysconf( _SC_NPROCESSORS_ONLN );
}

void* geSysFileOpen(const char* filename, int mode){
	char fp_mode[8] = "";

	if(mode & GE_FILE_MODE_BINARY){
		fp_mode[1] = 'b';
	}

	if(mode & GE_FILE_MODE_READ){
		fp_mode[0] = 'r';
	}else
	if(mode & GE_FILE_MODE_WRITE){
		fp_mode[0] = 'w';
	}else
	if(mode & GE_FILE_MODE_APPEND){
		fp_mode[0] = 'a';
	}else
	if(mode & GE_FILE_MODE_READWRITE){
		fp_mode[0] = 'r';
	}else
	if(mode & GE_FILE_MODE_CLEAR_RW){
		fp_mode[0] = 'w';
	}else
	if(mode & GE_FILE_MODE_APPEND_RW){
		fp_mode[0] = 'a';
	}
	if((mode&GE_FILE_MODE_READWRITE) || (mode&GE_FILE_MODE_CLEAR_RW) || (mode&GE_FILE_MODE_APPEND_RW)){
		if(fp_mode[1] != 0x0){
			fp_mode[2] = '+';
		}else{
			fp_mode[1] = '+';
		}
	}

	char* file2 = strdup(filename);
	int i;
	for(i=0; file2[i]; i++){
		if(file2[i] == '\\'){
			file2[i] = '/';
		}
	}
	void* ret = (void*)fopen(file2, fp_mode);
	free(file2);
	return ret;
}

void* geSysFileOpenFd(int fd, int mode){
	return NULL;
}

void geSysFileClose(void* file){
	fclose((FILE*)file);
}

int geSysFileSeek(void* file, int offset, int origin){
	fseek((FILE*)file, offset, origin);
	return ftell((FILE*)file);
}

int geSysFileTell(void* file){
	return ftell((FILE*)file);
}

char geSysFileGetc(void* file){
	return fgetc((FILE*)file);
}

char* geSysFileGets(void* file, char* buffer, int max){
//	return fgets(buffer, max, (FILE*)file);
	int ret = fread(buffer, 1, max, (FILE*)file);
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
				fseek((FILE*)file, -(ret-strlen(buffer))+1, SEEK_CUR);
			}else{
				fseek((FILE*)file, -(ret-strlen(buffer)), SEEK_CUR);
			}
			break;
		}
	}
//	printf("geSysFileGets 3 (\"%s\")\n", buffer);
	if(ret <= 0){
		return NULL;
	}
	return buffer;
}

int geSysFileRead(void* file, void* buffer, int size){
	return fread(buffer, 1, size, (FILE*)file);
}

int geSysFileWrite(void* file, void* buffer, int size){
	return fwrite(buffer, 1, size, (FILE*)file);
}


void* geSysDopen(const char* file){
	return opendir(file);
}

void geSysDclose(void* fd){
	closedir((DIR*)fd);
}

int geSysDread(void* fd, char* path, ge_Dirent* entry){
	if(!fd || !entry){
		return -1;
	}
	struct dirent* e = readdir((DIR*)fd);
	struct stat st;
	if(!e){
		return -2;
	}

	strncpy(entry->name, e->d_name, 256);
	
	char fullpath[2048+256] = "";
	sprintf(fullpath, "%s/%s", path, e->d_name);
	stat(fullpath, &st);
	entry->stat.isDir = (st.st_mode & S_IFDIR)? 1 : 0;
	entry->stat.size = st.st_size;
	memcpy(&entry->stat.ctime, &st.st_ctime, sizeof(ge_Date));
	memcpy(&entry->stat.atime, &st.st_atime, sizeof(ge_Date));
	memcpy(&entry->stat.mtime, &st.st_mtime, sizeof(ge_Date));

	return 1;
}

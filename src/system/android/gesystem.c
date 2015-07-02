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
#include <sys/stat.h>
#include <sys/time.h>
#include <android/log.h>
#include <android/asset_manager.h>
int AndroidSwapBuffers();

#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "libge", __VA_ARGS__))

u32 tick_pause = 0;


void* geglImportFunction(const char* func_name){
	void* func = NULL;
	return func;
}

int SystemSwapBuffers(){
	return AndroidSwapBuffers();
}

void geDebugOut(char* buff, int bufsz){
	if(bufsz <= 0){
		return;
	}
	buff[strlen(buff)-1] = 0x0;
	LOGW("%s", buff);
}

u32 geGetTick(){
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
//	return (now.tv_sec*1000000000LL + now.tv_nsec) / 1000;
	return now.tv_sec*1000 + now.tv_nsec/1000000 - tick_pause;
}

float geGetTickFloat(){
	return (double)geGetTick() / 1000.0;
/*
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
// 	clock_gettime(CLOCK_REALTIME_HR, &now);
	float ret = (float)now.tv_sec;
	u32 ms = now.tv_nsec/1000000 - tick_pause;
	ret += ((float)ms) / 1000.0;
	return ret;
*/
}

int geGetTickResolution(){
	return 1000;
}

void geSleep(int msec){
//	sleep(msec);
	usleep(msec*1000);
}

void geUSleep(int usec){
	usleep(usec);
}

int geGetNumCPU(){
	return sysconf( _SC_NPROCESSORS_ONLN );
}

void geInitSocket(){
}

void* geLibcFileOpenFd(int fd, int mode){
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
	
	void* ret = (void*)fdopen(fd, fp_mode);
	return ret;
}

void* geLibcFileOpen(const char* filename, int mode){
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
	
	void* ret = (void*)fopen(filename, fp_mode);
	return ret;
}

void geLibcFileClose(void* file){
	fclose((FILE*)file);
}

int geLibcFileSeek(void* file, int offset, int origin){
	fseek((FILE*)file, offset, origin);
	return ftell((FILE*)file);
}

int geLibcFileTell(void* file){
	return ftell((FILE*)file);
}

char geLibcFileGetc(void* file){
	return fgetc((FILE*)file);
}

int geLibcFileRead(void* file, void* buffer, int size){
	return fread(buffer, 1, size, (FILE*)file);
}

int geLibcFileWrite(void* file, void* buffer, int size){
	return fwrite(buffer, 1, size, (FILE*)file);
}

typedef struct _ge_android_fd {
	int isAsset;
	void* fd;
} _ge_android_fd;

void* geSysFileOpenFd(int fd, int mode){
	void* ret = geLibcFileOpenFd(fd, mode);
	if(ret){
		_ge_android_fd* fd = (_ge_android_fd*)geMalloc(sizeof(_ge_android_fd));
		fd->isAsset = false;
		fd->fd = ret;
		ret = (void*)fd;
	}
	return ret;
}

char* convert_path(const char* path){
	char* buf0 = (char*)geMalloc(strlen(path) + 1);
	char* buf = (char*)geMalloc(strlen(path) + 1);
	int i, j;
	
	strcpy(buf0, path);
	gePrintDebug(0x100, "convert : '%s' =>\n", buf0);
	for(i=0; buf0[i]!='\0'; i++){
		if(buf0[i] == '\\'){
			buf0[i] = '/';
		}
		while(buf0[i] == '/' && buf0[i+1] == '/'){
			for(j=i; buf0[j]!='\0'; j++){
				buf0[j] = buf0[j+1];
			}
			buf0[j] = 0x0;
		}
	}
	gePrintDebug(0x100, "          '%s'\n", buf0);

	for(i=0, j=0; i<strlen(buf0); i++, j++){
		if(!strncmp(&buf0[i], "/../", 4) && i > 0){
			while(j > 0 && buf[j--] != '/');
			if(!j)j--;
			buf[j+1] = 0x0;
			i += 2;
			gePrintDebug(0x100, "  [%d] %s\n", i, buf0);
			gePrintDebug(0x100, "   [%d] %s\n", j, buf);
		}else{
			buf[j] = buf0[i];
		}
	}

	while(buf[0] == '/'){
		for(j=0; buf[j]!='\0'; j++){
			buf[j] = buf[j+1];
		}
		buf[j] = 0x0;
	}
	geFree(buf0);
	return buf;
}

void* geSysFileOpen(const char* filename, int mode){
	LibGE_AndroidContext* context = (LibGE_AndroidContext*)libge_context->syscontext;
	if(!context){
		context = _ge_GetAndroidContext();
	}
	bool isAsset = false;
	void* ret = geLibcFileOpen(filename, mode);
	if(!ret){
		ret = geLibcFileOpen(_ge_BuildPath(context->state->activity->internalDataPath, filename), mode);
	}
	if(!ret){
		char* path = convert_path(filename);
// 		gePrintDebug(0x100, "Android converted path : '%s'\n", path);
		ret = AAssetManager_open(context->state->activity->assetManager, path, AASSET_MODE_STREAMING);
		geFree(path);
		isAsset = true;
	}
	if(ret){
		_ge_android_fd* fd = (_ge_android_fd*)geMalloc(sizeof(_ge_android_fd));
		fd->isAsset = isAsset;
		fd->fd = ret;
		ret = (void*)fd;
	}
	return ret;
}

void geSysFileClose(void* file){
	if(!file){
		return;
	}
	_ge_android_fd* fd = (_ge_android_fd*)file;
	if(fd->isAsset){
		AAsset_close((AAsset*)fd->fd);
	}else{
		geLibcFileClose(fd->fd);
	}
}

int geSysFileSeek(void* file, int offset, int origin){
	if(!file){
		return -1;
	}
	int ret = 0;
	_ge_android_fd* fd = (_ge_android_fd*)file;
	if(fd->isAsset){
		ret = AAsset_seek((AAsset*)fd->fd, offset, origin);
	}else{
		geLibcFileSeek(fd->fd, offset, origin);
		ret = geLibcFileTell(fd->fd);
	}
	return ret;
}

int geSysFileTell(void* file){
	if(!file){
		return 0;
	}
	_ge_android_fd* fd = (_ge_android_fd*)file;
	if(fd->isAsset){
		return AAsset_seek((AAsset*)fd->fd, 0, SEEK_CUR);
	}
	return geLibcFileTell(fd->fd);
}

char geSysFileGetc(void* file){
	if(!file){
		return 0;
	}
	_ge_android_fd* fd = (_ge_android_fd*)file;
	if(fd->isAsset){
		char c;
		AAsset_read((AAsset*)fd->fd, &c, 1);
		return c;
	}
	return geLibcFileGetc(fd->fd);
}

int geSysFileRead(void* file, void* buffer, int size){
	if(!file){
		return 0;
	}
	_ge_android_fd* fd = (_ge_android_fd*)file;
	if(fd->isAsset){
		return AAsset_read((AAsset*)fd->fd, buffer, size);
	}
	return geLibcFileRead(fd->fd, buffer, size);
}

int geSysFileWrite(void* file, void* buffer, int size){
	if(!file){
		return 0;
	}
	_ge_android_fd* fd = (_ge_android_fd*)file;
	if(fd->isAsset){
		return 0;
	}
	return geLibcFileWrite(fd->fd, buffer, size);
}

char* geSysFileGets(void* file, char* buffer, int max){
	if(!file){
		return 0;
	}
//	return fgets(buffer, max, (FILE*)file);
	int ret = geSysFileRead(file, buffer, max);
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
				geSysFileSeek(file, -(ret-strlen(buffer))+1, SEEK_CUR);
			}else{
				geSysFileSeek(file, -(ret-strlen(buffer)), SEEK_CUR);
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

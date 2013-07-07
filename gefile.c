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

#include "ge_internal.h"

static int nOpen = 0;
static int nClose = 0;
bool isPartFile(const char* file, char* orig, char* name);

int geFileFullRead(const char* filename, void** buf){
	ge_File* fp = geFileOpen(filename, GE_FILE_MODE_READ | GE_FILE_MODE_BINARY);
	if(!fp)return 0;

	geFileSeek(fp, 0, GE_FILE_SEEK_END);
	int size = geFileTell(fp);
	geFileRewind(fp);

	*buf = geMalloc(size+1);
	geFileRead(fp, *buf, size);

	geFileClose(fp);
	return size;
}

char* geFileFromBuffer(void* ptr, int size){
	char* out = (char*)geMalloc(64);
	sprintf(out, "_ge_file_buffer_0x%p_%d", ptr, size);
	return out;
}

char* gePathFromFile(ge_File* file){
	char* out = (char*)geMalloc(64);
	sprintf(out, "_ge_file_descriptor_0x%p", file);
	return out;
}

bool geFileExists(const char* filename){
	bool last_critical = geDebugCritical(false);
	bool ret = false;
	ge_File* fp = geFileOpen(filename, GE_FILE_MODE_READ | GE_FILE_MODE_BINARY);
	if(fp){
		geFileClose(fp);
		ret = true;
	}
	geDebugCritical(last_critical);
	return ret;
}

ge_File* geFileOpenFd(int fd, int mode){
	if(fd <= 0){
		return NULL;
	}

	gePrintDebug(0x100, "Opening fd 0x%08X\n", fd);
	
	ge_File* file = (ge_File*)geMalloc(sizeof(ge_File));
	memset(file, 0, sizeof(ge_File));

	file->mode = mode;
	file->type = GE_FILE_TYPE_NORMAL;
	file->fd = geSysFileOpenFd(fd, mode);

	ge_file_desc* desc = (ge_file_desc*)geMalloc(sizeof(ge_file_desc));
	desc->offset = 0;
	desc->fake_offset = 0;
	file->desc = desc;

	if(file->fd <= 0){
		gePrintDebug(0x102, "System file open error 0x%08X on fd 0x%08X\n", file->fd, fd);
		geFree((void*)file->desc);
		geFree(file);
		return NULL;
	}
	
	nOpen++;
	return file;
}

ge_File* geFileOpen(const char* filename, int mode){
	if(!filename || filename[0]==0x0){
		return NULL;
	}

	gePrintDebug(0x100, "Opening file \"%s\"\n", filename);
	if(strstr(filename, "_ge_file_buffer_")){
		void* ptr = NULL;
		int size = 0;
		sscanf(filename, "_ge_file_buffer_0x%p_%d", &ptr, &size);
		return geFileBuffer((const void*)ptr, size, mode);
	}
	if(strstr(filename, "_ge_file_descriptor_")){
		void* ptr = NULL;
		sscanf(filename, "_ge_file_descriptor_0x%p", &ptr);
		return (ge_File*)ptr;
	}
	
	char part_file[2048] = "";
	char part_pfile[2048] = "";
	if(isPartFile(filename, part_file, part_pfile)){
		return geFilePart(part_file, part_pfile);
	}
	
	ge_File* file = (ge_File*)geMalloc(sizeof(ge_File));
	memset(file, 0, sizeof(ge_File));

	strncpy(file->filename, filename, 2048);
	file->mode = mode;
	file->type = GE_FILE_TYPE_NORMAL;
	file->fd = geSysFileOpen(filename, mode);

	ge_file_desc* desc = (ge_file_desc*)geMalloc(sizeof(ge_file_desc));
	desc->offset = 0;
	desc->fake_offset = 0;
	file->desc = desc;

	if((int)file->fd <= 0){
		gePrintDebug(0x102, "System file open error 0x%08X on \"%s\"\n", file->fd, filename);
		geFree((void*)file->desc);
		geFree(file);
		return NULL;
	}
	
	nOpen++;
	return file;
}

ge_File* geFileBuffer(const void* buf, int size, int mode){
	ge_File* file = (ge_File*)geMalloc(sizeof(ge_File));
	memset(file, 0, sizeof(ge_File));

	sprintf(file->filename, "file buffer");
	file->mode = mode;
	file->type = GE_FILE_TYPE_BUFFER;
	file->fd = 0;

	if(buf){
		file->buffer = geMemalign(size, 16);
		memcpy(file->buffer, buf, size);
	}

	ge_file_desc* desc = (ge_file_desc*)geMalloc(sizeof(ge_file_desc));
	desc->offset = 0;
	desc->size = size;
	file->desc = desc;
	desc->fake_offset = 0;

	return file;
}

typedef struct ge_package_file {
	int size;
	u32 data_addr;
	int name_size;
	u32 name_addr;
} ge_package_file;

ge_File* geFilePart(const char* filename, const char* name){
	ge_File* fd = geFileOpen(filename, GE_FILE_MODE_READ | GE_FILE_MODE_BINARY);
	if(!fd)goto error_0;
	ge_data_file pak;
	geFileRead(fd, &pak, sizeof(ge_data_file));

	ge_File* file = NULL;

	gePrintDebug(0x100, "m_magic: 0x%2.2X\nmagic: \"%s\"\nversion: %d.%d\n\n", pak.mini_magic, pak.magic, pak.version_maj, pak.version_min);
	if(pak.mini_magic==0x0e && !strcmp(pak.magic, "LIB_GAME_ENGINE_PACKAGE") && pak.version_maj==0x00 && (pak.version_min==0x01||pak.version_min==0x02)){
		int num_files = pak.num_files;
//		pspDebugScreenPrintf("num_files: %d\n", num_files);
		if(num_files<=0)goto error_0;
		gePrintDebug(0x100, "1\n");
		ge_package_file* files = (ge_package_file*)geMalloc(sizeof(ge_package_file)*num_files);
		gePrintDebug(0x100, "2\n");
		geFileRead(fd, files, sizeof(ge_package_file)*num_files);
		gePrintDebug(0x100, "3\n");

		int i = 0;
		for(i=0; i<num_files; i++){
			char nm[2048] = "";
			geFileRead(fd, nm, files[i].name_size);
//			pspDebugScreenPrintf("nm[%d]: \"%s\"\n", i, nm);
			if(!strcmp(nm,name))break;
		}
		gePrintDebug(0x100, "4\n");
		if(i==num_files){
			gePrintDebug(0x102, "geFilePart: File \"%s\" not found in package \"%s\"\n", name, filename);
			goto error_1;
		}
		gePrintDebug(0x100, "5\n");

		file = (ge_File*)geMalloc(sizeof(ge_File));
		memset(file, 0, sizeof(ge_File));
		file->fd = fd;
		file->mode = GE_FILE_MODE_READ | GE_FILE_MODE_BINARY | (pak.encrypted?GE_FILE_MODE_ENCRYTPTED:0);
		file->type = GE_FILE_TYPE_PART;
		gePrintDebug(0x100, "6\n");

		file->desc = geMalloc(sizeof(ge_file_desc));
		((ge_file_desc*)file->desc)->start = files[i].data_addr;
		((ge_file_desc*)file->desc)->offset = 0;
		((ge_file_desc*)file->desc)->size = files[i].size;
		((ge_file_desc*)file->desc)->fake_offset = 0;
		gePrintDebug(0x100, "7\n");
		
		if(pak.encrypted){
			u8 PKG_key[16] = { 0xE2, 0x1D, 0xAA, 0xA2, 0xEE, 0xDC, 0x3A, 0x55, 0xED, 0xE7, 0x10, 0x69, 0x0A, 0xC4, 0x86, 0x22 };
			u8 ptr0[64];
			ge_Buffer key;
			key.ptr = ptr0;
			key.size = pak.key_len;
			geFileSeek(fd, -key.size, GE_FILE_SEEK_END);
			geFileRead(fd, key.ptr, key.size);
			geBufferDecrypt(&key, PKG_key, 16);
			geFileSetKey(file, key.ptr, key.size);
			gePrintDebug(0x100, "\nPart file \"%s\" has key :\n  { ", filename);
			int i = 0;
			for(i=0; i<key.size; i++){
				gePrintDebug(0x100, "%02X ", ((u8*)key.ptr)[i]);
			}
			gePrintDebug(0x100, "}\n\n");
		}
		gePrintDebug(0x100, "8\n");

		geFileSeek(fd, files[i].data_addr, GE_FILE_SEEK_SET);
	}

	return file;

error_0:
	geFileClose(fd);
	return NULL;

error_1:
	geFileClose(fd);
	return NULL;
}

void geFileOffset(ge_File* file, int offset){
	((ge_file_desc*)file->desc)->fake_offset = offset;
}

void geFileSetKey(ge_File* file, u8* key, int len){
	if(file->type&GE_FILE_TYPE_BUFFER)return;
	file->buffer = (u8*)geMalloc(len+4);
	((int*)file->buffer)[0] = len;
	memcpy(&file->buffer[4], key, len);
}

void geFileClose(ge_File* file){
	if(file->type == GE_FILE_TYPE_BUFFER){
		if(file->buffer)geFree(file->buffer);
	}else if(file->type == GE_FILE_TYPE_PART){
		geFileClose((ge_File*)file->fd);
	}else{
		geSysFileClose(file->fd);
		nClose++;
	}
	if(file->desc)geFree((ge_file_desc*)file->desc);
	geFree(file);
}

void geFileRewind(ge_File* file){
	if(file->type == GE_FILE_TYPE_BUFFER){
		((ge_file_desc*)file->desc)->offset = 0;
	}else if(file->type == GE_FILE_TYPE_PART){
		geFileSeek((ge_File*)file->fd, ((ge_file_desc*)file->desc)->start, GE_FILE_SEEK_SET);
		((ge_file_desc*)file->desc)->offset = 0;
	}else{
		geSysFileSeek(file->fd, ((ge_file_desc*)file->desc)->fake_offset, GE_FILE_SEEK_SET);
	}
}

void geFileSeek(ge_File* file, int offset, int origin){
	ge_file_desc* desc = (ge_file_desc*)file->desc;
	if(file->type == GE_FILE_TYPE_BUFFER){
		if(origin == GE_FILE_SEEK_SET){
			desc->offset = offset;
		}else
		if(origin == GE_FILE_SEEK_CUR){
			desc->offset += offset;
		}else
		if(origin == GE_FILE_SEEK_END){
			desc->offset = desc->size + offset;
		}
	}else if(file->type == GE_FILE_TYPE_PART){
		if(origin == GE_FILE_SEEK_SET){
			desc->offset = offset;
		}else
		if(origin == GE_FILE_SEEK_CUR){
			desc->offset += offset;
		}else
		if(origin == GE_FILE_SEEK_END){
			desc->offset = desc->size + offset;
		}
	}else{
		desc->offset = geSysFileSeek(file->fd, offset+desc->fake_offset, origin);
	}
}

int geFileTell(ge_File* file){
	if(file->type == GE_FILE_TYPE_BUFFER){
		ge_file_desc* desc = (ge_file_desc*)file->desc;
		return desc->offset;
	}else
	if(file->type == GE_FILE_TYPE_PART){
		ge_file_desc* desc = (ge_file_desc*)file->desc;
		return desc->offset;
	}
	return geSysFileTell(file->fd);
}

char geFileGetChar(ge_File* file){
	char chr;
	geFileRead(file, &chr, 1);
	return chr;
}

int geFileGets(ge_File* file, char* buffer, int max){
	int i = 0;

	if(file->type == GE_FILE_TYPE_BUFFER){
		if(((ge_file_desc*)file->desc)->offset+max > ((ge_file_desc*)file->desc)->size)max=((ge_file_desc*)file->desc)->size-((ge_file_desc*)file->desc)->offset;
		strncpy(buffer, (char*)&file->buffer[((ge_file_desc*)file->desc)->offset], max);
		((ge_file_desc*)file->desc)->offset += max;
		return max;
	}else{
		/*
		int b_max = max;
		int lastpos = geFileTell(file);
		if(( max=geFileRead(file, buffer, sizeof(char)*max) )<=0){
			memset(buffer, 0, b_max);
			return 0;
		}
		max--;
		for(i=0; i<max; i++){
			if(buffer[i]=='\n' || (buffer[i]==0x0d && buffer[i]==0x0a)){
				buffer[i] = '\n';
				buffer[i+1] = 0x00;
				break;
			}
			if(buffer[i]==0x00)break;
		}
		if(i == 0){
			geFileSeek(file, 0, GE_FILE_SEEK_END);
			return 0;
		}
		geFileSeek(file, lastpos+strlen(buffer), GE_FILE_SEEK_SET);
		*/
		char* ret = geSysFileGets(file->fd, buffer, max);
		if(ret == NULL){
			return 0;
		}
		i = strlen(buffer);
	}

	return i;
}

int geFileRead(ge_File* file, void* buffer, int size){
	ge_file_desc* desc = (ge_file_desc*)file->desc;
	int bytes = 0;
	if(file->type == GE_FILE_TYPE_BUFFER){
//		gePrintDebug(0x100, "Reading %d bytes in buffer(=%d) at offset %d\n", size, desc->size, desc->offset);
		if(desc->offset+size > desc->size){
			size -= (desc->offset+size) - desc->size;
		}
		if(file->mode & GE_FILE_MODE_VIRTUAL){
			*((void**)buffer) = &file->buffer[desc->offset+desc->fake_offset];
		}else{
			memcpy(buffer, &file->buffer[desc->offset+desc->fake_offset], size);
		}
		desc->offset += size;
		bytes = size;
	}else
	if(file->type == GE_FILE_TYPE_PART){
		ge_file_desc* desc = (ge_file_desc*)file->desc;
		geFileSeek((ge_File*)file->fd, desc->start+desc->offset+desc->fake_offset, GE_FILE_SEEK_SET);
		bytes = geFileRead((ge_File*)file->fd, buffer, size);
	//	geSysFileSeek(file->fd, desc->start+desc->offset, size);
	//	bytes = geSysFileRead(file->fd, buffer, size);
		desc->offset += bytes;
	}else{
		bytes = geSysFileRead(file->fd, buffer, size);
	}
	if( (!(file->type & GE_FILE_TYPE_BUFFER)) && (file->mode & GE_FILE_MODE_ENCRYTPTED)){
		int key_index = (desc->offset+desc->fake_offset-bytes) % ((int*)file->buffer)[0];
		ge_Buffer buf = { buffer, size };
		geBufferDecryptIndex(&buf, &file->buffer[4], ((int*)file->buffer)[0], key_index);
	}
	return bytes;
}

int geFileWrite(ge_File* file, void* buffer, int size){
	ge_file_desc* desc = (ge_file_desc*)file->desc;
	if( (!(file->type & GE_FILE_TYPE_BUFFER)) && (file->mode & GE_FILE_MODE_ENCRYTPTED)){
		int key_index = (desc->offset+desc->fake_offset) % ((int*)file->buffer)[0];
		ge_Buffer buf = { buffer, size };
		geBufferEncryptIndex(&buf, &file->buffer[4], ((int*)file->buffer)[0], key_index);
	}

	int bytes = 0;
	if(file->type == GE_FILE_TYPE_BUFFER){
		if(!file->buffer){
			file->buffer = (u8*)geMalloc(((ge_file_desc*)file->desc)->size);
		}
		if(desc->offset+size > desc->size){
			size -= (desc->offset+size) - desc->size;
		}
		memcpy(&file->buffer[desc->offset+desc->fake_offset], buffer, size);
		desc->offset += size;
		bytes = size;

	}else
	if(file->type == GE_FILE_TYPE_PART){
		ge_file_desc* desc = (ge_file_desc*)file->desc;
		geFileSeek((ge_File*)file->fd, desc->start+desc->offset+desc->fake_offset, GE_FILE_SEEK_SET);
		bytes = geFileWrite((ge_File*)file->fd, buffer, size);
		desc->offset += bytes;
	}else{
		bytes = geSysFileWrite(file->fd, buffer, size);
	}
	return bytes;
}

ge_Dir* geDirOpen(const char* dirname){
	ge_Dir* dir = (ge_Dir*)geMalloc(sizeof(ge_Dir));
	dir->fd = geSysDopen(dirname);
	strncpy(dir->dirname, dirname, 2048);
	return dir;
}

int geDirReadNext(ge_Dir* dir, ge_Dirent* entry){
	int ret = geSysDread(dir->fd, dir->dirname, entry);
	if(ret <= 0){
		return 0; // EOF | error
	}
	return ret;
}

char** geDirList(ge_Dir* dir, int* count){
	return NULL;
}

void geDirClose(ge_Dir* dir){
	geSysDclose(dir->fd);
	geFree(dir);
}

void geDirRewind(ge_Dir* dir){
	geSysDclose(dir->fd);
	dir->fd = geSysDopen(dir->dirname);
}

bool isPartFile(const char* file, char* orig, char* name){
	if(strchr(file, '/')){
		int i=0, j=0, k=0;
		int len = strlen(file);
		char com[16][256] = { "" };
		for(i=0,j=0,k=0; i<len; i++){
			if(file[i] == '/'){
				j++;
			}
			int l = 0;
			for(l=j; l<16; l++){
				com[l][k] = file[i];
			}
			k++;
		}
		k = 0;
		for(i=0; i<16; i++){
			if(i > 0 && !strcmp(com[i], com[i - 1])){
				break;
			}
			k += strlen(com[i]) + 1;
			void* test = NULL;
			test = geSysFileOpen(com[i], GE_FILE_MODE_READ | GE_FILE_MODE_BINARY);
			if(test){
				ge_data_file pak;
				geSysFileRead(test, &pak, sizeof(ge_data_file));
				if(com[i][0]!=0x0 && pak.mini_magic==0x0e && !strcmp(pak.magic, "LIB_GAME_ENGINE_PACKAGE")){
					strcpy(orig, com[i]);
				//	strcpy(name, strchr(com[i+1],'/')+1);
					strcpy(name, &file[k]);
					geSysFileClose(test);
					return true;
				}
				geSysFileClose(test);
			}
		}
	}
	return false;
}

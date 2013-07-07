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

#include "../ge_internal.h"

int LoadWav(ge_Sound* sound);
int MP3_FullDecode(void* ptr, int size, void* cb, void* cbud);

static void ffread(void* a, int b, int c, FILE* d){
	int ret = fread(a, b, c, d);
	(void)ret;
}

ge_Sound* geLoadSound(const char* file){
	ge_Sound* sound = (ge_Sound*)malloc(sizeof(ge_Sound));
	if(!sound)return NULL;

	FILE* fp = fopen(file, "rb");
	if(!fp){ free(sound); return NULL; }
	//Get size and buffering
	fseek(fp, 0, SEEK_END);
	sound->size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	sound->ptr = (u8*)malloc(sound->size);
	if(!sound->ptr){ free(sound); fclose(fp); return NULL; }
	ffread(sound->ptr, 1, sound->size, fp);

	//Get type and load
	int ret = 0;
	u8 buffer[32] = { 0 };

	fseek(fp, 0, SEEK_SET);
	ffread(buffer, 1, 32, fp);
	fclose(fp);

	if(buffer[0]=='R' && buffer[1]=='I' && buffer[2]=='F' && buffer[3]=='F' && buffer[8]=='W' && buffer[9]=='A' && buffer[10]=='V' && buffer[11]=='E'){
		sound->type = GE_AUDIO_FILE_WAV;
		ret = LoadWav(sound);
	}
	if(ret < 0){ free(sound); return NULL; }

	sound->handle = 0x0;
	sound->handle = geAudioReserveOutput(sound->sample_rate, sound->bps, sound->nChannels, sound->speakers);

	return sound;
}

void geSoundSetSpeakers(ge_Sound* sound, int speakers){
	geAudioCloseOutput(sound->handle);
	sound->speakers = speakers;
	sound->handle = geAudioReserveOutput(sound->sample_rate, sound->bps, sound->nChannels, sound->speakers);
}

void geSoundPlay(ge_Sound* sound){
	int pos = geAudioGetPlayerPosition(sound->handle);
	if(pos>0 && pos<sound->size-44)return;

	u8* buf = &sound->ptr[44];
	geAudioOutputBlocking(sound->handle, buf, sound->size-44);
}

int geAudioDecode(char* file, void* callback, void* cbdata){
	char type[8] = "";
	int i=0, j=0;
	for(i=strlen(file)-1; i>=0; i--){
		if(file[i]=='.')break;
	}
	for(i=i+1, j=0; i<strlen(file); i++, j++){
		type[j] = file[i];
	}

	ge_File* fp = geFileOpen(file, GE_FILE_MODE_READ | GE_FILE_MODE_BINARY);

	//Get size and buffering
	geFileSeek(fp, 0, GE_FILE_SEEK_END);
	int size = geFileTell(fp);
	void* ptr = (void*)geMalloc(size);
	if(!ptr){
		geFileClose(fp);
		return GE_ERROR_OUTOFMEMORY;
	}
	geFileRewind(fp);
	geFileRead(fp, ptr, size);

	//Get type and load
	u8 buffer[32] = { 0 };
	geFileSeek(fp, 0, GE_FILE_SEEK_SET);
	geFileRead(fp, buffer, 32);
	printf("buffer: \"%s\"\ntype: \"%s\"\n", buffer, type);
	if((buffer[0]=='I' && buffer[1]=='D' && buffer[2]=='3') || strstr(type, "mp3") || strstr(type, "MP3")){
		MP3_FullDecode(ptr, size, callback, cbdata);
	}

	geFree(ptr);
	geFileClose(fp);
	return GE_ERROR_NOERROR;
}

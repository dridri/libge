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

void MP3_Init(ge_Music* mp3);
int MP3_Thread(int args, void *argp);
short* MP3_GetBuffer(ge_Music* music, int* buflen);
void geGetIdTag(ge_Music* music, const char* file);

ge_Music* geLoadMusic(const char* file){
	ge_Music* music = (ge_Music*)geMalloc(sizeof(ge_Music));
	if(!music)return NULL;

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
	music->size = geFileTell(fp);
	geFileRewind(fp);

	music->ptr = (u8*)geMalloc(music->size + 8);
	if(music->ptr){
		geFileRead(fp, music->ptr, music->size);
	}else{
		geFileClose(fp);
		printf("err on %d+8\n", music->size);
		return 0;
	}

	//Get type and load
	u8 buffer[32] = { 0 };
	geFileSeek(fp, 0, GE_FILE_SEEK_SET);
	geFileRead(fp, buffer, 32);
	printf("buffer: \"%s\"\ntype: \"%s\"\n", buffer, type);
	if((buffer[0]=='I' && buffer[1]=='D' && buffer[2]=='3') || strstr(type, "mp3") || strstr(type, "MP3")){
		printf("is mp3\n");
		MP3_Init(music);
		music->type = GE_AUDIO_FILE_MP3;
	}
	printf("geLoadMusic 1 (%d)\n", music->type);

	geFileClose(fp);
	printf("geLoadMusic 2 (%d)\n", music->type);
	geGetIdTag(music, file);
	printf("geLoadMusic 3 (%d)\n", music->type);

	music->sample_rate = -1;
	music->speakers = GE_ALL_SPEAKERS;
	music->nChannels = 1;
	music->isPlaying = false;

	int (*thread)(int args, void* argp) = NULL;
	if(music->type == GE_AUDIO_FILE_MP3){
		printf("type MP3\n");
		thread = MP3_Thread;
	}
	printf("geLoadMusic 4 (%d)\n", music->type);
	if(thread){
		printf("creating thread\n");
		music->thread = geCreateThread("mp3_player_thread", thread, GE_THREAD_PRIORITY_TIME_CRITICAL);
		void* argp[2] = { (void*)music, 0x0 };
		geThreadStart(music->thread, sizeof(argp), argp);
		geSleep(300);
	}

	return music;
}

void geMusicSetSpeakers(ge_Music* music, int speakers){
	music->speakers = speakers;
}

void geMusicPlay(ge_Music* music){
	if(music->stopped){
		geAudioStartOutputLoop(music->handle);
		geTimerStart(music->timer);
		music->isPlaying = true;
		music->stopped = false;
		return;
	}

	music->isPlaying = true;
}

void geMusicPause(ge_Music* music){
	if(music->isPlaying){
		geAudioPauseOutputLoop(music->handle);
	}else{
		geAudioResumeOutputLoop(music->handle);
	}
	geTimerPause(music->timer);
	music->isPlaying = !music->isPlaying;
}

void geMusicStop(ge_Music* music){
	music->stopped = true;
	music->isPlaying = false;
	geAudioResetOutputLoop(music->handle);
	geTimerReset(music->timer);
}

void geFreeMusic(ge_Music* music){
}

short* geMusicGetBuffer(ge_Music* music, int* buflen){
	if(music->type == GE_AUDIO_FILE_MP3){
		return MP3_GetBuffer(music, buflen);
	}
	return NULL;
}

void geMusicDecodeCallback(ge_Music* music, void* fct, void* udata){
	if(!music)return;
	music->cb = (int(*)(ge_Music*,short*,int,void*))fct;
	music->cbdata = udata;
}

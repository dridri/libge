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

#define MP3_HEADER_MAX 0x200

static int Mp3GetInfoV3(unsigned char* buf, char* dest);
void Mp3IdTagV1(FILE* fp, int sz, ge_Music* mp3);
void Mp3IdTagV2(FILE* fp, int sz, ge_Music* mp3);
void Mp3IdTagV3(FILE* fp, int sz, ge_Music* mp3);
void GetTitleArtistByName(const char* filename, char* title, char* artist);

static void ffread(void* a, int b, int c, FILE* d){
	int ret = fread(a, b, c, d);
	(void)ret;
}

void Mp3IdTagV1(FILE* fp, int sz, ge_Music* mp3){
	char buf[128] = "";
	char tmp[64] = "";
	
	fseek(fp, -127, SEEK_END);
	ffread(buf, 1, 128, fp);
	
	if(!strncmp(buf, "TAG", 3))return;
	
	strncpy(mp3->title, &buf[3], 30);
	strncpy(mp3->artist, &buf[33], 30);
	strncpy(mp3->album, &buf[63], 30);
	strncpy(tmp, &buf[93], 30);
	sscanf(tmp, "%d", &mp3->year);
}

void Mp3IdTagV2(FILE* fp, int sz, ge_Music* mp3){
	char buf[MP3_HEADER_MAX] = "";
	char* tmp = NULL;
	char tmp2[256] = "";

	fseek(fp, 0, SEEK_SET);
	ffread(buf, 1, MP3_HEADER_MAX, fp);

	int i = 0;
	for(i=0; i<sz && i<MP3_HEADER_MAX; i++){
		if(buf[i]!='T')continue;
		tmp = &buf[i];
		if(!strncmp(tmp, "TP1", 3)){
			strcpy(mp3->artist, &tmp[7]);
		}
		if(!strncmp(tmp, "TT2", 3)){
			strcpy(mp3->title, &tmp[7]);
		}
		if(!strncmp(tmp, "TAL", 3)){
			strcpy(mp3->album, &tmp[7]);
		}
		if(!strncmp(tmp, "TCO", 3)){
			strcpy(mp3->genre, &tmp[7]);
		}
		if(!strncmp(tmp, "TYE", 3)){
			strcpy(tmp2, &tmp[7]);
			sscanf(tmp2, "%d", &mp3->year);
		}
		if(!strncmp(tmp, "TRC", 3)){
			strcpy(tmp2, &tmp[7]);
			sscanf(tmp2, "%d", &mp3->track);
		}
	}
}

void Mp3IdTagV3(FILE* fp, int sz, ge_Music* mp3){
	char buf[MP3_HEADER_MAX] = "";
	char* tmp = NULL;
	char tmp2[256] = "";

	fseek(fp, 0, SEEK_SET);
	ffread(buf, 1, MP3_HEADER_MAX, fp);

	int i = 0;
	for(i=0; i<sz && i<MP3_HEADER_MAX; i++){
		if(buf[i]!='T')continue;
		tmp = &buf[i];
		if(!strncmp(tmp, "TPE1", 4)){
			i += Mp3GetInfoV3((unsigned char*)tmp, mp3->artist);
		}
		if(!strncmp(tmp, "TALB", 4)){
			i += Mp3GetInfoV3((unsigned char*)tmp, mp3->album);
		}
		if(!strncmp(tmp, "TIT2", 4)){
			i += Mp3GetInfoV3((unsigned char*)tmp, mp3->title);
		}
		if(!strncmp(tmp, "TCON", 4)){
			i += Mp3GetInfoV3((unsigned char*)tmp, mp3->genre);
		}
		if(!strncmp(tmp, "TYER", 4)){
			i += Mp3GetInfoV3((unsigned char*)tmp, tmp2);
			sscanf(tmp2, "%d", &mp3->year);
		}
		if(!strncmp(tmp, "TRCK", 4)){
			i += Mp3GetInfoV3((unsigned char*)tmp, tmp2);
			sscanf(tmp2, "%d", &mp3->track);
		}
	}
}

void geGetIdTag(ge_Music* music, const char* file){
	FILE* fp = fopen(file, "rb");
	if(!fp)return;
	int ok = 0;
	
	fseek(fp, 0, SEEK_END);
	int sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	unsigned char tmp[16] = "";
	ffread(tmp, 1, 4, fp);

	if(music->type == GE_AUDIO_FILE_MP3 && strstr((char*)tmp, "ID3")){
		if(tmp[3] >= 0x03){
			Mp3IdTagV3(fp, sz, music);
		}else
		if(tmp[3] == 0x02){
			Mp3IdTagV2(fp, sz, music);
		}else{
			Mp3IdTagV1(fp, sz, music);
		}
		ok = 1;
	}
	
//	if(tmp[0]==0xff && tmp[1]==0xfb){
	if(ok == 0){
		GetTitleArtistByName(file, music->title, music->artist);
	}

	fclose(fp);
}

static int Mp3GetInfoV3(unsigned char* buf, char* dest){
	int stepping = 0;
	unsigned char* tmp = NULL;
	int len = 0;
	int ret = 0;

	//Check size
	tmp = &buf[4];
	len = tmp[3] | tmp[2]<<8 | tmp[1]<<16 | tmp[0]<<24;
	len -= 1;

	//Check ˙ţ
	if(buf[11]==0xff && buf[12]==0xfe){
		stepping = 1;
		len = (int)len/2 - 1;
	}

	if(len > 255){
		len = 255;
	}
	
	tmp = &buf[11+(stepping*2)];

	if(stepping){
		int a, b;
		for(a=0, b=0; a<256 && b<128 && b<len; a+=2, b++){
			dest[b] = (char)tmp[a];
		}
		ret = strlen(dest)*2 + 11;
	}else{
		strncpy(dest, (char*)tmp, len);
		ret = strlen(dest) + 11;
	}

	return ret;
}

char* strafterlast(char* out, char* in, char st);
void GetTitleArtistByName(const char* filename, char* title, char* artist){
	gePrintDebug(0x100, "GetTitleArtistByName('%s', 0x%08X, 0x%08X)\n", filename, title, artist);
	char* file = (char*)filename;
	if(strchr(file, '/')){
		file = strafterlast(file, file, '/');
	}
	gePrintDebug(0x100, "GetTitleArtistByName 2\n");

	int len = strlen(file) - strlen(strstr(file," - "));
	gePrintDebug(0x100, "GetTitleArtistByName 3\n");
	memcpy(artist, file, len);
	gePrintDebug(0x100, "GetTitleArtistByName 4\n");

	len = strlen(file) - len - 3;
	gePrintDebug(0x100, "GetTitleArtistByName 5\n");
	memcpy(title, strstr(file," - ")+3, len-4);
	gePrintDebug(0x100, "GetTitleArtistByName 6\n");
}

char* strafterlast(char* out, char* in, char st){
	char* s = strrchr(in, st);
	return s + 1;
}
 

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
#include <Mmreg.h>

#define WAVE_FORMAT_EXTENSIBLE 0xFFFE
/*
typedef struct {
  WAVEFORMATEX  Format;
  union {
    WORD  wValidBitsPerSample;
    WORD  wSamplesPerBlock;
    WORD  wReserved;
  } Samples;
  DWORD   dwChannelMask; 
  GUID    SubFormat;
} WAVEFORMATEXTENSIBLE, *PWAVEFORMATEXTENSIBLE;
*/
#define DEFINE_GUIDEX(name) const CDECL GUID name
#define DEFINE_GUIDSTRUCT(g, n) DEFINE_GUIDEX(n)
#define DEFINE_GUIDNAMED(n) n

#define CreateGuid(name,a,b,c,d) \
	name.Data1 = a; \
	name.Data2 = b; \
	name.Data3 = c; \
	name.Data4[0]=d[0]; name.Data4[1]=d[1]; name.Data4[2]=d[2]; name.Data4[3]=d[3]; \
	name.Data4[4]=d[4]; name.Data4[5]=d[5]; name.Data4[6]=d[6]; name.Data4[7]=d[7];

u32 MP3_Th(void *argp);
void* geAudioReserveOutput(int sample_rate, int bps, int input_channels, int speakers){

	WAVEFORMATEXTENSIBLE waveform;
	HWAVEOUT* hWaveOut = (HWAVEOUT*)malloc(sizeof(HWAVEOUT));
	WAVEHDR* WaveHdrOut = (WAVEHDR*)malloc(sizeof(WAVEHDR));

	waveform.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
	waveform.Format.nChannels = input_channels;
	waveform.Format.nSamplesPerSec = sample_rate;
	waveform.Format.nBlockAlign = (input_channels*bps) /8;
	waveform.Format.nAvgBytesPerSec = sample_rate * waveform.Format.nBlockAlign;
	waveform.Format.wBitsPerSample = bps;
	waveform.Format.cbSize = 22;
	waveform.Samples.wValidBitsPerSample = bps;
	waveform.dwChannelMask = speakers;

	u8 da[8] = { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 };
	CreateGuid(waveform.SubFormat, 0x00000001, 0x0000, 0x0010, da);

/*
	printf("Sound mappers :\n");
	int i = 0;
	for(i=0; i<waveOutGetNumDevs(); i++){
		WAVEOUTCAPS caps;
		waveOutGetDevCaps(i, &caps, sizeof(WAVEOUTCAPS));
		printf("\t%d: \"%s\"\n", i, caps.szPname);
	}
*/
	int dev = WAVE_MAPPER;
	int err = waveOutOpen(hWaveOut, dev, &waveform.Format, 0, 0, 0);
	if(err != MMSYSERR_NOERROR){
		printf("Win32 API waveOutOpen error %d\n", err);
		return 0;
	}
	waveOutSetVolume(*hWaveOut, 0xffffffff);

	u8* buf = (u8*)malloc(GE_OUTPUT_BUFFER_SIZE*8);
	WaveHdrOut->lpData = (void*)buf;
	WaveHdrOut->dwBufferLength = GE_OUTPUT_BUFFER_SIZE*4;
	WaveHdrOut->dwBytesRecorded = 0;
	WaveHdrOut->dwUser = 0;
	WaveHdrOut->dwFlags = WHDR_BEGINLOOP | WHDR_ENDLOOP;
	WaveHdrOut->dwLoops = -1;
	WaveHdrOut->lpNext = NULL;
	WaveHdrOut->reserved = 0;

	_ge_Audio_Header* _ge_head = (_ge_Audio_Header*)malloc(sizeof(_ge_Audio_Header));
	_ge_head->handle = (t_ptr)hWaveOut;
	_ge_head->winHdr = (t_ptr)WaveHdrOut;
	return (void*)_ge_head;
}

void geAudioCloseOutput(void* header){
	_ge_Audio_Header* head = (_ge_Audio_Header*)header;
	HWAVEOUT hWaveOut = *(HWAVEOUT*)head->handle;
	WAVEHDR* WaveHdrOut = (WAVEHDR*)head->winHdr;

	free(WaveHdrOut->lpData);
	waveOutClose(hWaveOut);
	free(head);
}

int geAudioOutputBlocking(void* header, void* buf, int size){
	_ge_Audio_Header* head = (_ge_Audio_Header*)header;
	HWAVEOUT* hWaveOut = (HWAVEOUT*)head->handle;
	WAVEHDR* WaveHdrOut = (WAVEHDR*)head->winHdr;
	waveOutReset(*hWaveOut);
	waveOutUnprepareHeader(*hWaveOut, WaveHdrOut, sizeof(WAVEHDR));

	WaveHdrOut->lpData = buf;
	WaveHdrOut->dwBufferLength = size;
	WaveHdrOut->dwBytesRecorded = 0;
	WaveHdrOut->dwUser = 0;
	WaveHdrOut->dwFlags = 0;
	WaveHdrOut->dwLoops = 0;
	WaveHdrOut->lpNext = NULL;
	WaveHdrOut->reserved = 0;

	waveOutPrepareHeader(*hWaveOut, WaveHdrOut, sizeof(WAVEHDR));
	waveOutWrite(*hWaveOut, WaveHdrOut, sizeof(WAVEHDR));
	return size;
}

void geAudioStartOutputLoop(void* header){
	_ge_Audio_Header* head = (_ge_Audio_Header*)header;
	HWAVEOUT* hWaveOut = (HWAVEOUT*)head->handle;
	WAVEHDR* WaveHdrOut = (WAVEHDR*)head->winHdr;

	waveOutPrepareHeader(*hWaveOut, WaveHdrOut, sizeof(WAVEHDR));
	waveOutWrite(*hWaveOut, WaveHdrOut, sizeof(WAVEHDR));
}

void geAudioStopOutputLoop(void* header){
	_ge_Audio_Header* head = (_ge_Audio_Header*)header;
	HWAVEOUT* hWaveOut = (HWAVEOUT*)head->handle;

	waveOutReset(*hWaveOut);
}

void* geAudioGetOutputBuffer(void* header){
	_ge_Audio_Header* head = (_ge_Audio_Header*)header;
	WAVEHDR* hdr = (WAVEHDR*)head->winHdr;
	return hdr->lpData;
}

void geAudioPauseOutputLoop(void* header){
	_ge_Audio_Header* head = (_ge_Audio_Header*)header;
	HWAVEOUT* hWaveOut = (HWAVEOUT*)head->handle;
	waveOutPause(*hWaveOut);
}

void geAudioResumeOutputLoop(void* header){
	_ge_Audio_Header* head = (_ge_Audio_Header*)header;
	HWAVEOUT* hWaveOut = (HWAVEOUT*)head->handle;
	waveOutRestart(*hWaveOut);
}

void geAudioResetOutputLoop(void* header){
	_ge_Audio_Header* head = (_ge_Audio_Header*)header;
	HWAVEOUT* hWaveOut = (HWAVEOUT*)head->handle;

	waveOutReset(*hWaveOut);
}

int geAudioGetPlayerPosition(void* header){
	_ge_Audio_Header* head = (_ge_Audio_Header*)header;
	HWAVEOUT hWaveOut = *(HWAVEOUT*)head->handle;
	MMTIME mmt;
	mmt.wType = 4;
	waveOutGetPosition(hWaveOut, &mmt, sizeof(MMTIME));
	return mmt.u.cb;
}

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

#ifndef __GE_AUDIO__
#define __GE_AUDIO__

#ifdef __cplusplus
extern "C" {
#endif

#include "getypes.h"
#include "gefile.h"
#define GE_INPUT_BUFFER_SIZE (5*8192)
//#define GE_OUTPUT_BUFFER_SIZE 2359296//->1152*2048
//#define GE_OUTPUT_BUFFER_SIZE 294912//->1152*256
#define GE_OUTPUT_BUFFER_SIZE 36864//->1152*32

#ifdef WIN32
#define GE_SPEAKER_FRONT_LEFT		0x1
#define GE_SPEAKER_FRONT_RIGHT		0x2
#define GE_SPEAKER_FRONT_CENTER		0x4
#define GE_SPEAKER_LOW_FREQUENCY	0x8
#define GE_SPEAKER_BACK_LEFT		0x10
#define GE_SPEAKER_BACK_RIGHT		0x20
#define GE_SPEAKER_FRONT_LEFT_OF_CENTER 	0x40
#define GE_SPEAKER_FRONT_RIGHT_OF_CENTER 	0x80
#define GE_SPEAKER_BACK_CENTER		0x100
#define GE_SPEAKER_SIDE_LEFT		0x200
#define GE_SPEAKER_SIDE_RIGHT		0x400
#elif(defined(PLATFORM_android))
#include <SLES/OpenSLES.h>
#define GE_SPEAKER_FRONT_LEFT		SL_SPEAKER_FRONT_LEFT
#define GE_SPEAKER_FRONT_RIGHT		SL_SPEAKER_FRONT_RIGHT
#define GE_SPEAKER_FRONT_CENTER		SL_SPEAKER_FRONT_CENTER
#define GE_SPEAKER_LOW_FREQUENCY	SL_SPEAKER_LOW_FREQUENCY
#define GE_SPEAKER_BACK_LEFT		SL_SPEAKER_BACK_LEFT
#define GE_SPEAKER_BACK_RIGHT		SL_SPEAKER_BACK_RIGHT
#define GE_SPEAKER_FRONT_LEFT_OF_CENTER 	SL_SPEAKER_FRONT_LEFT_OF_CENTER
#define GE_SPEAKER_FRONT_RIGHT_OF_CENTER 	SL_SPEAKER_FRONT_RIGHT_OF_CENTER
#define GE_SPEAKER_BACK_CENTER		SL_SPEAKER_BACK_CENTER
#define GE_SPEAKER_SIDE_LEFT		SL_SPEAKER_SIDE_LEFT
#define GE_SPEAKER_SIDE_RIGHT		SL_SPEAKER_SIDE_RIGHT
#else
#define GE_AUDIO_WRITE_INTERLEAVED	0x01
#define GE_AUDIO_WRITE_NONINTERLEAVED	0x02
#define GE_AUDIO_MMAP_NONINTERLEAVED	0x03

#define GE_SPEAKER_FRONT_LEFT		0x1
#define GE_SPEAKER_FRONT_RIGHT		0x2
#define GE_SPEAKER_FRONT_CENTER		0x4
#define GE_SPEAKER_LOW_FREQUENCY	0x8
#define GE_SPEAKER_BACK_LEFT		0x10
#define GE_SPEAKER_BACK_RIGHT		0x20
#define GE_SPEAKER_FRONT_LEFT_OF_CENTER 	0x40
#define GE_SPEAKER_FRONT_RIGHT_OF_CENTER 	0x80
#define GE_SPEAKER_BACK_CENTER		0x100
#define GE_SPEAKER_SIDE_LEFT		0x200
#define GE_SPEAKER_SIDE_RIGHT		0x400

#endif
#define GE_ALL_SPEAKERS		(GE_SPEAKER_FRONT_LEFT|GE_SPEAKER_FRONT_RIGHT|GE_SPEAKER_FRONT_CENTER| \
							GE_SPEAKER_LOW_FREQUENCY|GE_SPEAKER_BACK_LEFT|GE_SPEAKER_BACK_RIGHT|GE_SPEAKER_FRONT_LEFT_OF_CENTER| \
							GE_SPEAKER_FRONT_RIGHT_OF_CENTER|GE_SPEAKER_BACK_CENTER|GE_SPEAKER_SIDE_LEFT|GE_SPEAKER_SIDE_RIGHT)
	
typedef struct ge_Music ge_Music;
struct ge_Music {
	//User defs
	char title[128];
	char artist[128];
	char album[128];
	char genre[128];
	int year, track;
	bool isPlaying;
	bool loop_mode;
	ge_Timer* timer;
	int total_seconds;
	int total_minuts;
	int total_hours;

	int nChannels;
	int speakers;
	int cFrame;
	int (*cb)(ge_Music* music, short* samples, int nSamples, void* udata);
	void* cbdata;

	//File IO
	ge_File* fp;
	u8* ptr;
	int size;
	bool eos;
	int type;

	bool stopped;
/*
	short* _buf;

	//Mad
	u32 Stream;
	u32 Frame;
	u32 Synth;
	u32 Timer;
*/
	//System
	ge_Thread* thread;
	int sample_rate;
	void* decoder;
	void* handle;
};

typedef struct ge_Sound {
	int nChannels;
	int speakers;

	u8* ptr;
	int size;
	int type;

	int sample_rate;
	int bps;
	void* handle;
} ge_Sound;

typedef struct _ge_Audio_Header {
	t_ptr handle;
	t_ptr winHdr;
	u8* buffer;
	int buf_size;

	int period_size;
} _ge_Audio_Header;

LIBGE_API void* geAudioReserveOutput(int sample_rate, int bps, int input_channels, int speakers);
LIBGE_API void geAudioCloseOutput(void* header);
LIBGE_API int geAudioOutputBlocking(void* header, void* buf, int size);
LIBGE_API void* geAudioGetOutputBuffer(void* header);
LIBGE_API int geAudioGetOutputBufferSize(void* header);
LIBGE_API void geAudioSetVolume(void* header, int left, int right);

LIBGE_API void geAudioCreateOutputLoop(void* header, void* func, int user_data_size, void* user_data);
LIBGE_API void geAudioStartOutputLoop(void* header);
LIBGE_API void geAudioStopOutputLoop(void* header);
LIBGE_API void geAudioPauseOutputLoop(void* header);
LIBGE_API void geAudioResumeOutputLoop(void* header);
LIBGE_API void geAudioResetOutputLoop(void* header);
LIBGE_API int geAudioGetPlayerPosition(void* header);

LIBGE_API int geAudioDecode(char* file, void* callback, void* cbdata);

LIBGE_API ge_Music* geLoadMusic(const char* file);
LIBGE_API void geMusicSetSpeakers(ge_Music* music, int speakers);
LIBGE_API void geMusicPlay(ge_Music* music);
LIBGE_API void geMusicPause(ge_Music* music);
LIBGE_API void geMusicStop(ge_Music* music);
LIBGE_API short* geMusicGetBuffer(ge_Music* music, int* buflen);
LIBGE_API void geMusicDecodeCallback(ge_Music* music, void* fct, void* udata);

LIBGE_API ge_Sound* geLoadSound(const char* file);
LIBGE_API void geSoundSetSpeakers(ge_Sound* sound, int speakers);
LIBGE_API void geSoundPlay(ge_Sound* sound);

LIBGE_API void geAudioTransform(float* out, short* inbuf, int len, int mode);
LIBGE_API void geDrawAudioVisualizer(int X, int Y, int width, int height, void* data, int len, u32* colors, int mode);

#ifdef __cplusplus
}
#endif
#endif

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
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_android.h>

#define ASSERT(cond) \
	if((cond) == 0){ gePrintDebug(0x102, "Assertion failed with ("#cond") at line %d !\n", __LINE__); }

typedef struct ge_sl_stream {
  // engine interfaces
  SLObjectItf engineObject;
  SLEngineItf engineEngine;

  // output mix interfaces
  SLObjectItf outputMixObject;

  // buffer queue player interfaces
  SLObjectItf bqPlayerObject;
  SLPlayItf bqPlayerPlay;
  SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;
  SLEffectSendItf bqPlayerEffectSend;

  // buffer indexes
  int currentOutputIndex;

  // current buffer half (0, 1)
  int currentOutputBuffer;
  
  // buffers
  short* outBuf;
  short* outputBuffer[2];

  // size of buffers
  int outBufSamples;

  // locks
  int  last_outlock;
  int  outlock;

  double time;
  int outchannels;
  int   sr;
  SLuint32 cpos;
  SLuint32 blkpos;
} ge_sl_stream;

static void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void* p){
	((ge_sl_stream*)p)->outlock++;
}

static void waitThreadLock(ge_sl_stream* p){
	while(p->outlock == p->last_outlock){
		usleep(1000);
	}
	p->last_outlock = p->outlock;
}

void* geAudioReserveOutput(int sample_rate, int bps, int input_channels, int speakers){
	SLresult result;
	ge_sl_stream* handle;
	handle = (ge_sl_stream*)geMalloc(sizeof(ge_sl_stream));

	handle->outchannels = input_channels;
	handle->sr = sample_rate;
	handle->outlock = 0;
	handle->last_outlock = 0;
	handle->outBufSamples = GE_OUTPUT_BUFFER_SIZE;
	handle->outBuf = (short*)geMalloc(2 * GE_OUTPUT_BUFFER_SIZE * sizeof(short));
	handle->outputBuffer[0] = &handle->outBuf[0];
	handle->outputBuffer[1] = &handle->outBuf[GE_OUTPUT_BUFFER_SIZE];

	handle->currentOutputIndex = 0;
	handle->currentOutputBuffer = 0;
	handle->cpos = 0;
	handle->blkpos = 0;
	
	// Create Engine
	result = slCreateEngine(&(handle->engineObject), 0, NULL, 0, NULL, NULL);
	ASSERT(result == SL_RESULT_SUCCESS);
	result = (*handle->engineObject)->Realize(handle->engineObject, SL_BOOLEAN_FALSE);
	ASSERT(result == SL_RESULT_SUCCESS);
	result = (*handle->engineObject)->GetInterface(handle->engineObject, SL_IID_ENGINE, &(handle->engineEngine));
	ASSERT(result == SL_RESULT_SUCCESS);

//	GetAvailableAudioOutputs
	
	// Open Output Mix
	const SLInterfaceID ids[] = { SL_IID_VOLUME };
	const SLboolean req[] = { SL_BOOLEAN_FALSE };
	result = (*handle->engineEngine)->CreateOutputMix(handle->engineEngine, &(handle->outputMixObject), 1, ids, req);
	ASSERT(result == SL_RESULT_SUCCESS);

	result = (*handle->outputMixObject)->Realize(handle->outputMixObject, SL_BOOLEAN_FALSE);
	ASSERT(result == SL_RESULT_SUCCESS);
	
	SLDataLocator_AndroidSimpleBufferQueue loc_bufq = { SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2 };
	SLDataFormat_PCM format_pcm = { SL_DATAFORMAT_PCM, 2, sample_rate*1000, SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16, speakers, SL_BYTEORDER_LITTLEENDIAN };

	SLDataSource audioSrc = { &loc_bufq, &format_pcm };
	SLDataLocator_OutputMix loc_outmix = { SL_DATALOCATOR_OUTPUTMIX, handle->outputMixObject };
	SLDataSink audioSnk = { &loc_outmix, NULL };

	const SLInterfaceID ids1[] = { SL_IID_ANDROIDSIMPLEBUFFERQUEUE };
	const SLboolean req1[] = { SL_BOOLEAN_TRUE };
	result = (*handle->engineEngine)->CreateAudioPlayer(handle->engineEngine, &(handle->bqPlayerObject), &audioSrc, &audioSnk, 1, ids1, req1);
	ASSERT(result == SL_RESULT_SUCCESS);
	
	result = (*handle->bqPlayerObject)->Realize(handle->bqPlayerObject, SL_BOOLEAN_FALSE);
	ASSERT(result == SL_RESULT_SUCCESS);

	result = (*handle->bqPlayerObject)->GetInterface(handle->bqPlayerObject, SL_IID_PLAY, &(handle->bqPlayerPlay));
	ASSERT(result == SL_RESULT_SUCCESS);

	result = (*handle->bqPlayerObject)->GetInterface(handle->bqPlayerObject, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &(handle->bqPlayerBufferQueue));
	ASSERT(result == SL_RESULT_SUCCESS);

	result = (*handle->bqPlayerBufferQueue)->RegisterCallback(handle->bqPlayerBufferQueue, bqPlayerCallback, handle);
	ASSERT(result == SL_RESULT_SUCCESS);

	handle->outlock++;

	return handle;
}

void geAudioCloseOutput(void* header){
	ge_sl_stream* handle = (ge_sl_stream*)header;

	if (handle == NULL){
		return;
	}

	if(handle->bqPlayerObject != NULL){
		(*handle->bqPlayerObject)->Destroy(handle->bqPlayerObject);
		handle->bqPlayerObject = NULL;
		handle->bqPlayerPlay = NULL;
		handle->bqPlayerBufferQueue = NULL;
		handle->bqPlayerEffectSend = NULL;
	}

	if(handle->outputMixObject != NULL){
		(*handle->outputMixObject)->Destroy(handle->outputMixObject);
		handle->outputMixObject = NULL;
	}

	if(handle->engineObject != NULL){
		(*handle->engineObject)->Destroy(handle->engineObject);
		handle->engineObject = NULL;
		handle->engineEngine = NULL;
	}

	geFree(handle->outBuf);
	geFree(handle);
}

int geAudioOutputBlocking(void* header, void* buf, int size){
	ge_sl_stream* handle = (ge_sl_stream*)header;

	short *outBuffer, *inBuffer;
	int i;
	int bufsamps = handle->outBufSamples;
	int index = handle->currentOutputIndex;
	if(bufsamps == 0){
		return 0;
	}
	outBuffer = handle->outputBuffer[handle->currentOutputBuffer];

	for(i=0; i < size; i++){
		outBuffer[index] = ((short*)buf)[i];
		index++;
		if (index >= handle->outBufSamples) {
			waitThreadLock(handle);
			(*handle->bqPlayerBufferQueue)->Enqueue(handle->bqPlayerBufferQueue, outBuffer, bufsamps * sizeof(short));
			handle->currentOutputBuffer = (handle->currentOutputBuffer ? 0 : 1);
			index = 0;
			outBuffer = handle->outputBuffer[handle->currentOutputBuffer];
		}
	}
	handle->currentOutputIndex = index;
	handle->time += (double) size / (handle->sr * handle->outchannels);
	return i;
}

void geAudioStartOutputLoop(void* header){
	ge_sl_stream* handle = (ge_sl_stream*)header;
	SLresult result = (*handle->bqPlayerPlay)->SetPlayState(handle->bqPlayerPlay, SL_PLAYSTATE_PLAYING);
	ASSERT(result == SL_RESULT_SUCCESS);
}

void geAudioStopOutputLoop(void* header){
	ge_sl_stream* handle = (ge_sl_stream*)header;
	SLresult result = (*handle->bqPlayerPlay)->SetPlayState(handle->bqPlayerPlay, SL_PLAYSTATE_STOPPED);
	ASSERT(result == SL_RESULT_SUCCESS);
}

void* geAudioGetOutputBuffer(void* header){
	return ((ge_sl_stream*)header)->outBuf;
}

void geAudioPauseOutputLoop(void* header){
	ge_sl_stream* handle = (ge_sl_stream*)header;
	SLresult result = (*handle->bqPlayerPlay)->SetPlayState(handle->bqPlayerPlay, SL_PLAYSTATE_PAUSED);
	ASSERT(result == SL_RESULT_SUCCESS);
}

void geAudioResumeOutputLoop(void* header){
	ge_sl_stream* handle = (ge_sl_stream*)header;
	SLresult result = (*handle->bqPlayerPlay)->SetPlayState(handle->bqPlayerPlay, SL_PLAYSTATE_PLAYING);
	ASSERT(result == SL_RESULT_SUCCESS);
}

void geAudioResetOutputLoop(void* header){
	geAudioStopOutputLoop(header);
	geAudioStartOutputLoop(header);
}

int geAudioGetPlayerPosition(void* header){
	SLuint32 pos = 0;
	if(!header)return 0;
	ge_sl_stream* handle = (ge_sl_stream*)header;

	(*handle->bqPlayerPlay)->GetPosition(handle->bqPlayerPlay, &pos);
	pos = (pos * handle->sr / 1000) * sizeof(short) * handle->outchannels;
	if(pos < handle->cpos){
		handle->blkpos += handle->cpos;
	}

	handle->cpos = pos;
	return handle->blkpos + handle->cpos;
}

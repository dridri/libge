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
#include <alsa/asoundlib.h>

void ge_AudioDevicesList(){
	int err;
	int cardId = -1;
	snd_ctl_card_info_t* cardInfo = NULL;
	char str[64] = "";
	snd_ctl_t* cardHandle = NULL;

	snd_ctl_card_info_alloca(&cardInfo);
	while((err = snd_card_next(&cardId)) >= 0 && cardId >= 0){
		gePrintDebug(0x102, str, "hw:%i", cardId);
		if((err = snd_ctl_open(&cardHandle, str, 0)) < 0){
			gePrintDebug(0x102, "Can't open card %i: %s\n", cardId, snd_strerror(err));
			break;
		}
		if ((err = snd_ctl_card_info(cardHandle, cardInfo)) < 0){
			gePrintDebug(0x102, "Can't get info for card %i: %s\n", cardId, snd_strerror(err));
		}else{
			gePrintDebug(0x100, "Card %i = %s\n", cardId, snd_ctl_card_info_get_name(cardInfo));
		}
		snd_ctl_close(cardHandle);
	}
	gePrintDebug(0x100, "hello\n");
}

void* geAudioReserveOutput(int sample_rate, int bps, int input_channels, int speakers){
	int err, dir;
	snd_pcm_t* handle = NULL;
	snd_pcm_hw_params_t* hwparams;
	snd_pcm_sw_params_t* swparams;

	if((err = snd_pcm_open(&handle,"default", SND_PCM_STREAM_PLAYBACK, /*SND_PCM_NONBLOCK*/0)) < 0){
		gePrintDebug(0x102, "Can't open default PCM : %s\n", snd_strerror(err));
		return NULL;
	}

	int rrate = sample_rate;
	int buffer_time = 500000;
	int period_time = 100000;
	snd_pcm_uframes_t size = 0;
	snd_pcm_sframes_t buffer_size = 0;
	snd_pcm_sframes_t period_size = 0;

	snd_pcm_hw_params_malloc(&hwparams);
	snd_pcm_sw_params_malloc(&swparams);


	err = snd_pcm_hw_params_any(handle, hwparams);
	if(err < 0){
		gePrintDebug(0x102, "Broken configuration for playback: no configurations available: %s\n", snd_strerror(err));
		return NULL;
	}
	err = snd_pcm_hw_params_set_rate_resample(handle, hwparams, 1);
	if(err < 0){
		gePrintDebug(0x102, "Resampling setup failed for playback: %s\n", snd_strerror(err));
		return NULL;
	}
	err = snd_pcm_hw_params_set_access(handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED);
	if(err < 0){
		gePrintDebug(0x102, "Access type not available for playback: %s\n", snd_strerror(err));
		return NULL;
	}
	err = snd_pcm_hw_params_set_format(handle, hwparams, SND_PCM_FORMAT_S16);
	if(err < 0){
		gePrintDebug(0x102, "Sample format not available for playback: %s\n", snd_strerror(err));
		return NULL;
	}
	err = snd_pcm_hw_params_set_channels(handle, hwparams, input_channels);
	if(err < 0){
		gePrintDebug(0x102, "Channels count (%i) not available for playbacks: %s\n", input_channels, snd_strerror(err));
		return NULL;
	}
	err = snd_pcm_hw_params_set_rate_near(handle, hwparams, &rrate, 0);
	if(err < 0){
		gePrintDebug(0x102, "Rate %iHz not available for playback: %s\n", sample_rate, snd_strerror(err));
		return NULL;
	}
	if(rrate != sample_rate){
		gePrintDebug(0x102, "Rate doesn't match (requested %iHz, get %iHz)\n", sample_rate, rrate);
		return NULL;
	}
	buffer_size = GE_OUTPUT_BUFFER_SIZE / (input_channels * sizeof(short));
	err = snd_pcm_hw_params_set_buffer_size_near(handle, hwparams, &buffer_size);
	if(err < 0){
		gePrintDebug(0x102, "Unable to set buffer size for playback: %s\n", snd_strerror(err));
		return NULL;
	}

	err = snd_pcm_hw_params(handle, hwparams);
	if(err < 0){
		gePrintDebug(0x102, "Unable to set hw hwparams for playback: %s\n", snd_strerror(err));
		return NULL;
	}

	snd_pcm_start(handle);

	gePrintDebug(0x100, "geAudioReserveOutput OK (buffer: %d [%d]\n", buffer_size, GE_OUTPUT_BUFFER_SIZE / (input_channels * sizeof(short)));
	_ge_Audio_Header* _ge_head = (_ge_Audio_Header*)geMalloc(sizeof(_ge_Audio_Header));
	_ge_head->handle = (t_ptr)handle;
	_ge_head->buf_size = input_channels * sizeof(short) * buffer_size;
	_ge_head->buffer = (u8*)geMalloc(_ge_head->buf_size * 4);
	return (void*)_ge_head;
}

void geAudioCloseOutput(void* header){
}

int geAudioOutputBlocking(void* header, void* buf, int size){
	_ge_Audio_Header* head = (_ge_Audio_Header*)header;
	snd_pcm_writei((snd_pcm_t*)head->handle, buf, size);
}

void geAudioStartOutputLoop(void* header){
}

void geAudioStopOutputLoop(void* header){
}

void* geAudioGetOutputBuffer(void* header){
	_ge_Audio_Header* head = (_ge_Audio_Header*)header;
	return head->buffer;
}

int geAudioGetOutputBufferSize(void* header){
	_ge_Audio_Header* head = (_ge_Audio_Header*)header;
	return head->buf_size;
}

void geAudioPauseOutputLoop(void* header){
}

void geAudioResumeOutputLoop(void* header){
}

void geAudioResetOutputLoop(void* header){
}

int geAudioGetPlayerPosition(void* header){
	_ge_Audio_Header* head = (_ge_Audio_Header*)header;
	snd_pcm_status_t* status = NULL;
	snd_timestamp_t time;

	snd_pcm_status_alloca(&status);
	snd_pcm_status((snd_pcm_t*)head->handle, status);
	snd_pcm_status_get_tstamp(status, &time);
/*
	printf("time: %d\n", (int)time.tv_sec);
	printf("avail : %d\n", (int)snd_pcm_status_get_avail(status));
	printf("avail2 : %d\n", (int)snd_pcm_avail((snd_pcm_t*)head->handle));
	printf("delay: %d\n", (int)snd_pcm_status_get_delay(status));
*/
	return 0;
}

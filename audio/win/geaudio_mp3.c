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

#include <limits.h>
#include <mad.h>
#define BLOCK_SIZE 2097152
#define MadErrorString(x) mad_stream_errorstr(x)

static int PrintFrameInfo(struct mad_header *Header);
static signed short MadFixedToSshort(mad_fixed_t Fixed);

void MP3_TotalLength(ge_Music* mp3, int* hours, int* minuts, int* seconds);

typedef struct ge_Mp3Decoder {
	short* _buf;
	short* _buf2;
    unsigned long samplesOut;

	//Mad
	t_ptr Stream;
	t_ptr Frame;
	t_ptr Synth;
	t_ptr Timer;
} ge_Mp3Decoder;

short* MP3_GetBuffer(ge_Music* mp3, int* buflen){
	if(!mp3 ||!mp3->decoder){
		return NULL;
	}
	ge_Mp3Decoder* decoder = (ge_Mp3Decoder*)mp3->decoder;
	if(buflen){
		*buflen = 1152 * 2;
	}
//	return &decoder->_buf{decoder->samplesOut/**mp3->nChannels*/];
	return decoder->_buf2;
}

void MP3_Init(ge_Music* mp3){
	printf("MP3_Init 1\n");
	mp3->isPlaying = false;
	ge_Mp3Decoder* decoder = (ge_Mp3Decoder*)geMalloc(sizeof(ge_Mp3Decoder));
	mp3->decoder = decoder;
	printf("MP3_Init 2\n");

	decoder->Stream = (t_ptr)malloc(sizeof(struct mad_stream));
	decoder->Frame = (t_ptr)malloc(sizeof(struct mad_frame));
	decoder->Synth = (t_ptr)malloc(sizeof(struct mad_synth));
	decoder->Timer = (t_ptr)malloc(sizeof(mad_timer_t));
	printf("MP3_Init 3\n");

	mad_stream_init((struct mad_stream*)decoder->Stream);
	mad_frame_init((struct mad_frame*)decoder->Frame);
	mad_synth_init((struct mad_synth*)decoder->Synth);
	mad_timer_reset((mad_timer_t*)decoder->Timer);
	printf("MP3_Init 4\n");

	MP3_TotalLength(mp3, &mp3->total_hours, &mp3->total_minuts, &mp3->total_seconds);
	printf("MP3_Init 5\n");

    //ModPlay_Load("",data);
}

int geLoadMp3(ge_Music* mp3){

	return 0;
}

void gotoxy(int x, int y){
    printf("%c[%d;%df",0x1B,y,x);
}

int _ge__mp3_thread_test(int args, void* argp){
	ge_Music* mp3 = ((ge_Music**)argp)[0];
	ge_Mp3Decoder* decoder = ((ge_Mp3Decoder**)argp)[1];
	int* bc = ((int**)argp)[2];
	int* w = ((int**)argp)[3];
	while(1){
		while(!mp3->handle){
			Sleep(1);
		}
		int n;
		int p = geAudioGetPlayerPosition(mp3->handle)/4 - *bc/4;
		system("cls");
		printf("p: %d / %d [%d] [%d]\n", p, GE_OUTPUT_BUFFER_SIZE, *bc/4, *w);
		for(n=0; n<80; n++){
			if(n == p * 80 / (GE_OUTPUT_BUFFER_SIZE)){
				printf("#");
			}else{
				printf("-");
			}
		}
		printf("\n");
		for(n=0; n<80; n++){
			if(n == decoder->samplesOut * 80 / (GE_OUTPUT_BUFFER_SIZE)){
				printf("#");
			}else{
				printf("-");
			}
		}
		printf("\n");
	}
	return 0;
}

int MP3_Thread(int args, void *argp){
	printf("MP3_Thread 1\n");
	ge_Music* mp3 = (ge_Music*)((t_ptr*)argp)[0];
	ge_Mp3Decoder* decoder = (ge_Mp3Decoder*)mp3->decoder;
	struct mad_stream Stream = *(struct mad_stream*)decoder->Stream;
	struct mad_frame Frame = *(struct mad_frame*)decoder->Frame;
	struct mad_synth Synth = *(struct mad_synth*)decoder->Synth;
	
	printf("MP3_Thread 2\n");
	int numSamples = GE_OUTPUT_BUFFER_SIZE;

	mp3->timer = geCreateTimer("ge_mp3->timer");
	printf("MP3_Thread 3\n");
	
    decoder->samplesOut = 0;
	decoder->_buf2 = (short*)geMalloc(sizeof(short)*1152 * 2);

	mp3->cFrame = 0;
	int FrameCount = 0;
	bool first = true;
	bool error = false;
	(void)error;

	int w = 0;
	int pos = 0;
	int bc = 0;
	//int base_cursor = 0;
	int cursorpos = 0;
	int data_read = 0;
	int samples_read = 0;
	printf("MP3_Thread 4\n");
/*
	ge_Thread* th = geCreateThread("test", thread_test, 0);
	unsigned long int ap[5] = { mp3, decoder, &bc, &w, 0 };
	geThreadStart(th, sizeof(ap), ap);
*/
	mad_stream_buffer(&Stream, mp3->ptr, mp3->size);
	printf("MP3_Thread 5\n");
	while(1){
//		if(mp3->isPlaying){
			decoder->samplesOut = 0;
			data_read = 0;
		//	printf("Decoder: %d\nPlayer : %d\n\n", samples_read, cursorpos/2/2);
			while(decoder->samplesOut < numSamples){
				geTimerUpdate(mp3->timer);
				if(Stream.buffer == NULL || Stream.error == MAD_ERROR_BUFLEN){
					Stream.error = 0;
					continue;
				}

			//	printf("loop %d\n", decoder->samplesOut);
				if(mad_frame_decode(&Frame, &Stream)){
					if(MAD_RECOVERABLE(Stream.error)){
						printf("libmad error: %d  %s\n", Stream.error, MadErrorString(&Stream));
						error = true;
						continue;
					}else if(Stream.error == MAD_ERROR_BUFLEN){
						printf("libmad error: MAD_ERROR_BUFLEN\n");
						printf("end of stream\n");
					//	memset(decoder->_buf, 0, GE_OUTPUT_BUFFER_SIZE*4);
						memset(&decoder->_buf[decoder->samplesOut * 2], 0, (numSamples - decoder->samplesOut) * 2 * 2);
						if(!mp3->loop_mode)mp3->eos=true;
					}else{
					//	MP3_Stop();
					}
				}
			//	PrintFrameInfo(&Frame.header);
				if(FrameCount == 1){
					PrintFrameInfo(&Frame.header);
					mp3->sample_rate = Frame.header.samplerate;
					if (MAD_NCHANNELS(&Frame.header) == 2){
						mp3->nChannels = 2;
					}
					if(!mp3->handle)mp3->handle = geAudioReserveOutput(mp3->sample_rate, 16, 2/*mp3->nChannels*/, mp3->speakers);
					decoder->_buf = (short*)geAudioGetOutputBuffer(mp3->handle);
				//	geAudioStartOutputLoop(mp3->handle);
					FrameCount++;
					continue;
				}
				if(FrameCount <= 2){
					FrameCount++;
					continue;
				}
				if(FrameCount == 64){
					geAudioStartOutputLoop(mp3->handle);
					geTimerStart(mp3->timer);
				}
				if(FrameCount > 64+32){
				//	mp3->cFrame++;
				}
			//	mp3->cFrame = max(0, samples_read / 1152 - (64 + 32));
				FrameCount++;

				mad_synth_frame(&Synth, &Frame);

				pos = geAudioGetPlayerPosition(mp3->handle);
				cursorpos = pos;
				mp3->cFrame = pos / 2 / (MAD_NCHANNELS(&Frame.header) * 1152);

				int diff = samples_read*MAD_NCHANNELS(&Frame.header)*2-131072 - cursorpos;
				w = (diff * 4) / mp3->sample_rate;
				if(w<0)w=0;
				geSleep((u32) (w));

				if((pos-bc)/4 >= numSamples){
					bc = pos;
				}
				/*
				u32 ctime = geGetTick();
			//	while(geGetTick() - ctime < w){
					int n;
					int p = geAudioGetPlayerPosition(mp3->handle)/4 - bc/4;
					system("cls");
					printf("p: %d / %d [%d] [%d]\n", p, GE_OUTPUT_BUFFER_SIZE, bc/4, w);
					for(n=0; n<80; n++){
						if(n == p * 80 / (GE_OUTPUT_BUFFER_SIZE)){
							printf("#");
						}else{
							printf("-");
						}
					}
					printf("\n");
					for(n=0; n<80; n++){
						if(n == decoder->samplesOut * 80 / (GE_OUTPUT_BUFFER_SIZE)){
							printf("#");
						}else{
							printf("-");
						}
					}
					printf("\n");
			//	}
			*/
				
				int i = 0;
				unsigned long last_samplesOut = decoder->samplesOut;
				for(i = 0; i < Synth.pcm.length; i++){
					while(!mp3->isPlaying){
						geSleep(1000);
					}
					signed short Sample;
					if(decoder->samplesOut >= numSamples){
						break;
					}
					if(first){
						mp3->cFrame = 0;
						data_read = 0;
						samples_read = 0;
						first = false;
					}
					if(!first){
						if(mp3->eos){
							while(pos < samples_read*MAD_NCHANNELS(&Frame.header)*2){
								pos = geAudioGetPlayerPosition(mp3->handle);
								mp3->cFrame = pos / 2 / MAD_NCHANNELS(&Frame.header) / 1152;
								geSleep(4);
							}
							geAudioStopOutputLoop(mp3->handle);
							mp3->isPlaying = false;
							FrameCount = 0;
						}
					}
					if(mp3->stopped){
						mad_stream_buffer(&Stream, mp3->ptr, mp3->size);
						Sample = decoder->samplesOut = samples_read = 0;
						FrameCount = 1;
						cursorpos = data_read = 0;
						memset(decoder->_buf, 0, GE_OUTPUT_BUFFER_SIZE*4);
						break;
					}

					if(MAD_NCHANNELS(&Frame.header) == 2){
						//Left channel
						Sample = MadFixedToSshort(Synth.pcm.samples[0][i]);
						decoder->_buf[decoder->samplesOut * 2] = Sample;
						//Right channel
						Sample = MadFixedToSshort(Synth.pcm.samples[1][i]);
						decoder->_buf[decoder->samplesOut * 2 + 1] = Sample;

					}else{
						Sample = MadFixedToSshort(Synth.pcm.samples[0][i]);
						//Left channel
						decoder->_buf[decoder->samplesOut * 2] = Sample;
						//Right channel
						decoder->_buf[decoder->samplesOut * 2 + 1] = Sample;
					}
					if(mp3->handle/* && FrameCount >= 64*/){
					//	long int ofs = ((pos / 2) % numSamples);
					//	ofs -= (ofs % 1152);
						long int ofs = ((int)mp3->cFrame * 1152 * 2) % numSamples;
						if (ofs >= 0){
							decoder->_buf2[i * 2 + 0] = decoder->_buf[ofs + i*2 + 0];
							decoder->_buf2[i * 2 + 1] = decoder->_buf[ofs + i*2 + 1];
						}
					}

					decoder->samplesOut++;
					samples_read++;
					data_read = cursorpos;
				}
				if(mp3->cb != NULL){
					mp3->cb(mp3, &decoder->_buf[last_samplesOut*2], Synth.pcm.length, mp3->cbdata);
				}
			}
		//	base_cursor += numSamples/8;
//		}
	}
	return 0;
}

int MP3_FullDecode(void* ptr, int size, void* cb, void* cbud){
	struct mad_stream Stream;
	struct mad_frame Frame;
	struct mad_synth Synth;
	mad_timer_t Timer;
	mad_stream_init(&Stream);
	mad_frame_init(&Frame);
	mad_synth_init(&Synth);
	mad_timer_reset(&Timer);
	
    int samplesOut = 0;
	int numSamples = 1152 * 2048;

	bool first = true;
	bool error = false;
	(void)error;

	int samples_read = 0;

	mad_stream_buffer(&Stream, (const unsigned char*)ptr, size);

	//int _bufp = 0;
	short* _buf = (short*)geMalloc(numSamples*sizeof(short)*2);

	bool run = true;
	while(run){
		samplesOut = 0;
			while(samplesOut < numSamples){
				if(Stream.buffer == NULL || Stream.error == MAD_ERROR_BUFLEN){
					Stream.error = 0;
					continue;
				}

				if(mad_frame_decode(&Frame, &Stream)){
					if(MAD_RECOVERABLE(Stream.error)){
						printf("libmad error: %d  %s\n", Stream.error, MadErrorString(&Stream));
						error = true;
						continue;
					}else if(Stream.error == MAD_ERROR_BUFLEN){
						printf("libmad error: MAD_ERROR_BUFLEN\n");
						printf("end of stream\n");
						run = false;
						break;
					}else{
						run = false;
						break;
					}
				}

				mad_synth_frame(&Synth, &Frame);

				int frame_sample_base = samplesOut * 2;
				int i = 0;
				for(i = 0; i < Synth.pcm.length; i++){
					signed short Sample;
					if(samplesOut >= numSamples){
						break;
					}
					if(first){
						samples_read = 0;
						first = false;
					}

					if(MAD_NCHANNELS(&Frame.header) == 2){
						Sample = MadFixedToSshort(Synth.pcm.samples[0][i]);
						_buf[samplesOut * 2] = Sample;
						Sample = MadFixedToSshort(Synth.pcm.samples[1][i]);
						_buf[samplesOut * 2 + 1] = Sample;

					}else{
						Sample = MadFixedToSshort(Synth.pcm.samples[0][i]);
						_buf[samplesOut * 2] = Sample;
						_buf[samplesOut * 2 + 1] = Sample;
					}


					samplesOut++;
					samples_read++;
				}
				if(cb != NULL){
					int (*fct)(short* samples, int nSamples, int nChannels, void* udata) = (int(*)(short*,int,int,void*))cb;
					int ret = fct(&_buf[frame_sample_base], Synth.pcm.length, 2, cbud);
					if(ret != 0){
						run = false;
						break;
					}
				}
			}
	}

	geFree(_buf);
	return 0;
}

static int PrintFrameInfo(struct mad_header *Header){
    char Layer[64] = "";
	char Mode[64] = "";
	char Emphasis[64] = "";

    /* Convert the layer number to it's printed representation. */
    switch(Header->layer){
		case MAD_LAYER_I:
			sprintf(Layer, "I");
			break;
		case MAD_LAYER_II:
			sprintf(Layer, "II");
			break;
		case MAD_LAYER_III:
			sprintf(Layer, "III");
			break;
		default:
			sprintf(Layer, "(unexpected layer value)");
			break;
    }

    /* Convert the audio mode to it's printed representation. */
    switch(Header->mode){
		case MAD_MODE_SINGLE_CHANNEL:
			sprintf(Mode, "single channel");
			break;
		case MAD_MODE_DUAL_CHANNEL:
			sprintf(Mode, "dual channel");
			break;
		case MAD_MODE_JOINT_STEREO:
			sprintf(Mode, "joint (MS/intensity) stereo");
			break;
		case MAD_MODE_STEREO:
			sprintf(Mode, "normal LR stereo");
			break;
		default:
			sprintf(Mode, "(unexpected mode value)");
			break;
    }

    /* Convert the emphasis to it's printed representation. Note that
     * the MAD_EMPHASIS_RESERVED enumeration value appeared in libmad
     * version 0.15.0b.
     */
    switch(Header->emphasis){
		case MAD_EMPHASIS_NONE:
			sprintf(Emphasis, "no");
			break;
		case MAD_EMPHASIS_50_15_US:
			sprintf(Emphasis, "50/15 us");
			break;
		case MAD_EMPHASIS_CCITT_J_17:
			sprintf(Emphasis, "CCITT J.17");
			break;
		default:
			sprintf(Emphasis, "(unexpected emphasis value)");
			break;
    }
    printf("%lu kb/s audio MPEG layer %s %s stream at %dHz\n", Header->bitrate / 1000, Layer, Mode, Header->samplerate);
    return (0);
}

static signed short MadFixedToSshort(mad_fixed_t Fixed){
    /* Clipping */
    if (Fixed >= MAD_F_ONE)
	return (SHRT_MAX);
    if (Fixed <= -MAD_F_ONE)
	return (-SHRT_MAX);

    /* Conversion. */
    Fixed = Fixed >> (MAD_F_FRACBITS - 15);
    return ((signed short) Fixed);
}

void MP3_TotalLength(ge_Music* mp3, int* hours, int* minuts, int* seconds){
	ge_Mp3Decoder* decoder = (ge_Mp3Decoder*)mp3->decoder;
	struct mad_stream Stream = *(struct mad_stream*)decoder->Stream;
	struct mad_frame Frame = *(struct mad_frame*)decoder->Frame;

	int FrameCount = 0;
	mad_timer_t libMadlength;
	mad_timer_t current;
	mad_timer_reset(&libMadlength);

	mad_stream_buffer(&Stream, mp3->ptr, mp3->size);
	while(1){
    	if (mad_header_decode(&Frame.header, &Stream) == -1){
			if (Stream.buffer == NULL || Stream.error == MAD_ERROR_BUFLEN){
				break;
			}else if (MAD_RECOVERABLE(Stream.error)){
    			continue;
    		}else{
    			break;
    		}
		}else{
    		//Conteggio frame e durata totale:
    		FrameCount++;
    		mad_timer_add(&current, Frame.header.duration);
    		mad_timer_add(&libMadlength, Frame.header.duration);
		}
	}
	int sec = (int)( libMadlength.seconds );
	*seconds = (int)( sec % 60 );
	*minuts = (int)( sec / 60  );
	*hours = (int)( sec / 3600 );
}

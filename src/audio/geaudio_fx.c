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
#include "fix_fft.c"

static short* fft = NULL;
static int fft_size = 4096;
void geAudioTransform(float* out, short* inbuf, int len, int mode){
	int i, j;
	if(!fft){
		fft = (short*)geMalloc(sizeof(short)*4096);
	}

	switch((mode&0x0FFFFFFF)){
		case GE_AUDIO_TRANSFORM_FFT : {
			int N = fft_size;
			int log2FFT = (int)( geLog(N / 2) / geLog(2) );
			int log2N = log2FFT + 1;
		//	memcpy(fft, inbuf, sizeof(short)*N);
			for(i=0, j=0; i<N; i++, j++){
				if(j>=len)j=0;
				fft[i] = inbuf[j];
			}
			fix_fftr(fft, log2N, mode & GE_AUDIO_TRANSFORM_PASS2);
			int y;
			for(i=0; i<N; i++){
				y = fft[i];
				y = y < 0 ? -y : y;
				out[i] = ((float)y) / ((float)(32768/8));

				out[i] *= (1.0/0.8);
				out[i] = out[i] > 1.0 ? 1.0 : out[i];
			}
		}
		default : break;
	}
}

void geDrawAudioVisualizer(int X, int Y, int width, int height, void* data, int len, u32* colors, int mode){
	int x, y, i;
	short* pcm = (short*)data;
	float* pcmf = (float*)data;
	float* fft = (float*)data;
	u32 color1 = colors[1];

	switch(( mode & 0x00FFFFFF )){
		case GE_AUDIO_VISUALIZER_PCM : {
			for(x=0; x<(width-1); x++){
				y = pcm[x*len/width] * (height/2)/32768;
				int y1 = pcm[(x+1)*len/width] * (height/2)/32768;
			//	gePrintDebug(0x100, "%d, %d, %d, %d, 0x%08X\n", X+x, Y-y, X+x+1, Y-y1, colors[0]);
				geDrawLineScreen(X+x, Y-y, X+x+1, Y-y1, colors[0]);
			}
			break;
		}
		case GE_AUDIO_VISUALIZER_PCM_FLOAT : {
			for(x=0; x<(width-1); x++){
				y = pcmf[x*len/width] * (height/2)/32768;
				int y1 = pcmf[(x+1)*len/width] * (height/2)/32768;
				geDrawLineScreen(X+x, Y-y, X+x+1, Y-y1, colors[0]);
			}
			break;
		}
		case GE_AUDIO_VISUALIZER_PLAIN : {
			for(x=0; x<width; x++){
				y = pcm[x*len/width] * (height/2)/32768;
				geDrawLineScreenFade(X+x, Y, X+x, Y-y, colors[0], colors[1]);
			}
			break;
		}
		case GE_AUDIO_VISUALIZER_FIRE : {
			for(x=0; x<width; x++){
				y = pcm[x*len/width] * height/32768;
				y = y < 0 ? -y : y;
				geDrawLineScreenFade(X+x, Y, X+x, Y-y, colors[0], colors[1]);
			}
			break;
		}
		case GE_AUDIO_VISUALIZER_FFT : {
			for(x=0, i=0; x<width; x++, i=x*len/width){
				y = (int)( fft[i] * height );
				y = y < 0 ? -y : ( y > height ? height : y );
				if(!(mode & GE_AUDIO_VISUALIZER_FIX_COLORS)){
					color1 = geMixColors(colors[0], colors[1], ((float)y)/((float)height));
				}
				if(mode & GE_AUDIO_VISUALIZER_REVERSE){
					geDrawLineScreenFade(X+x, Y, X+x, Y+y, colors[0], color1);
				}else{
					geDrawLineScreenFade(X+x, Y, X+x, Y-y, colors[0], color1);
				}
			}
			break;
		}
		default : break;
	}
}

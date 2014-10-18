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

void ge_AudioDevicesList(){
}

void* geAudioReserveOutput(int sample_rate, int bps, int input_channels, int speakers){
	return 0;
}

void geAudioCloseOutput(void* header){
}

int geAudioOutputBlocking(void* header, void* buf, int size){
	return 0;
}

void geAudioStartOutputLoop(void* header){
}

void geAudioStopOutputLoop(void* header){
}

void* geAudioGetOutputBuffer(void* header){
	return 0;
}

int geAudioGetOutputBufferSize(void* header){
	return 0;
}

void geAudioPauseOutputLoop(void* header){
}

void geAudioResumeOutputLoop(void* header){
}

void geAudioResetOutputLoop(void* header){
}

int geAudioGetPlayerPosition(void* header){
	return 0;
}

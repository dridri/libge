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

#define getIntBuffer(b,a) ((int)((u8)b[a] | (u16)(b[a+1]<<8) | (u32)(b[a+2]<<16) | (u32)(b[a+3]<<24)))
#define getShortBuffer(b,a) ((short)((u8)b[a] | (u16)(b[a+1]<<8)))

int LoadWav(ge_Sound* sound){
	printf("\n\n\n");
	u8* buffer = sound->ptr;
	int block_sz = getIntBuffer(buffer, 16);
	int fmt = getShortBuffer(buffer, 20);

	if( !(buffer[12]=='f' && buffer[13]=='m' && buffer[14]=='t' && buffer[15]==' ' && block_sz==16 && fmt==1)){
		return -1;
	}
	sound->nChannels = getShortBuffer(buffer, 22);
	sound->sample_rate = (u16)getShortBuffer(buffer, 24);
	sound->bps = getShortBuffer(buffer, 30) *8;

	return 0;
}

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

#include "libge.h"

typedef struct xpm_color {
	char key[8];
	u32 color;
} xpm_color;

ge_Image* geLoadXPM(ge_File* file){
	geFileSeek(file, 0, GE_FILE_SEEK_END);
	int size = geFileTell(file);
	geFileRewind(file);
	char* buf = (char*)geMalloc(size);
	geFileRead(file, buf, size);

	if(strstr(buf, "static char")){
		// File mode, so convert into buffer
		geFileRewind(file);
		char buffer[1024];
		char* buf2 = (char*)geMalloc(size);
		int i = 0;
		int ln = 0;
		while(geFileGets(file, buffer, 1024)){
			ln++;
			if(strchr(buffer, '"')){
				char b[1024];
				strcpy(b, strchr(buffer, '"')+1);
				if(!strchr(b, '"')){
					gePrintDebug(0x102, "[%s]:%d: Missing \" terminating character !\n", file->filename, ln);
					return NULL;
				}
				strchr(b, '"')[0] = 0x0;
				strcpy(&buf2[i], b);
				i += strlen(b);
			}
		}
		geFree(buf);
		buf = buf2;
	}

	if(strstr(buf, "*/")){
		memmove(buf, strstr(buf, "*/")+2, size-((strstr(buf, "*/")+2)-buf));
	}

	int i, j, k;
	int attribs[4];
	geGetParamIntMulti(buf, "", attribs, 4);
	int width = attribs[0];
	int height = attribs[1];
	int n_colors = attribs[2];
	int keys_len = attribs[3];
	xpm_color* colors = (xpm_color*)geMalloc(sizeof(xpm_color)*n_colors);
	gePrintDebug(0x100, "XPM attribs : %d %d %d %d\n", attribs[0], attribs[1], attribs[2], attribs[3]);

	i = k = 0;
	while(k < 4){
		if(buf[i] >= '0' && buf[i] <= '9'){
		}else{
			k++;
		}
		if(k<4)i++;
	}

	for(j=0; j<n_colors; j++){
		strncpy(colors[j].key, &buf[i], keys_len);
		i += keys_len;
		while(buf[i] != 'c')i++;
		while(buf[i] != '#'){
			if(!strncmp(&buf[i], "None", 4)){
				break;
			}
			i++;
		}
		if(!strncmp(&buf[i], "None", 4)){
			colors[j].color = 0x00000000;
			i += 4;
		}else
		if(buf[i] == '#'){
			char _r[3] = { 0 };
			char _g[3] = { 0 };
			char _b[3] = { 0 };
			int r, g, b;
			strncpy(_r, &buf[i+1], 2);
			strncpy(_g, &buf[i+3], 2);
			strncpy(_b, &buf[i+5], 2);
			sscanf(_r, "%X", (unsigned int*)&r);
			sscanf(_g, "%X", (unsigned int*)&g);
			sscanf(_b, "%X", (unsigned int*)&b);
			colors[j].color = RGBA(r, g, b, 255);
			i += 7;
		}
		gePrintDebug(0x100, "XPM color[%d] = { \"%s\", 0x%08X }\n", j, colors[j].key, colors[j].color);
	}

	ge_Image* image = geCreateSurface(width, height, 0x00000000);

	int x, y;
	for(y=0; y<height; y++){
		for(x=0; x<width; x++){
			u32 color = 0x00000000;
			for(k=0; k<n_colors; k++){
				if(!strncmp(&buf[i], colors[k].key, keys_len)){
					color = colors[k].color;
					break;
				}
			}
			image->data[y*image->textureWidth + x] = color;
			i++;
		}
	}

	return image;
}

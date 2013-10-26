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

static bool CheckIn(const char* buffer, const char* attrib){
//	gePrintDebug(0x100, "CheckIn(\"%s\", \"%s\")\n", buffer, attrib);
	if(!strlen(attrib)){
//		gePrintDebug(0x100, "  Ok\n");
		return true;
	}
	if(!strstr(buffer,attrib)){
		return false;
	}
	
	char* pos = strstr(buffer, attrib);
	char next = pos[strlen(attrib)];
	if(pos[strlen(attrib) - 1] != '>' && next != ' ' && next != '\t' && next != '\n' && next != '=' && next != ',' && next != ';'){
		return false;
	}

	if(pos != buffer && ((attrib[0] >= 'a' && attrib[0] <= 'z') || (attrib[0] >= 'A' && attrib[0] <= 'Z') || (attrib[0] >= '0' && attrib[0] <= '9'))){
		char prev = pos[-1];
		if(prev != ' ' && prev != '\t'){
			return false;
		}
	}
	
//	gePrintDebug(0x100, "  Ok\n");
	return true;
}

bool geGetParamInt(const char* buffer, const char* attrib, int* dest){
	if(!CheckIn(buffer,attrib))return false;
	bool ok = false;
	int i = 0;
	int start = (strstr(buffer,attrib)-buffer) + strlen(attrib);
	int len = strlen(buffer);

	for(i=start; i<len; i++){
		if((buffer[i]>='0' && buffer[i]<='9')    || (buffer[i]=='-' && buffer[i+1]>='0' && buffer[i+1]<='9')){
			sscanf(buffer+i, "%d", &dest[0]);
			ok = true;
			break;
		}
	}

	return ok;
}

int geGetParamIntMulti(const char* buffer, const char* attrib, int* dest, int num){
	if(!CheckIn(buffer,attrib))return false;
	int i = 0;
	int n = 0;
	int start = (strstr(buffer,attrib)-buffer) + strlen(attrib);
	int len = strlen(buffer);

	for(i=start; i<len; i++){
		if((buffer[i]>='0' && buffer[i]<='9')    || (buffer[i]=='-' && buffer[i+1]>='0' && buffer[i+1]<='9')){
		//	sscanf(buffer+i, "%d", &dest[n]);
			char buf[16];
			strncpy(buf, &buffer[i], 16);
			sscanf(buf, "%d", &dest[n]);
			while(buffer[i]>='0' && buffer[i]<='9')i++;
			n++;
			if(n == num){
				break;
			}
		}
	}

	return n;
}

bool geGetParamDouble(const char* buffer, const char* attrib, double* dest){
	if(!CheckIn(buffer,attrib))return false;
	bool ok = false;
	int i = 0;
	int start = (strstr(buffer,attrib)-buffer) + strlen(attrib);
	int len = strlen(buffer);

	for(i=start; i<len; i++){
		if((buffer[i]>='0' && buffer[i]<='9')    || (buffer[i]=='-' && buffer[i+1]>='0' && buffer[i+1]<='9')){
			sscanf(buffer+i, "%e", (float*)dest);
			ok = true;
			break;
		}
	}

	return ok;
}

bool geGetParamFloat(const char* buffer, const char* attrib, float* dest){
	if(!CheckIn(buffer,attrib))return false;
	bool ok = false;
	int i = 0;
	int start = (strstr(buffer,attrib)-buffer) + strlen(attrib);
	int len = strlen(buffer);

	for(i=start; i<len; i++){
		if((buffer[i]>='0' && buffer[i]<='9')    || (buffer[i]=='-' && buffer[i+1]>='0' && buffer[i+1]<='9')){
			sscanf(&buffer[i], "%f", dest);
			ok = true;
			break;
		}
	}

	return ok;
}

int geGetParamFloatMulti(const char* buffer, const char* attrib, float* dest, int num){
	if(!CheckIn(buffer,attrib))return false;
	int i = 0;
	int n = 0;
	int start = (strstr(buffer,attrib)-buffer) + strlen(attrib);
	int len = strlen(buffer);

	for(i=start; i<len; i++){
		if( (buffer[i]>='0' && buffer[i]<='9')    || (buffer[i]=='-' && buffer[i+1]>='0' && buffer[i+1]<='9') ){
		//	sscanf(&buffer[i], "%f", &dest[n]);
			char buf[18];
			strncpy(buf, &buffer[i], 18);
			sscanf(buf, "%f", &dest[n]);
			while((buffer[i]>='0' && buffer[i]<='9') || buffer[i]=='.' || buffer[i]=='-')i++;
			n++;
			if(n == num){
				break;
			}
			i--;
		}
	}

	return n;
}

int geGetParamString(const char* buffer, const char* attrib, char* dest, int max){
	memset(dest, 0, max);
	if(!CheckIn(buffer,attrib))return false;
	int i = 0;
	int start = (strstr(buffer,attrib)-buffer) + strlen(attrib);
	int len = strlen(buffer);
	int j = 0;
	bool cp = false;

	for(i=start; i<len; i++){
		if(buffer[i] == '\"' && !cp){
			cp = true;
			continue;
		}
		if(cp){
			if(buffer[i] == '\"'){
				dest[j] = 0x0;
				break;
			}
			dest[j] = buffer[i];
			j++;
		}
	}

	if(!cp){
		for(i=start; i<len; i++){
			if(buffer[i] == ' ' && !cp){
				start++;
			//	cp = 1;
				continue;
			}
			if(buffer[i] == '\n'){
				break;
			}
			dest[j] = buffer[i];
			j++;
		}
		if(dest[j-1] != 0x0){
	//		dest[j-1] = 0x0;
		}
	}

	return start;
}

int geGetParamStringMulti(const char* buffer, char* separators, char** dest, int max_len, int max_dest){
	int i = 0;
	int len = strlen(buffer);
	int count = 0;
	bool cp = true;
	int j = 0;
	int s = 0;

	for(i=0; i<len; i++){
		for(s=0; s<strlen(separators); s++){
			if(buffer[i] == separators[s]){
				i++;
				if(cp){ //Terminated one string
					dest[count][j] = 0x0;
					count++;
					if(count == max_dest){
						i = -1;
						break;
					}
				}
			//	cp = !cp;
				if(cp){
					j = 0;
				}
				continue;
			}
		}
		if(i == -1){
			break;
		}
		if(cp){
			dest[count][j] = buffer[i];
			j++;
		}
	}
/*
	if(!cp){
		for(i=0; i<len; i++){
			if(buffer[i] == ' '){
				if(cp){ //Terminated one string
					dest[count][j] = 0x0;
					count++;
				}
				cp = !cp;
				if(cp){
					j = 0;
				}
				continue;
			}
			if(cp){
				dest[count][j] = buffer[i];
				j++;
				if(j == max_len){
					dest[count][j-1] = 0x0;
				}
			}
		}
	}
*/
	return count;
}

int geGetStringList(const char* buffer, char** dest, int max_len, int max_dest){
	int i = 0;
	int len = strlen(buffer);
	int count = 0;
	bool cp = false;
	int j = 0;

	for(i=0; i<len; i++){
		if(buffer[i] == '"'){
			if(cp){ //Terminated one string
				dest[count][j] = 0x0;
				count++;
			}
			cp = !cp;
			if(cp){
				j = 0;
			}
			continue;
		}
		if(cp){
			dest[count][j] = buffer[i];
			j++;
		}
	}

	return count;
}

void geBufferEncryptIndex(ge_Buffer* buffer, u8* key, int key_size, int key_index){
	int i = 0, j = 0, j1 = 0, j2 = 0;
	u8* buf = (u8*)buffer->ptr;
	
	for(i=0, j=key_index, j1=key_index+1, j2=key_index+2; i<buffer->size; i++, j++, j1=j+1, j2=j+2){
		if(j>=key_size)j-=key_size;
		if(j1>=key_size)j1-=key_size;
		if(j2>=key_size)j2-=key_size;
		char src = (char)buf[i];
		char dst = 0;

		dst = src ^ key[j];
		dst = dst ^ (key[j] & key[j2]);
		dst += (key[j] & key[j1] & key[j2]);
		dst = dst ^ (key[j] & key[j1]);
		dst = dst ^ (key[j] ^ key[j1] ^ key[j2]) ^ (key[j] & key[j1] & key[j2]);
		dst -= (key[j] & key[j1] & key[j2]);
		dst += key[j];

		buf[i] = (u8)dst;
	}
}

void geBufferDecryptIndex(ge_Buffer* buffer, u8* key, int key_size, int key_index){
	int i = 0, j = 0, j1 = 0, j2 = 0;
	u8* buf = (u8*)buffer->ptr;
	
	for(i=0, j=key_index, j1=key_index+1, j2=key_index+2; i<buffer->size; i++, j++, j1=j+1, j2=j+2){
		if(j>=key_size)j-=key_size;
		if(j1>=key_size)j1-=key_size;
		if(j2>=key_size)j2-=key_size;
		char dst = (char)buf[i];
		char src = 0;
		
		dst -= key[j];
		dst += (key[j] & key[j1] & key[j2]);
		dst = dst ^ (key[j] ^ key[j1] ^ key[j2]) ^ (key[j] & key[j1] & key[j2]);
		dst = dst ^ (key[j] & key[j1]);
		dst -= (key[j] & key[j1] & key[j2]);
		dst = dst ^ (key[j] & key[j2]);
		src = dst ^ key[j];

		buf[i] = (u8)src;
	}
}

void geBufferEncrypt(ge_Buffer* buffer, u8* key, int key_size){
	geBufferEncryptIndex(buffer, key, key_size, 0);
}

void geBufferDecrypt(ge_Buffer* buffer, u8* key, int key_size){
	geBufferDecryptIndex(buffer, key, key_size, 0);
}

int geBufferRead(ge_Buffer* buffer, void* data, int len){
	if(buffer->cursor + len > buffer->size){
		len = buffer->size - buffer->cursor;
	}
	memcpy(data, &((u8*)buffer->ptr)[buffer->cursor], len);
	buffer->cursor += len;
	return len;
}

int geBufferWrite(ge_Buffer* buffer, void* data, int len){
	if(buffer->cursor + len > buffer->size){
		buffer->size = buffer->cursor + len + 1;
	//	buffer->size = buffer->size + 4096 * (int)((len + 4096) / 4096);
		buffer->ptr = geRealloc(buffer->ptr, buffer->size);
	}
	memcpy(&((u8*)buffer->ptr)[buffer->cursor], data, len);
	buffer->cursor += len;
	return len;
}

void gePrintBuffer(void* _buf, int size){
	unsigned char* buf = (unsigned char*)_buf;
	int b, b0, c;
	gePrintDebug(0x103, "  ");
	for(b=0; b<size; b++){
		b0 = b;
		gePrintDebug(0x103, " %02X", buf[b]);
		if((b > 0 && (b+1) % 8 == 0) || b+1 == size){
			gePrintDebug(0x103, " ");
		}
		if((b > 0 && (b+1) % 16 == 0) || b+1 == size){
			int dec = 15;
			if((b+1) % 16 != 0){
				dec = ((b+1)%16);
				for(c=0; c < 16-dec; c++){
					gePrintDebug(0x103, "   ");
				}
#ifdef WIN32
				gePrintDebug(0x103, " ");
#endif
			}
			for(c=0; c<16 && (b0-dec+c) < size; c++){
				if(buf[b0-dec+c] < 0x20){
					gePrintDebug(0x103, ".");
				}else{
					gePrintDebug(0x103, "%c", buf[b0-dec+c]);
				}
			}
		}
		if(b > 0 && (b+1) % 16 == 0){
			gePrintDebug(0x103, "\n  ");
		}
	}
	gePrintDebug(0x103, "\n");
}

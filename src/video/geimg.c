/*
	The Game Engine Library is a multiplatform library made to make games
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
void drawLine(int x0, int y0, int x1, int y1, u32 color, u32* destination, int width, int w, int h);

#ifndef min
#	define min(x,y) (x<y?x:y)
#endif

u32 geColorMix(u32 c0, u32 c1, float f){
	float rev = 1.0 - f;
	u8 r = (u8)min( ((float)R(c0))*rev + ((float)R(c1))*f , 255 );
	u8 g = (u8)min( ((float)G(c0))*rev + ((float)G(c1))*f , 255 );
	u8 b = (u8)min( ((float)B(c0))*rev + ((float)B(c1))*f , 255 );
	u8 a = (u8)min( ((float)A(c0)) + ((float)A(c1)) , 255 );
	return RGBA(r, g, b, a);
}

void geDrawLineImage(int x0, int y0, int x1, int y1, u32 color, ge_Image* dest){
	drawLine(x0, y0, x1, y1, color, dest->data, dest->textureWidth, dest->width, dest->height);
}

void geFillRectImage(int x, int y, int width, int height, u32 color, ge_Image* destination){
	if(!destination)return;
	int skipX = destination->textureWidth - width;
	int X, Y;
	u32* data = &destination->data[x + y * destination->textureWidth];
	for (Y = 0; Y < height; Y++, data += skipX) {
		for (X = 0; X < width; X++, data++){
			*data = color;
		}
	}
}

void geBlitImageToImage(int x, int y, ge_Image* source, int sx, int sy, int width, int height, ge_Image* destination){
	if(x < 0){
		sx += (-x);
		x = 0;
	}
	if(y < 0){
		sy += (-y);
		y = 0;
	}
	if(x + width >= destination->width){
		width = destination->width - x;
	}
	if(y + height >= destination->height){
		height = destination->height - y;
	}
	u32* destinationData = &destination->data[destination->textureWidth * y + x];
	int destinationSkipX = destination->textureWidth - width;
	u32* sourceData = &source->data[source->textureWidth * sy + sx];
	int sourceSkipX = source->textureWidth - width;
	int X, Y;
	u8 a;
	for (Y = 0; Y < height; Y++, destinationData += destinationSkipX, sourceData += sourceSkipX) {
		for (X = 0; X < width; X++, destinationData++, sourceData++) {
			u32 color = *sourceData;
			a = color >> 24;
		//	if(a >= 0x7E) *destinationData = color;
			*destinationData = geColorMix(*destinationData, color, (float)a / 255.0f);
		}
	}
}

void geBlitImageToImageStretched(int x, int y, ge_Image* source, int sx, int sy, int ex, int ey, int width, int height, ge_Image* destination){
	int X, Y;
	float rx = (float)ex / (float)width;
	float ry = (float)ey / (float)height;
//	printf("rx : %f = %d / %d\n", rx, ex, width);
//	printf("ry : %f = %d / %d\n", ry, ey, height);
	for(Y=0; Y<height; Y++){
		for(X=0; X<width; X++){
			int _x = (int)( ((float)X)*rx ) + sx;
			int _y = (int)( ((float)Y)*ry ) + sy;
			u32 color = source->data[_x + _y*source->textureWidth];
			u32 a = A(color);
			int dx = X + x;
			int dy = Y + y;
			u32* destinationData = &destination->data[dx + dy*destination->textureWidth];
			*destinationData = geColorMix(*destinationData, color, (float)a / 255.0f);
		}
	}
}

static ge_Image* circlebuf = NULL;

void geDrawCircle(int x, int y, int r, u32 color)
{
	if(circlebuf == NULL || circlebuf->width != geGetContext()->width || circlebuf->height != geGetContext()->height){
		if(circlebuf){
			geFreeImage(circlebuf);
		}
		circlebuf = geCreateSurface(geGetContext()->width, geGetContext()->height, 0);
	}

	memset(circlebuf->data, 0, circlebuf->textureWidth * circlebuf->height * 4);

	int i, j;
	for(j=-r; j<r; j++){
		for(i=-r; i<r; i++){
			int a = i*i + j*j;
			if(a >= r*r - r*2 && a <= r*r){
			//	geFillRectScreen(x + i, y + j, 1, 1, color);
				circlebuf->data[(y + j) * circlebuf->textureWidth + x + i] = color;
			}
		}
	}

	geUpdateImage(circlebuf);
	geDrawImage(0, 0, circlebuf);
}

void geFillCircle(int x, int y, int r, u32 color)
{
	if(circlebuf == NULL || circlebuf->width != geGetContext()->width || circlebuf->height != geGetContext()->height){
		if(circlebuf){
			geFreeImage(circlebuf);
		}
		circlebuf = geCreateSurface(geGetContext()->width, geGetContext()->height, 0);
	}

	memset(circlebuf->data, 0, circlebuf->textureWidth * circlebuf->height * 4);

	int i, j;
	for(j=-r; j<r; j++){
		for(i=-r; i<r; i++){
			if(i*i + j*j <= r*r){
			//	geFillRectScreen(x + i, y + j, 1, 1, color);
				circlebuf->data[(y + j) * circlebuf->textureWidth + x + i] = color;
			}
		}
	}

	geUpdateImage(circlebuf);
	geDrawImage(0, 0, circlebuf);
}

u32 geImagePixel(int x, int y, ge_Image* img){
	if(!img || x<0 || y<0 || x>=img->width || y>=img->height)return 0;
	return img->data[x + y*img->textureWidth];
}

void geDrawImage(int x, int y, ge_Image* img){
	if(!img)return;
	geBlitImageStretched(x, y, img, 0, 0, img->width, img->height, img->width, img->height, 0);
}

void geDrawImageStretched(int x, int y, ge_Image* img, int width, int height){
	if(!img)return;
	geBlitImageStretched(x, y, img, 0, 0, img->width, img->height, width, height, 0);
}

void geDrawImageRotated(int x, int y, ge_Image* img, float angle, bool centered){
	if(!img)return;
	geBlitImageRotated(x, y, img, 0, 0, img->width, img->height, angle, centered? GE_BLIT_CENTERED : 0);
}

void geDrawImageDepth(int x, int y, int z, ge_Image* img){
	if(!img)return;
	geBlitImageDepthStretched(x, y, z, img, 0, 0, img->width, img->height, img->width, img->height, 0);
}

void geDrawImageDepthStretched(int x, int y, int z, ge_Image* img, int width, int height){
	if(!img)return;
	geBlitImageDepthStretched(x, y, z, img, 0, 0, img->width, img->height, width, height, 0);
}

void geDrawImageDepthRotated(int x, int y, int z, ge_Image* img, float angle, bool centered){
	if(!img)return;
	geBlitImageDepthRotated(x, y, z, img, 0, 0, img->width, img->height, angle, centered? GE_BLIT_CENTERED : 0);
}

void geBlitImageDepth(int x, int y, int z, ge_Image* img, int sx, int sy, int width, int height, int flags){
	if(!img)return;
	geBlitImageDepthStretched(x, y, z, img, sx, sy, width, height, width, height, flags);
}

void geDrawImage3D(int x, int y, float z, ge_Image3D* img){
	if(!img)return;
//	geBlitImage3D(x, y, z, img, 0, 0, img->width, img->height, true);
}

void drawLine(int x0, int y0, int x1, int y1, u32 color, u32* destination, int width, int w, int h){
	short int dy = y1 - y0;
	short int dx = x1 - x0;
	short int stepx, stepy;
	
	if (dy < 0) { dy = -dy;  stepy = -width; } else { stepy = width; }
	if (dx < 0) { dx = -dx;  stepx = -1; } else { stepx = 1; }
	dy <<= 1;
	dx <<= 1;
	
	y0 *= width;
	y1 *= width;
	if((x0 >= 0) && (x0 <= w) && ((y0/width) >= 0) && ((y0/width) <= h)){
		destination[x0+y0] = color;
	}
	int fraction;
	if (dx > dy) {
		fraction = dy - (dx >> 1);
		while (x0 != x1) {
			if (fraction >= 0) {
				y0 += stepy;
				fraction -= dx;
			}
			x0 += stepx;
			fraction += dy;
			if((x0>=0) && (x0<w) && ((y0/width)>=0) && ((y0/width)<=h)){
				destination[x0+y0] = color;
			}else if(x1 < 0){
				break;
			}
		}
	} else {
		fraction = dx - (dy >> 1);
		while (y0 != y1) {
			if (fraction >= 0) {
				x0 += stepx;
				fraction -= dy;
			}
			y0 += stepy;
			fraction += dx;
			if((x0>=0) && (x0<w) && ((y0/width)>=0) && ((y0/width)<=h)){
				destination[x0+y0] = color;
			}else if(y1 < 0){
				break;
			}
		}
	}
}

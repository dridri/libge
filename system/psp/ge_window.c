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

static ge_Image* loadRawCursor();
static int SetupCallbacks(void);
static void (*setup_system)() = NULL;

#ifndef abs
#define abs(x) (((x) < 0) ? (-(x)) : (x))
#endif

int geCreateMainWindow(const char* title, int _width, int _height, int flags){
	LibGE_PspContext* context = (LibGE_PspContext*)geMalloc(sizeof(LibGE_PspContext));
	libge_context->syscontext = (u32)context;

	int width = 480;
	int height = 272;

	getcwd(context->cwd, 2048);

/*
	int width = _width>480?480:_width;
	int height = _height>272?272:_height;
*/
	libge_context->width = width;
	libge_context->height = height;

	gePrintDebug(0x100, "geInit 1\n");

	sceDisplaySetMode(0, width, height);
	sceDisplaySetFrameBuf((void*)(0x40000000|(u32)sceGeEdramGetAddr()), geGetNextPower2(width), PSP_DISPLAY_PIXEL_FORMAT_8888, 1);
	
	gePrintDebug(0x100, "geInit 2\n");
	//Graphics
	geCreateContext();
	geGraphicsInit();
	//Controls
	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
	gePrintDebug(0x100, "geInit 3\n");

	void* drawBuffer = valloc(sizeof(u32)*geGetNextPower2(width)*height);
	void* dispBuffer = valloc(sizeof(u32)*geGetNextPower2(width)*height);
	void* depthBuffer = valloc(sizeof(u16)*geGetNextPower2(width)*height);
	geDrawBuffer(GE_PSM_8888, (void*)vrelptr(drawBuffer), geGetNextPower2(width));
	geDisplayBuffer((void*)vrelptr(dispBuffer), width, height);
	geDepthBuffer((void*)vrelptr(depthBuffer), geGetNextPower2(width));
	gePrintDebug(0x100, "geInit 4\n");

	libge_context->gpumem += 2.5 * sizeof(u32)*geGetNextPower2(width)*height;

	geOffset(2048 - (width/2), 2048 - (height/2));
	geViewport(2048, 2048, width, height);
	geDepthRange(65535, 0);
//	geDepthRange(0, 65535);
//	_sw(0, 0);
	geScissor(0, 0, width, height);

	//User setup ?
	if(!setup_system){
		setup_system = (void(*)())SetupCallbacks;
	}
	if(setup_system)setup_system();
	gePrintDebug(0x100, "geInit 5\n");

	geMakeClearList();

	//Enable depth-testing
	geDepthFunc(GE_GEQUAL);
	geEnable(GE_DEPTH_TEST);
	//Set shade model to better
	geShadeModel(GE_SMOOTH);
	//Enable and set face-culling
	geFrontFace(GE_CW);
	geEnable(GE_CULL_FACE);
	//Enable clip planes
	geEnable(GE_CLIP_PLANES);
	//Set ambient to white
	geAmbientColor(0xffffffff);
	gePrintDebug(0x100, "geInit 6\n");

	//Set texture mode to 32 bits
	geSendCommandi(CMD_TEX_PIXEL_MODE, GE_PSM_8888);
	//Set pixmaps to 0 and enable swizzled textures
	geSendCommandi(CMD_TEX_MIPMAP_DRAW_MODE, (0 << 16) | (0 << 8) | (true));
	//Set tex func and tex filter for 2D
	geSendCommandi(CMD_TEX_FUNC, ((GE_TCC_RGBA << 8)|GE_TFX_MODULATE)|0);
	geSendCommandi(CMD_TEX_FILTER, (GE_LINEAR << 8)|GE_LINEAR);
	gePrintDebug(0x100, "geInit 7\n");

	//Set alpha and blend funcs to default
	geSendCommandi(CMD_ALPHA_FUNC, (GE_GREATER | ((0 & 0xff) << 8) | ((0xff & 0xff) << 16)));
	geSendCommandi(CMD_BLEND_FUNC, GE_SRC_ALPHA | (GE_ONE_MINUS_SRC_ALPHA << 4) | (GE_ADD << 8));
	geSendCommandi(CMD_BLEND_SRCFIX, 0 & 0xffffff);
	geSendCommandi(CMD_BLEND_DSTFIX, 0 & 0xffffff);
	gePrintDebug(0x100, "geInit 8\n");


	//Setup default 3D
	geDrawingMode(GE_DRAWING_MODE_3D);
	geMatrixMode(GE_MATRIX_PROJECTION);
	geLoadIdentity();
	gePerspective(45, (float)16.0/9.0, 0.01, 100);
	geMatrixMode(GE_MATRIX_VIEW);
	geLoadIdentity();
	geMatrixMode(GE_MATRIX_MODEL);
	geLoadIdentity();
	gePrintDebug(0x100, "geInit 9\n");
	geUpdateMatrix();
	gePrintDebug(0x100, "geInit 10\n");

	//Set default mode to 2D
	geDrawingMode(GE_DRAWING_MODE_2D);
	geDrawSync();
	gePrintDebug(0x100, "geInit 11\n");

	geClearMode(GE_CLEAR_COLOR_BUFFER);
	
	context->cursor = loadRawCursor();
	gePrintDebug(0x100, "geInit 12\n");
	return 0;
}

void geWaitVsync(int enabled){
	libge_context->vsync = enabled;
}

bool mouse_visible = true;
bool mouse_round = false;
short mouse_last_x=0, mouse_last_y=0;
short mouse_warp_x=0, mouse_warp_y=0;

void geKeyMoveCursor(int x, int y){
	x -= 128;
	y -= 128;
	// Avoid joypad bugs
	if(abs(x) < 50 && abs(y) < 50){
		x = 0;
		y = 0;
	}
	if(abs(x) < 50){
		x = 0;
	}
	if(abs(y) < 50){
		y = 0;
	}
	
	mouse_last_x = libge_context->mouse_x;
	mouse_last_y = libge_context->mouse_y;
	libge_context->mouse_x += x / 20;
	libge_context->mouse_y += y / 20;

	if(mouse_round){
		if(libge_context->mouse_x <= 10){
			libge_context->mouse_x = libge_context->width-15;
			mouse_last_x += libge_context->width-25;
		}
		if(libge_context->mouse_x >= libge_context->width-10){
			libge_context->mouse_x = 15;
			mouse_last_x -= libge_context->width-25;
		}
		if(libge_context->mouse_y <= 10){
			libge_context->mouse_y = libge_context->height-15;
			mouse_last_y += libge_context->height-25;
		}
		if(libge_context->mouse_y >= libge_context->height-10){
			libge_context->mouse_y = 15;
			mouse_last_y -= libge_context->height-25;
		}
	}else{
		if(libge_context->mouse_x < 0){
			libge_context->mouse_x = 0;
		}
		if(libge_context->mouse_x > libge_context->width){
			libge_context->mouse_x = libge_context->width;
		}
		if(libge_context->mouse_y < 0){
			libge_context->mouse_y = 0;
		}
		if(libge_context->mouse_y > libge_context->height){
			libge_context->mouse_y = libge_context->height;
		}
	}

	mouse_warp_x = libge_context->mouse_x-mouse_last_x;
	mouse_warp_y = libge_context->mouse_y-mouse_last_y;
}

void geCursorPosition(int* x, int* y){
	*x = libge_context->mouse_x;
	*y = libge_context->mouse_y;
}

void geCursorWarp(int* x, int* y){
	*x = mouse_warp_x;
	*y = mouse_warp_y;
}

void geCursorRoundMode(bool active){
	mouse_round = active;
}

void geCursorVisible(bool visible){
	mouse_visible = visible;
}


static int Exp(int val){
	u32 i;
	asm("clz %0, %1\n" : "=r"(i):"r"(val&0x3FF));
	return 31-i;
}
typedef struct TextureVertex {
	short u, v;
	u32 color;
	short x, y, z;
} TextureVertex;
void geCursorRender(){
	if(!mouse_visible || libge_context->cursor_image)return;

	if(libge_context->drawing_mode & GE_DRAWING_2D_DEPTH){
		geSendCommandi(CMD_DEPTH_MASK, 1);
		geSendCommandi(GE_DEPTH_TEST, 0);
	}

	ge_Image* img = ((LibGE_PspContext*)libge_context->syscontext)->cursor;
	int width = img->width;
	int height = img->height;
	
	sceKernelDcacheWritebackInvalidateAll();
	
	geSendCommandi(GE_ALPHA_TEST, true);
	geSendCommandi(GE_BLEND, true);
	geSendCommandi(CMD_TEX_MIPMAP_DRAW_MODE, (0 << 16) | (0 << 8) | (img->flags&GE_IMAGE_SWIZZLED));

	//TexImage
	geSendCommandi(CMD_TEX_MIPMAP0, ((u32)img->data) & 0xffffff);
	geSendCommandi(CMD_TEX_STRIDE0, ((((u32)img->data) >> 8) & 0x0f0000)|img->textureWidth);
	geSendCommandi(CMD_TEX_SIZE0, (Exp(geGetNextPower2(img->height)) << 8) | (Exp(img->textureWidth)));

	//TexScale
	geSendCommandf(CMD_TEX_SCALE_U, 1.0f / ((float)img->textureWidth));
	geSendCommandf(CMD_TEX_SCALE_V, 1.0f / ((float)img->textureHeight));

	geSendCommandf(CMD_TEX_FLUSH, 0.0f);

	int j = 0;
	while (j < width) {
		TextureVertex* vertices = (TextureVertex*)geGetMemory(2 * sizeof(TextureVertex));
		int sliceWidth = 64;
		if (j + sliceWidth > width) sliceWidth = width - j;
		vertices[0].u = 0 + j;
		vertices[0].v = 0;
		vertices[0].x = libge_context->mouse_x + j;
		vertices[0].y = libge_context->mouse_y;
		vertices[1].u = 0 + (j+sliceWidth);
		vertices[1].v = 0 + height;
		vertices[1].x = libge_context->mouse_x + j + sliceWidth;
		vertices[1].y = libge_context->mouse_y + height;
		vertices[0].color = vertices[1].color = img->color;
		geSendCommandi(CMD_DRAW_MODE, GE_COLOR_8888 | GE_TEXTURE_16BIT | GE_VERTEX_16BIT | GE_TRANSFORM_2D);
		geSendCommandi(CMD_VERTICES_POINTER, ((unsigned int)vertices) & 0xffffff);
		geSendCommandi(CMD_DRAW_TYPE_VCOUNT, (GE_SPRITES << 16)|2);
		j += sliceWidth;
	}

	if(libge_context->drawing_mode & GE_DRAWING_2D_DEPTH){
		geSendCommandi(CMD_DEPTH_MASK, 0);
		geSendCommandi(GE_DEPTH_TEST, 1);
	}

}

static unsigned char _ge_default_cursor[] = {
	11, 19,
	3,
	'^', 000, 000, 000, 000,
	'0', 255, 255, 255, 255,
	'#', 000, 000, 000, 255,
	'#','^','^','^','^','^','^','^','^','^','^',
	'#','#','^','^','^','^','^','^','^','^','^',
	'#','0','#','^','^','^','^','^','^','^','^',
	'#','0','0','#','^','^','^','^','^','^','^',
	'#','0','0','0','#','^','^','^','^','^','^',
	'#','0','0','0','0','#','^','^','^','^','^',
	'#','0','0','0','0','0','#','^','^','^','^',
	'#','0','0','0','0','0','0','#','^','^','^',
	'#','0','0','0','0','0','0','0','#','^','^',
	'#','0','0','0','0','0','0','0','0','#','^',
	'#','0','0','0','0','0','#','#','#','#','#',
	'#','0','0','#','0','0','#','^','^','^','^',
	'#','0','#','^','#','0','0','#','^','^','^',
	'#','#','^','^','#','0','0','#','^','^','^',
	'#','^','^','^','^','#','0','0','#','^','^',
	'^','^','^','^','^','#','0','0','#','^','^',
	'^','^','^','^','^','^','#','0','0','#','^',
	'^','^','^','^','^','^','#','0','0','#','^',
	'^','^','^','^','^','^','^','#','#','^','^'
};

static ge_Image* loadRawCursor(){
	gePrintDebug(0x100, "loadRawCursor...");
	int i, j, c;
	unsigned char* data = _ge_default_cursor;
	ge_Image* surface = geCreateSurface(data[0], data[1], 0x000000000);
	surface->flags &= ~GE_IMAGE_SWIZZLED;
	for(j=0; j<data[1]; j++){
		for(i=0; i<data[0]; i++){
			int pixel_index = i+j*data[0] +3 +5*data[2];
			for(c=0; c<data[2]; c++){
				if(data[3+ c*5] == data[pixel_index]){
					break;
				}
			}
			c = 3+ c*5 +1;
			u32 color = RGBA(data[c], data[c+1], data[c+2], data[c+3]);
			surface->data[i+j*surface->textureWidth] = color;
		}
	}

	geUpdateImage(surface);
	gePrintDebug(0x100, "Ok\n");
	return surface;
}


static int exit_callback(int arg1, int arg2, void *common){
	sceKernelExitGame();
	return 0;
}
static int CallbackThread(SceSize args, void *argp){
	int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);
	sceKernelSleepThreadCB();
	return 0;
}
static int SetupCallbacks(void){
	int thid = 0;
	thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
	if(thid >= 0){
		sceKernelStartThread(thid, 0, 0);
	}
	return thid;
}

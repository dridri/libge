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

static unsigned int __attribute__((aligned(16))) ge_init_list[] = {
	0x01000000, 0x02000000, 0x10000000, 0x12000000, 0x13000000, 0x15000000, 0x16000000, 0x17000000,
	0x18000000, 0x19000000, 0x1a000000, 0x1b000000, 0x1c000000, 0x1d000000, 0x1e000000, 0x1f000000,
	0x20000000, 0x21000000, 0x22000000, 0x23000000, 0x24000000, 0x25000000, 0x26000000, 0x27000000,
	0x28000000, 0x2a000000, 0x2b000000, 0x2c000000, 0x2d000000, 0x2e000000, 0x2f000000, 0x30000000,
	0x31000000, 0x32000000, 0x33000000, 0x36000000, 0x37000000, 0x38000000, 0x3a000000, 0x3b000000,
	0x3c000000, 0x3d000000, 0x3e000000, 0x3f000000, 0x40000000, 0x41000000, 0x42000000, 0x43000000,
	0x44000000, 0x45000000, 0x46000000, 0x47000000, 0x48000000, 0x49000000, 0x4a000000, 0x4b000000,
	0x4c000000, 0x4d000000, 0x50000000, 0x51000000, 0x53000000, 0x54000000, 0x55000000, 0x56000000,
	0x57000000, 0x58000000, 0x5b000000, 0x5c000000, 0x5d000000, 0x5e000000, 0x5f000000, 0x60000000,
	0x61000000, 0x62000000, 0x63000000, 0x64000000, 0x65000000, 0x66000000, 0x67000000, 0x68000000,
	0x69000000, 0x6a000000, 0x6b000000, 0x6c000000, 0x6d000000, 0x6e000000, 0x6f000000, 0x70000000,
	0x71000000, 0x72000000, 0x73000000, 0x74000000, 0x75000000, 0x76000000, 0x77000000, 0x78000000,
	0x79000000, 0x7a000000, 0x7b000000, 0x7c000000, 0x7d000000, 0x7e000000, 0x7f000000, 0x80000000,
	0x81000000, 0x82000000, 0x83000000, 0x84000000, 0x85000000, 0x86000000, 0x87000000, 0x88000000,
	0x89000000, 0x8a000000, 0x8b000000, 0x8c000000, 0x8d000000, 0x8e000000, 0x8f000000, 0x90000000,
	0x91000000, 0x92000000, 0x93000000, 0x94000000, 0x95000000, 0x96000000, 0x97000000, 0x98000000,
	0x99000000, 0x9a000000, 0x9b000000, 0x9c000000, 0x9d000000, 0x9e000000, 0x9f000000, 0xa0000000,
	0xa1000000, 0xa2000000, 0xa3000000, 0xa4000000, 0xa5000000, 0xa6000000, 0xa7000000, 0xa8040004,
	0xa9000000, 0xaa000000, 0xab000000, 0xac000000, 0xad000000, 0xae000000, 0xaf000000, 0xb0000000,
	0xb1000000, 0xb2000000, 0xb3000000, 0xb4000000, 0xb5000000, 0xb8000101, 0xb9000000, 0xba000000,
	0xbb000000, 0xbc000000, 0xbd000000, 0xbe000000, 0xbf000000, 0xc0000000, 0xc1000000, 0xc2000000,
	0xc3000000, 0xc4000000, 0xc5000000, 0xc6000000, 0xc7000000, 0xc8000000, 0xc9000000, 0xca000000,
	0xcb000000, 0xcc000000, 0xcd000000, 0xce000000, 0xcf000000, 0xd0000000, 0xd2000000, 0xd3000000,
	0xd4000000, 0xd5000000, 0xd6000000, 0xd7000000, 0xd8000000, 0xd9000000, 0xda000000, 0xdb000000,
	0xdc000000, 0xdd000000, 0xde000000, 0xdf000000, 0xe0000000, 0xe1000000, 0xe2000000, 0xe3000000,
	0xe4000000, 0xe5000000, 0xe6000000, 0xe7000000, 0xe8000000, 0xe9000000, 0xeb000000, 0xec000000,
	0xee000000, 0xf0000000, 0xf1000000, 0xf2000000, 0xf3000000, 0xf4000000, 0xf5000000, 0xf6000000,
	0xf7000000, 0xf8000000, 0xf9000000,
	0x0f000000, 0x0c000000, 0, 0
};

static LibGE_PspContext* context = NULL;
static int _ge_caps_list[64] = { 0 };
static int _ge_max_cmd = 65536; //262144;

static int draw_mode = 0;
static int draw_type = GE_TRIANGLES;
static int draw_count = 0;
static void* _ge_vertices_pointer = NULL;
static int _ge_vertex_size = 0;

struct {
	int used;
	int cap;
	int state;
} _ge_force_caps[32] = { false, 0, 0 };

inline void geSendCommandi(int cmd, int args){
	*(context->dList.current++) = (cmd << 24) | (args & 0xffffff);
}

inline void geSendCommandf(int cmd, float args){
	union {
		float f;
		unsigned int i;
	} t;
	t.f = args;
	*(context->dList.current++) = (cmd << 24) | ((t.i>>8) & 0xffffff);
}

inline void geListSendCommandi(LibGE_DisplayList* dList, int cmd, int args){
	*(dList->current++) = (cmd << 24) | (args & 0xffffff);
	gePrintDebug(0x100, "geListSendCommandi(0x%08X, %d, 0x%08X) => 0x%08X\n", (u32)dList, cmd, args, (cmd << 24) | (args & 0xffffff));
}

inline void geListSendCommandf(LibGE_DisplayList* dList, int cmd, float args){
	union {
		float f;
		unsigned int i;
	} t;
	t.f = args;
	*(dList->current++) = (cmd << 24) | ((t.i>>8) & 0xffffff);
}

void geCreateContext(){
	// initialize graphics hardware
	context = (LibGE_PspContext*)libge_context->syscontext;
	context->ge_list_executed[0] = sceGeListEnQueue((void*)((unsigned int)ge_init_list & 0x1fffffff), 0, 0, 0);
	sceGeListSync(context->ge_list_executed[0], 0);
}

void geGraphicsInit(){
	context->ge_edram_address = (0x40000000|(u32)sceGeEdramGetAddr());

	u32* DisplayList = (u32*)geMemalign(sizeof(u32)*_ge_max_cmd, 16);
	gePrintDebug(0x100, "geGraphicsInit DisplayList: 0x%08X\n", DisplayList);
	context->dList.start = (u32*)((u32)DisplayList | 0x40000000);
	context->dList.current = context->dList.start;
	context->dList.last = context->dList.current;
	context->dList.defaultList = context->dList.start;
	libge_context->clear_color = 0x00000000;
	context->vfpu_context = (struct pspvfpu_context*)pspvfpu_initcontext();
	libge_context->ram_extended = false;

	static int dither_matrix[16] = {
		-4, 0,-3, 1,
		 2,-2, 3,-1,
		-3, 1,-4, 0,
		 3,-1, 2,-2
	};
	geSetDither((ScePspIMatrix4*)dither_matrix);
	geSendCommandi(CMD_PATH_DIVIDE, (16 << 8)|16);
	geSendCommandi(CMD_MATERIAL, GE_AMBIENT|GE_DIFFUSE|GE_SPECULAR);
	geSendCommandf(CMD_SPECULAR, 1.0f);
	geSendCommandf(CMD_TEX_SCALE_U, 1.0f);
	geSendCommandf(CMD_TEX_SCALE_V, 1.0f);
}

int geDrawingMode(int mode){
	int last = libge_context->drawing_mode;
	libge_context->drawing_mode = mode;
	if(mode & GE_DRAWING_MODE_2D){
		geDisable(GE_LIGHTING);
		geEnable(GE_TEXTURE_2D);
		geEnable(GE_BLEND);
		geEnable(GE_ALPHA_TEST);
		geDrawMode(GE_COLOR_8888 | GE_TEXTURE_16BIT | GE_VERTEX_16BIT | GE_TRANSFORM_2D);
		geSendCommandi(CMD_TEX_FILTER, (GE_LINEAR << 8)|GE_LINEAR);

		geMatrixMode(GE_MATRIX_PROJECTION);
		geLoadIdentity();
		geOrthogonal(0.0, libge_context->width, libge_context->height, 0.0, -2048.0, 2048.0);
		geMatrixMode(GE_MATRIX_VIEW);
		geLoadIdentity();
		geMatrixMode(GE_MATRIX_MODEL);
		geLoadIdentity();
		geMatrixMode(GE_MATRIX_TEXTURE);
		geLoadIdentity();
		geUpdateMatrix();

		if(mode & GE_DRAWING_2D_DEPTH){
			geDisable(CMD_DEPTH_MASK);
			geEnable(GE_DEPTH_TEST);
			geClearMode(GE_CLEAR_COLOR_BUFFER | GE_CLEAR_DEPTH_BUFFER);
		}else{
			geEnable(CMD_DEPTH_MASK);
			geDisable(GE_DEPTH_TEST);
			geClearMode(GE_CLEAR_COLOR_BUFFER);
		}
	}else
	if(mode & GE_DRAWING_MODE_3D){
		geEnable(GE_DEPTH_TEST);
		geDepthMask(true);
		geDisable(GE_TEXTURE_2D);
		geDisable(GE_BLEND);
		geDisable(GE_ALPHA_TEST);
		geDisable(GE_LIGHTING);

		geClearMode(GE_CLEAR_COLOR_BUFFER | GE_CLEAR_DEPTH_BUFFER);
		geDrawMode(GE_TEXTURE_32BITF | GE_COLOR_8888 | GE_NORMAL_32BITF | GE_VERTEX_32BITF | GE_TRANSFORM_3D);
		geSendCommandi(CMD_TEX_FILTER, (GE_NEAREST << 8) | GE_NEAREST);

		geMatrixMode(GE_MATRIX_VIEW);
		geLoadIdentity();
		geMatrixMode(GE_MATRIX_MODEL);
		geLoadIdentity();
		geMatrixMode(GE_MATRIX_TEXTURE);
		geLoadIdentity();
		geUpdateMatrix();
		geMatrixMode(GE_MATRIX_PROJECTION);
		geLoadIdentity();
		if(libge_context->projection_matrix[0] == (float)0xFFFFFFFF){
			gePerspective(60.0, (float)libge_context->width / (float)libge_context->height, 1.0, 500.0);
		}else{
			geLoadMatrix(libge_context->projection_matrix);
		}
	}
	return last;
}

void geCallList(LibGE_DisplayList* list){
	geSendCommandi(CMD_BASE_ADDR, (((u32)list->start) >> 8) & 0xf0000);
	geSendCommandi(CMD_CALL, ((u32)list->start) & 0xffffff);
}

int geEnable(int what){
	int ret = _ge_caps_list[what];
	_ge_caps_list[what] = 1;
	geSendCommandi(what, 1);
	return ret;
}

int geDisable(int what){
	int ret = _ge_caps_list[what];
	_ge_caps_list[what] = 0;
	geSendCommandi(what, 0);
	return ret;
}

void geForceCap(int which, int en){
	int i;
	for(i=0; i<32; i++){
		if(!_ge_force_caps[i].used || _ge_force_caps[i].cap == which){
			if(en == -1){
				_ge_force_caps[i].used = false;
				_ge_force_caps[i].cap = 0;
				_ge_force_caps[i].state = 0;
			}else{
				_ge_force_caps[i].used = true;
				_ge_force_caps[i].cap = which;
				_ge_force_caps[i].state = en;
			}
			break;
		}
	}
}

void geDrawBuffer(int psm, void* pointer, int frame_width){
	context->draw_buffer.pixel_size = psm;
	context->draw_buffer.frame_width = frame_width;
	context->draw_buffer.frame_buffer = (u32*)pointer;

	geSendCommandi(CMD_PIXEL_MODE, psm);
	geSendCommandi(CMD_FRAME_BUFFER, ((u32)context->draw_buffer.frame_buffer) & 0xffffff);
	geSendCommandi(CMD_FRAME_BUFFER_WIDTH, ((((u32)context->draw_buffer.frame_buffer) & 0xff000000) >> 8)|context->draw_buffer.frame_width);
	geSendCommandi(CMD_DEPTH_BUFFER, ((u32)context->draw_buffer.depth_buffer) & 0xffffff);
	geSendCommandi(CMD_DEPTH_BUFFER_WIDTH, ((((u32)context->draw_buffer.depth_buffer) & 0xff000000) >> 8)|context->draw_buffer.depth_width);
}

void geDisplayBuffer(void* pointer, int width, int height){
	context->draw_buffer.width = width;
	context->draw_buffer.height = height;
	context->draw_buffer.disp_buffer = (u32*)pointer;

	geSendCommandi(CMD_DRAW_REGION_START_POINT, (0 << 10) | 0);
	geSendCommandi(CMD_DRAW_REGION_END_POINT, ((height-1) << 10) | (width-1));
	sceDisplaySetMode(0, width, height);

	sceDisplaySetFrameBuf((void*)(((u32)context->ge_edram_address) + ((u32)context->draw_buffer.disp_buffer)), context->draw_buffer.frame_width, PSP_DISPLAY_PIXEL_FORMAT_8888, PSP_DISPLAY_SETBUF_IMMEDIATE);
	pspDebugScreenSetOffset((int)context->draw_buffer.disp_buffer);
}

void geDepthBuffer(void* pointer, int frame_width){
	geSendCommandi(CMD_DEPTH_BUFFER, ((u32)pointer) & 0xffffff);
	geSendCommandi(CMD_DEPTH_BUFFER_WIDTH, ((((u32)pointer) & 0xff000000) >> 8)|frame_width);
	context->draw_buffer.depth_buffer = (u32*)pointer;
	context->draw_buffer.depth_width = frame_width;
}

void geOffset(u32 x, u32 y){
	geSendCommandi(CMD_OFFSET_X, x << 4);
	geSendCommandi(CMD_OFFSET_Y, y << 4);
}

void geViewport(int x, int y, int width, int height){
	geSendCommandf(CMD_VIEWPORT_WIDTH, (float)(width>>1));
	geSendCommandf(CMD_VIEWPORT_HEIGHT, (float)((-height)>>1));
	geSendCommandf(CMD_VIEWPORT_X, (float)x);
	geSendCommandf(CMD_VIEWPORT_Y, (float)y);
}

void geScissor(int x, int y, int ex, int ey){
	geSendCommandi(CMD_SCISSOR_START_POINT, (y << 10) | x);
	geSendCommandi(CMD_SCISSOR_END_POINT, ((ey-1) << 10) | (ex-1));
}

void geSetDither(const ScePspIMatrix4* matrix){
	geSendCommandi(CMD_DITHER_MATRIX_LINE0, (matrix->x.x & 0x0f)|((matrix->x.y & 0x0f) << 4)|((matrix->x.z & 0x0f) << 8)|((matrix->x.w & 0x0f) << 12));
	geSendCommandi(CMD_DITHER_MATRIX_LINE1, (matrix->y.x & 0x0f)|((matrix->y.y & 0x0f) << 4)|((matrix->y.z & 0x0f) << 8)|((matrix->y.w & 0x0f) << 12));
	geSendCommandi(CMD_DITHER_MATRIX_LINE2, (matrix->z.x & 0x0f)|((matrix->z.y & 0x0f) << 4)|((matrix->z.z & 0x0f) << 8)|((matrix->z.w & 0x0f) << 12));
	geSendCommandi(CMD_DITHER_MATRIX_LINE3, (matrix->w.x & 0x0f)|((matrix->w.y & 0x0f) << 4)|((matrix->w.z & 0x0f) << 8)|((matrix->w.w & 0x0f) << 12));
}

void geAmbientColor(u32 color){
	geSendCommandi(CMD_AMBIENT_COLOR, color & 0xffffff);
	geSendCommandi(CMD_AMBIENT_COLOR_2, color >> 24);
}

void geMaterial(int mode, int color){
	if(mode & 0x01){
		geSendCommandi(85, color & 0xffffff);
		geSendCommandi(88, color >> 24);
	}

	if(mode & 0x02){
		geSendCommandi(86, color & 0xffffff);
	}
	if(mode & 0x04){
		geSendCommandi(87, color & 0xffffff);
	}
}

void geFrontFace(int order){
	geSendCommandi(155, order);
}

void geShadeModel(int mode){
	geSendCommandi(80, mode);
}

void geDepthRange(int near, int far){
	unsigned int max = (unsigned int)near + (unsigned int)far;
	int val = (int)((max >> 31) + max);
	float z = (float)(val >> 1);
	geSendCommandf(68, z - ((float)near));
	geSendCommandf(71, z);
	gePrintDebug(0x100, "  max : %d\n", max);
	gePrintDebug(0x100, "  val : %d\n", val);
	gePrintDebug(0x100, "  z : %f\n", z);
	gePrintDebug(0x100, "  z - near : %f\n", z - ((float)near));
	gePrintDebug(0x100, "\n");
	if (near > far){
		int temp = near;
		near = far;
		far = temp;
	}

	geSendCommandi(214, near);
	geSendCommandi(215, far);
}

void geDepthFunc(int func){
	geSendCommandi(222, func);
}

void geDepthMask(int en){
	geSendCommandi(CMD_DEPTH_MASK, !en);
}

void geBlendFunc(int src, int dst){
	geSendCommandi(CMD_BLEND_FUNC, src | (dst << 4) | (GE_ADD << 8));
}

#define clear_list_len (sizeof(LibGE_ClearVertex)/4*8*2+2 + 6 + 4 + 1)
void geMakeClearList(){
	LibGE_DisplayList* clear_list = (LibGE_DisplayList*)geMalloc(sizeof(LibGE_DisplayList));
	clear_list->start = (u32*)geMemalign(clear_list_len*4, 16);
	clear_list->current = clear_list->start;
	clear_list->last = clear_list->start+clear_list_len*4;
	memset(clear_list->start, 0, clear_list_len*4);

	LibGE_ClearVertex* curr;
	u32 i = 0;
//	int count = ((libge_context->draw_buffer.width+63)/64)*2;
	int count = 8 * 2;
	LibGE_ClearVertex* clear_vertices = (LibGE_ClearVertex*)geListGetMemory(clear_list, count * sizeof(LibGE_ClearVertex));
	curr = clear_vertices;
	for (i = 0; i < count; ++i, ++curr){
		u32 j = i >> 1;
		u32 k = (i & 1);
		curr->color = libge_context->clear_color;
		curr->x = (j+k) * 64;
		curr->y = k * context->draw_buffer.height;
		curr->z = 0xFFFF;
		curr->pad = 0xEEEE;
		// with z = 0xFFFF;
		// geDepthRange(0, 65535);
		// geDepthFunc(GE_LEQUAL);
	}

	geListSendCommandi(clear_list, CMD_CLEAR_BITS, (libge_context->clear_mode << 8) | 1);
	geListSendCommandi(clear_list, CMD_DRAW_MODE, GE_COLOR_8888|GE_VERTEX_16BIT|GE_TRANSFORM_2D);
	geListSendCommandi(clear_list, CMD_DRAW_VERTICES_POINTER, (((unsigned int)clear_vertices) >> 8) & 0xf0000);
	geListSendCommandi(clear_list, CMD_VERTICES_POINTER, ((unsigned int)clear_vertices) & 0xffffff);
	geListSendCommandi(clear_list, CMD_DRAW_TYPE_VCOUNT, (GE_SPRITES << 16)|count);
	geListSendCommandi(clear_list, CMD_CLEAR_BITS, 0);

	geListSendCommandi(clear_list, CMD_RETURN, 0);

	context->clear_list = clear_list;
	context->clear_vertices = clear_vertices;
}

void geClearColor(u32 color){
	libge_context->clear_color = color;
	LibGE_ClearVertex* curr = context->clear_vertices;
	int i;
	for(i = 0; i < (8*2); i++, curr++){
		curr->color = color;
	}
}

u32 geClearMode(u32 mode){
	u32 last = libge_context->clear_mode;
	libge_context->clear_mode = mode;
	context->clear_list->start[50] = (CMD_CLEAR_BITS << 24) | (((mode << 8) | 1) & 0xffffff);
	return last;
}

void geClearScreen(){
//	geCallList(context->clear_list);
	if(libge_context->drawing_mode & GE_DRAWING_2D_DEPTH){
		memset(libge_context->img_stack, 0x0, sizeof(float)*4098);
	}
	
	LibGE_ClearVertex* curr;
	u32 i = 0;
	int count = ((context->draw_buffer.width+63)/64)*2;
	LibGE_ClearVertex* vertices = (LibGE_ClearVertex*)geGetMemory(count * sizeof(LibGE_ClearVertex));
	curr = vertices;
	for (i = 0; i < count; ++i, ++curr){
		u32 j = i >> 1;
		u32 k = (i & 1);
		curr->color = libge_context->clear_color;
		curr->x = (j+k) * 64;
		curr->y = k * context->draw_buffer.height;
		curr->z = 0;
		curr->pad = 0;
	}

	geSendCommandi(CMD_CLEAR_BITS, (libge_context->clear_mode << 8) | 1);
	geDrawArrayFull(GE_SPRITES, GE_COLOR_8888|GE_VERTEX_16BIT|GE_TRANSFORM_2D, vertices, count);
	geSendCommandi(CMD_CLEAR_BITS, 0);

	geDrawSync();
}

void geDrawSync(){
//	gePrintDebug(0x100, "geDrawSync 1\n");
	*(context->dList.current++) = 0x0f000000;
	*(context->dList.current++) = 0x0c000000;
	*(context->dList.current++) = 0x00000000;
	*(context->dList.current++) = 0x00000000;
//	gePrintDebug(0x100, "geDrawSync 2\n");

	sceKernelDcacheWritebackInvalidateAll();
	asm("sync\n");

	context->ge_list_executed[0] = sceGeListEnQueue(context->dList.start, context->dList.current, 0, NULL);
	asm("sync\n");
//	gePrintDebug(0x100, "geDrawSync 3\n");
	sceGeListSync(context->ge_list_executed[0], PSP_GE_LIST_DONE);
	asm("sync\n");
//	gePrintDebug(0x100, "geDrawSync 4\n");
	sceGeDrawSync(PSP_GE_LIST_DRAWING_DONE);
	asm("sync\n");
//	gePrintDebug(0x100, "geDrawSync 5\n");

	context->dList.current = context->dList.start;
//	gePrintDebug(0x100, "geDrawSync 6\n");
}

static u32 last_time = 0;
static int draw_buffer = 0;
int SystemSwapBuffers(){
	u32 current_time = geGetTick();
	if((current_time - last_time) >= 16){
		last_time = current_time;
		SceCtrlData pad;
		sceCtrlPeekBufferPositive(&pad, 1);
		geKeyMoveCursor(pad.Lx, pad.Ly);
	}

	geCursorRender();

	void* temp = context->draw_buffer.disp_buffer;
	context->draw_buffer.disp_buffer = context->draw_buffer.frame_buffer;
	context->draw_buffer.frame_buffer = (u32*)temp;

	geSendCommandi(CMD_FRAME_BUFFER, ((u32)context->draw_buffer.frame_buffer) & 0xffffff);
	geDrawSync();
	
	if(libge_context->vsync){
		sceDisplayWaitVblankStart();
	}

	sceDisplaySetFrameBuf((void*)((u32)context->ge_edram_address + (u32)context->draw_buffer.disp_buffer), context->draw_buffer.frame_width, PSP_DISPLAY_PIXEL_FORMAT_8888, PSP_DISPLAY_SETBUF_IMMEDIATE);
	sceKernelDelayThread(1); // 0

	return (draw_buffer=!draw_buffer);
}

void geDrawType(int type){
	draw_type = type;
}

void geDrawMode(int mode){
	geSendCommandi(CMD_DRAW_MODE, mode);
	draw_mode = mode;
	_ge_vertex_size = 0;
	if(mode & GE_COLOR_8888){
		_ge_vertex_size += 4;
	}else
	if((mode & GE_COLOR_4444) || (mode & GE_COLOR_5551) || (mode & GE_COLOR_5650)){
		_ge_vertex_size += 2;
	}
	if(mode & GE_TEXTURE_32BITF){
		_ge_vertex_size += 4 * 2;
	}else
	if(mode & GE_TEXTURE_16BIT){
		_ge_vertex_size += 2 * 2;
	}else
	if(mode & GE_TEXTURE_8BIT){
		_ge_vertex_size += 1 * 2;
	}
	if(mode & GE_NORMAL_32BITF){
		_ge_vertex_size += 4 * 3;
	}else
	if(mode & GE_NORMAL_16BIT){
		_ge_vertex_size += 2 * 3;
	}else
	if(mode & GE_NORMAL_8BIT){
		_ge_vertex_size += 1 * 3;
	}
	if(mode & GE_VERTEX_32BITF){
		_ge_vertex_size += 4 * 3;
	}else
	if(mode & GE_VERTEX_16BIT){
		_ge_vertex_size += 2 * 3;
	}else
	if(mode & GE_VERTEX_8BIT){
		_ge_vertex_size += 1 * 3;
	}
}

void geDrawArrayPointer(void* vertices){
	geSendCommandi(CMD_DRAW_VERTICES_POINTER, (((unsigned int)vertices) >> 8) & 0xf0000);
	geSendCommandi(CMD_VERTICES_POINTER, ((unsigned int)vertices) & 0xffffff);
	_ge_vertices_pointer = vertices;
}

void geDrawIndicesPointer(void* indices){
	geSendCommandi(CMD_INDICES_POINTER, ((unsigned int)indices) & 0xffffff);
}

void geDrawCount(int count){
	draw_count = count;
}

void geDraw(){
	geSendCommandi(CMD_DRAW_TYPE_VCOUNT, (draw_type << 16) | draw_count);
}

void geDrawArray(int type, int start, int count){
	if(_ge_vertices_pointer){
		draw_type = type;
		u32 addr = (u32)_ge_vertices_pointer + start * _ge_vertex_size;
		geSendCommandi(CMD_DRAW_VERTICES_POINTER, (addr >> 8) & 0xf0000);
		geSendCommandi(CMD_VERTICES_POINTER, addr & 0xffffff);
		geSendCommandi(CMD_DRAW_TYPE_VCOUNT, (type << 16) | count);
	}
}

void geDrawArrayFull(int type, int mode, void* vertices, int count){
	draw_type = type;
	draw_mode = mode;
	geSendCommandi(CMD_DRAW_MODE, mode);
	geSendCommandi(CMD_DRAW_VERTICES_POINTER, (((unsigned int)vertices) >> 8) & 0xf0000);
	geSendCommandi(CMD_VERTICES_POINTER, ((unsigned int)vertices) & 0xffffff);
	geSendCommandi(CMD_DRAW_TYPE_VCOUNT, (type << 16)|count);
}

void* geListGetMemory(LibGE_DisplayList* dList, int size){
	size += 3;
	size += ((unsigned int)(size >> 31)) >> 30;
	size = (size >> 2) << 2;

	unsigned int* orig_ptr = dList->current;
	unsigned int* new_ptr = (unsigned int*)(((unsigned int)orig_ptr) + size + 8);
	
	orig_ptr[0] = (16 << 24) | ((((unsigned int)new_ptr) >> 8) & 0xf0000);
	orig_ptr[1] = (8 << 24) | (((unsigned int)new_ptr) & 0xffffff);

	dList->current = new_ptr;

	return orig_ptr + 2;
}

void* geGetMemory(int size){
	return geListGetMemory(&context->dList, size);
}

void geDebugOut(char* buff, int bufsz){
	sceIoWrite(2, buff, bufsz);
}

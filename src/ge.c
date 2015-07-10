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

#include "ge_internal.h"

void exit(int);

LibGE_Context* libge_context = NULL;
void ge_UseDraw2dService();
void ge_UnuseDraw2dService();
void WindowsRoutine();
int SystemSwapBuffers();

extern unsigned char ge_splash_screen[161041];
static bool _ge_splashscreen_enabled = true;

void geDebugOut(char*, int);
static ge_File* logfile = NULL;
static int debug_mode = GE_DEBUG_ERROR | GE_DEBUG_PRINT;
static bool debug_critical = true;

static char _ge_path_build[10][2048] = { "" };
static int _ge_path_build_i = 0;
char* _ge_BuildPath(const char* path, const char* filename){
	if(path == NULL || path[0] == 0x0){
		strcpy(_ge_path_build[_ge_path_build_i], filename);
	}else{
		sprintf(_ge_path_build[_ge_path_build_i], "%s/%s", path, filename);
	}
	char* ret = _ge_path_build[_ge_path_build_i];
	_ge_path_build_i++;
	if(_ge_path_build_i >= 10){
		_ge_path_build_i = 0;
	}
	return ret;
}

void ge_BaseInit(){
	logfile = NULL;
	debug_mode = GE_DEBUG_ERROR | GE_DEBUG_PRINT;
	debug_critical = true;
	memset(_ge_path_build, 0, sizeof(_ge_path_build));
	_ge_path_build_i = 0;
}

void geInit(){
	libge_context = (LibGE_Context*)geMalloc(sizeof(LibGE_Context));
	libge_context->mouse_x = 0;
	libge_context->mouse_y = 0;
	libge_context->mouse_round = false;
	libge_context->ge_keys = geCreateKeys();
	libge_context->clear_color = 0xFF000000;
	libge_context->clear_mode = GE_CLEAR_COLOR_BUFFER;
	libge_context->ram_extended = false;
	libge_context->drawing_mode = GE_DRAWING_MODE_3D;
	libge_context->swap_func = SystemSwapBuffers;
	libge_context->cursor_image = NULL;
	libge_context->fontbuf = NULL;
	libge_context->draw_off_x = 0;
	libge_context->draw_off_y = 0;
	libge_context->projection_matrix[0] = (float)0xFFFFFFFF;
	libge_context->default_shaders_path = "default_shaders";

	geInitFps();

	if((debug_mode & GE_DEBUG_LOGFILE) && !logfile){
		logfile = geFileOpen("ge.log", GE_FILE_MODE_WRITE);
	}
}

LibGE_Context* geGetContext(){
	return libge_context;
}

void geSetContext(LibGE_Context* ctx){
	ge_Image* last_cur = libge_context->cursor_image;
	u32 last_sys = libge_context->syscontext;

	memcpy(libge_context, ctx, sizeof(LibGE_Context));

	libge_context->syscontext = last_sys;
	libge_context->cursor_image = last_cur;
}

void geDrawOffset(int x, int y){
	libge_context->draw_off_x = x;
	libge_context->draw_off_y = y;
}

u32 geMixColors(u32 c1, u32 c2, float f){
	u8 R1=R(c1);	u8 G1=G(c1);	u8 B1=B(c1);	u8 A1=A(c1);
	u8 R2=R(c2);	u8 G2=G(c2);	u8 B2=B(c2);	u8 A2=A(c2);
	float nf = 1.0 - f;
	/*
	u8 r = (R1 + R2) /2;
	u8 g = (G1 + G2) /2;
	u8 b = (B1 + B2) /2;
	u8 a = (A1 + A2) /2;
	*/
	u8 r = R1*nf + R2*f;
	u8 g = G1*nf + G2*f;
	u8 b = B1*nf + B2*f;
	u8 a = A1*nf + A2*f;
	r = sqrtf((R1*R1 + R2*R2)/2);
	g = sqrtf((G1*G1 + G2*G2)/2);
	b = sqrtf((B1*B1 + B2*B2)/2);
	a = sqrtf((A1*A1 + A2*A2)/2);
	return RGBA(r, g, b, a);
}

int geGetDrawingMode(){
	return libge_context->drawing_mode;
}

void geCursorImage(ge_Image* img){
	if(img){
		libge_context->cursor_image = img;
		geCursorVisible(false);
	}else{
		geCursorVisible(true);
		libge_context->cursor_image = NULL;
	}
}

void geSwapFunction(int (*func)()){
	libge_context->swap_func = func;
}

int geSwapBuffers(){
#ifndef LIBGE_MINI
	WindowsRoutine();
#endif
	if(libge_context->cursor_image){
		int mx=0, my=0;
		geCursorPosition(&mx, &my);
		geDrawImage(mx, my, libge_context->cursor_image);
	}
	ge_Fps_Routine();
	geReadKeys(libge_context->ge_keys);
	return libge_context->swap_func();
}

void geSplashscreenEnable(bool enabled){
	_ge_splashscreen_enabled = enabled;
}

void ge_Splashscreen(){
#ifndef LIBGE_MINI
	if(!_ge_splashscreen_enabled)return;
	u32 ticks = geGetTick();
	u32 time = 0;
	ge_Image* splash = geLoadImage(geFileFromBuffer(ge_splash_screen, sizeof(ge_splash_screen)));
	int h = (splash->height > libge_context->height) ? libge_context->height : splash->height;
	int w = h * splash->width / splash->height;
	geDrawingMode(GE_DRAWING_MODE_2D);
	while(time < 3000){
		time = geGetTick() - ticks;
		if(time < 800){
			splash->color = RGBA(255, 255, 255, (u8)(time * 255 / 800));
		}else
		if(time < 2200){
			splash->color = RGBA(255, 255, 255, 255);
		}else
		if(time < 3000){
			splash->color = RGBA(255, 255, 255, (u8)((1000-(time-3000)) * 255 / 1000));
		}
		geClearScreen();
		geBlitImageStretched(libge_context->width/2, libge_context->height/2, splash, 0, 0, splash->width, splash->height, w, h, GE_BLIT_CENTERED);
		geSwapBuffers();
	}
	geFreeImage(splash);
#endif
}

void geDebugMode(int mode){
	debug_mode = mode;
}

bool geDebugCritical(bool enabled){
	bool last = debug_critical;
	debug_critical = enabled;
	return last;
}

void geDebugTerm(){
	if(logfile){
		geFileClose(logfile);
	}
}

void gePrintDebug(int level, const char *format, ...){
	if(!debug_mode){
		return;
	}
	va_list	opt;
	char     buff[8192] = "";
	char     buff2[8192] = "";
	va_start(opt, format);
	vsnprintf( buff, (size_t) sizeof(buff), format, opt);

	if((level & 0xF) < 0 || (level & 0xF) > 3){
		return;
	}

	if((level & 0xF) == 0 && (debug_mode & GE_DEBUG_INFO)){
		if(level & 0x100){
			sprintf(buff2, "LibGE::INFO: %s", buff);
		}else{
			sprintf(buff2, "INFO: %s", buff);
		}
	}else
	if((level & 0xF) == 1 && (debug_mode & GE_DEBUG_WARN)){
		if(level & 0x100){
			sprintf(buff2, "LibGE::WARNING: %s", buff);
		}else{
			sprintf(buff2, "WARNING: %s", buff);
		}
	}else
	if((level & 0xF) == 2 && (debug_mode & GE_DEBUG_ERROR)){
		if(level & 0x100){
			sprintf(buff2, "LibGE::CRITICAL: %s", buff);
		}else{
			sprintf(buff2, "CRITICAL: %s", buff);
		}
		debug_mode |= GE_DEBUG_LOGFILE;
	}else
	if((level & 0xF) == 3 && (debug_mode & GE_DEBUG_INFO)){
		strcpy(buff2, buff);
	}

	if(debug_mode & GE_DEBUG_PRINT){
		geDebugOut(buff2, strlen(buff2));
#ifdef WIN32
		fflush(stdout);
#endif
	}
	if((logfile) && (debug_mode & GE_DEBUG_LOGFILE)){
		int x = strlen(buff2);
		if(x > 0 && buff2[x-1] != '\n'){
			buff2[x] = '\n';
			buff2[x+1] = 0x0;
		}
		geFileWrite(logfile, buff2, strlen(buff2));
	}

	if((level & 0xF) == 2 && debug_critical){
		if(logfile){
			geFileClose(logfile);
		}
		exit(0);
	}
}

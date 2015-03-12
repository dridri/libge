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

#include <jni.h>
#include <errno.h>
#include <stdbool.h>

#include "../../ge_internal.h"

#include <android/sensor.h>
#include <android/log.h>
#include <android/native_activity.h>
#include <android/native_window_jni.h>
#include <android/window.h>
#include <jni.h>
#include <android_native_app_glue.h>

void exit(int);

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "libge", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "libge", __VA_ARGS__))

int main(int, char**);

typedef struct Engine {
	JNIEnv* env;
	jobject aJavaSurface;
	struct android_app* app;
	ANativeWindow* aSurface;
	int ofsx;
	int ofsy;
	int fullscreen;

	ASensorManager* sensorManager;
	const ASensor* accelerometerSensor;
	ASensorEventQueue* sensorEventQueue;

	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
	EGLConfig config;
	EGLint format;
	int32_t width;
	int32_t height;
} Engine;

typedef struct ATouch {
	int used;
	int id;
	int action;
	float x, y;
	float force;
} ATouch;

static Engine engine;
static bool hasSurface = false;
static LibGE_AndroidContext* _ge_android_context = NULL;
static bool geCreateMainWindow_called = false;

static bool v_keys[GE_KEYS_COUNT] = { 0 };
static ATouch touches[16] = { { 0, 0, 0, 0.0, 0.0, 0.0 } };
static int nPressed = 0;
static int cursorId = -1;
static bool last_cPress = false;
static bool cPress = false;

static int aflags = 0;

static int mouse_last_x = 0;
static int mouse_last_y = 0;
static int mouse_warp_x = 0;
static int mouse_warp_y = 0;
static bool cursor_pressed = false;
static bool cursor_visible = true;
static bool ge_window_created = false;
static bool gotfocus = true;
static ge_Image* loadRawCursor();

static void engine_term_display(Engine* engine) {
	if (engine->display != EGL_NO_DISPLAY) {
		eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (engine->context != EGL_NO_CONTEXT) {
			eglDestroyContext(engine->display, engine->context);
		}
		if (engine->surface != EGL_NO_SURFACE) {
			eglDestroySurface(engine->display, engine->surface);
		}
		eglTerminate(engine->display);
	}
	engine->display = EGL_NO_DISPLAY;
	engine->context = EGL_NO_CONTEXT;
	engine->surface = EGL_NO_SURFACE;
}

int geCreateMainWindow(const char* title, int Width, int Height, int flags){
	LOGW("geCreateMainWindow(\"%s\", %d, %d, %d)\n", title, Width, Height, flags);
	libge_context->syscontext = (t_ptr)_ge_android_context;

// 					if(engine.aJavaSurface){
// 						LOGI("Getting surface !");
// 						engine.aSurface = ANativeWindow_fromSurface(engine.env, engine.aJavaSurface);
// 						LOGI(" ===> %p", engine.aSurface);
// 					}else{
// 						engine.aSurface = engine.app->window;
// 					}
	while(hasSurface && !engine.aSurface){
		geSleep(10);
	}

	aflags = AWINDOW_FLAG_KEEP_SCREEN_ON;
	if(flags & GE_WINDOW_FULLSCREEN){
		aflags |= AWINDOW_FLAG_FULLSCREEN;
		engine.aSurface = 0;
	}
// 	if(engine.app && engine.aSurface){
		ANativeActivity_setWindowFlags(_ge_android_context->state->activity, aflags, 0);
// 	}


	geCreateMainWindow_called = true;

	while(hasSurface && !engine.aSurface){
		geSleep(10);
	}
// 	while(!hasSurface && !engine.aSurface){
// 		engine.aSurface = engine.app->window;
// 	}

	const EGLint attribs[] = {
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
			EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
			EGL_BLUE_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_RED_SIZE, 8,
			EGL_ALPHA_SIZE, 8,
			EGL_DEPTH_SIZE, 16,
			EGL_NONE
	};
	const EGLint context_attribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};

	EGLint w, h, dummy, format;
	EGLint numConfigs;
	EGLConfig config;
	EGLSurface surface;
	EGLContext context;

	EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	eglInitialize(display, 0, 0);
	eglChooseConfig(display, attribs, &config, 1, &numConfigs);
	eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

	int base_width = 0;
	int base_height = 0;
	if(engine.app && engine.aSurface){
		base_width = ANativeWindow_getWidth(engine.aSurface);
		base_height = ANativeWindow_getHeight(engine.aSurface);
	}
	LOGW("base size: %d x %d", base_width, base_height);

	int width = Width < 0 ? base_width : Width;
	int height = Height < 0 ? base_height : Height;
	LOGW("new size: %d x %d", width, height);
	if(engine.app && engine.aSurface){
		ANativeWindow_setBuffersGeometry(engine.aSurface, width, height, WINDOW_FORMAT_RGBA_8888/*format*/);
// 		ANativeWindow_setBuffersGeometry(engine.aSurface, 0, 0, WINDOW_FORMAT_RGBA_8888/*format*/);
		LOGW("Ok");
		LOGW("BLAHLBH");
		surface = eglCreateWindowSurface(display, config, engine.aSurface, NULL);
	}
	if(engine.context == EGL_NO_CONTEXT){
		context = eglCreateContext(display, config, NULL, context_attribs);
	}else{
		context = engine.context;
	}
	if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
		LOGW("Unable to eglMakeCurrent");
		//return -1;
	}

	eglQuerySurface(display, surface, EGL_WIDTH, &w);
	eglQuerySurface(display, surface, EGL_HEIGHT, &h);

	libge_context->vidcontext = (t_ptr)&engine;
	libge_context->width = w;
	libge_context->height = h;
	LOGI("final size: %d x %d", w, h);
	
	if(engine.context == EGL_NO_CONTEXT){
		gePrintDebug(0x100, "glGetString: 0x%08lX\n", (unsigned long)glGetString);
		gePrintDebug(0x100, "Current OpenGL version: %s\n", (const char*)glGetString(GL_VERSION));
		geInitVideo();
		gePrintDebug(0x100, "geCreateMainWindow 4\n");
		geInitShaders();
		gePrintDebug(0x100, "geCreateMainWindow 5\n");
		geGraphicsInit();
		gePrintDebug(0x100, "geCreateMainWindow 6\n");
		geDrawingMode(GE_DRAWING_MODE_2D);
		gePrintDebug(0x100, "geCreateMainWindow 7\n");
		_ge_android_context->cursor = loadRawCursor();
		gePrintDebug(0x100, "geCreateMainWindow 8\n");
	}
	
	
	engine.format = format;
	engine.config = config;
	engine.display = display;
	engine.context = context;
	engine.surface = surface;
	engine.width = w;
	engine.height = h;
	_ge_android_context->config = config;
	_ge_android_context->display = display;
	_ge_android_context->context = context;
	_ge_android_context->surface = surface;

	ge_window_created = true;
	return 0;
}

void geFullscreen(bool fullscreen, int width, int height){
}

void geWaitVsync(int enabled){
	eglSwapInterval(engine.display, enabled);
}

static int32_t engine_handle_input(struct android_app* app, AInputEvent* event){
	Engine* engine = (Engine*)app->userData;
	int i, j, k;
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION){
		int n = AMotionEvent_getPointerCount(event);
		int action = AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK;
		int pointer_index = (AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
		/*
		gePrintDebug(0x100, "nTouchPoints: %d\n", n);
		gePrintDebug(0x100, "action: %02X\n", action);
		gePrintDebug(0x100, "pointer_index: %d\n", pointer_index);
		*/
		for(j=0; j<n; j++){
		//	gePrintDebug(0x100, " => [%d] : %f %f  id %d\n", j, AMotionEvent_getX(event, j), AMotionEvent_getY(event, j), AMotionEvent_getPointerId(event, j));
			ATouch* touch = NULL;
			for(i=0; i<sizeof(touches)/sizeof(ATouch); i++){
				if(touches[i].used && ((touches[i].action == AMOTION_EVENT_ACTION_UP) || (touches[i].action == AMOTION_EVENT_ACTION_POINTER_UP))){
					touches[i].used = false;
					if(touches[i].id == cursorId){
						cursorId = -1;
					}
				}
				if(touches[i].used && touches[i].id == AMotionEvent_getPointerId(event, j)){
					touch = &touches[i];
					break;
				}
			}
			if(!touch){
				for(i=0; i<sizeof(touches)/sizeof(ATouch); i++){
					if(touches[i].used == false){
						touch = &touches[i];
						touch->used = true;
						touch->id = AMotionEvent_getPointerId(event, j);
						break;
					}
				}
			}
			if(touch){
				if(j == pointer_index){
					touch->action = action;
				}
				touch->x = AMotionEvent_getX(event, j) * engine->width / ANativeWindow_getWidth(engine->app->window);
				touch->y = AMotionEvent_getY(event, j) * engine->height / ANativeWindow_getHeight(engine->app->window);
// 				gePrintDebug(0x100, "press y : %.2f => %.2f (%d -- %d)", touch->y, AMotionEvent_getY(event, j), engine->height, ANativeWindow_getHeight(engine->app->window));
				touch->force = AMotionEvent_getPressure(event, j);
				if(cursorId < 0 || (touch->id <= cursorId && touch->used)){
					if(action == AMOTION_EVENT_ACTION_DOWN || action == AMOTION_EVENT_ACTION_POINTER_DOWN){
						v_keys[GEK_LBUTTON] = true;
					}
					if(action == AMOTION_EVENT_ACTION_UP || action == AMOTION_EVENT_ACTION_POINTER_UP){
						v_keys[GEK_LBUTTON] = false;
					}
					last_cPress = cPress;
					cPress = v_keys[GEK_LBUTTON];
					cursorId = touch->id;
					mouse_last_x = libge_context->mouse_x;
					mouse_last_y = libge_context->mouse_y;
					libge_context->mouse_x = (int)touch->x;
					libge_context->mouse_y = (int)touch->y;
					if(!last_cPress){
						mouse_last_x = libge_context->mouse_x;
						mouse_last_y = libge_context->mouse_y;
					}
					mouse_warp_x = libge_context->mouse_x - mouse_last_x;
					mouse_warp_y = libge_context->mouse_y - mouse_last_y;
				}
			}
		}
        return 1;
    }
	return 0;
}

static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
	Engine* engine = (Engine*)app->userData;
	int w, h;
	switch(cmd){
		case APP_CMD_SAVE_STATE:
			gePrintDebug(0x101, "---------- APP_CMD_SAVE_STATE\n");
			break;
		case APP_CMD_INIT_WINDOW:
			gePrintDebug(0x101, "---------- APP_CMD_INIT_WINDOW\n");
			if(engine->app->window != NULL && ge_window_created){
					while(hasSurface && !engine->aSurface){
						geSleep(10);
					}
// 					if(engine->aJavaSurface){
// 						LOGI("Getting surface !");
// 						engine->aSurface = ANativeWindow_fromSurface(engine->env, engine->aJavaSurface);
// 						LOGI(" ===> %p", engine->aSurface);
// 					}else{
// 						engine->aSurface = engine->app->window;
// 					}
					LOGW("-------------- CREATING WINDOW 1\n");
					eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
					ANativeActivity_setWindowFlags(_ge_android_context->state->activity, aflags, 0);
					LOGW("-------------- CREATING WINDOW 2\n");
					eglDestroySurface(engine->display, engine->surface);
					LOGW("-------------- CREATING WINDOW 3\n");
// 					ANativeWindow_setBuffersGeometry(engine->aSurface, engine->width, engine->height, /*engine->format*/WINDOW_FORMAT_RGBA_8888);
					ANativeWindow_setBuffersGeometry(engine->aSurface, 0, 0, /*engine->format*/WINDOW_FORMAT_RGBA_8888);
					LOGW("-------------- CREATING WINDOW 3.1\n");
					engine->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
					LOGW("-------------- CREATING WINDOW 3.2\n");
					engine->surface = eglCreateWindowSurface(engine->display, engine->config, engine->aSurface, NULL);
					LOGW("-------------- CREATING WINDOW 4 (0X%08X / %d\n", engine->aSurface, engine->aSurface);
					LOGW("-------------- CREATING WINDOW 4 (0X%08X / %d\n", engine->display, engine->display);
					LOGW("-------------- CREATING WINDOW 4 (0X%08X / %d\n", engine->surface, engine->surface);
					eglMakeCurrent(engine->display, engine->surface, engine->surface, engine->context);
					LOGW("-------------- CREATING WINDOW 5\n");
					eglQuerySurface(engine->display, engine->surface, EGL_WIDTH, &w);
					eglQuerySurface(engine->display, engine->surface, EGL_HEIGHT, &h);
					LOGW("final size: %d x %d", w, h);
					_ge_android_context->display = engine->display;
					_ge_android_context->surface = engine->surface;
			}
			break;
		case APP_CMD_TERM_WINDOW:
			gePrintDebug(0x101, "---------- APP_CMD_TERM_WINDOW\n");
			eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
			if (engine->surface != EGL_NO_SURFACE) {
				eglDestroySurface(engine->display, engine->surface);
				engine->surface = EGL_NO_SURFACE;
			}
			if(hasSurface){
				engine->aSurface = 0;
			}
			break;
		case APP_CMD_GAINED_FOCUS:
			gePrintDebug(0x101, "---------- APP_CMD_GAINED_FOCUS\n");
			ge_ResumeAllThreads();
			gotfocus = true;
			break;
		case APP_CMD_LOST_FOCUS:
			gePrintDebug(0x101, "---------- APP_CMD_LOST_FOCUS\n");
			ge_PauseAllThreads();
			gotfocus = false;
			break;
	}
}

static void onContentRectChanged(ANativeActivity* activity, const ARect* rect) {
	LOGW("onContentRectChanged: l=%d,t=%d,r=%d,b=%d", rect->left, rect->top, rect->right, rect->bottom);
}

extern u32 tick_pause;
void PollEvents(){
	int ident;
	int events;
	struct android_poll_source* source;
	u32 pause_start = geGetTick();
	bool time_shift = false;

	do {
		while ((ident=ALooper_pollAll(0, NULL, &events, (void**)&source)) >= 0) {
			if (source != NULL) {
				source->process(_ge_android_context->state, source);
			}
			if (_ge_android_context->state->destroyRequested != 0) {
				engine_term_display(&engine);
				geDebugTerm();
				exit(0);
				return;
			}
		}
		if(gotfocus == false){
			geSleep(100);
			time_shift = true;
		}
	} while(gotfocus == false);

	if(time_shift){
		tick_pause += geGetTick() - pause_start;
	}
}

void AndroidSwapBuffers(){
	mouse_warp_x = mouse_warp_y = 0;

	PollEvents();
	while(engine.surface == EGL_NO_SURFACE){
		geSleep(100);
		PollEvents();
	}

	if(cursor_visible && !libge_context->cursor_image){
		int drawing_mode = geDrawingMode(GE_DRAWING_MODE_2D);
		geDrawImage(libge_context->mouse_x, libge_context->mouse_y, _ge_android_context->cursor);
		geDrawingMode(drawing_mode);
	}
	eglSwapBuffers(engine.display, engine.surface);
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
	libge_context->mouse_round = active;
}

void geCursorVisible(bool visible){
	cursor_visible = visible;
}

void AndroidReadKeys(ge_Keys* keys){
	int i;
	for(i=0; i<keys->touch->maxReports; i++){
		if(touches[i].used){
			keys->touch->reports[i].used = true;
			keys->touch->reports[i].pressed = ((touches[i].action != AMOTION_EVENT_ACTION_UP) && (touches[i].action != AMOTION_EVENT_ACTION_POINTER_UP));
			keys->touch->reports[i].force = (u16)(touches[i].force * 255.0);
			keys->touch->reports[i].x = (u16)touches[i].x;
			keys->touch->reports[i].y = (u16)touches[i].y;
		}else{
			keys->touch->reports[i].used = false;
		}
	}
	memcpy(keys->pressed, v_keys, GE_KEYS_COUNT);
}

#include <stdlib.h>
void android_main(struct android_app* state){
	LOGW("\n\n\n\n\n\n\n\n\n\n\n");
	LOGW("LibGE For Android beta bordel de merde !\n");
	chdir("/data/data/com.ssp.libge/files");

	memset(&engine, 0, sizeof(engine));
	state->userData = &engine;
	state->onAppCmd = engine_handle_cmd;
	state->onInputEvent = engine_handle_input;
	engine.app = state;
	engine.aSurface = engine.app->window;

	_ge_android_context = (LibGE_AndroidContext*)malloc(sizeof(LibGE_AndroidContext));
	_ge_android_context->state = state;

	while(!state->window){
		PollEvents();
	}
	/*
	geInit();
	geDebugMode(GE_DEBUG_ALL);
	geCreateMainWindow("LibGE", -1, -1, GE_WINDOW_FULLSCREEN);

	glClearColor(1.0, 0.0, 1.0, 1.0);
	while (1) {
		glClear(GL_COLOR_BUFFER_BIT);
		PollEvents();
		if(engine.display != EGL_NO_DISPLAY && engine.surface != EGL_NO_SURFACE){
			eglSwapBuffers(engine.display, engine.surface);
		}
	}
	*/
	main(0, NULL);
}

JNIEXPORT void JNICALL Java_com_drich_libge_LibGE_setSurface(JNIEnv* env, jobject obj, jobject surface, jint ofsx, jint ofsy)
{
	engine.env = env;
	engine.aJavaSurface = surface;
	LOGI("Java_com_drich_libge_LibGE_setSurface(%p, %d, %d)", surface, ofsx, ofsy);
	engine.ofsx = ofsx;
	engine.ofsy = ofsy;
	engine.aSurface = ANativeWindow_fromSurface(env, surface);
	LOGI(" ===> %p", engine.aSurface);
}

JNIEXPORT void JNICALL Java_com_drich_libge_LibGE_setHasSurface(JNIEnv* env, jobject obj, jint _hasSurface)
{
	hasSurface = _hasSurface;
}

LibGE_AndroidContext* _ge_GetAndroidContext(){
	return _ge_android_context;
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
	gePrintDebug(0x100, "loading RawCursor");
	int i, j, c;
	unsigned char* data = _ge_default_cursor;
	ge_Image* surface = geCreateSurface(data[0], data[1], 0x000000000);
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
	return surface;
}

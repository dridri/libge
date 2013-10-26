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
#include <stdlib.h>

void LinuxInit();
void CloseFullScreen();

bool initializing = false;

static XWMHints* win_hints = NULL;
static XSizeHints* win_size_hints = NULL;
static int attributes[] = { GLX_RGBA, GLX_DOUBLEBUFFER, GLX_RED_SIZE, 8, GLX_GREEN_SIZE, 8, GLX_BLUE_SIZE, 8, GLX_ALPHA_SIZE, 8, GLX_DEPTH_SIZE, 16, None};
static Cursor invisible_cursor;
static XEvent event;
static int event_mask = ExposureMask | PointerMotionMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | StructureNotifyMask;

void _ge_exit(){
	LibGE_LinuxContext* context = (LibGE_LinuxContext*)libge_context->syscontext;
	CloseFullScreen();
}

int geCreateMainWindow(const char* title, int Width, int Height, int flags){
	initializing = true;

	LibGE_LinuxContext* context = (LibGE_LinuxContext*)geMalloc(sizeof(LibGE_LinuxContext));
	libge_context->syscontext = (unsigned long)context;


	win_hints = XAllocWMHints();
	win_size_hints = XAllocSizeHints();

	libge_context->width = Width;
	libge_context->height = Height;
	context->fs = flags & GE_WINDOW_FULLSCREEN;

	XVisualInfo *vi;
	Colormap cmap;
	Window winDummy;
	unsigned int borderDummy;

	context->dpy = XOpenDisplay(0);
	context->screen = DefaultScreen(context->dpy);

	// get an appropriate visual
	vi = glXChooseVisual(context->dpy, context->screen, attributes);
	context->doubleBuffered = True;

	// create a color map
	cmap = XCreateColormap(context->dpy, RootWindow(context->dpy, vi->screen), vi->visual, AllocNone);
	context->attr.colormap = cmap;
	context->attr.border_pixel = 0;
	context->attr.background_pixmap = None;

	win_size_hints->flags = PSize;
	if(!(flags & GE_WINDOW_RESIZABLE)){
		win_size_hints->flags = PSize | PMinSize | PMaxSize;
		win_size_hints->min_width = libge_context->width;
		win_size_hints->min_height = libge_context->height;
		win_size_hints->max_width = libge_context->width;
		win_size_hints->max_height = libge_context->height;
		win_size_hints->base_width = libge_context->width;
		win_size_hints->base_height = libge_context->height;
	}

	if (context->fs){
	}else{
		// create a window in window mode
		context->attr.event_mask = event_mask;
		context->win = XCreateWindow(context->dpy, RootWindow(context->dpy, vi->screen), 0, 0, libge_context->width, libge_context->height, 0, vi->depth, InputOutput, vi->visual, CWBorderPixel | CWBackPixmap | CWColormap | CWEventMask, &context->attr);
		// only set window title and handle wm_delete_events if in windowed mode 
		XSetStandardProperties(context->dpy, context->win, title, title, None, NULL, 0, NULL);
		XMapRaised(context->dpy, context->win);
	}

	// create a GLX context
	context->ctx = glXCreateContext(context->dpy, vi, 0, true);

	Atom wmDelete = XInternAtom(context->dpy, "WM_DELETE_WINDOW", True);
	XSetWMProtocols(context->dpy, context->win, &wmDelete, 1);

	Pixmap bm_no; XColor black;
	static char bm_no_data[] = {0, 0, 0, 0, 0, 0, 0, 0};
	bm_no = XCreateBitmapFromData(context->dpy, context->win, bm_no_data, 8, 8);
	invisible_cursor = XCreatePixmapCursor(context->dpy, bm_no, bm_no, &black, &black, 0, 0);
	if (bm_no!=None)XFreePixmap(context->dpy, bm_no);

	XSetWMNormalHints(context->dpy, context->win, win_size_hints);
	XSetWMHints(context->dpy, context->win, win_hints);
	XSelectInput(context->dpy, context->win, event_mask);

	// connect the glx-context to the window
	glXMakeCurrent(context->dpy, context->win, context->ctx);
	XGetGeometry(context->dpy, context->win, &winDummy, &context->x, &context->y, (u32*)&libge_context->width, (u32*)&libge_context->height, &borderDummy, (u32*)&context->depth);
	gePrintDebug(0x100, "X11 Window: %dx%d	depth:%d	Direct rendering: %s\n", libge_context->width, libge_context->height, context->depth, glXIsDirect(context->dpy, context->ctx)?"yes":"no");

	LinuxInit();
	geWaitVsync(true);

	gePrintDebug(0x100, "Current OpenGL version: %s\n", (const char*)glGetString(GL_VERSION));
	geInitVideo();
	geInitShaders();
	geGraphicsInit();
	geDrawingMode(GE_DRAWING_MODE_2D);
	
	atexit(_ge_exit);

	initializing = false;
	return 0;
}

void geSetIcon(ge_Image* icon){
	LibGE_LinuxContext* context = (LibGE_LinuxContext*)libge_context->syscontext;

	Pixmap icon_pixmap = XCreatePixmapFromBitmapData(context->dpy, context->win, (char*)icon->data, icon->width, icon->height, 1, 0, 32);

	win_hints->flags |= IconPixmapHint;
	win_hints->icon_pixmap = icon_pixmap;
	XSetWMHints(context->dpy, context->win, win_hints);
}

void geFullscreen(bool fullscreen, int width, int height){
	if(width==0)width=libge_context->width;
	if(height==0)height=libge_context->height;
//	if(width<0)width=GetSystemMetrics(SM_CXSCREEN);
//	if(height<0)height=GetSystemMetrics(SM_CYSCREEN);
}

void CloseFullScreen(){
}

void geWaitVsync(int enabled){
	void (*glXSwapIntervalSGI)(int) = (void(*)(int))glXGetProcAddressARB((GLubyte*)"glXSwapIntervalSGI");
	if(glXSwapIntervalSGI){
		glXSwapIntervalSGI(enabled);
	}
}

short mouse_last_x=0, mouse_last_y=0;
short mouse_warp_x=0, mouse_warp_y=0;

int smooth_x=0, smooth_y=0;
short mouse_last_smooth_x=0, mouse_last_smooth_y=0;
short mouse_smooth_warp_x=0, mouse_smooth_warp_y=0;

bool keys_pressed[GE_KEYS_COUNT] = { false };
bool keys_released[GE_KEYS_COUNT] = { true };
static int last_pressed = 0;

static int current_buf = 0;

static bool pending_wup_release = false;
static bool pending_wdown_release = false;

static bool changed = false;
static bool _to_close = false;

int LinuxSwapBuffers(){
	LibGE_LinuxContext* context = (LibGE_LinuxContext*)libge_context->syscontext;

	mouse_warp_x = mouse_warp_y = 0;
	glXSwapBuffers(context->dpy, context->win);

	bool finished = false;
	int key = 0;

	if(pending_wup_release){
		keys_pressed[GEK_MWHEELUP] = false;
		keys_released[GEK_MWHEELUP] = true;
		pending_wup_release = false;
	}
	if(pending_wdown_release){
		keys_pressed[GEK_MWHEELDOWN] = false;
		keys_released[GEK_MWHEELDOWN] = true;
		pending_wdown_release = false;
	}

	while(XPending(context->dpy)){
		XNextEvent(context->dpy, &event);
	//	if(event.type)printf("event: %d\n", event.type);
		switch (event.type){
			case ClientMessage:	
			//	printf("Atom: \"%s\"\n", XGetAtomName(context->dpy, event.xclient.message_type));
				if (*XGetAtomName(context->dpy, event.xclient.message_type) == *"WM_PROTOCOLS"){
					/*
					ge_event.type = GE_EVENT_WINDOW_CLOSE;
					*/
					finished = true;
				}
				if (event.xclient.data.l[0] == XInternAtom(context->dpy, "WM_DELETE_WINDOW", False)){
					finished = true;
				}
				break;
			case ConfigureNotify:
				if(event.xexpose.width!=libge_context->width || event.xexpose.height!=libge_context->height){
				//	printf("expose : %d, %d, %d, %d\n", event.xconfigure.x, event.xconfigure.y, event.xconfigure.width, event.xconfigure.height);
					libge_context->width = event.xconfigure.width;
					libge_context->height = event.xconfigure.height;
					libge_context->projection_matrix[0] = (float)0xFFFFFFFF;
					geGraphicsInit();
					geDrawingMode(libge_context->drawing_mode | 0xF0000000);
				}
				break;
			case KeymapNotify:
				XRefreshKeyboardMapping(&event.xmapping);
				break;
			case KeyPress:
				key = (int)XLookupKeysym(&event.xkey, 0);
				if(key >= 0xFF00){
					key -= 0xFF00;
				}
				if(key >= 'a' && key <= 'z'){
					key += ('A' - 'a');
				}
				{
					char str[25];
					int len;
					KeySym keysym;
					len = XLookupString(&event.xkey, str, 25, &keysym, NULL);
				//	printf("input : '%s' [%d] (%d)\n", str, (u8)str[0], len);
					last_pressed = (u8)str[0];
				}
				//printf("----------------- PRESS KEY %d ---------------------\n", key);
				keys_pressed[key] = true;
				keys_released[key] = false;
				if(keys_pressed[GEK_LALT] && keys_pressed[GEK_F4]){
					finished = true;
					/*
					ge_event.type = GE_EVENT_WINDOW_CLOSE;
					*/
				}
				break;
			case KeyRelease:
				key = (int)XLookupKeysym(&event.xkey, 0);
				if(key >= 0xFF00){
					key -= 0xFF00;
				}
				if(key >= 'a' && key <= 'z'){
					key += ('A' - 'a');
				}
				keys_pressed[key] = false;
				keys_released[key] = true;
				break;
			case ButtonPress:
			//	printf("----------------- PRESS BUTTON %d ---------------------\n", event.xbutton.button);
				keys_pressed[event.xbutton.button] = true;
				keys_released[event.xbutton.button] = false;
				break;
			case ButtonRelease:
			//	printf("----------------- RELEASE BUTTON %d ---------------------\n", event.xbutton.button);
				if(event.xbutton.button == GEK_MWHEELUP){
					pending_wup_release = true;
				}else
				if(event.xbutton.button == GEK_MWHEELDOWN){
					pending_wdown_release = true;
				}else{
					keys_pressed[event.xbutton.button] = false;
					keys_released[event.xbutton.button] = true;
				}
				break;
			case MotionNotify:
				if(changed){
					changed = false;
					continue;
				}
				if(libge_context->mouse_round){
					/*
					if(libge_context->mouse_x <= 100){
						libge_context->mouse_x = libge_context->width-150;
						mouse_last_x += libge_context->width-250;
						changed = true;
					}
					if(libge_context->mouse_x >= libge_context->width-100){
						libge_context->mouse_x = 150;
						mouse_last_x -= libge_context->width-250;
						changed = true;
					}
					if(libge_context->mouse_y <= 100){
						libge_context->mouse_y = libge_context->height-150;
						mouse_last_y += libge_context->height-250;
						changed = true;
					}
					if(libge_context->mouse_y >= libge_context->height-100){
						libge_context->mouse_y = 150;
						mouse_last_y -= libge_context->height-250;
						changed = true;
					}
					*/
					mouse_warp_x = event.xmotion.x - libge_context->width / 2;
					mouse_warp_y = event.xmotion.y - libge_context->height / 2;
					libge_context->mouse_x = libge_context->width / 2;
					libge_context->mouse_y = libge_context->height / 2;
					changed = true;
					XWarpPointer(context->dpy, context->win, context->win, 0, 0, 0, 0, libge_context->mouse_x, libge_context->mouse_y);
				}else{
					mouse_last_x = libge_context->mouse_x;
					mouse_last_y = libge_context->mouse_y;
					libge_context->mouse_x = event.xmotion.x;
					libge_context->mouse_y = event.xmotion.y;
					mouse_warp_x = libge_context->mouse_x-mouse_last_x;
					mouse_warp_y = libge_context->mouse_y-mouse_last_y;
				}
				break;
			default:
				break;
		}
	}
	if(_to_close){
		CloseFullScreen();
		exit(1);
	}
	if(finished){
		_to_close = true;
		finished = false;
		exit(0);
	}

	changed = false;
	return (current_buf^=1);
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
	if(active){
		LibGE_LinuxContext* context = (LibGE_LinuxContext*)libge_context->syscontext;
		XWarpPointer(context->dpy, context->win, context->win, 0, 0, 0, 0, libge_context->width / 2, libge_context->height / 2);
	}
}

void geCursorVisible(bool visible){
	LibGE_LinuxContext* context = (LibGE_LinuxContext*)libge_context->syscontext;
	if(!visible){
		XDefineCursor(context->dpy, context->win, invisible_cursor);
	}else{
		XUndefineCursor(context->dpy, context->win);
	}
}

void LinuxGetPressedKeys(u8* k){
	memcpy(k, keys_pressed, GE_KEYS_COUNT*sizeof(u8));
}

int LinuxGetLastPressed(){
	int ret = last_pressed;
	last_pressed = 0;
	return ret;
}

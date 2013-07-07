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

#ifndef PFNWGLSWAPINTERVALFARPROC
typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALFARPROC)( int );
#endif

void WindowsInit();
void ListGlExtensions();
int CheckExtensionAvailable(const char* name);

static MSG message;
LRESULT CALLBACK _ge_windowProcedure(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
static void (*_ge_messageCallback)(HWND window, UINT message, WPARAM wParam, LPARAM lParam) = NULL;

static int initializing = false;
static int initialized = false;
static int width, height;
static int normal_width, normal_height;
static bool fsmode = false;
int iFormat = 0;

int geCreateMainWindow(const char* title, int Width, int Height, int flags){
	WNDCLASS winClass;
	RECT WindowRect;
	bool fullscreen = false;
	bool resizable = false;
	if(flags & GE_WINDOW_FULLSCREEN){
		fullscreen = true;
	}
	if(flags & GE_WINDOW_RESIZABLE){
		resizable = true;
	}
	initializing = true;

	LibGE_WinlowContext* context = (LibGE_WinlowContext*)geMalloc(sizeof(LibGE_WinlowContext));
	libge_context->syscontext = (t_ptr)context;

	context->hOpenGL = LoadLibrary("opengl32.dll");
	context->flags = flags;
	context->maxw = GetSystemMetrics(SM_CXSCREEN);
	context->maxh = GetSystemMetrics(SM_CYSCREEN);

	width = Width;
	height = Height;
	if(width<0)width=GetSystemMetrics(SM_CXSCREEN);
	if(height<0)height=GetSystemMetrics(SM_CYSCREEN);
	normal_width = width;
	normal_height = height;
	WindowRect.left=WindowRect.top=0;
	WindowRect.right=width;	WindowRect.bottom=height;
	context->instance = GetModuleHandle(NULL);

	libge_context->width = width;
	libge_context->height = height;

	memset(&winClass, 0x0, sizeof(winClass));
	winClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    winClass.lpfnWndProc = _ge_windowProcedure;
    winClass.cbClsExtra = 0;
    winClass.cbWndExtra = 0;
    winClass.hInstance = context->instance;
	winClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winClass.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
	winClass.lpszMenuName = NULL;
    winClass.lpszClassName = "_LibGE_OpenGL";
	RegisterClass(&winClass);

	int dwExStyle = 0;
	int dwStyle = 0;

	if(fullscreen){
		fsmode = true;
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth	= width;
		dmScreenSettings.dmPelsHeight	= height;
		dmScreenSettings.dmBitsPerPel	= 32;
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
	}

	if(fullscreen){
		dwExStyle=WS_EX_APPWINDOW;
		dwStyle=WS_POPUP;
	}else{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		if(resizable){
			dwStyle= (WS_OVERLAPPEDWINDOW-WS_MAXIMIZEBOX-WS_THICKFRAME)| WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU;
		}else if(!fullscreen){
			dwStyle= (WS_OVERLAPPEDWINDOW-WS_MAXIMIZEBOX-WS_THICKFRAME)| WS_BORDER | WS_MINIMIZEBOX | WS_SYSMENU;
		}
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);

	context->window = CreateWindowEx(dwExStyle, "_LibGE_OpenGL", title, dwStyle|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, WindowRect.right-WindowRect.left, WindowRect.bottom-WindowRect.top, NULL, NULL, context->instance, NULL);

	static	PIXELFORMATDESCRIPTOR pfd;
	context->hDC = GetDC(context->window);

	
	ZeroMemory(&pfd, sizeof (pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 2;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_STEREO;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cAlphaBits = 8;
//	pfd.cDepthBits = 16;
	pfd.cDepthBits = 32;
	pfd.iLayerType = PFD_MAIN_PLANE;
	int fmt = 0;
	if(iFormat == 0){
		fmt = ChoosePixelFormat(context->hDC, &pfd);
	}else{
		fmt = iFormat;
	}
	printf("fmt : %d\n", fmt);
	SetPixelFormat(context->hDC, fmt, &pfd);


	context->hRC = wglCreateContext(context->hDC);
	wglMakeCurrent(context->hDC, context->hRC);

	/*
	PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
	if(wglChoosePixelFormatARB){
		int pixelFormat;
		UINT numFormats;
		float fAttributes[] = {0,0};
		int iAttributes[] = {
			WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
			WGL_COLOR_BITS_ARB, 24,
			WGL_ALPHA_BITS_ARB, 8,
			WGL_DEPTH_BITS_ARB, 16,
			WGL_STENCIL_BITS_ARB, 0,
//			WGL_STEREO_ARB, GL_TRUE,
			WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
			WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
			WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
			WGL_SAMPLES_ARB, 8,
			0,0
		};
		printf("Chose format !\n");
		int _ret = wglChoosePixelFormatARB(context->hDC, iAttributes, fAttributes, 1, &pixelFormat, &numFormats);
		printf("RETURNED 0x%08X\n", _ret);
		int _if = iFormat;
		iFormat = pixelFormat;
		if(_if == 0){
			wglMakeCurrent(context->hDC, 0);
			wglDeleteContext(context->hRC);
			ReleaseDC(context->window, context->hDC);
			DestroyWindow(context->window);
			return geCreateMainWindow(title, Width, height, flags);
		}
	}
	*/
	geWaitVsync(true);
	
	WindowsInit();
	
	gePrintDebug(0x100, "Current OpenGL version: %s\n", (const char*)glGetString(GL_VERSION));
	geInitVideo();
	geInitShaders();
	geGraphicsInit();
	ge_Splashscreen();
	geDrawingMode(GE_DRAWING_MODE_2D);

	initialized = true;
	initializing = false;
	return message.wParam;
}

void geFullscreen(bool fullscreen, int width, int height){
	if(width==0)width=libge_context->width;
	if(height==0)height=libge_context->height;
	if(fullscreen){
		if(width<0)width=((LibGE_WinlowContext*)libge_context->syscontext)->maxw;
		if(height<0)height=((LibGE_WinlowContext*)libge_context->syscontext)->maxh;
	}else{
		if(width<0)width=normal_width;
		if(height<0)height=normal_height;
	}
	fsmode = fullscreen;

	DEVMODE dmScreenSettings;
	memset(&dmScreenSettings, 0x0, sizeof(dmScreenSettings));
	dmScreenSettings.dmSize = sizeof(dmScreenSettings);
	dmScreenSettings.dmBitsPerPel = 32;
	dmScreenSettings.dmFields=DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	if(fullscreen){
		RECT rChild;
		GetWindowRect(((LibGE_WinlowContext*)libge_context->syscontext)->window, &rChild);
		((LibGE_WinlowContext*)libge_context->syscontext)->winx = rChild.left;
		((LibGE_WinlowContext*)libge_context->syscontext)->winy = rChild.top;
		dmScreenSettings.dmPelsWidth = width;
		dmScreenSettings.dmPelsHeight = height;
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
		int dwStyle = WS_POPUP;
		SetWindowLong(((LibGE_WinlowContext*)libge_context->syscontext)->window, GWL_STYLE, dwStyle|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_VISIBLE);
		SetWindowPos(((LibGE_WinlowContext*)libge_context->syscontext)->window, 0, 0, 0, width, height, SWP_NOZORDER|SWP_DRAWFRAME|SWP_SHOWWINDOW);
	}else{
		dmScreenSettings.dmPelsWidth = ((LibGE_WinlowContext*)libge_context->syscontext)->maxw;
		dmScreenSettings.dmPelsHeight = ((LibGE_WinlowContext*)libge_context->syscontext)->maxh;
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
		int dwStyle = 0;
		if(((LibGE_WinlowContext*)libge_context->syscontext)->flags & GE_WINDOW_RESIZABLE){
			dwStyle = (WS_OVERLAPPEDWINDOW-WS_MAXIMIZEBOX-WS_THICKFRAME)| WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU;
		}else{
			dwStyle = (WS_OVERLAPPEDWINDOW-WS_MAXIMIZEBOX-WS_THICKFRAME)| WS_BORDER | WS_MINIMIZEBOX | WS_SYSMENU;
		}
		SetWindowLong(((LibGE_WinlowContext*)libge_context->syscontext)->window, GWL_STYLE, dwStyle|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_VISIBLE);
		SetWindowPos(((LibGE_WinlowContext*)libge_context->syscontext)->window, 0, ((LibGE_WinlowContext*)libge_context->syscontext)->winx, ((LibGE_WinlowContext*)libge_context->syscontext)->winy, width, height, SWP_NOZORDER|SWP_DRAWFRAME|SWP_SHOWWINDOW);
	}
}

void geWaitVsync(int enabled){
	PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress("wglSwapIntervalEXT");
	if(wglSwapIntervalEXT){
		wglSwapIntervalEXT(enabled);
	}
}

short mouse_last_x=0, mouse_last_y=0;
short mouse_warp_x=0, mouse_warp_y=0;

int smooth_x=0, smooth_y=0;
short mouse_last_smooth_x=0, mouse_last_smooth_y=0;
short mouse_smooth_warp_x=0, mouse_smooth_warp_y=0;

int last_key = 0;
bool keys_pressed[GE_KEYS_COUNT] = { false };
bool keys_released[GE_KEYS_COUNT] = { true };
static POINT point;

static bool has_focus = true;
static bool cursor_visible = true;

static int current_buf = 0;

ge_Thread* kb_thid = NULL;
int kb_thread(int args, void* argp){
	while(1){
		if(has_focus){
			int i;
			for(i=8; i<255; i++){
				keys_pressed[i] = GetAsyncKeyState(i);
			}
		}
		/*
		GetCursorPos(&point);
		ScreenToClient(((LibGE_WinlowContext*)libge_context->syscontext)->window, &point);
		if(point.x < 0 || point.x > libge_context->width || point.y < 0 || point.y > libge_context->height){
			keys_pressed[GEK_LBUTTON] = false;
			keys_pressed[GEK_RBUTTON] = false;
			keys_pressed[GEK_MBUTTON] = false;
			keys_pressed[GEK_MWHEELUP] = false;
			keys_pressed[GEK_MWHEELDOWN] = false;
		}else{
			keys_pressed[GEK_LBUTTON] = GetAsyncKeyState(GEK_LBUTTON);
			keys_pressed[GEK_RBUTTON] = GetAsyncKeyState(GEK_RBUTTON);
			keys_pressed[GEK_MBUTTON] = GetAsyncKeyState(GEK_MBUTTON);
		}
		*/
		geSleep(10);
	}
	return 0;
}

int WindowsSwapBuffers(){
	SwapBuffers(((LibGE_WinlowContext*)libge_context->syscontext)->hDC);
	
	mouse_warp_x = mouse_warp_y = 0;

	if(!kb_thid){
		kb_thid = geCreateThread("kb_thread", kb_thread, GE_THREAD_PRIORITY_NORMAL);
		geThreadStart(kb_thid, 0, NULL);
	//	AttachThreadInput(kb_thid->id, GetCurrentThreadId(), true);
	}
	keys_pressed[GEK_MWHEELUP] = false;
	keys_pressed[GEK_MWHEELDOWN] = false;

	if(has_focus){
			GetCursorPos(&point);
			ScreenToClient(((LibGE_WinlowContext*)libge_context->syscontext)->window, &point);

			mouse_last_smooth_x = smooth_x;
			mouse_last_smooth_y = smooth_y;
			smooth_x = (smooth_x + (point.x - smooth_x) * 0.7);
			smooth_y = (smooth_y + (point.y - smooth_y) * 0.7);
			mouse_smooth_warp_x = smooth_x - mouse_last_smooth_x;
			mouse_smooth_warp_y = smooth_y - mouse_last_smooth_y;
			
			mouse_last_x = libge_context->mouse_x;
			mouse_last_y = libge_context->mouse_y;
			libge_context->mouse_x = point.x;
			libge_context->mouse_y = point.y;

			bool changed = false;
			if(libge_context->mouse_round){
				if(libge_context->mouse_x <= 100){
					libge_context->mouse_x = width-150;
					mouse_last_x += width-250;
					smooth_x = width-150;
					mouse_last_smooth_x += width-250;
					changed = true;
				}
				if(libge_context->mouse_x >= width-100){
					libge_context->mouse_x = 150;
					mouse_last_x -= width-250;
					smooth_x = 150;
					mouse_last_smooth_x -= width-250;
					changed = true;
				}
				if(libge_context->mouse_y <= 100){
					libge_context->mouse_y = height-150;
					mouse_last_y += height-250;
					smooth_y = height-150;
					mouse_last_smooth_y += height-250;
					changed = true;
				}
				if(libge_context->mouse_y >= height-100){
					libge_context->mouse_y = 150;
					mouse_last_y -= height-250;
					smooth_y = 150;
					mouse_last_smooth_y += height-250;
					changed = true;
				}
				if(changed){
					point.x = libge_context->mouse_x;
					point.y = libge_context->mouse_y;
					ClientToScreen(((LibGE_WinlowContext*)libge_context->syscontext)->window, &point);
					SetCursorPos(point.x, point.y);
				}
			}
			mouse_warp_x = libge_context->mouse_x-mouse_last_x;
			mouse_warp_y = libge_context->mouse_y-mouse_last_y;
	}
	
	if(PeekMessage(&message, NULL, 0, 0, PM_REMOVE)){
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
	
	return (current_buf^=1);
}

void geRegisterSystemMessageCallback(void* cb){
	_ge_messageCallback = (void (*)(HWND window, UINT message, WPARAM wParam, LPARAM lParam))cb;
}

LRESULT CALLBACK _ge_windowProcedure(HWND window, UINT message, WPARAM wParam, LPARAM lParam){
	last_key = 0;
	if(message == WM_SETFOCUS){
		has_focus = true;
		ShowCursor(cursor_visible);
	}
	if(message == WM_KILLFOCUS){
		has_focus = false;
		ShowCursor(true);
	}
	if(message == WM_DESTROY){
		if(initializing == false){
			PostQuitMessage(0);
			exit(0);
		}
	}
	if(initialized && message == WM_SIZE){
		if(lParam != 0){
			if(!fsmode){
				normal_width = (lParam & 0x0000FFFF);
				normal_height = ((lParam & 0xFFFF0000) >> 16);
			}
			libge_context->width = (lParam & 0x0000FFFF);
			libge_context->height = ((lParam & 0xFFFF0000) >> 16);
			libge_context->projection_matrix[0] = (float)0xFFFFFFFF;
			geGraphicsInit();
			geDrawingMode(libge_context->drawing_mode | 0xF0000000);
		}
	}
	if(!initializing && has_focus){
		switch(message){
			case WM_CREATE:
				break;

			case WM_COMMAND:
				break;

			case WM_CHAR:
				last_key = wParam;
				break;
	/*
			case WM_KEYDOWN:
				last_key = wParam;
				break;

			case WM_KEYUP:
				last_key = 0;
				break;
	*/
			case WM_LBUTTONDOWN:
				keys_pressed[GEK_LBUTTON] = true;
				break;

			case WM_LBUTTONUP:
				keys_pressed[GEK_LBUTTON] = false;
				break;

			case WM_RBUTTONDOWN:
				keys_pressed[GEK_RBUTTON] = true;
				break;

			case WM_RBUTTONUP:
				keys_pressed[GEK_RBUTTON] = false;
				break;

			case WM_MBUTTONDOWN:
				keys_pressed[GEK_MBUTTON] = true;
				break;

			case WM_MBUTTONUP:
				keys_pressed[GEK_MBUTTON] = false;
				break;
			
			case WM_MOUSEWHEEL:
				if(((short)((wParam & 0xFFFF0000) >> 16)) > 0){
					keys_pressed[GEK_MWHEELUP] = true;
				}
				if(((short)((wParam & 0xFFFF0000) >> 16)) < 0){
					keys_pressed[GEK_MWHEELDOWN] = true;
				}
				break;

			case WM_MOUSEMOVE:
				break;

			case 0x00FF: //WM_INPUT
				break;
		}
	}

	if(_ge_messageCallback){
		_ge_messageCallback(window, message, wParam, lParam);
	}

	return DefWindowProc(window, message, wParam, lParam);
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
	ShowCursor(visible);
	cursor_visible = visible;
}

void WindowsGetPressedKeys(u8* k){
	memcpy(k, keys_pressed, GE_KEYS_COUNT*sizeof(u8));
}

int WindowsGetLastPressed(){
	return last_key;
}

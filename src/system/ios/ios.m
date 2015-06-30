#import <UIKit/UIKit.h>
#import <stdlib.h>
#import <string.h>
#include "../../../include/getypes.h"
#include "../../../include/gekeys.h"
#include "../../../include/geenums.h"
#include "../../ge_internal.h"
#include "AppDelegate.h"

void _ge_iOSFramebufferUseDefault();

extern EAGLContext* _ge_eagl_window_context;

extern int _ge_ios_defaultFramebuffer;
extern int _ge_ios_colorRenderbuffer;
extern int _ge_ios_depthRenderbuffer;

extern int _ge_ios_framebufferWidth;
extern int _ge_ios_framebufferHeight;

extern int _ge_argc;
extern char** _ge_argv;

int ge_internal_main(int argc, char** argv)
{
	@autoreleasepool {
		_ge_argc = argc;
		_ge_argv = argv;
		return UIApplicationMain(0, NULL, nil, NSStringFromClass([AppDelegate class]));
	}
}

void iOSOpenWindow(int* Width, int* Height, int flags){
	LibGE_iOSContext* context = (LibGE_iOSContext*)geMalloc(sizeof(LibGE_iOSContext));
	libge_context->syscontext = (void*)context;
	
	libge_context->width = _ge_ios_framebufferWidth;
	libge_context->height = _ge_ios_framebufferHeight;

	_ge_iOSFramebufferUseDefault();
}

void _ge_iOSFramebufferUseDefault(){
	glBindFramebuffer(GL_FRAMEBUFFER, _ge_ios_defaultFramebuffer);
}

static int _i = 0;
static bool warped = false;

void iOSWarpMouse(int x, int y){
}

void iOSSwapBuffer(void){
	glBindFramebuffer(GL_FRAMEBUFFER, _ge_ios_defaultFramebuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, _ge_ios_colorRenderbuffer);
	[_ge_eagl_window_context presentRenderbuffer:GL_RENDERBUFFER];
	
	SInt32 result;
	do {
		result = CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, TRUE);
	} while(result == kCFRunLoopRunHandledSource);
}

void* iOSGetProcAddress(const char *name){
}

void* iOSCreateGLContext(){
}

void iOSSetWarpMode(int en){
}

void iOSGetMousePos(int*x, int* y){
}

void geCursorVisible(bool visible){
}

FILE* _ge_ios_fopen(const char* file, const char* mode){
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *docs_dir = [paths objectAtIndex:0];
	NSString* aFile = [docs_dir stringByAppendingPathComponent: [NSString stringWithUTF8String:file]];
	
	gePrintDebug(0x100, "fopen on '%s'\n", [aFile fileSystemRepresentation]);
	FILE* fp = fopen([aFile fileSystemRepresentation], mode);
	if(!fp)gePrintDebug(0x100, "   error : %s\n", strerror(errno));
	if(!fp){
		NSString* path = [[NSBundle mainBundle] pathForResource: [NSString stringWithUTF8String:file] ofType:nil];
		gePrintDebug(0x100, "fopen on '%s' (%s)\n", [path cStringUsingEncoding:1], [path fileSystemRepresentation]);
		fp = fopen([path cStringUsingEncoding:1], mode);
		if(!fp)gePrintDebug(0x100, "   error : %s\n", strerror(errno));
	}

	return fp;
}

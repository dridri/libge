#import <Cocoa/Cocoa.h>
#import <mach-o/dyld.h>
#import <stdlib.h>
#import <string.h>
#include <OpenGL/OpenGL.h>
#include "../../../include/getypes.h"
#include "../../../include/gekeys.h"
#include "../../../include/geenums.h"

#define DEFAULT_OPENGL  "/System/Library/Frameworks/OpenGL.framework/Libraries/libGL.dylib"

@interface YsMacDelegate : NSObject /* < NSApplicationDelegate > */
/* Example: Fire has the same problem no explanation */
{
}
/* - (BOOL) applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication; */
@end

@implementation YsMacDelegate
- (BOOL) applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication
{
	return YES;
}
@end

@interface YsOpenGLWindow : NSWindow
{
}
@end


@interface YsOpenGLView : NSOpenGLView 
{
}
- (void) drawRect: (NSRect) bounds;
@end

void _ge_mac_resize(int w, int h);
static int win_flags = 0;
static NSOpenGLPixelFormat* gl_format;
static YsOpenGLWindow* ysWnd = nil;
static YsOpenGLView* ysView = nil;
static int warp_mode = 0;
static int mx = 0;
static int my = 0;
int _ge_mac_mouse_warp_x = 0;
int _ge_mac_mouse_warp_y = 0;
static u8 keys[GE_KEYS_COUNT + 32] = { 0 };


@implementation YsOpenGLWindow
- (id) initWithContentRect: (NSRect)rect styleMask:(NSUInteger)wndStyle backing:(NSBackingStoreType)bufferingType defer:(BOOL)deferFlg
{
	[super initWithContentRect:rect styleMask:wndStyle backing:bufferingType defer:deferFlg];

	[[NSNotificationCenter defaultCenter] 
		addObserver:self
		selector:@selector(windowDidResize:)
		name:NSWindowDidResizeNotification
		object:self];

	[[NSNotificationCenter defaultCenter]
	  addObserver:self
	  selector:@selector(windowWillClose:)
	  name:NSWindowWillCloseNotification
	  object:self];

	[self setAcceptsMouseMovedEvents:YES];

	printf("%s\n",__FUNCTION__);
	return self;
}

- (void) windowDidResize: (NSNotification *)notification
{
}

- (void) windowWillClose: (NSNotification *)notification
{
	[NSApp terminate:nil];	// This can also be exit(0);
}

- (BOOL)canBecomeKeyWindow
{
    return !(win_flags & 0xF0000000);
}

- (BOOL)canBecomeMainWindow
{
    return TRUE;
}

- (BOOL)willUseFullScreenContentSize
{
    return (win_flags & GE_WINDOW_FULLSCREEN);
}

@end


@implementation YsOpenGLView
-(void) drawRect: (NSRect) bounds
{
	printf("%s\n",__FUNCTION__);
//	exposure=1;
}

-(void) prepareOpenGL
{
	printf("%s\n",__FUNCTION__);
}

-(NSMenu *)menuForEvent: (NSEvent *)theEvent
{
	printf("%s\n",__FUNCTION__);
	return [NSView defaultMenu];
}

- (BOOL)acceptsFirstResponder {
	printf("%s\n",__FUNCTION__);
    return !(win_flags & 0xF0000000);
}

- (BOOL) isOpaque
{
	return FALSE;
}

- (void) flagsChanged: (NSEvent *)theEvent
{
	unsigned int flags;
	flags = [theEvent modifierFlags];
}

- (void) keyDown:(NSEvent *)theEvent
{
}

- (void) keyUp:(NSEvent *)theEvent
{
}

- (void) mouseMoved:(NSEvent *)theEvent
{
}

- (void)viewDidMoveToWindow
{
	[[NSNotificationCenter defaultCenter]
		addObserver:self
		selector:@selector(windowResized:)
		name:NSWindowDidResizeNotification
		object:[self window]];
}

- (void)windowResized:(NSNotification *)notification;
{
	NSSize size = [[self window] frame].size;
	NSLog(@"window width = %f, window height = %f", size.width, size.height);
	_ge_mac_resize(size.width, size.height);
}

@end

void MacOpenWindow(int* Width, int* Height, int flags){
	NSAutoreleasePool *pool=[[NSAutoreleasePool alloc] init];

	[NSApplication sharedApplication];
	[NSBundle loadNibNamed:@"MainMenu" owner:NSApp];

	YsMacDelegate *delegate;
	delegate = [YsMacDelegate alloc];
	[delegate init];
	[NSApp setDelegate: delegate];
	
	[NSApp finishLaunching];

	win_flags = flags;
	int width = *Width;
	int height = *Height;

	if(width < 0){
		*Width = width = [[NSScreen mainScreen] frame].size.width;
	}
	if(height < 0){
		*Height = height = [[NSScreen mainScreen] frame].size.height;
	}

	int samples = 1;
	if(flags & GE_WINDOW_MSAA2X){
		samples = 2;
	}
	if(flags & GE_WINDOW_MSAA4X){
		samples = 4;
	}
	if(flags & GE_WINDOW_MSAA8X){
		samples = 8;
	}

	NSRect contRect;
	contRect = NSMakeRect(0, 0, width, height);
	
	unsigned int winStyle=NSTitledWindowMask|NSClosableWindowMask|NSMiniaturizableWindowMask|NSResizableWindowMask;
	
	ysWnd = [YsOpenGLWindow alloc];
	if(flags & GE_WINDOW_FULLSCREEN){
		[ysWnd initWithContentRect:[[NSScreen mainScreen] frame] styleMask:NSBorderlessWindowMask backing:NSBackingStoreBuffered defer:NO];
		[ysWnd setLevel:NSMainMenuWindowLevel+1];
		[ysWnd setOpaque:YES];
	}else{
		[ysWnd initWithContentRect:contRect styleMask:winStyle backing:NSBackingStoreBuffered defer:NO];
	}
	if(flags & 0xF0000000){
	//	[ysWnd setBackgroundColor:[NSColor colorWithDeviceRed:0.0 green:0.0 blue:1.0 alpha:0.5]];
		[ysWnd setBackgroundColor:[NSColor clearColor]];
		[ysWnd setOpaque:NO];
	}

	NSOpenGLPixelFormatAttribute formatAttrib[] =
	{
#ifdef LIBGE_OPENGL30
        NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
#endif
    //    NSOpenGLPFAOpenGLProfile, 0x3200,
        NSOpenGLPFAColorSize    , 24                           ,
        NSOpenGLPFAAlphaSize    , 8                            ,
		NSOpenGLPFADepthSize, (NSOpenGLPixelFormatAttribute)32,
        NSOpenGLPFADoubleBuffer ,
		NSOpenGLPFAAccelerated  ,
		NSOpenGLPFANoRecovery   ,
		samples > 1 ? NSOpenGLPFASamples : 0, samples > 1 ? samples : 0,
	//	NSOpenGLPFAWindow,
		0
	};

	gl_format = [NSOpenGLPixelFormat alloc];
	[gl_format initWithAttributes: formatAttrib];
	
	ysView = [YsOpenGLView alloc];
	if(flags & GE_WINDOW_FULLSCREEN){
		contRect = NSMakeRect(0, 0, [[NSScreen mainScreen] frame].size.width, [[NSScreen mainScreen] frame].size.height);
	}else{
		contRect = NSMakeRect(0, 0, width, height);
	}
	[ysView initWithFrame:contRect pixelFormat:gl_format];

	[ysWnd setContentView:ysView];
/*
	[ysWnd setInitialFirstResponder:ysView];
	[ysWnd setNextResponder:ysView];
	[ysWnd makeFirstResponder:ysView];
*/
	[ysWnd makeMainWindow];
//	[ysWnd makeKeyAndOrderFront:ysWnd];
	[ysWnd orderFrontRegardless];
	if(flags & 0xF0000000){
		[ysWnd setLevel:NSMainMenuWindowLevel+1];
 	}else{
 		if(flags & GE_WINDOW_FULLSCREEN){
			[ysWnd setHidesOnDeactivate:YES];
 		}
		[NSApp setActivationPolicy: NSApplicationActivationPolicyRegular];
 		[ysWnd makeKeyWindow];
		[NSApp activateIgnoringOtherApps:YES];
 	}

//	YsAddMenu();

	[pool release];

	[[ysView openGLContext] makeCurrentContext];
	long zeroOpacity = 0;
	[[ysView openGLContext] setValues:&zeroOpacity forParameter:NSOpenGLCPSurfaceOpacity];

/*
	int i;
	for(i=0; i<FSKEY_NUM_KEYCODE; i++)
	{
		fsKeyIsDown[i]=0;
	}
*/
	CGAssociateMouseAndMouseCursorPosition(YES);
}

static int _i = 0;
static bool warped = false;
void MacWarpMouse(int x, int y){
	NSRect rect = [ysWnd contentRectForFrameRect:[ysWnd frame]];
	NSPoint location = [NSEvent mouseLocation];
	int winx = (int)rect.origin.x;
	int winy = (int)(ysWnd.screen.frame.size.height - (rect.origin.y + rect.size.height));
	int winw = (int)rect.size.width;
	int winh = (int)rect.size.height;
	int cx = winx + winw / 2;
	int cy = winy + winh / 2;
//	printf("[%d]pos = %d, %d, %d, %d\n", _i, winx, winy, winw, winh);
//	printf("[%d]center = %d, %d\n", _i, cx, cy);
//	printf("[%d]cursor : %f, %f (%d, %d)\n", _i, location.x, location.y, x, y);
	CGPoint point = CGPointMake(cx, cy);

	_ge_mac_mouse_warp_x = (winx + x) - cx;
	_ge_mac_mouse_warp_y = -((winy + y) - cy);
//	printf("[%d]warp : %d, %d\n", _i++, _ge_mac_mouse_warp_x, _ge_mac_mouse_warp_y);
	if(!warped){
		warped = true;
		CGSetLocalEventsSuppressionInterval(0.0);
		CGWarpMouseCursorPosition(point);
		CGSetLocalEventsSuppressionInterval(0.1);
	}else{
	}
}

void MacSwapBuffer(void){
	NSEvent* event;
 	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

 	keys[GEK_MWHEELUP] = false;
 	keys[GEK_MWHEELDOWN] = false;
	_ge_mac_mouse_warp_x = 0;
	_ge_mac_mouse_warp_y = 0;
	warped = false;

	while(1){
		[pool release];
		pool = [[NSAutoreleasePool alloc] init];

		event = [NSApp nextEventMatchingMask:NSAnyEventMask untilDate:[NSDate distantPast] inMode:NSDefaultRunLoopMode dequeue:YES];
//		if([event type] != 0)printf("event type : %d\n", (int)[event type]);
		if([event type] == NSMouseMoved || [event type] == NSLeftMouseDragged || [event type] == NSRightMouseDragged){
			if(warp_mode){
				MacWarpMouse([event locationInWindow].x, [event locationInWindow].y);
			}else{
				NSRect rect = [ysWnd contentRectForFrameRect:[ysWnd frame]];
				mx = [event locationInWindow].x;
				my = rect.size.height - [event locationInWindow].y;
			}
		}
		if([event type] == NSLeftMouseDown){
			keys[GEK_LBUTTON] = 1;
		}
		if([event type] == NSLeftMouseUp){
			keys[GEK_LBUTTON] = 0;
		}
		if([event type] == NSRightMouseDown){
			keys[GEK_RBUTTON] = 1;
		}
		if([event type] == NSRightMouseUp){
			keys[GEK_RBUTTON] = 0;
		}
		if([event type] == NSOtherMouseDown){
			keys[GEK_MBUTTON] = 1;
		}
		if([event type] == NSOtherMouseUp){
			keys[GEK_MBUTTON] = 0;
		}
		if([event type] == NSScrollWheel){
		//	printf("scroll : %f\n", [event deltaY]);
			if([event deltaY] >= 0.1f){
				keys[GEK_MWHEELUP] = true;
			}
			if([event deltaY] <= -0.1f){
				keys[GEK_MWHEELDOWN] = true;
			}
		}
		if([event type] == NSFlagsChanged){
  			keys[GEK_SHIFT] = [event modifierFlags] & NSShiftKeyMask ? true : false;
		}
		if([event type] == NSKeyDown){
		//	printf("down\n");
			int key = [[event characters] characterAtIndex:0];
			key = toupper(key);
		 //   printf("key down event : %c\n", key);
			keys[key] = true;
		}
		if([event type] == NSKeyUp){
			int key = [[event characters] characterAtIndex:0];
			key = toupper(key);
		//    printf("key up event : %c\n", key);
			keys[key] = false;
		}
		if(event != nil && [event type] != 0){
			[NSApp sendEvent:event];
			[NSApp updateWindows];
		}else{
			break;
		}
	}

	[pool release];	

	[[ysView openGLContext] flushBuffer];
}

void* MacNSGLGetProcAddress(const char *name){
    NSSymbol symbol;
    char *symbolName;
    symbolName = malloc (strlen (name) + 2); // 1
    strcpy(symbolName + 1, name); // 2
    symbolName[0] = '_'; // 3
    symbol = NULL;
    if (NSIsSymbolNameDefined (symbolName)) // 4
        symbol = NSLookupAndBindSymbol (symbolName);
    free (symbolName); // 5
    return symbol ? NSAddressOfSymbol (symbol) : NULL; // 6
}

void* MacCreateGLContext(){
	NSOpenGLContext* ctx = [[NSOpenGLContext alloc] initWithFormat: gl_format shareContext:[ysView openGLContext]];
	[ctx makeCurrentContext];
	return ctx;
}
void MacSetWarpMode(int en){
	warp_mode = en;
	if(en){
		NSRect rect = [ysWnd contentRectForFrameRect:[ysWnd frame]];
		int winx = (int)rect.origin.x;
		int winy = (int)(ysWnd.screen.frame.size.height - (rect.origin.y + rect.size.height));
		int winw = (int)rect.size.width;
		int winh = (int)rect.size.height;
		int cx = winx + winw / 2;
		int cy = winy + winh / 2;
		CGPoint point = CGPointMake(cx, cy);
		CGWarpMouseCursorPosition(point);
	}
}

void MacGetPressedKeys(u8* k){
	memcpy(k, keys, GE_KEYS_COUNT*sizeof(u8));
}

void MacGetMousePos(int*x, int* y){
	*x = mx;
	*y = my;
}

void geCursorVisible(bool visible){
	if(visible){
		[NSCursor unhide];
	}else{
		[NSCursor hide];
	}
}

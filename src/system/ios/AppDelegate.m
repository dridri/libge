//
//  AppDelegate.m
//  gl
//
//  Created by Adrien AUBRY on 28/01/15.
//  Copyright (c) 2015 Adrien AUBRY. All rights reserved.
//

#include <libge/libge.h>
#include "../../ge_internal.h"
#import "AppDelegate.h"

int _ge_main(int, char**);

EAGLContext* _ge_eagl_window_context = nil;
static GLKView* view = 0;
static u8 _ge_ios_keys[GE_KEYS_COUNT + 32] = { 0 };

int _ge_ios_defaultFramebuffer = 0;
int _ge_ios_colorRenderbuffer = 0;
int _ge_ios_depthRenderbuffer = 0;

int _ge_ios_framebufferWidth = 0;
int _ge_ios_framebufferHeight = 0;

int _ge_argc = 0;
char** _ge_argv = 0;

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];

	_ge_eagl_window_context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
	view = [[GLKView alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
	view.context = _ge_eagl_window_context;
	view.delegate = self;
	[self.window addSubview:view];
	
	
	CAEAGLLayer * const eaglLayer = (CAEAGLLayer*)view.layer;
	eaglLayer.opaque = YES;

	[EAGLContext setCurrentContext:_ge_eagl_window_context];

	
	glGenFramebuffers(1, &_ge_ios_defaultFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, _ge_ios_defaultFramebuffer);
	
	glGenRenderbuffers(1, &_ge_ios_colorRenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, _ge_ios_colorRenderbuffer);
	[_ge_eagl_window_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &_ge_ios_framebufferWidth);
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &_ge_ios_framebufferHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _ge_ios_colorRenderbuffer);

	glGenRenderbuffers(1, &_ge_ios_depthRenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, _ge_ios_depthRenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, _ge_ios_framebufferWidth, _ge_ios_framebufferHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _ge_ios_depthRenderbuffer);
	
	gePrintDebug(0x100, "Framebuffer : %d, %d\n", _ge_ios_framebufferWidth, _ge_ios_framebufferHeight);

    self.window.backgroundColor = [UIColor blackColor];
    [self.window makeKeyAndVisible];

 	[self performSelector:@selector(runMain:) withObject:nil afterDelay:0.2f];

    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    //Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

- (void)runMain:(id)sender
{
	
	[EAGLContext setCurrentContext:_ge_eagl_window_context];

	_ge_main(_ge_argc, _ge_argv);
}

#pragma mark - GLKViewDelegate
- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
}

- (void) touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
	CGPoint startPoint = [[touches anyObject] locationInView:view];
	float x = startPoint.x;
	float y = startPoint.y;
	x = x * libge_context->width / [[UIScreen mainScreen] bounds].size.width;
	y = y * libge_context->height / [[UIScreen mainScreen] bounds].size.height;
	libge_context->mouse_x = x;
	libge_context->mouse_y = y;
	_ge_ios_keys[GEK_LBUTTON] = 1;
}

- (void) touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
	CGPoint startPoint = [[touches anyObject] locationInView:view];
	float x = startPoint.x;
	float y = startPoint.y;
	x = x * libge_context->width / [[UIScreen mainScreen] bounds].size.width;
	y = y * libge_context->height / [[UIScreen mainScreen] bounds].size.height;
	libge_context->mouse_x = x;
	libge_context->mouse_y = y;
}

- (void) touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event
{
	CGPoint startPoint = [[touches anyObject] locationInView:view];
	float x = startPoint.x;
	float y = startPoint.y;
	x = x * libge_context->width / [[UIScreen mainScreen] bounds].size.width;
	y = y * libge_context->height / [[UIScreen mainScreen] bounds].size.height;
	libge_context->mouse_x = x;
	libge_context->mouse_y = y;
	_ge_ios_keys[GEK_LBUTTON] = 0;
}

void iOSGetPressedKeys(u8* k){
	memcpy(k, _ge_ios_keys, GE_KEYS_COUNT+32);
}


@end

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

#ifndef __LIBGE_HEADER__
#define __LIBGE_HEADER__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef WIN32
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#else
#include <malloc.h>
#endif

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#ifdef PSP
#include <pspkernel.h>
#include <pspge.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <psppower.h>
#endif
	
#include "geerror.h"
#include "getypes.h"
#include "gekeys.h"
#include "gejoystick.h"
#include "getimers.h"
#include "gefile.h"
#include "geresources.h"
#include "gesys.h"
#include "gefont.h"
#ifndef LIBGE_NOLUA
#include "gelua.h"
#endif
#include "gegui.h"
#include "gesocket.h"
#include "geutils.h"
#include "gephysics.h"
#include "geaudio.h"
#include "gegfx.h"
#include "geenums.h"
//#include "geevents.h"
#ifdef OPENCL
#include "gecl.h"
#endif

typedef struct LibGE_Context {
	int width;
	int height;
	unsigned int clear_color;
	unsigned int clear_mode;
	int ram_extended;
	int drawing_mode;
	int vsync;

	int draw_off_x;
	int draw_off_y;
	float img_stack[4098];
	ge_Image* fontbuf;

	float projection_matrix[16];
	int blend_src;
	int blend_dst;
	
	ge_Image* cursor_image;
	int (*swap_func)();

	int shaders_available;
	char* default_shaders_path;
	short mouse_x, mouse_y;
	int mouse_round;
	ge_Keys* ge_keys;
	int gpumem;
	int mem;
	int allocs, frees;
	t_ptr syscontext;
	t_ptr vidcontext;
} LibGE_Context;

LIBGE_API void geInit();
LIBGE_API void geQuit();
LIBGE_API int geCreateMainWindow(const char* title, int Width, int Height, int flags);
LIBGE_API void geSplashscreenEnable(bool enabled);
LIBGE_API void geFullscreen(bool fullscreen, int width, int height);
LIBGE_API void geWaitVsync(int enabled);
LIBGE_API LibGE_Context* geGetContext();
LIBGE_API void geSetContext(LibGE_Context* ctx);
LIBGE_API void geDebugMode(int mode);
LIBGE_API bool geDebugCritical(bool enabled);
LIBGE_API void gePrintDebug(int level, const char *format, ...);
LIBGE_API void geRegisterSystemMessageCallback(void* cb);
LIBGE_API void geCustomProcAdress(void* (*fct)(const char*));

LIBGE_API int geEnable(int what);
LIBGE_API int geDisable(int what);
LIBGE_API void geForceCap(int which, int en);
LIBGE_API void geBlendEquation(int mode);
LIBGE_API void geFrontFace(int order);

LIBGE_API void geCursorVisible(bool visible);
LIBGE_API void geCursorImage(ge_Image* img);
LIBGE_API void geCursorRoundMode(bool enabled);
LIBGE_API void geCursorPosition(int* x, int* y);
LIBGE_API void geCursorWarp(int* x, int* y);

LIBGE_API void geViewport(int x, int y, int w, int h);
LIBGE_API void geScissor(int x, int y, int w, int h);
LIBGE_API int geGetDrawingMode();
LIBGE_API int geDrawingMode(int mode);
LIBGE_API void geDrawOffset(int x, int y);

LIBGE_API void geClearColor(u32 color);
LIBGE_API u32 geClearMode(u32 mode);
LIBGE_API void geClearScreen();
LIBGE_API int geSwapBuffers();
LIBGE_API void geSwapFunction(int (*func)());
LIBGE_API int geFps();
LIBGE_API void geFpsLimit(int max);

LIBGE_API u32 geMixColors(u32 c1, u32 c2, float f);

LIBGE_API void geDrawLineScreen(int x0, int y0, int x1, int y1, u32 color);
LIBGE_API void geDrawLineImage(int x0, int y0, int x1, int y1, u32 color, ge_Image* destination);
LIBGE_API void geFillRectScreen(int x, int y, int width, int height, u32 color);
LIBGE_API void geFillRectImage(int x, int y, int width, int height, u32 color, ge_Image* destination);
LIBGE_API u32 geScreenPixel(int x, int y);
LIBGE_API u32 geImagePixel(int x, int y, ge_Image* img);

LIBGE_API void geDrawLineScreenDepth(int x0, int y0, int z0, int x1, int y1, int z1, u32 color);
LIBGE_API void geFillRectScreenDepth(int x, int y, int z, int width, int height, u32 color);

LIBGE_API void geDrawLineScreenFade(int x0, int y0, int x1, int y1, u32 color0, u32 color1);
LIBGE_API void geDrawLineScreenFadeDepth(int x0, int y0, int z0, int x1, int y1, int z1, u32 color0, u32 color1);

LIBGE_API void geDrawCircle(int x, int y, int r, u32 color);
LIBGE_API void geFillCircle(int x, int y, int r, u32 color);

// Image Loading
LIBGE_API ge_Image* geLoadImage(const char* file);
LIBGE_API ge_Image* geLoadImageOptions(const char* file, int flags);
LIBGE_API ge_Image* geLoadImageResized(const char* file, int pref_w, int pref_h);
LIBGE_API ge_Image* geLoadImageResizedOptions(const char* file, int pref_w, int pref_h, int flags);
LIBGE_API ge_Image* geCreateSurface(short width, short height, u32 color);
LIBGE_API ge_Image3D* geCreateSurface3D(short width, short height, short depth, u32 color);
LIBGE_API ge_Image* geResizeImage(ge_Image* src, int new_width, int new_height, int method);
LIBGE_API ge_Image* geDuplicateImage(ge_Image* source);
LIBGE_API ge_Image* geAnimateImage(ge_Image* _img, int n, float t);
LIBGE_API void geFlipImageVertical(ge_Image* img);
LIBGE_API void geUpdateImage(ge_Image* img);
LIBGE_API void geDeleteImage(ge_Image* img);
LIBGE_API void geFreeImage(ge_Image* image);
LIBGE_API int geGetNextPower2(int width);


// 2D Blitting
LIBGE_API void geDrawImage(int x, int y, ge_Image* img);
LIBGE_API void geDrawImageStretched(int x, int y, ge_Image* img, int width, int height);
LIBGE_API void geDrawImageRotated(int x, int y, ge_Image* img, float angle, bool centered);
LIBGE_API void geBlitImage(int x, int y, ge_Image* image, int sx, int sy, int ex, int ey, int flags);
LIBGE_API void geBlitImageStretched(int x, int y, ge_Image* img, int sx, int sy, int ex, int ey, int width, int height, int flags);
LIBGE_API void geBlitImageRotated(int x, int y, ge_Image* image, int sx, int sy, int ex, int ey, float angle, int flags);
LIBGE_API void geBlitImageNegative(int x, int y, ge_Image* image, int sx, int sy, int ex, int ey);
LIBGE_API void geBlitImageStretchedRotated(int x, int y, ge_Image* img, int sx, int sy, int ex, int ey, int width, int height, float angle, int flags);

LIBGE_API void geDrawImageDepth(int x, int y, int z, ge_Image* img);
LIBGE_API void geDrawImageDepthStretched(int x, int y, int z, ge_Image* img, int width, int height);
LIBGE_API void geDrawImageDepthRotated(int x, int y, int z, ge_Image* img, float angle, bool centered);
LIBGE_API void geBlitImageDepth(int x, int y, int z, ge_Image* image, int sx, int sy, int ex, int ey, int flags);
LIBGE_API void geBlitImageDepthStretched(int x, int y, int z, ge_Image* img, int _sx, int _sy, int ex, int ey, int width, int height, int flags);
LIBGE_API void geBlitImageDepthRotated(int x, int y, int z, ge_Image* img, int _sx, int _sy, int ex, int ey, float angle, int flags);
LIBGE_API void geBlitImageDepthStretchedRotated(int x, int y, int z, ge_Image* img, int sx, int sy, int ex, int ey, int width, int height, float angle, int flags);

LIBGE_API void geBlitImageToImage(int x, int y, ge_Image* source, int sx, int sy, int width, int height, ge_Image* destination);
LIBGE_API void geBlitImageToImageStretched(int x, int y, ge_Image* source, int sx, int sy, int ex, int ey, int width, int height, ge_Image* destination);


// Keyboard handling
LIBGE_API void geKeyboardInit();
LIBGE_API void geKeyboardFinished();
LIBGE_API void geKeyboardDescrition(const char* desc);
LIBGE_API void geKeyboardOutput(char* ptr, int max_len, int maxlines);
LIBGE_API void geKeyboardColor(u32 color);
LIBGE_API int geKeyboardUpdate();
LIBGE_API int geKeyboardIndex();
LIBGE_API void geKeyboardDefine(void* init, void* finished, void* desc, void* output, void* color, void* update, void* index);

// Threading
LIBGE_API ge_Thread* geCreateThread(const char* name, void* func, int flags);
LIBGE_API void geThreadStackSize(ge_Thread* thread, u32 size);
LIBGE_API void geThreadStart(ge_Thread* thread, int args, void* argp);
LIBGE_API void geThreadSuspend(ge_Thread* thread);
LIBGE_API void geThreadResume(ge_Thread* thread);
LIBGE_API void geThreadTerminate(ge_Thread* thread);
LIBGE_API void geThreadSleep();
LIBGE_API void geThreadExit(int ret_code);
LIBGE_API void geFreeThread(ge_Thread* thread);

// Memory
LIBGE_API void* geMalloc(int size);
LIBGE_API void* geMemalign(int size, int align);
LIBGE_API void* geRealloc(void* last, int size);
LIBGE_API void geFree(void* data);
LIBGE_API size_t geMemBlockSize(void* ptr);


// Hardware accelerated maths functions (not available on PC)
LIBGE_API float geCos(float angle);
LIBGE_API float geSin(float angle);
LIBGE_API float geCosSin(float angle, float* ret_cos, float* ret_sin);
LIBGE_API float geMult(float v1, float v2);
LIBGE_API float geDiv(float v1, float v2);
LIBGE_API float geExp(float a0);
LIBGE_API float geLog(float x);
LIBGE_API float geDistance2D(float x1, float y1, float x2, float y2);
LIBGE_API float geDistance3D(float x1, float y1, float z1, float x2, float y2, float z2);
LIBGE_API void geMatrix44Add(float* matrix, float* matrix0, float* matrix1);
LIBGE_API void geMatrix44Sub(float* matrix, float* matrix0, float* matrix1);
LIBGE_API void geMatrix44Mult(float* matrix, float* matrix0, float* matrix1);
LIBGE_API void geMatrix44Vec4Mult(float* ret, float* mat, float* vec);
LIBGE_API void geMatrixInverse(float* ret, float* base_mat, int size);
LIBGE_API void geMatrixTranspose(float* ret, float* base_mat, int size);
LIBGE_API void geNormalize(float* v);
LIBGE_API void geNormalized(double* v);
LIBGE_API void geCrossProduct(float* r, const float* a, const float* b);
LIBGE_API float geDotProduct(const ge_Vector3f* a, const ge_Vector3f* b);
LIBGE_API void geVector3FromPoints(float* v, float* a, float* b);
LIBGE_API void geVector3Mult(float* r, float* a, float* b);


// 3D Loading
LIBGE_API ge_Scene* geLoadScene(const char* file);
LIBGE_API ge_Object* geLoadObject(const char* file);
LIBGE_API int geObjectsCountInFile(const char* file);
LIBGE_API void geLoadObjectsList(const char* file, const char** list, int count, ge_Object** objs, ge_Animator* anim);
LIBGE_API void geFreeScene(ge_Scene* scene);


// 3D Graphics matrices
LIBGE_API void geMatrixMode(int mode);
LIBGE_API void geLoadIdentity();
LIBGE_API void geUpdateMatrix();
LIBGE_API void geLoadMatrix(float* m);
LIBGE_API void geMatrixMult(float* m);
LIBGE_API void geSetMatrix(float* m);
LIBGE_API float* geGetMatrix(int whitch);
LIBGE_API void geTranslate(float x, float y, float z);
LIBGE_API void geRotate(float x, float y, float z);
LIBGE_API void geScale(float x, float y, float z);
LIBGE_API void gePerspective(float fov, float aspect, float zNear, float zFar);
LIBGE_API void geOrthogonal(float left, float right, float bottom, float top, float Near, float Far);
LIBGE_API void geLookAt(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ);
LIBGE_API void geVerticalView(float x, float y, float z);
LIBGE_API void geUpdateCurrentMatrix();
LIBGE_API void geSetMatricesUpdate(int u);

LIBGE_API bool ge_IsIdentity(float* m);
LIBGE_API void ge_LoadIdentity(float* m);
LIBGE_API void ge_Translate(float* m, float x, float y, float z);
LIBGE_API void ge_Rotate(float* m, float x, float y, float z);
LIBGE_API void ge_Scale(float* m, float x, float y, float z);
LIBGE_API void ge_LookAt(float* m, float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ);
LIBGE_API void ge_Perspective(float* m, float fov, float aspect, float zNear, float zFar);

LIBGE_API void geObjectMatrixIdentity(ge_Object* obj);
LIBGE_API void geObjectMatrixLoad(ge_Object* obj, float* m);
LIBGE_API void geObjectMatrixMult(ge_Object* obj, float* m);
LIBGE_API void geObjectMatrixRotate(ge_Object* obj, float x, float y, float z);
LIBGE_API void geObjectMatrixTranslate(ge_Object* obj, float x, float y, float z);
LIBGE_API void geObjectMatrixScale(ge_Object* obj, float x, float y, float z);


//3D Camera
LIBGE_API ge_Camera* geCreateCamera();
LIBGE_API void geCameraSetMaximumAngles(ge_Camera* cam, float min, float max, int what);
LIBGE_API void geCameraLookAt(ge_Camera* cam, float x, float y, float z, float cX, float cY, float cZ);
LIBGE_API void geCameraFromAngles(ge_Camera* cam, float RotH, float RotV);
LIBGE_API void geCameraLook(ge_Camera* cam);
LIBGE_API void geCameraRotateWithMouse(ge_Camera* cam, float sens);
LIBGE_API void geCameraMoveForward(ge_Camera* cam, float sensibility);
LIBGE_API void geCameraMoveBack(ge_Camera* cam, float sensibility);
LIBGE_API void geCameraMoveLeft(ge_Camera* cam, float sensibility);
LIBGE_API void geCameraMoveRight(ge_Camera* cam, float sensibility);
LIBGE_API void geCameraMoveForwardToPoint(ge_Camera* cam, float sensibility);
LIBGE_API void geCameraMoveBackFromPoint(ge_Camera* cam, float sensibility);
LIBGE_API void geCameraFollow(ge_Camera* cam, float x, float y, float z, float up, float camz, float min_distance, float speed);
LIBGE_API void geCameraFollowMatrix(ge_Camera* cam, float* m, float up, float camz, float min_distance, float speed);
LIBGE_API void geCameraFollowAround(ge_Camera* cam, float cX, float cY, float cZ, float distance, float rotH, float rotV, float speed);


// 3D Setup
LIBGE_API ge_Framebuffer* geCreateFramebuffer(int width, int height);
LIBGE_API ge_Framebuffer* geCreateFramebufferMultisample(int width, int height, int samples);
LIBGE_API void geFreeFramebuffer(ge_Framebuffer* fbo);
LIBGE_API u32 geFramebufferReadPixel(ge_Framebuffer* fbo, int x, int y);
LIBGE_API void geFramebufferUse(ge_Framebuffer* fbo);
LIBGE_API void geSetupMultipleRenderTargets(int n, ge_Framebuffer** fbs);
LIBGE_API void geBlendFunc(int src, int dst);

// 3D Graphics
LIBGE_API void geClipPlane(float x, float y, float z, float w);
LIBGE_API void geRendererCreateContext(ge_Scene* scene, ge_Renderer* render);
LIBGE_API void geRendererUpdateContext(ge_Scene* scene, ge_Renderer* render);
LIBGE_API void geRendererUse(ge_Renderer* render);
LIBGE_API ge_Renderer* geCreateRenderer(ge_Shader* shader);
LIBGE_API void geRendererLinkObject(ge_Renderer* render, ge_Object* obj);
LIBGE_API void geRendererAddObject(ge_Renderer* render, ge_Object* obj);
LIBGE_API void geRendererUnlinkObject(ge_Renderer* render, ge_Object* obj);
LIBGE_API void geRendererRemoveObject(ge_Renderer* render, ge_Object* obj);
LIBGE_API int geRendererObjectIndex(ge_Renderer* render, const char* name);
LIBGE_API void geRendererUpdate(ge_Renderer* render);
LIBGE_API void geRenderObjects(ge_Renderer* render);
LIBGE_API ge_Renderer* geSceneRenderer(ge_Scene* scene, const char* name);
LIBGE_API void geSceneUpdateMatrices(ge_Scene* scene);
LIBGE_API void geSceneDraw(ge_Scene* scene);
LIBGE_API void geObjectDraw(ge_Object* obj);
LIBGE_API void geTextureImage(int unit, ge_Image* img);
LIBGE_API void geTextureMode(ge_Image* img, int mode);
LIBGE_API void geTextureWrap(ge_Image* img, int wrapx, int wrapy);
LIBGE_API void geTextureCopyBuffer(ge_Image* target, int ofsx, int ofsy, int sx, int sy, int w, int h);
LIBGE_API void geDrawArray(int mode, int start, int num);
LIBGE_API void geDrawArrayInstanced(int mode, int start, int num, int nInstances);

// 3D Lights
LIBGE_API ge_Light* geCreateLight(float x, float y, float z, u32 diffuse, u32 ambient);
LIBGE_API void geLightSpot(ge_Light* light, float innerAngler, float exponent);
LIBGE_API void geLightInitShadow(ge_Light* light, ge_Shader* shader, int size, int depth, float size_factor);
LIBGE_API void geLightComputeShadow(ge_Light* light, ge_Camera* cam, void (*render)(void*), void* udata);
LIBGE_API void geRendererLinkLight(ge_Renderer* render, ge_Light* light);


// 3D Animations
LIBGE_API ge_AnimSampler* geSceneAnimationSampler(ge_Scene* scene, const char* target_name);
LIBGE_API void geAnimationStart(ge_AnimSampler* sampler);
LIBGE_API void geAnimationStop(ge_AnimSampler* sampler);
LIBGE_API void geRendererAnimate(ge_Renderer* render);
LIBGE_API void geSceneAnimate(ge_Scene* scene);

LIBGE_API ge_Particles* geCreateParticles(float x, float y, float z, float radius, float velocity, float life_time, int nParticles, ge_Image* texture);
LIBGE_API void geParticlesLoop(ge_Particles* particles, bool active);
LIBGE_API void geParticlesDirection(ge_Particles* particles, float dirX, float dirY, float dirZ);
LIBGE_API void geParticlesSize(ge_Particles* particles, float w, float h);
LIBGE_API void geParticlesLifeSeed(ge_Particles* particles, float seed);
LIBGE_API void geParticlesUpdate(ge_Particles* particles);
LIBGE_API void geParticlesDraw(ge_Particles* particles);

// Shaders
LIBGE_API ge_Shader* geCreateShader();
LIBGE_API void geFreeShader(ge_Shader* shader);
LIBGE_API void geShaderLoadVertexSource(ge_Shader* shader, const char* file);
LIBGE_API void geShaderLoadTessControlSource(ge_Shader* shader, const char* file);
LIBGE_API void geShaderLoadTessEvaluationSource(ge_Shader* shader, const char* file);
LIBGE_API void geShaderLoadGeometrySource(ge_Shader* shader, const char* file);
LIBGE_API void geShaderLoadFragmentSource(ge_Shader* shader, const char* file);
LIBGE_API ge_Shader* geShaderUse(ge_Shader* shader);
LIBGE_API int geShaderUniformID(ge_Shader* shader, const char* name);
LIBGE_API int geShaderAttribID(ge_Shader* shader, const char* name);
LIBGE_API void geShaderUniform1i(int id, int v1);
LIBGE_API void geShaderUniform2i(int id, int v1, int v2);
LIBGE_API void geShaderUniform3i(int id, int v1, int v2, int v3);
LIBGE_API void geShaderUniform4i(int id, int v1, int v2, int v3, int v4);
LIBGE_API void geShaderUniform1f(int id, float v1);
LIBGE_API void geShaderUniform2f(int id, float v1, float v2);
LIBGE_API void geShaderUniform3f(int id, float v1, float v2, float v3);
LIBGE_API void geShaderUniform4f(int id, float v1, float v2, float v3, float v4);
LIBGE_API void geShaderUniform1fv(int id, int n, float* v);
LIBGE_API void geShaderUniform2fv(int id, int n, float* v);
LIBGE_API void geShaderUniform3fv(int id, int n, float* v);
LIBGE_API void geShaderUniform4fv(int id, int n, float* v);
LIBGE_API void geShaderUniform1iv(int id, int n, int* v);
LIBGE_API void geShaderUniform2iv(int id, int n, int* v);
LIBGE_API void geShaderUniform3iv(int id, int n, int* v);
LIBGE_API void geShaderUniform4iv(int id, int n, int* v);
LIBGE_API void geVertexAttribPointer(int id, int size, int type, bool normalized, int stride, void* data);
LIBGE_API ge_Shader* geForceShader(ge_Shader* sh);



// PSP Macros/Functions
#ifdef PSP
#define GE_PSP_INFO(name, v_major, v_minor, mem_size_kb) PSP_MODULE_INFO(name, 0, v_major, v_minor); PSP_HEAP_SIZE_KB(mem_size_kb)
LIBGE_API void geSwizzle(ge_Image* image);
LIBGE_API void geSwizzleModule(ge_Image* image);
LIBGE_API void geSwizzleVMEM(ge_Image* image);
LIBGE_API void geUnswizzle(ge_Image* img);
LIBGE_API void geMoveImageToVram(ge_Image* image);
LIBGE_API void geTextureScale(ge_Image* img, float u, float v);
#else
#define GE_PSP_INFO(name, v_major, v_minor, mem_size_kb) ;
#define geSwizzle(image) ;
#define geSwizzleModule(image) ;
#define geSwizzleVMEM(image) ;
#define geMoveImageToVram(image) ;
#endif

// PSP/PSV/PS3 Functions
#define GE_PSV_INFO(name, v_major, v_minor, mem_size_kb) ;
#define GE_PS3_INFO(name, v_major, v_minor, mem_size_kb) ;
LIBGE_API void geModuleProgram(bool en);
LIBGE_API void geExtendRamToDisk(bool enabled);
LIBGE_API u32* geGetDrawBuffer();
LIBGE_API u32* geGetDisplayBuffer();

#ifdef __cplusplus
}
#endif
#endif //LibGE HEADER

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

#ifndef __GE_ENUMS__
#define __GE_ENUMS__

#ifdef __cplusplus
extern "C" {
#endif


#ifdef PSP
	#define U32_COLORS
	#define NO_3D_TEXCOORDS
#endif

#define GE_DEBUG_INFO 1
#define GE_DEBUG_WARN 2
#define GE_DEBUG_ERROR 4
#define GE_DEBUG_PRINT 8
#define GE_DEBUG_LOGFILE 16
#define GE_DEBUG_ALL (GE_DEBUG_INFO|GE_DEBUG_WARN|GE_DEBUG_ERROR|GE_DEBUG_PRINT|GE_DEBUG_LOGFILE)

//Flags for geCreateMainWindow
#define GE_WINDOW_FULLSCREEN 0x10
#define GE_WINDOW_FULLWINDOW 0x40
#define GE_WINDOW_RESIZABLE 0x80
#define GE_WINDOW_MSAA2X 0x10000
#define GE_WINDOW_MSAA4X 0x20000
#define GE_WINDOW_MSAA8X 0x40000

//Events types
#define GE_EVENT_WINDOW_CLOSE 0x01
#define GE_EVENT_WINDOW_RESIZE 0x02
#define GE_EVENT_KEY_PRESS 0x04
#define GE_EVENT_KEY_RELEASE 0x08
#define GE_EVENT_MOUSE_MOVE 0x10

//Flags for threads
#define GE_THREAD_STOPPED 0x00
#define GE_THREAD_RUNNING 0x01
#define GE_THREAD_PAUSED 0x02
#define GE_THREAD_PRIORITY_LOWEST 1
#define GE_THREAD_PRIORITY_BELOW_NORMAL 2
#define GE_THREAD_PRIORITY_NORMAL 4
#define GE_THREAD_PRIORITY_ABOVE_NORMAL 8
#define GE_THREAD_PRIORITY_HIGHEST 16
#define GE_THREAD_PRIORITY_TIME_CRITICAL 32
#define GE_THREAD_GRAPHIC_CONTEXT 128
#define GE_THREAD_SHARED_GRAPHIC_CONTEXT 256

// Audio
#define GE_AUDIO_TRANSFORM_FFT 0x00000001
#define GE_AUDIO_TRANSFORM_PASS2 0x40000000
#define GE_AUDIO_VISUALIZER_PCM 0
#define GE_AUDIO_VISUALIZER_FFT 1
#define GE_AUDIO_VISUALIZER_FIRE 2
#define GE_AUDIO_VISUALIZER_PLAIN 3
#define GE_AUDIO_VISUALIZER_PCM_FLOAT 10
#define GE_AUDIO_VISUALIZER_REVERSE 0x01000000
#define GE_AUDIO_VISUALIZER_FIX_COLORS 0x02000000

//Lua
#define GE_LUA_EXECUTION_MODE_NORMAL 1
#define GE_LUA_EXECUTION_MODE_PARALLEL 2

//Files types
#define GE_AUDIO_FILE_WAV 0xaf0
#define GE_AUDIO_FILE_MP3 0xaf1
#define GE_OBJECT_FILE_WAVEFRONT 1
#define GE_OBJECT_FILE_COLLADA 2
#define GE_OBJECT_FILE_GEOBJECT 4

//Drawing modes
#define GE_DRAWING_MODE_2D  0x01
#define GE_DRAWING_MODE_3D  0x02
#define GE_DRAWING_2D_DEPTH 0x04

//Flags for able
#define GE_MOUSE_VISIBLE 0x00

// For keyboards
#define GE_KEYBOARD_SONY 0x00
#define GE_KEYBOARD_DANZEFF 0x01
#define GE_KEYBOARD_USER 0x20

// Blit image flags
#define GE_BLIT_NOALPHA 0x10
#define GE_BLIT_NOOFFSET 0x20
#define GE_BLIT_CENTERED 0x40
#define GE_BLIT_VFLIP 0x80

//Image flags
#define GE_IMAGE_SWIZZLED 0x01
#define GE_IMAGE_VRAM 0x02
#define GE_IMAGE_NO_MIPMAPS 0x04
#define GE_IMAGE_BUMP 0x08
#define GE_IMAGE_3D 0x10
#define GE_IMAGE_RECTANGLE 0x20
#define GE_IMAGE_ARRAY 0x40
#define GE_IMAGE_CLAMPX 0x1000
#define GE_IMAGE_CLAMPY 0x2000

// Renderer flags
#define GE_RENDERER_WATER 0x1000

//Images effects
#define GE_EFFECT_ANTI_GRADATION 0xe0

// Clouds effects
#define GE_CLOUD_TYPE_HIGH_LEVEL 0x0
#define GE_CLOUD_TYPE_MID_LEVEL 0x1
#define GE_CLOUD_TYPE_LOW_LEVEL 0x2

// Animations
#define GE_ANIMATION_MODE_ONCE	0xa01
#define GE_ANIMATION_MODE_LOOP	0xa02
#define GE_ANIMATION_LINEAR		0xa41

//Color & types macros
#undef RGB
#define ABGR(a,b,g,r) (((a) << 24)|((b) << 16)|((g) << 8)|(r))
#define ARGB(a,r,g,b) ABGR((a),(b),(g),(r))
#define RGBA(r,g,b,a) ARGB((a),(r),(g),(b))
#define RGB(r,g,b) RGBA(r,g,b,255)
#define RGBf(r,g,b) RGBA((u8)((r)*255.0f),(u8)((g)*255.0f),(u8)((b)*255.0f),255)
#define RGBAf(r,g,b,a) RGBA((u8)((r)*255.0f),(u8)((g)*255.0f),(u8)((b)*255.0f),(u8)((a)*255.0f))
#define fRGBA(f) RGBAf(f[0], f[1], f[2], f[3])
#define R(color) (u8)(color)
#define G(color) (u8)(color>>8)
#define B(color) (u8)(color>>16)
#define A(color) (u8)(color>>24)
#define Rf(color) (((float)R(color))/255.0)
#define Gf(color) (((float)G(color))/255.0)
#define Bf(color) (((float)B(color))/255.0)
#define Af(color) (((float)A(color))/255.0)

#ifdef U32_COLORS
	#define SET_COLOR_RGBA(c,r,g,b,a)c=RGBA(r,g,b,a)
	#define SET_COLOR_RGBAf(c,r,g,b,a)c=RGBAf(r,g,b,a)
	#define SET_COLOR(c,i)c=i;
	#define SET_COLOR_fRGBA(c,r,g,b,a)c[0]=r; c[1]=g; c[2]=b; c[3]=a
#else
	#define SET_COLOR_RGBA(c,r,g,b,a)c[0]=r/255.0; c[1]=g/255.0; c[2]=b/255.0; c[3]=a/255.0
	#define SET_COLOR_RGBAf(c,r,g,b,a)c[0]=r; c[1]=g; c[2]=b; c[3]=a
	#define SET_COLOR(c,i)c[0]=Rf(i); c[1]=Gf(i); c[2]=Bf(i); c[3]=Af(i)
	#define SET_COLOR_fRGBA(c,r,g,b,a)c[0]=r; c[1]=g; c[2]=b; c[3]=a
#endif
	

#ifndef M_PI
	#define M_PI		3.14159265358979323846
#endif

#define GE_FOG_LINEAR 0
#define GE_FOG_EXP2 1
#define GE_FOG_EXP 2

#define GE_CAMERA_HORIZONTAL 0xC3
#define GE_CAMERA_VERTICAL 0xC4

#define GE_LIGHT_TYPE_DIRECTIONNAL 0
#define GE_LIGHT_TYPE_OMNIDIRECTIONNAL 1
#define GE_LIGHT_TYPE_SPOT 2

#define GE_LIGHT_HAVE_SHADOW 0x1
#define GE_LIGHT_SHADOW_PASS_DONE 0x80000000

#ifdef PSP
	#define GE_CLEAR_COLOR_BUFFER 1
	#define GE_CLEAR_STENCIL_BUFFER 2
	#define GE_CLEAR_DEPTH_BUFFER 4
	#define GE_CLEAR_FAST 16

	#define GE_POINTS 0
	#define GE_LINES 1
	#define GE_LINE_STRIP 2
	#define GE_TRIANGLES 3
	#define GE_TRIANGLE_STRIP 4
	#define GE_TRIANGLE_FAN 5
	#define GE_SPRITES 6
	#define GE_PATCHES GE_TRIANGLES

	#define GE_CW 0
	#define GE_CCW 1

	#define GE_FLAT 0
	#define GE_SMOOTH 1

	#define GE_AMBIENT 1
	#define GE_DIFFUSE 2
	#define GE_SPECULAR 4

	#define GE_PSM_5650 0
	#define GE_PSM_5551 1
	#define GE_PSM_4444 2
	#define GE_PSM_8888 3
	#define GE_PSM_T4 4
	#define GE_PSM_T8 5
	#define GE_PSM_T16 6
	#define GE_PSM_T32 7
	#define GE_PSM_DXT1 8
	#define GE_PSM_DXT3 9
	#define GE_PSM_DXT5 10

	#define GE_NEAREST 0
	#define GE_LINEAR 1
	#define GE_NEAREST_MIPMAP_NEAREST 4
	#define GE_LINEAR_MIPMAP_NEAREST 5
	#define GE_NEAREST_MIPMAP_LINEAR 6
	#define GE_LINEAR_MIPMAP_LINEAR 7

	#define GE_REPEAT 0
	#define GE_CLAMP 1

	#define GE_TFX_MODULATE 0
	#define GE_TFX_DECAL 1
	#define GE_TFX_BLEND 2
	#define GE_TFX_REPLACE 3
	#define GE_TFX_ADD 4

	/* Texture Color Component */
	#define GE_TCC_RGB 0
	#define GE_TCC_RGBA 1

	/* Blending Op */
	#define GE_ADD 0
	#define GE_SUBTRACT 1
	#define GE_REVERSE_SUBTRACT 2
	#define GE_MIN 3
	#define GE_MAX 4
	#define GE_ABS 5

	/* Blending Factor */
	#define GE_SRC_COLOR 0
	#define GE_ONE_MINUS_SRC_COLOR 1
	#define GE_SRC_ALPHA 2
	#define GE_ONE_MINUS_SRC_ALPHA 3
	#define GE_DST_COLOR 0
	#define GE_ONE_MINUS_DST_COLOR 1
	#define GE_DST_ALPHA 4
	#define GE_ONE_MINUS_DST_ALPHA 5
	#define GE_FIX 10

	/* Test Function */
	#define GE_NEVER 0
	#define GE_ALWAYS 1
	#define GE_EQUAL 2
	#define GE_NOTEQUAL 3
	#define GE_LESS 4
	#define GE_LEQUAL 5
	#define GE_GREATER 6
	#define GE_GEQUAL 7

	#define GE_COLOR_5650 16
	#define GE_COLOR_5551 20
	#define GE_COLOR_4444 24
	#define GE_COLOR_8888 28

	#define GE_VERTEX_8BIT 128
	#define GE_VERTEX_16BIT 256
	#define GE_VERTEX_32BITF 384

	#define GE_NORMAL_8BIT 32
	#define GE_NORMAL_16BIT 64
	#define GE_NORMAL_32BITF 96

	#define GE_TEXTURE_8BIT 1
	#define GE_TEXTURE_16BIT 2
	#define GE_TEXTURE_32BITF 3

	#define GE_TRANSFORM_3D 0
	#define GE_TRANSFORM_2D 0x800000

	#define GE_MATRIX_MODEL 0
	#define GE_MATRIX_VIEW 1
	#define GE_MATRIX_PROJECTION 2
	#define GE_MATRIX_TEXTURE 3

	#define GE_STATIC_DRAW 0
	#define GE_STREAM_DRAW 0
	#define GE_DYNAMIC_DRAW 0

	#define GE_UNSIGNED_BYTE 0
	#define GE_BYTE 1
	#define GE_SHORT 2
	#define GE_INT 3
	#define GE_FLOAT 4
	#define GE_DOUBLE 5

#else
#ifdef PLATFORM_android
#include <GLES/gl.h>
#include <GLES2/gl2.h>
#elif PLATFORM_mac
#define GL3_PROTOTYPES 1
#include <OpenGL/gl3.h>
//#include <OpenGL/gl3ext.h>
//#include <GL/glext.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif

	#define GE_CLEAR_COLOR_BUFFER GL_COLOR_BUFFER_BIT
	#define GE_CLEAR_STENCIL_BUFFER GL_STENCIL_BUFFER_BIT
	#define GE_CLEAR_DEPTH_BUFFER GL_DEPTH_BUFFER_BIT
	#define GE_CLEAR_FAST 0

	#define GE_BLEND GL_BLEND
	#define GE_CULL_FACE GL_CULL_FACE

	#define GE_DEPTH_TEST GL_DEPTH_TEST

	#define GE_CW GL_CW
	#define GE_CCW GL_CCW

	#define GE_MIN 0x8007
	#define GE_MAX 0x8008
	#define GE_FUNC_ADD 0x8006
	#define GE_FUNC_SUBTRACT 0x800A
	#define GE_FUNC_REVERSE_SUBTRACT 0x800B

	#define GE_DEFAULT -1
	#define GE_ZERO GL_ZERO
	#define GE_ONE GL_ONE
	#define GE_SRC_COLOR GL_SRC_COLOR
	#define GE_ONE_MINUS_SRC_COLOR GL_ONE_MINUS_SRC_COLOR
	#define GE_DST_COLOR GL_DST_COLOR
	#define GE_ONE_MINUS_DST_COLOR GL_ONE_MINUS_DST_COLOR
	#define GE_SRC_ALPHA GL_SRC_ALPHA
	#define GE_ONE_MINUS_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	#define GE_DST_ALPHA GL_DST_ALPHA
	#define GE_ONE_MINUS_DST_ALPHA GL_ONE_MINUS_DST_ALPHA
	#define GE_CONSTANT_COLOR GL_CONSTANT_COLOR
	#define GE__ONE_MINUS_CONSTANT_COLOR GL_ONE_MINUS_CONSTANT_COLOR
	#define GE__CONSTANT_ALPHA GL_CONSTANT_ALPHA
	#define GE_ONE_MINUS_CONSTANT_ALPHA GL_ONE_MINUS_CONSTANT_ALPHA

	#define GE_LINEAR GL_LINEAR
	#define GE_NEAREST GL_NEAREST

#ifndef GL_CLAMP
	#define GL_CLAMP 0x290
#endif
#ifndef GL_CLAMP_TO_EDGE
	#define GL_CLAMP_TO_EDGE 0x812F
#endif
	#define GE_REPEAT GL_REPEAT
	#define GE_CLAMP GL_CLAMP
	#define GE_CLAMP_TO_EDGE GL_CLAMP_TO_EDGE

	#define GE_MATRIX_MODEL 0
	#define GE_MATRIX_VIEW 1
	#define GE_MATRIX_PROJECTION 2
	#define GE_MATRIX_NORMAL 3
	#define GE_MATRIX_MODELVIEW 4
	#define GE_MATRIX_TEXTURE0 5
	#define GE_MATRIX_TEXTURE1 6
	#define GE_MATRIX_TEXTURE2 7
	#define GE_MATRIX_TEXTURE3 8
	#define GE_MATRIX_TEXTURE4 9
	#define GE_MATRIX_TEXTURE5 10
	#define GE_MATRIX_TEXTURE6 11
	#define GE_MATRIX_TEXTURE7 12
	#define GE_MATRIX_SUBMODEL 13

	#define GE_POINTS GL_POINTS
	#define GE_TRIANGLES GL_TRIANGLES
	#define GE_TRIANGLE_STRIP GL_TRIANGLE_STRIP
	#define GE_LINES GL_LINES
	#define GE_LINE_STRIP GL_LINE_STRIP
#ifdef ANDROID
	#define GE_PATCHES -1
#else
	#define GE_PATCHES GL_PATCHES
#endif

	#define GE_UNSIGNED_BYTE GL_UNSIGNED_BYTE
	#define GE_BYTE GL_BYTE
	#define GE_SHORT GL_SHORT
	#define GE_INT GL_INT
	#define GE_FLOAT GL_FLOAT
	#define GE_DOUBLE GL_DOUBLE

	#define GE_STATIC_DRAW GL_STATIC_DRAW
	#define GE_STREAM_DRAW GL_STREAM_DRAW
	#define GE_DYNAMIC_DRAW GL_DYNAMIC_DRAW
#endif

#ifdef __cplusplus
}
#endif
#endif

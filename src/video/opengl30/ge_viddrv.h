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

#ifndef __GE_GL_H__
#define __GE_GL_H__

#ifdef PLATFORM_mac
//#define GL3_PROTOTYPES 1
#include <OpenGL/gl3.h>
//#include <OpenGL/gl3ext.h>
#include <GL/glext.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif

#ifdef WIN32
#include <GL/wglext.h>
#endif
#include "getypes.h"

#ifndef GLAPIENTRY
#define GLAPIENTRY APIENTRY
#endif
#define BUFFER_OFFSET(a) ((char*)NULL + (a))
/*
#ifndef GLsizeiptrARB
typedef ptrdiff_t GLsizeiptrARB;
#endif
#ifndef GLintptrARB
typedef ptrdiff_t GLintptrARB;
#endif
*/
void* geglImportFunction(const char* func_name);
int geCheckExtensionAvailable(const char* name);
void UpdateGlTexture(ge_Image*, int);

typedef struct LibGE_VideoContext {
	ge_Shader* shader2d;
	int loc_textured;
} LibGE_VideoContext;

LibGE_VideoContext* _ge_GetVideoContext();

#ifndef GL_INTENSITY
#define GL_INTENSITY 0x8049
#endif

#define define_proc(base, name) \
	PFNGL##base##PROC name
/*
//Misc
define_proc(CLEAR, glClear);
define_proc(CLEARCOLOR, glClearColor);
define_proc(GETSTRING, glGetString);
define_proc(GETERROR, glGetError);
define_proc(ENABLE, glEnable);
define_proc(DISABLE, glDisable);
define_proc(VIEWPORT, glViewport);
define_proc(SCISSOR, glScissor);
define_proc(FRONTFACE, glFrontFace);
define_proc(CULLFACE, glCullFace);
define_proc(POLYGONMODE, glPolygonMode);
define_proc(LINEWIDTH, glLineWidth);

//Drawing
define_proc(DRAWARRAYS, glDrawArrays);
define_proc(DRAWELEMENTS, glDrawElements);

//Texturing
define_proc(GENTEXTURES, glGenTextures);
define_proc(DELETETEXTURES, glDeleteTextures);
define_proc(BINDTEXTURE, glBindTexture);
define_proc(TEXIMAGE2D, glTexImage2D);
define_proc(TEXSUBIMAGE2D, glTexSubImage2D);
define_proc(COPYTEXSUBIMAGE2D, glCopyTexSubImage2D);
define_proc(TEXIMAGE3D, glTexImage3D);
define_proc(TEXSUBIMAGE3D, glTexSubImage3D);
define_proc(TEXPARAMETERI, glTexParameteri);
define_proc(TEXPARAMETERF, glTexParameterf);
define_proc(ACTIVETEXTURE, glActiveTexture);

//Blending
define_proc(BLENDFUNC, glBlendFunc);
define_proc(BLENDEQUATION, glBlendEquation);
define_proc(BLENDCOLOR, glBlendColor);

//Draw buffers
define_proc(DEPTHFUNC, glDepthFunc);
define_proc(DEPTHMASK, glDepthMask);
define_proc(DEPTHRANGE, glDepthRange);
define_proc(CLEARDEPTH, glClearDepth);
define_proc(COLORMASK, glColorMask);
*/

#ifdef WIN32
define_proc(BLENDEQUATION, glBlendEquation);
define_proc(TEXIMAGE3D, glTexImage3D);
define_proc(TEXSUBIMAGE3D, glTexSubImage3D);
define_proc(TEXIMAGE2DMULTISAMPLE, glTexImage2DMultisample);
define_proc(GENERATEMIPMAP, glGenerateMipmap);
#endif

#ifndef PLATFORM_mac
//Texturing
define_proc(ACTIVETEXTURE, _glActiveTexture);
#define glActiveTexture _glActiveTexture

//Vertex-buffuring
define_proc(GENVERTEXARRAYS, glGenVertexArrays);
define_proc(BINDVERTEXARRAY, glBindVertexArray);
define_proc(ENABLEVERTEXATTRIBARRAY, glEnableVertexAttribArray);
define_proc(DISABLEVERTEXATTRIBARRAY, glDisableVertexAttribArray);
define_proc(VERTEXATTRIBPOINTER, glVertexAttribPointer);
define_proc(VERTEXATTRIBIPOINTER, glVertexAttribIPointer);

//Buffering
define_proc(GENBUFFERS, glGenBuffers);
define_proc(DELETEBUFFERS, glDeleteBuffers);
define_proc(BINDBUFFER, glBindBuffer);
define_proc(BUFFERDATA, glBufferData);
define_proc(BUFFERSUBDATA, glBufferSubData);
define_proc(GETBUFFERPARAMETERIV, glGetBufferParameteriv);
define_proc(BLITFRAMEBUFFER, glBlitFramebuffer);

//Render buffer
define_proc(GENRENDERBUFFERS, glGenRenderbuffers);
define_proc(BINDRENDERBUFFER, glBindRenderbuffer);
define_proc(RENDERBUFFERSTORAGEMULTISAMPLE, glRenderbufferStorageMultisample);
define_proc(FRAMEBUFFERRENDERBUFFER, glFramebufferRenderbuffer);

//Framebuffer
define_proc(GENFRAMEBUFFERS, glGenFramebuffers);
define_proc(DELETEFRAMEBUFFERS, glDeleteFramebuffers);
define_proc(BINDFRAMEBUFFER, glBindFramebuffer);
define_proc(FRAMEBUFFERTEXTURE, glFramebufferTexture);
define_proc(FRAMEBUFFERTEXTURE2D, glFramebufferTexture2D);
define_proc(FRAMEBUFFERTEXTURE3D, glFramebufferTexture3D);
define_proc(FRAMEBUFFERTEXTURELAYER, glFramebufferTextureLayer);
define_proc(DRAWBUFFERS, glDrawBuffers);

//Shader
define_proc(CREATESHADER, glCreateShader);
define_proc(SHADERSOURCE, glShaderSource);
define_proc(COMPILESHADER, glCompileShader);
define_proc(ATTACHSHADER, glAttachShader);
define_proc(GETSHADERINFOLOG, glGetShaderInfoLog);
define_proc(DELETESHADER, glDeleteShader);
define_proc(DELETEPROGRAM, glDeleteProgram);

//Shader program
define_proc(CREATEPROGRAM, glCreateProgram);
define_proc(LINKPROGRAM, glLinkProgram);
define_proc(USEPROGRAM, glUseProgram);
define_proc(GETPROGRAMINFOLOG, glGetProgramInfoLog);

//Shader communication
define_proc(BINDATTRIBLOCATION, glBindAttribLocation);
define_proc(GETUNIFORMLOCATION, glGetUniformLocation);
define_proc(GETATTRIBLOCATION, glGetAttribLocation);
define_proc(UNIFORM1I, glUniform1i);
define_proc(UNIFORM2I, glUniform2i);
define_proc(UNIFORM3I, glUniform3i);
define_proc(UNIFORM4I, glUniform4i);
define_proc(UNIFORM1F, glUniform1f);
define_proc(UNIFORM2F, glUniform2f);
define_proc(UNIFORM3F, glUniform3f);
define_proc(UNIFORM4F, glUniform4f);
define_proc(UNIFORM1FV, glUniform1fv);
define_proc(UNIFORM2FV, glUniform2fv);
define_proc(UNIFORM3FV, glUniform3fv);
define_proc(UNIFORM4FV, glUniform4fv);
define_proc(UNIFORM1IV, glUniform1iv);
define_proc(UNIFORM2IV, glUniform2iv);
define_proc(UNIFORM3IV, glUniform3iv);
define_proc(UNIFORM4IV, glUniform4iv);
define_proc(UNIFORMMATRIX3FV, glUniformMatrix3fv);
define_proc(UNIFORMMATRIX4FV, glUniformMatrix4fv);
define_proc(GETUNIFORMFV, glGetUniformfv);
define_proc(VERTEXATTRIBPOINTER, glVertexAttribPointer);
#endif // PLATFORM_mac

//Tesselation
define_proc(PATCHPARAMETERI, glPatchParameteri);

#endif //__GE_GL_H__

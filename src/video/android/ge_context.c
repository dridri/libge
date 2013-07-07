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
#include "ge_viddrv.h"

LibGE_VideoContext* _ge_GetVideoContext(){
	return (LibGE_VideoContext*)libge_context->vidcontext;
}

void geGraphicsInit(){
	LibGE_VideoContext* ctx = (LibGE_VideoContext*)geMalloc(sizeof(LibGE_VideoContext));
	libge_context->vidcontext = (unsigned long)ctx;
	libge_context->gpumem += 2 * libge_context->width * libge_context->height * sizeof(u32);

	ctx->shader2d = geCreateShader();
	geShaderLoadVertexSource(ctx->shader2d, _ge_BuildPath(libge_context->default_shaders_path, "generic_2d.vert"));
	geShaderLoadFragmentSource(ctx->shader2d, _ge_BuildPath(libge_context->default_shaders_path, "generic_2d.frag"));
	ctx->loc_textured = geShaderUniformID(ctx->shader2d, "textured");
	glUseProgram(ctx->shader2d->programId);
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(ctx->shader2d->programId, "ge_Texture"), 0);
	glUseProgram(0);
	
	glViewport(0, 0, libge_context->width, libge_context->height);
	
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_FRONT);
	
	glScissor(0, 0, libge_context->width, libge_context->height);
	glEnable(GL_SCISSOR_TEST);
	
	//libge_context->fontbuf = geCreateSurface(libge_context->width, libge_context->height, 0x00000000);
}

int geEnable(int what){
	int ret = glIsEnabled(what);
	glEnable(what);
	return ret;
}

int geDisable(int what){
	int ret = glIsEnabled(what);
	glDisable(what);
	return ret;
}

int geDrawingMode(int mode){
	int last = libge_context->drawing_mode;
	if(mode != libge_context->drawing_mode){
		if(mode & GE_DRAWING_MODE_2D){
			glActiveTexture(GL_TEXTURE0);
			glEnable(GL_TEXTURE_2D);
			
			glDisable(GL_CULL_FACE);
		
			glEnable(GL_ALPHA_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			
			geMatrixMode(GE_MATRIX_PROJECTION);
			geLoadIdentity();
			geOrthogonal(0.0, libge_context->width, libge_context->height, 0.0, -2049.0, 2049.0);
			geMatrixMode(GE_MATRIX_VIEW);
			geLoadIdentity();
			geMatrixMode(GE_MATRIX_MODEL);
			geLoadIdentity();
			geMatrixMode(GE_MATRIX_MODELVIEW);
			geLoadIdentity();
			geMatrixMode(GE_MATRIX_TEXTURE0);
			geLoadIdentity();

			if(mode & GE_DRAWING_2D_DEPTH){
				glEnable(GL_DEPTH_TEST);
				geClearMode(GE_CLEAR_COLOR_BUFFER | GE_CLEAR_DEPTH_BUFFER);
			}else{
				glDisable(GL_DEPTH_TEST);
				geClearMode(GE_CLEAR_COLOR_BUFFER);
			}
		
			geShaderUse(_ge_GetVideoContext()->shader2d);
			geUpdateMatrix();
		}
		if(mode & GE_DRAWING_MODE_3D){
			glEnable(GL_CULL_FACE);
			glFrontFace(GL_CW);
			glEnable(GL_DEPTH_TEST);
			geClearMode(GE_CLEAR_COLOR_BUFFER | GE_CLEAR_DEPTH_BUFFER);

			geMatrixMode(GE_MATRIX_PROJECTION);
			geLoadIdentity();
			if(libge_context->projection_matrix[0] == (float)0xFFFFFFFF){
				gePerspective(60.0, (float)libge_context->width / (float)libge_context->height, 1.0, 10000.0);
			}else{
				geLoadMatrix(libge_context->projection_matrix);
			}
		}
	}
	libge_context->drawing_mode = mode;
	return last;
}

void geClearColor(u32 color){
	libge_context->clear_color = color;
	glClearColor(Rf(color), Gf(color), Bf(color), Af(color));
}

u32 geClearMode(u32 mode){
	u32 last = libge_context->clear_mode;
	libge_context->clear_mode = mode;
	return last;
}

void geClearScreen(){
	glDepthMask(true);
	glClear(libge_context->clear_mode);
//	if(libge_context->drawing_mode & GE_DRAWING_2D_DEPTH){
		memset(libge_context->img_stack, 0x0, sizeof(float)*4098);
//	}
}

void geViewport(int x, int y, int w, int h){
	glViewport(x, y, w, h);
}

void geScissor(int x, int y, int w, int h){
	glScissor(x, y, w, h);
}

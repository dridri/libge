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

static char _ge_shader_ogl3_generic_2d_vert[] = 
	"#define NO_DEFAULT_INCLUDE\n"
	"in vec3 ge_VertexTexcoord;\n"
	"in vec4 ge_VertexColor;\n"
	"in vec3 _ge_VertexPosition;\n"
	"uniform mat4 ge_MatrixProjection;\n"
	"smooth out vec4 ge_Color;\n"
	"smooth out vec2 ge_TexCoord0;\n"
	"void main(){\n"
	"	ge_Color = ge_VertexColor;\n"
	"	ge_TexCoord0 = ge_VertexTexcoord.st;\n"
	"	gl_Position = ge_MatrixProjection * vec4(_ge_VertexPosition, 1.0);\n"
	"}\n";

static char _ge_shader_ogl3_generic_2d_frag[] = 
	"#define NO_DEFAULT_INCLUDE\n"
	"smooth in vec4 ge_Color;\n"
	"smooth in vec2 ge_TexCoord0;\n"
	"uniform sampler2D ge_Texture;\n"
	"uniform float ge_HasTexture;\n"
	"smooth out vec4 ge_FragColor;\n"
	"void main(){\n"
	"	ge_FragColor = ge_Color * (texture(ge_Texture, ge_TexCoord0) + vec4(1.0-ge_HasTexture));\n"
	"}\n";

struct {
	int used;
	int cap;
	int state;
} _ge_force_caps[32] = { { false, 0, 0 } };


LibGE_VideoContext* _ge_GetVideoContext(){
	return (LibGE_VideoContext*)libge_context->vidcontext;
}

void geGraphicsInit(){
	LibGE_VideoContext* ctx = (LibGE_VideoContext*)libge_context->vidcontext;
	if(!ctx){
		ctx = (LibGE_VideoContext*)geMalloc(sizeof(LibGE_VideoContext));
		libge_context->vidcontext = (t_ptr)ctx;
		libge_context->gpumem += 2 * libge_context->width * libge_context->height * sizeof(u32);
	}

	if(!ctx->shader2d){
		gePrintDebug(0x100, "No 2D shader, loading it..\n");
		ctx->shader2d = geCreateShader();
		geShaderLoadVertexSource(ctx->shader2d, geFileFromBuffer(_ge_shader_ogl3_generic_2d_vert, sizeof(_ge_shader_ogl3_generic_2d_vert)+1));
		geShaderLoadFragmentSource(ctx->shader2d, geFileFromBuffer(_ge_shader_ogl3_generic_2d_frag, sizeof(_ge_shader_ogl3_generic_2d_frag)+1));
	//	ctx->loc_textured = geShaderUniformID(ctx->shader2d, "textured");
		geShaderUse(ctx->shader2d);
		glUseProgram(ctx->shader2d->programId);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(geShaderUniformID(ctx->shader2d, "ge_Texture"), 0);
		glUseProgram(0);
	}

	libge_context->blend_src = GE_DEFAULT;
	libge_context->blend_dst = GE_DEFAULT;

	glViewport(0, 0, libge_context->width, libge_context->height);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_FRONT);

	glScissor(0, 0, libge_context->width, libge_context->height);
	glEnable(GL_SCISSOR_TEST);
	
	if(libge_context->fontbuf){
		geFreeImage(libge_context->fontbuf);
		libge_context->fontbuf = NULL;
	}
	libge_context->fontbuf = geCreateSurface(libge_context->width, libge_context->height, 0x00000000);
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

void geForceCap(int which, int en){
	int i;
	for(i=0; i<32; i++){
		if(!_ge_force_caps[i].used || _ge_force_caps[i].cap == which){
			if(en == -1){
				_ge_force_caps[i].used = false;
				_ge_force_caps[i].cap = 0;
				_ge_force_caps[i].state = 0;
			}else{
				_ge_force_caps[i].used = true;
				_ge_force_caps[i].cap = which;
				_ge_force_caps[i].state = en;
			}
			break;
		}
	}
}

void geBlendEquation(int mode){
	glBlendEquation(mode);
}

void geFrontFace(int face){
	glFrontFace(face);
}

void geBlendFunc(int src, int dst){
	libge_context->blend_src = src;
	libge_context->blend_dst = dst;
	if(src >= 0 && dst >= 0){
		glBlendFunc(src, dst);
	}
	if(src < 0 && dst < 0 && (libge_context->drawing_mode &= GE_DRAWING_MODE_2D)){
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}

int geDrawingMode(int mode){
	int last = libge_context->drawing_mode;
	if(mode != libge_context->drawing_mode || mode & 0xF0000000){
		if(mode & GE_DRAWING_MODE_2D){
			glActiveTexture(GL_TEXTURE0);
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
			geUpdateMatrix();

			if(mode & GE_DRAWING_2D_DEPTH){
				glEnable(GL_DEPTH_TEST);
				geClearMode(GE_CLEAR_COLOR_BUFFER | GE_CLEAR_DEPTH_BUFFER);
			}else{
				glDisable(GL_DEPTH_TEST);
				geClearMode(GE_CLEAR_COLOR_BUFFER);
			}
		
			geShaderUse(_ge_GetVideoContext()->shader2d);
			glActiveTexture(GL_TEXTURE0);
		}
		if(mode & GE_DRAWING_MODE_3D){
			glEnable(GL_CULL_FACE);
			glFrontFace(GL_CW);
			glEnable(GL_DEPTH_TEST);
			geClearMode(GE_CLEAR_COLOR_BUFFER | GE_CLEAR_DEPTH_BUFFER);

			geMatrixMode(GE_MATRIX_PROJECTION);
			geLoadIdentity();
			if(libge_context->projection_matrix[0] == (float)0xFFFFFFFF){
				gePerspective(60.0, (float)libge_context->width / (float)libge_context->height, 0.1, 100000.0);
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
	glClearColor(Rf(libge_context->clear_color), Gf(libge_context->clear_color), Bf(libge_context->clear_color), Af(libge_context->clear_color));
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

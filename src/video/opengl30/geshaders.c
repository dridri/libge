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

char* load_source(const char* file, int* _size);
static void _geShaderSource(ge_Shader* shader, int type, char* src);

ge_Shader* ge_current_shader = NULL;
ge_Shader* ge_force_shader = NULL;
ge_Shader* ge_line_shader = NULL;

void geMatrixLocations();

#define load_func(name) \
	name = geglImportFunction(#name); \
	gePrintDebug(0x100, #name ": 0x%08X\n", (t_ptr)name)

int geInitShaders(){
	int ext = 0;
	ext += geCheckExtensionAvailable("GL_ARB_shading_language_100");
	ext += geCheckExtensionAvailable("GL_ARB_shader_objects");
	ext += geCheckExtensionAvailable("GL_ARB_vertex_shader");
	ext += geCheckExtensionAvailable("GL_ARB_fragment_shader");
	if(true){ // OpenGL 3.0+ should support shaders
		libge_context->shaders_available = true;
	}

	if(libge_context->shaders_available){
#ifndef PLATFORM_mac
		load_func(glCreateShader);
		load_func(glShaderSource);
		load_func(glCompileShader);
		load_func(glAttachShader);
		load_func(glGetShaderInfoLog);
		load_func(glDeleteShader);
		load_func(glDeleteProgram);

		load_func(glCreateProgram);
		load_func(glLinkProgram);
		load_func(glUseProgram);
		load_func(glGetProgramInfoLog);

		load_func(glBindAttribLocation);
		load_func(glGetUniformLocation);
		load_func(glGetAttribLocation);
		load_func(glVertexAttribPointer);
		load_func(glUniform1f);
		load_func(glUniform2f);
		load_func(glUniform3f);
		load_func(glUniform4f);
		load_func(glUniform1fv);
		load_func(glUniform2fv);
		load_func(glUniform3fv);
		load_func(glUniform4fv);
		load_func(glUniform1i);
		load_func(glUniform1iv);
		load_func(glUniform2iv);
		load_func(glUniform3iv);
		load_func(glUniform4iv);
		load_func(glUniformMatrix3fv);
		load_func(glUniformMatrix4fv);
		load_func(glGetUniformfv);
#endif
	}else{
		return -1;
	}
	return 0;
}

ge_Shader* geCreateShader(){
	if(!libge_context->shaders_available)return NULL;
	ge_Shader* shader = (ge_Shader*)geMalloc(sizeof(ge_Shader));
	shader->programId = glCreateProgram();
	return shader;
}

void geFreeShader(ge_Shader* shader){
	if(shader){
		glDeleteShader(shader->gShaderId);
		glDeleteShader(shader->vShaderId);
		glDeleteShader(shader->fShaderId);
		glDeleteProgram(shader->programId);
		geFree(shader);
	}
}

void geShaderLoadVertexSource(ge_Shader* shader, const char* file){
	char* src = load_source(file, NULL);
	_geShaderSource(shader, GL_VERTEX_SHADER, src);
	geFree(src);
}

void geShaderLoadTessControlSource(ge_Shader* shader, const char* file){
	char* src = load_source(file, NULL);
	_geShaderSource(shader, GL_TESS_CONTROL_SHADER, src);
	geFree(src);
}

void geShaderLoadTessEvaluationSource(ge_Shader* shader, const char* file){
	char* src = load_source(file, NULL);
	_geShaderSource(shader, GL_TESS_EVALUATION_SHADER, src);
	geFree(src);
}

void geShaderLoadGeometrySource(ge_Shader* shader, const char* file){
	char* src = load_source(file, NULL);
	_geShaderSource(shader, GL_GEOMETRY_SHADER, src);
	geFree(src);
}

void geShaderLoadFragmentSource(ge_Shader* shader, const char* file){
	char* src = load_source(file, NULL);
	_geShaderSource(shader, GL_FRAGMENT_SHADER, src);
	geFree(src);
}

void geShaderLoadVertexSourceBuffer(ge_Shader* shader, const char* src){
	_geShaderSource(shader, GL_VERTEX_SHADER, (char*)src);
}

void geShaderLoadTessControlSourceBuffer(ge_Shader* shader, const char* src){
	_geShaderSource(shader, GL_TESS_CONTROL_SHADER, (char*)src);
}

void geShaderLoadTessEvaluationSourceBuffer(ge_Shader* shader, const char* src){
	_geShaderSource(shader, GL_TESS_EVALUATION_SHADER, (char*)src);
}

void geShaderLoadGeometrySourceBuffer(ge_Shader* shader, const char* src){
	_geShaderSource(shader, GL_GEOMETRY_SHADER, (char*)src);
}

void geShaderLoadFragmentSourceBuffer(ge_Shader* shader, const char* src){
	_geShaderSource(shader, GL_FRAGMENT_SHADER, (char*)src);
}

void ge_ParseLog(char* log, char* header){
	gePrintDebug(0x100, "ParseLog : 1\n");
	int headerln = 0;
	int i;
	for(i=0; i<strlen(header); i++){
		if(header[i] == '\n'){
			headerln++;
		}
	}
	gePrintDebug(0x100, "ParseLog : 2\n");
	
	char buf[512];
	char tmp[512];
	char* out = (char*)geMalloc(strlen(log)*2);
	i = 0;
	gePrintDebug(0x100, "ParseLog : 3\n");
	while(i < strlen(log)){
		int linelen = (int)( ((t_ptr)strchr(&log[i], '\n')) - ((t_ptr)&log[i]) );
		if(linelen <= 0){
			linelen = strlen(log) - i;
		}
		strncpy(buf, &log[i], linelen);
		buf[linelen] = 0x0;
		if(strstr(buf, "ERROR") && strstr(buf, "0:")){
			int ln = 0;
			sscanf(strstr(buf, "0:")+2, "%d", &ln);
			strcpy(tmp, buf);
			strstr(tmp, "0:")[2] = 0x0;
			sprintf(tmp, "%s%d", tmp, ln);
			int ln2 = strlen(tmp);
			strcpy(tmp, buf);
			strstr(tmp, "0:")[2] = 0x0;
			sprintf(out, "%s\n%s%d%s", out, tmp, ln-headerln, &buf[ln2]);
		}else{
			sprintf(out, "%s\n%s", out, buf);
		}
		i += linelen +1;
	}
	
	gePrintDebug(0x100, "ParseLog : 4\n");
	strcpy(log, out);
	gePrintDebug(0x100, "ParseLog : 5\n");
}

static void _geShaderSource(ge_Shader* shader, int type, char* src){
	if(!libge_context->shaders_available)return;
	gePrintDebug(0x100, "_geShaderSource(0x%08lX, %d, \"...\")\n", shader, type);
	u32 glId = glCreateShader(type);
	if(type==GL_GEOMETRY_SHADER)shader->gShaderId=glId;
	if(type==GL_VERTEX_SHADER)shader->vShaderId=glId;
	if(type==GL_FRAGMENT_SHADER)shader->fShaderId=glId;
	if(type==GL_TESS_CONTROL_SHADER)shader->tcShaderId=glId;
	if(type==GL_TESS_EVALUATION_SHADER)shader->teShaderId=glId;
	gePrintDebug(0x100, "_geShaderSource : shader initialized\n");

	char* buf = NULL;
	int srclen = strlen(src);
	char* header = NULL;
	char* fullheader = NULL;
	int headerlen = 0;
	bool no_include = false;
	int i;
	for(i=0; i<srclen; i++){
		if(src[i] == '#'){
			for(i+=1; i<srclen && (src[i]==' ' || src[i]=='\t'); i++);
			if(!strncmp(&src[i], "define", 6)){
				for(i+=6; i<srclen && (src[i]==' ' || src[i]=='\t'); i++);
				if(!strncmp(&src[i], "NO_DEFAULT_INCLUDE", 18)){
					no_include = true;
					break;
				}
			}
		}
	}
	if(no_include == false && type == GL_VERTEX_SHADER){
		header = load_source("default_shaders/geshader_gl3v.h", &headerlen);
	}
	if(no_include == false && type == GL_GEOMETRY_SHADER){
		header = load_source("default_shaders/geshader_gl3g.h", &headerlen);
	}
	if(no_include == false && type == GL_FRAGMENT_SHADER){
		header = load_source("default_shaders/geshader_gl3f.h", &headerlen);
	}
	if(no_include == false && header != NULL){
		gePrintDebug(0x100, "_geShaderSource : Header loaded\n");
		fullheader = (char*)geMalloc(sizeof(char)*(headerlen + 256));
		buf = (char*)geMalloc(sizeof(char)*(srclen + headerlen + 256));
		sprintf(fullheader, "%s\n", header);
		//sprintf(buf, "%s%s", fullheader, src);
#ifdef PLATFORM_mac
		sprintf(buf, "#version 150\n%s%s", fullheader, src);
#else
		sprintf(buf, "#version 130\n%s%s", fullheader, src);
#endif
	}else{
		buf = (char*)geMalloc(sizeof(char)*(srclen + 256));
	//	strcpy(buf, src);
#ifdef PLATFORM_mac
		sprintf(buf, "#version 150\n%s", src);
#else
		sprintf(buf, "#version 130\n%s", src);
#endif
	}
	gePrintDebug(0x100, "_geShaderSource : File computed\n");
	glShaderSource(glId, 1, (const GLchar**)&buf, NULL);
	glCompileShader(glId);
	gePrintDebug(0x100, "_geShaderSource : Shader compiled\n");

	char log[4096] = "";
	int logsize = 4096;
	glGetShaderInfoLog(glId, logsize, &logsize, log);
	gePrintDebug(0x100, "_geShaderSource : GetShaderInfoLog ok\n");
//	ParseLog(log, fullheader);
	gePrintDebug(0x100, "_geShaderSource : ParseLog ok\n");
	if(strstr(log, "ERROR")){
		gePrintDebug(0x102, "Errors compiling shader\nCompilation infos: \n    %s", log);
	}else{
		gePrintDebug(0x100, "Compilation infos: \n    %s", log);
	}

	glAttachShader(shader->programId, glId);
	gePrintDebug(0x100, "_geShaderSource : Shader attached\n");

	glBindAttribLocation(shader->programId, 0, "ge_VertexColor");
	glBindAttribLocation(shader->programId, 1, "ge_VertexTexcoord");
	glBindAttribLocation(shader->programId, 2, "ge_VertexNormal");
	glBindAttribLocation(shader->programId, 3, "_ge_VertexPosition");

	glLinkProgram(shader->programId);
	gePrintDebug(0x100, "_geShaderSource : Shader linked\n");

	memset(log, 0, 4096);
	logsize = 4096;
	glGetProgramInfoLog(shader->programId, logsize, &logsize, log);
	if(strstr(log, "ERROR")){
		gePrintDebug(0x102, "Errors compiling shader\nCompilation infos: \n    %s", log);
	}else{
		gePrintDebug(0x100, "program linking infos: \n    %s", log);
	}

	
	shader->loc_time = geShaderUniformID(shader, "ge_Time");
	shader->loc_ratio = geShaderUniformID(shader, "ge_ScreenRatio");
	shader->loc_camera = geShaderUniformID(shader, "ge_CameraPosition");
	shader->loc_HasTexture = geShaderUniformID(shader, "ge_HasTexture");

	shader->loc_lights_d_count = geShaderUniformID(shader, "ge_DynamicLightsCount");
	shader->loc_lights_s_count = geShaderUniformID(shader, "ge_StaticLightsCount");
	shader->loc_front_ambient = geShaderUniformID(shader, "ge_FrontMaterial.ambient");
	shader->loc_front_diffuse = geShaderUniformID(shader, "ge_FrontMaterial.diffuse");
	shader->loc_front_specular = geShaderUniformID(shader, "ge_FrontMaterial.specular");

	shader->loc_mvp = geShaderUniformID(shader, "ge_MatrixModelViewProjection");
	shader->loc_projection = geShaderUniformID(shader, "ge_MatrixProjection");
	shader->loc_modelview = geShaderUniformID(shader, "ge_MatrixModelView");
	shader->loc_normal = geShaderUniformID(shader, "ge_MatrixNormal");
	shader->loc_view = geShaderUniformID(shader, "ge_MatrixView");
	shader->loc_model = geShaderUniformID(shader, "ge_MatrixModel");
	shader->loc_submodel = geShaderUniformID(shader, "ge_MatrixSubModel");
	shader->loc_texture0 = geShaderUniformID(shader, "ge_MatrixTexture[0]");
	shader->loc_texture1 = geShaderUniformID(shader, "ge_MatrixTexture[1]");
	shader->loc_texture2 = geShaderUniformID(shader, "ge_MatrixTexture[2]");
	shader->loc_texture3 = geShaderUniformID(shader, "ge_MatrixTexture[3]");
	shader->loc_texture4 = geShaderUniformID(shader, "ge_MatrixTexture[4]");
	shader->loc_texture5 = geShaderUniformID(shader, "ge_MatrixTexture[5]");
	shader->loc_texture6 = geShaderUniformID(shader, "ge_MatrixTexture[6]");
	shader->loc_texture7 = geShaderUniformID(shader, "ge_MatrixTexture[7]");
	shader->loc_clipplane = geShaderUniformID(shader, "ge_ClipPlane");

	shader->loc_fog_density = geShaderUniformID(shader, "ge_Fog.density");
	shader->loc_fog_color = geShaderUniformID(shader, "ge_Fog.color");
	shader->loc_fog_start = geShaderUniformID(shader, "ge_Fog.start");
	shader->loc_fog_end = geShaderUniformID(shader, "ge_Fog.end");

	gePrintDebug(0x100, "_geShaderSource : Uniforms Ok\n");
	
	gePrintDebug(0x100, "_geShaderSource : Free..");
	if(header)geFree(header);
	gePrintDebug(3, "1..");
	if(fullheader)geFree(fullheader);
	gePrintDebug(3, "2..");
	if(buf)geFree(buf);
	gePrintDebug(3, "3");
}

void geShaderUse(ge_Shader* shader){
	if(!libge_context->shaders_available)return;
	if(ge_force_shader){
		shader = ge_force_shader;
	}
	ge_current_shader = shader;
	if(!shader){
		glUseProgram(0);
		if(libge_context->drawing_mode & GE_DRAWING_MODE_2D){
			geShaderUse(_ge_GetVideoContext()->shader2d);
		}
	}else{
		glUseProgram(shader->programId);
		geMatrixLocations();
//		ge_draw_object_set_shader(shader);
	}
}

void geForceShader(ge_Shader* sh){
	ge_force_shader = sh;
}

void geLineShader(ge_Shader* sh){
	ge_line_shader = sh;
}

int geShaderUniformID(ge_Shader* shader, const char* name){
	int ret = glGetUniformLocation(shader->programId, name);
//	gePrintDebug(0x100, "geShaderUniformID(%p, %s) = %d\n", shader, name, ret);
	return ret;
}

int geShaderAttribID(ge_Shader* shader, const char* name){
	return glGetAttribLocation(shader->programId, name);
}

void geVertexAttribPointer(int id, int size, int type, bool normalized, int stride, void* data){
	glVertexAttribPointer(id, size, type, normalized, stride, data);
}

void geShaderUniform1i(int id, int v1){
	glUniform1i(id, v1);
}

void geShaderUniform2i(int id, int v1, int v2){
	glUniform2i(id, v1, v2);
}

void geShaderUniform3i(int id, int v1, int v2, int v3){
	glUniform3i(id, v1, v2, v3);
}

void geShaderUniform4i(int id, int v1, int v2, int v3, int v4){
	glUniform4i(id, v1, v2, v3, v4);
}

void geShaderUniform1f(int id, float v1){
	glUniform1f(id, v1);
}

void geShaderUniform2f(int id, float v1, float v2){
	glUniform2f(id, v1, v2);
}

void geShaderUniform3f(int id, float v1, float v2, float v3){
	glUniform3f(id, v1, v2, v3);
}

void geShaderUniform4f(int id, float v1, float v2, float v3, float v4){
	glUniform4f(id, v1, v2, v3, v4);
}

void geShaderUniform1fv(int id, int n, float* v){
	glUniform1fv(id, n, v);
}

void geShaderUniform2fv(int id, int n, float* v){
	glUniform2fv(id, n, v);
}

void geShaderUniform3fv(int id, int n, float* v){
	glUniform3fv(id, n, v);
}

void geShaderUniform4fv(int id, int n, float* v){
	glUniform4fv(id, n, v);
}

void geShaderUniform1iv(int id, int n, int* v){
	glUniform1iv(id, n, v);
}

void geShaderUniform2iv(int id, int n, int* v){
	glUniform2iv(id, n, v);
}

void geShaderUniform3iv(int id, int n, int* v){
	glUniform3iv(id, n, v);
}

void geShaderUniform4iv(int id, int n, int* v){
	glUniform4iv(id, n, v);
}

int geFileFullRead(const char* filename, void** buf);
char* load_source(const char* file, int* _size){
	void* _buf = NULL;
	int size = geFileFullRead(file, &_buf);
	char* buf = (char*)_buf;
	if(_size)*_size=size;
	/*
	gePrintDebug(0x100, "-----------------------------------------------------------\n");
	gePrintDebug(0x100, "%d |||| \n%s\n", size, buf);
	gePrintDebug(0x100, "-----------------------------------------------------------\n");
	*/
	return (char*)buf;
}

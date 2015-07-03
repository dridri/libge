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

char* _ge_shader_load_header(const char* file, void* buf, long buflen, int* _size);
char* _ge_shader_load_source(const char* file, int* _size);

ge_Shader* ge_current_shader = NULL;
ge_Shader* ge_force_shader = NULL;
ge_Shader* ge_line_shader = NULL;

extern char GE_BLOB(geshader_gl2v_h_start);
extern char GE_BLOB(geshader_gl2v_h_end);
extern char GE_BLOB(geshader_gl2v_h_size);
extern char GE_BLOB(geshader_gl2g_h_start);
extern char GE_BLOB(geshader_gl2g_h_end);
extern char GE_BLOB(geshader_gl2g_h_size);
extern char GE_BLOB(geshader_gl2f_h_start);
extern char GE_BLOB(geshader_gl2f_h_end);
extern char GE_BLOB(geshader_gl2f_h_size);

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
	if(ext == 4){
		libge_context->shaders_available = true;
	}

	// Assume shaders are available
	libge_context->shaders_available = true;

#if( !defined(GL_GLEXT_PROTOTYPES) && !defined(PLATFORM_mac) )
	if(libge_context->shaders_available){
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
	}else{
		return -1;
	}
#endif
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

void _geShaderSource(ge_Shader* shader, int type, char* src){
	if(!libge_context->shaders_available || type == 0xFFFFFFFF)return;
	gePrintDebug(0x100, "_geShaderSource(0x%08lX, %d, \"...\")\n", shader, type);
	u32 glId = glCreateShader(type);
	if(type==GL_VERTEX_SHADER)shader->vShaderId=glId;
	if(type==GL_FRAGMENT_SHADER)shader->fShaderId=glId;
#ifndef PLATFORM_mac
        if(type==GL_GEOMETRY_SHADER)shader->gShaderId=glId;
	if(type==GL_TESS_CONTROL_SHADER)shader->tcShaderId=glId;
	if(type==GL_TESS_EVALUATION_SHADER)shader->teShaderId=glId;
#endif
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

 	if(no_include == false){
		char* header_file = "default_shaders/geshader_gl2v.h";
		void* header_bin = NULL;
		t_ptr header_bin_sz = 0;
#ifndef PLATFORM_mac
		header_bin = &GE_BLOB(geshader_gl2v_h_start);
		header_bin_sz = (t_ptr)&GE_BLOB(geshader_gl2v_h_size);
#endif
#ifndef PLATFORM_mac
		if(type == GL_GEOMETRY_SHADER){
			header_file = "default_shaders/geshader_gl2g.h";
			header_bin = &GE_BLOB(geshader_gl2g_h_start);
			header_bin_sz = (t_ptr)&GE_BLOB(geshader_gl2g_h_size);
		}
#endif
		if(type == GL_FRAGMENT_SHADER){
			header_file = "default_shaders/geshader_gl2f.h";
#ifndef PLATFORM_mac
			header_bin = &GE_BLOB(geshader_gl2f_h_start);
			header_bin_sz = (t_ptr)&GE_BLOB(geshader_gl2f_h_size);
#endif
		}
		header = _ge_shader_load_header(header_file, header_bin, header_bin_sz, &headerlen);
	}

	if(no_include == false && header != NULL){
		gePrintDebug(0x100, "_geShaderSource : Header loaded\n");
		fullheader = (char*)geMalloc(sizeof(char)*(headerlen + 256));
		buf = (char*)geMalloc(sizeof(char)*(srclen + headerlen + 256));
#ifndef PLATFORM_mac
		sprintf(fullheader, "#define PLATFORM_mac\n");
#endif
		sprintf(fullheader, "%s#define LOW_PROFILE\n%s\n", fullheader, header);
		sprintf(buf, "%s%s", fullheader, src);
		gePrintDebug(0x100, "_geShaderSource : File computed\n");
	}else{
		buf = (char*)geMalloc(sizeof(char)*(srclen + 256));
		strcpy(buf, src);
	}
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
	glLinkProgram(shader->programId);
	gePrintDebug(0x100, "_geShaderSource : Shader linked\n");

	memset(log, 0, 4096);
	logsize = 4096;
	glGetProgramInfoLog(shader->programId, logsize, &logsize, log);
	gePrintDebug(0x100, "program linking infos: \n    %s", log);

	
	shader->loc_time = glGetUniformLocation(shader->programId, "ge_Time");
	shader->loc_ratio = glGetUniformLocation(shader->programId, "ge_ScreenRatio");
	shader->loc_camera = glGetUniformLocation(shader->programId, "ge_CameraPosition");
	shader->loc_HasTexture = glGetUniformLocation(shader->programId, "ge_HasTexture");

	shader->loc_lights_d_count = glGetUniformLocation(shader->programId, "ge_DynamicLightsCount");
	shader->loc_lights_s_count = glGetUniformLocation(shader->programId, "ge_StaticLightsCount");
	shader->loc_front_ambient = glGetUniformLocation(shader->programId, "ge_FrontMaterial.ambient");
	shader->loc_front_diffuse = glGetUniformLocation(shader->programId, "ge_FrontMaterial.diffuse");
	shader->loc_front_specular = glGetUniformLocation(shader->programId, "ge_FrontMaterial.specular");
	shader->loc_mvp = glGetUniformLocation(shader->programId, "ge_MatrixModelViewProjection");
	shader->loc_view = glGetUniformLocation(shader->programId, "ge_MatrixView");
	shader->loc_model = glGetUniformLocation(shader->programId, "ge_MatrixModel");
	shader->loc_submodel = glGetUniformLocation(shader->programId, "ge_MatrixSubModel");

	shader->loc_fog_density = glGetUniformLocation(shader->programId, "ge_Fog.density");
	shader->loc_fog_color = glGetUniformLocation(shader->programId, "ge_Fog.color");
	shader->loc_fog_start = glGetUniformLocation(shader->programId, "ge_Fog.start");
	shader->loc_fog_end = glGetUniformLocation(shader->programId, "ge_Fog.end");

	gePrintDebug(0x100, "_geShaderSource : Uniforms Ok\n");
	
	gePrintDebug(0x100, "_geShaderSource : Free..");
	if(header)geFree(header);
	gePrintDebug(3, "1..");
	if(fullheader)geFree(fullheader);
	gePrintDebug(3, "2..");
	if(buf)geFree(buf);
	gePrintDebug(3, "3");
}

int geShaderUniformID(ge_Shader* shader, const char* name){
	return glGetUniformLocation(shader->programId, name);
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

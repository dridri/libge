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
	gePrintDebug(0x100, #name ": 0x%08X\n", (unsigned long)name)

int geInitShaders(){
	libge_context->shaders_available = true;

	geCheckExtensionAvailable("");
	if(libge_context->shaders_available){
	/*
		load_func(glCreateShader);
		load_func(glShaderSource);
		load_func(glCompileShader);
		load_func(glAttachShader);
		load_func(glGetShaderInfoLog);

		load_func(glCreateProgram);
		load_func(glLinkProgram);
		load_func(glUseProgram);
		load_func(glGetProgramInfoLog);

		load_func(glBindAttribLocation);
		load_func(glGetUniformLocation);
		load_func(glGetAttribLocation);
		load_func(glUniform1f);
		load_func(glUniform2f);
		load_func(glUniform3f);
		load_func(glUniform4f);
		load_func(glUniform1fv);
		load_func(glUniform1i);
		load_func(glUniform1iv);
		load_func(glUniformMatrix3fv);
		load_func(glUniformMatrix4fv);
		load_func(glGetUniformfv);
	*/
	}else{
		return -1;
	}
	return 0;
}

ge_Shader* geCreateShader(){
	if(!libge_context->shaders_available)return NULL;
	ge_Shader* shader = (ge_Shader*)geMalloc(sizeof(ge_Shader));
	memset(shader, 0xFF, sizeof(ge_Shader));
	shader->loc_lights = NULL;
	shader->programId = glCreateProgram();
	return shader;
}

void geShaderLoadVertexSource(ge_Shader* shader, const char* file){
	char* src = load_source(file, NULL);
	_geShaderSource(shader, GL_VERTEX_SHADER, src);
	geFree(src);
}

void geShaderLoadTessControlSource(ge_Shader* shader, const char* file){
/*
	char* src = load_source(file, NULL);
	_geShaderSource(shader, GL_TESS_CONTROL_SHADER, src);
	geFree(src);
*/
}

void geShaderLoadTessEvaluationSource(ge_Shader* shader, const char* file){
/*
	char* src = load_source(file, NULL);
	_geShaderSource(shader, GL_TESS_EVALUATION_SHADER, src);
	geFree(src);
*/
}

void geShaderLoadGeometrySource(ge_Shader* shader, const char* file){
/*
	char* src = load_source(file, NULL);
	_geShaderSource(shader, GL_GEOMETRY_SHADER, src);
	geFree(src);
*/
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
//	_geShaderSource(shader, GL_TESS_CONTROL_SHADER, (char*)src);
}

void geShaderLoadTessEvaluationSourceBuffer(ge_Shader* shader, const char* src){
//	_geShaderSource(shader, GL_TESS_EVALUATION_SHADER, (char*)src);
}

void geShaderLoadGeometrySourceBuffer(ge_Shader* shader, const char* src){
//	_geShaderSource(shader, GL_GEOMETRY_SHADER, (char*)src);
}

void geShaderLoadFragmentSourceBuffer(ge_Shader* shader, const char* src){
	_geShaderSource(shader, GL_FRAGMENT_SHADER, (char*)src);
}

static void ParseLog(char* log, char* header){
	int headerln = 0;
	int i;
	for(i=0; i<strlen(header); i++){
		if(header[i] == '\n'){
			headerln++;
		}
	}
	
	char buf[512];
	char tmp[512];
	char* out = (char*)geMalloc(strlen(log)*2);
	i = 0;
	while(i < strlen(log)){
		int linelen = (int)( ((unsigned long)strchr(&log[i], '\n')) - ((unsigned long)&log[i]) );
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

	strcpy(log, out);
}

static void _geShaderSource(ge_Shader* shader, int type, char* src){
	if(!libge_context->shaders_available)return;
	u32 glId = glCreateShader(type);
//	if(type==GL_GEOMETRY_SHADER)shader->gShaderId=glId;
	if(type==GL_VERTEX_SHADER)shader->vShaderId=glId;
	if(type==GL_FRAGMENT_SHADER)shader->fShaderId=glId;

	int srclen = strlen(src);
	char* header = "";
	int headerlen = 0;
	if(type == GL_VERTEX_SHADER){
		header = load_source(_ge_BuildPath(libge_context->default_shaders_path, "geshader_gles2v.h"), &headerlen);
	}
	if(type == GL_FRAGMENT_SHADER){
		header = load_source(_ge_BuildPath(libge_context->default_shaders_path, "geshader_gles2f.h"), &headerlen);
	}
	gePrintDebug(0x100, "_geShaderSource : Header loaded\n");
	char* fullheader = (char*)geMalloc(sizeof(char)*(headerlen + 256));
	char* buf = (char*)geMalloc(sizeof(char)*(srclen + headerlen + 256));
	memset(buf, 0, sizeof(char)*(srclen + headerlen + 256));
	sprintf(fullheader, "#define LOW_PROFILE\n%s\n", header);
	sprintf(buf, "%s%s", fullheader, src);
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
	gePrintDebug(0x100, "Compilation infos: \n    %s", log);
	if(((strstr(log, "ERROR") || strstr(log, "error")) && !strstr(log, "0 error") && !strstr(log, "0 ERROR")) ||
	   ((strstr(log, "WARNING") || strstr(log, "warning")) && !strstr(log, "0 warning") && !strstr(log, "0 WARNING"))){
		gePrintDebug(0x102, "Errors compiling shader\n");
	}

	glAttachShader(shader->programId, glId);
	gePrintDebug(0x100, "_geShaderSource : Shader attached\n");
	glBindAttribLocation(shader->programId, 0, "ge_VertexTexcoord");
	glBindAttribLocation(shader->programId, 1, "ge_VertexColor");
	glBindAttribLocation(shader->programId, 2, "ge_VertexNormal");
	glBindAttribLocation(shader->programId, 3, "ge_VertexPosition");
	glLinkProgram(shader->programId);
	gePrintDebug(0x100, "_geShaderSource : Shader linked\n");

	memset(log, 0, 4096);
	logsize = 4096;
	glGetProgramInfoLog(shader->programId, logsize, &logsize, log);
	gePrintDebug(0x100, "program linking infos: \n    %s", log);

	if(shader->vShaderId != 0 && shader->fShaderId != 0){
		shader->loc_model = glGetUniformLocation(shader->programId, "ge_MatrixModel");
		shader->loc_view = glGetUniformLocation(shader->programId, "ge_MatrixView");
		shader->loc_projection = glGetUniformLocation(shader->programId, "ge_MatrixProjection");
		shader->loc_normal = glGetUniformLocation(shader->programId, "ge_MatrixNormal");
		shader->loc_modelview = glGetUniformLocation(shader->programId, "ge_MatrixModelView");
		
		shader->loc_time = glGetUniformLocation(shader->programId, "ge_Time");
		shader->loc_ratio = glGetUniformLocation(shader->programId, "ge_ScreenRatio");
		shader->loc_camera = glGetUniformLocation(shader->programId, "ge_CameraPosition");
		shader->loc_lights_d_count = glGetUniformLocation(shader->programId, "ge_DynamicLightsCount");
		shader->loc_lights_s_count = glGetUniformLocation(shader->programId, "ge_StaticLightsCount");
		shader->loc_front_ambient = glGetUniformLocation(shader->programId, "ge_FrontMaterial.ambient");
		shader->loc_front_diffuse = glGetUniformLocation(shader->programId, "ge_FrontMaterial.diffuse");
		shader->loc_submodel = glGetUniformLocation(shader->programId, "ge_MatrixSubModel");

		shader->loc_fog_density = glGetUniformLocation(shader->programId, "ge_Fog.density");
		shader->loc_fog_color = glGetUniformLocation(shader->programId, "ge_Fog.color");
		shader->loc_fog_start = glGetUniformLocation(shader->programId, "ge_Fog.start");
		shader->loc_fog_end = glGetUniformLocation(shader->programId, "ge_Fog.end");
	}
	
	geFree(header);
	geFree(fullheader);
	geFree(buf);
}

void geShaderUse(ge_Shader* shader){
	if(!libge_context->shaders_available)return;
	ge_current_shader = shader;
	if(ge_force_shader){
		shader = ge_force_shader;
	}
	if(!shader){
		glUseProgram(0);
		return;
	}
	glUseProgram(shader->programId);
	geMatrixLocations();
//	ge_draw_object_set_shader(shader);
}

void geForceShader(ge_Shader* sh){
	ge_force_shader = sh;
}

void geLineShader(ge_Shader* sh){
	ge_line_shader = sh;
}

int geShaderUniformID(ge_Shader* shader, const char* name){
	return glGetUniformLocation(shader->programId, name);
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

void geShaderUniform3fv(int id, int n, float* v){
	glUniform3fv(id, n, v);
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

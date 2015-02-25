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

#include "../ge_internal.h"

int geFileFullRead(const char* filename, void** buf);
char* _ge_shader_load_source(const char* file, int* _size);
void _geShaderSource(ge_Shader* shader, int type, char* src);

extern ge_Shader* ge_current_shader;
extern ge_Shader* ge_force_shader;
extern ge_Shader* ge_line_shader;

#ifdef GL_VERTEX_SHADER
const unsigned int _i_vertex_shader = GL_VERTEX_SHADER;
#else
const unsigned int _i_vertex_shader = 0xFFFFFFFF;
#endif

#ifdef GL_GEOMETRY_SHADER
const unsigned int _i_geometry_shader = GL_GEOMETRY_SHADER;
#else
const unsigned int _i_geometry_shader = 0xFFFFFFFF;
#endif

#ifdef GL_FRAGMENT_SHADER
const unsigned int _i_fragement_shader = GL_FRAGMENT_SHADER;
#else
const unsigned int _i_fragement_shader = 0xFFFFFFFF;
#endif

#ifdef GL_TESS_CONTROL_SHADER
const unsigned int _i_tesscontrol_shader = GL_TESS_CONTROL_SHADER;
#else
const unsigned int _i_tesscontrol_shader = 0xFFFFFFFF;
#endif

#ifdef GL_TESS_EVALUATION_SHADER
const unsigned int _i_tesseval_shader = GL_TESS_EVALUATION_SHADER;
#else
const unsigned int _i_tesseval_shader = 0xFFFFFFFF;
#endif

void geShaderLoadVertexSource(ge_Shader* shader, const char* file){
#ifdef GL_VERTEX_SHADER
	char* src = _ge_shader_load_source(file, NULL);
	_geShaderSource(shader, _i_vertex_shader, src);
	geFree(src);
#endif
}

void geShaderLoadTessControlSource(ge_Shader* shader, const char* file){
#ifdef GL_TESS_CONTROL_SHADER
	char* src = _ge_shader_load_source(file, NULL);
	_geShaderSource(shader, _i_tesscontrol_shader, src);
	geFree(src);
#endif
}

void geShaderLoadTessEvaluationSource(ge_Shader* shader, const char* file){
#ifdef GL_TESS_EVALUATION_SHADER
	char* src = _ge_shader_load_source(file, NULL);
	_geShaderSource(shader, _i_tesseval_shader, src);
	geFree(src);
#endif
}

void geShaderLoadGeometrySource(ge_Shader* shader, const char* file){
#ifdef GL_GEOMETRY_SHADER
	char* src = _ge_shader_load_source(file, NULL);
	_geShaderSource(shader, _i_geometry_shader, src);
	geFree(src);
#endif
}

void geShaderLoadFragmentSource(ge_Shader* shader, const char* file){
#ifdef GL_FRAGMENT_SHADER
	char* src = _ge_shader_load_source(file, NULL);
	_geShaderSource(shader, _i_fragement_shader, src);
	geFree(src);
#endif
}

void geShaderLoadVertexSourceBuffer(ge_Shader* shader, const char* src){
#ifdef GL_VERTEX_SHADER
	_geShaderSource(shader, _i_vertex_shader, (char*)src);
#endif
}

void geShaderLoadTessControlSourceBuffer(ge_Shader* shader, const char* src){
#ifdef GL_TESS_CONTROL_SHADER
	_geShaderSource(shader, _i_tesscontrol_shader, (char*)src);
#endif
}

void geShaderLoadTessEvaluationSourceBuffer(ge_Shader* shader, const char* src){
#ifdef GL_TESS_EVALUATION_SHADER
	_geShaderSource(shader, _i_tesseval_shader, (char*)src);
#endif
}

void geShaderLoadGeometrySourceBuffer(ge_Shader* shader, const char* src){
#ifdef GL_GEOMETRY_SHADER
	_geShaderSource(shader, _i_geometry_shader, (char*)src);
#endif
}

void geShaderLoadFragmentSourceBuffer(ge_Shader* shader, const char* src){
#ifdef GL_FRAGMENT_SHADER
	_geShaderSource(shader, _i_fragement_shader, (char*)src);
#endif
}

ge_Shader* geShaderUse(ge_Shader* shader){
	if(!libge_context->shaders_available || glUseProgram == 0)return 0;
	ge_Shader* ret = ge_current_shader;
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
	return ret;
}

ge_Shader* geForceShader(ge_Shader* sh){
	ge_Shader* ret = ge_force_shader;
	ge_force_shader = sh;
	return ret;
}

void geLineShader(ge_Shader* sh){
	ge_line_shader = sh;
}

char* _ge_shader_load_header(const char* file, void* buf, long buflen, int* _size){
	char* ret = 0;
	bool crit = geDebugCritical(false);
	ret = _ge_shader_load_source(file, _size);
	geDebugCritical(crit);
	if(!ret){
		ret = _ge_shader_load_source(geFileFromBuffer(buf, buflen), _size);
	}
	return ret;
}

char* _ge_shader_load_source(const char* file, int* _size){
	void* _buf = NULL;
	int size = geFileFullRead(file, &_buf);
	if(size <= 0){
		return 0;
	}
	char* buf = (char*)_buf;
	if(_size)*_size=size;
	/*
	gePrintDebug(0x100, "-----------------------------------------------------------\n");
	gePrintDebug(0x100, "%d |||| \n%s\n", size, buf);
	gePrintDebug(0x100, "-----------------------------------------------------------\n");
	*/
	return (char*)buf;
}

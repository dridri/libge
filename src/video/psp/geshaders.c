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

int geInitShaders(){
	return 0;
}

ge_Shader* geCreateShader(){
	return NULL;
}

void geFreeShader(ge_Shader* shader){
}

void geShaderLoadGeometrySource(ge_Shader* shader, const char* file){
}

void geShaderLoadVertexSource(ge_Shader* shader, const char* file){
}

void geShaderLoadFragmentSource(ge_Shader* shader, const char* file){
}

void geShaderLoadTessControlSource(ge_Shader* shader, const char* file){
}

void geShaderLoadTessEvaluationSource(ge_Shader* shader, const char* file){
}

void geShaderLoadGeometrySourceBuffer(ge_Shader* shader, const char* src){
}

void geShaderLoadVertexSourceBuffer(ge_Shader* shader, const char* src){
}

void geShaderLoadFragmentSourceBuffer(ge_Shader* shader, const char* src){
}

void geShaderUse(ge_Shader* shader){
}

void geForceShader(ge_Shader* sh){
}

void geLineShader(ge_Shader* sh){
}

int geShaderUniformID(ge_Shader* sh, const char* name){
	return 0;
}

int geShaderAttribID(ge_Shader* shader, const char* name){
	return 0;
}

void geShaderUniform1i(int id, int v1){
}

void geShaderUniform2i(int id, int v1, int v2){
}

void geShaderUniform3i(int id, int v1, int v2, int v3){
}

void geShaderUniform4i(int id, int v1, int v2, int v3, int v4){
}

void geShaderUniform1f(int id, float v1){
}

void geShaderUniform2f(int id, float v1, float v2){
}

void geShaderUniform3f(int id, float v1, float v2, float v3){
}

void geShaderUniform4f(int id, float v1, float v2, float v3, float v4){
}

void geShaderUniform3fv(int id, int n, float* v){
}

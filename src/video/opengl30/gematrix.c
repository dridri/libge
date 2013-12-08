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
#include <math.h>

/*
	GE_MATRIX_MODEL 0
	GE_MATRIX_VIEW 1
	GE_MATRIX_PROJECTION 2
	GE_MATRIX_NORMAL 3
	GE_MATRIX_MODELVIEW 4
	GE_MATRIX_TEXTURE0 5
	GE_MATRIX_TEXTURE1 6
	GE_MATRIX_TEXTURE2 7
	GE_MATRIX_TEXTURE3 8
	GE_MATRIX_TEXTURE4 9
	GE_MATRIX_TEXTURE5 10
	GE_MATRIX_TEXTURE6 11
	GE_MATRIX_TEXTURE7 12
	GE_MATRIX_SUBMODEL 13
*/

#define PI_OVER_360 0.0087266
#define PI_OVER_180 0.0174532
#define PI_OVER_90 0.0349065

int ge_current_mode = GE_MATRIX_PROJECTION;
int ge_matrix_update[14] = { 0 };
int ge_current_matrix_update = 0;
float ge_matrices[14][16];
float* ge_current_matrix = ge_matrices[GE_MATRIX_PROJECTION];
float ge_clip_plane[4] = { 0.0 };

extern ge_Shader* ge_current_shader;

void ge_LoadIdentity(float* m);
void ge_Translate(float* m, float x, float y, float z);
void ge_Rotate(float* m, float x, float y, float z);
void ge_Scale(float* m, float x, float y, float z);
void geSendMatrix(int type, float* m);

void PrintMatrix(const char* name, float* m, int s){
	gePrintDebug(0x100, "%s = {\n", name);
	int i = 0;
	for(i=0; i<s; i++){
		gePrintDebug(0x100, "  %f %f %f %f\n", m[i*4+0], m[i*4+1], m[i*4+2], m[i*4+3]);
	}
}

void geMatrixMode(int mode){
	ge_matrix_update[ge_current_mode] = ge_current_matrix_update;
	ge_current_mode = mode;
	ge_current_matrix_update = ge_matrix_update[mode];
	ge_current_matrix = ge_matrices[mode];
}

void geLoadIdentity(){
	float* m = (float*)ge_current_matrix;
	m[0]= 1.0;	m[1]= 0.0;	m[2]= 0.0;	m[3]= 0.0;
	m[4]= 0.0;	m[5]= 1.0;	m[6]= 0.0;	m[7]= 0.0;
	m[8]= 0.0;	m[9]= 0.0;	m[10]=1.0;	m[11]=0.0;
	m[12]=0.0;	m[13]=0.0;	m[14]=0.0;	m[15]=1.0;
	ge_current_matrix_update = true;
}

void geMatrixMult(float* m){
//	geMatrix44Mult(ge_current_matrix, ge_current_matrix, m);
	float tmp[16];
	geMatrix44Mult(tmp, ge_current_matrix, m);
	memcpy(ge_current_matrix, tmp, sizeof(float)*16);
	ge_current_matrix_update = true;
}

void geLoadMatrix(float* m){
	memcpy(ge_current_matrix, m, sizeof(float)*16);
	ge_current_matrix_update = true;
}

void geTranslate(float x, float y, float z){
	float t[16];
	ge_LoadIdentity(t);
	t[12] = x;
	t[13] = y;
	t[14] = z;

	geMatrix44Mult(ge_current_matrix, ge_current_matrix, t);
	ge_current_matrix_update = true;
}

void geRotate(float x, float y, float z){
	float tx[16], ty[16], tz[16], tb[16];

	if(x != 0.0f){
		ge_LoadIdentity(tx);
		float cx = cosf(x);
		float sx = sinf(x);
		tx[1*4+1] = cx;
		tx[1*4+2] = sx;
		tx[2*4+1] = -sx;
		tx[2*4+2] = cx;

		memcpy(tb, ge_current_matrix, sizeof(float)*16);
		geMatrix44Mult(ge_current_matrix, tb, tx);
	}
	
	if(y != 0.0f){
		ge_LoadIdentity(ty);
		float cy = cosf(y);
		float sy = sinf(y);
		ty[0*4+0] = cy;
		ty[0*4+2] = -sy;
		ty[2*4+0] = sy;
		ty[2*4+2] = cy;

		float tb[16];
		memcpy(tb, ge_current_matrix, sizeof(float)*16);
		geMatrix44Mult(ge_current_matrix, tb, ty);
	}
	
	if(z != 0.0f){
		ge_LoadIdentity(tz);
		float cz = cosf(z);
		float sz = sinf(z);
		tz[0*4+0] = cz;
		tz[0*4+1] = sz;
		tz[1*4+0] = -sz;
		tz[1*4+1] = cz;

		float tb[16];
		memcpy(tb, ge_current_matrix, sizeof(float)*16);
		geMatrix44Mult(ge_current_matrix, tb, tz);
	}

	ge_current_matrix_update = true;
}

void geScale(float x, float y, float z){
	float t[16];
	ge_LoadIdentity(t);
	t[0] = x;
	t[5] = y;
	t[10] = z;

	geMatrix44Mult(ge_current_matrix, ge_current_matrix, t);
	ge_current_matrix_update = true;
}

void gePerspective(float fov, float aspect, float zNear, float zFar){
	const float h = 1.0f/tan(fov*PI_OVER_360);
	float neg_depth = zNear-zFar;

	ge_current_matrix[0] = h / aspect;
	ge_current_matrix[1] = 0;
	ge_current_matrix[2] = 0;
	ge_current_matrix[3] = 0;
	ge_current_matrix[4] = 0;
	ge_current_matrix[5] = h;
	ge_current_matrix[6] = 0;
	ge_current_matrix[7] = 0;
	ge_current_matrix[8] = 0;
	ge_current_matrix[9] = 0;
	ge_current_matrix[10] = (zFar + zNear)/neg_depth;
	ge_current_matrix[11] = -1;
	ge_current_matrix[12] = 0;
	ge_current_matrix[13] = 0;
	ge_current_matrix[14] = 2.0f*(zNear*zFar)/neg_depth;
	ge_current_matrix[15] = 0;

	if(ge_current_mode == GE_MATRIX_PROJECTION){
		memcpy(libge_context->projection_matrix, ge_current_matrix, sizeof(float)*16);
	}
	ge_current_matrix_update = true;
}

void geOrthogonal(float left, float right, float bottom, float top, float zNear, float zFar){
	ge_LoadIdentity(ge_current_matrix);

	float tx = - (right + left) / (right - left);
	float ty = - (top + bottom) / (top - bottom);
	float tz = - (zFar + zNear) / (zFar - zNear);

	ge_current_matrix[0] = 2.0 / (right - left);
	ge_current_matrix[5] = 2.0 / (top - bottom);
	ge_current_matrix[10] = -2.0 / (zFar - zNear);

	ge_current_matrix[12] = tx;
	ge_current_matrix[13] = ty;
	ge_current_matrix[14] = tz;

	if(ge_current_mode == GE_MATRIX_PROJECTION){
	//	memcpy(libge_context->projection_matrix, ge_current_matrix, sizeof(float)*16);
	}
	ge_current_matrix_update = true;
}

static float u_vec[3] = { 0.0, 0.0, 1.0 };
void geVerticalView(float x, float y, float z){
	u_vec[0]=x; u_vec[1]=y; u_vec[2]=z;
}

float* ge_GetVerticalView(){
	return u_vec;
}

void geLookAt(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ){
	float m[16];
	float f[4];
	float u[3];
	float s[4];

	f[0] = centerX - eyeX;
	f[1] = centerY - eyeY;
	f[2] = centerZ - eyeZ;
	f[3] = 0.0;

	geNormalize(f);
	geCrossProduct(s, f, u_vec);
	s[3] = 0.0;
	geNormalize(s);
	geCrossProduct(u, s, f);

	m[0] = s[0];
	m[4] = s[1];
	m[8] = s[2];
	m[12]= 0.0;

	m[1] = u[0];
	m[5] = u[1];
	m[9] = u[2];
	m[13]= 0.0;

	m[2] = -f[0];
	m[6] = -f[1];
	m[10]= -f[2];
	m[14]= 0.0;

	m[3] = 0.0;
	m[7] = 0.0;
	m[11]= 0.0;
	m[15]= 1.0;

	float result[16] = { 0.0 };
	geMatrix44Mult(result, ge_current_matrix, m);
	memcpy(ge_current_matrix, result, sizeof(float)*16);
	geTranslate(-eyeX, -eyeY, -eyeZ);

	ge_current_matrix_update = true;
}

void ge_LookAt(float* _m, float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ){
	float m[16];
	float f[4];
	float u[3];
	float s[4];

	f[0] = centerX - eyeX;
	f[1] = centerY - eyeY;
	f[2] = centerZ - eyeZ;
	f[3] = 0.0;

	geNormalize(f);
	geCrossProduct(s, f, u_vec);
	s[3] = 0.0;
	geNormalize(s);
	geCrossProduct(u, s, f);

	m[0] = s[0];
	m[4] = s[1];
	m[8] = s[2];
	m[12]= 0.0;

	m[1] = u[0];
	m[5] = u[1];
	m[9] = u[2];
	m[13]= 0.0;

	m[2] = -f[0];
	m[6] = -f[1];
	m[10]= -f[2];
	m[14]= 0.0;

	m[3] = 0.0;
	m[7] = 0.0;
	m[11]= 0.0;
	m[15]= 1.0;

	float result[16] = { 0.0 };
	geMatrix44Mult(result, _m, m);
	memcpy(_m, result, sizeof(float)*16);
	ge_Translate(_m, -eyeX, -eyeY, -eyeZ);
}

bool ge_IsIdentity(float* m){
	if(((((m[0] == m[5]) == m[10]) == m[15]) == 1.0) && ((((((((((((m[1] == m[2]) == m[3]) == m[4]) == m[6]) == m[7]) == m[8]) == m[9]) == m[11]) == m[12]) == m[13]) == m[14]) == 0.0)){
		return true;
	}
	return false;
}

void ge_LoadIdentity(float* m){
	m[0]= 1.0;	m[1]= 0.0;	m[2]= 0.0;	m[3]= 0.0;
	m[4]= 0.0;	m[5]= 1.0;	m[6]= 0.0;	m[7]= 0.0;
	m[8]= 0.0;	m[9]= 0.0;	m[10]=1.0;	m[11]=0.0;
	m[12]=0.0;	m[13]=0.0;	m[14]=0.0;	m[15]=1.0;
}

void ge_Translate(float* m, float x, float y, float z){
	float t[16];
	ge_LoadIdentity(t);
	t[12] = x;
	t[13] = y;
	t[14] = z;
	geMatrix44Mult(m, m, t);
}

void ge_Rotate(float* m, float x, float y, float z){
	float tx[16], ty[16], tz[16], tb[16];

	if(x != 0.0f){
		ge_LoadIdentity(tx);
		float cx = cosf(x);
		float sx = sinf(x);
		tx[1*4+1] = cx;
		tx[1*4+2] = sx;
		tx[2*4+1] = -sx;
		tx[2*4+2] = cx;

		memcpy(tb, m, sizeof(float)*16);
		geMatrix44Mult(m, tb, tx);
	}
	
	if(y != 0.0f){
		ge_LoadIdentity(ty);
		float cy = cosf(y);
		float sy = sinf(y);
		ty[0*4+0] = cy;
		ty[0*4+2] = -sy;
		ty[2*4+0] = sy;
		ty[2*4+2] = cy;

		float tb[16];
		memcpy(tb, m, sizeof(float)*16);
		geMatrix44Mult(m, tb, ty);
	}
	
	if(z != 0.0f){
		ge_LoadIdentity(tz);
		float cz = cosf(z);
		float sz = sinf(z);
		tz[0*4+0] = cz;
		tz[0*4+1] = sz;
		tz[1*4+0] = -sz;
		tz[1*4+1] = cz;

		float tb[16];
		memcpy(tb, m, sizeof(float)*16);
		geMatrix44Mult(m, tb, tz);
	}
}

void ge_Scale(float* m, float x, float y, float z){
	float t[16];
	ge_LoadIdentity(t);
	t[0] = x;
	t[5] = y;
	t[10] = z;

	geMatrix44Mult(m, m, t);
}

void ge_Perspective(float* m, float fov, float aspect, float zNear, float zFar){
	const float h = 1.0f/tan(fov*PI_OVER_360);
	float neg_depth = zNear-zFar;

	m[0] = h / aspect;
	m[1] = 0;
	m[2] = 0;
	m[3] = 0;
	m[4] = 0;
	m[5] = h;
	m[6] = 0;
	m[7] = 0;
	m[8] = 0;
	m[9] = 0;
	m[10] = (zFar + zNear)/neg_depth;
	m[11] = -1;
	m[12] = 0;
	m[13] = 0;
	m[14] = 2.0f*(zNear*zFar)/neg_depth;
	m[15] = 0;
}

void geClipPlane(float x, float y, float z, float w){
	ge_clip_plane[0] = x;
	ge_clip_plane[1] = y;
	ge_clip_plane[2] = z;
	ge_clip_plane[3] = w;
}

extern ge_Camera* ge_current_camera;
void CalculateModelMatrices(){
	float ModelView3[9];
	float temp[16]; //9
	
	geMatrix44Mult(ge_matrices[GE_MATRIX_MODELVIEW], ge_matrices[GE_MATRIX_VIEW], ge_matrices[GE_MATRIX_MODEL]);

	ModelView3[0]=ge_matrices[GE_MATRIX_MODELVIEW][0]; ModelView3[1]=ge_matrices[GE_MATRIX_MODELVIEW][1]; ModelView3[2]=ge_matrices[GE_MATRIX_MODELVIEW][2];
	ModelView3[3]=ge_matrices[GE_MATRIX_MODELVIEW][4]; ModelView3[4]=ge_matrices[GE_MATRIX_MODELVIEW][5]; ModelView3[5]=ge_matrices[GE_MATRIX_MODELVIEW][6];
	ModelView3[6]=ge_matrices[GE_MATRIX_MODELVIEW][8]; ModelView3[7]=ge_matrices[GE_MATRIX_MODELVIEW][9]; ModelView3[8]=ge_matrices[GE_MATRIX_MODELVIEW][10];
	
	geMatrixInverse(temp, ModelView3, 3);
	geMatrixTranspose(ge_matrices[GE_MATRIX_NORMAL], temp, 3);
/*
	float temp[16]; //9
	geMatrixInverse(temp, ge_matrices[GE_MATRIX_MODELVIEW], 4);
	geMatrixTranspose(temp2, temp, 4);
	ge_matrices[GE_MATRIX_NORMAL][0]=temp2[0]; ge_matrices[GE_MATRIX_NORMAL][1]=temp2[1]; ge_matrices[GE_MATRIX_NORMAL][2]=temp2[2];
	ge_matrices[GE_MATRIX_NORMAL][3]=temp2[4]; ge_matrices[GE_MATRIX_NORMAL][4]=temp2[5]; ge_matrices[GE_MATRIX_NORMAL][5]=temp2[6];
	ge_matrices[GE_MATRIX_NORMAL][6]=temp2[8]; ge_matrices[GE_MATRIX_NORMAL][7]=temp2[9]; ge_matrices[GE_MATRIX_NORMAL][8]=temp2[10];
*/
}

static int loc_mvp = -1;
static int loc_model = -1;
static int loc_view = -1;
static int loc_projection = -1;
static int loc_normal = -1;
static int loc_modelview = -1;
static int loc_texture0 = -1;
static int loc_texture1 = -1;
static int loc_texture2 = -1;
static int loc_texture3 = -1;
static int loc_texture4 = -1;
static int loc_texture5 = -1;
static int loc_texture6 = -1;
static int loc_texture7 = -1;
static int loc_submodel = -1;
static int loc_clipplane = -1;

void SendModelMatrices(){
	geSendMatrix(GE_MATRIX_MODELVIEW, (float*)&ge_matrices[GE_MATRIX_MODELVIEW]);
	geSendMatrix(GE_MATRIX_NORMAL, (float*)&ge_matrices[GE_MATRIX_NORMAL]);
	
	float temp[16]; //9
	geMatrix44Mult(temp, ge_matrices[GE_MATRIX_PROJECTION], ge_matrices[GE_MATRIX_MODELVIEW]);
	glUniformMatrix4fv(loc_mvp, 1, GL_FALSE, temp);
//	PrintMatrix("MatrixModelView", (float*)&ge_matrices[GE_MATRIX_MODELVIEW], 4);
//	PrintMatrix("MatrixNormal", (float*)&ge_matrices[GE_MATRIX_NORMAL], 4);
}
/*
void geUpdateMatrix(int whitch){
	ge_matrix_update[ge_current_mode] = ge_current_matrix_update;
	if(ge_matrix_update[whitch]){
		geSendMatrix(whitch, (float*)&ge_matrices[whitch]);
		if(whitch == GE_MATRIX_MODEL){
			SendModelMatrices();
		}
	}
}
*/

void geUpdateMatrix(){
	ge_matrix_update[ge_current_mode] = ge_current_matrix_update;

	CalculateModelMatrices();

	geSendMatrix(GE_MATRIX_MODEL, (float*)&ge_matrices[GE_MATRIX_MODEL]);
	geSendMatrix(GE_MATRIX_SUBMODEL, (float*)&ge_matrices[GE_MATRIX_SUBMODEL]);
	geSendMatrix(GE_MATRIX_VIEW, (float*)&ge_matrices[GE_MATRIX_VIEW]);
	geSendMatrix(GE_MATRIX_PROJECTION, (float*)&ge_matrices[GE_MATRIX_PROJECTION]);
//	if(ge_matrix_update[GE_MATRIX_MODEL] || ge_matrix_update[GE_MATRIX_VIEW]){
		SendModelMatrices();
//	}

	geSendMatrix(GE_MATRIX_TEXTURE0, (float*)&ge_matrices[GE_MATRIX_TEXTURE0]);
	geSendMatrix(GE_MATRIX_TEXTURE1, (float*)&ge_matrices[GE_MATRIX_TEXTURE1]);
	geSendMatrix(GE_MATRIX_TEXTURE2, (float*)&ge_matrices[GE_MATRIX_TEXTURE2]);
	geSendMatrix(GE_MATRIX_TEXTURE3, (float*)&ge_matrices[GE_MATRIX_TEXTURE3]);
	geSendMatrix(GE_MATRIX_TEXTURE4, (float*)&ge_matrices[GE_MATRIX_TEXTURE4]);
	geSendMatrix(GE_MATRIX_TEXTURE5, (float*)&ge_matrices[GE_MATRIX_TEXTURE5]);
	geSendMatrix(GE_MATRIX_TEXTURE6, (float*)&ge_matrices[GE_MATRIX_TEXTURE6]);
	geSendMatrix(GE_MATRIX_TEXTURE7, (float*)&ge_matrices[GE_MATRIX_TEXTURE7]);
	
	glActiveTexture(GL_TEXTURE0);
	if(!ge_current_shader)return;
	GLdouble cp[4] = { ge_clip_plane[0], ge_clip_plane[1], ge_clip_plane[2], ge_clip_plane[3] };
#ifndef PLATFORM_mac
	glClipPlane(GL_CLIP_PLANE0, cp);
#endif
}

void geUpdateCurrentMatrix(){
	ge_matrix_update[ge_current_mode] = ge_current_matrix_update;
//	if(ge_current_mode==GE_MATRIX_VIEW || ge_current_mode==GE_MATRIX_MODEL){
		SendModelMatrices();
//	}
	geSendMatrix(ge_current_mode, (float*)&ge_matrices[ge_current_mode]);
}

void geMatrixLocations(){
	if(!ge_current_shader)return;

	loc_mvp = ge_current_shader->loc_mvp;
	loc_model = ge_current_shader->loc_model;
	loc_view = ge_current_shader->loc_view;
	loc_projection = ge_current_shader->loc_projection;
	loc_normal = ge_current_shader->loc_normal;
	loc_modelview = ge_current_shader->loc_modelview;
	loc_texture0 = ge_current_shader->loc_texture0;
	loc_texture1 = ge_current_shader->loc_texture1;
	loc_texture2 = ge_current_shader->loc_texture2;
	loc_texture3 = ge_current_shader->loc_texture3;
	loc_texture4 = ge_current_shader->loc_texture4;
	loc_texture5 = ge_current_shader->loc_texture5;
	loc_texture6 = ge_current_shader->loc_texture6;
	loc_texture7 = ge_current_shader->loc_texture7;
	loc_submodel = ge_current_shader->loc_submodel;
	loc_clipplane = ge_current_shader->loc_clipplane;
}

void geSetMatricesUpdate(int u){
	ge_matrix_update[0] = ge_matrix_update[1] = ge_matrix_update[2] = ge_matrix_update[3] = u;
}

void geSetMatrix(float* m){
	memcpy(ge_current_matrix, m, sizeof(float)*16);
}

void geSendMatrix(int type, float* m){
//	if(type<3 && !ge_matrix_update[type])return;
	if(!ge_current_shader)return;

	int i = -1;

	if(type == GE_MATRIX_MODEL)
		i = loc_model;
	else if(type == GE_MATRIX_SUBMODEL)
		i = loc_submodel;
	else if(type == GE_MATRIX_VIEW)
		i = loc_view;
	else if(type == GE_MATRIX_PROJECTION)
		i = loc_projection;
	else if(type == GE_MATRIX_MODELVIEW)
		i = loc_modelview;
	else if(type == GE_MATRIX_NORMAL){
		glUniformMatrix3fv(loc_normal, 1, GL_FALSE, m);
		return;
	}
	else if(type == GE_MATRIX_TEXTURE0)
		i = loc_texture0;
	else if(type == GE_MATRIX_TEXTURE1)
		i = loc_texture1;
	else if(type == GE_MATRIX_TEXTURE2)
		i = loc_texture2;
	else if(type == GE_MATRIX_TEXTURE3)
		i = loc_texture3;
	else if(type == GE_MATRIX_TEXTURE4)
		i = loc_texture4;
	else if(type == GE_MATRIX_TEXTURE5)
		i = loc_texture5;
	else if(type == GE_MATRIX_TEXTURE6)
		i = loc_texture6;
	else if(type == GE_MATRIX_TEXTURE7)
		i = loc_texture7;
	else if(type == GE_MATRIX_SUBMODEL)
		i = loc_submodel;


//	if(type==GE_MATRIX_MODELVIEW)printf("Updating matrix GE_MATRIX_MODELVIEW (%d)\n", i);
	if(i<0)return;

	glUniformMatrix4fv(i, 1, GL_FALSE, m);

//	ge_matrix_update[type] = 0;
}

float* geGetMatrix(int which){
	return ge_matrices[which];
}

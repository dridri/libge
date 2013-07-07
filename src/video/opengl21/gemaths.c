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
#define PI_180 0.01745329

float geDistance2D(float x1, float y1, float x2, float y2){
	return sqrtf((float)fabs( (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) ));
}

float geDistance3D(float x1, float y1, float z1, float x2, float y2, float z2){
	return (float)sqrt(fabs((double)( (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) + (z2-z1)*(z2-z1) )));
}

float geCos(float angle){
	return cos(angle);
}

float geSin(float angle){
	return sin(angle);
}

float geLog(float x){
	return logf(x);
}

void geVector3FromPoints(float* v, float* a, float* b){
	v[0] = b[0] - a[0];
	v[1] = b[1] - a[1];
	v[2] = b[2] - a[2];
}

void geVector3Mult(float* r, float* a, float* b){
	r[0] = a[1]*b[2] - a[2]*b[1];
	r[1] = a[2]*b[0] - a[0]*b[2];
	r[2] = a[0]*b[1] - a[1]*b[0];
}

#define GE_EPSILON 0.00001f
void geNormalize(float* v){
	/*
	float l = sqrtf((v[0]*v[0]) + (v[1]*v[1]) + (v[2]*v[2]));
	if (l > GE_EPSILON){
		float il = 1.0f / l;
		v[0] *= il; v[1] *= il; v[2] *= il;
	}
	*/
	double l = sqrt((double)( (v[0]*v[0]) + (v[1]*v[1]) + (v[2]*v[2]) ));
	if (l > GE_EPSILON){
		double il = 1.0f / l;
		v[0] *= il; v[1] *= il; v[2] *= il;
	}
}

#define GE_EPSILON 0.00001f
void geNormalized(double* v){
	double l = sqrt((double)( (v[0]*v[0]) + (v[1]*v[1]) + (v[2]*v[2]) ));
	if (l > GE_EPSILON){
		double il = 1.0f / l;
		v[0] *= il; v[1] *= il; v[2] *= il;
	}
}

void geCrossProduct(float* r, const float* a, const float* b){
	r[0] = (a[1] * b[2]) - (a[2] * b[1]);
	r[1] = (a[2] * b[0]) - (a[0] * b[2]);
	r[2] = (a[0] * b[1]) - (a[1] * b[0]);
}

float geDotProduct(const ge_Vector3f* a, const ge_Vector3f* b){
	return (a->x * b->x) + (a->y * b->y) + (a->z * b->z);
}


void geMatrix44Vec4Mult(float* ret, float* mat, float* vec){
	ret[0] = vec[0]*mat[0] + vec[1]*mat[4] + vec[2]*mat[8]  + vec[3]*mat[12];
	ret[1] = vec[0]*mat[1] + vec[1]*mat[5] + vec[2]*mat[9]  + vec[3]*mat[13];
	ret[2] = vec[0]*mat[2] + vec[1]*mat[6] + vec[2]*mat[10] + vec[3]*mat[14];
	ret[3] = vec[0]*mat[3] + vec[1]*mat[7] + vec[2]*mat[11] + vec[3]*mat[15];
}

void geMatrix44Mult(float* matrix, float* matrix0, float* matrix1){
	int i=0, j=0, k=0;
	for(i=0; i<16; i++){
		matrix[i] = matrix0[j]*matrix1[k]+matrix0[j+4]*matrix1[k+1]+matrix0[j+8]*matrix1[k+2]+matrix0[j+12]*matrix1[k+3];
		j++;
		if(j>3){
			j=0;
			k += 4;
		}
	}
}

void geMatrix44Add(float* matrix, float* matrix0, float* matrix1){
	int i=0;
	for(i=0; i<16; i++){
		matrix[i] = matrix0[i] + matrix1[i];
	}
}

void geMatrix44Sub(float* matrix, float* matrix0, float* matrix1){
	int i=0;
	for(i=0; i<16; i++){
		matrix[i] = matrix0[i] - matrix1[i];
	}
}

void ge_LoadIdentity(float*);
void geMatrixPseudoInverse(float* m, const ge_Matrix4f* a){
	ge_Matrix4f t;
	ge_Vector3f negPos = { -a->w.x, -a->w.y, -a->w.z };

	// transpose rotation
	t.x.x = a->x.x;
	t.x.y = a->y.x;
	t.x.z = a->z.x;
	t.x.w = 0;

	t.y.x = a->x.y;
	t.y.y = a->y.y;
	t.y.z = a->z.y;
	t.y.w = 0;

	t.z.x = a->x.z;
	t.z.y = a->y.z;
	t.z.z = a->z.z;
	t.z.w = 0;


	// compute inverse position
	t.w.x = geDotProduct(&negPos, (ge_Vector3f*)&a->x);
	t.w.y = geDotProduct(&negPos, (ge_Vector3f*)&a->y);
	t.w.z = geDotProduct(&negPos, (ge_Vector3f*)&a->z);
	t.w.w = 1.0;

//	ge_LoadIdentity(m);
//	geMatrix44Mult(m, m, (float*)&t);
	memcpy(m, (float*)&t, sizeof(float)*16);
}

static float* temp_matrix = NULL;
void geMatrixInverse(float* ret, float* base_mat, int size){
	if(!temp_matrix)
		temp_matrix = (float*)geMalloc(sizeof(float)*16); //max usable matrix: 4x4


	int i, I, j, k, K;
	int full_size = size * size;

	for(i=0; i<full_size; i++){
		temp_matrix[i] = base_mat[i];
	}

	for(i=0,I=0; i<size; i++, I+=size){
		ret[I+i] = 1.0 / temp_matrix[I+i];
		for(j=0; j<size; j++){
			if(!(u32)temp_matrix[I+i]){ //det is null
			//	gePrintDebug(0x100, "Null matrix determinant\n");
			//	memcpy(ret, base_mat, size*size*sizeof(float));
			//	geMatrixPseudoInverse(ret, (ge_Matrix4f*)base_mat);
			//	return;
			}
			if(j != i)
				ret[I+j] = -temp_matrix[I+j] / temp_matrix[I+i];

			for(k=0,K=0; k<size; k++, K+=size){
				if(k != i)
					ret[K+i] = temp_matrix[K+i] / temp_matrix[I+i];
				if(j!=i && k != i)
					ret[K+j] = temp_matrix[K+j] - temp_matrix[I+j] * temp_matrix[K+i] / temp_matrix[I+i];
			}
		}

		for(k=0; k<full_size; k++){
			temp_matrix[k] = ret[k];
		}
	}
}

void geMatrixTranspose(float* ret, float* base_mat, int size){
	int i, j;
	for(j=0; j<size; j++){
		for(i=0; i<size; i++){
			ret[j*size+i] = base_mat[i*size+j];
		}
	}
}

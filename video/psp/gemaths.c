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
#include <pspvfpu.h>

static int deg_mode = true;
static float PI = 3.1415926535;
static float PI_2 = 1.5707963267;
static float PI_180 = 0.01745329;
extern struct pspvfpu_context* vfpu_context;

void geAngleInDegrees(int en){
	deg_mode = en;
}

float geCos(float val){
	float ret;
	float t = deg_mode? PI_180 : 1.0;
	__asm__ volatile (
		"mtv	 %1, S000\n"
		"mtv	 %2, S001\n"
		"vmul.s  S000, S000, S001\n" // Degres => radians
		"vcst.s  S002, VFPU_2_PI\n"
		"vmul.s  S000, S000, S002\n"
		"vcos.s  S003, S000\n"
		"mfv	 %0, S003\n"
	: "=r"(ret) : "r"(val), "r"(t));
	return ret;
}

float geSin(float val){
	float ret;
	float t = deg_mode? PI_180 : 1.0;
	__asm__ volatile (
		"mtv	 %1, S000\n"
		"mtv	 %2, S001\n"
		"vmul.s  S000, S000, S001\n" // Degres => radians
		"vcst.s  S002, VFPU_2_PI\n"
		"vmul.s  S000, S000, S002\n"
		"vsin.s  S003, S000\n"
		"mfv	 %0, S003\n"
	: "=r"(ret) : "r"(val), "r"(t));
	return ret;
}

float geAcos(float x) {
    float ret;
	float t = deg_mode? PI_180 : 1.0;
    __asm__ volatile (
		"mtv	 %1, S000\n"
		"mtv	 %2, S001\n"
		"vmul.s  S000, S000, S001\n" // Degres => radians
		"vcst.s  S002, VFPU_2_PI\n"
		"vmul.s  S000, S000, S002\n"
		"vasin.s S000, S000\n"
        "vocp.s  S000, S000\n"
		"mfv	 %0, S000\n"
	: "=r"(ret) : "r"(x), "r"(t));
    return ret;
}

float geAsin(float x) {
    float ret;
	float t = deg_mode? PI_180 : 1.0;
    __asm__ volatile (
		"mtv	 %1, S000\n"
		"mtv	 %2, S001\n"
		"vmul.s  S000, S000, S001\n" // Degres => radians
		"vcst.s  S002, VFPU_2_PI\n"
		"vmul.s  S000, S000, S002\n"
		"vasin.s S003, S000\n"
		"mfv	 %0, S003\n"
	: "=r"(ret) : "r"(x), "r"(t));
    return ret;
}

float geAtan(float x) {
	float ret;
	float t = deg_mode? PI_180 : 1.0;
//	ret = asinf(x/sqrt(x*x+1))
	__asm__ volatile (
		"mtv      %1, S000\n" // Get x
		"mtv	  %2, S001\n" // Get PI_180
		"vmul.s   S000, S000, S001\n" // Degres => radians
		"vcst.s   S001, VFPU_PI_2\n"
		"vmul.s   S000, S000, S001\n"

		"vmul.s   S001, S000, S000\n" // x*x
		"vadd.s   S001, S001, S001[1]\n" // (x*x)+1
		"vrsq.s   S001, S001\n"
		"vmul.s   S000, S000, S001\n"
		"vasin.s  S000, S000\n"
		"mfv      %0, S000\n"
	: "=r"(ret) : "r"(x), "r"(t));
	return ret;
}

float geAtan2(float y, float x){
	float ret;
	float abs_x, abs_y;
	float t = deg_mode? PI_180 : 1.0;

	__asm__ volatile(
		"mtv     %2, S000\n"
		"mtv     %3, S001\n"
		"vabs.s  S000, S000\n"
		"vabs.s  S001, S001\n"
		"mfv     %0, S000\n"
		"mfv     %1, S001\n"
	: "=r"(abs_x), "=r"(abs_y) : "r"(x), "r"(y));

	if(abs_x >= abs_y){
//		(Y/X) => ATAN    ==> r = geAtan(y/x);
		__asm__ volatile (
			"mtv      %1, S000\n" // Get x
			"mtv      %2, S001\n" // Get y
			"mtv	  %2, S002\n" // Get PI_180
			"vdiv.s   S000, S001, S000\n" // y / x
			"vmul.s   S000, S000, S002\n" // Degres => radians
			"vcst.s   S001, VFPU_PI_2\n" // ???
			"vmul.s   S000, S000, S001\n" // ???
			"vmul.s   S001, S000, S000\n" // x*x
			"vadd.s   S001, S001, S001[1]\n" // (x*x)+1
			"vrsq.s   S001, S001\n"
			"vmul.s   S000, S000, S001\n"
			"vasin.s  S000, S000\n"
			"mfv      %0, S000\n"
		: "=r"(ret) : "r"(x), "r"(y), "r"(t));

		if(x < 0.0f){
			ret += (y>=0.0f ? PI : -PI);
		}
	}else{
//		(X/Y) => ATAN    ==> r = geAtan(x/y);
		__asm__ volatile (
			"mtv      %1, S000\n" // Get x
			"mtv      %2, S001\n" // Get y
			"mtv	  %2, S002\n" // Get PI_180
			"vdiv.s   S000, S000, S001\n" // y / x
			"vmul.s   S000, S000, S002\n" // Degres => radians
			"vcst.s   S001, VFPU_PI_2\n" // ???
			"vmul.s   S000, S000, S001\n" // ???
			"vmul.s   S001, S000, S000\n" // x*x
			"vadd.s   S001, S001, S001[1]\n" // (x*x)+1
			"vrsq.s   S001, S001\n"
			"vmul.s   S000, S000, S001\n"
			"vasin.s  S000, S000\n"
			"mfv      %0, S000\n"
		: "=r"(ret) : "r"(x), "r"(y), "r"(t));

		ret += (y < 0.0f ? -PI_2 : PI_2);
	}
	return ret;
}

float geFmod(float x, float y) {
	float ret;
	// return x-y*((int)(x/y));
	__asm__ volatile (
		"mtv       %2, S001\n"
		"mtv       %1, S000\n"
		"vrcp.s    S002, S001\n"
		"vmul.s    S003, S000, S002\n"
		"vf2iz.s   S002, S003, 0\n"
		"vi2f.s    S003, S002, 0\n"
		"vmul.s    S003, S003, S001\n"
		"vsub.s    S000, S000, S003\n"
		"mfv       %0, S000\n"
	: "=r"(ret) : "r"(x), "r"(y));
	return ret;
}

float geSqrt(float x){
	float ret;
	__asm__ volatile (
		"mtv	 %1, S000\n"
		//SQRT
		"vsqrt.s  S001, S000\n"
		"mfv	 %0, S001\n"
	: "=r"(ret) : "r"(x));
	return ret;
}

float geMult(float val1, float val2){
	float ret;
	__asm__ volatile (
		"mtv	 %1, S000\n"
		"mtv	 %2, S001\n"
		"vmul.s  S002, S000, S001\n"
		"mfv	 %0, S002\n"
	: "=r"(ret) : "r"(val1), "r"(val2));
	return ret;
}

float geDiv(float val1, float val2){
	float ret;
	__asm__ volatile (
		"mtv	 %1, S000\n"
		"mtv	 %2, S001\n"
		"vdiv.s  S002, S000, S001\n"
		"mfv	 %0, S002\n"
	: "=r"(ret) : "r"(val1), "r"(val2));
	return ret;
}

float geExp(float a0){
	float result;
	__asm__ volatile (
		"mtv	 %1, S000\n"
		"vcst.s  S001, VFPU_LN2\n"
		"vrcp.s  S001, S001\n"
		"vmul.s  S000, S000, S001\n"
		"vexp2.s S000, S000\n"
		"mfv	 %0, S000\n"
	: "=r"(result) : "r"(a0));
	return result;
}

float geLog(float a0){
	float result;
	__asm__ volatile (
		"mtv	 %1, S000\n"
		"vcst.s  S001, VFPU_LOG2E\n"
		"vrcp.s  S001, S001\n"
		"vlog2.s S000, S000\n"
		"vmul.s  S000, S000, S001\n"
		"mfv	 %0, S000\n"
	: "=r"(result) : "r"(a0));
	return result;
}

float gePow(float a0, float a1){
	float result;
	__asm__ volatile (
		"mtv	 %1, S000\n"
		"mtv	 %2, S001\n"
		//Init log
		"vcst.s  S011, VFPU_LOG2E\n"
		"vrcp.s  S011, S011\n"
		//Do Log
		"vlog2.s S001, S001\n"
		"vmul.s  S000, S000, S011\n"
		//Mult the result with a1
		"vmul.s  S000, S000, S001\n"
		//Init exp
		"vcst.s  S010, VFPU_LN2\n"
		"vrcp.s  S010, S010\n"
		//Do Exp
		"vmul.s  S000, S000, S010\n"
		"vexp2.s S000, S000\n"
		"mfv	 %0, S000\n"
	: "=r"(result) : "r"(a0), "r"(a1));

	return result;
//	return geExp(a1 * geLog(a0));
}

float geAbs(float x){
	float ret;
	__asm__ volatile(
		"mtv    %1, S000\n"
		"vabs.s  S000, S000\n"
		"mfv    %0, S000\n"
	: "=r"(ret) : "r"(x));
	return ret;
}

void geMatrix44Mult(float* matrix, float* matrix0, float* matrix1){
	pspvfpu_use_matrices(((LibGE_PspContext*)libge_context->syscontext)->vfpu_context, 0, VMAT0 | VMAT1 | VMAT2);
	__asm__ volatile(
		"ulv.q C000,  0 + %1\n"
		"ulv.q C010, 16 + %1\n"
		"ulv.q C020, 32 + %1\n"
		"ulv.q C030, 48 + %1\n"
		"ulv.q C100,  0 + %2\n"
		"ulv.q C110, 16 + %2\n"
		"ulv.q C120, 32 + %2\n"
		"ulv.q C130, 48 + %2\n"
		"vmmul.q M200, M000, M100\n"
		"usv.q C200,  0 + %0\n"
		"usv.q C210, 16 + %0\n"
		"usv.q C220, 32 + %0\n"
		"usv.q C230, 48 + %0\n"
	: "=m"(*matrix) : "m"(*matrix0), "m"(*matrix1) : "memory");
}

void geMatrix44Add(float* matrix, float* matrix0, float* matrix1){
/*
	int i=0;
	for(i=0; i<16; i++){
		__asm__ volatile ("vadd.s  %0, %1, %2\n" : "=r"(matrix[i]) : "r"(matrix0[i]), "r"(matrix1[i]));
	}
*/
	pspvfpu_use_matrices(((LibGE_PspContext*)libge_context->syscontext)->vfpu_context, 0, VMAT0 | VMAT1 | VMAT2);
	__asm__ volatile(
		"ulv.q C000,  0 + %1\n"
		"ulv.q C010, 16 + %1\n"
		"ulv.q C020, 32 + %1\n"
		"ulv.q C030, 48 + %1\n"
		"ulv.q C100,  0 + %2\n"
		"ulv.q C110, 16 + %2\n"
		"ulv.q C120, 32 + %2\n"
		"ulv.q C130, 48 + %2\n"
		"vadd.q C200, C000, C100\n"
		"vadd.q C210, C010, C110\n"
		"vadd.q C220, C020, C120\n"
		"vadd.q C230, C030, C130\n"
		"usv.q C200,  0 + %0\n"
		"usv.q C210, 16 + %0\n"
		"usv.q C220, 32 + %0\n"
		"usv.q C230, 48 + %0\n"
	: "=m"(*matrix) : "m"(*matrix0), "m"(*matrix1) : "memory");
}

void geMatrix44Sub(float* matrix, float* matrix0, float* matrix1){
/*
	int i=0;
	for(i=0; i<16; i++){
		__asm__ volatile ("vsub.s  %0, %1, %2\n" : "=r"(matrix[i]) : "r"(matrix0[i]), "r"(matrix1[i]));
	}
*/
	pspvfpu_use_matrices(((LibGE_PspContext*)libge_context->syscontext)->vfpu_context, 0, VMAT0 | VMAT1 | VMAT2);
	__asm__ volatile(
		"ulv.q C000,  0 + %1\n"
		"ulv.q C010, 16 + %1\n"
		"ulv.q C020, 32 + %1\n"
		"ulv.q C030, 48 + %1\n"
		"ulv.q C100,  0 + %2\n"
		"ulv.q C110, 16 + %2\n"
		"ulv.q C120, 32 + %2\n"
		"ulv.q C130, 48 + %2\n"
		"vsub.q C200, C000, C100\n"
		"vsub.q C210, C010, C110\n"
		"vsub.q C220, C020, C120\n"
		"vsub.q C230, C030, C130\n"
		"usv.q C200,  0 + %0\n"
		"usv.q C210, 16 + %0\n"
		"usv.q C220, 32 + %0\n"
		"usv.q C230, 48 + %0\n"
	: "=m"(*matrix) : "m"(*matrix0), "m"(*matrix1) : "memory");
}

// vhtfm4.q/3.t/2.p vd, md, vt (homogenous)
// vtfm4.q/3.t/2.p vd, md, vt
// vtfm4.q result, matrix, vector
void geVector4MultMatrix44(float* v, float* m){
	pspvfpu_use_matrices(((LibGE_PspContext*)libge_context->syscontext)->vfpu_context, 0, VMAT0 | VMAT1 | VMAT2);
	__asm__ volatile(
		"ulv.q C210, %0\n"
		"ulv.q C100,  0 + %1\n"
		"ulv.q C110, 16 + %1\n"
		"ulv.q C120, 32 + %1\n"
		"ulv.q C130, 48 + %1\n"
		"vmidt.q M000\n"
		"vmov.s S000, S210\n"
		"vmov.s S010, S211\n"
		"vmov.s S020, S212\n"
		"vmov.s S030, S213\n"

		"vmmul.q M200, M100, M000\n"

		"vmov.s S000, S200\n"
		"vmov.s S001, S210\n"
		"vmov.s S002, S220\n"
		"vmov.s S003, S230\n"
		"usv.q C000, %0\n"
	: "+m"(*v) : "m"(*m) : "memory");
}
/*
void geVector4MultMatrix44(float* v, float* m){
	pspvfpu_use_matrices(libge_context->vfpu_context, 0, VMAT0 | VMAT1 | VMAT2);
	__asm__ volatile(
		"ulv.q C010, %0\n"
		"ulv.q C100,  0 + %1\n"
		"ulv.q C110, 16 + %1\n"
		"ulv.q C120, 32 + %1\n"
		"ulv.q C130, 48 + %1\n"

		"vmul.s S000, S100, S010\n"
		"vmul.s S001, S101, S011\n"
		"vmul.s S002, S102, S012\n"
		"vmul.s S003, S103, S013\n"
		"vadd.s S200, S000, S001\n"
		"vadd.s S200, S200, S002\n"
		"vadd.s S200, S200, S003\n"

		"vmul.s S000, S110, S010\n"
		"vmul.s S001, S111, S011\n"
		"vmul.s S002, S112, S012\n"
		"vmul.s S003, S113, S013\n"
		"vadd.s S201, S000, S001\n"
		"vadd.s S201, S201, S002\n"
		"vadd.s S201, S201, S003\n"

		"vmul.s S000, S120, S010\n"
		"vmul.s S001, S121, S011\n"
		"vmul.s S002, S122, S012\n"
		"vmul.s S003, S123, S013\n"
		"vadd.s S202, S000, S001\n"
		"vadd.s S202, S202, S002\n"
		"vadd.s S202, S202, S003\n"

		"vmul.s S000, S130, S010\n"
		"vmul.s S001, S131, S011\n"
		"vmul.s S002, S132, S012\n"
		"vmul.s S003, S133, S013\n"
		"vadd.s S203, S000, S001\n"
		"vadd.s S203, S203, S002\n"
		"vadd.s S203, S203, S003\n"

		"usv.q C200,  %0\n"
	: "+m"(*v) : "m"(*m) : "memory");
}
*/
#define GE_EPSILON 0.00001f
void geNormalize2(float* v){
	float root = geSqrt((v[0]*v[0]) + (v[1]*v[1]) + (v[2]*v[2]));
	if (root > GE_EPSILON){
		float length = 1.0f / root;
		v[0] *= length; v[1] *= length; v[2] *= length;
	}
}
void geNormalize(float* v){
//	float root = sqrtf((v[0]*v[0]) + (v[1]*v[1]) + (v[2]*v[2]));
//	if (root > GE_EPSILON){
//		float length = 1.0f / root;
//		v[0] *= length v[1] *= length; v[2] *= length;
//	}
	pspvfpu_use_matrices(((LibGE_PspContext*)libge_context->syscontext)->vfpu_context, 0, VMAT0);
	__asm__ volatile (
		"ulv.q  C000, %0\n"
		"vdot.t S010, C000, C000\n"
		"vrsq.s S010, S010\n"
		"vscl.t C000, C000, S010\n"
		"usv.q  C000, %0\n"
	: "+m"(*v));

}

void geCrossProduct2(float* r, const float* a, const float* b){
	r[0] = (a[1] * b[2]) - (a[2] * b[1]);
	r[1] = (a[2] * b[0]) - (a[0] * b[2]);
	r[2] = (a[0] * b[1]) - (a[1] * b[0]);
}
void geCrossProduct(float* r, const float* a, const float* b){
/*
	r[0] = (a[1] * b[2]) - (a[2] * b[1]);
	r[1] = (a[2] * b[0]) - (a[0] * b[2]);
	r[2] = (a[0] * b[1]) - (a[1] * b[0]);
*/
	pspvfpu_use_matrices(((LibGE_PspContext*)libge_context->syscontext)->vfpu_context, 0, VMAT0);
	__asm__ volatile (
		"ulv.q  C010, %1\n"
		"ulv.q  C020, %2\n"
		"vcrsp.t C000, C010, C020\n"
		"usv.q  C000, %0\n"
		: "=m"(*r) : "m"(*a), "m"(*b));
}

float geDistance2D(float x1, float y1, float x2, float y2){
	float ret;
	__asm__ volatile (
		"mtv	 %1, S000\n"
		"mtv	 %2, S001\n"
		"mtv	 %3, S002\n"
		"mtv	 %4, S003\n"
		//(X2-X1)²
		"vsub.s  S000, S002, S000\n"
		"vmul.s  S000, S000, S000\n"
		//(Y2-Y1)²
		"vsub.s  S001, S003, S001\n"
		"vmul.s  S001, S001, S001\n"
		//ADD
		"vadd.s  S002, S000, S001\n"
		//SQRT
		"vsqrt.s  S003, S002\n"
		"mfv	 %0, S003\n"
	: "=r"(ret) : "r"(x1), "r"(y1), "r"(x2), "r"(y2));
	return ret;
}

float geDistance3D(float x1, float y1, float z1, float x2, float y2, float z2){
	float ret;
	__asm__ volatile (
		"mtv	 %1, S000\n" // X1
		"mtv	 %2, S001\n" // Y1
		"mtv	 %3, S002\n" // Z1
		"mtv	 %4, S003\n" // X2
		"mtv	 %5, S010\n" // Y2
		"mtv	 %6, S011\n" // Z2
		//(X2-X1)² => S000
		"vsub.s  S000, S003, S000\n"
		"vmul.s  S000, S000, S000\n"
		//(Y2-Y1)² => S001
		"vsub.s  S001, S010, S001\n"
		"vmul.s  S001, S001, S001\n"
		//(Z2-Z1)² => S002
		"vsub.s  S002, S011, S002\n"
		"vmul.s  S002, S002, S002\n"
		//ADD => S003
		"vadd.s  S003, S000, S001\n"
		"vadd.s  S003, S003, S002\n"
		//SQRT => S000
		"vsqrt.s  S000, S003\n"
		"mfv	 %0, S000\n"
	: "=r"(ret) : "r"(x1), "r"(y1), "r"(z1), "r"(x2), "r"(y2), "r"(z2));
	return ret;
}

void geMatrixTranspose(float* ret, float* base_mat, int size){
	int i, j;
	for(j=0; j<size; j++){
		for(i=0; i<size; i++){
			ret[j*size+i] = base_mat[i*size+j];
		}
	}
}

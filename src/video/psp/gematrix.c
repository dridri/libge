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
//The following functions are based on the PSPGUM because i don't really understand how to use matrices :)

#define GUM_ALIGNED_MATRIX() (ScePspFMatrix4*)((((unsigned int)alloca(sizeof(ScePspFMatrix4)+64)) + 63) & ~63)
#define GUM_ALIGNED_VECTOR() (ScePspFVector4*)((((unsigned int)alloca(sizeof(ScePspFVector4)+64)) + 63) & ~63)

int ge_current_mode = GE_MATRIX_PROJECTION;
int ge_matrix_update[4] = { 0 };
int ge_current_matrix_update = 0;
ScePspFMatrix4 ge_matrices[4];
ScePspFMatrix4* ge_matrix_stack[4] = {
  &ge_matrices[0],
  &ge_matrices[1],
  &ge_matrices[2],
  &ge_matrices[3]
};
ScePspFMatrix4* ge_current_matrix = &ge_matrices[GE_MATRIX_PROJECTION];
float ge_clip_plane[4] = { 0.0 };

void geSendMatrix(int type, float* matrix);

void gePrintDebugMatrix(const char* name, float* matrix){
	gePrintDebug(0x100, "%s = {\n", name);
	gePrintDebug(0x100, "  %f %f %f %f\n", matrix[0], matrix[1], matrix[2], matrix[3]);
	gePrintDebug(0x100, "  %f %f %f %f\n", matrix[4], matrix[5], matrix[6], matrix[7]);
	gePrintDebug(0x100, "  %f %f %f %f\n", matrix[8], matrix[9], matrix[10], matrix[11]);
	gePrintDebug(0x100, "  %f %f %f %f\n", matrix[12], matrix[13], matrix[14], matrix[15]);
	gePrintDebug(0x100, "}\n");
}

bool ge_IsIdentity(float* m){
	if(((((m[0] == m[5]) == m[10]) == m[15]) == 1.0) && ((((((((((((m[1] == m[2]) == m[3]) == m[4]) == m[6]) == m[7]) == m[8]) == m[9]) == m[11]) == m[12]) == m[13]) == m[14]) == 0.0)){
		return true;
	}
	return false;
}

void ge_Translate(float* m, float x, float y, float z){
	float v[3] = { x, y, z };
	pspvfpu_use_matrices(((LibGE_PspContext*)libge_context->syscontext)->vfpu_context, 0, VMAT0 | VMAT1);
	__asm__ volatile (
		"ulv.q C030, %1\n"
		"ulv.q C100,  0 + %0\n"
		"ulv.q C110, 16 + %0\n"
		"ulv.q C120, 32 + %0\n"
		"ulv.q C130, 48 + %0\n"
		"vscl.q	C000, C100, S030\n"
		"vscl.q	C010, C110, S031\n"
		"vscl.q	C020, C120, S032\n"
		"vadd.q	C130, C130, C000\n"
		"vadd.q	C130, C130, C010\n"
		"vadd.q	C130, C130, C020\n"
		"usv.q C130, 48 + %0\n"
		: "+m"(*m) : "m"(*v));
}

void ge_RotateX(float* m, float angle){
	pspvfpu_use_matrices(((LibGE_PspContext*)libge_context->syscontext)->vfpu_context, 0, VMAT0 | VMAT1 | VMAT2);
	__asm__ volatile (
		"vmidt.q M000\n"
		"mtv %1, S100\n"
		"vcst.s S101, VFPU_2_PI\n"
		"vmul.s S100, S101, S100\n"
		"vrot.q C010, S100, [ 0, c, s, 0]\n"
		"vrot.q C020, S100, [ 0,-s, c, 0]\n"

		"ulv.q C100,  0 + %0\n"
		"ulv.q C110, 16 + %0\n"
		"ulv.q C120, 32 + %0\n"
		"ulv.q C130, 48 + %0\n"
		"vmmul.q M200, M100, M000\n"
		"vmmov.q M100, M200\n"
		"usv.q C100,  0 + %0\n"
		"usv.q C110, 16 + %0\n"
		"usv.q C120, 32 + %0\n"
		"usv.q C130, 48 + %0\n"
	: "+m"(*m) : "r"(angle));
	ge_current_matrix_update = 1;
}

void ge_RotateY(float* m, float angle){
	pspvfpu_use_matrices(((LibGE_PspContext*)libge_context->syscontext)->vfpu_context, 0, VMAT0 | VMAT1 | VMAT2);
	__asm__ volatile (
		"vmidt.q M000\n"
		"mtv %1, S100\n"
		"vcst.s S101, VFPU_2_PI\n"
		"vmul.s S100, S101, S100\n"
		"vrot.q C000, S100, [ c, 0,-s, 0]\n"
		"vrot.q C020, S100, [ s, 0, c, 0]\n"

		"ulv.q C100,  0 + %0\n"
		"ulv.q C110, 16 + %0\n"
		"ulv.q C120, 32 + %0\n"
		"ulv.q C130, 48 + %0\n"
		"vmmul.q M200, M100, M000\n"
		"vmmov.q M100, M200\n"
		"usv.q C100,  0 + %0\n"
		"usv.q C110, 16 + %0\n"
		"usv.q C120, 32 + %0\n"
		"usv.q C130, 48 + %0\n"
	: "+m"(*m) : "r"(angle));
	ge_current_matrix_update = 1;
}

void ge_RotateZ(float* m, float angle){
	pspvfpu_use_matrices(((LibGE_PspContext*)libge_context->syscontext)->vfpu_context, 0, VMAT0 | VMAT1 | VMAT2);
	__asm volatile (
		"vmidt.q M000\n"
		"mtv %1, S100\n"
		"vcst.s S101, VFPU_2_PI\n"
		"vmul.s S100, S101, S100\n"
		"vrot.q C000, S100, [ c, s, 0, 0]\n"
		"vrot.q C010, S100, [-s, c, 0, 0]\n"

		"ulv.q C100,  0 + %0\n"
		"ulv.q C110, 16 + %0\n"
		"ulv.q C120, 32 + %0\n"
		"ulv.q C130, 48 + %0\n"
		"vmmul.q M200, M100, M000\n"
		"vmmov.q M100, M200\n"
		"usv.q C100,  0 + %0\n"
		"usv.q C110, 16 + %0\n"
		"usv.q C120, 32 + %0\n"
		"usv.q C130, 48 + %0\n"
	: "+m"(*m) : "r"(angle));
	ge_current_matrix_update = 1;
}

void ge_Rotate(float* m, float rx, float ry, float rz){
	if(rx != 0.0){
		ge_RotateX(m, rx);
	}
	if(ry != 0.0){
		ge_RotateY(m, ry);
	}
	if(rz != 0.0){
		ge_RotateZ(m, rz);
	}
}

void ge_Scale(float* m, float x, float y, float z){
	float v[3] = { x, y, z };
	pspvfpu_use_matrices(((LibGE_PspContext*)libge_context->syscontext)->vfpu_context, 0, VMAT0 | VMAT1);
	__asm__ volatile (
		"ulv.q C000, %1\n"
		"ulv.q C100,  0 + %0\n"
		"ulv.q C110, 16 + %0\n"
		"ulv.q C120, 32 + %0\n"
		"ulv.q C130, 48 + %0\n"
		"vscl.t C100, C100, S000\n"
		"vscl.t C110, C110, S001\n"
		"vscl.t C120, C120, S002\n"
		"usv.q C100,  0 + %0\n"
		"usv.q C110, 16 + %0\n"
		"usv.q C120, 32 + %0\n"
		"usv.q C130, 48 + %0\n"
		: "+m"(*m) : "m"(*v));
}

void ge_LoadIdentity(float* m){
	pspvfpu_use_matrices(((LibGE_PspContext*)libge_context->syscontext)->vfpu_context, 0, VMAT0);
	__asm__ volatile (
		"vmidt.q M000\n"
		"usv.q C000,  0 + %0\n"
		"usv.q C010, 16 + %0\n"
		"usv.q C020, 32 + %0\n"
		"usv.q C030, 48 + %0\n"
	: "=m"(*m) : : "memory" );
}

void ge_Perspective(float* m, float fovy, float aspect, float near, float far){
	pspvfpu_use_matrices(((LibGE_PspContext*)libge_context->syscontext)->vfpu_context, 0, VMAT0 | VMAT1 | VMAT2);
	__asm__ volatile (
		"vmzero.q M100\n"				   // set M100 to all zeros
		"mtv	 %1, S000\n"				// S000 = fovy
		"viim.s  S001, 90\n"				// S002 = 90.0f
		"vrcp.s  S001, S001\n"			  // S002 = 1/90
		"vmul.s  S000, S000, S000[1/2]\n"   // S000 = fovy * 0.5 = fovy/2
		"vmul.s  S000, S000, S001\n"		// S000 = (fovy/2)/90
		"vrot.p  C002, S000, [c, s]\n"	  // S002 = cos(angle), S003 = sin(angle)
		"vdiv.s  S100, S002, S003\n"		// S100 = m->x.x = cotangent = cos(angle)/sin(angle)
		"mtv	 %3, S001\n"				// S001 = near
		"mtv	 %4, S002\n"				// S002 = far
		"vsub.s  S003, S001, S002\n"		// S003 = deltaz = near-far
		"vrcp.s  S003, S003\n"			  // S003 = 1/deltaz
		"mtv	 %2, S000\n"				// S000 = aspect
		"vmov.s  S111, S100\n"			  // S111 = m->y.y = cotangent
		"vdiv.s  S100, S100, S000\n"		// S100 = m->x.x = cotangent / aspect
		"vadd.s  S122, S001, S002\n"		// S122 = m->z.z = far + near
		"vmul.s  S122, S122, S003\n"		// S122 = m->z.z = (far+near)/deltaz
		"vmul.s  S132, S001, S002\n"		// S132 = m->w.z = far * near
		"vmul.s  S132, S132, S132[2]\n"	 // S132 = m->w.z = 2 * (far*near)
		"vmul.s  S132, S132, S003\n"		// S132 = m->w.z = 2 * (far*near) / deltaz
		"vsub.s  S123, S123, S123[1]\n"	 // S123 = m->z.w = -1.0
		"ulv.q C200,  0 + %0\n"
		"ulv.q C210, 16 + %0\n"
		"ulv.q C220, 32 + %0\n"
		"ulv.q C230, 48 + %0\n"
		"vmmul.q M000, M200, M100\n"
		"vmmov.q M200, M000\n"
		"usv.q C200,  0 + %0\n"
		"usv.q C210, 16 + %0\n"
		"usv.q C220, 32 + %0\n"
		"usv.q C230, 48 + %0\n"
	: "+m"(*m) : "r"(fovy),"r"(aspect),"r"(near),"r"(far));
	ge_current_matrix_update = 1;
}

void geLoadIdentity(){
	pspvfpu_use_matrices(((LibGE_PspContext*)libge_context->syscontext)->vfpu_context, VMAT3, 0);
	__asm__ volatile (
		"vmidt.q M300\n"
	);
	ge_current_matrix_update = 1;
}

void geLoadMatrix(float* m){
	pspvfpu_use_matrices(((LibGE_PspContext*)libge_context->syscontext)->vfpu_context, VMAT3, 0);
	__asm__ volatile (
		"ulv.q C300.q,  0 + %0\n"
		"ulv.q C310.q, 16 + %0\n"
		"ulv.q C320.q, 32 + %0\n"
		"ulv.q C330.q, 48 + %0\n"
	: : "m"(*m) : "memory");
	ge_current_matrix_update = 1;
}

float _up[3] = { 0.0, 0.0, 1.0 };
void geVerticalView(float x, float y, float z){
	_up[0] = x;
	_up[1] = y;
	_up[2] = z;
}

void geLookAt(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ){
	ScePspFVector3 eye = { eyeX, eyeY, eyeZ };
	ScePspFVector3 center = { centerX, centerY, centerZ };
	ScePspFVector3 up;
	up.x = _up[0];
	up.y = _up[1];
	up.z = _up[2];

	pspvfpu_use_matrices(((LibGE_PspContext*)libge_context->syscontext)->vfpu_context, VMAT3, VMAT0 | VMAT1 | VMAT2);
	__asm__ volatile (
		"ulv.q   C010, %0\n" // C010 = center
		"ulv.q   C030, %1\n" // C030 = eye
		"vsub.q  C100, C010, C030\n" // center - eye
		"vdot.t  S110, C100, C100\n" //normalize(forward)
		"vrsq.s  S110, S110\n"
		"vscl.t  C100, C100, S110\n" // C100 = forward
		"ulv.q   C020, %2\n" // C020 = up
		"vcrsp.t C010, C100, C020\n" // crossProduct(side, forward, up) C010 = side
		"vdot.t  S000, C010, C010\n" // normalize(side);
		"vrsq.s  S000, S000\n"
		"vscl.t  C010, C010, S000\n" // side = C010
		"vcrsp.t C020, C010, C100\n" // crossProduct(lup, side, forward)
		"vmidt.q M200\n" // loadIdentity(t)
		"vmov.s  S200, S010\n" // t.x.x = side.x
		"vmov.s  S210, S011\n" // t.y.x = side.y
		"vmov.s  S220, S012\n" // t.z.x = side.z
		"vmov.s  S201, S020\n" // t.x.y = lup.x
		"vmov.s  S211, S021\n" // t.y.y = lup.y
		"vmov.s  S221, S022\n" // t.z.y = lup.z
		"vneg.q  C100, C100\n" // forward = -forward
		"vmov.s  S202, S100\n" // t.x.z = -forward.x
		"vmov.s  S212, S101\n" // t.y.z = -forward.y
		"vmov.s  S222, S102\n" // t.z.z = -forward.z
		"vmidt.q M000\n" // M000 = m
		// geMatrix44Mult((float*)&m, (float*)&m, (float*)&t);
		"vmmul.q M100, M000, M200\n" // M100 = m
		"ulv.q   C030, %1\n" // C030 = eye
		"vneg.q  C030, C030\n" // C030 = ieye = -eye
		// ge_Translate(&m, &ieye);
		"vscl.q	 C000, C100, S030\n"
		"vscl.q	 C010, C110, S031\n"
		"vscl.q	 C020, C120, S032\n"
		"vadd.q  C130, C130, C000\n"
		"vadd.q	 C130, C130, C010\n"
		"vadd.q	 C130, C130, C020\n"
		// geMultMatrix((float*)&m);
		"vmmul.q M000, M300, M100\n"
		"vmmov.q M300, M000\n"
	: : "m"(center), "m"(eye), "m"(up) : "memory");

	ge_current_matrix_update = 1;
}


void geMatrixMode(int mode){
	pspvfpu_use_matrices(((LibGE_PspContext*)libge_context->syscontext)->vfpu_context, VMAT3, 0);

	__asm__ volatile (
		"sv.q C300,  0 + %0\n"
		"sv.q C310, 16 + %0\n"
		"sv.q C320, 32 + %0\n"
		"sv.q C330, 48 + %0\n"
	: "=m"(*ge_current_matrix) : : "memory");

	ge_matrix_update[ge_current_mode] = ge_current_matrix_update;
	ge_matrix_stack[ge_current_mode] = ge_current_matrix;
	ge_current_mode = mode;
	ge_current_matrix = ge_matrix_stack[ge_current_mode];
	ge_current_matrix_update = ge_matrix_update[ge_current_mode];

	__asm__ volatile (
		"lv.q C300,  0 + %0\n"
		"lv.q C310, 16 + %0\n"
		"lv.q C320, 32 + %0\n"
		"lv.q C330, 48 + %0\n"
	: : "m"(*ge_current_matrix) : "memory");
}

void geMultMatrix(const float* m){
	pspvfpu_use_matrices(((LibGE_PspContext*)libge_context->syscontext)->vfpu_context, VMAT3, VMAT0 | VMAT1);
	__asm__ volatile (
		"ulv.q C000,  0 + %0\n"
		"ulv.q C010, 16 + %0\n"
		"ulv.q C020, 32 + %0\n"
		"ulv.q C030, 48 + %0\n"
		"vmmul.q M100, M300, M000\n"
		"vmmov.q M300, M100\n"
	: : "m"(*m));
	ge_current_matrix_update = 1;
}

void geOrthogonal(float left, float right, float bottom, float top, float near, float far){
	pspvfpu_use_matrices(((LibGE_PspContext*)libge_context->syscontext)->vfpu_context, VMAT3, VMAT0 | VMAT1);
	__asm__ volatile (
		"vmidt.q M100\n"
		"mtv	 %1, S000\n"
		"mtv	 %3, S001\n"
		"mtv	 %5, S002\n"					// C000 = [right, top,	far ]
		"mtv	 %0, S010\n"
		"mtv	 %2, S011\n"
		"mtv	 %4, S012\n"					// C010 = [left,  bottom, near]
		"vsub.t  C020, C000, C010\n"			// C020 = [  dx,   dy,   dz]
		"vrcp.t  C020, C020\n"				  // C020 = [1/dx, 1/dy, 1/dz]
		"vmul.s  S100, S100[2], S020\n"		 // S100 = m->x.x = 2.0 / dx
		"vmul.s  S111, S111[2], S021\n"		 // S110 = m->y.y = 2.0 / dy
		"vmul.s  S122, S122[2], S022[-x]\n"	 // S122 = m->z.z = -2.0 / dz
		"vsub.t  C130, C000[-x,-y,-z], C010\n"  // C130 = m->w[x, y, z] = [-(right+left), -(top+bottom), -(far+near)]
												// we do vsub here since -(a+b) => (-1*a) + (-1*b) => -a - b
		"vmul.t  C130, C130, C020\n"			// C130 = [-(right+left)/dx, -(top+bottom)/dy, -(far+near)/dz]
		"vmmul.q M000, M300, M100\n"
		"vmmov.q M300, M000\n"
	: : "r"(left), "r"(right), "r"(bottom), "r"(top), "r"(near), "r"(far));
	ge_current_matrix_update = 1;
}

void gePerspective(float fovy, float aspect, float near, float far){
	pspvfpu_use_matrices(((LibGE_PspContext*)libge_context->syscontext)->vfpu_context, VMAT3, VMAT0 | VMAT1);
	__asm__ volatile (
		"vmzero.q M100\n"				   // set M100 to all zeros
		"mtv	 %0, S000\n"				// S000 = fovy
		"viim.s  S001, 90\n"				// S002 = 90.0f
		"vrcp.s  S001, S001\n"			  // S002 = 1/90
		"vmul.s  S000, S000, S000[1/2]\n"   // S000 = fovy * 0.5 = fovy/2
		"vmul.s  S000, S000, S001\n"		// S000 = (fovy/2)/90
		"vrot.p  C002, S000, [c, s]\n"	  // S002 = cos(angle), S003 = sin(angle)
		"vdiv.s  S100, S002, S003\n"		// S100 = m->x.x = cotangent = cos(angle)/sin(angle)
		"mtv	 %2, S001\n"				// S001 = near
		"mtv	 %3, S002\n"				// S002 = far
		"vsub.s  S003, S001, S002\n"		// S003 = deltaz = near-far
		"vrcp.s  S003, S003\n"			  // S003 = 1/deltaz
		"mtv	 %1, S000\n"				// S000 = aspect
		"vmov.s  S111, S100\n"			  // S111 = m->y.y = cotangent
		"vdiv.s  S100, S100, S000\n"		// S100 = m->x.x = cotangent / aspect
		"vadd.s  S122, S001, S002\n"		// S122 = m->z.z = far + near
		"vmul.s  S122, S122, S003\n"		// S122 = m->z.z = (far+near)/deltaz
		"vmul.s  S132, S001, S002\n"		// S132 = m->w.z = far * near
		"vmul.s  S132, S132, S132[2]\n"	 // S132 = m->w.z = 2 * (far*near)
		"vmul.s  S132, S132, S003\n"		// S132 = m->w.z = 2 * (far*near) / deltaz
		"vsub.s  S123, S123, S123[1]\n"	 // S123 = m->z.w = -1.0
	"vmmul.q M000, M300, M100\n"
		"vmmov.q M300, M000\n"
	: : "r"(fovy),"r"(aspect),"r"(near),"r"(far));
	ge_current_matrix_update = 1;
	if(ge_current_mode == GE_MATRIX_PROJECTION){
		memcpy(libge_context->projection_matrix, ge_current_matrix, sizeof(float)*16);
	}
}

void geRotateX(float angle){
	pspvfpu_use_matrices(((LibGE_PspContext*)libge_context->syscontext)->vfpu_context, VMAT3, VMAT0 | VMAT1);
	__asm__ volatile (
		"vmidt.q M000\n"
		"mtv %0, S100\n"
		"vcst.s S101, VFPU_2_PI\n"
		"vmul.s S100, S101, S100\n"
		"vrot.q C010, S100, [ 0, c, s, 0]\n"
		"vrot.q C020, S100, [ 0,-s, c, 0]\n"
		"vmmul.q M100, M300, M000\n"
		"vmmov.q M300, M100\n"
	: : "r"(angle));
	ge_current_matrix_update = 1;
}

void geRotateY(float angle){
	pspvfpu_use_matrices(((LibGE_PspContext*)libge_context->syscontext)->vfpu_context, VMAT3, VMAT0 | VMAT1);
	__asm__ volatile (
		"vmidt.q M000\n"
		"mtv %0, S100\n"
		"vcst.s S101, VFPU_2_PI\n"
		"vmul.s S100, S101, S100\n"
		"vrot.q C000, S100, [ c, 0,-s, 0]\n"
		"vrot.q C020, S100, [ s, 0, c, 0]\n"
		"vmmul.q M100, M300, M000\n"
		"vmmov.q M300, M100\n"
	: : "r"(angle));
	ge_current_matrix_update = 1;
}

void geRotateZ(float angle){
	pspvfpu_use_matrices(((LibGE_PspContext*)libge_context->syscontext)->vfpu_context, VMAT3, VMAT0 | VMAT1);
	__asm volatile (
		"vmidt.q M000\n"
		"mtv %0, S100\n"
		"vcst.s S101, VFPU_2_PI\n"
		"vmul.s S100, S101, S100\n"
		"vrot.q C000, S100, [ c, s, 0, 0]\n"
		"vrot.q C010, S100, [-s, c, 0, 0]\n"
		"vmmul.q M100, M300, M000\n"
		"vmmov.q M300, M100\n"
	: : "r"(angle));
	ge_current_matrix_update = 1;
}

#define ROTATE_BASE(p) \
		"mtv " p ", S100\n" \
		"vcst.s S101, VFPU_2_PI\n" \
		"vmul.s S100, S101, S100\n"

void geRotate(float x, float y, float z){
//	geRotateX(x);
//	geRotateY(y);
//	geRotateZ(z);
	pspvfpu_use_matrices(((LibGE_PspContext*)libge_context->syscontext)->vfpu_context, VMAT3, VMAT0 | VMAT1 | VMAT2 | VMAT4);
	__asm__ volatile (
		"vmidt.q M000\n"
		"vmidt.q M200\n"
		"vmidt.q M400\n"

		//rot X
		"beqz %0, rotate_y\n"
		"nop\n"
		ROTATE_BASE("%0")
		"vrot.q C010, S100, [ 0, c, s, 0]\n"
		"vrot.q C020, S100, [ 0,-s, c, 0]\n"
		"vmmul.q M100, M300, M000\n"
		"vmmov.q M300, M100\n"

		//rot Y
		"rotate_y:\n"
		"beqz %1, rotate_z\n"
		"nop\n"
		ROTATE_BASE("%1")
		"vrot.q C200, S100, [ c, 0,-s, 0]\n"
		"vrot.q C220, S100, [ s, 0, c, 0]\n"
		"vmmul.q M100, M300, M200\n"
		"vmmov.q M300, M100\n"

		//rot Z
		"rotate_z:\n"
		"beqz %2, end\n"
		"nop\n"
		ROTATE_BASE("%2")
		"vrot.q C400, S100, [ c, s, 0, 0]\n"
		"vrot.q C410, S100, [-s, c, 0, 0]\n"
		"vmmul.q M100, M300, M400\n"
		"vmmov.q M300, M100\n"

		"end:\n"
	: : "r"(x), "r"(y), "r"(z));

	ge_current_matrix_update = 1;
}
/*
void geTranslate(float x, float y, float z){
	pspvfpu_use_matrices(libge_context->vfpu_context, VMAT3, VMAT0 | VMAT1);
	ScePspFVector3 v = { x, y, z };
	__asm__ volatile (
		"vmidt.q M000\n"
		"ulv.q   C100, %0\n"
		"vmov.t  C030, C100\n"
		"vmmul.q M100, M300, M000\n"
		"vmmov.q M300, M100\n"
	: : "m"(v));

	ge_current_matrix_update = 1;
}
*/


void geTranslate(float x, float y, float z){
	pspvfpu_use_matrices(((LibGE_PspContext*)libge_context->syscontext)->vfpu_context, VMAT3, VMAT0);
	ScePspFVector3 v = { x, y, z };
	__asm__ volatile (
		"ulv.q C030, %0\n"
		"vscl.q	C000, C300, S030\n"
		"vscl.q	C010, C310, S031\n"
		"vscl.q	C020, C320, S032\n"
		"vadd.q	C330, C330, C000\n"
		"vadd.q	C330, C330, C010\n"
		"vadd.q	C330, C330, C020\n"
	: : "m"(v));

	ge_current_matrix_update = 1;
}

void geScale(float x, float y, float z){
	pspvfpu_use_matrices(((LibGE_PspContext*)libge_context->syscontext)->vfpu_context, VMAT3, VMAT0);
	ScePspFVector3 v = { x, y, z };

	__asm__ volatile (
		"ulv.q C000, %0\n"
		"vscl.t C300, C300, S000\n"
		"vscl.t C310, C310, S001\n"
		"vscl.t C320, C320, S002\n"
	: : "m"(v));

	ge_current_matrix_update = 1;
}

void geUpdateMatrix(){
	ge_matrix_stack[ge_current_mode] = ge_current_matrix;

	if (ge_current_matrix_update){
		pspvfpu_use_matrices(((LibGE_PspContext*)libge_context->syscontext)->vfpu_context, VMAT3, 0);

		__asm__ volatile (
			"sv.q C300,  0 + %0\n"
			"sv.q C310, 16 + %0\n"
			"sv.q C320, 32 + %0\n"
			"sv.q C330, 48 + %0\n"
		: "=m"(*ge_current_matrix) : : "memory");
		ge_matrix_update[ge_current_mode] = ge_current_matrix_update;
		ge_current_matrix_update = 0;
	}

	unsigned int i;
	for (i = 0; i < 4; ++i){
		if (ge_matrix_update[i]){
			geSendMatrix(i, (float*)ge_matrix_stack[i]);
			ge_matrix_update[i] = 0;
		}
	}
}

void geSendMatrix(int type, float* matrix){
	u32 i = 0x0;

	geSendCommandf(CMD_MATRIX_MODEL+type*2, 0.0);
	if(type == GE_MATRIX_PROJECTION){
		for(i = 0; i < 16; i++)
			geSendCommandf(CMD_MATRIX_PROJECTION_ENTRY, matrix[i]);
			
	}else{
		for(i = 0; i < 4; ++i){
			geSendCommandf(CMD_MATRIX_MODEL_ENTRY+type*2, matrix[0+i*4]);
			geSendCommandf(CMD_MATRIX_MODEL_ENTRY+type*2, matrix[1+i*4]);
			geSendCommandf(CMD_MATRIX_MODEL_ENTRY+type*2, matrix[2+i*4]);
		}
	}
}

void geSetMatrix(float* matrix){
	pspvfpu_use_matrices(((LibGE_PspContext*)libge_context->syscontext)->vfpu_context, VMAT3, 0);
	__asm__ volatile (
		"ulv.q C300.q,  0 + %0\n"
		"ulv.q C310.q, 16 + %0\n"
		"ulv.q C320.q, 32 + %0\n"
		"ulv.q C330.q, 48 + %0\n"
	: : "m"(*matrix) : "memory");
	ge_current_matrix_update = 1;
}

float* geGetMatrix(int type){
	if (ge_current_matrix_update){
		pspvfpu_use_matrices(((LibGE_PspContext*)libge_context->syscontext)->vfpu_context, VMAT3, 0);

		__asm__ volatile (
			"sv.q C300,  0 + %0\n"
			"sv.q C310, 16 + %0\n"
			"sv.q C320, 32 + %0\n"
			"sv.q C330, 48 + %0\n"
		: "=m"(*ge_current_matrix) : : "memory");
		ge_matrix_update[ge_current_mode] = ge_current_matrix_update;
		ge_current_matrix_update = 0;
	}
	return (float*)&ge_matrices[type];
}

void geClipPlane(float x, float y, float z, float w){
	ge_clip_plane[0] = x;
	ge_clip_plane[1] = y;
	ge_clip_plane[2] = z;
	ge_clip_plane[3] = w;
}

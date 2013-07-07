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

#include "ge_internal.h"
#include <stdarg.h>

ge_Vector3d vadd(int n, ...){
	ge_Vector3d c = { 0.0, 0.0, 0.0 };
	ge_Vector3d b;

	va_list p;
	va_start(p, n);

	int i = 0;
	for(i=0; i<n; i++){
		b = va_arg(p, ge_Vector3d);
		c.x += b.x;
		c.y += b.y;
		c.z += b.z;
	}

	va_end(p);
	return c;
}

ge_Vector3d vsub(int n, ...){
	ge_Vector3d c = { 0.0, 0.0, 0.0 };
	ge_Vector3d b;

	va_list p;
	va_start(p, n);

	int i = 0;
	for(i=0; i<n; i++){
		b = va_arg(p, ge_Vector3d);
		c.x -= b.x;
		c.y -= b.y;
		c.z -= b.z;
	}

	va_end(p);
	return c;
}

ge_Vector3d vmulk(ge_Vector3d a, double k){
	ge_Vector3d c;
	c.x = a.x * k;
	c.y = a.y * k;
	c.z = a.z * k;
	return c;
}

ge_Vector3d vnorm(ge_Vector3d v){
	ge_Vector3d ret = v;
	double l = sqrt((double)( (v.x*v.x) + (v.y*v.y) + (v.z*v.z) ));
	if (l > 0.0000000000000001){
		double il = 1.0f / l;
		ret.x *= il; ret.y *= il; ret.z *= il;
	}
	return ret;
}

double vdist(ge_Vector3d v1, ge_Vector3d v2){
	/*
	double d = (v1.x-v2.x)*(v1.x-v2.x) + (v1.y-v2.y)*(v1.y-v2.y) + (v1.z-v2.z)*(v1.z-v2.z);
	return sqrt(d);
	*/
	ge_Vector3d v;
	v.x = v1.x - v2.x;
	v.y = v1.y - v2.y;
	v.z = v1.z - v2.z;
	return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

void geRK4Recalculate(ge_RK4State* state){
	state->velocity = vmulk(state->momentum, 1.0 / state->mass);
}
/*
ge_RK4Deriv RK4Evaluate0(ge_RK4State state){
	ge_RK4Deriv output;
	output.velocity = state.velocity;
	output.force = state.force;
	return output;
}

ge_RK4Deriv RK4Evaluate(ge_RK4State state, double t, double dt, ge_RK4Deriv d){
	state.position = vadd(2, state.position, vmulk(d.velocity, dt));
	state.momentum = vadd(2, state.momentum, vmulk(d.force, dt));
	RK4Recalculate(&state);

	ge_RK4Deriv output;
	output.force = state.force;
	output.velocity = state.velocity;
	return output;
}

void RK4Integrate(ge_RK4State* state, double t, double dt){
	ge_RK4Deriv a = RK4Evaluate0(*state);
	ge_RK4Deriv b = RK4Evaluate(*state, t, dt*0.5f, a);
	ge_RK4Deriv c = RK4Evaluate(*state, t, dt*0.5f, b);
	ge_RK4Deriv d = RK4Evaluate(*state, t, dt, c);

//	state.position += 1.0f/6.0f * dt * (a.velocity + 2.0f*(b.velocity + c.velocity) + d.velocity);
//	state.momentum += 1.0f/6.0f * dt * (a.force + 2.0f*(b.force + c.force) + d.force);

	state->position = vadd(2, state->position, vmulk(vadd(3, a.velocity, vmulk(vadd(2, b.velocity, c.velocity), 2.0), d.velocity), 1.0f/6.0f * dt));
	state->momentum = vadd(2, state->momentum, vmulk(vadd(3, a.force, vmulk(vadd(2, b.force, c.force), 2.0), d.force), 1.0f/6.0f * dt));

	RK4Recalculate(state);
}
*/

ge_RK4Deriv geRK4Evaluate0(ge_RK4State state){
	ge_RK4Deriv output;
	output.velocity = state.velocity;
	output.force = state.force;
	return output;
}

ge_RK4Deriv geRK4Evaluate(ge_RK4State state, double t, double dt, ge_RK4Deriv d){
	state.position = vadd(2, state.position, vmulk(d.velocity, dt));
	state.momentum = vadd(2, state.momentum, vmulk(d.force, dt));
	geRK4Recalculate(&state);

	ge_RK4Deriv output;
	output.force = state.force;
	output.velocity = state.velocity;
	return output;
}

void geRK4Integrate(ge_RK4State* state, double t, double dt){
	ge_RK4Deriv a = geRK4Evaluate0(*state);
	ge_RK4Deriv b = geRK4Evaluate(*state, t, dt*1.0/3.0, a);
	ge_RK4Deriv ab;
	ab.force = vadd(2, a.force, b.force);
	ab.velocity = vadd(2, a.velocity, b.velocity);
	ge_RK4Deriv c = geRK4Evaluate(*state, t, dt*1.0/6.0, ab);
	ge_RK4Deriv ac;
	ac.force = vadd(2, a.force, vmulk(c.force, 3.0));
	ac.velocity = vadd(2, a.velocity, vmulk(c.velocity, 3.0));
	ge_RK4Deriv d = geRK4Evaluate(*state, t, dt*1.0/8.0, ac);
	ge_RK4Deriv acd;
	acd.force = vadd(2, a.force, vmulk(d.force, 4.0));
	acd.velocity = vadd(2, vsub(2, a.velocity, vmulk(c.velocity, 3.0)), vmulk(d.velocity, 4.0));
	ge_RK4Deriv e = geRK4Evaluate(*state, t, dt*1.0/2.0, acd);

//	state.position += 1.0f/6.0f * dt * (a.velocity + 4.0f*d.velocity + e.velocity);
//	state.momentum += 1.0f/6.0f * dt * (a.force + 4.0f*d.force + e.force);

	state->position = vadd(2, state->position, vmulk(vadd(3, a.velocity, vmulk(d.velocity, 4.0), e.velocity), 1.0f/6.0f * dt));
	state->momentum = vadd(2, state->momentum, vmulk(vadd(3, a.force, vmulk(d.force, 4.0), e.force), 1.0f/6.0f * dt));

	geRK4Recalculate(state);
}


void geRK4ResetForce(ge_RK4State* state){
	state->force.x = 0.0;
	state->force.y = 0.0;
	state->force.z = 0.0;
}

void geRK4SetForce(ge_RK4State* state, ge_Vector3d vec){
	state->force = vec;
}

void geRK4ApplyForce(ge_RK4State* state, ge_Vector3d vec){
	state->force = vadd(2, state->force, vec);
}

void geRK4ApplyGravity(ge_RK4State* s1, ge_RK4State* s2){
	const double G = 6.67234E-11;

	double d = vdist(s1->position, s2->position);
	if(d <= 1.0 || d < s1->radius_min || d < s2->radius_min){
		return;
	}
	double P = G * ((s1->mass * s2->mass) / (d * d));
	ge_Vector3d force, f1, f2;

	force.x = s2->position.x - s1->position.x;
	force.y = s2->position.y - s1->position.y;
	force.z = s2->position.z - s1->position.z;
	force = vnorm(force);
	force = vmulk(force, P);
	f1.x = force.x;
	f1.y = force.y;
	f1.z = force.z;
	f2.x = -force.x;
	f2.y = -force.y;
	f2.z = -force.z;
	geRK4ApplyForce(s1, f1);
	geRK4ApplyForce(s2, f2);
}

void geRK4AddTarget(ge_RK4State* state, ge_Object* obj){
	ge_RK4Target* curr = NULL;
	if(!state->targets){
		state->targets = (ge_RK4Target*)geMalloc(sizeof(ge_RK4Target));
		curr = state->targets;
	}else{
		curr = state->targets;
		while(curr->next){
			curr = curr->next;
		}
		curr->next = (ge_RK4Target*)geMalloc(sizeof(ge_RK4Target));
		curr = curr->next;
	}
	curr->obj = obj;
}

void geRK4TargetCallback(ge_RK4State* state, int (*cb)(ge_RK4State*, ge_RK4Target*, void*), void* cbdata){
	state->target_cb = cb;
	state->target_cbdata = cbdata;
}

void geRK4UpdateTargets(ge_RK4State* state){
	ge_RK4Target* curr = state->targets;
	float matrix[16];
	ge_LoadIdentity(matrix);
	ge_Translate(matrix, state->position.x, state->position.y, state->position.z);
	while(curr){
		if(state->target_cb){
			state->target_cb(state, curr, state->target_cbdata);
		}else if(curr->obj){
			memcpy(curr->obj->matrix, matrix, sizeof(float) * 16);
			curr->obj->matrix_used = true;
		}
		curr = curr->next;
	}
}

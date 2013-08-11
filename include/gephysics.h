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

#ifndef H_GE_PHYSICS
#define H_GE_PHYSICS

#ifdef __cplusplus
extern "C" {
#endif

#include "getypes.h"

typedef struct ge_RK4Target ge_RK4Target;
struct ge_RK4Target {
	ge_Object* obj;
	ge_RK4Target* next;
};

typedef struct ge_RK4State ge_RK4State;
struct ge_RK4State {
	ge_Vector3d position;	// m
	ge_Vector3d momentum;	// kg/m/s
	ge_Vector3d velocity;	// m/s
	double mass;			// kg
	double radius_min;		// m
	ge_Vector3d force;		// N
	ge_RK4Target* targets;
	int (*target_cb)(ge_RK4State*, ge_RK4Target*, void*);
	void* target_cbdata;
};

typedef struct ge_RK4Deriv {
	ge_Vector3d velocity;	// m/s
	ge_Vector3d force;		// N
} ge_RK4Deriv;

LIBGE_API void geRK4Integrate(ge_RK4State* state, double t, double dt);
LIBGE_API void geRK4AddTarget(ge_RK4State* state, ge_Object* obj);
LIBGE_API void geRK4TargetCallback(ge_RK4State* state, int (*cb)(ge_RK4State*, ge_RK4Target*, void*), void* cbdata);
LIBGE_API void geRK4UpdateTargets(ge_RK4State* state);

LIBGE_API void geRK4ResetForce(ge_RK4State* state);
LIBGE_API void geRK4SetForce(ge_RK4State* state, ge_Vector3d vec);
LIBGE_API void geRK4ApplyForce(ge_RK4State* state, ge_Vector3d vec);
LIBGE_API void geRK4ApplyGravity(ge_RK4State* s1, ge_RK4State* s2);

#ifdef __cplusplus
};
#endif

#endif // H_GE_PHYSICS

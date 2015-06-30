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

#ifndef __GE_TIMER__
#define __GE_TIMER__
#include "getypes.h"

#ifdef __cplusplus
extern "C" {
#endif

LIBGE_API u32 geGetTick();
LIBGE_API float geGetTickFloat();
LIBGE_API int geGetTickResolution();
LIBGE_API void geSleep(int msec);
LIBGE_API void geUSleep(int usec);
LIBGE_API u32 geWaitTick(int tick, u32 last);

LIBGE_API ge_Timer* geCreateTimer(const char* name);
LIBGE_API void geTimerUpdate(ge_Timer* timer);
LIBGE_API void geTimerStart(ge_Timer* timer);
LIBGE_API void geTimerPause(ge_Timer* timer);
LIBGE_API void geTimerStop(ge_Timer* timer);
LIBGE_API void geTimerReset(ge_Timer* timer);
LIBGE_API ge_Timer* geTimerGetByName(char* name);
LIBGE_API void geFreeTimer(ge_Timer* timer);

#ifdef __cplusplus
}
#endif
#endif

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

static int tick_resolution = 0;
static int fps = 0;
static int last_fps = 0;
static u32 last_time = 0;

static u32 rate = 0;
static u32 frames = 0;

void geInitFps(){
	tick_resolution = geGetTickResolution();
}

int geFps(){
	return last_fps;
}

int ge_Calculate_Fps(u32 current_time){
	fps++;
	if((current_time - last_time) >= tick_resolution){
		last_fps = fps;
		fps = 0;
		last_time = current_time;
	}
	return last_fps;
}

void geFpsLimit(int limit){
	rate = 100 * limit / 64;
}

u32 last_ticks = 0;
void ge_Fps_Routine(){
	u32 current_ticks;

	if(rate > 0){
		while( (geGetTick() - last_ticks) <= (1000 / rate) ){
			geUSleep(10);
		}
	}

	current_ticks = geGetTick();
	ge_Calculate_Fps(current_ticks);
	frames++;
	
	last_ticks = geGetTick();
}

u32 geWaitTick(int tick, u32 last){
	u32 ticks = geGetTick();
	if((ticks - last) < tick){
		geSleep(tick - (ticks - last) - 1);
	}
	return geGetTick();
//	return ticks;
}

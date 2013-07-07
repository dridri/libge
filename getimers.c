#include "ge_internal.h"

static ge_Timer* timers[64];
void RegisterTimer(ge_Timer* timer);

ge_Timer* geCreateTimer(const char* name){
	ge_Timer* timer = (ge_Timer*)geMalloc(sizeof(ge_Timer));
	if(!timer)return NULL;

	RegisterTimer(timer);

	return timer;
}

void geFreeTimer(ge_Timer* timer){
	int i = 0;
	for(i=0; i<64; i++){
		if(timers[i] == timer){
			timers[i] = NULL;
		}
	}
	geFree(timer);
}

void geTimerStart(ge_Timer* timer){
	timer->start_time = geGetTick();
	timer->current_time = 0;
	timer->running = true;
}

void geTimerPause(ge_Timer* timer){
	if(timer->running){
		timer->pause_start = geGetTick();
	}else{
		timer->pause_end = geGetTick();
		timer->pause_time = timer->pause_end - timer->pause_start;
		timer->start_time += timer->pause_time;
	}
	timer->running = !timer->running;
}

void geTimerUpdate(ge_Timer* timer){
	if(!timer->running)return;
	timer->current_time = geGetTick();

	timer->ellapsed = timer->current_time - timer->start_time;

	int sec = (int)( timer->ellapsed/geGetTickResolution() );
	timer->seconds = (int)( sec % 60 );
	timer->minuts = (int)( sec / 60  );
	timer->hours = (int)( sec / 3600 );
}

void geTimerStop(ge_Timer* timer){
	timer->running = false;
}

void geTimerReset(ge_Timer* timer){
	memset(timer, 0, sizeof(ge_Timer));
	timer->running = false;
}

void RegisterTimer(ge_Timer* timer){
	int i = 0;
	for(i=0; i<64; i++){
		if(!timers[i]){
			timers[i] = timer;
			break;
		}
	}
}

ge_Timer* geTimerGetByName(char* name){
	int i = 0;
	for(i=0; i<64; i++){
		if(!strcmp(timers[i]->name, name)){
			return timers[i];
		}
	}
	return NULL;
}

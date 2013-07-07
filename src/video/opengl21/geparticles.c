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

static ge_Vertex _ge_particles_vertices[6] = {
	{ 0.0, 0.0, 0.0, { 1.0, 1.0, 1.0, 1.0 }, 0.0, 0.0, 1.0,-0.5,-0.5, 0.0, { 0.0 } },
	{ 1.0, 0.0, 0.0, { 1.0, 1.0, 1.0, 1.0 }, 0.0, 0.0, 1.0, 0.5,-0.5, 0.0, { 0.0 } },
	{ 0.0, 1.0, 0.0, { 1.0, 1.0, 1.0, 1.0 }, 0.0, 0.0, 1.0,-0.5, 0.5, 0.0, { 0.0 } },

	{ 0.0, 1.0, 0.0, { 1.0, 1.0, 1.0, 1.0 }, 0.0, 0.0, 1.0,-0.5, 0.5, 0.0, { 0.0 } },
	{ 1.0, 0.0, 0.0, { 1.0, 1.0, 1.0, 1.0 }, 0.0, 0.0, 1.0, 0.5,-0.5, 0.0, { 0.0 } },
	{ 1.0, 1.0, 0.0, { 1.0, 1.0, 1.0, 1.0 }, 0.0, 0.0, 1.0, 0.5, 0.5, 0.0, { 0.0 } }
};

ge_Particles* geCreateParticles(float x, float y, float z, float radius, float velocity, float life_time, int nParticles, ge_Image* texture){
	ge_Particles* particles = (ge_Particles*)geMalloc(sizeof(ge_Particles));
	particles->loop_mode = true;
	particles->origin.x = x;
	particles->origin.y = y;
	particles->origin.z = z;
	particles->radius = radius;
	particles->direction.x = 0.0;
	particles->direction.y = 1.0;
	particles->direction.z = 0.0;
	particles->velocity = velocity;
	particles->life_time = life_time / (velocity==0.0?1.0:velocity);
	particles->nParticles = nParticles;
	particles->size.x = 1.0;
	particles->size.y = 1.0;
	particles->texture = texture;
	particles->nParticles = nParticles;
	particles->particles = (ge_Particle*)geMalloc(sizeof(ge_Particle)*particles->nParticles);
	geParticlesLifeSeed(particles, 10.0);
	particles->shader = geCreateShader();
	geShaderLoadVertexSource(particles->shader, "default_shaders/particles.vert");
	geShaderLoadFragmentSource(particles->shader, "default_shaders/particles.frag");
	particles->render = geCreateRenderer(particles->shader);
	particles->render->blend_enabled = true;
	particles->render->depth_mask = false;
	ge_Object* obj = (ge_Object*)geMalloc(sizeof(ge_Object));
	particles->render->objs = (ge_Object**)geMalloc(sizeof(ge_Object*));
	particles->render->objs[0] = obj;
	particles->render->nObjs = 1;
	particles->render->nVerts = 6;
	obj->nVerts = 6;
	obj->verts = _ge_particles_vertices;
	particles->loc_size = geShaderUniformID(particles->render->shader, "size");
	particles->loc_pos = geShaderUniformID(particles->render->shader, "pos");
	particles->loc_rpos = geShaderUniformID(particles->render->shader, "rpos");
	particles->loc_life = geShaderUniformID(particles->render->shader, "life");
	particles->loc_textured = geShaderUniformID(particles->render->shader, "textured");
	particles->render->verts = _ge_particles_vertices;
	geRendererCreateContext(NULL, particles->render);

	particles->ut = ((float)geGetTick())/1000.0;
	particles->dt = ((float)geGetTick())/1000.0;
	return particles;
}

void geParticlesLoop(ge_Particles* particles, bool active){
	particles->loop_mode = active;
}

void geParticlesLifeSeed(ge_Particles* particles, float seed){
	int i;
	for(i=0; i<particles->nParticles; i++){
		//particles->particles[i].life_base = 0.0 - (((float)i)/((float)particles->nParticles)) * 100.0;
		particles->particles[i].life_base = 0.0 - ((float)(rand() % 1000) / 1000.0);
		particles->particles[i].life_base *= seed;
		particles->particles[i].life = particles->particles[i].life_base;
	}
}

void geParticlesDirection(ge_Particles* particles, float dirX, float dirY, float dirZ){
	particles->direction.x = dirX;
	particles->direction.y = dirY;
	particles->direction.z = dirZ;
	int i;
	for(i=0; i<particles->nParticles; i++){
		if(((dirX == dirY) == dirZ) == 0.0){
			float a1 = (((float)(rand() % 1000)) / 1000.0) * M_PI;
			float a2 = (((float)(rand() % 1000)) / 1000.0) * M_PI;
			float rtemp = cosf(a1);
			particles->particles[i].dpos.z = geSin(a1);
			particles->particles[i].dpos.x = rtemp*geCos(a2);
			particles->particles[i].dpos.y = rtemp*geSin(a2);
		}else{
			particles->particles[i].dpos.x = dirX;
			particles->particles[i].dpos.y = dirY;
			particles->particles[i].dpos.z = dirZ;
		}
	}
}

void geParticlesSize(ge_Particles* particles, float w, float h){
	particles->size.x = w;
	particles->size.y = h;
}

void geParticlesUpdate(ge_Particles* particles){
	float t = ((float)geGetTick())/1000.0;
	float dt = t - particles->ut;
	particles->ut = t;

	int i;
	for(i=0; i<particles->nParticles; i++){
		particles->particles[i].pos.x += particles->particles[i].dpos.x * dt * particles->velocity;
		particles->particles[i].pos.y += particles->particles[i].dpos.y * dt * particles->velocity;
		particles->particles[i].pos.z += particles->particles[i].dpos.z * dt * particles->velocity;

		float last_life = particles->particles[i].life;
		particles->particles[i].life += dt;
		if(last_life < 0.0 && particles->particles[i].life >= 0.0){
			float r = ((float)(rand() % 1000)) / 1000.0;
			float r2 = ((float)(rand() % 1000)) / 1000.0;
			float r3 = ((float)(rand() % 1000)) / 1000.0;
			r = r * M_PI * 2.0;
			r2 = r2 * M_PI * 2.0;
			float rtemp = geCos(r);
			
			particles->particles[i].pos.x = particles->origin.x + r3 * particles->radius * rtemp * geCos(r2);
			particles->particles[i].pos.y = particles->origin.y + r3 * particles->radius * rtemp * geSin(r2);
			particles->particles[i].pos.z = particles->origin.z + r3 * particles->radius * geSin(r);
			
			/*
			particles->particles[i].pos.x = particles->origin.x;
			particles->particles[i].pos.y = particles->origin.y;
			particles->particles[i].pos.z = particles->origin.z;
			*/
			/*
			particles->particles[i].pos.x = particles->origin.x + r2 * geCos(r) * particles->radius;
			particles->particles[i].pos.y = particles->origin.y;
			particles->particles[i].pos.z = particles->origin.z + r2 * geSin(r) * particles->radius;
			*/
		}
		if(particles->loop_mode && particles->particles[i].life >= particles->life_time){
			particles->particles[i].life = particles->particles[i].life_base;
		}
	}
}

void geParticlesDraw(ge_Particles* particles){
	float t = ((float)geGetTick())/1000.0;
	//float dt = t - particles->dt;
	particles->dt = t;

	geRendererUse(particles->render);
	if(particles->texture){
		geShaderUniform1f(particles->loc_textured, 1.0);
		geTextureImage(0, particles->texture);
	}else{
		geShaderUniform1f(particles->loc_textured, 0.0);
	}

	particles->visible_parts = 0;
	int i;
	for(i=0; i<particles->nParticles; i++){
		if(particles->particles[i].life >= 0.0){
			geShaderUniform1f(particles->loc_life, particles->particles[i].life / particles->life_time);
			geShaderUniform2f(particles->loc_size, particles->size.x, particles->size.y);
			geShaderUniform3f(particles->loc_pos, particles->particles[i].pos.x, particles->particles[i].pos.y, particles->particles[i].pos.z);
			geShaderUniform3f(particles->loc_rpos, particles->particles[i].pos.x-particles->origin.x, particles->particles[i].pos.y-particles->origin.y, particles->particles[i].pos.z-particles->origin.z);
			geDrawArray(GE_TRIANGLES, 0, 6);
			particles->visible_parts++;
		}
	}
}

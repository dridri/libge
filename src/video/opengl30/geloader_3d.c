 /*
	The Game Engine Library is a multiplatform library made to make games
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
void GenerateClouds(ge_Scene*);

void geSceneSetup(ge_Scene* scene){
	int i, j;
	for(i=0; i<scene->nRenderers; i++){
		glUseProgram(scene->renderers[i].shader->programId);
		for(j=0; j<8; j++){
			glActiveTexture(GL_TEXTURE0+j);
			glEnable(GL_TEXTURE_2D);
			char tmp[32] = "ge_Texture";
			if(j)sprintf(tmp, "ge_Texture%d", j);
			glUniform1i(glGetUniformLocation(scene->renderers[i].shader->programId, tmp), j);
		}
		glUseProgram(0);
	}
	
	printf("SKY TEXTURE = \"%s\"\n", ((char*)scene->sky.depth_enabled));
	if(((char*)scene->sky.depth_enabled) != NULL && ((char*)scene->sky.depth_enabled)[0]!=0x0){
		scene->sky.shader = geCreateShader();
		printf("geLoadScene 6\n");
		geShaderLoadVertexSource(scene->sky.shader, "default_shaders/generic_sky.vert");
		printf("geLoadScene 7\n");
		geShaderLoadFragmentSource(scene->sky.shader, "default_shaders/generic_sky.frag");
		printf("geLoadScene 8\n");
		glUseProgram(scene->sky.shader->programId);
		printf("geLoadScene 9\n");
		scene->sky.objs = (ge_Object**)geMalloc(sizeof(ge_Object*));
		printf("geLoadScene 10\n");
		scene->sky.objs[0] = geLoadObject("default_objects/sky.obj");
		scene->sky.objs[0]->material.textures[0] = geLoadImage(((char*)scene->sky.depth_enabled));
		scene->sky.objs[0]->material.textures[0]->flags |= GE_IMAGE_NO_MIPMAPS;
		strcpy(scene->sky.objs[0]->name, "sky");
		scene->sky.objs[0]->vert_start = 0;
		glBindTexture(GL_TEXTURE_2D, scene->sky.objs[0]->material.textures[0]->id);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		scene->sky.nVerts = scene->sky.objs[0]->nVerts;
		scene->sky.verts = scene->sky.objs[0]->verts;
		geRendererCreateContext(&scene->sky);
	}
	
	if(scene->cloudsGenerator){
		scene->cloudsGenerator->shader = geCreateShader();
		geShaderLoadVertexSource(scene->cloudsGenerator->shader, "default_shaders/generic_clouds.vert");
		geShaderLoadFragmentSource(scene->cloudsGenerator->shader, "default_shaders/generic_clouds.frag");
		scene->cloudsGenerator->loc_tex_decal = glGetUniformLocation(scene->cloudsGenerator->shader->programId, "tex_decal");
		scene->cloudsGenerator->loc_cloud_pos = glGetUniformLocation(scene->cloudsGenerator->shader->programId, "cloud_pos");
		glUseProgram(scene->cloudsGenerator->shader->programId);

		scene->cloudsGenerator->objs[0] = geLoadObject("default_objects/cloud.obj");
		scene->cloudsGenerator->objs[0]->material.textures[0] = geLoadImage("default_objects/maps/cirrus.bmp");
		strcpy(scene->cloudsGenerator->objs[0]->name, "high");
		scene->cloudsGenerator->objs[0]->vert_start = 0;
		glBindTexture(GL_TEXTURE_2D, scene->cloudsGenerator->objs[0]->material.textures[0]->id);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		scene->cloudsGenerator->objs[1] = geLoadObject("default_objects/cloud.obj");
		scene->cloudsGenerator->objs[1]->material.textures[0] = geLoadImage("default_objects/maps/cloud.bmp");
		strcpy(scene->cloudsGenerator->objs[1]->name, "mid");
		scene->cloudsGenerator->objs[1]->vert_start = scene->cloudsGenerator->objs[0]->vert_start+scene->cloudsGenerator->objs[0]->nVerts;
		glBindTexture(GL_TEXTURE_2D, scene->cloudsGenerator->objs[1]->material.textures[0]->id);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		scene->cloudsGenerator->objs[2] = geLoadObject("default_objects/cloud.obj");
		scene->cloudsGenerator->objs[2]->material.textures[0] = scene->cloudsGenerator->objs[1]->material.textures[0];
		strcpy(scene->cloudsGenerator->objs[2]->name, "low");
		scene->cloudsGenerator->objs[2]->vert_start = scene->cloudsGenerator->objs[1]->vert_start+scene->cloudsGenerator->objs[1]->nVerts;
		glBindTexture(GL_TEXTURE_2D, scene->cloudsGenerator->objs[2]->material.textures[0]->id);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		scene->cloudsGenerator->nVerts = scene->cloudsGenerator->objs[0]->nVerts+scene->cloudsGenerator->objs[1]->nVerts+scene->cloudsGenerator->objs[2]->nVerts;
		scene->cloudsGenerator->verts = (ge_Vertex*)geMalloc(sizeof(ge_Vertex)*scene->cloudsGenerator->nVerts);
		memcpy(scene->cloudsGenerator->verts, scene->cloudsGenerator->objs[0]->verts, sizeof(ge_Vertex)*scene->cloudsGenerator->objs[0]->nVerts);
		memcpy(&scene->cloudsGenerator->verts[scene->cloudsGenerator->objs[1]->vert_start], scene->cloudsGenerator->objs[1]->verts, sizeof(ge_Vertex)*scene->cloudsGenerator->objs[1]->nVerts);
		memcpy(&scene->cloudsGenerator->verts[scene->cloudsGenerator->objs[2]->vert_start], scene->cloudsGenerator->objs[2]->verts, sizeof(ge_Vertex)*scene->cloudsGenerator->objs[2]->nVerts);
		free(scene->cloudsGenerator->objs[0]->verts);
		free(scene->cloudsGenerator->objs[1]->verts);
		free(scene->cloudsGenerator->objs[2]->verts);

		geRendererCreateContext((ge_Renderer*)scene->cloudsGenerator);
		GenerateClouds(scene);
	}
}

void geRendererCreateContext(ge_Renderer* render){
	//Create buffer
	glGenBuffers(1, &render->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, render->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ge_Vertex)*render->nVerts, render->verts, GL_STATIC_DRAW);

	//Create VAO
	glGenVertexArrays(1, &render->vao);
	glBindVertexArray(render->vao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(ge_Vertex), BUFFER_OFFSET(0));
	glVertexAttribPointer(0, 4, GL_FLOAT, false, sizeof(ge_Vertex), BUFFER_OFFSET(12));
	glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(ge_Vertex), BUFFER_OFFSET(28));
	glVertexAttribPointer(3, 3, GL_FLOAT, false, sizeof(ge_Vertex), BUFFER_OFFSET(40));
	glBindVertexArray(0);

	
	glUseProgram(0/*render->shader->programId*/);
	glLinkProgram(render->shader->programId);

	render->shader->loc_lights = (ge_Gl_Loc_Light*)geMalloc(sizeof(ge_Gl_Loc_Light)* (8/*scene->nDynamicLights*/+scene->nLights));

#define GET_DYNAMIC_LIGHT_UNIFORM(PROG, RET, NAME) \
	sprintf(tmp, "ge_DynamicLights[%d].%s", i, #NAME); \
	RET.loc_##NAME = glGetUniformLocation(PROG, tmp)
#define GET_STATIC_LIGHT_UNIFORM(PROG, RET, NAME) \
	sprintf(tmp, "ge_StaticLights[%d].%s", i, #NAME); \
	RET.loc_##NAME = glGetUniformLocation(PROG, tmp)

	int i;
	for(i=0; i<scene->nDynamicLights; i++){
		GET_DYNAMIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i], position);
		GET_DYNAMIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i], target);
		GET_DYNAMIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i], ambient);
		GET_DYNAMIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i], diffuse);
		GET_DYNAMIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i], specular);
		GET_DYNAMIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i], spotCutoff);
		GET_DYNAMIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i], spotCosCutoff);
		GET_DYNAMIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i], spotExponent);
		GET_DYNAMIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i], attenuation);
	}
	for(i=0; i<scene->nLights; i++){
		GET_STATIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i+8/*i+scene->nDynamicLights*/], position);
		GET_STATIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i+8/*i+scene->nDynamicLights*/], target);
		GET_STATIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i+8/*i+scene->nDynamicLights*/], ambient);
		GET_STATIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i+8/*i+scene->nDynamicLights*/], diffuse);
		GET_STATIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i+8/*i+scene->nDynamicLights*/], specular);
		GET_STATIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i+8/*i+scene->nDynamicLights*/], spotCutoff);
		GET_STATIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i+8/*i+scene->nDynamicLights*/], spotCosCutoff);
		GET_STATIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i+8/*i+scene->nDynamicLights*/], spotExponent);
		GET_STATIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i+8/*i+scene->nDynamicLights*/], attenuation);
		GET_STATIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i+8/*i+scene->nDynamicLights*/], vector);
		GET_STATIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i+8/*i+scene->nDynamicLights*/], targetVector);
		GET_STATIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i+8/*i+scene->nDynamicLights*/], CosInnerMinusOuterAngle);
		GET_STATIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i+8/*i+scene->nDynamicLights*/], CosOuterConeAngle);
	}
}

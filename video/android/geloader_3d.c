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
void GenerateClouds(ge_Scene*);

int geObjectsCountInFile(const char* file);
void geLoadObjectsList(const char* file, const char** list, int count, ge_Object** objs, ge_Animator* anim);

void geSceneSetup(ge_Scene* scene){
	gePrintDebug(0x100, "geSceneSetup 1\n");
	gePrintDebug(0x100, "geSceneSetup 1 (0x%08X)\n", scene);
	gePrintDebug(0x100, "geSceneSetup 1 (%d)\n", scene->nRenderers);
	char tmp[256];
	int k;
	for(k=0; k<scene->nRenderers; k++){
		gePrintDebug(0x100, "geSceneSetup 1..%d.1\n", k);
		ge_Renderer* render = &scene->renderers[k];
	//	glUseProgram(0/*render->shader->programId*/);
	//	glLinkProgram(render->shader->programId);
		gePrintDebug(0x100, "geSceneSetup 1..%d.2\n", k);

		render->shader->loc_lights = (ge_Gl_Loc_Light*)geMalloc(sizeof(ge_Gl_Loc_Light)* (8/*scene->nDynamicLights*/+scene->nLights));
		gePrintDebug(0x100, "geSceneSetup 1..%d.3\n", k);

	#define GET_DYNAMIC_LIGHT_UNIFORM(PROG, RET, NAME) \
		sprintf(tmp, "ge_DynamicLights[%d].%s", i, #NAME); \
		RET.loc_##NAME = glGetUniformLocation(PROG, tmp); \
		gePrintDebug(0x100, "renderer %d: %s: %d\n", k, tmp, RET.loc_##NAME);
	#define GET_STATIC_LIGHT_UNIFORM(PROG, RET, NAME) \
		sprintf(tmp, "ge_StaticLights[%d].%s", i, #NAME); \
		RET.loc_##NAME = glGetUniformLocation(PROG, tmp); \
		gePrintDebug(0x100, "renderer %d: %s: %d\n", k, tmp, RET.loc_##NAME);

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
		gePrintDebug(0x100, "geSceneSetup 1..%d.5\n", k);
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
		gePrintDebug(0x100, "geSceneSetup 1..%d.6\n", k);
	}

	/*
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
	*/
	glActiveTexture(GL_TEXTURE0);
	printf("a\n");
	printf("scene: 0x%08X\n", (u32)scene);
	if(((char*)scene->sky.animator)){
		gePrintDebug(0x100, "SKY TEXTURE = \"%s\"\n", ((char*)scene->sky.animator));
	}
	printf("b\n");
	if(((char*)scene->sky.animator) != NULL && ((char*)scene->sky.animator)[0]!=0x0){
		scene->sky.shader = geCreateShader();
		geShaderLoadVertexSource(scene->sky.shader,_ge_BuildPath(libge_context->default_shaders_path, "generic_sky.vert"));
		geShaderLoadFragmentSource(scene->sky.shader, _ge_BuildPath(libge_context->default_shaders_path, "generic_sky.frag"));
		glUseProgram(scene->sky.shader->programId);

		/*
		scene->sky.objs = (ge_Object**)geMalloc(sizeof(ge_Object*));
		if(scene->sky.nVerts == -2){
			scene->sky.objs[0] = geLoadObject("default_objects/skysphere.obj");
		}else{
			scene->sky.objs[0] = geLoadObject("default_objects/sky.obj");
		}
		*/
		char fl[2048];
		if(scene->sky.nVerts == -2){
			strcpy(fl, "default_objects/skysphere.obj");
		}else{
			strcpy(fl, "default_objects/sky.obj");
		}
		scene->sky.nObjs = geObjectsCountInFile(fl);
		scene->sky.objs = (ge_Object**)geMalloc(sizeof(ge_Object*) * scene->sky.nObjs);
		int i;
		for(i=0; i<scene->sky.nObjs; i++){
			scene->sky.objs[i] = (ge_Object*)geMalloc(sizeof(ge_Object));
		}
		geLoadObjectsList(fl, NULL, scene->sky.nObjs, scene->sky.objs, NULL);

		scene->sky.objs[0]->material.textures[0] = geLoadImage(((char*)scene->sky.animator));
		scene->sky.objs[0]->material.textures[0]->flags |= GE_IMAGE_NO_MIPMAPS;
		if(strstr((char*)scene->sky.animator, "star")){
			scene->sky.objs[0]->material.textures[1] = geLoadImage("default_objects/maps/generic_stars_2.png");
			scene->sky.objs[0]->material.textures[1]->flags |= GE_IMAGE_NO_MIPMAPS;
		}
		/*
		strcpy(scene->sky.objs[0]->name, "sky");
		scene->sky.objs[0]->vert_start = 0;
		glBindTexture(GL_TEXTURE_2D, scene->sky.objs[0]->material.textures[0]->id);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		scene->sky.nVerts = scene->sky.objs[0]->nVerts;
		scene->sky.verts = scene->sky.objs[0]->verts;
		*/
		
		scene->sky.nVerts = 0;
		int a;
		for(a=0; a<scene->sky.nObjs; a++){
			scene->sky.nVerts += scene->sky.objs[a]->nVerts;
		}
		gePrintDebug(0x100, "scene->sky.nObjs: %d\n", scene->sky.nObjs);
		gePrintDebug(0x100, "scene->sky.nVerts: %d\n", scene->sky.nVerts);
		
		scene->sky.verts = (ge_Vertex*)geMalloc(sizeof(ge_Vertex)*scene->sky.nVerts);
		int v = 0;
		for(a=0; a<scene->sky.nObjs; a++){
			memcpy(&scene->sky.verts[v], scene->sky.objs[a]->verts, sizeof(ge_Vertex)*scene->sky.objs[a]->nVerts);
			geFree(scene->sky.objs[a]->verts);
			scene->sky.objs[a]->verts = &scene->sky.verts[v];
			scene->sky.objs[a]->vert_start = v;
			v += scene->sky.objs[a]->nVerts;
		}
		
		scene->sky.depth_enabled = false;
		scene->sky.blend_enabled = false;
		scene->sky.enabled = true;
		geRendererCreateContext(scene, &scene->sky);
	}
	
	if(scene->cloudsGenerator){
		scene->cloudsGenerator->shader = geCreateShader();
		geShaderLoadVertexSource(scene->cloudsGenerator->shader, _ge_BuildPath(libge_context->default_shaders_path, "generic_clouds.vert"));
		geShaderLoadFragmentSource(scene->cloudsGenerator->shader, _ge_BuildPath(libge_context->default_shaders_path, "generic_clouds.frag"));
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
		geFree(scene->cloudsGenerator->objs[0]->verts);
		geFree(scene->cloudsGenerator->objs[1]->verts);
		geFree(scene->cloudsGenerator->objs[2]->verts);

		geRendererCreateContext(scene, (ge_Renderer*)scene->cloudsGenerator);
	//	GenerateClouds(scene);
	}
}

void geRendererCreateContext(ge_Scene* scene, ge_Renderer* render){
	//Create VBO
	bool first = false;
	if(render->vbo){
		int mem = 0;
		glBindBuffer(GL_ARRAY_BUFFER, render->vbo);
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &mem);
		libge_context->gpumem -= mem;
	}
	if(!render->vbo){
		first = true;
		int id;
		glGenBuffers(1, (GLuint*)&id);
		render->vbo = id;
	}
	glBindBuffer(GL_ARRAY_BUFFER, render->vbo);
	if(render->customVert){
		glBufferData(GL_ARRAY_BUFFER, render->customVert->size*render->nVerts, render->verts, render->memory_mode);
		libge_context->gpumem += render->customVert->size*render->nVerts;
	}else{
		glBufferData(GL_ARRAY_BUFFER, sizeof(ge_Vertex)*render->nVerts, render->verts, render->memory_mode);
		libge_context->gpumem += sizeof(ge_Vertex)*render->nVerts;
	}
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	
	/*
	if(first){
		int i;
		glUseProgram(0);
		glLinkProgram(render->shader->programId);

		if(scene){
			for(i=0; i<scene->nDynamicLights; i++){
			}
			for(i=0; i<scene->nLights; i++){
			}
		}
		glUseProgram(render->shader->programId);
		for(i=0; i<8; i++){
			glActiveTexture(GL_TEXTURE0+i);
			glEnable(GL_TEXTURE_2D);
			char tmp[32] = "ge_Texture";
			if(i)sprintf(tmp, "ge_Texture%d", i);
			glUniform1i(glGetUniformLocation(render->shader->programId, tmp), i);
		}
		glUseProgram(0);
	}
	*/
}

void geRendererUpdateContext(ge_Scene* scene, ge_Renderer* render){
	int mem = 0;
	glBindBuffer(GL_ARRAY_BUFFER, render->vbo);
	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &mem);
	libge_context->gpumem -= mem;
	if(render->customVert){
		glBufferData(GL_ARRAY_BUFFER, render->customVert->size*render->nVerts, render->verts, render->memory_mode);
		libge_context->gpumem += render->customVert->size*render->nVerts;
	}else{
		glBufferData(GL_ARRAY_BUFFER, sizeof(ge_Vertex)*render->nVerts, render->verts, render->memory_mode);
		libge_context->gpumem += sizeof(ge_Vertex)*render->nVerts;
	}
}

void geRendererLinkLight(ge_Renderer* render, ge_Light* light){
	if(!render || !light || (light->isDynamic && render->nDynamicLights >= 8))return;
	int i, k=0;
	char tmp[128];

	gePrintDebug(0x100, "geRendererLinkLight 1\n");

	render->lights = (ge_Light**)geRealloc(render->lights, sizeof(ge_Light*) * (render->nLights + 8/*render->nDynamicLights*/ + !light->isDynamic));
	render->shader->loc_lights = (ge_Gl_Loc_Light*)geRealloc(render->shader->loc_lights, sizeof(ge_Gl_Loc_Light) * (8/*scene->nDynamicLights*/+render->nLights + !light->isDynamic));
	gePrintDebug(0x100, "geRendererLinkLight 2\n");
	
	if(light->isDynamic){
		for(i=0; i<(render->nDynamicLights+render->nLights); i++);
		for(i--; i>render->nDynamicLights; i--){
			render->lights[i]->i_loc = render->lights[i - 1]->i_loc;
			render->lights[i] = render->lights[i - 1];
			memcpy(&render->shader->loc_lights[i], &render->shader->loc_lights[i - 1], sizeof(ge_Gl_Loc_Light));
		}
		i = render->nDynamicLights;
		light->i_loc = i;
		GET_DYNAMIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i], flags);
		GET_DYNAMIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i], position);
		GET_DYNAMIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i], target);
		GET_DYNAMIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i], ambient);
		GET_DYNAMIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i], diffuse);
		GET_DYNAMIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i], specular);
		GET_DYNAMIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i], spotCutoff);
		GET_DYNAMIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i], spotCosCutoff);
		GET_DYNAMIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i], spotExponent);
		GET_DYNAMIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i], attenuation);
		GET_DYNAMIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i], shadow);
		render->lights[render->nDynamicLights] = light;
		render->nDynamicLights++;
	}else{
		gePrintDebug(0x100, "geRendererLinkLight 3\n");
		i = render->nLights;
		light->i_loc = i+8;
		GET_STATIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i+8], flags);
		GET_STATIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i+8], position);
		GET_STATIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i+8], target);
		GET_STATIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i+8], ambient);
		GET_STATIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i+8], diffuse);
		GET_STATIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i+8], specular);
		GET_STATIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i+8], spotCutoff);
		GET_STATIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i+8], spotCosCutoff);
		GET_STATIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i+8], spotExponent);
		GET_STATIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i+8], attenuation);
		GET_STATIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i+8], shadow);
		GET_STATIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i+8], vector);
		GET_STATIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i+8], targetVector);
		GET_STATIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i+8], CosInnerMinusOuterAngle);
		GET_STATIC_LIGHT_UNIFORM(render->shader->programId, render->shader->loc_lights[i+8], CosOuterConeAngle);
		gePrintDebug(0x100, "geRendererLinkLight 4\n");
		render->lights[render->nLights+8] = light;
		render->nLights++;
		gePrintDebug(0x100, "geRendererLinkLight 5\n");
	}
}

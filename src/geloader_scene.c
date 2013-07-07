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

ge_Scene* geLoadScene_ge_text(const char* file);

#define LGES_len 21
#define _GE_TYPE_TEXT_ 0
#define _GE_TYPE_BINARY_ 1
#define getIntBuffer(b,a) ((int)((u8)b[a] | (u16)(b[a+1]<<8) | (u32)(b[a+2]<<16) | (u32)(b[a+3]<<24)))
#define getShortBuffer(b,a) ((short)((u8)b[a] | (u16)(b[a+1]<<8)))

void geSceneInit(ge_Scene* scene);
void PrecalculateLight(ge_Light* light);


ge_Scene* geLoadScene(const char* file){
	gePrintDebug(0x100, "geLoadScene 1\n");
	ge_File* fp = geFileOpen(file, GE_FILE_MODE_READ | GE_FILE_MODE_BINARY);
	gePrintDebug(0x100, "geLoadScene 1.2\n");
	u8 buffer[48] = { 0x0 };
	gePrintDebug(0x100, "geLoadScene 1.3\n");
	geFileRead(fp, buffer, 48);
	gePrintDebug(0x100, "geLoadScene 1.4 (%s)\n", (char*)buffer);
	geFileClose(fp);
	gePrintDebug(0x100, "geLoadScene 2\n");

	char check[LGES_len] = "";
	int version = 0;
	int type = -1;

	ge_Scene* scene = NULL;
	gePrintDebug(0x100, "geLoadScene 3\n");

	if(buffer[0]==0x0e){
		strcpy(check, (char*)&buffer[1]);
		if(!strcmp(check, "LIB_GAME_ENGINE_SCENE")){
			type = _GE_TYPE_BINARY_;
		}
		version = getShortBuffer(buffer, 0x20);
	}else
	if(!strncmp((char*)buffer, "LIB_GAME_ENGINE_SCENE", LGES_len)){
		gePrintDebug(0x100, "geLoadScene type text\n");
		type = _GE_TYPE_TEXT_;
		geGetParamInt((char*)buffer, "version", &version);
		gePrintDebug(0x100, "geLoadScene version %d\n", version);
	}
	gePrintDebug(0x100, "geLoadScene 4\n");
	
	if(type == _GE_TYPE_BINARY_){
		if(version == 1){
		}
	}else
	if(type == _GE_TYPE_TEXT_){
		if(version == 1){
			scene = geLoadScene_ge_text(file);
		}
	}
	gePrintDebug(0x100, "geLoadScene 5\n");
	geSceneSetup(scene);
	

	gePrintDebug(0x100, "Scene loaded !\n");
	return scene;
}

ge_Renderer* geSceneRenderer(ge_Scene* scene, const char* name){
	int i;
	for(i=0; i<scene->nRenderers; i++){
		if(!strcmp(scene->renderers[i].name, name)){
			return &scene->renderers[i];
		}
	}
	return NULL;
}

ge_Renderer* geCreateRenderer(ge_Shader* shader){
	ge_Renderer* render = (ge_Renderer*)geMalloc(sizeof(ge_Renderer));
	render->shader = shader;
	render->enabled = true;
	render->depth_enabled = true;
	render->depth_mask = true;
	render->draw_mode = GE_TRIANGLES;
	render->memory_mode = GE_STATIC_DRAW;
	return render;
}

void geRendererLinkObject(ge_Renderer* render, ge_Object* obj){
	render->objs = (ge_Object**)geRealloc(render->objs, sizeof(ge_Object*)*(render->nObjs+1));
	render->objs[render->nObjs] = obj;
	render->nObjs++;

	render->verts = (ge_Vertex*)geRealloc(render->verts, sizeof(ge_Vertex)*(render->nVerts+obj->nVerts));
	memcpy(&render->verts[render->nVerts], obj->verts, sizeof(ge_Vertex)*obj->nVerts);
	geFree(obj->verts);
	obj->verts = &render->verts[render->nVerts];
	obj->vert_start = render->nVerts;
	render->nVerts += obj->nVerts;
}

void geRendererAddObject(ge_Renderer* render, ge_Object* obj){
	ge_Object* newobj = (ge_Object*)geMalloc(sizeof(ge_Object));
	memcpy(newobj, obj, sizeof(ge_Object));
	geRendererLinkObject(render, newobj);
}

void geRendererUnlinkObject(ge_Renderer* render, ge_Object* obj){
	int i;
	for(i=0; i<render->nObjs; i++){
		if(render->objs[i] == obj){
			break;
		}
	}
	if(i == render->nObjs){
		return;
	}

	obj->verts = (ge_Vertex*)geMalloc(sizeof(ge_Vertex)*obj->nVerts);
	memcpy(obj->verts, &render->verts[obj->vert_start], sizeof(ge_Vertex)*obj->nVerts);
	memmove(&render->verts[obj->vert_start], &render->verts[obj->vert_start+obj->nVerts], obj->nVerts);
	render->verts = (ge_Vertex*)geRealloc(render->verts, sizeof(ge_Vertex)*(render->nVerts-obj->nVerts));

	render->objs[i] = render->objs[render->nObjs-1];
	render->objs = (ge_Object**)geRealloc(render->objs, sizeof(ge_Object*)*(render->nObjs-1));
	render->nObjs--;
	render->nVerts -= obj->nVerts;
}

void geRendererRemoveObject(ge_Renderer* render, ge_Object* obj){
	geRendererUnlinkObject(render, obj);
	geFree(obj->verts);
	geFree(obj);
}

int geRendererObjectIndex(ge_Renderer* render, const char* name){
	int index = -1;
	int i = 0;
	for(i=0; i<render->nObjs; i++){
		gePrintDebug(0x100, "geRendererObjectIndex: \"%s\"\n", render->objs[i]->name);
		if(!strcmp(render->objs[i]->name, name)){
			index = i;
			break;
		}
	}
	return index;
}

ge_Light* geCreateLight(float x, float y, float z, u32 diffuse, u32 ambient){
	ge_Light* light = (ge_Light*)geMalloc(sizeof(ge_Light));
	light->type = GE_LIGHT_TYPE_OMNIDIRECTIONNAL;
	light->used = true;
	light->attenuation = -2.0;
	light->position.x = x;
	light->position.y = y;
	light->position.z = z;
	light->position.w = 1.0;
	light->target.w = 1.0;
	SET_COLOR(light->diffuse, diffuse);
	SET_COLOR(light->ambient, ambient);
	return light;
}

void geLightSpot(ge_Light* light, float innerAngler, float exponent){
	light->spot_cutoff = innerAngler * M_PI / 180.0;
	light->spot_coscutoff = geCos(light->spot_cutoff);
	PrecalculateLight(light);
}

void geFreeScene(ge_Scene* scene){
	int i, j, k;
	for(i=0; i<scene->nRenderers; i++){
		for(j=0; j<scene->renderers[i].nObjs; j++){
			for(k=0; k<8; k++){
				if(scene->renderers[i].objs[j]->material.textures[k]){
					geFree(scene->renderers[i].objs[j]->material.textures[k]);
				}
			}
			geFree(scene->renderers[i].objs[j]);
		}
		geFree(scene->renderers[i].objs);
		geFree(scene->renderers[i].lights);
		geFreeShader(scene->renderers[i].shader);
	}
	geFree(scene->renderers);
	geFree(scene->lights);
	geFree(scene);
}

void set_object_color(ge_Scene* scene, ge_Object* obj, u32 color){
	int i = 0;
	for(i=0; i<obj->nVerts; i++){
		SET_COLOR(obj->verts[i].color, color);
	}
}

void transformPath(const char* path, const char* buffer, char* out){
	int st = 0;
	for(st=0; st<strlen(buffer); st++){
		if(buffer[st]=='='){
			st+=3;
			break;
		}
	}
#ifdef WIN32
	int en = 2;
#else
	int en = 3;
#endif
	int i = 0;
	char tmp[2048] = "";
	strcpy(tmp, path);
	for(i=st; i<strlen(buffer)-en; i++){
		tmp[strlen(path)+i-st] = buffer[i];
	}
	strcpy(out, tmp);
}

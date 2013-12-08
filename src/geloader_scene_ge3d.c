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
#include <math.h>

#define PASS_COMMENTS \
	if(buffer[0]=='#')continue; \
	if(buffer[0]=='/' && buffer[1]=='/')continue; \
	if(buffer[0]=='/' && buffer[1]=='*'){ \
		while(geFileGets(fp, buffer, 1024) && !(buffer[0]=='*'&&buffer[1]=='/')); \
	}

void transformPath(const char* path, const char* buffer, char* out);
ge_Object* loadObjectOBJ(const char* file, const char* obj_name);
void set_object_color(ge_Scene* scene, ge_Object* obj, u32 color);
void PrecalculateLight(ge_Light* light);

#define RGB_to_floats(rgb,floats)	\
	floats[0]=(float)rgb[0]/255;	\
	floats[1]=(float)rgb[1]/255;	\
	floats[2]=(float)rgb[2]/255


void LoadLight(ge_File* fp, ge_Light* light){
	char buffer[1024] = "";
	char options[32][64] = { "" };
	int rgba[4] = { 0 };
	float frgba[4] = { 0 };
	int v = 0, i;
	memset(light, 0, sizeof(ge_Light));
	light->position.w = light->target.w = 1.0;
	light->attenuation = -2.0;

	while(geFileGets(fp, buffer, 1024)){
		PASS_COMMENTS;
		if(strstr(buffer, "END_LIGHT"))break;
		if(geGetParamInt(buffer, "spot", &v)){
			if(v)light->type=GE_LIGHT_TYPE_SPOT;
		}
		if(geGetParamIntMulti(buffer, "diffuse", rgba, 3)){
			for(i=0; i<4; i++){
				frgba[i] = (float)rgba[i];
			}
			SET_COLOR_RGBAf(light->diffuse, frgba[0] / 255.0, frgba[1] / 255.0, frgba[2] / 255.0, 0.0);
		}
		if(geGetParamIntMulti(buffer, "ambient", rgba, 3)){
			for(i=0; i<4; i++){
				frgba[i] = (float)rgba[i];
			}
			SET_COLOR_RGBAf(light->ambient, frgba[0] / 255.0, frgba[1] / 255.0, frgba[2] / 255.0, 1.0);
		}
		if(geGetParamIntMulti(buffer, "specular", rgba, 3)){
			for(i=0; i<4; i++){
				frgba[i] = (float)rgba[i];
			}
			SET_COLOR_RGBAf(light->specular, frgba[0] / 255.0, frgba[1] / 255.0, frgba[2] / 255.0, 1.0);
		}
		if(strstr(buffer, "options")){
			int n = geGetStringList(buffer, (char**)options, 64, 32);
			int i;
			for(i=1; i<n; i++){
				if(!strcmp(options[i], "SHADOWS")){
					light->flags |= GE_LIGHT_HAVE_SHADOW;
				}
			}
		}
		geGetParamFloatMulti(buffer, "pos", &light->position.x, 3);
		geGetParamFloatMulti(buffer, "spot_dir", &light->target.x, 3);
		geGetParamFloat(buffer, "spot_cutoff", &light->spot_cutoff);
		geGetParamFloat(buffer, "spot_exponent", &light->spot_exponent);
		geGetParamFloat(buffer, "attenuation", &light->attenuation);
	}
	light->spot_cutoff = light->spot_cutoff * M_PI / 180.0;
	light->spot_coscutoff = geCos(light->spot_cutoff / 1.0);
	light->used = true;
}

int geObjectsCountInFile(const char* file);
int geObjectsCountInFileSubs(const char* file, const char** list, int list_count);
void geObjectsListInRenderer(const char* file, const char** list, int count, ge_Renderer* render);
//ge_Object* geLoadObjectList(const char* file, const char** list, int count);
void geLoadObjectsList(const char* file, const char** list, int count, ge_Object** objs, ge_Animator* anim);
#define GE_TYPE_OBJ 1
#define GE_TYPE_DAE 2
int ObjGetType(const char* file);

void LoadRenderer(const char* path, ge_File* fp, ge_Scene* scene, ge_Renderer* render){
	gePrintDebug(0x100, "LoadRenderer(\"%s\", 0x%16llX, 0x%16llX, 0x%16llX)\n", path, (t_ptr)fp, (t_ptr)scene, (t_ptr)render);
	char buffer[2048] = "";
	char tmp[2048] = "";
	char fl[2048] = "";
//	ge_Object* tmp_obj = NULL;
	gePrintDebug(0x100, "LoadRenderer 1\n");

	render->shader = geCreateShader();
	render->depth_enabled = true;
	render->depth_mask = true;
	render->blend_enabled = false;
	render->enabled = true;
	render->matrix_used = false;
	render->draw_mode = GE_TRIANGLES;
	render->memory_mode = GE_STATIC_DRAW;
	gePrintDebug(0x100, "LoadRenderer 2\n");

	while(geFileGets(fp, buffer, 2048)){
		PASS_COMMENTS;
		if(strstr(buffer, "END_RENDERER"))break;
		geGetParamInt(buffer, "enabled", &render->enabled);
		geGetParamInt(buffer, "depth_enabled", &render->depth_enabled);
		geGetParamInt(buffer, "blend_enabled", &render->blend_enabled);
		geGetParamInt(buffer, "depth_mask", &render->depth_mask);
		if(geGetParamString(buffer, "extension", tmp, 2048)){
			float f = 0.0;
			if(geGetParamFloat(buffer, "WATER", &f)){
				geWaterInit(render, f);
			}
		}
		
		if(strstr(buffer, "view")){
			float params[3];
			if(geGetParamFloatMulti(buffer, "view", params, 3)){
				render->matrix_used = true;
				ge_LoadIdentity(render->projection_matrix);
				ge_Perspective(render->projection_matrix, params[0], (float)geGetContext()->width/(float)geGetContext()->height, params[1], params[2]);
			}
		}

		if(geGetParamString(buffer, "objects", tmp, 2048)){
			gePrintDebug(0x100, " LoadRenderer B\n");
			sprintf(fl, "%s%s", path, tmp);
			char** obj_list = (char**)geMalloc(sizeof(char*)*256);
			int a = 0;
			for(a=0; a<256; a++){
				obj_list[a] = (char*)geMalloc(sizeof(char)*128);
			}
			int cnt = geGetStringList(buffer, (char**)obj_list, 128, 256);

			if(cnt > 1){
				memset(obj_list[0],0x0,128); // Pas the "xxxxx.xxx"
				render->nObjs = geObjectsCountInFileSubs(fl, (const char**)obj_list, cnt);
			}else if(cnt == 1){
				render->nObjs = geObjectsCountInFile(fl);
				cnt = 0;
			}
			gePrintDebug(0x100, " LoadRenderer C\n");
			render->objs = (ge_Object**)geMalloc(sizeof(ge_Object*)*render->nObjs);
			for(a=0; a<render->nObjs; a++){
				render->objs[a] = (ge_Object*)geMalloc(sizeof(ge_Object));
			}
			if(ObjGetType(fl) == GE_TYPE_DAE){
				render->animator = (ge_Animator*)geMalloc(sizeof(ge_Animator));
			}
			gePrintDebug(0x100, " LoadRenderer D\n");
			geLoadObjectsList(fl, (const char**)obj_list, cnt, render->objs, render->animator);
			gePrintDebug(0x100, " LoadRenderer E\n");

			for(a=0; a<render->nObjs; a++){
				render->nVerts += render->objs[a]->nVerts;
			}
			gePrintDebug(0x100, " LoadRenderer F\n");
			render->verts = (ge_Vertex*)geMalloc(sizeof(ge_Vertex)*render->nVerts);
			gePrintDebug(0x100, " LoadRenderer G\n");
			int v = 0;
			for(a=0; a<render->nObjs; a++){
				memcpy(&render->verts[v], render->objs[a]->verts, sizeof(ge_Vertex)*render->objs[a]->nVerts);
				geFree(render->objs[a]->verts);
				render->objs[a]->verts = &render->verts[v];
				render->objs[a]->vert_start = v;
				v += render->objs[a]->nVerts;
			}
			gePrintDebug(0x100, " LoadRenderer H\n");
		}
		if(geGetParamString(buffer, "vertex_shader", tmp, 2048)){
			gePrintDebug(0x100, " LoadRenderer F\n");
			if(!strncmp(tmp, "generic", 7)){
				sprintf(fl, "%s/%s.vert", libge_context->default_shaders_path, tmp);
			}else{
				sprintf(fl, "%s%s", path, tmp);
			}
			gePrintDebug(0x100, "file: \"%s\"\n", fl);
			geShaderLoadVertexSource(render->shader, fl);
		}
		if(geGetParamString(buffer, "tesselation_control_shader", tmp, 2048)){
			gePrintDebug(0x100, " LoadRenderer D\n");
			if(!strncmp(tmp, "generic", 7)){
				sprintf(fl, "%s/%s.tess", libge_context->default_shaders_path, tmp);
			}else{
				sprintf(fl, "%s%s", path, tmp);
			}
			gePrintDebug(0x100, "file: \"%s\"\n", fl);
			geShaderLoadTessControlSource(render->shader, fl);
			render->tesselated = true;
		}
		if(geGetParamString(buffer, "tesselation_evaluation_shader", tmp, 2048)){
			gePrintDebug(0x100, " LoadRenderer E\n");
			if(!strncmp(tmp, "generic", 7)){
				sprintf(fl, "%s/%s.tess", libge_context->default_shaders_path, tmp);
			}else{
				sprintf(fl, "%s%s", path, tmp);
			}
			gePrintDebug(0x100, "file: \"%s\"\n", fl);
			geShaderLoadTessEvaluationSource(render->shader, fl);
			render->tesselated = true;
		}
		if(geGetParamString(buffer, "geometry_shader", tmp, 2048)){
			gePrintDebug(0x100, " LoadRenderer C\n");
			if(!strncmp(tmp, "generic", 7)){
				sprintf(fl, "%s/%s.geom", libge_context->default_shaders_path, tmp);
			}else{
				sprintf(fl, "%s%s", path, tmp);
			}
			gePrintDebug(0x100, "file: \"%s\"\n", fl);
			geShaderLoadGeometrySource(render->shader, fl);
		}
		if(geGetParamString(buffer, "fragment_shader", tmp, 2048)){
			gePrintDebug(0x100, " LoadRenderer G\n");
			if(!strncmp(tmp, "generic", 7)){
				sprintf(fl, "%s/%s.frag", libge_context->default_shaders_path, tmp);
			}else{
				sprintf(fl, "%s%s", path, tmp);
			}
			gePrintDebug(0x100, "file: \"%s\"\n", fl);
			geShaderLoadFragmentSource(render->shader, fl);
		}
	}

//#ifndef PLATFORM_mac
	if(render->tesselated){
		render->draw_mode = GE_PATCHES;
	}
//#endif
	gePrintDebug(0x100, "LoadRenderer 3\n");
	geRendererCreateContext(scene, render);
	gePrintDebug(0x100, "LoadRenderer 4\n");
}

ge_Scene* geLoadScene_ge_text(const char* file){
	ge_File* fp = NULL;
	if( !(fp = geFileOpen(file, GE_FILE_MODE_READ | GE_FILE_MODE_BINARY))){
		return NULL;
	}
	char path[2048] = "";
	strncpy(path, file, strlen(file));
	if(strstr(path, "/")){
		int A = 0;
		for(A=strlen(path); A>=0; A--){
			if(path[A]=='/')break;
			path[A] = 0x0;
		}
	}
	ge_Scene* scene = (ge_Scene*)geMalloc(sizeof(ge_Scene));
	ge_Fog* fog = (ge_Fog*)geMalloc(sizeof(ge_Fog));
	memset(scene, 0, sizeof(ge_Scene));
	memset(fog, 0, sizeof(ge_Fog));
	scene->fog = fog;

	int i = 0;
	int rgba[4] = { -1 };
	char buffer[2048] = "";
	char tmp[2048] = "";
	int tmp_ivec[4];
	scene->cloudsGenerator = NULL;

	while(geFileGets(fp, buffer, 2048)){
		PASS_COMMENTS;

		if(strstr(buffer, "RENDERER") && !strstr(buffer, "END_RENDERER")){
			scene->nRenderers++;
			continue;
		}

		if(strstr(buffer, "LIGHT") && !strstr(buffer, "END_LIGHT")){
			if(strstr(buffer,"dynamic")){
				scene->nDynamicLights++;
			}else{
				scene->nLights++;
			}
			continue;
		}

		if(geGetParamIntMulti(buffer, "clear_color", rgba, 3)){
			SET_COLOR_RGBAf(scene->clear_color, rgba[0], rgba[1], rgba[2], 1.0);
		}
		if(geGetParamIntMulti(buffer, "material_ambient", rgba, 3)){
			SET_COLOR_RGBAf(scene->material_ambient, rgba[0], rgba[1], rgba[2], 1.0);
		}
		if(geGetParamIntMulti(buffer, "material_diffuse", rgba, 3)){
			SET_COLOR_RGBAf(scene->material_diffuse, rgba[0], rgba[1], rgba[2], 1.0);
		}
		if(geGetParamString(buffer, "skysphere", tmp, 2048)){
			if(tmp[0] != 0x0 && tmp[0] != '\n'){
				gePrintDebug(0x100, "sky : \"%s\"\n", tmp);
				scene->sky.animator = (ge_Animator*)geMalloc(sizeof(char)*2048);
				scene->sky.nVerts = -2;
				if(strstr(tmp, "generic")){
					sprintf(((char*)scene->sky.animator), "default_objects/maps/%s.png", tmp);
				}else{
					sprintf(((char*)scene->sky.animator), "%s%s", path, tmp);
				}
			}
		}else if(geGetParamString(buffer, "sky", tmp, 2048)){
			if(tmp[0] != 0x0 && tmp[0] != '\n'){
				gePrintDebug(0x102, "sky : \"%s\"\n", tmp);
				scene->sky.animator = (ge_Animator*)geMalloc(sizeof(char)*2048);
				if(strstr(tmp, "generic")){
					sprintf(((char*)scene->sky.animator), "default_objects/maps/%s.png", tmp);
				}else{
					sprintf(((char*)scene->sky.animator), "%s%s", path, tmp);
				}
			}
		}

		if(strstr(buffer,"fog"))scene->fogEnabled=true;
		if(strstr(buffer, "fog_mode = ")){
			if(strstr(buffer, "LINEAR"))fog->mode=GE_FOG_LINEAR;
			if(strstr(buffer, "EXP2"))fog->mode=GE_FOG_EXP2;
			if(strstr(buffer, "EXP"))fog->mode=GE_FOG_EXP;
		}
		if(geGetParamIntMulti(buffer, "fog_color", rgba, 3)){
			SET_COLOR_RGBAf(fog->color, rgba[0], rgba[1], rgba[2], 1.0);
		}
		geGetParamFloat(buffer, "fog_density", &fog->density);
		geGetParamFloat(buffer, "fog_start", &fog->start);
		geGetParamFloat(buffer, "fog_end", &fog->end);

		if(strstr(buffer, "CLOUDS_GENERATOR") && !strstr(buffer, "END_CLOUDS_GENERATOR")){
			gePrintDebug(0x100, " LoadRenderer A\n");
			scene->cloudsGenerator = (ge_CloudsGenerator*)geMalloc(sizeof(ge_CloudsGenerator));
			while(geFileGets(fp, buffer, 2048)){
				PASS_COMMENTS;
				if(strstr(buffer, "END_CLOUDS_GENERATOR"))break;
				geGetParamIntMulti(buffer, "map_size", &scene->cloudsGenerator->map_size_x, 2);
				if(strstr(buffer, "CLOUDS_LEVEL")){
					geGetParamString(buffer, "CLOUDS_LEVEL", tmp, 2048);
					int type = -1;
					if(!strcmp(tmp, "high")){
						type = GE_CLOUD_TYPE_HIGH_LEVEL;
					}else
					if(!strcmp(tmp, "mid")){
						type = GE_CLOUD_TYPE_MID_LEVEL;
					}else
					if(!strcmp(tmp, "low")){
						type = GE_CLOUD_TYPE_LOW_LEVEL;
					}else{
						break;
					}
					while(geFileGets(fp, buffer, 2048)){
						PASS_COMMENTS;
						if(strstr(buffer, "END_CLOUDS_LEVEL"))break;
						if(geGetParamIntMulti(buffer, "size_range", tmp_ivec, 2)){
							scene->cloudsGenerator->size_min[type] = tmp_ivec[0];
							scene->cloudsGenerator->size_max[type] = tmp_ivec[1];
						}
						if(geGetParamIntMulti(buffer, "parts_range", tmp_ivec, 2)){
							scene->cloudsGenerator->parts_min[type] = tmp_ivec[0];
							scene->cloudsGenerator->parts_max[type] = tmp_ivec[1];
						}
						geGetParamInt(buffer, "n_clouds", &scene->cloudsGenerator->n_clouds[type]);
					}
				}
			}
		}
	}
	geFileRewind(fp);
	gePrintDebug(0x100, "scene 1\n");

	gePrintDebug(0x100, "scene->nLights %d\n", scene->nLights);
	scene->lights = (ge_Light*)geMalloc(sizeof(ge_Light)*(scene->nLights + 8));
	scene->dynamicLights = scene->lights;
//	scene->dynamicLights = (ge_Light*)geMalloc(sizeof(ge_Light)*scene->nDynamicLights);
	int iS=8, iD=0;
	while(geFileGets(fp, buffer, 1024)){
		PASS_COMMENTS;
		if(strstr(buffer, "LIGHT") && !strstr(buffer, "END_LIGHT")){
			if(strstr(buffer,"dynamic")){
				LoadLight(fp, &scene->lights[iD]);
				scene->lights[iD].isDynamic = true;
				iD++;
			}else{
				LoadLight(fp, &scene->lights[iS]);
				scene->lights[iS].isDynamic = false;
				PrecalculateLight(&scene->lights[iS]);
				iS++;
			}
		}
	}
	gePrintDebug(0x100, "scene 2\n");
	geFileRewind(fp);
	
	scene->renderers = (ge_Renderer*)geMalloc(sizeof(ge_Renderer)*scene->nRenderers);
	gePrintDebug(0x100, "scene 2.1\n");
	i = 0;
	while(geFileGets(fp, buffer, 1024)){
		PASS_COMMENTS;
		if(strstr(buffer, "RENDERER") && !strstr(buffer, "END_RENDERER")){
			memset(&scene->renderers[i], 0, sizeof(ge_Renderer));
			geGetParamString(buffer, "RENDERER", scene->renderers[i].name, 64);
			gePrintDebug(0x100, "scene 2.2.%d.1\n", i);
			LoadRenderer(path, fp, scene, &scene->renderers[i]);
			gePrintDebug(0x100, "scene 2.2.%d.2\n", i);
			i++;
			if(i>scene->nRenderers)break;
		}
	}
	gePrintDebug(0x100, "scene 3\n");

	geFileClose(fp);
	
	return scene;
}

void PrecalculateLight(ge_Light* light){
	float cos_inner_cone_angle = light->spot_coscutoff;
	float cos_outer_cone_angle = cos_inner_cone_angle - 0.002*light->spot_exponent;
	float cos_inner_minus_outer_angle = cos_inner_cone_angle - cos_outer_cone_angle;
	light->CosOuterConeAngle = cos_outer_cone_angle;
	light->CosInnerMinusOuterAngle = cos_inner_minus_outer_angle;
}

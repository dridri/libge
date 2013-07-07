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

void geSceneSetup(ge_Scene* scene){
	int k;
	int i;
	gePrintDebug(0x100, "geSceneSetup(%p) (%d)\n", scene, scene->nRenderers);
	for(k=0; k<scene->nRenderers; k++){
		ge_Renderer* render = &scene->renderers[k];
		if(!render->lights){
			gePrintDebug(0x100, "nLights : %d\n", scene->nLights);
			render->lights = (ge_Light**)geMalloc(sizeof(ge_Light*) * scene->nLights);
			for(i=0; i<scene->nLights; i++){
				render->lights[i] = &scene->lights[i + 8];
			}
			for(i=0; i<scene->nLights; i++){
				gePrintDebug(0x100, "{ %08X }\n", render->lights[i]->diffuse);
			}
			render->nLights = scene->nLights;
		}
		gePrintDebug(0x100, "renderer %d : %d\n", k, render->nLights);
	}


	gePrintDebug(0x100, "SKY TEXTURE = \"%s\"\n", ((char*)scene->sky.depth_enabled));
	if(((char*)scene->sky.depth_enabled) != NULL && ((char*)scene->sky.depth_enabled)[0]!=0x0){
	}
	
	if(scene->cloudsGenerator){
	}
}

void geRendererCreateContext(ge_Scene* scene, ge_Renderer* render){
}

void geRendererLinkLight(ge_Renderer* render, ge_Light* light){
	if(!render || !light || render->nLights >= 4)return;
	int i;

	gePrintDebug(0x100, "geRendererLinkLight 1\n");

	render->lights = (ge_Light**)geRealloc(render->lights, sizeof(ge_Light*) * (render->nLights + 1));
	gePrintDebug(0x100, "geRendererLinkLight 2\n");
	
	i = render->nLights;
	render->lights[render->nLights] = light;
	render->nLights++;
}

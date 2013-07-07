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

ge_AnimSampler* FindSamplerByTargetName(ge_Animator* animator, const char* target_name){
	int i = 0;
	for(i=0; i<animator->nSamplers; i++){
		if(!animator->samplers[i].target){
			continue;
		}
		if(!strcmp(target_name, animator->samplers[i].target->name)){
			return &animator->samplers[i];
		}
	}
	return NULL;
}

ge_AnimSampler* geSceneAnimationSampler(ge_Scene* scene, const char* target_name){
	int i = 0;
	ge_AnimSampler* sampler = NULL;
	for(i=0; i<scene->nRenderers; i++){
		sampler = FindSamplerByTargetName(scene->renderers[i].animator, target_name);
		if(sampler){
			break;
		}
	}
	return sampler;
}

ge_AnimNode* FindNode(ge_AnimNode* root, const char* name){
	if(!strcmp(root->name, name)){
		return root;
	}
	int i = 0;
	for(i=0; i<root->nChilds; i++){
		ge_AnimNode* node = FindNode(&root->childs[i], name);
		if(node){
			return node;
		}
	}
	return NULL;
}

ge_AnimNode* geSceneAnimationNode(ge_Scene* scene, const char* name){
	int i = 0;
	for(i=0; i<scene->nRenderers; i++){
		ge_AnimNode* node = FindNode(&scene->renderers[i].animator->root, name);
		if(node){
			return node;
		}
	}
	return NULL;
}

void geSceneAnimationMode(ge_Scene* scene, const char* target_name, int mode){
	ge_AnimSampler* sampler = geSceneAnimationSampler(scene, target_name);
	if(!sampler)return;
	sampler->mode = mode;
	memset(&sampler->timer, 0x0, sizeof(ge_Timer));
}

void geAnimationStart(ge_AnimSampler* sampler){
	if(sampler->timer.running)return;
	geTimerReset(&sampler->timer);
	geTimerStart(&sampler->timer);
}

void geAnimationStop(ge_AnimSampler* sampler){
	geTimerReset(&sampler->timer);
}

void geAnimationInit(ge_AnimNode* node){
	int i = 0;

	memcpy(node->matrix, node->default_matrix, sizeof(float)*16);

	for(i=0; i<node->nChilds; i++){
		geAnimationInit(&node->childs[i]);
	}
}



int CheckAnimation(ge_Animator* animator, ge_AnimNode* node, float* matrix){
	int i = 0;
	int k = 0;
	for(i=0; i<animator->nSamplers; i++){
	//	gePrintDebug(0x100, "COMPARE \"%s\" with \"%s\"\n", animator->samplers[i].target->name, name);
		if(animator->samplers[i].target == node){
	//	if(!strcmp(animator->samplers[i].target->name, name)){
			ge_AnimSampler* sampler = &animator->samplers[i];
			if(sampler->mode == GE_ANIMATION_MODE_ONCE){
				if(!sampler->timer.running){
					ge_LoadIdentity(matrix);
					break;
				}
			}
			if(sampler->mode == GE_ANIMATION_MODE_LOOP){
				if(!sampler->timer.running){
					geAnimationStart(sampler);
				}
			}
			geTimerUpdate(&sampler->timer);
			float time = (float)sampler->timer.ellapsed / (float)geGetTickResolution();
			time *= sampler->speed;
			for(k=0; k<(sampler->nTimes-1); k++){
			//	if(time > sampler->times[k] && time < sampler->times[k+1]){
			//		geAnimationTransform(sampler->target, &sampler->matrices[k*16]);
			//	}
				if(time >= sampler->times[k] && time < sampler->times[k+1]){
					if(sampler->interpolations[k] == GE_ANIMATION_LINEAR){
						float factor = (time-sampler->times[k]) * 1.0 / (sampler->times[k+1] - sampler->times[k]);
						int m;
						for(m=0; m<16; m++){
							matrix[m] = (sampler->matrices[k*16+m]*(1.0-factor) + sampler->matrices[(k+1)*16+m]*factor);
						}
						return 1;
					}
				}
			}
			if(time >= sampler->times[sampler->nTimes-1]){
				if(sampler->mode == GE_ANIMATION_MODE_LOOP){
					geTimerReset(&sampler->timer);
					geTimerStart(&sampler->timer);
				}
				if(sampler->mode == GE_ANIMATION_MODE_ONCE){
					geTimerStop(&sampler->timer);
				}
			}
			break;
		}
	}
	return 0;
}

void geAnimationTransform(ge_Animator* animator, ge_AnimNode* node, float* matrix){
	int i = 0;
//	gePrintDebug(0x100, "geAnimationTransform(\"%s\", ...)\n", node->name);

	float tmp_mat[16];
	float anim_matrix[16];
	
	if(CheckAnimation(animator, node, anim_matrix)){
		geMatrix44Mult(node->matrix, matrix, anim_matrix);
	}else{
		memcpy(tmp_mat, node->matrix, sizeof(float)*16);
		geMatrix44Mult(node->matrix, matrix, tmp_mat);
	}

	if(node->target){
		memcpy(node->target->matrix, node->matrix, sizeof(float)*16);
		node->target->matrix_used = true;
	}

	for(i=0; i<node->nChilds; i++){
		geAnimationTransform(animator, &node->childs[i], node->matrix);
	//	geAnimationTransform(animator, &node->childs[i], matrix);
	}
}

void geRendererAnimate(ge_Renderer* render){
	float matrix[16];
	int a;

	ge_LoadIdentity(render->animator->root.default_matrix);
	geAnimationInit(&render->animator->root);

	for(a=0; a<render->animator->root.nChilds; a++){
		ge_LoadIdentity(matrix);
		geAnimationTransform(render->animator, &render->animator->root.childs[a], matrix);
	}
}

void geSceneAnimate(ge_Scene* scene){
	int j = 0;
	int a = 0;

	for(j=0; j<scene->nRenderers; j++){
		if(!scene->renderers[j].animator){
			continue;
		}
		ge_LoadIdentity(scene->renderers[j].animator->root.default_matrix);
		geAnimationInit(&scene->renderers[j].animator->root);
		float matrix[16];
		ge_LoadIdentity(matrix);
	//	geAnimationTransform(render->animator->root, matrix);
	}

	for(j=0; j<scene->nRenderers; j++){
		if(!scene->renderers[j].animator){
			continue;
		}
		for(a=0; a<scene->renderers[j].animator->root.nChilds; a++){
			float matrix[16];
			ge_LoadIdentity(matrix);
			geAnimationTransform(scene->renderers[j].animator, &scene->renderers[j].animator->root.childs[a], matrix);
		}
	}
}



/*

void geAnimationTransform(ge_AnimNode* node, float* matrix){
	int i = 0;
	gePrintDebug(0x100, "geAnimationTransform(\"%s\", ...)\n", node->name);
	PrintMatrix("  matrix :", matrix, 4);
	gePrintDebug(0x100, "  }\n");
	PrintMatrix("  node :", node->matrix, 4);
	gePrintDebug(0x100, "  }\n");
	gePrintDebug(0x100, "\n");
//	ge_LoadIdentity(node->matrix);
	float tmp_mat[16];
	memcpy(tmp_mat, node->matrix, sizeof(float)*16);
	geMatrix44Mult(node->matrix, matrix, tmp_mat);
//	geMatrix44Mult(node->matrix, tmp_mat, matrix);
	if(node->target){
		memcpy(node->target->matrix, node->matrix, sizeof(float)*16);
	}

	for(i=0; i<node->nChilds; i++){
		geAnimationTransform(&node->childs[i], node->matrix);
	//	geAnimationTransform(&node->childs[i], matrix);
	}
}


void geSceneAnimate(ge_Scene* scene){
	int i = 0;
	int j = 0;
	int k = 0;

	for(j=0; j<scene->nRenderers; j++){
		for(i=0; i<scene->renderers[j].animator->nSamplers; i++){
			ge_AnimSampler* sampler = &scene->renderers[j].animator->samplers[i];
			geAnimationInit(sampler->target);
		}
	}

	for(j=0; j<scene->nRenderers; j++){
		for(i=0; i<scene->renderers[j].animator->nSamplers; i++){
	//	for(i=scene->renderers[j].animator->nSamplers-1; i>=0; i--){
			ge_AnimSampler* sampler = &scene->renderers[j].animator->samplers[i];
			if(sampler->mode == GE_ANIMATION_MODE_ONCE){
			}
			if(sampler->mode == GE_ANIMATION_MODE_LOOP){
				if(!sampler->timer.running){
					geAnimationStart(sampler);
				}
			}
			geTimerUpdate(&sampler->timer);
			float time = (float)sampler->timer.ellapsed / (float)geGetTickResolution();
			for(k=0; k<(sampler->nTimes-1); k++){
			//	if(time > sampler->times[k] && time < sampler->times[k+1]){
			//		geAnimationTransform(sampler->target, &sampler->matrices[k*16]);
			//	}
				if(time >= sampler->times[k] && time < sampler->times[k+1]){
					if(sampler->interpolations[k] == GE_ANIMATION_LINEAR){
						float factor = (time-sampler->times[k]) * 1.0 / (sampler->times[k+1] - sampler->times[k]);
						float mat[16];
						int m;
						for(m=0; m<16; m++){
							mat[m] = (sampler->matrices[k*16+m]*(1.0-factor) + sampler->matrices[(k+1)*16+m]*factor);
						}
						geAnimationTransform(sampler->target, mat);
					}
				}
			}
			if(time >= sampler->times[sampler->nTimes-1]){
				if(sampler->mode == GE_ANIMATION_MODE_LOOP){
					geTimerReset(&sampler->timer);
					geTimerStart(&sampler->timer);
				}
			}
		}
	}
}


*/

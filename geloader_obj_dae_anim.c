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

#define gePrintDebug(...) ;

#define CHECK_EXIT(name) if(strstr(buf, name)){ break; }

#define DAE_ID_LEN 128
#define DAE_NAME_LEN 64
#define DAE_SEMANTIC_LEN 64
#define DAE_SOURCE_LEN 128
#define DAE_TYPE_LEN 64
#define DAE_FILE_LEN 512

typedef struct DAE_array {
	char id[DAE_ID_LEN];
	int count;
	float* data;
} DAE_array;

typedef struct DAE_Name_array {
	char id[DAE_ID_LEN];
	int count;
	char** data;
} DAE_Name_array;

typedef struct DAE_input {
	char semantic[DAE_SEMANTIC_LEN];
	int offset;
	int set;
	char source[DAE_SOURCE_LEN];
} DAE_input;

typedef struct DAE_param {
	char name[DAE_NAME_LEN];
	char type[DAE_TYPE_LEN];
} DAE_param;

typedef struct DAE_accessor {
	char source[DAE_SOURCE_LEN];
	int count;
	int stride;
	int nParams;
	DAE_param* params;
} DAE_accessor;

typedef struct DAE_technique_common {
	DAE_accessor accessor;
} DAE_technique_common;

typedef struct DAE_source {
	char id[DAE_ID_LEN];
	DAE_array array;
	DAE_Name_array Name_array;
	DAE_technique_common technique;
} DAE_source;

typedef struct DAE_sampler {
	char id[DAE_ID_LEN];
	int nInputs;
	DAE_input* inputs;
} DAE_sampler;

typedef struct DAE_channel {
	char source[DAE_ID_LEN];
	char target[DAE_ID_LEN];
} DAE_channel;

typedef struct DAE_animation {
	char id[DAE_ID_LEN];
	char name[DAE_ID_LEN];
	int nSources;
	DAE_source* sources;
	DAE_sampler sampler;
	DAE_channel channel;
} DAE_animation;

int DaeReadArrayFloat(ge_File* fp, const char* start, const char* end, float* data, int nData);
void DaeLoadSource(ge_File* fp, char* buf, DAE_source* source);
void DaeLoadInputs(ge_File* fp, const char* out, DAE_input** _inputs, int* _nInputs);

void DaeLoadAnimation(ge_File* fp, char* buf, DAE_animation* anim){
	geGetParamString(buf, "id", anim->id, DAE_ID_LEN);
	geGetParamString(buf, "name", anim->name, DAE_ID_LEN);
	gePrintDebug(0x100, "  Animation id=\"%s\", name=\"%s\"\n", anim->id, anim->name);
	while(geFileGets(fp, buf, 4096)){
		CHECK_EXIT("</animation>");
		if(strstr(buf, "<source")){
			anim->sources = (DAE_source*)geRealloc(anim->sources, sizeof(DAE_source)*(anim->nSources+1));
			memset(&anim->sources[anim->nSources], 0x0, sizeof(DAE_source));
			DaeLoadSource(fp, buf, &anim->sources[anim->nSources]);
			anim->nSources++;
		}
		if(strstr(buf, "<sampler")){
			geGetParamString(buf, "id", anim->sampler.id, DAE_ID_LEN);
			gePrintDebug(0x100, "  Sampler id=\"%s\"\n", anim->sampler.id);
			DaeLoadInputs(fp, "</sampler>", &anim->sampler.inputs, &anim->sampler.nInputs);
		}
		if(strstr(buf, "<channel")){
			geGetParamString(buf, "source", anim->channel.source, DAE_ID_LEN);
			geGetParamString(buf, "target", anim->channel.target, DAE_ID_LEN);
			gePrintDebug(0x100, "  Channel source=\"%s\", target=\"%s\"\n", anim->channel.source, anim->channel.target);
		}
	}
}

DAE_animation* DaeLoadAnimations(ge_File* fp, int* _nAnims){
	int nAnims = 0;
	DAE_animation* anims = NULL;
	char buf[4096] = "";

	while(geFileGets(fp, buf, 4096)){
		CHECK_EXIT("</library_animations>");
		if(strstr(buf, "<animation") && !strstr(buf, "<animation>")){
			anims = (DAE_animation*)geRealloc(anims, sizeof(DAE_animation)*(nAnims+1));
			memset(&anims[nAnims], 0x0, sizeof(DAE_animation));
			DaeLoadAnimation(fp, buf, &anims[nAnims]);
			nAnims++;
		}
	}

	*_nAnims = nAnims;
	return anims;
}

DAE_source* DaeAnimationFindSource(DAE_animation* anim, const char* id){
//	gePrintDebug(0x100, "DaeAnimationFindSource(\"%s\")\n", id);
	int i = 0;
	for(i=0; i<anim->nSources; i++){
		if(!strcmp(id, anim->sources[i].id)){
			return &anim->sources[i];
		}
	}
	return NULL;
}

ge_AnimNode* geFindAnimNode(ge_AnimNode* root, const char* name){
	gePrintDebug(0x100, "geFindAnimNode(\"%s\", \"%s\")\n", root->name, name);
	if(!strcmp(root->name, name)){
		return root;
	}
	int i = 0;
	gePrintDebug(0x100, "geFindAnimNode \"%s\"->nChilds: %d\n", root->name, root->nChilds);
	for(i=0; i<root->nChilds; i++){
		ge_AnimNode* found = geFindAnimNode(&root->childs[i], name);
		if(found){
			return found;
		}
	}
	return NULL;
}

void ComputeAnimations(DAE_animation* anims, int nAnims, ge_Animator* animator){
	int i = 0;
	int j = 0;
	animator->samplers = (ge_AnimSampler*)geMalloc(sizeof(ge_AnimSampler)*nAnims);
	animator->nSamplers = nAnims;
	for(i=0; i<nAnims; i++){
		gePrintDebug(0x100, "ComputeAnimation %d/%d\n", i+1, nAnims);
		DAE_source* input = NULL;
		DAE_source* output = NULL;
		DAE_source* interpolation = NULL;
		for(j=0; j<anims[i].sampler.nInputs; j++){
			if(!strcmp(anims[i].sampler.inputs[j].semantic, "INPUT")){
				input = DaeAnimationFindSource(&anims[i], anims[i].sampler.inputs[j].source);
			}
			if(!strcmp(anims[i].sampler.inputs[j].semantic, "OUTPUT")){
				output = DaeAnimationFindSource(&anims[i], anims[i].sampler.inputs[j].source);
			}
			if(!strcmp(anims[i].sampler.inputs[j].semantic, "INTERPOLATION")){
				interpolation = DaeAnimationFindSource(&anims[i], anims[i].sampler.inputs[j].source);
			}
		}
		if(!input || !output){
			continue;
		}

		animator->samplers[i].speed = 1.0;

		animator->samplers[i].nTimes = input->array.count;
		animator->samplers[i].times = (float*)geMalloc(sizeof(float)*input->array.count);
		memcpy(animator->samplers[i].times, input->array.data, sizeof(float)*input->array.count);

		animator->samplers[i].nMatrices = output->array.count / 16;
		animator->samplers[i].matrices = (float*)geMalloc(sizeof(float)*output->array.count);
		memcpy(animator->samplers[i].matrices, output->array.data, sizeof(float)*output->array.count);
		for(j=0; j<output->array.count; j+=16){
			float tmp_mat[16];
			memcpy(tmp_mat, &animator->samplers[i].matrices[j], sizeof(float)*16);
			geMatrixTranspose(&animator->samplers[i].matrices[j], tmp_mat, 4);
		}

		if(interpolation){
			animator->samplers[i].nInterpolations = interpolation->Name_array.count;
			animator->samplers[i].interpolations = (int*)geMalloc(sizeof(int)*interpolation->Name_array.count);
			int k;
			for(k=0; k<interpolation->Name_array.count; k++){
				if(!strcmp(interpolation->Name_array.data[k], "LINEAR")){
					animator->samplers[i].interpolations[k] = GE_ANIMATION_LINEAR;
				}
			}
		}else{
			animator->samplers[i].interpolations = NULL;
		}

		if(strstr(anims[i].channel.target, "/matrix")){
			char tmp[64] = "";
			strncpy(tmp, anims[i].channel.target, 64);
			strstr(tmp, "/matrix")[0] = 0x0;
			gePrintDebug(0x100, "  target = \"%s\"\n", tmp);
			animator->samplers[i].target = geFindAnimNode(&animator->root, tmp);
			gePrintDebug(0x100, "  target => 0x%08lX\n", (unsigned long)animator->samplers[i].target);
		}
		gePrintDebug(0x100, "  matrices [%d] :\n", animator->samplers[i].nMatrices);
		/*
		for(j=0; j<animator->samplers[i].nMatrices*16; j+=16){
			float* m = &animator->samplers[i].matrices[j];
			int a = 0;
			for(a=0; a<4; a++){
				gePrintDebug(0x100, "    %f %f %f %f\n", m[a*4+0], m[a*4+1], m[a*4+2], m[a*4+3]);
			}
			gePrintDebug(0x100, "\n");
		}
		*/
	}
}

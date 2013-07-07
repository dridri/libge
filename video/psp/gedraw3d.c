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

extern ge_Camera* ge_current_camera;
void DrawClouds(ge_Scene* scene);

void geLoadMatrix(float* m);
void ge_LoadIdentity(float* m);
void ge_Translate(float* m, float x, float y, float z);
void ge_Rotate(float* m, float x, float y, float z);
void ge_Scale(float* m, float x, float y, float z);

extern struct {
	int used;
	int cap;
	int state;
} _ge_force_caps[32];

void geObjectMatrixIdentity(ge_Object* obj){
	ge_LoadIdentity(obj->matrix);
	obj->matrix_used = false;
}

void geObjectMatrixLoad(ge_Object* obj, float* m){
	memcpy(obj->matrix, m, sizeof(float)*16);
	obj->matrix_used = true;
}

void geObjectMatrixRotate(ge_Object* obj, float x, float y, float z){
	ge_Rotate(obj->matrix, x, y, z);
	obj->matrix_used = true;
}

void geObjectMatrixTranslate(ge_Object* obj, float x, float y, float z){
	ge_Translate(obj->matrix, x, y, z);
	obj->matrix_used = true;
}

void geObjectMatrixScale(ge_Object* obj, float x, float y, float z){
	ge_Scale(obj->matrix, x, y, z);
	obj->matrix_used = true;
}

void geRendererUse(ge_Renderer* render){
	int i;

	if(render->customVert){
		u32 mode = 0;	
		if(render->customVert->vertex_offset >= 0){
			if(render->customVert->vertex_type == GE_FLOAT){
				mode |= GE_VERTEX_32BITF;
			}else
			if(render->customVert->vertex_type == GE_SHORT){
				mode |= GE_VERTEX_16BIT;
			}else
			if(render->customVert->vertex_type == GE_BYTE){
				mode |= GE_VERTEX_8BIT;
			}
		}
		if(render->customVert->color_offset >= 0){
			mode |= GE_COLOR_8888;
		}
		if(render->customVert->texture_offset >= 0){
			if(render->customVert->texture_type == GE_FLOAT){
				mode |= GE_TEXTURE_32BITF;
			}else
			if(render->customVert->texture_type == GE_SHORT){
				mode |= GE_TEXTURE_16BIT;
			}else
			if(render->customVert->texture_type == GE_BYTE){
				mode |= GE_TEXTURE_8BIT;
			}
		}
		if(render->customVert->normal_offset >= 0){
			if(render->customVert->normal_type == GE_FLOAT){
				mode |= GE_NORMAL_32BITF;
			}else
			if(render->customVert->normal_type == GE_SHORT){
				mode |= GE_NORMAL_16BIT;
			}else
			if(render->customVert->normal_type == GE_BYTE){
				mode |= GE_NORMAL_8BIT;
			}
		}
		geDrawMode(mode | GE_TRANSFORM_3D);
		geDrawArrayPointer(render->verts);
	}else{
		geDrawMode(GE_COLOR_8888 | GE_TEXTURE_32BITF | GE_NORMAL_32BITF | GE_VERTEX_32BITF | GE_TRANSFORM_3D);
		geDrawArrayPointer(render->verts);
	}

	if(render->depth_enabled){
		geEnable(GE_DEPTH_TEST);
	}else{
		geDisable(GE_DEPTH_TEST);
	}
	geDepthMask(render->depth_mask);

	if(render->blend_enabled){
		geEnable(GE_BLEND);
		geBlendFunc(GE_SRC_ALPHA, GE_ONE_MINUS_SRC_ALPHA);
	}else{
		geDisable(GE_BLEND);
	}
	
	for(i=0; i<32; i++){
		if(_ge_force_caps[i].used){
			if(_ge_force_caps[i].state == 1){
				geEnable(_ge_force_caps[i].cap);
			}else
			if(_ge_force_caps[i].state == 0){
				geDisable(_ge_force_caps[i].cap);
			}
		}
	}
	
	if(render->ext_func){
		render->ext_func(render, -1);
	}

	if(render->nLights > 0){
		geEnable(GE_LIGHTING);
	}
	for(i=0; i<4; i++){
		if(i < render->nLights){
			geEnable(GE_LIGHT0 + i);
			geSendCommandi(CMD_LIGHT_TYPE_0 + i, (render->lights[i]->type << 8) | 0);
			geSendCommandi(CMD_LIGHT_AMBIENT_0 + 3*i, render->lights[i]->ambient);
			geSendCommandi(CMD_LIGHT_DIFFUSE_0 + 3*i, render->lights[i]->diffuse);
			geSendCommandi(CMD_LIGHT_SPECULAR_0 + 3*i, render->lights[i]->specular);
			geSendCommandf(CMD_LIGHT_X_0 + 3*i, render->lights[i]->position.x);
			geSendCommandf(CMD_LIGHT_Y_0 + 3*i, render->lights[i]->position.y);
			geSendCommandf(CMD_LIGHT_Z_0 + 3*i, render->lights[i]->position.z);
			geSendCommandf(CMD_LIGHT_DIRX_0 + 3*i, render->lights[i]->target.x);
			geSendCommandf(CMD_LIGHT_DIRY_0 + 3*i, render->lights[i]->target.y);
			geSendCommandf(CMD_LIGHT_DIRZ_0 + 3*i, render->lights[i]->target.z);
			geSendCommandf(CMD_LIGHT_CONSTANT_ATTENUATION_0, 0.0);
			
		}else{
			geDisable(GE_LIGHT0 + i);
		}
	}
}

void geRenderObjects(ge_Renderer* render){
	geRendererUse(render);

	geMatrixMode(GE_MATRIX_MODEL);
	geLoadIdentity();

	int i=0;
//	int current_tex_id[8] = { 0 };
	int default_matrix_ok = false;
	int update_matrices = true;

	geUpdateMatrix();

	for(i=0; i<render->nObjs; i++){
		ge_Object* obj = render->objs[i];
		if(obj->vert_start<0)continue;
		/*
		if(obj->matrix_used){
			geLoadMatrix(obj->matrix);
			default_matrix_ok = false;
			update_matrices = true;
		}else if(!default_matrix_ok){
			geLoadIdentity();
			default_matrix_ok = true;
			update_matrices = true;
		}

		if(render->ext_func){
			render->ext_func(render, i);
		}

		if(update_matrices){
			geUpdateMatrix();
			update_matrices = false;
		}
		*/
		bool textured = false;
		if(obj->material.textures[0]){
			textured = true;
			geTextureImage(0, obj->material.textures[0]);
		}else{
			geDisable(GE_TEXTURE_2D);
		}

		geDrawArray(render->draw_mode, obj->vert_start, obj->nVerts);
	}
}

void geObjectDraw(ge_Object* obj){
}

void geRendererUpdate(ge_Renderer* render){
}

void geSceneDraw(ge_Scene* scene){
	int i;
	for(i=0; i<scene->nRenderers; i++){
		if(scene->renderers[i].enabled == false)continue;
		if(scene->renderers[i].matrix_used){
			geMatrixMode(GE_MATRIX_PROJECTION);
			geLoadMatrix(scene->renderers[i].projection_matrix);
		}else{
			geMatrixMode(GE_MATRIX_PROJECTION);
			geLoadMatrix(libge_context->projection_matrix);
		}
		if(scene->renderers[i].ext_func){
			scene->renderers[i].ext_func(&scene->renderers[i], -1);
		}

		geRendererUpdate(&scene->renderers[i]);
		geRenderObjects(&scene->renderers[i]);
		if(scene->renderers[i].callback){
			scene->renderers[i].callback(&scene->renderers[i], -1);
		}
	}
}

void geSceneUpdateMatrices(ge_Scene* scene){
}

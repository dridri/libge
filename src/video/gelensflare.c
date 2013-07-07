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

#include "../ge_internal.h"

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

ge_LensFlarePart _ge_default_lensflare_parts_[] = {
	{ 0, 0.50, RGBA( 80, 100, 150,  64), 0, 0, 64, 64, 2.0 },
	{ 0, 0.55, RGBA( 80, 100, 150,  64), 0, 0, 64, 64, 1.09375 },
	{ 0, 0.45, RGBA( 80, 100, 150,  64), 0, 0, 64, 64, 0.78125 },
	{ 0, 0.75, RGBA(255, 160,  40, 100), 0, 0, 64, 64, 0.78125 },
	{ 1, 0.00, RGBA(117, 250, 196, 192), 64, 0, 64, 64, 0.5 },
	{ 1, 0.50, RGBA( 88, 128, 192,  64), 0, 0, 64, 64, 1.0 },
	{ 1, 0.37, RGBA(255, 160,  40,  64), 0, 0, 64, 64, 2.5 },
	{ 1, 0.407, RGBA(255, 160,  40,  64), 0, 0, 64, 64, 1.40625 },
	{ 1, 0.333, RGBA(255, 160,  40,  64), 0, 0, 64, 64, 0.78125 },
	{ 1, 0.75, RGBA(100, 192,  40,  64), 0, 128, 128, 128, 2.0 },
	{ 2, 0.00, RGBA(255, 255, 255, 255), 128, 128, 128, 128, 4.0 },
	{ -1 }
};

ge_LensFlare* geGfxLensFlareCreate(ge_Shader* shader, ge_Image* texture, ge_LensFlarePart* parts, ge_Image* depth_buffer){
	ge_LensFlare* lens = (ge_LensFlare*)geMalloc(sizeof(ge_LensFlare));
	lens->alpha = 1.0;
	lens->shader = shader;
	lens->texture = texture;
	lens->parts = parts;
	lens->depth_buffer = depth_buffer;
	if(!lens->shader){
		lens->shader = geCreateShader();
		geShaderLoadVertexSource(lens->shader, _ge_BuildPath(libge_context->default_shaders_path, "generic_lensflare.vert"));
		geShaderLoadFragmentSource(lens->shader, _ge_BuildPath(libge_context->default_shaders_path, "generic_lensflare.frag"));
	}
	if(!lens->texture){
		lens->texture = geLoadImage(_ge_BuildPath(libge_context->default_shaders_path, "generic_lensflare.png"));
	}
	if(!lens->parts){
		lens->parts = _ge_default_lensflare_parts_;
	}
	lens->loc_depth_buffer_exists = geShaderUniformID(lens->shader, "depth_buffer_exists");
	lens->loc_depth_buffer = geShaderUniformID(lens->shader, "depth_buffer");
	lens->loc_sun_pos = geShaderUniformID(lens->shader, "sun_pos");
	return lens;
}

void geGfxLensFlareRender(ge_LensFlare* lens, float* light_screen_coord){
	int vec_orig[2];
	int vec[2];
	int end_vec[2];
	int center_vec[2];
	float alpha = 0.0;
	int w, h, i;

	if(light_screen_coord[3] < 0.0){
		return;
	}
	
	w = geGetContext()->width;
	h = geGetContext()->height;
	alpha = 1.0 - fminf(1.0, 0.9 * geDistance2D(0.0, 0.0, light_screen_coord[0]*((float)w)/((float)h), light_screen_coord[1]));
	alpha = alpha * lens->alpha;
	vec_orig[0] = (int)( ((light_screen_coord[0] + 1.0) / 2.0) * (float)w );
	vec_orig[1] = (int)( ((-light_screen_coord[1] + 1.0) / 2.0) * (float)h );

	end_vec[0] = geGetContext()->width - vec_orig[0];
	end_vec[1] = geGetContext()->height - vec_orig[1];
	center_vec[0] = geGetContext()->width/2;
	center_vec[1] = geGetContext()->height/2;

	geShaderUse(lens->shader);
	geTextureImage(1, lens->depth_buffer);
	geShaderUniform1i(lens->loc_depth_buffer, 1);
	geShaderUniform1i(lens->loc_depth_buffer_exists, lens->depth_buffer ? 1 : 0);
	geShaderUniform3f(lens->loc_sun_pos, light_screen_coord[0], light_screen_coord[1], light_screen_coord[2]);
	geTextureImage(0, NULL);
	for(i=0; lens->parts[i].origin>=0; i++){
		int ialpha = max(0, min(255, (int)((float)A(lens->parts[i].color) * alpha)));
		lens->texture->color = (lens->parts[i].color & 0x00FFFFFF) | (ialpha << 24);
		if(lens->parts[i].origin == 0){
			vec[0] = vec_orig[0] + (center_vec[0] - vec_orig[0]) * lens->parts[i].pos_factor;
			vec[1] = vec_orig[1] + (center_vec[1] - vec_orig[1]) * lens->parts[i].pos_factor;
		}else
		if(lens->parts[i].origin == 1){
			vec[0] = center_vec[0] + (end_vec[0] - center_vec[0]) * lens->parts[i].pos_factor;
			vec[1] = center_vec[1] + (end_vec[1] - center_vec[1]) * lens->parts[i].pos_factor;
		}else
		if(lens->parts[i].origin == 2){
			vec[0] = end_vec[0];
			vec[1] = end_vec[1];
		}else{
			continue;
		}
		int pw = (int)(((float)lens->parts[i].ex) * lens->parts[i].size);
		int ph = (int)(((float)lens->parts[i].ey) * lens->parts[i].size);
		int last_depth = geDisable(GE_DEPTH_TEST);
		geBlitImageStretched(vec[0], vec[1], lens->texture, lens->parts[i].sx, lens->parts[i].sy, lens->parts[i].ex, lens->parts[i].ey, pw, ph, GE_BLIT_CENTERED);
		if(last_depth){
			geEnable(GE_DEPTH_TEST);
		}
	}
	geShaderUse(NULL);
}

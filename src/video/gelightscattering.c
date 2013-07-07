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

ge_LightScatter* geLightScatterCreate(int nSamples, ge_Shader* shader, ge_Image* color_buffer, ge_Image* depth_buffer){
	ge_LightScatter* scatter = (ge_LightScatter*)geMalloc(sizeof(ge_LightScatter));
	scatter->color_buffer = color_buffer;
	scatter->depth_buffer = depth_buffer;
	scatter->shader = shader;
	scatter->sun_diameter = 0.5;
	scatter->weight = 5.65;
	if(!scatter->shader){
		scatter->shader = geCreateShader();
		geShaderLoadVertexSource(scatter->shader, _ge_BuildPath(libge_context->default_shaders_path, "generic_scatter.vert"));
		geShaderLoadFragmentSource(scatter->shader, _ge_BuildPath(libge_context->default_shaders_path, "generic_scatter.frag"));
	}
	scatter->loc_ScreenRatio = geShaderUniformID(scatter->shader, "ge_ScreenRatio");
	scatter->loc_texture0 = geShaderUniformID(scatter->shader, "ge_Texture");
	scatter->loc_texture1 = geShaderUniformID(scatter->shader, "ge_Texture1");
	scatter->loc_nLights = geShaderUniformID(scatter->shader, "nLights");
	scatter->loc_lights_pos[0] = geShaderUniformID(scatter->shader, "lights_pos[0]");
	scatter->loc_lights_pos[1] = geShaderUniformID(scatter->shader, "lights_pos[1]");
	scatter->loc_lights_pos[2] = geShaderUniformID(scatter->shader, "lights_pos[2]");
	scatter->loc_lights_pos[3] = geShaderUniformID(scatter->shader, "lights_pos[3]");
	scatter->loc_lights_pos[4] = geShaderUniformID(scatter->shader, "lights_pos[4]");
	scatter->loc_lights_pos[5] = geShaderUniformID(scatter->shader, "lights_pos[5]");
	scatter->loc_lights_pos[6] = geShaderUniformID(scatter->shader, "lights_pos[6]");
	scatter->loc_lights_pos[7] = geShaderUniformID(scatter->shader, "lights_pos[7]");
	scatter->loc_sun_diameter = geShaderUniformID(scatter->shader, "sun_diameter");
	scatter->loc_weight = geShaderUniformID(scatter->shader, "weight");
	return scatter;
}

void geLightScatterRender(ge_LightScatter* scatter, float** lights_screen_coord, int nLights){
	int w, h, i;
	w = geGetContext()->width;
	h = geGetContext()->height;
	float ratio = ((float)w) / ((float)h);
	
	int mode = geDrawingMode(GE_DRAWING_MODE_2D | GE_DRAWING_2D_DEPTH);
	geShaderUse(scatter->shader);

	for(i=0; i<nLights && i<8; i++){
		geShaderUniform4f(scatter->loc_lights_pos[i], lights_screen_coord[i][0]/*ratio*/, lights_screen_coord[i][1], lights_screen_coord[i][2], lights_screen_coord[i][3]);
	}
	for(; i<8; i++){
		geShaderUniform4f(scatter->loc_lights_pos[i], 0.0, 0.0, 0.0, -1.0);
	}
	geShaderUniform1f(scatter->loc_sun_diameter, scatter->sun_diameter);
	geShaderUniform1f(scatter->loc_weight, scatter->weight);
	geShaderUniform1f(scatter->loc_ScreenRatio, ratio);
	geShaderUniform1i(scatter->loc_nLights, nLights);
	geShaderUniform1i(scatter->loc_texture1, 1);
	geShaderUniform1i(scatter->loc_texture0, 0);
	geTextureImage(1, scatter->depth_buffer);
	geTextureImage(0, scatter->color_buffer);
	geBlitImageDepth(0, 0, -2047, scatter->color_buffer, 0, 0, w, h, GE_BLIT_VFLIP | GE_BLIT_NOALPHA);

	geShaderUse(NULL);
	geDrawingMode(mode);
}

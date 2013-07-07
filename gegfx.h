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

#ifndef __GE_H_GFX__
#define __GE_H_GFX__

#include "getypes.h"

typedef struct ge_LensFlarePart {
	int origin;
	float pos_factor;
	u32 color;
	int sx, sy, ex, ey;
	float size;
} ge_LensFlarePart;

typedef struct ge_LensFlare {
	float alpha;
	ge_Image* texture;
	ge_Shader* shader;
	ge_LensFlarePart* parts;
	ge_Image* depth_buffer;
	int loc_depth_buffer_exists;
	int loc_depth_buffer;
	int loc_sun_pos;
} ge_LensFlare;

typedef struct ge_LightScatter {
	float sun_diameter;
	float weight;
	ge_Shader* shader;
	ge_Image* color_buffer;
	ge_Image* depth_buffer;
	int loc_ScreenRatio;
	int loc_texture0;
	int loc_texture1;
	int loc_nLights;
	int loc_lights_pos[8];
	int loc_sun_diameter;
	int loc_weight;
} ge_LightScatter;

LIBGE_API ge_LensFlare* geGfxLensFlareCreate(ge_Shader* shader, ge_Image* texture, ge_LensFlarePart* parts, ge_Image* depth_buffer);
LIBGE_API void geGfxLensFlareRender(ge_LensFlare* lens, float* light_screen_coord);

LIBGE_API ge_LightScatter* geLightScatterCreate(int nSamples, ge_Shader* shader, ge_Image* color_buffer, ge_Image* depth_buffer);
LIBGE_API void geLightScatterRender(ge_LightScatter* scatter, float** lights_screen_coord, int nLights);

LIBGE_API void geWaterInit(ge_Renderer* render, float z);
LIBGE_API void geWaterRender(ge_Renderer* render, ge_Camera* cam, void (*cb)(void*), void* cb_data);

#endif /* __GE_H_GFX__ */

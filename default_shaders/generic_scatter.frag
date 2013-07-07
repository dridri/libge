uniform sampler2D ge_Texture1;
uniform sampler2D ge_Texture2;
uniform vec4 lights_pos[8];
uniform int nLights;
uniform float sun_diameter = 0.5;
uniform float weight = 5.65;
smooth in vec3 frag_coord;

#define NUM_SAMPLES 64
float exposure = 0.0034;
float decay = 1.0;
float density = 0.84;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec4 scatter(vec2 coord, vec2 light_coord, float dist){
	decay = 1.0;
	exposure = 0.34 / float(NUM_SAMPLES);
//	float n_sp = float(NUM_SAMPLES) * 0.9;
	float n_sp = float(NUM_SAMPLES) * 1.0;

	vec4 color = vec4(1.0);
	vec2 deltaTextCoord = vec2(coord - light_coord);
	vec2 textCoo = coord;
	deltaTextCoord *= 1.0 / n_sp * density;
	float illuminationDecay = 1.0;

	for(int i=0; i < NUM_SAMPLES; i++){
		textCoo -= deltaTextCoord;
		vec4 sample = vec4(0.0);
		if(all(greaterThanEqual(textCoo, vec2(0.0))) && all(lessThanEqual(textCoo, vec2(1.0)))){
			sample = texture(ge_Texture1, textCoo);
		}
		if(sample.r >= 1.0){
			sample = vec4(0.25, 0.25, 0.25, 1.0);
		}else{
			sample = vec4(0.0, 0.0, 0.0, 1.0);
		}
		sample *= illuminationDecay * weight;
		color += sample;
		illuminationDecay *= decay;
	}

	color *= (exposure * 0.45);

	if(dist < sun_diameter){
		float d = 1.0 - (dist / sun_diameter);
		float s = d * d * (exp(-d) + 1.0);
		color = color + color * vec4(clamp(0.0, 1.0, s));
	}

	color.a = 1.0;
	return color;
}

void main(){
	if(true){
		vec4 color = vec4(0.0);
		vec2 coord = ge_TexCoord0.xy;
		vec4 light_depth;
		int i;
		float dist;
		vec2 frag_coord_norm = vec2(frag_coord.x*ge_ScreenRatio, frag_coord.y);

		color = texture(ge_Texture, ge_TexCoord0.st);
		for(i=0; i<nLights; i++){
			dist = distance(frag_coord_norm, vec2(lights_pos[i].x*ge_ScreenRatio, lights_pos[i].y));
			if(lights_pos[i].w > 0.0){
				vec2 light = (vec2(lights_pos[i].x, lights_pos[i].y) + vec2(1.0)) / 2.0;
				light_depth = texture(ge_Texture1, light);
				vec4 scatt = scatter(coord, light, dist);
				color = color + vec4(scatt.rgb, 1.0) * scatt.a;
			}
		}
		ge_FragColor = color;
	}else{
		float zNear = 1.0;
		float zFar = 100.0;
		float base_depth = (2.0 * zNear) / (zFar + zNear - texture(ge_Texture1, ge_TexCoord0.st).x * (zFar - zNear));
		ge_FragColor = vec4(base_depth, base_depth, base_depth, 1.0);
	}
}

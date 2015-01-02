smooth in vec3 normal, vVertex, eyeVec;
smooth in vec4 ShadowCoord0;
smooth in vec4 ShadowCoord1;
smooth in vec4 ShadowCoord2;
smooth in vec4 ShadowCoord3;
smooth in vec4 ShadowCoord4;
smooth in vec4 frag_coord;
smooth in vec4 v_coord;

const vec2 noise_bias[64] = {
	vec2(0.385430, 0.013303),
	vec2(0.618253, 0.014185),
	vec2(0.116847, 0.821238),
	vec2(0.999727, 0.516517),
	vec2(0.840026, 0.866582),
	vec2(0.758047, 0.071734),
	vec2(0.920435, 0.229381),
	vec2(0.000036, 0.505969),
	vec2(0.725438, 0.946293),
	vec2(0.987327, 0.388141),
	vec2(0.047543, 0.712797),
	vec2(0.952422, 0.712872),
	vec2(0.880935, 0.823865),
	vec2(0.818902, 0.114901),
	vec2(0.112609, 0.816114),
	vec2(0.111796, 0.184885),
	vec2(0.128821, 0.835002),
	vec2(0.823695, 0.881080),
	vec2(0.980630, 0.637820),
	vec2(0.289277, 0.953427),
	vec2(0.892913, 0.190777),
	vec2(0.556193, 0.003168),
	vec2(0.438629, 0.996219),
	vec2(0.714085, 0.048151),
	vec2(0.446173, 0.997094),
	vec2(0.343281, 0.974804),
	vec2(0.936057, 0.744651),
	vec2(0.633152, 0.018055),
	vec2(0.938507, 0.740233),
	vec2(0.994694, 0.427351),
	vec2(0.499895, 1.000000),
	vec2(0.205956, 0.095602),
	vec2(0.611232, 0.987470),
	vec2(0.032717, 0.322106),
	vec2(0.748315, 0.933981),
	vec2(0.429560, 0.995013),
	vec2(0.858869, 0.848156),
	vec2(0.099312, 0.200919),
	vec2(0.536461, 0.001331),
	vec2(0.747297, 0.934562),
	vec2(0.193722, 0.895214),
	vec2(0.165975, 0.127942),
	vec2(0.842578, 0.135802),
	vec2(0.996555, 0.441404),
	vec2(0.528151, 0.000793),
	vec2(0.835822, 0.870437),
	vec2(0.072509, 0.759329),
	vec2(0.264726, 0.058813),
	vec2(0.929384, 0.756183),
	vec2(0.086879, 0.781657),
	vec2(0.470799, 0.000853),
	vec2(0.577827, 0.006094),
	vec2(0.249375, 0.932651),
	vec2(0.111268, 0.185536),
	vec2(0.000064, 0.491988),
	vec2(0.000131, 0.511435),
	vec2(0.309121, 0.037869),
	vec2(0.826544, 0.878641),
	vec2(0.118184, 0.177174),
	vec2(0.967320, 0.677799),
	vec2(0.998755, 0.535263),
	vec2(0.095615, 0.205938),
	vec2(0.317525, 0.034487),
	vec2(0.987577, 0.389235)
};

uniform sampler3D ge_LightShadowJitter;


float LFSR_Rand_Gen_1f(float n){
  return (mod(n * mod(n*n*15731.0+789221.0, 2147483647.0) + 1376312589.0, 2147483647.0));
}

int LFSR_Rand_Gen_i(vec2 v){
	float n = dot(v, vec2(43.0, 148.0));
	return int(n);
/*
	int n = int(dot(v, vec2(43.0, 148.0)));
	n = (n << 13) ^ n;
	return (n * (n*n*15731+789221) + 1376312589) & 0x7fffffff;
*/
}

vec4 calculateShadow(vec2 shadowCoord, vec4 shadowCoordinateWdivide, float lambertTerm, float lod, float bias){
	vec4 shadow = vec4(1.0);
/*
	distanceFromLight = texture2DArray(ge_LightShadows[0], vec3(shadowCoord, lod)).r;
	if(step(shadowCoordinateWdivide.z - bias, distanceFromLight) == 1.0){
		return shadow;
	}
*/
/*
	distanceFromLight = texture2DArray(ge_LightShadows[0], vec3(shadowCoord, lod)).r;
	if(distanceFromLight < shadowCoordinateWdivide.z - 0.00005){
		shadow = vec4(0.0);
	}
*/
//	bias = 0.0;
//	vec3 jcoord = vec3((frag_coord.xy * vec2(1.0, -1.0)) * 2.0 - vec2(1.0), 0);
	vec3 jcoord = vec3(fract(v_coord.xyz) * 0.15);
	float res = 0.0;
	float fsize = shadowCoordinateWdivide.w * 20.0;
//	fsize = 5.0;

	bias = 0.0;

	for(int i=0; i<4; i++){
		vec4 offset = texture3D(ge_LightShadowJitter, jcoord);
		offset = offset * 2.0 - 1.0;
		offset *= 0.0005;
		jcoord.z += 1.0 / 32.0;

		vec3 smCoord = vec3(0.0, 0.0, lod);

		smCoord.xy = offset.xy * fsize + shadowCoord;
		if(texture2DArray(ge_LightShadows[0], smCoord).r >= shadowCoordinateWdivide.z - bias){
			res += 1.0 / 8.0;
		}

		smCoord.xy = offset.zw * fsize + shadowCoord;
		if(texture2DArray(ge_LightShadows[0], smCoord).r >= shadowCoordinateWdivide.z - bias){
			res += 1.0 / 8.0;
		}
	}
	jcoord = vec3(fract(v_coord.xyz) * 4.0);

	if ((res - 1) * res * lambertTerm != 0){
		res = res / 8.0;

		for(int i=0; i<32; i++){
			vec4 offset = texture3D(ge_LightShadowJitter, jcoord);
			offset = offset * 2.0 - 1.0;
			offset *= 0.0005;
			jcoord.z += 1.0 / 32.0;
			vec3 smCoord = vec3(0.0, 0.0, lod);

			smCoord.xy = offset.xy * fsize + shadowCoord;
			res += step(shadowCoordinateWdivide.z - bias, texture2DArray(ge_LightShadows[0], smCoord).r) / 64.0;

			smCoord.xy = offset.zw * fsize + shadowCoord;
			res += step(shadowCoordinateWdivide.z - bias, texture2DArray(ge_LightShadows[0], smCoord).r) / 64.0;
		}
	}
	shadow = vec4(res);

/*
	range = 1.0;
	for(y=-range*yincr; y<=range*yincr; y+=yincr){
		for(x=-range*xincr; x<=range*xincr; x+=xincr){
			distanceFromLight = texture2DArray(ge_LightShadows[0], vec3(shadowCoord + vec2(x, y), lod)).r;
			if(distanceFromLight < shadowCoordinateWdivide.z - bias){
			}else{
				res += 1.0;
			}
			tot += 1.0;
		}
	}
	shadow = vec4(res / tot);
*/

//	range = range * 0.5;
/*
	vec2 randv = vec2(0.0);
//	randv = noise_bias[LFSR_Rand_Gen_i(shadowCoord) % 64];
	randv = noise_bias[int(dot(frag_coord.xy * 50.0 + shadowCoord, vec2(43.0, 148.0))) % 64];
	randv = randv * 2.0 - vec2(1.0);
	randv = randv * vec2(xincr, yincr);
	randv = randv * range * 0.5;

	for(y=-range*yincr; y<=range*yincr; y+=yincr){
		for(x=-range*xincr; x<=range*xincr; x+=xincr){
			distanceFromLight = texture2DArray(ge_LightShadows[0], vec3(shadowCoord + randv + vec2(x, y), lod)).r;
			res += step(shadowCoordinateWdivide.z - bias, distanceFromLight);
			tot += 1.0;
		}
	}
	shadow = vec4(res / (tot - 1.0));
//	shadow = exp(shadow * 1.7 - 1.0);
*/
/*
	distanceFromLight = texture2DArray(ge_LightShadows[0], vec3(shadowCoord + randv, lod)).r;
	shadow = step(shadowCoordinateWdivide.z - bias, distanceFromLight);
*/

//	float noise_idx = ((gl_FragCoord.x*0.5+0.5) + 8.0 * (gl_FragCoord.y*0.5+0.5)) * 4.0;
//	vec2 p = range * 2.0 * vec2(xincr, yincr) * (noise_bias[int(noise_idx) % 64] * 0.25 - 0.125);
/*
	vec2 p = vec2(0.0);
	range *= 0.5;
	for(y=-range*yincr; y<=range*yincr; y+=yincr){
		for(x=-range*xincr; x<=range*xincr; x+=xincr){
			distanceFromLight = texture2DArray(ge_StaticLights[0].shadow, vec3(shadowCoord + p + vec2(x, y), lod)).r;
			if(distanceFromLight < shadowCoordinateWdivide.z - bias){
			}else{
				res += 1.0;
			}
		//	tot += length(vec2(x, y) + p);
			tot += 1.0;
		}
	}
	shadow = vec4(res / tot);
*/
/*
	if(lod == 0.0){
		shadow = vec4(1.0, 0.3, 0.3, 1.0) * shadow;
	}
	if(lod == 1.0){
		shadow = vec4(0.3, 1.0, 0.3, 1.0) * shadow;
	}
	if(lod == 2.0){
		shadow = vec4(0.3, 0.3, 1.0, 1.0) * shadow;
	}
	if(lod == 3.0){
		shadow = vec4(0.3, 1.0, 1.0, 1.0) * shadow;
	}
	if(lod == 4.0){
		shadow = vec4(1.0, 0.3, 1.0, 1.0) * shadow;
	}
*/
	shadow.a = 1.0;
	return shadow;
}

vec3 N;
vec4 calculateLight(ge_struct_Light light){
	vec4 final_color = light.ambient;
	if(light.attenuation == -1.0){
		return vec4(0.0);
	}else{
		vec3 L = normalize(light.vector - vVertex);
		vec3 L2 = normalize(light.target.xyz - light.position.xyz);
		float lambertTerm = max(dot(N, L), 0.0);

		vec4 shadow = vec4(1.0);

		if((light.flags & GE_LIGHT_HAVE_SHADOW) != 0){
			//shadow += shadow2DProj(light.shadow, ShadowCoord).r;

			vec4 shadowCoordinateWdivide0 = vec4(ShadowCoord0.xyz / ShadowCoord0.w, ShadowCoord0.w);
			vec2 shadowCoord0 = shadowCoordinateWdivide0.st;
			vec4 shadowCoordinateWdivide1 = vec4(ShadowCoord1.xyz / ShadowCoord1.w, ShadowCoord1.w);
			vec2 shadowCoord1 = shadowCoordinateWdivide1.st;
			vec4 shadowCoordinateWdivide2 = vec4(ShadowCoord2.xyz / ShadowCoord2.w, ShadowCoord2.w);
			vec2 shadowCoord2 = shadowCoordinateWdivide2.st;
			vec4 shadowCoordinateWdivide3 = vec4(ShadowCoord3.xyz / ShadowCoord3.w, ShadowCoord3.w);
			vec2 shadowCoord3 = shadowCoordinateWdivide3.st;
			vec4 shadowCoordinateWdivide4 = vec4(ShadowCoord4.xyz / ShadowCoord4.w, ShadowCoord4.w);
			vec2 shadowCoord4 = shadowCoordinateWdivide4.st;

			if(ShadowCoord0.w > 0.0 && all(equal(shadowCoord0, clamp(shadowCoord0, vec2(0.0), vec2(1.0))))){
				shadow = calculateShadow(shadowCoord0, shadowCoordinateWdivide0, lambertTerm, 0.0, 0.0000005);
			}else
			if(ShadowCoord1.w > 0.0 && all(equal(shadowCoord1, clamp(shadowCoord1, vec2(0.0), vec2(1.0))))){
				shadow = calculateShadow(shadowCoord1, shadowCoordinateWdivide1, lambertTerm, 1.0, 0.000005);
			}else
			if(ShadowCoord2.w > 0.0 && all(equal(shadowCoord2, clamp(shadowCoord2, vec2(0.0), vec2(1.0))))){
				shadow = calculateShadow(shadowCoord2, shadowCoordinateWdivide2, lambertTerm, 2.0, 0.00005);
			}else
			if(ShadowCoord3.w > 0.0 && all(equal(shadowCoord3, clamp(shadowCoord3, vec2(0.0), vec2(1.0))))){
				shadow = calculateShadow(shadowCoord3, shadowCoordinateWdivide3, lambertTerm, 3.0, 0.0001);
			}else
			if(ShadowCoord4.w > 0.0 && all(equal(shadowCoord4, clamp(shadowCoord4, vec2(0.0), vec2(1.0))))){
				shadow = calculateShadow(shadowCoord4, shadowCoordinateWdivide4, lambertTerm, 4.0, 0.0001);
			}

		}

		if(light.attenuation == -2.0){
			final_color += light.diffuse * lambertTerm * shadow;
			if(length(ge_FrontMaterial.specular.xyz) > 0.0){
				vec3 E = normalize(eyeVec);
				vec3 R = reflect(-L, N);
				float specular = pow(max(dot(R, E), 0.0), 2.0);
				final_color += ge_FrontMaterial.specular * light.specular * lambertTerm * specular * shadow;
			}
		}else{
			if(lambertTerm > 0.0){
				float cos_cur_angle = dot(-L, light.targetVector);
				float spot = clamp((cos_cur_angle - light.CosOuterConeAngle) / light.CosInnerMinusOuterAngle, 0.0, 1.0);
				final_color += light.diffuse * lambertTerm * spot * shadow;
				if(length(ge_FrontMaterial.specular.xyz) > 0.0){
					vec3 E = normalize(eyeVec);
					vec3 R = reflect(-L, N);
					float specular = pow(max(dot(R, E), 0.0), 2.0);
					final_color += ge_FrontMaterial.specular * light.specular * lambertTerm * spot * specular * shadow;
				}
			}
		}
	}

	return final_color;
}

void main(){
	N = normalize(normal);

	vec4 diffuse = ge_Color;
	if(ge_HasTexture != 0){
		diffuse *= texture(ge_Texture, ge_TexCoord0.st);
	}
	/*
	if(diffuse.a < 0.2){
		discard;
	}
	*/
	vec4 final_color = vec4(0.0);//ge_FrontMaterial.ambient;
	final_color += calculateLight(ge_StaticLights[0]);
	/*
	final_color += calculateLight(ge_StaticLights[1]);
	final_color += calculateLight(ge_StaticLights[2]);
	final_color += calculateLight(ge_StaticLights[3]);
	final_color += calculateLight(ge_StaticLights[4]);
	*/
	final_color.a = 1.0;

	ge_FragColor = final_color * diffuse;
}

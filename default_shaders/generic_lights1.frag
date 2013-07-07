smooth in vec3 normal, vVertex, eyeVec;

vec3 N;
vec4 calculateLight(ge_struct_Light light){
	vec4 final_color = light.ambient;
	if(light.attenuation == -1.0){
		return vec4(0.0);
	}else{
		vec3 L = normalize(light.vector - vVertex);
		float lambertTerm = max(dot(N,L), 0.0);
		if(light.attenuation == -2.0){
			if(lambertTerm > 0.0){
				final_color += light.diffuse * lambertTerm;
			}
		}else{
			if(lambertTerm > 0.0){
				float cos_cur_angle = dot(-L, light.targetVector);
				float spot = clamp((cos_cur_angle - light.CosOuterConeAngle) / light.CosInnerMinusOuterAngle, 0.0, 1.0);
				final_color += light.diffuse * lambertTerm * spot;
			}
		}
		if(ge_FrontMaterial.specular != 0.0){
			vec3 E = normalize(eyeVec);
			vec3 R = reflect(-L, N);
			float specular = pow(max(dot(R, E), 0.0), 2.0);
			final_color += vec4(specular * ge_FrontMaterial.specular);
		}
	}

	return final_color;
}

void main(){
	N = normalize(normal);
	vec4 final_color = vec4(0.0);//ge_FrontMaterial.ambient;
	final_color += calculateLight(ge_StaticLights[0]);

	ge_FragColor = ge_Color * final_color;

/*
	float z = ge_FragCoord.z / ge_FragCoord.w;
	float fogFactor = clamp(exp2( -0.000005 * 0.000005 * z * z * 1.442695), 0.0, 1.0);
	vec3 c = normalize(10000.0 * vec3(5.266626e-006, 1.230601e-005, 3.004397e-005));
//	ge_FragColor = vec4(c, 1.0) * clamp(0.5 * 1.0 / fogFactor - 0.2, 0.0, 1.0) + ge_FragColor;
	float f = clamp(0.0, 0.2, 1.0-fogFactor);
	ge_FragColor = (vec4(c, 1.0) * f + ge_Color) * final_color;
*/
}
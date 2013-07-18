smooth in vec3 normal, vVertex, eyeVec;


vec3 N;
vec4 calculateLight(ge_struct_Light light){
	vec4 final_color = vec4(0.0);
	if(light.attenuation == -1.0){
		return vec4(0.0);
	}else{
		vec3 L = normalize(light.vector - vVertex);
		float lambertTerm = max(dot(N,L), 0.0);
		if(light.attenuation == -2.0){
			final_color = light.ambient + light.diffuse * lambertTerm;
		}else{
			if(lambertTerm > 0.0){
				float cos_cur_angle = dot(-L, light.targetVector);
				float spot = clamp((cos_cur_angle - light.CosOuterConeAngle) / light.CosInnerMinusOuterAngle, 0.0, 1.0);
				final_color = light.ambient + light.diffuse * lambertTerm * spot;
			}
		}
		vec3 E = normalize(eyeVec);
		vec3 R = reflect(-L, N);
		float specular = pow(max(dot(R, E), 0.0), 2.0);
		final_color += vec4(specular);
	}

	return final_color;
}

void main(){
	N = normalize(normal);
	vec4 final_color = vec4(0.0);//ge_FrontMaterial.ambient;
	final_color += calculateLight(ge_StaticLights[0]);


	ge_FragColor = ge_Color * final_color;
}
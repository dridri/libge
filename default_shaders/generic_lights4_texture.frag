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
		if(length(ge_FrontMaterial.specular.xyz) > 0.0){
			vec3 E = normalize(eyeVec);
			vec3 R = reflect(-L, N);
			float specular = pow(max(dot(R, E), 0.0), 2.0);
			final_color += specular * light.specular * ge_FrontMaterial.specular;
		}
	}

	return final_color;
}

void main(){
	N = normalize(normal);
	vec4 final_color = vec4(0.0);//ge_FrontMaterial.ambient;
	final_color += calculateLight(ge_StaticLights[0]);
	final_color += calculateLight(ge_StaticLights[1]);
	final_color += calculateLight(ge_StaticLights[2]);
	final_color += calculateLight(ge_StaticLights[3]);

	ge_FragColor = ge_Color * final_color * texture(ge_Texture, ge_TexCoord0);
}
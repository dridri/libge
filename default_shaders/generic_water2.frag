uniform vec3 normalMapPos;
uniform sampler2D ge_Texture1;
uniform sampler2D ge_Texture2;
uniform sampler2D ge_Texture3;

smooth in vec4 view_coords;
smooth in vec2 refraction_coords;
smooth in vec2 normal_coords;
smooth in vec4 light_tangent, light_tangent2;
smooth in vec4 eye_tangent;
smooth in float fogFactor;

void main(){
	const float distortion_para = 0.02; // 0.02
	const float refraction_para = 0.015; // 0.015
	const float shine_para = 100.0;
	const vec4 water_color = vec4(0.2, 0.4, 0.6, 1.0);

	vec2 offset_coords = refraction_coords + vec2(texture(ge_Texture2, normal_coords)) * distortion_para;
	vec4 dudv = texture(ge_Texture2, offset_coords);
	dudv = normalize (dudv * 2.0 - 1.0) * refraction_para;
	dudv.x = dudv.x * ((cos(normalMapPos[2])+2.0) / 2.0);
	dudv.y = dudv.y * ((sin(normalMapPos[2])+2.0) / 2.0);

	vec4 norm = texture(ge_Texture1, offset_coords) * 2.0 - 1.0;
	norm.a = 0.0;
	norm *= 0.4;

	vec4 reflection_dir2 = normalize(reflect(light_tangent2, norm)) * sin(ge_Time * 3.0) * cos(ge_Time * 2.0);
	float inv_fresnel = dot (norm, -reflection_dir2);
	float fresnel = 1.0 - inv_fresnel;

	vec4 proj_coords = view_coords / view_coords.w;
	proj_coords = (proj_coords + 1.0) / 2.0;
	proj_coords += dudv;
	proj_coords = clamp(proj_coords, 0.001, 0.999);

	vec4 reflection_color = texture(ge_Texture, proj_coords.xy);
	reflection_color *= fresnel;
	vec4 refraction_color = texture(ge_Texture3, proj_coords.xy);
	refraction_color *= fresnel;
//	refraction_color += water_color * abs(inv_fresnel);


	vec4 spec_reflection_dir = normalize(reflect(light_tangent, norm));
	vec4 eye = normalize(eye_tangent);
	float spec = pow(max(dot(spec_reflection_dir, eye), 0.0), shine_para);
	vec4 specular_color = vec4(spec);

	ge_FragColor = water_color*fresnel + water_color*reflection_color*0.8 + water_color*refraction_color*0.4 + specular_color*0.7;

//	ge_FragColor = mix(ge_Fog.color, ge_FragColor, fogFactor);

	ge_FragColor.a = 1.0;
}
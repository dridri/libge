struct ge_struct_Light {
	vec4 position;
	vec4 target;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float spotCutoff;
	float spotCosCutoff;
	float spotExponent;
	float attenuation;
	vec3 vector;
	vec3 targetVector;
};
struct ge_struct_material {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};

// Lights
uniform int ge_DynamicLightsCount;
uniform int ge_StaticLightsCount;
uniform ge_struct_Light ge_StaticLights[];
uniform ge_struct_Light ge_DynamicLights[];

// Materials
uniform ge_struct_material ge_FrontMaterial;



#ifdef LOW_PROFILE

#define ge_Color gl_Color
#define ge_FragColor gl_FragColor

#endif
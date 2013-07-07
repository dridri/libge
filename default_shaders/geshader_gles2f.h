precision highp float;

#define smooth
#define in varying
#define out varying

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
	float CosOuterConeAngle;
	float CosInnerMinusOuterAngle;
};
struct ge_struct_material {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};

// Lights
uniform int ge_DynamicLightsCount;
uniform int ge_StaticLightsCount;
uniform ge_struct_Light ge_StaticLights[8];
uniform ge_struct_Light ge_DynamicLights[8];

// Materials
uniform ge_struct_material ge_FrontMaterial;
uniform sampler2D ge_Texture;

uniform highp float ge_Time;


varying vec3 ge_TexCoord0;
varying vec4 ge_Color;

#define ge_FragColor gl_FragColor
#define ge_FragData gl_FragData
#define ge_FragCoord gl_FragCoord

#define texture texture2D

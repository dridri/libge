precision highp float;

#define smooth
#define in varying
#define out varying

// Vertex Attributes
attribute vec3 ge_VertexTexcoord;
attribute vec4 ge_VertexColor;
attribute vec3 ge_VertexNormal;
attribute vec4 ge_VertexPosition;

// Matrices
uniform mat4 ge_MatrixProjection;
uniform mat4 ge_MatrixView;
uniform mat4 ge_MatrixModel;
uniform mat4 ge_MatrixModelView;
uniform mat4 ge_MatrixTexture[8];
uniform mat3 ge_MatrixNormal;
uniform vec4 ge_ClipPlane;

// Outs
varying vec3 ge_TexCoord0;
varying vec4 ge_Color;

#define ge_Position gl_Position
uniform vec3 ge_CameraPosition;

uniform float ge_Time;

struct ge_struct_Light {
	int flags;
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
	sampler2D shadow;
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

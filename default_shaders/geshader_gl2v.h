#define smooth
#define in varying
#define out varying

#define ge_VertexTexcoord gl_MultiTexCoord0
#define ge_VertexColor gl_Color
#define ge_VertexNormal gl_Normal
#define ge_VertexPosition gl_Vertex

// Matrices
#define ge_MatrixProjection gl_ProjectionMatrix
#define ge_MatrixModelView gl_ModelViewMatrix
#define ge_MatrixNormal gl_NormalMatrix
#define ge_MatrixTexture gl_TextureMatrix
#define ge_ClipPlane gl_ClipPlane[0]

//#define ge_Color gl_FrontColor
#define ge_TexCoord0 gl_TexCoord[0]

uniform mat4 ge_MatrixView;
uniform mat4 ge_MatrixModel;
uniform mat4 ge_MatrixSubModel;
smooth out vec4 ge_Color;

#define ge_Position gl_Position
uniform vec3 ge_CameraPosition;
uniform float ge_Time;
uniform int ge_HasTexture;

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
	sampler2DArray shadow;
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

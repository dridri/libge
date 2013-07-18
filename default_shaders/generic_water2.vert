uniform vec3 normalMapPos;

smooth out vec4 view_coords;
smooth out vec2 refraction_coords;
smooth out vec2 normal_coords;
smooth out vec4 light_tangent, light_tangent2;
smooth out vec4 eye_tangent;
smooth out float fogFactor;

void main(){
	vec4 water_normal = vec4(0.0, 0.0, 1.0, 0.0);
	vec4 water_tangent = vec4(1.0, 0.0, 0.0, 0.0);
	vec4 water_bitangent = vec4(0.0, 1.0, 0.0, 0.0);
	vec4 v = ge_VertexPosition;
	v.z = 2.0 * sin(ge_Time + v.x) * cos(2*ge_Time + v.y); /////////////
	vec4 eye_dir = ge_MatrixModelView * v;	

	vec4 light_dir = (ge_MatrixModelView*ge_StaticLights[0].position) - v;
	light_tangent.x = dot(light_dir, water_tangent);
	light_tangent.y = dot(light_dir, water_bitangent);
	light_tangent.z = dot(light_dir, water_normal);
	light_tangent.w = 1.0;

//	light_dir = vec4(cos(normalMapPos.z)*15000.0, sin(normalMapPos.z)*15000.0, 10000.0, 1.0) + v;
//	light_dir = (ge_MatrixView*light_dir) - v;
	light_tangent2.x = dot(light_dir, water_tangent);
	light_tangent2.y = dot(light_dir, water_bitangent);
	light_tangent2.z = dot(light_dir, water_normal);
	light_tangent2.w = 1.0;

	eye_tangent.x = dot(eye_dir, water_tangent);
	eye_tangent.y = dot(eye_dir, water_bitangent);
	eye_tangent.z = dot(eye_dir, water_normal);
	eye_tangent.w = 1.0;

	refraction_coords = ge_VertexTexcoord.xy * 2000.0 + normalMapPos.xy;
	normal_coords = ge_VertexTexcoord.xy * 2000.0 + normalMapPos.xy;

	view_coords = ge_MatrixProjection * ge_MatrixModelView * v;
	ge_Position = view_coords;
/*
	const float LOG2 = 1.442695;
	float FogFragCoord = length(ge_MatrixModelView * vec4(v.xy, ge_CameraPosition.z, v.w));
	fogFactor = exp2(-ge_Fog.density * ge_Fog.density * FogFragCoord * FogFragCoord * LOG2);
	fogFactor = clamp(fogFactor, 0.0, 1.0);
*/
}
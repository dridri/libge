smooth out vec3 normal, lightDir, halfVector;

uniform vec3 ofs;

void main(){
	normal = normalize(ge_MatrixNormal * ge_VertexNormal);
	lightDir = normalize(vec3(1.0,1.0,1.0));
	halfVector = normalize(vec3(1.0,1.0,1.0) + ge_CameraPosition);

	ge_Color = ge_VertexColor;
	vec4 p = vec4((ge_VertexPosition.xyz + ofs), ge_VertexPosition.w);
	ge_Position = ge_MatrixProjection * ge_MatrixModelView * p;
}

smooth out vec3 normal, vVertex, eyeVec;

uniform vec3 ofs;

void main(){
	normal = ge_MatrixNormal * ge_VertexNormal;
	vVertex = vec3(ge_MatrixModelView * ge_VertexPosition);
	eyeVec = -vVertex;

	ge_Color = ge_VertexColor;
//	gl_Position = ge_MatrixProjection * ge_MatrixModelView * ge_VertexPosition;

	vec4 p = vec4(ge_VertexPosition.xyz + ofs, ge_VertexPosition.w);
	gl_Position = ge_MatrixProjection * ge_MatrixModelView * p;
}

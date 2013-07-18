smooth out vec3 normal, vVertex, eyeVec;

void main(){
	normal = ge_MatrixNormal * ge_VertexNormal;
	vVertex = vec3(ge_MatrixModelView * ge_VertexPosition);
	eyeVec = -vVertex;

	ge_Color = ge_VertexColor;
	gl_Position = ge_MatrixProjection * ge_MatrixModelView * ge_VertexPosition;
}
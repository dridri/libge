smooth out vec3 normal, lightDir, halfVector;

void main(){
	normal = normalize(ge_MatrixNormal * ge_VertexNormal);
	lightDir = normalize(vec3(0,1,1));
	halfVector = normalize(vec3(0,1,1) + ge_CameraPosition);

	ge_Color = ge_VertexColor;
	ge_Position = ge_MatrixProjection * ge_MatrixModelView * ge_VertexPosition;
}
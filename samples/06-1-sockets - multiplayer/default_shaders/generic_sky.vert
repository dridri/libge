smooth out vec3 pos;

void main(){
	ge_Color = ge_VertexColor;
	ge_TexCoord0 = ge_VertexTexcoord;
	ge_Position = ge_MatrixProjection * ge_MatrixModelView * ge_VertexPosition;
	pos = ge_VertexPosition.xyz;
}

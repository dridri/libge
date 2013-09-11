smooth out vec3 frag_coord;

void main(){
	ge_Color = ge_VertexColor;
	ge_TexCoord0 = ge_VertexTexcoord;
	ge_Position = ge_MatrixProjection * ge_VertexPosition;
	frag_coord = ge_Position.xyz;
}

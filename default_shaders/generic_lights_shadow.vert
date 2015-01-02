smooth out vec3 normal, vVertex, eyeVec;
smooth out vec4 ShadowCoord0;
smooth out vec4 ShadowCoord1;
smooth out vec4 ShadowCoord2;
smooth out vec4 ShadowCoord3;
smooth out vec4 ShadowCoord4;
smooth out vec4 frag_coord;
smooth out vec4 v_coord;

void main(){
	ShadowCoord0 = ge_MatrixTexture[7] * ge_VertexPosition;
	ShadowCoord1 = ge_MatrixTexture[6] * ge_VertexPosition;
	ShadowCoord2 = ge_MatrixTexture[5] * ge_VertexPosition;
	ShadowCoord3 = ge_MatrixTexture[4] * ge_VertexPosition;
	ShadowCoord4 = ge_MatrixTexture[3] * ge_VertexPosition;
	ge_TexCoord0 = ge_VertexTexcoord;

	normal = ge_MatrixNormal * ge_VertexNormal;
	vVertex = vec3(ge_MatrixModelView * ge_VertexPosition);
	eyeVec = -vVertex;

	v_coord = ge_VertexPosition;
	ge_Color = ge_VertexColor;
	ge_Position = ge_MatrixProjection * ge_MatrixModelView * ge_VertexPosition;
	frag_coord = ge_Position;
	frag_coord.xyz /= frag_coord.w;
}

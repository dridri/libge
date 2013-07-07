uniform vec3 ofs;

void main(){
	vec4 p = vec4((ge_VertexPosition.xyz + ofs), ge_VertexPosition.w);
	ge_Position = ge_MatrixProjection * ge_MatrixModelView * p;
	ge_Position.z = max(0.0, ge_Position.z-0.1);
//	ge_Position = ge_MatrixProjection * ge_MatrixModelView * ge_VertexPosition;
}

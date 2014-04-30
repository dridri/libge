void main(){
	ge_Color = ge_VertexColor;
	gl_Position = ge_MatrixProjection * ge_MatrixModelView * ge_VertexPosition;
}

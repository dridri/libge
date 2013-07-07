void main(){
	ge_Color = ge_VertexColor;
	ge_Position = ge_MatrixProjection * ge_MatrixModelView * ge_VertexPosition;
}

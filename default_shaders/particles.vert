uniform vec2 size;
uniform vec3 pos;
smooth out vec3 coord;

void main(){
	ge_Color = ge_VertexColor;
	mat4 mv = ge_MatrixModelView;
	mv[0][0] = 1.0;
	mv[0][1] = 0.0;
	mv[0][2] = 0.0;
	mv[1][0] = 0.0;
	mv[1][1] = 1.0;
	mv[1][2] = 0.0;
	mv[2][0] = 0.0;
	mv[2][1] = 0.0;
	mv[2][2] = 1.0;
/*
	mat4 proj_mv = ge_MatrixProjection * mv;

	mat4 mvp = ge_MatrixProjection * ge_MatrixModelView;

	ge_Position = proj_mv * ge_VertexPosition + mvp * vec4(pos, 1.0);

	coord = (proj_mv * ge_VertexPosition).xyz;
*/
/*
	ge_Position = ge_MatrixProjection * (mv*ge_VertexPosition + ge_MatrixModelView*vec4(pos, 1.0));
	coord = (ge_MatrixProjection * mv * ge_VertexPosition).xyz;
*/
	vec4 vertex = ge_MatrixProjection * mv * (ge_VertexPosition * vec4(size, 1.0, 1.0));
	ge_Position = vertex + ge_MatrixProjection * ge_MatrixModelView * (vec4(pos, 1.0));
	coord = ge_VertexPosition.xyz;
}

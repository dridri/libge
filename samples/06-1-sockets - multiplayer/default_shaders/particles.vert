uniform vec2 size;
uniform vec3 pos;
/*
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
	vec4 vpos = vec4(ge_VertexPosition.xy * size, ge_VertexPosition.z, ge_VertexPosition.w) + vec4(pos, 0.0);
	ge_Position = ge_MatrixProjection * mv * vpos;
}
*/
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
	mat4 proj_mv = ge_MatrixProjection * mv;

	vec4 vpos = ge_VertexPosition;
	vpos.x *= size.x;
	vpos.y *= size.y;
	vpos += vec4(pos, 0.0);
	
	coord = (proj_mv * ge_VertexPosition).xyz;
	ge_Position = proj_mv * vpos;
}

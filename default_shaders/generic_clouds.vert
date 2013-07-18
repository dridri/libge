uniform vec4 pos;
uniform vec2 tex_decal;

uniform vec3 cloud_pos;
smooth out float dist;

void main(){
	vec4 sun = vec4(150000.0, 150000.0, 300000.0, 1.0);
	dist = 1.0;
	
	vec3 cloud_sun = cloud_pos.xyz - sun.xyz;
	vec3 point_sun = (ge_MatrixSubModel * ge_MatrixModel * ge_VertexPosition).xyz - sun.xyz;
	if(length(point_sun) > length(cloud_sun)){
		dist = 0.5;
	}else
	if(length((ge_MatrixSubModel * ge_MatrixModel * ge_VertexPosition).xyz - cloud_pos.xyz) < 7500.0){
		dist = 0.5;
	}

/***
	vec3 vVertex = (ge_MatrixSubModel * ge_MatrixModel * ge_VertexPosition).xyz;
	vec3 sunVec = (ge_MatrixSubModel * ge_MatrixModel * ge_VertexPosition).xyz - sun.xyz;
	vec3 eyeVec = vVertex;
	dist = (dot(sunVec,eyeVec)) / (length(sunVec)*length(eyeVec));
***/

	ge_Color = ge_VertexColor;
	ge_TexCoord0.st = ge_VertexTexcoord.st * vec2(0.5);
	/*
	if(cos(ge_MatrixSubModel[3][0] / 4000.0) <= 0.0){
		ge_TexCoord0.x += 0.5;
	}
	ge_Color.a = abs(cos(ge_MatrixSubModel[3][0] / 4000.0));
	*/

	/*
	if(cos(ge_MatrixSubModel[3][1] / 4000.0) * cos(ge_MatrixSubModel[3][0] / 4000.0) <= 0.0){
		ge_TexCoord0.y += 0.5;
	}
	ge_Color.a *= abs(cos(ge_MatrixSubModel[3][1] / 4000.0) * cos(ge_MatrixSubModel[3][0] / 4000.0));
	*/
	ge_Color.a = 1.0;

	mat4 mview = ge_MatrixView;
	mview[0][0] = 1.0;
	mview[0][1] = 0.0;
	mview[0][2] = 0.0;
	mview[1][0] = 0.0;
	mview[1][1] = 1.0;
	mview[1][2] = 0.0;
	mview[2][0] = 0.0;
	mview[2][1] = 0.0;
	mview[2][2] = 1.0;
	ge_Position = ge_MatrixProjection * ge_MatrixView * ge_MatrixSubModel * ge_MatrixModel * ge_VertexPosition;
//	ge_Position = ge_MatrixProjection * ge_MatrixView * ge_MatrixSubModel * ge_MatrixModel * ge_VertexPosition;
}

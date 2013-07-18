uniform int depth_buffer_exists;
uniform sampler2D depth_buffer;
uniform vec3 sun_pos;
smooth in vec3 frag_coord;

void main(){
	vec4 color = ge_Color;
	vec4 tex = texture(ge_Texture, ge_TexCoord0);
	color.a = color.a * tex.r;

	float l = length((ge_TexCoord0.xy*256.0) - vec2(192.0));
	if(l > 43.0 && l < 58.0){
		float d = l - 43.0;
		color.r = pow(0.5 * (1.0 + sin(1.07 + 5.0*( (d-7.5)/7.5 + 0.3333 ))), 2.0);
		color.g = pow(0.5 * (1.0 + sin(1.07 + 5.0*( (d-7.5)/7.5 + 0.0000 ))), 2.0);
		color.b = pow(0.5 * (1.0 + sin(1.07 + 5.0*( (d-7.5)/7.5 - 0.3333 ))), 2.0);
		color.a *= 0.2;
	}

	if((depth_buffer_exists == 1) && (texture(depth_buffer, (sun_pos.xy + vec2(1.0)) / 2.0).r != 1.0)){
		color = vec4(0.0);
	}
/*
	color.r = color.r + (color.g - color.r) * 0.5;
	color.b = color.b - (color.b - color.g) * 0.5;
*/
	ge_FragColor = color;
}

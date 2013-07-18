/*
void main(){
	ge_FragColor = ge_Color;
}
*/

uniform vec3 pos;
uniform float life;
smooth in vec3 coord;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main(){
	vec4 color = vec4(0.0);

	color = vec4(1.0);

	vec2 vec = coord.xy;
	vec.x = vec.x * (1680.0/1050.0);
	float d = length(vec);

	float s = 1.0 - d;
//	color.rgb = vec3(s);
	s = s * exp(s + 4.0);
	color.a = clamp(s, 0.0, 1.0);


	color.a *= 1.0 - life;
	color.rgb = vec3(1.0, 0.0, 0.0) + vec3(0.0, 1.0, 0.0) * life;

	ge_FragColor = color;
}

uniform vec3 pos;
uniform float life;
smooth in vec3 coord;

void main(){
	vec4 color = vec4(0.0);
	color = vec4(1.0);

	vec2 d = coord.xy;
	float f = 1.0 - 2.0*length(d);
	color = vec4(vec3(f * 2.0), f * (1.0-life));

	ge_FragColor = color;
}


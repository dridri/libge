uniform sampler2D ge_Texture1;

void main(){
	vec4 color = vec4(0.0);
	vec4 c;
	float i;
	float div = 1.0;
	float pass = 20.0;
	
	vec2 sz = vec2(textureSize(ge_Texture, 0));
	for(i=-pass; i<pass; i+=1.0){
		float depth = texture(ge_Texture1, ge_TexCoord0.st + vec2(i*2.0, 0.0) / sz).r;
		if(depth < 1.0){
			c = texture(ge_Texture, ge_TexCoord0.st + vec2(i*2.0, 0.0) / sz);
			vec4 f = clamp((exp(c - 0.7) - 1.0) / 2.0, vec4(0.0), vec4(1.0));
			color = color + 2.0 * clamp(5.0 * (exp(f) - 1.0), vec4(0.0), vec4(1.0));
		}
		div++;
	}
	color /= (pass * 2.0);
	ge_FragColor = clamp(color, 0.0, 1.0);
}

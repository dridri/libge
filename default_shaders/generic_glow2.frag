void main(){
	vec4 color = vec4(0.0);
	vec4 c;
	float i;
	float pass = 20.0;

	vec4 base = texture(ge_Texture, ge_TexCoord0.st);
	vec2 sz = vec2(textureSize(ge_Texture, 0));
	for(i=-pass; i<pass; i+=1.0){
		c = texture(ge_Texture, ge_TexCoord0.st + vec2(0.0, i*2.0) / sz);
		color = color + c;
	}
	color /= (pass * 2.0);
	ge_FragColor = clamp(color, 0.0, 1.0);

//	ge_FragColor = texture(ge_Texture, ge_TexCoord0.st);
}

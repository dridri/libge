uniform sampler2D ge_Texture1;
uniform int stars;
smooth in vec3 pos;
/*
float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec4 background(){
	vec4 color = vec4(0.0);

	color = vec4(max(0.8, rand(ge_TexCoord0.yx)), max(0.8, rand(-ge_TexCoord0.xy)), 1.0, 1.0);

	float a = rand( (ge_TexCoord0.xy*2.0-vec2(1.0)) ) * cos(rand(ge_TexCoord0.yx) * ge_Time*10000.0);
//	float diffuse = max(0.2, 8.0 * a);
	float diffuse = 8.0 * a;

	diffuse *= rand(ge_TexCoord0.yx);
	diffuse = max(0.2, diffuse);

	return 8.0 * diffuse * color * texture2D(ge_Texture, ge_TexCoord0);

}

vec4 bigstars(){
	vec4 color = vec4(0.0);
	color = texture(ge_Texture1, ge_TexCoord0);

	return color;
}
*/
void main(){
	ge_FragColor = texture2D(ge_Texture, ge_TexCoord0.st);
/*	return;

	if(stars != 0){
		ge_FragColor += background() + bigstars();
	}else{
		ge_FragColor = texture2D(ge_Texture, ge_TexCoord0.st);
	}*/
}

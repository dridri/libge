smooth in vec3 normal, lightDir, halfVector;

void main(){
	vec3 n, halfV;
	float NdotL, NdotHV;
	vec4 light_color = vec4(0.2, 0.2, 0.2, 0.0);

	n = normalize(normal);
	NdotL = max(dot(n,lightDir),0.0);

	if(NdotL > 0.0){
		light_color += vec4(1.0) * NdotL;
		halfV = normalize(halfVector);
		NdotHV = max(dot(n,halfV),0.0);
	}

	ge_FragColor = light_color * ge_Color * texture(ge_Texture, ge_TexCoord0);
}
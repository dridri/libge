smooth in vec3 normal, lightDir, halfVector;

void main(){
/*
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

	ge_FragColor = light_color;
	ge_FragColor.a = 1.0;
*/
	float alpha = ge_Color.a;
	/*
	if(ge_HasTexture != 0){
		alpha *= texture(ge_Texture, ge_TexCoord0.st).a;
	}
	*/
	float depth = gl_FragCoord.z / gl_FragCoord.w;
	gl_FragDepth = depth;
	ge_FragColor.r = depth;

/*
	float zNear = 1.0;
	float zFar = 10.0;
	depth = (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
	ge_FragColor.rgba = vec4(vec3(depth), 1.0);
*/
//	ge_FragColor.g = 1.0 - alpha;

	if(alpha < 0.75){
	//	discard;
		gl_FragDepth = 1.0;
	}
}

smooth in float dist;

void main(){
	vec4 tex = texture(ge_Texture, ge_TexCoord0);

//	ge_FragColor = max(0.9,ge_FrontMaterial.diffuse.r) * normalize(ge_Color) * 1.9;
	ge_FragColor = vec4(1.0);
	ge_FragColor.a = ge_Color.a * tex.r * tex.a * ge_FrontMaterial.diffuse.a;

	if(ge_FragColor.a > 0.25){
		gl_FragDepth = 0.999999;
	}else{
		gl_FragDepth = 1.0;
	}

//	ge_FragColor.a *= 1.5;
//	ge_FragColor.rgb *= abs(2.0 - ge_FragColor.a) * 0.9;
//	ge_FragColor.rgb *= dist * 1.8;
/*
	if(gl_FragDepth > 0.0){
		ge_FragColor = vec4(1.0);
	}
*/
}

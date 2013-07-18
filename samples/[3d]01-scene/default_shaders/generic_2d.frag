uniform float textured;
//uniform sampler2DRect ge_Texture;

void main(){
	ge_FragColor = ge_Color * (texture(ge_Texture, ge_TexCoord0) + vec4(1.0-textured));
//	ge_FragColor = ge_Color * (texture2DRect(ge_Texture, ge_TexCoord0) + vec4(1.0-textured));
}

uniform float textured;
//uniform sampler2DRect ge_Texture;

void main(){
	ge_FragColor = ge_Color * (texture(ge_Texture, ge_TexCoord0) + vec4(1.0-textured));
//	ge_FragColor = ge_Color * (texture2DRect(ge_Texture, ge_TexCoord0) + vec4(1.0-textured));
}

/*

smooth in vec2 coord;


     
vec4 effect(vec2 light_pos, vec2 texture_coords, vec2 pixel_coords, vec2 screen_size){
	vec4 light_color = vec4(1.0);

//	vec2 light_vec = light_pos - vec2( pixel_coords.x / screen_size.x, ( screen_size.y - pixel_coords.y ) / screen_size.y );
	vec2 light_vec = light_pos - pixel_coords;

	float light_value = 1.0 * ( 1.0 - ( length( light_vec ) / 0.5 ) );

	light_color = light_color * vec4( 1.5, light_value, light_value, 1.0 ) * light_value;

	return light_color;
}

void main(){
	vec4 color = vec4(1.0);
	vec2 light_pos = vec2(0.0, 0.0);
	light_pos = -vec2(cos(ge_Time) * (1050.0 / 1680.0), sin(ge_Time));
	vec2 light_dir = vec2(cos(ge_Time), sin(ge_Time));

	vec2 vec = coord - light_pos;
	vec.x = vec.x * (1680.0 / 1050.0);

	float s = 1.0 - length(vec) / 2.0;
	s = s * exp(s + 1.0);
	color.rgb = vec3(s);

	float angle =  3.14/ 4.0;
	float dv = dot(normalize(vec), normalize(light_dir));
	float spot = (clamp(dv, cos(angle), 1.0) - cos(angle)) * (cos(angle) * 2.0);
	color.rgb *= clamp(spot, 0.0, 1.0);

	ge_FragColor = color;
	ge_FragColor.a = 1.0;
//	ge_FragColor *= texture(ge_Texture, ge_TexCoord0);

//	ge_FragColor = effect(vec4(0.0), vec2(0.0), ((coord+vec2(1.0))/2.0)*vec2(800.0,600.0), vec2(800.0, 600.0));
	ge_FragColor = effect(vec2(0.5, 0.5), vec2(0.0), coord, vec2(800.0, 600.0));
}
*/

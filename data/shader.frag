varying vec2 light_texcoord;
varying vec2 material_texcoord;

uniform sampler2D light_texture;
uniform sampler2D material_texture;

void main()
{
	vec4 light = texture2D(light_texture, light_texcoord);
	gl_FragColor = light * gl_Color;
}

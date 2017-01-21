
varying vec2 texcoord0;
varying vec2 texcoord1;
varying vec4 color;

uniform sampler2D Texture0;
uniform sampler2D Texture1;

void main()
{
	vec4 texel0 = texture2D(Texture0, texcoord0);
	vec4 texel1 = texture2D(Texture1, texcoord1);
	gl_FragColor = texel0 * texel1 * color;
}

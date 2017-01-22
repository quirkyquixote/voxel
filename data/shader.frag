
varying vec2 texcoord;
varying vec4 color;

uniform sampler2D Texture0;
uniform sampler2D Texture1;

void main()
{
	vec4 texel0 = texture2D(Texture0, vec2(color.y, color.z));
	vec4 texel1 = texture2D(Texture1, texcoord);
	vec4 texel2 = vec4(color.x, color.x, color.x, 1.0);
	vec4 texel = texel0 * texel1 * texel2;
	if (texel.a < 0.5)
		discard;
	gl_FragColor = texel;
}

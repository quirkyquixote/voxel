
varying vec2 texcoord0;
varying vec2 texcoord1;
varying vec4 color;

void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	texcoord0 = gl_MultiTexCoord0.xy;
	texcoord1 = gl_MultiTexCoord0.zw;
	color = gl_Color;
}

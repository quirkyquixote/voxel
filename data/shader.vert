
varying vec2 texcoord;
varying vec4 color;

void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	texcoord = gl_MultiTexCoord0.xy;
	color = gl_Color;
}


varying vec2 texcoord0;
varying vec2 texcoord1;

void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	texcoord0 = gl_MultiTexCoord0.xy;
	texcoord1 = gl_MultiTexCoord0.zw;
	gl_FrontColor = gl_Color;
}

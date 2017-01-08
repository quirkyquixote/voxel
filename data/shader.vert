
varying vec2 light_texcoord;
varying vec2 material_texcoord;

void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	light_texcoord = vec2(gl_MultiTexCoord0);
	material_texcoord = vec2(gl_MultiTexCoord1);
	gl_FrontColor = gl_Color;
}

#version 330 compatibility

out vec2 vXY;

void
main( )

{
	vXY = gl_Vertex.xy;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
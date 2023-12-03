// make this 120 for the mac:
#version 330 compatibility

// out variables to be interpolated in the rasterizer and sent to each fragment shader:

out  vec3  vNormal;	  // normal vector
//out	 vec2  vST;
out	 int   vParity; 

uniform int SideVertexCount;

void
main( )
{
	vParity = 0; 
	//int ix = gl_VertexID % SideVertexCount;
	int row_num = gl_VertexID / SideVertexCount;

	//if (ix % 2 == 1)
	//	vParity ^= 1;
	if (row_num % 2 == 1)
		vParity ^= 1;

	vNormal = normalize(gl_NormalMatrix * gl_Normal);
	//vST = gl_MultiTexCoord0.st;
	gl_Position = gl_Vertex;
}

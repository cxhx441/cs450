// make this 120 for the mac:
#version 330 compatibility
#extension GL_EXT_gpu_shader4: enable
#extension GL_EXT_geometry_shader4: enable
layout ( triangles ) in;
layout ( triangle_strip, max_vertices=1000) out;

// out variables to be interpolated in the rasterizer and sent to each fragment shader:

in  vec3  vNormal[3];	  // normal vector
in  vec3  vPointToLight[3];	  // vector from point to light
in  vec3  vPointToEye[3];	  // vector from point to eye
in  vec2  vST[3];	  // (s,t) texture coordinates

out  vec3  gNormal;	  // normal vector
out  vec3  gPointToLight;	  // vector from point to light
out  vec3  gPointToEye;	  // vector from point to eye
out  vec2  gST;	  // (s,t) texture coordinates

uniform float waveTime; 

void
main( )
{
	gNormal = vNormal[0];
	gPointToLight = vPointToLight[0];
	gPointToEye = vPointToEye[0];
	gST = vST[0];
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();

	gNormal = vNormal[1];
	gPointToLight = vPointToLight[1];
	gPointToEye = vPointToEye[1];
	gST = vST[1];
	gl_Position = gl_in[1].gl_Position;
	EmitVertex();

	gNormal = vNormal[2];
	gPointToLight = vPointToLight[2];
	gPointToEye = vPointToEye[2];
	gST = vST[2];
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();
	EndPrimitive();
}

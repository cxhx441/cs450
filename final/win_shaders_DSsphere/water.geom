// make this 120 for the mac:
#version 330 compatibility
#extension GL_EXT_gpu_shader4: enable
#extension GL_EXT_geometry_shader4: enable
layout ( triangles ) in;
layout ( triangle_strip, max_vertices=2000) out;

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
	float x = gl_in[0].gl_Position.x;
	float y = gl_in[0].gl_Position.y;
	float z = gl_in[0].gl_Position.z;

	gNormal = vNormal[0];
	gNormal = normalize( gl_NormalMatrix * gNormal );  // normal vector
	gPointToLight = vPointToLight[0];
	gPointToEye = vPointToEye[0];
	gST = vST[0];
	gl_Position = gl_ModelViewProjectionMatrix * vec4 (x+1, y, z, 1);
	EmitVertex();

	gNormal = vNormal[0];
	gNormal = normalize( gl_NormalMatrix * gNormal );  // normal vector
	gPointToLight = vPointToLight[0];
	gPointToEye = vPointToEye[0];
	gST = vST[0];
	gl_Position = gl_ModelViewProjectionMatrix * vec4 (x-1, y, z, 1);
	EmitVertex();

	gNormal = vNormal[0];
	gNormal = normalize( gl_NormalMatrix * gNormal );  // normal vector
	gPointToLight = vPointToLight[0];
	gPointToEye = vPointToEye[0];
	gST = vST[0];
	gl_Position = gl_ModelViewProjectionMatrix * vec4 (x, y, z+1, 1);
	EmitVertex();
	EndPrimitive();
}

// make this 120 for the mac:
#version 330 compatibility
#extension GL_EXT_gpu_shader4: enable
#extension GL_EXT_geometry_shader4: enable
#define M_PI 3.1415926535897932384626433832795
layout ( triangles ) in;
layout ( triangle_strip, max_vertices=1000) out;

// out variables to be interpolated in the rasterizer and sent to each fragment shader:

out  vec3  gNormal;	  // normal vector
out  vec3  gPointToLight;	  // vector from point to light
out  vec3  gPointToEye;	  // vector from point to eye
out  vec2  gST;	  // (s,t) texture coordinates

uniform float waveTime; 
//uniform float water_d

// where the light is:

const vec3 LightPosition = vec3(  0., 5., 5. );

void
main( )
{
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
	gl_Position = gl_in[1].gl_Position;
	EmitVertex();
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();
	EndPrimitive();
}

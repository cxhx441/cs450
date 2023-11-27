// make this 120 for the mac:
#version 330 compatibility

// out variables to be interpolated in the rasterizer and sent to each fragment shader:

out  vec3  vNormal;	  // normal vector
out  vec3  vPointToLight;	  // vector from point to light
out  vec3  vPointToEye;	  // vector from point to eye
out  vec2  vST;	  // (s,t) texture coordinates

// where the light is:

const vec3 LightPosition = vec3(  0., 5., 5. );

void
main( )
{
	vST = gl_MultiTexCoord0.st;
	vec4 ECposition = gl_ModelViewMatrix * gl_Vertex;
	vNormal = normalize( gl_NormalMatrix * gl_Normal );  // normal vector
	vPointToLight = LightPosition - ECposition.xyz;	    // vector from the point
							// to the light position
	vPointToEye = vec3( 0., 0., 0. ) - ECposition.xyz;       // vector from the point
							// to the eye position
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}

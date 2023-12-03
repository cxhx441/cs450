// make this 120 for the mac:
#version 330 compatibility

// out variables to be interpolated in the rasterizer and sent to each fragment shader:

out  vec3  vNormal;	  // normal vector
out  vec3  vPointToLight;	  // vector from point to light
out  vec3  vPointToEye;	  // vector from point to eye
out  vec2  vST;	  // (s,t) texture coordinates

uniform float waveTime; 
//uniform float water_d

// where the light is:

const vec3 LightPosition = vec3(  0., 5., 5. );

float generateOffset(float x, float z)
{
	float waveLength = 2.f;
	float waveAmplitude = 1.f;
	float PI = 3.14159265359; 
	float radiansX = (x / waveLength + waveTime) * 2.0 * PI;
	float radiansZ = (z / waveLength + waveTime) * 2.0 * PI;
	return waveAmplitude * 0.5 * (sin(radiansZ) + cos(radiansX));
}

vec3 applyDistortion(vec3 vertex)
{
	float xDistortion = generateOffset(vertex.x, vertex.z); 
	float yDistortion = generateOffset(vertex.x, vertex.z); 
	float zDistortion = generateOffset(vertex.x, vertex.z); 
	return vertex + vec3(xDistortion, yDistortion, zDistortion);
}

void
main( )
{
	vec3 cur_vertex = vec3(gl_Vertex.x, 0, gl_Vertex.z);
	cur_vertex = applyDistortion(cur_vertex);

	vST = gl_MultiTexCoord0.st;
	//vec4 ECposition = gl_ModelViewMatrix * gl_Vertex;
	//vNormal = normalize( gl_NormalMatrix * gl_Normal );  // normal vector
	vec4 ECposition = gl_ModelViewMatrix * vec4(cur_vertex, 1.0);
	vNormal = normalize( gl_NormalMatrix * gl_Normal );  // normal vector
	vPointToLight = LightPosition - ECposition.xyz;	    // vector from the point to the light position
	vPointToEye = vec3( 0., 0., 0. ) - ECposition.xyz;       // vector from the point to the eye position
	//gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_Position = gl_ModelViewMatrix * vec4(cur_vertex, 1.0);
}

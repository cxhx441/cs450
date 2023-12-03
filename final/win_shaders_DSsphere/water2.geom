#version 330 compatibility
#extension GL_EXT_gpu_shader4: enable
#extension GL_EXT_geometry_shader4: enable
layout ( triangles ) in;
layout ( triangle_strip, max_vertices=3) out;


in	vec3  vNormal[3];
in  vec3  vPointToLight[3];	  // vector from point to light
in  vec3  vPointToEye[3];	  // vector from point to eye
in  vec2  vST[3];	  // (s,t) texture coordinates
in	vec3  Triangle[3];

out  vec3  gNormal;
out  vec3  gPointToLight;	  // vector from point to light
out  vec3  gPointToEye;	  // vector from point to eye
out  vec2  gST;	  // (s,t) texture coordinates

uniform float waveTime; 

// where the light is:

const vec3 LightPosition = vec3(  0., 5., 5. );
const float PI = 3.14159265359; 

float generateOffset(float x, float z)
{
	float waveLength = 2.f;
	float waveAmplitude = 0.5f;
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
	vec3 v0 = gl_in[0].gl_Position.xyz;
	vec3 v1 = gl_in[1].gl_Position.xyz;
	vec3 v2 = gl_in[2].gl_Position.xyz;

	v0 = applyDistortion(v0);
	v1 = applyDistortion(v1);
	v2 = applyDistortion(v2);

	gNormal = cross(v0, v1); 
	gNormal = normalize( gl_NormalMatrix * gNormal );  // normal vector
	if (gNormal.y < 0)
		gNormal = gNormal * vec3 (-1, -1, -1);


	vec4 ECposition = gl_ModelViewMatrix * vec4(v0, 1.0);
	gPointToLight = LightPosition - ECposition.xyz;	    // vector from the point to the light position
	gPointToEye = vec3( 0., 0., 0. ) - ECposition.xyz;       // vector from the point to the eye position
	//gPointToLight = vPointToLight[0];
	//gPointToEye = vPointToEye[0];
	gST = vST[0];
	gl_Position = gl_ModelViewProjectionMatrix * vec4(v0, 1.0);
	EmitVertex();

	ECposition = gl_ModelViewMatrix * vec4(v1, 1.0);
	gPointToLight = LightPosition - ECposition.xyz;	    // vector from the point to the light position
	gPointToEye = vec3( 0., 0., 0. ) - ECposition.xyz;       // vector from the point to the eye position
	//gPointToLight = vPointToLight[1];
	//gPointToEye = vPointToEye[1];
	gST = vST[1];
	gl_Position = gl_ModelViewProjectionMatrix * vec4(v1, 1.0);
	EmitVertex();

	ECposition = gl_ModelViewMatrix * vec4(v1, 1.0);
	gPointToLight = LightPosition - ECposition.xyz;	    // vector from the point to the light position
	gPointToEye = vec3( 0., 0., 0. ) - ECposition.xyz;       // vector from the point to the eye position
	//gPointToLight = vPointToLight[2];
	//gPointToEye = vPointToEye[2];
	gST = vST[2];
	gl_Position = gl_ModelViewProjectionMatrix * vec4(v2, 1.0);
	EmitVertex();
	EndPrimitive(); 
}

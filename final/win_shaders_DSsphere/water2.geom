#version 330 compatibility
#extension GL_EXT_gpu_shader4: enable
#extension GL_EXT_geometry_shader4: enable
layout ( triangles ) in;
layout ( triangle_strip, max_vertices=3) out;

in  vec2  vST[3];	  // (s,t) texture coordinates

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
	float waveLength = 8.f;
	float waveAmplitude = 0.30f;
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
// ts i made up but vs are from docs
//          v3
//         /\
//        /  \
//	     /    \
//      /  t2  \
//   v2 -------- v4
//     /\      /\
//    /  \ t4 /  \
//   / t1 \  /    \
//  /      \/  t3  \
//  ----------------- 
//  v1      v0       v5

	vec3 v0 = gl_in[0].gl_Position.xyz; 
	vec3 v1 = gl_in[1].gl_Position.xyz; 
	vec3 v2 = gl_in[2].gl_Position.xyz; 
	//vec3 v3 = gl_in[3].gl_Position.xyz; 
	//vec3 v4 = gl_in[4].gl_Position.xyz; 
	//vec3 v5 = gl_in[5].gl_Position.xyz; 
										
	v0 = applyDistortion(v0);
	v1 = applyDistortion(v1);
	v2 = applyDistortion(v2);
	//v3 = applyDistortion(v3);
	//v4 = applyDistortion(v4);
	//v5 = applyDistortion(v5);
	vec3 v0_to_v1 = vec3 (v1.x - v0.x, v1.y - v0.y, v1.z - v0.z);
	vec3 v0_to_v2 = vec3 (v2.x - v0.x, v2.y - v0.y, v2.z - v0.z);
	gNormal = cross(v0_to_v1, v0_to_v2);


	//if (gNormal.y < 0)
	//	gNormal = gNormal * vec3 (-1, -1, -1);
	gNormal = normalize( gl_NormalMatrix * gNormal );  // normal vector

	vec4 ECposition;
	
	ECposition = gl_ModelViewProjectionMatrix * vec4(v0, 1.0);
	gPointToLight = LightPosition - ECposition.xyz;	    // vector from the point to the light position
	gPointToEye = vec3( 0., 0., 0. ) - ECposition.xyz;       // vector from the point to the eye position
	gST = vST[0];
	gl_Position = gl_ModelViewProjectionMatrix * vec4(v0, 1.0);
	EmitVertex();

	ECposition = gl_ModelViewProjectionMatrix * vec4(v1, 1.0);
	gPointToLight = LightPosition - ECposition.xyz;	    // vector from the point to the light position
	gPointToEye = vec3( 0., 0., 0. ) - ECposition.xyz;       // vector from the point to the eye position
	gST = vST[1];
	gl_Position = gl_ModelViewProjectionMatrix * vec4(v1, 1.0);
	EmitVertex();

	ECposition = gl_ModelViewProjectionMatrix * vec4(v2, 1.0);
	gPointToLight = LightPosition - ECposition.xyz;	    // vector from the point to the light position
	gPointToEye = vec3( 0., 0., 0. ) - ECposition.xyz;       // vector from the point to the eye position
	gST = vST[2];
	gl_Position = gl_ModelViewProjectionMatrix * vec4(v2, 1.0);
	EmitVertex();
	EndPrimitive(); 
}

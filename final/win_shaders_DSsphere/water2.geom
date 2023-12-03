#version 330 compatibility
#extension GL_EXT_gpu_shader4: enable
#extension GL_EXT_geometry_shader4: enable
#define M_PI 3.1415926535897932384626433832795
layout ( points ) in;
layout ( triangle_strip, max_vertices=3) out;


in  vec3  vNormal[];		   // normal vector
//in	vec2  vST[];
in  int   vParity[];

out  vec3  gNormal;
out  vec3  gPointToLight;	  // vector from point to light
out  vec3  gPointToEye;	  // vector from point to eye
//out  vec2  gST[2];	  // (s,t) texture coordinates

uniform float waveTime; 

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
	//gST = vST;

	float x_d = 0.5;
	float z_d = x_d * tan(2 * M_PI / 6); //60 deg

	vec3 cur_vertex;
	vec3 left_vertex;
	vec3 right_vertex;
	vec3 central_point;
	float central_point_shift = z_d * (2.f/3.f);

	float x = gl_in[0].gl_Position.x;
	//float y = gl_in[0].gl_Position.y;
	float z = gl_in[0].gl_Position.z;

	if (vParity[0] == 0) //draw_up (upside down)
	{
		cur_vertex = vec3(x, 0, z);
		central_point = vec3(x, 0, z + central_point_shift);
		// up_left and up_right
		left_vertex = vec3 (cur_vertex.x - x_d, cur_vertex.y, cur_vertex.z + z_d);
		right_vertex = vec3 (cur_vertex.x + x_d, cur_vertex.y, cur_vertex.z + z_d);
	}
	else if (vParity[0] == 1) // draw down (rightside up)
	{
		cur_vertex = vec3(x + x_d, 0, z);
		central_point = vec3(x, 0, z - central_point_shift);
		// down_left and down_right
		left_vertex = vec3 (cur_vertex.x - x_d, cur_vertex.y, cur_vertex.z - z_d);
		right_vertex = vec3 (cur_vertex.x + x_d, cur_vertex.y, cur_vertex.z - z_d);
	}
	gNormal = cross(left_vertex, right_vertex); 

	cur_vertex = applyDistortion(cur_vertex);
	left_vertex = applyDistortion(left_vertex);
	right_vertex = applyDistortion(right_vertex);

	vec4 ECposition = gl_ModelViewMatrix * vec4(central_point, 1.0);
	gNormal = normalize( gl_NormalMatrix * gNormal );  // normal vector
	gPointToLight = LightPosition - ECposition.xyz;	    // vector from the point
							// to the light position
	gPointToEye = vec3( 0., 0., 0. ) - ECposition.xyz;       // vector from the point
							// to the eye position

	ECposition = gl_ModelViewMatrix * vec4(cur_vertex, 1.0);
	gNormal = normalize( gl_NormalMatrix * gNormal );  // normal vector
	gPointToLight = LightPosition - ECposition.xyz;	    // vector from the point to the light position
	gPointToEye = vec3( 0., 0., 0. ) - ECposition.xyz;       // vector from the point to the eye position
	gl_Position = gl_ModelViewProjectionMatrix * vec4(cur_vertex, 1.0);
	EmitVertex();

	ECposition = gl_ModelViewMatrix * vec4(left_vertex, 1.0);
	gNormal = normalize( gl_NormalMatrix * gNormal );  // normal vector
	gPointToLight = LightPosition - ECposition.xyz;	    // vector from the point to the light position
	gPointToEye = vec3( 0., 0., 0. ) - ECposition.xyz;       // vector from the point to the eye position
	gl_Position = gl_ModelViewProjectionMatrix * vec4(left_vertex, 1.0);
	EmitVertex();

	ECposition = gl_ModelViewMatrix * vec4(right_vertex, 1.0);
	gNormal = normalize( gl_NormalMatrix * gNormal );  // normal vector
	gPointToLight = LightPosition - ECposition.xyz;	    // vector from the point to the light position
	gPointToEye = vec3( 0., 0., 0. ) - ECposition.xyz;       // vector from the point to the eye position
	gl_Position = gl_ModelViewProjectionMatrix * vec4(right_vertex, 1.0);
	EmitVertex();
}

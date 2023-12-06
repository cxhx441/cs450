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
float tri_leg_len; 
float tri_height; 

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


vec3 get_up_right(vec3 v)
{
	float x = tri_leg_len / 2.0f;
	float y = 0;
	float z = tri_height;
	return v + vec3(x, y, z);
}

vec3 get_right(vec3 v)
{
	float x = tri_leg_len / 2.0f;
	float y = 0;
	float z = 0;
	return v + vec3(x, y, z);
}

vec3 get_down_right(vec3 v)
{
	float x = tri_leg_len / 2.0f;
	float y = 0;
	float z = -tri_height;
	return v + vec3(x, y, z);
}

vec3 get_up_left(vec3 v)
{
	float x = -tri_leg_len / 2.0f;
	float y = 0;
	float z = tri_height;
	return v + vec3(x, y, z);
}

vec3 get_left(vec3 v)
{
	float x = -tri_leg_len / 2.0f;
	float y = 0;
	float z = 0;
	return v + vec3(x, y, z);
}

vec3 get_down_left(vec3 v)
{
	float x = -tri_leg_len / 2.0f;
	float y = 0;
	float z = -tri_height;
	return v + vec3(x, y, z);
}

vec3 flip_normal_if_neg(vec3 normal)
{
	if (normal.y < 0)
	{
		return normal * vec3(-1);
	}
	return normal;
}

//   pattern =         
//       v11------v3
//        /\      /\   
//       /  \ t12/  \  
//	    / t11\  / t1 \ 
//     /      \/      \
//   v10------v0------v4
//    /\      /\      /\      
//   /  \ t10/  \ t2 /  \    
//  / t9 \  / t0 \  /    \  
// /      \/      \/  t3  \
//v9------v2------v1------v5
// \      /\      /\      /
//  \ t8 /  \ t6 /  \ t4 /
//   \  / t7 \  / t5 \  / 
//    \/      \/      \/ 
//    v8------v7------v6
//
// will only be exporting t1, t2, t3 with average normals based on movement of other triangles.
//        v6------v1
//        /\      /\   
//       /  \ t6 /  \  
//	    /    \  /    \ 
//     /  t5  \/  t2  \
//   v5-------v0------v2
//     \  t4  /\  t2  /
//      \    /  \    /
//       \  / t3 \  /
//        \/      \/
//        v4------v3
vec3 get_avg_normal(vec3 v0)
{
	vec3 v1 = get_up_right(v0);
	vec3 v2 = get_right(v0);
	vec3 v3 = get_down_right(v0);
	vec3 v4 = get_up_left(v0);
	vec3 v5 = get_left(v0);
	vec3 v6 = get_down_left(v0);

	v0 = applyDistortion(v0);
	v1 = applyDistortion(v1);
	v2 = applyDistortion(v2);
	v3 = applyDistortion(v3);
	v4 = applyDistortion(v4);
	v5 = applyDistortion(v5);
	v6 = applyDistortion(v6);
	
	vec3 t0N = cross(v2-v0, v1-v0);
	vec3 t1N = cross(v3-v0, v2-v0);
	vec3 t2N = cross(v4-v0, v3-v0);
	vec3 t3N = cross(v5-v0, v4-v0);
	vec3 t4N = cross(v6-v0, v5-v0);
	vec3 t5N = cross(v1-v0, v6-v0);

	t0N = flip_normal_if_neg(t0N);
	t1N = flip_normal_if_neg(t1N);
	t2N = flip_normal_if_neg(t2N);
	t3N = flip_normal_if_neg(t3N);
	t4N = flip_normal_if_neg(t4N);
	t5N = flip_normal_if_neg(t5N);

	vec3 avgNorm = (t0N + t1N + t2N + t3N + t4N +t5N) / 6.0;
	avgNorm = normalize(gl_NormalMatrix * avgNorm);
	avgNorm = flip_normal_if_neg(avgNorm);
	return avgNorm;
}

void
main( )
{
	vec3 v0 = gl_in[0].gl_Position.xyz; 
	vec3 v1 = gl_in[1].gl_Position.xyz; 
	vec3 v2 = gl_in[2].gl_Position.xyz; 

	// get leg_len
	float max_x = max(v0.x, v1.x);
	max_x = max(max_x, v2.x);
	float min_x = min(v0.x, v1.x);
	min_x = min(min_x, v2.x);
	tri_leg_len = abs(max_x - min_x);
	tri_height = sin(2.0*PI / 6.0) * tri_leg_len; 

	// Average Normals
	vec3 v0Normal = get_avg_normal(v0);
	vec3 v1Normal = get_avg_normal(v1);
	vec3 v2Normal = get_avg_normal(v2);

	// apply distortion
	v0 = applyDistortion(v0);
	v1 = applyDistortion(v1);
	v2 = applyDistortion(v2);

	vec4 ECposition;
	
	gNormal = v0Normal;
	ECposition = gl_ModelViewProjectionMatrix * vec4(v0, 1.0);
	gPointToLight = LightPosition - ECposition.xyz;	    // vector from the point to the light position
	gPointToEye = vec3( 0., 0., 0. ) - ECposition.xyz;       // vector from the point to the eye position
	gST = vST[0];
	gl_Position = gl_ModelViewProjectionMatrix * vec4(v0, 1.0);
	EmitVertex();

	gNormal = v1Normal;
	ECposition = gl_ModelViewProjectionMatrix * vec4(v1, 1.0);
	gPointToLight = LightPosition - ECposition.xyz;	    // vector from the point to the light position
	gPointToEye = vec3( 0., 0., 0. ) - ECposition.xyz;       // vector from the point to the eye position
	gST = vST[1];
	gl_Position = gl_ModelViewProjectionMatrix * vec4(v1, 1.0);
	EmitVertex();

	gNormal = v2Normal;
	ECposition = gl_ModelViewProjectionMatrix * vec4(v2, 1.0);
	gPointToLight = LightPosition - ECposition.xyz;	    // vector from the point to the light position
	gPointToEye = vec3( 0., 0., 0. ) - ECposition.xyz;       // vector from the point to the eye position
	gST = vST[2];
	gl_Position = gl_ModelViewProjectionMatrix * vec4(v2, 1.0);
	EmitVertex();
	EndPrimitive(); 
}

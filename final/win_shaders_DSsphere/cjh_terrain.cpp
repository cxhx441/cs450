#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#ifndef F_PI
#define F_PI		((float)(M_PI))
#define F_2_PI		((float)(2.f*F_PI))
#define F_PI_2		((float)(F_PI/2.f))
#endif


/// <summary>
/// TERRAIN CALCS
/// </summary>
const float AMPLITUDE = 70.f;
const int OCTAVES = 3;
const float ROUGHNESS = 0.3f;

void
set_terrain_seed(int new_seed)
{
	if (new_seed == 0)
		TERRAIN_SEED = time(NULL) % 1000000000; // billion
	else
		TERRAIN_SEED = new_seed;
	printf("Terrain Seed Used: %d\n", TERRAIN_SEED);
}

float
gen_noise(int x, int z)
{
	srand(x * 49632 + z * 325176 + TERRAIN_SEED);
	float r_num = (float)rand() / RAND_MAX; // 0 to 1
	r_num = r_num * 2.f - 1.f; // -1 to 1
	return r_num;
}

float 
gen_smoothed_noise(int x, int z)
{
	float y_corners_weight = 16.f;
	float y_sides_weight = 8.f;
	float y_center_weight = 4.f;

	float y_corners = (
		gen_noise(x - 1, z + 1) +
		gen_noise(x - 1, z - 1) +
		gen_noise(x + 1, z + 1) +
		gen_noise(x + 1, z - 1)
		) / (float)y_corners_weight;

	float y_sides = (
		gen_noise(x - 1, z) +
		gen_noise(x + 1, z) +
		gen_noise(x, z + 1) +
		gen_noise(x, z - 1)
		) / (float)y_sides_weight;

	float y_center = gen_noise(x, z) / (float)y_center_weight;

	float y = (y_corners + y_sides + y_center) / (float)3.f;
	return y;

}

float
interpolate(float a, float b, float blend)
{
	double theta = blend * F_PI;
	float blend_factor = (float) (1.f - cos(theta)) * 0.5f; // 0 to 1; 
	return a * (1.f - blend_factor) + b * blend_factor;
}

float 
get_interpolated_noise(float x, float z)
{
	int int_x = x; 
	int int_z = z; 
	float frac_x = int_x - x; 
	float frac_z = int_z - z; 
	float v1 = gen_smoothed_noise(int_x    , int_z    );
	float v2 = gen_smoothed_noise(int_x + 1, int_z    );
	float v3 = gen_smoothed_noise(int_x    , int_z + 1);
	float v4 = gen_smoothed_noise(int_x + 1, int_z + 1);
	float i1 = interpolate(v1, v2, frac_x);
	float i2 = interpolate(v3, v4, frac_x);
	return interpolate(i1, i2, frac_z);
}

float
get_height(float x, float z)
{
	float total = 0; 
	float d = (float)pow(2, OCTAVES - 1);
	for (int i = 0; i < OCTAVES; i++)
	{
		float freq = (float)pow(2, i) / d; 
		float amp = (float)pow(ROUGHNESS, i) * AMPLITUDE;
		total += get_interpolated_noise(x * freq, z * freq) * amp;
	}
	return total;
	//float total = get_interpolated_noise(x/4.f, z/4.f) * AMPLITUDE;
	//total += get_interpolated_noise(x/2.f, z/2.f) * AMPLITUDE/3.f;
	//total += get_interpolated_noise(x, z) * AMPLITUDE/6.f;
	//return total;
}

void
cjh_terrain( int side_length, int side_vertex_count )
{
	int x0 = 0;
	int z0 = 0;
	float d = side_length / (float) side_vertex_count;
	
	struct vertex
	{
		float x; 
		float y; 
		float z;
	};
	
	vertex p1;
	vertex p2;
	vertex p3;
	vertex p4;
	
	float t1_u_vector[3]; 
	float t1_v_vector[3]; 
	float t1_normal[3];
	float t2_u_vector[3]; 
	float t2_v_vector[3]; 
	float t2_normal[3];


	//printf("x: 4, z: 5, result: %f\n", get_height(4, 5));
	//printf("x: 4, z: 5, result: %f\n", get_height(4, 5));
	//printf("x: 4, z: 5, result: %f\n", get_height(4, 5));
	//printf("x: 4, z: 5, result: %f\n", get_height(4, 5));
	//printf("x: 5, z: 5, result: %f\n", get_height(5, 5));

	glPushMatrix();
	for( int i = 0; i < side_vertex_count; i++ )
	{
		for( int j = 0; j < side_vertex_count; j++ )
		{
			//   p1    p2
			//    ------
			//    |	  /|
			//    |  / |
			//    | /  |
			//    |/   |
			//    ------
			//   p3    p4

			// set x's
			p1.x = x0 + d * (float)(j + 0);
			p2.x = x0 + d * (float)(j + 1);
			p3.x = x0 + d * (float)(j + 0);
			p4.x = x0 + d * (float)(j + 1);
			// set z's
			p1.z = z0 + d * (float)(i + 0);
			p2.z = x0 + d * (float)(i + 0);
			p3.z = x0 + d * (float)(i + 1);
			p4.z = x0 + d * (float)(i + 1);
			// set y's
			p1.y = get_height(p1.x, p1.z); 
			p2.y = get_height(p2.x, p2.z); 
			p3.y = get_height(p3.x, p3.z); 
			p4.y = get_height(p4.x, p4.z); 

			// get vectors for normals
			t1_u_vector[0] = p2.x - p1.x;
			t1_u_vector[1] = p2.y - p1.y;
			t1_u_vector[2] = p2.z - p1.z;
			t1_v_vector[0] = p3.x - p1.x;
			t1_v_vector[1] = p3.y - p1.y;
			t1_v_vector[2] = p3.z - p1.z;
			Cross(t1_v_vector, t1_u_vector, t1_normal);

			t2_u_vector[0] = p2.x - p4.x;
			t2_u_vector[1] = p2.y - p4.y;
			t2_u_vector[2] = p2.z - p4.z;
			t2_v_vector[0] = p3.x - p4.x;
			t2_v_vector[1] = p3.y - p4.y;
			t2_v_vector[2] = p3.z - p4.z;
			Cross(t2_u_vector, t2_v_vector, t2_normal);


			glBegin( GL_TRIANGLES );
				//glNormal3f( 0., 1., 0. );
				glNormal3f(t1_normal[0], t1_normal[1], t1_normal[2]);
				glVertex3f(p1.x, p1.y, p1.z);
				glVertex3f(p2.x, p2.y, p2.z);
				glVertex3f(p3.x, p3.y, p3.z);
			glEnd();

			glBegin( GL_TRIANGLES );
				//glNormal3f( 0., 1., 0. );
				glNormal3f(t2_normal[0], t2_normal[1], t2_normal[2]);
				// 2, 4, 3 for clockwise draw
				glVertex3f(p2.x, p2.y, p2.z);
				glVertex3f(p4.x, p4.y, p4.z);
				glVertex3f(p3.x, p3.y, p3.z);
			glEnd( );
		}
	}
	glPopMatrix();
}


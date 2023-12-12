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

struct vertex
{
	float x; 
	float y; 
	float z;
};

struct vector3
{
	float x; 
	float y; 
	float z;
};
void
normalize(float v[3])
{
	float magnitude = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	if (magnitude > 0.0)
	{
		magnitude = sqrtf(magnitude);
		v[0] /= magnitude;
		v[1] /= magnitude;
		v[2] /= magnitude;
	}
}

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

vector3 get_normal(vertex p0, vertex p1, vertex p2)
{
	vector3 u_vec;
	vector3 v_vec;
	vector3 normal;

	u_vec.x = p1.x - p0.x;
	u_vec.y = p1.y - p0.y;
	u_vec.z = p1.z - p0.z;
	v_vec.x = p2.x - p0.x;
	v_vec.y = p2.y - p0.y;
	v_vec.z = p2.z - p0.z;

	float u[3] = { u_vec.x, u_vec.y, u_vec.z };
	float v[3] = { v_vec.x, v_vec.y, v_vec.z };
	float n[3] = { 0, 0, 0 };
	Cross(u, v, n);
	normalize(n);
	normal = { n[0], n[1], n[2] };
	return normal;
}
vector3 get_avg_normal(float x, float y, float z, float d)
{ 
	vertex p0 = {x    , y, z    };
	vertex p1 = {x    , y, z + d};
	vertex p2 = {x + d, y, z + d};
	vertex p3 = {x + d, y, z    };
	vertex p4 = {x    , y, z - d};
	vertex p5 = {x - d, y, z - d};
	vertex p6 = {x - d, y, z    };

	p0.y = get_height(p0.x, p0.y);
	p1.y = get_height(p1.x, p1.y);
	p2.y = get_height(p2.x, p2.y);
	p3.y = get_height(p3.x, p3.y);
	p4.y = get_height(p4.x, p4.y);
	p5.y = get_height(p5.x, p5.y);
	p6.y = get_height(p6.x, p6.y);
	
	vector3 t1_n = get_normal(p0, p1, p2);
	vector3 t2_n = get_normal(p0, p2, p3);
	vector3 t3_n = get_normal(p0, p3, p4);
	vector3 t4_n = get_normal(p0, p4, p5);
	vector3 t5_n = get_normal(p0, p5, p6);
	vector3 t6_n = get_normal(p0, p6, p1);

	vector3 avg_normal; 
	avg_normal.x = ( t1_n.x + t2_n.x + t3_n.x + t4_n.x + t5_n.x + t6_n.x ) / 6.f;
	avg_normal.y = ( t1_n.y + t2_n.y + t3_n.y + t4_n.y + t5_n.y + t6_n.y ) / 6.f;
	avg_normal.z = ( t1_n.z + t2_n.z + t3_n.z + t4_n.z + t5_n.z + t6_n.z ) / 6.f;

	return avg_normal;
}

void
cjh_terrain( int side_length, int side_vertex_count )
{
	int x0 = 0;
	int z0 = 0;
	float d = side_length / (float) side_vertex_count;
	
	
	vertex p1;
	vertex p2;
	
	glPushMatrix();
	for( int i = 0; i < side_vertex_count; i++ )
	{
		glBegin(GL_TRIANGLE_STRIP);
		for( int j = 0; j < side_vertex_count; j++ )
		{
			//   p1    p1(++)
			//    ------
			//    |	  /|
			//    |  / |
			//    | /  |
			//    |/   |
			//    ------
			//   p2    p2(++)

			p1.x = x0 + d * (float)j;
			p2.x = x0 + d * (float)j;
			p1.y = 0;
			p2.y = 0;
			p1.z = z0 + d * (float)(i - 0);
			p2.z = x0 + d * (float)(i - 1);

			// get normals
			vector3 p1N = get_avg_normal(p1.x, p1.y, p1.z, d);
			vector3 p2N = get_avg_normal(p2.x, p2.y, p2.z, d);

			// set y's
			p1.y = get_height(p1.x, p1.z); 
			p2.y = get_height(p2.x, p2.z); 

			float s = (i * d) / (float)side_length;
			float t = (j * d) / (float)side_length;

			glTexCoord2f(s, t);

			glNormal3f(p1N.x, p1N.y, p1N.z);
			glVertex3f(p1.x, p1.y, p1.z);

			// clockwise draw? so maybe flip these
			glNormal3f(p2N.x, p2N.y, p2N.z);
			glVertex3f(p2.x, p2.y, p2.z);


		}
		glEnd( );
	}
	glPopMatrix();
}


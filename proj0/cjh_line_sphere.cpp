#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#include <OpenGL/gl.h>
#include "cjh_circle_vertices.cpp"

#ifndef F_PI
#define F_PI		((float)(M_PI))
#define F_2_PI		((float)(2.f*F_PI))
#define F_PI_2		((float)(F_PI/2.f))
#endif

void
cjh_line_sphere( float radius, int num_circle_segs, int num_rotations, float colors[9])
{
  float drotation = 360. / num_rotations;

  for( int i = 0; i < num_rotations/2; i++ )
  {

    float color[3] = {colors[0], colors[1], colors[2]};
    glRotatef(drotation, 1, 0, 0);
    if (i == 1){ continue; }
    glBegin( GL_LINE_STRIP );
      cjh_circle_vertices(radius, num_circle_segs, 'z', color);
    glEnd( ); 
  }

  for( int i = 0; i < num_rotations/2; i++ )
  {
    float color[3] = {colors[3], colors[4], colors[5]};
    glRotatef(drotation, 0, 1, 0);
    if (i == 1){ continue; }
    glBegin( GL_LINE_STRIP );
      cjh_circle_vertices(radius, num_circle_segs, 'x', color);
    glEnd( ); 
  }

  for( int i = 0; i < num_rotations/2; i++ )
  {
    float color[3] = {colors[6], colors[7], colors[8]};
    glRotatef(drotation, 0, 0, 1);
    if (i == 1){ continue; }
    glBegin( GL_LINE_STRIP );
      cjh_circle_vertices(radius, num_circle_segs, 'y', color);
    glEnd( ); 
  }
}

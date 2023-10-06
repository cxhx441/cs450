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
cjh_line_sphere( float radius, int num_circle_segs, int num_rotations )
{
  float drotation = 360. / num_rotations;

  for( int i = 0; i < num_rotations/2; i++ )
  {
    glColor3f( 1, .1, 1 ); 
    glRotatef(drotation, 1, 0, 0);
    if (i == 1){ continue; }
    glBegin( GL_LINE_STRIP );
      cjh_circle_vertices(radius, num_circle_segs, 'z');
    glEnd( ); 
  }

  for( int i = 0; i < num_rotations/2; i++ )
  {
    glColor3f( 0.1, 1, 1 ); 
    glRotatef(drotation, 0, 1, 0);
    if (i == 1){ continue; }
    glBegin( GL_LINE_STRIP );
      cjh_circle_vertices(radius, num_circle_segs, 'x');
    glEnd( ); 
  }

  for( int i = 0; i < num_rotations/2; i++ )
  {
    glColor3f( 1, 1, .1 ); 
    glRotatef(drotation, 0, 0, 1);
    if (i == 1){ continue; }
    glBegin( GL_LINE_STRIP );
      cjh_circle_vertices(radius, num_circle_segs, 'y');
    glEnd( ); 
  }
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#include <OpenGL/gl.h>

#ifndef F_PI
#define F_PI		((float)(M_PI))
#define F_2_PI		((float)(2.f*F_PI))
#define F_PI_2		((float)(F_PI/2.f))
#endif

void
cjh_circle_vertices( float radius, int numsegs, char normal)
{
  float dang = 2. * M_PI / (float)( numsegs - 1 ); 
  float ang = 0.;
  for( int j = 0; j < numsegs; j++ )
  { 
    switch (normal) 
    {
      case 'x':
        glVertex3f( 0, radius*cos(ang), radius*sin(ang) ); 
        break;
      case 'y':
        glVertex3f( radius*cos(ang), 0, radius*sin(ang) ); 
        break;
      case 'z':
        glVertex3f( radius*cos(ang), radius*sin(ang), 0 ); 
        break;
    }
    ang += dang; 
  }
}

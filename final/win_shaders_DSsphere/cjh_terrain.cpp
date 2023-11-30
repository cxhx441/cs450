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

void
cjh_terrain( int size, int vertex_count )
{
	glBegin( GL_TRIANGLES );

		int count = vertex_count * vertex_count;

		//float vertices[count * 3];
		//float normals[count * 3];
		//float texture_coords[count * 2];

		//int indices[6*(vertex_count-1) * (vertex_count-1)];
		//int vertexPointer = 0;

		for(int i=0;i<vertex_count;i++){
			for(int j=0;j<vertex_count;j++){
				//vertices[vertexPointer*3] = (float)j/((float)vertex_count - 1) * size;
				//vertices[vertexPointer*3+1] = 0;
				//vertices[vertexPointer*3+2] = (float)i/((float)vertex_count - 1) * size;
				//normals[vertexPointer*3] = 0;
				//normals[vertexPointer*3+1] = 1;
				//normals[vertexPointer*3+2] = 0;
				//textureCoords[vertexPointer*2] = (float)j/((float)vertex_count - 1);
				//textureCoords[vertexPointer*2+1] = (float)i/((float)vertex_count - 1);

				//vertexPointer++;

				//me
				glVertex3f((float)j / ((float)vertex_count - 1) * size, 0, (float)i / ((float)vertex_count - 1) * size);
				glNormal3f(0, 1, 0);
				glTexCoord2f((float)j / ((float)vertex_count - 1), (float)i / ((float)vertex_count - 1));
			}
		}
		//int pointer = 0;
		//for(int gz=0;gz<vertex_count-1;gz++){
		//	for(int gx=0;gx<vertex_count-1;gx++){
		//		int topLeft = (gz*vertex_count)+gx;
		//		int topRight = topLeft + 1;
		//		int bottomLeft = ((gz+1)*vertex_count)+gx;
		//		int bottomRight = bottomLeft + 1;
		//		indices[pointer++] = topLeft;
		//		indices[pointer++] = bottomLeft;
		//		indices[pointer++] = topRight;
		//		indices[pointer++] = topRight;
		//		indices[pointer++] = bottomLeft;
		//		indices[pointer++] = bottomRight;
		//	}
		//}

	glEnd( );
}

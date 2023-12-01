#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifndef F_PI
#define F_PI		((float)(M_PI))
#define F_2_PI		((float)(2.f*F_PI))
#define F_PI_2		((float)(F_PI/2.f))
#endif


#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#endif

#include "glew.h"
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include "glut.h"


// title of these windows:

const char *WINDOWTITLE = "OpenGL Shader Sample -- Craig Harris";
const char *GLUITITLE   = "User Interface Window";

// what the glui package defines as true and false:

const int GLUITRUE  = true;
const int GLUIFALSE = false;

// the escape key:

const int ESCAPE = 0x1b;

// initial window size:

const int INIT_WINDOW_SIZE = 1200;

// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = 1.f;
const float SCLFACT = 0.005f;

// minimum allowable scale factor:

const float MINSCALE = 0.05f;

// scroll wheel button values:

const int SCROLL_WHEEL_UP   = 3;
const int SCROLL_WHEEL_DOWN = 4;

// equivalent mouse movement when we click the scroll wheel:

const float SCROLL_WHEEL_CLICK_FACTOR = 5.f;

// active mouse buttons (or them together):

const int LEFT   = 4;
const int MIDDLE = 2;
const int RIGHT  = 1;

// which projection:

enum Projections
{
	ORTHO,
	PERSP
};

// which button:

enum ButtonVals
{
	RESET,
	QUIT
};

// window background color (rgba):

const GLfloat BACKCOLOR[ ] = { 0., 0., 0., 1. };

// line width for the axes:

const GLfloat AXES_WIDTH   = 3.;

// the color numbers:
// this order must match the radio button order, which must match the order of the color names,
// 	which must match the order of the color RGB values

enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA
};

char * ColorNames[ ] =
{
	(char *)"Red",
	(char*)"Yellow",
	(char*)"Green",
	(char*)"Cyan",
	(char*)"Blue",
	(char*)"Magenta"
};

// the color definitions:
// this order must match the menu order

const GLfloat Colors[ ][3] = 
{
	{ 1., 0., 0. },		// red
	{ 1., 1., 0. },		// yellow
	{ 0., 1., 0. },		// green
	{ 0., 1., 1. },		// cyan
	{ 0., 0., 1. },		// blue
	{ 1., 0., 1. },		// magenta
};

// for lighting:

const float	WHITE[ ] = { 1.,1.,1.,1. };

// for animation:

const int MS_PER_CYCLE = 35000;		// 10000 milliseconds = 10 seconds

// non-constant global variables:

int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
int		DebugOn;				// != 0 means to print debugging info
bool	Freeze;
int		MainWindow;				// window id for main graphics window
int		NowColor;				// index into Colors[ ]
int		NowProjection;			// ORTHO or PERSP
bool	UseAnimation;			// use animation bool
bool	UseKeytime;			// use keytime bool
float	Scale;					// scaling factor
float	Time;					// used for animation, this has a value between 0. and 1.
int		Xmouse, Ymouse;			// mouse values
float	Xrot, Yrot;				// rotation angles in degrees
GLdouble fovy, aspect, zNear, zFar; // for perspective
float	increment;
int		parity;
int		TERRAIN_SEED;


int		TriforceTopList;
int		TriforceLeftList;
int		TriforceRightList;
int		ZeldaTextList;
int		TheLegendOfTextList;
int		LTTPTextList;
int		SwordList;
int		MountainList;
int		HillsList;
int		CastleList;
int		SkyList;
int		WaterList;
int		TerrainList;

// function prototypes:

void	Animate( );
void	Display( );
void	DoAxesMenu( int );
void	DoColorMenu( int );
void	DoDebugMenu( int );
void	DoMainMenu( int );
void	DoProjectMenu( int );
void	DoRasterString( float, float, float, char * );
void	DoStrokeString( float, float, float, float, char * );
float	ElapsedSeconds( );
void	InitGraphics( );
void	InitLists( );
void	InitMenus( );
void	Keyboard( unsigned char, int, int );
void	MouseButton( int, int, int, int );
void	MouseMotion( int, int );
void	Reset( );
void	Resize( int, int );
void	Visibility( int );

void	Axes( float );
void	HsvRgb( float[3], float [3] );
void	Cross(float[3], float[3], float[3]);
float	Dot(float [3], float [3]);
float	Unit(float [3], float [3]);
float	Unit(float [3]);
void    cjh_terrain(int, int);
void    cjh_water(int, int);
float	gen_noise(int, int);
float   gen_smoothed_noise(int, int);
void	set_terrain_seed(int);

// utility to create an array from 3 separate values:

float *
Array3( float a, float b, float c )
{
	static float array[4];

	array[0] = a;
	array[1] = b;
	array[2] = c;
	array[3] = 1.;
	return array;
}

// utility to create an array from a multiplier and an array:

float *
MulArray3( float factor, float array0[ ] )
{
	static float array[4];

	array[0] = factor * array0[0];
	array[1] = factor * array0[1];
	array[2] = factor * array0[2];
	array[3] = 1.;
	return array;
}


float *
MulArray3(float factor, float a, float b, float c )
{
	static float array[4];

	float* abc = Array3(a, b, c);
	array[0] = factor * abc[0];
	array[1] = factor * abc[1];
	array[2] = factor * abc[2];
	array[3] = 1.;
	return array;
}

// these are here for when you need them -- just uncomment the ones you need:

//#include "setmaterial.cpp"
//#include "setlight.cpp"
#include "osusphere.cpp"
//#include "osucone.cpp"
//#include "osutorus.cpp"
#include "bmptotexture.cpp"
#include "loadobjfile.cpp"
#include "keytime.cpp"
#include "glslprogram.cpp"
#include "cjh_terrain.cpp""

float NowS_center, NowT_center, NowRadius_s, NowRadius_t; // elipse center and radius
GLSLProgram Pattern;
GLSLProgram WaterShader;
GLSLProgram SkyShader;
unsigned char* SkyTexture;
GLuint		  SkyTextureList;
Keytimes ellipseS_Center, ellipseT_Center;


// main program:

int
main( int argc, char *argv[ ] )
{
	// turn on the glut package:
	// (do this before checking argc and argv since glutInit might
	// pull some command line arguments out)

	fprintf(stderr, "Starting.\n");
	glutInit(&argc, argv);

	// setup all the graphics stuff:

	InitGraphics( );

	// create the display lists that **will not change**:

	InitLists( );

	// init all the global variables used by Display( ):
	// this will also post a redisplay

	Reset( );

	// setup all the user interface stuff:

	InitMenus( );

	// draw the scene once and wait for some interaction:
	// (this will never return)

	glutSetWindow( MainWindow );
	glutMainLoop( );

	// glutMainLoop( ) never actually returns
	// the following line is here to make the compiler happy:

	return 0;
}


// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutPostRedisplay( ) do it

void
Animate()
{
	// put animation stuff in here -- change some global variables for Display( ) to find:

	int ms = glutGet(GLUT_ELAPSED_TIME);
	ms %= MS_PER_CYCLE;							// makes the value of ms between 0 and MS_PER_CYCLE-1
	Time = (float)ms / (float)MS_PER_CYCLE;		// makes the value of Time between 0. and slightly less than 1.

	// for example, if you wanted to spin an object in Display( ), you might call: glRotatef( 360.f*Time,   0., 1., 0. );

	// force a call to Display( ) next time it is convenient:

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// draw the complete scene:

void
Display()
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting Display.\n");

	// set which window we want to do the graphics into:
	glutSetWindow(MainWindow);

	// erase the background:
	glDrawBuffer(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	// specify shading to be flat:

	glShadeModel(GL_FLAT);

	// set the viewport to be a square centered in the window:

	GLsizei vx = glutGet(GLUT_WINDOW_WIDTH);
	GLsizei vy = glutGet(GLUT_WINDOW_HEIGHT);
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = (vx - v) / 2;
	GLint yb = (vy - v) / 2;
	glViewport(0, 0, vx, vy);

	// set the viewing volume:
	// remember that the Z clipping  values are given as DISTANCES IN FRONT OF THE EYE
	// USE gluOrtho2D( ) IF YOU ARE DOING 2D !
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (NowProjection == ORTHO)
		glOrtho(-2.f, 2.f, -2.f, 2.f, 0.1f, 1000.f);
	else
		//gluPerspective(70.f, 1.f, 0.1f, 1000.f);
		gluPerspective(fovy, aspect, zNear, zFar);

	// place the objects into the scene:
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// set the eye position, look-at position, and up-vector:
	gluLookAt(0.f, 0.f, 5.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f);

	// rotate the scene:
	glRotatef((GLfloat)Yrot, 0.f, 1.f, 0.f);
	glRotatef((GLfloat)Xrot, 1.f, 0.f, 0.f);

	// uniformly scale the scene:
	if (Scale < MINSCALE)
		Scale = MINSCALE;
	glScalef((GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale);

	// possibly draw the axes:
	if (AxesOn != 0)
	{
		glColor3fv(&Colors[NowColor][0]);
		glCallList(AxesList);
	}

	// since we are using glScalef( ), be sure the normals get unitized:
	glEnable(GL_NORMALIZE);

	// draw the box object by calling up its display list:
	Pattern.Use();

	//// set the uniform variables that will change over time:
	//if (UseKeytime == true)
	//{
	//	NowS_center = ellipseS_Center.GetValue(Time * MS_PER_CYCLE, false);
	//	NowT_center = ellipseT_Center.GetValue(Time * MS_PER_CYCLE, false);
	//}
	//else
	//{
	//	NowS_center = 0.5;
	//	NowT_center = 0.5;
	//}

	//if (UseAnimation == true)
	//{
	//	NowRadius_s = 0.1*sin(3 * F_2_PI * Time);
	//	NowRadius_t = 0.1*cos(F_2_PI * Time);
	//}
	//else
	//{
	//	NowRadius_s = 0.05;
	//	NowRadius_t = 0.1;
	//}
	//Pattern.SetUniformVariable( "uSc", NowS_center ); 
	//Pattern.SetUniformVariable( "uTc", NowT_center ); 
	//Pattern.SetUniformVariable( "uRs", NowRadius_s );
	//Pattern.SetUniformVariable( "uRt", NowRadius_t );


	Pattern.SetUniformVariable("LightPosition", 0., 5., 5.);
	Pattern.SetUniformVariable("uColor", 1.f, 1.f, 0.f); // Triforce Color
	glCallList(TriforceTopList);
	glCallList(TriforceLeftList);
	glCallList(TriforceRightList);

	Pattern.SetUniformVariable("uColor", 1.f, 0.f, 0.f); // Zelda Text Color
	glCallList(ZeldaTextList);

	Pattern.SetUniformVariable("uColor", 1.f, 1.f, 1.f); // LTTP Text
	glCallList(LTTPTextList);
	glCallList(TheLegendOfTextList);

	Pattern.SetUniformVariable("uColor", 0.5f, 0.5f, 0.5f); // Sword 
	glCallList(SwordList);

	Pattern.SetUniformVariable("uColor", 0.25f, 0.1f, 0.f); // Castle
	glCallList(CastleList);

	//Pattern.SetUniformVariable("LightPosition", 0., 15., -100.);
	Pattern.SetUniformVariable("uColor", 1.f, 1.f, 1.f); // Mountain
	glCallList(MountainList);

	Pattern.SetUniformVariable("uColor", 0.f, 1.f, 0.f); // Hills
	glCallList(HillsList);


	//Pattern.SetUniformVariable("uColor", 0.5f, 0.5f, 1.f); //  Sky
	//glCallList(SkyList);

	Pattern.SetUniformVariable("uColor", 0.f, 1.f, 0.f); //  Terrain
	glCallList(TerrainList);

	//Pattern.SetUniformVariable("uColor", 0.25f, 0.3f, 0.75f); //  Water
	//glCallList(WaterList);

	Pattern.UnUse( );       // Pattern.Use(0);  also works

	SkyShader.Use();
	glActiveTexture(GL_TEXTURE5); // use texture unit #5
	glBindTexture(GL_TEXTURE_2D, SkyTextureList);
	SkyShader.SetUniformVariable("uTexUnit", 5);
		glCallList(SkyList);
	SkyShader.UnUse();


	WaterShader.Use();
	WaterShader.SetUniformVariable("uColor", 0.25f, 0.3f, 0.75f); //  Water
	WaterShader.SetUniformVariable("waveTime", Time*5);
	glCallList(WaterList);
	WaterShader.UnUse();



	// draw some gratuitous text that just rotates on top of the scene:
	// i commented out the actual text-drawing calls -- put them back in if you have a use for them
	// a good use for thefirst one might be to have your name on the screen
	// a good use for the second one might be to have vertex numbers on the screen alongside each vertex

	//glDisable( GL_DEPTH_TEST );
	//glColor3f( 0.f, 1.f, 1.f );
	//DoRasterString( 0.f, 1.f, 0.f, (char *)"Text That Moves" );


	// draw some gratuitous text that is fixed on the screen:
	//
	// the projection matrix is reset to define a scene whose
	// world coordinate system goes from 0-100 in each axis
	//
	// this is called "percent units", and is just a convenience
	//
	// the modelview matrix is reset to identity as we don't
	// want to transform these coordinates

	//glDisable( GL_DEPTH_TEST );
	//glMatrixMode( GL_PROJECTION );
	//glLoadIdentity( );
	//gluOrtho2D( 0.f, 100.f,     0.f, 100.f );
	//glMatrixMode( GL_MODELVIEW );
	//glLoadIdentity( );
	//glColor3f( 1.f, 1.f, 1.f );
	//DoRasterString( 5.f, 5.f, 0.f, (char *)"Text That Doesn't" );

	// swap the double-buffered framebuffers:

	glutSwapBuffers( );

	// be sure the graphics buffer has been sent:
	// note: be sure to use glFlush( ) here, not glFinish( ) !

	glFlush( );
}


void
DoAxesMenu( int id )
{
	AxesOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoColorMenu( int id )
{
	NowColor = id - RED;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDebugMenu( int id )
{
	DebugOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// main menu callback:

void
DoMainMenu( int id )
{
	switch( id )
	{
		case RESET:
			Reset( );
			break;

		case QUIT:
			// gracefully close out the graphics:
			// gracefully close the graphics window:
			// gracefully exit the program:
			glutSetWindow( MainWindow );
			glFinish( );
			glutDestroyWindow( MainWindow );
			exit( 0 );
			break;

		default:
			fprintf( stderr, "Don't know what to do with Main Menu ID %d\n", id );
	}

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoProjectMenu( int id )
{
	NowProjection = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// use glut to display a string of characters using a raster font:

void
DoRasterString( float x, float y, float z, char *s )
{
	glRasterPos3f( (GLfloat)x, (GLfloat)y, (GLfloat)z );

	char c;			// one character to print
	for( ; ( c = *s ) != '\0'; s++ )
	{
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, c );
	}
}


// use glut to display a string of characters using a stroke font:

void
DoStrokeString( float x, float y, float z, float ht, char *s )
{
	glPushMatrix( );
		glTranslatef( (GLfloat)x, (GLfloat)y, (GLfloat)z );
		float sf = ht / ( 119.05f + 33.33f );
		glScalef( (GLfloat)sf, (GLfloat)sf, (GLfloat)sf );
		char c;			// one character to print
		for( ; ( c = *s ) != '\0'; s++ )
		{
			glutStrokeCharacter( GLUT_STROKE_ROMAN, c );
		}
	glPopMatrix( );
}


// return the number of seconds since the start of the program:

float
ElapsedSeconds( )
{
	// get # of milliseconds since the start of the program:

	int ms = glutGet( GLUT_ELAPSED_TIME );

	// convert it to seconds:

	return (float)ms / 1000.f;
}


// initialize the glui window:

void
InitMenus( )
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitMenus.\n");

	glutSetWindow( MainWindow );

	int numColors = sizeof( Colors ) / ( 3*sizeof(float) );
	int colormenu = glutCreateMenu( DoColorMenu );
	for( int i = 0; i < numColors; i++ )
	{
		glutAddMenuEntry( ColorNames[i], i );
	}

	int axesmenu = glutCreateMenu( DoAxesMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int debugmenu = glutCreateMenu( DoDebugMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int projmenu = glutCreateMenu( DoProjectMenu );
	glutAddMenuEntry( "Orthographic",  ORTHO );
	glutAddMenuEntry( "Perspective",   PERSP );

	int mainmenu = glutCreateMenu( DoMainMenu );
	glutAddSubMenu(   "Axes",          axesmenu);
	glutAddSubMenu(   "Axis Colors",   colormenu);

	glutAddSubMenu(   "Projection",    projmenu );
	glutAddMenuEntry( "Reset",         RESET );
	glutAddSubMenu(   "Debug",         debugmenu);
	glutAddMenuEntry( "Quit",          QUIT );

// attach the pop-up menu to the right mouse button:

	glutAttachMenu( GLUT_RIGHT_BUTTON );
}



// initialize the glut and OpenGL libraries:
//	also setup callback functions

void
InitGraphics()
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitGraphics.\n");

	// request the display modes:
	// ask for red-green-blue-alpha color, double-buffering, and z-buffering:

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	// set the initial window configuration:

	glutInitWindowPosition(0, 0);
	glutInitWindowSize(INIT_WINDOW_SIZE*16/9, INIT_WINDOW_SIZE);

	// open the window and set its title:

	MainWindow = glutCreateWindow(WINDOWTITLE);
	glutSetWindowTitle(WINDOWTITLE);

	// set the framebuffer clear values:

	glClearColor(BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3]);

	// setup the callback functions:
	// DisplayFunc -- redraw the window
	// ReshapeFunc -- handle the user resizing the window
	// KeyboardFunc -- handle a keyboard input
	// MouseFunc -- handle the mouse button going down or up
	// MotionFunc -- handle the mouse moving with a button down
	// PassiveMotionFunc -- handle the mouse moving with a button up
	// VisibilityFunc -- handle a change in window visibility
	// EntryFunc	-- handle the cursor entering or leaving the window
	// SpecialFunc -- handle special keys on the keyboard
	// SpaceballMotionFunc -- handle spaceball translation
	// SpaceballRotateFunc -- handle spaceball rotation
	// SpaceballButtonFunc -- handle spaceball button hits
	// ButtonBoxFunc -- handle button box hits
	// DialsFunc -- handle dial rotations
	// TabletMotionFunc -- handle digitizing tablet motion
	// TabletButtonFunc -- handle digitizing tablet button hits
	// MenuStateFunc -- declare when a pop-up menu is in use
	// TimerFunc -- trigger something to happen a certain time from now
	// IdleFunc -- what to do when nothing else is going on

	glutSetWindow(MainWindow);
	glutDisplayFunc(Display);
	glutReshapeFunc(Resize);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);
	glutPassiveMotionFunc(MouseMotion);
	//glutPassiveMotionFunc( NULL );
	glutVisibilityFunc(Visibility);
	glutEntryFunc(NULL);
	glutSpecialFunc(NULL);
	glutSpaceballMotionFunc(NULL);
	glutSpaceballRotateFunc(NULL);
	glutSpaceballButtonFunc(NULL);
	glutButtonBoxFunc(NULL);
	glutDialsFunc(NULL);
	glutTabletMotionFunc(NULL);
	glutTabletButtonFunc(NULL);
	glutMenuStateFunc(NULL);
	glutTimerFunc(-1, NULL, 0);

	// setup glut to call Animate( ) every time it has
	// 	nothing it needs to respond to (which is most of the time)
	// we don't need to do this for this program, and really should set the argument to NULL
	// but, this sets us up nicely for doing animation

	glutIdleFunc(Animate);

	// init the glew package (a window must be open to do this):

#ifdef WIN32
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		fprintf(stderr, "glewInit Error\n");
	}
	else
		fprintf(stderr, "GLEW initialized OK\n");
	fprintf(stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif
	SkyShader.Init();
	bool valid = SkyShader.Create("sky.vert", "sky.frag");
	if (!valid)
		fprintf(stderr, "Could not create the Sky shader!\n");
	else
		fprintf(stderr, "Sky shader created!\n");
	SkyShader.Use();
	SkyShader.SetUniformVariable("uKambient", 0.1f);
	SkyShader.SetUniformVariable("uKdiffuse", 0.5f);
	SkyShader.SetUniformVariable("uKspecular", 0.4f);
	SkyShader.SetUniformVariable("uColor", 0.5f, 0.5f, 1.f); //  Sky
	SkyShader.SetUniformVariable("uSpecularColor", 0.5f, 0.5f, 1.f); // white
	SkyShader.SetUniformVariable("uShininess", 1.f); // shine
	SkyShader.UnUse();



	
	// all other setups go here, such as GLSLProgram and KeyTime setups:
	Pattern.Init();
	valid = Pattern.Create("pattern.vert", "pattern.frag");
	if (!valid)
		fprintf(stderr, "Could not create the Pattern shader!\n");
	else
		fprintf(stderr, "Pattern shader created!\n");
	// set the uniform variables that will not change:
	Pattern.Use();
	Pattern.SetUniformVariable("uKambient", 0.1f);
	Pattern.SetUniformVariable("uKdiffuse", 0.5f);
	Pattern.SetUniformVariable("uKspecular", 0.4f);
	Pattern.SetUniformVariable("uSpecularColor", 1.f, 1.f, 1.f); // white
	Pattern.SetUniformVariable("uShininess", 12.f); // shine
	Pattern.UnUse();


	WaterShader.Init();
	//bool valid = Water.Create("terrain.vert", "terrain.frag");
	valid = WaterShader.Create("water.vert", "water.frag");
	if (!valid)
		fprintf(stderr, "Could not create the Water shader!\n");
	else
		fprintf(stderr, "Water shader created!\n");
	// set the uniform variables that will not change:
	WaterShader.Use();
	WaterShader.SetUniformVariable("uKambient", 0.1f);
	WaterShader.SetUniformVariable("uKdiffuse", 0.5f);
	WaterShader.SetUniformVariable("uKspecular", 0.4f);
	WaterShader.SetUniformVariable("uColor", 0.f, 0.f, 1.f); // blue
	WaterShader.SetUniformVariable("uSpecularColor", 1.f, 1.f, 1.f); // white
	WaterShader.SetUniformVariable("uShininess", 12.f); // shine
	WaterShader.UnUse();


	// Texture Setup
	glGenTextures(1, &SkyTextureList);
	int num_s, num_t;
	SkyTexture = BmpToTexture("..//..//Textures//sky.bmp", &num_s, &num_t);
	if (SkyTexture == NULL) {
		fprintf(stderr, "cannot open skytexture");
	}
	glBindTexture(GL_TEXTURE_2D, SkyTextureList);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	//glTexImage2D( GL_TEXTURE_2D, 0, 3, num_s, num_t, 0, 3, GL_RGB, GL_UNSIGNED_BYTE, SkyTexture );
	glTexImage2D(GL_TEXTURE_2D, 0, 3, num_s, num_t, 0, GL_RGB, GL_UNSIGNED_BYTE, SkyTexture);



	struct st_pair {
		float s;
		float t;
	};
	struct st_pair ST_PAIRS[] =
	{
		{0.5, 0.5},
		{0.5, 0.5},
		{0.5, 0.5},
		{0.5, 0.5},

		{0.5+.05f, 0.5},
		{0.5    , 0.5+.05f},
		{0.5-.05f, 0.5},
		{0.5    , 0.5-.05f},

		{0.5+.1f, 0.5},
		{0.5    , 0.5+.1f},
		{0.5-.1f, 0.5},
		{0.5    , 0.5-.1f},

		{0.5+.15f, 0.5},
		{0.5    , 0.5+.15f},
		{0.5-.15f, 0.5},
		{0.5    , 0.5-.15f},

		{0.5+.2f, 0.5},
		{0.5    , 0.5+.2f},
		{0.5-.2f, 0.5},
		{0.5    , 0.5-.2f},

		{0.5+.25f, 0.5},
		{0.5    , 0.5+.25f},
		{0.5-.25f, 0.5},
		{0.5    , 0.5-.25f},
	};

	int num_pairs = sizeof(ST_PAIRS) / sizeof(st_pair);
	for (int pair = 0; pair < num_pairs; pair++)
	{
		float front_half = (MS_PER_CYCLE / (num_pairs * 2)) * pair;
		float back_half = MS_PER_CYCLE - front_half; 
		ellipseS_Center.AddTimeValue(front_half, ST_PAIRS[pair].s);
		ellipseT_Center.AddTimeValue(front_half, ST_PAIRS[pair].t);

		ellipseS_Center.AddTimeValue(back_half, ST_PAIRS[pair].s);
		ellipseT_Center.AddTimeValue(back_half, ST_PAIRS[pair].t);
	}
}

// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )

void
InitLists( )
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitLists.\n");

	glutSetWindow( MainWindow );

	// create the object:

	TriforceTopList = glGenLists( 1 );
	glNewList( TriforceTopList, GL_COMPILE );
		LoadObjFile("..//..//OBJs//zelda_2//triforce_top.obj");
	glEndList( );

	TriforceLeftList = glGenLists( 1 );
	glNewList( TriforceLeftList, GL_COMPILE );
		LoadObjFile("..//..//OBJs//zelda_2//triforce_left.obj");
	glEndList( );

	TriforceRightList = glGenLists( 1 );
	glNewList( TriforceRightList, GL_COMPILE );
		LoadObjFile("..//..//OBJs//zelda_2//triforce_right.obj");
	glEndList( );

	ZeldaTextList = glGenLists( 1 );
	glNewList( ZeldaTextList, GL_COMPILE );
		LoadObjFile("..//..//OBJs//zelda_2//zelda_text.obj");
	glEndList( );

	TheLegendOfTextList = glGenLists( 1 );
	glNewList( TheLegendOfTextList, GL_COMPILE );
		LoadObjFile("..//..//OBJs//zelda_2//TheLegendOf_text.obj");
	glEndList( );

	LTTPTextList = glGenLists( 1 );
	glNewList( LTTPTextList, GL_COMPILE );
		LoadObjFile("..//..//OBJs//zelda_2//lttp_text.obj");
	glEndList( );

	SwordList = glGenLists( 1 );
	glNewList( SwordList, GL_COMPILE );
		LoadObjFile("..//..//OBJs//zelda_2//sword.obj");
	glEndList( );

	MountainList = glGenLists( 1 );
	glNewList( MountainList, GL_COMPILE );
		//LoadObjFile("..//..//OBJs//zelda_2//mountain.obj");
		int side_length = 25;
		//set_terrain_seed(0); // 0 == time based
		set_terrain_seed(701323697);
		glPushMatrix();
			glRotatef(-5, 0, 0, 1);
			glTranslatef(-200, 0, -side_length*15);
			glRotatef(30, 1, 0, 0);
			glScalef(40, 20, 3);
			glTranslatef(-side_length/2, -3, -side_length/2);
			cjh_terrain(side_length, 25);
		glPopMatrix();
	glEndList( );

	HillsList = glGenLists( 1 );
	glNewList( HillsList, GL_COMPILE );
		//LoadObjFile("..//..//OBJs//zelda_2//hills.obj");
	glEndList( );

	CastleList = glGenLists( 1 );
	glNewList( CastleList, GL_COMPILE );
		glPushMatrix();
			LoadObjFile("..//..//OBJs//zelda_2//castle2.obj");
		glPopMatrix();
	glEndList( );

	SkyList = glGenLists( 1 );
	glNewList( SkyList, GL_COMPILE );
	glPushMatrix();
		glTranslatef(0, 150, -350);
		glScalef(20, 9, 1);
		LoadObjFile("..//..//OBJs//zelda_2//sky.obj");
	glPopMatrix();
	glEndList( );

	WaterList = glGenLists( 1 );
	glNewList( WaterList, GL_COMPILE );
		//LoadObjFile("..//..//OBJs//zelda_2//water.obj");
		glScalef(5, 1, 5);
		glTranslatef(-side_length/2, -2, -side_length*0.9);
		cjh_water(25, 25);
	glEndList( );

	TerrainList = glGenLists( 1 );
	glNewList( TerrainList, GL_COMPILE );
		set_terrain_seed(701323697);
		side_length = 25;
		glPushMatrix();
			glScalef(5, 1.5, 5);
			glRotatef(-20, 0, 1, 0);
			glTranslatef(-side_length/2, -2, -side_length*0.9);
			cjh_terrain(side_length, 25);
		glPopMatrix();
	glEndList( );

	// create the axes:

	AxesList = glGenLists( 1 );
	glNewList( AxesList, GL_COMPILE );
		glLineWidth( AXES_WIDTH );
			Axes( 1.5 );
		glLineWidth( 1. );
	glEndList( );
}


// the keyboard callback:

void
Keyboard( unsigned char c, int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Keyboard: '%c' (0x%0x)\n", c, c );

	switch( c )
	{
	case 'v':
		fovy += parity*increment;
		printf("fovy %f\naspect %f\nzFar %f\nzNear %f\nincrement %f\n\n", fovy, aspect, zFar, zNear, increment);
		break;
	case 'a':
		aspect -= parity*increment;
		printf("fovy %f\naspect %f\nzFar %f\nzNear %f\nincrement %f\n\n", fovy, aspect, zFar, zNear, increment);
		break;
	case 'z':
		zFar += parity*increment;
		printf("fovy %f\naspect %f\nzFar %f\nzNear %f\nincrement %f\n\n", fovy, aspect, zFar, zNear, increment);
		break;
	case 'n':
		zNear += parity*increment;
		printf("fovy %f\naspect %f\nzFar %f\nzNear %f\nincrement %f\n\n", fovy, aspect, zFar, zNear, increment);
		break;
	case 'j':
		increment /= 10;
		printf("fovy %f\naspect %f\nzFar %f\nzNear %f\nincrement %f\n\n", fovy, aspect, zFar, zNear, increment);
		break;
	case 'l':
		increment *= 10;
		printf("fovy %f\naspect %f\nzFar %f\nzNear %f\nincrement %f\n\n", fovy, aspect, zFar, zNear, increment);
		break;
	case 'r':
		Reset();
		break;
	case 'w':
		parity *= -1;
		break;

	case 'k':
	case 'K': 
		UseKeytime = !UseKeytime;
		break;
	case 't':
	case 'T': 
		UseAnimation = !UseAnimation;
		break;
	case 'f':
	case 'F':
		Freeze = !Freeze;
		if (Freeze)
			glutIdleFunc(NULL);
		else
			glutIdleFunc(Animate);
		break;

	case 'o':
	case 'O':
		NowProjection = ORTHO;
		break;

	case 'p':
	case 'P':
		NowProjection = PERSP;
		break;

	case 'q':
	case 'Q':
	case ESCAPE:
		DoMainMenu( QUIT );	// will not return here
		break;				// happy compiler

	default:
		fprintf( stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c );
	}

	// force a call to Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// called when the mouse button transitions down or up:

void
MouseButton( int button, int state, int x, int y )
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	if( DebugOn != 0 )
		fprintf( stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y );

	
	// get the proper button bit mask:

	switch( button )
	{
		case GLUT_LEFT_BUTTON:
			b = LEFT;		break;

		case GLUT_MIDDLE_BUTTON:
			b = MIDDLE;		break;

		case GLUT_RIGHT_BUTTON:
			b = RIGHT;		break;

		case SCROLL_WHEEL_UP:
			Scale += SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
			// keep object from turning inside-out or disappearing:
			if (Scale < MINSCALE)
				Scale = MINSCALE;
			break;

		case SCROLL_WHEEL_DOWN:
			Scale -= SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
			// keep object from turning inside-out or disappearing:
			if (Scale < MINSCALE)
				Scale = MINSCALE;
			break;

		default:
			b = 0;
			fprintf( stderr, "Unknown mouse button: %d\n", button );
	}

	// button down sets the bit, up clears the bit:

	if( state == GLUT_DOWN )
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}

	glutSetWindow(MainWindow);
	glutPostRedisplay();

}


// called when the mouse moves while a button is down:

void
MouseMotion( int x, int y )
{
	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if( ( ActiveButton & LEFT ) != 0 )
	{
		Xrot += ( ANGFACT*dy );
		Yrot += ( ANGFACT*dx );
	}

	if( ( ActiveButton & MIDDLE ) != 0 )
	{
		Scale += SCLFACT * (float) ( dx - dy );

		// keep object from turning inside-out or disappearing:

		if( Scale < MINSCALE )
			Scale = MINSCALE;
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void
Reset( )
{
	ActiveButton = 0;
	AxesOn = 1;
	DebugOn = 0;
	Freeze = false;
	Scale  = 1.0;
	NowColor = YELLOW;
	NowProjection = PERSP;
	Xrot = Yrot = 0.;
	UseAnimation = true;
	UseKeytime = true;
	fovy = 70.f; // 27?
	aspect = 1.78f; 
	zNear = 0.1f;
	zFar = 1000.f;
	increment = 0.1;
	parity = 1;
}


// called when user resizes the window:

void
Resize( int width, int height )
{
	// don't really need to do anything since window size is
	// checked each time in Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// handle a change to the window's visibility:

void
Visibility ( int state )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Visibility: %d\n", state );

	if( state == GLUT_VISIBLE )
	{
		glutSetWindow( MainWindow );
		glutPostRedisplay( );
	}
	else
	{
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it ...
	}
}



///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////


// the stroke characters 'X' 'Y' 'Z' :

static float xx[ ] = { 0.f, 1.f, 0.f, 1.f };

static float xy[ ] = { -.5f, .5f, .5f, -.5f };

static int xorder[ ] = { 1, 2, -3, 4 };

static float yx[ ] = { 0.f, 0.f, -.5f, .5f };

static float yy[ ] = { 0.f, .6f, 1.f, 1.f };

static int yorder[ ] = { 1, 2, 3, -2, 4 };

static float zx[ ] = { 1.f, 0.f, 1.f, 0.f, .25f, .75f };

static float zy[ ] = { .5f, .5f, -.5f, -.5f, 0.f, 0.f };

static int zorder[ ] = { 1, 2, 3, 4, -5, 6 };

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void
Axes( float length )
{
	glBegin( GL_LINE_STRIP );
		glVertex3f( length, 0., 0. );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., length, 0. );
	glEnd( );
	glBegin( GL_LINE_STRIP );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., 0., length );
	glEnd( );

	float fact = LENFRAC * length;
	float base = BASEFRAC * length;

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 4; i++ )
		{
			int j = xorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( base + fact*xx[j], fact*xy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 5; i++ )
		{
			int j = yorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( fact*yx[j], base + fact*yy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 6; i++ )
		{
			int j = zorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( 0.0, fact*zy[j], base + fact*zx[j] );
		}
	glEnd( );

}


// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );

void
HsvRgb( float hsv[3], float rgb[3] )
{
	// guarantee valid input:

	float h = hsv[0] / 60.f;
	while( h >= 6. )	h -= 6.;
	while( h <  0. ) 	h += 6.;

	float s = hsv[1];
	if( s < 0. )
		s = 0.;
	if( s > 1. )
		s = 1.;

	float v = hsv[2];
	if( v < 0. )
		v = 0.;
	if( v > 1. )
		v = 1.;

	// if sat==0, then is a gray:

	if( s == 0.0 )
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}

	// get an rgb from the hue itself:
	
	float i = (float)floor( h );
	float f = h - i;
	float p = v * ( 1.f - s );
	float q = v * ( 1.f - s*f );
	float t = v * ( 1.f - ( s * (1.f-f) ) );

	float r=0., g=0., b=0.;			// red, green, blue
	switch( (int) i )
	{
		case 0:
			r = v;	g = t;	b = p;
			break;
	
		case 1:
			r = q;	g = v;	b = p;
			break;
	
		case 2:
			r = p;	g = v;	b = t;
			break;
	
		case 3:
			r = p;	g = q;	b = v;
			break;
	
		case 4:
			r = t;	g = p;	b = v;
			break;
	
		case 5:
			r = v;	g = p;	b = q;
			break;
	}


	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}

void
Cross(float v1[3], float v2[3], float vout[3])
{
	float tmp[3];
	tmp[0] = v1[1] * v2[2] - v2[1] * v1[2];
	tmp[1] = v2[0] * v1[2] - v1[0] * v2[2];
	tmp[2] = v1[0] * v2[1] - v2[0] * v1[1];
	vout[0] = tmp[0];
	vout[1] = tmp[1];
	vout[2] = tmp[2];
}

float
Dot(float v1[3], float v2[3])
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}


float
Unit(float vin[3], float vout[3])
{
	float dist = vin[0] * vin[0] + vin[1] * vin[1] + vin[2] * vin[2];
	if (dist > 0.0)
	{
		dist = sqrtf(dist);
		vout[0] = vin[0] / dist;
		vout[1] = vin[1] / dist;
		vout[2] = vin[2] / dist;
	}
	else
	{
		vout[0] = vin[0];
		vout[1] = vin[1];
		vout[2] = vin[2];
	}
	return dist;
}


float
Unit( float v[3] )
{
	float dist = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	if (dist > 0.0)
	{
		dist = sqrtf(dist);
		v[0] /= dist;
		v[1] /= dist;
		v[2] /= dist;
	}
	return dist;
}
 
void
cjh_water( int side_length, int side_vertex_count )
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
			//p1.y = get_height(p1.x, p1.z); 
			//p2.y = get_height(p2.x, p2.z); 
			//p3.y = get_height(p3.x, p3.z); 
			//p4.y = get_height(p4.x, p4.z); 

			p1.y = 0;
			p2.y = 0;
			p3.y = 0;
			p4.y = 0;


			// get vectors for normals
			t1_u_vector[0] = p2.x - p1.x;
			t1_u_vector[1] = p2.y - p1.y;
			t1_u_vector[2] = p2.z - p1.z;
			t1_v_vector[0] = p3.x - p1.x;
			t1_v_vector[1] = p3.y - p1.y;
			t1_v_vector[2] = p3.z - p1.z;
			Cross(t1_v_vector, t1_u_vector, t1_normal);
			Unit(t1_normal);

			t2_u_vector[0] = p2.x - p4.x;
			t2_u_vector[1] = p2.y - p4.y;
			t2_u_vector[2] = p2.z - p4.z;
			t2_v_vector[0] = p3.x - p4.x;
			t2_v_vector[1] = p3.y - p4.y;
			t2_v_vector[2] = p3.z - p4.z;
			Cross(t2_u_vector, t2_v_vector, t2_normal);
			Unit(t2_normal);


			float s = (i * d) / (float)side_length;
			float t = (j * d) / (float)side_length;
			glBegin( GL_TRIANGLES );
				glTexCoord2f(s, t);
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

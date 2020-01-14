#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <iostream>
#include <string>


#define _USE_MATH_DEFINES
#include <math.h>

#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#include "glew.h"
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"


//	This is a sample OpenGL / GLUT program
//
//	The objective is to draw a 3d object and change the color of the axes
//		with a glut menu
//
//	The left mouse button does rotation
//	The middle mouse button does scaling
//	The user interface allows:
//		1. The axes to be turned on and off
//		2. The color of the axes to be changed
//		3. Debugging to be turned on and off
//		4. Depth cueing to be turned on and off
//		5. The projection to be changed
//		6. The transformations to be reset
//		7. The program to quit
//
//	Author:			Joe Graphics

// NOTE: There are a lot of good reasons to use const variables instead
// of #define's.  However, Visual C++ does not allow a const variable
// to be used as an array size or as the case in a switch( ) statement.  So in
// the following, all constants are const variables except those which need to
// be array sizes or cases in switch( ) statements.  Those are #defines.


// title of these windows:

const char *WINDOWTITLE = { "OpenGL / GLUT Sample -- Joe Graphics" };
const char *GLUITITLE   = { "User Interface Window" };


// what the glui package defines as true and false:

const int GLUITRUE  = { true  };
const int GLUIFALSE = { false };


// the escape key:

#define ESCAPE		0x1b


// initial window size:

const int INIT_WINDOW_SIZE = { 600 };


// size of the box:

const float BOXSIZE = { 2.f };



// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = { 1. };
const float SCLFACT = { 0.005f };


// minimum allowable scale factor:

const float MINSCALE = { 0.05f };


// active mouse buttons (or them together):

const int LEFT   = { 4 };
const int MIDDLE = { 2 };
const int RIGHT  = { 1 };


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

const GLfloat AXES_WIDTH   = { 3. };


// the color numbers:
// this order must match the radio button order

enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA,
	WHITE,
	BLACK
};

char * ColorNames[ ] =
{
	"Red",
	"Yellow",
	"Green",
	"Cyan",
	"Blue",
	"Magenta",
	"White",
	"Black"
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
	{ 1., 1., 1. },		// white
	{ 0., 0., 0. },		// black
};


// fog parameters:

const GLfloat FOGCOLOR[4] = { .0, .0, .0, 1. };
const GLenum  FOGMODE     = { GL_LINEAR };
const GLfloat FOGDENSITY  = { 0.30f };
const GLfloat FOGSTART    = { 0. };
const GLfloat FOGEND      = { 10. };


// non-constant global variables:

int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
int		DebugOn;				// != 0 means to print debugging info
int		DepthCueOn;				// != 0 means to use intensity depth cueing
int		DepthBufferOn;			// != 0 means to use the z-buffer
int		DepthFightingOn;		// != 0 means to use the z-buffer
GLuint	BrickList;// object display list
GLuint	PaddleList;
GLuint	WallList;
int		MainWindow;				// window id for main graphics window
float	Scale;					// scaling factor
int		WhichColor;				// index into Colors[ ]
int		WhichProjection;		// ORTHO or PERSP
int		Xmouse, Ymouse;			// mouse values
float	Xrot, Yrot;				// rotation angles in degrees
float	Time;
long double	t = 0;
long double tb = 0;
long double tp = 0;
class ball;
class box;
float bl = 1;
float bw = 0.5;
float bh = 0.5;

float pl = 1;
float pw = 1;
float ph = 1;

float wl = 1;
float ww = 1;
float wh = 1;

float ol = 1;
float ow = 1;
float oh = 1;

bool up = 0;
bool down = 0;
bool left = 0;
bool right = 0;

bool begin = 1;
bool test = 0;
bool gg = 0;
int solve;

int score = 0;
int Lives = 4;

//Class box, for use with bricks, walls, the paddle, and the back wall. Basically creates a volume that you can reference for collision detection
class box {
public:
	float x, y, z;
	float vx = 0;
	float vy = 0;
	float l, w, h;
	bool draw = 1;
	int ti = 0;

	void rmb(); //remove the box after it has been hit
	void mov(int t);
};

void box::rmb() {
	draw = 0;
	score += 1;
}

void box::mov(int t) {
	if (x < 3 && vx > 0 || x > -3 && vx < 0) {
		x += vx * (t - ti);
	}
	if (y < 3 && vy > 0 || y > -3 && vy < 0) {
		y += vy * (t - ti);
	}
	if (x > 2.5) { x = 2.5; vx = 0; }
	if (x < -2.5) { x = -2.5; vx = 0; }
	if (y > 2.5) { y = 2.5; vy = 0; }
	if (y < -2.5) { y = -2.5; vy = 0; }
	
	ti = t;
}

class ball {
public:
	float x, y, z;
	float xi, yi, zi;
	float ti;
	float t2;
	float t3;
	long double vx, vy, vz;
	long double vxi, vyi, vzi;
	float r;

	void mov(int t); //move the ball according to velocity
	bool hit(box b); //check if ball inside brick volume passed in
	void alv(box b); //function to ALter the Velocity of the ball.
	void rst(); //reset the ball with 0 Velocity
	void bgn(); //set the ball into motion
	void rev(box b);

};

void ball::mov(int t) {
	t2 = t;
	t3 = ti;
	x += vx * (t - ti);
	y += vy * (t - ti);
	z += vz * (t - ti);

	
	if (x - r < -3.3) {
		x = -3 + r;
	}
	if (x + r > 3.3) {
		x = 3 - r;
	}
	if (y - r < -3.3) {
		y = -3 + r;
	}
	if (y + r > 3.3) {
		y = 3 - r;
	}
	if (z - r < -5.3) {
		z = -5 + r;
	}
	

	ti = t;
}

void ball::rev(box b) {
	/*
	while (this->hit(b)) {
		x -= vxi * (t2 - t3);
		y -= vyi * (t2 - t3);
		z -= vzi * (t2 - t3);
	}
	*/
	
	if (x - r < -3) {
		x = -3 + r;
	}
	if (x + r > 3) {
		x = 3 - r;
	}
	if (y - r < -3) {
		y = -3 + r;
	}
	if (y + r > 3) {
		y = 3 - r;
	}
	if (z - r < -5) {
		z = -5 + r;
	}
	
}

bool ball::hit(box b) {
	//check if ball is on same depth level z
	if (z > b.z && z-r < b.z+(b.w/2) || z < b.z && z+r > b.z-(b.w/2)) {
		
		//check if ball is on same x
		if (x > b.x && x - r < b.x + (b.l / 2) || x < b.x && x + r > b.x - (b.l / 2)) {

			//check if ball is on same y
			if (y > b.y && y - r < b.y + (b.h / 2) || y < b.y && y + r > b.y - (b.h / 2)) {
				return 1;
			}

		}

	}
	return 0;
}

void ball::alv(box b) {
	vxi = vx;
	vyi = vy;
	vzi = vy;
	int moe = 0.1;
	if (x < b.x + moe + (b.l / 2) && x > b.x - (moe+(b.l / 2))) {
		if (y < b.y + moe + (b.h / 2) && y > b.y - (b.h / 2)) {
			vz *= -1;
		}
		else if (z < b.z + moe + (b.w / 2) && z > b.z - (moe+(b.w / 2))) {
			
			vy = -vy;
			//y += 100*vy;
			//test = 1;
		}
	}
	else if (y < b.y + moe + (b.h / 2) && y > b.y - (moe+(b.h / 2))) {
		if (x < b.x + moe + (b.l / 2) && x > b.x - (moe+(b.l / 2))) {
			vz *= -1;
		}
		else if (z < b.z + moe + (b.w / 2) && z > b.z - (moe+(b.w / 2))) {
			vx *= -1;
		}
	}
	else if (z < b.z + moe + (b.w / 2) && z > b.z - (moe+(b.w / 2))) {
		if (x < b.x + moe + (b.l / 2) && x > b.x - (moe+(b.l / 2))) {
			vy *= -1;
		}
		else if (y < b.y + moe + (b.h / 2) && y > b.y - (moe+(b.h / 2))) {
			vx *= -1;
		}
	}

}

void ball::rst() {
	//set ball x y z back to predecided position. set vx vy vz to 0
	x = 0;
	y = 0;
	z = -1.4;
	vx = 0;
	vy = 0;
	vz = 0;
}

void ball::bgn() {
	//set the ball in motion by setting initial vz
	if (vz == 0) { 
		vz = -0.002; 
		vx = -0.001;
		vy = -0.003;
	}

}
//MAJOR ITEMS IN SCENE////////////////////////////////////////////////////////////////////////
const int totalWalls = 5;
const int totalBricks = 28;

ball Ball;
box Paddle;

box Back;
box Walls[totalWalls];
box Bricks[totalBricks];

// function prototypes:

void	Animate( );
void	Display( );
void	DoAxesMenu( int );
void	DoColorMenu( int );
void	DoDepthBufferMenu( int );
void	DoDepthFightingMenu( int );
void	DoDepthMenu( int );
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
void	KeyboardUp(unsigned char, int, int);
void	MouseButton( int, int, int, int );
void	MouseMotion( int, int );
void	Reset( );
void	Resize( int, int );
void	Visibility( int );

void	Axes( float );
void	HsvRgb( float[3], float [3] );

// main program:

int
main( int argc, char *argv[ ] )
{
	// turn on the glut package:
	// (do this before checking argc and argv since it might
	// pull some command line arguments out)

	glutInit( &argc, argv );


	// setup all the graphics stuff:

	InitGraphics( );


	// create the display structures that will not change:

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


	// this is here to make the compiler happy:
	return 0;
}


// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutMainLoop( ) do it

void
Animate( )
{
	// put animation stuff in here -- change some global variables
	// for Display( ) to find:

	// force a call to Display( ) next time it is convenient:
	int ms = glutGet(GLUT_ELAPSED_TIME);
	ms %= 1000;
	Time = (float)ms / (float)1000;
	t = glutGet(GLUT_ELAPSED_TIME);
	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// draw the complete scene:

void
Display( )
{
	if( DebugOn != 0 )
	{
		fprintf( stderr, "Display\n" );
	}


	// set which window we want to do the graphics into:
	
	if (begin == 1) {
		Ball.x = 0;
		Ball.y = 0;
		Ball.z = -1.4;
		Ball.r = 0.275;
		begin = 0;

		Paddle.l = 2;
		Paddle.w = 2;
		Paddle.h = 0.5;
		Paddle.z = 0.25;

		for (int i = 0; i < totalBricks; i++) {
			Bricks[i].l = 1;
			Bricks[i].w = 0.5;
			Bricks[i].h = 0.5;
		}
		for (int i = 0; i < 7; i++) {
			Bricks[0 + (i * 4)].x = -2.1;
			Bricks[0 + (i * 4)].y = 2.1 - (0.7 * i);
			Bricks[0 + (i * 4)].z = -4.2;

			Bricks[1 + (i * 4)].x = -0.7;
			Bricks[1 + (i * 4)].y = 2.1 - (0.7 * i);
			Bricks[1 + (i * 4)].z = -4.2;
			
			Bricks[2 + (i * 4)].x = 0.7;
			Bricks[2 + (i * 4)].y = 2.1 - (0.7 * i);
			Bricks[2 + (i * 4)].z = -4.2;
			
			Bricks[3 + (i * 4)].x = 2.1;
			Bricks[3 + (i * 4)].y = 2.1 - (0.7 * i);
			Bricks[3 + (i * 4)].z = -4.2;
		}

		Walls[0].l = 1;
		Walls[0].w = 10;
		Walls[0].h = 10;
		Walls[0].x = -3.5;
		Walls[0].y = 0;
		Walls[0].z = 0;

		Walls[1].l = 1;
		Walls[1].w = 10;
		Walls[1].h = 10;
		Walls[1].x = 3.5;
		Walls[1].y = 0;
		Walls[1].z = 0;

		Walls[2].l = 10;
		Walls[2].w = 10;
		Walls[2].h = 1;
		Walls[2].x = 0;
		Walls[2].y = 3.5;
		Walls[2].z = 0;

		Walls[3].l = 10;
		Walls[3].w = 10;
		Walls[3].h = 1;
		Walls[3].x = 0;
		Walls[3].y = -3.5;
		Walls[3].z = 0;

		Walls[4].l = 10;
		Walls[4].w = 1;
		Walls[4].h = 10;
		Walls[4].x = 0;
		Walls[4].y = 0;
		Walls[4].z = -5.5;

		Back.l = 20;
		Back.h = 20;
		Back.w = 0.1;
		Back.x = 0;
		Back.y = 0;
		Back.z = 1;
		
	}
	glutSetWindow( MainWindow );


	// erase the background:

	glDrawBuffer( GL_BACK );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	if( DepthBufferOn != 0 )
		glEnable( GL_DEPTH_TEST );
	else
		glDisable( GL_DEPTH_TEST );


	// specify shading to be flat:

	glShadeModel( GL_FLAT );


	// set the viewport to a square centered in the window:

	GLsizei vx = glutGet( GLUT_WINDOW_WIDTH );
	GLsizei vy = glutGet( GLUT_WINDOW_HEIGHT );
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = ( vx - v ) / 2;
	GLint yb = ( vy - v ) / 2;
	glViewport( xl, yb,  v, v );


	// set the viewing volume:
	// remember that the Z clipping  values are actually
	// given as DISTANCES IN FRONT OF THE EYE
	// USE gluOrtho2D( ) IF YOU ARE DOING 2D !

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	if( WhichProjection == ORTHO )
		glOrtho( -3., 3.,     -3., 3.,     0.1, 1000. );
	else
		gluPerspective( 90., 1.,	0.1, 1000. );


	// place the objects into the scene:

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );

	
	// set the eye position, look-at position, and up-vector:
	//controls for movement acceleration
	if (up == 1 && Paddle.vy < 0.005) {
		Paddle.vy += 0.0001;
	}
	//down
	if (down == 1 && Paddle.vy > -0.005) {
		Paddle.vy -= 0.0001;
	}
	//left
	if (left == 1 && Paddle.vx > -0.005) {
		Paddle.vx -= 0.0001;
	}
	//right
	if (right == 1 && Paddle.vx < 0.005) {
		Paddle.vx += 0.0001;
	}

	if (up == 0 && down == 0) {
		Paddle.vy *= 0.95;
	}
	if (left == 0 && right == 0) {
		Paddle.vx *= 0.95;
	}


	Paddle.mov(t);

	gluLookAt( Paddle.x, Paddle.y, 3.,     Paddle.x, Paddle.y, 0.,     0., 1., 0. );


	// rotate the scene:

	//glRotatef( (GLfloat)Yrot, 0., 1., 0. );
	//glRotatef( (GLfloat)Xrot, 1., 0., 0. );


	// uniformly scale the scene:

	if( Scale < MINSCALE )
		Scale = MINSCALE;
	glScalef( (GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale );


	// set the fog parameters:
	/*
	if( DepthCueOn != 0 )
	{
	*/
		glFogi( GL_FOG_MODE, FOGMODE );
		glFogfv( GL_FOG_COLOR, FOGCOLOR );
		glFogf( GL_FOG_DENSITY, FOGDENSITY );
		glFogf( GL_FOG_START, FOGSTART );
		glFogf( GL_FOG_END, FOGEND );
		glEnable( GL_FOG );
	

	// since we are using glScalef( ), be sure normals get unitized:

	glEnable( GL_NORMALIZE );


	//start by moving the ball according to its velocity. Pass in time so that it can be used to determine how much time has passed since move was last called
	if (!gg) { Ball.mov(t); }

	//Now check to see if the ball is hitting any of the bricks in the brick array. Ball.hit takes a brick and determines if the ball is hitting it. If so,
	//ball.alv decides which direction the brick is from the ball and uses that to alter its velocity. Then the function to remove the brick is called
	
	for(int i = 0; i < totalBricks; i++){
		if(Ball.hit( Bricks[i] )){
			Ball.alv( Bricks[i] );
			Ball.rev(Bricks[i]);
			Bricks[i].rmb();
		}
	}
	
	//Now check to see if the ball is hitting any of the walls. Again, this will call alv if necessary but rmb is not called at any point
	
	for(int i = 0; i < totalWalls; i++){
		if(Ball.hit(Walls[i])){

			Ball.alv(Walls[i]);
			Ball.rev(Walls[i]);
		}
	}
	
	//Now check to see if the ball has hit the paddle.
	
	if(Ball.hit(Paddle)){
		Ball.alv(Paddle);
		Ball.rev(Paddle);
	}
	
	//Now check to see if the ball has hit the back wall. if so, then take a ball away from the lives. If the number of remaining balls becomes zero, then put
	//up text that says GAME OVER. If not, reset the Ball for another round
	
	if(Ball.hit(Back)){
		Lives -= 1;
		if(Lives <= 0){
			gg = 1;
		}
		else{
			Ball.rst();
		}
	}
	

	
	glPushMatrix();
	glTranslatef(-3.5, 0, 0);
	glRotatef(90, 0, 1, 0);
	glCallList(WallList);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(3.5, 0, 0);
	glRotatef(90, 0, 1, 0);
	glCallList(WallList);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 3.5, 0);
	glRotatef(90, 1, 0, 0);
	glCallList(WallList);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, -3.5, 0);
	glRotatef(90, 1, 0, 0);
	glCallList(WallList);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 0, -5.5);
	glCallList(WallList);
	glPopMatrix();


	for (int i = 0; i < totalBricks; i++) {
	
		if (Bricks[i].draw == 1){
			glPushMatrix();
				glTranslatef(Bricks[i].x, Bricks[i].y, -4);
				glCallList(BrickList);
			glPopMatrix();
		}
		
	}
	
	//Draw the Ball//////////////////////////////////////////////////////
	glPushMatrix();
		glColor3f(0.7, 0.7, 0.7);
		glTranslatef(Ball.x, Ball.y, Ball.z);
		if (!gg) { glutSolidSphere(0.4, 50, 50); }
	glPopMatrix();

	glPushMatrix();
	glTranslatef(Paddle.x, Paddle.y, 0);
	glCallList(PaddleList);
	glPopMatrix();

	if (test == 1) {
		Ball.rst();
	}
	// draw some gratuitous text that just rotates on top of the scene:

	glDisable( GL_DEPTH_TEST );
	glColor3f( 0., 1., 1. );
	//DoRasterString( 0., 1., 0., "Text that moves" );


	// draw some gratuitous text that is fixed on the screen:
	//
	// the projection matrix is reset to define a scene whose
	// world coordinate system goes from 0-100 in each axis
	//
	// this is called "percent units", and is just a convenience
	//
	// the modelview matrix is reset to identity as we don't
	// want to transform these coordinates

	glDisable( GL_DEPTH_TEST );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	gluOrtho2D( 0., 100.,     0., 100. );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );
	glColor3f( 1., 1., 1. );
	if (gg) { DoRasterString(5., 5., 0., "GAME OVER"); }
	DoRasterString(70., 5., 0., "SCORE: ");

	
	DoRasterString(90., 5., 0., "0");
	



	for(int i = 0; i < Lives; i++){
	
		glPushMatrix();
			glTranslatef(5+5*i, 5, 3);
			glutSolidSphere( 4, 20, 20);
		glPopMatrix();
	}
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
	WhichColor = id - RED;

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


void
DoDepthBufferMenu( int id )
{
	DepthBufferOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthFightingMenu( int id )
{
	DepthFightingOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthMenu( int id )
{
	DepthCueOn = id;

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
	WhichProjection = id;

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
	glutSetWindow( MainWindow );

	int numColors = sizeof( Colors ) / ( 3*sizeof(int) );
	int colormenu = glutCreateMenu( DoColorMenu );
	for( int i = 0; i < numColors; i++ )
	{
		glutAddMenuEntry( ColorNames[i], i );
	}

	int axesmenu = glutCreateMenu( DoAxesMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthcuemenu = glutCreateMenu( DoDepthMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthbuffermenu = glutCreateMenu( DoDepthBufferMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthfightingmenu = glutCreateMenu( DoDepthFightingMenu );
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
	glutAddSubMenu(   "Colors",        colormenu);
	//glutAddSubMenu(   "Depth Buffer",  depthbuffermenu);
	glutAddSubMenu(   "Depth Fighting",depthfightingmenu);
	glutAddSubMenu(   "Depth Cue",     depthcuemenu);
	glutAddSubMenu(   "Projection",    projmenu );
	glutAddMenuEntry( "Reset",         RESET );
	glutAddSubMenu(   "Debug",         debugmenu);
	glutAddMenuEntry( "Quit",          QUIT );

// attach the pop-up menu to the right mouse button:

	glutAttachMenu( GLUT_RIGHT_BUTTON );
}



// initialize the glut and OpenGL libraries:
//	also setup display lists and callback functions

void
InitGraphics( )
{
	// request the display modes:
	// ask for red-green-blue-alpha color, double-buffering, and z-buffering:

	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );

	// set the initial window configuration:

	glutInitWindowPosition( 0, 0 );
	glutInitWindowSize( INIT_WINDOW_SIZE, INIT_WINDOW_SIZE );

	// open the window and set its title:

	MainWindow = glutCreateWindow( WINDOWTITLE );
	glutSetWindowTitle( WINDOWTITLE );

	// set the framebuffer clear values:

	glClearColor( BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3] );

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

	glutSetWindow( MainWindow );
	glutDisplayFunc( Display );
	glutReshapeFunc( Resize );
	glutKeyboardFunc( Keyboard );
	glutKeyboardUpFunc(KeyboardUp);
	glutMouseFunc( MouseButton );
	glutMotionFunc( MouseMotion );
	glutPassiveMotionFunc( NULL );
	glutVisibilityFunc( Visibility );
	glutEntryFunc( NULL );
	glutSpecialFunc( NULL );
	glutSpaceballMotionFunc( NULL );
	glutSpaceballRotateFunc( NULL );
	glutSpaceballButtonFunc( NULL );
	glutButtonBoxFunc( NULL );
	glutDialsFunc( NULL );
	glutTabletMotionFunc( NULL );
	glutTabletButtonFunc( NULL );
	glutMenuStateFunc( NULL );
	glutTimerFunc( -1, NULL, 0 );
	glutIdleFunc( Animate );

	// init glew (a window must be open to do this):

#ifdef WIN32
	GLenum err = glewInit( );
	if( err != GLEW_OK )
	{
		fprintf( stderr, "glewInit Error\n" );
	}
	else
		fprintf( stderr, "GLEW initialized OK\n" );
	fprintf( stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

}


// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )

void
InitLists( )
{
	glutSetWindow( MainWindow );

	// create the object:
	BrickList = glGenLists(1);
	glNewList(BrickList, GL_COMPILE);
		glColor4f(0.5, 0.2, 0.2, 1);
		glBegin(GL_QUAD_STRIP);
			glVertex3f(-0.5, -0.25, -0.25);
			glVertex3f(0.5, -0.25, -0.25);
			glVertex3f(-0.5, -0.25, 0.25);
			glVertex3f(0.5, -0.25, 0.25);
			glVertex3f(-0.5, 0.25, 0.25);
			glVertex3f(0.5, 0.25, 0.25);
			glVertex3f(-0.5, 0.25, -0.25);
			glVertex3f(0.5, 0.25, -0.25);
			glVertex3f(-0.5, -0.25, -0.25);
			glVertex3f(0.5, -0.25, -0.25);
		glEnd();
		glBegin(GL_QUADS);
			glVertex3f(-0.5, -0.25, -0.25);
			glVertex3f(-0.5, -0.25, 0.25);
			glVertex3f(-0.5, 0.25, 0.25);
			glVertex3f(-0.5, 0.25, -0.25);

			glVertex3f(0.5, -0.25, -0.25);
			glVertex3f(0.5, -0.25, 0.25);
			glVertex3f(0.5, 0.25, 0.25);
			glVertex3f(0.5, 0.25, -0.25);
		glEnd();
	glEndList();

	PaddleList = glGenLists(1);
	glNewList(PaddleList, GL_COMPILE);
		glColor4f(0, 0.6, 0, 0.1);
		glBegin(GL_TRIANGLE_STRIP);
			glVertex3f(-0.5, 0.5, 0);
			glVertex3f(-0.4, 0.4, 0);
			glVertex3f(0.5, 0.5, 0);
			glVertex3f(0.4, 0.4, 0);

			glVertex3f(0.5, -0.5, 0);
			glVertex3f(0.4, -0.4, 0);
			glVertex3f(-0.5, -0.5, 0);
			glVertex3f(-0.4, -0.4, 0);

			glVertex3f(-0.5, 0.5, 0);
			glVertex3f(-0.4, 0.4, 0);
		glEnd();
	glEndList();

	WallList = glGenLists(1);
		glNewList(WallList, GL_COMPILE);

		glColor4f(0.2, 0.2, 0.2, 1);

		glBegin(GL_QUAD_STRIP);
		glVertex3f(-5, -5, -0.5);
		glVertex3f(5, -5, -0.5);
		glVertex3f(-5, -5, 0.5);
		glVertex3f(5, -5, 0.5);
		glVertex3f(-5, 5, 0.5);
		glVertex3f(5, 5, 0.5);
		glVertex3f(-5, 5, -0.5);
		glVertex3f(5, 5, -0.5);
		glVertex3f(-5, -5, -0.5);
		glVertex3f(5, -5, -0.5);
		glEnd();

		

	glEndList();



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
		case 'o':
		case 'O':
			WhichProjection = ORTHO;
			break;

		case 'p':
		case 'P':
			WhichProjection = PERSP;
			break;

		case 'w':
		case 'W':
			up = 1;
			down = 0;
			break;

		case 'a':
		case 'A':
			left = 1;
			right = 0;
			break;

		case 's':
		case 'S':
			down = 1;
			up = 0;
			break;

		case 'd':
		case 'D':
			right = 1;
			left = 0;
			break;

		case ' ':
			Ball.bgn();
			break;

		
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

void
KeyboardUp(unsigned char c, int x, int y) {
	if (DebugOn != 0)
		fprintf(stderr, "KeyboardUp: '%c' (0x%0x)\n", c, c);

	switch (c)
	{
	case 'w':
	case 'W':
		up = 0;
		break;

	case 'a':
	case 'A':
		left = 0;
		break;

	case 's':
	case 'S':
		down = 0;
		break;

	case 'd':
	case 'D':
		right = 0;
		break;

	default:
		fprintf(stderr, "Don't know what to do with keyboard release: '%c' (0x%0x)\n", c, c);
	}

	// force a call to Display( ):

	glutSetWindow(MainWindow);
	glutPostRedisplay();
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
}


// called when the mouse moves while a button is down:

void
MouseMotion( int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "MouseMotion: %d, %d\n", x, y );


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
	DepthBufferOn = 1;
	DepthFightingOn = 0;
	DepthCueOn = 0;
	Scale  = 1.0;
	WhichColor = WHITE;
	WhichProjection = PERSP;
	Xrot = Yrot = 0.;
}


// called when user resizes the window:

void
Resize( int width, int height )
{
	if( DebugOn != 0 )
		fprintf( stderr, "ReSize: %d, %d\n", width, height );

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

static float xx[ ] = {
		0.f, 1.f, 0.f, 1.f
	      };

static float xy[ ] = {
		-.5f, .5f, .5f, -.5f
	      };

static int xorder[ ] = {
		1, 2, -3, 4
		};

static float yx[ ] = {
		0.f, 0.f, -.5f, .5f
	      };

static float yy[ ] = {
		0.f, .6f, 1.f, 1.f
	      };

static int yorder[ ] = {
		1, 2, 3, -2, 4
		};

static float zx[ ] = {
		1.f, 0.f, 1.f, 0.f, .25f, .75f
	      };

static float zy[ ] = {
		.5f, .5f, -.5f, -.5f, 0.f, 0.f
	      };

static int zorder[ ] = {
		1, 2, 3, 4, -5, 6
		};

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
	
	float i = floor( h );
	float f = h - i;
	float p = v * ( 1.f - s );
	float q = v * ( 1.f - s*f );
	float t = v * ( 1.f - ( s * (1.f-f) ) );

	float r, g, b;			// red, green, blue
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


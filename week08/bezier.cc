#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>

#if !defined(GLUT_WHEEL_UP)
# define GLUT_WHEEL_UP 3
# define GLUT_WHEEL_DOWN 4
#endif

bool leftButton = false;
int downX, downY;
float sphi=0, stheta=0;
GLfloat xOffset, yOffset;
float zoomLevel = 1;



/*
* Having an error that if you zoom out when you click it will then auto rescale back to how it was initally
*Also having an error that resizing the screen isnt resizing the curve and 
* whilst the coordinates are visibly in the right positions clicking that position isnt 
recongised
*
*/




class wcPt3D
{
	public:
		GLfloat x, y, z;
		wcPt3D()
		{
			x = 0.0;
			y = 0.0;
			z = 0.0;
		};
		wcPt3D(GLfloat xx, GLfloat yy, GLfloat zz)
		{
			x = xx;
			y = yy;
			z = zz;
		};
};




/*  Set initial size of the display window.  */
GLsizei winWidth = 600, winHeight = 600;  

/*  Set size of world-coordinate clipping window.  */
GLfloat xwcMin = -50.0, xwcMax = 50.0;
GLfloat ywcMin = -50.0, ywcMax = 50.0;

//int downX, downY;
bool lineClicked = false;
bool pointClicked = false;
bool pointAdded = false;
wcPt3D* clickedPoint = NULL;



GLint nCtrlPts = 6, nBezCurvePts = 1000;
wcPt3D* ctrlPts;


void initPoints(void)
{
	ctrlPts = (wcPt3D*)malloc(nCtrlPts * sizeof(wcPt3D));
	ctrlPts[0] = wcPt3D(-40, -40, 0);
	ctrlPts[1] = wcPt3D(-10, 100, 0);
	ctrlPts[2] = wcPt3D(10, -100, 0);
	ctrlPts[3] = wcPt3D(100, -100, 0);
	ctrlPts[4] = wcPt3D(100, 100, 0);
	ctrlPts[5] = wcPt3D(40, 40, 0);
}

void addPoint(wcPt3D point)
{
	nCtrlPts++;
	ctrlPts = (wcPt3D*)realloc(ctrlPts, nCtrlPts * sizeof(wcPt3D));
	ctrlPts[nCtrlPts - 1] = point;
}

void clippingWindow(void) {
	float biggestX = ctrlPts[0].x;
	float biggestY = ctrlPts[0].y;
	float smallestX = ctrlPts[0].x;
	float smallestY = ctrlPts[0].y;
	
	
	for (int i = 0; i < 6; i++) {
		if (ctrlPts[i].x > biggestX) {
			biggestX = ctrlPts[i].x;
		} else if (ctrlPts[i].x < smallestX) {
			smallestX = ctrlPts[i].x;
		}
		if (ctrlPts[i].y > biggestY) {
			biggestY = ctrlPts[i].y;
		}else if (ctrlPts[i].y < smallestY) {
			smallestY = ctrlPts[i].y;
		}

	}
	
	xwcMin = smallestX;
	xwcMax = biggestX;
	ywcMin = smallestY;
	ywcMax = biggestY;
		
	
}






void init (void)
{
   /*  Set color of display window to white.  */
   glClearColor (1.0, 1.0, 1.0, 0.0);
   
   initPoints();
	clippingWindow();
   
}

void plotPoint (wcPt3D bezCurvePt)
{
    glBegin (GL_POINTS);
        glVertex2f (bezCurvePt.x, bezCurvePt.y);
    glEnd ( );
}

/*  Compute binomial coefficients C for given value of n.  */
void binomialCoeffs (GLint n, GLint * C)
{
   GLint k, j;

   for (k = 0;  k <= n;  k++) {
      /*  Compute n!/(k!(n - k)!).  */
      C [k] = 1;
      for (j = n;  j >= k + 1;  j--)
        C [k] *= j;
      for (j = n - k;  j >= 2;  j--)
        C [k] /= j;
   }
}

void computeBezPt (GLfloat t, wcPt3D * bezPt, GLint nCtrlPts, wcPt3D * ctrlPts, GLint * C)
{
   GLint k, n = nCtrlPts - 1;
   GLfloat bezBlendFcn;

   bezPt->x = bezPt->y = bezPt->z = 0.0;

   /*  Compute blending functions and blend control points. */
   for (k = 0; k < nCtrlPts; k++) {
      bezBlendFcn = C [k] * pow (t, k) * pow (1 - t, n - k);
      bezPt->x += ctrlPts [k].x * bezBlendFcn;
      bezPt->y += ctrlPts [k].y * bezBlendFcn;
      bezPt->z += ctrlPts [k].z * bezBlendFcn;
   }
}

void bezier(wcPt3D* ctrlPts, GLint nCtrlPts, GLint nBezCurvePts)
{
	wcPt3D bezCurvePt;
	GLfloat t;
	GLint* C;

	/*  Allocate space for binomial coefficients  */
	C = new GLint[nCtrlPts];

	binomialCoeffs(nCtrlPts - 1, C);

	for (int i = 0; i <= nBezCurvePts; i++)
	{
		t = GLfloat(i) / GLfloat(nBezCurvePts);
		computeBezPt(t, &bezCurvePt, nCtrlPts, ctrlPts, C);
		plotPoint(bezCurvePt);
	}

	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINE_STRIP);

	for (int i = 0; i < nCtrlPts; i++)
	{
		glVertex2f(ctrlPts[i].x, ctrlPts[i].y);
	}

	glEnd();

	glColor3f(0.0, 0.0, 1.0);

	for (int i = 0; i < nCtrlPts; i++)
	{
		plotPoint(ctrlPts[i]);
	}

	delete[] C;
}

void displayFcn (void)
{
   glClear (GL_COLOR_BUFFER_BIT);   //  Clear display window.

   //  Set example number of control points and number of
    //  curve positions to be plotted along the Bezier curve.
    //
  
	/*
   glPointSize (8);
   glColor3f (1.0, 0.0, 0.0);      //  Set point color to red.
  glTranslatef(sphi, stheta, 0.0);
   bezier (ctrlPts, nCtrlPts, nBezCurvePts);
   glutSwapBuffers();
   */
   
   GLint nBezCurvePts = 1000;

	glPointSize(4);
	glColor3f(1.0, 0.0, 0.0);      //  Set point color to red.

	bezier(ctrlPts, nCtrlPts, nBezCurvePts);
	glutSwapBuffers();
}

void winReshapeFcn (GLint newWidth, GLint newHeight)
{
   /*  Maintain an aspect ratio of 1.0.  */
   glViewport (0, 0, newHeight, newHeight );

   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ( );

   gluOrtho2D(xwcMin , xwcMax , ywcMin , ywcMax );
   
   glutPostRedisplay();
}




wcPt3D scale(int x, int y)
{
	wcPt3D point;

	// bug, pH <16:46:57; Mon,  7 Apr 2014>
	//point.x = ((float)x / winWidth * (xwcMax - xwcMin)) - xwcMax - xOffset;
	//point.y = ((float)(winHeight - y) / winHeight * ywcMax * 2.0) - ywcMax - yOffset;
	point.x = ((float)x / winWidth * (xwcMax - xwcMin)) + xwcMin - xOffset;
	point.y = ((float)(winHeight - y) / winHeight * (ywcMax - ywcMin)) + ywcMin - yOffset;

	return point;
}



void mouse(int btn, int state, int x, int y)
{
	lineClicked = false;
	pointClicked = false;
	pointAdded = false;
	clickedPoint = NULL;
	
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		wcPt3D scaledPoint = scale(x, y);

		for (int i = 0; i < nCtrlPts; i++)
		{
			if (fabs(ctrlPts[i].x - scaledPoint.x) < 10.0 && fabs(ctrlPts[i].y - scaledPoint.y) < 10.0)
			{
				pointClicked = true;
				clickedPoint = ctrlPts + i;
			}
		}

		lineClicked = pointClicked ? false : true;
	}

	if(glutGetModifiers() && GLUT_ACTIVE_CTRL){
		if(btn==GLUT_WHEEL_UP ) {
			glScalef(1.1, 1.1, 0.0);  //ZOOM IN 
			zoomLevel = zoomLevel * 1.1;
			glutPostRedisplay();
		}
		if(btn==GLUT_WHEEL_DOWN && GLUT_ACTIVE_CTRL ) {
			glScalef(0.9, 0.9, 0.0);
			zoomLevel = zoomLevel / 1.1;
			glutPostRedisplay();
		}		//ZOOM OUT
	}
	
	
	downX = x; 
	downY = y;
	leftButton = ((btn == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN));
}



void dragFcn(int x, int y)
{
	

	if (lineClicked)
	{
		xOffset += (x - downX) * zoomLevel / 2.0;
		yOffset += (downY - y) * zoomLevel / 2.0;
		downX = x;
		downY = y;
	}
	else if (pointClicked && clickedPoint != NULL)
	{
		*clickedPoint = scale(x, y);
	}

	winReshapeFcn(winWidth, winHeight); // cannot use glutPostRedisplay() - gluOrtho2D needs to be computed with the new values
}




void MotionCallback(int x, int y)
{
  if (leftButton){sphi+=(float)(x-downX)/8;stheta+=(float)(downY-y)/8;} // rotate
  
  
  
  
  downX = x;   downY = y; 
  glutPostRedisplay();
}








int main (int argc, char** argv)
{

   glutInit (&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
   glutInitWindowPosition (50, 50);
   glutInitWindowSize (winWidth, winHeight);
   glutCreateWindow ("Bezier Curve");
   //glutKeyboardFunc(keyPress);
    //glutMotionFunc(MotionCallback);
	glutMotionFunc(dragFcn);
   //glutGetModifiers(mouse);
   glutMouseFunc(mouse);
   
   
   

   init ( );
   glutDisplayFunc (displayFcn);
   glutReshapeFunc (winReshapeFcn);
 //  glutMouseFunc(mouse);
   glutMainLoop ( );
}

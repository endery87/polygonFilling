///////////////////////////////////////////////////////////////
//// CMPE - 460 PROJECT #1 : POLYGON FILLING ALGORITHM	///////
//// Student :	ENDER YUNUS								///////
//// No :		2004101303								///////
///////////////////////////////////////////////////////////////


#include <windows.h>
#include <glut.h>
#include <iostream>
#include <stdio.h>
#include <string>
using namespace std;
#include <math.h>
#define WINDOW_HEIGHT 500				//height of the window is fixed


///////////////////////////////// SCANLINE-FILL ALGORITHM//////////////////////////////////////////////

struct GLintPoint{
	int x;
	int y;
};

typedef struct tEdge {
	int yUpper;
	float xIntersect, dxPerScan;
	struct tEdge * next;
} Edge;

typedef struct tdcPt {
	int x;
	int y;
} dcPt;

static tdcPt Corners[50];


void scanFill (int cnt, dcPt * pts);
void buildEdgeList (int cnt, dcPt * pts, Edge * edges[]);
int yNext (int k, int cnt, dcPt * pts);
void makeEdgeRec(dcPt lower, dcPt upper, int yComp, Edge * edge, Edge * edges[]);
void insertEdge (Edge * list, Edge * edge) ;
void buildActiveList (int scan, Edge * active, Edge * edges[]);
void fillScan (int scan, Edge * active) ;
void updateActiveList (int scan, Edge * active);
void deleteAfter (Edge * q);
void resortActiveList (Edge * active);



// 3 Channel Pixel Buffer

GLubyte PixelBuffer[2000 * 2000 *3];

// Width & Heigth of Pixel Rectangle (image)

int		imagew = 200;
int		imageh = 200;
int		static NumberOfCorners=0;


// Size of GLUT window

int		windowX = 500;
int		windowY = 500;


// Fill in the Pixel Buffer



void scanFill (int cnt, dcPt * pts) {
	Edge * edges[WINDOW_HEIGHT], * active;
	int i, scan;
	for (i=0; i<WINDOW_HEIGHT; i++){
		edges[i] = (Edge *) malloc (sizeof (Edge));
		edges[i]->next = NULL;
	}
	buildEdgeList (cnt, pts, edges);
	active = (Edge *) malloc (sizeof (Edge));
	active->next = NULL;
	for (scan=0; scan<WINDOW_HEIGHT; scan++) {
		buildActiveList (scan, active, edges);
		if (active->next) {
			fillScan (scan, active);
			updateActiveList (scan, active);
			resortActiveList (active);
		}
	}
/* Free edge records that have been malloc’ed ... */
}

void buildEdgeList (int cnt, dcPt * pts, Edge * edges[]) {
	Edge * edge;
	dcPt v1, v2;
	int i, yPrev = pts[cnt - 2].y;
	v1.x = pts[cnt-1].x; v1.y = pts[cnt-1].y;
	for (i=0; i<cnt; i++){
		v2 = pts[i];
		if (v1.y != v2.y){ /* nonhorizontal line */
			edge = (Edge *) malloc (sizeof (Edge));
			if (v1.y < v2.y) /* up-going edge */
				makeEdgeRec (v1, v2, yNext (i, cnt, pts), edge, edges);
			else /* down-going edge */
				makeEdgeRec (v2, v1, yPrev, edge, edges);
		}
		yPrev = v1.y;

		v1 = v2;
	}
}

/* For an index, return y-coordinate of next nonhorizontal line */
int yNext (int k, int cnt, dcPt * pts) {
	int j;
	if ((k+1) > (cnt-1))
		j = 0;
	else
		j = k + 1;
	while (pts[k].y == pts[j].y)
		if ((j+1) > (cnt-1))
			j = 0;
		else
			j++;
	return (pts[j].y);
}


		/* Store lower-y coordinate and inverse slope for each edge. Adjust
		and store upper-y coordinate for edges that are the lower member
		of a monotically increasing or decreasing pair of edges */
void makeEdgeRec(dcPt lower, dcPt upper, int yComp, Edge * edge, Edge * edges[]){
	edge->dxPerScan =(float) (upper.x - lower.x) / (upper.y - lower.y);
	edge->xIntersect = lower.x;
	if (upper.y < yComp)
		edge->yUpper = upper.y - 1;
	else
		edge->yUpper = upper.y;
	insertEdge (edges[lower.y], edge);
}

/* Inserts edge into list in order of increasing xIntersect field. */
void insertEdge (Edge * list, Edge * edge) {
	Edge * p, * q = list;
	p = q->next;
	while (p != NULL) {
		if (edge->xIntersect < p->xIntersect)
			p = NULL;
		else {
			q = p;
			p = p->next;
		}
	}
	edge->next = q->next;
	q->next = edge;
}

void buildActiveList (int scan, Edge * active, Edge * edges[]){
	Edge * p, * q;
	p = edges[scan]->next;
	while (p) {
		q = p->next;
		insertEdge (active, p);
		p = q;
	}
}

void fillScan (int scan, Edge * active) {
	Edge * p1, * p2;
	int i;
	p1 = active->next;
	while (p1) {
		p2 = p1->next;
		for (i=p1->xIntersect; i<p2->xIntersect; i++)
		{
			glRasterPos2i(i-2,scan+2);
			glDrawPixels(4, 4, GL_BGR_EXT, GL_UNSIGNED_BYTE, PixelBuffer);
			//setPixel ((int) i, scan);
		}
		p1 = p2->next;
	}
}


/* Delete completed edges. Update ’xIntersect’ field for others */
void updateActiveList (int scan, Edge * active) {
	Edge * q = active, * p = active->next;
	while (p){
		if (scan >= p->yUpper) {
			p = p->next;
			deleteAfter (q);
		} else {
			p->xIntersect = p->xIntersect + p->dxPerScan;
			q = p;
			p = p->next;
		}
	}
}

void deleteAfter (Edge * q) {
	Edge * p = q->next;
	q->next = p->next;
	free (p);
}

void resortActiveList (Edge * active) {
	Edge * q, * p = active->next;
	active->next = NULL;
	while (p) {
		q = p->next;
		insertEdge (active, p);
		p = q;
	}
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void myMouse(int button, int state, int x, int y)
{
	  
      if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)///if the left button is clicked a point is chosen
      {
         glRasterPos2i(x-1,y-1);
		 Corners[NumberOfCorners].x=x-1;
		 Corners[NumberOfCorners].y=y-1;
		 glDrawPixels(3,3,GL_BGR_EXT, GL_UNSIGNED_BYTE, PixelBuffer);
		 NumberOfCorners++;

      }
      else if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) /// if the right button is clicked scanfilling is done
	  {
			scanFill (NumberOfCorners, Corners);
			NumberOfCorners=0;
	  }
      glFlush();
}
void myKeyboard(unsigned char theKey, int mouseX, int mouseY)
{
       
	switch(theKey)
	{
		case 'n':									//if the 'n' button of the keyboard is clicked
			glClear(GL_COLOR_BUFFER_BIT);			//the window is cleared
			glFlush();
			NumberOfCorners=0;
			break;
	}
}
// Initialize GL Stuff

void init(){
	glShadeModel(GL_FLAT);								// Enable Smooth Shading
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);				// White Background

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 500.0, 500.0, 0.0, -1.0, 1.0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  
}

// Display Function

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glutMouseFunc(myMouse);
	glutKeyboardFunc(myKeyboard);
	glFlush();
}


	

void main(int argc, char **argv)
{       
        glutInit(&argc,argv);
        glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
        glutInitWindowPosition(0,0);
        glutInitWindowSize(windowX,windowY);		//window for polygon drawing
		glutCreateWindow("CMPE460-PROJECT#1");        
        init();
        glutDisplayFunc(display);			
        glutMainLoop();
}
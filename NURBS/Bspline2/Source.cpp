#include <windows.h>
#include "myNurbs.h"

bool showmodel = true;
bool showsurface = false;

float x[] = { 17,17,-17,-17,-17,17,17,
10,10,-10,-10,-10,10,10,
15,15,-15,-15,-15,15,15,
30,30,-30,-30,-30,30,30,
20,20,-20,-20,-20,20,20,
10,10,-10,-10,-10,10, 10,
0,0,0,0,0,0,0
 };

float x1[] = { 15,15,-15,-15,-15,15,15,
30,30,-30,-30,-30,30,30,
20,20,-20,-20,-20,20,20,
10,10,-10,-10,-10,10,10,
0,0,0,0,0,0,0 };

float y[] = { 100,100,100,100,100,100,100,
90,90,90,90,90,90,90,
60,60,60,60,60,60,60,
30,30,30,30,30,30,30,
10,10,10,10,10,10,10,
0,0,0,0,0,0,0,
0,0,0,0,0,0,0
};

float y2[] = { 60,60,60,60,60,60,60,
30,30,30,30,30,30,30,
10,10,10,10,10,10,10,
0,0,0,0,0,0,0,
0,0,0,0,0,0,0 };

float z[] = { 60,77,77,60,43,43,60,
60,70,70,60,50,50,60,
60,75,75,60,45,45,60,
60,90,90,60,30,30,60,
60,80,80,60,40,40,60,
60,70,70,60,50,50,60,
60,60,60,60,60,60,60
 };

float z1[] = { 60,75,75,60,45,45,60,
60,90,90,60,30,30,60,
60,80,80,60,40,40,60,
60,70,70,60,50,50,60,
60,60,60,60,60,60,60 };

float w[] = { 1,0.5,0.5,1,0.5,0.5,1,
1,0.5,0.5,1,0.5,0.5,1,
1,0.5,0.5,1,0.5,0.5,1,
1,0.5,0.5,1,0.5,0.5,1,
1,0.5,0.5,1,0.5,0.5,1,
1,0.5,0.5,1,0.5,0.5,1,
1,0.5,0.5,1,0.5,0.5,1
};

float w1[] = { 1,0.5,0.5,1,0.5,0.5,1,
1,0.5,0.5,1,0.5,0.5,1,
1,0.5,0.5,1,0.5,0.5,1,
1,0.5,0.5,1,0.5,0.5,1,
1,0.5,0.5,1,0.5,0.5,1 };

float u[] = { 0,0,0,0.25,0.5,0.5,0.75,1,1,1 };
float v[] = { 0,0,0,0.1,0.4,0.6,0.95,1,1,1 };
float suv[161][161][3];
float r = 0.0f;
float dx = 1.0f;
float mx = 0.0f;

void lightm()
{
	GLfloat lamb[4] = { 0.35f,0.35f,0.35f,1.0f };
	GLfloat ldif[4] = { 0.35f,0.35f,0.35f,1.0f };
	GLfloat lspe[4] = { 0.55f,0.55f,0.55f,1.0f };
	GLfloat lpos[4] = { 200.0f,200.0f,60.0f,1.0f };

	GLfloat mamb[4] = { 0.5f,0.5f,0.5f,1.0f };
	GLfloat mdif[4] = { 0.5f,0.5f,0.5f,1.0f };
	GLfloat mspe[4] = { 0.7f,0.7f,0.7f,1.0f };
	GLfloat memi[4] = { 0.0f,0.0f,0.0f,1.0f };
	GLfloat mshininess = 128.0f;

	glLightfv(GL_LIGHT1, GL_AMBIENT, lamb);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, ldif);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lspe);
	glLightfv(GL_LIGHT1, GL_POSITION, lpos);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mamb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mdif);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mspe);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, memi);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mshininess);
}

void TimerFunction(int value)
{
	if (r<360.f)
		r += 1.0f;
	else
		r = 0.0f;
	mx += dx;
	if (mx>50)
		dx = -dx;
	if (mx<-100)
		dx = -dx;
	glutPostRedisplay();
	glutTimerFunc(33, TimerFunction, 1);
}

void RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glColor3f(1.0f, 1.0f, 1.0f);
	glTranslatef(10, -60, -90);
	glRotatef(-30, 1.0, 1.0, 1.0);
//	glRotatef(-2 * r, 0.0, 1.0, 0.0);
//	glRotatef(-r, 1.0, 1.0, 1.0);
	NurbsFace(2, 9, u, 10, 9, v, 10, x, y, z, w, suv);
	if (showsurface)
		ShowSurface(10, 10, suv, 0);
	if(showmodel)
		ShowSurface(10, 10, suv, 1);
	//NurbsFace(2, 9, u, 160, 9, v, 160, x, y, z, w, suv);
	//ShowSurface(160, 160, suv, 1);
	glPopMatrix();
	glutSwapBuffers();
}
void SetupRC()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	lightm();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0);
	glEnable(GL_LIGHT1);
	glEnable(GL_COLOR_MATERIAL);
}

void WindowSize(GLsizei w, GLsizei h)
{
	GLfloat aspectRatio;
	GLfloat tmb = 110.0;

	if (h == 0)
		h = 1;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	aspectRatio = (GLfloat)w / (GLfloat)h;
	if (w <= h)
		glOrtho(-tmb, tmb, -tmb / aspectRatio, tmb / aspectRatio, 10 * tmb, -10 * tmb);
	else
		glOrtho(-tmb*aspectRatio, tmb*aspectRatio, -tmb, tmb, 10 * tmb, -10 * tmb);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 's':
		showsurface = true;
		showmodel = false;
		break;
	case 'm':
		showmodel = true;
		showsurface = false;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(1024, 768);
	glutCreateWindow("B-NURBS");
	glutDisplayFunc(RenderScene);
	glutReshapeFunc(WindowSize);
	glutTimerFunc(33, TimerFunction, 1);
	glutKeyboardFunc(keyboard);
	SetupRC();
	glutMainLoop();
	return 0;
}

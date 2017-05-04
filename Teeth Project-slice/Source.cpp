/***********left-up: curve1***********/
/***********right-up: curve2***********/
/***********right-bottom: curve3***********/
/***********left-up: curve1***********/
#pragma warning(disable:4146)
#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>
#include "curve.h"
#include "viewport.h"

typedef struct finalcurve
{
	CubicBezierCurve cur[4];
	GLfloat z, xmin, xmax, ymin, ymax;
	int time = 1;
}finalcurve;

/* global */
finalcurve finalcur[9];
int edit_ctrlpts_idx[4] = { -1,-1,-1,-1 };
int mode = 0;

/*load model parameters*/
int numVertex = 0;
int numFaces = 0;/*texture face's number is the same*/

float(*vertices)[3] = NULL;
int(*faces)[3] = NULL;

GLfloat x_max, xmaxy, x_min, xminy, y_max, ymaxx, y_min, yminx, z;
Point3 cs[4],cm[4],ce[4],ctrlp_2[4], ctrlp_3[4];
GLsizei width = 800, height = 600;
float viewportwidth = 400, viewportheight = 300;

Vector3d eye;
Vector3d center;
Vector3d upVector;
bool isDragging = false;
float radius;
int lastX = -1;
int lastY = -1;

void readtxt();
void DrawCruve(CubicBezierCurve curve, int x);

void Init()
{
	eye = Vector3d(750, 750, 750);
	center = Vector3d(0, 0, 0);
	upVector = Vector3d(0, 1, 0);

	//Set a sign
	for (int i = 0; i < 9; i++)
	{
		finalcur[i].cur[0].control_pts[0][0] = 0;
		finalcur[i].z = 0;
	}
	readtxt();
}

//Draw curve in 3D
void curvein3d()
{
	for (int i = 0; i < 9; i++)
	{
		if (finalcur[i].cur[0].control_pts[0][0] != 0)
		{
			glPushMatrix();
			glTranslatef(0, 0, finalcur[i].z);
			for (int j = 0; j < 4; j++)
			{
				DrawCruve(finalcur[i].cur[j], 1);
			}
			glPopMatrix();
		}
	}
}

int hit_index(CubicBezierCurve *curve, int x, int y)
{
	int i;
	for (i = 0; i<4; ++i) {
		REAL tx = curve->control_pts[i][0] - x;
		REAL ty = curve->control_pts[i][1] - y;
		if ((tx * tx + ty * ty) < 30)
			return i;
	}
	return -1;
}

void freedom()
{
	free((float *)vertices);
	free((int *)faces);

	numVertex = 0;
	numFaces = 0;/*texture face's number is the same*/
}

void readtxt()
{
	char line[256];//get line
	FILE* fp = NULL;
	//load model
	if (mode == 0)
	{
		printf("slice 0 has been chosen\n");
		fp = fopen("2_17_slice0.txt", "rb");
	}
	else if (mode == 1)
	{
		printf("slice 1 has been chosen\n");
		fp = fopen("2_17_slice1.txt", "rb");
	}
	else if (mode == 2)
	{
		printf("slice 1 has been chosen\n");
		fp = fopen("2_17_slice2.txt", "rb");
	}
	else if (mode == 3)
	{
		printf("slice 1 has been chosen\n");
		fp = fopen("2_17_slice3.txt", "rb");
	}
	else if (mode == 4)
	{
		printf("slice 1 has been chosen\n");
		fp = fopen("2_17_slice4.txt", "rb");
	}
	else if (mode == 5)
	{
		printf("slice 1 has been chosen\n");
		fp = fopen("2_17_slice5.txt", "rb");
	}
	else if (mode == 6)
	{
		printf("slice 1 has been chosen\n");
		fp = fopen("2_17_slice6.txt", "rb");
	}
	else if (mode == 7)
	{
		printf("slice 1 has been chosen\n");
		fp = fopen("2_17_slice7.txt", "rb");
	}
	else if (mode == 8)
	{
		printf("slice 1 has been chosen\n");
		fp = fopen("2_17_slice8.txt", "rb");
	}

	if (fp == NULL)
	{
		printf("%s file can not open", "slice.txt");
		exit(1);
	}

	while (!feof(fp))
	{
		fgets(line, 256, fp);
		if (line[0] == 'v')
		{
			if (line[1] == ' ')   //vertex:v ***
				numVertex++;
		}
		else if (line[0] == 'f') {	//face:f ***
			numFaces++;
		}
	}
	//back to start point of file
	rewind(fp);

	printf("number of vertices : %d\n", numVertex);
	printf("number of faces : %d\n", numFaces);

	vertices = (float(*)[3])malloc(sizeof(float) * 3 * numVertex);		//정점 
	faces = (int(*)[3])malloc(sizeof(int) * 3 * numFaces);				//면 (정점)

	int j = 0, t = 0, n = 0, k = 0;
	int IdxFace = 0;

	while (!feof(fp))
	{
		fgets(line, 256, fp);
		if (line[0] == 'v')
		{
			if (line[1] == ' ')
			{
				/* v */
				fseek(fp, -(strlen(line) + 1), SEEK_CUR);		//한줄 앞으로

				float x, y, z;
				fscanf(fp, "%s %f %f %f", line, &x, &y, &z);

				//Scalef
				vertices[j][0] = (x + 22) * 30 + 0.5* viewportwidth;	vertices[j][1] = (y - 78) * 30 + 0.5* viewportheight;	vertices[j][2] = (z - 38) * 30;
				j++;
			}
		}
		else if (line[0] == 'f')
		{
			fseek(fp, -(strlen(line) + 1), SEEK_CUR);		    //한줄 앞으로
																//printf("length of line is %d\n", strlen(line));

			int x1, x2, y1, y2, z1, z2;
			fscanf(fp, "%s %d//%d %d//%d %d//%d", line, &x1, &x2, &y1, &y2, &z1, &z2);

			//면의 정점                                                          순서 정보
			faces[IdxFace][0] = x1 - 1;
			faces[IdxFace][1] = y1 - 1;
			faces[IdxFace][2] = z1 - 1;
			IdxFace++;
		}
	}
	x_max = x_min = ymaxx = yminx = vertices[0][0];
	y_max = y_min = xmaxy = xminy = vertices[0][1];
	z = vertices[0][2];
//	printf("z is %f\n", z);

	for (int i = 1; i < numVertex; i++)
	{
		if (x_max < vertices[i][0])
		{
			x_max = vertices[i][0];
			xmaxy = vertices[i][1];
		}
		if (x_min > vertices[i][0])
		{
			x_min = vertices[i][0];
			xminy = vertices[i][1];
		}
		if (y_max < vertices[i][1])
		{
			y_max = vertices[i][1];
			ymaxx = vertices[i][0];
		}
		if (y_min > vertices[i][1])
		{
			y_min = vertices[i][1];
			yminx = vertices[i][0];
		}
	}
	//curve1
	cs[0][0] = x_min;
	cs[0][1] = xminy;

	cm[0][0] = x_min;
	cm[0][1] = y_max;

	ce[0][0] = ymaxx;
	ce[0][1] = y_max;
	//curve2
	cs[1][0] = ymaxx;
	cs[1][1] = y_max;

	cm[1][0] = x_max;
	cm[1][1] = y_max;

	ce[1][0] = x_max;
	ce[1][1] = xmaxy;
	//curve3
	cs[2][0] = x_max;
	cs[2][1] = xmaxy;

	cm[2][0] = x_max;
	cm[2][1] = y_min;

	ce[2][0] = yminx;
	ce[2][1] = y_min;
	//curve4
	cs[3][0] = yminx;
	cs[3][1] = y_min;

	cm[3][0] = x_min;
	cm[3][1] = y_min;

	ce[3][0] = x_min;
	ce[3][1] = xminy;

	for (int i = 0; i < 4; i++)
	{
		ctrlp_2[i][0] = 0.333*cs[i][0] + 0.666*cm[i][0];
		ctrlp_2[i][1] = 0.333*cs[i][1] + 0.666*cm[i][1];

		ctrlp_3[i][0] = 0.666*cm[i][0] + 0.333*ce[i][0];
		ctrlp_3[i][1] = 0.666*cm[i][1] + 0.333*ce[i][1];
	}

	if (finalcur[mode].time < 5)
	{
		for (int i = 0; i < 4; i++)
		{
			{
				SET_PT2(finalcur[mode].cur[i].control_pts[0], cs[i][0], cs[i][1]);
				SET_PT2(finalcur[mode].cur[i].control_pts[1], ctrlp_2[i][0], ctrlp_2[i][1]);
				SET_PT2(finalcur[mode].cur[i].control_pts[2], ctrlp_3[i][0], ctrlp_3[i][1]);
				SET_PT2(finalcur[mode].cur[i].control_pts[3], ce[i][0], ce[i][1]);
			}
			finalcur[mode].time++;
		}
		finalcur[mode].z = z;
		finalcur[mode].xmin = x_min;
		finalcur[mode].xmax = x_max;
		finalcur[mode].ymin = y_min;
		finalcur[mode].ymax = y_max;
	}
	fclose(fp);
}

void reshape_callback(GLint nw, GLint nh)
{
	width = nw;
	height = nh;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);
}

void DrawModel()
{
	glPushMatrix();
	//Draw boundary and mesh
	glColor3f(1.0, 1.0, 0.0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(x_min, y_max);
	glVertex2f(x_min, y_min);
	glVertex2f(x_max, y_min);
	glVertex2f(x_max, y_max);
	glEnd();

	for (int i = 0; i < numFaces - 1; i++)
	{
		float p1[3] = { vertices[faces[i][0]][X], vertices[faces[i][0]][Y] };
		float p2[3] = { vertices[faces[i][1]][X], vertices[faces[i][1]][Y] };
		float p3[3] = { vertices[faces[i][2]][X], vertices[faces[i][2]][Y] };

		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_TRIANGLES);
		glVertex2fv(p1);
		glVertex2fv(p2);
		glVertex2fv(p3);
		glEnd();

		glColor3f(0.0, 1.0, 0.0);
		glBegin(GL_LINE_LOOP);
		glVertex2fv(p1);
		glVertex2fv(p2);
		glVertex2fv(p3);
		glEnd();
	}
	glPopMatrix();
}

void DrawCruve(CubicBezierCurve curve, int x)
{
#define RES 100
	int i;
	glColor3f(0.2, 0.2, 0.2);
	glLineWidth(2.0);
	/* curve */
	glBegin(GL_LINE_STRIP);
	for (i = 0; i <= RES; ++i) {
		Point2 pt;
		const REAL t = (REAL)i / (REAL)RES;
		evaluate_curve(&curve, t, pt);
		glVertex2f(pt[0], pt[1]);
	}
	glEnd();
	glLineWidth(1.0);

	/* control pts */
	glColor3ub(0, 0, 255);
	glPointSize(10.0);

	if (x == 0)
	{
		glBegin(GL_POINTS);
		for (i = 0; i < 4; ++i) {
			REAL *pt = curve.control_pts[i];
			glVertex2f(pt[0], pt[1]);
		}
		glEnd();
	}
}

void display_callback(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor3f(0, 0, 0);
	glBegin(GL_LINES);
	glVertex3f(-1, 0, 0);
	glVertex3f(1, 0, 0);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(0, -1, 0);
	glVertex3f(0, 1, 0);
	glEnd();


	// XY
	glViewport(0, viewportheight, viewportwidth, viewportheight);
	glLoadIdentity();
	gluOrtho2D(0, (double)viewportwidth, 0, (double)viewportheight);
	glColor3ub(255, 255, 255);

	DrawModel();
	for (int i = 0; i < 4; i++)
		DrawCruve(finalcur[mode].cur[i], 0);

	// 3D
	glViewport(viewportwidth, viewportheight, viewportwidth, viewportheight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(25, width / (double)height, 0.1, 25000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye.x, eye.y, eye.z, center.x, center.y, center.z, upVector.x, upVector.y, upVector.z);

	glEnable(GL_DEPTH_TEST);
	glBegin(GL_LINES);
	glColor3f(1.0f, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(500.0f, 0, 0);
	glColor3f(0, 1.0f, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 500.0f, 0);
	glColor3f(0, 0, 1.0f);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 500.0f);
	glEnd();

	curvein3d();

	glDisable(GL_DEPTH_TEST);

	glutSwapBuffers();
}

void mouse_callback(GLint button, GLint action, GLint x, GLint y)
{
	Vector3d lastP = getMousePoint(lastX, lastY, viewportwidth, viewportheight, radius);
	Vector3d currentP = getMousePoint(x - viewportwidth, y, viewportwidth, viewportheight, radius);

	if (GLUT_LEFT_BUTTON == button) {
		switch (action) {
		case GLUT_DOWN:
			for (int i = 0; i < 4; i++)
			{
				//change finalcurve when curve changed
				edit_ctrlpts_idx[i] = hit_index(&finalcur[mode].cur[i], x, viewportheight - y);
			}
			break;

		case GLUT_UP:
			for (int i = 0; i<4; i++)
				edit_ctrlpts_idx[i] = -1;
			break;
		}
	}
	glutPostRedisplay();
}

void mouse_move_callback(GLint x, GLint y)
{
	//changed
	for (int i = 0; i < 4; i++)
	{
		//modify this part to let curve points translate to (- 0.5 * viewportwidth, - 0.5 * viewportwidth)
		if (edit_ctrlpts_idx[i] != -1) {
			//change finalcurve
			finalcur[mode].cur[i].control_pts[edit_ctrlpts_idx[i]][0] = (REAL)x;
			finalcur[mode].cur[i].control_pts[edit_ctrlpts_idx[i]][1] = (REAL)(viewportheight - y);
		}
	}
	glutPostRedisplay();
}

void keyboard_callback(unsigned char key, int x, int y)
{
	switch (key)
	{
		//print control points of each curve
	case '0':
	{
		mode = 0;
		freedom();
		readtxt();
		break;
	}
	case '1':
	{
		mode = 1;
		freedom();
		readtxt();
		break;
	}
	case '2':
	{
		mode = 2;
		freedom();
		readtxt();
		break;
	}
	case '3':
	{
		mode = 3;
		freedom();
		readtxt();
		break;
	}
	case '4':
	{
		mode = 4;
		freedom();
		readtxt();
		break;
	}
	case '5':
	{
		mode = 5;
		freedom();
		readtxt();
		break;
	}
	case '6':
	{
		mode = 6;
		freedom();
		readtxt();
		break;
	}
	case '7':
	{
		mode = 7;
		freedom();
		readtxt();
		break;
	}
	case '8':
	{
		mode = 8;
		freedom();
		readtxt();
		break;
	}
	case 'a':
	{
		printf("----------------------------------------------------------------------------\n");
		printf("                      Ctl_pt1,       Ctl_pt2,        Ctl_pt3,       Ctl_pt4\n");
		printf("finalcur[%d].cur 1: (%.2f,%.2f),(%.2f,%.2f),(%.2f,%.2f),(%.2f,%.2f)\n", mode, finalcur[mode].cur[0].control_pts[0][0], finalcur[mode].cur[0].control_pts[0][1], finalcur[mode].cur[0].control_pts[1][0], finalcur[mode].cur[0].control_pts[1][1], finalcur[mode].cur[0].control_pts[2][0], finalcur[mode].cur[0].control_pts[2][1], finalcur[mode].cur[0].control_pts[3][0], finalcur[mode].cur[0].control_pts[3][1]);
		printf("finalcur[%d].cur 2: (%.2f,%.2f),(%.2f,%.2f),(%.2f,%.2f),(%.2f,%.2f)\n", mode, finalcur[mode].cur[1].control_pts[0][0], finalcur[mode].cur[1].control_pts[0][1], finalcur[mode].cur[1].control_pts[1][0], finalcur[mode].cur[1].control_pts[1][1], finalcur[mode].cur[1].control_pts[2][0], finalcur[mode].cur[1].control_pts[2][1], finalcur[mode].cur[1].control_pts[3][0], finalcur[mode].cur[1].control_pts[3][1]);
		printf("finalcur[%d].cur 3: (%.2f,%.2f),(%.2f,%.2f),(%.2f,%.2f),(%.2f,%.2f)\n", mode, finalcur[mode].cur[2].control_pts[0][0], finalcur[mode].cur[2].control_pts[0][1], finalcur[mode].cur[2].control_pts[1][0], finalcur[mode].cur[2].control_pts[1][1], finalcur[mode].cur[2].control_pts[2][0], finalcur[mode].cur[2].control_pts[2][1], finalcur[mode].cur[2].control_pts[3][0], finalcur[mode].cur[2].control_pts[3][1]);
		printf("finalcur[%d].cur 4: (%.2f,%.2f),(%.2f,%.2f),(%.2f,%.2f),(%.2f,%.2f)\n", mode, finalcur[mode].cur[3].control_pts[0][0], finalcur[mode].cur[3].control_pts[0][1], finalcur[mode].cur[3].control_pts[1][0], finalcur[mode].cur[3].control_pts[1][1], finalcur[mode].cur[3].control_pts[2][0], finalcur[mode].cur[3].control_pts[2][1], finalcur[mode].cur[3].control_pts[3][0], finalcur[mode].cur[3].control_pts[3][1]);
		printf("----------------------------------------------------------------------------\n");
	}
	default:
		break;
	}
	glutPostRedisplay();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(600, 100);
	glutCreateWindow("Bezier Editor");

	Init();
	glutReshapeFunc(reshape_callback);
	glutKeyboardFunc(keyboard_callback);
	glutMouseFunc(mouse_callback);
	glutMotionFunc(mouse_move_callback);
	glutDisplayFunc(display_callback);
	glutMainLoop();
	return 0;
}

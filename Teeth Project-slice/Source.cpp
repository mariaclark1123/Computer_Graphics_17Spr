/***********left-up: curve1***********/
/***********right-up: curve2***********/
/***********right-bottom: curve3***********/
/***********left-up: curve1***********/
#pragma warning(disable:4146)
#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <GL/glut.h>
#include "curve.h"
#include "viewport.h"

#define RES 32
typedef struct finalcurve
{
	CubicBezierCurve cur[4];
	Point3 vertices[5000];
	Point3 faces[10000];
	GLint verticenum = 0;
	GLint facenum = 0;
	GLfloat z, xmin, xmax, ymin, ymax;
	GLint time = 1;
}finalcurve;

/* global */
BicubicBezierSurface surface;
float points[RES + 1][RES + 1][3];
CubicBezierCurve cur_mid, cur_mid1, cur_mid2;

finalcurve finalcur[9];
int edit_ctrlpts_idx[4] = { -1,-1,-1,-1 };
int mode = 0;

GLsizei width = 800, height = 400;
GLsizei viewportwidth = 400, viewportheight = 400;

Vector3d eye;
Vector3d center;
Vector3d upVector;
bool isDragging = false;
float radius;
int lastX = -1;
int lastY = -1;
int mouseButton = -1;
int selectedscene = 0;
int selected = -1;

void setcurve(char* filename, int level);
void DrawCruve(CubicBezierCurve curve, int x);
void createsurface(int level1, int level2, CubicBezierCurve cur1, CubicBezierCurve cur2);

//20170511
void calc_surface()
{
	for (int i = 0; i <= RES; i++)
		for (int j = 0; j <= RES; j++)
		{
			evaluate_surface(&surface, i / (float)RES, j / (float)RES, points[i][j]);
		}
}

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
	setcurve("2_17_slice0.txt", 0);
	setcurve("2_17_slice1.txt", 1);
	setcurve("2_17_slice2.txt", 2);
	setcurve("2_17_slice3.txt", 3);
	setcurve("2_17_slice4.txt", 4);
	setcurve("2_17_slice5.txt", 5);
	setcurve("2_17_slice6.txt", 6);
	setcurve("2_17_slice7.txt", 7);
	setcurve("2_17_slice8.txt", 8);

	//20170511
	createsurface(0, 1, finalcur[0].cur[0], finalcur[1].cur[0]);
}

//20170511
void createsurface(int level1, int level2, CubicBezierCurve cur1, CubicBezierCurve cur2)
{
	//CubicBezierCurve cur_mid, cur_mid1, cur_mid2;
	Point3 vector[4];
	GLfloat cur_mid_z;
	int i, j;

	for (i = 0; i < 4; i++)
	{
		vector[i][X] = 0.1667*(cur2.control_pts[i][X] - cur1.control_pts[i][X]);
		vector[i][Y] = 0.1667*(cur2.control_pts[i][Y] - cur1.control_pts[i][Y]);
		vector[i][Z] = 0.1667*(finalcur[level2].z - finalcur[level1].z);
	}

	for (i = 0; i < 4; i++)
	{
		cur_mid.control_pts[i][X] = 0.5*(cur1.control_pts[i][X] + cur2.control_pts[i][X]);
		cur_mid.control_pts[i][Y] = 0.5*(cur1.control_pts[i][Y] + cur2.control_pts[i][Y]);
		cur_mid.control_pts[i][Z] = 0.5*(finalcur[level1].z + finalcur[level2].z);
	}

	for (i = 0; i < 4; i++)
	{
		cur_mid1.control_pts[i][X] = cur_mid.control_pts[i][X] - vector[i][X];
		cur_mid1.control_pts[i][Y] = cur_mid.control_pts[i][Y] - vector[i][Y];
		cur_mid1.control_pts[i][Z] = cur_mid.control_pts[i][Z] - vector[i][Z];

		cur_mid2.control_pts[i][X] = cur_mid.control_pts[i][X] + vector[i][X];
		cur_mid2.control_pts[i][Y] = cur_mid.control_pts[i][Y] + vector[i][Y];
		cur_mid2.control_pts[i][Z] = cur_mid.control_pts[i][Z] + vector[i][Z];
	}

	//20170511
	for (i = 0; i < 4; i++)
	{
		SET_PT3(surface.control_pts[0][i], cur1.control_pts[i][X], cur1.control_pts[i][Y], finalcur[level1].z);
		SET_PT3(surface.control_pts[1][i], cur_mid1.control_pts[i][X], cur_mid1.control_pts[i][Y], cur_mid1.control_pts[0][Z]);
		SET_PT3(surface.control_pts[2][i], cur_mid2.control_pts[i][X], cur_mid2.control_pts[i][Y], cur_mid2.control_pts[0][Z]);
		SET_PT3(surface.control_pts[3][i], cur2.control_pts[i][X], cur2.control_pts[i][Y], finalcur[level2].z);
	}

	calc_surface();

	//20170511
	for (int i = 0; i <= RES; i += 4)
	{
		glBegin(GL_LINE_STRIP);
		for (int j = 0; j <= RES; j++)
			glVertex3f(points[i][j][0], points[i][j][1], points[i][j][2]);
		glEnd();
	}
	for (int i = 0; i <= RES; i += 4)
	{
		glBegin(GL_LINE_STRIP);
		for (int j = 0; j <= RES; j++)
			glVertex3f(points[j][i][0], points[j][i][1], points[j][i][2]);
		glEnd();
	}
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

void setcurve(char* filename, int level)
{
	int numVertex = 0;
	int numFaces = 0;
	int IdxV = 0;
	int IdxFace = 0;
	GLfloat x_max, xmaxy, x_min, xminy, y_max, ymaxx, y_min, yminx, z;
	Point3 cs[4], cm[4], ce[4], ctrlp_2[4], ctrlp_3[4];

	char line[256];//get line
	FILE* fp = NULL;
	//load model
	fp = fopen(filename, "rb");

	if (fp == NULL)
	{
		printf("%s file can not open", filename);
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

	//set finalcur's facenum and verticenum
	finalcur[level].verticenum = numVertex;
	finalcur[level].facenum = numFaces;

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
		        //vertices[j][0] = (x + 22) * 30 + 0.5* viewportwidth;	vertices[j][1] = (y - 78) * 30 + 0.5* viewportheight;	vertices[j][2] = (z - 38) * 30;

				finalcur[level].vertices[IdxV][X] = (x + 22) * 30 + 0.5* viewportwidth;
				finalcur[level].vertices[IdxV][Y] = (y - 78) * 30 + 0.5* viewportheight;
				finalcur[level].vertices[IdxV][Z] = (z - 38) * 30;
				IdxV++;
			}
		}
		else if (line[0] == 'f')
		{
			fseek(fp, -(strlen(line) + 1), SEEK_CUR);		    //한줄 앞으로
																//printf("length of line is %d\n", strlen(line));

			int x1, x2, y1, y2, z1, z2;
			fscanf(fp, "%s %d//%d %d//%d %d//%d", line, &x1, &x2, &y1, &y2, &z1, &z2);

			//면의 정점                                                          순서 정보
			finalcur[level].faces[IdxFace][X] = x1 - 1;
			finalcur[level].faces[IdxFace][Y] = y1 - 1;
			finalcur[level].faces[IdxFace][Z] = z1 - 1;
			IdxFace++;
		}
	}
	x_max = x_min = ymaxx = yminx = finalcur[level].vertices[0][X];
	y_max = y_min = xmaxy = xminy = finalcur[level].vertices[0][Y];
	z = finalcur[level].vertices[0][Z];
	//	printf("z is %f\n", z);

	for (int i = 1; i < numVertex; i++)
	{
		if (x_max < finalcur[level].vertices[i][X])
		{
			x_max = finalcur[level].vertices[i][X];
			xmaxy = finalcur[level].vertices[i][Y];
		}
		if (x_min > finalcur[level].vertices[i][X])
		{
			x_min = finalcur[level].vertices[i][X];
			xminy = finalcur[level].vertices[i][Y];
		}
		if (y_max < finalcur[level].vertices[i][Y])
		{
			y_max = finalcur[level].vertices[i][Y];
			ymaxx = finalcur[level].vertices[i][X];
		}
		if (y_min > finalcur[level].vertices[i][Y])
		{
			y_min = finalcur[level].vertices[i][Y];
			yminx = finalcur[level].vertices[i][X];
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

	if (finalcur[level].time < 5)
	{
		for (int i = 0; i < 4; i++)
		{
			{
				SET_PT2(finalcur[level].cur[i].control_pts[0], cs[i][0], cs[i][1]);
				SET_PT2(finalcur[level].cur[i].control_pts[1], ctrlp_2[i][0], ctrlp_2[i][1]);
				SET_PT2(finalcur[level].cur[i].control_pts[2], ctrlp_3[i][0], ctrlp_3[i][1]);
				SET_PT2(finalcur[level].cur[i].control_pts[3], ce[i][0], ce[i][1]);

			}
			finalcur[level].time++;
		}
		finalcur[level].z = z;
		finalcur[level].xmin = x_min;
		finalcur[level].xmax = x_max;
		finalcur[level].ymin = y_min;
		finalcur[level].ymax = y_max;
	}
	fclose(fp);
}

void Showslice()
{
	glPushMatrix();
	//Draw boundary and mesh
	glColor3f(1.0, 1.0, 0.0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(finalcur[mode].xmin, finalcur[mode].ymax);
	glVertex2f(finalcur[mode].xmin, finalcur[mode].ymin);
	glVertex2f(finalcur[mode].xmax, finalcur[mode].ymin);
	glVertex2f(finalcur[mode].xmax, finalcur[mode].ymax);
	glEnd();

	for (int i = 0; i < finalcur[mode].facenum - 1; i++)
	{
		int f1 = finalcur[mode].faces[i][0];
		int f2 = finalcur[mode].faces[i][1];
		int f3 = finalcur[mode].faces[i][2];

		float p1[3] = { finalcur[mode].vertices[f1][X], finalcur[mode].vertices[f1][Y] };
		float p2[3] = { finalcur[mode].vertices[f2][X], finalcur[mode].vertices[f2][Y] };
		float p3[3] = { finalcur[mode].vertices[f3][X], finalcur[mode].vertices[f3][Y] };

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
	/*glBegin(GL_LINES);
	glVertex3f(-1, 0, 0);
	glVertex3f(1, 0, 0);
	glEnd();*/
	glBegin(GL_LINES);
	glVertex3f(0, -1, 0);
	glVertex3f(0, 1, 0);
	glEnd();


	// XY
//	glViewport(0, viewportheight, viewportwidth, viewportheight);

	glViewport(0, 0, viewportwidth, viewportheight);
	glLoadIdentity();
	gluOrtho2D(0, (double)viewportwidth, 0, (double)viewportheight);
	glColor3ub(255, 255, 255);


    //DrawModel();
	Showslice();
	for (int i = 0; i < 4; i++)
		DrawCruve(finalcur[mode].cur[i], 0);

	// 3D
//	glViewport(viewportwidth, viewportheight, viewportwidth, viewportheight);
	glViewport(viewportwidth, 0, viewportwidth, viewportheight);

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

	glColor3f(0.7, 0.7, 0.7);
	for (int i = 0; i < 8; i++)
	{
		createsurface(i, i + 1, finalcur[i].cur[0], finalcur[i + 1].cur[0]);
		createsurface(i, i + 1, finalcur[i].cur[1], finalcur[i + 1].cur[1]);
		createsurface(i, i + 1, finalcur[i].cur[2], finalcur[i + 1].cur[2]);
		createsurface(i, i + 1, finalcur[i].cur[3], finalcur[i + 1].cur[3]);
	}

	curvein3d();
	glDisable(GL_DEPTH_TEST);

	glutSwapBuffers();
}

void mouse_callback(GLint button, GLint action, GLint x, GLint y)
{
	int scene = 0;
	if (x < viewportwidth)
	{
		if (y < viewportheight)
			scene = 1;
		else
		{
			scene = 3;
			y -= (int)viewportheight;
		}
	}
	else
	{
		x -= (int)viewportwidth;
		if (y < viewportheight)
			scene = 2;
		else
		{
			scene = 4;
			y -= (int)viewportheight;
		}
	}

	if (action == GLUT_UP)
	{
		isDragging = false;
		mouseButton = -1;
	}

	if (scene == 2)
	{
		if (action == GLUT_DOWN)
		{
			mouseButton = button;
			isDragging = true;
			lastX = x;
			lastY = y;
		}
	}
	else
	{
		if (button == GLUT_LEFT_BUTTON)
		{
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
	}
	glutPostRedisplay();
}

void mouse_move_callback(GLint x, GLint y)
{
	Vector3d lastP = getMousePoint(lastX, lastY, viewportwidth, viewportheight, radius);
	Vector3d currentP = getMousePoint(x - viewportwidth, y, viewportwidth, viewportheight, radius);

	if (mouseButton == GLUT_LEFT_BUTTON)
	{
		Vector3d rotateVector;
		rotateVector.cross(currentP, lastP);
		double angle = -currentP.angle(lastP) * 2;
		rotateVector = unProjectToEye(rotateVector, eye, center, upVector);

		Vector3d dEye;
		dEye.sub(center, eye);
		dEye = rotate(dEye, rotateVector, -angle);
		upVector = rotate(upVector, rotateVector, -angle);
		eye.sub(center, dEye);
	}
	else if (mouseButton == GLUT_RIGHT_BUTTON) {
		Vector3d dEye;
		dEye.sub(center, eye);
		double offset = 0.025;
		if ((y - lastY) < 0) {
			dEye.scale(1 - offset);
		}
		else {
			dEye.scale(1 + offset);
		}
		eye.sub(center, dEye);
	}
	else if (mouseButton == GLUT_MIDDLE_BUTTON) {
		double dx = x - viewportwidth - lastX;
		double dy = y - lastY;
		if (dx != 0 || dy != 0)
		{
			Vector3d moveVector(dx, dy, 0);
			moveVector = unProjectToEye(moveVector, eye, center, upVector);
			moveVector.normalize();
			double eyeDistance = Vector3d(eye).distance(Vector3d(center));
			moveVector.scale(std::sqrt(dx*dx + dy*dy) / 1000 * eyeDistance);
			center.add(moveVector);
			eye.add(moveVector);
		}
	}

	lastX = x - viewportwidth;
	lastY = y;


//	if (selectedscene == 1)
	if (x<viewportwidth)
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
		printf("slice[%d]-vertice number: %d, face number: %d\n", mode, finalcur[mode].verticenum, finalcur[mode].facenum);
		break;
	}
	case '1':
	{
		mode = 1;
		printf("slice[%d]-vertice number: %d, face number: %d\n", mode, finalcur[mode].verticenum, finalcur[mode].facenum);
		break;
	}
	case '2':
	{
		mode = 2;
		printf("slice[%d]-vertice number: %d, face number: %d\n", mode, finalcur[mode].verticenum, finalcur[mode].facenum);
		break;
	}
	case '3':
	{
		mode = 3;
		printf("slice[%d]-vertice number: %d, face number: %d\n", mode, finalcur[mode].verticenum, finalcur[mode].facenum);
		break;
	}
	case '4':
	{
		mode = 4;
		printf("slice[%d]-vertice number: %d, face number: %d\n", mode, finalcur[mode].verticenum, finalcur[mode].facenum);
		break;
	}
	case '5':
	{
		mode = 5;
		printf("slice[%d]-vertice number: %d, face number: %d\n", mode, finalcur[mode].verticenum, finalcur[mode].facenum);
		break;
	}
	case '6':
	{
		mode = 6;
		printf("slice[%d]-vertice number: %d, face number: %d\n", mode, finalcur[mode].verticenum, finalcur[mode].facenum);
		break;
	}
	case '7':
	{
		mode = 7;
		printf("slice[%d]-vertice number: %d, face number: %d\n", mode, finalcur[mode].verticenum, finalcur[mode].facenum);
		break;
	}
	case '8':
	{
		mode = 8;
		printf("slice[%d]-vertice number: %d, face number: %d\n", mode, finalcur[mode].verticenum, finalcur[mode].facenum);
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

void reshape_callback(GLint nw, GLint nh)
{
	width = nw;
	height = nh;
	viewportwidth = width / 2.0f;
//	viewportheight = height / 2.0f;

	viewportheight = height;
	radius = std::sqrt(viewportwidth * viewportwidth + viewportheight * viewportheight) / 2;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
}

int main(int argc, char *argv[])
{
//	setcurve("2_17_slice0.txt", 1);
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

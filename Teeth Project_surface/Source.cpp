/***********left-up: curve1***********/
/***********right-up: curve2***********/
/***********right-bottom: curve3***********/
/***********left-up: curve1***********/
#pragma warning(disable:4146)
#pragma warning(disable:4996)

#include <algorithm>
#include <windows.h>
#include <GL/glut.h>
#include "curve.h"
#include "viewport.h"
#include "init.h"

#define RES 16
#define MAX_CHAR 128

/* global */
BicubicBezierSurface surface[4];
SB slice[9];
float points[4][RES + 1][RES + 1][3];
int edit_ctrlpts_idx[4] = { -1,-1,-1,-1 };

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
int mode = 0;
int scene;
int showslice = 1;
int showpt = 1;
int showsurface = 1;

//20170511
void calc_surface()
{
	for (int i = 0; i <= RES; i++)
		for (int j = 0; j <= RES; j++)
		{
			evaluate_surface(&surface[0], i / (float)RES, j / (float)RES, points[0][i][j]);
			evaluate_surface(&surface[1], i / (float)RES, j / (float)RES, points[1][i][j]);
			evaluate_surface(&surface[2], i / (float)RES, j / (float)RES, points[2][i][j]);
			evaluate_surface(&surface[3], i / (float)RES, j / (float)RES, points[3][i][j]);
		}
}

void Init()
{
	eye = Vector3d(1000, 1000, 1000);
	center = Vector3d(0, 0, 0);
	upVector = Vector3d(0, 1, 0);

	//Set a sign
	for (int i = 0; i < 9; i++)
	{
		slice[i].cur[0].control_pts[0][0] = 0;
		slice[i].z = 0;
	}
	setcurve("2_17_slice0.txt", &slice[0]);
	setcurve("2_17_slice1.txt", &slice[1]);
	setcurve("2_17_slice2.txt", &slice[2]);
	setcurve("2_17_slice3.txt", &slice[3]);
	setcurve("2_17_slice4.txt", &slice[4]);
	setcurve("2_17_slice5.txt", &slice[5]);
	setcurve("2_17_slice6.txt", &slice[6]);
	setcurve("2_17_slice7.txt", &slice[7]);
	setcurve("2_17_slice8.txt", &slice[8]);
}

//20170511
void Slice_surface(int level1, int level2)
{
	CubicBezierCurve cur_mid[4], cur_mid1[4], cur_mid2[4];
	Point3 vector[4][4];
	Point3 center_mid;
	Point3 dist[4];
	Point3 dist2[4];
	Point3 ave[4];
	Point3 ave1[4];
	Point3 ave2[4];
	
	float d1[4], d2[4], rate[4];
	int i, j;

	for (i = 0; i < 3; i++)
		for (j = 0; j < 4; j++)
		{
			ave[j][i] = 0;
			ave1[j][i] = 0;
			ave2[j][i] = 0;
		}

	center_mid[X] = 0.5*(slice[level1].center[X] + slice[level2].center[X]);
	center_mid[Y] = 0.5*(slice[level1].center[Y] + slice[level2].center[Y]);
	center_mid[Z] = 0.5*(slice[level1].center[Z] + slice[level2].center[Z]);

	for (int j = 0; j < 4; j++)
	{
		//surface between one curve and another curve
		for (i = 0; i < 4; i++)
		{
			ave1[j][X] += slice[level1].cur[j].control_pts[i][X];
			ave1[j][Y] += slice[level1].cur[j].control_pts[i][Y];
			ave1[j][Z] += slice[level1].z;

			ave2[j][X] += slice[level2].cur[j].control_pts[i][X];
			ave2[j][Y] += slice[level2].cur[j].control_pts[i][Y];
			ave2[j][Z] += slice[level2].z;
		}

		ave1[j][X] = ave1[j][X] / 4.0;
		ave1[j][Y] = ave1[j][Y] / 4.0;
		ave1[j][Z] = ave1[j][Z] / 4.0;

		ave2[j][X] = ave2[j][X] / 4.0;
		ave2[j][Y] = ave2[j][Y] / 4.0;
		ave2[j][Z] = ave2[j][Z] / 4.0;

		ave[j][X] = 0.5 * (ave1[j][X] + ave2[j][X]);
		ave[j][Y] = 0.5 * (ave1[j][Y] + ave2[j][Y]);
		ave[j][Z] = 0.5 * (ave1[j][Z] + ave2[j][Z]);

		for (i = 0; i < 4; i++)
		{
			vector[j][i][X] = 0.1667*(slice[level2].cur[j].control_pts[i][X] - slice[level1].cur[j].control_pts[i][X]);
			vector[j][i][Y] = 0.1667*(slice[level2].cur[j].control_pts[i][Y] - slice[level1].cur[j].control_pts[i][Y]);
			vector[j][i][Z] = 0.1667*(slice[level2].z - slice[level1].z);
		}
		for (i = 0; i < 4; i++)
		{
			cur_mid[j].control_pts[i][X] = 0.5*(slice[level1].cur[j].control_pts[i][X] + slice[level2].cur[j].control_pts[i][X]);
			cur_mid[j].control_pts[i][Y] = 0.5*(slice[level1].cur[j].control_pts[i][Y] + slice[level2].cur[j].control_pts[i][Y]);
			cur_mid[j].control_pts[i][Z] = 0.5*(slice[level1].z + slice[level2].z);
		}

		//distance between center and cur_ave
		dist[j][X] = ave[j][X] - center_mid[X];
		dist[j][Y] = ave[j][Y] - center_mid[Y];
		d1[j] = dist[j][X] * dist[j][X] + dist[j][Y] * dist[j][Y];

		//distance between cur_ave1 and cur_ave2
		dist2[j][X] = ave1[j][X] - ave2[j][X];
		dist2[j][Y] = ave1[j][Y] - ave2[j][Y];
		dist2[j][Z] = ave1[j][Z] - ave2[j][Z];
		d2[j] = dist2[j][X] * dist2[j][X] + dist2[j][Y] * dist2[j][Y] + dist2[j][Z] * dist2[j][Z];

		rate[j] = (d2[j] * 1.0) / d1[j];

		for (i = 0; i < 4; i++)
		{
			cur_mid1[j].control_pts[i][X] = cur_mid[j].control_pts[i][X] - vector[j][i][X] + 0.07 * rate[j] * dist[j][X];
			cur_mid1[j].control_pts[i][Y] = cur_mid[j].control_pts[i][Y] - vector[j][i][Y] + 0.07 * rate[j] * dist[j][Y];// +1.5 * fabs(vector[i][Y]);
			cur_mid1[j].control_pts[i][Z] = cur_mid[j].control_pts[i][Z] - vector[j][i][Z];

			cur_mid2[j].control_pts[i][X] = cur_mid[j].control_pts[i][X] + vector[j][i][X] + 0.07 * rate[j] * dist[j][X];
			cur_mid2[j].control_pts[i][Y] = cur_mid[j].control_pts[i][Y] + vector[j][i][Y] + 0.07 * rate[j] * dist[j][Y];// +1.5 * fabs(vector[i][Y]);
			cur_mid2[j].control_pts[i][Z] = cur_mid[j].control_pts[i][Z] + vector[j][i][Z];
		}

		//20170726 3:30
		for (i = 0; i < 4; i++)
		{
			SET_PT3(surface[j].control_pts[0][i], slice[level1].cur[j].control_pts[i][X], slice[level1].cur[j].control_pts[i][Y], slice[level1].z);
			SET_PT3(surface[j].control_pts[1][i], cur_mid1[j].control_pts[i][X], cur_mid1[j].control_pts[i][Y], cur_mid1[j].control_pts[0][Z]);
			SET_PT3(surface[j].control_pts[2][i], cur_mid2[j].control_pts[i][X], cur_mid2[j].control_pts[i][Y], cur_mid2[j].control_pts[0][Z]);
			SET_PT3(surface[j].control_pts[3][i], slice[level2].cur[j].control_pts[i][X], slice[level2].cur[j].control_pts[i][Y], slice[level2].z);
		}
	}
    calc_surface();
	
	if (showpt)
	{
		//center_mid point
		glColor3f(1.0, 0.0, 0.0);
		glPointSize(5.0);
		glBegin(GL_POINTS);
		glVertex3f(center_mid[X], center_mid[Y], center_mid[Z]);
		glEnd();

		//ave point
		glBegin(GL_POINTS);
		for (int j = 0; j < 4; j++)
			glVertex3f(ave[j][X], ave[j][Y], ave[j][Z]);
		glEnd();

		//draw distance
		glBegin(GL_LINES);
		for (int j = 0; j < 4; j++)
		{
			glVertex3f(center_mid[X], center_mid[Y], center_mid[Z]);
			glVertex3f(ave[j][X], ave[j][Y], ave[j][Z]);
		}
		glEnd();
	}
	glColor3f(0.7, 0.7, 0.7);


	//20170511
	for (int h = 0; h < 4; h++)
	{
		for (int i = 0; i <= RES; i += 4)
		{
			glBegin(GL_LINE_STRIP);
			for (int j = 0; j <= RES; j++)
				glVertex3f(points[h][i][j][0], points[h][i][j][1], points[h][i][j][2]);
			glEnd();
		}
		for (int i = 0; i <= RES; i += 4)
		{
			glBegin(GL_LINE_STRIP);
			for (int j = 0; j <= RES; j++)
				glVertex3f(points[h][j][i][0], points[h][j][i][1], points[h][j][i][2]);
			glEnd();
		}
	}
}

//Draw curve in 3D
void curvein3d()
{
	for (int i = 0; i < 9; i++)
	{
		if (slice[i].cur[0].control_pts[0][0] != 0)
		{
			glPushMatrix();
			glTranslatef(0, 0, slice[i].z);
			for (int j = 0; j < 4; j++)
			{
				DrawCruve(slice[i].cur[j], 1);
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

void Showslice()
{
	glPushMatrix();
	//Draw boundary and mesh
	glColor3f(1.0, 1.0, 0.0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(slice[mode].xmin, slice[mode].ymax);
	glVertex2f(slice[mode].xmin, slice[mode].ymin);
	glVertex2f(slice[mode].xmax, slice[mode].ymin);
	glVertex2f(slice[mode].xmax, slice[mode].ymax);
	glEnd();

	for (int i = 0; i < slice[mode].facenum - 1; i++)
	{
		int f1 = slice[mode].faces[i][0];
		int f2 = slice[mode].faces[i][1];
		int f3 = slice[mode].faces[i][2];

		float p1[3] = { slice[mode].vertices[f1][X], slice[mode].vertices[f1][Y] };
		float p2[3] = { slice[mode].vertices[f2][X], slice[mode].vertices[f2][Y] };
		float p3[3] = { slice[mode].vertices[f3][X], slice[mode].vertices[f3][Y] };

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

//Draw annotation
void drawString(const char* str)
{
	static int isFirstCall = 1;
	static GLuint lists;

	if (isFirstCall)
	{
		isFirstCall = 0;
		lists = glGenLists(MAX_CHAR);

		wglUseFontBitmaps(wglGetCurrentDC(), 0, MAX_CHAR, lists);
	}
	for (; *str != '\0'; ++str)
		glCallList(lists + *str);
}

void display_callback(void)
{
	//change num to string
	char anno[20] = "Slice ";
	char num[10];
	itoa(mode, num, 10);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor3f(0, 0, 0);
	
	glBegin(GL_LINES);
	glVertex3f(0, -1, 0);
	glVertex3f(0, 1, 0);
	glEnd();


	// XY
	glViewport(0, 0, viewportwidth, viewportheight);
	glLoadIdentity();
	gluOrtho2D(0, (double)viewportwidth, 0, (double)viewportheight);
	
	/*annotation*/
	strcat(anno, num);
	glRasterPos2f(10.0f, viewportheight - 10.0);
	drawString(anno);

	Showslice();
	for (int i = 0; i < 4; i++)
		DrawCruve(slice[mode].cur[i], 0);

	// 3D
	glViewport(viewportwidth, 0, viewportwidth, viewportheight);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(35, viewportwidth / (double)viewportheight, 0.1, 25000);

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
	
	if (showsurface)
	{
		for (int i = 0; i < 8; i++)
			Slice_surface(i, i + 1);
	}

	if(showslice)
		curvein3d();
	glDisable(GL_DEPTH_TEST);

	glutSwapBuffers();
}

void mouse_callback(GLint button, GLint action, GLint x, GLint y)
{

	if (x < viewportwidth)
	{
		if (y < viewportheight)
			scene = 1;
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
					//change sliceve when curve changed
					edit_ctrlpts_idx[i] = hit_index(&slice[mode].cur[i], x, viewportheight - y);
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
				//change sliceve
				slice[mode].cur[i].control_pts[edit_ctrlpts_idx[i]][0] = (REAL)x;
				slice[mode].cur[i].control_pts[edit_ctrlpts_idx[i]][1] = (REAL)(viewportheight - y);
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
		printf("slice[%d]-vertice number: %d, face number: %d\n", mode, slice[mode].verticenum, slice[mode].facenum);
		break;
	}
	case '1':
	{
		mode = 1;
		printf("slice[%d]-vertice number: %d, face number: %d\n", mode, slice[mode].verticenum, slice[mode].facenum);
		break;
	}
	case '2':
	{
		mode = 2;
		printf("slice[%d]-vertice number: %d, face number: %d\n", mode, slice[mode].verticenum, slice[mode].facenum);
		break;
	}
	case '3':
	{
		mode = 3;
		printf("slice[%d]-vertice number: %d, face number: %d\n", mode, slice[mode].verticenum, slice[mode].facenum);
		break;
	}
	case '4':
	{
		mode = 4;
		printf("slice[%d]-vertice number: %d, face number: %d\n", mode, slice[mode].verticenum, slice[mode].facenum);
		break;
	}
	case '5':
	{
		mode = 5;
		printf("slice[%d]-vertice number: %d, face number: %d\n", mode, slice[mode].verticenum, slice[mode].facenum);
		break;
	}
	case '6':
	{
		mode = 6;
		printf("slice[%d]-vertice number: %d, face number: %d\n", mode, slice[mode].verticenum, slice[mode].facenum);
		break;
	}
	case '7':
	{
		mode = 7;
		printf("slice[%d]-vertice number: %d, face number: %d\n", mode, slice[mode].verticenum, slice[mode].facenum);
		break;
	}
	case '8':
	{
		mode = 8;
		printf("slice[%d]-vertice number: %d, face number: %d\n", mode, slice[mode].verticenum, slice[mode].facenum);
		break;
	}
	case 'a':
	{
		printf("----------------------------------------------------------------------------\n");
		printf("                      Ctl_pt1,       Ctl_pt2,        Ctl_pt3,       Ctl_pt4\n");
		printf("slice[%d].cur 1: (%.2f,%.2f),(%.2f,%.2f),(%.2f,%.2f),(%.2f,%.2f)\n", mode, slice[mode].cur[0].control_pts[0][0], slice[mode].cur[0].control_pts[0][1], slice[mode].cur[0].control_pts[1][0], slice[mode].cur[0].control_pts[1][1], slice[mode].cur[0].control_pts[2][0], slice[mode].cur[0].control_pts[2][1], slice[mode].cur[0].control_pts[3][0], slice[mode].cur[0].control_pts[3][1]);
		printf("slice[%d].cur 2: (%.2f,%.2f),(%.2f,%.2f),(%.2f,%.2f),(%.2f,%.2f)\n", mode, slice[mode].cur[1].control_pts[0][0], slice[mode].cur[1].control_pts[0][1], slice[mode].cur[1].control_pts[1][0], slice[mode].cur[1].control_pts[1][1], slice[mode].cur[1].control_pts[2][0], slice[mode].cur[1].control_pts[2][1], slice[mode].cur[1].control_pts[3][0], slice[mode].cur[1].control_pts[3][1]);
		printf("slice[%d].cur 3: (%.2f,%.2f),(%.2f,%.2f),(%.2f,%.2f),(%.2f,%.2f)\n", mode, slice[mode].cur[2].control_pts[0][0], slice[mode].cur[2].control_pts[0][1], slice[mode].cur[2].control_pts[1][0], slice[mode].cur[2].control_pts[1][1], slice[mode].cur[2].control_pts[2][0], slice[mode].cur[2].control_pts[2][1], slice[mode].cur[2].control_pts[3][0], slice[mode].cur[2].control_pts[3][1]);
		printf("slice[%d].cur 4: (%.2f,%.2f),(%.2f,%.2f),(%.2f,%.2f),(%.2f,%.2f)\n", mode, slice[mode].cur[3].control_pts[0][0], slice[mode].cur[3].control_pts[0][1], slice[mode].cur[3].control_pts[1][0], slice[mode].cur[3].control_pts[1][1], slice[mode].cur[3].control_pts[2][0], slice[mode].cur[3].control_pts[2][1], slice[mode].cur[3].control_pts[3][0], slice[mode].cur[3].control_pts[3][1]);
		printf("----------------------------------------------------------------------------\n");
		break;
	}
	case 'p':
		showpt = (showpt + 1) % 2;
		break;
	case 'c':
		showslice = (showslice + 1) % 2;
		break;
	case 's':
		showsurface = (showsurface + 1) % 2;
		break;
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
	viewportheight = height;
	radius = std::sqrt(viewportwidth * viewportwidth + viewportheight * viewportheight) / 2;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
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

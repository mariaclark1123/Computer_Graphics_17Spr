/*
 * System Software(Computer graphics) HW1 
 * 학과: 컴퓨터공학부
 * 학번: 2016-23145
 * 이름: 진방달
 * Date: 20170329
 * Seoul National University
 */

#include <iostream>
#include <math.h>
#include <GL/glut.h>

 /*Draw line between two points*/
void Drawline(float x1, float y1, float z1, float x2, float y2, float z2);

/*Draw sphere*/
void drawSphere();

/*Caculate the distance between 3d point and line*/
float sq_distance(float x0, float y0, float z0, float l_x1, float l_y1, float l_z1, float l_x2, float l_y2, float l_z2);
 
//get the near and far point's position
void UnProject(float mouse_x, float mouse_y);

 /*Select which sphere has been chosen, if there is intersection, choose the nearst one*/
void Select(float l_x1, float l_y1, float l_z1, float l_x2, float l_y2, float l_z2);

/*Drag the selected sphere with mouse*/
void Drag(float near_x, float near_y, float near_z, float far_x, float far_y, float far_z);

float near_x, near_y, near_z, far_x, far_y, far_z;
bool select_1 = false, select_2 = false, select_3 = false, mouse_mv = false;
float sp1_x, sp1_y, sp1_z, sp2_x, sp2_y, sp2_z, sp3_x, sp3_y, sp3_z;
float sp1_r, sp2_r, sp3_r;

//gluLookAt(GLdouble eyeX,GLdouble eyeY,GLdouble eyeZ,GLdouble centerX,GLdouble centerY,GLdouble centerZ,GLdouble upX,GLdouble upY,GLdouble upZ)
GLdouble eyeX = 0, eyeY = 10, eyeZ = -10;
GLdouble centerX = 0, centerY = 0, centerZ = 0;
GLdouble upX = 0, upY = 1, upZ = 0;

//gluPerspective(GLdouble fovy, GLdouble aspect, GLdouble Near, GLdouble Far)
GLdouble fovy = 50, aspect = 1260 / 760, Near = 0.1, Far = 100;

void Drawline(float x1, float y1, float z1, float x2, float y2, float z2)
{
	glColor3f(1.0, 1.0, 1.0);
	glLineWidth(1.0);
	glBegin(GL_LINES);
	glVertex3f(x1, y1, z1); 
	glVertex3f(x2, y2, z2);
	glEnd();	
}

void init()
{
	/*Distance between sphere and camera*/
	sp1_r = 15.62;    /*(0,0,2) to (0,10,-10)*/
	sp2_r = 17.2;     /*(0,0,4) to (0,10,-10)*/
	sp3_r = 18.87;    /*(0,0,6) to (0,10,-10)*/

	sp1_x = 0;
	sp1_y = 0;
	sp1_z = 2;
	sp2_x = 0;
	sp2_y = 0;
	sp2_z = 4;
	sp3_x = 0;
	sp3_y = 0;
	sp3_z = 6;
	 
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//caculate the distance between 3d point and line
float sq_distance(float x0, float y0, float z0, float l_x1, float l_y1, float l_z1, float l_x2, float l_y2, float l_z2)
{
	float mat1[3], mat2[3], mat3[3], temp[3], temp1, temp2, sq_dist;

	mat1[0] = x0 - l_x1;
	mat1[1] = y0 - l_y1;
	mat1[2] = z0 - l_z1;

	mat2[0] = x0 - l_x2;
	mat2[1] = y0 - l_y2;
	mat2[2] = z0 - l_z2;

	temp[0] = mat1[1] * mat2[2] - mat1[2] * mat2[1];
	temp[1] = - mat1[0] * mat2[2] + mat1[2] * mat2[0];
	temp[2] = mat1[0] * mat2[1] - mat1[1] * mat2[0];

	temp1 = temp[0] * temp[0] + temp[1] * temp[1] + temp[2] * temp[2];

	mat3[0] = l_x2 - l_x1;      //(p2-p1)
	mat3[1] = l_y2 - l_y1;
	mat3[2] = l_z2 - l_z1;

	temp2 = mat3[0] * mat3[0] + mat3[1] * mat3[1] + mat3[2] * mat3[2];

	sq_dist = 1.0 * temp1 / temp2;
	return sq_dist;
}

void drawSphere()
{
	glutSolidSphere(0.5, 20, 20);
}

void Drag(float near_x, float near_y, float near_z, float far_x, float far_y, float far_z)
{
	float nv[3], temp;
	nv[0] = far_x - near_x;            //caculate normal vector
	nv[1] = far_y - near_y;
	nv[2] = far_z - near_z;

	temp = nv[0] * nv[0] + nv[1] * nv[1] + nv[2] * nv[2];
	temp = sqrt(temp);
	nv[0] = nv[0] / temp;
	nv[1] = nv[1] / temp;
	nv[2] = nv[2] / temp;

	if (select_1)
	{
		sp1_x = near_x + nv[0] * sp1_r;   //near[3] + normal[3] * sp_length
		sp1_y = near_y + nv[1] * sp1_r;
		sp1_z = near_z + nv[2] * sp1_r;
	}
	else if (select_2)
	{
		sp2_x = near_x + nv[0] * sp2_r;
		sp2_y = near_y + nv[1] * sp2_r;
		sp2_z = near_z + nv[2] * sp2_r;
	}
	else if (select_3)
	{
		sp3_x = near_x + nv[0] * sp3_r;
		sp3_y = near_y + nv[1] * sp3_r;
		sp3_z = near_z + nv[2] * sp3_r;
	}
}

//get the near and far point's position
void UnProject(float mouse_x, float mouse_y)             //Mouse Position
{
	double modelview[16], projection[16], temp;
	int viewport[4];

	float winX, winY, winZ;
	double object_x = 0, object_y = 0, object_z = 0;     //3D coordinates
	
	double nv[3];    //normal vetor of far-near

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	winX = (float)mouse_x;
	winY = (float)viewport[3] - (float)mouse_y;
	
	gluUnProject((GLdouble)winX, (GLdouble)winY, 0, modelview, projection, viewport, &object_x, &object_y, &object_z);
	near_x = object_x;
	near_y = object_y;
	near_z = object_z;
	printf("Near plane's 3D coordinate is (%f,%f,%f)\n", near_x, near_y, near_z);

	gluUnProject((GLdouble)winX, (GLdouble)winY, 1, modelview, projection, viewport, &object_x, &object_y, &object_z);
	far_x = object_x;
	far_y = object_y;
	far_z = object_z;
	printf("Far plane's 3D coordinate is (%f,%f,%f)\n\n", far_x, far_y, far_z);

	Select(near_x, near_y, near_z, far_x, far_y, far_z);   //select sphere
	if(mouse_mv)
		Drag(near_x, near_y, near_z, far_x, far_y, far_z);    //drag sphere
}

void Select(float l_x1, float l_y1, float l_z1, float l_x2, float l_y2, float l_z2)
{
	float dist1, dist2, dist3;

	dist1 = sq_distance(sp1_x, sp1_y, sp1_z, l_x1, l_y1, l_z1, l_x2, l_y2, l_z2);   //distance from sphere1's center and line
	dist2 = sq_distance(sp2_x, sp2_y, sp2_z, l_x1, l_y1, l_z1, l_x2, l_y2, l_z2);   //distance from sphere2's center and line
	dist3 = sq_distance(sp3_x, sp3_y, sp3_z, l_x1, l_y1, l_z1, l_x2, l_y2, l_z2);   //distance from sphere3's center and line

	if (dist1 <= 0.25)
	{
		printf("select No.1 sphere\n");
		select_1 = true;
		select_2 = false;
		select_3 = false;
	}
	else if (dist2 <= 0.25)
	{
		printf("select No.2 sphere\n");
		select_2 = true;
		select_3 = false;
	}
	else if (dist3 <= 0.25)
	{
		printf("select No.3 sphere\n");
		select_3 = true;
	}
	printf("-------------------------------------------------------------\n");
}

void display(void) 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ);


	glPushMatrix();                     /*draw sphere1*/
	glTranslatef(sp1_x, sp1_y, sp1_z);
	if (select_1)
		glColor3f(0.7, 0.7, 0.7);
	else
		glColor3f(0.7, 0.0, 0.0);
	drawSphere();
	glPopMatrix();

	glPushMatrix();                     /*draw sphere2*/
	glTranslatef(sp2_x, sp2_y, sp2_z);
	if (select_2)
		glColor3f(0.7, 0.7, 0.7);
	else
		glColor3f(0.0, 0.7, 0.0);
	drawSphere();
	glPopMatrix();

	glPushMatrix();                     /*draw sphere3*/
	glTranslatef(sp3_x, sp3_y, sp3_z);
	if (select_3)
		glColor3f(0.7, 0.7, 0.7);
	else
		glColor3f(0.0, 0.0, 0.7);
	drawSphere();
	glPopMatrix();
	glutSwapBuffers();
}

void reshape(int w, int h) {
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, aspect, Near, Far);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ);
}

void mouse_callback(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			printf("Mouse Coordinate is (%d,%d)\n", x, y);
			UnProject(x, y);
			mouse_mv = true;
		}
		if (state == GLUT_UP)
		{
			select_1 = false;
			select_2 = false;
			select_3 = false;
			mouse_mv = false;
		}
	}
	glutPostRedisplay();
}

void mouse_move_callback(GLint x, GLint y)
{
	printf("Mouse Coordinate is (%d,%d)\n", x, y);
	UnProject((float)x, (float)y);
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(500, 100);
	glutCreateWindow("HW1");
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse_callback);
	glutMotionFunc(mouse_move_callback);
	glutMainLoop();

	return EXIT_SUCCESS;
}
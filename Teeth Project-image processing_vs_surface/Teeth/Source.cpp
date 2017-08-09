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

#define RES 64
#define MAX_CHAR 128
#define SNUM 21

/* global */
BicubicBezierSurface surface[4];
SB slice[SNUM];
float points[4][RES + 1][RES + 1][3];
int edit_ctrlpts_idx[4] = { -1,-1,-1,-1 };

GLsizei width = 800, height = 400;
GLsizei viewportwidth = 400, viewportheight = 400;

Vector3d eye;
Vector3d center;
Vector3d upVector;

//realmodel
float vertices[10000][3];
int faces[15000][3];
int numFaces = 0, numVertex = 0;

//139_5
float tx = 240, ty = 460, tz = 50;

//20170801 2:11
//2_4
//float tx = -9, ty = 95, tz = 65;
//2_8
//float tx = -15, ty = 95, tz = 65;
//2_17 
//float tx = -22, ty = 78, tz = 38;
//3_3
//float tx = -5, ty = 97, tz = 60;

//20170802
/*
  step_1: slice control point
  step_2: control point between two curves
  step_3: produce curve
 */
bool step_1 = 0, step_2 = 0, step_3 = 0;
bool isDragging = false;
float radius;
int lastX = -1;
int lastY = -1;
int mouseButton = -1;
int selectedscene = 0;
int mode = 0;
int scene;
int showslice = 1;
int showpt = 0;
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

//20170809
void getmodel(char* filename)
{
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
		if (line[0] == 'v'&& line[1] == ' ')
			numVertex++;
		else if (line[0] == 'f')
			numFaces++;
	}
	printf("numVertex is %d, numFaces is %d\n", numVertex, numFaces);
	//back to start point of file
	rewind(fp);

	int IdxVertex = 0;
	int IdxFace = 0;

	while (!feof(fp))
	{
		fgets(line, 256, fp);
		if (line[0] == 'v' && line[1] == ' ')
		{
			fseek(fp, -(strlen(line) + 1), SEEK_CUR);		//한줄 앞으로

			float x, y, z;
			fscanf(fp, "%s %f %f %f", line, &x, &y, &z);

			vertices[IdxVertex][X] = (x - 355
				
				
				
				) * 6 + 200;
			vertices[IdxVertex][Y] = (y - 450) * 6 + 200;
			vertices[IdxVertex][Z] = (z - 291) * 6;

			IdxVertex++;
		}

		else if (line[0] == 'f')
		{
			fseek(fp, -(strlen(line) + 1), SEEK_CUR);		    //한줄 앞으로
																//printf("length of line is %d\n", strlen(line));
			int x1, x2, y1, y2, z1, z2;
			fscanf(fp, "%s %d//%d %d//%d %d//%d", line, &x1, &x2, &y1, &y2, &z1, &z2);

			//면의 정점                                                          순서 정보
			faces[IdxFace][X] = x1 - 1;
			faces[IdxFace][Y] = y1 - 1;
			faces[IdxFace][Z] = z1 - 1;

			IdxFace++;
		}
	}
	printf("IdxVertex is %d, IdxFace is %d\n", IdxVertex, IdxFace);

	fclose(fp);
}

void Init()
{
	eye = Vector3d(1000, 1000, 1000);
	center = Vector3d(0, 0, 0);
	upVector = Vector3d(0, 1, 0);

	//Set a sign
	for (int i = 0; i < SNUM; i++)
	{
		slice[i].cur[0].control_pts[0][0] = 0;
		slice[i].z = 0;
	}
	getmodel("15.txt");
//	setcurve("15_242.txt", &slice[0], tx, ty, tz, 0);
	setcurve("15_247.txt", &slice[0], tx, ty, tz, 1);
	setcurve("15_252.txt", &slice[1], tx, ty, tz, 2);
	setcurve("15_257.txt", &slice[2], tx, ty, tz, 3);
	setcurve("15_262.txt", &slice[3], tx, ty, tz, 4);
	setcurve("15_267.txt", &slice[4], tx, ty, tz, 5);
	setcurve("15_272.txt", &slice[5], tx, ty, tz, 6);
	setcurve("15_277.txt", &slice[6], tx, ty, tz, 7);
	setcurve("15_282.txt", &slice[7], tx, ty, tz, 8);
	setcurve("15_287.txt", &slice[8], tx, ty, tz, 9);
	setcurve("15_292.txt", &slice[9], tx, ty, tz, 10);
	setcurve("15_297.txt", &slice[10], tx, ty, tz, 11);
	setcurve("15_302.txt", &slice[11], tx, ty, tz, 12);
	setcurve("15_307.txt", &slice[12], tx, ty, tz, 13);
	setcurve("15_312.txt", &slice[13], tx, ty, tz, 14);
	setcurve("15_317.txt", &slice[14], tx, ty, tz, 15);
	setcurve("15_322.txt", &slice[15], tx, ty, tz, 16);
	setcurve("15_327.txt", &slice[16], tx, ty, tz, 17);
	setcurve("15_332.txt", &slice[17], tx, ty, tz, 18);
	setcurve("15_337.txt", &slice[18], tx, ty, tz, 19);
	setcurve("15_342.txt", &slice[19], tx, ty, tz, 20);
	setcurve("15_347.txt", &slice[20], tx, ty, tz, 21);
//	setcurve("15_350.txt", &slice[22], tx, ty, tz, 22);

	/*setcurve("2_4_slice0.txt", &slice[0], tx, ty, tz);
	setcurve("2_4_slice1.txt", &slice[1], tx, ty, tz);
	setcurve("2_4_slice2.txt", &slice[2], tx, ty, tz);
	setcurve("2_4_slice3.txt", &slice[3], tx, ty, tz);
	setcurve("2_4_slice4.txt", &slice[4], tx, ty, tz);
	setcurve("2_4_slice5.txt", &slice[5], tx, ty, tz);
	setcurve("2_4_slice6.txt", &slice[6], tx, ty, tz);
	setcurve("2_4_slice7.txt", &slice[7], tx, ty, tz);
	setcurve("2_4_slice8.txt", &slice[8], tx, ty, tz);
	setcurve("2_4_slice9.txt", &slice[9], tx, ty, tz);*/

	/*setcurve("2_8_slice0.txt", &slice[0], tx, ty, tz);
	setcurve("2_8_slice1.txt", &slice[1], tx, ty, tz);
	setcurve("2_8_slice2.txt", &slice[2], tx, ty, tz);
	setcurve("2_8_slice3.txt", &slice[3], tx, ty, tz);
	setcurve("2_8_slice4.txt", &slice[4], tx, ty, tz);
	setcurve("2_8_slice5.txt", &slice[5], tx, ty, tz);
	setcurve("2_8_slice6.txt", &slice[6], tx, ty, tz);
	setcurve("2_8_slice7.txt", &slice[7], tx, ty, tz);
	setcurve("2_8_slice8.txt", &slice[8], tx, ty, tz);
	setcurve("2_8_slice9.txt", &slice[9], tx, ty, tz);
	setcurve("2_8_slice10.txt", &slice[10], tx, ty, tz);*/

    /*setcurve("2_18_slice0.txt", &slice[0], tx, ty, tz);
	setcurve("2_18_slice1.txt", &slice[1], tx, ty, tz);
	setcurve("2_18_slice2.txt", &slice[2], tx, ty, tz);
	setcurve("2_18_slice3.txt", &slice[3], tx, ty, tz);
	setcurve("2_18_slice4.txt", &slice[4], tx, ty, tz);
	setcurve("2_18_slice5.txt", &slice[5], tx, ty, tz);
	setcurve("2_18_slice6.txt", &slice[6], tx, ty, tz);
	setcurve("2_18_slice7.txt", &slice[7], tx, ty, tz);
	setcurve("2_18_slice8.txt", &slice[8], tx, ty, tz);
	setcurve("2_18_slice9.txt", &slice[9], tx, ty, tz);
	setcurve("2_18_slice10.txt", &slice[10], tx, ty, tz);*/

	/*setcurve("3_3_slice0.txt", &slice[0], tx, ty, tz);
	setcurve("3_3_slice1.txt", &slice[1], tx, ty, tz);
	setcurve("3_3_slice2.txt", &slice[2], tx, ty, tz);
	setcurve("3_3_slice3.txt", &slice[3], tx, ty, tz);
	setcurve("3_3_slice4.txt", &slice[4], tx, ty, tz);
	setcurve("3_3_slice5.txt", &slice[5], tx, ty, tz);
	setcurve("3_3_slice6.txt", &slice[6], tx, ty, tz);
	setcurve("3_3_slice7.txt", &slice[7], tx, ty, tz);
    setcurve("3_3_slice8.txt", &slice[8], tx, ty, tz);*/	
}

void showmodel()
{
	glPushMatrix();
	glColor3f(0.3, 0.3, 0.3);
	for (int i = 0; i < numFaces - 1; i++)
	{
		glBegin(GL_LINE_LOOP);
		glVertex3f(vertices[faces[i][0]][X], vertices[faces[i][0]][Y], vertices[faces[i][0]][Z]);
		glVertex3f(vertices[faces[i][1]][X], vertices[faces[i][1]][Y], vertices[faces[i][1]][Z]);
		glVertex3f(vertices[faces[i][2]][X], vertices[faces[i][2]][Y], vertices[faces[i][2]][Z]);
		glEnd();
	}
	glPopMatrix();
}

//20170731
void jiaodian(int level1, int level2)
{
	glPointSize(5.0);
	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_POINTS);
	glVertex3f(slice[level2].center[X], slice[level2].center[Y], slice[level2].center[Z]-90);
	glEnd();
	glPointSize(1.0);
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

		//cur_average
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
			cur_mid1[j].control_pts[i][Y] = cur_mid[j].control_pts[i][Y] - vector[j][i][Y] + 0.07 * rate[j] * dist[j][Y];
			cur_mid1[j].control_pts[i][Z] = cur_mid[j].control_pts[i][Z] - vector[j][i][Z];

			cur_mid2[j].control_pts[i][X] = cur_mid[j].control_pts[i][X] + vector[j][i][X] + 0.07 * rate[j] * dist[j][X];
			cur_mid2[j].control_pts[i][Y] = cur_mid[j].control_pts[i][Y] + vector[j][i][Y] + 0.08 * rate[j] * dist[j][Y];
			cur_mid2[j].control_pts[i][Z] = cur_mid[j].control_pts[i][Z] + vector[j][i][Z];
		}
		glColor3f(1.0, 0.0, 1.0);
		glPointSize(5.0);
	}
	//	//ave point
	//	glBegin(GL_POINTS);
	//	for (int i = 0; i < 4; i++)
	//	{
	//	    // j is index of surface/curve, i is index of control pt
	//		//1.cur0's ctrl0 and cur3's ctrl3
	//		glVertex3fv(cur_mid1[0].control_pts[0]);
	//		glVertex3fv(cur_mid1[3].control_pts[3]);

	//		glVertex3fv(cur_mid2[0].control_pts[0]);
	//		glVertex3fv(cur_mid2[3].control_pts[3]);
	//		//2. cur0's ctrl3 and cur1's ctrl0
	//		glVertex3fv(cur_mid1[0].control_pts[3]);
	//		glVertex3fv(cur_mid1[1].control_pts[0]);

	//		glVertex3fv(cur_mid2[0].control_pts[3]);
	//		glVertex3fv(cur_mid2[1].control_pts[0]);
	//		//3. cur1's ctrl3 and cur2's ctrl0
	//		glVertex3fv(cur_mid1[1].control_pts[3]);
	//		glVertex3fv(cur_mid1[2].control_pts[0]);

	//		glVertex3fv(cur_mid2[1].control_pts[3]);
	//		glVertex3fv(cur_mid2[2].control_pts[0]);
	//		//4. cur2's ctrl3 and cur3's ctrl0
	//		glVertex3fv(cur_mid1[2].control_pts[3]);
	//		glVertex3fv(cur_mid1[3].control_pts[0]);

	//		glVertex3fv(cur_mid2[2].control_pts[3]);
	//		glVertex3fv(cur_mid2[3].control_pts[0]);
	//	}
	//	glEnd();	

	//avoid different average point
	for (i = 0; i < 2; i++)
	{
		cur_mid1[3].control_pts[3][i] = cur_mid1[0].control_pts[0][i] = 0.5 * (cur_mid1[0].control_pts[0][i] + cur_mid1[3].control_pts[3][i]);
		cur_mid2[3].control_pts[3][i] = cur_mid2[0].control_pts[0][i] = 0.5 * (cur_mid2[0].control_pts[0][i] + cur_mid2[3].control_pts[3][i]);

		cur_mid1[0].control_pts[3][i] = cur_mid1[1].control_pts[0][i] = 0.5 * (cur_mid1[0].control_pts[3][i] + cur_mid1[1].control_pts[0][i]);
		cur_mid2[0].control_pts[3][i] = cur_mid2[1].control_pts[0][i] = 0.5 * (cur_mid2[0].control_pts[3][i] + cur_mid2[1].control_pts[0][i]);

		cur_mid1[1].control_pts[3][i] = cur_mid1[2].control_pts[0][i] = 0.5 * (cur_mid1[1].control_pts[3][i] + cur_mid1[2].control_pts[0][i]);
		cur_mid2[1].control_pts[3][i] = cur_mid2[2].control_pts[0][i] = 0.5 * (cur_mid2[1].control_pts[3][i] + cur_mid2[2].control_pts[0][i]);

		cur_mid1[2].control_pts[3][i] = cur_mid1[3].control_pts[0][i] = 0.5 * (cur_mid1[2].control_pts[3][i] + cur_mid1[3].control_pts[0][i]);
		cur_mid2[2].control_pts[3][i] = cur_mid2[3].control_pts[0][i] = 0.5 * (cur_mid2[2].control_pts[3][i] + cur_mid2[3].control_pts[0][i]);
	}

	//20170726 3:30
	for (j = 0; j < 4; j++)
	{
		for (i = 0; i < 4; i++)
		{
			SET_PT3(surface[j].control_pts[0][i], slice[level1].cur[j].control_pts[i][X], slice[level1].cur[j].control_pts[i][Y], slice[level1].z);
			SET_PT3(surface[j].control_pts[1][i], cur_mid1[j].control_pts[i][X], cur_mid1[j].control_pts[i][Y], cur_mid1[j].control_pts[0][Z]);
			SET_PT3(surface[j].control_pts[2][i], cur_mid2[j].control_pts[i][X], cur_mid2[j].control_pts[i][Y], cur_mid2[j].control_pts[0][Z]);
			SET_PT3(surface[j].control_pts[3][i], slice[level2].cur[j].control_pts[i][X], slice[level2].cur[j].control_pts[i][Y], slice[level2].z);
		}
	}
    calc_surface();

	if (step_1)
	{
		glColor3f(0.0, 0.0, 0.0);
		glPointSize(5.0);
		//ave point


		glBegin(GL_POINTS);
		for (int j = 0; j < 4; j++)
			for (int i = 0; i < 4; i++)
				glVertex3fv(slice[level1].cur[j].control_pts[i]);
		glEnd();
	}
	if (step_2)
	{
		glColor3f(1.0, 0.0, 1.0);
		glPointSize(5.0);
		//ave point
		glBegin(GL_POINTS);
		for (int j = 0; j < 4; j++)
		{
			for (int i = 0; i < 4; i++)
			{
				glVertex3fv(cur_mid1[j].control_pts[i]);
				glVertex3fv(cur_mid2[j].control_pts[i]);
			}
		}
		glEnd();
	}

	if (step_3)
	{
	}

	if (showpt)
	{
		//center_mid point
		/*
		glBegin(GL_POINTS);
		glVertex3f(center_mid[X], center_mid[Y], center_mid[Z]);
		glEnd();*/
		glColor3f(1.0, 0.0, 0.0);
		glPointSize(5.0);
		//ave point
		glBegin(GL_POINTS);
		for (int j = 0; j < 4; j++)
			glVertex3f(ave[j][X], ave[j][Y], ave[j][Z]);
		glEnd();

		//draw distance
		/*glBegin(GL_LINES);
		for (int j = 0; j < 4; j++)
		{
			glVertex3f(center_mid[X], center_mid[Y], center_mid[Z]);
			glVertex3f(ave[j][X], ave[j][Y], ave[j][Z]);
		}
		glEnd();*/
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
	for (int i = 0; i < SNUM; i++)
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

	//show real model
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

	showmodel();

	glColor3f(0.7, 0.7, 0.7);
	if (showsurface)
	{
		for (int i = 0; i < SNUM - 1; i++)
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

void specialkey(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_F1:
		step_1 = (step_1 + 1) % 2;
		break;
	case GLUT_KEY_F2:
		step_2 = (step_2 + 1) % 2;
		break;
	case GLUT_KEY_F3:
		step_3 = (step_3 + 1) % 2;
		break;
	case GLUT_KEY_LEFT:
		if (mode > 0)
			mode = (mode - 1) % SNUM;
		else
			mode = (mode - 1 + SNUM) % SNUM;
		printf("slice[%d]-vertice number: %d, face number: %d\n", mode, slice[mode].verticenum, slice[mode].facenum);
		break;
	case GLUT_KEY_RIGHT:
		mode = (mode + 1) % SNUM;
		printf("slice[%d]-vertice number: %d, face number: %d\n", mode, slice[mode].verticenum, slice[mode].facenum);
		break;

	default:
		break;
	}
	glutPostRedisplay();
}

void keyboard_callback(unsigned char key, int x, int y)
{
	switch (key)
	{
		//print control points of each curve
	case 'p':
		showpt = (showpt + 1) % 2;
		break;
	case 'c':
		showslice = (showslice + 1) % 2;
		break;
	case 's':
		showsurface = (showsurface + 1) % 2;
		break;

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
	glutSpecialFunc(specialkey);
	glutMouseFunc(mouse_callback);
	glutMotionFunc(mouse_move_callback);
	glutDisplayFunc(display_callback);
	glutMainLoop();
	return 0;
}

#include "curve.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <GL/glut.h>

enum XYZ { X = 0, Y, Z };

#define COPY_PT(DST, SRC)               do { (DST)[X] = SRC[X]; (DST)[Y] = SRC[Y]; } while (0)
#define SET_VECTOR2(V, V1, V2)          do { (V)[X] = (V1); (V)[Y] = (V2); } while (0)
#define VECTOR2_X_SCALA_ADD(O, V, S)    do { O[X] += (S) * (V)[X]; O[Y] += (S) * (V)[Y]; } while (0)
#define VECTOR3_X_SCALA_ADD(O, V, S)    do { O[X] += (S) * (V)[X]; O[Y] += (S) * (V)[Y]; O[Z] += (S) * (V)[Z]; } while (0)

#ifdef DEBUG
void PRINT_CTRLPTS(CubicBezierCurve* crv) {
	int i;
	printf("curve %p\n[\n", crv);
	for (i=0; i<4; ++i)
		printf("[%f, %f]\n", crv->control_pts[i][X], crv->control_pts[i][Y]);
	printf("]\n");
}
#endif

void evaluate_curve(const CubicBezierCurve *curve, const REAL t, Point2 value)
{
	const REAL t_inv = 1.0f - t;
	const REAL t_inv_sq = t_inv * t_inv;
	const REAL t_sq = t * t;
	const REAL b0 = t_inv_sq * t_inv;
	const REAL b1 = 3 * t_inv_sq * t;
	const REAL b2 = 3 * t_inv * t_sq;
	const REAL b3 = t_sq * t;
	SET_VECTOR2(value, 0, 0);
	VECTOR2_X_SCALA_ADD(value, curve->control_pts[0], b0);
	VECTOR2_X_SCALA_ADD(value, curve->control_pts[1], b1);
	VECTOR2_X_SCALA_ADD(value, curve->control_pts[2], b2);
	VECTOR2_X_SCALA_ADD(value, curve->control_pts[3], b3);
}

void evaluate_surface(const BicubicBezierSurface *curve, const float t1, const float t2, Point3 value)
{
	const float t1_inv = 1.0f - t1;
	const float t1_inv_sq = t1_inv * t1_inv;
	const float t1_sq = t1 * t1;
	const float t2_inv = 1.0f - t2;
	const float t2_inv_sq = t2_inv * t2_inv;
	const float t2_sq = t2 * t2;

	float b[2][4];
	b[0][0] = t1_inv_sq * t1_inv;
	b[0][1] = 3 * t1_inv_sq * t1;
	b[0][2] = 3 * t1_inv * t1_sq;
	b[0][3] = t1_sq * t1;
	b[1][0] = t2_inv_sq * t2_inv;
	b[1][1] = 3 * t2_inv_sq * t2;
	b[1][2] = 3 * t2_inv * t2_sq;
	b[1][3] = t2_sq * t2;

	SET_PT3(value, 0, 0, 0);
	for (int i = 0; i < 4; i++)
	for (int j = 0; j < 4; j++)
		VECTOR3_X_SCALA_ADD(value, curve->control_pts[i][j], b[0][i] * b[1][j]);
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

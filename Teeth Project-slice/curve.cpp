#include "curve.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

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

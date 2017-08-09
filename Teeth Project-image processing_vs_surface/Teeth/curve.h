#ifndef _CURVE_H_
#define _CURVE_H_

#define PRECISION   1e-5
#define EPS         1e-6        /* data type is float */

typedef float REAL;

typedef REAL Point3[3];
typedef REAL Point2[2];
typedef int Face3[3];

typedef struct CubicBezierCurve {
	Point3 control_pts[4];
} CubicBezierCurve;

typedef struct BicubicBezierSurface
{
	Point3 control_pts[4][4];
} BicubicBezierSurface;

typedef struct sliceboundary
{
	CubicBezierCurve cur[4];             //one slice boundary consists of 4 curves
	Point3 vertices[5000];
	Point3 center;
	Face3 faces[10000];
	int verticenum = 0;
	int facenum = 0;
	REAL z, xmin, xmax, ymin, ymax;      //to draw the bound box 
}SB;

#ifdef DEBUG
void PRINT_CTRLPTS(CubicBezierCurve* crv);
#else
#   define PRINT_CTRLPTS(X)
#endif

#define SET_PT2(V, V1, V2) do { (V)[0] = (V1); (V)[1] = (V2); } while (0)
#define SET_PT3(V, V1, V2, V3) do { (V)[0] = (V1); (V)[1] = (V2); (V)[2] = (V3); } while (0)

void DrawCruve(CubicBezierCurve curve, int x);
void evaluate_curve(const CubicBezierCurve *curve, const REAL t, Point2 value);
void evaluate_surface(const BicubicBezierSurface *curve, const float t1, const float t2, Point3 value);

#endif /* _CURVE_H_ */

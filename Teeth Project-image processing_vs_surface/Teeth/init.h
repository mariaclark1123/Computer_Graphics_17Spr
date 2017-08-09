#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "curve.h"

enum XY { X = 0, Y, Z };

void setcurve(char* filename, SB* slice, float tx, float ty, float tz, int mode)
{
	int numVertex = 0;
	int numFaces = 0;

	float x_max, xmax_y, x_min, xmin_y, y_max, ymax_x, y_min, ymin_x, z;
	Point3 start[4], middle[4], end[4], ctrlp_2[4], ctrlp_3[4];

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
	//back to start point of file
	rewind(fp);

	//set slice's facenum and verticenum
	slice->verticenum = numVertex;
	slice->facenum = numFaces;

	//printf("vertice is %d\n", numVertex);
	//printf("face is %d\n", numFaces);

	numVertex = 0;
	numFaces = 0;

	while (!feof(fp))
	{
		fgets(line, 256, fp);
		if (line[0] == 'v' && line[1] == ' ')
		{
			fseek(fp, -(strlen(line) + 1), SEEK_CUR);		//한줄 앞으로

			float x, y, z;
			fscanf(fp, "%s %f %f %f", line, &x, &y, &z);

			//let x,y,z bigger than 0
			/*slice->vertices[numVertex][X] = (x - tx) * 30 + 200;
			slice->vertices[numVertex][Y] = (y - ty) * 30 + 200;
			slice->vertices[numVertex][Z] = (z - tz) * 30;*/

			//20170808 special case z = z + mode * 5
			slice->vertices[numVertex][X] = (x - tx) * 6 + 200;
			slice->vertices[numVertex][Y] = (y - ty) * 6 + 200;
			slice->vertices[numVertex][Z] = (z - tz + mode * 5) * 6;

			numVertex++;
		}
		
		else if (line[0] == 'f')
		{
			fseek(fp, -(strlen(line) + 1), SEEK_CUR);		    //한줄 앞으로
																//printf("length of line is %d\n", strlen(line));

			int x1, x2, y1, y2, z1, z2;
			fscanf(fp, "%s %d//%d %d//%d %d//%d", line, &x1, &x2, &y1, &y2, &z1, &z2);

			//면의 정점                                                          순서 정보
			slice->faces[numFaces][X] = x1 - 1;
			slice->faces[numFaces][Y] = y1 - 1;
			slice->faces[numFaces][Z] = z1 - 1;
			numFaces++;
		}
	}

	x_max = x_min = ymax_x = ymin_x = slice->vertices[0][X];
	y_max = y_min = xmax_y = xmin_y = slice->vertices[0][Y];

	//20170808 special case z = z + mode * 5
	z = slice->vertices[0][Z];
	//	printf("z is %f\n", z);

	for (int i = 1; i < numVertex; i++)
	{
		if (x_max <= slice->vertices[i][X])
		{
			x_max = slice->vertices[i][X];
			xmax_y = slice->vertices[i][Y];
		}
		if (x_min >= slice->vertices[i][X])
		{
			x_min = slice->vertices[i][X];
			xmin_y = slice->vertices[i][Y];
		}
		if (y_max <= slice->vertices[i][Y])
		{
			y_max = slice->vertices[i][Y];
			ymax_x = slice->vertices[i][X];
		}
		if (y_min >= slice->vertices[i][Y])
		{
			y_min = slice->vertices[i][Y];
			ymin_x = slice->vertices[i][X];
		}
	}
	//curve1
	start[0][0] = x_min;
	start[0][1] = xmin_y;

	middle[0][0] = x_min;
	middle[0][1] = y_max;

	end[0][0] = ymax_x;
	end[0][1] = y_max;
	//curve2
	start[1][0] = ymax_x;
	start[1][1] = y_max;

	middle[1][0] = x_max;
	middle[1][1] = y_max;

	end[1][0] = x_max;
	end[1][1] = xmax_y;
	//curve3
	start[2][0] = x_max;
	start[2][1] = xmax_y;

	middle[2][0] = x_max;
	middle[2][1] = y_min;

	end[2][0] = ymin_x;
	end[2][1] = y_min;
	//curve4
	start[3][0] = ymin_x;
	start[3][1] = y_min;

	middle[3][0] = x_min;
	middle[3][1] = y_min;

	end[3][0] = x_min;
	end[3][1] = xmin_y;

	for (int i = 0; i < 4; i++)
	{
		ctrlp_2[i][0] = 0.333*start[i][X] + 0.666*middle[i][X];
		ctrlp_2[i][1] = 0.333*start[i][Y] + 0.666*middle[i][Y];

		ctrlp_3[i][0] = 0.666*middle[i][X] + 0.333*end[i][X];
		ctrlp_3[i][1] = 0.666*middle[i][Y] + 0.333*end[i][Y];

		SET_PT3(slice->cur[i].control_pts[0], start[i][0], start[i][1], z);
		SET_PT3(slice->cur[i].control_pts[1], ctrlp_2[i][0], ctrlp_2[i][1], z);
		SET_PT3(slice->cur[i].control_pts[2], ctrlp_3[i][0], ctrlp_3[i][1], z);
		SET_PT3(slice->cur[i].control_pts[3], end[i][0], end[i][1], z);
	}

	slice->z = z;
	slice->xmin = x_min;
	slice->xmax = x_max;
	slice->ymin = y_min;
	slice->ymax = y_max;

	slice->center[X] = 0.5*(x_min + x_max);
	slice->center[Y] = 0.5*(y_min + y_max);
	slice->center[Z] = z;

	fclose(fp);
}
/*
*  main.cpp
*  Computer Graphics - HW2(FBO)
*
*  Created by Fangda Chen (mariaclark1123@snu.ac.kr)
*  CREATED: 2017-04-22
*/

#include <iostream>
#include <cstdlib>
#include <sstream>
//#include <GL/glew.h>
//#include <GL/glut.h>
#include "include/GL/glew.h"
#include "include/GL/glut.h"

using namespace std;

#pragma comment(lib, "glew32.lib")

int choose = 0;
int screenwidth = 500;
int screenheight = 500;
float aspect = (float)screenwidth / (float)screenheight;
const int texturewidth = 256;
const int textureheight = 256;
GLfloat spin = 30.f;

// global variables
GLuint fboID;                       // ID of FBO
GLuint textureID;                   // ID of texture
GLuint rbID;                        // IDs of Renderbuffer
void *font = GLUT_BITMAP_8_BY_13;

//cube with 8 faces  
GLfloat cube[24][3] = {
	{ 0.5f, -0.5f, -0.5f },{ 0.5f, 0.5f, -0.5f },{ 0.5f, 0.5f, 0.5f },{ 0.5f, -0.5f, 0.5f },      //front
	{ -0.5f, -0.5f, -0.5f },{ -0.5f, 0.5f, -0.5f },{ -0.5f, 0.5f, 0.5f },{ -0.5f, -0.5f, 0.5f },  //back
	{ 0.5f, -0.5f, 0.5f },{ 0.5f, 0.5f, 0.5f },{ -0.5f, 0.5f, 0.5f },{ -0.5f, -0.5f, 0.5f },      //up
	{ 0.5f, -0.5f, -0.5f },{ 0.5f, 0.5f, -0.5f },{ -0.5f, 0.5f, -0.5f },{ -0.5f, -0.5f, -0.5f },  //down
	{ 0.5f, -0.5f, -0.5f },{ -0.5f, -0.5f, -0.5f },{ -0.5f, -0.5f, 0.5f },{ 0.5f, -0.5f, 0.5f },  //left
	{ 0.5f, 0.5f, -0.5f },{ -0.5f, 0.5f, -0.5f },{ -0.5f, 0.5f, 0.5f },{ 0.5f, 0.5f, 0.5f }       //right
};

///////////////////////////////////////////////////////////////////////////////
// write 2d text using GLUT
// The projection matrix must be set to orthogonal before call this function.
///////////////////////////////////////////////////////////////////////////////
void drawString(const char *str, int x, int y, float color[4], void *font)
{
	glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
	glDisable(GL_LIGHTING);     // need to disable lighting for proper text color
	glDisable(GL_TEXTURE_2D);

	glColor4fv(color);          // set text color
	glRasterPos2i(x, y);        // place text position

								// loop all characters in the string
	while (*str)
	{
		glutBitmapCharacter(font, *str);
		++str;
	}

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glPopAttrib();
}

///////////////////////////////////////////////////////////////////////////////
// display info messages
///////////////////////////////////////////////////////////////////////////////
void showInfo()
{
	// backup current model-view matrix
	glPushMatrix();                     // save current modelview matrix
	glLoadIdentity();                   // reset modelview matrix

										// set to 2D orthogonal projection
	glMatrixMode(GL_PROJECTION);     // switch to projection matrix
	glPushMatrix();                  // save current projection matrix
	glLoadIdentity();                // reset projection matrix
	gluOrtho2D(0, screenwidth, 0, screenheight);  // set to orthogonal projection

	float color[4] = { 1.0, 0.0, 0.1, 1 };

	stringstream ss;
	ss << " Please use space bar to change (MODE1/MODE2/MODE3)" << std::ends;  
	drawString(ss.str().c_str(), 1, screenheight - 15, color, font);
	ss.str(""); // clear buffer

	ss << " MODE: " << choose + 1 << std::ends;  // add 0(ends) at the end
	drawString(ss.str().c_str(), 1, 10, color, font);

	// restore projection matrix
	glPopMatrix();                   // restore to previous projection matrix

									 // restore modelview matrix
	glMatrixMode(GL_MODELVIEW);      // switch to modelview matrix
	glPopMatrix();                   // restore to previous modelview matrix
}

///////////////////////////////////////////////////////////////////////////////
// checkFramebufferStatus
///////////////////////////////////////////////////////////////////////////////
void checkFramebufferStatus(GLuint fbo)
{
	// check FBO status
	glBindFramebuffer(GL_FRAMEBUFFER, fbo); // bind
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	switch (status)
	{
	case GL_FRAMEBUFFER_COMPLETE:
		printf("Framebuffer complete.\n");
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		printf("[ERROR] Framebuffer incomplete : Attachment is NOT complete.\n");
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		printf("[ERROR] Framebuffer incomplete: No image is attached to FBO.\n");
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		printf("[ERROR] Framebuffer incomplete: Draw buffer.\n");
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		printf("[ERROR] Framebuffer incomplete: Read buffer.\n");
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
		printf("[ERROR] Framebuffer incomplete: Multisample.\n");
		break;

	case GL_FRAMEBUFFER_UNSUPPORTED:
		printf("[ERROR] Framebuffer incomplete: Unsupported by FBO implementation.\n");
		break;

	default:
		printf("[ERROR] Framebuffer incomplete: Unknown error.\n");
		break;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);   // unbind
}

///////////////////////////////////////////////////////////////////////////////
// With FBO, render directly to a texture
///////////////////////////////////////////////////////////////////////////////
void InitFBOtexture()
{
	//Generate FBO
	glGenFramebuffers(1, &fboID);

	//Generate texture
	glGenTextures(1, &textureID);

	//Generate renderbuffer
	//glGenRenderbuffers(1, &rbID);
	glGenTextures(1, &rbID);

	//Set the rendering destination to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, fboID);

	//Bind Texture
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texturewidth, textureheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

	glBindTexture(GL_TEXTURE_2D, rbID);    //depth
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, texturewidth, textureheight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, texturewidth, textureheight);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, rbID, 0);
//	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbID);    //Attach render buffer to framebuffer
}

///////////////////////////////////////////////////////////////////////////////
// draw cube
///////////////////////////////////////////////////////////////////////////////
void drawcube()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, screenwidth, screenheight);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, aspect, 1, 10);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_TEXTURE_2D);
	if (choose == 0 | choose == 2)
		glBindTexture(GL_TEXTURE_2D, textureID);
	if (choose == 1)
		glBindTexture(GL_TEXTURE_2D, rbID);

	glDisable(GL_LIGHTING);

	glTranslatef(0, 0, -3);
	glRotatef(spin, 1.0, 1.0, 1.0);

	glBegin(GL_QUADS);
	glColor3f(1.0, 1.0, 1.0);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3fv(cube[0]);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(cube[1]);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3fv(cube[2]);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3fv(cube[3]);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3fv(cube[8]);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(cube[9]);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3fv(cube[10]);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3fv(cube[11]);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3fv(cube[16]);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(cube[17]);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3fv(cube[18]);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3fv(cube[19]);
	glEnd();

	//draw grey teapot
	if (choose == 2)
		glBindTexture(GL_TEXTURE_2D, rbID);

	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3fv(cube[4]);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(cube[5]);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3fv(cube[6]);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3fv(cube[7]);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3fv(cube[12]);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(cube[13]);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3fv(cube[14]);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3fv(cube[15]);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3fv(cube[20]);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(cube[21]);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3fv(cube[22]);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3fv(cube[23]);
	glEnd();
}

///////////////////////////////////////////////////////////////////////////////
// Create rotate teapot texture
///////////////////////////////////////////////////////////////////////////////
void Texteapot()
{
	glViewport(0, 0, texturewidth, textureheight);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, 1, 10);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

	glBindTexture(GL_TEXTURE_2D, 0);
	glEnable(GL_TEXTURE_2D);

	// set the rendering destination to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, fboID);

	//Clear buffer
	glClearColor(1.0, 1.0, 1.0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glTranslatef(0, 0, -4);
	glRotatef(spin, 1.0, 1.0, 1.0);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	glColor3f(1.0, 1.0, 0.0);
	glutSolidTeapot(0.75);
	glDisable(GL_COLOR_MATERIAL);

	glPopMatrix();

	// back to normal window-system-provided framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // unbind
}

void display()
{
	Texteapot();
	drawcube();
	// draw info messages
	showInfo();
	glutSwapBuffers();
}

void reshape(int w, int h)
{
	screenwidth = w;
	screenheight = h;
	aspect = (float)screenwidth / (float)screenheight;
	glutPostRedisplay();
}

void idle()
{
	if (spin < 360)
		spin = spin + 2;
	else
		spin = spin - 360 + 2;
	glutPostRedisplay();
}

void keyboard_callback(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27: // ESCAPE
		exit(0);
		break;

	case ' ':
	{
		choose += 1;
		choose = choose % 3;
		break;
	}

	default:
		break;
	}
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(screenwidth, screenheight);
	glutInitWindowPosition(500, 0);
	glutCreateWindow("CG-HW2");
	glewInit();
	checkFramebufferStatus(fboID);

	InitFBOtexture();

	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard_callback);
	glutMainLoop();
}



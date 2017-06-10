#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <map>
#include <cmath>
#include <cwchar>
#include <cstdio>
#include <ctime>
#include <windows.h>

#include <omp.h>
#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GLFW/glfw3.h"
#include "il_inc.h"

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/ext.hpp"

using namespace std;

#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "freeglut.lib")
#pragma comment (lib, "glfw3dll.lib")

#pragma comment (lib, "glu32.lib")    // link OpenGL Utility lib
#pragma comment (lib, "opengl32.lib") // link Microsoft OpenGL lib

#pragma warning( disable : 4996 ) // disable warning of sprintf,swprintf

bool toggle[3];
bool depth = true;
bool save_shadow = true;
const int lights_num = 3;

GLuint tex_walls, tex_depth[3], tex_shadow[lights_num];
GLuint frame_buffer_s;
const int shadow_w = 2048, shadow_h = 2048;
glm::vec4 light_pos[lights_num]; // 3 lights' world position

//Draw2
glm::mat4 shadow_mat_p; // shadow map projection
glm::mat4 shadow_mat_v[lights_num]; // shadow map view
GLuint tex_sh[lights_num]; // tex_sh[3]
int choose_scene = 1;
bool scene2_tex = true;

bool light_rotate = false;
void *font = GLUT_BITMAP_8_BY_13;

float vertices[5000][3]; //obj file information
int faces[5000][3];
int numVertex;
int numFaces;

class keyFunc { public: void(*f)(); const wchar_t* s; };
static glm::mat4	mat_model, mat_view;// transformation matrix
static glm::mat4	mat_projection;     // projection matrix
static float		speed_scale = 0.2f;	// interactive speed factor
static float		frustum_fovy = 45;	// fovy of frustum
static GLFWwindow*	curr_window;  // current window
static bool			help_display; // should display help content or not
static int          font_size = 16; // font size in pixels
static bool			fps_display = true; // display fps at lover left or not
static std::map<int, keyFunc> key_funcs; // the key-function map

//Draw annotation
void drawString(const char* str)
{
	static int isFirstCall = 1;
	static GLuint lists;

	if (isFirstCall)
	{
		isFirstCall = 0;
		lists = glGenLists(128);

		wglUseFontBitmaps(wglGetCurrentDC(), 0, 128, lists);
	}
	for (; *str != '\0'; ++str)
		glCallList(lists + *str);
}

//file reader
void readfile(char* obj, int* numV, int* numF, float vertices[5000][3], int faces[5000][3])
{
	int numVertex = 0;
	int numFaces = 0;
	char line[256];//get line

				   //load model
	FILE* fp = fopen(obj, "r");

	if (fp == NULL)
	{
		printf("%s file can not open", obj);
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

	*numV = numVertex;
	*numF = numFaces;

	//back to start point of file
	rewind(fp);

	printf("%s's infomation:\n", obj);
	printf("number of v : %d\n", numVertex);
	printf("number of f : %d\n", numFaces);

	int IdxVertex = 0;
	int IdxFace = 0;

	int length;
	while (!feof(fp))
	{
		fgets(line, 256, fp);
		if (line[0] == 'v')
		{
			if (line[1] == ' ')
			{
				length = strlen(line) + 1;
				fseek(fp, -length, SEEK_CUR);		//한줄 앞으로

				float x, y, z;
				fscanf(fp, "%s %f %f %f", line, &x, &y, &z);
				vertices[IdxVertex][0] = x;
				vertices[IdxVertex][1] = y;
				vertices[IdxVertex][2] = z;
				IdxVertex++;
			}
		}

		else if (line[0] == 'f')
		{
			length = strlen(line) + 1;
			fseek(fp, -length, SEEK_CUR);		    //한줄 앞으로

			int x1, x2, y1, y2, z1, z2;
			fscanf(fp, "%s %d//%d %d//%d %d//%d", line, &x1, &x2, &y1, &y2, &z1, &z2);

			faces[IdxFace][0] = x1 - 1;
			faces[IdxFace][1] = y1 - 1;
			faces[IdxFace][2] = z1 - 1;
			IdxFace++;
		}
	}
}

//Draw object
void drawobj(int numV, int numF, float vertices[5000][3], int faces[5000][3])
{
	glPushMatrix();

	glTranslatef(0.0, 1.6, 0.0);
	glScalef(10, 10, 10);
	glColor3f(1.0, 1.0, 1.0);
	for (int i = 0; i < numF - 1; i++)
	{
		float p1[3] = { vertices[faces[i][0]][0], vertices[faces[i][0]][1], vertices[faces[i][0]][2] };
		float p2[3] = { vertices[faces[i][1]][0], vertices[faces[i][1]][1], vertices[faces[i][1]][2] };
		float p3[3] = { vertices[faces[i][2]][0], vertices[faces[i][2]][1], vertices[faces[i][2]][2] };

		glBegin(GL_TRIANGLES);
		glVertex3fv(p1);
		glVertex3fv(p2);
		glVertex3fv(p3);

		glEnd();
	}
	glPopMatrix();
}

//Draw the floor with tex-repeat and subdivision
void floor(float len, float tex_repeat, int subdivision)
{
	GLfloat color[] = { .8f, 0.8f, 0.8f, 1 };
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glScalef(len, len, 1);
	glNormal3f(0, 0, 1);
	float u = 1.0f / subdivision;
	float tu = tex_repeat / subdivision;
	for (int i = 0; i < subdivision; ++i)
		for (int j = 0; j < subdivision; ++j) {
			float ox = u*i, oy = u*j;
			float tox = tu*i, toy = tu*j;
			glBegin(GL_POLYGON);
			glTexCoord2f(tox, toy);
			glVertex3f(ox, oy, 0);
			glTexCoord2f(tox + tu, toy);
			glVertex3f(ox + u, oy, 0);
			glTexCoord2f(tox + tu, toy + tu);
			glVertex3f(ox + u, oy + u, 0);
			glTexCoord2f(tox, toy + tu);
			glVertex3f(ox, oy + u, 0);
			glEnd();
		}
	glPopMatrix();
}

//----------------------------------- get ,  set ----------------------------------
const glm::mat4& get_mat_model() {
	return mat_model;
}
void				set_mat_model(const glm::mat4& mat) {
	mat_model = mat;
}

const glm::mat4& get_mat_view() {
	return mat_view;
}
void				set_mat_view(const glm::mat4& mat) {
	mat_view = mat;
}

const glm::mat4& get_mat_projection() {
	return mat_projection;
}

int  get_frame_width() {
	int width, height;
	glfwGetFramebufferSize(curr_window, &width, &height);
	return width;
}
int  get_frame_height() {
	int width, height;
	glfwGetFramebufferSize(curr_window, &width, &height);
	return height;
}
void get_frame_size(int* width, int* height) {
	glfwGetFramebufferSize(curr_window, width, height);
}

// do not use F1,WSAD,Up,Down,Left,Right,Home,End,PageUp,PageDown
void add_key_callback(int key, void(*func)(), const wchar_t* description)
{
	if (func == 0) return;
	keyFunc kf;
	kf.f = func;
	kf.s = description;
	key_funcs[key] = kf;
}


//----------------------------------- callbacks -----------------------------------

void callback_error(int error, const char* description)
{
	std::cout << "GLFW Error code: " << error
		<< "\t\tDescription: " << description << '\n';
	std::cin.get(); // hold the screen
}
// at pressent, this is the same as window size callback

void callback_frameBufferSize(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	mat_projection = glm::perspective(
		glm::radians(frustum_fovy), float(width) / height, 1.0f, 1.0e10f);
	glLoadMatrixf(&mat_projection[0][0]);
	glMatrixMode(GL_MODELVIEW);
}

void trackball(float* theta, glm::vec3* normal,	float ax, float ay, float bx, float by, float r)
{
	float r2 = r * 0.9f;
	float da = std::sqrt(ax*ax + ay*ay);
	float db = std::sqrt(bx*bx + by*by);
	if (std::max(da, db) > r2) {
		float dx, dy;
		if (da > db) {
			dx = (r2 / da - 1)*ax;
			dy = (r2 / da - 1)*ay;
		}
		else {
			dx = (r2 / db - 1)*bx;
			dy = (r2 / db - 1)*by;
		}
		ax += dx; ay += dy; bx += dx; by += dy;
	}
	float az = std::sqrt(r*r - (ax*ax + ay*ay));
	float bz = std::sqrt(r*r - (bx*bx + by*by));
	glm::vec3 a = glm::vec3(ax, ay, az);
	glm::vec3 b = glm::vec3(bx, by, bz);
	*theta = std::acos(glm::dot(a, b) / (r*r));
	*normal = glm::cross(a, b);
}

// action: press, release, repeat  mods: mod_shift,ctrl,alt
void callback_key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action != GLFW_RELEASE) {
		switch (key)
		{
		case GLFW_KEY_PAGE_UP: {
			glm::vec3 v = glm::vec3(mat_view*glm::vec4(0, 1, 0, 0));
			mat_view = glm::translate(-speed_scale*v) * mat_view; }
			break;
		case GLFW_KEY_PAGE_DOWN: {
			glm::vec3 v = glm::vec3(mat_view*glm::vec4(0, 1, 0, 0));
			mat_view = glm::translate(speed_scale*v) * mat_view; }
			break;

		case GLFW_KEY_S: {
			save_shadow = true;
		}
		break;

		case GLFW_KEY_C: {
			if (choose_scene == 1)
			{
				scene2_tex = true;
				choose_scene = 2;
			}
			else if (choose_scene == 2)
			{
				scene2_tex = true;
				choose_scene = 1;
			}
		}
		break;

		case GLFW_KEY_F1: {
			if (toggle[0] == 0)
				toggle[0] = 1;
			else
				toggle[0] = 0;
		}
		break;

		case GLFW_KEY_F2: {
			if (toggle[1] == 0)
				toggle[1] = 1;
			else
				toggle[1] = 0;
		}
		break;

		case GLFW_KEY_F3: {
			if (toggle[2] == 0)
				toggle[2] = 1;
			else
				toggle[2] = 0;
		}
		break;

		default:
			if (key_funcs.find(key) != key_funcs.end()) {
				if (key_funcs[key].f) (*key_funcs[key].f)();
			}
		}
	}
}

// button: left, right, mid  action: press, release
void callback_mousePress(GLFWwindow* window, int button, int action, int mods)
{
//	if (x > 100)
	{
		if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_RIGHT) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_RIGHT) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
}

void callback_scroll(GLFWwindow* window, double xoffset, double yoffset)
{
	mat_view = glm::translate(glm::vec3(0, 0, -speed_scale * 5 * float(yoffset))) * mat_view;
}

// xpos,ypos: the new xy-coordinate, in screen coordinates, of the cursor
void callback_mouseMove(GLFWwindow* window, double xpos, double ypos)
{
	static double xpos_last, ypos_last;
	ypos = get_frame_height() - ypos; // window use upper left as origin, but gl use lower left
//	if (xpos > 100)
	{
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			float dx = float(xpos - xpos_last), dy = float(ypos - ypos_last);
			if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) != GLFW_RELEASE) { // key left Ctrl is pressed
				mat_view *= glm::rotate(speed_scale / 50 * dx, glm::vec3(0, 1, 0));
				glm::vec3 v = glm::vec3(glm::affineInverse(mat_view)*glm::vec4(1, 0, 0, 0));
				mat_view *= glm::rotate(-speed_scale / 50 * dy, v);
			}
			else {
				float theta; glm::vec3 n;
				int width, height; glfwGetFramebufferSize(window, &width, &height);
				trackball(&theta, &n,
					float(xpos_last) - width / 2.0f, float(ypos_last) - height / 2.0f,
					float(xpos) - width / 2.0f, float(ypos) - height / 2.0f, std::min(width, height) / 4.0f);
				glm::vec3 normal = glm::vec3(
					glm::affineInverse(mat_model) * glm::affineInverse(mat_view)
					* glm::vec4(n.x, n.y, n.z, 0));
				mat_model *= glm::rotate(theta, normal);
			}
		}

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{
			float dx = float(xpos - xpos_last), dy = float(ypos - ypos_last);
			if (dy != 0)
				mat_view = glm::rotate(-speed_scale / 50 * dy, glm::vec3(1, 0, 0)) * mat_view;
			if (dx != 0)
				mat_view = glm::rotate(speed_scale / 50 * dx, glm::vec3(mat_view * glm::vec4(0, 1, 0, 0))) * mat_view;
		}

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
		{
			float dx = float(xpos - xpos_last), dy = float(ypos - ypos_last);
			if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) != GLFW_RELEASE) { // key left Ctrl is pressed
				mat_view = glm::translate(glm::vec3(speed_scale / 5 * dx, speed_scale / 5 * dy, 0)) * mat_view;
			}
			else {
				glm::vec4 v = glm::affineInverse(mat_view) * glm::vec4(dx, dy, 0, 0);
				mat_model = glm::translate(glm::vec3(speed_scale / 5 * v)) * mat_model;
			}
		}
		xpos_last = xpos; ypos_last = ypos;
	}
}

//----------------------------------- utilities -----------------------------------
/* hue:0-360; saturation:0-1; lightness:0-1
* hue:        red(0) -> green(120) -> blue(240) -> red(360)
* saturation: gray(0) -> perfect colorful(1)
* lightness:  black(0) -> perfect colorful(0.5) -> white(1)
*/
void hsl_to_rgb(float h, float s, float l, float* rgb)
{
	if (s == 0) { rgb[0] = rgb[1] = rgb[2] = l; return; }
	float q, p, hk, t[3];
	if (l < 0.5f) { q = l * (1 + s); }
	else { q = l + s - l * s; }
	p = 2 * l - q;
	hk = h / 360;
	t[0] = hk + 1 / 3.0f;
	t[1] = hk;
	t[2] = hk - 1 / 3.0f;
	for (int i = 0; i < 3; ++i) {
		if (t[i] < 0) { t[i] += 1; }
		else if (t[i] > 1) { t[i] -= 1; }
	}
	for (int i = 0; i < 3; ++i) {
		if (t[i] < 1 / 6.0f) { rgb[i] = p + (q - p) * 6 * t[i]; }
		else if (t[i] < 1 / 2.0f) { rgb[i] = q; }
		else if (t[i] < 2 / 3.0f) { rgb[i] = p + (q - p) * 6 * (2 / 3.0f - t[i]); }
		else { rgb[i] = p; }
	}
}

float rgb_to_gray(float r, float g, float b)
{
	return r*0.299f + g*0.587f + b*0.114f;
}

//----------------------------------- initialize ----------------------------------
void init_win(int width, int height, const char* tile, const char* font_file)
{
	// glfw init
	glfwSetErrorCallback(callback_error);
	if (!glfwInit()) {
		std::cout << "GLFW init Error";
		std::cin.get(); // hold the screen
	}

	// create window
	glfwWindowHint(GLFW_SAMPLES, 8); // anti-aliase, the RGBA,depth,stencil are set by default
	const GLFWvidmode* mods = glfwGetVideoMode(glfwGetPrimaryMonitor());
	curr_window = glfwCreateWindow(width, height, tile, 0, 0);
	if (!curr_window) {
		std::cout << "Create window Error";
		std::cin.get(); // hold the screen
	}
	// window at center of screem
	glfwSetWindowPos(curr_window,
		std::max(4, mods->width / 2 - width / 2), std::max(24, mods->height / 2 - height / 2));
	glfwMakeContextCurrent(curr_window);
	glfwSetFramebufferSizeCallback(curr_window, callback_frameBufferSize);
	glfwSetKeyCallback(curr_window, callback_key);
	glfwSetCursorPosCallback(curr_window, callback_mouseMove);
	glfwSetScrollCallback(curr_window, callback_scroll);
	glfwSetMouseButtonCallback(curr_window, callback_mousePress);

	// glut init
	int argc = 0;
	glutInit(&argc, NULL);

	// glew init, have to be after the GL context has been created
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cout << "GLEW init Error: " << glewGetErrorString(err);
		std::cin.get(); // hold the screen
	}

	// image library init
	il_init();
}

void init_light()
{
	GLfloat vec4f[4] = { 0 };
	vec4f[0] = 1.0f; vec4f[1] = 0.2f; vec4f[2] = 0.2f;
	glLightfv(GL_LIGHT0, GL_DIFFUSE, vec4f); // red
	glLightfv(GL_LIGHT0, GL_SPECULAR, vec4f);

	vec4f[0] = 0.2f; vec4f[1] = 1.0f; vec4f[2] = 0.2f;
	glLightfv(GL_LIGHT1, GL_DIFFUSE, vec4f); // green
	glLightfv(GL_LIGHT1, GL_SPECULAR, vec4f);

	vec4f[0] = 0.0f; vec4f[1] = 0.2f; vec4f[2] = 1.0f;
	glLightfv(GL_LIGHT2, GL_DIFFUSE, vec4f); // blue
	glLightfv(GL_LIGHT2, GL_SPECULAR, vec4f);

	vec4f[0] = 0; vec4f[1] = 0; vec4f[2] = 0;
	glLightfv(GL_LIGHT0, GL_AMBIENT, vec4f); // black AMBIENT
	glLightfv(GL_LIGHT1, GL_AMBIENT, vec4f);
	glLightfv(GL_LIGHT2, GL_AMBIENT, vec4f);

	float gac2[4] = { 0.15f,0.15f,0.15f,1 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, gac2);

	light_pos[0] = glm::vec4(5, 6, -2, 1);
	light_pos[1] = glm::vec4(-2, 6, 5, 1);
	light_pos[2] = glm::vec4(-3, 6, -3, 1);
}

void init_gl()
{
	toggle[0] = 1;
	toggle[1] = 1;
	toggle[2] = 1;

	// projection matrix
	glMatrixMode(GL_PROJECTION);
	int w, h; get_frame_size(&w, &h);

	mat_projection = glm::perspective(glm::radians(frustum_fovy), float(w) / h, 1.0f, 1.0e10f);
	glLoadMatrixf(&mat_projection[0][0]);

	// model-view matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// color
	glClearColor(0, 0, 0.25f, 1);
	glColor4f(.5f, .5f, .5f, 1);
	glShadeModel(GL_SMOOTH);

	// material
	GLfloat c[] = { .7f, .7f, .7f, 1 };
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, c); // front, gray
	c[0] = .4f; c[1] = .4f; c[2] = .4f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, c);
	glMaterialf(GL_FRONT, GL_SHININESS, 50);
	c[0] = 0; c[1] = 0; c[2] = 0;
	glMaterialfv(GL_BACK, GL_AMBIENT_AND_DIFFUSE, c); // back, black

	// lighting, light0
	GLfloat vec4f[] = { 1, 1, 1, 1 };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, vec4f); // white DIFFUSE, SPECULAR
	glLightfv(GL_LIGHT0, GL_SPECULAR, vec4f);
	vec4f[0] = .0f; vec4f[1] = .0f; vec4f[2] = .0f;
	glLightfv(GL_LIGHT0, GL_AMBIENT, vec4f); // black AMBIENT
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); // LOCAL_VIEWER
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE); // single side
	vec4f[0] = 0.25f; vec4f[1] = 0.25f; vec4f[2] = 0.25f;
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, vec4f); // global AMBIENT lighting, gray

	//glEnable(GL_CULL_FACE); glCullFace(GL_BACK); glFrontFace(GL_CCW);
	glDisable(GL_CULL_FACE);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

	// blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_NORMALIZE);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
}

void renderLoop(void(*draw1)(const glm::mat4&, const glm::mat4&), void(*draw2)(const glm::mat4&, const glm::mat4&))
{
	static double t1, t2, t3;
#define TIME_START(n) t##n=omp_get_wtime()
#define TIME_END(n)   t##n=omp_get_wtime()-t##n
#define TIME_TEXT(n)  {char st[50]; \

	while (!glfwWindowShouldClose(curr_window))
	{
		TIME_START(1);
		// draw function
		if(choose_scene == 1)	
			draw1(mat_model, mat_view);
		if (choose_scene == 2)
			draw2(mat_model, mat_view);

		TIME_START(2);
		// swap buffers and poll events
		glfwSwapBuffers(curr_window);
		TIME_END(2);

		TIME_START(3);
		glfwPollEvents();
		TIME_END(3);
	}
	// no more events will be delivered for that window and its handle becomes invalid
//	glfwDestroyWindow(curr_window);

	// destroys all remaining windows, frees any allocated resources and into an uninitialized
//	glfwTerminate();
}


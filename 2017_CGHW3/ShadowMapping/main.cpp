#include "setup.h"

/*
glActiveTexture(GL_TEXTURE0) Ordinary texture
glActiveTexture(GL_TEXTURE1) Shadow texture
*/

//Initialize texture
void tex_init()
{
	glActiveTexture(GL_TEXTURE0); // ordinary texture: GL_TEXTURE0
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	void* data;
	int w, h;

	il_readImg(L"d0.png", &data, &w, &h);
	glGenTextures(1, &tex_depth[0]);
	glBindTexture(GL_TEXTURE_2D, tex_depth[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	delete data;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


	il_readImg(L"d1.png", &data, &w, &h);
	glGenTextures(1, &tex_depth[1]);
	glBindTexture(GL_TEXTURE_2D, tex_depth[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	delete data;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


	il_readImg(L"d2.png", &data, &w, &h);
	glGenTextures(1, &tex_depth[2]);
	glBindTexture(GL_TEXTURE_2D, tex_depth[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	delete data;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	il_readImg(L"world.png", &data, &w, &h);
	glGenTextures(1, &tex_walls);
	glBindTexture(GL_TEXTURE_2D, tex_walls);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	delete data;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glActiveTexture(GL_TEXTURE1); // shadow texuture: GL_TEXTURE1
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
	glEnable(GL_TEXTURE_GEN_Q);

	glGenTextures(lights_num, tex_shadow);
	for (int i = 0; i < lights_num; ++i) {
		glBindTexture(GL_TEXTURE_2D, tex_shadow[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		GLfloat c[4] = { 1,1,1, 1 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, c);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_w, shadow_h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	}

	glActiveTexture(GL_TEXTURE2); // shadow texuture: GL_TEXTURE1
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	for (int i = 0; i < lights_num; i++)
	{
		glGenTextures(1, &tex_sh[i]);
		glBindTexture(GL_TEXTURE_2D, tex_sh[i]);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
	}

	// frame buffer object
	glGenFramebuffers(1, &frame_buffer_s);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame_buffer_s);
	//	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex_shadow, 0);
}

void dlight(glm::vec4& light_pos, int index) // draw a sphere in light position
{
	GLboolean li = glIsEnabled(GL_LIGHTING);
	if (li)
		glDisable(GL_LIGHTING);
	GLfloat cc[4];
	glGetFloatv(GL_CURRENT_COLOR, cc);
	if(index == 0)
		glColor3f(1.0f, 0.2f, 0.2f);
	else if (index == 1)
		glColor3f(0.2f, 1.0f, 0.2f);
	else if (index == 2)
		glColor3f(0.2f, 0.2f, 1.0f);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(light_pos[0], light_pos[1], light_pos[2]);
	//draw sphere
	if (index < 2 && toggle[index])
		glutSolidSphere(0.1f, 50, 50);
	else if (index == 2 && toggle[index])
		glutSolidCube(0.2f);
	glPopMatrix();
	if (li)
		glEnable(GL_LIGHTING);
	glColor4fv(cc);
}

//Draw a world, a wood floor
void draw_world()
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glRotatef(90, -1, 0, 0);
	glTranslatef(-10, -10, 0);
	floor(20, 3, 100);
	glPopMatrix();
}

//Draw all models upon wood floor
void draw_model()
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	drawobj(numVertex, numFaces, vertices, faces);
	glTranslatef(0, 0, 0);
	glutSolidCube(2);

	glTranslatef(2, 0, 2);
	glRotatef(-90, 1, 0, 0);
	glutSolidCone(1, 2, 50, 50);
	glRotatef(90, 1, 0, 0);

	glTranslatef(0, 0, -4);
	glutSolidSphere(1, 50, 50);

	glTranslatef(-4, 1, 0);
	glRotatef(45, 1, 0, 0);
	glutSolidTorus(0.5f, 1, 50, 50);
	glRotatef(-45, 1, 0, 0);

	glPopMatrix();
}

void draw_tex() // draw a floor in the world
{
	glm::vec4 v1(-30, 0, -30, 1), v2(-30, 0, 30, 1), v3(30, 0, 30, 1), v4(30, 0, -30, 1);//四个顶点
//	glm::vec4 v1(-50, -50, 0, 1), v2(-50, 50, 0, 1), v3(50, 50, 0, 1), v4(50, -50, 0, 1);//四个顶点
	glm::mat4 m = glm::translate(glm::vec3(0.5f, 0.5f, 0.5f)) * glm::scale(glm::vec3(0.5f, 0.5f, 0.5f)); // change clipping coordinate[-1,+1] to [0,1]
	glm::vec4 t;
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_POLYGON);
	glNormal3f(0, 1, 0);
	t = m*shadow_mat_p*shadow_mat_v[0] * v1; // caculate texture coordinate
	glTexCoord4fv(&t[0]); glVertex3fv(&v1[0]);
	t = m*shadow_mat_p*shadow_mat_v[0] * v2;
	glTexCoord4fv(&t[0]); glVertex3fv(&v2[0]);
	t = m*shadow_mat_p*shadow_mat_v[0] * v3;
	glTexCoord4fv(&t[0]); glVertex3fv(&v3[0]);
	t = m*shadow_mat_p*shadow_mat_v[0] * v4;
	glTexCoord4fv(&t[0]); glVertex3fv(&v4[0]);
	glEnd();

	glColor3f(1.0, 1.0, 1.0);
}


void draw2(const glm::mat4& mat_model, const glm::mat4& mat_view)
{
	if (scene2_tex == true)
	{
		scene2_tex = false;
		tex_init();
	}

	//very important
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//shadow[lights_num]
	for (int i = 0; i < lights_num; i++)
	{
		glDisable(GL_LIGHTING);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shadow_mat_p = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, 1.0e10f);
		shadow_mat_v[i] = glm::lookAt(glm::vec3(light_pos[i]), glm::vec3(0), glm::vec3(0, 1, 0));

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadMatrixf(&shadow_mat_p[0][0]); 
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadMatrixf(&shadow_mat_v[i][0][0]); 


		glMultMatrixf(&mat_model[0][0]);
		draw_model();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		glBindTexture(GL_TEXTURE_2D, tex_sh[i]);
		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, 0, get_frame_width(), get_frame_height(), 0);
		glEnable(GL_TEXTURE_2D); 
	}

	//divide windows
//	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, 1000, 800);
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor3f(0, 0, 0);

	//draw divide line
	glBegin(GL_LINES);
	glVertex3f(-1, -0.5, 0);
	glVertex3f(1, -0.5, 0);
	glEnd();

	//3 small windows
	for (int i = 0; i < lights_num; i++)
	{
		//gap is 100, start from (100, 0)
		glViewport(100 * (i + 1) + 200 * i, 0, 200, 200);
		glLoadIdentity();
		gluOrtho2D(0, 200, 0, 200);
		glColor3f(0.6, 0.6, 0.6);

		glActiveTextureARB(GL_TEXTURE0);
		glEnable(GL_TEXTURE_2D);
	//	if(i<2)
		glBindTexture(GL_TEXTURE_2D, tex_sh[i]);
		/*else
			glBindTexture(GL_TEXTURE_2D, tex_walls);*/
		glBegin(GL_POLYGON);
		glTexCoord2d(1, 0);
		glVertex3f(0, 0, 0);
		glTexCoord2d(1, 1);
		glVertex3f(0, 200, 0);
		glTexCoord2d(0, 1);
		glVertex3f(200, 200, 0);
		glTexCoord2d(0, 0);
		glVertex3f(200, 0, 0);
		glEnd();
	}

	//	glBindTexture(GL_TEXTURE_2D, tex_sh[0]);
	/* --------3D-------- */
	glViewport(0, 200, 1000, 600);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//Designer annotation
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos2f(2.0f, 190.0f);
	drawString("This scene is just for checking continue depth overlay (3 different point lighting)");
	glRasterPos2f(2.0f, 185.0f);
	drawString("Use mouse left button to rotate model, see how the depth overlay change");
	glRasterPos2f(2.0f, 170.0f);
	drawString("Keyboard:");
	glRasterPos2f(2.0f, 165.0f);
	drawString("C to change scene");
	
	gluPerspective(25, 8 / 6.0, 0.1, 25000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(50, 50, 50, 0, 0, 0, 0, 1, 0);

	//Draw x,y,z coordinates
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0, 0, 0);
	glVertex3f(50.0f, 0, 0);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 50.0f, 0);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 50.0f);
	glEnd();

	//-----------------------------second path------------
	glEnable(GL_LIGHTING);
	glMatrixMode(GL_MODELVIEW);

	glLoadMatrixf(&mat_view[0][0]);
	draw_tex();
	glMultMatrixf(&mat_model[0][0]);
	draw_model();
}


void draw1(const glm::mat4& mat_model, const glm::mat4& mat_view)
{
	if(save_shadow==true)
		tex_init();
	//------------------------First draw -------------------------
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame_buffer_s);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glViewport(0, 0, shadow_w, shadow_h);

	// No need light and texture
	GLboolean li = glIsEnabled(GL_LIGHTING);
	if (li) 
		glDisable(GL_LIGHTING);
	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);
	//9:24
	glActiveTexture(GL_TEXTURE2);
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glm::mat4 shadow_mat_pd = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, -5.0f, 5.0f);
	glm::mat4 shadow_mat_pp = glm::perspective(glm::radians(100.0f), 1.0f, 1.0f, 1.0e10f);
	glm::vec3 cen = glm::vec3(get_mat_model()*glm::vec4(0, 0, 0, 1));
	glm::mat4 shadow_mat_v[lights_num];

	for (int i = 0; i < lights_num; ++i)
	{
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex_shadow[i], 0);
		glClear(GL_DEPTH_BUFFER_BIT);
		if (i < 2)
			shadow_mat_v[i] = glm::lookAt(glm::vec3(light_pos[i]), cen, glm::vec3(0, 1, 0));
		else
			shadow_mat_v[i] = glm::lookAt(glm::vec3(cen), glm::vec3(-light_pos[i]), glm::vec3(0, 1, 0));
		//point lighting
		if (i < 2)
		{
			glMatrixMode(GL_PROJECTION);
			glLoadMatrixf(&shadow_mat_pp[0][0]);
		}
		//directional lighting
		else
		{
			glMatrixMode(GL_PROJECTION);
			glLoadMatrixf(&shadow_mat_pd[0][0]);
		}
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(&shadow_mat_v[i][0][0]);
		draw_world();
		glMultMatrixf(&mat_model[0][0]);
		draw_model();
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glDisable(GL_CULL_FACE);
	glCullFace(GL_BACK);


		//shadow[lights_num]
		/*glBindFramebuffer(GL_FRAMEBUFFER, 0);
		for (int i = 0; i < lights_num; i++)
		{
			glDisable(GL_LIGHTING);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			shadow_mat_p = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, 1.0e10f);
			shadow_mat_v[i] = glm::lookAt(glm::vec3(light_pos[i]), glm::vec3(0), glm::vec3(0, 1, 0));

			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadMatrixf(&shadow_mat_p[0][0]); 
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadMatrixf(&shadow_mat_v[i][0][0]); 


			glMultMatrixf(&mat_model[0][0]);
			draw_model();

			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, tex_sh[i]);
			glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, 0, get_frame_width(), get_frame_height(), 0);
			glEnable(GL_TEXTURE_2D); 
		}
		*/
	if (save_shadow) {
		save_shadow = false;
		glActiveTexture(GL_TEXTURE1);
	    GLfloat* data = new GLfloat[shadow_w*shadow_h];
		for (int i = 0; i < lights_num; ++i) {
			glBindTexture(GL_TEXTURE_2D, tex_shadow[i]);
				glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, data);//get texture data
				wchar_t ss[50]; swprintf(ss, L"d%d.png", i);
				il_saveImgDep(ss, data, shadow_w, shadow_w);
		}
		delete[] data;
	}


	glViewport(0, 0, get_frame_width(), get_frame_height());
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	if (li)
		glEnable(GL_LIGHTING);

	//------------------------------- second draw scene ----------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, 1000, 800);
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor3f(0, 0, 0);

	//draw divide line
	glBegin(GL_LINES);
	glVertex3f(-1, -0.5, 0);
	glVertex3f(1, -0.5, 0);
	glEnd();

	//3 small windows
	glDisable(GL_LIGHTING);
	for (int i = 0; i < lights_num; i++)
	{
		//gap is 100, start from (100, 0)
		glViewport(100 * (i + 1) + 200 * i, 0, 200, 200);
		glLoadIdentity();
		gluOrtho2D(0, 200, 0, 200);

		glColor3f(1.0, 1.0, 1.0);

		glActiveTextureARB(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex_depth[i]);
		glActiveTexture(GL_TEXTURE0);
		glEnable(GL_TEXTURE_2D);

		//glActiveTextureARB(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, tex_shadow[i]);
		//glActiveTexture(GL_TEXTURE1);
		//glEnable(GL_TEXTURE_2D); 

		glBegin(GL_POLYGON);
		glTexCoord2d(1, 0);
		glVertex3f(0, 0, 0);
		glTexCoord2d(1, 1);
		glVertex3f(0, 200, 0);
		glTexCoord2d(0, 1);
		glVertex3f(200, 200, 0);
		glTexCoord2d(0, 0);
		glVertex3f(200, 0, 0);
		glEnd();
	}
	glBindTexture(GL_TEXTURE_2D, tex_walls);
	glActiveTexture(GL_TEXTURE0);

	/* --------3D-------- */
	glViewport(0, 200, 1000, 600);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//Designer annotation
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos2f(2.0f, 190.0f);
	drawString("Mouse:");
	glRasterPos2f(2.0f, 185.0f);
	drawString("USE scroll bar to resize scene, push scroll bar to move model");
	glRasterPos2f(2.0f, 180.0f);
	drawString("USE left button to rotate model, right button to rotate scene");
	glRasterPos2f(2.0f, 170.0f);
	drawString("Keyboard:");
	glRasterPos2f(2.0f, 165.0f);
	drawString("F1: open/close light1, F2: open/close light2, F3: open/close light3");
	glRasterPos2f(2.0f, 160.0f);
	drawString("USE P to (start/end) light rotation");
	glRasterPos2f(2.0f, 155.0f);
	drawString("USE S to update depth");
	glRasterPos2f(2.0f, 150.0f);
	drawString("USE C to change scene");
	glRasterPos2f(2.0f, 145.0f);
	drawString("USE PageUp to higer scene");
	glRasterPos2f(2.0f, 140.0f);
	drawString("USE PageDown to lower scene");

	gluPerspective(25, 8 / 6.0, 0.1, 25000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(50, 50, 50, 0, 0, 0, 0, 1, 0);
	
	//Draw x,y,z coordinates
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0, 0, 0);
	glVertex3f(50.0f, 0, 0);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 50.0f, 0);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 50.0f);
	glEnd();

	glEnable(GL_LIGHTING);
	// 1 Environment lighting
	for (int i = 0; i < lights_num; ++i)
		glDisable(GL_LIGHT0 + i);
	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&mat_view[0][0]);
	draw_world();
	glMultMatrixf(&mat_model[0][0]);
	draw_model();

	// 2 Point Lighting
	GLfloat la[4];
	glGetFloatv(GL_LIGHT_MODEL_AMBIENT, la);
	float gac[4] = { 0,0,0,1 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, gac); // black
	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);

	glDepthFunc(GL_EQUAL);
	glBlendFunc(GL_ONE, GL_ONE);

	for (int i = 0; i < lights_num; ++i)
	{
		if (toggle[i] == true)
		{
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, tex_shadow[i]);

			// When the eye planes are specified, the GL will automatically post-multiply them
			// with the inverse of the current modelview matrix.
			glMatrixMode(GL_MODELVIEW);
			glLoadMatrixf(&mat_view[0][0]); //glLoadIdentity();
	
			glm::mat4 mat;
			if (i < 2)
			{
				mat = glm::translate(glm::vec3(0.5f, 0.5f, 0.5f))	* glm::scale(glm::vec3(0.5f, 0.5f, 0.5f)) * shadow_mat_pp * shadow_mat_v[i];
			}
			else
			{
				mat = glm::translate(glm::vec3(0.5f, 0.5f, 0.5f))	* glm::scale(glm::vec3(0.5f, 0.5f, 0.5f)) * shadow_mat_pd * shadow_mat_v[i];
			}
			mat = glm::transpose(mat);
			glTexGenfv(GL_S, GL_EYE_PLANE, &mat[0][0]);
			glTexGenfv(GL_T, GL_EYE_PLANE, &mat[1][0]);
			glTexGenfv(GL_R, GL_EYE_PLANE, &mat[2][0]);
			glTexGenfv(GL_Q, GL_EYE_PLANE, &mat[3][0]);

			glMatrixMode(GL_MODELVIEW);
			glLoadMatrixf(&mat_view[0][0]);
			glLightfv(GL_LIGHT0 + i, GL_POSITION, &light_pos[i][0]);
			glEnable(GL_LIGHT0 + i);

			glActiveTexture(GL_TEXTURE0);
			glEnable(GL_TEXTURE_2D);
			glMatrixMode(GL_MODELVIEW);
			glLoadMatrixf(&mat_view[0][0]);
			draw_world();
			glMultMatrixf(&mat_model[0][0]);
			glDisable(GL_TEXTURE_2D);
			draw_model();

			glDisable(GL_LIGHT0 + i);
		}
	}

	//	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, la); // Regain environment lighting
	glDepthFunc(GL_LESS);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// 3 draw sphere in light position
	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);
	glMatrixMode(GL_MODELVIEW);

	glLoadMatrixf(&mat_view[0][0]);
	for (int i = 0; i < lights_num; ++i)
		dlight(light_pos[i], i);

	if (light_rotate)
	{
		for (int i = 0; i < lights_num; ++i)
		{
			light_pos[i] = glm::rotate(glm::radians(1.0f), glm::vec3(0, 1, 0)) * light_pos[i];
		}
	}
}

void key_p()
{
	light_rotate = !light_rotate;
}

int main(void)
{
	init_win(1000, 800, "Shadow Mapping_Fangda Chen", "msyh.ttf");
	init_gl();
	init_light();

	readfile("bunny.obj", &numVertex, &numFaces, vertices, faces);

	set_mat_model(glm::translate(glm::vec3(0, 1, 0)));

	set_mat_view(glm::lookAt(glm::vec3(25, 25, 25), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));

	add_key_callback('P', key_p, L"Rotate the light");

	renderLoop(draw1, draw2);

	glutMainLoop();
}

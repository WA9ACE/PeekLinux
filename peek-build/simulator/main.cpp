
#include "lgui.h"
#include "tweet.h"
#include "buikeymap.h"

#include <GL/glut.h>
#include <stdio.h>
#include <string.h>

#define GL_UNSIGNED_SHORT_5_6_5           0x8363

unsigned char screenBuf[320*240*2];

void recv_sms(void)
{

}

void processKeys(unsigned char key, int x, int y) {

	if (key == 'a') {
		tweetKey(KP_WHEEL_UP);
	} else
	if (key == 'z') {
		tweetKey(KP_WHEEL_DOWN);
	} else
	if (key == 'q') {
		tweetKey(KP_ENTER_KEY);
	} else
	if (key == 'w') {
		tweetKey(KP_BACKSPACE_KEY);
	} else
	if (key == '1') {
		tweetKey(KP_1_KEY);
	} else
	if (key == '2') {
		tweetKey(KP_2_KEY);
	} else
	if (key == '3') {
		tweetKey(KP_3_KEY);
	} else
	if (key == 27) 
		exit(0);
	else
		tweetKey(key);

	glutPostRedisplay();
}

void drawGUI(void)
{
	static int initd = 0;

	if (!initd) {
		lgui_attach(screenBuf);
		tweetInit();
		initd = 1;
	}
	tweetDrawScreen();
	glutPostRedisplay();
}

void processMouse(int button, int state, int x, int y) 
{
    // Used for wheels, has to be up
	if (state == GLUT_UP )
	{
		printf("Button is %d\n", button);
	}
}

void
display(void)
{
  int errcode;
  glClear(GL_COLOR_BUFFER_BIT);
  errcode = glGetError(); if (errcode != 0) fprintf(stderr, "Error Code %x at %d\n", errcode, __LINE__);
  drawGUI();
  errcode = glGetError(); if (errcode != 0) fprintf(stderr, "Error Code %x at %d\n", errcode, __LINE__);
  glPixelStorei(GL_UNPACK_ALIGNMENT,1);
  errcode = glGetError(); if (errcode != 0) fprintf(stderr, "Error Code %x at %d\n", errcode, __LINE__);
  glPixelZoom(1.0f, -1.0f);
  errcode = glGetError(); if (errcode != 0) fprintf(stderr, "Error Code %x at %d\n", errcode, __LINE__);
  glRasterPos2i(-1, 1);
  errcode = glGetError(); if (errcode != 0) fprintf(stderr, "Error Code %x at %d\n", errcode, __LINE__);
  glDrawPixels(320, 240, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, screenBuf);
  errcode = glGetError(); if (errcode != 0) fprintf(stderr, "Error Code %x at %d\n", errcode, __LINE__);
  glutSwapBuffers();
}

int
main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(320, 240);
  glutCreateWindow("simulator");
  glutDisplayFunc(display);
  glutKeyboardFunc(processKeys);
  glutMouseFunc(processMouse);

  glutMainLoop();
  return 0;            
}


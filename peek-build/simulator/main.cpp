
#include "lgui.h"
//#include "tweet.h"
#include "buikeymap.h"
#include "Platform.h"
#include "ConnectionContext.h"
#include "ApplicationManager.h"

#include <GL/glut.h>
#include <stdio.h>
#include <string.h>

#define GL_UNSIGNED_SHORT_5_6_5           0x8363

extern "C" {
unsigned char screenBuf[320*240*2];
}

void recv_sms(void)
{

}

void processKeys(unsigned char key, int x, int y) {
#if 0
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
#endif
	if (key == 27) 
		exit(0);
	else
#ifdef USE_TWEET
		tweetKey(key);
#else
		manager_handleKey(key);
#endif

	glutPostRedisplay();
}

#if 0
void drawGUI(void)
{
	static int initd = 0;

	if (!initd) {
		lgui_attach(screenBuf);
#ifdef USE_TWEET
		tweetInit();
		initd = 1;
		tweetDrawScreen();
#else
		manager_init();
		manager_drawScreen();
#endif
		//glutPostRedisplay();
	}
}
#endif

void processMouse(int button, int state, int x, int y) 
{
    // Used for wheels, has to be up
	if (state == GLUT_UP )
	{
		if (button == 0) {
#ifdef USE_TWEET
			tweetKey(87);
#else
			manager_handleKey(87);
#endif
		} else if (button == 2) {
#ifdef USE_TWEET
			tweetKey(86);
#else
			manager_handleKey(86);
#endif
		}
	}
#ifdef USE_TWEET
	tweetDrawScreen();
#else
	manager_drawScreen();
#endif
	glutPostRedisplay();
}

static unsigned char glBuffer[320*240*2];
void
display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	//drawGUI();
	
	//fprintf(stderr, "in display\n");

	if (!lgui_is_dirty())
		goto show_prev_screen;

	//fprintf(stderr, "in blit\n");

	// we really use glBuffer to make sure drawing works properly
	int index, upper;
	upper = lgui_index_count();
	if (upper == 0) {
		memcpy(glBuffer, screenBuf, 320*240*2);
	} else {
		Rectangle *rect;
		for (index = 0; index < upper; ++index) {
			rect = lgui_get_region(index);
			fprintf(stderr, "DRegion(%d, %d, %d, %d)\n", rect->x, rect->y,
					rect->width, rect->height);

			for (int ypos = 0; ypos < rect->height; ++ypos) {
				memcpy(glBuffer + rect->x*2 + (rect->y+ypos)*320*2,
						screenBuf + rect->x*2 + (rect->y+ypos)*320*2,
						rect->width*2);
			}
		}
	}

show_prev_screen:
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glPixelZoom(1.0f, -1.0f);
	glRasterPos2i(-1, 1);
	glDrawPixels(320, 240, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, glBuffer);
	glutSwapBuffers();
	lgui_blit_done();
}

extern "C" void main_test(void);

extern "C" {
void net_thread(void *d)
{
	main_test();
}
}

static void timerCallback (int value)
{
#ifdef USE_TWEET
	tweetDrawScreen();
#else
	manager_drawScreen();
#endif
	glutPostRedisplay();

	glutTimerFunc(100, timerCallback, 0);
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

  thread_run(net_thread, NULL);

  manager_init();
  lgui_attach(screenBuf);

  glutTimerFunc(100, timerCallback, 0);

  glutMainLoop();
  return 0;            
}


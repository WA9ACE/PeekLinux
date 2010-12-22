#include <string>
#include <stdio.h>
#include <stdarg.h>
#include <SDL/SDL.h>


#include "lgui.h"
#include "Platform.h"
#include "ConnectionContext.h"
#include "ApplicationManager.h"

#include "DataObject.h"
#include "RenderManager.h"

#define EKEY_ACTIVATE           13
#define EKEY_BACK                       12
#define EKEY_FOCUSPREV          0xFFFFFF01
#define EKEY_FOCUSNEXT          0xFFFFFF02
#define EKEY_ALTTAB                     42

void drawScreen(void);

extern "C" {
unsigned char screenBuf[320*240*2];
}

SDL_Surface *load_image( char *filename )
{
    SDL_Surface* loadedImage = NULL;
    SDL_Surface* optimizedImage = NULL;

    loadedImage = SDL_LoadBMP(filename);

    if( loadedImage != NULL )
    {
        optimizedImage = SDL_DisplayFormat( loadedImage );
        SDL_FreeSurface( loadedImage );
    }

    return optimizedImage;
}

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination )
{
    SDL_Rect offset;

    offset.x = x;
    offset.y = y;

    SDL_BlitSurface( source, NULL, destination, &offset );
}

void update_surface(SDL_Surface *dest, unsigned char *buf) 
{
	
	SDL_LockSurface(dest);
	memcpy((Uint8 *)dest->pixels, buf, dest->w * dest->h * 2);
	SDL_UnlockSurface(dest);

}

void processKeys(SDL_keysym key, unsigned int state)
{
	switch(key.sym) {
		case SDLK_UP:
			manager_handleKey(EKEY_FOCUSPREV);
			break;
		case SDLK_DOWN:
			manager_handleKey(EKEY_FOCUSNEXT);
			break;
		case SDLK_ESCAPE:
			manager_handleKey(EKEY_BACK);
			break;
		case SDLK_RETURN:
			manager_handleKey(EKEY_ACTIVATE);
			break;
		default:
			manager_handleKey(key.sym);
	}

	drawScreen();
}

void processMouse(int button, int state, int x, int y) 
{
    // Used for wheels, has to be up
	//if (state == GLUT_UP )
	if(0) // fix
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
	//glutPostRedisplay();
}

static unsigned char glBuffer[320*240*2];
void
display(void)
{
	//glClear(GL_COLOR_BUFFER_BIT);
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

	//glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	//glPixelZoom(1.0f, -1.0f);
	//glRasterPos2i(-1, 1);
	//glDrawPixels(320, 240, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, glBuffer);

#ifdef DRAW_REGIONS
	/* show redraw regions */
	glColor3ub(255, 0, 0);
	if (lgui_is_dirty()) {
		upper = lgui_index_count();
		if (upper == 0) {
			glBegin(GL_LINE_LOOP);
				glVertex2f(-1.0+0.01, -1.0+0.01);
				glVertex2f(1.0-0.01, -1.0+0.01);
				glVertex2f(1.0-0.01, 1.0-0.01);
				glVertex2f(-1.0+0.01, 1.0-0.01);
			glEnd();
		} else {
			Rectangle *rect;
			for (index = 0; index < upper; ++index) {
				rect = lgui_get_region(index);
				glBegin(GL_LINE_LOOP);
					glVertex2f(rect->x/320.0*2.0-1.0, -(rect->y/240.0*2.0-1.0));
					glVertex2f((rect->x+rect->width)/320.0*2.0-1.0, -(rect->y/240.0*2.0-1.0));
					glVertex2f((rect->x+rect->width)/320.0*2.0-1.0, -((rect->y+rect->height)/240.0*2.0-1.0));
					glVertex2f(rect->x/320.0*2.0-1.0, -((rect->y+rect->height)/240.0*2.0-1.0));
				glEnd();
			}
		}
	}
#endif

	//glutSwapBuffers();
show_prev_screen:
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
	//glutPostRedisplay();

	//glutTimerFunc(100, timerCallback, 0);
}

SDL_Surface *screen = NULL;
SDL_Surface *background = NULL;
SDL_Surface *mainscreen = NULL;

const int SCREEN_WIDTH = 423;
const int SCREEN_HEIGHT = 634;
const int SCREEN_BPP = 16;

void drawScreen(void)
{
    renderman_flush();
    manager_drawScreen();
    lgui_set_dirty();

	update_surface(mainscreen, screenBuf);
	apply_surface( 53, 79, mainscreen, screen );

	lgui_blit_done();

	if( SDL_Flip(screen ) == -1 )
		return;
}

void SDLloop(void)
{
	SDL_Event event;
	bool quit;
	
	do
	{
		quit = false;
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_KEYUP:
					processKeys(event.key.keysym, 1);
					break;
				case SDL_QUIT:
					quit = true;
					break;
			}
		}
	} while(!quit);
}

int main(int argc, char **argv)
{
  SDL_Color palette[256];

  thread_run(net_thread, NULL);

  if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
  	return 1;

  screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );

  SDL_WM_SetCaption( "Peek Simulator", NULL );

  background = load_image( "peek.bmp" );
  apply_surface( 0, 0, background, screen );

  mainscreen = SDL_CreateRGBSurface(SDL_SWSURFACE, 320, 240,
        screen->format->BitsPerPixel,
        screen->format->Rmask,
        screen->format->Gmask,
        screen->format->Bmask,
        screen->format->Amask);

  SDL_SetColors(screen, palette, 0, 256);

  dataobject_platformInit();
  renderman_init();

  PlatformInit();

  manager_init();

  drawScreen();

  SDLloop();

  return 0;            
}

extern "C" int simAutoDetect()
{
	return 0;
}

extern "C" void updateScreen()
{
	drawScreen();
}

extern "C" void emo_printf(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
}

extern "C" void script_emo_printf(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
}

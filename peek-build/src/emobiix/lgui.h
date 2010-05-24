#ifndef _LGUI_H_
#define _LGUI_H_

#include "Gradient.h"
#include "Font.h"
#include "Point.h"

#ifdef __cplusplus
extern "C" {
#endif

void lgui_attach(void *buf);

void lgui_vertical_gradient(unsigned char start_red, unsigned char start_green, unsigned char start_blue,
	unsigned char end_red, unsigned char end_green, unsigned char end_blue,
	int startx, int starty, int width, int height);

void lgui_vertical_gradientG(Gradient *g,
	int startx, int starty, int width, int height);

void lgui_hline(int x, int y, int len, int width, unsigned char red, unsigned char green, unsigned char blue);
void lgui_vline(int x, int y, int len, int width, unsigned char red, unsigned char green, unsigned char blue);

void lgui_box(int x, int y, int width, int height, int linewidth, unsigned char red, unsigned char green, unsigned char blue);
void lgui_roundedbox_line(int x, int y, int width, int height, int radius,
						  unsigned char red, unsigned char green, unsigned char blue);
void lgui_roundedbox_fill(int x, int y, int width, int height, int radius,
						  unsigned char red, unsigned char green, unsigned char blue);
void lgui_rbox_gradient(Gradient *g, int x, int y, int width, int height, int radius);
void lgui_circlefill(int xc, int yc, int r,
		  unsigned char red, unsigned char green, unsigned char blue);

#define LGUI_TOPRIGHT		0x1
#define LGUI_TOPLEFT		0x2
#define LGUI_BOTTOMRIGHT	0x4
#define LGUI_BOTTOMLEFT		0x8
#define LGUI_TBLR			(LGUI_TOPRIGHT|LGUI_TOPLEFT|LGUI_BOTTOMRIGHT|LGUI_BOTTOMLEFT)
void lgui_aacircle(int x, int y, int rad, int arcs,
				   unsigned char red, unsigned char green, unsigned char blue);

void lgui_luminence_alpha_blitC(int destx, int desy, int imgx, int imgy,
		int imgdx, int imgdy, int imgwidth, int imgheight, unsigned char *img,
		unsigned char r, unsigned char g, unsigned char b);
void lgui_luminence_A4_blitC(int destx, int desy, int imgx, int imgy,
		int imgdx, int imgdy, int imgwidth, int imgheight, unsigned char *img,
		Color c);
void lgui_blitRGB565(int destx, int desy, int imgx, int imgy,
	 	int imgwidth, int imgheight, unsigned char *img);
void lgui_black_alpha_blitRGB(int destx, int desy, int imgx, int imgy,
	 	int imgwidth, int imgheight, unsigned char *img);
void lgui_alpha_blitRGBA(int destx, int desty, int imgx, int imgy,
	 	int imgwidth, int imgheight, unsigned char *img);

void lgui_draw_font(int x, int y, const char *utf8, Font *f, Color c); 
void lgui_measure_font(const char *utf8, Font *f, IPoint *output);

#ifdef __cplusplus
}
#endif

#endif

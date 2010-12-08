#ifndef _LGUI_H_
#define _LGUI_H_

#include "Gradient.h"
#include "Font.h"
#include "Point.h"
#include "Rectangle.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RGB_TO_565(r,g,b) (((((unsigned short)(r)) << 8) & 0xF800) | ((((unsigned short)(g)) << 3) & 0x7E0) | (b >> 3 ))

#define LGUI_MODE_REPLACE		0
#define LGUI_MODE_STENCIL		1
#define LGUI_MODE_ALPHA			2

#define LGUI_CORNER_TOPLEFT			0x01
#define LGUI_CORNER_TOPRIGHT			0x02
#define LGUI_CORNER_BOTTOMRIGHT		0x04
#define LGUI_CORNER_BOTTOMLEFT			0x08

#define LGUI_TOP				0x01
#define LGUI_RIGHT				0x02
#define LGUI_BOTTOM				0x04
#define LGUI_LEFT				0x08

#define LGUI_CORNERS_ALL		0x0F
#define LGUI_SIDES_ALL			0x0F

void lgui_attach(void *buf);
void lgui_clear(unsigned char pixel);

void lgui_vertical_gradient(unsigned char start_red, unsigned char start_green,
	unsigned char start_blue, unsigned char start_alpha,
	unsigned char end_red, unsigned char end_green, unsigned char end_blue,
	unsigned char end_alpha, int startx, int starty, int width, int height);

void lgui_vertical_gradientG(Gradient *g,
	int startx, int starty, int width, int height);

void lgui_hline(int x, int y, int len, int width, unsigned char red, unsigned char green,
				unsigned char blue, unsigned char alpha);
void lgui_vline(int x, int y, int len, int width, unsigned char red, unsigned char green,
				unsigned char blue, unsigned char alpha);

void lgui_box(int x, int y, int width, int height, int linewidth,
			  unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha,
			  unsigned int cornerFlags);
void lgui_roundedbox_line(int x, int y, int width, int height, int radius,
						  unsigned char red, unsigned char green, unsigned char blue,
						  unsigned int borderFlags, unsigned int borderCornerFlags);
void lgui_roundedbox_fill(int x, int y, int width, int height, int radius,
						  unsigned char red, unsigned char green, unsigned char blue,
						  unsigned char alpha, unsigned int cornerFlags);
void lgui_rbox_gradient(Gradient *g, int x, int y, int width, int height, int radius,
						unsigned int cornerFlags);
void lgui_circlefill(int xc, int yc, int r,
		  unsigned char red, unsigned char green, unsigned char blue,
		  unsigned char alpha);

#define LGUI_TBLR			(LGUI_TOPRIGHT|LGUI_TOPLEFT|LGUI_BOTTOMRIGHT|LGUI_BOTTOMLEFT)
void lgui_aacircle(int x, int y, int rad, int arcs,
				   unsigned char red, unsigned char green, unsigned char blue);

void lgui_luminence_alpha_blitC(int destx, int desy, int imgx, int imgy,
		int imgdx, int imgdy, int imgwidth, int imgheight, unsigned char *img,
		unsigned char r, unsigned char g, unsigned char b);

void lgui_luminence_A4_blitC(int destx, int desy, int imgx, int imgy,
		int imgdx, int imgdy, int imgwidth, int imgheight, unsigned char *img,
		Color c, int mode, int isItalic);
void lgui_blitRGB565(int destx, int desy, int imgx, int imgy,
	 	int imgwidth, int imgheight, int rowstride,
		unsigned char *img, int isStencil, unsigned char alpha);
void lgui_blitRGB565A8(int destx, int desy, int imgx, int imgy,
	 	int imgwidth, int imgheight, int rowstride,
		unsigned char *img, unsigned char alpha);
void lgui_black_alpha_blitRGB(int destx, int desy, int imgx, int imgy,
	 	int imgwidth, int imgheight, unsigned char *img);
void lgui_alpha_blitRGBA(int destx, int desty, int imgx, int imgy,
	 	int imgwidth, int imgheight, unsigned char *img);

void lgui_draw_font(int x, int y, int maxw, int maxh, const char *utf8, Font *f, Color c, int isBold); 
void lgui_measure_font(const char *utf8, Font *f, int isBold, IPoint *output);

void lgui_complex_draw_font(int x, int y, int maxw, int mahx, const char *utf8,
		Font *f, Color c, int cursorindex, int startindex, int *windowPercent,
		int *windowOffset, int useEscapeSeqs, Rectangle *cursorBox);
void lgui_measure_font_complex(const char *utf8, Font *f, IPoint *output);

void lgui_clip_set(Rectangle *rect);
void lgui_clip_union(Rectangle *rect);
void lgui_clip_and(Rectangle *rect);
void lgui_clip_identity(void);
void lgui_clip_push(void);
void lgui_clip_pop(void);

void lgui_translate(int x, int y);

void lgui_push_region(void);
int lgui_index_count(void);
Rectangle *lgui_get_region(int index);
void lgui_set_dirty(void);
int lgui_is_dirty(void);
void lgui_blit_done(void);

int lgui_clip_rect(Rectangle *rect, int *lowx, int *lowy,
		int *highx, int *highy);

#ifdef __cplusplus
}
#endif

#endif

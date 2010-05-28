#include "lgui.h"
#include "Debug.h"
#include "Color.h"

#include "p_malloc.h"

#include <math.h>
#include <stdio.h>

#ifndef M_PI
#define M_PI 3.141592654f
#endif

#define LGUI_WIDTH 320
#define LGUI_HEIGHT 240
static unsigned short *lgui_buffer = 0;

#define RGB_TO_565(r,g,b) (((r << 8) & 0xF800) | ((g << 3) & 0x7E0) | ((b >> 3) & 0x1F))
#define SCALE_PIXEL(scale, r, g, b) ((( (((int)(r))*scale/255) << 8) & 0xF800) | \
	(((((int)(g))*scale/255) << 3) & 0x7E0) | \
	(((((int)(b))*scale/255) >> 3) & 0x1F))

#define PIXEL_MODULATE_ALPHA(pixel, srcpixel, alpha) \
	( (( (((unsigned int)(pixel & 0xF800)) * (unsigned int)(alpha))/255)&0xF800) + \
    (( (((unsigned int)(srcpixel & 0xF800)) * (255-(unsigned int)(alpha)))/255)&0xF800) ) | \
	( (( (((unsigned int)(pixel & 0x7E0)) * (unsigned int)(alpha))/255)&0x7E0) + \
    (( (((unsigned int)(srcpixel & 0x7E0)) * (255-(unsigned int)(alpha)))/255)&0x7E0) ) | \
	( (( (((unsigned int)(pixel & 0x1F)) * (unsigned int)(alpha))/255)&0x1F) + \
    (( (((unsigned int)(srcpixel & 0x1F)) * (255-(unsigned int)(alpha)))/255)&0x1F))

#define FONT_HEADER_SIZE (6*4)

static void initD(void );

void lgui_attach(void *buf)
{
	static int aacircleInit = 0;
	lgui_buffer = (unsigned short *)buf;
    

	if (!aacircleInit) {
		initD();
		aacircleInit = 1;
	}
}

#define GRADIENT_STEP(res, sr, er, val, idx) res = sr*100 * (((val-1)-idx)*100)/((val-1)*100); \
		res += er*100 * (idx*100)/((val-1)*100); \
		res /= 100;
void lgui_vertical_gradient(unsigned char start_red, unsigned char start_green, unsigned char start_blue,
	unsigned char end_red, unsigned char end_green, unsigned char end_blue,
	int startx, int starty, int width, int height)
{
	unsigned int red;
	unsigned int green;
	unsigned int blue;
	int line, ypos, i;
	unsigned short *buf, pixel;

	ypos = starty;
	for (line = 0; line < height; ++line) {
		if (ypos < 0 || ypos >= LGUI_HEIGHT) {
			++ypos;
			continue;
		}
		GRADIENT_STEP(red, start_red, end_red, height, line)
		GRADIENT_STEP(green, start_green, end_green, height, line)
		GRADIENT_STEP(blue, start_blue, end_blue, height, line)

		pixel = RGB_TO_565(red, green, blue);

		buf = lgui_buffer + startx + ypos*LGUI_WIDTH;

		for (i = 0; i < width; ++i) {
			*buf = pixel;
			++buf;
		}
		++ypos;
	}
}

void lgui_vertical_gradientG(Gradient *g,
	int startx, int starty, int width, int height)
{
	int hstart, hend;
	int stop, maxStop;
	Color cstart, cend;

	hstart = height;
	cstart = gradient_getStop(g, 0, &hstart);
	hend = height;
	cend = gradient_getStop(g, 1, &hend);
	stop = 1;
	maxStop = gradient_stops(g);
	do {
		if (hstart != hend)
			lgui_vertical_gradient(cstart.rgba.red, cstart.rgba.green, cstart.rgba.blue,
					cend.rgba.red, cend.rgba.green, cend.rgba.blue,
					startx, starty+hstart, width, hend-hstart);
		++stop;
		cstart = cend;
		hstart = hend;
		hend = height;
		if (stop < maxStop)
			cend = gradient_getStop(g, stop, &hend);
	} while(stop < maxStop);
}

void lgui_hline(int x, int y, int len, int width, unsigned char red, unsigned char green, unsigned char blue)
{
	int line, ypos, i;
	unsigned short *buf, pixel;

	ypos = y;
	pixel = RGB_TO_565(red, green, blue);
	for (line = 0; line < width; ++line) {
		if (ypos < 0 || ypos >= LGUI_HEIGHT) {
			++ypos;
			continue;
		}

		buf = lgui_buffer + x + ypos*LGUI_WIDTH;

		for (i = 0; i < len; ++i) {
			*buf = pixel;
			++buf;
		}

		++ypos;
	}
}

void lgui_vline(int x, int y, int len, int width, unsigned char red, unsigned char green, unsigned char blue)
{
	int line, ypos, i;
	unsigned short *buf, pixel;

	ypos = y;
	pixel = RGB_TO_565(red, green, blue);
	for (line = 0; line < len; ++line) {
		if (ypos < 0 || ypos >= LGUI_HEIGHT) {
			++ypos;
			continue;
		}

		buf = lgui_buffer + x + ypos*LGUI_WIDTH;

		for (i = 0; i < width; ++i) {
			*buf = pixel;
			++buf;
		}

		++ypos;
	}
}

void lgui_box(int x, int y, int width, int height, int linewidth, unsigned char red, unsigned char green, unsigned char blue)
{
	lgui_hline(x, y, width, linewidth, red, green, blue);
	lgui_hline(x, y+height-linewidth, width, linewidth, red, green, blue);
	lgui_vline(x, y, height, linewidth, red, green, blue);
	lgui_vline(x+width-linewidth, y, height, linewidth, red, green, blue);
}

void lgui_luminence_alpha_blitC(int destx, int desty, int imgx, int imgy, int imgdx, int imgdy,
							   int imgwidth, int imgheight, unsigned char *img,
							   unsigned char red, unsigned char green, unsigned char blue)
{
	unsigned short *buf;
	unsigned char *imgbuf;
	unsigned short pixel, srcpixel;
	unsigned char scale;
	int line, col, ypos, imgypos;
	
	ypos = desty;
	imgypos = imgy;
	for (line = 0; line < imgdy; ++line) {
		if (ypos < 0 || ypos >= LGUI_HEIGHT) {
			++ypos;
			++imgypos;
			continue;
		}
		buf = lgui_buffer + destx + ypos*LGUI_WIDTH;
		imgbuf = img + imgx + imgypos*imgwidth;

		for (col = 0; col < imgdx; ++col) {
			scale = (*imgbuf);
			if (scale > 0) {
				srcpixel = *buf;
				pixel = RGB_TO_565(red, green, blue);
				*buf = (unsigned short)(PIXEL_MODULATE_ALPHA(pixel, srcpixel, scale));
			}
			++buf;
			++imgbuf;
		}
		++ypos;
		++imgypos;
	}
}

void lgui_luminence_A4_blitC(int destx, int desty, int imgx, int imgy,
		int imgdx, int imgdy, int imgwidth, int imgheight, unsigned char *img,
		Color c)
{
	unsigned short *buf;
	unsigned char *imgbuf;
	unsigned short pixel, srcpixel;
	unsigned char scale, scaleorig;
	int line, col, ypos, imgypos;
	
	ypos = desty;
	imgypos = imgy;
	for (line = 0; line < imgdy; ++line) {
		if (ypos < 0 || ypos >= LGUI_HEIGHT) {
			++ypos;
			++imgypos;
			continue;
		}
		buf = lgui_buffer + destx + ypos*LGUI_WIDTH;
		imgbuf = img + imgx + imgypos*((imgwidth+1)>>1);

		for (col = 0; col < (imgdx+1) >> 1; ++col) {
			scaleorig = (*imgbuf);
			scale = scaleorig & 0xF0;
			if (scale > 0) {
				srcpixel = *buf;
				pixel = RGB_TO_565(c.rgba.red, c.rgba.green, c.rgba.blue);
				*buf = (unsigned short)(PIXEL_MODULATE_ALPHA(pixel, srcpixel, scale));
			}
			++buf;
			scale = (scaleorig << 4) & 0xF0;
			if (scale > 0) {
				srcpixel = *buf;
				pixel = RGB_TO_565(c.rgba.red, c.rgba.green, c.rgba.blue);
				*buf = (unsigned short)(PIXEL_MODULATE_ALPHA(pixel, srcpixel, scale));
			}
			
			++buf;
			++imgbuf;
		}
		++ypos;
		++imgypos;
	}
}

void lgui_blitRGB565(int destx, int desty, int imgx, int imgy,
        int imgwidth, int imgheight, unsigned char *img)
{
    unsigned short *buf;
    unsigned char *imgbuf;
    unsigned short pixel;
    int line, col, ypos, imgypos;
   
    ypos = desty;
    imgypos = imgy;
    for (line = 0; line < imgheight; ++line) {
        if (ypos < 0 || ypos >= LGUI_HEIGHT) {
            ++ypos;
            ++imgypos;
            continue;
        }
        buf = lgui_buffer + destx + ypos*LGUI_WIDTH;
        imgbuf = img + imgx + imgypos*imgwidth*2;

        for (col = 0; col < imgwidth; ++col) {
                pixel = *((unsigned short *)(imgbuf));
                /*if (pixel > 0)*/
                    *buf = pixel;
            ++buf;
            imgbuf +=2;
        }
        ++ypos;
        ++imgypos;
    }
}

void lgui_black_alpha_blitRGB(int destx, int desty, int imgx, int imgy,
	 	int imgwidth, int imgheight, unsigned char *img)
{
	unsigned short *buf;
	unsigned char *imgbuf;
	unsigned short pixel;
	int line, col, ypos, imgypos;
	
	ypos = desty;
	imgypos = imgy;
	for (line = 0; line < imgheight; ++line) {
		if (ypos < 0 || ypos >= LGUI_HEIGHT) {
			++ypos;
			++imgypos;
			continue;
		}
		buf = lgui_buffer + destx + ypos*LGUI_WIDTH;
		imgbuf = img + imgx + imgypos*imgwidth*3;

		for (col = 0; col < imgwidth; ++col) {
				pixel = RGB_TO_565(imgbuf[0], imgbuf[1], imgbuf[2]);
				if (pixel > 0)
					*buf = pixel;
			++buf;
			imgbuf +=3;
		}
		++ypos;
		++imgypos;
	}

}

void lgui_alpha_blitRGBA(int destx, int desty, int imgx, int imgy,
	 	int imgwidth, int imgheight, unsigned char *img)
{
	unsigned short *buf;
	unsigned char *imgbuf, scale;
	unsigned short pixel, srcpixel;
	int line, col, ypos, imgypos;
	
	ypos = desty;
	imgypos = imgy;
	for (line = 0; line < imgheight; ++line) {
		if (ypos < 0 || ypos >= LGUI_HEIGHT) {
			++ypos;
			++imgypos;
			continue;
		}
		buf = lgui_buffer + destx + ypos*LGUI_WIDTH;
		imgbuf = img + imgx + imgypos*imgwidth*4;

		for (col = 0; col < imgwidth; ++col) {
				scale = imgbuf[3];
				srcpixel = *buf;
				pixel = RGB_TO_565(imgbuf[0], imgbuf[1], imgbuf[2]);
				*buf = (unsigned short)(PIXEL_MODULATE_ALPHA(pixel, srcpixel, scale));
			++buf;
			imgbuf +=4;
		}
		++ypos;
		++imgypos;
	}

}

/* Wu Citcle implementation */
static int I=255;           /* Intensity. */
static int A=2;             /*Accuracy determained by argument of the root function D.*/

/*  
 * R < 10^A
 * Memory = 4 * 10^(2A)
 * For example:
 *   for memory 40K (small machines), screens not more than about 100*100,
 *   for memory 4Meg, screens about 1000*1000.
 */

/*
 *  Auxiliary:
 */
static int D[10000];             /* Precalcuated reversed fractions of roots.*/
static int POWERES_OF_TEN[] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000 };

 static void initD(){
    int p = POWERES_OF_TEN[A*2];
	int i, f;
    /* D = new int[p]; */
    for(i=0; i<p; i++ ){  
        f = (       (int)(I*sqrt((double)i))       )%I;
        D[i] =  f == 0 ? 0 : I-f;
    }
    D[0] = 0;  /* Secure from floating point errors on poor platforms. */
}

static int putpixelX = 0;
static int putpixelY = 0;
static void putpixelAlpha(int x, int y, unsigned char red, unsigned char green, unsigned char blue,
					 unsigned char intensity)
{
	unsigned short pixel, srcpixel;
	unsigned short *buf;

	x+=putpixelX;
	y+=putpixelY;

    if (y < 0 || y > LGUI_HEIGHT)
        return;

	buf = lgui_buffer+x+y*LGUI_WIDTH;
	pixel = RGB_TO_565(red, green, blue);
	srcpixel = *buf;
	*buf = (unsigned short)(PIXEL_MODULATE_ALPHA(pixel, srcpixel, intensity));
}

     /*Input: uses precalculated variables D and I.*/
 void drawArch( int R, unsigned char red, unsigned char green, unsigned char blue , int arcs) {
    int y=0;
    int x=R;
    int d=0;
	int dnew;
    while( y<x ) {
        dnew = D[R*R-y*y];
        if( dnew < d ) x--;
		/* lower half or circle */
		if (arcs & LGUI_BOTTOMRIGHT) {
		/* / upper */
        putpixelAlpha(x-1,y,   red, green, blue, dnew);
        putpixelAlpha(x,  y,   red, green, blue, (I-dnew));
		/* / lower */
        putpixelAlpha(y, x-1,   red, green, blue, dnew);
        putpixelAlpha(y,   x,   red, green, blue, (I-dnew));
		}

		if (arcs & LGUI_BOTTOMLEFT) {
		/* \ upper */
        putpixelAlpha(-(x-1),y,   red, green, blue, dnew);
        putpixelAlpha(-x,  y,   red, green, blue, (I-dnew));
		/* \ lower */
		putpixelAlpha(-y, x-1,   red, green, blue, dnew);
        putpixelAlpha(-y,   x,   red, green, blue, (I-dnew));
		}

		/* upper half or circle */
		if (arcs & LGUI_TOPRIGHT) {
		/* \ upper */
		putpixelAlpha(x-1,-y,   red, green, blue, dnew);
        putpixelAlpha(x,  -y,   red, green, blue, (I-dnew));
		/* \ lower */
        putpixelAlpha(y, -(x-1),   red, green, blue, dnew);
        putpixelAlpha(y,   -x,   red, green, blue, (I-dnew));
		}

		if (arcs & LGUI_TOPLEFT) {
		/* / upper */
        putpixelAlpha(-(x-1),-y,   red, green, blue, dnew);
        putpixelAlpha(-x,  -y,   red, green, blue, (I-dnew));
		/* / lower */
		putpixelAlpha(-y, -(x-1),   red, green, blue, dnew);
        putpixelAlpha(-y,   -x,   red, green, blue, (I-dnew));
		}

        y++;
        d = dnew;
    }    
}

 void lgui_aacircle(int x, int y, int rad, int arcs, unsigned char red, unsigned char green, unsigned char blue)
 {
	putpixelX = x;
	putpixelY = y;
	drawArch(rad, red, green, blue, arcs);
 }

 void lgui_roundedbox_line(int x, int y, int width, int height, int radius,
						  unsigned char red, unsigned char green, unsigned char blue)
 {
	lgui_aacircle(x+radius, y+radius, radius, LGUI_TOPLEFT, red, green, blue);
	lgui_aacircle(x+width-radius, y+radius, radius, LGUI_TOPRIGHT, red, green, blue);
	lgui_aacircle(x+radius, y+height-radius, radius, LGUI_BOTTOMLEFT, red, green, blue);
	lgui_aacircle(x+width-radius, y+height-radius, radius, LGUI_BOTTOMRIGHT, red, green, blue);

	lgui_hline(x+radius, y, width-radius*2, 1, red, green, blue);
	lgui_hline(x+radius, y+height, width-radius*2, 1, red, green, blue);

	lgui_vline(x, y+radius, height-radius*2, 1, red, green, blue);
	lgui_vline(x+width, y+radius, height-radius*2, 1, red, green, blue);
 }

 void lgui_roundedbox_fill(int x, int y, int width, int height, int radius,
						  unsigned char red, unsigned char green, unsigned char blue)
 {
	lgui_circlefill(x+radius, y+radius, radius, red, green, blue);
	lgui_circlefill(x+width-radius, y+radius, radius, red, green, blue);
	lgui_circlefill(x+radius, y+height-radius, radius, red, green, blue);
	lgui_circlefill(x+width-radius, y+height-radius, radius, red, green, blue);

	lgui_hline(x+radius, y, width-radius*2, height, red, green, blue);
	lgui_hline(x, y+radius, radius*2, height-radius*2, red, green, blue);
	lgui_hline(x+width-radius*2, y+radius, radius*2, height-radius*2, red, green, blue);
 }

void lgui_rbox_gradient(Gradient *g, int x, int y, int width, int height, int radius)
{
	int hstart, hend;
	int stop, maxStop;
	Color cstart, cend;
	float cr;

	unsigned int red;
	unsigned int green;
	unsigned int blue;
	int line, ypos, i;
	unsigned short *buf, pixel;
	int iheight, iwidth, imod;
	int startx;

	hstart = height;
	cstart = gradient_getStop(g, 0, &hstart);
	hend = height;
	cend = gradient_getStop(g, 1, &hend);
	stop = 1;
	maxStop = gradient_stops(g);
	do {
		if (hstart != hend) {
			/*lgui_vertical_gradient(cstart.rgba.red, cstart.rgba.green, cstart.rgba.blue,
					cend.rgba.red, cend.rgba.green, cend.rgba.blue,
					startx, starty+hstart, width, hend-hstart);*/
			startx = x;
			ypos = y+hstart;
			iheight = hend-hstart;
			for (line = 0; line < iheight; ++line) {
				if (ypos < 0 || ypos >= LGUI_HEIGHT) {
					++ypos;
					continue;
				}
				GRADIENT_STEP(red, cstart.rgba.red, cend.rgba.red, iheight, line)
				GRADIENT_STEP(green, cstart.rgba.green, cend.rgba.green, iheight, line)
				GRADIENT_STEP(blue, cstart.rgba.blue, cend.rgba.blue, iheight, line)

				pixel = RGB_TO_565(red, green, blue);

				if (ypos-y < radius) {
					imod = radius-(ypos-y);
					cr = (float)cos((float)imod/(float)(radius)*M_PI/2.0f);
					imod = radius - (int)(cr*(float)radius);
				} else if (ypos-y > height-radius) {
					imod = (height+y)-ypos;
					cr = (float)cos((radius-imod)/(float)radius*M_PI/2.0f);
					imod = radius - (int)(cr*(float)radius);
				} else {
					imod = 0;
				}

				buf = lgui_buffer + startx +imod + ypos*LGUI_WIDTH;
		

				iwidth = width - (imod<<1);
				for (i = 0; i < iwidth; ++i) {
					*buf = pixel;
					++buf;
				}
				++ypos;
			}
		}
		++stop;
		cstart = cend;
		hstart = hend;
		hend = height;
		if (stop < maxStop)
			cend = gradient_getStop(g, stop, &hend);
	} while(stop < maxStop);
}


#define SQR(x) ((x)*(x))
#define ROUND(x) (int)((x)+0.5f)
void lgui_circlefill(int xc, int yc, int r,
		  unsigned char red, unsigned char green, unsigned char blue)
{
	int x1, x2;
	int y;
	float wid;
    for (y=yc-r; y<=yc+r; ++y)
    {
		wid = (float)sqrt((float)(SQR(r) - SQR(y - yc))); 
		x1 = ROUND(xc - wid);
		x2 = ROUND(xc + wid);
		lgui_hline(x1, y, x2-x1, 1, red, green, blue);
    }
}

void lgui_draw_font(int x, int y, const char *utf8, Font *f, Color c)
{
	const char *p;
	unsigned int val;
	int adv;
	int xadvance, yadvance, width, height, baselinedy;
	unsigned char *data;
	int fontHeight;
	
	fontHeight = font_getHeight(f);

	p = utf8;
	while (*p != 0) {
		val = UTF8toUTF32(p, &adv);
		data = (unsigned char *)font_getGlyph(f, val, A4, &width, &height,
				&xadvance, &yadvance, &baselinedy);
		if (data == NULL)
			emo_printf(" Glyph missing");
		else
			lgui_luminence_A4_blitC(x, y+fontHeight-baselinedy, 0, 0, width, height,
					width, height, data, c);
		x += xadvance;
		y += yadvance;
		p += adv;
	}
}

void lgui_measure_font(const char *utf8, Font *f, IPoint *output)
{
	const char *p;
	unsigned int val;
	int adv;
	int xadvance, yadvance, width, height, baselinedy;
	unsigned char *data;
	int fontHeight;
	
	fontHeight = font_getHeight(f);
	output->x = 0;
	output->y = fontHeight+1;


	p = utf8;
	while (*p != 0) {
		val = UTF8toUTF32(p, &adv);
		data = (unsigned char *)font_getGlyph(f, val, A4, &width, &height,
				&xadvance, &yadvance, &baselinedy);
		if (data == NULL)
			emo_printf(" Glyph missing");
		else {
			output->x += xadvance;
		}
		p += adv;
	}
}
#define guint int
#define guint8 unsigned char

#include "IMG_C"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define LGUI_WIDTH 320
#define LGUI_HEIGHT 240

#define RGB_TO_565(r,g,b) (((r << 8) & 0xF800) | ((g << 3) & 0x7E0) | ((b >> 3) & 0x1F))

void imageRGB_to_RGB565(unsigned short *lgui_buffer, 
        int imgwidth, int imgheight, const unsigned char *img)
{
    unsigned short *buf;
    const unsigned char *imgbuf;
    unsigned short pixel;
    int line, col, ypos, imgypos;
    
    ypos = 0;
    imgypos = 0;
    for (line = 0; line < imgheight; ++line) {
        buf = lgui_buffer + ypos*LGUI_WIDTH;
        imgbuf = img + imgypos*imgwidth*3;

        for (col = 0; col < imgwidth; ++col) {
                pixel = RGB_TO_565(imgbuf[0], imgbuf[1], imgbuf[2]);
                //if (pixel > 0)
                *buf = pixel;
            ++buf;
            imgbuf +=3;
        }
        ++ypos;
        ++imgypos;
    }
}

void imageRGBA_to_A4(unsigned char *lgui_buffer, 
        int imgwidth, int imgheight, const unsigned char *img)
{
    unsigned char *buf;
    const unsigned char *imgbuf;
    unsigned char pixel, outpixel;
    int line, col, ypos, imgypos;
    
    ypos = 0;
    imgypos = 0;
    buf = lgui_buffer;
    imgbuf = img;
    for (line = 0; line < imgheight; ++line) {
        //buf = lgui_buffer + ypos*LGUI_WIDTH;
        //imgbuf = img + imgypos*imgwidth*4;

        for (col = 0; col < imgwidth; col += 2) {
                pixel = imgbuf[3];
                outpixel = pixel & 0xF0;
                imgbuf += 4;
                pixel = imgbuf[3];
                outpixel |= (pixel >> 4) & 0x0F;               

                *buf = outpixel;
            ++buf;
            imgbuf +=4;
        }
        ++ypos;
        ++imgypos;
    }
}

typedef enum {RGB565, A4} Mode;

int main(int argc, char **argv)
{
    unsigned char *output;
    int i, max, lpos;
    unsigned char pixel;
    int bpp;
    Mode RunMode = RUN_MODE;

    if (RunMode == RGB565) {
        bpp = 2;
        output = (unsigned char *)malloc(gimp_image.width*gimp_image.height*2);
        imageRGB_to_RGB565((unsigned short *)output, gimp_image.width,
                gimp_image.height, gimp_image.pixel_data);
    } else if (RunMode == A4) {
        bpp = 1;
        output = (unsigned char *)malloc(gimp_image.width/2*gimp_image.height);
        imageRGBA_to_A4((unsigned char *)output, gimp_image.width,
                gimp_image.height, gimp_image.pixel_data);
    }

    printf("/* ryank converted with cvt */\n\n");
    printf("static const struct {\n");
    printf("  int\twidth;\n");
    printf("  int\theight;\n");
    printf("  int\tbytes_per_pixel;\n");
    if (RunMode == A4) {
        printf("  unsigned char\tpixel_data[%d * %d * %d];\n", gimp_image.width/2,
                gimp_image.height, 1);
    } else {
        printf("  unsigned char\tpixel_data[%d * %d * %d];\n", gimp_image.width,
                gimp_image.height, bpp);
    }
    printf("} %s = {\n", argv[1]);
    printf("  %d, %d, %d, {\n", gimp_image.width, gimp_image.height, bpp);

    if (RunMode == A4) 
        max = gimp_image.height * gimp_image.width/2;
    else
        max = gimp_image.height * gimp_image.width * bpp;
    lpos = 0;
    for (i = 0; i < max; ++i) {
        if (lpos == 0)
            printf("  ");
                
        pixel = output[i];
        printf("0x%02X, ", pixel);
        lpos += 6;

        if (lpos > 70) {
            printf("\n");
            lpos = 0;
        }
    }
    if (lpos != 0)
        printf("\n");
    printf("} };\n");

    return 0;
}

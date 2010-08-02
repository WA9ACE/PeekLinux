#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define guint unsigned int
#define guint8 unsigned char

#include "icons/battcharge4.c"
 
static void rgba_to_trgb(unsigned int * px, int width, int height)
{
  unsigned int i,a,r,g,b;
  for(i = 0; i < (height * width); i++) {
	r = (*(px+i) & 0x000000ff);
	a = ((*(px+i) & 0xff000000) >> 24);
	b = ((*(px+i) & 0x00ff0000) >> 16);
	g = ((*(px+i) & 0x0000ff00) >> 8);
	*(px+i) = (a ^ 0xFF) | (r << 8) | (g << 16) | (b << 24);
  }
}

int main(int argc, char * argv[]) 
{ 
    unsigned char pixel;
    int i, lpos;
    int size = ((gimp_image.width * gimp_image.height) * gimp_image.bytes_per_pixel);
    char *buf = malloc(size);
    memcpy(buf, gimp_image.pixel_data, size);
    rgba_to_trgb((unsigned int *)buf, gimp_image.width, gimp_image.height);

    printf("static const struct {\n");
    printf("  int\twidth;\n");
    printf("  int\theight;\n");
    printf("  int\tbytes_per_pixel;\n");
    printf("  unsigned char\tpixel_data[%d * %d * %d];\n", gimp_image.width, gimp_image.height, gimp_image.bytes_per_pixel);
    printf("} gimp_image = {\n");
    printf("  %d, %d, %d, {\n", gimp_image.width, gimp_image.height, gimp_image.bytes_per_pixel);

    lpos = 0;
    for (i = 0; i < size; ++i) {
        if (lpos == 0)
            printf("  ");

        pixel = buf[i];
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


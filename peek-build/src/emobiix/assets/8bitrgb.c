#define gint int
#define guint unsigned int
#define guint8 unsigned char

#include "sms.c"

#include <stdio.h>

int main(int argc, char **argv)
{
    int x, y;
    int pos, lpos;
    unsigned char out;

    printf("/* ryank converted with 8bitrgb */\n\n");
    printf("static const struct {\n");
    printf("  int\twidth;\n");
    printf("  int\theight;\n");
    printf("  unsigned char\tpixel_data[%d * %d];\n", gimp_image.width,
            gimp_image.height, 1);
    printf("} %s = {\n", "image_name");
    printf("  %d, %d, {\n", gimp_image.width, gimp_image.height);

    pos = 0;
    lpos = 0;
    for (y = 0; y < gimp_image.height; ++y) {
        for (x = 0; x < gimp_image.width; ++x) {
            out = gimp_image.pixel_data[pos++] & 0xE0;
            out |= (gimp_image.pixel_data[pos++] >> 3) & 0x1C;
            out |= (gimp_image.pixel_data[pos++] >> 6) & 0x03;
            printf("0x%02X, ", out);

            ++lpos;            

            if (lpos == 30) {
                printf("\n");
                lpos = 0;
            }
        }
    }

    if (lpos != 0)
        printf("\n");
    printf("} };\n");
}

#ifndef _FONT_H_
#define _FONT_H_

#include "DataObject_internal.h"
#include "Color.h"

#ifdef __cplusplus
extern "C" {
#endif

struct Font_t;
typedef struct Font_t Font;

Font *font_load(DataObject *dobj);
void font_setHeight(Font *f, int height);
int font_getHeight(Font *f);
void *font_getGlyph(Font *f, unsigned int utf32, int isBold,
		PixelFormat pf, int *width, int *height,
		int *xadvance, int *yadvance, int *baselinedy);

unsigned int UTF8toUTF32(const char *utf8, int *advance);

#ifdef __cplusplus
}
#endif

#endif /* _FONT_H_ */


#ifndef _BITMAPCACHE_H_
#define _BITMAPCACHE_H_

#include "Color.h"

#ifdef __cplusplus
extern "C" {
#endif

struct BitmapCache_t;
typedef struct BitmapCache_t BitmapCache;

BitmapCache *bitmapcache_new(void);
void *bitmapcache_add(BitmapCache *c, unsigned long value, int isBold,
		void *data, PixelFormat pf, int width, int height,
		int xadvance, int yadvance, int pitch, int baselinedy);
int bitmapcache_get(BitmapCache *c, unsigned long value, int isBold,
		void **data, PixelFormat *pf, int *width, int *height,
		int *xadvance, int *yadvance, int *baselinedy);

#ifdef __cplusplus
}
#endif

#endif /* _BITMAPCACHE_H_ */


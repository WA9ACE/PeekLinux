#include "BitmapCache.h"
#include "Map.h"
#include "Debug.h"

#include "p_malloc.h"
#include <stdio.h>

/*#define BITMAPCACHE_STEP 1024*/
#define BITMAPCACHE_STEP 8192 

struct Bitmap_t {
	int data;
	int width, height;
	int xadvance, yadvance;
	int baselinedy;
	PixelFormat pf;
	int isBold;
};
typedef struct Bitmap_t Bitmap;

struct BitmapCache_t {
	unsigned char *data;
	size_t dataPos;
	size_t dataSize;
	Map *cache;
};

BitmapCache *bitmapcache_new(void)
{
	BitmapCache *output;

	output = (BitmapCache *)p_malloc(sizeof(BitmapCache));
	output->data = NULL;
	output->dataSize = 0;
	output->dataPos = 0;
	output->cache = map_int();

	return output;
}

void *bitmapcache_add(BitmapCache *c, unsigned long value, int isBold,
		void *data, PixelFormat pf, int width, int height,
		int xadvance, int yadvance, int pitch, int baselinedy)
{
	Bitmap *b;
	int dataSize;
	int i, j;
	unsigned char *src, *dest, *rsrc;
	unsigned char val;
	int oddWidth;

	EMO_ASSERT_NULL(c != NULL,
			"Bitmap Cache NULL when adding bitmap")

	b = (Bitmap *)p_malloc(sizeof(Bitmap));
	b->pf = A4;
	b->xadvance = xadvance;
	b->yadvance = yadvance;
	b->width = width;
	b->height = height;
	b->baselinedy = baselinedy;
	b->isBold = isBold;

	dataSize = ((width+1)>>1) * height;

# if 1
	if (c->dataPos+dataSize > c->dataSize) {
		c->data = (void *)p_realloc(c->data, c->dataSize + BITMAPCACHE_STEP);
		c->dataSize += BITMAPCACHE_STEP;
		if (c->data == NULL) {
			emo_printf("realloc bitmapcache failed" NL);
			return NULL;
		}
	}
#endif

	b->data = c->dataPos;

	//emo_printf("A4 image ");

	src = data;
	dest = c->data+b->data;
	oddWidth = (b->width % 2) == 1;
	for (i = 0; i < b->height; ++i) {
		rsrc = src;
		for (j = 0; j < b->width; j += 2) {
			if (oddWidth && j == b->width-1) {
				val = (*rsrc) & 0xF0;
				++rsrc;
			} else {
				val = (*rsrc) & 0xF0;
				++rsrc;
				val |= ((*rsrc) >> 4) & 0x0F;
				++rsrc;
			}
			//emo_printf("%02X ", val);
			*dest = val;
			++dest;
		}
		src += pitch;
	}
#if 1
	c->dataPos += dataSize;
#endif

	//emo_printf("" NL);
#if 1
	map_append(c->cache, (void *)(value | (isBold ? 0x80000000 : 0)), (void *)b);
#endif
	return c->data+b->data;
}

int bitmapcache_get(BitmapCache *c, unsigned long value, int isBold, 
		void **data, PixelFormat *pf, int *width, int *height,
		int *xadvance, int *yadvance, int *baselinedy)
{
	Bitmap *b;

	EMO_ASSERT_INT(c != NULL, 0,
			"Bitmap Cache NULL when getting bitmap")
	EMO_ASSERT_INT(data != NULL, 0,
			"Bitmap Cache supplied data NULL when getting bitmap")
	EMO_ASSERT_INT(pf != NULL, 0,
			"Bitmap Cache supplied pixelformat NULL when getting bitmap")
	EMO_ASSERT_INT(width != NULL, 0,
			"Bitmap Cache supplied width NULL when getting bitmap")
	EMO_ASSERT_INT(height != NULL, 0,
			"Bitmap Cache supplied height NULL when getting bitmap")
	EMO_ASSERT_INT(xadvance != NULL, 0,
			"Bitmap Cache supplied xadvance NULL when getting bitmap")
	EMO_ASSERT_INT(yadvance != NULL, 0,
			"Bitmap Cache supplied yadvance NULL when getting bitmap")
	EMO_ASSERT_INT(baselinedy != NULL, 0,
			"Bitmap Cache supplied baselinedy NULL when getting bitmap")

	b = map_find(c->cache, (void *)(value | (isBold ? 0x80000000 : 0)));
	
	if (b == NULL)
		return 0;

	*data = c->data+b->data;
	*pf = b->pf;
	*width = b->width;
	*height = b->height;
	*xadvance = b->xadvance;
	*yadvance = b->yadvance;
	*baselinedy = b->baselinedy;

	return 1;
}

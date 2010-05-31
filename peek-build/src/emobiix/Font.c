#include "emobiix_font.h"
#include "Font.h"
#include "BitmapCache.h"
#include "DataObject.h"
#include "Debug.h"
#include "p_malloc.h"

static FT_Library ftlibrary;
static int ftInit = 0;

struct Font_t {
	FT_Face face;
	int height;
	BitmapCache *cache;
};

Font *font_load(DataObject *dobj)
{
	int error;
	void *data;
	size_t size;
	Font *output;

	if (!ftInit) {
		ftlibrary = *init_freetype();
		ftInit = 1;
	}
	
	output = (Font *)p_malloc(sizeof(Font));
	if (output == NULL) {
		emo_printf("Font alloc null" NL);
		return NULL;
	}

	data = dataobject_getValue(dobj, "data")->field.data.bytes;
	size = (size_t)dataobject_getValue(dobj, "data")->field.data.size;

	error = FT_New_Memory_Face(ftlibrary,
			data, size, 0, &output->face);
	if (error) {
		emo_printf("Failed to load font face: %d" NL, error); 
		return NULL;
	}

	output->cache = bitmapcache_new();

	return output;
}

void font_setHeight(Font *f, int height)
{
	int error;
	
	f->height = height;
	error = FT_Set_Pixel_Sizes(f->face, 0, height);
	if (error)
		emo_printf("Failed to set font height" NL);
}
int font_getHeight(Font *f)
{
	return f->height;
}

void *font_getGlyph(Font *f, unsigned int utf32,
		PixelFormat pf, int *width, int *height,
		int *xadvance, int *yadvance, int *baselinedy)
{
	int error;
	void *data;
	PixelFormat pformat;

#if 1
	if (bitmapcache_get(f->cache, utf32,
			&data, &pformat, width, height,
			xadvance, yadvance, baselinedy)) {
		return data;
	}
#endif

	error = FT_Load_Char(f->face, utf32, FT_LOAD_RENDER );
	if (error) {
		emo_printf("Failed to FT load char" NL);
	}

	if(f->face->glyph->bitmap.pixel_mode != FT_PIXEL_MODE_GRAY) {
        emo_printf(" font didnt come back as greys" NL);
        return NULL;
    }
	
#if 0
	emo_printf("FontGlyph(");
	for (i = 0; i < f->face->glyph->bitmap.width*f->face->glyph->bitmap.rows; ++i)
		emo_printf("0x%02X ", 	f->face->glyph->bitmap.buffer[i]);
	emo_printf(")" NL);
#endif

#if 1
	data = bitmapcache_add(f->cache, utf32,
		f->face->glyph->bitmap.buffer, pf,
		f->face->glyph->bitmap.width,
		f->face->glyph->bitmap.rows,
		f->face->glyph->advance.x >> 6,
		f->face->glyph->advance.y >> 6,
		f->face->glyph->bitmap.pitch,
		f->face->glyph->bitmap_top);
#else
	data = f->face->glyph->bitmap.buffer;
#endif

	*width = f->face->glyph->bitmap.width;
	*height = f->face->glyph->bitmap.rows;
	*xadvance = f->face->glyph->advance.x >> 6;
	*yadvance = f->face->glyph->advance.y >> 6;
	*baselinedy = f->face->glyph->bitmap_top;

    return data;            
}

int get_length(unsigned char ch)
{
  int l;
  unsigned char c = ch;
  c >>= 3;

  if (c == 0x1e)
    {
      l = 4;
    }
      else {
c >>= 1;
if (c == 0xe)
  {
    l = 3;
  }
else {
  c >>= 1;
  if (c == 0x6)
    {
      l = 2;
    }
  else {
    l = 1;
  }
}
      }
  return l;
}

unsigned int UTF8toUTF32(const char *utf8, int *advance)
{
	const unsigned char *p = (const unsigned char *)utf8;
	unsigned int ch;
	int l;
	int y;
  
	l = get_length(*p);
	*advance = l;

	switch (l) {
		case 4:
			ch = (*p ^ 0xf0);
			break;
		case 3:
			ch = (*p ^ 0xe0);
			break;
		case 2:
			ch = (*p ^ 0xc0);
			break;
		case 1:
			ch = *p;
			break;
		default:
			break;
	}
    ++p;
    
	for (y = l; y > 1; y--) {
		ch <<= 6;
		ch |= (*p ^ 0x80);
		++p;
	}

	return ch;
}

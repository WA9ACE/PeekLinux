#include "emobiix_font.h"
#include "Font.h"
#include "BitmapCache.h"
#include "DataObject.h"
#include "Debug.h"
#include "File.h"

#include "p_malloc.h"

#include "freetype/ftsystem.h"
#include FT_OUTLINE_H

static FT_Library ftlibrary;
static int ftInit = 0;

struct Font_t {
	FT_Face face;
	int height;
	BitmapCache *cache;
};

/* stream functions */
static int fsi_new_FT_Stream(const char *filename, FT_Stream stream);
static void fsi_FT_Stream_CloseFunc(FT_Stream stream);
static unsigned long fsi_FT_Stream_IoFunc(FT_Stream stream,
		unsigned long offset, unsigned char* buffer,
		unsigned long count);

#undef __FTERRORS_H__
#define FT_ERRORDEF( e, v, s )  { e, s },
#define FT_ERROR_START_LIST     {
#define FT_ERROR_END_LIST       { -1, 0 } };

static const char *
FtError(FT_Error fterr)
{
    struct ft_errors {                                          
	int          code;             
	const char*  msg;
    };
    static struct ft_errors ft_err_mesgs[] = 
#include FT_ERRORS_H            

    struct ft_errors *fp;
    for (fp = ft_err_mesgs; fp->msg != NULL; fp++) {
	if (fp->code == fterr) {
	    return fp->msg;
	}
    }
    return "unknown Freetype error";
}

Font *font_load(DataObject *dobj)
{
	FT_Open_Args *openArgs;
	FT_Stream stream;
	int error;
	char *data;
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

	data = dataobject_getValue(dobj, "data")->field.string;
	if (data == NULL) {
		emo_printf("Attempting to load a NULL font file name" NL);
		return NULL;
	}
	openArgs = (FT_Open_Args *)p_malloc(sizeof(FT_Open_Args));
	stream = (FT_Stream)p_malloc(sizeof(FT_StreamRec));
	openArgs->stream = stream;
	if (!fsi_new_FT_Stream(data, stream)) {
		emo_printf("Failed to create stream for font %s" NL, data);
		return NULL;
	}
	openArgs->flags = FT_OPEN_STREAM;

	error = FT_Open_Face(ftlibrary, openArgs, 0, &output->face);
	if (error) {
		emo_printf("Failed to load font face (%s) : %s" NL, data, FtError(error)); 
		return NULL;
	}

	/*error = FT_New_Memory_Face(ftlibrary,
			data, size, 0, &output->face);
	if (error) {
		emo_printf("Failed to load font face: %d" NL, error); 
		return NULL;
	}*/

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

void *font_getGlyph(Font *f, unsigned int utf32, int isBold,
		PixelFormat pf, int *width, int *height,
		int *xadvance, int *yadvance, int *baselinedy)
{
	int error, glyph_index;
	void *data;
	PixelFormat pformat;

#if 1
	if (bitmapcache_get(f->cache, utf32, isBold,
			&data, &pformat, width, height,
			xadvance, yadvance, baselinedy)) {
		return data;
	}
#endif

	glyph_index = FT_Get_Char_Index(f->face, utf32);
	if (glyph_index == 0) {
		emo_printf("Glyph missing for utf32: %d" NL, utf32);	
		return NULL;
	}
	error = FT_Load_Glyph(f->face, glyph_index, FT_LOAD_DEFAULT | FT_LOAD_TARGET_NORMAL | FT_LOAD_FORCE_AUTOHINT);
	if (error) {
		emo_printf("Failed to FT load char: %s" NL, FtError(error));
		return NULL;
	}
	if (isBold) {
		error = FT_Outline_Embolden(&f->face->glyph->outline, 60);
		if (error)
			emo_printf("Embolden didnt work: %s" NL, FtError(error));
	}
	error = FT_Render_Glyph(f->face->glyph, FT_RENDER_MODE_NORMAL);
	if (error) {
		emo_printf("Failed to render glyph: %s" NL, FtError(error));
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
	data = bitmapcache_add(f->cache, utf32, isBold, 
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

static int fsi_new_FT_Stream(const char *filename, FT_Stream output)
{
	File *fs;

	fs = file_openRead(filename);
	if (fs == NULL)
		return 0;
	emo_printf("Opened file: %p" NL, fs);

	output->base = NULL;
	output->size = file_size(fs);
	output->pos = 0;
	output->descriptor.pointer = fs;
	output->pathname.pointer = NULL;
	output->read = fsi_FT_Stream_IoFunc;
	output->close = fsi_FT_Stream_CloseFunc;
	emo_printf("Stream loaded: %p" NL, output);
	return 1;
}

static void fsi_FT_Stream_CloseFunc(FT_Stream  stream)
{
	FT_StreamRec *rec;

	emo_printf("Close stream: %p" NL, stream);
	rec = (FT_StreamRec *)&stream;

	emo_printf("Close file: %p" NL, rec->descriptor.pointer);
	file_close((File *)rec->descriptor.pointer);
}

static unsigned long fsi_FT_Stream_IoFunc(FT_Stream rec,
		unsigned long offset, unsigned char* buffer,
		unsigned long count)
{
	File *f;
	emo_printf("Stream IO %p: @%d size: %d" NL, rec, offset, count);
	if (rec->descriptor.pointer == NULL) {
		emo_printf("font io file pointer is NULL" NL);
		return 0;
	}
	f = (File *)rec->descriptor.pointer;
	file_seek(f, offset, FP_START);
	if (count > 0)
		return file_read(f, count, buffer);

	return 0;
}


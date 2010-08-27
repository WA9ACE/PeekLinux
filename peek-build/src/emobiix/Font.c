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

	EMO_ASSERT_NULL(dobj != NULL, "loading from from NULL object")

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
	
	EMO_ASSERT(f != NULL, "setting height on NULL font")

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

	EMO_ASSERT_NULL(f != NULL, "getting glyph from NULL font")
	EMO_ASSERT_NULL(width != NULL, "getting glyph missing width")
	EMO_ASSERT_NULL(height != NULL, "getting glyph missing height")
	EMO_ASSERT_NULL(xadvance != NULL, "getting glyph missing xadvance")
	EMO_ASSERT_NULL(yadvance != NULL, "getting glyph missing yadvance")
	EMO_ASSERT_NULL(baselinedy != NULL, "getting glyph missing baselinedy")

#if 1
	if (bitmapcache_get(f->cache, utf32, isBold,
			&data, &pformat, width, height,
			xadvance, yadvance, baselinedy)) {
		return data;
	}
#endif

	glyph_index = FT_Get_Char_Index(f->face, utf32);
	if (glyph_index == 0) {
		if (utf32 != 10 && utf32 != 13)
			emo_printf("Glyph missing for utf32: %d" NL, utf32);
		return NULL;
	}
	error = FT_Load_Glyph(f->face, glyph_index, FT_LOAD_DEFAULT | FT_LOAD_TARGET_NORMAL | FT_LOAD_FORCE_AUTOHINT);
	if (error) {
		emo_printf("Failed to FT load char: %s" NL, FtError(error));
		return NULL;
	}
	if (isBold) {
		error = FT_Outline_Embolden(&f->face->glyph->outline, 30);
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

static const char trailingBytesForUTF8[256] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
};
static const unsigned int offsetsFromUTF8[6] = { 0x00000000UL, 0x00003080UL, 0x000E2080UL, 
		     0x03C82080UL, 0xFA082080UL, 0x82082080UL };
#define UNI_SUR_HIGH_START  (unsigned int)0xD800
#define UNI_SUR_HIGH_END    (unsigned int)0xDBFF
#define UNI_SUR_LOW_START   (unsigned int)0xDC00
#define UNI_SUR_LOW_END     (unsigned int)0xDFFF
#define UNI_REPLACEMENT_CHAR  (unsigned int)0x0000FFFD 

unsigned int UTF8toUTF32(const char *isource, int *advance)
{
	const unsigned char *source;
	unsigned short extraBytesToRead;
	unsigned int ch;

	EMO_ASSERT_INT(isource != NULL, 0, "UTF8->UTF32 missing UTF8")
	EMO_ASSERT_INT(advance != NULL, 0, "UTF8->UTF32 missing advance")

	source = (const unsigned char *)isource;

	ch = 0;
	extraBytesToRead = trailingBytesForUTF8[*source];
	*advance = extraBytesToRead+1;

	switch (extraBytesToRead) {
	    case 5: ch += *source++; ch <<= 6;
	    case 4: ch += *source++; ch <<= 6;
	    case 3: ch += *source++; ch <<= 6;
	    case 2: ch += *source++; ch <<= 6;
	    case 1: ch += *source++; ch <<= 6;
	    case 0: ch += *source++;
	}
	ch -= offsetsFromUTF8[extraBytesToRead];

    if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END)
	    ch = UNI_REPLACEMENT_CHAR;
    
    return ch;
}

static int fsi_new_FT_Stream(const char *filename, FT_Stream output)
{
	File *fs;

	EMO_ASSERT_INT(filename != NULL, 0, "new FT stream missing filename")
	EMO_ASSERT_INT(output != NULL, 0, "new FT stream missing stream")

	fs = file_openRead(filename);
	if (fs == NULL)
		return 0;
	//emo_printf("Opened file: %p" NL, fs);

	output->base = NULL;
	output->size = file_size(fs);
	output->pos = 0;
	output->descriptor.pointer = fs;
	output->pathname.pointer = NULL;
	output->read = fsi_FT_Stream_IoFunc;
	output->close = fsi_FT_Stream_CloseFunc;

	//emo_printf("Stream loaded: %p" NL, output);
	return 1;
}

static void fsi_FT_Stream_CloseFunc(FT_Stream  stream)
{
	FT_StreamRec *rec;

	EMO_ASSERT(stream != NULL, "close FT stream missing stream")

	//emo_printf("Close stream: %p" NL, stream);
	rec = (FT_StreamRec *)&stream;

	//emo_printf("Close file: %p" NL, rec->descriptor.pointer);
	file_close((File *)rec->descriptor.pointer);
}

static unsigned long fsi_FT_Stream_IoFunc(FT_Stream rec,
		unsigned long offset, unsigned char* buffer,
		unsigned long count)
{
	File *f;

	EMO_ASSERT_INT(rec != NULL, 0, "IO on FT stream missing stream")
	
	//emo_printf("Stream IO %p: @%d size: %d" NL, rec, offset, count);
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


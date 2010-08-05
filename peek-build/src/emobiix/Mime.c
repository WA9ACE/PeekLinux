#include "Mime.h"

#include "DataObject.h"
#include "Debug.h"
#include "Widget.h"
#include "Color.h"
#include "lgui.h"

#include "p_malloc.h"

#include "png.h"

#include <stdio.h>
#include <string.h>

static int load_png(DataObject *dobj);
static unsigned char *imageConvert(void *input, int inputFormat, int outputFormat,
		int width, int height, int *outSize);

int mime_load(DataObject *dobj)
{
	DataObjectField *mimeType;

	mimeType = dataobject_getValue(dobj, "mime-type");
	if (mimeType == NULL)
		return 0;

	if (strcmp("png", mimeType->field.string) == 0) {
		return load_png(dobj);
	} else {
		emo_printf("Unknown mime: %s" NL, mimeType->field.string);	
		dataobject_debugPrint(dobj);
	}

	return 0;
}

void mime_loadAll(DataObject *dobj)
{
	ListIterator iter;

	mime_load(dobj);
	
	widget_getChildren(dobj, &iter);
	while (!listIterator_finished(&iter)) {
		mime_loadAll((DataObject *)listIterator_item(&iter));
		listIterator_next(&iter);
	}
}

struct PNG_memory_t {
	unsigned char *ptr;
	int size;
	int pos;
};
typedef struct PNG_memory_t PNG_memory_t;

static void readFromPNGmemory(png_structp png_ptr, png_bytep data, png_size_t length)
{
	PNG_memory_t *mem;

	mem = (PNG_memory_t *)png_get_io_ptr(png_ptr);
	
	if (mem == NULL)
        return;

	if (mem->pos >= mem->size)
		return;

	if ((int)length > mem->size-mem->pos) {
		memcpy(data, mem->ptr+mem->pos, mem->size-mem->pos);
		mem->pos = mem->size;
	} else {
		memcpy(data, mem->ptr+mem->pos, length);
		mem->pos += length;
	}
}

static void user_error_fn(png_structp png, png_const_charp sz)
{
	emo_printf("PNG error: %s" NL, sz);
}

static void user_warning_fn (png_structp png, png_const_charp sz)
{
	emo_printf("PNG warning: %s" NL, sz);
}

#define PNG_BYTES_TO_CHECK 4
static int load_png(DataObject *dobj)
{
	DataObjectField *data;
    png_structp     pngPtr;
    png_infop       infoPtr;
    png_uint_32     width, height;
    int             bitDepth, colorType, interlaceType, components;
    unsigned int    i;
    png_bytep       *rowPointers;
	PNG_memory_t	pngReader;
	int outputFormat, inputFormat, outSize;
	unsigned char *output, *convertedOutput;

	data = dataobject_getValue(dobj, "src");
	if (data == NULL || data->type != DOF_DATA)
		return 0;

	if (!png_check_sig(data->field.data.bytes, PNG_BYTES_TO_CHECK)) {
		emo_printf("PNG failed signature check" NL);
        return 0;
	}
 
	/*
     * Create a read and info structure
     */
    pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
			user_error_fn, user_warning_fn);
    if (pngPtr == NULL)
        return 0;
    infoPtr = png_create_info_struct(pngPtr);
    if (infoPtr == NULL) {
        png_destroy_read_struct(&pngPtr, (png_infopp)NULL, (png_infopp)NULL);
        return 0;
    }

    /*
     * Set up our expand settings
     */
    png_set_palette_to_rgb(pngPtr);
    png_set_tRNS_to_alpha(pngPtr);

    /*
     * Init IO and read the header information
     */
	pngReader.ptr = data->field.data.bytes;
	pngReader.size = data->field.data.size;
	pngReader.pos = 0;
	png_set_read_fn(pngPtr, &pngReader, readFromPNGmemory);
    /*png_init_io(pngPtr, input);*/
    /*png_set_sig_bytes(pngPtr, PNG_BYTES_TO_CHECK);*/
    png_read_info(pngPtr, infoPtr);
    png_get_IHDR(pngPtr, infoPtr, &width, &height,
        &bitDepth, &colorType, &interlaceType,
        NULL, NULL);

    /* 
     * Only allow 8 bits per component images
     */
    if (bitDepth != 8) {
        png_destroy_read_struct(&pngPtr, &infoPtr, (png_infopp)NULL);
        return 0;
    }

	/*
     * Check to see if we use an alpha channel or not
     */
    switch(colorType) {
		case PNG_COLOR_TYPE_GRAY:
			components = 1;
			outputFormat = A4;
			inputFormat = A8;
			break;
		case PNG_COLOR_TYPE_GRAY_ALPHA:
			components = 2;
			outputFormat = A4;
			inputFormat = A8A8;
			break;
        case PNG_COLOR_TYPE_RGB:
            components = 3;
			outputFormat = RGB565;
			inputFormat = RGB;
            break;
        case PNG_COLOR_TYPE_RGB_ALPHA:
            components = 4;
			outputFormat = RGB565A8;
			inputFormat = RGBA;
            break;
        case PNG_COLOR_TYPE_PALETTE:
            if (png_get_valid(pngPtr, infoPtr, PNG_INFO_tRNS)) {
                png_set_tRNS_to_alpha(pngPtr);
                components = 4;
				outputFormat = RGB565A8;
				inputFormat = RGBA;
            } else {
                png_set_palette_to_rgb(pngPtr);
                components = 3;
				outputFormat = RGB565;
				inputFormat = RGB;
            }
            break;
        default:
            png_destroy_read_struct(&pngPtr, &infoPtr, (png_infopp)NULL);
            emo_printf("Colour type not supported in PNG" NL);
            return 0;
    }

	/*
     * Malloc output
     */
    output = (unsigned char *)p_malloc(width*height*components);

    /*
     * Alloc the row pointers
     */
    rowPointers = (png_bytep *)p_malloc(height*sizeof(png_bytep));
    for (i = 0; i < height; ++i)
        rowPointers[i] = output+i*width*components;

    /*
     * Read the entire image and shut down png
     */
    png_read_image(pngPtr, rowPointers);
    png_read_end(pngPtr, infoPtr);
    png_destroy_read_struct(&pngPtr, &infoPtr, NULL);

    /*
     * Set return vales
     */
	dataobject_setValue(dobj, "width", dataobjectfield_int(width));
	dataobject_setValue(dobj, "height", dataobjectfield_int(height));
	dataobject_setValue(dobj, "pixelformat", dataobjectfield_int(outputFormat));

    /*
     * Free the row pointers
     */
    p_free(rowPointers);

    convertedOutput = imageConvert(output, inputFormat, outputFormat, width, height,
			&outSize);
	p_free(output);

	dataobject_setValue(dobj, "data", dataobjectfield_data(convertedOutput, outSize));

	p_free(data->field.data.bytes);
	data->field.data.bytes = NULL;
	data->field.data.size = 0;

	return 1;
}

static unsigned char *imageConvert(void *input, int inputFormat, int outputFormat, int width,
		int height, int *outSize)
{
	int pos, upper;
	/*int x, y;*/
	unsigned char *src, *dest;
	unsigned char *output, *ooutput;
	unsigned short pixel;
	unsigned char alpha;

	src = (unsigned char *)input;

	switch (outputFormat) {
		case A4:
			*outSize = (width*height+1)/2;
			break;
		case RGB565:
			*outSize = (width*height)*2;
			break;
		case RGB565A8:
			*outSize = (width*height)*3;
			break;
		default:
			emo_printf("Unknown output format" NL);
			return NULL;
	}
	output = (unsigned char *)p_malloc(*outSize);
	ooutput = output;

	if (outputFormat == A4) {
		if (inputFormat != A8 && inputFormat != A8A8) {
			emo_printf("Unsupported converstion to A4" NL);
			return NULL;
		}
		upper = width*height;
		for (pos = 0; pos < upper; pos += 2) {
			output[pos>>1] = *src & 0xF0;
			++src;
			if (inputFormat == A8A8)
				++src;

			output[pos>>1] |= (*src >> 4);
			++src;
			if (inputFormat == A8A8)
				++src;
		}
		return output;
	}

	upper = width * height;
	dest = output;
	for (pos = 0; pos < upper; ++pos) {
	/*for (y = 0; y < height; ++y) {
		for (x = 0; x < width; ++x) {*/
		switch (inputFormat) {
			case RGB:
				pixel = RGB_TO_565((*src), *(src+1), *(src+2));
				src += 3;
				break;
			case RGBA:
				pixel = RGB_TO_565((*src), *(src+1), *(src+2));
				alpha = *(src+3);
				src += 4;
				break;
			default:
				emo_printf("Unhandled input format" NL);
				return NULL;
		}

		switch (outputFormat) {
			case RGB565:
				*((unsigned short *)output) = pixel;
				output += 2;
				break;
			case RGB565A8:
				*((unsigned char *)(output)) = pixel & 0xFF;
				*((unsigned char *)(output+1)) = (pixel >> 8) & 0xFF;
				*((unsigned char *)(output+2)) = alpha;
				output += 3;
				break;
			default:
				emo_printf("Unhandled output format" NL);
				return NULL;
		}
	/*	}*/
	}

	return ooutput;

}


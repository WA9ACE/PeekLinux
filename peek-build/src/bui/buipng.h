#ifndef UIPNG_H
#define UIPNG_H

#include <stdio.h>
#include <setjmp.h>

#define ZLIB
#ifdef ZLIB
#include "buizlib.h"
#endif

/* Some typedefs to get us started.  These should be safe on most of the
 * common platforms.  The typedefs should be at least as large as the
 * numbers suggest (a png_uint_32 must be at least 32 bits long), but they
 * don't have to be exactly that size.  Some compilers dislike passing
 * unsigned shorts as function parameters, so you may be better off using
 * unsigned int for png_uint_16.  Likewise, for 64-bit systems, you may
 * want to have unsigned int for png_uint_32 instead of unsigned long.
 */
typedef unsigned long png_uint_32;
typedef long png_int_32;
typedef unsigned short png_uint_16;
typedef short png_int_16;
typedef unsigned char png_byte;
typedef png_int_32 png_fixed_point;

/* Add typedefs for pointers */
typedef void* png_voidp;
typedef png_byte* png_bytep;
typedef png_uint_32* png_uint_32p;
typedef png_int_32* png_int_32p;
typedef png_uint_16* png_uint_16p;
typedef png_int_16* png_int_16p;
typedef const char* png_const_charp;
typedef char* png_charp;
typedef png_fixed_point* png_fixed_point_p;

/* This is usually size_t.  It is typedef'ed just in case you need it to
   change (I'm not sure if you will or not, so I thought I'd be safe) */
typedef size_t png_size_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Three color definitions.  The order of the red, green, and blue, (and the
 * exact size) is not important, although the size of the fields need to
 * be png_byte or png_uint_16 (as defined below).
 */
typedef struct
{
  png_byte red;
  png_byte green;
  png_byte blue;
} png_color;
typedef png_color* png_colorp;

/* These determine if an ancillary chunk's data has been successfully read
 * from the PNG header, or if the application has filled in the corresponding
 * data in the info_struct to be written into the output file.  The values
 * of the PNG_INFO_<chunk> defines should NOT be changed.
 */
#define PNG_INFO_gAMA 0x0001
#define PNG_INFO_sBIT 0x0002
#define PNG_INFO_cHRM 0x0004
#define PNG_INFO_PLTE 0x0008
#define PNG_INFO_tRNS 0x0010
#define PNG_INFO_bKGD 0x0020
#define PNG_INFO_hIST 0x0040
#define PNG_INFO_pHYs 0x0080
#define PNG_INFO_oFFs 0x0100
#define PNG_INFO_tIME 0x0200
#define PNG_INFO_pCAL 0x0400
#define PNG_INFO_sRGB 0x0800   
#define PNG_INFO_iCCP 0x1000   
#define PNG_INFO_sPLT 0x2000   
#define PNG_INFO_sCAL 0x4000   
#define PNG_INFO_IDAT 0x8000L

/* This is used for the transformation routines, as some of them
 * change these values for the row.  It also should enable using
 * the routines for other purposes.
 */
typedef struct
{
  png_uint_32 width; /* width of row */
  png_uint_32 rowbytes; /* number of bytes in row */
  png_byte color_type; /* color type of row */
  png_byte bit_depth; /* bit depth of row */
  png_byte channels; /* number of channels (1, 2, 3, or 4) */
  png_byte pixel_depth; /* bits per pixel (depth * channels) */
} png_row_info;

typedef png_row_info* png_row_infop;
typedef png_row_info** png_row_infopp;

/* png_info is a structure that holds the information in a PNG file so
 * that the application can find out the characteristics of the image.
 * If you are reading the file, this structure will tell you what is
 * in the PNG file.  If you are writing the file, fill in the information
 * you want to put into the PNG file, then call png_write_info().
 * The names chosen should be very close to the PNG specification, so
 * consult that document for information about the meaning of each field.
 */
typedef struct
{
  /* the following are necessary for every PNG file */
  png_uint_32 width;       /* width of image in pixels (from IHDR) */
  png_uint_32 height;      /* height of image in pixels (from IHDR) */
  png_uint_32 valid;       /* valid chunk data (see PNG_INFO_ below) */
  png_uint_32 rowbytes;    /* bytes needed to hold an untransformed row */
  png_colorp palette;      /* array of color values (valid & PNG_INFO_PLTE) */
  png_uint_16 num_palette; /* number of color entries in "palette" (PLTE) */
  png_uint_16 num_trans;   /* number of transparent palette color (tRNS) */
  png_byte bit_depth;      /* 1, 2, 4, 8, or 16 bits/channel (from IHDR) */
  png_byte color_type;     /* see PNG_COLOR_TYPE_ below (from IHDR) */

  /* The following three should have been named *_method not *_type */
  png_byte compression_type; /* must be PNG_COMPRESSION_TYPE_BASE (IHDR) */
  png_byte filter_type;    /* must be PNG_FILTER_TYPE_BASE (from IHDR) */
  png_byte interlace_type; /* One of PNG_INTERLACE_NONE, PNG_INTERLACE_ADAM7 */

  /* The following is informational only on read, and not used on writes. */
  png_byte channels;       /* number of data channels per pixel (1, 2, 3, 4) */
  png_byte pixel_depth;    /* number of bits per pixel */
  png_byte spare_byte;     /* to align the data, and for future use */
  png_byte signature[8];   /* magic bytes read by libpng from start of file */
} png_info;
typedef png_info* png_infop;
typedef png_info** png_infopp;

typedef struct png_struct_def png_struct;
typedef png_struct* png_structp;
typedef png_struct** png_structpp;

typedef void (*png_error_ptr)(png_structp, png_const_charp);
typedef void (*png_rw_ptr)(png_structp, png_bytep, png_size_t);
typedef void (*png_read_status_ptr)(png_structp, png_uint_32, int);

/* The structure that holds the information to read and write PNG files. */
struct png_struct_def
{
  jmp_buf jmpbuf;            /* used in png_error */

  png_error_ptr error_fn;    /* function for printing errors and aborting */
  png_error_ptr warning_fn;  /* function for printing warnings */
  png_voidp error_ptr;       /* user supplied struct for error functions */
  png_rw_ptr write_data_fn;  /* function for writing output data */
  png_rw_ptr read_data_fn;   /* function for reading input data */
  png_voidp io_ptr;          /* ptr to application struct for I/O functions */

  png_uint_32 mode;          /* tells us where we are in the PNG file */
  png_uint_32 flags;         /* flags indicating various things to libpng */
  png_uint_32 transformations; /* which transformations to perform */

#ifdef ZLIB
  z_stream zstream;          /* pointer to decompression structure (below) */
#endif
  png_bytep zbuf;            /* buffer for zlib */
  png_size_t zbuf_size;      /* size of zbuf */
  int zlib_level;            /* holds zlib compression level */
  int zlib_method;           /* holds zlib compression method */
  int zlib_window_bits;      /* holds zlib compression window bits */
  int zlib_mem_level;        /* holds zlib compression memory level */
  int zlib_strategy;         /* holds zlib compression strategy */

  png_uint_32 width;         /* width of image in pixels */
  png_uint_32 height;        /* height of image in pixels */
  png_uint_32 num_rows;      /* number of rows in current pass */
  png_uint_32 usr_width;     /* width of row at start of write */
  png_uint_32 rowbytes;      /* size of row in bytes */
  png_uint_32 irowbytes;     /* size of current interlaced row in bytes */
  png_uint_32 iwidth;        /* width of current interlaced row in pixels */
  png_uint_32 row_number;    /* current row in interlace pass */
  png_bytep prev_row;        /* buffer to save previous (unfiltered) row */
  png_bytep row_buf;         /* buffer to save current (unfiltered) row */
  png_bytep sub_row;         /* buffer to save "sub" row when filtering */
  png_bytep up_row;          /* buffer to save "up" row when filtering */
  png_bytep avg_row;         /* buffer to save "avg" row when filtering */
  png_bytep paeth_row;       /* buffer to save "Paeth" row when filtering */
  png_row_info row_info;     /* used for transformation routines */

  png_uint_32 idat_size;     /* current IDAT size for read */
  png_uint_32 crc;           /* current chunk CRC value */
  png_colorp palette;        /* palette from the input file */
  png_uint_16 num_palette;   /* number of color entries in palette */
  png_uint_16 num_trans;     /* number of transparency values */
  png_byte chunk_name[5];    /* null-terminated name of current chunk */
  png_byte compression;      /* file compression type (always 0) */
  png_byte filter;           /* file filter type (always 0) */
  png_byte interlaced;       /* PNG_INTERLACE_NONE, PNG_INTERLACE_ADAM7 */
  png_byte pass;             /* current interlace pass (0 - 6) */
  png_byte do_filter;        /* row filter flags (see PNG_FILTER_ below ) */
  png_byte color_type;       /* color type of file */
  png_byte bit_depth;        /* bit depth of file */
  png_byte usr_bit_depth;    /* bit depth of users row */
  png_byte pixel_depth;      /* number of bits per pixel */
  png_byte channels;         /* number of channels in file */
  png_byte usr_channels;     /* channels at start of write */
  png_byte sig_bytes;        /* magic bytes read/written from start of file */

  png_read_status_ptr read_row_fn;   /* called after each row is decoded */
  png_bytep big_row_buf;             /* buffer to save current (unfiltered) row */
};

/* Maximum positive integer used in PNG is (2^31)-1 */
#define PNG_MAX_UINT ((png_uint_32)0x7fffffffL)
#define PNG_ZBUF_SIZE 8192

/* These describe the color_type field in png_info. */
/* color type masks */
#define PNG_COLOR_MASK_PALETTE    1
#define PNG_COLOR_MASK_COLOR      2
#define PNG_COLOR_MASK_ALPHA      4

/* color types.  Note that not all combinations are legal */
#define PNG_COLOR_TYPE_GRAY       0
#define PNG_COLOR_TYPE_PALETTE    (PNG_COLOR_MASK_COLOR | PNG_COLOR_MASK_PALETTE)
#define PNG_COLOR_TYPE_RGB        (PNG_COLOR_MASK_COLOR)
#define PNG_COLOR_TYPE_RGB_ALPHA  (PNG_COLOR_MASK_COLOR | PNG_COLOR_MASK_ALPHA)
#define PNG_COLOR_TYPE_GRAY_ALPHA (PNG_COLOR_MASK_ALPHA)
/* aliases */
#define PNG_COLOR_TYPE_RGBA  PNG_COLOR_TYPE_RGB_ALPHA
#define PNG_COLOR_TYPE_GA    PNG_COLOR_TYPE_GRAY_ALPHA

/* This is for compression type. PNG 1.0-1.2 only define the single type. */
#define PNG_COMPRESSION_TYPE_BASE    0 /* Deflate method 8, 32K window */
#define PNG_COMPRESSION_TYPE_DEFAULT PNG_COMPRESSION_TYPE_BASE

/* This is for filter type. PNG 1.0-1.2 only define the single type. */
#define PNG_FILTER_TYPE_BASE        0 /* Single row per-byte filtering */
#define PNG_INTRAPIXEL_DIFFERENCING 64 /* Used only in MNG datastreams */
#define PNG_FILTER_TYPE_DEFAULT     PNG_FILTER_TYPE_BASE

/* These are for the interlacing type.  These values should NOT be changed. */
#define PNG_INTERLACE_NONE        0 /* Non-interlaced image */
#define PNG_INTERLACE_ADAM7       1 /* Adam7 interlacing */
#define PNG_INTERLACE_LAST        2 /* Not a valid value */

/* Maximum number of entries in PLTE/sPLT/tRNS arrays */
#define PNG_MAX_PALETTE_LENGTH    256

/* Various modes of operation.  Note that after an init, mode is set to
 * zero automatically when the structure is created.
 */
#define PNG_HAVE_IHDR               0x01
#define PNG_HAVE_PLTE               0x02
#define PNG_HAVE_IDAT               0x04
#define PNG_AFTER_IDAT              0x08
#define PNG_HAVE_IEND               0x10
#define PNG_HAVE_gAMA               0x20
#define PNG_HAVE_cHRM               0x40
#define PNG_HAVE_sRGB               0x80
#define PNG_HAVE_CHUNK_HEADER      0x100
#define PNG_WROTE_tIME             0x200
#define PNG_WROTE_INFO_BEFORE_PLTE 0x400
#define PNG_BACKGROUND_IS_GRAY     0x800
#define PNG_HAVE_PNG_SIGNATURE    0x1000

/* flags for the transformations the PNG library does on the image data */
#define PNG_BGR                0x0001
#define PNG_INTERLACE          0x0002
#define PNG_PACK               0x0004
#define PNG_SHIFT              0x0008
#define PNG_SWAP_BYTES         0x0010
#define PNG_INVERT_MONO        0x0020
#define PNG_DITHER             0x0040
#define PNG_BACKGROUND         0x0080
#define PNG_BACKGROUND_EXPAND  0x0100
                          /*   0x0200 unused */
#define PNG_16_TO_8            0x0400
#define PNG_RGBA               0x0800
#define PNG_EXPAND             0x1000
#define PNG_GAMMA              0x2000
#define PNG_GRAY_TO_RGB        0x4000
#define PNG_FILLER             0x8000L
#define PNG_PACKSWAP          0x10000L
#define PNG_SWAP_ALPHA        0x20000L
#define PNG_STRIP_ALPHA       0x40000L
#define PNG_INVERT_ALPHA      0x80000L
#define PNG_USER_TRANSFORM   0x100000L
#define PNG_RGB_TO_GRAY_ERR  0x200000L
#define PNG_RGB_TO_GRAY_WARN 0x400000L
#define PNG_RGB_TO_GRAY      0x600000L  /* two bits, RGB_TO_GRAY_ERR|WARN */

/* flags for png_create_struct */
#define PNG_STRUCT_PNG   0x0001
#define PNG_STRUCT_INFO  0x0002

/* Scaling factor for filter heuristic weighting calculations */
#define PNG_WEIGHT_SHIFT 8
#define PNG_WEIGHT_FACTOR (1<<(PNG_WEIGHT_SHIFT))
#define PNG_COST_SHIFT 3
#define PNG_COST_FACTOR (1<<(PNG_COST_SHIFT))

/* flags for the png_ptr->flags rather than declaring a byte for each one */
#define PNG_FLAG_ZLIB_CUSTOM_STRATEGY     0x0001
#define PNG_FLAG_ZLIB_CUSTOM_LEVEL        0x0002
#define PNG_FLAG_ZLIB_CUSTOM_MEM_LEVEL    0x0004
#define PNG_FLAG_ZLIB_CUSTOM_WINDOW_BITS  0x0008
#define PNG_FLAG_ZLIB_CUSTOM_METHOD       0x0010
#define PNG_FLAG_ZLIB_FINISHED            0x0020
#define PNG_FLAG_ROW_INIT                 0x0040
#define PNG_FLAG_FILLER_AFTER             0x0080
#define PNG_FLAG_CRC_ANCILLARY_USE        0x0100
#define PNG_FLAG_CRC_ANCILLARY_NOWARN     0x0200
#define PNG_FLAG_CRC_CRITICAL_USE         0x0400
#define PNG_FLAG_CRC_CRITICAL_IGNORE      0x0800
#define PNG_FLAG_FREE_PLTE                0x1000
#define PNG_FLAG_FREE_TRNS                0x2000
#define PNG_FLAG_FREE_HIST                0x4000
#define PNG_FLAG_KEEP_UNKNOWN_CHUNKS      0x8000L
#define PNG_FLAG_KEEP_UNSAFE_CHUNKS       0x10000L
#define PNG_FLAG_LIBRARY_MISMATCH         0x20000L
#define PNG_FLAG_STRIP_ERROR_NUMBERS      0x40000L
#define PNG_FLAG_STRIP_ERROR_TEXT         0x80000L
#define PNG_FLAG_MALLOC_NULL_MEM_OK       0x100000L

/* For use in png_set_keep_unknown, png_handle_as_unknown */
#define HANDLE_CHUNK_AS_DEFAULT   0
#define HANDLE_CHUNK_NEVER        1
#define HANDLE_CHUNK_IF_SAFE      2
#define HANDLE_CHUNK_ALWAYS       3

#define PNG_FLAG_CRC_ANCILLARY_MASK (PNG_FLAG_CRC_ANCILLARY_USE | \
                                     PNG_FLAG_CRC_ANCILLARY_NOWARN)

#define PNG_FLAG_CRC_CRITICAL_MASK  (PNG_FLAG_CRC_CRITICAL_USE | \
                                     PNG_FLAG_CRC_CRITICAL_IGNORE)

#define PNG_FLAG_CRC_MASK           (PNG_FLAG_CRC_ANCILLARY_MASK | \
                                     PNG_FLAG_CRC_CRITICAL_MASK)

/* Flags for png_ptr->free_me and info_ptr->free_me */
#define PNG_FREE_HIST 0x0008
#define PNG_FREE_ICCP 0x0010
#define PNG_FREE_SPLT 0x0020
#define PNG_FREE_ROWS 0x0040
#define PNG_FREE_PCAL 0x0080
#define PNG_FREE_SCAL 0x0100
#define PNG_FREE_UNKN 0x0200
#define PNG_FREE_LIST 0x0400
#define PNG_FREE_PLTE 0x1000
#define PNG_FREE_TRNS 0x2000
#define PNG_FREE_TEXT 0x4000
#define PNG_FREE_ALL  0x7fff
#define PNG_FREE_MUL  0x4220 /* PNG_FREE_SPLT|PNG_FREE_TEXT|PNG_FREE_UNKN */

/* Filter values (not flags) - used in pngwrite.c, pngwutil.c for now.
 * These defines should NOT be changed.
 */
#define PNG_FILTER_VALUE_NONE  0
#define PNG_FILTER_VALUE_SUB   1
#define PNG_FILTER_VALUE_UP    2
#define PNG_FILTER_VALUE_AVG   3
#define PNG_FILTER_VALUE_PAETH 4
#define PNG_FILTER_VALUE_LAST  5

/* Constant strings for known chunk types.  If you need to add a chunk,
 * define the name here, and add an invocation of the macro in uipng.c and
 * wherever it's needed.
 */
#define PNG_IHDR const png_byte png_IHDR[5] = { 73,  72,  68,  82, '\0'}
#define PNG_IDAT const png_byte png_IDAT[5] = { 73,  68,  65,  84, '\0'}
#define PNG_IEND const png_byte png_IEND[5] = { 73,  69,  78,  68, '\0'}
#define PNG_PLTE const png_byte png_PLTE[5] = { 80,  76,  84,  69, '\0'}
#define PNG_bKGD const png_byte png_bKGD[5] = { 98,  75,  71,  68, '\0'}
#define PNG_cHRM const png_byte png_cHRM[5] = { 99,  72,  82,  77, '\0'}
#define PNG_gAMA const png_byte png_gAMA[5] = {103,  65,  77,  65, '\0'}
#define PNG_hIST const png_byte png_hIST[5] = {104,  73,  83,  84, '\0'}
#define PNG_iCCP const png_byte png_iCCP[5] = {105,  67,  67,  80, '\0'}
#define PNG_iTXt const png_byte png_iTXt[5] = {105,  84,  88, 116, '\0'}
#define PNG_oFFs const png_byte png_oFFs[5] = {111,  70,  70, 115, '\0'}
#define PNG_pCAL const png_byte png_pCAL[5] = {112,  67,  65,  76, '\0'}
#define PNG_sCAL const png_byte png_sCAL[5] = {115,  67,  65,  76, '\0'}
#define PNG_pHYs const png_byte png_pHYs[5] = {112,  72,  89, 115, '\0'}
#define PNG_sBIT const png_byte png_sBIT[5] = {115,  66,  73,  84, '\0'}
#define PNG_sPLT const png_byte png_sPLT[5] = {115,  80,  76,  84, '\0'}
#define PNG_sRGB const png_byte png_sRGB[5] = {115,  82,  71,  66, '\0'}
#define PNG_tEXt const png_byte png_tEXt[5] = {116,  69,  88, 116, '\0'}
#define PNG_tIME const png_byte png_tIME[5] = {116,  73,  77,  69, '\0'}
#define PNG_tRNS const png_byte png_tRNS[5] = {116,  82,  78,  83, '\0'}
#define PNG_zTXt const png_byte png_zTXt[5] = {122,  84,  88, 116, '\0'}

png_structp vt_png_create_read_struct(void);
png_infop vt_png_create_info_struct(png_structp png_ptr);
void vt_png_destroy_read_struct(png_structpp png_ptr_ptr, png_infopp info_ptr_ptr, png_infopp end_info_ptr_ptr);
void vt_png_set_read_fn(png_structp png_ptr, png_voidp io_ptr, png_rw_ptr read_data_fn);
void vt_png_set_error_fn(png_structp png_ptr, png_voidp error_ptr, png_error_ptr error_fn, png_error_ptr warning_fn);
void vt_png_read_info(png_structp png_ptr, png_infop info_ptr);
int vt_png_set_interlace_handling(png_structp png_ptr);
void vt_png_read_row(png_structp png_ptr, png_bytep row);
void vt_png_read_end(png_structp png_ptr, png_infop info_ptr);

#ifdef __cplusplus
}
#endif /* __cplusplus */





#endif


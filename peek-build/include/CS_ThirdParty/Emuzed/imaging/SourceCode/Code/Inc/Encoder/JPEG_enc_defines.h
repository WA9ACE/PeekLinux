/*
*******************************************************************************
                       Copyright eMuzed Inc., 2003-2004.
All rights Reserved, Licensed Software Confidential and Proprietary Information
    of eMuzed Incorporation Made available under Non-Disclosure Agreement OR
                            License as applicable.
*******************************************************************************
*/

/*
*******************************************************************************
Product     : Streaming Solutions
Module      : JPEG Image Encoder
File        : JPEG_enc_defines.h
Description : Defines for JPEG Encoder module.

Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date            Id          Author				Comment
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Nov 27, 2001                Sudhindra			Initial code

*******************************************************************************
*/
#ifndef INCLUDE_JPEG_DEFINES
#define INCLUDE_JPEG_DEFINES

#ifndef WIN32
	#define ARM9TDMI_INLINE
#endif


#define PUT_BITS_IN_BUFFER_ON

//#define ROM_RESIDENT_JPEGENCODER
//#define FLASH_RESIDENT_JPEGENCODER
//#define INTERNAL_RAM_RESIDENT_JPEGENCODER
//#define WINDOWS_BUILD
//#define BLOCK_SIZE 8

#define MARKER 1
#define DATA 2

/*General*/
enum {OFF, ON};

#define THUMBNAIL_QFACTOR 50

/*Bit stream formating*/
#define WRITE32(op, x) gPutBits(op, 16, ((uint32)x) >> 16);       \
                       gPutBits(op, 16, x & 0x0000ffff)

#define EOB								0

#define START_OF_IMAGE					0xFFD8
#define END_OF_IMAGE		            0xFFD9

#define BASELINE                        1

#define START_OF_FRAME_BASELINE		    0xFFC0
#define START_OF_FRAME_EXTENDED         0xFFC1
#define START_OF_FRAME_PROGRESSIVE		0xFFC2
#define START_OF_FRAME_LOSSLESS         0xFFC3

#define START_OF_SCAN		            0xFFDA

#define QUANT_TABLE_MARKER	            0xFFDB

#define HUFF_TABLE_MARKER	            0xFFC4
#define DNL_MARKER			            0xFFDC
#define DRI_MARKER			            0xFFDD
#define APPLICATION_MARKER	            0xFFE0
#define APP0                            0xFFE0
#define APP14                           0xFFEE
#define COMMENT				            0xFFFE

/*Transform*/
#define LOG_BLOCK_WIDTH 3       /* log(BLOCK_WIDTH, 2) */
#define DCT_PRE_SCALING

/*Quantization*/
#define BLOCK_COEFF_SIZE	64
#define	MAX_SAT_VAL			2047
#define	MIN_SAT_VAL			-2048
#define LUMBLOCK			1
#define	CHRBLOCK			0
#define	FIXED_PT_BITS		16
#define MAX_FIXED_VALUE		32768
#define FIXED_PT_QUANT

/*Error Handling*/
enum{WARNING, FATAL};

/*ThumbScaling*/
#define RESOLUTION	8         /* Number of pixels between two integer pels*/
#define RESOLUTION_INDEX 3    /* log2(RESOLUTION)*/
#define GRID_INDEX	0x7		  /* binary equivalent of RESOLUTION_INDEX*/

#define RESOLUTION_LEVEL (18 - RESOLUTION_INDEX)
#define TWICE_RESOLUTION_INDEX (RESOLUTION_INDEX << 1)

#define	RESOLUTION_LEVEL_PLUS_INDEX		18

#endif

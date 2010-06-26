/*
*******************************************************************************
                       Copyright eMuzed Inc., 2001-2004.
All rights Reserved, Licensed Software Confidential and Proprietary Information 
    of eMuzed Incorporation Made available under Non-Disclosure Agreement OR
                            License as applicable.
*******************************************************************************
*/

/*
*******************************************************************************
Product     : JPEG Image Codec
Module      : Decoder Interface.
File        : JpegMacros.h
Description : It contains different macros used in jpeg decoder object

Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date            Id          Author            Comment 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


*******************************************************************************
*/

#ifndef INCLUDE_MACROS
#define INCLUDE_MACROS

#define EOB					0
#define BLOCK_SIZE			64
#define HALF_BLOCK_SIZE		32
#define DATA_WIDTH			8
#define BLOCK_WIDTH			8
#define LOG_BLOCK_WIDTH		3

#define	MB_WIDTH			16
#define	LOG_MB_WIDTH		4

#define START_OF_IMAGE		0xFFD8
#define DISPLAY_SCAN


#define BASELINE                        1
#define EXTENDED                        2
#define PROGRESSIVE                     3
#define LOSSLESS                        4   
#define RGB								5 
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
#define APP13							0xFFED
#define APP14                           0xFFEE
#define COMMENT				            0xFFFE

#define END_OF_IMAGE		            0xFFD9	 

#define MAX_FRAME_WIDTH  4096/*2048*/       /* Maximum decoded frame width */
#define MAX_FRAME_HEIGHT 4096/*1536*/       /* Maximum decoded frame height */
#define MAX_PIXELS_DECODE 16777216 /*3200000*/   /* Decode only DC coeff. of progressive image
                                       if number of pixels is beyond this value */

#define DETECT_BIT_ERROR          /* Enable bit error detection */
//#define ARM_ASM
#define VLC_STRUCT_INLINE
//#define ROM_BUILD
//#define FLASH_BUILD
//#define WINDOWS_BUILD

#endif /*INCLUDE_MACROS*/

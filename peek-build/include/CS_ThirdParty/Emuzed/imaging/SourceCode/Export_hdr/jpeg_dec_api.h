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
Product     : JPEG Decoder
Module      : Decoder Interface.
File        : jpeg_dec_api.h
Description : This file gives the APIs for JPEG Image decoder. 

Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date            Id          Author            Comment 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Aug 30, 2004                 Jayaprakash Pai   Initial code
*******************************************************************************
*/

#ifndef INCLUDE_JPEG_DEC_API
#define	INCLUDE_JPEG_DEC_API

#include "common.h"


/* Rectangular area for cropping */ 
typedef struct Rect_JDEmz
{
	int32 xOffset;		/* The starting posiiton of the cropped frame */
	int32 yOffset;		/* The starting position of the cropped frame */
	int32 wndWidth;		/* Crop window width */
	int32 wndHeight;	/* Crop window height */
} tRect_JDEmz;


/* Information about the image */
typedef struct ImageInfo_JDEmz
{
	uint16  width;			/* Extended width of the frame */
	uint16  height;			/* Extended height of the frame */
	uint16  actualWidth;	/* Actual width */
	uint16  actualHeight;	/* Actual height */
	uint8	frameType;		/* Color format of the frame */
	uint8	planarFormat;
	uint8	progressiveFormat;
	uint8	align0;			/* added these variables to make structure aligned*/

} tImageInfo_JDEmz;


int32 JDEmz_gParseJpegHeader(uint8 *configStream, uint32 configLength, 
					   tImageInfo_JDEmz *imageInfo);

int32 JDEmz_gCreateJpegDecoder(int32 **handle, uint8 *stream, uint32 streamLength,
						 tRect_JDEmz *cropWindow, tImageInfo_JDEmz *imageInfo, 
						 uint8 dctScaleFactor);

int32 JDEmz_gDecodeJpegImage(int32 *handle, uint32 numRows,tBaseVideoFrame_JDEmz outBuffer);

int32 JDEmz_gDeleteJpegDecoder(int32 *handle);

void* JDEmz_MemoryAlloc(int32 size);

void JDEmz_FreeMemory(void* ptr);

void* JDEmz_Memorycalloc(int32 size, int32 lbytes);



#endif  /* INCLUDE_JPEG_DEC_API */

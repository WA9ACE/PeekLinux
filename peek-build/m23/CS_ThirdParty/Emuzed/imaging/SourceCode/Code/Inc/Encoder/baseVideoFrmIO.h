/*
*******************************************************************************
                       Copyright eMuzed Inc., 2001-2002.
All rights Reserved, Licensed Software Confidential and Proprietary Information 
    of eMuzed Incorporation Made available under Non-Disclosure Agreement OR
                            License as applicable.
*******************************************************************************
*/

/*
*******************************************************************************
Product     : Streaming Solutions 
Module      : Video I/O
File        : baseVideoFrmIO.h
Description : This is the base header for video frame Input/Output Object.

Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date            Id          Author          Comment 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
March 4, 2001               Deepak Jaiswal   Initial code
*******************************************************************************
*/

#ifndef INCLUDE_BASE_VIDEO_FRM_IO 
#define INCLUDE_BASE_VIDEO_FRM_IO

#include "common.h"

/* Base Video Frame, all video frame are derived from it */
typedef struct BaseVideoFrame
{
	
    uint8  *lum;       /* Luminance pointer */
	uint8  *cb;        /* Cb pointer */
	uint8  *cr;        /* Cr pointer */
	uint16  width;     /* Width of the frame */
	uint16  height;    /* Height of the frame */
	uint32  timeStamp; /* Playback/Capture Timestamp - Format to be decided */

} tBaseVideoFrame;

/* Typedef for pixel - 8 bit pixel */
typedef  uint8   tPixel;

#endif  /* INCLUDE_BASE_VIDEO_FRM_IO */

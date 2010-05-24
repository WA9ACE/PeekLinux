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
Module      : MPEG-4 Video Encoder
File        : exp_JPEG_Enc.h
Description : base video encoder structure for MPEG-4 SP Video encoder module.

Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date            Id          Author              Comment
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Mar 12, 2001                K. Ramkishor        Initial code
Aug 13, 2003	EXP_0001	Ajeet K Mall 	    Support for User defined Quant
											    Tables is provided.
Oct 05, 2004,				G.Nageswara Rao		Support for Streaming Mode is
												added.
*******************************************************************************
*/
#ifndef INCLUDE_EXP_JPEG_ENC
#define INCLUDE_EXP_JPEG_ENC

#include "baseImageEnc.h"
#include "common.h"


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#define GET_MAX_OUTBUF_SIZE 10
#define GET_CONFIG_DATA_SIZE 11

enum {NONE,BASELINE,PROGRESSIVE,LOSSLESS};

#define E_INVALID_FORMAT		(-18)
#define E_INVALID_MODE			(-19)
#define E_INVALID_SIZE			(-20)
#define E_DATA_NOT_SUFFICIENT	(-21)


typedef struct{
	int32	maxXDimension;
	int32	maxYDimension;
	int32	qualityFactor;
	uint32  InputColorFormat;
	/* 1 for Enabling MCU row by MCU row encoding, 0 for whole Image Encoding */
	uint32	StreamingMode;

	tBaseYUVFrame* YuvFrame;
}tJpegImageEncParam;

/* Emuzed JPEG enc APIs */

extern int32 gCreateJpegImageEncoder(tBaseImageEncoder**,tJpegImageEncParam*);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif

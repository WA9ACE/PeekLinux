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
Product     : JPEG Baseline Encoder
Module      : encoder
File        : encoder.h
Description : This is the include file for the C file encoder.c

Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date            Id          Author            Comment 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Oct 13, 2004				G.Nageswara Rao		 Support for output buffer
												 streaming is added.
*******************************************************************************
*/

#ifndef INCLUDE_ENCODER
#define	INCLUDE_ENCODER

extern int32 gEncodeImage(tJpegImageEncoder* enc);
extern int32 gEncodeMCURow(tJpegImageEncoder*, uint32 );

extern int32 gEncodeMCURow_Patched(tJpegImageEncoder*, uint32);

/* Functions used in Baseline Mode of Encoding */
extern void sCodeMCU(tMcu *, tDCPrediction *, tJpegImageEncoder*,tPutBitsInfo *);

extern int32 gGetParamJpegImageEncoder(tBaseImageEncoder*, uint32, uint32*);

extern int32 gJpegEncodeImage(tBaseImageEncoder*, tBaseYUVFrame*, uint8*, uint32*, uint32);

extern int32 gJpegEncodeImage_Patched(tBaseImageEncoder*, tBaseYUVFrame*, uint8*, uint32*, uint32);

extern int32 gPutConfigurationData_JpegEnc(tBaseImageEncoder *, tPixel *, uint32 *);
extern int32 gDestroyJpegImageEncoder(tBaseImageEncoder* base);
#endif //INCLUDE_ENCODER

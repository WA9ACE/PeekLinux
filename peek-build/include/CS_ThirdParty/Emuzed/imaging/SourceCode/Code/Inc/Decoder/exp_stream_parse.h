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
Product     : JPEG-DECODER
Module      : Stream Parse
File        : stream_parse.c
Description : This file contains  definition of stream parsing functions.
              
Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date            Id          Author              Comment 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
July 6, 2001                Rajiv Kumar Bajpai  Initial Code 
*******************************************************************************
*/


#ifndef INCLUDE_EXP_STREAM_PARSE
#define	INCLUDE_EXP_STREAM_PARSE

extern int32 JDEmz_gInterpretMarker(tJpegDecoder_JDEmz *JpegDecoder, int32 *marker);
int32 JDEmz_gParseFrameHeader(tJpegDecoder_JDEmz *JpegDecoder);
void JDEmz_gParseComponent(tJpegDecoder_JDEmz *JpegDecoder);
int32 JDEmz_gParseScanHeader(tJpegDecoder_JDEmz *JpegDecoder );
void JDEmz_gParseScanComponent(tJpegDecoder_JDEmz *JpegDecoder);
void JDEmz_gReSyncMarker(tBaseBitIn_JDEmz *bitIn);
void JDEmz_gSyncEndOfImageMarker(tBaseBitIn_JDEmz *bitIn);
int32 JDEmz_sParseDri(tBaseBitIn_JDEmz *bitIn, tDriHeader_JDEmz *driHeader);
int32 JDEmz_sParseApplMarker(tJpegDecoder_JDEmz *jpegDecoder, int32 *marker);
int32 JDEmz_sParseComment(tBaseBitIn_JDEmz *bitIn);
int32 JDEmz_sParseDNL(tBaseBitIn_JDEmz *bitIn);


#endif /*INCLUDE_EXP_STREAM_PARSE */

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
Module      : Frame Decode
File        : frame_decode.h
Description : It contains definition of functions for decoding scan and frame.

Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date            Id          Author            Comment 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


*******************************************************************************
*/


#ifndef INCLUDE_FRAME_DECODE
#define	INCLUDE_FRAME_DECODE

#include "texture_decode.h"
#include "exp_texture_decode.h"
#include "exp_stream_parse.h"
#include "JpegMacros.h"
#include "transform_scaled_chen.h"

int32 JDEmz_sDecodeScan_patched(tJpegDecoder_JDEmz *jpegDecoder, int32 numMcuRow);
int32 JDEmz_sDecodeDataUnit (tJpegDecoder_JDEmz *jpegDecoder,  tComponent_JDEmz *scanComponent, 
                       int32 dataUnitX, int32 dataUnitY);

#endif /*INCLUDE_FRAME_DECODE*/

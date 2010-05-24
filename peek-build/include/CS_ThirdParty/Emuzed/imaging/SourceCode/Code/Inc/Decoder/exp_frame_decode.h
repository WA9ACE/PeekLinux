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
File        : exp_frame_decode.h
Description : It contains definition of functions for decoding scan and frame.

Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date            Id          Author            Comment 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


*******************************************************************************
*/


#ifndef INCLUDE_EXP_FRAME_DECODE 
#define	INCLUDE_EXP_FRAME_DECODE

int32 JDEmz_gDecodeFrame_patched(tBaseImageDecoder_JDEmz *base, int32 numRows);
/* for the progressive case */
int32 JDEmz_gPDecodeFrame (tBaseImageDecoder_JDEmz *base, int32 numScan);

 

#endif /*INCLUDE_EXP_FRAME_DECODE*/

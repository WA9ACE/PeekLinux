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
Module      : JPEG Encoder
File        : baseJPEGEnc.h
Description : base video encoder structure for JPEG encoder module.

Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date            Id          Author              Comment 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Mar 14, 2001                K. Ramkishor        Initial code

*******************************************************************************
*/
#ifndef INCLUDE_BASEJPEGENC
#define INCLUDE_BASEJPEGENC

#include "common.h"
//#include "exp_JPEG_Enc.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef struct BaseImageEncoder tBaseImageEncoder;

struct BaseImageEncoder{

	int32  (*vEncodeFrame) (tBaseImageEncoder  *lBaseJpegEnc, tBaseYUVFrame *FrameBuffer, 
		uint8 *outBuf, uint32 *numBytes, uint32 numInBytes);  
	/*Function pointer to encode a frame */
	int32 (*vSetParam) (tBaseImageEncoder *base, uint32 flag, uint32 val);  
	/* Function ptr to set parameters */
	int32 (*vGetParam) (tBaseImageEncoder *base, uint32 flag, uint32 *val); 
	/* Function ptr to get a specified parameter value */
    int32   (*vDelete) (tBaseImageEncoder *base);

	int32   (*vPutCongfigData) (tBaseImageEncoder *base, uint8 *outBuf, uint32 *numBytes);
};

#ifdef __cplusplus
}
#endif //__cplusplus

#endif

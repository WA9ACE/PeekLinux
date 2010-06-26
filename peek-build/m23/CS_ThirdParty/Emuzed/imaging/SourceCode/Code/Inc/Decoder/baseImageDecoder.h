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
File        : 
Description : 

Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date            Id          Author            Comment 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


*******************************************************************************
*/

#ifndef INCLUDE_BASE_IMAGE_DECODER
#define	INCLUDE_BASE_IMAGE_DECODER

#include "common.h"
#include "baseBitIn.h"

typedef struct BaseImageDecoder_JDEmz tBaseImageDecoder_JDEmz;

struct BaseImageDecoder_JDEmz
{
	int32   (*vDecodeFrame) (tBaseImageDecoder_JDEmz *base, int32 numRows);  
	            /* Function pointer to decode a frame */
	int32   (*vSetParam) (tBaseImageDecoder_JDEmz *base, uint32 flag, uint32 val);  
	            /* Function pointer to set parameters */
	int32   (*vGetParam) (tBaseImageDecoder_JDEmz *base, uint32 flag, uint32 *val); 
	            /* Function pointer to get a specified parameter value */
    int32   (*vDelete) (tBaseImageDecoder_JDEmz *base);
	                /* Function pointer to destroy a video decoder object */

	tBaseVideoFrame_JDEmz  *frameOut;       /* Frame output object */
	tBaseBitIn_JDEmz      *bitIn;          /* Bit input object  */

};


#endif  /* INCLUDE_BASE_IMAGE_DECODER */


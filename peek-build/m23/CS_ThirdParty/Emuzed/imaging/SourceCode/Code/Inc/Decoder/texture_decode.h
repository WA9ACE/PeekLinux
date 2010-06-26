/*
******************************************************************************
                    Copyright eMuzed Inc., 2001-2004.
All rights Reserved,Licensed Software Confidential and Proprietary Information 
of eMuzed Incorporation Made available under Non-Disclosure Agreement OR 
                        License as applicable.
******************************************************************************
*/

/*
******************************************************************************
Product        : JPEG-Decoder
Module         : Texture Decoding
File           : texture_decode.h
Description	   : This file contains the texture decoding functions definitions.


Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date               Id        Author              Comment    
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
July 6, 2001				 Rajiv Kumar Bajpai	  Initial Code				
******************************************************************************
*/

#ifndef INCLUDE_TEXTURE_DECODE
#define	INCLUDE_TEXTURE_DECODE

#include "vld.h"
#include "exp_vld.h"
#include "stream_parse.h"
#include "jpegDecoder.h"
#include "transform_scaled_chen.h"

void JDEmz_sStoreUnit(uint8 *currFrame, int16 *currMBData, int32 x,
                int32 y, int32 frameWidth, int32 dctScale); 
void JDEmz_sStoreUnit1(uint8 *currFrame, int16 *currMBData, int32 frameWidth);
void JDEmz_sStoreUnit2(uint8 *currFrame, int16 *currMBData, int32 frameWidth);
void JDEmz_sStoreUnit4(uint8 *currFrame, int16 *currMBData, int32 frameWidth);
void JDEmz_sStoreUnit8(uint8 *currFrame, int16 *currMBData);


#endif /*INCLUDE_TEXTURE_DECODE*/

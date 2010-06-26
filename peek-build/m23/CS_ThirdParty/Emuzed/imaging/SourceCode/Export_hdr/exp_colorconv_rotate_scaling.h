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
Product     : JPEG Image Decoder
Module      : 
File        : exp_colorconv_rotate.h
Description : Color conversion(YUV to RGB & RGB to YUV) is done here.

Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date            Id          Author				Comment 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Apr 09, 2001                Tushar Adhikary
Jul 10, 2003				Jayaprakash Pai		Added cropping suport with color
												conversion      
Jul 16, 2003				Jayaprakash Pai		Added cropping with scaling 
Sep 15, 2003				Gururaj.G.P.		Added scaling and rotation in 
												RGB16 format
*******************************************************************************
*/


#ifndef INCLUDE_COLORCONV_ROTATE_SCALING
#define INCLUDE_COLORCONV_ROTATE_SCALING

#include "common.h"


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#define COLOR_CONV_PRECISION    14
#define COLOR_CONV_ROUND        8192

int32 IPEmz_gOverLay (uint8 *srcImage, uint8 *overlayImage, uint8 *dstImage,
						 int32 srcWidth, int32 srcHeight,
						 int32 overlayWidth, int32 overlayHeight,
						 int32 srcXOffset, int32 srcYOffset,
						 int32 threshold, uint8 dstFlag);

int32 IPEmz_gAlphaBlending (tBaseVideoFrame_JDEmz * srcImage, 
				tBaseVideoFrame_JDEmz * blendImage, 
				tBaseVideoFrame_JDEmz * dstImage, int32 xOffset, 
				int32 yOffset,int32 transmissivity);

int32 IPEmz_gScaleFrame (tBaseVideoFrame_JDEmz *srcFrame, 
						 tBaseVideoFrame_JDEmz *dstFrame, 
						 uint32 xOffset, uint32 yOffset, uint32 cropWidth, 
						 uint32 cropHeight, uint8 srcClrFmt);

int32 IPEmz_gTransCoding(tBaseVideoFrame_JDEmz *yuvFrame, uint8* yuyvFrame,
					  uint16 actWidth, uint16 actHeight, uint8 srcClrFmt);

int32 IPEmz_gAddSepiaMask (tBaseVideoFrame_JDEmz *src, tBaseVideoFrame_JDEmz *dst, uint8 srcClrFmt, uint8 flag);

int32 IPEmz_gGrayscaleMask(tBaseVideoFrame_JDEmz *src, tBaseVideoFrame_JDEmz *dst, 
					 uint8 srcClrFmt, uint8 flag);

int32 IPEmz_gRotateFrame (tBaseVideoFrame_JDEmz *srcFrame, tBaseVideoFrame_JDEmz *dstFrame,
				  uint8 rotateFlag, uint8 *srcClrFmt);

int32 IPEmz_gColorConversion(tBaseVideoFrame_JDEmz *yuvFrame, 
					  uint8* rgbFrame, uint16 actWidth, 
					  uint16 actHeight, uint8 srcClrFmt, 
					  int32 numBytes, uint8 rgbFormat);

int32 IPEmz_gColorConversionRGB(uint8* rgbFrame, tBaseVideoFrame_JDEmz *yuvFrame,
					  uint16 actWidth, uint16 actHeight, int32 rgbFormat, int32 dstClrFmt);					  

int32 IPEmz_gRotateFrame_singleBuff(tBaseVideoFrame_JDEmz *frame, 
						uint8 rotateFlag, uint8 *srcClrFmt);


void* IPEmz_MemoryAlloc(int32 size);
void IPEmz_FreeMemory(void* ptr);
void* IPEmz_Memcpy(void *dst, void *src, int32 size);
void* IPEmz_Memset(void *dst, int32 val, int32 size);
#ifdef __cplusplus
}
#endif //__cplusplus

#endif  /* INCLUDE_COLORCONV_ROTATE_SCALING */

/*
*******************************************************************************
                       Copyright eMuzed Inc., 2001-2005.
All rights Reserved, Licensed Software Confidential and Proprietary Information 
    of eMuzed Incorporation Made available under Non-Disclosure Agreement OR
                            License as applicable.
*******************************************************************************
*/

/*
*******************************************************************************
Product     : 
Module      : Post Processing
File		: Scaling.c
Description : This file contains the functions required to Overlay an RGB16
			  image on another RGB16 image. 

Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date            Id          Author				Comment 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Apr 25, 2005           Anjaneya Prasad M		Initial Code for overlay
*******************************************************************************
*/

#include "common.h"

int32 sOverLayRGB16Image(uint8 *srcImage, uint8 *overlayImage, uint8 *dstImage,
						 int32 srcWidth, int32 srcHeight,
						 int32 overlayWidth, int32 overlayHeight,
						 int32 srcXOffset, int32 srcYOffset,
						 int32 threshold, uint8 dstFlag);

int32 IPEmz_gAlphaBlending (tBaseVideoFrame_JDEmz * srcImage, 
				tBaseVideoFrame_JDEmz * blendImage, 
				tBaseVideoFrame_JDEmz * dstImage, int32 xOffset, 
				int32 yOffset,int32 transmissivity);







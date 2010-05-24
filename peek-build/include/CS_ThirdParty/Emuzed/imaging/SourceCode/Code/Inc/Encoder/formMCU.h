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
Module      : JPEG Image Encoder
File        : formMCU.h
Description : Header for formMCU.c file having the MCU forming control flow..

Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date            Id          Author              Comment
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
May 10, 2002                Raghu.T.S           Initial code

*******************************************************************************
*/
#ifndef INCLUDE_FORMMCU

#define INCLUDE_FORMMCU

/* Functions used in Baseline Mode of Encoding */
#ifndef ARM9TDMI_INLINE
	extern void gFormMCUYUYV422to420(tMcu *, tJpegImageEncoder *);
	extern void gFormMCU420(tMcu *, tJpegImageEncoder *);
	extern void gFormMCU422(tMcu *, tJpegImageEncoder *);
# else
	extern void gFormMCUYUYV422to420(tMcu *, int32 totalLumFrW, int32 totalCrFrW); 
	extern void gFormMCU420(tMcu *, int32 totalLumFrW, int32 totalCrFrW); 
	extern void gFormMCU422(tMcu *, int32 totalLumFrW, int32 totalCrFrW); 
#endif

	extern void gFormBoundryMCU(tJpegImageEncoder*, int32 extWidth,int32 extHeight);

#endif

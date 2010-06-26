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
Product        : Common Module
Module         : BITIO
File           : baseBitIn.h
Description    : header file base bit input object.

Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date               Id        Author              Comment    
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
March 2, 2001                Murali              Initial Code
March 5, 2001                Deepak Jaiswal      Moving to common area and
                                                 file name change.
******************************************************************************
*/

#ifndef INCLUDE_BASE_BIT_IN_JPEG
#define INCLUDE_BASE_BIT_IN_JPEG


int32 JDEmz_gGetBitsJpegTcoeff (tBaseBitIn_JDEmz *base, int32 numBits, int32 *flag);
void JDEmz_gFlushBitsJpegTcoeff(tBaseBitIn_JDEmz *base, int32 numBits, int32 *flag);
int32 JDEmz_gShowBitsJpegTcoeff(tBaseBitIn_JDEmz *base, int32 numBits, int32 *flag);
int32 JDEmz_gByteAlign(tBaseBitIn_JDEmz *bitIn);
void JDEmz_gInitBitsJpegTcoeff (tBaseBitIn_JDEmz *base, int32 *flag);

#endif /* INCLUDE_BASE_BIT_IN_JPEG */

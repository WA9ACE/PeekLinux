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
Module      : JPEG Baseline Encoder
File        : bsf.h
Description : Header for bit stream formatting module.

Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date            Id          Author              Comment 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Nov 27, 2001                Sudhindra        Initial code

*******************************************************************************
*/
#ifndef INCLUDE_BSF
#define INCLUDE_BSF

extern void gPutJfifHeader(tBaseBitOut *);
extern void gPutQuantTables(tJpegImageEncoder*, tPutBitsInfo *);
extern void gPutSofHeader(tJpegImageEncoder*, tPutBitsInfo *);
extern void gPutSosHeader(tJpegImageEncoder*, tPutBitsInfo *);
extern void gPutHuffTables(tJpegImageEncoder*, tPutBitsInfo *);


#endif

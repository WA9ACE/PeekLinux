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
File        : entropy_coding.h
Description : Header for rate control module.

Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date            Id          Author              Comment 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Nov 26, 2001                Sudhindra        Initial code

*******************************************************************************
*/
#ifndef INCLUDE_ENTROPY_CODING
#define INCLUDE_ENTROPY_CODING

extern void gEntropyCodeACDC(int16*, tBaseBitOut*, tEntropyTables*, tPutBitsInfo*, int32);

extern void sPutBytes_JpegEncEmz (tBaseBitOut*, int32, uint8*);

extern const uint16 acLumCodeWord[162];

extern const uint16 acChromCodeWord[162];

extern const uint8 acLumCodeLen[162];
	
extern const uint8 acChromCodeLen[162];

extern const uint16 dcLumCodeWord[12];
	
extern const uint8 dcLumCodeLen[12];

extern const uint16 dcChromCodeWord[12];

extern const uint8 dcChromCodeLen[12];

extern const uint8 zigzagScan_EzdJpgEnc[64];

extern const uint8 sInvZigzagScan_EzdJpgEnc[64];

#endif 

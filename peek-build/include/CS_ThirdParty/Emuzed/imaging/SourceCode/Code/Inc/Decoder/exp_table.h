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
Module         : Table Preparation
File           : table.c
Description	   : This file contains definition of different function used in 
                 table preparation.

Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date               Id        Author              Comment    
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
July 6, 2001                 Rajiv Kumar Bajpai  Initial Code
******************************************************************************
*/
#ifndef INCLUDE_EXP_TABLE
#define	INCLUDE_EXP_TABLE

int32 JDEmz_gParseQuantTable(tJpegDecoder_JDEmz *jpegDecoder);
int32 JDEmz_gParseHuffmanTable(tJpegDecoder_JDEmz *jpegDecoder);

#endif		/*INCLUDE_EXP_TABLE*/

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
Module         : Variable Length Decoding
File           : vld.c 
Description    : This file contains the routine of variable length decoding

Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date               Id        Author              Comment    
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
July 6, 2001				 Rajiv Kumar Bajpai  Initial Code
			
******************************************************************************
*/



#ifndef INCLUDE_EXP_VLD
#define INCLUDE_EXP_VLD

int32 JDEmz_sDecode(tBaseBitIn_JDEmz *bitIn, tHuffmanTable_JDEmz *huffmanTable, int32 *flag);

int32 JDEmz_gDecodeTCoeff_patched (tJpegDecoder_JDEmz *jpegDecoder, tComponent_JDEmz *scanComponent, 
                     int16 *invBlockdata,
                     int32 *lastCoeffIndex);

int32 JDEmz_gPDecodeAcCoeff (tBaseBitIn_JDEmz *bitIn, tScanHeader_JDEmz *scanHeader, 
                      tHuffmanTable_JDEmz *acTable, int16 *blockdata, 
                      uint8 *lastCoeffIndex,
                      int32 *eobRun, int32 *flag);

int32 JDEmz_gPDecodeDcCoeff (tBaseBitIn_JDEmz *bitIn, tScanHeader_JDEmz *scanHeader, 
                      tHuffmanTable_JDEmz *dcHuffmanTable, int16 *blockData, 
                      int16 *dcPrediction, int32 *flag);

int32 JDEmz_gPDecodeAcCoeffRefine (tBaseBitIn_JDEmz *bitIn, tScanHeader_JDEmz *scanHeader, 
                             tHuffmanTable_JDEmz *acTable, int16 *blockData, 
                             uint8 *lastCoeffIndex,
                             int32 *eobRun, int32 *flag);
int32 JDEmz_gNextBit (tBaseBitIn_JDEmz *bitIn, int32 *count, 
                uint8 *byte, int32 *markerFlag);


#endif /* INCLUDE_EXP_VLD */

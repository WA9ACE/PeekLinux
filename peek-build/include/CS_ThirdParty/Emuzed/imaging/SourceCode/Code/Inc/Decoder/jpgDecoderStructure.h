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
Product     : JPEG - DECODER
Module      : 
File        : jpgDecoderStructures.h
Description : This file defines the common structures used in JPEG 
              decoder.

Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date            Id          Author              Comment 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


*******************************************************************************
*/
#ifndef INCLUDE_DECODER_STRUCTURE
#define	INCLUDE_DECODER_STRUCTURE

#include "table.h"

typedef struct Component_JDEmz
{
	uint8 identifier;		/* Identifier to Component*/
	uint8 hSamplingFactor;	/* Horizontal Sampling Factor */
	uint8 vSamplingFactor;	/* Vertical Sampling Factor */
	uint8 qTableId;			/* Identifier for the Quantisation Table,the Component Uses*/
	uint8 dcTableId;		/* Identifier for the DC Huffman Table, the Component Uses */
	uint8 acTableId;		/* Identifier for the AC Huffman Table, the Component Uses */
	int16 dcPrediction;
	int32 xDimension;
	int32 frameXDimension;
    int32 oriXDimension;
	int32 oriYDimension;
    uint8 *currFrame; /* this is for storing purpose*/
    /* for the progressive case */
    int16 (*data)[64];
    uint8 *lastCoeff;
    uint8 *quant;
    tHuffmanTable_JDEmz *dc;
    tHuffmanTable_JDEmz *ac;
}tComponent_JDEmz;

typedef struct Identifier_JDEmz
{
    uint8 lum;
    uint8 cb;
    uint8 cr;
	uint8 align0;		/*Added this variable to make structure aligned*/
}tIdentifier_JDEmz;

typedef struct FrameHeader_JDEmz
{
	uint16 length;			/* Frame Header Length*/
	uint8 precision;		/* Sample Precision*/
	uint8 imageComponents;	/* Image Component Number*/
	int32 height;			/* Height of the Image Frame */
	int32 width;			/* Width of the Image Frame */
}tFrameHeader_JDEmz;

typedef struct ScanHeader_JDEmz
{
	int32 length;			/* Scan Header Length*/
	uint8 scanComponents;	/* Scan Components Numbers*/
    uint8 spectralStart;
    uint8 spectralEnd; 
    uint8 approxBitHigh;
    uint8 approxBitLow;
	uint8 align0;
	uint16 align1;			/*Added these variables to make structure aligned*/
}tScanHeader_JDEmz;

typedef struct Mcu_JDEmz
{
	int32 x;
	int32 y;
	uint8 width;
	uint8 heigth;
    uint8 maxHsampFactor;
    uint8 maxVsampFactor;
    int32 markerFlag;
}tMcu_JDEmz;

typedef struct DriHeader_JDEmz
{
	uint16 riMcuNo;
    uint16 driFlag;	
}tDriHeader_JDEmz;

typedef struct RestrictedParam_JDEmz
{
    int32 width;
    int32 height;
    uint8 flag;
	uint8 align0;
	uint16 align1;	/*Added these variables to make structure aligned*/
}tRestrictedParam_JDEmz;


#endif /*INCLUDE_DECODER_STRUCTURE*/

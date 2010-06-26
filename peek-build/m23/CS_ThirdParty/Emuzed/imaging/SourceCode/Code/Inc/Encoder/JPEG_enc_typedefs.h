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
File        : JPEG_enc_typedefs.h
Description : Type definitions for JPEG Image Encoder module.

Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date            Id          Author              Comment
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Nov 23, 2001                Sudhindra           Initial code

*******************************************************************************
*/
#ifndef INCLUDE_JPEG_TYPEDEFS
#define INCLUDE_JPEG_TYPEDEFS

#include "JPEG_enc_includes.h"

/* Structure to define image component properties */
typedef struct
{
	int32		componentID;
	int32		relHSamplingFactor;
	int32		relVSamplingFactor;
	int32		dcTableID;
	int32		acTableID;
	int32		qTableID;
}tComponent;


/* Structure to define Minimum Coded Unit (MCU) */
typedef struct
{
	int32		x;
	int32		y;
	int32		noOfDataUnits;
	int16		*Data;
	uint8		*LumCurrPos;
	uint8		*CbCurrPos;
	uint8		*CrCurrPos;
	//uint32		TotalFrmSize;

}tMcu;


typedef struct
{
	int32		x;
	int32		y;
	int32		noOfDataUnits;
	int16		*LumBuffer;
	int16		*CbBuffer;
	int16		*CrBuffer;
}tMcuProg;

/* Structure to define parameters for Progressive AC Scan */
typedef struct
{
	int32		startOfScan;
	int32		endOfScan;
	int32		EOBRun;
}tProgACScan;

/* Structure to define Entropy tables */

typedef struct
{
	uint16		*codeWord;
	uint8		*codeLen;
	int32		*run;
	int32		*size;
}tHuffTables;


typedef struct
{
	tHuffTables	*AC;
	tHuffTables	*DC;
}tEntropyTables;


typedef struct
{
	int32		length;
	int32		tc;
	int32		th;
	int32		bits[16];
}tHuffTableSpec;


/* Structure to define Quantization Tables */
typedef struct
{
	int32		length;
	int32		precision;
	int32		identifier;
	uint16		*qData;
}tQuantTables;


/* DC Prediction Structure */
typedef struct
{
	int32	lumDCPred;
	int32	cbDCPred;
	int32	crDCPred;
}tDCPrediction;


/* Structure to define Frame Header */
typedef struct
{
	int32		length;
	int32		precision;
	int32		maxXDimn;
	int32		maxYDimn;
	int32		noOfComponetsInFrame;
}tFrameHeader;



/* Structure to define Scan Header */
typedef struct
{
	int32		length;
	int32		noOfComponentsInScan;
	int32		spectralStart;
	int32		spectralEnd;
	int32		approxBitHigh;
	int32		approxBitLow;
}tScanHeader;


/* JPEG File Interchange Format (JFIF) */
typedef struct
{
	int32 		length;
	int32		identifier[5];
	int32		densityUnits;
	int32		xDensity;
	int32		yDensity;
	int32		xThumbnail;
	int32		yThumbnail;
}tJfif;



typedef struct
{
	uint32	cache;
	int32	filledBits;
	uint8   modBitBuffer[256];
	int32	bitCount;
	int32	buffCnt;
}tPutBitsInfo;


typedef struct{

	int32  (*vEncodeFrame) (tBaseImageEncoder  *lBaseJpegEnc,
		tBaseYUVFrame *FrameBuffer, uint8 *outBuf, uint32 *numBytes, uint32 numInBytes);
	/*Function pointer to encode a frame */
	int32 (*vSetParam) (tBaseImageEncoder *base, uint32 flag, uint32 val);
	/* Function ptr to set parameters */
	int32 (*vGetParam) (tBaseImageEncoder *base, uint32 flag, uint32 *val);
	/* Function ptr to get a specified parameter value */
    int32   (*vDelete) (tBaseImageEncoder *base);
	/* Functioon pointer to put header data into given buffer*/
	int32   (*vPutCongfigData) (tBaseImageEncoder *base, uint8 *outBuf, uint32 *numBytes);

	tBaseBitOut   		*bitOut;          /*Bit output object  */
	tBaseVideoFrame		*CurrFrame;
	tBaseVideoFrame		CurrFrameData;
	tComponent			*component;
	tMcu				mcu;
	tMcuProg			mcuProg;
	tEntropyTables		entropyTable[2];
	tQuantTables		quantTable[4];
	tPutBitsInfo		putBitsInfo;
	int32				maxHSamplingFactor;
	int32				maxVSamplingFactor;
	int32				maxXDimn;
	int32				maxYDimn;
	int32				extXDimension;
	int32				extYDimension;
	int32				noOfComponents;
	int32				precision;
	int32				interLeaveFormat;
	int32				qualityFactor;
	int32				encodingMode;  /* Sequential or Progressive */
	int32				lumExtW;
	int32				lumExtH;
	int32               crFrW;
	int32               crFrH;
	int32				chromExtW;
	int32				chromExtH;
	int32				lumXStep;
	int32				lumYStep;
	int32				chromXStep;
	int32				chromYStep;
	int32				totalLumFrW;
	int32				totalCrFrW;
	int32				noOfLumBlk;
	int32				noOfCrBlk;
	int32 				threshold;
	int32				LumQuantTableFlag;
	int32				ChrQuantTableFlag;
	uint16				*LumQuantTables;
	uint16				*ChrQuantTables;
	int32				thumbnailFlag;
	int32				thumbnailFormat;
	int32				thumbnailWidth;
	int32				thumbnailHeight;
	int32				thumbnailExtWidth;
	int32				thumbnailExtHeight;
	tBaseVideoFrame		*ThumbFrame;
	uint8				*RawImageBuffer;	/* Added for IJG interface support*/
	int32				ScanlineCount;
	int32				BytesinScanline;
	int32				OriginterLeaveFormat;
	uint32				RowsTobeCoded;
	uint32			    VPad;
	uint32				RowsCoded;
	uint32				RowsCodedthisItr;
	uint32				RowsThisItr;
	uint32				StreamingMode;
	uint32				BytesCoded;
	uint32				TotInBytes;
	uint32				RemBytes;
	int32				PrevReturn;
	int32				MaxBuffSize;
	int32				ProgScanCount;
	int32				ProgComponetCount;
	int32				ImageStartFlag;	/* E_ON to indicate start of of Image E_OFF for end*/
	int32				ThumbBuffSize;
	tProgACScan			ProgACScan;
	tDCPrediction		DCPred;
	tPixel*				TempMcuBuffer;

	uint32		TotalFrmSize;


}tJpegImageEncoder;



#endif

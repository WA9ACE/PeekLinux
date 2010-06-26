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
Product     : JPEG Image Codec
Module      : 
File        : jpegDecoder.h 
Description : it contains the structure of derived jpeg object.
Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date            Id          Author            Comment 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


*******************************************************************************
*/


#ifndef INCLUDE_JPEG_DECODER
#define	INCLUDE_JPEG_DECODER

#include "baseImageDecoder.h"
#include "table.h"
#include "jpgDecoderStructure.h"
#include "exp_frame_decode.h"

typedef struct jpegDecoder_JDEmz
{
	int32   (*vDecodeFrame) (tBaseImageDecoder_JDEmz *base, int32 numRows);  
	            /* Function pointer to decode a frame */
	int32   (*vSetParam) (tBaseImageDecoder_JDEmz *base, uint32 flag, uint32 val);  
	            /* Function pointer to set parameters */
	int32   (*vGetParam) (tBaseImageDecoder_JDEmz *base, uint32 flag, uint32 *val); 
	            /* Function pointer to get a specified parameter value */
    int32   (*vDelete) (tBaseImageDecoder_JDEmz *base);
	                /* Function pointer to destroy a video decoder object */
	
	uint8					*frameOut;       /* Frame output object */
	tBaseBitIn_JDEmz		*bitIn;          /* Bit input object  */
	int32					flag;
	tFrameHeader_JDEmz		frameHeader;
	tScanHeader_JDEmz		scanHeader;
	tComponent_JDEmz		*imageComponent;
	tComponent_JDEmz		*scanComponent;
	tEntropyTables_JDEmz	entropyTable;
	tQTable_JDEmz			quantTable[4];
	tDriHeader_JDEmz		driHeader;
    tIdentifier_JDEmz		id;
	tBaseVideoFrame_JDEmz	*currFrame;
    tRestrictedParam_JDEmz	restrictedParam; /** new incluison */
    int32           lumVerUnit;    //New inclusion
    int32           lumHoriUnit; /* total no  of horizontal data unit */
    
    int16           (*lum)[64];
    int16           (*cb) [64];
    int16           (*cr) [64];
    uint8           *lumLastCoeff;
    uint8           *cbLastCoeff;
    uint8           *crLastCoeff;
    uint8           typeFlag;	/* Compression format */
    uint8           format;		/* Color space in the image */
	uint8			jfifFlag;
	uint8			colorCode;
   	uint8			dcTblFlag[4];	/* Flags to check presence of tables */
	uint8			acTblFlag[4];
	uint8			qTblFlag[4];
	uint32			dctScaleFactor; /* 1, 2, 4 and 8 values only permitted */
	int32			xOffset;	/* The starting posiiton of the cropped frame */
	int32			yOffset;	/* The starting position of the cropped frame */
	int32			wndWidth;	/* Crop window width */
	int32			wndHeight;	/* Crop window height */
	//tBaseVideoFrame_JDEmz *outBuffer;   /* Decoded Output is stored in this Buffer */
	uint32			firstInput;
	uint32			accNumRow;
	uint32			remMcuCount;
	tMcu_JDEmz		currMcu;
	int32			riNum;
	int32			riCounter;
	//int32			reSync;
	int32			prevMarker;
	int32			lastRiMcu;
	int32			origMcuCount;

	tBaseBitIn_JDEmz		*InBitstream[3];          /* Bit input object  */
	int32					lflag[3];
	uint32			lfirstInput[3];
	uint32			laccNumRow[3];
	uint32			lremMcuCount[3];
	tMcu_JDEmz		lcurrMcu[3];
	int32			lriNum[3];
	int32			lriCounter[3];
	//int32			reSync;
	int32			lprevMarker[3];
	int32			llastRiMcu[3];
	int32			lorigMcuCount[3];
	uint8			compID;
	uint8			planarFormat;

} tJpegDecoder_JDEmz;

#endif /* INCLUDE_JPEG_DECODER */


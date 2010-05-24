/*-------------------------------------------------------------------
File name : mv3JpegDecPatch.h

MV400 JPEG Decode Patch Header File

(C) Copyright MtekVision, Inc. 2003~2004 All right reserved.
  Confidential Information

All parts of the MtekVision Program Source are protected by copyright law 
and all rights are reserved. 
This documentation may not, in whole or in part, be copied, photocopied, 
reproduced, translated, or reduced to any electronic medium or machine 
readable form without prior consent, in writing, from the MtekVision. 

Last modification : 2005. 02. 03
----------------------------------------------------------------------*/
#ifndef __MV3JPEGDECPATCH_H__
#define __MV3JPEGDECPATCH_H__


//////////////////////////////////////////////////////////////////////////
//  SYSTEM Definitions
///////////////////////////////////////////////////////////////////////////
#include "mv3Attach.h"

// If you do not use MV Library,....
//#define INDEPENDENT_OF_LIB

// Define Max JPEG Max Buffer size
#define MAX_JPEG_BUFFER_SIZE	1024


#if defined(INDEPENDENT_OF_LIB)

	#if defined (FEATURE_MV3X7)
		#define YUV411_HALF_SIZE_WIDTH      320
	#elif defined (FEATURE_MV3X9)
		#define YUV411_HALF_SIZE_WIDTH      640
	#endif /* (FEATURE_MV3X9) */

	#if defined(FEATURE_MV3X9)
		#define BUFFER_HIGH_ADDRESS    (MV3_BYTE_SPREAD * 0)
		#define BUFFER_LOW_ADDRESS    (MV3_BYTE_SPREAD * 1)
		#define MV3_BUFFER_DATA      (MV3_BYTE_SPREAD * 2)
	#elif defined(FEATURE_MV3X7)
		#define BUFFER_HIGH_ADDRESS    (MV3_BYTE_SPREAD * 0x0C)
		#define BUFFER_LOW_ADDRESS    (MV3_BYTE_SPREAD * 0x0D)
		#define MV3_BUFFER_DATA      (MV3_BYTE_SPREAD * 0x0B)
	#endif

	#define MV3_WRITE(addr, data)           outpw_(MV3_BASE_ADDR  + (addr), (data))

	#define MV3_MEMORY_WRITE_1WORD(addr, data) {\
	    MV3_WRITE(BUFFER_HIGH_ADDRESS, (addr) >> 16);\
	    MV3_WRITE(BUFFER_LOW_ADDRESS, (addr));\
	    MV3_WRITE(MV3_BUFFER_DATA, (data))

	typedef struct {
	   mvUint16 extWidth;                   // JPEG Encode/Decode HSIZE (Multifly value of 8/16)
	   mvUint16 extHeight;                  // JPEG Encode/Decode VSIZE (Multifly value of 8/16)
	   mvUint16 jpegWidth;                  // JPEG image width
	   mvUint16 jpegHeight;                 // JPEG iamge height
	   
	   mvUint16 decMode;                    // JPEG Decompressing Type
	   mvUint8  halfSize;                   // for 4bytes alignment
	   mvUint8  component;					// for 4bytes alignment
	   
	   mvUint16 dri;                        // added in v1.12 for checking if there is DRI 
	} MV3_JPEG_INFO_STRUCT;
#else
	#include "mv3Gvar.h"
#endif

// JPEG ERROR CODES
#define SOF_ERROR		0x0100
#define P_ERROR			0x0200
#define COMP_ERROR		0x0400
#define SAMPLE_ERROR	0x0800
#define CNT_ERROR		0x1000
#define DRI_ERROR		0x2000

typedef struct {
	mvUint32 start;
	mvUint32 end;
} RANGE;



//==============================================================
// JpegCore
//==============================================================
#include <setjmp.h>

#define TRUE  1
#define FALSE 0

// huffman table infomation
// from IJG(Independent Jpeg Group) Library
typedef struct {        		/* A Huffman coding table */
    /* These two fields directly represent the contents of a JPEG DHT marker */
    mvUint8 bits[17];   		/* bits[k] = # of symbols with codes of */
                       			/* length k bits; bits[0] is unused */
    mvUint8 huffval[256]; 		/* The symbols, in order of incr code length */
    /* The remaining fields are computed from the above to allow more efficient
     * coding and decoding.  These fields should be considered private to the
     * Huffman compression & decompression modules.
    */
    mvUint16 ehufco[256]; 		/* code for each symbol */
    mvUint8 ehufsi[256];  		/* length of code for each symbol */
    mvUint16 mincode[17]; 		/* smallest code of length k */
    mvInt32 maxcode[17];  		/* largest code of length k (-1 if none) */
    mvUint16 valptr[17];   		/* huffval[] index of 1st symbol of length k */
} strHuffTbl;

// JPEG transfer infomation
typedef struct {
    //---------------------------------------------------
    // for Input ...
    //  needs in every scan lines
    mvUint16      nImage_Width;      // JPEG Size X
    mvUint16      nImage_Height;     // JPEG Size Y
    mvUint8       *input;            // current bit-extraction buffer 
    mvUint32      nHuffman_BufferD;  // # of unused bits in it  
    mvInt32       nHuffman_BitsLeftD;
    //---------------------------------------------------
    // Huffman tables
    
// Lib 1.15 : Huffman table buffer name change.
#if 0    
    strHuffTbl  strHuffTbl_Luminance[2];		// Luminance huffman table
    strHuffTbl  strHuffTbl_Chrominance[2];		// Chrominance huffman table
#else
    strHuffTbl  strHuffTbl_DC_Luminance;
    strHuffTbl  strHuffTbl_AC_Luminance;

    strHuffTbl  strHuffTbl_DC_Chrominance;
    strHuffTbl  strHuffTbl_AC_Chrominance;
#endif
   jmp_buf setjmp_buffer;	/* for return to caller */ 
} strJpegInfo;

//========================================================================
// Lib 1.15 : Rename search_marker() to mv3JpegHeaderAnalysis()                             
extern mvBoolean  mv3JpegHeaderAnalysis(MV3_JPEG_INFO_STRUCT *jpginquiry, 
								mvUint8 *buf,
								mvUint32 *size);

extern mvBoolean mv3JpegWrite(MV3_JPEG_INFO_STRUCT *jinfo,
                              mvUint8 *jpg,
                              mvUint32 *size,
                              mvUint32 jd_in_addr);

// Lib 1.15 : 
extern void mv3ByteStuffing(MV3_JPEG_INFO_STRUCT *jpegInquiry, mvUint16 *imageAddr);
#endif
                              

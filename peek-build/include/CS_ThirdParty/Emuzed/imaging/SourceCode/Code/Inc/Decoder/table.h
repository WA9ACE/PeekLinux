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
#ifndef INCLUDE_TABLE
#define	INCLUDE_TABLE

#include "common.h"
#include <stdlib.h>
#include "baseBitIn.h"
#include "JpegMacros.h" 	

#ifndef VLC_STRUCT_INLINE
typedef struct VlcTbl_JDEmz
{
	uint8 value;
	uint8 length;
    uint16 align0;	/*Added this variable to make structure aligned*/
}tVlcTbl_JDEmz;
#endif

typedef struct HuffmanTable_JDEmz tHuffmanTable_JDEmz;
struct HuffmanTable_JDEmz
{
	uint8  tableClass;			/* For ac/dc table*/
	uint8  identifier;			/* Identifier of the Huffman Table*/
	uint8  bits[16];
	uint16	align0;	/*Added this variable to make structure aligned*/
	uint8 *huffSize;
	int16 *huffCode;
    uint8  valPtr[16];
   	uint8 *huffValues;
	int32  minCode[16];
#ifndef VLC_STRUCT_INLINE
    tVlcTbl_JDEmz  vlcTable[256];	
#else
	uint16 vlcTable[256];	
#endif
};

typedef  struct EntropyTables_JDEmz tEntropyTables_JDEmz;

struct EntropyTables_JDEmz
{
	tHuffmanTable_JDEmz *DC;
	tHuffmanTable_JDEmz *AC;
};

typedef struct QTable_JDEmz tQTable_JDEmz;
struct QTable_JDEmz
{
	uint8  precision;
	uint8  identifier;
	uint8  qData[64];
	uint16	align0;		/*Added this variable to make structure aligned*/
};

#endif		/*INCLUDE_TABLE*/

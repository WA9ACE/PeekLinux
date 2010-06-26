/*
*******************************************************************************
                       Copyright eMuzed Inc., 2004-2005.
All rights Reserved, Licensed Software Confidential and Proprietary Information 
    of eMuzed Incorporation Made available under Non-Disclosure Agreement OR
                            License as applicable.
*******************************************************************************
*/

/*
*******************************************************************************
Product     : JPEG Image Codec
Module      : Frame Decode
File        : frame_decode.h
Description : It contains definition of functions for decoding scan and frame.

Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date            Id          Author            Comment 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Jan 2, 2005					Durga prasad.P	  Jump tables are added.
Jan 17, 2005				Jayaprakash Pai	  SyncEOIMarker added.
*******************************************************************************
*/

#ifndef INCLUDE_FRAME_DECODE_JMP_TABLE
#define	INCLUDE_FRAME_DECODE_JMP_TABLE

#include "frame_decode.h"
#include "texture_decode.h"
typedef struct
{

	/* ROM->ROM jumps (patchability) */
	int32 (*const vJDEmz_DecodeScan)(tJpegDecoder_JDEmz* , int32);
	//int32 (*const vJDEmz_DecodeFrame)(tBaseImageDecoder_JDEmz*, int32);
	
	int32 (*const vJDEmz_ShowBits)(tBaseBitIn_JDEmz* , int32);

	int32 (*const vJDEmz_GetBits)(tBaseBitIn_JDEmz*base, int32 numBits);

	void (*const vJDEmz_FlushBits)(tBaseBitIn_JDEmz *base, int32 numBits);

	/* Inverse Transform functions */
	void (*const vJDEmz_RowIDCT)(int16*);

	void (*const vJDEmz_ColumnIDCT)(int16*);
	
	void (*const vJDEmz_RowIDCT2)(int16*);

	void (*const vJDEmz_ColumnIDCT2)(int16*);

	void (*const vJDEmz_RowIDCT4)(int16*);

	void (*const vJDEmz_ColumnIDCT4)(int16*);

	void (*const vJDEmz_IDCT)(int16*, int32);

	//void (*const vJDEmz_IDCT0)(int16*, int32);

	void (*const vJDEmz_InitBitsJpegTcoeff)(tBaseBitIn_JDEmz*, int32*);

	int32 (*const vJDEmz_ShowBitsJpegTcoeff)(tBaseBitIn_JDEmz*, int32, int32*);

	int32 (*const vJDEmz_GetBitsJpegTcoeff)(tBaseBitIn_JDEmz*, int32, int32*);

	void (*const vJDEmz_FlushBitsJpegTcoeff)(tBaseBitIn_JDEmz*, int32, int32*);

	int32 (*const vJDEmz_Decode)(tBaseBitIn_JDEmz*, tHuffmanTable_JDEmz*, int32*);

	int32 (*const vJDEmz_DecodeTCoeff)(tJpegDecoder_JDEmz*, tComponent_JDEmz*, 
					 int16*, int32*);

	void (*const vJDEmz_StoreUnit)(uint8*, int16*, int32 ,int32 ,int32 ,int32);

	void (*const vJDEmz_StoreUnit1)(uint8*, int16*, int32);

	void (*const vJDEmz_StoreUnit2)(uint8*, int16*, int32);

	void (*const vJDEmz_StoreUnit4)(uint8*, int16*, int32);

	void (*const vJDEmz_StoreUnit8)(uint8*, int16*);

	/* texture2_decode.c*/
	int32 (*const vJDEmz_DecodeDataUnit)(tJpegDecoder_JDEmz*, tComponent_JDEmz*, 
                       int32 , int32);

	int32 (*const vJDEmz_DecodeMCU)(tJpegDecoder_JDEmz*, tMcu_JDEmz*, int32*);

	int32 (*const vJDEmz_ParseMCU)(tJpegDecoder_JDEmz*, tMcu_JDEmz*, int32*);

	/* for ROM to FLASH calls */
	int32 (*const vJDEmz_InterpretMarker)(tJpegDecoder_JDEmz* , int32*);

	void (*const vJDEmz_ReSyncMarker)(tBaseBitIn_JDEmz*);

	int32 (*const vJDEmz_ParseScanHeader)(tJpegDecoder_JDEmz*);

	void (*const vJDEmz_ParseScanComponent)(tJpegDecoder_JDEmz*);

	void (*const vJDEmz_SyncEndOfImageMarker)(tBaseBitIn_JDEmz*);

	uint32 (*const vJDEmz_GetAvailableBytes)(tBaseBitIn_JDEmz*);

} T_JDEMZ_ROM_JMPTAB;

extern const T_JDEMZ_ROM_JMPTAB *jd_emz_jmptab_p;

#define JDEMZDECODESCAN(_p1, _p2)	jd_emz_jmptab_p->vJDEmz_DecodeScan(_p1, _p2)
//#define JDEMZDECODEFRAME		jd_emz_jmptab_p->vJDEmz_DecodeFrame
#define JDEMZINTERPRETMARKER(_p1, _p2) jd_emz_jmptab_p->vJDEmz_InterpretMarker(_p1, _p2)
#define JDEMZSHOWBITS(_p1, _p2)		jd_emz_jmptab_p->vJDEmz_ShowBits(_p1, _p2)
#define JDEMZGETBITS(_p1, _p2)		jd_emz_jmptab_p->vJDEmz_GetBits(_p1, _p2)
#define JDEMZFLUSHBITS(_p1, _p2)	jd_emz_jmptab_p->vJDEmz_FlushBits(_p1, _p2)

/* Macros for Inverse Transform functions */

#define JDEMZROWIDCT(_p1)		jd_emz_jmptab_p->vJDEmz_RowIDCT(_p1)
#define JDEMZCOLUMNIDCT(_p1)	jd_emz_jmptab_p->vJDEmz_ColumnIDCT(_p1)
#define JDEMZROWIDCT2(_p1)		jd_emz_jmptab_p->vJDEmz_RowIDCT2(_p1)
#define JDEMZCOLUMNIDCT2(_p1)	jd_emz_jmptab_p->vJDEmz_ColumnIDCT2(_p1)
#define JDEMZROWIDCT4(_p1)		jd_emz_jmptab_p->vJDEmz_RowIDCT4(_p1)
#define JDEMZCOLUMNIDCT4(_p1)	jd_emz_jmptab_p->vJDEmz_ColumnIDCT4(_p1)
#define JDEMZIDCT(_p1, _p2)		jd_emz_jmptab_p->vJDEmz_IDCT(_p1, _p2)
//#define JDEMZIDCT0(_p1, _p2)	jd_emz_jmptab_p->vJDEmz_IDCT0(_p1, _p2)

/* BaseBitin_jpeg Macros */
#define JDEMZINITBITJPEG(_p1, _p2)	jd_emz_jmptab_p->vJDEmz_InitBitsJpegTcoeff(_p1,_p2)
#define JDEMZGETBITJPEG(_p1, _p2, _p3)  \
		jd_emz_jmptab_p->vJDEmz_GetBitsJpegTcoeff(_p1,_p2, _p3)
#define JDEMZSHOWBITJPEG(_p1, _p2, _p3) \
		jd_emz_jmptab_p->vJDEmz_ShowBitsJpegTcoeff(_p1, _p2, _p3)
#define JDEMZFLUSHBITJPEG(_p1, _p2, _p3) \
		jd_emz_jmptab_p->vJDEmz_FlushBitsJpegTcoeff(_p1, _p2, _p3)

/* vld.c Macros*/

#define JDEMZDECODE(_p1, _p2, _p3)		jd_emz_jmptab_p->vJDEmz_Decode(_p1, _p2, _p3)
#define JDEMZDECODETCOEFF(_p1, _p2, _p3, _p4) \
		jd_emz_jmptab_p->vJDEmz_DecodeTCoeff(_p1, _p2, _p3, _p4)

/* texturedecode1.c Macros -- reconstruction */
#define JDEMZSTOREUNIT(_p1, _p2, _p3, _p4, _p5, _p6) \
		jd_emz_jmptab_p->vJDEmz_StoreUnit(_p1, _p2, _p3, _p4, _p5, _p6)
#define JDEMZSTOREUNIT1(_p1, _p2, _p3)	 jd_emz_jmptab_p->vJDEmz_StoreUnit1(_p1, _p2, _p3)
#define JDEMZSTOREUNIT2(_p1, _p2, _p3)	 jd_emz_jmptab_p->vJDEmz_StoreUnit2(_p1, _p2, _p3)
#define JDEMZSTOREUNIT4(_p1, _p2, _p3)	 jd_emz_jmptab_p->vJDEmz_StoreUnit4(_p1, _p2, _p3)
#define JDEMZSTOREUNIT8(_p1, _p2)		jd_emz_jmptab_p->vJDEmz_StoreUnit8(_p1, _p2)

/* texture2_decode.c Macros */
#define JDEMZDECODEDATAUNIT(_p1, _p2, _p3, _p4) \
		jd_emz_jmptab_p->vJDEmz_DecodeDataUnit(_p1, _p2, _p3, _p4)
#define JDEMZDECODEMCU(_p1, _p2, _p3)	jd_emz_jmptab_p->vJDEmz_DecodeMCU(_p1, _p2, _p3)
#define JDEMZPARSEMCU(_p1, _p2, _p3)	jd_emz_jmptab_p->vJDEmz_ParseMCU(_p1, _p2, _p3)
/* ROM to FLASH function calls */
#define JDEMZRESYNCMARKER(_p1)			jd_emz_jmptab_p->vJDEmz_ReSyncMarker(_p1)
#define JDEMZPARSESCANHEADER(_p1)		jd_emz_jmptab_p->vJDEmz_ParseScanHeader(_p1)
#define JDEMZPARSESCANCOMPONENT(_p1)	jd_emz_jmptab_p->vJDEmz_ParseScanComponent(_p1)
#define JDEMZSYNCENDOFIMAGEMARKER(_p1)	jd_emz_jmptab_p->vJDEmz_SyncEndOfImageMarker(_p1)
#define JDEMZGETAVIALABLEBYTES(_p1)		jd_emz_jmptab_p->vJDEmz_GetAvailableBytes(_p1)

#endif /*FRAME_DECODE_JMP_TABLE*/

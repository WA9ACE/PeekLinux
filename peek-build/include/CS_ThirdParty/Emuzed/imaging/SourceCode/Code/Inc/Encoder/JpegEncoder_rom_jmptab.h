/*
*******************************************************************************
                       Copyright eMuzed Inc., 2003-2004.
All rights Reserved, Licensed Software Confidential and Proprietary Information
    of eMuzed Incorporation Made available under Non-Disclosure Agreement OR
                            License as applicable.
*******************************************************************************
*/

/*
*******************************************************************************
Product     : JPEG encoder
Module      : Jump Table genaration
File        : JpegEncoder_rom_jmptab.c
Description : This file contains the declarations for jump tables  

Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date            Id          Author              Comment
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Jan 04, 2005   Intial Code	G.Nageswara Rao		 Jump table data structure
*******************************************************************************
*/
#ifndef INCLUDE_JUMPTABLE
#define INCLUDE_JUMPTABLE
#include "JPEG_enc_includes.h"
/*
*******************************************************************************
Name            : gPutConfigurationData_JpegEnc
Description     : Function to put the header info into a given buffer
Parameter       : 
Return Value    : None
*******************************************************************************
*/

typedef struct
{
	/* ROM -> ROM jumps (patchabilty) */
	
	int32 (*EncodeMCURow_JpegEncEmz)(tJpegImageEncoder*, uint32 );

	void (*CodeMCU_JpegEncEmz)(tMcu *, tDCPrediction *, tJpegImageEncoder*,tPutBitsInfo *);

#ifndef ARM9TDMI_INLINE
	void (*FormMCU_JpegEncEmz)(tMcu *, tJpegImageEncoder *);
# else
	void (*FormMCU_JpegEncEmz)(tMcu *, int32, int32); 

#endif

	void (*InitBaseBitOutJpeg_JpegEncEzd) (tBaseBitOut * ,uint8 *,uint32*); 
	void (*PutBits_JpegEzd )(tBaseBitOut *, int32, uint32);
	void (*InitBaseBitOutStreaming_JpegEzd)(tBaseBitOut *,uint8 *,uint32*);
	void (*PutJfifHeader)(tBaseBitOut *);

	void (*PutQuantTables)(tJpegImageEncoder*, tPutBitsInfo *);
	void (*PutSofHeader)(tJpegImageEncoder*, tPutBitsInfo *);
	void (*PutSosHeader)(tJpegImageEncoder*, tPutBitsInfo *);
	void (*PutHuffTables)(tJpegImageEncoder*, tPutBitsInfo *);

#ifndef ARM9TDMI_INLINE
	void (*DCT_Jpeg_enc)(int16* );
	int32 (*QuantBlock_Jenc)(int16 *, uint16 *,uint16 *, int32);
	void (*RowDCT_Jenc)(int16*);
	void (*ColumnDCT_Jenc)(int16*);
#else
	void (*DCTQuant_Jpeg_enc)(int16*,uint16 *,uint16 *, int32,int32*);
#endif
	

	int32 (*GetParamJpegImageEncoder)(tBaseImageEncoder*, uint32, uint32*);

	int32 (*JpegEncodeImage)(tBaseImageEncoder*, tBaseYUVFrame*, uint8*, uint32*, uint32);

	int32 (*PutConfigurationData_JpegEnc)(tBaseImageEncoder *, tPixel *, uint32 *);

	void (*EntropyCodeACDC)(int16*, tBaseBitOut*, tEntropyTables*, tPutBitsInfo*, int32);
#ifndef ARM9TDMI_INLINE
	void (*PutBytes_JpegEncEmz) (tBaseBitOut*, int32, uint8*);
#endif


	/* Function ptr to set parameters */
	int32 (*vSetParam) (tBaseImageEncoder *, uint32, uint32);  
	/* Function ptr to get a specified parameter value */
	int32   (*vDelete) (tBaseImageEncoder *);


} T_JPEGENC_ROM_JMPTAB; 


//extern const T_JPEGENC_ROM_JMPTAB *JpegEnc_rom_jmptab_p; 
extern const T_JPEGENC_ROM_JMPTAB *JpegEnc_rom_jmptab_p; 


#ifndef ARM9TDMI_INLINE
#define  FORMMCUYUYV422_JPEGENC(_p1,_p2) gFormMCUYUYV422to420(_p1,_p2)
#define  FORMMCUYUV420P_JPEGENC(_p1,_p2) gFormMCU420(_p1,_p2)
#define  FORMMCUYUV422P_JPEGENC(_p1,_p2) gFormMCU422(_p1,_p2)
#else
#define  FORMMCUYUYV422_JPEGENC(_p1,_p2,_p3) gFormMCUYUYV422to420(_p1,_p2, _p3)
#define  FORMMCUYUV420P_JPEGENC(_p1,_p2,_p3) gFormMCU420(_p1,_p2, _p3)
#define  FORMMCUYUV422P_JPEGENC(_p1,_p2,_p3) gFormMCU422(_p1,_p2, _p3)
#endif

#define  ENCODEMCUROW_JPEGENC(_p1, _p2) JpegEnc_rom_jmptab_p->EncodeMCURow_JpegEncEmz(_p1, _p2)
#define  CODEMCU_JPEGENC(_p1,_p2,_p3,_p4) JpegEnc_rom_jmptab_p->CodeMCU_JpegEncEmz(_p1,_p2,_p3,_p4)

#ifndef ARM9TDMI_INLINE
#define  FORMMCU_JPEGENC(_p1,_p2) JpegEnc_rom_jmptab_p->FormMCU_JpegEncEmz(_p1,_p2)
#else
#define  FORMMCU_JPEGENC(_p1,_p2,_p3) JpegEnc_rom_jmptab_p->FormMCU_JpegEncEmz(_p1,_p2,_p3)
#endif

#define  INITBASEBIT_JPEGENC(_p1,_p2,_p3) JpegEnc_rom_jmptab_p->InitBaseBitOutJpeg_JpegEncEzd(_p1,_p2,_p3)
#define  INITBASEBITSTREAM_JPEGENC(_p1,_p2,_p3) JpegEnc_rom_jmptab_p->InitBaseBitOutStreaming_JpegEzd(_p1,_p2,_p3)
#define  PUTBITS_JPEGENC(_p1,_p2,_p3) JpegEnc_rom_jmptab_p->PutBits_JpegEzd(_p1,_p2,_p3)
#define  JIFFHEADER_JPEGENC(_p1) JpegEnc_rom_jmptab_p->PutJfifHeader(_p1)

#define  PUTQUANTTABLE_JPEGENC(_p1,_p2) JpegEnc_rom_jmptab_p->PutQuantTables(_p1,_p2)
#define  PUTSOFHEADER_JPEGENC(_p1,_p2) JpegEnc_rom_jmptab_p->PutSofHeader(_p1,_p2)
#define  PUTSOSHEADER_JPEGENC(_p1,_p2) JpegEnc_rom_jmptab_p->PutSosHeader(_p1,_p2)
#define  PUTHUFFTABLE_JPEGENC(_p1,_p2) JpegEnc_rom_jmptab_p->PutHuffTables(_p1,_p2)

#ifndef ARM9TDMI_INLINE
#define  DCT_JPEGENC(_p1) JpegEnc_rom_jmptab_p->DCT_Jpeg_enc(_p1)
#define  QUANT_JPEGENC(_p1,_p2,_p3,_p4) JpegEnc_rom_jmptab_p->QuantBlock_Jenc(_p1,_p2,_p3,_p4)
#define  ROWDCT_JPEGENC(_p1) JpegEnc_rom_jmptab_p->RowDCT_Jenc(_p1)
#define  COLDCT_JPEGENC(_p1) JpegEnc_rom_jmptab_p->ColumnDCT_Jenc(_p1)
#else
#define  DCTQUANT_JPEGENC(_p1,_p2,_p3,_p4,_p5) JpegEnc_rom_jmptab_p->DCTQuant_Jpeg_enc(_p1,_p2,_p3,_p4,_p5)
#endif

#define  GETPARAM_JPEGENC(_p1,_p2,_p3) JpegEnc_rom_jmptab_p->GetParamJpegImageEncoder(_p1,_p2,_p3)
#define  JPEGENCODE_JPEGENC(_p1,_p2,_p3,_p4) JpegEnc_rom_jmptab_p->JpegEncodeImage(_p1,_p2,_p3,_p4)
#define  PUTCONFIGDATA_JPEGENC(_p1,_p2,_p3) JpegEnc_rom_jmptab_p->PutConfigurationData_JpegEnc(_p1,_p2,_p3)
#define  ENTROPYCODE_JPEGENC(_p1,_p2,_p3,_p4,_p5) JpegEnc_rom_jmptab_p->EntropyCodeACDC(_p1,_p2,_p3,_p4,_p5)
#define  DELETE_JPEGENC(_p1) JpegEnc_rom_jmptab_p->vDelete(_p1)
#define  SETPARAM_JPEGENC(_p1,_p2,_p3) JpegEnc_rom_jmptab_p->vSetParam(_p1,_p2,_p3)
#ifndef ARM9TDMI_INLINE
#define	 PUTBYTES_JPEGENC(_p1, _p2,_p3) JpegEnc_rom_jmptab_p->PutBytes_JpegEncEmz(_p1, _p2,_p3)
#endif
#define  FORMBOUNDARYMCU(_p1,_p2,_p3)JpegEnc_rom_jmptab_p->FormBoundryMCU(_p1, _p2,_p3);
#endif

//end



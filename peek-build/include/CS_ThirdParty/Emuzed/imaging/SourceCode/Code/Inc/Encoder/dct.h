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
Product     : JPEG Baseline Encoder
Module      : Transform
File        : transform_scaled_chain.h
Description : This is the include file for the C file transform_scaled_chain.c

Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date            Id          Author            Comment
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Jan 31, 2001                Tushar Adhikary   Initial code

*******************************************************************************
*/
#ifndef DCT_INCLUDE
#define DCT_INCLUDE

#ifndef ARM9TDMI_INLINE

void sRowDCT_Jenc(int16 *block);
void sColumnDCT_Jenc(int16 *block);

#endif

#ifndef ARM9TDMI_INLINE
	extern void gDCT_Jpeg_enc(int16* );
#else
	extern void gDCTQuant_Jpeg_enc(int16*,uint16 *,uint16 *, int32,int32*);
#endif



/* Constant multipliers */
#define TAN_PI_BY_8     27145
#define TAN_PI_BY_16    13036
#define TAN_3PI_BY_16   43789
#define COS_PI_BY_4     46340
#define COS_PI_BY_8     60546
#define COS_PI_BY_16    64276
#define COS_3PI_BY_16   54490

/* Shift amount and corresponding rounding constants for DCT */
#define DCT_PRECISION            16
#define PRESHIFT 1


#define DCT_KEPT_PRECISION       1
#define DCT_PRECISION_PLUS_KEPT  19      /* DCT_PRECISION + 2 + DCT_KEPT_PRECISION */
#define DCT_ROUND_PLUS_KEPT      0//262144  /* 2^(DCT_PRECISION_PLUS_KEPT - 1) */
//#define DCT_ROUND                0//32768   /* 2^(DCT_PRECISION - 1) */


/* Shift amount and corresponding rounding constants for IDCT */
#define IDCT_KEPT_PRECISION       4
#define IDCT_PRECISION_MINUS_KEPT 12     /* DCT_PRECISION - IDCT_KEPT_PRECISION */
#define IDCT_ROUND_MINUS_KEPT     2048   /* 2^(IDCT_PRECISION_MINUS_KEPT - 1) */
#define KEPT_PRECISION_PLUS_2     6      /* IDCT_KEPT_PRECISION + 2 */
#define ROUND_KEPT_PLUS_2         32     /* 2^(KEPT_PRECISION_PLUS_2 - 1) */


/* Shift amount and corresponding rounding constants for DCT */
#define KEPT_PRECISION_PLUS_2     6      /* IDCT_KEPT_PRECISION + 2 */
#define ROUND_KEPT_PLUS_2         32     /* 2^(KEPT_PRECISION_PLUS_2 - 1) */




#endif

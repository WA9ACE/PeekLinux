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
Product     : MPEG-4 Video Codec
Module      : Transform
File        : exp_transform_scaled_chain.h
Description : This include file contains the global variables and functions 
              exported by the C file transform_scaled_chain.c
Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date            Id          Author            Comment 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Jan 31, 2001                Tushar Adhikary   Initial code

*******************************************************************************
*/

#ifndef INCLUDE_EXP_TRANSFORM_SCALED_CHEN
#define	INCLUDE_EXP_TRANSFORM_SCALED_CHEN

/* Shift amount and corresponding rounding constants for DCT */
#define IDCT_KEPT_PRECISION       4
#define IDCT_PRECISION_MINUS_KEPT 12     /* DCT_PRECISION - IDCT_KEPT_PRECISION */
#define IDCT_ROUND_MINUS_KEPT     2048   /* 2^(IDCT_PRECISION_MINUS_KEPT - 1) */ 
#define KEPT_PRECISION_PLUS_2     6      /* IDCT_KEPT_PRECISION + 2 */
#define ROUND_KEPT_PLUS_2         32     /* 2^(KEPT_PRECISION_PLUS_2 - 1) */


void JDEmz_gIDCT0(int16 *block, int32 scale);
void JDEmz_gIDCT(int16 *block, int32 scale);
void JDEmz_sColumnIDCT(int16 *block);
void JDEmz_sColumnIDCT2(int16 *block);
void JDEmz_sColumnIDCT4(int16 *block);
void JDEmz_sRowIDCT(int16 *block);
void JDEmz_sRowIDCT2(int16 *block);
void JDEmz_sRowIDCT4(int16 *block);
#endif  /* INCLUDE_EXP_TRANSFORM_SCALED_CHEN */

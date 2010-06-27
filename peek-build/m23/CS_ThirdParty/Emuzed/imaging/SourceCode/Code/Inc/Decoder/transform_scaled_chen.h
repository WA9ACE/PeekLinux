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
File        : transform_scaled_chain.h
Description : This is the include file for the C file transform_scaled_chain.c

Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date            Id          Author            Comment 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Jan 31, 2001                Tushar Adhikary   Initial code

*******************************************************************************
*/

#ifndef INCLUDE_TRANSFORM_SCALED_CHEN
#define	INCLUDE_TRANSFORM_SCALED_CHEN

#include <stdlib.h>
#include "common.h"
#include "jpegmacros.h"
#include "exp_transform_scaled_chen.h"

/* Constant multipliers */
#define TAN_PI_BY_8     27145
#define TAN_PI_BY_16    13036
#define TAN_3PI_BY_16   43789
#define COS_PI_BY_4     46340 

/* Shift amount and corresponding rounding constants for DCT */
#define DCT_PRECISION            16
#define DCT_ROUND                32768   /* 2^(DCT_PRECISION - 1) */ 


#endif  /* INCLUDE_TRANSFORM_SCALED_CHEN */

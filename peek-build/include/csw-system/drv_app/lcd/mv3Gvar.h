/*-------------------------------------------------------------------
File name : mv3Gvar.h

MV400 middle level camera control routines

(C) Copyright MtekVision, Inc. 2003~2004 All right reserved.
  Confidential Information

All parts of the MtekVision Program Source are protected by copyright law 
and all rights are reserved. 
This documentation may not, in whole or in part, be copied, photocopied, 
reproduced, translated, or reduced to any electronic medium or machine 
readable form without prior consent, in writing, from the MtekVision. 

Last modification : 2005. 01. 15
----------------------------------------------------------------------*/

/*------------- MV400 IC Selection ----------------------------
	Use Makefile's -D option.
-----------------------------------------------------------*/
#ifndef __MV3GVAR_H__    // mv3StructDef.h ¿¡ ÀÌ ÄÚµå¸¦ ³ÖÀ¸¸é Error ¹ß»ý...å?å?å? ±×·¡¼­ ¿©±â¿¡ ³ÖÀ½...
#define __MV3GVAR_H__

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

#include "mv3Lib.h"
#include "mv3CommonDef.h"
#include "mv3StructDef.h"

#include "mv301xDef.h"

#ifdef _PROGMAIN
    #define GLOBALVAR
#else
    #define GLOBALVAR extern
#endif

/*------------- Global Variable  ------------------------------------*/
GLOBALVAR MV3_INFO_STRUCT	gCamera;

// Fill in with SENSOR_MAX_SIZE * pow(RATIO,i).
GLOBALVAR mvUint16	    	previewZoomSrcMaskTable[MAX_ZOOM_STEP][MAX_NUM_ELEMENTS];

GLOBALVAR MV3_PLL_SET_STRUCT  gPllSet;

#endif // __MV3GVAR_H__
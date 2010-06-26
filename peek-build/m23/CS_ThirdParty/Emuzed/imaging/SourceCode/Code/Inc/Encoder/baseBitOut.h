/*
******************************************************************************
                    Copyright eMuzed Inc., 2001-2002.
All rights Reserved,Licensed Software Confidential and Proprietary Information 
of eMuzed Incorporation Made available under Non-Disclosure Agreement OR 
                        License as applicable.
******************************************************************************
*/

/*
******************************************************************************
Product        : Common Module
Module         : BITIO
File           : baseBitOut.h
Description    : Base bit output object definitions

Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date        Id        Author        Comment    
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
March 2, 2001                Murali              Initial Code
March 5, 2001                Deepak Jaiswal      Moving to common area and
                                                 file name change.
******************************************************************************
*/

#ifndef INCLUDE_BASE_BITOUT
#define INCLUDE_BASE_BITOUT

#include "common.h"

/* Flag for Set/Get Params */

#define   GET_FRM_BITS      0x01     /* Flag for frame bits count  */
#define   GET_PKT_BITS      0x02     /* Flag for packet bits count */
#define   SET_FRM_END       0x03     /* Signal end of frame */
#define   SET_PKT_END       0x04     /* PKT end Signal, this is not send
                                        when SET_FRM_END is signaled */
#define   GET_BUF1_HANDLE   0x05     /* First partition handle */
#define   GET_BUF2_HANDLE   0x06     /* Second partition handle */
#define   GET_BUF3_HANDLE   0x07     /* Third partition handle */

#define   GET_TOC_START     0x08
#define   SET_FRM_TYPE	    0x09
#define   SET_TOTAL_FRAMES  0x0a
#define	  SET_CODEC_MODE    0x0b

#define   SET_FRM_PRIORITY  0x0c   /* Flag for Frame priority   */

#define   SET_SKIP_END      0xff

/* Hash defines for frame type */
#define   PR_I_FRAME        0x0   
#define   PR_P_FRAME        0x1

typedef struct BaseBitOut tBaseBitOut;

struct BaseBitOut
{

    uint32  cache;                /* 32 bits cache */
    int32   filledBits;           /* Remaining bits available in the cache */
    uint32 *buffer;               /* Local buffer pointer */
    int32   index;                /* Buffer Index - multiple of four*/
    int32   length;               /* Length of the buffer */
    uint32  cntBits;              /* Count of bits */
    void (*vChangeBuffer) (tBaseBitOut *base);    /* Fn.ptr to change */
                                                  /* buffer           */
    int32 (*vSetParam) (tBaseBitOut *base, int32 flag, int32 val);
                                   /* Fn. ptr to set parameters */
    int32 (*vGetParam) (tBaseBitOut *base, int32 flag, int32 *val);
                                /* Fn. ptr to get the parameter values */
    void (*vDelete) (tBaseBitOut *base);
                                /* Delete the base object & derived object */
};

/* Global function exported */
void gInitBaseBitOutJpeg_JpegEncEzd (tBaseBitOut *base ,uint8 *OutputBuffer,uint32* length); 
void gPutBits_JpegEzd (tBaseBitOut *base, int32 numBits, uint32 value);

extern void gInitBaseBitOutStreaming_JpegEzd(tBaseBitOut *base ,uint8 *OutputBuffer,uint32* length);
		

#endif /* INCLUDE_BASE_BITOUT */

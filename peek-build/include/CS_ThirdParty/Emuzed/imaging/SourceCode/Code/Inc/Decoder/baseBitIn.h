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
Product        : Common Module
Module         : BITIO
File           : baseBitIn.h
Description    : header file base bit input object.

Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date               Id        Author              Comment    
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
March 2, 2001                Murali              Initial Code
March 5, 2001                Deepak Jaiswal      Moving to common area and
                                                 file name change.
******************************************************************************
*/

#ifndef INCLUDE_BASE_BITIN
#define INCLUDE_BASE_BITIN

#include "common.h"

#define MAX_BITS 32

#ifndef NULL
#define NULL  0
#endif


typedef struct BaseBitIn_JDEmz tBaseBitIn_JDEmz;

struct BaseBitIn_JDEmz
{

    uint32  cache;                /* 32 bits cache */
    int32   remBits;              /* Remaining bits available in the cache */
    int32   extraBytes;           /* No.of bytes that falls out of 32 bits */
    uint32 *buffer;               /* Local buffer pointer */
    int32   index;                /* Buffer Index */
    int32   length;               /* Length of the buffer */
    void  (*vChangeBuffer) (tBaseBitIn_JDEmz *base);  /* Fn.ptr to change */
                                                /* buffer           */
    int32 (*vSetParam)(tBaseBitIn_JDEmz *base, int32 flag, int32 val);
                                /* Fn. ptr to set parameters */
    int32 (*vGetParam)(tBaseBitIn_JDEmz *base, int32 flag, int32 *val);
                                /* Fn. ptr to get the parameter values */
    void (*vDelete) (tBaseBitIn_JDEmz *base);
                                /* Delete the base object & derived object */
	uint8   dummyBuffer[8];
	uint32  dummyBytes;

};

/* Forward get bits function */
void  JDEmz_gInitBaseBitIn (tBaseBitIn_JDEmz *base);
int32 JDEmz_gGetBits (tBaseBitIn_JDEmz *base, int32 numBits);
void  JDEmz_gFlushBits (tBaseBitIn_JDEmz *base, int32 numBits);
int32 JDEmz_gShowBits (tBaseBitIn_JDEmz *base, int32 numBits);
int32 gShowBitsAligned (tBaseBitIn_JDEmz *base, int32 numBits);
void JDEmz_gFlushBytes (tBaseBitIn_JDEmz *base, int32 numBytes);

/* Base buffer object operation */
/* This function is called to set a input buffer to the base object */
/* These functions are called when buffer are changed without change 
   buffer functions */
void JDEmz_gSetBaseBitInBuff (tBaseBitIn_JDEmz *base, uint8 *buffer, int32 length);

/* Gets the available bytes in the buffer */
uint32 JDEmz_gGetAvailableBytes(tBaseBitIn_JDEmz *base);

#endif /* INCLUDE_BASE_BITIN */

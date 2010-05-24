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
Product     : Common module 
Module      : 
File        : common.h
Description : This is the commom header to be used by all modules.

Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date            Id          Author          Comment 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Aug 30, 2004                 Jayaprakash Pai   Initial code
*******************************************************************************
*/

#ifndef INCLUDE_COMMON
#define INCLUDE_COMMON

#include "general.h"

#ifndef BASIC_TYPES
#define BASIC_TYPES

#ifndef int8
#define int8 INT8
#endif

#ifndef uint8
#define uint8 UINT8
#endif

#ifndef int16
#define int16 INT16
#endif

#ifndef uint16
#define uint16 UINT16
#endif

#ifndef int32
#define int32 INT32
#endif

#ifndef uint32
#define uint32 UINT32
#endif

#endif/*BASIC_TYPES*/

typedef float                flt32;
typedef double               flt64;

typedef unsigned char        tBool;
typedef signed int           tError;


#define E_TRUE               1
#define E_FALSE              0

#define E_ON                 1
#define E_OFF                0

#define E_SUCCESS            0
#define E_FAILURE           (-1)
#define E_OUT_OF_MEMORY     (-2)
#define E_OUT_OF_RANGE      (-3)
#define E_FILE_CREATE_FAIL  (-4)
#define E_UNDEFINED_FLAG    (-5)
#define E_FILE_READ_FAIL    (-6)
#define E_FILE_OPEN_FAIL    (-7)
#define E_END_OF_FILE		(-8)
#define E_NOT_COMPLETE		(-9)
#define E_INSUFFICIENT_OUTBUFF	(-10)
#define	E_COMPLETE			(-17)

#define E_INSUFFICIENT_DATA	(-11)
#define E_ERR_ARGUMENT		(-12)
#define E_NOT_SUPPORTED		(-13)
#define E_PARTIAL_DECODED	(-14)
#define E_CORRUPT			(-15)
#define E_ERROR_ARGUMENT	(-16)


#define mIsSuccess(code)    ((code)>=0)
#define mIsFailure(code)    ((code)<0)

/* Video Frame structure */
typedef struct BaseVideoFrame_JDEmz
{
    uint8   *lum;       /* Luminance pointer */
	uint8   *cb;        /* Cb pointer */
	uint8   *cr;        /* Cr pointer */
	uint16  width;      /* Width of the frame */
	uint16  height;     /* Height of the frame */
	uint32  timeStamp;  /* Playback/Capture Timestamp */

} tBaseVideoFrame_JDEmz;
typedef struct{
uint8* luma;
uint8* cb;
uint8* cr;
}tBaseYUVFrame;

#define SWAP_BYTE(x) ((x<<24) | (x>>24) | ((x&0xFF00)<<8) | ((x&0xFF0000)>>8))

/* Color Formats */

#define   YUV420           0x01    
#define   YUV422H          0x02	// Chrominance subsampling in Horizontal direction 
#define   YUYV		       0x03	
#define   YUV444           0x04
#define   RGB444           0x05
#define   MONOCHROME       0x06
#define	  YUV422V		   0x07 // Chrominance subsampling in Vertical direction 	
#define	  RGB16			   0x08

/* Output data format */
#define   PLANAR			0	
#define   INTERLEAVED		1

#endif  /* INCLUDE_COMMON */

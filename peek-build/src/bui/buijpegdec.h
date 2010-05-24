#ifndef __JPEGDEC_H__
#define __JPEGDEC_H__ 
#define BYTES_PER_PIXEL 2

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sysdefs.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*normal types*/
#ifdef BYD_USE_SIM
	typedef short   int16; 
	typedef unsigned char    uint8;
	typedef unsigned short   uint16;
#endif
	typedef unsigned char		byte;
	typedef char				sbyte;
	typedef unsigned short		word;
	typedef short int			sword;
	typedef unsigned int		dword;
	typedef signed int		sdword;

typedef enum {
	JPEG_ERROR_SUCCESS = 0,
	JPEG_ERROR_NO_MEMORY,
	JPEG_ERROR_NO_FILE,
	JPEG_ERROR_BAD_FILE,
	JPEG_ERROR_DEFAULT
}JpegErrorType;

typedef struct {
	uint16 w;
	uint16 h;
        uint8  BitCount;
	byte *Bmp;
}JpegBitmapType;

typedef struct
{
  word XImage;        /*X pixels of image*/
  word YImage;        /*Y pixels of image*/
  word XRound;        /* The dimensions rounded to multiple of Hmax*8*/
  word YRound;        /* The dimensions rounded to multiple of Vmax*8 */
  dword Length;       /*file length*/
  dword XImageBytes;  /* size in bytes of 1 line of the image = X_round * 4*/
  dword YIncValue;    /*/BYTES_PER_8PIXELS*X_round; // used by decode_MCU_1x2,2x1,2x2*/
  byte YH;
  byte YV;            /* sampling factors (horizontal and vertical) for Y,Cb,Cr*/
  byte CbH;
  byte CbV;
  byte CrH;
  byte CrV;
  word Hmax;
  word Vmax;
}JpegInfoType;

JpegErrorType JpegDecode(char *Filename, JpegBitmapType *BitmapPtr);
JpegErrorType JpegDecodeBuffer(const uint8* BufferP, uint32 BuffLen,	JpegBitmapType *BitmapPtr);
JpegErrorType JpegTest(byte *pData,JpegBitmapType *pBitmap);
void JpegFreeMemory(void *pBuf);
void *JpegGetMemory(int32 nSize);

#ifdef __cplusplus
}
#endif /* __cplusplus */

 /*****************************************************************************
 * $Log: buijpegdec.h $
 * Revision 1.2  2007/11/09 17:39:14  gzhu
 * revise for 1bit, 8bit bmp display
 * Revision 1.1  2007/11/08 18:25:32  gzhu
 * Initial revision
 *****************************************************************************/
 


#endif


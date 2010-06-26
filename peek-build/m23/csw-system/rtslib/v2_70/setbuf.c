/*****************************************************************************/
/*  SETVBUF.C v2.54                                                          */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                   */
/*****************************************************************************/

/*****************************************************************************/
/* Functions:                                                                */
/*    SETBUF   -  Set the default buffering mode for a stream                */
/*****************************************************************************/
#include <stdio.h>
 

/*****************************************************************************/
/* SETBUF   -  Set the default buffering mode for a stream                   */
/*                                                                           */
/*    This function calls SETVBUF, and sets the buffering mode to non-       */
/*    buffered if _BUF is NULL, or fully-buffered if _BUF is not NULL.       */
/*                                                                           */
/*****************************************************************************/
_CODE_ACCESS void setbuf(register FILE *_fp, char *_buf)
{
   setvbuf(_fp, _buf, _buf ? _IOFBF : _IONBF, BUFSIZ);
   return;
}
 


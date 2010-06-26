/*****************************************************************************/
/*  FSEEK.C v2.54                                                            */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                   */
/*****************************************************************************/

/*****************************************************************************/
/* Functions:                                                                */
/*    FSEEK -  Reposition the file pointer of a stream                       */
/*****************************************************************************/
#include <stdio.h>
#include "file.h"

extern int _doflush(FILE *_fp);




/*****************************************************************************/ 
/* FSEEK -  Reposition the file pointer of a stream                          */
/*                                                                           */ 
/*    This funtion flushes stream _FP, clears the EOF and I/O Error flags,   */
/*    repositions the file pointer of the stream.  It returns a 0 upon       */
/*    success, and an EOF upon failure.                                      */
/*                                                                           */ 
/*****************************************************************************/ 
_CODE_ACCESS int fseek(register FILE *_fp, long _offset, int _ptrname)
{
   /*------------------------------------------------------------------------*/
   /* If the current stream is not associated with a file, return an error.  */
   /*------------------------------------------------------------------------*/
   if(_fp->fd == -1) return (EOF);

   /*------------------------------------------------------------------------*/
   /* When positioning to a location relative to the current location,       */
   /* adjust for the fact that there may be something in the buffer.         */
   /*------------------------------------------------------------------------*/
   if(_ptrname == SEEK_CUR && _STCHK(_fp, _MODER) && _fp->buff_stop)
      _offset -= (_fp->buff_stop - _fp->pos);

   _doflush(_fp);

   _UNSET(_fp, (_STATEOF | _UNGETC));
   
   if((lseek(_fp->fd, _offset, _ptrname)) == -1) return (EOF);
   
   return (0);
}
   

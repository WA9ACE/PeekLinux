/*****************************************************************************/
/*  FTELL.C v2.54                                                            */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                   */
/*****************************************************************************/
#include <errno.h>
#include <stdio.h>
#include "file.h"

/*****************************************************************************/
/* FTELL -  Get the location of the file pointer in a stream                 */
/*                                                                           */
/*    This function gets the current location of the file pointer for the    */
/*    given stream, and returns it after adjusting it for any inaccuracies   */
/*    that buffering might have caused.                                      */
/*                                                                           */
/*****************************************************************************/
_CODE_ACCESS long ftell(FILE *_fp)
{
   /*------------------------------------------------------------------------*/
   /* Local variables                                                        */
   /*------------------------------------------------------------------------*/
   fpos_t  _pos;
   int   adjust   = 0;

   /*------------------------------------------------------------------------*/
   /* If the stream pointer given is not currently open, return a -1.        */
   /*------------------------------------------------------------------------*/
   if(_fp->fd == -1) { errno = 5; return (-1L);}

   /*------------------------------------------------------------------------*/
   /* For files in read mode, we must subtract the unread data in the buffer */
   /* from the location of the file pointer.  For files in write mode, we    */
   /* must add the data in the buffer that has not yet gone to disk.         */
   /*------------------------------------------------------------------------*/
   if(_STCHK(_fp, _MODER) && _fp->buff_stop > _fp->pos)
      adjust = -(_fp->buff_stop - _fp->pos);
   if(_STCHK(_fp, _MODEW)) adjust = (_fp->pos - _fp->buf); 
 
   /*------------------------------------------------------------------------*/
   /* Get the file pointer's position                                        */
   /*------------------------------------------------------------------------*/
   _pos = lseek(_fp->fd, 0L, SEEK_CUR);

   /*------------------------------------------------------------------------*/
   /* If the call to lseek was unsuccessful, return an -1                    */
   /*------------------------------------------------------------------------*/
   if(_pos < 0) { errno = 5; return (-1L);}

   /*------------------------------------------------------------------------*/
   /* Make the necessary adjustment, and return the value                    */
   /*------------------------------------------------------------------------*/
   _pos += adjust;
   return (_pos);
}


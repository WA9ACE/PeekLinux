/*****************************************************************************/
/*  _BUFREAD.C v2.54                                                         */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                   */
/*****************************************************************************/
 
/*****************************************************************************/
/* Functions:                                                                */
/*    _BUFF_READ  -  Fill a stream's buffer from its file                    */
/*****************************************************************************/
#include <stdio.h>
#include <_lock.h>
#include "file.h"

extern _DATA_ACCESS int  _ft_end;
extern int  _doflush(FILE *_fp);
extern void _buff_read(FILE *_fp);


/*****************************************************************************/
/* _BUFF_READ  -   Fill a stream's buffer from its file                      */
/*                                                                           */
/*    This function fills stream _FP's buffer with the contents of the file  */
/*    it is associated with.  It returns nothing, but sets flags in the      */
/*    stream if any I/O errors occur.                                        */
/*                                                                           */
/*****************************************************************************/
void _buff_read(FILE *_fp)
{
   /*------------------------------------------------------------------------*/
   /* Local variables                                                        */
   /*------------------------------------------------------------------------*/
   int   errchk,
         j,
         buffer_size    = _fp->bufend - _fp->buf;

   /*------------------------------------------------------------------------*/
   /* If this is a line buffered stream, flush all line buffered streams.    */
   /*------------------------------------------------------------------------*/
   if(_BUFFMODE(_fp) == _IOLBF)
   {
       /*--------------------------------------------------------------------*/
       /* This is a critical section because it depends on the global	     */
       /* variable _ft_end.						     */
       /*--------------------------------------------------------------------*/
       _lock();
       for(j=0; j < _ft_end; j++)
	   if(_BUFFMODE(&_ftable[j]) == _IOLBF)
	       _doflush(&_ftable[j]);
       _unlock();
   }

   /*------------------------------------------------------------------------*/
   /* Read in the next characters from the file.                             */
   /*------------------------------------------------------------------------*/
   errchk = read(_fp->fd, (char *)_fp->buf, buffer_size);

   /*------------------------------------------------------------------------*/
   /* Adjust the buffer pointers.                                            */
   /*------------------------------------------------------------------------*/
   _fp->buff_stop = _fp->buf + errchk;
   _fp->pos = _fp->buf;

   /*------------------------------------------------------------------------*/
   /* Set any error flags if necessary.                                      */
   /*------------------------------------------------------------------------*/
   switch(errchk)
   {
      case -1 : _SET(_fp, _STATERR);
                break;
 
      case 0  : _SET(_fp, _STATEOF);
                break;
   }

   return;
 
}
 

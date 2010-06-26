/*****************************************************************************/
/*  FREAD.C v2.54                                                            */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                   */
/*****************************************************************************/

/*****************************************************************************/
/* Functions:                                                                */
/*    FREAD       -  Read a block of bytes from a stream                     */
/*****************************************************************************/
#include <string.h>
#include <stdio.h>
#include "file.h"
 
extern int  _rd_ok(FILE *_fp);
extern void _buff_read(FILE *_fp);





/*****************************************************************************/
/* FREAD -  Read a block of bytes from a stream                              */
/*                                                                           */
/*    This function reads _COUNT blocks of _SIZE size from stream _FP, and   */
/*    stores them in string _PTR.  The function returns the number of        */
/*    blocks read.                                                           */
/*                                                                           */
/*****************************************************************************/
_CODE_ACCESS size_t fread(void *_ptr, size_t _size, size_t _count, FILE *_fp)
{
   /*------------------------------------------------------------------------*/
   /* Local variables                                                        */
   /*------------------------------------------------------------------------*/
   unsigned char     *fpos       = (unsigned char *)_ptr;
            size_t   num_left    = _size * _count,
                     num_read    = 0,
                     num_to_read = 0;
 
   /*------------------------------------------------------------------------*/
   /* Make sure that the file is readable.                                   */
   /*------------------------------------------------------------------------*/
   if(! _rd_ok(_fp) || _size == 0 || _count == 0) return (0);
 
   /*------------------------------------------------------------------------*/
   /* If the stream is non-buffered, call the lowlevel READ function.	     */
   /*------------------------------------------------------------------------*/
   if(_BUFFMODE(_fp) == _IONBF)
   {
       int num_read = 0;

       while (num_left > 0)
       {
	   int read_return = (size_t)(read(_fp->fd, 
					   (char *)fpos + num_read, num_left));
	   if (read_return < 0)
	   {
	       _SET(_fp, _STATERR);
	       return (num_read / _size);
	   }
	   else if (read_return == 0) 
	   {
	       _SET(_fp, _STATEOF);
	       return (num_read / _size);
	   }
	   else 
	   {
	       num_read += read_return;
	       num_left -= read_return;
	   }
       }

       return (num_read / _size);
   }
   
   while (num_left > 0)
   {
      /*---------------------------------------------------------------------*/
      /* If the buffer has been completely read, fill it up.  Exit the loop  */
      /* if an I/O error occurs, or the end of the file is reached.          */
      /*---------------------------------------------------------------------*/
      if(_fp->pos == _fp->buff_stop || !_fp->buff_stop)  _buff_read(_fp);
      if(_STCHK(_fp, (_STATERR | _STATEOF))) break;

      /*---------------------------------------------------------------------*/
      /* Determine how many characters can fit in the buffer, and read them  */
      /* in.                                                                 */
      /*---------------------------------------------------------------------*/
      num_to_read = (num_left < (_fp->buff_stop - _fp->pos)) ?
                    num_left : (_fp->buff_stop - _fp->pos);
      memcpy(fpos, _fp->pos, num_to_read);

      /*---------------------------------------------------------------------*/
      /* Update pointers and counters.                                       */
      /*---------------------------------------------------------------------*/
      fpos += num_to_read;
      _fp->pos += num_to_read;
      num_read += num_to_read;
      num_left -= num_to_read; 
   }

   /*------------------------------------------------------------------------*/
   /* Clear the _UNGETC flag in the stream, and return the number of blocks  */
   /* read.                                                                  */
   /*------------------------------------------------------------------------*/
   _UNSET(_fp, _UNGETC);
 
   return (num_read / _size);
 
}
 

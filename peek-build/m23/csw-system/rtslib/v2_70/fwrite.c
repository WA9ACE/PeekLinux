/*****************************************************************************/
/*  FWRITE.C v2.54                                                           */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                   */
/*****************************************************************************/

/*****************************************************************************/
/* Functions:                                                                */
/*    FWRITE   -  Write a block of bytes to a stream                         */
/*****************************************************************************/
#include <stdio.h>
#include "file.h"
#include <stdlib.h>
#include <string.h>
 
extern int _doflush(FILE *_fp);
extern int _wrt_ok(FILE *_fp);

 
/*****************************************************************************/
/* FWRITE   -  Write a block of bytes to a stream                            */
/*                                                                           */
/*    This function reads _COUNT blocks of size _SIZE from a buffer          */
/*    pointed to by _PTR, and writes them to stream _FP.  It returns the     */
/*    number of blocks successfully written.                                 */
/*                                                                           */
/*****************************************************************************/
_CODE_ACCESS size_t fwrite(const void *_ptr, size_t _size, size_t _count,
                           register FILE *_fp)
{
   /*------------------------------------------------------------------------*/
   /* Local variables                                                        */
   /*------------------------------------------------------------------------*/
   unsigned char    *fpos       = (unsigned char *)_ptr;
   unsigned char    *nl_pos;
            size_t   buffer_size = (_fp->bufend - _fp->buf),
                     next_nl,
                     room_left;
            size_t   num_left    = _size * _count,
                     num_to_write,
                     num_written  = 0;

   /*------------------------------------------------------------------------*/
   /* Make sure that the stream is writeable.                                */
   /*------------------------------------------------------------------------*/
   if(! _wrt_ok(_fp) || _size == 0 || _count == 0) return (0);
 
   /*------------------------------------------------------------------------*/
   /* If the stream is non-buffered, call the lowlevel WRITE function.       */
   /*------------------------------------------------------------------------*/
   if(_BUFFMODE(_fp) == _IONBF) 
   {
       int num_written = 0;

       while (num_left > 0)
       {
	   int write_return = write(_fp->fd, 
				    (char *)fpos + num_written, num_left);
	   if (write_return <= 0) 
	   { 
	       _SET(_fp, _STATERR); 
	       return (num_written / _size);
	   }
	   else
	   {
	       num_written += write_return;
	       num_left    -= write_return;
	   }
       }

       return (num_written / _size);
   }
 
   room_left   = (_fp->bufend - _fp->pos);
   if (_STCHK(_fp, _IOLBF))
       next_nl = (char *)memchr(fpos, '\n', num_left) - (char *)fpos + 1;
 
   while (num_left > 0)
   {
      /*---------------------------------------------------------------------*/
      /* Determine how many characters should be written based on buffering  */
      /* mode.  For non-buffered streams, call the lowlevel WRITE function.  */
      /* For fully buffered streams, put as many characters in the buffer as */
      /* possible.  For line buffered streams, put characters into the       */
      /* util the buffer is full, the last character is reached, or a        */
      /* newline character is reached.                                       */
      /*---------------------------------------------------------------------*/
      switch (_BUFFMODE(_fp))
      {
         case _IOFBF : num_to_write = (room_left > num_left) ? 
                       num_left : room_left;
                       break;
 
         case _IOLBF : num_to_write = (room_left > next_nl) ? next_nl :
                       (room_left > num_left) ? num_left : room_left;
                       break;
 
         default     : return (0);
      }
 
      /*---------------------------------------------------------------------*/
      /* Write the data to the buffer, and update the buffer pointer and the */
      /* ROOM_LEFT coutner.                                                  */
      /*---------------------------------------------------------------------*/
      memcpy(_fp->pos, fpos, num_to_write);
      _fp->pos += num_to_write;
      room_left = (_fp->bufend - _fp->pos);

      /*---------------------------------------------------------------------*/
      /* If the buffer is full, or a newline character has been encountered  */
      /* on a line-buffered stream, flush it.                                */
      /*---------------------------------------------------------------------*/
      if (room_left == 0 || (_STCHK(_fp, _IOLBF) && num_to_write == next_nl))
      {
         if(_doflush(_fp))
         {
            _SET(_fp, _STATERR); 
            return (num_written / _size);
         }
         room_left = buffer_size;

         /*------------------------------------------------------------------*/
         /* The _DOFLUSH function clears the write flag on streams opened in */
         /* update mode.  Make sure that the write flag is still set here.   */
         /*------------------------------------------------------------------*/
         _SET(_fp, _MODEW);
      }

      /*---------------------------------------------------------------------*/
      /* Update pointers and counters.                                       */
      /*---------------------------------------------------------------------*/
      num_written += num_to_write;
      fpos += num_to_write;
      num_left -= num_to_write;
 
      /*---------------------------------------------------------------------*/
      /* For line-buffered streams, find the next occurance of a newline     */
      /* character.  If there are no more, and the remaining data will fit   */
      /* in the buffer, exit the loop where the remaining data will be moved */
      /* there.  Otherwise loop until this condition is true.                */
      /*---------------------------------------------------------------------*/
      if (_STCHK(_fp, _IOLBF))
      {
         nl_pos = (unsigned char *)memchr((fpos + 1), '\n', 
					  (num_left > 0) ? (num_left-1) : 0);
         if (! nl_pos)
         {
            if (num_left < room_left) break;
            else next_nl = buffer_size + 1;
         }
         else next_nl = (nl_pos - fpos) + 1;
      }
   }

   /*------------------------------------------------------------------------*/
   /* Copy the rest of the characters into the buffer for line-buffered      */
   /* streams.                                                               */
   /*------------------------------------------------------------------------*/
   if (_STCHK(_fp, _IOLBF))
   {
      memcpy(_fp->pos, fpos, num_left);
      num_written += num_left;
      _fp->pos += num_left;
   }
 
   return (num_written / _size);
}
 

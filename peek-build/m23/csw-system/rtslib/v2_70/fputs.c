/*****************************************************************************/
/*  FPUTS.C v2.54                                                            */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                   */
/*****************************************************************************/

/*****************************************************************************/
/* Functions:                                                                */
/*    PUTS  -  Write a string to stdout                                      */
/*    FPUTS -  Write a string to a stream                                    */
/*****************************************************************************/
#include <string.h>
#include <stdio.h>
#include "file.h"

extern int _wrt_ok(FILE *_fp);
extern int _doflush(FILE *_fp);






/*****************************************************************************/
/* PUTS  -  Write a string + newline to stdout.                              */
/*                                                                           */
/*    This function calls FPUTS, with stdout as the stream argument.         */
/*                                                                           */
/*****************************************************************************/
_CODE_ACCESS int puts(const char *_ptr) 
{ 
    int count = fputs(_ptr, stdout);
    count += fputs("\n", stdout); 
    return count;
}



/*****************************************************************************/
/* FPUTS -  Write a string to a stream                                       */
/*                                                                           */ 
/*    This function writes string _PTR to stream _FP, returning the number   */
/*    of characters written upon success, or an EOF upon failure.            */
/*                                                                           */ 
/*****************************************************************************/
_CODE_ACCESS int fputs(const char *_ptr, register FILE *_fp)
{
   /*------------------------------------------------------------------------*/
   /* Local variables                                                        */
   /*------------------------------------------------------------------------*/
   size_t   num_left, ptr_strlen;
   char     *fpos          = (char *)_ptr;
   int      room_left,
            flush_flag     = 0,
            num_to_write;

   /*------------------------------------------------------------------------*/
   /* Make sure that the stream is writeable.                                */
   /*------------------------------------------------------------------------*/
   if(! _wrt_ok(_fp)) return (EOF);
 
   room_left = (int)(_fp->bufend - _fp->pos);
   ptr_strlen = num_left = strlen(_ptr);

   /*------------------------------------------------------------------------*/
   /* If the stream is non-buffered, call the lowlevel WRITE function.       */
   /*------------------------------------------------------------------------*/
   if(_BUFFMODE(_fp) == _IONBF) 
   {
       int num_written = 0;

       while (num_left > 0)
       {
	   int write_return = write(_fp->fd, _ptr + num_written, num_left);
	   if (write_return < 0) 
	   { 
	       _SET(_fp, _STATERR); 
	       return (EOF);
	   }
	   else
	   {
	       num_written += write_return;
	       num_left    -= write_return;
	   }
       }

       return ptr_strlen;
   }
 
   /*------------------------------------------------------------------------*/
   /* Write the string into the buffer, flushing it when full.               */
   /*------------------------------------------------------------------------*/
   while(num_left > 0)
   {
      num_to_write = (num_left > room_left) ? room_left : num_left;
      if((_BUFFMODE(_fp) == _IOLBF) && memchr(fpos, '\n', num_to_write))
      { 
         num_to_write = (char *)memchr(fpos, '\n', num_to_write) - fpos + 1;
         flush_flag = 1;
      }
      memcpy(_fp->pos, fpos, num_to_write);

      /*---------------------------------------------------------------------*/
      /* Update pointers and counters.                                       */
      /*---------------------------------------------------------------------*/
      _fp->pos  += num_to_write;
      fpos      += num_to_write;
      num_left  -= num_to_write;
      room_left -= num_to_write;

      /*---------------------------------------------------------------------*/
      /* If the buffer is full, flush it.  Any I/O errors cause this         */
      /* function to exit, returning an EOF.                                 */
      /*---------------------------------------------------------------------*/
      if(room_left == 0 || flush_flag)
      {
         if(_doflush(_fp))
         {
            _SET(_fp, _STATERR);
            return (EOF);
         }
         room_left = (int)(_fp->bufend - _fp->pos);
         _SET(_fp, _MODEW);
         flush_flag = 0;
      }
   }
   return ptr_strlen;
}


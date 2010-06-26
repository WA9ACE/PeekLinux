/*****************************************************************************/
/*  FPUTC.C v2.54                                                            */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                   */
/*****************************************************************************/

/*****************************************************************************/
/* Functions:                                                                */
/*    PUTC     -  Write a character to a stream                              */
/*    PUTCHAR  -  Write a character to stdout                                */
/*    FPUTC    -  Write a character to a stream                              */
/*****************************************************************************/
#include <stdio.h>
#include "file.h"

extern int _wrt_ok(FILE *_fp);
extern int _doflush(FILE *_fp);






/*****************************************************************************/
/* PUTC  -  Write a character to a stream                                    */
/*                                                                           */
/*    This function is equivalent to FPUTC.                                  */
/*                                                                           */
/*****************************************************************************/
_CODE_ACCESS int putc(int _x, FILE *_fp) { return(fputc(_x, _fp)); }






/*****************************************************************************/
/* PUTCHAR  -  Write a character to stdout                                   */
/*                                                                           */
/*    This function calls a macro defined in STDIO.H, which in turn calls    */
/*    FPUTC, with stdout as its stream argument.                             */
/*                                                                           */
/*****************************************************************************/
_CODE_ACCESS int putchar(int _x) { return(_putchar(_x)); }






/*****************************************************************************/
/* FPUTC -  Write a character to a stream                                    */
/*                                                                           */
/*    This function writes character _C into the stream specified by _FP.    */
/*    Upon success, it returns the character written.  Upon failure, it      */
/*    returns an EOF.                                                        */
/*                                                                           */
/*****************************************************************************/
_CODE_ACCESS int fputc(int _c, register FILE *_fp)
{
   /*------------------------------------------------------------------------*/
   /* Make sure that the stream is writeable.                                */
   /*------------------------------------------------------------------------*/
   if(! _wrt_ok(_fp)) return (EOF);

   /*------------------------------------------------------------------------*/
   /* If the stream is non-buffered, call the lowlevel WRITE function.       */
   /*------------------------------------------------------------------------*/
   if(_BUFFMODE(_fp) == _IONBF)
   {
      char cbuf = (char)_c;

      if((write(_fp->fd, &cbuf, 1)) == -1)
      {
         _SET(_fp, _STATERR);
         return (EOF);
      }
      else return ((unsigned char)_c);
   }

   /*------------------------------------------------------------------------*/
   /* Put the character in the buffer.                                       */
   /*------------------------------------------------------------------------*/
   *(_fp->pos++) = (unsigned char)_c;

   /*------------------------------------------------------------------------*/
   /* If the buffer is full, or a line-buffered stream reached a newline     */
   /* character, flush it.                                                   */
   /*------------------------------------------------------------------------*/
   if((_fp->pos == _fp->bufend) || (_STCHK(_fp, _IOLBF) && _c == '\n'))
      if(_doflush(_fp))
      {
         _SET(_fp, _STATERR);
         return (EOF);
      }

   return((unsigned char)_c);
}


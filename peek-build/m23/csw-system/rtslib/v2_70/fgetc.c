/*****************************************************************************/
/*  FGETC.C v2.54                                                            */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                   */
/*****************************************************************************/

/*****************************************************************************/
/* Functions:                                                                */
/*    GETC     -  Get a character from a stream                              */
/*    GETCHAR  -  Get a character from stdin                                 */
/*    FGETC    -  Get a character from a stream                              */
/*****************************************************************************/
#include <stdio.h>
#include "file.h"

extern void _buff_read(FILE *_fp);
extern int  _rd_ok    (FILE *_fp);

extern _CODE_ACCESS int fgetc(register FILE *_fp);






/*****************************************************************************/
/* GETC  -  Get a character from a stream                                    */
/*                                                                           */
/*    This function is equivalent to FGETC.                                  */
/*                                                                           */
/*****************************************************************************/
_CODE_ACCESS int getc(FILE *_fp) { return (fgetc(_fp)); }






/*****************************************************************************/
/* GETCHAR  -  Get a character from stdin                                    */
/*                                                                           */
/*    This function calls a macro defined in STDIO.H, which calls FGETC with */
/*    stdin as its argument.                                                 */
/*                                                                           */
/*****************************************************************************/
_CODE_ACCESS int getchar(void) { return(_getchar()); }






/*****************************************************************************/
/* FGETC -  Get a character from a stream                                    */
/*****************************************************************************/
_CODE_ACCESS int fgetc(register FILE *_fp)
{
   /*------------------------------------------------------------------------*/
   /* Make sure that it is OK to read from this stream.                      */
   /*------------------------------------------------------------------------*/
   if(! _rd_ok(_fp)) return (EOF);
   
   /*------------------------------------------------------------------------*/
   /* For non-buffered streams, call the lowlevel READ function.             */
   /*------------------------------------------------------------------------*/
   if(_BUFFMODE(_fp) == _IONBF)
   {
      int   errchk;
      char  result;

      errchk = read(_fp->fd, &result, 1);
      if(errchk <= 0)
      {
         _SET(_fp, (errchk == 0) ? _STATEOF : _STATERR);
         return (EOF);
      }

      return ((int)result);
   }

   /*------------------------------------------------------------------------*/
   /* If the buffer has been entirely read, or is empty, call _BUFF_READ to  */
   /* fill the buffer.                                                       */
   /*------------------------------------------------------------------------*/
   if(_fp->pos == _fp->buff_stop || !_fp->buff_stop) _buff_read(_fp);
 
   /*------------------------------------------------------------------------*/
   /* If the buffer read was unsuccessful, return an EOF.  Otherwise, clear  */
   /* the _UNGETC flag in the stream, and return the next character.         */
   /*------------------------------------------------------------------------*/
   if(_STCHK(_fp, (_STATERR | _STATEOF))) return (EOF);
   _UNSET(_fp, _UNGETC);

   return ((int)*(_fp->pos++)); 
   
}


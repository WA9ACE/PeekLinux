/*****************************************************************************/
/*  FGETS.C v2.54                                                            */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                   */
/*****************************************************************************/

/*****************************************************************************/
/* Functions:                                                                */
/*    FGETS    -  Get a string from a stream                                 */
/*    GETS     -  Get a string from stdin                                    */
/*    _DOFGETS -  Perform the main FGETS routine                             */
/*****************************************************************************/
#include <string.h>
#include <stdio.h>
#include "file.h"

extern int  _rd_ok(FILE *_fp);
extern void _buff_read(FILE *_fp);

static char *_dofgets(char *_ptr, register int _size, register FILE *_fp,
                      int _gets_flag);






/*****************************************************************************/
/* FGETS -  Get a string from a stream                                       */
/*                                                                           */
/*    This function calls _DOFGETS, with a 0 for the _GETS_FLAG argument.    */
/*                                                                           */
/*****************************************************************************/
_CODE_ACCESS char *fgets(char *_ptr, register int _size, register FILE *_fp)
{
   return(_dofgets(_ptr, _size, _fp, 0));
}






/*****************************************************************************/
/* GETS  -  Get a string from stdin                                          */
/*                                                                           */
/*    This function calls FGETS with stdin as the stream argument.           */
/*                                                                           */
/*****************************************************************************/
_CODE_ACCESS char *gets(char *_ptr) { return (_dofgets(_ptr, 1, stdin, 1)); }






/*****************************************************************************/
/* _DOFGETS -  Perform the main FGETS routine                                */
/*                                                                           */
/*    This function gets a string from the stream _FP.  It returns a pointer */
/*    to the string upon success, or NULL upon failure.                      */
/*                                                                           */
/*****************************************************************************/
static char *_dofgets(char *_ptr, register int _size, register FILE *_fp, 
                      int _gets_flag)
{
   /*------------------------------------------------------------------------*/
   /* Local variables                                                        */
   /*------------------------------------------------------------------------*/
   unsigned    char     *next_nl    = NULL;
               char     *fpos       = _ptr;
               size_t   num_left    = _size -1,
                        num_to_read;
               int      skipped_nl  = 0;

   /*------------------------------------------------------------------------*/
   /* Make sure that the stream is readable.                                 */
   /*------------------------------------------------------------------------*/
   if(! _rd_ok(_fp) || _STCHK(_fp, _STATEOF) || (_size -1) < 0) return (NULL);

   /*------------------------------------------------------------------------*/
   /* If the stream is non buffered, read the string in one character at a   */
   /* time, using the lowlevel READ function.                                */
   /*------------------------------------------------------------------------*/
   if(_BUFFMODE(_fp) == _IONBF)
   {
      int errchk;
 
      while((errchk = read(_fp->fd, fpos, 1) > 0) && (*(fpos++) != '\n'));
 
      switch(errchk)
      {
         case  0 :
         case -1 : return (NULL);
      }
 
      *fpos = '\0';
 
      return (_ptr);
   }
 
   while((num_left > 0 || _gets_flag) && ! next_nl)
   {
      /*---------------------------------------------------------------------*/
      /* If the buffer is empty or completely read, fill it up.  If the read */
      /* is unsuccessful, return a NULL                                      */
      /*---------------------------------------------------------------------*/
      if(_fp->pos == _fp->buff_stop || !_fp->buff_stop) _buff_read(_fp);
      if(_STCHK(_fp, (_STATERR | _STATEOF))) return (NULL);

      /*---------------------------------------------------------------------*/
      /* Determine the location of the next newline character (NEXT_NL).     */
      /*---------------------------------------------------------------------*/
      next_nl = (unsigned char *)memchr(_fp->pos, '\n', (_fp->buff_stop - 
                                        _fp->pos));

      /*---------------------------------------------------------------------*/
      /* Determine how many characters to read from the buffer.  If this     */
      /* function was called from GETS (_GETS_FLAG == 1), read until a       */
      /* newline character, EOF character, or the end of the buffer is       */
      /* encountered.  Do the same otherwise, but do not read any more       */
      /* characters than specified by NUM_LEFT.                              */
      /*---------------------------------------------------------------------*/
      num_to_read =  _fp->buff_stop - _fp->pos;
      num_to_read =  (next_nl) ? (next_nl - _fp->pos + 1) : num_to_read;
      num_to_read =  (_gets_flag) ? num_to_read :
                     (num_left < num_to_read) ? num_left : num_to_read;

      /*---------------------------------------------------------------------*/
      /* If this function was called from GETS (_GETS_FLAG == 1), do not     */
      /* retain a newline character if one is present                        */
      /*---------------------------------------------------------------------*/
      if(next_nl && _gets_flag)
      {
         num_to_read--;
         skipped_nl = 1;
      }

      /*---------------------------------------------------------------------*/
      /* Copy the characters, and update pointers and counters.              */
      /*---------------------------------------------------------------------*/
      memcpy(fpos, _fp->pos, num_to_read);
      _fp->pos += (skipped_nl + num_to_read);
      num_left -= num_to_read;
      fpos += num_to_read;
   }

   /*------------------------------------------------------------------------*/
   /* Tack on the NULL terminator, clear the _UNGETC flag and the _GETS_FLAG.*/
   /*------------------------------------------------------------------------*/
   *fpos = '\0';
   _UNSET(_fp, _UNGETC);
   _gets_flag = 0; 

   return(_ptr);
}


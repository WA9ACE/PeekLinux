/*****************************************************************************/
/*  UNGETC.C v2.54                                                           */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                   */
/*****************************************************************************/

/*****************************************************************************/
/* Functions:                                                                */
/*    UNGETC   -  Push a character back onto a stream                        */
/*****************************************************************************/
#include <stdio.h>


/*****************************************************************************/
/* UNGETC   -  Push a character back onto a stream                           */
/*                                                                           */
/*    This function returns the character pushed back upon success, or an    */
/*    EOF upon failure.                                                      */
/*                                                                           */
/*****************************************************************************/
_CODE_ACCESS int ungetc(int _c, register FILE *_fp)
{
   /*------------------------------------------------------------------------*/
   /* If the current stream is not associated with a file, return an error.  */
   /*------------------------------------------------------------------------*/
   if(_fp->fd == -1) return (EOF);

   /*------------------------------------------------------------------------*/
   /* If the character is an EOF, or if an UNGETC has already been performed */
   /* and there is no room left in the buffer, return an EOF.                */
   /*------------------------------------------------------------------------*/
   if(_c == EOF || (_fp->pos < _fp->buf)) return (EOF);

   /*------------------------------------------------------------------------*/
   /* If the file is in update mode, and is currently writing, change it to  */
   /* read mode.                                                             */
   /*------------------------------------------------------------------------*/
   if(_STCHK(_fp, _MODERW))
   {
      _UNSET(_fp, _MODEW);
      _SET(_fp, _MODER);
   }
 
   /*------------------------------------------------------------------------*/
   /* Set up a buffer if one is needed, and none exists.                     */
   /*------------------------------------------------------------------------*/
   if(_fp->buf == NULL && !_STCHK(_fp, _IONBF))
      if(setvbuf(_fp, NULL, _BUFFMODE(_fp), BUFSIZ) == EOF) return (EOF);
 
   /*------------------------------------------------------------------------*/
   /* Put the character in the buffer, set the _UNGETC flag in the stream,   */
   /* and clear the EOF flag in the stream.                                  */
   /*------------------------------------------------------------------------*/
   *(--_fp->pos) = (unsigned char)_c;
   _SET(_fp, _UNGETC);
   _UNSET(_fp, _STATEOF);

   /*------------------------------------------------------------------------*/
   /* If this is an UNGETC on an empty file, make the end of the read buffer.*/
   /*------------------------------------------------------------------------*/
   if(!_fp->buff_stop) _fp->buff_stop = _fp->buf;

   return ((unsigned char)_c);
}


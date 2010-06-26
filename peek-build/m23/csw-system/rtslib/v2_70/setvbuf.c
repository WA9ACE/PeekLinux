/*****************************************************************************/
/*  SETVBUF.C  v2.54                                                         */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                   */
/*****************************************************************************/

/*****************************************************************************/
/* Functions:                                                                */
/*    SETVBUF  -  Set the buffering mode for a stream                        */
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>

extern void   _cleanup();
extern _DATA_ACCESS void (*_cleanup_ptr)(void);


/*****************************************************************************/
/* SETVBUF  -  Set the buffering mode for a stream                           */
/*                                                                           */
/*    This function sets the buffering mode for a stream, and allocates a    */
/*    buffer for it if necessary.                                            */
/*                                                                           */
/*****************************************************************************/
_CODE_ACCESS int setvbuf(register FILE *_fp, register char *_buf,
			 register int _type, register size_t _size)
{
   /*------------------------------------------------------------------------*/
   /* If the current stream is not associated with a file, return an error.  */
   /*------------------------------------------------------------------------*/
   if(_fp->fd == -1 || (_type != _IONBF && _size <= 0)) return (EOF);

   /*------------------------------------------------------------------------*/
   /* If a buffer already exists, free it if it was malloc'd, and reset all  */
   /* of the stream's buffer pointers.                                       */
   /*------------------------------------------------------------------------*/
   if(_fp->buf)
   {
      if(_STCHK(_fp, _BUFFALOC)) free((_fp->buf)-1);
      _UNSET(_fp, _BUFFALOC);
      _fp->buf = NULL;
      _fp->pos = NULL;
      _fp->bufend = NULL;
      _fp->buff_stop = NULL;
   }

   /*------------------------------------------------------------------------*/
   /* If NULL was used for the buffering mode, default to fully-buffered.    */
   /*------------------------------------------------------------------------*/
   if(! _type) _type = _IOFBF;

   /*------------------------------------------------------------------------*/
   /* Clear any previous buffering flags, and set the new one.               */
   /*------------------------------------------------------------------------*/
   _UNSET(_fp, (_IOLBF | _IOFBF | _IONBF));
   _SET(_fp, _type);

   /*------------------------------------------------------------------------*/
   /* If a buffer was provided, but its size is only one byte, allocate a    */
   /* different one.  Also, do not allow a buffer size greater than BUFSIZ.  */
   /* The buffer will always have one space at the beginning that is         */
   /* for UNGETC, in the event that an UNGETC is performed on an empty file, */
   /* or when the buffer is full, but unread.                                */
  /*------------------------------------------------------------------------*/
   if(_size == 1) _buf = NULL;
   if(_size > BUFSIZ-1) _size = BUFSIZ -1;
   _size++;

   if (_buf) _fp->pos = _fp->buf = (unsigned char*)_buf+1;
   else
   {
      if (! (_fp->pos = _fp->buf = (unsigned char*)malloc(_size))) return (EOF);
      _fp->pos++;
      _fp->buf++;
      _SET(_fp, _BUFFALOC);
   }

   _fp->bufend = _fp->buf + _size -1;

   /*------------------------------------------------------------------------*/
   /* SETUP _CLEANUP_PTR SO ALL BUFFERS WILL BE FLUSHED AT EXIT.             */
   /*------------------------------------------------------------------------*/
   _cleanup_ptr = _cleanup;

   return (0);
}


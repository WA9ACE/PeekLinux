/*****************************************************************************/
/*  _IO_PERM.C v2.54                                                         */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                   */
/*****************************************************************************/

/*****************************************************************************/
/* Functions:                                                                */
/*    _RD_OK   -  Check to make sure that the stream is readable             */
/*    _WRT_OK  -  Check to make sure that the stream is writeable            */
/*****************************************************************************/
#include <stdio.h>

extern int _rd_ok(FILE *_fp);
extern int _wrt_ok(FILE *_fp);


/*****************************************************************************/
/* _RD_OK   -  Check to make sure that the stream is readable.               */
/*                                                                           */
/*    This function checks to make sure that the stream _FP has been opened  */
/*    for reading, and allocates a buffer for the stream if one hasn't been  */
/*    already.  The function returns a 1 if it is alright to write to this   */
/*    stream, otherwise it returns a 0.                                      */
/*                                                                           */
/*****************************************************************************/
int _rd_ok(FILE *_fp)
{
   /*------------------------------------------------------------------------*/
   /* Local variables                                                        */
   /*------------------------------------------------------------------------*/
   int   result   =  0;

   /*------------------------------------------------------------------------*/
   /* If the current stream is not associated with a file, return an error.  */
   /*------------------------------------------------------------------------*/
   if(_fp->fd == -1) return (0);

   /*------------------------------------------------------------------------*/
   /* If this stream is currently being written to, return a 0.              */
   /*------------------------------------------------------------------------*/
   if(_STCHK(_fp, _MODEW)) return (0);
 
   /*------------------------------------------------------------------------*/
   /* If this stream was opened in update mode, and is able to either read   */
   /* or write currently, put it in read mode.                               */
   /*------------------------------------------------------------------------*/
   if(!_STCHK(_fp, _MODER) && _STCHK(_fp, _MODERW)) _SET(_fp, _MODER);
 
   /*------------------------------------------------------------------------*/
   /* Allocate a buffer for the stream if needed, and none exist.            */
   /*------------------------------------------------------------------------*/
   if(_fp->buf == NULL && !_STCHK(_fp, _IONBF))
      result |= setvbuf(_fp, NULL, _BUFFMODE(_fp), BUFSIZ);
 
   return (! result);
 
}


/*****************************************************************************/
/* _WRT_OK()   -  Check to make sure that a stream is writeable              */
/*                                                                           */
/*    This function takes the FILE pointer _FP, and makes sure that it is ok */
/*    to write to it.  It also allocates a buffer if one is needed.  The     */
/*    function returns a 1 if it is ok to write to this stream, otherwise it */
/*    returns a 0.                                                           */
/*                                                                           */
/*****************************************************************************/
int _wrt_ok(FILE *_fp)
{
   /*------------------------------------------------------------------------*/
   /* Local variables                                                        */
   /*------------------------------------------------------------------------*/
   int   result   = 0;
 
   /*------------------------------------------------------------------------*/
   /* If the current stream is not associated with a file, return an error.  */
   /*------------------------------------------------------------------------*/
   if(_fp->fd == -1) return (0);

   /*------------------------------------------------------------------------*/
   /* If this stream is currently being read return a 0.                     */
   /*------------------------------------------------------------------------*/
   if (_STCHK(_fp, _MODER)) return (0);
 
   /*------------------------------------------------------------------------*/
   /* If the stream is in append mode, move the file pointer to the end of   */
   /* the file.                                                              */
   /*------------------------------------------------------------------------*/
   if(_STCHK(_fp, _MODEA)) fseek(_fp, 0L, SEEK_END);
 
   /*------------------------------------------------------------------------*/
   /* If this stream was opened in update mode, and is able to either read   */
   /* or write currently, put it in write mode.                              */
   /*------------------------------------------------------------------------*/
   if(!_STCHK(_fp, _MODEW) && _STCHK(_fp, _MODERW)) _SET(_fp, _MODEW);
 
   /*------------------------------------------------------------------------*/
   /* Allocate a buffer for the stream if needed, and none exist.            */
   /*------------------------------------------------------------------------*/
   if(_fp->buf == NULL && !_STCHK(_fp, _IONBF))
      result |= setvbuf(_fp, NULL, _BUFFMODE(_fp), BUFSIZ);
 
   return (! result);
}


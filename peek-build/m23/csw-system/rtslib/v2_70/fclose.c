/*****************************************************************************/
/*  FCLOSE.C v2.54                                                           */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                   */
/*****************************************************************************/

/*****************************************************************************/
/*    Functions:                                                             */
/*       FCLOSE   -  Close a stream                                          */
/*****************************************************************************/
#include <stdio.h>
#include "file.h"
#include <string.h>
#include <stdlib.h>

extern int _doflush(FILE *_fp);


/*****************************************************************************/
/* FCLOSE   -  Close a stream                                                */
/*                                                                           */
/*    This function flushes the current stream, deallocates any buffers      */
/*    associated with the stream, and closes it.  This function returns a 0  */
/*    if the close was successful, or an EOF if an error occured.            */
/*                                                                           */
/*****************************************************************************/
_CODE_ACCESS int fclose(FILE *_fp)
{
   /*------------------------------------------------------------------------*/
   /* Local variables                                                        */
   /*------------------------------------------------------------------------*/
   int fd = _fp->fd;
   int errchk;
   int remove_flag = _STCHK(_fp, _TMPFILE);

   /*------------------------------------------------------------------------*/
   /* If the current stream is not associated with a file, return an error.  */
   /*------------------------------------------------------------------------*/
   if(_fp->fd == -1) return (EOF);

   /*------------------------------------------------------------------------*/
   /* Flush the buffer.                                                      */
   /*------------------------------------------------------------------------*/
   if(_doflush(_fp)) return (EOF);

   /*------------------------------------------------------------------------*/
   /* Free the buffer if there was one, and it was malloc'd by SETVBUF.      */
   /*------------------------------------------------------------------------*/
   if(_STCHK(_fp, _BUFFALOC))
   {
      free((_fp->buf)-1);
      _UNSET(_fp, _BUFFALOC);
   }

   /*------------------------------------------------------------------------*/
   /* Reset the buffer pointers in the stream.                               */
   /*------------------------------------------------------------------------*/
   _fp->buf = NULL;
   _fp->pos = NULL;
   _fp->bufend = NULL;

   /*------------------------------------------------------------------------*/
   /* Close the file, and give the FILE structure an invalid file descriptor */
   /* so it cannot be used until it is assigned to another stream.           */
   /*------------------------------------------------------------------------*/
   errchk = close(_fp->fd);
   _fp->fd = -1;

   /*------------------------------------------------------------------------*/
   /* If the FILE pointer was associated with a temporary file, look up that */
   /* temporary file's filename, and remove it.                              */
   /*------------------------------------------------------------------------*/
   if(remove_flag) remove(_tmpnams[fd]);  

   return (errchk);
}


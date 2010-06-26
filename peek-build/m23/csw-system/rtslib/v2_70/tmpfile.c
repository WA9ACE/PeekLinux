/*****************************************************************************/
/*  TMPFILE.C v2.54                                                          */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                   */
/*****************************************************************************/

/*****************************************************************************/
/* Functions:                                                                */
/*    TMPFILE  -  Create a temporary file, and return a pointer to the       */
/*                stream                                                     */
/*****************************************************************************/
#include <stdio.h>
#include "file.h"
#include <string.h>


/*****************************************************************************/
/* TMPFILE  -  Create a temporary file, and return a pointer to its FILE     */
/*             structure                                                     */
/*****************************************************************************/
_CODE_ACCESS FILE *tmpfile(void)
{
   char tfname[L_tmpnam];
   FILE *_fp = (tmpnam(tfname)) ? (fopen(tfname, "wb+")) : NULL;

   if(_fp)
   {
      _SET(_fp, _TMPFILE);
      strcpy(_tmpnams[_fp->fd], tfname);
   }

   return (_fp);
}


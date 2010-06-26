/*****************************************************************************/
/*  FGETPOS.C v2.54                                                          */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                   */
/*****************************************************************************/

/*****************************************************************************/
/*    This function contains the function FGETPOS, which stores the current  */
/*    position in the file into the variable pointed to by _POS.  A 0 is     */
/*    returned upon success, and upon failure, ERRNO is set to 5, and a -1L  */
/*    is returned.                                                           */
/*****************************************************************************/
#include <stdio.h>
#include <errno.h>
#include "file.h"

_CODE_ACCESS int fgetpos(FILE *_fp, fpos_t *_pos)
{
   if((*_pos = ftell(_fp)) != -1L) return (0);
   else { errno = 5; return (-1);  }
}


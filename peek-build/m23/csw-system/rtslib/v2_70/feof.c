/*****************************************************************************/
/*  FEOF.C v2.54                                                             */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                   */
/*****************************************************************************/

/*****************************************************************************/
/*    This file contains the function FEOF, which returns a 1 if an EOF      */
/*    has occured on that stream.  The function merely calls a macro         */
/*    that is defined in STDIO.H.                                            */
/*****************************************************************************/
#include <stdio.h>

_CODE_ACCESS int feof(FILE *_fp) { return (((_fp)->flags & _STATEOF)); }


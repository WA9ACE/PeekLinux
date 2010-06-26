/*****************************************************************************/
/*  FERROR.C v2.54                                                           */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                   */
/*****************************************************************************/

/*****************************************************************************/
/*    This file contains the function FERROR, which returns a 1 if an I/O    */
/*    error has occured on that stream.  The function merely calls a macro   */
/*    that is defined in STDIO.H.                                            */
/*****************************************************************************/
#include <stdio.h>

_CODE_ACCESS int ferror(FILE *_fp) { return (_STCHK(_fp, _STATERR)); }


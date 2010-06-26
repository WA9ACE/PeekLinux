/*****************************************************************************/
/*  CLEARERR.C v2.54                                                         */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                   */
/*****************************************************************************/
#include <stdio.h>


/*****************************************************************************/
/* CLEARERR -  Clear the I/O error flag (_STATERR)                           */
/*                                                                           */
/*    This function calls a macro defined in stdio.h                         */
/*                                                                           */
/*****************************************************************************/
_CODE_ACCESS void clearerr(FILE *_fp) { _clearerr(_fp); }


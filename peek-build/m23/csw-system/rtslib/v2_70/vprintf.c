/*****************************************************************************/
/*  VPRINTF.C v2.54                                                          */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                   */
/*****************************************************************************/

/*****************************************************************************/
/* Functions:                                                                */
/*    VPRINTF  -  Print formatted output to stdio                            */
/*    _OUTC    -  Put a character in a stream                                */
/*    _OUTS    -  Put a string in a stream                                   */
/*****************************************************************************/
#include <stdio.h>
#include "format.h"
#include <stdarg.h>
#include <string.h>
 
extern int _printfi(char **_format, va_list _ap, void *_op,
                    int (*_outc)(char, void *), int (*_outs)(char *, void *));
 
static int _outc(char c, void *_op);
static int _outs(char *s, void *_op);
 

/*****************************************************************************/
/* VPRINTF  -  Print formatted output to a stdio                             */
/*                                                                           */
/*    This function passes a the format string and an argument list to       */
/*    _PRINTFI, and writes the result string to the stream stdio.            */
/*                                                                           */
/*****************************************************************************/
_CODE_ACCESS int vprintf(const char *_format, va_list _ap)
{
   char *fptr = (char *)_format;

   /*------------------------------------------------------------------------*/
   /* If the current stream is not associated with a file, return an error.  */
   /*------------------------------------------------------------------------*/
   if(stdout->fd == -1) return (EOF);

   return (_printfi(&fptr, _ap, (void *)stdout, _outc, _outs));
 
}
 

/*****************************************************************************/
/* _OUTC -  Put a character in a stream                                      */
/*****************************************************************************/
static int _outc(char c, void *_op) { return (fputc(c, (FILE *)_op)); }
 

/*****************************************************************************/
/* _OUTS -  Put a string in a stream                                         */
/*****************************************************************************/
static int _outs(char *s, void *_op) { return (fputs(s, (FILE *)_op)); }


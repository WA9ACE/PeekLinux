/*****************************************************************************/
/*  VSPRINTF.C v2.54                                                         */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                   */
/*****************************************************************************/

/*****************************************************************************/
/* Functions:                                                                */
/*    VSPRINTF -  Copy formatted output to a string                          */
/*    _OUTC    -  Put a character in a string                                */
/*    _OUTS    -  Append a string to another string                          */
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
/* VSPRINTF -  Copy formatted output to a string                             */
/*                                                                           */
/*    This function passes a format string and an argument list to           */
/*    _PRINTFI, and writes the result string to the string _STRING.          */
/*                                                                           */
/*****************************************************************************/
_CODE_ACCESS int vsprintf(char *_string, const char *_format, va_list _ap)
{
    int   rval;
    char *fptr = (char *)_format;
    char *out_end = _string;

    rval = _printfi(&fptr, _ap, (void *)&out_end, _outc, _outs);

    *out_end = '\0';

    return rval;
}


/*****************************************************************************/
/* _OUTC -  Put a character in a string                                      */
/*****************************************************************************/
static int _outc(char c, void *_op)
{
    return *(*((char **)_op))++ = c;
}
 

/*****************************************************************************/
/* _OUTS -  Append a string to another string                                */
/*****************************************************************************/
static int _outs(char *s, void *_op)
{
    size_t len = strlen(s);
    
    memcpy(*((char **)_op), s, len);
    *((char **)_op) += len;
    return len;
}
 

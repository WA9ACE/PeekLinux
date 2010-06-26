/*****************************************************************************/
/*  VSNPRINTF.C v2.54                                                        */
/*  Copyright (c) 2001-2004 Texas Instruments Incorporated                   */
/*****************************************************************************/

/*****************************************************************************/
/* Functions:                                                                */
/*    VSNPRINTF -  Copy formatted output to a string                         */
/*    _OUTC     -  Put a character in a string                               */
/*    _OUTS     -  Append a string to another string                         */
/*****************************************************************************/
#include <stdio.h>
#include "format.h"
#include <stdarg.h>
#include <string.h>

extern int _printfi(char **_format, va_list _ap, void *_op,
                    int (*_outc)(char, void *), int (*_outs)(char *, void *));

static int _outc(char c, void *_op);
static int _outs(char *s, void *_op);

struct holder {
    char  *out_end;
    size_t n;
    size_t written;
};


/*****************************************************************************/
/* VSNPRINTF -  Copy formatted output to a string                            */
/*                                                                           */
/*    This function passes a format string and an argument list to           */
/*    _PRINTFI, and writes the result string to the string _STRING.          */
/*                                                                           */
/*****************************************************************************/
_CODE_ACCESS int vsnprintf(char *_string, size_t _n,
			   const char *_format, va_list _ap)
{
    int    rval;
    char  *fptr = (char *)_format;
    struct holder holder;

    holder.out_end = _string;
    holder.n       = _n;
    holder.written = 0;

    rval = _printfi(&fptr, _ap, (void *)&holder, _outc, _outs);

    if (_n) *holder.out_end = '\0';

    return rval;
}


/*****************************************************************************/
/* _OUTC -  Put a character in a string                                      */
/*****************************************************************************/
static int _outc(char c, void *_op)
{
    struct holder *holder = (struct holder *)_op;

    if (holder->written < (holder->n-1))
	*holder->out_end++ = c;

    holder->written++;

    return c;
}


/*****************************************************************************/
/* _OUTS -  Append a string to another string                                */
/*****************************************************************************/
static int _outs(char *s, void *_op)
{
    struct holder *holder = (struct holder *)_op;
    size_t len = strlen(s);

    if (holder->written < holder->n-1)
    {
	size_t space = (holder->n-1) - holder->written;//Don't forget the null char termination
	size_t use = len > space ? space : len;
	memcpy(holder->out_end, s, use);
	holder->out_end += use;
    }

    holder->written += len;

    return len;
}




/*****************************************************************************/
/*  FSCANF.C v2.54                                                           */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                   */
/*****************************************************************************/

/*****************************************************************************/
/* Functions:                                                                */
/*    FSCANF      -  Read formatted input from a stream                      */
/*    SCANF       -  Read formatted input from stdin                         */
/*    _INPCHAR    -  Get a character from the stream                         */
/*    _UNINPCHAR  -  Put a character back onto the stream                    */
/*    _CHKMBC     -  Check that the characters match the input               */
/*****************************************************************************/
#include <stdio.h>
#include "format.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

static int _inpchar(void **inp);
static void _uninpchar(void **inp, char outchar);
static int _chkmbc(void **inp, char **_format, int *num_read);

/*****************************************************************************/
/* FSCANF   -  Read formatted input from a stream                            */
/*                                                                           */
/*    This function returns the number of arguments that were assigned a     */
/*    value.                                                                 */
/*                                                                           */ 
/*****************************************************************************/
_CODE_ACCESS int fscanf(FILE *_fp, const char *_fmt, ...)
{
#ifdef ENABLE_SCANF
   va_list _ap;
   va_start(_ap, _fmt);
 
   /*------------------------------------------------------------------------*/
   /* If the current stream is not associated with a file, return an error.  */
   /*------------------------------------------------------------------------*/
   if (_fp->fd == -1) return (EOF);

   return (_scanfi((void *)_fp, _fmt, _ap, _chkmbc, _inpchar, _uninpchar));
#else
return 0;
#endif
}

/*****************************************************************************/
/* SCANF -  Read formatted input from stdin                                  */
/*                                                                           */ 
/*    This function returns the number of arguments that were assigned a     */
/*    value.                                                                 */
/*                                                                           */ 
/*****************************************************************************/ 
int scanf(const char *_fmt, ...)
{
#ifdef ENABLE_SCANF
   va_list _ap;
   va_start(_ap, _fmt);
 
   /*------------------------------------------------------------------------*/
   /* If the current stream is not associated with a file, return an error.  */
   /*------------------------------------------------------------------------*/
   if (stdin->fd == -1) return (EOF);

   return (_scanfi((void *)stdin, _fmt, _ap, _chkmbc, _inpchar, _uninpchar));
   #else
	return 0;
   #endif
 
}

/*****************************************************************************/
/* _INPCHAR -  Get a character from the stream                               */
/*****************************************************************************/
static int _inpchar(void **inp) { return (fgetc((FILE *)(*inp))); }

/*****************************************************************************/
/* _UNINPCHAR  -  Put a character back onto the stream                       */
/*****************************************************************************/
static void _uninpchar(void **inp, char outchar)
{ 
   ungetc(outchar, (FILE *)*inp);
}

/*****************************************************************************/
/* _CHKMBC  -  Check that the characters match the input                     */
/*                                                                           */
/*    Check that all characters in the format string that are not part of a  */
/*    conversion specification match the input, until the next '%' or the    */
/*    end of the format string is reached.  The function returns an EOF if   */
/*    the end of the file is reached prematurely, a 0 upon reaching the end  */
/*    of the format string, or a 1 if a '%' is encountered.                  */
/*                                                                           */
/*****************************************************************************/
static int _chkmbc(void **inp, char **_format, int *num_read)
{
   FILE        *_fp = (FILE *)*inp;
   signed char  c;

   /*------------------------------------------------------------------------*/
   /* If there is a white space character in the format statement, skip to   */
   /* the next non-white space character in the input.                       */
   /*------------------------------------------------------------------------*/
   while (isspace(**_format))
   {
      for(c = fgetc(_fp);isspace(c);c = fgetc(_fp)) (*num_read)++;
      ungetc(c, _fp);
      (*_format)++;
   }

   for(;(**_format != '%') && (**_format != '\0');)
      if (*((*_format)++) != (c = fgetc(_fp))) 
      {
         ungetc(c, _fp);
         return ((c == EOF) ? EOF : 0);
      }
      else (*num_read)++;

   if (**_format == '%') return 1;
   else return (0);
}


/*****************************************************************************/
/*  SSCANF.C v2.54                                                           */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                   */
/*****************************************************************************/

/*****************************************************************************/
/* Functions:                                                                */
/*    SSCANF      -  Read formatted input from a string                      */
/*    _INPCHAR    -  Get a character from the string                         */
/*    _UNINPCHAR  -  Put a character back onto the string                    */
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
/* SSCANF   -  Read formatted input from a string                            */
/*                                                                           */
/*    This function returns the number of arguments that were assigned a     */
/*    value.                                                                 */
/*                                                                           */
/*****************************************************************************/
_CODE_ACCESS int sscanf(const char *_str, const char *_fmt, ...)
{
#ifdef ENABLE_SCANF
   va_list _ap;
   va_start(_ap, _fmt);

   return (_scanfi((void *)_str, _fmt, _ap, _chkmbc, _inpchar, _uninpchar));
#else
   return 0;
#endif
}

/*****************************************************************************/
/* _INPCHAR -  Get a character from the string                               */
/*****************************************************************************/
static int _inpchar(void **inp)
{
   char result;
   char *buf = (char *)(*inp);

   result = *(buf++);
   *inp = (void *)buf;  

   return ((result == '\0') ? EOF : result);
}

/*****************************************************************************/
/* _UNINPCHAR  -  Put a character back into the string                       */
/*****************************************************************************/
static void _uninpchar(void **inp, char outchar)
{
   char *buf = (char *)(*inp);
   buf--;
   *inp = (void *)buf;  
   return;
}

/*****************************************************************************/
/* _CHKMBC  -  Check that the characters match the input                     */
/*                                                                           */
/*    Check that all characters in the format string that are not part of a  */
/*    conversion specification match the input, until the next '%' or the    */
/*    end of the format string is reached.  The function returns a 0 upon    */
/*    reaching the end of the format string or a mismatch, or a 1 if a '%'   */
/*    is encountered.                                                        */
/*                                                                           */
/*****************************************************************************/
static int _chkmbc(void **inp, char **_format, int *num_read)
{
   char *buf = (char *)*inp;

   /*------------------------------------------------------------------------*/
   /* If there is a white space character in the format statement, skip to   */
   /* the next non-white space character in the input.                       */
   /*------------------------------------------------------------------------*/
   while(isspace(**_format))
   {
      for(;isspace(*buf); buf++) (*num_read)++;
      (*_format)++;
   }

   for(;(**_format != '%') && (**_format != '\0');)
      if(*((*_format)++) != *(buf++)) return (0);
      else (*num_read)++;

   if(**_format == '%')
   {
      *inp = (void *)buf;
       return 1;
   }
   else return (0);
}

/*****************************************************************************/
/*  _PRINTFI.C v2.54                                                         */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                   */
/*****************************************************************************/

/*****************************************************************************/
/*    This file contains the main routines that all six variations of the    */
/*    printf function use.  The main function in the file is _printfi,       */
/*    and the other functions here are called by it.                         */
/*                                                                           */
/* FUNCTIONS:                                                                */
/*    _printfi       -  Perform the main printf routine                      */
/*    _pproc_fflags  -  Process the format flags for a conversion            */
/*    _pproc_fwp     -  Convert field width and precision into numbers       */
/*    _pproc_str     -  Process the string (%s) conversion                   */
/*    _setfield      -  Performs conversions when the '%' is encountered     */
/*    _pproc_fge     -  Process the conversion for f, g, G, e, and E         */
/*    _pconv_f       -  Perform the %f conversion                            */
/*    _pconv_e       -  Perform the %e conversion                            */
/*    _pconv_g       -  Perform the %g conversion                            */
/*    _fcpy          -  Copy the fraction part of a float to a string        */
/*    _ecpy          -  Copy the "E+xx" part of a float to a string          */
/*    _mcpy          -  Copy the whole number part of a float to a string    */
/*    _pproc_diouxp  -  Process the conversion for d, i, o, u, x, and p      */
/*    _getarg_diouxp -  Get the argument for d, i, o, u, x, or p conversion  */
/*    _ltostr        -  Convert an integer to a string of up to base 16      */
/*    _div           -  Divide two integers                                  */
/*                                                                           */
/* Note:  If NOFLOAT is defined at compile time, this file will be compiled  */
/*        without floating point support.                                    */
/*****************************************************************************/

/*
 * 07/30/04 re-entrance of sprintf.
 * Change originally came from RTS1.22e modified on 02/27/01 re-entrance of sprintf.
 * _ARSIZE is the size of the fld array defined in local static in _setfied
 * in the original version of rts.src. Now this array is not static and is
 * defined in _printfi because the current stack may be corrupted if the fld
 * array is defined in _setfield. The size of the array is reduced to avoid
 * to use too much space in stack. The prototype of _setfield is modified to
 * use the fld array defined in _printfi.
 * The call to malloc in _pproc_str is removed and replaced with a local
 * array because malloc uses a global variable.
 */


#include <stdio.h>
#include <format.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <_lock.h>

#define SIGNED_CONV (pfield->conv != 'u' && pfield->conv != 'o' && \
                     pfield->conv != 'x' && pfield->conv != 'X')

extern _CODE_ACCESS char *fcvt(long double value, register int ndigit,
			       int *decpt, int *sign);
extern _CODE_ACCESS char *ecvt(long double value, register int ndigit,
			       int *decpt, int *sign);
extern _CODE_ACCESS char *memccpy(char *dest, const char *src, int ch,
				  int count);

extern int _printfi(char **_format, va_list _ap, void *_op,
                    int (*_outc)(char, void *), int (*_outs)(char *, void *));

static void _pproc_fflags(_PFIELD *pfield, char **it);
static void _pproc_fwp(_PFIELD *pfield, char **it, va_list *_ap);
static void _pproc_str(_PFIELD *pfield, void *_op, va_list *_ap, int *count,
                       int (*_outs)(char *, void *));
static void _setfield(char *fld, _PFIELD *pfield, va_list *_ap);
static void _pproc_fge(_PFIELD *pfield, int *minus_flag, char **a_it,
                       va_list *_ap);
static void _pconv_f(long double cvt, _PFIELD *pfield, char **a_it);
static void _pconv_e(long double cvt, _PFIELD *pfield, char **a_it);
static void _pconv_g(long double cvt, _PFIELD *pfield, char **a_it);
static char *_fcpy(const char *tmpbuf, int dpt, int trail, int precision,
                   char **a_it);
static char *_ecpy(int exp, char letter, char **a_it);
static char *_mcpy(const char *tmpbuf, int dpt, int putdec, char **a_it);
static int _pproc_diouxp(_PFIELD *pfield, int *minus_flag, char **a_it,
                         va_list *_ap);
static uintmax_t _getarg_diouxp(_PFIELD *pfield, va_list *_ap);
static int _ltostr(uintmax_t cvt, int base, char conv, char **a_it);
static uintmax_t _div(uintmax_t cvt, int base);





/*****************************************************************************/
/* _PRINTFI -  Perform the main printf routine                               */
/*                                                                           */
/*    This function processes the format string.  It copies the format       */
/*    string into the result string until a '%' is encountered, where any    */
/*    flags, the field width, the precision, and the type of conversion are  */
/*    read in, stored in a structure called PFIELD, and passed to _SETFIELD, */
/*    where the actual conversion is processed.  This function returns       */
/*    the number of characters output.                                       */
/*                                                                           */
/*****************************************************************************/
int _printfi(char **_format, va_list _ap, void *_op,
             int (*_outc)(char, void *), int (*_outs)(char *, void *))
{
   /*------------------------------------------------------------------------*/
   /* Local Variables                                                        */
   /*                                                                        */
   /*    *end     -  A pointer to the end of the format string               */
   /*    *pfield  -  A pointer to a structure _PFIELD, which stores all of   */
   /*                flags and parameters needed to perform a conversion.    */
   /*------------------------------------------------------------------------*/
   char     *end           =  *_format + strlen(*_format);
   int      count          =  0;
   _PFIELD  pfield;
   char     fld[_ARSIZE];

   /*------------------------------------------------------------------------*/
   /* Iterate through the format string until the end of it is reached.      */
   /*------------------------------------------------------------------------*/
   while(*_format < end)
   {
      /*---------------------------------------------------------------------*/
      /* Initialize PFIELD.                                                  */
      /*---------------------------------------------------------------------*/
      pfield.flags     = 0;
      pfield.fwidth    = 0;
      pfield.precision = -1;
      pfield.conv      = 0;

      /*---------------------------------------------------------------------*/
      /* Copy the format string directly to the target string until a '%'    */
      /* is encountered.                                                     */
      /*---------------------------------------------------------------------*/
      for(; **_format != '%' && **_format != '\0';
          _outc(*((*_format)++), _op), count++);

      /*---------------------------------------------------------------------*/
      /* If the end of the format string has been reached, break out of the  */
      /* while loop.                                                         */
      /*---------------------------------------------------------------------*/
      if(! (**_format)) break;

      (*_format)++;                   /* Skip to the character after the '%' */

      /*---------------------------------------------------------------------*/
      /* Process the flags immediately after the '%'.                        */
      /*---------------------------------------------------------------------*/
      _pproc_fflags(&pfield, _format);

      /*---------------------------------------------------------------------*/
      /* Convert the field width and precision into numbers.                 */
      /*---------------------------------------------------------------------*/
      _pproc_fwp(&pfield, _format, &_ap);

      /*---------------------------------------------------------------------*/
      /* If the h, l, or L flag was specified, set the corresponding flag    */
      /* in pfield.                                                          */
      /*---------------------------------------------------------------------*/
      switch (**_format)
      {
	  case 'L': _SET(&pfield, _MFLD); (*_format)++; break;
	  case 'h': _SET(&pfield, _MFH);  (*_format)++; break;
	  case 'l':
	  {
	      (*_format)++;
	      if (**_format == 'l') { _SET(&pfield, _MFLL); (*_format)++; }
	      else _SET(&pfield, _MFL);
	  }
      }

      /*---------------------------------------------------------------------*/
      /* Set the conversion character in pfield.                             */
      /*---------------------------------------------------------------------*/
      pfield.conv = *((*_format)++);

      /*---------------------------------------------------------------------*/
      /* If 'n' is the conversion specifier, process it in this function,    */
      /* since it is the only one that makes no conversions.  It just stores */
      /* the number of characters printed so far into the next argument.     */
      /* Otherwise, call _SETFIELD which performs the conversion.            */
      /*---------------------------------------------------------------------*/
      if(pfield.conv == 'n')
         switch (pfield.flags & (_MFLL | _MFL | _MFH))
         {
#ifdef LLONG_MAX
                           /* The 'll' flag was specified */
            case _MFLL  :  *(va_arg(_ap, long long*)) = (long long)count;
                           break;
#endif
                           /* The 'l' flag was specified */
            case _MFL   :  *(va_arg(_ap, long*)) = (long)count;
                           break;

                           /* The 'h' flag was specified */
            case _MFH   :  *(va_arg(_ap, short*)) = (short)count;
                           break;

            default     :  *(va_arg(_ap, int*)) = (int)count;
                           break;

         }
      else if(pfield.conv == 's')
         _pproc_str(&pfield, _op, &_ap, &count, _outs);
      else
      {
         /*------------------------------------------------------------------*/
         /* Append the converted string to the result string, and reposition */
         /* its iterator, it2.                                               */
         /*------------------------------------------------------------------*/
	 /* Must hold the system lock to call _setfield() because it returns
	    a static buffer. */
	 _lock();
         _setfield(fld, &pfield, &_ap);
         count += _outs(fld, _op);
	 _unlock();
      }
   }

   return (count);
}


/*****************************************************************************/
/* _PPROC_FFLAGS   -  Process the format flags for a conversion              */
/*                                                                           */
/*    This function takes the flags directly after the '%' and stores them   */
/*    in the _PFIELD structure PFIELD for later reference.                   */
/*                                                                           */
/*****************************************************************************/
static void _pproc_fflags(_PFIELD *pfield, char **it)
{
   /*------------------------------------------------------------------------*/
   /* Local variables                                                        */
   /*------------------------------------------------------------------------*/
   int flags_done = 0;

   /*---------------------------------------------------------------------*/
   /* Read in all of the flags associated with this conversion, and set   */
   /* the corresponding flags in the PFIELD structure.                    */
   /*---------------------------------------------------------------------*/
   while(! flags_done)
      switch (**it)
      {
         case '-' :  _SET(pfield, _PFMINUS);
                     (*it)++;
                     break;

         case '+' :  _SET(pfield, _PFPLUS);
                     (*it)++;
                     break;

         case ' ' :  _SET(pfield, _PFSPACE);
                     (*it)++;
                     break;

         case '#' :  _SET(pfield, _PFPOUND);
                     (*it)++;
                     break;

         case '0' :  _SET(pfield, _PFZERO);
                     (*it)++;
                     break;

         default  :  flags_done = 1;
      }

   return;
}


/*****************************************************************************/
/* _PPROC_FWP   -  Convert the field width and precision from the format     */
/*                 string into numbers.                                      */
/*                                                                           */
/*    This function reads the field and precision out of the format string   */
/*    and converts them into numbers that will be stored in the _PFIELD      */
/*    structure pointed to by PFIELD.  They will be needed for future        */
/*    reference.                                                             */
/*                                                                           */
/*****************************************************************************/
static void _pproc_fwp(_PFIELD *pfield, char **it, va_list *_ap)
{
   char tmpstr[10];
   char *tmpptr;

   /*------------------------------------------------------------------------*/
   /* If a '*' was given for the field width, use the next argument as       */
   /* the field width for the conversion.  Otherwise, copy the following     */
   /* numerical characters into a temporary string, and convert that         */
   /* string into an integer, which will be used for the field width.        */
   /*------------------------------------------------------------------------*/
   if(**it == '*')
   {
      pfield->fwidth = va_arg(*_ap, int);

      /*---------------------------------------------------------------------*/
      /* If the argument given for the field width is negative, treat it as  */
      /* if the '-' flag was used, and the field width was positive.         */
      /*---------------------------------------------------------------------*/
      if(pfield->fwidth < 0)
      {
         pfield->fwidth = -(pfield->fwidth);
         _SET(pfield, _PFMINUS);
      }

      (*it)++;
   }
   else
   {
      /*---------------------------------------------------------------------*/
      /* Initialize the temporary string and iterator that will hold the     */
      /* field width temporarily.                                            */
      /*---------------------------------------------------------------------*/
      tmpptr = tmpstr;
      memset(tmpptr, '\0', 10);

      while((**it >= '0') && (**it <= '9')) *(tmpptr++) = *((*it)++);

      if(*tmpstr != '\0') pfield->fwidth = atoi(tmpstr);
   }

   /*------------------------------------------------------------------------*/
   /* If a '.' appears as the next character, process the following          */
   /* characters as a precision.                                             */
   /*------------------------------------------------------------------------*/
   if(**it == '.')
   {
      (*it)++;

      /*---------------------------------------------------------------------*/
      /* If a '*' was given for the precision, use the next argument as      */
      /* the precision for the conversion.  Otherwise, copy the following    */
      /* numerical characters into a temporary string, and convert that      */
      /* string into an integer, which will be used for the precision.       */
      /*---------------------------------------------------------------------*/
      if(**it == '*')
      {
         pfield->precision = va_arg(*_ap, int);
         (*it)++;
      }
      else
      {
         /*------------------------------------------------------------------*/
         /* Initialize the temporary string and iterator that will hold      */
         /* the field width temporarily.                                     */
         /*------------------------------------------------------------------*/
         tmpptr = tmpstr;
         memset(tmpptr, '\0', 10);

         while((**it >= '0') && (**it <= '9'))  *(tmpptr++) = *((*it)++);

         if(*tmpstr != '\0') pfield->precision = atoi(tmpstr);
         else                pfield->precision = 0;
      }
   }
   return;
}


/*****************************************************************************/
/* _PPROC_STR  -  Processes the string conversion (%s)                       */
/*                                                                           */
/*    This function places all or a portion of the input string into the     */
/*    the temporary string.  It returns a zero, unless the input string had  */
/*    a length of zero.  In this case, a one is returned.                    */
/*                                                                           */
/*****************************************************************************/
static void _pproc_str(_PFIELD *pfield, void *_op, va_list *_ap, int *count,
                       int (*_outs)(char *, void *))
{
    /*------------------------------------------------------------------------*/
    /* Local variables                                                        */
    /*------------------------------------------------------------------------*/
    char  *strbuf;
    char  tmpstr[80+1];
    int   len, buflen;
    int  strbuf_length , output_length, input_length, padding_length;

    /*------------------------------------------------------------------------*/
    /* Get the next argument.                                                 */
    /*------------------------------------------------------------------------*/
    strbuf = va_arg(*_ap, char*);
    strbuf_length = strlen(strbuf);
    /*------------------------------------------------------------------------*/
    /* Handle NULL strings.                                                   */
    /*------------------------------------------------------------------------*/
    if (strbuf == NULL)
    {
        _outs("(null)", _op);
        return;
    }
    input_length = (pfield->precision >= 0 && pfield->precision < strbuf_length) ?
                   pfield->precision : strbuf_length;
    output_length = (pfield->fwidth > input_length) ? pfield->fwidth : input_length;
    padding_length = output_length - input_length;
    *(tmpstr+80) = '\0';

    *count += output_length;

    while (padding_length > 0 && !_STCHK(pfield, _PFMINUS))
    {
        if (padding_length > 80)
        {
            buflen = 80;
        }
        else
        {
            buflen = padding_length;

        }

        padding_length -= buflen;

        memset(tmpstr, ' ', (buflen));
        *(tmpstr + buflen) = '\0';

        _outs(tmpstr, _op);
    }

    do
    {
        if (input_length > 80)
        {
            buflen = 80;
        }
        else
        {
            buflen = input_length;
        }
        input_length -= buflen;

        memcpy(tmpstr, strbuf, buflen);
        *(tmpstr + buflen) = '\0';

        _outs(tmpstr, _op);
        strbuf += buflen;

    }
    while (input_length > 0);

    while (padding_length > 0 && _STCHK(pfield, _PFMINUS))
    {
        if (padding_length > 80)
        {
            buflen = 80;
        }
        else
        {
            buflen = padding_length;

        }
        padding_length -= buflen;

        memset(tmpstr, ' ', (buflen));
        *(tmpstr + buflen) = '\0';

        _outs(tmpstr, _op);
    }

    return;
}


/*****************************************************************************/
/* _SETFIELD   -  Performs conversions when the '%' is encountered           */
/*                                                                           */
/*    This function takes pfield, and calls the appropriate processing       */
/*    function for the conversion required in the _PFIELD structure.  It     */
/*    returns a pointer to the result string.                                */
/*                                                                           */
/*****************************************************************************/
static void _setfield(char *fld, _PFIELD *pfield, va_list *_ap)
{
   /*------------------------------------------------------------------------*/
   /* Local variable declarations, and a description of their use            */
   /*                                                                        */
   /*    FLD is a temporary string that will hold the conversion.  F_START   */
   /*    will be a pointer to the beginning of the field, and if a           */
   /*    field width was specified, F_END will be a pointer to the end of    */
   /*    the field.  This designated field is located at the beginning of    */
   /*    the string FLD.   A_END is a pointer to the end of the string FLD,  */
   /*    and this is where the primary conversion will take place.  Using    */
   /*    A_IT, an iterator beginning at the end of FLD, the number will be   */
   /*    written one digit at a time starting with the rightmost digit.      */
   /*    Using the pointer WHERE, the number in string form will be moved    */
   /*    to its appropriate place within the field after making adjustments  */
   /*    called for by various flags in the format specification (a minus    */
   /*    sign, leading zeros, etc.).  The string FLD will then be returned.  */
   /*                                                                        */
   /*    MINUS_FLAG is exactly what it says ( = 1 if number is negative).    */
   /*                                                                        */
   /*------------------------------------------------------------------------*/

   char *f_start   =  (char *)fld;
   char *f_end     =  f_start + pfield->fwidth;
   char *a_end     =  f_start + _ARSIZE -1;
   char *a_it      =  a_end;
   char *where;
   int  minus_flag = 0;
   int  plus_flag  = 0;

   /*------------------------------------------------------------------------*/
   /* Initialize the temporary string.  Then, since we are working from      */
   /* right to left, begin with the NULL character.                          */
   /*------------------------------------------------------------------------*/
   memset(fld, ' ', _ARSIZE);
   *(a_it--) = '\0';

   /*------------------------------------------------------------------------*/
   /* Call the appropriate processing function.                              */
   /*------------------------------------------------------------------------*/
   switch(pfield->conv)
   {
      case 'd' :
      case 'i' :
      case 'o' :
      case 'u' :
      case 'x' :
      case 'X' :
      case 'p' :  _pproc_diouxp(pfield, &minus_flag, &a_it, _ap);
                  break;

#ifndef NOFLOAT
      case 'g' :
      case 'G' :
      case 'e' :
      case 'E' :
      case 'f' :  _pproc_fge(pfield, &minus_flag, &a_it, _ap);
                  break;
#endif

      case 'c' :  *(a_it--) = va_arg(*_ap, int);
                  _UNSET(pfield, _PFPLUS);
                  break;

      case '%' :  strcpy(f_start, "%");


   }

   plus_flag = (_STCHK(pfield, _PFPLUS) && SIGNED_CONV);

   /*------------------------------------------------------------------------*/
   /* If the number was negative, or the '+' flag was used, insert the sign. */
   /* Make sure unsigned conversions don't get a '+' sign.                   */
   /*------------------------------------------------------------------------*/
   if (minus_flag) *(a_it--) = '-';
   else if (plus_flag) *(a_it--) = '+';

   /*------------------------------------------------------------------------*/
   /* If the number was positive, the '+' flag was not used, and the ' '     */
   /* flag was used, insert a space.                                         */
   /*------------------------------------------------------------------------*/
   if(! minus_flag && ! plus_flag && _STCHK(pfield, _PFSPACE)) *(a_it--) = ' ';

   /*------------------------------------------------------------------------*/
   /* If the '-' flag was used or the resulting string is larger than the    */
   /* field, left justify the result in the array.  Otherwise right-justify  */
   /* it.                                                                    */
   /*------------------------------------------------------------------------*/
   where = (_STCHK(pfield, _PFMINUS) ||
           ((a_end - a_it) > pfield->fwidth)) ? f_start :
           (f_end - (a_end - a_it)+1);

   a_it = (char *)memccpy(where, a_it+1, '\0', _ARSIZE);

   /*------------------------------------------------------------------------*/
   /* If a resulting left-justified string is smaller than the field width,  */
   /* move the terminating NULL character to the end of the field.           */
   /*------------------------------------------------------------------------*/
   if(a_it <= f_end)
   {
      memset(a_it - 1, ' ', f_end - a_it + 1);
      *f_end = '\0';
   }

   /*------------------------------------------------------------------------*/
   /* If the '0' flag was used, and the resulting string is right-justified, */
   /* fill in the leading zeros.                                             */
   /*------------------------------------------------------------------------*/
   if(_STCHK(pfield, _PFZERO))
   {
      memset(f_start, '0', (where - f_start));

      /*---------------------------------------------------------------------*/
      /* Make sure any sign or leading space is moved to the left side of    */
      /* any leading zeros.                                                  */
      /*---------------------------------------------------------------------*/
      if((minus_flag || plus_flag || _STCHK(pfield, _PFSPACE)) &&
          where != f_start)
      {
         *f_start = *where;
         *where = '0';
      }
   }
   else
      memset(f_start, ' ', (where - f_start));

}


#ifndef NOFLOAT
/*****************************************************************************/
/* _PPPROC_FGE   -  Process the conversion for f, g, G, e, and E             */
/*                                                                           */
/*    This function takes the structure PFIELD, which contains all of the    */
/*    flags and parameters to process the conversion, and it does this       */
/*    conversion, and stores the result in the string pointed to by          */
/*    *A_IT.                                                                 */
/*****************************************************************************/
static void _pproc_fge(_PFIELD *pfield, int *minus_flag, char **a_it,
                       va_list *_ap)
{
   /*------------------------------------------------------------------------*/
   /* Local variables                                                        */
   /*------------------------------------------------------------------------*/
   long double cvt =  0;

   /*------------------------------------------------------------------------*/
   /* Determine what kind of argument is coming next, and read it into CVT.  */
   /*------------------------------------------------------------------------*/
   switch(_STCHK(pfield, _MFLD))
   {
      case  0  :  cvt = (double)va_arg((*_ap), double);
                  break;

      default  :  cvt = va_arg((*_ap), long double);

   }

   /*------------------------------------------------------------------------*/
   /* If CVT is negative, set the MINUS_FLAG and reverse the sign of CVT.    */
   /*------------------------------------------------------------------------*/
   if((*minus_flag = (cvt < 0)) != 0) cvt = -cvt;

   /*------------------------------------------------------------------------*/
   /* Call the proper conversion function                                    */
   /*------------------------------------------------------------------------*/
   switch(pfield->conv)
   {
      case 'f' :  _pconv_f(cvt, pfield, a_it);
                  break;
      case 'e' :
      case 'E' :  _pconv_e(cvt, pfield, a_it);
                  break;
      case 'g' :
      case 'G' :  _pconv_g(cvt, pfield, a_it);
   }
}


/*****************************************************************************/
/* _PCONV_F -  Perform the %f conversion                                     */
/*****************************************************************************/
static void _pconv_f(long double cvt, _PFIELD *pfield, char **a_it)
{
   /*------------------------------------------------------------------------*/
   /* Local variables                                                        */
   /*------------------------------------------------------------------------*/
   char  tmpbuf[400];
   int   dpt,
         sign,
         putdec;

   /*------------------------------------------------------------------------*/
   /* If no precision was specified, set it to 6.                            */
   /*------------------------------------------------------------------------*/
   if(pfield->precision < 0) pfield->precision = 6;

   /*------------------------------------------------------------------------*/
   /* Call the FCVT float to string function, then copy the fractional part, */
   /* determine whether or not a decimal point should be placed, and copy    */
   /* whole number part.                                                     */
   /*------------------------------------------------------------------------*/
   strcpy(tmpbuf, fcvt(cvt, pfield->precision, &dpt, &sign));
   _fcpy(tmpbuf, dpt, 1, pfield->precision, a_it);
   putdec = (dpt != (int)strlen(tmpbuf) || _STCHK(pfield, _PFPOUND)) ? 1 : 0;
   _mcpy(tmpbuf, dpt, putdec, a_it);
}


/*****************************************************************************/
/* _PCONV_E -  Perform the %e conversion                                     */
/*****************************************************************************/
static void _pconv_e(long double cvt, _PFIELD *pfield, char **a_it)
{
   /*------------------------------------------------------------------------*/
   /* Local variables                                                        */
   /*------------------------------------------------------------------------*/
   char  tmpbuf[100];
   int   dpt,
         sign,
         putdec,
         exp      = 0;

   /*------------------------------------------------------------------------*/
   /* If no precision was specified, set it to 6.                            */
   /*------------------------------------------------------------------------*/
   if(pfield->precision < 0) pfield->precision = 6;

   /*------------------------------------------------------------------------*/
   /* Check if CVT is within legal range -HUGE_VALL <= CVT <= HUGE_VALL      */
   /*------------------------------------------------------------------------*/
   if (cvt >= HUGE_VALL) cvt = HUGE_VALL;
   else if (cvt <= -HUGE_VALL) cvt = -HUGE_VALL;

   /*------------------------------------------------------------------------*/
   /* Convert CVT to x.xxxe+xx form, keeping the exponent in EXP.            */
   /*------------------------------------------------------------------------*/
   if(cvt)
   {
      for(;cvt < 1; cvt *= 10, exp--);
      for(;cvt >= 10; cvt /= 10, exp++);
   }

   /*------------------------------------------------------------------------*/
   /* Call the FCVT float to string function, copy the exponent part, the    */
   /* fractional part, then determine whether or not a decimal point should  */
   /* be placed, and copy the whole number part.                             */
   /*------------------------------------------------------------------------*/
   strcpy(tmpbuf, fcvt(cvt, pfield->precision, &dpt, &sign));
   if(dpt==2) /* fcvt() might have rounded the number */
   {
       dpt--; exp++; tmpbuf[strlen(tmpbuf)-1] = 0;
   }
   _ecpy(exp, pfield->conv, a_it);
   _fcpy(tmpbuf, dpt, 1, pfield->precision, a_it);
   putdec = (dpt != (int)strlen(tmpbuf) || _STCHK(pfield, _PFPOUND)) ? 1 : 0;
   _mcpy(tmpbuf, dpt, putdec, a_it);
}


/*****************************************************************************/
/* _PCONV_G -  Perform the %g conversion                                     */
/*****************************************************************************/
static void _pconv_g(long double cvt, _PFIELD *pfield, char **a_it)
{
   /*------------------------------------------------------------------------*/
   /* Local variables                                                        */
   /*------------------------------------------------------------------------*/
   char  tmpbuf[100];
   char  *check;
   int   dpt,
         sign,
         putdec,
         exp         = 0,
         change_test = 0;

   /*------------------------------------------------------------------------*/
   /* If the precision was given as 0, set it to one.                        */
   /*------------------------------------------------------------------------*/
   if(pfield->precision == 0) pfield->precision = 1;

   /*------------------------------------------------------------------------*/
   /* If no precision was specified, set it to 6.                            */
   /*------------------------------------------------------------------------*/
   if(pfield->precision < 0) pfield->precision = 6;

   strcpy(tmpbuf, ecvt(cvt, pfield->precision, &dpt, &sign));

   /*------------------------------------------------------------------------*/
   /* If the exponent is less than -4, or greater than or equal to the       */
   /* precision, convert the number as a %e conversion.  Otherwise convert   */
   /* it as a %f conversion.                                                 */
   /*------------------------------------------------------------------------*/
   if(dpt < -3 || dpt > pfield->precision)
   {
      for(;dpt > 1; dpt--, exp++);
      for(;dpt < 1; dpt++, exp--);

      _ecpy(exp, pfield->conv-2, a_it);
   }

   /*------------------------------------------------------------------------*/
   /* Copy the fractional part of the number.  CHANGE_TEST will be set if    */
   /* there was a fractional part, otherwise it will remain a zero.          */
   /*------------------------------------------------------------------------*/
   check = *a_it;
   _fcpy(tmpbuf, dpt, (_STCHK(pfield, _PFPOUND)) ? 1 : 0, pfield->precision,
         a_it);
   change_test = (check != *a_it);

   /*------------------------------------------------------------------------*/
   /* If the '#' flag was used, or there was a fractional part to the number */
   /* a decimal point will be placed.                                        */
   /*------------------------------------------------------------------------*/
   putdec = (_STCHK(pfield, _PFPOUND) || change_test) ? 1 : 0;

   _mcpy(tmpbuf, dpt, putdec, a_it);

}


/*****************************************************************************/
/* _FCPY -  Copy the fraction part of a float to a string                    */
/*****************************************************************************/
static char *_fcpy(const char *tmpbuf, int dpt, int trail, int precision,
                   char **a_it)
{
   /*------------------------------------------------------------------------*/
   /* Local variables                                                        */
   /*------------------------------------------------------------------------*/
   int   i;
   char  *tmpptr = (char *)tmpbuf + strlen(tmpbuf) -1;

   /*------------------------------------------------------------------------*/
   /* Fill all unused precision spaces with zeros.                           */
   /*------------------------------------------------------------------------*/
   for(i = 0; i < precision && dpt > (int)strlen(tmpbuf) && trail; i++)
      *((*a_it)--) = '0';

   /*------------------------------------------------------------------------*/
   /* Copy the fractional part of the float into the string.                 */
   /*------------------------------------------------------------------------*/
   if(dpt < (int)strlen(tmpbuf) && dpt >= 0)
   {
      i = (int)strlen(tmpbuf) - dpt;

      /*---------------------------------------------------------------------*/
      /* Skip trailing zeros if TRAIL is not set.                            */
      /*---------------------------------------------------------------------*/
      if(! trail) for(; i > 0 && *tmpptr == '0'; tmpptr--, i--);

      for(; i > 0; tmpptr--, i--) *((*a_it)--) = *tmpptr;

   }


   /*------------------------------------------------------------------------*/
   /* Place any leading fractional zeros if necessary.                       */
   /*------------------------------------------------------------------------*/
   if(dpt < 0)
   {
      if (-dpt < precision)
          for(i = strlen(tmpbuf); i > 0; tmpptr--, i--) *((*a_it)--) = *tmpptr;
      if (-dpt > precision) dpt = -precision;
      for(i = -dpt; i > 0; i--) *((*a_it)--) = '0';
   }

   return (*a_it);
}


/*****************************************************************************/
/* _ECPY -  Copy the "e+xx" part of a float to a string                      */
/*****************************************************************************/
static char *_ecpy(int exp, char letter, char **a_it)
{
   _ltostr((exp > 0) ? exp : -exp, 10, 'd', a_it);
   if(exp < 10 && exp > -10) *((*a_it)--) = '0';
   *((*a_it)--) = (exp < 0) ? '-' : '+';
   *((*a_it)--) = letter;

   return (*a_it);
}


/*****************************************************************************/
/* _MCPY -  Copy the whole number part of a float to a string                */
/*****************************************************************************/
static char *_mcpy(const char *tmpbuf, int dpt, int putdec, char **a_it)
{
   /*------------------------------------------------------------------------*/
   /* Local variables                                                        */
   /*------------------------------------------------------------------------*/
   int   i;
   char  *tmpptr = NULL;

   /*------------------------------------------------------------------------*/
   /* If the number has both a whole number part and a fractional part,      */
   /* position TMPPTR to the last character of the whole number.             */
   /*------------------------------------------------------------------------*/
   if(dpt > 0 && dpt <= (int)strlen(tmpbuf)) tmpptr = (char *)tmpbuf + dpt -1;

   /*------------------------------------------------------------------------*/
   /* Place a decimal point if PUTDEC is set.                                */
   /*------------------------------------------------------------------------*/
   if(putdec) *((*a_it)--) = '.';

   /*------------------------------------------------------------------------*/
   /* Place any whole number trailing zeros.                                 */
   /*------------------------------------------------------------------------*/
   for(i = dpt; i > (int)strlen(tmpbuf); i--) *((*a_it)--) = '0';

   /*------------------------------------------------------------------------*/
   /* Copy the rest of the whole number.                                     */
   /*------------------------------------------------------------------------*/
   if(i > 0) for(; tmpptr >= tmpbuf; tmpptr--) *((*a_it)--) = *tmpptr;
   else *((*a_it)--) = '0';

   return (*a_it);
}
#endif


/*****************************************************************************/
/* _PPROC_DIOUXP   -  Process the conversion for d, i, o, u, x, and p        */
/*                                                                           */
/*    This function takes the structure PFIELD, which contains all of the    */
/*    flags and parameters to process the conversion, and it does this       */
/*    conversion, and stores the result in the string pointed to by          */
/*    *A_IT.                                                                 */
/*                                                                           */
/*****************************************************************************/
static int _pproc_diouxp(_PFIELD *pfield, int *minus_flag, char **a_it,
                         va_list *_ap)
{
   /*------------------------------------------------------------------------*/
   /* Local variables                                                        */
   /*------------------------------------------------------------------------*/
   int digits =  0;
   int base   = 10;
   uintmax_t cvt;

   /*------------------------------------------------------------------------*/
   /* If no precision was given, set it to 1.                                */
   /*------------------------------------------------------------------------*/
   if(pfield->precision < 0) pfield->precision = 1;
   else                      _UNSET(pfield, _PFZERO);

   /*------------------------------------------------------------------------*/
   /* Set the base of the number by the type of conversion specified.        */
   /*------------------------------------------------------------------------*/
   switch(pfield->conv)
   {
      case 'p' :
      case 'x' :
      case 'X' :  base = 16;
                  break;

      case 'o' :  base = 8;
                  break;

      default  :
      case 'u' :
      case 'd' :
      case 'i' :  base = 10;
  	          break;
   }

   /*------------------------------------------------------------------------*/
   /* Get the next argument.                                                 */
   /*------------------------------------------------------------------------*/
   cvt = _getarg_diouxp(pfield, _ap);

   /*------------------------------------------------------------------------*/
   /* If the precision is 0, and the number is 0, do nothing and return 1.   */
   /*------------------------------------------------------------------------*/
   if(pfield->precision == 0 && cvt == 0) return 1;

   /*------------------------------------------------------------------------*/
   /* If the number is signed and negative, set the minus sign flag, and     */
   /* negate the number.                                                     */
   /*------------------------------------------------------------------------*/
   if((pfield->conv == 'd' || pfield->conv == 'i')
      && ((intmax_t)cvt < 0))
   {
      *minus_flag = 1;
      cvt = -(intmax_t)cvt;
   }

   /*------------------------------------------------------------------------*/
   /* Call the function to convert the number to a string, and add the       */
   /* total number of digits assigned into DIGITS.                           */
   /*------------------------------------------------------------------------*/
   digits += _ltostr(cvt, base, pfield->conv, a_it);

   /*------------------------------------------------------------------------*/
   /* Fill in the remainder of the precision with zeros.                     */
   /*------------------------------------------------------------------------*/
   while(digits++ < pfield->precision) *((*a_it)--) = '0';


   /*------------------------------------------------------------------------*/
   /* If the "#" flag was used in the X or x conversion, prefix a "0x" or    */
   /* "0X" to the hexadecimal number.                                        */
   /*------------------------------------------------------------------------*/
   if((pfield->conv == 'x' || pfield->conv == 'X') && _STCHK(pfield, _PFPOUND))
   {
      *((*a_it)--) = pfield->conv;
      *((*a_it)--) = '0';
   }

   /*------------------------------------------------------------------------*/
   /* If the "#' flag was used in the o conversion, prefix a "0" to the      */
   /* octal number.                                                          */
   /*------------------------------------------------------------------------*/
   if(pfield->conv == 'o' && _STCHK(pfield, _PFPOUND)) *((*a_it)--) = '0';

   return (0);

}


/*****************************************************************************/
/* _GETARG_DIOUXP -  Get the argument for a d, i, o, u, x, or p conversion   */
/*                                                                           */
/*    This function takes the next argument off the argument list, after     */
/*    determining what kind of argument it is.  It decides this by checking  */
/*    to see if the 'h' or the 'l' flag was used.  It returns the next       */
/*    argument.                                                              */
/*****************************************************************************/
static uintmax_t _getarg_diouxp(_PFIELD *pfield, va_list *_ap)
{
   /*------------------------------------------------------------------------*/
   /* Local variables                                                        */
   /*------------------------------------------------------------------------*/
   uintmax_t cvt = 0;

   if (pfield->conv == 'p') cvt = (uintmax_t)va_arg(*_ap, void *);
   else
   /*------------------------------------------------------------------------*/
   /* Get the number from the next argument.  Determine what kind of         */
   /* argument by checking for the h or l flag in the format specification.  */
   /*------------------------------------------------------------------------*/
   switch(_STCHK(pfield, (_MFH | _MFL | _MFLL)))
   {
      case _MFH   :  switch(pfield->conv)
                     {
                        case 'd' :
                        case 'i' :
			    cvt = va_arg(*_ap, int);
			    break;

                        case 'o' :
                        case 'u' :
                        case 'x' :
                        case 'X' :
			    cvt = (unsigned short) va_arg(*_ap, unsigned int);
                     }
                     break;

      case _MFL   :  switch(pfield->conv)
                     {
                        case 'd' :
                        case 'i' :
			    cvt = va_arg(*_ap, long int);
			    break;

                        case 'o' :
                        case 'u' :
                        case 'x' :
                        case 'X' :
			    cvt = va_arg(*_ap, unsigned long int);
                     }
                     break;
#ifdef LLONG_MAX
      case _MFLL  :  switch(pfield->conv)
                     {
                        case 'd' :
                        case 'i' :
			    cvt = va_arg(*_ap, long long int);
			    break;

                        case 'o' :
                        case 'u' :
                        case 'x' :
                        case 'X' :
			    cvt = va_arg(*_ap, unsigned long long int);
                     }
                     break;
#endif
      default     :  switch(pfield->conv)
                     {
                        case 'd' :
                        case 'i' :
			    cvt = va_arg(*_ap, int);
			    break;

                        case 'o' :
                        case 'u' :
                        case 'x' :
                        case 'X' :
			    cvt = va_arg(*_ap, unsigned int);
                     }
   }

   return (cvt);
}


/*****************************************************************************/
/* _LTOSTR  -  Convert an integer to a string of up to base 16               */
/*                                                                           */
/*    This function takes an uintmax_t integer, converts it to a string      */
/*    which is pointed to by *A_IT.  The result will also be converted to    */
/*    a base corresponding to the variable base.                             */
/*                                                                           */
/*****************************************************************************/
static int _ltostr(uintmax_t cvt, int base, char conv, char **a_it)
{
   /*------------------------------------------------------------------------*/
   /* Local Variables                                                        */
   /*------------------------------------------------------------------------*/
   uintmax_t  quot,
              rem;
       char  *bnum = "0123456789abcdef0123456789ABCDEF";

   /*------------------------------------------------------------------------*/
   /* The number CVT will be converted to a string by taking the remainder   */
   /* of a division of it by its base, and converting it to a character.     */
   /* The number CVT is then set equal to itself divided by its base, and    */
   /* this continues until CVT is 0.                                         */
   /*------------------------------------------------------------------------*/

   if(! cvt) *((*a_it)--) = '0';

   while(cvt)
   {
      quot = _div(cvt, base);
      rem = cvt - (quot * base);

      if(conv == 'X') rem += 16;

      *((*a_it)--) = bnum[rem];
      cvt = quot;
   }

   return (strlen(*a_it) - 1);
}


/*****************************************************************************/
/* _DIV  -  Divide two integers                                              */
/*                                                                           */
/*    This function takes a uintmax_t, and divides it by an integer.         */
/*    Division must take place in unsigned arithmetic, because signed '/'    */
/*    can overflow.  This function is used by _LTOSTR when it is converting  */
/*    an unsigned int to a string.                                           */
/*                                                                           */
/*****************************************************************************/
static uintmax_t _div(uintmax_t cvt, int base)
{
    /*-----------------------------------------------------------------------*/
    /* Use shifts to optimize power-of-two bases                             */
    /*-----------------------------------------------------------------------*/
    switch(base)
    {
	case  8: return cvt >> 3;
	case 16: return cvt >> 4;
    }

    /*-----------------------------------------------------------------------*/
    /* Perform divide in narrowest arithmetic possible, for speed.	     */
    /*-----------------------------------------------------------------------*/
         if (cvt < UINT_MAX)  return (unsigned int)cvt / base;
    else if (cvt < ULONG_MAX) return (unsigned long)cvt / base;
    else return cvt / base;
}




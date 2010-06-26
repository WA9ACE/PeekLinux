/*****************************************************************************/
/*  _SCANFI.C v2.54                                                          */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                   */
/*****************************************************************************/

/*****************************************************************************/
/*    This file contains the main routines that all three variations of the  */
/*    scanf function use.  The main function in the file is _scanfi, and     */
/*    the other functions here are called by it.                             */
/*                                                                           */
/* FUNCTIONS:                                                                */
/*    _scanfi        -  The main scanf handling routine                      */
/*    _sget_conv     -  Read the format flags into the _SFIELD pointer sfield*/
/*    _sget_scanset  -  Read in the scanset from the format statement        */
/*    _sproc_int     -  Read an integer string into a temporary string       */
/*    _sproc_float   -  Read a float string into a temporary string          */
/*    _sproc_str     -  Copy a string from the input source to a temporary   */
/*                      string                                               */
/*    _sproc_lb      -  Process the %[ conversion                            */
/*    _sset_arg      -  Assign the converted value to the next argument      */
/*****************************************************************************/
#include <stdio.h>
#include "format.h"
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>

#ifdef LLONG_MAX
_CODE_ACCESS long long strtoll(const char *_st, char **_endptr, int _base);
_CODE_ACCESS unsigned long long strtoull(const char *_st, char **_endptr,
					 int _base);
#endif

_CODE_ACCESS long double strtold(const char *st, char **endptr);

static int _sget_conv(char **_format, _SFIELD *sfield);
static int _sget_scanset(_SFIELD *sfield, char **_format);
static int _sproc_int(int w_counter, int (*_inpchar)(void **inp),
                       void (*_uninpchar)(void **inp, char outchar),
                       char *tmpptr, char conv, void **inp, int *num_read);
static int _sproc_float(int w_counter, int (*_inpchar)(void **inp),
                         void (*_uninpchar)(void **inp, char outchar),
                         char *tmpptr, char conv, void **inp, int *num_read);
static int _sproc_str(int w_counter, int (*_inpchar)(void **inp),
                       void (*_uninpchar)(void **inp, char outchar),
                       char *tmpptr, char conv, void **inp, int *num_read);
static int _sproc_lb(int (*_inpchar)(void **inp),
                      void (*_uninpchar)(void **inp, char outchar),
                      char *tmpptr, _SFIELD *sfield, void **inp, int *num_read);
static void _sset_arg(_SFIELD *sfield, va_list *_ap, char *tmpbuf);

/*****************************************************************************/
/* _SCANFI  -  The main scanf handling routine                               */
/*                                                                           */
/*    This function parses all non-conversion characters in the format       */
/*    string, passes control to the appropriate function when a '%' is       */
/*    encountered, then calls _SSET_ARG, which assignes the result to the    */
/*    next argument.                                                         */
/*                                                                           */
/*****************************************************************************/
int _scanfi(void *inp, const char *_format, va_list _ap, 
            int (*_chkmbc)(void **inp, char **_format, int *num_read),
            int (*_inpchar)(void **inp), 
            void (*_uninpchar)(void **inp, char outchar))
{
#ifdef ENABLE_SCANF
   /*------------------------------------------------------------------------*/
   /* Local variables                                                        */
   /*------------------------------------------------------------------------*/
   _SFIELD  sfield;
   char     tmpbuf[256],
           *tmpptr,
           *f_ptr        = (char *)_format;
   int      num_assigned =  0,
            inchar,
            num_read     =  0,
            stat         =  0;

   /*------------------------------------------------------------------------*/
   /* If the first character in the format string is a white space character */
   /* parse the format string until a non-white space character is found.    */
   /* Do the same for the input, but put the first non-white space character */
   /* back onto the input stream when finished.                              */
   /*------------------------------------------------------------------------*/
   if (isspace(*f_ptr))
   {
      for(;isspace(*f_ptr);f_ptr++);

      inchar = _inpchar(&inp);
      if(inchar == EOF) return EOF;
      num_read++;

      for(;isspace(inchar); inchar = _inpchar(&inp), num_read++);

      _uninpchar(&inp, inchar);
      num_read--;

      if(inchar == EOF) return EOF;
   }

   while(1)
   {
      /*---------------------------------------------------------------------*/
      /* Initialize sfield                                                   */
      /*---------------------------------------------------------------------*/
      memset(&sfield, 0, sizeof(_SFIELD));
      sfield.fwidth = -1;

      /*---------------------------------------------------------------------*/
      /* Call _chkmbc to compare the format string to the input.  If a       */
      /* mismatch occurs, return an EOF, if the end of the format string     */
      /* is reached, return the number of arguments assigned.  Otherwise     */
      /* a '%' has been encountered, so call _sget_conv to process it.       */
      /*---------------------------------------------------------------------*/
      switch(_chkmbc(&inp, &f_ptr, &num_read))
      {
         case  EOF   : return (EOF);
   
         case  0     : return (num_assigned);
   
         case  1     :  _sget_conv(&f_ptr, &sfield);
   
      }
   
      tmpptr = tmpbuf;

      /*---------------------------------------------------------------------*/
      /* Unless the conversion specifier is a [, c, or n, skip to the next   */
      /* non-white space character in the input.                             */
      /*---------------------------------------------------------------------*/
      if (sfield.conv != '[' && sfield.conv != 'c' && sfield.conv != 'n')
      {
         inchar = _inpchar(&inp);
         num_read++;
    
         for(;isspace(inchar); inchar = _inpchar(&inp), num_read++);
    
         _uninpchar(&inp, inchar);
         num_read--;

         /*---------------------------------------------------------------*/
         /* If we've encountered the end of the stream AND we haven't     */
         /* matched anything yet, return EOF.                             */
         /*---------------------------------------------------------------*/
         if(inchar == EOF && num_read == 0) return EOF;
      }
      else
      {
         /*---------------------------------------------------------------*/
         /* If we've encountered the end of the stream AND we haven't     */
         /* matched anything yet, return EOF.                             */
         /*---------------------------------------------------------------*/
         inchar = _inpchar(&inp);
         _uninpchar(&inp, inchar);
         if(inchar == EOF && num_read == 0) return EOF;
      }

      /*---------------------------------------------------------------------*/
      /* The flags have been set in sfield, so process the conversion by     */
      /* calling the appropriate function.                                   */
      /*---------------------------------------------------------------------*/
      switch(sfield.conv)
      {
         case  'X'   :  sfield.conv = 'x';
         case  'i'   :
         case  'p'   :  
         case  'x'   :
         case  'u'   :
         case  'o'   :
         case  'd'   :  stat = _sproc_int(sfield.fwidth, _inpchar, _uninpchar, 
                                         tmpptr, sfield.conv, &inp, &num_read); 
                        break;
         case  'f'   :
         case  'e'   :
         case  'E'   :
         case  'g'   :
         case  'G'   :  stat = _sproc_float(sfield.fwidth, _inpchar, _uninpchar,
                                     tmpptr, sfield.conv, &inp, &num_read);
                        break;

         case  'c'   :  
         case  's'   : {
                          char *stptr = (sfield.flags & _SFSTAR) ?
                                         NULL : va_arg(_ap, char*);

                          stat = _sproc_str(sfield.fwidth, _inpchar, _uninpchar,
                                     stptr, sfield.conv, &inp, &num_read);
                       }
                       stat = (stat != EOF);
                       if (!(sfield.flags & _SFSTAR) && stat) num_assigned++;
                       break;

         case  '['   :  stat = _sproc_lb(_inpchar, _uninpchar, tmpptr, &sfield,
                                         &inp, &num_read);

      }

      stat = (stat != EOF);

      /*---------------------------------------------------------------------*/
      /* Now, call the function to handle the actual assignment, or if there */
      /* is no assignment to take place, process it here.                    */
      /*---------------------------------------------------------------------*/
      switch(sfield.conv)
      {
         case  'i'   :
         case  'd'   :  
         case  'x'   :
         case  'u'   :
         case  'o'   :  
         case  'p'   : 
         case  'e'   :
         case  'f'   :
         case  'g'   :
         case  'E'   :
         case  'G'   :  _sset_arg(&sfield, &_ap, tmpbuf);
                        if ((!(sfield.flags & _SFSTAR)) && stat) num_assigned++;
                        break;
 
         case  'n'   :  if (!(sfield.flags & _SFSTAR))
                           switch(sfield.flags & (_MFH | _MFL | _MFLL))
                           {
                              case  _MFH  :  *(va_arg(_ap, short int*)) =
                                             (short int)num_read;
                                             break;
 
                              case _MFL   :  *(va_arg(_ap, long int*)) =
                                             (long int)num_read;
                                             break;
#ifdef LLONG_MAX
                              case _MFLL  :  *(va_arg(_ap, long long int*)) =
                                             (long long int)num_read;
                                             break;
#endif
                              default     :  *(va_arg(_ap, int*)) =
                                             num_read;
                                             break;
                           }
                        break;

         case  '%'   :  inchar = _inpchar(&inp);
                        if (inchar != '%') return (EOF);
                        else num_read++;
                        break;
                              
         case  '['   :  if ((!(sfield.flags & _SFSTAR)) && stat)
                        {
                           strcpy(va_arg(_ap, char *), tmpbuf);
                           num_assigned++;
                        }
      }
   }
#else
return 0;
#endif
}

/*****************************************************************************/
/* _SGET_CONV  -  Read the format flags into the _SFIELD pointer sfield      */
/*                                                                           */
/*    This function reads the characters directly after the '%' character,   */
/*    and stores them as flags in sfield, a pointer to a _SFIELD structure.  */
/*    These flags will later be used to process the conversion.              */
/*                                                                           */
/*****************************************************************************/
static int _sget_conv(char **_format, _SFIELD *sfield)
{
   /*------------------------------------------------------------------------*/
   /* Local variables                                                        */
   /*------------------------------------------------------------------------*/
   char  tmpbuf[8],
         *tmpptr     =  tmpbuf,
         *strend     =  (*_format) + strlen(*_format);

   (*_format)++;                        /* Go to the character after the '%' */

   /*------------------------------------------------------------------------*/
   /* If the next character in the format statement is a '*', set the        */
   /* _SFSTAR flag in sfield                                                 */
   /*------------------------------------------------------------------------*/
   if (**_format == '*')
   {
      _SET(sfield, _SFSTAR);
      (*_format)++;
   }
 
   /*------------------------------------------------------------------------*/
   /* If numerical characters follow, read them into a temporary string,     */
   /* convert it into a number, and store it as the field width in sfield    */
   /*------------------------------------------------------------------------*/
   for(;**_format >= '0' && **_format <= '9'; *(tmpptr++) = *((*_format)++));
   *tmpptr = '\0';
 
   if (strlen(tmpbuf)) sfield->fwidth = atoi(tmpbuf);
 
   /*------------------------------------------------------------------------*/
   /* Set the h, l, or L flags if they were specified                        */
   /*------------------------------------------------------------------------*/
   switch(**_format)
   {
       case 'L': _SET(sfield, _MFLD); (*_format)++; break;
       case 'h': _SET(sfield, _MFH);  (*_format)++; break;
       case 'l': 
       {
	   (*_format)++;
	   if (**_format == 'l') { _SET(sfield, _MFLL); (*_format)++; }
	   else _SET(sfield, _MFL);
       }
   }

   /*------------------------------------------------------------------------*/
   /* Read in the last character as the conversion specifier                 */
   /*------------------------------------------------------------------------*/
   sfield->conv = *((*_format)++);

   /*------------------------------------------------------------------------*/
   /* For the '[' conversion, read in the scanset.  Return an EOF if         */
   /* _SGET_SCANSET fails.                                                   */
   /*------------------------------------------------------------------------*/
   if ((sfield->conv == '[') && _sget_scanset(sfield, _format)) return (EOF);

   /*------------------------------------------------------------------------*/
   /* If we read past the end of the format string, return an error          */
   /*------------------------------------------------------------------------*/
   if (*_format > strend) return (EOF);
   else return (0);
}

/*****************************************************************************/
/* _SGET_SCANSET  -  Read in the scanset from the format statement           */
/*                                                                           */
/*    This function is called when the '[' conversion specifier has been     */
/*    encountered.  It reads in the scanset from the format statement,       */
/*    and stores it in sfield for later reference during the conversion.     */
/*                                                                           */
/*****************************************************************************/
static int _sget_scanset(_SFIELD *sfield, char **_format)
{
   /*------------------------------------------------------------------------*/
   /* Local variables                                                        */
   /*------------------------------------------------------------------------*/
   char  *tmpptr  =  sfield->scanset;

   if (**_format == '^')
   {
      _SET(sfield, _SFCIRC);
      (*_format)++;
   }

   if (**_format == ']') *(tmpptr++) = *((*_format)++);

   while(**_format != ']' && **_format != '\0') *(tmpptr++) = *((*_format)++);

   *tmpptr = '\0';
   if (**_format == ']') (*_format)++;
   if (**_format == '\0') return (EOF);

   return(0);
}

/*****************************************************************************/
/* _SPROC_INT  -  Read an integer string into a temporary string             */
/*                                                                           */
/*    This function takes the next integer in character form from the        */
/*    current input source, and copies it into a temporary string, where     */
/*    it can later be converted into a numerical value.                      */
/*                                                                           */
/*****************************************************************************/
static int _sproc_int(int w_counter, int (*_inpchar)(void **inp), 
                       void (*_uninpchar)(void **inp, char outchar), 
                       char *tmpptr, char conv, void **inp, int *num_read) 
{
   /*------------------------------------------------------------------------*/
   /* Note: w_counter is a parameter that holds the field width.  When       */
   /*       the number of digits specified by w_counter has been read from   */
   /*       input, the function finishes.  w_counter is checked before each  */
   /*       read to make sure that it is not equal to zero, and it is        */
   /*       decremented after each read.  If no field width was specified,   */
   /*       w_counter will be equal to -1, in which case it will never       */
   /*       equal zero, and the function will read from input until it       */
   /*       encounters the first invalid character.                          */
   /*------------------------------------------------------------------------*/

   /*------------------------------------------------------------------------*/
   /* Local variables                                                        */
   /*------------------------------------------------------------------------*/
   signed char inchar;
   int bnum_read = *num_read;

   /*------------------------------------------------------------------------*/
   /* Read in the next character                                             */
   /*------------------------------------------------------------------------*/
   inchar = _inpchar(inp);
   (*num_read)++;

   /*------------------------------------------------------------------------*/
   /* Accept the next character if it is a sign for the number               */
   /*------------------------------------------------------------------------*/
   if ((inchar == '+' || inchar == '-') && w_counter != 0)
   {
      *(tmpptr++) = inchar;
      inchar = _inpchar(inp);
      (*num_read)++;
      w_counter--;
   }


   /*------------------------------------------------------------------------*/
   /* Accept a leading '0' for an octal number, or a '0x' or '0X' for a      */
   /* hexadecimal number.                                                    */
   /*------------------------------------------------------------------------*/
   if ((conv == 'o' || conv == 'i' || conv =='x' || conv == 'p') && 
      w_counter != 0 && inchar == '0')
   {
      *(tmpptr++) = inchar;
      inchar = _inpchar(inp);
      (*num_read)++;
      w_counter--;
   }

   if ((conv == 'x' || conv == 'p' || conv == 'i') && w_counter != 0 &&
     (inchar == 'x' || inchar == 'X'))
   {
      *(tmpptr++) = inchar;
      inchar = _inpchar(inp);
      (*num_read)++;
      w_counter--;
   }

   /*------------------------------------------------------------------------*/
   /* Accept digits 0-9 for decimal numbers, or 0-F for hexadecimal numbers  */
   /*------------------------------------------------------------------------*/
   while(((inchar >= '0' && inchar <= '9') || 
          ((conv == 'x' || conv == 'p' || conv == 'i') &&
          ((inchar >= 'A' && inchar <= 'F') ||
          (inchar >= 'a' && inchar <= 'f')))) && w_counter != 0)
   {
      *(tmpptr++) = inchar;
      inchar = _inpchar(inp);
      (*num_read)++;
      w_counter--;
   }

   _uninpchar(inp, inchar);
   (*num_read)--;

   *tmpptr = '\0';

   if (bnum_read == *num_read) return(EOF);
   return(1);
}

/*****************************************************************************/
/* _SPROC_FLOAT   -  Read a float string into a temporary string             */
/*                                                                           */
/*    This function takes the next float in character form from the          */
/*    current input source, and copies it into a temporary string, where     */
/*    it can later be converted into a numerical value.                      */
/*                                                                           */
/*****************************************************************************/
static int _sproc_float(int w_counter, int (*_inpchar)(void **inp), 
                         void (*_uninpchar)(void **inp, char outchar), 
                         char *tmpptr, char conv, void **inp, int *num_read)
{
   /*------------------------------------------------------------------------*/
   /* Note: w_counter is a parameter that holds the field width.  When       */
   /*       the number of digits specified by w_counter has been read from   */
   /*       input, the function finishes.  w_counter is checked before each  */
   /*       read to make sure that it is not equal to zero, and it is        */
   /*       decremented after each read.  If no field width was specified,   */
   /*       w_counter will be equal to -1, in which case it will never       */
   /*       equal zero, and the function will read from input until it       */
   /*       encounters the first invalid character.                          */
   /*------------------------------------------------------------------------*/
 
   /*------------------------------------------------------------------------*/
   /* Local variables                                                        */
   /*------------------------------------------------------------------------*/
   signed char  inchar;
   int          invalid   = 0;
   int          bnum_read = *num_read;
   int          dec_flag  =  0;
   int          e_flag    =  0;

   /*------------------------------------------------------------------------*/
   /* Read in the next character                                             */
   /*------------------------------------------------------------------------*/
   inchar = _inpchar(inp);
   (*num_read)++;

   /*------------------------------------------------------------------------*/
   /* Accept the next character if it is a sign                              */
   /*------------------------------------------------------------------------*/
   if ((inchar == '+' || inchar == '-') && w_counter != 0)
   {
      *(tmpptr++) = inchar;
      inchar = _inpchar(inp);
      (*num_read)++;
      w_counter--;
   }
 
   /*------------------------------------------------------------------------*/
   /* Accept the next character if it is a numerical digit.                  */
   /* The characters '.', 'e', 'E', '+', and '-' are accepted under the      */
   /* following conditions:                                                  */
   /*                                                                        */
   /* '.'         :  a '.', an 'e', or an 'E' has not yet been encountered   */
   /* 'e' or 'E'  :  neither of these characters have been encounterd yet    */
   /* '+' or '-'  :  If the last character read was an 'E' or an 'e'         */
   /*------------------------------------------------------------------------*/
   while((inchar >= '0' && inchar <= '9') || inchar == '.' || inchar == 'e'
          || inchar == 'E' || inchar == '+' || inchar == '-')
   {

      switch(inchar)
      {
         case  '.'   :  if (dec_flag || e_flag) { invalid = 1; break; } 
                        else dec_flag = 1;
                        break;

         case  'e'   :
         case  'E'   :  if (e_flag) { invalid = 1; break; }
                        else e_flag = 1;
                        break;

         case  '+'   :  
         case  '-'   :  if (*(tmpptr-1) != 'E' && *(tmpptr-1) != 'e')
                        { invalid = 1; break; }
      }

      if (invalid) break;
      *(tmpptr++) = inchar;
      inchar = _inpchar(inp);
      if (inchar != EOF) (*num_read)++;
      w_counter--;
   }

   _uninpchar(inp, inchar);
   (*num_read)--;

   *tmpptr = '\0';

   if (bnum_read == *num_read) return(EOF);
   return(1);
}

/*****************************************************************************/
/* _SPROC_STR  -  Copy a string from the input source to a temporary string  */
/*                                                                           */
/*    This function takes a string from the input source, and copies it      */
/*    into a temporary string, to be later assigned to a scanf argument.     */
/*                                                                           */
/*****************************************************************************/
static int _sproc_str(int w_counter, int (*_inpchar)(void **inp), 
                       void (*_uninpchar)(void **inp, char outchar), 
                       char *tmpptr, char conv, void **inp, int *num_read)
{
   /*------------------------------------------------------------------------*/
   /* Local variables                                                        */
   /*------------------------------------------------------------------------*/
   signed char inchar;
   int bnum_read = *num_read;

   /*------------------------------------------------------------------------*/
   /*	If no precision was given for the %c conversion, set it to one.        */
   /*------------------------------------------------------------------------*/
   if ((conv == 'c') && (w_counter < 0)) w_counter = 1;

   /*------------------------------------------------------------------------*/
   /* Read in the next character, then while that character is not a white   */
   /* space character, a null terminator, an EOF character, and the field    */
   /* width has not been exceeded, copy it to the temporary string, and      */
   /* get another character.                                                 */
   /*------------------------------------------------------------------------*/
   inchar = _inpchar(inp);
   (*num_read)++;

   while((conv == 'c' || !isspace(inchar)) && inchar != '\0' &&
         inchar != EOF && w_counter--)
   { 
      if (tmpptr) *(tmpptr++) = inchar;
      inchar = _inpchar(inp);
      (*num_read)++;
   }
   
   /*------------------------------------------------------------------------*/
   /* The %s conversion specifies that a null terminator be placed at the    */
   /* end of the conversion.                                                 */
   /*------------------------------------------------------------------------*/
   if (conv == 's' && tmpptr) *tmpptr = '\0';

   _uninpchar(inp, inchar);
   (*num_read)--;

   if (bnum_read == *num_read) return(EOF);
   return(1);
}

/*****************************************************************************/
/* _SPROC_LB   -  Process the %[ conversion                                  */
/*                                                                           */
/*    This function copies characters from the input stream into a           */
/*    temporary string until it satisfies the field width, or encounters a   */
/*    character that is not in the scanset.  The scanset is defined as the   */
/*    characters passed between the left and right brackets.  If a '^' is    */
/*    first character after the left bracket, then the scanset is every      */
/*    character that is not listed between the two brackets.                 */
/*                                                                           */
/*****************************************************************************/
static int _sproc_lb(int (*_inpchar)(void **inp), 
                      void (*_uninpchar)(void **inp, char outchar),
                      char *tmpptr, _SFIELD *sfield, void **inp, int *num_read)
{
   /*------------------------------------------------------------------------*/
   /* Local variables                                                        */
   /*------------------------------------------------------------------------*/
   signed char  inchar;
   int          bnum_read = *num_read;
   int          w_counter =  sfield->fwidth;
   int          test;

   inchar = _inpchar(inp);
   (*num_read)++;

   for (; w_counter != 0 && inchar != EOF; w_counter--)
   {
      test = (strrchr(sfield->scanset, inchar) != NULL);
      if (_STCHK(sfield, _SFCIRC)) test = !test;
             
      if (test)
      {
         *(tmpptr++) = inchar;
         inchar = _inpchar(inp);
         (*num_read)++;
      }
      else
      {
         _uninpchar(inp, inchar);
         (*num_read)--;
         break;
      }
   }

   *tmpptr = '\0';

   if (bnum_read == *num_read) return(EOF);
   return(1);
}

/*****************************************************************************/
/* _SSET_ARG   -  Assign the converted value to the next argument            */
/*                                                                           */
/*    This function takes a pointer to the result conversion string, and     */
/*    assigns it to the next argument.  The type of argument to be assigned  */
/*    is determined by the conversion specifier, and the h, l, or L flags    */
/*    if they were used.                                                     */
/*                                                                           */
/*****************************************************************************/
static void _sset_arg(_SFIELD *sfield, va_list *_ap, char *tmpbuf)
{
   /*------------------------------------------------------------------------*/
   /* Local variables                                                        */
   /*------------------------------------------------------------------------*/
   int base;

   /*------------------------------------------------------------------------*/
   /* Do the assignment only if the result string has a length greater than  */
   /* zero, and the '*' flag was not used                                    */
   /*------------------------------------------------------------------------*/
   if (tmpbuf[0] != '\0' && !_STCHK(sfield, _SFSTAR))
      switch(sfield->conv)
      {
         case  'i'   :
         case  'd'   :  base = (sfield->conv == 'd') ? 10 : 0;
   
                        switch(_STCHK(sfield, (_MFH | _MFL | _MFLL)))
                        {
                           case  _MFH  :  *(va_arg(*_ap, short int*)) =
                                          (short int)strtol(tmpbuf, NULL,
                                          base);
                                          break;

                           case _MFL   :  *(va_arg(*_ap, long int*)) =
                                          strtol(tmpbuf, NULL, base);
                                          break;
#ifdef LLONG_MAX
                           case _MFLL  :  *(va_arg(*_ap, long long int*)) =
                                          strtoll(tmpbuf, NULL, base);
                                          break;
#endif
                           default     :  *(va_arg(*_ap, int*)) =
                                          (int)strtol(tmpbuf, NULL, base);
                                          break;
                        }
                        break;

      /*---------------------------------------------------------------------*/
      /* Suppress "conversion from integer to smaller pointer" warning for   */
      /* the %p case.                                                        */
      /*---------------------------------------------------------------------*/
#pragma DIAGNOSTIC_SUPPRESS(1107)
      case  'p'   :  *(va_arg(*_ap, void **))=(void *)strtoul(tmpbuf, NULL, 16);
		     break;
#pragma DIAGNOSTIC_RESET(1107)

      case  'x'   :
      case  'u'   :
      case  'o'   :  base = (sfield->conv == 'u') ? 10 :
                            (sfield->conv == 'x') ? 16 : 8;

                     switch(_STCHK(sfield, (_MFH | _MFL | _MFLL)))
                     {
                        case _MFH : *(va_arg(*_ap, unsigned short int*))=
                                    (unsigned short int)strtoul(tmpbuf,
                                    NULL, base);
                                    break;

                        case _MFL : *(va_arg(*_ap, unsigned long int*)) =
                                    strtoul(tmpbuf, NULL, base);
                                    break;
#ifdef LLONG_MAX
                        case _MFLL: *(va_arg(*_ap, unsigned long long int*)) =
                                    strtoull(tmpbuf, NULL, base);
                                    break;
#endif
                        default   : *(va_arg(*_ap, unsigned int*)) =
                                    (unsigned int)strtoul(tmpbuf, NULL,
                                    base);
                                    break;
                     }
                     break;

      case  'e'   :
      case  'f'   :
      case  'g'   :
      case  'E'   :
      case  'G'   :  switch(_STCHK(sfield, (_MFL | _MFLD)))
                     {
                        case _MFL : *(va_arg(*_ap, double*))=
                                    strtod(tmpbuf, NULL);
                                    break;

                        case _MFLD: *(va_arg(*_ap, long double*)) =
                                    strtold(tmpbuf, NULL);
                                    break;

                        default   : *(va_arg(*_ap, float*)) =
                                    (float)strtod(tmpbuf, NULL);
                                    break;
                     }
                     break;

   }
}

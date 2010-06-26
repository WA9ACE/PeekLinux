/*****************************************************************************
 * $Id: rawprint.c,v 1.2 2001/03/28 18:25:10 rf Exp $
 * $Name: rel_1_3_b_10 $
 *----------------------------------------------------------------------------
 * NexGenOS v1.3
 * Standart Librairies Replacement Functions
 *----------------------------------------------------------------------------
 *    Copyright (c) 1998-2001 NexGen Software.
 *
 *  All rights reserved. NexGen Software' source code is an unpublished
 *  work and the use of a copyright notice does not imply otherwise.
 *  This source code contains confidential, trade secret material of
 *  NexGen Software. Any attempt or participation in deciphering, decoding,
 *  reverse engineering or in any way altering the source code is
 *  strictly prohibited, unless the prior written consent of
 *  NexGen Software is obtained.
 *
 *    This software is  supplied  under  the terms of a
 *    license agreement or nondisclosure agreement with
 *    NexGen Software, and may not be copied or disclosed
 *    except  in  accordance  with  the  terms of  that
 *    agreement.
 *
 *----------------------------------------------------------------------------
 * ngRawPrintf()
 *----------------------------------------------------------------------------
 * 17/10/98 - Regis Feneon
 * 01/12/98 -
 *  Added support for Ethernet and Internet address display
 * 03/02/2000 - Adrien Felon
 *  Corrected unsigned number parsing in ngVSPrintf() call, by changing cast
 *  to unsigned pointers to write into variable value (see case ParseNum).
 * 10/02/2000 - Regis Feneon
 *  ngVSPrintf() format processing moved to ngRawPrintf()
 *  now use macro NG_VA_ARG() for argument processing
 * 20/02/2000 - Adrien Felon
 *  Casting to (unsigned long) before left shift for comparaison with "value"
 *  in ParseNum case
 * 01/03/2000 -
 *  New macro CHECK_OUT_F - private to this file - to check that output char
 *  function is not null (useful when writing chars into a buffer)
 * 02/11/2000 - Regis Feneon
 *  added (unsigned char) cast in CHECK_OUT_F()
 *****************************************************************************/

#include <ngos.h>

/*****************************************************************************
 * CHECK_OUT_F()
 *****************************************************************************
 * This local macro is used to check wether or not an output function
 * is to be used to output a given char.  If no output function is
 * defined (f is NULL) then we write the char into the data buffer and
 * we increment it.  Note that the particular case where we want to
 * flush a buffer (eg. in memory) is handle in ngRawPrintf().
 *****************************************************************************
 * Parameters:
 *  f       output routine (may be 0)
 *  c       char to write (a negative val stands for flushing)
 *  d       output buffer (may be NULL)
 * Return value: none...
 *
 * WARNING: one must not forget this is a macro call, so calls like
 *  CHECK_OUT_F(out_f, *(sPtr++), data) are forbidden because sPtr would be
 *  incremented several times!
 */

#define CHECK_OUT_F(f,c,d) \
    if ((f) != NULL) \
      (f)((unsigned char)(c),(d)); \
    else \
      *(*((char**)d))++ = (c);

/* FillSpaces() prints out a given number of spaces or zeroes */
static void FillSpaces( void (*out_f)( int, void *), void *data,
                        int Size, int DoIt, int FillZero)
{
  if (DoIt)
    while (Size-- >0) {
      CHECK_OUT_F(out_f, FillZero ? '0' : ' ', data);
    }
  return;
}

/* HexDigit() returns the correct hexadecimal digit for a number */
static char HexDigit( unsigned long value, char UpperLower)
{
  if (value < 10)
    return (char)(value + '0');

  return (char)(value-10 + 'A' + UpperLower - 'X');
}

/*****************************************************************************
 * ngRawPrintf()
 *****************************************************************************
 * Tiny implementation of printf
 * It supports: % [-] [0] [<length>] [l] [s|c|x|X|i|d|u|E|I]
 *  -     == Left justify
 *  0     == Fill field with zeroes
 * length == Minimum field width
 *  l     == Doubleword for x|X|i|d|u
 *  s     == string pointer
 *  c     == single character
 *  x,X   == hex number
 *  i,d   == signed decimal number
 *  u     == unsigned number
 *  E     == ethernet address pointer xxxxxx-xxxxxx
 *  I     == internet address (net byte order) a.b.c.d
 *****************************************************************************
 * Parameters:
 *  out_f   output routine
 *  data    data for output routine
 *  fmt     format string
 *  args    list of arguments
 * Return value: number of characters written
 */

int ngRawPrintf( void (*out_f)( int, void *), void *data,
         const char *fmt, NGva_list args)
{
  static const char Percent[] = "%";
  static const char PercentQuery[] = "%???";
  static const char EtherFmt[] = "%02X%02X%02X-%02X%02X%02X";
  static const char IPFmt[] = "%d.%d.%d.%d";

  char *sPtr;
  int count;

  int ZeroFill, Long, LeftJustify, Signed;

  int MinWidth;
  enum {ParseIncomplete, ParseString, ParseNum, ParsePercent,
        ParseAddress, ParseEthernet, ParseInternet, ParseNonsense} ParseType;
  unsigned int NumBase;

  char ch;
  char NumString[16]; /* Holds largest number */

  count = 0;

  for (;;) {
    switch( ch = *(fmt++)) {

    case 0:
      /* flush output */
      /* Do not use macro CHECK_OUT_F because we are flushing it) */

      if (out_f)
        out_f(-1, data);
      else
        **((char**)data) = 0;

      return( count);

    case '%':
      ZeroFill = 0;
      MinWidth = 0;
      Long = 0;
      LeftJustify = 0;
      sPtr = 0;
      NumBase = 10;
      Signed = 0;
      ParseType = ParseIncomplete;
      while( ParseType == ParseIncomplete)
        switch( ch = *(fmt++)) {
        case 0:
          fmt--;
          ParseType = ParseNonsense;
          break;
        case '-':
          LeftJustify = 1;
          break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          MinWidth = MinWidth*10 + ch - '0';
          if (MinWidth == 0) ZeroFill = 1;
          break;
        case 'l':
          Long = 1;
          break;
        case 'c':
          sPtr = NumString;
          sPtr[0] = NG_VA_ARG( args, int);
          sPtr[1] = 0;
          ParseType = ParseString;
          break;
        case 's':
          sPtr = NG_VA_ARG( args, char *);
          ParseType = ParseString;
          break;
        case 'x':
        case 'X':
          NumBase = 16;
          ParseType = ParseNum;
          break;
        case 'd':
        case 'i':
          Signed = 1;
        case 'u':
          ParseType = ParseNum;
          break;
        case 'E':
          ParseType = ParseEthernet;
          break;
        case 'I':
          ParseType = ParseInternet;
          break;
        case '%':
          ParseType = ParsePercent;
          break;
        default:
          ParseType = ParseNonsense;
          break;
        }

      switch (ParseType) {
      case ParsePercent:
        sPtr = (char *) Percent;
        break;

      case ParseNonsense:
        sPtr = (char *) PercentQuery;
        break;

      case ParseEthernet:
        {
          NGubyte *addr;
          MinWidth -= 13;
          FillSpaces( out_f, data, MinWidth, !LeftJustify, 0);
          count += !LeftJustify && (MinWidth>0) ? MinWidth : 0;
          addr = NG_VA_ARG( args, NGubyte *);
          ngSPrintf( NumString, EtherFmt,
                     addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
          sPtr = NumString;
          break;
        }

      case ParseInternet:
        {
          NGuint addr;
          addr = NG_VA_ARG( args, NGuint);
          ngSPrintf( NumString, IPFmt,
                     ((NGubyte *) &addr)[0],
                     ((NGubyte *) &addr)[1],
                     ((NGubyte *) &addr)[2],
                     ((NGubyte *) &addr)[3] );
          sPtr = NumString;
          break;
        }
      case ParseNum:
        {
          unsigned long value;
          if (Long) {
            value = NG_VA_ARG( args, unsigned long);
          }
          else {
            value = NG_VA_ARG( args, unsigned int);
          }

          if (Signed) {
            Signed = (value >= ( Long ?
                                 ((unsigned long)1)<<(sizeof(long)*8-1) :
                                 ((unsigned long)1)<<(sizeof(int)*8-1)) );
            if (Signed) {
              value = 0-value;
              if (!Long) value &= ((unsigned) 1<<(sizeof( int)*8-1))-1;
            }
          }

          sPtr = &NumString[sizeof(NumString)];
          *(--sPtr) = 0;
                    do {
                      *(--sPtr) = HexDigit(value % NumBase,ch);
                      value /= NumBase;
                    } while (value > 0);
                    if (Signed) *(--sPtr) = '-';
        }

      } /* switch( ParseType) */

      /* Lint warning removal. To avoid chance of initialised NULL value */
      if(sPtr == NULL)
      {
        return(0);
      }
      { /* string length */
        char *p;
        p = sPtr;
        while( *p++) MinWidth--;
      }
      FillSpaces( out_f, data, MinWidth, !LeftJustify, ZeroFill);
      count += !LeftJustify && (MinWidth>0) ? MinWidth : 0;
      while (*sPtr != 0) {
        CHECK_OUT_F(out_f, *(sPtr), data);
        sPtr++;
        count++;
      }
      FillSpaces( out_f, data, MinWidth, LeftJustify, 0);
                count += LeftJustify && (MinWidth>0)? MinWidth : 0;
                break;

    default:
      CHECK_OUT_F(out_f, ch, data);
      count++;
      break;
    } /* switch( ch) */
  } /* for */
}


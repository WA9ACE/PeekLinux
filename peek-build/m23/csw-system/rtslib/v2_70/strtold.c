/****************************************************************************/
/*  strtold v2.54                                                           */
/*  Copyright (c) 2002-2004 Texas Instruments Incorporated                  */
/****************************************************************************/
#include <stdlib.h>
#include <ctype.h>
#include <float.h>
#include <errno.h>
#include <math.h>
#include <linkage.h>
#include <stdio.h>
#include <limits.h>

static const _DATA_ACCESS double powerof10[]  = { 1.e1, 1.e2, 1.e4, 1.e8, 
						  1.e16, 1.e32 };
static const _DATA_ACCESS double digits[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

_CODE_ACCESS long double strtold(const char *st, char **endptr)
{
    long double      result = 0;
    char        cp;
    const char *fst    = st;
    int         exp    = 0;               /* EXPONENT              */
    int         count;                    /* EXPONENT CALCULATION  */
    int         value  = 0;               /* SUCCESSFUL PARSE      */
    int         sign;
    int         plus_or_minus = 0;        /* READ IN EXPONENT SIGN (+/-) */

    while (_isspace(*fst)) ++fst;  /* SKIP WHITE SPACE */
    if ((sign = ((cp = *fst) == '-')) || (cp == '+')) { ++fst; value = 1; }

    /*----------------------------------------------------------------------*/
    /* READ IN FRACTIONAL PART OF NUMBER, UNTIL AN 'E' IS REACHED.          */
    /* COUNT DIGITS AFTER DECIMAL POINT.                                    */
    /*----------------------------------------------------------------------*/
    for (; _isdigit(cp = *fst); ++fst) 
    {
       result = result * 10 + digits[cp - '0']; 
       value  = 1;
    }

    if (cp == '.') {
       while (_isdigit(cp = *++fst)) 
       {
          result = result * 10 + digits[cp - '0']; 
          value  = 1;
	  --exp;
       }
    }

    if (sign) result = -result;  /* IF NEGATIVE NUMBER, REVERSE SIGN */

    /*----------------------------------------------------------------------*/
    /* READ IN EXPLICIT EXPONENT AND CALCULATE REAL EXPONENT.               */
    /* IF EXPONENT IS BOGUS (i.e. "1.234empty" or "1.234e+mpty") RESTORE    */
    /* BOGUS EXPONENT BACK ONTO RETURNED STRING (endptr).                   */
    /*----------------------------------------------------------------------*/
    if (value && _toupper(*fst) == 'E')
    {
       if ((sign = ((cp = *++fst) == '-')) || (cp == '+'))
       {
          cp = *++fst;
          plus_or_minus = 1;
       }
 
       if (!_isdigit(cp))
       {
          if (plus_or_minus) *--fst;
          *--fst;
          goto skip_loop;
       }

       for (count = 0; _isdigit(cp); cp = *++fst)
       {
	  if ((INT_MAX - abs(exp) - (cp - '0')) / 10 > count)
          {
             count *= 10; 
	     count += cp - '0';
	  } else
	  {
	    count = INT_MAX - exp;
	    break;
	  }
       }

skip_loop:

       if (sign) exp -= count;
       else      exp += count;
    }

    /*----------------------------------------------------------------------*/
    /* ADJUST NUMBER BY POWERS OF TEN SPECIFIED BY FORMAT AND EXPONENT.     */
    /*----------------------------------------------------------------------*/
    if (result != 0.0)
    {
       if (exp > LDBL_MAX_10_EXP) 
	  { errno = ERANGE; result = (result < 0) ? -HUGE_VALL : HUGE_VALL; }
       else if (exp < LDBL_MIN_10_EXP) 
	  { errno = ERANGE; result = 0.0; }
       else if (exp < 0) {
	    for (count = 0, exp = -exp; exp; count++, exp >>= 1)
	        { if (exp & 1) result /= powerof10[count]; }
       }
       else {
	    for (count = 0; exp; count++, exp >>= 1)
	        { if (exp & 1) result *= powerof10[count]; }
       }
    }

    if (endptr) *endptr = (char *)(value ? fst : st);
    return result;
}

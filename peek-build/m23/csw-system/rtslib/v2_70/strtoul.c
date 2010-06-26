/****************************************************************************/
/*  strtoul v2.54                                                           */
/*  Copyright (c) 1993-2004  Texas Instruments Incorporated                 */
/****************************************************************************/
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <errno.h>

_CODE_ACCESS unsigned long strtoul(const char *st, char **endptr, int base)
{
    register unsigned long result = 0;
    register char          cp;
    register const char   *fst = st;
    int                    digits = 0;
    int                    minus = 0;

    while (_isspace(*fst)) ++fst;  /* SKIP WHITE SPACE */
    if (base > 36) base = 0;  

    if ((cp = *fst) == '+') cp = *++fst;
    else if ((cp = *fst) == '-') { cp = *++fst; minus = 1; }

    /*-------------------------------------------------------------------*/
    /* DETERMINE BASE IF ZERO BASE ASKED FOR.                            */
    /*-------------------------------------------------------------------*/
    switch (base)
    {
	case  0 : if      (cp != '0')           base = 10;
		  else if ((cp = *++fst) == 'x' || cp == 'X')
		     if   (_isxdigit(fst[1])) { base = 16; cp = *++fst; }
		     else                     { base = 10; cp = *--fst; }
		  else 
		  {
		     base = 8; 
		     if (cp < '0' || cp > '7')  cp = *--fst;
		  }
	          break;

        case 16 : if (cp == '0' && ((fst[1] == 'x') || (fst[1] == 'X')) &&
		      _isxdigit(fst[2]))
	             cp = *(fst += 2);
    }

    /*-------------------------------------------------------------------*/
    /* CONVERT THE NUMBER USING THE SPECIFIED BASE.                      */
    /*-------------------------------------------------------------------*/
    for (;; cp = *++fst)
    {
	register unsigned long addval;

	if (!((_isdigit(cp) && (addval = cp - '0')      < base) ||
	      (_isupper(cp) && (addval = cp - 'A' + 10) < base) ||
	      (_islower(cp) && (addval = cp - 'a' + 10) < base)))  break;

        if (result == (addval = (result * base + addval))/base) result = addval;
	else 
	{   
	    errno = ERANGE;
	    if (endptr) *endptr = (char *)st;        
	    return ULONG_MAX; 
	}

	digits++;
    }

    /*-------------------------------------------------------------------*/
    /* MARK THE END OF THE CONVERTED INPUT.                              */
    /*-------------------------------------------------------------------*/
    if (endptr) *endptr = (char *)(digits ? fst : st);

    return minus ? -result : result;
}


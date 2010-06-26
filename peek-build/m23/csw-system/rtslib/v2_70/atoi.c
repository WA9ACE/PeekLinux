/****************************************************************************/
/*  atoi v2.54                                                              */
/*  Copyright (c) 1993-2004 Texas Instruments Incorporated                  */
/****************************************************************************/
#include <stdlib.h>
#include <ctype.h>

_CODE_ACCESS int atoi(const char *st)
{
    int  result = 0;
    char cp;
    int  sign;

    while (_isspace(*st)) ++st;  /* SKIP WHITE SPACE */

    if (sign = ((cp = *st) == '-')) cp = *++st;
    else if (cp == '+')             cp = *++st;

    while (_isdigit(cp))
    {
        result *= 10;
        result += cp - '0';
        cp = *++st;
    }

    if (sign) result *= -1;
    return result;
}


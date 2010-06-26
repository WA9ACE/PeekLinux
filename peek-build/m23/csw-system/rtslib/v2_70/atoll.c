/****************************************************************************/
/*  atoll v2.54                                                             */
/*  Copyright (c) 2002-2004 Texas Instruments Incorporated                  */
/****************************************************************************/
#include <stdlib.h>
#include <ctype.h>

_CODE_ACCESS long long atoll(const char *st)
{
    long long result = 0;
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


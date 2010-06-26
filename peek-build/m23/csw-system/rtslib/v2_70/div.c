/****************************************************************************/
/*  div ldiv    v2.54                                                       */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                  */
/****************************************************************************/
#include <stdlib.h>

div_t div(int numer, int denom)
{
    div_t result;

    if (denom == 0) return;

    result.quot = numer / denom;
    result.rem  = numer - (result.quot * denom);
    return result;
}

ldiv_t ldiv(long numer, long denom)
{
    ldiv_t result;

    if (denom == 0) return;

    result.quot = numer / denom;
    result.rem  = numer - (result.quot * denom);
    return result;
}


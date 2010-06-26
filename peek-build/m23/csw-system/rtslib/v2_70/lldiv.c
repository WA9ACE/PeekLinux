/****************************************************************************/
/*  lldiv    v2.54                                                          */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                  */
/****************************************************************************/
#include <stdlib.h>
#include <limits.h>

lldiv_t lldiv(long long num, long long den)
{
    lldiv_t rv;
    
    rv.quot = num / den;
    rv.rem  = num - (rv.quot * den);

    return rv;
}

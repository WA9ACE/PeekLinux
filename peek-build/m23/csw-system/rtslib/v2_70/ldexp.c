/****************************************************************************/
/*  ldexp  v2.54                                                            */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                  */
/****************************************************************************/
#include <math.h>
#include <errno.h>

double ldexp(double x, int exp)
{
     int *ptr = (int *) &x;
     int texp = exp + ((*ptr >> 20) & 0x7FF);

     /***********************************************************************/
     /* IF RESULT UNDERFLOWS, RETURN 0.0.  IF RESULT OVERFLOWS, RETURN      */
     /* +- INFINITY.                                                        */
     /***********************************************************************/
     if (texp < 1) return 0.0; 
     if (texp > 0x7FF) { errno = ERANGE; return (x < 0) ? -HUGE_VAL:HUGE_VAL;}

     *ptr = (*ptr & 0x800FFFFF) | ((texp << 20) & (0x7FF << 20));
     return x;
}

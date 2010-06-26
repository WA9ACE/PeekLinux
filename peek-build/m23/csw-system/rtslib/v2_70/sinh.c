/****************************************************************************/
/*  sinh   v2.54                                                            */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                  */
/****************************************************************************/
#include <math.h>
#include <values.h>
#include <errno.h>

/****************************************************************************/
/*  SINH() - Hyberbolic Sine						    */
/*									    */
/*  Based on the algorithm from "Software Manual for the Elementary         */
/*  Functions", Cody and Waite, Prentice Hall 1980, chapter 12.             */
/*									    */
/*  result = (exp(x) - 1 / exp(x)) / 2					    */
/*  if x < 0, result = - result 					    */
/*  if abs(x) <=1, R = polynomial expansion; result = x + x^3 * R           */
/*  if abs(x) >1, result = (exp(x) - 1/exp(x))/2                            */
/****************************************************************************/
double sinh(double x)
{
    double g, z, q, p, r, a, b, result;
    int sign = x < 0;
    int    n;

    if ((x = fabs(x)) <= 1.0)
    {
      g = x * x;

      /************************************************************************/
      /* determine polynomial expression                                      */
      /************************************************************************/
#if BITS<=24
      p = SHP1 * g + SHP0;
      q = g + SHQ0;
#elif BITS>=25 && BITS<=40
      p = (SHP2 * g + SHP1) * g + SHP0;
      q = (g + SHQ1) * g + SHQ0;
#elif BITS>=41 && BITS<=50
      p = ((SHP3 * g + SHP2) * g + SHP1) * g + SHP0;
      q = (g + SHQ1) * g + SHQ0;
#else
      p = ((SHP3 * g + SHP2) * g + SHP1) * g + SHP0;
      q = ((g + SHQ2) * g + SHQ1) * g + SHQ0;
#endif

      result = x + x * g * (p / q);
      return sign ? -result : result; 
    }

    /**************************************************************************/
    /* check to see if overflow would occur                                   */
    /**************************************************************************/
    if (x > MAXH) { errno = ERANGE; return (sign ? -HUGE_VAL : HUGE_VAL); }

    if (x < 0) n = (int) (x * INVLOGe2 - 0.5);   /* since (int) -1.5 = -1.0 */
    else       n = (int) (x * INVLOGe2 + 0.5);

    /**************************************************************************/
    /* g = x - n * ln(2) (but more mathematically stable)                     */
    /**************************************************************************/
    g = (x - n * C3) - n * C4;

    /**************************************************************************/
    /* determine polynomial expression                                        */
    /**************************************************************************/
    z = g * g;

#if BITS <=29
    p = (EXP1 * z + EXP0) * g;
    q = EXQ1 * z + EXQ0;
#elif BITS>=30 && BITS<=42
    p = (EXP1 * z + EXP0) * g;
    q = (EXQ2 * z + EXQ1) * z + EXQ0;
#elif BITS>=43 && BITS<=56
    p = ((EXP2 * z + EXP1) * z + EXP0) * g;
    q = (EXQ2 * z + EXQ1) * z + EXQ0;
#else
    p = ((EXP2 * z + EXP1) * z + EXP0) * g;
    q = ((EXQ3 * z + EXQ2) * z + EXQ1) * z + EXQ0;
#endif

    /**************************************************************************/
    /* calculate exp(g)                                                       */
    /**************************************************************************/
    r = 0.5 + p / (q - p);

    /**************************************************************************/
    /* exp(x)/2 = exp(g) * 2 ^ n                                              */
    /**************************************************************************/
    a = ldexp(r, n);

    /**************************************************************************/
    /* exp(-x)*2 = 1 / (exp(x)/2)                                             */
    /**************************************************************************/
    b = 1.0 / a;

    /**************************************************************************/
    /* sinh(x) = (exp(x)/2 - (exp(-x)*2)/4) * sign(x)                         */
    /**************************************************************************/
    result = a - b * 0.25;
    return sign ? -result : result; 
}

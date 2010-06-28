/****************************************************************************/
/*  sqrt   v2.54                                                            */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                  */
/****************************************************************************/
#include <math.h>
#include <errno.h>
#include "values.h"

/***************************************************************************/
/* SQRT() - Square Root                                                    */
/*   Computes square root of x using a Newton-Raphson approximation for    */
/*   sqrt(1/x).  Initial value x0 = .75 * 2 ^ -(e/2), where x = a * 2 ^ e. */
/***************************************************************************/

double sqrt(double x)
{
    double x0;          /* estimate */
    int exp;

    /************************************************************************/
    /* Check to see if the input is not in the function's domain.           */
    /************************************************************************/
    if (x <= 0.0)
    {
      if (x < 0.0) errno = EDOM;
      return (0.0);
    }

    /************************************************************************/
    /* initial estimate = .75 * 2 ^ -(exp/2)                                */
    /************************************************************************/
    exp = ( (*((unsigned long *) &x) >> 20) & 0x7FF) - 1023;
    x0  = ldexp(0.75, -exp / 2);

    /************************************************************************/
    /* Refine estimate                                                      */
    /************************************************************************/
    x0 *= (1.5 - x * 0.5 * x0 * x0);
    x0 *= (1.5 - x * 0.5 * x0 * x0);
    x0 *= (1.5 - x * 0.5 * x0 * x0);
    x0 *= (1.5 - x * 0.5 * x0 * x0);
    x0 *= (1.5 - x * 0.5 * x0 * x0);
    x0 *= (1.5 - x * 0.5 * x0 * x0);

    /************************************************************************/
    /* sqrt(x) = x * sqrt(1/x)                                              */
    /************************************************************************/
    return (x0 * x);
}
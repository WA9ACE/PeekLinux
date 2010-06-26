/****************************************************************************/
/*  atan2  v2.54                                                            */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                  */
/****************************************************************************/
#include <math.h>
#include <values.h>
#include <errno.h>

/****************************************************************************/
/*  ATAN2() - Arctangent2						    */
/*									    */
/*  Based on the algorithm from "Software Manual for the Elementary         */
/*  Functions", Cody and Waite, Prentice Hall 1980, chapter 11.             */
/*									    */
/*  if x >= 0, result = atan(y / x)		  			    */
/*  if x < 0 & y >= 0, result = pi + atan(y / x)			    */
/*  if x < 0 & y < 0, result = atan (y / x) - pi			    */
/*									    */
/****************************************************************************/
double atan2(double y, double x)
{
    double g, p, q, r;
    int    sign;
    int    t = 0;
    int   ys = (y >= 0);
    int   xs = (x >= 0);
    int    n = 0;

    static const _DATA_ACCESS double a[4] = {0.0, 0.52359877559829887308, 
			             		1.57079632679489661923,
		                     		1.04719755119659774615};

    /*********************************************************************/
    /* check for error in domain                                         */
    /*********************************************************************/
    if (x == 0)
    {
       if (y == 0) { errno = EDOM; return (0.0); }
       else          return (ys ? HALFPI : -HALFPI);
    }

    /*********************************************************************/
    /* check for negative                                                */
    /*********************************************************************/
    sign = ((x = y / x) < 0.0);

    if ((x = fabs(x)) > 1.0)
    {
       x = 1.0 / x;	
       n = 2;	
       t = 1;	                         /* negate partial result */
    }

    /**********************************************************************/
    /* if (x > (2 - sqrt(3)) x = (x * sqrt(3) -1) / (sqrt(3) + x)         */
    /**********************************************************************/
    if (x > TWO_SQRT3)
    {
       x = (x * SQRTTHREE - 1.0) / (SQRTTHREE + x); 
       ++n;
    }

    /**********************************************************************/
    /* determine polynomial expression                                    */
    /**********************************************************************/
    g = x * x;

#if BITS<=24
    p = (ATP1 * g + ATP0) * g;
    q = g + ATQ0;
#elif BITS>=25 && BITS<=32
    p = (ATP1 * g + ATP0) * g;
    q = (g + ATQ1) * g + ATQ0;
#elif BITS>=33 && BITS<=50
    p = ((ATP2 * g + ATP1) * g + ATP0) * g;
    q = ((g + ATQ2) * g + ATQ1) * g + ATQ0;
#else
    p = (((ATP3 * g + ATP2) * g + ATP1) * g + ATP0) * g;
    q = (((g + ATQ3) * g + ATQ2) * g + ATQ1) * g + ATQ0;
#endif

    /*********************************************************************/
    /* calculate the result multiplied by the correct sign               */
    /*********************************************************************/
    r = ((p / q) * x + x);
    r = (t ? -r : r) + a[n];
    r = (sign ? -r : r); 

    /*********************************************************************/
    /* adjust result to be in correct quadrant                           */
    /*********************************************************************/
    if (!xs && ys)  r = (PI + r);
    if (!xs && !ys) r = (r - PI);

    return (r);
}

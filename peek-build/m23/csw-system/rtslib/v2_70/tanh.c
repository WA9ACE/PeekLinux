/****************************************************************************/
/*  tanh   v2.54                                                            */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                  */
/****************************************************************************/
#include <math.h>
#include <values.h>

/****************************************************************************/
/*  TANH() - Hyperbolic Tangent						    */
/*									    */
/*  Based on the algorithm from "Software Manual for the Elementary         */
/*  Functions", Cody and Waite, Prentice Hall 1980, chapter 13.             */
/*  This also uses the Cody and Waite algorithm for exp in chapter 6.	    */
/*									    */
/*  f = abs(x)								    */
/*  if f < ln(3) / 2,							    */
/*	g = f * f							    */
/*	R = polynomial expansion					    */
/*	result = f + f * R						    */
/*									    */
/*  if d > XBIG, result = sign(x)                                           */
/*  if f > ln(3) / 2 & f <= XBIG, result = 1 - 2 / (exp(2f) + 1)	    */
/*  if f < ln(3) / 2, R = polynomial expansion; result = f + f * R          */
/*  if x < 0, result = - result 					    */
/****************************************************************************/
double tanh(double x)
{
    double f = x, g, p, q, r, result;
    int sign = x < 0;

    f = fabs(x);

    if      (f > XBIG)     return (sign ? -1.0 : 1.0);
    else if (f > LOGe3by2) 
	 {
	    result = 1.0 - 2.0 / (exp(2.0 * f) + 1.0);
	    return sign ? -result : result; 
	 }

    /**********************************************************************/
    /* determine polynomial expression                                    */
    /**********************************************************************/
    g = f * f;

#if BITS<=24
    p = (THP1 * g + THP0) * g;
    q = g + THQ0;
#elif BITS>=25 && BITS<=36
    p = (THP1 * g + THP0) * g;
    q = (g + THQ1) * g + THQ0;
#elif BITS>=37 && BITS<=48
    p = ((THP2 * g + THP1) * g + THP0) * g;
    q = (g + THQ1) * g + THQ0;
#else
    p = ((THP2 * g + THP1) * g + THP0) * g;
    q = ((g + THQ2) * g + THQ1) * g + THQ0;
#endif

    r = p / q;

    /*****************************************************************/
    /* tanh(x) = sign * result                                       */
    /*****************************************************************/
    result = f + f * r; 
    return sign ? -result : result; 
}

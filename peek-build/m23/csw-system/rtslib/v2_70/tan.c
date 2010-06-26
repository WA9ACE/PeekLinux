/****************************************************************************/
/*  tan    v2.54                                                            */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                  */
/****************************************************************************/
#include <math.h>
#include <values.h>

/****************************************************************************/
/*  TAN() - Tangent							    */
/*									    */
/*  Based on the algorithm from "Software Manual for the Elementary         */
/*  Functions", Cody and Waite, Prentice Hall 1980, chapter 9.              */
/*									    */
/*  N = round(x * 2 / PI) 						    */
/*  f = mantissa x							    */
/*  g = f * f								    */
/*  xnum = f * p1 * g + 1						    */
/*  xden = (q2 * g + q1) * g + 1					    */
/*									    */
/*  if N is even, result = xnum / xden					    */
/*  if N is odd, result = xden / - xnum 				    */
/*									    */
/*  This will return the wrong result for x >= MAXINT * PI		    */
/****************************************************************************/
double tan(double x)
{
    double xn, f, g, xnum, xden;
    int n;

    /*************************************************************************/
    /* n = round(x * 2/PI)                                                   */
    /*************************************************************************/
    if (x < 0.0) n = (int) (x * TWOINVPI - 0.5);  /* since (int) -1.5 = -1.0 */
    else         n = (int) (x * TWOINVPI + 0.5);

    xn = (double) n;

    /*************************************************************************/
    /* f = x - xn * PI / 2 (but more mathematically stable)                  */
    /*************************************************************************/
    f = (x - xn * C5) - xn * C6;

    /*************************************************************************/
    /* determine polynomial expression                                       */
    /*************************************************************************/
    g = f * f;

#if BITS<=24
    xnum = TAP1 * g * f + f;
    xden = (TAQ2 * g + TAQ1) * g + 0.5 + 0.5;
#elif BITS>=25 && BITS<=32
    xnum = (TAP2 * g + TAP1) * g * f + f;
    xden = (TAQ2 * g + TAQ1) * g + 0.5 + 0.5;
#elif BITS>=33 && BITS<=52
    xnum = ((TAP3 * g + TAP2) * g + TAP1) * g * f + f;
    xden = ((TAQ3 * g + TAQ2) * g + TAQ1) * g + 0.5 + 0.5;
#else
    xnum = ((TAP3 * g + TAP2) * g + TAP1) * g * f + f;
    xden = (((TAQ4 * g + TAQ3) * g + TAQ2) * g + TAQ1) * g + 0.5 + 0.5;
#endif

    /*************************************************************************/
    /* if n is odd                                                           */
    /*************************************************************************/
    if (n % 2 == 0) return (xnum / xden);
    else            return (xden / -xnum);	
}

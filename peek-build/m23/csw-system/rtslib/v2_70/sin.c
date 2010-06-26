/****************************************************************************/
/*  sin    v2.54                                                            */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                  */
/****************************************************************************/
#include <math.h>
#include <values.h>

/****************************************************************************/
/*  SIN() - sine							    */
/*									    */
/*  Based on the algorithm from "Software Manual for the Elementary         */
/*  Functions", Cody and Waite, Prentice Hall 1980, chapter 8.              */
/*									    */
/*  N = round(x / PI)							    */
/*  f = x - N * PI							    */
/*  g = f * f								    */
/*  R = polynomial expansion 						    */
/*									    */
/*  result = f + f * R							    */
/*									    */
/*  if x < 0, result = - result 					    */
/*  if N is even, result = - result					    */
/*									    */
/*  This will return the wrong result for x >= MAXINT * PI		    */
/****************************************************************************/
double sin(double x)
{
    double xn, f, g, rg, result;
    int    sign = (x < 0); 
    int    n;

    x  = fabs(x);
    n  = (int) ((x * INVSPI) + 0.5);
    xn = (double) n;

    /*************************************************************************/
    /* if n is odd, negate the sign                                          */
    /*************************************************************************/
    sign ^= n % 2; 

    /*************************************************************************/
    /* f = x - xn * PI (but mathematically more stable)                      */
    /*************************************************************************/
    f = (x - xn * C1) - xn * C2;

    /*************************************************************************/
    /* determine polynomial expression                                       */
    /*************************************************************************/
    g = f * f;

#if BITS<=24
    rg = (((R4 * g + R3) * g + R2) * g + R1) * g;
#elif BITS>=25 && BITS<=32
    rg = ((((R5 * g + R4) * g + R3) * g + R2) * g + R1) * g;
#elif BITS>=33 && BITS<=50
    rg = ((((((R7*g+R6)*g+R5)*g+R4)*g+R3)*g+R2)*g+R1)*g;
#else
    rg = (((((((R8*g+R7)*g+R6)*g+R5)*g+R4)*g+R3)*g+R2)*g+R1)*g;
#endif

    result = f + f * rg;
    return sign ? -result : result; 
}

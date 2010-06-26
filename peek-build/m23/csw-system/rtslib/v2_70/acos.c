/****************************************************************************/
/*  acos   v2.54                                                            */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                  */
/****************************************************************************/
#include <math.h>
#include <values.h>
#include <errno.h>

/****************************************************************************/
/*  ACOS() - Arccosine							    */
/*									    */
/*  Based on the algorithm from "Software Manual for the Elementary         */
/*  Functions", Cody and Waite, Prentice Hall 1980, chapter 10.             */
/*									    */
/*  Y = abs(x)								    */
/*  if Y > 0.5, g = (1 - Y) / 2, Y = - 2 * sqrt(g), i = 1		    */
/*  if Y < 0.5, g = Y * Y, i = 0					    */
/*  R = polynomial expression			 			    */
/*									    */
/*  result = Y + Y * R							    */
/*									    */
/*  if x < 0, result = bi - result + bi 				    */
/*  if x > 0, result = ci + result + ci 				    */
/****************************************************************************/
double acos(double x)
{
  double p, q, g, y;
  static const _DATA_ACCESS double b[2] = {1.57079632679489661923, 
					   0.78539816339744830962};
  static const _DATA_ACCESS double c[2] = {0.0, 0.78539816339744830962};
  int i = 1;

  /*************************************************************************/
  /* acos(x) = acos(-x)                                                    */  
  /*************************************************************************/
  if ((y = fabs(x)) > 0.5)
  {
     /**********************************************************************/
     /* check if x is out of domain                                        */
     /**********************************************************************/
     if (y > 1.0) { errno = EDOM; return (0.0); }

     i = 0;
     g = (1.0 - y) * 0.5;
     y = -2.0 * sqrt(g);
  }
  else g = y * y;

  /*************************************************************************/
  /* determine polynomial expansion                                        */
  /*************************************************************************/
#if BITS<=24
  p = (ASP2 * g + ASP1) * g;
  q = (g + ASQ1) * g + ASQ0;
#elif BITS>=25 && BITS<=36
  p = ((ASP3 * g + ASP2) * g + ASP1) * g;
  q = ((g + ASQ2) * g + ASQ1) * g + ASQ0;
#elif BITS>=37 && BITS<=48
  p = (((ASP4 * g + ASP3) * g + ASP2) * g + ASP1) * g;
  q = (((g + ASQ3) * g + ASQ2) * g + ASQ1) * g + ASQ0;
#else
  p = ((((ASP5 * g + ASP4) * g + ASP3) * g + ASP2) * g + ASP1) * g;
  q = ((((g + ASQ4) * g + ASQ3) * g + ASQ2) * g + ASQ1) * g + ASQ0;
#endif

  return (x < 0) ? ((b[i] + y + y * p / q) + b[i])
		 : ((c[i] - y - y * p / q) + c[i]);
}

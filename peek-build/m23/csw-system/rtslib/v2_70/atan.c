/****************************************************************************/
/*  atan   v2.54                                                            */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                  */
/****************************************************************************/
#include <math.h>
#include <values.h>

/****************************************************************************/
/*  ATAN() - Arctangent							    */
/*									    */
/*  Based on the algorithm from "Software Manual for the Elementary         */
/*  Functions", Cody and Waite, Prentice Hall 1980, chapter 11.             */
/*									    */
/*  if x > 1, x = 1 / x 						    */
/*  if x > 2 - sqrt(3), x = (x * sqrt(3) - 1) / (sqrt(3) + x)		    */
/*  g = x * x								    */
/*  R = polynomial expression						    */
/*									    */
/*  result = (t * (x + x * R) + an) * s					    */
/****************************************************************************/
double atan(double x)
{
    double g, p, q;
    int    sign = x < 0.0;       /* REMEMBER SIGN (1 == negative) */
    int    t = 0;                /* PARTIAL RESULT SIGN FLAG      */
    int    n = 0;
    double result; 

    static const _DATA_ACCESS double a[4] = {0.0, 0.52359877559829887308, 
		     	     		        1.57079632679489661923,
		                     		1.04719755119659774615};

    if ((x = fabs(x)) > 1.0)
    {
	x = 1.0 / x;	
	n = 2;	

	/******************************************************************/
	/* the partial result needs to be negated                         */
	/******************************************************************/
	t = 1;    
    }

    /**********************************************************************/
    /* for x > (2 - sqrt(3)  )                                            */
    /**********************************************************************/
    if (x > TWO_SQRT3)			
    {
	/******************************************************************/
	/* x = (x * sqrt(3) -1)	/ (sqroot(3) + x)                         */
	/******************************************************************/
	x = (x * SQRTTHREE - 1.0) / (SQRTTHREE + x);
	++n; 			
    }

    /*********************************************************************/
    /* determine polynomial expression                                   */
    /*********************************************************************/
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

    /**********************************************************************/
    /* calculate the result multiplied by the correct sign                */
    /**********************************************************************/
    result = ((p / q) * x + x); 
    result = (t ? -result : result) + a[n];
    return sign ? -result : result; 
}

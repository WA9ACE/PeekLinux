/****************************************************************************/
/*  FABS   v2.54                                                            */
/*  Copyright (c) 1993-2004 Texas Instruments Incorporated                  */
/****************************************************************************/

#include <math.h>

/****************************************************************************/
/*  FABS() - Floating Point Absolute Value				    */
/*									    */
/*  The definition below is NOT a recursive definition!  The compiler       */
/*  generates calls to fabs() inline, using a single ABS instruction.       */
/****************************************************************************/
double fabs(double x)
{
    return fabs(x);        /* This will be generated inline by the compiler */
}

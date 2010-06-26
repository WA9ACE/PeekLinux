/****************************************************************************/
/*  difftime v2.54                                                          */
/*  Copyright (c) 1996-2004 Texas Instruments Incorporated                  */
/****************************************************************************/
#undef _INLINE
#include <time.h> 

_CODE_ACCESS double difftime(time_t time1, time_t time0)
{
    /*----------------------------------------------------------------------*/
    /* THE FOLLOWING CONVERSION INSURES THAT ANY POSSIBLE DIFFERENCE IN THE */
    /* RANGE OF UNSIGNED LONG IS PROPERLY REPRESENTED IN THE RESULT.        */
    /*----------------------------------------------------------------------*/
    return ((time1 > time0) ?  (double)(time1 - time0)
	                    : -(double)(time0 - time1));
}

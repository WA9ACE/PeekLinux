/*****************************************************************************/
/*  TIME.C v2.54                                                             */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                   */
/*****************************************************************************/
#include <time.h> 

extern _CODE_ACCESS time_t HOSTtime();

_CODE_ACCESS time_t time(time_t *timer) 
{
   time_t result;

   result = (time_t)HOSTtime();
   if(timer) *timer = result;
   return(result);
}


/****************************************************************************/
/*  gmtime v2.54                                                            */
/*  Copyright (c) 1993-2004  Texas Instruments Incorporated                 */
/****************************************************************************/
#include <time.h>

_CODE_ACCESS struct tm *gmtime(const time_t *timer)
{
    time_t gtime = _tz.timezone; /* DIFFERENCE BETWEEN CURRENT TIME ZONE    */
				 /* AND GMT IN SECONDS                      */

    if (timer) gtime += *timer;
    return (localtime(&gtime));
}

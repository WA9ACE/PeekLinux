/****************************************************************************/
/*  asctime v2.54                                                           */
/*  Copyright (c) 1993-2004  Texas Instruments Incorporated                 */
/****************************************************************************/
#include <time.h> 

extern _CODE_ACCESS int sprintf(char *_string, const char *_format, ...);

static _DATA_ACCESS const char *const day[7]  = {
		      "Sun","Mon","Tue","Wed","Thu","Fri","Sat" };
static _DATA_ACCESS const char *const mon[12] = {
		     "Jan","Feb","Mar","Apr","May","Jun", 
                     "Jul","Aug","Sep","Oct","Nov","Dec" };

_CODE_ACCESS char *asctime(const struct tm *timeptr)
{
    struct tm   tptr = *timeptr;
    static _DATA_ACCESS char result[26];

    mktime(&tptr);      /* MAKE SURE THIS A SANE TIME */

    sprintf(result, "%s %s%3d %02d:%02d:%02d %d\n",
		    day[tptr.tm_wday], mon[tptr.tm_mon], tptr.tm_mday, 
		    tptr.tm_hour, tptr.tm_min, tptr.tm_sec,
		    tptr.tm_year + 1900);

    return (result);
}

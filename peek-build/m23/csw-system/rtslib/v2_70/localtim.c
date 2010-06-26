/****************************************************************************/
/*  localtime v2.54                                                         */
/*  Copyright (c) 1993-2004  Texas Instruments Incorporated                 */
/****************************************************************************/
#include <time.h>
#include <limits.h>

#define SECS_IN_MIN (time_t)60
#define MINS_IN_HR  (time_t)60
#define HRS_IN_DAY  (time_t)24
#define SECS_IN_HR  (SECS_IN_MIN * MINS_IN_HR)
#define SECS_IN_DAY (SECS_IN_HR * HRS_IN_DAY)
 
#define LEAPYEAR(y) (    (y+1900) % 4   == 0                               \
			 && ((y+1900) % 100 != 0 || (y+1900) % 400 == 0))

#define DAYS_IN_YR(y) ((time_t)365 + LEAPYEAR(y))
 
_CODE_ACCESS struct tm *localtime(const time_t *timer)
{
    static _DATA_ACCESS struct tm local;
    time_t ltime  = timer ? *timer : 0;
 
    local.tm_sec  = 0;
    local.tm_min  = 0;
    local.tm_hour = 0;
    local.tm_mday = 1;
    local.tm_mon  = 0;
    local.tm_year = 0;
 
    if (timer == 0 || ltime == (time_t)-1) return &local;
 
#if INT_MAX <= 32767
    /*------------------------------------------------------------------*/
    /* MAKE SURE THE NUMBER OF SECONDS SINCE Jan 1, 1900 CAN BE         */
    /* REPRESENTED IN SIGNED INTS.                                      */
    /*------------------------------------------------------------------*/
    local.tm_sec   =  ltime % SECS_IN_MIN;
    local.tm_min   = (ltime / SECS_IN_MIN) % MINS_IN_HR;
    local.tm_hour  = (ltime / SECS_IN_HR)  % HRS_IN_DAY;
 
    /*------------------------------------------------------------------*/
    /* CONVERT ltime TO NUMBER OF DAYS                                  */
    /*------------------------------------------------------------------*/
    ltime /= SECS_IN_DAY;
 
    /*------------------------------------------------------------------*/
    /* TO DETERMINE THE YEAR, INSTEAD OF DIVIDING BY 365, DO A SUBTRACT */
    /* LOOP THAT ACCOUNTS FOR LEAP YEARS.                               */
    /*------------------------------------------------------------------*/
    {
	int year = 0;
	while (ltime >= DAYS_IN_YR(year))
	{
	    ltime -= DAYS_IN_YR(year);
	    ++year;
	}
     
	local.tm_year  = year;
	local.tm_mday += ltime;
    }
 
#else
    /*------------------------------------------------------------------*/
    /* MAKE SURE THE NUMBER OF SECONDS SINCE Jan 1, 1900 CAN BE         */
    /* REPRESENTED IN SIGNED INTS.                                      */
    /*------------------------------------------------------------------*/
    if ((int)ltime < 0)
    {
	local.tm_sec  = ltime % 60; 
	local.tm_min  = ltime / 60; 
    }
    else local.tm_sec = ltime;
#endif
 
    /*------------------------------------------------------------------*/
    /* MAKE VALUES IN local INTO A VALID TIME.                          */
    /*------------------------------------------------------------------*/
    mktime(&local);
    return &local;
}

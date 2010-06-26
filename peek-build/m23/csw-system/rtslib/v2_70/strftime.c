/****************************************************************************/
/*  strftime v2.54                                                          */
/*  Copyright (c) 1993-2004  Texas Instruments Incorporated                 */
/****************************************************************************/
#include <time.h>
#include <string.h>

extern int  sprintf(char *_string, const char *_format, ...);

static _DATA_ACCESS const char *const fday[] =
           { "Sunday","Monday","Tuesday","Wednesday",
	     "Thursday","Friday","Saturday" };

static _DATA_ACCESS const char *const fmon[] =
	   { "January","February","March","April","May",
	     "June", "July","August","September",
	     "October","November", "December" };

_CODE_ACCESS size_t strftime(char            *out,
	                     size_t           maxsize, 
	                     const char      *format,
	                     const struct tm *time)
{
    struct tm tt = *time; 
    int       count = 0;
    char      buf[37];

    mktime(&tt);     /* MAKE SURE THIS IS A LEGITIMATE TIME */

    for (;;)
    {
	while ((count < maxsize) && *format && (*format != '%'))
	{
	    count++;
	    *out++ = *format++;
	}

	if (count >= maxsize) return 0;
	if (*format == '\0')  { *out = 0; return count; }

	switch (*++format)
	{
	    case '%' : strcpy(buf, "%");                              break;
	    case 'z' : strcpy(buf, _tz.dstname);                      break;
	    case 'Z' : strcpy(buf, _tz.tzname);                       break;

	    case 'A' : strcpy (buf, fday[tt.tm_wday]);                break;
	    case 'a' : strncpy(buf, fday[tt.tm_wday], 3); buf[3] = 0; break;
	    case 'B' : strcpy (buf, fmon[tt.tm_mon]);                 break;
	    case 'b' : strncpy(buf, fmon[tt.tm_mon], 3);  buf[3] = 0; break;
	    case 'c' : sprintf(buf, "%s %s%3d %02d:%02d:%02d %d",
			       fday[tt.tm_wday], fmon[tt.tm_mon],
			       tt.tm_mday, tt.tm_hour, tt.tm_min,
			       tt.tm_sec, tt.tm_year + 1900);         break;

	    case 'I' : sprintf(buf, "%02d", ((tt.tm_hour != 0) &&
			                     (tt.tm_hour != 12)) ? 
			                    tt.tm_hour % 12 : 12);    break;
	    case 'H' : sprintf(buf, "%02d", tt.tm_hour);              break;
	    case 'M' : sprintf(buf, "%02d", tt.tm_min);               break;
	    case 'S' : sprintf(buf, "%02d", tt.tm_sec);               break;
	    case 'j' : sprintf(buf, "%03d", tt.tm_yday + 1);          break;
	    case 'd' : sprintf(buf, "%02d", tt.tm_mday);              break;
	    case 'e' : sprintf(buf, "%2d",  tt.tm_mday);              break;
	    case 'w' : sprintf(buf, "%1d",  tt.tm_wday);              break;
	    case 'm' : sprintf(buf, "%02d", tt.tm_mon + 1);           break;
	    case 'y' : sprintf(buf, "%02d", tt.tm_year % 100);        break;
	    case 'Y' : sprintf(buf, "%4d", 1900 + tt.tm_year);        break;

	    case 'p' : strcpy(buf, tt.tm_hour >= 12 ? "PM":"AM");     break;

	    case 'W' : sprintf(buf, "%02d", tt.tm_yday / 7 +
			       (tt.tm_yday % 7 >= (tt.tm_wday + 6) % 7));
	    							      break;
	    case 'U' : sprintf(buf, "%02d", tt.tm_yday / 7 +
			       (tt.tm_yday % 7 >= tt.tm_wday));       break;

	    case 'x' : sprintf(buf, "%3s %d, %d", fmon[tt.tm_mon], 
			       tt.tm_mday, tt.tm_year + 1900);        break;
	    case 'X' : sprintf(buf, "%02d:%02d:%02d", tt.tm_hour, 
			       tt.tm_min, tt.tm_sec);                 break;
	}

	/*-------------------------------------------------------------*/
	/* IF THIS STRING WILL FIT IN BUFFER, APPEND IT.               */
	/*-------------------------------------------------------------*/
	if ((count += strlen(buf)) > maxsize) return 0;
	strcpy(out, buf);
	out += strlen(buf);
	format++;
    }
}

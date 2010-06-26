/****************************************************************************/
/*  strcoll v2.54                                                           */
/*  Copyright (c) 1993-2004  Texas Instruments Incorporated                 */
/****************************************************************************/
#include <string.h>

_CODE_ACCESS int   strcoll(const char *string1, const char *string2)
{
     register const char *r1 = string1 - 1;
     register const char *r2 = string2 - 1;
     register char cp;
 
     while ((*++r2 == (cp = *++r1)) && cp);
     return *r1 - *r2;
}

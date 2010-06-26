/****************************************************************************/
/*  strspn  v2.54                                                           */
/*  Copyright (c) 1993-2004  Texas Instruments Incorporated                 */
/****************************************************************************/
#include <string.h>

_CODE_ACCESS size_t strspn(register const char *string, const char *chs)
{
     size_t count = 0;

     for (;;)            /* FOR EACH CHARACTER IN STRING */
     {
	register char  ch    = *string++;
        register char *check = (char *)chs;
	register char  tch   = *check;                

        while (tch)      /* FOR EACH CHARACTER IN LOOKUP STRING */
        {
	   if (ch == tch) break;
           tch = *++check;
        }

        if (!ch || ch != tch) return(count);
	count++;
     }
}

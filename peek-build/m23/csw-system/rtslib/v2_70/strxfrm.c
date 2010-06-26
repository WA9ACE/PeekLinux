/****************************************************************************/
/*  strxfrm v2.54                                                           */
/*  Copyright (c) 1993-2004  Texas Instruments Incorporated                 */
/****************************************************************************/
#include <stddef.h>
#include <string.h>

_CODE_ACCESS size_t strxfrm(register char *to,
			    register const char *from,
			    register size_t n)
{
     register int count = 0;     
     while (*from++) count++;                       /* GET LENGTH OF STRING */
     from -= count;
     from--;

     if (n) while ((*to++ = *from++) && --n);       /* COPY STRING          */

     return (count);
}

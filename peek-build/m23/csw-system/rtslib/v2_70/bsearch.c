/****************************************************************************/
/*  bsearch v2.54                                                           */
/*  Copyright (c) 1993-2004  Texas Instruments Incorporated                 */
/****************************************************************************/
#include <stdlib.h>

_CODE_ACCESS void *bsearch(
	      register const void *key,     /* ITEM TO SEARCH FOR           */
              register const void *base,    /* POINTER TO ARRAY OF ELEMENTS */
              size_t nmemb,                 /* NUMBER OF ELEMENTS TO SORT   */
              size_t size,                  /* SIZE IN BYTES OF EACH ITEM   */
              int (*compar)(const void *,const void *)) /* COMPARE FUNCTION */

{
   register int i       = 0;             /* left scan index  */
   register int j       = nmemb - 1;     /* right scan index */

   while( i <= j )
   {
      register int pivot = (j + i) / 2;            
      int result = (*compar) (key, (char *)base + (pivot * size));
      if (result == 0) return((char *)base + (pivot * size));
      if (result < 0) j = pivot - 1;
		 else i = pivot + 1;
   }

   return ((void *)0);   /* IF WE GET TO HERE, WE FAILED */
}



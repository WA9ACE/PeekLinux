/****************************************************************************/
/*  qsort v2.54                                                             */
/*  Copyright (c) 1993-2004  Texas Instruments Incorporated                 */
/****************************************************************************/
#include <stdlib.h>

static void swap_item(register char *src, 
                      register char *dest, 
                      register size_t size);

_CODE_ACCESS void qsort(void *base, size_t nmemb, size_t size, 
			int (*compar)(const void *, const void *))
{
   register char     *basep  = base;         /* POINTER TO ARRAY OF ELEMENTS */
   register unsigned  i      = 0;            /* left scan index  */
   register unsigned  j      = nmemb - 1;    /* right scan index */
   register unsigned  pivot  = (nmemb / 2);
   register char     *pivp   = basep + (pivot * size);

   if (nmemb <= 1) return;

   while( i < j )
   {
      while( (*compar) (basep + (i * size), pivp) < 0 ) ++i;
      while( (*compar) (basep + (j * size), pivp) > 0 ) --j;
      if( i < j )
      {
	 swap_item(basep + (i * size), basep + (j * size), size);
	 if     ( pivot == i ) { pivot = j; pivp = basep + (pivot * size); }
	 else if( pivot == j ) { pivot = i; pivp = basep + (pivot * size); } 
	 ++i; --j;
      }
      else if ( i == j ) { ++i; --j;  break; }
   }

   if( j > 0)       qsort(basep, j + 1, size, compar);
   if( i < nmemb-1) qsort(basep + (i * size), nmemb - i, size, compar);
}


static void swap_item(register char *src, 
                      register char *dest, 
                      register size_t size)
{
    while (size--)
    {
        register int temp = *src;       
        *src++            = *dest;
        *dest++           = temp;
    }
}


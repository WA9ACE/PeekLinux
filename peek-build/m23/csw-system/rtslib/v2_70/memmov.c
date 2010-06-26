/****************************************************************************/
/*  memmov.c  v2.54                                                         */
/*  Copyright (c) 1996-2004 Texas Instruments Incorporated                  */
/****************************************************************************/
#include <string.h>

void *memmove(void *s1, const void *s2, size_t n)
{
   if (s2 > s1)
      return memcpy(s1, s2, n);
   else
   {
      unsigned char *st1 = (unsigned char *)s1;
      unsigned char *st2 = (unsigned char *)s2;
      size_t         ln;
 
      st1 += n;
      st2 += n;
      for (ln = 0; ln < n; ln++) *--st1 = *--st2;
   }

   return s1;
}


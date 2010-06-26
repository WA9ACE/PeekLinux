/****************************************************************************/
/*  strpbrk v2.54                                                           */
/*  Copyright (c) 1993-2004  Texas Instruments Incorporated                 */
/****************************************************************************/
#include <string.h>

_CODE_ACCESS char *strpbrk(const char *s1, const char *s2)
{
   for (; *s1; s1++)
   {
      const char *check = s2;
 
      while (*check)
         if (*check++ == *s1) return (char *)s1;
   }
   return NULL;
}

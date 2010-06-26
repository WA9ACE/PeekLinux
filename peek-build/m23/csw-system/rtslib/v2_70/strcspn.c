/****************************************************************************/
/*  strcspn v2.54                                                           */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                  */
/****************************************************************************/
#include <string.h>

_CODE_ACCESS size_t strcspn(const char *s1, const char *s2)
{
   const char *check;
   size_t      count = 0;
 
   /*---------------------------------------------------------------------*/
   /* FOR EACH CHAR IN s1                                                 */
   /*---------------------------------------------------------------------*/
   for (; *s1; s1++, count++)
   {
      for (check = s2; *check;)
	 /*---------------------------------------------------------------*/
	 /* QUIT WHEN FIND A CHAR IN s1 THAT'S EQUAL TO A CHAR IN s2      */
	 /*---------------------------------------------------------------*/
	 if (*s1 == *check++) return count;
   }
   return count;
}

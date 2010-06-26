/****************************************************************************/
/*  memcpy.c  v2.54                                                         */
/*  Copyright (c) 1996-2004 Texas Instruments Incorporated                  */
/****************************************************************************/
#include <string.h>

void *memcpy(void *s1, const void *s2, register size_t n)
{
   return memcpy(s1, s2, n);       /* NOTE:  MEMCPY() IS A BUILTIN FUNCTION */
}

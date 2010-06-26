/*****************************************************************************/
/*  MEMCCPY.C  v2.54                                                         */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                   */
/*****************************************************************************/
#include "stdlib.h"

extern _CODE_ACCESS char *memccpy(char *dest, const char *src, int ch, int cnt);

/*--------------------------------------------------------------------------*/
/* PERFORM SAME OPERATION AS MEMCPY, ABORT IF "ch" is COPIED.               */
/* RETURN POINTER TO BYTE AFTER ch IS FOUND, OR NULL IF NOT FOUND           */
/*--------------------------------------------------------------------------*/
_CODE_ACCESS char *memccpy(char *dest, const char *src, int ch, int cnt)
{
   if (cnt <= 0) return NULL;
   do
      if ((*dest++ = *src++) == ch) return (dest);
   while (--cnt != 0);

   return NULL;
}

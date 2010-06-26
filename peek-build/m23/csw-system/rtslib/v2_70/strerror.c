/****************************************************************************/
/*  strerror  v2.54                                                         */
/*  Copyright (c) 1993-2004  Texas Instruments Incorporated                 */
/****************************************************************************/
#include <errno.h>
#include <string.h>

_CODE_ACCESS char *strerror(int errnum)
{
   switch(errnum)
   {
      case 0      :  return ("No error");
      case EDOM   :  return ("Domain error");
      case ERANGE :  return ("Range error");
      case ENOENT :  return ("No such file or directory");
      case EFPOS  :  return ("File positioning error");
   }

   return ("Unknown error");
}

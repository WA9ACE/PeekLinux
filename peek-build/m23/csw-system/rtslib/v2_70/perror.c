/*****************************************************************************/
/*  PERROR.C v2.54                                                           */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                   */
/*****************************************************************************/

/*****************************************************************************/
/* Functions:                                                                */
/*    PERROR   -  Report any system errors to stderr                         */
/*****************************************************************************/
#include <stdio.h>
#include <errno.h>


/*****************************************************************************/
/* PERROR   -  Report any system errors to stderr                            */
/*****************************************************************************/
_CODE_ACCESS void perror(const char *_s)
{
   /*------------------------------------------------------------------------*/
   /* Local variables                                                        */
   /*------------------------------------------------------------------------*/
   char *buf;

   /*------------------------------------------------------------------------*/
   /* If _s is not NULL, prepend it to any error message.                    */
   /*------------------------------------------------------------------------*/
   if (_s && *_s)
   {
      fputs(_s, stderr);
      fputs(": ", stderr);
   }

   switch(errno)
   {
      case 0      :  buf = "No error";                  break;
      case EDOM   :  buf = "Domain error";              break;
      case ERANGE :  buf = "Range error";               break;
      case ENOENT :  buf = "No such file or directory"; break;
      case EFPOS  :  buf = "File positioning error";    break;
      default     :  buf = "Unknown error";             break;
   }

   fputs(buf, stderr);
   fputc('\n', stderr);
}


/*****************************************************************************/
/*  TMPNAM.C v2.54                                                           */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                   */
/*****************************************************************************/

/*****************************************************************************/
/* Functions:                                                                */
/*    TMPNAM   -  Generate a unique filename                                 */
/*    _GETNAME -  Generate the character string for the filename             */
/*****************************************************************************/
#include <stdio.h>
#include "file.h"
#include <string.h>

static char *_getname(int cnt, char *result);


/*****************************************************************************/
/* TMPNAM   -  Generate a unique filename                                    */
/*                                                                           */
/*    This function generates a unique filename, and can be called up to     */
/*    TMP_MAX times.                                                         */
/*                                                                           */
/*****************************************************************************/
_CODE_ACCESS char *tmpnam(char *_s)
{
   /*------------------------------------------------------------------------*/
   /* Local variables                                                        */
   /*------------------------------------------------------------------------*/
   static _DATA_ACCESS unsigned int  counter = 0;
   static _DATA_ACCESS char tfname[L_tmpnam];
   int  fd;
 
   /*------------------------------------------------------------------------*/ 
   /* Get a filename from _GETNAME                                           */
   /*------------------------------------------------------------------------*/ 
   _getname(counter++, tfname);

   /*------------------------------------------------------------------------*/ 
   /* Check to see if the filename exists.  Keep getting filenames until     */
   /* a unique one is found, or this function has reached its limit.         */
   /*------------------------------------------------------------------------*/ 
   while(((fd=open(tfname, O_RDONLY, 0666)) >= 0) && (counter < TMP_MAX))
   {
      close(fd);
      _getname(counter++, tfname);
   }
 
   if(counter >= TMP_MAX) return (NULL);
 
   /*------------------------------------------------------------------------*/ 
   /* If _S is not NULL, store the new filename in it.                       */
   /*------------------------------------------------------------------------*/ 
   if(_s)
   {
      strcpy(_s, tfname);
      return (_s);
   }
 
   return (tfname);
}


/*****************************************************************************/
/* _GETNAME -  Generate the character string for the filename                */
/*                                                                           */
/*    This function takes a number passed to it by TMPNAM, and generates a   */
/*    filename from it.  It returns a pointer to the filename.               */
/*                                                                           */
/*****************************************************************************/
static char *_getname(int cnt, char *result)
{
   /*------------------------------------------------------------------------*/
   /* Local variables                                                        */
   /*------------------------------------------------------------------------*/
   char *end   = result + L_tmpnam -1;
   char *spos;
 
   strcpy(result, P_tmpdir);
   spos = (result + strlen(result));
 
   *(spos++) = 'T';
 
   for(;cnt; cnt /= 10) *(spos++) = ((cnt % 10)+ '0');
 
   for(;spos < end; *(spos++)='0');
   *(end--) = '\0';
 
   return (result);
}


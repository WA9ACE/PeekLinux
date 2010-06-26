/*****************************************************************************/
/*  470MSG.C v2.54                                                           */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                   */
/*****************************************************************************/

/*****************************************************************************/
/* Bottom level data transfer routines for host communication with the       */
/* target.                                                                   */
/*                                                                           */
/* Functions:                                                                */
/*  writemsg()  -  Sends the passed data and parameters on to the host.      */
/*  readmsg()   -  Reads the data and parameters passed from the host.       */
/*****************************************************************************/
#include <stdio.h>
#include <trgcio.h>

unsigned int _CIOBUF_[CIOBUFSIZ/sizeof(unsigned int)];


/***************************************************************************/
/*                                                                         */
/*  WRITEMSG()  -  Sends the passed data and parameters on to the host.    */
/*                                                                         */
/***************************************************************************/
void writemsg(unsigned char  command,
              register const unsigned char *parm,
              register const          char *data,
              unsigned int            length)
{
   unsigned char *p = (unsigned char *)(_CIOBUF_+1);
   unsigned int i;

   /***********************************************************************/
   /* THE LENGTH IS WRITTEN AS A TARGET INT                               */
   /***********************************************************************/ 
   _CIOBUF_[0] = length;
   
   /***********************************************************************/ 
   /* THE COMMAND IS WRITTEN AS A TARGET BYTE                             */
   /***********************************************************************/ 
   *p++ = command;
   
   /***********************************************************************/ 
   /* PACK THE PARAMETERS AND DATA SO THE HOST READS IT AS BYTE STREAM    */
   /***********************************************************************/ 
   for (i = 0; i < 8; i++)      PACKCHAR(*parm++, p, i);
   for (i = 0; i < length; i++) PACKCHAR(*data++, p, i+8);
   
   /***********************************************************************/
   /* THE BREAKPOINT THAT SIGNALS THE HOST TO DO DATA TRANSFER            */
   /***********************************************************************/
#if defined(EMBED_CIO_BP)
   __asm("         .global C$$IOE$$");
#if defined(__32bis__)
   __asm("C$$IOE$$:.word 0xDEFED0FE");
#else
   __asm("	 .align  4");
#if defined(__big_endian__)
   __asm("C$$IOE$$:.half 0xDEFE");
#else
   __asm("C$$IOE$$:.half 0xD0FE");
#endif /* __big_endian__ */
#endif /* __32bis__      */

#else  /* !EMBED_CIO_BP */
   __asm("	  .global C$$IO$$");
   __asm("C$$IO$$: nop");
#endif
}



/***************************************************************************/
/*                                                                         */
/*  READMSG()   -  Reads the data and parameters passed from the host.     */
/*                                                                         */
/***************************************************************************/
void readmsg(register unsigned char *parm,
	     register char          *data)
{
   unsigned char *p = (unsigned char *)(_CIOBUF_+1);
   unsigned int   i;
   unsigned int   length;
   
   /***********************************************************************/
   /* THE LENGTH IS READ AS A TARGET INT                                  */
   /***********************************************************************/
   length = _CIOBUF_[0];
    
   /***********************************************************************/
   /* UNPACK THE PARAMETERS AND DATA                                      */
   /***********************************************************************/
   for (i = 0; i < 8; i++) *parm++ = UNPACKCHAR(p, i);
   if (data != NULL) 
      for (i = 0; i < length; i++) *data++ = UNPACKCHAR(p, i+8);
}

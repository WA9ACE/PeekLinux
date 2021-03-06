/*
*******************************************************************************
*
*      COPYRIGHT (C) 2000 BY ERICSSON EUROLAB DEUTSCHLAND GmbH
*      90411 NUERNBERG, GERMANY, Tel Int + 49 911 5217 100
*
*      The program(s) may be used and/or copied only with the
*      written permission from Ericsson or in accordance
*      with the terms and conditions stipulated in the agreement or
*      contract under which the program(s) have been supplied.
*
*******************************************************************************
*
*      File             : ucs_functions.c
*      Author           : EEDN/RV Matthias Doerbecker
*      Tested Platforms : SUN Solaris
*                         due to the different encoding in DOS, the characters
*                         of the Latin-1 Supplement are not supported in DOS!!
*      Description      : Functions for supporting the 
*                         Universal Multiple-Octet Coded Character Set (UCS)
*                         accoring to ISO/IEC 10646-1
*
*      Revision history
*
*      $Log: $
*
*******************************************************************************
*/

/*
*******************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
*******************************************************************************
*/

#include "l1_macro.h"
#include "l1_confg.h"

#if (L1_GTT == 1)

  #include "l1_types.h"
  #include "ucs_functions.h"
  #include "ctm_defines.h"    /* definition of BITS_PER_SYMB */
  #include "fifo.h"
  #include "ctm_typedefs.h"
  #include <stdio.h>
  #include <stdlib.h> /* For exit function */

  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
    #include "rvt_gen.h"
    #include "l1_trace.h"
  #endif

  /* Definition of row 00 (Basic Latin & Latin-1 Supplement)      */
  /* of the Basic Multilingual Plane accoring to ISO/IEC 10646-1  */
  /* (note that not all characters are implemented yet -- missing */
  /* characters are coded a \0)                                   */
  /* The Latin-1 Supplement (codes 80...FF) is not supported on a */
  /* DOS platform due to the different encoding.                  */

  static const char ucsCharTab[] = 
  "\0\0\0\0\0\0\0\a\b\0\n\0\0\r\0\0"    /* 00...0F */
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"    /* 10...1F */
  " !\"#$%&'()*+,-./"                   /* 20...2F */
  "0123456789:;<=>?"                    /* 30...3F */
  "@ABCDEFGHIJKLMNO"                    /* 40...4F */
  "PQRSTUVWXYZ[\\]^_"                   /* 50...5F */
  "`abcdefghijklmno"                    /* 60...6F */
  "pqrstuvwxyz{|}~\0"                   /* 70...7F */
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"    /* 80...8F */
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"    /* 90...9F */
  " ������������-��"                    /* A0...AF */
  "����������������"                    /* B0...BF */
  "����������������"                    /* C0...CF */
  "����������������"                    /* D0...DF */
  "����������������"                    /* E0...EF */
  "����������������";                   /* F0...FF */


  /****************************************************************************/
  /* convertChar2UCScode()                                                    */
  /* *********************                                                    */
  /* Conversion from character into UCS code                                  */
  /* (Universal Multiple-Octet Coded Character Set, Row 00                    */
  /* of the Multilingual plane according to ISO/IEC 10646-1).                 */
  /* This routine only handles characters in the range 0...255 since that is  */
  /* all that is required for the demonstration of Baudot support.            */
  /*                                                                          */
  /* input variables:                                                         */
  /* - inChar       character that shall be converted                         */
  /*                                                                          */
  /* return value:  UCS code of the input character                           */
  /*                or 0x0016 <IDLE> in case that inChar is not valid         */
  /*                (e.g. inChar=='\0')                                       */
  /*                                                                          */
  /* Matthias Doerbecker, Ericsson Eurolab Deutschland (EED/N/RV), 2000/09/18 */
  /****************************************************************************/

  UWORD16 convertChar2UCScode(char inChar)
  {
    UWORD16  ucsCode = 0x0016; 
    if (inChar != '\0')
      {
        /* determine the character's UCS code index */
        ucsCode = 0;
        while((inChar!=ucsCharTab[ucsCode]) && (ucsCode < (1<<BITS_PER_SYMB)))
          ucsCode++;
      }
    if (ucsCode==(1<<BITS_PER_SYMB))
      ucsCode = 0x0016;
    
    return ucsCode;
  }



  /****************************************************************************/
  /* convertUCScode2char()                                                    */
  /* *********************                                                    */
  /* Conversion from UCS code into character                                  */
  /* This routine only handles characters in the range 0...255 since that is  */
  /* all that is required for the demonstration of Baudot support.            */
  /*                                                                          */
  /* input variables:                                                         */
  /* - ucsCode      UCS code index,                                           */
  /*                must be within the range 0...255                          */
  /*                                                                          */
  /* return value:  character (or '\0' if ucsCode is not valid)               */
  /*                                                                          */
  /* Matthias Doerbecker, Ericsson Eurolab Deutschland (EED/N/RV), 2000/09/18 */
  /****************************************************************************/

  char convertUCScode2char(UWORD16 ucsCode)
  {
    char outChar = '\0';
    
    if (ucsCode < (1<<BITS_PER_SYMB))
      outChar = ucsCharTab[ucsCode];
    
    return outChar;
  }

  /****************************************************************************/
  /* transformUCS2UTF()                                                       */
  /* ******************                                                       */
  /* Transformation from UCS code into UTF-8. UTF-8 is a sequence consisting  */
  /* of 1, 2, 3, or 5 octets (bytes). See ISO/IEC 10646-1 Annex G.            */
  /*                                                                          */
  /* This routine only handles UCS codes in the range 0...0xFF since that is  */
  /* all that is required for the demonstration of Baudot support.            */
  /*                                                                          */
  /* input variables:                                                         */
  /* - ucsCode               UCS code index,                                  */
  /*                                                                          */
  /* output variables:                                                        */
  /* - ptr_octet_fifo_state  pointer to the output fifo state buffer for      */
  /*                         the UTF-8 octets.                                */
  /*                                                                          */
  /* Matthias Doerbecker, Ericsson Eurolab Deutschland (EED/N/RV), 2000/06/29 */
  /****************************************************************************/

  void transformUCS2UTF(UWORD16      ucsCode,
                        fifo_state_t*  ptr_octet_fifo_state)
  {
    WORD16  tmpValue;

    if (ucsCode < 0x000000A0)
      {
        /* ucsCodes between 0x00 and 0x9F are coded into one octet */
        tmpValue = (WORD16)ucsCode;
        Shortint_fifo_push(ptr_octet_fifo_state, &tmpValue, 1);
      }
    else if (ucsCode < 0x00000100)
      {
        /* ucsCodes between 0xA0 and 0xFF are coded into two octets */
        tmpValue = 0xA0;
        Shortint_fifo_push(ptr_octet_fifo_state, &tmpValue, 1);
        tmpValue = (WORD16)ucsCode;
        Shortint_fifo_push(ptr_octet_fifo_state, &tmpValue, 1);
      }
    else
      {
  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
        if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_GTT)
        {
          char str[80];
                  
          sprintf(str, 
                  "transformUCS2UTF(): UCS codes > 0xFF are not supported so far!\n\r");
        
          GTT_send_trace_cpy(str);
        }
  #endif    /* End trace  */
        //exit(1);  Should never come here !!

      }
  }

  /****************************************************************************/
  /* transformUTF2UCS()                                                       */
  /* ******************                                                       */
  /* Transformation from UTF-8 into UCS code.                                 */
  /*                                                                          */
  /* This routine only handles UTF-8 sequences consisting of one or two       */
  /* octets (corresponding to UCS codes in the range 0...0xFF) since that is  */
  /* all that is required for the demonstration of Baudot support.            */
  /*                                                                          */
  /* input/output variables:                                                  */
  /* - ptr_octet_fifo_state  pointer to the input fifo state buffer for       */
  /*                         the UTF-8 octets.                                */
  /*                                                                          */
  /* output variables:                                                        */
  /* - *ptr_ucsCode          UCS code index                                   */
  /*                                                                          */
  /* return value:                                                            */
  /* true,  if conversion was successful                                      */
  /* false, if the input fifo buffer didn't contain enough octets for a       */
  /*        conversion into UCS code. The output variable *ptr_ucsCode        */
  /*        doesn't contain a valid value in this case                        */
  /*                                                                          */
  /* Matthias Doerbecker, Ericsson Eurolab Deutschland (EED/N/RV), 2000/06/29 */
  /****************************************************************************/

  BOOL transformUTF2UCS(UWORD16     *ptr_ucsCode,
                        fifo_state_t  *ptr_octet_fifo_state)
  {
    WORD16         numAvailOctets;
    static WORD16  octetBuffer[5];
    
    numAvailOctets = Shortint_fifo_check(ptr_octet_fifo_state);
    if (numAvailOctets>5)
      numAvailOctets=5;
    
    if (numAvailOctets==0)
      return false;
    else 
      {
        Shortint_fifo_peek(ptr_octet_fifo_state, octetBuffer, numAvailOctets);
        if (octetBuffer[0] < 0xA0)
          {
            /* ucsCodes between 0x00 and 0x9F are coded in one octet */
            *ptr_ucsCode = octetBuffer[0];
            Shortint_fifo_pop(ptr_octet_fifo_state, octetBuffer, 1);
            return true;
          }
        else if (octetBuffer[0] == 0xA0)
          {
            /* ucsCodes between 0xA0 and 0xFF are coded in two octets, where  */
            /* the first octet is 0xA0 and the second carries the information */
            if (numAvailOctets>1)
              {
                *ptr_ucsCode = octetBuffer[1];
                Shortint_fifo_pop(ptr_octet_fifo_state, octetBuffer, 2);
                return true;
              }
            else 
              /* the symbol following the 0xA0 is still missing            */
              /* -> we have to wait until the fifo buffer is filled enough */
              return false;
          }
        else
          {
            /* Ignore the actual octet; remove it from the fifo */
            Shortint_fifo_pop(ptr_octet_fifo_state, octetBuffer, 1);

  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
            if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_GTT)
            {
              char str[60];
                  
              sprintf(str, 
                      "transformUTF2UCS(): Octet removed.\n\r");
              
              GTT_send_trace_cpy(str);
            }
  #endif    /* End trace  */
            return false;
          }
      }
  }

#endif // L1_GTT

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_cnvt.c      $|
| $Author:: Es  $ CONDAT GmbH           $Revision:: 1               $|
| CREATED: 03.08.00                     $Modtime:: 03.08.00 18:57   $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_CNVT

   PURPOSE : string convertion

   EXPORT  :

   TO DO   :

   $History:: mfw_cnvt.c                                             $
*/

#define ENTITY_MFW

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#if defined (NEW_FRAME)

#include "typedefs.h"
#include "vsi.h"
#include "pei.h"
#include "custom.h"
#include "gsm.h"

#else

#include "STDDEFS.H"
#include "custom.h"
#include "gsm.h"
#include "vsi.h"

#endif

#include "mfw_mfw.h"
#include "mfw_phb.h"
#include "mfw_phbi.h"
#include "mfw_cm.h"
#include "mfw_cmi.h"
#include "mfw_nm.h"
#include "mfw_sim.h"
#include "mfw_sima.h"
#include "mfw_nmi.h"
#include "mfw_simi.h"
#include "mfw_win.h"

#include "mfw_tim.h"

#include "ksd.h"
#include "psa.h"

#if defined (FAX_AND_DATA)
#include "aci_fd.h"
#endif

#include "cmh.h"
#include "phb.h"
#include "cmh_phb.h"

#include "mfw_ss.h"
#include "mfw_ssi.h"

#define PASSED 0
#define FAILED 1
#define LEN_OFFSET 1

/* Added to remove warning Aug - 11 */
EXTERN char   ATB_char_GSM (char ascii_character);
/* End - remove warning Aug - 11 */ 
/*
+---------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_CNVT        |
| STATE  : code                         ROUTINE: mfw_getStrMode |
+---------------------------------------------------------------+

   PURPOSE : This function is used to request the coding format
             of a given string. The return value is 
             "MFW_DCS_7bits" or " MFW_DCS_UCS2".

*/

T_MFW_DCS mfw_getStrMode( UBYTE *str, // alpha string in MFW
                          UBYTE len)  // length of alpha string
                         
{
  
  if( str[0] == 0x80)
    return MFW_DCS_UCS2;
  else
    return MFW_DCS_7bits;
}

/*
+---------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_CNVT        |
| STATE  : code                         ROUTINE: mfw_SIM2GsmStr |
+---------------------------------------------------------------+

   PURPOSE : This function is used to convert the MFW alpha
             formatted string (consisting of alpha name and
             length of name in bytes) to a GSM default string
             (including zero-termination) and to additionally
             set the @ code to 0x80. 
   
*/

T_MFW mfw_SIM2GsmStr( UBYTE inLen, 
                      UBYTE *inMfwStr, 
                      UBYTE maxOutSize,   /* in byte */
                      UBYTE *outGsmStr )
                     
{
  int i;
  UBYTE len;
  
  if (!inLen OR !maxOutSize)
    return FAILED;        /* string is zero length */
  
  if (*inMfwStr EQ 0x80)  /* input UCS, output GSM default */
  {
    len = inLen / 2;      /* GSM string is half length of unicode string */
    for( i=0; i<MINIMUM(len,maxOutSize-1); i++ )
    {
      if (inMfwStr[(i*2)+2] == 0x00)
        * (outGsmStr + i) = 0x80;
      else
        * (outGsmStr + i) = inMfwStr[(i*2)+2]; /* copy every 2nd byte */
    }
    outGsmStr[i] = '\0';
    return PASSED;
  }
  else                   /* input GSM default, output GSM default */
  {
    for( i=0; i<MINIMUM(inLen,maxOutSize-1); i++ )
    {
      if (inMfwStr[i] EQ 0x00)
        * (outGsmStr + i) = 0x80;
      else
        * (outGsmStr + i) = inMfwStr[i];
    }
    outGsmStr[i] = '\0';
    return PASSED;
  }            

}

/*
+---------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_CNVT        |
| STATE  : code                         ROUTINE: mfw_SIM2UcsStr |
+---------------------------------------------------------------+

   PURPOSE : This function is used to convert the MFW alpha
             formatted string to a unicode string. The first
             two bytes of this unicode string indicates the
             number of characters in the string.
   
*/

T_MFW mfw_SIM2UcsStr( UBYTE   inLen, 
                      UBYTE*  inMfwStr, 
                      UBYTE   maxOutSize,    /* in short */
                      U16*    outUcsStr)
                     
{
  int i;
  short cnt;
  
  if (!inLen OR !maxOutSize)
    return FAILED;
  
  if (inMfwStr[0] EQ 0x80)  /* input UCS, output UCS */
  {
  TRACE_FUNCTION("First byte is indeed 0x80");
    //inMfwStr++;
    inLen--;
    
    // NOTE: do not add '\0' termination!
    // len should not include termination
    cnt = 1;
    for( i=0; i<MINIMUM(inLen/2,maxOutSize-1); i++ )
    {
      outUcsStr[ i + LEN_OFFSET] = ((U16)inMfwStr[cnt] << 8)&0xFF00 + ((U16)inMfwStr[cnt + 1])&0xFF;
      cnt += 2;
    } 
    outUcsStr[0] = i;
    return PASSED;
  }
  else                    /* input GSM default, output UCS */
  {
    for( i=0; i<MINIMUM(inLen,maxOutSize-1); i++ )
      * (outUcsStr + i + LEN_OFFSET) = (U16) inMfwStr[i];
    
    outUcsStr[0] = i;     /* length of unicode string */
    return PASSED;
  }
}

/*
+---------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_CNVT        |
| STATE  : code                         ROUTINE: mfw_Gsm2SIMStr |
+---------------------------------------------------------------+

   PURPOSE : This function is used to convert a zero-terminated
             GSM default string ( @ code is 0x80) to a MFW 
             alpha formatted string (consisting of alpha name
             and length of name in bytes and @ code is 0x00). 
             The MFW alpha format is dependent on the parameter "outMode" 
             "outMode" is set to either " MFW_DCS_7bits" or " MFW_DCS_UCS2" 

GW-SPR#762- Increased array size - required for larger phonebook entries (>40 chars). 
*/

#define MAX_CONVERT_LEN 255
T_MFW mfw_Gsm2SIMStr( T_MFW_DCS outMode, 
                      UBYTE *inGsmStr, 
                      UBYTE maxOutSize,
                      UBYTE *outMfwStr,
                      UBYTE *outLen )
{
  
  UBYTE temp_str[MAX_CONVERT_LEN];
  int i;
  UBYTE len = (UBYTE) strlen( (char *)inGsmStr );
  char GSM_char;/*SPR2175*/
  
  len=MINIMUM(len,sizeof(temp_str));
  memcpy( temp_str,inGsmStr,len );
  
  if( outMode EQ MFW_DCS_7bits )
  {
    /* write to outMfwStr */
    for( i=0; i<MINIMUM(len,maxOutSize); i++ )
    {	/*SPR2175, convert to GSM encoding*/
       GSM_char= ATB_char_GSM( temp_str[i] );
       temp_str[i]= GSM_char;
      * (outMfwStr + i) = temp_str[i];
    }
    /* write length of name to outLen */
    *outLen = len;
    return PASSED;
  }
  else if( outMode EQ MFW_DCS_UCS2 )
  {
    /* write to outMfwStr */
    outMfwStr[0] = 0x80;
    for( i=0; i<MINIMUM(len,maxOutSize-1); i++ )
    {
      if( temp_str[i] EQ 0x80 ) /* check for occurance of @ */
        temp_str[i] = 0x00;
      * (outMfwStr + (i*2) + 1) = 0x00;
      * (outMfwStr + (i*2) + 2) = temp_str[i];
    }
    /* write length of name to outLen */
    *outLen = len*2 + 1;
    return PASSED;
  }
  else
  {
    TRACE_EVENT ("mfw_Gsm2SIMStr() - outMode is not valid");
    return FAILED;
  }
  
//  return PASSED;  // RAVI
}

/*
+---------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_CNVT        |
| STATE  : code                         ROUTINE: mfw_Ucs2SIMStr |
+---------------------------------------------------------------+

   PURPOSE : This function is used to convert a unicode string
             to MFW alpha format. The MFW alpha format is 
             dependent on the parameter "outMode". "outMode" 
             is " MFW_DCS_7bits" or " MFW_DCS_UCS2".
   
*/

T_MFW mfw_Ucs2SIMStr( T_MFW_DCS outMode, 
                      U16 *inDcsStr, 
                      UBYTE maxOutSize,
                      UBYTE *outMfwStr,
                      UBYTE *outLen )
{
  int i;
  U16 len;
  
  len = inDcsStr[0];
  
  if( outMode EQ MFW_DCS_7bits )
  {
    /* write to outMfwStr */
    for( i=0; i<MINIMUM(len,maxOutSize); i++ )
    {
      * (outMfwStr + (i*2) + 1) = (UBYTE) (inDcsStr[i+1] & 0xff);
      * (outMfwStr + (i*2)) = (UBYTE) ((inDcsStr[i+1] & 0xff00) >> 8);
    }
    /* write length of name to outLen */
    *outLen = len * 2;
    
    return PASSED;
  }
  else if( outMode EQ MFW_DCS_UCS2 )
  {
    /* write to outMfwStr */
    outMfwStr[0] = 0x80;
    for( i=0; i<len; i++)
    {/*MC, SPR 1752, bytes are no longer swapped*/
      * (outMfwStr + (i*2) + 1) = (UBYTE) (inDcsStr[i+1] & 0xff);
      * (outMfwStr + (i*2) + 2) = (UBYTE) ((inDcsStr[i+1] & 0xff00) >> 8);
    }
    /* write length to outLen */
    *outLen = (len*2) + 1;
    
    return PASSED;
    
  }
  else	/* string does not require conversion to unicode */
  {
    TRACE_EVENT ("mfw_Ucs2SIMStr() - outMode is not valid");
    return FAILED;
  }
  
//  return PASSED;  // RAVI
}

/*CONQUEST 5992(related) MC added function to convert GSM string to ASCII*/
static const unsigned char gsm_2_ascii_table[128] = { 64, 156, 36, 157, 138, 130, 151, 141, 149, 128,
													 10,   2 ,  7,  32, 143, 134,   4,  95, 232, 226, 239, 234, 227,   5, 228,
											 		233, 240,  32, 146, 145, 225, 144,  32,  33,  34,  35,   1,  37,  38,  39,
											 		 40,  41,  42,  43,  44, 45,  46,  47,  48,  49,  50,  51,  52,  53,  54,
											 		 55,  56,  57,  58,  59,  60,  61,  62,  63, 173,  65,  66,  67,  68,  69,
											 		 70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,
											 		 85,  86,  87,  88,  89,  90, 142, 153, 165, 154,  06, 168,  97,  98,  99,
											 		100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114,
											 		115, 116, 117, 118, 119, 120, 121, 122, 132, 148, 164, 129, 133};
/* PATCH JPS 29.09 END */

/*
+---------------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS                  |
| STATE   : code                         ROUTINE :   mfw_GSM_2_ASCII      |
+---------------------------------------------------------------------------+

  PURPOSE : copy the index and status of a short message

*/
void mfw_GSM_2_ASCII(UBYTE* gsm_string, UBYTE* ascii_string, SHORT length)
{	int i;

	for(i=0;i<length;i++)
	{
		ascii_string[i] = gsm_2_ascii_table[gsm_string[i]];
	}
}


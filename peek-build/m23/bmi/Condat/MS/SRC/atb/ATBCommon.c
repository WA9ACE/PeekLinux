/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	                                                      
 $Project code:	                                                           
 $Module:		
 $File:		    ATBCommon.c
 $Revision:		                                                      
                                                                              
 $Author:		SH - Condat(UK)                                                         
 $Date:		                                                          
                                                                               
********************************************************************************
                                                                              
 Description: ATB Common functions and definitions.
    
                        
********************************************************************************

 $History: ATBCommon.c
      May 7, 2007 OMAPS00130278 a0393213(R.Prabakar)
      Description : CT-GCF[34.2.5.1] - MS fails to display class 0 MT SMS when it contains alphanumeric and special characters
      
	Nov 24, 2006 DR: OMAPS00092906 - x0039928
    	Description: CT-GCF[34.3]-Some broadcasted charcters not dispayed correctly in MMI
    	Solution: g_translation_8bit_table[] array is modified. Changed code 0xFF to 0x1B.
    
    Jun 09, 2006 DR: OMAPS00080543 - xreddymn
    ATB_convert_String does not work correctly for 7-bit GSM to 8-bit
    conversion. This is due to a dependency with ACI function. This has been
    replaced now with an independent conversion function, which 
    ATB_convert_String can call.

	Mar 30, 2005	REF: CRR 29986	xpradipg
	Description: Optimisation 1: Removal of unused variables and dynamically
	allocate/ deallocate mbndata
	Solution: removed the definition of translation_ascii_2_unicode and added 
	the extern declaration

       March 24, 2005 REF: CRR MMI-FIX-9828 x0012852
	Description: BMI having problems with Umlauts ,especially "?
	Solution: g_translation_8bit_table[] array is modified . Changed codes 0x80 , 0x81,      
                     0x82 to ox81, 0x82 ,0x83 respectively.

	Jan 05, 2005 REF: CRR MMI-FIX-24757 xnkulkar
	Description: the display doesn´t show special chars correctly; if there is an "@" in the text, the "@" and 
			    text after is not visible
	Solution: Fix for MMI-SPR-25978 has solved the problem of data loss after '@' symbol. However, some other 
	              symbols (like ?etc.) were not displayed properly. Modified the g_translation_8bit_table[] array 
			to take care of all the special GSM character set .
 
	Nov 30, 2004 REF: CRR MMI-SPR-25978 xnkulkar
	Description: Messages:Not able to receive message after @ in the receiver phone (Bug raised in 3.x)
	Solution: in g_translation_8bit_table, the value for '@' symbol was used as 0x00. It has been
	              changed to 0x40 now.
 	
 	26-05-04      CRR 19656    Sandip and Deepa M.D
      	Fix for The Dsample doesn't display lines ( <CR> or <LF> ) in the SETUP IDLE 
	TEXT command.
	Fix for The idle screen is empty when a SETUP IDLE TEXT command is sent with 
	a long text ( 239 characters ).
	
	   
 $End

*******************************************************************************/

/* includes */
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
#include "mfw_win.h"
#include "mfw_kbd.h"
#include "mfw_tim.h"
#include "mfw_phb.h"
#include "mfw_sms.h"
#include "mfw_ss.h"
#include "mfw_icn.h"
#include "mfw_mnu.h"
#include "mfw_lng.h"
#include "mfw_sat.h"
#include "mfw_kbd.h"
#include "mfw_nm.h"
#include "mfw_cm.h"
#include "ATBCommon.h"

/*SPR2175, Added macro and tables for string conversion routines*/
#define MAX_STRING_LEN (5+MFW_NUM_LEN+MAX_MSG_LEN+4)

/*SPR 1508, changed codes 128, 129 and 130 to 129, 130, and 131 respectively.*/
static const unsigned char g_translation_ascii_table[128] = { 64, 156, 36, 157, 138, 131, 151, 141, 149, 129,
													 10,   2 ,  7,  32, 143, 134,   4,  95, 232, 226, 239, 234, 227,   5, 228,
											 		233, 240,  27, 146, 145, 225, 144,  32,  33,  34,  35,   1,  37,  38,  39,
											 		 40,  41,  42,  43,  44, 45,  46,  47,  48,  49,  50,  51,  52,  53,  54,
											 		 55,  56,  57,  58,  59,  60,  61,  62,  63, 173,  65,  66,  67,  68,  69,
											 		 70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,
											 		 85,  86,  87,  88,  89,  90, 142, 153, 165, 154,  168, 06,  97,  98,  99,
											 		100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114,
											 		115, 116, 117, 118, 119, 120, 121, 122, 132, 148, 164, 130, 133};



static const int g_translation_unicode[128] = { 0x0040, 0x00A3, 0x0024, 0x00A5, 0x00E8, 0x00E9, 0x00F9, 0x00EC, 0x00F2, 0x00C7, 0x000A, 0x00D8, 0x00F8,
												0x000D, 0x00C5, 0x00E5, 0x0394, 0x005F, 0x03A6, 0x0393, 0x039B, 0x03A9, 0x03A0, 0x03A8, 0x03A3, 0x0398,
												0x039E, 0x03A2, 0x00C6, 0x00E6, 0x00DF, 0x00C9, 0x0020, 0x0021, 0x0022, 0x0023, 0x00A4, 0x0025, 0x0026,
												0x0027, 0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F, 0x0030, 0x0031, 0x0032, 0x0033,
												0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F, 0x00A1,
												0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D,
												0x004E, 0x004F, 0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005A,
												0x00C4, 0x00D6, 0x00D1, 0x00DC, 0x00A7, 0x00BF, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
												0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F, 0x0070, 0x0071, 0x0072, 0x0073, 0x0074,
												0x0075,	0x0076, 0x0077, 0x0078, 0x0079, 0x007A, 0x00E4, 0x00F6, 0x00F1, 0x00FC,	0x00E0};

/*SPR 1508, added conversion table*/												
//	Mar 30, 2005	REF: CRR 29986	xpradipg
//	remove the definition of the table
#ifdef FF_MMI_OPTIM
extern const int translation_ascii_2_unicode[];
#else
static const int translation_ascii_2_unicode[256] = { 
	0x0000, 0x00A4, 0x00d8, 0xFFFF, 0x0394, 0x03A8, 0x00BF, 0x00F8, 0xFFFF, 0xFFFF, 0x000A, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
	0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
	0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026,0x0027, 0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F,
	0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F,
	0x0040,0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,
	0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005A, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x005F, 
	0xFFFF, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,
	0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007A, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,

	0xFFFF, 0x00C7, 0x00FC, 0x00E9, 0x00E4, 0x00E0, 0x00E5, 0xFFFF, 0xFFFF, 0xFFFF, 0x00E8, 0xFFFF, 0xFFFF, 0x00EC, 0x00C4, 0x00C5, 
	0x00C9, 0x00E6, 0x00C6, 0xFFFF, 0x00F6, 0x00F2, 0xFFFF, 0x00F9, 0xFFFF, 0x00D6, 0x00DC, 0xFFFF, 0x00A3, 0x00A5, 0xFFFF, 0xFFFF,
	0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x00F1, 0x00D1, 0xFFFF, 0xFFFF, 0x00A7, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x00A1, 0xFFFF, 0xFFFF,
	0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
	0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
	0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
	0xFFFF, 0x00DF, 0x0393, 0x03A0, 0x03A3, 0xFFFF, 0xFFFF, 0xFFFF, 0x03A6, 0x0398, 0x03A9, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x039B, 
	0x039E, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF
	};
#endif	
//	MMI_FIX-19656   Using 8 bit table for dispay start
//	Nov 30, 2004 REF: CRR MMI-SPR-25978 xnkulkar
//	Description: Messages:Not able to receive message after @ in the receiver phone (Bug raised in 3.x)
//	Solution: in g_translation_8bit_table, the value for '@' symbol was used as 0x00. It has been
//	              changed to 0x40 now.

//	Jan 05, 2005 REF: CRR MMI-FIX-24757 xnkulkar
//	Description: the display doesn´t show special chars correctly; if there is an "@" in the text, the "@" and 
//			    text after is not visible
//	Solution: Fix for MMI-SPR-25978 has solved the problem of data loss after '@' symbol. However, some other 
//                 symbols (like ?etc.) were not displayed properly. Modified the g_translation_8bit_table[] array 
//		       to take care of all the special GSM character set .

//   March 24, 2005 REF: CRR MMI-FIX-9828 x0012852
//   Description: BMI having problems with Umlauts ,especially "?
//   Solution: g_translation_8bit_table[] array is modified . Changed codes 0x80 , 0x81,      
//                 0x82 to ox81, 0x82 ,0x83 respectively.


static const unsigned char g_translation_8bit_table[256] ={ 
	0x40, 0x9C, 0x24, 0x9D, 0x8A, 0x83, 0x97, 0x8D, 0x95, 0x81, 0x0A, 0x02, 0x07, 0x20, 0x8F, 0x86,   
	0x04, 0x5F, 0xE8, 0xE2, 0xEF, 0xEA, 0xE3, 0x05, 0xE4, 0xE9,0xF0, 0x1B, 0x92, 0x91, 0xE1, 0x90,   
	0x20, 0x21, 0x22, 0x23, 0x01, 0x25, 0x26,0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,  
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0xAD,0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x8E, 0x99, 0xA5, 0x9A, 0xA8,    
	0x06, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,  
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x84, 0x94, 0xA4, 0x82, 0x85,  

	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
//MMI_FIX-19656   Using 8 bit table for dispay start

static char convertUCS2_2_ascii_char(char* ipString);
static char convertUCS2_2_gsm_char(char* ipString);
static USHORT ATB_swap_bytes(USHORT character);

/*******************************************************************************

 $Function:    	convertUCS2_2_ascii_char

 $Description:	convert unicode char to ASCII char. Added for SPR 1508. 
 				moved here for SPR 2175

 $Returns:		ASCII char
 $Arguments:	pointer to first byte of unicode char
*******************************************************************************/
static char convertUCS2_2_ascii_char(char* ipString) 
{
	U16 word_code;
	BOOL match_found= FALSE;
	int k;
	char debug[60];
	
	word_code = ipString[0]*0x100 + ipString[1];
	sprintf(debug, "Word code: %d", word_code);
	TRACE_EVENT(debug);
	if (word_code <= 0xFF)
		if (translation_ascii_2_unicode[word_code] == word_code)
			return word_code;
		

		for (k=0; k < 256; k++)
		{	if(translation_ascii_2_unicode[k] == word_code)
			{	sprintf(debug, "Match found: %d, %d", word_code, k);
				TRACE_EVENT(debug);
				match_found = TRUE; /*a0393213 warnings removal-missing return statement-modified the flow*/
				break;		
			}
							
		}

	if (!match_found)
	{	TRACE_EVENT("No match");

		return 0x3F;/*question mark*/
	}
	else
	{
		return k;
	}
}
/*******************************************************************************
 $Function:    	convertUCS2_2_ascii_char
 $Description:	convert unicode char to GSM char.  Added for SPR 1508.
 				moved here for SPR 2175
 $Returns:		GSM char
 $Arguments:	pointer to first byte of unicode char
*******************************************************************************/
static char convertUCS2_2_gsm_char(char* ipString) 
{
	U16 word_code;
	BOOL match_found= FALSE;
	int k;
	word_code = ipString[0]*0x100 + ipString[1];
								
	if ( word_code > 0x7F || g_translation_unicode[word_code] != word_code)
	{	

		for (k=0; k < 128; k++) /*a0393213 lint warning:out-of-bounds soln:256 changed to 128*/
		{	if(g_translation_unicode[k] == word_code)
			{
				match_found = TRUE;
				break;
			}
							
		}
	}
	else
		return word_code;
	if (!match_found)
	{	return '?';
	}
	else
	{
		return k;
	}
}
/*a0393213(R.Prabakar) OMAPS00130278 - MS fails to display class 0 MT SMS when it contains alphanumeric and special characters.*/
/*********************************************************
 $Function   : convertUnpacked7To8
 $Description: This function converts the unpacked 7 bit encoded string to NULL terminated ASCII string. 
 $Arguments  : ipString - input string
               ipLength - length of input string
               opString - output string
               opLength - length of output string
**********************************************************/
void convertUnpacked7To8 (char * ipString, int ipLength, 
						  char * opString, int opLength)
{
	int index;

	for (index = 0; index < ipLength && index < (opLength-1); index++)
	{
	opString[index]= (char)g_translation_ascii_table[ipString[index]];
	}
	/*null terminate the string*/
	opString[index]='\0';	
}


/*******************************************************************************
 $Function:     ATB_cvt7To8
 $Description:  This is a replacement for the ACI function utl_cvt7To8.
                utl_cvt7To8 cannot be used for output strings larger than
                255 bytes, because it uses UBYTE values. ATB_cvt7To8 uses
                U16 values and overcomes this problem.
 $Arguments:    Arguments are similar to utl_cvt7To8.
                source - Source string which is 7-bit GSM encoded.
                source_len - Length of source string in bytes.
                dest - Destination string, which will be 8-bit encoded.
                bit_offset - indicates which bit of the first byte is the 
                boundary of the 7bits source.
                ex: for bit_offset = 3, bits 0-2 (low weight bits)
                    will not be decoded
*******************************************************************************/
/* xreddymn OMAPS00080543 Jun-06-2006 */

U16 ATB_cvt7To8 (const UBYTE*   source,
                       U16      source_len,
                       UBYTE*   dest,
                       UBYTE    bit_offset)
{
  UBYTE d_mask = 0x01;
  UBYTE s_mask;
  U16 icnt;
  U16 ocnt;

  *dest = 0x00;
  icnt  = source_len;   

  if( !icnt ) return( 0 );
  ocnt  = 0;

  while (icnt)               
  {
    s_mask = 0x01;      

    if (icnt == source_len)
      s_mask <<= bit_offset;

    while (s_mask > 0x00)   
    {
      if (s_mask & *source)  
        *dest |= d_mask;  

      s_mask <<= 1;     
      d_mask <<= 1;

      if (d_mask > 0x40)     
      {
        dest++;              
        ocnt++;
        d_mask = 0x01;
        *dest  = 0x00;
      }
    }

    source++;                
    icnt--;
  }
  return( ocnt );
}

/*******************************************************************************

 $Function:    	ATB_convert_String

 $Description:	Convert a string into an ascii string with 0x80 as first word if data is unicode
 				moved here and renamed for SPR 2175
 $Returns:		execution status

 $Arguments:	ipString - input byte array
 				ipDataType - type of input data
 				ipLength - length of input string (in bytes) 
				opString - output byte array
 				opDataType - type of output data
 				opLength - length of output string (in bytes) 
*******************************************************************************/
/* MC, SPR 1257, merged in b-sample version of this function*/

int ATB_convert_String(			char * ipString,	UBYTE ipDataType,	int ipLength,
										char * opString, 	UBYTE opDataType,	int opLength, UBYTE addNull)
{
	int i;/*SPR 1946, changed from byte to int*/
	int j;/*SPR 1946, changed from byte to int*/
    /*JVJ SPR1298 Converted to dynamic variable */
	UBYTE* text_8bit = NULL;/*SPR1991, don't alloacte this till it's needed*/ 
	int  len=0;/*SPR 1946, changed from byte to int*/

#ifdef TRACE_SMSREAD

  char buf[150];
  sprintf(buf, "opString%x ipString%x ipLength%d opLength%d",opString, ipString, ipLength, opLength);
  TRACE_EVENT(buf);
#endif
  
	if ((opString == NULL) || (ipString==NULL) || (opLength ==0))
		return (FALSE);
	if (ipLength>MAX_STRING_LEN)
		{
		TRACE_EVENT("Too long, returning.");
		return FALSE;
		}
	opString[0] = 'X';
	opString[1] = '?';
	opString[2] = 0x00;
	
#ifdef TRACE_SMSREAD
  sprintf(buf,"iDT: %d,iL:%d, oDT:%d, oL:%d, A0:%d",ipDataType,ipLength,opDataType,opLength,addNull);
  TRACE_EVENT(buf);
#endif
  
		
	switch (ipDataType)
	{
	case MFW_DCS_UCS2:
			if (opDataType == MFW_DCS_UCS2)
			{	//Copy array and add 0x80, 0x7F as first word
				if (ipLength < opLength)
					len = ipLength;
				else
					len = opLength;
		/*SPR 1507, copy the bytes in pairs*/
				for (i=0;i<len;i+=2)
				{ opString[i] = ipString[i];
					opString[i+1] = ipString[i+1];
				}
				if (i+1<opLength)
				{
					opString[i  ] = 0x00;
					opString[i+1] = 0x00;
				}
			}
			else if ((opDataType == MFW_DCS_8bits) ||
					(opDataType == MFW_ASCII))
			{			
				if ((ipLength/2 ) < opLength)
					len = ipLength/2;
				else
					len = opLength;

				for (i=0;i<len;i++)
				{	/*SPR 1508, use new functions to convert to ASCII/GSM*/
					if (opDataType == MFW_ASCII)
						opString[i] = convertUCS2_2_ascii_char(&ipString[i*2]);
					else
						opString[i] = convertUCS2_2_gsm_char(&ipString[i*2]);
				}
				if (len<opLength)
				{
					opString[len  ] = 0x00;
				}
	
			}
			else	//Output data type is 7 bits - not implemented
			{
			}
			break;
	case MFW_DCS_8bits:
		
			if (opDataType == MFW_DCS_UCS2)
			{	//Convert data to unicode
				if (ipLength*2 < opLength)
					len = ipLength;
				else
					len = opLength/2;
#ifdef TRACE_SMSREAD
				sprintf(buf, "8->UCS2, len:%d", len);
				TRACE_EVENT(buf);
#endif				
				for (i = 0; i < len; i++)
				{	/*SPR 1508 use table to convert to Unicode*/
					opString[i*2]   = (g_translation_unicode[ipString[i]]>>8)&0x00FF;
					opString[i*2+1] = (g_translation_unicode[ipString[i]])&0x00FF;
				}
				if (len*2+1 < opLength)
				{
					opString[len*2]   = 0x00;
					opString[len*2+1] = 0x00;
				}
			}
			else if (opDataType == MFW_DCS_8bits)
			{	//Data already in correct format - copy
				if (ipLength < opLength)
					len = ipLength;
				else
					len = opLength;
				for (i=0;i<len;i++)
					{
					opString[i] = (char)g_translation_8bit_table[ipString[i]];
					}
				if (len<opLength)
					opString[len] = 0x00;
			}
			else if (opDataType == MFW_ASCII)	// SH - translate characters, don't change bit width
			{
				len = ipLength;
				if (len > opLength)
					len = opLength;
				for (i=0;i<len;i++)
					{
					opString[i] = (char)g_translation_8bit_table[ipString[i]];
					}
				if (len<opLength)
					opString[len] = 0x00;

				//MMI_FIX-19656   Using 8 bit table for mapping end
			}
			else // convert data to 7bit
			{
				len = utl_cvt8To7 ((UBYTE *)ipString , ipLength, (UBYTE*)opString, 0);
			}
		break;
	case MFW_DCS_7bits:
	default:
		/*SPR 1991, allocate memory to conversion buffer*/
		text_8bit = (UBYTE*)mfwAlloc(MAX_STRING_LEN+1);  
		
			if (opDataType == MFW_DCS_7bits) //text string is already in this format
			{
				//Data already in correct format - copy
				if (ipLength < opLength)
					len = ipLength;
				else
					len = opLength;
				for (i=0;i<len;i++)
					opString[i] = ipString[i];
				if (len<opLength)
					opString[len] = 0x00;
			}
			else if (opDataType == MFW_DCS_8bits)
			{
			    /* xreddymn OMAPS00080543 Jun-06-2006 */
				len = ATB_cvt7To8 ((UBYTE *)ipString , ipLength, (UBYTE *)opString, 0 );
				if (len <opLength)
					opString[len] = 0x00;
        else
          opString[opLength]=0x00;
			}
			else if (opDataType == MFW_ASCII)	// SH - expand to 8 bit and convert chars
			{
				/* xreddymn OMAPS00080543 Jun-06-2006 */
				len = ATB_cvt7To8 ((UBYTE *)ipString , ipLength, text_8bit, 0 );
				if (len > opLength)
					len = opLength;
  				for (i = 0; i < len; i++)
					{
					opString[i] = (char)g_translation_ascii_table[UBYTEToChar(text_8bit[i])];
					}
				if (len <opLength)
					opString[len] = 0x00;
			}
			else
			{	//Convert to unicode
			    /* xreddymn OMAPS00080543 Jun-06-2006 */
				len = ATB_cvt7To8 ((UBYTE *)ipString , ipLength, text_8bit, 0);
				if (len*2 > opLength)
					len = opLength/2;
				for (i = 0; i < len; i++)
				{/*SPR 1508, use new table to convert to Unicode*/
					opString[i*2]   = (g_translation_unicode[ipString[i]]>>8)&0x00FF;
					opString[i*2+1] = g_translation_unicode[UBYTEToChar(text_8bit[i])];
				}
				if (len*2+1 < opLength)
				{
					opString[len*2]   = 0x00;
					opString[len*2+1] = 0x00;
				}
			}
		break;
		
	case MFW_ASCII:
			if (ipLength <=0)
				ipLength = strlen(ipString); //calculate length if not supplied
			if (opDataType == MFW_DCS_UCS2)
			{	//Convert data to unicode
				if (ipLength*2 < opLength)
					len = ipLength;
				else
					len = opLength/2;
				for (i = 0; i < len; i++)
				{	/*SPR 1508, convert to Unicode using new table*/
					opString[i*2]   = (translation_ascii_2_unicode[ ipString[i]]>>8)&0x00FF;
					opString[i*2+1] = (translation_ascii_2_unicode[ ipString[i]])&0x00FF;
				}
				if (len*2+1 < opLength)
				{
					opString[len*2]   = 0x00;
					opString[len*2+1] = 0x00;
				}
			}
			else if (opDataType == MFW_ASCII)
			{	//Data already in correct format - copy
				if (ipLength < opLength)
					len = ipLength;
				else
					len = opLength;
				for (i=0;i<len;i++)
					opString[i] = ipString[i];
				if (len<opLength)
					opString[len] = 0x00;
			}
			else if (opDataType == MFW_DCS_8bits || opDataType == MFW_DCS_7bits)	// SH - translate characters, don't change bit width
			{

				if (ipLength < opLength)
					len = ipLength;
				else
					len = opLength;
				for (i=0;i<len;i++)
					{
					opString[i] = ipString[i];
					if ((unsigned char)(ipString[i])>=128 || g_translation_ascii_table[ipString[i]]!=(unsigned char)ipString[i])
						{
						for (j=0; j<128; j++)			// Reverse translation
							{
							if (ipString[i] == (char)g_translation_ascii_table[j])
								opString[i] = j;
							}
						#ifdef TRACE_SMSREAD
						sprintf(buf, "Converted %x to %x", ipString[i], opString[i]);
						TRACE_EVENT(buf);
						#endif
						}
					}
				if (len<opLength)
					opString[len] = 0x00;
			}
			if (opDataType == MFW_DCS_7bits) // convert data to 7bit
			{
				len = utl_cvt8To7 ((UBYTE *)ipString , ipLength, (UBYTE*)opString, 0);
			}
		break;
	}

	if (addNull)
	{	//Add 0x00 (or 0x00,0x00) to end of array

	/*SPR925 - SH - changed so NULL is in right place for unicode*/
		if (opDataType == MFW_DCS_UCS2)
		{
			if (len*2+1>opLength)
				len = opLength/2;

			opString[len*2] = 0x00;
			opString[len*2+1] = 0x00;
		}
		else
		{
			if (len>opLength)
				len = opLength;

			opString[len] = 0x00;
		}
	/* end SPR925 */
	}
	/*SPR 1991, free memory only if it has been allocated*/
	if (text_8bit!= NULL)
        mfwFree(text_8bit,MAX_STRING_LEN+1);
	return TRUE;
}



/*******************************************************************************

 $Function:    	ATB_mem_Alloc

 $Description:	ATB memory allocation function.
 
 $Returns:		Address if successful, null if not.

 $Arguments:	Size of memory to be allocated.
 
*******************************************************************************/

UBYTE* ATB_mem_Alloc(USHORT size)
{
	return (UBYTE *)mfwAlloc(size);
}


/*******************************************************************************

 $Function:    	ATB_mem_Free

 $Description:	Frees memory allocated by the ATB
 
 $Returns:		None.

 $Arguments:	address		- Pointer to memory to free
 				size		- Size of the allocated block
 
*******************************************************************************/

void ATB_mem_Free(UBYTE* address, USHORT size)
{
	mfwFree(address,size);
	return;
}
/*******************************************************************************

 $Function:		ATB_swap_bytes

 $Description:	Returns a unicode character with the bytes swapped. Added for SPR2175

 $Returns:		USHORT

 $Arguments:	USHORT

*******************************************************************************/
static USHORT ATB_swap_bytes(USHORT character)
{
	return ((UNICODE_NONASCII&character) << 8) + ((UNICODE_ASCII&character) >>8);

}

/*******************************************************************************

 $Function:		ATB_char_GSM

 $Description:	Returns the GSM of the provided ascii character.Added for SPR 2175.

 $Returns:		A GSM character

 $Arguments:	character - The ascii character

*******************************************************************************/

char ATB_char_GSM(char ascii_character)
{	int i;
	/*check if GSM and ASCII encoding are the same value*/
	if (g_translation_ascii_table[ascii_character] == (unsigned char)ascii_character)
		return ascii_character;
	
	/*otherwise we have to search for the correct entry in the GSM to ASCII table*/
	for (i=0; i<128; i++)
	{
		if (g_translation_ascii_table[i] == (unsigned char)ascii_character)
			return i;
	}
	return 0x3F;/*question mark*//*a0393213 warnings removal*/
}
/*******************************************************************************

 $Function:		ATB_char_Ascii

 $Description:	Returns the ascii of the provided unicode character

 $Returns:		An ascii character

 $Arguments:	character - The unicode character

*******************************************************************************/

char ATB_char_Ascii(USHORT character)
{	int i;

	/*SPR2175, rewrote function*/
	/*if the first byte of the character is the same as the ASCII code, return the first byte*/
	if (translation_ascii_2_unicode[((character & UNICODE_ASCII)>>8)] == ((character & UNICODE_ASCII)>>8))
		return (char)((character & UNICODE_ASCII)>>8);
	
	/*otherwise we have to search for the correct entry in the ASCII to unicode table*/
	for (i=0; i<256; i++)
	{
		if (translation_ascii_2_unicode[i] == ATB_swap_bytes(character))
			return i;
	}
	return 0x3F;/*question mark*//*a0393213 warnings removal*/
}


/*******************************************************************************

 $Function:		ATB_char_Unicode

 $Description:	Returns the unicode of the provided ascii character

 $Returns:		A unicode character

 $Arguments:	character - The ascii character

*******************************************************************************/

USHORT ATB_char_Unicode(char character)
{	/*SPR 2175, re-wrote function*/
	USHORT table_value;
	USHORT return_value = NULL;
	/*look up corresponding Unicode char in table*/
	table_value= translation_ascii_2_unicode[character];

	/*swap bytes as little-endian*/
	return_value = ATB_swap_bytes(table_value);

	return return_value;
}


/*******************************************************************************

 $Function:		ATB_char_IsAscii

 $Description:	Returns TRUE if the unicode character provided is ascii

 $Returns:		TRUE or FALSE

 $Arguments:	character - The unicode character

*******************************************************************************/

BOOL ATB_char_IsAscii(USHORT character)
{
	BOOL	result = FALSE;

	if (character & UNICODE_NONASCII)					// Make sure character is ASCII
	{
		result = FALSE;
	}
	else
	{
		result = TRUE;
	}

	return result;
}


/*******************************************************************************

 $Function:		ATB_char_IsAlphabetic

 $Description:	Return TRUE if unicode character is part of latin alphabet

 $Returns:		TRUE or FALSE

 $Arguments:	character - The unicode character

*******************************************************************************/

BOOL ATB_char_IsAlphabetic(USHORT character)
{
	BOOL	result = FALSE;
	UBYTE	asciiChar; 

	if (ATB_char_IsAscii(character))					// Make sure character is ASCII
	{
		asciiChar = ATB_char_Ascii(character);
		if ( (asciiChar >='A') && (asciiChar <='Z') )
			result = TRUE;
		else if ( (asciiChar >='a') && (asciiChar <='z') )
			result = TRUE;
	}

	return result;
}


/*******************************************************************************

 $Function:		ATB_char_IsNumierc

 $Description:	Return TRUE if unicode character is a digit, 0-9

 $Returns:		TRUE or FALSE

 $Arguments:	character - The unicode character

*******************************************************************************/

BOOL ATB_char_IsNumeric(USHORT character)
{
	BOOL	result = FALSE;
	UBYTE	asciiChar;
 
	if (ATB_char_IsAscii(character))				// Make sure character is ASCII
	{
		asciiChar = ATB_char_Ascii(character);
		if ( (asciiChar >='0') && (asciiChar <='9') )
			result = TRUE;
	}

	return result;
}

/*******************************************************************************

 $Function:		ATB_char_IsVisible

 $Description:	Return TRUE if unicode character is visible, i.e. not ascii <32

 $Returns:		TRUE or FALSE

 $Arguments:	character - The unicode character

*******************************************************************************/

BOOL ATB_char_IsVisible(USHORT character)
{
	BOOL	result = TRUE;
	UBYTE	asciiChar; 

	if (ATB_char_IsAscii(character))					// Make sure character is ASCII
	{
		asciiChar = ATB_char_Ascii(character);
		/*SPR 2175, if ASCII value has no unicode equivalent assume not visible*/
		if (translation_ascii_2_unicode[asciiChar] == 0xFFFF)
			result = FALSE;
	}

	return result;
}


/*******************************************************************************

 $Function:		ATB_string_Size

 $Description:	Return the size in bytes of a character in the string

 $Returns:		No. of bytes per character

 $Arguments:	text	- The text string

*******************************************************************************/
  
UBYTE ATB_string_Size(T_ATB_TEXT *text)
{
	UBYTE size;
	
	switch(text->dcs)
	{
		case ATB_DCS_UNICODE:
			size = 2;
			break;

		default:
			size = 1;
			break;
	}
	
	return size;
}


/*******************************************************************************

 $Function:		ATB_string_SetChar

 $Description:	Set a character in the string to a value.  Use this function rather than
 				accessing text data directly; it allows transparent ASCII and Unicode
 				handling.
 
 $Returns:		None

 $Arguments:	text		- The text string
 				textIndex	- The position of the character to set
 				character	- The unicode character to set it to

*******************************************************************************/
  
void ATB_string_SetChar(T_ATB_TEXT *text, USHORT textIndex, USHORT character)
{
	USHORT *unicode;
	
//TISH modified for MSIM
//Begin
	if (text->dcs==ATB_DCS_UNICODE)
	{
		unicode = (USHORT *)text->data;
		#ifdef WIN32
		if(unicode[textIndex] != character)
		#endif
			unicode[textIndex] = character;
	}
	else
	{
		#ifdef WIN32
		if(text->data[textIndex] != ATB_char_Ascii(character))
		#endif
			text->data[textIndex] = ATB_char_Ascii(character);
	}
//End
	return;
}


/*******************************************************************************

 $Function:		ATB_string_GetChar

 $Description:	Gets the value of a character in the string.  Use this function rather than
 				accessing text data directly; it allows transparent ASCII and Unicode
 				handling.
 
 $Returns:		The unicode character at the required position

 $Arguments:	text		- The text string
 				textIndex	- The position of the character to get

*******************************************************************************/

USHORT ATB_string_GetChar(T_ATB_TEXT *text, USHORT textIndex)
{
	USHORT *unicode;
	USHORT character;
	
	if (text->dcs==ATB_DCS_UNICODE)
	{
		unicode = (USHORT *)text->data;
		character = unicode[textIndex];
	}
	else
		character = ATB_char_Unicode(text->data[textIndex]);

	return character;
}


/*******************************************************************************

 $Function:		ATB_string_MoveLeft

 $Description:	Move part of string left by 'offset' characters
 
 $Returns:		TRUE if the operation was successful

 $Arguments:	text		- The text string
 				textIndex	- The place where the newly moved section is to go
 				offset		- The number of characters to move the section left by

*******************************************************************************/
  
BOOL ATB_string_MoveLeft(T_ATB_TEXT *text, USHORT textIndex, USHORT offset)
{
	BOOL outcome;
	USHORT character;
	
	/* Copy until end of line char reached */

	while ((textIndex+offset)<=text->len)
	{
		character = ATB_string_GetChar(text, textIndex+offset);
		ATB_string_SetChar(text, textIndex, character);
		textIndex++;
	}
	outcome = TRUE;
	text->len -= offset;
	
	return outcome;
}


/*******************************************************************************

 $Function:		ATB_string_MoveRight

 $Description:	Move part of string right by 'offset' characters
 
 $Returns:		TRUE if the operation was successful

 $Arguments:	text		- The text string
 				textIndex	- The start of the section to be moved right
 				offset		- The number of characters to move the section right by
 				size		- The maximum possible length of the string

*******************************************************************************/

BOOL ATB_string_MoveRight(T_ATB_TEXT *text, USHORT textStart, USHORT offset, USHORT size)
{
	int textIndex;
	BOOL outcome;
	USHORT character;
	
	textIndex = text->len;
	
	if ((text->len+ offset) < size)	/* SPR#1995 - SH - <= should be < */
	{
		while (textIndex >= textStart)
		{
			character = ATB_string_GetChar(text, textIndex);
			ATB_string_SetChar(text, textIndex+offset, character);
			textIndex--;
		}
		text->len += offset;
		outcome = TRUE;
	}
	else
		outcome = FALSE;
	
	return outcome;
}


/*******************************************************************************

 $Function:		ATB_string_Length

 $Description:	Returns the length of the string, also setting the 'len' parameter of
 				the text structure
 
 $Returns:		Length of the string in characters

 $Arguments:	text		- The text string

*******************************************************************************/

USHORT ATB_string_Length(T_ATB_TEXT* text)
{


	if (text->dcs==ATB_DCS_UNICODE)
	{
		text->len = ATB_string_UCLength((USHORT *)text->data);
	}
	else
	{
		text->len = strlen((char *)text->data);
	}
	
	return text->len;
}


/*******************************************************************************

 $Function:		ATB_string_UCLength

 $Description:	Returns the length of the provided unicode string
 
 $Returns:		Length of the string in characters

 $Arguments:	text		- Pointer to the unicode string

*******************************************************************************/

USHORT ATB_string_UCLength(USHORT* text)
{
	USHORT 	length = 0;

	while (text[length]!=UNICODE_EOLN)
	{
		length++;
	}
	
	return (length);
}


/*******************************************************************************

 $Function:		ATB_string_Copy

 $Description:	Copy 'src' string into 'dest' string.
 
 $Returns:		None.

 $Arguments:	dest		- The destination string (allocated by caller)
 				src			- The source string (allocated by caller)

*******************************************************************************/

void ATB_string_Copy(T_ATB_TEXT* dest, T_ATB_TEXT* src)
{
	USHORT textIndex;
	USHORT character; 

	dest->dcs = src->dcs;
	
	for (textIndex=0; textIndex<src->len; textIndex++)
	{
		character = ATB_string_GetChar(src, textIndex);
		ATB_string_SetChar(dest, textIndex, character);
	}

	ATB_string_SetChar(dest, textIndex, UNICODE_EOLN);
	dest->len = textIndex;

	return;
}


/*******************************************************************************

 $Function:		ATB_string_Concat

 $Description:	Concatenate the second string, adding it to the end of the first.
 				N.B. The strings don't have to have the same DCS.
 
 $Returns:		None.

 $Arguments:	dest		- The destination string (allocated by caller)
 				src			- The source string (allocated by caller)

*******************************************************************************/

void ATB_string_Concat(T_ATB_TEXT* dest, T_ATB_TEXT* src)
{
	USHORT textIndex;
	USHORT character;
 	TRACE_EVENT_P4("dest_len:%d, src len:%d, dest dcs: %d, src dcs:%d", dest->len, src->len, dest->dcs, src->dcs);
 	
	for (textIndex = 0; textIndex<src->len; textIndex++)
	{
		character = ATB_string_GetChar(src, textIndex);
		/*SPR 2512, use destination length */
		ATB_string_SetChar(dest, textIndex+dest->len, character);
	}
	
	dest->len += src->len;
	/*SPR 2512, use destination length to find point to insert termination*/
	ATB_string_SetChar(dest, dest->len, UNICODE_EOLN);

	return;
}


/*******************************************************************************

 $Function:		ATB_string_Concat

 $Description:	Return position of character in string, or -1 if not found
 
 $Returns:		Position of character in string

 $Arguments:	text		- The text string to search
 				findChar	- The character to find

*******************************************************************************/

SHORT ATB_string_FindChar(T_ATB_TEXT* text, USHORT findChar)
{
	USHORT textIndex;
	SHORT foundPos;
	USHORT character;
	 
	textIndex = 0;
	foundPos = -1;

	character = ATB_string_GetChar(text, 0);

	while (character!=UNICODE_EOLN && foundPos==-1)
	{
		if (character==findChar)
			foundPos = (int)textIndex;
		textIndex++;
		character = ATB_string_GetChar(text, textIndex);
	}
	
	return foundPos;
}


/*******************************************************************************

 $Function:		ATB_string_ToUnicode

 $Description:	Convert a string to a unicode string.  Memory for the unicode
  				string should be allocated before calling.  Returns a pointer to the
  				unicode string.
  
 $Returns:		None.

 $Arguments:	dest		- The destination string (allocated by caller)
 				src			- The source string (allocated by caller)

*******************************************************************************/

void ATB_string_ToUnicode(T_ATB_TEXT *dest, T_ATB_TEXT *src)
{
	USHORT textIndex;
	USHORT character; 
	
	dest->dcs = ATB_DCS_UNICODE;

	for (textIndex=0; textIndex<src->len; textIndex++)
	{
		character = ATB_string_GetChar(src, textIndex);
		ATB_string_SetChar(dest, textIndex, character);
	}

	ATB_string_SetChar(dest, textIndex, UNICODE_EOLN);
	dest->len = textIndex;

	return;
}


/*******************************************************************************

 $Function:		ATB_string_ToAscii

 $Description:	Convert a string to an ascii string.  Memory for the ascii
  				string should be allocated before calling.  Returns a pointer to the
  				ascii string.
  
 $Returns:		None.

 $Arguments:	dest		- The destination string (allocated by caller)
 				src			- The source string (allocated by caller)

*******************************************************************************/

void ATB_string_ToAscii(T_ATB_TEXT *dest, T_ATB_TEXT *src)
{
	USHORT textIndex;
	USHORT character;
	 
	dest->dcs = ATB_DCS_ASCII;

	for (textIndex = 0; textIndex<src->len; textIndex++)
	{
		character = ATB_string_GetChar(src, textIndex);

		if (ATB_char_IsAscii(character) && ATB_char_IsVisible(character)) // Filter out non-ascii characters
		{
			character = ATB_char_Unicode('?');
		}

		ATB_string_SetChar(dest, textIndex, character);
	}

	return;
}


/*******************************************************************************

 $Function:		ATB_string_ToUnicode

 $Description:	Moves index position through text
  
 $Returns:		The new index.

 $Arguments:	text		- The text string
				textIndex	- Current position within the text buffer
				offset		- Negative for backwards movement, positive for forwards
							  movement.

*******************************************************************************/ 

USHORT ATB_string_IndexMove(T_ATB_TEXT *text, USHORT textIndex, SHORT offset)
{ 
	while (textIndex>0 && offset<0)
	{
		textIndex--;
		offset++;
	}

	while (ATB_string_GetChar(text, textIndex)!=UNICODE_EOLN && offset>0)
	{
		textIndex++;
		offset--;
	}

	return textIndex;
}


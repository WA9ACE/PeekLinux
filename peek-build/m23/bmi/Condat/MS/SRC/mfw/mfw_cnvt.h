/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_cnvt.h      $|
| $Author:: Es  $ CONDAT GmbH           $Revision:: 1               $|
| CREATED: 03.08.00                     $Modtime:: 03.08.00 18:23   $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_CNVT

   PURPOSE : String convertion

   EXPORT  :

   TO DO   :

   $History:: mfw_cnvt.h                                             $
*/

#ifndef _DEF_MFW_CNVT_H_
#define _DEF_MFW_CNVT_H_


/* #include "phb.h" RSA 20/12/01 - Removed */
#include"mfw_phb.h" //RSA 20/12/01 - Added
#include "mfw_cm.h"
#include "mfw_ss.h"

T_MFW_DCS mfw_getStrMode(UBYTE *str,UBYTE len);
T_MFW     mfw_SIM2GsmStr(UBYTE inLen, UBYTE *inMfwStr, UBYTE maxOutSize, 
                         UBYTE *outGsmStr);
T_MFW     mfw_SIM2UcsStr(UBYTE inLen, UBYTE *inMfwStr, UBYTE maxOutSize, 
                         U16 *outUcsStr);
T_MFW     mfw_Gsm2SIMStr(T_MFW_DCS outMode, UBYTE *inGsmStr, UBYTE maxOutSize, 
                         UBYTE *outMfwStr,UBYTE *outLen);
T_MFW     mfw_Ucs2SIMStr(T_MFW_DCS outMode, U16 *inUcsStr, UBYTE maxOutSize, 
                         UBYTE *outMfwStr, UBYTE *outLen);
/*CONQUEST 5992(related) MC added function to convert GSM string to ASCII*/
EXTERN void mfw_GSM_2_ASCII(UBYTE* gsm_string, UBYTE* ascii_string, SHORT length);

#endif

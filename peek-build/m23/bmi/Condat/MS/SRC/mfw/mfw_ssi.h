/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_ssi.h       $|
| $Author:: Vo                          $Revision::  1              $|
| CREATED: 25.1.99                      $Modtime:: 12.01.00 12:06   $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_SSI

   PURPOSE : Internal definitions for supplementary management of MMI framework


   $History:: mfw_ssi.h                                              $

	July 23, 2005   REF : MMI-22565 - a0876501
	Description:	IMEI CD byte calculation added
	Solution:	Byte alignment done for IMEI display.

 * 
 * *****************  Version 7  *****************
 * User: Vo           Date: 14.01.00   Time: 20:17
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * 
 * *****************  Version 6  *****************
 * User: Vo           Date: 13.09.99   Time: 10:36
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * new prototype
 * 
 * *****************  Version 5  *****************
 * User: Vo           Date: 30.08.99   Time: 11:45
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * add 'reason' in function ss_error_string()
 * 
 * *****************  Version 4  *****************
 * User: Vo           Date: 2.07.99    Time: 15:27
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 3  *****************
 * User: Vo           Date: 22.06.99   Time: 8:54
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 2  *****************
 * User: Vo           Date: 20.05.99   Time: 17:47
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 1  *****************
 * User: Vo           Date: 10.02.99   Time: 12:41
 * Created in $/GSM/DEV/MS/SRC/MFW
*/

#ifndef DEF_MFW_SSI
#define DEF_MFW_SSI

#include "ksd.h"

typedef enum              
{
    MFW_MODUL_UNKNOWN = 0,
    MFW_PHB,
    MFW_SIM_CPIND,
    MFW_SIM_CPINE
} T_MFW_SS_MODUL;
    
/* SS Control Block */
typedef struct
{
    T_MFW_EVENT     emask;          /* events of interest */
    T_MFW_EVENT     event;          /* current event */
    T_MFW_CB        handler;
    T_MFW_SS_PARA   para;
} T_MFW_SS;

/* Supplementary services capabilities */
typedef struct
{
    UBYTE speech;   /* speech support (vers. 1) */
    UBYTE data;     /* Data support             */
    UBYTE asyn;     /* T Async data support     */
    UBYTE syn;      /* NT Sync data support     */
    UBYTE aoc;
    UBYTE dtmf;
    UBYTE cf;
    UBYTE cb;
    UBYTE ussd;
    UBYTE etc;
    UBYTE clire;
    UBYTE clipr;
} T_MFW_SS_SUPORT;

T_MFW_SS_RETURN ss_decode   (UBYTE *string, CHAR **rt, T_KSD_SEQPARAM *p);
void            ss_notify   (T_MFW_SS_NOTIFY_CODES code,
						     SHORT                 index,
						     UBYTE                *number, 
						     UBYTE                *subaddr);
T_MFW           ss_set_clck (T_ACI_FAC fac, 
                             T_ACI_CLCK_MOD mode, 
                             CHAR *passwd,
                             T_ACI_CLASS class_type, 
                             UBYTE modul);
void            ss_command_info  (T_ACI_CLOG* clog);
void            ss_ok_string     (T_ACI_AT_CMD cmdId);
void            ss_error_string  (T_ACI_AT_CMD cmdId, T_ACI_CME_ERR reason);
void            ss_ok_clck      (void);
void            ss_error_clck   (void);
void            ss_ok_ussd      (T_ACI_AT_CMD cmdId);
void            ss_error_ussd   (T_ACI_AT_CMD cmdId, T_ACI_CME_ERR reason);
T_MFW_SS_NOTIFY_CODES ss_cvtCssiCodes(T_ACI_CSSI_CODE code);
T_MFW_SS_NOTIFY_CODES ss_cvtCssuCodes(T_ACI_CSSU_CODE code);


//	July 23, 2005   REF : MMI-22565 - a0876501
UBYTE getCdByteFromImei(UBYTE *imei);

#endif

/*
+--------------------------------------------------------------------+
| PROJECT: GSM-MFW (8417)               $Workfile:: Mfw_cb.c        $|
| $Author:: Ak                          $Revision::  1              $|
| CREATED: 13.10.98                     $Modtime:: 10.04.00 15:01   $|
| CREATED: 13.10.98                     $Modtime:: 20.01.00 14:29   $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_CB

   PURPOSE : This modul contains the functions for call back.


   $History:: Mfw_cb.c                                              $
       Aug 07, 2007 DR:OMAPS00137726 x0066814(Geetha)
       Description:HPLMN Manual Selection at power on -unable to perform manual selection at power ON
       Solution: In rAT_PlusCME, under case AT_CMD_NRG, added the code to list PLMN if Manual is the
                    network regieteration mode during bootup. 
   
       May 31, 2007 DR: OMAPS00133201 x0pleela
       Description: MS keeps searching networks and doesn't list available PLMNs after 
       			performing manual selection
	Solution : In function rAT_PlusCME(), for commands AT_CMD_COPS and 
			AT_CMD_NRG, passing the error code as parameter to function 
			nm_error_cops()

	June 05, 2007 DR:OMAPS00133241 x0061560(sateeshg)
       Description:ASTEC29607,SIM:The sim card should not return 9240 still.
       Solution     :Error indication from Aci E_PHB_ERROR sent to mmi.
       
		
   	May 10 2007 DR:OMAPS00127983 x066814
	Description: PSTN caller name not displayed on MS
	Solution: Added the following functions:
		1. set_cnap_name()
		2. get_cnap_name()
		3. send_cnap_name_information_mmi()
	Using these functions, added the code for rAT_PercentCNAP.

	Oct 05, 2006    ER: OMAPS00094496 x0061088(Prachi)
 	Description:Enhance RSSI to 3 parameters: Strength, quality, min-access-level
 	Solution:To solve the ER OMAPS0094496, rAT_PercentCSQ() is now receving four 
 	parameters i.e. RSSI,Bit Error rate, Actual level, Minimum access level and also 
 	rAT_PercentCSQ() is checking if actual level is less than minimum level and then 
 	calls globalSignalUpdate() to show the signal strength.
 

	Aug 01, 2006   DR: OMAPS00082374 x0pleela
	Description: EONS Failures - unexpected output from the MMI: 2 carriers; on the top always T-Mobile 
   	Solution: a) Added  the functionality which was removed before for rat_%CSTAT
	
	Jun 23, 2006    DR: OMAPS00082374 x0pleela
   	Description: EONS Failures - unexpected output from the MMI: 2 carriers; on the top always T-Mobile 
   	Solution: a) Removed the usage of global variable gEons_alloc_active
			b) Removed the call to function set_opn_sel_done(TRUE); in func rat_%CNIV
			c) Removed the functionality added before for rat_%CSTAT
   	
	July  17, 2006  DR: OMAPS00082792  x0039928
	Description:Changes to be done in MMI for Move all entries from SIM to FFS (Defect 76492)
	Solution: Ready signal is sent to MMI when rAT_Ok is called by ACI for CPBW command
	
    June 02, 2006    DR: OMAPS00078005 x0021334
    Descripton: Test for NITZ (Network Identity and Timezone) support - No New time was displayed
    Solution: New Time/Date received from network is sent to MFW by lower layer by invoking 'rAT_PercentCTZV'
    function. This function has now been implemented to pass on the received values to MMI
    for user confirmation and RTC updation.   

	May 18, 2006    DR: OMAPS00070657 x0pleela
   	Description: CPHS feature on Locosto-Lite 
   	Solution: 	Function "sendCPWIEvt" is called in rAT_PercentCPVWI to handle voice mail indication 
   			
	xrashmic 1 Jul, 2006 OMAPS00075784
       Changed U8 to T_ACI_CME_ERR in rAT_PlusCME

    June 15, 2006 OMAPS00079492 x0043642 
    Description : FT TMO (wk20/06) TC 9.1, 9.2 Multi Party, MS fails to return to idle mode
    Solution    : call set_cmdHold() to assign MFW_CM_DISCONNECT_ALL to variable cmd_hold 
                      when ATH command is entered.

	May 15, 2006    DVT: OMAPS00077751 x0043642
    Description: MFW needs to implement rAT_PercentSATT needed for SAT class E
    Solution:  Define a function  rAT_PercentSATT() 
	May 15, 2006 DR: OMAPS00067919 x0pleela
	Description: SIMP:On repeat Depersonalisation with invalid password MEPD unblock key should 
				be asked not Puk1.
	Solution:Made the following changes
		       1. Added functionality for rAT_PlusCPIN
			2. Made code changss in rat_plusCME for SIMP

     May 10, 2006    REF: DRT OMAPS00075829  xdeepadh
    Description: Submission of ACI and SAP changes for Imaging ERs 
    Solution: Using the mechanism where MFW can receive primitives without
    ACI to know about it.


    Apr 24, 2006    ERT: OMAPS00067603 x0043642
    Description: Engineering mode
    Solution:  Handle the response from ACI (through rAT_percentEINFO).
       

	Mar 15, 2006   ER: OMAPS00067709  x0pleela
	Description: Voice Buffering implementation on C+ for PTT via PCM API 
	Solution: Functions  rAT_NO_CARRIER, rAT_NO_ANSWER
			Changes: update the reason for calling record_stop
   
 *   Mar 23, 2006    REF:DR OMAPS00069173  a0393213
 *   Description:   GSM - MOC - AT USB - Live Network . Display problem when a MO call is made through AT interface
 *   Solution: misplaced break statement corrected
 

	Feb 15, 2006    ER: OMAPS00065203 x0pleela
   	Description: Cingular Operator Name Prioritization implementation required
   	Solution: Incorporated review comments
   			- Removed global variable gOper_sel_done added earlier

	Feb 09, 2006    ER: OMAPS00065203 x0pleela
   	Description: Cingular Operator Name Prioritization implementation required
   	Solution: Implemented functionality for rAT_PercentCNIV
		   	Implemented functionality for rAT_PercentCSTAT
   			
   	
 *
 *	Nov 02, 2005    DR OMAPS00052132   xdeepadh
 *	Bug:Not able to access the SMS inbox and also not able to send SMS in Locosto Lite build
 *	Fix: The ATI Enums usage has been put under the FF_ATI flag

	Nov 03, 2005 DR: OMAPS00052032 - xpradipg
	Description : Locosto: SIMP - integration of issues submitted by solDel on 
			     ME Personalization - changes as per the new interfaces
	Solution	:   The return value of sAT_PlusCLCK of AT_EXCT is handled and 
			    a please wait screen is displayed until the response is recieved
   
 *
 * June 01, 2005  REF: CRR 31559  x0021334
 * Description: TCS2.1.1 SW not buildable without engineering mode
 * Solution: Engineering Mode specific functions are guarded by 'FF_EM_MODE' flag
 *
 * x0018858 24 Mar, 2005 MMI-FIX-11321
 * Issue Description :- If the inbox is full and a new message is receivd, no indication is received.
 * Fix: Added support for handling indication when message box is full and
 * there is a pending message. 

 * x0047685, May 22 2006
 * Added R99 CC Interface changes.
 * rAT_PercentCMGRS moved under REL99 flag.
 *
 * *****************  Version 51  *****************
 * User: Vo           Date: 20.01.00   Time: 14:30
 * Updated in $/GSM/Condat/MS/SRC/MFW
 *
 * *****************  Version 50  *****************
 * User: Vo           Date: 20.01.00   Time: 12:24
 * Updated in $/GSM/Condat/MS/SRC/MFW
 *
 * *****************  Version 49  *****************
 * User: Vo           Date: 14.01.00   Time: 20:02
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * New: Save the setting for SMS parameter and cell broadcast
 * parameter
 *
 * *****************  Version 48  *****************
 * User: Vo           Date: 10.01.00   Time: 16:15
 * Updated in $/GSM/Condat/MS/SRC/MFW
 *
 * *****************  Version 47  *****************
 * User: Ak           Date: 5.01.00    Time: 13:27
 * Updated in $/GSM/Condat/MS/SRC/MFW
 *
 * *****************  Version 46  *****************
 * User: Vo           Date: 22.12.99   Time: 15:47
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * add "call id" for disconnect
 *
 * *****************  Version 45  *****************
 * User: Sab          Date: 2.12.99    Time: 9:25
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * Why: Improvement.
 * What: Implementation of phonebook ready indication.
 *
 * *****************  Version 44  *****************
 * User: Vo           Date: 11.11.99   Time: 14:31
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * New: DTMF
 *
 * *****************  Version 43  *****************
 * User: Vo           Date: 21.10.99   Time: 13:05
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * new implemention for command CPOL
 *
 * *****************  Version 42  *****************
 * User: Vo           Date: 10.09.99   Time: 19:14
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * Add CNUM and USSD
 *
 * *****************  Version 41  *****************
 * User: Vo           Date: 30.08.99   Time: 13:25
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * AoC
 * PIN unblock
 * call held
 *
 * *****************  Version 39  *****************
 * User: Vo           Date: 16.07.99   Time: 13:50
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 38  *****************
 * User: Es           Date: 6.07.99    Time: 12:02
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 37  *****************
 * User: Vo           Date: 2.07.99    Time: 19:02
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 36  *****************
 * User: Vo           Date: 2.07.99    Time: 15:25
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 35  *****************
 * User: Vo           Date: 22.06.99   Time: 12:53
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 34  *****************
 * User: Vo           Date: 21.06.99   Time: 20:59
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 32  *****************
 * User: Vo           Date: 14.06.99   Time: 13:56
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 31  *****************
 * User: Vo           Date: 14.06.99   Time: 10:06
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 30  *****************
 * User: Vo           Date: 1.06.99    Time: 14:46
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 29  *****************
 * User: Vo           Date: 20.05.99   Time: 17:26
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 28  *****************
 * User: Vo           Date: 28.04.99   Time: 16:11
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 26  *****************
 * User: Vo           Date: 31.03.99   Time: 14:09
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 24  *****************
 * User: Vo           Date: 15.03.99   Time: 13:40
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 23  *****************
 * User: Vo           Date: 11.03.99   Time: 18:32
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 22  *****************
 * User: Es           Date: 20.02.99   Time: 11:55
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 21  *****************
 * User: Vo           Date: 17.02.99   Time: 19:03
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 20  *****************
 * User: Vo           Date: 12.02.99   Time: 18:59
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 19  *****************
 * User: Vo           Date: 9.02.99    Time: 14:54
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 18  *****************
 * User: Vo           Date: 13.01.99   Time: 11:01
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 17  *****************
 * User: Vo           Date: 8.01.99    Time: 14:31
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 16  *****************
 * User: Es           Date: 17.12.98   Time: 17:25
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 15  *****************
 * User: Es           Date: 9.12.98    Time: 15:12
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 14  *****************
 * User: Es           Date: 9.12.98    Time: 14:19
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * added some needed 'rAT_' functions (as dummies)
 *
 * *****************  Version 13  *****************
 * User: Es           Date: 8.12.98    Time: 16:53
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 11  *****************
 * User: Vo           Date: 27.11.98   Time: 13:19
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 10  *****************
 * User: Vo           Date: 24.11.98   Time: 21:40
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * first successful compilation of MFW
 *
 * *****************  Version 9  *****************
 * User: Vo           Date: 13.11.98   Time: 16:21
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 8  *****************
 * User: Vo           Date: 12.11.98   Time: 13:46
 * Updated in $/GSM/DEV/MS/SRC/MFW
|
| *****************  Version 7  *****************
| User: Le           Date: 11.11.98   Time: 14:47
| Updated in $/GSM/DEV/MS/SRC/MFW
|
| *****************  Version 6  *****************
| User: Le           Date: 11.11.98   Time: 13:35
| Updated in $/GSM/DEV/MS/SRC/MFW
|
| *****************  Version 5  *****************
| User: Le           Date: 10.11.98   Time: 16:07
| Updated in $/GSM/DEV/MS/SRC/MFW
|
| *****************  Version 4  *****************
| User: Le           Date: 10.11.98   Time: 11:08
| Updated in $/GSM/DEV/MS/SRC/MFW
|
| *****************  Version 3  *****************
| User: Le           Date: 27.10.98   Time: 15:59
| Updated in $/GSM/DEV/MS/SRC/MFW
|
| *****************  Version 2  *****************
| User: Le           Date: 21.10.98   Time: 16:20
| Updated in $/GSM/DEV/MS/SRC/MFW
|
| *****************  Version 1  *****************
| User: Le           Date: 21.10.98   Time: 10:33
| Created in $/GSM/DEV/MS/SRC/MFW
*/

#define ENTITY_MFW

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#if defined (NEW_FRAME)

#include "typedefs.h"
#include "Vsi.h"
#include "PEI.H"
#include "custom.h"
#include "gsm.h"

#else

#include "STDDEFS.H"
#include "custom.h"
#include "gsm.h"
#include "Vsi.h"

#endif
#include"rfs/rfs_api.h"

#include "mfw_mfw.h"
#include "mfw_nm.h"
#include "mfw_sim.h"
#include "mfw_simi.h"
#include "mfw_sima.h"
#include "mfw_nmi.h"
#include "mfw_phb.h"
#include "mfw_phbi.h"
#include "mfw_cm.h"
#include "mfw_cmi.h"
#include "mfw_sms.h"
#include "mfw_smsi.h"
#ifdef FF_CPHS_REL4
#include "mfw_cphs.h"
#endif

// June 02, 2006    DR: OMAPS00078005 x0021334
#ifdef FF_TIMEZONE
#include "mfw_td.h"
#endif

#include "drv_key.h"


#include "ksd.h"
#include "psa.h"

#if defined (FAX_AND_DATA)
#include "aci_fd.h"
#endif

#include "cmh.h"
#include "phb.h"
#include "cmh_phb.h"
#include "psa_sim.h"

#include "mfw_ss.h"
#include "mfw_ssi.h"

// ADDED BY RAVI - 7-10-2005
#if (defined(FF_2TO1_PS) || (BOARD == 61))
#define GPRS TI_GPRS
#endif
// END ADDITION BY RAVI - 7-10-2005

#ifdef GPRS /* kgt 12-05-2001 */

// ADDED BY RAVI - 7-10-2005
#if (defined(FF_2TO1_PS) || (BOARD == 61))
#include "aci_cmh.h"
#include "aci_all.h"
#include "ati_cmd.h"
#include "aci_cmd.h"
#include "aci.h"
#include "dti_conn_mng.h"
#include "dti_cntrl_mng.h"
#include "gaci.h" /* include types declared in GACI             */
#endif
// END ADDITION - RAVI - 7-10-2005

#include "aci_all.h"
#include "aci_cmh.h"  /* include types declared in ACI              */
#include "ati_cmd.h"
#include "aci_cmd.h"
#include "aci.h"
#include "dti_conn_mng.h"
#include "dti_cntrl_mng.h"
#include "gaci.h" /* include types declared in GACI  */
/*#include "gaci_cmh.h"  include types declared in GACI             */
#include "mfw_gprs.h"
#endif  /* GPRS */

#include "Mfw_band.h" /* SPR919 - SH */

#ifdef FF_CPHS
#include "cphs.h"
#endif /* ff_cphs */

//x0pleela  20 Mar, 2006  ER:OMAPS00067709
#ifdef FF_PCM_VM_VB
#include "mfw_aud.h"
#endif


#ifdef NEPTUNE_BOARD /*EngMode */
#include "mfw_em.h"

UBYTE RssiVal;
#endif





#if defined(FF_MMI_CAMERA_APP) ||defined(FF_MMI_FILEMANAGER)
#include "msl/inc/msl_api.h"
#include "mfw_fm.h"
#endif

#include "aci.h"
//May 10, 2006    REF: DRT OMAPS00075829  xdeepadh
#include "mfw_cb.h"


LOCAL void mfw_format_plmn_name(UBYTE dcs,UBYTE *in, UBYTE in_length,
                                UBYTE *out, UBYTE max_out_length);		


/*May 10,2007 DR:OMAP00127983 x0066814(Geetha)-begin*/
/*size of the name when converted from 7bit to 8bit alphabet*/
#define CNAP_EXPANDED_SIZE (((sizeof(BUF_nameString)+1)*8)/7)
//function to get the name from rAT_PercentCNAP 
void set_cnap_name(T_namePresentationAllowed *cnap_name_info );
//function to give the name from set_cnap_name to MMI
GLOBAL T_MFW_PHB_TEXT *get_cnap_name();
GLOBAL T_MFW_PHB_TEXT cnap_name;

typedef enum
{
  CNAP_PRES_ALLOWED = 0,
  CNAP_PRES_RESTRICTED,
  CNAP_NAME_UNAVAILABLE,
  CNAP_NAME_PRES_RESTRICTED
} T_ATI_CNAP_PRESENTATION;//possible presentation modes for CNAP

GLOBAL BOOL CALL_NAME_NETWORK = FALSE;//implies that name is not sent via network(rAT_%CNAP)
//Function to process the data sent from rAT_PercentCNAP based on presentation mode for CNAP
LOCAL void send_cnap_name_information_mmi(T_ATI_CNAP_PRESENTATION pres_mode,
                                      T_callingName *NameId);
/*May 10,2007 DR:OMAP00127983 x0066814(Geetha)-end*/
/********* currently not used ***************************************/
extern void sate_error(T_ACI_CME_ERR error);  // ADDED BY RAVI - 29-11-2005
GLOBAL void rAT_PlusCSMS (T_ACI_CSMS_SERV service,
                          T_ACI_CSMS_SUPP mt,
                          T_ACI_CSMS_SUPP mo,
                          T_ACI_CSMS_SUPP bm) {}

//x0pleela 21 Apr, 2006  DR: OMAPS00067919
//Requesting for any of the SIMP category pin if enabled based on the "code"
GLOBAL void rAT_PlusCPIN (T_ACI_CPIN_RSLT code) 
{
#ifdef SIM_PERS
	TRACE_FUNCTION("rAT_PlusCPIN()");
  sim_simlock_cpin_code(code );	
#endif
}

GLOBAL void rAT_PlusCLCK  ( T_ACI_CLSSTAT * clsStaLst) {}
GLOBAL void rAT_PlusCIMI  (CHAR *imsi) {}
GLOBAL void rAT_PlusCCFC  (T_ACI_CCFC_SET* setting) {}
GLOBAL void rAT_PlusCLIR  (T_ACI_CLIR_MOD  mode, T_ACI_CLIR_STAT stat) {}
GLOBAL void rAT_PercentCOLR ( T_ACI_COLR_STAT stat) {}
GLOBAL void rAT_PlusCRSM  ( SHORT           sw1,
                            SHORT           sw2,
                            SHORT           rspLen,
                            UBYTE          *rsp    ) {}
GLOBAL void rAT_PlusCSIM  ( SHORT           rspLen,
                            UBYTE          *rsp    ) {}

GLOBAL void rAT_PlusCNUM  (T_ACI_CNUM_MSISDN *msisdn, UBYTE num) 
{
#ifdef FF_CPHS_REL4
	static UBYTE     count = 0;
	UBYTE	i;

	TRACE_FUNCTION("rAT_PlusCNUM");
	for(i = 0; i < 2; i++)
	{
		count = count + check_msp_rec(&msisdn[i],count);
	}
	if (count == 2)
		qAT_PlusCNUM(CMD_SRC_LCL, 1);
	else
		send_msp_signal(count);
#endif
}

GLOBAL void rAT_PlusCCWV ( T_ACI_CCWV_CHRG charge ) {}

GLOBAL void rAT_PercentALS(T_ACI_ALS_MOD ALSmode) {}  /* has to be implemented in MFW */


/* May 15, 2006    REF:ER OMAPS00077751  */

#ifdef NEPTUNE_BOARD
EXTERN void set_cmdHold(int command);
EXTERN int globalSignalUpdate(U8 value);
EXTERN void sendCPWIEvt(UBYTE flag_set, USHORT line);
EXTERN void             cmhMM_CnvrtPLMN2INT( const T_PS_plmn_id *plmn,
                                                   USHORT * mccBuf,
                                                   USHORT * mncBuf );
#endif  /* NEPTUNE_BOARD */
/* End - remove warning Aug -11 */
/* May 15, 2006    REF:ER OMAPS00077751  */

#ifdef NEPTUNE_BOARD
/*******************************************************************************

 $Function:    rAT_PercentSATT

 $Description:CallBack function, needed for target builds with SAT class E enabled
 
 $Returns: void

 $Arguments: T_ACI_CMD_SRC  srcId
                    T_ACI_SATT_CS  cause
 
*******************************************************************************/

GLOBAL void rAT_PercentSATT( T_ACI_CMD_SRC  srcId, T_ACI_SATT_CS  cause ) 
{
    TRACE_FUNCTION("rAT_PercentSATT");
}
#endif

// ADDED BY RAVI - ACI CHANGES - 6-10-2005
#if (BOARD == 61)
GLOBAL void rAT_PlusCKEV( T_ACI_CKEV_VALUES      sCkevValues,
                          T_ACI_CMER_MODE_TYPE   mode) {}
GLOBAL void rAT_PercentCPKY(CHAR *keys, SHORT time, SHORT pause) {}
#endif
// END OF ADDITION - 6-10-2005

/*to be removed
#ifndef FF_2TO1_PS
GLOBAL void rAT_PercentRDL(T_ACI_CC_REDIAL_STATE state) {}
GLOBAL void rAT_PercentRDLB(T_ACI_CC_RDL_BLACKL_STATE state)  {}
GLOBAL void rAT_PercentCSTAT(T_ACI_STATE_MSG msgType) {}
GLOBAL void rAT_PercentCPRSM (T_ACI_CPRSM_MOD mode) {}
#else
#endif  */


/* SPR#1985 - SH - This is now implemented in mfw_cm.c */
/*GLOBAL void rAT_PercentCTYI (T_ACI_CTTY_NEG neg,
                             T_ACI_CTTY_TRX trx) {}*/

/********* current define *******************************************/
#ifdef SIM_TOOLKIT
extern char mfwSatMoAccepted;
#endif

#ifdef SIM_PERS
EXTERN T_MFW_SIMP_CLCK_FLAG sim_unlock_in_prog;
#endif

//x0018858 24 Mar, 2005 MMI-FIX-11321
//Added a global variable and a function to retreive any pending message.
//begin - x0018858 24 Mar, 2005 MMI-FIX-11321
UBYTE isMessageWaiting = FALSE;

/*
+--------------------------------------------------------------------+
| PROJECT: GSM-MFW (8417)               MODULE:  MFW_CB              |
| STATE  : code                         ROUTINE: sms_check_message_pending
+--------------------------------------------------------------------+
   PURPOSE :   To retreive the status from server wrt to any pending message.
*/
UBYTE sms_check_message_pending(void)
{
	TRACE_EVENT_P1(" the message status is -- %d", isMessageWaiting);
	return isMessageWaiting;
}

///end - x0018858 24 Mar, 2005 MMI-FIX-11321

/*
+--------------------------------------------------------------------+
| PROJECT: GSM-MFW (8417)               MODULE:  MFW_CB              |
| STATE  : code                         ROUTINE: rAT_OK              |
+--------------------------------------------------------------------+


   PURPOSE :   OK indication from ACI

*/

GLOBAL void rAT_OK ( T_ACI_AT_CMD cmdId )
{

  TRACE_EVENT_P1("rAT_OK %d",cmdId);
  switch ((int)cmdId)
  {
    case AT_CMD_CFUN:
      sim_ok_cfun ();
      break;

    case AT_CMD_CPBW:
// July  17, 2006  DR: OMAPS00082792  x0039928		
		phb_signal(E_PHB_READY, 0);
	break;
		
    case AT_CMD_COPS:
      nm_ok_cops ();
      break;

    case AT_CMD_NRG:
      nm_ok_cops ();
      break;

    case AT_CMD_PVRF:
      sim_ok_cpin ();
      break;

    case AT_CMD_CPIN:
      sim_ok_cpin ();
      break;

    case AT_CMD_A:
      cm_ok_connect();
      break;

    case AT_CMD_CMGD:
/* JVJ SPR1119 apparently not needed since the rAT_PlusCMGD() already handles it */
TRACE_EVENT("ACI has deleted SMS!!");
       sms_ok_delete ();
/*Issue 1432: 01/10/2002: Needed as called by ACI instead of rAT_PlusCMGD if concatenated SMS */
      break;

    case AT_CMD_CMGR:
      sms_ok_change ();
      break;

    case AT_CMD_CHUP:
      cm_ok_disconnect(CMD_SRC_LCL, 0);
      break;

    case AT_CMD_H:
      cm_ok_disconnect(CMD_SRC_LCL, 0);
      break;

    case AT_CMD_ABRT:
      cm_ok_disconnect(CMD_SRC_LCL, 0);
      break;

    case AT_CMD_D:
      cm_ok_connect();
      break;

    case AT_CMD_CPWD:
      sim_ok_cpinc();
      break;

    case AT_CMD_CHLD:
      cm_result_cmd(CMD_SRC_LCL, Present);
      break;

    case AT_CMD_CLCK:
#ifdef SIM_PERS		
//	Nov 03, 2005 DR: OMAPS00052032 - xpradipg
//	if the flag is set then SIM Personalization has triggered the CLCK 
	if( (sim_unlock_in_prog EQ SIMP_BOOTUP) OR
		(sim_unlock_in_prog EQ SIMP_MENU ) )
	{
		mfw_simlock_continue(TRUE);
		sim_unlock_in_prog = SIMP_NONE;
	}
	else
#endif	
      ss_ok_clck();
      break;

    case AT_CMD_CCFC:
      break;

    case AT_CMD_CCWA:
      break;

    case KSD_CMD_CB:
    case KSD_CMD_CF:
    case KSD_CMD_CL:
    case KSD_CMD_CW:
    case KSD_CMD_PWD:
    case KSD_CMD_UBLK:
      ss_ok_string(cmdId);
      break;

    case AT_CMD_CPUC:
    case AT_CMD_CAMM:
    case AT_CMD_CACM:
      cm_ok_aoc(cmdId);
      break;

    case AT_CMD_CUSD:
    case KSD_CMD_USSD:
      ss_ok_ussd(cmdId);
      break;
    case AT_CMD_CMGL:
        sms_ok_CMGL();
       break;
    case AT_CMD_CPMS:
    case AT_CMD_CMGW:
    case AT_CMD_CMSS:
    case AT_CMD_CMGS:
      break;

    case AT_CMD_CPOL:
      nm_ok_pref_plmn();
      break;

    case AT_CMD_VTS:
      cm_ok_dtmf();
      break;

    case AT_CMD_CSAS:
      sms_ok_csas();
      break;

#ifdef GPRS /* KGT 12-05-2001 */
  case AT_CMD_CGATT:
  case AT_CMD_CGDCONT:
  case AT_CMD_CGACT:
  case AT_CMD_CGQREQ:
  case AT_CMD_CGQMIN:
  case AT_CMD_CGDATA:
  case AT_CMD_CGPADDR:
  case AT_CMD_CGAUTO:
  case AT_CMD_CGANS:
  case AT_CMD_CGCLASS:
  case AT_CMD_CGEREP:
  case AT_CMD_CGREG:
  case AT_CMD_CGSMS:
      gprs_ok(cmdId);
      break;
#endif /* GPRS */

  /* SPR919 - SH*/
  case AT_CMD_BAND:
  	nm_ok_band();
  	break;
  /*MC, SPR 1392, call deflection successful*/
  case AT_CMD_CTFR:
  	cm_ok_CTFR();

  /* Marcus: CCBS: 14/11/2002: Start */
  case AT_CMD_CCBS:
    TRACE_EVENT("AT_CMD_CCBS");
    cm_ccbs_ok();
    break;
  /* Marcus: CCBS: 14/11/2002: Start */
  /*a0393213 cphs rel4*/
  #ifdef FF_CPHS_REL4
  case AT_CMD_P_CFIS:
  	{
		T_MFW_SIMOP_STATUS result=MFW_SIMOP_WRITE_OK;
	  	sendCPHSSignal(E_CPHS_SET_DV_STAT,&result);
  	}
  #endif
    default:
      break;
  }
}


/*
+--------------------------------------------------------------------+
| PROJECT: GSM-MFW (8417)               MODULE:  MFW_CB              |
| STATE  : code                         ROUTINE: rAT_PlusCME         |
+--------------------------------------------------------------------+


   PURPOSE :   Error indication from ACI

*/

GLOBAL void rAT_PlusCME   ( T_ACI_AT_CMD cmdId, T_ACI_CME_ERR err )
{
  TRACE_EVENT_P2("rAT_PlusCME(cmdId %d, err %d)",cmdId, err);

  switch ((int)cmdId) /*a0393213 lint warnings removal - typecasting done*/
  {
//x0061560 Date May16,07 (sateeshg) OMAPS00133241 
  case AT_CMD_CPBW:
   	phb_signal(E_PHB_ERROR, 0);
   	break;	
  case AT_CMD_COPS:
  	//x0pleela 30 May, 2007  DR: OMAPS00133201
  	//Passing the error code
    nm_error_cops ((T_ACI_CME_ERR)err);
    break;

  case AT_CMD_NRG:
  	/*Aug 07, 2007 DR:OMAPS00137726 x0066814(Geetha)*/
  	TRACE_EVENT_P1("CME ERROR VALUE in NRG: %d",err);
  	    if( (nm_get_mode() == NM_MANUAL) &&
			((err == CME_ERR_NoServ) ||(err == CME_ERR_LimServ) ||(err ==CME_ERR_Unknown)))
  	    	{
  	    			nm_get_plmn_list_during_bootup();
  	    	}
	    else
		{
  	  		//x0pleela 30 May, 2007  DR: OMAPS00133201
  			//Passing the error code
    			nm_error_cops ((T_ACI_CME_ERR)err);
		}
    break;
  case AT_CMD_CFUN:
    /* OMAPS00075177 - 17,Apr-2006 */
/* Signature of the function has been changed from U8 to T_ACI_CME_ERR. 
   because, when calling this function, it is converting from T_ACI_CME_ERR to U8. 
   It is loosing the negative values( the enum T_ACI_CME_ERR has negative values )
*/
    sim_error_cfun ((T_ACI_CME_ERR)err);
    break;

  case AT_CMD_CPIN:
    sim_error_cpin ((T_ACI_CME_ERR)err);
    break;

  case AT_CMD_PVRF:
    sim_error_cpin ((T_ACI_CME_ERR)err);
    break;

  case AT_CMD_CHLD:
    cm_result_cmd(CMD_SRC_LCL, NotPresent);
    break;

  case AT_CMD_CLCK:
#ifdef SIM_PERS  	
//x0pleela 27 Apr, 2006  DR: OMAPS00067919
	if(sim_unlock_in_prog EQ SIMP_BOOTUP)
	{
	    //xrashmic 1 Jul, 2006 OMAPS00075784
	    //Changed U8 to T_ACI_CME_ERR
		sim_error_cfun ((T_ACI_CME_ERR)err);
	}

//	Nov 03, 2005 DR: OMAPS00052032 - xpradipg
//	if the flag is set then SIM Personalization has triggered the CLCK 
	else if(sim_unlock_in_prog EQ SIMP_MENU)
	{
		sim_unlock_in_prog = SIMP_NONE;	/*a0393213 compiler warnings removal - FALSE changed to SIMP_NONE*/	
		mfw_simlock_continue(SIMP_NONE);
	}
	else
#endif	
    ss_error_clck();
    break;

    case AT_CMD_CPWD:
    sim_error_cpinc();
    break;

  case KSD_CMD_CB:
  case KSD_CMD_CF:
  case KSD_CMD_CL:
  case KSD_CMD_CW:
  case KSD_CMD_PWD:
  case KSD_CMD_UBLK:
    ss_error_string(cmdId, err);
    break;

  case AT_CMD_CPUC:
  case AT_CMD_CAMM:
  case AT_CMD_CACM:
    cm_error_aoc(cmdId, err);
    break;

  case AT_CMD_CNUM:
    phb_read_upn_eeprom();
    break;

  case AT_CMD_CUSD:
  case KSD_CMD_USSD:
    ss_error_ussd(cmdId, err);
    break;

  case AT_CMD_CPOL:
    nm_error_pref_plmn();
    break;

  case AT_CMD_VTS:
    cm_error_dtmf();
    break;

  case AT_CMD_D:
    cm_error_dial(err);
    break;

  case AT_CMD_CLAN:
#ifdef FF_2TO1_PS
    sim_read_lp_cnf(CAUSE_SIM_EF_INVALID,NULL);
#else
    sim_read_lp_cnf(SIM_CAUSE_EF_INVALID,NULL);
#endif
    break;

#ifdef GPRS /* KGT 12-05-2001 */
  case AT_CMD_CGATT:
  case AT_CMD_CGDCONT:
  case AT_CMD_CGACT:
  case AT_CMD_CGQREQ:
  case AT_CMD_CGQMIN:
  case AT_CMD_CGDATA:
  case AT_CMD_CGPADDR:
  case AT_CMD_CGAUTO:
  case AT_CMD_CGANS:
  case AT_CMD_CGCLASS:
  case AT_CMD_CGEREP:
  case AT_CMD_CGREG:
  case AT_CMD_CGSMS:
      gprs_error(cmdId, err);
      break;
#endif /* GPRS */

  /* Marcus: CCBS: 14/11/2002: Start */
  case AT_CMD_CCBS:
    TRACE_EVENT("AT_CMD_CCBS");
    cm_ccbs_error();
    break;

  case AT_CMD_H:
    cm_error_disconnect();
    break;
  /* Marcus: CCBS: 14/11/2002: Start */

 /*MC, SPR 1392, call deflection error*/
  case AT_CMD_CTFR:
  	cm_error_CTFR();
  break;

  /*SPR#2321 - DS - Handle SAT errors */
  case AT_CMD_SATE:
	sate_error(err);
  break;
  }
}



/*
+--------------------------------------------------------------------+
| PROJECT: GSM-MFW (8417)               MODULE:  MFW_CB              |
| STATE  : code                         ROUTINE: rAT_NO_CARRIER      |
+--------------------------------------------------------------------+


   PURPOSE :   No Carrier indication from ACI

*/

GLOBAL void rAT_NO_CARRIER( T_ACI_AT_CMD cmdId, SHORT cId )
{
  TRACE_FUNCTION ("rAT_NO_CARRIER()");

//x0pleela  14 Mar, 2006  ER:OMAPS00067709
#ifdef FF_PCM_VM_VB
//update the reason for calling record_stop
  if( get_voice_memo_type() EQ VOICE_BUFFERING )
    set_voice_buffering_rec_stop_reason(CALLED_PARTY_END_CALL);
#endif

#ifndef GPRS /* kgt 12-05-2001 */
  cm_error_connect(cId);
#else  /* GPRS */
  cm_error_connect(cId);
#endif /* GPRS */
}

/*
+--------------------------------------------------------------------+
| PROJECT: GSM-MFW (8417)               MODULE:  MFW_CB              |
| STATE  : code                         ROUTINE: rAT_BUSY            |
+--------------------------------------------------------------------+


   PURPOSE :   Connect indication from ACI

*/

GLOBAL void rAT_BUSY  ( T_ACI_AT_CMD cmdId, SHORT cId )
{
  TRACE_FUNCTION ("rAT_BUSY()");

#ifndef GPRS /* kgt 12-05-2001 */
  cm_error_connect(cId);
#else  /* GPRS */
  cm_error_connect(cId);
#endif /* GPRS */
}


/*
+--------------------------------------------------------------------+
| PROJECT: GSM-MFW (8417)               MODULE:  MFW_CB              |
| STATE  : code                         ROUTINE: rAT_NO_ANSWER       |
+--------------------------------------------------------------------+


   PURPOSE :   Connect indication from ACI

*/

GLOBAL void rAT_NO_ANSWER  ( T_ACI_AT_CMD cmdId, SHORT cId )
{
  TRACE_FUNCTION ("rAT_NO_ANSWER()");

//x0pleela  07 Mar, 2006  ER:OMAPS00067709
#ifdef FF_PCM_VM_VB
//update the reason for calling record_stop
  if( get_voice_memo_type() EQ VOICE_BUFFERING )
    set_voice_buffering_rec_stop_reason(CALLED_PARTY_NOT_AVAILABLE);
#endif

#ifndef GPRS /* kgt 12-05-2001 */
  cm_error_connect(cId);
#else  /* GPRS */
  cm_error_connect(cId);
#endif /* GPRS */
}


/*
+--------------------------------------------------------------------+
| PROJECT: GSM-MFW (8417)               MODULE:  MFW_CB              |
| STATE  : code                         ROUTINE: rAT_NO_DIALTONE     |
+--------------------------------------------------------------------+


   PURPOSE :   Connect indication from ACI

*/

GLOBAL void rAT_NO_DIALTONE  ( void )
{
  TRACE_FUNCTION ("rAT_NO_DIALTONE()");
}


/*
+--------------------------------------------------------------------+
| PROJECT: GSM-MFW (8417)               MODULE:  MFW_CB              |
| STATE  : code                         ROUTINE: rAT_CONNECT         |
+--------------------------------------------------------------------+


   PURPOSE :   Connect indication from ACI, only for data call

*/

GLOBAL void rAT_CONNECT ( T_ACI_AT_CMD cmdId,
                          T_ACI_BS_SPEED speed,
                          SHORT cId )
{
  TRACE_FUNCTION ("rAT_CONNECT()");

#ifndef GPRS /* kgt 12-05-2001 */

  if (speed==11)
    cm_wap_data_connected();
else
  cm_data_connect(cId);
#else  /* GPRS */
  cm_data_connect(cId);
#endif /* GPRS */
}


/*
+------------------------------------------------------------------+
| PROJECT : GSM-MFW (8417)               MODULE  :  MFW_CB         |
| STATE   : code                         ROUTINE : rAT_PlusCSSI    |
+------------------------------------------------------------------+


   PURPOSE :   Supplementary service notifications

*/

void rAT_PlusCSSI(T_ACI_CSSI_CODE code, SHORT index)
{
    TRACE_FUNCTION("rAT_PlusCSSI()");

    if (code == CSSI_CODE_NotPresent)
        return;

    ss_notify(ss_cvtCssiCodes(code), index, 0, 0);
}


/*
+------------------------------------------------------------------+
| PROJECT : GSM-MFW (8417)               MODULE  :  MFW_CB         |
| STATE   : code                         ROUTINE : rAT_PlusCSSU    |
+------------------------------------------------------------------+


   PURPOSE :   Supplementary service notifications

*/

void rAT_PlusCSSU  ( T_ACI_CSSU_CODE code,
                     SHORT           index,
                     CHAR           *number,
                     T_ACI_TOA      *type,
                     CHAR           *subaddr,
                     T_ACI_TOS      *satype)
{
    TRACE_FUNCTION("rAT_PlusCSSU()");

    if (code == CSSU_CODE_NotPresent)
        return;

    else if ((code == CSSU_CODE_OnHold) OR
             (code == CSSU_CODE_Retrieved))
        cm_notify(code, index, (UBYTE *)number);

    else
        ss_notify(ss_cvtCssuCodes(code),
                  index,
                  (UBYTE *)number,
                  (UBYTE *)subaddr);
}


/*
+------------------------------------------------------------------+
| PROJECT : GSM-MFW (8417)               MODULE  : MFW_CB          |
| STATE   : code                         ROUTINE : rAT_PercentRLOG |
+------------------------------------------------------------------+


   PURPOSE :

*/

void rAT_PercentRLOG(T_ACI_RLOG *rslt)
{
    T_MFW_CM_CALL_STAT stat;
    UBYTE              ton;
    UBYTE              mode;

    TRACE_FUNCTION("rAT_PercentRLOG()");


    TRACE_EVENT_P1("rslt->dest %d",rslt->dest);
    TRACE_EVENT_P1("rslt->atRslt %d",rslt->atRslt);


    if (rslt->dest > CMD_SRC_LCL)
    {
        switch (rslt->atRslt)
        {
            case RLOG_RSLT_OK:
                TRACE_EVENT_P1("rslt->rsltPrm.rOK.cmdId %d", rslt->rsltPrm.rOK.cmdId);
                if (rslt->rsltPrm.rOK.cmdId EQ AT_CMD_A
                    OR rslt->rsltPrm.rOK.cmdId EQ AT_CMD_D)
                    cm_rmt_ok_connect();
                if (rslt->rsltPrm.rOK.cmdId EQ AT_CMD_H
                    OR rslt->rsltPrm.rOK.cmdId EQ AT_CMD_CHUP)
                   {
                             //NM Berlin / UK integration 21.09.01
                            //Nov 02, 2005    DR OMAPS00052132   xdeepadh                          
                            //The usage of ATI enum is put under the flag FF_ATI
#if   defined (FF_ATI) || defined (NEPTUNE_BOARD)
                        /*June 15, 2006    DR:OMAPS00079492    x0043642 */
                        /* Set cmd_hold variable to MFW_CM_DISCONNECT_ALL to end all calls*/
                        set_cmdHold(MFW_CM_DISCONNECT_ALL);
                        cm_ok_disconnect(CMD_SRC_ATI_1, rslt->rsltPrm.rOK.cId);
#endif
                   }
                if (rslt->rsltPrm.rOK.cmdId EQ AT_CMD_ABRT)
                {
                    if (cm_search_callStat(rslt->rsltPrm.rOK.cId, &stat, &ton, &mode) == CM_OK)
                    {
                        if (stat == CALL_DEACTIVE)
                        	{
					//NM Berlin / UK integration 21.09.01
					//Nov 02, 2005    DR OMAPS00052132   xdeepadh
					//The usage of ATI enum is put under the flag FF_ATI
#ifdef FF_ATI	
					cm_ok_disconnect(CMD_SRC_ATI_1, rslt->rsltPrm.rOK.cId);
#endif 	//FF_ATI  
                        	} 
                    }
                }
                if (rslt->rsltPrm.rOK.cmdId EQ AT_CMD_CHLD)
                	{
				//NM Berlin / UK integration 21.09.01
				//Nov 02, 2005    DR OMAPS00052132   xdeepadh   
				//The usage of ATI enum is put under the flag FF_ATI
#ifdef FF_ATI					
				cm_result_cmd(CMD_SRC_ATI_1, Present);
#endif      //FF_ATI  
                	}

#ifdef GPRS /* JVJ 10-10-02 */
                /* SH - GPRS*/
                switch(rslt->rsltPrm.rOK.cmdId)
                {
                    case AT_CMD_CGDCONT:
                    case AT_CMD_CGQREQ:
                    case AT_CMD_CGQMIN:
                    case AT_CMD_CGATT:
                    case AT_CMD_CGACT:
                    case AT_CMD_CGDATA:
                    case AT_CMD_CGPADDR:
                    case AT_CMD_CGAUTO:
                    case AT_CMD_CGANS:
                    case AT_CMD_CGCLASS:
                    case AT_CMD_CGEREP:
                    case AT_CMD_CGREG:
                    case AT_CMD_CGSMS:
                    case AT_CMD_SNCNT:
                    case AT_CMD_P_CGREG:
                        gprs_ok((T_ACI_AT_CMD)rslt->rsltPrm.rOK.cId);
                        break;
                }
                /* end SH */
#endif
			//Mar 23, 2006 DR OMAPS00069173 Prabakar R
			//break was put inside #ifdef GPRS. it has been moved outside
                break;
            case RLOG_RSLT_NoCarrier:
#ifdef GPRS /* JVJ 10-10-02 */
                /* SH - don't send to CM for GPRS call*/
                if (rslt->rsltPrm.rNO_CARRIER.cmdId == AT_CMD_CGDATA)
                {
                    gprs_err_connect(rslt->rsltPrm.rNO_CARRIER.cId);
                }
                else
#endif
                cm_rmt_error_connect(rslt->rsltPrm.rNO_CARRIER.cId);

                break;
            case RLOG_RSLT_Busy:
                /* SH - don't send to CM for GPRS call*/
#ifdef GPRS /* JVJ 10-10-02 */
                if (rslt->rsltPrm.rBUSY.cmdId == AT_CMD_CGDATA)
                    gprs_err_connect(rslt->rsltPrm.rBUSY.cId);
                else
#endif
                    cm_rmt_error_connect(rslt->rsltPrm.rBUSY.cId);
                break;

            case RLOG_RSLT_NoAnswer:
                /* SH - don't send to CM for GPRS call*/
#ifdef GPRS /* JVJ 10-10-02 */
                if (rslt->rsltPrm.rNO_ANSWER.cmdId == AT_CMD_CGDATA)
                    gprs_err_connect(rslt->rsltPrm.rNO_ANSWER.cId);
                else
#endif
                    cm_rmt_error_connect(rslt->rsltPrm.rNO_ANSWER.cId);
                break;

            case RLOG_RSLT_Connect:
                //NM
                //merged latest version of MFW from Berlin
                //only related with NO_ASCIIZ
                //code 1: was replaced by code 2
                //keep it still comment out the code 1
                //by reason of problem swap the codes

                //code 1:
                //cm_data_connect(rslt->rsltPrm.rCONNECT.cId);

                /* notify MMI of connect by external MMI */
                //code 2:

                /* SH - don't send to CM for GPRS call*/
#ifdef GPRS /* JVJ 10-10-02 */
                if (rslt->rsltPrm.rCONNECT.cmdId == AT_CMD_CGDATA)
                    gprs_connect(rslt->rsltPrm.rCONNECT.cId);
                else
                /* SH end */
#endif
                    cm_rmt_ok_connect(); /* VO ???: to be changed (needs E_CM_CONNECT) */


                break;
#ifdef GPRS /* JVJ 10-10-02 */
            case RLOG_RSLT_CME:
                /* SH - GPRS*/
		TRACE_FUNCTION_P1("rslt->rsltPrm.rOK.cmdId=%d",rslt->rsltPrm.rOK.cmdId);
                switch(rslt->rsltPrm.rOK.cmdId)
                {
                    case AT_CMD_CGDCONT:
                    case AT_CMD_CGQREQ:
                    case AT_CMD_CGQMIN:
                    case AT_CMD_CGATT:
                    case AT_CMD_CGACT:
                    case AT_CMD_CGDATA:
                    case AT_CMD_CGPADDR:
                    case AT_CMD_CGAUTO:
                    case AT_CMD_CGANS:
                    case AT_CMD_CGCLASS:
                    case AT_CMD_CGEREP:
                    case AT_CMD_CGREG:
                    case AT_CMD_CGSMS:
                    case AT_CMD_SNCNT:
                    case AT_CMD_P_CGREG:
                        gprs_error(rslt->rsltPrm.rCME.cmdId,rslt->rsltPrm.rCME.err);
                        break;
			//CQ-16432 start
			case AT_CMD_CPWD:
			TRACE_FUNCTION("AT_CMD_CPWD");
			   ss_sat_passwdReqd(rslt->rsltPrm.rCME.cmdId,rslt->rsltPrm.rCME.err);
			   break;
			 //CQ-16432 end
                }
                /* end SH */
                break;
#endif
        }
    }
}


/*
+------------------------------------------------------------------+
| PROJECT : GSM-MFW (8417)               MODULE  : MFW_CB          |
| STATE   : code                         ROUTINE : rAT_PercentCLOG |
+------------------------------------------------------------------+


   PURPOSE :

*/

void rAT_PercentCLOG(T_ACI_CLOG *cmd)
{
    TRACE_FUNCTION("rAT_PercentCLOG() in mfw");

    if (cmd->retCode NEQ AT_CMPL AND cmd->retCode NEQ AT_EXCT)
        return;

    switch (cmd->cmdType)
    {
        case CLOG_TYPE_Set:
            switch ((int)(cmd->atCmd)) /*a0393213 lint warnings removal - typecast done*/
            {
                case AT_CMD_D:
                    if (cmd->cmdPrm.sD.simCallCtrl NEQ D_SIMCC_ACTIVE_CHECK)
                        cm_AT_D(cmd);
                    break;
#ifdef SIM_TOOLKIT
                case AT_CMD_A:
                    if (mfwSatMoAccepted)
                    {                   /* ES!! accepted SAT Call   */
                        mfwSatMoAccepted = 0;
                        cm_AT_D(cmd);
                    }
                    break;
#endif

                case KSD_CMD_CB:
                case KSD_CMD_CF:
                case KSD_CMD_CW:
                case KSD_CMD_CL:
                case KSD_CMD_PWD:
                case KSD_CMD_UBLK:
                case KSD_CMD_USSD:
                case AT_CMD_CUSD:
                    ss_command_info(cmd);
                    break;

                case AT_CMD_CHLD:
                    cm_AT_CHLD(cmd);
                    break;

                default:
                    break;
            }
            break;

        case CLOG_TYPE_Query:
            break;

        case CLOG_TYPE_Test:
            break;
    }
}


/*
+--------------------------------------------------------------------+
| PROJECT: GSM-MFW (8417)               MODULE:  MFW_CB              |
| STATE  : code                         ROUTINE: rAT_PercentDRV      |
+--------------------------------------------------------------------+


   PURPOSE :   Driver indication from layer 1

*/

GLOBAL void rAT_PercentDRV( T_ACI_DRV_DEV device,
                            T_ACI_DRV_FCT function,
                            UBYTE         val1,
                            UBYTE         val2)
{
  TRACE_FUNCTION ("rAT_PercentDRV()");
//ES!! check device & function !!


    drvKeyUpDown(val2,val1); //ES!! val2 korrekt ?
}

/*
+--------------------------------------------------------------------+
| PROJECT: GSM-MFW (8417)               MODULE:  MFW_CB              |
| STATE  : code                         ROUTINE: send_cnap_name_information_mmi   |
+--------------------------------------------------------------------+


   PURPOSE :  Function to process the data sent from rAT_PercentCNAP based on presentation mode for CNAP 

*/
/*May 10,2007 DR:OMAP00127983 x0066814(Geetha)-begin*/
LOCAL void send_cnap_name_information_mmi(T_ATI_CNAP_PRESENTATION pres_mode,
                                      T_callingName *NameId)
{
  T_namePresentationAllowed *cnap_name_info = NULL;
    
  TRACE_FUNCTION("send_cnap_name_information()");

  switch(pres_mode)
  {
    case(CNAP_PRES_ALLOWED):
      TRACE_EVENT("CNAP: name presentation is allowed");
       CALL_NAME_NETWORK=TRUE;//implies that name is got from rAT_%CNAP
      cnap_name_info = &(NameId->namePresentationAllowed);
      break;

    case(CNAP_NAME_PRES_RESTRICTED):
    case(CNAP_NAME_UNAVAILABLE):
    case(CNAP_PRES_RESTRICTED):
      TRACE_EVENT("CNAP: name presentation is restricted/unavilable");
	break;

    default:
      TRACE_ERROR("CNAP: wrong presentation mode");
      return;
  }
 set_cnap_name(cnap_name_info);
}

/*
+--------------------------------------------------------------------+
| PROJECT: GSM-MFW (8417)               MODULE:  MFW_CB              |
| STATE  : code                         ROUTINE:     set_cnap_name  |
+--------------------------------------------------------------------+


   PURPOSE :  function to get the name from rAT_PercentCNAP

*/
void set_cnap_name(T_namePresentationAllowed *cnap_name_info )
{
TRACE_FUNCTION("set_cnap_name");
  if(cnap_name_info EQ NULL)
  {
    TRACE_ERROR("CNAP cnap_name_info is NULL");
    return;
  }

  if ((cnap_name_info->v_dataCodingScheme) AND
    (cnap_name_info->v_lengthInCharacters) AND
    (cnap_name_info->v_nameString))
  	{
	UBYTE expanded_len=0;
       UBYTE  *cnap_name_expanded = NULL;
	UBYTE dcs;
	TRACE_FUNCTION("All values present");
       cnap_name_expanded=mfwAlloc(CNAP_EXPANDED_SIZE+1);
	dcs =(UBYTE)cnap_name_info->dataCodingScheme;
		
if( cmh_getAlphabetCb( (UBYTE)dcs ) EQ 0 ) /* 7bit alphabet */
    {
    TRACE_FUNCTION("/* 7bit alphabet */");
      expanded_len =
        utl_cvt7To8((UBYTE *)cnap_name_info->nameString.b_nameString,
                    (UBYTE)cnap_name_info->lengthInCharacters,
                    cnap_name_expanded,
                    0 );	
    }
    else
    {
     TRACE_FUNCTION("/* 8bit alphabet */");
      memcpy( cnap_name_expanded,
              (UBYTE *)cnap_name_info->nameString.b_nameString,
              (UBYTE)cnap_name_info->lengthInCharacters );
      expanded_len = (UBYTE)cnap_name_info->lengthInCharacters;
    }
	  cnap_name.len= (UBYTE)expanded_len;
#ifdef NO_ASCIIZ		
	 memcpy(cnap_name.data,cnap_name_expanded,sizeof(cnap_name.data));
#else
	 strcpy(cnap_name.data,cnap_name_expanded);
#endif
         TRACE_EVENT_P1("cnap_name.data: %s",cnap_name.data);
	  mfwFree(cnap_name_expanded, CNAP_EXPANDED_SIZE+1);
  }
  return;
}

/*
+--------------------------------------------------------------------+
| PROJECT: GSM-MFW (8417)               MODULE:  MFW_CB              |
| STATE  : code                         ROUTINE:   get_cnap_name    |
+--------------------------------------------------------------------+


   PURPOSE :  function to give the name from set_cnap_name to MMI 

*/
GLOBAL T_MFW_PHB_TEXT * get_cnap_name()
{
TRACE_FUNCTION("get_cnap_name");
return &cnap_name;
}

/*
+--------------------------------------------------------------------+
| PROJECT: GSM-MFW (8417)               MODULE:  MFW_CB              |
| STATE  : code                         ROUTINE: rAT_PercentCNAP     |
+--------------------------------------------------------------------+

  PURPOSE : handles rAT_PercentCNAP call back

*/

GLOBAL void rAT_PercentCNAP  ( T_callingName *NameId,T_ACI_CNAP_STATUS status)
{

  TRACE_FUNCTION("rAT_PercentCNAP()");
  TRACE_EVENT_P1("NameId: %d",NameId);
  if(NameId NEQ NULL)
  {
     if(NameId->v_namePresentationAllowed)
  {
    send_cnap_name_information_mmi(CNAP_PRES_ALLOWED, NameId);
    return;
  }

  if(NameId->v_presentationRestricted)
  {
    send_cnap_name_information_mmi(CNAP_PRES_RESTRICTED, NameId);
    return;
  }

  if(NameId->v_nameUnavailable)
  {
    send_cnap_name_information_mmi(CNAP_NAME_UNAVAILABLE, NameId);
    return;
  }

  if(NameId->v_namePresentationRestricted)
  {
    send_cnap_name_information_mmi(CNAP_NAME_PRES_RESTRICTED, NameId);
    return;
  }
 }
}
/*May 10,2007 DR:OMAP00127983 x0066814(Geetha) -end*/
/*
+--------------------------------------------------------------------+
| PROJECT :                             MODULE  : MFW_ACI_DUMMY      |
| STATE   : code                        ROUTINE : rAT_PlusCLAN       |
+--------------------------------------------------------------------+

  PURPOSE : handles AT_PlusCLAN call back

*/

GLOBAL void rAT_PlusCLAN  (T_ACI_LAN_SUP  *CLang )
{
  TRACE_FUNCTION("rAT_PlusCLAN()");

#ifdef FF_2TO1_PS /* kgt 12-05-2001 */
  sim_read_lp_cnf(CAUSE_SIM_NO_ERROR,CLang);
#else
  sim_read_lp_cnf(SIM_NO_ERROR,CLang);
#endif

}

/*
+--------------------------------------------------------------------+
| PROJECT :                             MODULE  : MFW_ACI_DUMMY      |
| STATE   : code                        ROUTINE : rAT_PlusCLAE       |
+--------------------------------------------------------------------+

  PURPOSE : handles AT_PlusCLAE call back

*/

GLOBAL void rAT_PlusCLAE  (T_ACI_LAN_SUP  *CLang )
{
  TRACE_FUNCTION("rAT_PlusCLAE()");
}

/*
+--------------------------------------------------------------------+
| PROJECT : ACI/MMI             MODULE  : ACI_RET            |
| STATE   : code                        ROUTINE : rAT_PercentCSQ       |
+--------------------------------------------------------------------+

  PURPOSE : handles rAT_PercentCSQ call back

  Shen,Chao

*/

//Oct 05, 2006    ER: OMAPS00094496 x0061088(Prachi)
#ifdef FF_PS_RSSI
#define MMI_MIN_RXLEV_FAULT 99
GLOBAL void rAT_PercentCSQ  (UBYTE rssi, UBYTE ber, UBYTE actlevel, UBYTE min_access_level)
#else
GLOBAL void rAT_PercentCSQ  (UBYTE rssi, UBYTE ber, UBYTE actlevel)
#endif
{

	static UBYTE prelevel=0;
#ifdef NEPTUNE_BOARD
	    RssiVal = rssi;
#endif

	TRACE_FUNCTION("rAT_PercentCSQ()");
   #ifdef FF_PS_RSSI
    if((min_access_level NEQ MMI_MIN_RXLEV_FAULT) AND (rssi < min_access_level ))
    	{
	  globalSignalUpdate(0);
      prelevel = 0;
    	}
	else 
   #endif	
	if( prelevel != actlevel )
	{
		globalSignalUpdate(actlevel);
		prelevel = actlevel;
	}

}



#ifdef FF_CPHS
/*
+--------------------------------------------------------------------+
| PROJECT :                             MODULE  : MFW_ACI_DUMMY      |
| STATE   : code                        ROUTINE : rAT_PercentCPVWI   |
+--------------------------------------------------------------------+

  PURPOSE : handles AT_PercentCPVWI call back

*/

GLOBAL void rAT_PercentCPVWI ( UBYTE         flag_set,
                               USHORT        line)
{ 
TRACE_FUNCTION("rAT_PercentCPVWI()");
	//18 May, 2006  DR: OMAPS00070657  x0pleela
	//Send this event when there is any voice mail
	sendCPWIEvt(flag_set, line); 
}

/*
+--------------------------------------------------------------------+
| PROJECT :                             MODULE  : MFW_ACI_DUMMY      |
| STATE   : code                        ROUTINE : rAT_PercentCPNUMS  |
+--------------------------------------------------------------------+

  PURPOSE : handles AT_PercentCPNUMS call back

*/
GLOBAL void rAT_PercentCPNUMS(UBYTE element_index,
                              UBYTE index_level,
                              CHAR  *alpha_tag,
                              CHAR  *number,
                              BOOL  premium_flag,
                              BOOL  network_flag,
                              UBYTE type_of_address)
{
}


/*
+--------------------------------------------------------------------+
| PROJECT :                             MODULE  : MFW_ACI_DUMMY      |
| STATE   : code                        ROUTINE : rAT_PercentCPCFU   |
+--------------------------------------------------------------------+

  PURPOSE : handles AT_PercentCPCFU call back

*/
GLOBAL void rAT_PercentCPCFU(UBYTE        flag_set,
                             T_CPHS_LINES line)
{}


/*
+--------------------------------------------------------------------+
| PROJECT :                             MODULE  : MFW_ACI_DUMMY      |
| STATE   : code                        ROUTINE : rAT_PercentCPROAM  |
+--------------------------------------------------------------------+

  PURPOSE : handles AT_PercentCPROAM call back

*/
GLOBAL void rAT_PercentCPROAM(UBYTE roam_status)
{}

#endif /* FF_CPHS */

/*
+--------------------------------------------------------------------+
| PROJECT :                             MODULE  : MFW_ACI_DUMMY      |
| STATE   : code                        ROUTINE : rAT_PlusCIEV       |
+--------------------------------------------------------------------+

  PURPOSE : dummy for +CIEV unsolicited message

*/
GLOBAL void rAT_PlusCIEV( T_ACI_MM_CIND_VAL_TYPE sCindValues,
                          T_ACI_MM_CMER_VAL_TYPE sCmerSettings )
{
  TRACE_FUNCTION("rAT_PlusCIEV()");

//x0018858 24 Mar, 2005 MMI-FIX-11321
//Added a switch statement to handle the indication received.
//begin - x0018858 24 Mar, 2005 MMI-FIX-11321

  switch(sCmerSettings.sCmerIndParam)
  {
    case CMER_INDICATOR_INVALID:
    case CMER_INDICATOR_0:
    case CMER_INDICATOR_1:
      break;
    case CMER_INDICATOR_2:
    {
      switch (sCindValues.sCindSmsFullParam)
      {
         case CIND_SMSFULL_INDICATOR_INVALID:
           break;
         case CIND_SMSFULL_INDICATOR_MEMFULL:
           TRACE_FUNCTION("CIND_SMSFULL_INDICATOR_MEMFULL");
           isMessageWaiting = TRUE;
           break;
         case CIND_SMSFULL_INDICATOR_MEMAVAIL:
           TRACE_FUNCTION("CIND_SMSFULL_INDICATOR_MEMAVAIL");           
           isMessageWaiting = FALSE;
           break;
         case CIND_SMSFULL_INDICATOR_MAX:
         default:
           break;
      }
    }
    case CMER_INDICATOR_TYPE_MAX:
    default:
    {
      break;
    }
  }
//end - x0018858 24 Mar, 2005 MMI-FIX-11321
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)              MODULE  : MFW_CB |
| STATE   : code          ROUTINE : mfw_format_plmn_name    |
+--------------------------------------------------------------------+

  PURPOSE : Decodes PLMN name to remote source format 

*/

void mfw_format_plmn_name(UBYTE dcs,UBYTE *in, UBYTE in_length, UBYTE *out, UBYTE max_out_length)
{ 
    USHORT size_of_plmn_name;
	UBYTE dest_len=0;
	UBYTE buf[33];
	
	memset(buf,33,0);	
	if(dcs ==0) /*GSM default */
	{
	  dest_len = utl_cvt7To8(in,in_length,buf,0);
	  utl_chsetFromGsm(buf,dest_len,out,max_out_length,&size_of_plmn_name,GSM_ALPHA_Def);
	  memset(buf, 0, 33);
    }
	else if(dcs ==1) /*UCS2 */
	{
	    utl_hexFromGsm(in,in_length,out,max_out_length,&size_of_plmn_name,GSM_ALPHA_Def,CSCS_ALPHA_7_Bit);
		
	}
}	



#ifndef FF_2TO1_PS/*
+--------------------------------------------------------------------+
| PROJECT :                             MODULE  : MFW_ACI_DUMMY      |
| STATE   : code                        ROUTINE : rAT_PercentCNIV    |
+--------------------------------------------------------------------+

  PURPOSE : dummy for %CNIV unsolicited message

*/
//x0pleela 09 Feb, 2006  ER:OMAPS00065203
GLOBAL void rAT_PercentCNIV (T_MMR_INFO_IND *mmr_info_ind)
{
	T_MFW_NETWORK_STRUCT plmn_netw_data;
	SHORT mcc = 0; 
       SHORT mnc = 0;
	
	TRACE_FUNCTION("rAT_PercentCNIV()");

      //copy plmn info into plmn_netw_data from mmr_info_ind
	memset(&plmn_netw_data, 0, sizeof(T_MFW_NETWORK_STRUCT) );

      if (mmr_info_ind->plmn.v_plmn)
      {
        cmhMM_CnvrtPLMN2INT( mmr_info_ind->plmn.mcc,
        						mmr_info_ind->plmn.mnc,
        						&mcc, &mnc );
      }
      /* build numeric plmn representation */
      	if ((mnc & 0x000F) EQ 0x000F)
	{
		   sprintf((char*)&plmn_netw_data.network_numeric, "%d%d",
	    				mcc, (mnc & 0x0FF0) >> 4);
	 }
	 else
	 {
	   	   sprintf((char*)&plmn_netw_data.network_numeric, "%d%d", mcc, mnc);
	 }	  
//Apr 13, 2007 DRT:OMAPS00122739 x0066814(Geetha)
	if (mmr_info_ind->short_name.v_name)  /* short name  */
  	{ 
  		TRACE_FUNCTION(" SHORT NAME ");
    		mfw_format_plmn_name(mmr_info_ind->short_name.dcs,(UBYTE*)mmr_info_ind->short_name.text,
                         mmr_info_ind->short_name.c_text,    (UBYTE *)plmn_netw_data.network_short.data, 33);
 	 }
  
 	if (mmr_info_ind->full_name.v_name) /* full name  */
  	{ 
  		TRACE_FUNCTION(" Long NAME ");
    		mfw_format_plmn_name(mmr_info_ind->full_name.dcs,(UBYTE*)mmr_info_ind->full_name.text, 
	                     mmr_info_ind->full_name.c_text, (UBYTE *)plmn_netw_data.network_long.data, 33);
  }

  plmn_netw_data.opn_read_from = NM_NITZ;

  nm_get_network_data(&plmn_netw_data);
}

#else             
                       /* DMPS MMREG SAP */
GLOBAL void rAT_PercentCNIV (T_MMREG_INFO_IND *mmr_info_ind)
{
  T_MFW_NETWORK_STRUCT plmn_netw_data;
  SHORT mcc = 0; 
  SHORT mnc = 0;
  
  TRACE_FUNCTION("rAT_PercentCNIV()");

  /* copy plmn info into plmn_netw_data from mmr_info_ind */
  memset(&plmn_netw_data, 0, sizeof(T_MFW_NETWORK_STRUCT));

  if (mmr_info_ind->v_plmn_id)
  {
    cmhMM_CnvrtPLMN2INT( &(mmr_info_ind->plmn_id), (USHORT *)&mcc, (USHORT *)&mnc );
  }
  /* build numeric plmn representation */
  if ((mnc & 0x000F) EQ 0x000F)
  {
      sprintf((char*)&plmn_netw_data.network_numeric, "%d%d",
              mcc, (mnc & 0x0FF0) >> 4);
	 
  }
  else
  {
          sprintf((char*)&plmn_netw_data.network_numeric, "%d%d", mcc, mnc);
		  
  }    

  if (mmr_info_ind->v_short_net_name AND /* short name only */
      !mmr_info_ind->v_full_net_name)
  { 
  TRACE_FUNCTION(" SHORT NAME ");
#ifdef NO_ASCIIZ  
    memcpy(plmn_netw_data.network_short.data, 
           mmr_info_ind->short_net_name.text, 
           mmr_info_ind->short_net_name.c_text);
#else
    strcpy(plmn_netw_data.network_short, (char*)mmr_info_ind->short_net_name.text);
#endif
  }
  else if (!mmr_info_ind->v_short_net_name AND
            mmr_info_ind->v_full_net_name)    /* full name only */
  {  
    TRACE_FUNCTION(" LONG NAME ");
#ifdef NO_ASCIIZ  
    memcpy( plmn_netw_data.network_long.data, 
          mmr_info_ind->full_net_name.text,  
          mmr_info_ind->full_net_name.c_text);
#else
    strcpy( plmn_netw_data.network_long, (char*)mmr_info_ind->full_net_name.text );
#endif    
  }
  else if (mmr_info_ind->v_short_net_name AND
            mmr_info_ind->v_full_net_name)
  {
  TRACE_FUNCTION(" LONG AND SHORT ");
#ifdef NO_ASCIIZ  
    memcpy( plmn_netw_data.network_long.data, 
          mmr_info_ind->full_net_name.text,  
          mmr_info_ind->full_net_name.c_text);
    memcpy( plmn_netw_data.network_short.data, 
        mmr_info_ind->short_net_name.text, 
        mmr_info_ind->short_net_name.c_text);
#else
    strcpy( plmn_netw_data.network_long, (char*)mmr_info_ind->full_net_name.text );
    strcpy( plmn_netw_data.network_short, (char*)mmr_info_ind->short_net_name.text);
#endif	  
	  }
  else
  {
	TRACE_EVENT("No name found");
  }

	plmn_netw_data.opn_read_from = NM_NITZ;

	nm_get_network_data(&plmn_netw_data);
}

#endif
/*
+--------------------------------------------------------------------+
| PROJECT :                       MODULE  : MFW_ACI_DUMMY            |
| STATE   : code                  ROUTINE : rAT_PercentCCCN          |
+--------------------------------------------------------------------+

  PURPOSE :
*/
GLOBAL void rAT_PercentCCCN ( T_ACI_FAC_DIR tDirection,
                              SHORT cId,
#ifdef FF_2TO1_PS
                              T_NAS_fac_inf *acFie )
#else
                              T_MNCC_fac_inf *acFie )
#endif
{
  TRACE_FUNCTION ("rAT_PercentCCCN ()");
}

/*
+--------------------------------------------------------------------+
| PROJECT :                       MODULE  : MFW_ACI_DUMMY            |
| STATE   : code                  ROUTINE : rAT_PercentCSSN          |
+--------------------------------------------------------------------+

  PURPOSE :
*/
GLOBAL void rAT_PercentCSSN ( T_ACI_FAC_DIR tDirection,
                              T_ACI_FAC_TRANS_TYPE tType,
#ifdef FF_2TO1_PS
                              T_NAS_fac_inf       *acFie )
#else
                              T_MNCC_fac_inf *acFie )
#endif
{
  TRACE_FUNCTION ("rAT_PercentCSSN ()");
}


#if defined (REL99) || defined (FF_2TO1_PS)

/*
+--------------------------------------------------------------------+
| PROJECT :                       MODULE  : MFW_ACI_DUMMY            |
| STATE   : code                  ROUTINE : rAT_PercentCMGRS         |
+--------------------------------------------------------------------+

  PURPOSE :
*/
GLOBAL void rAT_PercentCMGRS ( T_ACI_CMGRS_MODE  mode,
                                UBYTE             tp_mr,
                                UBYTE             resend_count,
                                UBYTE             max_retrans )
{
  TRACE_FUNCTION ("rAT_PercentCMGRS ()");
}

#endif /* REL99 OR FF_2TO1_PS*/
/*
+--------------------------------------------------------------------+
| PROJECT :                       MODULE  : MFW_ACI_DUMMY            |
| STATE   : code                  ROUTINE : rAT_PercentRDLB          |
+--------------------------------------------------------------------+

  PURPOSE :
*/
GLOBAL void rAT_PercentRDLB ( T_ACI_CC_RDL_BLACKL_STATE state )
{
  TRACE_FUNCTION ("rAT_PercentRDLB ()");
}
/*
+--------------------------------------------------------------------+
| PROJECT :                       MODULE  : MFW_ACI_DUMMY            |
| STATE   : code                  ROUTINE : rAT_PercentCPRSM         |
+--------------------------------------------------------------------+

  PURPOSE :
*/

GLOBAL void rAT_PercentCPRSM ( T_ACI_CPRSM_MOD mode )
{
  TRACE_FUNCTION ("rAT_PercentCPRSM ()");
}

/*
+--------------------------------------------------------------------+
| PROJECT :                       MODULE  : MFW_ACI_DUMMY            |
| STATE   : code                  ROUTINE : rAT_PercentCSTAT         |
+--------------------------------------------------------------------+

  PURPOSE :
*/

//x0pleela 13 Feb, 2006
//Querying for EONS data when %CSTAT returns EONS ready

GLOBAL void rAT_PercentCSTAT ( T_ACI_STATE_MSG msgType )
{
#ifndef NEPTUNE_BOARD
	T_MFW_NETWORK_STRUCT plmn_eons_data;
#endif
  
  TRACE_FUNCTION ("rAT_PercentCSTAT ()");
  #ifndef NEPTUNE_BOARD
  memset(&plmn_eons_data, 0, sizeof(T_MFW_NETWORK_STRUCT) );
  if( (msgType.entityId EQ STATE_MSG_EONS ) AND 
  	(msgType.entityState EQ ENTITY_STATUS_Ready) )
  {
    nm_get_COPN(&plmn_eons_data);  

    //send E_NM_OPN event 
    nm_get_network_data(&plmn_eons_data);
  }
#endif

}


/*
+--------------------------------------------------------------------+
| PROJECT :                       MODULE  : MFW_ACI_DUMMY            |
| STATE   : code                  ROUTINE : rAT_PercentRDL           |
+--------------------------------------------------------------------+

  PURPOSE :
*/

GLOBAL void rAT_PercentRDL ( T_ACI_CC_REDIAL_STATE state )
{
  TRACE_FUNCTION ("rAT_PercentRDL ()");
}

#ifndef FF_2TO1_PS
/*
+--------------------------------------------------------------------+
| PROJECT :                       MODULE  : MFW_ACI_DUMMY            |
| STATE   : code                  ROUTINE : rAT_PercentCMGR          |
+--------------------------------------------------------------------+

  PURPOSE :
*/

GLOBAL void rAT_PercentCMGR ( T_ACI_CMGL_SM*  sm,
                              T_ACI_CMGR_CBM * cbm)
{
  TRACE_FUNCTION ("rAT_PercentCMGR ()");
}

/*
+--------------------------------------------------------------------+
| PROJECT :                       MODULE  : MFW_ACI_DUMMY            |
| STATE   : code                  ROUTINE : rAT_PercentCMGL          |
+--------------------------------------------------------------------+

  PURPOSE :
*/

GLOBAL void rAT_PercentCMGL ( T_ACI_CMGL_SM * smLst)
{
  TRACE_FUNCTION ("rAT_PercentCMGL ()");
}


/*
+--------------------------------------------------------------------+
| PROJECT :                       MODULE  : MFW_ACI_DUMMY            |
| STATE   : code                  ROUTINE : rAT_*          |
+--------------------------------------------------------------------+

  PURPOSE : Following functions MOved from ATI. (ati_ret.c)
*/


GLOBAL void rAT_PercentCREG  ( T_ACI_CREG_STAT       status,
                               USHORT                lac,
                               USHORT                cid,
                               T_ACI_P_CREG_GPRS_IND gprs_ind)
{
  TRACE_FUNCTION("rAT_PercentCREG()");
}

#if defined (MFW) || defined (SMI) 
#ifdef FF_TIMEZONE
GLOBAL void rAT_PlusCTZV ( S32 timezone )
{
  TRACE_FUNCTION("rAT_PlusCTZV()");
}
#else
GLOBAL void rAT_PlusCTZV ( UBYTE* timezone )
{
  TRACE_FUNCTION("rAT_PlusCTZV()");
}
#endif
#endif


// June 02, 2006    DR: OMAPS00078005 x0021334
// Descripton: Test for NITZ (Network Identity and Timezone) support - No New time was displayed
// Solution: New Time/Date received from network is sent to MFW by lower layer by invoking 'rAT_PercentCTZV'
// function. This function has now been implemented to pass on the received values to MMI
// for user confirmation and RTC updation.
/*
+--------------------------------------------------------------------+
| PROJECT :                       MODULE  : MFW_ACI_DUMMY            |
| STATE   : code                  ROUTINE : rAT_PercentCTZV         |
+--------------------------------------------------------------------+

  PURPOSE :
*/
#if defined (MFW) || defined (SMI) 
GLOBAL void rAT_PercentCTZV (T_MMR_INFO_IND *mmr_info_ind, S32 timezone )
{
  #ifdef FF_TIMEZONE
  T_MFW_TIME_IND currclock;
  #endif 

  TRACE_FUNCTION("rAT_PercentCTZV()");

  #ifdef FF_TIMEZONE
  // Copy time related data received from network to 'currclock' 
  // to be passed to MMI.

  currclock.hour = mmr_info_ind->time.hour;
  currclock.minute = mmr_info_ind->time.minute;
  currclock.second = mmr_info_ind->time.second;

  // Copy date related data received from network to 'currclock' 
  // to be passed to MMI.  
  currclock.day = mmr_info_ind->time.day;
  currclock.month = mmr_info_ind->time.month;
  currclock.year = mmr_info_ind->time.year;

  // Pass time and date info to MMI
  nm_set_network_time(&currclock);
  #endif 
}
#endif

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6147)         MODULE  :     |
| STATE   : code                  ROUTINE :                     |
+--------------------------------------------------------------------+

  PURPOSE : dummy function, needs not to be used.
*/
GLOBAL void rAT_Z ( void ){}

/*
+--------------------------------------------------------------------+
| PROJECT :                       MODULE  : MFW_ACI_DUMMY            |
| STATE   : code                  ROUTINE : rAT_*          |
+--------------------------------------------------------------------+

  PURPOSE : Following functions MOved from ACI. (ati_em_ret.c)
*/

//    June 01, 2005  REF: CRR 31559  x0021334
//    Description: TCS2.1.1 SW not buildable without engineering mode
//    Solution: Engineering Mode specific functions are guarded by 'FF_EM_MODE' flag

#ifndef NEPTUNE_BOARD
#ifdef FF_EM_MODE
GLOBAL void rAT_PercentEM ( /*UBYTE srcId,*/ T_EM_VAL *val_tmp )
{
}

GLOBAL void rAT_PercentEMET ( /*UBYTE srcId,*/ T_EM_VAL val_tmp )
{

}

GLOBAL void rAT_PercentEMETS ( UBYTE entity )
{
}
#endif
#else 
#ifdef TI_PS_FF_EM
GLOBAL void rAT_PercentEM ( /*UBYTE srcId,*/ T_EM_VAL *val_tmp )
{
}

GLOBAL void rAT_PercentEMET ( /*UBYTE srcId,*/ T_EM_VAL val_tmp )
{

}

GLOBAL void rAT_PercentEMETS ( UBYTE entity )
{
}
#endif
#endif /*FF_EM_MODE*/

/*
+--------------------------------------------------------------------+
| PROJECT :                       MODULE  : MFW_ACI_DUMMY            |
| STATE   : code                  ROUTINE : rAT_*          |
+--------------------------------------------------------------------+

  PURPOSE : Following functions MOved from ACI. (ati_cphs.c)
*/



#ifndef MFW
#ifndef FF_MMI_RIV
/* dummy in case of SMI */
GLOBAL void rAT_PercentCPNUMS(UBYTE element_index,
                              UBYTE index_level,
                              CHAR  *alpha_tag,
                              CHAR  *number,
                              BOOL  premium_flag,
                              BOOL  network_flag,
                              UBYTE type_of_address)
{
}
#endif /*ndef FF_MMI_RIV */
#endif /* ndef MFW */

#ifndef MFW
#ifndef FF_MMI_RIV
/* dummy in case of SMI */
GLOBAL void rAT_PercentCPROAM(UBYTE roam_status)
{}
#endif /* ndef FF_MMI_RIV */
#endif /* ndef MFW */

#ifndef MFW
#ifndef FF_MMI_RIV
/* dummy in case of SMI */
GLOBAL void rAT_PercentCPVWI ( UBYTE         flag_set, 
                               USHORT        line)
{ }
#endif /* FF_MMI_RIV */
#endif /* MFW */
#endif /*FF_2TO1_PS*/

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)              MODULE  : MFW_ACI_DUMMY            |
| STATE   : code                        ROUTINE : rAT_PercentMBI       |
+--------------------------------------------------------------------+

  PURPOSE : handles callback for %MBI, which queries the mailbox identifiers
*/
#ifdef FF_CPHS_REL4
GLOBAL void rAT_PercentMBI(T_ACI_MBI* mbi)
{
	T_MFW_CPHS_REL4_MBI mfw_mbi;
	mfw_mbi.voice=mbi->mbdn_id_voice;
	mfw_mbi.fax=mbi->mbdn_id_fax;
	mfw_mbi.electronic=mbi->mbdn_id_email;
	mfw_mbi.other=mbi->mbdn_id_other;
	mfw_mbi.valid=1; /*1 means valid*/
	
	/*need to send MBI signal to BMI*/
	sendCPHSSignal(E_CPHS_REL4_GET_MBI, &mfw_mbi);	
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)              MODULE  : MFW_ACI_DUMMY            |
| STATE   : code                        ROUTINE : rAT_PercentMBI       |
+--------------------------------------------------------------------+

  PURPOSE : handles callback for %MBI, which queries the mailbox identifiers
*/

GLOBAL void rAT_PercentMBDN(T_ACI_MBDN* mbdn)
{

	/*send MBDN signal to BMI*/
	sendCPHSSignal(E_CPHS_REL4_GET_MBDN,mbdn);
}


extern  T_MFW_CPHS_REL4_MWIS mwis_stat[4];
/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)              MODULE  : MFW_ACI_DUMMY            |
| STATE   : code                        ROUTINE : rAT_PercentMWIS|
+--------------------------------------------------------------------+

  PURPOSE : handles callback for %MWIS, which queries the message waiting status*/
  
GLOBAL void rAT_PercentMWIS(T_ACI_MWIS_MWI* mwis)
{
	/*send MBDN signal to BMI*/
	int profile;
	static UBYTE count = 1;

//	memcpy(mwis_stat[count - 1],mwis,sizeof(T_MFW_CPHS_REL4_MWIS));
	mwis_stat[count-1].mwisStat = mwis->mwiStat;
	mwis_stat[count-1].voicemail = mwis->mwis_count_voice;
	mwis_stat[count-1].fax = mwis->mwis_count_fax;
	mwis_stat[count-1].electronic = mwis->mwis_count_email;
	mwis_stat[count-1].other = mwis->mwis_count_other;


	if(count == mfw_cphs_get_no_profile())
		sendCPHSSignal(E_CPHS_GET_VC_STAT,0);
	else
		count++;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)              MODULE  : MFW_ACI_DUMMY            |
| STATE   : code                        ROUTINE : rAT_PercentMWI|
+--------------------------------------------------------------------+

  PURPOSE : handles callback for %MWI. This function is called whenever message waiting status is received*/
GLOBAL void rAT_PercentMWI(UBYTE msp_id,T_ACI_MWIS_MWI* mwis)
{
	TRACE_FUNCTION("rAT_PercentMWI()");
//	memcpy(mwis_stat[msp_id - 1],mwis,sizeof(T_MFW_CPHS_REL4_MWIS));
	mwis_stat[msp_id - 1].mwisStat = mwis->mwiStat;
	mwis_stat[msp_id - 1].voicemail = mwis->mwis_count_voice;
	mwis_stat[msp_id - 1].fax = mwis->mwis_count_fax;
	mwis_stat[msp_id - 1].electronic = mwis->mwis_count_email;
	mwis_stat[msp_id - 1].other = mwis->mwis_count_other;
	
	sendCPHSSignal(E_CPHS_GET_VC_STAT,0);
}

#endif



/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)              MODULE  : MFW_ACI_DUMMY            |
| STATE   : code                        ROUTINE : rAT_PlusCLVL       |
+--------------------------------------------------------------------+

  PURPOSE : handles callback for +CLVL querying the volume
*/

GLOBAL void rAT_PlusCLVL(U8 drvVol)
{
  TRACE_FUNCTION ("rAT_PlusCLVL()");

  return;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)              MODULE  : MFW_ACI_DUMMY            |
| STATE   : code                        ROUTINE : rAT_PlusCRSL       |
+--------------------------------------------------------------------+

  PURPOSE : handles callback for +CRSL querying the volume
*/

GLOBAL void rAT_PlusCRSL(U8 drvVol)
{
  TRACE_FUNCTION ("rAT_PlusCRSL()");

  return;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)              MODULE  : MFW_ACI_DUMMY            |
| STATE   : code                        ROUTINE : rAT_PlusCMUT       |
+--------------------------------------------------------------------+

  PURPOSE : handles callback for +CMUT querying the volume
*/

GLOBAL void rAT_PlusCMUT(U8 drvMute)
{
  TRACE_FUNCTION ("rAT_PlusCMUT()");

  return;
}

/* Mar 14, 2006    REF:ER OMAPS00070811  */
#ifdef NEPTUNE_BOARD
/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : MFW_ACI_DUMMY            |
| STATE   : code                  ROUTINE : rAT_PercentCMGR          |
+--------------------------------------------------------------------+

 */
GLOBAL void rAT_PercentCMGR  ( T_ACI_CMGL_SM*  sm,
                               T_ACI_CMGR_CBM* cbm )
{
  TRACE_FUNCTION ("rAT_PercentCMGR()");
}
 
/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : MFW_ACI_DUMMY            |
| STATE   : code                  ROUTINE : rAT_PercentCMGL          |
+--------------------------------------------------------------------+

  PURPOSE : 
*/
GLOBAL void rAT_PercentCMGL ( T_ACI_CMGL_SM * smLst )
{
  TRACE_FUNCTION ("rAT_PercentCMGL()");
}

#endif

#if defined(TI_UMTS) && defined(TI_PS_3GPP_R5)
/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)              MODULE  : MFW_ACI_DUMMY      |
| STATE   : code                        ROUTINE : rAT_PercentMMCI    |
+--------------------------------------------------------------------+

  PURPOSE : handles callback for %MMCC indication cc service change
            fallback case ( SCUDIF)
*/

GLOBAL void rAT_PercentMMCI ( T_ACI_MMCI_STAT state)
{
  TRACE_FUNCTION("rAT_PercentMMCI()");

  return;
}

#endif /* TI_UMTS && TI_PS_3GPP_R5 */


#ifdef TI_PS_FF_EM

GLOBAL void rAT_PercentEINFO (U16 offset, U16 length, T_ENG_INFO_REQ *eng_info_req)
{
  TRACE_FUNCTION("rAT_PercentEINFO()");
  Mfw_em_response_EINFO(eng_info_req);
}

GLOBAL void rAT_PercentESINFO (U16 offset, U16 length, T_ENG_INFO_STATIC_REQ *eng_info_static_req)
{
  TRACE_FUNCTION("rAT_PercentESINFO()");
  Mfw_em_response_ESINFO(eng_info_static_req);
}

#endif /* TI_PS_FF_EM */

/*a0393213 cphs rel4*/

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)              MODULE  : ACI_RET            |
| STATE   : code                        ROUTINE : rAT_PercentCFIS    |
+--------------------------------------------------------------------+
  
  PURPOSE : handles rAT_PercentCFIS call back
  PARAMETERS : cfis    - Call forward indication status
 */
#ifdef FF_CPHS_REL4
GLOBAL void rAT_PercentCFIS ( T_ACI_CFIS_CFU *cfis )
{	
  T_MFW_CPHS_REL4_DV_STAT divert_status;
  TRACE_FUNCTION("rAT_PercentCFIS()");

  if(cfis->cfuStat & 0x01) /*first bit corresponds to voice*/
	divert_status.voice=MFW_CFLAG_SET;
  else
  	divert_status.voice=MFW_CFLAG_NOTSet;

  if(cfis->cfuStat & 0x02) /*second bit corresponds to fax*/
	divert_status.fax=MFW_CFLAG_SET;
  else
  	divert_status.fax=MFW_CFLAG_NOTSet;

  if(cfis->cfuStat & 0x04) /*third bit corresponds to data*/
	divert_status.data=MFW_CFLAG_SET;
  else
  	divert_status.data=MFW_CFLAG_NOTSet;
  
  memcpy(divert_status.voice_dialling_number,cfis->number,PHB_MAX_LEN);

  TRACE_EVENT_P4("voice  %x,  fax  %x,  data  %x,  number  %s",
  	divert_status.voice,divert_status.fax,divert_status.data,
  	divert_status.voice_dialling_number);
  
  sendCPHSSignal(E_CPHS_GET_DV_STAT,&divert_status);
}
#endif /* FF_CPHS_REL4 */


//May 10, 2006    REF: DRT OMAPS00075829  xdeepadh
//Using the mechanism where MFW can receive primitives without
// ACI to know about it
#if defined(FF_MMI_FILEMANAGER)||defined(FF_MMI_CAMERA_APP)
/*
+------------------------------------------------------------------------------
|  Function     :  mslPrimHandler
+------------------------------------------------------------------------------
|  Description  :  This fn is the premitieve handler for MSL
|
|  Parameters   :  opc-opc code
|                           data - data
|
|
|  Return       :  TRUE/FALSE
+------------------------------------------------------------------------------
*/
GLOBAL BOOL mslPrimHandler (USHORT opc, void * data)
{
TRACE_FUNCTION ("mslPrimHandler");
TRACE_EVENT_P1("opc is %x",opc);

  if (opc EQ MSL_MMI_IND)
  {
    Mslil_msl_mmi_ind((T_MSL_MMI_IND*)data);
    return TRUE;
  }

  return FALSE;
    
}

/*
+------------------------------------------------------------------------------
|  Function     :  Mslil_msl_mmi_ind
+------------------------------------------------------------------------------
|  Description  :  This fn is called when msl_mmi_ind is received
|
|  Parameters   :  msl_mmi_ind - MSL_MMI_IND primitive structure
|
|  Return       :  void
+------------------------------------------------------------------------------
*/

GLOBAL void Mslil_msl_mmi_ind ( T_MSL_MMI_IND *msl_mmi_ind)
{

  U32 ucp=msl_mmi_ind->ucp;
  /*a0393213 compiler warnings removal - removed variable status*/
  
  TRACE_FUNCTION ("Mslil_msl_mmi_ind");
  
	switch(ucp)
	{
#ifdef FF_MMI_CAMERA_APP	
	case  MSL_UCP_IMGCAP:/*MSL_UCP_IMGCAP*/
		mfw_msl_cam_cb(msl_mmi_ind->message, msl_mmi_ind->appData);
		break;
#endif		
#ifdef FF_MMI_FILEMANAGER		
	case  MSL_UCP_IMGTHMB:/*MSL_UCP_IMGTHMB*/
		//Call the mmi cb for imagethumbnail.
		mfw_fm_thmb_mslcb(msl_mmi_ind->message, msl_mmi_ind->appData);
		break;
	case  MSL_UCP_IMGVIEW:/*MSL_UCP_IMGPREVIEW*/
		//Call the mmi cb for magepreview.
		mfw_fm_view_mslcb(msl_mmi_ind->message, msl_mmi_ind->appData);
		break;
#endif		
	}

  PFREE(msl_mmi_ind);
}

/*
+------------------------------------------------------------------------------
|  Function     :  Msl_mslil_Callback
+------------------------------------------------------------------------------
|  Description  :  This function should be called by other MSL functions when a callback is to be called
|
|  Parameters   : handle - MSL handle
|			    ucp_type - Use case pipeline type
|			    appData  - application specific data
|			    message - status message
|
|  Return       :  void
|                  
+------------------------------------------------------------------------------
*/

void Msl_mslil_Callback(void* handle, U32 ucp_type, U32 tCMd, U32 tStatus)
{
#if defined (NEW_FRAME)
	EXTERN T_HANDLE hCommACI;
#else
	EXTERN T_VSI_CHANDLE hCommACI;
#endif

	PALLOC(msl_mmi_ind,MSL_MMI_IND);
      TRACE_FUNCTION ("Msl_mslil_Callback");
	msl_mmi_ind->ucp = ucp_type;
	msl_mmi_ind->handle = (U32)handle;
	msl_mmi_ind->message = tCMd;
	msl_mmi_ind->appData = (U32)tStatus;

#if defined (NEW_FRAME)
	PSENDX(ACI,msl_mmi_ind);
#else
#if defined (_TMS470)
	vsi_c_send("",hCommACI,D2P(msl_mmi_ind),
	        sizeof(T_PRIM_HEADER)+sizeof(T_MSL_MMI_IND));
#else
	PSEND(ACI,msl_mmi_ind);
#endif
#endif

	return;
}

#endif //#if defined(FF_MMI_FILEMANAGER)||defined(FF_MMI_CAMERA_APP)
/* EOF */


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_sim.c       $|
| $Author:: Vo                          $Revision::  1              $|
| CREATED: 13.10.98                     $Modtime:: 22.03.00 17:21   $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_SIM

   PURPOSE : This modul contains the functions for SIM management.


   $History:: mfw_sim.c
   
    May 30, 2007  DR: OMAPS00132483 x0pleela
    Description: MM: While playing an AAC, unplug USB freezes the audio
    Solution: Defined new functions mfw_get_Phlock_status(), mfw_set_Phlock_status( ), 
    			mfw_get_Auto_Phlock_status(), mfw_set_Auto_Phlock_status() to set and 
    			get the phone lock and automatic phone lock status
   
    Apr 03, 2007    ER: OMAPS00122561  x0pleela
    Description:   [ACI] Phone lock feature has to be supported by ACI
    Solution: Phone Lock ER implementation
   
      Feb 19, 2006 ER:OMAPS00115179 a0393213
      Description : support SIM insertion without powering down
      Solution	   : New case is handled in rAT_PercentSIMINS
   
	Dec 13, 2006 DR: OMAPS00105640  x0039928
 	Description: SIMP: The board reboots when removing SIM card.
 	Solution: Sim status is updated.
 	
	Nov 13, 2006 DR: OMAPS00103356  x0pleela
 	Description: SIMP:Master unlock failed through MMI
 	Solution: In function mfw_simlock_master_unlock() a check is added if the master unlock 
 			password is wrong. If wrong, displays Wrong password. 
 			Also a check is added if 	the error code for any of the categories (non-dependent) 
 			being locked/blocked. If yes, return OK as the dependent categories are unblocked 
 			using Master unblock password
 	   
   	Sep 26, 2006 DR: OMAPS00095524 x0pleela
 	Description:Implement 2 missing SIM Lock types
 	Solution: Made the following changes
		1) sim_error_cfun(), sim_error_cpin()
			- Setting the aciErrdesc separately for all the CME errors related to 
				SIMP categories
			- Handling the extension error of request for Blocked network password 
				(both PIN and PUK)
		2) mfw_simlock_check_lock_bootup()
			- Updating aciErrDesc with Blocked Network (PIN or PUK) password request
		3) mfw_simlock_return_lock_type()
			- Return coresponding ACI lock type for Blocked network category
		4) sim_simlock_cpin_code()
			- Set the reqduired parameters for Blocked Network Password and Unblock
				code request
			
    	Sep 26, 2006 DR: OMAPS00096565  x0pleela
 	Description: After blocking PIN, when ISAMPLE is reset, instead of prompting for PUK, master 
 				unlock key is asked
 	Solution: Reset C_KEY_REQ to 0 in functions sim_error_cfun() and sim_error_cpin()
 			
 	Sep 11, 2006 DR: OMAPS00094215  x0pleela
 	Description: MMI compilation failed as CME_ErrBusy undefined
 	Solution: Handling new Ext_ERR_Busy sent by ACI   

	xashmic 9 Sep 2006, OMAPS00092732
	USBMS ER

    	Sep 08, 2006 DR: OMAPS00091250 x0pleela
 	Description:The phone resets if no MEPD data is presented
 	Solution: CME_Unknown error is sent by ACI and is now handled by MFW in fucntions sim_error_cpin() and
 			sim_error_cfun

	July 21, 2006 ER: OMAPS00087586, OMAPS00087587 x0pleela
	Description: 	OMAPS00087586: MasterKey funtion in TCS3.2 SW
				OMAPS00087587: Time Penaly feature extend handset security
	Solution:Made the following changes
		1) Defined new static variable "Simp_Busy_State" which will be set to either TRUE or FALSE based on the ACI's busy error
		2) sim_error_cfun(): Update global aciErrDesc with the current CME error code
		3) mfw_simlock_check_lock_bootup(): Check for the CME busy error and update "Simp_Busy_State" accordingly
		4) mfw_simlock_return_lock_type(): Return coresponding lock type for master unlock 
		5) Added following new functions:
			mfw_simlock_master_unlock: This function unlocks(dependent categories) or unblocks ME
			mfw_simlock_check_masterkey: This function checks from ACI whether Master Unlock feature is enabled or not. If 
						enabled, displays a menu item for the user toy select master unlocking option. If not enabled, doesn't 
						display master unlocking option in the menu
			mfw_simlock_set_busy_state: This function sets Simp_Busy_State to either TRUE or FALSE based on ACI's CME error
			mfw_simlock_get_busy_state: This function returns the current value of Simp_Busy_State
	
	Jun 23, 2006    DR: OMAPS00082374 x0pleela
   	Description: EONS Failures - unexpected output from the MMI: 2 carriers; on the top always T-Mobile 
   	Solution:	a) Removed the usage of global variable gEons_alloc_active
			b) Removed the check for services 51 and 52 from function rat_pluscfunp
   	
	xrashmic 6 Jul, 2006 OMAPS00080708
	For error return value in sAT_PercentPVRF, bmi has to be informed
		
       xrashmic 1 Jul, 2006 OMAPS00075784
       Hanlding of SIMP events during bootup
            
	May 15, 2006 DR: OMAPS00067919 x0pleela
	Description: SIMP:On repeat Depersonalisation with invalid password MEPD unblock key should 
				be asked not Puk1.
	Solution:Made the following changes
			1. Added global variables (simp_cpin_code, simp_cpin_flag) to store the code result of qAT_PlusCPIN() result
			2. Using new enum type T_MFW_SIMP_CLCK_FLAG sim_unlock_in_prog instead of U8
			3. Added code in sim_error_cpin() to set the sim_status field to request for Unblocking code
			4. Modified the code to check SIMP during bootup sequence
			5. Added new function sim_simlock_cpin_code() which checks for the code and take action for SIMP category unlocking 
	
	Feb 15, 2006    ER: OMAPS00065203 x0pleela
   	Description: Cingular Operator Name Prioritization implementation required
   	Solution: Incorporated review comments
			Combined both checks for services 51 and 52 in rAT_PlusCFUNP

	Feb 09, 2006    ER: OMAPS00065203 x0pleela
   	Description: Cingular Operator Name Prioritization implementation required
   	Solution: Check for services 51 - OPL and 52 - PNN in function rAT_PlusCFUNP and 
   			gEons_alloc_active is updated accordingly
   	
	Nov 24, 2005 DR: OMAPS00045909 - Shashi Shekar B.S.
	Description: Improve IMEI control mechanism
	Solution : When MMI calls sAT_PLUSCFUN, if an IMEI invalid error is returned,
		we will block on that screen & will not allow the user to browse menus further,
		since the PS will not be booted at all!!!

    Nov 15,2005 	DR : OMAPS00057280	-	x0034700
 	Description:	Compilation Errors in Neptune build while integrating  latest MMI Changes
 	Solution:		Included the compilation FLAG "NEPTUNE_BOARD" for un wanted code for Neptune build

	Nov 03, 2005 DR: OMAPS00052032 - xpradipg
	Description : Locosto: SIMP - integration of issues submitted by solDel on 
			     ME Personalization - changes as per the new interfaces
	Solution	:   The return value of sAT_PlusCLCK of AT_EXCT is handled and 
			    a please wait screen is displayed until the response is recieved    

	Nov 03, 2005 DR: OMAPS00050595 - xpradipg
	Description: SIMP: If the SIM is blocked (by entering invalid PIN1), the user is 
			    not able to unblock it even after entering the valid PUK1
	Solution : The global flag for SIM Personlaization PUK request was set even
			for the SIM PUK request. This setting is removed. Also the condition
			check has been done first for the global flag and then the rest of the
			checks.
   
	Sept 24, 2005 REF:  LOCOSTO-ENH-34438, xpradipg
  	Description: Integration changes required with new aci labe
  	Solution: changes to adopt to the new ACI label   

	July 19, 2005 REF: CRR LOCOSTO-ENH-28173 xpradipg
  	Description: To provide MMI Support to enable/disable/change password and 
  				query all the Personalization locks
  	Solution: Integration of the changes for the same provided by the soldel 
  			  team
  			  
	Aug 16, 2004    REF: CRR 24323   Deepa M.D
	Bug:Clenup of sprintf used for tracing
	Fix:Replace the char buf[]; sprintf (buf, "...", ...); TRACE_EVENT (buf); statements by TRACE_EVENT_PX                                      $

    May 22, 2006 x0047685
    Removed pref_plmn from nm_mmi_parameter(...); due to REL99 EF handling.

 * * *****************  Version 43  *****************
 * User: MC           Date: 14.11.02  Time: 10:40
 * Updated in $/BMI/Condat/MS/SRC/MFW
 * Changed simlock functions into wrappers for aci_slock functions, and
 * removed variables definitions and helper functions rendered obsolete.
 * 
 * *****************  Version 43  *****************
 * User: Vo           Date: 22.03.00   Time: 17:29
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * Add new parameter PIN type in structure T_MFW_SIM_STATUS to
 * identify PIN type
 * Change in SIM lock
 *
 * *****************  Version 42  *****************
 * User: Es           Date: 2.03.00    Time: 16:18
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * use 'aci_delete()' instead of 'aci_ext_init()' to clear the
 * primitive routing for mfw_sim. (because other routings may have to
 * be preserved!)
 *
 * *****************  Version 41  *****************
 * User: Vo           Date: 22.12.99   Time: 16:03
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * Remove maximum preferred PLMN records and used preferred PLMN
 * records in local memory
 *
 * *****************  Version 40  *****************
 * User: Vo           Date: 2.12.99    Time: 13:18
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * New: request of max/used perferred PLMN list
 *
 * *****************  Version 39  *****************
 * User: Vo           Date: 17.11.99   Time: 11:37
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * Bug fix: de-personalisation for SIM lock
 * New: SIM inserted
 *
 * *****************  Version 38  *****************
 * User: Vo           Date: 21.10.99   Time: 13:26
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * add response of de-registration
|
| *****************  Version 37  *****************
| User: Le           Date: 11.10.99   Time: 16:27
| Updated in $/GSM/Condat/MS/SRC/MFW
|
| *****************  Version 36  *****************
| User: Le           Date: 8.10.99    Time: 13:38
| Updated in $/GSM/Condat/MS/SRC/MFW
|
| *****************  Version 35  *****************
| User: Le           Date: 4.10.99    Time: 10:33
| Updated in $/GSM/Condat/MS/SRC/MFW
 *
 * *****************  Version 34  *****************
 * User: Es           Date: 29.09.99   Time: 11:20
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * including CFUN_OK enhancement by AK, not fully implemented
 *
 * *****************  Version 33  *****************
 * User: Es           Date: 16.09.99   Time: 16:56
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * reordered include statements due to addons in mfw_phb.h
 *
 * *****************  Version 32  *****************
 * User: Vo           Date: 13.09.99   Time: 12:05
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * change comment
 *
 * *****************  Version 31  *****************
 * User: Vo           Date: 12.09.99   Time: 15:32
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * Bug fix: change PIN
 * Bug fix/New: SIM lock
 * New: request of PIN status
 * New: save preferred language
 * New: read group identifier level 1 & 2
 *
 * *****************  Version 30  *****************
 * User: Vo           Date: 30.08.99   Time: 14:23
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * bug fix
 * new function
 *
 * *****************  Version 29  *****************
 * User: Vo           Date: 9.08.99    Time: 13:11
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * PIN/PUK count
 *
 * *****************  Version 28  *****************
 * User: Vo           Date: 26.07.99   Time: 14:34
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * Move include file mfw_cb.h
 *
 * *****************  Version 27  *****************
 * User: Vo           Date: 21.07.99   Time: 14:41
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 26  *****************
 * User: Vo           Date: 16.07.99   Time: 13:50
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 25  *****************
 * User: Vo           Date: 9.07.99    Time: 14:34
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 24  *****************
 * User: Vo           Date: 2.07.99    Time: 19:03
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 23  *****************
 * User: Vo           Date: 28.05.99   Time: 9:52
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 22  *****************
 * User: Vo           Date: 31.03.99   Time: 14:09
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 21  *****************
 * User: Es           Date: 15.03.99   Time: 18:21
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * recode
 *
 * *****************  Version 20  *****************
 * User: Vo           Date: 25.02.99   Time: 16:47
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 19  *****************
 * User: Vo           Date: 17.02.99   Time: 19:03
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 18  *****************
 * User: Vo           Date: 12.02.99   Time: 18:05
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 17  *****************
 * User: Vo           Date: 12.02.99   Time: 17:46
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 16  *****************
 * User: Vo           Date: 9.02.99    Time: 14:54
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 15  *****************
 * User: Vo           Date: 12.01.99   Time: 14:27
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 14  *****************
 * User: Es           Date: 23.12.98   Time: 15:47
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 12  *****************
 * User: Es           Date: 18.12.98   Time: 13:05
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 11  *****************
 * User: Vo           Date: 17.12.98   Time: 14:31
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 10  *****************
 * User: Es           Date: 8.12.98    Time: 16:53
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 8  *****************
 * User: Vo           Date: 24.11.98   Time: 21:40
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * first successful compilation of MFW
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
#if defined (SIM_PERS) && !defined(NEPTUNE_BOARD)
#include "p_sim.h"
#endif
#include "prim.h"
#include "message.h"
#include "prim.h"
#include "aci_cmh.h"
#include "psa.h"
#include "psa_sim.h"

#ifndef PCM_2_FFS
#include "pcm.h"
#endif

#include "mfw_mfw.h"
#include "mfw_nm.h"
#include "mfw_sim.h"
#include "mfw_sima.h"
#include "mfw_simi.h"
#include "mfw_nmi.h"
#include "mfw_nma.h"
#include "mfw_phb.h"
#include "mfw_cm.h"
#include "mfw_cmi.h"
#include "mfw_acie.h"
#include "mfw_sms.h"
#include "mfw_smsi.h"
#include "mfw_win.h"


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

#include "aci_ext_pers.h"	/*MC, SPR 1351 include ACI simlock files*/
#include "aci_slock.h" 

#ifdef SIM_TOOLKIT
#include "mfw_sat.h"
#endif


extern BOOL sima_response_cb (ULONG opc, void * data);
extern void  simDecodeIMSI (UBYTE *ci, UBYTE li, UBYTE *di);
static int simCommand (ULONG cmd, void *h); /* handle win commands      */
extern UBYTE mfw_IMSI [];                  /* expanded IMSI (ASCIIZ)   */

#ifdef SIM_TOOLKIT
char mfwSATLPRefresh;
#endif

//	Sept 24, 2005 REF:  LOCOSTO-ENH-34438, xpradipg
//	required for the new compiler 2.54
#ifdef SIM_PERS
T_ACI_FAC mfw_simlock_return_lock_type(S16 lck);
T_MFW mfw_simlock_check_status(S16 lcktyp,int *max,int *curr);
int mfw_simlock_get_lock_status(S16 lcktyp);
int C_KEY_REQ = 0;

//x0pleela 25 Apr, 2006  DR: OMAPS00067919
//Global variable to store the code result of qAT_PlusCPIN() result
GLOBAL T_ACI_CPIN_RSLT simp_cpin_code; 
GLOBAL UBYTE simp_cpin_flag; //Global to indicate whether Category pwds are asked after qAT_PlusCPIN()
#endif

static T_MFW_SIM_STATUS sim_status;
static T_MFW_SIM_CONFIG sim_config;
static int pin_flag;
static UBYTE pin_ident;

static UBYTE sim_service_table [0x10];      /* SIM service table        *//*a0393213 cphs R4*/
static UBYTE sim_stat = SIM_NOT_ACTIVE;

//	Sept 24, 2005 REF:  LOCOSTO-ENH-34438, xpradipg
//	Fix for the wrong screens in PIN enable/disable/change password
UBYTE pin1_set = MFW_SIM_DISABLE;
UBYTE pin2_set = MFW_SIM_ENABLE;

static BOOL sat_update = FALSE;

static int pSlot;                       /* primitive handler slot   */

CallbackCPRI mmi_cprs_cb;

EXTERN MfwHdr * current_mfw_elem;
#if defined (WIN32)
/*
 * Only for windows to check several SIM lock combinations
 */
UBYTE sim_lock_mode = 0;     
#endif
UBYTE sim_lock_field = NOT_PRESENT_8BIT;  /* type of sim lock field   */

#ifdef SIM_PERS
EXTERN UBYTE gsim_status;
//	Nov 03, 2005 DR: OMAPS00052032 - xpradipg
T_MFW_SIMP_CLCK_FLAG	sim_unlock_in_prog = SIMP_NONE;

//x0pleela 20 July, 2006
static UBYTE Simp_Busy_State;		/* Set to either TRUE or FALSE based on the ACI's busy error */

#endif

//x0pleela 29 May, 2007 DR: OMAPS00132483
#ifdef FF_PHONE_LOCK
#define Phlock_ResetFlag	0x00 //toset any bit to zero
#define PhlockStatus		0x01 //for phone lock status
#define AutoPhlockStatus	0x02 //for automatic phone lock status
static int Phlock_flag;

//Function Prototypes
GLOBAL int mfw_get_Phlock_status(void);
GLOBAL void mfw_set_Phlock_status( int phlock_status);
GLOBAL int mfw_get_Auto_Phlock_status(void);
GLOBAL void mfw_set_Auto_Phlock_status( int phlock_status);

#endif //FF_PHONE_LOCK

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : sim_init           |
+--------------------------------------------------------------------+

  PURPOSE : initialize SIM manager

*/

int sim_init (void)
{
   /*a0393213 compiler warnings removal - removed simlck variable*/


    TRACE_FUNCTION("sim_init()");

    pin_flag  = -1;
	  pin_ident = MFW_SIM_UNKNOWN;

    /* 
     * install prim handler     
     */
    pSlot = aci_create(sima_response_cb,NULL);
    mfwCommand[MfwTypSim] = (MfwCb) simCommand;

    return TRUE;
}


/*
+---------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM             |
| STATE   : code                        ROUTINE : sim_init_sim_insert |
+---------------------------------------------------------------------+

  PURPOSE : initialize SIM manager

*/

void sim_init_sim_insert()
{
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : sim_exit           |
+--------------------------------------------------------------------+

  PURPOSE : finalize SIM manager

*/

void sim_exit (void)
{
    TRACE_FUNCTION("sim_exit()");

    /* 
     * remove prim handler      
     */
    aci_delete(pSlot);
    sim_stat = SIM_NOT_ACTIVE;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : sim_create         |
+--------------------------------------------------------------------+

  PURPOSE : create SIM event handler

*/

T_MFW_HND sim_create (MfwHnd hWin, MfwEvt event, MfwCb cbfunc)
{
    MfwHdr *hdr;
    T_MFW_SIM *sim_para;
	MfwHdr *insert_status =0;

    TRACE_FUNCTION("sim_create()");

    hdr = (MfwHdr *) mfwAlloc(sizeof(MfwHdr));
    sim_para = (T_MFW_SIM *) mfwAlloc(sizeof(T_MFW_SIM));

    if (!hdr || !sim_para)
    {
    	TRACE_ERROR("ERROR: sim_create() Mem Alloc Failed.");
			
	   	if(hdr)
   			mfwFree((U8*)hdr,sizeof(MfwHdr));

   		if(sim_para)
   			mfwFree((U8*)sim_para,sizeof(T_MFW_SIM));
   		
	   	return 0;
    }
    sim_para->emask = event;
    sim_para->handler = cbfunc;

    hdr->data = sim_para;
    hdr->type = MfwTypSim;
    
    insert_status =  mfwInsert((MfwHdr *) hWin,hdr);
    
    if(!insert_status)
  	 {
     	TRACE_ERROR("ERROR: sim_create() Failed to Install Handler. ");
   		mfwFree((U8*)hdr,sizeof(MfwHdr));
   		mfwFree((U8*)sim_para,sizeof(T_MFW_SIM)); 	
		return 0;
  	 }
    return insert_status;

}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : sim_delete         |
+--------------------------------------------------------------------+

  PURPOSE : delete SIM event handler

*/

MfwRes sim_delete (MfwHnd h)
{
    TRACE_FUNCTION("sim_delete()");

    if (!h || !((MfwHdr *) h)->data)
        return MfwResIllHnd;

    if (!mfwRemove((MfwHdr *) h))
        return MfwResIllHnd;

    mfwFree((U8 *) ((MfwHdr *) h)->data,sizeof(T_MFW_SIM));
    mfwFree((U8 *) h,sizeof(MfwHdr));

    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : sim_signal         |
+--------------------------------------------------------------------+

  PURPOSE : Send a signal

*/

void sim_signal (MfwEvt event, void *para)
{
 UBYTE temp = dspl_Enable(0);
    TRACE_FUNCTION ("sim_signal()");

  if (mfwSignallingMethod EQ 0)
  {
    if (mfwFocus)
        if (sim_sign_exec(mfwFocus,event,para))
			{
				dspl_Enable(temp);            
				return;
			}
    if (mfwRoot)
        sim_sign_exec(mfwRoot,event,para);
  }
  else
    {
      MfwHdr * h = 0;

      /*
       * Focus set, then start here
       */
      if (mfwFocus)
        h = mfwFocus;
      /*
       * Focus not set, then start root
       */
      if (!h)
        h = mfwRoot;

      /*
       * No elements available, return
       */

      while (h)


      {
        /*
         * Signal consumed, then return
         */
        if (sim_sign_exec (h, event, para))
			{
				dspl_Enable(temp);            
				return;
			}

        /*
         * All windows tried inclusive root
         */
        if (h == mfwRoot)
			{
				dspl_Enable(temp);            
				return;
			}

        /*
         * get parent window
         */
        h = mfwParent(mfwParent(h));
		if(h)
			h = ((MfwWin * )(h->data))->elems;
      }
      sim_sign_exec (mfwRoot, event, para);
    }      
	dspl_Enable(temp);            
	return;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : sim_sign_exec      |
+--------------------------------------------------------------------+

  PURPOSE : Send a signal if SIM management handler

*/

int sim_sign_exec (MfwHdr *cur_elem, MfwEvt event, T_MFW_SIM_PARA *para)
{

    TRACE_FUNCTION("sim_sign_exec()");

    while (cur_elem)
    {
        if (cur_elem->type == MfwTypSim)
        {
            T_MFW_SIM *sim_data;
            sim_data = (T_MFW_SIM *) cur_elem->data;
            if (sim_data->emask & event)
            {
                sim_data->event = event;
                switch (event)
                {
                    case E_SIM_STATUS:
#ifdef NEPTUNE_BOARD
                        sim_data->para.status.sim_operation_mode = para->status.sim_operation_mode;
                        sim_data->para.status.sim_pin_retries         = para->status.sim_pin_retries;
			   sim_data->para.status.sim_procedure          = para->status.sim_procedure;
			   sim_data->para.status.sim_status                = para->status.sim_status;
			   sim_data->para.status.sim_status_type        = para->status.sim_status_type;
#else
                        memcpy(&sim_data->para.status,
                                para,sizeof(T_MFW_SIM_STATUS));
#endif
                        break;
                   /*MC CONQ5862, 13/06/02*, added new event*/
                    case E_SIM_READ_CALLBACK:
                        memcpy(&sim_data->para.callback,
                                para,sizeof(T_MFW_READ_CALLBACK));
                        break;
                }
                if (sim_data->handler)
                {
                  // PATCH LE 06.06.00
                  // store current mfw elem
                  current_mfw_elem = cur_elem;
                  // END PATCH LE 06.06.00
                  if ((*(sim_data->handler))(sim_data->event,
                                             (void *) &sim_data->para))
                    return TRUE;
                }
            }
        }
        cur_elem = cur_elem->next;
    }

    return FALSE;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : sim_activate       |
+--------------------------------------------------------------------+

  PURPOSE : Start SIM card activation

*/

void sim_activate (void)
{

	T_ACI_RETURN cfun;
	T_ACI_ERR_DESC cfun_err;

    TRACE_FUNCTION("sim_activate()");

    /* 
     * Full functionality is used don´t reset the ME 
     */

// Nov 24, 2005, a0876501, DR: OMAPS00045909
	cfun = sAT_PlusCFUN(CMD_SRC_LCL,CFUN_FUN_Full,CFUN_RST_NotPresent);

    if (cfun EQ AT_FAIL)
	{
		cfun_err = qAT_ErrDesc();

		if((cfun_err & 0x0000FFFF) EQ EXT_ERR_IMEICheck)
		{
			sim_stat = IMEI_NOT_VALID;
		    sim_status.sim_status = MFW_IMEI_NOT_VALID;
		    sim_status.sim_operation_mode = sim_config.oper_mode;
		    sim_status.sim_pin_retries = 0;
		    sim_status.sim_procedure = MFW_SIM_ACTIVATION;
		    sim_status.sim_status_type = MFW_SIM_UNKNOWN;

		    sim_signal(E_SIM_STATUS,&sim_status);
		}
	}
	else if (cfun NEQ AT_EXCT)
    {
        TRACE_ERROR("sAT_PlusCFUN error");
    }
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : sim_spn_req        |
+--------------------------------------------------------------------+

  PURPOSE : Request service provider name

*/

void sim_spn_req (void)
{
    T_EF_SPN spn;

    TRACE_FUNCTION("sim_spn_req()");

    memset(&spn,0,sizeof(T_EF_SPN));

    if (sim_check_service(17,sim_service_table)
        EQ ALLOCATED_AND_ACTIVATED)
        sim_read_sim(SIM_SPN,17,17);
    else
#ifdef FF_2TO1_PS
        nm_spn_cnf(CAUSE_SIM_ACCESS_PROHIBIT, &spn);
#else
        nm_spn_cnf(SIM_CAUSE_ACCESS_PROHIBIT, &spn);
#endif

}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : sim_ok_cfun        |
+--------------------------------------------------------------------+

  PURPOSE : Notify successful end of SIM card activation

*/

void sim_ok_cfun (void)
{
    T_ACI_CFUN_FUN fun;

    TRACE_FUNCTION ("sim_ok_cfun()");

    qAT_PlusCFUN( CMD_SRC_LCL, &fun );

    if( fun EQ CFUN_FUN_Full )
    {
      sim_stat = SIM_ACTIVE;
      sim_status.sim_status = MFW_SIM_NO_PIN;
      sim_status.sim_operation_mode = sim_config.oper_mode;
      sim_status.sim_pin_retries = 0;
      sim_status.sim_procedure = MFW_SIM_ACTIVATION;
      sim_status.sim_status_type = pin_ident;

      pin_ident = MFW_SIM_UNKNOWN;
      sim_signal(E_SIM_STATUS,&sim_status);
      nm_activate(MFW_SIM_NO_PIN);

      pin1_set = MFW_SIM_DISABLE;
    }
    else if( fun EQ CFUN_FUN_Minimum )
    {
	    nm_ok_deregistration();
    }
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : sim_error_cfun     |
+--------------------------------------------------------------------+

  PURPOSE : Notify error status of SIM card

*/

/* OMAPS00075177 - 17,Apr-2006 */
/* Signature of the function has been changed from U8 to T_ACI_CME_ERR. 
   because, when calling this function, it is converting from T_ACI_CME_ERR to U8. 
   It is loosing the negative values( the enum T_ACI_CME_ERR has negative values )
*/
void sim_error_cfun (T_ACI_CME_ERR result)
{
    TRACE_FUNCTION("sim_error_cfun()");

    sim_status.sim_procedure = MFW_SIM_ACTIVATION;
    sim_status.sim_operation_mode = sim_config.oper_mode;

    switch (result)
    {
        case CME_ERR_SimPinReq:         /* PIN 1 must be entered    */
            sim_status.sim_status = MFW_SIM_PIN_REQ;
            sim_status.sim_pin_retries = sim_pin_count(MFW_SIM_PIN1);
            sim_status.sim_status_type = MFW_SIM_PIN1;
            sim_stat = SIM_ACTIVE;
            break;
        case CME_ERR_SimPin2Req:        /* PIN 2 must be entered    */
            sim_status.sim_status = MFW_SIM_PIN2_REQ;
            sim_status.sim_pin_retries = sim_pin_count(MFW_SIM_PIN2);
            sim_status.sim_status_type = MFW_SIM_PIN2;
            sim_stat = SIM_ACTIVE;
            break;
  
#ifdef SIM_PERS
	case CME_ERR_PhoneFail:
      	case CME_ERR_NetworkPersPukReq:
     	case CME_ERR_NetworkSubsetPersPukReq:
     	case CME_ERR_ProviderPersPukReq:
     	case CME_ERR_CorporatePersPukReq:     		
	//x0pleela 25 Sep, 2006 ER: OMAPS00095524
	//Update global aciErrDesc with the current CME error code
	ACI_ERR_DESC( ACI_ERR_CLASS_Cme, result );
        C_KEY_REQ = 1;
	sim_status.sim_status = MFW_SIM_PUK_REQ;
	sim_status.sim_pin_retries = sim_pin_count(MFW_SIM_PUK1);
	sim_status.sim_status_type = MFW_SIM_PUK1;
	sim_stat = SIM_ACTIVE;
	break;
		
#endif     	
//	Nov 03, 2005 DR: OMAPS00050595 - xpradipg
//	the setting of C_KEY_REQ is done only for SIM Personalization PUK request
       case CME_ERR_SimPukReq:         /* PUK 1 must be entered    */	
#ifdef SIM_PERS	   	
	   	C_KEY_REQ = 0;
#endif
            sim_status.sim_status = MFW_SIM_PUK_REQ;
            sim_status.sim_pin_retries = sim_pin_count(MFW_SIM_PUK1);
            sim_status.sim_status_type = MFW_SIM_PUK1;
            sim_stat = SIM_ACTIVE;
            break;
        case CME_ERR_SimPuk2Req:        /* PUK 2 must be entered    */
            sim_status.sim_status = MFW_SIM_PUK2_REQ;
            sim_status.sim_pin_retries = sim_pin_count(MFW_SIM_PUK2);
            sim_status.sim_status_type = MFW_SIM_PUK2;
            sim_stat = SIM_ACTIVE;
            break;
        case CME_ERR_NotPresent:        /* JVJ -#1580. Unexpected error should be treated as INVALID CARD for FTA*/        
        case CME_ERR_SimFail:           /* SIM card is invalid      */
        case CME_ERR_SimWrong:
            sim_status.sim_status = MFW_SIM_INVALID_CARD;
            sim_status.sim_pin_retries = 0;
            sim_status.sim_status_type = MFW_SIM_UNKNOWN;
            sim_stat = SIM_NOT_ACTIVE;
            break;
        case CME_ERR_SimNotIns:         /* no SIM card inserted     */
            sim_status.sim_status = MFW_SIM_NO_SIM_CARD;
            sim_status.sim_pin_retries = 0;
            sim_status.sim_status_type = MFW_SIM_UNKNOWN;
            sim_stat = SIM_NOT_ACTIVE;
            break;
#ifdef SIM_PERS    
			//The events are handled that are posted by rAT_PlusCME()
			//for all the five SIM Locks.
         case CME_ERR_PhSimPinReq:
   	     //x0pleela 25 Sep, 2006 ER: OMAPS00095524
   	     //Update global aciErrDesc with the current CME error code
   	     ACI_ERR_DESC( ACI_ERR_CLASS_Cme, result );
            sim_status.sim_status = MFW_SIM_PLOCK_REQ;
            sim_status.sim_pin_retries = 0;
            sim_status.sim_status_type = MFW_SIM_LOCKED ;
            sim_stat = SIM_ACTIVE;
            break;
         case CME_ERR_NetworkPersPinReq:
		//x0pleela 25 Sep, 2006 ER: OMAPS00095524
		//Update global aciErrDesc with the current CME error code
		ACI_ERR_DESC( ACI_ERR_CLASS_Cme, result );		 	
         	sim_status.sim_status = MFW_SIM_NLOCK_REQ;
         	sim_status.sim_pin_retries = 0;
         	sim_status.sim_status_type = MFW_SIM_LOCKED;
         	sim_stat = SIM_ACTIVE;
         	gsim_status=sim_stat;
         	break;
         case CME_ERR_NetworkSubsetPersPinReq:
		//x0pleela 25 Sep, 2006 ER: OMAPS00095524
		//Update global aciErrDesc with the current CME error code
		ACI_ERR_DESC( ACI_ERR_CLASS_Cme, result );
         	sim_status.sim_status = MFW_SIM_NSLOCK_REQ;
         	sim_status.sim_pin_retries = 0;
         	sim_status.sim_status_type = MFW_SIM_LOCKED;
         	sim_stat = SIM_ACTIVE;
         	break;
         case CME_ERR_ProviderPersPinReq:
		//x0pleela 25 Sep, 2006 ER: OMAPS00095524
		//Update global aciErrDesc with the current CME error code
		ACI_ERR_DESC( ACI_ERR_CLASS_Cme, result );
         	sim_status.sim_status = MFW_SIM_SPLOCK_REQ;
         	sim_status.sim_pin_retries = 0;
         	sim_status.sim_status_type = MFW_SIM_LOCKED;
         	sim_stat = SIM_ACTIVE;
         	break;
         case CME_ERR_CorporatePersPinReq:
		//x0pleela 25 Sep, 2006 ER: OMAPS00095524
		//Update global aciErrDesc with the current CME error code
		ACI_ERR_DESC( ACI_ERR_CLASS_Cme, result );		 	
         	sim_status.sim_status = MFW_SIM_CLOCK_REQ;
         	sim_status.sim_pin_retries = 0;
         	sim_status.sim_status_type = MFW_SIM_LOCKED;
         	sim_stat = SIM_ACTIVE;
         	break;
			
//x0pleela 27 Apr, 2006  DR: OMAPS00067919
	case CME_ERR_WrongPasswd:
		{
			TRACE_EVENT("CME_ERR_WrongPasswd");
			switch( aciErrDesc & 0x0000FFFF )
				{
					case CME_ERR_PhoneFail:
      					case CME_ERR_NetworkPersPukReq:
     					case CME_ERR_NetworkSubsetPersPukReq:
     					case CME_ERR_ProviderPersPukReq:
     					case CME_ERR_CorporatePersPukReq:     		
       					C_KEY_REQ = 1;
				            	sim_status.sim_status = MFW_SIM_PUK_REQ;
            					sim_status.sim_pin_retries = sim_pin_count(MFW_SIM_PUK1);
            					sim_status.sim_status_type = MFW_SIM_LOCKED;
				            	sim_stat = SIM_ACTIVE;
	            				break;

					default:
						C_KEY_REQ = 0;
						break;
				}
		}
	break;
  
//x0pleela 23 Aug, 2006 DR: OMAPS00091250
	case CME_ERR_Unknown:
	{
		if( ((aciErrDesc & 0xFFFF0000) >> 16) EQ ACI_ERR_CLASS_Ext) 
		{
			switch(aciErrDesc & 0x0000FFFF)
			{
				//x0pleela 06 Sep, 2006 DR: OMAPS00091250
				//Check for No MEPD Error
				case EXT_ERR_NoMEPD:
					sim_status.sim_status = MFW_MEPD_INVALID;
					sim_status.sim_pin_retries = 0;
					sim_status.sim_status_type = MFW_SIM_UNKNOWN;
					sim_stat = SIM_ACTIVE;
				break;

				//x0pleela 25 Sep, 2006 ER: OMAPS00095524
				//Set the reqd. parameters for Blocked Network Password request
				case EXT_ERR_BlockedNetworkPersPinReq:
					sim_status.sim_status = MFW_SIM_PBLOCK_REQ;
			         	sim_status.sim_pin_retries = 0;
				      	sim_status.sim_status_type = MFW_SIM_LOCKED;
			         	sim_stat = SIM_ACTIVE;
				break;

				//x0pleela 25 Sep, 2006 ER: OMAPS00095524
				//Set the reqd. parameters for Blocked Network Unblock code request
				case EXT_ERR_BlockedNetworkPersPukReq:
					C_KEY_REQ = 1;
				      	sim_status.sim_status = MFW_SIM_PUK_REQ;
            				sim_status.sim_pin_retries = sim_pin_count(MFW_SIM_PUK1);
            				sim_status.sim_status_type = MFW_SIM_LOCKED;
				      	sim_stat = SIM_ACTIVE;
	            		break;

				default:
					break;
			}
		}
	}
	break;
#endif
        default:
            TRACE_ERROR("sim_error_cfun(): result");
            return;
    }

    sim_signal(E_SIM_STATUS,&sim_status);

    if ((result == CME_ERR_SimPinReq)
        OR (result == CME_ERR_SimPukReq))
        pin1_set = MFW_SIM_ENABLE;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : sim_error_cpin     |
+--------------------------------------------------------------------+

  PURPOSE : Inform about error of PIN entering or PUK entering

*/

void sim_error_cpin (T_ACI_CME_ERR reason)
{
    T_ACI_CPIN_RSLT code=CPIN_RSLT_NotPresent;

    TRACE_FUNCTION("sim_error_cpin()");

    sim_status.sim_procedure = pin_flag;
    sim_status.sim_operation_mode = sim_config.oper_mode;
    sim_status.sim_status = MFW_SIM_FAILURE;
    sim_status.sim_status_type = pin_ident;

	  pin_flag = -1;
	  pin_ident = MFW_SIM_UNKNOWN;

 	//x0pleela 26 Sep, 2006 DR: OMAPS00096565
	//Reset C_KEY_REQ to 0
#ifdef SIM_PERS	  
   	C_KEY_REQ = 0; 
#endif

    switch (reason)
    {
        case CME_ERR_WrongPasswd:
            if (qAT_PlusCPIN(CMD_SRC_LCL,&code) != AT_CMPL)
            {
                TRACE_ERROR("sim_error_cpin():CPIN? failed");
                sim_signal(E_SIM_STATUS,&sim_status);
                return;
            }
            break;
#ifdef SIM_PERS	
 case CME_ERR_SimPinReq:         /* PIN 1 must be entered    */
           sim_status.sim_status = MFW_SIM_PIN_REQ;
            sim_status.sim_pin_retries = sim_pin_count(MFW_SIM_PIN1);
            sim_status.sim_status_type = MFW_SIM_PIN1;
            sim_status.sim_procedure = MFW_SIM_ACTIVATION;
   	     sim_stat = SIM_ACTIVE; 
   	      sim_signal(E_SIM_STATUS,&sim_status);
    		return;
           
        case CME_ERR_SimPin2Req:        /* PIN 2 must be entered    */
            sim_status.sim_status = MFW_SIM_PIN2_REQ;
            sim_status.sim_pin_retries = sim_pin_count(MFW_SIM_PIN2);
            sim_status.sim_status_type = MFW_SIM_PIN2;
            sim_status.sim_procedure = MFW_SIM_ACTIVATION;
            sim_stat = SIM_ACTIVE;
             sim_signal(E_SIM_STATUS,&sim_status);
    		return;
		
	case CME_ERR_PhoneFail:
      	case CME_ERR_NetworkPersPukReq:
     	case CME_ERR_NetworkSubsetPersPukReq:
     	case CME_ERR_ProviderPersPukReq:
     	case CME_ERR_CorporatePersPukReq:     		
		//x0pleela 25 Sep, 2006 ER: OMAPS00095524
		//Update global aciErrDesc with the current CME error code
		ACI_ERR_DESC( ACI_ERR_CLASS_Cme, reason );
		C_KEY_REQ = 1;
		sim_status.sim_status = MFW_SIM_PUK_REQ;
		sim_status.sim_pin_retries = sim_pin_count(MFW_SIM_PUK1);
		sim_status.sim_status_type = MFW_SIM_PUK1;
		sim_status.sim_procedure = MFW_SIM_ACTIVATION;
		sim_stat = SIM_ACTIVE;
		sim_signal(E_SIM_STATUS,&sim_status);
		return;

//	Nov 03, 2005 DR: OMAPS00050595 - xpradipg		
        case CME_ERR_SimPukReq:         /* PUK 1 must be entered    */
	     C_KEY_REQ = 0;
            sim_status.sim_status = MFW_SIM_PUK_REQ;
            sim_status.sim_pin_retries = sim_pin_count(MFW_SIM_PUK1);
            sim_status.sim_status_type = MFW_SIM_PUK1;
            sim_status.sim_procedure = MFW_SIM_ACTIVATION;
            sim_stat = SIM_ACTIVE;
             sim_signal(E_SIM_STATUS,&sim_status);
    		return;
        case CME_ERR_SimPuk2Req:        /* PUK 2 must be entered    */
            sim_status.sim_status = MFW_SIM_PUK2_REQ;
            sim_status.sim_pin_retries = sim_pin_count(MFW_SIM_PUK2);
            sim_status.sim_status_type = MFW_SIM_PUK2;
            sim_status.sim_procedure = MFW_SIM_ACTIVATION;
            sim_stat = SIM_ACTIVE;
             sim_signal(E_SIM_STATUS,&sim_status);
        return;
        case CME_ERR_NotPresent:        /* JVJ -#1580. Unexpected error should be treated as INVALID CARD for FTA*/        
        case CME_ERR_SimFail:           /* SIM card is invalid      */
#endif			
        case CME_ERR_SimWrong:
			sim_status.sim_status = MFW_SIM_INVALID_CARD;
#ifdef SIM_PERS
            sim_status.sim_pin_retries = 0;
            sim_status.sim_status_type = MFW_SIM_UNKNOWN;
            sim_status.sim_procedure = MFW_SIM_ACTIVATION;
            sim_stat = SIM_NOT_ACTIVE;
            sim_signal(E_SIM_STATUS,&sim_status);
#else
             sim_signal(E_SIM_STATUS,&sim_status);
#endif
            return;

            
        case CME_ERR_SimNotIns:         /* no SIM card inserted     */
            sim_status.sim_status = MFW_SIM_NO_SIM_CARD;
            sim_status.sim_pin_retries = 0;
            sim_status.sim_status_type = MFW_SIM_UNKNOWN;
            sim_status.sim_procedure = MFW_SIM_ACTIVATION;
            sim_stat = SIM_NOT_ACTIVE;
             sim_signal(E_SIM_STATUS,&sim_status);
   		 return;

#ifdef SIM_PERS    
			//The events are handled that are posted by rAT_PlusCME()
			//for all the five SIM Locks.
         case CME_ERR_PhSimPinReq:
		//x0pleela 25 Sep, 2006 ER: OMAPS00095524
		//Update global aciErrDesc with the current CME error code
		ACI_ERR_DESC( ACI_ERR_CLASS_Cme, reason );		 	
            sim_status.sim_status = MFW_SIM_PLOCK_REQ;
            sim_status.sim_pin_retries = 0;
            sim_status.sim_status_type = MFW_SIM_LOCKED ;
            sim_status.sim_procedure = MFW_SIM_ACTIVATION;
            sim_stat = SIM_ACTIVE;
             sim_signal(E_SIM_STATUS,&sim_status);
  		  return;
         case CME_ERR_NetworkPersPinReq:
		//x0pleela 25 Sep, 2006 ER: OMAPS00095524
		//Update global aciErrDesc with the current CME error code
		ACI_ERR_DESC( ACI_ERR_CLASS_Cme, reason );		 	
         	sim_status.sim_status = MFW_SIM_NLOCK_REQ;
         	sim_status.sim_pin_retries = 0;
         	sim_status.sim_status_type = MFW_SIM_LOCKED;
         	sim_status.sim_procedure = MFW_SIM_ACTIVATION;
         	//sim_stat = SIM_NOT_ACTIVE; temporary changes
         	sim_stat = SIM_ACTIVE;
         	 sim_signal(E_SIM_STATUS,&sim_status);
    		return;
         case CME_ERR_NetworkSubsetPersPinReq:
		//x0pleela 25 Sep, 2006 ER: OMAPS00095524
		//Update global aciErrDesc with the current CME error code
		ACI_ERR_DESC( ACI_ERR_CLASS_Cme, reason );			 	
         	sim_status.sim_status = MFW_SIM_NSLOCK_REQ;
         	sim_status.sim_pin_retries = 0;
         	sim_status.sim_status_type = MFW_SIM_LOCKED;
         	sim_status.sim_procedure = MFW_SIM_ACTIVATION;
         	sim_stat = SIM_ACTIVE;
 		sim_signal(E_SIM_STATUS,&sim_status);
    		return;
         case CME_ERR_ProviderPersPinReq:
		//x0pleela 25 Sep, 2006 ER: OMAPS00095524
		//Update global aciErrDesc with the current CME error code
		ACI_ERR_DESC( ACI_ERR_CLASS_Cme, reason );			 	
         	sim_status.sim_status = MFW_SIM_SPLOCK_REQ;
         	sim_status.sim_pin_retries = 0;
         	sim_status.sim_status_type = MFW_SIM_LOCKED;
         	sim_status.sim_procedure = MFW_SIM_ACTIVATION;
         	sim_stat = SIM_ACTIVE;
         	 sim_signal(E_SIM_STATUS,&sim_status);
    		return;
         case CME_ERR_CorporatePersPinReq:
		//x0pleela 25 Sep, 2006 ER: OMAPS00095524
		//Update global aciErrDesc with the current CME error code
		ACI_ERR_DESC( ACI_ERR_CLASS_Cme, reason );			 	
         	sim_status.sim_status = MFW_SIM_CLOCK_REQ;
         	sim_status.sim_pin_retries = 0;
         	sim_status.sim_status_type = MFW_SIM_LOCKED;
         	sim_status.sim_procedure = MFW_SIM_ACTIVATION;
         	sim_stat = SIM_ACTIVE;
         	 sim_signal(E_SIM_STATUS,&sim_status);
    		return;

	case CME_ERR_Unknown:
	{
		//Check for No MEPD Error
		if( ((aciErrDesc & 0xFFFF0000) >> 16) EQ ACI_ERR_CLASS_Ext) 
		{
			switch(aciErrDesc & 0x0000FFFF)
			{
				//x0pleela 06 Sep, 2006 DR: OMAPS00091250
				case EXT_ERR_NoMEPD:
					sim_status.sim_status = MFW_MEPD_INVALID;
					sim_status.sim_pin_retries = 0;
					sim_status.sim_status_type = MFW_SIM_UNKNOWN;
					sim_stat = SIM_ACTIVE;
					sim_signal(E_SIM_STATUS,&sim_status);
					return;
					
				//x0pleela 25 Sep, 2006 ER: OMAPS00095524
				//Set the reqd. parameters for Blocked Network Password request
				case EXT_ERR_BlockedNetworkPersPinReq:
					sim_status.sim_status = MFW_SIM_PBLOCK_REQ;
					sim_status.sim_pin_retries = 0;
					sim_status.sim_status_type = MFW_SIM_LOCKED;
					sim_status.sim_procedure = MFW_SIM_ACTIVATION;
					sim_stat = SIM_ACTIVE;
					sim_signal(E_SIM_STATUS,&sim_status);
					return;
				
				//x0pleela 25 Sep, 2006 ER: OMAPS00095524
				//Set the reqd. parameters for Blocked Network Unblock code request
				case EXT_ERR_BlockedNetworkPersPukReq:
					C_KEY_REQ = 1;
					sim_status.sim_status = MFW_SIM_PUK_REQ;
					sim_status.sim_pin_retries = sim_pin_count(MFW_SIM_PUK1);
					sim_status.sim_status_type = MFW_SIM_PUK1;
					sim_status.sim_procedure = MFW_SIM_ACTIVATION;
					sim_stat = SIM_ACTIVE;
					sim_signal(E_SIM_STATUS,&sim_status);
					return;
	            		
				default:
					break;
			}
		}
	}
	break;
#endif	

        default:
            TRACE_ERROR("sim_error_cpin():Ill reason");
            sim_signal(E_SIM_STATUS,&sim_status);
            return;
    }

    switch (code)
    {
        case CPIN_RSLT_SimPinReq:
            sim_status.sim_status = MFW_SIM_PIN_REQ;
            sim_status.sim_pin_retries = sim_pin_count(MFW_SIM_PIN1);
            break;
        case CPIN_RSLT_SimPukReq:
#ifdef SIM_PERS			
	     C_KEY_REQ = 0;
#endif
            sim_status.sim_status = MFW_SIM_PUK_REQ;
            sim_status.sim_pin_retries = sim_pin_count(MFW_SIM_PUK1);
	    //x0pleela 10 May, 2006  DR: OMAPS00067919
	     sim_status.sim_status_type = MFW_SIM_PUK1;
            sim_status.sim_procedure = MFW_SIM_ACTIVATION;
	    sim_stat = SIM_ACTIVE;
            break;
        case CPIN_RSLT_SimPin2Req:
            sim_status.sim_status = MFW_SIM_PIN2_REQ;
            sim_status.sim_pin_retries = sim_pin_count(MFW_SIM_PIN2);
            break;
        case CPIN_RSLT_SimPuk2Req:
            sim_status.sim_status = MFW_SIM_PUK2_REQ;
            sim_status.sim_pin_retries = sim_pin_count(MFW_SIM_PUK2);
            break;
        case CPIN_RSLT_NotPresent:
        case CPIN_RSLT_SimReady:
        default:
            TRACE_ERROR("sim_error_cpin():Ill code");
            break;
    }

    sim_signal(E_SIM_STATUS,&sim_status);
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : sim_ok_cpin        |
+--------------------------------------------------------------------+

  PURPOSE : Successful end of PIN entering or PUK entering

*/

void sim_ok_cpin (void)
{


    switch (pin_flag)
    {
        case MFW_SIM_VERIFY:
            sim_status.sim_procedure = MFW_SIM_VERIFY;
            sim_status.sim_status = MFW_SIM_NO_PIN;
            sim_status.sim_status_type = pin_ident;
            break;
        case MFW_SIM_UNBLOCK:
            sim_status.sim_procedure = MFW_SIM_UNBLOCK;
            sim_status.sim_status = MFW_SIM_SUCCESS;
            sim_status.sim_status_type = pin_ident;
	//TISH due to 11.11, if pin is unblock successfully, it should set pin Enabled.
//#ifdef SIM_PERS
		pin1_set = MFW_SIM_ENABLE; 
//#endif
            break;
            //xrashmic 1 Jul, 2006 OMAPS00075784	
            //To handle the simp lock disable events during bootup
            case MFW_SIM_ACTIVATION:
            sim_status.sim_procedure = MFW_SIM_ACTIVATION;
            sim_status.sim_status = MFW_SIM_NO_PIN;
            sim_status.sim_status_type = pin_ident;
            break;

        default:
            break;
    }

    pin_flag = -1;
	  pin_ident = MFW_SIM_UNKNOWN;
    sim_status.sim_operation_mode = sim_config.oper_mode;
    sim_status.sim_pin_retries = 0;

    sim_signal (E_SIM_STATUS, &sim_status);
    if (nm_reg_flag_req())
      nm_activate(MFW_SIM_NO_PIN);
//	Sept 24, 2005 REF:  LOCOSTO-ENH-34438, xpradipg
//	removed the flag that was getting set

}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : sim_ss_unblock     |
+--------------------------------------------------------------------+

  PURPOSE : SIM unblock via SS string

*/

void sim_ss_unblock(UBYTE result, T_ACI_CME_ERR reason)
{
    TRACE_FUNCTION("sim_ss_unblock()");
        
    pin_flag = MFW_SIM_UNBLOCK;

    if (result == SIM_UBLK_ERROR)
        sim_error_cpin (reason);
    if (result == SIM_UBLK_OK)
        sim_ok_cpin ();
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : sim_verify_pin     |
+--------------------------------------------------------------------+

  PURPOSE : Verify PIN

*/

void sim_verify_pin (U8 pin_id, char *pin)
{
	TRACE_FUNCTION("sim_verify_pin()");

    pin_flag = MFW_SIM_VERIFY;
	  pin_ident = pin_id;
    switch (pin_id)
    {
        case MFW_SIM_PIN1:
            sAT_PercentPVRF(CMD_SRC_LCL, PVRF_TYPE_Pin1, pin, 0);
            break;
        case MFW_SIM_PIN2:
            sAT_PercentPVRF(CMD_SRC_LCL, PVRF_TYPE_Pin2, pin, 0);
            break;
    }
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : sim_unblock_pin    |
+--------------------------------------------------------------------+

  PURPOSE : unblock SIM

*/

void sim_unblock_pin (U8 pin_id, char *puk, char *newpin)
{
	T_ACI_RETURN ret;//xrashmic 6 Jul, 2006 OMAPS00080708
    TRACE_FUNCTION("sim_unblock_pin()");

    pin_flag = MFW_SIM_UNBLOCK;
	  pin_ident = pin_id;
    switch (pin_id)
    {
        case MFW_SIM_PUK1:
            ret= sAT_PercentPVRF(CMD_SRC_LCL, PVRF_TYPE_Puk1, puk, newpin);
		//xrashmic 6 Jul, 2006 OMAPS00080708
		// For error return value, bmi has to be informed
	    if(ret==AT_FAIL)
	    	{
			sim_status.sim_status = MFW_SIM_PUK_REQ;
			sim_status.sim_pin_retries = sim_pin_count(MFW_SIM_PUK1);
			sim_status.sim_status_type = MFW_SIM_PUK1;
			sim_status.sim_procedure = MFW_SIM_ACTIVATION;
			sim_stat = SIM_ACTIVE;
			sim_signal(E_SIM_STATUS,&sim_status);

	    	}
            break;
        case MFW_SIM_PUK2:
           ret= sAT_PercentPVRF(CMD_SRC_LCL, PVRF_TYPE_Puk2, puk, newpin);
		//xrashmic 6 Jul, 2006 OMAPS00080708
		// For error return value, bmi has to be informed
		if(ret==AT_FAIL)
	    {
			sim_status.sim_status = MFW_SIM_PUK2_REQ;
			sim_status.sim_pin_retries = sim_pin_count(MFW_SIM_PUK2);
			sim_status.sim_status_type = MFW_SIM_PUK2;
			sim_status.sim_procedure = MFW_SIM_ACTIVATION;
			sim_stat = SIM_ACTIVE;
			sim_signal(E_SIM_STATUS,&sim_status);
	    }
            break;
    }
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : sim_change_pin     |
+--------------------------------------------------------------------+

  PURPOSE : Request of PIN change

*/

void sim_change_pin (U8 pin_id, U8 *old_pin, U8 *new_pin)
{
    TRACE_FUNCTION("sim_change_pin()");

    pin_ident = pin_id;
    switch (pin_id)
    {
        case MFW_SIM_PIN1:
	/*a0393213 warnings removal - CPWD_FAC_Sc changed to FAC_Sc as a result of ACI enum change*/
            sAT_PlusCPWD(CMD_SRC_LCL, FAC_Sc,              
                         (char *)old_pin, (char *)new_pin);
            break;
        case MFW_SIM_PIN2:
            sAT_PlusCPWD(CMD_SRC_LCL, FAC_P2,
                         (char *)old_pin, (char *)new_pin);
            break;
    }
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : sim_ok_cpinc       |
+--------------------------------------------------------------------+

  PURPOSE : Notify successful end of PIN change

*/

void sim_ok_cpinc (void)
{
    TRACE_FUNCTION("sim_ok_cpinc()");

    sim_status.sim_procedure = MFW_SIM_CHANGE;
    sim_status.sim_status = MFW_SIM_SUCCESS;
    sim_status.sim_operation_mode = sim_config.oper_mode;
    sim_status.sim_pin_retries = 0;
    sim_status.sim_status_type = pin_ident;

    pin_ident = MFW_SIM_UNKNOWN;
    sim_signal(E_SIM_STATUS,&sim_status);
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : sim_error_cpinc    |
+--------------------------------------------------------------------+

  PURPOSE : Notify failed PIN change

*/

void sim_error_cpinc (void)
{
    TRACE_FUNCTION("sim_error_cpinc()");

    sim_status.sim_procedure = MFW_SIM_CHANGE;
    sim_status.sim_status = MFW_SIM_FAILURE;
    sim_status.sim_operation_mode = sim_config.oper_mode;
    if (pin_ident EQ MFW_SIM_PIN1)
      sim_status.sim_pin_retries = sim_pin_count(MFW_SIM_PIN1);
    else if (pin_ident EQ MFW_SIM_PIN2)
      sim_status.sim_pin_retries = sim_pin_count(MFW_SIM_PIN2);
    else
      sim_status.sim_pin_retries = 0;
    sim_status.sim_status_type = pin_ident;

    pin_ident = MFW_SIM_UNKNOWN;
    sim_signal(E_SIM_STATUS,&sim_status);
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : sim_disable_pin    |
+--------------------------------------------------------------------+

  PURPOSE : Request PIN disability.SPR 2145 now returns status

*/

T_MFW sim_disable_pin (U8 *pin)
{
    TRACE_FUNCTION("sim_disable_pin()");
    /*a0393213 warnings removal - CLCK_FAC_Sc changed to FAC_Sc as a result of ACI enum change*/
    return ss_set_clck(FAC_Sc, CLCK_MOD_Unlock,
               (char *)pin, CLASS_NotPresent, MFW_SIM_CPIND); /*a0393213 compiler warnings removal - -1 changed to CLASS_NotPresent*/
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : sim_ok_cpind       |
+--------------------------------------------------------------------+

  PURPOSE : Successful end of PIN disability

*/

void sim_ok_cpind (void)
{
    TRACE_FUNCTION("sim_ok_cpind()");

    sim_status.sim_procedure = MFW_SIM_DISABLE;
    sim_status.sim_status = MFW_SIM_SUCCESS;
    sim_status.sim_operation_mode = sim_config.oper_mode;
    sim_status.sim_pin_retries = 0;
    sim_status.sim_status_type = MFW_SIM_UNKNOWN; // no indication

    pin1_set = MFW_SIM_DISABLE;
    sim_signal(E_SIM_STATUS,&sim_status);
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : sim_error_cpind    |
+--------------------------------------------------------------------+

  PURPOSE : Failed PIN disability

*/

void sim_error_cpind (void)
{
    TRACE_FUNCTION("sim_error_cpind()");

    sim_status.sim_procedure = MFW_SIM_DISABLE;
    sim_status.sim_status = MFW_SIM_FAILURE;
    sim_status.sim_operation_mode = sim_config.oper_mode;
    sim_status.sim_pin_retries = sim_pin_count(MFW_SIM_PIN1);
    sim_status.sim_status_type = MFW_SIM_UNKNOWN; // no indication

    sim_signal(E_SIM_STATUS,&sim_status);
}
/* 
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : sim_disable       |
+--------------------------------------------------------------------+

 PURPOSE : To disable the protocol stack
xashmic 9 Sep 2006, OMAPS00092732
*/
void sim_disable (void)
{

	T_ACI_RETURN cfun;

    TRACE_FUNCTION("sim_disable()");
    /* 
     * Full functionality is used don´t reset the ME 
     */
	cfun = sAT_PlusCFUN(CMD_SRC_LCL,CFUN_FUN_Minimum,CFUN_RST_NotPresent);

	 if (cfun NEQ AT_EXCT)
	{
	    TRACE_ERROR("sAT_PlusCFUN error");
	}
}
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : sim_enable       |
+--------------------------------------------------------------------+

  PURPOSE : To enable the protocol stack
xashmic 9 Sep 2006, OMAPS00092732
*/
void sim_enable (void)
{

	T_ACI_RETURN cfun;

    TRACE_FUNCTION("sim_enable()");
	cfun = sAT_PlusCFUN(CMD_SRC_LCL,CFUN_FUN_Full,CFUN_RST_NotPresent);

	 if (cfun NEQ AT_EXCT)
	{
	    TRACE_ERROR("sAT_PlusCFUN error");
	}
}
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : sim_enable_pin     |
+--------------------------------------------------------------------+

  PURPOSE : Request PIN enability.  SPR 2145 now returns status

*/

T_MFW sim_enable_pin (U8 *pin)
{
    TRACE_FUNCTION("sim_enable_pin()");
    /*a0393213 warnings removal - CLCK_FAC_Sc, changed to FAC_Sc as a result of ACI enum change*/
    return ss_set_clck(FAC_Sc, CLCK_MOD_Lock,
               (char *)pin, CLASS_NotPresent, MFW_SIM_CPINE);/*a0393213 compiler warnings removal - -1 changed to CLASS_NotPresent*/
}

#ifdef SIM_PERS
//	Nov 03, 2005 DR: OMAPS00052032 - xpradipg
/*******************************************************************************
 $Function:		mfw_simlock_continue
 $Description:		for displaying the sAT_PlusCLCK result
 $Returns:		None
 $Arguments:	state if passed / failed
 
*******************************************************************************/
void	mfw_simlock_continue(U8 state)
{
	mmi_display_result(state);
}

/*******************************************************************************
 $Function:		mfw_simlock_unblockME
 $Description:	for unblocking ME
 $Returns:	SUCCESS or FAILURE 
 $Arguments:	PUK
 
*******************************************************************************/
int mfw_simlock_unblockME(char *puk)
{ 
TRACE_FUNCTION("mfw_simlock_unblockME");
/*a0393213 warnings removal - CLCK_FAC_Sc, changed to FAC_Sc as a result of ACI enum change*/
	if( sAT_PlusCLCK(CMD_SRC_LCL,FAC_Fc,CLCK_MOD_Unlock,(char*)puk,CLASS_NotPresent)==AT_CMPL)/*a0393213 compiler warnings removal - -1 changed to CLASS_NotPresent*/
		//return UNBLOCK_SUCESS;
		return MFW_SS_OK;
		else 
		//return UNBLOCK_FAILURE;
		return MFW_SS_FAIL;
}

/*******************************************************************************
 $Function:		mfw_simlock_check_lock_bootup
 $Description:	checking PIN during bootup
 $Returns:	SUCCESS or FAILURE 
 $Arguments:	PIN
 
*******************************************************************************/
int mfw_simlock_check_lock_bootup(char *pin, S16 lcktype)
{ 
	
/*	if( sAT_PlusCPIN(CMD_SRC_LCL, pin, NULL)==AT_CMPL)
		return BOOTUP_LOCK_SUCCESS;
	else 
		return BOOTUP_LOCK_FAILURE;*/ //Leela for CPIN to CLCK change

	T_ACI_FAC lckFac;/*a0393213 warnings removal - T_ACI_CLCK_FAC changed to T_ACI_FAC as a result of ACI enum changed*/
	/*a0393213 compiler warnings removal- removed variables clsStat,slock_status,status*/
	
	int maxval, curval;
	T_ACI_RETURN ret; //x0pleela 21 Apr, 2006  DR: OMAPS00067919
	T_ACI_CPIN_RSLT code; //x0pleela 21 Apr, 2006  DR: OMAPS00067919
	
	TRACE_FUNCTION("mfw_simlock_check_lock_bootup");
	
       lckFac= mfw_simlock_return_lock_type(lcktype);

	//x0pleela 27 Apr, 2006  DR: OMAPS00067919
	sim_unlock_in_prog = SIMP_BOOTUP;
       simp_cpin_flag = FALSE;
       //xrashmic 1 Jul, 2006 OMAPS00075784
       //To handle the simp lock disable events during bootup
       pin_flag=MFW_SIM_ACTIVATION;
	if(sAT_PlusCLCK(CMD_SRC_LCL,lckFac,CLCK_MOD_Unlock,(char*)pin,CLASS_NotPresent)== AT_FAIL)/*a0393213 compiler warnings removal - -1 changed to CLASS_NotPresent*/
	{
		//x0pleela 11 Sep, 2006  DR: OMASP00094215
		//checking for Class type and the ext busy error and update Simp_Busy_State accordingly	
		//x0pleela 06 Sep, 2006  SIMP: fix for ACI
		if( ( ((aciErrDesc & 0xFFFF0000) >> 16) EQ ACI_ERR_CLASS_Ext) AND
			(aciErrDesc & 0x0000FFFF) EQ EXT_ERR_Busy)
		  mfw_simlock_set_busy_state(TRUE);
		else
		  mfw_simlock_set_busy_state(FALSE);
		
		 mfw_simlock_check_status(lcktype,&maxval,&curval);
		//set the error descriptor
		switch( lcktype)
		{
			case MFW_SIM_NLOCK:
				if(curval !=0)
				{
					ACI_ERR_DESC( ACI_ERR_CLASS_Cme, CME_ERR_NetworkPersPinReq );
				}
				else 
				{
					 ACI_ERR_DESC( ACI_ERR_CLASS_Cme,  CME_ERR_NetworkPersPukReq); 
				}
				break;
					
        		case MFW_SIM_NSLOCK:
				if(curval !=0)
				{
					ACI_ERR_DESC( ACI_ERR_CLASS_Cme, CME_ERR_NetworkSubsetPersPinReq );
				}
				else 
				{
					ACI_ERR_DESC( ACI_ERR_CLASS_Cme,  CME_ERR_NetworkSubsetPersPukReq); 
				}
				break;
				 
   		 	case MFW_SIM_SPLOCK:
			
				if(curval !=0)   	
					{
					ACI_ERR_DESC( ACI_ERR_CLASS_Cme,  CME_ERR_ProviderPersPinReq); 
   		 			}
				else 
					{
					ACI_ERR_DESC( ACI_ERR_CLASS_Cme, CME_ERR_ProviderPersPukReq ); 					
					}
   		 	break;
				 
        		case MFW_SIM_CLOCK:
			
				if(curval !=0)		
					{
					ACI_ERR_DESC( ACI_ERR_CLASS_Cme,  CME_ERR_CorporatePersPinReq); 				
					}
				else 
					{
		 			ACI_ERR_DESC( ACI_ERR_CLASS_Cme, CME_ERR_CorporatePersPukReq ); 
					}
        		break;
						
        		case MFW_SIM_PLOCK:
			
				if(curval !=0)        
        				{
        				ACI_ERR_DESC( ACI_ERR_CLASS_Cme, CME_ERR_PhSimPinReq ); 					
					}
        			else
        				{
        				ACI_ERR_DESC( ACI_ERR_CLASS_Cme,  CME_ERR_PhoneFail); 
        				}
			      		break;

			//x0pleela 25 Sep, 2006 ER: OMAPS00095524
			case MFW_SIM_PBLOCK:
				if(curval !=0)
				{
					ACI_ERR_DESC( ACI_ERR_CLASS_Ext, EXT_ERR_BlockedNetworkPersPinReq );
				}
				else 
				{
					 ACI_ERR_DESC( ACI_ERR_CLASS_Ext,  EXT_ERR_BlockedNetworkPersPukReq); 
				}
        		break;

		   	// default:                    
		   	 //	ACI_ERR_DESC( ACI_ERR_CLASS_Cme, CME_ERR_WrongPasswd );          
		   	 //	break;

		}
	        return BOOTUP_LOCK_FAILURE;
	}
	//x0pleela 21 Apr, 2006  DR: OMAPS00067919
	//Calling qat_plusCPIN for next category check by ACI
	else
	{	
 	  ret = qAT_PlusCPIN( CMD_SRC_LCL, &code );

	  if( code EQ CPIN_RSLT_SimReady )
	  {
		TRACE_EVENT("BOOTUP_LOCK_SUCCESS");
		simp_cpin_flag = FALSE;
	  	return BOOTUP_LOCK_SUCCESS;
	  }
	  simp_cpin_flag = TRUE;
	  simp_cpin_code = code;
	  switch( ret )
	  {
	    case AT_FAIL:
	      TRACE_EVENT("qAT_PlusCPIN error");
	      return BOOTUP_LOCK_FAILURE;
		
	    case AT_CMPL:
//           case AT_EXCT:
		TRACE_EVENT("BOOTUP_OK");		
	      return BOOTUP_OK;
			
	    default:
			break;
	  }	   
	}
       return BOOTUP_LOCK_FAILURE; /*a0393213 lint warnings removal - return statement added*/
}

/*******************************************************************************
 $Function:		mfw_simlock_get_sup_info
 $Description:	for getting the fc value
 $Returns:	value of the req fc type
 $Arguments:	fc  type
 
*******************************************************************************/
int mfw_simlock_get_sup_info(int flag)
{
     T_SUP_INFO sup_info;
    // sup_info=(T_SUP_INFO *)malloc(sizeof(T_SUP_INFO));
     sup_info.infoType=(T_SUP_INFO_TYPE)flag; /*a0393213 compiler warnings removal - explicit typecast*/
     if(qAT_PercentMEPD( CMD_SRC_LCL, &sup_info)  EQ  AT_CMPL)
     	{
     		//TRACE_EVENT_P1("Leela: FC: %d", sup_info.datavalue);
	     	 return sup_info.datavalue;
     	}
     else
     	return MFW_FAILURE;
 }
/*******************************************************************************
 $Function:		mfw_simlock_return_lock_type
 $Description:	The lock type is converted to the ACI enum type
 $Returns:		corresponding ACi lock type
 $Arguments:	MMI lock type - (SIM Locks Only)
 
*******************************************************************************/
/*a0393213 warnings removal - return type changed from T_ACI_CLCK_FAC as a result of ACI enum change*/
T_ACI_FAC mfw_simlock_return_lock_type(S16 lck)
{
    switch (lck)
    {
        //case 0:
        case MFW_SIM_NLOCK:
                    return FAC_Pn;
        //case 1: 
        case MFW_SIM_SPLOCK:
                    return FAC_Pp;
        //case 2: 
        case MFW_SIM_NSLOCK:
                    return FAC_Pu;
        //case 3: 
        case MFW_SIM_CLOCK:
                    return FAC_Pc;
        //case 4: 
        case MFW_SIM_PLOCK:
                    return FAC_Ps;

        case MFW_SIM_FC:
       		return FAC_Fc;
	
        case MFW_SIM_FCM:
       		return FAC_Fcm;

	//x0pleela 20 July, 2006
	//Return coresponding lock type for master unlock 
        case MFW_SIM_MKEY:
       		return FAC_Mu;
       case MFW_SIM_MKEYM:
       		return FAC_Mum;

	//x0pleela 25 Sep, 2006 ER: OMAPS00095524
	//Return coresponding ACI lock type for Blocked network 
	case MFW_SIM_PBLOCK:
		return FAC_Bl;
		
        default:
                return FAC_NotPresent; /*a0393213 compiler warnings removal - -1 changed to CLCK_FAC_NotPresent*/
    }
        
}


/*******************************************************************************
 $Function:		mfw_simlock_check_status
 $Description:	stub function to retireve the maximum and current number of tries
 				for a given lock type
 $Returns:		success or failure
 $Arguments:	input  - type of lock,
 				output - *max and *curr to return the values
 *******************************************************************************/
T_MFW mfw_simlock_check_status(S16 lcktyp,int *max,int *curr)
{
	T_SUP_INFO_TYPE FLAG;
     	int curr_fc, max_fc,FC_Reset_Fail_Max,FC_Reset_Fail_Current,FC_Reset_Success_Max,FC_Reset_Success_Current ;
	//T_DISPLAY_DATA display_info;
	
//     T_MFW_HND  win  = mfwParent(mfw_header());
//	Sept 24, 2005 REF:  LOCOSTO-ENH-34438, xpradipg
//	used new enum value
     FLAG=FCMAX;          /* set flag to current */
     max_fc=mfw_simlock_get_sup_info(FLAG); /* get the value of the corresponding flag*/

      FLAG=FCATTEMPTSLEFT;          /* set flag to current */
     curr_fc=mfw_simlock_get_sup_info(FLAG); /* get the value of the corresponding flag*/

//	Sept 24, 2005 REF:  LOCOSTO-ENH-34438, xpradipg
//	used new enum value
	FLAG = FCRESETFAILMAX;
	FC_Reset_Fail_Max=mfw_simlock_get_sup_info(FLAG);
//	Sept 24, 2005 REF:  LOCOSTO-ENH-34438, xpradipg
//	used new enum value	
	FLAG =FCRESETFAILATTEMPTSLEFT;
	FC_Reset_Fail_Current=mfw_simlock_get_sup_info(FLAG);
	//	Sept 24, 2005 REF:  LOCOSTO-ENH-34438, xpradipg
//	used new enum value
	FLAG =FCRESETSUCCESSMAX;
	FC_Reset_Success_Max=mfw_simlock_get_sup_info(FLAG);
//	Sept 24, 2005 REF:  LOCOSTO-ENH-34438, xpradipg
//	used new enum value	
	FLAG =FCRESETSUCCESSATTEMPTSLEFT;
	FC_Reset_Success_Current=mfw_simlock_get_sup_info(FLAG);

if( lcktyp !=0)
{
	*max = max_fc;
	*curr = curr_fc;
}
else
{
	*max = FC_Reset_Fail_Current;
	*curr = FC_Reset_Success_Current;
}

	TRACE_FUNCTION_P4(" FC_Reset_Fail_Max %d FC_Reset_Fail_Current %d FC_Reset_Success_Max %d  FC_Reset_Success_Current %d",FC_Reset_Fail_Max,FC_Reset_Fail_Current,FC_Reset_Success_Max,FC_Reset_Success_Current); 

	/* if( lcktyp == 0 ) */
	if (curr_fc == 0)
	{
		if(( FC_Reset_Fail_Current !=0 )&& (FC_Reset_Success_Current !=0) )
		{
			//temporarily blocked
			TRACE_FUNCTION("chk_status:MFW_SIM_BLOCKED");
			return MFW_SIM_BLOCKED;
			
		}
		else
		{
			//permanently blocked
			TRACE_FUNCTION("check_staus: MFW_SIM_PERM_BLOCKED");
			return MFW_SIM_PERM_BLOCKED;
		}	
			
	}
	else
	{
		if( curr_fc == 0 )
			return MFW_SIM_BLOCKED;
		else
			return MFW_SIM_NOT_BLOCKED;
	}
}

/*******************************************************************************
 $Function:		mfw_simlock_get_lock_status
 $Description:	returns the status of the lock - enabled / disabled
 $Returns:		lock status
 $Arguments:	lock type
 
*******************************************************************************/
int mfw_simlock_get_lock_status(S16 lcktyp)
{
    /*a0393213 compiler warnings removal - removed variable stat*/
    T_ACI_CLSSTAT clsStat;
    T_ACI_FAC lckfac;/*a0393213 warnings removal - T_ACI_CLCK_FAC changed to T_ACI_FAC as a result of ACI enum change*/
    int status;
    int maxval;
    int curval;

	status = mfw_simlock_check_status(lcktyp,&maxval,&curval);
	if((status == MFW_SIM_BLOCKED))
		return MFW_SIM_BLOCKED;
	if(status == MFW_SIM_PERM_BLOCKED)
		return MFW_SIM_PERM_BLOCKED;
	
     lckfac= mfw_simlock_return_lock_type(lcktyp);
    if (qAT_PlusCLCK(CMD_SRC_LCL, lckfac,
                     CLASS_NotPresent, &clsStat) EQ AT_CMPL)
    {
    TRACE_FUNCTION("Leela:qAT_PlusCLCK->AT_CMPL ");
        switch (clsStat.status)
        {
            case STATUS_NotActive:
                return MFW_SIM_DISABLE;

            case STATUS_Active:
                return MFW_SIM_ENABLE;

        }
    }
    return MFW_SIM_FAILURE;
    
}




/*******************************************************************************
 $Function:		mfw_simlock_enable_lock
 $Description:	Changes the oldpassword with the new one for the given 
 				lock type
 $Returns:		success or failure
 $Arguments:	type of lock and old and new password
 
*******************************************************************************/

T_MFW_SS_RETURN mfw_simlock_enable_lock(U8 *pin, S16 lcktyp)
{

    T_ACI_FAC lckFac;/*a0393213 warnings removal - T_ACI_CLCK_FAC changed to T_ACI_FAC as a result of ACI enum change*/
//	Nov 03, 2005 DR: OMAPS00052032 - xpradipg	
     T_ACI_RETURN ret;
    TRACE_FUNCTION("mfw_simlock_enable_lock()");

  /*  return ss_set_clck(FAC_Ps, CLCK_MOD_Lock,
               (char *)pin, -1, MFW_SIM_CPINE);*/
    lckFac= mfw_simlock_return_lock_type(lcktyp);
  ret = sAT_PlusCLCK(CMD_SRC_LCL,lckFac,CLCK_MOD_Lock,(char*)pin,CLASS_NotPresent); /*a0393213 compiler warnings removal - -1 changed to CLASS_NotPresent*/
//	Nov 03, 2005 DR: OMAPS00052032 - xpradipg
//	handle the AT_EXCT case and set the global to true 
  switch(ret)
  {
  	case AT_CMPL:
		return MFW_SS_OK;
	case AT_EXCT:
		sim_unlock_in_prog = SIMP_MENU;
		return MFW_EXCT;
	case AT_FAIL:
		return MFW_SS_FAIL;
  }
  return MFW_SS_FAIL; /*a0393213 compiler warnings removal - added return statement*/
}

/*******************************************************************************
 $Function:		mfw_simlock_disable_lock
 $Description:	disables the lock with the given password for the given lock
 $Returns:		success or failure
 $Arguments:	type of lock and password
 
*******************************************************************************/
T_MFW_SS_RETURN mfw_simlock_disable_lock(U8 *pin, S16 lcktyp)
{
    T_ACI_FAC lckFac;/*a0393213 warnings removal - T_ACI_CLCK_FAC changed to T_ACI_FAC as a result of ACI enum change*/
     T_ACI_RETURN ret;

    TRACE_FUNCTION("mfw_simlock_disable_lock()");
  /*  return ss_set_clck(FAC_Ps, CLCK_MOD_Lock,
               (char *)pin, -1, MFW_SIM_CPINE);*/
            lckFac= mfw_simlock_return_lock_type(lcktyp);
  ret =  sAT_PlusCLCK(CMD_SRC_LCL,lckFac,CLCK_MOD_Unlock,(char*)pin,CLASS_NotPresent) ; /*a0393213 compiler warnings removal - -1 changed to CLASS_NotPresent*/
  if(ret != AT_CMPL)
        return MFW_SS_FAIL;
    else
        return MFW_SS_OK;
}

T_MFW mfw_simlock_reset_fc_value(U8 *pin, int fc_flag)
{

    T_ACI_FAC lckFac;/*a0393213 warnings removal - T_ACI_CLCK_FAC changed to T_ACI_FAC as a result of ACI enum change*/
    T_ACI_RETURN stat;
    TRACE_FUNCTION("mfw_simlock_reset_fc_value()");
    TRACE_EVENT_P1("SimLock_MFW FC Password=%s",pin);	
    lckFac= mfw_simlock_return_lock_type(fc_flag);
    stat = sAT_PlusCLCK(CMD_SRC_LCL,lckFac,CLCK_MOD_Unlock,(char*)pin,CLASS_NotPresent) ;/*a0393213 compiler warnings removal - -1 changed to CLASS_NotPresent*/
   if( stat != AT_CMPL)
   	{
   	TRACE_FUNCTION_P1("Reset Failed %d",stat);
        return MFW_SS_FAIL;
   	}
    else
   	{
   	TRACE_FUNCTION("Reset Success");
        return MFW_SS_OK;
    	}
}

/*******************************************************************************
 $Function:		mfw_simlock_master_unlock
 $Description:		This function unlocks(dependent categories) or unblocks ME
 $Returns:		success or failure
 $Arguments:	 
*******************************************************************************/
//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
T_MFW mfw_simlock_master_unlock(U8 *pin, S16 MK_Unblock_flag)
{
  T_ACI_FAC lckFac;/*a0393213 warnings removal - T_ACI_CLCK_FAC changed to T_ACI_FAC as a result of ACI enum change*/
  T_ACI_RETURN stat;

  TRACE_FUNCTION("mfw_simlock_master_unlock()");

  lckFac= mfw_simlock_return_lock_type(MK_Unblock_flag);
  stat =sAT_PlusCLCK(CMD_SRC_LCL,lckFac,CLCK_MOD_Unlock,(char*)pin,CLASS_NotPresent);/*a0393213 warnings removal - -1 changed to CLASS_NotPresent*/
  if( stat != AT_CMPL)
  {
    TRACE_EVENT_P1("Master Unlock Failed %d",stat);
	//x0pleela 13 Nov, 2006 DR: OMAPS00103356
	//Check if the master unlock password is wrong
	//If wrong, displays Wrong password
	if((aciErrDesc & 0x0000FFFF) EQ CME_ERR_WrongPasswd)
	  return MFW_WRONG_PWD;

	//x0pleela 13 Nov, 2006 DR: OMAPS00103356
	//Check the error code for any of the following categories (non-dependent) being locked/blocked
	//If yes, return OK as the dependent categories are unblocked using Master unblock password
	else if (
		( (aciErrDesc & 0x0000FFFF) EQ CME_ERR_NetworkPersPinReq ) OR
		( (aciErrDesc & 0x0000FFFF) EQ CME_ERR_NetworkPersPukReq ) OR
		( (aciErrDesc & 0x0000FFFF) EQ CME_ERR_NetworkSubsetPersPinReq ) OR
		( (aciErrDesc & 0x0000FFFF) EQ CME_ERR_NetworkSubsetPersPukReq ) OR
		( (aciErrDesc & 0x0000FFFF) EQ CME_ERR_ProviderPersPinReq ) OR
		( (aciErrDesc & 0x0000FFFF) EQ CME_ERR_ProviderPersPukReq ) OR
		( (aciErrDesc & 0x0000FFFF) EQ CME_ERR_CorporatePersPinReq ) OR
		( (aciErrDesc & 0x0000FFFF) EQ CME_ERR_CorporatePersPukReq ) OR
		( (aciErrDesc & 0x0000FFFF) EQ CME_ERR_PhSimPinReq ) OR
		( (aciErrDesc & 0x0000FFFF) EQ CME_ERR_PhoneFail ) OR
		( ( ((aciErrDesc & 0xFFFF0000) >> 16) EQ ACI_ERR_CLASS_Ext) AND
			(aciErrDesc & 0x0000FFFF) EQ EXT_ERR_BlockedNetworkPersPinReq)  OR
		( ( ((aciErrDesc & 0xFFFF0000) >> 16) EQ ACI_ERR_CLASS_Ext) AND
			(aciErrDesc & 0x0000FFFF) EQ EXT_ERR_BlockedNetworkPersPukReq)
		)
		return MFW_MASTER_OK;
	else
    return MFW_SS_FAIL;
  }
  else
  {
    TRACE_EVENT("Master Unlock Success");
    return MFW_SS_OK;
  }
}

/*******************************************************************************
 $Function:		mfw_simlock_check_masterkey
 $Description:		This function checks from ACI whether Master Unlock feature is enabled or not
 				If enabled, displays a menu item for the user to select master unlocking option
 				If not enabled, doesn't display master unlocking option in the menu
 $Arguments: 
*******************************************************************************/
//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587

UBYTE mfw_simlock_check_masterkey( S16 MK_unlock_type)
{
  T_ACI_CLSSTAT clsStat;
  T_ACI_FAC lckfac;/*a0393213 warnings removal - T_ACI_CLCK_FAC changed to T_ACI_FAC as a result of ACI enum change*/

  TRACE_FUNCTION("mfw_simlock_check_masterkey()");
  
  //get the lock type
  lckfac= mfw_simlock_return_lock_type(MK_unlock_type);

  //query ACI for master unlocking option
  qAT_PlusCLCK(CMD_SRC_LCL, lckfac, CLASS_NotPresent, &clsStat);

  switch( clsStat.status)
  {
    case STATUS_Active:
      return MFW_MASTER_UNLOCK_ACTIVE;

    case STATUS_NotActive:
    default:
      return MFW_MASTER_UNLOCK_DEACTIVE;
  }
}

/*******************************************************************************
 $Function:		mfw_simlock_set_busy_state
 $Description:		This function sets Simp_Busy_State to either TRUE or FALSE based on ACI's 
 				CME error
 $Arguments: 
*******************************************************************************/
//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
void mfw_simlock_set_busy_state( UBYTE state )
{
  TRACE_FUNCTION("mfw_simlock_set_busy_state()");
  Simp_Busy_State = state;
}

/*******************************************************************************
 $Function:		mfw_simlock_get_busy_state
 $Description:		This function returns the current value of Simp_Busy_State	
 $Arguments: 
*******************************************************************************/
//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
UBYTE mfw_simlock_get_busy_state( void )
{
  TRACE_FUNCTION("mfw_simlock_get_busy_state()");
  return Simp_Busy_State;
}


//	Sept 24, 2005 REF:  LOCOSTO-ENH-34438, xpradipg
//	this function is never referenced and this inturn calls sAT_PercentSLPVRF
//	which has been removed at ACI leve
#if 0
/*******************************************************************************
 $Function:		mfw_simlock_verify_lock
 $Description:	If locked, it performs unlock + lock, if unlocks, it performs 
 				lock + unlock, thus validating if the password provided is correct
 $Returns:		success or failure
 $Arguments:	type of lock and password
 
*******************************************************************************/

int mfw_simlock_verify_lock(char* pwd,S16 lock)
{
    int status;
    T_ACI_FAC lckfac;

	TRACE_FUNCTION("mfw_simlock_verify_lock()");
     	TRACE_EVENT_P1("Vidya: SimLock_MFW Password=%s",pwd);	

//	status = mfw_simlock_get_lock_status(lock);
	lckfac=mfw_simlock_return_lock_type(lock);
	if(sAT_PercentSLPVRF(CMD_SRC_LCL, lckfac, pwd)==AT_CMPL)
		return MFW_SS_OK;
	else
		return MFW_SS_FAIL;
/*
     	if(status==MFW_SIM_ENABLE)

     {
           if( sAT_PlusCLCK(CMD_SRC_LCL,lckfac,CLCK_MOD_Unlock,pwd,CLASS_NotPresent) !=AT_CMPL)
           	{
           	TRACE_EVENT("Vidya: SIM_ENABLE unlock fail");
             return MFW_SS_FAIL;
           	}
           else
            {
                if(sAT_PlusCLCK(CMD_SRC_LCL,lckfac,CLCK_MOD_Lock,pwd,CLASS_NotPresent)!=AT_CMPL)
                {
                    	TRACE_EVENT("Vidya: SIM_ENABLE lock fail");
                    	return MFW_SS_FAIL;
                }
                else
	             return MFW_SS_OK;
     	}
                
        }
     	else if(status==MFW_SIM_DISABLE)
     		{
           if( sAT_PlusCLCK(CMD_SRC_LCL,lckfac,CLCK_MOD_Lock,pwd,CLASS_NotPresent) !=AT_CMPL)
           	{
           	TRACE_EVENT("Vidya: SIM_DISABLE lock fail");
             return MFW_SS_FAIL;
           	}
           else
            {
                if(sAT_PlusCLCK(CMD_SRC_LCL,lckfac,CLCK_MOD_Unlock,pwd,CLASS_NotPresent)!=AT_CMPL)
                {
                    	TRACE_EVENT("Vidya: SIM_DISABLE unlock fail");
                    	return MFW_SS_FAIL;
                }
                else
                  return MFW_SS_OK;
            }
           
                
        }
     */
 /*   if(aci_ext_personalisation_MMI_verify_password(pwd) != MMILOCK_VERIFIED)
        MFW_SS_FAIL;
    else
        MFW_SS_OK;*/

}
#endif
/*******************************************************************************
 $Function:		mfw_simlock_change_lock_code
 $Description:	Changes the oldpassword with the new one for the given 
 				lock type
 $Returns:		success or failure
 $Arguments:	type of lock and old and new password
 
*******************************************************************************/
T_MFW mfw_simlock_change_lock_code(int type,char*oldpswd,char* newpswd)
{
	T_ACI_FAC lckType;/*a0393213 warnings removal - T_ACI_CPWD_FAC changed to T_ACI_FAC as a result of ACI enum change*/

	switch(type)
	{
		case MFW_SIM_NLOCK:
			lckType = FAC_Pn;
			break;
		case MFW_SIM_SPLOCK:
			lckType = FAC_Pp;
			break;
		case MFW_SIM_NSLOCK:
			lckType = FAC_Pu;
			break;
		case MFW_SIM_CLOCK:
			lckType = FAC_Pc;
			break;
		case MFW_SIM_PLOCK:
			lckType = FAC_Ps;
			break;
		default:
			lckType=FAC_NotPresent;
			break;
	}

    if(sAT_PlusCPWD(CMD_SRC_LCL, lckType,
                         (char *)oldpswd,(char *)newpswd)!=AT_CMPL)
                         return MFW_SS_FAIL;
    else
       return MFW_SS_OK;
}

#endif

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : sim_ok_cpine       |
+--------------------------------------------------------------------+
  PURPOSE : Successful end of PIN enability
*/
void sim_ok_cpine()
{
    TRACE_FUNCTION("sim_ok_cpine()");

    sim_status.sim_procedure = MFW_SIM_ENABLE;
    sim_status.sim_status = MFW_SIM_SUCCESS;
    sim_status.sim_operation_mode = sim_config.oper_mode;
    sim_status.sim_pin_retries = 0;
    sim_status.sim_status_type = MFW_SIM_UNKNOWN; // no indication
    pin1_set = MFW_SIM_ENABLE;
    sim_signal(E_SIM_STATUS,&sim_status);
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : sim_error_cpine    |
+--------------------------------------------------------------------+

  PURPOSE : Failed PIN enability

*/

void sim_error_cpine (void)
{
    TRACE_FUNCTION("sim_error_cpine()");

    sim_status.sim_procedure = MFW_SIM_ENABLE;
    sim_status.sim_status = MFW_SIM_FAILURE;
    sim_status.sim_operation_mode = sim_config.oper_mode;
    sim_status.sim_pin_retries = sim_pin_count(MFW_SIM_PIN1);
    sim_status.sim_status_type = MFW_SIM_UNKNOWN; // no indication

    sim_signal(E_SIM_STATUS,&sim_status);
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SIM           |
| STATE   : code                         ROUTINE : sim_check_service |
+--------------------------------------------------------------------+

  PURPOSE : Checks a service status

*/

U8 sim_check_service (U8 nr, U8 *serv_table)
{
    U8 value;

    TRACE_FUNCTION("sim_check_service()");

    value = *(serv_table + (nr - 1) / 4);
    value >>=  (((nr - 1) & 3) * 2);

    return (value & 3);
}


/*
+-----------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SIM              |
| STATE   : code                         ROUTINE : sim_serv_table_check |
+-----------------------------------------------------------------------+

  PURPOSE : Checks a service status in SIM service table

*/

T_MFW sim_serv_table_check(UBYTE serv_num)
{
  TRACE_FUNCTION("sim_serv_table_check()");
  return sim_check_service(serv_num, sim_service_table);
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SIM           |
| STATE   : code                         ROUTINE : rAT_PlusCFUNP     |
+--------------------------------------------------------------------+

  PURPOSE : MMI relevant parameters from the SIM card

*/

void rAT_PlusCFUNP (T_SIM_MMI_INSERT_IND *mmi_insert_ind)
{
    TRACE_FUNCTION("rAT_PlusCFUNP()");

    sim_stat = SIM_ACTIVE;

    sim_config.oper_mode = mmi_insert_ind->func;

#ifdef FF_2TO1_PS
    nm_mmi_parameters(&mmi_insert_ind->imsi_field,
                      &mmi_insert_ind->pref_plmn);

    memcpy(sim_service_table,mmi_insert_ind->serv_table,
#else
    nm_mmi_parameters(&mmi_insert_ind->imsi_field);
    memcpy(sim_service_table,mmi_insert_ind->sim_serv,
#endif
                                sizeof(sim_service_table));
    sim_config.phase = mmi_insert_ind->phase;
    sim_config.access_acm = mmi_insert_ind->access_acm;
    sim_config.access_acmmax = mmi_insert_ind->access_acmmax;
    sim_config.access_puct = mmi_insert_ind->access_puct;


 /* Apr 13, 2004	REF: CRR 16303	JuanVi Jativa-Villoldo                   */
 /*  SPN must always be read, not only when nm_registration is called    */  
    sim_spn_req();       

   if (sim_check_service(15,sim_service_table)
        == ALLOCATED_AND_ACTIVATED)
        sim_read_sim(SIM_GID1, NOT_PRESENT_8BIT, 5);
    #ifdef BMI_TEST_MC_SIM_EVENT
    TRACE_EVENT("Reading SIM_ICCID");
     sim_read_sim(SIM_ICCID, NOT_PRESENT_8BIT, 5);
    #endif
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SIM           |
| STATE   : code                         ROUTINE : sim_mmi_parameter |
+--------------------------------------------------------------------+

  PURPOSE : MMI relevant parameters from the SIM card

*/

void sim_mmi_parameter(T_SIM_ACTIVATE_CNF *sim_act_cnf)
{
  TRACE_FUNCTION("sim_mmi_parameter()");

  memcpy( sim_config.pref_lang, sim_act_cnf->pref_lang, 5);
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SIM           |
| STATE   : code                         ROUTINE : sim_gid1_cnf      |
+--------------------------------------------------------------------+

  PURPOSE : read SIM group identifier 1 from SIM card

*/

void sim_gid1_cnf(USHORT error, UBYTE *data)
{
  TRACE_FUNCTION("sim_gid1_cnf()");

#ifdef FF_2TO1_PS
  if (error EQ CAUSE_SIM_NO_ERROR)
#else
  if (error EQ SIM_NO_ERROR)
#endif
    memcpy(sim_config.sim_gidl1, data, 5);

  if (!sat_update)
  {
    if (sim_check_service(16,sim_service_table)
        == ALLOCATED_AND_ACTIVATED)
        sim_read_sim(SIM_GID2, NOT_PRESENT_8BIT, 5);
  }
#ifdef SIM_TOOLKIT
  else
  {
    sat_update = FALSE;
    satUpdateFiles ( TRUE, SIM_GID1 );
  }
#endif
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SIM           |
| STATE   : code                         ROUTINE : sim_gid2_cnf      |
+--------------------------------------------------------------------+

  PURPOSE : read SIM group identifier 2 from SIM card

*/

void sim_gid2_cnf(USHORT error, UBYTE *data)
{
  TRACE_FUNCTION("sim_gid2_cnf()");
#ifdef FF_2TO1_PS
  if (error EQ CAUSE_SIM_NO_ERROR)
#else
  if (error EQ SIM_NO_ERROR)
#endif
    memcpy(sim_config.sim_gidl2, data, 5);

#ifdef SIM_TOOLKIT
  if ( sat_update )
  {
    sat_update = FALSE;
    satUpdateFiles ( TRUE, SIM_GID2 );
  }
#endif
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : sim_save_pref_lang |
+--------------------------------------------------------------------+

  PURPOSE : save preferred language

*/

void sim_save_pref_lang (UBYTE *lang, UBYTE len)
{
  UBYTE sim_data[7];
  UBYTE i;

  TRACE_FUNCTION("sim_save_pref_lang()");

  //
  // copy a maximum of 5 bytes for storing on the SIM card
  //
  for (i=0;i<len AND i<5;i++)
    sim_data[i] = lang[i];

  //
  // if less than 5 bytes fill with dummy values
  //
  for (i=len; i<5;i++)
    sim_data[i] = 0xFF;

  //
  // write to the SIM card
  //
  sim_write_sim(SIM_LP, sim_data, 5);
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : rAT_PercentSIMREM  |
+--------------------------------------------------------------------+

  PURPOSE : call back for SIM removed

*/

void rAT_PercentSIMREM( T_ACI_SIMREM_TYPE srType )
{
    TRACE_FUNCTION("rAT_PercentSIMREM()");

/* Dec 13, 2006 DR: OMAPS00105640  x0039928
    Fix: sim_stat is updated                                  */
      sim_stat = SIM_NOT_ACTIVE;	
    if ( srType EQ SIMREM_FAILURE )
    {
      sim_status.sim_procedure = MFW_SIM_REMOVED;
      sim_status.sim_status = MFW_SIM_NO_SIM_CARD;
      sim_status.sim_operation_mode = sim_config.oper_mode;
      sim_status.sim_pin_retries = 0;
      sim_status.sim_status_type = MFW_SIM_UNKNOWN;
      sim_signal(E_SIM_STATUS,&sim_status);
    }
    else if ( srType EQ SIMREM_RESET )
    {
#ifdef SIM_TOOLKIT
      sim_signal(E_SIM_RESET, 0); 
#endif
    }
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : rAT_percentSIMINS  |
+--------------------------------------------------------------------+

  PURPOSE : call back for SIM inserted

*/

void rAT_PercentSIMINS( T_ACI_CME_ERR err )
{
    TRACE_FUNCTION("rAT_percentSIMINS()");

    sim_signal(E_SIM_INSERTED, 0);

    switch ( err )
    {
      case CME_ERR_SimPinReq:
        sim_status.sim_status = MFW_SIM_PIN_REQ;
        sim_status.sim_pin_retries = sim_pin_count(MFW_SIM_PIN1);
        sim_status.sim_status_type = MFW_SIM_PIN1;
        sim_stat = SIM_ACTIVE;
        sim_signal(E_SIM_STATUS, &sim_status);
        break;

      case CME_ERR_SimPukReq:
        sim_status.sim_status = MFW_SIM_PUK_REQ;
        sim_status.sim_pin_retries = sim_pin_count(MFW_SIM_PUK1);
        sim_status.sim_status_type = MFW_SIM_PUK1;
        sim_stat = SIM_ACTIVE;
        sim_signal(E_SIM_STATUS, &sim_status);
        break;
	case CME_ERR_SimResetNeeded: /*OMAPS00115179 -a0393213 (R.Prabakar*/
	 {
	 	T_ACI_RETURN cfun;
		/*This event is not passed to BMI as there is no need currently*/
		cfun = sAT_PlusCFUN(CMD_SRC_LCL,CFUN_FUN_Full,CFUN_RST_NotPresent);		
		 if (cfun NEQ AT_EXCT)
		{
		    TRACE_ERROR("rAT_PercentSIMINS() : sAT_PlusCFUN error");
		}		
		  break;
	}

      default:
        break;
    }
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : sim_configuration  |
+--------------------------------------------------------------------+

  PURPOSE : Request the configuration of SIM card

*/

UBYTE sim_cvt_access_status(UBYTE acs)
{
    TRACE_FUNCTION("sim_cvt_access_status()");
    
    switch(acs)
    {
#ifdef FF_2TO1_PS
        case SIM_ACCESS_ALWAYS: return MFW_SIM_NO_PIN;
        case SIM_ACCESS_PIN_1:  return MFW_SIM_PIN1;
        case SIM_ACCESS_PIN_2:  return MFW_SIM_PIN2;
#else
        case ACCESS_ALWAYS: return MFW_SIM_NO_PIN;
        case ACCESS_PIN_1:  return MFW_SIM_PIN1;
        case ACCESS_PIN_2:  return MFW_SIM_PIN2;
#endif
        default:            return MFW_SIM_UNKNOWN;
    }
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : sim_configuration  |
+--------------------------------------------------------------------+

  PURPOSE : Request the configuration of SIM card

*/

MfwRes sim_configuration (U8 *phase, U8 *serv, U8 *slen,
                          U8 *lang, U8 *llen,
                          T_MFW_AOC_ACCESS *access)
{
    U8 l;
    TRACE_FUNCTION("sim_configuration()");

		//PATCH2 TB 1310: Add test of NULL parameter to avoid to declare parameter of unwanted data
    if (sim_stat != SIM_ACTIVE)
        return MfwResErr;/*a0393213 warnings removal-MFW_SIM_FAILURE changed to MfwResErr*/

		if (phase != NULL)
			*phase = sim_config.phase;

		if ((serv != NULL) && (slen != NULL))
		{			
			l = sizeof(sim_service_table);
			if (l < *slen)
					*slen = l;
			memcpy(serv,sim_service_table,*slen);
		}

    //PATCH1 TB 1310: replace slen by llen
		if ((lang != NULL) && (llen != NULL))
		{
			l = sizeof(sim_config.pref_lang);
			if (l < *llen)
					*llen = l;
			memcpy(lang, sim_config.pref_lang, *llen);
		}
		//END PATCH1 TB

		if (access != NULL)
		{
			access->access_acm    = sim_cvt_access_status(sim_config.access_acm);
			access->access_acmmax = sim_cvt_access_status(sim_config.access_acmmax);
			access->access_puct   = sim_cvt_access_status(sim_config.access_puct);
		}
		//END PATCH2 TB

    return MfwResDone;/*a0393213 warnings removal-MFW_SIM_SUCCESS changed to MfwResDone*/
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : sim_status_check   |
+--------------------------------------------------------------------+

  PURPOSE : check activation of SIM card

*/

int sim_status_check (void)
{
    TRACE_FUNCTION("sim_status_check()");

    return sim_stat;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : sim_pin_count      |
+--------------------------------------------------------------------+

  PURPOSE : Request PIN/PUK count

*/

int sim_pin_count(U8 pin_id)
{
    SHORT        pn1Cnt;
    SHORT        pn2Cnt;
    SHORT        pk1Cnt;
    SHORT        pk2Cnt;
    T_ACI_PVRF_STAT ps1;
    T_ACI_PVRF_STAT ps2;
        
    TRACE_FUNCTION("sim_pin_count()");


    if (qAT_PercentPVRF(CMD_SRC_LCL, &pn1Cnt,
                        &pn2Cnt, &pk1Cnt, &pk2Cnt, &ps1, &ps2) != AT_CMPL)
        return -1;

    switch (pin_id)
    {
        case MFW_SIM_PIN1:  return pn1Cnt;
        case MFW_SIM_PIN2:  return pn2Cnt;
        case MFW_SIM_PUK1:  return pk1Cnt;
        case MFW_SIM_PUK2:  return pk2Cnt;
        default:            return -1;
    }
}

int SimHasPin(U8 pin_id)
{
    SHORT        pn1Cnt;
    SHORT        pn2Cnt;
    SHORT        pk1Cnt;
    SHORT        pk2Cnt;
    T_ACI_PVRF_STAT ps1;
    T_ACI_PVRF_STAT ps2;
        
    TRACE_FUNCTION("sim_pin_count()");


    if (qAT_PercentPVRF(CMD_SRC_LCL, &pn1Cnt,
                        &pn2Cnt, &pk1Cnt, &pk2Cnt, &ps1, &ps2) != AT_CMPL)
        return -1;
	/***************************Go-lite Optimization changes Start***********************/
	//Aug 16, 2004    REF: CRR 24323   Deepa M.D
	TRACE_EVENT_P1("PIN2 STATUS: %d", ps2);
	/***************************Go-lite Optimization changes end***********************/
    switch (pin_id)
    {
        case MFW_SIM_PIN1:  
        { 	if (ps1 ==PVRF_STAT_NotPresent)
        		return 0;
        	else
        		return 1;
        } // break;		 // RAVI
        case MFW_SIM_PIN2:   
        { 	if (ps2 ==PVRF_STAT_NotPresent)
        		return 0;
        	else
        		return 1;
        }// break;	  // RAVI
        default:            return -1;
    }
}

/*SPR 1351, added new function*/
/*
+---------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM             |
| STATE   : code                        ROUTINE : sim_convert_simlock_status|
+---------------------------------------------------------------------+

  PURPOSE : Conversion of ACI simlock status to MFW simlock status codes for 
  compatibility with existing MMI layer.

*/
UBYTE sim_convert_simlock_status(T_SIMLOCK_STATUS aci_status)
{
	switch(aci_status)
	{
		case(SIMLOCK_DISABLED): return MFW_SIM_DISABLE; //break;  // RAVI
		case(SIMLOCK_ENABLED): return MFW_SIM_ENABLE; //break;  // RAVI
		case(SIMLOCK_BLOCKED): return MFW_SIM_BLOCKED; // break; // RAVI
		case(SIMLOCK_LOCKED): return MFW_SIM_LOCKED; //break; // RAVI
		case (SIMLOCK_FAIL): return MFW_SIM_FAILURE; // break; // RAVI
		default: return MFW_SIM_FAILURE; 
	}

}
/*
+---------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM             |
| STATE   : code                        ROUTINE : sim_unlock_sim_lock |
+---------------------------------------------------------------------+

  PURPOSE : de-personalisation(unlocking) for SIM lock.
  Returns code for lock type if successful, or return MFW_SIM_UNLOCK_ERR if not.

*/
/*MC SPR1351, replaced previous implemnetation with wrapper for aci implementation*/
T_MFW sim_unlock_sim_lock(UBYTE type, UBYTE *passwd)
{
  
  T_SIMLOCK_STATUS simlock_status=SIMLOCK_DISABLED;/*status from ACI  */
   /*for CQ-34324 -- While Integrating of VLE5  ME personalization code aci_slock_unlock 
      function is moved under SIM_PERS flag and this function is never called . So variable 
      simlock_status is hardcoded to default value */ 
  UBYTE mfw_simlock_status;/*status converted to MFW encoding*/

  TRACE_EVENT_P1("sim_unlock_sim_lock(): %d", type);


  switch (type)
  {
    /*
     * N-Lock
     */
    case MFW_SIM_NLOCK:
    #ifdef MMI_34324
     simlock_status= aci_slock_unlock(SIMLOCK_NETWORK, (char*)passwd);
    #endif
    mfw_simlock_status = sim_convert_simlock_status(simlock_status);
   switch(mfw_simlock_status)
   	{
   		case (MFW_SIM_DISABLE): return MFW_SIM_NLOCK;
    	case(MFW_SIM_LOCKED): return MFW_SIM_UNLOCK_ERR;
    	default: return MFW_SIM_UNLOCK_ERR;
   	}
//   break;  // RAVI

    /*
     * SP-Lock
     */
    case MFW_SIM_SPLOCK:
      #ifdef MMI_34324
	simlock_status = aci_slock_unlock(SIMLOCK_SERVICE_PROVIDER, (char*)passwd);
     #endif
    mfw_simlock_status = sim_convert_simlock_status(simlock_status);
       switch(mfw_simlock_status)
   	{

   		case (MFW_SIM_DISABLE): return MFW_SIM_SPLOCK;
    	case(MFW_SIM_LOCKED): return MFW_SIM_UNLOCK_ERR;
    	default: return MFW_SIM_UNLOCK_ERR;
   	}
//   break;  // RAVI
    
    /*
     * NS-Lock
     */
    case MFW_SIM_NSLOCK:
          #ifdef  MMI_34324
           simlock_status = aci_slock_unlock(SIMLOCK_NETWORK_SUBSET, (char*)passwd);
         #endif
    mfw_simlock_status = sim_convert_simlock_status(simlock_status);
       switch(mfw_simlock_status)
   	{

   		case (MFW_SIM_DISABLE): return MFW_SIM_NSLOCK;
    	case(MFW_SIM_LOCKED): return MFW_SIM_UNLOCK_ERR;
    	default: return MFW_SIM_UNLOCK_ERR;
   	}
//   break;  // RAVI

    /*
     * C-Lock
     */
    case MFW_SIM_CLOCK:
      #ifdef  MMI_34324
         simlock_status = aci_slock_unlock(SIMLOCK_CORPORATE, (char*)passwd);
      #endif
    mfw_simlock_status = sim_convert_simlock_status(simlock_status);
      switch(mfw_simlock_status)
   	{

   		case (MFW_SIM_DISABLE): return MFW_SIM_CLOCK;
    	case(MFW_SIM_LOCKED): return MFW_SIM_UNLOCK_ERR;
    	default: return MFW_SIM_UNLOCK_ERR;
   	}
//   break;  // RAVI

    /*
     * P-Lock
     */
    case MFW_SIM_PLOCK:

       #ifdef  MMI_34324
          simlock_status = aci_slock_unlock( SIMLOCK_SIM, (char*)passwd);
      #endif
    mfw_simlock_status = sim_convert_simlock_status(simlock_status);
	switch(mfw_simlock_status)
   	{
   		case (MFW_SIM_DISABLE): return MFW_SIM_PLOCK;
    	case(MFW_SIM_LOCKED): return MFW_SIM_UNLOCK_ERR;
    	default:return MFW_SIM_UNLOCK_ERR;
   	}
//   break;  // RAVI
      
  }

  return MFW_SIM_UNLOCK_ERR;
}

/*
+---------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM             |
| STATE   : code                        ROUTINE : sim_check_sim_Plock |
+---------------------------------------------------------------------+

  PURPOSE : check SIM lock

*/
/*MC SPR1351, replaced previous implemnetation with wrapper for aci implementation*/
int sim_check_sim_Plock (void)
{ 	T_SIMLOCK_STATUS simlock_status =  SIMLOCK_DISABLED; 
     /*ACI set default value for CQ-34324--While Integrating of VLE5 ME personalization code
        aci_slock_checklock function is removed  and this function is never called. So variable 
        simlock_status is hardcoded to default value  
      */

	TRACE_EVENT ("sim_check_sim_Plock");
       #ifdef  MMI_34324
	simlock_status =aci_slock_checklock(SIMLOCK_SIM);
       #endif
	TRACE_EVENT_P1("PLock: %d", simlock_status);
	/*convert status from ACI to MFW encoding*/ 
	return sim_convert_simlock_status(simlock_status);

}


/*
+---------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM             |
| STATE   : code                        ROUTINE : sim_check_sim_Nlock |
+---------------------------------------------------------------------+

  PURPOSE : Check network lock

*/
/*MC SPR1351, replaced previous implemnetation with wrapper for aci implementation*/
int sim_check_sim_Nlock (void)
{	T_SIMLOCK_STATUS simlock_status  = SIMLOCK_DISABLED; 
       /*ACI set default value for CQ-34324--While Integrating of VLE5 ME personalization code
        aci_slock_checklock function is removed  and this function is never called. So variable 
        simlock_status is hardcoded to default value  
      */
 	TRACE_EVENT ("sim_check_sim_Nlock()");
        #ifdef  MMI_34324
	  simlock_status =aci_slock_checklock(SIMLOCK_NETWORK);
       #endif
	TRACE_EVENT_P1("NLock: %d", simlock_status);
	/*convert status from ACI to MFW encoding*/ 
	return sim_convert_simlock_status(simlock_status);


}


/*
+---------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM             |
| STATE   : code                        ROUTINE : sim_check_sim_SPlock|
+---------------------------------------------------------------------+

  PURPOSE : Check SIM service provider lock. It only works with SIMs
            which support GID1 file.

*/
/*MC SPR1351, replaced previous implemnetation with wrapper for aci implementation*/
int sim_check_sim_SPlock (void)
{	T_SIMLOCK_STATUS simlock_status = SIMLOCK_DISABLED;
      /*ACI set default value for CQ-34324--While Integrating of VLE5 ME personalization code
        aci_slock_checklock function is removed  and this function is never called. So variable 
        simlock_status is hardcoded to default value  
      */
	TRACE_EVENT ("sim_check_sim_SPlock()");
	
       #ifdef  MMI_34324
        simlock_status =aci_slock_checklock(SIMLOCK_SERVICE_PROVIDER);
       #endif
	   
	TRACE_EVENT_P1("NLock: %d", simlock_status);
	/*convert status from ACI to MFW encoding*/ 
	return sim_convert_simlock_status(simlock_status);

}


/*
+---------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM             |
| STATE   : code                        ROUTINE : sim_check_sim_NSlock|
+---------------------------------------------------------------------+

  PURPOSE : Check network subset lock

*/
/*MC SPR1351, replaced previous implementation with wrapper for aci implementation*/
int sim_check_sim_NSlock (void)
{	T_SIMLOCK_STATUS simlock_status  = SIMLOCK_DISABLED; 
       /*ACI set default value for CQ-34324--While Integrating of VLE5 ME personalization code
        aci_slock_checklock function is removed  and this function is never called. So variable 
        simlock_status is hardcoded to default value  
      */
	TRACE_EVENT ("sim_check_sim_NSlock()");
	
       #ifdef  MMI_34324
         simlock_status =aci_slock_checklock(SIMLOCK_NETWORK_SUBSET);
       #endif
	TRACE_EVENT_P1("NSLock: %d", simlock_status);

	/*convert status from ACI to MFW encoding*/ 
	return sim_convert_simlock_status(simlock_status);
}

/*
+---------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM             |
| STATE   : code                        ROUTINE : sim_check_sim_Clock |
+---------------------------------------------------------------------+

  PURPOSE : Check corporate lock. It only works with SIMs
            which support GID1 and GID2 files.

*/
/*MC SPR1351, replaced previous implemnetation with wrapper for aci implementation*/
int sim_check_sim_Clock (void)
{	T_SIMLOCK_STATUS simlock_status = SIMLOCK_DISABLED; 
       /*ACI set default value for CQ-34324--While Integrating of VLE5 ME personalization code
        aci_slock_checklock function is removed  and this function is never called. So variable 
        simlock_status is hardcoded to default value  
      */
	TRACE_EVENT ("sim_check_sim_Clock()");
       #ifdef  MMI_34324
        simlock_status =aci_slock_checklock(SIMLOCK_CORPORATE);
       #endif
	TRACE_EVENT_P1("CLock: %d", simlock_status);
	/*convert status from ACI to MFW encoding*/ 
	return sim_convert_simlock_status(simlock_status);
}



/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : sim_pin_status     |
+--------------------------------------------------------------------+

  PURPOSE : Request PIN1/PIN2 status

*/

void sim_pin_status(T_MFW_SIM_PIN_STATUS *status)
{
  SHORT           pn1Cnt;
  SHORT           pn2Cnt;
  SHORT           pk1Cnt;
  SHORT           pk2Cnt;
  T_ACI_PVRF_STAT ps1;
  T_ACI_PVRF_STAT ps2;
//  char debug[30];  // RAVI
  TRACE_FUNCTION("sim_pin_status()");

  status->stat = MFW_SIM_UNKNOWN;
  status->set  = MFW_SIM_UNKNOWN;
  
  if (qAT_PercentPVRF(CMD_SRC_LCL, &pn1Cnt, 
                      &pn2Cnt, &pk1Cnt, &pk2Cnt, &ps1, &ps2) == AT_CMPL)
  {
    switch (status->type)
    {
      case MFW_SIM_PIN1:
		/*CONQ 5578, check service table to see if allowed to disable PIN1*/
      	if (!psaSIM_ChkSIMSrvSup(SRV_CHV1_Disable))
      	{
			status->stat = MFW_SIM_NO_DISABLE;
      	}
      	else
      	{
        	status->stat = sim_cvtPINstatus(ps1, MFW_SIM_PIN1);
      	}
        status->set = pin1_set;
        break;
      case MFW_SIM_PIN2:
        status->stat = sim_cvtPINstatus(ps2, MFW_SIM_PIN2);
        status->set  = pin2_set;
        break;
    }
  }
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : sim_cvtPINstatus   |
+--------------------------------------------------------------------+

  PURPOSE :

*/

UBYTE sim_cvtPINstatus(T_ACI_PVRF_STAT ps, UBYTE type)
{
  TRACE_FUNCTION("sim_cvtPINstatus()");

  switch (ps)
  {
    case PVRF_STAT_NotRequired: 
      return MFW_SIM_NO_PIN;
    case PVRF_STAT_Required:
      if (type EQ MFW_SIM_PIN1) 
        return MFW_SIM_PIN_REQ;
      if (type EQ MFW_SIM_PIN2) 
        return MFW_SIM_PIN2_REQ;
      return MFW_SIM_UNKNOWN;
    default:                    
      return MFW_SIM_UNKNOWN;
  }
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : simDecodeIMSI      |
+--------------------------------------------------------------------+

  PURPOSE : convert imsi (packed bcd to ASCIIZ; ->11.11)

*/

void simDecodeIMSI (UBYTE * imsi_field, 
                    UBYTE   imsi_c_field, 
                    UBYTE * imsi_extracted)
{
  UBYTE length;
  UBYTE i;
  UBYTE digit;

  TRACE_FUNCTION ("simDecodeImsi()");

  /*
   * calculate number of digits
   */
  length = (imsi_c_field-1)*2;

  /*
   * if odd number of digits add one
   */
  if (imsi_field[0] & 0x08)
    length++;

  /*
   * extract all digits
   */
  for (i = 0; i < length; i++)
  {
    digit = (i & 1) ?
        imsi_field[(i + 1) / 2] & 0x0f :
       (imsi_field[(i + 1) / 2] & 0xf0) >> 4;

#if defined (WIN32)
    {
		/***************************Go-lite Optimization changes Start***********************/
		//Aug 16, 2004    REF: CRR 24323   Deepa M.D
		TRACE_EVENT_P2("DIGIT [%d] = %d",i, digit);
		/***************************Go-lite Optimization changes end***********************/
    }
#endif
    if (i < LONG_NAME)
      imsi_extracted [i] = digit + 0x30;
    else
    {
      imsi_extracted [i] = 0;
      return;
    }
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : sim_pin_ident      |
+--------------------------------------------------------------------+

  PURPOSE : handle mfw windows command

*/

void sim_pin_ident(UBYTE id)
{
  TRACE_FUNCTION("sim_pin_ident()");
  pin_ident = id;
}



/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : simCommand         |
+--------------------------------------------------------------------+

  PURPOSE : handle mfw windows command

*/

static int simCommand (U32 cmd, void *h)
{
    switch (cmd)
    {
        case MfwCmdDelete:              /* delete me                */
            if (!h)
                return 0;
            sim_delete(h);
            return 1;
        default:
            break;
    }

    return 0;
}







/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)     MODULE  : MFW_SIM               |
| STATE   : code                     ROUTINE : sim_simlock_ok        |
+--------------------------------------------------------------------+

  PURPOSE : The function checks all simlocks and returns SIMLOCK_DISABLED
  if none are set.
            
*/
/*spr 1351, replaced previous implemntation with a wrapper for aci implentation*/
UBYTE sim_simlock_ok (void)
{	
	T_SIMLOCK_STATUS simlock_status = SIMLOCK_ENABLED; 
       /*ACI set default value for CQ-34324 -- While Integrating of VLE5 ME personalization code 
         aci_slock_checkpersonalisation function is moved under SIM_PERS flag and this function is 
         never called . So variable simlock_status is hardcoded to default value */
      #ifdef  MMI_34324       
	simlock_status = aci_slock_checkpersonalisation();
      #endif
	return sim_convert_simlock_status(simlock_status);

}

void mfwSimRestartPinEntry(void)
{
        sim_signal(E_SIM_STATUS, &sim_status);
	return;
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : sim_get_imsi       |
+--------------------------------------------------------------------+

  PURPOSE : Request IMSI number in SIM card

*/

UBYTE * sim_get_imsi (void)
{
  return mfw_IMSI;
}

/*
+---------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)   MODULE  : MFW_SIM                  |
| STATE   : code                   ROUTINE : sim_mmi_update_parameter |
+---------------------------------------------------------------------+

  PURPOSE : MMI relevant parameters from the SIM card

*/

void sim_mmi_update_parameter(T_SIM_ACTIVATE_IND *sim_act_ind)
{
  TRACE_FUNCTION("sim_mmi_update_parameter()");

  memcpy( sim_config.pref_lang, sim_act_ind->pref_lang, 5);
}

#ifdef SIM_TOOLKIT
/*
+---------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)   MODULE  : MFW_SIM                  |
| STATE   : code                   ROUTINE : sim_sat_file_update      |
+---------------------------------------------------------------------+

  PURPOSE : SIM file change indication

*/

void sim_sat_file_update(USHORT dataId)
{
  TRACE_FUNCTION("sim_sat_file_update()");

  sat_update = TRUE;
  switch (dataId)
  {
    case SIM_SST:
      sim_read_sim(SIM_SST, NOT_PRESENT_8BIT, 10);
      break;
    case SIM_GID1:
      sim_read_sim(SIM_GID1, NOT_PRESENT_8BIT, 5);
      break;
    case SIM_GID2:
      sim_read_sim(SIM_GID2, NOT_PRESENT_8BIT, 5);
      break;
    case SIM_LP:
	{
		  T_ACI_LAN_SUP   lngCde;
		  CHAR            lang_buffer[3]; 

		  memset(lang_buffer, 0, sizeof(lang_buffer));
		  lngCde.str = lang_buffer;
	  if (qAT_PlusCLAN(CMD_SRC_LCL, &lngCde) == AT_EXCT)
			return;
#ifdef SIM_TOOLKIT
	  else
	  if (mfwSATLPRefresh)
	  {
		sat_update = FALSE;
		mfwSATLPRefresh =FALSE;
		psaSAT_FUConfirm (0, SIM_FU_ERROR);	/*If not possible, notify to the SIM */
	  }
#endif
	}
	  break;
    default:
      break;
  }
}
#endif

/*
+---------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)   MODULE  : MFW_SIM                  |
| STATE   : code                   ROUTINE : sim_read_sst_cnf         |
+---------------------------------------------------------------------+

  PURPOSE : SIM file change indication

*/

void sim_read_sst_cnf (USHORT error, UBYTE *data)
{
  TRACE_FUNCTION("sim_read_sst_cnf()");

#ifdef FF_2TO1_PS
  if (error EQ CAUSE_SIM_NO_ERROR)
#else
  if (error EQ SIM_NO_ERROR)
#endif
    memcpy(sim_service_table, data, sizeof(sim_service_table));
  else
    TRACE_EVENT("SST read error");

#ifdef SIM_TOOLKIT
  if (sat_update)
  {
    sat_update = FALSE;
    satUpdateFiles ( TRUE, SIM_SST );
  }
#endif
}

//TISH, patch for ASTec32515/OMAPS00134509
//start
void sim_invalid_ind(void)
{
			sim_status.sim_status = MFW_SIM_INVALID_CARD;
             sim_signal(E_SIM_STATUS,&sim_status);
}
//end
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : sim_init_CPRS    |
+--------------------------------------------------------------------+

  PURPOSE :  initialize the ciphering indication 

*/

UBYTE sim_init_CPRS (CallbackCPRI func, UBYTE CPRImode )
{

	//UBYTE getCPRImode;  // RAVI

	TRACE_FUNCTION("sim_init_CPRS()");
	
	//keep the callback function from MMI
	if (func != NULL)
		mmi_cprs_cb = func;
	else
		return FALSE;

	//now set the mode
	sAT_PercentCPRI (CMD_SRC_LCL, CPRImode);

	return TRUE;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : rAT_PercentCPRI    |
+--------------------------------------------------------------------+

  PURPOSE :  is called to inform the application about a received ciphering indication. 
  gsm_ciph and gprs_ciph are used to inform the application about a change in ciphering 
  mode in GSM or GPRS.

*/


void rAT_PercentCPRI (UBYTE gsm_ciph, UBYTE gprs_ciph)
{

	//call here the function from MMI
	mmi_cprs_cb(gsm_ciph, gprs_ciph);
}

/*
+---------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)   MODULE  : MFW_SIM                 |
| STATE   : code                   ROUTINE : sim_read_lp_cnf            |
+---------------------------------------------------------------------+

  PURPOSE : 

*/ 

void sim_read_lp_cnf (USHORT error, T_ACI_LAN_SUP  *CLang)
{
  TRACE_FUNCTION("sim_read_lp_cnf()");


#ifdef SIM_TOOLKIT
  if ((mfwSATLPRefresh)&&(sat_update))
  {
	mfwSATLPRefresh= FALSE;
    sat_update = FALSE;
#ifdef FF_2TO1_PS
	if (error EQ CAUSE_SIM_NO_ERROR)
#else
	if (error EQ SIM_NO_ERROR)
#endif
	    satUpdateFiles ( TRUE, SIM_SST );
	else
        psaSAT_FUConfirm (0, SIM_FU_ERROR);	/*If not possible, notify to the SIM */
  }
#endif
}

/*
+---------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)   MODULE  : MFW_SIM                 |
| STATE   : code                   ROUTINE : sim_simlock_cpin_code            |
+---------------------------------------------------------------------+

  PURPOSE : This function checks for the code and take action for SIMP category unlocking 
*/ 
#ifdef SIM_PERS
void sim_simlock_cpin_code (T_ACI_CPIN_RSLT code)
{
    TRACE_FUNCTION("sim_simlock_cpin_code()");

    sim_status.sim_procedure = MFW_SIM_ACTIVATION;
    sim_status.sim_operation_mode = sim_config.oper_mode;
    sim_status.sim_pin_retries = 0;
    sim_stat = SIM_ACTIVE;
    pin_flag = -1;
    pin_ident = MFW_SIM_UNKNOWN;
  
	 if( code EQ CPIN_RSLT_SimReady )
	  {
		TRACE_EVENT("CPIN_RSLT_SimReady");
      		sim_status.sim_status = MFW_SIM_NO_PIN;
     		 sim_status.sim_status_type = pin_ident;
	      pin_ident = MFW_SIM_UNKNOWN;
		  simp_cpin_flag = FALSE;
      		sim_signal(E_SIM_STATUS,&sim_status);
	  	return ;
	  }
    switch ( code )
    {
      case CPIN_RSLT_PhSimPinReq:         
            sim_status.sim_status = MFW_SIM_PLOCK_REQ;
            sim_status.sim_status_type = MFW_SIM_LOCKED ;
            sim_signal(E_SIM_STATUS,&sim_status);
  	    break;

      case CPIN_RSLT_PhNetPinReq:
         	sim_status.sim_status = MFW_SIM_NLOCK_REQ;
         	sim_status.sim_status_type = MFW_SIM_LOCKED;
         	sim_signal(E_SIM_STATUS,&sim_status);
    		break;

         case CPIN_RSLT_PhNetSubPinReq:
         	sim_status.sim_status = MFW_SIM_NSLOCK_REQ;
         	sim_status.sim_status_type = MFW_SIM_LOCKED;
 			sim_signal(E_SIM_STATUS,&sim_status);
    		break;

         case CPIN_RSLT_PhSPPinReq:
         	sim_status.sim_status = MFW_SIM_SPLOCK_REQ;
         	sim_status.sim_status_type = MFW_SIM_LOCKED;
         	sim_signal(E_SIM_STATUS,&sim_status);
    		break;

         case CPIN_RSLT_PhCorpPinReq:
         	sim_status.sim_status = MFW_SIM_CLOCK_REQ;
         	sim_status.sim_status_type = MFW_SIM_LOCKED;
         	sim_signal(E_SIM_STATUS,&sim_status);
    		break;

	case CPIN_RSLT_PhNetPukReq:
      	case CPIN_RSLT_PhNetSubPukReq:
     	case CPIN_RSLT_PhSPPukReq:
     	case CPIN_RSLT_PhCorpPukReq:
	case CPIN_RSLT_PhSimFail:
    	     C_KEY_REQ = 1;	
            sim_status.sim_status = MFW_SIM_PUK_REQ;
            sim_status.sim_pin_retries = sim_pin_count(MFW_SIM_PUK1);
            sim_status.sim_status_type = MFW_SIM_PUK1;
            sim_status.sim_procedure = MFW_SIM_ACTIVATION;
            sim_signal(E_SIM_STATUS,&sim_status);
    	    break;

	//x0pleela 25 Sep, 2006 ER: OMAPS00095524
	//Set the reqd. parameters for Blocked Network Password request
	case CPIN_RSLT_PhBlockedNetPinReq:
         	sim_status.sim_status = MFW_SIM_PBLOCK_REQ;
         	sim_status.sim_status_type = MFW_SIM_LOCKED;
         	sim_signal(E_SIM_STATUS,&sim_status);
    		break;
			
	//x0pleela 25 Sep, 2006 ER: OMAPS00095524
	//Set the reqd. parameters for Blocked Network Unblock code request
	case CPIN_RSLT_PhBlockedNetPukReq:
    	     C_KEY_REQ = 1;	
            sim_status.sim_status = MFW_SIM_PUK_REQ;
            sim_status.sim_pin_retries = sim_pin_count(MFW_SIM_PUK1);
            sim_status.sim_status_type = MFW_SIM_PUK1;
            sim_status.sim_procedure = MFW_SIM_ACTIVATION;
            sim_signal(E_SIM_STATUS,&sim_status);
    	    break;

        default:
            TRACE_ERROR("sim_simlock_cpin_code (): result");
            break;
    }  
	return ;
}
#endif

//x0pleela 19 Feb, 2007  ER: OMAPS00122561
#ifdef FF_PHONE_LOCK

/*******************************************************************************
 $Function:		mfw_phlock_get_lock_status
 $Description:	returns the status of the lock - enabled / disabled
 $Returns:		lock status
 $Arguments:	lock type
 
*******************************************************************************/
int mfw_phlock_get_lock_status(S16 lcktyp)
{
    T_ACI_CLSSTAT clsStat;
    T_ACI_FAC lckfac;

    TRACE_FUNCTION("mfw_phlock_get_lock_status()");
	
	lckfac = FAC_Pl;

	if (qAT_PlusCLCK(CMD_SRC_LCL, lckfac,
                     CLASS_NotPresent, &clsStat) EQ AT_CMPL)
    {
    TRACE_FUNCTION("qAT_PlusCLCK->AT_CMPL ");
        switch (clsStat.status)
        {
            case STATUS_NotActive:
		  TRACE_FUNCTION("MFW_PH_LOCK_DISABLE ");

		  //x0pleela 29 May, 2007  DR: OMAPS00132483
		  //set the phone lock status to FALSE
		  mfw_set_Phlock_status(FALSE);
                return MFW_PH_LOCK_DISABLE;

            case STATUS_Active:
		  TRACE_FUNCTION("MFW_PH_LOCK_ENABLE ");

  		  //x0pleela 29 May, 2007  DR: OMAPS00132483
  		  //set the phone lock status to TRUE
		  mfw_set_Phlock_status(TRUE);

                return MFW_PH_LOCK_ENABLE;
        }
    }
    return MFW_PH_LOCK_FAILURE;
    
}

/*******************************************************************************
 $Function:		mfw_autophlock_get_lock_status
 $Description:	returns the status of the lock - enabled / disabled
 $Returns:		lock status
 $Arguments:	lock type
 
*******************************************************************************/
int mfw_phlock_get_autolock_status(S16 lcktyp)
{
    T_ACI_CLSSTAT clsStat;
    T_ACI_FAC lckfac=FAC_NotPresent;

    TRACE_FUNCTION("mfw_phlock_get_lock_status()");

	if( lcktyp == MFW_AUTO_PH_LOCK)
		lckfac = FAC_Apl;

	if (qAT_PlusCLCK(CMD_SRC_LCL, lckfac,
                     CLASS_NotPresent, &clsStat) EQ AT_CMPL)
    {
    TRACE_FUNCTION("qAT_PlusCLCK->AT_CMPL ");
        switch (clsStat.status)
        {
            case STATUS_NotActive:
		  TRACE_FUNCTION("MFW_PH_LOCK_DISABLE ");
		  
  		  //x0pleela 29 May, 2007  DR: OMAPS00132483
  		  //set the auto phone lock status to FALSE
		  mfw_set_Auto_Phlock_status(FALSE);		  
                return MFW_PH_LOCK_DISABLE;

            case STATUS_Active:
		  TRACE_FUNCTION("MFW_PH_LOCK_ENABLE ");

    		  //x0pleela 29 May, 2007  DR: OMAPS00132483
    		  //set the auto phone lock status to TRUE
		  mfw_set_Auto_Phlock_status(TRUE);		
                return MFW_PH_LOCK_ENABLE;
        }
    }
    return MFW_PH_LOCK_FAILURE;
    
}
/*******************************************************************************
 $Function:		mfw_phlock_enable_lock
 $Description:	disables the lock with the given password for the given lock
 $Returns:		success or failure
 $Arguments:	type of lock and password
 
*******************************************************************************/
T_MFW_SS_RETURN mfw_phlock_enable_lock(U8 *pin, S16 lcktyp)
{
    T_ACI_FAC lckFac=FAC_NotPresent;
     T_ACI_RETURN ret;
    TRACE_FUNCTION("mfw_phlock_enable_lock()");
	if( lcktyp == MFW_AUTO_PH_LOCK )
		lckFac = FAC_Apl;
	else if ( lcktyp == MFW_PH_LOCK )
		lckFac = FAC_Pl;

  	ret = sAT_PlusCLCK(CMD_SRC_LCL,lckFac,CLCK_MOD_Lock,(char*)pin,CLASS_NotPresent); 
	  switch(ret)
	  {
	  	case AT_CMPL:
			return MFW_SS_OK;
		case AT_FAIL:
			return MFW_SS_FAIL;
	  }
  return MFW_SS_FAIL; 
}

/*******************************************************************************
 $Function:		mfw_phlock_disable_lock
 $Description:	disables the lock with the given password for the given lock
 $Returns:		success or failure
 $Arguments:	type of lock and password
 
*******************************************************************************/
T_MFW_SS_RETURN mfw_phlock_disable_lock(U8 *pin, S16 lcktyp)
{
	T_ACI_FAC lckFac=FAC_NotPresent;
	T_ACI_RETURN ret;

	TRACE_FUNCTION("mfw_phlock_disable_lock()");
	if( lcktyp == MFW_PH_LOCK )
		lckFac = FAC_Pl;

		ret =  sAT_PlusCLCK(CMD_SRC_LCL,lckFac,CLCK_MOD_Unlock,(char*)pin,CLASS_NotPresent) ; 
	if(ret != AT_CMPL)
		return MFW_SS_FAIL;
	else
		return MFW_SS_OK;
}

/*******************************************************************************
 $Function:		mfw_phlock_disable_autoLock
 $Description:	disables the auto lock when user selects Autolock Off option from menu
 $Returns:		success or failure
 $Arguments:	type of lock and password
 
*******************************************************************************/
T_MFW_SS_RETURN mfw_phlock_disable_autoLock( S16 lcktyp)
{
	T_ACI_FAC lckFac=FAC_NotPresent;
	T_ACI_RETURN ret;

	TRACE_FUNCTION("mfw_phlock_disable_lock()");
	if( lcktyp == MFW_AUTO_PH_LOCK )
		lckFac = FAC_Apl;

	if( mfw_phlock_get_autolock_status(MFW_AUTO_PH_LOCK) == MFW_PH_LOCK_ENABLE ) 
	{	
		ret =  sAT_PlusCLCK(CMD_SRC_LCL,lckFac,CLCK_MOD_Unlock,NULL,CLASS_NotPresent) ;

	if(ret != AT_CMPL)
		return MFW_SS_FAIL;
	else
		return MFW_SS_OK;
	}
	else
	{
		return MFW_SS_ALRDY_DIS;
	}
}

/*******************************************************************************
 $Function:		mfw_phlock_change_lock_code
 $Description:	Changes the oldpassword with the new one for the given 
 				lock type
 $Returns:		success or failure
 $Arguments:	type of lock and old and new password
 
*******************************************************************************/
T_MFW mfw_phlock_change_lock_code(int type,char*oldpswd,char* newpswd)
{
    T_ACI_FAC lckType=FAC_NotPresent;
	T_ACI_RETURN ret;

		TRACE_FUNCTION("mfw_phlock_change_lock_code()");
	if(type == MFW_PH_LOCK )
	lckType= FAC_Pl;

    ret = sAT_PlusCPWD(CMD_SRC_LCL, lckType, (char *)oldpswd,(char *)newpswd);
	TRACE_EVENT_P1("mfw_phlock_change_lock_code:ret: %d", ret);
	if( 	ret != AT_CMPL )
	{
	TRACE_FUNCTION("mfw_phlock_change_lock_code: FAILED");
		return MFW_SS_FAIL;
	}
	else
		{
		TRACE_FUNCTION("mfw_phlock_change_lock_code: SUCCESS");
       return MFW_SS_OK;
		}
}

/*******************************************************************************

 $Function:  mfw_set_Auto_Phlock_status

 $Description:	 set /resets the AutoPhlockStatus bit in Phlock_flag 

 $Returns:    none.

 $Arguments:

*******************************************************************************/
//x0pleela 29 May, 2007  DR: OMAPS00132483
GLOBAL void mfw_set_Auto_Phlock_status( int phlock_status)
{
	TRACE_FUNCTION("mfw_set_Auto_Phlock_status()");
	if( phlock_status )
		Phlock_flag |=  AutoPhlockStatus;
	else
		Phlock_flag &= Phlock_ResetFlag;
	return;
}

/*******************************************************************************

 $Function:  mfw_get_Auto_Phlock_status

 $Description:	 returns the AutoPhlockStatus status from Phlock_flag 

 $Returns:    none.

 $Arguments:

*******************************************************************************/
//x0pleela 29 May, 2007  DR: OMAPS00132483
GLOBAL int mfw_get_Auto_Phlock_status(void)
{
	TRACE_FUNCTION("mfw_get_Auto_Phlock_status()");
	if( (Phlock_flag & AutoPhlockStatus) == AutoPhlockStatus)
		return TRUE;
	else
		return FALSE;
}


/*******************************************************************************

 $Function:  mfw_set_Phlock_status

 $Description:	 set /resets the PhlockStatus bit in Phlock_flag 

 $Returns:    none.

 $Arguments:

*******************************************************************************/
//x0pleela 29 May, 2007  DR: OMAPS00132483
GLOBAL void mfw_set_Phlock_status( int phlock_status)
{
	TRACE_FUNCTION("mfw_set_Phlock_status()");
	if( phlock_status )
		Phlock_flag |=  PhlockStatus;
	else
		Phlock_flag &= Phlock_ResetFlag;
	return;
}

/*******************************************************************************

 $Function:  mfw_get_Phlock_status

 $Description: returns the PhlockStatus status from Phlock_flag 

 $Returns:    none.

 $Arguments:

*******************************************************************************/
//x0pleela 29 May, 2007  DR: OMAPS00132483
GLOBAL int mfw_get_Phlock_status(void)
{
	TRACE_FUNCTION("mfw_get_Phlock_status()");
	if( (Phlock_flag & PhlockStatus) == PhlockStatus)
		return TRUE;
	else
		return FALSE;
}
#endif /* FF_PHONE_LOCK */


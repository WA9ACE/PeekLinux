/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_nm.c        $|
| $Author:: Es                          $Revision::  1              $|
| CREATED: 07.10.98                     $Modtime:: 2.03.00 16:28    $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_NM

   PURPOSE : This modul contains the functions for network management.


   $History:: mfw_nm.c

   Aug 07, 2007 DR:OMAPS00137726 x0066814(Geetha)
   Description:HPLMN Manual Selection at power on -
            unable to perform manual selection at power ON
   Solution: Added a new function nm_get_plmn_list_during_bootup() 
            to get the PLMN list during boot up.
  

   Apr 13, 2007 DRT:OMAPS00122739 x0066814(Geetha)
   Description: With Orange SIM card, once attached (at+cgatt=1), "5201" is displayed instead of "Orange"
   Solution: In nm_nitz_info_ind(), commented the code to copy mnc, mcc, long name and short anme 
   to net_ind from mmr_info_ind.
   May 31, 2007 DR: OMAPS00133201 x0pleela
   Description: MS keeps searching networks and doesn't list available PLMNs after 
   			performing manual selection
   Solution : Modified the function nm_error_cops() to check for 
   			CME_ERR_AbortedByNetwork error and send event 
   			E_NM_ABORT_NETWORK to BMI to destroy the n/w searching dialog

   Mar 12 2007 ,x0061088 (Prachi)
   Removed function nm_restrict_bands()as function pl_pei_config() is no 
   more avalilable in g23m.


   Oct 30 2006, OMAPS00098881 x0039928(sumanth)
   Removal of power variant
      
   Sep 18,2006    DRT: OMAPS0090268 x0061088(Prachi)
   Description : To check for Forbidden network (for denied services),it takes long time.
   Solution : Function rAT_PercentCREG() has now be implemented to handle CREG_STAT_Denied,
   which sends nm_signal(NETWORK_FORBIDDEN_NETWORK) to Bmi .   

   June 02, 2006    DR: OMAPS00078005 x0021334
   Descripton: Test for NITZ (Network Identity and Timezone) support - No New time was displayed
   Solution: New Time/Date received from network is sent to MFW by lower layer by invoking 'rAT_PercentCTZV'
   function. This function has now been implemented to pass on the received values to MMI
   for user confirmation and RTC updation.   

	Jun 23, 2006    DR: OMAPS00082374 x0pleela
   	Description: EONS Failures - unexpected output from the MMI: 2 carriers; on the top always T-Mobile 
   	Solution:	a) Removed the usage of globals gEons_alloc_active and opn_sel_done
			b) Removed the check if( plmn_ident->opn_read_from EQ NM_EONS ) in function nm_update_plmn_data
			c) Updating the long name and short name lengths in function nm_update_plmn_data
			d) Removed the code where plmn_ident was updated with new MNC and MCC after qat_%COPN in function nm_update_plmn_data
			e) Updated function nm_get_opername() by calling only sat_%CNIV()
			f) Removed the function definitions of set_opn_sel_done and get_opn_sel_done
   	
    May 15, 2006   OMAPS00075777  a0393213 (R.Prabakar)
    Description :  It is supposed that the sample register itself automatically (in Manual Mode) 
                   to the last manual selected HPLMN cell before the power cycle, and this does not happen; 
                   instead this, the sample shows up the available networks list
    Fix         :  call to nm_avail_plmn_list() is changed to call to sAT_PercentNRG() 

	Feb 15, 2006    ER: OMAPS00065203 x0pleela
   	Description: Cingular Operator Name Prioritization implementation required
   	Solution: Incorporated review comments
   			- Changed from NEQ NM_CPHS to EQ NM_ROM as we know %CNIV will 
   				be called only if source id is ROM or CPHS and not roaming in nm_get_opername()
   			- set_opn_sel_done(): sets opn_sel_done flag to TRUE or FALSE. 
   			- get_opn_sel_done(): returns the current value of flag opn_sel_done.

	Feb 09, 2006    ER: OMAPS00065203 x0pleela
   	Description: Cingular Operator Name Prioritization implementation required
   	Solution: Handling E_NM_OPN for operator name display
			Query for COPN in rAT_PlusCREG and update plmn data
			Defined following new functions:
				nm_decodeSourceId:to convert the source id used by ACI to the source id used by MFW
				nm_update_plmn_data: update plmn data after querying COPN
				nm_get_COPN: query COPN
				nm_get_opername: updates operator name based on cingular's prioritization requirement. 
							 Priority would be in order ONS->NITZ->ROM
				nm_get_network_data: gets operator name from network

	Nov 24, 2005 DR: OMAPS00045909 - Shashi Shekar B.S.
	Description: Improve IMEI control mechanism
	Solution : When MMI calls sAT_PLUSCFUN, if an IMEI invalid error is returned,
		we will block on that screen & will not allow the user to browse menus further,
		since the PS will not be booted at all!!!

	July 14, 2005  REF: CRR 31916  x0021334
    	Description: rCI_PercentCOPS() and rAT_PercentCOPS() is not implemented
    	Solution: rAT_PercentCOPS() is implemented. 

	Mar 01, 2005    REF: ENH 29327   xdeepadh
	Bug:Locosto MMI Optimization
	Fix:The changes done for the Golite optimization have been utilised for color build.
	This is done by removing the functions under #ifndef MMI_LITE.
	The static vaiable T_MFW_PLMN_LIST plmn_list has been made local variable.
	It has been dynamically allocated and deallocated.
	
	Aug 16, 2004    REF: CRR 24293   Deepa M.D
	Bug:Go-lite optimisation dynamic allocation of variable 'plmn_list'
	Fix:The static vaiable T_MFW_PLMN_LIST plmn_list has been made local variable.
	It has been dynamically allocated and deallocated.


	Aug 16, 2004    REF: CRR 24323   Deepa M.D
	Bug:Clenup of sprintf used for tracing
	Fix:Replace the char buf[]; sprintf (buf, "...", ...); TRACE_EVENT (buf); statements by TRACE_EVENT_PX                                         $
 
    Sep 23, 2004        REF: CRR 24562  xvilliva
    Bug:	Enable/disable the manual network selection option depending
			on ENS.
    Fix:	If the manual network selection is disabled we check if an 
			error is returned and set the global g_ensErr.


  May 22, 2006   X0047685
  Removed pref_plmn for if not defined FF_2TO1_PS

   
 *
 * *****************  Version 39  *****************
 * User: Es           Date: 2.03.00    Time: 16:31
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * use 'aci_delete()' to clear the primitive routing for mfw_nm.
 *
 * *****************  Version 38  *****************
 * User: Vo           Date: 16.02.00   Time: 14:22
 * Updated in $/GSM/Condat/MS/SRC/MFW
 *
 * *****************  Version 37  *****************
 * User: Vo           Date: 11.02.00   Time: 12:57
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * Bug fix: limited service
 *
 * *****************  Version 36  *****************
 * User: Vo           Date: 11.02.00   Time: 11:01
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * add limited service indication
 *
 * *****************  Version 35  *****************
 * User: Vo           Date: 28.01.00   Time: 12:48
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * Limited service
 *
 * *****************  Version 34  *****************
 * User: Vo           Date: 20.01.00   Time: 13:17
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * Add new methodes to save preferred PLMN entry (change interface
 * function nm_change_pref_plmn())
 *
 * *****************  Version 33  *****************
 * User: Vo           Date: 22.12.99   Time: 15:59
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * New: Request of preferred PLMN memory of SIM card
 *
 * *****************  Version 32  *****************
 * User: Vo           Date: 3.12.99    Time: 12:16
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * remove the request of maximum/used preferred PLMN list
 *
 * *****************  Version 31  *****************
 * User: Vo           Date: 2.12.99    Time: 13:28
 * Updated in $/GSM/Condat/MS/SRC/MFW
 *
 * *****************  Version 30  *****************
 * User: Vo           Date: 2.12.99    Time: 13:25
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * New: request of max/used perferred PLMN list
 *
 * *****************  Version 29  *****************
 * User: Vo           Date: 21.10.99   Time: 13:24
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * changed of read/change perferred PLMN list
 * add event for successful de-registration
|
| *****************  Version 28  *****************
| User: Le           Date: 8.10.99    Time: 13:38
| Updated in $/GSM/Condat/MS/SRC/MFW
 *
 * *****************  Version 27  *****************
 * User: Es           Date: 16.09.99   Time: 16:56
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * reordered include statements due to addons in mfw_phb.h
 *
 * *****************  Version 26  *****************
 * User: Vo           Date: 13.09.99   Time: 11:39
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * Bug fix: registration with numeric network format
 *
 * *****************  Version 25  *****************
 * User: Vo           Date: 30.08.99   Time: 14:03
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * bug fix
 * new function
 *
 * *****************  Version 24  *****************
 * User: Vo           Date: 2.07.99    Time: 18:16
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 23  *****************
 * User: Vo           Date: 20.05.99   Time: 17:39
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 22  *****************
 * User: Vo           Date: 28.04.99   Time: 16:11
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 21  *****************
 * User: Vo           Date: 31.03.99   Time: 14:09
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 20  *****************
 * User: Es           Date: 15.03.99   Time: 18:21
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * recode
 *
 * *****************  Version 19  *****************
 * User: Vo           Date: 17.02.99   Time: 19:03
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 18  *****************
 * User: Vo           Date: 12.02.99   Time: 18:59
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 17  *****************
 * User: Vo           Date: 9.02.99    Time: 14:54
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 16  *****************
 * User: Vo           Date: 12.01.99   Time: 14:55
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 15  *****************
 * User: Es           Date: 23.12.98   Time: 15:47
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 14  *****************
 * User: Es           Date: 18.12.98   Time: 13:04
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 13  *****************
 * User: Es           Date: 8.12.98    Time: 16:53
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 11  *****************
 * User: Vo           Date: 1.12.98    Time: 15:43
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 10  *****************
 * User: Vo           Date: 30.11.98   Time: 17:49
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 9  *****************
 * User: Vo           Date: 24.11.98   Time: 21:40
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * first successful compilation of MFW
 *
 * *****************  Version 8  *****************
 * User: Vo           Date: 13.11.98   Time: 16:21
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 7  *****************
 * User: Es           Date: 12.11.98   Time: 13:50
 * Updated in $/GSM/DEV/MS/SRC/MFW
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

#include "prim.h"

#include "mfw_mfw.h"
#include "mfw_nm.h"
#include "mfw_sim.h"
#include "mfw_sima.h"
#include "mfw_nmi.h"
#include "mfw_simi.h"
#include "mfw_nma.h"
#include "mfw_acie.h"
#include "mfw_phb.h"
#include "mfw_cm.h"
#include "mfw_sms.h"
#include "mfw_win.h"

// June 02, 2006    DR: OMAPS00078005 x0021334
#ifdef FF_TIMEZONE
#include "mfw_td.h"
#endif

#include "ksd.h"
#include "psa.h"

#include "mfw_ss.h"
#include "mfw_ssi.h"

#if defined (FAX_AND_DATA)
#include "aci_fd.h"
#endif

#include "mfw_ffs.h"

#include "message.h"
#include "prim.h"
#include "aci_cmh.h"	//GW 22-11-01 - Added

#ifdef SIM_TOOLKIT
#include "mfw_sat.h"
#endif

#ifndef NEPTUNE_BOARD

#ifdef TI_PS_OP_OPN_TAB_ROMBASED
/* Changes for ROM data */ 
/* Operator List size in ROM. Should go to where the decompression algo is present*/
#define PLMN_OPER_LST_SIZE_ROM 628
/* Changes for ROM data 
extern const T_OPER_NTRY_FIXED operListFixed[]; 
 Side access into cus_aci.h 
 Above decl no more required */
EXTERN const UBYTE *ptr_plmn_compressed;
EXTERN SHORT cmhMM_decodePlmn (T_OPER_ENTRY *oper, const UBYTE *plmn_comp_entry);
EXTERN USHORT cmhMM_PlmnEntryLength (const UBYTE *plmn_comp_entry);
#else
extern const T_OPER_NTRY_FIXED operListFixed[]; /* Side access into cus_aci.h */
#endif

#else
extern const T_OPER_NTRY operList[];
#endif
EXTERN MfwHdr * current_mfw_elem;

U8 mfw_IMSI [LONG_NAME];            /* expanded IMSI (ASCIIZ)              */

static U8 last_selected_mode = NOT_PRESENT_8BIT;
static U8 reg_stat;
static U8 reg_flag;
#ifdef NO_ASCIIZ
static T_MFW_SP_NAME service_provider_name;
#else
static U8 service_provider_name [LONG_NAME];
#endif
static U8 display_condition;
static T_MFW_PREF_PLMN_LIST pplmn_list;
static T_MFW_NET_IND net_ind;
static T_MFW_TIME_IND tim_ind;
static UBYTE mfwPplmnReq;
static BOOL   sat_update = FALSE;
//    Sep 23, 2004        REF: CRR 24562  xvilliva
int g_ensErr = 0;

/*SPR#2122 - DS - Commented out. Now operator list is dynamically allocated in nm_avail_plmn_list()
  *static T_ACI_COPS_OPDESC oper_list [MAX_OPER];
  */

static int nmCommand (U32 cmd, void *h); /* handle window commands  */
static void nm_search_reg_status(void);

U8 pplmnCnt;
U8 pplmnCmd;

static int pSlot;                       /* primitive handler slot   */

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM             |
| STATE   : code                        ROUTINE : nm_init            |
+--------------------------------------------------------------------+

  PURPOSE : Initialize network manager

*/
int nm_init (void)
{

    TRACE_FUNCTION("nm_init()");

    if (last_selected_mode == NOT_PRESENT_8BIT)
    {
            last_selected_mode = FFS_flashData.PLMN_selection_mode;
    }
                                        /* install prim handler     */
    pSlot = aci_create(nma_response_cb,NULL);

    reg_flag = 0;
    mfwPplmnReq = 0;
    reg_stat = NOT_PRESENT_8BIT;
    sat_update  = FALSE;
    mfwCommand[MfwTypNm] = (MfwCb) nmCommand;

    return last_selected_mode;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM             |
| STATE   : code                        ROUTINE : nm_exit            |
+--------------------------------------------------------------------+

  PURPOSE : Finalize network manager

*/

void nm_exit (void)
{
                                        /* remove prim handler      */
    aci_delete(pSlot);
    last_selected_mode = NOT_PRESENT_8BIT;
    reg_stat = NOT_PRESENT_8BIT;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM             |
| STATE   : code                        ROUTINE : nm_create          |
+--------------------------------------------------------------------+

  PURPOSE : Create network manager

*/

MfwHnd nm_create (MfwHnd hWin, MfwEvt event, MfwCb cbfunc)
{
    MfwHdr *hdr;
    T_MFW_NM *nm_para;
	MfwHdr *insert_status =0;
	
    TRACE_FUNCTION("nm_create()");

    hdr = (MfwHdr *) mfwAlloc(sizeof(MfwHdr));
    nm_para = (T_MFW_NM *) mfwAlloc(sizeof(T_MFW_NM));

    if (!hdr || !nm_para)
    {
    	TRACE_ERROR("ERROR: nm_create() Mem Alloc Failed.");
			
	   	if(hdr)
   			mfwFree((U8*)hdr,sizeof(MfwHdr));
   		if(nm_para)
   			mfwFree((U8*)nm_para,sizeof(T_MFW_NM));	
   		
	   	return FALSE;
  	}

    nm_para->emask = event;
    nm_para->handler = cbfunc;

    hdr->data = nm_para;                /* store parameter in node  */
    hdr->type = MfwTypNm;               /* type of event handler    */

    insert_status = mfwInsert((MfwHdr *) hWin,hdr);
    if(!insert_status)
	{
  		TRACE_ERROR("ERROR: nm_create() Failed to Install Handler. ");
   		mfwFree((U8*)hdr,sizeof(MfwHdr));
   		mfwFree((U8*)nm_para ,sizeof(T_MFW_NM));
		return 0;
  	}
    return insert_status;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM             |
| STATE   : code                        ROUTINE : nm_delete          |
+--------------------------------------------------------------------+

  PURPOSE : delete network manager

*/

MfwRes nm_delete (MfwHnd h)
{
    TRACE_FUNCTION("nm_delete()");

    if (!h || !((MfwHdr *) h)->data)
        return MfwResIllHnd;

    if (!mfwRemove((MfwHdr *) h))
        return MfwResIllHnd;

    mfwFree(((MfwHdr *) h)->data,sizeof(T_MFW_NM));
    mfwFree(h,sizeof(MfwHdr));

    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM             |
| STATE   : code                        ROUTINE : nm_sign_exec       |
+--------------------------------------------------------------------+

  PURPOSE : dispatch network management signal

*/

static int nm_sign_exec (MfwHdr *cur_elem, MfwEvt event,
                                        T_MFW_NM_PARA * para)
{
    TRACE_FUNCTION("nm_sign_exec()");

    while (cur_elem)
    {
        if (cur_elem->type == MfwTypNm)
        {
            T_MFW_NM *nm_data;

            nm_data = (T_MFW_NM *) cur_elem->data;
            if (nm_data->emask & event)
            {
                nm_data->event = event;
                switch (event)
                {
                    case E_NM_FULL_SERVICE:
		//x0pleela 9 Feb, 2006  ER:OMAPS00065203
		//Handling E_NM_OPN for operator name display
                    case E_NM_OPN:
                        memcpy(&nm_data->para.ident,para,
                                    sizeof(T_MFW_NETWORK_STRUCT));
                        break;
                    case E_NM_PLMN_LIST:
                        memcpy(&nm_data->para.plmns,para,
                                    sizeof(T_MFW_PLMN_LIST));
                        break;
                    case E_NM_PREF_PLMN_LIST:
                        memcpy(&nm_data->para.pplmns,para,
                                    sizeof(T_MFW_PREF_PLMN_LIST));
                        break;
                    case E_NM_PPLMN_MEM:
                        memcpy(&nm_data->para.pplmn_mem,para,
                                    sizeof(T_MFW_PPLMN_MEM));
                        break;
                    case E_NM_CELL_SELECT:
                    	memcpy(&nm_data->para.cell_info,para,
                                    sizeof(T_MFW_CELL_STRUCT));
                    	break;
                    case E_MFW_NET_IND:
                        memcpy(&nm_data->para.net_info,para,
                                    sizeof(T_MFW_NET_IND));
                        break;
                    case E_MFW_TIME_IND:
                        memcpy(&nm_data->para.tim_info,para,
                                    sizeof(T_MFW_TIME_IND));
                        break;
                 }
                if (nm_data->handler)
                {
                  // PATCH LE 06.06.00
                  // store current mfw elem
                  current_mfw_elem = cur_elem;
                  // END PATCH LE 06.06.00
                  if ((*(nm_data->handler)) (nm_data->event,
                                            (void *) &nm_data->para))
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
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM             |
| STATE   : code                        ROUTINE : nm_signal          |
+--------------------------------------------------------------------+

  PURPOSE : Send a signal

*/

static void nm_signal (MfwEvt event, void *para)
{
  UBYTE temp=0;
    TRACE_FUNCTION("nm_signal()");

#ifndef FF_POWER_MANAGEMENT
  temp = dspl_Enable(0);
#else
  /*OMAPS00098881 (removing power variant) a0393213(Prabakar)
  display is enabled only when lcd refresh is needed*/
  if(FFS_flashData.refresh == 1)
	temp = dspl_Enable(0);
#endif
  if (mfwSignallingMethod EQ 0)
  {
    if (mfwFocus)
        if (nm_sign_exec(mfwFocus,event,para))
    {
	#ifndef FF_POWER_MANAGEMENT
		dspl_Enable(temp);            
	#else
	       /*OMAPS00098881 (removing power variant) a0393213(Prabakar)
               display is enabled only when lcd refresh is needed*/
	      if(FFS_flashData.refresh == 1)
			dspl_Enable(temp);
	#endif
      return;
    }
    if (mfwRoot)
        nm_sign_exec(mfwRoot,event,para);
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
        if (nm_sign_exec (h, event, para))
    {
	#ifndef FF_POWER_MANAGEMENT
		dspl_Enable(temp);            
	#else
	       /*OMAPS00098881 (removing power variant) a0393213(Prabakar)
               display is enabled only when lcd refresh is needed*/
	      if(FFS_flashData.refresh == 1)
			dspl_Enable(temp);
	#endif
      return;
    }

        /*
         * All windows tried inclusive root
         */
        if (h == mfwRoot)
    {
	#ifndef FF_POWER_MANAGEMENT
		dspl_Enable(temp);            
	#else
	       /*OMAPS00098881 (removing power variant) a0393213(Prabakar)
               display is enabled only when lcd refresh is needed*/
	      if(FFS_flashData.refresh == 1)
			dspl_Enable(temp);
	#endif
      return;
    }

        /*
         * get parent window
         */
        h = mfwParent(mfwParent(h));
    if(h)
      h = ((MfwWin * )(h->data))->elems;
      }
      nm_sign_exec (mfwRoot, event, para);
    }
   #ifndef FF_POWER_MANAGEMENT
   	dspl_Enable(temp);            
   #else
          /*OMAPS00098881 (removing power variant) a0393213(Prabakar)
              display is enabled only when lcd refresh is needed*/
         if(FFS_flashData.refresh == 1)
   		dspl_Enable(temp);
   #endif
}



/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM             |
| STATE   : code                        ROUTINE : nm_set_mode        |
+--------------------------------------------------------------------+

  PURPOSE : Change registration mode

*/

void nm_set_mode (U8 mode)
{

    TRACE_FUNCTION("nm_set_mode()");

    if (mode != NM_AUTOMATIC && mode != NM_MANUAL)
        return;

    if (mode != last_selected_mode)
    {
        last_selected_mode = mode;
        {
            FFS_flashData.PLMN_selection_mode = mode;
            flash_write();
        }
    }
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM             |
| STATE   : code                        ROUTINE : nm_get_mode        |
+--------------------------------------------------------------------+

  PURPOSE : Get registration mode

*/

T_MFW nm_get_mode ()
{
    return last_selected_mode;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM             |
| STATE   : code                        ROUTINE : nm_activate        |
+--------------------------------------------------------------------+

  PURPOSE : Notify about SIM card activation

*/

void nm_activate (U8 result)
{
  //TRACE_FUNCTION("nm_activate()");

}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM             |
| STATE   : code                        ROUTINE : nm_reg_flag_req    |
+--------------------------------------------------------------------+

  PURPOSE : Notify about SIM card activation

*/

T_MFW nm_reg_flag_req()
{
  return reg_flag;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM             |
| STATE   : code                        ROUTINE : nm_mmi_parameters  |
+--------------------------------------------------------------------+

  PURPOSE : Notify about relevant parameters

*/

#ifdef FF_2TO1_PS
void nm_mmi_parameters (T_SIM_imsi_field *imsi, T_SIM_pref_plmn *plmn)
{
    void simDecodeIMSI (U8 *ci, U8 li, U8 *di);

    TRACE_FUNCTION("nm_mmi_parameters()");

    simDecodeIMSI(imsi->field,imsi->c_field,mfw_IMSI);
}
#else
void nm_mmi_parameters (T_imsi_field *imsi)
{
    void simDecodeIMSI (U8 *ci, U8 li, U8 *di);

    TRACE_FUNCTION("nm_mmi_parameters()");

    simDecodeIMSI(imsi->field,imsi->c_field,mfw_IMSI);
}
#endif


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM             |
| STATE   : code                        ROUTINE : nm_pref_plmn_list  |
+--------------------------------------------------------------------+

  PURPOSE : Request preferred PLMN list

*/

void nm_pref_plmn_list(T_MFW_NETWORK_STRUCT *pplmn, UBYTE count, SHORT start)
{
  SHORT lastIdx;
  T_ACI_CPOL_OPDESC operLst[MAX_OPER];
  T_ACI_RETURN res;
  UBYTE i, j;

  TRACE_FUNCTION("nm_pref_plmn_list()");

  pplmn_list.count = 0;
  pplmn_list.plmn = pplmn;
  pplmnCnt = count;

  j = 0;
  while (1)
  {
    res = qAT_PlusCPOL(CMD_SRC_LCL,
                       start,
                       &lastIdx,
                       operLst,
                       CPOL_MOD_CompactList);
    if (res == AT_EXCT)
      return;

    if (res == AT_CMPL)
    {
      for (i=0; i<MAX_OPER; i++)
      {
        if (operLst[i].index == -1 OR pplmn_list.count >= pplmnCnt)
          break;

        pplmn_list.plmn[j].index = operLst[i].index;
#ifdef NO_ASCIIZ
        sAT_PlusCOPSE((UBYTE *)operLst[i].oper,
                      (UBYTE)operLst[i].format,
                      &pplmn_list.plmn[j].network_long,
                      &pplmn_list.plmn[j].network_short,
                      pplmn_list.plmn[j].network_numeric);
#else
        sAT_PlusCOPSE((UBYTE *)operLst[i].oper,
                      (UBYTE)operLst[i].format,
                      pplmn_list.plmn[j].network_long,
                      pplmn_list.plmn[j].network_short,
                      pplmn_list.plmn[j].network_numeric);
#endif
        pplmn_list.count++;
        j++;
      }
      if (i < MAX_OPER OR (j EQ pplmnCnt))
      {
        nm_signal(E_NM_PREF_PLMN_LIST, &pplmn_list);
        return;
      }
      start = lastIdx + 1;
    }
    else
    {
      nm_signal(E_NM_PREF_PLMN_LIST, &pplmn_list);
      return;
    }
  }
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM             |
| STATE   : code                        ROUTINE : rAT_PlusCPOL       |
+--------------------------------------------------------------------+

  PURPOSE :  callback of preferred PLMN list

*/

void rAT_PlusCPOL(SHORT startIdx, SHORT lastIdx,
                  T_ACI_CPOL_OPDESC *operLst,
                  SHORT              usdNtry)
{
  UBYTE i, j;
  SHORT endIdx;
  T_MFW_PPLMN_MEM pplmn_mem;

  TRACE_FUNCTION("nm_pref_plmn_list()");

  if (mfwPplmnReq)
  {
    mfwPplmnReq = 0;
    pplmn_mem.maxRcd = (UBYTE)lastIdx;
    pplmn_mem.usedRcd = (UBYTE)usdNtry;
    nm_signal(E_NM_PPLMN_MEM, &pplmn_mem);
    return;
  }

  endIdx = lastIdx;
  j = 0;
  while (1)
  {
    for (i=0; i<MAX_OPER; i++)
    {
      if (operLst[i].index == -1 OR pplmn_list.count >= pplmnCnt)
          break;

        pplmn_list.plmn[j].index = operLst[i].index;
#ifdef NO_ASCIIZ
        sAT_PlusCOPSE((UBYTE *)operLst[i].oper,
                      (UBYTE)operLst[i].format,
                      &pplmn_list.plmn[j].network_long,
                      &pplmn_list.plmn[j].network_short,
                      pplmn_list.plmn[j].network_numeric);
#else
        sAT_PlusCOPSE((UBYTE *)operLst[i].oper,
                      (UBYTE)operLst[i].format,
                      pplmn_list.plmn[j].network_long,
                      pplmn_list.plmn[j].network_short,
                      pplmn_list.plmn[j].network_numeric);
#endif
        pplmn_list.count++;
        j++;
    }
    if ((i<MAX_OPER) OR (j EQ pplmnCnt))
    {
      nm_signal(E_NM_PREF_PLMN_LIST, &pplmn_list);
      return;
    }
    startIdx = endIdx + 1;
    if (qAT_PlusCPOL(CMD_SRC_LCL, startIdx, &endIdx,
                     operLst, CPOL_MOD_CompactList) != AT_CMPL)
    {
      nm_signal(E_NM_PREF_PLMN_LIST, &pplmn_list);
      return;
    }
  }
}


/*
+----------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)       MODULE  : MFW_NM                |
| STATE   : code                       ROUTINE : nm_change_pref_plmn   |
+----------------------------------------------------------------------+

  PURPOSE : Save/Change/Delete a preferred PLMN

*/

T_MFW_NM_RETURN nm_change_pref_plmn(T_MFW_PPLMN_MODE mode,
                                    SHORT index, SHORT index2,
                                    UBYTE *network_numeric)
{
    SHORT           idx;
    T_ACI_CPOL_FRMT format;
    CHAR          * oper;
    SHORT           idx2;
    T_ACI_CPOL_MOD  md;

    TRACE_FUNCTION("nm_change_pref_plmn()");

    switch (mode)
    {
        case NEW_PREF_PLMN:
            idx = -1;
            idx2 = -1;
            format = CPOL_FRMT_Numeric;
            oper = (CHAR *)network_numeric;
            md = CPOL_MOD_CompactList;
            break;
        case REPLACE_PREF_PLMN:
            idx = index;
            idx2 = -1;
            format = CPOL_FRMT_Numeric;
            oper = (CHAR *)network_numeric;
            md = CPOL_MOD_CompactList;
            break;
        case DELETE_PREF_PLMN:
            idx = index;
            idx2 = -1;
            format = CPOL_FRMT_NotPresent;
            oper = NULL;
            md = CPOL_MOD_CompactList;
            break;
        case INSERT_PREF_PLMN:
            idx = index;
            idx2 = -1;
            format = CPOL_FRMT_Numeric;
            oper = (CHAR *)network_numeric;
            md = CPOL_MOD_Insert;
            break;
        case CHANGE_PREF_PLMN:
            idx = index;
            idx2 = index2;
            format = CPOL_FRMT_Numeric;
            oper = (CHAR *)network_numeric;
            md = CPOL_MOD_CompactList;
            break;
        default:
            return NM_ERR;
    }

    if (sAT_PlusCPOL(CMD_SRC_LCL, idx, format,
                     oper, idx2, md) != AT_EXCT)
        return NM_ERR;
    pplmnCmd = CHANGE_PREF_PLMN;
    return NM_OK;
}


/*
+----------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)       MODULE  : MFW_NM                |
| STATE   : code                       ROUTINE : nm_change_pref_plmn_longname   |
+----------------------------------------------------------------------+

  PURPOSE : Change a preferred PLMN using longname. added for SPR 2635.

*/

T_MFW_NM_RETURN nm_change_pref_plmn_longname(T_MFW_PPLMN_MODE mode,
                                    SHORT index, SHORT index2,
                                    char *network_long)
{
    SHORT           idx;
    T_ACI_CPOL_FRMT format;
    CHAR          * oper;
    SHORT           idx2;
    T_ACI_CPOL_MOD  md;

    TRACE_FUNCTION("nm_change_pref_plmn()");

    switch (mode)
    {
     
        case REPLACE_PREF_PLMN:
            idx = index;
            idx2 = -1;
            format = CPOL_FRMT_Long;
            oper = (CHAR *)network_long;
            md = CPOL_MOD_CompactList;
            break;

        default:
            return NM_ERR;
    }

    if (sAT_PlusCPOL(CMD_SRC_LCL, idx, format,
                     oper, idx2, md) != AT_EXCT)
        return NM_ERR;
    pplmnCmd = CHANGE_PREF_PLMN;
    return NM_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM             |
| STATE   : code                        ROUTINE : nm_ok_pref_plmn    |
+--------------------------------------------------------------------+


  PURPOSE : The change of preferred PLMN is successful.

*/

void nm_ok_pref_plmn(void)
{
    TRACE_FUNCTION("nm_ok_pref_plmn()");

    if (pplmnCmd == CHANGE_PREF_PLMN)
    {
        pplmnCmd = CMD_NOT_PRESENT;
        nm_signal(E_NM_CHANGE_PPLMN_OK, 0);
    }
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM             |
| STATE   : code                        ROUTINE : nm_error_pref_plmn |
+--------------------------------------------------------------------+


  PURPOSE : The change of preferred PLMN is failed.

*/

void nm_error_pref_plmn(void)
{
    TRACE_FUNCTION("nm_error_pref_plmn()");

    if (pplmnCmd == CHANGE_PREF_PLMN)
    {
        pplmnCmd = CMD_NOT_PRESENT;
        nm_signal(E_NM_CHANGE_PPLMN_ERR, 0);
        return;
    }
    if (mfwPplmnReq)
      mfwPplmnReq = 0;
    pplmn_list.count = 0;
    nm_signal(E_NM_PREF_PLMN_LIST, &pplmn_list);
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM             |
| STATE   : code                        ROUTINE : nm_copy_plmn_list  |
+--------------------------------------------------------------------+


  PURPOSE : Write the PLMN information in PLMN list

*/

void nm_copy_plmn_list (T_MFW_NETWORK_STRUCT *plmn, T_ACI_COPS_OPDESC *operLst)
{
    U8 *forbidden_id;
    U8 *fieldstrength;

    TRACE_FUNCTION("nm_copy_plmn_list()");
#ifdef NO_ASCIIZ
    plmn->network_long.len = 0;
    plmn->network_short.len = 0;
    memset(plmn->network_numeric, 0, NUMERIC_LENGTH);

    if (operLst->longOper)
    {
 	  USHORT length=strlen((const char *)operLst->longOper);
        plmn->network_long.len = MINIMUM(LONG_NAME-1, length);        
        memcpy (plmn->network_long.data, operLst->longOper, plmn->network_long.len);
        plmn->network_long.data[plmn->network_long.len] = '\0';
        plmn->network_long.dcs = MFW_ASCII;
    }

    if (operLst->shortOper)
    {
     	  USHORT length=strlen((const char *)operLst->shortOper);
        plmn->network_short.len = MINIMUM(SHORT_NAME-1, length);            
        memcpy (plmn->network_short.data, operLst->shortOper, plmn->network_short.len);
        plmn->network_short.data[plmn->network_short.len] = '\0';
        plmn->network_short.dcs = MFW_ASCII;
    }
#else
    memset(plmn->network_long, 0, LONG_NAME);
    memset(plmn->network_short, 0, SHORT_NAME);
    memset(plmn->network_numeric, 0, NUMERIC_LENGTH);

    if (operLst->longOper)
        strncpy((char *)plmn->network_long, (char *)operLst->longOper, LONG_NAME-1);

    if (operLst->shortOper)
        strncpy((char *)plmn->network_short, (char *)operLst->shortOper, SHORT_NAME-1);
#endif
    if (operLst->numOper)
        strncpy((char *)plmn->network_numeric, (char *)operLst->numOper, NUMERIC_LENGTH-1);

    if (operLst->status == COPS_STAT_Available
        || operLst->status == COPS_STAT_Current)
        plmn->forbidden_indicator = MFW_PLMN_AVAILABLE;
    else
        plmn->forbidden_indicator = MFW_PLMN_FORBIDDEN;

    if (!strncmp((char *) mfw_IMSI,(char *) plmn->network_numeric,5))
        plmn->roaming_indicator = 0;
    else
        plmn->roaming_indicator = 1;

    sAT_PlusCOPSF(&forbidden_id,&fieldstrength);
    plmn->fieldstrength = fieldstrength;
}


/*
+---------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM              |
| STATE   : code                        ROUTINE : nm_pplmn_mem_req    |
+---------------------------------------------------------------------+

  PURPOSE : Request the maximum number of preferred PLMN records
            in SIM card

*/

T_MFW_NM_RETURN nm_pplmn_mem_req(void)
{
  T_ACI_RETURN res;
  SHORT maxRcd, usedRcd;
  T_MFW_PPLMN_MEM pplmn_mem;

  TRACE_FUNCTION("nm_pplmn_mem_req()");

  res = tAT_PlusCPOL(CMD_SRC_LCL, &maxRcd, &usedRcd);
  if (res == AT_EXCT)
  {
    mfwPplmnReq = 1;
    return NM_OK;
  }
  if (res == AT_CMPL)
  {
    pplmn_mem.maxRcd = (UBYTE)maxRcd;
    pplmn_mem.usedRcd = (UBYTE)usedRcd;
    nm_signal(E_NM_PPLMN_MEM, &pplmn_mem);
    return NM_OK;
  }

  return NM_ERR;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM             |
| STATE   : code                        ROUTINE : nm_registration    |
+--------------------------------------------------------------------+

  PURPOSE : Start registration

*/
/*
T_MFW nm_registration (U8 mode, U8 freq_bands, U8 limited)
{
    U8 sim_stat;

    TRACE_FUNCTION("nm_registration()");

    sAT_Abort(CMD_SRC_LCL, AT_CMD_NRG);

    sim_stat = sim_status_check();
    reg_flag = 1;

    if (limited)
    {
        if (sAT_PercentNRG(CMD_SRC_LCL,
                           NRG_RGMD_Auto,
                           NRG_SVMD_Limited,
                           NRG_FRMT_Numeric,
                           NULL) != AT_EXCT)
            TRACE_EVENT("Limited Service error: SAT_PercentNRG");
        return sim_stat;
    }

// Nov 24, 2005, a0876501, DR: OMAPS00045909
    if (!limited && sim_stat == IMEI_NOT_VALID)
      return IMEI_NOT_VALID;

    if (!limited && sim_stat != SIM_ACTIVE)
      return SIM_NOT_ACTIVE;


    if (mode == NM_MANUAL)
    {
   
 //Apr 13, 2004	REF: CRR 16303	JuanVi Jativa-Villoldo      
 // sim_spn_req();     Moved to the function  rAT_PlusCFUNP           

        // May 15, 2006,  a0393213 (R.Prabakar),  OMAPS00075777  
        //   Description :  It is supposed that the sample register itself automatically (in Manual Mode) 
        //                  to the last manual selected HPLMN cell before the power cycle, and this does not happen; 
         //                 instead this, the sample shows up the available networks list
         //  Fix         :  call to nm_avail_plmn_list() is changed to call to sAT_PercentNRG() 
     	if (sAT_PercentNRG(CMD_SRC_LCL,
                           NRG_RGMD_Manual,
                           NRG_SVMD_Full,
                           NRG_FRMT_Numeric,
                           NULL) != AT_EXCT)
        {
            TRACE_ERROR("sAT_PercentNRG error");
        }
    }
    else
    {
    	if (sAT_PercentNRG(CMD_SRC_LCL,
                           NRG_RGMD_Auto,
                           NRG_SVMD_Full,
                           NRG_FRMT_Numeric,
                           NULL) != AT_EXCT)
        {
            TRACE_ERROR("sAT_PercentNRG error");
        }
   
 //Apr 13, 2004	REF: CRR 16303	JuanVi Jativa-Villoldo      
 // sim_spn_req();     Moved to the function  rAT_PlusCFUNP           
 //   }
    return SIM_ACTIVE;
}
*/

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM             |
| STATE   : code                        ROUTINE : nm_avail_plmn_list |
+--------------------------------------------------------------------+

  PURPOSE : Request the available PLMN list

  SPR#2122 - DS - Restructured to make oper_list dynamically allocated
*/



void nm_avail_plmn_list ()
{
    T_ACI_RETURN result;
    int i;
    SHORT last_ind = 0;
    T_ACI_COPS_OPDESC* oper_list = NULL;
	//Mar 01, 2005    REF: ENH 29327   xdeepadh
	//Dynamically allocate the plmn_list.
    T_MFW_PLMN_LIST* plmn_list;
	plmn_list = (T_MFW_PLMN_LIST*)mfwAlloc(sizeof(T_MFW_PLMN_LIST));
	if(plmn_list == NULL)
	{
		TRACE_EVENT("plmn_list malloc failed");
		return ;
	}	

    TRACE_FUNCTION("nm_avail_plmn_list()");

    /* Dynamically allocate operator list buffer */
    oper_list = (T_ACI_COPS_OPDESC*)mfwAlloc(sizeof(T_ACI_COPS_OPDESC)*MAX_OPER);

    if (oper_list == NULL)
    {
        TRACE_ERROR("ERROR: Failed to allocate oper list buffer");
	//Mar 01, 2005    REF: ENH 29327   xdeepadh
	//Deallocate the plmn_list
	if(plmn_list !=NULL)
	{
		mfwFree((U8*)plmn_list,sizeof(T_MFW_PLMN_LIST));
		plmn_list=NULL;
	}
        return;
    }
	

// PATCH JVU - PLMN selection during registration
    sAT_Abort(CMD_SRC_LCL, AT_CMD_NRG);
// END PATCH JVU - PLMN selection during registration

    plmn_list->count = 0;
    result = tAT_PlusCOPS(CMD_SRC_LCL,0,&last_ind,oper_list);
    if (result == AT_FAIL || result == AT_BUSY)
    {
        TRACE_ERROR("ERROR: tAT_PlusCOPS error");
		 //Sep 23, 2004        REF: CRR 24562  xvilliva
		 //If AT_FAIL is returned, we query for the exact error
		 //and set a variable accordingly. 
		 if(result == AT_FAIL)
		 {
				ULONG err;

				err = qAT_ErrDesc();
				switch (ACI_ERR_DESC_CLASS( err ))
				{
				case ACI_ERR_CLASS_Cme:
						if (ACI_ERR_DESC_NR( err ) == CME_ERR_OpNotAllow)
						{
							g_ensErr = 1;						
						}
					break;
				}
		 }
		
		/* Deallocate operator list buffer */
        if (oper_list != NULL)
        {
            mfwFree((U8*)oper_list, sizeof(T_ACI_COPS_OPDESC)*MAX_OPER);
            oper_list = NULL;
        }
		
        nm_signal(E_NM_PLMN_LIST,plmn_list);
		//Mar 01, 2005    REF: ENH 29327   xdeepadh
		//Deallocate the plmn_list
		if(plmn_list !=NULL)
		{
			mfwFree((U8*)plmn_list,sizeof(T_MFW_PLMN_LIST));
			plmn_list=NULL;
		}
        return;
    }
    if (result == AT_EXCT)
    {
         /* Deallocate operator list buffer */
        if (oper_list != NULL)
        {
            mfwFree((U8*)oper_list, sizeof(T_ACI_COPS_OPDESC)*MAX_OPER);
            oper_list = NULL;
        }
	//Mar 01, 2005    REF: ENH 29327   xdeepadh
	//Deallocate the plmn_list
	if(plmn_list !=NULL)
	{
		mfwFree((U8*)plmn_list,sizeof(T_MFW_PLMN_LIST));
		plmn_list=NULL;
	}
        return;                     /* wait for rAT_PlusCOPS()  */
    }

    if (last_ind > MAX_OPER)
        last_ind = MAX_OPER;

    for (i = 0; i <= last_ind; i++)
        nm_copy_plmn_list(&plmn_list->plmn[i], &oper_list[i]);

    /* Deallocate operator list buffer */
    if (oper_list != NULL)
    {
        mfwFree((U8*)oper_list, sizeof(T_ACI_COPS_OPDESC)*MAX_OPER);
        oper_list = NULL;
    }

	plmn_list->count = i;
	nm_signal(E_NM_PLMN_LIST,plmn_list);
	//Mar 01, 2005    REF: ENH 29327   xdeepadh
	//Deallocate the plmn_list
	if(plmn_list !=NULL)
	{
		mfwFree((U8*)plmn_list,sizeof(T_MFW_PLMN_LIST));
		plmn_list=NULL;
	}
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM             |
| STATE   : code                        ROUTINE : nm_home_plmn       |
+--------------------------------------------------------------------+

  PURPOSE : Get the home PLMN name

*/

void nm_home_plmn (T_MFW_NETWORK_STRUCT *hplmn)
{
    U8 oper[6];

    /* get network operator */
    strncpy((char *)oper, (char *)mfw_IMSI, 5);
    oper[5] = 0;

#ifdef NO_ASCIIZ
    sAT_PlusCOPSE(oper,
                  COPS_FRMT_Numeric,
                  &hplmn->network_long,
                  &hplmn->network_short,
                  hplmn->network_numeric);

#else
    sAT_PlusCOPSE(oper,
                  COPS_FRMT_Numeric,
                  hplmn->network_long,
                  hplmn->network_short,
                  hplmn->network_numeric);
#endif
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM             |
| STATE   : code                        ROUTINE : nm_plmn_list       |
+--------------------------------------------------------------------+

  PURPOSE : Request existing network list
  
*/
#ifndef NEPTUNE_BOARD
#ifdef TI_PS_OP_OPN_TAB_ROMBASED
int nm_plnm_list(T_MFW_NETWORK_STRUCT *plmn_list, int start, int count)
{
    int  i;

    /* Changes for ROM data */ 
    const UBYTE *plmn_comp_entry; /* get a local pointer holder */
    T_OPER_ENTRY oper;


    TRACE_FUNCTION("nm_plnm_list()");

    /* 
    PLMN_OPER_LST_SIZE_ROM is size of the PLMN operlist
    */

    if (start >= PLMN_OPER_LST_SIZE_ROM)
        return 0;


    /* Changes for ROM data */ 
    plmn_comp_entry = ptr_plmn_compressed;


    /* skip the first (start-1) number of plmn entries. */
    i=0;
    while (i < start) {
        plmn_comp_entry += cmhMM_PlmnEntryLength (plmn_comp_entry);
        i+=1;
    }

    /* for each pointer in the list plmn_list */
    i=0;
    do 
    {
        /* Get first compressed PLMN entry */
        cmhMM_decodePlmn (&oper, plmn_comp_entry);

        if (oper.longName NEQ NULL)
        {
             plmn_list[i].index = start+1;
#ifdef NO_ASCIIZ
             sAT_PlusCOPSE((UBYTE *)oper.longName,
                          COPS_FRMT_Long,
                          &plmn_list[i].network_long,
                          &plmn_list[i].network_short,
                          plmn_list[i].network_numeric);
#else
             sAT_PlusCOPSE((UBYTE *)oper.longName,
                          COPS_FRMT_Long,
                          plmn_list[i].network_long,
                          plmn_list[i].network_short,
                          plmn_list[i].network_numeric);
#endif
             start++;
        }
        else
        {
#ifdef NO_ASCIIZ
             plmn_list[i].network_long.len = 0;
             plmn_list[i].network_short.len = 0;
             plmn_list[i].network_numeric[0] = 0;
#else
             plmn_list[i].network_long[0] = 0;
             plmn_list[i].network_short[0] = 0;
             plmn_list[i].network_numeric[0] = 0;
#endif
             break;
        }

        /* Next compressed PLMN entry */
        plmn_comp_entry += cmhMM_PlmnEntryLength (plmn_comp_entry);
    
        i+=1;
    } while (i < count);

    return PLMN_OPER_LST_SIZE_ROM;
}

#else

int nm_plnm_list(T_MFW_NETWORK_STRUCT *plmn_list, int start, int count)
{
    int  i;
    int  sum;

    TRACE_FUNCTION("nm_plnm_list()");
    sum = 0;
    while (operListFixed[sum].longName != NULL)
        sum++;

    if (start >= sum)
        return 0;
    for (i=0; i<count; i++)
    {
        if (operListFixed[start].longName != NULL)
        {
          plmn_list[i].index = start+1;
#ifdef NO_ASCIIZ
          sAT_PlusCOPSE((UBYTE *)operListFixed[start].longName,
                          COPS_FRMT_Long,
                          &plmn_list[i].network_long,
                          &plmn_list[i].network_short,
                          plmn_list[i].network_numeric);
#else
           sAT_PlusCOPSE((UBYTE *)operListFixed[start].longName,
                          COPS_FRMT_Long,
                          plmn_list[i].network_long,
                          plmn_list[i].network_short,
                          plmn_list[i].network_numeric);
#endif
            start++;
        }
        else
        {
#ifdef NO_ASCIIZ
            plmn_list[i].network_long.len = 0;
            plmn_list[i].network_short.len = 0;
            plmn_list[i].network_numeric[0] = 0;
#else
            plmn_list[i].network_long[0] = 0;
            plmn_list[i].network_short[0] = 0;
            plmn_list[i].network_numeric[0] = 0;
#endif
            break;
        }
    }
    return sum;
}

#endif

#else
int nm_plnm_list(T_MFW_NETWORK_STRUCT *plmn_list, int start, int count)
{
    int  i;
    int  sum;

    TRACE_FUNCTION("nm_plnm_list()");
    sum = 0;
    while (operList[sum].longName != NULL)
        sum++;

    if (start >= sum)
        return 0;

    for (i=0; i<count; i++)
    {
        if (operList[start].longName != NULL)
        {
          plmn_list[i].index = start+1;
#ifdef NO_ASCIIZ
          sAT_PlusCOPSE((UBYTE *)operList[start].longName,
                          COPS_FRMT_Long,
                          &plmn_list[i].network_long,
                          &plmn_list[i].network_short,
                          plmn_list[i].network_numeric);
#else
           sAT_PlusCOPSE((UBYTE *)operList[start].longName,
                          COPS_FRMT_Long,
                          plmn_list[i].network_long,
                          plmn_list[i].network_short,
                          plmn_list[i].network_numeric);
#endif
            start++;
        }
        else
        {
#ifdef NO_ASCIIZ
            plmn_list[i].network_long.len = 0;
            plmn_list[i].network_short.len = 0;
            plmn_list[i].network_numeric[0] = 0;
#else
            plmn_list[i].network_long[0] = 0;
            plmn_list[i].network_short[0] = 0;
            plmn_list[i].network_numeric[0] = 0;
#endif
            break;
        }
    }
    return sum;
}
#endif
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM             |
| STATE   : code                        ROUTINE : nm_plmn_list_longnames       |
+--------------------------------------------------------------------+

  PURPOSE : Request existing network list longnames.  added for SPR 2635.
  
*/

#ifndef NEPTUNE_BOARD
#ifdef TI_PS_OP_OPN_TAB_ROMBASED
int nm_plnm_list_longnames(char**plmn_ln_list, int start, int count)
{
    int  i;
    /* Changes for ROM data */ 
    const UBYTE *plmn_comp_entry; /* get a local pointer holder */
    T_OPER_ENTRY oper;

	
    TRACE_FUNCTION("nm_plnm_list_longnames()");
    
    /* 
    PLMN_OPER_LST_SIZE_ROM is size of the PLMN operlist
    */

    if (start >= PLMN_OPER_LST_SIZE_ROM)
        return 0;

    /* skip the first (start-1) number of plmn entries. */
    i=0;
    while (i < start) {
        plmn_comp_entry += cmhMM_PlmnEntryLength (plmn_comp_entry);
        i+=1;
    }

    /* for each pointer in the list plmn_list */
    i=0;
    do 
    {
        /* Get first compressed PLMN entry */
        cmhMM_decodePlmn (&oper, plmn_comp_entry);

        if (oper.longName NEQ NULL)
        {
             /*just set pointer to point at longname in ROM in ACI*/
             plmn_ln_list[i] = (char*)oper.longName;
		
             start++;
        }
        else
        {
            plmn_ln_list[i] = NULL;
            break;
        }

        /* Next compressed PLMN entry */
        plmn_comp_entry += cmhMM_PlmnEntryLength (plmn_comp_entry);
    
        i+=1;
    } while (i < count);

    return PLMN_OPER_LST_SIZE_ROM;
}

#else

int nm_plnm_list_longnames(char**plmn_ln_list, int start, int count)
{
    int  i;
    int  sum;
	
    TRACE_FUNCTION("nm_plnm_list_longnames()");
    
    sum = 0;

    /*count the number of longnames*/
    while (operListFixed[sum].longName != NULL)
        sum++;
	/*if we attempt read past the end of the list*/
    if (start >= sum)
        return 0;

	/*for each pointer in the list*/
    for (i=0; i<count; i++)
    {
        if (operListFixed[start].longName != NULL)
        {
          	/*just set pointer to point at longname in ROM in ACI*/
            plmn_ln_list[i] = (char*)operListFixed[start].longName;
		
            start++;
        }
        else
        {
			plmn_ln_list[i] = NULL;
            break;
        }
    }
    return sum;
}
#endif

#else
int nm_plnm_list_longnames(char**plmn_ln_list, int start, int count)
{
    int  i;
    int  sum;
	
    TRACE_FUNCTION("nm_plnm_list_longnames()");
    
    sum = 0;

    /*count the number of longnames*/
    while (operList[sum].longName != NULL)
        sum++;
	/*if we attempt read past the end of the list*/
    if (start >= sum)
        return 0;

	/*for each pointer in the list*/
    for (i=0; i<count; i++)
    {
        if (operList[start].longName != NULL)
        {
          	/*just set pointer to point at longname in ROM in ACI*/
			plmn_ln_list[i] = (char*)operList[start].longName;
		
            start++;
        }
        else
        {
			plmn_ln_list[i] = NULL;
            break;
        }
    }
    return sum;
}
#endif

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM             |
| STATE   : code                        ROUTINE : nm_select_plmn     |
+--------------------------------------------------------------------+

  PURPOSE : Select PLMN for registration

*/

void nm_select_plmn (U8 *network)
{
    T_ACI_RETURN result;
    TRACE_FUNCTION("nm_select_plmn()");

   result = sAT_PercentNRG(CMD_SRC_LCL,
                       NRG_RGMD_Manual,
                       NRG_SVMD_Full,
                       NRG_FRMT_Numeric,
                       (char *) network);
   result = AT_FAIL;
    if (result != AT_EXCT)
    {
        TRACE_ERROR("sAT_PercentNRG error");
		 //Sep 23, 2004        REF: CRR 24562  xvilliva
		 //If AT_FAIL is returned, we query for the exact error
		 //and set a variable accordingly. 
		 if(result == AT_FAIL)	
		 {
				ULONG err;

				err = qAT_ErrDesc();
				switch (ACI_ERR_DESC_CLASS( err ))
				{
				case ACI_ERR_CLASS_Cme:
						if (ACI_ERR_DESC_NR( err ) == CME_ERR_OpNotAllow)
						{
							g_ensErr = 1;						
						}
					break;
				}
		 }
    }
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM             |
| STATE   : code                        ROUTINE : nm_spn_cnf         |
+--------------------------------------------------------------------+

  PURPOSE : Notify about service provider name

*/
/*a0393213 lint warnings removal - type of error changed from SHORT to USHORT*/
void nm_spn_cnf (USHORT error, T_EF_SPN *efspn)
{
    U8 i;

    USHORT simErr;
    TRACE_FUNCTION("nm_spn_cnf()");


#ifdef FF_2TO1_PS
    simErr = CAUSE_SIM_NO_ERROR;
#else
    simErr = SIM_NO_ERROR;
#endif

#ifdef NO_ASCIIZ
    memset (service_provider_name.data, 0, sizeof(service_provider_name.data));
    service_provider_name.len = 0;
    
    if (error EQ simErr)
    {
        for (i=0; (efspn->service_provider[i] != 0xFF) && (i < SP_NAME); i++)
            service_provider_name.data[i] = efspn->service_provider[i];

        service_provider_name.len = i;
    }
#else
    memset (service_provider_name, 0, sizeof(service_provider_name));

    if (error EQ simErr)
    {
        for (i=0; (efspn->service_provider[i] != 0xFF) && (i < SP_NAME-1); i++)
            service_provider_name[i] = efspn->service_provider[i];
    }
#endif
    display_condition = efspn->display_condition & 0x01;

#ifdef SIM_TOOLKIT
  if (sat_update)
  {
    sat_update = FALSE;
    satUpdateFiles ( TRUE, SIM_SPN );
  }
#endif
}

#ifdef SIM_TOOLKIT
/*
+---------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)   MODULE  : MFW_SIM                  |
| STATE   : code                   ROUTINE : nm_sat_file_update       |
+---------------------------------------------------------------------+

  PURPOSE : SIM file change indication

*/

void nm_sat_file_update(USHORT dataId)
{
  /*a0393213 compiler warnings removal - removed serv_stat*/

  TRACE_FUNCTION("nm_sat_file_update()");

  switch (dataId)
  {
    case SIM_SPN:
      if (sim_serv_table_check(17) NEQ NO_ALLOCATED)
      {
        sat_update = TRUE;
        sim_read_sim(SIM_SPN, NOT_PRESENT_8BIT, 17);
      }
      else
        satUpdateFiles ( TRUE, SIM_SPN );
      break;
    default:
      break;
  }
}
#endif


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM             |
| STATE   : code                        ROUTINE : nm_deregistration  |
+--------------------------------------------------------------------+

  PURPOSE : Delete registration

*/
/*
void nm_deregistration (void)
{
    TRACE_FUNCTION ("nm_deregistration()");

    if (sAT_PlusCFUN(CMD_SRC_LCL,CFUN_FUN_Minimum,CFUN_RST_NotPresent)
        != AT_EXCT)
    {
        TRACE_ERROR("sAT_PlusCFUN error");
    }
}
*/

/*
+-----------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM                |
| STATE   : code                        ROUTINE : nm_ok_deregistration  |
+-----------------------------------------------------------------------+

  PURPOSE : de-registration successful

*/

void nm_ok_deregistration (void)
{
    TRACE_FUNCTION ("nm_ok_deregistration()");
    nm_signal(E_NM_DEREGISTRATION, 0);
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM             |
| STATE   : code                        ROUTINE : nm_ok_cops         |
+--------------------------------------------------------------------+

  PURPOSE : Successful end of operator selection

*/

void nm_ok_cops (void)
{
  TRACE_FUNCTION("nm_ok_cops()");

  nm_search_reg_status();
  reg_flag = 0;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM             |
| STATE   : code                        ROUTINE : nm_ok_cops         |
+--------------------------------------------------------------------+

  PURPOSE : Successful selection of band
  SPR919 - SH - added this function.
*/

void nm_ok_band (void)
{
  TRACE_FUNCTION("nm_ok_band()");
  
  nm_signal(E_NM_BAND, 0);
}



/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM             |
| STATE   : code                        ROUTINE : nm_error_cops      |
+--------------------------------------------------------------------+

  PURPOSE : error end of operator selection

*/
//x0pleela 30 May, 2007  DR: OMAPS00133201
//Passing T_ACI_CME_ERR as parameter
void nm_error_cops ( T_ACI_CME_ERR err)
{
    TRACE_FUNCTION("nm_error_cops()");

    //x0pleela 30 May, 2007  DR: OMAPS00133201
    if( err == CME_ERR_AbortedByNetwork )
	nm_signal(E_NM_ABORT_NETWORK, 0);
    else
    nm_search_reg_status();
    reg_flag = 0;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM             |
| STATE   : code                        ROUTINE : nm_reg_status      |
+--------------------------------------------------------------------+

  PURPOSE : Check the registration status

*/
/*
T_MFW nm_reg_status (void)
{
    TRACE_FUNCTION("nm_reg_status()");

    return reg_stat;
}
*/

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM             |
| STATE   : code                        ROUTINE : rAT_PlusCOPS       |
+--------------------------------------------------------------------+

  PURPOSE : Notify about available PLMN list
  NOTE    : oper_list defines as char *
            Every operator (PLMN) uses 3 bytes
            
// July 14, 2005  REF: CRR 31916  x0021334
// The functionality in rAT_PlusCOPS has been moved to new function 'nm_Plus_Percent_COPS()'
     which is common for rAT_PercentCOPS as well.
            
//SPR#0777 - DS - Added check to make sure last_ind is valid. If invalid set to 0.
//			       - Changed check to make last_ind equal to MAX_OPER-1 if last_ind is greater than MAX_OPER.
//				- Changed <= to < in nm_copy_plmn_list for loop.

*/

void rAT_PlusCOPS (S16 last_ind, T_ACI_COPS_OPDESC *operLst)
{
	TRACE_FUNCTION ("rAT_PlusCOPS()");

	nm_Plus_Percent_COPS(last_ind, operLst);
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM                                |
| STATE   : code                                     ROUTINE : rAT_PercentCOPS                   |
+--------------------------------------------------------------------+

  PURPOSE : Notify about available PLMN list. Call is made to nm_Plus_Percent_COPS().

// July 14, 2005  REF: CRR 31916  x0021334
// New function 'rAT_PercentCOPS()' implemented.
                  
*/

void rAT_PercentCOPS (S16 last_ind, T_ACI_COPS_OPDESC *operLst)
{
	TRACE_FUNCTION ("rAT_PercentCOPS()");

	nm_Plus_Percent_COPS(last_ind, operLst);
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM                                |
| STATE   : code                                     ROUTINE : nm_Plus_Percent_COPS          |
+--------------------------------------------------------------------+

  PURPOSE : Notify about available PLMN list. Common function called from rAT_PlusCOPS
  		     and rAT_PercentCOPS

// July 14, 2005  REF: CRR 31916  x0021334
// New function 'nm_Plus_Percent_COPS()' implemented.

*/

void nm_Plus_Percent_COPS (S16 last_ind, T_ACI_COPS_OPDESC *operLst)
{
    	int i;
	//Mar 01, 2005    REF: ENH 29327   xdeepadh
	//Dynamically allocate the plmn_list
    	T_MFW_PLMN_LIST *plmn_list;
	/* Aug 09, 2007 REF: OMAPS00140507 x0045876 
	   Added patch given by Dong Feng */	   
	char longName[MAX_ALPHA_OPER_LEN] = { '\0' };
  	char shrtName[MAX_ALPHA_OPER_LEN] = { '\0' };
	plmn_list=(T_MFW_PLMN_LIST*)mfwAlloc(sizeof(T_MFW_PLMN_LIST));
	if(plmn_list==NULL)
	{
		TRACE_EVENT("plmn_list malloc failed");
		return;
	}
	TRACE_FUNCTION ("nm_Plus_Percent_COPS()");

    memset(plmn_list, 0, sizeof(T_MFW_PLMN_LIST));

    if (operLst[0].status == COPS_STAT_Unknown)
    {
		nm_signal(E_NM_PLMN_LIST,plmn_list);
		//Mar 01, 2005    REF: ENH 29327   xdeepadh
		//Deallocate the plmn_list
		if(plmn_list !=NULL)
		{
			mfwFree((U8*)plmn_list,sizeof(T_MFW_PLMN_LIST));
			plmn_list = NULL;
		}
        return;
    }
	/***************************Go-lite Optimization changes Start***********************/
	//	Aug 16, 2004    REF: CRR 24323   Deepa M.D
	TRACE_EVENT_P1("rAT_PlusCOPS: last_ind: %d",last_ind);
	/***************************Go-lite Optimization changes END***********************/	
    if (last_ind > 128) //Invalid index passed in.
    {
	  last_ind = -1;
    }
    else
    {
    	  if (last_ind >= MAX_OPER) //Index is larger than max number of operators.
    	  {
        	last_ind = MAX_OPER-1;
    	  }
    }
    	

    //DS 27/5/2002 - changed <= to <
    for (i = 0; i <= last_ind; i++)      /* operLst == oper_list !!  */
    {
        nm_copy_plmn_list(&plmn_list->plmn[i], &operLst[i]);
    }

	
	/* BEGIN: Aug 09, 2007 REF: OMAPS00140507 x0045876 
	   Added patch given by Dong Feng */	  
	for(i=0; i < MAX_OPER; i++)
	{
		if (operLst[i].status < 0)
		{
			break;
		}
		if (operLst[i].pnn)
		{
			if (operLst[i].long_len)
			{
				switch (operLst[i].long_ext_dcs>>4 & 0x07)
				{
					case 0x00:  /* GSM default alphabet */
						utl_cvtPnn7To8((UBYTE *)operLst[i].longOper,
												operLst[i].long_len,
												operLst[i].long_ext_dcs,
												(UBYTE *)longName);
#ifdef NO_ASCIIZ
						plmn_list->plmn[i].network_long.len = 0;
						plmn_list->plmn[i].network_short.len = 0;
	
						if (operLst->longOper)
						{
							plmn_list->plmn[i].network_long.len = MINIMUM(LONG_NAME-1, strlen((char *)longName));
							memcpy (plmn_list->plmn[i].network_long.data, longName, plmn_list->plmn[i].network_long.len);
							plmn_list->plmn[i].network_long.data[plmn_list->plmn[i].network_long.len] = '\0';
							plmn_list->plmn[i].network_long.dcs = MFW_ASCII;
						}
#else
						memset(plmn_list->plmn[i].network_long, 0, LONG_NAME);
	
						if (operLst->longOper)
							strncpy((char *)plmn_list->plmn[i].network_long, (char *)longName, LONG_NAME-1);
#endif
						break;
				  	case 0x01:  /* UCS2 */
						TRACE_ERROR ("Unhandled UCS2 in PNN entry");
						break;
				  	default:
						TRACE_ERROR ("Unknown DCS in PNN entry");
						break;
				}
			 }
			 if (operLst[i].shrt_len)
			 {
			 	switch (operLst[i].shrt_ext_dcs>>4 & 0x07)
				{
					case 0x00:  /* GSM default alphabet */
						utl_cvtPnn7To8((UBYTE *)operLst[i].shortOper,
												operLst[i].shrt_len,
												operLst[i].shrt_ext_dcs,
								   				(UBYTE *)shrtName);
	
#ifdef NO_ASCIIZ
						plmn_list->plmn[i].network_short.len = 0;
	
						if (operLst->shortOper)
						{
							plmn_list->plmn[i].network_short.len = MINIMUM(SHORT_NAME-1, strlen((char *)shrtName));
							memcpy (plmn_list->plmn[i].network_short.data, shrtName, plmn_list->plmn[i].network_short.len);
							plmn_list->plmn[i].network_short.data[plmn_list->plmn[i].network_short.len] = '\0';
							plmn_list->plmn[i].network_short.dcs = MFW_ASCII;
						}
#else
						memset(plmn_list->plmn[i].network_short, 0, SHORT_NAME);
	
						if (operLst->shortOper)
							strncpy((char *)plmn_list->plmn[i].network_short, (char *)shrtName, SHORT_NAME-1);
#endif
						break;
				  	case 0x01:  /* UCS2 */
						TRACE_ERROR ("ERROR: Unhandled UCS2");
						break;
				  	default:
						TRACE_ERROR ("ERROR: Unknown DCS");
						break;
				}
			 }
		 }
	 }
	/* END: Aug 09, 2007 REF: OMAPS00140507 x0045876 
	   Added patch given by Dong Feng */	  
	/***************************Go-lite Optimization changes Start***********************/
	//	Aug 16, 2004    REF: CRR 24323   Deepa M.D
    TRACE_EVENT_P1("rAT_PlusCOPS: Loop i val is %d",i);
	/***************************Go-lite Optimization changes END***********************/
    plmn_list->count = i;
    
    nm_signal(E_NM_PLMN_LIST,plmn_list);
	//Mar 01, 2005    REF: ENH 29327   xdeepadh
	//Deallocate the plmn_list
	if(plmn_list !=NULL)
	{
		mfwFree((U8*)plmn_list,sizeof(T_MFW_PLMN_LIST));
		plmn_list =NULL;
	}
}




/*
+----------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM               |
| STATE   : code                        ROUTINE : nm_search_reg_status |
+----------------------------------------------------------------------+

  PURPOSE : Change registration status

*/

void nm_search_reg_status(void)
{
  T_ACI_NRG_RGMD regMode;
  T_ACI_NRG_SVMD srvMode;
  T_ACI_NRG_FRMT oprFrmt;
  T_ACI_NRG_SVMD srvStat;
  U8 oper[LONG_NAME];

  qAT_PercentNRG(CMD_SRC_LCL,
                 &regMode,
                 &srvMode,
                 &oprFrmt,
                 &srvStat,
                 (char *)oper);

  if (srvStat == NRG_SVMD_Limited)
  {
    reg_stat = MFW_LIMITED_SERVICE;
    nm_signal(E_NM_LIMITED_SERVICE,NULL);
  }
  if (srvStat == NRG_SVMD_NoSrv)
  {
    reg_stat = MFW_NO_SERVICE;
    nm_signal(E_NM_NO_SERVICE,NULL);
  }
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM             |
| STATE   : code                        ROUTINE : rAT_PlusCREG       |
+--------------------------------------------------------------------+

  PURPOSE : Change registration status

*/

void rAT_PlusCREG (T_ACI_CREG_STAT stat,
                   USHORT          lac,
                   USHORT          cid)
{
    T_MFW_NETWORK_STRUCT plmn_ident;
    T_MFW_CELL_STRUCT cell;
    T_ACI_NRG_RGMD regMode;
    T_ACI_NRG_SVMD srvMode;
    T_ACI_NRG_FRMT oprFrmt;
    T_ACI_NRG_SVMD srvStat;
	USHORT cause;     //Added for OMAPS00134509/ASTec32515
    U8 oper[LONG_NAME];

   TRACE_FUNCTION("rAT_PlusCREG()");

    switch (stat)
    {
        case CREG_STAT_Reg:
        case CREG_STAT_Roam:
            reg_stat = MFW_FULL_SERVICE;
            qAT_PercentNRG(CMD_SRC_LCL,
                           &regMode,
                           &srvMode,
                           &oprFrmt,
                           &srvStat,
                           (char *)oper);
#ifdef NO_ASCIIZ
            sAT_PlusCOPSE(oper,(U8) oprFrmt,&plmn_ident.network_long,
                                      &plmn_ident.network_short,
                                      plmn_ident.network_numeric);
            memcpy(plmn_ident.service_provider.data,
                   service_provider_name.data,
                   service_provider_name.len);
            plmn_ident.service_provider.len = service_provider_name.len;
#else
            sAT_PlusCOPSE(oper,(U8) oprFrmt,plmn_ident.network_long,
                                      plmn_ident.network_short,
                                      plmn_ident.network_numeric);
            strcpy((char *) plmn_ident.service_provider,
                   (char *) service_provider_name);
#endif
            plmn_ident.display_condition = display_condition & 0x01;
            if (stat == CREG_STAT_Reg)
                plmn_ident.roaming_indicator = 0;
            if (stat == CREG_STAT_Roam)
                plmn_ident.roaming_indicator = 1;
            
             //x0pleela 08 Feb, 2006 ER: OMAPS00065203
             nm_get_COPN( &plmn_ident );      

              nm_signal(E_NM_FULL_SERVICE,&plmn_ident);

  		/* Cell ReSelection Code */
            	cell.ci = cid;
            	cell.lid = lac;
            	nm_signal(E_NM_CELL_SELECT,&cell);         
            break;
            
        case CREG_STAT_NoSearch:
        case CREG_STAT_NotPresent:
            if (!reg_flag)
                nm_search_reg_status();
            break;
        case CREG_STAT_Search:
            if (reg_flag)
            {
                reg_stat = MFW_SEARCH_NETWORK;
                nm_signal(E_NM_SEARCH_NETWORK,NULL);
            }
            else
                nm_search_reg_status();
            break;
               //Sep 18,2006    DRT: OMAPS0090268 x0061088(Prachi)
		//Description : added case for Denied services,which send nm_signal.
           case CREG_STAT_Denied:
//TISH, patch for ASTec32515/OMAPS00134509
//start
					qAT_PlusCEER( CMD_SRC_LCL, &cause);
					if (cause EQ 0x8488)     // If SIM is rejected in authentification
						sim_invalid_ind();
					else
//end
              	  nm_signal(E_NM_FORBIDDEN_NETWORK, NULL);
            break;
        default:
            nm_search_reg_status();
            break;
    }
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM             |
| STATE   : code                        ROUTINE : nm_abort_avail_plmn|
+--------------------------------------------------------------------+

  PURPOSE : Change registration status

*/

void nm_abort_avail_plmn(void)
{
    sAT_Abort(CMD_SRC_LCL, AT_CMD_COPS);
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM             |
| STATE   : code                        ROUTINE : nm_nitz_info_ind   |
+--------------------------------------------------------------------+

  PURPOSE : NITZ information indication

*/

#ifdef FF_2TO1_PS
void  nm_nitz_info_ind(T_MMREG_INFO_IND * mmr_info_ind)


{
  UBYTE flag;

  TRACE_FUNCTION("nm_nitz_info_ind() ");

  flag = 0;

 /* BEGIN ADD: SUMIT: REQ : aligning to 5.x SAP definitions */
#ifdef NEPTUNE_BOARD

  /* copy country code and network code */
  memset (net_ind.mcc, 0, PS_SIZE_MCC);
  memset (net_ind.mnc, 0, PS_SIZE_MNC_MAX);
  if (mmr_info_ind->v_plmn_id EQ TRUE)
  {
    flag = 1;
    memcpy (net_ind.mcc, mmr_info_ind->plmn_id.mcc, PS_SIZE_MCC);
    memcpy (net_ind.mnc, mmr_info_ind->plmn_id.mnc, PS_SIZE_MNC_MAX);
  }

  /* copy long name */
  memset(&net_ind.long_name, 0, sizeof(T_MFW_NET_NAME));

  if (mmr_info_ind->v_full_net_name EQ TRUE)
  {
    flag = 1;
    net_ind.long_name.dcs       = mmr_info_ind->full_net_name.cs;
    net_ind.long_name.add_ci    = mmr_info_ind->full_net_name.add_ci;
    net_ind.long_name.num_spare = mmr_info_ind->full_net_name.num_spare;
    net_ind.long_name.len = MINIMUM (MMREG_MAX_TEXT_LEN, mmr_info_ind->full_net_name.c_text);
    memcpy (net_ind.long_name.data,
            mmr_info_ind->full_net_name.text,
            net_ind.long_name.len);
  }

  /* copy short name */
  memset(&net_ind.short_name, 0, sizeof(T_MFW_NET_NAME));
  if (mmr_info_ind->v_short_net_name EQ TRUE)
  {
    flag = 1;
    net_ind.short_name.dcs       = mmr_info_ind->short_net_name.cs;
    net_ind.short_name.add_ci    = mmr_info_ind->short_net_name.add_ci;
    net_ind.short_name.num_spare = mmr_info_ind->short_net_name.num_spare;
    net_ind.short_name.len = MINIMUM (MMREG_MAX_TEXT_LEN, mmr_info_ind->short_net_name.c_text);
    memcpy (net_ind.short_name.data,
            mmr_info_ind->short_net_name.text,
            net_ind.short_name.len);
  }

  if (flag)
    nm_signal(E_MFW_NET_IND, &net_ind);

  flag = 0;
  memset(&tim_ind, 0, sizeof(T_MFW_TIME_IND));
  /* copy time */
  if (mmr_info_ind->v_universal_time EQ TRUE AND
      mmr_info_ind->v_local_time_zone EQ TRUE )
  {
    flag = 1;
    tim_ind.timezone = mmr_info_ind->local_time_zone.time_zone;
    tim_ind.year     = (mmr_info_ind->universal_time.year);
    tim_ind.month    = (mmr_info_ind->universal_time.month);
    tim_ind.day      = (mmr_info_ind->universal_time.day);
    tim_ind.hour     = (mmr_info_ind->universal_time.hour);
    tim_ind.minute   =(mmr_info_ind->universal_time.minute);
    tim_ind.second   = (mmr_info_ind->universal_time.second );
  }
  else if (mmr_info_ind->v_local_time_zone EQ TRUE AND
           mmr_info_ind->v_universal_time NEQ TRUE )
  {
    flag = 1;
    tim_ind.timezone = mmr_info_ind->local_time_zone.time_zone;
  }

#else
  /* END ADD: Sumit */
 
//Apr 13, 2007 DRT:OMAPS00122739 x0066814(Geetha)
#if(0)
  /* copy country code and network code */
  memset (net_ind.mcc, 0, PS_SIZE_MCC);
  memset (net_ind.mnc, 0, PS_SIZE_MNC_MAX);
  if (mmr_info_ind->plmn.plmn_presence EQ TRUE)
  {
    flag = 1;
    memcpy (net_ind.mcc, mmr_info_ind->plmn.plmn_id.mcc, PS_SIZE_MCC);
    memcpy (net_ind.mnc, mmr_info_ind->plmn.plmn_id.mnc, PS_SIZE_MNC_MAX);
  }

  /* copy long name */
  memset(&net_ind.long_name, 0, sizeof(T_MFW_NET_NAME));
  if (mmr_info_ind->full_net_name.v_name EQ TRUE)
  {
    flag = 1;
    net_ind.long_name.dcs       = mmr_info_ind->full_net_name.cs;
    net_ind.long_name.add_ci    = mmr_info_ind->full_net_name.add_ci;
    net_ind.long_name.num_spare = mmr_info_ind->full_net_name.num_spare;
    net_ind.long_name.len = MINIMUM (MMREG_MAX_TEXT_LEN, mmr_info_ind->full_net_name.c_text);
    memcpy (net_ind.long_name.data,
            mmr_info_ind->full_net_name.text,
            net_ind.long_name.len);
  }

  /* copy short name */
  memset(&net_ind.short_name, 0, sizeof(T_MFW_NET_NAME));
  if (mmr_info_ind->short_net_name.v_name EQ TRUE)
  {
    flag = 1;
    net_ind.short_name.dcs       = mmr_info_ind->short_net_name.cs;
    net_ind.short_name.add_ci    = mmr_info_ind->short_net_name.add_ci;
    net_ind.short_name.num_spare = mmr_info_ind->short_net_name.num_spare;
    net_ind.short_name.len = MINIMUM (MMREG_MAX_TEXT_LEN, mmr_info_ind->short_net_name.c_text);
    memcpy (net_ind.short_name.data,
            mmr_info_ind->short_net_name.text,
            net_ind.short_name.len);
  }

  if (flag)
    nm_signal(E_MFW_NET_IND, &net_ind);
#endif
  flag = 0;
  memset(&tim_ind, 0, sizeof(T_MFW_TIME_IND));
  /* copy time */
  if (mmr_info_ind->net_time_zone.v_time_zone EQ TRUE AND
      mmr_info_ind->net_time.v_time EQ TRUE )
  {
    flag = 1;
    tim_ind.timezone = mmr_info_ind->net_time_zone.time_zone;
    tim_ind.year     = ((mmr_info_ind->net_time.year[0]-'0')   * 10 + (mmr_info_ind->net_time.year[1]-'0') );
    tim_ind.month    = ((mmr_info_ind->net_time.month[0]-'0')  * 10 + (mmr_info_ind->net_time.month[1]-'0') );
    tim_ind.day      = ((mmr_info_ind->net_time.day[0]-'0')    * 10 + (mmr_info_ind->net_time.day[1]-'0') );
    tim_ind.hour     = ((mmr_info_ind->net_time.hour[0]-'0')   * 10 + (mmr_info_ind->net_time.hour[1]-'0') );
    tim_ind.minute   = ((mmr_info_ind->net_time.minute[0]-'0') * 10 + (mmr_info_ind->net_time.minute[1]-'0') );
    tim_ind.second   = ((mmr_info_ind->net_time.second[0]-'0') * 10 + (mmr_info_ind->net_time.second[1]-'0') );
  }
  else if (mmr_info_ind->net_time_zone.v_time_zone EQ TRUE AND
           mmr_info_ind->net_time.v_time NEQ TRUE )
  {
    flag = 1;
    tim_ind.timezone = mmr_info_ind->net_time_zone.time_zone;
  }
/*BEGIN ADD: Neptune */
#endif
/*END ADD: Neptune */
  if (flag)
    nm_signal(E_MFW_TIME_IND, &tim_ind);
}
#else
void  nm_nitz_info_ind(T_MMR_INFO_IND * mmr_info_ind)
{
  UBYTE flag;

  TRACE_FUNCTION("nm_nitz_info_ind() ");
//Apr 13, 2007 DRT:OMAPS00122739 x0066814(Geetha)  
#if(0)
  flag = 0;
  /* copy country code and network code */
  memset (net_ind.mcc, 0, SIZE_MCC);
  memset (net_ind.mnc, 0, SIZE_MNC);
  if (mmr_info_ind->plmn.v_plmn EQ TRUE)
  {
    flag = 1;
    memcpy (net_ind.mcc, mmr_info_ind->plmn.mcc, SIZE_MCC);
    memcpy (net_ind.mnc, mmr_info_ind->plmn.mnc, SIZE_MNC);
  }

  /* copy long name */
  memset(&net_ind.long_name, 0, sizeof(T_MFW_NET_NAME));
  if (mmr_info_ind->full_name.v_name EQ TRUE)
  {
    flag = 1;
    net_ind.long_name.dcs       = mmr_info_ind->full_name.dcs;
    net_ind.long_name.add_ci    = mmr_info_ind->full_name.add_ci;
    net_ind.long_name.num_spare = mmr_info_ind->full_name.num_spare;
    net_ind.long_name.len = MINIMUM (MMR_MAX_TEXT_LEN, mmr_info_ind->full_name.c_text);
    memcpy (net_ind.long_name.data,
            mmr_info_ind->full_name.text,
            net_ind.long_name.len);
  }

  /* copy short name */
  memset(&net_ind.short_name, 0, sizeof(T_MFW_NET_NAME));
  if (mmr_info_ind->short_name.v_name EQ TRUE)
  {
    flag = 1;
    net_ind.short_name.dcs       = mmr_info_ind->short_name.dcs;
    net_ind.short_name.add_ci    = mmr_info_ind->short_name.add_ci;
    net_ind.short_name.num_spare = mmr_info_ind->short_name.num_spare;
    net_ind.short_name.len = MINIMUM (MMR_MAX_TEXT_LEN, mmr_info_ind->short_name.c_text);
    memcpy (net_ind.short_name.data,
            mmr_info_ind->short_name.text,
            net_ind.short_name.len);
  }

  if (flag)
    nm_signal(E_MFW_NET_IND, &net_ind);
#endif

  flag = 0;
  memset(&tim_ind, 0, sizeof(T_MFW_TIME_IND));
  /* copy time */
  if (mmr_info_ind->ntz.v_tz EQ TRUE AND
      mmr_info_ind->time.v_time EQ TRUE )
  {
    flag = 1;
    tim_ind.timezone = mmr_info_ind->ntz.tz;
    tim_ind.year     = mmr_info_ind->time.year;
    tim_ind.month    = mmr_info_ind->time.month;
    tim_ind.day      = mmr_info_ind->time.day;
    tim_ind.hour     = mmr_info_ind->time.hour;
    tim_ind.minute   = mmr_info_ind->time.minute;
    tim_ind.second   = mmr_info_ind->time.second;
  }
  else if (mmr_info_ind->ntz.v_tz EQ TRUE AND
           mmr_info_ind->time.v_time NEQ TRUE )
  {
    flag = 1;
    tim_ind.timezone = mmr_info_ind->ntz.tz;
  }

  if (flag)
    nm_signal(E_MFW_TIME_IND, &tim_ind);
}
#endif

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM             |
| STATE   : code                        ROUTINE : nmCommand          |
+--------------------------------------------------------------------+

  PURPOSE : handle mfw windows command

*/

static int nmCommand (U32 cmd, void *h)
{
    switch (cmd)
    {
        case MfwCmdDelete:              /* delete me                */
            if (!h)
                return 0;
            nm_delete(h);
            return 1;
        default:
            break;
    }

    return 0;
}



#ifdef NO_ASCIIZ
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM             |
| STATE   : code                        ROUTINE : nm_get_spn         |
+--------------------------------------------------------------------+

  PURPOSE : Function to give access to the service provider string

  Parameters  : None

  Return: Service provider string

*/

void nm_get_spn (T_MFW_SP_NAME *name)
{
    if (name NEQ NULL)
        memcpy(name, &service_provider_name, sizeof(T_MFW_SP_NAME));
}

#else
/* PATCH PMC 000804 */
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM             |
| STATE   : code                        ROUTINE : nm_get_spn         |
+--------------------------------------------------------------------+

  PURPOSE : Function to give access to the service provider string

  Parameters  : None

  Return: Service provider string

*/

void nm_get_spn (char *name)
{
    if (name NEQ NULL)
        strncpy (name, service_provider_name, LONG_NAME);
}
/* END PATCH PMC */
#endif

/*
+---------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_NM              |
| STATE   : code                        ROUTINE : nm_abort_registraion|
+---------------------------------------------------------------------+

  PURPOSE : abort current registration

  Parameters  : None

*/
void nm_abort_registraion(void)
{
    sAT_Abort(CMD_SRC_LCL, AT_CMD_NRG);
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)  MODULE  : MFW_NM                  |
| STATE   : code                  ROUTINE : nm_decodeSourceId         |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to convert the source id
            used by ACI to the source id used by MFW.
*/

//x0pleela 09 Feb, 2006  ER: OMAPS00065203
int nm_decodeSourceId( UBYTE sourceId)
{
   TRACE_FUNCTION("nm_decodeSourceId()");
#ifndef NEPTUNE_BOARD
    switch (sourceId)
    {	
        case Read_ROM_TABLE:        	return NM_ROM;
        case Read_EONS:          		return NM_EONS;
        case Read_CPHS:        		return NM_CPHS;
        case Read_INVALID:	        	
        default:                        		return NM_NONE;
    }
#endif
/* to remove warning Aug -11 */
#ifdef NEPTUNE_BOARD
   return NM_NONE;
#endif
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)  MODULE  : MFW_NM                  |
| STATE   : code                  ROUTINE : nm_update_plmn_data         |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to update plmn data after querying COPN
*/
//x0pleela 09 Feb, 2006  ER: OMAPS00065203
void nm_update_plmn_data( T_MFW_NETWORK_STRUCT *plmn_ident, 
							    T_ACI_OPER_NTRY oper_ntry)
{
  TRACE_FUNCTION("nm_update_plmn_data()");
 
#ifndef NEPTUNE_BOARD
  plmn_ident->opn_read_from = (T_MFW_NM_OPN)nm_decodeSourceId(oper_ntry.source); /*a0393213 compiler warnings removal - explicit typecast done*/
#endif


      //update plmn_ident with EONS
#ifdef NO_ASCIIZ
      memset(plmn_ident->network_long.data, 0, LONG_NAME);
      memset(plmn_ident->network_short.data, 0, SHORT_NAME);
      memcpy( plmn_ident->network_long.data, 
                      oper_ntry.longName, 
                      strlen(oper_ntry.longName) );
    
      memcpy( plmn_ident->network_short.data, 
                      oper_ntry.shrtName, 
                    strlen(oper_ntry.shrtName));

      plmn_ident->network_long.len = strlen((const char*)(plmn_ident->network_long.data)); /*a0393213 warnings removal - explicit typecasting done*/
      plmn_ident->network_short.len = strlen((const char*)(plmn_ident->network_short.data)); 

#else
      memset(plmn_ident->network_long, 0, LONG_NAME);
      memset(plmn_ident->network_short, 0, SHORT_NAME);
      strcpy( plmn_ident->network_long, 
      		      oper_ntry.longName);
      strcpy( plmn_ident->network_short, 
      		      oper_ntry.shrtName);
#endif	       
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)  MODULE  : MFW_NM                  |
| STATE   : code                  ROUTINE : nm_get_COPN         |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to query for COPS and COPN which will indicate from 
  		    where the operator name was read
*/
//x0pleela 09 Feb, 2006  ER: OMAPS00065203
void nm_get_COPN( T_MFW_NETWORK_STRUCT *plmn_ident )
{
  T_ACI_COPS_MOD mode;
  T_ACI_COPS_FRMT format;
  T_ACI_OPER_NTRY oper_ntry;
  char opername[LONG_NAME];

  TRACE_FUNCTION("nm_get_COPN()");

  memset (&oper_ntry, 0, sizeof( T_ACI_OPER_NTRY ));
  //Check where MS is registered
  if( qAT_PlusCOPS ( CMD_SRC_LCL, &mode,
                                  	&format, (char *)opername ) EQ AT_CMPL )
  {
    //Request info about operator name
    if(qAT_PercentCOPN(CMD_SRC_LCL, 
    		format, 
    		(char *)opername, 
    		&oper_ntry) EQ AT_CMPL )
    {	  
        nm_update_plmn_data(plmn_ident, oper_ntry);       
    }
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)  MODULE  : MFW_NM                  |
| STATE   : code                  ROUTINE : nm_get_opername         |
+--------------------------------------------------------------------+

  PURPOSE : This function updates operator name based on cingular's prioritization requirement. 
       Priority would be in order ONS->NITZ->ROM

*/
//x0pleela 09 Feb, 2006  ER: OMAPS00065203
void nm_get_opername(T_MFW_NETWORK_STRUCT  * plmn_ident)
{
  TRACE_FUNCTION("nm_get_opername()");
     sAT_PercentCNIV(CMD_SRC_LCL, CNIV_MODE_ON);
 
  return;
}


// June 02, 2006    DR: OMAPS00078005 x0021334
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)  MODULE  : MFW_NM                   |
| STATE   : code                  ROUTINE : nm_get_network_time           |
+--------------------------------------------------------------------+

  PURPOSE : This function sets the PCTZV mode on.

*/
#ifdef FF_TIMEZONE
void nm_get_network_time(void)
{
	TRACE_FUNCTION("nm_get_network_time()");

    // Put PCTZV mode on to get time and date updates
    //x0066814(Geetha), OMAPS00098351, Timezone information is update through nm_nitz_info_ind() function
    //sAT_PercentCTZV(CMD_SRC_LCL, PCTZV_MODE_ON);
}
#endif


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)  MODULE  : MFW_NM                  |
| STATE   : code                  ROUTINE : nm_get_opername         |
+--------------------------------------------------------------------+

  PURPOSE : This function gets operator name from network. 
*/
//x0pleela 09 Feb, 2006  ER: OMAPS00065203

void nm_get_network_data(T_MFW_NETWORK_STRUCT *plmn_netw_data)
{
	TRACE_FUNCTION("nm_get_network_data()");
	
	//send E_NM_OPN event 
	nm_signal(E_NM_OPN, plmn_netw_data); 
}


// June 02, 2006    DR: OMAPS00078005 x0021334
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)  MODULE  : MFW_NM                   |
| STATE   : code                  ROUTINE : nm_set_network_time      |
+--------------------------------------------------------------------+

  PURPOSE : This function sets time and date info received from network. 
*/
#ifdef FF_TIMEZONE
void nm_set_network_time(T_MFW_TIME_IND *currclock)
{
	TRACE_FUNCTION("nm_set_network_time()");

  	//send E_MFW_TIME_IND event 
    nm_signal(E_MFW_TIME_IND, currclock); 
}
#endif

/* Aug 07, 2007 DR:OMAPS00137726 x0066814(Geetha)*/
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)  MODULE  : MFW_NM                   |
| STATE   : code                  ROUTINE : nm_get_plmn_list_during_bootup      |
+--------------------------------------------------------------------+

  PURPOSE : This function gets the  plmn list during bootup
*/
void nm_get_plmn_list_during_bootup(void)
{
  	TRACE_FUNCTION ("nm_get_plmn_list_during_bootup()");
     nm_signal(E_NM_PLMN_LIST_BOOTUP,0);
	
}

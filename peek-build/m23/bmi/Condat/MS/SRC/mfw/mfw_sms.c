/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_sms.c       $|
| $Author:: Vo                          $Revision::  1              $|
| CREATED: 21.12.98                     $Modtime:: 24.03.00 10:25   $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_SMS
   PURPOSE : This module contains SMS management functions.

     Mar 22 2007 ER:OMAPS00113882 x0061088 (Prachi)
    Description:SMS alphanumeric address
    Solution:Inorder to support alphanumeric address,MMI is setting editor mode as
    "ED_MODE_ALPHA" in  the function SmsSend_R_OPTExeSend(). Also MMI is checking whether user
    have entered Numeric oralphanumeric address in the function sms_submit() and
    sms_submit before calling sAT_PlusCMGS() and sAT_PlusCMGW() respectively.

      Feb 28, 2007 OMAPS00109953 a0393213(R.Prabakar)
      Description : Need to be able to store SMS in SIM/ME
      Solution     : Earlier, index alone was sufficient to identify messages. Now memory should also be taken care of.

	July 27, 2006    DR: OMAPS00085347 x0021334
   	Description: The display freeze when reading a stored concatenated SMS (This concatenated
   	                  SMS consists of 3 SMS) from SIM.
   	Solution: Added code to free the allocated memory after concatenated SMS has been read.   

	May 18, 2006    DR: OMAPS00070657 x0pleela
   	Description: CPHS feature on Locosto-Lite 
   	Solution: 	Enabling the voice mail indication if there are any voice mails		

	Feb 07, 2006    DR: OMAPS00059261 x0pleela
   	Description: SMS is not forwarding STATUS reports that are read from SIM storage to ACI 
   	Solution: sms_decode_status(): removed the code added for handling pending status of a status report
			rat_plusCMGR(): updating sms_str based on tp_status of a status report
			Removed macros STAT_MIN_VALUE and STAT_MAX_VALUE added earlier
			Replaced MT_STATUS_RPT with MFW_SMS_STATRPT as MT_STATUS_RPT is removed			

     	Feb 01, 2006    DR: OMAPS00059261 x0pleela
   	Description: SMS is not forwarding STATUS reports that are read from SIM storage to ACI 
   	Solution: Added two macros to check the range of status of status report
			Used enums for the hardcoded values for MFW_MSG_DELIVER(MT), MFW_MSG_SUBMIT (MO) and MFW_MSG_STATUS_REPORT (STATUS REPORT) types while reading the messages
			Added a check in "sms_decodeStatus" to support pending status of the status report
			Made changes as per review comments
   	
      	Jan 30, 2006    DR: OMAPS00059261 x0pleela
   	Description: SMS is not forwarding STATUS reports that are read from SIM storage to ACI 
   	Solution: Added code to read MT Status report
   	

   	Jan 20, 2006    REF: DRT OMAPS00053377 xdeepadh
   	Description: SMS Validity Period - using MMI - Failure, MS not able to set SMS validity
   	Solution: While setting the validity period, the validity period format also has been set in the 
   	frst octet.

	Nov 15,2005 	DR : OMAPS00057280	-	x0034700
 	Description:	Compilation Errors in Neptune build while integrating  latest MMI Changes
 	Solution:		Included the compilation FLAG "NEPTUNE_BOARD" for un wanted code for Neptune build

	Feb 22, 2006	REF: MMI-FIX-OMAPS00058684 x0035544(Sudha)
	Description: Can't send a SMS with a specific SIM card
	Solution: Check the PID(Protocol ID) value for an MO SMS and Set the PID(Protocol ID) accordingly. 
	
       Dec 15, 2005   REF: MMI-FIX-OMAPS00058684 x0035544
       Description: OMAPS00045726 / MMI-FIX-30439 Follow-Up : ifdef for FF Home Zone forgotten.
       Solution: Added Feature flag FF_HOMEZONE for the function sms_homezoneCBToggle(U8 Activate_CB)
       
        Nov 8, 2005 REF: OMAPS00050772, a0876501 (Shashi)  
  	Issue description:  CMGD merge from tcs4.0 to g23m.
  	Solution: New parameter is added to sAT_PlusCMGD function. Migrating the MFW changes.


   	Oct 18, 2005    REF: SPR 34678 xdeepadh
   	Description: MMI Screen – shows garbled characters for Cell Information 
   	Solution: The CB messages with DCS 242(class-2,default alphabet)  are discarded.

        May 27, 2005    MMI-FIX-29869 x0018858
   	Description: There is a limitation on the number of messages that can be downloaded from 
   	server even though there is space on the sim.
   	Solution: The limitation has been removed and the number of messages that can be dowloaded
   	has been modified to the masimum number that can be stored.

	Jun 14, 2005 REF: MMI-FIX-30439 x0018858
   	Description: The homezone/cityzone tags were not being displayed properly.
   	Solution: Modified to save the tags properly in caches and also made the 
	appropriate modifications to update the same properly during mobility.

   	July 7, 2005 REF: CRR MMI-SPR-32179 x0018858  
  	Issue description:  PS crashed while performing call Forwarding.
  	Solution: Since this is related to memory leaks which occur, the code has been added to 
  			free certain memory based on a a checking condition and also by cleaning the buffer.

       xrashmic 9 May, 2005 MMI-SPR-30159
       Concat PUSH msg are stored and sent to wap client once all parts are received

	x0018858 24 Mar, 2005 MMI-FIX-11321
	Issue Description :- If the inbox is full and a new message is receivd, no indication is received.
	Fix: Added support for handling indication when message box is full and
	there is a pending message.

      	March 31, 2005    REF: CRR MMI-SPR-24511   x0012852
	Bug:The phone crashes when the user selects the 'Delete All' option 
	almost the same time as there's an incoming message.
       Fix:Check added for part message before deleting all messages.

       March 2, 2005    REF: CRR 11536 x0018858
       Description: Reply path not supported while sending a reply.
       Solution: Added menu option for the reply path.


	Feb 28, 2004    REF: CRR 28809   Deepa M.D
	Bug:SRAM Optimization :Dynamic allocation of sms_mo
	Fix:The static T_MFW_SMS_MO sms_mo variable, is made a local pointer variable.
	It is dynamically allocated and deallocated.
	The Optimization changes done for golite build have been incorpaorated to color build.
	The  MMI_LITE flag used for rAT_PlusCMGR has been removed.

   	Feb 16, 2005 REF: CRR MMI-SPR-27900 xnkulkar 
  	Issue description:  BMI: using the tool nowSMS the voicemail indication via sms does not work
  	Solution: Instead of identifying voice using addr[0] and pid, we use dcs. Also, messages belonging
  	              to group "1100" are deleted when voicemail icon is displayed.

   	Dec 23, 2004 REF: CRR MMI-SPR-27741 xkundadu 
  	Issue description:  BMI: crashing when using the function : sms_index_list() located in mfw
  	Solution: Changed the msg_list variable to array of T_MFW_SMS_IDX.
 	
	Aug 18, 2004    REF: CRR 24282   Deepa M.D
	Bug:Go-lite Optimization  Dynamic allocation of sms_mo
	Fix:static T_MFW_SMS_MO sms_mo has been made a local variable.
	It is allocated/deallocated dynamically.

   	Aug 16, 2004    REF: CRR 24323   Deepa M.D
	Bug:Clenup of sprintf used for tracing
	Fix:Replace the char buf[]; sprintf (buf, "...", ...); TRACE_EVENT (buf); statements by TRACE_EVENT_PX

	Aug 17, 2004    REF: CRR 24281     Deepa M.D
	Bug:Go-lite  Optimization  Remove the unsed variable  temp_conv_buffer
	Fix:Removed the unused variables from mfw_sms.c

	Aug 26, 2004    REF: CRR 23287     Deepa M.D
	Bug:Inconsistent events being passed in MFW sms module
	Fix:For E_SMS_MT_RECEIVED event,sms_id_wait is passed.

	Jul 07, 2004    REF: CRR 15846     Deepa M.D
	Bug	:	Concatenated SMT : SMS present but not displayed !!!
	Fix :	Check has been made before freeing the memory.
  
	SPR 2260:  Merged in bugfixes 1833, 1669, 872,1830, 1828, 2031, and 2007 
	fom the 1.6.3 branch into this file.
	
	Apr 12, 2004    REF: CRR 13370     Deepa M.D
	Bug : The Sender name was not displayed properly in the status report.
	Fix : The sendername had garbage values when the corresponding number was not
	present in the Phonebook.So when the name was not present, only the number is displayed.
	  
			

*/

#define ENTITY_MFW

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#if defined (NEW_FRAME)

#include "typedefs.h"
#include "vsi.h"
#include "custom.h"
#include "gsm.h"

#else

#include "STDDEFS.H"
#include "custom.h"
#include "gsm.h"
#include "vsi.h"

#endif

#include "cus_aci.h"
#include "message.h"
#include "prim.h"
#include "aci_cmh.h"
#include "ksd.h"
#include "prim.h"
#ifndef PCM_2_FFS
#include "pcm.h"
#endif

#ifndef FF_2TO1_PS
#include "p_mnsms.val"
#endif

#include "psa.h"
#include "psa_sim.h"

#include "mfw_mfw.h"
#include "mfw_nm.h"
#include "mfw_sima.h"
#include "mfw_nmi.h"
#include "mfw_phb.h"
#include "mfw_cm.h"
#include "mfw_cmi.h"
#include "mfw_sim.h"
#include "mfw_simi.h"
#include "mfw_phbi.h"
#include "mfw_sms.h"
#include "mfw_smsi.h"
#include "mfw_win.h"
#include "mfw_cphs.h"
#include "mfw_cphsi.h"
#include "aci_lst.h"
#include "conc_sms.h"


#ifdef SIM_TOOLKIT
#include "mfw_sat.h"
#endif

#include <string.h>
#include <stdio.h>

/********* current define *******************************************/
/*SPR 2260 added definitions*/
#define FIRST_READ_OF_MEMORY_USE 1
#define POST_DELETE_CHECK        2
// Feb 22, 2006 DR:OMAPS00068268 x0035544
//Added the #defines for the bits 7, 6 and 5 
#define BIT_7	0x80
#define BIT_6	0x40
#define BIT_5	0x20

//May 27, 2005    MMI-FIX-29869 x0018858 - Added the #defines
#ifdef FF_MMI_SMS_DYNAMIC
#define ALLOC_MEMORY mfwAlloc
#define FREE_MEMORY mfwFree
//May 27, 2005    MMI-FIX-29869 x0018858- Modified from staic array to dynamic memory.
T_MFW_SMS_MSG* msg_info = NULL; /* message table */

//May 27, 2005    MMI-FIX-29869 x0018858 - Added a Checking variable.
static int Counter_first = 0;
#else
static T_MFW_SMS_MSG msg_info[MAX_MESSAGES]; /* message table */
#endif


static T_MFW_SMS_MT sms_mt;
static T_MFW_SMS_CB sms_cb;
static T_MFW_SMS_ID sms_id, sms_id_wait;

static T_MFW_MEM_INFO     memory;
static T_MFW_MEM_INFO sim_memory; /*a0393213 OMAPS00109953 - To keep track of sms memory in SIM*/
static T_MFW_MEM_INFO ffs_memory;   /*a0393213 OMAPS00109953 - To keep track of sms memory in ME*/
/*a0393213 OMAPS00109953 - state machine for index retrieval - first retrieve all SIM indexes then all ME indexes*/
static T_MFW_SMS_INDEX_RETRIEVE_STATE index_retrieve_state; 

static SHORT			 status_change_index;
/*a0393213 - To store the memory(SIM/ME) where the message, whose status is to be changed, resides*/
static T_MFW_SMS_MEM status_change_memory=MFW_MEM_NOTPRESENT; 

static SHORT			 deleting_index;
/*a0393213 OMAPS00109953 - To store the memory where the SMS, which is to be deleted, resides*/
static T_MFW_SMS_MEM deleting_memory=MFW_MEM_NOTPRESENT; 

static UBYTE              f_update;
static UBYTE              int_req;
static UBYTE              smsReady;
static UBYTE              phbReady;
static T_ACI_SMS_STAT              sentStatus; /*a0393213 compiler warnings removal - type of sentStatus changed from UBYTE*/
static UBYTE              sms_update = FALSE;

static UBYTE              coll_sms_send;   /* mt sms during send process */
static UBYTE              coll_sms_read;   /* mt sms during read process */
static UBYTE              coll_sms_delete; /* mt sms during delete process */
static UBYTE              coll_sms_req;    /* mt sms during request process */

static UBYTE              data[30];

static UBYTE			initialising_flag;

UBYTE     sms_type;
UBYTE     mt_type;
UBYTE     int_init_req;
CHAR     *wr_addr;
UBYTE Delete_1_2_1_vm_icon=FALSE;
UBYTE Display_1_2_1_vm_icon =FALSE;
static UBYTE     cb_mid[10];  /* cell broadcast message identifier */
static UBYTE     fg;
EXTERN MfwHdr * current_mfw_elem;

static BOOL cmgr_awaiting_ok = FALSE; // Marcus: Issue 2260: 03/10/2002
#if defined (FF_WAP) && defined (FF_GPF_TCPIP)
static UBYTE sms_wap_sent_flag = FALSE;  /* SPR#2086 - SH - TRUE if SMS is sent */
#endif

// Feb 16, 2005 REF: CRR MMI-SPR-27900 xnkulkar 
// Added this variable to identify voice mail message to be deleted
BOOL invalidVm = FALSE; 

//x0pleela 30 Jan, 2006 DR: OMAPS00059261 
//SMS buffer storage size
#define MAX_STAT_MSG_LEN 30 

extern UBYTE reply_path_bit ;
/*********************Prototypes****************************************/

//Feb 22, 2006 DR:OMAPS00068268 x0035544 
BOOL Sms_Check_PID(SHORT pid);
/*SPR2260 added prototype for new function*/
/*a0393213 OMAPS00109953 - memory added as new parameter*/
BOOL delete_sms_from_table (SHORT index, T_MFW_SMS_MEM memory);
//Dec 14, 2005 REF: MMI-FIX-OMAPS00058684 x0035544
#ifdef FF_HOMEZONE
//Jun 14, 2005 REF: MMI-FIX-30439 x0018858
void sms_homezoneCBToggle(U8 Activate_CB);
#endif
LOCAL T_ACI_SMS_STOR sms_mem_cvt(UBYTE mem);
#ifdef FF_MMI_SMS_DYNAMIC

//Mar 22 2007 ER:OMAPS00113882 x0061088 (Prachi)
int is_alphanumeric_addr(char *dest_addr);

//	May 27, 2005    MMI-FIX-29869 x0018858 - Begin
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS           |
| STATE   : code                         ROUTINE : Check_max_sms_allowed          |
+--------------------------------------------------------------------+

   PURPOSE : Check for the max number of messages that can be stored and aloocate 
   memory accordingly.

*/
void Check_max_sms_allowed(void)
{

	int i,num, my_count ;
	T_MFW_SMS_MSG* cpy_msg_info = NULL; /* message table */
	U8 *MaxTotal;

	if((memory.total > 0) &&( Counter_first == 0))
		{
			g_max_messages = memory.total;

			if( msg_info == NULL)
				{
					msg_info = (T_MFW_SMS_MSG*)ALLOC_MEMORY(g_max_messages*sizeof(T_MFW_SMS_MSG));

				}
			Counter_first ++;
		}

}
//	May 27, 2005    MMI-FIX-29869 x0018858 - End
#endif
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS           |
| STATE   : code                         ROUTINE : sms_init          |
+--------------------------------------------------------------------+

   PURPOSE : initialize for SMS management

*/


void sms_init(void)
{
  /*a0393213 compiler warnings removal - removed variable i*/

  TRACE_FUNCTION ("sms_init()");

  /* Setting the default SMS parameter */
  sms_type = MFW_NO_MESSAGE;
  mt_type = MFW_NO_MESSAGE;
  status_change_index = 0;
  deleting_index = 0;
  /*a0393213 OMAPS00109953 - SMS memory related variables updated*/
  status_change_memory=MFW_MEM_NOTPRESENT;
  deleting_memory=MFW_MEM_NOTPRESENT;
  smsReady = 0;
  phbReady = 0;
  sms_id.index = sms_id_wait.index = 0;
#if 0
  for (i=0; i<MAX_MESSAGES; i++)
  {
    msg_info[i].index = -1;
    msg_info[i].addr.number[0] = '\0';
    msg_info[i].addr.tag[0] = '\0';
    msg_info[i].addr.ton = MFW_TON_UNKNOWN;
    msg_info[i].addr.npi = MFW_NPI_UNKNOWN;
    msg_info[i].first_index = -1; /*SPR 2260*/
	msg_info[i].concat_status = MFW_SMS_NO_CONC;

  }
#endif
  /* PATCH-6363 VO 30.01.01 */
  /*a0393213 OMAPS00109953 - SMS memory related variables updated*/
/*This member indicates the preferred memory - the memory, which is considered first for saving the incoming messages/drafts*/
  memory.mem=(UBYTE)MFW_MEM_NOTPRESENT; 
  memory.used=0;
  memory.total=0;
  ffs_memory.mem = MFW_MEM_ME;
  ffs_memory.used = 0;
  ffs_memory.total = 0;
  sim_memory.mem = MFW_MEM_SIM;
  sim_memory.used = 0;
  sim_memory.total = 0;

  index_retrieve_state=MFW_SMS_INDEX_NOT_RETRIEVED;
  /* PATCH-6363 end */
}

//    x0018858 24 Mar, 2005 MMI-FIX-11321
//begin- MMI-FIX-11321
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS           |
| STATE   : code                         ROUTINE : sms_set_cind_values        |
+--------------------------------------------------------------------+

   PURPOSE : Set the Indication parameters.

*/

T_MFW sms_set_cind_values( T_ACI_MM_CIND_VAL_TYPE *cind_Settings)
{
	if(0 == cind_Settings)
		return MFW_SMS_FAIL;
		
		
	if(sAT_PlusCIND(CMD_SRC_LCL,CIND_SIGNAL_INDICATOR_LVL0,CIND_SMSFULL_INDICATOR_MEMFULL)NEQ AT_CMPL)
	{
		TRACE_EVENT(" The indication couldnt not be set");
		return MFW_SMS_FAIL;
	}
	return MFW_SMS_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS           |
| STATE   : code                         ROUTINE : sms_set_cmer_values        |
+--------------------------------------------------------------------+

   PURPOSE : Set the CMER parameters.

*/
T_MFW sms_set_cmer_values(T_ACI_MM_CMER_VAL_TYPE *cmer_Settings)
{
	if(0 == cmer_Settings)
		return MFW_SMS_FAIL;

	// ADDED BY RAVI - ACI CHANGES - 6-10-2005
	#if (BOARD==61)
		if(sAT_PlusCMER(CMD_SRC_LCL,CMER_MODE_2,(T_ACI_CMER_KEYP_TYPE)CMER_INDICATOR_2,
                                          (T_ACI_CMER_IND_TYPE)CMER_BFR_1, (T_ACI_CMER_BFR_TYPE)0)NEQ AT_CMPL)  /* typecast to remove warning Aug - 11 */
	#else	
		if(sAT_PlusCMER(CMD_SRC_LCL,CMER_MODE_2,CMER_INDICATOR_2,CMER_BFR_1)NEQ AT_CMPL)
	#endif
	// END ADDITION RAVI - 6-10-2005
	{
		TRACE_EVENT(" The indication couldnt not be set");
		return MFW_SMS_FAIL;
	}
	return MFW_SMS_OK;
}
//end. - MMI-FIX-11321

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS           |
| STATE   : code                         ROUTINE : sms_exit          |
+--------------------------------------------------------------------+

   PURPOSE : initialize for SMS management

*/

void sms_exit(void)
{
  TRACE_FUNCTION ("sms_exit()");
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS           |
| STATE   : code                         ROUTINE : sms_phbReady_ind  |
+--------------------------------------------------------------------+

   PURPOSE : finished initialize from the SIM card

*/

void sms_phbReady_ind(void)
{
  if (smsReady)
  {
    TRACE_FUNCTION("sms_phbReady_ind");

    f_update = MFW_SMS_NEW;
    TRACE_EVENT("all free");
    coll_sms_send = coll_sms_read = coll_sms_delete = coll_sms_req = MFW_SMS_FREE;
    sms_read_msg_info();
    smsReady = 0;
    phbReady = 0;
  }
  else
    phbReady = 1;
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS           |
| STATE   : code                         ROUTINE : rAT_sms_ready     |
+--------------------------------------------------------------------+

   PURPOSE : finished initialize from the SIM card

*/

void rAT_sms_ready(void)
{
    TRACE_FUNCTION("rAT_sms_ready()");

    if (phbReady)
    {
      f_update = MFW_SMS_NEW;
      sms_read_msg_info();
      smsReady = 0;
      phbReady = 0;
    }
    else
      smsReady = 1;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)              MODULE  : MFW_CB             |
| STATE   : code                        ROUTINE : rAT_SignalSMS      |
+--------------------------------------------------------------------+

  PURPOSE : handles rAT_SignalSMS call back

*/

GLOBAL void rAT_SignalSMS ( UBYTE state )
{
  TRACE_FUNCTION("rAT_SignalSMS()");
  switch (state)
  {
#ifdef FF_2TO1_PS
    case MNSMS_STATE_INITIALISING: initialising_flag = TRUE; break;
    case MNSMS_STATE_READY: initialising_flag = FALSE; break;
#else
	case SMS_STATE_INITIALISING: initialising_flag = TRUE; break;
  	case SMS_STATE_READY: initialising_flag = FALSE; break;
#endif
  }
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS           |
| STATE   : code                         ROUTINE : sms_create        |
+--------------------------------------------------------------------+

   PURPOSE : create SMS control

*/

T_MFW_HND sms_create(T_MFW_HND hWin, T_MFW_EVENT event, T_MFW_CB cbfunc)
{
  T_MFW_HDR *hdr;
  T_MFW_SMS *sms_para;
  MfwHdr *insert_status =0;
  
  TRACE_FUNCTION ("sms_create()");

  hdr      = (T_MFW_HDR *) mfwAlloc(sizeof(T_MFW_HDR));
  sms_para = (T_MFW_SMS *) mfwAlloc(sizeof(T_MFW_SMS));

  if (!hdr OR !sms_para)
  	{
    	TRACE_ERROR("ERROR: sms_create() Mem Alloc Failed.");
			
	   	if(hdr)
   			mfwFree((U8*)hdr,sizeof(MfwHdr));

   		if(sms_para)
   			mfwFree((U8*)sms_para,sizeof(T_MFW_SMS));
   		
	   	return FALSE;
    }
  /* initialisation of the handler */
  sms_para->emask   = event;
  sms_para->handler = cbfunc;

  hdr->data = sms_para;         /* store parameter in node */
  hdr->type = MFW_TYP_SMS;     /* store type of event handler */

  /* installation of the handler */

  insert_status =  mfwInsert((T_MFW_HDR *)hWin, hdr);/*SPR 1968, inserted inadvertently deleted function call*/
  
  if(!insert_status)
  	{
  		TRACE_ERROR("ERROR: sms_create() Failed to Install Handler. ");
   		mfwFree((U8*)hdr,sizeof(MfwHdr));
   		mfwFree((U8*)sms_para,sizeof(T_MFW_SIM)); 	
		return 0;
  	}
    return insert_status;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS           |
| STATE   : code                         ROUTINE : sms_delete        |
+--------------------------------------------------------------------+

   PURPOSE : delete SMS control

*/

T_MFW_RES sms_delete(T_MFW_HND h)
{
  TRACE_FUNCTION ("sms_delete()");

  if (!h OR !((T_MFW_HDR *)h)->data)
    return MFW_RES_ILL_HND;

  if (!mfwRemove((T_MFW_HDR *) h))
    return MFW_RES_ILL_HND;

  mfwFree((U8 *)(((T_MFW_HDR *) h)->data),sizeof(T_MFW_SMS));
  mfwFree((U8 *)h,sizeof(T_MFW_HDR));

 return MFW_RES_OK;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS           |
| STATE   : code                         ROUTINE : sms_signal        |
+--------------------------------------------------------------------+

   PURPOSE : send a event.

*/

void sms_signal(T_MFW_EVENT event, void * para)
{
	UBYTE temp = dspl_Enable(0);
  TRACE_FUNCTION ("sms_signal()");
 if (mfwSignallingMethod EQ 0)
  {
  /* focus is on a window */
  if (mfwFocus)
    /* send event to sim management handler if available */
    if (sms_sign_exec (mfwFocus, event, para))
	{
	  dspl_Enable(temp);
      return;
	}

  /* acutal focussed window is not available or has no
   * network management registration handler, then search
   * all nodes from the root. */
   if (mfwRoot)
     sms_sign_exec (mfwRoot, event, para);
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
        if (sms_sign_exec (h, event, para))
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
      sms_sign_exec (mfwRoot, event, para);
    }
	  dspl_Enable(temp);
      return;
 }


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS           |
| STATE   : code                         ROUTINE : sms_sign_exec     |
+--------------------------------------------------------------------+

   PURPOSE : Send a signal if there is an SMS management handler.

*/

BOOL sms_sign_exec (T_MFW_HDR * cur_elem,
                    T_MFW_EVENT event,
                    T_MFW_SMS_PARA * para)
{
	/***************************Go-lite Optimization changes Start***********************/
	//Aug 16, 2004    REF: CRR 24323   Deepa M.D
	TRACE_EVENT_P1("sms_sign_exec(): %d", event);
	/***************************Go-lite Optimization changes end***********************/
  while (cur_elem)
  {
    /* event handler is available */
    if (cur_elem->type EQ MFW_TYP_SMS)
    {
      T_MFW_SMS * sms_data;
      /* handler is SMS management handler */
      sms_data = (T_MFW_SMS *)cur_elem->data;
      if (sms_data->emask & event)
      {
        /* event is expected by the call back function */
        sms_data->event = event;
        switch (event)
        {
          case E_SMS_MO_AVAIL:
            memcpy (&sms_data->para.index, para, sizeof (UBYTE));
            break;
          case E_SMS_CMD_AVAIL:
            memcpy (&sms_data->para.index, para, sizeof (UBYTE));
            break;
          case E_SMS_SAVE_AVAIL:
            memcpy (&sms_data->para.index, para, sizeof (UBYTE));
            break;
          case E_SMS_MT:
            memcpy (&sms_data->para.sms_mt, para, sizeof (T_MFW_SMS_MT));
            break;
          case E_SMS_MO:
            memcpy (&sms_data->para.sms_mo, para, sizeof (T_MFW_SMS_MO));
            break;
          case E_SMS_CB:
            memcpy (&sms_data->para.sms_cb, para, sizeof (T_MFW_SMS_CB));
            break;
          case E_SMS_CB_RECEIVED:
            memcpy (&sms_data->para.sms_id, para, sizeof (T_MFW_SMS_ID));
            break;
          case E_SMS_MT_RECEIVED:
            memcpy (&sms_data->para.sms_id, para, sizeof (T_MFW_SMS_ID));
            break;
          case E_SMS_STATUS:
            memcpy (&sms_data->para.sms_status, para, sizeof (T_MFW_SMS_STATUS));
            break;
          case E_SMS_MEM:
            memcpy (&sms_data->para.sms_mem, para, sizeof (T_MFW_SMS_MEM_INFO));
            break;
          case E_SMS_OK:
/*SPR 2653, no need for conditional compilation round if, removing it*/
			  // EF we only copy if non null
		  if (para != NULL)
			  memcpy (&sms_data->para.cmd_ind, para, sizeof (UBYTE));
            break;
          case E_SMS_ERR:
            memcpy (&sms_data->para.cmd_ind, para, sizeof (UBYTE));
            break;
#if defined (FF_WAP) && defined (FF_GPF_TCPIP)
            /* SPR#2086 */
            case E_SMS_WAP_MT_RECEIVED:
              memcpy (&sms_data->para.sms_mt, para, sizeof (T_MFW_SMS_MT));
              break;
            case E_SMS_WAP_MO_SENT:
              memcpy (&sms_data->para.index, para, sizeof (UBYTE));
              break;
            case E_SMS_WAP_ERR:
              memcpy (&sms_data->para.index, para, sizeof (UBYTE));
              break;
#endif
        }

        /* if call back defined, call it */
        if (sms_data->handler)
        {
          // PATCH LE 06.06.00
          // store current mfw elem
          current_mfw_elem = cur_elem;
          // END PATCH LE 06.06.00
          if ((*(sms_data->handler)) (sms_data->event, (void *)&sms_data->para))
          {
            TRACE_EVENT("send SMS event");
            return TRUE;
          }
        }
      }
    }
    cur_elem = cur_elem->next;
  }

  return FALSE;
}


/*
+-------------------------------------------------------------------+
| PROJECT : GSM-PS (6147)         MODULE  : MFW_PHB                 |
| STATE   : code                  ROUTINE : sms_read_msg_info       |
+-------------------------------------------------------------------+

  PURPOSE : This function is used to read total information about SMS.
*/

void sms_read_msg_info()
{
    int i;

    TRACE_FUNCTION("sms_read_msg_info()");

#ifdef FF_MMI_SMS_DYNAMIC
    for (i=0; i<g_max_messages; i++)
#else
    for (i=0; i<MAX_MESSAGES; i++)
#endif
    {
        msg_info[i].index = -1;
        msg_info[i].addr.number[0] = '\0';
        msg_info[i].addr.tag[0] = '\0';
        msg_info[i].addr.ton = MFW_TON_UNKNOWN;
        msg_info[i].addr.npi = MFW_NPI_UNKNOWN;
	  msg_info[i].msg_ref=0;
	  /*a0393213 OMAPS00109953*/
	  msg_info[i].mem=MFW_MEM_NOTPRESENT;

    }

    /*a0393213 OMAPS00109953 - set preferred memory before calling sAT_PlusCMGL
	Retrieve SMS indexes in SIM first*/	
    if(sms_set_pref_memory(MFW_MEM_SIM)!=MFW_SMS_OK)
    	{
    	TRACE_ERROR("sms_read_msg_info():Not able to set pref memory");
	return;
    }

    f_update = MFW_SMS_NEW;
    if (sAT_PlusCMGL (CMD_SRC_LCL, SMS_STAT_All,
#ifdef FF_2TO1_PS
                      0, (T_ACI_SMS_READ)MNSMS_READ_PREVIEW) NEQ AT_EXCT)  /* typecast to remove warning Aug - 11 */
#else
                      0, SMS_READ_Preview) NEQ AT_EXCT)/*a0393213 warnings removal-READ_PREVIEW changed to SMS_READ_Preview*/
#endif
    {
        TRACE_EVENT("sAT_PlusCMGL error");
        f_update = MFW_SMS_FREE;
    }
    else     /*a0393213 OMAPS00109953 - update the state machine*/
    	{
    	index_retrieve_state=MFW_SMS_SIM_INDEX_RETRIEVING;
    }
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)  MODULE  : MFW_SMS                  |
| STATE   : code                  ROUTINE : rAT_PlusCMGL             |
+--------------------------------------------------------------------+

  PURPOSE : Response of read SMS list from ACI
*/

void rAT_PlusCMGL ( T_ACI_CMGL_SM * smLst )
{
  T_SMS_TYPE received_sms_type;
  T_CONC_ASSEMBLY_RETURN concatenated_status=CONC_ERR_UNKN;
#ifdef TI_PS_FF_CONC_SMS  
  T_SM_DATA_EXT conc_data_ext={NULL,0};
#endif /*TI_PS_FF_CONC_SMS*/
  
  TRACE_FUNCTION ("rAT_PlusCMGL()");
  /*a0393213 OMAPS00109953 - third parameter added for SMS_getSMSType()*/
  if(initialising_flag == TRUE)
  {
    received_sms_type = SMS_getSMSType( &smLst->udh, smLst->adress, MODE3);
  }
  else
  {
    received_sms_type = SMS_getSMSType( &smLst->udh, smLst->adress, MODE2);
  }
  TRACE_EVENT_P1("SMS Type is = %d", received_sms_type);
 TRACE_EVENT_P1("SMS message status:%d", smLst->stat);

#ifdef TI_PS_FF_CONC_SMS
  if (received_sms_type == CONCATE)
	  {
	  if(initialising_flag == TRUE)
	  	{
             /*a0393213 OMAPS00109953 - memory added as parameter*/
		concatenated_status =concSMS_Collect(&conc_data_ext, smLst, FALSE, SMS_STOR_NotPresent);
	  	}
	  else
	  	{
             /*a0393213 OMAPS00109953*/
		concatenated_status =concSMS_Collect(&conc_data_ext, smLst, TRUE, sms_mem_cvt(memory.mem));
	  	}
		TRACE_EVENT_P2("conc status: %d, conc length: %d", concatenated_status, conc_data_ext.len);

  
	  }
// July 27, 2006    DR: OMAPS00085347 x0021334
// Free the allocated memory after concatenated SMS has been read. 
  if (concatenated_status EQ CONC_COMPLETED)
  {
      if (conc_data_ext.data != NULL)
      {
           MFREE (conc_data_ext.data);
      }
  }
#endif /*TI_PS_FF_CONC_SMS*/ 	
  
  /*a0393213 OMAPS00109953 - memory added as parameter*/
  sms_store_new_msg_info(smLst->msg_ref, smLst, TRUE/*MC SPR 2260*/, concatenated_status,received_sms_type,(T_MFW_SMS_MEM)memory.mem);  
	

			f_update = MFW_SMS_NEW;
{
	TRACE_EVENT_P4("CMGL Msg table: %d,%d,%d, %d", msg_info[0].index, msg_info[1].index,msg_info[2].index,msg_info[3].index);
	TRACE_EVENT_P4("CMGL Msg table: %d,%d,%d, %d", msg_info[4].index, msg_info[5].index,msg_info[6].index,msg_info[7].index);
}
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)  MODULE  : MFW_SMS                  |
| STATE   : code                  ROUTINE : sms_update_ready         |
+--------------------------------------------------------------------+

  PURPOSE : message list update is ready
*/

void sms_update_ready(void)
{
  TRACE_FUNCTION("sms_update_ready()");

    sms_signal(E_SMS_MT_RECEIVED, &sms_id);
  sms_id.index = 0;

  if (memory.used == memory.total)
    sms_signal(E_SMS_MEM_FULL, 0);
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)  MODULE  : MFW_SMS                  |
| STATE   : code                  ROUTINE : sms_update_msg_info      |
+--------------------------------------------------------------------+

  PURPOSE : update the SMS information list
*/

void sms_update_msg_info(T_MFW_SMS_STAT stat)
{
    int i;

    TRACE_FUNCTION("sms_update_msg_info()");
#ifdef FF_MMI_SMS_DYNAMIC
    for (i=0; i<g_max_messages; i++)
#else
    for (i=0; i<MAX_MESSAGES; i++)
#endif
    {
        if (msg_info[i].index EQ status_change_index && 
       	msg_info[i].mem EQ status_change_memory) /*a0393213 OMAPS00109953 - memory also taken into consideration*/
        	{
            	msg_info[i].stat = stat;
        	}
    }

    status_change_index = 0;
    /*a0393213 OMAPS00109953 - invalidate the variable*/
    status_change_memory=MFW_MEM_NOTPRESENT;
}


/*
+-------------------------------------------------------------------+
| PROJECT : GSM-PS (6147)         MODULE  : MFW_PHB                 |
| STATE   : code                  ROUTINE : sms_mem_cvt             |
+-------------------------------------------------------------------+

  PURPOSE : This function is used to convert the type of memory
            used by MFW to the type of memory used by ACI.
*/
LOCAL T_ACI_SMS_STOR sms_mem_cvt(UBYTE mem)
{
  switch ( mem )
  {
    case ( MFW_MEM_ME ):  return SMS_STOR_Me;
    case ( MFW_MEM_SIM ): return SMS_STOR_Sm;
    default:          return SMS_STOR_NotPresent;
  }
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS           |
| STATE   : code                         ROUTINE : sms_set_mt_ind    |
+--------------------------------------------------------------------+

   PURPOSE :  MMI selects, how the receiving of new messages
              from the network is indicated.

*/

void sms_set_mt_ind(T_MFW_SMS_MT_IND mt_ind)
{
  T_ACI_RETURN ret = AT_CMPL;
  TRACE_FUNCTION("sms_set_mt_ind()");

  switch (mt_ind)
  {
    case MT_IND_IDX:
      TRACE_EVENT("MT_IND_IDX");
      ret = sAT_PlusCNMI ( CMD_SRC_LCL,
                     CNMI_MT_SmsDeliverInd,
                     CNMI_BM_NotPresent,
                     CNMI_DS_SmsStatRpt );
      break;

    case MT_IND_MSG:
      TRACE_EVENT("MT_IND_MSG");
      ret = sAT_PlusCNMI ( CMD_SRC_LCL,
                     CNMI_MT_SmsDeliver,
                     CNMI_BM_NotPresent,
                     CNMI_DS_SmsStatRpt );
      break;

    case MT_IND_NO_IDX:
      TRACE_EVENT("MT_IND_NO_IDX");
      ret = sAT_PlusCNMI ( CMD_SRC_LCL,
                     CNMI_MT_NoSmsDeliverInd,
                     CNMI_BM_NotPresent,
                     CNMI_DS_SmsStatRpt );
      break;

    case MT_CB_IDX:
      TRACE_EVENT("MT_CB_IDX");
      ret = sAT_PlusCNMI ( CMD_SRC_LCL,
                     CNMI_MT_NotPresent,
                     CNMI_BM_CbmInd,
                     CNMI_DS_NotPresent );
      break;

    case MT_CB_MSG:
      TRACE_EVENT("MT_CB_MSG");
      ret = sAT_PlusCNMI ( CMD_SRC_LCL,
                     CNMI_MT_NotPresent,
                     CNMI_BM_Cbm,
                     CNMI_DS_NotPresent );
      break;

    case MT_CB_NO_IDX:
      TRACE_EVENT("MT_CB_NO_IDX");
      ret = sAT_PlusCNMI ( CMD_SRC_LCL,
                     CNMI_MT_NotPresent,
                     CNMI_BM_NoCbmInd,
                     CNMI_DS_NotPresent );
      break;

    /* Marcus: Issue 1170: 07/10/2002: Start */
    case MT_IND_SRR_ON:
      TRACE_EVENT("MT_IND_SRR_ON");
      ret = sAT_PlusCNMI ( CMD_SRC_LCL,
                     CNMI_MT_NotPresent, /*a0393213 compiler warnings removal - CNMI_BM_NotPresent changed to CNMI_MT_NotPresent*/
                     CNMI_BM_NotPresent,
                     CNMI_DS_SmsStatRpt );
      break;

    case MT_IND_SRR_OFF:
      TRACE_EVENT("MT_IND_SRR_OFF");
      ret = sAT_PlusCNMI ( CMD_SRC_LCL,
                     CNMI_MT_NotPresent, /*a0393213 compiler warnings removal - CNMI_BM_NotPresent changed to CNMI_MT_NotPresent*/
                     CNMI_BM_NotPresent,
                     CNMI_DS_NoSmsStatRpt );
      break;
    /* Marcus: Issue 1170: 07/10/2002: End */

    default:
      TRACE_EVENT_P1("default: mt_ind = %d", mt_ind);
      break;
  }

  if (ret != AT_CMPL)
    TRACE_EVENT_P1("sAT_PlusCNMI: %d", ret);
}




/*
+-------------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS                |
| STATE   : code                         ROUTINE : sms_change_read_status |
+-------------------------------------------------------------------------+

   PURPOSE :  Change SMS status from MFW_SMS_UNREAD to MFW_SMS_READ

GW-SPR#779-Modified code to abort if status has already been updated or if no match is found

*/
/*a0393213 OMAPS00109953 - memory added as parameter*/
T_MFW sms_change_read_status(SHORT index, T_MFW_SMS_MEM sms_memory)
{
    SHORT i;
    //UBYTE fg;
    T_ACI_RETURN ret;
    TRACE_FUNCTION("sms_change_read_status()");

    fg = 0;
#ifdef FF_MMI_SMS_DYNAMIC
    for (i=0; i<g_max_messages; i++)
#else
    for (i=0; i<MAX_MESSAGES; i++)
#endif
    {
        /*a0393213 OMAPS00109953 - memory also taken into consideration*/
        if (msg_info[i].index == index	&& msg_info[i].mem == sms_memory)    
        {
            if ((msg_info[i].stat == MFW_SMS_UNREAD) ||
               (msg_info[i].stat == MFW_SMS_STOR_UNSENT))
            {
            	fg = 1;
            }
            else
            {
            	/* status is not unread/unsent - no need to change it*/
            	return (1);
            }
            /* Stop search as soon as the index matches */
            break;
        }
    }

/* This change is no completely right, because the SIM will be always updated,
but it makes possible that the read message doesn't come up as new every time
that the mobile is switched on.
GW - Replaced code that returns if no match is found
*/

    if (!fg)
    {
        TRACE_EVENT("sms_change_read_status No index match-ERROR");
        return 0;
    }

     /*a0393213 OMAPS00109953 - set preferred memory before calling sAT_PlusCMGR*/	
     if(sms_set_pref_memory(sms_memory)!=MFW_SMS_OK)
     	{
     	TRACE_ERROR("sms_change_read_status():Not able to set pref memory");
 	return 0;
     	}
 
    sms_type = MFW_NO_MESSAGE;/* SPR 2260 Don't do anything scary in rAT_PlusCMGR callback func*/
/*CONQUEST 5991- MC- Changed third parameter in function call*/
    ret = sAT_PlusCMGR(CMD_SRC_LCL, (UBYTE)index, SMS_READ_StatusChange);

    if (ret NEQ AT_CMPL AND ret NEQ AT_EXCT)
    {
        TRACE_EVENT("sms_change_read_status ERROR");
        fg = 0;
        return 0;
    }
    /*CONQUEST 5991- MC- Added line to set current message to "read" status*/
#ifdef FF_MMI_SMS_DYNAMIC /*a0393213 lint warnings removal - Possible access of out-of-bounds pointer*/
   if(i<g_max_messages)
#else
   if(i<MAX_MESSAGES)
#endif
   {
    if ( msg_info[i].stat == MFW_SMS_STOR_UNSENT)
    	msg_info[i].stat = MFW_SMS_STOR_SENT;
    else
    	msg_info[i].stat = MFW_SMS_READ;//MC
    }
    status_change_index = index;
    /*a0393213 OMAPS00109953 - set the global variable*/
    status_change_memory = sms_memory;
    return 1;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS           |
| STATE   : code                         ROUTINE : sms_submit        |
+--------------------------------------------------------------------+

   PURPOSE :  Send a mobile originated short massage.

   GW 09/10/01 - Modified code to always copy entire (possible) message length
*/

#if defined (FF_WAP) && defined (FF_GPF_TCPIP)
/* NM, 27.5.04

the sms_submit function with the parameters : addr_type and sca 
has been taken out as it does not send SMS ?!

*/
#endif

#ifdef NO_ASCIIZ
T_MFW sms_submit(T_MFW_SMS_MO_TYPE type,
                 CHAR *dest_addr,
                 UBYTE *msg,
                 USHORT msg_len,
                 CHAR *sca)
#else
    #if defined (FF_WAP) && defined (FF_GPF_TCPIP)
T_MFW sms_submit(T_MFW_SMS_MO_TYPE type,
                 CHAR *dest_addr,
                 T_MFW_PHB_TON addr_type,
                 UBYTE *msg,
                 USHORT msg_len,
                 CHAR *sca,
                 T_MFW_PHB_TON sc_type)
    #else
T_MFW sms_submit(T_MFW_SMS_MO_TYPE type,
                 CHAR *dest_addr,

                 UBYTE *msg,
                 USHORT msg_len,
                 CHAR *sca)
// sbh - added length to the above definition, since '@' characters stored as NULL & will stop
// strlen from working properly.

    #endif
#endif
{
#ifdef TI_PS_FF_CONC_SMS
  T_SM_DATA_EXT	conc_data_ext;
#else /*TI_PS_FF_CONC_SMS*/
  T_ACI_SM_DATA conc_data_ext;
#endif  /*TI_PS_FF_CONC_SMS*/

  SHORT            rp;
#if defined (FF_WAP) && defined (FF_GPF_TCPIP)
  T_ACI_TOA			toa;
  T_ACI_TOA			tosca;
#endif
  //Mar 22 2007 ER:OMAPS00113882 x0061088 (Prachi) 
   T_ACI_TOA	set_toa;
   int addr_type;
   T_ACI_RETURN ACI_MFW_return;

  TRACE_FUNCTION ("sms_submit()");
//March 2, 2005    REF: CRR 11536 x0018858
//Modified the condition for setting the reply path.
  if (reply_path_bit ==1)
  	{
    rp = 1;   /* reply path = 0 */
  	}
  else
  	{
    rp = 0;  /* reply path = default */
  	}

#ifdef TI_PS_FF_CONC_SMS
 conc_data_ext.data = msg;
 conc_data_ext.len = msg_len;
#else /*TI_PS_FF_CONC_SMS*/
conc_data_ext.len = (UBYTE)msg_len;
 memcpy(conc_data_ext.data, msg, conc_data_ext.len);
#endif /*TI_PS_FF_CONC_SMS*/

#if defined (FF_WAP) && defined (FF_GPF_TCPIP)
//NM  toa.ton = phb_ncvtTon(addr_type);
//NM  toa.npi = NPI_IsdnTelephony;
//NM  tosca.ton = phb_ncvtTon(sc_type);
//NM  tosca.npi = NPI_IsdnTelephony;
#endif

//Mar 22 2007 ER:OMAPS00113882 x0061088 (Prachi)
//setting type of address.
  addr_type = is_alphanumeric_addr(dest_addr);


//Mar 22 2007 ER:OMAPS00113882 x0061088 (Prachi)
//sending toa instead of NULL
  if ( addr_type == TRUE )
  {
  	    set_toa.ton = TON_Alphanumeric;
        set_toa.npi = NPI_NotPresent;
        ACI_MFW_return = sAT_PlusCMGS(CMD_SRC_LCL, dest_addr, &set_toa, &conc_data_ext, sca, NULL, rp); 

  }
  else 
     ACI_MFW_return = sAT_PlusCMGS(CMD_SRC_LCL, dest_addr, NULL, &conc_data_ext, sca, NULL, rp);

  if(ACI_MFW_return != AT_EXCT)
  #if defined (FF_WAP) && defined (FF_GPF_TCPIP)
	//NM  if (sAT_PlusCMGS(CMD_SRC_LCL, dest_addr, &toa, &conc_data_ext, sca, &tosca, rp) NEQ AT_EXCT)
  #endif
       return MFW_SMS_FAIL;
  return MFW_SMS_OK; 

}

#if defined (FF_WAP) && defined (FF_GPF_TCPIP)


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS           |
| STATE   : code                         ROUTINE : sms_submit        |
+--------------------------------------------------------------------+

   PURPOSE :  Send a mobile originated short message for WAP.
     - Sets a flag so that success or failure of send can be reported
       to MMI.
*/
T_MFW sms_submit_wap_sms(T_MFW_SMS_MO_TYPE type,
                 CHAR *dest_addr,
                 T_MFW_PHB_TON addr_type,
                 UBYTE *msg,
                 USHORT msg_len,
                 CHAR *sca,
                 T_MFW_PHB_TON sc_type)
{
	TRACE_FUNCTION ("sms_submit_wap_sms()");
	
	sms_wap_sent_flag = TRUE;

/* NM, 27.5.04

the sms_submit function with the parameters : addr_type and sca 
has been taken out as it does not send SMS ?!

*/
	
//NM	return sms_submit(type, dest_addr, addr_type, msg, msg_len, sca, sc_type);
}

#endif /* #if defined (FF_WAP) && defined (FF_GPF_TCPIP) */

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS           |
| STATE   : code                         ROUTINE : sms_stored_submit |
+--------------------------------------------------------------------+

   PURPOSE :  Send a stored mobile originated short massage.

*/

T_MFW sms_stored_submit(CHAR *dest_addr, UBYTE index)
{
  TRACE_FUNCTION ("sms_stored_submit()");

  if (sAT_PlusCMSS(CMD_SRC_LCL, index, dest_addr, NULL) NEQ AT_EXCT)
  {
    return MFW_SMS_FAIL;
  }
  return MFW_SMS_OK;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS           |
| STATE   : code                         ROUTINE : rAT_PlusCMSS      |
+--------------------------------------------------------------------+

   PURPOSE :  Response of Send a stored mobile originated
              short massage.

*/

void rAT_PlusCMSS (UBYTE mr, UBYTE numSeg)
{
    TRACE_FUNCTION ("rAT_PlusCMSS()");

    sms_update_msg_info(MFW_SMS_STOR_SENT);
    sms_signal(E_SMS_MO_AVAIL, &mr);

}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS           |
| STATE   : code                         ROUTINE : rAT_PlusCMT       |
+--------------------------------------------------------------------+

   PURPOSE :  Receive a new short message.

*/

void rAT_PlusCMT(T_ACI_CMGL_SM*  sm)
{	T_SMS_TYPE received_sms_type;
	T_CONC_ASSEMBLY_RETURN concatenated_status;
#ifdef TI_PS_FF_CONC_SMS	
	T_SM_DATA_EXT conc_data_ext;
#endif /*TI_PS_FF_CONC_SMS*/
	int i;
  TRACE_FUNCTION ("rAT_PlusCMT()");

  TRACE_EVENT_P1("CMT, sm->data.len: %d", sm->data.len);
  /*a0393213 OMAPS00109953 - new parameter added for SMS_getSMSType()*/
  received_sms_type = SMS_getSMSType( &sm->udh, sm->adress, MODE1);

#ifdef TI_PS_FF_CONC_SMS
  if (received_sms_type == CONCATE)
  {
       /*a0393213 OMAPS00109953 - memory added as parameter*/
  	concatenated_status =concSMS_Collect(&conc_data_ext, sm, FALSE, SMS_STOR_NotPresent);
	TRACE_EVENT_P2("conc status: %d, conc length: %d", concatenated_status, conc_data_ext.len);
	for(i=0;i<conc_data_ext.len;i++)
		{
			TRACE_EVENT_P2("%02x, %c", conc_data_ext.data[i],conc_data_ext.data[i]);
		}
		switch (concatenated_status)
		{
			case CONC_ERR_UNKN:
				TRACE_EVENT("Concatenated SMS error");break;
			case CONC_ERR_BUF_FULL:
				TRACE_EVENT("Concatented SMS buffer Full Error");break;
			case CONC_CONTINUED: //not last SMS
				TRACE_EVENT("Concatenated Continued!");
				break;
			case CONC_COMPLETED:
			{
				  TRACE_EVENT("COncatenation Completed!");
				  memset(&sms_mt, 0, sizeof(sms_mt));

#ifdef NO_ASCIIZ
				  memcpy(sms_mt.orig_addr.tag,
				         sm->alpha.data,
				         sm->alpha.len);
				  sms_mt.orig_addr.len = sm->alpha.len;
#else
				  phb_Alpha2Gsm(sm->alpha, (UBYTE *)sms_mt.orig_addr.tag);
#endif
				  strcpy(sms_mt.orig_addr.number, sm->adress);
				  sms_mt.orig_addr.ton =
				      phb_cvtTon(sm->toa.ton);
				  sms_mt.orig_addr.npi = (T_MFW_PHB_NPI)phb_cvtNpi(sm->toa.npi);
				  if (sm->tosca.ton == TON_International)
				    sprintf(sms_mt.sc_addr, "+%s", sm->sca);
				  else
				    strcpy(sms_mt.sc_addr, sm->sca);
				  sms_mt.prot_id = (UBYTE)sm->pid;
				  sms_mt.dcs     = (UBYTE)sm->dcs & 0xFC;	/* Ignore message class */

				  sms_mt.rp      = sm->fo >> 7 & 1;

				/* SBH */
				if (sm->dcs & 0x10)		/* Bit 5 indicates that we should get message class from bits 0 & 1 */
				{
					sms_mt.ti = sm->dcs & 3;
				}
				else					/* Otherwise, set to default value */
				{
					sms_mt.ti = 1;
				}
				TRACE_EVENT_P2("dcs: %d, ti: %d", sms_mt.dcs, sms_mt.ti);
				/* SBH end */

				  memcpy((char *)sms_mt.sctp.year, (char *)sm->scts.year, MAX_SCTP_DIGITS);
				  memcpy((char *)sms_mt.sctp.month, (char *)sm->scts.month, MAX_SCTP_DIGITS);
				  memcpy((char *)sms_mt.sctp.day, (char *)sm->scts.day, MAX_SCTP_DIGITS);
				  memcpy((char *)sms_mt.sctp.hour, (char *)sm->scts.hour, MAX_SCTP_DIGITS);
				  memcpy((char *)sms_mt.sctp.minute, (char *)sm->scts.minute, MAX_SCTP_DIGITS);
				  memcpy((char *)sms_mt.sctp.second, (char *)sm->scts.second, MAX_SCTP_DIGITS);
				  sms_mt.sctp.timezone = sm->scts.timezone;

				/* copy length of whole concat SMS*/
				  sms_mt.msg_len = conc_data_ext.len;
				  memset(sms_mt.sms_msg, 0, sm->data.len);
				/*copy all data from concat SMS*/
				  memcpy(sms_mt.sms_msg, (char*)conc_data_ext.data, conc_data_ext.len);
				  sms_mt.udh_len = sm->udh.len;
				  memcpy((char *)sms_mt.udh, (char *)sm->udh.data, sm->udh.len);
		
				  /*SPR#1408 - DS - Now done at end of function
				   *sms_signal(E_SMS_MT, &sms_mt);
				   */

  			}

  		}
  	}
  else
#endif /*TI_PS_FF_CONC_SMS*/
  {

		
	  memset(&sms_mt, 0, sizeof(sms_mt));

#ifdef NO_ASCIIZ
	  memcpy(sms_mt.orig_addr.tag,
	         sm->alpha.data,
	         sm->alpha.len);
	  sms_mt.orig_addr.len = sm->alpha.len;
#else
	  phb_Alpha2Gsm(sm->alpha, (UBYTE *)sms_mt.orig_addr.tag);
#endif
	  strcpy(sms_mt.orig_addr.number, sm->adress);
	  sms_mt.orig_addr.ton =
	      phb_cvtTon(sm->toa.ton);
	  sms_mt.orig_addr.npi = (T_MFW_PHB_NPI)phb_cvtNpi(sm->toa.npi);
	  if (sm->tosca.ton == TON_International)
	    sprintf(sms_mt.sc_addr, "+%s", sm->sca);
	  else
	    strcpy(sms_mt.sc_addr, sm->sca);
	  sms_mt.prot_id = (UBYTE)sm->pid;
	  sms_mt.dcs     = (UBYTE)sm->dcs & 0xFC;	/* Ignore message class */
	  sms_mt.rp      = sm->fo >> 7 & 1;
	/* PATCH JPS 04.10 END */

	/* SBH */
	if (sm->dcs & 0x10)		/* Bit 5 indicates that we should get message class from bits 0 & 1 */
	{
		sms_mt.ti = sm->dcs & 3;
	}
	else					/* Otherwise, set to default value */
	{
		sms_mt.ti = 1;
	}
	TRACE_EVENT_P2("dcs: %d, ti: %d", sms_mt.dcs, sms_mt.ti);
	/* SBH end */

	  memcpy((char *)sms_mt.sctp.year, (char *)sm->scts.year, MAX_SCTP_DIGITS);
	  memcpy((char *)sms_mt.sctp.month, (char *)sm->scts.month, MAX_SCTP_DIGITS);
	  memcpy((char *)sms_mt.sctp.day, (char *)sm->scts.day, MAX_SCTP_DIGITS);
	  memcpy((char *)sms_mt.sctp.hour, (char *)sm->scts.hour, MAX_SCTP_DIGITS);
	  memcpy((char *)sms_mt.sctp.minute, (char *)sm->scts.minute, MAX_SCTP_DIGITS);
	  memcpy((char *)sms_mt.sctp.second, (char *)sm->scts.second, MAX_SCTP_DIGITS);
	  sms_mt.sctp.timezone = sm->scts.timezone;

	  sms_mt.msg_len = sm->data.len;
	  memcpy(sms_mt.sms_msg, (char *)sm->data.data, sm->data.len);
	  sms_mt.udh_len = sm->udh.len;
	  memcpy((char *)sms_mt.udh, (char *)sm->udh.data, sm->udh.len);
	  
	  /*SPR#1408 - DS - Now done at end of function
	    *sms_signal(E_SMS_MT, &sms_mt);
	    */
  	}
  
  /*MC SPR 1364, need to call this function to update CPHS voicemail data*/ 
 /*SPR#1408 - DS - Modified to store voicemail status in SMS structure. Port of 1669 */
  if (sms_check_voice_mail(sm->pid, sm->dcs, sm->adress,
           &sm->toa, &sm->udh) == MFW_SMS_VOICE)
  {
	sms_mt.voicemail = TRUE;
	TRACE_EVENT("MT SMS is a voicemail message");
  }
  else
  {
	sms_mt.voicemail = FALSE;
  }
  sms_signal(E_SMS_MT, &sms_mt);
  
}

#if defined (FF_WAP) && defined (FF_GPF_TCPIP)
#define SMS_PUSH_PDU_OFFSET	1
#define PUSH_METHOD_PDU	0x06

static UBYTE *MMS_pdu=NULL;
static UBYTE id_no,seq_no;
static int pdu_len;

void redirectMMSNotification(T_ACI_CMGL_SM * sm)
{
	int header_len;
	char buf[80];
	int idx;

	TRACE_FUNCTION ("redirectMMSNotification()");

	if(MMS_pdu)
	{
		if(id_no!=sm->udh.data[8] || sm->udh.data[10]!=2)	/*had lost packet*/
		{
            TRACE_EVENT("-----MMS_pdu something------");
			mfwFree(MMS_pdu,MAX_SM_LEN<<1);
			MMS_pdu=NULL;
		}
		else
		{
			memcpy(MMS_pdu+pdu_len, sm->data.data, sm->data.len);

			TRACE_EVENT("Second SMS been recorded");
			TRACE_EVENT_P3("bgein=%x,end=%x,len=%d",MMS_pdu[pdu_len],MMS_pdu[pdu_len+sm->data.len-1],sm->data.len);

			pdu_len+=(sm->data.len);

			/*deliver PDU to WAP*/
			TRACE_EVENT("Deliver PDU to WAP");

/*			MWSmsNotification.data=MMS_pdu;
			MWSmsNotification.data_length=pdu_len;
			M_MMS_WAP_SMS_NOTIFICATION_IND(&MWSmsNotification);
*/
			//WMPush.data=MMS_pdu;
			//WMPush.data_length=pdu_len;
			//WMPush.is_sms_bearer=TRUE;
			//M_WAP_MMS_PUSH_IND(&WMPush);

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

			mfwFree(MMS_pdu,MAX_SM_LEN<<1);
			MMS_pdu=NULL;
			return;
		}
	}

	id_no=sm->udh.data[8];
	seq_no=sm->udh.data[10];

	if(seq_no!=1){
		return;
	    }

	/*identify pdu*/
	if(sm->data.data[SMS_PUSH_PDU_OFFSET]!=PUSH_METHOD_PDU){
		return;
	        }

	if(!(MMS_pdu=mfwAlloc(MAX_SM_LEN<<1)))
		return;

	header_len=sm->data.data[SMS_PUSH_PDU_OFFSET+1];

	pdu_len=sm->data.len-(header_len+1)-(SMS_PUSH_PDU_OFFSET+1);	
	memcpy(MMS_pdu,sm->data.data+SMS_PUSH_PDU_OFFSET+2+header_len,pdu_len);

	TRACE_EVENT("First SMS been recorded");
	TRACE_EVENT_P3("bgein=%x,end=%x,len=%d",MMS_pdu[0],MMS_pdu[pdu_len-1],pdu_len);
}


#endif /* #if defined (FF_WAP) && defined (FF_GPF_TCPIP) */
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS           |
| STATE   : code                         ROUTINE : rAT_PlusCMTI      |
+--------------------------------------------------------------------+

   PURPOSE :  Receive a new short message identifier.

*/
#if defined (FF_WAP) && defined (FF_GPF_TCPIP)
#else
void rAT_PlusCMTI(T_ACI_SMS_STOR mem, UBYTE index, T_ACI_CMGL_SM* sm)
{
/* MZ 2260 Handset displays Message Full on receiving an SMS. */
  //T_MFW_MEM_INFO memory;
#ifndef MMI_LITE
 // T_ACI_RETURN ret;   // RAVI
//  int i, j; // RAVI
#endif
  T_SMS_TYPE received_sms_type;
  T_CONC_ASSEMBLY_RETURN concatenated_status=CONC_ERR_UNKN;
#ifdef TI_PS_FF_CONC_SMS  
  T_SM_DATA_EXT conc_data_ext={NULL,0};
#endif /*TI_PS_FF_CONC_SMS*/

 if (initialising_flag == FALSE)
  	{
  		/* Ignore CMTI if initialising */
  		 	

	  TRACE_FUNCTION ("rAT_PlusCMTI()");
	  TRACE_EVENT_P1("f_update is = % in CMTI", f_update);

	   /*a0393213 OMAPS00109953 - new parameter added to SMS_getSMSType()*/
         if(initialising_flag == TRUE)
         {
           received_sms_type = SMS_getSMSType( &sm->udh, sm->adress, MODE3);
         }
         else
         {
           received_sms_type = SMS_getSMSType( &sm->udh, sm->adress, MODE2);
         }
	  TRACE_EVENT_P1("SMS Type is = %d", received_sms_type);
	  TRACE_EVENT_P2("msg_ref: %d, index: %d", sm->msg_ref, index);
 
#ifdef TI_PS_FF_CONC_SMS
	  if (received_sms_type == CONCATE)
		  {
		  if(initialising_flag == TRUE)
		  	{
		  	/*a0393213 OMAPS00109953 - memory added as parameter*/
		  	concatenated_status =concSMS_Collect(&conc_data_ext, sm, FALSE, SMS_STOR_NotPresent);
		  	}
		  else
		  	{
			/*a0393213 OMAPS00109953*/
		  	concatenated_status =concSMS_Collect(&conc_data_ext, sm, TRUE, mem);
		  	}
			TRACE_EVENT_P2("conc status: %d, conc length: %d", concatenated_status, conc_data_ext.len);
		  }
// July 7, 2005 REF: CRR MMI-SPR-32179 x0018858 
// Added a check to free the variable only when CONC_COMPLETED status is encountered.
       if (concatenated_status EQ CONC_COMPLETED)
       {
    	    if(conc_data_ext.data != NULL)
		{
			MFREE(conc_data_ext.data);
		}       
       } 
#else /*TI_PS_FF_CONC_SMS*/
	concatenated_status = CONC_ERR_UNKN;
#endif /*TI_PS_FF_CONC_SMS*/
        sms_store_new_msg_info(index,sm,TRUE, concatenated_status, received_sms_type, (T_MFW_SMS_MEM)mem);

	  if (f_update EQ MFW_SMS_UPDATE OR
	    coll_sms_send EQ MFW_SMS_WAIT OR
	    coll_sms_read EQ MFW_SMS_WAIT OR
	    coll_sms_delete EQ MFW_SMS_WAIT OR
	    coll_sms_req EQ MFW_SMS_WAIT)
		{
			if (sms_id_wait.index <= 0)
			{
				sms_id_wait.index = index;
				sms_id_wait.mem = (mem EQ SMS_STOR_Me)? MFW_MEM_ME: MFW_MEM_SIM;
				sms_id_wait.concat_status = concatenated_status;
				sms_id_wait.received_sms_type = received_sms_type;
			}
			else
			{
				TRACE_EVENT("Indication Overrun");
			}
		  f_update = MFW_SMS_UPDATE;
		}
	  else
	  {
	  	sms_send_new_msg_ind(sm,index, mem, received_sms_type, concatenated_status);
	  }
     f_update = MFW_SMS_FREE;
     if(memory.total == memory.used)
  	{
  		TRACE_EVENT_P2("memory used is = %d, of %d", memory.used, memory.total);
  		sms_signal(E_SMS_MEM_FULL, NULL);
  	} 
  	}
 if(msg_info != NULL)	
  {
	TRACE_EVENT_P4("CMTI Msg table: %d,%d,%d, %d", msg_info[0].index, msg_info[1].index,msg_info[2].index,msg_info[3].index);
	TRACE_EVENT_P4("CMTI Msg table: %d,%d,%d, %d", msg_info[4].index, msg_info[5].index,msg_info[6].index,msg_info[7].index);
}
/*MC SPR 1364, need to call this function to update CPHS voicemail data*/ 
  	 sms_check_voice_mail(sm->pid, sm->dcs, sm->adress,
                                       &sm->toa, &sm->udh);
 /*SPR 2260, this is already called in sms_store_new_msg_info, BUT we need it in for initialisation*/
}
#endif


#if defined (FF_WAP) && defined (FF_GPF_TCPIP)
//TISHMMS Project
/*
use UDH data to caculate destinator port and originator port id if any
If not port id, the out put id will be 0
*/
void sms_getPortId(UBYTE* data, UBYTE length, USHORT * destinatorPort, USHORT *originatorPort)
{
    UBYTE ucOffset = 0;
    USHORT usTempOffset; 
    UBYTE i;

    *destinatorPort = 0;
    *originatorPort = 0;

    if (length == 0)
    {
        return;
    }
    
    for (i = 0 ; i < 47; i++)//140/3=47
    {
        /*3GPP protocal 23040 clause 9.2.3.24*/
        //0x05 the IE is port id, length should be 4
        if ((data[ucOffset] == 0x05) && (data[ucOffset + 1] == 4))
        {
	    *destinatorPort = data[ucOffset + 3] + data[ucOffset + 2]*0x100;
	    *originatorPort = data[ucOffset + 5] + data[ucOffset + 4]*0x100;
            //do not return here because the last is the valid one
        }
        else
        {
            usTempOffset    = ucOffset + data[ucOffset + 1] + 2;

            //port information 6 bytes
            if (((usTempOffset + 6) > MAX_SM_LEN) ||((usTempOffset + 6) > length))
            {
                break;
            }
            
            ucOffset = usTempOffset;
        }
    }
    
    return;
}


/*
Search in UDH data to see if it is a EMS SMS
*/
BOOL sms_is_ems_type(UBYTE* UdhData, UBYTE UdhLength)
{
    BOOL Ret = FALSE;
    UBYTE ucOffset = 0;
    USHORT usTempOffset; 
    
    UBYTE i;

    if (UdhLength == 0)
    {
        return Ret;
    }
    
    for (i = 0 ; i < 47; i++)
    {
        /*3GPP protocal 23040 clause 9.2.3.24*/
        if ( (UdhData[ucOffset] >= 0X0A) && (UdhData[ucOffset] <= 0X1F) )
        {
            if ( (UdhData[ucOffset] >= 0X1B) && (UdhData[ucOffset] <= 0X1F) )
            {
                //xmzhou_trace_string_value("sms_is_ems_type Reserved for future EMS features: ", UdhData[ucOffset]);
            }

            Ret = TRUE;
            break;
        }
        else
        {
            usTempOffset    = ucOffset + UdhData[ucOffset + 1] + 2;
            
            if ((usTempOffset >= MAX_SM_LEN) ||(usTempOffset >= UdhLength))
            {
                break;
            }
            
            ucOffset = usTempOffset;
        }
    }

    return Ret;
}

//TISHMMS Project
T_MNSMS_MESSAGE_IND gTpduData;
void rAT_PlusCMTPdu (T_MNSMS_MESSAGE_IND* pTpdu)
{
    //xmzhou_trace_string("rAT_PlusCMTPdu called");
    memcpy(&gTpduData, pTpdu, sizeof(T_MNSMS_MESSAGE_IND));
    return;
}



#if defined (FF_MMI_MMS) || defined (FF_MMI_EMS)
EXTERN void sms_save_to_m4(U8* pSmsPud);
#endif

BOOL sms_save_to_m4_in_Mfw( T_MNSMS_MESSAGE_IND *mnsms_message_ind ) 
{
    T_sms_sdu * sms_sdu = &mnsms_message_ind->sms_sdu;
    U8 align_pdu[SIM_PDU_LEN];
    U8* pSmsPud;
    U8 *pAligned = align_pdu;
    U8 *pData = sms_sdu->buf;
    U8 offset;
    U16 i;
    U16 ByteLength;
#if 0
 /* First align data on byte boundry */
 pData += sms_sdu->o_buf / EIGHT_BITS;
 offset = sms_sdu->o_buf % EIGHT_BITS;

 

 if (offset)
 {
  for ( i=0; i < sms_sdu->l_buf; i += EIGHT_BITS)
  {
   *pAligned = *pData++ << offset;
   *pAligned++ |= *pData >> (EIGHT_BITS - offset);
  }
  /* Point back to the start of the data */
  pAligned = align_pdu;
 }
 else
  pAligned = pData;
#endif
    pSmsPud = sms_sdu->buf;
    if ((sms_sdu->l_buf & 0x07) == 0)
    {
        ByteLength = sms_sdu->l_buf/8;
    }
    else
    {
        ByteLength = (sms_sdu->l_buf/8) + 1;
    }
    
//NM    sms_save_tpdu_ffs(pSmsPud, ByteLength);

#if defined (FF_MMI_MMS) || defined (FF_MMI_EMS)
    sms_save_to_m4(pSmsPud);
#endif
    
 return TRUE;
}



/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS           |
| STATE   : code                         ROUTINE : rAT_PlusCMTI      |
+--------------------------------------------------------------------+

   PURPOSE :  Receive a new short message identifier.

*/

void rAT_PlusCMTI(T_ACI_SMS_STOR mem, UBYTE index, T_ACI_CMGL_SM* sm)
{
/* MZ 2007 Handset displays Message Full on receiving an SMS. */
  //T_MFW_MEM_INFO memory;
  T_ACI_RETURN ret;
  T_SMS_TYPE received_sms_type;
  T_CONC_ASSEMBLY_RETURN concatenated_status;
#ifdef TI_PS_FF_CONC_SMS  
  T_SM_DATA_EXT conc_data_ext;
#endif /*TI_PS_FF_CONC_SMS*/
  int i, j;
  static int fileid=0;
 
 if (initialising_flag == FALSE)
  	{
  		/* Ignore CMTI if initialising */
  		 	

	  TRACE_FUNCTION ("rAT_PlusCMTI()");
  		
#if 0
	  /////////////////////////////////////////////////////////added by ellen
	{
		char temp[120];
		char* temp2;
		int i,i_sum=0;

		TRACE_EVENT("=== SMS Received ===");
		TRACE_EVENT_P2("fo %02X, msg_ref %02X",sm->fo, sm->msg_ref);
		sprintf(temp," %d bytes of UDH", sm->udh.len);
		TRACE_EVENT(temp);


		temp2 = (char*)temp;
		sprintf(temp2,"udh ");
		temp2+=4;

               for(i=0; i<sm->udh.len; i++)
		    {
		    	sprintf(temp2,"%02X ",sm->udh.data[i]);
		    	temp2+=3;
		    	i_sum ++;
		    	if(i_sum > 30) 	break;
		    	
		    }
		TRACE_EVENT(temp);

		sprintf(temp," %d bytes of DATA", sm->data.len);
		TRACE_EVENT(temp);


		temp2 = (char*)temp;
		sprintf(temp2,"data ");
		temp2+=5;

               for(i=0; i<sm->data.len; i++)
		    {
		    	sprintf(temp2,"%02X ",sm->data.data[i]);
		    	temp2+=3;
		    	i_sum ++;
		    	if(i_sum > 30) 	break;
		    	
		    }
		TRACE_EVENT(temp);

	}
/////////////////////////////////////////////////////////
#endif

		/* SPR#2086 - SH - Intercept a WAP SMS *///TISHMMS Project
		{
            
			USHORT destinatorPort;
			USHORT originatorPort;
            
                     sms_getPortId(sm->udh.data, sm->udh.len, &destinatorPort, &originatorPort);

			//xmzhou_trace_string_value("New Msg arrived,desti port=",destinatorPort);
			//xmzhou_trace_string_value("New Msg arrived,orig port=",originatorPort);

  			//xmzhou_trace_string_value("CMTI new msg udh length=",sm->udh.len);
  			//xmzhou_trace_n_bytes(sm->udh.data,sm->udh.len);
  		
			TRACE_EVENT_P2("+++++ In rAT_PlusCMTI ready to judge PUSH or not DestP=%d,OrgP=%d +++++",destinatorPort,originatorPort);
			if (destinatorPort==2948
				|| destinatorPort==2949
				|| destinatorPort==49999	/*OTA Provisioning*/
				|| destinatorPort==49996	/*OTA SyncML*/
				|| originatorPort==9200
				|| originatorPort==9201
				|| originatorPort==9202
				|| originatorPort==9203)

			{	
				USHORT index_i;
				
  		//xmzhou_trace_string_value("CMTI new msg data length=",sm->data.len);
              //xmzhou_trace_string("WAP SMS be identified!!!!!!!");
  		//xmzhou_trace_n_bytes(sm->data.data,sm->data.len);
#if 0				
  		fileid++;
  		
  		if(fileid==1){
  		xmzhou_trace_string_value("CMTI new msg data1 length=",sm->data.len);
  		if((sm->data.len)<2048)	ffs_fwrite("/mms/data1", sm->data.data, sm->data.len);

		rvf_delay(500);
		
  		xmzhou_trace_string_value("CMTI new msg udh1 length=",sm->udh.len);
  		if((sm->udh.len)<2048)  ffs_fwrite("/mms/udh1", sm->udh.data, sm->udh.len);

  		}

  		if(fileid==2){
  		xmzhou_trace_string_value("CMTI new msg data2 length=",sm->data.len);
  		if((sm->data.len)<2048)	ffs_fwrite("/mms/data2", sm->data.data, sm->data.len);

		rvf_delay(500);

  		xmzhou_trace_string_value("CMTI new msg udh2 length=",sm->udh.len);
  		if((sm->udh.len)<2048)  ffs_fwrite("/mms/udh2", sm->udh.data, sm->udh.len);
  		}

  		rvf_delay(1000);
#endif  		
				/*a0393213 OMAPS00109953 - memory added as parameter*/
				sms_msg_delete(index,mem);				
#if 0
//////////////////////////////////////////////////////////////////////////////////
				TRACE_EVENT("MMS Notification");
				if(sm->udh.len > 8){
					redirectMMSNotification(sm);
				}
				else {
/////////////////////////////////////////////////////////////////////////////////			
#endif
				memset(&sms_mt, 0, sizeof(sms_mt));

				for (index_i=0; index_i<12; index_i++)
				{
					TRACE_EVENT_P2("Adress: %X (%c)", sm->adress[index_i], sm->adress[index_i]);
				}
				
				memcpy(sms_mt.orig_addr.tag, sm->alpha.data, sm->alpha.len);
				sms_mt.orig_addr.len = sm->alpha.len;
				strcpy(sms_mt.orig_addr.number, sm->adress);
				sms_mt.orig_addr.ton = phb_cvtTon(sm->toa.ton);
				sms_mt.orig_addr.npi = phb_cvtNpi(sm->toa.npi);
				if (sm->tosca.ton == TON_International)
				    sprintf(sms_mt.sc_addr, "+%s", sm->sca);
				else
				    strcpy(sms_mt.sc_addr, sm->sca);
				sms_mt.prot_id = (UBYTE)sm->pid;
				sms_mt.dcs     = (UBYTE)sm->dcs & 0xFC;	/* Ignore message class */
			       sms_mt.msg_len = sm->data.len;
				memset(sms_mt.sms_msg, 0, sm->data.len);
				memcpy(sms_mt.sms_msg, (char*)sm->data.data, sm->data.len);
				sms_mt.udh_len = sm->udh.len;

                            memcpy((char *)sms_mt.udh, (char *)sm->udh.data, sm->udh.len);

                            //xrashmic 9 May, 2005 MMI-SPR-30159
                            memcpy(&sms_mt.sctp,&sm->scts,sizeof(T_ACI_VP_ABS));

				sms_signal(E_SMS_WAP_MT_RECEIVED, &sms_mt);

				/* Delete the message */
				
				return;
#if 0				
			}/*end of else*/
#endif			
		}
	}

       {
        //BOOL sms_is_ems_type(UBYTE* UdhData, UBYTE UdhLength)
            BOOL bIsEms = FALSE;
        
            bIsEms = sms_is_ems_type(sm->udh.data, sm->udh.len);

            if (bIsEms == TRUE)
            {
                //xmzhou_trace_string("EMS SMS received!!!!!!!!!!!!");//proc EMS message
		   /*a0393213 OMAPS00109953 - memory added as parameter*/
                sms_msg_delete(index,mem);				
                sms_save_to_m4_in_Mfw( &gTpduData );
            }


       }
	
	  TRACE_EVENT_P1("f_update is = % in CMTI", f_update);
	/*a0393213 OMAPS00109953 - new parameter added to SMS_getSMSType()*/
	if(initialising_flag == TRUE)
       {
         received_sms_type = SMS_getSMSType( &sm->udh, sm->adress, MODE3);
       }
       else
       {
         received_sms_type = SMS_getSMSType( &sm->udh, sm->adress, MODE2);
       }
       TRACE_EVENT_P1("SMS Type is = %d", received_sms_type);
	 TRACE_EVENT_P2("msg_ref: %d, index: %d", sm->msg_ref, index);
 
#ifdef TI_PS_FF_CONC_SMS

	  if (received_sms_type == CONCATE)
		  {
		  if(initialising_flag == TRUE)
		  	{
			/*a0393213 OMAPS00109953 - memory added as parameter*/
		  	concatenated_status =concSMS_Collect(&conc_data_ext, sm, FALSE, SMS_STOR_NotPresent);
		  	}
		  else
		  	{
			/*a0393213 OMAPS00109953*/
		  	concatenated_status =concSMS_Collect(&conc_data_ext, sm, TRUE, mem);
		  	}
			TRACE_EVENT_P2("conc status: %d, conc length: %d", concatenated_status, conc_data_ext.len);
		  }
#else /*TI_PS_FF_CONC_SMS*/	  
	concatenated_status = CONC_ERR_UNKN;
#endif /*TI_PS_FF_CONC_SMS*/

  /*a0393213 OMAPS00109953 - memory added as parameter*/
	    sms_store_new_msg_info(index,sm,TRUE, concatenated_status, received_sms_type, mem);

	  if (f_update EQ MFW_SMS_UPDATE OR
	    coll_sms_send EQ MFW_SMS_WAIT OR
	    coll_sms_read EQ MFW_SMS_WAIT OR
	    coll_sms_delete EQ MFW_SMS_WAIT OR
	    coll_sms_req EQ MFW_SMS_WAIT)
		{
			if (sms_id_wait.index <= 0)
			{
				sms_id_wait.index = index;
				sms_id_wait.mem = (mem EQ SMS_STOR_Me)? MFW_MEM_ME: MFW_MEM_SIM;
				sms_id_wait.concat_status = concatenated_status;
				sms_id_wait.received_sms_type = received_sms_type;
			}
			else
			{
				TRACE_EVENT("Indication Overrun");
			}
		  f_update = MFW_SMS_UPDATE;
		}
	  else
	  {
	  	sms_send_new_msg_ind(sm,index, mem, received_sms_type, concatenated_status);
	  }
     f_update = MFW_SMS_FREE;
     if(memory.total == memory.used)
  	{
  		TRACE_EVENT_P2("memory used is = %d, of %d", memory.used, memory.total);
  		sms_signal(E_SMS_MEM_FULL, NULL);
  	} 
  	}
  if(msg_info != NULL)	
  {	
	TRACE_EVENT_P4("CMTI Msg table: %d,%d,%d, %d", msg_info[0].index, msg_info[1].index,msg_info[2].index,msg_info[3].index);
	TRACE_EVENT_P4("CMTI Msg table: %d,%d,%d, %d", msg_info[4].index, msg_info[5].index,msg_info[6].index,msg_info[7].index);
}
/*MC SPR 1366, need to call this function to update CPHS voicemail data*/ 
  	 sms_check_voice_mail(sm->pid, sm->dcs, sm->adress,
                                      &sm->toa, &sm->udh);
 /*SPR 1833, this is already called in sms_store_new_msg_info, BUT we need it in for initialisation*/
}

#endif /* #if defined (FF_WAP) && defined (FF_GPF_TCPIP) */
/*
+------------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS               |
| STATE   : code                         ROUTINE : find_slot_in_msg_info |
+------------------------------------------------------------------------+

   PURPOSE :  Find a slot in msg_info for the index of the SMS supplied

*/
/*a0393213 OMAPS00109953 - memory added as parameter*/
static int find_slot_in_msg_info(UBYTE index, UBYTE update_mem, T_MFW_SMS_MEM mem)
{
    int ret = -1;
    int i;

    TRACE_FUNCTION("find_slot_in_msg_info");
    
    if (index >= 1)
    {
        /* A valid index, so look for it already being in msg_info */
#ifdef FF_MMI_SMS_DYNAMIC
        for (i = 0; (i < g_max_messages) && (ret == -1); i++)
#else
        for (i = 0; (i < MAX_MESSAGES) && (ret == -1); i++)
#endif
        {
            /*a0393213 OMAPS00109953 - mem also taken into consideration*/
            if (msg_info[i].index == index && msg_info[i].mem==mem) 
            {
                /* Index already in msg_info, so replace */
                ret = i;
            }
        }

        if (ret == -1)
        {
            /* Index was not already in msg_info, so look for unused slot */
#ifdef FF_MMI_SMS_DYNAMIC
            for (i = 0; (i < g_max_messages) && (ret == -1); i++)
#else
            for (i = 0; (i < MAX_MESSAGES) && (ret == -1); i++)
#endif
            {
                if (msg_info[i].index == -1)
                {
                    /* First unused slot in msg_info found */
                    ret = i;
               		if (update_mem)
            			memory.used++;
				/*a0393213 OMAPS00109953 - memory usage updated*/
				if(update_mem)
					{
					if(mem==MFW_MEM_SIM)
						sim_memory.used++;
					else
						ffs_memory.used++;
					}
               }
            }
        }
    }

    TRACE_EVENT_P1("find_slot_in_msg_info returning %d", ret);

    return ret;
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS           |
| STATE   : code                         ROUTINE : sms_store_new_msg_info |
+--------------------------------------------------------------------+

   PURPOSE :  Store message information

*/
/*a0393213 OMAPS00109953 - memory added as parameter*/
T_MFW sms_store_new_msg_info(UBYTE index, T_ACI_CMGL_SM* sm, UBYTE update_mem, T_CONC_ASSEMBLY_RETURN conc_stat	,
									   T_SMS_TYPE received_sms_type, T_MFW_SMS_MEM mem)
{
  int j;
  // Feb 16, 2005 REF: CRR MMI-SPR-27900 xnkulkar 
  // Added this variable to store the voice mail status
  T_MFW status; 

#ifndef MMI_LITE
  //T_CONC_ASSEMBLY_RETURN concatenated_status;  // RAVI
//  T_SM_DATA_EXT conc_data_ext;  // RAVI
#endif
 int i;


  TRACE_EVENT("sms_store_new_msg_info");
  
#if defined (FF_WAP) && defined (FF_GPF_TCPIP)
  sms_type = MFW_SMS_MESSAGE;
#else
	/*SPR 2530 removed setting of sms_type flag*/
#endif
    /*a0393213 OMAPS00109953 - memory added as parameter*/
    if ((j = find_slot_in_msg_info(index, update_mem,mem)) != -1)		
	{
		//x0pleela 07 Feb, 2006  DR: OMAPS00059261
		//Initial value of msg_type is set to not voice
		 msg_info[j].msg_type = MFW_SMS_NotVOICE;

	    TRACE_EVENT_P1("SMS Type is = %d", received_sms_type);
	 	

#ifdef TI_PS_FF_CONC_SMS	
			/*SPR 2260*/
		  if (received_sms_type == CONCATE)
		  {	 
             	/*a0393213 OMAPS00109953 - code cleanup done*/
			UBYTE first_index=0;
			T_MFW_SMS_MEM first_mem=MFW_MEM_NOTPRESENT;
			T_CONC_BUF_ELEM* conc_buf_elem;
			conc_buf_elem=concSMS_GetFirstIndex_ext(concSMS_GetMsgRef ( sm ), sm->adress);
			/*a0393213 OMAPS00109953 - The memory type returned by concSMS_GetFirstIndex_ext 
			if of psa type. It should be changed to cmh type. Clean solution would be to move this conversion to ACI */
			if(conc_buf_elem)
				{
				first_index=conc_buf_elem->rec_num;
				cmhSMS_getMemCmh(conc_buf_elem->mem,(T_ACI_SMS_STOR*)(&first_mem));			
				}
			/*a0393213 OMAPS00109953 - Msg Ref (unique number to each message, don't confuse with index) is stored*/
			msg_info[j].msg_ref=concSMS_GetMsgRef ( sm );
		   	 	
	   	  	if (conc_stat == CONC_COMPLETED)
		   	{	/*replace index withat of first message*/
		   		index = first_index;
				/*a0393213 OMAPS00109953 - first_mem is the memory where the first segment of the cSMS resides*/
				mem=first_mem;
			
				msg_info[j].concat_status = MFW_SMS_CONC_WHOLE;
				TRACE_EVENT("Setting to WHOLE");
		   	 }
		   	 else /*otherwise leave index as it is*/
		   	 {	 /*check if message already complete*/
#ifdef FF_MMI_SMS_DYNAMIC
  				for (i=0; i<g_max_messages; i++)
#else
  				for (i=0; i<MAX_MESSAGES; i++)
#endif
				{	if (msg_info[i].index != -1)
					{	if (msg_info[i].first_index == first_index
						&& msg_info[i].first_mem == first_mem /*a0393213 OMAPS00109953*/
						&& msg_info[i].concat_status == MFW_SMS_CONC_WHOLE)
							return 1;/*don't bother copying this message into the table*/ // RAVI - Changed return to return 1
					}
				}
						
		   	  msg_info[j].concat_status = MFW_SMS_CONC_PART;
		   	  TRACE_EVENT("Setting to PART");
		   	  }
		   	 
		   	  TRACE_EVENT_P1("index = %d",index);

		   	  msg_info[j].first_index =first_index;
/*a0393213 OMAPS00109953*/
          		  msg_info[j].first_mem=first_mem;
	   	  		
		  		/*if complete SMS, delete old fragments*/
		  		if (conc_stat == CONC_COMPLETED)
#ifdef FF_MMI_SMS_DYNAMIC
  				for (i=0; i<g_max_messages; i++)
#else
		  		for (i=0; i<MAX_MESSAGES; i++)
#endif
		   	  	{	
					if (msg_info[i].index != -1)
					{
  					       /*a0393213 OMAPS00109953*/
						 if (msg_info[i].msg_ref==concSMS_GetMsgRef ( sm )
                                               &&(msg_info[i].concat_status == MFW_SMS_CONC_PART ))
						 {	
						 	TRACE_EVENT_P1("Deleting index number:%d", msg_info[i].index);
			  				delete_sms_from_table(msg_info[i].index, msg_info[i].mem);/*a0393213 OMAPS00109953*/
			  			}
					}
		  			
		  	  	}
		  }
		  /*a0393213 OMAPS00109953 - new case added - it's the case where segments of cSMS are treated as normal SMS*/
		  else if (received_sms_type == NORMAL_IND_CSMS)
		  {
			TRACE_EVENT( "sms_store_new_msg_info Incomplete sms!!!!");
			msg_info[j].first_index = 0xFF;/*invalid index*/
			msg_info[j].first_mem=MFW_MEM_NOTPRESENT;/*a0393213 OMAPS00109953*/
		  	msg_info[j].concat_status = MFW_SMS_CONC_PART;
		  }	
		  else
#endif /*TI_PS_FF_CONC_SMS*/	
		  {	
		  	msg_info[j].first_index = 0xFF;/*invalid index*/
			msg_info[j].first_mem=MFW_MEM_NOTPRESENT;/*a0393213 OMAPS00109953*/
		  	msg_info[j].concat_status = MFW_SMS_NO_CONC;
		  	TRACE_EVENT("Setting to NO CONC");
		  }
	
	   	
   	    sm->index = index;
   		msg_info[j].index = index;
		msg_info[j].mem=mem;/*a0393213 OMAPS00109953*/

   		//x0pleela 08 Feb, 2006 DR: OMAPS00059261
		//Replacing MT_STATUS_RPT with MFW_SMS_STATRPT as MT_STATUS_RPT is removed
		
		//x0pleela 07 Feb, 2006 DR: OMAPS00059261
		//Update msg_info's message type for STATUS REPORT
		if(( 0x03 & sm->fo ) EQ MFW_MSG_STATUS_REPORT )
		{
			msg_info[j].msg_type = MFW_SMS_STATRPT;
		}
		msg_info[j].stat = sms_decodeStatus(sm->stat);
		
		strcpy((char *)msg_info[j].addr.number, sm->adress);
		msg_info[j].addr.ton = phb_cvtTon(sm->toa.ton);
		msg_info[j].addr.npi = (T_MFW_PHB_NPI)phb_cvtNpi(sm->toa.npi);


#ifdef NO_ASCIIZ
			memcpy(msg_info[j].addr.tag, 
			sm->alpha.data, 
			sm->alpha.len);
			msg_info[j].addr.len = sm->alpha.len;
#else
			phb_Alpha2Gsm(&sm->alpha, 
			(UBYTE *)msg_info[j].addr.tag);
#endif

		memcpy(&msg_info[j].rctp, &sm->scts, sizeof(T_MFW_SMS_SCTP));

		// check voice mail status
		// Feb 16, 2005 REF: CRR MMI-SPR-27900 xnkulkar 
		// Commented the 'if' condition below.
/*		if (sms_check_voice_mail(sm->pid, sm->dcs, sm->adress,
		    &sm->toa, &sm->udh) EQ MFW_SMS_VOICE)
		    		msg_info[j].msg_type = MFW_SMS_VOICE;*/
		status = 	sms_check_voice_mail(sm->pid, sm->dcs, sm->adress,
		    &sm->toa, &sm->udh);
		// Check if it is a voice mail message
		if(status == MFW_SMS_VOICE || status == MFW_SMS_VOICEMSG_DELETE)
		{
			// If voicemail message is from group "1100"
			if(status == MFW_SMS_VOICEMSG_DELETE)
			{	
				// Mark this message for deletion
				msg_info[j].stat= MFW_SMS_VOICEMSG_DELETE;
			}
		    msg_info[j].msg_type = MFW_SMS_VOICE;
		} 
		
		coll_sms_read = MFW_SMS_FREE;
        f_update = MFW_SMS_FREE;
        TRACE_EVENT_P1("UDH Length is = %d", sm->udh.len);
    }
    /*SPR 2260, check if memory full*/
    TRACE_EVENT_P2("memory used is = %d, of %d", memory.used, memory.total);
    if(memory.total == memory.used)
  	{
  		
  		sms_signal(E_SMS_MEM_FULL, NULL);
  	}  
	return 1; // RAVI
}



/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS           |
| STATE   : code                         ROUTINE : sms_stored_submit |
+--------------------------------------------------------------------+

   PURPOSE :  Send a new message indication to the MMI

*/

T_MFW sms_send_new_msg_ind(T_ACI_CMGL_SM* sm,UBYTE index, UBYTE mem, UBYTE received_sms_type, UBYTE concatenated_status)
{
	TRACE_FUNCTION("sms_send_new_msg_ind");
#ifdef TI_PS_FF_CONC_SMS
	if (received_sms_type == CONCATE)
	{
		switch (concatenated_status)
		{
			case CONC_ERR_UNKN:
			TRACE_EVENT("Concatenated SMS error");break;
			case CONC_ERR_BUF_FULL:
			TRACE_EVENT("Concatented SMS buffer Full Error");break;
			case CONC_CONTINUED: //not last SMS
			TRACE_EVENT("Concatenation Continued");
			break;
			case CONC_COMPLETED:
			{
				TRACE_EVENT("Concatenation Completed");
				memset(&sms_mt, 0, sizeof(sms_mt));
				/*SPR 2260*/
 	 			if (sms_check_voice_mail(sm->pid, sm->dcs, sm->adress,
                &sm->toa, &sm->udh) == MFW_SMS_VOICE)
                  	sms_mt.voicemail = TRUE;
  	 			else
  	 				sms_mt.voicemail = FALSE;
				if(initialising_flag == TRUE)
					sms_signal(E_SMS_MT, &sms_mt);
				else
				{
					//Aug 26, 2004    REF: CRR 23287     Deepa M.D
					//The second parameter is not used in case of 
					//E_SMS_MT_RECEIVED event.sms_id_wait is the variable
					//used for MT SMS, which is ignored anyway.
					sms_signal(E_SMS_MT_RECEIVED, &sms_id_wait);
				}
			}
		}
	}
	else
#endif /*TI_PS_FF_CONC_SMS*/
	{
		memset(&sms_mt, 0, sizeof(sms_mt));

		if(initialising_flag == TRUE)
		sms_signal(E_SMS_MT, &sms_mt);
		else
		{
			//Aug 26, 2004    REF: CRR 23287     Deepa M.D
			//The second parameter is not used in case of 
			//E_SMS_MT_RECEIVED event.sms_id_wait is the variable
			//used for MT SMS, which is ignored anyway.
			sms_signal(E_SMS_MT_RECEIVED, &sms_id_wait);
		}
	}
	return 1; /*a0393213 compiler warnings removal - added return statement*/
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS           |
| STATE   : code                         ROUTINE : rAT_PlusCBM       |
+--------------------------------------------------------------------+

   PURPOSE :  Receive a new cell broadcast message.

*/
void rAT_PlusCBM(SHORT           sn,
                 SHORT           mid,
                 SHORT           dcs,
                 UBYTE           page,
                 UBYTE           pages,
                 T_ACI_CBM_DATA* data)
{
  TRACE_FUNCTION ("rAT_PlusCBM()");

  /* copy parameter */
  sms_cb.sn     = sn;
  sms_cb.msg_id = mid;
  sms_cb.dcs    = (UBYTE)dcs;
  sms_cb.page   = page;
  sms_cb.pages  = pages;
  sms_cb.msg_len = data->len;
  memcpy((char *)sms_cb.cb_msg, (char *)data->data, data->len);
/* send cell broadcast message event */
//  	Oct 18, 2005    REF: SPR 34678 xdeepadh
//The CB messages with DCS 242(class-2,default alphabet)  are discarded
	if(dcs!=0xF2)
	{
		sms_signal(E_SMS_CB, &sms_cb);
	}
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS           |
| STATE   : code                         ROUTINE : rAT_PlusCBMI      |
+--------------------------------------------------------------------+

   PURPOSE :  Receive a new cell broadcast message identifier.

*/

void rAT_PlusCBMI(T_ACI_SMS_STOR mem, UBYTE index)
{
  TRACE_EVENT ("rAT_PlusCBMI()");

  switch ( mem )
  {
    case ( SMS_STOR_Me ):
        sms_id.mem = MFW_MEM_ME;
        break;
    case ( SMS_STOR_Sm ):
        sms_id.mem = MFW_MEM_SIM;
        break;
    default:
        break;
  }

  sms_id.index = index;

  /* send a short message identifier event */
  sms_signal(E_SMS_CB_RECEIVED, &sms_id);
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS           |
| STATE   : code                         ROUTINE : rAT_PlusCDS       |
+--------------------------------------------------------------------+

   PURPOSE :  Receive a status report message.

*/

void rAT_PlusCDS(T_ACI_CDS_SM *st)
{
  T_MFW_SMS_STATUS sms_status;
  T_MFW_PHB_ENTRY  phb_entry;
  T_MFW_PHB_LIST   phb_list;
  SHORT            phb_index;
#ifdef NO_ASCIIZ
  T_MFW_PHB_TEXT  findText;
#endif

  TRACE_FUNCTION ("rAT_PlusCDS()");

  memset(&phb_entry, 0, sizeof(phb_entry));
  memset(&phb_list, 0, sizeof(phb_list));
  phb_list.entry = &phb_entry;

  sms_status.msg_id  = 0x03 & (UBYTE)st->fo;
  sms_status.msg_ref = st->msg_ref;
  strcpy(sms_status.addr.number, st->addr);
  sms_status.addr.ton = phb_cvtTon(st->toa.ton);
  sms_status.addr.npi = (T_MFW_PHB_NPI)phb_cvtNpi(st->toa.npi);

#ifdef NO_ASCIIZ
  strcpy((char *)findText.data, st->addr);
  findText.len = (UBYTE)strlen(st->addr);
  if (phb_find_entries  (PHB_ADN,   &phb_index,
                         MFW_PHB_NUMBER,
                         1, &findText,
                         &phb_list) EQ MFW_PHB_OK)
  {
    if (phb_list.num_entries)
    {
      memcpy(sms_status.addr.tag,
             phb_list.entry->name.data,
             phb_list.entry->name.len);
      sms_status.addr.len = phb_list.entry->name.len;
		  
	  }
//  Apr 12, 2004    REF: CRR 13370     Deepa M.D
	  else
	  {
		  sms_status.addr.tag[0]='\0';
		  sms_status.addr.len = 0;
		  
    }
  }
#else
  if (phb_find_entries  (PHB_ADN,	&phb_index,
                         MFW_PHB_NUMBER,
                         1,
                         st->ra,
                         &phb_list) EQ MFW_PHB_OK)
  {
	  
    if (phb_list.num_entries)
    {
      strcpy((char *)sms_status.addr.tag, (char *)phb_list.entry->name);
		  
    }
  }
#endif

  sms_status.status  = st->tp_status;

  memcpy((char *)sms_status.rctp.year, (char *)st->vpabs_scts.year, MAX_SCTP_DIGITS);
  memcpy((char *)sms_status.rctp.month, (char *)st->vpabs_scts.month, MAX_SCTP_DIGITS);
  memcpy((char *)sms_status.rctp.day, (char *)st->vpabs_scts.day, MAX_SCTP_DIGITS);
  memcpy((char *)sms_status.rctp.hour, (char *)st->vpabs_scts.hour, MAX_SCTP_DIGITS);
  memcpy((char *)sms_status.rctp.minute, (char *)st->vpabs_scts.minute, MAX_SCTP_DIGITS);
  memcpy((char *)sms_status.rctp.second, (char *)st->vpabs_scts.second, MAX_SCTP_DIGITS);
  sms_status.rctp.timezone = st->vpabs_scts.timezone;

  /* send status report message event */
  sms_signal(E_SMS_STATUS, &sms_status);
}


/*
+------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)      MODULE  : MFW_SMS            |
| STATE   : code                      ROUTINE : sms_set_pref_memory|
+------------------------------------------------------------------+

   PURPOSE :  Set preferred message storage.

*/

T_MFW sms_set_pref_memory(UBYTE mem)
{
  TRACE_FUNCTION ("sms_set_pref_memory()");

  /*a0393213 OMAPS00109953 - The pref memory is already same as mem*/
  if(memory.mem==mem)	
  	return MFW_SMS_OK;

  if (sAT_PlusCPMS(CMD_SRC_LCL,
                   sms_mem_cvt(mem),
                   sms_mem_cvt(mem),
                   sms_mem_cvt(mem)) NEQ AT_CMPL) /*a0393213 OMAPS00109953 sAT_PlusCPMS is a synchronous call - so AT_EXCT changed to AT_CMPL*/
    return MFW_SMS_FAIL;
  return MFW_SMS_OK;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS           |
| STATE   : code                         ROUTINE : sms_read          |
+--------------------------------------------------------------------+

   PURPOSE :  Read a SMS message.

*/
/*a0393213 OMAPS00109953 - memory added as parameter*/
T_MFW sms_read(UBYTE type, UBYTE index, T_MFW_SMS_MEM sms_memory)
{
  TRACE_EVENT_P1("sms_read():%d",index);

     /*a0393213 OMAPS00109953 - set preferred memory before calling sAT_PlusCMGR*/	
     if(sms_set_pref_memory(sms_memory)!=MFW_SMS_OK)
     	{
     	TRACE_ERROR("sms_read_msg_info():Not able to set pref memory");
 	return MFW_SMS_FAIL;
     	}

  coll_sms_read = MFW_SMS_WAIT;

  if (sAT_PlusCMGR(CMD_SRC_LCL, index, SMS_READ_Preview) NEQ AT_EXCT)
    return MFW_SMS_FAIL;
  /*SPR 2260, set sms type so we get event from rAT_plusCMGR*/
  sms_type = MFW_SMS_MESSAGE;

  memset(&sms_mt, 0, sizeof(sms_mt));
  sms_mt.index = index;
  sms_mt.memory=sms_memory;    /*a0393213 OMAPS00109953*/
  cmgr_awaiting_ok = FALSE; // Marcus: Issue 2260: 03/10/2002

  return MFW_SMS_OK;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS           |
| STATE   : code                         ROUTINE : rAT_PlusCMGR      |
+--------------------------------------------------------------------+


   PURPOSE :  Call back for read message.

*/

//Feb 28, 2004    REF: CRR 28809   Deepa M.D
/*The Optimization changes done for golite build have been incorpaorated to color build.
    The  MMI_LITE flag used for rAT_PlusCMGR has been removed.*/
void rAT_PlusCMGR(T_ACI_CMGL_SM  * sm, T_ACI_CMGR_CBM * cbm)
{
	T_SMS_TYPE received_sms_type;
	T_CONC_ASSEMBLY_RETURN concatenated_status=CONC_ERR_UNKN;
#ifdef TI_PS_FF_CONC_SMS
	T_SM_DATA_EXT conc_data_ext;
#endif /*TI_PS_FF_CONC_SMS*/
	//Feb 28, 2004    REF: CRR 28809   Deepa M.D
	//sms_mo is  made a local variable.It will be allocated /deallocated dynamically.
	T_MFW_SMS_MO *sms_mo=NULL;   
#if defined (FF_WAP) && defined (FF_GPF_TCPIP)
UBYTE wap_sms = FALSE;
#endif
  TRACE_FUNCTION ("rAT_PlusCMGR()");
  TRACE_EVENT_P2("messagepointer:%d, Cellbroadcast pointer:%d", sm, cbm);
  cmgr_awaiting_ok = TRUE; // Marcus: Issue 2260: 03/10/2002
#ifdef TI_PS_FF_CONC_SMS  
  conc_data_ext.data = NULL; //  Jul 07, 2004    REF: CRR 15846     Deepa M.D
#endif /*TI_PS_FF_CONC_SMS*/
  #if defined (FF_WAP) && defined (FF_GPF_TCPIP)
/* SPR#2086 - SH - Intercept a WAP SMS */
      {
          USHORT destinatorPort = sm->udh.data[3]+sm->udh.data[2]*0x100;
          USHORT originatorPort = sm->udh.data[5]+sm->udh.data[4]*0x100;
          
          if (destinatorPort==2948
              || destinatorPort==2949
              || originatorPort==9200
              || originatorPort==9201
              || originatorPort==9202
              || originatorPort==9203)
          {   
              wap_sms = TRUE;
          }
      }
  #endif
  switch (sms_type)
  {
    case MFW_SMS_MESSAGE:
    {	if (sm != NULL)  /*SPR2260 make sure we have a valid pointer*/
      	{   switch (0x03 & sm->fo)
	      {
	         case MFW_MSG_DELIVER:  //x0pleela 01 Feb, 2006 DR: OMAPS00059261 /* SMS type: MT */
	        { 
#ifdef TI_PS_FF_CONC_SMS
	    	USHORT conc_msg_ref;
	          	UBYTE conc_index;
     		T_CONC_BUF_ELEM* conc_buf_elem=NULL;/*a0393213 OMAPS00109953*/
	          int i;/*SPR2260*/
			  BOOL continue_flag=FALSE;/*SPR2260*/
#endif /*TI_PS_FF_CONC_SMS*/

			
	          	TRACE_EVENT_P1("UDH Length is = %d", sm->udh.len);
	    		received_sms_type = SMS_getSMSType( &sm->udh, sm->adress, MODE3);/*a0393213 OMAPS00109953*/
			TRACE_EVENT_P1("SMS Type is = %d", received_sms_type);
  		
#ifdef TI_PS_FF_CONC_SMS
	  		if (received_sms_type == CONCATE)
	  		{
			  /*a0393213 OMAPS00109953*/
			  concatenated_status =concSMS_Collect(&conc_data_ext, sm, FALSE, SMS_STOR_NotPresent);
			  TRACE_EVENT_P2("conc status: %d, conc length: %d", concatenated_status, conc_data_ext.len);
	  		
				switch (concatenated_status)
				{
				  
				  case CONC_ERR_BUF_FULL:
				    TRACE_EVENT("Concatented SMS buffer Full Error");
				     sms_signal(E_SMS_ERR, NULL);
					break;
				case CONC_ERR_UNKN:
				    TRACE_EVENT("Concatenated SMS error");
				  case CONC_CONTINUED: //not last SMS
				  	TRACE_EVENT("Concatenation Continued");
					/*SPR 2260*/
	                cmgr_awaiting_ok = FALSE; // Marcus: Issue 2260: 03/10/2002
					
					/*if the index exists in the message table, process the message*/
#ifdef FF_MMI_SMS_DYNAMIC
					for (i=0; i <g_max_messages;i++)
#else
					for (i=0; i <MAX_MESSAGES;i++)
#endif
						if (msg_info[i].index == sm->msg_ref && msg_info[i].concat_status == MFW_SMS_CONC_PART)
							continue_flag = TRUE;
					if (!continue_flag)
						break;
				  
				  case CONC_COMPLETED:
				  {
				    TRACE_EVENT("Concatenation Completed");
					memset(&sms_mt, 0, sizeof(sms_mt));
					/*a0393213 OMAPS00109953*/
					conc_msg_ref=concSMS_GetMsgRef(sm);
					conc_buf_elem=concSMS_GetFirstIndex_ext(conc_msg_ref,sm->adress);	
					conc_index=conc_buf_elem->rec_num;
#ifdef NO_ASCIIZ
					memcpy(sms_mt.orig_addr.tag,
						         sm->alpha.data,
						         sm->alpha.len);
					sms_mt.orig_addr.len = sm->alpha.len;
#else
					phb_Alpha2Gsm(sm->alpha, (UBYTE *)sms_mt.orig_addr.tag);
#endif
					strcpy(sms_mt.orig_addr.number, sm->adress);
					sms_mt.orig_addr.ton =
						      phb_cvtTon(sm->toa.ton);
				    sms_mt.orig_addr.npi = (T_MFW_PHB_NPI)phb_cvtNpi(sm->toa.npi);
					if (sm->tosca.ton == TON_International)
					  sprintf(sms_mt.sc_addr, "+%s", sm->sca);
					else
					  strcpy(sms_mt.sc_addr, sm->sca);
					sms_mt.prot_id = (UBYTE)sm->pid;
					sms_mt.dcs     = (UBYTE)sm->dcs & 0xFC;	/* Ignore message class */

					/*MC SPR 2260, message index should be set to that of first index in
					concatenated message if this is a complete message*/
					if (concatenated_status == CONC_COMPLETED)
						sms_mt.index = conc_index;
					else
						sms_mt.index = sm->msg_ref;
					/*a0393213 OMAPS00109953 - the memory is of psa type. It should be converted to cmh type
					Clean solution would be to move this conversion to ACI*/
					cmhSMS_getMemCmh(conc_buf_elem->mem,(T_ACI_SMS_STOR*)(&(sms_mt.memory)));								
					sms_mt.rp      = sm->fo >> 7 & 1;

					/* SBH */
					if (sm->dcs & 0x10)		/* Bit 5 indicates that we should get message class from bits 0 & 1 */
					{
						sms_mt.ti = sm->dcs & 3;
					}
					else					/* Otherwise, set to default value */
					{
						sms_mt.ti = 1;
					}
					/* SBH end */
					
					memcpy((char *)sms_mt.sctp.year, (char *)sm->scts.year, MAX_SCTP_DIGITS);
					memcpy((char *)sms_mt.sctp.month, (char *)sm->scts.month, MAX_SCTP_DIGITS);
					memcpy((char *)sms_mt.sctp.day, (char *)sm->scts.day, MAX_SCTP_DIGITS);
					memcpy((char *)sms_mt.sctp.hour, (char *)sm->scts.hour, MAX_SCTP_DIGITS);
					memcpy((char *)sms_mt.sctp.minute, (char *)sm->scts.minute, MAX_SCTP_DIGITS);
					memcpy((char *)sms_mt.sctp.second, (char *)sm->scts.second, MAX_SCTP_DIGITS);
					sms_mt.sctp.timezone = sm->scts.timezone;
						  
					/* copy length of whole concat SMS*/
					if (conc_data_ext.len > MAX_MSG_LEN)
					  sms_mt.msg_len = MAX_MSG_LEN;
					else
					  sms_mt.msg_len = conc_data_ext.len;
					/*copy all data from concat SMS*/
									/*SPR 2260*/
		         	if (sm->stat == SMS_STAT_Invalid)
						memset(sms_mt.sms_msg, NULL, MAX_MSG_LEN_ARRAY);
		         	else /*SPR2260 */
					{	if (concatenated_status == CONC_COMPLETED)
				    	{	memcpy(sms_mt.sms_msg, (char*)conc_data_ext.data, sms_mt.msg_len);}
				    	else/*copy data from sm pointer if message not complete*/
				    	{	memcpy(sms_mt.sms_msg, (char*)sm->data.data, sm->data.len);
							sms_mt.msg_len = sm->data.len;
						}
		         	}
					/* Free memory allocated by the ACI */
					//  Jul 07, 2004    REF: CRR 15846     Deepa M.D
					//Free the memory only if conc_data_ext.data  is not NULL.
					if(conc_data_ext.data != NULL)
					{
						MFREE(conc_data_ext.data);
					}
					sms_mt.udh_len = sm->udh.len;
					memcpy((char *)sms_mt.udh, (char *)sm->udh.data, sm->udh.len);
					
				  /*MC SPR 2260, need to call this function to update CPHS voicemail data*/ 
  	 				if (sms_check_voice_mail(sm->pid, sm->dcs, sm->adress,
                                       &sm->toa, &sm->udh) == MFW_SMS_VOICE)
                  		sms_mt.voicemail = TRUE;
  	 				else
  	 					sms_mt.voicemail = FALSE;
  	 				
	          			  sms_type = MFW_NO_MESSAGE;

	          			  coll_sms_read = MFW_SMS_FREE;
    					 /*whenever some operation is done the pref memory should be set back to SIM*/
       				  if(sms_set_pref_memory(MFW_MEM_SIM)!=MFW_SMS_OK)
   					  {
       					TRACE_ERROR("sms_ok_CMGL():Not able to set pref memory");			
       				  }						  
	         			  sms_signal(E_SMS_MT, &sms_mt);
						}
					}
		  		}
				else
#endif /*TI_PS_FF_CONC_SMS*/
				{
#ifdef NO_ASCIIZ
					memcpy(sms_mt.orig_addr.tag,
		            	     sm->alpha.data,
		                	 sm->alpha.len);

					sms_mt.orig_addr.len = sm->alpha.len;
#else
					phb_Alpha2Gsm(&sm->alpha, (UBYTE *)sms_mt.orig_addr.tag);
#endif

					if (sm->toa.ton == TON_International)
						sprintf(sms_mt.orig_addr.number, "+%s", sm->adress);
					else	
						strcpy(sms_mt.orig_addr.number, sm->adress);

					sms_mt.orig_addr.ton = phb_cvtTon(sm->toa.ton);
					sms_mt.orig_addr.npi = (T_MFW_PHB_NPI)phb_cvtNpi(sm->toa.npi);
		          
					if (sm->tosca.ton == TON_International)
						sprintf(sms_mt.sc_addr, "+%s", sm->sca);
					else
						strcpy(sms_mt.sc_addr, sm->sca);
		          
					sms_mt.prot_id = (UBYTE)sm->pid;
					sms_mt.dcs     = (UBYTE)sm->dcs & 0xFC;	/* Ignore message class */
					sms_mt.rp		 = sm->fo >> 7 & 1;

					// PATCH JPS 04.10 END
					/* SBH */
					if (sm->dcs & 0x10)		/* Bit 5 indicates that we should get message class from bits 0 & 1 */
					{
						sms_mt.ti = sm->dcs & 3;
					}
					else					/* Otherwise, set to default value */
					{
						sms_mt.ti = 1;
					}
					/* SBH end */

					TRACE_EVENT_P2("dcs: %d, ti: %d", sms_mt.dcs, sms_mt.ti);
					
					memcpy((char *)sms_mt.sctp.year, (char *)sm->scts.year, MAX_SCTP_DIGITS);
					memcpy((char *)sms_mt.sctp.month, (char *)sm->scts.month, MAX_SCTP_DIGITS);
					memcpy((char *)sms_mt.sctp.day, (char *)sm->scts.day, MAX_SCTP_DIGITS);
					memcpy((char *)sms_mt.sctp.hour, (char *)sm->scts.hour, MAX_SCTP_DIGITS);
					memcpy((char *)sms_mt.sctp.minute, (char *)sm->scts.minute, MAX_SCTP_DIGITS);
					memcpy((char *)sms_mt.sctp.second, (char *)sm->scts.second, MAX_SCTP_DIGITS);
					sms_mt.sctp.timezone = sm->scts.timezone;
					sms_mt.msg_len = sm->data.len;
					TRACE_EVENT_P2("msg_len %d, data.len %d", sms_mt.msg_len, sm->data.len);
				#if defined (FF_WAP) && defined (FF_GPF_TCPIP)
				        /* SPR#2086 - SH - If we've got a WAP SMS, replace the message
					 * with a string */
				
					if (wap_sms)
					{
						char *message = "WAP SMS";

						strcpy(sms_mt.sms_msg, message);
						sms_mt.msg_len = strlen(message);
					}
                                    else
                                    {
                                        
                                        /*SPR 2260, if invalid message just set text to NULL*/
				#endif
		         if (sm->stat == SMS_STAT_Invalid)
					memset(sms_mt.sms_msg, NULL, MAX_MSG_LEN_ARRAY);
		         else
				 {
				  	memcpy(sms_mt.sms_msg, (char*)sm->data.data, sm->data.len);
					sms_mt.msg_len = sm->data.len;
				  
				 }
		         /*SPR 2260 end*/
					sms_mt.udh_len = sm->udh.len;
					memcpy((char *)sms_mt.udh, (char *)sm->udh.data, sm->udh.len);
					sms_type = MFW_NO_MESSAGE;
					TRACE_EVENT("read=free1");
					coll_sms_read = MFW_SMS_FREE;
#if defined (FF_WAP) && defined (FF_GPF_TCPIP)
                                    }
#endif

			  /*MC SPR 2260, need to call this function to update CPHS voicemail data*/ 
  	 				if (sms_check_voice_mail(sm->pid, sm->dcs, sm->adress,
                                       &sm->toa, &sm->udh) == MFW_SMS_VOICE)
                  		sms_mt.voicemail = TRUE;
  	 				else
  	 					sms_mt.voicemail = FALSE;
					/*whenever some operation is done the pref memory should be set back to SIM*/
				      if(sms_set_pref_memory(MFW_MEM_SIM)!=MFW_SMS_OK)
					{
    					TRACE_ERROR("sms_ok_CMGL():Not able to set pref memory");			
    					}					
					sms_signal(E_SMS_MT, &sms_mt);
				}
	        }
	          break;

	        case MFW_MSG_SUBMIT://x0pleela 01 Feb, 2006 DR: OMAPS00059261 /* SMS type: MO */
	        	{	 
#ifdef TI_PS_FF_CONC_SMS
					int i;/*SPR2260*/
			  		BOOL continue_flag=FALSE;
					USHORT conc_msg_ref;/*SPR 2260*/
	          		UBYTE conc_index;
				/*a0393213 OMAPS00109953*/
				T_CONC_BUF_ELEM* conc_buf_elem=NULL;
#endif /*TI_PS_FF_CONC_SMS*/					

				//Feb 28, 2004    REF: CRR 28809   Deepa M.D
				//Allocate sms_mo dynamically
				sms_mo = (T_MFW_SMS_MO*)mfwAlloc(sizeof(T_MFW_SMS_MO));  
				if (sms_mo == NULL)
				{
					TRACE_EVENT("sms_mo alloc failed");
					return ;
				}
			    	received_sms_type = SMS_getSMSType( &sm->udh, sm->adress, MODE3);/*a0393213 OMAPS00109953*/
				TRACE_EVENT_P1("SMS Type is = %d", received_sms_type);

#ifdef TI_PS_FF_CONC_SMS
				if (received_sms_type == CONCATE)
				{
					/*a0393213 OMAPS00109953 - memory added as parameter*/
					concatenated_status =concSMS_Collect(&conc_data_ext, sm, FALSE, SMS_STOR_NotPresent);
					TRACE_EVENT_P2("conc status: %d, conc length: %d", concatenated_status, conc_data_ext.len);
				
				
				TRACE_EVENT_P1("UDH Length is = %d", sm->udh.len);
				
				
				
					switch (concatenated_status)
					{
						
					case CONC_ERR_BUF_FULL:
						TRACE_EVENT("Concatented SMS buffer Full Error");
						sms_signal(E_SMS_ERR, NULL);
						break;
					case CONC_ERR_UNKN:
						TRACE_EVENT("Concatenated SMS error");
					case CONC_CONTINUED: //not last SMS
						TRACE_EVENT("Concatenation Continued");
						cmgr_awaiting_ok = FALSE; // Marcus: Issue 2260: 03/10/2002
						/*SPR 2260,if the index exists in the message table, continue processing message*/
#ifdef FF_MMI_SMS_DYNAMIC
						for (i=0; i <g_max_messages;i++)
#else
						for (i=0; i <MAX_MESSAGES;i++)
#endif
							if (msg_info[i].index == sm->msg_ref && msg_info[i].concat_status == MFW_SMS_CONC_PART)
								continue_flag = TRUE;
							if (!continue_flag)
								break;
					case CONC_COMPLETED:
						{ 	
							
							TRACE_EVENT("Concatenation Completed");
							
							memset(sms_mo, 0, sizeof(T_MFW_SMS_MO));
							/*SPR 2260*/
							/*a0393213 OMAPS00109953*/
							conc_msg_ref=concSMS_GetMsgRef(sm);
							conc_buf_elem=concSMS_GetFirstIndex_ext(conc_msg_ref,sm->adress);	
							conc_index=conc_buf_elem->rec_num;
							TRACE_EVENT_P2("msg_len: %d conc len: %d", sms_mo->msg_len, conc_data_ext.len);
							
#ifdef NO_ASCIIZ
							memcpy(sms_mo->dest_addr.tag,
								sm->alpha.data,
								sm->alpha.len);
							sms_mo->dest_addr.len = sm->alpha.len;
#else
							phb_Alpha2Gsm(&sm->alpha, (UBYTE *)sms_mo->dest_addr.tag);
#endif
							strcpy(sms_mo->dest_addr.number, sm->adress);
							sms_mo->dest_addr.ton = phb_cvtTon(sm->toa.ton);
							sms_mo->dest_addr.npi = (T_MFW_PHB_NPI)phb_cvtNpi(sm->toa.npi);
							if (sm->tosca.ton == TON_International)
								sprintf(sms_mo->sc_addr, "+%s", sm->sca);
							else
								strcpy((char *)sms_mo->sc_addr, sm->sca);
							
							sms_mo->prot_id = (UBYTE)sm->pid;
							sms_mo->dcs     = sm->dcs;
							
							/*MC SPR 2260, message index should be set to that of first index in
							concatenated message if this is a complete message*/
							if (concatenated_status == CONC_COMPLETED)
								sms_mo->msg_ref = conc_index;
							else
								sms_mo->msg_ref = sm->msg_ref;
							/*a0393213 OMAPS00109953 - the memory is of psa type. It should be converted to cmh type.
							Clean solution would be to move this ACI*/
							cmhSMS_getMemCmh(conc_buf_elem->mem,(T_ACI_SMS_STOR*)(&(sms_mo->memory )));															
							
							sms_mo->vp_mode = 0x03 & sm->fo << 2;
							sms_mo->vp_rel  = (UBYTE)sm->vp_rel;
							
							memcpy((char *)sms_mo->vp_abs.year, (char *)sm->scts.year, MAX_SCTP_DIGITS);
							memcpy((char *)sms_mo->vp_abs.month, (char *)sm->scts.month, MAX_SCTP_DIGITS);
							memcpy((char *)sms_mo->vp_abs.day, (char *)sm->scts.day, MAX_SCTP_DIGITS);
							memcpy((char *)sms_mo->vp_abs.hour, (char *)sm->scts.hour, MAX_SCTP_DIGITS);
							memcpy((char *)sms_mo->vp_abs.minute, (char *)sm->scts.minute, MAX_SCTP_DIGITS);
							memcpy((char *)sms_mo->vp_abs.second, (char *)sm->scts.second, MAX_SCTP_DIGITS);
							sms_mo->vp_abs.timezone = sm->scts.timezone;
							
							/* copy length of whole concat SMS*/
							if (conc_data_ext.len > MAX_MSG_LEN)
								sms_mo->msg_len = MAX_MSG_LEN;
							else
								sms_mo->msg_len = conc_data_ext.len;
							/*SPR 2260  if invalid message just set text to NULL*/
							if (sm->stat == SMS_STAT_Invalid)
								memset(sms_mo->sms_msg, NULL, MAX_MSG_LEN_ARRAY);
							else
							{	if (concatenated_status == CONC_COMPLETED)
							{	memcpy(sms_mo->sms_msg, (char*)conc_data_ext.data, sms_mo->msg_len);}
							else
							{	memcpy(sms_mo->sms_msg, (char*)sm->data.data, sm->data.len);
							sms_mo->msg_len = sm->data.len;
							}
							}
							
							/* Free memory allocated by the ACI */
							//  Jul 07, 2004    REF: CRR 15846     Deepa M.D
							//Free the memory only if conc_data_ext.data  is not NULL.
							if(conc_data_ext.data != NULL)
							{
								MFREE(conc_data_ext.data);
							}
							sms_type = MFW_NO_MESSAGE;
							/*whenever some operation is done the pref memory should be set back to SIM*/
						      if(sms_set_pref_memory(MFW_MEM_SIM)!=MFW_SMS_OK)
    							{
		    					TRACE_ERROR("sms_ok_CMGL():Not able to set pref memory");			
		    					}
							sms_signal(E_SMS_MO, sms_mo);
						}
					}
				}
				else
#endif /*TI_PS_FF_CONC_SMS*/					
				{
#ifdef NO_ASCIIZ
					memcpy(sms_mo->dest_addr.tag,
						sm->alpha.data,
						sm->alpha.len);
					sms_mo->dest_addr.len = sm->alpha.len;
#else
					phb_Alpha2Gsm(&sm->alpha, (UBYTE *)sms_mo->dest_addr.tag);
#endif
					strcpy(sms_mo->dest_addr.number, sm->adress);
					sms_mo->dest_addr.ton = phb_cvtTon(sm->toa.ton);
					sms_mo->dest_addr.npi = (T_MFW_PHB_NPI)phb_cvtNpi(sm->toa.npi);
					if (sm->tosca.ton == TON_International)
						sprintf(sms_mo->sc_addr, "+%s", sm->sca);
					else
						strcpy((char *)sms_mo->sc_addr, sm->sca);
					
					sms_mo->prot_id = (UBYTE)sm->pid;
					sms_mo->dcs     = sm->dcs;
					sms_mo->msg_ref = sm->msg_ref;
					sms_mo->vp_mode = 0x03 & sm->fo << 2;
					sms_mo->vp_rel  = (UBYTE)sm->vp_rel;
					/*a0393213 OMAPS00109953 - for normal sms the memory, from where the message is read,
					would be the preferred memory, which is set just before calling sAT_PlusCMGR*/
					sms_mo->memory=(T_MFW_SMS_MEM)memory.mem;
					
					memcpy((char *)sms_mo->vp_abs.year, (char *)sm->scts.year, MAX_SCTP_DIGITS);
					memcpy((char *)sms_mo->vp_abs.month, (char *)sm->scts.month, MAX_SCTP_DIGITS);
					memcpy((char *)sms_mo->vp_abs.day, (char *)sm->scts.day, MAX_SCTP_DIGITS);
					memcpy((char *)sms_mo->vp_abs.hour, (char *)sm->scts.hour, MAX_SCTP_DIGITS);
					memcpy((char *)sms_mo->vp_abs.minute, (char *)sm->scts.minute, MAX_SCTP_DIGITS);
					memcpy((char *)sms_mo->vp_abs.second, (char *)sm->scts.second, MAX_SCTP_DIGITS);
					sms_mo->vp_abs.timezone = sm->scts.timezone;
					
					TRACE_EVENT("Normal MO Message");
					sms_mo->msg_len = sm->data.len;
					#if defined (FF_WAP) && defined (FF_GPF_TCPIP)
				/* SPR#2086 - SH - If we've got a WAP SMS, replace the message
				 * with a string */
				if (wap_sms)
				{
					char *message = "WAP SMS";
					strcpy(sms_mt.sms_msg, message);
					sms_mt.msg_len = strlen(message);
				}
                            else
                            {
	#endif   
					/* CQ:16780 clear the buffer before reading in the retrieved SMS, prevents residual characters
					from previous message being displayed. 15-01-04 MZ*/
					
					memset(sms_mo->sms_msg, NULL, MAX_MSG_LEN_ARRAY);  
					
					/*SPR 2260 if invalid message just set text to NULL*/
					if (sm->stat != SMS_STAT_Invalid)
						memcpy(sms_mo->sms_msg, (char *)sm->data.data, sm->data.len);
					#if defined (FF_WAP) && defined (FF_GPF_TCPIP)
                            }
#endif
					sms_type = MFW_NO_MESSAGE;
					/*whenever some operation is done the pref memory should be set back to SIM*/
				      if(sms_set_pref_memory(MFW_MEM_SIM)!=MFW_SMS_OK)
					{
    					TRACE_ERROR("sms_ok_CMGL():Not able to set pref memory");			
    					}
					sms_signal(E_SMS_MO, sms_mo);
				}
				}
				//Feb 28, 2004    REF: CRR 28809   Deepa M.D
				//Deallocate the variable
				if(sms_mo!=NULL)
				{
					mfwFree((U8*)sms_mo, sizeof(T_MFW_SMS_MO)); 
					sms_mo=NULL;
				}
				
				break;
 
				//x0pleela 30 Jan, 2006 DR: OMAPS00059261
				//Store the status report message into sms_mt
#ifndef NEPTUNE_BOARD
				case MFW_MSG_STATUS_REPORT://x0pleela 01 Feb, 2006 DR: OMAPS00059261 /* SMS type: STATUS REPORT */
				{
				  char sms_str[MAX_STAT_MSG_LEN];
				  TRACE_EVENT("Status report");
#ifdef NO_ASCIIZ
      				  memcpy(sms_mt.orig_addr.tag,
      				  sm->alpha.data,
      				  sm->alpha.len);
        			  sms_mt.orig_addr.len = sm->alpha.len;
#else
				  phb_Alpha2Gsm(&sm->alpha, (UBYTE *)sms_mt.orig_addr.tag);
#endif
				  if (sm->toa.ton == TON_International)
				    sprintf(sms_mt.orig_addr.number, "+%s", sm->adress);
				  else	
				    strcpy(sms_mt.orig_addr.number, sm->adress);

  				  sms_mt.orig_addr.ton = phb_cvtTon(sm->toa.ton);
  				  sms_mt.orig_addr.npi = (T_MFW_PHB_NPI)phb_cvtNpi(sm->toa.npi);

				  if (sm->tosca.ton == TON_International)
				    sprintf(sms_mt.sc_addr, "+%s", sm->sca);
				  else
				    strcpy(sms_mt.sc_addr, sm->sca);

				  sms_mt.prot_id = (UBYTE)sm->pid;
				  sms_mt.dcs     = (UBYTE)sm->dcs & 0xFC;	/* Ignore message class */
				  sms_mt.rp		 = sm->fo >> 7 & 1;

				  // PATCH JPS 04.10 END
				  /* SBH */
				  if (sm->dcs & 0x10)		/* Bit 5 indicates that we should get message class from bits 0 & 1 */
				  {
				    sms_mt.ti = sm->dcs & 3;
				  }
				  else					/* Otherwise, set to default value */
				  {
				    sms_mt.ti = 1;
				  }
				  /* SBH end */

				  TRACE_EVENT_P2("dcs: %d, ti: %d", sms_mt.dcs, sms_mt.ti);

				  memcpy((char *)sms_mt.sctp.year, (char *)sm->scts.year, MAX_SCTP_DIGITS);
				  memcpy((char *)sms_mt.sctp.month, (char *)sm->scts.month, MAX_SCTP_DIGITS);
				  memcpy((char *)sms_mt.sctp.day, (char *)sm->scts.day, MAX_SCTP_DIGITS);
				  memcpy((char *)sms_mt.sctp.hour, (char *)sm->scts.hour, MAX_SCTP_DIGITS);
				  memcpy((char *)sms_mt.sctp.minute, (char *)sm->scts.minute, MAX_SCTP_DIGITS);
				  memcpy((char *)sms_mt.sctp.second, (char *)sm->scts.second, MAX_SCTP_DIGITS);
				  sms_mt.sctp.timezone = sm->scts.timezone;
				  sms_mt.msg_len = sm->data.len;
				  TRACE_EVENT_P2("msg_len %d, data.len %d", sms_mt.msg_len, sm->data.len);

				  memset(sms_str, 0, MAX_STAT_MSG_LEN);
				  memset(sms_mt.sms_msg, NULL, MAX_MSG_LEN_ARRAY);
				  
				  //x0pleela 01 Feb, 2006 DR: OMAPS00059261
				  //To display the message as "Delivered" or "Pending" as per the status
				  TRACE_EVENT_P1("sm->stat: %d", sm->tp_status);
  				  //x0pleela 06 Feb, 2006 DR: OMAPS00059261
				  //To display the message as "Delivered" or "Pending" as per the tp_status
				  if( !sm->tp_status ) 
				  {
				    strcpy(sms_str, "Delivered:");
				  }
				  else
				  {
  				    strcpy(sms_str, "Pending:");
				  }
				  strcat(sms_str, sm->adress);
				  memcpy(sms_mt.sms_msg, sms_str, strlen(sms_str));
				  sms_mt.msg_len = strlen(sms_str);
				  TRACE_EVENT_P2("sm->adress:%s, sms_str:%s ",sm->adress,sms_str);
 				  							
				  /*SPR 2260 end*/
				  sms_mt.udh_len = sm->udh.len;
				  memcpy((char *)sms_mt.udh, (char *)sm->udh.data, sm->udh.len);
				  sms_type = MFW_NO_MESSAGE;
				  TRACE_EVENT("read=free1");
				  coll_sms_read = MFW_SMS_FREE;

				//x0pleela 08 Feb, 2006 DR: OMAPS00059261
				//Replacing MT_STATUS_RPT with MFW_SMS_STATRPT as MT_STATUS_RPT is removed
		
				  //x0pleela 30 Jan, 2006 DR: OMAPS00059261
				  //Store sms message type
				  sms_mt.sms_msg_type = MFW_SMS_STATRPT;
				  sms_mt.voicemail = FALSE;
   				  /*whenever some operation is done the pref memory should be set back to SIM*/
				  if(sms_set_pref_memory(MFW_MEM_SIM)!=MFW_SMS_OK)
				  {
					TRACE_ERROR("sms_ok_CMGL():Not able to set pref memory");			
				  }				  
 				  sms_signal(E_SMS_MT, &sms_mt);
				}
				break;
#endif
			default:
				sms_signal(E_SMS_ERR, NULL);
			}
		}
		
		else
		{	TRACE_EVENT("rAT_plusCMGR: SMS pointer error");
		
		}
  }
  break;

      case MFW_CB_MESSAGE:
		  if (cbm != NULL) /*SPR2260 make sure we have a valid pointer*/
		  {
			  sms_cb.sn     = cbm->sn;
			  sms_cb.msg_id = cbm->mid;
			  sms_cb.dcs    = cbm->dcs;
			  sms_cb.page   = cbm->page;
			  sms_cb.pages  = cbm->pages;
			  sms_cb.msg_len = cbm->data.len;
			  memcpy((char *)sms_cb.cb_msg, (char *)cbm->data.data, cbm->data.len);
			  
			  /* send cell broadcast message event */
			  sms_type = MFW_NO_MESSAGE;
			  sms_signal(E_SMS_CB, &sms_cb);
		  }
		  else
			  TRACE_EVENT("rAT_plusCMGR: SMS CB pointer error");
		  break;
		  
      case MFW_NO_MESSAGE:
		  TRACE_EVENT("read=free no msg");
		  coll_sms_read = MFW_SMS_FREE;
		  if(coll_sms_read EQ MFW_SMS_WAIT)
			  coll_sms_read = MFW_SMS_FREE;
		  break;
  }
  
  sms_check_mt_update();
  
  {	
	  TRACE_EVENT_P4("CMGR Msg table: %d,%d,%d, %d", msg_info[0].index, msg_info[1].index,msg_info[2].index,msg_info[3].index);
	  TRACE_EVENT_P4("CMGR Msg table: %d,%d,%d, %d", msg_info[4].index, msg_info[5].index,msg_info[6].index,msg_info[7].index);
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6147)         MODULE  : MFW_SMS                  |
| STATE   : code                  ROUTINE : rAT_PlusCMGD             |
+--------------------------------------------------------------------+

  PURPOSE :
*/
GLOBAL void rAT_PlusCMGD ()
{
  TRACE_FUNCTION ("rAT_PlusCMGD");
//July 7, 2005 REF: CRR MMI-SPR-32179 x0018858
//Clears the incompleted message buffers.
#ifndef NEPTUNE_BOARD //a0876501 - Shashi - Guarded the below function call for Neptune.
  concSMS_clearIncompleteMsg();
#endif
/*a0393213 OMAPS00109953*/
  sms_ok_delete();
}
/*a0393213 OMAPS00109953 - updates the memory details*/
static void sms_update_memory()
{
	qAT_PlusCPMS_ext( CMD_SRC_LCL, &sim_memory.total, &sim_memory.used, &ffs_memory.total, &ffs_memory.used );

	memory.total = sim_memory.total+ffs_memory.total;
	memory.used = sim_memory.used+ffs_memory.used;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS           |
| STATE   : code                         ROUTINE : sms_store         |
+--------------------------------------------------------------------+

   PURPOSE :  Save a short message.

  GW 09/10/01 - Modified code to always save the maximum message length
*/
#ifdef NO_ASCIIZ
T_MFW sms_store(T_MFW_SMS_MO_TYPE type,
				CHAR *addr,
                UBYTE *msg,
                USHORT msg_len,
				CHAR *sca)
#else
T_MFW sms_store(T_MFW_SMS_MO_TYPE type,
				CHAR *addr,
				UBYTE *msg,
				USHORT msg_len,
				CHAR *sca)
// sbh - added length to the above definition, since '@' characters stored as NULL & will stop
// strlen from working properly.
#endif
{
#ifdef TI_PS_FF_CONC_SMS
  T_SM_DATA_EXT		conc_data_ext;
#else /*TI_PS_FF_CONC_SMS*/
  T_ACI_SM_DATA conc_data_ext;
#endif /*TI_PS_FF_CONC_SMS*/
  SHORT            rp;
  /*a0393213 OMAPS00109953*/
  T_MFW_SMS_MEM pref_mem=MFW_MEM_NOTPRESENT;
//Mar 22 2007 ER:OMAPS00113882 x0061088 (Prachi) 
   T_ACI_TOA set_toa;
   int addr_type;
   T_ACI_RETURN ACI_MFW_return;

  TRACE_FUNCTION ("sms_store()");

/*a0393213 OMAPS00109953 - identify where to store the new message*/
  if(sim_memory.used==sim_memory.total)
  	{
  		if(ffs_memory.used==ffs_memory.total)
  			{
  			    sms_signal(E_SMS_MEM_FULL, 0);
			    return MFW_SMS_FAIL;  			
  			}
		else
			{
				pref_mem=MFW_MEM_ME;
			}
  	}
  else
  	{
  		pref_mem=MFW_MEM_SIM;
  	}

     /*a0393213 OMAPS00109953 - set preferred memory before calling sAT_PlusCMGW*/
     if(sms_set_pref_memory(pref_mem)!=MFW_SMS_OK)
     	{
     	TRACE_ERROR("sms_read_msg_info():Not able to set pref memory");
    return MFW_SMS_FAIL;
  }

  if (type == MFW_SMS_REPLY     OR
      type == MFW_SMS_REPLY_SENT  )
    rp = 0;   /* reply path = 0 */
  else
    rp = -1;  /* reply path = default */

#ifdef TI_PS_FF_CONC_SMS
  conc_data_ext.data = msg;
  conc_data_ext.len = msg_len;
#else /*TI_PS_FF_CONC_SMS*/
  conc_data_ext.len = (UBYTE)msg_len;
  memcpy(conc_data_ext.data, msg,  conc_data_ext.len);
#endif /*TI_PS_FF_CONC_SMS*/

  if (type == MFW_SMS_REPLY_SENT  OR
      type == MFW_SMS_SUBMIT_SENT   )
    sentStatus =  SMS_STAT_StoSent;
  else
    sentStatus =  SMS_STAT_StoUnsent;



//Mar 22 2007 ER:OMAPS00113882 x0061088 (Prachi)
//setting type of address.
  addr_type = is_alphanumeric_addr(addr);


//Mar 22 2007 ER:OMAPS00113882 x0061088 (Prachi)
//sending toa instead of NULL



  if ( addr_type == TRUE )
  {
  	    set_toa.ton = TON_Alphanumeric;
        set_toa.npi = NPI_NotPresent;
        ACI_MFW_return = sAT_PlusCMGW(CMD_SRC_LCL, 0, addr, &set_toa, 
                       sentStatus, 0xFF,   
                       &conc_data_ext, sca,
                       NULL, rp);

  }
  else 
     ACI_MFW_return = sAT_PlusCMGW(CMD_SRC_LCL, 0, addr, NULL, 
                       sentStatus, 0xFF,   
                       &conc_data_ext, sca,
                       NULL, rp);

  if(ACI_MFW_return != AT_EXCT)
  {
    TRACE_EVENT("sAT_PlusCMGW error");
    return MFW_SMS_FAIL;
  }

  wr_addr = addr;
  sms_type = MFW_WR_NEW_MESSAGE;
  return MFW_SMS_OK;

 
}




/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS           |
| STATE   : code                         ROUTINE : sms_mt_store      |
+--------------------------------------------------------------------+

   PURPOSE :  Save a MT message.

*/

T_MFW sms_mt_store(T_MFW_SMS_MT *mt_msg, T_MFW_SMS_STAT stat)
{
#ifdef TI_PS_FF_CONC_SMS
  T_SM_DATA_EXT		conc_data_ext;
#else /*TI_PS_FF_CONC_SMS*/
  T_ACI_SM_DATA conc_data_ext;
#endif /*TI_PS_FF_CONC_SMS*/
  T_ACI_TOA        toa;
  /*a0393213 OMAPS00109953*/
  T_MFW_SMS_MEM pref_mem=MFW_MEM_NOTPRESENT;


  TRACE_FUNCTION ("sms_mt_store()");

  /* check whether memory is full */
  /*a0393213 OMAPS00109953 - identify where to store the new message*/
  if(sim_memory.used==sim_memory.total)
  	{
  		if(ffs_memory.used==ffs_memory.total)
  			{
  			    sms_signal(E_SMS_MEM_FULL, 0);
			    return MFW_SMS_FAIL;  			
  			}
		else
			{
				pref_mem=MFW_MEM_ME;
			}
  	}
  else
  	{
  		pref_mem=MFW_MEM_SIM;
  	}

     /*a0393213 OMAPS00109953 - set preferred memory before calling sAT_PlusCMGW*/
     if(sms_set_pref_memory(pref_mem)!=MFW_SMS_OK)
     	{
     	TRACE_ERROR("sms_read_msg_info():Not able to set pref memory");
 	return MFW_SMS_FAIL;
     	}

#ifdef TI_PS_FF_CONC_SMS
  conc_data_ext.len = mt_msg->msg_len;
  conc_data_ext.data = (UBYTE *)mt_msg->sms_msg;
#else /*TI_PS_FF_CONC_SMS*/
  conc_data_ext.len = (UBYTE)mt_msg->msg_len;
  memcpy(conc_data_ext.data, (UBYTE *)mt_msg->sms_msg, conc_data_ext.len);
#endif /*TI_PS_FF_CONC_SMS*/

  toa.ton = phb_ncvtTon(mt_msg->orig_addr.ton);
  toa.npi = phb_ncvtNpi(mt_msg->orig_addr.npi);


  if (sAT_PlusCMGW(CMD_SRC_LCL, 0, mt_msg->orig_addr.number,
                   &toa, sms_codeStatus(stat), 0,  /* no message reference */
                   &conc_data_ext,
                   mt_msg->sc_addr, NULL, -1) NEQ AT_EXCT)

  {
    TRACE_EVENT("sAT_PlusCMGW error");
    return MFW_SMS_FAIL;
  }

  sms_type = MFW_WR_NEW_MESSAGE;
  return MFW_SMS_OK;
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SMS              |
| STATE  : code                         ROUTINE: sms_change          |
+--------------------------------------------------------------------+


   PURPOSE :  Save a changed short message.

  GW 09/10/01 - Copy entire message length.

*/

#ifdef NO_ASCIIZ
T_MFW sms_change(CHAR *addr, UBYTE *msg,
                 USHORT msg_len, UBYTE index
#else
T_MFW sms_change(CHAR *addr, USHORT *msg, USHORT msg_len, UBYTE index
// sbh - added length to the above function call, since '@' characters stored as NULL & will stop
// strlen from working properly.
#endif
					, T_MFW_SMS_MEM sms_memory) /*a0393213 OMAPS00109953 - memory added as new parameter*/
{
#ifdef TI_PS_FF_CONC_SMS
  T_SM_DATA_EXT		conc_data_ext;
#else /*TI_PS_FF_CONC_SMS*/
  T_ACI_SM_DATA conc_data_ext;
#endif /*TI_PS_FF_CONC_SMS*/


  TRACE_FUNCTION ("sms_change()");
     /*a0393213 OMAPS00109953 - set preferred memory before calling sAT_PlusCMGW*/
     if(sms_set_pref_memory(sms_memory)!=MFW_SMS_OK)
     	{
     	TRACE_ERROR("sms_read_msg_info():Not able to set pref memory");
 	return MFW_SMS_FAIL;
     	}


#ifdef TI_PS_FF_CONC_SMS
  conc_data_ext.len = msg_len;
  conc_data_ext.data = (UBYTE *)msg;
#else /*TI_PS_FF_CONC_SMS*/
  conc_data_ext.len = (UBYTE)msg_len;
  memcpy( conc_data_ext.data,(UBYTE *)msg, conc_data_ext.len);
#endif /*TI_PS_FF_CONC_SMS*/

    if (sAT_PlusCMGW(CMD_SRC_LCL, index, addr, NULL,
                       SMS_STAT_StoUnsent, 0xFF,    /* message reference */
                       &conc_data_ext,
                       NULL, NULL, -1) NEQ AT_EXCT)
    return MFW_SMS_FAIL;



  sms_type = MFW_WR_CHG_MESSAGE;
  return MFW_SMS_OK;
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SMS              |
| STATE  : code                         ROUTINE: rAT_PlusCMGW        |
+--------------------------------------------------------------------+


   PURPOSE :  Call back for saving a short message.

*/

void rAT_PlusCMGW(UBYTE index, UBYTE numSeg, UBYTE mem)
{
  int i;
  T_MFW_PHB_ENTRY phb_entry;
  T_MFW_PHB_LIST  phb_list;
  SHORT           phb_index;
#ifdef NO_ASCIIZ
  T_MFW_PHB_TEXT  findText;
#endif

  TRACE_FUNCTION ("rAT_PlusCMGW()");
  TRACE_EVENT_P2("Index: %d, numseg: %d", index, numSeg);
  /*whenever some operation is done the pref memory should be set back to SIM*/
  if(sms_set_pref_memory(MFW_MEM_SIM)!=MFW_SMS_OK)
  {
	TRACE_ERROR("sms_ok_CMGL():Not able to set pref memory");			
  }
  memset(&phb_entry, 0, sizeof(phb_entry));
	memset(&phb_list, 0, sizeof(phb_list));
	phb_list.entry = &phb_entry;

  if (sms_type == MFW_WR_NEW_MESSAGE)
  {
#ifdef FF_MMI_SMS_DYNAMIC
    for (i=0; i<g_max_messages; i++)
#else
    for (i=0; i<MAX_MESSAGES; i++)
#endif
    {
      if (msg_info[i].index == -1)
        break;
    }
#ifdef FF_MMI_SMS_DYNAMIC
    if (i < g_max_messages)
#else
    if (i < MAX_MESSAGES)
#endif
    {
      msg_info[i].index = index;
	/*a0393213 OMAPS00109953*/
	/*The memory type returned is of psa type. It should be converted to cmh type.
	clean solution would be to move this to ACI*/
	cmhSMS_getMemCmh(mem,(T_ACI_SMS_STOR*)&(msg_info[i].mem));
      strncpy (msg_info[i].addr.number, wr_addr, MFW_NUM_LEN-1);
      msg_info[i].addr.number[MFW_NUM_LEN-1] = 0;
      /* search the name in phonebook */
#ifdef NO_ASCIIZ
      strcpy((char *)findText.data, wr_addr);
      findText.len = strlen(wr_addr);
      if (phb_find_entries  (PHB_ADN,	&phb_index,
                             MFW_PHB_NUMBER,
                             1, &findText,
                             &phb_list) EQ MFW_PHB_OK)
      {
        if (phb_list.num_entries == 1)
        {
          memcpy((char *)msg_info[i].addr.tag,
                 (char *)phb_list.entry->name.data,
                 phb_list.entry->name.len);
          msg_info[i].addr.len = phb_list.entry->name.len;
        }
      }
#else
      if (phb_find_entries  (PHB_ADN,	&phb_index,
                             MFW_PHB_NUMBER,
                             1, (CHAR *)wr_addr,
                             &phb_list) EQ MFW_PHB_OK)
      {
        if (phb_list.num_entries == 1)
        {
          strcpy((char *)msg_info[i].addr.tag, (char *)phb_list.entry->name);
        }
      }
#endif

      if (sentStatus EQ SMS_STAT_StoUnsent)
        msg_info[i].stat = MFW_SMS_STOR_UNSENT;
      else
        msg_info[i].stat = MFW_SMS_STOR_SENT;
	/*SPR 2260, we're making the assumption that only complete concat SMSs or single SMSs
	can be saved successfully*/
      if (numSeg == 1)
      	msg_info[i].concat_status = MFW_SMS_NO_CONC;
#ifdef TI_PS_FF_CONC_SMS	  
      else
      	msg_info[i].concat_status = MFW_SMS_CONC_WHOLE;
#endif /*TI_PS_FF_CONC_SMS*/

      msg_info[i].first_index = index;/*SPR 2260*/
	/*a0393213 OMAPS00109953*/
	/*The memory type returned is of psa type. It should be converted to cmh type.
	clean solution would be to move this to ACI*/
	cmhSMS_getMemCmh(mem,(T_ACI_SMS_STOR*)&(msg_info[i].first_mem));
      msg_info[i].msg_type = MFW_SMS_NotVOICE; // ???
      memory.used +=numSeg;/*SPR2260*/
    }
  }
  /*SPR 2260*/
	/*a0393213 OMAPS00109953*/
	sms_update_memory();
  TRACE_EVENT_P2("memory used is = %d, of %d", memory.used, memory.total);
 if(memory.total == memory.used)
  {
  	
  	sms_signal(E_SMS_MEM_FULL, NULL);
  }  
  sms_type = MFW_NO_MESSAGE;

  sms_check_mt_update();

  sms_signal(E_SMS_SAVE_AVAIL, &index);
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SMS              |
| STATE  : code                         ROUTINE: sms_msg_delete      |
+--------------------------------------------------------------------+


   PURPOSE :  Delete a short message.

*/
/*a0393213 OMAPS00109953 - memory added as parameter*/
T_MFW sms_msg_delete(UBYTE index, T_MFW_SMS_MEM sms_memory) 
{
  TRACE_FUNCTION ("sms_msg_delete()");

     /*a0393213 OMAPS00109953 - set preferred memory before calling sAT_PlusCMGD*/
     if(sms_set_pref_memory(sms_memory)!=MFW_SMS_OK)
     	{
     	TRACE_ERROR("sms_read_msg_info():Not able to set pref memory");
 	return MFW_SMS_FAIL;
     	}
  coll_sms_delete = MFW_SMS_WAIT;

#ifdef FF_2TO1_PS
  if (sAT_PlusCMGD(CMD_SRC_LCL, index, MNSMS_CMGD_DEL_INDEX) NEQ AT_EXCT)
#else
/* a0876501 (Shashi), 08-Nov-2005, OMAPS00050772.
Added new parameter to be passed (3rd Parameter is set to 0). */
  if (sAT_PlusCMGD(CMD_SRC_LCL, index,0) NEQ AT_EXCT)
#endif
    return MFW_SMS_FAIL;
/*MC*/
  deleting_index = index;
  deleting_memory= sms_memory;
  return MFW_SMS_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SMS              |
| STATE  : code                         ROUTINE: sms_ok_change       |
+--------------------------------------------------------------------+

   PURPOSE :  Call back for the status change of a short message.form
              UNREAD to READ

*/

void sms_ok_change()
{



//  Dec 23, 2004 REF: CRR MMI-SPR-27741 xkundadu 
//  Issue description:  BMI: crashing when using the function : sms_index_list() located in mfw
//  Solution: Changed the msg_list variable to array of T_MFW_SMS_IDX.

//	May 27, 2005    MMI-FIX-29869 x0018858 - Modified from static to dynamic variable
#ifdef FF_MMI_SMS_DYNAMIC
	T_MFW_SMS_IDX* msg_list;
#else
    T_MFW_SMS_IDX msg_list[MAX_MESSAGES];
#endif
    int noOfVMSMS = 0; // stores number of VM sms
    int i;
//	May 27, 2005    MMI-FIX-29869 x0018858 - Modified from static to dynamic variable
#ifdef FF_MMI_SMS_DYNAMIC
	msg_list = (T_MFW_SMS_IDX*)ALLOC_MEMORY(g_max_messages*sizeof(T_MFW_SMS_IDX));
#endif   

    TRACE_FUNCTION ("sms_ok_change()");

    /* Marcus: Issue 2260: 03/10/2002: Start */
    if (cmgr_awaiting_ok == TRUE)
    {
        cmgr_awaiting_ok = FALSE;
    /* Marcus: Issue 2260: 03/10/2002: End */

        if (fg)
        {
            fg = 0;
            sms_update_msg_info(MFW_SMS_READ);
            sms_signal(E_SMS_OK, 0);
        }

        sms_check_mt_update();

        
//  Dec 23, 2004 REF: CRR MMI-SPR-27741 xkundadu 
//  Issue description:  BMI: crashing when using the function : sms_index_list() located in mfw
//  Solution: Changed the msg_list variable to array of T_MFW_SMS_IDX.
#ifdef FF_MMI_SMS_DYNAMIC
        noOfVMSMS = sms_index_list(MFW_SMS_VOICE, msg_list, g_max_messages) ;
#else
        noOfVMSMS = sms_index_list(MFW_SMS_VOICE, msg_list, MAX_MESSAGES) ;
#endif
	 TRACE_EVENT_P1("No of VM sms:%d", noOfVMSMS);
	// Checking the number of VM sms.
        if( noOfVMSMS > 0)
        {
		//Delete the vm sms which is read.
		for(i=0;i<noOfVMSMS;i++)
		{
			//Find out the VM sms which is already read and delete it.
			if(msg_list[i].stat == MFW_SMS_READ)
			{
    				/*a0393213 OMAPS00109953 - memory type passed as parameter*/
				sms_msg_delete(msg_list[i].index,msg_list[i].memory);				
				break;
			}
		}
        }
	
    /* Marcus: Issue 2260: 03/10/2002: Start */
    }
    else
    {
         TRACE_EVENT("Ignoring unexpected OK");
    }
#ifdef FF_MMI_SMS_DYNAMIC
	//	May 27, 2005    MMI-FIX-29869 x0018858 - Freeing the memory.
	FREE_MEMORY((U8*)msg_list, g_max_messages*sizeof(T_MFW_SMS_IDX));
#endif
    /* Marcus: Issue 2260: 03/10/2002: End */
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SMS              |
| STATE  : code                         ROUTINE: sms_ok_delete       |
+--------------------------------------------------------------------+


   PURPOSE :  Call back for deleting a short message.

*/

void sms_ok_delete()
{
 
  	TRACE_FUNCTION ("sms_ok_delete()");
	/*SPR2530, moved flag claering code here from delete_sms_from_table*/
	
  	 if(coll_sms_delete EQ MFW_SMS_WAIT)
    coll_sms_delete = MFW_SMS_FREE;
	/*whenever some operation is done the pref memory should be set back to SIM*/
      if(sms_set_pref_memory(MFW_MEM_SIM)!=MFW_SMS_OK)
      	{
		TRACE_ERROR("sms_ok_CMGL():Not able to set pref memory");			
	}	 
  	/*SPR 1833, replaces section of code with function call*/
	/*a0393213 OMAPS00109953 - memory type passed as parameter*/
  	if (delete_sms_from_table (deleting_index, deleting_memory))
  	{
	   	deleting_index = -1; /*Hope this fixes multiple deletions*/
		/*a0393213 OMAPS00109953*/
		deleting_memory = MFW_MEM_NOTPRESENT;
		/*SPR 2260, we have to request the SMS storage status to update memory.used*/
		/*as we have no idea how many segments the deleted SMS had*/
	        int_req = POST_DELETE_CHECK;/*update memory variable, don't send an event to MMI*/

		/* cq16875 If the SIM Entity is busy then don't send an E_SmS_OK event to the MMI
		    12-02-04 MZ.*/
		if(AT_BUSY != qAT_PlusCPMS(CMD_SRC_LCL))
	  		sms_signal(E_SMS_OK, 0);

		sms_signal(E_SMS_MEM_FREE, 0);

	  	sms_check_mt_update();
  	}
  	{	
		TRACE_EVENT_P4("CMGD Msg table: %d,%d,%d, %d", msg_info[0].index, msg_info[1].index,msg_info[2].index,msg_info[3].index);
		TRACE_EVENT_P4("CMGD Msg table: %d,%d,%d, %d", msg_info[4].index, msg_info[5].index,msg_info[6].index,msg_info[7].index);
	}
}
/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SMS              |
| STATE  : code                         ROUTINE: delete_sms_from_table       |
+--------------------------------------------------------------------+


   PURPOSE :  deletes a message from the MFW message table
   created for SPR2260

*/
/*a0393213 OMAPS00109953 - memory type added as a parameter*/
BOOL delete_sms_from_table (SHORT index, T_MFW_SMS_MEM memory)
{
  int i;
 
  TRACE_FUNCTION ("delete_sms_from_table()");

 /*SPR2530, removed flag clearing code*/
#ifdef FF_MMI_SMS_DYNAMIC
  for (i=0; i<g_max_messages; i++)
#else
  for (i=0; i<MAX_MESSAGES; i++)
#endif
  {
    /*a0393213 OMAPS00109953 - memory is also taken into consideration*/
    if (msg_info[i].index == index&& msg_info[i].mem == memory) 
    {
      msg_info[i].index = -1;
	/*a0393213 OMAPS00109953*/
      msg_info[i].mem = MFW_MEM_NOTPRESENT;
      msg_info[i].stat = MFW_SMS_UNKNOWN;
      msg_info[i].msg_type = MFW_SMS_UNKNOWN;
      msg_info[i].addr.number[0] = '\0';
      msg_info[i].addr.tag[0] = '\0';
      msg_info[i].addr.ton = MFW_TON_UNKNOWN;
      msg_info[i].addr.npi = MFW_NPI_UNKNOWN;
      /*SPR 2260, reset new elements*/
      msg_info[i].first_index = -1;
	/*a0393213 OMAPS00109953*/
	msg_info[i].first_mem=MFW_MEM_NOTPRESENT;
      msg_info[i].concat_status = MFW_SMS_NO_CONC;
      return TRUE;
    }
  }
  
return FALSE;
}
/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SMS              |
| STATE  : code                         ROUTINE: sms_command         |
+--------------------------------------------------------------------+


   PURPOSE :  Send a mobile originated short massage command.

*/

T_MFW sms_command(CHAR *dest_addr, UBYTE command, UBYTE msg_ref)
{
  TRACE_FUNCTION ("sms_command()");

  switch (command)
  {
    case SMS_ENQ_PREV_SMS:
      if (sAT_PlusCMGC(CMD_SRC_LCL,
                       0x22,       // SMS command with Status Report Request
                       0x00,       // Command Type
                       0x00,       // Protocol Identifier
                       msg_ref,    // message reference of previous SMS
                       dest_addr,  // destination address
                       NULL,       // type of destination address
                       NULL)       // command data
                       NEQ AT_EXCT)
        return MFW_SMS_FAIL;
      break;

    case SMS_CAN_STAT_REQ:
      if (sAT_PlusCMGC(CMD_SRC_LCL,
                       0x02,       // SMS command without Status Report Request
                       0x01,       // Command Type
                       0x00,       // Protocol Identifier
                       msg_ref,    // message reference of previous SMS
                       dest_addr,  // destination address
                       NULL,       // type of destination address
                       NULL)       // command data
                       NEQ AT_EXCT)
        return MFW_SMS_FAIL;
      break;

    case SMS_DEL_PREV_SMS:
      if (sAT_PlusCMGC(CMD_SRC_LCL,
                       0x02,       // SMS command without Status Report Request
                       0x02,       // Command Type
                       0x00,       // Protocol Identifier
                       msg_ref,    // message reference of previous SMS
                       dest_addr,  // destination address
                       NULL,       // type of destination address
                       NULL)       // command data
                       NEQ AT_EXCT)
        return MFW_SMS_FAIL;
      break;

    case SMS_REQ_STAT_REP:
      if (sAT_PlusCMGC(CMD_SRC_LCL,
                       0x02,       // SMS command without Status Report Request
                       0x03,       // Command Type
                       0x00,       // Protocol Identifier
                       msg_ref,    // message reference of previous SMS
                       dest_addr,  // destination address
                       NULL,       // type of destination address
                       NULL)       // command data
                       NEQ AT_EXCT)
        return MFW_SMS_FAIL;
      break;
    default:
      /*
       * non-supported command type
       */
      return MFW_SMS_FAIL;
  }
  return MFW_SMS_OK;
}

/*
+-------------------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SMS                        |
| STATE  : code                         ROUTINE: sms_set_SrvCntr_number           |
+-------------------------------------------------------------------------------+

   PURPOSE :  MMI sets the Service Center Number on the SIM

*/

T_MFW sms_set_SrvCntr_number(T_MFW_SMS_INFO *sms_info)
{
  T_ACI_RETURN     res;

  TRACE_FUNCTION ("sms_set_SrvCntr_Nmbr_parameter()");


  if(sms_info == 0)
      return MFW_SMS_FAIL;

  if (sAT_PlusCSCA(CMD_SRC_LCL, (char *)sms_info->sc_addr, 0) NEQ AT_CMPL)
          	return MFW_SMS_FAIL;
      /* Send text mode parameters to ACI */


   if (sAT_PlusCSMP(CMD_SRC_LCL, ACI_NumParmNotPresent, ACI_NumParmNotPresent,
                       NULL, NULL, ACI_NumParmNotPresent,
                       ACI_NumParmNotPresent) NEQ AT_CMPL)
      	    return MFW_SMS_FAIL;

      res = sAT_PlusCSAS(CMD_SRC_LCL, (SHORT) 1);
      switch (res)
      {
        case AT_FAIL:
          return MFW_SMS_FAIL;
        case AT_CMPL:
          sms_signal(E_SMS_OK, 0);
          return MFW_SMS_OK;
        case AT_EXCT:
          return MFW_SMS_OK;
        default:
          return MFW_SMS_FAIL;
      }

  /*a0393213 compiler warnings removal - return statement removed*/
}

/*
+-------------------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SMS                        |
| STATE  : code                         ROUTINE: sms_set_dcs_number           |
+-------------------------------------------------------------------------------+

   PURPOSE :  MMI sends the dcs data to the sim (control 7bit ascii (compressed),
			  8bit or 16bit (unicode) uncompressed data


*/
T_MFW sms_set_dcs_number(T_MFW_SMS_INFO *sms_info)
{

  TRACE_FUNCTION ("sms_set_dcs_parameter()");


  if(sms_info == 0)
      return MFW_SMS_FAIL;


   if (sAT_PlusCSMP(CMD_SRC_LCL, ACI_NumParmNotPresent, ACI_NumParmNotPresent,
                       NULL, NULL, ACI_NumParmNotPresent,
                       sms_info->dcs ) NEQ AT_CMPL)
      	    return MFW_SMS_FAIL;

  return MFW_SMS_OK;
}


/*
+-------------------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SMS                        |
| STATE  : code                         ROUTINE: sms_set_val_period           |
+-------------------------------------------------------------------------------+

   PURPOSE :  MMI sends the Validity Period to the ACI


*/
T_MFW sms_set_val_period(T_MFW_SMS_INFO *sms_info)
{

  TRACE_FUNCTION ("sms_set_val_period()");

  if(sms_info == 0)
      return MFW_SMS_FAIL;

	/*Jan 20, 2006    REF: DRT OMAPS00053377 xdeepadh
	MMI  supports only the relative validity period format.
	Hence, set the validity period format to relative for all the cases*/
	 if ((sms_info->vp_mode EQ SMS_VPF_NUMERICAL) ||(sms_info->vp_mode EQ SMS_VPF_STRING) ||(sms_info->vp_mode EQ SMS_VPF_NOTPRESENT))
      	{
      		
      		 TRACE_EVENT_P1("sms_info->vp_mode  %d",sms_info->vp_mode);
		TRACE_EVENT("MMI  supports only the relative validity period format ");	

		/*Set the TP-Validity-Period-Format (TP-VPF)  bits  to relative format	*/
		sms_info->first_oct  = sms_info->first_oct  | 0x10 ;

      	}

   	 TRACE_EVENT_P1("first_oct is %d",sms_info->first_oct);
	 
   //Send text mode parameters to ACI 
#ifndef EMO_SIM
   if (sAT_PlusCSMP(CMD_SRC_LCL, sms_info->first_oct, sms_info->vp_rel,
                       NULL, NULL, ACI_NumParmNotPresent,
                       ACI_NumParmNotPresent) NEQ AT_CMPL)
      	    return MFW_SMS_FAIL;
#endif
   return MFW_SMS_OK;
}

//Feb 22, 2006 DR:OMAPS00068268 x0035544 
BOOL Sms_Check_PID(SHORT pid)
{
TRACE_FUNCTION("Sms_Check_PID()");

if(pid & BIT_7)    
{
	if(!(pid & BIT_6)) // Bit 7 = 1 and Bit 6 = 0 
  		return TRUE;
} 
else  
 {
	 if(pid & BIT_6) // Bit 7 = 0 and Bit 6 = 1, Bits 5...0 some values are reserved
	 {
	     if(((pid >= 0x48) AND (pid <= 0x5E)) OR 
		 ((pid >= 0x60) AND (pid <= 0x7B)))
			 return TRUE;
	 }
	 else  
	 {
	  if(pid & BIT_5) // Bit 7 = 0, Bit 6 = 0 and Bit 5 = 1, Bits 4...0 some values are reserved
	   {
	 	  if((pid EQ 0x2E) OR (pid EQ 0x2F) OR 
		    ((pid >= 0x33) AND (pid <= 0x37)))
			  return TRUE;
	   }
    	 }
}
			
return FALSE;
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SMS              |
| STATE  : code                         ROUTINE: sms_parameter       |
+--------------------------------------------------------------------+


   PURPOSE :  MMI sets SMS parameters.

*/


T_MFW sms_parameter(T_MFW_SMS_INFO *sms_info, BOOL fGet)
{
  T_ACI_TOA        sc_typ;

  CHAR				sca[MAX_LEN];
  SHORT            first_oct=0x0000;

  SHORT            vprel;
  T_ACI_VP_ABS     vpabs;
  SHORT            pid;
  SHORT            dcs;
  T_ACI_RETURN     res;
  //x0035544 Feb 22, 2006 DR:OMAPS00068268
  BOOL PIDReserved;

  TRACE_FUNCTION ("sms_parameter()");

  if(sms_info == 0)
      return MFW_SMS_FAIL;

  if(fGet) {

      memset(sms_info, 0, sizeof(T_MFW_SMS_INFO));

      if (qAT_PlusCSCA(CMD_SRC_LCL,
                       sca,
                       &sc_typ) EQ AT_FAIL
          OR qAT_PlusCSMP(CMD_SRC_LCL,
                          &first_oct,
                          &vprel,
                          &vpabs,
                          NULL,
                          &pid,
                          &dcs ) EQ AT_FAIL)
      {

        return MFW_SMS_FAIL;
      }

      if (sc_typ.ton == TON_International)
        sprintf(sms_info->sc_addr, "+%s", sca);
      else
      strcpy((char *)sms_info->sc_addr, (char *)sca);
	//x0035544 Feb 22, 2006 DR:OMAPS00068268	
	PIDReserved = Sms_Check_PID(pid);
	/* If pid value falls in to any of Reserved category then set it to 0 */
	if(PIDReserved) 
		pid = 0;
      sms_info->prot_id = pid;
      sms_info->dcs = dcs;
      sms_info->vp_rel = (UBYTE)vprel;
      memcpy(&sms_info->vp_abs, &vpabs, sizeof(T_MFW_SMS_SCTP));

      if ((first_oct & 0x18) EQ 0x10)
        sms_info->vp_mode = SMS_VPF_NUMERICAL;
      if ((first_oct & 0x18) EQ 0x18)
        sms_info->vp_mode = SMS_VPF_STRING;
      if ((first_oct & 0x18) EQ 0x00)
        sms_info->vp_mode = SMS_VPF_NOTPRESENT;

	sms_info->rd = first_oct >> 2 & 1;
	sms_info->rp = first_oct >> 7 & 1;
	sms_info->srr = first_oct >> 5 & 1;
	/*Jan 20, 2006    REF: DRT OMAPS00053377 xdeepadh
	Store the first octet details in the structure.*/
	sms_info->first_oct = first_oct;

  } else {

      if (sAT_PlusCSCA(CMD_SRC_LCL, (char *)sms_info->sc_addr, 0) NEQ AT_CMPL)
      	{
        	return MFW_SMS_FAIL;
      	}
      /* Send text mode parameters to ACI */
      if (sms_info->vp_mode EQ SMS_VPF_NUMERICAL)
        first_oct = 0x11;
      if (sms_info->vp_mode EQ SMS_VPF_STRING)
        first_oct = 0x19;
      if (sms_info->vp_mode EQ SMS_VPF_NOTPRESENT)
        first_oct = 0x01;

      first_oct |= sms_info->rd << 2;
      first_oct |= sms_info->rp << 7;
      first_oct |= sms_info->srr << 5;

      if (sAT_PlusCSMP(CMD_SRC_LCL, first_oct, sms_info->vp_rel,
                       (T_ACI_VP_ABS *)&sms_info->vp_abs, NULL, sms_info->prot_id,
                       sms_info->dcs) NEQ AT_CMPL)
      	{
    	    return MFW_SMS_FAIL;
      	}
      res = sAT_PlusCSAS(CMD_SRC_LCL, (SHORT) 1);
      switch (res)
      {
        case AT_FAIL:
          return MFW_SMS_FAIL;
        case AT_CMPL:
          sms_signal(E_SMS_OK, 0);
          return MFW_SMS_OK;
        case AT_EXCT:
          return MFW_SMS_OK;
        default:
          return MFW_SMS_FAIL;
      }
  }

  return MFW_SMS_OK;
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SMS              |
| STATE  : code                         ROUTINE: sms_busy_ind        |
+--------------------------------------------------------------------+


   PURPOSE :  SMS busy indication to MMI.

*/

void sms_busy_ind(void) // bugfix for SIM refresh
{
  int_req = 0;
  sms_signal(E_SMS_BUSY, 0);
}


/*
+----------------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SMS                      |
| STATE  : code                         ROUTINE: sms_set_cell_broadcast_info |
+----------------------------------------------------------------------------+


   PURPOSE :  MMI sets SMS cell broadcast parameters.

*/

T_MFW sms_set_cell_broadcast_info(UBYTE mode, T_MFW_SMS_CB_INFO *sms_cb_info)
{
  T_ACI_RETURN     res;

  TRACE_FUNCTION ("sms_set_cell_broadcast_info()");
	/*SPR 2470, if the Channel ID list is empty, the mode logic is inverted due to the warped logic of 
	AT commands*/
	
	/*if we are disabling cell broadcast*/
	if (mode == 1)
	{	/* and the channel ID list is empty*/
		if (sms_cb_info->msg_id[0] == 0xFFFF)
			mode =0;
	}
	/*if we are enabling cell broadcast*/
	else
	{	/* and the channel ID list is empty*/
		if (sms_cb_info->msg_id[0] == 0xFFFF)
			mode =1;
	}

  if (sAT_PlusCSCB(CMD_SRC_LCL, (T_ACI_CSCB_MOD)mode, sms_cb_info->msg_id, sms_cb_info->dcs) NEQ AT_CMPL) /*a0393213 compiler warnings removal - explicit typecasting done*/
    return MFW_SMS_FAIL;

  res = sAT_PlusCSAS(CMD_SRC_LCL, (SHORT) 1);
  switch (res)
  {
    case AT_FAIL:
      return MFW_SMS_FAIL;
    case AT_CMPL:
      sms_signal(E_SMS_OK, 0);
      return MFW_SMS_OK;
    case AT_EXCT:
      return MFW_SMS_OK;
    default:
      return MFW_SMS_FAIL;
  }

    /*a0393213 compiler warnings removal - return statement removed*/
}

/*
+----------------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SMS                      |
| STATE  : code                         ROUTINE: sms_get_cell_broadcast_info |
+----------------------------------------------------------------------------+


   PURPOSE :  MMI gets SMS cell broadcast parameters.

*/
#ifdef FF_2TO1_PS
T_MFW sms_get_cell_broadcast_info(T_MFW_SMS_CB_INFO *sms_cb_info)
{
  T_ACI_CSCB_MOD   mode;
  USHORT            mids[CBM_MAX_RANGE_NUMBER];
  UBYTE            dcss[CBM_MAX_RANGE_NUMBER];
  UBYTE            c_mids, c_dcss;
  TRACE_FUNCTION ("sms_get_cell_broadcast_info()");

  memset(dcss,0xFF,CBM_MAX_RANGE_NUMBER);
  memset(mids,0xFFFF,CBM_MAX_RANGE_NUMBER);

  if (qAT_PlusCSCB(CMD_SRC_LCL, &mode, &c_mids, mids, &c_dcss, dcss) EQ AT_CMPL)
  {

    sms_cb_info->mode   = mode;
    memcpy((char *)sms_cb_info->dcs, (char *)dcss, CBM_MAX_RANGE_NUMBER);

     memcpy((char *)sms_cb_info->msg_id, (char *)mids, sizeof(mids));

    return MFW_SMS_OK;
  }
  else
    return MFW_SMS_FAIL;
}
#else
T_MFW sms_get_cell_broadcast_info(T_MFW_SMS_CB_INFO *sms_cb_info)
{
  T_ACI_CSCB_MOD   mode;
  USHORT            mids[MAX_IDENTS];
  UBYTE            dcss[MAX_IDENTS];

  TRACE_FUNCTION ("sms_get_cell_broadcast_info()");

  memset(dcss,0xFF,MAX_IDENTS);
  memset(mids,0xFFFF,MAX_IDENTS);

  if (qAT_PlusCSCB(CMD_SRC_LCL, &mode, mids, dcss) EQ AT_CMPL)
  {

    sms_cb_info->mode   = mode;
    memcpy((char *)sms_cb_info->dcs, (char *)dcss, MAX_IDENTS);

     memcpy((char *)sms_cb_info->msg_id, (char *)mids, sizeof(mids));

    return MFW_SMS_OK;
  }
  else
    return MFW_SMS_FAIL;
}


#endif

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SMS              |
| STATE  : code                         ROUTINE: rAT_PlusCMGS        |
+--------------------------------------------------------------------+


   PURPOSE :  Call back to inform about the successful sending of
              a mobile originated short massage.

*/

void rAT_PlusCMGS(UBYTE mr, UBYTE numSeg)
{
  TRACE_FUNCTION ("rAT_PlusCMGS()");

   if(coll_sms_send EQ MFW_SMS_WAIT)
		coll_sms_send = MFW_SMS_FREE;

  sms_check_mt_update();
#if defined (FF_WAP) && defined (FF_GPF_TCPIP)

  /* SPR#2086 - SH - Check if it's a WAP SMS */
  if (sms_wap_sent_flag)
  {
  	sms_wap_sent_flag = FALSE;
    sms_signal(E_SMS_WAP_MO_SENT, &mr);
  }
  else
  {
    sms_signal(E_SMS_MO_AVAIL, &mr);
  }
#else
  sms_signal(E_SMS_MO_AVAIL, &mr);
#endif
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SMS              |
| STATE  : code                         ROUTINE: rAT_PlusCMGC        |
+--------------------------------------------------------------------+


   PURPOSE :  Call back to inform about the successful sending of a
              mobile originated short massage command.

*/

void rAT_PlusCMGC(UBYTE mr)
{
  TRACE_FUNCTION ("rAT_PlusCMGC()");

  sms_check_mt_update();

  sms_signal(E_SMS_CMD_AVAIL, &mr);
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SMS              |
| STATE  : code                         ROUTINE: rAT_PlusCMS         |
+--------------------------------------------------------------------+


   PURPOSE :  Report the error result on failures of the last
              executed functions

*/

void rAT_PlusCMS (T_ACI_AT_CMD cmdId, T_ACI_CMS_ERR err, T_EXT_CMS_ERROR *ce)
{
  T_MFW_SMS_CMD_IND cmd_ind;

  TRACE_FUNCTION ("rAT_PlusCMS()");
  TRACE_EVENT_P1("Error: %d",cmdId);

  switch (cmdId)
  {
    case AT_CMD_CMGS:
    case AT_CMD_CMSS:
      cmd_ind = CMD_SMS_SUBMIT;
      coll_sms_send = MFW_SMS_FREE;

#if defined (FF_WAP) && defined (FF_GPF_TCPIP)
        
        /* SPR#2086 - SH - Check if it's a WAP SMS */
 	  if (sms_wap_sent_flag)
  	  {
    	    sms_wap_sent_flag = FALSE;
            sms_signal(E_SMS_WAP_ERR, &cmd_ind);
        }
        else
        {
            sms_signal(E_SMS_ERR, &cmd_ind);
        }
#else
      sms_signal(E_SMS_ERR, &cmd_ind);
#endif
      break;

    case AT_CMD_CMGC:
      cmd_ind = CMD_SMS_COMMAND;
      coll_sms_req = MFW_SMS_FREE;
      sms_signal(E_SMS_ERR, &cmd_ind);
      break;

    case AT_CMD_CPMS:
      cmd_ind = CMD_SMS_MEMORY;
      sms_signal(E_SMS_ERR, &cmd_ind);
      break;

    case AT_CMD_CMGR:
      cmd_ind = CMD_SMS_READ;
      coll_sms_read = MFW_SMS_FREE;
      sms_signal(E_SMS_ERR, &cmd_ind);
      break;

    case AT_CMD_CMGL:
      cmd_ind = CMD_SMS_READ;
      coll_sms_read = MFW_SMS_FREE;
      sms_signal(E_SMS_ERR, &cmd_ind);
      sms_ok_CMGL(); /*JVJ SPR1118 This case has been added to handle the Error of CMGL
                       when the SIM Card has no messages. Even without SMS in the SIM card,
                       the call to qAT_PlusCPMS to query the memory status is still needed */
      break;

    case AT_CMD_CMGW:
      sms_type = MFW_NO_MESSAGE;
      cmd_ind = CMD_SMS_STORE;
      coll_sms_send = MFW_SMS_FREE;
      sms_signal(E_SMS_ERR, &cmd_ind);
      break;

    case AT_CMD_CMGD:
      cmd_ind = CMD_SMS_SET_PARA;
      coll_sms_delete = MFW_SMS_FREE;
      sms_signal(E_SMS_ERR, &cmd_ind);
      break;

    case AT_CMD_CSAS:
      cmd_ind = CMD_SMS_DELETE;
      sms_signal(E_SMS_ERR, &cmd_ind);
      break;
  }

  sms_check_mt_update();
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)  MODULE  : MFW_SMS                  |
| STATE   : code                  ROUTINE : sms_get_memset           |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to request the preferred SMS memory
*/
T_MFW sms_get_memset(void)
{
    TRACE_FUNCTION ("sms_get_memset()");

    coll_sms_req = MFW_SMS_WAIT;

    if (qAT_PlusCPMS(CMD_SRC_LCL) EQ AT_EXCT)
        return MFW_SMS_OK;
    return MFW_SMS_FAIL;
}


/*
+---------------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS                  |
| STATE   : code                         ROUTINE : rAT_PlusCPMS             |
+---------------------------------------------------------------------------+

  PURPOSE : Call back for request of preferred SMS memory.
*/

GLOBAL void rAT_PlusCPMS (T_ACI_SMS_STOR_OCC * mem1,
                          T_ACI_SMS_STOR_OCC * mem2,
                          T_ACI_SMS_STOR_OCC * mem3)
{
    T_MFW_SMS_MEM_INFO sms_mem;

    TRACE_FUNCTION ("rAT_PlusCPMS");
    /*a0393213 OMAPS00109953*/
    /*update memory info*/
    switch(mem1->mem)
    	{
    	case SMS_STOR_Sm:
		sim_memory.total=mem1->total;
		sim_memory.used=mem1->used;
		break;
	case SMS_STOR_Me:
		ffs_memory.total=mem1->total;
		ffs_memory.used=mem1->used;
		break;
	default:
		TRACE_ERROR("rAT_PlusCPMS():Unknown memory");
    	}
		
    memory.mem=sms_mem.mem=mem1->mem;
    memory.total=sms_mem.total=sim_memory.total+ffs_memory.total;
    memory.used=sms_mem.used=sim_memory.used+ffs_memory.used;
    /* HLE temp for testing iface and for WIN32 simulation */

#if defined (WIN32)
	mem1->total = 10;
#endif

	if(coll_sms_req EQ MFW_SMS_WAIT)
        coll_sms_req = MFW_SMS_FREE;

    if (int_req || int_init_req)
    {	TRACE_EVENT_P2("memory.used:%d,memory.total:%d",mem1->used, mem1->total);

		if (int_req ==FIRST_READ_OF_MEMORY_USE)
        	sms_signal(E_SMS_READY, 0);
        int_req = 0;
        int_init_req = 0;
	/*a0393213 OMAPS00109953*/		
	if(memory.used==memory.total)
            sms_signal(E_SMS_MEM_FULL, 0);

#ifdef SIM_TOOLKIT
        if (sms_update)
        {
            sms_update = FALSE;
            sms_sat_update_cnf();
        }
#endif
        return;
    }
    else
        sms_signal(E_SMS_MEM, &sms_mem);

  sms_check_mt_update();
}


/*
+---------------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS                  |
| STATE   : code                         ROUTINE : sms_message_count        |
+---------------------------------------------------------------------------+

  PURPOSE : Request number of SMS message

*/

SHORT sms_message_count (T_MFW_SMS_STAT stat)
{
    SHORT count;
    int i;
    TRACE_FUNCTION("sms_message_count");

    count = 0;
    if ((stat == MFW_SMS_UNREAD)
        OR (stat == MFW_SMS_READ)
        OR (stat == MFW_SMS_STOR_UNSENT)
        OR (stat == MFW_SMS_STOR_SENT))
    {
#ifdef FF_MMI_SMS_DYNAMIC
        for (i=0; i<g_max_messages; i++)
#else
        for (i=0; i<MAX_MESSAGES; i++)
#endif
        {
// PATCH JPS 20.09 - Don't look at SMS with index -1 because it means it is deleted
			if (msg_info[i].index != -1)
			{

				if (msg_info[i].stat == stat)
					count++;
			}
// PATCH JPS 20.09 END - Don't look at SMS with index -1 because it means it is deleted
        }
        return count;
    }

    if (stat == MFW_SMS_ALL)
    {
#ifdef FF_MMI_SMS_DYNAMIC
       for (i=0; i<g_max_messages; i++)
#else
        for (i=0; i<MAX_MESSAGES; i++)
#endif
        {
            if (msg_info[i].index != -1)
                count++;
        }
        return count;
    }

    if (stat == MFW_SMS_MO)
    {
#ifdef FF_MMI_SMS_DYNAMIC
        for (i=0; i<g_max_messages; i++)
#else
        for (i=0; i<MAX_MESSAGES; i++)
#endif
        {
// PATCH JPS 20.09 - Don't look at SMS with index -1 because it means it is deleted
			if (msg_info[i].index != -1)
			{
				if ((msg_info[i].stat == MFW_SMS_STOR_UNSENT)
				   OR (msg_info[i].stat == MFW_SMS_STOR_SENT))
					count++;
			}
// PATCH JPS 20.09 END - Don't look at SMS with index -1 because it means it is deleted
		}
        return count;
    }
    if (stat == MFW_SMS_MT)
    {
#ifdef FF_MMI_SMS_DYNAMIC
        for (i=0; i<g_max_messages; i++)
#else
        for (i=0; i<MAX_MESSAGES; i++)
#endif
        {
// PATCH JPS 20.09 - Don't look at SMS with index -1 because it means it is deleted
			if (msg_info[i].index != -1)
			{
				if ((msg_info[i].stat == MFW_SMS_UNREAD)
					OR (msg_info[i].stat == MFW_SMS_READ))
					count++;
			}
// PATCH JPS 20.09 END - Don't look at SMS with index -1 because it means it is deleted
		}
        return count;
    }
    if (stat == MFW_SMS_VOICE)
    {
#ifdef FF_MMI_SMS_DYNAMIC
        for (i=0; i<g_max_messages; i++)
#else
        for (i=0; i<MAX_MESSAGES; i++)
#endif
        {
// PATCH JPS 20.09 - Don't look at SMS with index -1 because it means it is deleted
			if (msg_info[i].index != -1)
			{
				if (msg_info[i].msg_type == stat)
					count++;
			}
// PATCH JPS 20.09 END - Don't look at SMS with index -1 because it means it is deleted
        }
        return count;
    }
    return 0;
}


/*
+---------------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS                  |
| STATE   : code                         ROUTINE : sms_message_list         |
+---------------------------------------------------------------------------+

  PURPOSE : Request a stored message list

  Note: msg_type is not used in this implementation
*/

SHORT sms_message_list(T_MFW_SMS_STAT stat,
                       T_MFW_SMS_MSG * msg_list,
                       int max_entries)
{
    SHORT count;
    int i;

    TRACE_FUNCTION("sms_message_list()");

    count = 0;
    switch (stat)
    {
        case MFW_SMS_ALL:
#ifdef FF_MMI_SMS_DYNAMIC
        for (i=0; i<g_max_messages; i++)
#else
            for (i=0; i<MAX_MESSAGES; i++)
#endif
            {
                if (count >= max_entries)
                    return count;

                if (msg_info[i].index != -1)
                {
					sms_copy_msg_list(i, &msg_list[count]);
                    count++;
                }
            }
            return count;

        case MFW_SMS_MO:
#ifdef FF_MMI_SMS_DYNAMIC
        for (i=0; i<g_max_messages; i++)

#else
            for (i=0; i<MAX_MESSAGES; i++)
#endif
            {
                if (count >= max_entries)
                    return count;

                if ((msg_info[i].index != -1)
                    AND ((msg_info[i].stat == MFW_SMS_STOR_UNSENT)
                         OR (msg_info[i].stat == MFW_SMS_STOR_SENT)))
                {
                    sms_copy_msg_list(i, &msg_list[count]);
                    count++;
                }
            }
            return count;

        case MFW_SMS_STOR_UNSENT:
#ifdef FF_MMI_SMS_DYNAMIC
        for (i=0; i<g_max_messages; i++)
#else
            for (i=0; i<MAX_MESSAGES; i++)
#endif
            {
                if (count >= max_entries)
                    return count;

                if ((msg_info[i].index != -1)
                    AND (msg_info[i].stat == MFW_SMS_STOR_UNSENT))
                {
                    sms_copy_msg_list(i, &msg_list[count]);
                    count++;
                }
            }
            return count;

        case MFW_SMS_STOR_SENT:
#ifdef FF_MMI_SMS_DYNAMIC
        for (i=0; i<g_max_messages; i++)
#else
            for (i=0; i<MAX_MESSAGES; i++)
#endif
            {
                if (count >= max_entries)
                    return count;

                if ((msg_info[i].index != -1)
                    AND (msg_info[i].stat == MFW_SMS_STOR_SENT))
                {
                    sms_copy_msg_list(i, &msg_list[count]);
                    count++;
                }
            }
            return count;

        case MFW_SMS_MT:
#ifdef FF_MMI_SMS_DYNAMIC
        for (i=0; i<g_max_messages; i++)
#else
            for (i=0; i<MAX_MESSAGES; i++)
#endif
            {
                if (count >= max_entries)
                    return count;

                if ((msg_info[i].index != -1)
                    AND ((msg_info[i].stat == MFW_SMS_UNREAD)
                         OR (msg_info[i].stat == MFW_SMS_READ)))
                {
                    sms_copy_msg_list(i, &msg_list[count]);
                    count++;
                }
            }
            return count;

        case MFW_SMS_UNREAD:
#ifdef FF_MMI_SMS_DYNAMIC
        for (i=0; i<g_max_messages; i++)
#else
            for (i=0; i<MAX_MESSAGES; i++)
#endif
            {
                if (count >= max_entries)
                    return count;

                if ((msg_info[i].index != -1)
                    AND (msg_info[i].stat == MFW_SMS_UNREAD))
                {
                    sms_copy_msg_list(i, &msg_list[count]);
                    count++;
                }
            }
            return count;

        case MFW_SMS_READ:
#ifdef FF_MMI_SMS_DYNAMIC
        for (i=0; i<g_max_messages; i++)
#else
            for (i=0; i<MAX_MESSAGES; i++)
#endif
            {
                if (count >= max_entries)
                    return count;

                if ((msg_info[i].index != -1)
                    AND (msg_info[i].stat == MFW_SMS_READ))
                {
                    sms_copy_msg_list(i, &msg_list[count]);
                    count++;
                }
            }
            return count;

        case MFW_SMS_VOICE:
#ifdef FF_MMI_SMS_DYNAMIC
        for (i=0; i<g_max_messages; i++)
#else
            for (i=0; i<MAX_MESSAGES; i++)
#endif
            {
                if (count >= max_entries)
                    return count;

                if ((msg_info[i].index != -1)
                    AND (msg_info[i].msg_type == MFW_SMS_VOICE))
                {
                    sms_copy_msg_list(i, &msg_list[count]);
                    count++;
                }
            }
            return count;

        default:
            return count;
    }
}


/*
+---------------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS                  |
| STATE   : code                         ROUTINE : sms_copy_msg_list        |
+---------------------------------------------------------------------------+

  PURPOSE : copy a stored message list

*/

void sms_copy_msg_list(int num, T_MFW_SMS_MSG *msg_list)
{
    TRACE_FUNCTION("sms_copy_msg_list()");

	  msg_list->index = msg_info[num].index;
	  msg_list->mem=msg_info[num].mem; /*a0393213 OMAPS00109953*/
    /*
     * forward message reference to MMI
     */
    msg_list->msg_ref = msg_info[num].msg_ref;
    msg_list->stat = msg_info[num].stat;
    msg_list->msg_type = msg_info[num].msg_type;
    strcpy(msg_list->addr.number, msg_info[num].addr.number);
    msg_list->addr.ton = msg_info[num].addr.ton;
    msg_list->addr.npi = msg_info[num].addr.npi;
//	 March 31, 2005    REF: CRR MMI-SPR-24511   x0012852
//     first_index, concat_status are also added to use it  later on.
    msg_list->concat_status =  msg_info[num].concat_status;
    msg_list->first_index =  msg_info[num].first_index; 
    /*a0393213 OMAPS00109953*/
    msg_list->first_mem=msg_info[num].first_mem;
#ifdef NO_ASCIIZ
    memcpy(msg_list->addr.tag,
           msg_info[num].addr.tag,
           msg_info[num].addr.len);
    msg_list->addr.len = msg_info[num].addr.len;
#else
    strcpy(msg_list->addr.tag, msg_info[num].addr.tag);
#endif
	memcpy(&msg_list->rctp, &msg_info[num].rctp, sizeof(T_MFW_SMS_SCTP));
}

/*
+---------------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS                  |
| STATE   : code                         ROUTINE : sms_ok_csas              |
+---------------------------------------------------------------------------+

  PURPOSE : Setting of SMS parameter/cell broadcast parameter
            is successful.

*/

void sms_ok_csas(void)
{
  TRACE_FUNCTION("sms_ok_csas() ");
  sms_signal(E_SMS_OK, 0);

  sms_check_mt_update();
}

/*
+---------------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS                  |
| STATE   : code                         ROUTINE : sms_error_csas           |
+---------------------------------------------------------------------------+

  PURPOSE : Setting of SMS parameter/cell broadcast parameter
            is failed.

*/

void sms_error_csas(void)
{
  sms_signal(E_SMS_ERR, 0);
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)  MODULE  : MFW_SMS                  |
| STATE   : code                  ROUTINE : sms_codeStatus           |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to convert the status
            used by MFW to the status used by ACI.
*/

T_ACI_SMS_STAT sms_codeStatus(T_MFW_SMS_STAT stat)
{
    switch (stat)
    {
        case MFW_SMS_UNREAD:        return SMS_STAT_RecUnread;
        case MFW_SMS_READ:          return SMS_STAT_RecRead;
        case MFW_SMS_STOR_UNSENT:   return SMS_STAT_StoUnsent;
        case MFW_SMS_STOR_SENT:     return SMS_STAT_StoSent;
        case MFW_SMS_ALL:           return SMS_STAT_All;
        default:                    return SMS_STAT_NotPresent;
    }
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)  MODULE  : MFW_SMS                  |
| STATE   : code                  ROUTINE : sms_decodeStatus         |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to convert the status
            used by ACI to the status used by MFW.
*/

T_MFW_SMS_STAT sms_decodeStatus(T_ACI_SMS_STAT stat)
{
    switch (stat)
    {	case SMS_STAT_Invalid:		return MFW_SMS_INVALID;/*SPR 2260*/
        case SMS_STAT_RecUnread:        return MFW_SMS_UNREAD;
        case SMS_STAT_RecRead:          return MFW_SMS_READ;
        case SMS_STAT_StoUnsent:        return MFW_SMS_STOR_UNSENT;
        case SMS_STAT_StoSent:          return MFW_SMS_STOR_SENT;
        case SMS_STAT_All:              return MFW_SMS_ALL;
        default:               
      	    return MFW_SMS_UNKNOWN;
    }
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : sms_get_cbm_id     |
+--------------------------------------------------------------------+

  PURPOSE : get cell broadcast message identifier
            cbmid: array for cell broadcast message identifier
            len:    size of array

*/

void sms_get_cbm_id (USHORT *cbmid, UBYTE len)
{
    UBYTE max;
    int   i;
    int   p;

    if (len > 6) /* maximal 5 cell broadcast message identifiers + '\0' */
        max = 6;
    else
        max = len;

    p = 0;
    // from 0-4
    for (i=0; i<(max-1); i++)
    {
        if (cb_mid[p] == 0xFF AND cb_mid[p+1] == 0xFF)
            break;
        cbmid[i] = ( ( SHORT )cb_mid[p] << 8 ) + cb_mid[p+1];
        p += 2;
    }
    cbmid[i] = '\0';
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : sms_save_cbm_id    |
+--------------------------------------------------------------------+

  PURPOSE : save cell broadcast message identifier

*/

void sms_save_cbm_id (USHORT *cbmid, UBYTE len)
{
    int   i;
    UBYTE p;
    UBYTE max;

    memset(cb_mid, 0xFF, sizeof(cb_mid));

    if (len > 5)
        max = 5;
    else
        max = len;

    p = 0;
    for (i=0; i<max; i++)
    {
        if (!cbmid[i])
            break;

        cb_mid[p++] = (UBYTE)(cbmid[i] >> 8);
        cb_mid[p++] = (UBYTE)cbmid[i];
    }

    memset(data, 0xFF, sizeof(data));

    memcpy(data, cb_mid, 10);

    sim_write_sim(SIM_CBMI, data, 10);
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : sms_check_mt_update|
+--------------------------------------------------------------------+

  PURPOSE : save cell broadcast message identifier

*/

void sms_check_mt_update()
{
  TRACE_EVENT("sms_check_mt_update() ");

if (sms_id.index > 0)
  {
	sms_send_new_msg_ind(NULL,sms_id.index, sms_id.mem, sms_id.received_sms_type, sms_id.concat_status);

    sms_id.index = 0;
  }
  if (sms_id_wait.index > 0)
  {
  sms_send_new_msg_ind(NULL,sms_id_wait.index, sms_id_wait.mem, sms_id_wait.received_sms_type, sms_id_wait.concat_status);

    sms_id_wait.index = 0;

  }


}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SIM            |
| STATE   : code                        ROUTINE : sms_check_voice_mail|
+--------------------------------------------------------------------+

  PURPOSE : Check voice mail.

*/

T_MFW sms_check_voice_mail(UBYTE pid, UBYTE dcs, char *addr,
                           T_ACI_TOA *toa, T_ACI_UDH_DATA *udh)
{	T_MFW status =MFW_SMS_UNKNOWN;//MC
  //x0pleela 24 Mar, 2006  ER OMAPS00070657
  //Flag for Voice mail waiting indication icon to display or not
#ifdef FF_CPHS
  BOOL udh_flag=FALSE;
#endif
    TRACE_FUNCTION("sms_check_voice_mail()");

  if (dcs == 0xC0      /* check data coding scheme */
      OR dcs == 0xC8
      OR dcs == 0xD0
      OR dcs == 0xD8
      OR dcs == 0xE0
      OR dcs == 0xE8)
  	{
    	
    	status = MFW_SMS_VOICE;
  	}

  else if (udh->len > 0 AND udh->data[0] == 0x01
           AND (udh->data[2] == 0x80 OR udh->data[2] == 0x00)) /* check user data header */ 
  { 
           status = MFW_SMS_VOICE; 
	  //x0pleela 24 Mar, 2006  ER OMAPS00070657
	  /*If message count is zero, so delete icon */ 
#ifdef FF_CPHS	  
	    if( udh->data[3] == 0x00 )   
	    { 
	      TRACE_EVENT("message count is zero" ); 
	      udh_flag = TRUE; 
           }
#endif
   }

// Feb 16, 2005 REF: CRR MMI-SPR-27900 xnkulkar 
// Voice mail identification is already done using dcs. Identification should not be done using pid. Check commented
//  else if (pid == 0x5F OR pid ==0x42 /*On power-up check pid to determine SMS VM */)                              /* check message identifier */
/*  	{	status = MFW_SMS_VOICE;
    	
  	} */
  else if (toa->ton == TON_Alphanumeric
  		 // 0x30 special case for one-2-one when voice mail has been deleted.
           AND (addr[0] == 0x10 OR addr[0] == 0x11 OR addr[0]==0x31 OR addr[0]==0x30)) /* check CPHS */
  	{
  		// Fix for one-2-one VM icon control. 26/6/01 MZ
  		if(addr[0]==0x30)
  		{
  			Delete_1_2_1_vm_icon = TRUE;
	  		Display_1_2_1_vm_icon = FALSE;
  		}
  		else if(addr[0]==0x31 && Delete_1_2_1_vm_icon == FALSE)
  			Display_1_2_1_vm_icon = TRUE;

  		status = MFW_SMS_VOICE;
    	
  	}
	TRACE_EVENT_P2("Addr[0]:%x, Addr[1]:%x", addr[0], addr[1]);
	TRACE_EVENT_P2("Addr[2]:%x, Addr[3]:%x", addr[2], addr[3]);
  /*MC1364, improved this to check if voicemail indicator to be set or cleared*/

// Feb 16, 2005 REF: CRR MMI-SPR-27900 xnkulkar 
// Instead of identifying voice mail ON (indication)/OFF(deletion) using addr[0], we use dcs.
// (addr[0]&0x01) condition replaced with (dcs & 0x08) 

	//x0pleela 24 Mar, 2006  ER OMAPS00070657
#ifdef FF_CPHS
       if ((status == MFW_SMS_VOICE) && (dcs & 0x08)&& (!udh_flag)) //if a voicemail indication
#else
	 if ((status == MFW_SMS_VOICE) && (dcs & 0x08))
#endif
  	{	
  		Delete_1_2_1_vm_icon = FALSE;/*SPR2260*/
	  	Display_1_2_1_vm_icon = TRUE;

//x0pleela 09 June, 2006 DR: OMAPS00079692
//following code is commented as MMI need not set the VMWI flag 
#if 0 

  		/*MC, if MSB set, bit 0 of second char in 7-bit encoding)*/
 		if (addr[1] & 0x08) /*SPR 2260 experimentally, bit set by orange*/
  			cphs_set_mailbox_status(0,MFW_CFLAG_SET , 0, 0);/*set line2*/
  		else
  			cphs_set_mailbox_status(MFW_CFLAG_SET, 0, 0, 0);/*set line1*/
#endif
  	}
	//x0pleela 24 Mar, 2006  ER OMAPS00070657
#ifdef FF_CPHS
	if ( ((status == MFW_SMS_VOICE) && !(dcs & 0x08)) OR(udh_flag)) //if a voicemail deletion
#else
	if ((status == MFW_SMS_VOICE) && !(dcs & 0x08)) 
#endif
  	{	
		Delete_1_2_1_vm_icon = TRUE;/*SPR2260*/
	  	Display_1_2_1_vm_icon = FALSE;

//x0pleela 09 June, 2006 DR: OMAPS00079692
//following code is commented as MMI need not set the VMWI flag 
#if 0 		
  		/*MC, if MSB set, bit 0 of second char in 7-bit encoding)*/
 	 	if (addr[1] & 0x08) /*SPR 2260 experimentally, bit set by orange*/
  			cphs_set_mailbox_status(0,MFW_CFLAG_NOTSet , 0, 0);/*clear line2*/
  		else
  			cphs_set_mailbox_status(MFW_CFLAG_NOTSet, 0, 0, 0);/*clear line1*/
#endif
  	}
// Feb 16, 2005 REF: CRR MMI-SPR-27900 xnkulkar 
// If voicemail message is from group "1100", update status as MFW_SMS_VOICEMSG_DELETE
	if ((status == MFW_SMS_VOICE) && ((dcs & 0xF0) == 0xC0)) 
	{
		status = MFW_SMS_VOICEMSG_DELETE;
		invalidVm = TRUE; // set this flag to TRUE to indicate that this voice mail sms is to be deleted
	}
  	
    return status;
}

#ifdef SIM_TOOLKIT
/*
+---------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)   MODULE  : MFW_SMS                  |
| STATE   : code                   ROUTINE : sms_sat_file_update      |
+---------------------------------------------------------------------+

  PURPOSE : SIM file change indication

*/

void sms_sat_file_update(USHORT dataId)
{
  TRACE_FUNCTION("sms_sat_file_update()");

  sms_update = TRUE;
  switch (dataId)
  {
    case SIM_SMS:
      sms_read_msg_info();
      break;
    default:
      break;
  }
}
#endif

#ifdef SIM_TOOLKIT
/*
+---------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)   MODULE  : MFW_SMS                  |
| STATE   : code                   ROUTINE : sms_sat_update_cnf       |
+---------------------------------------------------------------------+

  PURPOSE : SIM file change indication

*/

void sms_sat_update_cnf (void)
{
    TRACE_FUNCTION("sms_sat_update_cnf()");
    satUpdateFiles ( TRUE, SIM_SMS );
}
#endif

/* PATCH JPS 29.09: like sms_message_list but returns only index and status */

/*
+---------------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS                  |
| STATE   : code                         ROUTINE : sms_index_list	        |
+---------------------------------------------------------------------------+

  PURPOSE : Request a stored message index and status
*/

SHORT sms_index_list(T_MFW_SMS_STAT stat,
                       T_MFW_SMS_IDX * msg_list,
                       int max_entries)
{
    SHORT count;
    int i;

    TRACE_FUNCTION("sms_index_list()");

    count = 0;
    switch (stat)
    {
        case MFW_SMS_ALL:
#ifdef FF_MMI_SMS_DYNAMIC
        for (i=0; i<g_max_messages; i++)
#else
            for (i=0; i<MAX_MESSAGES; i++)
#endif
            {
                if (count >= max_entries)
                    return count;

                if (msg_info[i].index != -1)
                {
					sms_copy_idx_list(i, &msg_list[count]);
                    count++;
                }
            }
            return count;

        case MFW_SMS_MO:
#ifdef FF_MMI_SMS_DYNAMIC
        for (i=0; i<g_max_messages; i++)
#else
            for (i=0; i<MAX_MESSAGES; i++)
#endif
            {
                if (count >= max_entries)
                    return count;

                if ((msg_info[i].index != -1)
                    AND ((msg_info[i].stat == MFW_SMS_STOR_UNSENT)
                         OR (msg_info[i].stat == MFW_SMS_STOR_SENT)))
                {
                    sms_copy_idx_list(i, &msg_list[count]);
                    count++;
                }
            }
            return count;

        case MFW_SMS_STOR_UNSENT:
#ifdef FF_MMI_SMS_DYNAMIC
        for (i=0; i<g_max_messages; i++)
#else
            for (i=0; i<MAX_MESSAGES; i++)
#endif
            {
                if (count >= max_entries)
                    return count;

                if ((msg_info[i].index != -1)
                    AND (msg_info[i].stat == MFW_SMS_STOR_UNSENT))
                {
                    sms_copy_idx_list(i, &msg_list[count]);
                    count++;
                }
            }
            return count;

        case MFW_SMS_STOR_SENT:
#ifdef FF_MMI_SMS_DYNAMIC
        for (i=0; i<g_max_messages; i++)
#else
            for (i=0; i<MAX_MESSAGES; i++)
#endif
            {
                if (count >= max_entries)
                    return count;

                if ((msg_info[i].index != -1)
                    AND (msg_info[i].stat == MFW_SMS_STOR_SENT))
                {
                    sms_copy_idx_list(i, &msg_list[count]);
                    count++;
                }
            }
            return count;

        case MFW_SMS_MT:
#ifdef FF_MMI_SMS_DYNAMIC
        for (i=0; i<g_max_messages; i++)
#else
            for (i=0; i<MAX_MESSAGES; i++)
#endif
            {
                if (count >= max_entries)
                    return count;

                if ((msg_info[i].index != -1)
                    AND ((msg_info[i].stat == MFW_SMS_UNREAD)
                         OR (msg_info[i].stat == MFW_SMS_READ)))
                {
                    sms_copy_idx_list(i, &msg_list[count]);
                    count++;
                }
            }
            return count;

        case MFW_SMS_UNREAD:
#ifdef FF_MMI_SMS_DYNAMIC
        for (i=0; i<g_max_messages; i++)
#else
            for (i=0; i<MAX_MESSAGES; i++)
#endif
            {
                if (count >= max_entries)
                    return count;

                if ((msg_info[i].index != -1)
                    AND (msg_info[i].stat == MFW_SMS_UNREAD))
                {
                    sms_copy_idx_list(i, &msg_list[count]);
                    count++;
                }
            }
            return count;

        case MFW_SMS_READ:
#ifdef FF_MMI_SMS_DYNAMIC
        for (i=0; i<g_max_messages; i++)
#else
            for (i=0; i<MAX_MESSAGES; i++)
#endif
            {
                if (count >= max_entries)
                    return count;

                if ((msg_info[i].index != -1)
                    AND (msg_info[i].stat == MFW_SMS_READ))
                {
                    sms_copy_idx_list(i, &msg_list[count]);
                    count++;
                }
            }
            return count;

        case MFW_SMS_VOICE:
#ifdef FF_MMI_SMS_DYNAMIC
        for (i=0; i<g_max_messages; i++)
#else
            for (i=0; i<MAX_MESSAGES; i++)
#endif
            {
                if (count >= max_entries)
                    return count;

                if ((msg_info[i].index != -1)
                    AND (msg_info[i].msg_type == MFW_SMS_VOICE))
                {
                    sms_copy_idx_list(i, &msg_list[count]);
                    count++;
                }
            }
            return count;

        default:
            return count;
    }
}


/*
+---------------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS                  |
| STATE   : code                         ROUTINE : sms_copy_idx_list        |
+---------------------------------------------------------------------------+

  PURPOSE : copy the index and status of a short message

*/

void sms_copy_idx_list(int num, T_MFW_SMS_IDX *msg_list)
{
    TRACE_FUNCTION("sms_copy_idx_list()");

	  msg_list->index = msg_info[num].index;
    /*
     * forward message reference to MMI
     */
    msg_list->stat = msg_info[num].stat;
    msg_list->msg_type = msg_info[num].msg_type;
	/*a0393213 OMAPS00109953*/
	msg_list->memory=msg_info[num].mem;
	
}
/*a0393213 OMAPS00109953 - memory type added as a parameter*/
void sms_copy_idx(UBYTE index, T_MFW_SMS_IDX *msg_list, T_MFW_SMS_MEM sms_memory)
{int i;
    TRACE_FUNCTION("sms_copy_idx()");

#ifdef FF_MMI_SMS_DYNAMIC
        for (i=0; i<g_max_messages; i++)
#else
	for (i=0;i<MAX_MESSAGES; i++)
#endif
	{
          /*a0393213 OMAPS00109953 - memory is also taken into consideration*/
	    if (index==msg_info[i].index && sms_memory==msg_info[i].mem) 
    	/*
     	* forward message reference to MMI
     	*/
    	{	
		msg_list->concat_status = msg_info[i].concat_status;/*SPR 2260*/
	       msg_list->stat = msg_info[i].stat;
    		msg_list->msg_type = msg_info[i].msg_type;
    		break;
    	}
	}
}

//GW-SPR#780 - Removed procedure - status is already updated by 'sms_change_read_status'
/* PATCH JPS 29.09 END */
/*
+---------------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS                |
| STATE   : code                         ROUTINE : sms_ok_CMGL             |
+---------------------------------------------------------------------------+

  PURPOSE : End of the rAT_PlusCMGL()

*/
void sms_ok_CMGL (void)
{
        TRACE_FUNCTION("sms_ok_CMGL: rAT_PlusCMGL cmpl");
	  /*a0393213 OMAPS00109953*/		
	   if(index_retrieve_state==MFW_SMS_SIM_INDEX_RETRIEVING)
	   	{
	   	/*all the indexes of SMes residing in SIM has been got - now get indexes from ME*/
		    if(sms_set_pref_memory(MFW_MEM_ME)!=MFW_SMS_OK)
    			{
		    	TRACE_ERROR("sms_ok_CMGL():Not able to set pref memory");			
		    	}
			else
			{
	        	    f_update=MFW_SMS_NEW;
			    if (sAT_PlusCMGL (CMD_SRC_LCL, SMS_STAT_All,
				  #ifdef FF_2TO1_PS
			                      0, (T_ACI_SMS_READ)MNSMS_READ_PREVIEW) NEQ AT_EXCT)  /* typecast to remove warning Aug - 11 */
				  #else
			                      0, SMS_READ_Preview) NEQ AT_EXCT)/*a0393213 warnings removal-READ_PREVIEW changed to SMS_READ_Preview*/
				  #endif
			    {
			        TRACE_EVENT("sAT_PlusCMGL error");		        
			    }			  
			    else
			    	{		    	
			    	index_retrieve_state=MFW_SMS_FFS_INDEX_RETRIEVING;
				return;
			    	}
	   		}
	   	}
	   else if(index_retrieve_state==MFW_SMS_FFS_INDEX_RETRIEVING)
	   	{
	   	/*all the indexes of SMes residing in FFS has been got - so all the indexes has been got*/
	   		index_retrieve_state=MFW_SMS_INDEX_RETRIEVED;
			/*setting the pref memory back to SIM*/
		      if(sms_set_pref_memory(MFW_MEM_SIM)!=MFW_SMS_OK)
    			{
		    	TRACE_ERROR("sms_ok_CMGL():Not able to set pref memory");			
		    	}
	   	}
	   else
	   	{
	   		TRACE_ERROR("sms_ok_CMGL() : Improper index retrieving state");
	   	}
			  /* request default memory */
          f_update = MFW_SMS_FREE;
          int_req = FIRST_READ_OF_MEMORY_USE;/*SPR2260*/

          qAT_PlusCPMS(CMD_SRC_LCL);
	    
}



//Jun 14, 2005 REF: MMI-FIX-30439 x0018858
//begin 30439
/*
+---------------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)         MODULE  : MFW_SMS                |
| STATE   : code                         ROUTINE : sms_homezoneCBToggle    |
+---------------------------------------------------------------------------+

  PURPOSE : 

*/
#ifndef NEPTUNE_BOARD

//Dec 14, 2005 REF: MMI-FIX-OMAPS00058684 x0035544
#ifdef FF_HOMEZONE
void sms_homezoneCBToggle(U8 Activate_CB)
{
	  T_ACI_RETURN    ret;
	  T_ACI_CBHZ_MOD  mode;
	  T_ACI_CS        dcs;
	  UBYTE           timeout;
	  int def_timeout = 60;

       TRACE_FUNCTION("sms_homezoneCBToggle()");

	ret = qAT_PercentCBHZ(CMD_SRC_LCL , &mode, &dcs, &timeout);

	if (ret EQ AT_FAIL)
	{
	//Aug 16, 2005 REF: MMI-FIX-30439 x0018858
	//Incase the querying fails, we still wil have to activate/ de-activate with default 
	//values for dcs and timeout
		if(1 == Activate_CB )
		{
			TRACE_FUNCTION("ACTIVATING HOMEZONE");
			/*a0393213 warnings removal-CS_GsmDef used instead of 0*/
			ret = sAT_PercentCBHZ(CMD_SRC_LCL , CBHZ_MOD_Active,CS_GsmDef,def_timeout);
		}
		else
		{
			TRACE_FUNCTION("DE-ACTIVATING HOMEZONE");
			/*a0393213 warnings removal-CS_GsmDef used instead of 0*/
			ret = sAT_PercentCBHZ(CMD_SRC_LCL , CBHZ_MOD_NotActive,CS_GsmDef,def_timeout);	
		}
		return;
	}


	if(1 == Activate_CB )
	{
		TRACE_FUNCTION("ACTIVATING HOMEZONE");
		ret = sAT_PercentCBHZ(CMD_SRC_LCL , CBHZ_MOD_Active,dcs,timeout);
	}
	else
	{
		TRACE_FUNCTION("DE-ACTIVATING HOMEZONE");
		ret = sAT_PercentCBHZ(CMD_SRC_LCL , CBHZ_MOD_NotActive,dcs,timeout);	
	}
	return;

}
//end 30439
#endif
#endif

////Mar 22 2007 ER:OMAPS00113882 x0061088 (Prachi)
/*******************************************************************************

 $Function:     is_alphanumeric_addr

 $Description:  check whether the address entered by user is alphanumeric or numeric.
 				
 $Returns:    None

 $Arguments:  win

*******************************************************************************/

int is_alphanumeric_addr(char *dest_addr)
{
  int addr_length,index=0;
  addr_length= strlen(dest_addr);
  while( index < addr_length )
  	{
  	   if ( ((*dest_addr) >= '0') && ( (*dest_addr) <= '9'))
  	   	{
	   	   dest_addr++;
           index++; /* address is numeric */
  	   	}   
       else
          return TRUE;  /*alphanumeric address*/
        
  	}
  return FALSE;
}


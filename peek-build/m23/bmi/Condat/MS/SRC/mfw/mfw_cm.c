/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	$Workfile:: mfw_cm.c	    $|
| $Author:: Ak				$Revision::  1		    $|
| CREATED: 3.11.98		       $Modtime:: 10.04.00 14:58    $|
| STATE  : code 						     |
+--------------------------------------------------------------------+

   MODULE  : MFW_CM

   PURPOSE : This modul contains call management functions.

   HISTORY:
     Dec 20, 2006 DR:OMAPS00108192 a0393213(R.Prabakar)
     Description : Continous DTMF tones for simultanous keypress
     Solution     : Bug was due to improper nesting of START-STOP sequence. Now they are avoided.
     
     Dec 13, 2006 DR:OMAPS00107103 a0393213(R.Prabakar)
     Description : No DTMF tones for simultanous keypress
     Solution     : Once there was some problem in sending DTMF tones, the problem persisted till the next reset.
                       This was because the queue was not getting flushed once the call was ended. 
                       Now the queue is flushed once the call is ended.
   
    Nov 30, 2006 DR:OMAPS00105758 a0393213(R.Prabakar)
    Description :No DTMF tones after collision of DTMF tones
       
    Nov 03, 2006  DR: OMAPS00101158 x0pleela 
    Description : Board crashes after sending atd command through HyperTerminal
    Solution    : Avoid peforming phonebook search operation if the source id is not LOCAL
    			- Defined new static variable uOthersrc to set the value to TRUE or FALSE 
    				based on the source Id
    			- Defined two new function mfw_set_OtherSrc_Value() to set the value of 
    				uOtherSrc to either TRUE or FALSE and mfw_get_OtherSrc_Value 
    				to get the value of uOtherSrc

    Oct 10, 2006  DR: OMAPS00093720   x0039928 
    Description : If dial a international number and fail, when auto redail enabled, it will delete '+'.
    Solution    : MFW_TON_INTERNATIONAL is set for out_call.ton if the dialling number contains '+'
    
    Sep 01, 2006 REF:OMAPS00090555  x0039928
    Description : When make a MO call,MFW can not get call number from ACI.
    Solution     : The called number is extracted from the parameter of callCmEvent() 
    for event E_CM_MO_RES and is displayed. Also stack memory optimization is done to 
    resolve MMI stack overflow problem.

    June 15, 2006 OMAPS00079492 x0043642 
    Description : FT TMO (wk20/06) TC 9.1, 9.2 Multi Party, MS fails to return to idle mode
    Solution    : assign MFW_CM_DISCONNECT_ALL to variable cmd_hold, and call cm_signal()


   May 30, 2006  DR: OMAPS00070657 x0pleela
   Description: CPHS feature on Locosto-Lite
   Solution: For ALS feature,
   			a) Added definition for new function "mfw_get_ALS_type"
   			
    Apr 25, 2006 OMAPS00075711 a0393213 (R.Prabakar)
    Description : 'End Conference' option doesn't work
    Solution    : The ACI_CHLD_MOD is changed from CHLD_MOD_RelActSpec to CHLD_MOD_RelActAndAcpt while calling sAT_PlusCHLD()
                  in cm_disconnect(). This is done since CHLD_MOD_RelActSpec with callId 0 is equivalent to sending 
                  AT+CHLD=10 which is invalid according to GSM specification 
    
    Apr 26, 2006 OMAPS00074886 a0393213 (Prabakar R)
    Description : No option in MMI to release the held call. 
    Solution    : No option was available. Added a menu option.
    
    Mar 28, 2006    REF:DR OMAPS00072407  x0039928
    Description:   Phone hangs when tried to make an outgoing call
    Solution: Reverting back the changes of issue OMAPS00048894 to stop from stack overflow.

    Feb 27, 2006    REF:DR OMAPS00067884  x0039928
    Description:   Held call released at locosto phone does not release the call at other end
    Solution: if the mode is greater than CHLD_MOD_RelDialCall then sAT_PercentCHLD() is called 
    instead of sAT_PlusCHLD()

    Jan 27, 2006    REF:DR OMAPS00048894  x0039928
    Description:   Outgoing Call: The called number is not displayed while an outgoing call is made
    Solution: The called number is extracted from the parameter of callCmEvent() for event E_CM_MO_RES and is displayed.
	
   Apr 22, 2006 OMAPS00070762 a0393213 (Prabakar R)
   Bug : Make MO call from MS1 to MS2. Make MT call from MS3 to MS1. Type at+chld=1 in ATI. 
      expected behaviour : active call should be released and the incoming call should be accepted
      observed behaviour : happens as expected. but display shows incoming call screen only.
                           active screen is not shown eventhough the connection is established between MS1 and MS3
   Fix : cmd.call_number was not set to 0, which is required in cm_result_cmd()
         to call cm_ok_connect()which updates the display. It is set now.


   Apr 21, 2006 OMAPS00070774 a0393213 (Prabakar R)
   Bug : Make MO call from MS1 to MS2. Make MT call from MS3 to MS1. Type at+chld=2 in ATI. 
      expected behaviour : active call should be put to hold and the incoming call should be accepted
      observed behaviour : happens as expected. but display shows incoming call screen only.
                           active screen is not shown eventhough the connection is established between MS1 and MS3
   Fix : AT commands from CMD_SRC_ATI_3 was not processed. As a result, MFW was not informed of the acceptance of the call 
         through ATI. In precise cmd_hold variable was not set to MFW_CM_MT_ACCEPT, which is necessary for display update


   Sep 12, 2005   REF: MMI-SMR-34085 x0021334
   Description: RE: Add +CDIP command - MMI Additions
   Solution: Dummy implementation of function "rAT_PlusCDIP()" is done to enable successful compilation.
	Nov 02, 2005    DR OMAPS00052132   xdeepadh
	Bug:Not able to access the SMS inbox and also not able to send SMS in Locosto Lite build
	Fix: The ATI Enums usage has been put under the FF_ATI flag.

   Aug 10, 2005   REF: MMI-SPR-30099 x0018858
   Description: During a held communication, if the user presses 4 and send then there is no response from network.
   Solution: Modified the return value and also added a case statement to handle the response received.

   	Apr 07 2005		REF: CRR 29989	xpradipg
	Description:	Optimisation 3: replace the static global data with dynamic
					allocation / deallocation
	Solution:		The static global variables are dynamically allocated and
					deallocated

   Apr 03, 2005 REF: CRR 29988 - xpradipg
   Description: Optimisation 2: Reduce the keyboard buffer and the dtmf buffer
   size
   Solution : The dtmf buffer is reduced to 20 from 50

	Apr 14, 2005	REF: CRR 29991   xpradipg
	Description:	Optimisation 5: Remove the static allocation and use dynamic 
					allocation/ deallocation for pb_list and black_list
	Solution:	The static definition is removed and replaced with the dynamic
					allocation

   Feb 03, 200   REF: CRR 25960   x0012850
   Description: Call Hold: While one call in Active and other in Held by swapping either of phone is not audible
   Solution: swapping action results in one of the calls getting muted. We attach user connection again.
	
   Aug 23, 2004 REF: CRR 22222  xkundadu
   Bug:  Runaway Call Waiting Tone.
		 Call Waiting tone doesnot stop after ansering waiting call.

   Fix: Handle the AT-Command source id CMD_SRC_ATI_2 also for call control 
    	callback functions.

   Jun 05, 2004	REF: CRR 18262  NISHIKANT KULKARNI 
   Description: The sample sends a STOP DTMF message without release of the key by the user
   Solution: Instead of sending DTMF commands in "VTS_MOD_Auto" mode, on key press DTMF tone is started
  		   using VTS_MOD_ManStart and on key release DTMF tone is stopped using VTS_MOD_ManStop mode.

	//  Apr 26, 2004 REF: CRR 16545 Ajith K P and Side effect of CRR 16545.
	//  Issue description: When the user enters 11SEND and if the first call is 
	//  HELD call,the held call disconnects. This should not happen.
	//  Problem was, though sAT_PlusCHLD(..) is expected to disconnect an ACTIVE
	//  call only, it was disconnecting HELD call rather than returning AT_FAIL.
	//  If sAT_PlusCHLD() Fails, Send the user entered string as USSD to network using sAT-Dn().
	//  So now if the user wants to release a HELD call, in cm_disconect() we are calling the 
	//  sAT_PlusCHLD(..) function with C


    Mar 28, 2006 REF:ER OMAPS00071798  x0043642
    Added new options in BMI to accept the value for tty audio mode,
    and the accepted value is passed as parameter in function call sAT_PercentCTTY

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
#include "mfw_utils.h"

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
#include "mfw_win.h"
#include "mfw_ffs.h"

#include "prim.h"
#ifndef PCM_2_FFS
#include "pcm.h"
#endif



/*
 * Conditional compilation flag for CCBS testing.
 * When defined, the cm_ccbs_... functions do not call ACI functions but
 * simply set the return value variable to indicate success. Also the body
 * of the rAT_PercentMMITEST function is defined for parsing AT%MMITEST
 * commands and calling ACI/MFW functions as if CCBS events had occured.
 */
// #define CCBS_TEST

/********* current define *******************************************/
//	Apr 14, 2005	REF: CRR 29991   xpradipg
//	remove the static definition
#ifndef FF_MMI_OPTIM
static	 T_MFW_CM_BLACKLIST_LIST    black_list;
#endif

//  Apr 06 2005		REF: CRR 29989	xpradipg
//	The definition of global is removed since it is locally used
#ifndef FF_MMI_OPTIM
static	 T_ACI_CAL_ENTR 	    call_tab[MAX_CALL_NR];
#endif
static	 T_MFW_PHB_LIST 	    entries;
static	 T_MFW_CM_CC_STAT	    cc_stat;	   /* current outgoing call status	    */
static	 T_MFW_CM_COMMAND	    cmd;
static	 T_MFW_CM_NOTIFY	    notify;

static	 UBYTE			    call_type;	   /* indicate an ECC call or normal call   */
static	 UBYTE			    call_stat;	   /* indicate an MO call or MT call	    */
/*a0393213 lint WR:symbol not referenced soln:the variable is put under the flag (which won't be defined) so that the flag
   can be removed if the variable is necessary*/
#ifdef MMI_LINT_WARNING_REMOVAL 
static	 SHORT			    cc_id;	   /* current call index		    */
#endif
static	 UBYTE			    redial_stat;
static	 UBYTE			    redial_count;
static	 UBYTE			    cmd_hold;

static	 T_MFW_CM_CW_INFO	    in_call;
static	 T_MFW_CM_MO_INFO	    out_call;
static	 T_MFW_CM_DISCONNECT	    dis_connect;
static	 T_MFW_CM_REDIAL	    rdl;
static	 T_MFW_CM_CPI		    cm_cpi;
/* Marcus: CCBS: 14/11/2002: Start */
static	 T_MFW_CM_CCBS_INFO	    ccbs_info;              // E_CM_CCBS data
static   T_MFW_CM_CCBS_OP       ccbs_op = CM_CCBS_NONE; // Current CCBS operation
/* Marcus: CCBS: 14/11/2002: End */

static	 T_CM_CLI		    cli_prefix;
/*CONQUEST 5998-MC 30/04/02 -initialised timers to zero (prevents corruption) */
static	 MfwHnd 		    redialTim1= NULL;	/* the 1th redialing			 */
static	 MfwHnd 		    redialTim24 = NULL;  /* the 2th to 4st redialing		  */
static	 MfwHnd 		    redialTim5 = NULL;	 /* the 5th or over redialing		  */

static int cmCommand (U32 cmd, void *h); /* handle window commands  */
static int cm_redial_timeout(MfwEvt e, MfwTim *t);
static char currAccepting = 0; /* ES!! 210999 */
static char currConnecting = 0; /* ES!! 290999 */
/*a0393213 lint WR:symbol not referenced soln:the variable is put under the flag (which won't be defined) so that the flag
   can be removed if the variable is necessary*/
#ifdef MMI_LINT_WARNING_REMOVAL 
static T_ACI_AT_CMD currCmd;
#endif
static T_MFW_CM_DTMF dtmf_sent={FALSE};/*OMAPS00108192 a0393213 - variable which keeps track of the dtmf sent to ACI (via sAT_PlusVTS)*/
static T_MFW_CM_DTMF dtmf_queued={FALSE};/*OMAPS00108192 a0393213 - variable which keeps track of the recent dtmf added to the queue*/
void cm_force_disconnect(void); // RAVI

/* June 15, 2006 OMAPS00079492 x0043642 */
void set_cmdHold(int command);

//APR 03 2005 - REF CRR: 29988  xpradipg
//The DTMF buffer is reduced from 50 to 20
#ifdef FF_MMI_REDUCED_KBD_BUFFER
#define MAX_DTMF_Q_ENTRIES 20
#else
#define MAX_DTMF_Q_ENTRIES 50
#endif
#define CPI_CAUSE_MASK 0xFF    /* cq11676 cause mask 17-02-04 MZ */
static UBYTE dtmf_q[MAX_DTMF_Q_ENTRIES];
static SHORT mfw_cm_dtmf_q_id;

// MMI-SPR 13873 xkundadu.
// Added this boolean variable to check whether a particular call is in 
// multiparty or not
BOOL bIsMultpty = FALSE;

EXTERN MfwHdr * current_mfw_elem;

LOCAL T_MFW_CM_CALL_STAT cm_cvt_call_status( T_ACI_CAL_STAT stat );

//x0pleela 03 Nov, 2006  DR: OMAPS00101158
//Variable which says the type of source Id
//FALSE if LOCAL SRC, TRUE if other sources like AT1, AT2, AT3...
static UBYTE uOtherSrc;
//function to set the value of uOtherSrc to either TRUE or FALSE
GLOBAL void mfw_set_OtherSrc_Value(UBYTE otherSrc);
//function to get the value of uOtherSrc
GLOBAL UBYTE mfw_get_OtherSrc_Value(void);
/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_init	     |
+--------------------------------------------------------------------+


   PURPOSE :   initialize for call management

*/

void cm_init(void)
{
//	Apr 14, 2005	REF: CRR 29991   xpradipg
//	local variable definition
#ifdef FF_MMI_OPTIM
	T_MFW_CM_BLACKLIST_LIST black_list;
	/*a0393213 compiler warnings removal- removed variable 'file'*/
#endif	
  TRACE_FUNCTION ("cm_init()");
  memset(&black_list, 0, sizeof(T_MFW_CM_BLACKLIST_LIST));
//	Apr 14, 2005	REF: CRR 29991   xpradipg  
//	Since the blacklist is not persistent across powercycles we reset the file
//	contents
#ifdef FF_MMI_OPTIM
	if(	flash_MMI_blackList_write((U8 *)&black_list, sizeof(T_MFW_CM_BLACKLIST_LIST),0) < 0)
		TRACE_EVENT("Error opening the black list file");
#endif  

    currAccepting = 0; /* ES!! 210999 */
    currConnecting = 0; /* ES!! 290999 */

  call_type    = MFW_CALL_FREE;
  call_stat    = MFW_CALL_FREE;
  cc_stat.type = MFW_CALL_FREE;
  #ifdef MMI_LINT_WARNING_REMOVAL /*a0393213 lint warnings removal*/
  cc_id        = 0;
  #endif

  cmd_hold = 0;
  #ifdef MMI_LINT_WARNING_REMOVAL  /*a0393213 lint warnings removal*/
  currCmd = AT_CMD_NONE;
  #endif

// get prefix from EEPROM
  cli_prefix = 0;

  /* create time event for redialing */
  redialTim1  = timCreate (0, MIN_DURATION_1, (MfwCb)cm_redial_timeout);
  redialTim24 = timCreate (0, MIN_DURATION_2_4, (MfwCb)cm_redial_timeout);
  redialTim5  = timCreate (0, MIN_DURATION_5, (MfwCb)cm_redial_timeout);

	redial_stat = (UBYTE)FFS_flashData.redial_mode;

  mfwCommand[MfwTypCm] = (MfwCb) cmCommand;

/*
** Create a static circular buffer to hold the dtmf tones
*/
  mfw_cm_dtmf_q_id = mfw_cbuf_create(	MAX_DTMF_Q_ENTRIES,
  										sizeof(UBYTE),
  				   						0,
  				   						0xFF,
  										TRUE,
  										(void *)&dtmf_q[0]);

  if (mfw_cm_dtmf_q_id < 0)
  	TRACE_EVENT_P1("ERROR : mfw_cbuf_create failed with error value %d", mfw_cm_dtmf_q_id);
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_exit	     |
+--------------------------------------------------------------------+


   PURPOSE :

*/

void cm_exit(void)
{
  TRACE_FUNCTION ("cm_exit()");

  /* delete time event for redialing */
  timDelete (redialTim5);
  timDelete (redialTim24);
  timDelete (redialTim1);
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_create	     |
+--------------------------------------------------------------------+


   PURPOSE :  create event for call management

*/

T_MFW_HND cm_create(T_MFW_HND hWin, T_MFW_EVENT event, T_MFW_CB cbfunc)
{
  T_MFW_HDR *hdr;
  T_MFW_CM  *cm_para;

  TRACE_FUNCTION ("cm_create()");

  hdr	   = (T_MFW_HDR *) mfwAlloc(sizeof (T_MFW_HDR));
  cm_para  = (T_MFW_CM *) mfwAlloc(sizeof (T_MFW_CM));

  if (!hdr OR !cm_para)
    return FALSE;

  /*
   * initialisation of the handler
   */
  cm_para->emask   = event;
  cm_para->handler = cbfunc;

  hdr->data = cm_para;				 /* store parameter in node	*/
  hdr->type = MFW_TYP_CM;		    /* store type of event handler */

  /*
   * installation of the handler
   */
  return mfwInsert((T_MFW_HDR *)hWin, hdr);
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_delete	     |
+--------------------------------------------------------------------+


   PURPOSE :   delete a event for call management

*/

T_MFW_RES cm_delete(T_MFW_HND h)
{
  TRACE_FUNCTION ("cm_delete()");

  if (!h OR !((T_MFW_HDR *)h)->data)
    return MFW_RES_ILL_HND;

  if (!mfwRemove((T_MFW_HDR *)h))
    return MFW_RES_ILL_HND;

  mfwFree((U8 *)(((T_MFW_HDR *) h)->data),sizeof(T_MFW_CM));
  mfwFree((U8 *)h,sizeof(T_MFW_HDR));

  return MFW_RES_OK;
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_signal	     |
+--------------------------------------------------------------------+

   PURPOSE : send a event signal.

*/

void cm_signal(T_MFW_EVENT event, void * para)
{
	UBYTE temp;



  temp = dspl_Enable(0);


if (mfwSignallingMethod EQ 0)
  {
  /*
   * focus is on a window
   */
  if (mfwFocus)
    /*
     * send event to sim management
     * handler if available
     */
    if (cm_sign_exec (mfwFocus, event, para))
		{
			dspl_Enable(temp);
			return;
		}

  /*
   * acutal focussed window is not available
   * or has no network management registration
   * handler, then search all nodes from the root.
   */
   if (mfwRoot)
     cm_sign_exec (mfwRoot, event, para);
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
		 if (cm_sign_exec (h, event, para))
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
	   cm_sign_exec (mfwRoot, event, para);
	}
	dspl_Enable(temp);
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_sign_exec	     |
+--------------------------------------------------------------------+


   PURPOSE : Send a signal if CM management handler.

*/

BOOL cm_sign_exec (T_MFW_HDR * cur_elem, T_MFW_EVENT event, T_MFW_CM_PARA * para)
{
  TRACE_FUNCTION ("cm_sign_exec()");

  while (cur_elem)
  {
    /*
     * event handler is available
     */
    if (cur_elem->type EQ MFW_TYP_CM)
    {
      T_MFW_CM * cm_data;
      /*
       * handler is CM management handler
       */
      cm_data = (T_MFW_CM *)cur_elem->data;
      if (cm_data->emask & event)
      {
	/*
	 * event is expected by the call back function
	 */
	cm_data->event = event;
	switch (event)
	{
	  case E_CM_COLP:
	    memcpy (&cm_data->para.mo_data, para, sizeof (T_MFW_CM_MO_INFO));
	    break;

	  case E_CM_DISCONNECT:
	    memcpy (&cm_data->para.disconnect, para, sizeof (T_MFW_CM_DISCONNECT));
	    break;
	  case E_CM_DISCONNECT_ALL:
	    memcpy (&cm_data->para.disconnect_all, para, sizeof (T_MFW_CM_DISCONNECT_ALL));
	    break;

	  case E_CM_CW:
	    memcpy (&cm_data->para.cw_info, para, sizeof (T_MFW_CM_CW_INFO));
	    break;

	  case E_CM_COMMAND:
	    memcpy (&cm_data->para.command, para, sizeof (T_MFW_CM_COMMAND));
	    break;

	  case E_CM_AOC:
	    memcpy (&cm_data->para.aoc_info, para, sizeof (T_MFW_CM_AOC_INFO));
	    break;

	  case E_CM_REDIAL:
	    memcpy (&cm_data->para.rdl, para, sizeof (T_MFW_CM_REDIAL));
	    break;

	  case E_CM_CONNECT_ACK:
	    memcpy (&cm_data->para.call_id, para, sizeof (SHORT));
	    break;

// Sep 01, 2006 REF:OMAPS00090555  x0039928
// Fix: Copy MO data
	  case E_CM_MO_RES:
	    memcpy (&cm_data->para.mo_data, para, sizeof (T_MFW_CM_MO_INFO));
	    break;

	  case E_CM_CPI:
	    memcpy (&cm_data->para.cm_cpi, para, sizeof (T_MFW_CM_CPI));
	    break;

	  case E_CM_RING_OFF:
	    break;

	  case E_CM_AOC_CNF:
	    memcpy (&cm_data->para.aoc_cnf, para, sizeof (T_MFW_CM_AOC_CNF));
	    break;

	  case E_CM_DTMF:
	    memcpy (&cm_data->para.dtmf_tone, para, sizeof (UBYTE));
	    break;

	  case E_CM_NOTIFY:
	    memcpy (&cm_data->para.notify, para, sizeof (T_MFW_CM_NOTIFY));
	    break;
	  /*MC SPR 1392*/
	  case E_CM_CALL_DEFLECT_STATUS:
	  	 memcpy (&cm_data->para.call_deflect_status, para, sizeof (BOOL));
	  break;
      /* Marcus: CCBS: 14/11/2002: Start */
	  case E_CM_CCBS:
	    TRACE_EVENT("E_CM_CCBS");
	    memcpy (&cm_data->para.ccbs, para, sizeof (T_MFW_CM_CCBS_INFO));
	    break;
      /* Marcus: CCBS: 14/11/2002: End */

	   case E_CM_CTYI:
	  	memcpy (&cm_data->para.ctyi, para, sizeof (T_MFW_CM_CTYI));
	  	break;

	}

	/*
	 * if call back defined, call it
	 */
	if (cm_data->handler)
	{
	  // PATCH LE 06.06.00
	  // store current mfw elem
	  current_mfw_elem = cur_elem;
	  // END PATCH LE 06.06.00
	  if ((*(cm_data->handler)) (cm_data->event, (void *)&cm_data->para))
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
| PROJECT : MMI-Framework (8417)	 MODULE  : MFW_CM	     |
| STATE   : code			 ROUTINE : cm_callTyp_cvt    |
+--------------------------------------------------------------------+


   PURPOSE : This function is used to convert the type of call class
	     used by MFW to the type of call class used by ACI.

*/

LOCAL T_MFW_CM_CALL_TYPE cm_callTyp_cvt(T_ACI_CLASS cls)
{
    switch ( cls )
    {
	case ( CLASS_Vce ) :	  return VOICE_CALL;
	case ( CLASS_AuxVce):	  return AuxVOICE_CALL;
	case ( CLASS_Dat ) :	  return DATA_CALL;
	case ( CLASS_Fax ) :	  return FAX_CALL;
	case ( CLASS_VceDat ) :
	case ( CLASS_VceFax ) :
	case ( CLASS_DatFax ) :
	case ( CLASS_VceDatFax ): return (T_MFW_CM_CALL_TYPE)cls;
	case ( CLASS_None )	: return NONE_TYPE;
	case (CLASS_NotPresent):	return NONE_TYPE;

	default:		  return NONE_TYPE;
    }
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_search_callId    |
+--------------------------------------------------------------------+


   PURPOSE : find call index from call table

*/

int cm_search_callId(T_ACI_CAL_STAT type, SHORT *id)
{
    int i;
//  Apr 06 2005		REF: CRR 29989	xpradipg    
//	Local definition and allocation
#ifdef FF_MMI_OPTIM
	T_ACI_CAL_ENTR 	    *call_tab;
	*id = 0;
	call_tab = (T_ACI_CAL_ENTR*)mfwAlloc(sizeof(T_ACI_CAL_ENTR)*MAX_CALL_NR);
	if(NULL == call_tab)
		return CM_ERROR;
#else
	*id = 0;
#endif	
    TRACE_FUNCTION("cm_search_callId()");
    

    if (qAT_PercentCAL(CMD_SRC_LCL, call_tab) != AT_CMPL)
	{
//   	Apr 06 2005		REF: CRR 29989	xpradipg
//		deallocation of memory allocated
#ifdef FF_MMI_OPTIM
	    if(call_tab)
	    mfwFree((U8*)call_tab, sizeof(T_ACI_CAL_ENTR)*MAX_CALL_NR);
#endif
		return CM_ERROR;
	}		

    for (i=0; i<MAX_CALL_NR; i++)
    {
	if (call_tab[i].index == -1)
	    break;

	if (call_tab[i].status == type)
	{
	    *id = call_tab[i].index;
//   	Apr 06 2005		REF: CRR 29989	xpradipg
//		deallocation of memory
#ifdef FF_MMI_OPTIM
	    if(call_tab)
	    mfwFree((U8*)call_tab, sizeof(T_ACI_CAL_ENTR)*MAX_CALL_NR);
#endif	    
	     return CM_OK;
	}

    }
//   	Apr 06 2005		REF: CRR 29989	xpradipg
//		deallocation of memory
#ifdef FF_MMI_OPTIM
	    if(call_tab)
	    mfwFree((U8*)call_tab, sizeof(T_ACI_CAL_ENTR)*MAX_CALL_NR);
#endif    
    return CM_ERROR;
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_search_callMode  |
+--------------------------------------------------------------------+


   PURPOSE : find call type from call table

*/

T_ACI_CAL_MODE cm_search_callMode(SHORT id)
{
    int i;
//  Apr 06 2005		REF: CRR 29989	xpradipg
//	allocation of memory, if the memory allocation fails then we return the 
//	error value same as when the qAT_PercentCAL() fails
#ifdef FF_MMI_OPTIM
	T_ACI_CAL_ENTR 	    *call_tab;//[MAX_CALL_NR];
	T_ACI_CAL_MODE		calMode;
	call_tab = (T_ACI_CAL_ENTR*)mfwAlloc(sizeof(T_ACI_CAL_ENTR)*MAX_CALL_NR);
	if(NULL == call_tab)
		return CAL_MODE_Unknown;
#endif	

    TRACE_FUNCTION("cm_search_callMode()");

    if (qAT_PercentCAL(CMD_SRC_LCL, call_tab) != AT_CMPL)
	{
//   	Apr 06 2005		REF: CRR 29989	xpradipg	
//		deallocate memory
#ifdef FF_MMI_OPTIM
	    if(call_tab)
	    mfwFree((U8*)call_tab, sizeof(T_ACI_CAL_ENTR)*MAX_CALL_NR);
#endif	
		return CAL_MODE_Unknown;
	}
    for (i=0; i<MAX_CALL_NR; i++)
    {
	if (call_tab[i].index == -1)
	    break;

		if (call_tab[i].index == id)
		{	
//   	Apr 06 2005		REF: CRR 29989	xpradipg
//		deallocate memory
#ifdef FF_MMI_OPTIM
			calMode = call_tab[i].calMode;
		    if(call_tab)
		        mfwFree((U8*)call_tab, sizeof(T_ACI_CAL_ENTR)*MAX_CALL_NR);
		    return calMode;
#else		
		    return call_tab[i].calMode;
#endif		    
		}	    
    }
// 	Apr 06 2005		REF: CRR 29989	xpradipg    
//	deallocate memory
#ifdef FF_MMI_OPTIM
	    if(call_tab)
	    mfwFree((U8*)call_tab, sizeof(T_ACI_CAL_ENTR)*MAX_CALL_NR);
#endif
    return CAL_MODE_Unknown;
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_search_mptyId    |


   PURPOSE : find multiparty call index from call table

*/

int cm_search_mptyId(T_ACI_CAL_MPTY type, SHORT *id)
{
    int i;
// 	Apr 06 2005		REF: CRR 29989	xpradipg    
//	allocate memory
#ifdef FF_MMI_OPTIM
	T_ACI_CAL_ENTR 	    *call_tab;//[MAX_CALL_NR];
	call_tab = (T_ACI_CAL_ENTR*)mfwAlloc(sizeof(T_ACI_CAL_ENTR)*MAX_CALL_NR);
	if( NULL == call_tab)
		return CM_ERROR;
#endif	

    TRACE_FUNCTION("cm_search_mptyId()");
    *id = 0;

    if (qAT_PercentCAL(CMD_SRC_LCL, call_tab) != AT_CMPL)
	{
// 	Apr 06 2005		REF: CRR 29989	xpradipg	
//	deallocate memory
#ifdef FF_MMI_OPTIM
	    if(call_tab)
	    mfwFree((U8*)call_tab, sizeof(T_ACI_CAL_ENTR)*MAX_CALL_NR);
#endif
		return CM_ERROR;
	}
    for (i=0; i<MAX_CALL_NR; i++)
    {
	if (call_tab[i].index == -1)
	    break;

	if (call_tab[i].mpty == type)
	{
	    *id = call_tab[i].index;
// 	Apr 06 2005		REF: CRR 29989	xpradipg	
//	deallocate memory
#ifdef FF_MMI_OPTIM
	    if(call_tab)
	    mfwFree((U8*)call_tab, sizeof(T_ACI_CAL_ENTR)*MAX_CALL_NR);
#endif	    
	    return CM_OK;
	}
    }
// 	Apr 06 2005		REF: CRR 29989	xpradipg	
//	deallocate memory
#ifdef FF_MMI_OPTIM
	    if(call_tab)
	    mfwFree((U8*)call_tab, sizeof(T_ACI_CAL_ENTR)*MAX_CALL_NR);
#endif    
    return CM_ERROR;
}


/*
+-------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		    |
| STATE  : code 			ROUTINE: cm_search_numId    |
+-------------------------------------------------------------------+
   PURPOSE : find call index from call table
*/
/*a0393213(R.Prabakar) lint warnings removal
   warning:symbol(cm_search_numId) not referenced
   soln     :this function was not at all called. so this function has been put under MMI_LINT_WARNING_REMOVAL,
               which is not defined. If this function is needed this flag can be removed*/

#ifdef MMI_LINT_WARNING_REMOVAL
LOCAL SHORT cm_search_numId(UBYTE *number)
{
	int i;
// 	Apr 06 2005		REF: CRR 29989	xpradipg	
//	allocate memory
#ifdef FF_MMI_OPTIM
	SHORT	index;
	T_ACI_CAL_ENTR 	    *call_tab;//[MAX_CALL_NR];
	call_tab = (T_ACI_CAL_ENTR*)mfwAlloc(sizeof(T_ACI_CAL_ENTR)*MAX_CALL_NR);
	if(NULL == call_tab)
		return 0;
#endif	

    TRACE_FUNCTION("cm_search_numId()");

    if (qAT_PercentCAL(CMD_SRC_LCL, call_tab) != AT_CMPL)
	{
// 	Apr 06 2005		REF: CRR 29989	xpradipg	
//	deallocate memory
#ifdef FF_MMI_OPTIM
	    if(call_tab)
	    mfwFree((U8*)call_tab, sizeof(T_ACI_CAL_ENTR)*MAX_CALL_NR);
#endif	
		return 0;
	}		

    for (i=0; i<MAX_CALL_NR; i++)
    {
		if (call_tab[i].index == -1)
		    break;

		if (!strcmp(call_tab[i].number, (char *)number))
		{
// 	Apr 06 2005		REF: CRR 29989	xpradipg	
//	deallocate memory
#ifdef FF_MMI_OPTIM
			index = call_tab[i].index;
		    if(call_tab)
		    mfwFree((U8*)call_tab, sizeof(T_ACI_CAL_ENTR)*MAX_CALL_NR);
		    return index;
#else	
		    return call_tab[i].index;
#endif		    
		}	    
    }
// 	Apr 06 2005		REF: CRR 29989	xpradipg	
//	deallocate memory
#ifdef FF_MMI_OPTIM
	    if(call_tab)
	    mfwFree((U8*)call_tab, sizeof(T_ACI_CAL_ENTR)*MAX_CALL_NR);
#endif    
    return 0;
}

#endif
/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_search_callStat  |
+--------------------------------------------------------------------+


   PURPOSE :  Search the information from call table.

*/

T_MFW cm_search_callStat(SHORT call_number,
			 T_MFW_CM_CALL_STAT *stat,
			 UBYTE *ton,
			 UBYTE *mode)
{
    int i;
// 	Apr 06 2005		REF: CRR 29989	xpradipg	
//	allocat memory
#ifdef FF_MMI_OPTIM
	T_ACI_CAL_ENTR 	    *call_tab;//[MAX_CALL_NR];
	call_tab = (T_ACI_CAL_ENTR*)mfwAlloc(sizeof(T_ACI_CAL_ENTR)*MAX_CALL_NR);
	if( NULL == call_tab)
		return CM_ERROR;
#endif	

    if (qAT_PercentCAL(CMD_SRC_LCL, call_tab) EQ AT_CMPL)
    {
	for (i=0; i<MAX_CALL_NR; i++)
	{
	    if (call_tab[i].index == -1)
	    break;

	    if (call_tab[i].index == call_number)
	    {
		*stat = cm_cvt_call_status(call_tab[i].status);
		*ton = phb_cvtTon(call_tab[i].type.ton);
		*mode = cm_cvt_call_mode(call_tab[i].calMode);
// 	Apr 06 2005		REF: CRR 29989	xpradipg	
//	deallocate memory
#ifdef FF_MMI_OPTIM
			    if(call_tab)
			    mfwFree((U8*)call_tab, sizeof(T_ACI_CAL_ENTR)*MAX_CALL_NR);
#endif

		return CM_OK;
	    }
	}
// 	Apr 06 2005		REF: CRR 29989	xpradipg	
//	this will be returned once we get out of the if statement
#ifndef FF_MMI_OPTIM
	return CM_ERROR;
#endif		
    }
// 	Apr 06 2005		REF: CRR 29989	xpradipg	    
//  deallocate memory  
#ifdef FF_MMI_OPTIM
	    if(call_tab)
	    mfwFree((U8*)call_tab, sizeof(T_ACI_CAL_ENTR)*MAX_CALL_NR);
#endif    
    return CM_ERROR;
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_check_emergency  |
+--------------------------------------------------------------------+


   PURPOSE : Check emergency call number

*/

T_MFW cm_check_emergency(UBYTE *number)
{
  //UBYTE 	    index; // RAVI
 // T_MFW_PHB_TEXT    findText;  // RAVI
 // T_MFW_PHB_ENTRY   entry;  // RAVI

  TRACE_FUNCTION ("cm_check_emergency()");
  TRACE_EVENT_P1("String being passed = %s", (char*)number);

//TISH, patch for OMAPS00125064
  if (strlen((char *)number) <= 0 OR strlen((char *)number) > 6)
  //if (strlen((char *)number) <= 0 OR strlen((char *)number) > 3)
    return FALSE;

  if(psaCC_phbSrchECC((char*)number, FALSE))
  	{
  		TRACE_EVENT("psaCC_phbSrchECC - returned - TRUE");
  		return TRUE;
	}
  else
  	{
  	  	TRACE_EVENT("psaCC_phbSrchECC - returned - FALSE");
	 	return FALSE;
  	}

}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_mo_prefix	     |
+--------------------------------------------------------------------+


   PURPOSE : set/get CLIP, CLIR, COLP and COLR tempstatus

*/

T_CM_CLI cm_mo_prefix(T_CM_CLI prefix)
{
	UBYTE bits;

  if ((bits = (prefix & 0x0c)) != 0)
    cli_prefix = (cli_prefix & 0xf3) | bits;
  if ((bits = (prefix & 0xc0)) != 0)
    cli_prefix = (cli_prefix & 0x3f) | bits;
	return cli_prefix;
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_set_cmd_status   |
+--------------------------------------------------------------------+


   PURPOSE :

*/

void cm_set_cmd_status(T_ACI_CHLD_MOD mode, CHAR* call)
{
    SHORT id;
    // Added xkundadu MMI-SPR 13873
    // Stores the status of a particular call.
    T_MFW_CM_STATUS status;

    TRACE_FUNCTION("cm_set_cmd_status()");  // Marcus: CCBS: 14/11/2002

    memset(&dis_connect, 0, sizeof(T_MFW_CM_DISCONNECT));
    memset(&cmd, 0, sizeof(T_MFW_CM_COMMAND));
    switch (mode)
    {
	case CHLD_MOD_RelHldOrUdub:
        TRACE_EVENT("CHLD_MOD_RelHldOrUdub");       // Marcus: CCBS: 14/11/2002
	     /*SPR#1965 - DS - Changed from MFW_CM_MT_REJECT to MFW_CM_TERMINATED because
             * the E_CM_DISCONNECT signal is not sent in cm_ok_disconnect() if cmd_hold = MFW_CM_MT_REJECT.
             * Port of 1962.
             */
            cmd_hold = MFW_CM_TERMINATED;

	    if (cm_search_callId(CAL_STAT_Wait, &id) == CM_OK)
	    {
            TRACE_EVENT("cm_search_callId(CAL_STAT_Wait, &id) == CM_OK");       // Marcus: CCBS: 14/11/2002
		    dis_connect.call_number = id;
	    }
	    else
	    {
            	   TRACE_EVENT("cm_search_callId(CAL_STAT_Wait, &id) != CM_OK");       // Marcus: CCBS: 14/11/2002
		    dis_connect.call_number = -1;	// call id?
	    }
				
	    break;
	case CHLD_MOD_RelActAndAcpt:

		TRACE_EVENT("CHLD_MOD_RelActAndAcpt");       // Marcus: CCBS: 14/11/2002
	        cmd_hold = MFW_CM_1_SEND;
	
		// Added xkundadu MMI-SPR 13873
		// Querying if there are any active calls, storing the index of the 
		// call and setting a flag if found to be multiparty.
		if (cm_search_callId(CAL_STAT_Active, &id) == CM_OK)
		{
			// Active call is present.
			dis_connect.call_number = id;	// call id ?
			cm_status(id, &status, 1);
			if (status.mtpy EQ MFW_MTPY_MEMBER)
			{
				bIsMultpty = TRUE;
			}
			else
			{
				bIsMultpty = FALSE;
			}
		
		}
		else
		{
			dis_connect.call_number = -1;	
		}
		// Added xkundadu MMI-SPR 13873
		// Querying if there are any held calls, storing the index of the call
		// to change the status to active call. 
		if (cm_search_callId(CAL_STAT_Held, &id) == CM_OK)
		{
			cmd.command     = CM_RETRIEVE;
			cmd.call_number = id;
		}
		// Added xkundadu MMI-SPR 13873
		// Checking is there any waiting call exists or not.
		if (cm_search_callId(CAL_STAT_Wait, &id) == CM_OK)
		{
			// This variable determines whether to accept/reject the call
			// in the cm_result_cmd().
			cmd.call_number=0;
		}
		else
		{
			cmd.call_number=-1;
		}
		
	    break;
		
	case CHLD_MOD_RelActSpec:
        TRACE_EVENT("CHLD_MOD_RelActSpec");       // Marcus: CCBS: 14/11/2002
	    dis_connect.call_number = (SHORT)atoi(call);
	    if (dis_connect.call_number)
	    {
            TRACE_EVENT("dis_connect.call_number");       // Marcus: CCBS: 14/11/2002
            cmd_hold = MFW_CM_TERMINATED;
	    }
		else
		{
            TRACE_EVENT("!dis_connect.call_number");       // Marcus: CCBS: 14/11/2002
		    cmd_hold = MFW_CM_MPTY_TERMINATED;
		}
	    break;
	case CHLD_MOD_HldActAndAcpt:
        TRACE_EVENT("CHLD_MOD_HldActAndAcpt");       // Marcus: CCBS: 14/11/2002
	    if (cm_search_callId(CAL_STAT_Wait, &id) == CM_OK)
	    {
            TRACE_EVENT("cm_search_callId(CAL_STAT_Wait, &id) == CM_OK");       // Marcus: CCBS: 14/11/2002
    		cmd_hold    = MFW_CM_MT_ACCEPT;
    		/*
    		 * initialize for result event
    		 */
    		cmd.command = CM_HOLD;
		#ifdef MMI_LINT_WARNING_REMOVAL /*a0393213 lint warnings removal*/
    		cc_id	    = id;
		#endif
	    }
	    else
	    {
            TRACE_EVENT("cm_search_callId(CAL_STAT_Wait, &id) != CM_OK");       // Marcus: CCBS: 14/11/2002
    		if ((cm_search_callId(CAL_STAT_Active, &id) == CM_OK)
    		    AND (cm_search_callId(CAL_STAT_Held, &id) == CM_OK))
    		{
                TRACE_EVENT("cmd.command = CM_SWAP");       // Marcus: CCBS: 14/11/2002
    		    cmd.command = CM_SWAP;
    		}
    		if ((cm_search_callId(CAL_STAT_Active, &id) != CM_OK)
    		    AND (cm_search_callId(CAL_STAT_Held, &id) == CM_OK))
    		{
                TRACE_EVENT("cmd.command = CM_RETRIEVE");       // Marcus: CCBS: 14/11/2002
    		    cmd.command = CM_RETRIEVE;
    		}
    		if ((cm_search_callId(CAL_STAT_Active, &id) == CM_OK)
    		    AND (cm_search_callId(CAL_STAT_Held, &id) != CM_OK))
    		{
                TRACE_EVENT("cmd.command = CM_HOLD");       // Marcus: CCBS: 14/11/2002
    		    cmd.command = CM_HOLD;
    		}
    		cmd.call_number = 0;
	    }
	    break;
	case CHLD_MOD_HldActExc:
        TRACE_EVENT("CHLD_MOD_HldActExc");       // Marcus: CCBS: 14/11/2002
	    cmd.command = CM_SPLIT_MULTIPARTY;
	    cmd.call_number = 0;
	    break;
	case CHLD_MOD_AddHld:
        TRACE_EVENT("CHLD_MOD_AddHld");       // Marcus: CCBS: 14/11/2002
	    cmd.command = CM_BUILD_MULTIPARTY;
	    cmd.call_number = 0;
	    break;
	case CHLD_MOD_Ect:
        TRACE_EVENT("CHLD_MOD_Ect");       // Marcus: CCBS: 14/11/2002
	    cmd.command = CM_ETC;
	    cmd.call_number = 0;
	    break;
	case CHLD_MOD_Ccbs:
        TRACE_EVENT("CHLD_MOD_Ccbs");       // Marcus: CCBS: 14/11/2002
	    cmd.command = CM_CCBS;
	    cmd.call_number = 0;
	    break;
	case CHLD_MOD_HldActDial:
        TRACE_EVENT("CHLD_MOD_HldActDial");       // Marcus: CCBS: 14/11/2002
	    cmd_hold = MFW_CM_HOLD_AND_DIAL;
	    /*
	     * initialize for result event
	     */
	    cmd.command = CM_HOLD;
	    /*cc_id	= id;*//*a0393213 lint warnings:var not initialized soln:commented as it is not needed*/
	    break;
    }
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_check_mo_number  |
+--------------------------------------------------------------------+


   PURPOSE : start mobile originated call

*/

T_MFW cm_check_mo_number(UBYTE *number)
{
  UBYTE i;
//	Apr 14, 2005	REF: CRR 29991   xpradipg
//	local definition
  #ifdef FF_MMI_OPTIM
  UBYTE count = 0;
  UBYTE blackListNum[MFW_NUM_LEN];
  int8 file;
  #endif

  /* check whether a redialing exists */
  if (call_stat EQ REDIALING_CALL)
  {
    cm_stop_redialTim();
    rdl.redial_mode = CM_REDIAL_OUTGOING;
    cm_signal(E_CM_REDIAL, &rdl);
  }


  if (!number)
    return CM_OK;
//	Apr 14, 2005	REF: CRR 29991   xpradipg
//	the list is read from the file
#ifdef FF_MMI_OPTIM
	if((	file = flash_MMI_blackList_open()) > 0)
	{
		if(flash_MMI_blackList_read(file,(U8*)&count,1,0) > 0)
		{
			for(i=0; i < (int)count; i++)
			{
				if(flash_MMI_blackList_read(file,(U8*)blackListNum,MFW_NUM_LEN,(i*MFW_NUM_LEN)+1) > 0)
				{
					if(!strcmp((char*)blackListNum, (char*)number))
					{
						flash_MMI_blackList_close(file);
						return CM_BLACKLIST;
					}
				}
			}
		}
		flash_MMI_blackList_close(file);
	}
#else	
	  /* check whether this number is in blacklist */
	  for (i=0; i<black_list.count; i++)
	  {
	    if (!strcmp((char *)black_list.number[i], (char *)number))
	      return CM_BLACKLIST;
	  }
#endif	

  /* check the type of this number */
  if (cm_check_emergency(number) EQ TRUE)
    call_type = MFW_ECC_CALL;
  else
    call_type = MFW_NORMAL_CALL;

  return CM_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_set_call_mode    |
+--------------------------------------------------------------------+


   PURPOSE : set call mode

*/

void cm_set_call_mode(T_MFW_CM_CALL_TYPE mode)
{
  switch (mode)
  {
    case VOICE_CALL:
    case AuxVOICE_CALL: /*MC, SPR 1026, assume line 2 is handled like line 1*/
    case DATA_CALL:
    case FAX_CALL:
      sAT_PlusCMOD(CMD_SRC_LCL, CMOD_MOD_Single);
      break;
    case VFD_VOICE:
    case VFD_DATA:
      sAT_PlusCMOD(CMD_SRC_LCL, CMOD_MOD_VoiceFlwdDat);
      break;
    case VAD_VOICE:
    case VAD_DATA:
      sAT_PlusCMOD(CMD_SRC_LCL, CMOD_MOD_VoiceDat);
      break;
    case VAF_VOICE:
    case VAF_FAX:
      sAT_PlusCMOD(CMD_SRC_LCL, CMOD_MOD_VoiceFax);
      break;
    default:
      break;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_mo_idx	     |
+--------------------------------------------------------------------+


   PURPOSE : start mobile originated call

*/

T_MFW cm_mo_idx(T_KSD_SEQPARAM *para, T_MFW_CM_CALL_TYPE mode)
{
  T_MFW_PHB_LIST entries;
  T_MFW_PHB_ENTRY   entry;
  T_ACI_RETURN	    aci_response; /*a0393213 compiler warnings removal - type of var changed*/
  T_MFW mfw_response;	/*a0393213 compiler warnings removal - a var introduced*/
//RM 19_09_2000
#ifdef NO_ASCIIZ
  T_ACI_PB_TEXT mfwpbText;
#endif
//RM

  entries.entry = &entry;

  /* search this number from phonebook */
  if ((phb_read_entries(PHB_ADN, (UBYTE)para->dialIdx.index,
			MFW_PHB_INDEX, 1,
			&entries) EQ MFW_PHB_OK)
      AND ((int)entries.num_entries > 0))
  {
    mfw_response= cm_check_mo_number(entries.entry->number);
    if (mfw_response NEQ CM_OK)
      return mfw_response;
  }
  else
  {
    mfw_response = cm_check_mo_number(0);
    if (mfw_response NEQ CM_OK)
      return mfw_response;
  }

  /* set the outgoing call status */
  call_stat	 = OUTGOING_CALL;
  redial_count	 = 0;
  cc_stat.type	 = OUTGOING_CALL;

  cm_set_call_mode(mode);

//RM 19_09_2000 instead change in ksd structure T_KSD_SEQPARAM etc., later to change !!!!
#ifdef NO_ASCIIZ
	if(para->dialIdx.str NEQ NULL)
	{
		mfwpbText.cs = CS_GsmDef;
		mfwpbText.len = strlen(para->dialIdx.str);
		memcpy(mfwpbText.data,para->dialIdx.str,mfwpbText.len);
	}
	else
	{
		mfwpbText.cs = CS_NotPresent;
		mfwpbText.len = 0;
	}
	aci_response= sAT_Dm(CMD_SRC_LCL, &mfwpbText,
			   para->dialIdx.mem, para->dialIdx.index,
			   para->dialIdx.clirOvrd, para->dialIdx.cugCtrl,
			   para->dialIdx.callType);
#else
	aci_response= sAT_Dm(CMD_SRC_LCL, para->dialIdx.str,
			   para->dialIdx.mem, para->dialIdx.index,
			   para->dialIdx.clirOvrd, para->dialIdx.cugCtrl,
			   para->dialIdx.callType);
#endif

  if ((aci_response== AT_FAIL) OR (aci_response== AT_BUSY))
  {
    call_stat	 = MFW_CALL_FREE;
	  return cm_error_code(AT_CMD_D);
  }

  return CM_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_mo_call	     |
+--------------------------------------------------------------------+


   PURPOSE : start mobile originated call

*/

T_MFW cm_mo_call(UBYTE *number, T_MFW_CM_CALL_TYPE type)
{
  CHAR			   * rest;
  T_KSD_SEQPARAM	     para;
  T_MFW_SS_RETURN	     typ;
  T_MFW_SS_RETURN	     new_typ;
  CHAR			   * string;
  UBYTE 		     cm_grp = 0;  // RAVI
  /*a0393213 compiler warning removal -- removed clir_status def*/
  UBYTE 		     colr_status;
  UBYTE 		     prefix;
  SHORT 		     id;
  T_ACI_RETURN		     res,aci_result;//aci_result was added to fix the bug MMI-SPR 16545
// Sep 01, 2006 REF:OMAPS00090555  x0039928  
// Fix: If flag FF_MMI_PB_OPTIM is defined then allocate memory for buf, phone and num_save variables 
// through heap instead of stack.
#ifdef FF_MMI_PB_OPTIM  
  char		     * buf = (char*)mfwAlloc(80);
  char 		     * phone = (char*)mfwAlloc(MFW_NUM_LEN);
  char 		     * num_save = (char*)mfwAlloc(MFW_STRING_LEN);
#else
  char		     buf[80];
  UBYTE 		     phone[MFW_NUM_LEN];
  UBYTE 		     num_save[MFW_STRING_LEN];
#endif

  TRACE_FUNCTION ("cm_mo_call()");

// Sep 01, 2006 REF:OMAPS00090555  x0039928  
#ifdef FF_MMI_PB_OPTIM  
  memset(buf,0,80);
  memset(phone,0,MFW_NUM_LEN);
  memset(num_save,0,MFW_STRING_LEN);
  strncpy(num_save, (char *)number, MFW_STRING_LEN-1);
#else
  strncpy((char *)num_save, (char *)number, MFW_STRING_LEN-1);
  num_save[MFW_STRING_LEN-1] = 0;
#endif
// Oct 10, 2006  DR: OMAPS00093720   x0039928 
// Fix: Set ton to MFW_TON_INTERNATIONAL
  if(number[0] EQ '+')
  {
    out_call.ton = MFW_TON_INTERNATIONAL;
  }
else
 {
    out_call.ton = MFW_TON_UNKNOWN;
 }
  /* check the type of number (key sequence), current no function */
  typ = ss_decode(number, &rest, &para);
  TRACE_EVENT_P1("Type: %d", typ);
  if (typ == MFW_SS_DIAL)
	{
	mfwStrncpy((char *)phone, para.dial.number, MFW_NUM_LEN);

	}

  if (typ == MFW_SS_HOLD)
  {
    cm_set_cmd_status(para.chld.mode, para.chld.call);

    // Feb 27, 2006    REF:DR OMAPS00067884  x0039928	
    // Fix :  sAT_PercentCHLD() is called if the mode > CHLD_MOD_RelDialCall
    if(para.chld.mode > CHLD_MOD_RelDialCall)
	aci_result = sAT_PercentCHLD(CMD_SRC_LCL, para.chld.mode, para.chld.call);
    else
	aci_result = sAT_PlusCHLD(CMD_SRC_LCL, para.chld.mode, para.chld.call);
	
	//  Apr 26, 2004 REF: CRR 16545 Ajith K P
	//  If sAT_PlusCHLD() Fails, Send the user entered string as USSD to network using sAT-Dn().
	//  So if its not failed just return CM_CTRL_STR, otherwise call sAT_Dn().	
    if ( (aci_result  EQ AT_CMPL ) OR (aci_result  EQ AT_EXCT ) OR (aci_result  EQ AT_BUSY ))
    {
// Sep 01, 2006 REF:OMAPS00090555  x0039928  
// Fix: Free the memory allocated for buf, phone and num_save variables
#ifdef FF_MMI_PB_OPTIM  
    if(buf)
	mfwFree((U8*) buf,80);
    if(phone)
	mfwFree((U8*) phone,MFW_NUM_LEN);	
    if(num_save)
      	mfwFree((U8*) num_save,MFW_STRING_LEN);
#endif	
		return CM_CTRL_STR;
	}
  }
  //MC 11.04.02
  if (typ == MFW_SS_ECT)
  {
	cm_set_cmd_status(para.chld.mode, para.chld.call);
	if (sAT_PlusCHLD(CMD_SRC_LCL, CHLD_MOD_Ect, 0) != AT_EXCT)
	{
		//Aug 10, 2005   REF: MMI-SPR-30099 x0018858
		//commented this return value.
		//	return CM_ERROR;
	}
	else
	{
// Sep 01, 2006 REF:OMAPS00090555  x0039928  
// Fix: Free the memory allocated for buf,phone and num_save variables	
	#ifdef FF_MMI_PB_OPTIM  
 	 	if(buf)
		    mfwFree((U8*) buf,80);
  		if(phone)
		    mfwFree((U8*) phone,MFW_NUM_LEN);	
 		if(num_save)
  		   mfwFree((U8*) num_save,MFW_STRING_LEN);
	#endif	
		return CM_CTRL_STR;
	}	
  }
  //MC

  if (typ == MFW_SS_DIAL_IDX)
  {
// Sep 01, 2006 REF:OMAPS00090555  x0039928  
// Fix: Free the memory allocated for buf,phone and num_save variables  
  	#ifdef FF_MMI_PB_OPTIM  
 	 	if(buf)
		    mfwFree((U8*) buf,80);
  		if(phone)
		    mfwFree((U8*) phone,MFW_NUM_LEN);	
 		if(num_save)
  		   mfwFree((U8*) num_save,MFW_STRING_LEN);
	#endif	
      return cm_mo_idx(&para, type);
  }

  switch (typ)
  {

//NM
    case MFW_SS_CLIR: //jgg wrong not CM_OK should be CM_SS, however cm_grp not used after funtion callnumber called

    case MFW_SS_DIAL:
      cm_grp = CM_OK;
      break;

    case MFW_SS_DTMF:
    case MFW_SS_CF:
    case MFW_SS_CB:
    case MFW_SS_CLIP:
    case MFW_SS_COLR:
    case MFW_SS_COLP:
    case MFW_SS_WAIT:
    case MFW_SS_HOLD:
    case MFW_SS_CCBS:
    case MFW_SS_REG_PW:
//Aug 10, 2005   REF: MMI-SPR-30099 x0018858
//Added this case statement.
    case MFW_SS_ECT:
      cm_grp = CM_SS;
      break;

    case MFW_SS_USSD:
      cm_grp = CM_USSD;
      break;

    case MFW_SS_SIM_LOCK:
    case MFW_SS_SIM_REG_PW:
    case MFW_SS_SIM_UNBLCK_PIN:
      cm_grp = CM_SIM;
      break;

    case MFW_SS_MMI:
      cm_grp = CM_MMI;
      break;

/* SPR#1352 - SH - TTY
 * If an SS string has been entered, temporarily enable or disable
 * TTY for this call only. */

	case MFW_SS_TTY_NEXTCALL_ON:
	case MFW_SS_TTY_NEXTCALL_OFF:
		{
			if (typ==MFW_SS_TTY_NEXTCALL_ON)
			{
				cm_tty_enable(TRUE);
			}
			else
			{
				cm_tty_enable(FALSE);
			}
// Sep 01, 2006 REF:OMAPS00090555  x0039928  			
	#ifdef FF_MMI_PB_OPTIM  
		strcpy(num_save, (num_save + 4)); /* Ignore SS string when dialling */
	#else
	      	strcpy((char *)num_save, (char *)&num_save[4]); /* Ignore SS string when dialling */
	#endif
			rest = (char *)&number[4];	/* Look at number following SS string */
		}
		break;
/* SH end */

    default:
// Sep 01, 2006 REF:OMAPS00090555  x0039928  
// Fix: Free the memory allocated for buf,phone and num_save variables    
      	#ifdef FF_MMI_PB_OPTIM  
 	 	if(buf)
		    mfwFree((U8*) buf,80);
  		if(phone)
		    mfwFree((U8*) phone,MFW_NUM_LEN);	
 		if(num_save)
  		   mfwFree((U8*) num_save,MFW_STRING_LEN);
	#endif	
      return CM_ERROR;
  }

  string = rest;
  
  colr_status = 0;
// Sep 01, 2006 REF:OMAPS00090555  x0039928    
#ifndef FF_MMI_PB_OPTIM    
  buf[0] = '\0';
#endif
  cc_stat.prefix[0] = '\0';
  while(strlen(string))
  {
  	TRACE_EVENT("In the while");
    new_typ = ss_decode((UBYTE *)string, &rest, &para);
// /*
    if (new_typ == MFW_SS_CLIR)
    {
      
      if (para.Clir.mode == CLIR_MOD_Supp)
	  {
	    strcat((char *)cc_stat.prefix, "*31#");
	    TRACE_EVENT ("CLIR_MOD_Supp");
	  }
      if (para.Clir.mode == CLIR_MOD_Invoc)
	  {
	    strcat((char *)cc_stat.prefix, "#31#");
		TRACE_EVENT ("CLIR_MOD_Invoc");
      }
    }
    if (new_typ == MFW_SS_COLR)
      colr_status = 1;
    if (new_typ == MFW_SS_DIAL)
    {
    	TRACE_EVENT("It's a dial");
	  /* SPR#1352 - SH - only copy string to dial, not whole string */
      if (typ==MFW_SS_TTY_NEXTCALL_ON || typ==MFW_SS_TTY_NEXTCALL_OFF)
      {
      	typ = new_typ;
      	TRACE_EVENT_P1("phone now holds: %s", phone);
      }
      else
      /* end SH */
      {
      	typ = new_typ;
      	mfwStrncpy((char *)phone, para.dial.number, MFW_NUM_LEN);
      }
    }
    string = rest;
  }

  if (!colr_status AND (typ == MFW_SS_DIAL))
  {
    prefix = cli_prefix & 0xc0;
    if (prefix == 0x80)
    {
      strcat(buf, "*77#");
      strcat((char *)cc_stat.prefix, "*77#");
    }
    if (prefix == 0xc0)
    {
      strcat(buf, "#77#");
      strcat((char *)cc_stat.prefix, "#77#");
    }
  }

  if (typ == MFW_SS_DIAL)
  {
    res = (T_ACI_RETURN)cm_check_mo_number((UBYTE*)phone); /*a0393213 lint warnings removal - typecast done*/
    if ( res != CM_OK)
    {
// Sep 01, 2006 REF:OMAPS00090555  x0039928  
// Fix: Free the memory allocated for buf,phone and num_save variables    
	#ifdef FF_MMI_PB_OPTIM  
 	 	if(buf)
		    mfwFree((U8*) buf,80);
  		if(phone)
		    mfwFree((U8*) phone,MFW_NUM_LEN);	
 		if(num_save)
  		   mfwFree((U8*) num_save,MFW_STRING_LEN);
	#endif	
	return res;
    }

    /* set the outgoing call status */
    call_stat	   = OUTGOING_CALL;
    redial_count   = 0;
    cc_stat.type   = OUTGOING_CALL;
    cm_grp = CM_OK;
    currConnecting = 1;
    cm_set_call_mode(type);
  }

  strcat(buf, (char *)num_save);
  if (cm_search_callId(CAL_STAT_Active, &id) EQ CM_OK)
  {
    /*
     * at least one active, this must be put on hold
     * by ACI first, then dial
     */
    cm_set_cmd_status(CHLD_MOD_HldActDial, "");
#ifdef MMI_LINT_WARNING_REMOVAL 	/*a0393213 lint warnings removal*/
    cc_id = id;
#endif
  }
  if (type == DATA_CALL)
	para.dial.callType = D_TOC_Data;

//  Apr 22, 2004        REF: CRR MMI-SPR-16873  Rashmi C N(Sasken) 
//		Using the CLIR status maintained in the flash, while making a call
	if (!FFS_flashData.CLIR)
	{
		res = sAT_Dn(CMD_SRC_LCL, buf, D_CLIR_OVRD_Supp, D_CUG_CTRL_NotPresent, para.dial.callType); /*a0393213 compiler warning removal-- -1 changed to D_CUG_CTRL_NotPresent*/
	}
	else 
	{
		res = sAT_Dn(CMD_SRC_LCL, buf, D_CLIR_OVRD_Invoc, D_CUG_CTRL_NotPresent, para.dial.callType); /*a0393213 compiler warning removal-- -1 changed to D_CUG_CTRL_NotPresent*/
	}
	
  if ((res == AT_FAIL) OR (res == AT_BUSY))
  {
// Sep 01, 2006 REF:OMAPS00090555  x0039928  
// Fix: Free the memory allocated for buf,phone and num_save variables  
  	#ifdef FF_MMI_PB_OPTIM  
 	 	if(buf)
		    mfwFree((U8*) buf,80);
  		if(phone)
		    mfwFree((U8*) phone,MFW_NUM_LEN);	
 		if(num_save)
  		   mfwFree((U8*) num_save,MFW_STRING_LEN);
	#endif	
    if (typ == MFW_SS_CLIP)
      return CM_CLI;

    call_stat	 = MFW_CALL_FREE;
	  return cm_error_code(AT_CMD_D);
  }
  if (res == AT_CMPL)
  {
	switch (typ)
	{
	  case MFW_SS_CLIR:
      case MFW_SS_CLIP:
      case MFW_SS_COLR:
      case MFW_SS_COLP:
// Sep 01, 2006 REF:OMAPS00090555  x0039928  
// Fix: Free the memory allocated for buf,phone and num_save variables	  	
	  #ifdef FF_MMI_PB_OPTIM  
 	 	if(buf)
		    mfwFree((U8*) buf,80);
  		if(phone)
		    mfwFree((U8*) phone,MFW_NUM_LEN);	
 		if(num_save)
  		   mfwFree((U8*) num_save,MFW_STRING_LEN);
	#endif	
	return CM_CLI;
      default:
	break;
    }
  }
// Sep 01, 2006 REF:OMAPS00090555  x0039928  
// Fix: Free the memory allocated for buf,phone and num_save variables  
  #ifdef FF_MMI_PB_OPTIM  
  	if(buf)
	    mfwFree((U8*) buf,80);
  	if(phone)
	    mfwFree((U8*) phone,MFW_NUM_LEN);	
 	if(num_save)
  	   mfwFree((U8*) num_save,MFW_STRING_LEN);
  #endif	
  return cm_grp;
}


/*
+--------------------------------------------------------------------+
| PROJECT: GSM-MFW (8417)		MODULE:  MFW_CB 	     |
| STATE  : code 			ROUTINE: rAT_PlusCRING_OFF   |
+--------------------------------------------------------------------+


   PURPOSE :   Alert Off indication from ACI

*/

#ifdef FF_2TO1_PS
GLOBAL void rAT_PlusCRING_OFF ( SHORT cId )
{
    T_MFW_CM_CALL_STAT stat;
    UBYTE	       ton;
    T_CAUSE_ps_cause report;  /* Marcus: Extended error reporting: 12/11/2002 */
    UBYTE	       mode;

    TRACE_FUNCTION ("rAT_PlusCRING_OFF()");

    if (cm_search_callStat(cId, &stat, &ton, &mode) == CM_OK)
    {
	call_stat = MFW_CALL_FREE;
	call_type = MFW_CALL_FREE;

	//TISH: if there is inband tone, it should not disconnect the call. OMAPS00104986
 	if (stat == CALL_DEACTIVE && !cm_cpi.inband)
	// if (stat == CALL_DEACTIVE)
	{
	    dis_connect.call_number = cId;

	    qAT_PlusCEER( CMD_SRC_LCL, &report);
        /* Marcus: Extended error reporting: 12/11/2002: Start */

//        TRACE_EVENT_P1("qAT_PlusCEER(%04X)", report);
        if (report.ctrl_value != CAUSE_is_from_nwcc)
        {
            /* Treat Condat defined cause values as if no cause given */
            dis_connect.cause = NOT_PRESENT_8BIT;
        }
        else
        {
            /* Standards defined cause value is given */
            dis_connect.cause = report.value.nwcc_cause;
        }
        TRACE_EVENT_P1("dis_connect.cause = %02X)", dis_connect.cause);
        /* Marcus: Extended error reporting: 12/11/2002: End */
	    cm_signal(E_CM_DISCONNECT, &dis_connect);
	    return;
	}
	cm_signal(E_CM_RING_OFF, 0);
    }
}
#else
GLOBAL void rAT_PlusCRING_OFF ( SHORT cId )
{
    T_MFW_CM_CALL_STAT stat;
    UBYTE	       ton;
    USHORT	       report;  /* Marcus: Extended error reporting: 12/11/2002 */
    UBYTE	       mode;

    TRACE_FUNCTION ("rAT_PlusCRING_OFF()");

    if (cm_search_callStat(cId, &stat, &ton, &mode) == CM_OK)
    {
	call_stat = MFW_CALL_FREE;
	call_type = MFW_CALL_FREE;

	//TISH: if there is inband tone, it should not disconnect the call. OMAPS00104986
 	if (stat == CALL_DEACTIVE && !cm_cpi.inband)
	// if (stat == CALL_DEACTIVE)
	{
	    dis_connect.call_number = cId;

	    qAT_PlusCEER( CMD_SRC_LCL, &report);
        /* Marcus: Extended error reporting: 12/11/2002: Start */
        TRACE_EVENT_P1("qAT_PlusCEER(%04X)", report);
        if (GET_CAUSE_DEFBY(report) == DEFBY_CONDAT)
        {
            /* Treat Condat defined cause values as if no cause given */
            dis_connect.cause = NOT_PRESENT_8BIT;
        }
        else
        {
            /* Standards defined cause value is given */
            dis_connect.cause = GET_CAUSE_VALUE(report);
        }
        TRACE_EVENT_P1("dis_connect.cause = %02X)", dis_connect.cause);
        /* Marcus: Extended error reporting: 12/11/2002: End */
	    cm_signal(E_CM_DISCONNECT, &dis_connect);
	    return;
	}
	cm_signal(E_CM_RING_OFF, 0);
    }
}
#endif

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	 MODULE  : MFW_CM	     |
| STATE   : code			 ROUTINE : rAT_PlusCRING     |
+--------------------------------------------------------------------+


   PURPOSE : Alert indication from ACI

*/
/*a0393213 warnings removal - second and third parameter changed as a result of ACI enum change*/
void rAT_PlusCRING(T_ACI_CRING_MOD mode,
		   T_ACI_CRING_SERV_TYP type1,
		   T_ACI_CRING_SERV_TYP type2)
{
  SHORT call_number;

  TRACE_FUNCTION ("rAT_PlusCRING()");

  in_call.result = Present;

  /* search the call identifier */
  if (cm_search_callId(CAL_STAT_Wait, &call_number) == CM_OK)
    in_call.call_number = call_number;
  else
    in_call.call_number = 0;

  /* If redial is active, it must be stoped. */
  if (call_stat == REDIALING_CALL)
  {
    cm_stop_redialTim();
    rdl.redial_mode = CM_REDIAL_INCOMING;
    cm_signal(E_CM_REDIAL, &rdl);
  }

  if (mode EQ CRING_MOD_Direct)
  {
    switch (type1)
    {
	    /* Voice call */
      case CRING_SERV_TYP_Voice:
	if (type2 EQ CRING_SERV_TYP_NotPresent)
	{
	  in_call.type = VOICE_CALL;
	  cm_set_call_mode(VOICE_CALL);
	}
	else
	{
	  switch (type2)
	  {
	    case CRING_SERV_TYP_Sync:
	    case CRING_SERV_TYP_Async:
	    case CRING_SERV_TYP_RelSync:
	    case CRING_SERV_TYP_RelAsync:
	      in_call.type = VFD_VOICE;
	      cm_bc_parameter(type2);
	      cm_set_call_mode(VFD_VOICE);
	      break;
	      /*MC, SPR 1026, check for line1 and line2*/
	    case CRING_SERV_TYP_Voice:
	      in_call.type = VOICE_CALL;
	      cm_set_call_mode(VOICE_CALL);
	      break;
	    case CRING_SERV_TYP_AuxVoice:
	      cm_set_call_mode(AuxVOICE_CALL);
	      in_call.type = AuxVOICE_CALL;
	      break;
	      /*MC, end*/
	    default:
	      in_call.type = VOICE_CALL;
	      cm_set_call_mode(VOICE_CALL);
	      break;
	  }
	}
	break;
	/*MC, SPR 1026, Added case of Type1=CRING_SERV_TYP_AuxVoice*/
	/*assuming  line 2 is handled like line 1*/
      case CRING_SERV_TYP_AuxVoice:
	if (type2 EQ CRING_SERV_TYP_NotPresent)
	{
	  in_call.type = AuxVOICE_CALL;
	  cm_set_call_mode(AuxVOICE_CALL);
	}
	else
	{
	  switch (type2)
	  {
	    case CRING_SERV_TYP_Sync:
	    case CRING_SERV_TYP_Async:
	    case CRING_SERV_TYP_RelSync:
	    case CRING_SERV_TYP_RelAsync:
	      in_call.type = VFD_VOICE;
	      cm_bc_parameter(type2);
	      cm_set_call_mode(VFD_VOICE);
	      break;
	    case CRING_SERV_TYP_Voice:
	      in_call.type = VOICE_CALL;
	      cm_set_call_mode(VOICE_CALL);
	      break;
	    case CRING_SERV_TYP_AuxVoice:
	      cm_set_call_mode(AuxVOICE_CALL);
	      in_call.type = AuxVOICE_CALL;
	      break;
	    default:
	      in_call.type = VOICE_CALL;
	      cm_set_call_mode(VOICE_CALL);
	      break;
	  }
	}
	break;
	/*MC, end*/
	    /* Data call */
	    case CRING_SERV_TYP_Sync:
      case CRING_SERV_TYP_Async:
      case CRING_SERV_TYP_RelSync:
      case CRING_SERV_TYP_RelAsync:
		    in_call.type = DATA_CALL;
	cm_bc_parameter(type1);
	cm_set_call_mode(DATA_CALL);
		    break;

      case CRING_SERV_TYP_Fax:
		    in_call.type = FAX_CALL;
	in_call.bc1.conn_elem	= Transparent;
	cm_set_call_mode(FAX_CALL);
		  break;

      default:
		    in_call.result = NotPresent;
	break;
    }
    call_stat = INCOMING_CALL;
  }

  if (mode EQ CRING_MOD_Alternate)
  {
	  cm_build_callType(type1, type2);
    cm_bc_parameter(type1);
    cm_bc_parameter(type2);
    cm_set_call_mode(in_call.type);
  }
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: rAT_PlusCLIP	     |
+--------------------------------------------------------------------+


   PURPOSE : The calling line identification for an incomming call.

*/

#ifdef NO_ASCIIZ
void rAT_PlusCLIP(T_ACI_CLIP_STAT   stat,
		  CHAR		  * number,
		  T_ACI_TOA	  * type,
		  U8		    validity,
		  CHAR		  * subaddr,
		  T_ACI_TOS	  * satype,
		  T_ACI_PB_TEXT   * alpha)
{
  int flag;

  TRACE_FUNCTION ("rAT_PlusCLIP()");

  flag = 0;

  if (number != NULL)
  {
    mfwStrncpy((char *)in_call.number,  (char *)number, MFW_NUM_LEN);
    in_call.ton = phb_cvtTon(type->ton);
    flag = 1;
  }
  else
    memset(in_call.number, '\0', MFW_NUM_LEN);

  if (alpha != NULL)
  {
    in_call.name.len = alpha->len;
	if (alpha->len)
    {
     memcpy(in_call.name.data,
	    alpha->data,
	    alpha->len);
     flag = 1;
    }
  }
  else
    in_call.name.len = 0;


  if (subaddr != NULL)
  {
    mfwStrncpy((char *)in_call.subaddr, (char *)subaddr, MFW_SUBADR_LEN);
    flag = 1;
  }
  else
    memset(in_call.subaddr, '\0', MFW_SUBADR_LEN);

  if (flag == 1)
    in_call.result = Present;
  else
    in_call.result = 0;

  cm_signal(E_CM_CW, &in_call);
}

#else

void rAT_PlusCLIP(T_ACI_CLIP_STAT   stat,
		  CHAR		  * number,
		  T_ACI_TOA	  * type,
		  U8		    validity,
		  CHAR		  * subaddr,
		  T_ACI_TOS	  * satype,
		  CHAR		  * alpha)
{
  int flag;

  TRACE_FUNCTION ("rAT_PlusCLIP()");

  flag = 0;
//  in_call.result = 0;

  if (number != NULL)
  {
    mfwStrncpy((char *)in_call.number,  (char *)number, MFW_NUM_LEN);
    in_call.ton = phb_cvtTon(type->ton);
    flag = 1;
  }
  else
    memset(in_call.number, '\0', MFW_NUM_LEN);

  if (alpha != NULL)
  {
    mfwStrncpy((char *)in_call.name,    (char *)alpha, MFW_TAG_LEN);
    flag = 1;
  }
  else
    memset(in_call.name, '\0', MFW_TAG_LEN);

  if (subaddr != NULL)
  {
    mfwStrncpy((char *)in_call.subaddr, (char *)subaddr, MFW_SUBADR_LEN);
    flag = 1;
  }
  else
    memset(in_call.subaddr, '\0', MFW_SUBADR_LEN);

  if (flag == 1)
    in_call.result = Present;
  else
    in_call.result = 0;

  cm_signal(E_CM_CW, &in_call);
}
#endif

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-MFW (8417)		 MODULE  :  MFW_CM	     |
| STATE   : code			 ROUTINE : rAT_PlusCOLP      |
+--------------------------------------------------------------------+


   PURPOSE :   Connected line presentation indication from ACI

*/

#ifdef NO_ASCIIZ
GLOBAL void rAT_PlusCOLP  ( T_ACI_COLP_STAT stat,
			    CHAR* number,
			    T_ACI_TOA* type,
			    CHAR* subaddr,
			    T_ACI_TOS* satype,
			    T_ACI_PB_TEXT* alpha)
{

  TRACE_FUNCTION ("rAT_PlusCOLP()");

  if (number != NULL)
  {
    mfwStrncpy((char *)out_call.number, (char *)number, MFW_NUM_LEN);
    out_call.ton = phb_cvtTon(type->ton);
  }
  else
    memset(out_call.number, '\0', MFW_NUM_LEN);

  /*NM, p001  */
  if(alpha != NULL)
  /*NM, p001 end */
  {
  out_call.name.len = MINIMUM(alpha->len, MFW_TAG_LEN);
  if (alpha->len)
    memcpy(out_call.name.data,
	   alpha->data,
	   alpha->len);
  }

  if (subaddr != NULL)
    mfwStrncpy((char *)out_call.subaddr, (char *)subaddr, MFW_SUBADR_LEN);
  else
    memset(out_call.subaddr, '\0', MFW_SUBADR_LEN);

  if (number != NULL)
    cm_signal(E_CM_COLP, &out_call);
}

#else

GLOBAL void rAT_PlusCOLP  ( T_ACI_COLP_STAT stat,
			    CHAR* number,
			    T_ACI_TOA* type,
			    CHAR* subaddr,
			    T_ACI_TOS* satype,
			    CHAR* alpha)
{
  TRACE_FUNCTION ("rAT_PlusCOLP()");

// patch PMA 15 06 99 from Andreas
//  if (stat == -1)
//    return;

  if (number != NULL)
  {
    mfwStrncpy((char *)out_call.number, (char *)number, MFW_NUM_LEN);
    out_call.ton = phb_cvtTon(type->ton);
  }
  else
    memset(out_call.number, '\0', MFW_NUM_LEN);

  if (alpha != NULL)
    mfwStrncpy((char *)out_call.name, (char *)alpha, MFW_TAG_LEN);
  else
    memset(out_call.name, '\0', MFW_TAG_LEN);

  if (subaddr != NULL)
    mfwStrncpy((char *)out_call.subaddr, (char *)subaddr, MFW_SUBADR_LEN);
  else
    memset(out_call.subaddr, '\0', MFW_SUBADR_LEN);

// patch PMA 15 06 99 from Andreas
  if (number != NULL)
	  cm_signal(E_CM_COLP, &out_call);
}
#endif

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-MFW (8417)		 MODULE  : MFW_CM	     |
| STATE   : code			 ROUTINE : rAT_PlusCCWA      |
+--------------------------------------------------------------------+


   PURPOSE :   Connected line presentation indication from ACI

*/

#ifdef NO_ASCIIZ

GLOBAL void rAT_PlusCCWA ( T_ACI_CLSSTAT * clsStatLst,
			   CHAR 	 * number,
			   T_ACI_TOA	 * type,
			   U8		   validity,
			   T_ACI_CLASS	   call_class,
			   T_ACI_PB_TEXT * alpha)
{
    SHORT call_number;

    TRACE_FUNCTION("rAT_PlusCCWA()");

    in_call.result = NotPresent;
    if (clsStatLst == 0)
    {
	if (number != NULL)
	{
	    mfwStrncpy((char *)in_call.number, (char *)number, MFW_NUM_LEN);
	    in_call.ton = phb_cvtTon(type->ton);
	}
	else
	    memset(in_call.number, '\0', MFW_NUM_LEN);

	if (alpha != NULL)
	  in_call.name.len = alpha->len;
	else
	  in_call.name.len = 0;

	if (in_call.name.len && alpha!=NULL) /*a0393213 lint warning removal - check for alpha done*/
	{
	    memcpy(in_call.name.data,
		   alpha->data,
		   alpha->len);
	}

	in_call.type = cm_callTyp_cvt(call_class);

	if (number != 0 || in_call.name.len != 0 || in_call.type != NONE_TYPE)
	    in_call.result = Present;

	cm_search_callId(CAL_STAT_Wait, &call_number);
	in_call.call_number = call_number;

	cm_signal(E_CM_CW, &in_call);
    }
}

#else

GLOBAL void rAT_PlusCCWA ( T_ACI_CLSSTAT * clsStatLst,
			   CHAR 	 * number,
			   T_ACI_TOA	 * type,
			   U8		   validity,
			   T_ACI_CLASS	   call_class,
			   CHAR 	 * alpha)
{
    SHORT call_number;

    in_call.result = NotPresent;
    if (clsStatLst == 0)
    {
	if (number != NULL)
	{
	    mfwStrncpy((char *)in_call.number, (char *)number, MFW_NUM_LEN);
	    in_call.ton = phb_cvtTon(type->ton);
	}
	else
	    memset(in_call.number, '\0', MFW_NUM_LEN);

	if (alpha != NULL)
	    mfwStrncpy((char *)in_call.name, (char *)alpha, MFW_TAG_LEN);
	else
	    memset(in_call.name, '\0', MFW_TAG_LEN);

	in_call.type = cm_callTyp_cvt(call_class);

	if (number != 0 || alpha != 0 || in_call.type != NONE_TYPE)
	    in_call.result = Present;

	cm_search_callId(CAL_STAT_Wait, &call_number);
	in_call.call_number = call_number;

	cm_signal(E_CM_CW, &in_call);
    }
}
#endif

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	 MODULE  : MFW_CM	     |
| STATE   : code			 ROUTINE : cm_mt_reject      |
+--------------------------------------------------------------------+


   PURPOSE : Reject the incoming call.

*/

void cm_mt_reject()
{
    TRACE_FUNCTION ("cm_mt_reject()");

    if (currAccepting)
    {
	currAccepting = 0; /* ES!! 210999 */
	sAT_Abort(CMD_SRC_LCL, AT_CMD_A);
	return;
    }

    cmd_hold = MFW_CM_MT_REJECT;
    cm_search_callId(CAL_STAT_Wait, &dis_connect.call_number);
    sAT_PlusCHLD(CMD_SRC_LCL, CHLD_MOD_RelHldOrUdub, 0);
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_mt_accept	     |
+--------------------------------------------------------------------+


   PURPOSE : Accept the incoming call. If the active call exists,
	     place this active call on hold and accepts the
	     waiting call.

*/

void cm_mt_accept()
{
    SHORT call_number;
    T_MFW_CM_CALL_STAT	stat;	/* SPR#1983 - SH */
    UBYTE		ton;
    UBYTE		mode;

    TRACE_FUNCTION ("cm_mt_accept()");

    if (cm_search_callId(CAL_STAT_Active, &call_number) != CM_OK)
    {
		if (sAT_A(CMD_SRC_LCL) NEQ AT_EXCT)
		{
			TRACE_ERROR("sAT_A error");
		}
		else
		{
		    currAccepting = 1; /* ES!! 210999 */
		}
    }
    else
    {
		cmd_hold = MFW_CM_MT_ACCEPT;

		/* SPR#1983 - SH - Get status of active call */

        cm_search_callStat(call_number, &stat, &ton, &mode);
#ifdef MMI_LINT_WARNING_REMOVAL 	/*a0393213 lint warnings removal*/
		if (cm_search_callId(CAL_STAT_Wait, &call_number) EQ CM_OK)
		  cc_id = call_number;
#else
		cm_search_callId(CAL_STAT_Wait, &call_number);
#endif
		/* SPR#1983 - SH - If data call is active, end it and accept other call */

		if (mode==DATA_CALL)
		{
			/*
             * Accepting a call when there is an active data call,
             * so drop the active call and accept the waiting call by the
             * equivalent of the AT+CHLD=1 command.
             */
            cm_set_cmd_status(CHLD_MOD_RelActAndAcpt, "");
            if (sAT_PlusCHLD(CMD_SRC_LCL, CHLD_MOD_RelActAndAcpt, 0) NEQ AT_EXCT)
            {
                TRACE_ERROR("sAT_PlusCHLD(CHLD_MOD_RelActAndAcpt) error");
            }
		}

		else	/* SPR#1983 - SH - Important! Acts on next 'if' */

        /* Marcus: issue 1049: 03/09/2002: Start */
        if (cm_search_callId(CAL_STAT_Held, &call_number) EQ CM_OK)
        {
            /*
             * Accepting a call when there is already an active call and a held
             * call, so drop the active call and accept the waiting call by the
             * equivalent of the AT+CHLD=1 command.
             */
            cm_set_cmd_status(CHLD_MOD_RelActAndAcpt, "");
            if (sAT_PlusCHLD(CMD_SRC_LCL, CHLD_MOD_RelActAndAcpt, 0) NEQ AT_EXCT)
            {
                TRACE_ERROR("sAT_PlusCHLD(CHLD_MOD_RelActAndAcpt) error");
            }
        }
        else
        {
            /*
             * Accepting a call when there is already an active call but no held
             * call, so hold the active call and accept the waiting call by the
             * equivalent of the AT+CHLD=2 command.
             */
            cm_set_cmd_status(CHLD_MOD_HldActAndAcpt, "");
            if (sAT_PlusCHLD(CMD_SRC_LCL, CHLD_MOD_HldActAndAcpt, 0) NEQ AT_EXCT)
            {
                TRACE_ERROR("sAT_PlusCHLD(CHLD_MOD_HldActAndAcpt) error");
            }
        }
        /* Marcus: issue 1049: 03/09/2002: End */
	}
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)		MODULE  : MFW_CM			 |
| STATE   : code						ROUTINE : cm_end_all		 |
+--------------------------------------------------------------------+


	PURPOSE :  End all calls (Held and Active)

*/

void cm_end_all(void)
{
    T_ACI_RETURN    aci_res;        // The value returned by sAT_H

    TRACE_EVENT("cm_end_all()");

        aci_res = sAT_H(CMD_SRC_LCL);

        if (aci_res == AT_EXCT)
        {
            TRACE_EVENT("sAT_H returned AT_EXCT");
            cmd_hold = MFW_CM_DISCONNECT_ALL;
        }
        else
        {
            TRACE_EVENT_P1("sAT_H returned %d", aci_res);
        }
	return;
}



// Apr 26 2006, a0393213 (Prabakar R), OMAPS00074886 : No option in MMI to release the held call. 
// This function is added to release all held calls
    
/*******************************************************************************

 $Function:     cm_end_held

 $Description: End held calls

 $Returns: void

 $Arguments: void

*******************************************************************************/

void cm_end_held(void)
{
    T_ACI_RETURN    aci_res;        // The value returned by sAT_PlusCHLD

    TRACE_EVENT("cm_end_held()");

        aci_res = sAT_PlusCHLD(CMD_SRC_LCL,CHLD_MOD_RelHldOrUdub,NULL);

        if (aci_res == AT_FAIL)
        {
            TRACE_EVENT("sAT_PlusCHLD returned AT_FAIL");
        }
        
	return;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)		MODULE  : MFW_CM			 |
| STATE   : code						ROUTINE : cm_ok_connect		 |
+--------------------------------------------------------------------+


	PURPOSE :  The connect is successful.

*/

void cm_ok_connect(void)
{
    SHORT call_number;

    TRACE_FUNCTION ("cm_ok_connect()");

    currAccepting = 0; /* ES!! 210999 */
    currConnecting = 0; /* ES!! 290999 */

    if (cc_stat.type == REDIALING_CALL)
    {
	rdl.redial_mode = CM_REDIAL_SUCCESS;
	cm_signal(E_CM_REDIAL, &rdl);
    }

    cm_search_callId(CAL_STAT_Active, &call_number);

    /* allow a new outgoing */
    call_stat = MFW_CALL_FREE;
    call_type = MFW_CALL_FREE;
    cmd_hold = MFW_CM_NO_FLAG;

    cm_signal(E_CM_CONNECT_ACK, &call_number);
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	 MODULE  : MFW_CM	     |
| STATE   : code			 ROUTINE : cm_rmt_ok_connect |
+--------------------------------------------------------------------+


   PURPOSE :  The connect is successful.

*/

void cm_rmt_ok_connect()
{
    SHORT call_number;

    TRACE_FUNCTION ("cm_rmt_ok_connect()");

    cm_search_callId(CAL_STAT_Active, &call_number);

    cm_signal(E_CM_CONNECT_ACK, &call_number);
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	 MODULE  : MFW_CM	     |
| STATE   : code			 ROUTINE : cm_error_connect  |
+--------------------------------------------------------------------+


   PURPOSE : The connection is not successful

*/

#ifdef FF_2TO1_PS
void cm_error_connect(SHORT call_number)
{
    int i;
    T_CAUSE_ps_cause report; /* Marcus: Extended error reporting: 12/11/2002 */
    USHORT extend_report; /* API - 1564 - 16-01-03 - add this variable to store the new error code */

    TRACE_FUNCTION ("cm_error_connect()");

    currAccepting = 0; /* ES!! 210999 */
    currConnecting = 0; /* ES!! 290999 */

    /* send event */
    dis_connect.call_number = call_number;

    qAT_PlusCEER( CMD_SRC_LCL, &report);
    /* Marcus: Extended error reporting: 12/11/2002: Start */
//    TRACE_EVENT_P1("qAT_PlusCEER(%04X)", report);

	/* API - 1564 - 16-01-03 - store the error value to the new variable & throughout this function replace 'report' with 'extend_report */
    extend_report = report.value.nwcc_cause;

    if (report.ctrl_value != CAUSE_is_from_nwcc)
    {
        /* Treat Condat defined cause values as if no cause given */
        dis_connect.cause = NOT_PRESENT_8BIT;
    }
    else
    {
        /* Standards defined cause value is given */
        dis_connect.cause = report.value.nwcc_cause;
    }
    TRACE_EVENT_P1("dis_connect.cause = %02X)", dis_connect.cause);
    /* Marcus: Extended error reporting: 12/11/2002: End */
    cm_signal(E_CM_DISCONNECT, &dis_connect);
    TRACE_EVENT("passed the E_CM_DISCONNECT point");

    if ((call_stat EQ OUTGOING_CALL)
	AND (call_type NEQ MFW_ECC_CALL)
	AND ((redial_stat EQ CM_REDIAL_AUTO) OR (redial_stat EQ CM_REDIAL_MANU)))
    {
	/* If this call is 2nd outgoing call, redial is not started */
	if (qAT_PercentCAL(CMD_SRC_LCL, call_tab) EQ AT_CMPL)
	{
	    for (i=0; i<MAX_CALL_NR; i++)
	    {
		if (call_tab[i].index == -1)
		    break;

		if ((call_tab[i].status == CAL_STAT_NotPresent)
		    OR (call_tab[i].status == CAL_STAT_DeactiveReq))
		    continue;

		if (call_tab[i].calType == CAL_TYPE_MOC)
		{
		    call_stat = MFW_CALL_FREE;
		    call_type = MFW_CALL_FREE;
		    cmd_hold = MFW_CM_NO_FLAG;
		    return;
		}
	    }
	}

	/* check blacklist whether it is full */
	if (black_list.count == MAX_BLACKLIST_NUM)
	{
			TRACE_EVENT ("blacklist is now full");

			/* Blacklist is now full and
			   MT shall prohibit further automatic call attempts
			*/
		redial_stat = CM_REDIAL_OFF;


	    /* allow a new outgoing */
	    call_stat	 = MFW_CALL_FREE;
	    call_type	 = MFW_CALL_FREE;
	    cc_stat.type = MFW_CALL_FREE;

	    rdl.redial_mode = CM_REDIAL_BLACKLIST;
	    cm_signal(E_CM_REDIAL, &rdl);
	    return;
	}

	/* check the registration staus */
	if (nm_reg_status() == MFW_NO_SERVICE)
	{
	    call_stat	 = MFW_CALL_FREE;
	    call_type	 = MFW_CALL_FREE;
	    cc_stat.type = MFW_CALL_FREE;

	    rdl.redial_mode = CM_REDIAL_NO_SERVICE;
	    cm_signal(E_CM_REDIAL, &rdl);
	    return;
	}

	TRACE_EVENT_P1("report = %d", extend_report);
	switch (extend_report)
	{
		case 28:
        // API - 1480 - Added this code to stop redial on an incorrect number
        if(!redial_count)
        {
			call_stat    = MFW_CALL_FREE;
			call_type    = MFW_CALL_FREE;
			cc_stat.type = MFW_CALL_FREE;

			rdl.redial_mode = CM_REDIAL_ABORTED;
			cm_signal(E_CM_REDIAL, &rdl);
			return;
		}
	    case 1: /* check whether the reason is in category 3 (GSM 02.07) */
	    case 3:
	    case 22:
	    case 38:
		TRACE_EVENT ("category 3");
		if (!redial_count)
		{
			/* redial_count == 0;
			   this is for the first outgoing call and
			   now it is going to start the Auto-redial procedure
			 */
		    cc_stat.type = REDIALING_CALL;
		    cc_stat.call_number = 0;

		    call_stat = REDIALING_CALL;
		    rdl.redial_mode = CM_REDIAL_STARTED;
		    mfwStrncpy((char *)rdl.number, (char *)cc_stat.number, MFW_NUM_LEN);

#ifdef NO_ASCIIZ
		    rdl.name.len = cc_stat.name.len;
		    memcpy(rdl.name.data,
			   cc_stat.name.data,
			   cc_stat.name.len);
#else
		    mfwStrncpy((char *)rdl.name, (char *)cc_stat.name, MFW_TAG_LEN);
//		      strcpy((char *)rdl.subaddr, (char *)cc_stat.subaddr);
#endif
		    rdl.ton = cc_stat.ton;
		    rdl.type = cc_stat.mode;

		    /* reasons in the category 3 have
		       a call repeat attempts of max. 1 !
		     */
		    redial_count = MAX_REDIAL_ATTEMPT-1;
		    cm_signal(E_CM_REDIAL, &rdl);
		    return;
		}
		else
		    redial_count = MAX_REDIAL_ATTEMPT;
		break;

	    case 17: /* check whether the reason is in category 1 and 2 (GSM 02.07) */
	    case 18:
	    case 19:
	    case 27:
	    case 34:
	    case 41:
	    case 42:
	    case 44:
	    case 47:
		TRACE_EVENT ("category 1 & 2");
		if (!redial_count)
		{
			/* redial_count == 0
			   this is for the first outgoing call and
			   now it is going to start the Auto-redial procedure
			 */
		    cc_stat.type = REDIALING_CALL;
		    cc_stat.call_number = 0;

		    call_stat = REDIALING_CALL;
		    rdl.redial_mode = CM_REDIAL_STARTED;
		    mfwStrncpy((char *)rdl.number, (char *)cc_stat.number, MFW_NUM_LEN);
#ifdef NO_ASCIIZ
		    rdl.name.len = cc_stat.name.len;
		    memcpy(rdl.name.data,
			   cc_stat.name.data,
			   cc_stat.name.len);
#else
		    mfwStrncpy((char *)rdl.name, (char *)cc_stat.name, MFW_TAG_LEN);
//		      strcpy((char *)rdl.subaddr, (char *)cc_stat.subaddr);
#endif
		    rdl.ton = cc_stat.ton;
		    rdl.type = cc_stat.mode;

		    /* reasons in the category 1 and 2 have
		       a call repeat attempts of max. 10 !
		     */

		    cm_signal(E_CM_REDIAL, &rdl);
		    return;
		}

		if (redial_count >= 1 AND redial_count <= 3)
		{
		    timStart(redialTim24);
		    call_stat = REDIALING_CALL;
		    cc_stat.call_number = 0;
		    return;
		}

		if (redial_count >= 4 AND redial_count < MAX_REDIAL_ATTEMPT)
		{
		    timStart(redialTim5);
		    call_stat = REDIALING_CALL;
		    cc_stat.call_number = 0;
		    return;
		}
#ifdef CUSTOMER_6366
				// JP PATCH The MMI needs to know if the 'error disconnect' has
				// resulted in redial starting irrespective of whether this was a redial attempt or not
				rdl.redial_mode = CM_REDIAL_STARTED;
				cm_signal(E_CM_REDIAL, &rdl);
				// END JP PATCH
#endif /* CUSTOMER_6366 */
		break;

	    default:
		if (redial_count)
		{
		  rdl.redial_mode = CM_REDIAL_STOP;
		  mfwStrncpy((char *)rdl.number, (char *)cc_stat.number, MFW_NUM_LEN);
#ifdef NO_ASCIIZ
		  rdl.name.len = cc_stat.name.len;
		  memcpy(rdl.name.data,
			 cc_stat.name.data,
			 cc_stat.name.len);
#else
		  mfwStrncpy((char *)rdl.name, (char *)cc_stat.name, MFW_TAG_LEN);//???
//		    strcpy((char *)rdl.subaddr, (char *)cc_stat.subaddr);
#endif
		  rdl.ton = cc_stat.ton;
		  rdl.type = cc_stat.mode;
		  cm_signal(E_CM_REDIAL, &rdl);
		}
		/* allow a new outgoing */
		call_stat    = MFW_CALL_FREE;
		call_type    = MFW_CALL_FREE;
		cc_stat.type = MFW_CALL_FREE;
		return;
	}

	if (redial_count >= MAX_REDIAL_ATTEMPT
	    AND black_list.count < MAX_BLACKLIST_NUM)
	{
		  mfwStrncpy((char *)black_list.number[black_list.count],
		   (char *)cc_stat.number, MFW_NUM_LEN);
	    black_list.count++;

	    rdl.redial_mode   = CM_REDIAL_BLACKLISTED;
	    mfwStrncpy((char *)rdl.number, (char *)cc_stat.number, MFW_NUM_LEN);
#ifdef NO_ASCIIZ
	    rdl.name.len = cc_stat.name.len;
	    memcpy(rdl.name.data,
		   cc_stat.name.data,
		   cc_stat.name.len);
#else
	    mfwStrncpy((char *)rdl.name, (char *)cc_stat.name, MFW_TAG_LEN);
//		strcpy((char *)rdl.subaddr, (char *)cc_stat.subaddr);
#endif
	    rdl.ton = cc_stat.ton;
	    rdl.type = cc_stat.mode;
	    rdl.left_attempts	= 0;
	    cc_stat.call_number = 0;
	    cm_signal(E_CM_REDIAL, &rdl);
	}
    }

    /* allow a new outgoing */
    call_stat	 = MFW_CALL_FREE;
    call_type	 = MFW_CALL_FREE;
    cc_stat.type = MFW_CALL_FREE;
}
#else
void cm_error_connect(SHORT call_number)
{

    int i;
    USHORT report;  /* Marcus: Extended error reporting: 12/11/2002 */
    USHORT extend_report; /* API - 1564 - 16-01-03 - add this variable to store the new error code */
// 	Apr 06 2005		REF: CRR 29989	xpradipg	
//	local definition
#ifdef FF_MMI_OPTIM
	T_ACI_CAL_ENTR 	    *call_tab;//[MAX_CALL_NR];
#endif	
//	Apr 14, 2005	REF: CRR 29991   xpradipg
//	local definition
#ifdef FF_MMI_OPTIM
	UBYTE count=0;
	UBYTE blackListNum[MFW_NUM_LEN];
	int32 file = 0;
#endif	
    TRACE_FUNCTION ("cm_error_connect()");

    currAccepting = 0; /* ES!! 210999 */
    currConnecting = 0; /* ES!! 290999 */

    /* send event */
    dis_connect.call_number = call_number;

    qAT_PlusCEER( CMD_SRC_LCL, &report);
    /* Marcus: Extended error reporting: 12/11/2002: Start */
    TRACE_EVENT_P1("qAT_PlusCEER(%04X)", report);

	/* API - 1564 - 16-01-03 - store the error value to the new variable & throughout this function replace 'report' with 'extend_report */
    extend_report = GET_CAUSE_VALUE(report);

    if (GET_CAUSE_DEFBY(extend_report) == DEFBY_CONDAT)
    {
        /* Treat Condat defined cause values as if no cause given */
        dis_connect.cause = NOT_PRESENT_8BIT;
    }
    else
    {
        /* Standards defined cause value is given */
        dis_connect.cause = GET_CAUSE_VALUE(extend_report);
    }
    TRACE_EVENT_P1("dis_connect.cause = %02X)", dis_connect.cause);
    /* Marcus: Extended error reporting: 12/11/2002: End */
    cm_signal(E_CM_DISCONNECT, &dis_connect);
    TRACE_EVENT("passed the E_CM_DISCONNECT point");

    if ((call_stat EQ OUTGOING_CALL)
	AND (call_type NEQ MFW_ECC_CALL)
	AND ((redial_stat EQ CM_REDIAL_AUTO) OR (redial_stat EQ CM_REDIAL_MANU)))
    {
// 	Apr 06 2005		REF: CRR 29989	xpradipg	
//	allocate memory
#ifdef FF_MMI_OPTIM
		call_tab = (T_ACI_CAL_ENTR*)mfwAlloc(sizeof(T_ACI_CAL_ENTR)*MAX_CALL_NR);
		if( NULL == call_tab)
			return; 
#endif		
	/* If this call is 2nd outgoing call, redial is not started */
	if (qAT_PercentCAL(CMD_SRC_LCL, call_tab) EQ AT_CMPL)
	{
	    for (i=0; i<MAX_CALL_NR; i++)
	    {
		if (call_tab[i].index == -1)
		    break;

		if ((call_tab[i].status == CAL_STAT_NotPresent)
		    OR (call_tab[i].status == CAL_STAT_DeactiveReq))
		    continue;

		if (call_tab[i].calType == CAL_TYPE_MOC)
		{
		    call_stat = MFW_CALL_FREE;
		    call_type = MFW_CALL_FREE;
		    cmd_hold = MFW_CM_NO_FLAG;
// 	Apr 06 2005		REF: CRR 29989	xpradipg	
//	deallocate memory
#ifdef FF_MMI_OPTIM
			    if(call_tab)
			    mfwFree((U8*)call_tab, sizeof(T_ACI_CAL_ENTR)*MAX_CALL_NR);
#endif			    
			    return;
		}
	    }
	}
// 	Apr 06 2005		REF: CRR 29989	xpradipg	
//	deallocate memory
#ifdef FF_MMI_OPTIM
	    if(call_tab)
	    mfwFree((U8*)call_tab, sizeof(T_ACI_CAL_ENTR)*MAX_CALL_NR);
#endif
//	Apr 14, 2005	REF: CRR 29991   xpradipg
//	read the list into the variable
#ifdef FF_MMI_OPTIM
	if((file = flash_MMI_blackList_open()) >= 0 )
	{	
		if(flash_MMI_blackList_read(file,(U8*)&count, 1,0) > 0)
		{
			if(count == MAX_BLACKLIST_NUM)		
#else
	/* check blacklist whether it is full */
	if (black_list.count == MAX_BLACKLIST_NUM)
#endif	
	{
			TRACE_EVENT ("blacklist is now full");

			/* Blacklist is now full and
			   MT shall prohibit further automatic call attempts
			*/
		redial_stat = CM_REDIAL_ABORTED;


	    /* allow a new outgoing */
	    call_stat	 = MFW_CALL_FREE;
	    call_type	 = MFW_CALL_FREE;
	    cc_stat.type = MFW_CALL_FREE;

	    rdl.redial_mode = CM_REDIAL_BLACKLIST;
	    cm_signal(E_CM_REDIAL, &rdl);
#ifdef FF_MMI_OPTIM
		flash_MMI_blackList_close(file);
#endif
    	return;
	}
#ifdef FF_MMI_OPTIM
		}
		flash_MMI_blackList_close(file);
	}
#endif	

	/* check the registration staus */
	if (nm_reg_status() == MFW_NO_SERVICE)
	{
	    call_stat	 = MFW_CALL_FREE;
	    call_type	 = MFW_CALL_FREE;
	    cc_stat.type = MFW_CALL_FREE;

	    rdl.redial_mode = CM_REDIAL_NO_SERVICE;
	    cm_signal(E_CM_REDIAL, &rdl);
	    return;
	}

	TRACE_EVENT_P1("report = %d", extend_report);
	switch (extend_report)
	{
		case 28:
        // API - 1480 - Added this code to stop redial on an incorrect number
        if(!redial_count)
        {
			call_stat    = MFW_CALL_FREE;
			call_type    = MFW_CALL_FREE;
			cc_stat.type = MFW_CALL_FREE;

			rdl.redial_mode = CM_REDIAL_ABORTED;
			cm_signal(E_CM_REDIAL, &rdl);
			return;
		}
	    case 1: /* check whether the reason is in category 3 (GSM 02.07) */
	    case 3:
	    case 22:
	    case 38:
		TRACE_EVENT ("category 3");
		if (!redial_count)
		{
			/* redial_count == 0;
			   this is for the first outgoing call and
			   now it is going to start the Auto-redial procedure
			 */
		    cc_stat.type = REDIALING_CALL;
		    cc_stat.call_number = 0;

		    call_stat = REDIALING_CALL;
		    rdl.redial_mode = CM_REDIAL_STARTED;
		    mfwStrncpy((char *)rdl.number, (char *)cc_stat.number, MFW_NUM_LEN);

#ifdef NO_ASCIIZ
		    rdl.name.len = cc_stat.name.len;
		    memcpy(rdl.name.data,
			   cc_stat.name.data,
			   cc_stat.name.len);
#else
		    mfwStrncpy((char *)rdl.name, (char *)cc_stat.name, MFW_TAG_LEN);
//		      strcpy((char *)rdl.subaddr, (char *)cc_stat.subaddr);
#endif
		    rdl.ton = cc_stat.ton;
		    rdl.type = cc_stat.mode;

		    /* reasons in the category 3 have
		       a call repeat attempts of max. 1 !
		     */
		    redial_count = MAX_REDIAL_ATTEMPT-1;
		    cm_signal(E_CM_REDIAL, &rdl);
		    return;
		}
		else
		    redial_count = MAX_REDIAL_ATTEMPT;
		break;

	    case 17: /* check whether the reason is in category 1 and 2 (GSM 02.07) */
	    case 18:
	    case 19:
	    case 27:
	    case 34:
	    case 41:
	    case 42:
	    case 44:
	    case 47:
		TRACE_EVENT ("category 1 & 2");
		if (!redial_count)
		{
			/* redial_count == 0
			   this is for the first outgoing call and
			   now it is going to start the Auto-redial procedure
			 */
		    cc_stat.type = REDIALING_CALL;
		    cc_stat.call_number = 0;

		    call_stat = REDIALING_CALL;
		    rdl.redial_mode = CM_REDIAL_STARTED;
		    mfwStrncpy((char *)rdl.number, (char *)cc_stat.number, MFW_NUM_LEN);
#ifdef NO_ASCIIZ
		    rdl.name.len = cc_stat.name.len;
		    memcpy(rdl.name.data,
			   cc_stat.name.data,
			   cc_stat.name.len);
#else
		    mfwStrncpy((char *)rdl.name, (char *)cc_stat.name, MFW_TAG_LEN);
//		      strcpy((char *)rdl.subaddr, (char *)cc_stat.subaddr);
#endif
		    rdl.ton = cc_stat.ton;
		    rdl.type = cc_stat.mode;

		    /* reasons in the category 1 and 2 have
		       a call repeat attempts of max. 10 !
		     */

		    cm_signal(E_CM_REDIAL, &rdl);
		    return;
		}

		if (redial_count >= 1 AND redial_count <= 3)
		{
		    timStart(redialTim24);
		    call_stat = REDIALING_CALL;
		    cc_stat.call_number = 0;
		    return;
		}

		if (redial_count >= 4 AND redial_count < MAX_REDIAL_ATTEMPT)
		{
		    timStart(redialTim5);
		    call_stat = REDIALING_CALL;
		    cc_stat.call_number = 0;
		    return;
		}
#ifdef CUSTOMER_6366
				// JP PATCH The MMI needs to know if the 'error disconnect' has
				// resulted in redial starting irrespective of whether this was a redial attempt or not
				rdl.redial_mode = CM_REDIAL_STARTED;
				cm_signal(E_CM_REDIAL, &rdl);
				// END JP PATCH
#endif /* CUSTOMER_6366 */
		break;

	    default:
		if (redial_count)
		{
		  rdl.redial_mode = CM_REDIAL_STOP;
		  mfwStrncpy((char *)rdl.number, (char *)cc_stat.number, MFW_NUM_LEN);
#ifdef NO_ASCIIZ
		  rdl.name.len = cc_stat.name.len;
		  memcpy(rdl.name.data,
			 cc_stat.name.data,
			 cc_stat.name.len);
#else
		  mfwStrncpy((char *)rdl.name, (char *)cc_stat.name, MFW_TAG_LEN);//???
//		    strcpy((char *)rdl.subaddr, (char *)cc_stat.subaddr);
#endif
		  rdl.ton = cc_stat.ton;
		  rdl.type = cc_stat.mode;
		  cm_signal(E_CM_REDIAL, &rdl);
		}
		/* allow a new outgoing */
		call_stat    = MFW_CALL_FREE;
		call_type    = MFW_CALL_FREE;
		cc_stat.type = MFW_CALL_FREE;
		return;
	}
#ifdef FF_MMI_OPTIM
	if (redial_count >= MAX_REDIAL_ATTEMPT
	    AND count < MAX_BLACKLIST_NUM)
	{
		  mfwStrncpy((char *)blackListNum,(char *)cc_stat.number, MFW_NUM_LEN);
//	Apr 14, 2005	REF: CRR 29991   xpradipg
//	once there is new addition update it onto the file
		if(flash_MMI_blackList_write((U8*)blackListNum, MFW_NUM_LEN, (count*MFW_NUM_LEN)+1) < 0)
			TRACE_FUNCTION("ERROR OPENING the blacklist file");
		count++;
		if(flash_MMI_blackList_write((U8*)&count,1,0) < 0)
			TRACE_FUNCTION("ERROR OPENING the blacklist file");
#else
	if (redial_count >= MAX_REDIAL_ATTEMPT
	    AND black_list.count < MAX_BLACKLIST_NUM)
	{
		  mfwStrncpy((char *)black_list.number[black_list.count],
		   (char *)cc_stat.number, MFW_NUM_LEN);
	    black_list.count++;
#endif
		rdl.redial_mode   = CM_REDIAL_BLACKLISTED;
	    mfwStrncpy((char *)rdl.number, (char *)cc_stat.number, MFW_NUM_LEN);
#ifdef NO_ASCIIZ
	    rdl.name.len = cc_stat.name.len;
	    memcpy(rdl.name.data,
		   cc_stat.name.data,
		   cc_stat.name.len);
#else
	    mfwStrncpy((char *)rdl.name, (char *)cc_stat.name, MFW_TAG_LEN);
//		strcpy((char *)rdl.subaddr, (char *)cc_stat.subaddr);
#endif
	    rdl.ton = cc_stat.ton;
	    rdl.type = cc_stat.mode;
	    rdl.left_attempts	= 0;
	    cc_stat.call_number = 0;
	    cm_signal(E_CM_REDIAL, &rdl);
	}
    }

    /* allow a new outgoing */
    call_stat	 = MFW_CALL_FREE;
    call_type	 = MFW_CALL_FREE;
    cc_stat.type = MFW_CALL_FREE;
}
#endif


/*
+-----------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	 MODULE  : MFW_CM		|
| STATE   : code			 ROUTINE : cm_rmt_error_connect |
+-----------------------------------------------------------------------+


   PURPOSE : The connection is not successful

*/

#ifdef FF_2TO1_PS
void cm_rmt_error_connect(SHORT call_number)
{
    T_CAUSE_ps_cause report;  /* Marcus: Extended error reporting: 12/11/2002 */

    TRACE_FUNCTION ("cm_error_connect()");

//TISH: OMAPS00104986 if there is inband tone, it should not disconnect the call, until User reject it.
 if (cm_cpi.inband) return;
    /* send event */
    dis_connect.call_number = call_number;

    qAT_PlusCEER( CMD_SRC_LCL, &report);
    /* Marcus: Extended error reporting: 12/11/2002: Start */
//    TRACE_EVENT_P1("qAT_PlusCEER(%04X)", report);

    if (report.ctrl_value != CAUSE_is_from_nwcc)
    {
        /* Treat Condat defined cause values as if no cause given */
        dis_connect.cause = NOT_PRESENT_8BIT;
    }
    else
    {
       /* Standards defined cause value is given */
       dis_connect.cause = report.value.nwcc_cause;
    }

    TRACE_EVENT_P1("dis_connect.cause = %02X)", dis_connect.cause);
    /* Marcus: Extended error reporting: 12/11/2002: End */

    cm_signal(E_CM_DISCONNECT, &dis_connect);
}
#else
void cm_rmt_error_connect(SHORT call_number)
{
    USHORT report;  /* Marcus: Extended error reporting: 12/11/2002 */

    TRACE_FUNCTION ("cm_error_connect()");
    //TISH: OMAPS00104986 if there is inband tone, it should not disconnect the call, until User reject it.
    if (cm_cpi.inband) return;

    /* send event */
    dis_connect.call_number = call_number;

    qAT_PlusCEER( CMD_SRC_LCL, &report);
    /* Marcus: Extended error reporting: 12/11/2002: Start */
    TRACE_EVENT_P1("qAT_PlusCEER(%04X)", report);
    if (GET_CAUSE_DEFBY(report) == DEFBY_CONDAT)
    {
        /* Treat Condat defined cause values as if no cause given */
        dis_connect.cause = NOT_PRESENT_8BIT;
    }
    else
    {
        /* Standards defined cause value is given */
        dis_connect.cause = GET_CAUSE_VALUE(report);
    }
    TRACE_EVENT_P1("dis_connect.cause = %02X)", dis_connect.cause);
    /* Marcus: Extended error reporting: 12/11/2002: End */

    cm_signal(E_CM_DISCONNECT, &dis_connect);
}
#endif


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_disconnect	     |
+--------------------------------------------------------------------+


   PURPOSE : Disconnection for outgoing call or active call

*/

T_MFW cm_disconnect(SHORT call_number)
{
    CHAR		callId[5];
    T_MFW_CM_CALL_STAT	stat;
    UBYTE		ton;
    UBYTE		mode;

    TRACE_FUNCTION ("cm_disconnect()");

    dis_connect.call_number = call_number;

	/*Apr 25, 2006 OMAPS00075711 a0393213 (R.Prabakar)
      Description : 'End Conference' option doesn't work
      Solution    : The ACI_CHLD_MOD is changed from CHLD_MOD_RelActSpec to CHLD_MOD_RelActAndAcpt while calling sAT_PlusCHLD().
                    This is done since CHLD_MOD_RelActSpec with callId 0 is equivalent to sending AT+CHLD=10 which is invalid
                    according to GSM specification. Before calling sAT_PlusCHLD command status is also set*/
    
    if (!call_number)
    {
    sprintf(callId, "%d", 0);
	cm_set_cmd_status(CHLD_MOD_RelActAndAcpt, callId);
	if (sAT_PlusCHLD(CMD_SRC_LCL, CHLD_MOD_RelActAndAcpt, callId) != AT_EXCT)  
	    return CM_ERROR;
	return CM_OK;
    }

    cm_search_callStat(call_number, &stat, &ton, &mode);

	TRACE_EVENT_P3("Status %d Ton %d Mode %d",stat,ton,mode);

	/* HANG UP DATA CALL (WAP)
	 * SPR#1983 - SH - Use sAT_Abort() when call has not yet connected,
	 * rather than sAT_H */

    if (mode == DATA_CALL)
	{
		T_ACI_RETURN retval;

		TRACE_EVENT_P1("***Disconnect data call, stat=%d", stat);

		if ((stat == CALL_DIAL) OR (stat == CALL_ALERT))
	    {
			if (sAT_Abort(CMD_SRC_LCL, AT_CMD_D) NEQ AT_CMPL)
				return CM_ERROR;
			return CM_OK;
	    }

	    if (stat == CALL_DEACTIVE && currConnecting)  /* ES/AK 290999 */
	    {
			currConnecting = 0;
			if (sAT_Abort(CMD_SRC_LCL, AT_CMD_D) NEQ AT_CMPL)
				return CM_ERROR;
			return CM_OK;
	    }

		retval = sAT_H(CMD_SRC_LCL);

		TRACE_EVENT_P1("Result of sAT_H = %d", retval);
		if (retval)
		    return CM_ERROR;
		return CM_OK;
	}


    if ((stat == CALL_DIAL) OR (stat == CALL_ALERT))
    {
	if (sAT_Abort(CMD_SRC_LCL, AT_CMD_D) NEQ AT_CMPL)
		return CM_ERROR;
	return CM_OK;
    }

    if (stat == CALL_DEACTIVE && currConnecting)  /* ES/AK 290999 */
    {
	currConnecting = 0;
	if (sAT_Abort(CMD_SRC_LCL, AT_CMD_D) NEQ AT_CMPL)
		return CM_ERROR;
	return CM_OK;
    }

    if ((stat == CALL_ACTIVE) OR (stat == CALL_DEACTIVE))
    {
		cmd_hold = MFW_CM_TERMINATED;
		sprintf(callId, "%d", call_number);
		if (sAT_PlusCHLD(CMD_SRC_LCL, CHLD_MOD_RelActSpec, callId) != AT_EXCT)
			return CM_ERROR;
		return CM_OK;
    }
    else if (stat == CALL_HELD) 
   {
   

                //  Apr 26, 2004 REF: CRR 16545 Ajith K P Side effect of CRR 16545.
                //  CHLD_MOD_RelActSpec no longer releases held calls.
                //  So now if the user wants to release a HELD call, now we are calling the 
                //  sAT_PlusCHLD(..) function with CHLD_MOD_RelAnySpec as the parameter.



		cmd_hold = MFW_CM_TERMINATED;
		sprintf(callId, "%d", call_number);
               // Feb 27, 2006    REF:DR OMAPS00067884  x0039928	 
               // Fix :  sAT_PercentCHLD() is called if the mode > CHLD_MOD_RelDialCall
		if (sAT_PercentCHLD(CMD_SRC_LCL, CHLD_MOD_RelAnySpec, callId) != AT_EXCT)
		    return CM_ERROR;
		return CM_OK;
   	}
	return CM_ERROR;
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_force_disconnect |
+--------------------------------------------------------------------+


   PURPOSE : Disconnection of any call existent needed for Multiparty and some structured procedures
	     call termination in state U9

*/

void cm_force_disconnect(void)
{
  T_ACI_RETURN res;

  TRACE_FUNCTION("cm_force_disconnect()");

  res = sAT_H(CMD_SRC_LCL);
  if (res != AT_EXCT && res != AT_CMPL)
      TRACE_ERROR("satReject() error");

  return;
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_ok_disconnect    |
+--------------------------------------------------------------------+


   PURPOSE : indicates disconnection from called party;
		sends E_CM_DISCONNECT to mmi

*/

#ifdef FF_2TO1_PS
void cm_ok_disconnect(T_ACI_CMD_SRC src, SHORT call_id)
{
    T_CAUSE_ps_cause cause;  /* Marcus: Extended error reporting: 12/11/2002 */
    T_MFW_CM_DISCONNECT_ALL disconnect_all;

    TRACE_FUNCTION("cm_ok_disconnect()");

    TRACE_EVENT_P4("dis_connect.call_number %d, cmd_hold %d, src %d, call_id %d",
                              dis_connect.call_number, cmd_hold, src, call_id);




    /* Marcus: CCBS: 14/11/2002: Start */
    if (cmd_hold == MFW_CM_CCBS_REGISTER)
    {
        /* Rejecting the offered CCBS registration */
        TRACE_EVENT("cmd_hold == MFW_CM_CCBS_REGISTER");
        cmd_hold = 0;
        cm_ccbs_ok();
        return;
    }
    /* Marcus: CCBS: 14/11/2002: End */

    if (cmd_hold == MFW_CM_MPTY_TERMINATED)
	       dis_connect.call_number = 0;
	//Nov 02, 2005    DR OMAPS00052132   xdeepadh	
	//The usage of ATI enum is put under the flag FF_ATI
#if  defined (FF_ATI) || defined (NEPTUNE_BOARD)
        else if (src EQ CMD_SRC_ATI_1) /* NM Berlin/UK integration 21.09.01 */
        {
               dis_connect.call_number = call_id;
        }
#endif 
       

    /* send event */
    qAT_PlusCEER( CMD_SRC_LCL, &cause);
    /* Marcus: Extended error reporting: 12/11/2002: Start */
//    TRACE_EVENT_P1("qAT_PlusCEER(%04X)", cause);

    if (cause.ctrl_value != CAUSE_is_from_nwcc)
    {
        /* Treat Condat defined cause values as if no cause given */
        dis_connect.cause = NOT_PRESENT_8BIT;
    }
    else
    {
        /* Standards defined cause value is given */
        dis_connect.cause = cause.value.nwcc_cause;
    }

    TRACE_EVENT_P1("dis_connect.cause = %02X)", dis_connect.cause);
    /* Marcus: Extended error reporting: 12/11/2002: End */

    /* allow a new outgoing */
    call_stat = MFW_CALL_FREE;
    call_type = MFW_CALL_FREE;

    if (cmd_hold EQ MFW_CM_DISCONNECT_ALL)
    {
        disconnect_all.result = CM_DISCONNECT_ALL_RESULT_OK;
        if (cause.ctrl_value != CAUSE_is_from_nwcc)
        {
            /* Treat Condat defined cause values as if no cause given */
            disconnect_all.cause = NOT_PRESENT_8BIT;
        }
        else
        {
            /* Standards defined cause value is given */
            disconnect_all.cause = cause.value.nwcc_cause;
        }
        cm_signal(E_CM_DISCONNECT_ALL,&disconnect_all);
    }
    else
        cm_signal(E_CM_DISCONNECT, &dis_connect);

    cmd_hold = 0;

}
#else
void cm_ok_disconnect(T_ACI_CMD_SRC src, SHORT call_id)
{
    USHORT cause;  /* Marcus: Extended error reporting: 12/11/2002 */
    T_MFW_CM_DISCONNECT_ALL disconnect_all;

    TRACE_FUNCTION("cm_ok_disconnect()");

    TRACE_EVENT_P4("dis_connect.call_number %d, cmd_hold %d, src %d, call_id %d",
                              dis_connect.call_number, cmd_hold, src, call_id);




    /* Marcus: CCBS: 14/11/2002: Start */
    if (cmd_hold == MFW_CM_CCBS_REGISTER)
    {
        /* Rejecting the offered CCBS registration */
        TRACE_EVENT("cmd_hold == MFW_CM_CCBS_REGISTER");
        cmd_hold = 0;
        cm_ccbs_ok();
        return;
    }
    /* Marcus: CCBS: 14/11/2002: End */

    if (cmd_hold == MFW_CM_MPTY_TERMINATED)
	       dis_connect.call_number = 0;
	//Nov 02, 2005    DR OMAPS00052132   xdeepadh	
	//The usage of ATI enum is put under the flag FF_ATI
#ifdef FF_ATI	
	else if (src EQ CMD_SRC_ATI_1) //NM Berlin/UK integration 21.09.01
	{
		dis_connect.call_number = call_id;
	}
#endif	//FF_ATI  

    /* send event */
    qAT_PlusCEER( CMD_SRC_LCL, &cause);
    /* Marcus: Extended error reporting: 12/11/2002: Start */
    TRACE_EVENT_P1("qAT_PlusCEER(%04X)", cause);
    if (GET_CAUSE_DEFBY(cause) == DEFBY_CONDAT)
    {
        /* Treat Condat defined cause values as if no cause given */
        dis_connect.cause = NOT_PRESENT_8BIT;
    }
    else
    {
        /* Standards defined cause value is given */
        dis_connect.cause = GET_CAUSE_VALUE(cause);
    }
    TRACE_EVENT_P1("dis_connect.cause = %02X)", dis_connect.cause);
    /* Marcus: Extended error reporting: 12/11/2002: End */

    /* allow a new outgoing */
    call_stat = MFW_CALL_FREE;
    call_type = MFW_CALL_FREE;

    if (cmd_hold EQ MFW_CM_DISCONNECT_ALL)
    {
        disconnect_all.result = CM_DISCONNECT_ALL_RESULT_OK;
        if (GET_CAUSE_DEFBY(cause) == DEFBY_CONDAT)
        {
            /* Treat Condat defined cause values as if no cause given */
            disconnect_all.cause = NOT_PRESENT_8BIT;
        }
        else
        {
            /* Standards defined cause value is given */
            disconnect_all.cause = GET_CAUSE_VALUE(cause);
        }
        cm_signal(E_CM_DISCONNECT_ALL,&disconnect_all);
    }
    else
        cm_signal(E_CM_DISCONNECT, &dis_connect);

    cmd_hold = 0;

}
#endif

/* Marcus: CCBS: 14/11/2002: Start */
/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM	        	     |
| STATE  : code 			ROUTINE: cm_error_disconnect             |
+--------------------------------------------------------------------+

   PURPOSE : indicates failed disconnection

*/
void cm_error_disconnect(void)
{
    TRACE_FUNCTION ("cm_error_disconnect()");
    if (cmd_hold == MFW_CM_DISCONNECT_ALL)
    {
        TRACE_EVENT("cmd_hold == MFW_CM_DISCONNECT_ALL");
        cmd_hold = 0;
    }

    /* Marcus: CCBS: 14/11/2002: Start */
    if (cmd_hold == MFW_CM_CCBS_REGISTER)
    {
        TRACE_EVENT("cmd_hold == MFW_CM_CCBS_REGISTER");
        /* Rejecting the offered CCBS registration */
        cmd_hold = 0;
        cm_ccbs_error();
    }
}
/* Marcus: CCBS: 14/11/2002: End */

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_command	     |
+--------------------------------------------------------------------+


   PURPOSE :

*/

T_MFW cm_command(SHORT call_number, UBYTE command, UBYTE crss_string)
{
  CHAR		      callId[5];
  UBYTE 	      res;
  SHORT retVal, modeRetVal; // xnkulkar SPR-18262: Added "modeRetVal"  to store the return value of "mfw_cbuf_put_mode ()"

  TRACE_FUNCTION ("cm_command()");

// JP PATCH
	cmd_hold = 0;
// END PATCH

  cmd.call_number = call_number;
  cmd.command	  = command;

  switch (command)
  {
    case CM_HOLD:
    case CM_SWAP:
    case CM_RETRIEVE:
    case CM_SWAP_MULTIPARTY:
    case CM_HOLD_MULTIPARTY:
    case CM_RETRIEVE_MULTIPARTY:
      /* place all active calls on held and accept held or waiting call */
      if (sAT_PlusCHLD(CMD_SRC_LCL, CHLD_MOD_HldActAndAcpt, 0) != AT_EXCT)
	return FALSE;
      break;

    case CM_BUILD_MULTIPARTY:
      if (sAT_PlusCHLD(CMD_SRC_LCL, CHLD_MOD_AddHld, 0)!= AT_EXCT)
	return FALSE;
      break;

    case CM_SPLIT_MULTIPARTY:
      if (call_number <= 0)
	return FALSE;
      sprintf(callId, "%d", call_number);
      if (sAT_PlusCHLD(CMD_SRC_LCL, CHLD_MOD_HldActExc, callId)!= AT_EXCT)
	return FALSE;
      break;

    case CM_ETC:
      /* connects two calls and disconnects the subsrciber from both calls */
      if (sAT_PlusCHLD(CMD_SRC_LCL, CHLD_MOD_Ect, 0) != AT_EXCT)
	return FALSE;
      break;

    case CM_CALL_DEFLECTION:
      break;

    case CM_CCBS:
        TRACE_EVENT("CM_CCBS");
      sAT_PlusCHLD(CMD_SRC_LCL, CHLD_MOD_Ccbs, 0);
      break;

    case CM_MODIFY:
      break;

    case CM_DTMF_ACTIVE:
	// xnkulkar SPR-18262
	// If there is a DTMF tone command already in the queue, add the current tone also
	// in the queue (with start mode). Also, start adding to the queue if we are expecting an acknowledgement 
	// (rAT_OK) for the previous DTMF tone request sent.
	/*OMAPS00108192 a0393213 DTMF START-DTMF STOP properly nested*/
	if (mfw_cbuf_num_elements(mfw_cm_dtmf_q_id) != 0)
    	{
		/*dtmf queue not empty*/
		if(dtmf_queued.mode==VTS_MOD_ManStop)
		{
			/*most recent element in the queue corresponds to 'stop'*/	
			/*queue up*/
			modeRetVal = mfw_cbuf_put_mode(mfw_cm_dtmf_q_id, VTS_MOD_ManStart);
	    		retVal = mfw_cbuf_put(mfw_cm_dtmf_q_id, &crss_string);
			// If the return values are negative, attempt to Queue the DTMF request failed
	    		if (retVal < 0 || modeRetVal < 0)
	    		{
	    			TRACE_EVENT_P1("ERROR : mfw_cbuf_put failed with error %d", retVal);
	    			TRACE_EVENT_P1("ERROR : mfw_cbuf_put_mode failed with error %d", modeRetVal);
	    			/*
	    			** Failed to Queue the DTMF request.
	    			*/
	    			return(FALSE);
	    		}
			/*successfully queued update the internal variable*/
			dtmf_queued.mode=VTS_MOD_ManStart;
			dtmf_queued.digit=crss_string;
			return (TRUE);			
		}
		else
		{
			/*ignore the key press*/
			return (TRUE);
		}

    	}
    	else
	{
		/*dtmf queue empty*/
	    	if(dtmf_sent.valid==TRUE)
    		{
    			/*some dtmf has already been sent to ACI and MMI is waiting for rAT_ok*/
    			if(dtmf_sent.mode==VTS_MOD_ManStop)
    			{
    				/*last sent dtmf is STOP - so queue up*/
				modeRetVal = mfw_cbuf_put_mode(mfw_cm_dtmf_q_id, VTS_MOD_ManStart);
		    		retVal = mfw_cbuf_put(mfw_cm_dtmf_q_id, &crss_string);
				// If the return values are negative, attempt to Queue the DTMF request failed
		    		if (retVal < 0 || modeRetVal < 0)
		    		{
		    			TRACE_EVENT_P1("ERROR : mfw_cbuf_put failed with error %d", retVal);
		    			TRACE_EVENT_P1("ERROR : mfw_cbuf_put_mode failed with error %d", modeRetVal);
		    			/*
		    			** Failed to Queue the DTMF request.
		    			*/
		    			return(FALSE);
		    		}
				/*successfully queued update the internal variable*/
				dtmf_queued.mode=VTS_MOD_ManStart;
				dtmf_queued.digit=crss_string;
				return (TRUE);
    			}
			else
			{
				return (TRUE); /*ignore the key press*/
			}
    		}
		else
		{
			/*we are not waiting for any rAT_ok - so call sAT_PlusVTS*/
			if(sAT_PlusVTS(CMD_SRC_LCL, (CHAR)crss_string, VTS_MOD_ManStart) == AT_EXCT)
				{
				TRACE_EVENT("sAT_PlusVTS() successful");
				dtmf_sent.valid=TRUE;
				dtmf_sent.mode=VTS_MOD_ManStart;
				dtmf_sent.digit=crss_string;
			}
			else
			{
				TRACE_EVENT("sAT_PlusVTS() busy or failed");
				return (FALSE);
			}				
		}

	}			
	#ifdef MMI_LINT_WARNING_REMOVAL 	/*a0393213 lint warnings removal*/
      currCmd = AT_CMD_VTS;
	#endif
      break;

    case CM_DTMF_INACTIVE:
	// xnkulkar SPR-18262
	// If there is a DTMF tone command already in the queue, add the current tone also
	// in the queue (with stop mode). Also, start adding to the queue if we are expecting an acknowledgement 
	// (rAT_OK) for the previous DTMF tone request sent. 
	/*OMAPS00108192 a0393213 DTMF START-DTMF STOP properly nested*/
    	if (mfw_cbuf_num_elements(mfw_cm_dtmf_q_id) != 0)
    	{
		/*dtmf queue not empty*/
		if(dtmf_queued.mode==VTS_MOD_ManStart && dtmf_queued.digit==crss_string)
		{
			/*most recent element in the queue corresponds to START and are of same digit so queue up*/	
			modeRetVal = mfw_cbuf_put_mode(mfw_cm_dtmf_q_id, VTS_MOD_ManStop);
	    		retVal = mfw_cbuf_put(mfw_cm_dtmf_q_id, &crss_string);
			// If the return values are negative, attempt to Queue the DTMF request failed
	    		if (retVal < 0 || modeRetVal < 0)
	    		{
	    			TRACE_EVENT_P1("ERROR : mfw_cbuf_put failed with error %d", retVal);
	    			TRACE_EVENT_P1("ERROR : mfw_cbuf_put_mode failed with error %d", modeRetVal);
	    			/*
	    			** Failed to Queue the DTMF request.
	    			*/
	    			return(FALSE);
	    		}
			/*successfully queued update the internal variable*/
			dtmf_queued.mode=VTS_MOD_ManStop;
			dtmf_queued.digit=crss_string;
			return (TRUE);			
		}
		else
		{
			/*ignore the key press*/
			return (TRUE);
		}

    	}
    	else
	{
		/*dtmf queue empty*/
	    	if(dtmf_sent.valid==TRUE)
    		{
    			/*some dtmf has already been sent to ACI and MMI is waiting for rAT_ok*/
    			if(dtmf_sent.mode==VTS_MOD_ManStart && dtmf_sent.digit==crss_string)
    			{
    				/*last sent dtmf corresponds to START and are of same digit - so queue up*/
				modeRetVal = mfw_cbuf_put_mode(mfw_cm_dtmf_q_id, VTS_MOD_ManStop);
		    		retVal = mfw_cbuf_put(mfw_cm_dtmf_q_id, &crss_string);
				// If the return values are negative, attempt to Queue the DTMF request failed
		    		if (retVal < 0 || modeRetVal < 0)
		    		{
		    			TRACE_EVENT_P1("ERROR : mfw_cbuf_put failed with error %d", retVal);
		    			TRACE_EVENT_P1("ERROR : mfw_cbuf_put_mode failed with error %d", modeRetVal);
		    			/*
		    			** Failed to Queue the DTMF request.
		    			*/
		    			return(FALSE);
		    		}
				/*successfully queued update the internal variable*/
				dtmf_queued.mode=VTS_MOD_ManStop;
				dtmf_queued.digit=crss_string;
				return (TRUE);
    			}
			else
			{
				return (TRUE); /*ignore the key press*/
			}
    		}
		else
		{
			/*rAT_ok is not expected. so call sAT_PlusVTS*/
		if(sAT_PlusVTS(CMD_SRC_LCL, (CHAR)crss_string, VTS_MOD_ManStop) == AT_EXCT)
			{
				TRACE_EVENT("sAT_PlusVTS() successful");
				dtmf_sent.valid=TRUE;
				dtmf_sent.mode=VTS_MOD_ManStop;
				dtmf_sent.digit=crss_string;
			}
			else
			{
				TRACE_EVENT("sAT_PlusVTS() busy or failed");
				return (FALSE);
			}				
		}
	
      }
      res = TRUE;
      cm_signal(E_CM_DTMF, &res);
      break;

    case CM_DTMF_AUTO:
	/*OMAPS00108192 a0393213(R.Prabakar)
	    As part of the issue CM_DTMF_ACTIVE and CM_DTMF_INACTIVE cases modified. But this case is not modified
	    because this case is felt irrelavant. If required the same logic can be extended to this case also*/
    	if (mfw_cbuf_num_elements(mfw_cm_dtmf_q_id) != 0)
    	{
    		/*
    		** There are already DTMF tones queued to be sent, so queue this request to.
    		*/
		modeRetVal = mfw_cbuf_put_mode(mfw_cm_dtmf_q_id, VTS_MOD_Auto);
    		retVal = mfw_cbuf_put(mfw_cm_dtmf_q_id, &crss_string);
    		if (retVal < 0  || modeRetVal < 0)
    		{
    			TRACE_EVENT_P1("ERROR : mfw_cbuf_put failed with error %d", retVal);
	    		TRACE_EVENT_P1("ERROR : mfw_cbuf_put_mode failed with error %d", modeRetVal);
    			/*
    			** Failed to Queue the DTMF request.
    			*/
    			return(FALSE);
    		}
    	}
    	else
		{
	    	if (sAT_PlusVTS(CMD_SRC_LCL, (CHAR)crss_string, VTS_MOD_Auto) != AT_EXCT)
	      	{
	      		/*
	      		** The attempt to send an immediate DTMF failed.
	      		** Add the failed attempt onto the DTMF queue and wait.
	      		*/
			modeRetVal = mfw_cbuf_put_mode(mfw_cm_dtmf_q_id, VTS_MOD_Auto);
	      		retVal = mfw_cbuf_put(mfw_cm_dtmf_q_id, &crss_string);
	    		if (retVal < 0 || modeRetVal < 0)
	    		{
	    			TRACE_EVENT_P1("ERROR : mfw_cbuf_put failed with error %d", retVal);
	    			TRACE_EVENT_P1("ERROR : mfw_cbuf_put_mode failed with error %d", modeRetVal);
	    			/*
	    			** Failed to Queue the DTMF request.
	    			*/
	    			return(FALSE);
	    		}
	      	}
		}
      break;

    default:
      break;
  }
  return TRUE;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	 MODULE  : MFW_CM	     |
| STATE   : code			 ROUTINE : send_queued_dtmf_tone	     |
+--------------------------------------------------------------------+


   PURPOSE : Check the DTMF Queue and if required send the next DTMF Tone Request.

*/
static void send_queued_dtmf_tone(void)
{
	SHORT retVal;
    UBYTE qdtmf;
	//xnkulkar SPR-18262: Added these variables to store the Mode
	T_ACI_VTS_MOD  vts_mode = VTS_MOD_NotPresent; 

    	TRACE_EVENT("send_queued_dtmf_tone()");
	/*OMAPS00108192 a0393213*/
    if (mfw_cbuf_num_elements(mfw_cm_dtmf_q_id) > 0)
    {
    		/*dtmf queue has some elements - so dequeue and send to ACI*/
    		// Get the mode (Start / Stop) corresponding to the DTMF tone
    		vts_mode = (T_ACI_VTS_MOD)mfw_cbuf_get_mode(mfw_cm_dtmf_q_id);
		// Get the DTMF key for which the tone is to be sent from the queue
	    	retVal = mfw_cbuf_get(mfw_cm_dtmf_q_id, &qdtmf);
	    	if (retVal > 0)
		{
	    		if (sAT_PlusVTS(CMD_SRC_LCL, (CHAR)qdtmf, vts_mode) == AT_EXCT)
		      	{
		      		dtmf_sent.valid=TRUE;
				dtmf_sent.mode=vts_mode;
				dtmf_sent.digit=qdtmf;
		      	}
			
		}
    		else
		{
			TRACE_EVENT_P1("ERROR : mfw_cbuf_get failed with error value %d", retVal);
			dtmf_sent.valid=FALSE;
		}    	
    		
    	}
	else
	{
		dtmf_sent.valid=FALSE;
    	 }
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	 MODULE  : MFW_CM	     |
| STATE   : code			 ROUTINE : resetDTMFQueue	     |
+--------------------------------------------------------------------+
   PURPOSE : reset the dtmf queue
*/
/*OMAPS00107103 (no dtmf tones for simultaneous key press) a0393213(R.Prabakar) - function added*/
void resetDTMFQueue()
{
	TRACE_EVENT("resetDTMFQueue");
	mfw_cbuf_reset(mfw_cm_dtmf_q_id);
	//expectRatOk=FALSE;
	dtmf_sent.valid=FALSE;	
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	 MODULE  : MFW_CM	     |
| STATE   : code			 ROUTINE : cm_ok_dtmf	     |
+--------------------------------------------------------------------+


   PURPOSE : DTMF activation is successful.

*/

void cm_ok_dtmf(void)
{
    UBYTE res;
    #ifdef MMI_LINT_WARNING_REMOVAL /*a0393213 lint warnings removal*/
    currCmd = AT_CMD_NONE;
    #endif
    res = TRUE;
    cm_signal(E_CM_DTMF, &res);

	send_queued_dtmf_tone();

	return;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	 MODULE  : MFW_CM	     |
| STATE   : code			 ROUTINE : cm_error_tdmf     |
+--------------------------------------------------------------------+


   PURPOSE : DTMF activation is failed.

*/

void cm_error_dtmf(void)
{
    UBYTE res;
#ifdef MMI_LINT_WARNING_REMOVAL /*a0393213 lint warnings removal*/
    currCmd = AT_CMD_NONE;
#endif
    res = FALSE;
    cm_signal(E_CM_DTMF, &res);

	send_queued_dtmf_tone();

}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	 MODULE  : MFW_CM	     |
| STATE   : code			 ROUTINE : cm_result_cmd     |
+--------------------------------------------------------------------+


   PURPOSE : Acknowledgement for the requested ss command.

*/

void cm_result_cmd(T_ACI_CMD_SRC src, UBYTE result)
{
    SHORT call_number;

	TRACE_EVENT_P1("cm_result_cmd(), cmd_hold = %d", cmd_hold);

	/* MC changed all the "if"s to a switch to allow default 11/04/02*/
    switch (cmd_hold)
    {
	case MFW_CM_MT_REJECT:
		break; /* CONQUEST 6040- MC-  put a break in the case so no action taken*/
	case MFW_CM_MPTY_TERMINATED:
	case MFW_CM_TERMINATED:

    {
      if (result EQ Present)
	cm_ok_disconnect(CMD_SRC_LCL, 0);
      return;
    }
//	break;  // RAVI

    case MFW_CM_MT_ACCEPT:
    {
      /*
       * active call on hold, accept new incoming call
       */
      cmd_hold = 0;
      if (result EQ Present)
	/*
	 * Call on hold was successful
	 */
	cm_ok_connect();

      else
      {
	if (cm_search_callId(CAL_STAT_Active, &call_number) != CM_OK)
	  return;
	cmd.call_number = call_number;
#ifdef MMI_LINT_WARNING_REMOVAL 	/*a0393213 lint warnings removal*/	
	cc_id = 0; // ???
#endif
      }
    }
	break;


	case MFW_CM_HOLD_AND_DIAL:
    {
      /*
       * active call on hold, accept new incoming call
       */
      cmd_hold = 0;
      if (result EQ Present)
	/*
	 * Call on hold was successful
	 */
	 cm_ok_connect();

      else
      {
	if (cm_search_callId(CAL_STAT_Active, &call_number) != CM_OK)
	  return;
	cmd.call_number = call_number;
#ifdef MMI_LINT_WARNING_REMOVAL 	/*a0393213 lint warnings removal*/	
	cc_id = 0; // ???
#endif	
      }
    }
	break;

	case MFW_CM_1_SEND:
	{
			
		cmd_hold = 0;
		if (result == Present)
		{
			// MMI-SPR 13873 xkundadu
			// Handling of 1SEND 
			// If the user presses 1SEND, release active calls and accept 
			// the WAITING call
		       if (dis_connect.call_number NEQ -1)
		    	{
		    		// If there is an active call, disconnect it.
				cm_ok_disconnect(CMD_SRC_LCL, 0);
		    	}
	      
	    		if (!cmd.call_number)
		       {
				// If there is waiting call, accept the call
			 	cm_ok_connect();
		       }
		    	cmd.call_number = -1;
		}
		
  	}
	   break;

        /* Marcus: CCBS: 14/11/2002: Start */
        case MFW_CM_CCBS_REGISTER:

            TRACE_EVENT("MFW_CM_CCBS_REGISTER");

            cmd_hold = 0;

            if (result == Present)
            {
                TRACE_EVENT("result == Present");
                cm_ccbs_ok();
            }
            else    /* result is NotPresent */
            {
                TRACE_EVENT("result != Present");
                cm_ccbs_error();
            }
            return;

        case MFW_CM_CCBS_RECALL:

            TRACE_EVENT("MFW_CM_CCBS_RECALL");

            cmd_hold = 0;

            if (result == Present)
            {
                TRACE_EVENT("result == Present");
                cm_ccbs_ok();
            }
            else    /* result is NotPresent */
            {
                TRACE_EVENT("result != Present");
                cm_ccbs_error();
            }
            return;
        /* Marcus: CCBS: 14/11/2002: Start */

	default:/*For other command types ensure correct result is passed up to BMI*/
	{
	    cmd.result = result;
	}
	break;
  }


#if defined (WIN32)
    {
      char buf[40];
      sprintf (buf, "Event CM_COMMAND for %d", cmd.call_number);
      TRACE_EVENT (buf);
      sprintf (buf, "Result %d Command %d",cmd.result, cmd.command);
      TRACE_EVENT (buf);
    }
#endif
    cmd.result = result;
    cm_signal(E_CM_COMMAND, &cmd);
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_cvt_speed_type   |
+--------------------------------------------------------------------+


   PURPOSE :  This function is used to convert the type of data rate
	      used by MFW to the type of data rate used by ACI.

*/

LOCAL T_ACI_BS_SPEED cm_cvt_speed_type ( T_MFW_CM_DATA_SPEED speed )
{
  switch ( speed )
  {
    case ( SPEED_300 ):   return BS_SPEED_300_V110;
    case ( SPEED_1200 ):  return BS_SPEED_1200_V110;
    case ( SPEED_2400 ):  return BS_SPEED_2400_V110;
    case ( SPEED_4800 ):  return BS_SPEED_4800_V110;
    case ( SPEED_9600 ):  return BS_SPEED_9600_V110;
    case ( SPEED_14400 ): return BS_SPEED_14400_V110;
    default:	      return BS_SPEED_NotPresent;
  }
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_cvt_bs_type      |
+--------------------------------------------------------------------+


   PURPOSE :  This function is used to convert the type of bearer service
	      used by MFW to the type of bearer service used by ACI.

*/

LOCAL T_ACI_CBST_NAM cm_cvt_bs_type ( T_MFW_CM_BS bs )
{
  switch ( bs )
  {
    case ( Asynch ): return CBST_NAM_Asynch;
    case ( Synch ):  return CBST_NAM_Synch;
    default:	     return CBST_NAM_NotPresent;
  }
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_cvt_ce_type      |
+--------------------------------------------------------------------+


   PURPOSE :  This function is used to convert the  data mode
	      used by MFW to the data mode used by ACI.

*/

LOCAL T_ACI_CBST_CE cm_cvt_ce_type ( T_MFW_CM_CE ce )
{
  switch ( ce )
  {
    case ( Transparent ):     return CBST_CE_Transparent;
    case ( NonTransparent ):  return CBST_CE_NonTransparent;
    default:	      return CBST_CE_NotPresent;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_set_bc_para      |
+--------------------------------------------------------------------+


   PURPOSE :  Set bearer parameter for mobile originated calls.

*/

void cm_set_bc_para(T_MFW_CM_BC_PARA * bc, UBYTE ti)
{
  TRACE_FUNCTION ("cm_set_bc_para()");
#ifdef FAX_AND_DATA
  sAT_PlusCBST(CMD_SRC_LCL,
	       cm_cvt_speed_type ((T_MFW_CM_DATA_SPEED)bc->rate),
	       cm_cvt_bs_type ((T_MFW_CM_BS)bc->bearer_serv),
	       cm_cvt_ce_type ((T_MFW_CM_CE)bc->conn_elem));
#endif /* FAX_AND_DATA */
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_notify	     |
+--------------------------------------------------------------------+


   PURPOSE :  Change of SS status from call party.

*/
void cm_notify(T_ACI_CSSU_CODE code,
			   SHORT	   index,
			   UBYTE	  *number)
{	char debug[50];
	sprintf(debug,"cm_notify(), code:%d, index:%d" , code, index);
	TRACE_EVENT(debug);
	if (number)
		notify.call_number = index;

	if (code == CSSU_CODE_OnHold)
		notify.notification = MFW_CM_HOLD;
	if (code == CSSU_CODE_Retrieved)
		notify.notification = MFW_CM_RETRIEVED;

	cm_signal(E_CM_NOTIFY, &notify);
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	 MODULE  : MFW_CM	     |
| STATE   : code			 ROUTINE : cm_get_aoc_value  |
+--------------------------------------------------------------------+


   PURPOSE :  Getting AoC (advice of charge) value.

*/

T_MFW cm_get_aoc_value(UBYTE request_type, T_MFW_CM_AOC_INFO *aoc_info)
{
    T_ACI_RPCT_VAL  rpuct;

    TRACE_FUNCTION ("cm_get_aoc_value()");

    switch (request_type)
    {
	/* Price pro unit and currency table */
	case CM_AOC_PUCT:
	    if (qAT_PlusCPUC(CMD_SRC_LCL,
		(CHAR *)aoc_info->cur,
		(CHAR *)aoc_info->ppu) == AT_CMPL)
	    {
		aoc_info->requested_type = CM_AOC_PUCT;
		return CM_OK;
	    }
	    else
		return CM_NO_AOC;
//	    break;  // RAVI

	/* maximum value of accumulated Call Meter */
	case CM_AOC_ACMMAX:
	    if (qAT_PlusCAMM(CMD_SRC_LCL, &aoc_info->acm_max) == AT_CMPL)
	    {
		aoc_info->requested_type = CM_AOC_ACMMAX;
		return CM_OK;
	    }
	    else
		return CM_NO_AOC;
//	    break;  // RAVI

	/* Accumulated Call Meter */
	case CM_AOC_ACM:
	    if (qAT_PlusCACM(CMD_SRC_LCL, &aoc_info->acm) == AT_CMPL)
	    {
		aoc_info->requested_type = CM_AOC_ACM;
		return CM_OK;
	    }
	    else
		return CM_NO_AOC;
	//     break;  // RAVI

	case CM_AOC_CCM:
	    if (qAT_PlusCAOC(CMD_SRC_LCL, &aoc_info->ccm) == AT_CMPL)
	    {
		aoc_info->requested_type = CM_AOC_CCM;
		return CM_OK;
	    }
	    else
		return CM_NO_AOC;
//	    break;   // RAVI

    case CM_AOC_TIMERS:
	if (qAT_PercentCTV(CMD_SRC_LCL, &aoc_info->actual_call_timer) == AT_CMPL)
	{
	    aoc_info->requested_type = CM_AOC_TIMERS;
	    return CM_OK;
	}
	else
	    return CM_NO_AOC;
//	break;  // RAVI

    case CM_AOC_CCM_PUCT:
	if (qAT_PercentCAOC(CMD_SRC_LCL, (CHAR *)aoc_info->cur, (CHAR *)aoc_info->ccm_puct) == AT_CMPL)
	{
	    aoc_info->requested_type = CM_AOC_CCM_PUCT;
	    return CM_OK;
	}
	else
	    return CM_NO_AOC;
//	break;  // RAVI

    case CM_AOC_ACM_PUCT:
	if (qAT_PercentCACM(CMD_SRC_LCL, (CHAR *)aoc_info->cur, (CHAR *)aoc_info->acm_puct) == AT_CMPL)
	{
	    aoc_info->requested_type = CM_AOC_ACM_PUCT;
	    return CM_OK;
	}
	else
	    return CM_NO_AOC;
    case CM_AOC_RAW_PUCT:
	if (qAT_PercentRPCT(CMD_SRC_LCL, &rpuct) == AT_CMPL)
	{
	    aoc_info->requested_type = CM_AOC_RAW_PUCT;
	    strncpy((char *)aoc_info->cur, (char *)rpuct.currency, MAX_CUR_LEN);
	    aoc_info->eppu = rpuct.eppu;
	    aoc_info->exp  = rpuct.exp;
	    aoc_info->sexp = rpuct.sexp;
	    return CM_OK;
	}
	else
	    return CM_NO_AOC;
//	break;  // RAVI

    default:
	return CM_NO_AOC;
  }
}



/*
+----------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	 MODULE  : MFW_CM	       |
| STATE   : code			 ROUTINE : cm_reset_aoc_value  |
+----------------------------------------------------------------------+


   PURPOSE :  Setting AoC (advice of charge) value.

*/

T_MFW cm_set_aoc_value(UBYTE request_type,
		       T_MFW_CM_AOC_INFO *aoc_info,
		       UBYTE *pwd)
{
    TRACE_FUNCTION ("cm_reset_aoc_value()");

    switch (request_type)
    {
	case CM_AOC_PUCT:
	    if (sAT_PlusCPUC(CMD_SRC_LCL, (CHAR *)aoc_info->cur,
			     (CHAR *)aoc_info->ppu, (CHAR *)pwd) != AT_EXCT)
		return CM_ERROR;
	    break;

	case CM_AOC_ACMMAX:
	    if (sAT_PlusCAMM(CMD_SRC_LCL, aoc_info->acm_max,
			     (CHAR *)pwd) != AT_EXCT)
		return CM_ERROR;
	    break;

	case CM_AOC_ACM:
	    if (sAT_PlusCACM(CMD_SRC_LCL, (CHAR *)pwd) != AT_EXCT)
		return CM_ERROR;
	    break;

	default:
	    return CM_ERROR;
    }

    return CM_OK;
}


/*
+----------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	 MODULE  : MFW_CM	       |
| STATE   : code			 ROUTINE : cm_cvt_cmd	       |
+----------------------------------------------------------------------+


   PURPOSE :  Convert AoC command

*/

UBYTE cm_cvt_cmd(T_ACI_AT_CMD cmdId)
{
    switch (cmdId)
    {
	case AT_CMD_CACM:   return CM_AOC_ACM;
	case AT_CMD_CAMM:   return CM_AOC_ACMMAX;
	case AT_CMD_CPUC:   return CM_AOC_PUCT;
	default:	    return CM_AOC_UNKNOWN;
    }
}


/*
+----------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	 MODULE  : MFW_CM	       |
| STATE   : code			 ROUTINE : cm_ok_aoc	       |
+----------------------------------------------------------------------+


   PURPOSE :  Setting AoC is successful.

*/

void cm_ok_aoc(T_ACI_AT_CMD cmdId)
{
    T_MFW_CM_AOC_CNF aoc_cnf;

    aoc_cnf.requested_type = cm_cvt_cmd(cmdId);
    aoc_cnf.result = CM_AOC_CONF_OK;
    cm_signal(E_CM_AOC_CNF, &aoc_cnf);
}


/*
+----------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	 MODULE  : MFW_CM	       |
| STATE   : code			 ROUTINE : cm_error_aoc        |
+----------------------------------------------------------------------+


   PURPOSE :  Setting AoC is failure.

*/

void cm_error_aoc(T_ACI_AT_CMD cmdId, T_ACI_CME_ERR reason)
{
    T_MFW_CM_AOC_CNF aoc_cnf;

    aoc_cnf.requested_type = cm_cvt_cmd(cmdId);
    aoc_cnf.result = CM_AOC_CONF_UNKNOWN;

    if (reason EQ CME_ERR_SimPin2Req)
	aoc_cnf.result = CM_AOC_CONF_SIMPIN2REQ;

    if (reason EQ CME_ERR_WrongPasswd)
	aoc_cnf.result = CM_AOC_CONF_WRONGPASSWD;

    if (reason EQ CME_ERR_OpNotAllow)
	aoc_cnf.result = CM_AOC_CONF_OPNOTALLOW;

    if (reason EQ CME_ERR_AcmResetNeeded)
	aoc_cnf.result = CM_AOC_CONF_ACMRESETNEEDED;

    cm_signal(E_CM_AOC_CNF, &aoc_cnf);
}


/*
+-----------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	 MODULE  : MFW_CM	  |
| STATE   : code			 ROUTINE : rAT_PercentCTV |
+-----------------------------------------------------------------+


   PURPOSE : manage timer in every second

*/

void rAT_PercentCTV()
{
    TRACE_FUNCTION("rAT_PercentCTV()");
    cm_signal(E_CM_TIMER_EVERY_SECOND, 0);
}


/*
+-----------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	 MODULE  : MFW_CM	  |
| STATE   : code			 ROUTINE : rAT_PlusCCCM   |
+-----------------------------------------------------------------+


   PURPOSE :

*/

void rAT_PlusCCCM(LONG *ccm)
{
    T_MFW_CM_AOC_INFO aoc_info;
    T_ACI_RPCT_VAL    rpuct;

    TRACE_FUNCTION("rAT_PlusCCCM()");

    memset(&aoc_info, 0, sizeof(T_MFW_CM_AOC_INFO));

    qAT_PlusCAMM(CMD_SRC_LCL, &aoc_info.acm_max);
    qAT_PlusCACM(CMD_SRC_LCL, &aoc_info.acm);
    aoc_info.ccm = *ccm;
    qAT_PercentCTV(CMD_SRC_LCL, &aoc_info.actual_call_timer);
    qAT_PercentRPCT(CMD_SRC_LCL, &rpuct);
    strncpy((char *)aoc_info.cur, (char *)rpuct.currency, MAX_CUR_LEN);
    aoc_info.requested_type = CM_AOC_CCM;
    aoc_info.eppu = rpuct.eppu;
    aoc_info.exp  = rpuct.exp;
    aoc_info.sexp = rpuct.sexp;

    cm_signal(E_CM_AOC, &aoc_info);
}


/*
+----------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	 MODULE  : MFW_CM	 |
| STATE   : code			 ROUTINE : cm_set_redial |
+----------------------------------------------------------------+


   PURPOSE :  Set redial mode (automatic/manual).

*/

void cm_set_redial(UBYTE redial_mode)
{
 // U8 version;  // RAVI
//	Apr 14, 2005	REF: CRR 29991   xpradipg
//	local definition
#ifdef FF_MMI_OPTIM
	T_MFW_CM_BLACKLIST_LIST black_list;
#endif	

  TRACE_FUNCTION ("cm_set_redial()");

  if (redial_mode == CM_REDIAL_BLACKLIST)
  {
	memset(&black_list, 0, sizeof(T_MFW_CM_BLACKLIST_LIST));
//	Apr 14, 2005	REF: CRR 29991   xpradipg
//	reset the file contents
#ifdef FF_MMI_OPTIM
  	if(flash_MMI_blackList_write((U8 *)&black_list, sizeof(T_MFW_CM_BLACKLIST_LIST),0) < 0)
		TRACE_FUNCTION(" Error writing into file");
#endif	
	return;
  }

  if (redial_mode == CM_REDIAL_OFF
	  || redial_mode == CM_REDIAL_AUTO
	  || redial_mode == CM_REDIAL_MANU)
  {
    redial_stat = redial_mode;

    /* write redialing mode to EEPROM */

	FFS_flashData.redial_mode =redial_mode;
	flash_write();

  }
}


/*
+---------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	 MODULE  : MFW_CM	      |
| STATE   : code			 ROUTINE : cm_get_redial_mode |
+---------------------------------------------------------------------+


   PURPOSE :  Request the setting of redial mode (automatic/manual).

*/

T_MFW cm_get_redial_mode(void)
{
    return redial_stat;
}




/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_stop_redialTim   |
+--------------------------------------------------------------------+


   PURPOSE :  Stop redialing time.

*/

void cm_stop_redialTim(void)
{
    if (!redial_count)
	timStop(redialTim1);

    if (redial_count >= 1 AND redial_count <= 3)
	timStop(redialTim24);

    if (redial_count >= 4 AND redial_count < MAX_REDIAL_ATTEMPT)
	timStop(redialTim5);
	redial_count = 0;
	rdl.left_attempts = 0;

	/*
		reset the counter
	*/

    call_stat	 = MFW_CALL_FREE;
    call_type	 = MFW_CALL_FREE;
    cc_stat.type = MFW_CALL_FREE;
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_redial_abort     |
+--------------------------------------------------------------------+


   PURPOSE :  Stop redialing from MMI.

*/

void cm_redial_abort(void)
{

	TRACE_FUNCTION ("cm_redial_abort()");

    if (call_stat == REDIALING_CALL OR	call_stat  == OUTGOING_CALL)
	cm_stop_redialTim();

    rdl.redial_mode = CM_REDIAL_ABORTED;
    cm_signal(E_CM_REDIAL, &rdl);
}


/*
+---------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		      |
| STATE  : code 			ROUTINE: cm_autoredial_start  |
+---------------------------------------------------------------------+


   PURPOSE :  start autoredial.

*/

void cm_autoredial_start(void)
{
	timStart(redialTim1);
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_redial_timeout   |
+--------------------------------------------------------------------+


   PURPOSE :  Call back from system clock for redialing time interval.

*/

static int cm_redial_timeout(MfwEvt e, MfwTim *t)
{
  CHAR number[MFW_NUM_LEN+MAX_LEN+1];
  int max_num_size = MFW_NUM_LEN+MAX_LEN+1;

  TRACE_FUNCTION ("cm_redial_timeout()");

  memset(number, 0, sizeof(number));
  mfwStrncpy(number, (CHAR *)cc_stat.prefix, max_num_size);
  max_num_size -= strlen((char *)cc_stat.prefix);
  
  if ((cc_stat.ton EQ MFW_TON_INTERNATIONAL) AND (cc_stat.number[0] != '+'))
  {
    strcat(number, "+");
    max_num_size--;
  }
  
  strncat(number, (CHAR *)cc_stat.number, max_num_size);

  switch (redial_stat)
  {
    case CM_REDIAL_AUTO:
      if (sAT_Dn(CMD_SRC_LCL,
			     (CHAR *)number,
			      D_CLIR_OVRD_Default,
			      D_CUG_CTRL_NotPresent,
			      D_TOC_Voice) NEQ AT_EXCT)
      {
	    TRACE_ERROR("sAT_Dn error");
	return 0;
      }

      call_stat    = OUTGOING_CALL;

      cm_search_callId(CAL_STAT_Dial, &cc_stat.call_number);
      cc_stat.type = REDIALING_CALL;

      rdl.redial_mode	= CM_REDIAL_ATTEMPT;
      rdl.call_id	= cc_stat.call_number;
      mfwStrncpy((char *)rdl.number, (char *)cc_stat.number, MFW_NUM_LEN);
#ifdef NO_ASCIIZ
      rdl.name.len = cc_stat.name.len;
      memcpy(rdl.name.data,
	     cc_stat.name.data,
	     cc_stat.name.len);
#else
      mfwStrncpy((char *)rdl.name, (char *)cc_stat.name, MFW_TAG_LEN);
//	strcpy((char *)rdl.subaddr, (char *)cc_stat.subaddr);
#endif
      rdl.ton = cc_stat.ton;
      rdl.type = cc_stat.mode;
      redial_count++;
      rdl.left_attempts = MAX_REDIAL_ATTEMPT - redial_count;
      cm_signal(E_CM_REDIAL, &rdl);
      return 1;

    case CM_REDIAL_MANU:
      rdl.redial_mode	= CM_REDIAL_MANU;
      mfwStrncpy((char *)rdl.number, (char *)cc_stat.number, MFW_NUM_LEN);
#ifdef NO_ASCIIZ
      rdl.name.len = cc_stat.name.len;
      memcpy(rdl.name.data,
	     cc_stat.name.data,
	     cc_stat.name.len);
#else
      mfwStrncpy((char *)rdl.name, (char *)cc_stat.name, MFW_TAG_LEN);
#endif
      rdl.ton = cc_stat.ton;
      rdl.type = cc_stat.mode;
      rdl.left_attempts = MAX_REDIAL_ATTEMPT - redial_count;

      cm_signal(E_CM_REDIAL, &rdl);
      return 1;

    default:
      return 0;
  }
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_redial	     |
+--------------------------------------------------------------------+


   PURPOSE :  Start the manual redialing.

*/

void cm_redial()
{
  char buf[80];

  int max_buf_size = sizeof(buf);

  TRACE_FUNCTION ("cm_redial()");

  buf[0] = '\0';

  mfwStrncpy(buf, (char *)cc_stat.prefix, max_buf_size);
  max_buf_size -= strlen((char *)cc_stat.prefix);
  
  if ((cc_stat.ton EQ MFW_TON_INTERNATIONAL) AND (cc_stat.number[0] != '+'))
  {
      strcat(buf, "+");
      max_buf_size--;
  }

  strncat(buf, (char *)cc_stat.number, max_buf_size);

  if (sAT_Dn(CMD_SRC_LCL, buf,  D_CLIR_OVRD_Default, D_CUG_CTRL_NotPresent, D_TOC_Voice) NEQ AT_EXCT) /*a0393213 compiler warning removal -- -1 changed to respective enum value*/
  {
	TRACE_ERROR("sAT_Dn error");
  }

  call_stat = OUTGOING_CALL;
  cc_stat.type = REDIALING_CALL;
  redial_count++;
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_redial_get_count	 |
+--------------------------------------------------------------------+


   PURPOSE :  returns the counter of call attempt

*/
UBYTE	cm_redial_get_count(void)
{
	return redial_count;
}



/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_cvt_call_status  |
+--------------------------------------------------------------------+


   PURPOSE :  Convert the call status.

*/

LOCAL T_MFW_CM_CALL_STAT cm_cvt_call_status( T_ACI_CAL_STAT stat )
{
  switch ( stat )
  {
    case ( CAL_STAT_Held ):    return CALL_HELD;
    case ( CAL_STAT_Active ):  return CALL_ACTIVE;
    case ( CAL_STAT_Wait ):    return CALL_WAIT;
    case ( CAL_STAT_Dial ):    return CALL_DIAL;
    case (CAL_STAT_Alerting):  return CALL_ALERT;
    case (CAL_STAT_DeactiveReq): return CALL_DEACTIVE;
    default:		       return NOT_PRESENT;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_cvt_call_status  |
+--------------------------------------------------------------------+


   PURPOSE :  Request the call status.

*/

T_MFW_CM_CALL_TYPE cm_cvt_call_mode( T_ACI_CAL_MODE type )
{
TRACE_EVENT_P1("cm_cnt_call_mode %d", type);
  switch ( type )
  {
    case ( CAL_MODE_Voice ):	return VOICE_CALL;
    case ( CAL_MODE_Data ):	return DATA_CALL;
    case ( CAL_MODE_Fax ):	return FAX_CALL;
    case (CAL_MODE_VFD_Voice):	return VFD_VOICE;
    case (CAL_MODE_VAD_Voice):	return VAD_VOICE;
    case (CAL_MODE_VAF_Voice):	return VAF_VOICE;
    case (CAL_MODE_VFD_Data):	return VFD_DATA;
    case (CAL_MODE_VAD_Data):	return VAD_DATA;
    case (CAL_MODE_VAF_Fax):	return VAF_FAX;
    default:			return NONE_TYPE;
  }
}

T_MFW_CM_CALL_DIR cm_cvt_call_direction( T_ACI_CAL_TYPE type )
{
TRACE_EVENT_P1("cm_cvt_call_direction %d", type);
  switch ( type )
  {
    case ( CAL_TYPE_NotPresent):	return MFW_CM_INV_DIR;
    case ( CAL_TYPE_MOC):			return MFW_CM_MOC;
    case ( CAL_TYPE_MTC):			return MFW_CM_MTC;
    default:			return MFW_CM_INV_DIR;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_status	     |
+--------------------------------------------------------------------+


   PURPOSE :  Request the call status.

*/

int cm_status(SHORT call_number, T_MFW_CM_STATUS *status, UBYTE num)
{
    UBYTE cnt;
    int   i;
// 	Apr 06 2005		REF: CRR 29989	xpradipg	
//	define and allocate memory
#ifdef FF_MMI_OPTIM
	T_ACI_CAL_ENTR 	    *call_tab;//[MAX_CALL_NR];
	call_tab = (T_ACI_CAL_ENTR*)mfwAlloc(sizeof(T_ACI_CAL_ENTR)*MAX_CALL_NR);
	if( NULL == call_tab)
		return 0;
#endif


    cnt = 0;
    qAT_PercentCAL(CMD_SRC_LCL, call_tab);

    if (call_number > 0)
    {
      for (i=0; i<MAX_CALL_NR; i++)
      {
	if (call_tab[i].index == call_number)
	{
	  status[0].call_number = call_tab[i].index;
	  status[0].stat =  cm_cvt_call_status(call_tab[i].status);
	  status[0].type = cm_cvt_call_mode(call_tab[i].calMode);
	  status[0].mtpy = (T_MFW_MTPY_MEMBER)call_tab[i].mpty;
	  status[0].call_direction = cm_cvt_call_direction(call_tab[i].calType);

	  if (call_tab[i].number != NULL)
	  {
	    mfwStrncpy((char *)status[0].number, call_tab[i].number, MFW_NUM_LEN);
	    status[0].ton = phb_cvtTon(call_tab[i].type.ton);
	  }
	  else
	    memset(status[0].number, '\0', MFW_NUM_LEN);

#ifdef NO_ASCIIZ
	  status[0].name.len = call_tab[i].alpha.len;
	  if (call_tab[i].alpha.len)
	    memcpy(status[0].name.data,
		   call_tab[i].alpha.data,
		   call_tab[i].alpha.len);
	  else //We'll also zero the string.
	    memset(status[0].name.data, '\0', PHB_MAX_LEN);
#else
	  if (call_tab[i].alpha != NULL)
	    mfwStrncpy((char *)status[0].name, call_tab[i].alpha, MFW_TAG_LEN);
	  else
	    memset(status[0].name, '\0', MFW_TAG_LEN);
#endif
	  cnt = 1;
	  break;
	}
      }
// 	Apr 06 2005		REF: CRR 29989	xpradipg	
//	deallocate memory
#ifdef FF_MMI_OPTIM
	    if(call_tab)
	    mfwFree((U8*)call_tab, sizeof(T_ACI_CAL_ENTR)*MAX_CALL_NR);
#endif      
      return cnt;
    }

    if (!call_number)
    {
      for (i=0; i<MAX_CALL_NR; i++)
      {
	if ((call_tab[i].index EQ -1) OR cnt > num)
	    break;

	if (call_tab[i].status != CAL_STAT_NotPresent)
	{
	  status[cnt].call_number = call_tab[i].index;
	  status[cnt].stat = cm_cvt_call_status(call_tab[i].status);
	  status[cnt].type = cm_cvt_call_mode(call_tab[i].calMode);
	  status[cnt].mtpy = (T_MFW_MTPY_MEMBER)call_tab[i].mpty;
	  if (call_tab[i].number != NULL)
	  {
	    mfwStrncpy((char *)status[cnt].number, call_tab[i].number, MFW_NUM_LEN);
	    status[cnt].ton = phb_cvtTon(call_tab[i].type.ton);
	  }
	  else
	    memset(status[cnt].number, '\0', MFW_NUM_LEN);

#ifdef NO_ASCIIZ
	  status[cnt].name.len = call_tab[i].alpha.len;
	  if (call_tab[i].alpha.len != 0)
	    memcpy(status[cnt].name.data,
		   call_tab[i].alpha.data,
		   call_tab[i].alpha.len);
	  else //We'll also zero the string.
	    memset(status[cnt].name.data, '\0', PHB_MAX_LEN);
#else
	  if (call_tab[i].alpha != NULL)
	    mfwStrncpy((char *)status[cnt].name, call_tab[i].alpha, MFW_TAG_LEN);
	  else
	    memset(status[cnt].name, '\0', MFW_TAG_LEN);
#endif
	  cnt++;
	}
      }
// 	Apr 06 2005		REF: CRR 29989	xpradipg	
//	deallocate memory
#ifdef FF_MMI_OPTIM
	    if(call_tab)
	    mfwFree((U8*)call_tab, sizeof(T_ACI_CAL_ENTR)*MAX_CALL_NR);
#endif      
      return cnt;
    }
// 	Apr 06 2005		REF: CRR 29989	xpradipg	
// deallocate memory
#ifdef FF_MMI_OPTIM
	    if(call_tab)
	    mfwFree((U8*)call_tab, sizeof(T_ACI_CAL_ENTR)*MAX_CALL_NR);
#endif    
    return 0;
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_AT_CHLD	     |
+--------------------------------------------------------------------+


   PURPOSE :  Notify the CHLD command

*/

void cm_AT_CHLD(T_ACI_CLOG *clog_cmd)
{
    SHORT id;
    #ifdef FF_ATI
    T_ACI_CAL_MODE call_mode;
    #endif
    T_MFW_CM_STATUS status;
    char buf[80];

    TRACE_EVENT("cm_AT_CHLD()");

    /*SPR#1965 - DS - Removed setting of call_number to zero. Port of 1962
     *dis_connect.call_number = 0;
     */
    sprintf(buf, "dis_connect.call_number %d, cmd_hold %d, cId %d, srcId %d, mode %d, act %d",
            dis_connect.call_number, cmd_hold, clog_cmd->cId, clog_cmd->cmdPrm.sCHLD.srcId,
            clog_cmd->cmdPrm.sCHLD.mode, clog_cmd->cmdPrm.sCHLD.act);
    TRACE_EVENT(buf);

    if ((clog_cmd->cmdPrm.sCHLD.act != CHLD_ACT_Retrieve) &&
         (clog_cmd->cmdPrm.sCHLD.act != CHLD_ACT_RetrieveMpty))
    {
         /*SPR#1962 - DS - Set call_number to call Id returned from ACI */
        TRACE_EVENT("Set dis_connect.call_number to clog_cmd->cId");
        dis_connect.call_number = clog_cmd->cId;
    }
    else
    {
        dis_connect.call_number = -1;
    }
    if (clog_cmd->cmdPrm.sCHLD.srcId == CMD_SRC_LCL)
    {
	switch(clog_cmd->cmdPrm.sCHLD.mode)
	{
	    case CHLD_MOD_RelHldOrUdub:

		if (cm_search_callId(CAL_STAT_DeactiveReq, &id) == CM_OK)
		{
		   TRACE_FUNCTION("CAL_STAT_DeactiveReq - 0SEND");
		    if ( cmd_hold EQ MFW_CM_1_SEND )
		    {
		    	TRACE_FUNCTION("cmd_hold EQ MFW_CM_1_SEND");
			cm_status(id, &status, 1);
			if (status.mtpy EQ MFW_MTPY_MEMBER)
			  dis_connect.call_number = 0;
			else
			  dis_connect.call_number = id;
		    }
		}

        // Apr 26 2006, a0393213 (Prabakar R), OMAPS00074886 : No option in MMI to release the held call. 
        // cmd_hold variable is set to MFW_CM_TERMINATED to indicate that the call has been terminated
		cmd_hold = MFW_CM_TERMINATED;
		break;
					
	case CHLD_MOD_RelActAndAcpt:

		// MMI-SPR 13873 xkundadu
		// Handling of 1+SEND - Releases all active calls (if any exist) and 
		// accepts the other (held or waiting) call.

		if( (cm_search_callId(CAL_STAT_DeactiveReq, &id) == CM_OK) OR
		(cm_search_callId(CAL_STAT_Active, &id) == CM_OK))
		{
			// If there is an active call/active call whose disconnection
			// is initiated, store the index of that call.
			dis_connect.call_number = id;
			if(bIsMultpty == TRUE)
			{
				//  Call is in multiparty
				//  1SEND for multiparty will disconnect all the active calls
				//  To clear all calls assign the dis_connect.call_number to 
				//  zero
				dis_connect.call_number = 0;	
				bIsMultpty = FALSE;
			}
		}
		else
		{
			// There is no active calls/active calls whose disconnection is 
			// iniated existing
			dis_connect.call_number = -1;	
		}
		if (cm_search_callId(CAL_STAT_Wait, &id) == CM_OK)
		{
			// Waiting call is existing.
			// In the function cm_result_cmd(), value of cmd.call_number
			// is used to determine whether to call cm_connect() or not
			cmd.call_number=0;
		}
		cmd_hold = MFW_CM_1_SEND; 
		break;
	
	case CHLD_MOD_HldActAndAcpt:
		
		// MMI -SPR 13873 xkundadu
		// Handling of 2+SEND - Places all active calls (if any exist) on hold
		// and accepts the other (held or waiting) call.
		
		switch (clog_cmd->cmdPrm.sCHLD.act)
		{
		
		  case CHLD_ACT_Accept:
		  	   // Accept the incoming call
			    cmd_hold = MFW_CM_MT_ACCEPT;
			    break;
				
		  case CHLD_ACT_Hold:
		  	    // Getting the id of the call which has been put on hold by ACI
			    cmd.command = CM_HOLD;
			    if (cm_search_callId(CAL_STAT_Held, &id) == CM_OK)
			    {
			     	   cmd.call_number = id;
			    }
			    else
			    {
			      	   cmd.call_number = -1;
			    }
			    break;
				
		  case CHLD_ACT_Swap:
		  	   // Swap the call
			    cmd.command = CM_SWAP;
			    break;
				
		  case CHLD_ACT_Retrieve:
		  	    // Getting the id of the call which has been made active by ACI
			    cmd.command = CM_RETRIEVE;
			    if (cm_search_callId(CAL_STAT_Active, &id) == CM_OK)
			    {
			      	   cmd.call_number = id;
			    }
			    else
			    {
			          cmd.call_number = -1;
			    }
			    break;
		  case CHLD_ACT_HoldMpty:
		  	    // Put the calls on HOLD
			    cmd.command = CM_HOLD_MULTIPARTY;
			    break;
				
		  case CHLD_ACT_RetrieveMpty:
		  	    // Retrieve the held calls to active
			    cmd.command = CM_RETRIEVE_MULTIPARTY;
			    break;
				
		   case CHLD_ACT_SwapMpty:
		  	    // Swap muliparty calls
			    cmd.command = CM_SWAP_MULTIPARTY;
			    break;
				
		  default:
			    cmd.command = CM_COMMAND_UNKNOWN;
			    break;
				
		}
		break;
	   case CHLD_MOD_RelActSpec:
		dis_connect.call_number = (SHORT)atoi(clog_cmd->cmdPrm.sCHLD.call);
		break;			
		
	    default:
		break;
	}
    }

//	Aug 23, 2004 REF: CRR 22222  xkundadu
//  Bug:  Runaway Call Waiting Tone.
//  Fix: Handle the AT-Command source id CMD_SRC_ATI_2 also for call control
//  callback functions.Include source Id CMD_SRC_ATI_2 also for checking.

//Nov 02, 2005    DR OMAPS00052132   xdeepadh
//The usage of ATI enum is put under the flag FF_ATI

//Apr 21, 2006 OMAPS00070774 a0393213 (Prabakar R)
//Bug : Make MO call from MS1 to MS2. Make MT call from MS3 to MS1. Type at+chld=2 in ATI. 
//      expected behaviour : active call should be put to hold and the incoming call should be accepted
//      observed behaviour : happens as expected. but display shows incoming call screen only.
//                           active screen is not shown eventhough the connection is established between MS1 and MS3
//Fix : AT commands from CMD_SRC_ATI_3 was not processed. As a result, MFW was not informed of the acceptance of the call 
//through ATI. In precise cmd_hold variable was not set to MFW_CM_MT_ACCEPT, which is necessary for display update

#ifdef FF_ATI  
    if ((clog_cmd->cmdPrm.sCHLD.srcId == CMD_SRC_ATI_1) //NM Berlin UK integration
		OR (clog_cmd->cmdPrm.sCHLD.srcId == CMD_SRC_ATI_2) // Added for MMI-SPR 22222 FIX
		OR (clog_cmd->cmdPrm.sCHLD.srcId == CMD_SRC_ATI_3)) // Added for OMAPS00070774 FIX  
    {
	switch(clog_cmd->cmdPrm.sCHLD.mode)
	{
	    case CHLD_MOD_RelHldOrUdub:
		if (clog_cmd->cmdPrm.sCHLD.act == CHLD_ACT_Release)
		    dis_connect.call_number = clog_cmd->cId;
		cmd_hold = MFW_CM_TERMINATED;
		break;
	    case CHLD_MOD_RelActAndAcpt:
		switch (clog_cmd->cmdPrm.sCHLD.act)
		{
		    case CHLD_ACT_Release:
			dis_connect.call_number = clog_cmd->cId;
			break;
		    case CHLD_ACT_Accept:
			if (cm_search_callId(CAL_STAT_DeactiveReq, &id) == CM_OK)
			    dis_connect.call_number = id;
			else if (cm_search_callId(CAL_STAT_Active, &id) == CM_OK)
			{
			    call_mode = cm_search_callMode(id);
			    if (call_mode EQ CAL_MODE_Data
				OR call_mode EQ CAL_MODE_Fax)
				dis_connect.call_number = id;
			}
			if (cm_search_callId(CAL_STAT_Wait, &id) == CM_OK)
			{
			   #ifdef MMI_LINT_WARNING_REMOVAL 	/*a0393213 lint warnings removal*/			
			    cc_id = id;
			   #endif
	
		//Apr 22, 2006 OMAPS00070762 a0393213 (Prabakar R)
        //Bug : Make MO call from MS1 to MS2. Make MT call from MS3 to MS1. Type at+chld=1 in ATI. 
        //expected behaviour : active call should be released and the incoming call should be accepted
        //observed behaviour : happens as expected. but display shows incoming call screen only.
        //           active screen is not shown eventhough the connection is established between MS1 and MS3
        //Fix : cmd.call_number was not set to 0, which is required in cm_result_cmd()
        //      to call cm_ok_connect()which updates the display. It's set.
				cmd.call_number=0;
			}
			    
			break;
		    case CHLD_ACT_Retrieve:
			if (cm_search_callId(CAL_STAT_Held, &id) == CM_OK)
			{
			    cmd.command     = CM_RETRIEVE;
			    cmd.call_number = id;
			}
			#ifdef MMI_LINT_WARNING_REMOVAL 	/*a0393213 lint warnings removal*/		
			if (cm_search_callId(CAL_STAT_Wait, &id) == CM_OK)
			    cc_id = id;
			#else
			cm_search_callId(CAL_STAT_Wait, &id);
			#endif
			break;
		}
		cmd_hold = MFW_CM_1_SEND;
		break;
	    case CHLD_MOD_RelActSpec:
		dis_connect.call_number = clog_cmd->cId;
		if (dis_connect.call_number)
	       cmd_hold = MFW_CM_TERMINATED;
	    else
	       cmd_hold = MFW_CM_MPTY_TERMINATED;
		break;
	    case   CHLD_MOD_HldActAndAcpt:
		switch (clog_cmd->cmdPrm.sCHLD.act)
		{
		  case CHLD_ACT_Accept:
		    cmd_hold = MFW_CM_MT_ACCEPT;
		    break;
		  case CHLD_ACT_Hold:
		    cmd.command = CM_HOLD;
		    if (cm_search_callId(CAL_STAT_Held, &id) == CM_OK)
		      cmd.call_number = id;
		    else
		      cmd.call_number = -1;
		    break;
		  case CHLD_ACT_Swap:
		    cmd.command = CM_SWAP;
		    break;
		  case CHLD_ACT_Retrieve:
		    cmd.command = CM_RETRIEVE;
		    if (cm_search_callId(CAL_STAT_Active, &id) == CM_OK)
		      cmd.call_number = id;
		    else
		      cmd.call_number = -1;
		    break;
		  case CHLD_ACT_SwapMpty:
		    cmd.command = CM_SWAP_MULTIPARTY;
		    break;
		  case CHLD_ACT_HoldMpty:
		    cmd.command = CM_HOLD_MULTIPARTY;
		    break;
		  case CHLD_ACT_RetrieveMpty:
		    cmd.command = CM_RETRIEVE_MULTIPARTY;
		    break;
		  default:
		    cmd.command = CM_COMMAND_UNKNOWN;
		    break;
		}
		break;
	    case CHLD_MOD_HldActExc:
		 if (clog_cmd->cmdPrm.sCHLD.act == CHLD_ACT_SplitMpty)
		  cmd.command = CM_SPLIT_MULTIPARTY;
		else
		  cmd.command = CM_COMMAND_UNKNOWN;
		break;
	    case CHLD_MOD_AddHld:
		if (clog_cmd->cmdPrm.sCHLD.act == CHLD_ACT_BuildMpty)
		  cmd.command = CM_RETRIEVE_MULTIPARTY;
		else
		  cmd.command = CM_COMMAND_UNKNOWN;
		break;
	    case CHLD_MOD_Ect:
		cmd.command = CM_ETC;
		break;
	    case CHLD_MOD_Ccbs:
	        TRACE_EVENT("CHLD_MOD_Ccbs");
		cmd.command = CM_CCBS;
		break;
	    case CHLD_MOD_HldActDial:
		cmd_hold = MFW_CM_HOLD_AND_DIAL;
		cmd.command = CM_HOLD;
		break;
	    default:
		break;
	}
   }
#endif		//FF_ATI  
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_AT_D	     |
+--------------------------------------------------------------------+


   PURPOSE :  Notify the outgoing call command.

*/

void cm_AT_D(T_ACI_CLOG *cmd)
{
    SHORT	       index;
    T_MFW_CM_CALL_STAT stat;
    UBYTE	       ton;
    UBYTE	       mode;
// Sep 01, 2006 REF:OMAPS00090555  x0039928  
// Fix : Allocate the memory for entry and findText from heap instead of stack.
#ifdef FF_MMI_PB_OPTIM	
    T_MFW_PHB_ENTRY    * entry =  (T_MFW_PHB_ENTRY*)mfwAlloc(sizeof(T_MFW_PHB_ENTRY));
    T_MFW_PHB_TEXT     * findText =  (T_MFW_PHB_TEXT*)mfwAlloc(sizeof(T_MFW_PHB_TEXT));
#else
    T_MFW_PHB_ENTRY    entry;
    T_MFW_PHB_TEXT     findText;
#endif

    TRACE_FUNCTION("cm_at_d()");

// Sep 01, 2006 REF:OMAPS00090555  x0039928  
#ifdef FF_MMI_PB_OPTIM
	#ifdef NO_ASCIIZ
  	  entry->name.len = 0;
	#else
	    memset (entry->name, '\0', MAX_LEN);
	#endif
	    memset (entry->number, '\0', MAX_LEN);
	    entry->index = 0;
	    entry->npi = 0;
	    entry->ton = 0;
	    entries.entry = entry;
#else
	#ifdef NO_ASCIIZ
 	   entry.name.len = 0;
	#else
	    memset (entry.name, '\0', MAX_LEN);
	#endif
	    memset (entry.number, '\0', MAX_LEN);
 	    entry.index = 0;
	    entry.npi = 0;
	    entry.ton = 0;
	    entries.entry = &entry;
#endif

    if (cmd->cmdPrm.sD.srcId == CMD_SRC_LCL)
    {
	out_call.call_id = cmd->cId;
	mfwStrncpy((char *)out_call.number, cmd->cmdPrm.sD.number, MFW_NUM_LEN);
	if (cc_stat.type == OUTGOING_CALL)
	{
	    mfwStrncpy((char *)cc_stat.number, cmd->cmdPrm.sD.number, MFW_NUM_LEN);
#ifdef NO_ASCIIZ
	    cc_stat.name.len = 0;
	    out_call.name.len = 0;
#else
	    cc_stat.name[0] = '\0';
	    out_call.name[0] = '\0';
#endif
	    if (cm_search_callStat(cmd->cId, &stat, &ton, &mode) == CM_OK)
	    {
		out_call.type = mode;
// Oct 10, 2006  DR: OMAPS00093720   x0039928 
// Fix: Set the cc_stat.ton info from out_call.ton
		cc_stat.ton  = out_call.ton;
		cc_stat.mode = (T_MFW_CM_CALL_TYPE)mode;
	    }

// Sep 01, 2006 REF:OMAPS00090555  x0039928  
#ifdef NO_ASCIIZ
	#ifdef FF_MMI_PB_OPTIM
	    mfwStrncpy((char *)findText->data, cmd->cmdPrm.sD.number, PHB_MAX_LEN);
	    findText->len = strlen(cmd->cmdPrm.sD.number);
	#else
	    mfwStrncpy((char *)findText.data, cmd->cmdPrm.sD.number, PHB_MAX_LEN);
	    findText.len = strlen(cmd->cmdPrm.sD.number);
	#endif
	    if (phb_find_entries (PHB_ADN,
				  &index,
				  MFW_PHB_NUMBER,
				  1,
			  #ifdef FF_MMI_PB_OPTIM
				  findText,
			  #else
			         &findText,
			  #endif
				  &entries) EQ MFW_PHB_OK)
	    {
		if (entries.num_entries)
		{
		    cc_stat.name.len = entries.entry[0].name.len;
		    memcpy(cc_stat.name.data,
			   entries.entry[0].name.data,
			   entries.entry[0].name.len);

		    out_call.name.len = entries.entry[0].name.len;
		    memcpy(out_call.name.data,
			   entries.entry[0].name.data,
			   entries.entry[0].name.len);
		}
	    }
#else
	    if (phb_find_entries (PHB_ADN,
				  &index,
				  MFW_PHB_NUMBER,
				  1,
				  cmd->cmdPrm.sD.number,
				  (T_MFW_PHB_LIST *)&entries) EQ MFW_PHB_OK)
	    {
		if (entries.num_entries)
		{
		    mfwStrncpy((char *)cc_stat.name, (char *)entries.entry[0].name, MFW_TAG_LEN);
		    mfwStrncpy((char *)out_call.name, (char *)entries.entry[0].name, PHB_MAX_LEN);
		}
	    }
#endif
	}

	if (cc_stat.type == REDIALING_CALL)
	{
#ifdef NO_ASCIIZ
	    out_call.name.len = cc_stat.name.len;
	    memcpy(out_call.name.data,
		   cc_stat.name.data,
		   cc_stat.name.len);
#else
	    mfwStrncpy((char *)out_call.name, (char *)cc_stat.name, MFW_TAG_LEN);
#endif
	    out_call.type = cc_stat.mode;
	}
// Sep 01, 2006 REF:OMAPS00090555  x0039928  
// Fix: Free the dynamic memory allocated if flag FF_MMI_PB_OPTIM is defined
	#ifdef FF_MMI_PB_OPTIM
      		mfwFree((U8*) entry, sizeof(T_MFW_PHB_ENTRY));
     		mfwFree((U8*) findText, sizeof(T_MFW_PHB_TEXT));
	#endif
	cm_signal(E_CM_MO_RES, &out_call);
    }
    else if (cmd->cmdPrm.sD.srcId < CMD_SRC_MAX)
    {
    	//x0pleela 03 Nov, 2006 DR: OMAPS00101158
    	//set the value of uOthersrc to TRUE as the source id is not LOCAL
    	mfw_set_OtherSrc_Value(TRUE);
	out_call.call_id = cmd->cId;
	mfwStrncpy((char *)out_call.number, (char *)cmd->cmdPrm.sD.number, MFW_NUM_LEN);
#ifdef NO_ASCIIZ
	out_call.name.len = 0;
#else
	out_call.name[0] = '\0';
#endif
	if (cm_search_callStat(cmd->cId, &stat, &ton, &mode) == CM_OK)
	{
	    out_call.type = mode;
	}
    	//x0pleela 03 Nov, 2006 DR: OMAPS00101158
    	//check the value of uOthersrc
    	//If the value is not set then do the phone book search operation
    	//Else do not perform  phone book search operation
    	//this is to avoid stack overflow probelm when the source id is not LOCAL
	if(!mfw_get_OtherSrc_Value())//x0pleela 3 nov 2006	
	{
// Sep 01, 2006 REF:OMAPS00090555  x0039928  
#ifdef NO_ASCIIZ
	#ifdef FF_MMI_PB_OPTIM
	mfwStrncpy((char *)findText->data, cmd->cmdPrm.sD.number, PHB_MAX_LEN);
	findText->len = strlen(cmd->cmdPrm.sD.number);
	#else
	mfwStrncpy((char *)findText.data, cmd->cmdPrm.sD.number, PHB_MAX_LEN);
	findText.len = strlen(cmd->cmdPrm.sD.number);
	#endif
	if (phb_find_entries (PHB_ADN,
			      &index,
			      MFW_PHB_NUMBER,
			      1,
			#ifdef FF_MMI_PB_OPTIM
			      findText,
			#else
			      &findText,
			#endif
			      &entries) EQ MFW_PHB_OK)
	{
	    if (entries.num_entries)
	    {
		out_call.name.len = entries.entry[0].name.len;
		memcpy(out_call.name.data,
		       entries.entry[0].name.data,
		       entries.entry[0].name.len);
	    }
	}
#else
	if (phb_find_entries (PHB_ADN,
			      &index,
			      MFW_PHB_NUMBER,
			      1,
			      cmd->cmdPrm.sD.number,
			      (T_MFW_PHB_LIST *)&entries) EQ MFW_PHB_OK)
	{
	    if (entries.num_entries)
	    {
		mfwStrncpy((char *)out_call.name, (char *)entries.entry[0].name, MFW_TAG_LEN);
	    }
	}
#endif
	} //3
// Sep 01, 2006 REF:OMAPS00090555  x0039928  
// Fix: Free the dynamic memory allocated if flag FF_MMI_PB_OPTIM is defined
	#ifdef FF_MMI_PB_OPTIM
      		mfwFree((U8*) entry, sizeof(T_MFW_PHB_ENTRY));
     		mfwFree((U8*) findText, sizeof(T_MFW_PHB_TEXT));
	#endif
	cm_signal(E_CM_MO_RES, &out_call);
    }
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_cvtCPItype	     |
+--------------------------------------------------------------------+


   PURPOSE :  Convert the type of

*/

T_MFW_CPI_TYPE cm_cvtCPItype(T_ACI_CPI_MSG msg)
{
    switch (msg)
	{
		case CPI_MSG_Setup: return CPI_TYPE_SETUP;
		case CPI_MSG_Disc:  return CPI_TYPE_DISC;
	case CPI_MSG_Alert: return CPI_TYPE_ALERT;
		case CPI_MSG_Proc:  return CPI_TYPE_PROC;
	case CPI_MSG_Sync:  return CPI_TYPE_SYNC;
	 case CPI_MSG_Conn: return CPI_TYPE_CONN;
	 /* CQ11676 Added new messages types in CPi indication. 17-02-04 MZ */
    #ifndef FF_2TO1_PS                      
		case CPI_MSG_Rls:  return CPI_TYPE_RELEASE;
	case CPI_MSG_Rjct:  return CPI_TYPE_REJECT;
    #endif                     
		default:		    return CPI_TYPE_UNKNOWN;
	}
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_cvtIBT	     |
+--------------------------------------------------------------------+


   PURPOSE :  Convert in-band tones indication

*/

T_MFW_CPI_IBT cm_cvtIBT(T_ACI_CPI_IBT ibt)
{
    switch (ibt)
	{
		case CPI_IBT_False: return CPI_IBT_FALSE;
		case CPI_IBT_True:  return CPI_IBT_TRUE;
		default:		    return CPI_IBT_UNKNOWN;
	}
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_cvtTCH	     |
+--------------------------------------------------------------------+


   PURPOSE :  Convert traffic channel indication

*/

T_MFW_CPI_TCH cm_cvtTCH(T_ACI_CPI_TCH tch)
{
    switch (tch)
	{
		case CPI_TCH_False: return CPI_TCH_FALSE;
		case CPI_TCH_True:  return CPI_TCH_TRUE;
		default:		    return CPI_TCH_UNKNOWN;
	}
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: rAT_PercentCPI      |
+--------------------------------------------------------------------+


   PURPOSE :  Call Progress Information

*/
// ADDED BY RAVI - FOR ACI CHANGES - 5-10-2005
#if (defined(FF_2TO1_PS)  &&  BOARD != 61)
void rAT_PercentCPI(SHORT cId,
		    T_ACI_CPI_MSG msgType,
		    T_ACI_CPI_IBT ibt,
		    T_ACI_CPI_TCH tch)

{
    cm_cpi.call_number = cId;
    cm_cpi.type        = cm_cvtCPItype(msgType);
    cm_cpi.inband      = cm_cvtIBT(ibt);
    cm_cpi.tch	       = cm_cvtTCH(tch);
//    cm_cpi.cause = CPI_CAUSE_MASK & cause; /* cq11676 Add cause information to the CPI indication 17-02-04 MZ */
    cm_signal(E_CM_CPI, &cm_cpi);
}
#else
void rAT_PercentCPI(SHORT cId,
		    T_ACI_CPI_MSG msgType,
		    T_ACI_CPI_IBT ibt,
		    T_ACI_CPI_TCH tch,
        	    USHORT        cause )        /*cq11676 Add cause parameter for cpi indication 17-02-04 MZ */
{
    cm_cpi.call_number = cId;
    cm_cpi.type        = cm_cvtCPItype(msgType);
    cm_cpi.inband      = cm_cvtIBT(ibt);
    cm_cpi.tch	       = cm_cvtTCH(tch);
    cm_cpi.cause = CPI_CAUSE_MASK & cause; /* cq11676 Add cause information to the CPI indication 17-02-04 MZ */
    cm_signal(E_CM_CPI, &cm_cpi);
}
#endif
// END OF ADDITION - RAVI - 5-10-2005.

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_error_code	     |
+--------------------------------------------------------------------+


   PURPOSE :

*/

UBYTE cm_error_code(T_ACI_AT_CMD cmd)
{
    ULONG err;

    err = qAT_ErrDesc();
    switch (ACI_ERR_DESC_CLASS( err ))
    {
	case ACI_ERR_CLASS_Cme:
	    break;
	case ACI_ERR_CLASS_Cms:
	    break;
	case ACI_ERR_CLASS_Ceer:
	    switch (ACI_ERR_DESC_NR( err ))
	    {
		case CEER_ACM_Max:	return CM_ACM_MAX;
		default:		return CM_ERROR;
	    }
//	    break;  // RAVI
	case ACI_ERR_CLASS_Ext:
	    switch (ACI_ERR_DESC_NR( err ))
	    {
		case EXT_ERR_FdnCheck:	return CM_CFDN_ERR;
		case EXT_ERR_BdnCheck:	return CM_CBDN_ERR;
		case EXT_ERR_OneCallOnHold:	return CM_2CALLS_ALREADY_ERR;
		//xvilliva 21421 - Handling err code returned by ACI.
#ifndef FF_2TO1_PS
	    case EXT_ERR_AlsLock:	return CM_ALS_ERR;
#endif
		default:		return CM_ERROR;
	    }
	  //  break;  // RAVI
	default:
	    break;
    }
    return CM_ERROR;
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	 MODULE  : MFW_CM	     |
| STATE   : code			 ROUTINE : cm_bc_parameter   |
+--------------------------------------------------------------------+


   PURPOSE : Notice bearer service parameter

*/
/*a0393213 warnings removal - parameter changed as a result of ACI enum change*/
void cm_bc_parameter(T_ACI_CRING_SERV_TYP type)
{
  TRACE_FUNCTION("cm_cvt_cringType()");

  switch (type)
  {
    case CRING_SERV_TYP_Voice:
      break;
    case CRING_SERV_TYP_Fax:
      in_call.bc1.conn_elem   = Transparent;
      break;
    case CRING_SERV_TYP_Sync:
      in_call.bc1.bearer_serv = Synch;
      in_call.bc1.conn_elem   = Transparent;
      break;
    case CRING_SERV_TYP_Async:
      in_call.bc1.bearer_serv = Asynch;
      in_call.bc1.conn_elem   = Transparent;
      break;
    case CRING_SERV_TYP_RelSync:
      in_call.bc1.bearer_serv = Synch;
      in_call.bc1.conn_elem   = NonTransparent;
      break;
    case CRING_SERV_TYP_RelAsync:
      in_call.bc1.bearer_serv = Asynch;
      in_call.bc1.conn_elem   = NonTransparent;
      break;
    default:
      break;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	 MODULE  : MFW_CM	     |
| STATE   : code			 ROUTINE : cm_build_callType |
+--------------------------------------------------------------------+


   PURPOSE :

*/
/*a0393213 warnings removal - parameters type changed as a result of ACI enum change*/
void cm_build_callType(T_ACI_CRING_SERV_TYP type1, T_ACI_CRING_SERV_TYP type2)
{
  if (type1 EQ CRING_SERV_TYP_Voice)
  {
    switch (type2)
    {
      case CRING_SERV_TYP_Sync:
      case CRING_SERV_TYP_Async:
      case CRING_SERV_TYP_RelSync:
      case CRING_SERV_TYP_RelAsync:
	in_call.type = VAD_VOICE;
	return;
      case CRING_SERV_TYP_Fax:
	in_call.type = VAF_VOICE;
	return;
      default:
	break;
    }
  }
  if (type2 EQ CRING_SERV_TYP_Voice)
  {
    switch (type1)
    {
      case CRING_SERV_TYP_Sync:
      case CRING_SERV_TYP_Async:
      case CRING_SERV_TYP_RelSync:
      case CRING_SERV_TYP_RelAsync:
	in_call.type = VAD_DATA;
	return;
      case CRING_SERV_TYP_Fax:
	in_call.type = VAF_FAX;
	return;
      default:
	break;
    }
  }
  in_call.type = NONE_TYPE;
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CM	     |
| STATE   : code			ROUTINE : cmCommand	     |
+--------------------------------------------------------------------+

  PURPOSE : handle mfw windows command

*/

static int cmCommand (U32 cmd, void *h)
{
    switch (cmd)
    {
	case MfwCmdDelete:		/* delete me		    */
	    if (!h)
		return 0;
	    cm_delete(h);
	    return 1;
	default:
	    break;
    }

    return 0;
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_command_abort    |
+--------------------------------------------------------------------+


   PURPOSE :

*/

T_MFW cm_command_abort(UBYTE command)
{
  TRACE_FUNCTION ("cm_command()");

  switch (command)
  {
    case CM_HOLD:
    case CM_SWAP:
    case CM_RETRIEVE:
    case CM_SWAP_MULTIPARTY:
    case CM_HOLD_MULTIPARTY:
    case CM_RETRIEVE_MULTIPARTY:
    case CM_BUILD_MULTIPARTY:
    case CM_SPLIT_MULTIPARTY:
    case CM_ETC:
    case CM_CCBS:
      sAT_Abort(CMD_SRC_LCL, AT_CMD_CHLD);
      break;

    case CM_DTMF_ACTIVE:
    case CM_DTMF_INACTIVE:
    case CM_DTMF_AUTO:
      sAT_Abort(CMD_SRC_LCL, AT_CMD_VTS);
      break;

    default:
      return FALSE;
  }
  return TRUE;
}

/* Marcus: CCBS: 13/11/2002: Start */
/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)     MODULE: MFW_CM                   |
| STATE  : code     ROUTINE: cm_ccbs_delete_all                      |
+--------------------------------------------------------------------+

   PURPOSE :    Delete all registered CCBS

   Returns:		T_MFW: CM_OK if successful, CM_ERROR if not

   Arguments:   None
*/
T_MFW    cm_ccbs_delete_all(void)
{
    T_MFW           res = CM_ERROR;     // The value to be returned by this function
    T_ACI_RETURN    aci_res;            // The value returned by sAT_PercentCCBS

    TRACE_FUNCTION("cm_ccbs_delete_all()");

    if (ccbs_op == CM_CCBS_NONE)
    {
        TRACE_EVENT("ccbs_op == CM_CCBS_NONE");
        /* No CCBS operation currently in progress */
#ifndef CCBS_TEST
        aci_res = sAT_PercentCCBS(CMD_SRC_LCL, 0);
#else
        aci_res = AT_EXCT;
#endif
        if (aci_res == AT_EXCT)
        {
            TRACE_EVENT("sAT_PercentCCBS returned AT_EXCT");
            ccbs_op = CM_CCBS_DELETE_ALL;
            res = CM_OK;
        }
        else
        {
            TRACE_EVENT_P1("sAT_PercentCCBS returned %d", aci_res);
        }
    }
    else
    {
        TRACE_EVENT_P1("ccbs_op is %d", ccbs_op);
    }

    return res;
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)     MODULE: MFW_CM                   |
| STATE  : code     ROUTINE: cm_ccbs_delete                          |
+--------------------------------------------------------------------+

   PURPOSE :    Delete a specific registered CCBS

   Returns:		T_MFW: CM_OK if successful, CM_ERROR if not

   Arguments:   SHORT idx: the index to the specific CCBS to be deleted
*/
T_MFW    cm_ccbs_delete(SHORT idx)
{
    T_MFW           res = CM_ERROR; // The value to be returned by this function
    T_ACI_RETURN    aci_res;        // The value returned by sAT_PercentCCBS

    TRACE_FUNCTION("cm_ccbs_delete()");
    TRACE_EVENT_P1("deleting CCBS %d", idx);

    if (ccbs_op == CM_CCBS_NONE)
    {
        TRACE_EVENT("ccbs_op == CM_CCBS_NONE");
        /* No CCBS operation currently in progress */
#ifndef CCBS_TEST
        aci_res = sAT_PercentCCBS(CMD_SRC_LCL, idx);
#else
        aci_res = AT_EXCT;
#endif
        if (aci_res == AT_EXCT)
        {
            TRACE_EVENT("sAT_PercentCCBS returned AT_EXCT");
            ccbs_op = CM_CCBS_DELETE;
            res = CM_OK;
        }
        else
        {
            TRACE_EVENT_P1("sAT_PercentCCBS returned %d", aci_res);
        }
    }
    else
    {
        TRACE_EVENT_P1("ccbs_op is %d", ccbs_op);
    }

    return res;
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)     MODULE: MFW_CM                   |
| STATE  : code     ROUTINE: cm_ccbs_list                            |
+--------------------------------------------------------------------+

   PURPOSE :    Delete all registered CCBS

   Returns:		T_MFW: CM_OK if successful, CM_ERROR if not

   Arguments:   None
*/
T_MFW    cm_ccbs_list(void)
{
    T_MFW           res = CM_ERROR; // The value to be returned by this function
    T_ACI_RETURN    aci_res;        // The value returned by qAT_PercentCCBS

    TRACE_FUNCTION("cm_ccbs_list()");

    if (ccbs_op == CM_CCBS_NONE)
    {
        /* No CCBS operation currently in progress */
        TRACE_EVENT("ccbs_op == CM_CCBS_NONE");
#ifndef CCBS_TEST
        aci_res = qAT_PercentCCBS(CMD_SRC_LCL);
#else
        aci_res = AT_EXCT;
#endif
        if (aci_res == AT_EXCT)
        {
            TRACE_EVENT("qAT_PercentCCBS returned AT_EXCT");
            ccbs_op = CM_CCBS_LIST;
            res = CM_OK;
        }
        else
        {
            TRACE_EVENT_P1("qAT_PercentCCBS returned %d", aci_res);
        }
    }
    else
    {
        TRACE_EVENT_P1("ccbs_op is %d", ccbs_op);
    }

    return res;
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)     MODULE: MFW_CM                   |
| STATE  : code     ROUTINE: cm_ccbs_accept_registration             |
+--------------------------------------------------------------------+

   PURPOSE :    Accepts the CCBS registration being offered

   Returns:		T_MFW: CM_OK if successful, CM_ERROR if not

   Arguments:   None
*/
T_MFW    cm_ccbs_accept_registration(void)
{
    T_MFW           res = CM_ERROR; // The value to be returned by this function
    T_ACI_RETURN    aci_res;        // The value returned by sAT_PlusCHLD

    TRACE_FUNCTION("cm_ccbs_accept_registration()");

    if (ccbs_op == CM_CCBS_NONE)
    {
        /* No CCBS operation currently in progress */
        TRACE_EVENT("ccbs_op == CM_CCBS_NONE");
        cm_set_cmd_status(CHLD_MOD_Ccbs, "");
	    cmd_hold = MFW_CM_CCBS_REGISTER;
#ifndef CCBS_TEST
	    aci_res = sAT_PlusCHLD(CMD_SRC_LCL, CHLD_MOD_Ccbs, NULL);
#else
	    aci_res = AT_EXCT;
#endif
        if (aci_res == AT_EXCT)
        {
            TRACE_EVENT("sAT_PlusCHLD returned AT_EXCT");
            ccbs_op = CM_CCBS_ACCEPT_REGISTRATION;
            res = CM_OK;
        }
        else
        {
            TRACE_EVENT_P1("sAT_PlusCHLD returned %d", aci_res);
        }
    }
    else
    {
        TRACE_EVENT_P1("ccbs_op is %d", ccbs_op);
    }

    return res;
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)     MODULE: MFW_CM                   |
| STATE  : code     ROUTINE: cm_ccbs_reject_registration             |
+--------------------------------------------------------------------+

   PURPOSE :    Rejects the CCBS registration being offered

   Returns:		T_MFW: CM_OK if successful, CM_ERROR if not

   Arguments:   None
*/
T_MFW    cm_ccbs_reject_registration(void)
{
    T_MFW           res = CM_ERROR; // The value to be returned by this function
    T_ACI_RETURN    aci_res;        // The value returned by sAT_H

    TRACE_FUNCTION("cm_ccbs_reject_registration()");

    if (ccbs_op == CM_CCBS_NONE)
    {
        /* No CCBS operation currently in progress */
        TRACE_EVENT("ccbs_op == CM_CCBS_NONE");
        cmd_hold = MFW_CM_CCBS_REGISTER;
#ifndef CCBS_TEST
        aci_res = sAT_H(CMD_SRC_LCL);
#else
        aci_res = AT_EXCT;
#endif
        if (aci_res == AT_EXCT)
        {
            TRACE_EVENT("sAT_H returned AT_EXCT");
            ccbs_op = CM_CCBS_REJECT_REGISTRATION;
            res = CM_OK;
        }
        else
        {
            TRACE_EVENT_P1("sAT_H returned %d", aci_res);
        }
    }
    else
    {
        TRACE_EVENT_P1("ccbs_op is %d", ccbs_op);
    }

    return res;
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)     MODULE: MFW_CM                   |
| STATE  : code     ROUTINE: cm_ccbs_accept_recall                   |
+--------------------------------------------------------------------+

   PURPOSE :    Accepts the CCBS recall being offered

   Returns:		T_MFW: CM_OK if successful, CM_BLACKIST or CM_ERROR if not

   Arguments:   UBYTE *number           - the number of the phone to call
                T_MFW_CM_CALL_TYPE type - the type of the call to be made
*/
T_MFW    cm_ccbs_accept_recall(UBYTE *number, T_MFW_CM_CALL_TYPE type)
{
    T_MFW           res = CM_ERROR; // The value to be returned by this function
    /*a0393213 compiler warnings removal -- removed def of search_res*/
    T_ACI_RETURN    aci_res;    // The value returned by sAT_PlusCHLD
    SHORT           call_number;// The call identity of any held call
    UBYTE           phone[MFW_NUM_LEN];   // Working copy of phone number
  //  CHAR			*rest;      // Parameter for ss_decode  // RAVI
//    T_KSD_SEQPARAM	para;       // Parameter for ss_decode  // RAVI
//    T_MFW_SS_RETURN	ss_res;     // The value returned by ss_decode  // RAVI

    TRACE_FUNCTION("cm_ccbs_accept_recall()");

    if (ccbs_op == CM_CCBS_NONE)
    {
        /* No CCBS operation currently in progress */
        TRACE_EVENT("ccbs_op == CM_CCBS_NONE");
        /* Ensure the number given is an ASCIIZ string thet is not too long */
        mfwStrncpy((char *)phone, (char *)number, MFW_NUM_LEN);
        TRACE_EVENT_P1("ccbs nr: %s", phone);

        /* Make sure that it is OK to call this number */
        if ((res = cm_check_mo_number(phone)) == CM_OK)
        {
            TRACE_EVENT("cm_check_mo_number returned CM_OK");
            /* Set up data as for mobile originated call */
            cm_set_call_mode(type);
            call_stat	   = OUTGOING_CALL;
            redial_count   = 0;
            cc_stat.type   = OUTGOING_CALL;
            currConnecting = 1;

            if ((cm_search_callId(CAL_STAT_Held, &call_number)) == CM_OK)
            {
                TRACE_EVENT("Held call: CHLD_MOD_RelActAndAcpt");
                /* There is a held call: release active call and accept callback */
                cm_set_cmd_status(CHLD_MOD_RelActAndAcpt, "");
#ifndef CCBS_TEST
                aci_res = sAT_PlusCHLD(CMD_SRC_LCL, CHLD_MOD_RelActAndAcpt, NULL);
#else
                aci_res = AT_EXCT;
#endif
            }
            else
            {
                TRACE_EVENT("No held call: CHLD_MOD_HldActAndAcpt");
                /* There is no held call: hold any active call and accept callback */
                cm_set_cmd_status(CHLD_MOD_HldActAndAcpt, "");
#ifndef CCBS_TEST
                aci_res = sAT_PlusCHLD(CMD_SRC_LCL, CHLD_MOD_HldActAndAcpt, NULL);
#else
                aci_res = AT_EXCT;
#endif
            }

            cmd_hold = MFW_CM_CCBS_RECALL;

            if (aci_res == AT_EXCT)
            {
                TRACE_EVENT("sAT_PlusCHLD returned AT_EXCT");
                ccbs_op = CM_CCBS_ACCEPT_RECALL;
                res = CM_OK;
            }
            else
            {
                /* The attempt to accept the callback failed */
                TRACE_EVENT_P1("sAT_PlusCHLD returned %d", aci_res);
                call_stat = MFW_CALL_FREE;
            }
        }
        else
        {
            /* It is not OK to call this number for some reason */
            TRACE_EVENT_P1("cm_check_mo_number returned %d", res);
        }
    }
    else
    {
        TRACE_EVENT_P1("ccbs_op is %d", ccbs_op);
    }

    return res;
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)     MODULE: MFW_CM                   |
| STATE  : code     ROUTINE: cm_ccbs_reject_recall                   |
+--------------------------------------------------------------------+

   PURPOSE :    Rejects the CCBS recall being offered

   Returns:		T_MFW: CM_OK if successful, CM_ERROR if not

   Arguments:   None
*/
T_MFW    cm_ccbs_reject_recall(void)
{
    T_MFW           res = CM_ERROR; // The value to be returned by this function
    T_ACI_RETURN    aci_res;        // The value returned by sAT_PlusCHLD

    TRACE_FUNCTION("cm_ccbs_reject_recall()");

    if (ccbs_op == CM_CCBS_NONE)
    {
        /* No CCBS operation currently in progress */
        TRACE_EVENT("ccbs_op == CM_CCBS_NONE");
        cm_set_cmd_status(CHLD_MOD_RelHldOrUdub, "");
        cmd_hold = MFW_CM_CCBS_RECALL;

#ifndef CCBS_TEST
        aci_res = sAT_H(CMD_SRC_LCL);
#else
        aci_res = AT_CMPL;
#endif
        if (aci_res == AT_CMPL)
        {
            TRACE_EVENT("sAT_H returned AT_CMPL");
            ccbs_op = CM_CCBS_REJECT_RECALL;
            res = CM_OK;
        }
        else
        {
            TRACE_EVENT_P1("sAT_H returned %d", aci_res);
        }
    }
    else
    {
        TRACE_EVENT_P1("ccbs_op is %d", ccbs_op);
    }

    return res;
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)     MODULE: MFW_CM                   |
| STATE  : code     ROUTINE: cm_ccbs_ok                              |
+--------------------------------------------------------------------+

   PURPOSE :    Called by rAT_OK for "OK" result code for %CCBS command

   Returns:		Nothing

   Arguments:   None
*/
void cm_ccbs_ok(void)
{
    TRACE_FUNCTION("cm_ccbs_ok()");

    memset((void *)&ccbs_info, 0, sizeof(ccbs_info));

    ccbs_info.operation = ccbs_op;
    ccbs_op             = CM_CCBS_NONE;
    ccbs_info.type      = CM_CCBS_OK;

    TRACE_EVENT_P2("E_CM_CCBS: op = %d: type = %d",
        ccbs_info.operation, ccbs_info.type);

    cm_signal(E_CM_CCBS, &ccbs_info);
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)     MODULE: MFW_CM                   |
| STATE  : code     ROUTINE: cm_ccbs_error                           |
+--------------------------------------------------------------------+

   PURPOSE :    Called by rAT_PlusCME for "+CME error" result code for %CCBS command

   Returns:		Nothing

   Arguments:   None
*/
void cm_ccbs_error(void)
{
    TRACE_FUNCTION("cm_ccbs_error()");

    memset((void *)&ccbs_info, 0, sizeof(ccbs_info));

    ccbs_info.operation = ccbs_op;
    ccbs_op             = CM_CCBS_NONE;
    ccbs_info.type      = CM_CCBS_ERROR;

    TRACE_EVENT_P2("E_CM_CCBS: op = %d: type = %d",
        ccbs_info.operation, ccbs_info.type);

    cm_signal(E_CM_CCBS, &ccbs_info);
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)     MODULE: MFW_CM                   |
| STATE  : code     ROUTINE: rAT_PercentCCBS                         |
+--------------------------------------------------------------------+

   PURPOSE :    Called by ACI for "%CCBS:" result code

   Returns:		Nothing

   Arguments:   T_ACI_CCBS_IND  indct   - type of %CCBS indication
                T_ACI_CCBS_STAT status  - status of %CCBS indication
                T_ACI_CCBS_SET *setting - optiona parameters of %CCBS indication
*/
void rAT_PercentCCBS( T_ACI_CCBS_IND  indct,
                        T_ACI_CCBS_STAT status,
                        T_ACI_CCBS_SET *setting )
{
    TRACE_FUNCTION("rAT_PercentCCBS()");
    TRACE_EVENT_P3("indct = %d: status = %d: setting = %p", indct, status, setting);

    memset((void *)&ccbs_info, 0, sizeof(ccbs_info));

    /* Fill in the fields of the E_CM_CCBS event structure */
    switch (indct)
    {
        case CCBS_IND_Possible:
        case CCBS_IND_Recall:
        case CCBS_IND_PossibilityTimedOut:
        case CCBS_IND_RecallTimedOut:

            /* An asynchronous %CCBS, not in response to a %CCBS command */
            TRACE_EVENT("Unsolicited event");
            ccbs_info.operation = CM_CCBS_ASYNC;
            break;

        default:
            TRACE_EVENT("Solicited event");
            ccbs_info.operation = ccbs_op;
            break;
    }

    ccbs_info.type      = CM_CCBS_INFO;
    ccbs_info.indct     = indct;
    ccbs_info.status    = status;

    TRACE_EVENT_P2("E_CM_CCBS: op = %d: type = %d",
        ccbs_info.operation, ccbs_info.type);
    TRACE_EVENT_P2(" indct = %d: status = %d",
        ccbs_info.indct, ccbs_info.status);

    if (setting == (T_ACI_CCBS_SET *) NULL)
    {
        /* No T_ACI_CCBS_SET structure given. Indicate in the idx field of ccbs_info */
        TRACE_EVENT("setting == NULL");
        ccbs_info.idx = 0;
    }
    else
    {
        /* T_ACI_CCBS_SET structure given. Fill in the remaining field of ccbs_info */
        TRACE_EVENT("setting != NULL");
        ccbs_info.idx = setting->idx;
        strncpy((char *) ccbs_info.number, (char *) setting->number, sizeof(ccbs_info.number)-1);
        ccbs_info.number[sizeof(ccbs_info.number)-1] = '\0';
        ccbs_info.ton = phb_cvtTon(setting->type.ton);
        ccbs_info.call_type = cm_callTyp_cvt(setting->class_type);
        strncpy((char *) ccbs_info.subaddr, (char *) setting->subaddr, sizeof(ccbs_info.subaddr)-1);
        ccbs_info.subaddr[sizeof(ccbs_info.subaddr)-1] = '\0';
        ccbs_info.alrtPtn = setting->alrtPtn;
        TRACE_EVENT_P3(" idx = %d: number = %s: ton = %d",
            ccbs_info.idx, ccbs_info.number, ccbs_info.ton);
        TRACE_EVENT_P3(" call_type = %d: subaddr = %s: alrtPtn = %d",
            ccbs_info.call_type, ccbs_info.subaddr, ccbs_info.alrtPtn);
    }

    cm_signal(E_CM_CCBS, &ccbs_info);
}

/* Marcus: CCBS: 13/11/2002: End */

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_error_dial	     |
+--------------------------------------------------------------------+


   PURPOSE : The call setup is not successful

*/

void cm_error_dial(T_ACI_CME_ERR err)
{
    TRACE_FUNCTION("cm_error_dial()");

    switch (err)
    {
        case CME_ERR_OpNotAllow: /* the active call can not be hold */
            /* allow a new outgoing */
            call_stat = MFW_CALL_FREE;
            call_type = MFW_CALL_FREE;
            cmd.command = CM_HOLD;
            if (cm_search_callId(CAL_STAT_Active, &cmd.call_number) NEQ CM_OK)
              cmd.call_number = -1;
            cmd.result = NotPresent;
            cm_signal(E_CM_COMMAND, &cmd);
            break;

        /*SPR#2235 - DS - If handset is powered-up without a SIM and an emergency
         *call is attempted, the UK networks will reject the call attempt. Send a signal to
         *the BMI to enable it to destroy the call and show an appropriate error message.
         *
         *The ACI calls rAT_PlusCME() - then rAT_PlusCME() calls this funct - for this SIM
         *failure but the ACI doesn't supply the callId. Without the callId, the MFW/BMI
         *cannot close down the call properly. The ACI interface will need to be changed
         *so that it supplies the callId (if there is an active call) when this failure occurs.
         */
        case CME_ERR_SimFail:
            /* allow a new outgoing */
            call_stat = MFW_CALL_FREE;
            call_type = MFW_CALL_FREE;

            /*SPR#2235 - DS - Fake the callId. Workaround needed until the ACI interface is changed */
            cm_error_connect(1);
            break;

        default:
            TRACE_EVENT_P1("Error code: %d. No signal sent.", err);
            break;
    }
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_data_connect     |
+--------------------------------------------------------------------+


   PURPOSE : Fax/Data connect

*/

void cm_data_connect(SHORT callId)
{
  SHORT call_number;

  TRACE_FUNCTION("cm_data_connect()");

  call_number = callId;
  cm_signal(E_CM_CONNECT, &call_number);
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_data_connect     |
+--------------------------------------------------------------------+


   PURPOSE : Fax/Data connect

*/

void cm_wap_data_connected(void)
{
 // SHORT call_number;  // RAVI

  TRACE_FUNCTION("cm_wap_data_connected()");

  cm_signal(E_CM_WAP_CALL_CONNECTED, NULL);
}
/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_deflect_incoming_call     |
+--------------------------------------------------------------------+


   PURPOSE : try to deflect a call using AT command CTFR

*/
T_MFW cm_deflect_incoming_call(char* number)
{	T_ACI_RETURN result;
	T_ACI_TOA type;
	T_ACI_TOS subaddr_type;

	type.ton = TON_Unknown;
	type.npi = NPI_Unknown;

	subaddr_type.tos = TOS_NotPresent;
	subaddr_type.oe = OE_NotPresent;

	result = sAT_PlusCTFR(CMD_SRC_LCL, number, &type, NULL, &subaddr_type);

	if ((result == AT_BUSY) || (result == AT_FAIL))
		return FALSE;
	return TRUE;

}
/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_ok_CTFR     |
+--------------------------------------------------------------------+


   PURPOSE : sends call deflect OK event to MMI

*/
void cm_ok_CTFR(void)
{	BOOL status = TRUE;

	cm_signal(E_CM_CALL_DEFLECT_STATUS, &status);

}
/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_error_CTFR     |
+--------------------------------------------------------------------+


   PURPOSE : Sends call deflect failed event to MMI

*/
void cm_error_CTFR(void)
{
	BOOL status = FALSE;

	cm_signal(E_CM_CALL_DEFLECT_STATUS, &status);
}
#ifdef CCBS_TEST
/* Marcus: CCBS testing: 20/11/2002: Start */
static void strlower(char *string)
{
    while(*string)
    {
        *string = (char)tolower((int) *string);
        string++;
    }
}

static BOOL getCmdId(char *token, T_ACI_AT_CMD *cmdId)
{
    BOOL cmdIdOk = FALSE;
    TRACE_FUNCTION("getCmdId()");
    if (token)
    {
        strlower(token);
        TRACE_EVENT_P1("token: %s", token);
        if (strstr(token, "ccbs"))
        {
            *cmdId = AT_CMD_CCBS;
            cmdIdOk = TRUE;
        }
        else if (strstr(token, "chld"))
        {
            *cmdId = AT_CMD_CHLD;
            cmdIdOk = TRUE;
        }
        else if (strcmp("h", token) == 0)
        {
            *cmdId = AT_CMD_H;
            cmdIdOk = TRUE;
        }
        else if ((*cmdId = atoi(token)))
        {
            cmdIdOk = TRUE;
        }
    }
    else
    {
        TRACE_EVENT("No token");
    }

    return cmdIdOk;
}
#endif

#if defined MFW & defined TI_PS_FF_AT_P_CMD_MMITEST
void rAT_PercentMMITEST(char *param)
{
    char work[80];
#ifdef CCBS_TEST         /*a0393213 compiler warning removal. 'token' removed conditionally*/
    char *token;    // Current token obtained from strtok
#endif
    TRACE_FUNCTION("rAT_PercentMMITEST()");

    strncpy(work, param, sizeof(work)-1);
    work[sizeof(work)-1] = '\0';

#ifdef CCBS_TEST
    if ((token = strtok(work, ",()")) != NULL)
#else
    if (strtok(work, ",()") != NULL)
#endif
    {
#ifdef CCBS_TEST
        T_ACI_AT_CMD cmdId;
        strlower(token);
        TRACE_EVENT_P1("token: %s", token);

        if (strcmp("ok", token) == 0)
        {
            TRACE_EVENT("OK");
            if ((token = strtok(NULL, ",()")) != NULL)
            {
                if (getCmdId(token, &cmdId) == TRUE)
                {
                    TRACE_EVENT_P1("rAT_OK(cmdId = %d", cmdId);
                    rAT_OK(cmdId);
                }
                else
                {
                    TRACE_EVENT("Unknown cmdId");
                }
            }
            else
            {
                TRACE_EVENT("No cmdId token");
            }
        }
        else if (strcmp("error", token) == 0)
        {
            TRACE_EVENT("ERROR");
            if ((token = strtok(NULL, ",()")) != NULL)
            {
                if (getCmdId(token, &cmdId) == TRUE)
                {
                    T_ACI_CME_ERR err;
                    if ((token = strtok(NULL, ",()")) != NULL)
                    {
                        err = atoi(token);
                        rAT_PlusCME(cmdId, err);
                    }
                    else
                    {
                        TRACE_EVENT("No err token");
                    }
                }
            }
            else
            {
                TRACE_EVENT("No cmdId token");
            }
        }
        else if (strcmp("info", token) == 0)
        {
            T_ACI_CCBS_IND  indct = CCBS_IND_NotPresent;
            T_ACI_CCBS_STAT status = CCBS_STAT_NotPresent;
            T_ACI_CCBS_SET  setting, *pSetting = NULL;
            TRACE_EVENT("INFO");
            if ((token = strtok(NULL, ",()")) != NULL)
            {
                strlower(token);
                TRACE_EVENT_P1("token: %s", token);
                if (strstr(token, "notpresent"))
                    indct = CCBS_IND_NotPresent;
                else if (strstr(token, "possibility"))
                    indct = CCBS_IND_PossibilityTimedOut;
                else if (strstr(token, "possible"))
                    indct = CCBS_IND_Possible;
                else if (strstr(token, "registered"))
                    indct = CCBS_IND_Registered;
                else if (strstr(token, "recallt"))
                    indct = CCBS_IND_RecallTimedOut;
                else if (strstr(token, "recall"))
                    indct = CCBS_IND_Recall;
                else if (strstr(token, "irgt"))
                    indct = CCBS_IND_IrgtResult;
            }

            if ((token = strtok(NULL, ",()")) != NULL)
            {
                strlower(token);
                TRACE_EVENT_P1("token: %s", token);
                if (strstr(token, "notpresent"))
                    status = CCBS_STAT_NotPresent;
                else if (strstr(token, "notprov"))
                    status = CCBS_STAT_NotProvisioned;
                else if (strstr(token, "prov"))
                    status = CCBS_STAT_Provisioned;
                else if (strstr(token, "active"))
                    status = CCBS_STAT_Active;
            }

            setting.idx = 0;
            if ((token = strtok(NULL, ",()")) != NULL)
            {
                TRACE_EVENT_P1("token: %s", token);
                setting.idx = atoi(token);
            }

            if (setting.idx != 0)
            {
                pSetting = &setting;
                strcpy(setting.number, "01317180538");
                setting.type.ton = TON_National;
                setting.type.npi = NPI_IsdnTelephony;
                setting.subaddr[0] = '\0';
                setting.satype.tos = TOS_NotPresent;
                setting.satype.oe = OE_NotPresent;
                setting.class_type = CLASS_Vce;
                setting.alrtPtn = ALPT_Level_0;

                if ((token = strtok(NULL, ",()")) != NULL)
                {
                    TRACE_EVENT_P1("token: %s", token);
                    strncpy(setting.number, token, sizeof(setting.number)-1);
                    setting.number[sizeof(setting.number)-1] = '\0';
                }

                if ((token = strtok(NULL, ",()")) != NULL)
                {
                    TRACE_EVENT_P1("token: %s", token);
                    setting.type.ton = TON_International;
                }
            }

            TRACE_EVENT_P3("rAT_PercentCCBS(%d, %d, %p)", indct, status, pSetting);
            if (pSetting != NULL)
            {
                TRACE_EVENT_P3("idx = %d: number = %s: ton = %d",
                    setting.idx, setting.number, setting.type.ton);
            }
            rAT_PercentCCBS(indct, status, pSetting);
        }
        else
        {
            TRACE_EVENT("Unrecognised token");
        }
#endif
    }
    else
    {
        TRACE_EVENT("No token");
    }
}
/* Marcus: CCBS testing: 20/11/2002: End */
#endif /* defined MFW & defined TI_PS_FF_AT_P_CMD_MMITEST */

/* SPR#1985 - SH - Added this function */

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_tty_enable    |
+--------------------------------------------------------------------+


   PURPOSE : Receipt of unsolicited TTY information

*/

void rAT_PercentCTYI(T_ACI_CTTY_NEG neg, T_ACI_CTTY_TRX trx)
{
	T_MFW_CM_CTYI res;

	TRACE_FUNCTION("rAT_PercentCTYI()");
#ifdef MMI_LINT_WARNING_REMOVAL  /*a0393213 lint warnings removal*/
	currCmd = AT_CMD_NONE;
#endif
	switch(neg)
	{
		case CTTY_NEG_None:
			res.neg =  CTYI_NONE;
			break;
		case CTTY_NEG_Request:
			res.neg = CTYI_REQUEST;
			break;
		case CTTY_NEG_Reject:
			res.neg = CTYI_REJECT;
			break;
		case CTTY_NEG_Grant:
			res.neg = CTYI_GRANT;
			break;
	}

	cm_signal(E_CM_CTYI, &res);

	return;
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_tty_enable    |
+--------------------------------------------------------------------+


   PURPOSE : Request ACI to switch TTY on or off

*/

void cm_tty_enable(UBYTE enable)
{
	/*a0393213 compiler warnings removal -- removed ret*/

/* Mar 27, 2006    REF:ER OMAPS00071798 */
    #ifdef NEPTUNE_BOARD 
        U8 audioMode = AUDIO_MODE_TEXT; 
    #endif

	TRACE_FUNCTION("cm_tty_enable()");

	/* SPR#1985 - SH - Now use proper ACI functions.  */

  /* sAT_PercentCTTY() is changed bec the audio mode info is needed in lower layer,
     for now just use default value when calling the function, if we want to have 
     the same functionality in BMI/MFW as in ATI, BMI/MFW needs to be updated */
#ifdef NEPTUNE_BOARD
    if (flash_read() >= EFFS_OK)
    {
        audioMode = FFS_flashData.tty_audio_mode;
    }

#endif

/* Mar 27, 2006    REF:ER OMAPS00071798 */
#ifdef NEPTUNE_BOARD 
#ifdef CTTY_AUDIO_MODE
	if (enable)
		sAT_PercentCTTY(CMD_SRC_LCL, CTTY_MOD_Enable, CTTY_REQ_On, audioMode);
	else
		sAT_PercentCTTY(CMD_SRC_LCL, CTTY_MOD_Enable, CTTY_REQ_Off, audioMode);
#else
	if (enable)
		sAT_PercentCTTY(CMD_SRC_LCL, CTTY_MOD_Enable, CTTY_REQ_On );
	else
		sAT_PercentCTTY(CMD_SRC_LCL, CTTY_MOD_Enable, CTTY_REQ_Off );
#endif

#else
	if (enable)
		sAT_PercentCTTY(CMD_SRC_LCL, CTTY_MOD_Enable, CTTY_REQ_On );
	else
		sAT_PercentCTTY(CMD_SRC_LCL, CTTY_MOD_Enable, CTTY_REQ_Off );
#endif


	/* SPR#1985 - SH - Check that TTY status has changed and inform MMI  */

	cm_tty_query();

	return;
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CM		     |
| STATE  : code 			ROUTINE: cm_tty_query    |
+--------------------------------------------------------------------+


   PURPOSE : Query status of TTY, inform MMI. */
   /*SPR#1985 - SH - Added this function  */

void cm_tty_query(void)
{
	T_MFW_CM_CTYI res;
//	T_ACI_RETURN ret;  // RAVI
	T_ACI_CTTY_MOD mode;
	T_ACI_CTTY_REQ req;
	T_ACI_CTTY_STAT stat;
	T_ACI_CTTY_TRX trx;

/* Mar 27, 2006    REF:ER OMAPS00071798 */
#ifdef NEPTUNE_BOARD 
      U8 audio_mode;
#endif

	TRACE_FUNCTION("cm_tty_query()");

/* Mar 27, 2006    REF:ER OMAPS00071798 */
#ifdef NEPTUNE_BOARD 
#ifdef CTTY_AUDIO_MODE
        qAT_PercentCTTY(CMD_SRC_LCL, &mode, &req, &stat, &trx, &audio_mode);
#else
		qAT_PercentCTTY(CMD_SRC_LCL, &mode, &req, &stat, &trx);
#endif
#else
        qAT_PercentCTTY(CMD_SRC_LCL, &mode, &req, &stat, &trx);
#endif

	res.neg = CTYI_REJECT;

	if (req==CTTY_REQ_On)
	{
		if (req)
			res.neg = CTYI_GRANT;
	}

	cm_signal(E_CM_CTYI, &res);

	return;
}

// Feb 03, 2005	REF: CRR 25960   x0012850
// Description: Call Hold: While one call in Active and other in Held by swapping either of phone is not audible

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework        	MODULE: MFW_CM		                               |
| STATE  : code 			              ROUTINE: cm_setSpeechMode                       |
+--------------------------------------------------------------------+


   PURPOSE : MFW function to call psaCC_setSpeechMode() to attach user connection.

*/
void cm_setSpeechMode(void)
{
	TRACE_FUNCTION("cm_setSpeechMode()");
	psaCC_setSpeechMode();
	return;
}


//   Sep 12, 2005   REF: MMI-SMR-34085 x0021334
//   Description: RE: Add +CDIP command - MMI Additions
//   Solution: Dummy implementation of function "rAT_PlusCDIP()" is done to enable successful compilation.

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework        	MODULE: MFW_CM		                               |
| STATE   : code                                ROUTINE : rAT_PlusCDIP                              |
+--------------------------------------------------------------------+

  PURPOSE : Stub implementation
*/

GLOBAL void rAT_PlusCDIP( CHAR   * number,
                             T_ACI_TOA       * type,
                             CHAR            * subaddr,
                             T_ACI_TOS       * satype)
{
	TRACE_FUNCTION ("rAT_PlusCDIP()");
}


/*June 15, 2006    DR:OMAPS00079492    x0043642 */
/*******************************************************************************

 $Function:     set_cmdHold

 $Description: Sets the value of the static variable cmd_hold.

 $Returns:      void   

 $Arguments: command
*******************************************************************************/
void set_cmdHold(int command)
{
    TRACE_FUNCTION("set_cmdHold()");
    TRACE_EVENT_P1("Old cmd_hold= %d", cmd_hold);
    cmd_hold = command;
    TRACE_EVENT_P1("New cmd_hold= %d", cmd_hold);
}





/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework        	MODULE: MFW_CM		                               |
| STATE   : code                                ROUTINE : mfw_get_ALS_type                      |
+--------------------------------------------------------------------+

  PURPOSE : this function returns the type of call (voice call or data call etc...)
*/
//x0pleela 25 May, 2006  DR: OMAPS00070657
#ifdef FF_CPHS
T_MFW mfw_get_ALS_type()
{
	return in_call.type;
}
#endif


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework        	MODULE: MFW_CM		                               |
| STATE   : code                                ROUTINE : mfw_set_OtherSrc_Value                      |
+--------------------------------------------------------------------+

  PURPOSE : this function sets the value of uOtherSrc to either TRUE or FALSE
*/
//x0pleela 03 Nov, 2006  DR: OMAPS00101158
GLOBAL void mfw_set_OtherSrc_Value(UBYTE otherSrc)
{
	TRACE_FUNCTION("mfw_set_OtherSrc_Value");
	uOtherSrc = otherSrc;
}
/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework        	MODULE: MFW_CM		                               |
| STATE   : code                                ROUTINE : mfw_get_OtherSrc_Value                      |
+--------------------------------------------------------------------+

  PURPOSE : this function is to get the value of uOtherSrc
*/
//x0pleela 03 Nov, 2006  DR: OMAPS00101158
GLOBAL UBYTE mfw_get_OtherSrc_Value(void)
{
	TRACE_FUNCTION("mfw_get_OtherSrc_Value");
	return uOtherSrc;
}


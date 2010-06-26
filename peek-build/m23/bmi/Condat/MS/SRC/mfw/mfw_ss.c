/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_ss.c        $|
| $Author:: Vo                          $Revision::  1              $|
| CREATED: 25.1.99                      $Modtime:: 24.03.00 9:48    $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_SS

   PURPOSE : This modul contains the functions for supplementary management.


   $History:: mfw_ss.c                                              $

     	Feb 27, 2007 ER: OMAPS00113891 x0pleela
 	Description:[ACI] MTC related call forwarded SS notification not differentiable on 
 				BAT/ATI level
 	Solution: In function ss_cvtCssuCodes(), new enum CSSU_CODE_IncCallForwarded
 			from ACI is handled and typecasted to MFW_SS_MT_CALL_FORWARDED 
 			for MMI to handle this event  
 				
   	Aug 28, 2006  DR: OMAPS00083503 x0pleela
	Description: Result of ss_check_ss_string may be ignored
	Solution:	rat_percentKSIR(): Removed the hardcoded dcs value "MFW_ASCII"
			and sending ss signal E_SS_USSD_REQ or E_SS_USSD_CNF based on user 
			action required or not

	Jul 04, 2006	REF: OMAPS00079722 	x0023848
	Description: MS displays garbage instead of appropriate user indication  when network 
       release with problem code  "unrecognized operation" or a cause "facility rejected".
	Solution: Added a NULL Check for the result string of the USSD command that is being received.
	
    Jun 30, 2006     REF : OMAPS00083503    R.Prabakar
    Description : When USSD string is sent, MMI shows requesting screen forever
    Solution     : Some cases in rAT_PlusCUSD weren't handled. They are handled now.
    
	Dec 16, 2005	REF: OMAPS00053316	x0018858
	Description: E-Sample 2.58 compiler code crashes when accessing the Voicemail service in the Messages
	Solution: Added a NULL Check for the USSD pointer that is being received.
	
	Jul 05, 2005	REF: CRR 22565	a0876501
	Description: IMEI display on handset through atd*#06# was swapped.
	Solution: Byte swapping corrected for IMEI decoding. 

	Mar 30, 2005	REF: CRR 29986	xpradipg
	Description: Optimisation 1: Removal of unused variables and dynamically
	allocate/ deallocate mbndata
	Solution: unused variables are removed 

	Apr 06 2005		REF: CRR 29989	xpradipg
	Description:	Optimisation 3: replace the static global data with dynamic
					allocation / deallocation
	Solution:		The static global variables are dynamically allocated and
					deallocated
	
	Aug 16, 2004    REF: CRR 24323   Deepa M.D
	Bug:Clenup of sprintf used for tracing
	Fix:Replace the char buf[]; sprintf (buf, "...", ...); TRACE_EVENT (buf); statements by TRACE_EVENT_PX

	Jul 15, 2004 REF: CRR 13873 xkundadu
	Description: Make an incoming call, the end user pressing 0+send will 
			    pick up the call. Instead, 0+send should reject the incoming 
			    call. 
	Solution:     Added the handling of 0/1/2 SEND for a waiting call.
			    Modified the ss_decode() function in the 
			    mfw_ss.c file to search the sequential table in the case of 
			    waiting call also. Made changes in mfw_cm.c file also
 *
 * *****************  Version 24  *****************
 * User: Vo           Date: 24.03.00   Time: 10:18
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * Bug fix: result of IMEI
 * New: notice PIN type of SS string for PIN change/PIN unblock
 *
 * *****************  Version 23  *****************
 * User: Vo           Date: 10.01.00   Time: 16:09
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * new function ss_exit()
 *
 * *****************  Version 22  *****************
 * User: Vo           Date: 10.01.00   Time: 13:32
 * Updated in $/GSM/Condat/MS/SRC/MFW
 *
 * *****************  Version 21  *****************
 * User: Ak           Date: 10.01.00   Time: 13:14
 * Updated in $/GSM/Condat/MS/SRC/MFW
 *
 * *****************  Version 20  *****************
 * User: Ak           Date: 21.10.99   Time: 11:56
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * Why: Improvement
 * What: Passing of network basic service codes instead of GSM 2.30
 * codes for SS control string results and invocations
 *
 * *****************  Version 19  *****************
 * User: Ak           Date: 29.09.99   Time: 8:37
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * Why: bug fix
 * What: strcpy do not check a NULL pointer argument, corrected
 *
 * *****************  Version 18  *****************
 * User: Ak           Date: 28.09.99   Time: 14:16
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * Why: bug fix
 * What: strcpy do not check a NULL pointer argument, corrected
 *
 * *****************  Version 17  *****************
 * User: Es           Date: 16.09.99   Time: 17:25
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * include SENSEI patch 13/09/99
 * two minor fixes
 *
 * *****************  Version 16  *****************
 * User: Vo           Date: 12.09.99   Time: 14:16
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * New: USSD handle
 *
 * *****************  Version 15  *****************
 * User: Vo           Date: 30.08.99   Time: 12:08
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * SS string for PIN unblock
 *
 * *****************  Version 14  *****************
 * User: Vo           Date: 9.08.99    Time: 16:53
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * SS result from network
 * present IMEI
 * different handling of network password and PIN1/PIN2 password
 *
 * *****************  Version 13  *****************
 * User: Vo           Date: 21.07.99   Time: 14:44
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 12  *****************
 * User: Vo           Date: 16.07.99   Time: 10:39
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 11  *****************
 * User: Vo           Date: 9.07.99    Time: 14:56
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 1  *****************
 * User: Vo           Date: 10.02.99   Time: 12:41
 * Created in $/GSM/DEV/MS/SRC/MFW

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

#include "message.h"
#include "prim.h"
#include "aci_cmh.h"

#include "cus_aci.h"
#include "ksd.h"
#include "prim.h"
#ifndef PCM_2_FFS
#include "pcm.h"
#else
#include "prim.h"
#include "Gsm.h"
#endif


#include "psa.h"
#include "psa_sim.h"

#include "mfw_mfw.h"
#include "mfw_nm.h"
#include "mfw_sim.h"
#include "mfw_sima.h"
#include "mfw_nmi.h"
#include "mfw_simi.h"
#include "mfw_nma.h"
#include "mfw_phb.h"
#include "mfw_cm.h"
#include "mfw_cmi.h"
#include "mfw_ss.h"
#include "mfw_ssi.h"
#include "mfw_win.h"
#include "mfw_phbi.h"

#include <string.h>


/*************** Variables ******************************************/
// 	Mar 30, 2005	REF: CRR 29986	xpradipg
//	remove unused variables
#ifndef FF_MMI_OPTIM
static T_MFW_SS_SUPORT    ss_suport;
#endif
//	Apr 06 2005		REF: CRR 29989	xpradipg
//	This scope of the variable lies within a function hence it is made as local
#ifndef FF_MMI_OPTIM
static T_MFW_SS_NOTIFY    notify;
#endif

//	Apr 06 2005		REF: CRR 29989	xpradipg
//	The below static globals are dynamically allocated in rAT_PercentKSIR where
//	the data is populated for the supplementary services response and 
//	deallocated once the ss_signal function is executed where in the contents
//	are copied onto another variable
#ifdef FF_MMI_OPTIM
T_MFW_SS_CF_CNF    *cf_cnf = NULL;
T_MFW_SS_CB_CNF    *cb_cnf = NULL;
T_MFW_SS_CLI_CNF   *cli_cnf = NULL;
T_MFW_SS_CW_CNF    *cw_cnf = NULL;
T_MFW_SS_PW_CNF    *pw_cnf = NULL;
T_MFW_IMEI		  *imei_info = NULL;
#else
static T_MFW_SS_CF_CNF    cf_cnf;
static T_MFW_SS_CB_CNF    cb_cnf;
static T_MFW_SS_CLI_CNF   cli_cnf;
static T_MFW_SS_CW_CNF    cw_cnf;
static T_MFW_SS_PW_CNF    pw_cnf;
static T_MFW_SS_RES       ss_res;
static T_MFW_IMEI		  imei_info;
#endif
static T_MFW_SS_USSD      ussd_info;
// 	Mar 30, 2005	REF: CRR 29986	xpradipg
//	remove unused variables
#ifndef FF_MMI_OPTIM
static U8                 clip_status;
static U8                 colp_status;
static U8                 clir_status;
static U8                 clir_prefix;
static U8                 colr_status;
#endif
static U8                 clck_flag;
static U8				  pwd_flag;   /* 1 for change PIN */



#define CF_LST_SIZE (sizeof(T_MFW_FEATURE)  *MFW_MAX_FEAT_NR)
#define CB_LST_SIZE (sizeof(T_MFW_TELECOM)  *MFW_MAX_TELE_NR)
#define CW_LST_SIZE (sizeof(T_MFW_SRV_GROUP)*MFW_MAX_SRV_NR)

#define MAX_LST_BUF (MAXIMUM(MAXIMUM(CF_LST_SIZE,\
                                     CB_LST_SIZE),\
                             CW_LST_SIZE))

#define LANG_RESET "*#0" /*mc, SPR 1111 beginning of language reset pseudo-SS string*/
#define TTY_NEXTCALL_ON "*55#" /* SPR#1352 - SH - Pseudo-SS string to switch on TTY on next call */
#define TTY_NEXTCALL_OFF "#55#" /* SPR#1352 - SH - Pseudo-SS string to switch off TTY on next call */

static ULONG ssLstBuf[MAX_LST_BUF/sizeof(ULONG)];
EXTERN MfwHdr * current_mfw_elem;

//	July 09, 2005   REF : MMI-22565 - a0876501
extern UBYTE cmhSS_getCdFromImei (T_ACI_IMEI* imei);

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SS               |
| STATE  : code                         ROUTINE: ss_mfw_init         |
+--------------------------------------------------------------------+


   PURPOSE :

*/

void ss_mfw_init(void)
{
// 	Mar 30, 2005	REF: CRR 29986	xpradipg
//	remove reundant code since the values assigned here are never referenced

#ifndef PCM_2_FFS

 #ifndef FF_MMI_OPTIM 
  EF_MSCAP mscap;
  EF_MSSUP mssup;
  EF_MSSET msset;
  UBYTE    version;

  /* Read the Mobile Capabilities from EEPROM */

  if (pcm_ReadFile((UBYTE *)EF_MSCAP_ID,
	               SIZE_EF_MSCAP,
				   (UBYTE *)&mscap,
				   &version) EQ PCM_OK)
  {
    ss_suport.speech = (UBYTE)FldGet(mscap.chnMode,spchSupV1);
    ss_suport.data   = (UBYTE)FldGet(mscap.datCap1,datSup);
    ss_suport.asyn   = (UBYTE)FldGet(mscap.datCap1,AsySup);
    ss_suport.syn    = (UBYTE)FldGet(mscap.datCap1,NTSynSup);
  }

  if (pcm_ReadFile((UBYTE *)EF_MSSUP_ID,
	               SIZE_EF_MSSUP,
				   (UBYTE *)&mssup,
				   &version) EQ PCM_OK)
  {
    ss_suport.aoc    = (UBYTE)FldGet(mssup.feat1,AoC);
    ss_suport.dtmf   = (UBYTE)FldGet(mssup.feat1,DTMF);
    ss_suport.cf     = (UBYTE)FldGet(mssup.feat1,CF);
    ss_suport.cb     = (UBYTE)FldGet(mssup.feat1,CB);
    ss_suport.ussd   = (UBYTE)FldGet(mssup.feat1,USSD);
    ss_suport.etc    = (UBYTE)FldGet(mssup.feat1,ETC);
  }

  if (pcm_ReadFile((UBYTE *)EF_MSSET_ID,
	               SIZE_EF_MSSET,
				   (UBYTE *)&msset,
				   &version) EQ PCM_OK)
  {
    ss_suport.clire   = (UBYTE)FldGet(msset.misc,clir);
    ss_suport.clipr   = (UBYTE)FldGet(msset.misc,clip);
  }
#endif
#endif
  pwd_flag = 0;
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SS               |
| STATE  : code                         ROUTINE: ss_exit             |
+--------------------------------------------------------------------+


   PURPOSE :

*/

void ss_exit(void)
{
  TRACE_FUNCTION("ss_exit()");
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SS               |
| STATE  : code                         ROUTINE: ss_create           |
+--------------------------------------------------------------------+


   PURPOSE :  create event for supplementary management

*/

T_MFW_HND ss_create(T_MFW_HND hWin, T_MFW_EVENT event, T_MFW_CB cbfunc)
{
  T_MFW_HDR *hdr;
  T_MFW_SS  *ss_para;
  MfwHdr * insert_status =0;
  
  TRACE_FUNCTION("ss_create()");

  hdr      = (T_MFW_HDR *) mfwAlloc(sizeof (T_MFW_HDR));
  ss_para  = (T_MFW_SS *) mfwAlloc(sizeof (T_MFW_SS));

  if (!hdr OR !ss_para)
  	{
    	TRACE_ERROR("ERROR: ss_create() Mem Alloc Failed.");
			
	   	if(hdr)
   			mfwFree((U8*)hdr,sizeof(MfwHdr));

   		if(ss_para)
   			mfwFree((U8*)ss_para,sizeof(T_MFW_SS));
   		
	   	return FALSE;
    }
  
  /*
   * initialisation of the handler
   */
  ss_para->emask   = event;
  ss_para->handler = cbfunc;

  hdr->data = ss_para;                           /* store parameter in node     */
  hdr->type = MFW_TYP_SS;                   /* store type of event handler */

  /*
   * installation of the handler
   */
  insert_status = mfwInsert((T_MFW_HDR *)hWin, hdr);
  
  if(!insert_status)
  	{
  		TRACE_ERROR("ERROR: ss_create() Failed to Install Handler. ");
   		mfwFree((U8*)hdr,sizeof(MfwHdr));
   		mfwFree((U8*)ss_para,sizeof(T_MFW_SS)); 	
		return 0;
  	}
    return insert_status;
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SS               |
| STATE  : code                         ROUTINE: ss_delete           |
+--------------------------------------------------------------------+


   PURPOSE :   delete a event for supplementary management

*/

T_MFW_RES ss_delete(T_MFW_HND h)
{
  TRACE_FUNCTION("ss_delete()");

  if (!h OR !((T_MFW_HDR *)h)->data)
    return MFW_RES_ILL_HND;

  if (!mfwRemove((T_MFW_HDR *)h))
    return MFW_RES_ILL_HND;

  mfwFree((U8 *)(((T_MFW_HDR *) h)->data),sizeof(T_MFW_SS));
  mfwFree((U8 *)h,sizeof(T_MFW_HDR));

  return MFW_RES_OK;
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SS               |
| STATE  : code                         ROUTINE: ss_sign_exec        |
+--------------------------------------------------------------------+


   PURPOSE : Send a signal if SS management handler.

*/

BOOL ss_sign_exec (T_MFW_HDR * cur_elem, T_MFW_EVENT event, T_MFW_SS_PARA * para)
{
  TRACE_FUNCTION ("ss_sign_exec()");

  while (cur_elem)
  {
    /*
     * event handler is available
     */
    if (cur_elem->type EQ MFW_TYP_SS)
    {
      T_MFW_SS * ss_data;
      /*
       * handler is SS management handler
       */
      ss_data = (T_MFW_SS *)cur_elem->data;
      if (ss_data->emask & event)
      {
        /*
         * event is expected by the call back function
         */
        ss_data->event = event;
        switch (event)
        {
          case E_SS_NOTIFY:
            memcpy (&ss_data->para.notify, para, sizeof (T_MFW_SS_NOTIFY));
            break;

          case E_SS_CF_CNF:
          case E_SS_CF_ICN_UPD:
            memcpy (&ss_data->para.cf_cnf, para, sizeof (T_MFW_SS_CF_CNF));
            break;

          case E_SS_CB_CNF:
            memcpy (&ss_data->para.cb_cnf, para, sizeof (T_MFW_SS_CB_CNF));
            break;

          case E_SS_CW_CNF:
            memcpy (&ss_data->para.cw_cnf, para, sizeof (T_MFW_SS_CW_CNF));
            break;

          case E_SS_CLI_CNF:
            memcpy (&ss_data->para.cli_cnf, para, sizeof (T_MFW_SS_CLI_CNF));
            break;

          case E_SS_RES:
            memcpy (&ss_data->para.ss_res, para, sizeof (T_MFW_SS_RES));
            break;

          case E_SS_GET_PW:
            memcpy (&ss_data->para.pw_cnf, para, sizeof (T_MFW_SS_PW_CNF));
            break;

		  case E_SS_IMEI:
            memcpy (&ss_data->para.imei_info, para, sizeof (T_MFW_IMEI));
            break;

          case E_SS_USSD_REQ:
            memcpy (&ss_data->para.ussd_info, para, sizeof (T_MFW_SS_USSD));
            break;

          case E_SS_USSD_CNF:
            memcpy (&ss_data->para.ussd_info, para, sizeof (T_MFW_SS_USSD));
            break;
        }

        /*
         * if call back defined, call it
         */
        if (ss_data->handler)
        {
          // PATCH LE 06.06.00
          // store current mfw elem
          current_mfw_elem = cur_elem;
          // END PATCH LE 06.06.00

          if ((*(ss_data->handler)) (ss_data->event, (void *)&ss_data->para))
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
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SS               |
| STATE  : code                         ROUTINE: ss_signal           |
+--------------------------------------------------------------------+

   PURPOSE : send a event signal.

*/

void ss_signal(T_MFW_EVENT event, void * para)
{
	UBYTE temp;
  TRACE_FUNCTION ("ss_signal()");
	
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
    if (ss_sign_exec (mfwFocus, event, para))
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
     ss_sign_exec (mfwRoot, event, para);
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
        if (ss_sign_exec (h, event, para))
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
      ss_sign_exec (mfwRoot, event, para);
    }
  dspl_Enable(temp);
}



/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SS               |
| STATE  : code                         ROUTINE: ss_check_ss_string  |
+--------------------------------------------------------------------+


   PURPOSE :  Check SS string

*/

T_MFW_SS_RETURN ss_check_ss_string(UBYTE *string)
{
  T_MFW_SS_RETURN  ss_group;
  T_KSD_SEQGRP     grp;
  CHAR            *rest;
  T_KSD_SEQPARAM   para;
  SHORT            id;
  int                string_length;
  UBYTE            call_active=FALSE;

  TRACE_FUNCTION("ss_check_ss_string");

  #define MFW_MAX_DIAL_SHORT_STRING_LEN 2

  /* check the type of destination address */
  if ((cm_search_callId(CAL_STAT_Held, &id) == CM_OK)
      OR (cm_search_callId(CAL_STAT_Active, &id) == CM_OK))
   {
     call_active = TRUE;   
    ksd_decode((CHAR *)string, TRUE, &grp, &rest, &para);
   }
  else
    ksd_decode((CHAR *)string, FALSE, &grp, &rest, &para);

  /* check short string */

  string_length = strlen((char*)string);

 TRACE_EVENT_P2("ksd_decode(""%s"") returns -> %d",(char*)string,grp);


  switch (grp)
  {
  case (SEQGRP_DIAL): /*JVJ CQ 6242 */
    if (string_length<=MFW_MAX_DIAL_SHORT_STRING_LEN)
    {
    /* If the call is active, the short string will be USSD in all the cases not described in ETSI 2.30*/
      if (call_active)                
        grp = SEQGRP_USSD;
        /* If the call is not active, the short strings 1x will be DIAL, the rest USSD*/
      else if ((string_length EQ 1)||(*(string) NEQ '1'))
        grp = SEQGRP_USSD;	
    }
  break;
  case (SEQGRP_SUP_CLIR):   /* JGG/JVJ - CQ 6537 */
  case (SEQGRP_INV_CLIR):
    if (strlen(rest)!=0)
    {
      T_MFW_SS_RETURN  rest_ss_group;

    /*check to make sure that the number is a real dial string */
      rest_ss_group = ss_check_ss_string((UBYTE*)rest);
      if (rest_ss_group EQ MFW_SS_DIAL) /*If rest is a dial string, then the whole string is dial */
        ss_group = MFW_SS_DIAL;
      else 
        ss_group = MFW_SS_UNKNOWN;   

      TRACE_EVENT_P2("ss_check_ss_string(""%s"") returns -> %d",(char*)string,ss_group);
      return ss_group;
    }
  break;
  default:
  break;
 }

  ss_group = ss_check_group(grp, para);

 TRACE_EVENT_P2("ss_check_ss_string(""%s"") returns -> %d",(char*)string,ss_group);

 return ss_group;
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SS               |
| STATE  : code                         ROUTINE: ss_decode           |
+--------------------------------------------------------------------+


   PURPOSE :  Decode SS string

*/

T_MFW_SS_RETURN ss_decode(UBYTE *string, CHAR **rt, T_KSD_SEQPARAM *p)
{
  T_KSD_SEQGRP     grp;
  CHAR            *rest;
  T_KSD_SEQPARAM   para;
  SHORT            id;
  BOOL	bRetVal;

  TRACE_FUNCTION("ss_decode()");

// Added the checking of wait call to solve the bug MMI-SPR 13873.
// If the call is a waiting also pass TRUE as the second parameter to the 
// function ksd_decode() to decode the entered number. In ksd_decode only if   
// the second parameter is TRUE the sequential table is searched to find out 
// the group where the user entered string belongs to.
  if ((cm_search_callId(CAL_STAT_Held, &id) == CM_OK)
      OR (cm_search_callId(CAL_STAT_Active, &id) == CM_OK)
        OR (cm_search_callId(CAL_STAT_Wait, &id) == CM_OK)) 
  {
    	bRetVal = ksd_decode((CHAR *)string, TRUE, &grp, &rest, &para);
    	if (!bRetVal)
      		*rest = 0;
  }
  else
  {
    bRetVal = ksd_decode((CHAR *)string, FALSE, &grp, &rest, &para);
    if (!bRetVal)
      *rest = 0;
  }

  if (!bRetVal)
    return MFW_SS_FAIL;

  *rt = rest;

  if (grp == SEQGRP_UNKNOWN)
    return MFW_SS_UNKNOWN;

  memcpy(p, &para, sizeof(T_KSD_SEQPARAM));
  return(ss_check_group(grp, para));
}



/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SS               |
| STATE  : code                         ROUTINE: ss_check_group      |
+--------------------------------------------------------------------+


   PURPOSE :  returns the group of SS string

*/

T_MFW_SS_RETURN ss_check_group(T_KSD_SEQGRP grp, T_KSD_SEQPARAM   para)
{
	TRACE_EVENT_P1("ss_check_group: grp=%d", grp);
	
  switch (grp)
  {
    case SEQGRP_DIAL:
      /*SPR#1352 - SH - Check for TTY On Next Call psuedo-ss strings*/
      if (!strncmp((char*)para.ussd.ussd, TTY_NEXTCALL_ON, strlen(TTY_NEXTCALL_ON)))
        return MFW_SS_TTY_NEXTCALL_ON;
      if (!strncmp((char*)para.ussd.ussd, TTY_NEXTCALL_OFF, strlen(TTY_NEXTCALL_OFF)))
        return MFW_SS_TTY_NEXTCALL_OFF;
	   /*SH end*/
      return MFW_SS_DIAL;

    case SEQGRP_DIAL_IDX:
      return MFW_SS_DIAL_IDX;

    case SEQGRP_DTMF:
      return MFW_SS_DTMF;

    case SEQGRP_USSD:
    /*MC, SPR 1111 check for LANGUAGE reset code*/
    
    if (!strncmp((char*)para.ussd.ussd, LANG_RESET, strlen(LANG_RESET)) && para.ussd.ussd[6]=='#')
    	return MFW_SS_LANG_RESET;
   /*MC end*/
     return MFW_SS_USSD;

    case SEQGRP_ACT_SIM_LOCK:       /* lock SIM card   */
    case SEQGRP_DEACT_SIM_LOCK:     /* unlock SIM card */
    case SEQGRP_INTRGT_SIM_LOCK:
      return MFW_SS_SIM_LOCK;

    case SEQGRP_CHANGE_REGISTER:
    case SEQGRP_SET_REGISTER:
    case SEQGRP_START_REGISTER:
      return MFW_SS_REG_PW;

    case SEQGRP_CF:
      return MFW_SS_CF;

    case SEQGRP_CB:                     /* barring services                     */
      return MFW_SS_CB;

    case SEQGRP_SUP_CLIR:
    case SEQGRP_INV_CLIR:
      return MFW_SS_CLIR;

    case SEQGRP_SUP_CLIP:
    case SEQGRP_INV_CLIP:
        return MFW_SS_CLIP;

    case SEQGRP_SUP_COLR:
    case SEQGRP_INV_COLR:
        return MFW_SS_COLR;

    case SEQGRP_SUP_COLP:
    case SEQGRP_INV_COLP:
        return MFW_SS_COLP;

    case SEQGRP_CL:
      switch (para.cl.ssCd)
      {
          case KSD_SS_CLIP:
              return MFW_SS_CLIP;
          case KSD_SS_CLIR:
              return MFW_SS_CLIR;
          case KSD_SS_COLP:
              return MFW_SS_COLP;
          case KSD_SS_COLR:
              return MFW_SS_COLR;
      }
      return MFW_SS_UNKNOWN;

    case SEQGRP_PWD:
	  switch(para.pwd.ssCd)
	  {
/*    case KSD_SS_CCBS:      CCBS has nothing to do with PWD! */
	    case KSD_SS_ALL_CBSS:
        case KSD_SS_BOC:
		case KSD_SS_BAOC:
		case KSD_SS_BOIC:
        case KSD_SS_BOICXH:
		case KSD_SS_BIC:
		case KSD_SS_BAIC:
		case KSD_SS_BICRM:

		/*NM 190602*/
		case KSD_SS_ALL_SERV:
		
		  return MFW_SS_REG_PW;

		case KSD_SS_PIN1:
		case KSD_SS_PIN2:
		  return MFW_SS_SIM_REG_PW;
	  }
	  return MFW_SS_UNKNOWN;

    case SEQGRP_UBLK:
      return MFW_SS_SIM_UNBLCK_PIN;

    case SEQGRP_CW:
      return MFW_SS_WAIT;

    case SEQGRP_CCBS:
      return MFW_SS_CCBS;

    case SEQGRP_PRSNT_IMEI:
      return MFW_SS_MMI;

    case SEQGRP_CHLD:
    /*MC 11.04.02 */
    	if (para.chld.mode == CHLD_MOD_Ect)
    		return MFW_SS_ECT;
      else if (para.chld.mode == CHLD_MOD_Ccbs)
        return MFW_SS_CCBS;
    	else
    /*MC*/
      		return MFW_SS_HOLD;

    case SEQGRP_UNKNOWN:
      return MFW_SS_UNKNOWN;

    default:
      return MFW_SS_UNKNOWN;
  }
}

/*
+-------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)   MODULE  : MFW_SS                 |
| STATE   : code                   ROUTINE : ss_execute_transaction |
+-------------------------------------------------------------------+

   PURPOSE :  Start a SS transaction

*/

T_MFW_SS_RETURN ss_execute_transaction(UBYTE *ss_string,
									   UBYTE ussd_mand)
{
    T_ACI_RETURN res;

    TRACE_FUNCTION("ss_execute_transaction()");

    res = sAT_Dn(CMD_SRC_LCL, (CHAR *)ss_string, D_CLIR_OVRD_Default, D_CUG_CTRL_NotPresent, D_TOC_Data); /*a0393213 compiler warnings removal - enum constants are used instead of int in parameters*/
    if( (res != AT_EXCT) AND (res != AT_CMPL))
    {
        TRACE_EVENT("sAT_D error");
        return MFW_SS_FAIL;
    }

    return ss_check_ss_string(ss_string);
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SS               |
| STATE  : code                         ROUTINE: ss_cvtCssuCodes     |
+--------------------------------------------------------------------+


   PURPOSE :  This function is used to convert the CSSU mode
              used by ACI to the CSSU mode used by MFW.

*/

T_MFW_SS_NOTIFY_CODES ss_cvtCssuCodes(T_ACI_CSSU_CODE code)
{
	switch (code)
	{
		case CSSU_CODE_ForwardedCall:  return MFW_SS_MT_FORWARDED;
		case CSSU_CODE_CUGCall:        return MFW_SS_MT_CUG_CALL;
		case CSSU_CODE_Multiparty:     return MFW_SS_NOTIFY_MULTIPARTY;
		case CSSU_CODE_HeldCallRel:    return MFW_SS_NOTIFY_holdRELEASED;
		case CSSU_CODE_FwrdCheckSS:    return MFW_SS_NOTIFY_checkSS;
		case CSSU_CODE_ECTAlert:       return MFW_SS_NOTIFY_ectALERT;
		case CSSU_CODE_ECTConnect:     return MFW_SS_NOTIFY_ectCONNECT;
		case CSSU_CODE_DeflectedCall:  return MFW_SS_MT_DEFLECTED;
		//x0pleela 22 Feb, 2007 ER: OMAPS00113891
		/* 	Adding the following new enum to differentiate the indications b/n the 
			subscriber who forwarded the call (CSSU_CODE_IncCallForwarded) and 
			to whom the call was forwarded(CSSU_CODE_ForwardedCall) */
		case CSSU_CODE_IncCallForwarded: return MFW_SS_MT_CALL_FORWARDED;
		default:					   return MFW_SS_NOTIFY_UNKNOWN;
	}
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SS               |
| STATE  : code                         ROUTINE: ss_cvtCssiCodes     |
+--------------------------------------------------------------------+


   PURPOSE :  This function is used to convert the CSSI mode
              used by ACI to the CSSI mode used by MFW.

*/

T_MFW_SS_NOTIFY_CODES ss_cvtCssiCodes(T_ACI_CSSI_CODE code)
{
	switch (code)
	{
		case CSSI_CODE_CFUActive:      return MFW_SS_MO_unconFORWARD;
		case CSSI_CODE_SomeCCFActive:  return MFW_SS_MO_conFORWARD;
		case CSSI_CODE_ForwardedCall:  return MFW_SS_MO_FORWARDED;
		case CSSI_CODE_CallWaiting:    return MFW_SS_MO_WAITING;
		case CSSI_CODE_CUGCall:        return MFW_SS_MO_CUG_CALL;
		case CSSI_CODE_OutCallsBarred: return MFW_SS_MO_moBARRED;
		case CSSI_CODE_IncCallsBarred: return MFW_SS_MO_mtBARRED;
		case CSSI_CODE_CLIRSupRej:     return MFW_SS_MO_CLIR;
		case CSSI_CODE_DeflectedCall:  return MFW_SS_MO_DEFLECTED;
		default:					   return MFW_SS_NOTIFY_UNKNOWN;
	}
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SS               |
| STATE  : code                         ROUTINE: ss_notify           |
+--------------------------------------------------------------------+


   PURPOSE :  Change of SS status from the network.

*/

void ss_notify(T_MFW_SS_NOTIFY_CODES code,
			   SHORT                 index,
			   UBYTE                *number,
			   UBYTE                *subaddr)
{
//	Apr 06 2005		REF: CRR 29989	xpradipg
//	defined the variable locally since the scope lies within this function
#ifdef FF_MMI_OPTIM
	T_MFW_SS_NOTIFY    notify;
#endif
	memset(&notify, 0, sizeof(T_MFW_SS_NOTIFY));

	notify.code = code;

	if(number)
		strcpy((char *)notify.number, (char *)number);
	if (subaddr)
		strcpy((char *)notify.subaddr, (char *)subaddr);

	//PATCH TB 1309: Wrong codes tested
	if ((code == MFW_SS_MO_CUG_CALL) OR 
		(code == MFW_SS_MT_CUG_CALL))
		notify.index = (UBYTE)index;
	/* END PATCH TB	*/
		
    {
		/***************************Go-lite Optimization changes Start***********************/
		//Aug 16, 2004    REF: CRR 24323   Deepa M.D
		TRACE_EVENT_P1 ("SS notify code = %d", notify.code);
		/***************************Go-lite Optimization changes end***********************/
    }

	ss_signal(E_SS_NOTIFY, &notify);
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE  : MFW_SS             |
| STATE  : code                         ROUTINE : ss_cvtOCode        |
+--------------------------------------------------------------------+


   PURPOSE :  Convert operation codes

*/

T_MFW ss_cvtOCode(T_ACI_KSD_OP code)
{
    switch (code)
	{
		case KSD_OP_IRGT:           return SS_INTERROGATION;
		case KSD_OP_REG:            return SS_REGISTRATION;
		case KSD_OP_ERS:            return SS_ERASURE;
		case KSD_OP_ACT:            return SS_ACTIVATION;
		case KSD_OP_DEACT:          return SS_DEACTIVATION;
		default:					return SS_CODES_UNKNOWN;
	}
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE  : MFW_SS             |
| STATE  : code                         ROUTINE : ss_cvtSCodes       |
+--------------------------------------------------------------------+


   PURPOSE :  This function is used to convert the SS code
              used by ACI to the SS code used by MFW.

*/

T_MFW ss_cvtSCode(T_ACI_KSD_SS code) 
{
	switch (code)
	{
		case KSD_SS_CLIP:           return SS_CLIP;
		case KSD_SS_CLIR:           return SS_CLIR;
		case KSD_SS_COLP:           return SS_COLP;
		case KSD_SS_COLR:           return SS_COLR;
		case KSD_SS_CW:             return SS_CW;
		case KSD_SS_BAIC:           return SS_CB_BAIC;
		case KSD_SS_BAOC:           return SS_CB_BAOC;
		case KSD_SS_ALL_CBSS:       return SS_CB_ALL;
		case KSD_SS_BICRM:          return SS_CB_BAICroam;
   		case KSD_SS_BIC:            return SS_CB_ALL_BAIC;
   		case KSD_SS_BOICXH:         return SS_CB_BAOICexH;
   		case KSD_SS_BOIC:           return SS_CB_BAOIC;
   		case KSD_SS_BOC:            return SS_CB_ALL_BAOC;
        case KSD_SS_CFB:            return SS_CF_CFB;
        case KSD_SS_CFNRC:          return SS_CF_CFNR;
        case KSD_SS_CFNRY:          return SS_CF_CFNRy;
        case KSD_SS_CFU:            return SS_CF_CFU;
        case KSD_SS_ALL_CFWSS:      return SS_CF_ALL_COND;
        case KSD_SS_ALL_FWSS:       return SS_CF_ALL;
        case KSD_SS_CCBS:           return SS_CCBS;
        case KSD_SS_PIN1:           return SS_PIN1;
        case KSD_SS_PIN2:           return SS_PIN2;
		default:					return SS_CODES_UNKNOWN;
	}
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE  : MFW_SS             |
| STATE  : code                         ROUTINE : ss_cvtCLStatus     |
+--------------------------------------------------------------------+


   PURPOSE :  This function is used to convert the CL status
              used by ACI to the CL status used by MFW.

*/

T_MFW ss_cvtCLStatus(T_ACI_KSD_CLIR_OP stat)
{
	switch (stat)
	{
		case KSD_CO_PERMANENT:            return MFW_CL_PERM;
        case KSD_CO_TEMPORARY:            return MFW_CL_TEMP_DEF_REST;
        case KSD_CO_ALLOWED:              return MFW_CL_TEMP_DEF_ALLOWED;
		default:		                  return MFW_CL_UNKNOWN;
	}
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE  : MFW_SS             |
| STATE  : code                         ROUTINE : ss_cvtCLOvrd       |
+--------------------------------------------------------------------+


   PURPOSE :  This function is used to convert the override status of
              CLIR used by ACI to the override status of CLIR used by
              MFW.

*/

T_MFW ss_cvtCLOvrd(T_ACI_KSD_OVRD_CTG ovrd)
{
	switch (ovrd)
	{
		case KSD_OVR_CAT_ENABLED:     return MFW_OV_ENABLED;
        case KSD_OVR_CAT_DISABLED:    return MFW_OV_DISABLED;
		default:		                  return MFW_OV_UNKNOWN;
	}
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE  : MFW_SS             |
| STATE  : code                         ROUTINE : ss_cvtErrCode      |
+--------------------------------------------------------------------+


   PURPOSE :  This function is used to convert the error code used
              by ACI to the error code used by MFW.

*/

T_MFW ss_cvtErrCode(T_ACI_KSD_ERR err)
{
	switch (err)
	{
		case KSD_NO_ERROR:                  return MFW_SS_NO_ERROR;
    case KSD_ERR_UNKNOWN_SUBSCRIBER:    return MFW_SS_ERR_UNKNOWN_SUBSCRIBER;
    case KSD_ERR_ILLEGAL_SUBSCRIBER:    return MFW_SS_ERR_ILLEGAL_SUBSCRIBER;
    case KSD_ERR_BEARER_SVC_NOT_PROV:   return MFW_SS_ERR_BEARER_SVC_NOT_PROV;
    case KSD_ERR_TELE_SVC_NOT_PROV:     return MFW_SS_ERR_TELE_SVC_NOT_PROV;
    case KSD_ERR_ILLEGAL_EQUIPMENT:     return MFW_SS_ERR_ILLEGAL_EQUIPMENT;
    case KSD_ERR_CALL_BARRED:           return MFW_SS_ERR_CALL_BARRED;
    case KSD_ERR_ILLEGAL_SS_OPERATION:  return MFW_SS_ERR_ILLEGAL_SS_OPERATION;
    case KSD_ERR_SS_ERR_STATUS:         return MFW_SS_ERR_SS_ERR_STATUS;
    case KSD_ERR_SS_NOT_AVAIL:          return MFW_SS_ERR_SS_NOT_AVAIL;
    case KSD_ERR_SS_SUBS_VIOLATION:     return MFW_SS_ERR_SS_SUBS_VIOLATION;
    case KSD_ERR_SS_INCOMP:             return MFW_SS_ERR_SS_INCOMP;
    case KSD_ERR_FAC_NOT_SUPPORTED:     return MFW_SS_ERR_FAC_NOT_SUPPORTED;
    case KSD_ERR_ABSENT_SUBS:           return MFW_SS_ERR_ABSENT_SUBS;
    case KSD_ERR_SYSTEM_FAIL:           return MFW_SS_ERR_SYSTEM_FAIL;
    case KSD_ERR_DATA_MISSING:          return MFW_SS_ERR_DATA_MISSING;
    case KSD_ERR_UNEXPECT_DATA:         return MFW_SS_ERR_UNEXPECT_DATA;
    case KSD_ERR_PWD_REG_FAIL:          return MFW_SS_ERR_PWD_REG_FAIL;
    case KSD_ERR_NEG_PWD_CHECK:         return MFW_SS_ERR_NEG_PWD_CHECK;
    case KSD_ERR_NUM_PWD_VIOLATION:     return MFW_SS_ERR_NUM_PWD_VIOLATION;
    case KSD_ERR_UNKNOWN_ALPHA:         return MFW_SS_ERR_UNKNOWN_ALPHA;
    case KSD_ERR_USSD_BUSY:             return MFW_SS_ERR_USSD_BUSY;
    case KSD_ERR_MAX_NUM_MPTY_EXCEED:   return MFW_SS_ERR_MAX_NUM_MPTY_EXCEED;
    case KSD_ERR_RESOURCE_NOT_AVAIL:    return MFW_SS_ERR_RESOURCE_NOT_AVAIL;
    case KSD_GEN_PROB_UNRECOG_CMP:      return MFW_SS_GEN_PROB_UNRECOG_CMP;
    case KSD_GEN_PROB_MISTYPED_CMP:     return MFW_SS_GEN_PROB_MISTYPED_CMP;
    case KSD_GEN_PROB_BAD_STRUCT_CMP:   return MFW_SS_GEN_PROB_BAD_STRUCT_CMP;
    case KSD_INV_PROB_DUPL_INV_ID:      return MFW_SS_INV_PROB_DUPL_INV_ID;
    case KSD_INV_PROB_UNRECOG_OP:       return MFW_SS_INV_PROB_UNRECOG_OP;
    case KSD_INV_PROB_MISTYPED_PAR:     return MFW_SS_INV_PROB_MISTYPED_PAR;
    case KSD_INV_PROB_RESOURCE_LIM:     return MFW_SS_INV_PROB_RESOURCE_LIM;
    case KSD_INV_PROB_INIT_RELEASE:     return MFW_SS_INV_PROB_INIT_RELEASE;
    case KSD_INV_PROB_UNRECOG_LNK_ID:   return MFW_SS_INV_PROB_UNRECOG_LNK_ID;
    case KSD_INV_PROB_LNK_RES_UNEXP:    return MFW_SS_INV_PROB_LNK_RES_UNEXP;
    case KSD_INV_PROB_UNEXP_LNK_OP:     return MFW_SS_INV_PROB_UNEXP_LNK_OP;
    case KSD_RES_PROB_UNRECOG_INV_ID:   return MFW_SS_RES_PROB_UNRECOG_INV_ID;
    case KSD_RES_PROB_RET_RES_UNEXP:    return MFW_SS_RES_PROB_RET_RES_UNEXP;
    case KSD_RES_PROB_MISTYPED_PAR:     return MFW_SS_RES_PROB_MISTYPED_PAR;
    case KSD_ERR_PROB_UNRECOG_INV_ID:   return MFW_SS_ERR_PROB_UNRECOG_INV_ID;
    case KSD_ERR_PROB_RET_ERR_UNEXP:    return MFW_SS_ERR_PROB_RET_ERR_UNEXP;
    case KSD_ERR_PROB_UNRECOG_ERR:      return MFW_SS_ERR_PROB_UNRECOG_ERR;
    case KSD_ERR_PROB_UNEXP_ERR:        return MFW_SS_ERR_PROB_UNEXP_ERR;
    case KSD_ERR_PROB_MISTYPED_PAR:     return MFW_SS_ERR_PROB_MISTYPED_PAR;
    case KSD_ERR_FATAL_INV_RESULT:      return MFW_SS_ERR_FATAL_INV_RESULT;
    case KSD_ERR_FATAL_CCD_DEC:         return MFW_SS_ERR_FATAL_CCD_DEC;
    case KSD_ERR_FATAL_SS_ENT:          return MFW_SS_ERR_FATAL_SS_ENT;
    default:                            return MFW_SS_ERR_UNKNOWN;
  }
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE  : MFW_SS             |
| STATE  : code                         ROUTINE : ss_decode_imei     |
+--------------------------------------------------------------------+


   PURPOSE :  convert imei (packed bcd to ASCII)

*/

void ss_decode_imei(T_ACI_KSIR *ksStat)
{
//	Apr 06 2005		REF: CRR 29989	xpradipg
#ifdef FF_MMI_OPTIM
	memset(imei_info, 0, sizeof(T_MFW_IMEI));

#ifdef NEPTUNE_BOARD  /* OMAPS00059546 : BCD conversion has been Added for NEPTUNE Board */

    imei_info.imei_number[0] = ( ksStat->ir.rKSIMEI.tac1 )   & 0x0F | 0x30;
    imei_info.imei_number[1] = ( ksStat->ir.rKSIMEI.tac1 & 0xF0 )   >> 4 | 0x30;
    imei_info.imei_number[2] = ( ksStat->ir.rKSIMEI.tac2 )   & 0x0F | 0x30;
    imei_info.imei_number[3] = ( ksStat->ir.rKSIMEI.tac2 & 0xF0 )   >> 4 | 0x30;
    imei_info.imei_number[4] = ( ksStat->ir.rKSIMEI.tac3 )   & 0x0F | 0x30;	
    imei_info.imei_number[5] = ( ksStat->ir.rKSIMEI.tac3 & 0xF0 )   >> 4 | 0x30;
    imei_info.imei_number[6] = ( ksStat->ir.rKSIMEI.fac  )    & 0x0F | 0x30;    
    imei_info.imei_number[7] = ( ksStat->ir.rKSIMEI.fac  & 0xF0 )   >> 4 | 0x30;
    imei_info.imei_number[8] = ( ksStat->ir.rKSIMEI.snr1 )   & 0x0F | 0x30;
    imei_info.imei_number[9] = ( ksStat->ir.rKSIMEI.snr1 & 0xF0 )   >> 4 | 0x30;
    imei_info.imei_number[10] = ( ksStat->ir.rKSIMEI.snr2 )  & 0x0F | 0x30;
    imei_info.imei_number[11] = ( ksStat->ir.rKSIMEI.snr2 & 0xF0 )  >> 4 | 0x30;
    imei_info.imei_number[12] = ( ksStat->ir.rKSIMEI.snr3 )  & 0x0F | 0x30;
    imei_info.imei_number[13] = ( ksStat->ir.rKSIMEI.snr3 & 0xF0 ) >> 4 | 0x30;
    imei_info.imei_number[14] = ( ksStat->ir.rKSIMEI.cd & 0x0F ) | 0x30;
    imei_info.imei_number[15] = ( ksStat->ir.rKSIMEI.svn )  & 0x0F | 0x30;
    imei_info.imei_number[16] = ( ksStat->ir.rKSIMEI.svn  & 0xF0 ) >> 4 | 0x30;
    imei_info.imei_number[17] = '\0';
	
#else
    imei_info->imei_number[0] = ( ksStat->ir.rKSIMEI.tac1 & 0xF0 )   >> 4 | 0x30;
    imei_info->imei_number[1] = ( ksStat->ir.rKSIMEI.tac1 )   & 0x0F | 0x30;
    imei_info->imei_number[2] = ( ksStat->ir.rKSIMEI.tac2 & 0xF0 )   >> 4 | 0x30;
	imei_info->imei_number[3] = ( ksStat->ir.rKSIMEI.tac2 )   & 0x0F | 0x30;
    imei_info->imei_number[4] = ( ksStat->ir.rKSIMEI.tac3 & 0xF0 )   >> 4 | 0x30;
	imei_info->imei_number[5] = ( ksStat->ir.rKSIMEI.tac3 )   & 0x0F | 0x30;
    imei_info->imei_number[6] = ( ksStat->ir.rKSIMEI.fac  & 0xF0 )   >> 4 | 0x30;
	imei_info->imei_number[7] = ( ksStat->ir.rKSIMEI.fac  )    & 0x0F | 0x30;
    imei_info->imei_number[8] = ( ksStat->ir.rKSIMEI.snr1 & 0xF0 )   >> 4 | 0x30;
	imei_info->imei_number[9] = ( ksStat->ir.rKSIMEI.snr1 )   & 0x0F | 0x30;
    imei_info->imei_number[10] = ( ksStat->ir.rKSIMEI.snr2 & 0xF0 )  >> 4 | 0x30;
	imei_info->imei_number[11] = ( ksStat->ir.rKSIMEI.snr2 )  & 0x0F | 0x30;
    imei_info->imei_number[12] = ( ksStat->ir.rKSIMEI.snr3 & 0xF0 ) >> 4 | 0x30;
	imei_info->imei_number[13] = ( ksStat->ir.rKSIMEI.snr3 )  & 0x0F | 0x30;
//a0876501 - Check byte comes before the Software version during display.
	imei_info->imei_number[14] = ( ksStat->ir.rKSIMEI.cd & 0x0F ) | 0x30;
    imei_info->imei_number[15] = ( ksStat->ir.rKSIMEI.svn  & 0xF0 ) >> 4 | 0x30;
	imei_info->imei_number[16] = ( ksStat->ir.rKSIMEI.svn )  & 0x0F | 0x30;
	imei_info->imei_number[17] = '\0';
#endif
#else
    memset(&imei_info, 0, sizeof(imei_info));

#ifdef NEPTUNE_BOARD /* OMAPS00059546 : BCD to conversion has been changed */
    imei_info.imei_number[0] = ( ksStat->ir.rKSIMEI.tac1 )   & 0x0F | 0x30;
    imei_info.imei_number[1] = ( ksStat->ir.rKSIMEI.tac1 & 0xF0 )   >> 4 | 0x30;
    imei_info.imei_number[2] = ( ksStat->ir.rKSIMEI.tac2 )   & 0x0F | 0x30;
    imei_info.imei_number[3] = ( ksStat->ir.rKSIMEI.tac2 & 0xF0 )   >> 4 | 0x30;
    imei_info.imei_number[4] = ( ksStat->ir.rKSIMEI.tac3 )   & 0x0F | 0x30;	
    imei_info.imei_number[5] = ( ksStat->ir.rKSIMEI.tac3 & 0xF0 )   >> 4 | 0x30;
    imei_info.imei_number[6] = ( ksStat->ir.rKSIMEI.fac  )    & 0x0F | 0x30;    
    imei_info.imei_number[7] = ( ksStat->ir.rKSIMEI.fac  & 0xF0 )   >> 4 | 0x30;
    imei_info.imei_number[8] = ( ksStat->ir.rKSIMEI.snr1 )   & 0x0F | 0x30;
    imei_info.imei_number[9] = ( ksStat->ir.rKSIMEI.snr1 & 0xF0 )   >> 4 | 0x30;
    imei_info.imei_number[10] = ( ksStat->ir.rKSIMEI.snr2 )  & 0x0F | 0x30;
    imei_info.imei_number[11] = ( ksStat->ir.rKSIMEI.snr2 & 0xF0 )  >> 4 | 0x30;
    imei_info.imei_number[12] = ( ksStat->ir.rKSIMEI.snr3 )  & 0x0F | 0x30;
    imei_info.imei_number[13] = ( ksStat->ir.rKSIMEI.snr3 & 0xF0 ) >> 4 | 0x30;
    imei_info.imei_number[14] = ( ksStat->ir.rKSIMEI.cd & 0x0F ) | 0x30;
    imei_info.imei_number[15] = ( ksStat->ir.rKSIMEI.svn )  & 0x0F | 0x30;
    imei_info.imei_number[16] = ( ksStat->ir.rKSIMEI.svn  & 0xF0 ) >> 4 | 0x30;
    imei_info.imei_number[17] = '\0';
	
   #else
//	Jul 05 2005		REF: CRR 22565	a0876501
//	Fixed the issue of byte swapping.

    imei_info.imei_number[0] = ( ksStat->ir.rKSIMEI.tac1 & 0xF0 )   >> 4 | 0x30;
    imei_info.imei_number[1] = ( ksStat->ir.rKSIMEI.tac1 )   & 0x0F | 0x30;
    imei_info.imei_number[2] = ( ksStat->ir.rKSIMEI.tac2 & 0xF0 )   >> 4 | 0x30;
	imei_info.imei_number[3] = ( ksStat->ir.rKSIMEI.tac2 )   & 0x0F | 0x30;
    imei_info.imei_number[4] = ( ksStat->ir.rKSIMEI.tac3 & 0xF0 )   >> 4 | 0x30;
	imei_info.imei_number[5] = ( ksStat->ir.rKSIMEI.tac3 )   & 0x0F | 0x30;
    imei_info.imei_number[6] = ( ksStat->ir.rKSIMEI.fac  & 0xF0 )   >> 4 | 0x30;
	imei_info.imei_number[7] = ( ksStat->ir.rKSIMEI.fac  )    & 0x0F | 0x30;
    imei_info.imei_number[8] = ( ksStat->ir.rKSIMEI.snr1 & 0xF0 )   >> 4 | 0x30;
	imei_info.imei_number[9] = ( ksStat->ir.rKSIMEI.snr1 )   & 0x0F | 0x30;
    imei_info.imei_number[10] = ( ksStat->ir.rKSIMEI.snr2 & 0xF0 )  >> 4 | 0x30;
	imei_info.imei_number[11] = ( ksStat->ir.rKSIMEI.snr2 )  & 0x0F | 0x30;
    imei_info.imei_number[12] = ( ksStat->ir.rKSIMEI.snr3 & 0xF0 ) >> 4 | 0x30;
	imei_info.imei_number[13] = ( ksStat->ir.rKSIMEI.snr3 )  & 0x0F | 0x30;
//a0876501 - Check byte comes before the Software version during display.
	imei_info.imei_number[14] = ( ksStat->ir.rKSIMEI.cd & 0x0F ) | 0x30;
    imei_info.imei_number[15] = ( ksStat->ir.rKSIMEI.svn  & 0xF0 ) >> 4 | 0x30;
	imei_info.imei_number[16] = ( ksStat->ir.rKSIMEI.svn )  & 0x0F | 0x30;
	imei_info.imei_number[17] = '\0';

#endif
  TRACE_EVENT_P8("ss_decode_imei IMEI: TAC %1c%1c%1c%1c%1c%1c%1c%1c",
                  imei_info.imei_number[0], imei_info.imei_number[1], imei_info.imei_number[2], imei_info.imei_number[3],
                  imei_info.imei_number[4], imei_info.imei_number[5], imei_info.imei_number[6], imei_info.imei_number[7]);
  TRACE_EVENT_P6("ss_decode_imei IMEI: SNR %1c%1c%1c%1c%1c%1c",
                  imei_info.imei_number[8],  imei_info.imei_number[9], imei_info.imei_number[10], imei_info.imei_number[11],
                  imei_info.imei_number[12], imei_info.imei_number[13]);
  TRACE_EVENT_P4("ss_decode_imei IMEI: SV CD %1c%1c%1c%1c", 
				  imei_info.imei_number[14], imei_info.imei_number[15], imei_info.imei_number[16],imei_info.imei_number[17]);

#endif	
}

#ifdef NEPTUNE_BOARD


/* ===========================================================*/
/**
* mfw_getSumOf2Digits () is called when getting IMEI request from the used
*
* @param UBYTE digits  - Byte to be added
*
*  @return UBYTE - Sum of two digits
*
*  @see
*/
/* =========================================================== */
LOCAL UBYTE mfw_getSumOf2Digits ( UBYTE idigits )
{
  UBYTE iDval;

   TRACE_FUNCTION( "mfw_getSumOf2Digits" );
  /* compute odd labeled digit */
  iDval = 2 * ( idigits & 0x0F );
  iDval = iDval - ( iDval < 10 ? 0 : 9 );

  /*
     return sum of even labeled digit and
     already processed odd labeled digit
  */
  return iDval + ( ( idigits >> 4 ) & 0x0F );
}

#endif

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE  : MFW_SS             |
| STATE  : code                         ROUTINE : mfw_getCdByteFromImei  |
+--------------------------------------------------------------------+
   PURPOSE :  Get CD byte from IMEI.  
*/
UBYTE getCdByteFromImei(UBYTE *imei)
{
	T_ACI_IMEI aci_imei;
  #ifdef NEPTUNE_BOARD
  UBYTE iSum = 0; /* sum of step 2    */
  UBYTE iCd;      /* Luhn Check Digit */
  #endif

  TRACE_FUNCTION( "getCdByteFromImei" );
//	For calculating the Cd byte, copy the buffer contents.
  aci_imei.tac1 = imei[0];
  aci_imei.tac2 = imei[1];
  aci_imei.tac3 = imei[2];
  aci_imei.fac  = imei[3];
  aci_imei.snr1 = imei[4];
  aci_imei.snr2 = imei[5];
  aci_imei.snr3 = imei[6];
  aci_imei.svn  = imei[7];
#ifndef NEPTUNE_BOARD

  return(cmhSS_getCdFromImei(&aci_imei));

#else
  
  /* OMAPS00059546 : Calculating the CD bytes for IMEI  */
  iSum += mfw_getSumOf2Digits ( aci_imei.tac1 );
  iSum += mfw_getSumOf2Digits ( aci_imei.tac2 );
  iSum += mfw_getSumOf2Digits ( aci_imei.tac3 );
  iSum += mfw_getSumOf2Digits ( aci_imei.fac  );
  iSum += mfw_getSumOf2Digits ( aci_imei.snr1 );
  iSum += mfw_getSumOf2Digits ( aci_imei.snr2 );
  iSum += mfw_getSumOf2Digits ( aci_imei.snr3 );
  
  iCd = 10 - ( iSum % 10 );
  iCd = ( iCd EQ 10 ? 0 : iCd );
  return ( iCd );
#endif

}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SS               |
| STATE  : code                         ROUTINE: rAT_PercentKSIR     |
+--------------------------------------------------------------------+


   PURPOSE :  Network response for key sequences

*/

void rAT_PercentKSIR ( T_ACI_KSIR *ksStat)
{
    UBYTE idx;
    T_MFW_TELECOM   *p_tele;
    T_MFW_FEATURE   *p_feat;
    T_MFW_SRV_GROUP *p_srv;

    TRACE_FUNCTION("rAT_PercentKSIR()");
    TRACE_EVENT_P1("ksStat->ksdCmd %d", ksStat->ksdCmd);

    switch (ksStat->ksdCmd)
    {
        case KSD_CMD_CB:
          TRACE_EVENT("KSD_CMD_CB");
          TRACE_EVENT_P1("ksStat->ir.rKSCB.ssCd %d", ksStat->ir.rKSCB.ssCd);
          TRACE_EVENT_P1("ksStat->ir.rKSCB.opCd %d", ksStat->ir.rKSCB.opCd);
          TRACE_EVENT_P1("ksStat->ir.rKSCB.ssErr %d", ksStat->ir.rKSCB.ssErr);
          TRACE_EVENT_P1("ksStat->ir.rKSCB.c_cbInfoLst %d", ksStat->ir.rKSCB.c_cbInfoLst);
          if (ksStat->ir.rKSCB.c_cbInfoLst)
          {
            int i;
            for (i = 0; i < ksStat->ir.rKSCB.c_cbInfoLst; i++)
            {
              TRACE_EVENT_P2("ksStat->ir.rKSCB.cbInfoLst[%d].bsTp %d", i, ksStat->ir.rKSCB.cbInfoLst[i].bsTp);
              TRACE_EVENT_P2("ksStat->ir.rKSCB.cbInfoLst[%d].bsCd %d", i, ksStat->ir.rKSCB.cbInfoLst[i].bsCd);
              TRACE_EVENT_P2("ksStat->ir.rKSCB.cbInfoLst[%d].ssSt %d", i, ksStat->ir.rKSCB.cbInfoLst[i].ssSt);
            }
          }
          p_tele = (T_MFW_TELECOM *) ssLstBuf;
//     	Apr 06 2005		REF: CRR 29989	xpradipg
#ifdef FF_MMI_OPTIM
			cb_cnf = (T_MFW_SS_CB_CNF*)mfwAlloc(sizeof(T_MFW_SS_CB_CNF));
			if(NULL != cb_cnf)
		{
		  TRACE_EVENT_P2("Mem Aloc for cb_cnf = %x Size = %d",cb_cnf,sizeof(T_MFW_SS_CB_CNF));
		  cb_cnf->ss_code = (T_MFW_SS_CODES)ss_cvtSCode((T_ACI_KSD_SS)ksStat->ir.rKSCB.ssCd); /*a0393213 compiler warnings removal - explicit type castingl*/
          cb_cnf->ss_category = (T_MFW_SS_CATEGORY)ss_cvtOCode((T_ACI_KSD_OP)ksStat->ir.rKSCB.opCd); /*a0393213 compiler warnings remova - explicit type castingl*/
          cb_cnf->ss_error    =(T_MFW_SS_ERROR) ss_cvtErrCode((T_ACI_KSD_ERR)ksStat->ir.rKSCB.ssErr); /*a0393213 compiler warnings removal - explicit type casting*/
          cb_cnf->ss_telecom_list = p_tele;
         }
         else
         	return;
#else
	   	  cb_cnf.ss_code = (T_MFW_SS_CODES) ss_cvtSCode((T_ACI_KSD_SS)ksStat->ir.rKSCB.ssCd);
          cb_cnf.ss_category = (T_MFW_SS_CATEGORY)ss_cvtOCode((T_ACI_KSD_OP)ksStat->ir.rKSCB.opCd);
          cb_cnf.ss_error    = (T_MFW_SS_ERROR)ss_cvtErrCode((T_ACI_KSD_ERR)ksStat->ir.rKSCB.ssErr);
          cb_cnf.ss_telecom_list = p_tele;
#endif          

          for( idx = 0;
               idx < ksStat->ir.rKSCB.c_cbInfoLst AND
               idx < MFW_MAX_TELE_NR;
               idx++, p_tele++ )
          {
            p_tele->ss_telecom_type    = ksStat->ir.rKSCB.
                                                     cbInfoLst[idx].bsTp;
            p_tele->ss_telecom_service = ksStat->ir.rKSCB.
                                                     cbInfoLst[idx].bsCd;
            p_tele->ss_status = ksStat->ir.rKSCB.
                                            cbInfoLst[idx].ssSt;
          }
//	Apr 06 2005		REF: CRR 29989	xpradipg          
#ifdef FF_MMI_OPTIM
		if(NULL != cb_cnf)	
          cb_cnf->ss_telecom_count = idx;
#else
          cb_cnf.ss_telecom_count = idx;
#endif          
          break;

        case KSD_CMD_CF:
          TRACE_EVENT("KSD_CMD_CF");
          TRACE_EVENT_P1("ksStat->ir.rKSCF.ssCd %d", ksStat->ir.rKSCF.ssCd);
          TRACE_EVENT_P1("ksStat->ir.rKSCF.opCd %d", ksStat->ir.rKSCF.opCd);
          TRACE_EVENT_P1("ksStat->ir.rKSCF.ssErr %d", ksStat->ir.rKSCF.ssErr);
          TRACE_EVENT_P1("ksStat->ir.rKSCF.c_cfFeatLst %d", ksStat->ir.rKSCF.c_cfFeatLst);
          if (ksStat->ir.rKSCF.c_cfFeatLst)
          {
            int i;
            for (i = 0; i < ksStat->ir.rKSCF.c_cfFeatLst; i++)
            {
              TRACE_EVENT_P2("ksStat->ir.rKSCF.cfFeatLst[%d].bsTp %d", i, ksStat->ir.rKSCF.cfFeatLst[i].bsTp);
              TRACE_EVENT_P2("ksStat->ir.rKSCF.cfFeatLst[%d].bsCd %d", i, ksStat->ir.rKSCF.cfFeatLst[i].bsCd);
              TRACE_EVENT_P2("ksStat->ir.rKSCF.cfFeatLst[%d].ssSt %d", i, ksStat->ir.rKSCF.cfFeatLst[i].ssSt);
              TRACE_EVENT_P2("ksStat->ir.rKSCF.cfFeatLst[%d].num %s", i, ksStat->ir.rKSCF.cfFeatLst[i].num);
              TRACE_EVENT_P2("ksStat->ir.rKSCF.cfFeatLst[%d].ton 0x%02X", i, ksStat->ir.rKSCF.cfFeatLst[i].ton);
              TRACE_EVENT_P2("ksStat->ir.rKSCF.cfFeatLst[%d].npi 0x%02X", i, ksStat->ir.rKSCF.cfFeatLst[i].npi);
              TRACE_EVENT_P2("ksStat->ir.rKSCF.cfFeatLst[%d].sub %s", i, ksStat->ir.rKSCF.cfFeatLst[i].sub);
              TRACE_EVENT_P2("ksStat->ir.rKSCF.cfFeatLst[%d].tos 0x%02X", i, ksStat->ir.rKSCF.cfFeatLst[i].tos);
              TRACE_EVENT_P2("ksStat->ir.rKSCF.cfFeatLst[%d].oe 0x%02X", i, ksStat->ir.rKSCF.cfFeatLst[i].oe);
              TRACE_EVENT_P2("ksStat->ir.rKSCF.cfFeatLst[%d].time 0x%02X", i, ksStat->ir.rKSCF.cfFeatLst[i].time);
            }
          }
          p_feat = (T_MFW_FEATURE *) ssLstBuf;
//	Apr 06 2005		REF: CRR 29989	xpradipg
#ifdef FF_MMI_OPTIM
		  cf_cnf = (T_MFW_SS_CF_CNF*)mfwAlloc(sizeof(T_MFW_SS_CF_CNF));
		  if(NULL != cf_cnf)
	      {
      		TRACE_EVENT_P2("Mem Loc = %x, allocates size = %d",cf_cnf,sizeof(T_MFW_SS_CF_CNF));
		      cf_cnf->ss_code = (T_MFW_SS_CODES)ss_cvtSCode((T_ACI_KSD_SS)ksStat->ir.rKSCF.ssCd);
	          cf_cnf->ss_category = (T_MFW_SS_CATEGORY)ss_cvtOCode((T_ACI_KSD_OP)ksStat->ir.rKSCF.opCd);
	          cf_cnf->ss_error    = (T_MFW_SS_ERROR)ss_cvtErrCode((T_ACI_KSD_ERR)ksStat->ir.rKSCF.ssErr);
	          cf_cnf->ss_feature_list = p_feat;
	      }
#else
		      cf_cnf.ss_code = (T_MFW_SS_CODES)ss_cvtSCode((T_ACI_KSD_SS)ksStat->ir.rKSCF.ssCd);
          cf_cnf.ss_category = (T_MFW_SS_CATEGORY)ss_cvtOCode((T_ACI_KSD_OP)ksStat->ir.rKSCF.opCd);
          cf_cnf.ss_error    = (T_MFW_SS_ERROR)ss_cvtErrCode((T_ACI_KSD_ERR)ksStat->ir.rKSCF.ssErr);

          cf_cnf.ss_feature_list = p_feat;
#endif	
          for( idx = 0;
               idx < ksStat->ir.rKSCF.c_cfFeatLst AND
               idx < MFW_MAX_FEAT_NR;
               idx++, p_feat++ )
          {
            p_feat->ss_telecom_type    = ksStat->ir.rKSCF.
                                                      cfFeatLst[idx].bsTp;
            p_feat->ss_telecom_service = ksStat->ir.rKSCF.
                                                      cfFeatLst[idx].bsCd;
            p_feat->ss_status = ksStat->ir.rKSCF.cfFeatLst[idx].ssSt;
            p_feat->ss_numbering_type = (UBYTE)phb_cvtTon((T_ACI_TOA_TON)ksStat->ir.rKSCF.
                                                                cfFeatLst[idx].ton); /*a0393213 compiler warnings removal - explicit type casting done*/
            p_feat->ss_numbering_plan = (UBYTE)phb_cvtNpi((T_ACI_TOA_NPI)ksStat->ir.rKSCF.
                                                                cfFeatLst[idx].npi);  /*a0393213 compiler warnings removal - explicit type casting done*/
            strncpy((char *)p_feat->ss_forwarded_to_number,
                    (char *)ksStat->ir.rKSCF.cfFeatLst[idx].num,
                    MFW_SS_NUM_LEN);
            strncpy((char *)p_feat->ss_forwarded_to_subaddress,
                    (char *)ksStat->ir.rKSCF.cfFeatLst[idx].sub,
                    MFW_SS_SUBADR_LEN);
            p_feat->ss_no_reply_condition_time = ksStat->ir.rKSCF.
                                                              cfFeatLst[idx].time;
          }
//	Apr 06 2005		REF: CRR 29989	xpradipg          
#ifdef FF_MMI_OPTIM
		if(NULL != cf_cnf)
          cf_cnf->ss_feature_count = idx;
#else
          cf_cnf.ss_feature_count = idx;
#endif          
	TRACE_EVENT("rat_percentksir->KSD_CMD_CF");
          if (ksStat->srcId != CMD_SRC_LCL)
//	Apr 06 2005		REF: CRR 29989	xpradipg          
#ifdef FF_MMI_OPTIM
			{	
				
				ss_signal(E_SS_CF_ICN_UPD,cf_cnf);
			}				
#else
 	            ss_signal(E_SS_CF_ICN_UPD, &cf_cnf);
#endif

          break;

        case KSD_CMD_CL:
          TRACE_EVENT("KSD_CMD_CL");
          TRACE_EVENT_P1("ksStat->ir.rKSCL.ssCd %d", ksStat->ir.rKSCL.ssCd);
          TRACE_EVENT_P1("ksStat->ir.rKSCL.opCd %d", ksStat->ir.rKSCL.opCd);
          TRACE_EVENT_P1("ksStat->ir.rKSCL.ssErr %d", ksStat->ir.rKSCL.ssErr);
          TRACE_EVENT_P1("ksStat->ir.rKSCL.ssSt %d", ksStat->ir.rKSCL.ssSt);
          TRACE_EVENT_P1("ksStat->ir.rKSCL.mode %d", ksStat->ir.rKSCL.mode);
          TRACE_EVENT_P1("ksStat->ir.rKSCL.clirOpt %d", ksStat->ir.rKSCL.clirOpt);
          TRACE_EVENT_P1("ksStat->ir.rKSCL.ovrdCtg %d", ksStat->ir.rKSCL.ovrdCtg);
//	Apr 06 2005		REF: CRR 29989	xpradipg          
#ifdef FF_MMI_OPTIM
		  cli_cnf = (T_MFW_SS_CLI_CNF*)mfwAlloc(sizeof(T_MFW_SS_CLI_CNF));
		  if(NULL != cli_cnf)
		  {
	      	cli_cnf->ss_code = (T_MFW_SS_CODES)ss_cvtSCode((T_ACI_KSD_SS)ksStat->ir.rKSCL.ssCd);  /*a0393213 compiler warning removal - explicit typecasting done*/
          	cli_cnf->ss_category = (T_MFW_SS_CATEGORY)ss_cvtOCode((T_ACI_KSD_OP)ksStat->ir.rKSCL.opCd);/*a0393213 compiler warning removal - explicit typecasting done*/
          	cli_cnf->ss_error     = (T_MFW_SS_ERROR)ss_cvtErrCode((T_ACI_KSD_ERR)ksStat->ir.rKSCL.ssErr);/*a0393213 compiler warning removal - explicit typecasting done*/
          	cli_cnf->ss_status      = ksStat->ir.rKSCL.ssSt;
          	cli_cnf->ss_clir_option = (T_MFW_CL_STATUS)ss_cvtCLStatus((T_ACI_KSD_CLIR_OP)ksStat->ir.rKSCL.clirOpt);/*a0393213 compiler warning removal - explicit typecasting done*/
          	cli_cnf->ss_ovrd_ctgry  = (T_MFW_OV_CTGRY)ss_cvtCLOvrd((T_ACI_KSD_OVRD_CTG)ksStat->ir.rKSCL.ovrdCtg);/*a0393213 compiler warning removal - explicit typecasting done*/
		  }
#else
	      cli_cnf.ss_code = (T_MFW_SS_CODES)ss_cvtSCode((T_ACI_KSD_SS)ksStat->ir.rKSCL.ssCd);
          cli_cnf.ss_category = (T_MFW_SS_CATEGORY)ss_cvtOCode((T_ACI_KSD_OP)ksStat->ir.rKSCL.opCd);
          cli_cnf.ss_error     = (T_MFW_SS_ERROR)ss_cvtErrCode((T_ACI_KSD_ERR)ksStat->ir.rKSCL.ssErr);

          cli_cnf.ss_status      = ksStat->ir.rKSCL.ssSt;
          cli_cnf.ss_clir_option = (T_MFW_CL_STATUS)ss_cvtCLStatus((T_ACI_KSD_CLIR_OP)ksStat->ir.rKSCL.clirOpt);/*a0393213 compiler warning removal - explicit typecasting done*/
          cli_cnf.ss_ovrd_ctgry  = (T_MFW_OV_CTGRY)ss_cvtCLOvrd((T_ACI_KSD_OVRD_CTG)ksStat->ir.rKSCL.ovrdCtg);
#endif          
          break;

        case KSD_CMD_CW:
          TRACE_EVENT("KSD_CMD_CW");
          TRACE_EVENT_P1("ksStat->ir.rKSCW.ssCd %d", ksStat->ir.rKSCW.ssCd);
          TRACE_EVENT_P1("ksStat->ir.rKSCW.opCd %d", ksStat->ir.rKSCW.opCd);
          TRACE_EVENT_P1("ksStat->ir.rKSCW.ssErr %d", ksStat->ir.rKSCW.ssErr);
          TRACE_EVENT_P1("ksStat->ir.rKSCW.ssSt %d", ksStat->ir.rKSCW.ssSt);
          TRACE_EVENT_P1("ksStat->ir.rKSCW.c_cwBSGLst %d", ksStat->ir.rKSCW.c_cwBSGLst);
          if (ksStat->ir.rKSCW.c_cwBSGLst)
          {
            int i;
            for (i = 0; i < ksStat->ir.rKSCW.c_cwBSGLst; i++)
            {
              TRACE_EVENT_P2("ksStat->ir.rKSCW.cwBSGLst[%d].bsTp %d", i, ksStat->ir.rKSCW.cwBSGLst[i].bsTp);
              TRACE_EVENT_P2("ksStat->ir.rKSCW.cwBSGLst[%d].bsCd %d", i, ksStat->ir.rKSCW.cwBSGLst[i].bsCd);
            }
          }
          p_srv = (T_MFW_SRV_GROUP *) ssLstBuf;
//		Apr 06 2005		REF: CRR 29989	xpradipg          
#ifdef FF_MMI_OPTIM
		  cw_cnf = (T_MFW_SS_CW_CNF*)mfwAlloc(sizeof(T_MFW_SS_CW_CNF));
		  if(cw_cnf != NULL)
		  {
	      	cw_cnf->ss_code =(T_MFW_SS_CODES) ss_cvtSCode((T_ACI_KSD_SS)ksStat->ir.rKSCW.ssCd);/*a0393213 compiler warning removal - explicit typecasting done*/
	        cw_cnf->ss_category = (T_MFW_SS_CATEGORY)ss_cvtOCode((T_ACI_KSD_OP)ksStat->ir.rKSCW.opCd);/*a0393213 compiler warning removal - explicit typecasting done*/
   	    	cw_cnf->ss_error    = (T_MFW_SS_ERROR)ss_cvtErrCode((T_ACI_KSD_ERR)ksStat->ir.rKSCW.ssErr);/*a0393213 compiler warning removal - explicit typecasting done*/
    	    cw_cnf->ss_status   = ksStat->ir.rKSCW.ssSt;
            cw_cnf->ss_service_list = p_srv;
          }
        
#else	
	      cw_cnf.ss_code = (T_MFW_SS_CODES)ss_cvtSCode((T_ACI_KSD_SS)ksStat->ir.rKSCW.ssCd);
          cw_cnf.ss_category = (T_MFW_SS_CATEGORY)ss_cvtOCode((T_ACI_KSD_OP)ksStat->ir.rKSCW.opCd);
          cw_cnf.ss_error    = (T_MFW_SS_ERROR)ss_cvtErrCode((T_ACI_KSD_ERR)ksStat->ir.rKSCW.ssErr);
          cw_cnf.ss_status   = ksStat->ir.rKSCW.ssSt;

          cw_cnf.ss_service_list = p_srv;
#endif
          for( idx = 0;
               idx < ksStat->ir.rKSCW.c_cwBSGLst AND
               idx < MFW_MAX_SRV_NR;
               idx++, p_srv++ )
          {
            p_srv->ss_telecom_type    = ksStat->ir.rKSCW.
                                                       cwBSGLst[idx].bsTp;
            p_srv->ss_telecom_service = ksStat->ir.rKSCW.
                                                       cwBSGLst[idx].bsCd;
          }
//	Apr 06 2005		REF: CRR 29989	xpradipg          
#ifdef FF_MMI_OPTIM
		if(NULL != cw_cnf)
		  cw_cnf->ss_service_count = idx;
#else
          cw_cnf.ss_service_count = idx;
#endif          
          break;

      case KSD_CMD_PWD:
          TRACE_EVENT("KSD_CMD_PWD");
          TRACE_EVENT_P1("ksStat->ir.rKSPW.ssCd %d", ksStat->ir.rKSPW.ssCd);
          TRACE_EVENT_P1("ksStat->ir.rKSPW.opCd %d", ksStat->ir.rKSPW.opCd);
          TRACE_EVENT_P1("ksStat->ir.rKSPW.ssErr %d", ksStat->ir.rKSPW.ssErr);
          TRACE_EVENT_P1("ksStat->ir.rKSPW.newPwd %s", ksStat->ir.rKSPW.newPwd);
//		Apr 06 2005		REF: CRR 29989	xpradipg          
#ifdef FF_MMI_OPTIM
		  pw_cnf = (T_MFW_SS_PW_CNF*)mfwAlloc(sizeof(T_MFW_SS_PW_CNF));
		  if( NULL != pw_cnf)		
		  {
            pw_cnf->ss_code      = (T_MFW_SS_CODES)ss_cvtSCode((T_ACI_KSD_SS)ksStat->ir.rKSPW.ssCd);/*a0393213 compiler warning removal - explicit typecasting done*/
          	pw_cnf->ss_category  = (T_MFW_SS_CATEGORY)ss_cvtOCode((T_ACI_KSD_OP)ksStat->ir.rKSPW.opCd);/*a0393213 compiler warning removal - explicit typecasting done*/
          	pw_cnf->ss_error     = (T_MFW_SS_ERROR)ss_cvtErrCode((T_ACI_KSD_ERR)ksStat->ir.rKSPW.ssErr);/*a0393213 compiler warning removal - explicit typecasting done*/

          	strncpy( (char *)pw_cnf->ss_new_pwd, (char *)ksStat->ir.rKSPW.newPwd,
                   MFW_SS_PWD_LEN);
		  }	                   
#else
          pw_cnf.ss_code      = (T_MFW_SS_CODES)ss_cvtSCode((T_ACI_KSD_SS)ksStat->ir.rKSPW.ssCd);
          pw_cnf.ss_category  = (T_MFW_SS_CATEGORY)ss_cvtOCode((T_ACI_KSD_OP)ksStat->ir.rKSPW.opCd);
          pw_cnf.ss_error     = (T_MFW_SS_ERROR)ss_cvtErrCode((T_ACI_KSD_ERR)ksStat->ir.rKSPW.ssErr);

          strncpy( (char *)pw_cnf.ss_new_pwd, (char *)ksStat->ir.rKSPW.newPwd,
                   MFW_SS_PWD_LEN);
#endif                   
          break;

	  case KSD_CMD_IMEI:
          TRACE_EVENT("KSD_CMD_IMEI");
          TRACE_EVENT_P1("ksStat->ir.rKSIMEI.tac1 %d", ksStat->ir.rKSIMEI.tac1);
          TRACE_EVENT_P1("ksStat->ir.rKSIMEI.tac2 %d", ksStat->ir.rKSIMEI.tac2);
          TRACE_EVENT_P1("ksStat->ir.rKSIMEI.tac3 %d", ksStat->ir.rKSIMEI.tac3);
          TRACE_EVENT_P1("ksStat->ir.rKSIMEI.fac %d", ksStat->ir.rKSIMEI.fac);
          TRACE_EVENT_P1("ksStat->ir.rKSIMEI.snr1 %d", ksStat->ir.rKSIMEI.snr1);
          TRACE_EVENT_P1("ksStat->ir.rKSIMEI.snr2 %d", ksStat->ir.rKSIMEI.snr2);
          TRACE_EVENT_P1("ksStat->ir.rKSIMEI.snr3 %d", ksStat->ir.rKSIMEI.snr3);
          TRACE_EVENT_P1("ksStat->ir.rKSIMEI.svn %d", ksStat->ir.rKSIMEI.svn);
          TRACE_EVENT_P1("ksStat->ir.rKSIMEI.cd %d", ksStat->ir.rKSIMEI.cd);
//	Apr 06 2005		REF: CRR 29989	xpradipg          
#ifdef FF_MMI_OPTIM
		  imei_info = (T_MFW_IMEI*)mfwAlloc(sizeof(T_MFW_IMEI));
		  if( NULL != imei_info)
		  {
#endif
          ss_decode_imei(ksStat);
//	Apr 06 2005		REF: CRR 29989	xpradipg          
#ifdef FF_MMI_OPTIM
		  imei_info->error = MFW_SS_NO_ERROR;
		  ss_signal(E_SS_IMEI, imei_info);
		  }
#else		  
          imei_info.error = MFW_SS_NO_ERROR;
			    ss_signal(E_SS_IMEI, &imei_info);
#endif
		      break;

      case KSD_CMD_USSD:
          TRACE_EVENT("KSD_CMD_USSD");
          TRACE_EVENT_P1("ksStat->ir.rKSUS.dcs 0x%02X", ksStat->ir.rKSUS.dcs);
          TRACE_EVENT_P1("ksStat->ir.rKSUS.mode %d", ksStat->ir.rKSUS.mode);
          TRACE_EVENT_P1("ksStat->ir.rKSUS.ssErr %d", ksStat->ir.rKSUS.ssErr);
          memset(&ussd_info, 0, sizeof(ussd_info));

	   /*  OMAPS00079722 - Null Check has been added */
          if( ksStat->ir.rKSUS.ussd != NULL )
          {
          	   TRACE_EVENT_P1("ksStat->ir.rKSUS.ussd %s", ksStat->ir.rKSUS.ussd);
          ussd_info.len = strlen((char *)ksStat->ir.rKSUS.ussd);
          strcpy((char*)ussd_info.ussd, (char *)ksStat->ir.rKSUS.ussd);
						}
		//x0pleela 28 Aug, 2006 DR: OMAPS00083503
		//Removed the hardcoded dcs value "MFW_ASCII"
          ussd_info.dcs = (T_MFW_DCS)(ksStat->ir.rKSUS.dcs);
          ussd_info.error    = ss_cvtErrCode((T_ACI_KSD_ERR)ksStat->ir.rKSUS.ssErr);

		//x0pleela 28 Aug, 2006 DR: OMAPS00083503
		//sending ss signal E_SS_USSD_REQ or E_SS_USSD_CNF based on user action required or not
		if (ksStat->ir.rKSUS.mode == CUSD_MOD_YesActReq)
			 ss_signal(E_SS_USSD_REQ, &ussd_info);
		if (ksStat->ir.rKSUS.mode == CUSD_MOD_NoActReq || ksStat->ir.rKSUS.mode==CUSD_MOD_TerminatedByNetwork  || 
				  ksStat->ir.rKSUS.mode == CUSD_MOD_OperationNotSupported || ksStat->ir.rKSUS.mode ==CUSD_MOD_NetworkTimeout)
		        ss_signal(E_SS_USSD_CNF, &ussd_info);
	
          break;

        default:
          TRACE_EVENT("default");
          break;
    }
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SS               |
| STATE  : code                         ROUTINE: ss_command_info     |
+--------------------------------------------------------------------+


   PURPOSE :  Notify for send a SS string from ACI

*/

void ss_command_info(T_ACI_CLOG* cmd)
{
//	Apr 06 2005		REF: CRR 29989	xpradipg
//	the variable is made local since the scope lies within this function
#ifdef FF_MMI_OPTIM
	    T_MFW_SS_RES       ss_res;
#endif	    
    TRACE_FUNCTION("ss_command_info()");

    if (cmd->cmdPrm.sCFUN.srcId != CMD_SRC_LCL)
		return;

    switch ((int)(cmd->atCmd)) /*a0393213 - typecasting done to avoid lint warning*/
    {
        case KSD_CMD_CB:
            ss_res.type     = ss_cvtSCode((T_ACI_KSD_SS)cmd->cmdPrm.sKSCB.ssCd);
            ss_res.category = ss_cvtOCode((T_ACI_KSD_OP)cmd->cmdPrm.sKSCB.opCd);
            if( cmd->cmdPrm.sKSCB.pwd )
              strncpy(ss_res.ppn, (char *)cmd->cmdPrm.sKSCB.pwd,
                     MAX_PWD_LEN);
            else
              ss_res.ppn[0] = 0;
            ss_res.pwd[0] = 0;
            ss_res.bs = cmd->cmdPrm.sKSCB.bsCd;
            ss_res.time = 0;
            ss_signal(E_SS_RES, &ss_res);
        		break;

        case KSD_CMD_CF:
            ss_res.type = ss_cvtSCode((T_ACI_KSD_SS)cmd->cmdPrm.sKSCF.ssCd);
            ss_res.category = ss_cvtOCode((T_ACI_KSD_OP)cmd->cmdPrm.sKSCF.opCd);
            ss_res.ppn[0] = 0;
            ss_res.pwd[0] = 0;
            ss_res.bs = cmd->cmdPrm.sKSCF.bsCd;
            ss_res.time = cmd->cmdPrm.sKSCF.time;
            ss_signal(E_SS_RES, &ss_res);
        		break;

        case KSD_CMD_CW:
            ss_res.type = SS_CW;
            ss_res.category = ss_cvtOCode((T_ACI_KSD_OP)cmd->cmdPrm.sKSCW.opCd);
            ss_res.ppn[0] = 0;
            ss_res.pwd[0] = 0;
            ss_res.bs = cmd->cmdPrm.sKSCW.bsCd;
            ss_res.time = 0;
            ss_signal(E_SS_RES, &ss_res);
        		break;

        case KSD_CMD_PWD:
			      switch(cmd->cmdPrm.sKSPW.ssCd)
			      {
				        case KSD_SS_CCBS:
				        case KSD_SS_ALL_CBSS:
				        case KSD_SS_BOC:
				        case KSD_SS_BAOC:
				        case KSD_SS_BOIC:
				        case KSD_SS_BOICXH:
				        case KSD_SS_BIC:
				        case KSD_SS_BAIC:
				        case KSD_SS_BICRM:


						/*NM 190602*/
						case KSD_SS_ALL_SERV :

						
					          ss_res.type = ss_cvtSCode((T_ACI_KSD_SS)cmd->cmdPrm.sKSPW.ssCd);
					          ss_res.category = SS_REGISTRATION;
                    if( cmd->cmdPrm.sKSPW.oldPwd )
					              strncpy(ss_res.ppn, (char *)cmd->cmdPrm.sKSPW.oldPwd,
                                MAX_PWD_LEN);
                    else
                        ss_res.ppn[0] = 0;
                    if( cmd->cmdPrm.sKSPW.newPwd )
					              strncpy(ss_res.pwd, (char *)cmd->cmdPrm.sKSPW.newPwd,
                                MAX_PWD_LEN);
                    else
                        ss_res.pwd[0] = 0;
					          ss_res.bs = 0;
					          ss_res.time = 0;
					          ss_signal(E_SS_RES, &ss_res);
        			      break;

				        case KSD_SS_PIN1:
                    pwd_flag = 1;
                    sim_pin_ident(MFW_SIM_PIN1);
					          break;
				        case KSD_SS_PIN2:
					          pwd_flag = 1;
                    sim_pin_ident(MFW_SIM_PIN2);
					          break;
	          }
            break;

        case KSD_CMD_UBLK:
            switch(cmd->cmdPrm.sKSPW.ssCd)
			      {
                case KSD_SS_PIN1:
                    pwd_flag = 1;
                    sim_pin_ident(MFW_SIM_PUK1);
					          break;
				        case KSD_SS_PIN2:
					          pwd_flag = 1;
                    sim_pin_ident(MFW_SIM_PUK2);
					          break;
            }
            break;

        case KSD_CMD_CL:
            ss_res.type = ss_cvtSCode((T_ACI_KSD_SS)cmd->cmdPrm.sKSCL.ssCd);
            ss_res.category = ss_cvtOCode((T_ACI_KSD_OP)cmd->cmdPrm.sKSCL.opCd);
            ss_res.ppn[0] = 0;
            ss_res.pwd[0] = 0;
            ss_res.bs = 0;
            ss_res.time = 0;
            ss_signal(E_SS_RES, &ss_res);
        		break;

        case KSD_CMD_USSD:
            memset(&ussd_info, 0, sizeof(ussd_info));
            if( cmd->cmdPrm.sKSUS.ussd )
            {
              ussd_info.len = strlen((char *)cmd->cmdPrm.sKSUS.ussd);
              strcpy((char*)ussd_info.ussd, (char *)cmd->cmdPrm.sKSUS.ussd);
            }
            else
              ussd_info.ussd[0] = 0;
            ussd_info.dcs = MFW_ASCII;
            //PATCH TB 1509
            ss_res.type = SS_USSD;
            ss_res.category = SS_CTG_UNKNOWN;
            ss_res.ppn[0] = 0;
            ss_res.pwd[0] = 0;
            ss_res.bs = 0;
            ss_res.time = 0;
            ss_signal(E_SS_RES, &ss_res);
            //END PATCH TB
            break;

        case AT_CMD_CUSD:
            memset(&ussd_info, 0, sizeof(ussd_info));
            ussd_info.len = cmd->cmdPrm.sCUSD.str->len;
            memcpy((char*)ussd_info.ussd, cmd->cmdPrm.sCUSD.str->data, cmd->cmdPrm.sCUSD.str->len);
            ussd_info.dcs = (T_MFW_DCS)cmd->cmdPrm.sCUSD.dcs;
/* SPR#CQ3204 -JVJ - USSD call back sent to MMI */
            ss_res.type = SS_USSD;
            ss_res.category = SS_CTG_UNKNOWN;
            ss_res.ppn[0] = 0;
            ss_res.pwd[0] = 0;
            ss_res.bs = 0;
            ss_res.time = 0;
            ss_signal(E_SS_RES, &ss_res);
/* SPR#CQ3204 -JVJ */
            break;
        default:
		    break;
    }
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SS               |
| STATE  : code                         ROUTINE: ss_ok_string        |
+--------------------------------------------------------------------+


   PURPOSE :  Send of SS string is successful.

*/

void ss_ok_string(T_ACI_AT_CMD cmdId)
{
    TRACE_FUNCTION("ss_ok_string()");

    switch ((int)cmdId)
    {
        case KSD_CMD_CB:
//	Apr 06 2005		REF: CRR 29989	xpradipg
//	free the allocated memory once the ss_signal() execution is done
#ifdef FF_MMI_OPTIM
			TRACE_EVENT("KSD_CMD_CB");
			if(cb_cnf)
			{
				ss_signal(E_SS_CB_CNF, cb_cnf);
				mfwFree((U8*)cb_cnf, sizeof(T_MFW_SS_CB_CNF));
			}
#else        
            ss_signal(E_SS_CB_CNF, &cb_cnf);
#endif         
            break;
        case KSD_CMD_CF:
//	Apr 06 2005		REF: CRR 29989	xpradipg        
#ifdef FF_MMI_OPTIM
			TRACE_EVENT("KSD_CMD_CF");
			if(cf_cnf)
			{
				ss_signal(E_SS_CF_CNF, cf_cnf);
				mfwFree((U8*)cf_cnf, sizeof(T_MFW_SS_CF_CNF));
			}
#else        
            ss_signal(E_SS_CF_CNF, &cf_cnf);
#endif            
            
            break;
        case KSD_CMD_CW:
//	Apr 06 2005		REF: CRR 29989	xpradipg        
#ifdef FF_MMI_OPTIM
			TRACE_EVENT("KSD_CMD_CW");
			if(cw_cnf)
			{
				ss_signal(E_SS_CW_CNF, cw_cnf);
				mfwFree((U8*)cw_cnf, sizeof(T_MFW_SS_CW_CNF));
			}
#else        
            ss_signal(E_SS_CW_CNF, &cw_cnf);
#endif           
            break;
        case KSD_CMD_PWD:
//	Apr 06 2005		REF: CRR 29989	xpradipg        
#ifdef FF_MMI_OPTIM
			TRACE_EVENT("KSD_CMD_PW");
			if(pw_cnf)
			{
				if (!pwd_flag)
					ss_signal(E_SS_GET_PW, pw_cnf);
				mfwFree((U8*)pw_cnf,sizeof(T_MFW_SS_PW_CNF));
			}
#else        
            if (!pwd_flag)
				ss_signal(E_SS_GET_PW, &pw_cnf);
#endif           
			
			if (pwd_flag == 1)
			{
				pwd_flag = 0;
				sim_ok_cpinc();
			}
        	break;
        case KSD_CMD_UBLK:
            sim_ss_unblock(SIM_UBLK_OK, CME_ERR_NotPresent);
            break;
        case KSD_CMD_CL:
//	Apr 06 2005		REF: CRR 29989	xpradipg        
#ifdef FF_MMI_OPTIM
			TRACE_EVENT("KSD_CMD_CLI");
			if(cli_cnf)
			{
				ss_signal(E_SS_CLI_CNF,(void*) cli_cnf);
				mfwFree((U8*)cli_cnf, sizeof(T_MFW_SS_CLI_CNF));
			}
#else
			ss_signal(E_SS_CLI_CNF,&cli_cnf);
#endif			
            break;
		case KSD_CMD_IMEI:
//	Apr 06 2005		REF: CRR 29989	xpradipg		
#ifdef FF_MMI_OPTIM
			if( imei_info)
			{
				imei_info->error = MFW_SS_NO_ERROR;
				ss_signal(E_SS_IMEI, imei_info);
				mfwFree((U8*)imei_info,sizeof(T_MFW_IMEI));
			}
#else
			imei_info.error = MFW_SS_NO_ERROR;
			ss_signal(E_SS_IMEI, &imei_info);
#endif			
			break;
        default:
            break;
    	}
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SS               |
| STATE  : code                         ROUTINE: ss_error_string     |
+--------------------------------------------------------------------+


   PURPOSE :   Send of SS string is failed.

*/

void ss_error_string(T_ACI_AT_CMD cmdId, T_ACI_CME_ERR reason)
{
    TRACE_FUNCTION("ss_error_string()");

    switch ((int)cmdId)
    {
        case KSD_CMD_CB:
//	Apr 06 2005		REF: CRR 29989	xpradipg
//	deallocation occurs either in ok/ error since the response is always is
//	either one of them
 #ifdef FF_MMI_OPTIM
 			TRACE_EVENT("KSD_CMD_CB");
 			if(cb_cnf)
 			{
				ss_signal(E_SS_CB_CNF, cb_cnf);
				mfwFree((U8*)cb_cnf,sizeof(T_MFW_SS_CB_CNF));
			}
#else        
            ss_signal(E_SS_CB_CNF, &cb_cnf);
#endif         
            break;
        case KSD_CMD_CF:
//	Apr 06 2005		REF: CRR 29989	xpradipg        
#ifdef FF_MMI_OPTIM
			TRACE_EVENT("KSD_CMD_CF");
			if(cf_cnf)
			{
				ss_signal(E_SS_CF_CNF, cf_cnf);
				mfwFree((U8*)cf_cnf,sizeof(T_MFW_SS_CF_CNF));
				TRACE_FUNCTION("memory freed for cf_cnf");
			}
#else        
            ss_signal(E_SS_CF_CNF, &cf_cnf);
#endif            
            break;
        case KSD_CMD_CW:
//	Apr 06 2005		REF: CRR 29989	xpradipg        
#ifdef FF_MMI_OPTIM       
			TRACE_EVENT("KSD_CMD_CW");
			if(cw_cnf)
			{
            	ss_signal(E_SS_CW_CNF, cw_cnf);
               	mfwFree((U8*)E_SS_CW_CNF,sizeof(T_MFW_SS_CW_CNF));
            }
#else            
            ss_signal(E_SS_CW_CNF, &cw_cnf);
#endif            
            break;
       case KSD_CMD_PWD:
//	Apr 06 2005		REF: CRR 29989	xpradipg       
#ifdef FF_MMI_OPTIM       
			TRACE_EVENT("KSD_CMD_PW");
			if(pw_cnf)
			{
		    	if (!pwd_flag)
					ss_signal(E_SS_GET_PW, pw_cnf);
				mfwFree((U8*)pw_cnf,sizeof(T_MFW_SS_PW_CNF));
			}
#else
		    if (!pwd_flag)
				ss_signal(E_SS_GET_PW, &pw_cnf);
#endif				
			if (pwd_flag == 1)
			{
				pwd_flag = 0;
				sim_error_cpinc();
			}
      		break;
        case KSD_CMD_UBLK:
            sim_ss_unblock(SIM_UBLK_ERROR, reason);
            break;
        case KSD_CMD_CL:
//	Apr 06 2005		REF: CRR 29989	xpradipg        
#ifdef FF_MMI_OPTIM
			TRACE_EVENT("KSD_CMD_CLI");
			if(cli_cnf)
			{
				ss_signal(E_SS_CLI_CNF, cli_cnf);
				mfwFree((U8*)cli_cnf,sizeof(T_MFW_SS_CLI_CNF));
			}
#else
			ss_signal(E_SS_CLI_CNF,&cli_cnf);
#endif	
		case KSD_CMD_IMEI:
//	Apr 06 2005		REF: CRR 29989	xpradipg		
#ifdef FF_MMI_OPTIM
			if( imei_info)
			{
				imei_info->error = MFW_SS_ERROR;
				ss_signal(E_SS_IMEI, imei_info);
				mfwFree((U8*)imei_info,sizeof(T_MFW_IMEI));
			}
#else
			imei_info.error = MFW_SS_ERROR;
			ss_signal(E_SS_IMEI, &imei_info);
#endif				
              break;
        default:
            break;
    }
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SS               |
| STATE  : code                         ROUTINE: ss_set_clck         |
+--------------------------------------------------------------------+


   PURPOSE :   This function execute the facility lock command.

*/
/*a0393213 warnings removal - T_ACI_CLCK_FAC changed to T_ACI_FAC as a result of ACI enum change*/
T_MFW ss_set_clck(T_ACI_FAC fac,			
                  T_ACI_CLCK_MOD mode,
                  CHAR *passwd,
                  T_ACI_CLASS class_type,
                  UBYTE modul)
{
    TRACE_EVENT("ss_set_clck()");

    clck_flag = modul;

    if (sAT_PlusCLCK(CMD_SRC_LCL, fac,mode, passwd, class_type) != AT_EXCT)
        return MFW_SS_FAIL;
    return MFW_SS_OK;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SS             |
| STATE   : code                        ROUTINE : ss_ok_clck         |
+--------------------------------------------------------------------+

  PURPOSE : The executing of the facility lock command is successful.

*/

void ss_ok_clck()
{
    TRACE_FUNCTION("ss_ok_clck()");

    switch (clck_flag)
    {
        case MFW_PHB:
            phb_ok_clck();
            break;
        case MFW_SIM_CPIND:
            sim_ok_cpind();
            break;
        case MFW_SIM_CPINE:
            sim_ok_cpine();
            break;
    }
    clck_flag = MFW_MODUL_UNKNOWN;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SS             |
| STATE   : code                        ROUTINE : ss_error_clck      |
+--------------------------------------------------------------------+

  PURPOSE : The executing of the facility lock command is failed.

*/

void ss_error_clck()
{
    TRACE_FUNCTION("ss_error_clck()");

    switch (clck_flag)
    {
        case MFW_PHB:
            phb_error_clck();
            break;
        case MFW_SIM_CPIND:
            sim_error_cpind();
            break;
        case MFW_SIM_CPINE:
            sim_error_cpine();
            break;
    }
    clck_flag = MFW_MODUL_UNKNOWN;
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_DMY              |
| STATE  : code                         ROUTINE: ss_send_ussd        |
+--------------------------------------------------------------------+


   PURPOSE :  Send USSD string.
              Note: ASCII is not allowed to use.

*/

T_MFW ss_send_ussd(T_MFW_SS_USSD *ussd_info)
{
    T_ACI_USSD_DATA str;
    T_ACI_RETURN    res;

    TRACE_FUNCTION("ss_send_ussd()");

    if (!ussd_info->len OR !ussd_info->ussd)
        return MFW_SS_FAIL;

    if (ussd_info->dcs == MFW_ASCII)
        return MFW_SS_FAIL;

    if (ussd_info->len > MAX_USSD_LEN)
    {
        str.len = MAX_USSD_LEN;
        memcpy(str.data, ussd_info->ussd, MAX_USSD_LEN);
    }
    else
    {
        str.len = ussd_info->len;
        memcpy(str.data, ussd_info->ussd, ussd_info->len);
    }
    res = sAT_PlusCUSD(CMD_SRC_LCL, &str, (SHORT)ussd_info->dcs);
    if ((res != AT_EXCT) AND (res != AT_CMPL))
        return MFW_SS_FAIL;

    return MFW_SS_OK;
}



/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_DMY              |
| STATE  : code                         ROUTINE: rAT_PlusCUSD        |
+--------------------------------------------------------------------+


   PURPOSE :  Notify or response for USSD from ACI

*/

void rAT_PlusCUSD(T_ACI_CUSD_MOD m, T_ACI_USSD_DATA *ussd, SHORT dcs)
{
	
    TRACE_FUNCTION("rAT_PlusCUSD()");

//Dec 16, 2005 OMAPS00053316	x0018858
//Added the NullCheck to ensure that we dont access the elements when the pointer is NULL.
//Begin OMAPS00053316
	
if(ussd != NULL)
{
    ussd_info.len = ussd->len;
    memcpy(ussd_info.ussd, ussd->data, sizeof(ussd_info.ussd));	
}
else
{
    ussd_info.len = 0;
    memset(ussd_info.ussd, 0x00, sizeof(ussd_info.ussd));	
}
//End OMAPS00053316
    ussd_info.dcs = (T_MFW_DCS)dcs;

	if (m==CUSD_MOD_TerminatedByNetwork  || 
	    m== CUSD_MOD_OperationNotSupported ||
	    m==CUSD_MOD_NetworkTimeout)
	      ussd_info.error = MFW_SS_ERROR;
	else
    ussd_info.error = MFW_SS_NO_ERROR;

   /* Jun 30, 2006     REF : OMAPS00083503    R.Prabakar
    Description : When USSD string is sent, MMI shows requesting screen forever
    Solution     : Some cases in rAT_PlusCUSD weren't handled. They are handled now*/
    if (m == CUSD_MOD_YesActReq)
        ss_signal(E_SS_USSD_REQ, &ussd_info);
    if (m == CUSD_MOD_NoActReq || m==CUSD_MOD_TerminatedByNetwork  || m== CUSD_MOD_OperationNotSupported || m==CUSD_MOD_NetworkTimeout)
        ss_signal(E_SS_USSD_CNF, &ussd_info);
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_DMY              |
| STATE  : code                         ROUTINE: ss_ok_ussd          |
+--------------------------------------------------------------------+


   PURPOSE :  USSD process is successful.

*/

void ss_ok_ussd(T_ACI_AT_CMD cmdId)
{
    TRACE_FUNCTION("ss_ok_ussd()");

    if (cmdId == (T_ACI_AT_CMD)KSD_CMD_USSD) /*a0393213 typecast done to remove lint warning*/
    {
        ussd_info.error = MFW_SS_NO_ERROR;
        ss_signal(E_SS_USSD_CNF, &ussd_info);
    }
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SS               |
| STATE  : code                         ROUTINE: ss_error_ussd       |
+--------------------------------------------------------------------+


   PURPOSE :   USSD process is failed.

*/

void ss_error_ussd(T_ACI_AT_CMD cmdId, T_ACI_CME_ERR reason)
{
    TRACE_FUNCTION("ss_error_ussd()");

//    ussd_info.error = ss_cvtErrCode(reason);
    ss_signal(E_SS_USSD_CNF, &ussd_info);
}

/*
+---------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SS                |
| STATE  : code                         ROUTINE: ss_abort_transaction |
+---------------------------------------------------------------------+


   PURPOSE :   Abort executing of SS transaction

*/

T_MFW ss_abort_transaction(void)
{
  TRACE_FUNCTION("ss_abort_transaction()");
  
  if (sAT_Abort(CMD_SRC_LCL, AT_CMD_CUSD) EQ AT_CMPL)
    return TRUE;
  else
  {
    if (sAT_Abort(CMD_SRC_LCL, AT_CMD_D) EQ AT_CMPL)
      return TRUE;
  }
  return FALSE;
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SS               |
| STATE  : code                         ROUTINE: ss_set_cug          |
+--------------------------------------------------------------------+


   PURPOSE : set parameter of close user group

*/
T_MFW_SS_RETURN ss_set_cug (T_MFW_SS_CUG_PARA *cug_param)
{
	T_ACI_CCUG_MOD  cug_mode  = CCUG_MOD_NotPresent;
    T_ACI_CCUG_IDX  cug_index = CCUG_IDX_NotPresent;
    T_ACI_CCUG_INFO cug_info  = CCUG_INFO_NotPresent;

	TRACE_FUNCTION("ss_set_cug()");

/* check cug mode  */
	switch(cug_param->mode)
	{
		case MFW_SS_CUG_TMP_DISABLE: 
			cug_mode = CCUG_MOD_DisableTmp;
			break;
		case MFW_SS_CUG_TMP_ENABLE:
			cug_mode = CCUG_MOD_EnableTmp; /* enable to control cug info   */
			break;
		default:                           /* ACI keep last valid value    */
			break;
	}
/* check cug index  */
	switch(cug_param->index)
	{
		case MFW_SS_CUG_INDEX0:
			cug_index = CCUG_IDX_0;
			break;
		case MFW_SS_CUG_INDEX1:
			cug_index = CCUG_IDX_1;
			break;
		case MFW_SS_CUG_INDEX2:
			cug_index = CCUG_IDX_2;
			break;
		case MFW_SS_CUG_INDEX3:
			cug_index = CCUG_IDX_3;
			break;
		case MFW_SS_CUG_INDEX4:
			cug_index = CCUG_IDX_4;
			break;
		case MFW_SS_CUG_INDEX5:
			cug_index = CCUG_IDX_5;
			break;
		case MFW_SS_CUG_INDEX6:
			cug_index = CCUG_IDX_6;
			break;
		case MFW_SS_CUG_INDEX7:
			cug_index = CCUG_IDX_7;
			break;
		case MFW_SS_CUG_INDEX8:
			cug_index = CCUG_IDX_8;
			break;
		case MFW_SS_CUG_INDEX9:		
			cug_index = CCUG_IDX_9;
			break;
		case MFW_SS_CUG_NO_INDEX:
			cug_index = CCUG_IDX_No;  /* preferred CUG taken from subscriber data */
			break;
		default:                      /* ACI keep last valid value      */
			break;
	}

/* check cug info  */
	switch(cug_param->info)
	{
		case MFW_SS_CUG_SUPP_OA:
			cug_info = CCUG_INFO_SuppOa;      /* suppress outgoing access  */
			break;
		case MFW_SS_CUG_SUPP_PREF:
			cug_info = CCUG_INFO_SuppPrefCug; /* suppress preferential closed user group */
			break;
		case MFW_SS_CUG_SUPP_BOTH:
			cug_info = CCUG_INFO_SuppBoth;    /* suppress outgoing access and preferential cug */
			break;
		case MFW_SS_CUG_INFO_NO:              /* no cug info */
			cug_info = CCUG_INFO_No;
			break;
		default:                              /* ACI keep last valid value      */
			break;
	}
	if(sAT_PlusCCUG(CMD_SRC_LCL,cug_mode,cug_index,cug_info) EQ AT_CMPL) /* set in ACI cug parameters */
		return MFW_SS_OK;
	else
		return MFW_SS_FAIL;
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SS               |
| STATE  : code                         ROUTINE: ss_get_cug          |
+--------------------------------------------------------------------+


   PURPOSE : get parameter of close user group

*/

T_MFW_SS_RETURN ss_get_cug(T_MFW_SS_CUG_PARA *cug_param)
{
	T_ACI_CCUG_MOD  cug_mode;
    T_ACI_CCUG_IDX  cug_index;
    T_ACI_CCUG_INFO cug_info; 

	cug_param->mode =  MFW_SS_CUG_MODE_Not_Present;
	cug_param->index = MFW_SS_CUG_INDEX_Not_Present;
	cug_param->info =  MFW_SS_CUG_INFO_Not_Present;

    TRACE_FUNCTION("ss_get_cug()");

	if(qAT_PlusCCUG(CMD_SRC_LCL,&cug_mode,&cug_index,&cug_info) NEQ AT_CMPL) /* get cug parameters of ACI */
		return MFW_SS_FAIL;
	else
	{
		cug_param->mode = (T_MFW_SS_CUG_MODE)cug_mode;
		cug_param->index = (T_MFW_SS_CUG_INDEX)cug_index;
		cug_param->info = (T_MFW_SS_CUG_INFO)cug_info;
		return MFW_SS_OK;
	}
}
//PATCH TB 1309
/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_SS               |
| STATE  : code                         ROUTINE: ss_command_abort    |
+--------------------------------------------------------------------+


   PURPOSE :

*/
T_MFW ss_command_abort(void)
{
  TRACE_FUNCTION ("ss_command_abort()");

	// Abort SS transaction.
	// Any SS AT_CMD will clear all SS (any) transaction(s)
  if (sAT_Abort(CMD_SRC_LCL, AT_CMD_CUSD) == AT_CMPL)
		return TRUE;
	else
		return FALSE;
}
//END PATCH TB
//CQ-16432 start
void ss_sat_passwdReqd(T_ACI_AT_CMD cmdId, T_ACI_CME_ERR err)
{
	TRACE_FUNCTION("ss_sat_passwdReqd");
	ss_signal(E_SS_SAT_CPWD,&cmdId);
	return;
}
//CQ-16432 end

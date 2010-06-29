/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	$Workfile:: mfw_cphs.c	    $|
| $Author:: VO				$Revision::  1		    $|
| CREATED:  14.7.00			$Modtime::		    $|
| STATE  : code 						     |
+--------------------------------------------------------------------+

   MODULE  : MFW_CPHS

   PURPOSE : This modul contains CPHS management functions.

  
   $History:: mfw_cphs.c					      $

   	June 27,2007 OMAPS00134788 x0066814(Geetha)
   	Description: EXT1 record not appende to CHPHS Voice Mail Number (6F17) though referenced
   	Solution: Added a function to read the EXT1 records for mailbox number.
   	Added a condition in cphs_read_mbn() to check whether EXT1 is present.
   
 	Nov 27, 2006 OMAPS00098359 a0393213(R.Prabakar)
 	Description: CPHS 4.2 : Call Divert doesn't work properly for Line 2
 	   
      Oct 30 2006, OMAPS00098881 x0039928(sumanth)
      Removal of power variant
      
       May 30, 2006 REF:OMAPS00079650  a0393213 (R.Prabakar)
       Description : CPHS Call Forwarding feature implementation

   	May 30, 2006  DR: OMAPS00070657 x0pleela
	Description: CPHS feature on Locosto-Lite
	Solution: For ALS feature,
			a) Added the definition for the new function "mfw_cphs_get_als_info()"

	May 18, 2006    DR: OMAPS00070657 x0pleela
   	Description: CPHS feature on Locosto-Lite 
   	Solution: 	For VMWI feature,
   			a) A new function "sendCPWIEvt" is defined to set the voice mail indication 
   			if there are any voice mails

	Jan 12, 2006	OMAPS00062632	x0018858
	Description: The differnace in the type of the data passed to the function cphs_read_information()
	is causing an incorrect value being received as the output.
	Solution: The errcode being passed to the function has been modified to USHORT
	
	Mar 30, 2005	REF: CRR 29986	xpradipg
	Description: Optimisation 1: Removal of unused variables and dynamically
	allocate/ deallocate mbndata
	Solution: Static allocation of mbnData is replaced with dynamic allocation
	and deallocation
   
	Aug 16, 2004    REF: CRR 24323   Deepa M.D
	Bug:Clenup of sprintf used for tracing
	Fix:Replace the char buf[]; sprintf (buf, "...", ...); TRACE_EVENT (buf); statements by TRACE_EVENT_PX

*/

#define ENTITY_MFW

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

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
#include "mfw_sim.h"
#include "mfw_nm.h"
#include "mfw_simi.h"
#include "mfw_phb.h"
#include "mfw_phbi.h"
#include "mfw_cphs.h"
#include "mfw_cphsi.h"
#include "mfw_win.h"
#include "mfw_cm.h"
#include "mfw_ss.h"
#include "mfw_ffs.h"

#include "ksd.h"
#include "psa.h"

#ifdef SIM_TOOLKIT
#include "mfw_sat.h"
#endif

#if defined (FAX_AND_DATA)
#include "aci_fd.h"
#endif

#include "cmh.h"
#include "phb.h"
#include "cmh_phb.h"
#include "psa_sim.h"

#ifndef PCM_2_FFS
#include "pcm.h"
#include "mfw_ffs.h"
#endif

//x0pleela 11 june, 2006  DR: OMAPS00079692
//Including this header to get the ACI's VC Status and update MFW VC staus variable
#ifdef FF_CPHS
#include "cphs.h"
#endif

EXTERN MfwHdr * current_mfw_elem;
static int cphsCommand (U32 cmd, void *h); /* handle win commands    */

/* event parameter define */
static T_MFW_CPHS_OP_NAME	opName;
static T_MFW_CPHS_VC_NUM	mbNum;
static T_MFW_CPHS_VC_STAT	mbStatus;
static T_MFW_CPHS_DV_STAT	dvStatus;
static T_MFW_CPHS_ALS_STATUS	alsStatus;
static T_MFW_CPHS_CSP		csProfile;
static T_MFW_CPHS_INFONUM_LIST	numList;
static T_MFW_CPHS_INFONUM_ENTRY infoEntry;
#ifdef FF_CPHS_REL4
static T_MFW_CPHS_MSP_INFO	mspinfo;
T_MFW_CPHS_REL4_MWIS mwis_stat[4];
#endif
//	Mar 30, 2005	REF: CRR 29986	xpradipg
//	the optimisation is placed under a flag, a pointer variable is defined 
//	instead of the static array, which is allocated and deallocated
#ifdef FF_MMI_OPTIM
//x0pleela 14 June, 2006  DR: OMAPS00080549
//changed the size of the Mail box number to avoid SIM write operation to overwrite other memory locations
#ifdef FF_CPHS 
	#if defined ( TI_PS_FFS_PHB) || defined(PHONEBOOK_EXTENSION)
		#define MAX_MAIL_BOX_NUMBER       120
	#else//TI_PS_FFS_PHB OR PHONEBOOK_EXTENSION
		#define MAX_MAIL_BOX_NUMBER 120
	#endif//TI_PS_FFS_PHB OR PHONEBOOK_EXTENSION
#else 	//FF_CPHS
	#define MAX_MAIL_BOX_NUMBER 120
#endif //FF_CPHS
UBYTE *mbnData; 
#else //FF_MMI_OPTIM
//x0pleela 14 June, 2006  DR: OMAPS00080549
#ifdef FF_CPHS 
	#if defined ( TI_PS_FFS_PHB) || defined(PHONEBOOK_EXTENSION)
		static UBYTE	mbnData[120]; // it should be allocated dynamically.
	#else//TI_PS_FFS_PHB OR PHONEBOOK_EXTENSION
		static UBYTE	mbnData[120]; // it should be allocated dynamically.
	#endif//TI_PS_FFS_PHB OR PHONEBOOK_EXTENSION
#else 	//FF_CPHS
	static UBYTE		mbnData[120]; // it should be allocated dynamically.
#endif //FF_CPHS
#endif //FF_MMI_OPTIM

/* PATCH VO 22.01.01 */
static UBYTE			mbsData[MFW_CPHS_MBS_SIZE]; // it should be allocated dynamically.
static UBYTE			dvData[MFW_CPHS_CFF_SIZE]; // it should be allocated dynamically.
/* PATCH VO 22.01.01 end */
static T_MFW_CPHS_READ_SIM_ENTRY read_sim_entry;//MC
/* variable define */
static T_MFW_CPHS_STATUS	cphsStatus;
static UBYTE			cphsPhase;
static UBYTE			cphsServTab[CPHS_SERVICE_TABLE];
static UBYTE			maxRec;
/* PATCH JPS 16.01.01: store mailbox number entry */
static T_MFW_CPHS_ENTRY 	MbnEntry;
/* PATCH JPS 16.01.01 END */
static T_MFW_CPHS_ENTRY 	*vcEntry;
static UBYTE			simStatus;
static UBYTE			alsData;
static UBYTE			idxLevel;
static UBYTE			startIdx;
static UBYTE			cphsUpdate = FALSE;
static UINT16			cphsPrevRead = 0;
/*June 27,2007 OMAPS00134788 x0066814(Geetha)*/
static UBYTE 	curr_recNr;
static void cphs_read_ext1(SHORT table_id);
//Added by Muthu Raja For PCM Reolacement

#ifdef PCM_2_FFS
#define PSPDF_MBN_ID       "/PCM/MBN"           /* Mailbox Numbers          */
#endif

static UBYTE			ubCphsMbExtRecNo = 0;
static SHORT	 		sFreeRec = 0;
static UBYTE			ubCurrExtRcWrite = 0;
static UBYTE            ubFlushRecords = 0;
static UBYTE 			ubExtRecData[200];
static UBYTE 			flush_data[13];


void FFS_log_message(char *LogMsg,...);
#define TRACE_FUNCTION FFS_log_message
#define TRACE_FUNCTION_P1 FFS_log_message
#define TRACE_EVENT FFS_log_message
#define TRACE_EVENT_P1 FFS_log_message
#define TRACE_EVENT_P2 FFS_log_message
#define TRACE_EVENT_P3 FFS_log_message
#define TRACE_EVENT_P4 FFS_log_message

void FFS_log_message(char *LogMsg,...)
{  
       fd_t fd= -1;   
       char log_ffs_buf[256]; 
       va_list varpars;
 va_start (varpars, LogMsg);      
 vsprintf(log_ffs_buf,LogMsg,varpars);
 va_end (varpars);     
  fd = ffs_open((const char*)"/ffslog", FFS_O_APPEND | FFS_O_WRONLY );
        if(fd < 0) 
           fd=ffs_open((const char*)"/ffslog", FFS_O_CREATE |FFS_O_WRONLY );
        if(fd > 0)
        {
     strcat(log_ffs_buf, "\n");
           ffs_write(fd, log_ffs_buf, strlen(log_ffs_buf));
           ffs_close(fd);
         }
}
 

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE:  MFW_CPHS	     |
| STATE  : code 			ROUTINE: cphs_init	     |
+--------------------------------------------------------------------+


   PURPOSE :   initialize for CPHS management

*/

void cphs_init(void)
{
  TRACE_FUNCTION("cphs_init()");

  cphsStatus = CPHS_NotPresent;
  simStatus  = MFW_SIMOP_UNKNOWN;
  mfwCommand[MfwTypCphs] = (MfwCb) cphsCommand;
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE:  MFW_CPHS	     |
| STATE  : code 			ROUTINE: cphs_exit	     |
+--------------------------------------------------------------------+


   PURPOSE :

*/

void cphs_exit(void)
{
  TRACE_FUNCTION ("cphs_exit()");
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE:  MFW_CPHS	     |
| STATE  : code 			ROUTINE: cphs_create	     |
+--------------------------------------------------------------------+


   PURPOSE :  create event for CPHS management

*/

MfwHnd cphs_create (MfwHnd hWin, MfwEvt event, MfwCb cbfunc)
{
  MfwHdr      *hdr;
  T_MFW_CPHS  *cphs_para;
  MfwHdr *insert_status=0;

  TRACE_FUNCTION("cphs_create()");

  hdr = (MfwHdr *) mfwAlloc(sizeof(MfwHdr));
  cphs_para = (T_MFW_CPHS *) mfwAlloc(sizeof(T_MFW_CPHS));

  if (!hdr || !cphs_para)
  	{
    	TRACE_ERROR("ERROR: cphs_create() Mem Alloc Failed.");
			
	   	if(hdr)
   			mfwFree((U8*)hdr,sizeof(MfwHdr));

   		if(cphs_para)
   			mfwFree((U8*)cphs_para,sizeof(T_MFW_CPHS));
   		
	   	return FALSE;
  	}

  cphs_para->emask = event;
  cphs_para->handler = cbfunc;

  hdr->data = cphs_para;	      /* store parameter in node  */
  hdr->type = MfwTypCphs;	      /* type of event handler	  */

  insert_status= mfwInsert((MfwHdr *) hWin,hdr);
  if(!insert_status)
	{
  		TRACE_ERROR("ERROR: cphs_create() Failed to Install Handler. ");
   		mfwFree((U8*)hdr,sizeof(MfwHdr));
   		mfwFree((U8*)cphs_para,sizeof(T_MFW_CPHS));
		return 0;
  	}
    return insert_status;
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE:  MFW_CPHS	     |
| STATE  : code 			ROUTINE: cphs_delete	     |
+--------------------------------------------------------------------+


   PURPOSE :   delete a event for CPHS management

*/

MfwRes cphs_delete (MfwHnd h)
{
  TRACE_FUNCTION("cphs_delete()");

  if (!h || !((MfwHdr *) h)->data)
    return MfwResIllHnd;

  if (!mfwRemove((MfwHdr *) h))
    return MfwResIllHnd;

  mfwFree(((MfwHdr *) h)->data,sizeof(T_MFW_CPHS));
  mfwFree(h,sizeof(MfwHdr));

  return MfwResOk;
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CPHS	     |
| STATE   : code			ROUTINE : cphs_sign_exec     |
+--------------------------------------------------------------------+

  PURPOSE : dispatch CPHS management signal

*/

static int cphs_sign_exec (MfwHdr *cur_elem, MfwEvt event,
			   T_MFW_CPHS_PARA * para)
{
  TRACE_FUNCTION("cphs_sign_exec()");

  while (cur_elem)
  {
    if (cur_elem->type == MfwTypCphs)
    {
      T_MFW_CPHS *cphs_data;

      cphs_data = (T_MFW_CPHS *) cur_elem->data;
      if (cphs_data->emask & event)
      {
	cphs_data->event = event;
	TRACE_EVENT_P1("cphs_sign_exec():event=%d",event); 
	switch (event)
	{
	  case E_CPHS_IND:  
	    memcpy(&cphs_data->para.stat, para, sizeof(T_MFW_CPHS_STATUS));
	    break;
	  case E_CPHS_OP_NAME:	
	    memcpy(&cphs_data->para.oper_name, para, sizeof(T_MFW_CPHS_OP_NAME));
	    break;
	  case E_CPHS_GET_VC_NUM:  
	    memcpy(&cphs_data->para.vc_entry, para, sizeof(T_MFW_CPHS_VC_NUM));
	    break;
	  case E_CPHS_SET_VC_NUM:  
	    memcpy(&cphs_data->para.result, para, sizeof(UBYTE));
	    break;
	  case E_CPHS_GET_VC_STAT:  
	    memcpy(&cphs_data->para.vc_status, para, sizeof(T_MFW_CPHS_VC_STAT));
	    break;
	  case E_CPHS_SET_VC_STAT:  
	    memcpy(&cphs_data->para.result, para, sizeof(UBYTE));
	    break; 
	  case E_CPHS_GET_DV_STAT:  
	    memcpy(&cphs_data->para.dv_status, para, sizeof(T_MFW_CPHS_DV_STAT));
	    break;
	  case E_CPHS_SET_DV_STAT:  
	    memcpy(&cphs_data->para.result, para, sizeof(UBYTE));
	    break;
	  case E_CPHS_GET_ALS_STATUS:  
	    memcpy(&cphs_data->para.als_status, para, sizeof(T_MFW_CPHS_ALS_STATUS));
	    break;
	  case E_CPHS_SET_LINE:  
	    memcpy(&cphs_data->para.result, para, sizeof(UBYTE));
	    break;
	  case E_CPHS_GET_CSP:	
	    memcpy(&cphs_data->para.csp, para, sizeof(T_MFW_CPHS_CSP));
	    break;
	  case E_CPHS_GET_INFO_LIST:  
	    memcpy(&cphs_data->para.num_list, para, sizeof(T_MFW_CPHS_INFONUM_LIST));
	    break;
	  case E_CPHS_GET_INFO_NUM:  
	    memcpy(&cphs_data->para.info_entry, para, sizeof(T_MFW_CPHS_INFONUM_ENTRY));
	    break;
	 case E_CPHS_GET_SIM_FIELD://MC added for generic read function
	    memcpy(&cphs_data->para.read_sim_entry, para, sizeof(T_MFW_CPHS_READ_SIM_ENTRY));
	    break;
	case E_CPHS_SET_SIM_FIELD://MC added for generic read function
	    memcpy(&cphs_data->para.read_sim_entry, para, sizeof(T_MFW_CPHS_READ_SIM_ENTRY));
	    break;


	}
	if (cphs_data->handler)
	{
	  // store current mfw elem
	  current_mfw_elem = cur_elem;

	  if ((*(cphs_data->handler)) (cphs_data->event, (void *) &cphs_data->para))
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
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CPHS	     |
| STATE   : code			ROUTINE : cphs_signal	     |
+--------------------------------------------------------------------+

  PURPOSE : Send a signal

*/

static void cphs_signal (MfwEvt event, void *para)
{	
/*MC, SPR 1389, we have to enable the display whenever 
	we send an event up to the MMI*/

      UBYTE temp=0;  
#ifndef FF_POWER_MANAGEMENT
	temp = dspl_Enable(0);
#else
      /*OMAPS00098881 (removing power variant) a0393213(Prabakar)
          display is enabled only when lcd refresh is needed*/
	if(FFS_flashData.refresh == 1)
		temp = dspl_Enable(0);
#endif

  TRACE_FUNCTION("cphs_signal()");

  if (mfwSignallingMethod EQ 0)
  {
    if (mfwFocus)
      if (cphs_sign_exec(mfwFocus,event,para))
      { 
      	#ifndef FF_POWER_MANAGEMENT
		dspl_Enable(temp);/*MC, SPR 1389*/
	#else
         /*OMAPS00098881 (removing power variant) a0393213(Prabakar)
             display is enabled only when lcd refresh is needed*/
		if(FFS_flashData.refresh == 1)
			dspl_Enable(temp);/*MC, SPR 1389*/	
	#endif	
		return;
      }
    if (mfwRoot)
      cphs_sign_exec(mfwRoot,event,para);
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
      if (cphs_sign_exec (h, event, para))
      {	
	      	#ifndef FF_POWER_MANAGEMENT
			dspl_Enable(temp);/*MC, SPR 1389*/
		#else
	         /*OMAPS00098881 (removing power variant) a0393213(Prabakar)
	             display is enabled only when lcd refresh is needed*/
			if(FFS_flashData.refresh == 1)
				dspl_Enable(temp);/*MC, SPR 1389*/	
		#endif	
		return;
      }

      /*
       * All windows tried inclusive root
       */
      if (h == mfwRoot)
      {
	      	#ifndef FF_POWER_MANAGEMENT
			dspl_Enable(temp);/*MC, SPR 1389*/
		#else
	         /*OMAPS00098881 (removing power variant) a0393213(Prabakar)
	             display is enabled only when lcd refresh is needed*/
			if(FFS_flashData.refresh == 1)
				dspl_Enable(temp);/*MC, SPR 1389*/	
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
    cphs_sign_exec(mfwRoot,event,para);
  }
  // Power Management	
#ifndef FF_POWER_MANAGEMENT
  dspl_Enable(temp);/*MC, SPR 1389*/
#else
  /*OMAPS00098881 (removing power variant) a0393213(Prabakar)
   display is enabled only when lcd refresh is needed*/
  if(FFS_flashData.refresh == 1)
     dspl_Enable(temp);/*MC, SPR 1389*/	
#endif	
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE:  MFW_CPHS	     |
| STATE  : code 			ROUTINE: sendCPWIEvt	     |
+--------------------------------------------------------------------+


   PURPOSE :this function sends the E_CPHS_GET_VC_STAT event if there is any voice mail
   MMI doesn't read from the SIM, but will set the vc Status based on the input from ACI

*/
//18 May, 2006  DR: OMAPS00070657  x0pleela
#ifdef FF_CPHS
void sendCPWIEvt( UBYTE flag_set, USHORT        line )
{
  T_MFW_CPHS_VC_STAT vc_stat;
  TRACE_FUNCTION("sendCPWIEvt()");
//11/06/2006

  vc_stat.line1 = MFW_CFLAG_NotPresent;
  vc_stat.line2 = MFW_CFLAG_NotPresent;
  vc_stat.data = MFW_CFLAG_NotPresent;
  vc_stat.fax = MFW_CFLAG_NotPresent;

  switch( line )
  {
  	case CPHS_LINE1:
		if( flag_set )
		  vc_stat.line1 = MFW_CFLAG_SET;
		else
		  vc_stat.line1 = MFW_CFLAG_NOTSet;
	break;

	case CPHS_LINE2:
		if( flag_set )
		  vc_stat.line2 = MFW_CFLAG_SET;
		else
		  vc_stat.line2 = MFW_CFLAG_NOTSet;
	break;

	case CPHS_LINE_DATA:
		if( flag_set )
		  vc_stat.data = MFW_CFLAG_SET;
		else
		  vc_stat.data = MFW_CFLAG_NOTSet;
	break;

	case CPHS_LINE_FAX:
		if( flag_set )
		  vc_stat.fax = MFW_CFLAG_SET;
		else
		  vc_stat.fax = MFW_CFLAG_NOTSet;
	break;

	default:
		vc_stat.line1 = MFW_CFLAG_NotPresent;
		vc_stat.line2 = MFW_CFLAG_NotPresent;
		vc_stat.data = MFW_CFLAG_NotPresent;
		vc_stat.fax = MFW_CFLAG_NotPresent;
	break;
		
  }
  
  cphs_signal(E_CPHS_GET_VC_STAT,(void*)&vc_stat);
}
#endif

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE:  MFW_CPHS	     |
| STATE  : code 			ROUTINE: sendCFISEvt	     |
+--------------------------------------------------------------------+


   PURPOSE :this function is used to signal CFIS event
*/

#ifdef FF_CPHS_REL4
void sendCPHSSignal( UBYTE event, void* para)
{
	TRACE_FUNCTION("sendCPHSSignal()");
	cphs_signal(event,para);	
}
#endif
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CPHS	     |
| STATE   : code			ROUTINE : cphs_support_check |
+--------------------------------------------------------------------+

  PURPOSE : Check whether CPHS is supported.

*/

void cphs_support_check (void)
{
  TRACE_FUNCTION ("cphs_support_check()");

  /* Read CPHS information filed (6F16 - 3 bytes) from SIM card */
  if (!cphs_read_sim_dat(SIM_CPHS_CINF, NOT_PRESENT_8BIT, MFW_CPHS_INFO_SIZE))
  {
   /* Read failed */
   /*MC-CONQUEST5999->6007, 20/05/2002, set status to ERR rather than NotPresent*/
    cphsStatus = CPHS_ERR;
    TRACE_EVENT("CPHS, error reading support indication");
    cphs_signal(E_CPHS_IND, &cphsStatus);
  }    
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CPHS	     |
| STATE   : code			ROUTINE : cphs_config	     |
+--------------------------------------------------------------------+

  PURPOSE : Request the Configuration of CPHS fields

*/

T_MFW cphs_config (T_MFW_CPHS_INFO *cphs_info)
{
  TRACE_FUNCTION ("cphs_config()");

  if (cphs_info NEQ NULL    /*	AND
      cphsStatus EQ MFW_CPHS_OK */	    )
  {
    cphs_info->phase = cphsPhase;
    cphs_info->csp   = cphs_ssc(MFW_CPHS_CSP,	       cphsServTab);
    cphs_info->mbNum = cphs_ssc(MFW_CPHS_MB_NUMBER,    cphsServTab);

    if (cphsPhase EQ 1) /* phase 1 only */
      cphs_info->sst = cphs_ssc(MFW_CPHS_SST,	       cphsServTab);
    else		/* not support */
      cphs_info->sst = NO_ALLOCATED;

    if (cphsPhase EQ 2) /* phase 2 only */
    {
      cphs_info->opNS  = cphs_ssc(MFW_CHPS_NAME_SHORT,	 cphsServTab);
      cphs_info->iNum  = cphs_ssc(MFW_CHPS_INFO_NUMBERS, cphsServTab);
    }
    else		/* not support */
    {
      cphs_info->opNS  = NO_ALLOCATED;
      cphs_info->iNum  = NO_ALLOCATED;
    }
  }
  return cphsStatus;
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CPHS	     |
| STATE   : code			ROUTINE : cphs_operator_name |
+--------------------------------------------------------------------+

  PURPOSE : Request the network operator name

*/

T_MFW cphs_operator_name (void)
{
  TRACE_FUNCTION ("cphs_operator_name()");

  /* Check CPHS support status */
  if (cphsStatus EQ CPHS_NotPresent) 
    return cphsStatus;

  /* Read operator name sring */
  if (!cphs_read_sim_dat(SIM_CPHS_ONSTR, NOT_PRESENT_8BIT, MFW_CPHS_ONS_SIZE))
  {
    /* Read operator name sring failed */
    opName.longName.len = 0;

    /* Read operator name shortform */
    if ((cphsPhase NEQ 2)							   OR
	(cphs_ssc(MFW_CHPS_NAME_SHORT,	 cphsServTab) NEQ ALLOCATED_AND_ACTIVATED) OR
	!cphs_read_sim_dat(SIM_CPHS_ONSHF, NOT_PRESENT_8BIT, MFW_CPHS_ONSF_SIZE)		  )
    {
      /* Read operator name shortform failed. 
	 Send event with empty info to MMI   */
      opName.shortName.len = 0;
      cphs_signal(E_CPHS_OP_NAME, &opName);
    }
  }
  return cphsStatus;
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CPHS	     |
| STATE   : code			ROUTINE : cphs_get_mailbox   |
+--------------------------------------------------------------------+

  PURPOSE : Request the Mailbox Numbers

*/

void cphs_get_mailbox (void) 
{
  TRACE_FUNCTION ("cphs_get_mailbox()");

  mbNum.count = 0;

  /* Check CPHS support status. 
     When CPHS is not support, read mailbox numbers from EEPROM */
  if (cphsStatus EQ CPHS_NotPresent)
  {
    cphs_read_eeprom_mailbox();
    return;
  }

  /* Check CPHS service table. 
     When CPHS is not support, read mailbox numbers from EEPROM */
  if (cphs_ssc(MFW_CPHS_MB_NUMBER, cphsServTab) NEQ ALLOCATED_AND_ACTIVATED)
  {
    cphs_read_eeprom_mailbox();
    return;
  }
  /* Read mailbox numbers from SIM. 
     When this reading failed, read mailbox numbers from EEPROM */
  if (!cphs_read_sim_rcd(SIM_CPHS_MBXN, 1, 0)) /* read the first record */
    cphs_read_eeprom_mailbox();
}

static UBYTE cphs_write_sim_voicemail_rcd(void)
{		
	BOOL 	bSavePossible 	= FALSE;
	UBYTE 	result			= MFW_SIMOP_WRITE_OK;
	
	TRACE_FUNCTION("cphs_write_sim_voicemail_rcd");
	
	/* allocate mbnData (sizeof(dataLen)) */
	//	Mar 30, 2005	REF: CRR 29986	xpradipg
	//	the dynamic allocation is done here, if the write fails it is deallocated
	//	else if success it is deallocated in the callback
#ifdef FF_MMI_OPTIM
	mbnData = (UBYTE*)mfwAlloc(200);
	if(mbnData != NULL)
	{
#endif
		ubCphsMbExtRecNo = 0;
		sFreeRec = 0;
		ubCurrExtRcWrite = 0;
		ubFlushRecords = 0;

		bSavePossible = cphs_build_mbn_data(mbnData, vcEntry->datalen);

		/**************************************************************************************/
		/**************************************************************************************/
		/* Check if enough EXT1 Records are available for storing the current number; If not, show Write Error  */
		if ( bSavePossible == FALSE )
		{
			result = MFW_SIMOP_WRITE_ERR;
			
			TRACE_FUNCTION("cphs_write_sim_voicemail_rcd: notenuf EXT1recs");
			cphs_signal(E_CPHS_SET_VC_NUM, &result);
			
			//	Mar 30, 2005	REF: CRR 29986	xpradipg
			//	mbnData is deallocated, on failure of write operation
			#ifdef FF_MMI_OPTIM
				if(mbnData)
					mfwFree(mbnData,200);
			#endif

			return 0;
		}
		/**************************************************************************************/
		/**************************************************************************************/
			
		if (!cphs_write_sim_rcd(SIM_CPHS_MBXN, vcEntry->index, 
			mbnData, vcEntry->datalen)) 
		{
			cphs_write_eeprom_mailbox(vcEntry);
		}
#ifdef FF_MMI_OPTIM      
	}
#endif      

	return 1;
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CPHS	     |
| STATE   : code			ROUTINE : cphs_set_mailbox   |
+--------------------------------------------------------------------+

  PURPOSE : Save the Mailbox Numbers

*/

T_MFW cphs_set_mailbox (T_MFW_CPHS_ENTRY *entry) 
{

  TRACE_FUNCTION ("cphs_set_mailbox()");

  TRACE_EVENT_P1("no: %s", entry->number);

  if (entry NEQ NULL	    AND
      strlen((char *)entry->number) NEQ 0 )
/* PATCH JPS 16.01.01: must copy the data and not only point on it!! */
  {
    memcpy((char *)&MbnEntry, (char *)entry, sizeof(T_MFW_CPHS_ENTRY));
    //vcEntry	= entry;
    vcEntry = &MbnEntry; // VO ???

  }
  else
    return CPHS_ERR;

  if ((entry->index <= 0)  OR
      (entry->index > 4)      )
    return CPHS_ERR;

  /* Check CPHS support status. 
     When CPHS is not support, write mailbox numbers to EEPROM */
  if (cphsStatus EQ CPHS_NotPresent)
  {
    cphs_write_eeprom_mailbox(entry);
	//x0pleela 11 June, 2006  DR:OMAPS00079692
	 //changed from CPHS_OK to MFW_CPHS_OK
    return MFW_CPHS_OK;
  }

  /* Check CPHS service table. 
     When CPHS is not support, write mailbox numbers to EEPROM */
  if ( cphs_ssc(MFW_CPHS_MB_NUMBER, cphsServTab) NEQ ALLOCATED_AND_ACTIVATED)
  {
    cphs_write_eeprom_mailbox(entry);
		//x0pleela 11 June, 2006  DR:OMAPS00079692
	 //changed from CPHS_OK to MFW_CPHS_OK
    return MFW_CPHS_OK;
  }

  /* Read first record to determine the data length,
     When the reading failed, write mailbox numbers to EEPROM */
//  simStatus = MFW_SIMOP_WRITE_OK;
  if (!cphs_write_sim_voicemail_rcd()) 
  {
  	return CPHS_ERR;
  }

  //x0pleela 11 June, 2006  DR:OMAPS00079692
  //changed from CPHS_OK to MFW_CPHS_OK
  return MFW_CPHS_OK;
}

/*
+-------------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CPHS		  |
| STATE   : code			ROUTINE : cphs_get_mailbox_status |
+-------------------------------------------------------------------------+

  PURPOSE : get the voice message waiting flag

*/

T_MFW cphs_get_mailbox_status () 
{
  TRACE_FUNCTION ("cphs_get_mailbox_status()");


  /* Read voice message waiting flag. 
     When this reading failed, send event with "read error" parameter to MMI */
  if (!cphs_read_sim_dat(SIM_CPHS_VMW, NOT_PRESENT_8BIT, MFW_CPHS_MBS_SIZE)) 
  {
    mbStatus.result = MFW_SIMOP_READ_ERR;
    cphs_signal(E_CPHS_GET_VC_STAT, &mbStatus);
  }
  return cphsStatus;
}
/*
+-------------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CPHS		  |
| STATE   : code			ROUTINE : cphs_rel4_get_mailbox_status |
+-------------------------------------------------------------------------+

  PURPOSE : get the voice message waiting flag

*/
#ifdef FF_CPHS_REL4
void cphs_rel4_get_mailbox_status()
{
	TRACE_FUNCTION("cphs_rel4_get_mailbox_status");
	qAT_PercentMWIS(CMD_SRC_LCL,0xFF);/*mspId is set to FF to get MWIS for all profiles*/
}

T_MFW_CPHS_REL4_MWIS* get_mwis_stat(int profile)
{
	return &mwis_stat[profile -1];
}
#endif
/*
+-------------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CPHS		  |
| STATE   : code			ROUTINE : cphs_set_mailbox_status |
+-------------------------------------------------------------------------+

  PURPOSE : Save the voice message waiting flag

*/

T_MFW cphs_set_mailbox_status (T_MFW_CFLAG_STATUS line1, 
			       T_MFW_CFLAG_STATUS line2, 
			       T_MFW_CFLAG_STATUS fax, 
			       T_MFW_CFLAG_STATUS data) 
{
  UBYTE result;

  TRACE_FUNCTION ("cphs_set_mailbox_status()");

  /* Check CPHS support status. */
  if (cphsStatus EQ CPHS_NotPresent)
  {
    TRACE_EVENT("cphsStatus is CPHS_NotPresent !");
    return cphsStatus;
  }

  /* Write status in buffer */
  if (line1 EQ MFW_CFLAG_SET   OR
      line1 EQ MFW_CFLAG_NOTSet  )
  {		mbsData[0] &=0xF0;			//zero the lower nibble
	mbsData[0] |= line1&0x0F;	     /* low Nibble */
	}
  /*else
    mbsData[0] = 0x0F;*/

  if (line2 EQ MFW_CFLAG_SET   OR
      line2 EQ MFW_CFLAG_NOTSet  )
  {	mbsData[0]&=0x0F;	
		mbsData[0] |= (line2 << 4)&0xF0;;    /* high Nibble */	
  }

  if (fax EQ MFW_CFLAG_SET   OR
      fax EQ MFW_CFLAG_NOTSet  )
  {	mbsData[1] &= 0xF0;			//zero the low nibble
		mbsData[1] |= fax&0x0F; 	     /* low Nibble */
  }

  if (data EQ MFW_CFLAG_SET   OR
      data EQ MFW_CFLAG_NOTSet	)
  {	mbsData[1] &= 0x0F;			//zero the high nibble
		mbsData[1] |= (data << 4)&0xF0;     /* high Nibble */  
  }

  /* Read voice message waiting flag to determine the size */
  simStatus = MFW_SIMOP_WRITE_OK;
  if (!cphs_read_sim_dat(SIM_CPHS_VMW, NOT_PRESENT_8BIT, MFW_CPHS_MBS_SIZE)) 
  {
    result    = MFW_SIMOP_WRITE_ERR;
    cphs_signal(E_CPHS_SET_VC_STAT, &result);
  }
/* PATCH VO 22.01.01 end */
  return cphsStatus;
}

/*
+------------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CPHS		 |
| STATE   : code			ROUTINE : cphs_get_divert_status |
+------------------------------------------------------------------------+

  PURPOSE : Request the call forwarding flags

*/

T_MFW cphs_get_divert_status () 
{
  TRACE_FUNCTION ("cphs_get_divert_status()");


  /* Read call forwarding flags. 
     When this reading failed, send event with "read error" parameter to MMI */
     /*a0393213 cphs rel4 - when EF(CFIS) is present call sAT_PercentCFIS to get CFIS*/
	#ifdef FF_CPHS_REL4
	if(sim_serv_table_check((UBYTE)SRV_No_55) != ALLOCATED_AND_ACTIVATED )
		{
	#endif
	
		  if (!cphs_read_sim_dat(SIM_CPHS_CFF, NOT_PRESENT_8BIT, MFW_CPHS_CFF_SIZE)) 
		  {
		    dvStatus.result = MFW_SIMOP_READ_ERR;
		    cphs_signal(E_CPHS_GET_DV_STAT, &dvStatus);
		  }
	#ifdef FF_CPHS_REL4
		}
	else
		{
		UBYTE mspId=mfw_cphs_get_reg_profile();
		TRACE_EVENT_P1("msp id  %d", mspId);
		/*needs clarification regarding third and fourth parameters*/
		sAT_PercentCFIS(CMD_SRC_LCL,	CFIS_MOD_Read, mspId , mspId, 0, NULL, NULL, 0);
		}
	#endif
  return cphsStatus;
}

/*
+-------------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CPHS		  |
| STATE   : code			ROUTINE : cphs_set_divert_status  |
+-------------------------------------------------------------------------+

  PURPOSE : Save the call forwarding flag

*/

T_MFW cphs_set_divert_status  (T_MFW_CFLAG_STATUS line1, 
			       T_MFW_CFLAG_STATUS line2, 
			       T_MFW_CFLAG_STATUS fax, 
			       T_MFW_CFLAG_STATUS data) 
{
  UBYTE result;

  TRACE_EVENT_P4 ("cphs_set_divert_status() %d %d %d %d",line1,line2,fax,data);


  /* Write status in buffer */
  if (line1 EQ MFW_CFLAG_SET   OR
      line1 EQ MFW_CFLAG_NOTSet  )
	{	dvData[0]&= 0xF0;		//zero the lower nibble
	dvData[0] |= line1&0x0F;	    /* low Nibble */
	}

  if (line2 EQ MFW_CFLAG_SET   OR
      line2 EQ MFW_CFLAG_NOTSet  )
  {		dvData[0]&= 0x0F;
	dvData[0] |= (line2 << 4)&0xF0;    /* high Nibble */  
  }

  if (fax EQ MFW_CFLAG_SET   OR
      fax EQ MFW_CFLAG_NOTSet  )
 {	dvData[1]&= 0xF0;		//zero the lower nibble
    dvData[1] |= fax&0x0F;		/* low Nibble */
  }
    

  if (data EQ MFW_CFLAG_SET   OR
      data EQ MFW_CFLAG_NOTSet	)
  {	dvData[1] &=0x0F;
		dvData[1] |= (data << 4)&0xF0;	   /* high Nibble */  
  }

  /* Read call forwarding flag to determine the size */
  simStatus = MFW_SIMOP_WRITE_OK;
  if (!cphs_read_sim_dat(SIM_CPHS_CFF, NOT_PRESENT_8BIT, MFW_CPHS_CFF_SIZE)) 
  {
    TRACE_EVENT("cphs_set_divert_status:cphs_read_sim_dat failed"); 
    result    = MFW_SIMOP_WRITE_ERR;
    cphs_signal(E_CPHS_SET_DV_STAT, &result);
  }
  return cphsStatus;
}


/*
+-------------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CPHS		  |
| STATE   : code			ROUTINE : cphs_rel4_set_divert_status  |
+-------------------------------------------------------------------------+

  PURPOSE : Save the call forwarding flag into SIM by calling sAT_PercentCFIS
  PARAMETERS: 	voice 	- call forward status for voice
  				fax		- call forward status for fax
  				data		- call forward status for data

*/
#ifdef FF_CPHS_REL4
T_MFW cphs_rel4_set_divert_status(T_MFW_CFLAG_STATUS voice, T_MFW_CFLAG_STATUS fax, 
										T_MFW_CFLAG_STATUS data, char number[PHB_MAX_LEN])
{
	UBYTE mspId=mfw_cphs_get_reg_profile();
	UBYTE cfuStat=0x00;

	TRACE_FUNCTION("cphs_rel4_set_divert_status");
	TRACE_EVENT_P4("voice - %d, fax - %d,  data - %d,  number -  %s", voice, fax, data, number);
	TRACE_EVENT_P1("msp id  %d",mspId);

	cfuStat= cfuStat | (voice == MFW_CFLAG_SET) | ((fax==MFW_CFLAG_SET)<<1) | ((data==MFW_CFLAG_SET)<<2);
	
	/*check third, fourth, sixth, seventh and eight parameters, number convert to 10 digit, TOA - psaSAT_getTonNpi is a local fn in ACI*/
	sAT_PercentCFIS( CMD_SRC_LCL,
                                    CFIS_MOD_Write, 
                                    mspId,
                                    mspId,
                                    cfuStat,
                                    number, 
                                    NULL,
                                    0xFF);
	return cphsStatus;

	
}
#endif



/*
+---------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CPHS	      |
| STATE   : code			ROUTINE : cphs_get_als_info   |
+---------------------------------------------------------------------+

  PURPOSE : Request the Status of Alternate Line Service information
	    (first read selected ALS line, dann status of this line).
	    When the field does not exist in SIM, read it from EEPROM.

*/

void cphs_get_als_info (T_MFW_CPHS_ALS_STATUS *info) 
{
  TRACE_FUNCTION ("cphs_get_als_info()");

    /* read alternate line service from EEPROM */
    cphs_read_eeprom_als(info);

    cphs_signal(E_CPHS_GET_ALS_STATUS, &alsStatus);
}

/*
+---------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CPHS	      |
| STATE   : code			ROUTINE : cphs_select_line    |
+---------------------------------------------------------------------+

  PURPOSE : Select the current used line. When the field does not 
	    exist in SIM, read it from EEPROM.

*/

void cphs_select_line (T_MFW_LINE_INDEX line) 
{
  UBYTE 	result;
  T_ACI_ALS_MOD alsMode=ALS_MOD_NOTPRESENT;

  TRACE_EVENT_P1 ("cphs_select_line() %d",line);

  if (line NEQ MFW_SERV_LINE1 AND
      line NEQ MFW_SERV_LINE2	  )
  {
    result = MFW_SIMOP_WRITE_ERR;
    cphs_signal(E_CPHS_SET_LINE, &result);
    return;
  }

  alsData   = (UBYTE) line;


      /* write alternate line service in EEPROM */
      cphs_write_eeprom_als(&result); 

      if (line EQ MFW_SERV_LINE1)
	alsMode = ALS_MOD_SPEECH;
      if (line EQ MFW_SERV_LINE2)
	alsMode = ALS_MOD_AUX_SPEECH;
      sAT_PercentALS(CMD_SRC_LCL, alsMode); 

      cphs_signal(E_CPHS_SET_LINE, &result);
}

/*
+---------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CPHS	      |
| STATE   : code			ROUTINE : cphs_set_als_status |
+---------------------------------------------------------------------+

  PURPOSE : Change the lock status of the line. When the field does 
	    not exist in SIM, read it from EEPROM.

*/

T_MFW cphs_set_als_status (T_MFW_LINE_STATUS status) 
{
  UBYTE result;
  T_MFW_SIM_PIN_STATUS pinStatus;

  TRACE_FUNCTION ("cphs_set_als_status()");

  /* check PIN 2 requirement */
  pinStatus.type = MFW_SIM_PIN2;
  sim_pin_status(&pinStatus);
  if (pinStatus.stat NEQ MFW_SIM_NO_PIN)
    return MFW_SIM_PIN2_REQ;	 

  /* check given parameter */
  if (status NEQ MFW_LINE_LOCKED  AND
      status NEQ MFW_LINE_UNLOCKED     )
  {
    result = MFW_SIMOP_WRITE_ERR;
    cphs_signal(E_CPHS_SET_ALS_STATUS, &result);
    return CPHS_ERR;
  }

  alsData = (UBYTE) status;

    /* write lock status in EEPROM */
    cphs_write_eeprom_alss(&result);
    cphs_signal(E_CPHS_SET_LINE, &result);
		//x0pleela 11 June, 2006  DR:OMAPS00079692
	 //changed from CPHS_OK to MFW_CPHS_OK
  return MFW_CPHS_OK;
}

/*
+---------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CPHS	      |
| STATE   : code			ROUTINE : cphs_get_csp	      |
+---------------------------------------------------------------------+

  PURPOSE : Request the customer service profile

*/

T_MFW cphs_get_csp () 
{
  UBYTE res;

  TRACE_FUNCTION ("cphs_get_csp()");


  /* check CPHS service table */
  res = cphs_ssc(MFW_CPHS_CSP, cphsServTab);
  if ( res NEQ ALLOCATED_AND_ACTIVATED)
    return res;

  /* Read customer service profile. 
     When this reading failed, send event with empty parameter array to MMI */
  if (!cphs_read_sim_dat(SIM_CPHS_CSP, NOT_PRESENT_8BIT, MFW_CPHS_CSP_SIZE))
  {
    csProfile.result = MFW_SIMOP_READ_ERR;
    memset(csProfile.csp, 0, sizeof(csProfile.csp));
    cphs_signal(E_CPHS_GET_CSP, &csProfile);
  }

  return cphsStatus;
}

/*
+---------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CPHS	      |
| STATE   : code			ROUTINE : cphs_get_info_num   |
+---------------------------------------------------------------------+

  PURPOSE : Request information numbers directory

*/

T_MFW cphs_get_info_num (UBYTE level, UBYTE startIndex) 
{
//  UBYTE res;  // RAVI
  T_MFW_CPHS_INFO cf_info;

  TRACE_FUNCTION ("cphs_get_info_num()");

	
  if (level < 1       OR
      startIndex < 1	)
    return CPHS_ERR;
	cphs_config(&cf_info);
 

  /* Read customer service profile to check 
     whether information numbers are supported.       */
  numList.count = 0;
  idxLevel	= level;
  startIdx	= startIndex;
  simStatus = MFW_SIMOP_READ_OK;
  if (!cphs_read_sim_dat(SIM_CPHS_CSP, NOT_PRESENT_8BIT, MFW_CPHS_CSP_SIZE))
  {
    simStatus = MFW_SIMOP_UNKNOWN;
    TRACE_EVENT("Can't read CSP for info nums");
    return CPHS_NotPresent;
  }

  return cphsStatus;
}

/*
+----------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CPHS	       |
| STATE   : code			ROUTINE : cphs_select_info_num |
+----------------------------------------------------------------------+

  PURPOSE : read a information number entry

*/

T_MFW cphs_select_info_num (UBYTE index) 
{
  TRACE_FUNCTION ("cphs_select_info_num()");


  /* Read a information number entry.	    
     When this reading failed, send event with "read error" parameter to MMI */
  simStatus = MFW_SIMOP_READ_OK;
  startIdx  = index;

  // By now we know if 6F19 or EA01 are present for reading info. num entries from
  // So lets use this knowledge.

  if (cphsPrevRead == SIM_CPHS_INFN)
  {
    if (!cphs_read_sim_rcd(SIM_CPHS_INFN, 1, 0)) 
    {
      infoEntry.result = MFW_SIMOP_READ_ERR;
      cphs_signal(E_CPHS_GET_INFO_NUM, &infoEntry);
      TRACE_EVENT("Error reading single info num");
    }
    else
    {
      cphsPrevRead = SIM_CPHS_INFN;
    }
  }
  else
  {
    if (cphsPrevRead == SIM_CPHS_INFN2)
    {
      if (!cphs_read_sim_rcd(SIM_CPHS_INFN2, 1, 0)) 
      {TRACE_EVENT("Error reading single info num 2");
	infoEntry.result = MFW_SIMOP_READ_ERR;
	cphs_signal(E_CPHS_GET_INFO_NUM, &infoEntry);
      }
      else
      {
	cphsPrevRead = SIM_CPHS_INFN2;
      }
    }
  }
  return cphsStatus;
}

/*
+-----------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CPHS		|
| STATE   : code			ROUTINE : cphs_read_information |
+-----------------------------------------------------------------------+

  PURPOSE : Read CPHS information

*/
//x0018858 OMAPS00062632 The errcode type has been modified from SHORT to USHORT.
void cphs_read_information(USHORT errCode, UBYTE *data, UBYTE dataLen)
{
  UBYTE *ptr;
/* PATCH VO 22.01.01: copy CPHS service table according to 
   the actual length in the SIM card			    */
  UBYTE len; 
/* PATCH VO 22.01.01 END */

  TRACE_FUNCTION ("cphs_read_information()");

  ptr = data;
/* PATCH VO 22.01.01: copy CPHS service table according to 
   the actual length in the SIM card			    */
  len = dataLen; 
  memset(cphsServTab, 0, sizeof(cphsServTab));
/* PATCH VO 22.01.01 END */
#ifdef FF_2TO1_PS
  if ( errCode NEQ CAUSE_SIM_NO_ERROR    OR
#else
  if ( errCode NEQ SIM_NO_ERROR    OR
#endif          
       ptr EQ NULL		   OR
       dataLen <= 0) /* PATCH VO 22.01.01: not check a certain length */
  {
    cphs_signal(E_CPHS_IND, &cphsStatus);
  }
  else
  {
    cphsPhase = *ptr; 
    len--;
    if (len > 0)
    {
      ptr++;
      memcpy (cphsServTab, ptr, MINIMUM(len, CPHS_SERVICE_TABLE));
    }
/* PATCH VO 22.01.01 END */
	//x0pleela 11 June, 2006  DR:OMAPS00079692
	 //changed from CPHS_OK to MFW_CPHS_OK
    cphsStatus = MFW_CPHS_OK;
	TRACE_EVENT("CPHS supported");
    cphs_signal(E_CPHS_IND, &cphsStatus);
  }
}

/*
+-----------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CPHS		|
| STATE   : code			ROUTINE : cphs_read_ons 			  |
+-----------------------------------------------------------------------+

  PURPOSE : Read operator name short form

*/
void cphs_read_ons(USHORT errCode, UBYTE *data, UBYTE dataLen)
{
  UBYTE *ptr;

  TRACE_FUNCTION ("cphs_read_ons()");

  ptr = data;
#ifdef FF_2TO1_PS
  if ( errCode NEQ CAUSE_SIM_NO_ERROR    OR
#else
  if ( errCode NEQ SIM_NO_ERROR    OR
#endif          
       ptr EQ NULL		   OR
       // dataLen < MFW_CPHS_ONS_SIZE  )
       dataLen <= 0  ) /* PATCH VO 22.01.01: not check a certain length */
  {
    opName.longName.len = 0;
  }
  else
  {
    cmhPHB_getMfwTagNt(ptr, (UBYTE)MINIMUM(LONG_NAME, dataLen), 
		       opName.longName.data, &opName.longName.len);
    opName.longName.dcs = MFW_DCS_SIM;

    /* Read operator name shortform */
    if ((cphsPhase NEQ 2)							   OR
	(cphs_ssc(MFW_CHPS_NAME_SHORT,	 cphsServTab) NEQ ALLOCATED_AND_ACTIVATED) OR
	!cphs_read_sim_dat(SIM_CPHS_ONSHF, NOT_PRESENT_8BIT, MFW_CPHS_ONSF_SIZE)		  )
    {
      opName.shortName.len = 0;
      if (opName.longName.len != 0)
	TRACE_EVENT("Got a long name!!!");
      cphs_signal(E_CPHS_OP_NAME, &opName);
    }
  }
}

/*
+-----------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CPHS		|
| STATE   : code			ROUTINE : cphs_read_onsf			  |
+-----------------------------------------------------------------------+

  PURPOSE : Read operator name short form

*/
void cphs_read_onsf(USHORT errCode, UBYTE *data, UBYTE dataLen)
{
  UBYTE *ptr;

  TRACE_FUNCTION ("cphs_read_onsf()");

  ptr = data;
#ifdef FF_2TO1_PS
  if ( errCode NEQ CAUSE_SIM_NO_ERROR    OR
#else
  if ( errCode NEQ SIM_NO_ERROR    OR
#endif          
       ptr EQ NULL		   OR
       dataLen < MFW_CPHS_ONSF_SIZE )
  {
    opName.shortName.len = 0;
  }
  else
  {
    cmhPHB_getMfwTagNt(ptr, MFW_CPHS_ONSF_SIZE, 
		       opName.shortName.data, &opName.shortName.len);
    opName.shortName.dcs = MFW_DCS_SIM;
    TRACE_EVENT("Got a short Name");
  }

  cphs_signal(E_CPHS_OP_NAME, &opName);
}

/*
+-----------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CPHS		|
| STATE   : code			ROUTINE : cphs_read_mbn 			  |
+-----------------------------------------------------------------------+

  PURPOSE : Read mailbox numbers

*/

void cphs_read_mbn(SHORT table_id)
{
  UBYTE*	pData;	       /* points to data buffer    */
  UBYTE 	alphaLen;
  UBYTE 	dataLen;
  T_ACI_TOA	type;

  TRACE_FUNCTION ("cphs_read_mbn()");

  pData   = simShrdPrm.atb[table_id].exchData;
  dataLen = simShrdPrm.atb[table_id].dataLen;
  /*June 27,2007 OMAPS00134788 x0066814(Geetha)*/
  curr_recNr   = simShrdPrm.atb[table_id].recNr;

  TRACE_EVENT_P1("curr_recNr: %d", curr_recNr);

  TRACE_EVENT_P1("dataLen: %d", dataLen);
  
#ifdef FF_2TO1_PS
  if ( simShrdPrm.atb[table_id].errCode NEQ CAUSE_SIM_NO_ERROR  OR
#else
  if ( simShrdPrm.atb[table_id].errCode NEQ SIM_NO_ERROR  OR
#endif          
       pData EQ NULL					  OR
       dataLen	 <  MFW_CPHS_MIN_MBN_SIZE )
  {
	  /*June 27,2007 OMAPS00134788 x0066814(Geetha)*/
	  if (curr_recNr <= 1)
	  {
	  	cphs_read_eeprom_mailbox();
	    return;
	  }
	  else
	  {
	    cphs_signal(E_CPHS_GET_VC_NUM, &mbNum);
	  }
  }
  else /* no read error */
  {
	  /*June 27,2007 OMAPS00134788 x0066814(Geetha)*/
	  if (curr_recNr EQ 1)
	  {
	  	maxRec = simShrdPrm.atb[table_id].recMax;
	  }

	  if ( dataLen >= MFW_CPHS_MIN_MBN_SIZE )
	  {
	  	alphaLen = dataLen - MFW_CPHS_MIN_MBN_SIZE;
	  }
	  else
	  {
	  	alphaLen = 0;
	  }

	  /* Copy/encode data */
	  if (  ( *(pData + alphaLen) > 0 ) /* If Length of the contents is > 0 */ 
	  	    &&
	  	    ( *(pData + alphaLen) <= 11 )
	  	 )
	  {
	  	  /*June 27,2007 OMAPS00134788 x0066814(Geetha)*/
		  mbNum.entries[mbNum.count].index = curr_recNr; 

		  mbNum.entries[mbNum.count].datalen = dataLen;

		  TRACE_EVENT_P1("mbNum.count: %d", mbNum.count);

		  TRACE_EVENT_P1("mbNum.entries[mbNum.count].index: %d", mbNum.entries[mbNum.count].index);
		 
		  cmhPHB_getMfwTagNt(pData, alphaLen, 
		  mbNum.entries[mbNum.count].alpha.data, 
				 &mbNum.entries[mbNum.count].alpha.len);
		  pData += alphaLen;

		  if ( *(pData) > 1 )
		  {
			  cmhPHB_getAdrStr ( (CHAR *)mbNum.entries[mbNum.count].number,
					 PHB_MAX_LEN - 1,
					 pData + 2,
					 *pData );
		  }
		  else
		  {
		  	memset ( (void *)mbNum.entries[mbNum.count].number, 0, PHB_MAX_LEN );
		  }

	      cmhPHB_toaDmrg ( *( pData + 1 ), &type );
	      mbNum.entries[mbNum.count].ton = phb_cvtTon(type.ton);
	      mbNum.entries[mbNum.count].npi = phb_cvtNpi(type.npi);

		  mbNum.entries[mbNum.count].ext_rec_number = 0;

		  TRACE_EVENT_P1("dataLen: %d", dataLen);

		  TRACE_EVENT_P1("alphaLen: %d", alphaLen);

		  TRACE_EVENT_P2("cphs_read_mbn: idx, ext_rec_number: %d, %d", mbNum.count, mbNum.entries[mbNum.count].ext_rec_number);
		  
	      pData += 12;

	      if ( *pData NEQ 0xFF )
	      {
			/* read bearer capability EF */
			/* return; */
	      }

	 	/*June 27,2007 OMAPS00134788 x0066814(Geetha)*/
		 pData += 1;
	 
		 if ( *pData NEQ 0xFF )
	 	 {
		 	/*Read EXT1 EF */
			UBYTE rcd_no;
			rcd_no= *pData;
			mbNum.entries[mbNum.count].ext_rec_idx[mbNum.entries[mbNum.count].ext_rec_number] = rcd_no;

			if(!cphs_read_sim_rcd(SIM_EXT1,rcd_no,UCHAR_MAX))
			{
				/*Append the extension data to mailbox number*/
				TRACE_FUNCTION("EXTENSION RECORD PRESENT");		
			}
			else
			{
				/*EXT1 record is being read. Wait the corr callback.*/
				return;
			}
		 }
		
	  	 mbNum.count++;
		 
	}
	  
	curr_recNr++; 
    if (curr_recNr > maxRec) 
    {
    	TRACE_EVENT("got CPHS MAilbox nos");
        cphs_signal(E_CPHS_GET_VC_NUM, &mbNum);
    }
    else
    {
      /* Read the next entry */
      if (!cphs_read_sim_rcd(SIM_CPHS_MBXN, curr_recNr, UCHAR_MAX)) 
  	  {
		cphs_signal(E_CPHS_GET_VC_NUM, &mbNum);
      }
    }
  }
}

/*
+-----------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CPHS		|
| STATE   : code			ROUTINE : cphs_read_ext1 			  |
+-----------------------------------------------------------------------+

  PURPOSE : Read EXT1 record for mailbox numbers

*/
/*June 27,2007 OMAPS00134788 x0066814(Geetha)*/
void cphs_read_ext1(SHORT table_id)
{
	UBYTE*	pData;	       /* points to data buffer    */
	UBYTE 	alphaLen;
	UBYTE 	dataLen;
	UBYTE 	recNr;
	UBYTE ext_rec_no;
	T_ACI_TOA	type;
	UBYTE ext1num[PHB_MAX_LEN];

	TRACE_FUNCTION ("cphs_read_ext1()");

	pData   = simShrdPrm.atb[table_id].exchData;
	dataLen = simShrdPrm.atb[table_id].dataLen;
	recNr   = simShrdPrm.atb[table_id].recNr;

	if ( *pData EQ 0x02 ) //implies that the EF conatins additional data 
	{
		
		/*Extract ext data*/
		if(*(pData+1) NEQ 0xFF)
		{
			alphaLen=*(pData+1);
			cmhPHB_getAdrStr ( (CHAR *)ext1num,
				 				PHB_MAX_LEN - 1,
							 	pData + 2,
			 					*(pData+1) );

			strncat( (CHAR *)mbNum.entries[mbNum.count].number, (CHAR *)ext1num, 
				      strlen((CHAR *)ext1num) );

			mbNum.entries[mbNum.count].ext_rec_number++;

			TRACE_EVENT_P2("cphs_read_ext1: idx, ext_rec_number: %d, %d", mbNum.count, mbNum.entries[mbNum.count].ext_rec_number);
			
			if ( *(pData + 12) == 0xFF )
			{
				//return control to mbn
				curr_recNr++;

				mbNum.count++;
				
				if (curr_recNr > maxRec) 
		    	{	
    				TRACE_EVENT("got CPHS MAilbox nos-2");
      				cphs_signal(E_CPHS_GET_VC_NUM, &mbNum);
    			}
				else 
				{									
					if(!cphs_read_sim_rcd(SIM_CPHS_MBXN, curr_recNr, UCHAR_MAX)) 
					{
						TRACE_FUNCTION("Control Back to mbn-1");
						return;
					}
				}
			}
			else
			{
   				ext_rec_no = *(pData + 12);

				mbNum.entries[mbNum.count].ext_rec_idx[mbNum.entries[mbNum.count].ext_rec_number] = ext_rec_no;
				
   				if(!cphs_read_sim_rcd(SIM_EXT1,ext_rec_no,UCHAR_MAX))
				{
					/*Append the extension data to mailbox number*/
					TRACE_FUNCTION( " MORE EXTENSION RECORDS PRESENT -But read Error");
					
					curr_recNr++;

					mbNum.count++;
					
					if(!cphs_read_sim_rcd(SIM_CPHS_MBXN, curr_recNr, UCHAR_MAX)) 
					{
						TRACE_FUNCTION("Control Back to mbn-2");
						return;
					}
				}
			}
		}
	}
  	else 
	  ;
	
	return;
}

/*
+-----------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CPHS		|
| STATE   : code			ROUTINE : cphs_rel4_get_mailbox_id 			  |
+-----------------------------------------------------------------------+

  PURPOSE : Get mailbox identifiers for the registered profile

*/
#ifdef FF_CPHS_REL4
void cphs_rel4_get_mailbox_id()
{
	UBYTE mspId=mfw_cphs_get_reg_profile();
	TRACE_FUNCTION("cphs_rel4_get_mailbox_id");
	/*get Mailbox Identifiers for the registered profile*/
	qAT_PercentMBI(CMD_SRC_LCL,mspId);
}
/*
+-----------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CPHS		|
| STATE   : code			ROUTINE : cphs_rel4_get_mailbox_no 			  |
+-----------------------------------------------------------------------+

  PURPOSE : Get mailbox number from the given index

*/
void cphs_rel4_get_mailbox_no(UBYTE index)
{
   TRACE_FUNCTION("cphs_rel4_get_mailbox_no");
   /*get mailbox number for the supplied index - the call is asynchronous*/
   qAT_PercentMBDN(CMD_SRC_LCL,index);
}

/*******************************************************************************

 $Function:	is_message_waiting
 $Description:	returns true if any message (in any profile) is waiting

*******************************************************************************/

BOOL is_message_waiting(void)
{
	TRACE_FUNCTION("is_message_waiting");
	if(mwis_stat[0].mwisStat&0x0F || mwis_stat[1].mwisStat&0x0F ||
		     mwis_stat[2].mwisStat&0x0F || mwis_stat[3].mwisStat&0x0F)
		     return TRUE;
	else
		return FALSE;
}


#endif

/*
+-----------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CPHS		|
| STATE   : code			ROUTINE : cphs_read_mbs 			  |
+-----------------------------------------------------------------------+

  PURPOSE : Read voice message waiting flag

*/
void cphs_read_mbs(USHORT errCode, UBYTE *data, UBYTE dataLen)
{
  UBYTE *ptr;
  int	i; /* PATCH VO 22.01.01 */

  TRACE_FUNCTION ("cphs_read_mbs()");

  ptr = data;
#ifdef FF_2TO1_PS
  if ( errCode NEQ CAUSE_SIM_NO_ERROR    OR
#else
  if ( errCode NEQ SIM_NO_ERROR    OR
#endif          
       ptr EQ NULL		   OR
       dataLen <= 0  )	/* PATCH VO 22.01.01: not check a certain length */
  {
    mbStatus.result = MFW_SIMOP_READ_ERR;
  }
  else
  {
    memset(&mbStatus, MFW_CFLAG_NotPresent, sizeof(mbStatus));
	
    //x0pleela, 07 June, 2006  DVT: OMAPS00079692
    //Copy SIM data into mbsData to initialise mbsData with the SIM data instead of zeros
    memcpy( mbsData, ptr, dataLen );
	
    mbStatus.result = MFW_SIMOP_READ_OK;

    for (i=0; i<dataLen; i++)
    {
      switch (i)
      {
	case 0:
	  mbStatus.line1  = (T_MFW_CFLAG_STATUS)(ptr[i] & 0x0F); /*a0393213 compiler warnings removal - extra parenthesis added*/
	  mbStatus.line2  = (T_MFW_CFLAG_STATUS)(( ptr[i] >> 4 ) & 0x0F);
	  break;
	case 1:
	  mbStatus.fax	  = (T_MFW_CFLAG_STATUS)(ptr[i] & 0x0F);
	  mbStatus.data   = (T_MFW_CFLAG_STATUS)(( ptr[i] >> 4 ) & 0x0F);
	  break;
	default:
	  break;
      }
    }
/* PATCH VO 22.01.01 END */
  }
	TRACE_EVENT("Reading Mailboxes");
  cphs_signal(E_CPHS_GET_VC_STAT, &mbStatus);
}





/*
+-----------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CPHS		|
| STATE   : code			ROUTINE : cphs_read_cff 			  |
+-----------------------------------------------------------------------+

  PURPOSE : Read call forwarding flag

*/
void cphs_read_cff(USHORT errCode, UBYTE *data, UBYTE dataLen)
{
  UBYTE *ptr;
  int	i; /* PATCH VO 22.01.01 */

  TRACE_FUNCTION ("cphs_read_cff()");

  ptr = data;
#ifdef FF_2TO1_PS
  if ( errCode NEQ CAUSE_SIM_NO_ERROR    OR
#else
  if ( errCode NEQ SIM_NO_ERROR    OR
#endif          
       ptr EQ NULL		   OR
       dataLen <= 0  )	/* PATCH VO 22.01.01: not check a certain length */
  {
    dvStatus.result = MFW_SIMOP_READ_ERR;
  }
  else
  {
    memset(&dvStatus, MFW_CFLAG_NotPresent, sizeof(dvStatus));
/*       May 30, 2006 REF:OMAPS00079650  a0393213 (R.Prabakar)
       Description : CPHS Call Forwarding feature implementation
       Solution     : As part of this implementation, dvData is made to be consistent with the data
                        read from SIM*/
    memcpy(&dvData, ptr,dataLen); 
    dvStatus.result = MFW_SIMOP_READ_OK;
    for (i=0; i<dataLen; i++)
    {
      switch (i)
      {
	case 0:
	  dvStatus.line1  = (T_MFW_CFLAG_STATUS)(ptr[i] & 0x0F); /*a0393213 compiler warnings removal - extra parenthesis added*/
	  dvStatus.line2  = (T_MFW_CFLAG_STATUS)(( ptr[i] >> 4 ) & 0x0F);
	  break;
	case 1:
	  dvStatus.fax	  = (T_MFW_CFLAG_STATUS)(ptr[i] & 0x0F);
	  dvStatus.data   = (T_MFW_CFLAG_STATUS)(( ptr[i] >> 4 ) & 0x0F);
	  break;
	default:
	  break;
      }
    }
/* PATCH VO 22.01.01 END */
  }
TRACE_EVENT("CPHS valid call forwrading status");
  cphs_signal(E_CPHS_GET_DV_STAT, &dvStatus);
}

/*
+-----------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CPHS		|
| STATE   : code			ROUTINE : cphs_read_als 			  |
+-----------------------------------------------------------------------+

  PURPOSE : Read alternate line service

*/
void cphs_read_als(USHORT errCode, UBYTE *data, UBYTE dataLen)
{

  TRACE_FUNCTION ("cphs_read_als()");
}

/*
+-----------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CPHS		|
| STATE   : code			ROUTINE : cphs_read_alss			  |
+-----------------------------------------------------------------------+

  PURPOSE : Read alternate line service status

*/
void cphs_read_alss(USHORT errCode, UBYTE *data, UBYTE dataLen)
{

  TRACE_FUNCTION ("cphs_read_alss()");

}

/*
+-----------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CPHS		|
| STATE   : code			ROUTINE : cphs_read_csp 			  |
+-----------------------------------------------------------------------+

  PURPOSE : Read customer service profile

*/
void cphs_read_csp(USHORT errCode, UBYTE *data, UBYTE dataLen)
{
  UBYTE *ptr;
  UBYTE flag;
  int	i;

  TRACE_FUNCTION ("cphs_read_csp()");

  ptr = data;
#ifdef FF_2TO1_PS
  if ( errCode NEQ CAUSE_SIM_NO_ERROR    OR
#else
  if ( errCode NEQ SIM_NO_ERROR    OR
#endif          
       ptr EQ NULL		   OR
       dataLen <= 0	 )  /* PATCH VO 22.01.01: not check a certain length */
  {
    if (simStatus EQ MFW_SIMOP_READ_OK) /* Used for read information numbers */  
    {
      simStatus = MFW_SIMOP_UNKNOWN;
      cphs_signal(E_CPHS_GET_INFO_LIST, &numList);
    }
    else 
    {
      csProfile.result = MFW_SIMOP_READ_ERR;
      memset(csProfile.csp, 0, sizeof(csProfile.csp));
      cphs_signal(E_CPHS_GET_CSP, &csProfile);
    }
  }
  else
  {
    if (simStatus EQ MFW_SIMOP_READ_OK) /* Used for read information numbers */
    {
      simStatus = MFW_SIMOP_UNKNOWN;

      /* check the service group code */
      flag = 0;
      for (i = 0; i < dataLen; i += 2) /* PATCH VO 22.01.01: use the actual length in the SIM card */
      {
	if (ptr[i] EQ 0xD5 AND ptr[i+1] EQ 0xFF)
	  flag = 1;
      }
      if (!flag)
      {
	cphs_signal(E_CPHS_GET_INFO_LIST, &numList);
	return;
      }

      /* Read the first information numbers record */
      if (!cphs_read_sim_rcd(SIM_CPHS_INFN, 1, 0))  
	cphs_signal(E_CPHS_GET_INFO_LIST, &numList);
      else
	cphsPrevRead = SIM_CPHS_INFN;
    }
    else
    {
      csProfile.result = MFW_SIMOP_READ_OK;
      TRACE_EVENT("CPHS valis CSP");
      memcpy(csProfile.csp, ptr, sizeof(csProfile.csp));
      cphs_signal(E_CPHS_GET_CSP, &csProfile);
    }
  }
}

/*
+------------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CPHS		 |
| STATE   : code			ROUTINE : cphs_req_info_num_read |
+------------------------------------------------------------------------+

  PURPOSE : Read customer service profile

*/

void cphs_req_info_num_read(UBYTE rcd_num, UBYTE dataLen)
{
  TRACE_FUNCTION ("cphs_req_info_num_read()");

  if (rcd_num > maxRec)
    cphs_signal(E_CPHS_GET_INFO_LIST, &numList);
  else
  {
    if (cphsPrevRead == SIM_CPHS_INFN)
    {
      if (!cphs_read_sim_rcd(SIM_CPHS_INFN, rcd_num, dataLen)) 
      {
	cphs_signal(E_CPHS_GET_INFO_LIST, &numList);
      }
      else
      {
	cphsPrevRead = SIM_CPHS_INFN;
      }
    }
    else
    {
      if (cphsPrevRead == SIM_CPHS_INFN2)
      {
	if (!cphs_read_sim_rcd(SIM_CPHS_INFN2, rcd_num, dataLen)) 
	{
	  cphs_signal(E_CPHS_GET_INFO_LIST, &numList);
	}
	else
	{
	  cphsPrevRead = SIM_CPHS_INFN2;
	}
      }
    }
  }
}

/*
+--------------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CPHS		   |
| STATE   : code			ROUTINE : cphs_read_first_info_num |
+--------------------------------------------------------------------------+

  PURPOSE : Read first record of information numbers

*/

void cphs_read_first_info_num(SHORT table_id)
{
  UBYTE 	dataLen;

  TRACE_FUNCTION ("cphs_read_first_info_num()");

#ifdef FF_2TO1_PS
  if ( simShrdPrm.atb[table_id].errCode NEQ CAUSE_SIM_NO_ERROR ) /* VO patch 040501 - remove dataLen and pData check */
#else
  if ( simShrdPrm.atb[table_id].errCode NEQ SIM_NO_ERROR ) /* VO patch 040501 - remove dataLen and pData check */
#endif      
  {
    if (simStatus EQ MFW_SIMOP_READ_OK) /* Read entry of information numbers */
    { 
      simStatus = MFW_SIMOP_UNKNOWN;
      infoEntry.result = MFW_SIMOP_READ_ERR;
      cphs_signal(E_CPHS_GET_INFO_NUM, &infoEntry);
    }
    else
    {// if we are here then its possible the read of 7F20 6F19 failed.

     if (cphsPrevRead == SIM_CPHS_INFN)
     {
      if (!cphs_read_sim_rcd(SIM_CPHS_INFN2, 1, 0)) 
      {
	infoEntry.result = MFW_SIMOP_READ_ERR;
	cphs_signal(E_CPHS_GET_INFO_LIST, &infoEntry);
      }
      else
      {
	cphsPrevRead = SIM_CPHS_INFN2;
      }
     }
     else
     {
       cphs_signal(E_CPHS_GET_INFO_LIST, &numList); 
     }
    }
  }
  else
  {
    maxRec = simShrdPrm.atb[table_id].recMax;
    dataLen = simShrdPrm.atb[table_id].dataLen;

    if (simStatus EQ MFW_SIMOP_READ_OK) /* Read entry of information numbers */
    {
      /* VO patch 040501 - add dataLen check: if (patch) else ...  */
      if (dataLen < MFW_CPHS_MIN_INS_SIZE )
      {
	simStatus = MFW_SIMOP_UNKNOWN;
	infoEntry.result = MFW_SIMOP_READ_ERR;
	cphs_signal(E_CPHS_GET_INFO_NUM, &infoEntry);
      }
      else
	cphs_req_info_num_read(startIdx, dataLen);
    }
    else /* Read list of information numbers */
    {
      /* VO patch 040501 - add dataLen check: if (patch) else ...  */
      if (dataLen < MFW_CPHS_MIN_INS_SIZE ) 
	cphs_signal(E_CPHS_GET_INFO_LIST, &numList);
      else
      {
	if (idxLevel EQ 1) /* read this entry */
	  cphs_read_info_num(table_id);
	else	/* read the startIndex record */
	  cphs_req_info_num_read(startIdx, dataLen);
      }
    }
  }
}

/*
+---------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CPHS	      |
| STATE   : code			ROUTINE : cphs_read_info_num  |
+---------------------------------------------------------------------+

  PURPOSE : Read first record of information numbers

*/

void cphs_read_info_num(SHORT table_id)
{
  UBYTE*	pData;	       /* points to data buffer    */
  UBYTE 	dataLen;
  UBYTE 	alphaLen;
  UBYTE 	recNr;
  T_ACI_TOA	type;

  TRACE_FUNCTION ("cphs_read_info_num()");

  pData   = simShrdPrm.atb[table_id].exchData;
  dataLen = simShrdPrm.atb[table_id].dataLen;
  recNr   = simShrdPrm.atb[table_id].recNr;

#ifdef FF_2TO1_PS
  if ( simShrdPrm.atb[table_id].errCode NEQ CAUSE_SIM_NO_ERROR  OR
#else
  if ( simShrdPrm.atb[table_id].errCode NEQ SIM_NO_ERROR  OR
#endif          
       pData EQ NULL					  OR
       dataLen	 <  MFW_CPHS_MIN_INS_SIZE		     )
  {
    if (simStatus EQ MFW_SIMOP_READ_OK) /* Read entry of information numbers */
    {
      simStatus = MFW_SIMOP_UNKNOWN;
      infoEntry.result = MFW_SIMOP_READ_ERR;
      cphs_signal(E_CPHS_GET_INFO_NUM, &infoEntry);
    }
    else
    {// if we are here then its possible the read of 7F20 6F19 failed.
     // so we must try reading 7F10 EA01 instead.

     if (cphsPrevRead == SIM_CPHS_INFN2)
     {
      if (!cphs_read_sim_rcd(SIM_CPHS_INFN2, 1, 0)) 
      {
	infoEntry.result = MFW_SIMOP_READ_ERR;
	cphs_signal(E_CPHS_GET_INFO_LIST, &infoEntry);
      }
      else
      {
	cphsPrevRead = SIM_CPHS_INFN2;
      }
     }
     else
     {
       cphs_signal(E_CPHS_GET_INFO_LIST, &numList); 
     }
    }
    cphs_signal(E_CPHS_GET_INFO_LIST, &numList); // correct ??? VO
  }
  else
  { 
    if (simStatus EQ MFW_SIMOP_READ_OK) 
    {
      /* Read entry of information numbers */
      alphaLen = *pData;
      if (alphaLen)	 
      {
	/* copy/encode entry */
	infoEntry.index   = recNr;
	infoEntry.entryStat = *(pData + 1);

	cmhPHB_getMfwTagNt(pData + 2, alphaLen, 
			  infoEntry.alpha.data, &infoEntry.alpha.len);

	pData += alphaLen + 2;

	if (*pData NEQ 0xFF)
	{
	  cmhPHB_getAdrStr ( (CHAR *)infoEntry.number,
			     3, 	 /* length of number */
			     pData + 2,
			     *pData );
	  cmhPHB_toaDmrg ( *( pData + 1 ), &type );
	  infoEntry.ton = phb_cvtTon(type.ton);
	  infoEntry.npi = phb_cvtNpi(type.npi);
	  infoEntry.entryStat = infoEntry.entryStat | 0x80;
	}
	else
	{
	  infoEntry.number[0] = '\0';
	  infoEntry.ton       = MFW_TON_UNKNOWN;
	  infoEntry.npi       = MFW_NPI_UNKNOWN;
	}
      }
      infoEntry.result = MFW_SIMOP_READ_OK;
      cphs_signal(E_CPHS_GET_INFO_NUM, &infoEntry);
      return;
    }
    else if ((*(pData + 1) & 0x0F) < idxLevel) 
    {
      /* The index level is out the wished index level. */
      cphs_signal(E_CPHS_GET_INFO_LIST, &numList);
    }
    else if ((*(pData + 1) & 0x0F) > idxLevel) 
    {
      /* This is not wished index level. Read the next */
      recNr++;
      cphs_req_info_num_read(recNr, dataLen);
    }
    else
    {
      /* This is the wished index level. Read it. */
      alphaLen = *pData;
      if (alphaLen)	 /* check alpha length */
      {
	/* copy/encode this entry in list */
	numList.level = idxLevel;
	numList.entry[numList.count].index   = recNr;
	numList.entry[numList.count].entryStat = *(pData + 1) & 0x7F;	/* set bit 8 to 0 */

	cmhPHB_getMfwTagNt(pData + 2, alphaLen, 
			   numList.entry[numList.count].alpha.data, 
			   &numList.entry[numList.count].alpha.len);

	pData += alphaLen + 2;

	if (*pData NEQ 0xFF)
	{
	  numList.entry[numList.count].entryStat = numList.entry[numList.count].entryStat | 0x80;
	}
	numList.count++;
      }
      recNr++;
      /* Read the next record */
      cphs_req_info_num_read(recNr, dataLen);
    }
  }
}

/*
+--------------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CPHS		   |
| STATE   : code			ROUTINE : cphs_read_eeprom_mailbox |
+--------------------------------------------------------------------------+

  PURPOSE : Read mailbox number from EEPROM

*/

void cphs_read_eeprom_mailbox(void)
{
  U8		      version;
 #ifdef PCM_2_FFS
  T_PSPDF_MBN	      mbn;
 #else
  EF_MBN	      mbn;
 #endif
 
  T_ACI_TOA	      numTp;
  int		      i;
  USHORT	      max_rcd;

  for (i=0; i<MAX_CPHS_ENTRY; i++)
  {
  #ifdef PCM_2_FFS
	if (ffs_ReadRecord((UBYTE *)PSPDF_MBN_ID, (UBYTE *)&mbn, 
		sizeof( T_PSPDF_MBN ), (USHORT)(i+1), 1 ) ==  sizeof( T_PSPDF_MBN ) )

#else
    if (pcm_ReadRecord((UBYTE *)EF_MBN_ID,
	(USHORT)(i+1),
	SIZE_EF_MBN, 
	(UBYTE *)&mbn, 
	&version,
	&max_rcd) == PCM_OK)
#endif

    {
      if (mbn.len)
      {
	mbNum.entries[mbNum.count].index = i+1;
	cmhPHB_getAdrStr ( (char *)mbNum.entries[mbNum.count].number, 
			   PHB_MAX_LEN - 1, mbn.mbNum, mbn.len );
	cmhPHB_getMfwTagNt ( mbn.alphId, 10, 
			     mbNum.entries[mbNum.count].alpha.data, 
			     &mbNum.entries[mbNum.count].alpha.len );
	cmhPHB_toaDmrg ( mbn.numTp, &numTp );
	mbNum.entries[mbNum.count].ton = phb_cvtTon(numTp.ton);
	mbNum.entries[mbNum.count].npi = phb_cvtNpi(numTp.npi);
	mbNum.entries[mbNum.count].service = i;
	mbNum.count++;
      }
    }
  }
  cphs_signal(E_CPHS_GET_VC_NUM, &mbNum);
}

/*
+---------------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CPHS		    |
| STATE   : code			ROUTINE : cphs_write_eeprom_mailbox |
+---------------------------------------------------------------------------+

  PURPOSE : Write mailbox number in EEPROM

*/

void cphs_write_eeprom_mailbox(T_MFW_CPHS_ENTRY *entry)
{
  T_ACI_PB_TEXT text;
  UBYTE 	len;
  //EF_MBN	  mbn;
  T_ACI_TOA	type;
  UBYTE 	*pNumber;
  UBYTE 	result;
  UBYTE 	outLen;

  len = MINIMUM ( MAX_PCM_MAILBOX_LEN, entry->alpha.len);
  text.len = len;
  memcpy(text.data, entry->alpha.data, len); 
  text.cs = CS_Sim;
  cmhPHB_getMfwTagSim ( &text, FFS_flashData.mbn_AlphId, &outLen, MAX_PCM_MAILBOX_LEN );

  if ( entry->number[0] EQ '+')
  {
    type.ton = TON_International;
    pNumber = &entry->number[1];
  }
  else
  {
    type.ton = (T_ACI_TOA_TON)entry->ton;/*a0393213 warnings removal-explicit typecasting done*/
    pNumber = &entry->number[0];
  }
  type.npi = (T_ACI_TOA_NPI)entry->npi;/*a0393213 warnings removal-explicit typecasting done*/
  //MC- switching from PCM to FFS
  cmhPHB_toaMrg ( &type, /*&mbn.numTp*/&FFS_flashData.mbn_numTp );

  cmhPHB_getAdrBcd ((UBYTE*) FFS_flashData.mbn_Num/*mbn.mbNum*/, /*&mbn.len*/&FFS_flashData.mbn_len,
		     MAX_PCM_MAILBOX_LEN, (CHAR *)pNumber );
  FFS_flashData.mbn_len/*mbn.len*/++;

  if (flash_write() == EFFS_OK)

    result = MFW_SIMOP_WRITE_OK;
  else
    result = MFW_SIMOP_WRITE_ERR;
  cphs_signal(E_CPHS_SET_VC_NUM, &result);
}

/*
+-----------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CPHS		|
| STATE   : code			ROUTINE : cphs_read_eeprom_als	|
+-----------------------------------------------------------------------+

  PURPOSE : Read alternate line service information from EEPROM

*/

void cphs_read_eeprom_als(T_MFW_CPHS_ALS_STATUS *info)
{
#ifdef PCM_2_FFS
  T_PSPDF_ALS alsInfo;
#else
  EF_ALS alsInfo;
#endif

//  UBYTE  version;  // RAVI

  TRACE_FUNCTION ("cphs_read_eeprom_als()");
   alsInfo.selLine = FFS_flashData.als_selLine;
   alsInfo.statLine = FFS_flashData.als_statLine;
  {
    switch (alsInfo.selLine)
    {
      case MFW_SERV_LINE1:
	alsStatus.selectedLine = MFW_SERV_LINE1;
	break;
      case MFW_SERV_LINE2:
	alsStatus.selectedLine = MFW_SERV_LINE2;
	break;
      default:
	alsStatus.selectedLine = MFW_SERV_LINE2;
	break;
    }

    switch (alsInfo.statLine)
    {
      case MFW_LINE_LOCKED:
	alsStatus.status = MFW_LINE_LOCKED;
	break;
      case MFW_LINE_UNLOCKED:
	alsStatus.status = MFW_LINE_UNLOCKED;
	break;
      default:
	alsStatus.status = MFW_LINE_UNLOCKED;
	break;
    }
  }

   info->selectedLine = alsStatus.selectedLine;
   info->status = alsStatus.status;
   
}

/*
+-----------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CPHS		|
| STATE   : code			ROUTINE : cphs_write_eeprom_als |
+-----------------------------------------------------------------------+

  PURPOSE : Write selected alternate line in EEPROM

*/

void cphs_write_eeprom_als(UBYTE *res)
{




  /*a0393213 compiler warnings removal - variable alsinfo deleted*/


  TRACE_FUNCTION ("cphs_write_eeprom_als()");
  {
    if (alsStatus.status EQ MFW_LINE_LOCKED)
      *res = MFW_SIMOP_WRITE_ERR;
  
    alsStatus.selectedLine = (T_MFW_LINE_INDEX)alsData;

     FFS_flashData.als_selLine = alsData;
	if (flash_write() == EFFS_OK)
	  *res = MFW_SIMOP_WRITE_OK;
       else
         *res = MFW_SIMOP_WRITE_ERR;
    }
}

/*
+-----------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CPHS		|
| STATE   : code			ROUTINE : cphs_write_eeprom_alss|
+-----------------------------------------------------------------------+

  PURPOSE : Write line lock status in EEPROM

*/

void cphs_write_eeprom_alss(UBYTE *res)
{


  /*a0393213 compiler warnings removal - variable alsinfo removed*/


  TRACE_FUNCTION ("cphs_write_eeprom_als()");
  {
    alsStatus.status = (T_MFW_LINE_STATUS)alsData;
    FFS_flashData.als_statLine = alsData;

    if (flash_write() == EFFS_OK)
      *res = MFW_SIMOP_WRITE_OK;
    else
      *res = MFW_SIMOP_WRITE_ERR;
  }
}

/*
+----------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CPHS	       |
| STATE  : code 			ROUTINE: cphs_read_sim_dat     |
+----------------------------------------------------------------------+


   PURPOSE :   Request to read SIM card.

*/

BOOL cphs_read_sim_dat(USHORT data_id, UBYTE len, UBYTE max_length)
{
  T_ACI_RETURN res;

  TRACE_FUNCTION ("cphs_read_sim_dat()");

#ifdef FF_2TO1_PS
 res = cmhSIM_ReadTranspEF ( CMD_SRC_NONE,
               AT_CMD_NONE,
               FALSE,    /* SCT: dummy only (path info valid flag) */
               NULL,     /* SCT: dummy only (path info)*/
               data_id,
               0,
               max_length,
               NULL,
               cphs_read_sim_dat_cb);
#else
  res = cmhSIM_ReadTranspEF ( CMD_SRC_NONE,
               AT_CMD_NONE,
               FALSE,    /* SCT: dummy only (path info valid flag) */
               NULL,     /* SCT: dummy only (path info)*/
               data_id,
               0,
               max_length,
               NULL,
               cphs_read_sim_dat_cb);
#endif  
  if (res NEQ AT_EXCT)
    return FALSE;
  return TRUE;
}

/*
+----------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CPHS	       |
| STATE  : code 			ROUTINE: cphs_read_sim_dat_cb  |
+----------------------------------------------------------------------+


   PURPOSE :   Call back for SIM read.

*/

void cphs_read_sim_dat_cb(SHORT table_id)
{
  UBYTE        dataLen;
  UBYTE        result;
  TRACE_FUNCTION ("cphs_read_sim_dat_cb()");
/***************************Go-lite Optimization changes Start***********************/
//Aug 16, 2004    REF: CRR 24323   Deepa M.D
  TRACE_EVENT_P1("SIM Error code!!!%d",simShrdPrm.atb[table_id].errCode);
/***************************Go-lite Optimization changes end***********************/
  switch (simShrdPrm.atb[table_id].reqDataFld)
  {
    case SIM_CPHS_CINF: /* CPHS information */
      if (!cphsUpdate)
	cphs_read_information(simShrdPrm.atb[table_id].errCode,
			      simShrdPrm.atb[table_id].exchData,
			      simShrdPrm.atb[table_id].dataLen);
#ifdef SIM_TOOLKIT
      else
	cphs_update_info_cnf(simShrdPrm.atb[table_id].errCode,
			     simShrdPrm.atb[table_id].exchData,
			     simShrdPrm.atb[table_id].dataLen);
#endif
      break;
    
    case SIM_CPHS_ONSTR:  /* operator name string */
      cphs_read_ons(simShrdPrm.atb[table_id].errCode,
		    simShrdPrm.atb[table_id].exchData,
		    simShrdPrm.atb[table_id].dataLen);
      break;
    
    case SIM_CPHS_ONSHF:	/* operator name short form */
      cphs_read_onsf(simShrdPrm.atb[table_id].errCode,
		     simShrdPrm.atb[table_id].exchData,
		     simShrdPrm.atb[table_id].dataLen);
      break;

    case SIM_CPHS_VMW:	/* voice message waiting flag */
/* PATCH VO 22.01.01 */
      if (simStatus EQ MFW_SIMOP_WRITE_OK) 
      {
	/* Determine the size of this field, and write the data.
	   When the writing is not possible, write this voice  
	   message waiting flag in EEPROM.		       */
	simStatus = MFW_SIMOP_UNKNOWN;
#ifdef FF_2TO1_PS
	if ( simShrdPrm.atb[table_id].errCode EQ CAUSE_SIM_NO_ERROR )
#else
	if ( simShrdPrm.atb[table_id].errCode EQ SIM_NO_ERROR )
#endif        
	{
	  dataLen = simShrdPrm.atb[table_id].dataLen;
	  /* Write voice message waiting flag. 
	     When this writing failed, send event with "write error" parameter to MMI */
	  if (!cphs_write_sim_dat(SIM_CPHS_VMW, mbsData, dataLen)) 
	  {
	    result    = MFW_SIMOP_WRITE_ERR;
	    cphs_signal(E_CPHS_SET_VC_STAT, &result);
	  }
	}
	else
	{
	  result = MFW_SIMOP_WRITE_ERR;
	  cphs_signal(E_CPHS_SET_VC_STAT, &result);
	}
      }
      else
/* PATCH VO 22.01.01 end */
	cphs_read_mbs (simShrdPrm.atb[table_id].errCode,
		       simShrdPrm.atb[table_id].exchData,
		       simShrdPrm.atb[table_id].dataLen);
      break;

    case SIM_CPHS_CFF:	/* call forwarding flag */
/* PATCH VO 22.01.01 */
      if (simStatus EQ MFW_SIMOP_WRITE_OK) 
      {
	/* Determine the size of this field, and write the data.
	   When the writing is not possible, write this call forward flag in EEPROM*/ 
	simStatus = MFW_SIMOP_UNKNOWN;
#ifdef FF_2TO1_PS
	if ( simShrdPrm.atb[table_id].errCode EQ CAUSE_SIM_NO_ERROR )
#else
	if ( simShrdPrm.atb[table_id].errCode EQ SIM_NO_ERROR )
#endif        
	{
	  dataLen = simShrdPrm.atb[table_id].dataLen;
	  /* Write call forwarding flag. 
	     When this writing failed, send event with "write error" parameter to MMI */
	     TRACE_EVENT_P2("cphs_read_sim_dat_cb():%x %x",dvData[0],dvData[1]);
	  if (!cphs_write_sim_dat(SIM_CPHS_CFF, dvData, dataLen)) 
	  {
	    result    = MFW_SIMOP_WRITE_ERR;
	    cphs_signal(E_CPHS_SET_DV_STAT, &result);
	  }
	}
	else
	{
	  result = MFW_SIMOP_WRITE_ERR;
       /*May 30, 2006 REF:OMAPS00079650  a0393213 (R.Prabakar)
          Description : CPHS Call Forwarding feature implementation
          Solution     : As part of the implementation, a typographical error is corrected.
          E_CPHS_SET_VC_STAT was changed to E_CPHS_SET_DV_STAT*/
	  cphs_signal(E_CPHS_SET_DV_STAT, &result);
	}
      }
      else
/* PATCH VO 22.01.01 end */
      cphs_read_cff (simShrdPrm.atb[table_id].errCode,
		     simShrdPrm.atb[table_id].exchData,
		     simShrdPrm.atb[table_id].dataLen);
      break;


    case SIM_CPHS_CSP:	/* customer service profile */
      cphs_read_csp (simShrdPrm.atb[table_id].errCode,
		     simShrdPrm.atb[table_id].exchData,
		     simShrdPrm.atb[table_id].dataLen);
      break;

    default:
      break;
  }
  simShrdPrm.atb[table_id].ntryUsdFlg = FALSE;
}

/*
+----------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CPHS	       |
| STATE  : code 			ROUTINE: cphs_write_sim_dat    |
+----------------------------------------------------------------------+


   PURPOSE :   Request to write SIM card.

*/

BOOL cphs_write_sim_dat(USHORT data_id, UBYTE *data, UBYTE length)
{
  T_ACI_RETURN res;

  TRACE_FUNCTION ("cphs_write_sim_dat()");

#ifdef FF_2TO1_PS
  res = cmhSIM_WriteTranspEF (CMD_SRC_NONE,
            AT_CMD_NONE,
            FALSE,    /* SCT: dummy only (path info valid flag) */
            NULL,     /* SCT: dummy only (path info) */
            data_id,
            0,
            length,
            data,
            cphs_write_sim_dat_cb);
#else
  res = cmhSIM_WriteTranspEF (CMD_SRC_NONE,
			      AT_CMD_NONE,
                              FALSE,    /* SCT: dummy only (path info valid flag) */
                              NULL,     /* SCT: dummy only (path info) */
			      data_id,
			      0,
			      length,
			      data,
			      cphs_write_sim_dat_cb);
#endif  

  if (res NEQ AT_EXCT)
    return FALSE;

  return TRUE;
}

/*
+----------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CPHS	       |
| STATE  : code 			ROUTINE: cphs_write_sim_dat_cb |
+----------------------------------------------------------------------+


   PURPOSE :   callback of SIM card writing.

*/

void cphs_write_sim_dat_cb(SHORT table_id)
{
  UBYTE result;
  TRACE_FUNCTION ("sim_write_sim_dat_cb()");
  /***************************Go-lite Optimization changes Start***********************/
  //Aug 16, 2004    REF: CRR 24323   Deepa M.D
  TRACE_EVENT_P1("SIM Error code!!!%d", simShrdPrm.atb[table_id].errCode);
  /***************************Go-lite Optimization changes end***********************/
  switch (simShrdPrm.atb[table_id].reqDataFld)
  {
    case SIM_CPHS_VMW:	/* voice message waiting flag */
#ifdef FF_2TO1_PS
      if ( simShrdPrm.atb[table_id].errCode EQ CAUSE_SIM_NO_ERROR )
#else
      if ( simShrdPrm.atb[table_id].errCode EQ SIM_NO_ERROR )
#endif          
      {
	result = MFW_SIMOP_WRITE_OK;
	cphs_signal(E_CPHS_SET_VC_STAT, &result);
      }
      else
      {
	result = MFW_SIMOP_WRITE_ERR;
	cphs_signal(E_CPHS_SET_VC_STAT, &result);
      }
      break;

    case SIM_CPHS_CFF:	/* call forwarding flag */
#ifdef FF_2TO1_PS
      if ( simShrdPrm.atb[table_id].errCode EQ CAUSE_SIM_NO_ERROR )
#else
      if ( simShrdPrm.atb[table_id].errCode EQ SIM_NO_ERROR )
#endif          
      {
	result = MFW_SIMOP_WRITE_OK;
	cphs_signal(E_CPHS_SET_DV_STAT, &result);
      }
      else
      {
	result = MFW_SIMOP_WRITE_ERR;
	cphs_signal(E_CPHS_SET_DV_STAT, &result);
      }
      break;
   
    default:
      break;
  }
  simShrdPrm.atb[table_id].ntryUsdFlg = FALSE;
}

/*
+----------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CPHS	       |
| STATE  : code 			ROUTINE: cphs_read_sim_rcd     |
+----------------------------------------------------------------------+


   PURPOSE :   Request to read SIM card.

*/

BOOL cphs_read_sim_rcd(USHORT data_id, UBYTE rcd_num, UBYTE len)
{
  T_ACI_RETURN res;
  UBYTE        dataLen;

  TRACE_FUNCTION ("cphs_read_sim_rcd()");
  TRACE_EVENT_P1("rec num to be read: %d", rcd_num);

  if (rcd_num EQ 1)
      dataLen  = UCHAR_MAX;
    else
      dataLen  = len;

#ifdef FF_2TO1_PS
  res = cmhSIM_ReadRecordEF ( CMD_SRC_NONE,
			      AT_CMD_NONE,
            TRUE,     /* SCT: dummy only (path info valid flag) */
			      NULL,     /* SCT: dummy only (path info) */
			      data_id,
			      rcd_num,
			      dataLen,
			      NULL,
			      cphs_read_sim_rcd_cb);
#else
  res = cmhSIM_ReadRecordEF ( CMD_SRC_NONE,
			      AT_CMD_NONE,
            FALSE,   /* SCT: dummy only (path info valid flag) */
            NULL,     /* SCT: dummy only (path info) */
			      data_id,
			      rcd_num,
			      dataLen,
			      NULL,
			      cphs_read_sim_rcd_cb);
#endif  

  if (res NEQ AT_EXCT)
    return FALSE;

  return TRUE;
}

/*
+----------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CPHS	       |
| STATE  : code 			ROUTINE: cphs_read_sim_rcd_cb  |
+----------------------------------------------------------------------+


   PURPOSE :   Request to read SIM card.

*/

void cphs_read_sim_rcd_cb (SHORT table_id)
{
  UBYTE dataLen;
  TRACE_FUNCTION ("cphs_read_sim_rcd_cb()");
  /***************************Go-lite Optimization changes Start***********************/
  //Aug 16, 2004    REF: CRR 24323   Deepa M.D
  TRACE_EVENT_P1("SIM Error code!!!%d", simShrdPrm.atb[table_id].errCode);

  
  TRACE_EVENT_P1("tableid: %d", table_id);
  /***************************Go-lite Optimization changes end***********************/
  switch (simShrdPrm.atb[table_id].reqDataFld)
  {
		case SIM_CPHS_MBXN: /* mailbox numbers */
		if (simStatus EQ MFW_SIMOP_WRITE_OK) 
		{
			/* Determine the size of record, and write a new record.
			When the writing is not possible, write this mailbox 
			number in EEPROM.				     */
			simStatus = MFW_SIMOP_UNKNOWN;
#ifdef FF_2TO1_PS
			if ( simShrdPrm.atb[table_id].errCode EQ CAUSE_SIM_NO_ERROR )
#else
			if ( simShrdPrm.atb[table_id].errCode EQ SIM_NO_ERROR )
#endif        
			{
				dataLen = simShrdPrm.atb[table_id].dataLen;
				/* allocate mbnData (sizeof(dataLen)) */
				//	Mar 30, 2005	REF: CRR 29986	xpradipg
				//	the dynamic allocation is done here, if the write fails it is deallocated
				//	else if success it is deallocated in the callback
#ifdef FF_MMI_OPTIM
				mbnData = (UBYTE*)mfwAlloc(200);
				if(mbnData != NULL)
				{
#endif
					ubCphsMbExtRecNo = 0;

					cphs_build_mbn_data(mbnData, dataLen);
					
					if (!cphs_write_sim_rcd(SIM_CPHS_MBXN, vcEntry->index, 
						mbnData, dataLen)) 
					{
						cphs_write_eeprom_mailbox(vcEntry);
					}
#ifdef FF_MMI_OPTIM      
				}
#endif      
			}
			else
				cphs_write_eeprom_mailbox(vcEntry);
		}
		else
			cphs_read_mbn(table_id);
		break;

    case SIM_CPHS_INFN: /* information numbers */
      if (simShrdPrm.atb[table_id].recNr EQ 1)
      {
	/* VO patch 150501 */
	if ((startIdx EQ 1) AND (simStatus EQ MFW_SIMOP_READ_OK)) 
	  cphs_read_info_num(table_id);
	else
	/* VO patch 150501 end */
	  cphs_read_first_info_num(table_id);
      }
      else
	cphs_read_info_num(table_id);
      break;
  
    case SIM_CPHS_INFN2:	 /* information numbers 2 - 7F10 'EA01' selected if SIM_CPHS_INFN '6F19' isn't there*/
      if (simShrdPrm.atb[table_id].recNr EQ 1)
      {
	/* VO patch 150501 */ 
	if ((startIdx EQ 1) AND (simStatus EQ MFW_SIMOP_READ_OK)) 
	  cphs_read_info_num(table_id); 
	else 
	/* VO patch end */
	cphs_read_first_info_num(table_id);
      }
      else
	cphs_read_info_num(table_id);
      break;
/*June 27,2007 OMAPS00134788 x0066814(Geetha)*/
	  case SIM_EXT1:
	  	
	  		cphs_read_ext1(table_id);
		
		
		
	  	break;
    
    default:
      break;
  }
  simShrdPrm.atb[table_id].ntryUsdFlg = FALSE;
}


/*
+----------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CPHS	       |
| STATE  : code 			ROUTINE: cphs_write_sim_rcd    |
+----------------------------------------------------------------------+


   PURPOSE :   Request to write SIM card.

*/

BOOL cphs_write_sim_rcd(USHORT data_id, UBYTE rcd_num,
			UBYTE *data,	UBYTE dataLen)
{
  T_ACI_RETURN res;

  TRACE_FUNCTION ("cphs_read_sim_rcd()");

#ifdef FF_2TO1_PS
  res = cmhSIM_WriteRecordEF (CMD_SRC_NONE,
			      AT_CMD_NONE,
            TRUE,     /* SCT: dummy only (path info valid flag) */
			      NULL,     /* SCT: dummy only (path info) */
			      data_id,
			      rcd_num,
			      dataLen,
			      data,
			      cphs_write_sim_rcd_cb);
#else
  res = cmhSIM_WriteRecordEF (CMD_SRC_NONE,
			      AT_CMD_NONE,
                              FALSE, /* SCT: dummy only (path info valid flag) */
                              NULL,  /* SCT: dummy only (path info) */
			      data_id,
			      rcd_num,
			      dataLen,
			      data,
			      cphs_write_sim_rcd_cb);
#endif  

  if (res NEQ AT_EXCT)
    return FALSE;

  return TRUE;
}

/*
+----------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CPHS	       |
| STATE  : code 			ROUTINE: cphs_build_ext1   |
+----------------------------------------------------------------------+


   PURPOSE :   Frame the data as per the EXT1 record structure

*/
UBYTE     *temp_data = NULL;


UBYTE  cphs_build_ext1(UBYTE *pData)

{
	UBYTE 	count = 0;
	UBYTE ext1num[PHB_MAX_LEN];
	UBYTE   indx = 0;
	SHORT   rec_no_to_write = 0;
	UBYTE	  numLen;
	UBYTE	  *pNumber;
	UBYTE	  i, j;
	unsigned int length_no = 0;
	UBYTE	  byteno = 0;

	TRACE_FUNCTION ( "cphs_build_ext1()" );

	memset( pData, MFW_INVALID_SIM_DATA, 13); 

	
	*pData = 0x02; //implies that the EF contains additional data 
	temp_data = pData;
	
	/* number data and TON/NPI data */
	if ( vcEntry->number[0] EQ '+' )
	{
	  vcEntry->ton = TON_International;
	  pNumber  = &vcEntry->number[1];
	}
	else
	  pNumber  = &vcEntry->number[0];


	cmhPHB_getAdrBcd ( ext1num, &numLen,
			   PHB_MAX_LEN - 1, (CHAR *) (pNumber));

	for(i=0; i < strlen((CHAR *)pNumber); i++)
	{
		TRACE_EVENT_P2("pNumber[%d] : %c", i,pNumber[i]);  
	}
	
	
	length_no = numLen; //strlen( (CHAR *)pNumber );
	byteno = numLen; //( length_no / 2 ) + ( length_no % 2 );

	indx = ( (ubCurrExtRcWrite + 1) * 10 );

	TRACE_EVENT_P1("length_no: %d", length_no);
	TRACE_EVENT_P1("indx: %d", indx);
	
	if ( ( byteno - indx ) > 10 )
	{
		 count = 10;
	}
	else
	{
		 count = ( byteno - indx );
	}

	TRACE_EVENT_P1("count: %d", count);

	if ( count != 0 )
	{
		memcpy ( (void *)(pData + 2), (void *)&(ext1num[indx]), 
			       ( count ) );
	}

	for(i=0; i < count; i++)
	{
		TRACE_EVENT_P2("pData + 2 + %d : %x", i, *(pData + 2+i));  
	}

	
	*(pData+1)  = ( count );

	pData += 12;

	mbNum.entries[vcEntry->list_index].ext_rec_idx[ubCurrExtRcWrite] = sFreeRec;

	TRACE_EVENT_P1("ubCurrExtRcWrite: %d", ubCurrExtRcWrite);

	TRACE_EVENT_P1("mbNum...ext_rec_idx[ubCurrExtRcWrite]: %d", mbNum.entries[vcEntry->list_index].ext_rec_idx[ubCurrExtRcWrite]);

	ubCurrExtRcWrite++;
	
	if ( ubCphsMbExtRecNo <= ubCurrExtRcWrite )
	{
		if (  ( sFreeRec <= 0 ) || ( sFreeRec == 0xFF ) )
		{		
		  TRACE_FUNCTION ("cphs_build_ext1() : ubCphsMbExtRecNo <= ubCurrExtRcWrite; no free rec");
		   *(pData) = 0xFF;
		   return FALSE;
		}
	
  	   rec_no_to_write = sFreeRec;
	   
	  *(pData) = 0xFF;
	}
	else
	{
	  rec_no_to_write = sFreeRec;
	  	
	  sFreeRec = 0;

	  if ( mbNum.entries[vcEntry->list_index].ext_rec_number > ubCurrExtRcWrite )
	  {
	  	
	  	TRACE_FUNCTION("ext rec taken from within");
		sFreeRec = mbNum.entries[vcEntry->list_index].ext_rec_idx[ubCurrExtRcWrite];
	  }
	  else
	  {
	  
		  TRACE_FUNCTION("ext rec taken from cmh");
#ifdef TI_PS_FFS_PHB
		  sFreeRec = cmh_Query_free_ext_record();
#endif
	  }

	  TRACE_EVENT_P1("sFreeRec : %d", sFreeRec);
		
 	  if (  ( sFreeRec <= 0 ) || ( sFreeRec == 0xFF ) )
	  { 	  
		TRACE_FUNCTION ("cphs_build_ext1() : No free records available");
		 *(pData) = 0xFF;
	  }
	  else
	  {
  	    *(pData) = 0x00;
		*(pData) = sFreeRec;
		TRACE_FUNCTION("cmh_PHB_update_ext_record to be");
#ifdef TI_PS_FFS_PHB
		cmh_PHB_update_ext_record(sFreeRec,TRUE);		
#endif
	  }

 	  mbNum.entries[vcEntry->list_index].ext_rec_idx[ubCurrExtRcWrite] = *(pData);	  
	}

	TRACE_EVENT_P1("rec_no_to_write : %d", rec_no_to_write);
	cphs_write_sim_rcd(SIM_EXT1, rec_no_to_write, temp_data, 13);
	
	return TRUE;
}

static UBYTE mfw_cphs_vm_write_cb_ext(void)
{
	UBYTE ret_internal = 0;
	UBYTE result;

	TRACE_FUNCTION("mfw_cphs_vm_write_cb_ext");

	TRACE_EVENT_P1("ubCphsMbExtRecNo : %d", ubCphsMbExtRecNo);
	TRACE_EVENT_P1("ubCurrExtRcWrite : %d", ubCurrExtRcWrite);
	TRACE_EVENT_P2("idx, ext_rec_number : %d, %d", vcEntry->list_index, mbNum.entries[vcEntry->list_index].ext_rec_number);

	if (  ( ubCphsMbExtRecNo == 0 ) || ( ubCphsMbExtRecNo <= ubCurrExtRcWrite ) )
	{	

		TRACE_FUNCTION("inside if!");
		
		/* Flush any EXT Records, if any */
		if ( mbNum.entries[vcEntry->list_index].ext_rec_number > ubCphsMbExtRecNo )
		{

		
			TRACE_FUNCTION("flush needed!--");
			
			if ( ubFlushRecords == 0 )
			{
				ubFlushRecords = 1;
			}

			TRACE_EVENT_P1("ubFlushRecords : %d", ubFlushRecords);
			TRACE_EVENT_P1("i : %d", (ubCphsMbExtRecNo + ubFlushRecords - 1));			
			TRACE_EVENT_P1("ext_rec_idx[i] : %d", mbNum.entries[vcEntry->list_index].ext_rec_idx[ubCphsMbExtRecNo + ubFlushRecords - 1]);
			
			memset ( (void *)&flush_data[0], 0xFF, 13 );
#ifdef TI_PS_FFS_PHB			
			cmh_PHB_update_ext_record(mbNum.entries[vcEntry->list_index].ext_rec_idx[ubCphsMbExtRecNo + ubFlushRecords - 1],
				FALSE);
#endif			
			cphs_write_sim_rcd(SIM_EXT1, 
				 mbNum.entries[vcEntry->list_index].ext_rec_idx[ubCphsMbExtRecNo + ubFlushRecords - 1], 
				 flush_data, 13);	
			
			mbNum.entries[vcEntry->list_index].ext_rec_idx[ubCphsMbExtRecNo + ubFlushRecords - 1] = 0xFF;
			mbNum.entries[vcEntry->list_index].ext_rec_number--;
			ubFlushRecords++;
			return 1;
		}
	
		ubFlushRecords = 0;

		result = MFW_SIMOP_WRITE_OK;

		
		TRACE_FUNCTION("mfw_cphs_vm_write_cb_ext: write OK");
		cphs_signal(E_CPHS_SET_VC_NUM, &result);
	
		//	Mar 30, 2005	REF: CRR 29986	xpradipg
		//	mbnData is deallocated, on successfull write operation
	#ifdef FF_MMI_OPTIM
		  if(mbnData)
			 mfwFree(mbnData,200);
	#endif

		  return 0;
		
	}
	else
	{
		ret_internal = cphs_build_ext1(ubExtRecData);	
		if ( ret_internal == FALSE )
		{
			TRACE_FUNCTION("mfw_cphs_vm_write_cb_ext: cphs_build_ext1 returns FALSE");

			result = MFW_SIMOP_WRITE_OK;
			
			cphs_signal(E_CPHS_SET_VC_NUM, &result);
		
			//	Mar 30, 2005	REF: CRR 29986	xpradipg
			//	mbnData is deallocated, on successfull write operation
#ifdef FF_MMI_OPTIM
			  if(mbnData)
				 mfwFree(mbnData,200);
#endif
		
			  return 0;		
		}
	}

	return 1;
}

/*
+----------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CPHS	       |
| STATE  : code 			ROUTINE: cphs_write_sim_rcd_cb |
+----------------------------------------------------------------------+


   PURPOSE :   callback of SIM card writing.

*/

void cphs_write_sim_rcd_cb (SHORT table_id)
{
  UBYTE result;
  UBYTE *data_write;
  UBYTE dataLen = 0;
  UBYTE ret_internal = 0;
  
  TRACE_FUNCTION ("cphs_write_sim_rcd_cb()");
  /***************************Go-lite Optimization changes Start***********************/
  //Aug 16, 2004    REF: CRR 24323   Deepa M.D
  TRACE_EVENT_P1("SIM Error code!!!%d", simShrdPrm.atb[table_id].errCode);
  /***************************Go-lite Optimization changes end***********************/
  switch (simShrdPrm.atb[table_id].reqDataFld)
  {
    case SIM_CPHS_MBXN: /* mailbox numbers */
		
#ifdef FF_2TO1_PS
      if ( simShrdPrm.atb[table_id].errCode EQ CAUSE_SIM_NO_ERROR )
#else
      if ( simShrdPrm.atb[table_id].errCode EQ SIM_NO_ERROR )
#endif          
      {
	  	result = MFW_SIMOP_WRITE_OK;

	    ret_internal = mfw_cphs_vm_write_cb_ext();
		if ( ret_internal == 1 )
		{
			return;
		}
      }
      else
      {
	    cphs_write_eeprom_mailbox(vcEntry);
      }
	  
      break;

	case SIM_EXT1: /* mailbox ext numbers */
		
#ifdef FF_2TO1_PS
		  if ( simShrdPrm.atb[table_id].errCode EQ CAUSE_SIM_NO_ERROR )
#else
		  if ( simShrdPrm.atb[table_id].errCode EQ SIM_NO_ERROR )
#endif          
		  {
			result = MFW_SIMOP_WRITE_OK;
	
			ret_internal = mfw_cphs_vm_write_cb_ext();
			if ( ret_internal == 1 )
			{
				return;
			}
		  }
		  else
		  {
			cphs_write_eeprom_mailbox(vcEntry);
		  }
		  
		  break;

	
    default:
      break;
  }
  
  simShrdPrm.atb[table_id].ntryUsdFlg = FALSE;
}

/*
+----------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CPHS	       |
| STATE  : code 			ROUTINE: cphs_build_mbn_data   |
+----------------------------------------------------------------------+


   PURPOSE :   Build mailbox number data in SIM format.

*/

BOOL cphs_build_mbn_data(UBYTE *data, UBYTE len)
{
  UBYTE     alphaLen;
  UBYTE     numLen;
  UBYTE     *pNumber;
  UBYTE     i, j;
  unsigned int length_no = 0;
  UBYTE	    byteno = 0;

  TRACE_FUNCTION ("cphs_build_mbn_data()");

  memset(data, MFW_INVALID_SIM_DATA, len);

  /* alpha data */
  if (len <= MFW_CPHS_MIN_MBN_SIZE)
    alphaLen = 0;
  else
    alphaLen = len - MFW_CPHS_MIN_MBN_SIZE;
  i = MINIMUM (alphaLen, vcEntry->alpha.len);

  for ( j = 0; j < i; j++ )
    data[j] = vcEntry->alpha.data[j];
  
  data += alphaLen;

  /* number data and TON/NPI data */
  if ( vcEntry->number[0] EQ '+' )
  {
    vcEntry->ton = TON_International;
    pNumber  = &vcEntry->number[1];
  }
  else
    pNumber  = &vcEntry->number[0];

  for(i=0; i < strlen((CHAR *)pNumber); i++)
  {
	  TRACE_EVENT_P2("pNumber[%d] : %c", i,pNumber[i]);  
  }
  
  cmhPHB_getAdrBcd ( data + 2, &numLen,
		     PHB_MAX_LEN - 1, (CHAR *)pNumber );

  for(i=0; i < numLen; i++)
  {
	  TRACE_EVENT_P2("data + 2 + %d : %x", i, *(data + 2+i));  
  }

  if ( numLen < 11 )
  {
  	*data  = numLen + 1;
  }
  else
  {
  	*data = 11;
  }
  
  
  *(data + 1) = ((( vcEntry -> ton << 4 ) & 0xF0 ) + ( vcEntry -> npi & 0x0F )) | 0x80;

  data += 12;

  length_no = numLen;
  byteno = length_no; // ( length_no / 2 ) + ( length_no % 2 );

  if ( byteno > 10 )
  {
  	ubCphsMbExtRecNo = ( byteno / 10 ) - ( ( byteno % 10 ) == 0 ? 1 : 0 );
  }

  TRACE_EVENT_P1("length_no : %d", length_no);
  TRACE_EVENT_P1("byteno : %d", byteno);
  TRACE_EVENT_P1("ubCphsMbExtRecNo : %d", ubCphsMbExtRecNo);
  TRACE_EVENT_P1("vcEntry->index : %d", vcEntry->index);  
  TRACE_EVENT_P1("vcEntry->list_index : %d", vcEntry->list_index);
  TRACE_EVENT_P1("dataLen : %d", len);
  TRACE_EVENT_P1("ext_rec_number : %d", mbNum.entries[vcEntry->list_index].ext_rec_number);


  /**************************************************************************************/
  /**************************************************************************************/

  /*    Check if enough EXT1 Records are available for storing the current number; If not, return FALSE    */

  if (  ( ubCphsMbExtRecNo > 0 ) && 
  	    ( mbNum.entries[vcEntry->list_index].ext_rec_number < ubCphsMbExtRecNo ) )
  {
  	for ( i = mbNum.entries[vcEntry->list_index].ext_rec_number; i < ubCphsMbExtRecNo; i++ )
  	{
  		sFreeRec = 0;
  #ifdef TI_PS_FFS_PHB
  		sFreeRec = cmh_Query_free_ext_record();
  #endif
		if ( ( sFreeRec <= 0 ) || ( sFreeRec == 0xFF ) )
		{
			sFreeRec = 0;
			TRACE_FUNCTION("Not enuf ext1 recs available, hence show writeerror");
			return FALSE;
		}
  	}

	sFreeRec = 0;
  }

  /**************************************************************************************/
  /**************************************************************************************/

  
  /* capability/configuration identifier data and EXT identifier data */
  *data  = 0xFF;

  if ( ubCphsMbExtRecNo == 0 )
  {
  	*(data + 1) = 0xFF;
  }
  else
  {
  	sFreeRec = 0;
	ubCurrExtRcWrite = 0;

	if ( mbNum.entries[vcEntry->list_index].ext_rec_number > ubCurrExtRcWrite )
	{
		TRACE_FUNCTION("ext rec taken from within");
		sFreeRec = mbNum.entries[vcEntry->list_index].ext_rec_idx[ubCurrExtRcWrite];
	}
	else
	{
	 	TRACE_FUNCTION("ext rec taken from cmh");
#ifdef TI_PS_FFS_PHB
    	sFreeRec = cmh_Query_free_ext_record();
#endif
	}

	TRACE_EVENT_P1("sFreeRec: %d", sFreeRec);
	
	if ( ( sFreeRec <= 0 ) || ( sFreeRec == 0xFF ) )
	{		
	  TRACE_FUNCTION ("cphs_build_mbn_data() : No free records available");
	  *(data + 1) = 0xFF;
	  mbNum.entries[vcEntry->list_index].ext_rec_idx[ubCurrExtRcWrite] = 0xFF;
	}
	else
	{
  	  *(data + 1) = 0x00;
   	  *(data + 1) = sFreeRec;
	  TRACE_FUNCTION("cmh_PHB_update_ext_record to be");	  
	#ifdef TI_PS_FFS_PHB
	   cmh_PHB_update_ext_record(sFreeRec,TRUE);
	#endif   
	   TRACE_EVENT_P1(" *(data + 1) : %d",  *(data + 1) );
	}

	mbNum.entries[vcEntry->list_index].ext_rec_idx[ubCurrExtRcWrite] = *(data + 1);
  }

  return TRUE;
}

/*
+----------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CPHS	       |
| STATE  : code 			ROUTINE: cphs_ssc				       |
+----------------------------------------------------------------------+


   PURPOSE :   Check CPHS service status.

*/

UBYTE cphs_ssc (UBYTE nr, UBYTE * serv_table)
{
  UBYTE value;

  TRACE_FUNCTION ("cphs_ssc()");

  serv_table = serv_table + (nr-1)/4;
  value      = * serv_table;

  value = value >> (((nr-1) & 3) * 2);
  value = value & 3;
 
  return value;
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)	MODULE	: MFW_CPHS	     |
| STATE   : code			ROUTINE : cphsCommand	     |
+--------------------------------------------------------------------+

  PURPOSE : handle mfw windows command

*/

static int cphsCommand (U32 cmd, void *h)
{
  switch (cmd)
  {
    case MfwCmdDelete:		    /* delete me		*/
      if (!h)
	return 0;
      cphs_delete(h);
      return 1;
    default:
      break;
  }

  return 0;
}

#ifdef SIM_TOOLKIT
/*
+---------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)   MODULE  : MFW_CPHS		      |
| STATE   : code		   ROUTINE : cphs_sat_file_update     |
+---------------------------------------------------------------------+

  PURPOSE : SIM file change indication

*/

void cphs_sat_file_update(USHORT dataId)
{
  TRACE_FUNCTION("cphs_sat_file_update()");

  cphsUpdate = TRUE;
  switch (dataId)
  {
    case SIM_CPHS_CINF:
      if (!cphs_read_sim_dat(SIM_CPHS_CINF, NOT_PRESENT_8BIT, MFW_CPHS_INFO_SIZE))
      {
	satUpdateFiles(TRUE, SIM_CPHS_CINF);
      }
      break;
    default:
      break;
  }
}
#endif

#ifdef SIM_TOOLKIT
/*
+---------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)   MODULE  : MFW_CPHS		      |
| STATE   : code		   ROUTINE : cphs_update_info_cnf     |
+---------------------------------------------------------------------+

  PURPOSE : call back for SIM file read 

*/
void cphs_update_info_cnf (USHORT errCode, UBYTE *data, UBYTE dataLen)
{
  UBYTE *ptr;

  TRACE_FUNCTION ("cphs_update_info_cnf()");

  cphsUpdate = FALSE; /* reset flag */
  ptr = data;
#ifdef FF_2TO1_PS
  if ( errCode NEQ CAUSE_SIM_NO_ERROR    OR
#else
  if ( errCode NEQ SIM_NO_ERROR    OR
#endif          
       ptr EQ NULL		   OR
       dataLen < MFW_CPHS_INFO_SIZE)
  {
    satUpdateFiles(TRUE, SIM_CPHS_CINF);
  }
  else
  {
    cphsPhase = *ptr; 
    ptr++;
    memcpy (cphsServTab, ptr, CPHS_SERVICE_TABLE*sizeof(UBYTE));
		//x0pleela 11 June, 2006  DR:OMAPS00079692
	 //changed from CPHS_OK to MFW_CPHS_OK
    cphsStatus = MFW_CPHS_OK;
    satUpdateFiles ( TRUE, SIM_CPHS_CINF );
  }
}
#endif
/*
+----------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CPHS	       |
| STATE  : code 			ROUTINE: Read_Sim		|
+----------------------------------------------------------------------+


   PURPOSE :   Request to read SIM card.

*/

BOOL Read_Sim(USHORT dat_id, USHORT offset, UBYTE max_len, UBYTE *exDat)
{
  T_ACI_RETURN res;
 /* 
  * x0047685 Added path_info to support R99 SIM Interface changes.
  */
  T_path_info  tmp_path;

  TRACE_FUNCTION ("Read_Sim()");

  tmp_path.df_level1   = SIM_DF_VI;
  tmp_path.v_df_level2 = FALSE;

#ifdef FF_2TO1_PS
  res = cmhSIM_ReadTranspEF ( CMD_SRC_NONE,
            AT_CMD_NONE,
            FALSE,     /* SCT: dummy only (path info valid flag) */
            NULL,      /* SCT: dummy only (path info) */
            dat_id,
            offset,
            max_len,
            exDat,
            cphs_read_sim_default_cb);
#else
  res = cmhSIM_ReadTranspEF ( CMD_SRC_NONE,
            AT_CMD_NONE,
            TRUE,      /* SCT: dummy only (path info valid flag) */
            &tmp_path, /* SCT: dummy only (path info) */
            dat_id,
            offset,
            max_len,
            exDat,
            cphs_read_sim_default_cb);
#endif  
  
  if (res NEQ AT_EXCT)
    return FALSE;

  return TRUE;
}


/*
+----------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CPHS	       |
| STATE  : code 			ROUTINE: Read_Sim_Record	 |
+----------------------------------------------------------------------+


   PURPOSE :   Request to read record from SIM card.

*/

BOOL Read_Sim_Record(USHORT dat_id, USHORT record, UBYTE len, UBYTE *exDat)
{
  T_ACI_RETURN res;
  UBYTE        dataLen;
 /* 
  * x0047685 Added path_info to support R99 SIM Interface changes.
  */
  T_path_info  tmp_path;
  
    if (record EQ 1)
      dataLen  = UCHAR_MAX;
    else
      dataLen  = len;

  TRACE_FUNCTION ("Read_Sim_Record()");

  tmp_path.df_level1   = SIM_DF_VI;
  tmp_path.v_df_level2 = FALSE;
  
#ifdef FF_2TO1_PS
  res = cmhSIM_ReadRecordEF ( CMD_SRC_NONE,
            AT_CMD_NONE,
            FALSE,     /* SCT: dummy only (path info valid flag) */
            NULL,      /* SCT: dummy only (path info) */
            dat_id,
            record,
            dataLen,
            exDat,
            cphs_read_sim_default_cb);
#else
  res = cmhSIM_ReadRecordEF ( CMD_SRC_NONE,
            AT_CMD_NONE,
            TRUE,      /* SCT: dummy only (path info valid flag) */
            &tmp_path, /* SCT: dummy only (path info) */
            dat_id,
            record,
            dataLen,
            exDat,
            cphs_read_sim_default_cb);
#endif  
  if (res NEQ AT_EXCT)
    return FALSE;

  return TRUE;
}

/*
+----------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CPHS	       |
| STATE  : code 			ROUTINE: Write_Sim     |
+----------------------------------------------------------------------+
   PURPOSE :   Request to write SIM card.
*/

BOOL Write_Sim(USHORT dat_id, UBYTE len, UBYTE *exDat)
{
  T_ACI_RETURN res;

  TRACE_FUNCTION ("Write_Sim()");

#ifdef FF_2TO1_PS
  res = cmhSIM_WriteTranspEF (CMD_SRC_NONE,
            AT_CMD_NONE,
            FALSE,    /* SCT: dummy only (path info valid flag) */
            NULL,     /* SCT: dummy only (path info) */
            dat_id,
            0,
            len,
            exDat,
            cphs_write_sim_default_cb);
#else
  res = cmhSIM_WriteTranspEF (CMD_SRC_NONE,
            AT_CMD_NONE,
            FALSE,    /* SCT: dummy only (path info valid flag) */
            NULL,     /* SCT: dummy only (path info) */
            dat_id,
            0,
            len,
            exDat,
            cphs_write_sim_default_cb);
#endif  

  if (res NEQ AT_EXCT)
    return FALSE;

  return TRUE;
}


/*
+----------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CPHS	       |
| STATE  : code 			ROUTINE: Write_Sim_Record	 |
+----------------------------------------------------------------------+


   PURPOSE :   Request to write record to SIM card.

*/

BOOL Write_Sim_Record(USHORT dat_id, USHORT record, UBYTE len, UBYTE *exDat)
{
  T_ACI_RETURN res;
 /* 
  * x0047685 Added path_info to support R99 SIM Interface changes.
  */
  T_path_info  tmp_path;

  TRACE_FUNCTION ("Write_Sim()");

  tmp_path.df_level1   = SIM_DF_VI;
  tmp_path.v_df_level2 = FALSE;

#ifdef FF_2TO1_PS
  res = cmhSIM_WriteRecordEF (CMD_SRC_NONE,
            AT_CMD_NONE,
            FALSE,     /* SCT: dummy only (path info valid flag) */
            NULL,     /* SCT: dummy only (path info) */
            dat_id,
            record,
            len,
            exDat,
            cphs_write_sim_default_cb);
#else
  res = cmhSIM_WriteRecordEF (CMD_SRC_NONE,
            AT_CMD_NONE,
            TRUE,      /* SCT: dummy only (path info valid flag) */
            &tmp_path, /* SCT: dummy only (path info) */
            dat_id,
            record,
            len,
            exDat,
            cphs_write_sim_default_cb);
#endif  

  if (res NEQ AT_EXCT)
    return FALSE;

  return TRUE;
}

/*
+----------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CPHS	      |
| STATE  : code 			ROUTINE: cphs_read_default_cb	  |
+----------------------------------------------------------------------+


   PURPOSE :   Sends event to MMI 

*/
void cphs_read_sim_default_cb(SHORT table_id)
{
 
	read_sim_entry.requested_field = simShrdPrm.atb[table_id].reqDataFld;
	read_sim_entry.exchange_data = simShrdPrm.atb[table_id].exchData;
	read_sim_entry.sim_result = simShrdPrm.atb[table_id].errCode;
	read_sim_entry.data_len = simShrdPrm.atb[table_id].dataLen;
	read_sim_entry.record = simShrdPrm.atb[table_id].recNr;
#ifdef FF_2TO1_PS
	if (read_sim_entry.sim_result != CAUSE_SIM_NO_ERROR)
#else
	if (read_sim_entry.sim_result != SIM_NO_ERROR)
#endif        
	{	read_sim_entry.exchange_data = NULL;
		read_sim_entry.data_len = 0;
	}
		
	cphs_signal(E_CPHS_GET_SIM_FIELD, &read_sim_entry);
  simShrdPrm.atb[table_id].ntryUsdFlg = FALSE;
}


/*
+----------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CPHS	      |
| STATE  : code 			ROUTINE: cphs_read_default_cb	  |
+----------------------------------------------------------------------+


   PURPOSE :   Sends event to MMI 

*/
void cphs_write_sim_default_cb(SHORT table_id)
{
 
	read_sim_entry.requested_field = simShrdPrm.atb[table_id].reqDataFld;
	read_sim_entry.exchange_data = simShrdPrm.atb[table_id].exchData;
	read_sim_entry.sim_result = simShrdPrm.atb[table_id].errCode;
	read_sim_entry.data_len = simShrdPrm.atb[table_id].dataLen;
#ifdef FF_2TO1_PS
	if (read_sim_entry.sim_result != CAUSE_SIM_NO_ERROR)
#else
	if (read_sim_entry.sim_result != SIM_NO_ERROR)
#endif        
	{	read_sim_entry.exchange_data = NULL;
		read_sim_entry.data_len = 0;
	}
		
	cphs_signal(E_CPHS_SET_SIM_FIELD, &read_sim_entry);
  simShrdPrm.atb[table_id].ntryUsdFlg = FALSE;
}

#ifdef FF_CPHS_REL4

UBYTE mfw_cphs_get_reg_profile(void)
{
	TRACE_FUNCTION("mfw_cphs_get_reg_profile");
	TRACE_EVENT_P1(" Get registered profile  - %d", mspinfo.registered_profile);
	return mspinfo.registered_profile;
}

UBYTE mfw_cphs_get_default_profile(void)
{
	TRACE_FUNCTION("mfw_cphs_get_default_profile");
	TRACE_EVENT_P1(" Get default profile  - %d", mspinfo.default_profile);
	return mspinfo.default_profile;
}

void mfw_cphs_set_reg_profile(UBYTE profile_id)
{
	TRACE_FUNCTION("mfw_cphs_set_reg_profile");
	TRACE_EVENT_P1(" Set registered profile  - %d", profile_id);
	mspinfo.registered_profile = profile_id;
}

void mfw_cphs_set_default_profile(UBYTE profile_id)
{
	TRACE_FUNCTION("mfw_cphs_set_default_profile");
	TRACE_EVENT_P1(" Set default profile  - %d", profile_id);
	mspinfo.default_profile = profile_id;
}

T_MFW_CPHS_MSP_INFO* mfw_cphs_get_msp(void)
{
	TRACE_FUNCTION("mfw_cphs_get_msp");
	TRACE_EVENT_P3("msp count  - %d, def profile  - %d, reg profile  - %d ",mspinfo.count, mspinfo.default_profile, mspinfo.registered_profile);
	
	return &mspinfo;
}

UBYTE mfw_cphs_get_no_profile(void)
{
	TRACE_FUNCTION("mfw_cphs_get_no_profile");
	return mspinfo.count;
}


/*
+----------------------------------------------------------------------+
| PROJECT: MMI-Framework 	MODULE: MFW_CPHS	      |
| STATE  : code 			ROUTINE: mfw_cphs_get_msp_info	  |
+----------------------------------------------------------------------+


   PURPOSE :   query for MSP

*/
void mfw_cphs_get_msp_info(void)
{
	TRACE_FUNCTION("mfw_cphs_get_msp_info()");
	qAT_PlusCNUM(CMD_SRC_LCL, 0);

}

/*
+----------------------------------------------------------------------+
| PROJECT: MMI-Framework 	MODULE: MFW_CPHS	      |
| STATE  : code 			ROUTINE: check_msp_rec	  |
+----------------------------------------------------------------------+


   PURPOSE :   check for valid records in sim

*/
int check_msp_rec(T_ACI_CNUM_MSISDN *msisdn, int i)
{
	TRACE_FUNCTION("check_msp_rec()");
	if(msisdn->vldFlag == TRUE)
	{
		strcpy(mspinfo.profile[i].alpha, msisdn->alpha);
		strcpy(mspinfo.profile[i].number, msisdn->number);
		TRACE_EVENT_P1("profile alpha %s",mspinfo.profile[i].alpha);
		TRACE_EVENT_P1("profile number %s",mspinfo.profile[i].number);
		return TRUE;
	}
	return FALSE;
}

/*
+----------------------------------------------------------------------+
| PROJECT: MMI-Framework 	MODULE: MFW_CPHS	      |
| STATE  : code 			ROUTINE: send_msp_signal	  |
+----------------------------------------------------------------------+


   PURPOSE :   send event to MMI

*/
void send_msp_signal(UBYTE count)
{
	TRACE_EVENT_P1("msp profile count %d", count);
	mspinfo.count = count;
	cphs_signal(E_CPHS_MSP_IND, &mspinfo);
}

#endif
/*
+----------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CPHS	      |
| STATE  : code 			ROUTINE: mfw_cphs_get_als_info	  |
+----------------------------------------------------------------------+
   PURPOSE :   Returns the current active line
*/

//x0pleela 25 May, 2006  DR: OMAPS00070657
#ifdef FF_CPHS
T_MFW mfw_cphs_get_als_info(void)
{
	T_ACI_ALS_MOD ALSmode = ALS_MOD_NOTPRESENT; /*a0393213 compiler warnings removal - 0 changed to ALS_MOD_NOTPRESENT*/
	TRACE_FUNCTION("mfw_cphs_get_als_info");

	if( qAT_PercentALS( CMD_SRC_LCL, &ALSmode )!= AT_CMPL ) 
		return ALS_MOD_NOTPRESENT;
	else
		return ALSmode;		
}
#endif


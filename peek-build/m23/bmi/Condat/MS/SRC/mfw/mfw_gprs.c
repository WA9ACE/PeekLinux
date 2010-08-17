/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_gprs.c      $|
| $Author: KGT                          $Revision:: 1               $|
| CREATED: 13.02.2001                   $Modtime::  13.02.2001       |
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_GPRS

   PURPOSE : This modul contains functions needed for the GPRS 
             management in MFW.

$History: Mfw_gprs.c

	Feb 24, 2006 REF:DR OMAPS00068976 x0035544
	Description: GPRS attach icon - MMI follow up issue of OMAPS00051034
	solution: Added the function p_gprs_status() to querry the gprs status and to
	store the status info in the structure variable stat.
	
$End	
*/

/* include necessary header files ----------------------------------*/

#ifndef NEW_FRAME
  #error GPRS need NEW_FRAME
#endif  /* NEW_FRAME */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "typedefs.h"
#include "mfw_mfw.h"
#include "vsi.h"
#include "custom.h"
#include "gsm.h"

#include "message.h"
#include "prim.h"
#include "aci_cmh.h"  /* include types declared in ACI              */

/* ADDED - 30-11-2005 */
#if (defined(FF_2TO1_PS) || (BOARD == 61))
#include "aci_all.h"
#include "ati_cmd.h"
#include "aci_cmd.h"
#include "aci.h"
#include "dti_conn_mng.h"
#include "dti_cntrl_mng.h"
#include "gaci.h"
#endif
/* END  */
#include "aci_all.h"
#include "aci_cmh.h"  /* include types declared in ACI              */
#include "ati_cmd.h"
#include "aci_cmd.h"
#include "aci.h"
#include "dti_conn_mng.h"
#include "dti_cntrl_mng.h"
#include "gaci.h" /* include types declared in GACI             */
#include "gaci_cmh.h" /* include types declared in GACI             */
//#include "cus_aci.h"
#include "prim.h"
#ifndef PCM_2_FFS
#include "pcm.h"
#endif

#include "mfw_win.h"
#include "mfw_gprs.h"

#include "p_sim.h"

/* Definition for Edge Icon display */
#ifdef NEPTUNE_BOARD
#define TEMP_STRING_SIZE    50
#endif

/* import of external globals --------------------------------------*/
EXTERN MfwHdr *current_mfw_elem;

void gprs_set_location(U32 lac, U32 ci);

/* definition of globals -------------------------------------------*/

/* definition of constants -----------------------------------------*/

/* definition of internal functions --------------------------------*/

static BOOL gprs_sign_exec ( T_MFW_HDR *cur_elem, T_MFW_EVENT event, 
                             T_MFW_GPRS_PARA *para );

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE:  MFW_GPRS            |
| STATE  : code                         ROUTINE: gprs_init           |
+--------------------------------------------------------------------+


   PURPOSE : initialize for GPRS magangement

*/
void gprs_init ( void )
{
  TRACE_FUNCTION ("gprs_init()");

  /* 
   * set callback for MFW commands 
   */
  mfwCommand[MfwTypGprs] = (MfwCb) gprsCommand;
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE:  MFW_GPRS            |
| STATE  : code                         ROUTINE: gprs_exit           |
+--------------------------------------------------------------------+


   PURPOSE : finalize for GPRS magangement

*/
void gprs_exit ( void )
{
  TRACE_FUNCTION ("gprs_exit()");
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE:  MFW_GPRS            |
| STATE  : code                         ROUTINE: gprs_create         |
+--------------------------------------------------------------------+


   PURPOSE : create event for gprs management

   PARAMETERS :

   RETURNS :
*/
T_MFW_HND gprs_create ( T_MFW_HND   h,
                        T_MFW_EVENT event,
                        T_MFW_CB    cbfunc )
{
  T_MFW_HDR      *hdr;
  T_MFW_GPRS     *gprs_data;
 
/*a0393213 compiler warnings removal - variables state ,cls, regState, lac, ci removed*/
 
 
 
 

  MfwHdr *insert_status =0;

  TRACE_FUNCTION ("gprs_create()");

  hdr     = (T_MFW_HDR  *) mfwAlloc(sizeof (T_MFW_HDR));
  gprs_data = (T_MFW_GPRS *) mfwAlloc(sizeof (T_MFW_GPRS));

  if ( !hdr OR !gprs_data )
  	{
    	TRACE_ERROR("ERROR: gprs_create() Mem Alloc Failed.");
			
	   	if(hdr)
   			mfwFree((U8*)hdr,sizeof(MfwHdr));

   		if(gprs_data)
   			mfwFree((U8*)gprs_data,sizeof(T_MFW_GPRS));
   		
	   	return FALSE;
  	}

  /*
   * initialisation of the handler 
   */
  gprs_data->emask   = event;
  gprs_data->handler = cbfunc;
  
  hdr->data = gprs_data;      /* store gprs control block in node */
  hdr->type = MfwTypGprs;     /* store type of event handler      */


  /*
   * initialisation of gprs data 
   */
  gprs_data->data.numContexts = 0;

  gprs_data->data.tclass = (T_CGCLASS_CLASS)NULL;
  //x0035544 Feb 23, 2006 DR:OMAPS00068976
  gprs_data->data.Reg_state.regState = (T_CGREG_STAT)NULL;
  gprs_data->data.attached = (T_CGATT_STATE)NULL;

  gprs_data->data.contexts = 
    (T_MFW_GPRS_CONTEXT *) mfwAlloc(sizeof(T_MFW_GPRS_CONTEXT) * MAX_CID_PLUS_EINS);

  /* 
   * installation of the handler 
   */
  insert_status = mfwInsert((T_MFW_HDR *)h, hdr);
  
  if(!insert_status)
	{
  		TRACE_ERROR("ERROR: gprs_create() Failed to Install Handler. ");
   		mfwFree((U8*)hdr,sizeof(MfwHdr));
   		mfwFree((U8*)gprs_data,sizeof(T_MFW_GPRS));
		return 0;
  	}
    return insert_status;
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE:  MFW_GPRS            |
| STATE  : code                         ROUTINE: gprs_delete         |
+--------------------------------------------------------------------+


   PURPOSE : delete event for gprs management

   PARAMETERS :

   RETURNS :
             MFW_RES_OK,      on success
             MFW_RES_ILL_HND, on failure
*/
T_MFW_RES gprs_delete ( T_MFW_HND h )
{
  T_MFW_GPRS *gprs_data;

  TRACE_FUNCTION ("gprs_delete()");

  if ( !h OR !((T_MFW_HDR *)h)->data )
    return MFW_RES_ILL_HND;

  /* 
   * deinstallation of the handler
   */

  //if ( !mfwRemove((T_MFW_HDR *)h) )
  if ( !mfwRemove(h) )
    return MFW_RES_ILL_HND;

  gprs_data = ((T_MFW_HDR *) h)->data;

  while ( gprs_data->data.numContexts )
  {
    mfwFree((U8 *)&(gprs_data->data.contexts[gprs_data->data.numContexts]),
            sizeof(T_MFW_GPRS_CONTEXT));
    gprs_data->data.numContexts--;
  }
  mfwFree((U8 *)&(gprs_data->data), sizeof(T_MFW_GPRS_DATA));
  mfwFree((U8 *)(((T_MFW_HDR *) h)->data),sizeof(T_MFW_GPRS));
  mfwFree((U8 *)h,sizeof(T_MFW_HDR));

  return MFW_RES_OK;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_GPRS           |
| STATE   : code                        ROUTINE : gprsCommand        |
+--------------------------------------------------------------------+

  PURPOSE : handle mfw windows command

  PARAMETERS :

  Returns : 0, on failure
            1, on success
*/

int gprsCommand (U32 cmd, void *h)
{
  switch ( cmd )
  {
    case MfwCmdDelete:              /* delete me */
      if ( !h )
        return 0;
      gprs_delete(h);
      return 1;
    default:
      break;
  }

  return 0;
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE:  MFW_GPRS            |
| STATE  : code                         ROUTINE: gprs_signal         |
+--------------------------------------------------------------------+

   PURPOSE : send a event signal.

   PARAMETERS :
             event : current event
             *para : pointer to the event parameters

*/
void gprs_signal( T_MFW_EVENT event, 
                  void       *para)
{/*MC, SPR 1389, we have to enable the display whenever 
	we send an event up to the MMI*/
  UBYTE temp = dspl_Enable(0);
	
  TRACE_FUNCTION ("gprs_signal()");

  if ( mfwSignallingMethod EQ 0 )
  {
    /* 
     * focus is on a window
     */
    if ( mfwFocus )
    {
      /*
       * send event to sim management handler if available 
       */
      if (gprs_sign_exec (mfwFocus, event, para))
     { dspl_Enable(temp);/*MC, SPR 1389*/
	        return;
	 }
    }

    /* 
     * acutal focussed window is not available or has no 
     * network management registration handler, then search 
     * all nodes from the root.     
     */
    if ( mfwRoot )
      gprs_sign_exec (mfwRoot, event, para);
  }
  else
  {
    MfwHdr *h = 0;

    /* 
     * Focus set, then start here
     */
    if ( mfwFocus )
    h = mfwFocus;

    /* 
     * Focus not set, then start at root 
     */
    if ( !h )
    h = mfwRoot;

    /* 
     * while elements found
     */
    while ( h )
    {
      /*
       * Signal consumed, then return 
       */
      if ( gprs_sign_exec (h, event, para) )
      { dspl_Enable(temp);/*MC, SPR 1389*/
	        return;
	  }

      /* 
       * All windows tried inclusive root 
       */
      if ( h == mfwRoot )
   	  { dspl_Enable(temp);/*MC, SPR 1389*/
	        return;
	  }

      /* 
       * get parent window 
       */
      h = mfwParent(mfwParent(h));

      /* 
       * look for next element 
       */
      if ( h )
        h = ((MfwWin * )(h->data))->elems;
    }
    gprs_sign_exec (mfwRoot, event, para);
  }
 dspl_Enable(temp);/*MC, SPR 1389*/

}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE:  MFW_GPRS            |
| STATE  : code                         ROUTINE: gprs_sign_exec      |
+--------------------------------------------------------------------+


   PURPOSE : Handle a signal if GPRS management handler.

   PARAMETERS :

   RETURNS : 
             TRUE,  if signal is processed
             FALSE, if signal is not processed
*/
BOOL gprs_sign_exec ( T_MFW_HDR     *cur_elem, 
                      T_MFW_EVENT    event, 
                      T_MFW_GPRS_PARA *para )
{
  int i,j;
  T_CGATT_STATE state;
  #if(BOARD != 61)
  T_PDP_CONTEXT tst_ctrc[PDP_CONTEXT_CID_MAX];
  /*  T_GPRS_CONT_REC tst_ctrc[MAX_CID_PLUS_EINS]; */
  #endif
  SHORT tst_cid;
  USHORT      lac;
  USHORT      ci;

  TRACE_FUNCTION ("gprs_sign_exec()");

  /* 
   * while event handler is available 
   */
  while ( cur_elem )
  {
    /*
     * if handler is GPRS management handler
     */
    if ( cur_elem->type EQ MfwTypGprs )
    {
      T_MFW_GPRS *gprs_data;
      gprs_data = (T_MFW_GPRS *)cur_elem->data;

      /* 
       * if event is expected by the call back function
       */
      if ( gprs_data->emask & event )
      {
        gprs_data->event = event;

        switch ( event )
        {
          case E_MFW_GPRS_S_CNTXT:
            /*
             * Check if contexts is defined in ACI ( if too many context
             * are defined, the new one will not be defined
             */
#ifndef FF_2TO1_PS
            qAT_PlusCGDCONT(CMD_SRC_LCL, tst_ctrc, &tst_cid);
#endif            
  
            if ( tst_cid >= gprs_data->data.numContexts + 1)
            {

              /* 
               * insert the new context
               */ 
              memcpy (&(gprs_data->data.contexts[gprs_data->data.numContexts]), para, 
                sizeof (T_MFW_GPRS_CONTEXT));
              gprs_data->data.numContexts++;
            }
            break;

          case E_MFW_GPRS_S_ATT:
          case E_MFW_GPRS_R_EREP_ATT:
			/* SPR#1983 - SH -'attached' is now of correct type.
			 * Don't check if unchanged, as this rejects some cases
			 * where signal needs to be sent to BMI
			 */
			 
            gprs_data->data.attached = para->attached;
            
            /*
             * set all contexts to deactivated after detach
             */
            if ( gprs_data->data.attached == CGATT_STATE_DETACHED )
            {
              for ( i=0; i > gprs_data->data.numContexts; i++)
              {
                  gprs_data->data.contexts[i].activated = CGACT_STATE_DEACTIVATED;
              }
            }
            else
        	{
			  if (qAT_PlusCGCLASS(CMD_SRC_LCL, &gprs_data->data.tclass) == AT_FAIL )
			  {
			    TRACE_EVENT("MFW_GPRS: Error. qAT_PlusCGCLASS failed");
			  }
			//x0035544 Feb 23, 2006 DR:OMAPS00068976
			  if (qAT_PercentCGREG(CMD_SRC_LCL, &gprs_data->data.Reg_state.p_reg_State, &lac, &ci) == AT_FAIL )
			  {
			    TRACE_EVENT("MFW_GPRS: Error. qAT_PercentCGREG failed");
        	  }
        	}
            break;

          case E_MFW_GPRS_S_ACT:
            /*
             * search in the known context for the context ids given
             * in the parameter
             */ 
            for ( i=0; i > gprs_data->data.numContexts; i++)
            {
              j=0;

              do
              {
                /* 
                 * set the new activation state for the found context id
                 */
                if ( para->contextAct.ids[j] == gprs_data->data.contexts[i].id)
                {
                  gprs_data->data.contexts[i].activated = para->contextAct.state;
                }
              }
              while ( j < MAX_CID && 
                (para->contextAct.ids[j] != gprs_data->data.contexts[i].id) );
            }
            break;

          case E_MFW_GPRS_S_DATA:
            /* 
             * if attached mode was detached, set mode to attached
             */ 
              if ( gprs_data->data.attached == CGATT_STATE_DETACHED )
              {
                gprs_data->data.attached = CGATT_STATE_ATTACHED; 
              }
            
            /*
             * search in the known context for the context ids given
             * in the parameter
             */ 
             for ( i=0; i < gprs_data->data.numContexts; i++)
            {
              j=0;

              do
              {
                /* 
                 * if context id of parameter is known, set the activation state 
                 * to activated when it was detached and copy L2P value
                 */
                if ( para->contextL2P.ids[j] == gprs_data->data.contexts[i].id)
                {
                  if ( gprs_data->data.contexts[j].activated == CGACT_STATE_DEACTIVATED )
                  {
                    gprs_data->data.contexts[j].activated = CGACT_STATE_ACTIVATED;
                  }
          
                  if (!(gprs_data->data.contexts[i].L2P) )
                    gprs_data->data.contexts[i].L2P = 
                     (char *)  mfwAlloc(sizeof(char) * strlen(para->contextL2P.L2P));

                  memcpy (gprs_data->data.contexts[i].L2P, para->contextL2P.L2P,
                          strlen( para->contextL2P.L2P ) * sizeof(char));
                }
                j++;
              }
              while ( j < MAX_CID && 
                ( para->contextL2P.ids[j]!= gprs_data->data.contexts[i].id) );
            }
            break;

          case E_MFW_GPRS_R_ACT:
          case E_MFW_GPRS_R_DATA:
          case E_MFW_GPRS_R_ANS:
            gprs_data->data.link_id = para->link_id;
            break;

          case E_MFW_GPRS_S_QOS:
          case E_MFW_GPRS_R_QOS:
            for ( i=0; (i > gprs_data->data.numContexts) && 
                (gprs_data->data.contexts[i].id == para->context.id); i++);

            if (gprs_data->data.contexts[i].id == para->context.id) 
              memcpy (&(gprs_data->data.contexts[i].data.qos), 
#ifdef FF_2TO1_PS
                      &(para->context.data.qos), sizeof(T_PS_qos));
#else
                      &(para->context.data.qos), sizeof(T_PS_qos));
#endif            
            else
              TRACE_ERROR("context id not defined");
            break;

          case E_MFW_GPRS_S_QOS_MIN:
            for ( i=0; (i > gprs_data->data.numContexts) && 
                (gprs_data->data.contexts[i].id == para->context.id); i++);

            if (gprs_data->data.contexts[i].id == para->context.id) 
              memcpy (&(gprs_data->data.contexts[i].data.min_qos), 
#ifdef FF_2TO1_PS
                &(para->context.data.min_qos), sizeof(T_PS_qos));
#else
                &(para->context.data.min_qos), sizeof(T_PS_qos));
#endif            
            else
              TRACE_ERROR("context id not defined");
            break;

          case E_MFW_GPRS_S_CLASS:
          case E_MFW_GPRS_R_EREP_CLASS:
            memcpy (&(gprs_data->data.tclass), &(para->tclass), sizeof (T_CGCLASS_CLASS));
            break;

          case E_MFW_GPRS_R_EREP_ACT:
            /*
             * search for context
             */
            /* SPR#2243 - SH - Changed 'para->context' to 'para->erepAct' */
            for ( i=0; (i > gprs_data->data.numContexts) && 
                (gprs_data->data.contexts[i].id == para->erepAct.cid); i++);

            /*
             * if context found, copy PDP address data to MFW data
             */
            /* SPR#2243 - SH - Changed 'para->context' to 'para->erepAct' */
            if (gprs_data->data.contexts[i].id == para->erepAct.cid) 
            {
#ifdef FF_2TO1_PS
              memcpy (&(gprs_data->data.contexts[i].data.attributes.pdp_addr), 
                &para->erepAct.pdp_addr, sizeof(T_NAS_ip));
              memcpy (&(gprs_data->data.contexts[i].data.attributes.pdp_type), 
                &para->erepAct.pdp_type, sizeof(T_NAS_ip));
#else
              memcpy (&(gprs_data->data.contexts[i].data.attributes.pdp_addr), 
                &para->erepAct.pdp_addr, sizeof(T_NAS_ip));
              memcpy (&(gprs_data->data.contexts[i].data.attributes.pdp_type), 
                &para->erepAct.pdp_type, sizeof(T_PDP_TYPE));
#endif              
              gprs_data->data.contexts[i].activated = CGACT_STATE_ACTIVATED;
            }
            else
            {
              if (gprs_data->data.numContexts < MAX_CID )
              {
#ifdef FF_2TO1_PS
                memcpy (&(gprs_data->data.contexts[gprs_data->data.numContexts].data.attributes.pdp_addr),
                  &para->erepAct.pdp_addr, sizeof (T_NAS_ip));
                memcpy (&(gprs_data->data.contexts[gprs_data->data.numContexts].data.attributes.pdp_type),
                  para->erepAct.pdp_type, sizeof (T_PDP_TYPE));
                gprs_data->data.contexts[gprs_data->data.numContexts].id = (U8)para->erepAct.cid;
#else
                memcpy (&(gprs_data->data.contexts[gprs_data->data.numContexts].data.attributes.pdp_addr), &para->erepAct.pdp_addr, 
                  sizeof (T_NAS_ip));
                memcpy (&(gprs_data->data.contexts[gprs_data->data.numContexts].data.attributes.pdp_type),para->erepAct.pdp_type, 
                  sizeof (T_PDP_TYPE));
                gprs_data->data.contexts[gprs_data->data.numContexts].id = para->erepAct.cid;
#endif                
                gprs_data->data.contexts[gprs_data->data.numContexts].activated = CGACT_STATE_ACTIVATED;
                gprs_data->data.numContexts++;
              }
              else
                TRACE_ERROR("MFW_GPRS: ERROR: Too many contexts!");
            }
            break;

            /* SPR#2243 - SH - This is now a separate case from the above.
             * Uses para->context throughout. */
          case E_MFW_GPRS_S_PDPADDR:
            /*
             * search for context
             */
              for ( i=0; (i > gprs_data->data.numContexts) && 
                (gprs_data->data.contexts[i].id == para->context.id); i++);

            /*
             * if context found, copy PDP address data to MFW data
             */
              if (gprs_data->data.contexts[i].id == para->context.id) 
            {
#ifdef FF_2TO1_PS
              memcpy (&(gprs_data->data.contexts[i].data.attributes.pdp_addr), 
                &para->context.data.attributes.pdp_addr, sizeof(T_NAS_ip));
              memcpy (&(gprs_data->data.contexts[i].data.attributes.pdp_type), 
                &para->context.data.attributes.pdp_type, sizeof(T_PDP_TYPE));
#else
              memcpy (&(gprs_data->data.contexts[i].data.attributes.pdp_addr), 
                &para->context.data.attributes.pdp_addr, sizeof(T_NAS_ip));
              memcpy (&(gprs_data->data.contexts[i].data.attributes.pdp_type), 
                &para->context.data.attributes.pdp_type, sizeof(T_PDP_TYPE));
#endif              
              gprs_data->data.contexts[i].activated = para->context.activated;
            }
            else
            {
              if (gprs_data->data.numContexts < MAX_CID )
              {
#ifdef FF_2TO1_PS
                memcpy (&(gprs_data->data.contexts[gprs_data->data.numContexts].data.attributes.pdp_addr),
                  &para->context.data.attributes.pdp_addr, sizeof (T_NAS_ip));
                memcpy (&(gprs_data->data.contexts[gprs_data->data.numContexts].data.attributes.pdp_type),
                  &para->context.data.attributes.pdp_type, sizeof (T_PDP_TYPE));
                gprs_data->data.contexts[gprs_data->data.numContexts].id = para->context.data.cid;
#else
                memcpy (&(gprs_data->data.contexts[gprs_data->data.numContexts].data.attributes.pdp_addr),
                  &para->context.data.attributes.pdp_addr, sizeof (T_NAS_ip));
                memcpy (&(gprs_data->data.contexts[gprs_data->data.numContexts].data.attributes.pdp_type),
                  &para->context.data.attributes.pdp_type, sizeof (T_PDP_TYPE));
                gprs_data->data.contexts[gprs_data->data.numContexts].id = para->context.id;
#endif                
                gprs_data->data.contexts[gprs_data->data.numContexts].activated = para->context.activated;
                gprs_data->data.numContexts++;
              }
              else
                TRACE_ERROR("MFW_GPRS: ERROR: Too many contexts!");
            }
            break;

          case E_MFW_GPRS_R_EREP_DEACT:
            /*
             * search for context
             */
            for ( i=0; (i < gprs_data->data.numContexts) && 
                (gprs_data->data.contexts[i].id != para->erepAct.cid); i++);

            /*
             * if context found, set mode to deactivated
             */
            if (gprs_data->data.contexts[i].id == para->erepAct.cid) 
            {
              gprs_data->data.contexts[i].activated = CGACT_STATE_DEACTIVATED;
            }
            else
            {
              for ( i=0; (i < gprs_data->data.numContexts) && 
#ifdef FF_2TO1_PS
              ( strcmp(gprs_data->data.contexts[i].data.attributes.pdp_type, para->erepAct.pdp_type) ||
				    strcmp((const char *)&gprs_data->data.contexts[i].data.attributes.pdp_addr.ip_address.ipv4_addr.a4,
              (const char *)&para->erepAct.pdp_addr.ip_address.ipv4_addr.a4) ); i++);

#else
                  ( strcmp(gprs_data->data.contexts[i].data.attributes.pdp_type, para->erepAct.pdp_type) ||
				    strcmp((const char *)&gprs_data->data.contexts[i].data.attributes.pdp_addr.ip_address.ipv4_addr.a4
				    , (const char *)&para->erepAct.pdp_addr.ip_address.ipv4_addr.a4 )); i++);
#endif              

              /*
               * if context found, set mode to deactivated
               */
#ifdef FF_2TO1_PS
              if (!strcmp((const char *)&gprs_data->data.contexts[i].data.attributes.pdp_type, (const char *)&para->erepAct.pdp_type) &&
                  !strcmp((const char *)&gprs_data->data.contexts[i].data.attributes.pdp_addr.ip_address.ipv4_addr.a4, (const char *)&para->erepAct.pdp_addr.ip_address.ipv4_addr.a4) ) 
#else
   if (!strcmp((const char *)&gprs_data->data.contexts[i].data.attributes.pdp_type, (const char *)&para->erepAct.pdp_type) &&
                  !strcmp((const char *)&gprs_data->data.contexts[i].data.attributes.pdp_addr.ip_address.ipv4_addr.a4, (const char *)&para->erepAct.pdp_addr.ip_address.ipv4_addr.a4) ) 
#endif                  
              {
                gprs_data->data.contexts[i].activated = CGACT_STATE_DEACTIVATED;
			  }
            }
            
            break;

          case E_MFW_GPRS_S_AUTORESP:
            gprs_data->data.autoResponse = para->mode;
            break;

          case E_MFW_GPRS_S_ANS:
            break;

          case E_MFW_GPRS_S_EREP:
            /*
             * copy new event reporting settings to MFW data
             */
            gprs_data->data.eventRep.mode = para->eventRep.mode;
            gprs_data->data.eventRep.bfr  = para->eventRep.bfr;            
            break;

          case E_MFW_GPRS_R_EREP_RJ:
            break;

          case E_MFW_GPRS_R_REG:
            /*
             * copy new registration status to MFW data
             */
             /* SH - now with cell reselection data */
	     //x0035544 Feb 23, 2006 DR:OMAPS00068976
            gprs_data->data.Reg_state.regState = para->cell.stat;
            gprs_data->data.lac = para->cell.lac;
            gprs_data->data.ci = para->cell.ci;

            /*
       * check if attachement state has changed
       */
            if (qAT_PlusCGATT(CMD_SRC_LCL, &state) != AT_FAIL )
              gprs_data->data.attached = state;
            else
              TRACE_EVENT("MFW_GPRS: Error. qAT_PlusCGATT failed");
      break;

          case E_MFW_GPRS_S_SMSSERV:
            /*
             * copy new SMS service mode to MFW data
             */
            gprs_data->data.SMSService = para->service;
            break;

            case E_MFW_GPRS_COUNTER:
            	/* SPR#1574 - SH - Small change to ensure counter value copied correctly */
				memcpy(&gprs_data->data.counter, &para->counter, sizeof(T_MFW_GPRS_COUNTER));
            break;

          case E_MFW_GPRS_OK:
          case E_MFW_GPRS_ERROR:
          case E_MFW_GPRS_CONNECT:
          case E_MFW_GPRS_ERR_CONNECT:
            break;


          default:
            /*
             * an event has been used, which is not handled.
             * perhaps an error?
             */
            TRACE_EVENT("mfw_gprs.gprs_sign_exec: unused event");
            break;
        }
  
        /* 
         * if callback defined, call it 
         */
        if ( gprs_data->handler )
        {
          current_mfw_elem = cur_elem;

          if ( (*(gprs_data->handler))( gprs_data->event, 
                                      (void*)&gprs_data->data ) )
            return TRUE;
        }
      }
    }

    /* 
     * look for next element 
     */
    cur_elem = cur_elem->next;
  }
  return FALSE;
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE:  MFW_GPRS            |
| STATE  : code                         ROUTINE: gprs_ok             |
+--------------------------------------------------------------------+

   PURPOSE : success indication from ACI 
             (This function is called by rAT_OK)

   PARAMETERS : AT command ID
*/
void gprs_ok(T_ACI_AT_CMD cmdId)
{
  T_EMOBIIX_MESSAGE *attachMessage;

  TRACE_FUNCTION("mfw_gprs.gprs_ok");

  gprs_signal(E_MFW_GPRS_OK, &cmdId);

  attachMessage = P_ALLOC(EMOBIIX_MESSAGE);
  attachMessage->attach = 1;
  P_OPC(attachMessage) = EMOBIIX_SOCK_CREA;
  PSENDX(APP, attachMessage);

  return;
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE:  MFW_GPRS            |
| STATE  : code                         ROUTINE: gprs_error          |
+--------------------------------------------------------------------+

   PURPOSE : failure indication from ACI 
             (This function is called by rAT_PlusCME)

   PARAMETERS : AT command ID
*/
void gprs_error(T_ACI_AT_CMD cmdId, T_ACI_CME_ERR err)
{
  TRACE_FUNCTION("mfw_gprs.gprs_error");
 
  gprs_signal(E_MFW_GPRS_ERROR, &cmdId);
  
  return;
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE:  MFW_GPRS            |
| STATE  : code                         ROUTINE: gprs_connect        |
+--------------------------------------------------------------------+

   PURPOSE : success indication from ACI for connection
             (This function is called by rAT_CONNECT)
*/
void gprs_connect(SHORT cId)
{
  TRACE_FUNCTION("mfw_gprs.gprs_connect");

  gprs_signal(E_MFW_GPRS_CONNECT, &cId);
  return;
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE:  MFW_GPRS            |
| STATE  : code                         ROUTINE: gprs_err_connect    |
+--------------------------------------------------------------------+

   PURPOSE : error indication from ACI for connection
             (This function is called by rAT_BUSY, rAT_NO_CARRIER,
              rAT_NO_ANSWERE)

*/
void gprs_err_connect(SHORT cId)
{
  TRACE_FUNCTION("mfw_gprs.gprs_err_connect");

  gprs_signal(E_MFW_GPRS_ERR_CONNECT, &cId);

  return;
}



/*--------------------------------------------------------------------
  MMI-MFW Interface Functions
--------------------------------------------------------------------*/

/*
+---------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE:  MFW_GPRS             |
| STATE  : code                         ROUTINE: gprs_definePDPContext|
+---------------------------------------------------------------------+


   PURPOSE : Define PDP context. If the contexts for cid does not 
             exist in the contexts list, the new context will be 
             inserted in the contexts list.

   PARAMETERS :
             cid    : context ID
            *context: context data
*/
#ifdef FF_2TO1_PS
T_MFW_RES gprs_definePDPContext(SHORT cid, T_PDP_CONTEXT *inputCtxt)
{
  T_MFW_GPRS_CONTEXT *context; 

  TRACE_FUNCTION ("gprs_definePDPContext()");

  /*
   * ACI call
   */
  if (sAT_PlusCGDCONT(CMD_SRC_LCL, (U8)cid, inputCtxt) == AT_FAIL)
  {
  TRACE_EVENT("MFW_GPRS: Error. sAT_PlusCGDCONT failed");
    return MFW_RES_ERR;
  }

  /*
   * insert context into MFW GPRS data
   */
  context = (T_MFW_GPRS_CONTEXT *) mfwAlloc(sizeof(T_MFW_GPRS_CONTEXT));

  context->id       = (U8)cid;
  memcpy (&(context->data.attributes), inputCtxt, sizeof (T_GPRS_CONT_REC));
  context->activated = CGACT_STATE_DEACTIVATED; /* not yet activated */

  gprs_signal(E_MFW_GPRS_S_CNTXT, context);

  mfwFree((void *) context, sizeof(T_MFW_GPRS_CONTEXT));

  return MFW_RES_OK;
}
#else
T_MFW_RES gprs_definePDPContext(SHORT cid, T_PDP_CONTEXT *inputCtxt)
{
  T_MFW_GPRS_CONTEXT *context; 

  TRACE_FUNCTION ("gprs_definePDPContext()");

  /*
   * ACI call
   */
  if (sAT_PlusCGDCONT(CMD_SRC_LCL, cid, inputCtxt) == AT_FAIL)
  {
  TRACE_EVENT("MFW_GPRS: Error. sAT_PlusCGDCONT failed");
    return MFW_RES_ERR;
  }

  /*
   * insert context into MFW GPRS data
   */
  context = (T_MFW_GPRS_CONTEXT *) mfwAlloc(sizeof(T_MFW_GPRS_CONTEXT));

  context->id       = cid;
  memcpy (&(context->data), inputCtxt, sizeof (T_GPRS_CONT_REC));
  context->activated = CGACT_STATE_DEACTIVATED; /* not yet activated */

  gprs_signal(E_MFW_GPRS_S_CNTXT, context);

  mfwFree((void *) context, sizeof(T_MFW_GPRS_CONTEXT));

  return MFW_RES_OK;
}
#endif



/*
+---------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE:  MFW_GPRS             |
| STATE  : code                         ROUTINE: gprs_setQOS          |
+---------------------------------------------------------------------+


   PURPOSE : Set the Quality of Service Profile for the context 
             specified by cid. The Quality of Service Profile is given 
             in the pointer parameter qos.

   PARAMETERS :
             cid      : context ID
            *inputQos : quality of service data
*/
#ifdef FF_2TO1_PS
T_MFW_RES gprs_setQOS(SHORT cid ,T_PS_qos *inputQos)
{
  T_MFW_GPRS_CONTEXT *cntxt; 

  TRACE_FUNCTION ("gprs_setQOS()");

  /*
   * ACI call
   */
  if(sAT_PlusCGQREQ(CMD_SRC_LCL, (U8)cid, inputQos) == AT_FAIL)
  {
  TRACE_EVENT("MFW_GPRS: Error. sAT_PlusCGQREQ failed");
    return MFW_RES_ERR;
  }

  /*
   * change qos data in MFW GPRS data
   */
  cntxt = (T_MFW_GPRS_CONTEXT *) mfwAlloc(sizeof(T_MFW_GPRS_CONTEXT));

  cntxt->id = (U8)cid;
  memcpy (&(cntxt->data.qos), inputQos, sizeof (T_PS_qos));

  gprs_signal(E_MFW_GPRS_S_QOS, cntxt);

  return MFW_RES_OK;
}
#else
T_MFW_RES gprs_setQOS(SHORT cid ,T_PS_qos *inputQos)
{
  T_MFW_GPRS_CONTEXT *cntxt; 

  TRACE_FUNCTION ("gprs_setQOS()");

  /*
   * ACI call
   */
  if(sAT_PlusCGQREQ(CMD_SRC_LCL, cid, inputQos) == AT_FAIL)
  {
  TRACE_EVENT("MFW_GPRS: Error. sAT_PlusCGQREQ failed");
    return MFW_RES_ERR;
  }

  /*
   * change qos data in MFW GPRS data
   */
  cntxt = (T_MFW_GPRS_CONTEXT *) mfwAlloc(sizeof(T_MFW_GPRS_CONTEXT));

  cntxt->id = cid;
  memcpy (&(cntxt->data.qos), inputQos, sizeof (T_PS_qos));

  gprs_signal(E_MFW_GPRS_S_QOS, cntxt);

  return MFW_RES_OK;
}
#endif


/*
+---------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE:  MFW_GPRS             |
| STATE  : code                         ROUTINE: gprs_setQOSMin       |
+---------------------------------------------------------------------+


   PURPOSE : Set the minimum acceptable Quality of Service Profile for 
             the context specified by cid.

   PARAMETERS :
             cid      : context ID
            *inputQos : quality of service data
*/
#ifdef FF_2TO1_PS
T_MFW_RES gprs_setQOSMin(SHORT cid ,T_PS_qos *inputQos)
{
  T_MFW_GPRS_CONTEXT *cntxt; 

  TRACE_FUNCTION ("gprs_setQOSMin()");

  /*
   * ACI call
   */
  if (sAT_PlusCGQMIN(CMD_SRC_LCL, (U8)cid, (T_PS_min_qos*)inputQos) == AT_FAIL)
  {
  TRACE_EVENT("MFW_GPRS: Error. sAT_PlusCGQMIN failed");
    return MFW_RES_ERR;
  }

  /*
   * change mininum acceptable qos data in MFW GPRS data
   */
  cntxt = (T_MFW_GPRS_CONTEXT *) mfwAlloc(sizeof(T_MFW_GPRS_CONTEXT));

  cntxt->id = (U8)cid;
  memcpy (&(cntxt->data.min_qos), inputQos, sizeof (T_PS_qos));

  gprs_signal(E_MFW_GPRS_S_QOS_MIN, cntxt);

  return MFW_RES_OK;
}
#else
T_MFW_RES gprs_setQOSMin(SHORT cid ,T_PS_qos *inputQos)
{
  T_MFW_GPRS_CONTEXT *cntxt; 

  TRACE_FUNCTION ("gprs_setQOSMin()");

  /*
   * ACI call
   */
  if (sAT_PlusCGQMIN(CMD_SRC_LCL, (U8)cid, (T_PS_min_qos*)inputQos) == AT_FAIL)
  {
  TRACE_EVENT("MFW_GPRS: Error. sAT_PlusCGQMIN failed");
    return MFW_RES_ERR;
  }

  /*
   * change mininum acceptable qos data in MFW GPRS data
   */
  cntxt = (T_MFW_GPRS_CONTEXT *) mfwAlloc(sizeof(T_MFW_GPRS_CONTEXT));

  cntxt->id = cid;
  memcpy (&(cntxt->data.min_qos), inputQos, sizeof (T_PS_qos));

  gprs_signal(E_MFW_GPRS_S_QOS_MIN, cntxt);

  return MFW_RES_OK;
}
#endif


/*
+---------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE:  MFW_GPRS             |
| STATE  : code                         ROUTINE: gprs_attach          |
+---------------------------------------------------------------------+


   PURPOSE : Attach to or detach from the GPRS Service. The parameter 
             state indicates the state of GPRS attchement 

   PARAMETERS :
             state    : attachement state ( 0 - detached, 1 - attached)

*/
T_MFW_RES gprs_attach(T_CGATT_STATE state)
{
  T_ACI_RETURN result;
  
  TRACE_FUNCTION ("gprs_attach()");

  /*
   * ACI call
   * SPR#1574 - SH - Don't return fail for AT_CMPL result
   */
  result = sAT_PlusCGATT(CMD_SRC_LCL, state);
  if ( result!= AT_EXCT && result!=AT_CMPL)
  {
  TRACE_EVENT("MFW_GPRS: Error. sAT_PlusCGATT failed");
    return MFW_RES_ERR;
  	}
  
  /*
   * change attachement state in MFW GPRS data
   */
  gprs_signal(E_MFW_GPRS_S_ATT, &state);


  return MFW_RES_OK;
}


/*
+---------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE:  MFW_GPRS            |
| STATE  : code                         ROUTINE: gprs_attach_abort       |
+---------------------------------------------------------------------+


   PURPOSE : Cancel attachment to or detachment from GPRS service
   SPR#1983 - SH - Added.
              
   PARAMETERS : None.
   
*/
T_MFW_RES gprs_attach_abort(void)
{
	T_ACI_RETURN result;

	TRACE_EVENT("gprs_attach_abort()");
	
	result = sAT_Abort(CMD_SRC_LCL, AT_CMD_CGATT);

	if (result==AT_FAIL)
		return MFW_RES_ERR;

	return MFW_RES_OK;
}


/*
+----------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE:  MFW_GPRS              |
| STATE  : code                         ROUTINE: gprs_contextActivation|
+----------------------------------------------------------------------+


   PURPOSE : Activate or Deactivate PDP contexts. The parameter state 
             indicates the state of GPRS activation.
             The parameter cids points to a list of contexts definitions. 
             If the list is empty all contexts will be attached or 
             detached. If the mobile is not attached before, a GPRS 
             attach is first performed. 

   PARAMETERS :
             state    : activation state ( 0 - deactivated, 1 - activated)
             cids     : list of contexts
*/
T_MFW_RES gprs_contextActivation(T_CGACT_STATE state, SHORT *cids)
{
  T_MFW_GPRS_CNTXT_ACT *cntxtAct; 

  TRACE_FUNCTION ("gprs_contextActivation()");

  /*
   * ACI call
   */
#ifdef FF_2TO1_PS
  if (sAT_PlusCGACT(CMD_SRC_LCL, state, (U8 *)cids) == AT_FAIL)
#else
  if (sAT_PlusCGACT(CMD_SRC_LCL, state, cids) == AT_FAIL)
#endif      
  {
  TRACE_EVENT("MFW_GPRS: Error. sAT_PlusCGCACT failed");
    return MFW_RES_ERR;
  }


  /*
   * change context activation data in MFW GPRS data
   */
  cntxtAct = (T_MFW_GPRS_CNTXT_ACT *) mfwAlloc(sizeof(T_MFW_GPRS_CNTXT_ACT));

  memcpy (&(cntxtAct->ids), cids, sizeof (SHORT));
  cntxtAct->state = state;

  gprs_signal(E_MFW_GPRS_S_ACT, cntxtAct);

  return MFW_RES_OK;
}


/*
+---------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE:  MFW_GPRS             |
| STATE  : code                         ROUTINE: gprs_setData         |
+---------------------------------------------------------------------+


   PURPOSE : Enter data state. Whatever actions are necessary to 
             establish communication between the TE and the network 
             will be performed (e.g. attach, context activate)
   PARAMETERS :
             L2P      : Layer 2 protocoll
             cids     : list of contexts

*/
#ifdef FF_2TO1_PS
T_MFW_RES gprs_setData(char *L2P, SHORT *cids)
{
  T_MFW_GPRS_CNTXT_L2P *cntxtL2P; 

  TRACE_FUNCTION ("gprs_setData()");

  /*
   * ACI call
   */
  if (sAT_PlusCGDATA(CMD_SRC_LCL, L2P, (U8 *)cids) == AT_FAIL)
  {
  TRACE_EVENT("MFW_GPRS: Error. sAT_PlusCGDATA failed");
    return MFW_RES_ERR;
  }


  /*
   * change attachement, activation and L2P data in MFW GPRS data
   */
  cntxtL2P = (T_MFW_GPRS_CNTXT_L2P *) mfwAlloc(sizeof(T_MFW_GPRS_CNTXT_L2P));
  cntxtL2P->L2P = (char *)  mfwAlloc((U16)(sizeof(char) * strlen(L2P)));
  cntxtL2P->ids = (USHORT *) mfwAlloc(sizeof(SHORT) * MAX_CID);

  memcpy (cntxtL2P->L2P, L2P, sizeof (CHAR) * strlen(L2P));
  memcpy (cntxtL2P->ids, cids, sizeof (SHORT));

  gprs_signal(E_MFW_GPRS_S_DATA, cntxtL2P);

  mfwFree( (U8 *) cntxtL2P->L2P, (U16)(sizeof(char) * strlen(L2P)));
  mfwFree( (U8 *) cntxtL2P->ids, (U16)(sizeof(SHORT) * MAX_CID));

  return MFW_RES_OK;
}
#else
T_MFW_RES gprs_setData(char *L2P, SHORT *cids)
{
  T_MFW_GPRS_CNTXT_L2P *cntxtL2P; 

  TRACE_FUNCTION ("gprs_setData()");

  /*
   * ACI call
   */
  if (sAT_PlusCGDATA(CMD_SRC_LCL, L2P, (U8 *)cids) == AT_FAIL)
  {
  TRACE_EVENT("MFW_GPRS: Error. sAT_PlusCGDATA failed");
    return MFW_RES_ERR;
  }


  /*
   * change attachement, activation and L2P data in MFW GPRS data
   */
  cntxtL2P = (T_MFW_GPRS_CNTXT_L2P *) mfwAlloc(sizeof(T_MFW_GPRS_CNTXT_L2P));
  cntxtL2P->L2P = (char *)  mfwAlloc(sizeof(char) * strlen(L2P));
  cntxtL2P->ids = (USHORT *) mfwAlloc(sizeof(SHORT) * MAX_CID);

  memcpy (cntxtL2P->L2P, L2P, sizeof (CHAR) * strlen(L2P));
  memcpy (cntxtL2P->ids, cids, sizeof (SHORT));

  gprs_signal(E_MFW_GPRS_S_DATA, cntxtL2P);

  mfwFree( (void *) cntxtL2P->L2P, sizeof(char) * strlen(L2P));
  mfwFree( (void *) cntxtL2P->ids, sizeof(SHORT) * MAX_CID);

  return MFW_RES_OK;
}
#endif



/*
+-----------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE:  MFW_GPRS               |
| STATE  : code                         ROUTINE: gprs_showPDPAddress    |
+-----------------------------------------------------------------------+


   PURPOSE : Show PDP address. The Function sets the pointer pdp_adress 
             to a list of PDP addresses for the specified context 
             identifiers in the parameter cids.

   PARAMETERS :
             pdp_address: list of contexts
   
   RETURNS:
             MFW_RES_OK:  On success
       MFW_RES_ERR: On failure
*/
#ifdef FF_2TO1_PS
T_MFW_RES gprs_showPDPAddress(SHORT *cids, T_NAS_ip *pdp_address)
{
  int i;

  T_MFW_GPRS_CONTEXT *cntxt; 

  TRACE_FUNCTION ("gprs_showPDPAddress()");

  /*
   * ACI call, get PDP addresses
   */
  if (sAT_PlusCGPADDR(CMD_SRC_LCL, (U8*)cids, pdp_address) == AT_FAIL)
  {
  TRACE_EVENT("MFW_GPRS: Error. sAT_PlusCGPADDR failed");
    return MFW_RES_ERR;
  }


  /*
   * change PDP address data in MFW GPRS data
   */
  cntxt = (T_MFW_GPRS_CONTEXT *) mfwAlloc(sizeof(T_MFW_GPRS_CONTEXT));

  for (i=0; i < MAX_CID; i++)
  {
    cntxt->id = (U8)cids[i];
    memcpy (&(cntxt->data.attributes.pdp_addr), &pdp_address[i], sizeof (T_NAS_ip));

    gprs_signal(E_MFW_GPRS_S_PDPADDR, cntxt);
  }

  return MFW_RES_OK;
}
#else
T_MFW_RES gprs_showPDPAddress(SHORT *cids, T_NAS_ip *pdp_address)
{
  int i;

  T_MFW_GPRS_CONTEXT *cntxt; 

  TRACE_FUNCTION ("gprs_showPDPAddress()");

  /*
   * ACI call, get PDP addresses
   */
  if (sAT_PlusCGPADDR(CMD_SRC_LCL, cids, pdp_address) == AT_FAIL)
  {
  TRACE_EVENT("MFW_GPRS: Error. sAT_PlusCGPADDR failed");
    return MFW_RES_ERR;
  }


  /*
   * change PDP address data in MFW GPRS data
   */
  cntxt = (T_MFW_GPRS_CONTEXT *) mfwAlloc(sizeof(T_MFW_GPRS_CONTEXT));

  for (i=0; i < MAX_CID; i++)
  {
    cntxt->id = cids[i];
    memcpy (&(cntxt->data.attributes.pdp_addr), &pdp_address[i], sizeof (T_NAS_ip));

    gprs_signal(E_MFW_GPRS_S_PDPADDR, cntxt);
  }

  return MFW_RES_OK;
}
#endif


/*
+----------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE:  MFW_GPRS              |
| STATE  : code                         ROUTINE: gprs_setAutoResponse  |
+----------------------------------------------------------------------+


   PURPOSE : Enable or Disable an automatic positive response to the 
             receipt of Context Activation Requests from the network. 
             The parameter mode indicates if the automatic response will 
             be enabled or disabled.

   PARAMETERS :
             mode : mode of auto response
*/
T_MFW_RES gprs_setAutoResponse(T_CGAUTO_N mode)
{

  TRACE_FUNCTION ("gprs_setAutoResponse()");

  /*
   * ACI call
   */
  if (sAT_PlusCGAUTO(CMD_SRC_LCL, mode) == AT_FAIL)
  {
  TRACE_EVENT("MFW_GPRS: Error. sAT_PlusCGAUTO failed");
    return MFW_RES_ERR;
  }

  /*
   * change auto response mode in MFW GPRS data
   */
  gprs_signal(E_MFW_GPRS_S_AUTORESP, &mode);

  return MFW_RES_OK;
}


/*
+----------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE:  MFW_GPRS              |
| STATE  : code                         ROUTINE: gprs_ManualResponse   |
+----------------------------------------------------------------------+


   PURPOSE : This Function is for a manual response to network request 
             for PDP context activation. The parameter response 
             indicates if the request will be accepted or rejected.
   PARAMETERS :
             response : ( 0 - rejected, 1 - accepted)
*/
T_MFW_RES gprs_ManualResponse(USHORT response, char *l2p, SHORT cid)
{
  TRACE_FUNCTION ("gprs_ManualResponse()");

  /*
   * send signal to current MFW element
   */
  gprs_signal(E_MFW_GPRS_S_ANS, &cid);

  /*
   * ACI call
   */
  if (sAT_PlusCGANS(CMD_SRC_LCL, response, l2p, cid) == AT_FAIL)
  {
  TRACE_EVENT("MFW_GPRS: Error. sAT_PlusCGANS failed");
    return MFW_RES_ERR;
  }

  return MFW_RES_OK;
}


/*
+----------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE:  MFW_GPRS              |
| STATE  : code                         ROUTINE: gprs_setClass         |
+----------------------------------------------------------------------+


   PURPOSE : Set the mobile to operate according to a GPRS mobile class. 

   PARAMETERS :
             m_class : GPRS mobile class
*/
T_MFW_RES gprs_setClass(T_CGCLASS_CLASS new_cls)
{
  T_CGATT_STATE state;

  TRACE_FUNCTION ("gprs_setClass()");

  /*
   * ACI call
   */
  if (sAT_PlusCGCLASS(CMD_SRC_LCL, new_cls) == AT_FAIL)
  {
  TRACE_EVENT("MFW_GPRS: Error. sAT_PlusCGCLASS failed");
    return MFW_RES_ERR;
  }


  /*
   * change class data in MFW GPRS data
   */
  gprs_signal(E_MFW_GPRS_S_CLASS, &new_cls);


  /*
   * check if attachement mode has changed while class change
   */
  if (qAT_PlusCGATT(CMD_SRC_LCL, &state) != AT_FAIL )
    gprs_signal(E_MFW_GPRS_S_ATT, &state);
  else
  TRACE_EVENT("MFW_GPRS: Error. qAT_PlusCGATT failed");

  return MFW_RES_OK;
}


/*
+----------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE:  MFW_GPRS              |
| STATE  : code                         ROUTINE: gprs_setEventReporting|
+----------------------------------------------------------------------+


   PURPOSE : Enables or Disables  the sending of certain events 
             occuring in the GPRS ME or the network to the TE. 

   PARAMETERS :
             mode : mode to specifies if the event reporting will be 
                    enabled or disabled
             bfr  : controls the effect on buffered events. 
*/
T_MFW_RES gprs_setEventReporting(T_CGEREP_MODE mode, T_CGEREP_BFR bfr)
{
  T_MFW_GPRS_EVENTREP *eventRep; 

  TRACE_FUNCTION ("gprs_setEventReporting()");

  /*
   * ACI call
   */
  if (sAT_PlusCGEREP(CMD_SRC_LCL, mode, bfr) == AT_FAIL)
    return MFW_RES_ERR;

  /*
   * change event reporting data in MFW GPRS data
   */
  eventRep = (T_MFW_GPRS_EVENTREP *) mfwAlloc(sizeof(T_MFW_GPRS_EVENTREP));

  eventRep->mode = mode;
  eventRep->bfr  = bfr;
  gprs_signal(E_MFW_GPRS_S_EREP, eventRep);

  return MFW_RES_OK;
}


/*
+----------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE:  MFW_GPRS              |
| STATE  : code                         ROUTINE: gprs_setServiceSMS    |
+----------------------------------------------------------------------+


   PURPOSE : The function specifies the service or service preference 
             to be used for SMS. 
 

   PARAMETERS :
             n : service to use for SMS ( 0 - GPRS, 1 - circuit switch, 
                 2 - GPRS preferred, 3 circuit switch preferred)
*/

T_MFW_RES gprs_setServiceSMS(T_CGSMS_SERVICE service)
{
  TRACE_FUNCTION ("gprs_setServiceSMS()");

  /*
   * ACI call
   */
  if (sAT_PlusCGSMS(CMD_SRC_LCL, service) == AT_FAIL)
    return MFW_RES_ERR;


  /*
   * change SMS service data in MFW GPRS data
   */
  gprs_signal(E_MFW_GPRS_S_SMSSERV, &service);

  return MFW_RES_OK;
}


/*
+----------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE:  MFW_GPRS              |
| STATE  : code                         ROUTINE: gprs_getServiceSMS    |
+----------------------------------------------------------------------+


   PURPOSE : This function returns the service preference to be used for SMS.
 

   PARAMETERS :
             n : service to use for SMS ( 0 - GPRS, 1 - circuit switch, 
                 2 - GPRS preferred, 3 circuit switch preferred)
*/

T_CGSMS_SERVICE gprs_getServiceSMS(void)
{	
	T_CGSMS_SERVICE service;
	
 if (qAT_PlusCGSMS(CMD_SRC_LCL, &service) != AT_FAIL )
    return service;
  else
    TRACE_EVENT("MFW_GPRS: Error. qAT_PlusCGCLASS failed");
	return CGSMS_SERVICE_INVALID;
}


/*
+----------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE:  MFW_GPRS              |
| STATE  : code                         ROUTINE: gprs_counter    |
+----------------------------------------------------------------------+


   PURPOSE : SPR#1450 - SH

   Resets the data counter if reset_counter is TRUE.
   In either case, response returned in rAT_PercentSNCNT below.
*/

T_MFW_RES gprs_counter(BOOL reset_counter)
{
	T_ACI_RETURN result;
	TRACE_FUNCTION("gprs_counter");

	result = sAT_PercentSNCNT(CMD_SRC_LCL, reset_counter);
	
	if (result!=AT_EXCT && result!=AT_CMPL)
		return MFW_RES_ERR;

	if (result==AT_CMPL)
		return MfwResDone;		/* No response expected */
		
	return MFW_RES_OK;
}


/*
+---------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE:  MFW_GPRS            |
| STATE  : code                         ROUTINE: gprs_counter_abort      |
+---------------------------------------------------------------------+


   PURPOSE : Cancel request for data count
   SPR#1983 - SH - Added.
              
   PARAMETERS : None.
   
*/

T_MFW_RES gprs_counter_abort(void)
{
	T_ACI_RETURN result;
	
	result = sAT_Abort(CMD_SRC_LCL, AT_CMD_SNCNT);

	if (result==AT_FAIL)
		return MFW_RES_ERR;

	return MFW_RES_OK;
}

#ifdef NEPTUNE_BOARD
/* OMAP00074454 - 24-04-2006 */
/* ===========================================================*/
/**
* mfw_gprs_edge_status () Returns the GPRS and EDGE connection established status value
*
* @param int *iStatus - Integer pointer through which the status is returned.
*
* @return integer - 0 - GSM ONLY, 1 - GPRS ONLY, 2 - EDGE ONLY
*
*  @see
*/
/* =========================================================== */
int mfw_gprs_edge_status(int *iStatus)
{
       /* Get the registered status */ 
	T_P_CGREG_STAT status = (T_P_CGREG_STAT)0;
	T_ACI_CREG_STAT stat = (T_ACI_CREG_STAT)0; 
	USHORT lac = 0;
	USHORT cid = 0;
	U8 rat          = 0;
	U8 gprs_ind = 0;
	/* char temp[TEMP_STRING_SIZE]; */

 
       /* Get the GPRS registered status */
	if (qAT_PercentCGREG(CMD_SRC_LCL, &status, &lac, &cid) == AT_FAIL )
 	{
    	     TRACE_EVENT("MFW_GPRS: Error. qAT_PercentCGREG failed");

	      *iStatus = (int)0; /* GSM only */  
		return 0;            /* rat - returned as GSM only */			 
	}



       /* Reassign lac and cid values */
	lac = 0;
	cid = 0;

       /* To get the gprs_ind and rat */
	if(qAT_PercentCREG(CMD_SRC_LCL, &stat, &lac, &cid, &gprs_ind, &rat) NEQ AT_CMPL)
	{
    	     TRACE_EVENT("MFW_GPRS: Error. qAT_PercentCREG failed");
	       
	      *iStatus = (int)status;
		return 0;                    /* rat - returned as GSM only */
	}

	*iStatus = (int)status;
 	return ((int)rat);
}
/* END */
#endif



/*
+----------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE:  MFW_GPRS              |
| STATE  : code                         ROUTINE: gprs_status    |
+----------------------------------------------------------------------+


   PURPOSE : Returns the GPRS status value
*/

T_CGREG_STAT gprs_status()
{

      /*a0393213 compiler warnings removal - variable result removed*/

	T_CGREG_STAT status = (T_CGREG_STAT)0;
	USHORT lac, ci;
	
	TRACE_FUNCTION("gprs_status");

	if (qAT_PlusCGREG(CMD_SRC_LCL, &status, &lac, &ci) == AT_FAIL )
 	{
    	TRACE_EVENT("MFW_GPRS: Error. qAT_PlusCGREG failed");
	}
	
	emo_printf("LOCATION: lac: %d ci: %d", lac, ci);
	
	return status;
}
//x0035544 Feb 23, 2006 DR:OMAPS00068976
/*
	PURPOSE : Returns the GPRS status value
*/
T_P_CGREG_STAT p_gprs_status()
{

      /*a0393213 compiler warnings removal - variable result removed*/

T_P_CGREG_STAT status = (T_P_CGREG_STAT)0;
	USHORT lac, ci;
	
	TRACE_FUNCTION("p_gprs_status");

	if (qAT_PercentCGREG(CMD_SRC_LCL, &status, &lac, &ci) == AT_FAIL )
	{
		TRACE_EVENT("MFW_GPRS: Error. qAT_PercentCGREG failed");
	}
		
	gprs_set_location(lac, ci);
	  
	return status;
}


/*--------------------------------------------------------------------
  ACI callback functions 
--------------------------------------------------------------------*/

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE:  MFW_GPRS            |
| STATE  : code                         ROUTINE: rAT_PlusCGACT       |
+--------------------------------------------------------------------+


   PURPOSE : The function rAt_PlusCGACT informs the application about 
             the link identifier for the data transfer.
             ( ACI callback funtion )

   PARAMETERS :
             link_id: Link identifier                 

*/
void rAT_PlusCGACT ( SHORT link_id )
{
  TRACE_FUNCTION ("rAT_PlusCGACT()");

  /*
   * send event and link id to the current mfw element
   */
  gprs_signal(E_MFW_GPRS_R_ACT, &link_id);
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE:  MFW_GPRS            |
| STATE  : code                         ROUTINE: rAT_PlusCGDATA      |
+--------------------------------------------------------------------+


   PURPOSE : The function rAt_PlusCGDATA informs the application about 
             the link identifier for the data transfer after 
             activation/deactivation (without connect).
             ( ACI callback funtion )

   PARAMETERS :
             link_id: Link identifier                 
*/
void rAT_PlusCGDATA ( SHORT link_id )
{
  TRACE_FUNCTION ("rAT_PlusCGDATA()");

  /*
   * send event and link id to the current mfw element
   */
  gprs_signal(E_MFW_GPRS_R_DATA, &link_id);
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE:  MFW_GPRS            |
| STATE  : code                         ROUTINE: rAT_PlusCGANS       |
+--------------------------------------------------------------------+


   PURPOSE : The function rAt_PlusCGANS informs the application about 
             the link identifier for the data transfer. It is a 
             manual reponse to a network request for PDP context 
             activation (after request by +CRING or RING).
             ( ACI callback funtion )

   PARAMETERS :
             link_id: Link identifier                 
*/
void rAT_PlusCGANS ( SHORT link_id )
{
  TRACE_FUNCTION ("rAT_PlusCGANS()");

  /*
   * send event and link id to the current mfw element
   */
  gprs_signal(E_MFW_GPRS_R_ANS, &link_id);
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE:  MFW_GPRS            |
| STATE  : code                         ROUTINE: rAT_PlusCGEREP     |
+--------------------------------------------------------------------+


   PURPOSE : The function rAt_PlusCGEREP informs the application about 
             GPRS events connect. 
             ( ACI callback funtion )

   PARAMETERS :
             event  : GPRS event       
             param  : event parameter 
             
*/
void rAT_PlusCGEREP ( T_CGEREP_EVENT            event, 
                      T_CGEREP_EVENT_REP_PARAM *param )
{

  /*a0393213 compiler warnings removal - variable msg removed*/

  T_CGATT_STATE state;

  TRACE_FUNCTION ("rAT_PlusCGEREP()");
  TRACE_EVENT_P1("GPRS eventrep: %d", (short)event);

  /*
   * send event and data to the current mfw element
   * SPR#2243 - SH - Use appropriate union fields in each gprs_signal.
   */
  switch ( event )
  {
    case CGEREP_EVENT_REJECT:
      gprs_signal(E_MFW_GPRS_R_EREP_RJ, &param->reject);
      break;

    case CGEREP_EVENT_NW_DETACH:
    case CGEREP_EVENT_ME_DETACH:
    state = CGATT_STATE_DETACHED;
      gprs_signal(E_MFW_GPRS_R_EREP_ATT, &state);
      break;

    case CGEREP_EVENT_NW_REACT:
      gprs_signal(E_MFW_GPRS_R_EREP_ACT, &param->act);
      break;

    case CGEREP_EVENT_NW_DEACT:
    case CGEREP_EVENT_ME_DEACT:
      gprs_signal(E_MFW_GPRS_R_EREP_DEACT, &param->act);
      break;

    case CGEREP_EVENT_NW_CLASS:
    case CGEREP_EVENT_ME_CLASS:
      gprs_signal(E_MFW_GPRS_R_EREP_CLASS, &param->mobile_class);
      break;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE:  MFW_GPRS            |
| STATE   : code                        ROUTINE : rAT_PercentCGEV     |
+--------------------------------------------------------------------+

  PURPOSE : handles rAT_PercentCGEV call back

*/
GLOBAL void rAT_PercentCGEV (T_CGEREP_EVENT event, T_CGEREP_EVENT_REP_PARAM *param )
{
  /*a0393213 compiler warnings removal - variable state removed*/

  TRACE_FUNCTION ("rAT_PercentCGEV()");
  TRACE_EVENT_P1("GPRS eventrep: %d", (short)event);

  /*
   * send event and data to the current mfw element
   */
  switch ( event )
  {
    case CGEREP_EVENT_REJECT:
      /* gprs_signal(E_MFW_GPRS_R_EREP_RJ, &param->reject);   - Use a different event for %CGEV */
      break;

    case CGEREP_EVENT_NW_DETACH:
    case CGEREP_EVENT_ME_DETACH:
    
      /* gprs_signal(E_MFW_GPRS_R_EREP_ATT, &state);   - Use a different event for %CGEV */
      break;

    case CGEREP_EVENT_NW_REACT:
      /* gprs_signal(E_MFW_GPRS_R_EREP_ACT, &param->act);   - Use a different event for %CGEV */
      break;

    case CGEREP_EVENT_NW_DEACT:
    case CGEREP_EVENT_ME_DEACT:
      /* gprs_signal(E_MFW_GPRS_R_EREP_DEACT, &param->act);   - Use a different event for %CGEV */
      break;

    case CGEREP_EVENT_NW_CLASS:
    case CGEREP_EVENT_ME_CLASS:
      /* gprs_signal(E_MFW_GPRS_R_EREP_CLASS, &param->mobile_class);   - Use a different event for %CGEV */
      break;

/* BEGIN ADD: Neptune Alignment*/
#ifndef NEPTUNE_BOARD
/* END ADD: Neptune Alignment */
    case CGEREP_EVENT_NW_ACT:
    case CGEREP_EVENT_ME_ACT:
    /*state = CGATT_STATE_ATTACHED;*/
      /* gprs_signal(E_MFW_GPRS_R_EREP_ATT, &state);   - Use a different event for %CGEV */
      break;
/* BEGIN ADD: Neptune Alignment*/
#endif
/* END ADD: Neptune Alignment */

  }
}


/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE:  MFW_GPRS            |
| STATE  : code                         ROUTINE: rAT_PlusCGREG       |
+--------------------------------------------------------------------+


   PURPOSE : The function rAt_PlusCGREG informs the application about 
             the GPRS registration status and location information.
             ( ACI callback funtion )

   PARAMETERS :
             stat   : indicates the current registration
             lac    : two byte location area code   
             ci     : two byte cell ID    
*/
void rAT_PlusCGREG  ( T_CGREG_STAT stat, 
                      USHORT       lac, 
                      USHORT       ci )
{ 
  T_MFW_GPRS_CELL_STRUCT cell;

  TRACE_FUNCTION ("rAT_PlusCGREG()");
#ifndef WIN32
  TRACE_EVENT_P3("GPRS regState: %d, lac %X, ci %X", (short)stat, lac, ci);
#endif
  /*
   * send signal for changed registry state to current
   * MFW element
   */

  cell.stat = stat;
  cell.lac = lac;
  cell.ci = ci;
  gprs_signal(E_MFW_GPRS_R_REG, &cell);
}



/*
+--------------------------------------------------------------------+
| PROJECT : GPRS (8441)                 MODULE  : GACI_RET           |
| STATE   : code                        ROUTINE : rAT_PercentCGREG      |
+--------------------------------------------------------------------+

  PURPOSE : handles rAT_PercentCGREG call back

*/

GLOBAL void rAT_PercentCGREG ( T_P_CGREG_STAT stat, USHORT lac, USHORT ci, BOOL bActiveContext )

{
  
  T_MFW_GPRS_CELL_STRUCT_P cell;

  TRACE_FUNCTION ("rAT_PercentCGREG()");
#ifndef WIN32
  TRACE_EVENT_P3("GPRS regState: %d, lac %X, ci %X", (short)stat, lac, ci);
#endif
  /*
   * send signal for changed registry state to current
   * MFW element
   */
TRACE_EVENT_P3("State received in RAT-GPRS State: %d, lac %X, ci %X", (short)stat, lac, ci);
  cell.stat = stat;
  cell.lac = lac;
  cell.ci = ci;
  gprs_signal(E_MFW_GPRS_R_REG, &cell);

}



/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE:  MFW_GPRS            |
| STATE  : code                         ROUTINE: rAT_changedQOS      |
+--------------------------------------------------------------------+


   PURPOSE : The function rAt_changedQOS informs the application about 
             changes for the Quality of Service Profiles.
             ( ACI callback funtion )

   PARAMETERS :
             cid    : 
            *qos    : 
*/
// ADDED - 1-11-2005
#ifdef FF_2TO1_PS
void rAT_changedQOS ( U8  cid, 
                      T_PS_qos *qos )
{
  T_MFW_GPRS_CONTEXT *cntxt; 

  TRACE_FUNCTION ("rAT_changedQOS()");

  /*
   * send signal for changed quality of service profile to current
   * MFW element
   */
  cntxt = (T_MFW_GPRS_CONTEXT *) mfwAlloc(sizeof(T_MFW_GPRS_CONTEXT));

  cntxt->id = cid;
   memcpy (&(cntxt->data.qos), qos, sizeof (T_PS_qos));

  gprs_signal(E_MFW_GPRS_R_QOS, cntxt);
}

#else
void rAT_changedQOS ( SHORT cid, 
                      T_PS_qos *qos )
{
  T_MFW_GPRS_CONTEXT *cntxt; 

  TRACE_FUNCTION ("rAT_changedQOS()");

  /*
   * send signal for changed quality of service profile to current
   * MFW element
   */
  cntxt = (T_MFW_GPRS_CONTEXT *) mfwAlloc(sizeof(T_MFW_GPRS_CONTEXT));

  cntxt->id = (U8)cid;
  memcpy (&(cntxt->data.qos), qos, sizeof (T_PS_qos));

  gprs_signal(E_MFW_GPRS_R_QOS, cntxt);
}

#endif
// END - 1-11-2005

/*
+--------------------------------------------------------------------+
| PROJECT : GPRS (8441)                 MODULE  : MFW_GPRS          |
| STATE   : code                        ROUTINE : rAT_PercentSNCNT   |
+--------------------------------------------------------------------+

  PURPOSE : handles rAT_PercentSNCNT call back

*/

GLOBAL void rAT_PercentSNCNT ( UBYTE c_id,
                               ULONG octets_uplink,
                               ULONG octets_downlink,
                               ULONG packets_uplink,
                               ULONG packets_downlink )
{
	T_MFW_GPRS_COUNTER	counter;

	TRACE_FUNCTION("rAT_PercentSNCNT()");
		
	counter.octets_uplink = octets_uplink;
	counter.octets_downlink = octets_downlink;
	counter.packets_uplink = packets_uplink;
	counter.packets_downlink = packets_downlink;

	gprs_signal(E_MFW_GPRS_COUNTER, &counter);
	
	return;
}

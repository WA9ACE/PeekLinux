

/*
+-----------------------------------------------------------------------------
|  Project :
|  Modul   :
+-----------------------------------------------------------------------------
|  Copyright 2002 Texas Instruments Berlin, AG
|                 All rights reserved.
|
|                 This file is confidential and a trade secret of Texas
|                 Instruments Berlin, AG
|                 The receipt of or possession of this file does not convey
|                 any rights to reproduce or disclose its contents or to
|                 manufacture, use, or sell anything it may describe, in
|                 whole, or in part, without the specific written consent of
|                 Texas Instruments Berlin, AG.
+-----------------------------------------------------------------------------
|  Purpose :  This Module defines the engineering mode (EM) device driver for the
|             G23 protocol stack. This driver is used to control all engineering
|             mode related functions.
+-----------------------------------------------------------------------------
*/

#ifndef GMM_EM_C
#define GMM_EM_C

#define ENTITY_GMM

/*--------- INCLUDES -----*/


#include "typedefs.h"    /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "custom.h"     /* to get EM for Simulation Stack compiled*/
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "gmm.h"        /* to get the global entity definitions */

#include  <string.h>    /* to get memcpy , memset*/

#include "gmm_em.h"     /*to get EM spezific definitions*/

#ifdef FF_EM_MODE
GLOBAL UBYTE em_gmm_event_buffer[EM_GMM_BUFFER_SIZE];
GLOBAL UBYTE em_gmm_buffer_write;

/* Event tracing flags for EM */
GLOBAL BOOL gmm_v[EM_MAX_GMM_EVENTS];

static UBYTE em_gmm_trace_occured;


/*----------Transfer Functions from and to GRR/GRLC -------*/
/*This functions handle Engineering Mode Primitives that are sent from ACI
  to GRR or GRLC (and the other way around). Because they cannot be sent directly,
 they have to be passed through GMM
 */
/*
+------------------------------------------------------------------------------
| Function    : em_gmm_sc_gprs_info_req
+------------------------------------------------------------------------------
| Description : The function em_gmm_sc_gprs_info_req() is used to forward a
|               request from ACI to GRR.
|
| Parameters  : EM_SC_GPRS_INFO_REQ
|
+------------------------------------------------------------------------------
*/
GLOBAL void em_gmm_sc_gprs_info_req ( T_EM_SC_GPRS_INFO_REQ *em_sc_gprs_info_req)
{ 
  GMM_TRACE_FUNCTION( "em_gmm_sc_gprs_info_req" );
  /* NO PFREE */
  PSEND ( hCommGRR, em_sc_gprs_info_req );

} /* em_gmm_sc_gprs_info_req () */

/*
+------------------------------------------------------------------------------
|  Function     : check_write_index
+------------------------------------------------------------------------------
|  Description  :  Checks the write index inside the buffer. No reset when
|                  buffer is full. 
|
|  Parameters   :  Number of bytes to be stored in buffer
|
|  Return       :  TRUE/FALSE                               
|                                                           
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE check_write_index (UBYTE n)
{
  TRACE_FUNCTION ("gmm_check_write_index()");
 
  if (em_gmm_buffer_write + n < EM_GMM_BUFFER_SIZE)
  {
    /*
     * ACI is informed about the first event trace, 
     * used for later data processing.
     */
    if (em_gmm_trace_occured EQ 0) 
    {
      PALLOC(data, EM_DATA_IND);
      data->entity = EM_GMM;
      PSENDX(MMI, data);
      em_gmm_trace_occured++;
    }
    return TRUE;
  }
  else
    return FALSE;
}
 
/*+------------------------------------------------------------------------------
|  Function     : em_init_gmm_event_trace
+------------------------------------------------------------------------------
|  Description  :  Initialize the event tracing flags for GMM
|
|  Parameters   :  void                   
|
|  Return       :  void                           
|                                                           
+------------------------------------------------------------------------------
*/
GLOBAL void em_init_gmm_event_trace(void)
{
  UBYTE i;

  TRACE_FUNCTION ("em_init_gmm_event_trace()");

  for (i = 0; i < EM_MAX_GMM_EVENTS; i++)
    gmm_v[i] = 0;

  em_gmm_buffer_write = 0;
}

/*
+------------------------------------------------------------------------------
|  Function     : em_gmm_event_req
+------------------------------------------------------------------------------
|  Description  :  Set the event tracing flags according the bitmask
|
|  Parameters   :  Primitive    - Bitmask
|
|  Return       :                                 
|                                                           
+------------------------------------------------------------------------------
*/

GLOBAL  void em_gmm_event_req (T_EM_GMM_EVENT_REQ *em_gmm_event_req)
{
  UBYTE i;

  TRACE_FUNCTION ("em_gmm_event_req()");

  /*
   *  The event tracing flags are set according the bitmask. mm_v[i] are 
   *  the flags belonging to the event number described in 8443.601
   */ 
  for(i = 1; i < EM_MAX_GMM_EVENTS; i++)
    gmm_v[i] = ((em_gmm_event_req->bitmask_gmm & (0x01<<(i-1))) > 0) ? TRUE : FALSE;

  /*
   * A new event trace is generated therefore the flag is reset.
   */
  em_gmm_trace_occured = 0;

  PFREE(em_gmm_event_req);
}

/*
+------------------------------------------------------------------------------
|  Function     : em_write_buffer_4
+------------------------------------------------------------------------------
|  Description  :  Perform buffer check and store corresponding data in it.
|
|  Parameters   :  Event number, data identity type (UBYTE), data value (UBYTE)
|
|  Return       :  TRUE/FALSE                               
| 
|  Pupose       :  Used for storing states                                                     
+------------------------------------------------------------------------------
*/

GLOBAL UBYTE em_write_buffer_4 (UBYTE event_no, UBYTE value1, UBYTE value2)
{ 
  TRACE_FUNCTION ("gmm_em_write_buffer_4()");  
  
  if (check_write_index(4))
  {
    em_gmm_event_buffer[em_gmm_buffer_write++] = event_no;          /* Tag: Event number */
    em_gmm_event_buffer[em_gmm_buffer_write++] = 2;                 /* Length */
    em_gmm_event_buffer[em_gmm_buffer_write++] = value1;
    em_gmm_event_buffer[em_gmm_buffer_write++] = value2;    
    return FALSE; /* Data is stored inside buffer, reset flag */
  }/* check write index*/
  else
    return TRUE;  /* No more space inside buffer, serve flag next time */
}
/*
+------------------------------------------------------------------------------
| Function    : em_gmm_sc_gprs_info_cnf
+------------------------------------------------------------------------------
| Description : The function em_gmm_sc_gprs_info_cnf() is used to forward a
|               confirm from GRR to ACI.
|
| Parameters  : EM_SC_GPRS_INFO_CN
|
+------------------------------------------------------------------------------
*/
GLOBAL void em_gmm_sc_gprs_info_cnf ( T_EM_SC_GPRS_INFO_CNF *em_sc_gprs_info_cnf)
{ 
  GMM_TRACE_FUNCTION( "em_gmm_sc_gprs_info_cnf" );
  /* NO PFREE */
  PSEND ( hCommMMI, em_sc_gprs_info_cnf );

} /* em_gmm_sc_gprs_info_cnf () */




/*
+------------------------------------------------------------------------------
| Function    : em_gmm_grlc_info_req
+------------------------------------------------------------------------------
| Description : The function em_gmm_grlc_info_req() is used to forward a
|               request from ACI to GRLC.
|
| Parameters  : EM_GRLC_INFO_REQ
|
+------------------------------------------------------------------------------
*/
GLOBAL void em_gmm_grlc_info_req ( T_EM_GRLC_INFO_REQ *em_grlc_info_req)
{ 
  GMM_TRACE_FUNCTION( "em_gmm_grlc_info_req" );
  /* NO PFREE */
  PSEND ( hCommGRLC, em_grlc_info_req);

} /* em_gmm_grlc_info_req () */


GLOBAL void em_gmm_throughput_info_req ( T_EM_THROUGHPUT_INFO_REQ *em_throughput_info_req)
{ 
  GMM_TRACE_FUNCTION( "em_gmm_throughput_info_req" );
  /* NO PFREE */
  PSEND ( hCommGRLC, em_throughput_info_req);

} /* em_gmm_throughput_info_req () */

GLOBAL void em_gmm_throughput_info_cnf ( T_EM_THROUGHPUT_INFO_CNF *em_throughput_info_cnf)
{ 
  GMM_TRACE_FUNCTION( "em_gmm_throughput_info_cnf" );
  /* NO PFREE */
  PSEND ( hCommMMI, em_throughput_info_cnf );

} /* em_gmm_throughput_info_cnf () */

GLOBAL void em_gmm_grr_event_req ( T_EM_GRR_EVENT_REQ *em_grr_event_req)
{ 
  GMM_TRACE_FUNCTION( "em_gmm_grr_event_req" );
  /* NO PFREE */
  PSEND ( hCommGRR, em_grr_event_req);

} /* em_gmm_grr_event_req () */

GLOBAL void em_gmm_grlc_event_req ( T_EM_GRLC_EVENT_REQ *em_grlc_event_req)
{ 
  GMM_TRACE_FUNCTION( "em_gmm_grlc_event_req" );
  /* NO PFREE */
  PSEND ( hCommGRLC, em_grlc_event_req);

} /* em_gmm_grlc_event_req () */

GLOBAL void em_gmm_data_ind ( T_EM_DATA_IND *em_data_ind)
{ 
  GMM_TRACE_FUNCTION( "em_gmm_data_ind" );
  /* NO PFREE */
  PSEND ( hCommMMI, em_data_ind);

} /* em_gmm_data_ind () */

/*
+------------------------------------------------------------------------------
| Function    : em_gmm_grlc_info_cnf
+------------------------------------------------------------------------------
| Description : The function em_gmm_grlc_info_cnf() is used to forward a
|               confirm from GRLC to ACI.
|
| Parameters  : EM_GRLC_INFO_CNF
|
+------------------------------------------------------------------------------
*/
GLOBAL void em_gmm_grlc_info_cnf ( T_EM_GRLC_INFO_CNF *em_grlc_info_cnf)
{ 
  GMM_TRACE_FUNCTION( "em_gmm_grlc_info_cnf" );
  /* NO PFREE */
  PSEND ( hCommMMI, em_grlc_info_cnf );

} /* em_gmm_grlc_info_cnf () */



/*----------------- GMM - Data - Function ---------------*/
/*
+------------------------------------------------------------------------------
| Function    : em_gmm_pco_trace_req
+------------------------------------------------------------------------------
| Description : The function em_gmm_pco_trace_req() is used to trace EM - GMM
|               relevant data at the PCO. If necessary the Request is forwarded
|               to GRR or/and GRLC.
|
| Parameters  : EM_PCO_TRACE_REQ
|
+------------------------------------------------------------------------------
*/
GLOBAL void em_gmm_pco_trace_req ( T_EM_PCO_TRACE_REQ    *em_pco_trace_req)
{
  GMM_TRACE_FUNCTION( "em_gmm_pco_trace_req" );

  /*check if GMM data is requested*/
  if(em_pco_trace_req->pco_bitmap & EM_PCO_GMM_INFO)
  {
      TRACE_EVENT_EM_P8("EM_GMM_INFO_REQ: ready_st:%d tlli:%u ptmsi:%u ptmsi_sig:%u ready_tim:%u ciph_alg:%d periodic_tim:%u ciph_on:%d",
      GET_STATE(RDY),
      gmm_data->tlli.current,
      gmm_data->ptmsi.current,
      gmm_data->ptmsi_signature.value,
      gmm_data->rdy.t3314_val,
      gmm_data->kern.auth_cap.ciphering_algorithm,
      gmm_data->rdy.t3312_val,
      gmm_data->cipher);


      TRACE_EVENT_EM_P2("EM_GMM_INFO_REQ: t3312_deactive:%u t3312_val_sec:%d",
      gmm_data->rdy.t3312_deactivated,
      gmm_data->rdy.t3312_val/SEC);
  }

  /*check if a forward to GRR is necessary*/
  if(em_pco_trace_req->pco_bitmap & EM_PCO_GPRS_INFO)
  {
    /*copy the request and send it to GRR*/
    PALLOC (em_pco_trace_req_grr, EM_PCO_TRACE_REQ);
    memcpy (em_pco_trace_req_grr, em_pco_trace_req, sizeof (T_EM_PCO_TRACE_REQ));

    /* NO PFREE */
    PSEND ( hCommGRR, em_pco_trace_req_grr );
  }

  /*check if a forward to GRLC is necessary*/
  if(em_pco_trace_req->pco_bitmap & EM_PCO_GRLC_INFO)
  {
    /*send original request, because it is no longer needed in GMM*/
    /* NO PFREE */
    PSEND ( hCommGRLC, em_pco_trace_req); 
  }
  else
  {
    PFREE(em_pco_trace_req);
  }

}/*em_gmm_pco_trace_req*/

/*
+------------------------------------------------------------------------------
| Function    : em_gmm_info_req
+------------------------------------------------------------------------------
| Description : The function em_gmm_info_req() is used to provide EM - GMM
|               relevant data and send it back to ACI.
|
| Parameters  : EM_GMM_INFO_REQ
|
+------------------------------------------------------------------------------
*/
GLOBAL void em_gmm_info_req ( T_EM_GMM_INFO_REQ *em_gmm_info_req)
{ 
  PALLOC (em_gmm_info_cnf, EM_GMM_INFO_CNF);

  GMM_TRACE_FUNCTION( "em_gmm_info_req" );
  PFREE(em_gmm_info_req);
 
  memset (em_gmm_info_cnf, 0, sizeof (T_EM_GMM_INFO_CNF));

  em_gmm_info_cnf->ready_state    =  GET_STATE(RDY);
  em_gmm_info_cnf->tlli           =  gmm_data->tlli.current;
  em_gmm_info_cnf->ptmsi          =  gmm_data->ptmsi.current;
  em_gmm_info_cnf->ptmsi_sig      =  gmm_data->ptmsi_signature.value;
  em_gmm_info_cnf->ready_timer    =  gmm_data->rdy.t3314_val;
  em_gmm_info_cnf->ciphering_algorithm  = gmm_data->kern.auth_cap.ciphering_algorithm;
  em_gmm_info_cnf->t3312.t3312_deactivated = gmm_data->rdy.t3312_deactivated;
  em_gmm_info_cnf->t3312.t3312_val = gmm_data->rdy.t3312_val/SEC;




    /* NO PFREE */
  PSEND ( hCommMMI, em_gmm_info_cnf);

} /* em_gmm_info_req () */

/*
+------------------------------------------------------------------------------
| Function    : em_gmm_map_state
+------------------------------------------------------------------------------
| Description : The function em_gmm_map_state maps the GMM state used within the 
                GMM entity to state and substate values required by the EM entity
|       
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void em_gmm_map_state ( UBYTE state, UBYTE *main_state, UBYTE *sub_state)
{
  switch(state)
  {
    case KERN_GMM_NULL_NO_IMSI:
    case KERN_GMM_NULL_IMSI:
    case KERN_GMM_NULL_NO_IMSI_LIMITED_SERVICE_REQ:
    case KERN_GMM_NULL_IMSI_LIMITED_SERVICE_REQ:
    case KERN_GMM_NULL_PLMN_SEARCH:
      *main_state = GMM_EM_NULL_STATE;
      *sub_state  = GMM_EM_POWER_OFF_SUBSTATE;
      break;

    case KERN_GMM_DEREG_INITIATED:
      *main_state = GMM_EM_DEREG_INIT_STATE;
      *sub_state  = GMM_EM_NORMAL_SRVC_SUBSTATE;
      break;

    case KERN_GMM_DEREG_ATTEMPTING_TO_ATTACH:   
      *main_state = GMM_EM_DEREG_STATE;
      *sub_state  = GMM_EM_ATTEMPT_TO_ATT_SUBSTATE;
      break;

    case KERN_GMM_DEREG_NO_CELL_AVAILABLE:
      *main_state = GMM_EM_DEREG_STATE;
      *sub_state  = GMM_EM_NO_CELL_SUBSTATE;
      break;
  
    case KERN_GMM_DEREG_LIMITED_SERVICE:
    case KERN_GMM_DEREG_NO_IMSI:
    case KERN_GMM_DEREG_PLMN_SEARCH:
      *main_state = GMM_EM_DEREG_STATE;
      *sub_state  = GMM_EM_ATT_NEEDED_SUBSTATE;
      break;

    case KERN_GMM_DEREG_SUSPENDED:
    case KERN_GMM_DEREG_SUSPENDING:
    case KERN_GMM_DEREG_RESUMING:
      *main_state = GMM_EM_DEREG_STATE;
      *sub_state  = GMM_EM_SUSPENDED_SUBSTATE;
      break;

    case KERN_GMM_REG_INITIATED:
      *main_state = GMM_EM_REG_INIT_STATE;
      *sub_state  = GMM_EM_NORMAL_SRVC_SUBSTATE;
      break;

    case KERN_GMM_REG_NO_CELL_AVAILABLE:
      *main_state = GMM_EM_REG_STATE;
      *sub_state  = GMM_EM_NO_CELL_SUBSTATE;
      break;

    case KERN_GMM_REG_LIMITED_SERVICE:
      *main_state = GMM_EM_REG_STATE;
      *sub_state  = GMM_EM_UPDATE_NEEDED_SUBSTATE;
      break;

    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE_MM:
      *main_state = GMM_EM_REG_STATE;
      *sub_state  = GMM_EM_ATTEMPT_TO_UPDATE_MM_SUBSTATE;
      break;

    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE:
      *main_state = GMM_EM_REG_STATE;
      *sub_state  = GMM_EM_ATTEMPT_TO_UPDATE_SUBSTATE;
      break;

    case KERN_GMM_REG_RESUMING:
    case KERN_GMM_REG_SUSPENDED:
    case KERN_GMM_REG_SUSPENDING:
      *main_state = GMM_EM_REG_STATE;
      *sub_state  = GMM_EM_SUSPENDED_SUBSTATE;
      break;

    case KERN_GMM_REG_NORMAL_SERVICE:
    case KERN_GMM_REG_TEST_MODE:
      *main_state = GMM_EM_REG_STATE;
      *sub_state  = GMM_EM_NORMAL_SRVC_SUBSTATE;
      break;

    case KERN_GMM_RAU_INITIATED:
    case KERN_GMM_RAU_WAIT_FOR_NPDU_LIST:
      *main_state = GMM_EM_RAU_INIT_STATE;
      *sub_state  = GMM_EM_NORMAL_SRVC_SUBSTATE;
      break;

    case KERN_GMM_REG_IMSI_DETACH_INITIATED:
      *main_state = GMM_EM_REG_STATE;
      *sub_state  = GMM_EM_IMSI_DETACH_INIT_SUBSTATE;
      break;
  
    case KERN_GMM_REG_TEST_MODE_NO_IMSI:           
      *main_state = GMM_EM_REG_STATE;
      *sub_state  = GMM_EM_POWER_OFF_SUBSTATE;
      break;

    default:
      TRACE_ERROR ("Unknown state passed to function");
      break;
  }
  return;
}


#endif /* FF_EM_MODE */

#endif /*GMM_EM_C */

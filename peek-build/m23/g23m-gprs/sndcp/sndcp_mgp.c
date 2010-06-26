/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  sndcp_mgp.c
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
|  Purpose :  This modul is part of the entity SNDCP and implements all 
|             functions to handles the incoming primitives as described in 
|             the SDL-documentation (MG-statemachine)
+----------------------------------------------------------------------------- 
*/ 



#define ENTITY_SNDCP

/*==== INCLUDES =============================================================*/

#include "typedefs.h"    /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gsm.h"        /* to get a lot of macros */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "dti.h"
#ifdef _SNDCP_DTI_2_
#include <string.h>
#endif /* _SNDCP_DTI_2_ */

#include "sndcp.h"        /* to get the global entity definitions */
#include "sndcp_f.h"       /* to get the functions to access the global arrays*/
#include "sndcp_mgf.h"     /* to get the local functions of service mg */
#include "sndcp_nuf.h"
#include "sndcp_nus.h"      /* to get the internal signals toservice nu */
#include "sndcp_nds.h"      /* to get the internal signals to service nd */
#include "sndcp_cias.h"      /* to get the internal signals to service cia */
#include "sndcp_sds.h"      /* to get the signals to sd service */
#include "sndcp_sus.h"      /* to get the internal signals to service su */
#include "sndcp_suas.h"      /* to get the internal signals to service sua */
#include "sndcp_sdas.h"     /* to get the signals to sda service */
#include "sndcp_sdf.h"
#include "sndcp_sdaf.h"

/*==== CONST ================================================================*/
#define TCPIP_NAME    "TCP"
/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/


/*
+------------------------------------------------------------------------------
| Function    : mg_est_cnf_est_pending
+------------------------------------------------------------------------------
| Description : After receiving an ll_establish_cnf, an snsm_activate_res
|               is sent to each waiting nsapi, sua and sda, sd are notified.
|
| Parameters  : ll_establish_cnf
|
+------------------------------------------------------------------------------
*/
LOCAL void mg_est_cnf_est_pending (T_LL_ESTABLISH_CNF* ll_establish_cnf) {
  UBYTE nsapi = 0;
  USHORT sapi_state = MG_IDLE;

  TRACE_FUNCTION( "mg_est_cnf_est_pending" );

  /*
   * If MG_REL_NEC_LOC/PEER is set for the affected sapi, there will be no
   * nsapi waiting for LL_ESTABLISH_CNF.
   * LL_RELEASE_REQ must be sent.
   */
  sndcp_get_sapi_state(ll_establish_cnf->sapi, &sapi_state);
  if ((sapi_state & MG_REL_NEC_LOC) > 0) {
    PALLOC(ll_release_req, LL_RELEASE_REQ);    

    ll_release_req->sapi = ll_establish_cnf->sapi;
    ll_release_req->local = TRUE;
    sndcp_unset_sapi_state(ll_release_req->sapi, MG_REL_NEC_LOC);
    /*
     * Set the "state" for the affected and sapi to MG_REL.
     */
    sndcp_set_sapi_state(ll_establish_cnf->sapi, MG_REL);
    sndcp_unset_sapi_state(ll_establish_cnf->sapi, MG_EST);

    PSEND(hCommLLC, ll_release_req);
    return;

  }

  if ((sapi_state & MG_REL_NEC_PEER) > 0) {
    PALLOC(ll_release_req, LL_RELEASE_REQ);    

    ll_release_req->sapi = ll_establish_cnf->sapi;
    ll_release_req->local = FALSE;
    sndcp_unset_sapi_state(ll_release_req->sapi, MG_REL_NEC_PEER);
    /*
     * Set the "state" for the affected and sapi to MG_REL.
     */
    sndcp_set_sapi_state(ll_establish_cnf->sapi, MG_REL);
    sndcp_unset_sapi_state(ll_establish_cnf->sapi, MG_EST);

    PSEND(hCommLLC, ll_release_req);

    return;
  }
  if ((sapi_state & MG_XID_NEC) > 0) {
    sndcp_unset_sapi_state(ll_establish_cnf->sapi, MG_EST);
    return;
  }


  /*
   * Send an SNSM_ACTIVATE_RES to all nsapis that are waiting for
   * an LL_ESTABLISH_CNF. 
   */
  for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi++) {

    USHORT nsapi_state = MG_IDLE;
    USHORT local_sapi_state = MG_IDLE;
    UBYTE sapi = 0;
    UBYTE sapi_index = 0;

    sndcp_get_nsapi_state(nsapi, &nsapi_state);
    sndcp_get_nsapi_sapi(nsapi, &sapi);
    sndcp_get_sapi_state(sapi, &local_sapi_state);
    sndcp_get_sapi_index(sapi, &sapi_index);
    if ((nsapi_state & MG_ACT) > 0 &&
        (local_sapi_state & MG_XID) == 0 &&
        sapi == ll_establish_cnf->sapi) {

      
#ifndef SNDCP_UPM_INCLUDED
        mg_dti_open(nsapi);
#else
        nu_ready_ind_if_nec(nsapi);
#endif
      
      mg_send_snsm_activate_res(nsapi);

      sndcp_unset_nsapi_state(nsapi, MG_ACT);

    }
    if ((nsapi_state & MG_ACT) > 0 &&
        (local_sapi_state & MG_XID) > 0 &&
        sapi == ll_establish_cnf->sapi) {

      /*
       * Open DTI connection.
       */
#ifndef SNDCP_UPM_INCLUDED
      mg_dti_open(nsapi);
#else
      nu_ready_ind_if_nec(nsapi);
#endif

      mg_send_snsm_activate_res(nsapi);

      /*
       * Set nsapi state to MG_IDLE.
       */
      sndcp_unset_nsapi_state(nsapi, MG_ACT);

    }
    if (((local_sapi_state & MG_EST) > 0) &&
        ((nsapi_state & MG_ACT) == 0) &&
        ((local_sapi_state & MG_XID) > 0)) {
      sig_mg_nu_reset_ack(nsapi, 0, 0, FALSE);
    }
     

  } /* for (nsapi... */
  
  /*
   * All nsapis at this sapi that use ack mode, enter recovery state.
   */
  for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi ++) {
    UBYTE sapi = 0;
    BOOL ack = FALSE;

    sndcp_get_nsapi_sapi(nsapi, &sapi);
    sndcp_get_nsapi_ack(nsapi, &ack);
    if (ack && sapi == ll_establish_cnf->sapi) {
      sig_mg_cia_delete_npdus(nsapi);
      sig_mg_sua_delete_pdus(nsapi, sapi, FALSE);
      sig_mg_nu_recover(nsapi);
      sig_mg_nd_recover(nsapi);
    }
  } /* for all nsapis */
 
  
  sndcp_unset_sapi_state(ll_establish_cnf->sapi, MG_XID);
  sndcp_unset_sapi_state(ll_establish_cnf->sapi, MG_EST);

  sndcp_set_sapi_ack(ll_establish_cnf->sapi, TRUE);
  /*
   * Service sda may now leave state SDA_ESTABLISH_REQUESTED.
   */
  sig_mg_sda_end_est(ll_establish_cnf->sapi, TRUE);
  /*
   * Services su and sua may now leave suspend state.
   */
  sig_mg_su_resume(ll_establish_cnf->sapi);
  sig_mg_sua_resume(ll_establish_cnf->sapi);
  mg_resume_affected_nus(ll_establish_cnf->sapi);

  /* 
   * If req xid has been changed during establishment, resend it.
   */
  mg_resend_xid_if_nec(ll_establish_cnf->sapi);


} /* mg_est_cnf_est_pending() */

/*
+------------------------------------------------------------------------------
| Function    : mg_check_unset_nsapi_flow_ctrl_flag
+------------------------------------------------------------------------------
| Description : This function checks if flow control is received before PDP 
|               activation for the nsapi by checking the bit in nsapi_rcv_rdy_b4_used flag
|               for the NSAPI
|               Depending on the mode calls the corresponding function to set the 
|               nsapi recv flag to true. 
|               In all cases unset the NSAPI bit in the nsapi_rcv_rdy_b4_used flag
|
| Parameters  : affected nsapi, mode
|
+------------------------------------------------------------------------------
*/
LOCAL void mg_check_unset_nsapi_flow_ctrl_flag (UBYTE nsapi, UBYTE mode) {

  /* check if flow control is  received before PDP activation */
  if ((sndcp_data->nsapi_rcv_rdy_b4_used & (0x001 << nsapi)) > 0)
  {    
    if (mode == SNDCP_ACK)
      sda_set_nsapi_rec(nsapi, TRUE);
    else if (mode == SNDCP_UNACK)
      sd_set_nsapi_rec(nsapi, TRUE);
    
    /* Set flow control flag for the NSAPI to FALSE */
    sndcp_data->nsapi_rcv_rdy_b4_used &= ~(0x001 << nsapi);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : mg_get_downlink_data
+------------------------------------------------------------------------------
| Description : If at the given sapi an nsapi is active and using 
|               unacknowledged LLC operation mode then sig_mg_sd_getunitdata
|               (sapi) is sent.
|               If at the given sapi an nsapi is active and using 
|               acknowledged LLC operation mode then sig_mg_sda_getdata(sapi)
|               is sent.
|
| Parameters  : affected sapi
|
+------------------------------------------------------------------------------
*/
LOCAL void mg_get_downlink_data (UBYTE sapi) {

  /*
   * Are unack or ack nsapis used?
   */
  UBYTE nsapi;

  TRACE_FUNCTION( "mg_get_downlink_data" );

  for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi ++) {
    BOOL used = FALSE;
    
    sndcp_is_nsapi_used(nsapi, &used);
    if (used) {
      UBYTE local_sapi = 0;

      sndcp_get_nsapi_sapi(nsapi, &local_sapi);
      if (sapi == local_sapi) {
        BOOL ack = FALSE;
        USHORT nsapi_state = MG_IDLE;
        USHORT sapi_state = MG_IDLE;

        sndcp_get_nsapi_state(nsapi, &nsapi_state);
        sndcp_get_sapi_state(local_sapi, &sapi_state);
        if (((sapi_state & MG_XID) == 0) &&
            ((sapi_state & MG_EST) == 0)) {
          sndcp_get_nsapi_ack(nsapi, &ack);
          if(ack){
            sig_mg_sda_getdata(sapi, nsapi);
          } else {
            sig_mg_sd_getunitdata(sapi, nsapi);
          }
        }
      }
    }
  } /* for all nsapis */
 
} /* mg_get_downlink_data() */




/*
+------------------------------------------------------------------------------
| Function    : mg_is_comp_available
+------------------------------------------------------------------------------
| Description : is another compressor available
|
| Parameters  : sapi, ack, kind, available
|
| Note: implementation dependent!!!
|
+------------------------------------------------------------------------------
*/
LOCAL void mg_is_comp_available (UBYTE sapi,
                                 BOOL ack, 
                                 UBYTE kind, 
                                 UBYTE msid, 
                                 BOOL* available) {

  UBYTE nsapi = 0;
  USHORT nsapi_set = 0;
  UBYTE sapi_index = 0;
  BOOL local_ack = FALSE;
  T_XID_BLOCK comp_xid_block;
  U16 sapi_state = MG_IDLE;

  TRACE_FUNCTION( "mg_is_comp_available" );

  sndcp_get_sapi_index(sapi, &sapi_index);

  /*
   * Implementation dependent!
   * Currently only implemented for VJ.
   * Is requested xid block for the given SAPI holding a compressor of the
   * given kind?
   */
  if (sndcp_data->vj_count < SNDCP_MAX_VJ_COUNT) {
    *available = TRUE;
    return;
  }

  /*
   * If an XID negotiation is ongoing we have to look at the compressors in
   * req_xid_block: is a compressor requested that will be used for the same
   * LLC operation mode?
   * If no negotiation is pending, we must look at the cur_xid_block of the
   * SAPI. 
   * Note: since we have checked before that vj_count has already reached
   * SNDCP_MAX_VJ_COUNT, ther must be an appropriate compressor on the
   * used SAPI, current or requested which may also be used for this nsapi. 
   * No new compressor is available.
   */

  sndcp_get_sapi_state(sapi, &sapi_state);

  if (((sapi_state & MG_EST) == 0) &&
      ((sapi_state & MG_XID) == 0)) {

    comp_xid_block = sndcp_data->mg.cur_xid_block[sapi_index];
  } else {
    comp_xid_block = sndcp_data->mg.req_xid_block[sapi_index];
  }


  nsapi_set = comp_xid_block.vj.nsapis;
  
  if (! comp_xid_block.vj.is_set ||
      (comp_xid_block.vj.is_set && nsapi_set == 0)) {
    *available = FALSE;
    return;
  }


  /*
   * If the LLC operation mode of the cur_xid_block is not equal to the
   * given LLC operation mode, no compressor is available.
   */
  for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi++) {
    if (((1 << nsapi) & nsapi_set) > 0) {
      break;
    }
  }
  if(nsapi < SNDCP_NUMBER_OF_NSAPIS){
    sndcp_get_nsapi_ack(nsapi, &local_ack);
    if ((local_ack && ack) ||
        (! local_ack && ack)) {
      *available = TRUE;
      return;
    }
  } else {
    /*
     * No nsapi yet using the compressor.
     */
    *available = TRUE;
    return;
  }
  *available = FALSE;
  return;

} /* mg_is_comp_available() */

/*
+------------------------------------------------------------------------------
| Function    : mg_ll_release_to_snsm_status
+------------------------------------------------------------------------------
| Description : Converts ll_release_ind->cause to snsm_status_req->cause
|
| Parameters  : ll_release_ind->error_cause
| Return      : snsm_status_req->cause
|
+------------------------------------------------------------------------------
*/
LOCAL U16 mg_ll_release_to_snsm_status (U16 ll_cause) {


  switch (ll_cause)
  {
#ifdef SNDCP_2to1
    return ll_cause;
#else /*SNDCP_2to1*/
#ifdef SNDCP_UPM_INCLUDED
    /* Here the cause values from LL sap is converted to
     * cause values from CAUSE include SAP. This is because
     * SM expects these cause values.
     */
    case LL_RELCS_INVALID_XID:
      return CAUSE_LLC_INVALID_XID;
    case LL_RELCS_DM_RECEIVED:
      return CAUSE_LLC_DM_RECEIVED;
    case LL_RELCS_NO_PEER_RES:
      return CAUSE_LLC_NO_PEER_RES;
    case LL_RELCS_NORMAL:
      return CAUSE_LLC_NORMAL_REL;
#endif /* SNDCP_UPM_INCLUDED */    
#endif /*SNDCP_2to1*/
    default:
      return ll_cause;
  }

} /* mg_ll_release_to_snsm_status() */

/*
+------------------------------------------------------------------------------
| Function    : mg_ll_status_to_snsm_status
+------------------------------------------------------------------------------
| Description : Converts ll_status_ind->error_cause to snsm_status_req->cause
|
| Parameters  : ll_status_ind->error_cause
| Return      : snsm_status_req->cause
|
+------------------------------------------------------------------------------
*/
LOCAL U16 mg_ll_status_to_snsm_status (U16 ll_error_cause) {

  switch (ll_error_cause)
  {
#ifdef SNDCP_2to1
    return ll_error_cause;
#else /*SNDCP_2to1*/
#ifdef SNDCP_UPM_INCLUDED
    /* Here the cause values from LL sap is converted to
     * cause values from CAUSE include SAP. This is because
     * SM expects these cause values.
     */
    case LL_ERRCS_INVALID_XID:
      return CAUSE_LLC_INVALID_XID;
    case LL_ERRCS_NO_PEER_RES:
      return CAUSE_LLC_NO_PEER_RES;
#endif /* SNDCP_UPM_INCLUDED */
#endif /* SNDCP_2to1 */
    default:
      return ll_error_cause;


  }
} /* mg_ll_status_to_snsm_status() */

/*
+------------------------------------------------------------------------------
| Function    : mg_mod_ack_unack
+------------------------------------------------------------------------------
| Description : An snsm_modify_ind has indicated that the operation mode of
|               the affected nsapi changed from acknowledged to unacknowledged.
|
| Parameters  : snsm_modify_ind
|
+------------------------------------------------------------------------------
*/
#ifdef SNDCP_UPM_INCLUDED
LOCAL void mg_mod_ack_unack (T_SN_MODIFY_REQ* snsm_modify_ind) {
#else
LOCAL void mg_mod_ack_unack (T_SNSM_MODIFY_IND* snsm_modify_ind) {
#endif /*#ifdef SNDCP_UPM_INCLUDED*/

  UBYTE nsapi = 0;
  BOOL alone = TRUE;

  TRACE_FUNCTION( "mg_mod_ack_unack" );

  mg_delete_npdus(snsm_modify_ind->nsapi);
  sig_mg_nu_reset(snsm_modify_ind->nsapi, TRUE);
  /*
   * LL_RELEASE_IND expected, if this was the only ack nsapi on the affected
   * sapi.
   */
  while (nsapi < SNDCP_NUMBER_OF_NSAPIS && alone) {

    UBYTE sapi = 0;
    BOOL ack = FALSE;

    sndcp_get_nsapi_sapi(nsapi, &sapi);
    sndcp_get_nsapi_ack(nsapi, &ack);
    alone = ! (nsapi != snsm_modify_ind->nsapi &&
               sapi == snsm_modify_ind->sapi &&
               ack);

    sndcp_set_nsapi_ack(snsm_modify_ind->nsapi, FALSE);         
    nsapi++;
  }
  if (alone) {
    sndcp_data->mg.mod_expects |= MG_MOD_X_REL;
  }

} /* mg_mod_ack_unack() */

/*
+------------------------------------------------------------------------------
| Function    : mg_mod_nsapi_new
+------------------------------------------------------------------------------
| Description : An snsm_modify_ind has indicated the creation of a new NSAPI.
|               Actions similar to snsm_activate_ind are taken, but the
|               establishment is never initiated by the MS. Everything
|               concerning the given nsapi is set to acknowlegded, but the
|               LLC establishment is expected to be done by the net.
|
| Parameters  : snsm_modify_ind
|
+------------------------------------------------------------------------------
*/
#ifdef SNDCP_UPM_INCLUDED
LOCAL void mg_mod_nsapi_new (T_SN_MODIFY_REQ* snsm_modify_ind) {
#else
LOCAL void mg_mod_nsapi_new (T_SNSM_MODIFY_IND* snsm_modify_ind) {
#endif /*#ifdef SNDCP_UPM_INCLUDED*/

  /*
   * LLC mode specified?
   */
  BOOL spec = FALSE;
  /*
   * LLC mode acknowledged?
   */
  BOOL b = FALSE;

  TRACE_FUNCTION( "mg_mod_nsapi_new" );

  sndcp_set_nsapi_used(snsm_modify_ind->nsapi, TRUE);
  sndcp_set_nsapi_qos(snsm_modify_ind->nsapi, 
                      snsm_modify_ind->snsm_qos);
  sndcp_set_nsapi_sapi(snsm_modify_ind->nsapi, 
                       snsm_modify_ind->sapi);
  sndcp_set_nsapi_prio(snsm_modify_ind->nsapi, 
                       snsm_modify_ind->radio_prio);
#ifdef REL99 
  sndcp_set_nsapi_pktflowid(snsm_modify_ind->nsapi,
                       snsm_modify_ind->pkt_flow_id);
#endif /*REL99*/

  mg_is_ack(snsm_modify_ind->snsm_qos, &spec, &b);
  if (spec) { 
    if (b) {

      BOOL ack = FALSE;

      /*
       * The NSAPI will use acknowledged LLC operation mode.
       */
      sndcp_set_nsapi_ack(snsm_modify_ind->nsapi, TRUE);
      /*
       * Is the affected SAPI already in ack mode?
       */
      sndcp_get_sapi_ack(snsm_modify_ind->sapi, &ack);
      /*
       * Reset service nu to acknowledged LLC mode.
       */
#ifdef SNDCP_UPM_INCLUDED
      sig_mg_nu_reset_ack(snsm_modify_ind->nsapi, 
                          0, /*snsm_modify_ind->send_no, FIXME !!*/
                          0, /*snsm_modify_ind->rec_no,  FIXME !!*/
                          TRUE);
#else
      sig_mg_nu_reset_ack(snsm_modify_ind->nsapi, 
                          snsm_modify_ind->send_no, 
                          snsm_modify_ind->rec_no,
                          TRUE);
#endif /*#ifdef SNDCP_UPM_INCLUDED*/
    }
  }
  /*
   * LL_ESTABLISH_IND expected.
   */
  sndcp_data->mg.mod_expects |= MG_MOD_X_EST;

} /* mg_mod_nsapi_new() */

/*
+------------------------------------------------------------------------------
| Function    : mg_mod_res_if_nec
+------------------------------------------------------------------------------
| Description : The service var 'mod_expects' informs about the measures to be
| taken before an SNSM_MODIFY_RES is sent.
| If it is set to MG_MOD_X_READY then one SNSM_MODIFY_RES will be sent to each 
| of the nsapis indicated in service var 'waiting_nsapis'. 
| Sets 'waiting_nsapis' to 0.
| Sets 'mod_expects' to MG_MOD_X_NONE.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
LOCAL void mg_mod_res_if_nec (void) {

  TRACE_FUNCTION( "mg_mod_res_if_nec" );
  
  if (sndcp_data->mg.mod_expects == MG_MOD_X_READY) {

    UBYTE nsapi = 0;

    for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi++) {
      if ((sndcp_data->mg.waiting_nsapis & (1 << nsapi)) > 0) {
      
#ifdef SNDCP_UPM_INCLUDED
         PALLOC (snsm_modify_res, SN_MODIFY_CNF);
#else
         PALLOC (snsm_modify_res, SNSM_MODIFY_RES);
#endif /*#ifdef SNDCP_UPM_INCLUDED*/
        snsm_modify_res->nsapi = nsapi;
#ifdef SNDCP_UPM_INCLUDED
        PSEND(hCommUPM, snsm_modify_res);
#else
        PSEND(hCommSM, snsm_modify_res);
#endif /*#ifdef SNDCP_UPM_INCLUDED*/
      }
    }
    sndcp_data->mg.waiting_nsapis = 0;
    sndcp_data->mg.mod_expects = MG_MOD_X_NONE;
  }
} /* mg_mod_res_if_nec() */

/*
+------------------------------------------------------------------------------
| Function    : mg_mod_sapi_diff
+------------------------------------------------------------------------------
| Description : An snsm_modify_ind has indicated that the new sapi for the 
|               affected context is different from the current one.
|
| Parameters  : snsm_modify_ind
|
+------------------------------------------------------------------------------
*/
#ifdef SNDCP_UPM_INCLUDED
LOCAL void mg_mod_sapi_diff (T_SN_MODIFY_REQ* snsm_modify_ind) {
#else
LOCAL void mg_mod_sapi_diff (T_SNSM_MODIFY_IND* snsm_modify_ind) {
#endif /*#ifdef SNDCP_UPM_INCLUDED*/

  BOOL old_sapi_ack = FALSE;
  BOOL alone = TRUE;
  BOOL new_context_ack = FALSE;
  BOOL new_context_ack_spec = FALSE;
  BOOL new_sapi_est = FALSE;
  UBYTE nsapi = 0;
  UBYTE old_sapi = 0;
  BOOL rec = FALSE;
  UBYTE old_sapi_index, new_sapi_index = 0;
  T_SN_UNITDATA_IND *temp_sn_unitdata = NULL;

  TRACE_FUNCTION( "mg_mod_sapi_diff" );

  /*
   * If old sapi was established and affected nsapi is only one connected
   * in ack mode: LL_RELEASE_IND expected.
   */
  sndcp_get_sapi_ack(snsm_modify_ind->sapi, &old_sapi_ack);
  while (nsapi < SNDCP_NUMBER_OF_NSAPIS && alone) {

    UBYTE sapi = 0;
    BOOL ack = FALSE;

    sndcp_get_nsapi_sapi(nsapi, &sapi);
    sndcp_get_nsapi_ack(nsapi, &ack);
    alone = ! (nsapi != snsm_modify_ind->nsapi &&
               sapi == snsm_modify_ind->sapi &&
               ack);

             
    nsapi++;
  }

  /*
   * If new qos is ack and new sapi is not yet established: expect 
   * LL_ESTABLISH_IND.
   */
  mg_is_ack(snsm_modify_ind->snsm_qos, 
            &new_context_ack_spec, 
            &new_context_ack);

  if (alone && !new_context_ack) {
    sndcp_data->mg.mod_expects |= MG_MOD_X_REL;
  }


  sndcp_get_sapi_ack(snsm_modify_ind->sapi, &new_sapi_est);
  if (! new_sapi_est) {
    sndcp_data->mg.mod_expects |= MG_MOD_X_EST;
  }

  /* Old & the New SAPI are UnAck Mode, send SNSM_MOD_CNF to SM and 
   * flowcontrol to LLC 
   */
  sndcp_get_nsapi_sapi(snsm_modify_ind->nsapi, &old_sapi);
  sndcp_get_sapi_ack(old_sapi, &old_sapi_ack);
  if(new_context_ack == old_sapi_ack && old_sapi_ack == FALSE) 
  {
    sndcp_get_sapi_index(old_sapi, &old_sapi_index);
    sndcp_data->sd = & sndcp_data->sd_base[old_sapi_index];
    sd_is_nsapi_rec(snsm_modify_ind->nsapi, &rec);

    /* Copy the Pointer, where Data on old Sapi was stored if any */
    if(sndcp_data->sd->cur_sn_unitdata_ind[snsm_modify_ind->nsapi] NEQ NULL)
    {      
      temp_sn_unitdata = 
             sndcp_data->sd->cur_sn_unitdata_ind[snsm_modify_ind->nsapi];
      sndcp_data->sd->cur_sn_unitdata_ind[snsm_modify_ind->nsapi] = NULL;
    }

    /* Set the old SAPI's NSAPI state to Non Receptive*/
    sd_set_nsapi_rec(snsm_modify_ind->nsapi, FALSE);

    /* Set the state of NSAPI associated with new SAPI same as Old SAPI */
    sndcp_get_sapi_index(snsm_modify_ind->sapi, &new_sapi_index);
    sndcp_data->sd = & sndcp_data->sd_base[new_sapi_index];
    sd_set_nsapi_rec(snsm_modify_ind->nsapi, rec);
    
    sndcp_data->mg.mod_expects = MG_MOD_X_READY;

    /* 
     * Taking care of DOWNLINK Data, when PDP is Modified 
     * from one sapi to another.
     */
    switch( sndcp_get_nsapi_rec_state(snsm_modify_ind->nsapi) )
    {
      case SD_UNACK_RECEIVE_SUBSEQUENT_SEGMENT:
        /* 
         *  If the Receiving State is SD_UNACK_RECEIVE_SUBSEQUENT_SEGMENT,
         *  this means that SNDCP has not receive the whole N-PDU, 
         *  it has received some SN-PDU's of the whole N-PDU.
         *  Hence we need to DELETE those SN-PDU's since PDP gets modified.
         */
        sig_mg_cia_delete_npdus(snsm_modify_ind->nsapi);
        sndcp_set_nsapi_rec_state(snsm_modify_ind->nsapi,
                                          SD_UNACK_RECEIVE_FIRST_SEGMENT);
        break;

       case SD_UNACK_WAIT_NSAPI:
        /* 
         *  If the Receiving State is SD_UNACK_WAIT_NSAPI, this means that
         *  SNDCP has received the whole N-PDU.
         *  Since SNDCP has not received the FLOW CONTROL from application,
         *  so it could not send the N-PDU to the application.
         *  In the meantime PDP gets modified.In this condition SNDCP needs 
         *  to copy the N-PDU from the old sapi to the modified sapi.
         */        
        if(temp_sn_unitdata NEQ NULL)
        {
          sndcp_data->sd->cur_sn_unitdata_ind[snsm_modify_ind->nsapi]
                                                        = temp_sn_unitdata;
        }
        break;

      default:
        /*
         * It is assumed that if the Receiving State is SD_UNACK_DISCARD
         * then SNDCP must have deleted all the SN-PDU,s that it might
         * have received on the old sapi. Hence we need not have to do 
         * anything here.
         */
        break;
    }

    /* Flowcontrol to LLC for new SAPI to send downlink data after 
     * PDP Modification
     */
    if(sndcp_data->sd->cur_sn_unitdata_ind[snsm_modify_ind->nsapi] EQ NULL)
    {
      sd_get_unitdata_if_nec(snsm_modify_ind->sapi);
    }
  }
  /*
   * Now switch to the new setting.
   */
  sndcp_set_nsapi_sapi(snsm_modify_ind->nsapi, snsm_modify_ind->sapi);

} /* mg_mod_sapi_diff() */

/*
+------------------------------------------------------------------------------
| Function    : mg_mod_unack_ack
+------------------------------------------------------------------------------
| Description : An snsm_modify_ind has indicated that the operation mode of
|               the affected nsapi changed from unacknowledged to acknowledged.
|
| Parameters  : snsm_modify_ind
|
+------------------------------------------------------------------------------
*/
#ifdef SNDCP_UPM_INCLUDED
LOCAL void mg_mod_unack_ack (T_SN_MODIFY_REQ* snsm_modify_ind) {
#else
LOCAL void mg_mod_unack_ack (T_SNSM_MODIFY_IND* snsm_modify_ind) {
#endif /*#ifdef SNDCP_UPM_INCLUDED*/

  BOOL ack = FALSE;

  TRACE_FUNCTION( "mg_mod_unack_ack" );

  mg_delete_npdus(snsm_modify_ind->nsapi);
  sndcp_set_nsapi_ack(snsm_modify_ind->nsapi, TRUE);
  sig_mg_nu_reset_ack(snsm_modify_ind->nsapi, 0, 0, TRUE);
  /*
   * LL_ESTABLISH_IND expected, if the sapi is not in established mode.
   */
  sndcp_get_sapi_ack(snsm_modify_ind->sapi, &ack);
  if (! ack) {
    sndcp_data->mg.mod_expects |= MG_MOD_X_EST;
  }
} /* mg_mod_unack_ack() */


/*
+------------------------------------------------------------------------------
| Function    : mg_start_re_est_timer
+------------------------------------------------------------------------------
| Description : According to GSM 4.65, 6.2.1.4, starts re-establishment timer
|
| Parameters  : affected sapi
|
+------------------------------------------------------------------------------
*/
LOCAL void mg_start_re_est_timer (UBYTE sapi) {
  UBYTE sapi_index = 0;

  TRACE_FUNCTION( "mg_start_re_est_timer" );

  sndcp_get_sapi_index(sapi, &sapi_index);
  vsi_t_start(SNDCP_handle, sapi_index, MG_RE_EST_TIME);

} /* mg_start_re_est_timer() */




/*==== PUBLIC FUNCTIONS =====================================================*/


/*
+------------------------------------------------------------------------------
| Function    : mg_exp_re_est_timer
+------------------------------------------------------------------------------
| Description : Re establishment timer has expired
|
| Parameters  : sapi_index
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_exp_re_est_timer (UBYTE sapi_index)
{ 
  TRACE_FUNCTION( "mg_exp_re_est_timer" );

  sndcp_data->su = & sndcp_data->su_base[sapi_index];
#ifdef SNDCP_UPM_INCLUDED
  mg_re_negotiate_ack(sndcp_data->su->sapi, 
                      CAUSE_SN_NO_PEER_RESPONSE);
#else
  mg_re_negotiate_ack(sndcp_data->su->sapi, 
                      LL_RELCS_NO_PEER_RES);
#endif /*#ifdef SNDCP_UPM_INCLUDED*/

} /* mg_exp_re_est_timer() */

/*
+------------------------------------------------------------------------------
| Function    : mg_ll_reset_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive LL_RESET_IND
|
| Parameters  : *ll_reset_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_ll_reset_ind ( T_LL_RESET_IND *ll_reset_ind )
{ 
  UBYTE nsapi = 0;
  UBYTE sapi_index = 0;
  BOOL  ack = FALSE;
  UBYTE sapi = 0;

  TRACE_FUNCTION( "mg_ll_reset_ind" );
  
  switch( GET_STATE( MG ) )
  {
    case MG_DEFAULT:
      for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS;nsapi++) {
        BOOL used = 0;

        sndcp_is_nsapi_used(nsapi, &used);
        if (! used) {
          continue;
        }
        sig_mg_nu_reset_ind(nsapi);
        sig_mg_nu_suspend(nsapi);

        sndcp_get_nsapi_sapi(nsapi, &sapi);
        sndcp_get_nsapi_ack(nsapi, &ack);

        if (ack) {
          sig_mg_sua_delete_pdus(nsapi, sapi, FALSE);
          sig_mg_sda_reset_ind(nsapi);
          sig_mg_nd_suspend(nsapi);
          sndcp_set_nsapi_state(nsapi, MG_SEQ);
          /* Set the flag as LL_ESTB_IND is expected 
           * before resend of buffered NPDU 
           */
          sndcp_set_sapi_state(sapi, MG_EST_IND);
        } else {
          sig_mg_su_delete_pdus(nsapi, sapi);
          sig_mg_sd_reset_ind(nsapi);
        }
        sig_mg_nd_reset_ind(nsapi);
      }
      sig_mg_cia_reset_ind();
      /*
       * Reset pending states and xid_block.
       */
      for (sapi_index = 0; sapi_index < SNDCP_NUMBER_OF_SAPIS; sapi_index++) {
       
        sapi = (sndcp_data->sua_base[sapi_index]).sapi;
        /*
         * Reset SUA and SU services
         */
        sig_mg_sua_n201(sapi, N201_I_DEFAULT);
        sig_mg_sua_reset_ind(sapi);
        sig_mg_su_n201(sapi, N201_U_DEFAULT);
        sig_mg_su_reset_ind(sapi);
        /*
         * cnf_xid_block and req_xid_block initialized.
         */
        sndcp_reset_xid_block(&sndcp_data->mg.cnf_xid_block[sapi_index]);
        sndcp_reset_xid_block(&sndcp_data->mg.req_xid_block[sapi_index]);

        mg_reset_states_n_rej(sapi_index);
        /*
         * Init renegotiation counter and cur_xid_block with default values.
         */
        sndcp_data->mg.renego[sapi_index] = 0;

        sndcp_reset_xid_block(&sndcp_data->mg.cur_xid_block[sapi_index]);
        sig_mg_cia_new_xid (&sndcp_data->mg.cur_xid_block[sapi_index]);
      }
      sndcp_data->mg.mod_expects = MG_MOD_X_NONE;
      sndcp_data->mg.waiting_nsapis = 0;

      break;
    default:
      TRACE_ERROR( "LL_RESET_IND unexpected" );
      break;
  }
  if (ll_reset_ind != NULL) {
    PFREE (ll_reset_ind);
  }
  
} /* mg_ll_reset_ind() */



/*
+------------------------------------------------------------------------------
| Function    : mg_ll_establish_cnf
+------------------------------------------------------------------------------
| Description : Handles the primitive LL_ESTABLISH_CNF
|
| Parameters  : *ll_establish_cnf - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_ll_establish_cnf ( T_LL_ESTABLISH_CNF *ll_establish_cnf )
{ 
  UBYTE sapi_index = 0;
  USHORT state = 0;

  TRACE_FUNCTION( "mg_ll_establish_cnf" );
  TRACE_EVENT_P1("ll_establish_cnf->n201_i: %d", ll_establish_cnf->n201_i);

  sndcp_get_sapi_index(ll_establish_cnf->sapi, &sapi_index); 
  switch( GET_STATE( MG ) )
  {
    case MG_DEFAULT:
      sndcp_get_sapi_state(ll_establish_cnf->sapi, &state);
      if ((state & MG_EST) > 0) {

        UBYTE dec_ret = 0;
        UBYTE check_ret = 0;

        /*
         * Reset all compressors at this SAPI in ack mode.
         */
        mg_reset_comp_ack(ll_establish_cnf->sapi);
        /*
         * Set N201 values in uplink services.
         */
        sig_mg_su_n201(ll_establish_cnf->sapi, ll_establish_cnf->n201_u);
        sig_mg_sua_n201(ll_establish_cnf->sapi, ll_establish_cnf->n201_i);

        /*
         * SDL label MG_EST_CNF_EST_XID_PENDING.
         */
        mg_decode_xid(&(ll_establish_cnf->sdu), 
                      &(sndcp_data->mg.cnf_xid_block[sapi_index]), 
                      &dec_ret,
                      ll_establish_cnf->sapi);
        if (dec_ret == MG_XID_OK) 
        {
          /*
           * SDL label MG_CHECK_CNF_XID_ACK
           */
          mg_check_cnf_xid(&check_ret, ll_establish_cnf->sapi);
          if (check_ret == MG_XID_OK) 
          {

            /*
             * This is now similar to the reaction to LL_XID_CNF:
             */
            /*
             * SDL Label MG_CNF_OK_ACK
             */
            mg_set_cur_xid_block(ll_establish_cnf->sapi);
            /*
             * Reset nsapis or ntts that were assigned, but are not any more.
             */
            mg_clean_xid(ll_establish_cnf->sapi);
            /*
             * Resume suspended data transfer on sapi will be done in 
             * mg_est_cnf_est_pending(ll_establish_cnf).
             */

            mg_est_cnf_est_pending(ll_establish_cnf);
            
            sndcp_get_sapi_state(ll_establish_cnf->sapi, &state);
            if (((state & MG_XID_NEC) > 0)
                &&
                ((state & MG_REL) == 0)) {
              mg_resend_xid_if_nec(ll_establish_cnf->sapi);
            }

          } else {
            #ifdef SNDCP_UPM_INCLUDED
              mg_re_negotiate_ack(ll_establish_cnf->sapi, 
                                  CAUSE_SN_INVALID_XID);
            #else
              mg_re_negotiate_ack(ll_establish_cnf->sapi, 
                                  SNSM_RELCS_INVALID_XID);
            #endif /*#ifdef SNDCP_UPM_INCLUDED*/
          }
        } else {
          #ifdef SNDCP_UPM_INCLUDED
            mg_re_negotiate_ack(ll_establish_cnf->sapi, 
                                CAUSE_SN_INVALID_XID);  
          #else
            mg_re_negotiate_ack(ll_establish_cnf->sapi, 
                                SNSM_RELCS_INVALID_XID);  
          #endif /*#ifdef SNDCP_UPM_INCLUDED*/
        }
        break;

      } else { /* if ((state & MG_EST) > 0)  */

        PALLOC(ll_release_req, LL_RELEASE_REQ);    

        sndcp_unset_sapi_state(ll_release_req->sapi, MG_REL_NEC_PEER);
        ll_release_req->sapi = ll_establish_cnf->sapi;
        ll_release_req->local = FALSE;
        /*
         * Set the "state" for the affected sapi to MG_REL.
         */
        sndcp_set_sapi_state(ll_establish_cnf->sapi, MG_REL);

        PSEND(hCommLLC, ll_release_req);
      }
      break;
    default:
      TRACE_ERROR( "LL_ESTABLISH_CNF unexpected" );
      break;
  }

  /*
   * Establish confirm has been computed. If no new XID is pending,
   * the req_xid_block may be reset.
   */
  sndcp_get_sapi_state(ll_establish_cnf->sapi, &state);
  if (((state & MG_XID) == 0) &&
      ((state & MG_EST) == 0)) {
    sndcp_reset_xid_block(&sndcp_data->mg.req_xid_block[sapi_index]);
  }

  if (ll_establish_cnf != NULL) {
    PFREE(ll_establish_cnf);
  }

} /* mg_ll_establish_cnf() */


/*
+------------------------------------------------------------------------------
| Function    : mg_ll_status_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive LL_STATUS_IND
|
| Parameters  : *ll_status_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_ll_status_ind ( T_LL_STATUS_IND *ll_status_ind )
{ 
  USHORT sapi_state = MG_IDLE;

  TRACE_FUNCTION( "mg_ll_status_ind" );
  
  switch( GET_STATE( MG ) )
  {
  case MG_DEFAULT:
      {
        #ifdef SNDCP_UPM_INCLUDED
         PALLOC(snsm_status_req, SN_STATUS_IND);
        #else
         PALLOC(snsm_status_req, SNSM_STATUS_REQ);
        #endif
        snsm_status_req->sapi = ll_status_ind->sapi;

#ifdef SNDCP_UPM_INCLUDED   
        snsm_status_req->ps_cause.ctrl_value = CAUSE_is_from_llc;
        snsm_status_req->ps_cause.value.llc_cause = 
#ifdef SNDCP_2to1
          mg_ll_status_to_snsm_status(ll_status_ind->ps_cause.value.llc_cause);
#else /*SNDCP_2to1*/
          mg_ll_status_to_snsm_status(ll_status_ind->error_cause);
#endif /*SNDCP_2to1*/

        PSEND(hCommUPM, snsm_status_req);

#else /*SNDCP_UPM_INCLUDED*/
        snsm_status_req->status_cause = 
        #ifdef _SNDCP_DTI_2_
          mg_ll_status_to_snsm_status(ll_status_ind->error_cause);
        #else /* _SNDCP_DTI_2_ */
          (U8)mg_ll_status_to_snsm_status(ll_status_ind->error_cause);
        #endif /* _SNDCP_DTI_2_ */

        PSEND(hCommSM, snsm_status_req);
#endif /*SNDCP_UPM_INCLUDED*/
        sndcp_get_sapi_state(ll_status_ind->sapi, &sapi_state);
        if ((sapi_state & MG_XID) != 0)
        {
          sig_mg_su_resume(ll_status_ind->sapi);
          sig_mg_sua_resume(ll_status_ind->sapi);
          mg_resume_affected_nus(ll_status_ind->sapi);
          sndcp_unset_sapi_state (ll_status_ind->sapi, MG_XID);
        }
        if ((sapi_state & MG_EST) != 0)
        {
#ifdef SNDCP_UPM_INCLUDED
          if (snsm_status_req->ps_cause.value.llc_cause == 
                                         CAUSE_LLC_NO_PEER_RES) {
#else /* SNDCP_UPM_INCLUDED */                                  
#ifdef _SNDCP_DTI_2_
          if (snsm_status_req->status_cause == LL_RELCS_NO_PEER_RES) {
#else /*_SNDCP_DTI_2_*/
          if (snsm_status_req->status_cause == SNSM_RELCS_NO_PEER_RES) {
#endif /* _SNDCP_DTI_2_*/
#endif /* SNDCP_UPM_INCLUDED */
            mg_start_re_est_timer(ll_status_ind->sapi);
          } else {
            sig_mg_su_resume(ll_status_ind->sapi);
            sig_mg_sua_resume(ll_status_ind->sapi);
            mg_resume_affected_nus(ll_status_ind->sapi);
            sndcp_unset_sapi_state (ll_status_ind->sapi, MG_EST);
          }
        }
      }
      break;
    default:
      TRACE_ERROR( "LL_STATUS_IND unexpected" );
      break;
  }

  if (ll_status_ind != NULL) {
    PFREE(ll_status_ind);
  }

} /* mg_ll_status_ind() */


/*
+------------------------------------------------------------------------------
| Function    : mg_ll_establish_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive LL_ESTABLISH_IND
|
| Parameters  : *ll_establish_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_ll_establish_ind ( T_LL_ESTABLISH_IND *ll_establish_ind )
{ 
  USHORT sapi_state = 0;

  TRACE_FUNCTION( "mg_ll_establish_ind" );
  
  switch( GET_STATE( MG ) )
  {
    case MG_DEFAULT:
      sndcp_unset_sapi_state(ll_establish_ind->sapi, MG_EST_IND);
      /*
       * Reset all compressors at this SAPI in ack mode.
       */
      mg_reset_comp_ack(ll_establish_ind->sapi);
      /*
       * If prim has been expected in course of modification, here it is.
       */
      sndcp_data->mg.mod_expects &= ~ MG_MOD_X_EST;
      /*
       * Do we have a collision?
       */
      sndcp_get_sapi_state(ll_establish_ind->sapi, &sapi_state);
      if ((sapi_state & MG_REL) > 0) {

        PFREE(ll_establish_ind);
        return;
      }


      if (((sapi_state & MG_XID) > 0) 
          ||
          ((sapi_state & MG_EST) > 0)) {
        /*
         * Is the establishment a re-establishment?
         */
        BOOL re = FALSE;

        sndcp_get_sapi_ack(ll_establish_ind->sapi, &re);
        if (re) {
          mg_col_re(ll_establish_ind);
        } else {
          mg_col_no_re(ll_establish_ind);
        }
      } else {
        /*
         * Is the establishment a re-establishment?
         */
        BOOL re = FALSE;

        sndcp_get_sapi_ack(ll_establish_ind->sapi, &re);
        if (re) {
          mg_no_col_re(ll_establish_ind);
        } else {
          mg_no_col_no_re(ll_establish_ind);
        }
      }
      /*
       * Now send snsm_modify_res, if expected.
       */
      mg_mod_res_if_nec();
      break;
    default:
      TRACE_ERROR( "LL_ESTABLISH_IND unexpected" );
      break;
  }
  if (ll_establish_ind != NULL) {
    PFREE(ll_establish_ind);
  }

} /* mg_ll_establish_ind() */



/*
+------------------------------------------------------------------------------
| Function    : mg_ll_release_cnf
+------------------------------------------------------------------------------
| Description : Handles the primitive LL_RELEASE_CNF
|
| Parameters  : *ll_release_cnf - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_ll_release_cnf ( T_LL_RELEASE_CNF *ll_release_cnf )
{ 
  UBYTE nsapi = 0;
  USHORT nsapi_state = MG_IDLE;
  UBYTE sapi = 0;
  USHORT sapi_state = MG_IDLE;

  TRACE_FUNCTION( "mg_ll_release_cnf" );
  
  switch( GET_STATE( MG ) )
  {
    case MG_DEFAULT:
      /*
       * Reset all compressors at this SAPI in ack mode.
       */
      mg_reset_comp_ack(ll_release_cnf->sapi);

      for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi++) {
        sndcp_get_nsapi_state(nsapi, &nsapi_state);
        sndcp_get_nsapi_sapi(nsapi, &sapi);
        sndcp_get_sapi_state(ll_release_cnf->sapi, &sapi_state);
        if (((sapi_state & MG_REL) > 0 )
            &&
            ((nsapi_state & MG_DEACT) > 0 )
            &&
            ((sapi_state & MG_XID) == 0 )) {
          /*
           * Release is now completed, negotiate XID or deactivate context.
           */
          if (sapi == ll_release_cnf->sapi) {
            BOOL nec = FALSE;

            mg_is_rel_comp_nec(nsapi, &nec);
            if (nec) {
              sndcp_set_sapi_state(sapi, MG_XID_NEC);
              sndcp_get_sapi_state(sapi, &sapi_state);
            } else {
            #ifdef SNDCP_UPM_INCLUDED
              PALLOC(snsm_deactivate_res, SN_DEACTIVATE_CNF);
              snsm_deactivate_res->nsapi = nsapi;
            #else
              PALLOC(snsm_deactivate_res, SNSM_DEACTIVATE_RES);
              snsm_deactivate_res->nsapi = nsapi;
            #endif /*#ifdef SNDCP_UPM_INCLUDED*/

              /*
               * Now the NSAPI is not in use anymore:
               */
              sndcp_set_nsapi_used(nsapi, FALSE);
              sndcp_set_nsapi_ack(nsapi, FALSE);
              /*
               * Set the "state" of the nsapi.
               */
              sndcp_unset_nsapi_state(nsapi, MG_DEACT);

#ifdef SNDCP_UPM_INCLUDED
              PSEND(hCommUPM, snsm_deactivate_res);
#else /*#ifdef SNDCP_UPM_INCLUDED*/
              PSEND(hCommSM, snsm_deactivate_res);
#endif /*#ifdef SNDCP_UPM_INCLUDED*/
            }
          }
        }
        if (((sapi_state & MG_REL) > 0 )
            &&
            ((nsapi_state & MG_DEACT) > 0 )
            &&
            ((sapi_state & MG_XID_NEC) > 0 )) {
          /*
           * Release is now completed, 
           * Remove nspi from compressor.
           */
          /*
           * Suspend uplink transfer for affected sapi.
           */
          sig_mg_su_suspend(sapi);
          sig_mg_sua_suspend(sapi);
          mg_suspend_affected_nus(sapi);
          /*
           * Negotiate possible deactivation of compressors.
           */
          mg_send_xid_req_del(sapi);
        }
      }
      sndcp_set_sapi_ack(ll_release_cnf->sapi, FALSE);
      sndcp_unset_sapi_state(ll_release_cnf->sapi, MG_REL);
      break;
    default:
      TRACE_ERROR( "LL_RELEASE_CNF unexpected" );
      break;
  }
  if (ll_release_cnf != NULL) {
    PFREE(ll_release_cnf);
  }

} /* mg_ll_release_cnf() */



/*
+------------------------------------------------------------------------------
| Function    : mg_ll_release_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive LL_RELEASE_IND
|
| Parameters  : *ll_release_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_ll_release_ind ( T_LL_RELEASE_IND *ll_release_ind )
{ 

  USHORT sapi_state = 0;
  U8 sapi_index = 0;

  TRACE_FUNCTION( "mg_ll_release_ind" );
  
  switch( GET_STATE( MG ) )
  {
    case MG_DEFAULT:

      sndcp_unset_sapi_state(ll_release_ind->sapi, 
                             MG_REL_NEC_LOC + MG_REL_NEC_PEER);
      sndcp_get_sapi_state(ll_release_ind->sapi, &sapi_state);

      /*
       * Service sda may now leave state SDA_ESTABLISH_REQUESTED.
       */
      sig_mg_sda_end_est(ll_release_ind->sapi, FALSE);

#ifdef SNDCP_2to1
      if (((sapi_state & MG_EST) > 0) &&
          ((ll_release_ind->ps_cause.ctrl_value == CAUSE_is_from_llc) &&
          (ll_release_ind->ps_cause.value.llc_cause == CAUSE_LLC_NO_PEER_RES)))
#else /*SNDCP_2to1*/
      if (((sapi_state & MG_EST) > 0) &&
          (ll_release_ind->cause == LL_RELCS_NO_PEER_RES)) 
#endif /*SNDCP_2to1*/
      {

        /*
         * SNDCP waits for LL_ESTABLISH_CNF and cause is "no peer response".
         * According to GSM 4.65, 6.2.1.4 a re-establishment is tried after an
         * implementation specific amount of time.
         */
        mg_start_re_est_timer(ll_release_ind->sapi);
        PFREE(ll_release_ind);
        return;
      }

      sndcp_unset_sapi_state(ll_release_ind->sapi, MG_EST);


      if ((sapi_state & MG_REL) > 0) {

        /*
         * SNDCP waits for LL_ESTABLISH_CNF and cause is "no peer response".
         * According to GSM 4.65, 6.2.1.4 a re-establishment is tried after an
         * implementation specific amount of time.
         */
        PALLOC (ll_release_cnf, LL_RELEASE_CNF);
        ll_release_cnf->sapi = ll_release_ind->sapi;
        mg_ll_release_cnf(ll_release_cnf);
        PFREE(ll_release_ind);
        return;
      }
      
      /*
       * Reset all compressors at this SAPI in ack mode.
       */
      mg_reset_comp_ack(ll_release_ind->sapi);
      /*
       * If release has been expected in course of modification, here it is.
       */
      if ((sndcp_data->mg.mod_expects & MG_MOD_X_REL) > 0) {
        sndcp_data->mg.mod_expects -= MG_MOD_X_REL;
      }
      mg_del_comp_pdus_ack(ll_release_ind->sapi);
      sig_mg_su_resume(ll_release_ind->sapi);
      sig_mg_sua_resume(ll_release_ind->sapi);
      mg_resume_affected_nus(ll_release_ind->sapi);
#ifdef SNDCP_2to1
      if (((ll_release_ind->ps_cause.ctrl_value == CAUSE_is_from_llc)&&
           (ll_release_ind->ps_cause.value.llc_cause == CAUSE_LLC_DM_RECEIVED)) ||
          ((ll_release_ind->ps_cause.ctrl_value == CAUSE_is_from_llc)&&
           (ll_release_ind->ps_cause.value.llc_cause == CAUSE_LLC_INVALID_XID)) ||
          ((ll_release_ind->ps_cause.ctrl_value == CAUSE_is_from_llc)&&
           (ll_release_ind->ps_cause.value.llc_cause == CAUSE_LLC_NO_PEER_RES))) {
#else /*SNDCP_2to1*/
      if (ll_release_ind->cause == LL_RELCS_DM_RECEIVED ||
          ll_release_ind->cause == LL_RELCS_INVALID_XID ||
          ll_release_ind->cause == LL_RELCS_NO_PEER_RES) {
#endif /*SNDCP_2to1*/

#ifdef SNDCP_UPM_INCLUDED
        PALLOC (snsm_status_req, SN_STATUS_IND);
#ifdef SNDCP_2to1
        if((ll_release_ind->ps_cause.ctrl_value == CAUSE_is_from_llc)&&
           (ll_release_ind->ps_cause.value.llc_cause == CAUSE_LLC_DM_RECEIVED)){
#else
        if (ll_release_ind->cause == LL_RELCS_DM_RECEIVED){
#endif
          UBYTE nsapi, sapi = 0;
          USHORT nsapi_set = 0;
          for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi++){
            sndcp_get_nsapi_sapi(nsapi, &sapi);
            if(sapi == ll_release_ind->sapi){
              BOOL ack = FALSE;
              sndcp_get_nsapi_ack(nsapi, &ack);
              if(ack){
                nsapi_set |= (1 << nsapi); 
              }
            }
          }
          snsm_status_req->nsapi_set = nsapi_set;
        }
        snsm_status_req->sapi = ll_release_ind->sapi;
        snsm_status_req->ps_cause.ctrl_value = CAUSE_is_from_llc;
        snsm_status_req->ps_cause.value.llc_cause = 
#ifdef SNDCP_2to1
        mg_ll_release_to_snsm_status(ll_release_ind->ps_cause.value.llc_cause);
#else /*SNDCP_2to1*/
          mg_ll_release_to_snsm_status(ll_release_ind->cause); 
#endif /*SNDCP_2to1*/
        PSEND(hCommUPM, snsm_status_req);
#else  /* SNDCP_UPM_INCLUDED */
        PALLOC (snsm_status_req, SNSM_STATUS_REQ);
        snsm_status_req->sapi = ll_release_ind->sapi;
        snsm_status_req->status_cause = 
        #ifdef _SNDCP_DTI_2_
          mg_ll_release_to_snsm_status(ll_release_ind->cause); 
        #else /* _SNDCP_DTI_2_ */
          (U8)mg_ll_release_to_snsm_status(ll_release_ind->cause);
        #endif /* _SNDCP_DTI_2_ */

        PSEND(hCommSM, snsm_status_req);
#endif /*#ifdef SNDCP_UPM_INCLUDED*/


      }
      sndcp_set_sapi_ack(ll_release_ind->sapi, FALSE);
     
      /*
       * Now send snsm_modify_res, if expected.
       */
      mg_mod_res_if_nec();
      break;
    default:
      TRACE_ERROR( "LL_RELEASE_IND unexpected" );
      break;
  }
  /*
   * Establish confirm has been computed. If no new XID is pending,
   * the req_xid_block may be reset.
   */
  sndcp_get_sapi_index(ll_release_ind->sapi, &sapi_index);
  sndcp_get_sapi_state(ll_release_ind->sapi, &sapi_state);
  if (((sapi_state & MG_XID) == 0) &&
      ((sapi_state & MG_EST) == 0)) {
    sndcp_reset_xid_block(&sndcp_data->mg.req_xid_block[sapi_index]);
  }

  if (ll_release_ind != NULL) {
    PFREE(ll_release_ind);
  }

} /* mg_ll_release_ind() */



/*
+------------------------------------------------------------------------------
| Function    : mg_ll_xid_cnf
+------------------------------------------------------------------------------
| Description : Handles the primitive LL_XID_CNF
|
| Parameters  : *ll_xid_cnf - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_ll_xid_cnf ( T_LL_XID_CNF* ll_xid_cnf )
{ 
  UBYTE dec_ret = 0;
  UBYTE check_ret = 0;
  UBYTE sapi_index = 0;

  TRACE_FUNCTION( "mg_ll_xid_cnf" );

  sndcp_get_sapi_index(ll_xid_cnf->sapi, &sapi_index);

#ifdef SNDCP_TRACE_ALL
  TRACE_EVENT("incoming xid block:");
  sndcp_trace_sdu(&ll_xid_cnf->sdu);
#endif

  switch( GET_STATE( MG ) )
  {
    case MG_DEFAULT:

      sndcp_unset_sapi_state(ll_xid_cnf->sapi, MG_XID);

      mg_decode_xid(&(ll_xid_cnf->sdu), 
                    &(sndcp_data->mg.cnf_xid_block[sapi_index]), 
                    &dec_ret,
                    ll_xid_cnf->sapi);
      if (dec_ret == MG_XID_OK) {
        mg_check_cnf_xid(&check_ret, ll_xid_cnf->sapi);
        if (check_ret == MG_XID_OK) {
          USHORT sapi_state = MG_IDLE;

          /*
           * SDL Label MG_CNF_OK
           */
          mg_set_cur_xid_block(ll_xid_cnf->sapi);
          mg_xid_cnf_ok_res(ll_xid_cnf->sapi);
          /*
           * Reset nsapis or ntts that were assigned, but are not any more.
           */
          mg_clean_xid(ll_xid_cnf->sapi),
          /*
           * Set n201.
           */
          sig_mg_su_n201 (ll_xid_cnf->sapi, ll_xid_cnf->n201_u);
          sig_mg_sua_n201 (ll_xid_cnf->sapi, ll_xid_cnf->n201_i);

          /*
           * Resume suspended data transfer on sapi.
           */

          sndcp_get_sapi_state(ll_xid_cnf->sapi, &sapi_state);
          if (((sapi_state & MG_XID) == 0)
              &&
              ((sapi_state & MG_EST) == 0)) {

            sig_mg_su_resume(ll_xid_cnf->sapi);
            sig_mg_sua_resume(ll_xid_cnf->sapi);
            mg_resume_affected_nus(ll_xid_cnf->sapi);
          }

          /*
           * Now get downlink data.
           */

          mg_get_downlink_data(ll_xid_cnf->sapi);

          /*
           * XID confirm has been computed. The negotiation is finished and
           * the req_xid_block may be reset.
           */
          sndcp_get_sapi_state(ll_xid_cnf->sapi, &sapi_state);
          if (((sapi_state & MG_XID) == 0) &&
              ((sapi_state & MG_EST) == 0)) {
            sndcp_reset_xid_block(&sndcp_data->mg.req_xid_block[sapi_index]);
          }
        } else {
          mg_re_negotiate(ll_xid_cnf->sapi);
        }
      } else {
        mg_re_negotiate(ll_xid_cnf->sapi);
      }
      break;
    default:
      TRACE_ERROR( "LL_XID_CNF unexpected" );
      break;
  }
  if (ll_xid_cnf != NULL) {
    PFREE(ll_xid_cnf);
    sndcp_reset_xid_block(&sndcp_data->mg.cnf_xid_block[sapi_index]);
  }

} /* mg_ll_xid_cnf() */



/*
+------------------------------------------------------------------------------
| Function    : mg_ll_xid_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive LL_XID_IND
|
| Parameters  : *ll_xid_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_ll_xid_ind ( T_LL_XID_IND *ll_xid_ind )
{ 
  UBYTE dec_ret = 0;
  UBYTE check_ret = 0;
  UBYTE sapi_index = 0;

  TRACE_FUNCTION( "mg_ll_xid_ind" );

  sndcp_get_sapi_index(ll_xid_ind->sapi, &sapi_index);
  
  switch( GET_STATE( MG ) )
  {
    case MG_DEFAULT:

      /*
       * Set N201 values in uplink services.
       */
      sig_mg_su_n201(ll_xid_ind->sapi, ll_xid_ind->n201_u);
      sig_mg_sua_n201(ll_xid_ind->sapi, ll_xid_ind->n201_i);

      TRACE_EVENT_P3("l3_valid: %d, N201_I: %d, N201_U: %d", 
                      ll_xid_ind->xid_valid, ll_xid_ind->n201_i, 
                      ll_xid_ind->n201_i);

      /*
       * If SNDCP XID block is not valid, we are ready.
       */
      if (ll_xid_ind->xid_valid == LL_XID_INVALID) {
        break;
      }
      /*
       * SNDCP XID block is valid and checked now.
       */
      mg_decode_xid(&(ll_xid_ind->sdu), 
                    &(sndcp_data->mg.ind_xid_block[sapi_index]), 
                    &dec_ret,
                    ll_xid_ind->sapi);
      if (dec_ret == MG_XID_OK) {
        mg_check_ind_xid(&check_ret, ll_xid_ind->sapi);
        if (check_ret == MG_XID_OK) {
          /*
           * Label MG_IND_OK
           */
          USHORT res_sdu_bit_len = 0;
          UBYTE ntt = 0;
          BOOL v42_rej = FALSE;
          BOOL vj_rej = FALSE;
          USHORT sapi_state = MG_IDLE;

          mg_set_res_cur_xid_block(ll_xid_ind->sapi, &res_sdu_bit_len);

          /*
           * Add the extra space for ntts with nsapis == 0.
           */
          for (ntt = 0; ntt < MG_MAX_ENTITIES; ntt++) {
            BOOL rej = FALSE;

            mg_get_sapi_dntt_rej(ll_xid_ind->sapi, ntt, &rej);
            if (rej) {
              /*
               * length of ntt octet and nsapis.
               */
              res_sdu_bit_len += 32;
              v42_rej = TRUE;
            }
            mg_get_sapi_pntt_rej(ll_xid_ind->sapi, ntt, &rej);
            if (rej) {
              /*
               * length of ntt octet and nsapis.
               */
              res_sdu_bit_len += 32;
              vj_rej = TRUE;
            }
          }
          if (! sndcp_data->mg.ind_xid_block[sapi_index].v42.is_set &&
              v42_rej) {
            /*
             * Add length of parameter type and length.
             */
            res_sdu_bit_len += 16;
          }
          if (! sndcp_data->mg.ind_xid_block[sapi_index].vj.is_set &&
              vj_rej) {
            /*
             * Add length of parameter type and length.
             */
            res_sdu_bit_len += 16;
          }

          {
            PALLOC_SDU (ll_xid_res, LL_XID_RES, res_sdu_bit_len);

            /*
             * Set sapi in ll_xid_res.
             */
            ll_xid_res->sapi = ll_xid_ind->sapi;
            /*
             * Write res_xid_block struct to sdu byte buffer. Implementation dep..
             */
            mg_set_res_xid_params(&ll_xid_res->sdu, ll_xid_ind->sapi);
            /*
             * Mark the affected nsapi and sapi as MG_XID_IDLE.
             */
            sndcp_unset_sapi_state(ll_xid_ind->sapi, MG_XID);
            sndcp_unset_sapi_state(ll_xid_ind->sapi, MG_EST);

            /*
             * Send the XID block to LLC.
             */
            
#ifdef SNDCP_RANGE_CHECK

            if(ll_xid_res EQ NULL)
            {
              TRACE_EVENT("ERROR: ll_xid_res is NULL");
            }
            else if(*((ULONG*)ll_xid_res - 7) NEQ 0)
            {
              TRACE_EVENT("ERROR in SNDCP: ll_xid_res is not allocated");
            }

#endif /* SNDCP_RANGE_CHECK */

            PSEND(hCommLLC, ll_xid_res);
          }

          /*
           * Reset nsapis or ntts that were assigned, but are not any more.
           */
          mg_clean_xid(ll_xid_ind->sapi);

          /*
           * If there was a collision and xid has not been negotiated 
           * sufficiently.
           */
          mg_resend_xid_if_nec(ll_xid_ind->sapi);
          /*
           * If activation or deactivation is pending, it will be responded if
           * no xid_req is pending.
           */
          sndcp_get_sapi_state(ll_xid_ind->sapi, &sapi_state);
          if ((sapi_state & MG_XID) == 0) {
            mg_xid_cnf_ok_res(ll_xid_ind->sapi);
          }


        } else {
          /* 
           * not (check_ret == MG_IND_XID_OK) 
           */
          /*
           * Label MG_CHECK_FAIL
           */
          USHORT res_sdu_bit_len = 0;
          UBYTE ntt = 0;
          BOOL v42_rej = FALSE;
          BOOL vj_rej = FALSE;

          /*
           * Add the extra space for ntts with nsapis == 0.
           */
          for (ntt = 0; ntt < MG_MAX_ENTITIES; ntt++) {
            BOOL rej = FALSE;

            mg_get_sapi_dntt_rej(ll_xid_ind->sapi, ntt, &rej);
            if (rej) {
              /*
               * length of ntt octet and nsapis.
               */
              res_sdu_bit_len += 32;
              v42_rej = TRUE;
            }
            mg_get_sapi_pntt_rej(ll_xid_ind->sapi, ntt, &rej);
            if (rej) {
              /*
               * length of ntt octet and nsapis.
               */
              res_sdu_bit_len += 32;
              vj_rej = TRUE;
            }
          }
          if (v42_rej) {
            /*
             * Add length of parameter type and length.
             */
            res_sdu_bit_len += 16;
          }
          if (vj_rej) {
            /*
             * Add length of parameter type and length.
             */
            res_sdu_bit_len += 16;
          }
          /*
           * Allocate response and send it.
           */
          {
            PALLOC_SDU (ll_xid_res, LL_XID_RES, res_sdu_bit_len);

            /*
             * Reset res_xid_block, ind_xid_block.
             */
            sndcp_reset_xid_block(&sndcp_data->mg.res_xid_block[sapi_index]);
            sndcp_reset_xid_block(&sndcp_data->mg.ind_xid_block[sapi_index]);
            /*
             * Set sapi in ll_xid_res.
             */
            ll_xid_res->sapi = ll_xid_ind->sapi;
            /*
             * Write res_xid_block struct to sdu byte buffer. Implementation dep..
             */
            mg_set_res_xid_params(&ll_xid_res->sdu, ll_xid_res->sapi);
            /*
             * Modify the affected sapi state.
             */
            sndcp_unset_sapi_state(ll_xid_ind->sapi, MG_XID);
            sndcp_unset_sapi_state(ll_xid_ind->sapi, MG_EST);

            /*
             * Send the XID block to LLC.
             */

#ifdef SNDCP_RANGE_CHECK

            if(ll_xid_res EQ NULL)
            {
              TRACE_EVENT("ERROR: ll_xid_res is NULL");
            }
            else if(*((ULONG*)ll_xid_res - 7) NEQ 0)
            {
              TRACE_EVENT("ERROR in SNDCP: ll_xid_res is not allocated");
            }

#endif /* SNDCP_RANGE_CHECK */

            PSEND(hCommLLC, ll_xid_res);
          }
          /*
           * If there was a collision and xid has not been negotiated 
           * sufficiently.
           */
          mg_resend_xid_if_nec(ll_xid_ind->sapi);

          /*
           * Reset nsapis or ntts that were assigned before 
           * but are not anymore.
           */ 
          mg_clean_xid(ll_xid_ind->sapi);
          /*
           * Allocate status req and send it  (label MG_SEND_STATUS_REQ).
           */
          {
#ifdef SNDCP_UPM_INCLUDED
            PALLOC (snsm_status_req, SN_STATUS_IND);
            snsm_status_req->sapi = ll_xid_ind->sapi;
            snsm_status_req->ps_cause.ctrl_value = CAUSE_is_from_sndcp;
            snsm_status_req->ps_cause.value.sn_cause = CAUSE_SN_INVALID_XID;
            PSEND(hCommUPM, snsm_status_req);
#else
            PALLOC (snsm_status_req, SNSM_STATUS_REQ);
            snsm_status_req->sapi = ll_xid_ind->sapi;
            snsm_status_req->status_cause = SNSM_RELCS_INVALID_XID;
            PSEND(hCommSM, snsm_status_req);
#endif /*#ifdef SNDCP_UPM_INCLUDED*/

          }
        } 
      } else { /* not if (dec_ret == MG_XID_OK) */
        /*
         * Reset nsapis or ntts that were assigned before 
         * but are not anymore.
         */ 
        sndcp_reset_xid_block(&sndcp_data->mg.cur_xid_block[sapi_index]);
        mg_clean_xid(ll_xid_ind->sapi);
        /*
         * Decoding of LL_XID_IND failed (label MG_SEND_STATUS_REQ).
         * Allocate status req and send it.
         */
        {
         #ifdef SNDCP_UPM_INCLUDED
          PALLOC (snsm_status_req, SN_STATUS_IND);
          snsm_status_req->sapi = ll_xid_ind->sapi;
          snsm_status_req->ps_cause.ctrl_value = CAUSE_is_from_sndcp;
          snsm_status_req->ps_cause.value.sn_cause = CAUSE_SN_INVALID_XID;
          PSEND(hCommUPM, snsm_status_req);
        #else
          PALLOC (snsm_status_req, SNSM_STATUS_REQ);
          snsm_status_req->sapi = ll_xid_ind->sapi;
          snsm_status_req->status_cause = SNSM_RELCS_INVALID_XID;
          PSEND(hCommSM, snsm_status_req);
        #endif /*#ifdef SNDCP_UPM_INCLUDED*/

        }

      }
      break;
    default:
      TRACE_ERROR( "LL_XID_IND unexpected" );
      break;
  }
  if (ll_xid_ind != NULL) {
    sndcp_reset_xid_block(&sndcp_data->mg.ind_xid_block[sapi_index]);
    PFREE(ll_xid_ind);
  }
} /* mg_ll_xid_ind() */



/*
+------------------------------------------------------------------------------
| Function    : mg_snsm_activate_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive SNSM_ACTIVATE_IND
|
| Parameters  : *snsm_activate_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
#ifdef SNDCP_UPM_INCLUDED
GLOBAL void mg_snsm_activate_ind ( T_SN_ACTIVATE_REQ *snsm_activate_ind )
#else
GLOBAL void mg_snsm_activate_ind ( T_SNSM_ACTIVATE_IND *snsm_activate_ind )
#endif /*#ifdef SNDCP_UPM_INCLUDED*/
{ 
  /*
   * is nsapi already used?
   */
  BOOL used = FALSE;
  /*
   * is LLC mode specified (TRUE) or should the subscribed one be used 
   * (FALSE)
   */
  BOOL spec = FALSE;
  /*
   * should the context use acknowledged LLC operation mode?
   */
  BOOL b = FALSE;

  TRACE_FUNCTION( "mg_snsm_activate_ind" );

#ifndef NCONFIG
  if (sndcp_data->millis > 0) {
    vsi_t_sleep(VSI_CALLER sndcp_data->millis); 
  }
#endif /* NCONFIG */
  
  switch( GET_STATE( MG ) )
  {
    case MG_DEFAULT:
      sndcp_is_nsapi_used(snsm_activate_ind->nsapi, &used);
      if (used == FALSE) {

        USHORT sapi_state = MG_IDLE;

        sndcp_set_nsapi_state(snsm_activate_ind->nsapi, MG_ACT);
        sndcp_reset_nd_nsapi_npdu_num(snsm_activate_ind->nsapi);
        sndcp_set_nsapi_used(snsm_activate_ind->nsapi, TRUE);
        sndcp_set_nsapi_qos(snsm_activate_ind->nsapi, 
                            snsm_activate_ind->snsm_qos);
        sndcp_set_nsapi_sapi(snsm_activate_ind->nsapi, 
                             snsm_activate_ind->sapi);
        sndcp_set_nsapi_prio(snsm_activate_ind->nsapi, 
                             snsm_activate_ind->radio_prio);
#ifdef REL99  
        sndcp_set_nsapi_pktflowid(snsm_activate_ind->nsapi, 
                                  snsm_activate_ind->pkt_flow_id);
#endif /*REL 99*/

#if 0
#ifdef _SNDCP_DTI_2_
#ifndef SNDCP_UPM_INCLUDED 
        if (snsm_activate_ind->dti_direction == DTI_CHANNEL_TO_LOWER_LAYER) {
          /* new constant instead of SNSM HOME , replaced by DTI_CHANNEL_TO_LOWER_LAYER */
          sndcp_set_nsapi_direction(snsm_activate_ind->nsapi, DTI_CHANNEL_TO_LOWER_LAYER);
        } else {
          sndcp_set_nsapi_direction(snsm_activate_ind->nsapi, DTI_CHANNEL_TO_HIGHER_LAYER);
        }
      
        sndcp_set_nsapi_linkid(snsm_activate_ind->nsapi, 
                               snsm_activate_ind->dti_linkid);

        sndcp_set_nsapi_neighbor(snsm_activate_ind->nsapi, 
#ifdef _SIMULATION_
                                 (snsm_activate_ind->dti_neighbor == 
                                    0xabcdabcd 
                                  ?
                                  "PPP" 
                                  :
                                  "NULL")
#else /* _SIMULATION_ */ 
                                 (U8*)snsm_activate_ind->dti_neighbor
#endif /* _SIMULATION_ */                            
                                 );
#endif /*#ifdef SNDCP_UPM_INCLUDED*/

#else /*_SNDCP_DTI_2_*/
        if (snsm_activate_ind->dti_direction == SNSM_HOME) {
          /* new constant instead of HOME, but the comiler switch! */
          sndcp_set_nsapi_direction(snsm_activate_ind->nsapi, HOME);
        } else {
          sndcp_set_nsapi_direction(snsm_activate_ind->nsapi, NEIGHBOR);
        }
        sndcp_set_nsapi_linkid(snsm_activate_ind->nsapi, 
                               snsm_activate_ind->dti_linkid);
        sndcp_set_nsapi_neighbor(snsm_activate_ind->nsapi, 
#ifdef _SIMULATION_
                                 (snsm_activate_ind->dti_neighbor == 
                                    0xabcdabcd 
                                  ?
                                  "PPP" 
                                  :
                                  "NULL")
#else
                                 (U8*)snsm_activate_ind->dti_neighbor
#endif /* _SIMULATION_ */                            
                                 ); 
#endif /*_SNDCP_DTI_2_*/
#endif
        mg_is_ack(snsm_activate_ind->snsm_qos, &spec, &b);
        if (spec) { 
          if (b) {
            /*
             * The NSAPI will use acknowledged LLC operation mode.
             */

            BOOL ack = FALSE;
            USHORT local_sapi_state = MG_IDLE;
            BOOL comp_available = FALSE;

            /*
             * Is another compressor available?
             */
          #ifdef SNDCP_UPM_INCLUDED  
            mg_is_comp_available(snsm_activate_ind->sapi, 
                                 TRUE,
                                 SNDCP_ACK,
                                 snsm_activate_ind->comp_params.msid,
                                 &comp_available);
          #else
            mg_is_comp_available(snsm_activate_ind->sapi, 
                                 TRUE,
                                 SNDCP_ACK,
                                 snsm_activate_ind->msid,
                                 &comp_available);
          #endif /*#ifdef SNDCP_UPM_INCLUDED*/

            sndcp_set_nsapi_ack(snsm_activate_ind->nsapi, TRUE);
            /*
             * Is the affected SAPI already in ack mode or est_pending??
             */
            sndcp_get_sapi_ack(snsm_activate_ind->sapi, &ack);
            sndcp_get_sapi_state(snsm_activate_ind->sapi, &local_sapi_state);

            if ((((local_sapi_state & MG_EST) > 0)
                  ||
                  ((local_sapi_state & MG_XID) > 0))
                 &&
                 comp_available) 
            {
              sndcp_set_sapi_state(snsm_activate_ind->sapi, MG_XID_NEC);
              mg_set_new_xid_block(snsm_activate_ind);

            } else if (ack) {

               /* check if flow control is  received before PDP activation */
              mg_check_unset_nsapi_flow_ctrl_flag (snsm_activate_ind->nsapi,SNDCP_ACK);
              /*
               * If XID negotiation is not requested, respond
               * activation to SM, else send LL_XID_REQ.
               */
          #ifdef SNDCP_UPM_INCLUDED 
              if ((snsm_activate_ind->comp_params.dcomp == NAS_DCOMP_OFF &&
                   snsm_activate_ind->comp_params.hcomp == NAS_HCOMP_OFF) ||
                  ! comp_available)               {
            #else
              if ((snsm_activate_ind->dcomp == SNSM_COMP_NEITHER_DIRECT &&
                   snsm_activate_ind->hcomp == SNSM_COMP_NEITHER_DIRECT) ||
                  ! comp_available)               {
            #endif /*#ifdef SNDCP_UPM_INCLUDED*/

                /*
                 * If est_pending, the res is sent after est_cnf.
                 */

                /*
                 * Reset service nu to acknowledged LLC mode.
                 */
                sig_mg_nu_reset_ack(snsm_activate_ind->nsapi, 0, 0, FALSE);
                /*
                 * Open DTI connection.
                 */
#ifndef SNDCP_UPM_INCLUDED
                mg_dti_open(snsm_activate_ind->nsapi);
#else
                nu_ready_ind_if_nec(snsm_activate_ind->nsapi);
#endif

                /*
                 * 
                 */
                sig_mg_sda_getdata(snsm_activate_ind->sapi, snsm_activate_ind->nsapi);
                /*
                 * Send response
                 */
                mg_send_snsm_activate_res(snsm_activate_ind->nsapi);

                /*
                 * unsetting the ACT-state
                 */
                sndcp_unset_nsapi_state(snsm_activate_ind->nsapi, MG_ACT);                

              } else { /* if XID negotiation necessary */
                USHORT punct_sapi_state = MG_IDLE;

                /*
                 * Increment number of requested compressors.
                 */
                sndcp_data->vj_count++;
                /*
                 * Set the N-PDU number in the affected nu instance to 0.
                 */
                sig_mg_nu_reset_ack(snsm_activate_ind->nsapi, 0, 0, TRUE);
                /*
                 * Uplink data transfer on SAPI is completely suspended.
                 */
                sig_mg_su_suspend(snsm_activate_ind->sapi);
                sig_mg_sua_suspend(snsm_activate_ind->sapi);
                mg_suspend_affected_nus(snsm_activate_ind->sapi);
                /*
                 * Send prim or store requested compression.
                 */
                sndcp_get_sapi_state(snsm_activate_ind->sapi, 
                                     &punct_sapi_state);
                mg_send_xid_req(snsm_activate_ind);
 
              } /* if XID negotiation necessary */
            } else {
              /*
               * Is XID negotiation necessary?
               */
          #ifdef SNDCP_UPM_INCLUDED  
              if ((snsm_activate_ind->comp_params.dcomp == NAS_DCOMP_OFF &&
                   snsm_activate_ind->comp_params.hcomp == NAS_HCOMP_OFF) ||
                   sndcp_data->vj_count == SNDCP_MAX_VJ_COUNT)
            #else
              if ((snsm_activate_ind->dcomp == SNSM_COMP_NEITHER_DIRECT &&
                   snsm_activate_ind->hcomp == SNSM_COMP_NEITHER_DIRECT) ||
                   sndcp_data->vj_count == SNDCP_MAX_VJ_COUNT)
            #endif /*#ifdef SNDCP_UPM_INCLUDED*/
              {
                /*
                 * Ack mode, sapi not yet established.
                 * No XID requested.
                 */
                PALLOC_SDU(ll_establish_req, LL_ESTABLISH_REQ, 0);

                /*
                 * Reset service nu to acknowledged LLC mode.
                 */
                sig_mg_nu_reset_ack(snsm_activate_ind->nsapi, 0, 0, TRUE);

                ll_establish_req->sapi = snsm_activate_ind->sapi;
                /*
                 * Set the establishment states of sapi
                 */
                sndcp_set_sapi_state(snsm_activate_ind->sapi, MG_EST);
                /*
                 * Uplink data transfer on SAPI is completely suspended.
                 */
                sig_mg_su_suspend(snsm_activate_ind->sapi);
                sig_mg_sua_suspend(snsm_activate_ind->sapi);
                mg_suspend_affected_nus(snsm_activate_ind->sapi);

                /*
                 * Send prim to LLC.
                 */
                sndcp_set_nsapi_rec_state(snsm_activate_ind->nsapi, SDA_ESTABLISH_REQUESTED);

#ifdef SNDCP_RANGE_CHECK

                if(ll_establish_req EQ NULL)
                {
                  TRACE_EVENT("ERROR: ll_establish_req is NULL");
                }
                else if(*((ULONG*)ll_establish_req - 7) NEQ 0)
                {
                  TRACE_EVENT
                    ("ERROR in SNDCP: ll_establish_req is not allocated");
                }

#endif /* SNDCP_RANGE_CHECK */

                PSEND(hCommLLC, ll_establish_req);
              } else {

                UBYTE sapi_index = 0;

                /*
                 * Ack mode, sapi not yet established.
                 * XID requested.
                 */
                PALLOC_SDU(ll_establish_req, 
                           LL_ESTABLISH_REQ, 
                           SNDCP_XID_BLOCK_BIT_LEN);

                /*
                 * Increment number of requested compressors.
                 */
                sndcp_data->vj_count++;

                sndcp_get_sapi_index(snsm_activate_ind->sapi, &sapi_index);
                /*
                 * Reset service nu to acknowledged LLC mode.
                 */
                sig_mg_nu_reset_ack(snsm_activate_ind->nsapi, 0, 0, TRUE);

                ll_establish_req->sapi = snsm_activate_ind->sapi;
                /*
                 * Write data from snsm_activate_ind to service variable req_xid_block.
                 */

                mg_set_req_xid_block(snsm_activate_ind);
                /*
                 * Fill the XID block. Implementation dependent!!!
                 */
                mg_set_xid_params(ll_establish_req->sapi,
                                  &ll_establish_req->sdu,
                                  sndcp_data->mg.req_xid_block[sapi_index]);

                /*
                 * Set the establishment states of sapi
                 */
                sndcp_set_sapi_state(snsm_activate_ind->sapi, MG_EST);
                /*
                 * Uplink data transfer on SAPI is completely suspended.
                 */
                sig_mg_su_suspend(snsm_activate_ind->sapi);
                sig_mg_sua_suspend(snsm_activate_ind->sapi);
                mg_suspend_affected_nus(snsm_activate_ind->sapi);

                /*
                 * Send prim to LLC.
                 */
                sndcp_set_nsapi_rec_state(snsm_activate_ind->nsapi, SDA_ESTABLISH_REQUESTED);

#ifdef SNDCP_RANGE_CHECK

                if(ll_establish_req EQ NULL)
                {
                  TRACE_EVENT("ERROR: ll_establish_req is NULL");
                }
                else if(*((ULONG*)ll_establish_req - 7) NEQ 0)
                {
                  TRACE_EVENT
                    ("ERROR in SNDCP: ll_establish_req is not allocated");
                }

#endif /* SNDCP_RANGE_CHECK */

                PSEND(hCommLLC, ll_establish_req);
              }
            }
          } else { 
            /*
             * The NSAPI will not use acknowledged LLC operation mode.
             */

            /*
             * Is XID negotiation necessary?
             */
             
             /* check if flow control is  received before PDP activation */
            mg_check_unset_nsapi_flow_ctrl_flag (snsm_activate_ind->nsapi,SNDCP_UNACK);


#ifdef TI_PS_FF_V42BIS
          #ifdef SNDCP_UPM_INCLUDED  
            if ((snsm_activate_ind->comp_params.hcomp == NAS_HCOMP_NEITHER_DIRECT
                 ||
                 sndcp_data->vj_count >= SNDCP_MAX_VJ_COUNT)
                &&
                snsm_activate_ind->comp_params.dcomp == NAS_HCOMP_NEITHER_DIRECT)
          #else
            if ((snsm_activate_ind->hcomp == SNSM_COMP_NEITHER_DIRECT 
                ||
                sndcp_data->vj_count >= SNDCP_MAX_VJ_COUNT) 
                &&
                snsm_activate_ind->dcomp == SNSM_COMP_NEITHER_DIRECT)

          #endif /*#ifdef SNDCP_UPM_INCLUDED*/
#else /* !TI_PS_FF_V42BIS */
         #ifdef SNDCP_UPM_INCLUDED  
            if (snsm_activate_ind->comp_params.hcomp == NAS_HCOMP_OFF
                ||
                sndcp_data->vj_count == SNDCP_MAX_VJ_COUNT) 
          #else
            if (snsm_activate_ind->hcomp == SNSM_COMP_NEITHER_DIRECT
                ||
                sndcp_data->vj_count == SNDCP_MAX_VJ_COUNT) 
          #endif /*#ifdef SNDCP_UPM_INCLUDED*/

#endif /* TI_PS_FF_V42BIS */
            {

              /*
               * No XID negotiation necessary.
               * BUT LLC needs trigger to send LLC XID request to network.
               * If LL_ESTABLISH_REQ or LL_XID_REQ is pending, no LL_XID_REQ
               * is needed here.
               * If LL_RELEASE_REQ is pending, XID must have been negotiated,
               * so it is not necessary.
               */
              USHORT local_sapi_state = MG_IDLE;
              sndcp_get_sapi_state(snsm_activate_ind->sapi,
                                   &local_sapi_state);
           
              if (sndcp_data->always_xid
                  &&
                  (local_sapi_state & MG_EST) == 0
                  &&
                  (local_sapi_state & MG_REL) == 0
                  &&
                  (local_sapi_state & MG_XID) == 0
                 ) {
             
                /*
                 * XID negotiation is not necessary, but done for LLC.
                 */
                /*
                 * Set the N-PDU number in the affected nu instance to 0.
                 */
                sig_mg_nu_reset(snsm_activate_ind->nsapi, TRUE);
                /*
                 * Uplink data transfer on SAPI is completely suspended.
                 */
                sig_mg_su_suspend(snsm_activate_ind->sapi);
                sig_mg_sua_suspend(snsm_activate_ind->sapi);
                mg_suspend_affected_nus(snsm_activate_ind->sapi);
                /*
                 * Send prim.
                 */
                mg_send_empty_xid_req(snsm_activate_ind);

              } else {
                /*
                 * No XID negotiation necessary.
                 */

                /*
                 * LLC may send data now
                 */
                sig_mg_sd_getunitdata(snsm_activate_ind->sapi, snsm_activate_ind->nsapi);
                /*
                 * Open DTI connection.
                 */
#ifndef SNDCP_UPM_INCLUDED
                mg_dti_open(snsm_activate_ind->nsapi);
#else
                nu_unitready_ind_if_nec(snsm_activate_ind->nsapi);
#endif
                /*
                 * Set the N-PDU number in the affected nu instance to 0.
                 */
                sig_mg_nu_reset(snsm_activate_ind->nsapi, FALSE);

                mg_send_snsm_activate_res(snsm_activate_ind->nsapi);
                /*
                 * The response is now sent, state flag can be unset.
                 */
                sndcp_unset_nsapi_state(snsm_activate_ind->nsapi, MG_ACT);

              }

            } else {
          #ifdef SNDCP_UPM_INCLUDED  
              if (snsm_activate_ind->comp_params.hcomp > NAS_HCOMP_OFF) {
             #else
                if (snsm_activate_ind->hcomp > SNSM_COMP_NEITHER_DIRECT) {
             #endif /*#ifdef SNDCP_UPM_INCLUDED*/
                /*
                 * Increment number of requested compressors.
                 */
                sndcp_data->vj_count++;
              }

              /*
               * XID negotiation is necessary.
               */
              /*
               * Set the N-PDU number in the affected nu instance to 0.
               */
              sndcp_get_sapi_state(snsm_activate_ind->sapi, &sapi_state);
              sig_mg_nu_reset(snsm_activate_ind->nsapi, TRUE);

              if (((sapi_state & MG_XID) == 0)
                  &&
                  ((sapi_state & MG_EST) == 0)) {

                  /*
                   * Uplink data transfer on SAPI is completely suspended.
                   */
                  sig_mg_su_suspend(snsm_activate_ind->sapi);
                  sig_mg_sua_suspend(snsm_activate_ind->sapi);
                  mg_suspend_affected_nus(snsm_activate_ind->sapi);
                /*
                 * Send prim.
                 */
                mg_send_xid_req(snsm_activate_ind);
              } else {
                mg_set_new_xid_block(snsm_activate_ind);
                sndcp_set_sapi_state(snsm_activate_ind->sapi, MG_XID_NEC);
              }

            } /* else from if negotiation necessary */
          } /* else from if (b) { */
        } /* if (spec) {  */
        if ( sndcp_data->tcp_flow)
        {
           TRACE_EVENT("sndcp_data.tcp_flow --- DATA FLOW PRIMITIVE ");
           TRACE_EVENT_P1("nsapi value %d", snsm_activate_ind->nsapi);
	         nd_dti_buffer_ready(snsm_activate_ind->nsapi);
        }
      } /* if (used == FALSE) { */
      break;
    default:
      TRACE_ERROR( "SNSM_ACTIVATE_IND unexpected" );
      break;
  }
  if (snsm_activate_ind != NULL) {
    PFREE(snsm_activate_ind);
  }

} /* mg_snsm_activate_ind() */



/*
+------------------------------------------------------------------------------
| Function    : mg_snsm_deactivate_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive SNSM_DEACTIVATE_IND
|
| Parameters  : *snsm_deactivate_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
#ifdef SNDCP_UPM_INCLUDED
GLOBAL void mg_snsm_deactivate_ind ( T_SN_DEACTIVATE_REQ *snsm_deactivate_ind )
#else
GLOBAL void mg_snsm_deactivate_ind ( T_SNSM_DEACTIVATE_IND *snsm_deactivate_ind )
#endif /*#ifdef SNDCP_UPM_INCLUDED*/
{ 
  UBYTE nsapi = 0;
  BOOL nec = FALSE;

  TRACE_FUNCTION( "mg_snsm_deactivate_ind" );
    
  switch( GET_STATE( MG ) )
  {
    case MG_DEFAULT:
      for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi ++) {
#ifdef SNDCP_UPM_INCLUDED  
        if (snsm_deactivate_ind->nsapi == nsapi) {
#else
        if (snsm_deactivate_ind->nsapi_set & (0x001 << nsapi)) {
#endif
          USHORT sapi_state = 0;
          BOOL ack = FALSE;
          BOOL used = FALSE;
          UBYTE sapi = 0;

          /*
           * Delete information about requested compressors.
           */
          sndcp_reset_xid_block(&sndcp_data->mg.user_xid_block[nsapi]);
          /*
           * If nsapi is not used, send response anyway to avoid block.
           */
          sndcp_is_nsapi_used(nsapi, &used);
          if (!used) {
#ifdef SNDCP_UPM_INCLUDED  
            PALLOC(snsm_deactivate_res, SN_DEACTIVATE_CNF);
            snsm_deactivate_res->nsapi = nsapi;
          #else
            PALLOC(snsm_deactivate_res, SNSM_DEACTIVATE_RES);
            snsm_deactivate_res->nsapi = nsapi;
          #endif /*#ifdef SNDCP_UPM_INCLUDED*/
            /*
             * Set the "state" of the nsapi.
             */
            sndcp_unset_nsapi_state(nsapi, MG_DEACT);


#ifdef SNDCP_UPM_INCLUDED  
            PSEND(hCommUPM, snsm_deactivate_res);
#else /*#ifdef SNDCP_UPM_INCLUDED*/
            PSEND(hCommSM, snsm_deactivate_res);
#endif /*#ifdef SNDCP_UPM_INCLUDED*/
            continue;
          }


          /*
           * Close affected DTI connection.
           */
#ifdef _SNDCP_DTI_2_
          /* FIX
          for entity test TC TC 321 and TC 327 and TC 262a*/
          sndcp_data->nu = & sndcp_data->nu_base[nsapi];
         /* End FIX */
#endif /* _SNDCP_DTI_2_ */
#ifndef SNDCP_UPM_INCLUDED
          mg_dti_close(nsapi);
#else
          /* 
           * This part is required for EDGE to get next line out of if 
           * statement.
           */
          {} 
#endif
          sndcp_set_nsapi_state(nsapi, MG_DEACT);
          sndcp_unset_nsapi_state(nsapi, MG_ACT);

          sndcp_get_nsapi_ack(nsapi, &ack);
          sndcp_get_nsapi_sapi(nsapi, &sapi);
          if (ack) {
            mg_rel_nsapi_nec(nsapi);
          }
          sndcp_get_sapi_state(sapi, &sapi_state);
          mg_delete_npdus(nsapi);
          sndcp_data->nu_base[nsapi].sn_unitready_ind_pending = FALSE;
          sndcp_data->nu_base[nsapi].sn_ready_ind_pending = FALSE;
          /*
           * is compressor deactivation necessary?
           */
          mg_is_rel_comp_nec(nsapi, &nec);

#ifdef SNDCP_UPM_INCLUDED  
          if ((snsm_deactivate_ind->rel_ind == PS_REL_IND_YES)
              &&
              ((sapi_state & (MG_REL + MG_REL_NEC_LOC + MG_REL_NEC_PEER)) == 0)
             ) {
        #else
          if ((snsm_deactivate_ind->rel_ind == REL_IND_YES)
              &&
             ((sapi_state & (MG_REL + MG_REL_NEC_LOC + MG_REL_NEC_PEER)) == 0)
             ) {
        #endif /*#ifdef SNDCP_UPM_INCLUDED*/
            /*
             * Local deactivation. No LL_XID_REQ will be sent (because rel_ind
             * is REL_IND_YES).
             * No release is pending or needed.
             */

#ifdef SNDCP_UPM_INCLUDED  
              PALLOC(snsm_deactivate_res, SN_DEACTIVATE_CNF);
              snsm_deactivate_res->nsapi = nsapi;
            #else
              PALLOC(snsm_deactivate_res, SNSM_DEACTIVATE_RES);
              snsm_deactivate_res->nsapi = nsapi;
            #endif /*#ifdef SNDCP_UPM_INCLUDED*/
            /*
             * Now the NSAPI is not in use anymore:
             */
            sndcp_set_nsapi_used(nsapi, FALSE);
            sndcp_set_nsapi_ack(nsapi, FALSE);
            /*
             * Set the "state" of the nsapi.
             */
            sndcp_unset_nsapi_state(nsapi, MG_DEACT);
            /*
             * Reset xid blocks.
             */
            mg_reset_compressors(nsapi);


          #ifdef SNDCP_UPM_INCLUDED  
            PSEND(hCommUPM, snsm_deactivate_res);
           #else 
            PSEND(hCommSM, snsm_deactivate_res);
           #endif /*#ifdef SNDCP_UPM_INCLUDED*/
            
          } else if ((sapi_state & (MG_REL + MG_REL_NEC_LOC + MG_REL_NEC_PEER)) 
                     == 0
                    ) {

            /*
             * No release is pending.
             * If no compressor has to be released: respond to SM.
             */
            if (nec) {
              /*
               * If xid req is sent, suspend uplink transfer for affected sapi.
               */
              sig_mg_su_suspend(sapi);
              sig_mg_sua_suspend(sapi);
              mg_suspend_affected_nus(sapi);
              /*
               * Negotiate possible deactivation of compressors.
               */
              mg_send_xid_req_del(sapi);

            } else { /* if (nec)  */

              sndcp_get_sapi_state(sapi, &sapi_state);
              if ((sapi_state & MG_XID_NEC) == 0) {
                /*
                 * No compressor negotiated, no release sent.
                 */

          #ifdef SNDCP_UPM_INCLUDED  
                PALLOC(snsm_deactivate_res, SN_DEACTIVATE_CNF);
                snsm_deactivate_res->nsapi = nsapi;
               #else
                PALLOC(snsm_deactivate_res, SNSM_DEACTIVATE_RES);
                snsm_deactivate_res->nsapi = nsapi;
               #endif /*#ifdef SNDCP_UPM_INCLUDED*/


                /*
                 * Now the NSAPI is not in use anymore:
                 */
                sndcp_set_nsapi_used(nsapi, FALSE);
                sndcp_set_nsapi_ack(nsapi, FALSE);
                /*
                 * Set the "state" of the nsapi.
                 */
                sndcp_unset_nsapi_state(nsapi, MG_DEACT);

              
          #ifdef SNDCP_UPM_INCLUDED  
                PSEND(hCommUPM, snsm_deactivate_res);
              #else /*#ifdef SNDCP_UPM_INCLUDED*/
                PSEND(hCommSM, snsm_deactivate_res);
              #endif /*#ifdef SNDCP_UPM_INCLUDED*/

              }
            } /* if (nec)  */
          } else {/* if ((state & (MG_REL + ....)) == 0) */
            /*
             * LL_RELEASE_REQ is pending. After reception of LL_RELEASE_REQ,
             * if XID negotiatio is necessary, LL_XID_REQ will be sent.
             */
            if (nec) {
              sndcp_set_sapi_state(sapi, MG_XID_NEC);
            }
          }
        }
      }
      break;
    default:
      TRACE_ERROR( "SNSM_DEACTIVATE_IND unexpected" );
      break;
  }
  if (snsm_deactivate_ind != NULL) {
    PFREE(snsm_deactivate_ind);
  }


} /* mg_snsm_deactivate_ind() */



/*
+------------------------------------------------------------------------------
| Function    : mg_snsm_sequence_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive SNSM_SEQUENCE_IND
|
| Parameters  : *snsm_sequence_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
#ifdef SNDCP_UPM_INCLUDED
GLOBAL void mg_snsm_sequence_ind ( T_SN_SEQUENCE_REQ *snsm_sequence_ind )
#else
GLOBAL void mg_snsm_sequence_ind ( T_SNSM_SEQUENCE_IND *snsm_sequence_ind )
#endif /*#ifdef SNDCP_UPM_INCLUDED*/
{ 

#ifdef SNDCP_UPM_INCLUDED
  int i = 0;
  U8 nsapi,rec_no,npdu_number;
#endif

  TRACE_FUNCTION( "mg_snsm_sequence_ind" );
    
  switch( GET_STATE( MG ) )
  {
    case MG_DEFAULT:
      {
#ifdef SNDCP_UPM_INCLUDED 

      PALLOC(snsm_sequence_res, SN_SEQUENCE_CNF);

      for (i = 0; 
           i < snsm_sequence_ind->c_receive_no_list; 
           i++) {

        nsapi = snsm_sequence_ind->receive_no_list[i].nsapi;
        rec_no = snsm_sequence_ind->receive_no_list[i].receive_no;

        sndcp_unset_nsapi_state(nsapi, MG_SEQ);
        sig_mg_nu_delete_to(nsapi, rec_no);
        npdu_number = sig_mg_nd_get_rec_no(nsapi);
        snsm_sequence_res->receive_no_list[i].nsapi = nsapi;
        snsm_sequence_res->receive_no_list[i].receive_no = npdu_number;
      }
      snsm_sequence_res->c_receive_no_list = i;
      PSEND(hCommUPM, snsm_sequence_res);
#else
      TRACE_EVENT_P1( "SEQ_IND rec_no: %d", snsm_sequence_ind->rec_no);
      sndcp_unset_nsapi_state(snsm_sequence_ind->nsapi, MG_SEQ);
      sig_mg_nu_delete_to(snsm_sequence_ind->nsapi,
                          snsm_sequence_ind->rec_no);
      sig_mg_nd_get_rec_no(snsm_sequence_ind->nsapi);
#endif /*#ifdef SNDCP_UPM_INCLUDED*/
      break;
      }
    default:
      TRACE_ERROR( "SNSM_SEQUENCE_IND unexpected" );
      break;
  }
  if (snsm_sequence_ind != NULL) {
    PFREE (snsm_sequence_ind);
  }

} /* mg_snsm_sequence_ind() */



/*
+------------------------------------------------------------------------------
| Function    : mg_snsm_modify_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive SNSM_MODIFY_IND
|
| Parameters  : *snsm_modify_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
#ifdef SNDCP_UPM_INCLUDED
GLOBAL void mg_snsm_modify_ind ( T_SN_MODIFY_REQ *snsm_modify_ind )
#else
GLOBAL void mg_snsm_modify_ind ( T_SNSM_MODIFY_IND *snsm_modify_ind )
#endif /*#ifdef SNDCP_UPM_INCLUDED*/
{ 
  /*
   * LLC mode specified?
   */
  BOOL spec = FALSE;
  /*
   * LLC mode acknowledged?
   */
  BOOL b = FALSE;
  BOOL nsapi_used = FALSE;
  BOOL old_nsapi_ack = FALSE;
  UBYTE old_sapi = 0;

  TRACE_FUNCTION( "mg_snsm_modify_ind" );
  
  switch( GET_STATE( MG ) )
  {
    case MG_DEFAULT:
      /*
       * If no release/establish will be done, SNSM_MODIFY_RES is expected now.
       */
      sndcp_data->mg.mod_expects = MG_MOD_X_READY;
      /*
       * Is NSAPI already used?
       */
      sndcp_is_nsapi_used(snsm_modify_ind->nsapi, &nsapi_used);
      if (! nsapi_used) {
        mg_mod_nsapi_new(snsm_modify_ind);
      }
      /*
       * Is LLC operation mode changing from unack to ack?
       */
      mg_is_ack(snsm_modify_ind->snsm_qos, &spec, &b);
      sndcp_get_nsapi_ack(snsm_modify_ind->nsapi, &old_nsapi_ack);
      if (spec && b && ! old_nsapi_ack) {
        mg_mod_unack_ack(snsm_modify_ind);
      }
      /*
       * Is LLC operation mode changing from ack to unack?
       */
      if (spec && ! b && old_nsapi_ack) {
        mg_mod_ack_unack(snsm_modify_ind);
      }
      /*
       * Is new sapi different from old one?
       */
      sndcp_get_nsapi_sapi(snsm_modify_ind->nsapi, &old_sapi);
      if (old_sapi != snsm_modify_ind->sapi) {
        mg_mod_sapi_diff(snsm_modify_ind);
      }
      /*
       * Add nsapi to waiting_nsapis
       */

        sndcp_data->mg.waiting_nsapis |= (1 << (snsm_modify_ind->nsapi)); 

      /*
       * The following will be done in any case:
       */
      sndcp_set_nsapi_qos(snsm_modify_ind->nsapi, snsm_modify_ind->snsm_qos);
      sndcp_set_nsapi_prio(snsm_modify_ind->nsapi, snsm_modify_ind->radio_prio);

#ifdef REL99 
      sndcp_set_nsapi_pktflowid(snsm_modify_ind->nsapi, 
                                snsm_modify_ind->pkt_flow_id);
#endif /*REL99*/
      /*
       * If no establish/release is expected, send res now.
       */
      mg_mod_res_if_nec();
      break;
    default:
      TRACE_ERROR( "SNSM_MODIFY_IND unexpected" );
      break;
  }
  if (snsm_modify_ind != NULL) {
    PFREE(snsm_modify_ind);
  }

} /* mg_snsm_modify_ind() */


#ifndef SNDCP_UPM_INCLUDED
/*
+------------------------------------------------------------------------------
| Function    : mg_sn_switch_req
+------------------------------------------------------------------------------
| Description : Handles the primitive SN_SWITCH_REQ
|
| Parameters  : *snswitch_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/

GLOBAL void mg_sn_switch_req ( T_SN_SWITCH_REQ *sn_switch_req )
{ 
#ifdef _SNDCP_DTI_2_
  UBYTE* neighbor = NULL;
#endif /* _SNDCP_DTI_2_ */
  TRACE_FUNCTION( "mg_sn_switch_req" );
  
  switch( GET_STATE( MG ) )
  {
    case MG_DEFAULT:
      sndcp_set_nsapi_linkid(sn_switch_req->nsapi,
                             sn_switch_req->dti_linkid);
#ifdef _SNDCP_DTI_2_
      sndcp_get_nsapi_neighbor(sn_switch_req->nsapi, &neighbor);
#endif /* _SNDCP_DTI_2_ */
#ifdef _SIMULATION_
      sndcp_set_nsapi_neighbor(sn_switch_req->nsapi,
                               (sn_switch_req->dti_neighbor == 
                                  0xabcdabcd 
                                ?
                                "PPP" 
                                :
                                "NULL"));
#else /*_SIMULATION_ */
      sndcp_set_nsapi_neighbor(sn_switch_req->nsapi,
                               (UBYTE*)sn_switch_req->dti_neighbor);
#endif /* _SIMULATION_ */
      /*
       * Map sn constants to dti constants.
       */
      if (sn_switch_req->dti_direction == SN_HOME) {
        sndcp_set_nsapi_direction(sn_switch_req->nsapi, HOME);
      } else {
        sndcp_set_nsapi_direction(sn_switch_req->nsapi, NEIGHBOR);
      }
#ifdef _SIMULATION_
#ifdef _SNDCP_DTI_2_
/* FIX for entity test TC 1205*/
      sndcp_data->nu = & sndcp_data->nu_base[sn_switch_req->nsapi];
      if ((sndcp_data->nu->connection_is_opened == TRUE) || 
        ((neighbor != NULL) && !(strcmp(neighbor,"NULL"))))
/* End FIX */
#endif /* _SNDCP_DTI_2_ */
#endif /* _SIMULATION_ */
#ifndef SNDCP_UPM_INCLUDED
      mg_dti_close(sn_switch_req->nsapi);
#endif
      sndcp_data->nu->sn_switch_cnf_expected = TRUE;
#ifndef SNDCP_UPM_INCLUDED
      mg_dti_open(sn_switch_req->nsapi);
#endif      

      break;
    default:
      TRACE_ERROR( "SN_SWITCH_REQ unexpected" );
      break;
  }

  PFREE(sn_switch_req);

} /* mg_sn_switch_req() */
#endif /*SNDCP_UPM_INCLUDED*/


#ifdef SNDCP_UPM_INCLUDED
/*
+------------------------------------------------------------------------------
| Function    : mg_sn_dti_req
+------------------------------------------------------------------------------
| Description : Handles the primitive SN_DTI_REQ
|
| Parameters  : *sn_dti_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_sn_dti_req ( T_SN_DTI_REQ *sn_dti_req )
{ 

  UBYTE* neighbor = NULL;

  /*
   * is nsapi already used?
   */
/*  BOOL used = FALSE;*/
  BOOL ack = FALSE;

  TRACE_FUNCTION( "mg_sn_dti_req" );
  
  switch( GET_STATE( MG ) )
  {
    case MG_DEFAULT:
      sndcp_set_nsapi_linkid(sn_dti_req->nsapi,
                             sn_dti_req->dti_linkid);

      TRACE_EVENT_P1("sn_dti_req->nsapi: %d", sn_dti_req->nsapi);

      if (sn_dti_req->dti_conn == NAS_CONNECT_DTI) {

        sndcp_get_nsapi_neighbor(sn_dti_req->nsapi, &neighbor);
        sndcp_data->nsapi = sn_dti_req->nsapi;

#ifdef SNDCP_2to1
       sndcp_set_nsapi_neighbor(sn_dti_req->nsapi,
                                (U8*)sn_dti_req->dti_neighbor.name);
#else /*SNDCP_2to1*/
#ifdef _SIMULATION_
        sndcp_set_nsapi_neighbor(sn_dti_req->nsapi,
                                 (sn_dti_req->dti_neighbor == 
                                    0xabcdabcd 
                                  ?
                                  "PPP" 
                                  :
                                  "NULL"));
#else /*_SIMULATION_ */
        sndcp_set_nsapi_neighbor(sn_dti_req->nsapi,
                                 (U8*)sn_dti_req->dti_neighbor);
#endif /* _SIMULATION_ */
#endif /*SNDCP_2to1*/

        /*
         * Map sn constants to dti constants.
         * The compiler switch is required because the dti_direction  
         * C-Macros have different values in old and new worlds.
         */

#ifdef SNDCP_UPM_INCLUDED
        if (sn_dti_req->dti_direction == NAS_HOME) 
        {
          sndcp_set_nsapi_direction(sn_dti_req->nsapi, DTI_CHANNEL_TO_LOWER_LAYER);
        } 
        else 
        {
          sndcp_set_nsapi_direction(sn_dti_req->nsapi, DTI_CHANNEL_TO_HIGHER_LAYER);
        }
#else /*SNDCP_UPM_INCLUDED*/
        if (sn_dti_req->dti_direction == DTI_CHANNEL_TO_LOWER_LAYER) 
        {
          sndcp_set_nsapi_direction(sn_dti_req->nsapi, DTI_CHANNEL_TO_LOWER_LAYER);
        } 
        else 
        {
          sndcp_set_nsapi_direction(sn_dti_req->nsapi, DTI_CHANNEL_TO_HIGHER_LAYER);
        }
#endif
        sndcp_set_nsapi_linkid(sn_dti_req->nsapi, 
                               sn_dti_req->dti_linkid);
        if (sn_dti_req->dti_neighbor NEQ (ULONG)TCPIP_NAME)
        {
            mg_dti_open(sn_dti_req->nsapi);
        }
#ifdef SNDCP_UPM_INCLUDED
        sndcp_get_nsapi_ack(sn_dti_req->nsapi, &ack);
        if (ack) {
          UBYTE sapi = 0;
          USHORT stat = MG_IDLE;
          sndcp_get_nsapi_sapi(sn_dti_req->nsapi, &sapi);
          sndcp_get_sapi_state(sapi, &stat);
          /*
           * Do not call nu_ready_ind_if_nec() if establish
           * is ongoing.
           */
          if ((stat & MG_EST) == 0) {
            nu_ready_ind_if_nec(sn_dti_req->nsapi);
          }
        } else {
          nu_unitready_ind_if_nec(sn_dti_req->nsapi);
        }
#endif

       } else {  /*  if (sn_dti_req->dti_conn == SNDCP_CONNECT_DTI) */  

        
        /* check if flow control is  received before PDP activation */
        mg_check_unset_nsapi_flow_ctrl_flag (sn_dti_req->nsapi,MG_MOD_X_NONE);

        mg_dti_close(sn_dti_req->nsapi);
      } /*  if (sn_dti_req->dti_conn == SNDCP_CONNECT_DTI) */

      break;
    default:
      TRACE_ERROR( "SN_DTI_REQ unexpected" );
      break;
  }

  PFREE(sn_dti_req);

} /* mg_sn_dti_req() */



#endif /* SNDCP_UPM_INCLUDED */

#ifdef TI_DUAL_MODE
/*
+------------------------------------------------------------------------------
| Function    : mg_get_pending_pdu_req
+------------------------------------------------------------------------------
| Description : Handles the primitive SN_GET_PENDING_PDU_REQ
|
| Parameters  : *sn_get_pending_pdu_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_get_pending_pdu_req (T_SN_GET_PENDING_PDU_REQ 
                                     *sn_get_pending_pdu_req)
{
  U8 nsapi = 0;
  BOOL ack = FALSE, used = FALSE;
  
  TRACE_FUNCTION( "mg_get_pending_pdu_req" );

  switch( GET_STATE( MG ) )
  {
    case MG_DEFAULT:
    {
      PALLOC(sn_get_pending_pdu_cnf, SN_GET_PENDING_PDU_CNF);
#ifdef _SIMULATION_
      PALLOC(sn_test_get_pending_pdu_cnf, SN_TEST_GET_PENDING_PDU_CNF);
#endif
      memset(sn_get_pending_pdu_cnf->ul_pdus,0, sizeof(T_SN_ul_pdus));
      for (nsapi = 0; nsapi < SN_SIZE_NSAPI; nsapi++) {
        sn_get_pending_pdu_cnf->ul_pdus[nsapi].c_desc_list2 = 0;
        sn_get_pending_pdu_cnf->ul_pdus[nsapi].nsapi = 0;
        sn_get_pending_pdu_cnf->ul_pdus[nsapi].dl_sequence_number = 0;
        sndcp_is_nsapi_used(nsapi, &used);
        if (used) {
          sndcp_get_nsapi_ack(nsapi, &ack);
          if (ack) {/*acknowledged context*/
            sn_get_pending_pdu_cnf = 
                mg_get_unsent_unconfirmed_npdus(nsapi, sn_get_pending_pdu_cnf);
            mg_clean_ack_npdu_queues_leave_data(nsapi);
          } else {/*unacknowledged context*/
            mg_clean_unack_npdu_queues_including_data(nsapi);
          }
        }
      }
      sn_get_pending_pdu_cnf->c_ul_pdus=SN_SIZE_NSAPI;
#ifdef _SIMULATION_
      sm_make_test_pending_pdu_cnf(sn_get_pending_pdu_cnf,
                                   sn_test_get_pending_pdu_cnf);
      PSEND(hCommUPM, sn_test_get_pending_pdu_cnf);
#else
      PSEND(hCommUPM, sn_get_pending_pdu_cnf);
#endif
      break;
    }
    default:
      TRACE_ERROR( "SN_GET_PENDING_PDU_REQ unexpected" );
      break;
  }
  if (sn_get_pending_pdu_req != NULL) {
    PFREE (sn_get_pending_pdu_req);
  }
}
#endif /*TI_DUAL_MODE*/

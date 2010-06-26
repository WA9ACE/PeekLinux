/*
+-----------------------------------------------------------------------------
|  Project :  GPRS (8441)
|  Modul   :  sndcp_nup.c
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
|             the SDL-documentation (NU-statemachine)
+-----------------------------------------------------------------------------
*/

/*---- HISTORY --------------------------------------------------------------*/

#define ENTITY_SNDCP

/*==== INCLUDES =============================================================*/

#include "typedefs.h"    /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gsm.h"        /* to get a lot of macros */
#include "prim.h"       /* to get the definitions of used SAP and directions */

#include "dti.h"

#include "sndcp.h"        /* to get the global entity definitions */
#include "sndcp_f.h"       /* to get the functions to access the global arrays*/

#include "sndcp_nuf.h"    /* to get functions from nu service */
#include "sndcp_sus.h"    /* to get internal signals to su signals */
#include "sndcp_suas.h"    /* to get internal signals to sua signals */


/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/


/*
+------------------------------------------------------------------------------
| Function    : nu_connection_state
+------------------------------------------------------------------------------
| Description : DTI connnection has been opened by calling sndcp_sig_callback
|               with reason DTI_REASON_CONNECTION_OPENED.
|
| Parameters  : UBYTE nsapi: the affected NSAPI
|
+------------------------------------------------------------------------------
*/
GLOBAL void nu_connection_state (UBYTE nsapi, BOOL opened)
{
  TRACE_ISIG( "nu_connection_state" );

  /*
   * set service instance according to nsapi in primitive
   */
  sndcp_data->nu = & sndcp_data->nu_base[nsapi];

  switch( GET_STATE( NU ) )
  {

    case NU_UNACK_SU_RECEPTIVE :
    case NU_UNACK_SU_NOT_RECEPTIVE:
#ifndef NCONFIG
    case NU_SEND_UNACK_SU_RECEPTIVE:
    case NU_SEND_UNACK_SU_NOT_RECEPTIVE:
#endif /* NCONFIG */
      if (!sndcp_data->nu->connection_is_opened
          &&
          opened) {
        sndcp_data->nu->connection_is_opened = TRUE;
#ifndef SNDCP_UPM_INCLUDED
        if (sndcp_data->nu->sn_switch_cnf_expected) {
          PALLOC (sn_switch_cnf, SN_SWITCH_CNF);
          sn_switch_cnf->nsapi = nsapi;
          sndcp_data->nu->sn_switch_cnf_expected = FALSE;
          PSEND(hCommMMI, sn_switch_cnf);
        }
#endif /*SNDCP_UPM_INCLUDED*/
#ifdef SNDCP_UPM_INCLUDED
        {
          U32 linkid = 0;
          PALLOC (sn_dti_cnf, SN_DTI_CNF);
          sndcp_get_nsapi_linkid(nsapi, &linkid);
          sn_dti_cnf->dti_linkid = linkid;
          sndcp_data->nu->sn_dti_cnf_expected = FALSE;
          sn_dti_cnf->dti_conn = NAS_CONNECT_DTI;

          PSEND(hCommMMI, sn_dti_cnf);
        }
#endif
        nu_unitready_ind_if_nec(nsapi);

      } else if (!opened) {
        sndcp_data->nu->connection_is_opened = FALSE;
        sndcp_data->nu->sn_unitready_ind_pending = FALSE;
        sndcp_data->nu->sn_switch_cnf_expected = FALSE;
      }
      break;
    case NU_ACK_SUA_RECEPTIVE:
    case NU_ACK_SUA_NOT_RECEPTIVE:
    case NU_SUS_SUA_RECEPTIVE:
    case NU_SUS_SUA_NOT_RECEPTIVE:
    case NU_REC_SUA_NOT_RECEPTIVE:
#ifndef NCONFIG
    case NU_SEND_ACK_SUA_RECEPTIVE:
    case NU_SEND_ACK_SUA_NOT_RECEPTIVE:
    case NU_SEND_SUS_SUA_RECEPTIVE:
    case NU_SEND_SUS_SUA_NOT_RECEPTIVE:
    case NU_SEND_REC_SUA_NOT_RECEPTIVE:
#endif /* NCONFIG */
      if (!sndcp_data->nu->connection_is_opened
          &&
          opened) {
        sndcp_data->nu->connection_is_opened = TRUE;
#ifdef SNDCP_UPM_INCLUDED
        {
          U32 linkid = 0;
          PALLOC (sn_dti_cnf, SN_DTI_CNF);
          sndcp_get_nsapi_linkid(nsapi, &linkid);
          sn_dti_cnf->dti_linkid = linkid;
          sndcp_data->nu->sn_dti_cnf_expected = FALSE;
          sn_dti_cnf->dti_conn = NAS_CONNECT_DTI;

          PSEND(hCommMMI, sn_dti_cnf);
        }
        
#else 
        if (sndcp_data->nu->sn_switch_cnf_expected) {
          PALLOC (sn_switch_cnf, SN_SWITCH_CNF);
          sn_switch_cnf->nsapi = nsapi;
          sndcp_data->nu->sn_switch_cnf_expected = FALSE;
          PSEND(hCommMMI, sn_switch_cnf);
        }
#endif
        nu_ready_ind_if_nec(nsapi);
      } else if (!opened) {
        sndcp_data->nu->connection_is_opened = FALSE;
        sndcp_data->nu->sn_ready_ind_pending = FALSE;
        sndcp_data->nu->sn_switch_cnf_expected = FALSE;
      }
      break;
    default:
      TRACE_ERROR( "Function nu_connection_state unexpected" );
      break;
  }
} /* nu_connection_state() */

/*
+------------------------------------------------------------------------------
| Function    : nu_sn_data_req
+------------------------------------------------------------------------------
| Description : Handles the primitive SN_DATA_REQ
|
| Parameters  : *sn_data_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void nu_sn_data_req ( T_SN_DATA_REQ *sn_data_req )
{
  BOOL used = FALSE;
  BOOL sack = FALSE;
  UBYTE sapi = 0;

  TRACE_FUNCTION( "nu_sn_data_req" );

  dti_stop(sndcp_data->hDTI,
           0,
#ifndef SNDCP_UPM_INCLUDED
           SNDCP_INTERFACE_ACK,
#else
           SNDCP_INTERFACE_UNACK,
#endif
           sn_data_req->nsapi);

  /*
   * set service instance according to nsapi in primitive
   */
  sndcp_data->nu = & sndcp_data->nu_base[sn_data_req->nsapi];
  /*
   * User has used up his ready_ind.
   */
  sndcp_data->nu->sn_ready_ind_pending = FALSE;
  /*
   * If context is not active, discard primitive.
   */
  sndcp_is_nsapi_used(sn_data_req->nsapi, &used);
  if (!used && sn_data_req != NULL) {
    MFREE_PRIM(sn_data_req);  
    sn_data_req = NULL;
    return;
  }
  if (sn_data_req == NULL) {
    /*
     * Should not be possible. Only included to avoid NULL pointer use.
     */
    return;
  }

#ifdef SNDCP_TRACE_IP_DATAGRAM
  if(sndcp_data->trace_ip_header || sndcp_data->trace_ip_datagram){
    TRACE_EVENT("--------- Uplink IP datagram ---------");
#ifdef _SNDCP_DTI_2_
    sndcp_trace_ip_datagram(& sn_data_req->desc_list2);
#else /*_SNDCP_DTI_2_*/
    sndcp_trace_ip_datagram(& sn_data_req->desc_list);
#endif /*_SNDCP_DTI_2_*/
    TRACE_EVENT("--------------------------------------");
  } else {
#ifdef _SNDCP_DTI_2_
    sndcp_default_ip_trace(& sn_data_req->desc_list2, SNDCP_UL_PACKET);
#else /*_SNDCP_DTI_2_*/
    sndcp_default_ip_trace& sn_data_req->desc_list, SNDCP_UL_PACKET);
#endif /*_SNDCP_DTI_2_*/
  }
  if(sndcp_data->trace_ip_bin){
#ifdef _SNDCP_DTI_2_
    sndcp_bin_trace_ip(& sn_data_req->desc_list2, SNDCP_UL_PACKET);
#else /*_SNDCP_DTI_2_*/
    sndcp_bin_trace_ip(& sn_data_req->desc_list, SNDCP_UL_PACKET);
#endif /*_SNDCP_DTI_2_*/
  }

#else /* SNDCP_TRACE_IP_DATAGRAM */

#ifdef SNDCP_TRACE_BUFFER
#ifdef _SNDCP_DTI_2_
  TRACE_EVENT_P1("uplink at SN SAP: %d octets",
                 sn_data_req->desc_list2.list_len);
  sndcp_trace_desc_list(& sn_data_req->desc_list2);
#else /*_SNDCP_DTI_2_*/
  TRACE_EVENT_P1("uplink at SN SAP: %d octets",
                 sn_data_req->desc_list.list_len);
  sndcp_trace_desc_list(& sn_data_req->desc_list);
#endif /*_SNDCP_DTI_2_*/
#endif /* SNDCP_TRACE_BUFFER */
#endif /* SNDCP_TRACE_IP_DATAGRAM */


  switch( GET_STATE( NU ) )
  {
    case NU_ACK_SUA_RECEPTIVE:
      /*
       * Buffer the given N-PDU
       */
      nu_buffer_npdu(sndcp_data->nu->send_npdu_number_ack, sn_data_req);
      /*
       * It is not checked here if the nsapi is active anyway!
       */
      sndcp_get_nsapi_sapi(sndcp_data->nu->nsapi, &sapi);
      SET_STATE(NU, NU_ACK_SUA_NOT_RECEPTIVE);
      sndcp_data->nu->send_npdu_number_ack =
        (U8)((sndcp_data->nu->send_npdu_number_ack + 1) % MAX_NPDU_NUMBER_ACK);
      sndcp_get_sapi_ack(sapi, &sack);
      if(sack){
        sig_nu_sua_data_req(sn_data_req,
                            (U8)((sndcp_data->nu->send_npdu_number_ack - 1
                            + MAX_NPDU_NUMBER_ACK)% MAX_NPDU_NUMBER_ACK),
                            sn_data_req->nsapi,
                            sapi);
      }


      break;
    case NU_UNACK_SU_NOT_RECEPTIVE:
    case NU_UNACK_SU_RECEPTIVE:
      nu_unitready_ind_if_nec(sn_data_req->nsapi);
#ifdef _SNDCP_DTI_2_
      MFREE_PRIM(sn_data_req);
#else /*_SNDCP_DTI_2_*/
      PFREE_DESC(sn_data_req);
#endif /*_SNDCP_DTI_2_*/
      sn_data_req = NULL;
      break;
    case NU_REC_SUA_NOT_RECEPTIVE:
    case NU_SUS_SUA_NOT_RECEPTIVE:
    case NU_SUS_SUA_RECEPTIVE:
    case NU_ACK_SUA_NOT_RECEPTIVE:

      nu_buffer_npdu(sndcp_data->nu->send_npdu_number_ack,
                     sn_data_req);
      sndcp_data->nu->send_npdu_number_ack =
        (U8)((sndcp_data->nu->send_npdu_number_ack + 1) % MAX_NPDU_NUMBER_ACK);
      break;
#ifndef NCONFIG
    case NU_SEND_UNACK_SU_RECEPTIVE:
    case NU_SEND_UNACK_SU_NOT_RECEPTIVE:
    case NU_SEND_ACK_SUA_RECEPTIVE:
    case NU_SEND_ACK_SUA_NOT_RECEPTIVE:
    case NU_SEND_SUS_SUA_RECEPTIVE:
    case NU_SEND_SUS_SUA_NOT_RECEPTIVE:
    case NU_SEND_REC_SUA_NOT_RECEPTIVE:
#ifndef SNDCP_UPM_INCLUDED
    dti_start(sndcp_data->hDTI, 0, SNDCP_INTERFACE_ACK, sn_data_req->nsapi);
#ifdef _SNDCP_DTI_2_
    MFREE_PRIM(sn_data_req);
#else /*_SNDCP_DTI_2_*/
    PFREE_DESC(sn_data_req);
#endif /*_SNDCP_DTI_2_*/
#else /*#ifndef SNDCP_UPM_INCLUDED*/
    dti_start(sndcp_data->hDTI, 0, SNDCP_INTERFACE_UNACK, sn_data_req->nsapi);
    MFREE_PRIM(sn_data_req);
#endif /*#ifndef  SNDCP_UPM_INCLUDED*/

      sn_data_req = NULL;
      break;
#endif
    default:
      TRACE_ERROR( "SN_DATA_REQ unexpected" );
#ifdef _SNDCP_DTI_2_
      MFREE_PRIM(sn_data_req);
#else /*_SNDCP_DTI_2_*/
      PFREE_DESC(sn_data_req);
#endif /*_SNDCP_DTI_2_*/
      sn_data_req = NULL;
      break;
  }

} /* nu_sn_data_req() */

/*
+------------------------------------------------------------------------------
| Function    : nu_sn_unitdata_req
+------------------------------------------------------------------------------
| Description : Handles the primitive SN_UNITDATA_REQ
|
| Parameters  : *sn_unitdata_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void nu_sn_unitdata_req ( T_SN_UNITDATA_REQ *sn_unitdata_req )
{
  UBYTE sapi = 0;
  BOOL used = FALSE;
  UBYTE nsapi = sn_unitdata_req->nsapi;


#ifdef FLOW_TRACE
  sndcp_trace_flow_control(FLOW_TRACE_SNDCP, FLOW_TRACE_UP, FLOW_TRACE_TOP, FALSE);
#endif

  TRACE_FUNCTION( "nu_sn_unitdata_req" );

     dti_stop(sndcp_data->hDTI,
              0,
              SNDCP_INTERFACE_UNACK,
              sn_unitdata_req->nsapi);

 /*
  * set service instance according to nsapi in primitive
  */
  sndcp_data->nu = & sndcp_data->nu_base[sn_unitdata_req->nsapi];
  /*
   * User has used up his ready_ind.
   */
  sndcp_data->nu->sn_unitready_ind_pending = FALSE;
  /*
   * If context is not active, discard primitive.
   */
  sndcp_is_nsapi_used(sn_unitdata_req->nsapi, &used);
  if (!used) {
#ifdef _SNDCP_DTI_2_
    MFREE_PRIM (sn_unitdata_req);
#else /*_SNDCP_DTI_2_*/
    PFREE_DESC (sn_unitdata_req);
#endif /*_SNDCP_DTI_2_*/
    sn_unitdata_req = NULL;
    return;
  }

#ifdef SNDCP_TRACE_IP_DATAGRAM
  if(sndcp_data->trace_ip_header || sndcp_data->trace_ip_datagram){
    TRACE_EVENT("--------- Uplink IP datagram ---------");
#ifdef _SNDCP_DTI_2_
    sndcp_trace_ip_datagram(& sn_unitdata_req->desc_list2);
#else /*_SNDCP_DTI_2_*/
    sndcp_trace_ip_datagram(& sn_unitdata_req->desc_list);
#endif /*_SNDCP_DTI_2_*/
    TRACE_EVENT("--------------------------------------");
  } else {
#ifdef _SNDCP_DTI_2_
    sndcp_default_ip_trace(& sn_unitdata_req->desc_list2, SNDCP_UL_PACKET);  
#else /*_SNDCP_DTI_2_*/
    sndcp_default_ip_trace(& sn_unitdata_req->desc_list, SNDCP_UL_PACKET);
#endif /*_SNDCP_DTI_2_*/
  }
  if(sndcp_data->trace_ip_bin){
#ifdef _SNDCP_DTI_2_
    sndcp_bin_trace_ip(& sn_unitdata_req->desc_list2, SNDCP_UL_PACKET);
#else /*_SNDCP_DTI_2_*/
    sndcp_bin_trace_ip(& sn_unitdata_req->desc_list, SNDCP_UL_PACKET);
#endif /*_SNDCP_DTI_2_*/
  } 
#else /* SNDCP_TRACE_IP_DATAGRAM */

#ifdef SNDCP_TRACE_BUFFER
#ifdef _SNDCP_DTI_2_
  TRACE_EVENT_P1("uplink at SN SAP: %d octets",
                 sn_unitdata_req->desc_list2.list_len);
  sndcp_trace_desc_list(& sn_unitdata_req->desc_list2);
#else /*_SNDCP_DTI_2_*/
  TRACE_EVENT_P1("uplink at SN SAP: %d octets",
                 sn_unitdata_req->desc_list.list_len);
  sndcp_trace_desc_list(& sn_unitdata_req->desc_list);
#endif /*_SNDCP_DTI_2_*/
#endif /* SNDCP_TRACE_BUFFER */
#endif /* SNDCP_TRACE_IP_DATAGRAM */

  switch( GET_STATE( NU ) )
  {
    case NU_ACK_SUA_NOT_RECEPTIVE:
    case NU_ACK_SUA_RECEPTIVE:
      if (sn_unitdata_req != NULL) {
#ifdef _SNDCP_DTI_2_
        MFREE_PRIM (sn_unitdata_req);
#else /*_SNDCP_DTI_2_*/
        PFREE_DESC(sn_unitdata_req);
#endif /*_SNDCP_DTI_2_*/
        sn_unitdata_req = NULL;
      }
      nu_ready_ind_if_nec(nsapi);
      break;
    case NU_UNACK_SU_NOT_RECEPTIVE:
      if (sn_unitdata_req != NULL) {
#ifdef _SNDCP_DTI_2_
        MFREE_PRIM (sn_unitdata_req);
#else /*_SNDCP_DTI_2_*/
        PFREE_DESC(sn_unitdata_req);
#endif /*_SNDCP_DTI_2_*/
        sn_unitdata_req = NULL;
      }
     break;
    case NU_UNACK_SU_RECEPTIVE:
      /*
       * It is not checked here if the nsapi is active anyway!
       */
      sndcp_get_nsapi_sapi(sndcp_data->nu->nsapi, &sapi);
      SET_STATE(NU, NU_UNACK_SU_NOT_RECEPTIVE);
      sndcp_data->nu->send_npdu_number_unack =
        (sndcp_data->nu->send_npdu_number_unack + 1) % MAX_NPDU_NUMBER_UNACK;
      sig_nu_su_unitdata_req(sn_unitdata_req,
                             (U16)((sndcp_data->nu->send_npdu_number_unack - 1
                             + MAX_NPDU_NUMBER_UNACK) % MAX_NPDU_NUMBER_UNACK),
                             sndcp_data->nu->nsapi,
                             sapi);


      break;
#ifndef NCONFIG
    case NU_SEND_UNACK_SU_RECEPTIVE:
    case NU_SEND_UNACK_SU_NOT_RECEPTIVE:
    case NU_SEND_ACK_SUA_RECEPTIVE:
    case NU_SEND_ACK_SUA_NOT_RECEPTIVE:
    case NU_SEND_SUS_SUA_RECEPTIVE:
    case NU_SEND_SUS_SUA_NOT_RECEPTIVE:
    case NU_SEND_REC_SUA_NOT_RECEPTIVE:
      dti_start(sndcp_data->hDTI, 0, SNDCP_INTERFACE_UNACK, sn_unitdata_req->nsapi);
      if (sn_unitdata_req != NULL) {
#ifdef _SNDCP_DTI_2_
        MFREE_PRIM (sn_unitdata_req);
#else /*_SNDCP_DTI_2_*/
        PFREE_DESC(sn_unitdata_req);
#endif /*_SNDCP_DTI_2_*/
        sn_unitdata_req = NULL;
      }

      break;
#endif
    default:
      TRACE_ERROR( "SN_UNITDATA_REQ unexpected" );
#ifdef _SNDCP_DTI_2_
      MFREE_PRIM (sn_unitdata_req);
#else /*_SNDCP_DTI_2_*/
      PFREE_DESC(sn_unitdata_req);
#endif /*_SNDCP_DTI_2_*/

      sn_unitdata_req = NULL;
      break;
  }

} /* nu_sn_unitdata_req() */



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
|  Purpose :  This modul is part of the entity LLC and implements all 
|             functions to handles the incoming primitives as described in 
|             the SDL-documentation (UITX-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#ifndef LLC_UITXP_C
#define LLC_UITXP_C
#endif

#define ENTITY_LLC

/*==== INCLUDES =============================================================*/

#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "cnf_llc.h"    /* to get cnf-definitions */
#include "mon_llc.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "llc.h"        /* to get the global entity definitions */
#include "llc_f.h"
#include "llc_uitxf.h"  /* to get local UITX functions */
#include "llc_txs.h"    /* to get signal interface to TX */
#include <string.h>     /* to get memcpy */
/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : uitx_ll_unitdesc_req
+------------------------------------------------------------------------------
| Description : Handles the primitive LL_UNITDESC_REQ
|
| Parameters  : *ll_unitdesc_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
/*#if defined(CF_FAST_EXEC) || defined(_SIMULATION_) || \
   defined(LL_2to1) */

GLOBAL void uitx_ll_unitdesc_req
(
#ifdef LL_DESC 
  T_LL_UNITDESC_REQ *ll_unitdesc_req_
#else
  T_LL_UNITDATA_REQ *ll_unitdesc_req_
#endif
)
{
  /*
   * Indicates if the frame shall be ciphered or not (LL_CIPHER_ON/OFF).
   */
  UBYTE                       cipher;
  T_FRAME_NUM                 used_nu;
  ULONG                       used_oc;

  TRACE_FUNCTION( "uitx_ll_unitdesc_req" );

  {
    /*
     * PPASS added to let the FRAME know that the primitive has to be copied
     * now for duplicating primiives. Otherwise the changed primitie will be duplicated 
     */
#ifdef LL_DESC
    PPASS (ll_unitdesc_req_, ll_unitdesc_req, LL_UNITDESC_REQ);
#else
    PPASS (ll_unitdesc_req_, ll_unitdesc_req, LL_UNITDATA_REQ);
#endif

    SWITCH_LLC (ll_unitdesc_req->sapi);
#ifdef REL99
    llc_data->cur_pfi        = ll_unitdesc_req->pkt_flow_id; /* store current packet flow identifier */
#endif
    TRACE_EVENT_P1("packet flow id = %d",ll_unitdesc_req->pkt_flow_id);

    llc_data->cur_qos        = ll_unitdesc_req->ll_qos;      /* store current QoS */
    llc_data->cur_radio_prio = ll_unitdesc_req->radio_prio; /* store current radio prioirity */
    /* The stored PFI,QOS and radio priority is later used to copy to PFI,QOS and radio priority
    respectively of control messages like SABM,DM,XID response,etc. */
    TRACE_EVENT_P1("radio priority = %d",ll_unitdesc_req->radio_prio);
    TRACE_EVENT_P1("peak throughput = %d",ll_unitdesc_req->ll_qos.peak);

    /*
     * Overwrite cipher request on all SAPIs except GMM. They don't
     * know anything about ciphering in current design.
     */
    if (ll_unitdesc_req->sapi != LL_SAPI_1)
    {
      if (llc_data->ciphering_algorithm == LLGMM_CIPHER_NO_ALGORITHM)
        cipher = LL_CIPHER_OFF;
      else
        cipher = LL_CIPHER_ON;
    }
    else
    {
      cipher = ll_unitdesc_req->cipher;
    }

#ifdef LL_DESC
    TRACE_3_PARA("s:%d len:%d c:%d", ll_unitdesc_req->sapi, 
                                   ll_unitdesc_req->desc_list3.list_len, 
                                   cipher );
#else
    TRACE_3_PARA("s:%d len:%d c:%d", ll_unitdesc_req->sapi, 
                                   BYTELEN(ll_unitdesc_req->sdu.l_buf), 
                                   cipher );
#endif

    /*
     * Set up the attached counter to CCI_NO_ATTACHE.
     */
     ll_unitdesc_req->attached_counter = CCI_NO_ATTACHE; 

#ifdef REL99 
    /*
     * Packet flow identifier is passed transparantly if data request is from SNDCP.
     * If data request is from GMM fill PFI = LL_PFI_SIGNALING,
     * If data request is from GSMS fill PFI = LL_PFI_SMS   
     * for all other SAPs(if sapi 2 & 8 are supported in future) set pkt_flow_id is to LL_PKT_FLOW_ID_NOT_PRES,
     * until specification are clarified.
     */
    switch(ll_unitdesc_req->sapi)
    {

      case LL_SAPI_3:
      case LL_SAPI_5:
      case LL_SAPI_9:
      case LL_SAPI_11:
        /*Packet flow identifier is passed transparantly if data request is from SNDCP.*/
        break;

      case LL_SAPI_1:
        /* 
         * From 24.008 & 23.060 it is interpreted that for all signalling data, a 
         * predefined PFI LL_PFI_SIGNALING shall be used.
         */
        ll_unitdesc_req->pkt_flow_id = LL_PFI_SIGNALING;
        ll_unitdesc_req->ll_qos.peak = LL_PEAK_256K;
        break;

      case LL_SAPI_7:
        /* 
         * From 24.008 & 23.060 it is interpreted that for all SMS data, a 
         * predefined PFI LL_PFI_SMS shall be used.
         */
        ll_unitdesc_req->pkt_flow_id = LL_PFI_SMS;
        break;

      default:
        /*
         * It is possible when system support llc sapi 2 and 8.
         * Fill PFI valuse it LL_PKT_FLOW_ID_NOT_PRES, until it is clarified in the specifications.
         */
        ll_unitdesc_req->pkt_flow_id = LL_PKT_FLOW_ID_NOT_PRES;
        break;
    }
#endif /* REL99 */
    /*
     * In case of SAPI 7 (GSMS) all requests has to use the relclass LL_RLC_PROT.
     */
    if (ll_unitdesc_req->sapi EQ LL_SAPI_7)
    {
      ll_unitdesc_req->ll_qos.relclass = LL_RLC_PROT;
    }

    switch (GET_STATE(UITX))
    {
      case UITX_ADM_READY:
        /*
         * NOTE: uitx_build_ui_header expects desc_list3 to already contain data.
         * The header is inserted _before_ the data, so desc_list3.o_buf is being 
         * decremented.
         */
        uitx_build_ui_header
          (
#ifdef LL_DESC
          &ll_unitdesc_req->desc_list3,
#else
          &ll_unitdesc_req->sdu,
#endif
          MS_COMMAND,
          ll_unitdesc_req->sapi, 
          llc_data->uitx->vu, 
          cipher,
          ll_unitdesc_req->ll_qos.relclass
          );

        used_nu = llc_data->uitx->vu;
        used_oc = llc_data->sapi->oc_ui_tx;

        llc_data->uitx->vu ++;

        if (llc_data->uitx->vu > MAX_SEQUENCE_NUMBER)
        {
          llc_data->uitx->vu        = 0;
          llc_data->sapi->oc_ui_tx += (MAX_SEQUENCE_NUMBER+1);
        }

        SET_STATE (UITX, UITX_ADM_NOT_READY);

        if (ll_unitdesc_req->ll_qos.relclass EQ LL_RLC_PROT)
        {
          /*
           * Label S_DATA
           */
          if (ll_unitdesc_req->sapi EQ LL_SAPI_1)
          {
            sig_uitx_tx_data_req (ll_unitdesc_req, cipher, GRLC_DTACS_MOBILITY_MANAGEMENT, used_nu, used_oc);
          }
          else
          {
            sig_uitx_tx_data_req (ll_unitdesc_req, cipher, GRLC_DTACS_DEF, used_nu, used_oc);
          }
        }
        else /* LL_PROT/LL_NO_REL */
        {
          /*
           * Label S_UNITDATA
           */
          sig_uitx_tx_unitdata_req (ll_unitdesc_req, cipher, used_nu, used_oc);
        }
        break;

      case UITX_TLLI_UNASSIGNED_READY:
        /*
         * LLC is not capable of sending data in state TLLI_UNASSIGNED.
         */
#ifdef LL_DESC
        llc_cl_desc3_free((T_desc3*)ll_unitdesc_req->desc_list3.first);
#endif
        PFREE (ll_unitdesc_req);

        SET_STATE (UITX, UITX_TLLI_UNASSIGNED_NOT_READY);
        break;

      default:
#ifdef LL_DESC
        llc_cl_desc3_free((T_desc3*)ll_unitdesc_req->desc_list3.first);
#endif   
        PFREE (ll_unitdesc_req);

        TRACE_ERROR( "ll_unitdesc_req unexpected" );
        break;
    }
  }
}/* uitx_ll_unitdesc_req */

/*#endif*/ /* CF_FAST_EXEC || _SIMULATION_ */

/*
+------------------------------------------------------------------------------
| Function    : uitx_ll_unitdata_req
+------------------------------------------------------------------------------
| Description : Handles the primitive LL_UNITDATA_REQ
|
| Parameters  : *ll_unitdata_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
#ifdef LL_DESC

#ifndef CF_FAST_EXEC

GLOBAL void uitx_ll_unitdata_req (T_LL_UNITDATA_REQ *ll_unitdata_req)
{ 
  T_desc3 *desc3 = NULL;
  U16     len, offset;
  U8      *buf;

  TRACE_FUNCTION( "uitx_ll_unitdata_req" );

  /*
   * Allocate primitive and fill in the structure elements
   */
  {
    PALLOC(ll_unitdesc_req, LL_UNITDESC_REQ);

    ll_unitdesc_req->sapi             = ll_unitdata_req->sapi;       
    ll_unitdesc_req->tlli             = ll_unitdata_req->tlli;       
    ll_unitdesc_req->ll_qos           = ll_unitdata_req->ll_qos;     
    ll_unitdesc_req->cipher           = ll_unitdata_req->cipher;     
    ll_unitdesc_req->radio_prio       = ll_unitdata_req->radio_prio; 
    ll_unitdesc_req->seg_pos          = ll_unitdata_req->seg_pos;    
    ll_unitdesc_req->attached_counter = ll_unitdata_req->attached_counter;
#ifdef REL99 
    ll_unitdesc_req->pkt_flow_id = ll_unitdata_req->pkt_flow_id;
#endif /* REL99 */
    /*
     * Allocate memory block for simulating desc2 (desc3->buffer) and set the values of the 
     * desc3-elements.
     */
    len    = ll_unitdata_req->sdu.l_buf >> 3; /* BIT to BYTE */
    offset = ll_unitdata_req->sdu.o_buf >> 3; /* BIT to BYTE */

    desc3  = llc_palloc_desc(len, offset); /* Only one desc3 and buffer is allocated */
    
    buf= (U8*)desc3->buffer; 
    
    memcpy(&buf[desc3->offset], 
           &ll_unitdata_req->sdu.buf[offset], 
           len);
    
    ll_unitdesc_req->desc_list3.first       = (U32)desc3;
    ll_unitdesc_req->desc_list3.list_len    = desc3->len;

    
    /*
     * Primitive is handled in uitx_ll_unitdesc_req
     */
    uitx_ll_unitdesc_req (ll_unitdesc_req);
  }
  PFREE(ll_unitdata_req);

} /* uitx_ll_unitdata_req() */

#endif /* CF_FAST_EXEC */

#endif /* LL_DESC */

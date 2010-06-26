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
|             functions to handles the incoming process internal signals as  
|             described in the SDL-documentation (UITX-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#ifndef LLC_UITXS_C
#define LLC_UITXS_C
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
#include "llc_f.h"      /* to get llc-functions  */
#include "llc_uitxf.h"  /* to get local UITX functions */
#include "llc_txs.h"    /* to get signal interface to TX */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : sig_llme_uitx_assign_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_LLME_UITX_ASSIGN_REQ
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_llme_uitx_assign_req (void) 
{ 
  TRACE_ISIG( "sig_llme_uitx_assign_req" );
  
  switch (GET_STATE(UITX))
  {
    case UITX_TLLI_UNASSIGNED_NOT_READY:
      /*
       * <R.LLC.TLLI_ASS.A.007>, <R.LLC.TLLI_ASS.A.008>
       */
      uitx_init_sapi();
      
      /*
       * <R.LLC.TLLI_ASS.A.006>
       */
      SET_STATE (UITX, UITX_ADM_NOT_READY);
      break;
    case UITX_TLLI_UNASSIGNED_READY:
      /*
       * <R.LLC.TLLI_ASS.A.007>, <R.LLC.TLLI_ASS.A.008>
       */
      uitx_init_sapi();

      /*
       * <R.LLC.TLLI_ASS.A.006>
       */
      SET_STATE (UITX, UITX_ADM_READY);
      break;
    default:
      TRACE_ERROR( "SIG_LLME_UITX_ASSIGN_REQ unexpected" );
      break;
  }
} /* sig_llme_uitx_assign_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_llme_uitx_unassign_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_LLME_UITX_UNASSIGN_REQ
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_llme_uitx_unassign_req (void) 
{ 
  TRACE_ISIG( "sig_llme_uitx_unassign_req" );
  
  switch (GET_STATE(UITX))
  {
    case UITX_ADM_NOT_READY:
      SET_STATE (UITX, UITX_TLLI_UNASSIGNED_NOT_READY);
      break;
    case UITX_ADM_READY:
      SET_STATE (UITX, UITX_TLLI_UNASSIGNED_READY);
      break;
    default:
      TRACE_ERROR( "SIG_LLME_UITX_UNASSIGN_REQ unexpected" );
      break;
  }
} /* sig_llme_uitx_unassign_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_llme_uitx_reset_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_LLME_UITX_RESET_REQ
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_llme_uitx_reset_req (void) 
{ 
  TRACE_ISIG( "sig_llme_uitx_reset_req" );
  
  switch (GET_STATE(UITX))
  {
    case UITX_ADM_NOT_READY:
      /*
       * No break!
       */
    case UITX_ADM_READY:
      /*
       * ATTENTION:
       * Only use this procedure as long as it initialises only the requested
       * variables on reset (V(U) as of V6.4.0).
       */
      uitx_init_sapi();
      break;
    default:
      TRACE_ERROR( "SIG_LLME_UITX_RESET_REQ unexpected" );
      break;
  }
} /* sig_llme_uitx_reset_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_tx_uitx_ready_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TX_UITX_READY_IND
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tx_uitx_ready_ind (void)
{ 
  TRACE_ISIG( "sig_tx_uitx_ready_ind" );
  
  switch (GET_STATE(UITX))
  {
    case UITX_TLLI_UNASSIGNED_NOT_READY:
      /*
       * This case should not be necessary, however, it depends on the assign 
       * order within LLME if this case is executed.
       */
      SET_STATE (UITX, UITX_TLLI_UNASSIGNED_READY);
        
      /*
       * No flow control for SAPI 1.
       */
      if (llc_data->current_sapi != LL_SAPI_1)
      {
        PALLOC (ll_unitready_ind, LL_UNITREADY_IND);
        
        ll_unitready_ind->sapi = llc_data->current_sapi;
        
        /*
         * Send primitive LL-UNITREADY-IND to either SNDCP or GSMS, 
         * depending on the SAPI.
         */
        switch (llc_data->current_sapi)
        {
          case LL_SAPI_3:
          case LL_SAPI_5:
          case LL_SAPI_9:
          case LL_SAPI_11:
            TRACE_1_OUT_PARA("s:%d", ll_unitready_ind->sapi);            
            PSEND (hCommSNDCP, ll_unitready_ind);
            break;
          case LL_SAPI_7:
#ifndef LL_2to1
            TRACE_PRIM_TO("GSMS");
            TRACE_1_OUT_PARA("s:%d", ll_unitready_ind->sapi);            
            PSEND (hCommGSMS, ll_unitready_ind);
#else
            PFREE (ll_unitready_ind);
#endif
            break;
          default:
            PFREE (ll_unitready_ind);
            TRACE_ERROR ("invalid global SAPI value");
            break;
        }
      } /* end of validity range of ll_unitready_ind */
      break;
    case UITX_TLLI_UNASSIGNED_READY:
      /*
       * NOTE: This signal is always send when enough space for one more 
       * frame is available in the local transmit buffer queue of TX. 
       * Therefore it can be safely ignored if UITX is already in 
       * state *_READY.
       */
      break;
    case UITX_ADM_NOT_READY:
      SET_STATE (UITX, UITX_ADM_READY);
        
      if (llc_data->current_sapi != LL_SAPI_1)
      {
        PALLOC (ll_unitready_ind, LL_UNITREADY_IND);
        
        ll_unitready_ind->sapi = llc_data->current_sapi;
        
        /*
         * Send primitive LL-UNITREADY-IND to either SNDCP or GSMS, 
         * depending on the SAPI.
         */
        switch (llc_data->current_sapi)
        {
          case LL_SAPI_3:
          case LL_SAPI_5:
          case LL_SAPI_9:
          case LL_SAPI_11:
            TRACE_1_OUT_PARA("s:%d", ll_unitready_ind->sapi);            
            PSEND (hCommSNDCP, ll_unitready_ind);
            break;
          case LL_SAPI_7:
#ifndef LL_2to1
            TRACE_PRIM_TO("GSMS");
            TRACE_1_OUT_PARA("s:%d", ll_unitready_ind->sapi);            
            PSEND (hCommGSMS, ll_unitready_ind);
#else
            PFREE (ll_unitready_ind);
#endif
            break;
          default:
            PFREE (ll_unitready_ind);
            TRACE_ERROR ("invalid global SAPI");
            break;
        }
      } /* end of validity range of ll_unitready_ind */
      break;
    case UITX_ADM_READY:
      /*
       * NOTE: This signal is always send when enough space for one more 
       * frame is available in the local transmit buffer queue of TX. 
       * Therefore it can be safely ignored if UITX is already in 
       * state *_READY.
       */
      break;
    default:
      TRACE_ERROR( "SIG_TX_UITX_READY_IND unexpected" );
      break;
  }
} /* sig_tx_uitx_ready_ind() */


/*
+------------------------------------------------------------------------------
| Function    : sig_tx_uitx_trigger_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TX_UITX_TRIGGER_IND
|
| Parameters  : cause - trigger cause
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tx_uitx_trigger_ind (UBYTE cause) 
{ 
  T_FRAME_NUM                 used_nu;
  ULONG                       used_oc;
  UBYTE                       cipher_mode;
#ifdef LL_DESC
  T_desc3 *desc3;
#endif


  TRACE_ISIG( "sig_tx_uitx_trigger_ind" );

  switch (GET_STATE(UITX))
  {
    case UITX_ADM_READY:
      /*
       * NOTE: llc_current_sapi is assumed to be LL_SAPI_1, when this signal 
       * is being received. Thus it is an error if UITX is not in state 
       * ADM_READY, because that means that the local transmit queue in TX for 
       * SAPI 1 is not big enough!
       */
      {

#ifndef LL_DESC 
        PALLOC_SDU (ll_unitdesc_req, LL_UNITDATA_REQ, UI_FRAME_MIN_OCTETS*8);

#else
        PALLOC (ll_unitdesc_req, LL_UNITDESC_REQ);
        desc3 = llc_palloc_desc(0, UI_FRAME_MIN_OCTETS);
#endif

        ll_unitdesc_req->sapi             = llc_data->current_sapi;
        ll_unitdesc_req->tlli             = llc_data->tlli_new;
        ll_unitdesc_req->ll_qos.delay     = LL_DELAY_SUB;
        ll_unitdesc_req->ll_qos.relclass  = LL_NO_REL;
        ll_unitdesc_req->ll_qos.peak      = LL_PEAK_SUB;
        ll_unitdesc_req->ll_qos.preced    = LL_PRECED_SUB;
        ll_unitdesc_req->ll_qos.mean      = LL_MEAN_SUB;
        ll_unitdesc_req->radio_prio       = LL_RADIO_PRIO_1;

#ifdef REL99 
        /* 
         * From 24.008 & 23.060 it is interpreted that for all signalling data, a 
         * predefined PFI LL_PFI_SIGNALING shall be used.
         */
        ll_unitdesc_req->pkt_flow_id = LL_PFI_SIGNALING;
#endif  /* REL99 */

        cipher_mode = LL_CIPHER_OFF;

#ifndef LL_DESC

         /*
         * No attach to primitive necessary. There is no retransmission.
         */
        ll_unitdesc_req->attached_counter = CCI_NO_ATTACHE;

        ll_unitdesc_req->sdu.l_buf        = 0;
        ll_unitdesc_req->sdu.o_buf        = UI_CTRL_MIN_OCTETS * 8;

        /*
         * uitx_build_ui_header expects SDU to already contain data. The
         * header is inserted _before_ sdu.o_buf, so set o_but to the length
         * of the header.
         */
        uitx_build_ui_header (&ll_unitdesc_req->sdu, MS_COMMAND, 
          llc_data->current_sapi, llc_data->uitx->vu, cipher_mode,
          ll_unitdesc_req->ll_qos.relclass); 
#else
        ll_unitdesc_req->attached_counter = CCI_NO_ATTACHE;
        ll_unitdesc_req->desc_list3.first       = (ULONG)desc3;
        ll_unitdesc_req->desc_list3.list_len    = desc3->len;

        uitx_build_ui_header (&ll_unitdesc_req->desc_list3, MS_COMMAND, 
          llc_data->current_sapi, llc_data->uitx->vu, cipher_mode,
          ll_unitdesc_req->ll_qos.relclass); 
#endif
        used_nu = llc_data->uitx->vu;
        used_oc = llc_data->sapi->oc_ui_tx;

        llc_data->uitx->vu ++;

        if (llc_data->uitx->vu > MAX_SEQUENCE_NUMBER)
        {
          llc_data->uitx->vu        = 0;
          llc_data->sapi->oc_ui_tx += (MAX_SEQUENCE_NUMBER+1);
        }
        sig_uitx_tx_data_req (ll_unitdesc_req, cipher_mode, cause, used_nu, used_oc);
      }
      break;

    default:
      TRACE_ERROR( "SIG_TX_UITX_TRIGGER_IND unexpected" );
      break;
  }



} /* sig_tx_uitx_trigger_ind() */

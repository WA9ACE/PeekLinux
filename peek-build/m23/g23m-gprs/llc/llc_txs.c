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
|             described in the SDL-documentation (TX-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#ifndef LLC_TXS_C
#define LLC_TXS_C
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

#include "llc_txf.h"    /* to get local TX functions */
#include "llc_txl.h"    /* to get local TX labels */
#include "llc_uitxs.h"  /* to get signal interface to UITX */
#include "llc_itxs.h"   /* to get signal interface to ITX */

#ifdef REL99 
#include "llc_uf.h"     /* to get interface to U control frame */
#endif /* REL99 */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/


/*
+------------------------------------------------------------------------------
| Function    : sig_llme_tx_assign_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_LLME_TX_ASSIGN_REQ
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_llme_tx_assign_req (void)
{
  T_SAPI sapi;


  TRACE_ISIG( "sig_llme_tx_assign_req" );

  switch( GET_STATE( TX ) )
  {
    case TX_TLLI_UNASSIGNED_NOT_READY:
      SET_STATE (TX, TX_TLLI_ASSIGNED_NOT_READY);

      /*
       * ATTENTION:
       * The following loops implies that all SAPIs are odd numbers beginning
       * with 1!
       */
      for (sapi = LL_SAPI_1; sapi <= LL_SAPI_11; sapi += 2)
      {
        SWITCH_SERVICE (llc, uitx, UIMAP(sapi));
        llc_data->current_sapi = sapi;

        sig_tx_uitx_ready_ind();
      }

      for (sapi = LL_SAPI_3; sapi <= LL_SAPI_11; sapi += 2)
      {
        SWITCH_SERVICE (llc, itx, IMAP(sapi));
        llc_data->current_sapi = sapi;

        if (sapi != LL_SAPI_7)
        {
          sig_tx_itx_ready_ind();
        }
      }
      break;
    case TX_TLLI_UNASSIGNED_READY:
      SET_STATE (TX, TX_TLLI_ASSIGNED_READY);

      /*
       * ATTENTION:
       * The following loops implies that all SAPIs are odd numbers beginning
       * with 1!
       */
      for (sapi = LL_SAPI_1; sapi <= LL_SAPI_11; sapi += 2)
      {
        SWITCH_SERVICE (llc, uitx, UIMAP(sapi));
        llc_data->current_sapi = sapi;

        sig_tx_uitx_ready_ind();
      }

      for (sapi = LL_SAPI_3; sapi <= LL_SAPI_11; sapi += 2)
      {
        SWITCH_SERVICE (llc, itx, IMAP(sapi));
        llc_data->current_sapi = sapi;

        if (sapi != LL_SAPI_7)
        {
          sig_tx_itx_ready_ind();
        }
      }
      break;

    default:
      TRACE_ERROR( "SIG_LLME_TX_ASSIGN_REQ unexpected" );
      break;
  }

  return;
} /* sig_llme_tx_assign_req() */


/*
+------------------------------------------------------------------------------
| Function    : sig_llme_tx_unassign_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_LLME_TX_UNASSIGN_REQ
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_llme_tx_unassign_req (void)
{
  TRACE_ISIG( "sig_llme_tx_unassign_req" );

  switch( GET_STATE( TX ) )
  {
    case TX_TLLI_ASSIGNED_NOT_READY:
      tx_clear_buffer();
      SET_STATE (TX, TX_TLLI_UNASSIGNED_NOT_READY);
      break;

    case TX_TLLI_ASSIGNED_READY:
      tx_clear_buffer();
      SET_STATE (TX, TX_TLLI_UNASSIGNED_READY);
      break;

    default:
      TRACE_ERROR( "SIG_LLME_TX_UNASSIGN_REQ unexpected" );
      break;
  }

  return;
} /* sig_llme_tx_unassign_req() */


/*
+------------------------------------------------------------------------------
| Function    : sig_llme_tx_reset_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_LLME_TX_RESET_REQ
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_llme_tx_reset_req (void)
{
  TRACE_ISIG( "sig_llme_tx_reset_req" );

  switch( GET_STATE( TX ) )
  {
    case TX_TLLI_ASSIGNED_NOT_READY:
      /*
       * No break!
       */
    case TX_TLLI_ASSIGNED_READY:
      tx_clear_buffer();
      //sig_tx_uitx_ready_ind();
      break;

    default:
      TRACE_ERROR( "SIG_LLME_TX_RESET_REQ unexpected" );
      break;
  }

  return;
} /* sig_llme_tx_reset_req() */


/*
+------------------------------------------------------------------------------
| Function    : sig_llme_tx_ready_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_LLME_TX_READY_REQ
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_llme_tx_ready_req (void)
{
  TRACE_ISIG( "sig_llme_tx_ready_req" );

  switch( GET_STATE( TX ) )
  {
    case TX_TLLI_ASSIGNED_NOT_READY:
      /*
       * No break!
       */
    case TX_TLLI_ASSIGNED_READY:
      sig_tx_uitx_ready_ind();
      break;

    default:
      TRACE_ERROR( "SIG_LLME_TX_READY_REQ unexpected" );
      break;
  }

  return;
} /* sig_llme_tx_ready_req() */




/*
+------------------------------------------------------------------------------
| Function    : sig_u_tx_data_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_U_TX_DATA_REQ
|
| Parameters  : ll_unitdata_req - a valid pointer to a GRLC-UNITDATA-REQ 
|                                 primitive
|               cause           - RLC/MAC cause
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_u_tx_data_req 
(
#ifdef LL_DESC
 T_LL_UNITDESC_REQ *ll_unitdesc_req,
#else
 T_LL_UNITDATA_REQ *ll_unitdesc_req,
#endif
 UBYTE cause
)
{
  ULONG             reservation_no;
  BOOL              buffer_available;   
  UBYTE             protected_mode;

  TRACE_ISIG( "sig_u_tx_data_req" );
  
  switch( GET_STATE( TX ) )
  {
    case TX_TLLI_ASSIGNED_NOT_READY:
      /* No break. */
    case TX_TLLI_ASSIGNED_READY:

      /*
       * U frames are always sent with default cause! except cell notification.
       */
#ifdef REL99 
      if (cause EQ GRLC_DTACS_CELL_NOTIFI_NULL_FRAME)
      {
        tx_reserve_buffer (ll_unitdesc_req, PRIM_DATA, cause, 
                          SERVICE_U, &reservation_no, &buffer_available);
      }
      else
#endif /* REL99 */
      {
        tx_reserve_buffer (ll_unitdesc_req, PRIM_DATA, GRLC_DTACS_DEF, 
                          SERVICE_U, &reservation_no, &buffer_available);
      }
      /*
       * U frames are always sent in LLC protected mode.
       */
      protected_mode = CCI_PM_PROTECTED;

      {
#ifdef LL_DESC 
        T_CCI_CIPHER_DESC_REQ *cci_cipher_desc_req;
        MALLOC(cci_cipher_desc_req, sizeof(T_CCI_CIPHER_DESC_REQ));
        cci_cipher_desc_req->desc_list3.first = ll_unitdesc_req->desc_list3.first;
        cci_cipher_desc_req->desc_list3.list_len = ll_unitdesc_req->desc_list3.list_len;
        cci_cipher_desc_req->attached_counter = ll_unitdesc_req->attached_counter;
        /*
         * decrease attached counter. If no one is still attached
         * free the primitive memory
         */
        if (ll_unitdesc_req->attached_counter == CCI_NO_ATTACHE)
        {
          PFREE(ll_unitdesc_req);
        } else {
          TRACE_0_INFO("LL_UNITDESC_REQ still attached");
        }
#else
        PPASS (ll_unitdesc_req, cci_cipher_desc_req, CCI_CIPHER_REQ);
#endif
        /*
         * Header size (offset of information) is reqired, if ciphering is used
         * or if the frame shall be send in unprotected mode. In this case both
         * is not true and so it can be ignored.
         */
        cci_cipher_desc_req->header_size = 0;

        /*
         * Associate reservation_no with the frame.
         */
        cci_cipher_desc_req->reference1 = reservation_no;

        /*
         * U frames are never sent ciphered, thus define LL_CIPHER_OFF and set
         * N(S) to 0 (it is not used).
         */
        tx_send_cipher_req (cci_cipher_desc_req, U_FRAME, protected_mode, 0,
          LL_CIPHER_OFF, 0);

      }
      break;

    default:
#ifdef LL_DESC
      llc_cl_desc3_free((T_desc3*)ll_unitdesc_req->desc_list3.first);
#endif /* LL_DESC */
      PFREE (ll_unitdesc_req);

      TRACE_ERROR( "SIG_U_TX_DATA_REQ unexpected" );
      break;
  }
} /* sig_u_tx_data_req() */


/*
+------------------------------------------------------------------------------
| Function    : sig_uitx_tx_data_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_UITX_TX_DATA_REQ
|
| Parameters  : ll_unitdata_req - a valid pointer to a GRLC-UNITDATA-REQ 
|                                 primitive
|               cipher - indicates if the frame shall be ciphered or not
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_uitx_tx_data_req 
  (
#ifdef LL_DESC
  T_LL_UNITDESC_REQ *ll_unitdesc_req,
#else
  T_LL_UNITDATA_REQ *ll_unitdesc_req,
#endif
  UBYTE             cipher,
  UBYTE             cause,
  T_FRAME_NUM       nu,
  ULONG             oc
  )
 
{
  ULONG             reservation_no;
  BOOL              buffer_available;
  UBYTE             protected_mode;


  TRACE_ISIG( "sig_uitx_tx_data_req" );
  
  switch( GET_STATE( TX ) )
  {
    case TX_TLLI_ASSIGNED_NOT_READY:
      /* No break. */
    case TX_TLLI_ASSIGNED_READY:

      tx_reserve_buffer (ll_unitdesc_req, PRIM_DATA, cause, SERVICE_UITX,
        &reservation_no, &buffer_available);

      if (ll_unitdesc_req->ll_qos.relclass EQ LL_NO_REL)
      {
        protected_mode = CCI_PM_UNPROTECTED;
      }
      else /* reliability classes 4 - 1 */
      {
        protected_mode = CCI_PM_PROTECTED;
      }

      {
#ifdef LL_DESC        
        T_CCI_CIPHER_DESC_REQ *cci_cipher_desc_req;
        MALLOC(cci_cipher_desc_req, sizeof(T_CCI_CIPHER_DESC_REQ));
        cci_cipher_desc_req->desc_list3.first = ll_unitdesc_req->desc_list3.first;
        cci_cipher_desc_req->desc_list3.list_len = ll_unitdesc_req->desc_list3.list_len;
        cci_cipher_desc_req->attached_counter = ll_unitdesc_req->attached_counter;
        /*
         * decrease attached counter. If no one is still attached
         * free the primitive memory
         */
        if (ll_unitdesc_req->attached_counter == CCI_NO_ATTACHE)
        {
          PFREE(ll_unitdesc_req);
        } else {
          TRACE_0_INFO("LL_UNITDESC_REQ still attached");
        }
#else
        PPASS (ll_unitdesc_req, cci_cipher_desc_req, CCI_CIPHER_REQ);
#endif
        /*
         * Header size (offset of information) is reqired, if ciphering is used
         * or if the frame shall be send in unprotected mode. 
         */
        cci_cipher_desc_req->header_size = UI_CTRL_MIN_OCTETS;

        /*
         * Associate reservation_no with the frame.
         */
        cci_cipher_desc_req->reference1 = reservation_no;

        tx_send_cipher_req (cci_cipher_desc_req, UI_FRAME, protected_mode, nu, 
          cipher, oc);

        if (buffer_available)
        {
          /*
           * Buffer is still available for current SAPI.
           */
          sig_tx_uitx_ready_ind();
        }

      }
      break;

    default:
#ifdef LL_DESC
      llc_cl_desc3_free((T_desc3*)ll_unitdesc_req->desc_list3.first);
#endif /* LL_DESC */
      PFREE (ll_unitdesc_req);

      TRACE_ERROR( "SIG_UITX_TX_DATA_REQ unexpected" );
      break;
  }
} /* sig_uitx_tx_data_req() */


/*
+------------------------------------------------------------------------------
| Function    : sig_uitx_tx_unitdata_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_UITX_TX_UNITDATA_REQ
|
| Parameters  : ll_unitdata_req - a valid pointer to a GRLC-UNITDATA-REQ 
|                                 primitive
|               cipher - indicates if the frame shall be ciphered or not
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_uitx_tx_unitdata_req
(
#ifdef LL_DESC
  T_LL_UNITDESC_REQ *ll_unitdesc_req, 
#else
  T_LL_UNITDATA_REQ *ll_unitdesc_req,
#endif  
  UBYTE             cipher,
  T_FRAME_NUM       nu,
  ULONG             oc
) 
  
{
  ULONG             reservation_no;
  BOOL              buffer_available;
  UBYTE             protected_mode;


  TRACE_ISIG( "sig_uitx_tx_unitdata_req" ); 
  
  switch( GET_STATE( TX ) )
  {
    case TX_TLLI_ASSIGNED_NOT_READY:
      /* No break. */
    case TX_TLLI_ASSIGNED_READY:

      /*
       * Parameter cause will be ignored for RLC/MAC unacknowledged frames,
       * so set it to 0.
       */
      tx_reserve_buffer (ll_unitdesc_req, PRIM_UNITDATA, 0, SERVICE_UITX,
        &reservation_no, &buffer_available);

      if (ll_unitdesc_req->ll_qos.relclass EQ LL_NO_REL)
      {
        protected_mode = CCI_PM_UNPROTECTED;
      }
      else /* reliability classes 4 - 1 */
      {
        protected_mode = CCI_PM_PROTECTED;
      }

      {
#ifdef LL_DESC
        T_CCI_CIPHER_DESC_REQ *cci_cipher_desc_req;
        MALLOC(cci_cipher_desc_req, sizeof(T_CCI_CIPHER_DESC_REQ));
        cci_cipher_desc_req->desc_list3.first = ll_unitdesc_req->desc_list3.first;
        cci_cipher_desc_req->desc_list3.list_len = ll_unitdesc_req->desc_list3.list_len;
        cci_cipher_desc_req->attached_counter = ll_unitdesc_req->attached_counter;
        /*
         * decrease attached counter. If no one is still attached
         * free the primitive memory
         */
        if (ll_unitdesc_req->attached_counter == CCI_NO_ATTACHE)
        {
          PFREE(ll_unitdesc_req); 
        } else {
          TRACE_0_INFO("LL_UNITDESC_REQ still attached");
        }
#else
        PPASS (ll_unitdesc_req, cci_cipher_desc_req, CCI_CIPHER_REQ);
#endif
        /*
         * Header size (offset of information) is reqired, if ciphering is used
         * or if the frame shall be send in unprotected mode. 
         */
        cci_cipher_desc_req->header_size = UI_CTRL_MIN_OCTETS;

        /*
         * Associate reservation_no with the frame.
         */
        cci_cipher_desc_req->reference1 = reservation_no;

        tx_send_cipher_req (cci_cipher_desc_req, UI_FRAME, protected_mode, nu, 
          cipher, oc);

        if (buffer_available)
        {
          /*
           * Buffer is still available for current SAPI.
           */
          sig_tx_uitx_ready_ind();
        }

      }
      break;

    default:
#ifdef LL_DESC
      llc_cl_desc3_free((T_desc3*)ll_unitdesc_req->desc_list3.first);
#endif /* LL_DESC */
      PFREE (ll_unitdesc_req);

      TRACE_ERROR( "SIG_UITX_TX_UNITDATA_REQ unexpected" );
      break;
  }
} /* sig_uitx_tx_unitdata_req() */


/*
+------------------------------------------------------------------------------
| Function    : sig_itx_tx_data_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ITX_TX_DATA_REQ
|
| Parameters  : ll_data_req - a valid pointer to a GRLC-DATA-REQ primitive
|               cause       - 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_itx_tx_data_req
(
#ifdef LL_DESC
  T_LL_UNITDESC_REQ *ll_unitdesc_req, 
#else
  T_LL_UNITDATA_REQ *ll_unitdesc_req, 
#endif
  T_PDU_TYPE        frame_type,
  T_FRAME_NUM       ns,
  UBYTE             cause,
  USHORT            header_size,
  ULONG             oc
)
{
  ULONG             reservation_no;
  BOOL              buffer_available;
  UBYTE             protected_mode;

  TRACE_ISIG( "sig_itx_tx_data_req" );
  
  switch( GET_STATE( TX ) )
  {
    case TX_TLLI_ASSIGNED_NOT_READY:
      /* No break. */
    case TX_TLLI_ASSIGNED_READY:

      tx_reserve_buffer (ll_unitdesc_req, PRIM_DATA, cause, SERVICE_ITX,
        &reservation_no, &buffer_available);

      /*
       * I/S + S frames always send in protected mode
       */
      protected_mode = CCI_PM_PROTECTED;

      {
#ifdef LL_DESC
        T_CCI_CIPHER_DESC_REQ *cci_cipher_desc_req;
        MALLOC(cci_cipher_desc_req, sizeof(T_CCI_CIPHER_DESC_REQ));
        cci_cipher_desc_req->desc_list3.first = ll_unitdesc_req->desc_list3.first;
        cci_cipher_desc_req->desc_list3.list_len = ll_unitdesc_req->desc_list3.list_len;
        cci_cipher_desc_req->attached_counter = ll_unitdesc_req->attached_counter;
        /*
         * free the primitive memory
         */
        PFREE(ll_unitdesc_req); 
#else
        PPASS (ll_unitdesc_req, cci_cipher_desc_req, CCI_CIPHER_REQ);
#endif
        /*
         * Header size (offset of information) is reqired, if ciphering is used
         * or if the frame shall be send in unprotected mode. 
         */
        cci_cipher_desc_req->header_size = header_size;

        /*
         * Associate reservation_no with the frame.
         */
        cci_cipher_desc_req->reference1 = reservation_no;

        if (frame_type == S_FRAME)
        {
          /*
           * Do not try to cipher an S frame (there is no information ;-)
           */
          tx_send_cipher_req (cci_cipher_desc_req, frame_type, protected_mode, ns, 
            LL_CIPHER_OFF, oc);
        }
        else
        {
          /*
           * I frames shall always send ciphered
           */
          tx_send_cipher_req (cci_cipher_desc_req, frame_type, protected_mode, ns, 
            LL_CIPHER_ON, oc);
        }

        if (buffer_available)
        {
          /*
           * Buffer is still available for current SAPI.
           */
          sig_tx_itx_ready_ind();
        }

      }
      break;

    default:
      if (ll_unitdesc_req->attached_counter == CCI_NO_ATTACHE)
      {
#ifdef LL_DESC
        llc_cl_desc3_free((T_desc3*)ll_unitdesc_req->desc_list3.first);
#endif /* LL_DESC */
        PFREE (ll_unitdesc_req);
      }
      TRACE_ERROR( "SIG_ITX_TX_DATA_REQ unexpected" );
      break;
  }
} /* sig_itx_tx_data_req() */


/*
+------------------------------------------------------------------------------
| Function    : sig_llme_tx_trigger_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_LLME_TX_TRIGGER_REQ
|
| Parameters  : cause - trigger cause
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_llme_tx_trigger_req (UBYTE cause)
{
  T_TX_QUEUE                  *elem;


  TRACE_ISIG( "sig_llme_tx_trigger_req" );
  
  TRACE_1_PARA("cause:%d", cause);

  switch( GET_STATE( TX ) )
  {
    case TX_TLLI_ASSIGNED_NOT_READY:
      /* no break! */
    case TX_TLLI_ASSIGNED_READY:
      tx_get_first_data_frame (&elem);

      if (elem EQ NULL)
      {
#ifdef REL99 
        if (cause EQ LLGMM_TRICS_CELL_UPDATE_NULL_FRAME)
        {
          cause = GRLC_DTACS_CELL_NOTIFI_NULL_FRAME;
          sig_tx_u_send_null(cause);
        }
        else 
#endif /* REL99 */      
        if (cause EQ LLGMM_TRICS_CELL_UPDATE)
        {
           cause = GRLC_DTACS_EMPTY_FRAME;
           sig_tx_uitx_trigger_ind (cause);
        }
        else
        {
           sig_tx_uitx_trigger_ind (cause);
        }
      }
      else /* elem is valid */
      {
        /*
         * NOTE: Possible cause values are DEFAULT, MOBILITY_MANAGEMENT, 
         * PAGE_RESPONSE, CELL_UPDATE and CELL NOTIFICATION.
         * Overwriting DEFAULT, MOBILITY_MANAGEMENT, and the same cause 
         * (which is already set) is ok. This leaves two relevant (and 
         * mutually exclusive) trigger causes to be considered. A problem may 
         * occur if the primitive contained the one remaining trigger cause 
         * and the other is requested. But according to ANS this should not 
         * happen.
         */
        
        if ((cause EQ LLGMM_TRICS_CELL_UPDATE)
#ifdef REL99 
               OR
            (cause EQ LLGMM_TRICS_CELL_UPDATE_NULL_FRAME)
#endif /*REL99*/
            )
        {
          cause = GRLC_DTACS_DEF;
        }

        if (elem->primitive EQ NULL)
        {
          /*
           * Primitive is not yet ready-to-send. Modify cause in primitve 
           * header variable.
           */
          elem->ph_cause = cause;
        }
        else /* elem->primitive is valid */
        {
          /*
           * Primitive is ready-to-send. Modifiy cause in primitive.
           */
          ((T_GRLC_DATA_REQ *)(elem->primitive))->cause = cause;
        }
      }
      break;

    default:
      TRACE_ERROR( "SIG_LLME_TX_TRIGGER_REQ unexpected" );
      break;
  }
} /* sig_llme_tx_trigger_req() */


/*
+------------------------------------------------------------------------------
| Function    : sig_llme_tx_suspend_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_LLME_TX_SUSPEND_REQ
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_llme_tx_suspend_req (void)
{
  TRACE_ISIG( "sig_llme_tx_suspend_req" );
  
  switch( GET_STATE( TX ) )
  {
    case TX_TLLI_UNASSIGNED_NOT_READY:
    case TX_TLLI_ASSIGNED_NOT_READY:
      /*
       * Nothing to do
       */
      break;

    case TX_TLLI_ASSIGNED_READY:
      /*
       * Flow Control has to be re-started by GRLC
       */
      SET_STATE (TX, TX_TLLI_ASSIGNED_NOT_READY);
      break;

    case TX_TLLI_UNASSIGNED_READY:
      /*
       * Flow Control has to be re-started by GRLC
       */
      SET_STATE (TX, TX_TLLI_UNASSIGNED_NOT_READY);
      break;

    default:
      TRACE_ERROR( "SIG_LLME_TX_SUSPEND_REQ unexpected" );
      break;
  }
} /* sig_llme_tx_suspend_req() */

/*
+------------------------------------------------------------------------------
| Function    : sig_llme_tx_resume_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_LLME_TX_RESUME_REQ
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_llme_tx_resume_req (BOOL grlc_was_suspened)
{
  T_GRLC_DATA_REQ              *grlc_data_req;
  T_GRLC_UNITDATA_REQ          *grlc_unitdata_req;
  T_PRIM_TYPE                 prim_type;
  T_SERVICE                   rx_service;
  UBYTE                       sapi;

  TRACE_ISIG( "sig_llme_tx_resume_req" );
  
  switch( GET_STATE( TX ) )
  {
    case TX_TLLI_UNASSIGNED_NOT_READY:
    case TX_TLLI_ASSIGNED_NOT_READY:
      /*
       * Nothing to do
       */
      break;

    case TX_TLLI_ASSIGNED_READY:
      if (grlc_was_suspened == FALSE)
      {
        /*
         * LLME indicates that any data may now be sent again.
         */
        do {
          tx_get_next_frame (&grlc_data_req, &grlc_unitdata_req, &prim_type, 
            &rx_service, &sapi);

          switch (prim_type)
          {
            case PRIM_DATA:
              /*
               * Label S_DATA
               */
              tx_label_s_data (rx_service, grlc_data_req);
              break;
            case PRIM_UNITDATA:
              /*
               * Label S_UNITDATA
               */
              tx_label_s_unitdata (rx_service, grlc_unitdata_req);
              break;
            case PRIM_REMOVED:
              /*
               * Label S_REMOVED
               */
              tx_label_s_removed (rx_service, sapi);
              break;
            default: /* NO_PRIM */
              break;
          }
        } while (prim_type == PRIM_REMOVED);
      }
      else
      {
        /*
         * Flow Control has to be re-started by GRLC
         */
        SET_STATE (TX, TX_TLLI_ASSIGNED_NOT_READY);
      }
      break;

    case TX_TLLI_UNASSIGNED_READY:
      /*
       * Flow Control has to be re-started by GRLC
       */
      SET_STATE (TX, TX_TLLI_UNASSIGNED_NOT_READY);
      break;

    default:
      TRACE_ERROR( "SIG_LLME_TX_RESUME_REQ unexpected" );
      break;
  }
} /* sig_llme_tx_resume_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_llme_tx_flush_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_LLME_TX_FLUSH_REQ
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_llme_tx_flush_req (T_SERVICE service)
{
  TRACE_ISIG( "sig_llme_tx_flush_req" );
  
  switch( GET_STATE( TX ) )
  {
    case TX_TLLI_ASSIGNED_READY:
    case TX_TLLI_ASSIGNED_NOT_READY:
      tx_remove_data_frames (service, llc_data->current_sapi);
      break;

    default:
      TRACE_ERROR( "SIG_LLME_TX_FLUSH_REQ unexpected" );
      break;
  }
} /* sig_llme_tx_flush_req() */


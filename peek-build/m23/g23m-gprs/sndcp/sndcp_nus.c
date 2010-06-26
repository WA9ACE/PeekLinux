/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  sndcp_nus.c
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
|             functions to handles the incoming process internal signals as  
|             described in the SDL-documentation (NU-statemachine)
+----------------------------------------------------------------------------- 
*/ 

/*---- HISTORY --------------------------------------------------------------*/

#define ENTITY_SNDCP

/*==== INCLUDES =============================================================*/

#include "typedefs.h"    /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "prim.h"       /* to get the definitions of used SAP and directions */

#include "dti.h"

#include "sndcp.h"        /* to get the global entity definitions */
#include "sndcp_f.h"       /* to get the functions to access the global arrays*/

#include "sndcp_nuf.h"      /* to get internal functions for service nu. */
#include "sndcp_suas.h"      /* to get signals to sua. */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : nu_send_pending
+------------------------------------------------------------------------------
| Description : Re-sends the next not yet acknowledged N-PDU. If this was the
| last one, ready is set to TRUE, otherwise to FALSE.
| Ready is also set to TRUE, if there is no pending NPDU at all.
|
| Parameters  : ready
| Pre         : correct instance of nu service is active.
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

LOCAL void nu_send_pending (BOOL* ready) 
{ 
  T_SN_DATA_REQ* sn_data_req = NULL;
  UBYTE npdu_number = 0;
  UBYTE sapi = 0;

  TRACE_FUNCTION( "nu_send_pending" );

  if (sndcp_data->nu->next_resend == NULL) {
    *ready = TRUE;
    return;
  }
  sn_data_req = sndcp_data->nu->next_resend->sn_data_req;
  sndcp_get_nsapi_sapi(sndcp_data->nu->nsapi, &sapi);
  npdu_number = sndcp_data->nu->next_resend->npdu_number;


  /*
   * Set next_resend to the next one if possible.
   */
  if (sndcp_data->nu->next_resend->next == NULL) {
    *ready = TRUE;
    SET_STATE(NU, NU_ACK_SUA_NOT_RECEPTIVE);
  } else {
    *ready = FALSE;
    sndcp_data->nu->next_resend = sndcp_data->nu->next_resend->next;
    SET_STATE(NU, NU_REC_SUA_NOT_RECEPTIVE);
  }

  sig_nu_sua_data_req(sn_data_req,
                      npdu_number,
                      sndcp_data->nu->nsapi,
                      sapi);

} /* nu_send_pending() */

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : nu_config_send_pending
+------------------------------------------------------------------------------
| Description : Re-sends the next not yet acknowledged N-PDU. If this was the
| last one, ready is set to TRUE, otherwise to FALSE.
| Ready is also set to TRUE, if there is no pending NPDU at all.
| The same function as nu_send_pending, but for tests with config send.
|
| Parameters  : ready
| Pre         : correct instance of nu service is active.
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC
#ifndef NCONFIG

LOCAL void nu_config_send_pending (BOOL* ready) 
{ 
  T_SN_DATA_REQ* sn_data_req = NULL;
  UBYTE npdu_number = 0;
  UBYTE sapi = 0;

  TRACE_FUNCTION( "nu_config_send_pending" );

  if (sndcp_data->nu->next_resend == NULL) {
    *ready = TRUE;
    return;
  }
  sn_data_req = sndcp_data->nu->next_resend->sn_data_req;
  sndcp_get_nsapi_sapi(sndcp_data->nu->nsapi, &sapi);
  npdu_number = sndcp_data->nu->next_resend->npdu_number;

  /*
   * Set next_resend to the next one if possible.
   */
  if (sndcp_data->nu->next_resend->next == NULL) {
    *ready = TRUE;
    SET_STATE(NU, NU_SEND_ACK_SUA_NOT_RECEPTIVE);
  } else {
    *ready = FALSE;
    sndcp_data->nu->next_resend = sndcp_data->nu->next_resend->next;
    SET_STATE(NU, NU_SEND_REC_SUA_NOT_RECEPTIVE);
  }
  sig_nu_sua_data_req(sn_data_req,
                      npdu_number,
                      sndcp_data->nu->nsapi,
                      sapi);

} /* nu_config_send_pending() */

#endif /* NCONFIG */
#endif /* CF_FAST_EXEC */

/*==== PUBLIC FUNCTIONS =====================================================*/


/*
+------------------------------------------------------------------------------
| Function    : sig_mg_nu_delete_npdus
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_MG_NU_DELETE_NPDUS. All the
|               buffered N-PDUs waiting for confirmation are deleted.
|
| Parameters  : UBYTE nsapi: the affected NSAPI
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sig_mg_nu_delete_npdus (UBYTE nsapi) 
{ 
  TRACE_ISIG( "sig_mg_nu_delete_npdus" );
  /*
   * set service instance according to nsapi in primitive
   */
  sndcp_data->nu = & sndcp_data->nu_base[nsapi];
  
  switch( GET_STATE( NU ) )
  {
    case NU_ACK_SUA_NOT_RECEPTIVE:
    case NU_ACK_SUA_RECEPTIVE:
    case NU_SUS_SUA_NOT_RECEPTIVE:
    case NU_SUS_SUA_RECEPTIVE:
    case NU_REC_SUA_NOT_RECEPTIVE:
      nu_delete_npdus();
      break;

    case NU_UNACK_SU_RECEPTIVE :
    case NU_UNACK_SU_NOT_RECEPTIVE:
#ifndef NCONFIG
    case NU_SEND_UNACK_SU_RECEPTIVE:
    case NU_SEND_UNACK_SU_NOT_RECEPTIVE:
    case NU_SEND_ACK_SUA_RECEPTIVE:
    case NU_SEND_ACK_SUA_NOT_RECEPTIVE:
    case NU_SEND_SUS_SUA_RECEPTIVE:
    case NU_SEND_SUS_SUA_NOT_RECEPTIVE:
    case NU_SEND_REC_SUA_NOT_RECEPTIVE:
#endif
      break;
    default:
      TRACE_ERROR( "SIG_SU_NU_DELETE_NPDUS unexpected" );
      break;
  }
} /* sig_mg_nu_delete_npdus() */

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sig_mg_nu_delete_to
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_MG_NU_DELETE_TO. All the
|               buffered N-PDUs waiting for confirmation are deleted until the
|               N-PDU with the given number is reached, beacuse this is the one
|               that is expected atthe peer side..
|
| Parameters  : naspi, npdu_num
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sig_mg_nu_delete_to (UBYTE nsapi, UBYTE npdu_num) 
{ 
  T_NPDU_BUFFER* help = NULL;
  /*
   * Has npdu number been found?
   */
  BOOL found = FALSE;

  TRACE_ISIG( "sig_mg_nu_delete_to" );

  /*
   * set service instance according to nsapi in primitive
   */
  sndcp_data->nu = & sndcp_data->nu_base[nsapi];
  
  switch( GET_STATE( NU ) )
  {
    case NU_ACK_SUA_NOT_RECEPTIVE:
    case NU_ACK_SUA_RECEPTIVE:
    case NU_SUS_SUA_NOT_RECEPTIVE:
    case NU_SUS_SUA_RECEPTIVE:
    case NU_REC_SUA_NOT_RECEPTIVE:
    
      help = sndcp_data->nu->first_buffered_npdu;
      while (help != NULL && ! found) {
        T_NPDU_BUFFER* buf = help;

        help = help->next;
        if (buf->npdu_number == npdu_num) {
          found = TRUE;
        } else {
          nu_delete_npdu(nsapi, buf->npdu_number);
        }
      }
      break;
    case NU_UNACK_SU_RECEPTIVE :
    case NU_UNACK_SU_NOT_RECEPTIVE:
#ifndef NCONFIG
    case NU_SEND_UNACK_SU_RECEPTIVE:
    case NU_SEND_UNACK_SU_NOT_RECEPTIVE:
    case NU_SEND_ACK_SUA_RECEPTIVE:
    case NU_SEND_ACK_SUA_NOT_RECEPTIVE:
    case NU_SEND_SUS_SUA_RECEPTIVE:
    case NU_SEND_SUS_SUA_NOT_RECEPTIVE:
    case NU_SEND_REC_SUA_NOT_RECEPTIVE:
#endif
      break;
    default:
      TRACE_ERROR( "SIG_MG_NU_DELETE_TO unexpected" );
      break;
  }
} /* sig_mg_nu_delete_to() */

#endif /* CF_FAST_EXEC */


/*
+------------------------------------------------------------------------------
| Function    : sig_mg_nu_recover
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_MG_NU_RECOVER. 
|
| Parameters  : UBYTE nsapi: the affected NSAPI
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sig_mg_nu_recover (UBYTE nsapi) 
{ 
  BOOL ready = FALSE;

  TRACE_ISIG( "sig_mg_nu_recover" );

  /*
   * set service instance according to nsapi in primitive
   */
  sndcp_data->nu = & sndcp_data->nu_base[nsapi];
  /*
   * If there is resend, start with first pdu!
   */
  sndcp_data->nu->next_resend = sndcp_data->nu->first_buffered_npdu;
  
  switch( GET_STATE( NU ) )
  {
    case NU_ACK_SUA_NOT_RECEPTIVE:
      /*
       * Is there at least 1 not yet acknowledged N-PDU?
       */
      if (sndcp_data->nu->first_buffered_npdu != NULL) {
        SET_STATE(NU, NU_REC_SUA_NOT_RECEPTIVE);
      }
      break;
    case NU_ACK_SUA_RECEPTIVE:
      nu_send_pending(&ready);
      break;
#ifndef NCONFIG
	case NU_SEND_ACK_SUA_NOT_RECEPTIVE:
      /*
       * Is there at least 1 not yet acknowledged N-PDU?
       */
      if (sndcp_data->nu->first_buffered_npdu != NULL) {
        SET_STATE(NU, NU_SEND_REC_SUA_NOT_RECEPTIVE);
      }
      break;
    case NU_SEND_ACK_SUA_RECEPTIVE:
    case NU_SEND_SUS_SUA_RECEPTIVE:
      nu_config_send_pending(&ready);
      break;
#endif    /* NCONFIG */
    case NU_SUS_SUA_RECEPTIVE:
    case NU_SUS_SUA_NOT_RECEPTIVE:
      break;
    default:
      TRACE_ERROR( "SIG_MG_NU_RECOVER unexpected" );
      break;
  }
} /* sig_mg_nu_recover() */

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sig_mg_nu_reset_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_MG_NU_RESET_IND. 
|
| Parameters  : UBYTE nsapi: the affected NSAPI
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sig_mg_nu_reset_ind (UBYTE nsapi) 
{ 
  TRACE_ISIG( "sig_mg_nu_reset_ind" );

  /*
   * set service instance according to nsapi in primitive
   */
  sndcp_data->nu = & sndcp_data->nu_base[nsapi];
  
  switch( GET_STATE( NU ) )
  {
    case NU_UNACK_SU_NOT_RECEPTIVE:
    case NU_UNACK_SU_RECEPTIVE:
    case NU_ACK_SUA_NOT_RECEPTIVE:
    case NU_ACK_SUA_RECEPTIVE:
    case NU_SUS_SUA_NOT_RECEPTIVE:
    case NU_SUS_SUA_RECEPTIVE:
    case NU_REC_SUA_NOT_RECEPTIVE:
#ifndef NCONFIG
    case NU_SEND_UNACK_SU_RECEPTIVE:
    case NU_SEND_UNACK_SU_NOT_RECEPTIVE:
    case NU_SEND_ACK_SUA_RECEPTIVE:
    case NU_SEND_ACK_SUA_NOT_RECEPTIVE:
    case NU_SEND_SUS_SUA_RECEPTIVE:
    case NU_SEND_SUS_SUA_NOT_RECEPTIVE:
    case NU_SEND_REC_SUA_NOT_RECEPTIVE:
#endif
        sndcp_data->nu->send_npdu_number_unack = 0;
      break;
    default:
      TRACE_ERROR( "SIG_MG_NU_RESET_IND unexpected" );
      break;
  }
} /* sig_mg_nu_reset_ind() */

#endif /* CF_FAST_EXEC */

  
/*
+------------------------------------------------------------------------------
| Function    : sig_mg_nu_resume
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_MG_NU_RESUME. 
|
| Parameters  : UBYTE nsapi: the affected NSAPI
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sig_mg_nu_resume (UBYTE nsapi) 
{ 
  BOOL ready = FALSE;
  USHORT sapi_state = 0;
  UBYTE sapi = 0;
  TRACE_ISIG( "sig_mg_nu_resume" );

  /*
   * set service instance according to nsapi in primitive
   */
  sndcp_data->nu = & sndcp_data->nu_base[nsapi];
  
  switch( GET_STATE( NU ) )
  {
    case NU_SUS_SUA_NOT_RECEPTIVE:
      /*
       * Is there at least 1 not yet acknowledged N-PDU?
       */
      if (sndcp_data->nu->first_buffered_npdu != NULL) {
        SET_STATE(NU, NU_REC_SUA_NOT_RECEPTIVE);
      } else {
        SET_STATE(NU, NU_ACK_SUA_NOT_RECEPTIVE);
      }
      break;
    case NU_SUS_SUA_RECEPTIVE:
      /*
       * Is there at least 1 not yet acknowledged N-PDU?
       */
      sndcp_get_nsapi_sapi(nsapi, &sapi);
      sndcp_get_sapi_state(sapi, &sapi_state);
      /*
       * In ACK mode, the PDU send should not resume until LL_ESTB_IND is received
       * Refer  3GPP 04.65 sec 5.1.2.26 
       */
      if ((sndcp_data->nu->first_buffered_npdu != NULL) AND 
          ((sapi_state & MG_EST_IND) == 0)) {
        nu_send_pending(&ready);
      } else {
        sndcp_unset_sapi_state(sapi, MG_EST_IND);
        SET_STATE(NU, NU_ACK_SUA_RECEPTIVE);
      }
      break;
    case NU_UNACK_SU_RECEPTIVE:
    case NU_UNACK_SU_NOT_RECEPTIVE:

    case NU_ACK_SUA_RECEPTIVE:

#ifndef NCONFIG
    case NU_SEND_UNACK_SU_RECEPTIVE:
    case NU_SEND_UNACK_SU_NOT_RECEPTIVE:
    case NU_SEND_ACK_SUA_RECEPTIVE:
    case NU_SEND_ACK_SUA_NOT_RECEPTIVE:
    case NU_SEND_SUS_SUA_RECEPTIVE:
    case NU_SEND_SUS_SUA_NOT_RECEPTIVE:
    case NU_SEND_REC_SUA_NOT_RECEPTIVE:
#endif
      break;
    default:
      TRACE_ERROR( "SIG_MG_NU_RESUME unexpected" );
      break;
  }
} /* sig_mg_nu_resume() */

#endif /* CF_FAST_EXEC */


/*
+------------------------------------------------------------------------------
| Function    : sig_mg_nu_suspend
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_MG_NU_SUSPEND. 
|
| Parameters  : UBYTE nsapi: the affected NSAPI
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sig_mg_nu_suspend (UBYTE nsapi) 
{ 
  TRACE_ISIG( "sig_mg_nu_suspend" );

  /*
   * set service instance according to nsapi in primitive
   */
  sndcp_data->nu = & sndcp_data->nu_base[nsapi];
  
  switch( GET_STATE( NU ) )
  {
    case NU_ACK_SUA_NOT_RECEPTIVE:
      SET_STATE(NU, NU_SUS_SUA_NOT_RECEPTIVE);
      break;
    case NU_ACK_SUA_RECEPTIVE:
      SET_STATE(NU, NU_SUS_SUA_RECEPTIVE);
      break;
#ifndef NCONFIG
    case NU_SEND_ACK_SUA_NOT_RECEPTIVE:
      SET_STATE(NU, NU_SEND_SUS_SUA_NOT_RECEPTIVE);
      break;
    case NU_SEND_ACK_SUA_RECEPTIVE:
      SET_STATE(NU, NU_SEND_SUS_SUA_RECEPTIVE);
      break;
    case NU_SEND_UNACK_SU_RECEPTIVE:
    case NU_SEND_UNACK_SU_NOT_RECEPTIVE:
    case NU_SEND_SUS_SUA_RECEPTIVE:
    case NU_SEND_SUS_SUA_NOT_RECEPTIVE:
    case NU_SEND_REC_SUA_NOT_RECEPTIVE:
#endif
	case NU_SUS_SUA_RECEPTIVE:
    case NU_UNACK_SU_RECEPTIVE :
    case NU_UNACK_SU_NOT_RECEPTIVE:
      break;
    default:
      TRACE_ERROR( "SIG_MG_NU_SUSPEND unexpected" );
      break;
  }
} /* sig_mg_nu_suspend() */

#endif /* CF_FAST_EXEC */


/*
+------------------------------------------------------------------------------
| Function    : sig_mg_nu_reset
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_MG_NU_RESET. The 
|               send_npdu_number_unack is set to 0 and GACI gets an
|               SN_UNITREADY_IND for the affected NSAPI.
|
| Parameters  : UBYTE nsapi: the affected NSAPI, discard ready flag
|                      (is old unitready_ind to be discarded?
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sig_mg_nu_reset (UBYTE nsapi, BOOL discard_ready) 
{ 
  TRACE_ISIG( "sig_mg_nu_reset" );

  /*
   * set service instance according to nsapi in primitive
   */
  sndcp_data->nu = & sndcp_data->nu_base[nsapi];
  
  switch( GET_STATE( NU ) )
  {

    case NU_UNACK_SU_RECEPTIVE :
    case NU_UNACK_SU_NOT_RECEPTIVE:
    case NU_ACK_SUA_RECEPTIVE:
    case NU_ACK_SUA_NOT_RECEPTIVE:
    case NU_SUS_SUA_RECEPTIVE:
    case NU_SUS_SUA_NOT_RECEPTIVE:
    case NU_REC_SUA_NOT_RECEPTIVE:

      sndcp_data->nu->send_npdu_number_unack = 0;
      sndcp_data->nu->send_npdu_number_ack = 0;
      sndcp_data->nu->rec_npdu_number_ack = 0;
      nu_delete_npdus();
      if (discard_ready) {
        sndcp_data->nu->sn_unitready_ind_pending = FALSE;
      }
      nu_unitready_ind_if_nec(nsapi);
      SET_STATE(NU, NU_UNACK_SU_RECEPTIVE);
      break;
#ifndef NCONFIG
    case NU_SEND_UNACK_SU_RECEPTIVE:
    case NU_SEND_UNACK_SU_NOT_RECEPTIVE:
    case NU_SEND_ACK_SUA_RECEPTIVE:
    case NU_SEND_ACK_SUA_NOT_RECEPTIVE:
    case NU_SEND_SUS_SUA_RECEPTIVE:
    case NU_SEND_SUS_SUA_NOT_RECEPTIVE:
    case NU_SEND_REC_SUA_NOT_RECEPTIVE:

      sndcp_data->nu->send_npdu_number_unack = 0;
      sndcp_data->nu->send_npdu_number_ack = 0;
      sndcp_data->nu->rec_npdu_number_ack = 0;
      nu_delete_npdus();
      if (discard_ready) {
        sndcp_data->nu->sn_unitready_ind_pending = FALSE;
      }
      nu_unitready_ind_if_nec(nsapi);
      SET_STATE(NU, NU_UNACK_SU_RECEPTIVE);
      break;
#endif /* NCONFIG */
    default:
      TRACE_ERROR( "SIG_MG_NU_RESET unexpected" );
      break;
  }
} /* sig_mg_nu_reset() */

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sig_mg_nu_reset_ack
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_MG_NU_RESET_ACK. The 
|               send_npdu_number_ack and rec_npdu_number_ack are set to 0 and 
|               GACI gets an
|               SN_READY_IND for the affected NSAPI.
|
| Parameters  : UBYTE nsapi: the affected NSAPI, send npdu number, 
|               rec npdu number, discard_ready flag
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sig_mg_nu_reset_ack (UBYTE nsapi, UBYTE send_no, UBYTE rec_no, BOOL discard_ready) 
{ 
  TRACE_ISIG( "sig_mg_nu_reset_ack" );

  /*
   * set service instance according to nsapi in primitive
   */
  sndcp_data->nu = & sndcp_data->nu_base[nsapi];
  
  switch( GET_STATE( NU ) )
  {
    case NU_UNACK_SU_RECEPTIVE :
    case NU_UNACK_SU_NOT_RECEPTIVE:
    case NU_ACK_SUA_RECEPTIVE:
    case NU_ACK_SUA_NOT_RECEPTIVE:
    case NU_SUS_SUA_RECEPTIVE:
    case NU_SUS_SUA_NOT_RECEPTIVE:
    case NU_REC_SUA_NOT_RECEPTIVE:
      sndcp_data->nu->send_npdu_number_unack = 0;
      sndcp_data->nu->send_npdu_number_ack = send_no;
      sndcp_data->nu->rec_npdu_number_ack = rec_no;
      nu_delete_npdus();
      if (discard_ready) {
        sndcp_data->nu->sn_ready_ind_pending = FALSE;
      }
      nu_ready_ind_if_nec(nsapi);
      SET_STATE(NU, NU_ACK_SUA_RECEPTIVE);
      break;
#ifndef NCONFIG
    case NU_SEND_UNACK_SU_RECEPTIVE:
    case NU_SEND_UNACK_SU_NOT_RECEPTIVE:
    case NU_SEND_ACK_SUA_RECEPTIVE:
    case NU_SEND_ACK_SUA_NOT_RECEPTIVE:
    case NU_SEND_SUS_SUA_RECEPTIVE:
    case NU_SEND_SUS_SUA_NOT_RECEPTIVE:
    case NU_SEND_REC_SUA_NOT_RECEPTIVE:

      sndcp_data->nu->send_npdu_number_unack = 0;
      sndcp_data->nu->send_npdu_number_ack = send_no;
      sndcp_data->nu->rec_npdu_number_ack = rec_no;
      nu_delete_npdus();
      if (discard_ready) {
        sndcp_data->nu->sn_ready_ind_pending = FALSE;
      }
      nu_ready_ind_if_nec(nsapi);
      SET_STATE(NU, NU_ACK_SUA_RECEPTIVE);
      break;
#endif /* NCONFIG */
    default:
      TRACE_ERROR( "SIG_MG_NU_RESET_ACK unexpected" );
      break;  
  }
} /* sig_mg_nu_reset_ack() */

#endif /* CF_FAST_EXEC */


/*
+------------------------------------------------------------------------------
| Function    : sig_su_nu_ready_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_SU_NU_READY_IND
|
| Parameters  : UBYTE nsapi: the affected NSAPI
|
+------------------------------------------------------------------------------
*/
/*f defined(CF_FAST_EXEC) || defined(_SIMULATION_) || \
   !defined(SNDCP_UPM_INCLUDED)   || defined(SNDCP_2to1) */

GLOBAL void sig_su_nu_ready_ind (UBYTE nsapi) 
{ 
  TRACE_ISIG( "sig_su_nu_ready_ind" );

  /*
   * Set service instance according to nsapi in signal.
   */
  sndcp_data->nu = & sndcp_data->nu_base[nsapi];
  
  switch( GET_STATE( NU ) )
  {
    case NU_ACK_SUA_NOT_RECEPTIVE:
      break;
    case NU_ACK_SUA_RECEPTIVE:
      break;
    case NU_UNACK_SU_NOT_RECEPTIVE:
      SET_STATE(NU, NU_UNACK_SU_RECEPTIVE);
      nu_unitready_ind_if_nec(nsapi);
      break;
    case NU_UNACK_SU_RECEPTIVE:
      break;
#ifndef NCONFIG
    case NU_SEND_ACK_SUA_RECEPTIVE: 
      break;
    case NU_SEND_ACK_SUA_NOT_RECEPTIVE:
      break;
    case NU_SEND_UNACK_SU_NOT_RECEPTIVE:
      SET_STATE(NU, NU_SEND_UNACK_SU_RECEPTIVE);
      nu_unitready_ind_if_nec(nsapi);
      break;
    case NU_SEND_UNACK_SU_RECEPTIVE:
      /*SET_STATE(NU, NU_SEND_UNACK_SU_RECEPTIVE);*/
      nu_config_send_next(nsapi);
      break;
    case NU_SEND_REC_SUA_NOT_RECEPTIVE:
      break;
    case NU_SEND_SUS_SUA_NOT_RECEPTIVE:
      break;
    case NU_SEND_SUS_SUA_RECEPTIVE:
      break;
#endif
    default:
      TRACE_ERROR( "SIG_SU_NU_READY_IND unexpected" );
      break;
  }
} /* sig_su_nu_ready_ind() */

/*#endif  *//* CF_FAST_EXEC || _SIMULATION_ || !SNDCP_UPM_INCLUDED || SNDCP_2to1 */

/*
+------------------------------------------------------------------------------
| Function    : sig_sua_nu_data_cnf
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_SUA_NU_DATA_CNF. The N-PDU
|               with the given N-PDU number has been confirmed completely by
|               the network and service nu is notified abou this.
|
| Parameters  : UBYTE nsapi: the affected NSAPI, UBYTE npdu_number: the number
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sig_sua_nu_data_cnf (UBYTE nsapi, UBYTE npdu_number) 
{ 
  TRACE_ISIG( "sig_sua_nu_npdu_number" );

  /*
   * Set service instance according to nsapi in signal.
   */
  sndcp_data->nu = & sndcp_data->nu_base[nsapi];
  
  switch( GET_STATE( NU ) )
  {
    case NU_ACK_SUA_NOT_RECEPTIVE:
    case NU_REC_SUA_NOT_RECEPTIVE:
    case NU_ACK_SUA_RECEPTIVE:
    case NU_SUS_SUA_NOT_RECEPTIVE:
    case NU_SUS_SUA_RECEPTIVE:
#ifndef NCONFIG
    case NU_SEND_ACK_SUA_RECEPTIVE:
    case NU_SEND_ACK_SUA_NOT_RECEPTIVE:
    case NU_SEND_SUS_SUA_RECEPTIVE:
    case NU_SEND_SUS_SUA_NOT_RECEPTIVE:
    case NU_SEND_REC_SUA_NOT_RECEPTIVE:
#endif /* NCONFIG */
      nu_delete_npdu(nsapi, npdu_number);
      break;
    case NU_UNACK_SU_NOT_RECEPTIVE:
      break;
    case NU_UNACK_SU_RECEPTIVE:
      break;
    default:
      TRACE_ERROR( "SIG_SUA_NU_DATA_CNF unexpected" );
      break;
  }
} /* sig_sua_nu_data_cnf() */

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sig_sua_nu_ready_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_SUA_NU_READY_IND
|
| Parameters  : UBYTE nsapi: the affected NSAPI
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sig_sua_nu_ready_ind (UBYTE nsapi) 
{ 
  BOOL ready;
#ifndef NCONFIG
  U32 *data_count;
  data_count = &sndcp_data->data_count[nsapi];
#endif

  TRACE_FUNCTION( "sig_sua_nu_ready_ind" );

  /*
   * set service instance according to nsapi in primitive
   */
  sndcp_data->nu = & sndcp_data->nu_base[nsapi];

  switch( GET_STATE( NU ) )
  {
    case NU_ACK_SUA_NOT_RECEPTIVE:
#ifndef NCONFIG
      if (*data_count == 0) {
        SET_STATE(NU, NU_ACK_SUA_RECEPTIVE);
        nu_ready_ind_if_nec(nsapi);
      } else {
        SET_STATE(NU, NU_SEND_ACK_SUA_NOT_RECEPTIVE);
        nu_config_send_next(sndcp_data->nu->nsapi);
      }
#else
        SET_STATE(NU, NU_ACK_SUA_RECEPTIVE);
        nu_ready_ind_if_nec(nsapi);
#endif
      break;
    case NU_REC_SUA_NOT_RECEPTIVE:
      nu_send_pending(&ready);
      break;
    case NU_SUS_SUA_NOT_RECEPTIVE:
      SET_STATE(NU, NU_SUS_SUA_RECEPTIVE);
      break;
    case NU_ACK_SUA_RECEPTIVE:
      break;
#ifndef NCONFIG
    case NU_SEND_ACK_SUA_RECEPTIVE: 
      nu_config_send_next(nsapi);
      break;
    case NU_SEND_ACK_SUA_NOT_RECEPTIVE:
      SET_STATE(NU, NU_SEND_ACK_SUA_RECEPTIVE);
      nu_ready_ind_if_nec(nsapi);
      break;
    case NU_SEND_UNACK_SU_NOT_RECEPTIVE:
      break;
    case NU_SEND_UNACK_SU_RECEPTIVE:
      SET_STATE(NU, NU_SEND_UNACK_SU_RECEPTIVE);
      break;
    case NU_SEND_REC_SUA_NOT_RECEPTIVE:
      nu_config_send_pending(&ready);
      break;
    case NU_SEND_SUS_SUA_NOT_RECEPTIVE:
      break;
    case NU_SEND_SUS_SUA_RECEPTIVE:
      break;
#endif
    default:
      TRACE_ERROR( "SIG_SUA_NU_READY_IND unexpected" );
      break;
  }
} /* sig_sua_nu_ready_ind() */

#endif /* CF_FAST_EXEC */




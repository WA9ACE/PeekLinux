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
|             described in the SDL-documentation (IRX-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#ifndef LLC_IRXS_C
#define LLC_IRXS_C
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

#include "llc_irxf.h"    /* to get IRX helper functions */
#include "llc_irxl.h"    /* to get IRX label functions */
#include "llc_itxs.h"    /* to get IRX/ITX signals */
#include "llc_us.h"      /* to get U signals */


/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : sig_llme_irx_assign_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_LLME_IRX_ASSIGN_REQ
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_llme_irx_assign_req (void)
{ 
  TRACE_ISIG( "sig_llme_irx_assign_req" );
  
  switch (GET_STATE(IRX))
  {
    case IRX_TLLI_UNASSIGNED:
      /*
       * Initialise current incarnation.
       */
      irx_init_sapi();
      SET_STATE (IRX, IRX_TLLI_ASSIGNED);
      break;

    default:
      TRACE_ERROR( "SIG_LLME_IRX_ASSIGN_REQ unexpected" );
      break;
  }
} /* sig_llme_irx_assign_req() */


/*
+------------------------------------------------------------------------------
| Function    : sig_llme_irx_unassign_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_LLME_IRX_UNASSIGN_REQ
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_llme_irx_unassign_req (void)
{ 
  TRACE_ISIG( "sig_llme_irx_unassign_req" );
  
  switch (GET_STATE(IRX))
  {
    case IRX_TLLI_UNASSIGNED:
      /*
       * Ignore unexpected signal
       */
      break;

    default:
      /*
       * Delete stored I frames and change to state TLLI_UNASSIGNED
       */
      irx_queue_clean();
      irx_init_sapi();
      SET_STATE (IRX, IRX_TLLI_UNASSIGNED);
      break;
  }
} /* sig_llme_irx_unassign_req() */


/*
+------------------------------------------------------------------------------
| Function    : sig_llme_irx_reset_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_LLME_IRX_RESET_REQ
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_llme_irx_reset_req (void)
{ 
  TRACE_ISIG( "sig_llme_irx_reset_req" );

 
  switch (GET_STATE(IRX))
  {
    case IRX_TLLI_UNASSIGNED:
    case IRX_TLLI_ASSIGNED:
      /*
       * Ignore signal.
       */
      break;

    case IRX_ABM:
    case IRX_ABM_BUSY:
      /*
       * Delete stored I frames and change to state TLLI_UNASSIGNED
       */
      irx_queue_clean();
      irx_init_sapi();
      SET_STATE (IRX, IRX_TLLI_ASSIGNED);
      break;

    default:
      TRACE_ERROR( "SIG_LLME_IRX_RESET_REQ unexpected" );
      break;
  }
} /* sig_llme_irx_reset_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_llme_irx_abmest_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_LLME_IRX_ABMEST_REQ
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_llme_irx_abmest_req (void)
{ 
  TRACE_ISIG( "sig_llme_irx_abmest_req" );

  switch (GET_STATE(IRX))
  {
    case IRX_TLLI_ASSIGNED:
      /*
       * Initialize ABM operation
       */
      irx_init_abm();
      SET_STATE (IRX, IRX_ABM);
      break;

    case IRX_ABM:
    case IRX_ABM_BUSY:
      /*
       * Delete stored I frames and re-initialize ABM operation
       */
      irx_queue_clean();
      irx_init_abm();
      SET_STATE (IRX, IRX_ABM);
      break;

    default:
      TRACE_ERROR( "SIG_LLME_IRX_ABMEST_REQ unexpected" );
      break;
  }
} /* sig_llme_irx_abmest_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_llme_irx_abmrel_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_LLME_IRX_ABMREL_REQ
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_llme_irx_abmrel_req (void)
{ 
  TRACE_ISIG( "sig_llme_irx_abmrel_req" );
 
  switch (GET_STATE(IRX))
  {
    case IRX_ABM:
    case IRX_ABM_BUSY:
      /*
       * Delete stored I frames and end ABM operation
       */
      irx_queue_clean();
      irx_init_sapi();
      SET_STATE (IRX, IRX_TLLI_ASSIGNED);
      break;

    default:
      /* TRACE_ERROR( "SIG_LLME_IRX_ABMREL_REQ unexpected" ); but ok */
      break;
  }
} /* sig_llme_irx_abmrel_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_rx_irx_data_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_RX_IRX_DATA_IND
|
| Parameters  : ll_unitdata_ind - a valid pointer to an LL-UNITDATA-IND 
|                                 primitive
|               command    - contains the command within the primitive
|               frame_type - type of PDU (I or S frame)
|               cr_bit     - setting of the CR bit
|               a_bit      - setting of the A bit
|               ns         - send sequence number of the frame 
|               nr         - receive sequence number of the frame
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_rx_irx_data_ind (T_LL_UNITDATA_IND *ll_unitdata_ind,
                                 T_COMMAND         command, 
                                 T_PDU_TYPE        frame_type,
                                 T_BIT             cr_bit,
                                 T_BIT             a_bit,
                                 T_FRAME_NUM       ns, 
                                 T_FRAME_NUM       nr)
{
  UBYTE     state;

  TRACE_ISIG( "sig_rx_irx_data_ind" );
  
#ifdef TRACE_EVE
    {
      static char* sf[] = { "RR", "ACK", "RNR", "SACK" };

      if(command <= 3)
      {
        if(frame_type == I_FRAME)
        {
          TRACE_4_INFO("got %s I-Frame NS:%d NR:%d A:%d", sf[command], ns, nr, a_bit);
        }
        else
        {
          TRACE_3_INFO("got %s S-Frame NR:%d A:%d", sf[command], nr, a_bit);
        }
      }
    }
#endif

  llc_data->u->current_tlli = ll_unitdata_ind->tlli;

  switch ( (state = GET_STATE(IRX)) )
  {
    case IRX_TLLI_ASSIGNED:
      /*
       * Send a DM response to any valid command received, that 
       * cannot be handeled (GSM 4.64 Release 6.7.0 Chapter 6.4.1.4)
       */
      TRACE_0_INFO("S- or I/S-Frame received in unexpected state");
      
      PFREE (ll_unitdata_ind);

      sig_irx_u_no_frame_expected_ind();
      break;

    case IRX_ABM:
    case IRX_ABM_BUSY:
      /*
       * va <= nr <= vs 
       */
      if ( FRAME_NUM_VALID (llc_data->sapi->va, nr, llc_data->sapi->vs) )
      {
        /*
         * frame valid
         */
        irx_ack_all_to ((T_FRAME_NUM)(nr - 1));

        switch (command)
        {
          case I_RR:
            /*
             * Label IRX_RR
             */
            sig_irx_itx_peer_ready_ind();
            break;

          case I_ACK:
            /*
             * Label IRX_ACK
             */
            sig_irx_itx_ack_ind (FALSE, nr );
            sig_irx_itx_ack_ind (TRUE, (T_FRAME_NUM)(nr + 1));
            sig_irx_itx_cnf_l3data_req ();
            sig_irx_itx_peer_ready_ind ();
            break;

          case I_SACK:
            /*
             * Label IRX_SACK
             */
            sig_irx_itx_ack_ind (FALSE, nr);
            irx_handle_sack (nr, ll_unitdata_ind, frame_type);
            sig_irx_itx_peer_ready_ind();
            break;

          case I_RNR:
            /*
             * Label IRX_RNR
             */
            sig_irx_itx_peer_busy_ind();
            break;

          default:
            TRACE_ERROR( "Unexpected command received" );
            break;
        }
      }
         
      if (frame_type == I_FRAME)
      {
        irx_label_if (ll_unitdata_ind, ns, a_bit, state);
      }
      else /*  S_FRAME */
      {
        if (state == IRX_ABM)
        {
          irx_send_ack (a_bit);
          PFREE (ll_unitdata_ind);
        }
        else
        {
          irx_send_rnr ();
          PFREE (ll_unitdata_ind);
        }
      }
      break;

    default:
      /*
       * Free unexpected data
       */
      PFREE (ll_unitdata_ind);
      TRACE_ERROR( "SIG_RX_IRX_DATA_IND unexpected" );
      break;
  }
} /* sig_rx_irx_data_ind() */



/*
+------------------------------------------------------------------------------
| Function    : sig_itx_irx_s_frame_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ITX_IRX_S_FRAME_REQ
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_itx_irx_s_frame_req (T_ABIT_REQ_TYPE req)
{ 
  TRACE_ISIG( "sig_itx_irx_s_frame_req" );
  
  switch (GET_STATE(IRX))
  {
    case IRX_ABM:
      /*
       * Send an S frame appropriate for the current state
       */
      if ( llc_data->irx->last_ns == NS_EQUAL_VR )
      {
        sig_irx_itx_send_rr_req (req);
      }
      else if ( llc_data->irx->last_ns == NS_EQUAL_VR_PLUS_1 )
      {
        sig_irx_itx_send_ack_req (req);
      }
      else
      {
        T_SACK_BITMAP bitmap;

        irx_build_sack_bitmap( &bitmap );
        sig_irx_itx_send_sack_req (req, &bitmap);
      }
      /* SET_STATE (IRX, SAME_STATE); */
      break;
  

    case IRX_ABM_BUSY:
      /*
       * The appropriate S frame in state busy anytime is an RNR
       */
      sig_irx_itx_send_rnr_req (req);
      /* SET_STATE (IRX, SAME_STATE); */
      break;

    default:
      TRACE_ERROR( "SIG_ITX_IRX_S_FRAME_REQ unexpected" );
      break;
  }
} /* sig_itx_irx_s_frame_req() */



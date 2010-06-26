/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  GRR
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
|  Purpose :  This module implements signal handler functions for service
|             PG of entity GRR.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRR_PGS_C
#define GRR_PGS_C
#endif

#define ENTITY_GRR

/*==== INCLUDES =============================================================*/

#include "typedefs.h"    /* to get Condat data types */

#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "ccdapi.h"     /* to get CCD API */
#include "cnf_grr.h"    /* to get cnf-definitions */
#include "mon_grr.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "message.h"
#include "grr.h"        /* to get the global entity definitions */


#include "grr_ctrls.h"    /* */

#include "grr_f.h"        /* to get the global functions */
#include "grr_pgf.h"    /* */


/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/
/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_pg_start
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_PG_START
|               PG has to manage the paging procedures according to network mode
|               and presence of PBCCH channel
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_pg_start ( void ) 
{ 
  TRACE_ISIG( "sig_ctrl_pg_start" );

  switch( GET_STATE( PG ) )
  {
    case PG_TRANSFER:
      TRACE_ERROR("SIG_CTRL_PG_START not expected in transfer mode");
      break;
    case PG_NULL:
      SET_STATE(PG, PG_IDLE);
      
      /*lint -fallthrough*/

    default:
      /* Because of having new cell or new PPCH */
      psc_db->last_pg_mode = psc_db->network_pg_mode = REORG_PAGING;

      pg_handle_idle_mode( NULL_IDLE );
      break;
  }

} /* sig_ctrl_pg_start() */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_pg_stop
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_PG_STOP
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_pg_stop ( void ) 
{ 
  TRACE_ISIG( "sig_ctrl_pg_stop" );
  
  grr_data->pg.packet_mode = PACKET_MODE_NULL;
  SET_STATE(PG,PG_NULL);
  pg_send_stop();
  psc_db->last_pg_mode = psc_db->network_pg_mode = REORG_PAGING;
} /* sig_ctrl_pg_stop() */



/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_pg_mode_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_PG_MODE_IND
                This signal indicates whether the MS is in idle, transfer or access mode
|
| Parameters  : packet_mode
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_pg_mode_ind ( T_PACKET_MODE packet_mode) 
{ 
  T_PACKET_MODE old_packet_mode;

  TRACE_ISIG( "sig_ctrl_pg_mode_ind" );
  
  old_packet_mode          = grr_data->pg.packet_mode;
  grr_data->pg.packet_mode = packet_mode;

  switch( GET_STATE( PG ) )
  {
    case PG_IDLE:
      switch( packet_mode )
      {
        case PACKET_MODE_PTM:
          SET_STATE( PG, PG_TRANSFER );

          pg_handle_transfer_mode( IDLE_PTM );
          break;

        case PACKET_MODE_PAM:
          SET_STATE( PG, PG_ACCESS );

          pg_handle_access_mode( );
          break;

        default:
          break;
      }
      break;

    case PG_TRANSFER:
      switch( packet_mode )
      {
        case PACKET_MODE_PAM:
          SET_STATE( PG, PG_ACCESS );

          pg_handle_access_mode( );
          break;

        case PACKET_MODE_PIM:
          SET_STATE( PG, PG_IDLE );  

          pg_handle_non_drx_timers( );
          pg_handle_idle_mode( PTM_IDLE );
          break;

        case PACKET_MODE_PTM:
          //TODOif( grr_test_mode_active( ) )
          {
            sig_pg_ctrl_stop_mon_ccch( );
          }
          break;
      }
      break;

    case PG_ACCESS:
      switch( packet_mode )
      {
        case PACKET_MODE_PAM:
          if( old_packet_mode EQ PACKET_MODE_2P_PTM )
          {
            TRACE_EVENT( "PACKET_MODE_2P_PTM --> PACKET_MODE_PAM" );

            pg_handle_access_mode( );
          }
          break;

        case PACKET_MODE_2P_PTM:
          /* 
           * Do not change the state to PG_TRANSFER because of transition rule
           */
          TRACE_EVENT( "PACKET_MODE_2P_PTM in PG_ACCESS" );

          pg_handle_transfer_mode( ACCESS_PTM );
          break;          
 
        case PACKET_MODE_PIM:
          SET_STATE( PG, PG_IDLE );

          pg_handle_idle_mode( ACCESS_IDLE );
          break;

        case PACKET_MODE_PTM:
          SET_STATE( PG, PG_TRANSFER );

          pg_handle_transfer_mode( ACCESS_PTM );
          break;
      }
      break;    

    default:
      TRACE_ERROR( "SIG_CTRL_PG_MODE_IND unexpected" );
      break;
  }
}/* sig_ctrl_pg_mode_ind() */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_pg_t3172_run
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_PG_T3172_RUN
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_pg_t3172_run ( void) 
{ 
  TRACE_ISIG( "sig_ctrl_pg_t3172_run" );
  
} /* sig_ctrl_pg_t3172_run() */


/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_pg_indication
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_PG_INDICATION. This 
|               signal indicates paging for packet access 
|
| Parameters  : UBYTE paging_type (IMSI or PTMSI(GPRS TMSI))
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_pg_indication ( UBYTE paging_type ) 
{ 
  UBYTE type;
  
  TRACE_ISIG( "sig_ctrl_pg_indication" );
  
  type = (paging_type EQ RRGRR_IMSI)? GMMRR_IMSI: GMMRR_PTMSI;
  
  switch( GET_STATE( PG ) )
  {
    case PG_IDLE:
      sig_pg_ctrl_downlink_transfer ( type );
      break;
    case PG_ACCESS:
      /* ignore PAGING REQUEST messages indicating a packet paging procedure*/
      break;
    default:
      TRACE_ERROR( "SIG_CTRL_PG_INDICATION unexpected" );
      break;
  }
} /* sig_ctrl_pg_indication() */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_pg_rr_est
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_PG_RR_EST. This 
|               signal indicates paging for RR connection.
|               This signal makes no sense in case of Class C mobile
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_pg_rr_est ( void) 
{ 
 
  TRACE_ISIG( "sig_ctrl_pg_rr_est" );
  
  if(grr_data->ms.class_of_mode EQ GMMRR_CLASS_CG)
  {
    TRACE_ERROR("CLASS_CG MS has received a RR_EST ");  
  }
  else
  {
    switch( GET_STATE( PG ) )
    {
      case PG_NULL:  
      case PG_IDLE:
      case PG_ACCESS:
        /* inform CTRL resp. GMM about the RR connection establishment request:
         *  GMM has to decide whether accept the request or not
         */
        sig_pg_ctrl_rr_est_req(PG_ON_CCCH);
        break;
      case PG_TRANSFER:
        if(grr_data->pg.nmo EQ GMMRR_NET_MODE_II)
        {
          TRACE_EVENT(" CS call in GMMRR_NET_MODE_II transfer mode");
          sig_pg_ctrl_rr_est_req(PG_ON_CCCH);
        }
        else
        {
          TRACE_EVENT_P1("CS call in transfer mode NMO_%d", grr_data->pg.nmo+1);
        }
        break;
      default:
        TRACE_ERROR( "SIG_CTRL_PG_RR_EST unexpected" );
        break;
    }
  }
} /* sig_ctrl_pg_indication() */



/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_pg_req
+------------------------------------------------------------------------------
| Description : Handles
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_pg_req ( void )
{ 
  MCAST( d_paging_req, D_PAGING_REQ );

  UBYTE state = GET_STATE( PG );

  TRACE_ISIG( "sig_gfp_pg_req" );

  switch( state )
  {
    case PG_IDLE:
    case PG_TRANSFER:
    case PG_ACCESS:
      pg_process_pp_req( d_paging_req, state );
      break;
    default:
      TRACE_ERROR( "sig_gfp_pg_req: page mode has no meaning in this state" );
      break;
  }
} /* sig_gfp_pg_req() */

/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_pg_mode
+------------------------------------------------------------------------------
| Description : Handles 
|
| Parameters  : UBYTE page_mode:       requested page mode
|               BOOL  ms_paging_group: paging belongs to MS paging group
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_pg_mode ( UBYTE page_mode, BOOL ms_paging_group )
{ 
  TRACE_ISIG( "sig_gfp_pg_mode" );

  if( ms_paging_group )
  {
    switch( GET_STATE( PG ) )
    {
      case PG_IDLE:
        if( page_mode NEQ SAME_PAGING )
        {
          psc_db->network_pg_mode = page_mode;
        }

        if( grr_is_non_drx_period( ) EQ  FALSE                   AND
            psc_db->last_pg_mode     NEQ psc_db->network_pg_mode     )
        {
          /*
           * To ensure that the paging mode is not set to page normal in other states
           * than packet idle mode, the function grr_is_packet_idle_mode is called in
           * advance.
           *
           * State PG_IDLE not necessarily means that GRR is in packet idle mode. In
           * case GRR is currently executing the RRGRR_STOP_TASK_REQ/CNF procedure,
           * just the services CPAP and TC changes their states. All other services
           * remain in state IDLE, which is not quite correct. A similar scenario
           * occurs during execution of the MPHP_ASSIGNMENT_REQ/CON procedure.
           */
          if( grr_is_packet_idle_mode( ) EQ TRUE )
          {
            pg_send_start( psc_db->network_pg_mode );
          }
          else
          {
            TRACE_EVENT( "sig_gfp_pg_mode: Page mode set by next packet idle mode" );
          }
        }
        break;

      case PG_ACCESS:
        if( page_mode NEQ SAME_PAGING )
        {
          psc_db->network_pg_mode = page_mode;
        }
        break;

      default:

#ifdef _SIMULATION_

        TRACE_ERROR( "SIG_GFP_PG_MODE unexpected" );

#endif /* #ifdef _SIMULATION_ */

        break;
    }
  }
} /* sig_gfp_pg_mode() */

/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_pg_stop_pccch_con
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_GFP_PG_STOP_PCCCH_CON
|
| Parameters  : mphp_stop_pccch_con: pointer to T_MPHP_STOP_PCCCH_CON
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_pg_stop_pccch_con ( T_MPHP_STOP_PCCCH_CON *mphp_stop_pccch_con) 
{ 
  TRACE_ISIG( "sig_gfp_pg_stop_pccch_con" );
} /* sig_gfp_pg_stop_pccch_con() */

/*
+------------------------------------------------------------------------------
| Function    : sig_cs_pg_start_t_nc2_ndrx
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CS_PG_START_NC2_NDRX
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_cs_pg_start_t_nc2_ndrx ( UBYTE nc_non_drx_period ) 
{ 
  TRACE_ISIG( "sig_cs_pg_start_t_nc2_ndrx" );

  pg_start_t_nc2_ndrx( nc_non_drx_period );

} /* sig_cs_pg_start_t_nc2_ndrx() */

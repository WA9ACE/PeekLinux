/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  GRLC
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
|  Purpose :  This module implements primitive handler functions for service
|             TM of entity GRLC.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRLC_TMP_C
#define GRLC_TMP_C
#endif

#define ENTITY_GRLC

/*==== INCLUDES =============================================================*/

#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include <string.h>
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "ccdapi.h"     /* to get CCD API */
#include "cnf_grlc.h"    /* to get cnf-definitions */
#include "mon_grlc.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "message.h"    /* to get air message definitions */
#include "grlc.h"        /* to get the global entity definitions */
#include "grlc_f.h"      /* to get the grlc global function definitions */
#include "grlc_rus.h"    /* to get the service RU signal definitions */
#include "grlc_rds.h"    /* to get the service RD signal definitions */
#include "grlc_tmf.h"    /* to get the service TM functions definitions */
#include "grlc_meass.h"  /* to get the service MEAS signal definitions */
#include "grlc_tpcs.h"
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
#include "grlc_tms.h"
#endif

/*==== CONST ================================================================*/

/*==== DIAGNOSTICS ==========================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== LOCAL TYPES ==========================================================*/

LOCAL void tm_uplink_data_req ( T_PRIM_TYPE  prim_type,
                                void        *llc_data_req );

/*==== PRIVATE FUNCTIONS ====================================================*/
/*
+------------------------------------------------------------------------------
| Function    : tm_uplink_data_req 
+------------------------------------------------------------------------------
| Description : Handles the primitives GRLC_DATA_REQ and GRLC_UNITDATA_REQ
|
| Parameters  : *llc_data_req  - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
LOCAL void tm_uplink_data_req ( T_PRIM_TYPE  prim_type,
                                void        *llc_data_req )
{ 
  UBYTE state = GET_STATE( TM );
  BOOL  use_data_req;

  T_GRLC_DATA_REQ     *grlc_data_req     = ( T_GRLC_DATA_REQ*     )llc_data_req;
  T_GRLC_UNITDATA_REQ *grlc_unitdata_req = ( T_GRLC_UNITDATA_REQ* )llc_data_req;

  TRACE_FUNCTION( "tm_uplink_data_req" );

  PACCESS( llc_data_req );

#ifdef FLOW_TRACE

  sndcp_trace_flow_control
                      ( FLOW_TRACE_GRLC, FLOW_TRACE_UP, FLOW_TRACE_TOP, FALSE );

#endif
  
  if( 
      (
        grlc_test_mode_active( )
      )
      AND 
      ( 
        ( prim_type              EQ  CGRLC_LLC_PRIM_TYPE_DATA_REQ     AND 
          grlc_data_req->sapi     NEQ GRLC_SAPI_TEST_MODE         )
        OR
        ( prim_type              EQ  CGRLC_LLC_PRIM_TYPE_UNITDATA_REQ AND 
          grlc_unitdata_req->sapi NEQ GRLC_SAPI_TEST_MODE         )
      )
    )
  {
    /*
     * all LLC data requests are ignored during GPRS test mode
     */      
    TRACE_EVENT( "tm_uplink_data_req: GPRS test mode active" );
    PFREE( llc_data_req );

    return;
  }


  if(   prim_type                  EQ CGRLC_LLC_PRIM_TYPE_DATA_REQ           AND 
        grlc_data->prim_start_tbf  <  PRIM_QUEUE_SIZE                        AND  
      ( grlc_data_req->cause       EQ GRLC_DTACS_EMPTY_FRAME            OR
        grlc_data_req->cause       EQ GRLC_DTACS_CELL_NOTIFI_NULL_FRAME    )     )
  {
    TRACE_EVENT_P1("EMPTY FRAME RECEIVED, LLLC FRAME AVAILABLE, delete empty frame: sdu_len=%d", grlc_data_req->sdu.l_buf / 8);    
    grlc_data->tm.send_grlc_ready_ind = SEND_A_GRLC_READY_IND;
    tm_handle_grlc_ready_ind( );
    PFREE( llc_data_req );
    return;
  }

  

/*

  if( prim_type EQ PRIM_TYPE_DATA_REQ )
  {
    TRACE_EVENT_P2( "tm_uplink_data_req: sdu_len=%ld data_req_cnt=%d",
                    grlc_data_req->sdu.l_buf / 8,
                    grlc_data->grlc_data_req_cnt );

    TRACE_EVENT_P9( "tm_uplink_data_req: sapi=%d tlli=%lx delay=%d relc=%d peak=%d prec=%d mean=%d r_pri=%d cause=%d",
                    grlc_data_req->sapi,
                    grlc_data_req->tlli,
                    grlc_data_req->qos.delay,
                    grlc_data_req->qos.relclass,
                    grlc_data_req->qos.peak,
                    grlc_data_req->qos.preced,
                    grlc_data_req->qos.mean,
                    grlc_data_req->radio_prio,
                    grlc_data_req->cause );
  }

*/

  grlc_data->tm.send_grlc_ready_ind = SEND_A_GRLC_READY_IND;

  switch( state )
  {
    case TM_ACCESS_DISABLED:
    case TM_ACCESS_PREPARED:
      use_data_req = 
        (
          grlc_data->tm.disable_class EQ CGRLC_DISABLE_CLASS_CR
        )
        AND
        (
          ( prim_type           EQ CGRLC_LLC_PRIM_TYPE_DATA_REQ     AND
            grlc_data_req->cause EQ GRLC_DTACS_DEF              )
          OR
          ( prim_type           EQ CGRLC_LLC_PRIM_TYPE_UNITDATA_REQ     )
        );
      break;
     
    default:
      use_data_req = tm_access_allowed (  grlc_data_req->radio_prio  );
      break;
  }

  if( use_data_req )
  {
    tm_data_req( prim_type, ( T_GRLC_DATA_REQ* )llc_data_req );
    grlc_data->grlc_data_req_cnt++;

    switch( state )
    {
      case TM_PIM:
        tm_ul_tbf_ind();
        break;

      case TM_PTM:
        tm_ul_tbf_ind();
        
        if( grlc_data->tbf_type NEQ TBF_TYPE_DL )
        {
          /* uplink is active*/
          sig_tm_ru_queue_status( );
        }
        break;

      default:
        /* do nothing */
        break;
    }
  }
  else
  {
    tm_cgrlc_status_ind( CGRLC_ACCESS_2_NETWORK_NOT_ALLOWED );
    PFREE( llc_data_req );
  }

  tm_handle_grlc_ready_ind( );

} /* tm_uplink_data_req () */

/*==== PUBLIC FUNCTIONS =====================================================*/
/*
+------------------------------------------------------------------------------
| Function    : tm_grlc_data_req 
+------------------------------------------------------------------------------
| Description : Handles the primitive GRLC_DATA_REQ 
|
| Parameters  : *grlc_data_req  - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_grlc_data_req  ( T_GRLC_DATA_REQ  *grlc_data_req  )
{ 
  TRACE_FUNCTION( "tm_grlc_data_req " );

  tm_uplink_data_req( CGRLC_LLC_PRIM_TYPE_DATA_REQ, ( void* )grlc_data_req );
   
} /* tm_grlc_data_req () */

/*
+------------------------------------------------------------------------------
| Function    : tm_grlc_unitdata_req
+------------------------------------------------------------------------------
| Description : Handles the primitive GRLC_UNITDATA_REQ
|
| Parameters  : *grlc_unitdata_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_grlc_unitdata_req ( T_GRLC_UNITDATA_REQ *grlc_unitdata_req )
{ 
  TRACE_FUNCTION( "tm_grlc_unitdata_req" );
  
  tm_uplink_data_req( CGRLC_LLC_PRIM_TYPE_UNITDATA_REQ, ( void* )grlc_unitdata_req );

} /* tm_grlc_unitdata_req() */



/*
+------------------------------------------------------------------------------
| Function    : tm_t3168
+------------------------------------------------------------------------------
| Description : Handles the primitive T3168
|               This timer is involved in 3 Procedures
|               1) Two Phase Access Procedure
|               2) Uplink TBF Setup on existing Downlink TBF
|               3) Uplink Realloaction Procedure on existing Uplink TBF
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_t3168 ( void )
{ 
  TRACE_FUNCTION( "tm_t3168" );
  TRACE_EVENT_P1("T3168 expired: n_acc=%d",grlc_data->tm.n_res_req);

  /* 
   * The expiry of this timer may occure in 3 cases:
   * 
   * a) uplink allocation during running downlink TBF  --> GRLC
   * b) two phase access procedure                    --> GRLC
   * c) uplink reallocation procedure on uplink TBF or on concurent TBF(UL&DL)  -->GRLC
   *
   */
  
  switch( GET_STATE( TM ) )
  {
    case TM_PTM:
      switch(grlc_data->tbf_type)
      {
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
        case TBF_TYPE_TP_ACCESS:
          grlc_data->uplink_tbf.ti=1;
          tm_handle_error_ra(); 
          break;
#endif
        case TBF_TYPE_DL:
          /*
           *  cse a)
           */
        if(grlc_data->tm.n_res_req < 4) /* Is resource re-allocation is running? */  
        {
          grlc_data->tm.n_res_req++;
          tm_build_chan_req_des(&grlc_data->chan_req_des, 
                              &grlc_data->prim_queue[grlc_data->prim_start_tbf]);
          sig_tm_rd_ul_req();     
        }
        else
        { /* grlc_data->tm.n_res_req EQ 4 
                      -> last Channel Request Description has been sent */
          tm_handle_error_ra(); 
        }
        break;

        case TBF_TYPE_UL:
        case TBF_TYPE_CONC:
          /*
           * case c)
           */
        if(grlc_data->tm.n_res_req < 4) /* Is resource re-allocation is running? */  
        { 
          T_U_GRLC_RESOURCE_REQ resource_req;/*lint !e813*/

          tm_build_res_req(&resource_req,
                           R_RE_ALLOC); 
          tm_store_ctrl_blk( CGRLC_BLK_OWNER_TM, ( void* )&resource_req );
        }
        else
        { /* 
           * grlc_data->tm.n_res_req EQ 4 -> last resource request has been sent 
           * return to packet idle and indicate packet access failure to upper layers
           */
          while(grlc_data->prim_start_tbf < PRIM_QUEUE_SIZE_TOTAL)
          {
            grlc_delete_prim();
          }
          tm_cgrlc_status_ind(CGRLC_PACKET_ACCESS_FAILURE);
          tm_abort_tbf(grlc_data->tbf_type);
          grlc_data->tm.n_res_req = 0;
          TRACE_EVENT("4x p. resource req. failed");
          SET_STATE(TM,TM_WAIT_4_PIM);
        }
        break;
        default:
          /*
           *  Should never occure
           */
          TRACE_ERROR("T3168 expired, no task should not appear");
          break;
      }
      
      break;

    default:
      TRACE_ERROR( "T3168 unexpected" );
      break;
  }

} /* tm_t3168() */


/*
+------------------------------------------------------------------------------
| Function    : tm_t3188
+------------------------------------------------------------------------------
| Description : Handles the primitive T3188
|
| Parameters  :   
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_t3188 ( void )
{ 
  TRACE_FUNCTION( "tm_t3188" );
  

  switch( GET_STATE( TM ) )
  {
    case TM_PTM:
      /*
       * perform abnormal release with random access
       */
      tm_handle_error_ra();
      break;
    default:
      TRACE_ERROR( "tm_t3188 unexpected" );
      break;
  }

} /* tm_t3188() */


/*
+------------------------------------------------------------------------------
| Function    : tm_grlc_activate_gmm_queue_req
+------------------------------------------------------------------------------
| Description : Handles the primitive GRLC_ACTIVATE_GMM_QUEUE_REQ
|
| Parameters  :   
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_grlc_activate_gmm_queue_req  ( T_GRLC_ACTIVATE_GMM_QUEUE_REQ *grlc_activate_gmm_queue_req  )
{ 
  TRACE_FUNCTION( "tm_grlc_activate_gmm_queue_req" );
  

  switch( GET_STATE( TM ) )
  {
    case TM_ACCESS_DISABLED:
      tm_get_gmm_prim_queue( );      
      break;

    case TM_ACCESS_PREPARED:
      SET_STATE( TM, TM_PIM );
      sig_tm_ru_reset_poll_array();	  
      tm_get_gmm_prim_queue( );      
      tm_handle_grlc_ready_ind( );
      break;

    default:
      TRACE_ERROR( "GRLC_ACTIVATE_GMM_QUEUE_REQ unexpected" );
      break;
  }

  PFREE( grlc_activate_gmm_queue_req );

} /* tm_grlc_activate_gmm_queue_req() */

/*
+------------------------------------------------------------------------------
| Function    : tm_grlc_flush_data_req
+------------------------------------------------------------------------------
| Description : When LLC receives XID RESET during Inter-SGSN RAU procedure,
|               LLC sends this primitive GRLC_FLUSH_DATA_REQ to GRLC to flush out LLC 
|               user data maintained in GRLC
|
| Parameters  : void               
|               
+------------------------------------------------------------------------------
*/

GLOBAL void tm_grlc_flush_data_req (T_GRLC_FLUSH_DATA_REQ *grlc_flush_data_req)
{
  
  UBYTE i;
  UBYTE temp_start_tbf;
  UBYTE temp_start_free;   

  TRACE_FUNCTION( "tm_grlc_flush_data_req" );

  TRACE_EVENT_P4(" ps=%d, pf=%d,sps=%d,spf=%d",
                                       grlc_data->prim_start_tbf,
                                       grlc_data->prim_start_free,
                                       grlc_data->save_prim_start_tbf,
                                       grlc_data->save_prim_start_free);


  if(!grlc_data->gmm_procedure_is_running OR 
     !(grlc_data->prim_start_tbf >= PRIM_QUEUE_SIZE))
  /* Check for the condition grlc_data->prim_start_tbf EQ 0xFF is not required since
   * grlc_data->prim_start_tbf=0xFF implies there are no primitives in queue to flush out.
   * LLC PDUs are flushed only when GMM queue is active dring Inter-SGSN RAU procedure */
  {
    TRACE_ERROR( "GRLC_FLUSH_DATA_REQ unexpected" );
    PFREE(grlc_flush_data_req);
    return;
  }  

 /* Remember GMM primitives when gmm procedure is running*/
  temp_start_tbf = grlc_data->prim_start_tbf;
  temp_start_free = grlc_data->prim_start_free;

 /* Retrieve the saved primitive values when LLC was suspended by GMM */
  grlc_data->prim_start_tbf = grlc_data->save_prim_start_tbf;
  grlc_data->prim_start_free = grlc_data->save_prim_start_free;

  i = grlc_data->prim_start_tbf;

  grlc_data->prim_user_data = 0;
  while(i NEQ 0xFF)
  {
    grlc_data->prim_user_data += BYTELEN(grlc_data->prim_queue[i].prim_ptr->sdu.l_buf);
    i = grlc_data->prim_queue[i].next;
  }
  grlc_data->tm.send_grlc_ready_ind = SEND_A_GRLC_READY_IND;

 /* Delete all LLC primitives (user data) irrespective of going in same or new tbf */

  do
  {
    grlc_delete_prim();
  } while( (grlc_data->prim_start_tbf < PRIM_QUEUE_SIZE_TOTAL));


  grlc_data->save_prim_start_tbf = grlc_data->prim_start_tbf;
  if(grlc_data->prim_start_tbf NEQ 0xff)
  {
    TRACE_ASSERT(grlc_data->prim_start_tbf NEQ 0xff);
  }

  tm_handle_grlc_ready_ind();

 /* The GMM primitive values are put back */
  grlc_data->prim_start_tbf = temp_start_tbf;
  grlc_data->prim_start_free = temp_start_free;


  TRACE_EVENT_P4(" ps=%d, pf=%d,sps=%d,spf=%d",
                                       grlc_data->prim_start_tbf,
                                       grlc_data->prim_start_free,
                                       grlc_data->save_prim_start_tbf,
                                       grlc_data->save_prim_start_free);


  PFREE(grlc_flush_data_req);
  
}


/*
+------------------------------------------------------------------------------
| Function    : tm_cgrlc_enable_req
+------------------------------------------------------------------------------
| Description : Handles the primitive cgrlc_enable_req
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_cgrlc_enable_req ( T_CGRLC_ENABLE_REQ *cgrlc_enable_req )
{ 
  TRACE_FUNCTION( "tm_cgrlc_enable_req" );


  switch( GET_STATE( TM ) )
  {
    case TM_ACCESS_DISABLED:
    case TM_ACCESS_PREPARED:
      if( cgrlc_enable_req->enable_cause EQ  
                                   CGRLC_ENAC_ABNORM_RELEASE_CRESELECT_FAILED )
      {
        SET_STATE( TM, TM_PIM );
        sig_tm_ru_reset_poll_array();		
        tm_cgrlc_status_ind( CGRLC_RLC_MAC_ERROR );
        tm_delete_prim_queue( );
      }
      else if(cgrlc_enable_req->queue_mode EQ CGRLC_QUEUE_MODE_GMM)
      {
        if(grlc_data->gmm_procedure_is_running )       
        {
          SET_STATE(TM, TM_PIM);
          tm_delete_prim_queue();
          sig_tm_ru_reset_poll_array();		  		  
          TRACE_EVENT_P2("LLC QUEUE IS DISABLED, DELETE GMM PRIM QUEUE ps=%d pf=%d",
                                  grlc_data->prim_start_tbf,
                                  grlc_data->prim_start_free);
        }
        else
        {
          SET_STATE( TM, TM_ACCESS_PREPARED );
        }
      }

      /*lint -fallthrough*/

    case TM_PIM:
    case TM_PAM:
    case TM_PTM:
    case TM_WAIT_4_PIM: 

      grlc_data->tm.max_grlc_user_data = MAX_GRLC_USER_DATA ;

      if(cgrlc_enable_req->queue_mode EQ CGRLC_QUEUE_MODE_LLC)
      {
        UBYTE state = GET_STATE( TM );

        if( state EQ TM_ACCESS_DISABLED OR
            state EQ TM_ACCESS_PREPARED    )
        {
          SET_STATE( TM, TM_PIM );
          sig_tm_ru_reset_poll_array();		  		  
        }
        if(grlc_data->gmm_procedure_is_running)
        {
          tm_get_llc_prim_queue();
          if ( (tm_prim_queue_get_free_count() > 0)            AND
             (grlc_data->prim_user_data <= grlc_data->tm.max_grlc_user_data))
          {
            grlc_data->tm.send_grlc_ready_ind = SEND_A_GRLC_READY_IND;
          }
          else
          {
            grlc_data->tm.send_grlc_ready_ind = PRIM_QUEUE_FULL;
          }
        }
        else
        {
          TRACE_EVENT("RESUME IS DONE");
        }
        /*
         * check if cell update is required
         */
        if(cgrlc_enable_req->cu_cause EQ CGRLC_RA_CU)
        {
          TRACE_EVENT("CU REQ from GRR");
          /*
           * Delete Empty frames
           */
          if(   grlc_data->prim_start_tbf                                        <  PRIM_QUEUE_SIZE                        AND
              ( grlc_data->prim_queue[grlc_data->prim_start_tbf].prim_ptr->cause EQ GRLC_DTACS_EMPTY_FRAME            OR 
                grlc_data->prim_queue[grlc_data->prim_start_tbf].prim_ptr->cause EQ GRLC_DTACS_CELL_NOTIFI_NULL_FRAME    )     )
          {
            grlc_delete_prim();
            TRACE_EVENT("CEll_res: delete prim");
          }
          grlc_data->uplink_tbf.access_type = CGRLC_AT_CELL_UPDATE;
        }
        tm_ul_tbf_ind();
        tm_handle_grlc_ready_ind();

      }

      grlc_data->burst_type             = cgrlc_enable_req->burst_type;
      grlc_data->ab_type                = cgrlc_enable_req->ab_type;
      grlc_data->t3168_val              = cgrlc_enable_req->t3168_val;
      grlc_data->uplink_tbf.tlli        = cgrlc_enable_req->ul_tlli;
      grlc_data->uplink_tbf.ac_class    = cgrlc_enable_req->ac_class;
      grlc_data->downlink_tbf.tlli      = cgrlc_enable_req->dl_tlli;
      grlc_data->tm.change_mark         = cgrlc_enable_req->change_mark;

#ifdef REL99
      grlc_data->pfi_support            = cgrlc_enable_req->pfi_support;
      grlc_data->nw_rel                 = cgrlc_enable_req->nw_rel;	  
#endif
      /* 
       * set N3102 to maximum value after cell reselction
       */
      if(cgrlc_enable_req->v_pan_struct)
      {
        if(cgrlc_enable_req->pan_struct.pmax NEQ CGRLC_NO_UPDATE_N3102)
        {
          grlc_data->pan_struct = cgrlc_enable_req->pan_struct;
          grlc_data->N3102      = cgrlc_enable_req->pan_struct.pmax;
        }
      }
      else
      {
        grlc_data->pan_struct = cgrlc_enable_req->pan_struct;
        grlc_data->N3102      = 0xFF;
      }
      if( grlc_data->uplink_tbf.tlli EQ (0xffffffff))
      {
        TRACE_EVENT("GMM HAS UNASSIGNED GRR/GRLC, DELETE DATA QUEUE");        
        /*
         * delete prim queue
         */
        while(grlc_data->prim_start_tbf < PRIM_QUEUE_SIZE_TOTAL)
        {
          grlc_delete_prim();
        }
      }
      break;
    default:
      TRACE_ERROR( "CGRLC_ENABLE_REQ unexpected" );
      break;
  }
  PFREE(cgrlc_enable_req);

} /* tm_cgrlc_enable_req() */



/*
+------------------------------------------------------------------------------
| Function    : tm_cgrlc_disable_req
+------------------------------------------------------------------------------
| Description : Handles the primitive cgrlc_disable_req
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_cgrlc_disable_req ( T_CGRLC_DISABLE_REQ        *cgrlc_disable_req )
{ 
  UBYTE state = GET_STATE( TM );

  TRACE_FUNCTION( "tm_cgrlc_disable_req" );

  grlc_data->tm.disable_class = cgrlc_disable_req->disable_class; 

  switch( state )
  {
    case TM_ACCESS_PREPARED:
      SET_STATE(TM, TM_ACCESS_DISABLED);
      break;

    case TM_ACCESS_DISABLED:
      /* nothing to do */
      break;

    case TM_PIM:
    case TM_WAIT_4_PIM:
      SET_STATE(TM, TM_ACCESS_DISABLED);

      tm_handle_test_mode_cnf( grlc_test_mode_active( ) );

      grlc_data->rel_type = REL_TYPE_NULL;
      break;

    case TM_PTM:
      tm_abort_tbf(grlc_data->tbf_type);
       /*
        * delete all poll positions
        */
      sig_tm_ru_reset_poll_array();  

      /*lint -fallthrough*/

    case TM_PAM:
      SET_STATE(TM, TM_ACCESS_DISABLED);

      tm_handle_test_mode_cnf( grlc_test_mode_active( ) );

      grlc_data->rel_type = REL_TYPE_NULL;

      if( state EQ TM_PAM AND cgrlc_disable_req->disable_class EQ CGRLC_DISABLE_CLASS_CR )
      {
        tm_cgrlc_status_ind( CGRLC_TBF_ESTABLISHMENT_FAILURE );
      }
      break;

    default:
      TRACE_ERROR( "CGRLC_DISABLE_REQ unexpected" );
      break;
  }

  if(cgrlc_disable_req->prim_status EQ CGRLC_PRIM_STATUS_ONE)
  {
    grlc_delete_prim();
    tm_handle_grlc_ready_ind();
  }
  else if(cgrlc_disable_req->prim_status EQ CGRLC_PRIM_STATUS_ALL)
  {
    tm_delete_prim_queue();
  /* Reinitialize GRLC context 
   * 1. Delete Primitives
   * 2. Abort TBF - (This is already done by GMMRR_SUSPEND_REQ)   
   *    We reach this disable req only after releasing all TBFs
   * 3. Reinitialize GRLC context */
  
    TRACE_EVENT("Reinit GRLC");
    grlc_init();
  }


  PFREE(cgrlc_disable_req);

} /* tm_cgrlc_disable_req() */


/*
+------------------------------------------------------------------------------
| Function    : tm_cgrlc_ul_tbf_res
+------------------------------------------------------------------------------
| Description : Handles the primitive cgrlc_ul_tbf_res
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_cgrlc_ul_tbf_res ( T_CGRLC_UL_TBF_RES *cgrlc_ul_tbf_res )
{ 
  TRACE_FUNCTION( "tm_cgrlc_ul_tbf_res" );

  tm_prcs_pwr_ctrl( &cgrlc_ul_tbf_res->pwr_ctrl );
  
  /*
   * Ignore the request for or modification of an UL TBF in case an UL TBF 
   * release request is currently sent to the data queue of GRR, it is just 
   * not yet processed, but TBF will be released anyway as one of the next
   * steps.
   */
  if( ( grlc_data->rel_type & REL_TYPE_UL ) EQ REL_TYPE_UL )
  {
    TRACE_EVENT( "tm_cgrlc_ul_tbf_res: release of UL TBF requested in parallel" );

    PFREE( cgrlc_ul_tbf_res );

    return;
  }

  vsi_t_stop(GRLC_handle,T3168);
  grlc_data->tm.n_res_req = 0;

  switch( GET_STATE( TM ) )
  {
    case TM_PAM:
      switch(cgrlc_ul_tbf_res->tbf_mode)
      {
        case CGRLC_TBF_MODE_ESTABLISHMENT_FAILURE:  /* establishment failure */
          SET_STATE(TM,TM_PIM);
          grlc_delete_prim();
          sig_tm_ru_reset_poll_array();		  
          tm_cgrlc_status_ind(CGRLC_TBF_ESTABLISHMENT_FAILURE);
          tm_handle_grlc_ready_ind();
          tm_ul_tbf_ind();
          break;
        case CGRLC_TBF_MODE_ACCESS_FAILED:
          if(cgrlc_ul_tbf_res->prim_status EQ CGRLC_PRIM_STATUS_ONE)
          {
            SET_STATE(TM,TM_PIM);
            grlc_delete_prim();
            sig_tm_ru_reset_poll_array();
            tm_cgrlc_status_ind(CGRLC_PACKET_ACCESS_FAILURE);
            tm_handle_grlc_ready_ind();
            tm_ul_tbf_ind();
          }
          else if(cgrlc_ul_tbf_res->prim_status EQ CGRLC_PRIM_STATUS_ALL)
          {
            SET_STATE(TM,TM_PIM);
            tm_delete_prim_queue();
            sig_tm_ru_reset_poll_array();
            tm_cgrlc_status_ind(CGRLC_PACKET_ACCESS_FAILURE);
            tm_handle_grlc_ready_ind();
            tm_ul_tbf_ind();
          }
          else if(cgrlc_ul_tbf_res->prim_status EQ CGRLC_PRIM_STATUS_TBF)
          {
            SET_STATE(TM,TM_PIM);
            sig_tm_ru_reset_poll_array();
            tm_cgrlc_status_ind(CGRLC_PACKET_ACCESS_FAILURE);
            tm_delete_prim_queue();
            tm_handle_grlc_ready_ind();
            tm_ul_tbf_ind();
          }
          else
          {
            tm_handle_error_ra();
            tm_ul_tbf_ind();
          }
          /*
           * what happens to the existing tbfs
           */
          break;
        case CGRLC_TBF_MODE_UL:
        case CGRLC_TBF_MODE_TMA:
        case CGRLC_TBF_MODE_TMB:
          SET_STATE(TM,TM_PTM);  
          if((grlc_data->testmode.mode EQ CGRLC_TEST_RANDOM) OR
             (grlc_data->testmode.mode EQ CGRLC_LOOP))
          {
            PALLOC(cgrlc_test_mode_ind,CGRLC_TEST_MODE_IND); /*T_CGRLC_TEST_MODE_IND*/
            cgrlc_test_mode_ind->test_mode_flag = grlc_data->testmode.mode;
            PSEND(hCommGRR,cgrlc_test_mode_ind);
          }
          grlc_data->uplink_tbf.cs_type      = (T_CODING_SCHEME)cgrlc_ul_tbf_res->cs_mode;
          grlc_data->uplink_tbf.mac_mode     = cgrlc_ul_tbf_res->mac_mode;
          grlc_data->uplink_tbf.nts          = cgrlc_ul_tbf_res->nts_max;
          grlc_data->uplink_tbf.ts_mask      = cgrlc_ul_tbf_res->tn_mask;
          grlc_data->uplink_tbf.ti           = cgrlc_ul_tbf_res->ti;
          grlc_data->uplink_tbf.tlli_cs_type = cgrlc_ul_tbf_res->tlli_cs_mode;
          grlc_data->uplink_tbf.bs_cv_max    = cgrlc_ul_tbf_res->bs_cv_max;
          grlc_data->r_bit                   = cgrlc_ul_tbf_res->r_bit;

          grlc_data->ul_tbf_start_time       = cgrlc_ul_tbf_res->starting_time;
          sig_tm_rd_ul_req_stop();
          sig_tm_meas_ptm();

          tm_activate_tbf ( TBF_TYPE_UL );

          if(cgrlc_ul_tbf_res->mac_mode)
          {
            grlc_data->uplink_tbf.fa_manag.fa_type = FA_NO_CURRENT;
            tm_store_fa_bitmap(&cgrlc_ul_tbf_res->fix_alloc_struct);
          }

          
          if(!grlc_data->uplink_tbf.ti)
            grlc_data->tm.n_acc_req_procedures = 0;

          grlc_data->tm.n_res_req = 0;

          grlc_data->uplink_tbf.rlc_db_granted = cgrlc_ul_tbf_res->rlc_db_granted;
          
          tm_tfi_handling  (cgrlc_ul_tbf_res->starting_time,CGRLC_TBF_MODE_UL, cgrlc_ul_tbf_res->tfi, 0xFF);
          sig_tm_ru_assign();
          if(cgrlc_ul_tbf_res->polling_bit NEQ 0xFF)
            tm_handle_polling_bit(cgrlc_ul_tbf_res->starting_time,cgrlc_ul_tbf_res->polling_bit);
          break;
        default:
          TRACE_ERROR("unexpected tbf mode in ul_tbf_res");
          break;
      }
      break;
    case TM_PTM:      
      switch(cgrlc_ul_tbf_res->tbf_mode)
      {
        case CGRLC_TBF_MODE_ACCESS_FAILED:
          if(cgrlc_ul_tbf_res->prim_status EQ CGRLC_PRIM_STATUS_ONE)
          {
            grlc_delete_prim();
            tm_cgrlc_status_ind(CGRLC_PACKET_ACCESS_FAILURE);
            tm_handle_grlc_ready_ind();
            tm_ul_tbf_ind();
          }
          else if(cgrlc_ul_tbf_res->prim_status EQ CGRLC_PRIM_STATUS_ALL)
          {
            tm_delete_prim_queue();
            tm_cgrlc_status_ind(CGRLC_PACKET_ACCESS_FAILURE);
            tm_handle_grlc_ready_ind();
            tm_ul_tbf_ind();
          }
          else if(cgrlc_ul_tbf_res->prim_status EQ CGRLC_PRIM_STATUS_TBF)
          {
            tm_cgrlc_status_ind(CGRLC_PACKET_ACCESS_FAILURE);
            tm_delete_prim_queue();
            tm_handle_grlc_ready_ind();
            tm_ul_tbf_ind();
          }
          else
          {
			tm_cgrlc_status_ind(CGRLC_PACKET_ACCESS_FAILURE);
            tm_handle_error_ra();
          }
          break;
        case CGRLC_TBF_MODE_UL:
        case CGRLC_TBF_MODE_TMA:
        case CGRLC_TBF_MODE_TMB:
          if((grlc_data->testmode.mode EQ CGRLC_TEST_RANDOM) OR
             (grlc_data->testmode.mode EQ CGRLC_LOOP))
          {
            PALLOC(cgrlc_test_mode_ind,CGRLC_TEST_MODE_IND); /*T_CGRLC_TEST_MODE_IND*/
            cgrlc_test_mode_ind->test_mode_flag = grlc_data->testmode.mode;
            PSEND(hCommGRR,cgrlc_test_mode_ind);
          }

          grlc_data->uplink_tbf.cs_type      = (T_CODING_SCHEME)cgrlc_ul_tbf_res->cs_mode;
          grlc_data->uplink_tbf.mac_mode     = cgrlc_ul_tbf_res->mac_mode;
          grlc_data->uplink_tbf.nts          = cgrlc_ul_tbf_res->nts_max;
          grlc_data->uplink_tbf.ts_mask      = cgrlc_ul_tbf_res->tn_mask;
          grlc_data->uplink_tbf.ti           = cgrlc_ul_tbf_res->ti;
          grlc_data->uplink_tbf.tlli_cs_type = cgrlc_ul_tbf_res->tlli_cs_mode;
          grlc_data->uplink_tbf.bs_cv_max    = cgrlc_ul_tbf_res->bs_cv_max;

#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
          if(grlc_data->tbf_type EQ TBF_TYPE_TP_ACCESS)
          {
            grlc_data->uplink_tbf.rlc_db_granted = cgrlc_ul_tbf_res->rlc_db_granted;
           
            if((grlc_data->testmode.mode EQ CGRLC_TEST_RANDOM) OR
               (grlc_data->testmode.mode EQ CGRLC_LOOP))
            {
              PALLOC(cgrlc_test_mode_ind,CGRLC_TEST_MODE_IND); /*T_CGRLC_TEST_MODE_IND*/
              cgrlc_test_mode_ind->test_mode_flag = grlc_data->testmode.mode;
              PSEND(hCommGRR,cgrlc_test_mode_ind);
            }
          }
          else
#endif
          {
            grlc_data->uplink_tbf.rlc_db_granted  = 0;
          }


          grlc_data->tm.n_res_req = 0;

          grlc_data->ul_tbf_start_time       = cgrlc_ul_tbf_res->starting_time;
          sig_tm_rd_ul_req_stop();

          tm_activate_tbf ( TBF_TYPE_UL );

          if(cgrlc_ul_tbf_res->mac_mode)
          {
            tm_store_fa_bitmap(&cgrlc_ul_tbf_res->fix_alloc_struct);
          }

          if(!grlc_data->uplink_tbf.ti)
            grlc_data->tm.n_acc_req_procedures = 0;

          tm_tfi_handling  (cgrlc_ul_tbf_res->starting_time,CGRLC_TBF_MODE_UL, cgrlc_ul_tbf_res->tfi, 0xFF);

          sig_tm_ru_assign();
          break;
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
        case CGRLC_TBF_MODE_2PA: 
          {
            /*tbf establishment on PACCH*/
            grlc_data->ul_tbf_start_time       = cgrlc_ul_tbf_res->starting_time;
            tm_send_prr_2p_ptm();              
            grlc_data->tm.pacch_prr_pca_sent = FALSE ; /* For Release of TBF , as two phase */
          }
          break;
#endif
        default:
          TRACE_ERROR("unexpected tbf mode in ul_tbf_res");
          break;
      }
      break;
    default:
      TRACE_ERROR( "CGRLC_UL_TBF_RES unexpected" );
      break;
  }
  PFREE(cgrlc_ul_tbf_res);

} /* tm_cgrlc_ul_tbf_res() */

/*
+------------------------------------------------------------------------------
| Function    : tm_cgrlc_dl_tbf_req
+------------------------------------------------------------------------------
| Description : Handles the primitive cgrlc_dl_tbf_req
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_cgrlc_dl_tbf_req ( T_CGRLC_DL_TBF_REQ *cgrlc_dl_tbf_req )
{ 
  UBYTE rd_state=GET_STATE( RD );
  TRACE_FUNCTION( "tm_cgrlc_dl_tbf_req" );

  tm_prcs_pwr_ctrl( &cgrlc_dl_tbf_req->pwr_ctrl );
  grlc_data->downlink_tbf.trans_id++;

  /*
   * Ignore the request for or modification of an DL TBF in case an DL TBF 
   * release request is currently sent to the data queue of GRR, it is just 
   * not yet processed, but TBF will be released anyway as one of the next
   * steps.
   */
  if( ( grlc_data->rel_type & REL_TYPE_DL ) EQ REL_TYPE_DL )
  {
    /* The scenario T3192 expires before packet DL assignment message has been received
    is handled.*/

    TRACE_EVENT( "tm_cgrlc_dl_tbf_req: release of DL TBF requested in parallel" );
    
    TRACE_EVENT( "Exception: T3192 expiry before PDA race condition, verify this" );
    grlc_data->rel_type &= ( ~ ( REL_TYPE_DL ) );

  }

  switch( GET_STATE( TM ) )
  {
    case TM_PAM:     
    case TM_PIM:
    case TM_WAIT_4_PIM:
      SET_STATE(TM,TM_PTM);
      grlc_data->r_bit = 0; 

      sig_tm_meas_ptm();

      /*lint -fallthrough*/

    case TM_PTM:
      
    if(cgrlc_dl_tbf_req->ctrl_ack_bit)
    {
      TRACE_EVENT("GRLC ctrl ack bit set");
      if((grlc_data->tbf_type EQ TBF_TYPE_UL) OR 
         (grlc_data->tbf_type EQ TBF_TYPE_NULL))
      {
        TRACE_EVENT("DL ass will not be discarded in GRLC");
      }
      {
        sig_tm_rd_nor_rel();
        tm_deactivate_tbf(TBF_TYPE_DL);
        grlc_data->dl_tfi = 0xFF;
      }
   }
   else if (!cgrlc_dl_tbf_req->ctrl_ack_bit  AND
               (grlc_t_status(T3192) > 0)       AND 
               #ifdef _TARGET_ 
                (grlc_t_status(T3192) < 25)
               #else
                (grlc_t_status(T3192) < 65) 
               #endif
              )
    {
      TRACE_EVENT_P1("XXXX accepting DL Ass T3192=%ld",grlc_t_status(T3192));
      if ( rd_state EQ RD_REL_ACK OR
           rd_state EQ RD_REL_UACK )
      {
        sig_tm_rd_nor_rel();
        tm_deactivate_tbf(TBF_TYPE_DL);
        grlc_data->dl_tfi = 0xFF;
      }
    }

      grlc_data->downlink_tbf.rlc_mode    = cgrlc_dl_tbf_req->rlc_mode;
      grlc_data->downlink_tbf.cs_type     = (T_CODING_SCHEME)cgrlc_dl_tbf_req->cs_mode;
      grlc_data->downlink_tbf.mac_mode    = cgrlc_dl_tbf_req->mac_mode;
      grlc_data->downlink_tbf.nts         = cgrlc_dl_tbf_req->nts_max;
      grlc_data->downlink_tbf.ts_mask     = cgrlc_dl_tbf_req->tn_mask;
      grlc_data->downlink_tbf.t3192_val   = cgrlc_dl_tbf_req->t3192_val;

      grlc_data->dl_tbf_start_time       = cgrlc_dl_tbf_req->starting_time;

      tm_activate_tbf ( TBF_TYPE_DL );

      tm_tfi_handling  (cgrlc_dl_tbf_req->starting_time,CGRLC_TBF_MODE_DL,0xFF,cgrlc_dl_tbf_req->tfi);

      sig_tm_rd_assign();

    
      if(cgrlc_dl_tbf_req->polling_bit NEQ 0xFF)
        tm_handle_polling_bit(cgrlc_dl_tbf_req->starting_time,cgrlc_dl_tbf_req->polling_bit);


      tm_ul_tbf_ind(); /* dl tbf during access mode, ul tbf will be established over downlink*/
      break;
    default:
      {
        /* This shall never happen, but in case, GRLC shall inform GRR about the 
         * immediate release of the TBF. Otherwise GRLC and GRR are running out
         * of synchronisation and the whole system hangs.
         */
        PALLOC( cgrlc_tbf_rel_ind, CGRLC_TBF_REL_IND );

        TRACE_ERROR( "CGRLC_DL_TBF_REQ unexpected, so send release ind!" );
      
        cgrlc_tbf_rel_ind->tbf_mode      = CGRLC_TBF_MODE_DL;
        cgrlc_tbf_rel_ind->tbf_rel_cause = CGRLC_TBF_REL_ABNORMAL;
        cgrlc_tbf_rel_ind->v_c_value     = FALSE;
        cgrlc_tbf_rel_ind->dl_trans_id   = grlc_data->downlink_tbf.trans_id;

        PSEND( hCommGRR, cgrlc_tbf_rel_ind );
      }
      break;
  }
  PFREE(cgrlc_dl_tbf_req);

} /* tm_cgrlc_dl_tbf_req() */

/*
+------------------------------------------------------------------------------
| Function    : tm_cgrlc_tbf_rel_req
+------------------------------------------------------------------------------
| Description : Handles the primitive cgrlc_tbf_rel_req
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_cgrlc_tbf_rel_req ( T_CGRLC_TBF_REL_REQ       *cgrlc_tbf_rel_req )
{ 
  BOOL rel_ul = FALSE;
  BOOL rel_dl = FALSE;
  BOOL mod_ul = FALSE;
  BOOL mod_dl = FALSE;


  TRACE_FUNCTION( "tm_cgrlc_tbf_rel_req" );


  switch( GET_STATE( TM ) )
  {
    case TM_PTM:
      /*
       * 1. abnomal release uplink            --> immediate release
       * 2. normal release uplink             --> normal tbf release after current pdu is transmitted
       * 3. abnormal release downlink no poll --> immediate release
       * 4. abnormal release downlink with poll--> release after poll is sent
       * release both tbfs in case of testmode B
       */

      if( (cgrlc_tbf_rel_req->tbf_mode EQ CGRLC_TBF_MODE_UL) OR
          (cgrlc_tbf_rel_req->tbf_mode EQ CGRLC_TBF_MODE_DL_UL)
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
          OR
          (cgrlc_tbf_rel_req->tbf_mode EQ CGRLC_TBF_MODE_2PA)
#endif
        )
      {
        if(grlc_data->testmode.mode EQ CGRLC_LOOP)
        {
           rel_ul = TRUE;
           rel_dl = TRUE;
        }
        else
        {
           if(cgrlc_tbf_rel_req->tbf_rel_cause EQ CGRLC_TBF_REL_ABNORMAL)
           {
              rel_ul = TRUE;
           }
           else
           {
              mod_ul = TRUE;
           }
        }

      }
      if( (cgrlc_tbf_rel_req->tbf_mode EQ CGRLC_TBF_MODE_DL) OR
          (cgrlc_tbf_rel_req->tbf_mode EQ CGRLC_TBF_MODE_DL_UL))
      {
        if(grlc_data->testmode.mode EQ CGRLC_LOOP)
        {
           rel_ul = TRUE;
           rel_dl = TRUE;
        }
        else
        {
           if(cgrlc_tbf_rel_req->rel_fn EQ  CGRLC_STARTING_TIME_NOT_PRESENT)
           {
               rel_dl = TRUE;
           }
           else
           {
               mod_dl = TRUE;
           }
        }

      }

      if(mod_ul)
      {
        if(grlc_data->ru.next_prim < PRIM_QUEUE_SIZE_TOTAL)
        {
          grlc_data->prim_queue[grlc_data->ru.next_prim].start_new_tbf = 1; 
          TRACE_EVENT_P5("QUEUE_STATUS BEFORE start=%d,next=%d active=%d rlc_oct=%ld sdu_len=%ld"
                                                    ,grlc_data->prim_start_tbf
                                                    ,grlc_data->ru.next_prim
                                                    ,grlc_data->ru.active_prim
                                                    ,grlc_data->ru.rlc_octet_cnt
                                                    ,grlc_data->ru.sdu_len);
          sig_tm_ru_queue_status();
          TRACE_EVENT_P5("QUEUE_STATUS AFTER start=%d,next=%d active=%d rlc_oct=%ld sdu_len=%ld"
                                                    ,grlc_data->prim_start_tbf
                                                    ,grlc_data->ru.next_prim
                                                    ,grlc_data->ru.active_prim
                                                    ,grlc_data->ru.rlc_octet_cnt
                                                    ,grlc_data->ru.sdu_len);
        }
      }
      if ( rel_ul AND
          (cgrlc_tbf_rel_req->rel_fn NEQ CGRLC_STARTING_TIME_NOT_PRESENT))
      {
        sig_tm_ru_abrel(cgrlc_tbf_rel_req->rel_fn, TRUE);
        mod_ul = TRUE;
        rel_ul = FALSE;
      }

      if(mod_dl)
      {  
        sig_tm_rd_abrel(cgrlc_tbf_rel_req->rel_fn, TRUE);
      }


      if(rel_dl AND rel_ul)
      {
        tm_abort_tbf(grlc_data->tbf_type);
      }
      else if(rel_dl)
      {
        tm_abort_tbf(TBF_TYPE_DL);
      }
      else if(rel_ul)
      {
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
        if(grlc_data->tbf_type EQ TBF_TYPE_TP_ACCESS )
          tm_abort_tbf(TBF_TYPE_TP_ACCESS);
         else
#endif
        tm_abort_tbf(TBF_TYPE_UL);
      }


      if(grlc_data->tbf_type EQ TBF_TYPE_NULL)
      {
        SET_STATE(TM,TM_WAIT_4_PIM);
        sig_tm_ru_reset_poll_array();
      }
      break;
    default:
      TRACE_ERROR( "CGRLC_TBF_REL_REQ unexpected" );
      break;
  }
  PFREE(cgrlc_tbf_rel_req);

} /* tm_cgrlc_tbf_rel_req() */

/*
+------------------------------------------------------------------------------
| Function    : tm_cgrlc_tbf_rel_res
+------------------------------------------------------------------------------
| Description : Handles the primitive cgrlc_tbf_rel_res
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_cgrlc_tbf_rel_res ( T_CGRLC_TBF_REL_RES *cgrlc_tbf_rel_res )
{ 

#if !defined (NTRACE)

  BOOL       trace    = FALSE;
  T_REL_TYPE rel_type = grlc_data->rel_type;

#endif /* #if !defined (NTRACE) */

  TRACE_FUNCTION( "tm_cgrlc_tbf_rel_res" );

  /*
   * Check for consistency: Only requested releases shall be indicated by GRR.
   */
  if( cgrlc_tbf_rel_res->tbf_mode EQ CGRLC_TBF_MODE_DL_UL OR
      cgrlc_tbf_rel_res->tbf_mode EQ CGRLC_TBF_MODE_DL       )
  {
    if( ( grlc_data->rel_type & REL_TYPE_DL ) EQ REL_TYPE_DL )
    {
      grlc_data->rel_type &= ( ~ ( REL_TYPE_DL ) );
    }

#if !defined (NTRACE)

    else
    {
      trace = TRUE;
    } 

#endif /* #if !defined (NTRACE) */

  }

  /*
   * Check for consistency: Only requested releases shall be indicated by GRR.
   */
  if( cgrlc_tbf_rel_res->tbf_mode EQ CGRLC_TBF_MODE_DL_UL OR
      cgrlc_tbf_rel_res->tbf_mode EQ CGRLC_TBF_MODE_UL       )
  {
    if( ( grlc_data->rel_type & REL_TYPE_UL ) EQ REL_TYPE_UL )
    {
      grlc_data->rel_type &= ( ~ ( REL_TYPE_UL ) );
    }

#if !defined (NTRACE)

    else
    {
      trace = TRUE;
    } 

#endif /* #if !defined (NTRACE) */

  }

#if !defined (NTRACE)

  /*
   * Inconsistency deteced.
   */
  if( trace EQ TRUE )
  {
    TRACE_EVENT_P3
      ( "tm_cgrlc_tbf_rel_res: unexpected release response - tbf_mode = %d, rel_type (old) = %d, rel_type (new) = %d",
        cgrlc_tbf_rel_res->tbf_mode, rel_type, grlc_data->rel_type );
  }

#endif /* #if !defined (NTRACE) */

  if( grlc_data->rel_type EQ REL_TYPE_NULL )
  {  
    switch( GET_STATE( TM ) )
    {
    case TM_ACCESS_DISABLED:
    case TM_ACCESS_PREPARED:
      break;
    case TM_WAIT_4_PIM:
      SET_STATE(TM,TM_PIM);
      sig_tm_ru_reset_poll_array();
      tm_handle_test_mode_cnf( grlc_data->testmode.mode EQ CGRLC_TEST_MODE_RELEASE );
      grlc_data->tm.n_res_req            = 0;

      /*lint -fallthrough*/

    case TM_PIM:
      /*
       * reset ta value
       */
      grlc_data->ta_value = 0xFF;  

      /*lint -fallthrough*/

    case TM_PTM:
      tm_ul_tbf_ind();
      break;
    default:
      TRACE_ERROR( "CGRLC_TBF_REL_RES unexpected" );
      break;
    }
  }
  PFREE(cgrlc_tbf_rel_res);

} /* tm_cgrlc_tbf_rel_res() */

/*
+------------------------------------------------------------------------------
| Function    : tm_cgrlc_data_req
+------------------------------------------------------------------------------
| Description : Handles the primitive cgrlc_data_req
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_cgrlc_data_req ( T_CGRLC_DATA_REQ          *cgrlc_data_req )
{ 
  TRACE_FUNCTION( "tm_cgrlc_data_req" );


  switch( GET_STATE( TM ) )
  {
    case TM_PAM:
    case TM_PTM:
      tm_store_ctrl_blk( cgrlc_data_req->blk_owner, ( void* )cgrlc_data_req->data_array ) ;
      break;
    default:
      TRACE_ERROR( "CGRLC_DATA_REQ unexpected" );
      break;
  }
  PFREE(cgrlc_data_req);

} /* tm_cgrlc_data_req() */

/*
+------------------------------------------------------------------------------
| Function    : tm_cgrlc_poll_req
+------------------------------------------------------------------------------
| Description : Handles the primitive cgrlc_poll_req
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_cgrlc_poll_req ( T_CGRLC_POLL_REQ          *cgrlc_poll_req )
{ 
  TRACE_FUNCTION( "tm_cgrlc_poll_req" );


  switch( GET_STATE( TM ) )
  {
    case TM_PAM:
      /*
       * handle cpap polling bit, no save pass directly to layer1, poll will 
       * be sent with the first call
       */

      grlc_data->next_poll_fn      = cgrlc_poll_req->poll_fn;
      grlc_data->ul_poll_pos_index = 0;
      
      if(grlc_data->burst_type NEQ CGRLC_BURST_TYPE_NB) 
      { 
        grlc_send_access_burst(cgrlc_poll_req->tn);
      }
      else 
      {
        T_U_GRLC_CTRL_ACK u_ctrl_ack; /* built pca, do not use ccd, because IA is stored*/
        u_ctrl_ack.msg_type  = U_GRLC_CTRL_ACK_c;
        u_ctrl_ack.pctrl_ack = cgrlc_poll_req->ctrl_ack;
        grlc_set_buf_tlli( &u_ctrl_ack.tlli_value, grlc_data->uplink_tbf.tlli );

        grlc_send_normal_burst((UBYTE *)&u_ctrl_ack, NULL, cgrlc_poll_req->tn);
      }

      break;
    case TM_PTM:
      grlc_save_poll_pos(cgrlc_poll_req->poll_fn,
                         cgrlc_poll_req->tn,
                         0xFF,
                         cgrlc_poll_req->poll_b_type,
                         cgrlc_poll_req->ctrl_ack);
      break;
    default:
      TRACE_ERROR( "CGRLC_POLL_REQ unexpected" );
      break;
  }
  PFREE(cgrlc_poll_req);

} /* tm_cgrlc_poll_req() */

/*
+------------------------------------------------------------------------------
| Function    : tm_cgrlc_access_status_req
+------------------------------------------------------------------------------
| Description : Handles the primitive cgrlc_access_status_req
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_cgrlc_access_status_req ( T_CGRLC_ACCESS_STATUS_REQ *cgrlc_access_status_req )
{ 
  TRACE_FUNCTION( "tm_cgrlc_access_status_req" );


  switch( GET_STATE( TM ) )
  {
    case TM_PIM:
    case TM_PAM:
      tm_ul_tbf_ind();
      break;
    default:
      TRACE_ERROR( "CGRLC_ACCESS_STATUS_REQ unexpected" );
      break;
  }
  PFREE(cgrlc_access_status_req);

} /* tm_cgrlc_access_status_req() */

/*
+------------------------------------------------------------------------------
| Function    : tm_cgrlc_test_mode_req
+------------------------------------------------------------------------------
| Description : Handles the primitive cgrlc_test_mode_req
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_cgrlc_test_mode_req ( T_CGRLC_TEST_MODE_REQ     *cgrlc_test_mode_req )
{  
  UBYTE pdus_to_queue;
  UBYTE count = 0;

  TRACE_FUNCTION( "tm_cgrlc_test_mode_req" );

  TRACE_EVENT_P1( "cgrlc_test_mode_req test mode is activ %x",cgrlc_test_mode_req->test_mode_flag);
  
  switch( GET_STATE( TM ) )
  {
    case TM_PIM:
    case TM_PTM:
     /* 
      * save the test mode data
      */

      grlc_data->testmode.mode   = cgrlc_test_mode_req->test_mode_flag; 

      grlc_data->testmode.n_pdu         = (ULONG)cgrlc_test_mode_req->no_of_pdus;
      grlc_data->testmode.dl_ts_offset  = cgrlc_test_mode_req->dl_timeslot_offset;


      if (grlc_data->testmode.mode EQ CGRLC_LOOP)
      {
        TRACE_EVENT("testmode b");
        switch(grlc_data->tbf_type)
        {
          case TBF_TYPE_DL:
            /*
             * request uplink tbf over existing DL
             */
            tm_queue_test_mode_prim(1);
            /*
             * set received buffer to special values
             */
            grlc_data->testmode.rec_data[0].block_status = 0xFEFE;
            grlc_data->testmode.rec_data[0].e_bit        = 0xFE;
            memset(grlc_data->testmode.rec_data[0].payload, 0xFE,50);
            grlc_data->testmode.rec_data[1].block_status = 0xFEFE;
            grlc_data->testmode.rec_data[1].e_bit        = 0xFE;
            memset(grlc_data->testmode.rec_data[1].payload, 0xFE,50);
            /*
             * If the "Downlink Timeslot Offset" is set to 3, TN3 shall be treated as the first downlink
             * timeslot if a TBF is established in TN3. If TN3 does not support a TBF, the first active 
             * timeslot after TN3 shall be treated as the first downlink timeslot. The counting sequence 
             * is continuous through TN7 and TN0.
             */
            while (!((0x80>>grlc_data->testmode.dl_ts_offset) & grlc_data->dl_tn_mask) && (count < 8))
            {
              if (grlc_data->testmode.dl_ts_offset > 7)
                grlc_data->testmode.dl_ts_offset = (UBYTE)-1;
              grlc_data->testmode.dl_ts_offset++;
              count++;
            }
            TRACE_EVENT_P1 ("downlink timeslot offset is %d", grlc_data->testmode.dl_ts_offset);
            break;
          default:
            TRACE_EVENT_P1("unexpected tbf type for testmode B:tbf_type=%d",grlc_data->tbf_type);
            break;
        }        

      }
      else  /* it is testmode A with regular PDU amount */ 
      {
        if (grlc_data->testmode.n_pdu EQ 0)
        {
          /*
           * In case of Testmode A and 0 PDUs are requested the MS may 
           * optionally interprete this as infinitive duration TBF.
           */
          TRACE_EVENT("testmode a infinite");
          grlc_data->testmode.n_pdu -= 1;
        }
        else
        {
          TRACE_EVENT_P1("testmode a %ld pdus",grlc_data->testmode.n_pdu);
        }
        /*
         * (PRIM_QUEUE_SIZE-1) to avoid the full prime queue situation in test mode.
         * It should also not be more then 50*64 byte, which are queue here.
         * The remainig PDUs will be queued when a PDU is completely transmitted.
         */
        if (grlc_data->testmode.n_pdu < (PEI_PRIM_QUEUE_SIZE/2))
        {
          pdus_to_queue = (UBYTE) grlc_data->testmode.n_pdu;
          grlc_data->testmode.n_pdu = 0;
        }
        else
        {
          pdus_to_queue = (PEI_PRIM_QUEUE_SIZE/2);
          grlc_data->testmode.n_pdu -= (PEI_PRIM_QUEUE_SIZE/2);
        }
        /*
         *  generation of pseudo primitve to start the test mode tbf TBF
         */
        tm_queue_test_mode_prim(pdus_to_queue);

        grlc_prbs(INITIALIZE, 0, 0);
        if(!grlc_data->testmode.ptr_test_data)
        {
          MALLOC(grlc_data->testmode.ptr_test_data,50/*Max. RLC Block Length*/);
        }
      }

      break;
    default:
      TRACE_ERROR( "CGRLC_TEST_MODE_REQ  unexpected" );
      TRACE_EVENT_P1("tm state %d", grlc_data->tm.state);
      break;
  }


  PFREE(cgrlc_test_mode_req);

} /* tm_cgrlc_test_mode_req() */



/*
+------------------------------------------------------------------------------
| Function    : tm_cgrlc_test_end_req
+------------------------------------------------------------------------------
| Description : Handles the primitive CGRLC_TEST_END_REQ
|               GRLC have to leave the test mode. The leaving procedure is 
|               finished with the transmission of GMMRR_TEST_MODE_CNF
| Parameters  : *gmmrr_suspend_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_cgrlc_test_end_req ( T_CGRLC_TEST_END_REQ *cgrlc_test_end_req )
{ 
  TRACE_FUNCTION( "tm_cgrlc_test_end_req" );

  tm_abort_tbf( grlc_data->tbf_type );

  /*
   * delete all primitives in queue.
   */
  tm_delete_prim_queue();

  SET_STATE(TM,TM_WAIT_4_PIM);

  grlc_data->tm.n_acc_req_procedures = 0;
  grlc_data->tm.n_res_req            = 0; /* reset counter of resource requests during access */
  
  PFREE(cgrlc_test_end_req);

} /* tm_cgrlc_test_end_req() */



/*
+------------------------------------------------------------------------------
| Function    : tm_cgrlc_ta_value_req
+------------------------------------------------------------------------------
| Description : Handles the primitive cgrlc_ta_value_req
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_cgrlc_ta_value_req ( T_CGRLC_TA_VALUE_REQ      *cgrlc_ta_value_req )
{ 
  TRACE_FUNCTION( "tm_cgrlc_ta_value_req" );

  grlc_data->ta_value = cgrlc_ta_value_req->ta_value;

  PFREE(cgrlc_ta_value_req);

} /* tm_cgrlc_ta_value_req() */

/*
+------------------------------------------------------------------------------
| Function    : tm_cgrlc_ready_timer_config_req
+------------------------------------------------------------------------------
| Description : Handles the primitive tm_cgrlc_ready_timer_config_req
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_cgrlc_ready_timer_config_req 
               ( T_CGRLC_READY_TIMER_CONFIG_REQ *cgrlc_ready_timer_config_req )
{ 
  TRACE_FUNCTION( "tm_cgrlc_ready_timer_config_req" );

  grlc_data->ready_timer.handling = READY_TIMER_HANDLING_ENABLED;
  grlc_data->ready_timer.value    = cgrlc_ready_timer_config_req->t3314_val; 

  if( grlc_data->ready_timer.value EQ CGRLC_DEACTIVATED )
  {
    grlc_enter_ready_state( );
  }

  PFREE(cgrlc_ready_timer_config_req);

} /* (tm_cgrlc_ready_timer_config_req) */

/*
+------------------------------------------------------------------------------
| Function    : tm_cgrlc_force_to_standby_req
+------------------------------------------------------------------------------
| Description : Handles the primitive tm_cgrlc_force_to_standby_req
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_cgrlc_force_to_standby_req 
                   ( T_CGRLC_FORCE_TO_STANDBY_REQ *cgrlc_force_to_standby_req )
{ 
  TRACE_FUNCTION( "tm_cgrlc_force_to_standby_req" );

 /*
  * By receiving this primitive the Ready Timer State will be forced to Standby and
  * the running Ready Timer will be stopped
  */
  grlc_enter_standby_state( );

  PFREE(cgrlc_force_to_standby_req);

} /* (tm_cgrlc_force_to_standby_req) */
/*
+------------------------------------------------------------------------------
| Function    : tm_cgrlc_pwr_ctrl_req
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_cgrlc_pwr_ctrl_req ( T_CGRLC_PWR_CTRL_REQ *pwr_ctrl_req )
{
  TRACE_FUNCTION( "tm_cgrlc_pwr_ctrl_req" );

  tm_prcs_pwr_ctrl( &pwr_ctrl_req->pwr_ctrl );

  PFREE( pwr_ctrl_req );

} /* tm_cgrlc_pwr_ctrl_req */ 

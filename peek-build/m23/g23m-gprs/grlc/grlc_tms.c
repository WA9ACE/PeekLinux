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
|  Purpose :  This module implements signal handler functions for service
|             TM of entity GRLC.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRLC_TMS_C
#define GRLC_TMS_C
#endif

#define ENTITY_GRLC

/*==== INCLUDES =============================================================*/

#include <string.h>
#include "typedefs.h"    /* to get Condat data types */
#include "vsi.h"         /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"         /* to get a lot of macros */
#include "ccdapi.h"      /* to get CCD API */
#include "cnf_grlc.h"    /* to get cnf-definitions */
#include "mon_grlc.h"    /* to get mon-definitions */
#include "prim.h"        /* to get the definitions of used SAP and directions */
#include "message.h"     /* to get air message definitions */
#include "grlc.h"        /* to get the global entity definitions */
#include "grlc_f.h"      /* to get the global entity definitions */
#include "grlc_tmf.h"    /* to get the service TM procedure definitions */
#include "grlc_tms.h"    /* to get the service TM signal definitions */
#include "grlc_rus.h"    /* to get the service RU signal definitions */
#include "grlc_rds.h"    /* to get the service RD signal definitions */
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
#include "grlc_gffs.h"
#endif



/*==== CONST ================================================================*/

/*==== DIAGNOSTICS ==========================================================*/

/*==== LOCAL TYPES===========================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/


/*
+------------------------------------------------------------------------------
| Function    : tm_get_ctrl_blk
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE* tm_get_ctrl_blk ( UBYTE *index, BOOL unacknowledged )
{ 
  UBYTE *ctrl_blk;

  TRACE_FUNCTION( "tm_get_ctrl_blk" );

  ctrl_blk = tm_set_start_ctrl_blk( index );
 
  if( unacknowledged )
  {
    /* 
     * in case the control blocks are send by the service RU, we are 
     * working in unacknowledged mode, so every block is transmitted 
     * succesfully
     */
    tm_set_stop_ctrl_blk( TRUE, CGRLC_BLK_OWNER_NONE, 0 );
  }
  
  return( ctrl_blk );

} /* tm_get_ctrl_blk() */

/*
+------------------------------------------------------------------------------
| Function    : tm_is_ctrl_blk_rdy
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL tm_is_ctrl_blk_rdy ( UBYTE ack_cnt_meas_rpt, UBYTE ack_cnt_other )
{ 
  BOOL        result    = FALSE;
  T_BLK_INDEX blk_index = grlc_data->tm.ul_ctrl_blk.seq[0];

  TRACE_FUNCTION( "tm_is_ctrl_blk_rdy" );

  if( blk_index NEQ MAX_CTRL_BLK_NUM )
  {
    switch( grlc_data->tm.ul_ctrl_blk.blk[blk_index].owner )
    {
      case( CGRLC_BLK_OWNER_MEAS ):
      case( CGRLC_BLK_OWNER_CS   ): result = ( ack_cnt_meas_rpt EQ 0 ); break;
      default               : result = ( ack_cnt_other    EQ 0 ); break;
    }
  }
  
  return( result );

} /* tm_is_ctrl_blk_rdy() */

/*
+------------------------------------------------------------------------------
| Function    : tm_get_num_ctrl_blck
+------------------------------------------------------------------------------
| Description : This function returns the number of allocated control blocks.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE tm_get_num_ctrl_blck ( void )
{ 
  UBYTE i   = 0; /* used for counting                  */
  UBYTE num = 0; /* number of allocated control blocks */

  TRACE_FUNCTION( "tm_get_num_ctrl_blck" );

  while( i                               <   MAX_CTRL_BLK_NUM AND 
         grlc_data->tm.ul_ctrl_blk.seq[i] NEQ MAX_CTRL_BLK_NUM     )
  {
    if( grlc_data->tm.ul_ctrl_blk.blk[grlc_data->tm.ul_ctrl_blk.seq[i]].state
                                                       EQ BLK_STATE_ALLOCATED )
    {
      num++;
    }

    i++;
  }
  
  return( num );

} /* tm_get_num_ctrl_blck() */

/*
+------------------------------------------------------------------------------
| Function    : tm_cpy_ctrl_blk_to_buffer
+------------------------------------------------------------------------------
| Description : This function ...
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE tm_cpy_ctrl_blk_to_buffer( UBYTE *buffer )
{ 
  UBYTE  index;
  UBYTE *ctrl_block = tm_get_ctrl_blk( &index, FALSE );

  TRACE_FUNCTION( "tm_cpy_ctrl_blk_to_buffer" );

  if( ctrl_block NEQ NULL )
  {
    memcpy( buffer, ctrl_block, BYTE_UL_CTRL_BLOCK );
  }
  else
  {
    TRACE_ERROR( "tm_cpy_ctrl_blk_to_buffer: ctrl_block EQ NULL" );
  }

  return( index );
} /* tm_cpy_ctrl_blk_to_buffer() */


/*==== SIGNAL FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : sig_ru_tm_error_ra
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_RU_TM_ERROR_RA
|               in case of contention resulution failed in service RU
|               refer to GSM0460v650 p35
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/

GLOBAL void sig_ru_tm_error_ra ( void ) 
{ 
  TRACE_ISIG( "sig_ru_tm_error_ra" );

  switch( GET_STATE( TM ) )
  {
   case TM_PTM:
      if( grlc_data->testmode.mode EQ CGRLC_TEST_RANDOM OR
          grlc_data->testmode.mode EQ CGRLC_LOOP    )
      {
        tm_abort_tbf(grlc_data->tbf_type);
        tm_delete_prim_queue();
        SET_STATE(TM,TM_WAIT_4_PIM);
        grlc_data->tm.n_acc_req_procedures = 0;
        grlc_data->tm.n_res_req            = 0; /* reset counter of resource requests during access */
      }
      else
      {
        tm_handle_error_ra();
      }
      break;
    default:
      TRACE_ERROR( "SIG_RU_TM_ERROR_RA unexpected" );
      break;
  }
} /* sig_ru_tm_error_ra() */




/*
+------------------------------------------------------------------------------
| Function    : sig_ru_tm_cs
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_RU_TM_CS
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ru_tm_cs ( void ) 
{ 
  TRACE_ISIG( "sig_ru_tm_cs" );
  
  switch( GET_STATE( TM ) )
  {
    case TM_PTM:
      /*TRACE_EVENT_P1("CONT_RES:nacc set from %d to zero",grlc_data->tm.n_acc_req_procedures);*/
      grlc_data->tm.n_acc_req_procedures  = 0;
      grlc_data->uplink_tbf.ti            = 0;
      {
        PALLOC(prim,CGRLC_CONT_RES_DONE_IND);
        PSEND(hCommGRR,prim);
      }
      break;
    default:
      TRACE_ERROR( "SIG_RU_TM_CS unexpected" );
      break;
  }
} /* sig_ru_tm_cs() */


/*
+------------------------------------------------------------------------------
| Function    : sig_ru_tm_end_of_tbf
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_RU_TM_END_OF_TBF
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ru_tm_end_of_tbf ( void ) 
{ 
  TRACE_ISIG( "sig_ru_tm_end_of_tbf" );
  

    /*
   * TBF end handling for TEST_MODE
   */


  if((grlc_data->testmode.mode EQ CGRLC_TEST_RANDOM ) OR
     (grlc_data->testmode.mode EQ CGRLC_LOOP ) )
  {
    grlc_delete_prim();
    tm_handle_grlc_ready_ind();
  }


  switch( GET_STATE( TM ) )
  {
    case TM_PTM:
      /* stop timer for sending chan req desc in dl ack/nack if running */
      /*TRACE_EVENT_P1("End of tbf:nacc set from %d to zero",grlc_data->tm.n_acc_req_procedures);*/
      grlc_data->tm.n_acc_req_procedures = 0;
      grlc_data->tm.n_res_req            = 0;
      vsi_t_stop(GRLC_handle,T3168);
      switch( grlc_data->tbf_type )
      {
        case TBF_TYPE_CONC:
        case TBF_TYPE_UL:
          tm_send_tbf_rel(TBF_TYPE_UL);
          tm_deactivate_tbf(TBF_TYPE_UL);
          break;
        default:
          break;
      }
      break;
    default:
      TRACE_ERROR( "SIG_RU_TM_END_OF_TBF unexpected" );
      break;
  }

  switch(grlc_data->tbf_type)
  {
    case TBF_TYPE_NULL:
      SET_STATE(TM,TM_WAIT_4_PIM);
      break;
    case TBF_TYPE_DL:
      /*
       * uplink tbf released, downlink is still running, a new pdu is uplink queue
       */
      tm_ul_tbf_ind();
      break;
    default:
      TRACE_ERROR( "tm_deactivate_tbf did not work correct in sig_ru_tm_end_of_tbf" );
      break;
  }
} /* sig_ru_tm_end_of_tbf() */




/*
+------------------------------------------------------------------------------
| Function    : sig_rd_tm_end_of_tbf
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_RD_TM_END_OF_TBF
|
| Parameters  : rel_ul - release UL TBF also. ( True,False)
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_rd_tm_end_of_tbf ( BOOL rel_ul ) 
{ 
/*  T_TIME dummy; */
  TRACE_ISIG( "sig_rd_tm_end_of_tbf" );

  switch( GET_STATE( TM ) )
  {
    case TM_PTM:
      if( grlc_data->testmode.mode EQ CGRLC_LOOP )
      {
        TRACE_EVENT("TESTMODE B: Normal DL release ");
        tm_send_tbf_rel(grlc_data->tbf_type);
        sig_tm_ru_abrel(1, FALSE);
        tm_deactivate_tbf(grlc_data->tbf_type);
        tm_delete_prim_queue();
        SET_STATE(TM,TM_WAIT_4_PIM);
        grlc_data->tm.n_acc_req_procedures = 0;
        grlc_data->tm.n_res_req            = 0; /* reset counter of resource requests during access */
        vsi_t_stop(GRLC_handle,T3168);
        return;
      }

      if (rel_ul AND grlc_data->tbf_type EQ TBF_TYPE_CONC)
      {
        sig_tm_ru_abrel(1, FALSE);
        tm_send_tbf_rel (grlc_data->tbf_type);          
        tm_deactivate_tbf(grlc_data->tbf_type);
        grlc_data->tm.n_acc_req_procedures = 0;
        grlc_data->tm.n_res_req            = 0; /* reset counter of resource requests during access */
        vsi_t_stop(GRLC_handle,T3168);
      }
      else
      {
        tm_send_tbf_rel (TBF_TYPE_DL);          
        tm_deactivate_tbf(TBF_TYPE_DL);
      }

      switch( grlc_data->tbf_type )
      {
        case  TBF_TYPE_NULL:      
          SET_STATE(TM,TM_WAIT_4_PIM);
          vsi_t_stop(GRLC_handle,T3168);           /* stop timer for sending chan req desc in dl ack/nack if running */
          break;
        case  TBF_TYPE_UL:
          /*
           * nothing to do, ul tbf is running
           */
          break;
        default:
          TRACE_ERROR( "tm_deactivate_tbf did not work correct in sig_rd_tm_end_of_tbf" );
          break;
      }
      break;
    default:
      TRACE_ERROR( "SIG_RD_TM_END_OF_TBF unexpected" );
      break;
  }
} /* sig_rd_tm_end_of_tbf() */


/*
+------------------------------------------------------------------------------
| Function    : sig_rd_tm_ul_req_cnf
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_RD_TM_UL_REQ_CNF
|               RD have to send that inter signal after it sends a Packet Downlink
|               AckNack including Channel Request Description.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_rd_tm_ul_req_cnf ( void ) 
{ 
  TRACE_ISIG( "sig_rd_tm_ul_req_cnf" );
  
  switch( GET_STATE( TM ) )
  {
    case TM_PTM:
      /*
      TRACE_EVENT_P1("T3168 start: %d ms",((grlc_data->t3168_val)+1)*500);
      */
      vsi_t_start(GRLC_handle,T3168,((grlc_data->t3168_val)+1)*500);
      break;
    default:
      TRACE_ERROR( "SIG_RD_TM_UL_REQ_CNF unexpected" );
      break;
  }
} /* sig_rd_tm_ul_req_cnf() */








/*
+------------------------------------------------------------------------------
| Function    : sig_ru_tm_end_of_pdu
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_RU_TM_END_OF_PDU 
|
| Parameters  : index_of_next_llc_pdu_i -  next LLC-PDU to send
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ru_tm_end_of_pdu ( UBYTE index_of_next_llc_pdu_i ) 
{ 
  TRACE_ISIG( "sig_ru_tm_end_of_pdu" );

  switch( GET_STATE( TM ) )
  {
    case TM_PTM:
      if(grlc_data->prim_queue[index_of_next_llc_pdu_i].re_allocation)
      {
        T_U_GRLC_RESOURCE_REQ resource_req; /*lint !e813*/

        tm_ini_realloc(index_of_next_llc_pdu_i);
        tm_build_res_req(&resource_req,
                         R_RE_ALLOC);
        tm_store_ctrl_blk( CGRLC_BLK_OWNER_TM, ( void* )&resource_req );
      }
      break;
    default:
      TRACE_ERROR( "SIG_RU_TM_END_OF_PDU unexpected" );
      break;
  }
} /* sig_ru_tm_end_of_pdu() */


/*
+------------------------------------------------------------------------------
| Function    : sig_ru_tm_prim_delete
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_RU_TM_PRIM_DELETE 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ru_tm_prim_delete ( void ) 
{ 
  TRACE_ISIG( "sig_ru_tm_prim_delete" );


  switch( GET_STATE( TM ) )
  {
    case TM_PTM:
      grlc_delete_prim();
      if( (grlc_data->testmode.mode EQ CGRLC_TEST_RANDOM) AND
          (grlc_data->testmode.n_pdu)  )
      {
        /*
         * queue one new pdu
         */
        TRACE_EVENT_P1("testmode a %ld pdus",grlc_data->testmode.n_pdu);
        grlc_data->testmode.n_pdu--;
        tm_queue_test_mode_prim(1);        
      }
      tm_handle_grlc_ready_ind();
      break;

    default:
      TRACE_ERROR( "SIG_RU_TM_PRIM_DELETE unexpected" );
      break;
  }
} /* sig_ru_tm_prim_delete() */





/*
+------------------------------------------------------------------------------
| Function    : sig_ru_tm_ctrl_blk_sent
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_RU_TM_CTRL_BLK_SENT
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ru_tm_ctrl_blk_sent ( UBYTE index )
{ 
  TRACE_ISIG( "sig_ru_tm_ctrl_blk_sent" );

  if( index < MAX_CTRL_BLK_NUM )
  {
    UBYTE i = 0;

    if( grlc_data->tm.ul_ctrl_blk.blk[index].owner EQ CGRLC_BLK_OWNER_TM )
    {
      grlc_data->tm.n_res_req++;
  
      if((grlc_t_status(T3168) > 0)) 
      {
#ifdef _SIMULATION_
          TRACE_EVENT("prr is transmitted once again for re-allocation");
#endif
          vsi_t_stop(GRLC_handle,T3168);
          grlc_data->tm.n_res_req--;
      }

      TRACE_EVENT_P2("T3168 start: %d ms, n_acc=%d",((grlc_data->t3168_val)+1)*500, grlc_data->tm.n_res_req);        
      vsi_t_start(GRLC_handle,T3168,(grlc_data->t3168_val+1)*500); 
    }

    while( i                               <   MAX_CTRL_BLK_NUM AND 
           grlc_data->tm.ul_ctrl_blk.seq[i] NEQ MAX_CTRL_BLK_NUM AND
           grlc_data->tm.ul_ctrl_blk.seq[i] NEQ index                )
    {
      i++;
    }

    if( i                               <  MAX_CTRL_BLK_NUM AND
        grlc_data->tm.ul_ctrl_blk.seq[i] EQ index                )
    {
      PALLOC(cgrlc_ctrl_msg_sent_ind,CGRLC_CTRL_MSG_SENT_IND);
      PSEND(hCommGRR,cgrlc_ctrl_msg_sent_ind);
      tm_set_stop_ctrl_blk( TRUE, CGRLC_BLK_OWNER_NONE, i );
    }
    else
    {
      TRACE_ERROR( "sig_ru_tm_ctrl_blk_sent: expected block not found" );
    }
  }
  else
  {
    TRACE_ERROR( "sig_ru_tm_ctrl_blk_sent: index >= MAX_CTRL_BLK_NUM" );
  }
} /* sig_ru_tm_ctrl_blk_sent() */





/*
+------------------------------------------------------------------------------
| Function    : sig_ru_tm_repeat_alloc
+------------------------------------------------------------------------------
| Description : Handles the internal signal sig_ru_tm_repeat_alloc 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ru_tm_repeat_alloc( void )
{ 
  MCAST(d_ul_ack,D_GRLC_UL_ACK);

  TRACE_ISIG( "sig_ru_tm_repeat_alloc" );

  switch( GET_STATE( TM ) )
  {
    case TM_PTM:
      if(d_ul_ack->gprs_ul_ack_nack_info.v_f_alloc_ack EQ 0 )
      {
        /*
         * if no fix alloc is present in last uplink ack/nack, delete any existing repeat alloc
         */
        if(grlc_data->uplink_tbf.fa_manag.fa_type EQ FA_REPEAT)
        {
          /*kill reapeat alloc, which is not yet started*/
          grlc_data->uplink_tbf.fa_manag.fa_type        = FA_NO_NEXT;
          grlc_data->uplink_tbf.fa_manag.repeat_alloc   = FALSE;
          grlc_data->uplink_tbf.fa_manag.ul_res_sum    -= grlc_data->uplink_tbf.fa_manag.next_alloc.ul_res_sum;
        }
        else if(grlc_data->uplink_tbf.fa_manag.fa_type       EQ FA_NO_NEXT AND 
                grlc_data->uplink_tbf.fa_manag.repeat_alloc  EQ TRUE )
        {
          T_RLC_VALUES  rlc_val;
          grlc_get_sdu_len_and_used_ts( &rlc_val);
          grlc_data->uplink_tbf.fa_manag.ul_res_remain = grlc_data->uplink_tbf.fa_manag.ul_res_sum - rlc_val.cnt_ts;
          if(grlc_data->uplink_tbf.fa_manag.ul_res_remain EQ grlc_data->uplink_tbf.fa_manag.current_alloc.ul_res_sum)
          {
            /*kill reapeat alloc if is mot yet started*/
            grlc_data->uplink_tbf.fa_manag.fa_type        = FA_NO_CURRENT;
            grlc_data->uplink_tbf.fa_manag.repeat_alloc   = FALSE;
            grlc_data->uplink_tbf.fa_manag.ul_res_sum    -= grlc_data->uplink_tbf.fa_manag.current_alloc.ul_res_sum;
          }
        }
        return;
      }
      vsi_t_stop(GRLC_handle,T3188);

      /*
       * check if current allocation is active 
       */
      if(grlc_data->uplink_tbf.fa_manag.fa_type EQ FA_NO_CURRENT  AND
         d_ul_ack->gprs_ul_ack_nack_info.f_alloc_ack.v_ts_overr )
      {
        ULONG next_start_fn;
        ULONG next_end_fn;
        /* repeat allocation in packet uplink ack/nack received,
         * no fixed allocation active, find out the next natural boundary
         * and store new allocation in current allocation
         */
        grlc_data->uplink_tbf.fa_manag.fa_type        = FA_NO_NEXT;
        grlc_data->uplink_tbf.fa_manag.repeat_alloc   = TRUE;
        grlc_data->uplink_tbf.fa_manag.current_alloc.ul_res_sum = 
        tm_set_fa_bitmap((UBYTE)(d_ul_ack->gprs_ul_ack_nack_info.f_alloc_ack.ts_overr /*& grlc_data->uplink_tbf.ts_usage*/), 
                            &grlc_data->uplink_tbf.fa_manag.current_alloc);
        grlc_data->uplink_tbf.fa_manag.ul_res_sum += grlc_data->uplink_tbf.fa_manag.current_alloc.ul_res_sum;
        /*find  start of alloc (natural boundary of next allocation)*/
        next_end_fn = grlc_data->uplink_tbf.fa_manag.current_alloc.alloc_end_fn;
        do
        {
          USHORT len;
          len = grlc_data->uplink_tbf.fa_manag.current_alloc.alloc.bitmap_len-1;
          next_start_fn = grlc_decode_tbf_start_rel(next_end_fn,0);
          next_end_fn   = grlc_decode_tbf_start_rel(next_start_fn,(USHORT)(len-1));
          if(grlc_data->ul_fn EQ next_start_fn)
            break;
          else if((next_end_fn > next_start_fn)   AND 
                  (grlc_data->ul_fn <= next_start_fn) AND
                  (grlc_data->ul_fn > next_start_fn) )
          {
            next_start_fn = grlc_decode_tbf_start_rel(next_end_fn,0);
            next_end_fn   = grlc_decode_tbf_start_rel(next_start_fn,(USHORT)(len-1));
            break;
          }
          else if( (next_end_fn < next_start_fn) AND
                   (
                    ((next_end_fn >= grlc_data->ul_fn) AND (next_start_fn > grlc_data->ul_fn)) OR
                    ((next_end_fn <  grlc_data->ul_fn) AND (next_start_fn < grlc_data->ul_fn))
                   )
                 )
          {
            next_start_fn = grlc_decode_tbf_start_rel(next_end_fn,0);
            next_end_fn = grlc_decode_tbf_start_rel(next_start_fn,(USHORT)(len-1));
            break;
          }
        }
        while (1);
        grlc_data->uplink_tbf.fa_manag.current_alloc.alloc_start_fn = next_start_fn;
        grlc_data->uplink_tbf.fa_manag.current_alloc.alloc_end_fn   = next_end_fn;          
        return;
      }
      else if(d_ul_ack->gprs_ul_ack_nack_info.f_alloc_ack.v_ts_overr )
      {
        UBYTE len;
        /* repeat allocation in packet uplink ack/nack received,
         * current allocation is active, store new alloc in next alloc
         * overwriting is possibe.
         */        
        grlc_data->uplink_tbf.fa_manag.fa_type        = FA_REPEAT;
        grlc_data->uplink_tbf.fa_manag.repeat_alloc   = TRUE;
        grlc_data->uplink_tbf.fa_manag.next_alloc.ul_res_sum = 
           tm_set_fa_bitmap((UBYTE)(d_ul_ack->gprs_ul_ack_nack_info.f_alloc_ack.ts_overr /*& grlc_data->uplink_tbf.ts_usage*/),&grlc_data->uplink_tbf.fa_manag.next_alloc);
        grlc_data->uplink_tbf.fa_manag.ul_res_sum += grlc_data->uplink_tbf.fa_manag.next_alloc.ul_res_sum;
        
        grlc_data->uplink_tbf.fa_manag.next_alloc.alloc_start_fn = 
            grlc_decode_tbf_start_rel(grlc_data->uplink_tbf.fa_manag.current_alloc.alloc_end_fn,0);
        len = grlc_data->uplink_tbf.fa_manag.current_alloc.alloc.bitmap_len-1;
        grlc_data->uplink_tbf.fa_manag.next_alloc.alloc_end_fn   =
            grlc_decode_tbf_start_rel(grlc_data->uplink_tbf.fa_manag.next_alloc.alloc_start_fn,
                                     (USHORT)(len-1));
        return;
      }
      grlc_data->uplink_tbf.fa_manag.repeat_alloc   = FALSE;

      break;
    default:
      TRACE_ERROR( "sig_ru_tm_repeat_alloc unexpected" );
      break;
  }
} /* sig_ru_tm_repeat_alloc() */


/*
+------------------------------------------------------------------------------
| Function    : sig_ru_tm_end_of_fix_alloc
+------------------------------------------------------------------------------
| Description : Handles the internal signal sig_ru_tm_end_of_fix_alloc,
|               received from ru if the end of a fixed allocation is reached.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ru_tm_end_of_fix_alloc(void)
{ 
  TRACE_ISIG( "sig_ru_tm_end_of_fix_alloc" );
  
  switch( GET_STATE( TM ) )
  {
    case TM_PTM:
      /*
       * if no new allocation is present start timer T3188
       * otherwise set next alloc to current alloc and update
       * grr_data->uplink_tbf.end_fix_alloc_fn
       */
      if (grlc_data->uplink_tbf.fa_manag.fa_type EQ FA_NO_NEXT)
      {
        grlc_data->uplink_tbf.fa_manag.fa_type = FA_NO_CURRENT;
        vsi_t_start(GRLC_handle,T3188,T3188_VALUE);
      }
      else if(grlc_data->uplink_tbf.fa_manag.fa_type EQ FA_BITMAP  OR
              grlc_data->uplink_tbf.fa_manag.fa_type EQ FA_REPEAT)
      {        
        grlc_data->uplink_tbf.fa_manag.fa_type          = FA_NO_NEXT;
        grlc_data->uplink_tbf.fa_manag.repeat_alloc     = FALSE;
        grlc_data->uplink_tbf.fa_manag.current_alloc    = grlc_data->uplink_tbf.fa_manag.next_alloc;
        /*
         * send packet rsource request if more uplink reseources is needed
         */
        tm_handle_final_alloc(0); /*calc ul resources*/ 
        if(grlc_data->uplink_tbf.fa_manag.tbf_oct_cap_remain < grlc_data->uplink_tbf.fa_manag.tbf_oct_cnt)
        { 
          T_U_GRLC_RESOURCE_REQ resource_req;  /*lint !e813*/

          /* send packet resource request*/
          tm_build_res_req (&resource_req, R_FIX_RE_ALLOC);
        }
      }
      break;
    default:
      TRACE_ERROR( "sig_ru_tm_end_of_fix_alloc unexpected" );
      break;
  }
} /* sig_ru_tm_end_of_fix_alloc() */


#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
/*
+------------------------------------------------------------------------------
| Function    : sig_ru_tm_ul_re_est_tbf
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_RU_TM_UL_RE_EST_TBF
|               RU have to send that inter signal after it has send resouurce request
|               by sending PCA or PRR.
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ru_tm_ul_re_est_tbf ( void ) 
{ 

  TRACE_ISIG( "sig_ru_tm_ul_re_est_tbf" );
  
  switch( GET_STATE( TM ) )
  {
    case TM_PTM:      
      /* start T3168 timer */
      vsi_t_start(GRLC_handle,T3168,((grlc_data->t3168_val)+1)*500);	  
      TRACE_EVENT("sig_ru_tm_ul_re_est_tbf : Timer T3168 started for tbf on pacch");

      /*GFF GFF_ACTIVE --> GFF_TWO_PHASE*/
      sig_tm_gff_ul_deactivate();
      sig_tm_gff_ul_activate(GFF_TWO_PHASE);


      /* TBF_TYPE_UL --> TBF_TYPE_NULL --> TBF_TYPE_TP_ACCESS */

      tm_deactivate_tbf(TBF_TYPE_UL);  
      tm_activate_tbf ( TBF_TYPE_TP_ACCESS );
      
      grlc_data->uplink_tbf.ti = 0;
      grlc_data->ul_tfi        = 0xFF;

      grlc_data->tm.pacch_prr_pca_sent = TRUE; /* PCA or PRR sent , 1st phase access*/ 
     
      grlc_data->tm.n_acc_req_procedures = 0;
      grlc_data->tm.n_res_req            = 0;

      grlc_data->pfi_value = 
        grlc_data->prim_queue[grlc_data->prim_start_tbf].prim_ptr->pkt_flow_id[0];
      
      
      /* new rlc mode of tbf on pacch */
      if ( grlc_data->prim_queue[grlc_data->prim_start_tbf].prim_type EQ 
           CGRLC_LLC_PRIM_TYPE_UNITDATA_REQ 
         )
      {
        grlc_data->uplink_tbf.rlc_mode = CGRLC_RLC_MODE_UACK; 
      }
      else
      {
        grlc_data->uplink_tbf.rlc_mode = CGRLC_RLC_MODE_ACK; 
      }
      
      {
        PALLOC(cgrlc_ul_tbf_ind,CGRLC_UL_TBF_IND);
        cgrlc_ul_tbf_ind->tbf_est_pacch = TRUE;
        PSEND(hCommGRR,cgrlc_ul_tbf_ind);
      }                  
      break;
    default:
      TRACE_ERROR( "SIG_RU_TM_UL_RE_EST_TBF unexpected" );
      break;
  }
} /*sig_ru_tm_ul_re_est_tbf*/



/*
+------------------------------------------------------------------------------
| Function    : sig_gff_tm_mac_ready_ind_two_phase
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_GFF_TM_MAC_READY_IND_TWO_PHASE
|
| Parameters  : mac_ready_ind describes the MAC_READY_IND primitive.
|               
|
+------------------------------------------------------------------------------
*/
void sig_gff_tm_mac_ready_ind_two_phase(T_MAC_READY_IND *mac_ready_ind)
{
  TRACE_ISIG( "sig_gff_tm_mac_ready_ind_two_phase" );     
  
  switch( grlc_data->tbf_type )
  {
    case TBF_TYPE_TP_ACCESS:
      if(mac_ready_ind->rlc_blocks_sent)
      {
        grlc_data->ru.ul_data[0].block_status = 0;
      
         /*If the timer is not already running then start the timer*/
        if(!(grlc_t_status(T3168) > 0))
        {
          /* This case would come in case when 
           * PRR has been sent in 2 phase access
           */
          vsi_t_start(GRLC_handle,T3168,((grlc_data->t3168_val)+1)*500);
          TRACE_EVENT_P3( "2PA: PRR sent at fn=%ld t3168=%ld | st_fn=%ld "
                                                                        ,mac_ready_ind->fn
                                                                        ,grlc_t_status(T3168)        
                                                                        ,grlc_data->ul_tbf_start_time );
        }
      }
      else 
      {
        if(!(grlc_t_status(T3168) > 0) AND
           grlc_data->ru.ul_data[0].block_status EQ 2) /* block still programmed but not sent */
        {
          /*Check if TBF starting time is elapsed*/
          if(grlc_check_if_tbf_start_is_elapsed ( grlc_data->ul_tbf_start_time, mac_ready_ind->fn))
          {
            TRACE_EVENT_P3("WAITING FOR Packet Resource Req Sending over st=%ld fn=%ld, ta_val=%d",grlc_data->ul_tbf_start_time
              ,mac_ready_ind->fn,grlc_data->ta_value);
            tm_handle_error_ra();
            return;
          }
        }
      }
      break;
      
    default:
      TRACE_ERROR( "SIG_GFF_TM_MAC_READY_IND_TWO_PHASE unexpected" );
      break;
  }
}
#endif


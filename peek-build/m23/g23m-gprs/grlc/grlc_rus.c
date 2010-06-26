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
|             RU of entity GRLC.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRLC_RUS_C
#define GRLC_RUS_C
#endif

#define ENTITY_GRLC

/*==== INCLUDES =============================================================*/

#include <stdio.h>
#include "typedefs.h"    /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "ccdapi.h"     /* to get CCD API */
#include "cnf_grlc.h"    /* to get cnf-definitions */
#include "mon_grlc.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "message.h"
#include "grlc.h"        /* to get the global entity definitions */
#include "grlc_f.h"      
#include "grlc_tms.h"
#include "grlc_ruf.h"
#include <string.h>


/*==== CONST ================================================================*/
/*#ifdef TRACE_ERROR
#undef TRACE_ERROR
#define TRACE_ERROR(x)
#endif
#ifdef TRACE_FUNCTION
#undef TRACE_FUNCTION
#define TRACE_FUNCTION(x)
#endif
#ifdef TRACE_ISIG
#undef TRACE_ISIG
#define TRACE_ISIG(x)
#endif*/
/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : sig_tm_ru_assign
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TM_RU_ASSIGN
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tm_ru_assign ( void) 
{ 
  TRACE_ISIG( "sig_tm_ru_assign" );
  
  switch( GET_STATE( RU ) )
  {
    case RU_WAIT_FOR_FIRST_CALL_ACK:
    case RU_WAIT_FOR_FIRST_CALL_UACK:
      /* 
       * reorg l1 one
       */
      {
        grlc_data->ru.write_pos_index -= grlc_data->ru.nts_max;      
      }
      TRACE_EVENT_P3("UL REASSIG during tbf starting time is running:%ld ul_tfi=%d st_fn_tfi=%d"
                                                                  ,grlc_data->ul_tbf_start_time
                                                                  ,grlc_data->ul_tfi
                                                                  ,grlc_data->start_fn_ul_tfi);

      /*lint -fallthrough*/

    case RU_NULL: 
      if(grlc_data->ul_tfi EQ 0xFF)
        grlc_data->ul_tfi = grlc_data->start_fn_ul_tfi;

	  switch( grlc_data->uplink_tbf.rlc_mode )
      {        
      case CGRLC_RLC_MODE_ACK:
      SET_STATE(RU,RU_WAIT_FOR_FIRST_CALL_ACK);
        ru_tbf_init(); 
        /*
         * sending the first block of the tbf
         */
        grlc_data->ru.N3104 = 0;

        while( grlc_data->ru.nts  AND
               tm_get_num_ctrl_blck( ) NEQ 0     )
        { /* 
           * next uplink block is a control block, 
           *  check if countdown procedure is statred or not
           */
          ru_send_control_block( );
        }     
        while(grlc_data->ru.nts AND grlc_data->ru.sdu_len)
        {
          ru_new_data();
        }
        while(grlc_data->ru.nts)
        {
          ru_ret_bsn();
        }
        break;
      case CGRLC_RLC_MODE_UACK:
      SET_STATE(RU,RU_WAIT_FOR_FIRST_CALL_UACK);
        ru_tbf_init();
        while( grlc_data->ru.nts  AND
               tm_get_num_ctrl_blck( ) NEQ 0     )
        { /* 
           * next uplink block is a control block, 
           *  check if countdown procedure is statred or not
           */
          ru_send_control_block( );
        }        
        while(grlc_data->ru.nts AND grlc_data->ru.sdu_len)
        {
          ru_new_data();
        }
        if(grlc_data->ru.cv EQ 0)
        {
          while( grlc_data->ru.nts 
                 AND (grlc_data->ru.count_cv_0 < 4) )
          {
            UBYTE bsn;
            bsn  = grlc_data->ru.vs - 1;
            bsn &= 0x7F;
            ru_send_mac_data_req(bsn);
            grlc_data->ru.count_cv_0++;
          }
        }
       break;
      default:
        TRACE_ERROR( "unknown RLC MODE for UL" );
        break;
      }      
      break;
    case RU_ACK:
    case RU_REL_ACK:      
    case RU_UACK:
    case RU_REL_UACK:
      /* 
       * store new params, modification at reaching starting time
       */
      grlc_data->ru.next_tbf_params   = grlc_data->uplink_tbf;
      grlc_data->ru.v_next_tbf_params = TRUE;
      break;
    default:
      TRACE_ERROR( "SIG_TM_RU_ASSIGN unexpected" );
      break;
  }
} /* sig_tm_ru_assign() */

/*
+------------------------------------------------------------------------------
| Function    : sig_tm_ru_abrel
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TM_RU_ABREL
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tm_ru_abrel (ULONG fn,BOOL poll_i) 
{ 
  TRACE_ISIG( "sig_tm_ru_abrel" );
  
  /*
   * stop all uplink timers 
   */
  vsi_t_stop(GRLC_handle,T3164);
  vsi_t_stop(GRLC_handle,T3166);
  vsi_t_stop(GRLC_handle,T3180);
  vsi_t_stop(GRLC_handle,T3182);  
  vsi_t_stop(GRLC_handle,T3184);/*timer for FA, used in simulation test*/  
  vsi_t_stop(GRLC_handle,T3188);/*timer for FA, used in simulation test*/

  if (grlc_data->prim_start_tbf < PRIM_QUEUE_SIZE_TOTAL)
  {
    grlc_data->prim_queue[grlc_data->prim_start_tbf].rlc_status  = FALSE;
    grlc_data->prim_queue[grlc_data->prim_start_tbf].cv_status   = FALSE;
    grlc_data->prim_queue[grlc_data->prim_start_tbf].last_bsn    = 0xFF;
  }
   if(!poll_i)
  {
    /*
     * abnormal release: abort TBF
     */
    grlc_data->tbf_ctrl[grlc_data->ul_index].end_fn = grlc_data->ul_fn;
    grlc_data->tbf_ctrl[grlc_data->ul_index].vs_vr  = grlc_data->ru.vs;
    grlc_data->tbf_ctrl[grlc_data->ul_index].va_vq  = grlc_data->ru.va;
    grlc_data->tbf_ctrl[grlc_data->ul_index].cnt_ts = grlc_data->ru.cnt_ts;
    grlc_trace_tbf_par ( grlc_data->ul_index );
    SET_STATE(RU,RU_NULL);
  }
  else
  {
    SET_STATE(RU,RU_NET_REL);
    grlc_data->ru.poll_fn = fn;
    TRACE_EVENT_P2("UL:RU_NET_REL p.tbf rel c_fn=%ld poll_fn=%d",grlc_data->dl_fn,grlc_data->ru.poll_fn);
  }

} /* sig_tm_ru_abrel() */



/*
+------------------------------------------------------------------------------
| Function    : sig_tm_ru_queue_status
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TM_RU_QUEUE_STATUS
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tm_ru_queue_status ( void) 
{ 
  TRACE_ISIG( "sig_tm_ru_queue_status" );
  
  switch( GET_STATE( RU ) )
  {
    case RU_WAIT_FOR_FIRST_CALL_ACK:
    case RU_WAIT_FOR_FIRST_CALL_UACK:
    case RU_ACK:
    case RU_UACK:
      grlc_data->ru.rlc_octet_cnt = ru_recalc_rlc_oct_cnt();
      break;
    default:
      /*
       * nothing to do
       */
      break;
  }
} /* sig_tm_ru_queue_status() */




/*
+------------------------------------------------------------------------------
| Function    : sig_gff_ru_ul_ack
+------------------------------------------------------------------------------
| Description : Handles the Signal SIG_GFF_RU_UL_ACK
|
| Parameters  : fn - current frame number
|               tn - current timeslot number
|               rrbp - rrbp parameter indicated in mac header
|               sp - sp parameter indicated in mac header
|
+------------------------------------------------------------------------------
*/

GLOBAL void sig_gff_ru_ul_ack (ULONG fn , UBYTE tn, UBYTE rrbp, UBYTE sp) 
{ 
  MCAST(d_ul_ack,D_GRLC_UL_ACK);

  TRACE_ISIG( "sig_gff_ru_ul_ack" );

#ifdef REL99
  /* Handling abnormal case
   * 8.7 While a TBF is in progress, if a mobile station receives a 
   * PACKET UPLINK ASSIGNMENT, PACKET UPLINK ACK/NACK or PACKET TIMESLOT RECONFIGURE 
   * message with message escape bit indicating EGPRS (resp. GPRS) contents whereas 
   * the current TBF mode is GPRS (resp. EGPRS), the mobile station shall ignore the 
   * message.
   */
  if(d_ul_ack->egprs_flag)
  {
    TRACE_ERROR( "EGPRS UL ACK/NACK received" );
    return;
  }
#endif

  grlc_data->tbf_ctrl[grlc_data->ul_index].ack_cnt++;
  
  
  if(grlc_data->uplink_tbf.mac_mode EQ CGRLC_MAC_MODE_FA)
  {
    vsi_t_stop(GRLC_handle,T3184);
    if(!d_ul_ack->gprs_ul_ack_nack_info.ack_nack_des.f_ack_ind)
    {
      vsi_t_start(GRLC_handle,T3184,T3184_VALUE);
      TRACE_EVENT("sig_gff_ru_ul_ack: T3184 started for FA");
    }
    sig_ru_tm_repeat_alloc();
  }

  switch( GET_STATE( RU ) )
  {    
    case RU_ACK     :
      if(ru_contention_resolution())
      {
        SET_STATE(RU,RU_NULL);
        return;
      }
      ru_update_vb();
      grlc_data->ru.va = ru_calc_va();  
      ru_delete_prims(grlc_data->ru.va);
      ru_handle_stall_ind();
      break;
    case RU_REL_ACK :
      /* 
       * For proper release of ul tbf
       */
      vsi_t_stop(GRLC_handle,T3182);
      if(ru_contention_resolution())
      {
        SET_STATE(RU,RU_NULL);
        return;
      }
      if(sp)
      {
        grlc_data->ru.poll_fn = grlc_calc_new_poll_pos(fn,rrbp);
        grlc_data->ru.poll_tn = tn;
      }
      if( !(d_ul_ack->gprs_ul_ack_nack_info.ack_nack_des.f_ack_ind) )
      {
        ru_update_vb();
        grlc_data->ru.va = ru_calc_va();
        ru_delete_prims(grlc_data->ru.va);
        ru_handle_stall_ind();
      }
      else if(d_ul_ack->gprs_ul_ack_nack_info.ack_nack_des.f_ack_ind AND 
              grlc_data->ru.cv EQ 0)
      {          
        SET_STATE(RU,RU_SEND_PCA);
        ru_delete_prims(grlc_data->ru.vs);
        grlc_data->ru.va                            = grlc_data->ru.vs;
        grlc_data->ru.nr_nacked_blks                = 0;
        grlc_data->tbf_ctrl[grlc_data->ul_index].fbi = 1;
        grlc_data->ru.reorg_l1_needed	             = TRUE;    /* delete blocks, dummies will be placed */
        if(!sp)
        {
          SET_STATE(RU,RU_NULL);
          TRACE_EVENT("FAI = 1 received but no poll");   
          vsi_t_stop(GRLC_handle,T3180);
          sig_ru_tm_error_ra();
        }
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
        else
        { /* network wishes to re-estab tbf on pacch */
          if(d_ul_ack->gprs_ul_ack_nack_info.release_99_str_d_ul_ack.tbf_est EQ 1 AND
             grlc_data->prim_start_tbf NEQ END_OF_LIST  AND /*UL data is present*/
             grlc_data->tbf_type       NEQ TBF_TYPE_CONC    /* No DL TBF */
            )
          {
            grlc_data->ru.tbf_re_est = TRUE;
            grlc_data->next_poll_array[grlc_data->poll_start_tbf].poll_type[tn] = CGRLC_POLL_RE_ASS;
          }

        }
#endif
      }      
      else
      {        
        /* 
         * if the mobile station has not started or has not completed the countdown procedure 
         * and it receives a Packet Uplink Ack/Nack with the Final Ack Indicator set, 
         * it shall perform an abnormal release with random access.
         */
        SET_STATE(RU,RU_NULL);
        TRACE_ERROR( "FBI set, but cv > 0" );   
        sig_ru_tm_error_ra();
      }
      break;
    case RU_UACK:
      grlc_data->ru.va = grlc_data->ru.vs;
      ru_handle_stall_ind();
      break;
    case RU_REL_UACK:
      /* 
       * For proper release of ul tbf
       */
      vsi_t_stop(GRLC_handle,T3182);
      grlc_data->ru.va = grlc_data->ru.vs;
      ru_handle_stall_ind();
      if(sp)
      {
        grlc_data->ru.poll_fn = grlc_calc_new_poll_pos(fn,rrbp);
        grlc_data->ru.poll_tn = tn;
        if( d_ul_ack->gprs_ul_ack_nack_info.ack_nack_des.f_ack_ind)
        {          
          SET_STATE(RU,RU_SEND_PCA);
          ru_delete_prims(grlc_data->ru.vs);

#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
          if(d_ul_ack->gprs_ul_ack_nack_info.release_99_str_d_ul_ack.tbf_est EQ 1 AND
             grlc_data->prim_start_tbf NEQ END_OF_LIST  AND /*UL data is present*/
             grlc_data->tbf_type       NEQ TBF_TYPE_CONC    /* No DL TBF */
            )
          {
            grlc_data->ru.tbf_re_est = TRUE;
            grlc_data->next_poll_array[grlc_data->poll_start_tbf].poll_type[tn] = CGRLC_POLL_RE_ASS;
          }
#endif
        }  
      }
      /*
       * this part is a workaround to pass GCF 14.16.1
       * R&S is not setting the sp bit in final ack
       */
      else if( d_ul_ack->gprs_ul_ack_nack_info.ack_nack_des.f_ack_ind                AND
              (grlc_data->ru.cv EQ 0)                          AND
              (grlc_data->testmode.mode EQ CGRLC_TEST_RANDOM))
      {
        TRACE_EVENT("SP bit not set in PULACK, FBI BIT SET --> PERFORM IMMEDIATE RELEASE DURING TMA");
        sig_ru_tm_error_ra();
      }
      else if( d_ul_ack->gprs_ul_ack_nack_info.ack_nack_des.f_ack_ind                AND
              (grlc_data->testmode.mode EQ CGRLC_TEST_RANDOM))
      {
        TRACE_EVENT_P3("SP=0,FBI=1,CV=%d-> rlc_oct=%ld,sdu_l=%ld"
                                                                ,grlc_data->ru.cv
                                                                ,grlc_data->ru.rlc_octet_cnt
                                                                ,grlc_data->ru.sdu_len);
      }
      break;
    case RU_SEND_PCA :
      if(sp)
      {
        grlc_data->ru.poll_fn = grlc_calc_new_poll_pos(fn,rrbp);
        grlc_data->ru.poll_tn = tn;
        TRACE_EVENT_P1("NEW RU POLL FN AT =%ld ",grlc_data->ru.poll_fn);

#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
        /* 
         * tbf re-establishment on pacch 
         */
        if(  d_ul_ack->gprs_ul_ack_nack_info.release_99_str_d_ul_ack.tbf_est         EQ  1            AND /* network wishes to re-estab tbf on pacch */
             grlc_data->prim_start_tbf NEQ END_OF_LIST  AND /* ul data is present*/
             grlc_data->tbf_type       NEQ TBF_TYPE_CONC    /* no dl tbf */
          )
        {
          grlc_data->ru.tbf_re_est = TRUE;
          grlc_data->next_poll_array[grlc_data->poll_start_tbf].poll_type[tn] = CGRLC_POLL_RE_ASS;           
        }
#endif
      }    
      break;
    default:
      TRACE_ERROR( "SIG_GFF_RU_UL_ACK unexpected" );
      break;
  }
  if(grlc_data->ru.cs_type NEQ (T_CODING_SCHEME)d_ul_ack->gprs_ul_ack_nack_info.chan_coding_cmd)
  {
    TRACE_EVENT_P7("ul ack:CS changed from %d to %d, ssn=%d, va=%d, vs=%d, neg_acked_blks=%d, fbi=%d ",
                                                grlc_data->ru.cs_type,
                                                d_ul_ack->gprs_ul_ack_nack_info.chan_coding_cmd,
                                                d_ul_ack->gprs_ul_ack_nack_info.ack_nack_des.ssn,
                                                grlc_data->ru.va,
                                                grlc_data->ru.vs,
                                                grlc_data->ru.nr_nacked_blks,
                                                d_ul_ack->gprs_ul_ack_nack_info.ack_nack_des.f_ack_ind);
    grlc_data->ru.cs_type = (T_CODING_SCHEME)d_ul_ack->gprs_ul_ack_nack_info.chan_coding_cmd;
    ru_change_of_cs(grlc_data->ru.cs_type);    
  }
  else if(grlc_data->ru.nr_nacked_blks OR d_ul_ack->gprs_ul_ack_nack_info.ack_nack_des.f_ack_ind)
  {
    ULONG rbb=0,i,dummy;
    for(i=0; i<d_ul_ack->gprs_ul_ack_nack_info.ack_nack_des.ssn;i++)
    {
      dummy = (USHORT)d_ul_ack->gprs_ul_ack_nack_info.ack_nack_des.rbb[WIN_SIZE-1-i];
      rbb  += dummy <<i; 
      if(i EQ 31)
        break;
    }
    TRACE_EVENT_P8("ul ack fn=%ld :ssn=%d,va=%d,vs=%d,neg_acked_blks=%d,fbi=%d,rbb=%lx,cnt_ts=%d",
                                                fn,
                                                d_ul_ack->gprs_ul_ack_nack_info.ack_nack_des.ssn,
                                                grlc_data->ru.va,
                                                grlc_data->ru.vs,
                                                grlc_data->ru.nr_nacked_blks,
                                                d_ul_ack->gprs_ul_ack_nack_info.ack_nack_des.f_ack_ind,
                                                rbb,
                                                grlc_data->ru.cnt_ts);
  }
  grlc_data->ru.bsn_ret = grlc_data->ru.va;


  
} /* sig_gff_ru_ul_ack() */


/*
+------------------------------------------------------------------------------
| Function    : sig_gff_ru_mac_ready_ind
+------------------------------------------------------------------------------
| Description : Handles the Signal SIG_GFF_RU_MAC_READY_IND
|
| Parameters  : *ptr_mac_ready_ind_i - Ptr to primitive MAC_READY_IND
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gff_ru_mac_ready_ind ( T_MAC_READY_IND * mac_ul_ready_ind) 
{ 
  UBYTE             bsn=0xFF;
  BOOL              release_tbf=FALSE;
  UBYTE             rd_state=GET_STATE( RD );
  UBYTE ctrl_blk_active_idx;
  BOOL realloc_prr_allowed = FALSE;

  TRACE_ISIG( "sig_gff_ru_mac_ready_ind" );


#if defined (_SIMULATION_)
      TRACE_EVENT_P1("fn = %ld",mac_ul_ready_ind->fn ); 
      TRACE_EVENT_P9("wpi=%d sent=%ld\n current.cnt=%d,bsn[0]=%d,bsn[1]=%d,bsn[2]=%d,bsn[3]=%d,nts=%d,nts_max=%d",
                                                      grlc_data->ru.write_pos_index,
                                                      mac_ul_ready_ind->rlc_blocks_sent,
                                                      grlc_data->ru.pl_retrans_current.cnt,
                                                      grlc_data->ru.pl_retrans_current.blk[0],
                                                      grlc_data->ru.pl_retrans_current.blk[1],
                                                      grlc_data->ru.pl_retrans_current.blk[2],
                                                      grlc_data->ru.pl_retrans_current.blk[3],
                                                      grlc_data->ru.nts,
                                                      grlc_data->ru.nts_max);
#endif /* defined (_SIMULATION_) */

  ru_switch_ul_buffer (mac_ul_ready_ind->rlc_blocks_sent);
  ru_check_pl_ret(mac_ul_ready_ind->rlc_blocks_sent);
  if(grlc_data->ru.v_next_tbf_params AND
     grlc_check_if_tbf_start_is_elapsed ( grlc_data->ul_tbf_start_time, ((mac_ul_ready_ind->fn+5)%FN_MAX)))
  {
    ru_handle_tbf_start_in_ptm( mac_ul_ready_ind->rlc_blocks_sent);
    ru_cgrlc_st_time_ind();
  }
  grlc_handle_poll_pos (mac_ul_ready_ind->fn);
  ru_handle_nts (mac_ul_ready_ind->rlc_blocks_sent);

  switch( GET_STATE( RU ) )
  {
    case RU_WAIT_FOR_FIRST_CALL_ACK:
      if( !grlc_data->ru.v_next_tbf_params AND
          !grlc_check_if_tbf_start_is_elapsed ( grlc_data->ul_tbf_start_time, ((mac_ul_ready_ind->fn+5)%FN_MAX)))
      {
        TRACE_EVENT_P2("WAIT FOR UL STARTING TIME, DL IS RUNNING st=%ld c_fn=%ld",grlc_data->ul_tbf_start_time
                                                                              ,mac_ul_ready_ind->fn);
        grlc_send_rem_poll_pos(mac_ul_ready_ind->fn);
        return;
      }
      if(grlc_data->ru.first_usf)
      {
        /* 
         * Tbf starting time is reached, start T3164 
         */
        vsi_t_start(GRLC_handle,T3164,T3164_VALUE);
      }
      SET_STATE(RU,RU_ACK);
      grlc_data->tbf_ctrl[grlc_data->ul_index].start_fn = mac_ul_ready_ind->fn;      
      if(!grlc_data->ru.cd_active AND 
        (grlc_data->ru.tbc <= grlc_data->uplink_tbf.bs_cv_max))
      {
        grlc_data->ru.cd_active = TRUE;
        ru_set_prim_queue(grlc_data->ru.cd_active);
      }
      ru_cgrlc_st_time_ind();
      TRACE_EVENT_P9 ("UL first call:fn=%ld (%ld) tfi=%d ul_mask=%x st_fn=%ld pdu_cnt=%d,PST=%d,PSF=%d ta=%d",
                                            mac_ul_ready_ind->fn,
                                            mac_ul_ready_ind->fn%42432,
                                            grlc_data->ul_tfi,                                           
                                            grlc_data->ul_tn_mask,
                                            grlc_data->ul_tbf_start_time,
                                            grlc_data->grlc_data_req_cnt,
                                            grlc_data->prim_start_tbf,
                                            grlc_data->prim_start_free,
                                            grlc_data->ta_value);

      /*lint -fallthrough*/

    case RU_ACK     :
      /*
       * Handling of counter N3104
       */
      if(ru_handle_n3104())
      {
        SET_STATE(RU,RU_NULL);
        TRACE_ERROR( "counter N3104 reaches its maximum, contention resolution has failed" );
        TRACE_EVENT_P7("va=%d,vs=%d,cnt_ts=%d,fn=%d n3104_max=%d,n3104=%d,bs_cv_max=%d",
                                                  grlc_data->ru.va,
                                                  grlc_data->ru.vs,
                                                  grlc_data->ru.cnt_ts,
                                                  mac_ul_ready_ind->fn,
                                                  grlc_data->ru.N3104_MAX,
                                                  grlc_data->ru.N3104,
                                                  grlc_data->uplink_tbf.bs_cv_max);
        sig_ru_tm_error_ra();
        return;
      }
      /* 
       * IS THERE A CONTROL BLOCK TO SEND ? 
       */
      while(  grlc_data->ru.nts              AND
             !grlc_data->ru.cd_active        AND
              tm_get_num_ctrl_blck( ) NEQ 0     )
      { /* 
         * next uplink block is a control block, 
         *  check if countdown procedure is statred or not
         */
        ru_send_control_block( );
      }

      /* 
       * IS RETRANSMISSION NEEDED ? 
       */
      while(grlc_data->ru.nts)
      { 
        ctrl_blk_active_idx = ru_peek_for_ctrl_blk();
        if ((ctrl_blk_active_idx EQ 0xFF) OR
          realloc_prr_allowed EQ TRUE)     /*No control block , form data block*/
        { 
          while( (grlc_data->ru.vb[grlc_data->ru.bsn_ret & WIN_MOD] EQ VB_NACKED) AND 
                  grlc_data->ru.nts)
          {
            ru_ret_bsn();
          }

          while(grlc_data->ru.nts) 
          {  
           /* 
            * IS THERE A STALL CONDITION ? 
            */
            if(grlc_data->ru.vs EQ ((grlc_data->ru.va + WIN_SIZE) & 0x7F))
            {
              ru_stall_ind();
            }
           /* 
            * IS THERE MORE DATA ?
            */   
            else if(grlc_data->ru.sdu_len)
            {
              ru_new_data();
            }
            else
            {
              break;
            }
          } 
          realloc_prr_allowed = FALSE;
          break;
        }
        else
        {
          
          TRACE_EVENT_P1("reallocation of llc pdu (index)=%d",ctrl_blk_active_idx);
          
          /* if already one PRR in L1 Buffer , replace it with new PRR */
          if (grlc_data->ru.pl_retrans_current.blk[0]
            EQ (BLK_INDEX_TM + OFFSET_CTRL_BLOCK_IDX))
          {      
            grlc_data->ru.write_pos_index--;
            grlc_data->ru.pl_retrans_current.cnt--;
            grlc_data->ru.nts++;  
            TRACE_EVENT("prr in l1 buffer queue,replace with new prr");
          }
          sig_ru_tm_end_of_pdu(ctrl_blk_active_idx);    
          ru_send_control_block();    
          realloc_prr_allowed = TRUE;      
        }
      }
      if( grlc_data->ru.cv EQ 0)
      {
        SET_STATE(RU,RU_REL_ACK);
        grlc_data->ru.bsn_ret = grlc_data->ru.va;  
      }
      while(grlc_data->ru.nts)
      {
        ru_ret_bsn();
      }
      grlc_send_rem_poll_pos (mac_ul_ready_ind->fn);
      break;
    case RU_REL_ACK :
      /*
       * All blocks are sent, wait for the last ack.
       * If uplink resources are available retransmit window
       */
      if(ru_handle_n3104())
      {
        SET_STATE(RU,RU_NULL);
        TRACE_ERROR( "counter N3104 reaches its maximum, contention resolution has failed" );
        sig_ru_tm_error_ra();
        grlc_data->ru.nts = 0; /*to avoid transmitting of data*/
        return;
      }

      /* 
       * IS THERE A CONTROL BLOCK TO SEND ? 
       */
      while(  grlc_data->ru.nts              AND      
              tm_get_num_ctrl_blck( ) NEQ 0  AND
              ru_ctrl_blk_selection_allowed())
      { 
        /* 
         * next uplink block is a control block,          
         */
        TRACE_EVENT("Ctrl blk selected in RU_REL_ACK");
        ru_send_control_block( );
      }
      while(grlc_data->ru.nts)
      { 
        ctrl_blk_active_idx = ru_peek_for_ctrl_blk();
        if ((ctrl_blk_active_idx EQ 0xFF) OR
          realloc_prr_allowed EQ TRUE)     /*No control block , form data block*/
        { 
          while(grlc_data->ru.nts AND (grlc_data->ru.va NEQ grlc_data->ru.vs))
          {
            ru_ret_bsn();
          }
      
          while(grlc_data->ru.nts)
          {
            TRACE_EVENT_P3("ALL BLKS ACKED, wait for fbi in PUAN wpi=%d t3182=%d t3180=%d"
                                                                ,grlc_data->ru.write_pos_index
                                                                ,grlc_t_status( T3182 )
                                                                ,grlc_t_status( T3180 ));
            ru_send_ul_dummy_block();
          }
          realloc_prr_allowed = FALSE;
          break;
        }
        else
        {
          
          TRACE_EVENT_P1("reallocation of llc pdu (index)=%d",ctrl_blk_active_idx);
          
          /* if already one PRR in L1 Buffer , replace it with new PRR */
          if (grlc_data->ru.pl_retrans_current.blk[0]
            EQ (BLK_INDEX_TM + OFFSET_CTRL_BLOCK_IDX))
          {      
            grlc_data->ru.write_pos_index--;
            grlc_data->ru.pl_retrans_current.cnt--;
            grlc_data->ru.nts++;  
            TRACE_EVENT("prr in l1 buffer queue,replace with new prr");
          }
          sig_ru_tm_end_of_pdu(ctrl_blk_active_idx);    
          ru_send_control_block();    
          realloc_prr_allowed = TRUE;      
        }
      }
      grlc_send_rem_poll_pos (mac_ul_ready_ind->fn);
      break;
    case RU_SEND_PCA:
      while(grlc_data->ru.nts)
      {
        ru_send_ul_dummy_block();
      }
#ifdef _TARGET_     
      if( grlc_check_dist(mac_ul_ready_ind->fn,grlc_data->ru.poll_fn,100) )
      {      
        if ((mac_ul_ready_ind->fn NEQ grlc_data->ru.poll_fn) AND 
            (mac_ul_ready_ind->last_poll_resp EQ 0))
        {
          release_tbf = TRUE;
          /*TRACE_EVENT_P2("REL TBF:fn =%ld  new ru_poll_fn=%ld",
                                                                mac_ul_ready_ind->fn,
                                                                grlc_data->ru.poll_fn);*/

        }
        else if(mac_ul_ready_ind->last_poll_resp)
        {
          grlc_data->ru.poll_fn +=104;
          grlc_data->ru.poll_fn %= FN_MAX;
          TRACE_ERROR("UL GAP: UL TBF NOT RELEASED +104");
          TRACE_EVENT_P3("fn =%ld   new ru_poll_fn=%ld poll_staus=%d",
                                                                mac_ul_ready_ind->fn,
                                                                grlc_data->ru.poll_fn,
                                                                mac_ul_ready_ind->last_poll_resp);
        }
        if(grlc_data->missed_poll_fn EQ grlc_data->ru.poll_fn)
        {
          release_tbf = FALSE;
          grlc_data->ru.poll_fn +=104;
          grlc_data->ru.poll_fn %= FN_MAX;
          TRACE_ERROR("DL GAP: UL TBF NOT RELEASED +104");
          TRACE_EVENT_P3("fn =%ld   missed poll=%ld new ru_poll_fn=%ld",
                                                                mac_ul_ready_ind->fn,
                                                                grlc_data->missed_poll_fn,
                                                                grlc_data->ru.poll_fn);
        }

      }
#endif
#ifdef _SIMULATION_
      if(mac_ul_ready_ind->fn EQ grlc_data->ru.poll_fn)
        release_tbf = TRUE;
#endif
      grlc_send_rem_poll_pos (mac_ul_ready_ind->fn);
      if(release_tbf) 
      {
        vsi_t_stop(GRLC_handle,T3182);
        vsi_t_stop(GRLC_handle,T3180);
        if ( grlc_data->poll_start_tbf NEQ 0xFF  AND
             (!grlc_check_if_tbf_start_is_elapsed(grlc_data->dl_tbf_start_time,grlc_data->next_poll_array[grlc_data->   
                                                                     poll_start_tbf].fn)                AND
             (rd_state EQ RD_WAIT_FOR_STARTING_TIME_ACK  OR
              rd_state EQ RD_WAIT_FOR_STARTING_TIME_UACK)) )
        {
          grlc_data->ru.poll_fn = grlc_data->next_poll_array[grlc_data->poll_start_tbf].fn;
          TRACE_EVENT_P2("UL DELAYED WHILE WAITING STARTING TIME DL poll_fn=%ld c_fn=%ld",grlc_data->next_poll_array[grlc_data->poll_start_tbf].fn
                                                                              ,mac_ul_ready_ind->fn);
        }
        else
        {
          SET_STATE(RU,RU_NULL);
          grlc_data->tbf_ctrl[grlc_data->ul_index].end_fn = grlc_data->ul_fn;
          grlc_data->tbf_ctrl[grlc_data->ul_index].vs_vr  = grlc_data->ru.vs;
          grlc_data->tbf_ctrl[grlc_data->ul_index].va_vq  = grlc_data->ru.va;
          grlc_data->tbf_ctrl[grlc_data->ul_index].cnt_ts = grlc_data->ru.cnt_ts;
          grlc_trace_tbf_par ( grlc_data->ul_index );

#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
          if(grlc_data->ru.tbf_re_est) 
          {
            /* 
             * signal tm to start timer 3168,re-establish tbf on pacch as poll sent succesfully 
             */
            sig_ru_tm_ul_re_est_tbf();
          }
          else 
#endif
          {
            sig_ru_tm_end_of_tbf();
          }
        }      
      }      
      break;

    case RU_WAIT_FOR_FIRST_CALL_UACK:
      if( !grlc_data->ru.v_next_tbf_params AND
          !grlc_check_if_tbf_start_is_elapsed ( grlc_data->ul_tbf_start_time, ((mac_ul_ready_ind->fn+5)%FN_MAX)))
      {
        TRACE_EVENT_P2("UACK MODE WAIT FOR UL STARTING TIME, DL IS RUNNING st=%ld c_fn=%ld",grlc_data->ul_tbf_start_time
                                                                              ,mac_ul_ready_ind->fn);
        grlc_send_rem_poll_pos(mac_ul_ready_ind->fn);
        return;
      }
      if(grlc_data->ru.first_usf)
      {
        /* 
         * Tbf starting time is reached, start T3164 
         */
        vsi_t_start(GRLC_handle,T3164,T3164_VALUE);
      }
      grlc_data->tbf_ctrl[grlc_data->ul_index].start_fn = mac_ul_ready_ind->fn;
      SET_STATE(RU,RU_UACK);
      if(!grlc_data->ru.cd_active AND 
        (grlc_data->ru.tbc <= grlc_data->uplink_tbf.bs_cv_max))
      {
        grlc_data->ru.cd_active = TRUE;
        ru_set_prim_queue(grlc_data->ru.cd_active);
      }
      ru_cgrlc_st_time_ind();
      TRACE_EVENT_P9 ("UL first call:fn=%ld (%ld) tfi=%d ul_mask=%x st_fn=%ld pdu_cnt=%d,PST=%d,PSF=%d ta=%d  UACK_MODE",
                                            mac_ul_ready_ind->fn,
                                            mac_ul_ready_ind->fn%42432,
                                            grlc_data->ul_tfi,                                           
                                            grlc_data->ul_tn_mask,
                                            grlc_data->ul_tbf_start_time,
                                            grlc_data->grlc_data_req_cnt,
                                            grlc_data->prim_start_tbf,
                                            grlc_data->prim_start_free,
                                            grlc_data->ta_value);
      /*lint -fallthrough*/

    case RU_UACK    :
      ru_del_prim_in_uack_mode ( mac_ul_ready_ind->rlc_blocks_sent);
      if(grlc_data->ru.last_bsn EQ LAST_BSN_STALL_CONDITION)
      { /*in uack mode is transmission at stall condition of data not allowed*/
        grlc_data->ru.nts = 0;
      }
      else if (grlc_data->ru.last_bsn EQ LAST_BSN_RESUME_UACK_MODE_AFTER_SI)
      { /* data transmission allowed after stall indication in rlc unackgnowledged mode*/
        grlc_data->ru.nts = grlc_data->ru.nts_max; 
        grlc_data->ru.last_bsn = LAST_BSN_NOT_BULIT;
        TRACE_EVENT(" STALL IND ELIMINATED IN RLC UACK");
      } 
      /* 
       * IS THERE A CONTROL BLOCK TO SEND ? 
       */
      while(  grlc_data->ru.nts              AND
             !grlc_data->ru.cd_active        AND
              tm_get_num_ctrl_blck( ) NEQ 0     )
      { 
        ru_send_control_block( );
      }
      /* 
       * IS THERE MORE DATA ?
       */
      while(grlc_data->ru.nts)
      { 
        ctrl_blk_active_idx = ru_peek_for_ctrl_blk();
        if ((ctrl_blk_active_idx EQ 0xFF) OR
          realloc_prr_allowed EQ TRUE)     /*No control block , form data block*/
        { 
          while( grlc_data->ru.nts AND 
                 grlc_data->ru.sdu_len )
          {
            ru_new_data();
          }
          if (grlc_test_mode_active())
          {
            /* 
             * to prevent stall indication in Testmode A/B
             */
            grlc_data->ru.va = grlc_data->ru.vs;
          }

          while( grlc_data->ru.nts 
                AND (grlc_data->ru.count_cv_0 < 4) )
          {
            bsn = (grlc_data->ru.vs - 1) % 128;
            ru_send_mac_data_req(bsn);
            grlc_data->ru.count_cv_0++;
          }
          while(grlc_data->ru.nts)
          {
            ru_send_ul_dummy_block();
          }
          realloc_prr_allowed = FALSE;
          break;
        }
        else
        {
          
          TRACE_EVENT_P1("reallocation of llc pdu (index)=%d",ctrl_blk_active_idx);
          
          /* if already one PRR in L1 Buffer , replace it with new PRR */
          if (grlc_data->ru.pl_retrans_current.blk[0]
            EQ (BLK_INDEX_TM + OFFSET_CTRL_BLOCK_IDX))
          {      
            grlc_data->ru.write_pos_index--;
            grlc_data->ru.pl_retrans_current.cnt--;
            grlc_data->ru.nts++;  
            TRACE_EVENT("prr in l1 buffer queue,replace with new prr");
          }
          sig_ru_tm_end_of_pdu(ctrl_blk_active_idx);    
          ru_send_control_block();    
          realloc_prr_allowed = TRUE;      
        }
      }

      if(grlc_data->ru.cv EQ 0)
      {
        SET_STATE(RU,RU_REL_UACK);
      }
      grlc_send_rem_poll_pos (mac_ul_ready_ind->fn);
      break;      
    case RU_REL_UACK:
      ru_del_prim_in_uack_mode ( mac_ul_ready_ind->rlc_blocks_sent);      
	  
      /* 
       * IS THERE A CONTROL BLOCK TO SEND ? 
       */
      while(  grlc_data->ru.nts              AND      
              tm_get_num_ctrl_blck( ) NEQ 0  AND
              ru_ctrl_blk_selection_allowed())
      { 
        /* 
         * next uplink block is a control block,          
         */
        TRACE_EVENT("Ctrl blk selected in RU_REL_UACK");
        ru_send_control_block( );
      }
      while(grlc_data->ru.nts)
      { 
        ctrl_blk_active_idx = ru_peek_for_ctrl_blk();
        if ((ctrl_blk_active_idx EQ 0xFF) OR
          realloc_prr_allowed EQ TRUE)     /*No control block , form data block*/
        { 
          while( grlc_data->ru.nts AND 
                (grlc_data->ru.count_cv_0 < 4) )
          {
            bsn = (grlc_data->ru.vs - 1) % 128;
            ru_send_mac_data_req(bsn);
            grlc_data->ru.count_cv_0++;
          }
          while(grlc_data->ru.nts)
          {
            ru_send_ul_dummy_block();
          }
          realloc_prr_allowed = FALSE;
          break;
        }
        else
        {
          
          TRACE_EVENT_P1("reallocation of llc pdu (index)=%d",ctrl_blk_active_idx);
          
          /* if already one PRR in L1 Buffer , replace it with new PRR */
          if (grlc_data->ru.pl_retrans_current.blk[0]
            EQ (BLK_INDEX_TM + OFFSET_CTRL_BLOCK_IDX))
          {      
            grlc_data->ru.write_pos_index--;
            grlc_data->ru.pl_retrans_current.cnt--;
            grlc_data->ru.nts++;  
            TRACE_EVENT("prr in l1 buffer queue,replace with new prr");
          }
          sig_ru_tm_end_of_pdu(ctrl_blk_active_idx);    
          ru_send_control_block();    
          realloc_prr_allowed = TRUE;      
        }
      }
      grlc_send_rem_poll_pos (mac_ul_ready_ind->fn);
      break;
      case RU_NET_REL:       
      /* 
       * send packet control acknowledgement and abort TBF
       */
      grlc_send_rem_poll_pos(mac_ul_ready_ind->fn);
      if(grlc_data->ru.release_tbf)
      {
        SET_STATE(RU,RU_NULL);
        grlc_data->ru.release_tbf = FALSE;                
        TRACE_EVENT_P2("poll sent after packet tbf release(UL): current_fn=%ld rel_fn=%ld",
                                        mac_ul_ready_ind->fn,
                                        grlc_data->ru.poll_fn);
        vsi_t_stop(GRLC_handle,T3180);
        vsi_t_stop(GRLC_handle,T3182);
        grlc_data->tbf_ctrl[grlc_data->ul_index].end_fn = grlc_data->ul_fn;
        grlc_data->tbf_ctrl[grlc_data->ul_index].vs_vr  = grlc_data->ru.vs;
        grlc_data->tbf_ctrl[grlc_data->ul_index].va_vq  = grlc_data->ru.va;
        grlc_data->tbf_ctrl[grlc_data->ul_index].cnt_ts = grlc_data->ru.cnt_ts;
        grlc_trace_tbf_par ( grlc_data->ul_index );
        sig_ru_tm_end_of_tbf();
      }
      if(grlc_check_if_tbf_start_is_elapsed ( grlc_data->ru.poll_fn, mac_ul_ready_ind->fn))
      {
        grlc_data->ru.release_tbf = TRUE;
        TRACE_EVENT_P2("wait for poll confirm after packet tbf release(UL) current_fn=%ld rel_fn=%ld",
                                        mac_ul_ready_ind->fn,
                                        grlc_data->ru.poll_fn);
      }
      break;
    default:
      TRACE_ERROR( "MAC_UL_READY_IND unexpected" );
      break;
  }

  if(grlc_data->uplink_tbf.mac_mode EQ CGRLC_MAC_MODE_FA  AND 
     grlc_data->uplink_tbf.fa_manag.current_alloc.alloc_end_fn EQ mac_ul_ready_ind->fn)
  {
    sig_ru_tm_end_of_fix_alloc();
  }



} /* sig_gff_ru_mac_ready_ind() */



/*
+------------------------------------------------------------------------------
| Function    : sig_tm_ru_reset_poll_array
+------------------------------------------------------------------------------
| Description : Handles the Signal SIG_TM_RU_RESET_POLL_ARRAY.  
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/

GLOBAL void sig_tm_ru_reset_poll_array()
{
  UBYTE i,j;
  /*
   * handle not_transmitted control blocks
  */
  for (i=0;i<NEXT_POLL_ARRAY_SIZE;i++)
  { 
    /*
     * set free list
     */
    grlc_data->next_poll_array[i].next      = i+1;
    grlc_data->next_poll_array[i].fn        = 0xFFFFFFFF;
    grlc_data->next_poll_array[i].cnt       = 0;
    for(j=0; j< POLL_TYPE_ARRAY_SIZE; j++)
    {
      grlc_data->next_poll_array[i].poll_type[j] = CGRLC_POLL_NONE;
    }
  }
  /* 
   * last free entry points to 0xff
   */ 
  grlc_data->next_poll_array[NEXT_POLL_ARRAY_SIZE-1].next = 0xFF;  
  /* 
   * indicates invalid paramters
   */
  grlc_data->poll_start_free = 0;
  grlc_data->poll_start_tbf  = 0xFF;  
}



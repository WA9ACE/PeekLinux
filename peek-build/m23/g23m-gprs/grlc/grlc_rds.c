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
|             RD of entity GRLC.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRLC_RDS_C
#define GRLC_RDS_C
#endif

#define ENTITY_GRLC

/*==== INCLUDES =============================================================*/
#include <string.h>

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
#include "grlc_rdf.h"
#include "grlc_tms.h"
#include "grlc_f.h"
#include "grlc_meass.h"

/*==== CONST ================================================================*/
const T_TIME T3192_Values[8] = {500, 1000, 1500, 1, 80, 120, 160, 200};

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : sig_tm_rd_assign
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TM_RD_ASSIGN
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tm_rd_assign ( void ) 
{ 
  TRACE_ISIG( "sig_tm_rd_assign" );
  
  switch( GET_STATE( RD ) )
  {
    case RD_WAIT_FOR_STARTING_TIME_ACK:
    case RD_WAIT_FOR_STARTING_TIME_UACK:
      TRACE_EVENT_P3("DL reassignment during starting time :%ld: dl_tfi=%d, st_dl_tfi=%d"
                                                                ,grlc_data->dl_tbf_start_time
                                                                ,grlc_data->dl_tfi
                                                                ,grlc_data->start_fn_dl_tfi);


      /*lint -fallthrough*/

    case RD_NULL:            
      rd_tbf_init();
      if(grlc_data->rd.rlc_mode EQ CGRLC_RLC_MODE_ACK)
      {
        SET_STATE(RD,RD_WAIT_FOR_STARTING_TIME_ACK);
      }
      else
      {
        SET_STATE(RD,RD_WAIT_FOR_STARTING_TIME_UACK);
      }    
      grlc_data->dl_index = 1;
      grlc_data->tbf_ctrl[grlc_data->dl_index].tbf_type    = TBF_TYPE_DL;
      grlc_data->tbf_ctrl[grlc_data->dl_index].tfi         = grlc_data->dl_tfi;
      grlc_data->tbf_ctrl[grlc_data->dl_index].pdu_cnt     = 0;
      grlc_data->tbf_ctrl[grlc_data->dl_index].rlc_oct_cnt = 0;
      grlc_data->tbf_ctrl[grlc_data->dl_index].cnt_ts      = 0;
      grlc_data->tbf_ctrl[grlc_data->dl_index].ack_cnt     = 0;
      grlc_data->tbf_ctrl[grlc_data->dl_index].fbi         = 0;
      grlc_data->tbf_ctrl[grlc_data->dl_index].ret_bsn     = 0;

      grlc_data->rd.ack_ctrl.cnt_meas_rpt                 = ACK_CNT_MEAS_RPT_FIRST;
      grlc_data->rd.ack_ctrl.cnt_other                    = ACK_CNT_NORMAL;
      break;
    case RD_ACK:
    case RD_UACK:
      vsi_t_stop(GRLC_handle,T3190); 
      vsi_t_start(GRLC_handle,T3190,T3190_VALUE);
      grlc_data->rd.next_tbf_params   = grlc_data->downlink_tbf;
      grlc_data->rd.v_next_tbf_params = TRUE;
      TRACE_EVENT_P3("dl tbf reassignment with tfi %d, nts=%d tbf_st_time=%ld"
                                              ,grlc_data->dl_tfi
                                              ,grlc_data->downlink_tbf.nts
                                              ,grlc_data->dl_tbf_start_time);
      break;
    default:
      TRACE_ERROR( "SIG_TM_RD_ASSIGN unexpected" );
      {
        TRACE_EVENT_P2("DL ASS: state=%d  t3192=%d "
                                                                        ,grlc_data->rd.state
                                                                        ,grlc_data->downlink_tbf.t3192_val );
      }
      break;
  }
} /* sig_tm_rd_assign() */



/*
+------------------------------------------------------------------------------
| Function    : sig_tm_rd_abrel
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TM_RD_ABREL
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tm_rd_abrel ( ULONG fn, BOOL poll) 
{ 
  TRACE_ISIG( "sig_tm_rd_abrel" );
  
  switch( GET_STATE( RD ) )
  {
    case RD_ACK:
    case RD_REL_ACK:
    case RD_UACK:
    case RD_REL_UACK:
    case RD_NET_REL:    
    case RD_WAIT_FOR_STARTING_TIME_ACK:
    case RD_WAIT_FOR_STARTING_TIME_UACK:
      vsi_t_stop(GRLC_handle,T3190);
      vsi_t_stop(GRLC_handle,T3192);
	    
      rd_free_desc_list_partions();
      rd_free_database_partions();

      grlc_data->tbf_ctrl[grlc_data->dl_index].vs_vr  = grlc_data->rd.vr;
      grlc_data->tbf_ctrl[grlc_data->dl_index].va_vq  = grlc_data->rd.vq;
      grlc_trace_tbf_par ( grlc_data->dl_index );
       
      if(!poll)
      {
        /*
         * abnormal release: abort TBF
         */
        SET_STATE(RD,RD_NULL);          
      }
      else
      {
        SET_STATE(RD,RD_NET_REL);
        grlc_data->rd.fn_p_tbf_rel = fn;
        TRACE_EVENT_P2("p. tbf rel c_fn=%ld poll_fn=%d",fn,grlc_data->rd.fn_p_tbf_rel);
      }
      break;
    default:
      TRACE_ERROR( "SIG_TM_RD_ABREL unexpected" );
      break;
  }
} /* sig_tm_rd_abrel() */

/*
+------------------------------------------------------------------------------
| Function    : sig_tm_rd_ul_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TM_RD_UL_REQ
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tm_rd_ul_req ( void) 
{ 
  TRACE_ISIG( "sig_tm_rd_ul_req" );
  
  switch( GET_STATE( RD ) )
  {
    case RD_ACK:
    case RD_REL_ACK:
    case RD_UACK:
    case RD_REL_UACK:
    case RD_WAIT_FOR_STARTING_TIME_ACK:
    case RD_WAIT_FOR_STARTING_TIME_UACK:
      grlc_data->rd.channel_req = TRUE;      
      break;
    default:
      TRACE_ERROR( "SIG_TM_RD_UL_REQ unexpected" );
      break;
  }
} /* sig_tm_rd_ul_req() */


/*
+------------------------------------------------------------------------------
| Function    : sig_tm_rd_ul_req_stop
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TM_RD_UL_REQ_STOP: No packet
|               channel description shall be sent in packet downlink ack/nack,
|               because a valid packet uplink assignment is received
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tm_rd_ul_req_stop ( void) 
{ 
  TRACE_ISIG( "sig_tm_rd_ul_req_stop" );
  /*
   * do it in every state
   */  
  grlc_data->rd.channel_req = FALSE;

} /* sig_tm_rd_ul_req_stop() */

/*
+------------------------------------------------------------------------------
| Function    : sig_tm_rd_nor_rel
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TM_RD_NOR_REL
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tm_rd_nor_rel ( void) 
{ 
  TRACE_ISIG( "sig_tm_rd_nor_rel" );
  
  switch( GET_STATE( RD ) )
  {
    case RD_NULL:
    case RD_ACK:
    case RD_REL_ACK:
    case RD_UACK:
    case RD_REL_UACK:
    case RD_NET_REL:  
    case RD_WAIT_FOR_STARTING_TIME_ACK:
    case RD_WAIT_FOR_STARTING_TIME_UACK:
      SET_STATE(RD,RD_NULL);
      vsi_t_stop(GRLC_handle,T3190);
      vsi_t_stop(GRLC_handle,T3192);	  
      grlc_data->tbf_ctrl[grlc_data->dl_index].vs_vr  = grlc_data->rd.vr;
      grlc_data->tbf_ctrl[grlc_data->dl_index].va_vq  = grlc_data->rd.vq; 
      grlc_trace_tbf_par ( grlc_data->dl_index );
      break;
    default:
      TRACE_ERROR( "SIG_TM_RD_NOR_REL unexpected" );
      break;
  }
} /* sig_tm_rd_nor_rel() */





/*
+------------------------------------------------------------------------------
| Function    : sig_gff_rd_mac_ready_ind
+------------------------------------------------------------------------------
| Description : Handles the Signal sig_gff_rd_mac_ready_ind
|
| Parameters  : *mac_dl_ready_ind - Ptr to primitive MAC_READY_IND
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gff_rd_mac_ready_ind ( T_MAC_READY_IND * mac_dl_ready_ind)
{
  UBYTE   *ptr_block=NULL;
  ULONG   delta_fn;
   
  TRACE_ISIG( "sig_gff_rd_mac_ready_ind" );


  grlc_data->tbf_ctrl[grlc_data->dl_index].end_fn = mac_dl_ready_ind->fn;  

  grlc_handle_poll_pos (mac_dl_ready_ind->fn);
  delta_fn = rd_calc_delta_fn(mac_dl_ready_ind->fn);

  switch( GET_STATE( RD ) )
  {
    case RD_WAIT_FOR_STARTING_TIME_ACK:
    case RD_WAIT_FOR_STARTING_TIME_UACK:       
      if(!grlc_check_if_tbf_start_is_elapsed ( grlc_data->dl_tbf_start_time, mac_dl_ready_ind->fn))
      {
        TRACE_EVENT_P2("WAIT FOR DL STARTING TIME, UL IS RUNNING st=%ld fn=%ld",grlc_data->dl_tbf_start_time
                                                                              ,mac_dl_ready_ind->fn);
        grlc_send_rem_poll_pos (mac_dl_ready_ind->fn);
        return;
      }
      if(grlc_data->rd.rlc_mode EQ CGRLC_RLC_MODE_ACK)
      {
        SET_STATE(RD,RD_ACK);
      }
      else
      {
        SET_STATE(RD,RD_UACK);
      } 
      grlc_data->tbf_ctrl[grlc_data->dl_index].start_fn    = mac_dl_ready_ind->fn;
      
      TRACE_EVENT_P6("DL first call at fn = %ld (%ld) with tfi=%d, dl_mask=%x tbf_st_time=%ld ta=%d",
                                        mac_dl_ready_ind->fn,
                                        mac_dl_ready_ind->fn%42432,
                                        grlc_data->dl_tfi,
                                        grlc_data->dl_tn_mask,
                                        grlc_data->dl_tbf_start_time,
                                        grlc_data->ta_value);
      rd_cgrlc_st_time_ind();
      vsi_t_start(GRLC_handle,T3190,T3190_VALUE);

      /*lint -fallthrough*/

    case RD_ACK:
    case RD_REL_ACK:
    case RD_UACK:
    case RD_REL_UACK:
      if(grlc_data->rd.v_next_tbf_params AND
          grlc_check_if_tbf_start_is_elapsed ( grlc_data->dl_tbf_start_time, mac_dl_ready_ind->fn))
      {
        grlc_data->rd.v_next_tbf_params = FALSE;
        rd_cgrlc_st_time_ind();
        vsi_t_start(GRLC_handle,T3190,T3190_VALUE);
      }
      if(grlc_data->rd.ch_req_in_ack_prog AND mac_dl_ready_ind->last_poll_resp)
      {
        grlc_data->rd.channel_req = TRUE;
      }
      else if(grlc_data->rd.ch_req_in_ack_prog)
      {
        sig_rd_tm_ul_req_cnf();
        grlc_data->rd.ch_req_in_ack_prog = FALSE;
      }

      
      if(grlc_data->rd.release_tbf)  
      {
        /*
         * Packet downlink ack/nack with final ack bit set to 1 was programmed
         */
        grlc_data->rd.release_tbf = FALSE;
        if(mac_dl_ready_ind->last_poll_resp EQ 0)
        {
          /*
           * Transmission from L1 confirmed
           */
          grlc_data->rd.cnt_sent_f_ack++;
        }

      }

       
      if( grlc_data->rd.f_ack_ind AND
          (grlc_data->rd.fn_p_tbf_rel NEQ 0xFFFFFFFF)  AND
          grlc_check_if_tbf_start_is_elapsed ( grlc_data->rd.fn_p_tbf_rel, mac_dl_ready_ind->fn))
      {
        /*
         * no addtional final downlink ack with fbi requested by the network
         * T3192 can be started
         */

        vsi_t_stop(GRLC_handle,T3190);

        if(grlc_data->rd.cnt_sent_f_ack)
        { 
          /*
           * it is only a retransmission of the final dl ack. 
           * Therefore t3192 is restarted, no offset is needed
           */
          vsi_t_start(GRLC_handle,T3192, T3192_Values[grlc_data->downlink_tbf.t3192_val]); 
          TRACE_EVENT_P3("T3192 started:  rel_fn=%ld fn=%ld T3192=%d",grlc_data->rd.fn_p_tbf_rel,mac_dl_ready_ind->fn,grlc_data->downlink_tbf.t3192_val);
        }
        else
        {
          /*
           * the first final dl ack is not retransmitted, therefore retransmission is required.
           * start t3190 and wait for new poll position
           */ 
          vsi_t_start(GRLC_handle,T3190,T3190_VALUE);
          TRACE_EVENT_P1("NO DL REL: ret needed T3190 is running %d",grlc_data->rd.cnt_sent_f_ack);
        }  
        grlc_data->rd.fn_p_tbf_rel = 0xFFFFFFFF;
      }


      if( (delta_fn EQ 4) 
           OR 
          (delta_fn EQ 5))                   
      {
        UBYTE  i=0;
        UBYTE  index;
        while(grlc_data->next_poll_array[grlc_data->poll_start_tbf].cnt 
              AND 
              (i<8))
        {
          if(grlc_data->next_poll_array[grlc_data->poll_start_tbf].poll_type[i] EQ CGRLC_POLL_DATA)
          {
           /* 
            * poll positon in next frame present, 
            * counter for for poll postions in one frame 
            */
            if( grlc_data->rd.channel_req                                OR /* uplink request */
                grlc_data->rd.next_poll_block EQ NEXT_POLL_BLOCK_DL_DATA OR /* last poll block was not ack nack */
                grlc_data->rd.f_ack_ind                                  OR /* final ack indicator */
                !tm_is_ctrl_blk_rdy
                   ( grlc_data->rd.ack_ctrl.cnt_meas_rpt, 
                     grlc_data->rd.ack_ctrl.cnt_other     ) )               /* no ctrl message */
            { 
              if ( !((grlc_data->rd.rlc_mode EQ CGRLC_RLC_MODE_UACK) AND grlc_data->rd.f_ack_ind) )
              {
                /* 
                 * sent downlink ack/nack 
                 */
                 ptr_block = rd_set_acknack();
                 sig_rd_meas_qual_rpt_sent( );
                 grlc_data->tbf_ctrl[grlc_data->dl_index].ack_cnt++;

                 if( grlc_data->rd.ack_ctrl.cnt_meas_rpt > 0 )
                   grlc_data->rd.ack_ctrl.cnt_meas_rpt--;
                 
                 if( grlc_data->rd.ack_ctrl.cnt_other > 0 )
                   grlc_data->rd.ack_ctrl.cnt_other--;

                 grlc_send_normal_burst(ptr_block, NULL, i);
              }
              else
              {
                /*
                 * unacknowledged mode, send packet control acknowledgment
                 */
                if(grlc_data->burst_type EQ CGRLC_BURST_TYPE_NB)
                {
                   ptr_block = grlc_set_packet_ctrl_ack();
                   grlc_send_normal_burst(ptr_block, NULL, i);
                }
                else
                {
                   /* send access burst. */
                   grlc_send_access_burst(i);
                }
              }
              grlc_data->rd.next_poll_block = NEXT_POLL_BLOCK_CTRL;
              if(grlc_data->rd.f_ack_ind)
              {
                grlc_data->rd.release_tbf = TRUE;
              }
            }
            else
            { 
              /* 
               * sent control block 
               */
              ptr_block = tm_get_ctrl_blk( &index, TRUE );
              grlc_send_normal_burst(NULL, ptr_block, i);
              grlc_data->rd.next_poll_block = NEXT_POLL_BLOCK_DL_DATA;
              
              if( grlc_data->rd.ack_ctrl.cnt_meas_rpt EQ 0 )
                grlc_data->rd.ack_ctrl.cnt_meas_rpt = ACK_CNT_NORMAL;
                 
              if( grlc_data->rd.ack_ctrl.cnt_other EQ 0 )
                grlc_data->rd.ack_ctrl.cnt_other = ACK_CNT_NORMAL;
            }
          }
          i++;
        }
      }
      grlc_send_rem_poll_pos(mac_dl_ready_ind->fn);
      break;
    case RD_NET_REL:       
      /* 
       * send packet control acknowledgement and abort TBF
       */
      grlc_send_rem_poll_pos(mac_dl_ready_ind->fn);
      if(grlc_data->rd.release_tbf)
      {
        SET_STATE(RD,RD_NULL);
        grlc_data->rd.release_tbf = FALSE;                
        TRACE_EVENT_P2("poll sent after packet tbf release(DL): current_fn=%ld rel_fn=%ld",
                                        mac_dl_ready_ind->fn,
                                        grlc_data->rd.fn_p_tbf_rel);
        sig_rd_tm_end_of_tbf(FALSE);
      }
      if(grlc_check_if_tbf_start_is_elapsed ( grlc_data->rd.fn_p_tbf_rel, mac_dl_ready_ind->fn))
      {
        grlc_data->rd.release_tbf = TRUE;
        TRACE_EVENT_P2("wait for poll confirm after packet tbf release(DL) current_fn=%ld rel_fn=%ld",
                                        mac_dl_ready_ind->fn,
                                        grlc_data->rd.fn_p_tbf_rel);
      }
	  break;
    default:
      TRACE_ERROR( "SIG_GFF_RD_MAC_READY_IND unexpected" );
      break;
  }
} /* sig_gff_rd_mac_ready_ind() */




/*
+------------------------------------------------------------------------------
| Function    : sig_gff_rd_data
+------------------------------------------------------------------------------
| Description : Handles the SIGNAL sig_gff_rd_data
|
| Parameters  : fn - current frame number
|               tn - current timeslot number
|               block_status - current block_status
|               sp           - sp bit of mac header.
|               bsn          - bsn number
|               fbi          - fbi bit
|               e_bit        - e bit
|               ptr_dl_block - pointer to the dl data block
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gff_rd_data (ULONG fn, UBYTE tn, USHORT block_status,UBYTE rrbp,UBYTE sp,UBYTE bsn,UBYTE fbi,UBYTE e_bit, UBYTE * ptr_dl_block) 
{
  UBYTE bsn_l;
  UBYTE diff;
  BOOL  InRange;
  BOOL  li_correct;
  T_CODING_SCHEME old_cs_type;


  TRACE_FUNCTION( "sig_gff_rd_data" );

  
  grlc_data->tbf_ctrl[grlc_data->dl_index].cnt_ts++;


  /*
   * TESTMODE B
   */
  if (grlc_data->testmode.mode EQ CGRLC_LOOP)
  {
   /* 
    * If the downlink TBF is established on more than one timeslot, the MS shall transmit in 
    * the second uplink timeslot (if present) RLC/MAC blocks received on the second downlink 
    * timeslot, and shall transmit in the third uplink timeslot (if present) RLC/MAC blocks 
    * received in the third downlink timeslot and so on.
    */
    if (grlc_data->downlink_tbf.nts > 1)
    {
      if (tn - grlc_data->testmode.dl_ts_offset)
      {
        grlc_data->testmode.rec_data[1].block_status = block_status;
        grlc_data->testmode.rec_data[1].e_bit        = e_bit;
        memcpy(grlc_data->testmode.rec_data[1].payload,ptr_dl_block,50);
      }
      else
      {
        grlc_data->testmode.rec_data[0].block_status = block_status;
        grlc_data->testmode.rec_data[0].e_bit        = e_bit;
        memcpy(grlc_data->testmode.rec_data[0].payload,ptr_dl_block,50);
      }
    }
    else
    {
      grlc_data->testmode.rec_data[0].block_status = block_status;
      grlc_data->testmode.rec_data[0].e_bit        = e_bit;
      memcpy(grlc_data->testmode.rec_data[0].payload,ptr_dl_block,50);
    }

    grlc_data->rd.f_ack_ind = rd_check_fbi(fbi,sp,fn,rrbp);

    vsi_t_stop(GRLC_handle,T3190);
    vsi_t_start(GRLC_handle,T3190,T3190_VALUE);

    TRACE_EVENT_P6("DL DATA in testmode B at fn=%ld: bsn =%d, bs=%d,e_bit=%d, fbi=%d sp=%d "
                            ,fn
                            ,bsn
                            ,block_status
                            ,grlc_data->testmode.rec_data[0].e_bit
                            ,fbi
                            ,sp);
    return;
  }




/******************************************************************************************/
#if defined (_SIMULATION_)
  TRACE_EVENT_P1("DL DATA bsn= %d",bsn);
#endif /* defined (_SIMULATION_) */


  if(grlc_data->rd.vq NEQ grlc_data->rd.vr)
  {
    TRACE_EVENT_P5("wait for neg acked blocks :bsn=%d  vr=%d vq=%d  fbi=%d fn=%ld",
                                                            bsn,
                                                            grlc_data->rd.vr,
                                                            grlc_data->rd.vq,
                                                            fbi,
                                                            fn);
  }
/******************************************************************************************/
  old_cs_type = grlc_data->rd.cs_type;
  switch( GET_STATE( RD ) )
  {    
    case RD_WAIT_FOR_STARTING_TIME_ACK:
      SET_STATE(RD,RD_ACK);    

      /*lint -fallthrough*/

    case RD_ACK     :
      vsi_t_stop(GRLC_handle,T3190); 
      vsi_t_start(GRLC_handle,T3190,T3190_VALUE);
      bsn_l   = bsn;
      InRange = rd_check_window_size(bsn_l);


      if( !InRange  OR (grlc_data->rd.vn[bsn_l & WIN_MOD] EQ VN_RECEIVED))
      {
        grlc_data->tbf_ctrl[grlc_data->dl_index].ret_bsn++;
        
        if( fbi                      AND 
            !grlc_data->rd.f_ack_ind AND
           ((bsn_l+1) & 0x7F) EQ grlc_data->rd.vr)
        { /* WORKAROUND FOR NORTEL, fbi bit is modified , only possible for last bsn*/ 
          
          grlc_data->rd.f_ack_ind=rd_check_fbi(fbi,sp,fn,rrbp);

          TRACE_EVENT_P8("fbi modification in ret bsn=%d,vr=%d,vq=%d,sp=%d,fbi=%d,e=%d ptr[0]=%x,ptr[1]=%x",
                                                                    bsn_l,
                                                                    grlc_data->rd.vr,
                                                                    grlc_data->rd.vq,
                                                                    sp,
                                                                    fbi,
                                                                    e_bit,
                                                                    ptr_dl_block[0],
                                                                    ptr_dl_block[1]);

        }
        
        return;
      }
      grlc_data->rd.rlc_data_len = rd_calc_rlc_data_len(block_status);
      li_correct                = rd_read_li_m_of_block(ptr_dl_block,
                                                        e_bit);
      if((old_cs_type NEQ grlc_data->rd.cs_type) AND
         (old_cs_type NEQ CS_ZERO))
      {
        TRACE_EVENT_P3("DL CS TYPE CHANGED from %d to %d, bsn=%d",old_cs_type,grlc_data->rd.cs_type,bsn_l);
      }
      
      if(!li_correct)
      {
        grlc_data->rd.vn[bsn_l & WIN_MOD] = VN_INVALID;
        TRACE_ERROR( "LI field is longer than RLC data block" );
        return;
      }
     
      /* 
       * compute receive parameter
       */
      rd_comp_rec_par(bsn_l); 

      rd_save_block( bsn_l, &(ptr_dl_block[grlc_data->rd.li_cnt]),
                              fbi );

#if defined (_SIMULATION_)
  TRACE_EVENT_P1("first_ptr= %ld",grlc_data->rd.data_array[bsn_l & WIN_MOD].first);
#endif /* defined (_SIMULATION_) */

      diff = (grlc_data->rd.vq - bsn_l) & 0x7F;
      while( ( diff <= WIN_SIZE)          AND
             (grlc_data->rd.vq NEQ bsn_l) AND 
             (grlc_data->rd.vn[bsn_l & WIN_MOD] EQ VN_RECEIVED )  )
      {
        rd_send_grlc_data_ind(bsn_l);		
        bsn_l = (bsn_l+1)  & 0x7F;
        diff  = (grlc_data->rd.vq - bsn_l) & 0x7F;
      }       
      
      grlc_data->rd.f_ack_ind = rd_check_fbi(fbi,sp,fn,rrbp);
      
      break;
    case RD_REL_ACK :    
      /*
       * all blocks are received, after sending p. dl ack nack timer t3192 will be restarted
       */
      grlc_data->tbf_ctrl[grlc_data->dl_index].ret_bsn++;

      if(grlc_data->rd.f_ack_ind AND sp)
      { /* 
         * will be restarted at sending the dl ack nack: secure, in case of gaps.
         * The value could be zero, therfore a offset is needed
         */ 
        vsi_t_stop(GRLC_handle,T3192); 
        grlc_data->rd.fn_p_tbf_rel = grlc_decode_tbf_start_rel(fn,(USHORT)(rrbp+3));
        TRACE_EVENT_P3("FINAL DL ACK RETR REQIURED t3192=%d rel_fn=%ld c_fn=%ld ",grlc_data->downlink_tbf.t3192_val,grlc_data->rd.fn_p_tbf_rel,fn);
      }
 /*     TRACE_EVENT_P7("BLOCK DISCARDED WAIT FOR TBF RELEASE f_ack=%d bsn=%d sp=%d rrbp=%d vr=%d vq=%d t3192=%d"
                                                               ,grlc_data->rd.f_ack_ind
                                                               ,bsn
                                                               ,sp
                                                               ,rrbp
                                                               ,grlc_data->rd.vr
                                                               ,grlc_data->rd.vq
                                                               ,grlc_data->downlink_tbf.t3192_val);
  */
      break;    
    case RD_WAIT_FOR_STARTING_TIME_UACK:
      SET_STATE(RD,RD_UACK);

      /*lint -fallthrough*/

    case RD_UACK:
      /*
       * After receiving each block the timer T3190 should be restarted. 
       */
      vsi_t_stop(GRLC_handle,T3190); 
      vsi_t_start(GRLC_handle,T3190,T3190_VALUE);

      if(!rd_check_window_size(bsn))
      {
        TRACE_EVENT_P3("UACK bsn outside window: bsn=%d vr=%d vq=%d",bsn,grlc_data->rd.vr,grlc_data->rd.vq);
        return;
      }
      
      grlc_data->rd.rlc_data_len  = rd_calc_rlc_data_len(block_status);
      li_correct                 = rd_read_li_m_of_block(ptr_dl_block, e_bit);
      if(!li_correct)
      {
        TRACE_ERROR( "LI field is longer than RLC data block" );
        return;
      }
     
      /* 
       * compute receive parameter 
       */
      rd_comp_rec_par(bsn); 
      rd_save_block( bsn, &(ptr_dl_block[grlc_data->rd.li_cnt]), fbi );
      if(!grlc_data->rd.inSequence)
        rd_fill_blocks(bsn);

      rd_send_grlc_data_ind(bsn);
      
      grlc_data->rd.f_ack_ind = rd_check_fbi(fbi,sp,fn,rrbp);

      break;
    case RD_REL_UACK:

      if(fbi AND sp)
      {
        vsi_t_stop(GRLC_handle,T3192); 
        grlc_data->rd.fn_p_tbf_rel = grlc_decode_tbf_start_rel(fn,(USHORT)(rrbp+3));
        TRACE_EVENT_P4("data in REL_UACK restart t3192:  bsn=%d   t3192=%d fn=%ld rel_fn=%ld"
                                                               ,bsn
                                                               ,grlc_data->downlink_tbf.t3192_val
                                                               ,fn
                                                               ,grlc_data->rd.fn_p_tbf_rel);

      }
      break;
    default:
      TRACE_ERROR( "SIG_GFF_RD_DATA unexpected" );
      break;
  }

} /* sig_gff_rd_data() */

/*
+------------------------------------------------------------------------------
| Function    : sig_ru_rd_get_downlink_release_state
+------------------------------------------------------------------------------
| Description : This function returns true if RD is in release state( if
|               fbi=1 is received in dowonlink data block. 
|
| Parameters  : release_state 
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ru_rd_get_downlink_release_state( BOOL *release_started)
{
  *release_started = FALSE ;

  if( (GET_STATE( RD ) EQ RD_REL_ACK ) OR
      (GET_STATE( RD ) EQ RD_REL_UACK ) )
  {
     *release_started = TRUE;
  }
  else if(GET_STATE( RD ) EQ RD_NULL )
  {
     TRACE_ERROR("ERROR:sig_ru_rd_get_downlink_release_state called in RD NULL state");
     *release_started = TRUE;
  }
}

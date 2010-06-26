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
|             GFF of entity GRLC.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRLC_GFFP_C
#define GRLC_GFFP_C
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
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "message.h"
#include "grlc.h"        /* to get the global entity definitions */
#include "grlc_gfff.h" 
#include "grlc_f.h" 
#include <string.h>
#include "grlc_rus.h"
#include "grlc_rds.h"
#include "grlc_func.h"
#include "grlc_tpcs.h"
#include "grlc_meass.h"
#include "cl_rlcmac.h"
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
#include "grlc_tms.h"
#endif

/*==== CONST ================================================================*/

#define BITS_PER_RADIO_BLOCK 456

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/
/*
+------------------------------------------------------------------------------
| Function    : gff_mac_pwr_ctrl_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive MAC_PWR_CTRL_IND
|
| Parameters  : *mac_pwr_ctrl_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void gff_mac_pwr_ctrl_ind ( T_MAC_PWR_CTRL_IND * mac_pwr_ctrl_ind )
{ 
  TRACE_FUNCTION( "gff_mac_pwr_ctrl_ind" );
  
#if defined _TARGET_ && !defined (NTRACE)

  if( grlc_data->tpc.n_tpc_trace & M_TPC_TRACE_FUNCTION )
  {
    T_L1TEST_CALL_MPHP_POWER_CONTROL l1test_call;

    UBYTE i;
    
    l1test_call.assignment_id = ( U8 )mac_pwr_ctrl_ind->assignment_id;
    l1test_call.crc_error     =       mac_pwr_ctrl_ind->crc_error;
    l1test_call.bcch_level    = ( S8 )mac_pwr_ctrl_ind->bcch_level;

    for( i = 0; i < L1TEST_BURST_PER_BLOCK; i++ )
    {
      l1test_call.burst_level[i] = ( S8 )mac_pwr_ctrl_ind->burst_level[i];
      l1test_call.radio_freq [i] =       mac_pwr_ctrl_ind->radio_freq[i];
    }

    TRACE_MEMORY_PRIM ( hCommL1, hCommGRLC, L1TEST_CALL_MPHP_POWER_CONTROL,
                        &l1test_call, sizeof( T_L1TEST_CALL_MPHP_POWER_CONTROL ) );
  }

#endif /* #if defined _TARGET_ && !defined (NTRACE) */

  gff_clip_rxlev( &mac_pwr_ctrl_ind->bcch_level,
                  &mac_pwr_ctrl_ind->bcch_level, 1 );
  
  gff_clip_rxlev( mac_pwr_ctrl_ind->burst_level,
                  mac_pwr_ctrl_ind->burst_level, 
                  MAC_BURST_PER_BLOCK );

  sig_gff_tpc_pwr_ctrl_ind( mac_pwr_ctrl_ind );

#if defined _TARGET_ && !defined (NTRACE)

  if( grlc_data->tpc.n_tpc_trace & M_TPC_TRACE_FUNCTION )
  {
    T_L1TEST_RETURN_MPHP_POWER_CONTROL l1test_return;
    
    tpc_get_pch( &l1test_return.pch[0] );
    
    TRACE_MEMORY_PRIM ( hCommGRLC, hCommL1, L1TEST_RETURN_MPHP_POWER_CONTROL,
                        &l1test_return, sizeof( T_L1TEST_RETURN_MPHP_POWER_CONTROL ) );
  }

#endif /* #if defined _TARGET_ && !defined (NTRACE) */

} /* gff_mac_pwr_ctrl_ind() */

/*
+------------------------------------------------------------------------------
| Function    : gff_mac_data_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive MAC_DATA_IND
|
| Parameters  : *mac_data_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void gff_mac_data_ind ( T_MAC_DATA_IND * mac_data_ind )
{ 
  UBYTE   i;
  USHORT  rx_no;

  TRACE_FUNCTION( "mac_data_ind" );
  
  rx_no  = grlc_data->func.dl_blocks.data_ptr.rx_no;

#ifdef _SIMULATION_
  {
    /*
     * copy received primitive to grlc_data->func.mac_data_ind
     */

    TRACE_EVENT_P5("mac_data_ind fn=%ld tn= %d -> mask=%x UL_MASK=%x DL_MASK=%x"
                                                   ,mac_data_ind->fn
                                                   ,mac_data_ind->dl_data.tn
                                                   ,0x80>>mac_data_ind->dl_data.tn
                                                   ,grlc_data->ul_tn_mask
                                                   ,grlc_data->dl_tn_mask);

    grlc_data->dl_fn = mac_data_ind->fn;
    grlc_data->ul_fn = mac_data_ind->fn;
    memcpy (
            &(grlc_data->func.mac_data_ind),
            mac_data_ind,
            sizeof(T_MAC_DATA_IND));
    PFREE(mac_data_ind);
    mac_data_ind = &(grlc_data->func.mac_data_ind);
    rx_no = 1;
  }
#endif

  switch( GET_STATE( GFF ) ) 
  {
    case GFF_ACTIVE:  
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
    case GFF_TWO_PHASE: 		
#endif
      {
        T_BER_IND ber_ind;

        ber_ind.num_dl_blck = 0;

        for(i=0; i< rx_no; i++)
        {
        

#ifdef _TARGET_
          {
            /*
             * copy next primitive to grlc_data->func.mac_data_ind
             */
            grlc_data->func.mac_data_ind.fn      = grlc_data->func.dl_blocks.fn;
            grlc_data->func.mac_data_ind.rx_no   = grlc_data->func.dl_blocks.data_ptr.rx_no;
            grlc_data->func.mac_data_ind.dl_data = grlc_data->func.dl_blocks.data_ptr.dl_data[i];  
            mac_data_ind = &(grlc_data->func.mac_data_ind);
          }
          
          TRACE_MEMORY_PRIM ( hCommL1, hCommGRLC, MAC_DATA_IND,
                              mac_data_ind, sizeof( T_MAC_DATA_IND ) );
          
#endif /* #ifdef _TARGET_ */

          /* 
           * In Test mode B,if there is CRC error on the payload data the MS will,
           * where required by the USF, transmit the decoded payload data. The block
           * transmitted will be a valid uplink block format.
           */
	      
          if( ( (mac_data_ind->dl_data.block_status & 0x0100) EQ 0x0000)  OR ( (grlc_data->testmode.mode EQ CGRLC_LOOP) ) )
          {
            /*
             * Only successfully decoded blocks shall be considered
             */

            if( gff_analyse_dl_data( mac_data_ind->fn, &( mac_data_ind->dl_data ) ) )
            {
              /*
               * Only blocks intended for that MS shall be considered
               */

              if( ber_ind.num_dl_blck < MAC_MAX_DL_DATA_BLCKS )
              {
                ber_ind.be_info[ber_ind.num_dl_blck].total = BITS_PER_RADIO_BLOCK;

                if( ( mac_data_ind->dl_data.block_status & 0x000F ) EQ 0x0006 )
                {
                  /* 
                   * If CS4 is used, the MS is allowed to report RXQUAL = 7,
                   * so just set the BER to 100%
                   */
                  ber_ind.be_info[ber_ind.num_dl_blck].fault =
                                                          BITS_PER_RADIO_BLOCK;
                }
                else
                {
                  ber_ind.be_info[ber_ind.num_dl_blck].fault =
                                    grlc_data->func.mac_data_ind.dl_data.d_nerr;
                }

                ber_ind.num_dl_blck++;
              }
              else
              {
                TRACE_ERROR( "be_info overflow in gff_mac_data_ind" );
              }
            }
          }
          else
          {
            TRACE_ERROR( "MAC_DATA_IND with bad CRC" );

            TRACE_BINDUMP
              ( hCommGRLC, TC_USER5,
                cl_rlcmac_get_msg_name( D_MSG_TYPE_CRC_ERROR_c, RLC_MAC_ROUTE_DL ),
                mac_data_ind->dl_data.dl_block, MAX_L2_FRAME_SIZE ); /*lint !e569*/

            /* grlc_data->nr_of_crc_errors++; */
          }
        }

        if( ber_ind.num_dl_blck NEQ 0 )
        {
           meas_sq_update( &ber_ind );
        }
      }
      break;
    default:
      /*TRACE_ERROR( "MAC_DATA_IND unexpected" );*/
      break;
  }

/*call control of asyn part TARGET DEBUGGING*/
#if defined (_TARGET_)
  grlc_data->dl_cnt_asyn = grlc_data->dl_cnt_syn;
#endif /* defined (_TARGET_) */

} /* gff_mac_data_ind() */



/*
+------------------------------------------------------------------------------
| Function    : gff_mac_ready_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive MAC_READY_IND
|
| Parameters  : *mac_ready_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void gff_mac_ready_ind ( T_MAC_READY_IND * mac_ready_ind )
{ 
  TRACE_FUNCTION( "mac_ready_ind" );

  /*
   * calculate the frame number from downlink call
   */
#ifdef _TARGET_
  {
    ULONG current_dl_fn;
    ULONG last_dl_fn;
    ULONG last_ul_fn;
    ULONG current_ul_fn;
    BOOL  trace_dl_gap = TRUE;

    current_dl_fn = grlc_data->func.mac_ready_ind.fn;
    last_dl_fn    = grlc_data->last_dl_fn;
    last_ul_fn    = grlc_data->last_ul_fn;

    grlc_data->func.mac_ready_ind.fn = grlc_data->ul_fn ;
    current_ul_fn                   = grlc_data->ul_fn;

    if(grlc_data->func.mac_ready_ind.last_poll_resp)
    {
      TRACE_EVENT_P5("POLL NOT SENT BY LAYER 1 fn=%ld last_poll_resp=0x%x np_fn=%ld,bs=%d,ind=%d"
                                                ,grlc_data->func.mac_ready_ind.fn
                                                ,grlc_data->func.mac_ready_ind.last_poll_resp
																								,grlc_data->next_poll_fn
																								,grlc_data->ul_poll_resp[0].block_status
																								,grlc_data->ul_poll_pos_index);
    }

    /* handle gaps*/
    if ((grlc_data->next_poll_fn EQ 2) OR (grlc_data->next_poll_fn EQ 1))
    { /* last poll is sent*/
      grlc_data->func.mac_ready_ind.last_poll_resp = 0; /* no ul gap for poll */ 
    }
    else if( grlc_data->ul_call_errors AND 
             (grlc_data->ul_fn_errors[grlc_data->ul_call_errors-1] EQ grlc_data->next_poll_fn ))
    {
      trace_dl_gap = FALSE;
      TRACE_EVENT_P7("CALL ERR:c_dl_fn=%ld l_dl_fn=%ld c_ul_fn=%ld l_ul_fn=%ld callerr=%d err_fn=%ld lp_fn=%ld",
                                                    current_dl_fn,
                                                    last_dl_fn,
                                                    current_ul_fn,
                                                    last_ul_fn,
                                                    grlc_data->ul_call_errors,
                                                    grlc_data->ul_fn_errors[grlc_data->ul_call_errors-1],
                                                    grlc_data->next_poll_fn);

      grlc_data->func.mac_ready_ind.last_poll_resp = 0xFF;
      grlc_data->next_poll_fn =2 ;
    }
    else if(current_ul_fn >= grlc_data->next_poll_fn) 
    {
      trace_dl_gap = FALSE;
      TRACE_EVENT_P6("2UL GAP NO POLL:c_dl_fn=%ld l_dl_fn=%ld c_ul_fn=%ld l_ul_fn=%ld lpr=%d lp_fn=%ld",
                                                    current_dl_fn,
                                                    last_dl_fn,
                                                    current_ul_fn,
                                                    last_ul_fn,
                                                    grlc_data->func.mac_ready_ind.last_poll_resp,
                                                    grlc_data->next_poll_fn);

      grlc_data->func.mac_ready_ind.last_poll_resp = 0xFF;
      grlc_data->next_poll_fn =2 ;
    }
    else if((grlc_data->next_poll_fn > current_ul_fn) AND (current_ul_fn < 52)) /*fn overflow*/
    {
      trace_dl_gap = FALSE;
      TRACE_EVENT_P6("3UL GAP NO POLL:c_dl_fn=%ld l_dl_fn=%ld c_ul_fn=%ld l_ul_fn=%ld lpr=%d lp_fn=%ld",
                                                    current_dl_fn,
                                                    last_dl_fn,
                                                    current_ul_fn,
                                                    last_ul_fn,
                                                    grlc_data->func.mac_ready_ind.last_poll_resp,
                                                    grlc_data->next_poll_fn);
      grlc_data->func.mac_ready_ind.last_poll_resp = 0xFF;
      grlc_data->next_poll_fn =2 ;
    }
    
    if( trace_dl_gap                           AND 
        ( ( current_dl_fn - last_dl_fn ) > 5 ) AND
        ( grlc_data->dl_cnt_syn OR grlc_data->dl_fn_store[0] OR grlc_data->dl_fn_store[1] ) )
    {
      TRACE_EVENT_P2( "DL GAP: c_dl_fn=%ld l_dl_fn=%ld", current_dl_fn, last_dl_fn );
    }
  }
#endif /* _TARGET_ */

#ifdef _SIMULATION_
  {
    /*
     * copy received primitive to grlc_data->func.mac_ready_ind
     */
    grlc_data->ul_fn = mac_ready_ind->fn;
    memcpy (
            &(grlc_data->func.mac_ready_ind),
            mac_ready_ind,
            sizeof(T_MAC_READY_IND));
    PFREE(mac_ready_ind);
  }
#endif


  mac_ready_ind = &(grlc_data->func.mac_ready_ind);

#ifdef _TARGET_

  TRACE_MEMORY_PRIM ( hCommL1, hCommGRLC, MAC_READY_IND, 
                      mac_ready_ind, sizeof( T_MAC_READY_IND ) );

#endif /* #ifdef _TARGET_ */

  /*
   * handle ta value
   */
  gff_handle_continious_ta();


  switch( GET_STATE(GFF) )
  {
    case GFF_ACTIVE:
      if(grlc_data->tfi_change NEQ TFI_CHANGE_NULL)
      {
        grlc_activate_tfi(mac_ready_ind->fn);
      }
      grlc_data->ul_poll_pos_index = 0;
      if(grlc_data->gff.rlc_status EQ RLC_STATUS_BOTH)
      {
        sig_gff_ru_mac_ready_ind(mac_ready_ind);
        sig_gff_rd_mac_ready_ind(mac_ready_ind);
      }
      else if(grlc_data->gff.rlc_status EQ RLC_STATUS_UL)
        sig_gff_ru_mac_ready_ind(mac_ready_ind);
      else if(grlc_data->gff.rlc_status EQ RLC_STATUS_DL)
        sig_gff_rd_mac_ready_ind(mac_ready_ind);
      else
      {
        TRACE_ERROR("unknown rlc status: gff active but no tbf");
      }
      break;
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
    case GFF_TWO_PHASE:
      grlc_data->ul_poll_pos_index = 0;
      sig_gff_tm_mac_ready_ind_two_phase(mac_ready_ind);
      break;
#endif
    default:
      /*TRACE_ERROR( "MAC_READY_IND unexpected" );*/
      break;
  }

#if defined (_TARGET_)
  /*call control of asyn part*/
  grlc_data->ul_cnt_asyn = grlc_data->ul_cnt_syn;
#endif /* defined (_TARGET_) */


} /* gff_mac_ready_ind() */

/*
+------------------------------------------------------------------------------
| Function    : gff_l1test_call_mphp_power_control
+------------------------------------------------------------------------------
| Description : Handles the primitive L1TEST_CALL_MPHP_POWER_CONTROL
|
| Parameters  : *mphp_power_control - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/

#ifdef _SIMULATION_

GLOBAL void gff_l1test_call_mphp_power_control 
                  ( T_L1TEST_CALL_MPHP_POWER_CONTROL *call_mphp_power_control )
{ 
  PALLOC( return_mphp_power_control, L1TEST_RETURN_MPHP_POWER_CONTROL );

  maca_power_control( call_mphp_power_control->assignment_id,
                      call_mphp_power_control->crc_error,
                      call_mphp_power_control->bcch_level,
                      call_mphp_power_control->radio_freq,
                      call_mphp_power_control->burst_level,
                      return_mphp_power_control->pch );

  PSEND( hCommL1, return_mphp_power_control );

  TRACE_FUNCTION( "gff_l1test_call_mphp_power_control" );

  PFREE( call_mphp_power_control );

} /* gff_l1test_call_mphp_power_control() */

#endif /* #ifdef _SIMULATION_ */

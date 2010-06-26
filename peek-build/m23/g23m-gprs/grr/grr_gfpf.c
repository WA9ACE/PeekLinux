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
|  Purpose :  This module implements local functions for service GFP of
|             entity GRR.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRR_GFPF_C
#define GRR_GFPF_C
#endif

#define ENTITY_GRR

/*==== INCLUDES =============================================================*/

#include "typedefs.h"   /* to get Condat data types */

#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "ccdapi.h"     /* to get CCD API */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "message.h"
#include "grr.h"        /* to get the global entity definitions */
#include "grr_f.h"      /* to get the global entity procedure definitions */
#include "grr_pgs.h"   
#include "grr_psis.h"   
#include "grr_css.h"   
#include "grr_meass.h"   
#include "grr_tcs.h"   
#include "grr_ctrls.h" 
#include <stdio.h>
#include "cl_rlcmac.h"

/*==== CONST ================================================================*/


/*==== DIAGNOSTICS ==========================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/
/*
+------------------------------------------------------------------------------
| Function    : gfp_init
+------------------------------------------------------------------------------
| Description : The function gfp_init() initialize all service relevant 
|               variables.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void gfp_init ( void )
{ 
  TRACE_FUNCTION( "gfp_init" );

  INIT_STATE(GFP,GFP_IDLE);
    
} /* gfp_init() */

/*
+------------------------------------------------------------------------------
| Function    : gfp_prcs_ctrl_msg
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL gfp_prcs_ctrl_msg ( UBYTE* l2_frame,
                                UBYTE  l2_channel,
                                UBYTE  relative_position,
                                ULONG  frame_number )
{ 
  T_MSGBUF * message_complete;
  UBYTE      message_type;
  T_D_HEADER temp_header;
  BOOL       ms_paging_group;
  BOOL       address;
  BOOL       result = FALSE;
#ifdef REL99
  T_TIME time_to_poll=0;
#endif

  TRACE_FUNCTION( "gfp_prcs_ctrl_msg" );

  grr_decode_rlcmac( l2_frame, &temp_header );

#ifdef _SIMULATION_

  TRACE_EVENT_P1( "Control message at FN = %ld", frame_number );

#endif /* #ifdef _SIMULATION_ */

  if( temp_header.payload EQ CTRL_BLK_NO_OPT OR
      temp_header.payload EQ CTRL_BLK_OPT       )  
  {
    ULONG tc_user        = TC_USER4;
    UBYTE trace_msg_type = D_MSG_TYPE_2ND_SEGMENT_c;
    
    if( temp_header.payload     EQ CTRL_BLK_NO_OPT OR 
        temp_header.d_ctrl.rbsn EQ 0                  )
    {
      trace_msg_type = temp_header.d_ctrl.msg_type;
    }

    if( trace_msg_type EQ D_DL_DUMMY_c )
    {
      tc_user = TC_USER5;
    }
    
    TRACE_BINDUMP( hCommGRR, tc_user,
                   cl_rlcmac_get_msg_name( trace_msg_type, RLC_MAC_ROUTE_DL ),
                   l2_frame, MAX_L2_FRAME_SIZE ); /*lint !e569*/

    message_complete = grr_handle_rlcmac_header( PACKET_MODE_PIM,
                                                 &temp_header,
                                                 frame_number );

    if( message_complete NEQ NULL )
    {
      message_type    = grr_decode_grr( message_complete );
      ms_paging_group = ( l2_channel EQ L2_PCHANNEL_PPCH ); 

      /*
       * check address 
       */
      switch( message_type )
      {
        case D_ACCESS_REJ_c:                 /* Packet Access Reject                 */
        case D_CELL_CHAN_ORDER_c:            /* Packet Cell Change Order             */
        case D_DL_ASSIGN_c:                  /* Packet Downlink Assignment           */
        case D_MEAS_ORDER_c:                 /* Packet Measurement Order             */
        case D_PAGING_REQ_c:                 /* Packet Paging Request                */
        case D_POLLING_REQ_c:                /* Packet Polling Request               */
        case D_PRACH_PAR_c:                  /* Packet PRACH Parameters              */
        case D_QUEUING_NOT_c:                /* Packet Queuing Notification          */
        case D_UL_ASSIGN_c:                  /* Packet Uplink Assignment             */
        case D_DL_DUMMY_c:                   /* Packet Downlink Dummy Control Block  */
        case PSI_1_c:                        /* Packet System Information Type 1     */
        case PSI_2_c:                        /* Packet System Information Type 2     */
        case PSI_3_c:                        /* Packet System Information Type 3     */
        case PSI_3_BIS_c:                    /* Packet System Information Type 3 bis */
#if defined (REL99) AND defined (TI_PS_FF_EMR)
        case PSI_3_TER_c:                    /* Packet System Information Type 3 ter */
#endif
        case PSI_4_c:                        /* Packet System Information Type 4     */
        case PSI_5_c:                        /* Packet System Information Type 5     */
#ifdef REL99
        case PSI_8_c:                        /* Packet System Information Type 8     */
#endif
          address = grr_check_address( message_type,0xFF );
          {
            /* the MS shall take into account the page mode info in any message on PCCCH */
            MCAST( d_dl_assign, D_DL_ASSIGN );

            sig_gfp_pg_mode( d_dl_assign->page_mode, ms_paging_group );
          }
          break;
        
        default:
        case D_PDCH_RELEASE_c:
        case D_CTRL_PWR_TA_c:
        case D_TS_RECONFIG_c:
        case D_TBF_RELEASE_c:
        case D_UL_ACK_c:
        case PSI_13_c:
          address = FALSE;

          TRACE_EVENT( "gfp_prcs_ctrl_msg: unexpected control message type" );
          break;
      }

      if( !address )
      {                
        return( result );
      }

      /* 0460 - Section 10.4.5 
        * MS shall ignore the RRBP field if received as part of  
        * packet access reject (or) packet queue notification (or) packet paging request meassages
        * Avoid sending Poll response */
      if ( message_type NEQ D_ACCESS_REJ_c AND 
           message_type NEQ D_QUEUING_NOT_c AND
           message_type NEQ D_PAGING_REQ_c )
      {
      /*
       * save poll position
       */
      if( temp_header.d_ctrl.sp )
      {
        MCAST( d_poll_req, D_POLLING_REQ);


        if( message_type EQ D_POLLING_REQ_c )
        {
          if( d_poll_req->ctrl_ack_type EQ 0 )
          {
            sig_gfp_tc_poll_ind(frame_number,temp_header.d_ctrl.rrbp,CGRLC_POLL_RES_AB,temp_header.pctrl_ack);
          }
          else
          {
            sig_gfp_tc_poll_ind(frame_number,temp_header.d_ctrl.rrbp,CGRLC_POLL_RES_NB,temp_header.pctrl_ack);
          }
        }
        else if(psc_db->gprs_cell_opt.ctrl_ack_type)
        {
          sig_gfp_tc_poll_ind(frame_number,temp_header.d_ctrl.rrbp,CGRLC_POLL_RES_NB,temp_header.pctrl_ack);
        }
        else
        {
          sig_gfp_tc_poll_ind(frame_number,temp_header.d_ctrl.rrbp,CGRLC_POLL_RES_AB,temp_header.pctrl_ack);
        }
#ifdef REL99
        time_to_poll = grr_get_time_to_send_poll(temp_header.d_ctrl.rrbp);
#endif
      }
      }
/*

      if( message_type EQ ... )
      {
        ULONG trace[5];

        trace[0]  = l2_frame[0]  << 24;
        trace[0] |= l2_frame[1]  << 16; 
        trace[0] |= l2_frame[2]  <<  8; 
        trace[0] |= l2_frame[3]  <<  0; 

        trace[1]  = l2_frame[4]  << 24;
        trace[1] |= l2_frame[5]  << 16; 
        trace[1] |= l2_frame[6]  <<  8; 
        trace[1] |= l2_frame[7]  <<  0; 

        trace[2]  = l2_frame[8]  << 24;
        trace[2] |= l2_frame[9]  << 16; 
        trace[2] |= l2_frame[10] <<  8; 
        trace[2] |= l2_frame[11] <<  0; 

        trace[3]  = l2_frame[12] << 24;
        trace[3] |= l2_frame[13] << 16; 
        trace[3] |= l2_frame[14] <<  8; 
        trace[3] |= l2_frame[15] <<  0; 

        trace[4]  = l2_frame[16] << 24;
        trace[4] |= l2_frame[17] << 16; 
        trace[4] |= l2_frame[18] <<  8; 
        trace[4] |= l2_frame[19] <<  0; 

        TRACE_EVENT_P8( "RLC_CTRL: %08X%08X%08X%08X%08X%02X%02X%02X",
                        trace[0], trace[1], trace[2], trace[3], trace[4],
                        l2_frame[20], l2_frame[21], l2_frame[22] ); 
      }

*/

      switch( message_type )
      {
        case D_ACCESS_REJ_c:                 /* Packet Access Reject */
          {
            
            sig_gfp_tc_access_rej( );
          }
          break;

        case D_QUEUING_NOT_c:                /* Packet Queuing Notification */
          {
            
            sig_gfp_tc_queuing_not( );
          }
          break; 

        case D_UL_ASSIGN_c:                  /* Packet Uplink Assignment */
          {
            MCAST( d_ul_assign, D_UL_ASSIGN );
            

            if( d_ul_assign->v_pers_lev )
            {
              sig_gfp_psi_save_persistence_level( &d_ul_assign->pers_lev );
            }
            
            sig_gfp_tc_ul_assign( );

            result = TRUE;
          } 
          break;

        case D_DL_ASSIGN_c:                  /* Packet Downlink Assignment */
          {
            MCAST( d_dl_assign, D_DL_ASSIGN );


            if( d_dl_assign->v_pers_lev )
            {
              sig_gfp_psi_save_persistence_level( &d_dl_assign->pers_lev );
            }

            sig_gfp_tc_dl_assign( );

            result = TRUE;
          }
          break;

        case D_PAGING_REQ_c:                 /* Packet Paging Request */
          {
            MCAST( d_paging_req, D_PAGING_REQ );


            if( d_paging_req->v_pers_lev )
            {
              sig_gfp_psi_save_persistence_level( &d_paging_req->pers_lev );
            }

            sig_gfp_pg_req( );
          }
          break;

        case PSI_1_c:                        /* Packet System Information Type 1 */
          {

            sig_gfp_psi_1( );
          }
          break;

        case PSI_2_c:                        /* Packet System Information Type 2 */
          {

            sig_gfp_psi_2( relative_position );
          }
          break;

        case PSI_3_c:                        /* Packet System Information Type 3 */
          {

            sig_gfp_psi_3( relative_position );
          }
          break;

        case PSI_3_BIS_c:                    /* Packet System Information Type 3 bis */
          {

            sig_gfp_psi_3_bis( relative_position );
          }
          break;

#if defined (REL99) AND defined (TI_PS_FF_EMR)
        case PSI_3_TER_c:                    /* Packet System Information Type 3 ter */
          {
            sig_gfp_psi_3_ter( relative_position );
          }
          break;
#endif

        case PSI_4_c:                        /* Packet System Information Type 4 */
          {

            sig_gfp_psi_4( relative_position );
          }
          break;

        case PSI_5_c:                        /* Packet System Information Type 5 */
          {

            sig_gfp_psi_5( relative_position );
          }
          break;

#ifdef REL99
        case PSI_8_c:                        /* Packet System Information Type 8 */
          {

            sig_gfp_psi_8( relative_position );
          }
          break;
#endif

        case D_CELL_CHAN_ORDER_c:            /* Packet Cell Change Order */
          {
#ifdef REL99
            sig_gfp_ctrl_cc_order(time_to_poll);
#else
            sig_gfp_ctrl_cc_order( );
#endif
          }
          break;

        case D_DL_DUMMY_c:                   /* Packet Downlink Dummy Control Block */
          {
            MCAST( d_dl_dummy, D_DL_DUMMY );

            if(  d_dl_dummy->v_pers_lev  )
            {
              sig_gfp_psi_save_persistence_level( &d_dl_dummy->pers_lev );
            }


            /* 
             * This message is not send to any other service.
             */
          }
          break;

        case D_MEAS_ORDER_c:                 /* Packet Measurement Order */
          {

            sig_gfp_meas_order( );
          }
          break;

        case D_POLLING_REQ_c:                /* Packet Polling Request */
          {

            /* 
             * No signal to GRR, polling is handled with grr_poll_pos_ind
             */
          }
          break;

        case D_PRACH_PAR_c:                  /* Packet PRACH Parameters */
          {

            sig_gfp_psi_prach( );
          }
          break;

        default:
          TRACE_ERROR( "gfp_prcs_ctrl_msg: unexpected control message type" );
          break;
      } /* switch( message_type ) */

      if( grr_data->pwr_ctrl_valid_flags.v_pwr_ctrl_param      OR
          grr_data->pwr_ctrl_valid_flags.v_glbl_pwr_ctrl_param OR
          grr_data->pwr_ctrl_valid_flags.v_freq_param             )
      {
        grr_cgrlc_pwr_ctrl_req( FALSE );
      }
    } /* if( message_complete NEQ NULL ) */ 
    else 
    {
      TRACE_EVENT_P1( "Incomplete control message at FN = %ld", frame_number );
    }
  }/* if( temp_header.payload EQ CTRL_BLK_NO_OPT OR temp_header.payload EQ CTRL_BLK_OPT )  */
  else 
  {
    TRACE_ERROR( "gfp_prcs_ctrl_msg: unexpected paylaod type" );

    TRACE_BINDUMP( hCommGRR, TC_USER4,
                   cl_rlcmac_get_msg_name( D_MSG_TYPE_UNKNOWN_c, RLC_MAC_ROUTE_DL ),
                   l2_frame, MAX_L2_FRAME_SIZE ); /*lint !e569*/
  }

  return( result );

} /* gfp_prcs_ctrl_msg() */



/*
+------------------------------------------------------------------------------
| Function    : gfp_check_tfi
+------------------------------------------------------------------------------
| Description : The function gfp_check_tfi() .... 
|
| Parameters  : ptr_header_i - header parameter
|               tn           - timeslot
|
+-------------------------------------------------------------------------------
*/
GLOBAL BOOL gfp_check_tfi ( T_D_HEADER * ptr_header_i , UBYTE tn)
{ 
  BOOL result;
  TRACE_FUNCTION( "gfp_check_tfi" );

  grr_data->gfp.tfi_check_needed = TRUE;
  
 if(( ptr_header_i->payload EQ CTRL_BLK_NO_OPT) 
	  OR 
        ((ptr_header_i->payload EQ CTRL_BLK_OPT) 
		   AND 
           !(ptr_header_i->d_ctrl.ac)))
  {
    result = TRUE;
  }
  else if((ptr_header_i->payload EQ CTRL_BLK_OPT) 
	       AND 
		   ptr_header_i->d_ctrl.ac 
		   AND
		   ptr_header_i->d_ctrl.d 
		   AND
		  (ptr_header_i->d_ctrl.tfi EQ grr_data->downlink_tbf.tfi)
       AND
       ((0x80>>tn) & grr_data->downlink_tbf.ts_mask))
  {
    grr_data->gfp.tfi_check_needed = FALSE; /*tfi correct ignore tfi in air message*/
    result = TRUE;
  }
  else if((ptr_header_i->payload EQ CTRL_BLK_OPT) 
	       AND 
		   ptr_header_i->d_ctrl.ac 
		   AND
		   !(ptr_header_i->d_ctrl.d) 
		   AND
		  (ptr_header_i->d_ctrl.tfi EQ grr_data->uplink_tbf.tfi)
       AND
      ((0x80>>tn) & grr_data->uplink_tbf.ts_mask))
  {
    grr_data->gfp.tfi_check_needed = FALSE; /*tfi correct ignore tfi in air message*/
    result = TRUE;
  }
  else if(ptr_header_i->payload EQ 3)
  {
    result = TRUE;
  }
  else
  {
    result = FALSE;
    TRACE_EVENT_P7("wrong tfi in ctrl block  pt=%d ac=%d  d=%d tfi=%d dl_tfi=%d ul_tfi=%d msg_type=0x%x",
                                                             ptr_header_i->payload,
                                                             ptr_header_i->d_ctrl.ac,
                                                             ptr_header_i->d_ctrl.d,
                                                             ptr_header_i->d_ctrl.tfi,
                                                             grr_data->downlink_tbf.tfi,
                                                             grr_data->uplink_tbf.tfi,
                                                             ptr_header_i->ptr_block[0]>>2);  }

  return result;
  
} /* gfp_check_tfi() */


/*
+------------------------------------------------------------------------------
| Function    : gfp_send_ctrl_block
+------------------------------------------------------------------------------
| Description : The function gfp_send_ctrl_block() .... 
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------                       X
*/
GLOBAL void gfp_send_ctrl_block (ULONG fn_i, UBYTE tn_i, UBYTE rrbp_i, UBYTE sp_i,UBYTE pctrl_ack_i , T_MSGBUF * ptr_ctrl_block_i )
{ 
  MCAST(d_dl_assign,D_DL_ASSIGN);
  MCAST(d_poll_req,D_POLLING_REQ);
  UBYTE msg_type;
  BOOL  address;
#ifdef REL99
  T_TIME time_to_poll=0;
#endif
  TRACE_FUNCTION( "gfp_send_ctrl_block" );



  msg_type = ptr_ctrl_block_i->buf[0] >> 2;
  switch(msg_type)
  {

#ifdef _TARGET_    
   case PSI_1_c :                                /* Packet System Information Type 1 */
      /*TRACE_EVENT("PSI 1");*/
      break;
    case PSI_2_c :                                /* Packet System Information Type 2 */
      /*TRACE_EVENT("PSI 2");*/
      break;
    case PSI_3_c :                                /* Packet System Information Type 3 */
      /*TRACE_EVENT("PSI 3");*/
      break;
    case PSI_3_BIS_c :                            /* Packet System Information Type 3 bis */
      /*TRACE_EVENT("PSI 3bis");*/
      break;

#if defined (REL99) AND defined (TI_PS_FF_EMR)
    case PSI_3_TER_c :
      /*TRACE_EVENT("PSI 3ter");*/
#endif

    case PSI_4_c :                                /* Packet System Information Type 4 */
      /*TRACE_EVENT("PSI 4");*/
      break;
#ifdef REL99
    case PSI_8_c :                                /* Packet System Information Type 8 */
      /*TRACE_EVENT("PSI 8");*/
      break;
#endif
#endif /*_TARGET_    */

    /*
     * the PSI5 should not be decoded on TARGET and in SIMULATION
     */
    case PSI_5_c :                                /* Packet System Information Type 5 */
      /*TRACE_EVENT("PSI 5");*/
      break;

    /*
     * the PSI13 should be decoded on TARGET and in SIMULATION
     *
     * case PSI_13_c :                             * Packet System Information Type 13 *
     *   TRACE_EVENT("PSI 13");
     *   break; 
     */

    default:
      msg_type  = grr_decode_grr(ptr_ctrl_block_i);
      if(grr_data->gfp.tfi_check_needed)
      { /* check address within air message */
        address    = grr_check_address(msg_type,tn_i);
      }
      else
      { /* ignore address within airmessage, block was addressed with correct tfi in optional header*/
        address    = TRUE;
      }
  
      /*
       * save  persistence level, even if address is wrong
       */
      switch (msg_type )
      {
        case D_DL_ASSIGN_c :
        case D_PAGING_REQ_c:
        case D_DL_DUMMY_c : 
          if(d_dl_assign->v_pers_lev)
          {
            sig_gfp_psi_save_persistence_level( &(d_dl_assign->pers_lev) );
          }
          break; 
      }

      if (address AND grr_data->uplink_tbf.ti AND 
          (grr_data->tbf_type EQ CGRLC_TBF_MODE_UL))
      {
        /*4.60, 7.1.2.3a	RLC/MAC procedures during contention resolution
        -	the mobile station shall not accept a PACKET MEASUREMENT ORDER
          message, a PACKET CELL CHANGE ORDER message and a PACKET POWER 
          CONTROL/TIMING ADVANCE message addressing the mobile station 
          with the TFI value associated with the uplink TBF ;
        */
        
        switch (msg_type)
        {
        case D_CTRL_PWR_TA_c :
          {
            MCAST(d_ctrl_pwr_ta,D_CTRL_PWR_TA);
            if (!grr_data->gfp.tfi_check_needed OR d_ctrl_pwr_ta->add4.v_glob_tfi)
            {
              TRACE_EVENT("CTRL_PWR_TA ignored - received during contention");
              address = FALSE;
            }
          }
          break;
        case D_MEAS_ORDER_c :
          {
            MCAST(d_meas_order,D_MEAS_ORDER);
            if (!grr_data->gfp.tfi_check_needed OR d_meas_order->add1.v_glob_tfi)
            {
              TRACE_EVENT("MEAS_ORDER ignored - received during contention");
              address = FALSE;
            }
          }
          break;
        case D_CELL_CHAN_ORDER_c :
          {
            MCAST(d_cell_chan_order,D_CELL_CHAN_ORDER);
            if (!grr_data->gfp.tfi_check_needed OR d_cell_chan_order->add1.v_glob_tfi)
            {
              TRACE_EVENT("CELL_CHAN_ORDER ignored - received during contention");
              address = FALSE;
            }
          }
          break;
        default :
          break;
        }
      }

      if(address)
      {
        /*
         * handle poll position
         */
        /* 0460 - Section 10.4.5 
         * MS shall ignore the RRBP field if received as part of  
         * packet access reject (or) packet queue notification (or) packet paging request meassages
         * Avoid sending Poll response */
        if( sp_i  AND 
           (msg_type NEQ D_UL_ACK_c) AND /* packet uplink ack/nack will be handled in grlc*/
           (msg_type NEQ D_ACCESS_REJ_c) AND 
           (msg_type NEQ D_PAGING_REQ_c ) ) 
        {
          PALLOC(cgrlc_poll_req,CGRLC_POLL_REQ);

          cgrlc_poll_req->poll_fn  = grr_calc_new_poll_pos(fn_i, rrbp_i);
          cgrlc_poll_req->tn       = tn_i;
          cgrlc_poll_req->ctrl_ack = pctrl_ack_i;
          if ((msg_type EQ D_POLLING_REQ_c) AND (d_poll_req->ctrl_ack_type))
          {
            cgrlc_poll_req->poll_b_type = CGRLC_POLL_RES_NB;
          }
          else if (msg_type EQ D_POLLING_REQ_c AND (d_poll_req->ctrl_ack_type EQ 0))
          {
            cgrlc_poll_req->poll_b_type = CGRLC_POLL_RES_AB;
          }
          else
          {
            cgrlc_poll_req->poll_b_type = CGRLC_POLL_CTRL;
          }
          grr_data->l1_del_tbf_start_fn = grr_decode_tbf_start_rel (cgrlc_poll_req->poll_fn,1); 
          PSEND(hCommGRLC,cgrlc_poll_req);
#ifdef REL99
          time_to_poll = grr_get_time_to_send_poll(rrbp_i);
#endif
        }
        if(msg_type NEQ D_DL_DUMMY_c)
        {
          if (sp_i)
          {
            TRACE_EVENT_P3("correct add with Poll: msg_type=0x%2x,fn_i=%ld,rrbp=%d ",
                                                                  msg_type,
                                                                  fn_i,
                                                                  rrbp_i );
          }
          else
          {
            TRACE_EVENT_P2("correct add at fn=%ld: msg_type = 0x%2x",fn_i,msg_type);
          }
        }
/*

        if( msg_type EQ ... )
        {
          ULONG trace[5];

          trace[0]  = ptr_ctrl_block_i->buf[0]  << 24;
          trace[0] |= ptr_ctrl_block_i->buf[1]  << 16; 
          trace[0] |= ptr_ctrl_block_i->buf[2]  <<  8; 
          trace[0] |= ptr_ctrl_block_i->buf[3]  <<  0; 

          trace[1]  = ptr_ctrl_block_i->buf[4]  << 24;
          trace[1] |= ptr_ctrl_block_i->buf[5]  << 16; 
          trace[1] |= ptr_ctrl_block_i->buf[6]  <<  8; 
          trace[1] |= ptr_ctrl_block_i->buf[7]  <<  0; 

          trace[2]  = ptr_ctrl_block_i->buf[8]  << 24;
          trace[2] |= ptr_ctrl_block_i->buf[9]  << 16; 
          trace[2] |= ptr_ctrl_block_i->buf[10] <<  8; 
          trace[2] |= ptr_ctrl_block_i->buf[11] <<  0; 

          trace[3]  = ptr_ctrl_block_i->buf[12] << 24;
          trace[3] |= ptr_ctrl_block_i->buf[13] << 16; 
          trace[3] |= ptr_ctrl_block_i->buf[14] <<  8; 
          trace[3] |= ptr_ctrl_block_i->buf[15] <<  0; 

          trace[4]  = ptr_ctrl_block_i->buf[16] << 24;
          trace[4] |= ptr_ctrl_block_i->buf[17] << 16; 
          trace[4] |= ptr_ctrl_block_i->buf[18] <<  8; 
          trace[4] |= ptr_ctrl_block_i->buf[19] <<  0; 

          TRACE_EVENT_P8( "RLC_CTRL: %08X%08X%08X%08X%08X%02X%02X%02X",
                          trace[0], trace[1], trace[2], trace[3], trace[4],
                          ptr_ctrl_block_i->buf[20], ptr_ctrl_block_i->buf[21],
                          ptr_ctrl_block_i->buf[22] ); 
        }

*/

        switch (msg_type )
        {
        case D_ACCESS_REJ_c :                         /* Packet Access Reject */
          sig_gfp_tc_access_rej_ptm();
          break;
        case D_TS_RECONFIG_c:                         /* Packet Timeslot reconfigure */
          sig_gfp_tc_ts_reconfig_ptm ();
          break; 
        case D_UL_ASSIGN_c :                          /* Packet Uplink Assignment */
          sig_gfp_tc_ul_assign_ptm ();
          break;
        case D_DL_ASSIGN_c :                          /* Packet Downlink Assignment */
          sig_gfp_tc_dl_assign_ptm ();
          break;
        case D_TBF_RELEASE_c :                        /* Packet TBF release */
          sig_gfp_tc_packet_tbf_rel_ptm(fn_i,rrbp_i,sp_i);
          break;
        case D_PAGING_REQ_c :                         /* Packet Paging Request */
          sig_gfp_pg_req ();
          break;
        case D_UL_ACK_c :                             /* Packet Uplink Ack/Nack */
          {
            MCAST(d_ul_ack,D_UL_ACK);
            if(grr_data->uplink_tbf.mac_mode EQ FIXED_ALLOCATION)
            {
              sig_gfp_tc_fix_alloc_ack_ptm();
            }
            /* 
             * send ta update, if pta is present and allocation bitmap is not present.
             * If pta and allocation bitmap is present, than the ta update is sent 
             * with MPHP_ASSIGNMENT_REQ primitive.
             */
            if( d_ul_ack->gprs_ul_ack_nack_info.v_pta AND
                (
                  (grr_data->uplink_tbf.mac_mode NEQ FIXED_ALLOCATION)  OR
                  (!(d_ul_ack->gprs_ul_ack_nack_info.v_f_alloc_ack AND d_ul_ack->gprs_ul_ack_nack_info.f_alloc_ack.v_fa_s2))
                )
              )
            {
              sig_gfp_tc_update_ta_req_ptm();
            }
          }
          break;
        case PSI_1_c :                                /* Packet System Information Type 1 */
          sig_gfp_psi_1_ptm ();
          break;
        case PSI_2_c :                                /* Packet System Information Type 2 */
          sig_gfp_psi_2_ptm ();
          break;
        case PSI_3_c :                                /* Packet System Information Type 3 */
          sig_gfp_psi_3_ptm ();
          break;
        case PSI_3_BIS_c :                            /* Packet System Information Type 3 bis */
          sig_gfp_psi_3_bis_ptm ();
          break;

#if defined (REL99) AND defined (TI_PS_FF_EMR)
        case PSI_3_TER_c :                            /* Packet System Information Type 3 ter */
          sig_gfp_psi_3_ter_ptm ();
#endif
		  /*lint -fallthrough*/

        case PSI_4_c :                                /* Packet System Information Type 4 */
          sig_gfp_psi_4_ptm ();
          break;
#ifdef REL99
        case PSI_8_c :                                /* Packet System Information Type 8 */
          sig_gfp_psi_8_ptm ();
          break;
#endif
        case PSI_13_c :                               /* Packet System Information Type 13 */
          sig_gfp_psi_13_ptm ();
          break;
        case D_CELL_CHAN_ORDER_c :                    /* Packet Cell Change Order */
#ifdef REL99
          sig_gfp_ctrl_cc_order (time_to_poll);
#else
          sig_gfp_ctrl_cc_order ();
#endif
          break;
        case D_DL_DUMMY_c :                           /* Packet Downlink Dummy Control Block */
          break;
        case D_MEAS_ORDER_c :                         /* Packet Measurement Order */
          sig_gfp_meas_order ();
          break;
        case D_PDCH_RELEASE_c :                       /* Packet PDCH Release */
          TRACE_EVENT_P4("pdch rel on tn %d: %4x-%4x-%4x",
                                                          tn_i,
                                                          ptr_ctrl_block_i->buf[0],
                                                          ptr_ctrl_block_i->buf[1],
                                                          ptr_ctrl_block_i->buf[2]);/*lint !e415 !e416*/
          sig_gfp_tc_pdch_release_ptm ( tn_i );
          break;
        case D_POLLING_REQ_c :                        /* Packet Polling Request */
          break;
        case D_CTRL_PWR_TA_c :                        /* Packet control power timing advance Parameters */
          sig_gfp_meas_ctrl_pwr_ta_ptm (  );
          break;
        default:
          TRACE_ERROR( "CGRLC_DATA_IND with unexpected ctrl message typ " );
          break;
        } /* switch (msg_type) */

        if( grr_data->pwr_ctrl_valid_flags.v_pwr_ctrl_param      OR
            grr_data->pwr_ctrl_valid_flags.v_glbl_pwr_ctrl_param OR
            grr_data->pwr_ctrl_valid_flags.v_freq_param             )
        {
          grr_cgrlc_pwr_ctrl_req( FALSE );
        }
      }
      else
      {
        TRACE_EVENT_P3("not add to MS: msg_type = %2x  fn_i=%ld sp=%d",msg_type,fn_i,sp_i);
      }
      break;
  }
  grr_data->l1_del_tbf_start_fn = GRR_INVALID_FN;
}/* gfp_send_ctrl_block() */

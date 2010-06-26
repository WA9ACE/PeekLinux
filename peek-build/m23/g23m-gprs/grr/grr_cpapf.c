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
|  Purpose :  This module implements local functions for service CPAP of
|             entity GRR.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRR_CPAPF_C
#define GRR_CPAPF_C
#endif

#define ENTITY_GRR

/*==== INCLUDES =============================================================*/

#include <string.h>
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
#include "pcm.h"        /* to get a lot of macros */
#include "grr.h"        /* to get the global entity definitions */
#include "grr_f.h"      /* to get the global function definitions */
#include "grr_tcf.h"    /* to get the tc function definitions */
#include "grr_tcs.h"    /* to get the tc sig function definitions */
#include "grr_cpapf.h"
#include "grr_meass.h"  /* to get the definitions for interference measurements */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

LOCAL void cpap_set_da_assignment_pdch (T_MPHP_ASSIGNMENT_REQ * ptr2prim_i);
LOCAL void cpap_set_dl_assignment_pdch (T_MPHP_ASSIGNMENT_REQ * ptr2prim_i);
LOCAL BOOL cpap_check_and_save_freq (void);


/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : cpap_init
+------------------------------------------------------------------------------
| Description : The function cpap_init() .... 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void cpap_init ( void )
{ 
  TRACE_FUNCTION( "cpap_init" );

  INIT_STATE(CPAP,CPAP_IDLE);
  grr_data->cpap.new_tbf_type   = CGRLC_TBF_MODE_NULL;
  grr_data->cpap.v_tma_ia       = FALSE;
  grr_data->cpap.p_d_imm_assign = NULL;
} /* cpap_init() */


/*
+------------------------------------------------------------------------------
| Function    : cpap_eval_ia
+------------------------------------------------------------------------------
| Description : The function cpap_eval_ia() .... 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL T_EVAL_IA cpap_eval_ia ( void )
{ 
  MCAST(d_imm_assign,D_IMM_ASSIGN);/*  T_D_IMM_ASSIGN T_U_CTRL_ACK */
  T_EVAL_IA result = E_IA_NULL;
  T_tfi_ass_alloc * p_tfi_ass;
  TRACE_FUNCTION( "cpap_eval_ia" );

  if(d_imm_assign->tma EQ TMA_1) 
  {
    /* the 1st message is only delivered by rr if the 2nd message has arrived */
    /* with correct request reference so it has not to be checked here again  */
    result = E_IA_TMA;       /* 1st part of TWO MESSAGE ASSIGNMENT */
  }
  else if(d_imm_assign->d_t EQ D_T_DED)
  {
    result = E_IA_DCCH;
  }
  else if(d_imm_assign->ia_rest_oct.v_ia_assign_par AND
       d_imm_assign->ia_rest_oct.ia_assign_par.v_pck_upl_ass_ia AND
       d_imm_assign->ia_rest_oct.ia_assign_par.pck_upl_ass_ia.v_tfi_ass_alloc)
  {
    T_pck_upl_ass_ia *pck_upl_ass_ia = 
      &(d_imm_assign->ia_rest_oct.ia_assign_par.pck_upl_ass_ia);
    p_tfi_ass = &(pck_upl_ass_ia->tfi_ass_alloc); /* more contents than before*/

    grr_data->uplink_tbf.ts_usage     = (0x80>>d_imm_assign->pck_chan_desc.tn);
    grr_data->uplink_tbf.nts          = 1;
    /*
     * handle polling bit
     */
    if(p_tfi_ass->poll AND
        (!grr_check_if_tbf_start_is_elapsed(grr_decode_tbf_start_abs((T_abs *)&d_imm_assign->ia_rest_oct.ia_assign_par.pck_upl_ass_ia.tfi_ass_alloc.tbf_start_time ),grr_data->ul_fn)))
      grr_data->uplink_tbf.polling_bit  = d_imm_assign->pck_chan_desc.tn;
    else
      grr_data->uplink_tbf.polling_bit  = 0xFF;  
    
    grr_data->uplink_tbf.cs_mode      = p_tfi_ass->ccm;
    grr_data->uplink_tbf.tlli_cs_mode = p_tfi_ass->tlli_bcc;


    
    if(p_tfi_ass->allo_flag EQ 0)    /* Dynamic Allocation */
    { 
      if(p_tfi_ass->v_usf AND p_tfi_ass->v_usf_gran)
      {
        result = E_IA_UL;             /* Dynamic Allocation */
        grr_data->uplink_tbf.mac_mode = CGRLC_MAC_MODE_DA;
      }
      else /* dynamic alloc but no usf value */
      {
        result = E_IA_ERROR_RA;
        TRACE_ERROR("dynamic alloc but no usf value!");
      }
    }
    else /*fixed alloc*/
      if (p_tfi_ass->v_allo_len5 AND p_tfi_ass->v_allo_bmp5 AND 
                                         p_tfi_ass->v_tbf_start_time)
    {
      result = E_IA_UL;               /* Fixed Allocation */
      grr_data->uplink_tbf.mac_mode = CGRLC_MAC_MODE_FA;
    }
    else /* fixed alloc but no alloc bitmap or tbf starting time */
    {
      result = E_IA_ERROR_RA;
      TRACE_ERROR("fixed alloc but no alloc bitmap or tbf starting time!");
    }

    if (!cpap_check_and_save_freq())
    {
      result = E_IA_ERROR_RA;
      TRACE_EVENT("cpap_eval_ia: return E_IA_ERROR_RA (frequencies wrong)");
    }

    if( result NEQ E_IA_ERROR_RA )
    {
      /* process power control parameter */
      grr_store_type_pck_upl_ass_ia( p_tfi_ass,
                                     d_imm_assign->pck_chan_desc.tn );
    }
  }
  else if(d_imm_assign->ia_rest_oct.v_ia_assign_par AND
     d_imm_assign->ia_rest_oct.ia_assign_par.v_pck_upl_ass_ia AND
     d_imm_assign->ia_rest_oct.ia_assign_par.pck_upl_ass_ia.v_sngl_block_alloc)
  { /* single block */
    T_sngl_block_alloc *sngl_block_ass = 
        &d_imm_assign->ia_rest_oct.ia_assign_par.pck_upl_ass_ia.sngl_block_alloc;

    if( grr_data->tc.v_sb_without_tbf )
    {
      result = E_IA_SB_WITHOUT_TBF;
    }
    else
    {
      result = E_IA_SB_2PHASE_ACCESS;
    }

    if (!cpap_check_and_save_freq())
    {
      result = E_IA_ERROR_RA;
      TRACE_EVENT("cpap_eval_ia: return E_IA_ERROR_RA (frequencies wrong)");
    }

    if( result NEQ E_IA_ERROR_RA )
    {
      /* process power control parameter */
      grr_store_type_pck_snbl_ass_ia( sngl_block_ass,
                                      d_imm_assign->pck_chan_desc.tn );
    }
  }
  else if(d_imm_assign->ia_rest_oct.v_ia_assign_par AND
     d_imm_assign->ia_rest_oct.ia_assign_par.v_ia_2nd_part)
  {
    result = E_IA_TMA_SECOND;
  }
  else
  {
    result = E_IA_ERROR_RA;
    TRACE_ERROR("corrupted message recieved!");
  }
  return(result);
} /* cpap_eval_ia() */




/*
+------------------------------------------------------------------------------
| Function    : cpap_eval_ia_dl
+------------------------------------------------------------------------------
| Description : The function cpap_eval_ia_dl() .... 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL T_EVAL_IA_DL cpap_eval_ia_dl ( void )
{ 
  MCAST(d_imm_assign,D_IMM_ASSIGN);/*  T_D_IMM_ASSIGN */
  T_EVAL_IA_DL result = E_IA_DL_IGNORE;
  T_pck_downl_ass_ia * p_dl_assign;

  TRACE_FUNCTION( "cpap_eval_ia_dl" );

  if(d_imm_assign->d_t EQ D_T_DED)
  {
    result = E_IA_DL_DCCH;
  }
  else if(d_imm_assign->tma EQ TMA_1) 
  {
    /* the 1st message is only delivered by rr if the 2nd message has arrived */
    /* with correct request reference so it has not to be checked here again  */
    if(d_imm_assign->ia_rest_oct.v_ia_assign_par AND
    d_imm_assign->ia_rest_oct.ia_assign_par.v_pck_downl_ass_ia AND
    d_imm_assign->ia_rest_oct.ia_assign_par.pck_downl_ass_ia.v_tfi_ass_rlc)
    {
      p_dl_assign =
        &(d_imm_assign->ia_rest_oct.ia_assign_par.pck_downl_ass_ia);
      if(grr_check_all_tlli((BUF_tlli_value *)&(p_dl_assign->ded_tlli)))
      {
        result = E_IA_DL_TMA;        
      }
      else
      {
        result = E_IA_DL_NOT_OURS;
      }
    }
    else
    {
      result = E_IA_DL_IGNORE;
      TRACE_ERROR("the 1st message is only delivered by rr if the 2nd message has arrived");
    }
  }
  else if(d_imm_assign->ia_rest_oct.v_ia_assign_par AND
    d_imm_assign->ia_rest_oct.ia_assign_par.v_pck_downl_ass_ia AND
    d_imm_assign->ia_rest_oct.ia_assign_par.pck_downl_ass_ia.v_tfi_ass_rlc)
  {
    p_dl_assign =
      &(d_imm_assign->ia_rest_oct.ia_assign_par.pck_downl_ass_ia);
    if(grr_check_all_tlli((BUF_tlli_value *)&(p_dl_assign->ded_tlli)))
    {
      result = E_IA_DL_ASSIGN;
      grr_data->downlink_tbf.ts_usage    = (0x80>>d_imm_assign->pck_chan_desc.tn);
      grr_data->downlink_tbf.nts         = 1; 
      /*
       * handle polling bit
       */
      if(p_dl_assign->tfi_ass_rlc.poll AND
         (!grr_check_if_tbf_start_is_elapsed(grr_decode_tbf_start_abs((T_abs *)&d_imm_assign->ia_rest_oct.ia_assign_par.pck_downl_ass_ia.tbf_start_time ),grr_data->ul_fn)))
        grr_data->downlink_tbf.polling_bit = d_imm_assign->pck_chan_desc.tn;
      else
        grr_data->downlink_tbf.polling_bit = 0xFF;
 
      grr_data->downlink_tbf.mac_mode    = DA;                          /* by default set to */
      grr_data->downlink_tbf.t3192       = FALSE;                      /* indicates if t3192 is running*/
      grr_data->downlink_tbf.rlc_mode    = p_dl_assign->tfi_ass_rlc.rlc_mode;
      grr_data->downlink_tbf.ctrl_ack_bit= 0;   

      if (!cpap_check_and_save_freq())
      {
        result = E_IA_DL_IGNORE;
        TRACE_EVENT("cpap_eval_ia_dl: return E_IA_DL_IGNORE (frequencies wrong)");
      }

      if( result EQ E_IA_DL_ASSIGN )
      {
        grr_store_type_tfi_ass_rlc( &p_dl_assign->tfi_ass_rlc,
                                    d_imm_assign->pck_chan_desc.tn );
      }
    }
    else
    {
      result = E_IA_DL_NOT_OURS;
    }
  }
  else if(d_imm_assign->ia_rest_oct.v_ia_assign_par AND
    d_imm_assign->ia_rest_oct.ia_assign_par.v_pck_downl_ass_ia AND
    d_imm_assign->ia_rest_oct.ia_assign_par.pck_downl_ass_ia.v_tbf_start_time)
  {
    p_dl_assign =
      &(d_imm_assign->ia_rest_oct.ia_assign_par.pck_downl_ass_ia);
    if(grr_check_all_tlli((BUF_tlli_value *)&(p_dl_assign->ded_tlli)))
    {
      result = E_IA_DL_SB;
      if (!cpap_check_and_save_freq())
      {
        result = E_IA_DL_IGNORE;
        TRACE_EVENT("cpap_eval_ia_dl: return E_IA_DL_IGNORE (frequencies wrong)");
      }
    }
    else
    {
      result = E_IA_DL_NOT_OURS;
    }
  }
  else if(d_imm_assign->ia_rest_oct.v_ia_assign_par AND
    d_imm_assign->ia_rest_oct.ia_assign_par.v_ia_2nd_part)
  {
    result = E_IA_DL_TMA_SECOND;
  }

  if(result EQ E_IA_DL_IGNORE)
  {
    TRACE_ERROR("None of the if switches were reached LINE 330 cpapf");
  }
  return(result);
} /* cpap_eval_ia_dl() */


/*
+------------------------------------------------------------------------------
| Function    : cpap_eval_pdch_assignment
+------------------------------------------------------------------------------
| Description : The function cpap_eval_pdch_assignment() .... 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL T_EVAL_PDCH_ASSIGN cpap_eval_pdch_assignment ( void )
{ 
  MCAST(d_pdch_ass_cmd,D_PDCH_ASS_CMD);/*  T_D_PDCH_ASS_CMD */
  T_EVAL_PDCH_ASSIGN result = E_PDCH_ASSIGN_IGNORE;

  TRACE_FUNCTION( "cpap_eval_pdch_assignment" );

  if(d_pdch_ass_cmd->msg_type EQ D_PDCH_ASS_CMD)
  {
    if(!(d_pdch_ass_cmd->v_pck_ul_ass OR d_pdch_ass_cmd->v_pck_dl_ass))
    {
      result = E_PDCH_ASSIGN_ERROR;
      TRACE_ERROR("no Packet Assignment included in message");
    }
    else if(d_pdch_ass_cmd->v_pck_ul_ass AND d_pdch_ass_cmd->pck_ul_ass.v_dyn_alloc)
    {
      UBYTE tx_slots = 0, ts_usage = 0,i,mask;
      grr_data->uplink_tbf.mac_mode = CGRLC_MAC_MODE_DA;
      result = E_PDCH_ASSIGN_UL;
      if(d_pdch_ass_cmd->pck_ul_ass.dyn_alloc.v_tagged_usf_tn)
      {
        for(i = 0,mask = 0x80;i < 8;i++)
        {
          if(d_pdch_ass_cmd->pck_ul_ass.dyn_alloc.tagged_usf_tn[i].v_usf)
          {
            ts_usage |= mask;
            tx_slots++;
          }
          mask >>= 1;
        }
      }
      else if(d_pdch_ass_cmd->pck_ul_ass.dyn_alloc.v_usf_gamma_csn1)
      {
        for(i = 0,mask = 0x80;i < 8;i++)
        {
          if(d_pdch_ass_cmd->pck_ul_ass.dyn_alloc.usf_gamma_csn1[i].v_usf_gamma)
          {
            ts_usage |= mask;
            tx_slots++;
          }
          mask >>= 1;
        }
      }
      grr_store_type_dyn_alloc( &d_pdch_ass_cmd->pck_ul_ass.dyn_alloc );
     
      grr_data->uplink_tbf.ts_usage = ts_usage;
      grr_data->uplink_tbf.nts = tx_slots;
      if( !handle_ms_cap(UL_ASSIGNMENT) )
      {
        result = E_PDCH_ASSIGN_ERROR;
        TRACE_ERROR("too much or no timeslot(s) assigned!");
      }
    }
    else if(d_pdch_ass_cmd->v_pck_ul_ass AND 
                                    d_pdch_ass_cmd->pck_ul_ass.v_single_alloc)
    {
      result = E_PDCH_ASSIGN_SB;
      if( d_pdch_ass_cmd->pck_ul_ass.single_alloc.v_alpha_gamma EQ TRUE )
      {
        grr_store_type_alpha_gamma
                 ( &d_pdch_ass_cmd->pck_ul_ass.single_alloc.alpha_gamma,
                    d_pdch_ass_cmd->pck_ul_ass.single_alloc.tn );
      }

      /* SZML-CPAP/029 */
    }
    else if(d_pdch_ass_cmd->v_pck_ul_ass AND 
                                       d_pdch_ass_cmd->pck_ul_ass.v_fix_alloc)
    {
      result = E_PDCH_ASSIGN_UL;
      grr_data->uplink_tbf.mac_mode = CGRLC_MAC_MODE_FA;
      if( d_pdch_ass_cmd->pck_ul_ass.fix_alloc.v_pwr_ctrl EQ TRUE )
      {
        grr_store_type_pwr_ctrl( &d_pdch_ass_cmd->pck_ul_ass.fix_alloc.pwr_ctrl );
      }

      /* SZML-CPAP/030 */
    }
    else if(d_pdch_ass_cmd->v_pck_dl_ass)
    {
      result = E_PDCH_ASSIGN_DL;
      if( d_pdch_ass_cmd->pck_dl_ass.v_pwr_ctrl EQ TRUE )
      {
        grr_store_type_pwr_ctrl( &d_pdch_ass_cmd->pck_dl_ass.pwr_ctrl );
      }

      /* SZML-CPAP/031 */
    }
  }
  else
  {
    TRACE_ERROR("wrong message type!");
  }
  return(result);
} /* cpap_eval_pdch_assignment() */


/*
+------------------------------------------------------------------------------
| Function    : cpap_build_gprs_data_request
+------------------------------------------------------------------------------
| Description : The function cpap_build_gprs_data_request() .... 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void cpap_build_gprs_data_request (T_RRGRR_GPRS_DATA_REQ *rrgrr_gprs_data_req)
{ 
  /* T_D_RR_INIT_REQ */ /* T_PRIM_QUEUE */
  /*  T_D_IMM_ASSIGN */ /* T_GRR_DATA_REQ */

  TRACE_FUNCTION( "cpap_build_gprs_data_request" );

  rrgrr_gprs_data_req->new_ptmsi = grr_data->db.ms_id.new_ptmsi;
  rrgrr_gprs_data_req->old_ptmsi = grr_data->db.ms_id.old_ptmsi;

  rrgrr_gprs_data_req->tlli = grr_data->db.ms_id.new_tlli;
  rrgrr_gprs_data_req->rai  = grr_data->db.ms_id.rai;
  rrgrr_gprs_data_req->mac_req = DA;
  rrgrr_gprs_data_req->cs_req  = COD_S_1;
  rrgrr_gprs_data_req->p_chan_req_des.mo_mt = OR_TY_MO;
  if( grr_data->tc.v_sb_without_tbf )
  {
    rrgrr_gprs_data_req->p_chan_req_des.llc_type     = LLC_NOT_ACK;
    rrgrr_gprs_data_req->p_chan_req_des.rlc_mode_req = RLC_UNACK_MODE;
    rrgrr_gprs_data_req->p_chan_req_des.prio = RADIO_PRIO_4;
    rrgrr_gprs_data_req->p_chan_req_des.req_bwd = 80;/* 8000 bit/s */
    rrgrr_gprs_data_req->p_chan_req_des.rlc_octets = 22;/* one RLC/MAC block */
  }
  else
  {
    UBYTE i=0;
    USHORT j =1;
    if(grr_data->uplink_tbf.prim_type EQ CGRLC_LLC_PRIM_TYPE_DATA_REQ)
    { /* CGRLC_LLC_PRIM_TYPE_DATA_REQ */
      rrgrr_gprs_data_req->p_chan_req_des.llc_type     = LLC_NOT_ACK;
      rrgrr_gprs_data_req->p_chan_req_des.rlc_mode_req = RLC_ACK_MODE;
    } 
    else
    { /* CGRLC_LLC_PRIM_TYPE_UNITDATA_REQ*/
      rrgrr_gprs_data_req->p_chan_req_des.llc_type     = LLC_IS_ACK;
      rrgrr_gprs_data_req->p_chan_req_des.rlc_mode_req = RLC_UNACK_MODE;
    }
    rrgrr_gprs_data_req->p_chan_req_des.prio =  grr_data->uplink_tbf.prio;

    rrgrr_gprs_data_req->p_chan_req_des.req_bwd    = 0;

    for(i=2; i<=grr_data->uplink_tbf.peak;i++)
    {
      j *= 2;
    }
    if(grr_data->uplink_tbf.peak)
      rrgrr_gprs_data_req->p_chan_req_des.req_bwd = j*80;

    rrgrr_gprs_data_req->p_chan_req_des.rlc_octets = grr_data->uplink_tbf.rlc_oct_cnt; 
  }

  rrgrr_gprs_data_req->gprs_meas_results.c_value  = meas_c_get_value( );
  rrgrr_gprs_data_req->gprs_meas_results.rxqual   = 0;
  rrgrr_gprs_data_req->gprs_meas_results.sign_var = 0;

} /* cpap_build_gprs_data_request() */




/*
+------------------------------------------------------------------------------
| Function    : cpap_send_ass_fail
+------------------------------------------------------------------------------
| Description : The function cpap_send_ass_fail() .... 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void cpap_send_ass_fail (UBYTE cause)
{  
  TRACE_FUNCTION( "cpap_send_ass_fail" );
  {
    PALLOC_SDU(rrgrr_data_req,RRGRR_DATA_REQ,3*8);

    rrgrr_data_req->sdu.l_buf  = 3*8;
    rrgrr_data_req->sdu.o_buf  = 0;
    rrgrr_data_req->sdu.buf[0] = 0x06;            /* protocol discriminator & skip indicator */
    rrgrr_data_req->sdu.buf[1] = U_ASSIGN_FAIL;   /* message type */                                   /*lint !e415*/
    rrgrr_data_req->sdu.buf[2] = cause;           /* RRC_CHANNEL_MODE*/ /* assignment failure cause */ /*lint !e415 !e416*/
    sig_cpap_tc_dcch_data_req(rrgrr_data_req);
  }
} /* cpap_send_ass_fail() */


/*
+------------------------------------------------------------------------------
| Function    : cpap_send_assign_req_pdch
+------------------------------------------------------------------------------
| Description : The function cpap_send_assign_req_pdch() builds MPHP_ASSIGNMENT_REQ
|               and send it.
|
| Parameters  : tbf_type_i - type of TBF that is to activate
|
+------------------------------------------------------------------------------
*/
GLOBAL void cpap_send_assign_req_pdch (T_TBF_TYPE tbf_type_i)
{ 
  TRACE_FUNCTION( "cpap_send_assign_req_pdch" );
  {
    PALLOC(ptr2prim,MPHP_ASSIGNMENT_REQ);
    
    memset(ptr2prim,0,sizeof(T_MPHP_ASSIGNMENT_REQ));
  
    grr_set_tbf_cfg_req_param( ptr2prim );
    
    switch( tbf_type_i )
    {
      case CGRLC_TBF_MODE_UL:
        ptr2prim->assign_cmd = UL_ASSIGNMENT;
        if(grr_data->uplink_tbf.mac_mode EQ CGRLC_MAC_MODE_DA)
          cpap_set_da_assignment_pdch(ptr2prim);
        else
        {
          /* SZML-CPAP/032 */
        }

        sig_cpap_tc_assign_pdch(ptr2prim, PDCH_UL_ASS_CMD);
        break;
      case CGRLC_TBF_MODE_DL:
        ptr2prim->assign_cmd = DL_ASSIGNMENT;
        cpap_set_dl_assignment_pdch(ptr2prim);
        sig_cpap_tc_assign_pdch(ptr2prim, PDCH_DL_ASS_CMD);
        break;
      default:
        PFREE(ptr2prim);
        TRACE_ERROR ( "unknown tbf type!" );
        break;
    } /* switch (tbf_type_i) */
  }
} /* cpap_send_assign_req_pdch() */


/*
+------------------------------------------------------------------------------
| Function    : cpap_set_da_assignment_pdch
+------------------------------------------------------------------------------
| Description : This function sets some parameter of MPHP_ASSIGFNMENT_REQ in 
|               case of uplink dynamic allocation.
|
| Parameters  : ptr2prim_i -  ptr to mphp_assignment_req
|
+------------------------------------------------------------------------------
*/
LOCAL void cpap_set_da_assignment_pdch (T_MPHP_ASSIGNMENT_REQ * ptr2prim_i)
{
  UBYTE i;
  MCAST(d_pdch_ass_cmd,D_PDCH_ASS_CMD); /* T_D_PDCH_ASS_CMD */
                                        /* T_D_IMM_ASSIGN   */

  TRACE_FUNCTION( "cpap_set_da_assignment_pdch" );
  if( d_pdch_ass_cmd->msg_type EQ D_PDCH_ASS_CMD) 
  {
    /* bts downlink power control parameters */
    if(d_pdch_ass_cmd->pck_ul_ass.dyn_alloc.v_p0)
      ptr2prim_i->p_dl_power.p0 = d_pdch_ass_cmd->pck_ul_ass.dyn_alloc.p0;
    else
      ptr2prim_i->p_dl_power.p0 = 0xff;
    /* dynamic allocation must use mode a */
    ptr2prim_i->p_dl_power.bts_pwr_ctl_mode = MODE_A;
    ptr2prim_i->p_dl_power.pr_mode = PR_MODE_A_ONE;
    /*
     * timing advance
     */
    if(d_pdch_ass_cmd->pck_ul_ass.pck_ta.v_ta)
    {
      ptr2prim_i->p_timing_advance.ta_value =
                                         d_pdch_ass_cmd->pck_ul_ass.pck_ta.ta;
    }
    else
    {
      ptr2prim_i->p_timing_advance.ta_value = 0xff;
    }
    if(d_pdch_ass_cmd->pck_ul_ass.pck_ta.v_ta_idx_nm)
    {
      ptr2prim_i->p_timing_advance.ta_index =
                           d_pdch_ass_cmd->pck_ul_ass.pck_ta.ta_idx_nm.ta_idx;
      ptr2prim_i->p_timing_advance.tn = 
                            d_pdch_ass_cmd->pck_ul_ass.pck_ta.ta_idx_nm.ta_nm;
    }
    else
    {
      ptr2prim_i->p_timing_advance.ta_index = 0xff;
      ptr2prim_i->p_timing_advance.tn = 0xff;
    }
    /*
     *  trainings sequence
     */    
    ptr2prim_i->tsc = d_pdch_ass_cmd->chan_desc.tsc; 
    /*
     *  frequency parameters
     */
    if(!d_pdch_ass_cmd->chan_desc.hop)
    {
      if(d_pdch_ass_cmd->chan_desc.v_arfcn)
      {
        ptr2prim_i->p_frequency_par.p_chan_sel.hopping=0;
        memset(ptr2prim_i->p_frequency_par.p_freq_list.p_rf_chan_no.p_radio_freq,0,MPHP_NUMC_MA);
        ptr2prim_i->p_frequency_par.p_chan_sel.p_rf_ch.arfcn = 
          grr_g23_arfcn_to_l1(d_pdch_ass_cmd->chan_desc.arfcn);
      }
    }
    /* SZML-CPAP/002 */
    /* SZML-CPAP/003 */
   /*
    * mac mode 
    */
    ptr2prim_i->mac_mode = 
                    (d_pdch_ass_cmd->pck_ul_ass.dyn_alloc.ext_dyn_all)? EDA:DA;
   /*
    * uplink allocation structure 
    */
    if(d_pdch_ass_cmd->pck_ul_ass.dyn_alloc.v_tfi)
      ptr2prim_i->p_ul_alloc.ul_tfi = d_pdch_ass_cmd->pck_ul_ass.dyn_alloc.tfi;
    else
      ptr2prim_i->p_ul_alloc.ul_tfi = grr_data->uplink_tbf.tfi; 
    ptr2prim_i->p_ul_alloc.ts_mask  = grr_data->uplink_tbf.ts_usage; 
   /*
    *  dynamic allocation structure
    */
    ptr2prim_i->p_ul_alloc.p_dynamic_alloc.usf_gran = 
                               d_pdch_ass_cmd->pck_ul_ass.dyn_alloc.usf_gran;
    if(d_pdch_ass_cmd->pck_ul_ass.dyn_alloc.v_tagged_usf_tn)
    {
      for(i = 0;i < 8;i++)
      {
        ptr2prim_i->p_ul_alloc.p_dynamic_alloc.usf_table[i] = 
                    d_pdch_ass_cmd->pck_ul_ass.dyn_alloc.tagged_usf_tn[i].usf;
      }
    }
    else if(d_pdch_ass_cmd->pck_ul_ass.dyn_alloc.v_usf_gamma_csn1)
    {
      for(i = 0;i < 8;i++)
      {
        ptr2prim_i->p_ul_alloc.p_dynamic_alloc.usf_table[i] = 
         d_pdch_ass_cmd->pck_ul_ass.dyn_alloc.usf_gamma_csn1[i].usf_gamma.usf;
      }
    }
    else
    {
      ptr2prim_i->p_ul_alloc.p_dynamic_alloc.usf_table[0] = 5;
      TRACE_EVENT("USF-Error: set (hardcoded) USF_TN0 = 0x05");
    }
    /*  
     * TBF starting time
     */
    if(d_pdch_ass_cmd->v_start_time)
    {
      ptr2prim_i->p_tbf_start.tbf_start_present = 1;
      ptr2prim_i->p_tbf_start.fn = grr_decode_tbf_start_abs((T_abs *)&d_pdch_ass_cmd->start_time);
      if(d_pdch_ass_cmd->v_chan_desc_before)
      {
        PALLOC(ptr2prim2,MPHP_ASSIGNMENT_REQ);
        memset(ptr2prim2,0,sizeof(T_MPHP_ASSIGNMENT_REQ));

        grr_set_tbf_cfg_req_param( ptr2prim2 );
  
        ptr2prim2->assign_cmd = UL_ASSIGNMENT;
        ptr2prim2->p_dl_power.p0 = ptr2prim_i->p_dl_power.p0;
        ptr2prim2->p_dl_power.bts_pwr_ctl_mode = 
                                  ptr2prim_i->p_dl_power.bts_pwr_ctl_mode;
        ptr2prim2->p_dl_power.pr_mode = ptr2prim_i->p_dl_power.pr_mode;
        ptr2prim2->p_timing_advance.ta_value =
                                    ptr2prim_i->p_timing_advance.ta_value;
        ptr2prim2->p_timing_advance.ta_index =
                                    ptr2prim_i->p_timing_advance.ta_index;
        ptr2prim2->p_timing_advance.tn = ptr2prim_i->p_timing_advance.tn;
        /*  trainings sequence  */    
        ptr2prim2->tsc = d_pdch_ass_cmd->chan_desc_before.tsc; 
        /*  frequency parameters  */
        if(!d_pdch_ass_cmd->chan_desc_before.hop)
        {
          if(d_pdch_ass_cmd->chan_desc_before.v_arfcn)
          {
            ptr2prim_i->p_frequency_par.p_chan_sel.hopping=0;
            memset(ptr2prim_i->p_frequency_par.p_freq_list.p_rf_chan_no.p_radio_freq,0,MPHP_NUMC_MA);
            ptr2prim_i->p_frequency_par.p_chan_sel.p_rf_ch.arfcn = 
                  grr_g23_arfcn_to_l1(d_pdch_ass_cmd->chan_desc_before.arfcn);
          }
        }
        /* SZML-CPAP/004 */
        /* SZML-CPAP/005 */
        /*  mac mode  */
        ptr2prim2->mac_mode = ptr2prim_i->mac_mode;
        /*  uplink allocation structure  */
        /* take from grr_data to avoide mistakes */
        ptr2prim2->p_ul_alloc.ul_tfi  = ptr2prim_i->p_ul_alloc.ul_tfi; 
        ptr2prim2->p_ul_alloc.ts_mask = ptr2prim_i->p_ul_alloc.ts_mask; 
        /*  dynamic allocation structure  */
        ptr2prim2->p_ul_alloc.p_dynamic_alloc.usf_gran = 
                        ptr2prim_i->p_ul_alloc.p_dynamic_alloc.usf_gran;
        for(i = 0;i < 8;i++)
        {
          ptr2prim2->p_ul_alloc.p_dynamic_alloc.usf_table[i] = 
                    ptr2prim_i->p_ul_alloc.p_dynamic_alloc.usf_table[i];
        }
        ptr2prim2->p_tbf_start.tbf_start_present = 0;
        sig_cpap_tc_assign_pdch(ptr2prim2, PDCH_UL_ASS_CMD);
      } /* if(d_pdch_ass_cmd->v_chan_desc_before) */
    }
    else /* if(d_pdch_ass_cmd->v_start_time) */
    {
      ptr2prim_i->p_tbf_start.tbf_start_present  = 0;
      ptr2prim_i->p_tbf_start.fn = 0xFFFFFFFF;
    }
  } 
  else /* if( d_pdch_ass_cmd->msg_type EQ D_PDCH_ASS_CMD) */
  {
    TRACE_ERROR ( "Wrong message type in _decodedCtrlMsg" );
  }


  if(ptr2prim_i->p_tbf_start.tbf_start_present)
    grr_data->uplink_tbf.tbf_start_fn = ptr2prim_i->p_tbf_start.fn;
  else
    grr_data->uplink_tbf.tbf_start_fn = CGRLC_STARTING_TIME_NOT_PRESENT;
  
  grr_data->uplink_tbf.st_tfi   = ptr2prim_i->p_ul_alloc.ul_tfi;
  grr_data->uplink_tbf.mac_mode = DA;

  return; 
}  /* cpap_set_da_assignment_pdch */





/*
+------------------------------------------------------------------------------
| Function    : cpap_set_dl_assignment_pdch
+------------------------------------------------------------------------------
| Description : This function sets some parameter of T_MPHP_ASSIGNMENT_REQ is 
|               case of ia downlink allocation
|
| Parameters  : ptr2prim_i -  ptr to MPHP_ASSIGNMENT_REQ
|
+------------------------------------------------------------------------------
*/
LOCAL void cpap_set_dl_assignment_pdch (T_MPHP_ASSIGNMENT_REQ * ptr2prim_i)
{
  MCAST(d_imm_assign,D_IMM_ASSIGN); /* T_D_IMM_ASSIGN */
  T_pck_downl_ass_ia *ptr2dl_assign;

  TRACE_FUNCTION( "cpap_set_dl_assignment_pdch" );
  if( d_imm_assign->msg_type EQ D_IMM_ASSIGN)
  {
    ptr2dl_assign = 
      &d_imm_assign->ia_rest_oct.ia_assign_par.pck_downl_ass_ia;
    /*
     * timing advance
     */
    if (ptr2dl_assign->tfi_ass_rlc.ta_valid)
    {
      ptr2prim_i->p_timing_advance.ta_value = d_imm_assign->time_advance.ta;
    }
    else
    {
      ptr2prim_i->p_timing_advance.ta_value = 0xff;
    }
    if(ptr2dl_assign->v_ta_idx)
    {
      ptr2prim_i->p_timing_advance.ta_index = ptr2dl_assign->ta_idx;
      ptr2prim_i->p_timing_advance.tn      = d_imm_assign->pck_chan_desc.tn;
    }
    else
    {
      ptr2prim_i->p_timing_advance.ta_index = 0xff;
      ptr2prim_i->p_timing_advance.tn = 0xff;
    }
    /*
     *  trainings sequence
     */    
    ptr2prim_i->tsc = d_imm_assign->pck_chan_desc.tsc; 
    /*
     * TBF starting time
     */
    if(ptr2dl_assign->v_tbf_start_time)
    {
      ptr2prim_i->p_tbf_start.tbf_start_present  = 1;
      ptr2prim_i->p_tbf_start.fn = grr_decode_tbf_start_abs((T_abs *)&ptr2dl_assign->tbf_start_time);
    }
    else
    {
      ptr2prim_i->p_tbf_start.tbf_start_present = 0;
    }
   /* 
    * mac mode 
    */
    ptr2prim_i->mac_mode = DA;
   /*
    *  downlink allocation structure
    */
    ptr2prim_i->p_dl_alloc.dl_tfi  = grr_data->downlink_tbf.tfi; /* take from grr_data to avoide mistakes */
    ptr2prim_i->p_dl_alloc.ts_mask = grr_data->downlink_tbf.ts_usage;


    if(ptr2prim_i->p_tbf_start.tbf_start_present)
      grr_data->downlink_tbf.tbf_start_fn = ptr2prim_i->p_tbf_start.fn;
    else
      grr_data->downlink_tbf.tbf_start_fn = CGRLC_STARTING_TIME_NOT_PRESENT;
    
    grr_data->downlink_tbf.st_tfi   = ptr2prim_i->p_dl_alloc.dl_tfi;
    grr_data->downlink_tbf.mac_mode = DA;

  } 
  return; 
}  /* cpap_set_dl_assignment_pdch()  */


/*
+------------------------------------------------------------------------------
| Function    : cpap_send_assign_req_ia
+------------------------------------------------------------------------------
| Description : The function cpap_send_assign_req_ia() builds MPHP_ASSIGNMENT_REQ
|               and send it.
|
| Parameters  : tbf_type_i - type of TBF that is to activate
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL cpap_send_assign_req_ia ( T_TBF_TYPE tbf_type_i )
{ 
  T_MPHP_ASSIGNMENT_REQ *ptr2prim = NULL;

  TRACE_FUNCTION( "cpap_send_assign_req_ia" );

  ptr2prim = tc_set_freq();
  if(NULL EQ ptr2prim)
  {
    return FALSE;
  }

  grr_set_tbf_cfg_req_param( ptr2prim );

  switch( tbf_type_i )
  {
    case CGRLC_TBF_MODE_UL:
      ptr2prim->assign_cmd = UL_ASSIGNMENT;
      if(grr_data->uplink_tbf.mac_mode EQ CGRLC_MAC_MODE_DA)
        cpap_set_da_assignment_ia(ptr2prim);
      else if(!cpap_set_fa_assignment_ia(ptr2prim))
      {
        PFREE(ptr2prim);
        return FALSE;
      }
      sig_cpap_tc_assign_pdch( ptr2prim, IA_UL );
      break;
    case CGRLC_TBF_MODE_DL:
      ptr2prim->assign_cmd = DL_ASSIGNMENT;
      cpap_set_dl_assignment_ia(ptr2prim);
      sig_cpap_tc_assign_pdch( ptr2prim, IA_DL );
      break;
    default:
      return FALSE;
  } /* switch (tbf_type_i) */
  return TRUE;
} /* cpap_send_assign_req_ia() */


/*
+------------------------------------------------------------------------------
| Function    : cpap_set_da_assignment_ia
+------------------------------------------------------------------------------
| Description : This function sets some parameter of MPHP_ASSIGFNMENT_REQ in 
|               case of uplink dynamic allocation.
|
| Parameters  : ptr2prim_i -  ptr to mphp_assignment_req
|
+------------------------------------------------------------------------------
*/
GLOBAL void cpap_set_da_assignment_ia (T_MPHP_ASSIGNMENT_REQ *ptr2prim)
{
  MCAST(d_imm_assign,D_IMM_ASSIGN); /* T_D_IMM_ASSIGN */
  T_pck_upl_ass_ia *ptr2ul_assign;

  TRACE_FUNCTION( "cpap_set_da_assignment_ia" );

  if( d_imm_assign->msg_type EQ D_IMM_ASSIGN) 
  {
    /*
     * timing advance
     */
    ptr2ul_assign =
      &d_imm_assign->ia_rest_oct.ia_assign_par.pck_upl_ass_ia;
    if(ptr2ul_assign->v_tfi_ass_alloc AND ptr2ul_assign->tfi_ass_alloc.v_p0_prmode)
    {
      ptr2prim->p_dl_power.p0 = ptr2ul_assign->tfi_ass_alloc.p0_prmode.p0;
      ptr2prim->p_dl_power.pr_mode =
                                   ptr2ul_assign->tfi_ass_alloc.p0_prmode.pr_mode;
    }
    else
    {
      ptr2prim->p_dl_power.p0 = 0xff;
    }
  /* Timing Advance */
    grr_data->ta_params.ta_valid = TRUE;
    grr_handle_ta ( 1, /* always present in Immediate Assignment*/
          d_imm_assign->time_advance.ta,
          ptr2ul_assign->tfi_ass_alloc.v_ta_idx, 
          ptr2ul_assign->tfi_ass_alloc.ta_idx,
          d_imm_assign->pck_chan_desc.tn,
          0xFF,
          0,
          0,
          &ptr2prim->p_timing_advance);

    /*  TBF starting time  */
    if(ptr2ul_assign->tfi_ass_alloc.v_tbf_start_time)
    {
      ptr2prim->p_tbf_start.tbf_start_present   = 1;
      ptr2prim->p_tbf_start.fn = grr_decode_tbf_start_abs((T_abs *)&ptr2ul_assign->tfi_ass_alloc.tbf_start_time);      
    }
    else
    {
      ptr2prim->p_tbf_start.tbf_start_present  = 0;
    } 
   /*
    * mac mode 
    */
    ptr2prim->mac_mode = DA;   
   /*
    * uplink allocation structure 
    */
    /* take from grr_data to avoide mistakes */
    ptr2prim->p_ul_alloc.ul_tfi  = ptr2ul_assign->tfi_ass_alloc.tfi; 
    ptr2prim->p_ul_alloc.ts_mask = grr_data->uplink_tbf.ts_usage; 
   /*
    *  dynamic allocation structure
    */    
    ptr2prim->p_ul_alloc.p_dynamic_alloc.usf_gran = 
                                        ptr2ul_assign->tfi_ass_alloc.usf_gran;
    if(ptr2ul_assign->tfi_ass_alloc.v_usf)
    {
      ptr2prim->p_ul_alloc.p_dynamic_alloc.usf_table[
           d_imm_assign->pck_chan_desc.tn] = ptr2ul_assign->tfi_ass_alloc.usf;
    }
    else
    {
      ptr2prim->p_ul_alloc.p_dynamic_alloc.usf_table[
                                          d_imm_assign->pck_chan_desc.tn] = 5;
      TRACE_EVENT("USF-Error: set (hardcoded) USF_TN0 = 0x05");
    }
  } 
  else
  {
    TRACE_ERROR ( "Wrong message type in _decodedCtrlMsg" );
  }


  if(ptr2prim->p_tbf_start.tbf_start_present)
    grr_data->uplink_tbf.tbf_start_fn = ptr2prim->p_tbf_start.fn;
  else
    grr_data->uplink_tbf.tbf_start_fn = CGRLC_STARTING_TIME_NOT_PRESENT;
  
  grr_data->uplink_tbf.st_tfi   = ptr2prim->p_ul_alloc.ul_tfi;
  grr_data->uplink_tbf.mac_mode = DA;


  return; 
}  /* cpap_set_da_assignment_ia */


/*
+------------------------------------------------------------------------------
| Function    : cpap_set_fa_assignment_ia
+------------------------------------------------------------------------------
| Description : This function sets some parameter of MPHP_ASSIGFNMENT_REQ in 
|               case of uplink fixed allocation.
|
| Parameters  : ptr2prim_i -  ptr to mphp_assignment_req
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE cpap_set_fa_assignment_ia (T_MPHP_ASSIGNMENT_REQ *ptr2prim)
{
  MCAST(d_imm_assign,D_IMM_ASSIGN); /* T_D_IMM_ASSIGN */
  T_pck_upl_ass_ia *ptr2ul_assign;

  TRACE_FUNCTION( "cpap_set_fa_assignment_ia" );

  ptr2ul_assign =
      &d_imm_assign->ia_rest_oct.ia_assign_par.pck_upl_ass_ia;
  if( d_imm_assign->msg_type EQ D_IMM_ASSIGN) 
  {
    /* PO, BTS_POWER_MODE and PR_ MODE */
    if(ptr2ul_assign->v_tfi_ass_alloc AND ptr2ul_assign->tfi_ass_alloc.v_p0_bts_prmode)
    {      
      ptr2prim->p_dl_power.p0      = ptr2ul_assign->tfi_ass_alloc.p0_bts_prmode.p0;
      ptr2prim->p_dl_power.bts_pwr_ctl_mode  = ptr2ul_assign->tfi_ass_alloc.p0_bts_prmode.pwr_ctrl_mode;
      ptr2prim->p_dl_power.pr_mode = ptr2ul_assign->tfi_ass_alloc.p0_bts_prmode.pr_mode;
    }
    else
    {
      ptr2prim->p_dl_power.p0 = 0xff;
    }
    /* 
   * timing advance
   */
    grr_data->ta_params.ta_valid = TRUE;
    grr_handle_ta ( 1, /* always present in Immediate Assignment*/
          d_imm_assign->time_advance.ta,
          ptr2ul_assign->tfi_ass_alloc.v_ta_idx, 
          ptr2ul_assign->tfi_ass_alloc.ta_idx,
          d_imm_assign->pck_chan_desc.tn,
          0xFF,
          0,
          0,
          &ptr2prim->p_timing_advance);
    /*
     * mac mode 
     */
    ptr2prim->mac_mode = FA;   
   /*
    * uplink allocation structure 
    */
    /* take from grr_data to avoide mistakes */
    ptr2prim->p_ul_alloc.ul_tfi  = ptr2ul_assign->tfi_ass_alloc.tfi; 
    ptr2prim->p_ul_alloc.ts_mask = grr_data->uplink_tbf.ts_usage; 
    /*  TBF starting time  */
    if(ptr2ul_assign->tfi_ass_alloc.v_tbf_start_time)
    {
      ptr2prim->p_tbf_start.tbf_start_present  = 1;
      ptr2prim->p_tbf_start.fn = grr_decode_tbf_start_abs((T_abs *)&ptr2ul_assign->tfi_ass_alloc.tbf_start_time);
    }
    else
    {
      TRACE_ERROR("IA FIX ALLOC WITHOUT TBF STARTING TIME");
      return FALSE;
    }
    /*  
     * DOWNLINK_CONTROL_TIMESLOT parameter shall always indicate a timeslot number 
     * which is used for TBF uplink.  GSM 04.60 Chapter 8.1.1.3.4 
     */
    switch(grr_data->uplink_tbf.ts_usage)
    {
    case 1:
      ptr2prim->p_ul_alloc.p_fixed_alloc.dl_ctrl_ts = 7;
      break;    
    case 2:
      ptr2prim->p_ul_alloc.p_fixed_alloc.dl_ctrl_ts = 6;
      break;
    case 4:
      ptr2prim->p_ul_alloc.p_fixed_alloc.dl_ctrl_ts = 5;
      break;
    case 8:
      ptr2prim->p_ul_alloc.p_fixed_alloc.dl_ctrl_ts = 4;
      break;
    case 16:
      ptr2prim->p_ul_alloc.p_fixed_alloc.dl_ctrl_ts = 3;
      break;
    case 32:
      ptr2prim->p_ul_alloc.p_fixed_alloc.dl_ctrl_ts = 2;
      break;
    case 64:
      ptr2prim->p_ul_alloc.p_fixed_alloc.dl_ctrl_ts = 1;
      break;
    case 128:
      ptr2prim->p_ul_alloc.p_fixed_alloc.dl_ctrl_ts = 0;
      break;
}
    
    /*
     * ALLOCATION BITMAP tc function call not allowed
     */
     grr_data->tc.fa_ctrl.fa_type        = FA_NO_CURRENT;
     tc_calc_fa_bitmap(ptr2prim->p_tbf_start.fn,                 /* starting time of cuurent tbf*/
                        1,                                          /* block periods*/
                        ptr2ul_assign->tfi_ass_alloc.allo_len5,     /* bitmap length */
                        ptr2ul_assign->tfi_ass_alloc.allo_bmp5,     /* ptr to alloc struct*/
                        &ptr2prim->p_ul_alloc.p_fixed_alloc); /* ptr to fix sttruct*/
  } 
  else
  {
    TRACE_ERROR ( "Wrong message type in _decodedCtrlMsg" );
  }

    
  if(ptr2prim->p_tbf_start.tbf_start_present)
    grr_data->uplink_tbf.tbf_start_fn = ptr2prim->p_tbf_start.fn;
  else
    grr_data->uplink_tbf.tbf_start_fn = CGRLC_STARTING_TIME_NOT_PRESENT;
  
  grr_data->uplink_tbf.st_tfi   = ptr2prim->p_ul_alloc.ul_tfi;
  grr_data->uplink_tbf.mac_mode = FA;

  return TRUE; 
}  /* cpap_set_fa_assignment_ia */


/*
+------------------------------------------------------------------------------
| Function    : cpap_set_dl_assignment_ia
+------------------------------------------------------------------------------
| Description : This function sets some parameter of T_MPHP_ASSIGNMENT_REQ is 
|               case of ia downlink allocation
|
| Parameters  : ptr2prim_i -  ptr to MPHP_ASSIGNMENT_REQ
|
+------------------------------------------------------------------------------
*/
GLOBAL void cpap_set_dl_assignment_ia (T_MPHP_ASSIGNMENT_REQ *ptr2prim)
{
  MCAST(d_imm_assign,D_IMM_ASSIGN); /* T_D_IMM_ASSIGN */
  T_pck_downl_ass_ia *ptr2dl_assign;
  TRACE_FUNCTION( "cpap_set_dl_assignment_ia" );
  if( d_imm_assign->msg_type EQ D_IMM_ASSIGN)
  {
    ptr2dl_assign = &d_imm_assign->ia_rest_oct.ia_assign_par.pck_downl_ass_ia;
    if(ptr2dl_assign->v_p0_bts_prmode)
    {
      ptr2prim->p_dl_power.p0 = ptr2dl_assign->p0_bts_prmode.p0;
      ptr2prim->p_dl_power.bts_pwr_ctl_mode =
                                   ptr2dl_assign->p0_bts_prmode.pwr_ctrl_mode;
      ptr2prim->p_dl_power.pr_mode = ptr2dl_assign->p0_bts_prmode.pr_mode;
    }
    else
    {
      ptr2prim->p_dl_power.p0 = 0xff;
    }
    /*
     * timing advance
     */
    grr_handle_ta ( ptr2dl_assign->tfi_ass_rlc.ta_valid, 
          d_imm_assign->time_advance.ta,
          0xFF, 
          0,
          0,
          ptr2dl_assign->v_ta_idx,
          ptr2dl_assign->ta_idx,
          d_imm_assign->pck_chan_desc.tn,
          &ptr2prim->p_timing_advance);
    /*
     * TBF starting time
     */
    if(ptr2dl_assign->v_tbf_start_time)
    {
      ptr2prim->p_tbf_start.tbf_start_present  = 1;
      ptr2prim->p_tbf_start.fn  = grr_decode_tbf_start_abs((T_abs *)&ptr2dl_assign->tbf_start_time);    
    }
    else
    {
      ptr2prim->p_tbf_start.tbf_start_present  = 0;
    }

   /* SZML-CPAP/008 */
    ptr2prim->mac_mode = DA;
   /*
    *  downlink allocation structure
    */
    ptr2prim->p_dl_alloc.dl_tfi  = ptr2dl_assign->tfi_ass_rlc.tfi;
    ptr2prim->p_dl_alloc.ts_mask = grr_data->downlink_tbf.ts_usage;

 
    if(ptr2prim->p_tbf_start.tbf_start_present)
      grr_data->downlink_tbf.tbf_start_fn = ptr2prim->p_tbf_start.fn;
    else
      grr_data->downlink_tbf.tbf_start_fn = CGRLC_STARTING_TIME_NOT_PRESENT;

    grr_data->downlink_tbf.st_tfi      = ptr2prim->p_dl_alloc.dl_tfi;

  } 
  return; 
}  /* cpap_set_dl_assignment_ia()  */


/*
+------------------------------------------------------------------------------
| Function    : cpap_send_resource_request_ia
+------------------------------------------------------------------------------
| Description : The function cpap_send_resource_request_ia() sends the primitive 
|               MPHP_SINGLE_BLOCK_REQ with Packet Resource Request
|               for two phase access due to the reception of a Immediate
|               Assignment message. 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE cpap_send_resource_request_ia ( void )
{
  { 
    MCAST(d_imm_assign,D_IMM_ASSIGN);  /* T_D_IMM_ASSIGN */
    
    T_pck_upl_ass_ia *ptr2ul_assign;
    UBYTE frame[23];
    ULONG start_fn;


    TRACE_FUNCTION( "cpap_send_resource_request_ia" );

    ptr2ul_assign = &d_imm_assign->ia_rest_oct.ia_assign_par.pck_upl_ass_ia;

    start_fn = grr_decode_tbf_start_abs((T_abs *)&ptr2ul_assign->sngl_block_alloc.tbf_start_time);
    if( !(grr_check_if_tbf_start_is_elapsed ( start_fn, grr_data->ul_fn)))
    {
      T_U_RESOURCE_REQ resource_req;
      sig_cpap_tc_build_res_req( &resource_req );
      grr_encode_ul_ctrl_block( frame, ( UBYTE* )&resource_req );
    }
    else
    {
      TRACE_ERROR("TBF Starting time is elpased in single block allocation!");
      return FALSE;
    }
    {
      PALLOC(ptr2prim,MPHP_SINGLE_BLOCK_REQ);
      memcpy( ptr2prim->l2_frame, frame, 23 );
      ptr2prim->p_tbf_start.tbf_start_present = 1; 
      ptr2prim->p_tbf_start.fn                = start_fn;

      if( ptr2ul_assign->v_sngl_block_alloc AND ptr2ul_assign->sngl_block_alloc.v_p0_bts_prmode )
      {
        ptr2prim->p_dl_power.p0               = ptr2ul_assign->sngl_block_alloc.p0_bts_prmode.p0;
        ptr2prim->p_dl_power.bts_pwr_ctl_mode = ptr2ul_assign->sngl_block_alloc.p0_bts_prmode.pwr_ctrl_mode;
        ptr2prim->p_dl_power.pr_mode          = ptr2ul_assign->sngl_block_alloc.p0_bts_prmode.pr_mode;
      }
      else
      {
        ptr2prim->p_dl_power.p0      = 0xff;
      }
   
      /*
       *   mark that single block is because of two phase access procedure 
       */
      ptr2prim->purpose      = TWO_PHASE_ACESS;
      ptr2prim->pc_meas_chan = psc_db->g_pwr_par.pc_meas_chan;
      ptr2prim->burst_type   = (psc_db->gprs_cell_opt.ab_type EQ AB_8_BIT)
                               ?AB_8_BIT
                               :AB_11_BIT;
     
      grr_handle_ta ( 1, /*Ta is mandatory in immediate assignment*/
                      d_imm_assign->time_advance.ta,
                      0, 
                      0,
                      0,
                      0,
                      0,
                      0,
                      &ptr2prim->p_timing_advance);

      /*
       *  trainings sequence
       */    
      ptr2prim->tsc = d_imm_assign->pck_chan_desc.tsc; 
      ptr2prim->tn  = d_imm_assign->pck_chan_desc.tn;

      /*
       *  frequency parameters
       */
      grr_set_freq_par( &ptr2prim->p_frequency_par );
      sig_cpap_tc_assign_sb( ( void* )ptr2prim, TWO_PHASE_ACESS );
    }
    return TRUE;
  }
} /* cpap_send_resource_request_ia() */


/*
+------------------------------------------------------------------------------
| Function    : cpap_send_single_block_without_tbf
+------------------------------------------------------------------------------
| Description : The function cpap_send_single_block_without_tbf() sends 
|               the primitive MPHP_SINGLE_BLOCK_REQ with single
|               block request without TBF establishment due to the reception
|               of an immediate assignment message. 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE cpap_send_single_block_without_tbf ( void )
{
  PALLOC( prim, MPHP_SINGLE_BLOCK_REQ );
  {
    MCAST( d_imm_assign, D_IMM_ASSIGN );

    T_pck_upl_ass_ia                   *assign;
    UBYTE                               result = FALSE;

    TRACE_FUNCTION( "cpap_send_single_block_without_tbf" );

    assign = &d_imm_assign->ia_rest_oct.ia_assign_par.pck_upl_ass_ia;

    if( assign->v_sngl_block_alloc AND assign->sngl_block_alloc.v_p0_bts_prmode )
    {
      prim->p_dl_power.p0               = assign->sngl_block_alloc.p0_bts_prmode.p0;
      prim->p_dl_power.bts_pwr_ctl_mode = assign->sngl_block_alloc.p0_bts_prmode.pwr_ctrl_mode;
      prim->p_dl_power.pr_mode          = assign->sngl_block_alloc.p0_bts_prmode.pr_mode;
    }
    else
    {
      prim->p_dl_power.p0      = 0xff;
    }

    /*
     *   mark that single block is because of two phase access procedure 
     */
    prim->purpose      = SINGLE_BLOCK_TRANSFER_UL;
    prim->pc_meas_chan = psc_db->g_pwr_par.pc_meas_chan;
    prim->burst_type   = ( psc_db->gprs_cell_opt.ab_type EQ AB_8_BIT )
                         ? AB_8_BIT : AB_11_BIT;

    prim->p_timing_advance.ta_value = d_imm_assign->time_advance.ta;

    grr_data->ta_params.ta_value = d_imm_assign->time_advance.ta;
    grr_data->ta_params.ta_valid = TRUE;

    if( assign->tfi_ass_alloc.v_ta_idx )
    {
      prim->p_timing_advance.ta_index = assign->tfi_ass_alloc.ta_idx;
      prim->p_timing_advance.tn       = d_imm_assign->pck_chan_desc.tn;
    }
    else
    {
      prim->p_timing_advance.ta_index = 0xff;
      prim->p_timing_advance.tn       = 0xff;
    }

    /*
     *  trainings sequence
     */    
    prim->tsc = d_imm_assign->pck_chan_desc.tsc; 

    /*
    TRACE_EVENT_P4( "request_ia1 tn:%d tsc: %d p0:%d pr_mode:%d", 
                    d_imm_assign->pck_chan_desc.tn, prim->tsc, 
                    prim->p_dl_power.p0, prim->p_dl_power.pr_mode );
    */


    /* SZML-CPAP/009 */
    /*
     * handle TBF starting time is always present in single block allocation,
     * otherwise something is wrong
     */  

    if( assign->v_sngl_block_alloc)
    {
      prim->p_tbf_start.tbf_start_present  = 1; 
      prim->p_tbf_start.fn = grr_decode_tbf_start_abs((T_abs*) &assign->sngl_block_alloc.tbf_start_time);     
      if(grr_check_if_tbf_start_is_elapsed ( prim->p_tbf_start.fn, grr_data->ul_fn))
      {
        TRACE_ERROR("TBF Starting time is ELAPSED in single block allocation!");
        TRACE_EVENT_P2("CPAP SBR TBF ST ELAPSED st=%ld  c_fn=%ld  ", prim->p_tbf_start.fn,grr_data->dl_fn);
      }
      else
      {
        result = TRUE;    
      }
    }
 
    if( result )
    {
      tc_cpy_ctrl_blk_to_buffer( prim->l2_frame );

      prim->tsc = grr_data->tc.freq_set.tsc;
      prim->tn  = d_imm_assign->pck_chan_desc.tn;

      /*
       *  frequency parameters
       */
      grr_set_freq_par( &prim->p_frequency_par );
      sig_cpap_tc_assign_sb( (void*)prim, SINGLE_BLOCK_TRANSFER_UL );

    /*
    TRACE_EVENT_P8("request_ia2 tn:%d tsc: %d ta:%d arfcn:%ld : nr: %d p0:%d pr:%d ctrl:%d", 
      prim_i->tn,
      prim_i->tsc, 
      prim_i->p_timing_advance, 
      prim_i->p_chan_sel.p_rf_ch.arfcn,
      prim_i->ptr2prim->p_frequency_par.p_freq_list.p_rf_chan_cnt,
      prim_i->p_dl_power.p0,
      prim_i->p_dl_power.pr_mode,
      prim_i->p_dl_power.bts_pwr_ctl_mode);    
    */
    }
    else
    {
      PFREE(prim);
    }

    return result;
  }
} /* cpap_send_single_block_without_tbf() */


/*
+------------------------------------------------------------------------------
| Function    : cpap_send_receive_normal_burst
+------------------------------------------------------------------------------
| Description : The function cpap_send_receive_normal_burst() sends the 
|               primitive MPHP_SINGLE_BLOCK_REQ to receive a single packet 
|               control block on the specified radio resource.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE cpap_send_receive_normal_burst ( void )
{ 
  MCAST( d_imm_assign, D_IMM_ASSIGN );

  T_pck_downl_ass_ia *ptr2dl_assign =
    &d_imm_assign->ia_rest_oct.ia_assign_par.pck_downl_ass_ia;
  UBYTE               result        = FALSE;

  PALLOC( mphp_single_block_req, MPHP_SINGLE_BLOCK_REQ );

  TRACE_FUNCTION( "cpap_send_receive_normal_burst" );

  memset( mphp_single_block_req, 0, sizeof( T_MPHP_SINGLE_BLOCK_REQ ) );

  mphp_single_block_req->assign_id = 0;
  grr_data->tc.last_rec_nb_id      = mphp_single_block_req->assign_id;
  mphp_single_block_req->purpose   = SINGLE_BLOCK_TRANSFER_DL;
  mphp_single_block_req->tsc       = grr_data->tc.freq_set.tsc;
  mphp_single_block_req->tn        = d_imm_assign->pck_chan_desc.tn;

  grr_set_freq_par( &mphp_single_block_req->p_frequency_par );

  if( ptr2dl_assign->v_p0_bts_prmode )
  {
    mphp_single_block_req->p_dl_power.p0 = ptr2dl_assign->p0_bts_prmode.p0;
    mphp_single_block_req->p_dl_power.bts_pwr_ctl_mode = 
                                    ptr2dl_assign->p0_bts_prmode.pwr_ctrl_mode;
    mphp_single_block_req->p_dl_power.pr_mode = ptr2dl_assign->p0_bts_prmode.pr_mode;
  }
  else
  {
    mphp_single_block_req->p_dl_power.p0               = 0xff;
    mphp_single_block_req->p_dl_power.bts_pwr_ctl_mode = 0xff;
    mphp_single_block_req->p_dl_power.pr_mode          = 0xff;
  }

  mphp_single_block_req->pc_meas_chan = psc_db->g_pwr_par.pc_meas_chan;
  mphp_single_block_req->burst_type   = (psc_db->gprs_cell_opt.ab_type EQ AB_8_BIT)
                                        ?AB_8_BIT
                                        :AB_11_BIT;

  /* a downlink single block don't need timing advance!! */
  mphp_single_block_req->p_timing_advance.ta_value = 0xff;
  mphp_single_block_req->p_timing_advance.ta_index = 0xff;
  mphp_single_block_req->p_timing_advance.tn       = 0xff;
  /*
   * handle TBF starting time
   * is always present in single block allocation (otherwise something is wrong)
   */
  if(ptr2dl_assign->v_tbf_start_time)
  {
    mphp_single_block_req->p_tbf_start.tbf_start_present = 1; 
    mphp_single_block_req->p_tbf_start.fn = grr_decode_tbf_start_abs((T_abs *)&ptr2dl_assign->tbf_start_time);
    if(grr_check_if_tbf_start_is_elapsed ( mphp_single_block_req->p_tbf_start.fn, grr_data->ul_fn))
    {
      TRACE_ERROR("TBF Starting time is ELAPSED in single block allocation!");
      TRACE_EVENT_P2("CPAP SBR TBF ST ELAPSED st=%ld  c_fn=%ld  ", mphp_single_block_req->p_tbf_start.fn,grr_data->dl_fn);
    }
    else
    {
      result = TRUE;  
      grr_data->tc.last_rec_nb_fn = mphp_single_block_req->p_tbf_start.fn;
    }
  }
  else
  {
    TRACE_ERROR("TBF Starting time is missing in single block allocation!");
    /* SZML-CPAP/013 */
  }

  if(result)
  {
    sig_cpap_tc_assign_sb((void*)mphp_single_block_req,SINGLE_BLOCK_TRANSFER_DL);
  }
  else
  {
    PFREE(mphp_single_block_req);
    TRACE_ERROR("SINGLE BLOCK STARTING TIME HAS ALREADY BEEN ELAPSED");
  }

  return result;
} /* cpap_send_receive_normal_burst() */


/*
+------------------------------------------------------------------------------
| Function    : cpap_store_ia_message
+------------------------------------------------------------------------------
| Description : The function cpap_store_ia_message() stores the 1st message of
|               a two message assignment.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void cpap_store_ia_message ( void )
{ 
  MCAST(d_imm_assign,D_IMM_ASSIGN);  /* T_D_IMM_ASSIGN */

  TRACE_FUNCTION( "cpap_store_ia_message" );

  memcpy(&grr_data->cpap.tma_ia, d_imm_assign, sizeof(T_D_IMM_ASSIGN));
  grr_data->cpap.v_tma_ia = TRUE;

} /* cpap_store_ia_message() */


/*
+------------------------------------------------------------------------------
| Function    : cpap_join_tma_messages
+------------------------------------------------------------------------------
| Description : The function cpap_join_tma_messages() join the stored 1st and
|               the 2nd message of a two message assignment.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void cpap_join_tma_messages ( void )
{ 
  MCAST(d_imm_assign,D_IMM_ASSIGN);  /* T_D_IMM_ASSIGN */

  TRACE_FUNCTION( "cpap_join_tma_messages" );

  /* the 2nd message contains only the packet channel description and the 
   * mobile allocation if needed.
   */

  d_imm_assign->tma = TMA_0;
  d_imm_assign->page_mode.pm = grr_data->cpap.tma_ia.page_mode.pm;
  d_imm_assign->time_advance.ta = grr_data->cpap.tma_ia.time_advance.ta;
  if(grr_data->cpap.tma_ia.v_start_time)
  {
    d_imm_assign->v_start_time = TRUE;
    d_imm_assign->start_time.t1 = grr_data->cpap.tma_ia.start_time.t1;
    d_imm_assign->start_time.t2 = grr_data->cpap.tma_ia.start_time.t2;
    d_imm_assign->start_time.t3 = grr_data->cpap.tma_ia.start_time.t3;
  }
  memcpy( &d_imm_assign->ia_rest_oct,
          &grr_data->cpap.tma_ia.ia_rest_oct, 
          sizeof(T_ia_rest_oct)                );
  grr_data->cpap.v_tma_ia = FALSE;

} /* cpap_join_tma_messages() */

/*
+------------------------------------------------------------------------------
| Function    : cpap_malloc_ia
+------------------------------------------------------------------------------
| Description : The function cpap_malloc_ia() .... 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void cpap_malloc_ia ( void )
{ 
  TRACE_FUNCTION( "cpap_malloc_ia" );

  if( grr_data->cpap.p_d_imm_assign EQ NULL )
  {
    MALLOC( grr_data->cpap.p_d_imm_assign, sizeof( T_D_IMM_ASSIGN ) );
  }
  else
  {
    TRACE_ERROR( "cpap_malloc_ia: fatal error, memory reallocation not implemented" );
  }

  *grr_data->cpap.p_d_imm_assign = *( ( T_D_IMM_ASSIGN * )_decodedMsg );

} /* cpap_malloc_ia() */

/*
+------------------------------------------------------------------------------
| Function    : cpap_mfree_ia
+------------------------------------------------------------------------------
| Description : The function cpap_mfree_ia() .... 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void cpap_mfree_ia ( BOOL restore_data )
{ 
  TRACE_FUNCTION( "cpap_mfree_ia" );

  if( grr_data->cpap.p_d_imm_assign NEQ NULL )
  {
    if( restore_data )
    {
      *( ( T_D_IMM_ASSIGN * )_decodedMsg ) = *grr_data->cpap.p_d_imm_assign;
    }

    MFREE( grr_data->cpap.p_d_imm_assign );
    grr_data->cpap.p_d_imm_assign = NULL;
  }
  else
  {
    TRACE_ERROR( "cpap_mfree_ia: fatal error, no memory allocated" );
  }
} /* cpap_mfree_ia() */

/*
+------------------------------------------------------------------------------
| Function    : cpap_check_and_save_freq
+------------------------------------------------------------------------------
| Description : This function check if the freq_par is valid or not..
|               If it is hopping case, save the hoppinging freq. 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
LOCAL BOOL cpap_check_and_save_freq (void)
{
  MCAST(d_imm_assign,D_IMM_ASSIGN); /* T_D_IMM_ASSIGN */
  T_pck_chan_desc *chan_desc = &d_imm_assign->pck_chan_desc;
  T_mob_alloc *mob_alloc = &d_imm_assign->mob_alloc;
  BOOL result = FALSE;
  T_freq_par   freq_par;
  TRACE_FUNCTION( "cpap_check_and_save_freq" );

  if(chan_desc->hop OR (!chan_desc->hop AND chan_desc->indir))
  { /* hopping*/

    if (chan_desc->indir)
    { 
      /* indirect encoding */
      /* set up tc's freq_par struct */
      memset(&freq_par,0,sizeof(T_freq_par));
      freq_par.v_indi_encod      = TRUE;
      freq_par.indi_encod.maio   = chan_desc->maio;
      freq_par.tsc               = chan_desc->tsc;
      freq_par.indi_encod.ma_num = 
                          chan_desc->ma_num + MA_NUMBER_4_PSI13_OR_CELL_ALLOC;
      if( chan_desc->v_ch_mark1)
      {
        freq_par.indi_encod.v_chamge_ma_sub   = TRUE;
        freq_par.indi_encod.chamge_ma_sub.cm1 = chan_desc->ch_mark1;
      }

      result = tc_set_hopping_par( &freq_par );
    }
    else if( chan_desc->v_maio AND chan_desc->v_hsn )
    { /* direct encoding  */ 

      UBYTE i;
      memset(&freq_par,0,sizeof(T_freq_par));
      freq_par.v_di_encod1       = TRUE;
      freq_par.di_encod1.maio    = chan_desc->maio;
      freq_par.tsc               = chan_desc->tsc;
      freq_par.di_encod1.gprs_ms_alloc_ie.hsn = chan_desc->hsn;

      if(0 NEQ mob_alloc->c_mac)
      {
        freq_par.di_encod1.gprs_ms_alloc_ie.v_ma_struct = TRUE;
        freq_par.di_encod1.gprs_ms_alloc_ie.ma_struct.c_ma_map 
                                                         = mob_alloc->c_mac*8;
        freq_par.di_encod1.gprs_ms_alloc_ie.ma_struct.ma_len =
                                                       (mob_alloc->c_mac*8)-1;
        for(i = 0;i < (mob_alloc->c_mac*8);i++)
        { /* convert bitbuffer type of rr to type of grr */
          freq_par.di_encod1.gprs_ms_alloc_ie.ma_struct.ma_map[i]
                                      = (mob_alloc->mac[i/8]>>(7-(i%8)))&0x01;
        }
      }

      result = tc_set_hopping_par( &freq_par );
    }
    else
    { /* corrupted message decoding  */
        grr_data->tc.v_freq_set = FALSE;
        result = FALSE;
    }

  }
  else if(chan_desc->v_arfcn)
  {
    /* absolut radio frequency channel number */
    grr_data->tc.v_freq_set = TRUE;
    result                  = TRUE;
    grr_data->tc.freq_set.freq_par.p_chan_sel.hopping      = FALSE;
    grr_data->tc.freq_set.tsc        = chan_desc->tsc;
    grr_data->tc.freq_set.freq_par.p_chan_sel.p_rf_ch.arfcn  = grr_g23_arfcn_to_l1(chan_desc->arfcn);    
    /* set up tc's freq_par struct */
    memset(&grr_data->tc.freq_set.freq_par.p_freq_list.p_rf_chan_no.p_radio_freq,0,sizeof(grr_data->tc.freq_set.freq_par.p_freq_list.p_rf_chan_no.p_radio_freq));
    grr_data->tc.freq_set.freq_par.p_freq_list.p_rf_chan_cnt = 0;
  }
  else
  {
    grr_data->tc.v_freq_set = FALSE;
    result = FALSE;
  }

  grr_data->pwr_ctrl_valid_flags.v_glbl_pwr_ctrl_param = TRUE;
  grr_data->pwr_ctrl_valid_flags.v_freq_param          = TRUE;

  return result;
}  /* cpap_check_and_save_freq */





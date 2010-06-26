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
|  Purpose :  This module implements local functions for service TC of
|             entity GRLC.
+-----------------------------------------------------------------------------
*/

#ifndef GRLC_TMF_C
#define GRLC_TMF_C
#endif

#define ENTITY_GRLC

/*==== INCLUDES =============================================================*/

#include <stdio.h>
#include <string.h>

#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "ccdapi.h"     /* to get CCD API */
#include "cnf_grlc.h"    /* to get cnf-definitions */
#include "mon_grlc.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "message.h"    /* to get air message definitions */
#include "grlc.h"        /* to get the global entity definitions */

#include "grlc_f.h"      /* to get Process GRLC global function definitions */
#include "grlc_tmf.h"    /* to get Service TM global function definitions  */
#include "grlc_tms.h"    /* to get Service TM inter signal definitions  */
#include "grlc_rus.h"    /* to get interface to service RU */
#include "grlc_rds.h"    /* to get interface to service RD */
#include "grlc_gffs.h"
#include "grlc_meass.h"
#include "grlc_tpcs.h"
#include "cl_rlcmac.h"

/*==== CONST ================================================================*/
/*
 * used in tm_compare_prim
 */
#define SAME_RLC_MODE       0
#define DIFFERENT_RLC_MODE  1

#define PRIO_LOWER          0
#define PRIO_SAME           1
#define PRIO_HIGHER         2

#define THROUGHPUT_LOWER    0
#define THROUGHPUT_SAME     1
#define THROUGHPUT_HIGHER   2

#ifdef REL99
#define SAME_PFI            0
#define DIFF_PFI            1
#endif




/*==== DIAGNOSTICS ==========================================================*/

/*==== LOCAL VARS ===========================================================*/


/*==== PRIVATE FUNCTIONS ====================================================*/


LOCAL void tm_close_gaps_in_ctrl_blk_seq( UBYTE index );
/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : tm_access_allowed
+------------------------------------------------------------------------------
| Description : The function tm_access_allowed() this function checks wheather
|               access is allowed or not. The return is TRUE if access
|               is allowed.
|
| Parameters  : radio_prrio - radio priority for current primitive
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL tm_access_allowed (  UBYTE  radio_prio  )
{
  BOOL result = FALSE;
  TRACE_FUNCTION( "tm_access_allowed" );

  switch(grlc_data->uplink_tbf.ac_class)
  {
    case CGRLC_PCCCH_AC_ALLOWED:
      result = TRUE;
      break;
    case CGRLC_PCCCH_AC_NOT_ALLOWED:
      TRACE_EVENT(" grlc ACCESS NOT ALLOWED PBCCH:");
      break;
    case CGRLC_CCCH_AC_NOT_ALLOWED:
      TRACE_EVENT("grlc ACCESS NOT ALLOWED CCCH");
      break;
    default:
      if ( (radio_prio+3) <= grlc_data->uplink_tbf.ac_class)
        result = TRUE;
      else
        TRACE_EVENT_P2("Radio prio to low : pdu_rp=%d  net_rp=%d",radio_prio,grlc_data->uplink_tbf.ac_class);
      break;
  }


  return(result);
} /* tm_access_allowed() */

/*
+------------------------------------------------------------------------------
| Function    : tm_build_chan_req_des
+------------------------------------------------------------------------------
| Description : The function builds a channel request description. It is called
|               during re-allocation or during uplink allocation on
|               a dowlink TBF.
|
| Parameters  : out_i - Function have to write the channel request description
|                       at this address
|               p_ptr_i - Pointer to primitive which causes the channel request
|                       description
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_build_chan_req_des ( T_chan_req_des * out_i,T_PRIM_QUEUE * p_ptr_i )
{

  UBYTE   next;

  TRACE_FUNCTION( "tm_build_chan_req_des" );

  grlc_data->uplink_tbf.ti  = 0; /* no contention resolution needed */
  out_i->peak_thr_class     = p_ptr_i->prim_ptr->grlc_qos.peak;
  out_i->radio_prio         = p_ptr_i->prim_ptr->radio_prio;

  /*
   * set RLC-Mode and LLC-Mode
   */
  out_i->llc_pdu_type = LLC_NOT_ACK; /* that means not acknowledged */
  if(p_ptr_i->prim_type EQ CGRLC_LLC_PRIM_TYPE_DATA_REQ)
  { /* CGRLC_LLC_PRIM_TYPE_DATA_REQ */
    out_i->rlc_mode = RLC_ACK_MODE;     /* that means RLC mode acknowledged */
  }
  else
  { /* CGRLC_LLC_PRIM_TYPE_UNITDATA_REQ */
    out_i->rlc_mode = RLC_UNACK_MODE;     /* that means RLC mode not acknowledged */
  }

  next = p_ptr_i->next;
  out_i->rlc_octet_cnt = p_ptr_i->prim_ptr->sdu.l_buf/8 +1;
  while (next < PRIM_QUEUE_SIZE_TOTAL)
  {
    out_i->rlc_octet_cnt +=
      grlc_data->prim_queue[next].prim_ptr->sdu.l_buf/8 +1;
    next  = grlc_data->prim_queue[next].next;
  }

  if (grlc_data->testmode.mode EQ CGRLC_LOOP)
  {
    out_i->rlc_octet_cnt = 0;
    TRACE_EVENT("open-ended tbf for testmode B requested");
  }
  return;

} /* tm_build_chan_req_des */



/*
+------------------------------------------------------------------------------
| Function    : tm_send_tbf_rel
+------------------------------------------------------------------------------
| Description : The function tm_send_tbf_rel() builds CGRLC_TBF_REL_IND
|               and send it.
|
| Parameters  : TBP-Type - that have to be deleted
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_send_tbf_rel ( T_TBF_TYPE tbf_i )
{
  PALLOC(cgrlc_tbf_rel_ind,CGRLC_TBF_REL_IND);

  TRACE_FUNCTION( "tm_send_tbf_rel" );


  if(grlc_data->uplink_tbf.ti)
    cgrlc_tbf_rel_ind->tbf_rel_cause = CGRLC_TBF_REL_CR_FAILED;
  else if (grlc_data->N3102 EQ 0) /* tbf error with cell reselction*/
    cgrlc_tbf_rel_ind->tbf_rel_cause = CGRLC_TBF_REL_WITH_CELL_RESELECT;
  else
    cgrlc_tbf_rel_ind->tbf_rel_cause = CGRLC_TBF_REL_NORMAL;  /* Find condition for abnormal release */

  grlc_data->uplink_tbf.ti = 0;

  /*
   * indicate if testmode or tbf_type is released
   */

  if((tbf_i NEQ TBF_TYPE_DL)  AND  /* Testmode released only in case of uplink tbf release*/
     (grlc_test_mode_active()))
  {
    if (grlc_data->testmode.mode EQ CGRLC_TEST_RANDOM)
    {
      MFREE(grlc_data->testmode.ptr_test_data);
      grlc_data->testmode.ptr_test_data = NULL;
    }
    grlc_data->testmode.mode = CGRLC_TEST_MODE_RELEASE;
    cgrlc_tbf_rel_ind->tbf_rel_cause = CGRLC_TBF_REL_NORMAL;
  }

#if defined REL99 AND defined TI_PS_FF_TI_PS_FF_TBF_EST_PACCH
  if (tbf_i EQ TBF_TYPE_TP_ACCESS AND
      grlc_data->tm.pacch_prr_pca_sent) /*PRR/PCA sent as POLL (tbf on pacch)*/
  {
    vsi_t_stop(GRLC_handle,T3168);
    cgrlc_tbf_rel_ind->tbf_mode = CGRLC_TBF_MODE_UL;
    grlc_data->tm.pacch_prr_pca_sent = FALSE;
    sig_tm_gff_ul_deactivate();
  }
  else
#endif
  if (tbf_i EQ TBF_TYPE_UL)
  {
    cgrlc_tbf_rel_ind->tbf_mode  = CGRLC_TBF_MODE_UL;
    grlc_data->rel_type         |= REL_TYPE_UL;
    sig_tm_gff_ul_deactivate();
  }
  else if (tbf_i EQ TBF_TYPE_DL)
  {
    cgrlc_tbf_rel_ind->tbf_mode  = CGRLC_TBF_MODE_DL;
    grlc_data->rel_type         |= REL_TYPE_DL;
    sig_tm_gff_dl_deactivate();
    cgrlc_tbf_rel_ind->dl_trans_id = grlc_data->downlink_tbf.trans_id;
  }
  else
  {
    cgrlc_tbf_rel_ind->tbf_mode = CGRLC_TBF_MODE_DL_UL;
    grlc_data->rel_type         = REL_TYPE_DL_UL;
    cgrlc_tbf_rel_ind->dl_trans_id = grlc_data->downlink_tbf.trans_id;
    sig_tm_gff_ul_deactivate();
    sig_tm_gff_dl_deactivate();
  }


  /*
   *  \   tbf_i | UL DL CONC
   *    \       |
   *      \     |
   *        \   |
   * tbf_type \ |
   * -----------+-----------
   *     NULL   | x  x   x
   *       UL   | x  -   x
   *       DL   | -  x   x
   *     CONC   | -  -   x
   *
   * x means, that no more TBF is existing
   * - means, that a TBF is still existing
   */

  if( tbf_i               EQ TBF_TYPE_CONC OR
      grlc_data->tbf_type EQ TBF_TYPE_NULL OR
      grlc_data->tbf_type EQ tbf_i            )
  {
    cgrlc_tbf_rel_ind->v_c_value = TRUE;

    meas_grlc_c_get_c_value( &cgrlc_tbf_rel_ind->c_value );
    meas_c_restart( );

  }
  else
  {
    cgrlc_tbf_rel_ind->v_c_value = FALSE;
  }

#if defined (_TARGET_)
  switch(cgrlc_tbf_rel_ind->tbf_mode)
  {
    case CGRLC_TBF_MODE_UL:
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
    case CGRLC_TBF_MODE_2PA:
#endif

      TRACE_EVENT_P4("UL_TBF_REL:nr_of_crc_errors= %ld  ul_call_errors=%ld  dl_call_errors=%ld  rel_type=%d",
                                            grlc_data->nr_of_crc_errors,
                                            grlc_data->ul_call_errors,
                                            grlc_data->dl_call_errors,
                                            grlc_data->rel_type);
      break;
    case CGRLC_TBF_MODE_DL:
      TRACE_EVENT_P4("DL_TBF_REL:nr_of_crc_errors= %ld  ul_call_errors=%ld  dl_call_errors=%ld  rel_type=%d",
                                            grlc_data->nr_of_crc_errors,
                                            grlc_data->ul_call_errors,
                                            grlc_data->dl_call_errors,
                                            grlc_data->rel_type);
      break;
    case CGRLC_TBF_MODE_DL_UL:
      TRACE_EVENT_P4("UL/DL_TBF_REL:nr_of_crc_errors= %ld  ul_call_errors=%ld  dl_call_errors=%ld  rel_type=%d",
                                            grlc_data->nr_of_crc_errors,
                                            grlc_data->ul_call_errors,
                                            grlc_data->dl_call_errors,
                                            grlc_data->rel_type);

      break;
    case CGRLC_TBF_MODE_TMA:
    case CGRLC_TBF_MODE_TMB:
      TRACE_EVENT_P3 ("Testmode TBF_REL:nr_of_crc_errors= %ld  ul_call_errors=%ld  dl_call_errors=%ld",
                                            grlc_data->nr_of_crc_errors,
                                            grlc_data->ul_call_errors,
                                            grlc_data->dl_call_errors);

      break;

  }
  if(grlc_data->ul_call_errors OR grlc_data->dl_call_errors)
    {
      TRACE_EVENT_P6("ul_fn_err=%ld,%ld,%ld//dl_fn_err=%ld,%ld,%ld",
                                                                    grlc_data->ul_fn_errors[0],
                                                                    grlc_data->ul_fn_errors[1],
                                                                    grlc_data->ul_fn_errors[2],
                                                                    grlc_data->dl_fn_errors[0],
                                                                    grlc_data->dl_fn_errors[1],
                                                                    grlc_data->dl_fn_errors[2]);
      grlc_data->ul_call_errors   = 0;
      grlc_data->dl_call_errors   = 0;
    }
    grlc_data->nr_of_crc_errors = 0;
#endif /* defined (_TARGET_) */

  PSEND(hCommGRR,cgrlc_tbf_rel_ind);

} /* tm_send_tbf_rel() */



/*
+------------------------------------------------------------------------------
| Function    : tm_abort_tbf
+------------------------------------------------------------------------------
| Description : The function tm_abort_tbf() stops a TBF.
|
| Parameters  : tbf_i - TBF type to abort
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_abort_tbf ( T_TBF_TYPE tbf_i )
{
 /*SZML-TC/056*/

  TRACE_FUNCTION( "tm_abort_tbf" );

  switch( tbf_i )
  {
    case TBF_TYPE_NULL:
      TRACE_EVENT("NULL TBF active: check if tbf starting time is running");
      /*SZML-TC/093*/
      break;
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
    case TBF_TYPE_TP_ACCESS:
      tm_send_tbf_rel(tbf_i);
      tm_deactivate_tbf(tbf_i);
      break;
#endif
    case TBF_TYPE_UL:
      tm_send_tbf_rel(tbf_i);
      sig_tm_ru_abrel(1, FALSE);
      tm_deactivate_tbf(tbf_i);
      break;
    case TBF_TYPE_DL:
      tm_send_tbf_rel(tbf_i);
      sig_tm_rd_abrel(1,FALSE);
      tm_deactivate_tbf(tbf_i);
      break;
    case TBF_TYPE_CONC:
      tm_send_tbf_rel(tbf_i);
      sig_tm_ru_abrel(1, FALSE);
      sig_tm_rd_abrel(1,FALSE);
      tm_deactivate_tbf(tbf_i);
      break;
    default:
      TRACE_ERROR ( "tm_abort_tbf: TBF type is invalid" );
      break;
  }
} /* tm_abort_tbf() */



/*
+------------------------------------------------------------------------------
| Function    : tm_ini_realloc
+------------------------------------------------------------------------------
| Description : The function tm_ini_realloc() do all action that are necessary
|               to start a Resource Re-Allocation Procedure.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_ini_realloc ( UBYTE start_of_new_tbf_i )
{
  TRACE_FUNCTION( "tm_ini_realloc" );
  grlc_data->tm.start_of_new_tbf = start_of_new_tbf_i ;

 /*SZML-TC/058*/

} /* tm_ini_realloc() */



/*
+------------------------------------------------------------------------------
| Function    : tm_build_res_req
+------------------------------------------------------------------------------
| Description : The function tm_build_res_req() builds Packet Resource Request.
|
| Parameters  : reason_i   - the reason for building that packet resouce
|                            reallocation
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_build_res_req (T_U_GRLC_RESOURCE_REQ *ptr2res_req,
                              T_REASON_BUILD    reason_i)
{
  TRACE_FUNCTION( "tm_build_res_req" );


  memset(ptr2res_req,0,sizeof(T_U_GRLC_RESOURCE_REQ) );


  /* processing of messsage type */
  ptr2res_req->msg_type =  U_GRLC_RESOURCE_REQ_c;

  /* processing of change mark */

  if( grlc_data->tm.change_mark EQ NOT_SET )
  {
    ptr2res_req->v_ma_ch_mark = 0;
  }
  else
  {
    ptr2res_req->v_ma_ch_mark = 1;
    ptr2res_req->ma_ch_mark   = grlc_data->tm.change_mark;
  }

  if(reason_i EQ R_FIX_RE_ALLOC)
  {
    /* 1 - processing of ACCESS_TYPE */
    ptr2res_req->v_access_type     = 0;

    /* 2 - processing of global TFI and TLLI */
    ptr2res_req->flag              = 0;
    ptr2res_req->v_glob_tfi        = 1;
    ptr2res_req->glob_tfi.flag     = 0;
    ptr2res_req->glob_tfi.v_ul_tfi = 1;
    ptr2res_req->glob_tfi.ul_tfi   = grlc_data->ul_tfi;

    /* 3 - processing of radio access capabilities */
    ptr2res_req->v_ra_cap          = FALSE;

    /* 4 - processing of channel request description */
    tm_build_chan_req_des(&ptr2res_req->chan_req_des,
                          &grlc_data->prim_queue[grlc_data->prim_start_tbf]);
  }
  else if(reason_i EQ R_RE_ALLOC)
  {
    /* 1 - processing of ACCESS_TYPE */
    ptr2res_req->v_access_type = 0;

    /* 2 - processing of global TFI and TLLI */
    ptr2res_req->flag          = 1;
    ptr2res_req->v_glob_tfi    = 0;
    ptr2res_req->v_tlli_value  = 1;

    grlc_set_buf_tlli( &ptr2res_req->tlli_value, grlc_data->uplink_tbf.tlli );

    /* 3 - processing of radio access capabilities */
    ptr2res_req->v_ra_cap = FALSE;

    /* 4 - processing of channel request description */
    tm_build_chan_req_des(&ptr2res_req->chan_req_des,
                          &grlc_data->prim_queue[grlc_data->tm.start_of_new_tbf]);
  }
  else if(reason_i EQ R_BUILD_2PHASE_ACCESS)
  {
    /* 1 - processing of ACCESS_TYPE */
    ptr2res_req->v_access_type = 1;
    ptr2res_req->access_type   = TWO_PHASE;

    /* 2 - processing of global TFI and TLLI */
    ptr2res_req->flag          = 1;
    ptr2res_req->v_glob_tfi    = 0;
    ptr2res_req->v_tlli_value  = 1;

    grlc_set_buf_tlli( &ptr2res_req->tlli_value, grlc_data->uplink_tbf.tlli );

    /* 3 - processing of radio access capabilities */
    if( rr_csf_get_radio_access_capability( &ptr2res_req->ra_cap ) EQ 0 )
    {
      ptr2res_req->v_ra_cap = TRUE;
    }
    else
    {
      ptr2res_req->v_ra_cap = FALSE;

      TRACE_ERROR( "tm_build_res_req: radio access capabilities invalid" );
    }

    /* 4 - processing of channel request description */
    tm_build_chan_req_des((T_chan_req_des *)(&ptr2res_req->chan_req_des),
                          &grlc_data->prim_queue[grlc_data->prim_start_tbf] );
  }

  /* 5 - processing of signal variance */
  ptr2res_req->signvar   = meas_sv_get_value( );
  ptr2res_req->v_signvar = TRUE;

  /* 6 - processing of relative interference levels */
  meas_int_get_rel_i_level( &ptr2res_req->ilev );

  /* 7 - processing of C value */
  ptr2res_req->c_value = meas_grlc_c_get_value( );

#ifdef REL99
  ptr2res_req->v_release_99_str_grlc_prr              = TRUE;

  /* Workaround to avoid definition of EGPRS measurements info
   * in grr.aim file. This will reduce the size of ccddata.lib
   */
  ptr2res_req->release_99_str_grlc_prr.flag           = 0;
  ptr2res_req->release_99_str_grlc_prr.flag2          = 0;

  /* Flag PFI, shall only include if PFI and BSS R99 */
  if (grlc_data->nw_rel AND grlc_data->pfi_support)
  {
    ptr2res_req->release_99_str_grlc_prr.v_pfi = 1;

    if ( reason_i EQ R_RE_ALLOC )
    {
      ptr2res_req->release_99_str_grlc_prr.pfi =
        grlc_data->prim_queue[grlc_data->tm.start_of_new_tbf].prim_ptr->pkt_flow_id[0];
    }
    else
    {
      ptr2res_req->release_99_str_grlc_prr.pfi = grlc_data->pfi_value;
    }
  }
  else
  {
    ptr2res_req->release_99_str_grlc_prr.v_pfi = 0;
  }

  ptr2res_req->release_99_str_grlc_prr.add_ms_rac     = 0;
  ptr2res_req->release_99_str_grlc_prr.retrans_of_prr = 0;
#endif


} /* tm_build_res_req() */










/*
+------------------------------------------------------------------------------
| Function    : tm_init_prim
+------------------------------------------------------------------------------
| Description : The function tm_init_prim() initializes the primitive queue.

| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_init_prim ( void )
{
  UBYTE  i;
  TRACE_FUNCTION( "tm_init_prim" );


  /* LLC PRIM QUEUE*/
  for (i=0;i<PRIM_QUEUE_SIZE;i++)
  {
    /*
     *   set next to the following entry
     */
    grlc_data->prim_queue[i].next = i+1;

    /*
     *  and initialize the not used primitive entry
     */
    grlc_data->prim_queue[i].prim_ptr      = NULL;
    grlc_data->prim_queue[i].prim_type     = CGRLC_LLC_PRIM_TYPE_NULL;
    grlc_data->prim_queue[i].cv_status     = FALSE;
    grlc_data->prim_queue[i].rlc_status    = FALSE;
    grlc_data->prim_queue[i].re_allocation = FALSE;
    grlc_data->prim_queue[i].start_new_tbf = FALSE;
    grlc_data->prim_queue[i].last_bsn      = 0xff;
  }


  /*
   * last free entry points to 0xff
   */
  grlc_data->prim_queue[PRIM_QUEUE_SIZE-1].next = 0xff;

  /*
   *   index 0 is the first free entry
   */
  grlc_data->prim_start_free = 0;

  /*
   * becauce there are no primitives in the tbf prim_start_tbf points to 0xff
   */
  grlc_data->prim_start_tbf = 0xff;

  /*
   * init because there are no primitives there is also no user data
   */
  grlc_data->prim_user_data = 0;


  /* init gmm prim queue*/

  for (i=PRIM_QUEUE_SIZE; i<(PRIM_QUEUE_SIZE_TOTAL);i++)
  {
    grlc_data->prim_queue[i].next = i+1;

    grlc_data->prim_queue[i].prim_ptr      = NULL;
    grlc_data->prim_queue[i].prim_type     = CGRLC_LLC_PRIM_TYPE_NULL;
    grlc_data->prim_queue[i].cv_status     = FALSE;
    grlc_data->prim_queue[i].rlc_status    = FALSE;
    grlc_data->prim_queue[i].re_allocation = FALSE;
    grlc_data->prim_queue[i].start_new_tbf = FALSE;
    grlc_data->prim_queue[i].last_bsn      = 0xff;
  }
  grlc_data->gmm_procedure_is_running = FALSE;
  grlc_data->prim_queue[PRIM_QUEUE_SIZE_TOTAL-1].next = 0xff;



} /* tm_init_prim() */




/*
+------------------------------------------------------------------------------
| Function    : tm_start_access
+------------------------------------------------------------------------------
| Description : The function tm_start_access() prepares the TC data structure
|               for a packet access procedure.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_start_access ( void )
{
  USHORT rlc_octetts;

  TRACE_FUNCTION( "tm_start_access" );

  /*
   *  mark that first packet access in process
   */

  grlc_data->tm.n_res_req  = 0;

#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
  grlc_data->tm.pacch_prr_pca_sent = FALSE;
#endif


  grlc_data->tm.n_acc_req_procedures++;

  if(grlc_data->prim_start_tbf < PRIM_QUEUE_SIZE_TOTAL)
  {
    grlc_data->uplink_tbf.prio
         = grlc_data->prim_queue[grlc_data->prim_start_tbf].prim_ptr->radio_prio;
    /*  save prio for packet access request building     */
    rlc_octetts =
       grlc_data->prim_queue[grlc_data->prim_start_tbf].prim_ptr->sdu.l_buf/8 +1;
#ifdef REL99
    if (grlc_data->pfi_support)
    {
      /* pfi will take 1 byte*/
      grlc_data->uplink_tbf.nr_blocks = ( rlc_octetts / 15 ) + 1;
      grlc_data->pfi_value =
        grlc_data->prim_queue[grlc_data->prim_start_tbf].prim_ptr->pkt_flow_id[0];
    }
    else
#endif
    { /* last "+1" we need complete bytes/octetts   */
      grlc_data->uplink_tbf.nr_blocks = ( rlc_octetts / 16 ) + 1;
    }

    /*TRACE_EVENT_P1("tm_start_access: nr_block:  %d", grlc_data->uplink_tbf.nr_blocks);*/
    /* 16 because 23(CS_1 length) -3(blockheader) -4(TLLI) = 16 octetts   */
    /* last "+1" we need complete blocks                                  */
    if(grlc_data->prim_queue[grlc_data->prim_start_tbf].prim_type EQ CGRLC_LLC_PRIM_TYPE_UNITDATA_REQ )
    {
      grlc_data->uplink_tbf.rlc_mode = CGRLC_RLC_MODE_UACK;
      grlc_data->uplink_tbf.ti = 0;  /*  mark that contention resolution is not requested    */
    }
    else
    {
      grlc_data->uplink_tbf.rlc_mode = CGRLC_RLC_MODE_ACK;
      grlc_data->uplink_tbf.ti = 1;     /*  mark that contention resulution is not yet done    */
    }
    /*
     *  estimate access_type
     */
    if(grlc_data->prim_queue[grlc_data->prim_start_tbf].prim_type EQ CGRLC_LLC_PRIM_TYPE_UNITDATA_REQ )
    {
      grlc_data->uplink_tbf.access_type = CGRLC_AT_TWO_PHASE;
    }
    else if(grlc_data->uplink_tbf.access_type EQ CGRLC_AT_CELL_UPDATE)
    {
      TRACE_EVENT("CU WILL BE PERFORMED");
    }
    else
    {
      switch(grlc_data->prim_queue[grlc_data->prim_start_tbf].prim_ptr->cause)
      {
        case GRLC_DTACS_DEF:
          if(grlc_data->uplink_tbf.nr_blocks <= 8)
          {
            grlc_data->uplink_tbf.access_type = CGRLC_AT_SHORT_ACCESS;
          }
          else
          {
            grlc_data->uplink_tbf.access_type = CGRLC_AT_ONE_PHASE;
          }
          break;

        case GRLC_DTACS_PAGE_RESPONSE:
          grlc_data->uplink_tbf.access_type = CGRLC_AT_PAGE_RESPONSE;
          break;

        case GRLC_DTACS_MOBILITY_MANAGEMENT:
          grlc_data->uplink_tbf.access_type = CGRLC_AT_MM_PROCEDURE;
          break;
        case GRLC_DTACS_EMPTY_FRAME:
        case GRLC_DTACS_CELL_NOTIFI_NULL_FRAME:
          grlc_data->uplink_tbf.access_type = CGRLC_AT_CELL_UPDATE;
          TRACE_EVENT("EMPTY FRAME for CU, but not requested by GMM");
          break;

        default:
          grlc_data->uplink_tbf.access_type = CGRLC_AT_NULL;
          break;
      }
    }
  }
  else
  {
    TRACE_ERROR("NO PRIM IN QUEUE in tm_start_access");
  }


} /* tm_start_access() */



/*
+------------------------------------------------------------------------------
| Function    : tm_data_req
+------------------------------------------------------------------------------
| Description : The function tm_data_req() starts the process of queueing
|               a primitive received from LLC
|
| Parameters  : prime_tpye_i - primitiven type (PRIME_TYPE_UNITDATA or
|                                                     PRIME_TYPE_DATA)
|               ptr2prim_i   - address of the primitive to handle
|
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_data_req ( T_PRIM_TYPE prim_tpye_i,
                          T_GRLC_DATA_REQ * ptr2prim_i)
{
  UBYTE free_entry_count , position , new_prim ;
  T_COMPARE_PRIM compare_result  = C_PRIM_CONTINUE;
  BOOL new_qos = FALSE;

  TRACE_FUNCTION( "tm_data_req" );


  /*
   *  get the index of next free prim queue entry
   */
  free_entry_count = tm_prim_queue_get_free_count();

  if (free_entry_count)
  {
    /* get first free entry from free list */
    new_prim  = grlc_prim_get_first (&grlc_data->prim_start_free);

#ifdef REL99
    /*SM currenlty give 0xFF , in case network does not assign PFI */
    if (ptr2prim_i->pkt_flow_id[0] EQ 0xFF)
    {
      ptr2prim_i->pkt_flow_id[0] = 0;
    }
#endif

    grlc_data->prim_queue[new_prim ].prim_type = prim_tpye_i;
    grlc_data->prim_queue[new_prim ].prim_ptr = ptr2prim_i;

    position  = grlc_data->prim_start_tbf;

    do
    {
      if(position  < PRIM_QUEUE_SIZE_TOTAL) /*  this "if" make sure only valid llc_pdu
                                                 are used in comparision */
      {

        compare_result  = tm_compare_prim(new_prim ,position ,&new_qos);

        /*
         * position for new prim is found !
         */
        if(compare_result  NEQ C_PRIM_CONTINUE) break;

        position  = grlc_data->prim_queue[position ].next;

      }
      else
      { /* first prim in queue */
        compare_result  = C_PRIM_NEW_TBF;
        break;
      }

    } while (1);


    switch( compare_result  )
    {
      case C_PRIM_NEW_TBF:
        grlc_data->prim_queue[new_prim ].start_new_tbf = 1;
        break;
      case C_PRIM_REALLOC_START:
        {
          T_U_GRLC_RESOURCE_REQ resource_req;/*lint !e813*/

          tm_ini_realloc(new_prim);
          tm_build_res_req( &resource_req,
                            R_RE_ALLOC );
		  tm_store_ctrl_blk( CGRLC_BLK_OWNER_TM, ( void* )&resource_req );
          if (grlc_data->prim_queue[position].next NEQ 0xff)/*sec 8.1.1.1.2 para 10*/
          { 
            grlc_data->prim_queue[grlc_data->prim_queue[position].next].re_allocation = 1;
          }
       }
        break;
      case C_PRIM_REALLOC_SHORT:
        {
          T_U_GRLC_RESOURCE_REQ resource_req; /*lint !e813*/

          tm_ini_realloc(new_prim);
          tm_build_res_req( &resource_req,
                            R_FIX_RE_ALLOC );
          tm_store_ctrl_blk( CGRLC_BLK_OWNER_TM, ( void* )&resource_req );
        }
        break;

      case C_PRIM_REALLOC_MARK:
        grlc_data->prim_queue[new_prim ].re_allocation = 1;
        break;
      case C_PRIM_RLC_MODE_CHANGE:  /*sec 8.1.1.1.2 para 5*/
        grlc_data->prim_queue[new_prim].start_new_tbf = 1;
        grlc_data->prim_queue[grlc_data->prim_queue[position].next].start_new_tbf = 1;
        break;

    }


    if(position < PRIM_QUEUE_SIZE_TOTAL)
    {
      position  = grlc_data->prim_queue[position].next;
    }
    /*This new QOS is for lower priority */
    if ( (position EQ  0xff) AND (new_qos EQ TRUE))
    {
      grlc_data->prim_queue[new_prim].re_allocation = 1;/*8.1.1.1.2 para 10*/
      TRACE_EVENT_P2("reallocation set for llc pdu = %d ,priority =%d",new_prim,ptr2prim_i->radio_prio);
    }

    grlc_prim_put(&grlc_data->prim_start_tbf,new_prim ,position );

    /*
     * update amount of buffered user data
     */
    grlc_data->prim_user_data += BYTELEN(ptr2prim_i->sdu.l_buf);

    /*
     * and check if the queue is now full
     */
    free_entry_count--;

    if( (free_entry_count == 0) || (grlc_data->prim_user_data > grlc_data->tm.max_grlc_user_data ) )
    {
      grlc_data->tm.send_grlc_ready_ind = PRIM_QUEUE_FULL;
    }
  }
  else
  {
    TRACE_ERROR("Data-Request, but prim_queue full");
    /*
     * This is an error of LLC.
     * Therefore here is no special handling of this case
     */
    PFREE(ptr2prim_i);

  }

} /* tm_data_req() */



/*
+------------------------------------------------------------------------------
| Function    : tm_grlc_init
+------------------------------------------------------------------------------
| Description : The function tm_grlc_init() initialize the TC data structure.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_grlc_init ( void )
{
  UBYTE i;

  TRACE_FUNCTION( "tm_grlc_init" );

  /*
   *  set some values
   */
  grlc_data->tm.disable_class         = CGRLC_DISABLE_CLASS_CR;
  grlc_data->tbf_type                 = TBF_TYPE_NULL;
  grlc_data->rel_type                 = REL_TYPE_NULL;
  grlc_data->tm.start_of_new_tbf      = 0xff;
  grlc_data->tm.send_grlc_ready_ind   = SEND_A_GRLC_READY_IND;


  grlc_data->tm.n_acc_req_procedures  = 0;
  grlc_data->tm.n_res_req             = 0;
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
  grlc_data->tm.pacch_prr_pca_sent    = FALSE;
#endif


  /* initialize the relevant data for uplink control block */
  grlc_data->tm.ul_ctrl_blk.seq[0]    = MAX_CTRL_BLK_NUM;

  for( i = 0; i < MAX_CTRL_BLK_NUM; i++ )
  {
    grlc_data->tm.ul_ctrl_blk.blk[i].state = BLK_STATE_NONE;
    grlc_data->tm.ul_ctrl_blk.blk[i].owner = CGRLC_BLK_OWNER_NONE;
  }

  /*
   * Initialise service name (uses define SERVICE_NAME_* in GRLC.H);
   */

  INIT_STATE(TM,TM_ACCESS_DISABLED);
  /*
   *  initialize primitive queue
   */
  tm_init_prim();


} /* tm_grlc_init() */




/*
+------------------------------------------------------------------------------
| Function    : tm_prim_queue_get_free_count
+------------------------------------------------------------------------------
| Description : This function returns the number of free
|               entries in the primitive queue.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE tm_prim_queue_get_free_count ( void )
{
  UBYTE i = grlc_data->prim_start_free;
  UBYTE result = 0,j;

  TRACE_FUNCTION( "tm_prim_queue_get_free_count" );


  for(j=0;j<PRIM_QUEUE_SIZE_TOTAL;j++)
  {
    if (i NEQ 0xff)
    {
      result++;
      i = grlc_data->prim_queue[i].next;
    }
    else
    {
      break;
    }
  }
  if((grlc_data->grlc_data_req_cnt + result NEQ PRIM_QUEUE_SIZE )AND
     !grlc_data->gmm_procedure_is_running)
  {
    TRACE_EVENT_P4("PST=%d PSF=%d PDU=%d FREE_CNT=%d: tm_prim_queue_get_free_count CHECK it"
                                                           ,grlc_data->prim_start_tbf
                                                           ,grlc_data->prim_start_free
                                                           ,grlc_data->grlc_data_req_cnt
                                                           ,result);
  }

  return(result);

} /* tm_prim_queue_get_free_count() */




/*
+------------------------------------------------------------------------------
| Function    : tm_compare_prim
+------------------------------------------------------------------------------
| Description : The function tm_compare_prim() check wheather the pdu have to
|               set at tis position or not. They function also estimate
|               how to set this primitive (with new TBF, immediately
|               re-allocation or re-allocation at next PDU-boundary)
|
| Parameters  : new_prim_i - index of the new-primitive entry in prim_queue[]
|               position_i - index of the preceding entry of the possible
|                            position of the new primitive
+------------------------------------------------------------------------------
*/
GLOBAL T_COMPARE_PRIM tm_compare_prim ( UBYTE new_prim_i,
                                        UBYTE position_i,
                                        BOOL *new_qos)
{
  BOOL rlc_mode;
  UBYTE new_prio       = PRIO_HIGHER;
  UBYTE new_throughput = THROUGHPUT_HIGHER;
#ifdef REL99
  UBYTE new_pfi = SAME_PFI;
#endif
  UBYTE pre_prim_index, post_prim_index, new_prim_index;
  T_GRLC_UNITDATA_REQ * pre_prim_ptr, * new_prim_ptr, * post_prim_ptr;
  T_COMPARE_PRIM result;

  TRACE_FUNCTION( "tm_compare_prim" );


  /*
   *  set all variables
   */

  pre_prim_index =  position_i;
  post_prim_index = grlc_data->prim_queue[position_i].next;
  new_prim_index = new_prim_i;


  /*
   * GRLC_UNITDATA_REQ and T_GRLC_UNITDATA_REQ have the same layout!!
   */
  pre_prim_ptr = (T_GRLC_UNITDATA_REQ *) grlc_data->prim_queue[pre_prim_index].prim_ptr;
  post_prim_ptr = (T_GRLC_UNITDATA_REQ *) grlc_data->prim_queue[post_prim_index].prim_ptr;
  new_prim_ptr = (T_GRLC_UNITDATA_REQ *) grlc_data->prim_queue[new_prim_index].prim_ptr;



  if(post_prim_index NEQ 0xff)
  {
#ifdef REL99
    if (grlc_data->pfi_support AND
        (post_prim_ptr->pkt_flow_id[0] NEQ new_prim_ptr->pkt_flow_id[0])
       )
    {
      new_pfi = DIFF_PFI;
    }
#endif

    /*
     * last position in queue; the values at position 0xff in the prim_queue are invalid
     * therefore they are here not set
     */
    if(post_prim_ptr->radio_prio < new_prim_ptr->radio_prio )
    {
      new_prio = PRIO_LOWER;
    }
    else if(post_prim_ptr->radio_prio EQ new_prim_ptr->radio_prio )
    {
      new_prio = PRIO_SAME;
    }

    if(post_prim_ptr->grlc_qos.peak > new_prim_ptr->grlc_qos.peak )
    {
      new_throughput = THROUGHPUT_LOWER;
    }
    else if(post_prim_ptr->grlc_qos.peak EQ new_prim_ptr->grlc_qos.peak )
    {
      new_throughput = THROUGHPUT_SAME;
    }
  }

/*  TRACE_EVENT_P2("np=%d np=%d",new_prio,new_throughput);*/


  /*
   * evalute - wheather the primitive have to put at this position or not
   * compare with "following primitive"
   */


  if(post_prim_index EQ 0xff)
  { /*
     * because this is the last entry in prim_queue this
     * primitive have to put at this position
     */
    result = C_PRIM_PUT;
  }
  else
  {
    if (grlc_data->prim_queue[post_prim_index].rlc_status)
    { /*
       * it is forbidden to change position of
       * primitives already in transmission
       */
      result = C_PRIM_CONTINUE;
    }
    else if (grlc_data->prim_queue[post_prim_index].cv_status)
    {/*
      * it is forbidden to change position of primitives
      * included in countdown procedure
      */
      result = C_PRIM_CONTINUE;
    }
#ifdef REL99
    else if (new_pfi EQ DIFF_PFI)
    {
      result = C_PRIM_PUT;
    }
#endif
    else if ( new_prio EQ PRIO_HIGHER)
    {
      result = C_PRIM_PUT;
    }
    else if ( (new_prio EQ PRIO_SAME) AND (new_throughput EQ THROUGHPUT_HIGHER) )
    {
      result = C_PRIM_PUT;
    }
    else
    {
      result = C_PRIM_CONTINUE;
    }
  }

  /*
   * evalute - type of "put"
   * compare with "preceding primitive"
   */


  /*TRACE_EVENT_P3("np=%d np=%d result=%d",new_prio,new_throughput,result);*/


  if(result EQ C_PRIM_PUT)
  {

    /*
     * set variables
     */


    if(pre_prim_ptr->radio_prio < new_prim_ptr->radio_prio )
    {
      new_prio = PRIO_LOWER;
      *new_qos = TRUE;
    }
    else if(pre_prim_ptr->radio_prio EQ new_prim_ptr->radio_prio )
    {
      new_prio = PRIO_SAME;
    }
    else
    {
      new_prio = PRIO_HIGHER;
    }

    if(pre_prim_ptr->grlc_qos.peak > new_prim_ptr->grlc_qos.peak )
    {
      new_throughput = THROUGHPUT_LOWER;
    }
    else if(pre_prim_ptr->grlc_qos.peak EQ new_prim_ptr->grlc_qos.peak )
    {
      new_throughput = THROUGHPUT_SAME;
    }
    else
    {
      new_throughput = THROUGHPUT_HIGHER;
    }

#ifdef REL99
    if (grlc_data->pfi_support AND
        (pre_prim_ptr->pkt_flow_id[0] NEQ new_prim_ptr->pkt_flow_id[0])
       )
    {
      new_pfi = DIFF_PFI;
    }
    else
    {
      new_pfi = SAME_PFI;
    }
#endif

    if (  grlc_data->prim_queue[pre_prim_index].prim_type EQ
      grlc_data->prim_queue[new_prim_index].prim_type)
    {
      rlc_mode = SAME_RLC_MODE;
    }
    else
    {
      rlc_mode = DIFFERENT_RLC_MODE;
    }





    /*
     * evalution
     */


   /* TRACE_EVENT_P4("result=%d,rlc_mode=%d,prio=%d,peak=%d",result,rlc_mode,new_prio,new_throughput);*/

    if(rlc_mode EQ DIFFERENT_RLC_MODE)
    {

      if (new_prio EQ PRIO_HIGHER AND post_prim_index NEQ 0xFF)
      {
        result = C_PRIM_RLC_MODE_CHANGE;
      }
      else
      {
        result = C_PRIM_NEW_TBF;
      }
    }
    else if (grlc_data->prim_queue[pre_prim_index].cv_status)
    { /*
       * if count-down-procedure has started for this pdu,
       * re-allocation is forbitten
       */
      result = C_PRIM_NEW_TBF;
    }
#ifdef REL99
    else if ( grlc_data->pfi_support                           AND
              grlc_data->prim_queue[pre_prim_index].rlc_status AND
              new_pfi EQ DIFF_PFI
            )
    {
        result = C_PRIM_REALLOC_START;
        TRACE_EVENT_P3("C_PRIM_REALLOC_START: npfi = %d, rlc_st = %d pre_prim_i=%d"
                                                       ,new_prim_ptr->pkt_flow_id[0]
                                                       ,grlc_data->prim_queue[pre_prim_index].rlc_status
                                                       ,pre_prim_index);

    }
#endif
    else if( (grlc_data->prim_queue[pre_prim_index].rlc_status) AND
              ((new_prio EQ PRIO_HIGHER ) OR
               ((new_throughput EQ THROUGHPUT_HIGHER ) AND (new_prio EQ PRIO_SAME ))
              )
            )
    { /*
       * C_PRIM_REALLOC_START only if the "pre-pdu"
       * is in transmission
       */
      TRACE_EVENT_P4("C_PRIM_REALLOC_START: ntp=%d np=%d rlc_st=%d  pre_prim_i=%d"
                                                            ,new_throughput
                                                            ,new_prio
                                                            ,grlc_data->prim_queue[pre_prim_index].rlc_status
                                                            ,pre_prim_index);
      result = C_PRIM_REALLOC_START;
    }
    else if(
              (new_prio EQ PRIO_LOWER ) OR
               ((new_throughput EQ THROUGHPUT_LOWER ) AND (new_prio EQ PRIO_SAME ))
           )

		{
        result = C_PRIM_REALLOC_MARK;
        TRACE_EVENT_P4("C_PRIM_REALLOC_MARK: ntp=%d np=%d rlc_st=%d  pre_prim_i=%d"
                                                            ,new_throughput
                                                            ,new_prio
                                                            ,grlc_data->prim_queue[pre_prim_index].rlc_status
                                                            ,pre_prim_index);
    }
    else if (grlc_data->uplink_tbf.ac_class >= CGRLC_PCCCH_AC_NOT_ALLOWED   AND  /* if pbcch is present */
             grlc_data->uplink_tbf.access_type EQ CGRLC_AT_SHORT_ACCESS     AND  /* last acess type */
             grlc_data->tbf_type NEQ TBF_TYPE_DL)                                /* not required if acess is over existing downlink tbf*/
    {
			/* if prevoious access type is short access, ms should send packet resource request*/
        result = C_PRIM_REALLOC_SHORT;
        grlc_data->uplink_tbf.access_type = CGRLC_AT_ONE_PHASE;
	      TRACE_EVENT_P4("C_PRIM_REALLOC_SHORT: ntp=%d np=%d rlc_st=%d  pre_prim_i=%d"
                                                            ,new_throughput
                                                            ,new_prio
                                                            ,grlc_data->prim_queue[pre_prim_index].rlc_status
                                                            ,pre_prim_index);

    }
  }

 /* TRACE_EVENT_P4("np=%d np=%d  rlc_mode=%d result=%d",new_prio,new_throughput,rlc_mode,result);*/


  return(result);

} /* tm_compare_prim() */



/*
+------------------------------------------------------------------------------
| Function    : tm_send_grlc_ready_ind
+------------------------------------------------------------------------------
| Description : This function sends the GRLC_READY_IND
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
LOCAL void tm_send_grlc_ready_ind ( void )
{
  if(grlc_data->gmm_procedure_is_running)
  {
    PALLOC(grlc_suspend_ready_ind, GRLC_SUSPEND_READY_IND);
    PSEND(hCommLLC, grlc_suspend_ready_ind);
  }
  else
  {
    PALLOC(grlc_ready_ind, GRLC_READY_IND);
    PSEND(hCommLLC, grlc_ready_ind);
  }
} /* tm_send_grlc_ready_ind() */

/*
+------------------------------------------------------------------------------
| Function    : tm_handle_grlc_ready_ind
+------------------------------------------------------------------------------
| Description : This function handles the sending of the GRLC_READY_IND. Call
|               to this function in any of the following cases:
|
|               - a GRLC_DATA_REQ was received
|               - a GRLC_UNITDATA_REQ was received
|               - a queued prim is deleted
|               - the TM state is changed from TM_ACCESS_DISABLED or
|                 TM_ACCESS_PREPARED to another
|
| Parameters  : none
+------------------------------------------------------------------------------
*/
GLOBAL void tm_handle_grlc_ready_ind ( void )
{
  UBYTE state = GET_STATE( TM );

  TRACE_FUNCTION( "tm_handle_grlc_ready_ind" );

  if( state EQ TM_ACCESS_DISABLED OR
      state EQ TM_ACCESS_PREPARED    )
  {
    TRACE_EVENT("TM_ACCESS_DISABLED/TM_ACCESS_PREPARED no ready ind");
    return;
  }

  switch (grlc_data->tm.send_grlc_ready_ind)
  {
    case SEND_A_GRLC_READY_IND:
      tm_send_grlc_ready_ind();
      grlc_data->tm.send_grlc_ready_ind = WAIT_FOR_LLC_DATA_REQ;
      break;

    case PRIM_QUEUE_FULL:
      /*
       * Check if we have now enough space
       */
      TRACE_EVENT_P3("PRIM_QUEUE_FULL  prim_start_tbf=%d prim_start_free=%ld user_data=%ld",
                                      grlc_data->prim_start_tbf,
                                      grlc_data->prim_start_free,
                                      grlc_data->prim_user_data);
      if ( (tm_prim_queue_get_free_count() > 0)            AND
           (grlc_data->prim_user_data <= grlc_data->tm.max_grlc_user_data)   )
      {
        TRACE_EVENT("NOT PRIM_QUEUE_FULL !!!");
        tm_send_grlc_ready_ind();
        grlc_data->tm.send_grlc_ready_ind = WAIT_FOR_LLC_DATA_REQ;
      }
      break;

    case WAIT_FOR_LLC_DATA_REQ:
      /* NO BREAK */
    default:
      /* Nothing to do */
      break;
  }

} /* tm_handle_grlc_ready_ind() */



/*
+------------------------------------------------------------------------------
| Function    : tm_handle_error_ra
+------------------------------------------------------------------------------
| Description : The function tm_handle_error_ra() handles actions related
|               to errors that leads to randam access procedure
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_handle_error_ra ( void )
{
  TRACE_FUNCTION( "tm_handle_error_ra" );


  /*
   * - called in case of contention resulution failed in RU
   * - t3168 expires contention resulution failed in 2P-Access
   * - If the mobile station has been assigned more PDCHs than it supports according
   *  to its MS multislot class, the mobile station shall reinitiate the packet access
   *  procedure unless it has already been repeated 4 times. In that
   *  case, TBF failure has occurred.
   */


  /*
   *  Kill TBF if running
   */

  TRACE_EVENT_P3("ERROR_RA: nacc: %d prim_start_tbf:%d   t3164_cnt=%d"
                                                            ,grlc_data->tm.n_acc_req_procedures
                                                            ,grlc_data->prim_start_tbf
                                                            ,grlc_data->t3164_to_cnt);

  if(grlc_data->tbf_type NEQ CGRLC_TBF_MODE_NULL)
  {
    SET_STATE(TM,TM_WAIT_4_PIM);
  }
  else
  {
    SET_STATE(TM,TM_PIM);
    sig_tm_ru_reset_poll_array();	
  }

  tm_abort_tbf(grlc_data->tbf_type);
  grlc_data->tm.n_res_req = 0; /* reset counter of resource requests during access */

  if (grlc_data->N3102 EQ 0)
  {
  }
  else if((grlc_data->tm.n_acc_req_procedures < 5) AND
     (grlc_data->t3164_to_cnt < 4))
  {
  }
  else
  {
    grlc_data->t3164_to_cnt            = 0;
    grlc_data->tm.n_acc_req_procedures = 0;
    tm_cgrlc_status_ind(CGRLC_TBF_ESTABLISHMENT_FAILURE);
    grlc_delete_prim();
    tm_handle_grlc_ready_ind();
  }



} /* tm_handle_error_ra() */

/*
+------------------------------------------------------------------------------
| Function    : tm_activate_tbf
+------------------------------------------------------------------------------
| Description : The function tm_activate_tbf() set the assigned TBF
|               into the data structures
|
| Parameters  : tbf_type_i - type of TBF to deactivate
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_activate_tbf ( T_TBF_TYPE tbf_type_i )
{
  TRACE_FUNCTION( "tm_activate_tbf" );

  /*
   *  Reset of n_acc_req_procedures because of the fact that number of
   *  access procedures during pacet access procedures must not considered in
   *  a error with random access occured during running TBF.
   */
 /* grlc_data->tm.n_acc_req_procedures = 0;*/

  switch( tbf_type_i )
  {
  case TBF_TYPE_UL:
    switch( grlc_data->tbf_type )
    {
    case TBF_TYPE_DL:
      grlc_data->tbf_type = TBF_TYPE_CONC;
      break;
    case TBF_TYPE_NULL:
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
    case TBF_TYPE_TP_ACCESS:
#endif
      grlc_data->tbf_type = TBF_TYPE_UL;
      break;
    default:
      break;
    }
    break;
  case TBF_TYPE_DL:
    switch( grlc_data->tbf_type )
    {
    case TBF_TYPE_UL:
      grlc_data->tbf_type = TBF_TYPE_CONC;
      break;
    case TBF_TYPE_NULL:
      grlc_data->tbf_type = TBF_TYPE_DL;
      break;
    default:
      break;
    }
    break;
  case TBF_TYPE_CONC:
    switch( grlc_data->tbf_type )
    {
    case TBF_TYPE_UL:
    case TBF_TYPE_DL:
    case TBF_TYPE_CONC:
      grlc_data->tbf_type = TBF_TYPE_CONC;
      break;
    default:
      {
        TRACE_ERROR("FATAL ERROR: tm_activate_tbf called with wrong tbf_type");
      }
      break;
    }
    break;
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
   case TBF_TYPE_TP_ACCESS:
     switch( grlc_data->tbf_type )
     {
       case TBF_TYPE_NULL:
         grlc_data->tbf_type = TBF_TYPE_TP_ACCESS;
         break;
       default:
         {
           TRACE_ERROR("FATAL ERROR: tm_activate_tbf called with wrong tbf_type");
         }
         break;
     }
     break;
#endif
  default:
      {
        TRACE_ERROR("FATAL ERROR: tm_activate_tbf called with wrong tbf_type");
      }
      break;
  } /* switch (tbf_type_i) */


} /* tm_activate_tbf() */


/*
+------------------------------------------------------------------------------
| Function    : tm_deactivate_tbf
+------------------------------------------------------------------------------
| Description : The function tm_deactivate_tbf() removes a TBF logical from TM
|               end estimates how to continue.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_deactivate_tbf ( T_TBF_TYPE tbf_i )
{
  TRACE_FUNCTION( "tm_deactivate_tbf" );

  switch( grlc_data->tbf_type )
  {

  case TBF_TYPE_CONC:
    switch( tbf_i )
    {
    case TBF_TYPE_UL:
      grlc_data->tbf_type = TBF_TYPE_DL;
      break;
    case TBF_TYPE_DL:
      grlc_data->tbf_type = TBF_TYPE_UL;
      break;
    case TBF_TYPE_CONC:
      grlc_data->tbf_type = TBF_TYPE_NULL;
      break;
    }
    break;



  case TBF_TYPE_DL:
  case TBF_TYPE_UL:
    if(grlc_data->tbf_type EQ tbf_i)
    {
      grlc_data->tbf_type = TBF_TYPE_NULL;
    }
    else
    {
      TRACE_ERROR("requested release tbf type does not exist");
      TRACE_EVENT_P2("SNH:  requested release tbf type does not exist tbf_i =%d tbf_ty=%d",tbf_i,grlc_data->tbf_type);
    }
    break;
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
  case TBF_TYPE_TP_ACCESS:
    grlc_data->tbf_type = TBF_TYPE_NULL;
    break;
#endif
  default:
    break;
  }

} /* tm_deactivate_tbf() */






/*
+------------------------------------------------------------------------------
| Function    : tm_delete_prim_queue
+------------------------------------------------------------------------------
| Description : delete all pdus in the queue
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_delete_prim_queue( void)
{
  TRACE_FUNCTION( "tm_delete_prim_queue" );

  do
  {
    grlc_delete_prim();

  } while( (grlc_data->prim_start_tbf < PRIM_QUEUE_SIZE_TOTAL) AND
           !grlc_data->prim_queue[grlc_data->prim_start_tbf].start_new_tbf );

  tm_handle_grlc_ready_ind();

} /* tm_delete_prim_queue */





/*
+------------------------------------------------------------------------------
| Function    : tm_queue_test_mode_prim()
+------------------------------------------------------------------------------
| Description : The function puts a number of primitives
|               in the primitive queue
|
| Parameters  : pdu_num_i  - number of primitives to queue
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_queue_test_mode_prim( UBYTE pdu_num_i )
{
  UBYTE i = 0;
  TRACE_FUNCTION( "tm_queue_test_mode_prim" );


  for (i=0; i<pdu_num_i; i++)
  {
    PALLOC_SDU(grlc_unitdata_req,GRLC_UNITDATA_REQ,(195*8));
    grlc_unitdata_req->sapi = GRLC_SAPI_TEST_MODE;
    grlc_unitdata_req->tlli = grlc_data->uplink_tbf.tlli;
    memset(&grlc_unitdata_req->grlc_qos,0,sizeof(T_GRLC_grlc_qos));
    grlc_unitdata_req->radio_prio=2;
    grlc_unitdata_req->sdu.l_buf= 195*8;
    grlc_unitdata_req->sdu.o_buf=0;
    memset(grlc_unitdata_req->sdu.buf,0x0f,195);             /*lint !e419*/
    PSEND(hCommGRLC,grlc_unitdata_req);
  }

} /* tm_queue_test_mode_prim */



/*
+------------------------------------------------------------------------------
| Function    : tm_close_gaps_in_ctrl_blk_seq
+------------------------------------------------------------------------------
| Description : The function tm_close_gaps_in_ctrl_blk_seq reorders the queue
|               holding the order which is used to identify the next control
|               block to sent.
|
| Parameters  : index - at this index the reordering starts
|
+------------------------------------------------------------------------------
*/
LOCAL void tm_close_gaps_in_ctrl_blk_seq ( UBYTE index )
{
  TRACE_FUNCTION( "tm_close_gaps_in_ctrl_blk_seq" );

  while( index                               <   MAX_CTRL_BLK_NUM AND
         grlc_data->tm.ul_ctrl_blk.seq[index] NEQ MAX_CTRL_BLK_NUM     )
  {
    grlc_data->tm.ul_ctrl_blk.seq[index-1] = grlc_data->tm.ul_ctrl_blk.seq[index];

    index++;
  }

  grlc_data->tm.ul_ctrl_blk.seq[index-1] = MAX_CTRL_BLK_NUM;

} /* tm_close_gaps_in_ctrl_blk_seq() */

/*
+------------------------------------------------------------------------------
| Function    : tm_store_ctrl_blk
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL tm_store_ctrl_blk ( T_BLK_OWNER blk_owner, void *blk_struct )
{
  UBYTE       i;                 /* used for counting                  */
  BOOL        result    = FALSE; /* indicates whether control block is */
                                 /* stored successfully or not         */
  T_BLK_INDEX blk_index;         /* index to the control block buffer  */

  TRACE_FUNCTION( "tm_store_ctrl_msg" );

  /*
   * we have to find a free buffer for the control block which is
   * indicated by the index to the control block buffer
   */
  blk_index = MAX_CTRL_BLK_NUM;

  switch( blk_owner )
  {
    case( CGRLC_BLK_OWNER_CTRL ):
      if( grlc_data->tm.ul_ctrl_blk.blk[BLK_INDEX_CTRL].state EQ BLK_STATE_NONE )
      {
        blk_index = BLK_INDEX_CTRL;
      }
      break;

    case( CGRLC_BLK_OWNER_CS ):
      if( grlc_data->tm.ul_ctrl_blk.blk[BLK_INDEX_CS].state EQ BLK_STATE_NONE )
      {
        blk_index = BLK_INDEX_CS;
      }
      break;

    case( CGRLC_BLK_OWNER_TM ):
      if( grlc_data->tm.ul_ctrl_blk.blk[BLK_INDEX_TM].state EQ BLK_STATE_NONE )
      {
        blk_index = BLK_INDEX_TM;
      }
      break;

    case( CGRLC_BLK_OWNER_MEAS ):

      blk_index = BLK_INDEX_MEAS;

      while( blk_index                                     <   MAX_CTRL_BLK_NUM AND
             grlc_data->tm.ul_ctrl_blk.blk[blk_index].state NEQ BLK_STATE_NONE       )
      {
        blk_index++;
      }
      break;

    default:
      /* do nothing */
      break;
  }

  /*
   * check whether the control block buffer is free,
   */
  if( blk_index < MAX_CTRL_BLK_NUM )
  {
    /*
     * check the queue holding the order which is used to identify the next
     * control block to send
     */
    i = 0;

    while( i                               <   MAX_CTRL_BLK_NUM AND
           grlc_data->tm.ul_ctrl_blk.seq[i] NEQ MAX_CTRL_BLK_NUM     )
    {
      i++;
    }

    /*
     * check whether there is a free entry in the order queue,
     * in case the check fails, there is something wrong with the concept
     */
    if( i < MAX_CTRL_BLK_NUM )
    {
      /*
       * store the control block data and state in the queues
       */
      if(blk_owner EQ CGRLC_BLK_OWNER_TM)
      {
        grlc_encode_ul_ctrl_block
        ( ( UBYTE* )&grlc_data->tm.ul_ctrl_blk.blk[blk_index].data[0],
          ( UBYTE* )blk_struct );
      }
      else
      {
        /*
         * encoded control message received by higher layers
         */
        memcpy(( UBYTE* )&grlc_data->tm.ul_ctrl_blk.blk[blk_index].data[0],( UBYTE* )blk_struct,BYTE_UL_CTRL_BLOCK);
      }

      grlc_data->tm.ul_ctrl_blk.blk[blk_index].state = BLK_STATE_ALLOCATED;
      grlc_data->tm.ul_ctrl_blk.blk[blk_index].owner = blk_owner;
      grlc_data->tm.ul_ctrl_blk.seq[i]               = blk_index;

      if( i < MAX_CTRL_BLK_NUM - 1 )
      {
        grlc_data->tm.ul_ctrl_blk.seq[i+1] = MAX_CTRL_BLK_NUM;
      }

      result = TRUE;
    }
    else
    {
      TRACE_ERROR( "tm_store_ctrl_blk: no free entry in queue found" );
    }
  }

  if( result EQ FALSE )
  {
    if( blk_owner EQ CGRLC_BLK_OWNER_MEAS )
    {
      TRACE_EVENT( "tm_store_ctrl_blk: no table entry allocated" );
    }
    else
    {
      TRACE_ERROR( "tm_store_ctrl_blk: no table entry allocated" );
    }
  }

  return( result );

} /* tm_store_ctrl_blk() */

/*
+------------------------------------------------------------------------------
| Function    : tm_cancel_ctrl_blk
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL tm_cancel_ctrl_blk ( T_BLK_OWNER blk_owner )
{
  BOOL  result     = FALSE;
  UBYTE i         = 0;
  UBYTE blk_index = grlc_data->tm.ul_ctrl_blk.seq[i];

  TRACE_FUNCTION( "tm_cancel_ctrl_blk" );


  while( blk_index NEQ MAX_CTRL_BLK_NUM AND
         result    EQ  FALSE                )
  {
    if( grlc_data->tm.ul_ctrl_blk.blk[blk_index].owner EQ blk_owner           AND
        grlc_data->tm.ul_ctrl_blk.blk[blk_index].state EQ BLK_STATE_ALLOCATED     )
    {
      /*
       * mark the entry in the queue as free
       */
      grlc_data->tm.ul_ctrl_blk.blk[blk_index].state = BLK_STATE_NONE;
      grlc_data->tm.ul_ctrl_blk.blk[blk_index].owner = CGRLC_BLK_OWNER_NONE;

      tm_close_gaps_in_ctrl_blk_seq( (UBYTE)( i + 1 ) );

      result = TRUE;
    }

    i++;
    if( i < MAX_CTRL_BLK_NUM )
    {
      blk_index = grlc_data->tm.ul_ctrl_blk.seq[i];
    }
    else
    {
      blk_index = MAX_CTRL_BLK_NUM;
    }
  }

  if( result EQ FALSE )
  {
    TRACE_EVENT( "tm_cancel_ctrl_blk: no block found" );
  }

  return( result );

} /* tm_cancel_ctrl_blk() */

/*
+------------------------------------------------------------------------------
| Function    : tm_set_start_ctrl_blk
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE* tm_set_start_ctrl_blk ( UBYTE *index )
{
  UBYTE       i         = 0;
  T_BLK_INDEX blk_index = MAX_CTRL_BLK_NUM;

  TRACE_FUNCTION( "tm_set_start_ctrl_blk" );

  while( i                               <   MAX_CTRL_BLK_NUM AND
         grlc_data->tm.ul_ctrl_blk.seq[i] NEQ MAX_CTRL_BLK_NUM AND
         blk_index                       EQ  MAX_CTRL_BLK_NUM     )
  {
    blk_index = grlc_data->tm.ul_ctrl_blk.seq[i];

    if( grlc_data->tm.ul_ctrl_blk.blk[blk_index].state EQ BLK_STATE_ALLOCATED )
    {
      grlc_data->tm.ul_ctrl_blk.blk[blk_index].state = BLK_STATE_SENT_REQ;

      /*
       * the control block was encoded without having correct information
       * about the value of the R bit. So we have to set the correct value now.
       */
      grlc_data->tm.ul_ctrl_blk.blk[blk_index].data[0] = 0x40 | grlc_data->r_bit;
    }
    else
    {
      blk_index = MAX_CTRL_BLK_NUM;
    }

    i++;
  }

  *index = blk_index;

  return( blk_index EQ MAX_CTRL_BLK_NUM ?
                         NULL : grlc_data->tm.ul_ctrl_blk.blk[blk_index].data );

} /* tm_set_start_ctrl_blk() */

/*
+------------------------------------------------------------------------------
| Function    : tm_set_stop_ctrl_blk
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL T_BLK_INDEX tm_set_stop_ctrl_blk
        ( BOOL is_tx_success, T_BLK_OWNER srch_owner, T_BLK_INDEX start_index )
{
  T_BLK_INDEX blk_index = grlc_data->tm.ul_ctrl_blk.seq[start_index];
  T_BLK_INDEX nxt_index;

  TRACE_FUNCTION( "tm_set_stop_ctrl_blk" );

  if( blk_index < MAX_CTRL_BLK_NUM )
  {
    T_BLK_OWNER blk_owner = grlc_data->tm.ul_ctrl_blk.blk[blk_index].owner;

    if( srch_owner EQ CGRLC_BLK_OWNER_NONE OR
        srch_owner EQ blk_owner         )
    {
      /*
       * mark the entry in the queue as free
       */
      grlc_data->tm.ul_ctrl_blk.blk[blk_index].state = BLK_STATE_NONE;
      grlc_data->tm.ul_ctrl_blk.blk[blk_index].owner = CGRLC_BLK_OWNER_NONE;

      tm_close_gaps_in_ctrl_blk_seq( 1 );

      nxt_index = start_index;
    }
    else
    {
      nxt_index = start_index + 1;
    }
  }
  else
  {
    nxt_index = MAX_CTRL_BLK_NUM;
  }

  return( nxt_index );

} /* tm_set_stop_ctrl_blk() */

/*
+------------------------------------------------------------------------------
| Function    : tm_set_stop_tm_ctrl_blk
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_set_stop_tm_ctrl_blk ( void )
{
  T_BLK_INDEX start_index = 0;

  TRACE_FUNCTION( "tm_set_stop_tm_ctrl_blk" );

  while
  (
    ( start_index = tm_set_stop_ctrl_blk( FALSE, CGRLC_BLK_OWNER_TM, start_index ) )
                                                           NEQ MAX_CTRL_BLK_NUM
                                                           ){};
} /* tm_set_stop_tm_ctrl_blk() */

/*
+------------------------------------------------------------------------------
| Function    : tm_set_stop_all_ctrl_blk
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_set_stop_all_ctrl_blk ( void )
{
  T_BLK_INDEX start_index = 0;

  TRACE_FUNCTION( "tm_set_stop_all_ctrl_blk" );

  while
  (
    ( start_index = tm_set_stop_ctrl_blk( FALSE,
                                          CGRLC_BLK_OWNER_NONE,
                                          start_index ) ) NEQ MAX_CTRL_BLK_NUM
                                          ){};
} /* tm_set_stop_all_ctrl_blk() */

/*
+------------------------------------------------------------------------------
| Function    : tm_get_gmm_prim_queue
+------------------------------------------------------------------------------
| Description : The function tm_get_gmm_prim_queue () ....
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_get_gmm_prim_queue ( void )
{
  UBYTE i;
  TRACE_FUNCTION( "tm_get_gmm_prim_queue " );



  /* init gmm prim queue*/
  TRACE_EVENT_P4("GET GMM QUEUE: BEFORE ps=%d, pf=%d,sps=%d,spf=%d",
                                                      grlc_data->prim_start_tbf,
                                                      grlc_data->prim_start_free,
                                                      grlc_data->save_prim_start_tbf,
                                                      grlc_data->save_prim_start_free);

  /*
   * Delte all GMM primitives in current llc queue, which are on the top.
   */
  while((grlc_data->prim_start_tbf >= PRIM_QUEUE_SIZE) AND
        (grlc_data->prim_start_tbf < PRIM_QUEUE_SIZE_TOTAL))
  {
    grlc_delete_prim();
  }

  grlc_data->gmm_procedure_is_running = TRUE;

  if(grlc_data->prim_start_free EQ 0xFF)
  {
    TRACE_ERROR("PRIM_QUEUE FULL: RAU MAY BE SENT, NEXT DATA PRIM WILL BE DELETED ");
  }

  for (i=PRIM_QUEUE_SIZE; i<(PRIM_QUEUE_SIZE_TOTAL);i++)
  {
    grlc_data->prim_queue[i].next = i+1;

    grlc_data->prim_queue[i].prim_ptr      = NULL;
    grlc_data->prim_queue[i].prim_type     = CGRLC_LLC_PRIM_TYPE_NULL;
    grlc_data->prim_queue[i].cv_status     = FALSE;
    grlc_data->prim_queue[i].rlc_status    = FALSE;
    grlc_data->prim_queue[i].re_allocation = FALSE;
    grlc_data->prim_queue[i].start_new_tbf = FALSE;
    grlc_data->prim_queue[i].last_bsn      = 0xff;
  }
  grlc_data->prim_queue[PRIM_QUEUE_SIZE_TOTAL-1].next = 0xff;

  grlc_data->save_prim_start_tbf  = grlc_data->prim_start_tbf;
  grlc_data->save_prim_start_free = grlc_data->prim_start_free;

  grlc_data->prim_start_tbf  = 0xFF;
  grlc_data->prim_start_free = PRIM_QUEUE_SIZE;
  grlc_data->prim_user_data  = 0;
  grlc_data->tm.send_grlc_ready_ind = SEND_A_GRLC_READY_IND;

  TRACE_EVENT_P4("GET GMM QUEUE: AFTER ps=%d, pf=%d,sps=%d,spf=%d",
                                                      grlc_data->prim_start_tbf,
                                                      grlc_data->prim_start_free,
                                                      grlc_data->save_prim_start_tbf,
                                                      grlc_data->save_prim_start_free);


} /* tm_get_gmm_prim_queue  */

/*
+------------------------------------------------------------------------------
| Function    : tm_get_llc_prim_queue
+------------------------------------------------------------------------------
| Description : The function tm_get_llc_prim_queue () ....
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_get_llc_prim_queue ( void )
{
  UBYTE i;

  TRACE_FUNCTION( "tm_get_llc_prim_queue " );


  TRACE_EVENT_P4("GET LLC QUEUE: BEFORE ps=%d, pf=%d,sps=%d,spf=%d",
                                                      grlc_data->prim_start_tbf,
                                                      grlc_data->prim_start_free,
                                                      grlc_data->save_prim_start_tbf,
                                                      grlc_data->save_prim_start_free);


  if(grlc_data->prim_start_tbf < PRIM_QUEUE_SIZE_TOTAL)
  {
    /*finish gmm data and than resume with llc data*/
   i = grlc_data->prim_start_tbf;
    while(grlc_data->prim_queue[i].next NEQ 0xFF)
    {
      i = grlc_data->prim_queue[i].next;
    }
    grlc_data->prim_queue[i].next          = grlc_data->save_prim_start_tbf;
    grlc_data->prim_queue[i].start_new_tbf = TRUE;
  }
  else
  {
    grlc_data->prim_start_tbf  = grlc_data->save_prim_start_tbf;
  }
  grlc_data->prim_start_free = grlc_data->save_prim_start_free;
  grlc_data->gmm_procedure_is_running = FALSE;
  grlc_data->prim_user_data = 0;
  i = grlc_data->prim_start_tbf;
  while(i NEQ 0xFF)
  {
    grlc_data->prim_user_data += BYTELEN(grlc_data->prim_queue[i].prim_ptr->sdu.l_buf);
    i = grlc_data->prim_queue[i].next;
  }


  TRACE_EVENT_P4("GET LLC QUEUE: AFTER ps=%d, pf=%d,sps=%d,spf=%d",
                                                      grlc_data->prim_start_tbf,
                                                      grlc_data->prim_start_free,
                                                      grlc_data->save_prim_start_tbf,
                                                      grlc_data->save_prim_start_free);




} /* tm_get_llc_prim_queue  */

/*
+------------------------------------------------------------------------------
| Function    : tm_ul_tbf_ind
+------------------------------------------------------------------------------
| Description : The function tm_ul_tbf_ind () ....
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_ul_tbf_ind ( void )
{
  TRACE_FUNCTION( "tm_ul_tbf_ind " );



  switch(grlc_data->tbf_type)
  {
    case TBF_TYPE_NULL:
      if( grlc_data->prim_start_tbf < PRIM_QUEUE_SIZE_TOTAL )
      {
        PALLOC(cgrlc_ul_tbf_ind,CGRLC_UL_TBF_IND); /*T_CGRLC_UL_TBF_IND*/

        tm_start_access();

        cgrlc_ul_tbf_ind->access_type    = grlc_data->uplink_tbf.access_type;
        cgrlc_ul_tbf_ind->ra_prio        = grlc_data->uplink_tbf.prio;
        cgrlc_ul_tbf_ind->nr_blocks      = grlc_data->uplink_tbf.nr_blocks;
        cgrlc_ul_tbf_ind->llc_prim_type  = grlc_data->prim_queue[grlc_data->prim_start_tbf].prim_type;
        cgrlc_ul_tbf_ind->rlc_oct_cnt    = grlc_data->prim_queue[grlc_data->prim_start_tbf].prim_ptr->sdu.l_buf/8 +1;
        cgrlc_ul_tbf_ind->peak           = grlc_data->prim_queue[grlc_data->prim_start_tbf].prim_ptr->grlc_qos.peak;
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
        cgrlc_ul_tbf_ind->tbf_est_pacch  = FALSE;
#endif

        SET_STATE( TM, TM_PAM );
        PSEND(hCommGRR,cgrlc_ul_tbf_ind);
      }
      else
      {
        /*
         * inform higher layers that no tbf is requested
         */
        PALLOC(cgrlc_ul_tbf_ind,CGRLC_UL_TBF_IND); /*T_CGRLC_UL_TBF_IND*/
        cgrlc_ul_tbf_ind->access_type     = CGRLC_AT_NULL;
        cgrlc_ul_tbf_ind->ra_prio         = 0xEE;
        cgrlc_ul_tbf_ind->nr_blocks       = 0xEE;
        cgrlc_ul_tbf_ind->llc_prim_type   = 0xEE;
        cgrlc_ul_tbf_ind->peak            = 0xEE;
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
        cgrlc_ul_tbf_ind->tbf_est_pacch  = FALSE;
#endif


        PSEND(hCommGRR,cgrlc_ul_tbf_ind);
        /*
         * delete control message queue
         */
        tm_set_stop_all_ctrl_blk();
      }
      tm_handle_grlc_ready_ind( );
    break;
    case TBF_TYPE_DL:
      if( (grlc_data->prim_start_tbf < PRIM_QUEUE_SIZE_TOTAL) AND (!grlc_data->tm.n_res_req))
      {
        tm_start_access();
        tm_build_chan_req_des(&grlc_data->chan_req_des,
                               &grlc_data->prim_queue[grlc_data->prim_start_tbf]);
        sig_tm_rd_ul_req();
        grlc_data->tm.n_res_req++;
      }
      break;
    case TBF_TYPE_CONC:
    case TBF_TYPE_UL:
      break;
    default:
      TRACE_ERROR("unknown tbf type during uplink access");
      break;
  }
} /* tm_ul_tbf_ind  */












/*
+------------------------------------------------------------------------------
| Function    : tm_tfi_handling
+------------------------------------------------------------------------------
| Description : The function tm_tfi_handling () handles modified tfi´s in
|               case of starting time
|
| Parameters  : start_fn_present: indicates if a starting time is present
|               tbf_type : type of the current tbf
|               ul_tfi   : new assignend uplink tfi
|               dl_tfi   : new assignend downlink tfi
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_tfi_handling  (ULONG tbf_start,UBYTE tbf_type, UBYTE ul_tfi, UBYTE dl_tfi)
{

  TRACE_FUNCTION( "tm_tfi_handling " );


  /*TRACE_EVENT_P6("TFI CHANGE: =tbf_type = %d\n fn= %ld \n new_Utfi=%d old_Utfi=%d \n new_Dtfi=%d old_Dtfi=%d"
                                     ,tbf_type
                                     ,tbf_start
                                     ,ul_tfi
                                     ,grlc_data->ul_tfi
                                     ,dl_tfi
                                     ,grlc_data->dl_tfi);

  */
  switch( tbf_type )
  {
    case CGRLC_TBF_MODE_UL:
      if((tbf_start NEQ CGRLC_STARTING_TIME_NOT_PRESENT)  AND  (ul_tfi NEQ grlc_data->ul_tfi)) /*lint !e650*/
      {
        if(grlc_data->tfi_change  EQ TFI_CHANGE_NULL)
          grlc_data->tfi_change = TFI_CHANGE_UL;
        else if(grlc_data->tfi_change  EQ TFI_CHANGE_DL)
          grlc_data->tfi_change = TFI_CHANGE_ALL;
        grlc_data->start_fn_ul_tfi = ul_tfi;
        grlc_data->ul_tbf_start_time = tbf_start;
        TRACE_EVENT_P3("GRLC UL TFI CHANGE AFTER ST. TIME = %ld new_tfi=%d old_tfi=%d"
                                     ,tbf_start
                                     ,ul_tfi
                                     ,grlc_data->ul_tfi);
      }
      else
      {
        if(grlc_data->tfi_change  EQ TFI_CHANGE_UL)
          grlc_data->tfi_change = TFI_CHANGE_NULL;
        else if(grlc_data->tfi_change  EQ TFI_CHANGE_ALL)
          grlc_data->tfi_change = TFI_CHANGE_DL;
        grlc_data->ul_tfi          = ul_tfi;
        grlc_data->start_fn_ul_tfi = 0xFF;
        grlc_data->ul_tbf_start_time = CGRLC_STARTING_TIME_NOT_PRESENT;
      }
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
      sig_tm_gff_ul_activate(GFF_ACTIVE);
#else
      sig_tm_gff_ul_activate();
#endif
      break;
    case CGRLC_TBF_MODE_DL:
      if((tbf_start NEQ CGRLC_STARTING_TIME_NOT_PRESENT) AND  (dl_tfi NEQ grlc_data->dl_tfi)) /*lint !e650*/
      {
        if(grlc_data->tfi_change  EQ TFI_CHANGE_NULL)
          grlc_data->tfi_change = TFI_CHANGE_DL;
        else if(grlc_data->tfi_change  EQ TFI_CHANGE_UL)
          grlc_data->tfi_change = TFI_CHANGE_ALL;
        grlc_data->start_fn_dl_tfi = dl_tfi;
        grlc_data->dl_tbf_start_time = tbf_start;
        TRACE_EVENT_P3("GRLC DL TFI CHANGE AFTER ST. TIME =%ld new_tfi=%d old_tfi=%d"
                                     ,tbf_start
                                     ,dl_tfi
                                     ,grlc_data->dl_tfi);
      }
      else
      {
        if(grlc_data->tfi_change  EQ TFI_CHANGE_DL)
          grlc_data->tfi_change = TFI_CHANGE_NULL;
        else if(grlc_data->tfi_change  EQ TFI_CHANGE_ALL)
          grlc_data->tfi_change = TFI_CHANGE_UL;
        grlc_data->dl_tfi = dl_tfi;
        grlc_data->start_fn_dl_tfi = 0xFF;
        grlc_data->dl_tbf_start_time = CGRLC_STARTING_TIME_NOT_PRESENT;
      }
      sig_tm_gff_dl_activate();
      break;
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
    case CGRLC_TBF_MODE_2PA:
      sig_tm_gff_ul_activate(GFF_TWO_PHASE);
      break;
#endif

  }


  /*TRACE_EVENT_P1("TFI_CHANGE = %d",grlc_data->tfi_change );*/

} /* tm_tfi_handling () */



/*
+------------------------------------------------------------------------------
| Function    : tm_cgrlc_status_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive CGRLC_STATUS_IND
|
| Parameters  : cause - status cause, gmm is informed
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_cgrlc_status_ind ( UBYTE cause )
{
  PALLOC(cgrlc_status_ind,CGRLC_STATUS_IND);

  TRACE_FUNCTION( "tm_cgrlc_status_ind" );

  cgrlc_status_ind->failure = cause;
  PSEND(hCommGMM,cgrlc_status_ind);

  TRACE_EVENT_P1("error cause = %ld",cause);


}/* tm_cgrlc_status_ind*/

/*
+------------------------------------------------------------------------------
| Function    : tm_handle_polling_bit
+------------------------------------------------------------------------------
| Description : Handles the the polling in the immediate assignment (uplink or
|               downlink)
|
| Parameters  : st_fn - starting time
|               tn    - timeslot
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_handle_polling_bit ( ULONG st_fn, UBYTE tn )
{

  TRACE_FUNCTION( "tm_handle_polling_bit" );



  grlc_data->next_poll_fn       = st_fn;
  grlc_data->ul_poll_pos_index  = 0;
  if(grlc_data->burst_type EQ 0)
  {
    grlc_send_access_burst( tn );
  }
  else
  {
    grlc_send_normal_burst(grlc_set_packet_ctrl_ack(), NULL, tn);
  }



}/* tm_handle_polling_bit */



/*
+------------------------------------------------------------------------------
| Function    : tm_store_fa_bitmap
+------------------------------------------------------------------------------
| Description : This function stores the allocation bitmap in
|               case of fixed allocation
|
| Parameters  : ptr2_fix_alloc - ptr to the fixed allocation struct
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_store_fa_bitmap (T_CGRLC_fix_alloc_struct * ptr2_fix_alloc)
{
  UBYTE i,j;
  USHORT ul_res_sum = 0;
  T_FA_ALLOC  * ptr_fa_ctrl;

  TRACE_FUNCTION( "tm_store_fa_bitmap" );


  /*
   * calculate uplink resources allocation
   */

  for(i=0;i<ptr2_fix_alloc->bitmap_len;i++)
  {
    if(ptr2_fix_alloc->bitmap_array[i])
    {
      UBYTE mask = 0x80;

      for(j=0; j<=7; j++)
      {
        if(ptr2_fix_alloc->bitmap_array[i] & mask)
          ul_res_sum++;
        mask>>=1;
      }
    }
  }


  /*
   * set fa_ctrl
   */
  if(grlc_data->uplink_tbf.fa_manag.fa_type EQ FA_NO_CURRENT)
  {
    grlc_data->uplink_tbf.fa_manag.fa_type = FA_NO_NEXT;
    ptr_fa_ctrl                            = &grlc_data->uplink_tbf.fa_manag.current_alloc;
    ptr_fa_ctrl->alloc_start_fn            = grlc_data->ul_tbf_start_time;
    ptr_fa_ctrl->alloc_end_fn              = ptr2_fix_alloc->end_fn;
  }
  else
  {
    grlc_data->uplink_tbf.fa_manag.fa_type = FA_BITMAP;
    ptr_fa_ctrl                            = &grlc_data->uplink_tbf.fa_manag.next_alloc;
    ptr_fa_ctrl->alloc_start_fn            = grlc_data->ul_tbf_start_time;

    /*
     * check conflict of end of current alloc and start of next alloc
     */
    if( grlc_check_dist(grlc_data->uplink_tbf.fa_manag.current_alloc.alloc_end_fn,
        grlc_data->uplink_tbf.fa_manag.next_alloc.alloc_start_fn,10000))
    {
      ULONG delta_fn;
      UBYTE idle_frames;
      UBYTE iden_radio_blocks;
      UBYTE ts_delet;

      delta_fn          = grlc_data->uplink_tbf.fa_manag.current_alloc.alloc_end_fn -
                           grlc_data->uplink_tbf.fa_manag.next_alloc.alloc_start_fn;
      idle_frames       = (UBYTE)((delta_fn / 52) *4  + delta_fn % 4);
      iden_radio_blocks = (UBYTE)((delta_fn - idle_frames) / 4 + 1);

      /*
       * remove shared allocation from current allocation
       */
      ts_delet = 0;
      for(i=0;i<iden_radio_blocks;i++)
      {
        UBYTE pos;
        pos = grlc_data->uplink_tbf.fa_manag.current_alloc.alloc.bitmap_len - i;

        /*
         * calculate number of timeslots for current frame and delete them from resources
         */
        if(grlc_data->uplink_tbf.fa_manag.current_alloc.alloc.bitmap_array[pos])
        {
          UBYTE mask = 0x80;

          for(j=0; j<=7; j++)
          {
            if(ptr2_fix_alloc->bitmap_array[i] & mask)
              ts_delet++;
            mask>>=1;
          }
        }
        grlc_data->uplink_tbf.fa_manag.current_alloc.alloc.bitmap_array[pos] = 0;
      }
      grlc_data->uplink_tbf.fa_manag.current_alloc.ul_res_sum -= ts_delet;
      grlc_data->uplink_tbf.fa_manag.ul_res_sum -= ts_delet;
      grlc_data->uplink_tbf.fa_manag.current_alloc.alloc.bitmap_len -= iden_radio_blocks;
      grlc_data->uplink_tbf.fa_manag.current_alloc.alloc_end_fn =
          grlc_data->uplink_tbf.fa_manag.current_alloc.alloc_end_fn - delta_fn;
    }
  }
  grlc_data->uplink_tbf.fa_manag.ul_res_sum += ul_res_sum;
  ptr_fa_ctrl->ul_res_sum                    = ul_res_sum;
  /*
   * store end of fixed allocation
   */
  ptr_fa_ctrl->alloc_end_fn              = ptr2_fix_alloc->end_fn;


  tm_handle_final_alloc (ptr2_fix_alloc->final_alloc);

} /* tm_store_fa_bitmap() */


/*
+------------------------------------------------------------------------------
| Function    : tm_handle_final_alloc
+------------------------------------------------------------------------------
| Description : This function handles the final allocation bit.
|
| Parameters  :   -
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_handle_final_alloc (UBYTE final_allocation)
{
  UBYTE  next;
  T_RLC_VALUES  rlc_val;
  USHORT rlc_data_size=0;

  TRACE_FUNCTION( "tm_handle_final_alloc" );

  switch(grlc_data->uplink_tbf.cs_type)
  {
    case CS_1:
      rlc_data_size = 20;
      break;
    case CS_2:
      rlc_data_size = 30;
      break;
    case CS_3:
      rlc_data_size = 36;
      break;
    case CS_4:
      rlc_data_size = 50;
      break;
    default:
      break;
  }
  if(grlc_data->prim_start_tbf < PRIM_QUEUE_SIZE_TOTAL AND
     grlc_data->prim_queue[grlc_data->prim_start_tbf].rlc_status EQ FALSE )
  {
    next = grlc_data->prim_queue[grlc_data->prim_start_tbf].next;
  }
  else
  {
    next = grlc_data->prim_start_tbf;
    TRACE_EVENT_P3("PST=%d PSF=%d PDU=%d: tm_handle_final_alloc"
                                                           ,grlc_data->prim_start_tbf
                                                           ,grlc_data->prim_start_free
                                                           ,grlc_data->grlc_data_req_cnt);

  }
  grlc_get_sdu_len_and_used_ts( &rlc_val);
  grlc_data->uplink_tbf.fa_manag.tbf_oct_cnt = rlc_val.sdu_len;
  grlc_data->uplink_tbf.fa_manag.ul_res_used = rlc_val.cnt_ts;
  grlc_data->uplink_tbf.fa_manag.ul_res_remain =
       grlc_data->uplink_tbf.fa_manag.ul_res_sum -  grlc_data->uplink_tbf.fa_manag.ul_res_used;
  grlc_data->uplink_tbf.fa_manag.tbf_oct_cap_remain = grlc_data->uplink_tbf.fa_manag.ul_res_remain * rlc_data_size;

  if(final_allocation)
  {
    /*
     * last allocation of the tbf, it must end at the end of tbf.
     * check if more data is in prim queue than available uplink resources.
     * NO: nothing to do
     * YES: reorganize prim queue(set start_new_tbf) and inform RU
     */


    while((grlc_data->prim_queue[next].start_new_tbf EQ 0) AND
          (next NEQ 0xFF) )
    {
      if(grlc_data->uplink_tbf.fa_manag.tbf_oct_cap_remain <
        (grlc_data->uplink_tbf.fa_manag.tbf_oct_cnt + grlc_data->prim_queue[next].prim_ptr->sdu.l_buf/8 + 1))
      {
        /*
         * find the end of the tbf.
         */
        grlc_data->prim_queue[next].start_new_tbf = 1;
        sig_tm_ru_queue_status();
        return;
      }
      grlc_data->uplink_tbf.fa_manag.tbf_oct_cnt   += grlc_data->prim_queue[next].prim_ptr->sdu.l_buf/8;
      grlc_data->uplink_tbf.fa_manag.tbf_oct_cnt++; /* for pdu boundaries */
      next                        = grlc_data->prim_queue[next].next;
    }
  }
  else
  {
    /*
     * not the last allocation
     */
    while((grlc_data->prim_queue[next].start_new_tbf EQ 0) AND
          (next NEQ 0xFF) )
    {
      grlc_data->uplink_tbf.fa_manag.tbf_oct_cnt   += grlc_data->prim_queue[next].prim_ptr->sdu.l_buf/8;
      grlc_data->uplink_tbf.fa_manag.tbf_oct_cnt++; /* for pdu boundaries */
      next                        = grlc_data->prim_queue[next].next;
    }
  }


} /* tm_handle_final_alloc() */


/*
+------------------------------------------------------------------------------
| Function    : tm_set_fa_bitmap
+------------------------------------------------------------------------------
| Description : sets fixed allocation bitmap for requested allocation after
|               receiving a repeat allocation with ts_override
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL USHORT tm_set_fa_bitmap( UBYTE ts_mask, T_FA_ALLOC* ptr_alloc)
{
  UBYTE  i,j;
  USHORT  tx_slots;
  TRACE_FUNCTION( "tm_set_fa_bitmap" );

  if (ts_mask)
  {
    /*
     * add new time slots to current allocation
     */
    tx_slots = 0;
    for(i=0;i<grlc_data->uplink_tbf.fa_manag.current_alloc.alloc.bitmap_len;i++)
    {
      ptr_alloc->alloc.bitmap_array[i] = grlc_data->uplink_tbf.fa_manag.current_alloc.alloc.bitmap_array[i] | ts_mask;

      if(ptr_alloc->alloc.bitmap_array[i])
      {
        UBYTE mask = 0x80;

        for(j=0; j<=7; j++)
        {
          if(ptr_alloc->alloc.bitmap_array[i] & mask)
            tx_slots++;
          mask>>=1;
        }
      }


    }
  }
  else
  {
    /*
     * no new timeslot allocated
     */
    tx_slots = grlc_data->uplink_tbf.fa_manag.current_alloc.ul_res_sum;
  }
  return (tx_slots);

} /* tm_set_fa_bitmap() */

/*
+------------------------------------------------------------------------------
| Function    : tm_handle_test_mode_cnf
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_handle_test_mode_cnf ( BOOL v_test_mode_cnf )
{
  TRACE_FUNCTION( "tm_handle_test_mode_cnf" );

  if( v_test_mode_cnf EQ TRUE )
  {
    PALLOC( cgrlc_test_mode_cnf, CGRLC_TEST_MODE_CNF );

    grlc_data->testmode.mode = CGRLC_NO_TEST_MODE;

    PSEND( hCommGMM, cgrlc_test_mode_cnf );

    tm_delete_prim_queue();
    /*inform grr that testmode is finished*/
    {
      PALLOC(cgrlc_test_mode_ind,CGRLC_TEST_MODE_IND); /*T_CGRLC_TEST_MODE_IND*/
      cgrlc_test_mode_ind->test_mode_flag = CGRLC_NO_TEST_MODE;
      PSEND(hCommGRR,cgrlc_test_mode_ind);
    }

  }
} /* tm_handle_test_mode_cnf() */

/*
+------------------------------------------------------------------------------
| Function    : tm_prcs_pwr_ctrl
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void tm_prcs_pwr_ctrl ( T_CGRLC_pwr_ctrl *pwr_ctrl )
{
  TRACE_FUNCTION( "tm_prcs_pwr_ctrl" );

  if( pwr_ctrl->v_pwr_ctrl_param )
  {
    tpc_set_pwr_ctrl_param( &pwr_ctrl->pwr_ctrl_param );
  }

  if( pwr_ctrl->v_glbl_pwr_ctrl_param )
  {
    tpc_set_glbl_pwr_ctrl_param( &pwr_ctrl->glbl_pwr_ctrl_param );
  }

  if( pwr_ctrl->v_freq_param )
  {
    grlc_data->tm.freq_param = pwr_ctrl->freq_param;
  }

  if( pwr_ctrl->v_c_value )
  {
    meas_grlc_c_set_c_value( &pwr_ctrl->c_value );
  }

  sig_tm_tpc_update_pch( );

  {
    PALLOC( pwr_ctrl_cnf, CGRLC_PWR_CTRL_CNF );
    PSEND( hCommGRR, pwr_ctrl_cnf );
  }
} /* tm_prcs_pwr_ctrl() */


#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
GLOBAL void tm_send_msg_directly_to_l1_queue (UBYTE * unencoded_msg)
{

  TRACE_FUNCTION( "tm_send_msg_directly_to_l1_queue" );

  /* Call function that formats ARAC message  */

  memset(&grlc_data->ru.ul_data[0],
      0,
      sizeof(T_ul_data));

  grlc_data->ru.ul_data[0].block_status = 2;
  grlc_encode_ul_ctrl_block(( UBYTE* ) grlc_data->ru.ul_data[0].ul_block,
  ( UBYTE* )unencoded_msg );

#ifdef _SIMULATION_
  {
    PALLOC(mac_data_req,MAC_DATA_REQ);
    memset(&(mac_data_req->ul_data),
          0,
          sizeof(T_ul_data));
    memcpy(&(mac_data_req->ul_data),
          &(grlc_data->ru.ul_data[0]),
          sizeof(T_ul_data));
    PSEND(hCommL1,mac_data_req);
  }
#else
  /* No need to copy again */
#endif   /* ! _SIMULATION_ */

  grlc_data->ru.ul_data[1].block_status = 0;
  return;

}


GLOBAL void tm_send_prr_2p_ptm ( void )
{
  T_U_GRLC_RESOURCE_REQ resource_req;

  TRACE_FUNCTION( "tm_send_prr_2p_ptm" );

  tm_build_res_req( &resource_req, R_BUILD_2PHASE_ACCESS);

  tm_send_msg_directly_to_l1_queue((UBYTE *)&resource_req);

  return;

}
#endif

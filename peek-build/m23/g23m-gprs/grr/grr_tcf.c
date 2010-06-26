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
|  Purpose :  This module implements local functions for service TC of
|             entity GRR.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRR_TCF_C
#define GRR_TCF_C
#endif

#define ENTITY_GRR

/*==== INCLUDES =============================================================*/

#include <stdio.h>
#include <string.h>

#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "ccdapi.h"     /* to get CCD API */
#include "cnf_grr.h"    /* to get cnf-definitions */
#include "mon_grr.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "message.h"    /* to get air message definitions */
#include "grr.h"        /* to get the global entity definitions */

#include "grr_f.h"      /* to get Process GRR global function definitions */
#include "grr_tcf.h"    /* to get Service TC global function definitions  */
#include "grr_tcs.h"    /* to get Service TC inter signal definitions  */
#include "grr_ctrls.h"  /* to get interface to service CTRL */
#include "grr_cpaps.h"  /* to get interface to service CPAP */
#include "grr_meass.h"  /* to get the definitions for interference measurements */
#include "cl_rlcmac.h"

/*==== CONST ================================================================*/

typedef enum
{
  GRR_STORE_TYPE_NONE,
  GRR_STORE_TYPE_TN_ALLOC_PWR,
  GRR_STORE_TYPE_ALF_GAM,
  GRR_STORE_TYPE_PWR_PAR
} T_GRR_STORE_TYPE;

/*
 * used in tc_calc_req
 *
 */
#define RANDOM_VALUE  0xAAAA

#define RANDOM_2BIT (RANDOM_VALUE & 0x0003)
#define RANDOM_3BIT (RANDOM_VALUE & 0x0007)
#define RANDOM_5BIT (RANDOM_VALUE & 0x001F)

#define ONE_PHASE_MASK_11BIT (0x0000 | RANDOM_3BIT )
#define SHORT_ACCESS_MASK_11BIT (0x0400 | RANDOM_3BIT )
#define TWO_PHASE_MASK_11BIT (0x0600 | RANDOM_3BIT )
#define PAGE_RESPONSE_MASK_11BIT (0x0620 | RANDOM_5BIT )
#define CELL_UPDATE_MASK_11BIT (0x0640 | RANDOM_5BIT )
#define MM_PROCEDURE_MASK_11BIT (0x0660 | RANDOM_5BIT )
#define SINGLE_BLOCK_MASK_11BIT (0x0680 | RANDOM_5BIT )

#define ONE_PHASE_MASK_8BIT (0x0080 | RANDOM_2BIT )
#define SHORT_ACCESS_MASK_8BIT (0x0000 | RANDOM_3BIT )
#define TWO_PHASE_MASK_8BIT (0x0040 | RANDOM_3BIT )
#define PAGE_RESPONSE_MASK_8BIT (0x0048 | RANDOM_3BIT )
#define CELL_UPDATE_MASK_8BIT (0x0050 | RANDOM_3BIT )
#define MM_PROCEDURE_MASK_8BIT (0x0058 | RANDOM_3BIT )
#define SINGLE_BLOCK_MASK_8BIT (0x0060 | RANDOM_3BIT )

/*
 * used in tc_handle_ra_con
 */
const UBYTE max_retrans_table[4] = {1,2,4,7};

const UBYTE tx_int_table[16] = {2,3,4,5,6,7,8,9,10,12,14,16,20,25,32,50};

const UBYTE s_table[16] = {12,15,20,30,41,55,76,109,163,217,
                           S_VALUE_RESERVED,S_VALUE_RESERVED,S_VALUE_RESERVED,
                           S_VALUE_RESERVED,S_VALUE_RESERVED,S_VALUE_RESERVED };



/*==== DIAGNOSTICS ==========================================================*/

/*==== LOCAL VARS ===========================================================*/


/*==== PRIVATE FUNCTIONS ====================================================*/

LOCAL void tc_set_da_assignment (T_MPHP_ASSIGNMENT_REQ * ptr2prim_i);
LOCAL void tc_set_fa_assignment (T_MPHP_ASSIGNMENT_REQ * ptr2prim_i);
LOCAL void tc_set_dl_assignment (T_MPHP_ASSIGNMENT_REQ * ptr2prim_i);

LOCAL void tc_close_gaps_in_ctrl_blk_seq( UBYTE index );

LOCAL void tc_prepare_send_tbf_release_req( T_TBF_TYPE tbf_type );

LOCAL BOOL tc_check_and_save_freq (void);

LOCAL USHORT grr_convert_11bit_2_etsi ( USHORT eleven_bit );

LOCAL USHORT tc_one_phase_mask_11bit (void)
{
#ifdef _TARGET_
    return (0x0000 | (USHORT)grr_random_value(PCR_RND_INDEX, 8));
#else
    return ONE_PHASE_MASK_11BIT;
#endif
}

LOCAL SHORT tc_short_access_mask_11bit (void)
{
#ifdef _TARGET_
  return (0x0400 | (USHORT)grr_random_value(PCR_RND_INDEX, 8) );
#else
  return SHORT_ACCESS_MASK_11BIT;
#endif
}

LOCAL SHORT tc_two_phase_mask_11bit  (void)
{
#ifdef _TARGET_
  return (0x0600 | (USHORT)grr_random_value(PCR_RND_INDEX, 8) );
#else
  return TWO_PHASE_MASK_11BIT;
#endif
}

LOCAL SHORT tc_page_response_mask_11bit  (void)
{
#ifdef _TARGET_
  return (0x0620 | (USHORT)grr_random_value(PCR_RND_INDEX, 32) );
#else
  return PAGE_RESPONSE_MASK_11BIT;
#endif
}

LOCAL SHORT tc_cell_update_mask_11bit  (void)
{
#ifdef _TARGET_
  return (0x0640 | (USHORT)grr_random_value(PCR_RND_INDEX, 32) );
#else
  return CELL_UPDATE_MASK_11BIT;
#endif
}

LOCAL SHORT tc_mm_procedure_mask_11bit  (void)
{
#ifdef _TARGET_
  return (0x0660 |(USHORT)grr_random_value(PCR_RND_INDEX, 32) );
#else
  return MM_PROCEDURE_MASK_11BIT;
#endif
}

LOCAL SHORT tc_single_block_mask_11bit  (void)
{
#ifdef _TARGET_
  return (0x0680 | (USHORT)grr_random_value(PCR_RND_INDEX, 32) );
#else
  return SINGLE_BLOCK_MASK_11BIT;
#endif
}

LOCAL SHORT tc_one_phase_mask_8bit  (void)
{
#ifdef _TARGET_
  return (0x0080 | (USHORT)grr_random_value(PCR_RND_INDEX, 4) );
#else
  return ONE_PHASE_MASK_8BIT;
#endif
}

LOCAL SHORT tc_short_access_mask_8bit  (void)
{
#ifdef _TARGET_
  return (0x0000 | (USHORT)grr_random_value(PCR_RND_INDEX, 8) );
#else
  return SHORT_ACCESS_MASK_8BIT;
#endif
}

LOCAL SHORT tc_two_phase_mask_8bit  (void)
{
#ifdef _TARGET_
  return (0x0040 | (USHORT)grr_random_value(PCR_RND_INDEX, 8) );
#else
  return TWO_PHASE_MASK_8BIT;
#endif
}

LOCAL SHORT tc_page_response_mask_8bit  (void)
{
#ifdef _TARGET_
  return (0x0048 | (USHORT)grr_random_value(PCR_RND_INDEX, 8) );
#else
  return PAGE_RESPONSE_MASK_8BIT;
#endif
}

LOCAL SHORT tc_cell_update_mask_8bit  (void)
{
#ifdef _TARGET_
  return (0x0050 | (USHORT)grr_random_value(PCR_RND_INDEX, 8) );
#else
  return CELL_UPDATE_MASK_8BIT;
#endif
}

LOCAL SHORT tc_mm_procedure_mask_8bit  (void)
{
#ifdef _TARGET_
  return (0x0058 | (USHORT)grr_random_value(PCR_RND_INDEX, 8) );
#else
  return MM_PROCEDURE_MASK_8BIT;
#endif
}
LOCAL SHORT tc_single_block_mask_8bit  (void)
{
#ifdef _TARGET_
  return (0x0060 | (USHORT)grr_random_value(PCR_RND_INDEX, 8) );
#else
  return SINGLE_BLOCK_MASK_8BIT;
#endif
}

/*
+------------------------------------------------------------------------------
| Function    : tc_set_hopping_par
+------------------------------------------------------------------------------
| Description : This function handles the frequency parameters in a ul or dl
|               assignment or ts reconfigure.
|
| Parameters  : ptr2prim - pointer to the MPHP_ASSIGNMENT_REQ prim
|                          that has to be set up.
|               freq_par - pointer to the received frequency parameter struct.
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL tc_set_hopping_par ( T_freq_par *freq_par )
{
  BOOL              result = FALSE;
  T_p_frequency_par frequency_par;

  TRACE_FUNCTION( "tc_set_hopping_par" );

  memset( &frequency_par, 0, sizeof( frequency_par ) );

  if(freq_par->v_indi_encod)
  {    
    result = grr_validate_ma_num_in_freq_par(freq_par,GRR_FREQ_PARAM_RECEIVED_IN_ASSIGNMENT);

    if(result)
    {
      result = grr_create_freq_list(  freq_par->indi_encod.ma_num,
                                      freq_par->indi_encod.maio,
                                     &frequency_par.p_chan_sel,
                                     &frequency_par.p_freq_list );
    }
    else
    {
      TRACE_ERROR("error creating frequency list");
      result = FALSE;
    }

  } /* if(freq_par->v_indi_encod) */
  else if(freq_par->v_di_encod1)
  {
    memcpy(&psc_db->gprs_ms_alloc_in_assignment.gprs_ms_alloc_ie,
           &freq_par->di_encod1.gprs_ms_alloc_ie, sizeof(T_gprs_ms_alloc_ie));

    psc_db->gprs_ms_alloc_in_assignment.ma_num = MA_NUMBER_4_ASSIGNMENT;
    psc_db->rfl[MAX_RFL].num                   = NOT_SET;

    result = grr_create_freq_list(  psc_db->gprs_ms_alloc_in_assignment.ma_num,
                                    freq_par->di_encod1.maio,
                                   &frequency_par.p_chan_sel,
                                   &frequency_par.p_freq_list );
  } /* if(freq_par->v_di_encod1) */
  else if(freq_par->v_di_encod2)
  {
    /* 
     * processing of T_freq_par from assignment and non-assignment messages
     * when direct encoding 2 is used, should be done in same object module as 
     * this involvs use of static array variables which are local to the 
     * object module they are used in(_local_dummy_list and _local_rfl_contents.
     * Hence processing for both assignment and non-assignement messages is done
     * in grr_f.c. If they are put in different files, we will have to duplicate
     * the static arrays.
     */
    result = 
      grr_cnv_freq_para_in_assignment_direct_enc_2(&frequency_par,freq_par);

  } /* if(freq_par->v_di_encod2) */
  else
  {
    TRACE_ERROR("FATAL ERROR: no frequency params in ul assignment!");
  }

  if( result EQ TRUE )
  {
    grr_data->tc.freq_set.freq_par = frequency_par;
    grr_data->tc.freq_set.tsc      = freq_par->tsc;
  }

  grr_data->tc.v_freq_set = result;

  return ( result );
}/* tc_set_hopping_par */



/*
+------------------------------------------------------------------------------
| Function    : tc_get_send_nb_prim_and_set_freq
+------------------------------------------------------------------------------
| Description : This function allocates a primitve depending on the hopping status
|               and sets the frequency parameter of T_MPHP_SINGLE_BLOCK_REQ
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
LOCAL T_MPHP_SINGLE_BLOCK_REQ * tc_get_send_nb_prim_and_set_freq ( void )
{
  PALLOC( mphp_single_block_req, MPHP_SINGLE_BLOCK_REQ );

  TRACE_FUNCTION( "tc_get_send_nb_prim_and_set_freq" );

  memset( mphp_single_block_req, 0, sizeof( T_MPHP_SINGLE_BLOCK_REQ ) );

  mphp_single_block_req->tsc = grr_data->tc.freq_set.tsc;

  grr_set_freq_par( &mphp_single_block_req->p_frequency_par );

  return( mphp_single_block_req );
}/* tc_get_send_nb_prim_and_set_freq */

/*
+------------------------------------------------------------------------------
| Function    : tc_check_and_save_freq
+------------------------------------------------------------------------------
| Description : This function check if the freq_par is valid or not..
|               If it is hopping case, save the hoppinging freq. 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
LOCAL BOOL tc_check_and_save_freq (void)
{
  MCAST(d_dl_assign,D_DL_ASSIGN);  /* T_D_DL_ASSIGN T_D_UL_ASSIGN T_D_TS_RECONFIG */
  MCAST(d_ul_assign,D_UL_ASSIGN);
  MCAST(d_ts_reconfig,D_TS_RECONFIG);

  BOOL      result = FALSE;
  T_freq_par   *freq_par = NULL;
  BOOL      mode_check = FALSE;

  TRACE_FUNCTION( "tc_check_and_save_freq" );

  /*
   * store frequeny parameters, if present.
   * If not present, take from data base
   */
  switch(d_dl_assign->msg_type)
  {
  case D_DL_ASSIGN:  /* T_D_DL_ASSIGN  */
    if(d_dl_assign->pda_trnc_grp.v_freq_par)
    {  /* replace possibly earlier stored freq params */
      freq_par = &d_dl_assign->pda_trnc_grp.freq_par;
    }
    if((grr_data->tbf_type EQ CGRLC_TBF_MODE_UL) OR (grr_data->tbf_type EQ CGRLC_TBF_MODE_DL_UL) )
    {
      mode_check = TRUE;
    }
    break;
  case D_UL_ASSIGN:  /* T_D_UL_ASSIGN */
      if(d_ul_assign->v_freq_par)
      {  /* replace possibly earlier stored freq params */
        freq_par = &d_ul_assign->freq_par;
      }
      if((grr_data->tbf_type EQ CGRLC_TBF_MODE_DL) OR (grr_data->tbf_type EQ CGRLC_TBF_MODE_DL_UL))
      {
        mode_check = TRUE;
      }

    break;
  case D_TS_RECONFIG:  /* T_D_TS_RECONFIG */
      if(d_ts_reconfig->v_freq_par)
      {  /* replace possibly earlier stored freq params */
        freq_par = &d_ts_reconfig->freq_par;
      }
    break;
  case D_UL_ACK:  /* T_D_UL_ACK */
    break;
  default:
    TRACE_ERROR ( "Wrong message type in _decodedMsg" );
    break;

  }
    /*  If frequency parameter element present, assigns frequency parameters to the 
        uplink TBF. If this information element is not present the mobile station 
        shall use its previously assigned frequency parameters.*/

  if( freq_par )/* save the hopping frequency */
  {
    if(freq_par->v_arfcn) /*static*/
    {
      T_FREQ_SET temp_freq_set;
      temp_freq_set.freq_par.p_chan_sel.hopping      = FALSE;
      temp_freq_set.tsc   = freq_par->tsc;
      temp_freq_set.freq_par.p_chan_sel.p_rf_ch.arfcn = grr_g23_arfcn_to_l1(freq_par->arfcn);
      
      /* variable needed by hopping case must be reset.*/
      memset(&(temp_freq_set.freq_par.p_freq_list.p_rf_chan_no.p_radio_freq), 0, sizeof(temp_freq_set.freq_par.p_freq_list.p_rf_chan_no.p_radio_freq));
      temp_freq_set.freq_par.p_freq_list.p_rf_chan_cnt = 0;

      if ( mode_check EQ TRUE ) 
      {                              
        /* checking the params already exist or not */
        if ( ( grr_data->tc.freq_set.freq_par.p_chan_sel.hopping EQ FALSE)
			 AND (temp_freq_set.tsc EQ  grr_data->tc.freq_set.tsc) 
			 AND ( temp_freq_set.freq_par.p_chan_sel.p_rf_ch.arfcn EQ 
			 grr_data->tc.freq_set.freq_par.p_chan_sel.p_rf_ch.arfcn ) )
        {
          TRACE_EVENT ("Frequency parameters are same");
          result = TRUE;
        }
        else
        {
          TRACE_EVENT ("Frequency parameters are changed, So discard it");
          result = FALSE;
        }
      }
      else
      {				           
        /* If it is new entry, storing in the grr_data */
        result = TRUE;
        grr_data->tc.v_freq_set = TRUE;
        grr_data->tc.freq_set = temp_freq_set;
      }
    }
    else
    { /* hopping cases */
      result = tc_set_hopping_par( freq_par );
    }
  }
  else if ( grr_data->tc.v_freq_set )/* if it is a two phase assignment or there is already another TBF running*/
  {
    result          = TRUE;
  }
  else
  {
    result          = FALSE;
    TRACE_ERROR("FATAL ERROR: no freq params in assignment but needed!");
  }

  if(result EQ TRUE)
  {
    grr_data->pwr_ctrl_valid_flags.v_glbl_pwr_ctrl_param = TRUE;
    grr_data->pwr_ctrl_valid_flags.v_freq_param          = TRUE;
  }

  return(result);

}/* tc_check_and_save_freq */




/*
+------------------------------------------------------------------------------
| Function    : tc_set_da_assignment
+------------------------------------------------------------------------------
| Description : This function sets some parameter of T_MPHP_ASSIGNMENT_REQ is
|               case of uplink dynamic allocation.
|
| Parameters  : ptr2prim_i -  ptr to MPHP_ASSIGNMENT_REQ
|
+------------------------------------------------------------------------------
*/
LOCAL void tc_set_da_assignment (T_MPHP_ASSIGNMENT_REQ * ptr2prim_i)
{
  MCAST(d_ul_assign,D_UL_ASSIGN);/* T_D_UL_ASSIGN */
  MCAST(d_ts_reconfig,D_TS_RECONFIG);

  TRACE_FUNCTION( "tc_set_da_assignment" );

  if( d_ul_assign->msg_type EQ D_UL_ASSIGN_c)
  {

    /*
     * timing advance
     */
    grr_handle_ta ( d_ul_assign->pta.v_ta_value, 
          d_ul_assign->pta.ta_value,
          d_ul_assign->pta.v_ta_index_tn, 
          d_ul_assign->pta.ta_index_tn.ta_index,
          d_ul_assign->pta.ta_index_tn.ta_tn,
          0xFF,
          0,
          0,
          &ptr2prim_i->p_timing_advance);

    /* bts output power control */
    if(d_ul_assign->dyn_alloc_p.v_p0)
    {
      ptr2prim_i->p_dl_power.p0 = d_ul_assign->dyn_alloc_p.p0;
      ptr2prim_i->p_dl_power.pr_mode = d_ul_assign->dyn_alloc_p.pr_mode;
      /* dynamic allocation must use mode a */
      ptr2prim_i->p_dl_power.bts_pwr_ctl_mode = MODE_A;
    }
    else
    {
      ptr2prim_i->p_dl_power.p0 = 0xff; /* 0xff -> invalid value */
      /* -> pr_mode and bts_pwr_ctl_mode don't care */
    }

    /*
     * TBF starting time
     */
    if(d_ul_assign->dyn_alloc_p.v_tbf_s_time)
    {
      ptr2prim_i->p_tbf_start.tbf_start_present = 1;
      if(d_ul_assign->dyn_alloc_p.tbf_s_time.v_abs)
      {        
        ptr2prim_i->p_tbf_start.fn = grr_decode_tbf_start_abs(&d_ul_assign->dyn_alloc_p.tbf_s_time.abs);       
      }
      else if (d_ul_assign->dyn_alloc_p.tbf_s_time.v_rel)
      { 
        ptr2prim_i->p_tbf_start.fn = grr_decode_tbf_start_rel(grr_data->dl_fn,
                                                              d_ul_assign->dyn_alloc_p.tbf_s_time.rel);   
      }
      else
      {  /* this shall never happen */
        ptr2prim_i->p_tbf_start.fn = grr_data->dl_fn;   
        TRACE_ERROR("UL v_start_tbf but either rel nor abs tbf included");
      }
    }
    else if(grr_data->l1_del_tbf_start_fn NEQ GRR_INVALID_FN)
    {
      ptr2prim_i->p_tbf_start.tbf_start_present = 1; 
      ptr2prim_i->p_tbf_start.fn = grr_data->l1_del_tbf_start_fn;	  
    }
    else
    {
      ptr2prim_i->p_tbf_start.tbf_start_present = 0; 
    }

   /*
    * mac mode
    */
    if (d_ul_assign->dyn_alloc_p.xdyn_alloc)
    {
      ptr2prim_i->mac_mode = EDA;
    }
    else
    {
      ptr2prim_i->mac_mode = DA;
    }

   /*
    * uplink allocation structure
    */
    if(d_ul_assign->dyn_alloc_p.v_ul_tfi_assign)
    {
      ptr2prim_i->p_ul_alloc.ul_tfi = d_ul_assign->dyn_alloc_p.ul_tfi_assign;
    }
    else
    {
      ptr2prim_i->p_ul_alloc.ul_tfi = grr_data->uplink_tbf.tfi;
    }
    ptr2prim_i->p_ul_alloc.ts_mask = grr_data->uplink_tbf.ts_usage;


   /*
    *  dynamic allocation structure
    */
    ptr2prim_i->p_ul_alloc.p_dynamic_alloc.usf_gran =
                                           d_ul_assign->dyn_alloc_p.usf_grant;

    if(d_ul_assign->dyn_alloc_p.v_tn_alloc EQ 1)
    {
      UBYTE i;
      for(i = 0;i < 8;i++)
      {
        if(d_ul_assign->dyn_alloc_p.tn_alloc[i].v_usf)
        {
          ptr2prim_i->p_ul_alloc.p_dynamic_alloc.usf_table[i] =
                                     d_ul_assign->dyn_alloc_p.tn_alloc[i].usf;
        }
      }
    }
    else if (d_ul_assign->dyn_alloc_p.v_tn_alloc_pwr)
    {
      UBYTE i;

      for(i = 0;i < 8;i++)
      {
        if(d_ul_assign->dyn_alloc_p.tn_alloc_pwr.usf_array[i].v_usf_g)
        {
          ptr2prim_i->p_ul_alloc.p_dynamic_alloc.usf_table[i] =
                 d_ul_assign->dyn_alloc_p.tn_alloc_pwr.usf_array[i].usf_g.usf;
        }
      }

      grr_store_type_tn_alloc_pwr( &d_ul_assign->dyn_alloc_p.tn_alloc_pwr );
    }
    else
    {
      ptr2prim_i->p_ul_alloc.p_dynamic_alloc.usf_table[0] = 5;
      TRACE_EVENT("USF-Error: set (hardcoded) USF_TN0 = 0x05");
    }

  }
  else if ( d_ts_reconfig->msg_type EQ D_TS_RECONFIG_c)
  {
    /*
     * timing advance is handled in tc_set_dl_assignment
     */
  /*  grr_handle_ta ( d_ts_reconfig->gpta.v_ta_value, 
          d_ts_reconfig->gpta.ta_value,
          d_ts_reconfig->gpta.v_ul_ta_index, 
          d_ts_reconfig->gpta.ul_ta_index,
          d_ts_reconfig->gpta.ul_ta_tn,
          0,
          0,
          0,
          ptr2prim_i->p_timing_advance);

          */
    /* bts output power control */
    if(d_ts_reconfig->dyn_alloc_ts.v_p0)
    {
      ptr2prim_i->p_dl_power.p0 = d_ts_reconfig->dyn_alloc_ts.p0;
      ptr2prim_i->p_dl_power.pr_mode = d_ts_reconfig->dyn_alloc_ts.pr_mode;
      /* dynamic allocation must use mode a */
      ptr2prim_i->p_dl_power.bts_pwr_ctl_mode = MODE_A;
    }
    else
    {
      ptr2prim_i->p_dl_power.p0 = 0xff; /* 0xff -> invalid value */
      /* -> pr_mode and bts_pwr_ctl_mode don't care */
    }

    /*
     * TBF starting time
     */    
    if(d_ts_reconfig->dyn_alloc_ts.v_tbf_s_time)
    {
      ptr2prim_i->p_tbf_start.tbf_start_present= 1;
      if(d_ts_reconfig->dyn_alloc_ts.tbf_s_time.v_abs)
      {
        ptr2prim_i->p_tbf_start.fn = grr_decode_tbf_start_abs(&d_ts_reconfig->dyn_alloc_ts.tbf_s_time.abs);              
      }
      else if (d_ts_reconfig->dyn_alloc_ts.tbf_s_time.v_rel)
      {
        ptr2prim_i->p_tbf_start.fn = grr_decode_tbf_start_rel(grr_data->dl_fn,
                                                              d_ts_reconfig->dyn_alloc_ts.tbf_s_time.rel);        
      }
      else
      {  /* this shall never happen */
        ptr2prim_i->p_tbf_start.fn = grr_data->dl_fn%FN_MAX;  
        TRACE_ERROR("TS RECON UL v_start_tbf but either rel nor abs tbf included");
      }
    }
    else if(grr_data->l1_del_tbf_start_fn NEQ GRR_INVALID_FN)
    {
      ptr2prim_i->p_tbf_start.tbf_start_present = 1; 
      ptr2prim_i->p_tbf_start.fn = grr_data->l1_del_tbf_start_fn;	  
    }
    else
    {
      ptr2prim_i->p_tbf_start.tbf_start_present = 0; 
    }


   /*
    * mac mode
    */
    if (d_ts_reconfig->dyn_alloc_ts.xdyn_alloc)
    {
      ptr2prim_i->mac_mode = EDA;
    }
    else
    {
      ptr2prim_i->mac_mode = DA;
    }

   /*
    * uplink allocation structure
    */
    if(d_ts_reconfig->v_ul_tfi)
    {
      ptr2prim_i->p_ul_alloc.ul_tfi = d_ts_reconfig->ul_tfi;
    }
    else
    {
      ptr2prim_i->p_ul_alloc.ul_tfi = grr_data->uplink_tbf.tfi;
    }
    ptr2prim_i->p_ul_alloc.ts_mask = grr_data->uplink_tbf.ts_usage;


   /*
    *  dynamic allocation structure
    */
    ptr2prim_i->p_ul_alloc.p_dynamic_alloc.usf_gran = d_ts_reconfig->dyn_alloc_ts.usf_grant;

    if(d_ts_reconfig->dyn_alloc_ts.v_tn_alloc EQ 1)
    {
      UBYTE i;
      for(i = 0;i < 8;i++)
      {
        if(d_ts_reconfig->dyn_alloc_ts.tn_alloc[i].v_usf)
        {
          ptr2prim_i->p_ul_alloc.p_dynamic_alloc.usf_table[i] =
                                   d_ts_reconfig->dyn_alloc_ts.tn_alloc[i].usf;
        }
      }
    }
    else if (d_ts_reconfig->dyn_alloc_ts.v_tn_alloc_pwr)
    {
      UBYTE i;

      for(i = 0;i < 8;i++)
      {
        if(d_ts_reconfig->dyn_alloc_ts.tn_alloc_pwr.usf_array[i].v_usf_g)
        {
          ptr2prim_i->p_ul_alloc.p_dynamic_alloc.usf_table[i] =
               d_ts_reconfig->dyn_alloc_ts.tn_alloc_pwr.usf_array[i].usf_g.usf;
        }
      }

      grr_store_type_tn_alloc_pwr( &d_ts_reconfig->dyn_alloc_ts.tn_alloc_pwr );
    }
    else
    {
      ptr2prim_i->p_ul_alloc.p_dynamic_alloc.usf_table[0] = 5;
      TRACE_EVENT("USF-Error: set (hardcoded) USF_TN0 = 0x05");
    }
  }
  else
  {
    TRACE_ERROR ( "Wrong message type in _decodedCtrlMsg" );
  }

  if(ptr2prim_i->p_tbf_start.tbf_start_present)
    grr_data->uplink_tbf.tbf_start_fn = ptr2prim_i->p_tbf_start.fn;
  else
    grr_data->uplink_tbf.tbf_start_fn = CGRLC_STARTING_TIME_NOT_PRESENT;

  grr_data->uplink_tbf.st_tfi = ptr2prim_i->p_ul_alloc.ul_tfi;



  return;
}/*tc_set_da_assignment*/


/*
+------------------------------------------------------------------------------
| Function    : tc_calc_fa_bitmap
+------------------------------------------------------------------------------
| Description : This function calculates rhe allocation bitmap in
|               case of fixed allocation
|
| Parameters  : blks_or_blkps  - indicates if blocks(0) or block periods(1) is used
|               len            - len of the allocation bitmap
|               ptr2_alloc_map - ptr to the allocation bitmap array
|               ptr2_fix_alloc - ptr to the fixed allocation struct
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_calc_fa_bitmap (ULONG tbf_start,
                               UBYTE blks_or_blkps,
                               USHORT len,
                               UBYTE * ptr2_alloc_map,
                               T_p_fixed_alloc  * ptr2_fix_alloc)
{
  UBYTE i;
  UBYTE help;
  UBYTE tn[8];
  UBYTE x,y;
  UBYTE len_1;
  USHORT index;

  TRACE_FUNCTION( "tc_calc_fa_bitmap" );

/*
  TRACE_EVENT_P9("FA bitmap: start_fn=%ld, blk=%d  len=%ld  bitmap: %d %d %d %d %d %d",
                                  tbf_start,
                                  blks_or_blkps,
                                  len,
                                  ptr2_alloc_map[0],
                                  ptr2_alloc_map[1],
                                  ptr2_alloc_map[2],
                                  ptr2_alloc_map[3],
                                  ptr2_alloc_map[4],
                                  ptr2_alloc_map[5]);

  TRACE_EVENT_P9("%d %d %d %d %d %d %d %d %d",
                                  ptr2_alloc_map[6],
                                  ptr2_alloc_map[7],
                                  ptr2_alloc_map[8],
                                  ptr2_alloc_map[9],
                                  ptr2_alloc_map[10],
                                  ptr2_alloc_map[11],
                                  ptr2_alloc_map[12],
                                  ptr2_alloc_map[13],
                                  ptr2_alloc_map[14]);

  TRACE_EVENT_P9("%d %d %d %d %d %d %d %d %d",
                                  ptr2_alloc_map[15],
                                  ptr2_alloc_map[16],
                                  ptr2_alloc_map[17],
                                  ptr2_alloc_map[18],
                                  ptr2_alloc_map[19],
                                  ptr2_alloc_map[20],
                                  ptr2_alloc_map[21],
                                  ptr2_alloc_map[22],
                                  ptr2_alloc_map[23]);
  TRACE_EVENT_P9("%d %d %d %d %d %d %d %d %d",
                                  ptr2_alloc_map[24],
                                  ptr2_alloc_map[25],
                                  ptr2_alloc_map[26],
                                  ptr2_alloc_map[27],
                                  ptr2_alloc_map[28],
                                  ptr2_alloc_map[29],
                                  ptr2_alloc_map[30],
                                  ptr2_alloc_map[31],
                                  ptr2_alloc_map[32]);
*/
  memset( tn, 0, sizeof(tn) );

  if(blks_or_blkps)
  {
    /*
     * block periods are used
     */
    for(i=0;i<len;i++)
    {
      if(ptr2_alloc_map[i])
      {
        ptr2_fix_alloc->bitmap[i] = grr_data->uplink_tbf.ts_usage;
      }
      else
        ptr2_fix_alloc->bitmap[i] = 0;
    }
    ptr2_fix_alloc->size_bitmap = (UBYTE)len;
  }
  else
  {
    UBYTE L;
    /*
     * blocks are used
     */
    /*
     * determinate the timeslot positions
     */
    help = grr_data->uplink_tbf.ts_usage;
    index = 0;
    for(i=0;i<=7;i++)
    {
      if(help >= (0x80 >> i))
      {
        tn[index]=i;
        index++;
        help -= (0x80 >> i);
      }
    }
    memset(ptr2_fix_alloc->bitmap,0,sizeof(ptr2_fix_alloc->bitmap));
    L = len - 1;
    for(i=0;i<=L;i++)
    {
      if(ptr2_alloc_map[i])
      {
        x = i / grr_data->uplink_tbf.nts;
        y = i % grr_data->uplink_tbf.nts;
        ptr2_fix_alloc->bitmap[x] |= 0x80 >> tn[y];
      }
    }
    ptr2_fix_alloc->size_bitmap = (UBYTE)len / grr_data->uplink_tbf.nts;
  }
  /*
   * set fa_ctrl
   */


  grr_data->tc.fa_ctrl.fa_type = FA_BITMAP;


  grr_data->tc.fa_ctrl.current_alloc.alloc_start_fn = tbf_start;

  /* store end of fixed allocation */
  len_1 = ptr2_fix_alloc->size_bitmap - 1;
  grr_data->tc.fa_ctrl.current_alloc.alloc_end_fn =
    grr_decode_tbf_start_rel(grr_data->tc.fa_ctrl.current_alloc.alloc_start_fn,(USHORT)(len_1-1));

  memcpy( &grr_data->tc.fa_ctrl.current_alloc.alloc,
            ptr2_fix_alloc,
            sizeof(T_p_fixed_alloc));

 /* TRACE_EVENT_P9("L1 bitmap %d %d %d %d %d %d %d %d %d",
                                  ptr2_fix_alloc->bitmap[0],
                                  ptr2_fix_alloc->bitmap[1],
                                  ptr2_fix_alloc->bitmap[2],
                                  ptr2_fix_alloc->bitmap[3],
                                  ptr2_fix_alloc->bitmap[4],
                                  ptr2_fix_alloc->bitmap[5],
                                  ptr2_fix_alloc->bitmap[6],
                                  ptr2_fix_alloc->bitmap[7],
                                  ptr2_fix_alloc->bitmap[8]);

  TRACE_EVENT_P9("%d %d %d %d %d %d %d %d %d",
                                  ptr2_fix_alloc->bitmap[9],
                                  ptr2_fix_alloc->bitmap[10],
                                  ptr2_fix_alloc->bitmap[11],
                                  ptr2_fix_alloc->bitmap[12],
                                  ptr2_fix_alloc->bitmap[13],
                                  ptr2_fix_alloc->bitmap[14],
                                  ptr2_fix_alloc->bitmap[15],
                                  ptr2_fix_alloc->bitmap[16],
                                  ptr2_fix_alloc->bitmap[17]);
  TRACE_EVENT_P9("%d %d %d %d %d %d %d %d %d",
                                  ptr2_fix_alloc->bitmap[18],
                                  ptr2_fix_alloc->bitmap[19],
                                  ptr2_fix_alloc->bitmap[20],
                                  ptr2_fix_alloc->bitmap[21],
                                  ptr2_fix_alloc->bitmap[22],
                                  ptr2_fix_alloc->bitmap[23],
                                  ptr2_fix_alloc->bitmap[24],
                                  ptr2_fix_alloc->bitmap[25],
                                  ptr2_fix_alloc->bitmap[26]);
*/
} /* tc_calc_fa_bitmap() */

/*
+------------------------------------------------------------------------------
| Function    : tc_handle_final_alloc
+------------------------------------------------------------------------------
| Description : This function handles the final allocation bit.
|
| Parameters  :   -
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_handle_final_alloc (UBYTE final_allocation)
{

  TRACE_FUNCTION( "tc_handle_final_alloc" );



} /* tc_handle_final_alloc() */

/*
+------------------------------------------------------------------------------
| Function    : tc_set_fa_assignment
+------------------------------------------------------------------------------
| Description : This function sets some parameter of T_MPHP_ASSIGNMENT_REQ is
|               case of fixed allocation
|
| Parameters  : ptr2prim_i -  ptr to MPHP_ASSIGNMENT_REQ
|
+------------------------------------------------------------------------------
*/
LOCAL void tc_set_fa_assignment (T_MPHP_ASSIGNMENT_REQ * ptr2prim_i)
{
/*  ULONG var; */
  MCAST(d_ul_assign,D_UL_ASSIGN);     /* T_D_UL_ASSIGN */
  MCAST(d_ts_reconfig,D_TS_RECONFIG); /* T_TS_RECONFIG */
  MCAST(d_ul_ack,D_UL_ACK);           /* T_D_UL_ACK */




  TRACE_FUNCTION( "tc_set_fa_assignment" );

#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
  if((d_ul_assign->msg_type EQ D_UL_ASSIGN_c) AND 
     (ptr2prim_i->assign_cmd EQ UL_TP_ACCESS))
  {
    /* training sequence code */
    ptr2prim_i->tsc = d_ul_assign->freq_par.tsc;

    /* handle timing advance */
    grr_handle_ta ( d_ul_assign->pta.v_ta_value, 
      d_ul_assign->pta.ta_value,
      d_ul_assign->pta.v_ta_index_tn, 
      d_ul_assign->pta.ta_index_tn.ta_index,
      d_ul_assign->pta.ta_index_tn.ta_tn,
      0xFF,
      0,
      0,
      &ptr2prim_i->p_timing_advance);

    /* Mac mode */	
    ptr2prim_i->mac_mode = FA;

    /* Handle single block allocation */
    if(d_ul_assign->v_sin_alloc EQ TRUE )
    {
      /* timeslot mask */
      ptr2prim_i->p_ul_alloc.ts_mask = 0x80>>d_ul_assign->sin_alloc.tn;

      /* PO, BTS_POWER_MODE and PR_ MODE */
      if(d_ul_assign->sin_alloc.v_bts_pwr_ctrl EQ TRUE  )
      {
        ptr2prim_i->p_dl_power.p0 = 
          d_ul_assign->sin_alloc.bts_pwr_ctrl.p0;

        ptr2prim_i->p_dl_power.bts_pwr_ctl_mode =
          d_ul_assign->sin_alloc.bts_pwr_ctrl.mode;

        ptr2prim_i->p_dl_power.pr_mode = 
          d_ul_assign->sin_alloc.bts_pwr_ctrl.pr_mode;
      }
      else
      {
        ptr2prim_i->p_dl_power.p0 = 0xff;
      }

      /* Downlink Control Timeslot */
      ptr2prim_i->p_ul_alloc.p_fixed_alloc.dl_ctrl_ts = d_ul_assign->sin_alloc.tn;

      /*
       * handle TBF starting time
       * is always present in single block allocation
       */
      if(d_ul_assign->sin_alloc.tbf_s_time.v_abs)
      {
        ptr2prim_i->p_tbf_start.fn = grr_decode_tbf_start_abs(&d_ul_assign->sin_alloc.tbf_s_time.abs);
        if(!grr_check_if_tbf_start_is_elapsed ( ptr2prim_i->p_tbf_start.fn, grr_data->ul_fn))
        {
          /*Allocated Number of time slots are 1 */
          ptr2prim_i->p_ul_alloc.p_fixed_alloc.size_bitmap = 1;
          ptr2prim_i->p_tbf_start.tbf_start_present=TRUE;
          ptr2prim_i->p_ul_alloc.p_fixed_alloc.bitmap[0] = 0x80 >> d_ul_assign->sin_alloc.tn;
        }
        else
        {
          TRACE_ERROR("TBF Starting time is ELAPSED in single block allocation!");
        }
      }
      else if (d_ul_assign->sin_alloc.tbf_s_time.v_rel)
      {
        ptr2prim_i->p_tbf_start.fn = grr_decode_tbf_start_rel(grr_data->dl_fn, 
                                                           (USHORT)(d_ul_assign->sin_alloc.tbf_s_time.rel));
        if(!grr_check_if_tbf_start_is_elapsed ( ptr2prim_i->p_tbf_start.fn, grr_data->ul_fn))
        {
          /*Allocated Number of time slots are 1 */
          ptr2prim_i->p_ul_alloc.p_fixed_alloc.size_bitmap = 1;
          ptr2prim_i->p_tbf_start.tbf_start_present=TRUE;
          ptr2prim_i->p_ul_alloc.p_fixed_alloc.bitmap[0] = 0x80 >> d_ul_assign->sin_alloc.tn;
        }
        else
        {
          TRACE_ERROR("TBF Starting time is ELAPSED in single block allocation!");
        }
      }
	}/* single block */
  }
  else
#endif
  if( d_ul_assign->msg_type EQ D_UL_ASSIGN_c)
  {
    /*
     * timing advance
     */

    grr_handle_ta ( d_ul_assign->pta.v_ta_value, 
          d_ul_assign->pta.ta_value,
          d_ul_assign->pta.v_ta_index_tn, 
          d_ul_assign->pta.ta_index_tn.ta_index,
          d_ul_assign->pta.ta_index_tn.ta_tn,
          0xFF,
          0,
          0,
          &ptr2prim_i->p_timing_advance);

    if(d_ul_assign->f_alloc_ul.v_bts_pwr_ctrl)
    {
      ptr2prim_i->p_dl_power.p0 = d_ul_assign->f_alloc_ul.bts_pwr_ctrl.p0;
      ptr2prim_i->p_dl_power.bts_pwr_ctl_mode =
                                    d_ul_assign->f_alloc_ul.bts_pwr_ctrl.mode;
      ptr2prim_i->p_dl_power.pr_mode =
                                 d_ul_assign->f_alloc_ul.bts_pwr_ctrl.pr_mode;
    }
    else
    {
      ptr2prim_i->p_dl_power.p0 = 0xff;
    }

    /*
     * mac mode
     */
    ptr2prim_i->mac_mode = FA;
    /*
     * uplink allocation structure
     */
    if(d_ul_assign->f_alloc_ul.v_ul_tfi_assign)
    {
      ptr2prim_i->p_ul_alloc.ul_tfi = d_ul_assign->f_alloc_ul.ul_tfi_assign;
    }
    else
    {
      ptr2prim_i->p_ul_alloc.ul_tfi = grr_data->uplink_tbf.tfi;
    }
    ptr2prim_i->p_ul_alloc.ts_mask = grr_data->uplink_tbf.ts_usage;
    /*
     *  fixed allocation structure
     *  half duplex mode is ignored, only valid for MS class 19 to 29
     */

    /*
     *  TBF STARTING TIME for fixed allocation structure
     */    
    if(d_ul_assign->f_alloc_ul.tbf_s_time.v_abs)
    {
      ptr2prim_i->p_tbf_start.fn = grr_decode_tbf_start_abs(&d_ul_assign->f_alloc_ul.tbf_s_time.abs);
    }
    else if (d_ul_assign->f_alloc_ul.tbf_s_time.v_rel)
    {
      ptr2prim_i->p_tbf_start.fn = grr_decode_tbf_start_rel(grr_data->dl_fn,
                                                              d_ul_assign->f_alloc_ul.tbf_s_time.rel);
    }
    else
    {  /* this shall never happen */
      ptr2prim_i->p_tbf_start.fn = (grr_data->dl_fn+13)%FN_MAX;       
      TRACE_ERROR("NO STARTING TIME IN FIXED ALLOCATION!!");
    }
    ptr2prim_i->p_tbf_start.tbf_start_present = 1;
    /*
     *  Downlink Control Timeslot
     */
    ptr2prim_i->p_ul_alloc.p_fixed_alloc.dl_ctrl_ts = d_ul_assign->f_alloc_ul.dl_ctrl_ts;
    grr_data->tc.fa_ctrl.dl_ctrl_ts                 = d_ul_assign->f_alloc_ul.dl_ctrl_ts;
    /*
     * ALLOCATION BITMAP
     */
    if(d_ul_assign->f_alloc_ul.v_block_struct)
    {
      tc_calc_fa_bitmap(ptr2prim_i->p_tbf_start.fn,                      /*starting time of cuurent tbf*/
                        d_ul_assign->f_alloc_ul.block_struct.bl_o_bl_per,/* blocks or block periods*/
                        d_ul_assign->f_alloc_ul.block_struct.a_map_len,  /* bitmap length*/
                        d_ul_assign->f_alloc_ul.block_struct.alloc_map,  /* ptr to alloc struct*/
                        &ptr2prim_i->p_ul_alloc.p_fixed_alloc);          /* ptr to fix sttruct*/
    }
    else if(d_ul_assign->f_alloc_ul.v_alloc_map)
    {
      tc_calc_fa_bitmap(ptr2prim_i->p_tbf_start.fn,                      /*starting time of cuurent tbf*/
                        0,                                               /* blocks : default*/
                        d_ul_assign->f_alloc_ul.c_alloc_map,             /* bitmap length*/
                        d_ul_assign->f_alloc_ul.alloc_map,               /* ptr to alloc struct*/
                        &ptr2prim_i->p_ul_alloc.p_fixed_alloc);          /* ptr to fix sttruct*/
    }
    /*
     * check final allocation
     */
    grr_data->tc.fa_ctrl.current_alloc.final_alloc = d_ul_assign->f_alloc_ul.final_alloc;
  }
  else if ( d_ts_reconfig->msg_type EQ D_TS_RECONFIG_c)
  {
    /*
     * timing advance is handled in tc_set_dl_assignment
     */

    if(d_ts_reconfig->f_alloc_re.v_bts_pwr_ctrl)
    {
      ptr2prim_i->p_dl_power.p0 = d_ts_reconfig->f_alloc_re.bts_pwr_ctrl.p0;
      ptr2prim_i->p_dl_power.bts_pwr_ctl_mode =
                                    d_ts_reconfig->f_alloc_re.bts_pwr_ctrl.mode;
      ptr2prim_i->p_dl_power.pr_mode =
                                 d_ts_reconfig->f_alloc_re.bts_pwr_ctrl.pr_mode;
    }
    else
    {
      ptr2prim_i->p_dl_power.p0 = 0xff;
    }

    /*
     * mac mode
     */
    ptr2prim_i->mac_mode = FA;
    /*
     * uplink allocation structure
     */
    if(d_ts_reconfig->v_ul_tfi)
    {
      ptr2prim_i->p_ul_alloc.ul_tfi = d_ts_reconfig->ul_tfi;
    }
    else
    {
      ptr2prim_i->p_ul_alloc.ul_tfi = grr_data->uplink_tbf.tfi;
    }
    ptr2prim_i->p_ul_alloc.ts_mask = grr_data->uplink_tbf.ts_usage;
    /*
     *  fixed allocation structure
     *  half duplex mode is ignored, only valid for MS class 19 to 29
     */
    /*
     *  TBF STARTING TIME for fixed allocation structure
     */
    if(d_ts_reconfig->f_alloc_re.tbf_s_time.v_abs)
    {
      ptr2prim_i->p_tbf_start.fn = grr_decode_tbf_start_abs(&d_ts_reconfig->f_alloc_re.tbf_s_time.abs);
    }
    else if (d_ts_reconfig->f_alloc_re.tbf_s_time.v_rel)
    {
      ptr2prim_i->p_tbf_start.fn = grr_decode_tbf_start_rel(grr_data->dl_fn,
                                                              d_ts_reconfig->f_alloc_re.tbf_s_time.rel);
    }
    else
    {  /* this shall never happen */
      ptr2prim_i->p_tbf_start.fn = (grr_data->dl_fn+13)%FN_MAX;       
      TRACE_ERROR("NO STARTING TIME IN FIXED ALLOCATION!!");
    }
    ptr2prim_i->p_tbf_start.tbf_start_present = 1;
     /*
     *  Downlink Control Timeslot
     */
    ptr2prim_i->p_ul_alloc.p_fixed_alloc.dl_ctrl_ts = d_ts_reconfig->f_alloc_re.dl_ctrl_ts;
    grr_data->tc.fa_ctrl.dl_ctrl_ts = d_ts_reconfig->f_alloc_re.dl_ctrl_ts;
    /*
     * ALLOCATION BITMAP
     */
    if(d_ts_reconfig->f_alloc_re.v_block_struct)
    {
      tc_calc_fa_bitmap(ptr2prim_i->p_tbf_start.fn,                         /*starting time of cuurent tbf*/
                        d_ts_reconfig->f_alloc_re.block_struct.bl_o_bl_per, /* blocks or block periods*/
                        d_ts_reconfig->f_alloc_re.block_struct.a_map_len,   /* bitmap length*/
                        d_ts_reconfig->f_alloc_re.block_struct.alloc_map,   /* ptr to alloc struct*/
                        &ptr2prim_i->p_ul_alloc.p_fixed_alloc);             /* ptr to fix sttruct*/
    }
    else if(d_ts_reconfig->f_alloc_re.v_alloc_map)
    {
      tc_calc_fa_bitmap(ptr2prim_i->p_tbf_start.fn,                         /*starting time of cuurent tbf*/
                        0,                                                  /* blocks : default*/
                        d_ts_reconfig->f_alloc_re.c_alloc_map,              /* bitmap length*/
                        d_ts_reconfig->f_alloc_re.alloc_map,                /* ptr to alloc struct*/
                        &ptr2prim_i->p_ul_alloc.p_fixed_alloc);             /* ptr to fix sttruct*/
    }
    /*
     * check final allocation
     */
    grr_data->tc.fa_ctrl.current_alloc.final_alloc = d_ts_reconfig->f_alloc_re.final_alloc;
  }
  else if ( d_ul_ack->msg_type EQ D_UL_ACK_c)
  {
    /*
     * Timing Advance in packet uplink ack/nack with fixed allocation
     */
    if(d_ul_ack->gprs_ul_ack_nack_info.v_pta)
    {
      grr_handle_ta ( d_ul_ack->gprs_ul_ack_nack_info.pta.v_ta_value, 
                      d_ul_ack->gprs_ul_ack_nack_info.pta.ta_value,
                      d_ul_ack->gprs_ul_ack_nack_info.pta.v_ta_index_tn, 
                      d_ul_ack->gprs_ul_ack_nack_info.pta.ta_index_tn.ta_index,
                      d_ul_ack->gprs_ul_ack_nack_info.pta.ta_index_tn.ta_tn,
                      0xFF,
                      0,
                      0,
                      &ptr2prim_i->p_timing_advance);
    }
    else
    {
      grr_handle_ta ( 0, 
                      0,
                      0xFF, 
                      0,
                      0,
                      0xFF,
                      0,
                      0,
                      &ptr2prim_i->p_timing_advance);
    }
 
    /*
     * mac mode
     */
    ptr2prim_i->mac_mode = FA;
    /*
     * uplink allocation structure
     */

    /*no change, therefore take from database*/
    ptr2prim_i->p_ul_alloc.ul_tfi = grr_data->uplink_tbf.tfi;
    ptr2prim_i->p_ul_alloc.ts_mask = grr_data->uplink_tbf.ts_usage;
    /*
     *  fixed allocation structure
     *  half duplex mode is ignored, only valid for MS class 19 to 29
     */
    /*
     *  TBF STARTING TIME for fixed allocation structure
     */
    if(d_ul_ack->gprs_ul_ack_nack_info.v_f_alloc_ack AND d_ul_ack->gprs_ul_ack_nack_info.f_alloc_ack.v_fa_s2)
    {
      if(d_ul_ack->gprs_ul_ack_nack_info.f_alloc_ack.fa_s2.tbf_s_time.v_abs)
      {
        ptr2prim_i->p_tbf_start.fn = grr_decode_tbf_start_abs(&d_ul_ack->gprs_ul_ack_nack_info.f_alloc_ack.fa_s2.tbf_s_time.abs);
      }
      else if (d_ul_ack->gprs_ul_ack_nack_info.f_alloc_ack.fa_s2.tbf_s_time.v_rel)
      {
        ptr2prim_i->p_tbf_start.fn = grr_decode_tbf_start_rel(grr_data->dl_fn,
                                                              d_ul_ack->gprs_ul_ack_nack_info.f_alloc_ack.fa_s2.tbf_s_time.rel);
      }
      else
      {  /* this shall never happen */
        ptr2prim_i->p_tbf_start.fn = (grr_data->dl_fn+13)%FN_MAX;       
        TRACE_ERROR("NO STARTING TIME IN FIXED ALLOCATION!! struct problem");
      }
    }
    else
    {  /* this shall never happen */
      ptr2prim_i->p_tbf_start.fn = (grr_data->dl_fn+13)%FN_MAX;       
      TRACE_ERROR("NO STARTING TIME IN FIXED ALLOCATION!!");
    }
    ptr2prim_i->p_tbf_start.tbf_start_present = 1;

 
    /*
     *  Downlink Control Timeslot, take from database
     */
    ptr2prim_i->p_ul_alloc.p_fixed_alloc.dl_ctrl_ts = grr_data->tc.fa_ctrl.dl_ctrl_ts;
    /*
     * ALLOCATION BITMAP
     */
    if(d_ul_ack->gprs_ul_ack_nack_info.f_alloc_ack.fa_s2.v_block_struct)
    {
      tc_calc_fa_bitmap(ptr2prim_i->p_tbf_start.fn,                          /* starting time of cuurent tbf*/
                        d_ul_ack->gprs_ul_ack_nack_info.f_alloc_ack.fa_s2.block_struct.bl_o_bl_per,/* blocks or block periods*/
                        d_ul_ack->gprs_ul_ack_nack_info.f_alloc_ack.fa_s2.block_struct.a_map_len,  /* bitmap length */
                        d_ul_ack->gprs_ul_ack_nack_info.f_alloc_ack.fa_s2.block_struct.alloc_map,  /* ptr to alloc struct*/
                        &ptr2prim_i->p_ul_alloc.p_fixed_alloc);              /* ptr to fix sttruct*/
    }
    else if(d_ul_ack->gprs_ul_ack_nack_info.f_alloc_ack.fa_s2.v_alloc_map)
    {
      tc_calc_fa_bitmap(ptr2prim_i->p_tbf_start.fn,                      /*starting time of cuurent tbf*/
                        0,                                               /* blocks : default*/
                        d_ul_ack->gprs_ul_ack_nack_info.f_alloc_ack.fa_s2.c_alloc_map,         /* bitmap length*/
                        d_ul_ack->gprs_ul_ack_nack_info.f_alloc_ack.fa_s2.alloc_map,           /* ptr to alloc struct*/
                        &ptr2prim_i->p_ul_alloc.p_fixed_alloc);          /* ptr to fix sttruct*/
    }
    /*
     * check final allocation
     */
    grr_data->tc.fa_ctrl.current_alloc.final_alloc = d_ul_ack->gprs_ul_ack_nack_info.f_alloc_ack.final_alloc;
  }
  else
  {
    TRACE_ERROR ( "Wrong message type in _decodedCtrlMsg" );
  }
   
  if(ptr2prim_i->p_tbf_start.tbf_start_present)
    grr_data->uplink_tbf.tbf_start_fn = ptr2prim_i->p_tbf_start.fn;
  else
    grr_data->uplink_tbf.tbf_start_fn = CGRLC_STARTING_TIME_NOT_PRESENT;

  grr_data->uplink_tbf.st_tfi = ptr2prim_i->p_ul_alloc.ul_tfi;


  return;
} /* tc_set_fa_assignment() */


/*
+------------------------------------------------------------------------------
| Function    : tc_set_dl_assignment
+------------------------------------------------------------------------------
| Description : This function sets some parameter of T_MPHP_ASSIGNMENT_REQ is
|               case of downlink allocation
|
| Parameters  : ptr2prim_i -  ptr to MPHP_ASSIGNMENT_REQ
|
+------------------------------------------------------------------------------
*/
LOCAL void tc_set_dl_assignment (T_MPHP_ASSIGNMENT_REQ * ptr2prim_i)
{
  MCAST(d_dl_assign, D_DL_ASSIGN); /* T_D_DL_ASSIGN  */
  MCAST(d_ts_reconfig,D_TS_RECONFIG);

  TRACE_FUNCTION( "tc_set_dl_assignment" );
  if( d_dl_assign->msg_type EQ D_DL_ASSIGN_c)
  {
    /*
     * timing advance
     */
  grr_handle_ta ( d_dl_assign->pta.v_ta_value, 
          d_dl_assign->pta.ta_value,
          0xFF, 
          0,
          0,
          d_dl_assign->pta.v_ta_index_tn,
          d_dl_assign->pta.ta_index_tn.ta_index,
          d_dl_assign->pta.ta_index_tn.ta_tn,
          &ptr2prim_i->p_timing_advance);
    /*
     * TBF starting time
     */
    
    if(d_dl_assign->pda_trnc_grp.v_tbf_s_time)
    {
      ptr2prim_i->p_tbf_start.tbf_start_present = 1;
      if(d_dl_assign->pda_trnc_grp.tbf_s_time.v_abs)
      {
        ptr2prim_i->p_tbf_start.fn = grr_decode_tbf_start_abs(&d_dl_assign->pda_trnc_grp.tbf_s_time.abs);
      }
      else if (d_dl_assign->pda_trnc_grp.tbf_s_time.v_rel)
      {
        ptr2prim_i->p_tbf_start.fn = grr_decode_tbf_start_rel(grr_data->dl_fn,
                                                              d_dl_assign->pda_trnc_grp.tbf_s_time.rel);
      }
      else
      {  /* this shall never happen */
        ptr2prim_i->p_tbf_start.fn = grr_data->dl_fn%FN_MAX;
        TRACE_ERROR("DL v_start_tbf but either rel nor abs tbf included");
      }      
    }
    else if(grr_data->l1_del_tbf_start_fn NEQ GRR_INVALID_FN)
    {
      ptr2prim_i->p_tbf_start.tbf_start_present = 1;  
      ptr2prim_i->p_tbf_start.fn = grr_data->l1_del_tbf_start_fn;
    }
    else
    {
      ptr2prim_i->p_tbf_start.tbf_start_present = 0;  
    }


    if(d_dl_assign->v_bts_pwr_ctrl)
    {
      ptr2prim_i->p_dl_power.p0 = d_dl_assign->bts_pwr_ctrl.p0;
      ptr2prim_i->p_dl_power.bts_pwr_ctl_mode = d_dl_assign->bts_pwr_ctrl.mode;
      ptr2prim_i->p_dl_power.pr_mode = d_dl_assign->bts_pwr_ctrl.pr_mode;
    }
    else
    {
      ptr2prim_i->p_dl_power.p0 = 0xff;
    }
    /*
    * mac mode
    */
    ptr2prim_i->mac_mode = d_dl_assign->mac_mode;

   /*
    *  downlink allocation structure
    */
    if(d_dl_assign->pda_trnc_grp.v_dl_tfi_assign)
    {
      ptr2prim_i->p_dl_alloc.dl_tfi = d_dl_assign->pda_trnc_grp.dl_tfi_assign;
    }
    else
    {
      ptr2prim_i->p_dl_alloc.dl_tfi = grr_data->downlink_tbf.tfi;
    }
    ptr2prim_i->p_dl_alloc.ts_mask = d_dl_assign->ts_alloc;

  }
  else if ( d_ts_reconfig->msg_type EQ D_TS_RECONFIG_c)
  {
    /* TS Reconfigure allways contains ul assign -> TBF starting time
     * is handled there
     */
    if(d_ts_reconfig->v_dl_tfi)
    {
      ptr2prim_i->p_dl_alloc.dl_tfi = d_ts_reconfig->dl_tfi;
    }
    else
    {
      ptr2prim_i->p_dl_alloc.dl_tfi = grr_data->downlink_tbf.tfi;
    }
    ptr2prim_i->p_dl_alloc.ts_mask = grr_data->downlink_tbf.ts_usage;
    /*
     * timing advance
     */
    grr_handle_ta ( d_ts_reconfig->gpta.v_ta_value, 
          d_ts_reconfig->gpta.ta_value,
          d_ts_reconfig->gpta.v_ul_ta_index, 
          d_ts_reconfig->gpta.ul_ta_index,
          d_ts_reconfig->gpta.ul_ta_tn,
          d_ts_reconfig->gpta.v_dl_ta_index,
          d_ts_reconfig->gpta.dl_ta_index,
          d_ts_reconfig->gpta.dl_ta_tn,
          &ptr2prim_i->p_timing_advance);
  }
  else
  {
    TRACE_ERROR ( "Wrong message type in _decodedCtrlMsg" );
  }


  if(ptr2prim_i->p_tbf_start.tbf_start_present)
    grr_data->downlink_tbf.tbf_start_fn = ptr2prim_i->p_tbf_start.fn;
  else
    grr_data->downlink_tbf.tbf_start_fn = CGRLC_STARTING_TIME_NOT_PRESENT;

  grr_data->downlink_tbf.st_tfi      = ptr2prim_i->p_dl_alloc.dl_tfi;
  grr_data->downlink_tbf.polling_bit = 0xFF;

  
  
  return;
} /* tc_set_dl_assignment() */



/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : tc_gen_rand
+------------------------------------------------------------------------------
| Description : This function generates a random value between db.prach.s
|               and (db.prach.tx_int -1)
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL USHORT tc_gen_rand ( void )
{
  USHORT result ;
  UBYTE i;
  TRACE_FUNCTION( "tc_gen_rand" );

  /*
   * the "-1" defined in ETS 04.60 will be done by tc_random_value()
   *
   */
  i = tx_int_table[psc_db->prach.tx_int];
  
  result =  (USHORT)grr_random_value (S_T_RND_INDEX, i );

  result +=  s_table[psc_db->prach.s_prach];

#ifndef _TARGET_
  TRACE_EVENT_P2("S, S+1 ..:%d tx_init:%d",result, i);
#endif /* _TARGET_ */

  return (result);

} /* tc_gen_rand() */



/*
+------------------------------------------------------------------------------
| Function    : tc_calc_req
+------------------------------------------------------------------------------
| Description : The function tc_calc_req() generate the message
|               Packet Access Request.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL USHORT tc_calc_req ( void )
{
  USHORT channel_request_data = 0;

  TRACE_FUNCTION( "tc_calc_req" );

  switch(psc_db->gprs_cell_opt.ab_type)
  {
    case AB_8_BIT:
      switch(grr_data->uplink_tbf.access_type)
      {
        case CGRLC_AT_ONE_PHASE:
          channel_request_data = tc_one_phase_mask_8bit();
          channel_request_data |= ((grr_get_gprs_ms_class( )-1)<<2);
          break;
        case CGRLC_AT_SHORT_ACCESS:
          channel_request_data = tc_short_access_mask_8bit();
          channel_request_data |= ((grr_data->uplink_tbf.nr_blocks-1)<<3);
          break;
        case CGRLC_AT_TWO_PHASE:
          channel_request_data=tc_two_phase_mask_8bit();
          break;
        case CGRLC_AT_PAGE_RESPONSE:
          channel_request_data=tc_page_response_mask_8bit();
          break;
        case CGRLC_AT_CELL_UPDATE:
          channel_request_data=tc_cell_update_mask_8bit();
          break;
        case CGRLC_AT_MM_PROCEDURE:
          channel_request_data=tc_mm_procedure_mask_8bit();
          break;
        case CGRLC_AT_SINGLE_BLOCK:
          channel_request_data=tc_single_block_mask_8bit();
          break;
        default:
          break;
      }
      break;

    case AB_11_BIT:
      switch(grr_data->uplink_tbf.access_type)
      {
        case CGRLC_AT_ONE_PHASE:
          channel_request_data=tc_one_phase_mask_11bit();
          channel_request_data |= ((grr_get_gprs_ms_class( )-1)<<5);
          channel_request_data |= (grr_data->uplink_tbf.prio<<3);
          break;
        case CGRLC_AT_SHORT_ACCESS:
          channel_request_data=tc_short_access_mask_11bit();
          channel_request_data |= (grr_data->uplink_tbf.prio<<3);
          channel_request_data |= ((grr_data->uplink_tbf.nr_blocks-1)<<5);
          break;
        case CGRLC_AT_TWO_PHASE:
          channel_request_data=tc_two_phase_mask_11bit();
          channel_request_data |= (grr_data->uplink_tbf.prio<<3);
          break;
        case CGRLC_AT_PAGE_RESPONSE:
          channel_request_data=tc_page_response_mask_11bit();
          break;
        case CGRLC_AT_CELL_UPDATE:
          channel_request_data=tc_cell_update_mask_11bit();
          break;
        case CGRLC_AT_MM_PROCEDURE:
          channel_request_data=tc_mm_procedure_mask_11bit();
          break;
        case CGRLC_AT_SINGLE_BLOCK:
          channel_request_data=tc_single_block_mask_11bit();
          break;
        default:
          channel_request_data = 0;
          break;
      }
      channel_request_data = grr_convert_11bit_2_etsi(channel_request_data);
      TRACE_EVENT_P1("11 bit Channel Request: 0x%X", channel_request_data);
      break;
    default:
      break;
  }
 return(channel_request_data);

} /* tc_calc_req() */



/*
+------------------------------------------------------------------------------
| Function    : tc_check_p_level
+------------------------------------------------------------------------------
| Description : The function tc_check_p_level() checks the p-level for the
|               PDU's radio priority aigainst a random value and returns
|               wheather to send the random access or to continue with
|               calculating.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL T_CHECK_P_LEVEL tc_check_p_level ( void )
{
  UBYTE p_level,i;
  T_CHECK_P_LEVEL result;
  TRACE_FUNCTION( "tc_check_p_level" );

  /*
   * get value between 0 and 15
   */
  i = (UBYTE) grr_random_value (PLEV_RND_INDEX, 16);

  p_level = psc_db->prach.pers_lev.plev[grr_data->uplink_tbf.prio];

  if(p_level EQ 15)
  {
    /*
     * transmission in every case forbitten, because refer to ETS the value 0xf
     * have to be interpreted as 16 and i is in every case more little (0 <= i <= 15).
     * This case should be avoided (dead lock!) and have to be checked
     * in tc_start_access()
     */
    TRACE_ERROR("unexpected persistence_level_value");
    result = C_P_LEVEL_DO_NOT_SEND;
  }
  else
  {
    result = ( (i >= p_level)  ?  C_P_LEVEL_SEND : C_P_LEVEL_DO_NOT_SEND );
  }

  return( result );

} /* tc_check_p_level() */



/*
+------------------------------------------------------------------------------
| Function    : tc_send_assign_req
+------------------------------------------------------------------------------
| Description : The function tc_send_assign_req() builds MPHP_ASSIGNMENT_REQ
|               and send it.
|
| Parameters  : tbf_type_i - type of TBF that is to activate
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_send_assign_req ( T_TBF_TYPE tbf_type_i )
{

  TRACE_FUNCTION( "tc_send_assign_req" );
  {
    T_MPHP_ASSIGNMENT_REQ * ptr2prim;//=NULL;

    ptr2prim=tc_set_freq();

    grr_set_tbf_cfg_req_param( ptr2prim );

    switch( tbf_type_i )
    {
      case CGRLC_TBF_MODE_DL:
        ptr2prim->assign_cmd = DL_ASSIGNMENT;
        tc_set_dl_assignment(ptr2prim);
        break;
      case CGRLC_TBF_MODE_UL:
        ptr2prim->assign_cmd = UL_ASSIGNMENT;
        if(grr_data->uplink_tbf.mac_mode EQ DA)
          tc_set_da_assignment(ptr2prim);
        else
          tc_set_fa_assignment(ptr2prim);
        break;
      case CGRLC_TBF_MODE_DL_UL:
        ptr2prim->assign_cmd = UL_DL_ASSIGNMENT;
        if(grr_data->uplink_tbf.mac_mode EQ DA)
          tc_set_da_assignment(ptr2prim);
        else
          tc_set_fa_assignment(ptr2prim);
        tc_set_dl_assignment(ptr2prim);
        break;
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
      case CGRLC_TBF_MODE_2PA:
        ptr2prim->assign_cmd = UL_TP_ACCESS;
        tc_set_fa_assignment(ptr2prim);
        break;
#endif
      default:
        break;
    } /* switch (tbf_type_i) */ 
    PSEND(hCommL1,ptr2prim);
    
  }
} /* tc_send_assign_req() */



/*
+------------------------------------------------------------------------------
| Function    : tc_send_tbf_rel
+------------------------------------------------------------------------------
| Description : The function tc_send_tbf_rel() builds MPHP_TBF_RELEASE_REQ
|               and send it.
|
| Parameters  : TBP-Type - that have to be deleted
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_send_tbf_rel ( T_TBF_TYPE tbf_i )
{
  UBYTE dummy = grr_data->tc.num_of_rels_running;

  TRACE_FUNCTION( "tc_send_tbf_rel" );

  tc_prepare_send_tbf_release_req( tbf_i );

  {
    T_RELEASE_STATE rel_state= REL_PERFORMED;

    if(dummy NEQ grr_data->tc.num_of_rels_running)
    {
      rel_state = REL_RUNNING;
    }
    sig_tc_ctrl_rel_state(rel_state);
  }


} /* tc_send_tbf_rel() */



/*
+------------------------------------------------------------------------------
| Function    : tc_send_pdch_rel
+------------------------------------------------------------------------------
| Description : The function tc_send_pdch_rel() builds MPHP_PDCH_RELEASE_REQ
|               and send it.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_send_pdch_rel ( UBYTE ts_available )
{
  PALLOC(mphp_pdch_release_req,MPHP_PDCH_RELEASE_REQ);

  TRACE_FUNCTION( "tc_send_pdch_rel" );

  mphp_pdch_release_req->assign_id = 0;
  mphp_pdch_release_req->ts_mask   = ts_available;
  PSEND(hCommL1,mphp_pdch_release_req);

} /* tc_send_pdch_rel() */



/*
+------------------------------------------------------------------------------
| Function    : tc_abort_tbf
+------------------------------------------------------------------------------
| Description : The function tc_abort_tbf() stops a TBF.
|
| Parameters  : tbf_i - TBF type to abort
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_abort_tbf ( T_TBF_TYPE tbf_i )
{

  TRACE_FUNCTION( "tc_abort_tbf" );

  switch( tbf_i )
  {
    case CGRLC_TBF_MODE_NULL:
      TRACE_EVENT("NULL TBF active: check if tbf starting time is running");
      tc_cgrlc_ul_tbf_res ( CGRLC_TBF_MODE_ACCESS_FAILED, CGRLC_PRIM_STATUS_NULL );
      break;
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
    case CGRLC_TBF_MODE_2PA:
      tc_send_tbf_rel(tbf_i);
      tc_cgrlc_tbf_rel_req(tbf_i,CGRLC_TBF_REL_ABNORMAL,CGRLC_STARTING_TIME_NOT_PRESENT);
      tc_deactivate_tbf(tbf_i);
      break;
#endif
    case CGRLC_TBF_MODE_UL:
      tc_send_tbf_rel(tbf_i);
      tc_cgrlc_tbf_rel_req(tbf_i,CGRLC_TBF_REL_ABNORMAL,CGRLC_STARTING_TIME_NOT_PRESENT);
      tc_deactivate_tbf(tbf_i);
      break;
    case CGRLC_TBF_MODE_DL:
      tc_send_tbf_rel(tbf_i);
      tc_cgrlc_tbf_rel_req(tbf_i,CGRLC_TBF_REL_ABNORMAL,CGRLC_STARTING_TIME_NOT_PRESENT);
      tc_deactivate_tbf(tbf_i);
      break;
    case CGRLC_TBF_MODE_DL_UL:
      tc_send_tbf_rel(tbf_i);
      tc_cgrlc_tbf_rel_req(tbf_i,CGRLC_TBF_REL_ABNORMAL,CGRLC_STARTING_TIME_NOT_PRESENT);
      tc_deactivate_tbf(tbf_i);
      break;
    default:
      TRACE_ERROR ( "tc_abort_tbf: TBF type is invalid" );
      break;
  }
} /* tc_abort_tbf() */





/*
+------------------------------------------------------------------------------
| Function    : tc_build_res_req
+------------------------------------------------------------------------------
| Description : The function tc_build_res_req() builds Packet Resource Request.
|
| Parameters  : reason_i   - the reason for building that packet resouce
|                            reallocation
|               src_info_i - indicates where to get the carrier information
|                            regarding the assigned TBF
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_build_res_req (T_U_RESOURCE_REQ *ptr2res_req,
                              T_REASON_BUILD    reason_i,
                              T_SRC_TBF_INFO    src_info_i )
{
  UBYTE change_mark;

  TRACE_FUNCTION( "tc_build_res_req" );


  memset(ptr2res_req,0,sizeof(T_U_RESOURCE_REQ) );

  /* processing of messsage type */
  ptr2res_req->msg_type = U_RESOURCE_REQ_c;

  /* processing of change mark */
  if( grr_is_pbcch_present( ) )
  {
    change_mark = psc_db->psi2_params.psi2_change_mark;
  }
  else
  {
    change_mark = psc_db->psi13_params.si13_change_mark;
  }
  
  if( change_mark EQ NOT_SET )
  {
    ptr2res_req->v_ma_ch_mark = 0;
  }
  else
  {
    ptr2res_req->v_ma_ch_mark = 1;
    ptr2res_req->ma_ch_mark   = change_mark;
  }


  if(reason_i EQ R_BUILD_2PHASE_ACCESS)
  {
    /* 1 - processing of ACCESS_TYPE */
    ptr2res_req->v_access_type = 1;
    ptr2res_req->access_type   = TWO_PHASE;

    /* 2 - processing of global TFI and TLLI */
    ptr2res_req->flag          = 1;
    ptr2res_req->v_glob_tfi    = 0;
    ptr2res_req->v_tlli_value  = 1;

    grr_set_buf_tlli( &ptr2res_req->tlli_value, grr_data->db.ms_id.new_tlli );

    /* 3 - processing of radio access capabilities */
    if( rr_csf_get_radio_access_capability( &ptr2res_req->ra_cap ) EQ 0 )
    {
      ptr2res_req->v_ra_cap = TRUE;
    }
    else
    {
      ptr2res_req->v_ra_cap = FALSE;

      TRACE_ERROR( "tc_build_res_req: radio access capabilities invalid" );
    }
    /* 4 - processing of channel request description */
    ptr2res_req->chan_req_des.peak_thr_class  = (UBYTE) grr_data->uplink_tbf.peak;
    ptr2res_req->chan_req_des.radio_prio      = grr_data->uplink_tbf.prio;
    if(grr_data->uplink_tbf.prim_type EQ CGRLC_LLC_PRIM_TYPE_UNITDATA_REQ)
    {
      ptr2res_req->chan_req_des.rlc_mode = CGRLC_RLC_MODE_UACK;
    }
    else
    {
      ptr2res_req->chan_req_des.rlc_mode = CGRLC_RLC_MODE_ACK;
    }
    ptr2res_req->chan_req_des.llc_pdu_type    = LLC_NOT_ACK;
    ptr2res_req->chan_req_des.rlc_octet_cnt   = grr_data->uplink_tbf.rlc_oct_cnt;

    /* 5 - processing of signal variance */
    ptr2res_req->v_signvar = FALSE;
  }

  /* processing of C value */
  {
    ptr2res_req->c_value = meas_c_get_value( );
  }

  /* processing of relative interference levels */
  {
    T_p_frequency_par freq_par;
    T_MEAS_IM_CARRIER ma;

    switch( src_info_i )
    {
      case( SRC_TBF_INFO_RE_ASSIGN ):
      case( SRC_TBF_INFO_IMM_ASSIGN ):
      case( SRC_TBF_INFO_UL_ASSIGN ):
        grr_set_freq_par( &freq_par );
        meas_im_set_carrier( &ma, &freq_par );
        break;

      default:
        TRACE_ASSERT( src_info_i EQ SRC_TBF_INFO_RE_ASSIGN  OR
                      src_info_i EQ SRC_TBF_INFO_IMM_ASSIGN OR
                      src_info_i EQ SRC_TBF_INFO_UL_ASSIGN     );
        break;
    }

    meas_im_get_rel_i_level( &ma, &ptr2res_req->ilev );
  }

#ifdef REL99
  ptr2res_req->v_release_99_str_prr              = FALSE;
  ptr2res_req->release_99_str_prr.flag           = 0;
  ptr2res_req->release_99_str_prr.flag2          = 0;
  ptr2res_req->release_99_str_prr.v_pfi          = 0;
  ptr2res_req->release_99_str_prr.add_ms_rac     = 0;
  ptr2res_req->release_99_str_prr.retrans_of_prr = 0;
#endif

} /* tc_build_res_req() */


/*
+------------------------------------------------------------------------------
| Function    : tc_eval_pdch_rel
+------------------------------------------------------------------------------
| Description : The function tc_eval_pdch_rel() evaluate a
|               Packet PDCH Release.
|
| Parameters  : slot_i - the slot where the message was received
|
+------------------------------------------------------------------------------
*/
GLOBAL T_EVAL_PDCH_REL tc_eval_pdch_rel ( UBYTE slot_i )
{
  MCAST(d_pdch_release,D_PDCH_RELEASE); /* T_D_PDCH_RELEASE */

  UBYTE mask,help=0xFF, help2=0xFF;
  T_EVAL_PDCH_REL result=E_PDCH_REL_NULL;

  TRACE_FUNCTION( "tc_eval_pdch_rel" );

  if(!d_pdch_release->v_ts_available)
  {
    mask  = 0x80;
    mask  >>= slot_i;
    mask ^= 0xFF;
    TRACE_EVENT_P7("NO TS PDCH REL: slot=%d ->mask=%x ul=%x st_ul=%x dl=%x st_dl=%x c_fn=%ld"
                                                                      ,slot_i
                                                                      ,mask
                                                                      ,grr_data->uplink_tbf.ts_mask
                                                                      ,grr_data->uplink_tbf.ts_usage
                                                                      ,grr_data->downlink_tbf.ts_mask
                                                                      ,grr_data->downlink_tbf.ts_usage
                                                                      , grr_data->ul_fn);
  }
  else
  {
    mask = d_pdch_release->ts_available;
    TRACE_EVENT_P7("ts PDCH REL: mask=%x ul=%x st_ul=%x dl=%x st_dl=%x c_fn=%ld"
                                                                      ,slot_i
                                                                      ,mask
                                                                      ,grr_data->uplink_tbf.ts_mask
                                                                      ,grr_data->uplink_tbf.ts_usage
                                                                      ,grr_data->downlink_tbf.ts_mask
                                                                      ,grr_data->downlink_tbf.ts_usage
                                                                      , grr_data->ul_fn); 
  }
  grr_data->pdch_rel_ts_mask = mask;

  switch(grr_data->tbf_type)
  {
    case CGRLC_TBF_MODE_UL:
      help = grr_data->uplink_tbf.ts_mask;
      grr_data->uplink_tbf.ts_mask &= mask;
      if(!grr_data->uplink_tbf.ts_mask)
      {
        result = E_PDCH_REL_RELEASE_UL;
      }
      else if(grr_data->uplink_tbf.ts_mask NEQ help)
      {
        grr_data->uplink_tbf.nts = grr_calc_nr_of_set_bits(grr_data->uplink_tbf.ts_mask);
        result = E_PDCH_REL_RECONF;
        tc_cgrlc_ul_tbf_res(CGRLC_TBF_MODE_UL,CGRLC_PRIM_STATUS_NULL);
      }
      else
      {
        TRACE_EVENT("NO change in pdch rel for ul tbf");
        result = E_PDCH_REL_NULL;
      }
      break;
    case CGRLC_TBF_MODE_DL:
      help = grr_data->downlink_tbf.ts_mask;
      grr_data->downlink_tbf.ts_mask &= mask;
      if(!grr_data->downlink_tbf.ts_mask)
      {
        result = E_PDCH_REL_RELEASE_DL;
      }
      else if(grr_data->downlink_tbf.ts_mask NEQ help)
      {
        grr_data->downlink_tbf.nts = grr_calc_nr_of_set_bits(grr_data->downlink_tbf.ts_mask);
        tc_cgrlc_dl_tbf_req();
        result = E_PDCH_REL_RECONF;
      }
      else
      {
        TRACE_EVENT("NO change in pdch rel for dl tbf");
        result = E_PDCH_REL_IGNORE;
      }
      break;
    case CGRLC_TBF_MODE_DL_UL:
      help  = grr_data->uplink_tbf.ts_mask;
      help2 = grr_data->downlink_tbf.ts_mask;
      grr_data->uplink_tbf.ts_mask   &= mask;
      grr_data->downlink_tbf.ts_mask &= mask;
      if( !handle_ms_cap(UL_DL_ASSIGNMENT))
      {
        result = E_PDCH_REL_RELEASE_BOTH;
        TRACE_EVENT_P3("MS CLASS ERROR in PDCH assignment  ul_mask=%x, dl_mask=%x, pdch_mask=%x",
                                                grr_data->uplink_tbf.ts_mask,
                                                grr_data->downlink_tbf.ts_mask,
                                                mask);
      }
      else if(!grr_data->downlink_tbf.ts_mask AND !grr_data->uplink_tbf.ts_mask)
      {
        result = E_PDCH_REL_RELEASE_BOTH;
        TRACE_EVENT_P3("rel of UL and DL  ul_mask=%x, dl_mask=%x, pdch_mask=%x",
                                                grr_data->uplink_tbf.ts_mask,
                                                grr_data->downlink_tbf.ts_mask,
                                                mask);
      }
      else if(!grr_data->downlink_tbf.ts_mask)
      { /*DL release*/
        if(grr_data->uplink_tbf.ts_mask EQ help)
        { /* no change for uplink*/
          result = E_PDCH_REL_RELEASE_DL;
          TRACE_EVENT_P3("rel of DL  ul_mask=%x, dl_mask=%x, pdch_mask=%x",
                                                grr_data->uplink_tbf.ts_mask,
                                                grr_data->downlink_tbf.ts_mask,
                                                mask);
        }
        else
        { /*uplink reconfigure*/
          result = E_PDCH_REL_RELEASE_DL_RECONF_UL;
          grr_data->uplink_tbf.nts = grr_calc_nr_of_set_bits(grr_data->uplink_tbf.ts_mask);
          tc_cgrlc_ul_tbf_res(CGRLC_TBF_MODE_UL,CGRLC_PRIM_STATUS_NULL);
          TRACE_EVENT_P3("rel of DL, conf UL ul_mask=%x, dl_mask=%x, pdch_mask=%x",
                                                grr_data->uplink_tbf.ts_mask,
                                                grr_data->downlink_tbf.ts_mask,
                                                mask);
        }
      }
      else if(!grr_data->uplink_tbf.ts_mask)
      { /*UL release*/
        if(grr_data->downlink_tbf.ts_mask EQ help2)
        { /* no change for downlink*/
          result = E_PDCH_REL_RELEASE_UL;
          TRACE_EVENT_P3("rel of UL  ul_mask=%x, dl_mask=%x, pdch_mask=%x",
                                                grr_data->uplink_tbf.ts_mask,
                                                grr_data->downlink_tbf.ts_mask,
                                                mask);
        }
        else
        { /*downlink reconfigure*/
          result = E_PDCH_REL_RELEASE_UL_RECONF_DL;
          grr_data->downlink_tbf.nts = grr_calc_nr_of_set_bits(grr_data->downlink_tbf.ts_mask);
          tc_cgrlc_dl_tbf_req();
          TRACE_EVENT_P3("rel of UL, conf DL ul_mask=%x, dl_mask=%x, pdch_mask=%x",
                                                grr_data->uplink_tbf.ts_mask,
                                                grr_data->downlink_tbf.ts_mask,
                                                mask);
        }
      }
      else if(grr_data->downlink_tbf.ts_mask NEQ help2)
      { /*change in DL*/
        result = E_PDCH_REL_RECONF;
        grr_data->downlink_tbf.nts = grr_calc_nr_of_set_bits(grr_data->downlink_tbf.ts_mask);
        tc_cgrlc_dl_tbf_req();
        if(grr_data->uplink_tbf.ts_mask EQ help)
        { /* no change for uplink*/
          TRACE_EVENT_P3("conf of DL  ul_mask=%x, dl_mask=%x, pdch_mask=%x",
                                                grr_data->uplink_tbf.ts_mask,
                                                grr_data->downlink_tbf.ts_mask,
                                                mask);
        }
        else
        { /*uplink reconfigure*/
          grr_data->uplink_tbf.nts = grr_calc_nr_of_set_bits(grr_data->uplink_tbf.ts_mask);
          tc_cgrlc_ul_tbf_res(CGRLC_TBF_MODE_UL,CGRLC_PRIM_STATUS_NULL);
          TRACE_EVENT_P3("conf ofDL/UL ul_mask=%x, dl_mask=%x, pdch_mask=%x",
                                                grr_data->uplink_tbf.ts_mask,
                                                grr_data->downlink_tbf.ts_mask,
                                                mask);
        }
      }
      else
      { /* no change in DL*/
        if(grr_data->uplink_tbf.ts_mask EQ help)
        { /* no change for uplink*/
          result = E_PDCH_REL_IGNORE;
          TRACE_EVENT_P3("NO conf UL/DL  ul_mask=%x, dl_mask=%x, pdch_mask=%x",
                                                grr_data->uplink_tbf.ts_mask,
                                                grr_data->downlink_tbf.ts_mask,
                                                mask);
        }
        else
        { /*uplink reconfigure*/
          result = E_PDCH_REL_RECONF;
          grr_data->uplink_tbf.nts = grr_calc_nr_of_set_bits(grr_data->uplink_tbf.ts_mask);
          tc_cgrlc_ul_tbf_res(CGRLC_TBF_MODE_UL,CGRLC_PRIM_STATUS_NULL);
          TRACE_EVENT_P3("conf of UL ul_mask=%x, dl_mask=%x, pdch_mask=%x",
                                                grr_data->uplink_tbf.ts_mask,
                                                grr_data->downlink_tbf.ts_mask,
                                                mask);
        }
      }
      break;
    default:
      TRACE_ERROR("unknown tbf type in tc_eval_pdch_rel");
      break;
  }
  return(result);

} /* tc_eval_pdch_rel() */



/*
+------------------------------------------------------------------------------
| Function    : tc_eval_ul_assign
+------------------------------------------------------------------------------
| Description : The function tc_eval_ul_assign() evaluates a
|               Packet Uplink Assignment.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL T_EVAL_UL_ASSIGN tc_eval_ul_assign ( void )
{
  MCAST(d_ul_assign,D_UL_ASSIGN); /* T_D_UL_ASSIGN */
  T_EVAL_UL_ASSIGN result = E_UL_ASSIGN_NULL;
  UBYTE tx_slots = 0;
  UBYTE i,mask;

  T_GRR_STORE_TYPE grr_store_type = GRR_STORE_TYPE_NONE;

  TRACE_FUNCTION( "tc_eval_ul_assign" );

#ifdef REL99
  if(d_ul_assign->egprs_flag)
  {
    
    TRACE_ERROR("EGPRS TBF received in UL ASSIGNMENT");
    
    if(grr_data->tbf_type EQ CGRLC_TBF_MODE_NULL)
    {
      /* TBF is not active. Handling abnormal case
       * 7.1.4  If the mobile station has been assigned a TBF in EGPRS mode and 
       * the MS does not support EGPRS, or has been assigned an MCS (e.g. 8-PSK 
       * in the Uplink) that the MS does not support, the MS shall return to packet
       * idle mode and notify higher layers (TBF establishment failure)
       */
      return E_UL_ASSIGN_ERROR_RA;	  
    }
    else
    {
      /* TBF is active. Handling abnormal case
       * 8.7 While a TBF is in progress, if a mobile station receives a 
       * PACKET UPLINK ASSIGNMENT, PACKET UPLINK ACK/NACK or PACKET TIMESLOT RECONFIGURE 
       * message with message escape bit indicating EGPRS (resp. GPRS) contents whereas 
       * the current TBF mode is GPRS (resp. EGPRS), the mobile station shall ignore the 
       * message.
       */
      return E_UL_ASSIGN_IGNORE;
    }
  }
#endif

  /*
   * Handle mac mode
   */  
  switch( grr_data->tbf_type )
  {
    case CGRLC_TBF_MODE_DL:
      grr_data->uplink_tbf.ti = 0;  /* contention resolution NOT required */

      /*lint -fallthrough*/

    case CGRLC_TBF_MODE_NULL:
      if(d_ul_assign->v_dyn_alloc_p)
      {
        if(!d_ul_assign->dyn_alloc_p.xdyn_alloc)
          grr_data->uplink_tbf.mac_mode = DA;
        else
        {
          grr_data->uplink_tbf.mac_mode = EDA;
        }
      }          
      else if(d_ul_assign->v_f_alloc_ul)
        grr_data->uplink_tbf.mac_mode = FA; 
      break;
    case CGRLC_TBF_MODE_UL:
      if( grr_data->uplink_tbf.mac_mode NEQ DA  AND 
          d_ul_assign->v_dyn_alloc_p
        )
      { 
        TRACE_ERROR("dynamic alloc received while fixed is running in uplink");
        return  E_UL_ASSIGN_ERROR_RA;
      }
      else if( grr_data->uplink_tbf.mac_mode NEQ FA  AND 
               d_ul_assign->v_f_alloc_ul 
             )
      {
        TRACE_ERROR("fixed alloc received while dynamic is running in uplink");
        return  E_UL_ASSIGN_ERROR_RA;
      }
      break;
    default:
      break;
  } 
  grr_data->uplink_tbf.cs_mode      = d_ul_assign->chan_coding_cmd;
  grr_data->uplink_tbf.tlli_cs_mode = d_ul_assign->tlli_chan_coding;
  grr_data->uplink_tbf.polling_bit  = 0xFF;

  if(d_ul_assign->v_dyn_alloc_p)
  {
    if( grr_data->uplink_tbf.mac_mode NEQ DA)
    {
      result = E_UL_ASSIGN_ERROR_RA;
      TRACE_ERROR("dyn alloc received but diff mac mode");
      TRACE_EVENT_P3("MAC_MODE:   ms=%d, Ex.DA=%d  tbf_mode=%d"
                                                              ,grr_data->uplink_tbf.mac_mode
                                                              ,d_ul_assign->dyn_alloc_p.xdyn_alloc
                                                              ,grr_data->tbf_type);
    }
    else
    {
      result = E_UL_ASSIGN_DYNAMIC;
      /* SZML-TC/071*/
      
      if (d_ul_assign->dyn_alloc_p.v_rlc_db_granted)/* close-ended TBF*/
      {
        UBYTE   data_size=20;

        switch( grr_data->uplink_tbf.cs_mode)
        {
          case CGRLC_CS_MODE_1:
            data_size = 20;
            break;
          case CGRLC_CS_MODE_2:
            data_size = 30;
            break;
          case CGRLC_CS_MODE_3:
            data_size = 36;
            break;
          case CGRLC_CS_MODE_4:
            data_size = 50;
            break;
          default:
            TRACE_ERROR("unknown Coding Scheme");
            break;
        }
        data_size -= grr_data->uplink_tbf.ti*4; /* CR TLLI */
  
        grr_data->uplink_tbf.rlc_db_granted = (d_ul_assign->dyn_alloc_p.rlc_db_granted + 9);

        if (grr_data->uplink_tbf.rlc_db_granted * data_size <  grr_data->uplink_tbf.rlc_oct_cnt)
        /* granted rlc blocks in bytes are not enough to transmit the first pdu*/
        {
          result = E_UL_ASSIGN_ERROR_RA;
          TRACE_EVENT_P3("granted rlc blocks are not enough to transmit the first pdu rlc_g=%ld sdu_len =%ld data_size=%d"
                          ,grr_data->uplink_tbf.rlc_db_granted * data_size
                          ,grr_data->uplink_tbf.rlc_oct_cnt
                          ,data_size);
          return(result);
        }
      }
      else
      {
        /*
         * open ended TBF
         */
        grr_data->uplink_tbf.rlc_db_granted = 0;
      }

      grr_data->uplink_tbf.ts_usage = 0;

      if(d_ul_assign->dyn_alloc_p.v_tn_alloc)
      {
        for(i = 0,mask = 0x80;i < 8;i++)
        {
          if(d_ul_assign->dyn_alloc_p.tn_alloc[i].v_usf)
          {
            grr_data->uplink_tbf.ts_usage |= mask;
            tx_slots++;
          }
          mask >>= 1;
        }
      }
      else if(d_ul_assign->dyn_alloc_p.v_tn_alloc_pwr)
      {
        for(i = 0,mask = 0x80;i < 8;i++)
        {
          if(d_ul_assign->dyn_alloc_p.tn_alloc_pwr.usf_array[i].v_usf_g)
          {

            grr_data->uplink_tbf.ts_usage |= mask;
            tx_slots++;
          }
          mask >>= 1;
        }

        grr_store_type = GRR_STORE_TYPE_TN_ALLOC_PWR;
      }
      grr_data->uplink_tbf.nts = tx_slots;
    }
  }
  else if(d_ul_assign->v_sin_alloc)
  {
    if( grr_data->tc.v_sb_without_tbf )
    {
      result = E_UL_ASSIGN_SB_WITHOUT_TBF;
    }
    else
    {
      result = E_UL_ASSIGN_SB_2PHASE_ACCESS ;
      grr_data->uplink_tbf.ti = 0;
    }

    /*
     *   estimate timeslot
     */
    grr_data->uplink_tbf.ts_usage = (0x80 >> d_ul_assign->sin_alloc.tn);
    tx_slots=1;
    grr_data->uplink_tbf.nts = tx_slots;

    if(d_ul_assign->sin_alloc.v_alf_gam)
    {
      grr_store_type = GRR_STORE_TYPE_ALF_GAM;
    }

    /* starting time allready elapsed? */
    if(d_ul_assign->sin_alloc.tbf_s_time.v_abs)
    {
      ULONG fn_out;
      fn_out = grr_decode_tbf_start_abs(&d_ul_assign->sin_alloc.tbf_s_time.abs);           
      TRACE_EVENT_P5("eval_ul fn_out:%ld fn_in:%d %d %d data->fn:%ld",
                                                                 fn_out,
                                                                 d_ul_assign->sin_alloc.tbf_s_time.abs.t1,
                                                                 d_ul_assign->sin_alloc.tbf_s_time.abs.t2,
                                                                 d_ul_assign->sin_alloc.tbf_s_time.abs.t3,
                                                                 grr_data->dl_fn);
      if(grr_check_if_tbf_start_is_elapsed ( fn_out, grr_data->ul_fn))
      {
        TRACE_ERROR("TBF Starting time is ELAPSED in single block allocation!");
        TRACE_EVENT_P2("CPAP SBR TBF ST ELAPSED st=%ld  c_fn=%ld  ", fn_out,grr_data->ul_fn);
        result = E_UL_ASSIGN_ERROR_RA;
      }
    }
    else if(d_ul_assign->sin_alloc.tbf_s_time.v_rel AND !d_ul_assign->sin_alloc.tbf_s_time.rel)
    {
      TRACE_ERROR("rel. TBF Starting time in single block allocation k=0 NOT ALLOWED");
      TRACE_EVENT_P1("k=%d ", d_ul_assign->sin_alloc.tbf_s_time.rel);
      result = E_UL_ASSIGN_ERROR_RA;
    }
    else if(!d_ul_assign->sin_alloc.tbf_s_time.v_rel)
    {
      TRACE_ERROR("NO TBF Starting time in single block allocation ");
      TRACE_EVENT_P2("v_abs=%d  v_rel=%d ", d_ul_assign->sin_alloc.tbf_s_time.v_abs
                                          , d_ul_assign->sin_alloc.tbf_s_time.v_rel);
      result = E_UL_ASSIGN_ERROR_RA;
    }
  }
  else if(d_ul_assign->v_f_alloc_ul)
  {
    if( grr_data->uplink_tbf.mac_mode EQ DA )
    {
      result = E_UL_ASSIGN_ERROR_RA;
      TRACE_ERROR("fixed alloc received while dynamic is running in uplink");
    }
    else
    {
      result = E_UL_ASSIGN_FIXED;
      grr_data->uplink_tbf.ts_usage = 0;
      tx_slots=0;
      if(d_ul_assign->f_alloc_ul.v_ts_alloc)
      {
        grr_data->uplink_tbf.ts_usage = d_ul_assign->f_alloc_ul.ts_alloc;
        /*calculate number of timeslots*/
        tx_slots = grr_calc_nr_of_set_bits(grr_data->uplink_tbf.ts_usage);
      }
      else if(d_ul_assign->f_alloc_ul.v_pwr_par)
      {
        for(i = 0,mask = 0x80;i < 8;i++)
        {
          if(d_ul_assign->f_alloc_ul.pwr_par.gamma_tn[i].v_gamma)
          {
            grr_data->uplink_tbf.ts_usage |= mask;
            tx_slots++;
          }
          mask >>= 1;
        }

        grr_store_type = GRR_STORE_TYPE_PWR_PAR;
      }
      grr_data->uplink_tbf.nts = tx_slots;
    }
  }
  else
  {
    TRACE_ERROR ("Packet Uplink Assignment without allocation type - corrupt airmessage!");
  }

  if( !handle_ms_cap(UL_ASSIGNMENT) )
  {
    result = E_UL_ASSIGN_ERROR_RA;
    TRACE_EVENT("tc_eval_.. will return E_UL_ASSIGN_ERROR_RA (tx_slots wrong)");
  }

  if ( !tc_check_and_save_freq ())
  {
    if ((grr_data->tbf_type EQ CGRLC_TBF_MODE_DL) OR (grr_data->tbf_type EQ CGRLC_TBF_MODE_DL_UL) )
    {
      result = E_UL_ASSIGN_IGNORE;
      TRACE_EVENT("tc_eval_.. will return E_UL_ASSIGN_IGNORE , DL ACTIVE (frequencies wrong)");		   
    }
    else
    {
      result = E_UL_ASSIGN_ERROR_RA;
      TRACE_EVENT("tc_eval_.. will return E_UL_ASSIGN_ERROR_RA (frequencies wrong)");	
    }
    return result;
  } 

  if( result NEQ E_UL_ASSIGN_ERROR_RA AND
      result NEQ E_UL_ASSIGN_NULL         )
  {
    switch( grr_store_type )
    {
      case GRR_STORE_TYPE_TN_ALLOC_PWR:
        grr_store_type_tn_alloc_pwr( &d_ul_assign->dyn_alloc_p.tn_alloc_pwr );
        break;

      case GRR_STORE_TYPE_ALF_GAM:
        grr_store_type_alf_gam( &d_ul_assign->sin_alloc.alf_gam,
                                 d_ul_assign->sin_alloc.tn       );
        break;
      
      case GRR_STORE_TYPE_PWR_PAR:
        grr_store_type_pwr_par( &d_ul_assign->f_alloc_ul.pwr_par, FALSE );
        break;
      
      default: 
        /* do nothing */
        break;
    }
  }

  return(result);
} /* tc_eval_ul_assign() */



/*
+------------------------------------------------------------------------------
| Function    : tc_eval_dl_assign
+------------------------------------------------------------------------------
| Description : The function tc_eval_dl_assign() evaluates a
|               Packet Downlink Assignment.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL T_EVAL_DL_ASSIGN tc_eval_dl_assign ()
{
  MCAST(d_dl_assign,D_DL_ASSIGN); /* T_D_DL_ASSIGN */
  T_EVAL_DL_ASSIGN result = E_DL_ASSIGN_IGNORE;

  TRACE_FUNCTION( "tc_eval_dl_assign" );

  /* SZML-TC/072: check for things that results in E_DL_ASSIGN_ERROR_RA */


     
  grr_data->downlink_tbf.ctrl_ack_bit= 0;   

  /*
   * compare uplink and new assigned downlink mac mode 
   * and set downlink mac mode
   */ 
  switch( grr_data->tbf_type )
  {
    case CGRLC_TBF_MODE_NULL:
      grr_data->downlink_tbf.mac_mode    = d_dl_assign->mac_mode;
      grr_data->downlink_tbf.t3192       = FALSE;                      /* indicates if t3192 is running*/
      grr_data->downlink_tbf.rlc_mode    = d_dl_assign->rlc_mode;

      result = E_DL_ASSIGN;
      break;
    case CGRLC_TBF_MODE_UL:
      grr_data->downlink_tbf.mac_mode    = d_dl_assign->mac_mode;
      grr_data->downlink_tbf.t3192       = FALSE;                      /* indicates if t3192 is running*/
      grr_data->downlink_tbf.rlc_mode    = d_dl_assign->rlc_mode;

      /*lint -fallthrough*/
    case CGRLC_TBF_MODE_DL_UL:
      if(d_dl_assign->mac_mode NEQ grr_data->uplink_tbf.mac_mode)
      {
        result = E_DL_ASSIGN_IGNORE;
        TRACE_EVENT_P2("Ignore DL_ASS, Diff MAC Mode UL=%d DL=%d ",grr_data->uplink_tbf.mac_mode,d_dl_assign->mac_mode);
        return result;
      }
      if ((grr_data->downlink_tbf.rlc_mode NEQ d_dl_assign->rlc_mode) AND !d_dl_assign->ctrl_ack)
      {
        TRACE_ERROR("Diff rlc mode in downlink ass..aborting tbf");
        result = E_DL_ASSIGN_ABORT_DL;
        return result;
      }
      grr_data->downlink_tbf.rlc_mode    = d_dl_assign->rlc_mode;
      result = E_DL_ASSIGN;
      break;
    case CGRLC_TBF_MODE_DL:
      if ((grr_data->downlink_tbf.rlc_mode NEQ d_dl_assign->rlc_mode) AND !d_dl_assign->ctrl_ack)
      {
        TRACE_ERROR("Diff rlc mode in downlink ass..aborting tbf");
        result = E_DL_ASSIGN_ABORT_DL;
        return result;
      }
      grr_data->downlink_tbf.rlc_mode    = d_dl_assign->rlc_mode;
      result = E_DL_ASSIGN;
      break;
    default:
      result = E_DL_ASSIGN;
      break;
  } 

  /*
   * compare old and new mac mode for downlink
   */ 
  if ( d_dl_assign->mac_mode EQ  FA_HALF_DUPLEX)
  {
    result = E_DL_ASSIGN_IGNORE;
    TRACE_EVENT( "DL ASS: Half duplex not supported" );
    return result;
  } 
  else if(d_dl_assign->mac_mode NEQ  grr_data->downlink_tbf.mac_mode)
  {
    result = E_DL_ASSIGN_IGNORE;
    TRACE_EVENT_P3( "DL ASS: current mac mode= %d differs from commanded =%d  tbf_type=%d"
                                                                ,grr_data->downlink_tbf.mac_mode
                                                                ,d_dl_assign->mac_mode
                                                                ,grr_data->tbf_type );
    return result;
  }


  /* 
   * ckeck if tfi is present while no downlink tbf is active
   */
  switch( grr_data->tbf_type )
  {
    case CGRLC_TBF_MODE_UL:
    case CGRLC_TBF_MODE_NULL:
      if(!d_dl_assign->pda_trnc_grp.v_dl_tfi_assign)
      {
        result = E_DL_ASSIGN_IGNORE;
        TRACE_EVENT("Ignore DL_ASS, no dl_tfi in P DL Ass(no dl tbf active) ");
        return result;
      }
      result = E_DL_ASSIGN;
      break;
    default:
      result = E_DL_ASSIGN;
      break;
  } 


  if(d_dl_assign->ctrl_ack)
  {
    if(grr_data->downlink_tbf.t3192)
    {
      grr_data->downlink_tbf.ctrl_ack_bit= 1;   
      grr_data->downlink_tbf.t3192    = FALSE;                      /* indicates if t3192 is running*/

    }
    else
    {
      result = E_DL_ASSIGN_IGNORE;
      TRACE_EVENT("DL ASS IGNORED, ctrl ack bit set but t3192 is not running");
      return result;
    }
  }
  else if(grr_data->downlink_tbf.t3192)
  {
    TRACE_EVENT("DL assignment:ctrl ack bit is not set but t3192 is running");
  }

  grr_data->downlink_tbf.ts_usage = d_dl_assign->ts_alloc;
  grr_data->downlink_tbf.nts      = grr_calc_nr_of_set_bits(grr_data->downlink_tbf.ts_usage);
  if( !handle_ms_cap(DL_ASSIGNMENT))
  {
    result = E_DL_ASSIGN_ERROR_RA;
    TRACE_EVENT("tc_eval_.. will return E_DL_ASSIGN_ERROR_RA (ms class error)");
    return result;
  }

  if( tc_check_and_save_freq( ) EQ FALSE )
  {
    if ((grr_data->tbf_type EQ CGRLC_TBF_MODE_UL) OR (grr_data->tbf_type EQ CGRLC_TBF_MODE_DL_UL))
    {
      result = E_DL_ASSIGN_IGNORE;
      TRACE_EVENT("tc_eval_.. will return E_DL_ASSIGN_IGNORE , UL ACTIVE (frequencies wrong)");
    }
    else
    {
      result = E_DL_ASSIGN_ERROR_RA;
      TRACE_EVENT("tc_eval_.. will return E_DL_ASSIGN_ERROR_RA (frequencies wrong)");
    }
    return result;
  }

  if( d_dl_assign->pda_trnc_grp.v_pwr_par EQ TRUE )
  {
    grr_store_type_pwr_par( &d_dl_assign->pda_trnc_grp.pwr_par, FALSE );
  }

  return(result);
} /* tc_eval_dl_assign() */



/*
+------------------------------------------------------------------------------
| Function    : tc_eval_access_rej
+------------------------------------------------------------------------------
| Description : The function tc_eval_access_rej() evaluates a
|               Packet Access Reject.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL T_EVAL_ACCESS_REJ tc_eval_access_rej ( ULONG * t3172_value_i)
{
  MCAST(d_access_rej,D_ACCESS_REJ);
  T_EVAL_ACCESS_REJ result = E_ACCESS_REJ_NULL;

  TRACE_FUNCTION( "tc_eval_access_rej" );
  /*SZML-TC/122*/

  if(d_access_rej->reject.v_wait)
  {
    result = E_ACCESS_REJ_WAIT;

    *t3172_value_i = d_access_rej->reject.wait.wait_ind;
    if( d_access_rej->reject.wait.waitsize  )
    {
      *t3172_value_i *= 20;
    }
    else
    {
      *t3172_value_i *= 1000;
    }
  }
  else
  {
    result = E_ACCESS_REJ_NO_WAIT;
  }

  return(result);

} /* tc_eval_access_rej() */



/*
+------------------------------------------------------------------------------
| Function    : tc_send_ra_req
+------------------------------------------------------------------------------
| Description : The function tc_send_ra_req() starts the build-process for a
|               Packet Access Request and send a MPHP_RA_REQ.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_send_ra_req ( void )
{
  TRACE_FUNCTION( "tc_send_ra_req" );


  while( (grr_data->tc.n_acc_req < 
          max_retrans_table[psc_db->prach.max_retrans[grr_data->uplink_tbf.prio]]+1)
         AND (tc_check_p_level() EQ C_P_LEVEL_DO_NOT_SEND) )
  {
    TRACE_EVENT("C_P_LEVEL_DO_NOT_SEND");
    grr_data->tc.n_acc_req++;
  }


  if (grr_data->tc.n_acc_req < 
    max_retrans_table[psc_db->prach.max_retrans[grr_data->uplink_tbf.prio]]+1)
  {
    PALLOC(mphp_ra_req,MPHP_RA_REQ);
    memset(mphp_ra_req,0x00,sizeof(T_MPHP_RA_REQ));
    /*
     * The powervalue txpwr_max_cch in DB is always a useful value.
     * The value is set by PSI. First a useful initial value.
     * After that a value from RR and after reception of PSI3 txpwr_max_cch
     * is set to a value from the scell-structure.
     */

    mphp_ra_req->txpwr = 
      grr_get_ms_txpwr_max_cch
        ( psc_db->pbcch.bcch.arfcn,
          grr_data->meas.pwr_offset, 
          psc_db->scell_par.cr_par_1.cr_pow_par.gprs_ms_txpwr_max_cch );

    if(grr_data->tc.sending_req EQ TRUE)
    {
      mphp_ra_req->rand               = (USHORT)tc_gen_rand();
    }
    else 
    {
      /* The first PCR may be sent immediately when ready. Only the second PCR   */
      /* needs a random value which has to have uniform probability distribution */
      mphp_ra_req->rand               = 1;
    }
    mphp_ra_req->channel_request_data = tc_calc_req();
    mphp_ra_req->bs_prach_blks        = psc_db->pccch.bs_prach_blks;  
    mphp_ra_req->burst_type           = psc_db->gprs_cell_opt.ab_type;

    {
      UBYTE channel_request_data[RLC_MAC_MAX_LEN_CHANNEL_REQ];

      channel_request_data[0] = ( mphp_ra_req->channel_request_data >> 8 );
      channel_request_data[1] = ( UBYTE )mphp_ra_req->channel_request_data;
      
      TRACE_BINDUMP
        ( hCommGRR, TC_USER4, 
          cl_rlcmac_get_msg_name( U_MSG_TYPE_CHANNEL_REQ_c, RLC_MAC_ROUTE_UL ),
          channel_request_data, RLC_MAC_MAX_LEN_CHANNEL_REQ ); /*lint !e569*/
    }

    PSEND(hCommL1,mphp_ra_req);
    grr_data->tc.sending_req = TRUE;
  }
  else 
  {
    vsi_t_start(GRR_handle,T3170,tc_get_t3170_value());
    vsi_t_stop(GRR_handle,T3186);
    tc_send_ra_stop_req();

  }


} /* tc_send_ra_req() */



/*
+------------------------------------------------------------------------------
| Function    : tc_send_ra_stop_req
+------------------------------------------------------------------------------
| Description : The function tc_send_ra_stop_req() stops the
|               sending of packet access requests.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_send_ra_stop_req ( void )
{
  TRACE_FUNCTION( "tc_send_ra_stop_req" );

  /*
   *  This timer might be run - in case of MPHP_RA_CON has not been received by GRR.
   *  If it does not run the stop-command will be ignored by vsi.
   */
  vsi_t_stop(GRR_handle,T3186);

  if(grr_data->tc.sending_req)
  {
    PALLOC(mphp_ra_stop_req,MPHP_RA_STOP_REQ);
    PSEND(hCommL1,mphp_ra_stop_req); 
      /*
       *   reset number of access in this procedure
       */
    grr_data->tc.n_acc_req =0;
        
    /*
     * to make sure that TC only stops
     * the sending procedure if it
     * is running
     */
    grr_data->tc.sending_req = FALSE;
  }

} /* tc_send_ra_stop_req() */



/*
+------------------------------------------------------------------------------
| Function    : tc_send_polling_res
+------------------------------------------------------------------------------
| Description : The function tc_send_polling_res() sends the primitive
|               MPHP_POLLING_RESPONSE_REQ.
|
| Parameters  : ctrl_ack_type_i - indicates if normal or access burst shall sent
|               rrbp_i - depending on this variable the send frame number is computed
|               fn_i   - frame number where Packet Polling Request was received
|               ctrl_ack_i - this value is sent within the pca, depends frim mac header
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_send_polling_res (UBYTE poll_type_i, ULONG fn_i, UBYTE rrbp_i,UBYTE ctrl_ack_i )
{
  PALLOC( mphp_polling_response_req, MPHP_POLLING_RESPONSE_REQ );

  TRACE_FUNCTION( "tc_send_polling_res" );

  mphp_polling_response_req->fn    = grr_calc_new_poll_pos( fn_i, rrbp_i );

  mphp_polling_response_req->txpwr =
    grr_get_ms_txpwr_max_cch 
      ( psc_db->pbcch.bcch.arfcn,
        grr_data->meas.pwr_offset, 
        psc_db->scell_par.cr_par_1.cr_pow_par.gprs_ms_txpwr_max_cch );

  if(poll_type_i EQ CGRLC_POLL_RES_NB)
  {
    mphp_polling_response_req->poll_resp_type = POLL_RESP_NB_CS1;
    memset( mphp_polling_response_req->poll_data,
            0x2b,
            sizeof(mphp_polling_response_req->poll_data) );    
    mphp_polling_response_req->poll_data[23] = 0 ; /* Byte is not used, set to 0 for test purposes */

    /* code without CCD, air message buffer is used by other service*/
    mphp_polling_response_req->poll_data[0] = 0x40 | grr_data->r_bit;
    mphp_polling_response_req->poll_data[1] = 0x04 |((UBYTE)(grr_data->db.ms_id.new_tlli>>30));
    mphp_polling_response_req->poll_data[2] = ((UBYTE)(grr_data->db.ms_id.new_tlli>>22));
    mphp_polling_response_req->poll_data[3] = ((UBYTE)(grr_data->db.ms_id.new_tlli>>14));
    mphp_polling_response_req->poll_data[4] = ((UBYTE)(grr_data->db.ms_id.new_tlli>>06));
    mphp_polling_response_req->poll_data[5] = ((UBYTE)(grr_data->db.ms_id.new_tlli<<2)) + ctrl_ack_i;
  
    /*
     * get TA from paxket uplink/downlink assignment
     */
    if(grr_data->ta_params.ta_value EQ 0xFF)
    {
      MCAST( d_ul_assign, D_UL_ASSIGN );
      MCAST( d_dl_assign, D_DL_ASSIGN);

      if(( d_dl_assign->msg_type EQ D_DL_ASSIGN_c) AND
           d_dl_assign->pta.v_ta_value)
           
      {
        mphp_polling_response_req->ta_index = d_dl_assign->pta.ta_value;
      }
      else if(( d_ul_assign->msg_type EQ D_UL_ASSIGN_c) AND
                d_ul_assign->pta.v_ta_value)
      {
        mphp_polling_response_req->ta_index = d_ul_assign->pta.ta_value;
      }
      else
      {
        TRACE_ERROR("NO TA IN MPHP_POLLING_REQ");
        TRACE_EVENT_P5("msg_typ=%d, v_pta_dl=%d v_pta_ul=%d v_ptai_dl=%d v_ptai_ul=%d"
                                                                                       ,d_ul_assign->msg_type
                                                                                       ,d_dl_assign->pta.v_ta_value
                                                                                       ,d_ul_assign->pta.v_ta_value
                                                                                       ,d_dl_assign->pta.v_ta_index_tn
                                                                                       ,d_ul_assign->pta.v_ta_index_tn);
      }

    }
    else
    {
      mphp_polling_response_req->ta_index     = grr_data->ta_params.ta_value;
      TRACE_EVENT_P1("valid ta in database at sending MPHP_POLLING_RES_REQ ta=%d",grr_data->ta_params.ta_value);
    }

    TRACE_BINDUMP
      ( hCommGRR, TC_USER4,
        cl_rlcmac_get_msg_name
          ( ( UBYTE )( mphp_polling_response_req->poll_data[1] >> 2 ), RLC_MAC_ROUTE_UL ),
        mphp_polling_response_req->poll_data, MAX_L2_FRAME_SIZE ); /*lint !e569*/
  }
  else
  {
    USHORT data;

    /*
     * initalize channel request data array
     */   
    memset( mphp_polling_response_req->poll_data,
            0,
            sizeof(mphp_polling_response_req->poll_data) );

    /*
     * format is four access bursts
     */
    if(psc_db->gprs_cell_opt.ab_type EQ AB_8_BIT)
    {
      /*
       *  8 Bit access burst
       */
      mphp_polling_response_req->poll_resp_type =  POLL_RESP_AB_8_BIT;
      data =0x0000;
      data|=0x7f;
      /*TRACE_EVENT_P1("POLL_RESP_8BIT_AB: FN %Ld", mphp_polling_response_req->fn);*/
    }
    else
    {
      /*
       *  11 Bit access burst
       */

      mphp_polling_response_req->poll_resp_type = POLL_RESP_AB_11_BIT;
      data = (0x07<<8);
      data|=0xe7;

      data = grr_convert_11bit_2_etsi(data);
      /*TRACE_EVENT_P1("POLL_RESP_11BIT_AB: FN %Ld", mphp_polling_response_req->fn);*/
    }

    mphp_polling_response_req->poll_data[0]= (UBYTE)(data & 0x00ff);        
    mphp_polling_response_req->poll_data[1]= (UBYTE)((data & 0xff00) >> 8);

    TRACE_BINDUMP
      ( hCommGRR, TC_USER4,
        cl_rlcmac_get_msg_name( U_MSG_TYPE_CHANNEL_REQ_c, RLC_MAC_ROUTE_UL ),
        mphp_polling_response_req->poll_data,
        RLC_MAC_MAX_LEN_CHANNEL_REQ ); /*lint !e569*/
  }

  PSEND( hCommL1, mphp_polling_response_req );

} /* tc_send_polling_res() */



/*
+------------------------------------------------------------------------------
| Function    : tc_send_single_block
+------------------------------------------------------------------------------
| Description : The function tc_send_single_block() sends the primitive
|               MPHP_SINGLE_BLOCK_REQ with measurement report.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_send_single_block ( void )
{
  MCAST(d_ul_assign,D_UL_ASSIGN);
  T_MPHP_SINGLE_BLOCK_REQ * ptr2prim;

  TRACE_FUNCTION( "tc_send_single_block" );

  ptr2prim = tc_get_send_nb_prim_and_set_freq( );

  ptr2prim->assign_id    = 0;
  ptr2prim->purpose      = SINGLE_BLOCK_TRANSFER_UL;
  ptr2prim->pc_meas_chan = psc_db->g_pwr_par.pc_meas_chan;
  ptr2prim->burst_type   = ( psc_db->gprs_cell_opt.ab_type EQ AB_8_BIT )
                           ? AB_8_BIT : AB_11_BIT;
  /* timing advance */
  if (d_ul_assign->pta.v_ta_value)
  {
    ptr2prim->p_timing_advance.ta_value = d_ul_assign->pta.ta_value;
  }
  else
  {
    ptr2prim->p_timing_advance.ta_value = 0xff;
  }

  if(d_ul_assign->pta.v_ta_index_tn)
  {
    ptr2prim->p_timing_advance.ta_index = d_ul_assign->pta.ta_index_tn.ta_index;
  }
  else
  {
    ptr2prim->p_timing_advance.ta_index = 0xff;
  }

  /*  trainings sequence   */
  ptr2prim->tsc = d_ul_assign->freq_par.tsc;

  /* handle TBF starting time (is always present in single block allocation) */
  if(d_ul_assign->sin_alloc.tbf_s_time.v_abs)
  {
    ptr2prim->p_tbf_start.tbf_start_present  = 1; 
    ptr2prim->p_tbf_start.fn = grr_decode_tbf_start_abs(&d_ul_assign->sin_alloc.tbf_s_time.abs);
    if(grr_check_if_tbf_start_is_elapsed ( ptr2prim->p_tbf_start.fn, grr_data->ul_fn))
    { /* SZML-TC/077: drop message, enter pim  */
      TRACE_ERROR("TBF Starting time is ELAPSED in single block allocation!");
      TRACE_EVENT_P2("CPAP SBR TBF ST ELAPSED st=%ld  c_fn=%ld  ", ptr2prim->p_tbf_start.fn,grr_data->ul_fn);      
    }
  }
  else if (d_ul_assign->sin_alloc.tbf_s_time.v_rel)
  {
    ptr2prim->p_tbf_start.tbf_start_present  = 1;         
    /* 
     * For the relative position set d_ul_assign->sin_alloc.tbf_s_time.rel-1, because grr_data->fn is
     * received from mphp_data_ind. This fn is the current fn, and not the received fn.
     */
    ptr2prim->p_tbf_start.fn = grr_decode_tbf_start_rel(grr_data->dl_fn,
                                                        (USHORT)(d_ul_assign->sin_alloc.tbf_s_time.rel));
  }
  else
  {
    TRACE_ERROR("TBF Starting time is missing in single block allocation!");
      /* SZML-TC/078:  drop message, enter pim  !  */
  }

  /* timeslot number */
  ptr2prim->tn=d_ul_assign->sin_alloc.tn;

  /* put measurement report into buffer */
  tc_cpy_ctrl_blk_to_buffer( ptr2prim->l2_frame );
  
  sig_tc_meas_update_pch( );

  TRACE_BINDUMP
    ( hCommGRR, TC_USER4,
      cl_rlcmac_get_msg_name
        ( ( UBYTE )( ptr2prim->l2_frame[1] >> 2 ), RLC_MAC_ROUTE_UL ),
      ptr2prim->l2_frame, MAX_L2_FRAME_SIZE ); /*lint !e569*/
  
  PSEND(hCommL1,ptr2prim);
} /* tc_send_single_block() */



/*
+------------------------------------------------------------------------------
| Function    : tc_send_resource_request_p
+------------------------------------------------------------------------------
| Description : The function tc_send_resource_request_p() sends the primitive
|               MPHP_SINGLE_BLOCK_REQ with Packet Resource Request for
|               two phase access due to reception of airmessage packet uplink
|               assignment.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_send_resource_request_p ( void )
{
  MCAST(d_ul_assign,D_UL_ASSIGN);

  T_MPHP_SINGLE_BLOCK_REQ * ptr2prim;

  TRACE_FUNCTION( "tc_send_resource_request_p" );


  ptr2prim = tc_get_send_nb_prim_and_set_freq( );

  /*
   *   mark that single block is because of two phase access procedure
   */
  ptr2prim->assign_id    = 0;
  ptr2prim->purpose      = TWO_PHASE_ACESS;
  ptr2prim->pc_meas_chan = psc_db->g_pwr_par.pc_meas_chan;
  ptr2prim->burst_type   = (psc_db->gprs_cell_opt.ab_type EQ AB_8_BIT)
                           ?AB_8_BIT
                           :AB_11_BIT;

  if (d_ul_assign->pta.v_ta_value)
  {
    ptr2prim->p_timing_advance.ta_value = d_ul_assign->pta.ta_value;
  }
  else
  {
    ptr2prim->p_timing_advance.ta_value = 0xff;
  }

  if(d_ul_assign->pta.v_ta_index_tn)
  {
    ptr2prim->p_timing_advance.ta_index =d_ul_assign->pta.ta_index_tn.ta_index;
  }
  else
  {
    ptr2prim->p_timing_advance.ta_index =0xff;
  }

  /*
   *  trainings sequence
   */
  ptr2prim->tsc = d_ul_assign->freq_par.tsc;


 /*
  * handle TBF starting time
  * is always present in single block allocation
  */
  if(d_ul_assign->sin_alloc.tbf_s_time.v_abs)
  {
    ptr2prim->p_tbf_start.tbf_start_present = 1; 
    ptr2prim->p_tbf_start.fn = grr_decode_tbf_start_abs(&d_ul_assign->sin_alloc.tbf_s_time.abs);
    if(grr_check_if_tbf_start_is_elapsed ( ptr2prim->p_tbf_start.fn, grr_data->ul_fn))
    { /* SZML-TC/079 */
      TRACE_ERROR("TBF Starting time is ELAPSED in single block allocation!");
      TRACE_EVENT_P2("CPAP SBR TBF ST ELAPSED st=%ld  c_fn=%ld  ", ptr2prim->p_tbf_start.fn,grr_data->ul_fn);
    }
    TRACE_EVENT_P5("res_req fn_out:%ld fn_in:%d %d %d data->fn:%ld",
                                                                ptr2prim->p_tbf_start.fn,
                                                                d_ul_assign->sin_alloc.tbf_s_time.abs.t1,
                                                                d_ul_assign->sin_alloc.tbf_s_time.abs.t2,
                                                                d_ul_assign->sin_alloc.tbf_s_time.abs.t3,
                                                                grr_data->dl_fn);
  }
  else if (d_ul_assign->sin_alloc.tbf_s_time.v_rel)
  {
    ptr2prim->p_tbf_start.tbf_start_present   = 1;
    /* 
     * For the relative position set d_ul_assign->sin_alloc.tbf_s_time.rel-1, because grr_data->fn is
     * received from mphp_data_ind. This fn is the current fn, and not the received fn.
     */
    ptr2prim->p_tbf_start.fn = grr_decode_tbf_start_rel(grr_data->dl_fn, 
                                                       (USHORT)(d_ul_assign->sin_alloc.tbf_s_time.rel));
    TRACE_EVENT_P3("REL: ST_TIME for PRR:  fn_i=%ld rel=%ld fn_out=%ld"
                                                                       ,grr_data->dl_fn
                                                                       ,d_ul_assign->sin_alloc.tbf_s_time.rel
                                                                       ,ptr2prim->p_tbf_start.fn);
  }
  else
  {
    TRACE_ERROR("TBF Starting time is missing in single block allocation!");
  }

  ptr2prim->tn = d_ul_assign->sin_alloc.tn;

  {
    T_U_RESOURCE_REQ resource_req;

    tc_build_res_req( &resource_req,
                      R_BUILD_2PHASE_ACCESS,
                      SRC_TBF_INFO_UL_ASSIGN );

    grr_encode_ul_ctrl_block( ptr2prim->l2_frame, ( UBYTE* )&resource_req );
  }

  sig_tc_meas_update_pch( );

  TRACE_BINDUMP
    ( hCommGRR, TC_USER4,
      cl_rlcmac_get_msg_name
        ( ( UBYTE )( ptr2prim->l2_frame[1] >> 2 ), RLC_MAC_ROUTE_UL ),
      ptr2prim->l2_frame, MAX_L2_FRAME_SIZE ); /*lint !e569*/

  PSEND(hCommL1,ptr2prim);

  return;

} /* tc_send_resource_request_p() */


/*
+------------------------------------------------------------------------------
| Function    : tc_handle_ra_con
+------------------------------------------------------------------------------
| Description : The function tc_handle_ra_con() checks wheather there is to send
|               another Packet Access Request or not.
|
| Parameters  : void
+------------------------------------------------------------------------------
*/
GLOBAL T_HANDLE_RA_CON tc_handle_ra_con ( void )
{
  T_HANDLE_RA_CON result;
  UBYTE max_retrans;
  TRACE_FUNCTION( "tc_handle_ra_con" );


  /*
   *  stop T3186 becauce packet access request was sent
   */


  /*
   *  get current max. retransmissson value
   */
  max_retrans
    = max_retrans_table[psc_db->prach.max_retrans[grr_data->uplink_tbf.prio]];

  if(grr_data->tc.n_acc_req >= max_retrans + 1) 
  {
    result = H_RA_CON_STOP_REQ;
    vsi_t_start(GRR_handle,T3170,tc_get_t3170_value());
    vsi_t_stop(GRR_handle,T3186);
  }
  else
  {
    result = H_RA_CON_CONTINUE;
  }


  return(result);

} /* tc_handle_ra_con() */







/*
+------------------------------------------------------------------------------
| Function    : tc_init
+------------------------------------------------------------------------------
| Description : The function tc_init() initialize the TC data structure.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_init ( void )
{
  UBYTE i;

  TRACE_FUNCTION( "tc_init" );

  /*
   *  set some values
   */
  grr_data->tc.disable_callback_func = NULL;
  grr_data->tc.disable_class         = CGRLC_DISABLE_CLASS_CR;
  grr_data->tbf_type                 = CGRLC_TBF_MODE_NULL;
  grr_data->tc.dcch_present          = FALSE;
  grr_data->tc.v_freq_set            = FALSE;

  /*
   * values for fixed alloc
   */
  grr_data->tc.fa_ctrl.fa_type       = FA_NO_CURRENT;
  grr_data->tc.fa_ctrl.repeat_alloc  = FALSE;
  grr_data->tc.fa_ctrl.ts_overr      = 0;

  grr_data->tc.num_of_rels_running   = 0;

  /* initialize the relevant data for uplink control block */
  grr_data->tc.v_sb_without_tbf      = FALSE;

  grr_data->tc.ul_ctrl_blk.seq[0]    = MAX_CTRL_BLK_NUM;

  for( i = 0; i < MAX_CTRL_BLK_NUM; i++ )
  {
    grr_data->tc.ul_ctrl_blk.blk[i].state = BLK_STATE_NONE;
    grr_data->tc.ul_ctrl_blk.blk[i].owner = CGRLC_BLK_OWNER_NONE;
  }

  grr_data->tc.two_2p_w_4_tbf_con           = TRUE;


  grr_data->tc.two_2p_w_4_tbf_con    = TRUE;

  /*
   * Initialise service name (uses define SERVICE_NAME_* in GRR.H);
   */

  INIT_STATE(TC,TC_ACCESS_DISABLED);
  /*
   *  initialize primitive queue
   */

  grr_data->tc.p_assign = NULL;

  grr_data->uplink_tbf.ts_mask    = 0;
  grr_data->uplink_tbf.ts_usage   = 0;
  grr_data->downlink_tbf.ts_mask  = 0;
  grr_data->downlink_tbf.ts_usage = 0;
  grr_data->tc.n_acc_req          = 0;

#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
  grr_data->tc.tbf_est_pacch = FALSE;
#endif

} /* tc_init() */





/*
+------------------------------------------------------------------------------
| Function    : tc_handle_error_pim
+------------------------------------------------------------------------------
| Description : The function tc_handle_error_pim() ...
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_handle_error_pim ( void )
{
  TRACE_FUNCTION( "tc_handle_error_pim" );
  TRACE_EVENT( "tc_handle_error_pim" );
  
  if( grr_data->tc.v_sb_without_tbf )
  {
    tc_set_stop_ctrl_blk( FALSE, CGRLC_BLK_OWNER_NONE, 0 );
    
      tc_cgrlc_access_status_req();
    }
  else
  {
      tc_cgrlc_ul_tbf_res ( CGRLC_TBF_MODE_ESTABLISHMENT_FAILURE, CGRLC_PRIM_STATUS_ONE );
    }
  
} /* tc_handle_error_pim() */

/*
+------------------------------------------------------------------------------
| Function    : tc_handle_error_ra
+------------------------------------------------------------------------------
| Description : The function tc_handle_error_ra() handles actions related
|               to errors that leads to randam access procedure
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_handle_error_ra ( void )
{
  TRACE_FUNCTION( "tc_handle_error_ra" );


  /*
   * - called in case of contention resulution failed in RU
   * - t3168 expires contention resulution failed in 2P-Access
   * - If the mobile station has been assigned more PDCHs than it supports according
   *  to its MS multislot class, the mobile station shall reinitiate the packet access
   *  procedure unless it has already been repeated 4 times. In that
   *  case, TBF failure has occurred.
   */

  /*
   *  cancel access procedure if running
   */
  if(grr_is_pbcch_present() 
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
      AND !grr_data->tc.tbf_est_pacch
#endif
    )
  {
    tc_send_ra_stop_req();
  }

  /*
   *  Kill TBF if running
   */
  tc_abort_tbf(grr_data->tbf_type);

  SET_STATE(TC,TC_PIM);



} /* tc_handle_error_ra() */

/*
+------------------------------------------------------------------------------
| Function    : tc_activate_tbf
+------------------------------------------------------------------------------
| Description : The function tc_activate_tbf() set the assigned TBF
|               into the data structures
|
| Parameters  : tbf_type_i - type of TBF to deactivate
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_activate_tbf ( T_TBF_TYPE tbf_type_i )
{
  TRACE_FUNCTION( "tc_activate_tbf" );

  /*
   *  Reset of n_acc_req_procedures because of the fact that number of
   *  access procedures during pacet access procedures must not considered in
   *  a error with random access occured during running TBF.
   */
 /* grr_data->tc.n_acc_req_procedures = 0;*/

  switch( tbf_type_i )
  {

  case CGRLC_TBF_MODE_UL:
    switch( grr_data->tbf_type )
    {
    case CGRLC_TBF_MODE_DL:
      grr_data->tbf_type = CGRLC_TBF_MODE_DL_UL;
      break;
    case CGRLC_TBF_MODE_NULL:
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
    case CGRLC_TBF_MODE_2PA:
#endif
      grr_data->tbf_type = CGRLC_TBF_MODE_UL;
      break;
    default:
      break;
    }
    break;

  case CGRLC_TBF_MODE_DL:
    switch( grr_data->tbf_type )
    {
    case CGRLC_TBF_MODE_UL:
      grr_data->tbf_type = CGRLC_TBF_MODE_DL_UL;
      break;
    case CGRLC_TBF_MODE_NULL:
      grr_data->tbf_type = CGRLC_TBF_MODE_DL;
      break;
    default:
      break;
    }
    break;
  case CGRLC_TBF_MODE_DL_UL:
    switch( grr_data->tbf_type )
    {
    case CGRLC_TBF_MODE_UL:
    case CGRLC_TBF_MODE_DL:
    case CGRLC_TBF_MODE_DL_UL:
      grr_data->tbf_type = CGRLC_TBF_MODE_DL_UL;
      break;
    default:
      {
        TRACE_ERROR("FATAL ERROR: tc_activate_tbf called with wrong tbf_type");
      }
      break;
    }
    break;
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
  case CGRLC_TBF_MODE_2PA:
    switch( grr_data->tbf_type )
    {
      case  CGRLC_TBF_MODE_UL:
        if (grr_data->tc.tbf_est_pacch)
        {
          grr_data->tbf_type = CGRLC_TBF_MODE_2PA;
        }
        else
        {
          TRACE_ERROR("FATAL ERROR: tc_activate_tbf called with wrong tbf_type");
        }
        break;
    
      default:
        TRACE_ERROR("FATAL ERROR: tc_activate_tbf called with wrong tbf_type:CGRLC_TBF_MODE_2PA");
        break;
    }
    break;
#endif
  default:
    {
      TRACE_ERROR("FATAL ERROR: tc_activate_tbf called with wrong tbf_type");
    }
    break;
  } /* switch (tbf_type_i) */


} /* tc_activate_tbf() */


/*
+------------------------------------------------------------------------------
| Function    : tc_deactivate_tbf
+------------------------------------------------------------------------------
| Description : The function tc_deactivate_tbf() removes a TBF logical from TC
|               end estimates how to continue.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_deactivate_tbf ( T_TBF_TYPE tbf_i )
{
  TRACE_FUNCTION( "tc_deactivate_tbf" );

#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
  if((tbf_i EQ CGRLC_TBF_MODE_UL) OR 
     (tbf_i EQ CGRLC_TBF_MODE_2PA ))
  {
    grr_data->tc.tbf_est_pacch = FALSE;
  }
#endif

  switch( grr_data->tbf_type )
  {

  case CGRLC_TBF_MODE_DL_UL:
    switch( tbf_i )
    {
    case CGRLC_TBF_MODE_UL:
      grr_data->tbf_type = CGRLC_TBF_MODE_DL;
      break;
    case CGRLC_TBF_MODE_DL:
      grr_data->tbf_type = CGRLC_TBF_MODE_UL;
      break;
    case CGRLC_TBF_MODE_DL_UL:
      grr_data->tbf_type = CGRLC_TBF_MODE_NULL;
      break;
    }
    break;

  case CGRLC_TBF_MODE_DL:
  case CGRLC_TBF_MODE_UL:
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
  case CGRLC_TBF_MODE_2PA:  
#endif
      grr_data->tbf_type = CGRLC_TBF_MODE_NULL;
    break;

  default:
    break;
  }

  switch( grr_data->tbf_type )
  {
  case CGRLC_TBF_MODE_DL:
    SET_STATE(TC,TC_TBF_ACTIVE);    
    /*  
     * downlink tbf is running --> TC_TBF_ACTIVE 
     */
    break;

  case CGRLC_TBF_MODE_UL:
    /*
     *  uplink tbf is running, no state change required
     */
    break;

  case CGRLC_TBF_MODE_NULL:
    SET_STATE(TC,TC_PIM);
    /*
     *  no tbf is running, enter TC_PIM
     */
     break;

  default:
    break;
  }


} /* tc_deactivate_tbf() */


/*
+------------------------------------------------------------------------------
| Function    : tc_start_timer_t3172
+------------------------------------------------------------------------------
| Description : The function tc_start_timer_t3172()
|               SZML-TC/085 this timer start
|               caused p access reject will be done
|               with cell reselection procedures
|
| Parameters  : none
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_start_timer_t3172 ( ULONG t3172_value_i )
{

  if(t3172_value_i > 10000)
  {
    TRACE_EVENT_P1( "T3172 start %d s", (t3172_value_i/1000));
  }
  else
  {
    TRACE_EVENT_P1( "T3172 start %d ms", t3172_value_i);
  }
  vsi_t_start(GRR_handle,T3172_1,t3172_value_i);

} /* tc_start_timer_t3172() */



/*
+------------------------------------------------------------------------------
| Function    : tc_handle_tbf_start
|------------------------------------------------------------------------------
| Description : The function tc_handle_tbf_start() handles the TBF-Starting-
|               Time in a Packet Uplink Assignment.
|
| Parameters  : tbf_type - type of tbf to be handled,
|               tbf_wait - pointer to variable to store the calculated timmer
|               value
+------------------------------------------------------------------------------
*/
GLOBAL void tc_handle_tbf_start( T_TBF_TYPE tbf_type )
{
  TRACE_FUNCTION( "tc_handle_tbf_start" );


  tc_send_assign_req(tbf_type);
  tc_activate_tbf(tbf_type);

  if((tbf_type EQ CGRLC_TBF_MODE_UL))
  {
    vsi_t_stop(GRR_handle,T3162);
    vsi_t_stop(GRR_handle,T3168);
    vsi_t_stop(GRR_handle,T3170);
    vsi_t_stop(GRR_handle,T3186);

    tc_cgrlc_ul_tbf_res(CGRLC_TBF_MODE_UL,CGRLC_PRIM_STATUS_NULL);
  }
  else if(tbf_type EQ CGRLC_TBF_MODE_DL)
  {
    tc_cgrlc_dl_tbf_req();
    if(grr_data->tc.v_sb_without_tbf EQ TRUE)
    {
    /*When Single Block is request for Measurement Report
      but network has sent the DL assignment, then this measurement report
      is sent to GRLC to be sent as normal control block*/
      tc_send_control_msg_to_grlc();
      grr_data->tc.v_sb_without_tbf = FALSE;
    }
  }
  else if(tbf_type EQ CGRLC_TBF_MODE_DL_UL)
  {
    vsi_t_stop(GRR_handle,T3168);
    tc_cgrlc_ul_tbf_res(CGRLC_TBF_MODE_UL,CGRLC_PRIM_STATUS_NULL);
    tc_cgrlc_dl_tbf_req();
  }
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
  else if(tbf_type EQ CGRLC_TBF_MODE_2PA)
  {
    tc_cgrlc_ul_tbf_res(CGRLC_TBF_MODE_2PA,CGRLC_PRIM_STATUS_NULL);
  }
#endif
}  /* tc_handle_tbf_start()  */

/*
+------------------------------------------------------------------------------
| Function    : tc_prepare_handle_tbf_start
|------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
+------------------------------------------------------------------------------
*/
GLOBAL void tc_prepare_handle_tbf_start ( T_TBF_TYPE tbf_type,
                                          UBYTE      new_state,
                                          UBYTE      old_state )
{
  MCAST( p_d_dl_assign, D_DL_ASSIGN );

  TRACE_FUNCTION( "tc_prepare_handle_tbf_start" );

  if( grr_is_pbcch_present( )          AND 
      ( 
        old_state EQ TC_PIM            OR
        old_state EQ TC_WAIT_ASSIGN    OR
        old_state EQ TC_WAIT_2P_ASSIGN OR
        old_state EQ TC_POLLING        OR
        old_state EQ TC_WAIT_STOP_TASK_CNF        
      )
    )
  {
    UBYTE ccch_read;

    if( p_d_dl_assign->msg_type EQ D_DL_ASSIGN )
    {
      ccch_read = ( p_d_dl_assign->pda_trnc_grp.v_tbf_s_time ? DONT_STOP_CCCH : STOP_CCCH );
    }
    else
    {
      MCAST( p_d_ul_assign, D_UL_ASSIGN );

      if( ( p_d_ul_assign->v_dyn_alloc_p            AND
            p_d_ul_assign->dyn_alloc_p.v_tbf_s_time     ) OR
            p_d_ul_assign->v_sin_alloc                    OR
            p_d_ul_assign->v_f_alloc_ul                      )
      {
        ccch_read = DONT_STOP_CCCH;
      }
      else
      {
        ccch_read = STOP_CCCH;
      }
    }

    grr_data->tc.last_eval_assign.ccch_read = ccch_read;
    grr_data->tc.last_eval_assign.tbf_type  = tbf_type;
    grr_data->tc.last_eval_assign.state     = new_state;

    SET_STATE( TC, TC_WAIT_STOP_TASK_CNF );

    tc_malloc_assign( );
    if(old_state NEQ TC_WAIT_STOP_TASK_CNF)
    {
      sig_tc_ctrl_set_pckt_mode( GLBL_PCKT_MODE_ASSIGNED, ccch_read );
    }
  }
  else
  {
    tc_handle_tbf_start( tbf_type );

#ifdef _TARGET_

    if( p_d_dl_assign->msg_type     EQ  D_DL_ASSIGN AND 
        grr_data->downlink_tbf.tbf_start_fn NEQ 0xFFFFFFFF      )
    {
      sig_tc_ctrl_set_pckt_mode( GLBL_PCKT_MODE_ACCESS, TASK_STOP_DUMMY_VALUE );
    }

#endif

  }
} /* tc_prepare_handle_tbf_start() */
/*
+------------------------------------------------------------------------------
| Function    : tc_set_fa_bitmap
+------------------------------------------------------------------------------
| Description : sets fixed allocation bitmap for requested allocation after
|               receiving a repeat allocation with ts_override
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL USHORT tc_set_fa_bitmap( UBYTE ts_mask, T_FIX_ALLOC* ptr_alloc)
{
  UBYTE  i;
  USHORT  tx_slots=0;
  TRACE_FUNCTION( "tc_set_fa_bitmap" );

  if (ts_mask)
  {
    /*
     * add new time slots to current allocation
     */
    for(i=0;i<grr_data->tc.fa_ctrl.current_alloc.alloc.size_bitmap;i++)
    {
      ptr_alloc->alloc.bitmap[i] = grr_data->tc.fa_ctrl.current_alloc.alloc.bitmap[i] | ts_mask;
    }
  }

  return (tx_slots);

} /* tc_set_fa_bitmap() */


/*
+------------------------------------------------------------------------------
| Function    : tc_send_ul_repeat_alloc_req
+------------------------------------------------------------------------------
| Description : send the primitive MPHP_REPEAT_UL_FIXED_ALLOC_REQ
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_send_ul_repeat_alloc_req( void)
{
  PALLOC(ptr_2prim,MPHP_REPEAT_UL_FIXED_ALLOC_REQ);

  TRACE_FUNCTION( "tc_send_ul_repeat_alloc_req" );

  ptr_2prim->repeat_alloc = grr_data->tc.fa_ctrl.repeat_alloc;
  ptr_2prim->ts_override  = grr_data->tc.fa_ctrl.ts_overr;

  if(grr_data->tc.fa_ctrl.repeat_alloc)
  {
    ptr_2prim->p_tbf_start.tbf_start_present = 1;
    ptr_2prim->p_tbf_start.fn  = grr_data->tc.fa_ctrl.current_alloc.alloc_start_fn;
  }
  else
  {
    ptr_2prim->p_tbf_start.tbf_start_present = 0;
    ptr_2prim->p_tbf_start.fn                = 0xffffffff;
  }
  PSEND(hCommL1,ptr_2prim);

} /* tc_send_ul_repeat_alloc_req() */




/*
+------------------------------------------------------------------------------
| Function    : tc_eval_ts_reconf
+------------------------------------------------------------------------------
| Description : The function tc_eval_ts_reconf() evaluates a
|               Packet Timeslot reconfigure.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL T_EVAL_TS_RECONFIG tc_eval_ts_reconf ( void )
{
  MCAST(d_ts_reconfig,D_TS_RECONFIG);  /* T_D_TS_RECONFIG */
  T_EVAL_TS_RECONFIG result = E_TS_IGNORE;
  UBYTE tx_slots;
  UBYTE i,mask;

  T_GRR_STORE_TYPE grr_store_type = GRR_STORE_TYPE_NONE;

  TRACE_FUNCTION( "tc_eval_ts_reconf" );
      
  grr_data->downlink_tbf.ctrl_ack_bit = 0;   

#ifdef REL99
  /* This function is called only when TBF is active. Handling abnormal case
   * 8.7 While a TBF is in progress, if a mobile station receives a 
   * PACKET UPLINK ASSIGNMENT, PACKET UPLINK ACK/NACK or PACKET TIMESLOT RECONFIGURE 
   * message with message escape bit indicating EGPRS (resp. GPRS) contents whereas 
   * the current TBF mode is GPRS (resp. EGPRS), the mobile station shall ignore the 
   * message.
   */
  if(d_ts_reconfig->egprs_flag)
  {
    TRACE_ERROR( " EGPRS TBF received in TS_RECONFIGURE " );
    return E_TS_IGNORE;
  }
#endif

  switch(grr_data->tbf_type)
  {
    case CGRLC_TBF_MODE_UL:
      /*
       * reassignment of uplink and downlink assignment
       */
      grr_data->downlink_tbf.mac_mode    = grr_data->uplink_tbf.mac_mode;
      grr_data->downlink_tbf.t3192       = FALSE;                      /* indicates if t3192 is running*/
      grr_data->downlink_tbf.rlc_mode    = d_ts_reconfig->dl_rlc_mode;

      if(d_ts_reconfig->v_dl_tfi EQ 0)
      {
        TRACE_ERROR( "No downlnik assignment in TS_RECONFIGURE " );
        result = E_TS_RECONFIG_ERROR_RA;
        return result;
      }
      if((d_ts_reconfig->v_dyn_alloc_ts EQ 1) AND
         grr_data->uplink_tbf.mac_mode EQ FIXED_ALLOCATION)
      {
        TRACE_ERROR( "fixed alloc expected but dynamic alloc received" );
        result = E_TS_RECONFIG_ERROR_RA;
        return result;
      }
      if(d_ts_reconfig->v_f_alloc_re EQ 1 AND
         grr_data->uplink_tbf.mac_mode EQ DYNAMIC_ALLOCATION)
      {
        TRACE_ERROR( "dynamic alloc expected but fixed alloc received" );
        result = E_TS_RECONFIG_ERROR_RA;
        return result;
      }
      result = E_TS_UL_REASSIG_NEW_DL;
      break;
    case CGRLC_TBF_MODE_DL:
      grr_data->uplink_tbf.ti = 0;  /* contention resolution NOT required */
      if(d_ts_reconfig->v_dyn_alloc_ts)
      {
        if(d_ts_reconfig->dyn_alloc_ts.xdyn_alloc)
          grr_data->uplink_tbf.mac_mode =EDA;
        else
          grr_data->uplink_tbf.mac_mode =DA;
      }
      else if(d_ts_reconfig->v_f_alloc_re)
      {
        grr_data->uplink_tbf.mac_mode = FA;
      }
      /*
       * downlink tbf active,
       * assignment of uplink and downlink reassignment
       */
      if(d_ts_reconfig->v_ul_tfi EQ 0)
      {
        TRACE_ERROR( "No uplnik tfi in TS_RECONFIGURE" );
        result = E_TS_RECONFIG_ERROR_RA;
        return result;
      }
      if ((grr_data->downlink_tbf.rlc_mode NEQ d_ts_reconfig->dl_rlc_mode) AND !d_ts_reconfig->ctrl_ack)
      {
        TRACE_ERROR("Diff rlc mode in TS_RECONFIGURE..msg rejected..aborting");
        result = E_TS_RECONFIG_ERROR_RA;
        return result;
      }
      grr_data->downlink_tbf.rlc_mode    = d_ts_reconfig->dl_rlc_mode;
      result = E_TS_NEW_UL_DL_REASSIG;
      break;
    case CGRLC_TBF_MODE_DL_UL:
      
      if((d_ts_reconfig->v_dyn_alloc_ts EQ 1) AND
         grr_data->uplink_tbf.mac_mode EQ FIXED_ALLOCATION)
      {
        TRACE_ERROR( "fixed alloc expected but dynamic alloc received" );
        result = E_TS_RECONFIG_ERROR_RA;
        return result;
      }
      if(d_ts_reconfig->v_f_alloc_re EQ 1 AND
         grr_data->uplink_tbf.mac_mode EQ DYNAMIC_ALLOCATION)
      {
        TRACE_ERROR( "dynamic alloc expected but fixed alloc received" );
        result = E_TS_RECONFIG_ERROR_RA;
        return result;
      }
      if ((grr_data->downlink_tbf.rlc_mode NEQ d_ts_reconfig->dl_rlc_mode) AND !d_ts_reconfig->ctrl_ack)
      {
        TRACE_ERROR("Diff rlc mode in TS_RECONFIGURE..msg rejected..aborting");
        result = E_TS_RECONFIG_ERROR_RA;
        return result;
      }
      grr_data->downlink_tbf.rlc_mode    = d_ts_reconfig->dl_rlc_mode;
      result = E_TS_UL_REASSIG_DL_REASSIG;
      break;
   default:
      {
        TRACE_ERROR( " tc_eval_ts_reconf unexpected" );
        return E_TS_IGNORE;
      }
  }

  /*
   * set DOWNLINK data base
   */

  grr_data->downlink_tbf.ts_usage = d_ts_reconfig->dl_tn_alloc;  
  grr_data->downlink_tbf.nts      = grr_calc_nr_of_set_bits(grr_data->downlink_tbf.ts_usage);

  if(d_ts_reconfig->ctrl_ack)
  {
    if(grr_data->downlink_tbf.t3192)
    {
      grr_data->downlink_tbf.ctrl_ack_bit= 1;
      grr_data->downlink_tbf.t3192 = FALSE;
    }
    else
    {
      result = E_TS_RECONFIG_ERROR_RA;
      TRACE_EVENT("TS_RECONF INVALID->error_ra, ctrl ack bit set but t3192 is not running");
      return result;
    }  
  }

  /*
   * set UPLINK data base
   */
    grr_data->uplink_tbf.cs_mode      = d_ts_reconfig->chan_coding_cmd; 
    grr_data->uplink_tbf.tlli_cs_mode = 1;
    grr_data->uplink_tbf.polling_bit  = 0xFF;
    grr_data->uplink_tbf.ti           = 0;

  /*
   * set glob parameters
   */

  if(d_ts_reconfig->v_freq_par)
  {
    /* SZML-TC/091 */
    /*
     *  here is also to consider a possiblely existing dl_tbf slot ussage:SZML-TC/092
     */
  }
  if(d_ts_reconfig->v_dyn_alloc_ts)
  {
    tx_slots = 0;
    grr_data->uplink_tbf.ts_usage = 0;
    if(d_ts_reconfig->dyn_alloc_ts.v_tn_alloc)
    {
      for(i = 0,mask = 0x80;i < 8;i++)
      {
        if(d_ts_reconfig->dyn_alloc_ts.tn_alloc[i].v_usf)
        {
          grr_data->uplink_tbf.ts_usage |= mask;
          tx_slots++;
        }
        mask >>= 1;
      }
    }
    else if(d_ts_reconfig->dyn_alloc_ts.v_tn_alloc_pwr)
    {
      for(i = 0,mask = 0x80;i < 8;i++)
      {
        if(d_ts_reconfig->dyn_alloc_ts.tn_alloc_pwr.usf_array[i].v_usf_g)
        {
          grr_data->uplink_tbf.ts_usage |= mask;
          tx_slots++;
        }
        mask >>= 1;
      }

      grr_store_type = GRR_STORE_TYPE_TN_ALLOC_PWR;
    }
    grr_data->uplink_tbf.nts = tx_slots;
  }
  else if(d_ts_reconfig->v_f_alloc_re)
  {
    grr_data->uplink_tbf.ts_usage = 0;
    tx_slots=0;
    if(d_ts_reconfig->f_alloc_re.v_ul_ts_alloc)
    {
      grr_data->uplink_tbf.ts_usage = d_ts_reconfig->f_alloc_re.ul_ts_alloc;
      /*calculate number of timeslots*/
      tx_slots = grr_calc_nr_of_set_bits(grr_data->uplink_tbf.ts_usage);
    }
    else if(d_ts_reconfig->f_alloc_re.v_pwr_par)
    {
      for(i = 0,mask = 0x80;i < 8;i++)
      {
        if(d_ts_reconfig->f_alloc_re.pwr_par.gamma_tn[i].v_gamma)
        {
          grr_data->uplink_tbf.ts_usage |= mask;
          tx_slots++;
        }
        mask >>= 1;
      }

      grr_store_type = GRR_STORE_TYPE_PWR_PAR;
    }
    grr_data->uplink_tbf.nts = tx_slots;
  }
  else
  {
    TRACE_ERROR ("Packet Timeslot Reconfigure without allocation type - corrupt airmessage!");
  }

  if( !handle_ms_cap(UL_DL_ASSIGNMENT) )
  {
    result = E_TS_RECONFIG_ERROR_RA;
    TRACE_EVENT("tc_eval_.. will return E_TS_RECONFIG_ERROR_RA (ms class error)");
  }

  if( tc_check_and_save_freq( ) EQ FALSE )
  {
    result = E_TS_RECONFIG_ERROR_RA;
    TRACE_EVENT("tc_eval_.. will return E_TS_RECONFIG_ERROR_RA (frequencies wrong)");
  }

  if( result NEQ E_TS_RECONFIG_ERROR_RA AND
      result NEQ E_TS_IGNORE                )
  {
    switch( grr_store_type )
    {
      case GRR_STORE_TYPE_TN_ALLOC_PWR:
        grr_store_type_tn_alloc_pwr( &d_ts_reconfig->dyn_alloc_ts.tn_alloc_pwr );
        break;

      case GRR_STORE_TYPE_PWR_PAR:
        grr_store_type_pwr_par( &d_ts_reconfig->f_alloc_re.pwr_par, FALSE );
        break;

      default:
        /* do nothing */
        break;
    }
  }

  return result;
} /* tc_eval_ts_reconf() */








/*
+------------------------------------------------------------------------------
| Function    : tc_stop_timer_t3172
+------------------------------------------------------------------------------
| Description : The function tc_stop_timer_t3172()
|               SZML-TC/094 this timer start
|               caused p access reject will be done
|               with cell reselection procedures
|
| Parameters  : none
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_stop_timer_t3172 ( void )
{
  TRACE_FUNCTION( "tc_stop_timer_t3172" );

  vsi_t_stop(GRR_handle,T3172_1);

} /* tc_stop_timer_t3172() */

/*
+------------------------------------------------------------------------------
| Function    : tc_close_gaps_in_ctrl_blk_seq
+------------------------------------------------------------------------------
| Description : The function tc_close_gaps_in_ctrl_blk_seq reorders the queue
|               holding the order which is used to identify the next control
|               block to sent.
|
| Parameters  : index - at this index the reordering starts
|
+------------------------------------------------------------------------------
*/
LOCAL void tc_close_gaps_in_ctrl_blk_seq ( UBYTE index )
{
  TRACE_FUNCTION( "tc_close_gaps_in_ctrl_blk_seq" );

  while( index                               <   MAX_CTRL_BLK_NUM AND
         grr_data->tc.ul_ctrl_blk.seq[index] NEQ MAX_CTRL_BLK_NUM     )
  {
    grr_data->tc.ul_ctrl_blk.seq[index-1] = grr_data->tc.ul_ctrl_blk.seq[index];

    index++;
  }

  grr_data->tc.ul_ctrl_blk.seq[index-1] = MAX_CTRL_BLK_NUM;

} /* tc_close_gaps_in_ctrl_blk_seq() */

/*
+------------------------------------------------------------------------------
| Function    : tc_store_ctrl_blk
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL tc_store_ctrl_blk ( T_BLK_OWNER blk_owner, void *blk_struct )
{
  UBYTE       i;                 /* used for counting                  */
  BOOL        result    = FALSE; /* indicates whether control block is */
                                 /* stored successfully or not         */
  T_BLK_INDEX blk_index;         /* index to the control block buffer  */

  TRACE_FUNCTION( "tc_store_ctrl_msg" );

  /*
   * we have to find a free buffer for the control block which is
   * indicated by the index to the control block buffer
   */
  blk_index = MAX_CTRL_BLK_NUM;

  switch( blk_owner )
  {
    case( CGRLC_BLK_OWNER_CTRL ):
      if( grr_data->tc.ul_ctrl_blk.blk[BLK_INDEX_CTRL].state EQ BLK_STATE_NONE )
      {
        blk_index = BLK_INDEX_CTRL;
      }
      break;

    case( CGRLC_BLK_OWNER_CS ):
      if( grr_data->tc.ul_ctrl_blk.blk[BLK_INDEX_CS].state EQ BLK_STATE_NONE )
      {
        blk_index = BLK_INDEX_CS;
      }
      break;

    case( CGRLC_BLK_OWNER_TM ):
      if( grr_data->tc.ul_ctrl_blk.blk[BLK_INDEX_TC].state EQ BLK_STATE_NONE )
      {
        blk_index = BLK_INDEX_TC;
      }
      break;

    case( CGRLC_BLK_OWNER_MEAS ):

      blk_index = BLK_INDEX_MEAS;

      while( blk_index                                     <   MAX_CTRL_BLK_NUM AND
             grr_data->tc.ul_ctrl_blk.blk[blk_index].state NEQ BLK_STATE_NONE       )
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
           grr_data->tc.ul_ctrl_blk.seq[i] NEQ MAX_CTRL_BLK_NUM     )
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
      grr_encode_ul_ctrl_block
        ( ( UBYTE* )&grr_data->tc.ul_ctrl_blk.blk[blk_index].data[0],
          ( UBYTE* )blk_struct );

      grr_data->tc.ul_ctrl_blk.blk[blk_index].state = BLK_STATE_ALLOCATED;
      grr_data->tc.ul_ctrl_blk.blk[blk_index].owner = blk_owner;
      grr_data->tc.ul_ctrl_blk.seq[i]               = blk_index;

      if( i < MAX_CTRL_BLK_NUM - 1 )
      {
        grr_data->tc.ul_ctrl_blk.seq[i+1] = MAX_CTRL_BLK_NUM;
      }

      result = TRUE;
    }
    else
    {
      TRACE_ERROR( "tc_store_ctrl_blk: no free entry in queue found" );
    }
  }

  if( result EQ FALSE )
  {
    if( blk_owner EQ CGRLC_BLK_OWNER_MEAS )
    {
      TRACE_EVENT( "tc_store_ctrl_blk: no table entry allocated" );
    }
    else
    {
      TRACE_ERROR( "tc_store_ctrl_blk: no table entry allocated" );
    }
  }

  return( result );

} /* tc_store_ctrl_blk() */

/*
+------------------------------------------------------------------------------
| Function    : tc_cancel_ctrl_blk
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL tc_cancel_ctrl_blk ( T_BLK_OWNER blk_owner )
{
  BOOL  result     = FALSE;
  UBYTE i         = 0;
  UBYTE blk_index = grr_data->tc.ul_ctrl_blk.seq[i];

  TRACE_FUNCTION( "tc_cancel_ctrl_blk" );


  while( blk_index NEQ MAX_CTRL_BLK_NUM AND
         result    EQ  FALSE                )
  {
    if( grr_data->tc.ul_ctrl_blk.blk[blk_index].owner EQ blk_owner           AND
        grr_data->tc.ul_ctrl_blk.blk[blk_index].state EQ BLK_STATE_ALLOCATED     )
    {
      /*
       * mark the entry in the queue as free
       */
      grr_data->tc.ul_ctrl_blk.blk[blk_index].state = BLK_STATE_NONE;
      grr_data->tc.ul_ctrl_blk.blk[blk_index].owner = CGRLC_BLK_OWNER_NONE;

      tc_close_gaps_in_ctrl_blk_seq( (UBYTE)( i + 1 ) );

      /*
       * notify owner of the control block, TC always runs in unacknowledged mode
       */
      if( blk_owner NEQ CGRLC_BLK_OWNER_TM )
      {
        sig_tc_ctrl_control_block_result( blk_owner, FALSE );
      }
    
      result = TRUE;
    }

    i++;
    if( i < MAX_CTRL_BLK_NUM )
    {
      blk_index = grr_data->tc.ul_ctrl_blk.seq[i];
    }
    else
    {
      blk_index = MAX_CTRL_BLK_NUM;
    }
  }

  if( result EQ FALSE )
  {
    TRACE_EVENT( "tc_cancel_ctrl_blk: no block found" );
  }

  return( result );

} /* tc_cancel_ctrl_blk() */

/*
+------------------------------------------------------------------------------
| Function    : tc_set_start_ctrl_blk
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE* tc_set_start_ctrl_blk ( UBYTE *index )
{
  UBYTE       i         = 0;
  T_BLK_INDEX blk_index = MAX_CTRL_BLK_NUM;

  TRACE_FUNCTION( "tc_set_start_ctrl_blk" );

  while( i                               <   MAX_CTRL_BLK_NUM AND
         grr_data->tc.ul_ctrl_blk.seq[i] NEQ MAX_CTRL_BLK_NUM AND
         blk_index                       EQ  MAX_CTRL_BLK_NUM     )
  {
    blk_index = grr_data->tc.ul_ctrl_blk.seq[i];

    if( grr_data->tc.ul_ctrl_blk.blk[blk_index].state EQ BLK_STATE_ALLOCATED )
    {
      grr_data->tc.ul_ctrl_blk.blk[blk_index].state = BLK_STATE_SENT_REQ;

      /*
       * the control block was encoded without having correct information
       * about the value of the R bit. So we have to set the correct value now.
       */
      grr_data->tc.ul_ctrl_blk.blk[blk_index].data[0] =
                               grr_get_ul_ctrl_block_header( grr_data->r_bit );
    }
    else
    {
      blk_index = MAX_CTRL_BLK_NUM;
    }

    i++;
  }

  *index = blk_index;

  return( blk_index EQ MAX_CTRL_BLK_NUM ?
                         NULL : grr_data->tc.ul_ctrl_blk.blk[blk_index].data );

} /* tc_set_start_ctrl_blk() */

/*
+------------------------------------------------------------------------------
| Function    : tc_set_stop_ctrl_blk
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL T_BLK_INDEX tc_set_stop_ctrl_blk
        ( BOOL is_tx_success, T_BLK_OWNER srch_owner, T_BLK_INDEX start_index )
{
  T_BLK_INDEX blk_index = grr_data->tc.ul_ctrl_blk.seq[start_index];
  T_BLK_INDEX nxt_index;

  TRACE_FUNCTION( "tc_set_stop_ctrl_blk" );

  if( blk_index < MAX_CTRL_BLK_NUM )
  {
    T_BLK_OWNER blk_owner = grr_data->tc.ul_ctrl_blk.blk[blk_index].owner;

    if( srch_owner EQ CGRLC_BLK_OWNER_NONE OR
        srch_owner EQ blk_owner         )
    {
      /*
       * mark the entry in the queue as free
       */
      grr_data->tc.ul_ctrl_blk.blk[blk_index].state = BLK_STATE_NONE;
      grr_data->tc.ul_ctrl_blk.blk[blk_index].owner = CGRLC_BLK_OWNER_NONE;

      tc_close_gaps_in_ctrl_blk_seq( 1 );

      /*
       * notify owner of the control block, TC always runs in unacknowledged mode
       */
      if( blk_owner NEQ CGRLC_BLK_OWNER_TM )
      {
        sig_tc_ctrl_control_block_result( blk_owner, is_tx_success );
      }

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

} /* tc_set_stop_ctrl_blk() */

/*
+------------------------------------------------------------------------------
| Function    : tc_set_stop_tc_ctrl_blk
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_set_stop_tc_ctrl_blk ( void )
{
  T_BLK_INDEX start_index = 0;

  TRACE_FUNCTION( "tc_set_stop_tc_ctrl_blk" );

  while
  (
    ( start_index = tc_set_stop_ctrl_blk( FALSE, CGRLC_BLK_OWNER_TM, start_index ) )
                                                           NEQ MAX_CTRL_BLK_NUM
                                                           ){};
} /* tc_set_stop_tc_ctrl_blk() */

/*
+------------------------------------------------------------------------------
| Function    : tc_set_stop_all_ctrl_blk
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_set_stop_all_ctrl_blk ( void )
{
  T_BLK_INDEX start_index = 0;

  TRACE_FUNCTION( "tc_set_stop_all_ctrl_blk" );

  while
  (
    ( start_index = tc_set_stop_ctrl_blk( FALSE,
                                          CGRLC_BLK_OWNER_NONE,
                                          start_index ) ) NEQ MAX_CTRL_BLK_NUM
                                          ){};
} /* tc_set_stop_all_ctrl_blk() */


/*
+------------------------------------------------------------------------------
| Function    : tc_check_access_is_needed
+------------------------------------------------------------------------------
| Description : The function tc_check_access_is_needed () ....
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_check_access_is_needed ( T_CHECK_ACCESS_CAUSE cause )
{
  TRACE_FUNCTION( "tc_check_access_is_needed " );

  if( ((grr_t_status( T3172_1 ) > 0) OR (grr_t_status( T3170 ) > 0) ) AND
      (grr_t_status( T3176 ) EQ 0))
  {
    if( cause EQ CAC_T3170_EXPIRED )
    {
      sig_tc_ctrl_set_pckt_mode( GLBL_PCKT_MODE_IDLE, TASK_STOP_DUMMY_VALUE );
    }

    TRACE_EVENT_P3("ACCESS_REJ RUNNING t3170=%d t3172=%d t3176=%d",grr_t_status( T3170 )
                                                                  ,grr_t_status( T3172_1 )
                                                                  ,grr_t_status( T3176 ));
    return;
  }

  switch(grr_data->tbf_type)
  {
    case CGRLC_TBF_MODE_NULL:
      if(grr_data->tc.num_of_rels_running)
      {
        TRACE_EVENT_P1("WAIT FOR %d REL CON ", grr_data->tc.num_of_rels_running);
        return;
      }
      SET_STATE( TC, TC_WAIT_STOP_TASK_CNF );
      sig_tc_ctrl_set_pckt_mode( GLBL_PCKT_MODE_ACCESS, TASK_STOP_DUMMY_VALUE );
     

    break;
    case CGRLC_TBF_MODE_DL:
    case CGRLC_TBF_MODE_UL:
    case CGRLC_TBF_MODE_DL_UL:
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
    case CGRLC_TBF_MODE_2PA:
#endif
      break;
    default:
      TRACE_ERROR("unknown tbf type during uplink access");
      break;
  }
} /* tc_check_access_is_needed  */

/*
+------------------------------------------------------------------------------
| Function    : tc_stop_normal_burst_req
+------------------------------------------------------------------------------
| Description : this function sends the primitive MPHP_STOP_SINGLE_BLOCK_REQ
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_stop_normal_burst_req ( void )
{
  TRACE_FUNCTION( "tc_stop_normal_burst_req" );

  {
    PALLOC(mphp_stop_single_block_req,MPHP_STOP_SINGLE_BLOCK_REQ);
    PSEND(hCommL1,mphp_stop_single_block_req);
  }
} /* tc_stop_normal_burst_req() */



/*
+------------------------------------------------------------------------------
| Function    : tc_malloc_assign
+------------------------------------------------------------------------------
| Description : The function tc_malloc_assign() .... 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_malloc_assign ( void )
{ 
  MCAST( p_d_dl_assign, D_DL_ASSIGN );

  TRACE_FUNCTION( "tc_malloc_assign" );

  if( grr_data->tc.p_assign EQ NULL )
  {
    if( p_d_dl_assign->msg_type EQ D_DL_ASSIGN )
    {
      MALLOC( grr_data->tc.p_assign, sizeof( T_D_DL_ASSIGN ) );
    }
    else
    {
      MALLOC( grr_data->tc.p_assign, sizeof( T_D_UL_ASSIGN ) );
    }
  }
  else
  {
    TRACE_ERROR( "tc_malloc_assign: fatal error, memory reallocation not implemented" );
  }

  if( p_d_dl_assign->msg_type EQ D_DL_ASSIGN )
  {
    *grr_data->tc.p_assign = *( ( T_D_DL_ASSIGN * )_decodedMsg );
  }
  else
  {
    *( ( T_D_UL_ASSIGN * )grr_data->tc.p_assign ) = *( ( T_D_UL_ASSIGN * )_decodedMsg );
  }
} /* tc_malloc_assign() */

/*
+------------------------------------------------------------------------------
| Function    : tc_mfree_assign
+------------------------------------------------------------------------------
| Description : The function tc_mfree_assign() .... 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_mfree_assign ( BOOL restore_data )
{ 
  TRACE_FUNCTION( "tc_mfree_assign" );

  if( grr_data->tc.p_assign NEQ NULL )
  {
    if( restore_data )
    {
      if( grr_data->tc.p_assign->msg_type EQ D_DL_ASSIGN )
      {
        *( ( T_D_DL_ASSIGN * )_decodedMsg ) = *grr_data->tc.p_assign;
      }
      else
      {
        *( ( T_D_UL_ASSIGN * )_decodedMsg ) = *( ( T_D_UL_ASSIGN * )grr_data->tc.p_assign );
      }
    }

    MFREE( grr_data->tc.p_assign );
    grr_data->tc.p_assign = NULL;
  }
  else
  {
    TRACE_ERROR( "tc_mfree_assign: fatal error, no memory allocated" );
  }
} /* tc_mfree_assign() */

/*
+------------------------------------------------------------------------------
| Function    : tc_prepare_send_tbf_release_req
+------------------------------------------------------------------------------
| Description : The function tc_prepare_send_tbf_release_req() .... 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
LOCAL void tc_prepare_send_tbf_release_req( T_TBF_TYPE tbf_type )
{ 
  TRACE_FUNCTION( "tc_prepare_send_tbf_release_req" );

  if( grr_is_pbcch_present( ) AND tbf_type EQ grr_data->tbf_type )
  {
    grr_data->tc.last_tbf_type = tbf_type;
  
    grr_data->tc.num_of_rels_running++;

    sig_tc_ctrl_set_pckt_mode( GLBL_PCKT_MODE_RELEASED, TASK_STOP_DUMMY_VALUE );
  }
  else
  {
    tc_send_tbf_release_req( tbf_type, TRUE );
  }
} /* tc_prepare_send_tbf_release_req() */

/*
+------------------------------------------------------------------------------
| Function    : tc_send_tbf_release_req
+------------------------------------------------------------------------------
| Description : The function tc_send_tbf_release_req() .... 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_send_tbf_release_req( T_TBF_TYPE tbf_type, BOOL is_synchron )
{ 
  UBYTE rel_req_tbf_type;
  UBYTE transid;

  TRACE_FUNCTION( "tc_send_tbf_release_req" );

  switch( tbf_type )
  {
    case CGRLC_TBF_MODE_UL:    rel_req_tbf_type = UL_ASSIGNMENT;    break;
    case CGRLC_TBF_MODE_DL:    rel_req_tbf_type = DL_ASSIGNMENT;    break;
    case CGRLC_TBF_MODE_DL_UL: rel_req_tbf_type = UL_DL_ASSIGNMENT; break;
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
    case CGRLC_TBF_MODE_2PA:   rel_req_tbf_type = UL_TP_ACCESS;     break;
#endif

    default:
      TRACE_ERROR( "tc_send_tbf_release_req: unknown tbf type" );
      return;
  }

  {
    PALLOC(mphp_tbf_release_req,MPHP_TBF_RELEASE_REQ);
    mphp_tbf_release_req->tbf_type = rel_req_tbf_type;
    PSEND(hCommL1,mphp_tbf_release_req);

    if( is_synchron )
    {
      grr_data->tc.num_of_rels_running++;
    }
  }

  switch(rel_req_tbf_type)
  {
    case UL_ASSIGNMENT:
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
    case UL_TP_ACCESS:
#endif
      memset(&grr_data->uplink_tbf,0xEE,sizeof(T_UL_TBF));
      grr_data->test_mode = CGRLC_NO_TEST_MODE;
      break;
    case DL_ASSIGNMENT:
      transid = grr_data->downlink_tbf.trans_id;
      memset(&grr_data->downlink_tbf,0xEE,sizeof(T_DL_TBF));
      grr_data->downlink_tbf.trans_id = transid;
      break;
    case UL_DL_ASSIGNMENT:
      transid = grr_data->downlink_tbf.trans_id;
      memset(&grr_data->uplink_tbf,0xEE,sizeof(T_UL_TBF));
      memset(&grr_data->downlink_tbf,0xEE,sizeof(T_DL_TBF));
      grr_data->test_mode = CGRLC_NO_TEST_MODE;
      grr_data->downlink_tbf.trans_id = transid;
      break;
  }



} /* tc_send_tbf_release_req() */



/*
+------------------------------------------------------------------------------
| Function    : tc_get_t3170_value
+------------------------------------------------------------------------------
| Description : The function tc_get_t3170_value() calculates T3170 and returns it 
|
| Parameters  : return T_TIME
|
+------------------------------------------------------------------------------
*/
GLOBAL T_TIME tc_get_t3170_value(void)
{
  T_TIME t3170_value;

  t3170_value = s_table[psc_db->prach.s_prach];

  if(S_VALUE_RESERVED EQ t3170_value)
    t3170_value = 0;
  
  t3170_value *= 2;
  t3170_value += tx_int_table[psc_db->prach.tx_int];
  /* T3170 value = TX_INT + 2 * S from prach control parameters */
  /* see gsm 04.60 13.1 */
  t3170_value *= 5;  /* 1 tdma frame ~ 5ms */
  TRACE_EVENT_P1("T3170: %ld", t3170_value);
  return t3170_value;
}




/*
+------------------------------------------------------------------------------
| Function    : tc_call_disable_callback_func
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_call_disable_callback_func ( void )
{
  if( grr_data->tc.disable_callback_func NEQ NULL )
  {
    T_TC_DISABLE_CALLBACK_FUNC disable_callback_func;

    TRACE_EVENT( "Call disable_callback_func after TBF release" );

    disable_callback_func              = grr_data->tc.disable_callback_func;
    grr_data->tc.disable_callback_func = NULL;
    
    disable_callback_func( );
  }
} /* tc_call_disable_callback_func */

/*
+------------------------------------------------------------------------------
| Function    : tc_set_freq
+------------------------------------------------------------------------------
| Description : This function allocates a primitve depending on the hopping status
|               and sets the frequency parameter of T_MPHP_ASSIGNMENT_REQ
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL T_MPHP_ASSIGNMENT_REQ * tc_set_freq ( void )
{
  PALLOC( ptr2prim, MPHP_ASSIGNMENT_REQ );

  TRACE_FUNCTION( "tc_set_freq" );

  memset( ptr2prim, 0, sizeof( T_MPHP_ASSIGNMENT_REQ ) );

  ptr2prim->tsc = grr_data->tc.freq_set.tsc;

  grr_set_freq_par( &ptr2prim->p_frequency_par );

  return( ptr2prim );
}/* tc_set_freq */

/*
+------------------------------------------------------------------------------
| Function    : tc_cgrlc_disable_req
+------------------------------------------------------------------------------
| Description : This function allocates sends the the primtive CGRLC_DISABLE_REQ
|               to GRLC
|
| Parameters  : prim_st - indicates if a primitive shall be deleted or not
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_cgrlc_disable_req        ( UBYTE prim_st )
{
  PALLOC(cgrlc_disable_req,CGRLC_DISABLE_REQ); /*T_CGRLC_DISABLE_REQ*/

  TRACE_FUNCTION( "tc_cgrlc_disable_req" );

  cgrlc_disable_req->disable_class = grr_data->tc.disable_class;
  cgrlc_disable_req->prim_status   = prim_st;

  PSEND(hCommGRLC,cgrlc_disable_req);

}/* tc_cgrlc_disable_req */


/*
+------------------------------------------------------------------------------
| Function    : tc_cgrlc_ul_tbf_res
+------------------------------------------------------------------------------
| Description : This function 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/

GLOBAL void tc_cgrlc_ul_tbf_res        ( UBYTE tbf_mode, UBYTE prim_status )
{
  PALLOC(cgrlc_ul_tbf_res,CGRLC_UL_TBF_RES); /*T_CGRLC_UL_TBF_RES*/

  TRACE_FUNCTION( "tc_cgrlc_ul_tbf_res" );

  cgrlc_ul_tbf_res->starting_time = grr_data->uplink_tbf.tbf_start_fn;     
  cgrlc_ul_tbf_res->tbf_mode      = tbf_mode;          
  cgrlc_ul_tbf_res->prim_status   = prim_status;       
  cgrlc_ul_tbf_res->polling_bit   = grr_data->uplink_tbf.polling_bit;       
  cgrlc_ul_tbf_res->cs_mode       = grr_data->uplink_tbf.cs_mode;           
  cgrlc_ul_tbf_res->mac_mode      = grr_data->uplink_tbf.mac_mode;          
  cgrlc_ul_tbf_res->nts_max       = grr_data->uplink_tbf.nts;
  cgrlc_ul_tbf_res->rlc_db_granted= grr_data->uplink_tbf.rlc_db_granted;

  if(grr_data->uplink_tbf.ts_usage)
  {
    cgrlc_ul_tbf_res->tn_mask     = grr_data->uplink_tbf.ts_usage;
  }
  else
  {
    cgrlc_ul_tbf_res->tn_mask     = grr_data->uplink_tbf.ts_mask;
    TRACE_EVENT("UL tn mask passed to grlc instead of ts_usage");
  }

  if(grr_data->uplink_tbf.st_tfi EQ 0xFF)
    cgrlc_ul_tbf_res->tfi = grr_data->uplink_tbf.tfi;               
  else
    cgrlc_ul_tbf_res->tfi = grr_data->uplink_tbf.st_tfi;
  
  cgrlc_ul_tbf_res->ti            = grr_data->uplink_tbf.ti;                
  cgrlc_ul_tbf_res->bs_cv_max     = psc_db->gprs_cell_opt.bs_cv_max;         
  cgrlc_ul_tbf_res->tlli_cs_mode  = grr_data->uplink_tbf.tlli_cs_mode;
  cgrlc_ul_tbf_res->r_bit         = grr_data->r_bit;

  if(cgrlc_ul_tbf_res->mac_mode EQ FA)
  {
    cgrlc_ul_tbf_res->fix_alloc_struct.bitmap_len  = grr_data->tc.fa_ctrl.current_alloc.alloc.size_bitmap;
    cgrlc_ul_tbf_res->fix_alloc_struct.end_fn      = grr_data->tc.fa_ctrl.current_alloc.alloc_end_fn;             
    cgrlc_ul_tbf_res->fix_alloc_struct.final_alloc = grr_data->tc.fa_ctrl.current_alloc.final_alloc;        
    memcpy (cgrlc_ul_tbf_res->fix_alloc_struct.bitmap_array,
            grr_data->tc.fa_ctrl.current_alloc.alloc.bitmap,
            MAX_ALLOC_BITMAP);
  }

  grr_prcs_pwr_ctrl ( &cgrlc_ul_tbf_res->pwr_ctrl, FALSE );
  
  /*Reset the GRR database for parameters which are used only once*/
  grr_data->uplink_tbf.polling_bit=0xFF;
  
  PSEND(hCommGRLC,cgrlc_ul_tbf_res);

}/* tc_cgrlc_ul_tbf_res */



/*
+------------------------------------------------------------------------------
| Function    : tc_cgrlc_access_status_req
+------------------------------------------------------------------------------
| Description : This function 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/

GLOBAL void tc_cgrlc_access_status_req        ( void )  
{
  PALLOC(cgrlc_access_status_req,CGRLC_ACCESS_STATUS_REQ); /*T_CGRLC_ACCESS_STATUS_REQ*/

  TRACE_FUNCTION( "tc_cgrlc_access_status_req" );

  PSEND(hCommGRLC,cgrlc_access_status_req);

}/* tc_cgrlc_access_status_req*/ 


/*
+------------------------------------------------------------------------------
| Function    : tc_send_control_msg_to_grlc
+------------------------------------------------------------------------------
| Description : This function 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/

GLOBAL void tc_send_control_msg_to_grlc        ( void )
{
  UBYTE i= 0;

  TRACE_FUNCTION( "tc_send_control_msg_to_grlc" );

    
  while( i                               <   MAX_CTRL_BLK_NUM AND 
         grr_data->tc.ul_ctrl_blk.seq[i] NEQ MAX_CTRL_BLK_NUM     )
  {
    if( grr_data->tc.ul_ctrl_blk.blk[grr_data->tc.ul_ctrl_blk.seq[i]].state
                                                       EQ BLK_STATE_ALLOCATED )
    {
      PALLOC(cgrlc_data_req,CGRLC_DATA_REQ);  /* T_CGRLC_DATA_REQ */

 
      cgrlc_data_req->blk_owner  = grr_data->tc.ul_ctrl_blk.blk[grr_data->tc.ul_ctrl_blk.seq[i]].owner;        
 
      memcpy(cgrlc_data_req->data_array,grr_data->tc.ul_ctrl_blk.blk[grr_data->tc.ul_ctrl_blk.seq[i]].data,23);

      PSEND(hCommGRLC,cgrlc_data_req);
     
      /* remove control block */

      tc_cancel_ctrl_blk(grr_data->tc.ul_ctrl_blk.blk[grr_data->tc.ul_ctrl_blk.seq[i]].owner);
    }
  }


}/* tc_send_control_msg_to_grlc*/ 

/*
+------------------------------------------------------------------------------
| Function    : tc_cgrlc_dl_tbf_req
+------------------------------------------------------------------------------
| Description : This function 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/

GLOBAL void tc_cgrlc_dl_tbf_req        ( void )
{
  PALLOC(cgrlc_dl_tbf_req,CGRLC_DL_TBF_REQ); /*T_CGRLC_DL_TBF_REQ*/


  TRACE_FUNCTION( "tc_cgrlc_dl_tbf_req" );

  grr_data->downlink_tbf.trans_id++;

  cgrlc_dl_tbf_req->starting_time = grr_data->downlink_tbf.tbf_start_fn;     
  cgrlc_dl_tbf_req->rlc_mode      = grr_data->downlink_tbf.rlc_mode;          
  cgrlc_dl_tbf_req->mac_mode      = grr_data->downlink_tbf.mac_mode;  
  cgrlc_dl_tbf_req->polling_bit   = grr_data->downlink_tbf.polling_bit;
  cgrlc_dl_tbf_req->nts_max       = grr_data->downlink_tbf.nts;

  if(grr_data->downlink_tbf.ts_usage)
  {
    cgrlc_dl_tbf_req->tn_mask      = grr_data->downlink_tbf.ts_usage;
    grr_data->downlink_tbf.ts_mask = grr_data->downlink_tbf.ts_usage; /* set before starting time is elapsed */
  }
  else
  {
    cgrlc_dl_tbf_req->tn_mask     = grr_data->downlink_tbf.ts_mask;
    TRACE_EVENT("DL tn mask passed to grlc instead of ts_usage");
  }


  if(grr_data->downlink_tbf.st_tfi EQ 0xFF)
    cgrlc_dl_tbf_req->tfi = grr_data->downlink_tbf.tfi;               
  else
    cgrlc_dl_tbf_req->tfi = grr_data->downlink_tbf.st_tfi;
          
  cgrlc_dl_tbf_req->t3192_val     = psc_db->gprs_cell_opt.t3192;          
  cgrlc_dl_tbf_req->ctrl_ack_bit  = grr_data->downlink_tbf.ctrl_ack_bit;    

  grr_prcs_pwr_ctrl ( &cgrlc_dl_tbf_req->pwr_ctrl, FALSE );
  
  /*Reset the GRR database for parameters which are used only once*/
  grr_data->downlink_tbf.ctrl_ack_bit=0;
  grr_data->downlink_tbf.polling_bit=0xFF;

  PSEND(hCommGRLC,cgrlc_dl_tbf_req);


}/* tc_cgrlc_dl_tbf_req*/ 


/*
+------------------------------------------------------------------------------
| Function    : tc_cgrlc_tbf_rel_res
+------------------------------------------------------------------------------
| Description : This function 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/

GLOBAL void tc_cgrlc_tbf_rel_res ( UBYTE tbf_type )
{
  PALLOC(cgrlc_tbf_rel_res,CGRLC_TBF_REL_RES);

  TRACE_FUNCTION( "tc_cgrlc_tbf_rel_res" );

  switch( tbf_type )
  {
    case DL_ASSIGNMENT:    cgrlc_tbf_rel_res->tbf_mode = CGRLC_TBF_MODE_DL;    break;
    case UL_ASSIGNMENT:    cgrlc_tbf_rel_res->tbf_mode = CGRLC_TBF_MODE_UL;    break;
    case UL_DL_ASSIGNMENT: cgrlc_tbf_rel_res->tbf_mode = CGRLC_TBF_MODE_DL_UL; break;
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
    case UL_TP_ACCESS:     cgrlc_tbf_rel_res->tbf_mode = CGRLC_TBF_MODE_2PA;   break;
#endif
  }

  PSEND(hCommGRLC,cgrlc_tbf_rel_res);

}/* tc_cgrlc_tbf_rel_res*/ 




/*
+------------------------------------------------------------------------------
| Function    : tc_cgrlc_tbf_rel_req
+------------------------------------------------------------------------------
| Description : This function 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/

GLOBAL void tc_cgrlc_tbf_rel_req        ( UBYTE tbf_type, UBYTE rel_cause, ULONG rel_fn )
{
  PALLOC(cgrlc_tbf_rel_req,CGRLC_TBF_REL_REQ); /*T_CGRLC_TBF_REL_REQ*/
 
  TRACE_FUNCTION( "tc_cgrlc_tbf_rel_req" );
 
  cgrlc_tbf_rel_req->tbf_mode       = tbf_type; 
  cgrlc_tbf_rel_req->tbf_rel_cause  = rel_cause;       
  cgrlc_tbf_rel_req->rel_fn         = rel_fn;    

  PSEND(hCommGRLC,cgrlc_tbf_rel_req);

}/* tc_cgrlc_tbf_rel_req*/ 



/*
+------------------------------------------------------------------------------
| Function    : tc_cgrlc_enable_req
+------------------------------------------------------------------------------
| Description : This function sends the primitive cgrlc_enable_req to GRLC
|
| Parameters  : queue_mode - indicates the queue mode
|               cu_cause   - indicates if cell update is required or not 
|
+------------------------------------------------------------------------------
*/

GLOBAL void tc_cgrlc_enable_req ( UBYTE queue_mode,
                                  UBYTE cu_cause,
                                  BOOL  cell_has_changed,
                                  UBYTE enable_cause )
{
 
  PALLOC(cgrlc_enable_req,CGRLC_ENABLE_REQ); /*T_CGRLC_ENABLE_REQ*/
 
  TRACE_FUNCTION( "tc_cgrlc_enable_req" );

  cgrlc_enable_req->enable_cause       = enable_cause;
  cgrlc_enable_req->queue_mode         = queue_mode;
  cgrlc_enable_req->cu_cause           = cu_cause;                 /* cell update request will be handled sig_ctrl_tc_enable_grlc */
  /*************Burst type handling********************************/
  cgrlc_enable_req->burst_type         = psc_db->gprs_cell_opt.ctrl_ack_type; 
  cgrlc_enable_req->ab_type            = psc_db->gprs_cell_opt.ab_type;

  /*************Counter N3102 handling*****************************/
  cgrlc_enable_req->v_pan_struct     = psc_db->gprs_cell_opt.v_pan_struct;
  cgrlc_enable_req->pan_struct.inc   = psc_db->gprs_cell_opt.pan_struct.inc;
  cgrlc_enable_req->pan_struct.dec   = psc_db->gprs_cell_opt.pan_struct.dec;
  if(cell_has_changed)
    cgrlc_enable_req->pan_struct.pmax  = (psc_db->gprs_cell_opt.pan_struct.pmax + 1 ) *4;
  else
    cgrlc_enable_req->pan_struct.pmax  = CGRLC_NO_UPDATE_N3102;

  cgrlc_enable_req->ul_tlli          = grr_data->db.ms_id.new_tlli;
  cgrlc_enable_req->dl_tlli          = grr_data->db.ms_id.received_tlli;

  cgrlc_enable_req->t3168_val        = psc_db->gprs_cell_opt.t3168;
  
  if(grr_is_pbcch_present())

  {
    cgrlc_enable_req->change_mark  = psc_db->psi2_params.psi2_change_mark;
    if(grr_data->ms.access_ctrl_class & ~psc_db->prach.ac_class)
    {
      cgrlc_enable_req->ac_class = CGRLC_PCCCH_AC_ALLOWED;
    }
    else
    {
      cgrlc_enable_req->ac_class = CGRLC_PCCCH_AC_NOT_ALLOWED;
      TRACE_EVENT_P3("ACCESS NOT ALLOWED PBCCH:  ms_acc=%d prach_ac=%d tc_state=%d"
                                                                                    ,grr_data->ms.access_ctrl_class
                                                                                    ,psc_db->prach.ac_class
                                                                                    ,grr_data->tc.state);
    }
  }
  else
  {
    cgrlc_enable_req->change_mark  = psc_db->psi13_params.si13_change_mark;
    if(grr_data->ms.access_ctrl_class & ~psc_db->prach.ac_class)
    {
      cgrlc_enable_req->ac_class = psc_db->net_ctrl.priority_access_thr;
    }
    else
    {
      cgrlc_enable_req->ac_class = CGRLC_PCCCH_AC_NOT_ALLOWED;
      TRACE_EVENT_P3("ACCESS NOT ALLOWED CCCH:  ms_acc=%d prach_ac=%d   tc_state=%d"
                                                                               ,grr_data->ms.access_ctrl_class
                                                                               ,psc_db->prach.ac_class
                                                                               ,grr_data->tc.state);
    }
  }

#ifdef REL99
  cgrlc_enable_req->pfi_support = psc_db->gprs_cell_opt.gprs_ext_bits.gprs_ext_info.pfc_feature_mode;
  cgrlc_enable_req->nw_rel      = psc_db->network_rel; /*Network Release Order*/
#endif

  PSEND(hCommGRLC,cgrlc_enable_req);
 
}/* tc_cgrlc_enable_req*/ 

/*
+------------------------------------------------------------------------------
| Function    : grr_convert_11bit_2_etsi
+------------------------------------------------------------------------------
| Description : Converts the 11 bit access burst value into ETSI format
|
| Parameters  : In:  eleven bit value
|               Out: converted eleven bit
|
+------------------------------------------------------------------------------
*/
LOCAL USHORT grr_convert_11bit_2_etsi ( USHORT eleven_bit )
{
  USHORT etsi11bit;
  USHORT dummy1 = 0, dummy2 = 0;

  TRACE_FUNCTION( "grr_convert_11bit_2_etsi" );

  /*
   *  11 Bit access burst
   * b: bit
   * b10 b9 b8 b7 b6 b5 b4 b3 b2 b1 b0
   * should be sent to the network -according 04.60 and 0404- in the
   * following 16-bit format:
   * 0 0 0 0 0 b2 b1 b0 b10 b9 b8 b7 b6 b5 b4 b3
   */

  /*
   * get b2 b1 b0
   */
  dummy1 = 0x0007 & eleven_bit;

  /*
   * shift it 8 bits to left
   */
  dummy1 = ( dummy1 << 8 );

  /*
   * get b10 b9 b8 b7 b6 b5 b4 b3
   */
  dummy2 = 0xFFF8 & eleven_bit;

  /*
   * shift it 3 bits to right
   */
  dummy2 = ( dummy2 >> 3 );

  /*
   * compose dummy1 and dummy2 to the target 16-bit format
   */
  etsi11bit = dummy1 | dummy2;
  
  return etsi11bit;

} /* grr_convert_11bit_2_etsi() */ 

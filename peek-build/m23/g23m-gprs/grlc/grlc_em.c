/*+-----------------------------------------------------------------------------
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
|  Purpose :  This Module defines the engineering mode (EM) device driver for the
|             G23 protocol stack. This driver is used to control all engineering
|             mode related functions.
+-----------------------------------------------------------------------------
*/

#ifndef GRLC_EM_C
#define GRLC_EM_C

#define ENTITY_GRLC

/*--------- INCLUDES -----*/
/* Maybee some include is missing but windows and target build works*/
#include <string.h>   /*definition of memset*/
#include "typedefs.h"

#include "vsi.h"
#include "gprs.h"

#include "gsm.h"
#include "custom.h"   /*set FF_EM_MODE compile switch for SIMULATION*/
#include "prim.h"
#include "message.h"

#include "grlc.h"
#include "grlc_em.h" 

#ifdef FF_EM_MODE
/*
+------------------------------------------------------------------------------
| Function    : grlc_em_pco_trace_req
| 
| Parameter   : *grlc_em_pco_trace_req (EM_PCO_TRACE_REQ )
|
| Result      : void
|
+------------------------------------------------------------------------------
This function traces the EM data at the PCO.
*/
GLOBAL void grlc_em_pco_trace_req    ( T_EM_PCO_TRACE_REQ    * em_pco_trace_req)
{
  TRACE_FUNCTION( "grlc_em_pco_trace_req" );

  if(em_pco_trace_req->pco_bitmap & EM_PCO_GRLC_INFO)
  {

    TRACE_EVENT_EM_P4("EM_GRLC_INFO_REQ: grlc_sta:%d tbf_mode:%d dl_tfi:%d dl_mac_mode:%d",
      grlc_data->tm.state,
      grlc_data->tbf_type,
      grlc_data->dl_tfi,
      grlc_data->downlink_tbf.mac_mode);

    TRACE_EVENT_EM_P5("EM_GRLC_INFO_REQ: ul_tfi:%d ul_mac_mode:%d ul_nb_blocks:%d ul_cv:%d ul_cs:%d",
      grlc_data->ul_tfi,
      grlc_data->uplink_tbf.mac_mode,
      grlc_data->ru.rlc_octet_cnt,
      grlc_data->ru.cv,
      grlc_data->ru.cs_type);
  }

  PFREE (em_pco_trace_req);

}/*grlc_em_pco_trace_req*/

/*
+------------------------------------------------------------------------------
| Function    : grlc_em_fmm_rlc_trans_info_req
| 
| Parameter   : *em_fmm_rlc_trans_info_req (EM_FMM_RLC_TRANS_INFO_REQ)
|
| Result      : void
|
+------------------------------------------------------------------------------
This function provides data needed for microtec fmm1251. It gives the actual status about RLC.
*/
GLOBAL void grlc_em_fmm_rlc_trans_info_req (T_EM_FMM_RLC_TRANS_INFO_REQ * em_fmm_rlc_trans_info_req)
{
 PFREE(em_fmm_rlc_trans_info_req);

 TRACE_FUNCTION( "grlc_em_fmm_rlc_trans_info_req" );
 
 #ifdef FF_WAP
 {
  PALLOC(em_fmm_rlc_trans_info_cnf, EM_FMM_RLC_TRANS_INFO_CNF);
  
  memset (em_fmm_rlc_trans_info_cnf, 0, sizeof (T_EM_FMM_RLC_TRANS_INFO_CNF));

  em_fmm_rlc_trans_info_cnf->rlc_num_retrans    =   grlc_data->tbf_ctrl[grlc_data->ul_index].ret_bsn;
  em_fmm_rlc_trans_info_cnf->rlc_num_trans      =   (grlc_data->tbf_ctrl[grlc_data->ul_index].cnt_ts) - (grlc_data->tbf_ctrl[grlc_data->ul_index].ret_bsn);
  em_fmm_rlc_trans_info_cnf->rlc_pdu_cnt        =   grlc_data->tbf_ctrl[grlc_data->ul_index].pdu_cnt;
  em_fmm_rlc_trans_info_cnf->rlc_start_fn       =   grlc_data->tbf_ctrl[grlc_data->ul_index].start_fn;
  em_fmm_rlc_trans_info_cnf->rlc_end_fn         =   grlc_data->tbf_ctrl[grlc_data->ul_index].end_fn;
  em_fmm_rlc_trans_info_cnf->rlc_tbc            =   grlc_data->ru.tbc;
  
/* TRACE_EVENT_P5("t:%d r:%d p:%d s:%d e:%d", em_fmm_rlc_trans_info_cnf->rlc_num_trans,
                em_fmm_rlc_trans_info_cnf->rlc_num_retrans, em_fmm_rlc_trans_info_cnf->rlc_pdu_cnt,
                em_fmm_rlc_trans_info_cnf->rlc_start_fn, em_fmm_rlc_trans_info_cnf->rlc_end_fn);*/

  PSEND(hCommWAP, em_fmm_rlc_trans_info_cnf);
 }

#endif /*FF_WAP*/

}

/*
+------------------------------------------------------------------------------
| Function    : grlc_em_grlc_info_req
| 
| Parameter   : *em_grlc_info_req (T_EM_GLRC_INFO_REQ)
|
| Result      : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void grlc_em_grlc_info_req (T_EM_GRLC_INFO_REQ * em_grlc_info_req)
{
  PALLOC (em_grlc_info_cnf, EM_GRLC_INFO_CNF);

  TRACE_FUNCTION( "grlc_em_grlc_info_req" ); 

  PFREE  (em_grlc_info_req);

  memset (em_grlc_info_cnf, 0, sizeof (T_EM_GRLC_INFO_CNF));

  em_grlc_info_cnf->grlc_state    =   grlc_data->tm.state;
  em_grlc_info_cnf->tbf_mod       =   grlc_data->tbf_type;
  /* ul tbf*/
  em_grlc_info_cnf->ul_tbf_par.tfi        =   grlc_data->ul_tfi;
  em_grlc_info_cnf->ul_tbf_par.mac_mod    =   grlc_data->uplink_tbf.mac_mode;
  em_grlc_info_cnf->ul_tbf_par.ul_nb_block=   grlc_data->ru.rlc_octet_cnt;
  em_grlc_info_cnf->ul_tbf_par.cv         =   grlc_data->ru.cv;
  em_grlc_info_cnf->ul_tbf_par.cs         =   grlc_data->ru.cs_type;
  /* dl tbf*/
  em_grlc_info_cnf->dl_tbf_par.tfi        =   grlc_data->dl_tfi;
  em_grlc_info_cnf->dl_tbf_par.mac_mod    =   grlc_data->downlink_tbf.mac_mode;

  PSEND(hCommGMM, em_grlc_info_cnf);
} /*grlc_em_grlc_info_req*/

#endif /* FF_EM_MODE */
#endif /* GRLC_EM_C */



/*
+-----------------------------------------------------------------------------
|  Project :
|  Modul   :
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
|  Purpose :  This Module defines the engineering mode (EM) device driver for the
|             G23 protocol stack. This driver is used to control all engineering
|             mode related functions.
+-----------------------------------------------------------------------------
*/

#ifndef GRR_EM_C
#define GRR_EM_C

#define ENTITY_GRR

/*--------- INCLUDES -----*/
/* Maybee some include is missing,
but compiling has worked*/
#include <string.h>   /*definition of memset*/
#include "typedefs.h" /* to get Condat data types */

#include "vsi.h"      /* to get a lot of macros */
#include "gprs.h"     

#include "gsm.h"      /* to get a lot of macros */
#include "custom.h"   /*set FF_EM_MODE compile switch for SIMULATION*/
#include "prim.h"     /* to get the definitions of used SAP and directions */
#include "message.h"  

#include "grr.h"      /* to get the global entity definitions */
#include "grr_em.h"   /* to get the definition for EM */

#ifdef FF_EM_MODE

/*-----Variables------------------*/

/*
*  em_dsc_val is used for the engineering mode. To avoid unnecessary primitive sending
*  this variable is defined as global and is used between entities.
*/
GLOBAL UBYTE em_dsc_val = 0;

#endif /* FF_EM_MODE */
/*-----------Functions ----------------*/

#ifdef FF_EM_MODE

/*
+------------------------------------------------------------------------------
| Function    : grr_em_pco_trace_req
| 
| Parameter   : *grr_em_pco_trace_req (EM_PCO_TRACE_REQ )
|
| Result      : void
|
+------------------------------------------------------------------------------
This function traces the EM data at the PCO.
*/
GLOBAL void grr_em_pco_trace_req ( T_EM_PCO_TRACE_REQ      * em_pco_trace_req        )
{
  if(em_pco_trace_req->pco_bitmap & EM_PCO_GPRS_INFO)
  {
   TRACE_EVENT_EM_P5("EM_SC_GPRS_INFO_REQ: dl_nts:%d nmo:%d nw:%d pri_acc_thr:%d cba:%d",
     grr_data->downlink_tbf.nts,
     psc_db->cell_info_for_gmm.cell_info.net_mode,
     psc_db->net_ctrl.spgc_ccch_supp.nw_supp,
     psc_db->net_ctrl.priority_access_thr,
     psc_db->scell_par.cell_ba);
   TRACE_EVENT_EM_P6("EM_SC_GPRS_INFO_REQ: rac:%d tav:%d dsc:%d c31:%d c32:%d nco:%d",
     psc_db->cell_info_for_gmm.cell_info.cell_env.rai.rac,
     grr_data->ta_params.ta_value,
     em_dsc_val,
     grr_data->db.scell_info.cr_crit.c31,
     grr_data->db.scell_info.cr_crit.c32,
	 psc_db->nc_cw.param.ctrl_order);
  }

  PFREE (em_pco_trace_req);

}/*grr_em_pco_trace_req*/

  

/*
+------------------------------------------------------------------------------
| Function    : grr_em_sc_gprs_info_req
| 
| Parameter   : *em_sc_gprs_info_req (T_EM_SC_GPRS_INFO_REQ)
|
| Result      : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void grr_em_sc_gprs_info_req (T_EM_SC_GPRS_INFO_REQ *em_sc_gprs_info_req)
{
  PALLOC(em_sc_gprs_info_cnf, EM_SC_GPRS_INFO_CNF);

  PFREE(em_sc_gprs_info_req);

  TRACE_FUNCTION( "grr_em_sc_gprs_info_req" );

  memset (em_sc_gprs_info_cnf, 0, sizeof (T_EM_SC_GPRS_INFO_CNF));

  em_sc_gprs_info_cnf->tn  = grr_data->downlink_tbf.nts;
  em_sc_gprs_info_cnf->nmo = psc_db->cell_info_for_gmm.cell_info.net_mode;
  em_sc_gprs_info_cnf->net_ctrl.spgc_ccch_sup       = psc_db->net_ctrl.spgc_ccch_supp.nw_supp;
  em_sc_gprs_info_cnf->net_ctrl.priority_access_thr = psc_db->net_ctrl.priority_access_thr;
  em_sc_gprs_info_cnf->cba = psc_db->scell_par.cell_ba;
  em_sc_gprs_info_cnf->rac = psc_db->cell_info_for_gmm.cell_info.cell_env.rai.rac;
  em_sc_gprs_info_cnf->tav = grr_data->ta_params.ta_value;
  em_sc_gprs_info_cnf->dsc = em_dsc_val;
  em_sc_gprs_info_cnf->c31 = grr_data->db.scell_info.cr_crit.c31;
  em_sc_gprs_info_cnf->c32 = grr_data->db.scell_info.cr_crit.c32;
  em_sc_gprs_info_cnf->nco = psc_db->nc_cw.param.ctrl_order;

  PSEND (hCommGMM, em_sc_gprs_info_cnf);
}
/*
+------------------------------------------------------------------------------
| Function    : grr_em_fmm_sc_gprs_info_req
| 
| Parameter   : *em_fmm_sc_gprs_info_req (EM_FMM_SC_GPRS_INFO_REQ)
|
| Result      : void
|
+------------------------------------------------------------------------------
This function provides data needed for microtec fmm. this is the only requested
data, that is not available in RR.
*/
GLOBAL void grr_em_fmm_sc_gprs_info_req (T_EM_FMM_SC_GPRS_INFO_REQ * em_fmm_sc_gprs_info_req)
{
 PALLOC(em_fmm_sc_gprs_info_cnf, EM_FMM_SC_GPRS_INFO_CNF);
 PFREE(em_fmm_sc_gprs_info_req);

 TRACE_FUNCTION( "grr_em_fmm_sc_gprs_info_req" );
  
 memset (em_fmm_sc_gprs_info_cnf, 0, sizeof (T_EM_FMM_SC_GPRS_INFO_CNF));

 em_fmm_sc_gprs_info_cnf->c31_hyst    =   psc_db->gen_cell_par.c31_hyst;
 if (psc_db->gen_cell_par.v_ra_re_hyst)
    em_fmm_sc_gprs_info_cnf->ra_re_hyst  =   psc_db->gen_cell_par.gprs_c_hyst;
 em_fmm_sc_gprs_info_cnf->nco = psc_db->nc_cw.param.ctrl_order;

TRACE_EVENT_P3("c31_hyst:%d ra_re_hyst:%d nco:%d", em_fmm_sc_gprs_info_cnf->c31_hyst,\
              em_fmm_sc_gprs_info_cnf->ra_re_hyst, em_fmm_sc_gprs_info_cnf->nco);

#ifdef FF_WAP 
  PSEND( hCommWAP , em_fmm_sc_gprs_info_cnf);
#else  /*FF_WAP*/
  PFREE(em_fmm_sc_gprs_info_cnf);
#endif /*FF_WAP*/
}


/*
+------------------------------------------------------------------------------
| Function    : grr_em_fmm_tbf_info_req
| 
| Parameter   : *grr_em_fmm_tbf_info_req (EM_FMM_TBF_INFO_REQ)
|
| Result      : void
|
+------------------------------------------------------------------------------
This function provides data needed for microtec fmm. this is the only requested
data, that is not available in RR.
*/
GLOBAL void grr_em_fmm_tbf_info_req (T_EM_FMM_TBF_INFO_REQ *em_fmm_tbf_info_req)
{
 PFREE(em_fmm_tbf_info_req);
 TRACE_FUNCTION( "grr_em_fmm_tbf_info_req" );
#ifdef FF_WAP
 {
  PALLOC(em_fmm_tbf_info_cnf, EM_FMM_TBF_INFO_CNF);
 
  memset (em_fmm_tbf_info_cnf, 0, sizeof (T_EM_FMM_TBF_INFO_CNF));

  em_fmm_tbf_info_cnf->tbf_trans_type = grr_data->tbf_type;
  em_fmm_tbf_info_cnf->tbf_num_ults   = grr_data->uplink_tbf.nts;
  em_fmm_tbf_info_cnf->tbf_num_rlc    = grr_data->uplink_tbf.rlc_oct_cnt;
  
/*TRACE_EVENT_P3("t: %d ts: %d r: %d", em_fmm_tbf_info_cnf->tbf_trans_type,
               em_fmm_tbf_info_cnf->tbf_num_ults, em_fmm_tbf_info_cnf->tbf_num_rlc);*/

 PSEND(hCommWAP, em_fmm_tbf_info_cnf);
 }

#endif /*FF_WAP*/
}



#endif /* FF_EM_MODE */
#endif /*GRR_EM_C */

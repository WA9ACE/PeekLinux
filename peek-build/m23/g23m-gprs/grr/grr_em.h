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
|  Purpose : Engineering Mode (EM) Declarations + Macros
|
+-----------------------------------------------------------------------------
*/
#ifndef GRR_EM_H
#define GRR_EM_H

#ifdef FF_EM_MODE

/*-------------- other Macros --------------------------- */
/* same as defined in rr_em.h*/
#define TRACE_EVENT_EM_P5(s,a1,a2,a3,a4,a5)     TRACE_USER_CLASS_P5(TC_USER8,s,a1,a2,a3,a4,a5)
#define TRACE_EVENT_EM_P6(s,a1,a2,a3,a4,a5,a6)  TRACE_USER_CLASS_P6(TC_USER8,s,a1,a2,a3,a4,a5,a6)

/* ------------ data declarations for EM ---------------- */
/*
*  em_dsc_val is used for the engineering mode. To avoid unnecessary primitive sending
*  this variable is defined as global and is used between entities.
*/
EXTERN UBYTE em_dsc_val;

/*in em_pco_trace_on_and_callback_off is stored if the EM data output is redirected*/
EXTERN UBYTE em_pco_trace_on_and_callback_off; /*lint -esym(526,em_pco_trace_on_and_callback_off)
												: not defined here */

/*---------Functions ---------*/
EXTERN void grr_em_pco_trace_req        ( T_EM_PCO_TRACE_REQ      * em_pco_trace_req        );
EXTERN void grr_em_sc_gprs_info_req     ( T_EM_SC_GPRS_INFO_REQ   * em_sc_gprs_info_req     );

/*-----------FMM--------------*/
EXTERN void grr_em_fmm_sc_gprs_info_req (T_EM_FMM_SC_GPRS_INFO_REQ * em_fmm_sc_gprs_info_req);      /*FMM*/
EXTERN void grr_em_fmm_tbf_info_req     (T_EM_FMM_TBF_INFO_REQ     * em_fmm_tbf_info_req    );       /*FMM*/

/*--- other macros -------*/

#define GRR_EM_SET_DSC_VAL(x) (em_dsc_val = x)
#define GRR_EM_SET_CELL_BARRED (psc_db->scell_par.cell_ba = psi3->scell_par.cell_ba)

#else /*FF_EM_MODE not defined*/

#define GRR_EM_SET_DSC_VAL(x)
#define GRR_EM_SET_CELL_BARRED

#endif /*FF_EM_MODE*/
#endif /* GRR_EM_H */

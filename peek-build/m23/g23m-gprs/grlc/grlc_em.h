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
#ifndef GRLC_EM_H
#define GRLC_EM_H

#ifdef FF_EM_MODE

/* ------------ data declarations for EM ---------------- */

/*same as in rr_em.h*/
#define TRACE_EVENT_EM_P4(s,a1,a2,a3,a4)        TRACE_USER_CLASS_P4(TC_USER8,s,a1,a2,a3,a4)
#define TRACE_EVENT_EM_P5(s,a1,a2,a3,a4,a5)     TRACE_USER_CLASS_P5(TC_USER8,s,a1,a2,a3,a4,a5)

/*---------Functions ---------*/
EXTERN void grlc_em_pco_trace_req    ( T_EM_PCO_TRACE_REQ    * em_pco_trace_req);
EXTERN void grlc_em_grlc_info_req    ( T_EM_GRLC_INFO_REQ    * em_grlc_info_req); 

/*---------FMM----------------*/
EXTERN void grlc_em_fmm_rlc_trans_info_req (T_EM_FMM_RLC_TRANS_INFO_REQ * em_fmm_rlc_trans_info_req);/*FMM*/

/* -------FMM Macros -----*/



#else /*FF_EM_MODE not defined*/

#endif /*FF_EM_MODE*/
#endif /* GRLC_EM_H */

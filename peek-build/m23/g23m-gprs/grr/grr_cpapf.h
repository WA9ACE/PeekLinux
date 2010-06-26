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
|  Purpose :  Definitions for service CPAP.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRR_CPAPF_H
#define GRR_CPAPF_H


typedef enum  
{
  E_PDCH_ASSIGN_IGNORE,     /* no effect on current tbf */
  E_PDCH_ASSIGN_ERROR,
  E_PDCH_ASSIGN_UL,
  E_PDCH_ASSIGN_SB,
  E_PDCH_ASSIGN_DL,
  E_PDCH_ASSIGN_ASSIGN     /* valid message send mphp_assign_req */
} T_EVAL_PDCH_ASSIGN;


EXTERN void cpap_init ( void );

EXTERN T_EVAL_IA cpap_eval_ia ( void );

EXTERN T_EVAL_IA_DL cpap_eval_ia_dl ( void );

EXTERN T_EVAL_PDCH_ASSIGN cpap_eval_pdch_assignment ( void );

EXTERN void cpap_build_gprs_data_request (T_RRGRR_GPRS_DATA_REQ *rrgrr_gprs_data_req);

EXTERN void cpap_send_ass_fail (UBYTE cause);
EXTERN void cpap_send_assign_req_pdch (T_TBF_TYPE tbf_type_i);
EXTERN BOOL cpap_send_assign_req_ia ( T_TBF_TYPE tbf_type_i );
EXTERN UBYTE cpap_send_resource_request_ia ( void );
EXTERN UBYTE cpap_send_single_block_without_tbf ( void );
EXTERN void cpap_set_da_assignment_ia (T_MPHP_ASSIGNMENT_REQ *ptr2prim);
EXTERN UBYTE cpap_set_fa_assignment_ia (T_MPHP_ASSIGNMENT_REQ *ptr2prim);
EXTERN void cpap_set_dl_assignment_ia (T_MPHP_ASSIGNMENT_REQ *ptr2prim);
EXTERN UBYTE cpap_send_receive_normal_burst ( void );
EXTERN void cpap_join_tma_messages ( void );
EXTERN void cpap_store_ia_message ( void );
EXTERN void cpap_malloc_ia ( void );
EXTERN void cpap_mfree_ia ( BOOL restore_data );

#endif /* !GRR_CPAPF_H */


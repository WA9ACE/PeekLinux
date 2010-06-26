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

#ifndef GRR_CPAPS_H
#define GRR_CPAPS_H

EXTERN void sig_tc_cpap_abort_access ( void );
EXTERN void sig_tc_cpap_acc_req ( void );
EXTERN void sig_tc_cpap_ia_received (T_RRGRR_IA_IND *rrgrr_ia_ind);
EXTERN void sig_tc_cpap_iaext_received (T_RRGRR_IAEXT_IND *rrgrr_iaext_ind);
EXTERN void sig_tc_cpap_iarej_received(T_RRGRR_ASSIGNMENT_REJ_IND *rrgrr_assignment_rej_ind);
EXTERN void sig_tc_cpap_ia_downlink_received(T_RRGRR_IA_DOWNLINK_IND *rrgrr_ia_downlink_ind);
EXTERN void sig_tc_cpap_stop_task_cnf_received(void);
EXTERN void sig_tc_cpap_pdch_assign_received(T_RRGRR_DATA_IND *rrgrr_data_ind);
EXTERN void sig_tc_cpap_new_cell(void );
EXTERN void sig_tc_cpap_con_res_fail ( void );
EXTERN void sig_tc_cpap_suspend_dcch_cnf ( void );
EXTERN void sig_tc_cpap_t3142 ( void );
EXTERN void sig_tc_cpap_reconnect_ind ( void );

#endif /* !GRR_CPAPS_H */


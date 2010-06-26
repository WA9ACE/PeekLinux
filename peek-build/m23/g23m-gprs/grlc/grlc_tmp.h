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
|  Purpose :  Definitions for grlc_tmp.c.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRLC_TCP_H
#define GRLC_TCP_H




EXTERN void tm_grlc_data_req                ( T_GRLC_DATA_REQ               *grlc_data_req                );
EXTERN void tm_grlc_unitdata_req            ( T_GRLC_UNITDATA_REQ           *grlc_unitdata_req            );
EXTERN void tm_grlc_activate_gmm_queue_req  ( T_GRLC_ACTIVATE_GMM_QUEUE_REQ *grlc_activate_gmm_queue_req  );
EXTERN void tm_grlc_flush_data_req          ( T_GRLC_FLUSH_DATA_REQ     *grlc_flush_data_req     );
EXTERN void tm_t3168 ( void );
EXTERN void tm_t3188 ( void );


EXTERN void tm_cgrlc_enable_req         ( T_CGRLC_ENABLE_REQ        *cgrlc_enable_req        );
EXTERN void tm_cgrlc_disable_req        ( T_CGRLC_DISABLE_REQ       *cgrlc_disable_req       );


EXTERN void tm_cgrlc_ul_tbf_res         ( T_CGRLC_UL_TBF_RES        *cgrlc_ul_tbf_res        );
EXTERN void tm_cgrlc_dl_tbf_req         ( T_CGRLC_DL_TBF_REQ        *cgrlc_dl_tbf_req        );
EXTERN void tm_cgrlc_tbf_rel_req        ( T_CGRLC_TBF_REL_REQ       *cgrlc_tbf_rel_req       );
EXTERN void tm_cgrlc_tbf_rel_res        ( T_CGRLC_TBF_REL_RES       *cgrlc_tbf_rel_res       );
EXTERN void tm_cgrlc_data_req           ( T_CGRLC_DATA_REQ          *cgrlc_data_req          );
EXTERN void tm_cgrlc_poll_req           ( T_CGRLC_POLL_REQ          *cgrlc_poll_req          );
EXTERN void tm_cgrlc_access_status_req  ( T_CGRLC_ACCESS_STATUS_REQ *cgrlc_access_status_req );
EXTERN void tm_cgrlc_test_mode_req      ( T_CGRLC_TEST_MODE_REQ     *cgrlc_test_mode_req     );
EXTERN void tm_cgrlc_test_end_req       ( T_CGRLC_TEST_END_REQ      *cgrlc_test_end_req      );
EXTERN void tm_cgrlc_test_end_req       ( T_CGRLC_TEST_END_REQ      *cgrlc_test_end_req      );
EXTERN void tm_cgrlc_ta_value_req       ( T_CGRLC_TA_VALUE_REQ      *cgrlc_ta_value_req      );

EXTERN void tm_cgrlc_ready_timer_config_req ( T_CGRLC_READY_TIMER_CONFIG_REQ *cgrlc_ready_timer_config_req );
EXTERN void tm_cgrlc_force_to_standby_req   ( T_CGRLC_FORCE_TO_STANDBY_REQ   *cgrlc_force_to_standby_req   );
EXTERN void tm_cgrlc_pwr_ctrl_req       ( T_CGRLC_PWR_CTRL_REQ      *pwr_ctrl_req            );

#endif /* !GRLC_TCP_H */


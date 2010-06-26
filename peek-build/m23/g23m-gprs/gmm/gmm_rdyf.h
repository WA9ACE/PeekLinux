/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  gmm_rdyf.h
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
|  Purpose :  Definitions for gmm_rdyf.c 
+----------------------------------------------------------------------------- 
*/ 


#ifndef GMM_RDYF_H
#define GMM_RDYF_H




EXTERN void rdy_init ( void );
EXTERN ULONG rdy_get_timer ( T_rau_timer * timer );
EXTERN void rdy_start_t3312 ( void ); 
EXTERN void rdy_cgrlc_ready_timer_config_req (ULONG timer); 


#endif /* !GMM_RDYF_H */


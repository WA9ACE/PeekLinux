/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  gmm_rdys.h
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
|  Purpose :  Definitions for gmm_rdys.c 
+----------------------------------------------------------------------------- 
*/ 


#ifndef GMM_RDYS_H
#define GMM_RDYS_H




EXTERN void sig_kern_rdy_force_ie_req ( UBYTE force_to_standby,
  BOOL attach_omplete );


EXTERN void sig_kern_rdy_t3314_req ( BOOL v_ptmsi, BOOL v_ready_timer, 
                                    T_ready_timer * ready_timer,
                                    T_rau_timer * rau_timer,
                                    BOOL attach_complete);

EXTERN void sig_kern_rdy_start_t3312_req ( void );
EXTERN void sig_kern_rdy_cu_req ( void );
EXTERN void sig_kern_rdy_start_t3302_req ( void );
EXTERN void sig_kern_rdy_stop_t3302_req ( void );
EXTERN void sig_kern_rdy_start_timer_req ( UBYTE timer, ULONG value );




#endif /* !GMM_RDYS_H */


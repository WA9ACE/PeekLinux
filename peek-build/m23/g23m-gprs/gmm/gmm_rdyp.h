/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  gmm_rdyp.h
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
|  Purpose :  Definitions for gmm_rdyp.c 
+----------------------------------------------------------------------------- 
*/ 


#ifndef GMM_RDYP_H
#define GMM_RDYP_H


EXTERN void rdy_t3316 (void);



EXTERN void rdy_cgrlc_trigger_ind ( T_CGRLC_TRIGGER_IND *cgrlc_trigger_ind ); /* TCS 2.1 */
EXTERN void rdy_cgrlc_standby_state_ind ( T_CGRLC_STANDBY_STATE_IND *cgrlc_standby_state_ind ); /* TCS 2.1 */
EXTERN void rdy_cgrlc_ready_state_ind ( T_CGRLC_READY_STATE_IND *cgrlc_ready_state_ind ); /* TCS 2.1 */


#endif /* !GMM_RDYP_H */


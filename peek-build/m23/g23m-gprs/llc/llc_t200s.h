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
|  Purpose :  Definitions for llc_t200s.c 
+----------------------------------------------------------------------------- 
*/ 


#ifndef LLC_T200S_H
#define LLC_T200S_H




EXTERN void sig_llme_t200_assign_req (void);

EXTERN void sig_llme_t200_unassign_req (void);

EXTERN void sig_u_t200_start_req 
(
#ifdef LL_DESC
 T_LL_UNITDESC_REQ *frame, 
#else                                
 T_LL_UNITDATA_REQ *frame,
#endif
 UBYTE cause
);

EXTERN void sig_u_t200_stop_req (void);

EXTERN void sig_u_t200_expire_req (void);



#endif /* !LLC_T200S_H */


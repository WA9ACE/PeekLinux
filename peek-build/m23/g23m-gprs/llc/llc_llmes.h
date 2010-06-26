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
|  Purpose :  Definitions for llc_llmes.c 
+----------------------------------------------------------------------------- 
*/ 


#ifndef LLC_LLMES_H
#define LLC_LLMES_H


EXTERN void sig_u_llme_reset_ind (void);
EXTERN void sig_u_llme_ready_ind (void);

#ifdef CC_CONCEPT
EXTERN void sig_itx_llme_reest_ind (USHORT error_cause);
#else
EXTERN void sig_itx_llme_reest_ind (UBYTE error_cause);
#endif

EXTERN void sig_u_llme_abmest_ind (void);

EXTERN void sig_u_llme_abmrel_ind (void);


#endif /* !LLC_LLMES_H */

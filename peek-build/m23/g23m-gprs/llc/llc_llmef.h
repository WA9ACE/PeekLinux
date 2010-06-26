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
|  Purpose :  Definitions for llc_llmef.c 
+----------------------------------------------------------------------------- 
*/ 

#ifndef LLC_LLMEF_H
#define LLC_LLMEF_H




EXTERN void llme_init (void);

#ifdef LL_2to1
EXTERN void llme_init_cipher (T_LLGMM_llgmm_kc kc, UBYTE ciphering_algorithm);
#else
EXTERN void llme_init_cipher (T_llgmm_kc kc, UBYTE ciphering_algorithm);
#endif



#endif /* !LLC_LLMES_H */


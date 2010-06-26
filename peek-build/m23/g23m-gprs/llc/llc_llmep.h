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
|  Purpose :  Definitions for llc_llmep.c 
+----------------------------------------------------------------------------- 
*/ 


#ifndef LLC_LLMEP_H
#define LLC_LLMEP_H




EXTERN void llme_llgmm_assign_req ( T_LLGMM_ASSIGN_REQ *llgmm_assign_req );

EXTERN void llme_llgmm_trigger_req ( T_LLGMM_TRIGGER_REQ *llgmm_trigger_req );

EXTERN void llme_llgmm_suspend_req ( T_LLGMM_SUSPEND_REQ *llgmm_suspend_req );

EXTERN void llme_llgmm_resume_req ( T_LLGMM_RESUME_REQ *llgmm_resume_req );


#endif /* !LLC_LLMES_H */


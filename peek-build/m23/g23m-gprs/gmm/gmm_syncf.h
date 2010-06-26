/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  gmm_syncf.h
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
|  Purpose :  Definitions for gmm_syncf.c 
+----------------------------------------------------------------------------- 
*/ 


#ifndef GMM_SYNCF_H
#define GMM_SYNCF_H


EXTERN BOOL sync_is_cid_equal (void);
EXTERN void sync_send_cell_info(void );
EXTERN void sync_gmm_init ( void );
EXTERN void sync_send_mm_cell_info(void);




#endif /* !GMM_RXS_H */


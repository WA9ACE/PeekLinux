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


#ifndef GMM_SYNCP_H
#define GMM_SYNCP_H

EXTERN void sync_tsync              ( void );
EXTERN void sync_gmmrr_cell_ind     ( T_GMMRR_CELL_IND     *gmmrr_cell_ind     );
EXTERN void sync_mmgmm_activate_ind ( T_MMGMM_ACTIVATE_IND *mmgmm_activate_ind );
EXTERN void sync_mmgmm_nreg_ind     ( T_MMGMM_NREG_IND     *mmgmm_nreg_ind     );
EXTERN void sync_mmgmm_reg_rej      ( T_MMGMM_REG_REJ      *mmgmm_reg_rej      );
EXTERN void sync_mmgmm_reg_cnf      ( T_MMGMM_REG_CNF      *mmgmm_reg_cnf      );

#endif /* !GMM_SYNCP_H */


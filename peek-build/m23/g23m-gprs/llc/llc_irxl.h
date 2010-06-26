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
|  Purpose :  Definitions for llc_irxl.c 
+----------------------------------------------------------------------------- 
*/ 


#ifndef LLC_IRXL_H
#define LLC_IRXL_H


EXTERN void irx_label_if        (T_LL_UNITDATA_IND *ll_unitdata_ind, 
                                 T_FRAME_NUM       ns, 
                                 T_BIT             a_bit, 
                                 UBYTE             state);


#endif /* !LLC_IRXL_H */

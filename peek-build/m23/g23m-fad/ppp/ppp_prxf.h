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
|  Purpose :  Definitions for ppp_prxf.c 
+----------------------------------------------------------------------------- 
*/ 

#ifndef PPP_PRXF_H
#define PPP_PRXF_H

EXTERN void prx_init (void);

#ifdef FF_STATUS_TE
EXTERN void cb_status_TE (T_DRV_SIGNAL * signal);
#endif /* FF_STATUS_TE */

#endif /* !PPP_PRXF_H */

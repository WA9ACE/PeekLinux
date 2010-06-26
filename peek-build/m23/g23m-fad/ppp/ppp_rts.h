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
|  Purpose :  Definitions for ppp_rts.c 
+----------------------------------------------------------------------------- 
*/ 

#ifndef PPP_RTS_H
#define PPP_RTS_H


EXTERN void sig_arb_rt_parameters_req (  UBYTE rt_time );

EXTERN void sig_any_rt_rrt_req (void);

EXTERN void sig_any_rt_srt_req (void);

#endif /* !PPP_RTS_H */

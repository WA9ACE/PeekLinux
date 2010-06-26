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
|  Purpose :  Definitions for ppp_frxf.c 
+----------------------------------------------------------------------------- 
*/ 

#ifndef PPP_FRXF_H
#define PPP_FRXF_H


EXTERN void frx_init (void);

EXTERN void frx_add_desc (void);

EXTERN void frx_detect_frame (void);

EXTERN void frx_send_pack_ready_mode (void);

EXTERN void frx_send_pack_transp_mode (void);

#endif /* !PPP_FRXF_H */

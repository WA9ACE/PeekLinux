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
|  Purpose :  Definitions for ppp_ftxf.c 
+----------------------------------------------------------------------------- 
*/ 

#ifndef PPP_FTXF_H
#define PPP_FTXF_H


EXTERN void ftx_init (void);

EXTERN void ftx_get_frame (USHORT ptype, 
                           T_desc2* packet,
                           T_desc_list2* ptr_frame);

EXTERN void ftx_check_frame (T_desc_list2* ptr_frame);

EXTERN void ftx_get_next_frame (T_desc_list2* ptr_frame);

#endif /* !PPP_FTXF_H */

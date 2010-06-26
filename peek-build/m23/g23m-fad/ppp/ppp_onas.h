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
|  Purpose :  Definitions for ppp_onas.c 
+----------------------------------------------------------------------------- 
*/ 

#ifndef PPP_ONAS_H
#define PPP_ONAS_H


EXTERN void sig_any_ona_ruc_ind (USHORT ptype, T_desc2* packet);

EXTERN void sig_any_ona_open_ind (USHORT ptype, T_desc2* packet);

EXTERN void sig_any_ona_down_ind (USHORT ptype);

EXTERN void sig_any_ona_close_ind (USHORT ptype, T_desc2* packet);

EXTERN void sig_any_ona_rtr_ind (USHORT ptype, T_desc2* packet);

EXTERN void sig_any_ona_to_ind (USHORT ptype);

EXTERN void sig_any_ona_rta_ind (USHORT ptype);

EXTERN void sig_any_ona_rxjn_ind (USHORT ptype, T_desc2* packet);

EXTERN void sig_any_ona_rcrn_ind (USHORT ptype, T_desc2* packet, UBYTE newFlag);

EXTERN void sig_any_ona_rcrp_ind (USHORT ptype, T_desc2* packet);

EXTERN void sig_any_ona_rcn_ind (USHORT ptype, T_desc2* packet);

EXTERN void sig_any_ona_rca_ind (USHORT ptype);

EXTERN void sig_any_ona_rxr_ind (USHORT ptype, T_desc2* packet);

#endif /* !PPP_ONAS_H */

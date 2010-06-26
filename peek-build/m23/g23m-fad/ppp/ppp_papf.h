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
|  Purpose :  Definitions for ppp_papf.c 
+----------------------------------------------------------------------------- 
*/ 

#ifndef PPP_PAPF_H
#define PPP_PAPF_H


EXTERN void pap_init (void);

EXTERN void pap_fill_out_packet (UBYTE pco_buf[], USHORT* ptr_pos);

EXTERN void pap_get_sar (T_desc2** ptr_packet);

EXTERN void pap_get_saa (T_desc2** ptr_packet);

EXTERN void pap_rar (T_desc2** ptr_packet, UBYTE* forward);

EXTERN void pap_raa (T_desc2* packet, UBYTE* forward);

EXTERN void pap_ran (T_desc2* packet, UBYTE* forward);

#endif /* !PPP_PAPF_H */

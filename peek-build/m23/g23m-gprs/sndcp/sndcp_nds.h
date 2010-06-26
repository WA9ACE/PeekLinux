/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  sndcp_nds.h
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
|  Purpose :  Definitions for sndcp_nds.c 
+----------------------------------------------------------------------------- 
*/ 

#ifndef SNDCP_NDS_H
#define SNDCP_NDS_H

EXTERN void sig_mg_nd_recover (UBYTE nsapi);

EXTERN void sig_mg_nd_reset_ind (UBYTE nsapi);

#ifdef SNDCP_UPM_INCLUDED
EXTERN U8 sig_mg_nd_get_rec_no(UBYTE nsapi);
#else
EXTERN void sig_mg_nd_get_rec_no(UBYTE nsapi);
#endif

EXTERN void sig_mg_nd_resume (UBYTE nsapi);

EXTERN void sig_mg_nd_suspend (UBYTE nsapi);

EXTERN void sig_sd_nd_unitdata_ind (UBYTE nsapi,
                                    T_SN_UNITDATA_IND* sn_unitdata_ind);

EXTERN void sig_sda_nd_data_ind (UBYTE nsapi,
                                 T_SN_DATA_IND* sn_data_ind,
                                 UBYTE npdu_number);

EXTERN void sndcp_reset_nd_nsapi_npdu_num (UBYTE nsapi); 

#endif /* !SNDCP_NDS_H */


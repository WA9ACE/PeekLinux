/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  sndcp_pdas.h
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
|  Purpose :  Definitions for sndcp_pdas.c 
+----------------------------------------------------------------------------- 
*/ 

/*---- HISTORY --------------------------------------------------------------*/
#if 0

  $Log:: /GPRS/Condat/MS/SRC/SNDCP/sndcp_pdas.h                            $
 * 
 * 8     15.05.00 16:55 Hk
 * T H E   E N D.
 * 
 * 7     15.05.00 16:16 Hk
 * backup.
 * 
 * 6     9.05.00 18:59 Hk
 * XID negotiation work in progress. Backup.
 * 
 * 5     18.04.00 18:47 Hk
 * Backup. Work on XID negotiation.
 * 
 * 4     30.03.00 16:04 Hk
 * 
 * 3     17.03.00 18:03 Hk
 * Acknowledged data transfer is now implemented and tested uplink and
 * downlink.
 * 
 * 2     10.03.00 17:49 Hk
 * Backup. First downlink ack data transfer ok (case 700).
 * 
 * 1     10.03.00 15:01 Hk
 * Initial.

#endif

#ifndef SNDCP_PDAS_H
#define SNDCP_PDAS_H




EXTERN void sig_sda_pda_getdata_req ( UBYTE nsapi );



#endif /* !SNDCP_PDAS_H */


/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  GRLC
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
|  Purpose :  Definitions for grlc_func.c. Only in Target used. 
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRLC_FUNC_H
#define GRLC_FUNC_H

/* 
 * Mapping of TI-types to Condat-types
 */
#define UWORD8    UBYTE
#define WORD8     UBYTE
#define WORD16    SHORT
#define UWORD16   USHORT
#define UWORD32   ULONG
#define API       USHORT 

EXTERN void maca_power_control ( UWORD8  assignment_id,
                                 BOOL    crc_error,
                                 WORD8   bcch_level,
                                 UWORD16 radio_freq[MAC_BURST_PER_BLOCK],
                                 WORD8   burst_level[MAC_BURST_PER_BLOCK],
                                 UWORD8 *pch );

#ifdef _TARGET_

EXTERN void rlc_uplink ( UWORD8   assignment_id,
                         UWORD8   tx_data_no,
                         UWORD32  fn,
                         UWORD8   timing_advance_value,
                         T_ul_poll_resp    * ul_poll_response,
                         T_ul_data         * ul_data, 
                         BOOL      allocation_exhausted);


EXTERN void rlc_downlink ( UWORD8         assignment_id,
                           UWORD32        fn,
                           T_dl_struct   *data_ptr,
                           UWORD8         rlc_blocks_sent,
                           UWORD8         last_poll_response);
#endif /* _TARGET_*/


#endif /* !GRLC_FUNC_H */


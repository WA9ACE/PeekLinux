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
|  Purpose :  Definitions for service MEAS.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRLC_MEASF_H
#define GRLC_MEASF_H

EXTERN UBYTE meas_c_calc_mean      ( UBYTE              *burst_level,
                                     USHORT             *radio_freq,
                                     ULONG              *ss_block,
                                     UBYTE              *pb_rdc,
                                     T_CGRLC_freq_param *freq_param     );

EXTERN void  meas_int_fill_rel_iLevel
                                   ( UBYTE              *v_ilev,
                                     UBYTE              *ilev,
                                     UBYTE               idx,
                                     USHORT              c_raw_data_lev );
#endif /* #ifndef GRLC_MEASF_H */


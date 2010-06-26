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

#ifndef GRLC_MEASS_H
#define GRLC_MEASS_H

/* TM Signals */

EXTERN void  sig_tm_meas_ptm       ( void                                  );

/* RD Signals */

EXTERN void  sig_rd_meas_qual_rpt_sent 
                                   ( void                                  );

/* Access Functions */

EXTERN void  meas_grlc_init        ( void                                  );

EXTERN UBYTE meas_grlc_c_get_value ( void                                  );

EXTERN void  meas_grlc_c_get_c_value 
                                   ( T_CGRLC_c_value       *c_value        );

EXTERN void  meas_grlc_c_set_c_value
                                   ( T_CGRLC_c_value       *c_value        );

EXTERN void  meas_c_val_update_ptm ( T_MAC_PWR_CTRL_IND    *mac_pwr_ctrl_ind,
                                     T_CGRLC_glbl_pwr_ctrl_param *pwr_ctrl_param,
                                     T_CGRLC_freq_param    *freq_param     );

EXTERN void  meas_c_restart        ( void                                  );

EXTERN UBYTE meas_sv_get_value     ( void                                  ); 

EXTERN void  meas_sv_update        ( T_MAC_PWR_CTRL_IND    *mac_pwr_ctrl_ind,
                                     T_CGRLC_glbl_pwr_ctrl_param *pwr_ctrl_param,
                                     T_CGRLC_freq_param    *freq_param     );

EXTERN void  meas_sv_restart       ( void                                  );

EXTERN UBYTE meas_sq_get_rxqual_value 
                                   ( void                                  );

EXTERN void  meas_sq_update        ( T_BER_IND            *ber_ind         );

EXTERN void  meas_sq_restart       ( void                                  );

EXTERN void  meas_int_get_rel_i_level
                                   ( T_ilev               *i_level         );

#endif /* #ifndef GRLC_MEASS_H */


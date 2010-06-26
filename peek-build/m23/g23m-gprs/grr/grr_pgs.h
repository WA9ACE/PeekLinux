/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  GRR
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
|  Purpose :  Definitions for service PG.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRR_PGS_H
#define GRR_PGS_H




EXTERN void sig_ctrl_pg_start ( void );



EXTERN void sig_ctrl_pg_stop ( void );



EXTERN void sig_ctrl_pg_mode_ind ( T_PACKET_MODE packet_mode );



EXTERN void sig_ctrl_pg_t3172_run ( void );


EXTERN void sig_ctrl_pg_indication ( UBYTE paging_type ) ;  /* paging for packet access */


EXTERN void sig_ctrl_pg_rr_est ( void );                    /* paging for RR connection */


/*GFP*/
EXTERN void sig_gfp_pg_req ( void ) ;

EXTERN void sig_gfp_pg_mode ( UBYTE page_mode, BOOL ms_paging_group ) ;

EXTERN void sig_gfp_pg_stop_pccch_con ( T_MPHP_STOP_PCCCH_CON *mphp_stop_pccch_con );

EXTERN void sig_cs_pg_start_t_nc2_ndrx ( UBYTE nc_non_drx_period );

#endif /* !GRR_PGS_H */


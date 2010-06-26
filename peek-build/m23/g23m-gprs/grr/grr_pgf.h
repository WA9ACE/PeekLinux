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

#ifndef GRR_PGF_H
#define GRR_PGF_H

typedef enum
{
  NULL_IDLE,
  IDLE_ACCESS,
  IDLE_PTM,
  ACCESS_IDLE,
  ACCESS_PTM,
  PTM_ACCESS,
  PTM_IDLE,
  T3172_PTM
}T_PG_TRANS;

EXTERN void pg_handle_classB_NMO_III ( T_PG_TRANS transition );

EXTERN void pg_handle_idle_mode ( T_PG_TRANS transition );

EXTERN void pg_handle_access_mode ( void );

EXTERN void pg_handle_transfer_mode ( T_PG_TRANS transition );

EXTERN void pg_init ( void );

EXTERN void pg_init_params ( void );

EXTERN void pg_process_pp_req ( T_D_PAGING_REQ *paging_req, UBYTE state );

EXTERN void pg_send_stop ( void );

EXTERN void pg_send_start ( UBYTE pg_mode );

EXTERN void pg_start_t_nc2_ndrx ( UBYTE nc_non_drx_period );

EXTERN void pg_non_drx ( void );

EXTERN void pg_handle_non_drx_timers(void);

EXTERN void pg_set_l1_ref_tn ( UBYTE* out_tn, UBYTE in_tn );

#endif /* !GRR_PGF_H */


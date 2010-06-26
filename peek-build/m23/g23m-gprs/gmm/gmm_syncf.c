/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  gmm_syncf.c
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
|  Purpose :  This modul is part of the entity GMM and implements all
|             procedures and functions as described in the
|             SDL-documentation (SYNC-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#ifndef GMM_SYNCF_C
#define GMM_SYNCF_C
#endif

#define ENTITY_GMM

/*==== INCLUDES =============================================================*/

#include "typedefs.h"    /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "ccdapi.h"     /* to get CCD API */
#include "cnf_gmm.h"    /* to get cnf-definitions */
#include "mon_gmm.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "gmm.h"        /* to get the global entity definitions */
#include "gmm_kerns.h"    /* to get some signals */

#include <string.h>     /* to get memset */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : rxgmm_init
+------------------------------------------------------------------------------
| Description : The function rxgmm_init() ....
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sync_gmm_init ( void )
{
  TRACE_FUNCTION( "sync_gmm_init" );

  /*
   * Initialise service SYNC with state SYNC_IDLE.
   */
  gmm_data->sync.grr_cell_info.net_mode = GMMRR_NET_MODE_II;
  gmm_data->sync.mm_cell_info.status= MMGMM_LIMITED_SERVICE;
  gmm_data->sync.sig_cell_info.env.rai.rac = GMMRR_RAC_INVALID;
  gmm_data->sync.mm_cell_info.cid = 0xff;
  gmm_data->sync.grr_cell_info.cell_env.cid= 0xff;
  gmm_data->sync.mm_cell_info.lac = GMMRR_LA_INVALID;
  gmm_data->sync.grr_cell_info.cell_env.rai.lac = GMMRR_LA_INVALID;

#ifdef REL99
  /* sgsnr_flag should be initialized to a value 0 (CL_SGSN_REL_98_OR_OLDER).
   * SM also will initialize the value of sgsnr_flag in its
   * context to 0. GMM will update SM when the value changes.
   */
  gmm_data->kern.sig_cell_info.sgsnr_flag =  PS_SGSN_98_OLDER; /* TCS 4.0 */
#endif    

  INIT_STATE ( SYNC, SYNC_IDLE );
  vsi_t_stop  ( GMM_handle, sync_TSYNC);
} /* sync_gmm_init() */

/*
+------------------------------------------------------------------------------
| Function    : sync_is_cid_equal
+------------------------------------------------------------------------------
| Description : The function sync_is_cid_equal() ....
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL sync_is_cid_equal ()
{
  GMM_TRACE_FUNCTION( "sync_is_cid_equal" );

  GMM_RETURN_ (
      gmm_data->sync.mm_cell_info.cid == gmm_data->sync.grr_cell_info.cell_env.cid
    && gmm_data->sync.mm_cell_info.lac == gmm_data->sync.grr_cell_info.cell_env.rai.lac
    && memcmp (gmm_data->sync.mm_cell_info.plmn.mnc, gmm_data->sync.grr_cell_info.cell_env.rai.plmn.mnc, SIZE_MNC)==0
    && memcmp (gmm_data->sync.mm_cell_info.plmn.mcc, gmm_data->sync.grr_cell_info.cell_env.rai.plmn.mcc, SIZE_MCC)==0
    );
} /* sync_sync_is_cid_equal() */

/*
+------------------------------------------------------------------------------
| Function    : sync_send_cell_info
+------------------------------------------------------------------------------
| Description : The function sync_send_cell_info() ....
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sync_send_cell_info()
{
  GMM_TRACE_FUNCTION( "sync_send_cell_info" );

  gmm_data->sync.sig_cell_info.env = gmm_data->sync.grr_cell_info.cell_env;
  gmm_data->sync.sig_cell_info.net_mode = gmm_data->sync.grr_cell_info.net_mode;
#ifdef GMM_TCS4
  gmm_data->sync.sig_cell_info.rt = gmm_data->sync.grr_cell_info.rt;
#endif
  /* MM */
  gmm_data->sync.sig_cell_info.mm_status  = gmm_data->sync.mm_cell_info.status;

  gmm_data->sync.sig_cell_info.t3212_val     = gmm_data->sync.mm_cell_info.t3212_val;



  /* GRR */
  if ( MMGMM_GPRS_SUPP_NO == gmm_data->sync.mm_cell_info.gprs_indicator)
  {
    gmm_data->sync.sig_cell_info.gmm_status = GMMRR_SERVICE_NONE;
    gmm_data->sync.sig_cell_info.env.cid  = gmm_data->sync.mm_cell_info.cid;
    gmm_data->sync.sig_cell_info.env.rai.lac = gmm_data->sync.mm_cell_info.lac;
    gmm_data->sync.sig_cell_info.env.rai.plmn = gmm_data->sync.mm_cell_info.plmn;
    gmm_data->sync.sig_cell_info.net_mode = GMMRR_NET_MODE_II;
  }
  else
  {
    gmm_data->sync.sig_cell_info.gmm_status = gmm_data->sync.grr_cell_info.service_state;
    if (GMMRR_SERVICE_NONE==gmm_data->sync.sig_cell_info.gmm_status)
    {
       gmm_data->sync.sig_cell_info.net_mode = GMMRR_NET_MODE_II;
    }

  }

  sig_sync_kern_cell_ind ( &gmm_data->sync.sig_cell_info );
  gmm_data->sync.sig_cell_info.gmmrr_cell_ind_received = FALSE;
  GMM_RETURN;
} /* sync_send_cell_info() */
/*
+------------------------------------------------------------------------------
| Function    : sync_send_mm_cell_info
+------------------------------------------------------------------------------
| Description : The function sync_send_cell_info() ....
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sync_send_mm_cell_info()
{
  GMM_TRACE_FUNCTION( "sync_send_mm_cell_info" );

  gmm_data->sync.sig_cell_info.mm_status  = gmm_data->sync.mm_cell_info.status;

  gmm_data->sync.sig_cell_info.t3212_val     = gmm_data->sync.mm_cell_info.t3212_val;



  /* GRR */
  if ( MMGMM_GPRS_SUPP_NO == gmm_data->sync.mm_cell_info.gprs_indicator)
  {
    gmm_data->sync.sig_cell_info.gmm_status = GMMRR_SERVICE_NONE;
    gmm_data->sync.sig_cell_info.net_mode = GMMRR_NET_MODE_II;
  }

  gmm_data->sync.sig_cell_info.env.cid  = gmm_data->sync.mm_cell_info.cid;
  gmm_data->sync.sig_cell_info.env.rai.lac = gmm_data->sync.mm_cell_info.lac;
  gmm_data->sync.sig_cell_info.env.rai.plmn = gmm_data->sync.mm_cell_info.plmn;

  sig_sync_kern_cell_ind ( &gmm_data->sync.sig_cell_info );

  GMM_RETURN;
} /* sync_send_cell_info() */


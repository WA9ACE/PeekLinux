/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  gmm_syncp.c
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
|  Purpose :  
+----------------------------------------------------------------------------- 
*/ 

#ifndef GMM_SYNCP_C
#define GMM_SYNCP_C
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
#include "gmm_f.h"      /* to get the gdebug print function */

#include "gmm_kerns.h"  /* to get signals */
#include "gmm_syncf.h"


/*
+------------------------------------------------------------------------------
| Function    : sync_tsync
+------------------------------------------------------------------------------
| Description : Handles expiry of timer TSYNC. This function is called from
|               pei_primitive().
|               Timeout for waiting of GMMRR_CELL_IND or MMGMM_ACTIVATE_IND
|                                                     or MMGMM_NREG_IND
| Parameters  : none
|
+------------------------------------------------------------------------------
*/
GLOBAL void sync_tsync (void)
{
  GMM_TRACE_FUNCTION( "sync_tsync" );
  TRACE_0_INFO("TIMEOUT TSYNC expired");

  switch( GET_STATE( SYNC ) )
  {
    case SYNC_WAIT_FOR_GPRS:
      SET_STATE ( SYNC, SYNC_IDLE );
      sync_send_mm_cell_info();
      break;
    case SYNC_WAIT_FOR_GSM:
      SET_STATE ( SYNC, SYNC_IDLE );
      sync_send_cell_info();
      break;
    case SYNC_IDLE:   
    default:
      TRACE_ERROR("TSYNC expired in wrong state");
      sync_send_cell_info();
      break;
  }
  GMM_RETURN;
} /* sync_tsync */

/*
+------------------------------------------------------------------------------
| Function    : sync_gmmrr_cell_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive GMMRR_CELL_IND
|
| Parameters  : *gmmrr_cell_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void sync_gmmrr_cell_ind (T_GMMRR_CELL_IND * gmmrr_cell_ind)
{

#ifdef REL99
  /*The flag value is initialized to 0(CL_SGSN_REL_98_OR_OLDER). */
  U8 temp_sgsnr_flag = PS_SGSN_98_OLDER; /* TCS 4.0 */
#endif

  GMM_TRACE_FUNCTION( "sync_gmmrr_cell_ind" );
#ifdef GMM_TCS4
  TRACE_12_PARA("%s, %s,%x%x%x, %x%x%x, NMO: %d, lac %x, rac %x, cid %x", /* TCS 4.0 */
        (RT_GPRS==gmmrr_cell_ind->cell_info.rt?"GPRS":"EDGE"), /* TCS 4.0 */
        GMMRR_SERVICE_FULL == gmmrr_cell_ind->cell_info.service_state?"service":(GMMRR_SERVICE_LIMITED== gmmrr_cell_ind->cell_info.service_state?"limited service":"NO GPRS service"),
        gmmrr_cell_ind->cell_info.cell_env.rai.plmn.mcc[0],
        gmmrr_cell_ind->cell_info.cell_env.rai.plmn.mcc[1],
        gmmrr_cell_ind->cell_info.cell_env.rai.plmn.mcc[2],
        gmmrr_cell_ind->cell_info.cell_env.rai.plmn.mnc[0],
        gmmrr_cell_ind->cell_info.cell_env.rai.plmn.mnc[1],
        gmmrr_cell_ind->cell_info.cell_env.rai.plmn.mnc[2],
        gmmrr_cell_ind->cell_info.net_mode+1,
        gmmrr_cell_ind->cell_info.cell_env.rai.lac,
        gmmrr_cell_ind->cell_info.cell_env.rai.rac,
        gmmrr_cell_ind->cell_info.cell_env.cid);
#else
  TRACE_11_PARA("%s,%x%x%x, %x%x%x, NMO: %d, lac %x, rac %x, cid %x", 
        GMMRR_SERVICE_FULL == gmmrr_cell_ind->cell_info.service_state?"service":(GMMRR_SERVICE_LIMITED== gmmrr_cell_ind->cell_info.service_state?"limited service":"NO GPRS service"),
        gmmrr_cell_ind->cell_info.cell_env.rai.plmn.mcc[0],
        gmmrr_cell_ind->cell_info.cell_env.rai.plmn.mcc[1],
        gmmrr_cell_ind->cell_info.cell_env.rai.plmn.mcc[2],
        gmmrr_cell_ind->cell_info.cell_env.rai.plmn.mnc[0],
        gmmrr_cell_ind->cell_info.cell_env.rai.plmn.mnc[1],
        gmmrr_cell_ind->cell_info.cell_env.rai.plmn.mnc[2],
        gmmrr_cell_ind->cell_info.net_mode+1,
        gmmrr_cell_ind->cell_info.cell_env.rai.lac,
        gmmrr_cell_ind->cell_info.cell_env.rai.rac,
        gmmrr_cell_ind->cell_info.cell_env.cid);
#endif
  
#ifdef REL99
  /* During initialiazation sgsnr_flag should be initialized to
   * the value 0 (CL_SGSN_REL_98_OR_OLDER)
   */
  /* The function getSGSNRelease is implemented by RR layer. It 
   * returns the current SGSN release flag - CL_SGSN_REL_98_OR_OLDER
   * or CL_SGSN_REL_99_ONWARDS
   */

  temp_sgsnr_flag = cl_nwrl_get_sgsn_release(); /* TCS 4.0 */
  TRACE_1_OUT_PARA ("SGSNR_FLAG is >>>>: %x", temp_sgsnr_flag); /* TCS 4.0 */
  /* CL can return three values. SGSN release can be R98, 
   * R99 or unknown. In case of SGSN release unknown 
   * we assume the value R98 and proceed.
   */
  if(PS_SGSN_99_ONWARDS != temp_sgsnr_flag)
  {
    temp_sgsnr_flag = PS_SGSN_98_OLDER;
  }

  /*Store the flag in context*/
  gmm_data->sync.sig_cell_info.sgsnr_flag = temp_sgsnr_flag;  /* TCS 4.0 */
#endif  

  gmm_data->sync.grr_cell_info = gmmrr_cell_ind->cell_info;
  PFREE (gmmrr_cell_ind);

  GMM_TRACE_GMM_DATA(GMM_DEBUG_PRINT_MASK_CID);

  gmm_data->sync.mm_cell_info.gprs_indicator=MMGMM_GPRS_SUPP_YES;
  gmm_data->sync.sig_cell_info.gmmrr_cell_ind_received = TRUE;
    
  switch( GET_STATE( SYNC ) )
  {

  case SYNC_IDLE:   
     /* 
      * 
      */
      if ( sync_is_cid_equal () )
      {
        /*
         * CELL IND in response to RESUME REQ
         */
         sync_send_cell_info();
      }
      else
      {
        /* 
         * Cell Info does not match, synchronize with ACTIVATE IND
         */
         SET_STATE ( SYNC, SYNC_WAIT_FOR_GSM );
        /*
         * Timer Supervision
         */
        TRACE_EVENT ( "Info: START: TSYNC" );
        vsi_t_start ( GMM_handle ,  sync_TSYNC, TSYNC );
      }
      break;

  case SYNC_WAIT_FOR_GPRS:
     /*
      * Stop Timer ACTIVATE SYNC
      */
      TRACE_EVENT ( "Info: STOP: TSYNC" );
      vsi_t_stop  ( GMM_handle, sync_TSYNC);

      if ( sync_is_cid_equal() )
      {
        /*
         * As expected
         */
         SET_STATE ( SYNC, SYNC_IDLE );
         sync_send_cell_info();
      }
      else
      {
        /* 
         * Cell Info does not match, wait for ACTIVATE IND
         */
         SET_STATE ( SYNC, SYNC_WAIT_FOR_GSM );
        /*
         * Timer Supervision
         */
         TRACE_EVENT ( "Info: START: TSYNC" );
         vsi_t_start ( GMM_handle ,  sync_TSYNC, TSYNC );
      }
      break;
  case SYNC_WAIT_FOR_GSM:
     /*
      * CELL IND again? Should not happen; timer runs already...
      */
      TRACE_ERROR (" CELL_IND repeatedly received ");
      break;

  default:
      TRACE_ERROR ("GMMRR_CELL_IND  wrong SYNC state");
      break;
  }
  GMM_RETURN;
} /* sync_gmmrr_cell_ind */

/*
+------------------------------------------------------------------------------
| Function    : sync_mmgmm_activate_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive MMGMM_ACTIVATE_IND
|
| Parameters  : *mmgmm_activate_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void sync_mmgmm_activate_ind (T_MMGMM_ACTIVATE_IND *mmgmm_activate_ind)
{
  GMM_TRACE_FUNCTION( "sync_mmgmm_activate_ind" );

#ifndef NTRACE
  switch (mmgmm_activate_ind->gprs_indicator)
  {
    case MMGMM_GPRS_SUPP_NO:
      TRACE_EVENT("IPar: NO GPRS Service!");
      break;
    case MMGMM_GPRS_SUPP_YES:
      TRACE_EVENT("IPar: cell supports GPRS");
      break;
    default:
      TRACE_ERROR("indicator is out of range");
      break;
  }

  switch (mmgmm_activate_ind->status)
  {
    case MMGMM_LIMITED_SERVICE:

      TRACE_EVENT("IPar: limited service");
      break;
    case MMGMM_FULL_SERVICE:
      TRACE_EVENT("IPar: full service");
      break;
    case MMGMM_CELL_SELECTED:
      TRACE_EVENT("IPar: cell changed");
      break;
    case MMGMM_WAIT_FOR_UPDATE:
      TRACE_EVENT("IPar: LUP REQ from MM");
      break;
    default:
      TRACE_ERROR("unknown MM status");
  }
#endif

  gmm_data->sync.mm_cell_info.cid            = mmgmm_activate_ind->cid;
  gmm_data->sync.mm_cell_info.gprs_indicator = mmgmm_activate_ind->gprs_indicator;
  gmm_data->sync.mm_cell_info.lac         = mmgmm_activate_ind->lac;
  gmm_data->sync.mm_cell_info.plmn        = mmgmm_activate_ind->plmn;
  gmm_data->sync.mm_cell_info.status      = mmgmm_activate_ind->status;
  gmm_data->sync.sig_cell_info.mm_status  = gmm_data->sync.mm_cell_info.status;
  gmm_data->sync.mm_cell_info.t3212_val   = mmgmm_activate_ind->t3212_val;
  PFREE (mmgmm_activate_ind);

  GMM_TRACE_GMM_DATA(GMM_DEBUG_PRINT_MASK_CID);

  switch( GET_STATE( SYNC ) )
  {
  case SYNC_IDLE: 

      if (sync_is_cid_equal())
      {
        /*
         * signal cell info
         */
         sync_send_cell_info();
      }
      else
      {
        /* 
         * synchronize with CELL IND
         */
         SET_STATE ( SYNC, SYNC_WAIT_FOR_GPRS );
        /*
         * Timer Supervision
         */
         TRACE_EVENT ( "Info: START: TSYNC" );
         vsi_t_start ( GMM_handle ,  sync_TSYNC, TSYNC );
      }
      break;

  case SYNC_WAIT_FOR_GSM:
     /*
      * Stop Timer CELL SYNC
      */
      TRACE_EVENT ( "Info: STOP: TSYNC" );
      vsi_t_stop  ( GMM_handle, sync_TSYNC);

      if (sync_is_cid_equal())
      {
        /*
         * As expected
         */
         SET_STATE ( SYNC, SYNC_IDLE );
         sync_send_cell_info();
      }
      else
      {
        /* 
         * Cell Info does not match, wait for CELL IND
         */
         SET_STATE ( SYNC, SYNC_WAIT_FOR_GPRS );
        /*
         * Timer Supervision
         */
         TRACE_EVENT ( "Info: START: TSYNC" );
         vsi_t_start ( GMM_handle ,  sync_TSYNC, TSYNC );
      }
      break;

  case SYNC_WAIT_FOR_GPRS:
     /*
      * Again, should not happen! Timer runs already..
      */
      TRACE_ERROR (" ACTIVATE_IND repeatedly received ");
      break;

  default:
      TRACE_ERROR ("MMGMM_ACTIVATE_IND wrong SYNC state");
      break;
  }
  GMM_RETURN;
} /* sync_mmgmm_activate_ind */

/*
+------------------------------------------------------------------------------
| Function    : sync_mmgmm_nreg_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive MMGMM_NREG_IND
|
| Parameters  : *mmgmm_nreg_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void sync_mmgmm_nreg_ind (T_MMGMM_NREG_IND *mmgmm_nreg_ind)
{
  GMM_TRACE_FUNCTION( "sync_mmgmm_nreg_ind" );
    
  switch( GET_STATE( SYNC ) )
  {

    case SYNC_WAIT_FOR_GSM:
     /*
      * NREG IND received while waiting for ACTIVATE IND
      */
      TRACE_EVENT ( "Info: STOP: TSYNC" );
      vsi_t_stop ( GMM_handle , sync_TSYNC);
     /*
      * handle as if ACTIVATE IND received
      */
      SET_STATE ( SYNC, SYNC_IDLE );
     /*
      * 
      */
      switch (mmgmm_nreg_ind->service) /* TCS 2.1 */
      {
        case NREG_NO_SERVICE: /* TCS 2.1 */
          gmm_data->sync.mm_cell_info.lac = GMMRR_LA_INVALID;
          gmm_data->sync.mm_cell_info.status = MMGMM_LIMITED_SERVICE;
          gmm_data->sync.mm_cell_info.gprs_indicator=MMGMM_GPRS_SUPP_NO;
          break;
        case NREG_LIMITED_SERVICE:
          gmm_data->sync.mm_cell_info.status = MMGMM_LIMITED_SERVICE;
          /* TCS 2.1 */
          /* TCS 2.1 */
          break;
        default:
          break;
      }
     /*
      * SIGNAL CELL INFO
      */
      sync_send_cell_info();
     /*
      * SIGNAL NREG IND 
      */
      sig_sync_kern_mmgmm_nreg_ind(mmgmm_nreg_ind);
      break;

    case SYNC_IDLE: 
     /*
      * safe information: no or limited service
      */
      switch (mmgmm_nreg_ind->service) /* TCS 2.1 */
      {
        case NREG_NO_SERVICE: /* TCS 2.1 */
          gmm_data->sync.mm_cell_info.lac = GMMRR_LA_INVALID;
          gmm_data->sync.mm_cell_info.status = MMGMM_LIMITED_SERVICE;
          gmm_data->sync.mm_cell_info.gprs_indicator=MMGMM_GPRS_SUPP_NO;
          break;
        case NREG_LIMITED_SERVICE:
          gmm_data->sync.mm_cell_info.status = MMGMM_LIMITED_SERVICE;
          /* TCS 2.1 */
          /* TCS 2.1 */
          break;
        default:
          break;
      }
     /*
      * synchronize
      */
      if (sync_is_cid_equal())
      {
        /*
         * signal cell info
         */
         sync_send_cell_info();
      }
      else
      {
        /* 
         * synchronize with CELL IND
         */
         SET_STATE ( SYNC, SYNC_WAIT_FOR_GPRS );
        /*
         * Timer Supervision
         */
         TRACE_EVENT ( "Info: START: TSYNC" );
         vsi_t_start ( GMM_handle ,  sync_TSYNC, TSYNC );
      }
     /*
      * handle nreg ind
      */
      sig_sync_kern_mmgmm_nreg_ind(mmgmm_nreg_ind);

      break;
      
    case SYNC_WAIT_FOR_GPRS:

      TRACE_EVENT ( "Info: STOP: TSYNC" );
      vsi_t_stop ( GMM_handle , sync_TSYNC);

      SET_STATE ( SYNC, SYNC_IDLE);
      
      switch (mmgmm_nreg_ind->service) /* TCS 2.1 */
      {
        case NREG_NO_SERVICE: /* TCS 2.1 */
          gmm_data->sync.mm_cell_info.lac = GMMRR_LA_INVALID;
          gmm_data->sync.mm_cell_info.status = MMGMM_LIMITED_SERVICE;
          gmm_data->sync.mm_cell_info.gprs_indicator=MMGMM_GPRS_SUPP_NO;
          break;
        case NREG_LIMITED_SERVICE:
          gmm_data->sync.mm_cell_info.status = MMGMM_LIMITED_SERVICE;
          /* TCS 2.1 */
          /* TCS 2.1 */
          break;
        default:
          break;
      }

      sig_sync_kern_mmgmm_nreg_ind(mmgmm_nreg_ind);
      break;
    default:
      TRACE_ERROR ("MMGMM_NREG_IND    unexpected");
      PFREE(mmgmm_nreg_ind);
      break;
  }
  GMM_RETURN;
} /* sync_mmgmm_nreg_ind */
/*
+------------------------------------------------------------------------------
| Function    : sync_mmgmm_reg_rej
+------------------------------------------------------------------------------
| Description : Handles the primitive MMGMM_REG_REJ
|
| Parameters  : *mmgmm_nreg_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void sync_mmgmm_reg_rej (T_MMGMM_REG_REJ *mmgmm_reg_rej)
{
  GMM_TRACE_FUNCTION( "sync_mmgmm_reg_rej" );
    
  switch( GET_STATE( SYNC ) )
  {
    case SYNC_IDLE:   
    case SYNC_WAIT_FOR_GSM:
    case SYNC_WAIT_FOR_GPRS:
      gmm_data->sync.mm_cell_info.status = MMGMM_LIMITED_SERVICE;
      switch (mmgmm_reg_rej->service) /* TCS 2.1 */
      {
        case NREG_NO_SERVICE: /* TCS 2.1 */
          gmm_data->sync.mm_cell_info.lac = GMMRR_LA_INVALID;
          break;
        case NREG_LIMITED_SERVICE:
        /* TCS 2.1 */
          break;
        default:
          break;
      }
      sig_sync_kern_mmgmm_reg_rej(mmgmm_reg_rej);
      break;
    default:
      TRACE_ERROR ("MMGMM_REG_REJ    unexpected");
      break;
  }
  GMM_RETURN;
} /* sync_mmgmm_reg_rej */

/*
+------------------------------------------------------------------------------
| Function    : sync_mmgmm_reg_cnf
+------------------------------------------------------------------------------
| Description : Handles the primitive MMGMM_REG_CNF
|
| Parameters  : *mmgmm_reg_cnf - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void sync_mmgmm_reg_cnf (T_MMGMM_REG_CNF *mmgmm_reg_cnf)
{
  GMM_TRACE_FUNCTION( "sync_mmgmm_reg_cnf" );
    
  switch( GET_STATE( SYNC ) )
  {

    case SYNC_IDLE:   
    case SYNC_WAIT_FOR_GSM:
    case SYNC_WAIT_FOR_GPRS:

      /* Changes for Boot Time Speedup. GMM will get MMGMM_REG_CNF with 
       * bootup_cause = PWR_SCAN_START.
       * No need to process this as it is response to dummy request. 
       * GMM need to send GMMREG_ATTACH_CNF indicating PWR_SCAN_START
       */
      if (mmgmm_reg_cnf->bootup_cause EQ PWR_SCAN_START)
      {
        PALLOC ( gmmreg_attach_cnf, GMMREG_ATTACH_CNF );
        gmmreg_attach_cnf->attach_type = VAL_ATTACH_TYPE___DEF;
        gmmreg_attach_cnf->plmn = mmgmm_reg_cnf->plmn;
        gmmreg_attach_cnf->search_running = SEARCH_RUNNING;
        gmmreg_attach_cnf->lac = mmgmm_reg_cnf->lac;
        gmmreg_attach_cnf->rac =  gmm_data->kern.sig_cell_info.env.rai.rac;
        gmmreg_attach_cnf->cid = mmgmm_reg_cnf->cid; // #HM#
        gmmreg_attach_cnf->gprs_indicator = gmm_data->kern.sig_cell_info.gmm_status;
        gmmreg_attach_cnf->bootup_cause = PWR_SCAN_START;
#ifdef GMM_TCS4
        gmmreg_attach_cnf->rt = gmm_data->kern.sig_cell_info.rt; // TCS 4.0
#endif
        PSEND ( hCommMMI, gmmreg_attach_cnf );
        return;
      }
      gmm_data->sync.mm_cell_info.status = MMGMM_FULL_SERVICE;
      sig_sync_kern_mmgmm_reg_cnf(mmgmm_reg_cnf);
      break;

    default:
      TRACE_ERROR ("MMGMM_REG_CNF    unexpected");
      break;
  }
  GMM_RETURN;
} /* sync_mmgmm_nreg_ind */


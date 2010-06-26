/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  gmm_kernp.c
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
|             functions to handles the incoming primitives as described in 
|             the SDL-documentation (KERN-statemachine)
+----------------------------------------------------------------------------- 
*/ 



#ifndef GMM_KERNP_C
#define GMM_KERNP_C
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

#include "gmm_f.h"      /* to ger the gobal functions */
#include "gmm_kernl.h"      /* to ger the local functions */
#include "gmm_kernf.h"      /* to ger the local functions */
#include "gmm_rdys.h"      /* to get some signals */
#include  <string.h>    /* to get memcpy */
#include "gmm_txs.h"    /* to get some signals */
#include "gmm_syncs.h"
#include "gmm_em.h"     /* To get Engineering Mode functions */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : kern_t3302
+------------------------------------------------------------------------------
| Description : Handles expiry of timer T3302. This function is called from
|               pei_primitive().
|               Timeout for Attach or RAU failure
| Parameters  : none
|
+------------------------------------------------------------------------------
*/
GLOBAL void  kern_t3302(void)
{
  GMM_TRACE_FUNCTION( "kern_t3302" );
  TRACE_0_INFO("TIMEOUT T3302");
  sig_kern_rdy_stop_t3302_req();
  /*
   * reset attach attempt counter
   * <R.GMM.ATTACH.M.010>
   */
  /*
   * reset rau attempt counter
   * <R.GMM.ATTACH.M.010>
   */


  switch( GET_STATE( KERN ) )
  {
    case KERN_GMM_DEREG_RESUMING:
    case KERN_GMM_DEREG_SUSPENDING:
    case KERN_GMM_DEREG_SUSPENDED:
      gmm_data->kern.attach_cap.enter_attempting_to_update_after_lau=FALSE;
      break;
    case KERN_GMM_REG_SUSPENDING:
      gmm_data->kern.attach_cap.enter_attempting_to_update_after_lau=FALSE;
      SET_STATE(GU,GU_UPDATE_NEEDED);
      break;
    case KERN_GMM_REG_RESUMING:
    case KERN_GMM_REG_SUSPENDED:
      gmm_data->kern.attach_cap.enter_attempting_to_update_after_lau=FALSE;
      SET_STATE(GU,GU_UPDATE_NEEDED);
      break;
    case KERN_GMM_DEREG_ATTEMPTING_TO_ATTACH:
      kern_attach_reset();
      kern_attach ();
      break;
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE:
      kern_attach_reset();

      if (PERIODIC_RAU == gmm_data->kern.attach_cap.update_proc_type)
      {
        /* 44.2.3.3.2 */
        kern_periodic_rau();
      }
      else
      {
        kern_rau_limited();
      }

      break;
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE_MM:
      /*
       * GPRS is still attached (normal service)
       * so we need to resume LLC again
       */
      kern_attach_reset();
      kern_rau();
      break;
    default:
      TRACE_ERROR( "kern_t3302 unexpected" );
      break;
  }
  GMM_RETURN;
  
} /* kern_t3302 */

/*
+------------------------------------------------------------------------------
| Function    : kern_t3310
+------------------------------------------------------------------------------
| Description : Handles expiry of timer T3310. This function is called from
|               pei_primitive().
|               Timeout for ATTACH
| Parameters  : none
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_t3310 (void)
{
  GMM_TRACE_FUNCTION( "kern_t3310" );
  TRACE_0_INFO("TIMEOUT T3310");

  gmm_data->kern.attach_cap.t3310_value=0;

  switch( GET_STATE( KERN ) )
  {
    case KERN_GMM_REG_INITIATED:
      /************************************************************************
       * MSC: 3.8 Attach
       *
       * MSC: 3.8.1 Normal Attach
       * MSC: 3.8.1.4 Abnormal cases
       * MSC: 3.8.1.4.3 c) T3310 time-out
       ************************************************************************/
      if ( gmm_data->kern.ct3310 < MAX_CT3310 )
      {
        /*
         * ATTACH
         * <R.GMM.AGABNORM.M.007>
         */
        kern_gmmreg_detach (  GMMREG_DT_GPRS,  
            GMMCS_NET_FAIL, /* TCS 2.1 */
            GMMREG_SEARCH_RUNNING,
            GMMCS_NET_FAIL);

        kern_attach();
      }
      else
      {
        /*
         * AAC
         *<R.GMM.AGABNORM.M.008>
         */
        kern_aac(GMMCS_NO_SERVICE); /* TCS 2.1 */
      }

      break;
    case KERN_GMM_RAU_INITIATED:
      /************************************************************************
       * MSC: 3.10 RAU
       *
       * MSC: 3.10.1 Normal and periodic RAU initiation
       * MSC: 3.10.1.4 Abnormal cases
       * MSC: 3.10.1.4.3 c) T3310 time-out
       *
       ************************************************************************/
      /* 
       * I reused the timer T3310 instead of T3330 
       */
      if ( gmm_data->kern.ct3310 < MAX_CT3310 )
      {
        if (PERIODIC_RAU == gmm_data->kern.attach_cap.update_proc_type)
        {
          /* 44.2.3.3.2 */
          kern_periodic_rau();
        }
        else
        {
          kern_rau_limited();
        }
      }
      else
      {
        /*
         * AAC
         */
        kern_aac(GMMCS_NO_SERVICE); /* TCS 2.1 */
      }
      break;
    /* START PATCH UBUOB ISSUE 8274 */
    case KERN_GMM_REG_RESUMING:
    case KERN_GMM_REG_SUSPENDING:
    case KERN_GMM_REG_SUSPENDED:
      gmm_data->kern.attach_cap.enter_attempting_to_update_after_lau = FALSE;
      SET_STATE(GU,GU_UPDATE_NEEDED);
      break;
    /* END PATCH UBUOB ISSUE 8274 */
    case KERN_GMM_REG_LIMITED_SERVICE:
      TRACE_EVENT("T3310 ignored in limited mode"); 
      break;
    default:
      TRACE_ERROR( "kern_t3310 unexpected" );
      break;
  }
  GMM_RETURN;
} /* kern_t3310 */
/*
+------------------------------------------------------------------------------
| Function    : kern_t3311
+------------------------------------------------------------------------------
| Description : Handles expiry of timer T3310. This function is called from
|               pei_primitive().
|               Timeout for ATTACH or RAU Reject
| Parameters  : none
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_t3311 (void)
{
  GMM_TRACE_FUNCTION( "kern_t3311" );
  TRACE_0_INFO("TIMEOUT T3311");
  switch( GET_STATE( KERN ) )
  {
    case KERN_GMM_DEREG_ATTEMPTING_TO_ATTACH:
      kern_attach ();
      break;
    case  KERN_GMM_REG_NORMAL_SERVICE:
      /*
       * <R.GMM.RNABNORM.M.022> 
       */
      kern_periodic_rau();
      break;
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE_MM:
      kern_llgmm_suspend(LLGMM_RAU);
      /*FALLTHROUGH*/
      //lint -fallthrough
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE:
      kern_rau_limited ();
      break;
    /* START PATCH UBUOB ISSUE 8893 */
    case KERN_GMM_DEREG_SUSPENDING:
    case KERN_GMM_DEREG_SUSPENDED:
    case KERN_GMM_DEREG_RESUMING:
    /* END PATCH UBUOB ISSUE 8893 */
    case KERN_GMM_REG_RESUMING:
    /* START PATCH UBUOB ISSUE 8273 */
    case KERN_GMM_REG_SUSPENDING:
    case KERN_GMM_REG_SUSPENDED:
      gmm_data->kern.attach_cap.enter_attempting_to_update_after_lau = FALSE;
      SET_STATE(GU,GU_UPDATE_NEEDED);
      break;
    /* END PATCH UBUOB ISSUE 8273 */
    default:
      TRACE_ERROR( "kern_t3311 unexpected" );
      break;
  }
  
  GMM_RETURN;
} /* kern_t3311 */
/*
+------------------------------------------------------------------------------
| Function    : kern_t3312
+------------------------------------------------------------------------------
| Description : Handles expiry of timer T3312. This function is called from
|               pei_primitive().

|               Periodic RAU timer
| Parameters  : none
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_t3312 (void)
{
  GMM_TRACE_FUNCTION( "kern_t3312" );
  TRACE_0_INFO("TIMEOUT T3312");
  switch( GET_STATE( KERN ) )
  {
    
    case KERN_GMM_REG_NORMAL_SERVICE:
      if (GMM_GRR_STATE_CR==gmm_data->kern.attach_cap.grr_state)
      {
         gmm_data->kern.timeout_t3312 = TRUE;
         SET_STATE(GU,GU_UPDATE_NEEDED);
      }
      else
      {
        kern_periodic_rau();
      }
      break;
    case KERN_GMM_REG_NO_CELL_AVAILABLE:
      /*
       * 24.008 ch 4.7.2.2. at the end:
       *
       * If the MS is both IMSI attached for GPRS and non-GPRS services in a network that
       * operates in network operation mode I, and if the MS has camped on a cell that does
       * not support GPRS, and timer T3312 expires, then the MS shall start an MM location
       * updating procedure. In addition, the MS shall perform a combined routing area update
       * procedure indicating "combined RA/LA updating with IMSI attach" when the MS enters
       * a cell that supports GPRS and indicates that the network is in operation mode I.
       */
      gmm_data->kern.timeout_t3312 = TRUE;
      SET_STATE(GU,GU_UPDATE_NEEDED);
      
      if (GMM_MM_DEREG != GET_STATE(MM)
      && GMMRR_LA_INVALID!=gmm_data->kern.sig_cell_info.env.rai.lac)
      {
        SET_STATE ( KERN, KERN_GMM_REG_SUSPENDING);
#ifdef FF_EM_MODE
        EM_GMM_SET_STATE(KERN_GMM_REG_SUSPENDING);
#endif
        kern_llgmm_suspend(LLGMM_CALL); /* in no cell availble LLC is alread susended */
        kern_gmmrr_suspend(GMMRR_NOT_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_LAU);
      }      
      break;


    case KERN_GMM_REG_LIMITED_SERVICE:
    case KERN_GMM_REG_IMSI_DETACH_INITIATED:
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE_MM:
    case KERN_GMM_REG_RESUMING:
    case KERN_GMM_REG_SUSPENDING:
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE:
    case KERN_GMM_REG_SUSPENDED:
        /* no break; */
        /*
         * <R.GMM.RAUTIMER.M.009>
         */
      gmm_data->kern.timeout_t3312 = TRUE;
      SET_STATE(GU,GU_UPDATE_NEEDED);
      break;
    default:
      break;
  }
  
  GMM_RETURN;
} /* kern_t3312 */
/*
+------------------------------------------------------------------------------
| Function    : kern_t3321
+------------------------------------------------------------------------------
| Description : Handles expiry of timer T3321. This function is called from
|               pei_primitive().
|               Timeout for DETACH
|
|               MSC: 3.9 GPRS detach procedure
|               
|               MCS: 3.9.3 Abnormal cases
|               MSC: 3.9.3.1 a) Timeout of timer T3321
|
| Parameters  : none
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_t3321 (void)
{

#ifdef REL99
  T_gmobile_identity gmobile_identity; /* TCS 4.0 */
#endif

  GMM_TRACE_FUNCTION( "kern_t3321" );
  TRACE_0_INFO("TIMEOUT T3321");
  /*
   * MSC: 3.9 GPRS detach procedure
   *
   * MCS: 3.9.3 Abnormal cases
   * MSC: 3.9.3.1 a) Timeout of timer T3321
   */
  if ( ++gmm_data->kern.ct3321 < 5 )
  {
    MCAST (u_detach_request, U_DETACH_REQUEST);
    
      TRACE_EVENT ("  START T3321" );
      sig_kern_rdy_start_timer_req ( kern_T3321, gmm_data->kern.t3321_val );
    
      u_detach_request->u_detach_type.u_type_of_detach 
                          = gmm_data->kern.detach_cap.detach_type,
      u_detach_request->u_detach_type.power_off = PO_NORMAL_DETACH;


#ifdef REL99
    /* Detach request sent to a Release 99 network has two addional
     * parameters. PTMSI and PTMSI signature  
     */        
    switch (gmm_data->kern.sig_cell_info.sgsnr_flag)/*!gmm_data->release99 =>old code*/ /* TCS 4.0 */
    {
      default:
      case PS_SGSN_UNKNOWN:
      case PS_SGSN_98_OLDER:
        u_detach_request->v_gmobile_identity = FALSE; /* TCS 4.0 */
        u_detach_request->v_p_tmsi_signature_2 = FALSE; /* TCS 4.0 */
        break;
      case PS_SGSN_99_ONWARDS:      
        /* 
         * PTMSI
         */
        kern_ulong2mobile_identity( gmm_data->ptmsi.current, &gmobile_identity ); /* TCS 4.0 */
        u_detach_request->v_gmobile_identity = TRUE; /* TCS 4.0 */
        u_detach_request->gmobile_identity   = gmobile_identity; /* TCS 4.0 */
        
        /* 
         * PTMSI Signature
         */
        if(gmm_data->ptmsi_signature.available) /* TCS 4.0 */
        { /* TCS 4.0 */
          u_detach_request->v_p_tmsi_signature_2 =TRUE; /* TCS 4.0 */
          u_detach_request->p_tmsi_signature_2.p_tmsi_signature_value /* TCS 4.0 */
                             = gmm_data->ptmsi_signature.value; /* TCS 4.0 */
        } /* TCS 4.0 */
        else  /* TCS 4.0 */
        { /* TCS 4.0 */
          u_detach_request->v_p_tmsi_signature_2=FALSE; /* TCS 4.0 */
        } /* TCS 4.0 */
        break;
      }
#endif


      switch ( gmm_data->kern.detach_cap.detach_type )
      {
        case GMMREG_DT_GPRS:
          u_detach_request->u_detach_type.u_type_of_detach 
                          = DT_GPRS;
          break;
        case GMMREG_DT_IMSI:
          u_detach_request->u_detach_type.u_type_of_detach 
                          = DT_IMSI;
          break;
        case GMMREG_DT_COMB:
          u_detach_request->u_detach_type.u_type_of_detach 
                          = DT_COMB;
          break;
        case GMMREG_DT_SIM_REMOVED:
          if ( GMM_MM_DEREG != GET_STATE(MM) )
          {
            u_detach_request->u_detach_type.u_type_of_detach 
                          = DT_COMB;
          }
          else
          {
            u_detach_request->u_detach_type.u_type_of_detach 
                          = DT_GPRS;
          }
          break;
        default:
          TRACE_ERROR ("unexpexted GMMREG_DT_TYPE");
          break;
      }
      kern_mm_detach_started ();    
      sig_kern_tx_data_req ( CURRENT_TLLI, U_DETACH_REQUEST);
  }
  else
  {    
    switch( GET_STATE( KERN ) )
    {
      case KERN_GMM_DEREG_INITIATED:
        kern_local_detach( GMMCS_INT_NOT_PRESENT, FALSE, GMM_LOCAL_DETACH_PROC_ENTER_DEREG); /* TCS 2.1 */
        break;  
      case KERN_GMM_REG_IMSI_DETACH_INITIATED:
        /*
         * <R.GMM.DMABNORM.M.020>
         */
        kern_local_detach( GMMCS_INT_NOT_PRESENT, FALSE, GMM_LOCAL_DETACH_PROC_ENTER_REG_NORMAL); /* TCS 2.1 */
        break;
      default:
        TRACE_ERROR( "kern_t3321 unexpected" );
        break;
    }
  }
  GMM_RETURN;
} /* kern_t3321 */
/*
+------------------------------------------------------------------------------
| Function    : kern_tpower_off
+------------------------------------------------------------------------------
| Description : Handles expiry of timer Tpower_off. This function is called from
|               pei_primitive().
|               Timeout for power_off after sending DETACH message
| Parameters  : none
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_tpower_off (void)
{
  GMM_TRACE_FUNCTION( "kern_tpower_off" );
  TRACE_0_INFO("TIMEOUT TPOWER_OFF");
  kern_local_detach( GMMCS_POWER_OFF, FALSE, /* TCS 2.1 */
                GMMREG_DT_SOFT_OFF==gmm_data->kern.detach_cap.detach_type?
                GMM_LOCAL_DETACH_PROC_SOFT_OFF:
                GMM_LOCAL_DETACH_PROC_POWER_OFF);

  GMM_RETURN;
  
} /* kern_tpower_off */

/*
+------------------------------------------------------------------------------
| Function    : kern_tlocal_detach
+------------------------------------------------------------------------------
| Description : Handles expiry of timer Tlocal_detach. This function is called from
|               pei_primitive().
|               function local_detach is called to locally detach the MS.
|               GRR and LLC can not unassigned immediately. So GMM waits untill 
|               the tlocal_detach timer has been expired or either CGRLC_TRIGGER_IND
|               has been received or GMMRR_SUSPEND_CNF has been received.
| Parameters  : none
|
+------------------------------------------------------------------------------
*/
GLOBAL void  kern_tlocal_detach (void)
{
  GMM_TRACE_FUNCTION( "kern_tlocal_detach" );


  switch( GET_STATE( KERN ) )
  {
    default:
      TRACE_ERROR("tlocal_detach not in state DEREG_SUSPENDING");
      /* NO break; */
    case KERN_GMM_DEREG_SUSPENDING:
      kern_gmmrr_suspend (GMMRR_NOT_SUSP_GPRS, GMMRR_NORMAL_RELEASE, 
                GMM_SUSP_UNKNOWN);  

      break;
  }

  GMM_RETURN  ;
} /* kern_tlocal_detach */




/*
+------------------------------------------------------------------------------
| Function    : kern_gmmreg_net_req
+------------------------------------------------------------------------------
| Description : Handles the primitive GMMREG_NET_REQ
|
|          MSC: 3.20 MM Interface
|          MSC: 3.20.2 Network selection
|
| Parameters  : *gmmreg_net_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_gmmreg_net_req ( T_GMMREG_NET_REQ *gmmreg_net_req )
{ 
  GMM_TRACE_FUNCTION( "kern_gmmreg_net_req" );

  PFREE (gmmreg_net_req);
  kern_mm_net_req();
  GMM_RETURN;
} /* kern_gmmreg_net_req() */

/*
+------------------------------------------------------------------------------
| Function    : kern_gmmreg_plmn_res
+------------------------------------------------------------------------------
| Description : Handles the primitive GMMREG_PLMN_RES
|
|          MSC: 3.20 MM Interface
|          MSC: 3.20.2 Network selection
|
| Parameters  : *gmmreg_plmn_res - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_gmmreg_plmn_res ( T_GMMREG_PLMN_RES *gmmreg_plmn_res )
{ 
  GMM_TRACE_FUNCTION( "kern_gmmreg_plmn_res" );

  gmm_data->kern.attach_cap.network_selection_mode=MODE_MAN;
  kern_remove_plmn_from_forbidden_list(gmmreg_plmn_res->plmn);
    
  /* START PATCH UBUOB ISSUE 8276 */
  /* T3302 should not be stopped here, because this is dangerous if the */
  /* user selection does not lead to a different PLMN */
  /*vsi_t_stop  ( GMM_handle, kern_T3302);*/
  /*sig_kern_rdy_start_t3302_req();*/
  /* END PATCH UBUOB ISSUE 8276 */

  /* When we receive manual mode network selection stop TSYNC timer */
  vsi_t_stop  ( GMM_handle, sync_TSYNC);

  gmm_data->kern.attach_cap.enter_attempting_to_update_after_lau = FALSE;
  /*
   * attach_cap.gmmreg sets the caller of the attach procedure to MMI
   */
  gmm_data->kern.attach_cap.gmmreg = TRUE;
  /*
   * if the moble has been rejected with cause #11,#12, or #13 it has to igored
   * when user wanted the moile explicitly to try to attach
   * TC 44.2.1.1.4.3.2
   */
  gmm_data->kern.detach_cap.error_cause=GMMCS_INT_NOT_PRESENT; /* TCS 2.1 */

  /* Setting mm_status to FULL Service immediately on receiving PLMN Response creates a false display 
   * on MMI if selecting forbidden PLMN second time in a row. However, it is necessary in case of Normal PLMN selection
   * for an early indication to ACI. Hence setting mm_status to Full Service only if the selected PLMN was not rejected by 
   * the N/W in the same power cycle.
   */
  if((memcmp(gmm_data->kern.attach_cap.list_of_forbidden_plmns_for_gprs_service[0].mcc,gmmreg_plmn_res->plmn.mcc, SIZE_MCC) NEQ 0) AND
     (memcmp(gmm_data->kern.attach_cap.list_of_forbidden_plmns_for_gprs_service[0].mnc,gmmreg_plmn_res->plmn.mnc, SIZE_MNC)NEQ 0))
  {
    gmm_data->kern.sig_cell_info.mm_status = MMGMM_FULL_SERVICE;
    sig_kern_sync_set_mm_state(MMGMM_FULL_SERVICE);
  }

#ifndef NTRACE
  /*
   * My assumption is, that I have to know the class if plmn_res comes as power 
   * on primitive with AT+COPS=1,2,"001001". There are no possabilities  to 
   * change the classes. a class change is handeled with attach_req
   */
  if(gmm_data->kern.attach_cap.mobile_class!=gmmreg_plmn_res->mobile_class)
  {
    TRACE_ERROR("class change not expected: NO DETACH_REQ will be send!");
  }
#endif
  gmm_data->kern.attach_cap.plmn_requested  = gmmreg_plmn_res->plmn; 
  gmm_data->kern.attach_cap.mobile_class = gmmreg_plmn_res->mobile_class;  
  gmm_data->kern.attach_cap.attach_type = gmmreg_plmn_res->attach_type;
#ifndef NTRACE      
  switch ( gmm_data->kern.attach_cap.mobile_class )
  {
    case GMMREG_CLASS_CC:
      TRACE_0_PARA ("MS class CC");
      break;
    case GMMREG_CLASS_CG:
      TRACE_0_PARA ("MS class CG");
      break;
    case GMMREG_CLASS_B:
      TRACE_0_PARA ("MS class B");
      break;
    case GMMREG_CLASS_BG:
      TRACE_0_PARA ("MS class BG");
      break;
    case GMMREG_CLASS_BC:
      TRACE_0_PARA ("MS class BC");
      break;
    case GMMREG_CLASS_A:
      TRACE_0_PARA ("MS class A");
      break;
    default:
      TRACE_ERROR ("unknown mobile class");
      break;
  }
  switch (gmm_data->kern.attach_cap.attach_type)
  {
    case GMMREG_AT_IMSI:
      TRACE_EVENT ("----- GSM only attach -----");
      break;
    case GMMREG_AT_GPRS:
      TRACE_EVENT ("----- GPRS attach -----");
      break;
    case GMMREG_AT_COMB:
      TRACE_EVENT ("----- GSM and GPRS attach -----");
      break;
    default:
      TRACE_ERROR ("unknown attach type");
      break;
  }

#endif
  {
    PALLOC ( mmgmm_plmn_res, MMGMM_PLMN_RES);
      mmgmm_plmn_res->plmn = gmmreg_plmn_res->plmn;
      mmgmm_plmn_res->reg_type = REG_CELL_SEARCH_ONLY;
      PFREE (gmmreg_plmn_res);

    switch( GET_STATE( KERN ) )
    {
      case KERN_GMM_NULL_NO_IMSI_LIMITED_SERVICE_REQ:
      case KERN_GMM_NULL_IMSI_LIMITED_SERVICE_REQ:           
        mmgmm_plmn_res->reg_type = REG_GPRS_INACTIVE;
        break;
      case KERN_GMM_NULL_NO_IMSI:
        if (GMMREG_CLASS_CC==gmm_data->kern.attach_cap.mobile_class
        || GMMREG_AT_IMSI== gmm_data->kern.attach_cap.attach_type)
        {
          /* NO state change */
        }
        else
        {
          SET_STATE ( KERN, KERN_GMM_DEREG_NO_IMSI);
#ifdef FF_EM_MODE
          EM_GMM_SET_STATE(KERN_GMM_DEREG_NO_IMSI);
#endif
        }
        mmgmm_plmn_res->reg_type = REG_GPRS_INACTIVE;

        break;

      case KERN_GMM_DEREG_RESUMING:
      case KERN_GMM_REG_RESUMING:
      case KERN_GMM_DEREG_NO_IMSI:
        break;
      /*
       * GMM-DEREGISTERED
       */
      case KERN_GMM_DEREG_SUSPENDING:
        /* 
         * not reachable beause of save in pei 
         */
      case KERN_GMM_DEREG_SUSPENDED:
        kern_resume_grr_der();
        /* NO break;*/
      case KERN_GMM_DEREG_PLMN_SEARCH:
      case KERN_GMM_DEREG_LIMITED_SERVICE:
      case KERN_GMM_DEREG_NO_CELL_AVAILABLE:
      case KERN_GMM_DEREG_ATTEMPTING_TO_ATTACH:
        if (GMMREG_CLASS_CC==gmm_data->kern.attach_cap.mobile_class
        || GMMREG_AT_IMSI== gmm_data->kern.attach_cap.attach_type)
        {
          SET_STATE ( KERN, KERN_GMM_NULL_PLMN_SEARCH);
#ifdef FF_EM_MODE
          EM_GMM_SET_STATE(KERN_GMM_NULL_PLMN_SEARCH);
#endif
          mmgmm_plmn_res->reg_type = REG_GPRS_INACTIVE;
        }
        else
        {
          SET_STATE ( KERN, KERN_GMM_DEREG_PLMN_SEARCH);
#ifdef FF_EM_MODE
          EM_GMM_SET_STATE(KERN_GMM_DEREG_PLMN_SEARCH);
#endif
        }

        break;

      case KERN_GMM_NULL_PLMN_SEARCH:
      case KERN_GMM_NULL_IMSI:          
       
        if (GMMREG_CLASS_CC==gmm_data->kern.attach_cap.mobile_class
        || GMMREG_AT_IMSI== gmm_data->kern.attach_cap.attach_type)
        {
          SET_STATE ( KERN, KERN_GMM_NULL_PLMN_SEARCH);
#ifdef FF_EM_MODE
          EM_GMM_SET_STATE(KERN_GMM_NULL_PLMN_SEARCH);
#endif
          mmgmm_plmn_res->reg_type = REG_GPRS_INACTIVE;
        }
        else
        {
          SET_STATE ( KERN, KERN_GMM_DEREG_PLMN_SEARCH);
#ifdef FF_EM_MODE
          EM_GMM_SET_STATE(KERN_GMM_DEREG_PLMN_SEARCH);
#endif
          kern_gmmrr_enable();
        }

        break;

      case KERN_GMM_REG_NORMAL_SERVICE:

      case KERN_GMM_REG_NO_CELL_AVAILABLE:
      case KERN_GMM_REG_LIMITED_SERVICE:
      case KERN_GMM_RAU_INITIATED:
      case KERN_GMM_REG_INITIATED:
      case KERN_GMM_REG_ATTEMPTING_TO_UPDATE_MM:
      case KERN_GMM_REG_ATTEMPTING_TO_UPDATE:  
        break;
      case KERN_GMM_REG_SUSPENDING:
        /* 
         * not reachablebeause of save in pei 
         */
      case KERN_GMM_REG_SUSPENDED:
        /*
         * Attach requests in suspended mode will be handled after resumption
         * by triggering form GRR with GMMRR_CELL_IND
         */

        /*
         * The user is not allowed to change the mobile class or establish a 
         * voice call the same time manuall network search is started. it would 
         * not be nice
         */
        kern_resume_grr_reg();

        TRACE_ERROR("I hoped, that this never is happened, ...");
        break;
      default:
        TRACE_ERROR( "GMMREG_PLMN_RES unexpected" );
        break;
       
    }
      mmgmm_plmn_res->mobile_class=gmm_data->kern.attach_cap.mobile_class;  
    PSEND ( hCommMM, mmgmm_plmn_res );
  }
  GMM_RETURN;
} /* kern_gmmreg_plmn_res() */

/*
+------------------------------------------------------------------------------
| Function    : kern_gmmreg_plmn_mode_req
+------------------------------------------------------------------------------
| Description : Handles the primitive GMMREG_PLMN_MODE_REQ
|
|          MSC: 3.20 MM Interface
|          MSC: 3.20.1 Change of PLMN Mode
|
| Parameters  : *gmmreg_plmn_mode_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_gmmreg_plmn_mode_req ( T_GMMREG_PLMN_MODE_REQ *gmmreg_plmn_mode_req )
{ 
  GMM_TRACE_FUNCTION( "kern_gmmreg_plmn_mode_req" );
  {
    PPASS ( gmmreg_plmn_mode_req, mmgmm_plmn_mode_req, MMGMM_PLMN_MODE_REQ );
      /* 
       * PATCH UBUOB 11.3.02: if we change from MANUAL to AUTOMATC or vice versa,
       * then we need to clear the limited_service flag
       * Problem was: Bouyg SIM, manual registration on SFR upon return to
       * auto mode, first GPRS ATTACH
       
      if (gmm_data->kern.attach_cap.network_selection_mode 
          != gmmreg_plmn_mode_req->net_selection_mode)
      {
         gmm_data->kern.sig_cell_info.mm_status = MMGMM_FULL_SERVICE;
         sig_kern_sync_set_mm_state(MMGMM_FULL_SERVICE);
      }
      */
      /*
       * PLMN mode set to manual mode with PLMN_RES only
        gmm_data->kern.attach_cap.network_selection_mode = 
        gmmreg_plmn_mode_req->net_selection_mode;
       */
     if (GMMREG_NET_SEL_MODE_AUTO == gmmreg_plmn_mode_req->net_selection_mode)
     {
       gmm_data->kern.attach_cap.network_selection_mode = 
         gmmreg_plmn_mode_req->net_selection_mode;
     }
     else
     {
      TRACE_0_INFO ("manual mode ignored. will be set with PLMN_RES");
     }

    PSEND ( hCommMM, mmgmm_plmn_mode_req );
  }
  GMM_RETURN;
} /* kern_gmmreg_plmn_mode_req () */

/*
+------------------------------------------------------------------------------
| Function    : kern_gmmreg_config_req
+------------------------------------------------------------------------------
| Description : Handles the primitive GMMREG_CONFIG_REQ
|
| Parameters  : *gmmreg_config_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_gmmreg_config_req ( T_GMMREG_CONFIG_REQ *gmmreg_config_req )
{ 
  GMM_TRACE_FUNCTION( "kern_gmmreg_config_req" );
  gmm_data->config.cipher_on 
                  = gmmreg_config_req->cipher_on;
  PFREE (gmmreg_config_req);
  GMM_RETURN;
} /* kern_gmmreg_plmn_mode_req () */
/*
+------------------------------------------------------------------------------
| Function    : kern_gmmreg_detach_req
+------------------------------------------------------------------------------
| Description : Handles the primitive GMMREG_DETACH_REQ
|
| Parameters  : *gmmreg_detach_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_gmmreg_detach_req ( T_GMMREG_DETACH_REQ *gmmreg_detach_req )
{ 
  GMM_TRACE_FUNCTION( "kern_gmmreg_detach_req" );

  /*
   * RAU may interrupt detach procedure, so we have to store the detach type
   */
  gmm_data->kern.detach_cap.detach_type=gmmreg_detach_req->detach_type;
  gmm_data->kern.detach_cap.gmmreg=TRUE;
  
  switch (gmmreg_detach_req->detach_type)
  {
    case GMMREG_DT_GPRS:
      switch(gmm_data->kern.attach_cap.attach_type)
      {
        case GMMREG_AT_GPRS:
        case GMMREG_AT_NOT_KNOWN:
          gmm_data->kern.attach_cap.attach_type=GMMREG_AT_NOT_KNOWN;
          break;
        default:
        case GMMREG_AT_IMSI:
        case GMMREG_AT_COMB:
          gmm_data->kern.attach_cap.attach_type=GMMREG_AT_IMSI;
          break;
      }
      TRACE_EVENT ("Para: GPRS detach");
      break;
    case GMMREG_DT_IMSI:
      gmm_data->kern.attach_cap.attach_type=
        (GMMREG_CLASS_CC == gmm_data->kern.attach_cap.mobile_class?
            GMMREG_AT_NOT_KNOWN:
            GMMREG_AT_GPRS);
      TRACE_EVENT ("Para: IMSI detach");
      break;
    case GMMREG_DT_POWER_OFF:
      /*
       * TMSI reset to remove TMSI in GRR when gmmrr_assign_req will
       * be sent
       */
      gmm_data->tmsi = GMMRR_TMSI_INVALID;
      gmm_data->kern.attach_cap.attach_type=GMMREG_AT_NOT_KNOWN;
      gmm_data->kern.attach_cap.network_selection_mode=MODE_AUTO;
      TRACE_EVENT ("Para: Power OFF");
      break;
    case GMMREG_DT_SOFT_OFF:
      /*
       * TMSI reset to remove TMSI in GRR when gmmrr_assign_req will
       * be sent
       */
      gmm_data->tmsi = GMMRR_TMSI_INVALID;
      gmm_data->kern.attach_cap.attach_type=GMMREG_AT_NOT_KNOWN;
      TRACE_EVENT ("Para: Soft OFF");
      break;
    case GMMREG_DT_COMB:
      gmm_data->kern.attach_cap.service_mode=SERVICE_MODE_LIMITED;
      gmm_data->kern.attach_cap.attach_type=GMMREG_AT_NOT_KNOWN;
      TRACE_EVENT ("Para: GMMREG_DT_COMB");
      break;
    case GMMREG_DT_SIM_REMOVED:
      gmm_data->kern.attach_cap.attach_type=GMMREG_AT_NOT_KNOWN;
      TRACE_EVENT ("Para: GMMREG_DT_SIM_REMOVED");
      break;
    case GMMREG_DT_DISABLE_GPRS:
      TRACE_EVENT ("Para: GMMREG_DT_DISABLE_GPRS");
      break;
    case GMMREG_DT_LIMITED_SERVICE:
      gmm_data->kern.attach_cap.service_mode=SERVICE_MODE_LIMITED;
      TRACE_EVENT ("Para: GMMREG_DT_LIMITED_SERVICE");
      break;
    default:
      gmm_data->kern.attach_cap.attach_type=
        (GMMREG_CLASS_CG == gmm_data->kern.attach_cap.mobile_class?
            GMMREG_AT_NOT_KNOWN:
            GMMREG_AT_IMSI);
      vsi_o_ttrace(VSI_CALLER TC_ERROR,"Error: Unknown detach_type = %x", gmmreg_detach_req->detach_type);
    break;


  }

  gmm_data->kern.detach_cap.detach_type = gmmreg_detach_req->detach_type;  

  gmm_data->kern.detach_cap.gmmreg = TRUE;
  gmm_data->kern.attach_cap.gmmreg = FALSE;
#ifndef GMM_TCS4  
  gmm_data->kern.attach_cap.gmmsm  = FALSE;
#endif
  
  PFREE ( gmmreg_detach_req );

  switch( GET_STATE( KERN ) )
  {
    case KERN_GMM_REG_LIMITED_SERVICE:
    case KERN_GMM_REG_NO_CELL_AVAILABLE:
    case KERN_GMM_REG_TEST_MODE:  
    case KERN_GMM_REG_TEST_MODE_NO_IMSI:  
      switch ( gmm_data->kern.detach_cap.detach_type )
      {
        case GMMREG_DT_IMSI:
        case GMMREG_DT_COMB:
        case GMMREG_DT_SIM_REMOVED:
          SET_STATE ( KERN, KERN_GMM_REG_SUSPENDED);
#ifdef FF_EM_MODE
          EM_GMM_SET_STATE(KERN_GMM_REG_SUSPENDED);
#endif
          kern_mm_imsi_detach();
          break;
        case GMMREG_DT_DISABLE_GPRS:
          kern_local_detach( GMMCS_INT_NOT_PRESENT, FALSE,
             GMM_LOCAL_DETACH_PROC_ENTER_NULL_IMSI); /* TCS 2.1 */
          break;
        case GMMREG_DT_GPRS:
          kern_local_detach( GMMCS_INT_NOT_PRESENT, FALSE,
             GMM_LOCAL_DETACH_PROC_ENTER_NULL_IMSI); /* TCS 2.1 */
          break;
        case GMMREG_DT_POWER_OFF:
          kern_local_detach( GMMCS_POWER_OFF, FALSE, /* TCS 2.1 */
                GMM_LOCAL_DETACH_PROC_POWER_OFF);
          break;
        case GMMREG_DT_SOFT_OFF:
          kern_local_detach( GMMCS_POWER_OFF, FALSE, /* TCS 2.1 */
                GMM_LOCAL_DETACH_PROC_SOFT_OFF);

          break;
        default:
          TRACE_ERROR ("Unexpected detach type in primitive GMMREG_DETACH_REQ");
          break;
      }
      break;
    case KERN_GMM_REG_INITIATED:
      /* 
       * This cause is abnormal, because the normal and not the local detach 
       * procedure is started on power off 
       *
       * MSC: 3.8.1.4.7 g) Power off
       * <R.GMM.AGABNORM.M.015>,
       * <R.GMM.AGABNORM.M.031>
       */
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE:
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE_MM:
      kern_gmmrr_attach_finished();
      /* NO break;*/

    case KERN_GMM_REG_NORMAL_SERVICE:
      switch ( gmm_data->kern.detach_cap.detach_type )
      {
        case GMMREG_DT_IMSI:
        case GMMREG_DT_COMB:
        case GMMREG_DT_SIM_REMOVED:
          switch (gmm_data->kern.sig_cell_info.net_mode)
          {
            case GMMRR_NET_MODE_I:
              kern_detach( );
              break;
            case GMMRR_NET_MODE_II:
              SET_STATE ( KERN, KERN_GMM_REG_SUSPENDING);
#ifdef FF_EM_MODE
              EM_GMM_SET_STATE(KERN_GMM_REG_SUSPENDING);
#endif
              kern_llgmm_suspend(LLGMM_CALL);
              kern_gmmrr_suspend(GMMRR_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_IMSI_DETACH);
              break;
            case GMMRR_NET_MODE_III:
              kern_detach( );
              break;
            default:
              TRACE_ERROR ("unknown net mode");
              break;
          }
          break;
        case GMMREG_DT_DISABLE_GPRS:
        case GMMREG_DT_GPRS:
        case GMMREG_DT_POWER_OFF:
        case GMMREG_DT_SOFT_OFF:
          kern_detach( );
          break;

        default:
          TRACE_ERROR ("Unexpected detach type in primitive GMMREG_DETACH_REQ");
          break;
      }
      break;
    
    case KERN_GMM_REG_IMSI_DETACH_INITIATED:
      switch ( gmm_data->kern.detach_cap.detach_type )
      {
        case GMMREG_DT_DISABLE_GPRS:
          gmm_data->kern.attach_cap.mobile_class = GMMREG_CLASS_CC;
          /* NO break */
        default:
          kern_detach( );
          break;
      }
      break;
 
    case KERN_GMM_REG_SUSPENDED:
      switch ( gmm_data->kern.detach_cap.detach_type )
      {
        case GMMREG_DT_IMSI:
        case GMMREG_DT_COMB:
        case GMMREG_DT_SIM_REMOVED:
          kern_mm_imsi_detach();
          break;
        case GMMREG_DT_DISABLE_GPRS:
          gmm_data->kern.attach_cap.mobile_class = GMMREG_CLASS_CC;
          /* NO break; */
        case GMMREG_DT_GPRS:
          /*
           * Wait untill GSM is finished
           */
          break;
        case GMMREG_DT_POWER_OFF:
          kern_local_detach( GMMCS_POWER_OFF, FALSE, /* TCS 2.1 */
                GMM_LOCAL_DETACH_PROC_POWER_OFF);
          break;
        case GMMREG_DT_SOFT_OFF:
          kern_local_detach( GMMCS_POWER_OFF, FALSE, /* TCS 2.1 */
                GMM_LOCAL_DETACH_PROC_SOFT_OFF);
          break;
       default:
          TRACE_ERROR ("Unexpected detach type in primitive GMMREG_DETACH_REQ");
          break;
      }
      break;
    case KERN_GMM_DEREG_RESUMING:
    case KERN_GMM_REG_RESUMING:
      TRACE_ERROR ("prim should be saved by pei");
      /* NO break */
    case KERN_GMM_REG_SUSPENDING:
      switch ( gmm_data->kern.detach_cap.detach_type )
      {
        case GMMREG_DT_IMSI:
        case GMMREG_DT_COMB:
        case GMMREG_DT_SIM_REMOVED:
          gmm_data->kern.suspension_type |= GMM_SUSP_IMSI_DETACH;
          break;
        case GMMREG_DT_DISABLE_GPRS:
          gmm_data->kern.attach_cap.mobile_class = GMMREG_CLASS_CC;
          /* NO break; */
        case GMMREG_DT_GPRS:
          /*
           * Wait untill GSM is finished
           */
          break;
        case GMMREG_DT_POWER_OFF:
        case GMMREG_DT_SOFT_OFF:
          SET_STATE ( KERN, KERN_GMM_REG_SUSPENDED);
#ifdef FF_EM_MODE
          EM_GMM_SET_STATE(KERN_GMM_REG_SUSPENDED);
#endif
          kern_resume_grr_reg();
          break;
        default:
          TRACE_ERROR ("Unexpected detach type in primitive GMMREG_DETACH_REQ");
          break;
      }
      break;
  
    case KERN_GMM_NULL_NO_IMSI:
    case KERN_GMM_NULL_NO_IMSI_LIMITED_SERVICE_REQ:
      switch ( gmm_data->kern.detach_cap.detach_type )
      {
        case GMMREG_DT_IMSI:
        case GMMREG_DT_COMB:
          if ( GMM_MM_DEREG != GET_STATE(MM) )
          {
            kern_mm_imsi_detach();
            break;
          }
          /* else NO break;*/
        case GMMREG_DT_SIM_REMOVED:
        case GMMREG_DT_GPRS:
        {  
          PALLOC (gmmreg_detach_cnf, GMMREG_DETACH_CNF);
            gmmreg_detach_cnf->detach_type = gmm_data->kern.detach_cap.detach_type;
          PSEND ( hCommMMI, gmmreg_detach_cnf );  
          break;
        }
        case GMMREG_DT_POWER_OFF:
          SET_STATE ( KERN, KERN_GMM_NULL_NO_IMSI );
#ifdef FF_EM_MODE
          EM_GMM_SET_STATE(KERN_GMM_NULL_NO_IMSI );
#endif
          /* NO break*/
        case GMMREG_DT_SOFT_OFF:
            kern_mm_imsi_detach_ind (GMMCS_INT_NOT_PRESENT, /* TCS 2.1 */
                  GET_STATE(MM)==GMM_MM_DEREG?
                    MMGMM_DETACH_DONE:
                    MMGMM_PERFORM_DETACH,
                    gmm_data->kern.detach_cap.detach_type);

          break;
        case GMMREG_DT_DISABLE_GPRS:
          gmm_data->kern.attach_cap.mobile_class = GMMREG_CLASS_CC;
          {  
            PALLOC (gmmreg_detach_cnf, GMMREG_DETACH_CNF);
              gmmreg_detach_cnf->detach_type = gmm_data->kern.detach_cap.detach_type;
            PSEND ( hCommMMI, gmmreg_detach_cnf );  
          }
          break;

        default:
          TRACE_ERROR ("Unexpected detach type in primitive GMMREG_DETACH_REQ");
          break;
      }
      break;
 

    case KERN_GMM_NULL_IMSI:
    case KERN_GMM_NULL_PLMN_SEARCH:
    case KERN_GMM_NULL_IMSI_LIMITED_SERVICE_REQ:
      switch ( gmm_data->kern.detach_cap.detach_type )
      {
        case GMMREG_DT_IMSI:
        case GMMREG_DT_COMB:
          if ( GMM_MM_DEREG != GET_STATE(MM) )
          {
            kern_mm_imsi_detach_ind (GMMCS_INT_NOT_PRESENT, MMGMM_PERFORM_DETACH,
                                     GMMREG_DT_SIM_REMOVED); 

            break;
          }
          /* else NO break;*/
        case GMMREG_DT_GPRS:
        {  
          PALLOC (gmmreg_detach_cnf, GMMREG_DETACH_CNF);
            gmmreg_detach_cnf->detach_type = gmm_data->kern.detach_cap.detach_type;
          PSEND ( hCommMMI, gmmreg_detach_cnf );  
          break;
        }
        case GMMREG_DT_SOFT_OFF:
          SET_STATE ( KERN, KERN_GMM_NULL_IMSI );
#ifdef FF_EM_MODE
          EM_GMM_SET_STATE(KERN_GMM_NULL_IMSI );
#endif
          kern_mm_imsi_detach_ind (GMMCS_INT_NOT_PRESENT, /* TCS 2.1 */
                  GET_STATE(MM)==GMM_MM_DEREG?
                    MMGMM_DETACH_DONE:
                    MMGMM_PERFORM_DETACH,gmm_data->kern.detach_cap.detach_type);

          break;
        case GMMREG_DT_SIM_REMOVED:
        case GMMREG_DT_POWER_OFF:
          SET_STATE ( KERN, KERN_GMM_NULL_NO_IMSI );
#ifdef FF_EM_MODE
          EM_GMM_SET_STATE(KERN_GMM_NULL_NO_IMSI );
#endif
          kern_mm_imsi_detach_ind (GMMCS_INT_NOT_PRESENT, /* TCS 2.1 */
                  GET_STATE(MM)==GMM_MM_DEREG?
                    MMGMM_DETACH_DONE:
                    MMGMM_PERFORM_DETACH,
                    gmm_data->kern.detach_cap.detach_type);

          break;
        case GMMREG_DT_DISABLE_GPRS:
          gmm_data->kern.attach_cap.mobile_class = GMMREG_CLASS_CC;
          {  
            PALLOC (gmmreg_detach_cnf, GMMREG_DETACH_CNF);
              gmmreg_detach_cnf->detach_type = gmm_data->kern.detach_cap.detach_type;
            PSEND ( hCommMMI, gmmreg_detach_cnf );  
          }
          break;
        default:
          TRACE_ERROR ("Unexpected detach type in primitive GMMREG_DETACH_REQ");
          break;
      }
      break;

    case KERN_GMM_DEREG_ATTEMPTING_TO_ATTACH:
      kern_gmmrr_attach_finished();
    
      /* NO break; */

    case KERN_GMM_DEREG_NO_IMSI:
    case KERN_GMM_DEREG_PLMN_SEARCH:
    case KERN_GMM_DEREG_LIMITED_SERVICE:
    case KERN_GMM_DEREG_NO_CELL_AVAILABLE:
    case KERN_GMM_DEREG_INITIATED:

    case KERN_GMM_DEREG_SUSPENDED:
    case KERN_GMM_DEREG_SUSPENDING:
      
      switch ( gmm_data->kern.detach_cap.detach_type )
      {
        case GMMREG_DT_IMSI:
        case GMMREG_DT_COMB:
          if ( GMM_MM_DEREG != GET_STATE(MM) )
          {
            switch ( GET_STATE( KERN ) )
            {
              case KERN_GMM_DEREG_SUSPENDING:
                gmm_data->kern.suspension_type |= GMM_SUSP_IMSI_DETACH;
                break;
              default:
                kern_imsi_detach_der_susp ();
                break;
            }
          }
          /* NO break;*/
        case GMMREG_DT_GPRS:
        {  
          PALLOC (gmmreg_detach_cnf, GMMREG_DETACH_CNF);
            gmmreg_detach_cnf->detach_type = gmm_data->kern.detach_cap.detach_type;
          PSEND ( hCommMMI, gmmreg_detach_cnf );  
          break;
        }
        case GMMREG_DT_SOFT_OFF:
          kern_local_detach( GMMCS_POWER_OFF, FALSE, GMM_LOCAL_DETACH_PROC_SOFT_OFF); /* TCS 2.1 */
          break;
        case GMMREG_DT_POWER_OFF:
          kern_local_detach( GMMCS_POWER_OFF, FALSE, GMM_LOCAL_DETACH_PROC_POWER_OFF); /* TCS 2.1 */
          break;
        case GMMREG_DT_DISABLE_GPRS:
        case GMMREG_DT_SIM_REMOVED:
          kern_local_detach( GMMCS_INT_PROTOCOL_ERROR, FALSE, /* TCS 2.1 */
            GMM_LOCAL_DETACH_PROC_ENTER_DEREG );
          break;
        default:
          TRACE_ERROR ("Unexpected detach type in primitive GMMREG_DETACH_REQ");
          break;
      }
      break;

    case KERN_GMM_RAU_INITIATED:
    case KERN_GMM_RAU_WAIT_FOR_NPDU_LIST:
      /*
       * DETACH whilst RAU
       */
      break;

    default:
      TRACE_ERROR( "GMMREG_DETACH_REQ unexpected" );
      break;
  }
  GMM_RETURN;
} /* kern_gmmreg_detach_req() */


/*
+------------------------------------------------------------------------------
| Function    : kern_gmmsms_reg_state_req
+------------------------------------------------------------------------------
| Description : Handles the primitive GMMSMS_REG_STATE_REQ
|
|          MSC: 3.21 Interaction with SMS
|
| Parameters  : *gmmsms_reg_state_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_gmmsms_reg_state_req ( T_GMMSMS_REG_STATE_REQ *gmmsms_reg_state_req )
{ 
  GMM_TRACE_FUNCTION( "kern_gmmsms_reg_state_req" );

  PFREE (gmmsms_reg_state_req);
  {
    PALLOC ( gmmsms_reg_state_cnf, GMMSMS_REG_STATE_CNF );
    gmmsms_reg_state_cnf->radio_priority_level = 
      gmm_data->kern.attach_cap.sms_radio_priority_level;
    switch( GET_STATE( KERN ) )
    {
      case KERN_GMM_NULL_NO_IMSI:
      case KERN_GMM_NULL_IMSI:
      case KERN_GMM_DEREG_NO_IMSI:
      case KERN_GMM_REG_SUSPENDED:
      case KERN_GMM_REG_RESUMING:
      case KERN_GMM_DEREG_SUSPENDED:
      case KERN_GMM_DEREG_RESUMING:
      case KERN_GMM_DEREG_PLMN_SEARCH:
      case KERN_GMM_DEREG_LIMITED_SERVICE:
      case KERN_GMM_DEREG_NO_CELL_AVAILABLE:
      case KERN_GMM_DEREG_ATTEMPTING_TO_ATTACH:
      case KERN_GMM_DEREG_INITIATED:
      case KERN_GMM_REG_INITIATED:
      case KERN_GMM_REG_ATTEMPTING_TO_UPDATE:
      case KERN_GMM_REG_NO_CELL_AVAILABLE:
      case KERN_GMM_REG_LIMITED_SERVICE:
      case KERN_GMM_REG_ATTEMPTING_TO_UPDATE_MM:
      case KERN_GMM_RAU_INITIATED:
        
        gmmsms_reg_state_cnf->reg_state = SMS_RS_DEREGISTERED;
        break;
   
      case KERN_GMM_REG_NORMAL_SERVICE:
        
        gmmsms_reg_state_cnf->reg_state = SMS_RS_REGISTERED;
        break;
      default:
        TRACE_ERROR( "GMMSMS_REG_STATE_REQ unexpected" );
        break;
    }
    PSEND(hCommSMS,gmmsms_reg_state_cnf);
  }/* PALLOC*/
  GMM_RETURN;
} /* kern_gmmsms_reg_state_req() */



/*
+------------------------------------------------------------------------------
| Function    : kern_gmmrr_page_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive GMMRR_PAGE_IND
|
| Parameters  : *gmmrr_page_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_gmmrr_page_ind ( T_GMMRR_PAGE_IND *gmmrr_page_ind )
{ 
  GMM_TRACE_FUNCTION( "kern_gmmrr_page_ind" );

  
  switch( GET_STATE( KERN ) )
  {
    /* 
     * case GMM-DEREG
     * If the MS is not GPRS attached when it receives a paging for
     * GPRS services, the MS shall ignore the paging.
     * <R.GMM.PAGNGPRS.M.17> 
     */
    case KERN_GMM_REG_NORMAL_SERVICE:
      switch ( gmmrr_page_ind->page_id )
      {
        case GMMRR_IMSI:
          TRACE_EVENT("paged with IMSI");
          PFREE ( gmmrr_page_ind );
          gmm_data->gu                        = GU2_NOT_UPDATED;
          kern_sim_del_locigprs ();
          kern_sim_gmm_update ();
          /*
           * <R.GMM.PGPIMSI.M.004>, <R.GMM.PGPIMSI.M.005>,
           * <R.GMM.PGPIMSI.M.008>
           */
          gmm_data->kern.detach_cap.detach_type = GMM_DT_RE_ATTACH;
          kern_local_detach( GMMCS_INT_PROTOCOL_ERROR, /* TCS 2.1 */
          FALSE, GMM_LOCAL_DETACH_PROC_RE_ATTACH);
          /* 
           * goto LABEL ATTACH
           *
           * <R.GMM.PGPIMSI.M.009>
           * GMM-DEREG.NORMAL-SERVICE
           *
          kern_attach_reset();
          kern_attach();
           */
          break;
        case GMMRR_PTMSI:
          TRACE_EVENT("paged with P-TMSI");
          PFREE ( gmmrr_page_ind );
        {
          /* 
           * <R.GMM.PGPPTMS.M.007> 
           */
          PALLOC (llgmm_trigger_req, LLGMM_TRIGGER_REQ);
            llgmm_trigger_req->trigger_cause = LLGMM_TRICS_PAGE_RESPONSE;
          PSEND ( hCommLLC, llgmm_trigger_req );
          break;
        }
        default:
          PFREE ( gmmrr_page_ind );
          TRACE_ERROR( "IMSI or PTMSI page ID expected" );
          break;
      }
      break;
    default:
      PFREE ( gmmrr_page_ind );
      TRACE_ERROR( "GMMRR_PAGE_IND unexpected" );
      break;
  }
  GMM_RETURN;
} /* kern_gmmrr_page_ind() */

/*
+------------------------------------------------------------------------------
| Function    : kern_gmmrr_cs_page_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive GMMRR_CS_PAGE_IND
|
| Parameters  : *gmmrr_cs_page_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_gmmrr_cs_page_ind ( T_GMMRR_CS_PAGE_IND *gmmrr_cs_page_ind )
{ 
  GMM_TRACE_FUNCTION( "kern_gmmrr_cs_page_ind" );

  PFREE ( gmmrr_cs_page_ind );

  
  {
    PALLOC ( gmmrr_cs_page_res, GMMRR_CS_PAGE_RES );

    switch (gmm_data->kern.attach_cap.mobile_class)
    {
      case GMMRR_CLASS_CG:
        gmmrr_cs_page_res->response = GMMRR_CS_PAGE_REJ;
        gmmrr_cs_page_res->susp_gprs = GMMRR_NOT_SUSP_GPRS;
        break;

/* PATCH JPS 26.10.02: with class BX, we can have BC in NMOIII */
      case GMMREG_CLASS_BG:
        if (GMMRR_NET_MODE_III==gmm_data->kern.sig_cell_info.net_mode)
        {
          gmmrr_cs_page_res->response = GMMRR_CS_PAGE_REJ;
          gmmrr_cs_page_res->susp_gprs = GMMRR_NOT_SUSP_GPRS;
          break;
        }
        /* NO break; */
      case GMMREG_CLASS_BC:
      case GMMREG_CLASS_B:
/* PATCH JPS 26.10.02 END */
        switch( GET_STATE( KERN ) )
        {
          case KERN_GMM_DEREG_RESUMING:
          case KERN_GMM_REG_RESUMING:
            TRACE_ERROR ("prim should be saved by pei");
            break;
          case KERN_GMM_DEREG_ATTEMPTING_TO_ATTACH:
            gmm_data->kern.attach_cap.enter_attempting_to_update_after_lau = TRUE;
            /* NO break;*/
          case KERN_GMM_DEREG_NO_IMSI:
          case KERN_GMM_DEREG_SUSPENDED:
          case KERN_GMM_DEREG_LIMITED_SERVICE:
          
          case KERN_GMM_DEREG_NO_CELL_AVAILABLE:
          case KERN_GMM_DEREG_PLMN_SEARCH:
          case KERN_GMM_DEREG_INITIATED:
          case KERN_GMM_REG_INITIATED:
            if (GMM_MM_DEREG != GET_STATE(MM)
            &&  MMGMM_LIMITED_SERVICE!=gmm_data->kern.sig_cell_info.mm_status
            )
            {
              gmmrr_cs_page_res->response = GMMRR_CS_PAGE_CNF;
              SET_STATE ( KERN, KERN_GMM_DEREG_SUSPENDED );
#ifdef FF_EM_MODE
              EM_GMM_SET_STATE(KERN_GMM_DEREG_SUSPENDED );
#endif
            }
            else
            {
              gmmrr_cs_page_res->response = GMMRR_CS_PAGE_REJ;
            }             
            gmmrr_cs_page_res->susp_gprs = GMMRR_NOT_SUSP_GPRS;
            break;
          case KERN_GMM_REG_SUSPENDED:
          case KERN_GMM_REG_LIMITED_SERVICE:
          case KERN_GMM_REG_NO_CELL_AVAILABLE:
            if (GMM_MM_DEREG != GET_STATE(MM)
            && MMGMM_LIMITED_SERVICE!=gmm_data->kern.sig_cell_info.mm_status)              
            {
              gmmrr_cs_page_res->response = GMMRR_CS_PAGE_CNF;
              SET_STATE ( KERN, KERN_GMM_REG_SUSPENDED );
#ifdef FF_EM_MODE
              EM_GMM_SET_STATE(KERN_GMM_REG_SUSPENDED );
#endif
              kern_llgmm_suspend(LLGMM_CALL);
            }
            else
            {
              gmmrr_cs_page_res->response = GMMRR_CS_PAGE_REJ;
            }              
            gmmrr_cs_page_res->susp_gprs = GMMRR_NOT_SUSP_GPRS;

            break;
          case KERN_GMM_REG_ATTEMPTING_TO_UPDATE:
            if (GMM_MM_DEREG != GET_STATE(MM)
            &&  MMGMM_LIMITED_SERVICE!=gmm_data->kern.sig_cell_info.mm_status)
            {
              gmmrr_cs_page_res->response = GMMRR_CS_PAGE_CNF;
              gmm_data->kern.attach_cap.enter_attempting_to_update_after_lau = TRUE;
              SET_STATE ( KERN, KERN_GMM_REG_SUSPENDED );
#ifdef FF_EM_MODE
              EM_GMM_SET_STATE(KERN_GMM_REG_SUSPENDED );
#endif
              kern_llgmm_suspend(LLGMM_CALL);
            }
            else
            {
              gmmrr_cs_page_res->response = GMMRR_CS_PAGE_REJ;
            }
            gmmrr_cs_page_res->susp_gprs = GMMRR_SUSP_GPRS;
            /*
             * kern_llgmm_unassign();
             */
            break;
          case KERN_GMM_REG_ATTEMPTING_TO_UPDATE_MM:
            /* IOT: Combined RA/LA is accepted with network cause 16
             * MSC temporarily unavailable. MM state is reset to GMM_MM_DEREG
             * From the IOT, it was observed that after GMM moves to NMOII, MM
             * was able to perform LAU successfully, but not combined RAU. Hence
             * in this state we should be able to make and receive CS calls
             * Also network is able to reach us through paging */
            if (/*GMM_MM_DEREG != GET_STATE(MM)
            && */ MMGMM_LIMITED_SERVICE!=gmm_data->kern.sig_cell_info.mm_status)
            {
              gmmrr_cs_page_res->response = GMMRR_CS_PAGE_CNF;
              gmm_data->kern.attach_cap.enter_attempting_to_update_after_lau = TRUE;
              SET_STATE ( KERN, KERN_GMM_REG_SUSPENDED );
              kern_llgmm_suspend(LLGMM_CALL);
            }
            else
            {
              gmmrr_cs_page_res->response = GMMRR_CS_PAGE_REJ;
            }
            gmmrr_cs_page_res->susp_gprs = GMMRR_SUSP_GPRS;
            /*
             * kern_llgmm_unassign();
             */
            break;
          case KERN_GMM_RAU_INITIATED:
          case KERN_GMM_REG_NORMAL_SERVICE:
            if (GMM_MM_DEREG != GET_STATE(MM)
            && MMGMM_LIMITED_SERVICE!=gmm_data->kern.sig_cell_info.mm_status)
            {
              gmmrr_cs_page_res->response = GMMRR_CS_PAGE_CNF;
              SET_STATE ( KERN, KERN_GMM_REG_SUSPENDED );
#ifdef FF_EM_MODE
              EM_GMM_SET_STATE(KERN_GMM_REG_SUSPENDED );
#endif
              kern_llgmm_suspend(LLGMM_CALL);
            }
            else
            {
              gmmrr_cs_page_res->response = GMMRR_CS_PAGE_REJ;
            }
            gmmrr_cs_page_res->susp_gprs = GMMRR_SUSP_GPRS;
            /*
             * kern_llgmm_unassign();
             */
            break;
          case KERN_GMM_NULL_IMSI:
            /*
             * in state KERN_GMM_NULL_IMSI MS is doing a GPRS detach
             * and GMM has informed MM to act as GSM only mobile but
             * before MM processes the message, GMM receives CS_PAGE_IND
             * GMM must reject the CS_PAGE as MM will take care of the
             * next CS_PAGE_IND after processing MMGMM_REG_REQ to act as
             * GSM only mobile. if GMM accepts the CS_PAGE, GRR will be
             * suspended and after coming out of dedicated mode MM will
             * not inform GMM about CS connection release and GRR will
             * not be resumed. This is to prevent such a race condition.
             */

            TRACE_EVENT("CS Call Received To GMM In GMM_NULL_IMSI state");
            gmmrr_cs_page_res->response = GMMRR_CS_PAGE_REJ;
            gmmrr_cs_page_res->susp_gprs = GMMRR_NOT_SUSP_GPRS;
            break;
          case KERN_GMM_NULL_PLMN_SEARCH:
            if (GMM_MM_DEREG != GET_STATE(MM)
            && MMGMM_LIMITED_SERVICE!=gmm_data->kern.sig_cell_info.mm_status)
            {
              gmmrr_cs_page_res->response = GMMRR_CS_PAGE_CNF;
            }
            else
            {
              gmmrr_cs_page_res->response = GMMRR_CS_PAGE_REJ;
            }
            gmmrr_cs_page_res->susp_gprs = GMMRR_NOT_SUSP_GPRS;
            break;
          default:
            if (GMM_MM_DEREG != GET_STATE(MM)
            && MMGMM_LIMITED_SERVICE!=gmm_data->kern.sig_cell_info.mm_status)
            {
              gmmrr_cs_page_res->response = GMMRR_CS_PAGE_CNF;
            }
            else
            {
              gmmrr_cs_page_res->response = GMMRR_CS_PAGE_REJ;
            }       
            gmmrr_cs_page_res->susp_gprs = GMMRR_NOT_SUSP_GPRS;
            TRACE_ERROR( "GMMRR_CS_PAGE_IND unexpected" );
            break;
        }  
        break;
      case GMMRR_CLASS_A:
      case GMMRR_CLASS_CC:
        gmmrr_cs_page_res->response = GMMRR_CS_PAGE_CNF;
        gmmrr_cs_page_res->susp_gprs = GMMRR_NOT_SUSP_GPRS; /* GMM1903A */
        break;
      default:
        TRACE_ERROR( "unknown MS class");
        break;
    }
    /* START PATCH UBUOB ISSUE 8379 */
    if (gmmrr_cs_page_res->response == GMMRR_CS_PAGE_CNF)
    {
      gmm_data->kern.gmmrr_resume_sent = FALSE;
    }
    /* END PATCH UBUOB ISSUES 8379 */
    PSEND ( hCommGRR, gmmrr_cs_page_res );
  }
  GMM_RETURN;
} /* kern_gmmrr_cs_page_ind() */

/*
+------------------------------------------------------------------------------
| Function    : kern_gmmrr_cr_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive GMMRR_CR_IND
|
| Parameters  : *gmmrr_cr_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_gmmrr_cr_ind ( T_GMMRR_CR_IND *gmmrr_cr_ind )
{     
  GMM_TRACE_FUNCTION( "kern_gmmrr_cr_ind" );

  PFREE ( gmmrr_cr_ind );
  if ( GMM_GRR_STATE_ON == gmm_data->kern.attach_cap.grr_state)
  /*
   * If GRR is suspended state should be kept.
   */
  {
    gmm_data->kern.attach_cap.grr_state = GMM_GRR_STATE_CR;
  }

  switch( GET_STATE( KERN ) )
  {
    case KERN_GMM_REG_INITIATED:
    case KERN_GMM_RAU_INITIATED:
      /*
       * store timer value to be able to restart timer after cell reselection
       */
      gmm_data->kern.attach_cap.t3310_value=0;
      if (VSI_OK == vsi_t_status ( GMM_handle , kern_T3310, &(gmm_data->kern.attach_cap.t3310_value))
      &&  gmm_data->kern.attach_cap.t3310_value>0)
      { 
        vsi_t_stop ( GMM_handle , kern_T3310);
      }
      
      TRACE_1_INFO ("TIMER t3310 = %dmsec stopped - waiting for CELL_IND",gmm_data->kern.attach_cap.t3310_value);
      break;
    default:
      break;
  }
  GMM_RETURN;
}/* kern_gmmrr_cr_ind */


/*
+------------------------------------------------------------------------------
| Function    : kern_cgrlc_status_ind (TCS 2.1)
+------------------------------------------------------------------------------
| Description : Handles the primitive CGRLC_STATUS_IND (TCS 2.1)
|
| Parameters  : *cgrlc_status_ind - Ptr to primitive payload (TCS 2.1)
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_cgrlc_status_ind ( T_CGRLC_STATUS_IND *cgrlc_status_ind ) /* TCS 2.1 */
{ 
  GMM_TRACE_FUNCTION( "kern_cgrlc_status_ind" ); /* TCS 2.1 */

  switch (cgrlc_status_ind->failure) /* TCS 2.1 */
  {
  case CGRLC_ACCESS_2_NETWORK_NOT_ALLOWED: /* TCS 2.1 */
    TRACE_EVENT ("Access to the network is not allowed");
    break;
  case CGRLC_PACKET_ACCESS_FAILURE: /* TCS 2.1 */
    TRACE_EVENT("Failure during packet access procedure, e.g. T3162 expired");
    break;
  case CGRLC_RLC_MAC_ERROR: /* TCS 2.1 */
    TRACE_EVENT("T3168 expires during contention resolution");
    break;
  case CGRLC_TLLI_MISMATCH: /* TCS 2.1 */
    TRACE_EVENT ("TLLI mismatch has occurred");
    break;
  case CGRLC_TBF_ESTABLISHMENT_FAILURE: /* TCS 2.1 */
    TRACE_EVENT ("T3164 expires or failure occurs due to any other reason");
    break;
  case CGRLC_RESUMPTION_FAILURE: /* TCS 2.1 */
    TRACE_EVENT ("resumption failure after dedicated mode was left");
    break;
  case CGRLC_CONTENTION_RESOLUTION_FAILED: /* TCS 2.1 */
    TRACE_EVENT ("Contantion Resolution has failed"); /* TCS 2.1 */
    break; /* TCS 2.1 */
  default:
    TRACE_ERROR ("unknown failure");
    break;
  }

  PFREE (cgrlc_status_ind); /* TCS 2.1 */
  
  switch( GET_STATE( KERN ) )
  {
    case KERN_GMM_REG_INITIATED:
    case KERN_GMM_RAU_INITIATED:
      /************************************************************************
       * MSC: 3.8 Attach
       *
       * MSC: 3.8.1 Normal Attach
       * MSC: 3.8.1.4 Abnormal cases
       * MSC: 3.8.1.4.2 b) Lower layer failure before the ATTACH ACCEPT or
       *                   ATTACH REJECT 
       *                   message is received
       ************************************************************************
       *
       *<R.GMM.AGABNORM.M.004>, <R.GMM.LOWERFAIL.M.001>
       */
      kern_aac( /*llgmm_status_ind->error_cause*/ 0);
      break;
    case KERN_GMM_DEREG_INITIATED:
      /************************************************************************
       * MSC: 3.9 GPRS detach procedure
       *
       * MCS: 3.9.3 Abnormal cases
       * MSC: 3.9.3.2 b) Lower layer failure
       ************************************************************************/
      kern_local_detach ( GMMCS_INT_PROTOCOL_ERROR, FALSE, GMM_LOCAL_DETACH_PROC_ENTER_DEREG); /* TCS 2.1 */
      break;
    case KERN_GMM_REG_IMSI_DETACH_INITIATED:
      /* 
       * <R.GMM.DMABNORM.M.021> 
       */
      /************************************************************************
       * MSC: 3.9 GPRS detach procedure
       *
       * MCS: 3.9.3 Abnormal cases
       * MSC: 3.9.3.2 b) Lower layer failure
       ************************************************************************/
       gmm_data->kern.detach_cap.detach_type = GMMREG_DT_IMSI;
       kern_local_detach( GMMCS_INT_PROTOCOL_ERROR, FALSE, GMM_LOCAL_DETACH_PROC_ENTER_REG_NORMAL); /* TCS 2.1 */
       break;
    case KERN_GMM_DEREG_RESUMING:
    case KERN_GMM_DEREG_SUSPENDED:
    case KERN_GMM_REG_RESUMING:
    case KERN_GMM_REG_SUSPENDED:
      TRACE_ERROR( "CGRLC_STATUS_IND unexpected" ); /* TCS 2.1 */
      break;
    default:
      break;
  }
  GMM_RETURN;
} /* kern_cgrlc_status_ind() */ /* TCS 2.1 */
/*
+------------------------------------------------------------------------------
| Function    : kern_llgmm_status_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive LLGMM_STATUS_IND
|
| Parameters  : *llgmm_status_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_llgmm_status_ind ( T_LLGMM_STATUS_IND *llgmm_status_ind )
{ 
  GMM_TRACE_FUNCTION( "kern_llgmm_status_ind" );

  PFREE (llgmm_status_ind);
  
  switch( GET_STATE( KERN ) )
  {
    case KERN_GMM_REG_INITIATED:
    case KERN_GMM_RAU_INITIATED:
      /************************************************************************
       * MSC: 3.8 Attach
       *
       * MSC: 3.8.1 Normal Attach
       * MSC: 3.8.1.4 Abnormal cases
       * MSC: 3.8.1.4.2 b) Lower layer failure before the ATTACH ACCEPT or
       *                   ATTACH REJECT 
       *                   message is received
       ************************************************************************
       *
       *<R.GMM.AGABNORM.M.004>, <R.GMM.LOWERFAIL.M.001>
       */
      kern_aac( /*llgmm_status_ind->error_cause*/ GMMCS_NO_SERVICE); /* TCS 2.1 */
      break;
    case KERN_GMM_DEREG_INITIATED:
      /************************************************************************
       * MSC: 3.9 GPRS detach procedure
       *
       * MCS: 3.9.3 Abnormal cases
       * MSC: 3.9.3.2 b) Lower layer failure
       ************************************************************************/
       kern_local_detach( GMMCS_INT_PROTOCOL_ERROR, FALSE, GMM_LOCAL_DETACH_PROC_ENTER_DEREG); /* TCS 2.1 */
       break;
    case KERN_GMM_REG_IMSI_DETACH_INITIATED:
      /* 
       * <R.GMM.DMABNORM.M.021> 
       */
      /************************************************************************
       * MSC: 3.9 GPRS detach procedure
       *
       * MCS: 3.9.3 Abnormal cases
       * MSC: 3.9.3.2 b) Lower layer failure
       ************************************************************************/
       gmm_data->kern.detach_cap.detach_type = GMMREG_DT_IMSI;
       kern_local_detach( GMMCS_INT_PROTOCOL_ERROR, FALSE, GMM_LOCAL_DETACH_PROC_ENTER_REG_NORMAL); /* TCS 2.1 */
      
       break;
    default:
      TRACE_ERROR( "LLGMM_STATUS_IND unexpected" );
      break;
  }
  GMM_RETURN;
} /* kern_llgmm_status_ind() */

/*
+------------------------------------------------------------------------------
| Function    : kern_llgmm_tlli_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive LLGMM_TLLI_IND
|
| Parameters  : *llgmm_status_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_llgmm_tlli_ind ( T_LLGMM_TLLI_IND *llgmm_tlli_ind )
{ 
  GMM_TRACE_FUNCTION( "kern_llgmm_tlli_ind" );
  TRACE_1_PARA ("TLLI: 0x%x", llgmm_tlli_ind->new_tlli);

  GMM_TRACE_GMM_DATA(GMM_DEBUG_PRINT_MASK_TLLI);
  
  switch( GET_STATE( KERN ) )
  {
    default:
    case KERN_GMM_NULL_NO_IMSI:
    case KERN_GMM_NULL_IMSI:
    case KERN_GMM_NULL_PLMN_SEARCH:
    case KERN_GMM_NULL_NO_IMSI_LIMITED_SERVICE_REQ:
    case KERN_GMM_NULL_IMSI_LIMITED_SERVICE_REQ:
      TRACE_ERROR("LLGMM_TLLI_IND unexpected");
      break;

    case KERN_GMM_DEREG_INITIATED:
    case KERN_GMM_DEREG_ATTEMPTING_TO_ATTACH:
    case KERN_GMM_DEREG_NO_CELL_AVAILABLE:
    case KERN_GMM_DEREG_LIMITED_SERVICE:
    case KERN_GMM_DEREG_NO_IMSI:
    case KERN_GMM_DEREG_PLMN_SEARCH:
    case KERN_GMM_DEREG_SUSPENDED:
    case KERN_GMM_REG_INITIATED:
    case KERN_GMM_REG_NO_CELL_AVAILABLE:
    case KERN_GMM_REG_LIMITED_SERVICE:
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE_MM:
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE:
    case KERN_GMM_REG_SUSPENDED:
    case KERN_GMM_REG_NORMAL_SERVICE:
    case KERN_GMM_RAU_INITIATED:
    case KERN_GMM_RAU_WAIT_FOR_NPDU_LIST:
    case KERN_GMM_REG_IMSI_DETACH_INITIATED:
    case KERN_GMM_REG_RESUMING:
    case KERN_GMM_DEREG_SUSPENDING:
    case KERN_GMM_DEREG_RESUMING:
    case KERN_GMM_REG_SUSPENDING:
    case KERN_GMM_REG_TEST_MODE:
    case KERN_GMM_REG_TEST_MODE_NO_IMSI:

      if ( llgmm_tlli_ind->new_tlli != gmm_data->tlli.current)
      {
         TRACE_ERROR("TLLI value not expected");
      }
      
      TRACE_0_OUT_PARA("Unassign Old PTMSI");
        
      gmm_data->tlli.old  = GMMRR_TLLI_INVALID;
      gmm_data->ptmsi.old = GMMRR_TMSI_INVALID;
  
      GMM_TRACE_GMM_DATA(GMM_DEBUG_PRINT_MASK_TLLI | GMM_DEBUG_PRINT_MASK_PTMSI);
      /*
       * LLC should know about unassigning itself
       */
      kern_gmmrr_assign();
      break;
  }

  PFREE (llgmm_tlli_ind);

  GMM_RETURN;
} /* kern_llgmm_tlli_ind() */

/*
+------------------------------------------------------------------------------
| Function    : kern_sim_gmm_insert_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive SIM_GMM_INSERT_IND
|      
|               MSC: 3.2   SIM
|
| Parameters  : *sim_gmm_insert_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_sim_gmm_insert_ind ( T_SIM_GMM_INSERT_IND *sim_gmm_insert_ind )
{ 
  GMM_TRACE_FUNCTION( "kern_sim_gmm_insert_ind" );
  /*
   * Set IMSI
   */

  kern_read_imsi ( sim_gmm_insert_ind ) ;

  GMM_TRACE_GMM_DATA(GMM_DEBUG_PRINT_MASK_IMSI);

  kern_read_loc_info (sim_gmm_insert_ind);

  GMM_TRACE_GMM_DATA(GMM_DEBUG_PRINT_MASK_CID);
  GMM_TRACE_GMM_DATA(GMM_DEBUG_PRINT_MASK_KERN_ATTACH);

  gmm_data->acc_contr_class = sim_gmm_insert_ind->acc_ctrl.acc[0] * 256 +
                               sim_gmm_insert_ind->acc_ctrl.acc[1];
  gmm_data->kern.sim_op_mode = sim_gmm_insert_ind->op_mode;
  kern_read_kc_cksn   (sim_gmm_insert_ind);

  PFREE ( sim_gmm_insert_ind );

  switch( GET_STATE( KERN ) )
  {
    case KERN_GMM_NULL_NO_IMSI:
    case KERN_GMM_NULL_NO_IMSI_LIMITED_SERVICE_REQ:
    case KERN_GMM_DEREG_NO_IMSI:
      /*
       *  MSC: 3.2   SIM
       *
       *  MSC: 3.2.1 SIM is inserted and valid
       */
      /*
       * SIM is considered as invalid until SIM removed or switching off
       */
      gmm_data->sim_gprs_invalid = FALSE;
      /*
       * reset the attach attampt counter and the rau attempt counter
       * <R.GMM.ATTACH.M.007>
       */
      gmm_data->kern.aac = 0;

      switch( GET_STATE( KERN ) )
      {
        case KERN_GMM_NULL_NO_IMSI:
          /* 
           * <R.GMM.DOTHCASE.M.001>
           * SET_STATE ( KERN, KERN_GMM_DEREG_PLMN_SEARCH );
           *
           * state GMM-DEREGISTERED.IMSI discribes the state GMM-NULL.IMSI-VALID
           */
       
          SET_STATE ( KERN, KERN_GMM_NULL_IMSI );
#ifdef FF_EM_MODE
          EM_GMM_SET_STATE(KERN_GMM_NULL_IMSI );
#endif
          break;
        case KERN_GMM_NULL_NO_IMSI_LIMITED_SERVICE_REQ:
          SET_STATE (KERN, KERN_GMM_NULL_IMSI_LIMITED_SERVICE_REQ);
#ifdef FF_EM_MODE
          EM_GMM_SET_STATE(KERN_GMM_NULL_IMSI_LIMITED_SERVICE_REQ);
#endif
          break;
        case KERN_GMM_DEREG_NO_IMSI:
          /*
           * GMM enters state NULL_IMSI beause GMM is waiting for a new attach 
           * prim from GACI (COPS=0)
           */
          SET_STATE ( KERN, KERN_GMM_NULL_IMSI );
#ifdef FF_EM_MODE
          EM_GMM_SET_STATE(KERN_GMM_NULL_IMSI );
#endif
          break;
        default:
          TRACE_ERROR( "State unexpected" );
          break;
      }
      break;
    default:
      TRACE_ERROR( "SIM_GMM_INSERT_IND unexpected" );
      break;
  }
  GMM_RETURN;
} /* kern_sim_gmm_insert_ind() */



/*
+------------------------------------------------------------------------------
| Function    : kern_sim_remove_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive SIM_REMOVE_IND
|
| Parameters  : *sim_remove_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_sim_remove_ind ( T_SIM_REMOVE_IND *sim_remove_ind )
{ 
  GMM_TRACE_FUNCTION( "kern_sim_remove_ind" );

  PFREE ( sim_remove_ind );

  gmm_data->sim_gprs_invalid = TRUE;
  gmm_data->kern.detach_cap.detach_type = GMMREG_DT_SIM_REMOVED;
  /*
   * multiple outstanding SIM_AUTHENTICATION_REQ messages reset the 
   * counter whenever SIM is removed
   */
  gmm_data->kern.auth_cap.last_auth_req_id = NOT_PRESENT_8BIT;
  
  switch( GET_STATE( KERN ) )
  {
    /*
     * MSC: 3.2 SIM
     *
     * MSC: 3.2.2. No SIM present or SIM not valid
     * MSC: 3.2.2.2 State GMM-REG
     */

    /*
     * MSC: 3.2.2.2.1 Network not reachable
     * state GMM_REG
     */
    case KERN_GMM_DEREG_RESUMING:
    case KERN_GMM_DEREG_SUSPENDED:
    case KERN_GMM_REG_RESUMING:
    case KERN_GMM_REG_SUSPENDED:
      if (GMM_GRR_STATE_SUSPENDED==gmm_data->kern.attach_cap.grr_state)
      /*
       * wait for MMGMM_CM_RELEASE_IND
       */
      {
        break;
      }
      /* NO break;*/
    case KERN_GMM_REG_LIMITED_SERVICE:
    case KERN_GMM_REG_NO_CELL_AVAILABLE:
    {
      kern_local_detach (GMMCS_SIM_REMOVED, FALSE,GMM_LOCAL_DETACH_PROC_SIM_REMOVED); /* TCS 2.1 */
      break;
    }
    case KERN_GMM_REG_TEST_MODE:  
      {
        PALLOC (cgrlc_test_end_req, CGRLC_TEST_END_REQ); /* TCS 2.1 */
        SET_STATE (KERN, KERN_GMM_REG_TEST_MODE_NO_IMSI);
#ifdef FF_EM_MODE
        EM_GMM_SET_STATE(KERN_GMM_REG_TEST_MODE_NO_IMSI);
#endif
        PSEND (hCommGRLC, cgrlc_test_end_req); /* TCS 2.1 */
       }
      break;
    /*
     * MSC: 3.2.2.2.2 Network reachable
     * state GMM_REG.NORMAL_SERVICE
     */
    case KERN_GMM_RAU_INITIATED:
    case KERN_GMM_RAU_WAIT_FOR_NPDU_LIST:
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE_MM:
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE:
    case KERN_GMM_REG_NORMAL_SERVICE:
      /*
       * <R.GMM.DSUBFANO.M.003>
       */
 
      kern_detach ();
      break;
    /*
     * MSC: 3.2 SIM
     *
     * MSC: 3.2.2. No SIM present or SIM not valid
     * MSC: 3.2.2.1 State GMM-DEREG
     */

    /*
     * state GMM_DEREG
     */
    case KERN_GMM_DEREG_PLMN_SEARCH:
    case KERN_GMM_DEREG_NO_CELL_AVAILABLE:
    case KERN_GMM_DEREG_LIMITED_SERVICE:
    case KERN_GMM_DEREG_ATTEMPTING_TO_ATTACH:
    case KERN_GMM_REG_INITIATED:
    case KERN_GMM_DEREG_INITIATED:

      kern_mm_imsi_detach_ind ( GMMCS_SIM_REMOVED, MMGMM_PERFORM_DETACH,
                                gmm_data->kern.detach_cap.detach_type); /* TCS 2.1 */
      /*
       * <R.GMM.ODNOIMSI.M.001>
       */
      SET_STATE ( KERN, KERN_GMM_DEREG_NO_IMSI );
#ifdef FF_EM_MODE
      EM_GMM_SET_STATE(KERN_GMM_DEREG_NO_IMSI );
#endif
      break;
    
    case KERN_GMM_NULL_IMSI:
    
      kern_mm_imsi_detach_ind ( GMMCS_SIM_REMOVED, MMGMM_PERFORM_DETACH,
                                gmm_data->kern.detach_cap.detach_type  );  /* TCS 2.1 */
      /*
       * <R.GMM.ODNOIMSI.M.001>
       */
      SET_STATE ( KERN, KERN_GMM_NULL_NO_IMSI );
#ifdef FF_EM_MODE
      EM_GMM_SET_STATE(KERN_GMM_NULL_NO_IMSI );
#endif
      break;

    default:
      kern_mm_imsi_detach_ind ( GMMCS_SIM_REMOVED, MMGMM_PERFORM_DETACH,
                                gmm_data->kern.detach_cap.detach_type); /* TCS 2.1 */
      TRACE_ERROR( "SIM_REMOVE_IND unexpected" );
      break;
  }
  GMM_RETURN;
} /* kern_sim_remove_ind() */


/*
+------------------------------------------------------------------------------
| Function    : kern_gmmrr_suspend_cnf
+------------------------------------------------------------------------------
| Description : Handles the primitive GMMRR_SUSPEND_CNF
|
|               GRR confirms that GRR is successful suspended and RR is
|               activated.
|
| Parameters  : *gmmrr_suspend_cnf - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_gmmrr_suspend_cnf ( T_GMMRR_SUSPEND_CNF *gmmrr_suspend_cnf )
{ 
  GMM_TRACE_FUNCTION( "kern_gmmrr_suspend_cnf" );
  PFREE(gmmrr_suspend_cnf);
  gmm_data->kern.attach_cap.grr_state = GMM_GRR_STATE_SUSPENDED;

  TRACE_1_PARA ("susp_type: %x", gmm_data->kern.suspension_type);
  switch( GET_STATE( KERN ) )
  {
    case KERN_GMM_DEREG_SUSPENDING:
      SET_STATE (KERN, KERN_GMM_DEREG_SUSPENDED);
#ifdef FF_EM_MODE
      EM_GMM_SET_STATE(KERN_GMM_DEREG_SUSPENDED);
#endif
     if(gmm_data->kern.detach_cap.detach_type EQ GMMREG_DT_GPRS)
     	{
        if ( GET_STATE(MM) != GMM_MM_REG_NORMAL_SERVICE)
            /*
                      * The state of MM is forwarded to MMI 
                      */
            {
              gmm_data->kern.detach_cap.detach_type = GMMREG_DT_COMB;  
            }
            if ( kern_lau_needed())
            {
              /* 
                          * see also kern_enter_der ()
                          */
              kern_gmmreg_detach (gmm_data->kern.detach_cap.detach_type, GMMCS_INT_NOT_PRESENT,
                                GMMREG_SEARCH_RUNNING, GMMCS_INT_NOT_PRESENT);
            }
            else
            {
              kern_gmmreg_detach (gmm_data->kern.detach_cap.detach_type, GMMCS_INT_NOT_PRESENT,
                                GMMREG_SEARCH_NOT_RUNNING,GMMCS_INT_NOT_PRESENT);
            }
     	}

      kern_call_undone_mm_proc_der();
      break;

    case KERN_GMM_REG_SUSPENDING:
      SET_STATE (KERN, KERN_GMM_REG_SUSPENDED);
#ifdef FF_EM_MODE
      EM_GMM_SET_STATE(KERN_GMM_REG_SUSPENDED);
#endif
      kern_call_undone_mm_proc_reg();
      break;

    case KERN_GMM_NULL_NO_IMSI:
      kern_local_detach ( GMMCS_POWER_OFF,    /* TCS 2.1 */
        FALSE, GMM_LOCAL_DETACH_PROC_NOTHING);
      break;
    default:
      TRACE_ERROR( "GMMRR_SUSPNED_CNF unexpected" );
      break;
  }
  GMM_RETURN;
} /* kern_gmmrr_suspend_cnf() */
/*
+------------------------------------------------------------------------------
| Function    : kern_cgrlc_test_mode_cnf (TCS 2.1)
+------------------------------------------------------------------------------
| Description : Handles the primitive GMMRR_SUSPEND_CNF
|
|               GRR confirms that the test mode is finished
|
| Parameters  : *cgrlc_test_mode_cnf - Ptr to primitive payload (TCS 2.1)
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_cgrlc_test_mode_cnf ( T_CGRLC_TEST_MODE_CNF *cgrlc_test_mode_cnf ) /* TCS 2.1 */
{ 
  GMM_TRACE_FUNCTION( "kern_cgrlc_test_mode_cnf" ); /* TCS 2.1 */
  PFREE (cgrlc_test_mode_cnf); /* TCS 2.1 */

  switch( GET_STATE( KERN ) )
  {
    case KERN_GMM_REG_TEST_MODE:
      kern_enter_reg_normal();
      break;
    case KERN_GMM_REG_TEST_MODE_NO_IMSI:
      /*
       * <R.GMM.DSUBFANO.M.003>
       */

      gmm_data->kern.detach_cap.detach_type = GMMREG_DT_SIM_REMOVED;
      kern_detach ();
      break;
    default:
      TRACE_ERROR( "CGRLC_TEST_MODE_CNF unexpected" ); /* TCS 2.1 */
      break;
  }
  GMM_RETURN;
} /* kern_cgrlc_test_mode_cnf() */ /* TCS 2.1 */

/*
+------------------------------------------------------------------------------
| Function    : kern_mmgmm_lup_needed_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive MMGMM_LUP_NEEDED_IND
|
|          MSC: 3.20.8 MMGMM LUP NEEDED IND
|
| Parameters  : *mmgmm_lup_needed_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_mmgmm_lup_needed_ind ( T_MMGMM_LUP_NEEDED_IND *mmgmm_lup_needed_ind )
{ 
  GMM_TRACE_FUNCTION( "kern_mmgmm_lup_needed_ind" );
#ifndef NTRACE
  switch (mmgmm_lup_needed_ind->reason) /* TCS 2.1 */
  {
    case MMGMM_T3212:
      TRACE_EVENT("T3212 has expired");
      break;
    case MMGMM_T_ACCEPT:
      TRACE_EVENT("T_ACCEPT has expired");
      break;
    case MMGMM_IMSI_IN_VLR:
      TRACE_EVENT("IMSI unknown in VLR received");
      break;
    case MMGMM_RXLEV_JUMP:
      TRACE_EVENT("RR indicated an increased RX level");
      break;
    case MMGMM_REG_TIMER:
      TRACE_EVENT("Registration timer in MM expired");
      break;
    default:
      TRACE_ERROR("GPRS indicator is out of range");
      break;
  }
#endif
  switch (GET_STATE( KERN ))
  {
    case KERN_GMM_DEREG_RESUMING:
    case KERN_GMM_DEREG_SUSPENDED:
    case KERN_GMM_NULL_NO_IMSI:
    case KERN_GMM_NULL_NO_IMSI_LIMITED_SERVICE_REQ:
    case KERN_GMM_NULL_IMSI:
    case KERN_GMM_NULL_IMSI_LIMITED_SERVICE_REQ:
    case KERN_GMM_DEREG_INITIATED:
    case KERN_GMM_DEREG_NO_IMSI:
    case KERN_GMM_DEREG_PLMN_SEARCH:    
    case KERN_GMM_DEREG_NO_CELL_AVAILABLE:
    case KERN_GMM_DEREG_ATTEMPTING_TO_ATTACH:
    case KERN_GMM_DEREG_LIMITED_SERVICE:
    case KERN_GMM_DEREG_SUSPENDING:
    case KERN_GMM_REG_NO_CELL_AVAILABLE:
      break;
    default:
      if( GMMRR_NET_MODE_I==gmm_data->kern.sig_cell_info.net_mode
      &&  MMGMM_T3212==mmgmm_lup_needed_ind->reason) /* TCS 2.1 */
      /*
       * <R.GMM.MMGMMONE.M.002>, <R.GMM.MMGMMONE.M.003>
       */
      {
        PFREE (mmgmm_lup_needed_ind);
        GMM_RETURN;  
      }
      break;
  }


  switch( GET_STATE( KERN ) )
  {
    case KERN_GMM_NULL_NO_IMSI:
    case KERN_GMM_NULL_NO_IMSI_LIMITED_SERVICE_REQ:
    case KERN_GMM_NULL_IMSI:
    case KERN_GMM_NULL_IMSI_LIMITED_SERVICE_REQ:
      /*
       * After deregistration MM will receive mmgmm_reg_req
       * automatically
       */
      break;
    case KERN_GMM_DEREG_NO_IMSI:
      /* PLU also if #7 */
      /*NO break;*/
    case KERN_GMM_DEREG_PLMN_SEARCH:    

    case KERN_GMM_DEREG_NO_CELL_AVAILABLE:
    case KERN_GMM_DEREG_LIMITED_SERVICE:
    case KERN_GMM_REG_INITIATED:
      SET_STATE ( KERN, KERN_GMM_DEREG_SUSPENDING );
#ifdef FF_EM_MODE
      EM_GMM_SET_STATE(KERN_GMM_DEREG_SUSPENDING );
#endif
      kern_gmmrr_suspend (GMMRR_NOT_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_LAU);  
      break;

    case KERN_GMM_DEREG_ATTEMPTING_TO_ATTACH:
      gmm_data->kern.attach_cap.enter_attempting_to_update_after_lau = TRUE;
      SET_STATE ( KERN, KERN_GMM_DEREG_SUSPENDING );
#ifdef FF_EM_MODE
      EM_GMM_SET_STATE(KERN_GMM_DEREG_SUSPENDING );
#endif
      kern_gmmrr_suspend (GMMRR_NOT_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_LAU);  
      break;
    case KERN_GMM_DEREG_INITIATED:
    case KERN_GMM_DEREG_RESUMING:      
    case KERN_GMM_DEREG_SUSPENDED:
    case KERN_GMM_REG_SUSPENDED:
    case KERN_GMM_REG_RESUMING:
    case KERN_GMM_RAU_INITIATED:
    case KERN_GMM_RAU_WAIT_FOR_NPDU_LIST:
    case KERN_GMM_REG_IMSI_DETACH_INITIATED:
      SET_STATE(MM, GMM_MM_REG_UPDATE_NEEDED);
      break;
      
    case KERN_GMM_REG_SUSPENDING:
    case KERN_GMM_DEREG_SUSPENDING:
      gmm_data->kern.suspension_type |= GMM_SUSP_LAU;
      break;

    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE:
      gmm_data->kern.attach_cap.enter_attempting_to_update_after_lau=TRUE;
      SET_STATE ( KERN, KERN_GMM_REG_SUSPENDING );
#ifdef FF_EM_MODE
      EM_GMM_SET_STATE(KERN_GMM_REG_SUSPENDING );
#endif
      kern_llgmm_suspend(LLGMM_CALL);
      kern_gmmrr_suspend (GMMRR_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_LAU);  
      break;
    case KERN_GMM_REG_LIMITED_SERVICE:
    case KERN_GMM_REG_NO_CELL_AVAILABLE:
      if(gmm_data->kern.gmmrr_resume_sent)
      /*
       * GMM is waiting for GMMRR_CELL_IND. So I rememmber to attach MM with
       * seting mm_imsi_attached to update_needed
       */
      {
        SET_STATE(MM,GMM_MM_REG_UPDATE_NEEDED);        
      }
      else
      {
        SET_STATE ( KERN, KERN_GMM_REG_SUSPENDING );
#ifdef FF_EM_MODE
        EM_GMM_SET_STATE(KERN_GMM_REG_SUSPENDING );
#endif
        kern_llgmm_suspend(LLGMM_CALL);
        kern_gmmrr_suspend (GMMRR_NOT_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_LAU);  
      }
      break;
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE_MM:
    case KERN_GMM_REG_NORMAL_SERVICE:
      if( GMMRR_NET_MODE_I==gmm_data->kern.sig_cell_info.net_mode)
      {
        kern_rau();
      }
      else
      {
        SET_STATE ( KERN, KERN_GMM_REG_SUSPENDING );
#ifdef FF_EM_MODE
        EM_GMM_SET_STATE(KERN_GMM_REG_SUSPENDING );
#endif
        kern_llgmm_suspend(LLGMM_CALL);
        kern_gmmrr_suspend (GMMRR_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_LAU);  
      }
      break;
    case KERN_GMM_REG_TEST_MODE_NO_IMSI:
    case KERN_GMM_REG_TEST_MODE:
      break;
    default:
      TRACE_ERROR ("Unexpexcted state");   
      break;
  }
  
  PFREE (mmgmm_lup_needed_ind);
  GMM_RETURN;  
} /* kern_mmgmm_lup_needed_ind() */


/*
+------------------------------------------------------------------------------
| Function    : kern_mmgmm_lup_accept_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive MMGMM_LUP_ACCEPT_IND
|
|          MSC: 3.20.3 IMSI attach/detach
|
| Parameters  : *mmgmm_lup_accept_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_mmgmm_lup_accept_ind ( T_MMGMM_LUP_ACCEPT_IND *mmgmm_lup_accept_ind )
{ 
  GMM_TRACE_FUNCTION( "kern_mmgmm_lup_accept_ind" );
 
  switch( GET_STATE( KERN ) )
  {
    case KERN_GMM_NULL_PLMN_SEARCH:
    case KERN_GMM_NULL_IMSI_LIMITED_SERVICE_REQ:
    case KERN_GMM_NULL_NO_IMSI_LIMITED_SERVICE_REQ:
    case KERN_GMM_NULL_NO_IMSI:
    
    case KERN_GMM_NULL_IMSI:
    case KERN_GMM_DEREG_NO_IMSI:
    case KERN_GMM_DEREG_LIMITED_SERVICE:
    case KERN_GMM_DEREG_ATTEMPTING_TO_ATTACH:
    case KERN_GMM_DEREG_NO_CELL_AVAILABLE:
    case KERN_GMM_DEREG_PLMN_SEARCH:
    case KERN_GMM_REG_INITIATED:

    case KERN_GMM_DEREG_RESUMING:
    case KERN_GMM_DEREG_SUSPENDED:
    case KERN_GMM_DEREG_SUSPENDING:
      if ( GMMRR_NET_MODE_III==gmm_data->kern.sig_cell_info.net_mode
      && GMMREG_CLASS_BC==  gmm_data->kern.attach_cap.mobile_class )
      {
        kern_mmgmmreg_attach_cnf (GMMREG_AT_IMSI, SEARCH_NOT_RUNNING, (T_MMGMM_REG_CNF*)mmgmm_lup_accept_ind);
      }
      else
      {
        kern_mmgmmreg_attach_cnf (GMMREG_AT_IMSI, SEARCH_RUNNING, (T_MMGMM_REG_CNF*)mmgmm_lup_accept_ind);
      }
      break;                                      
    case KERN_GMM_REG_NORMAL_SERVICE:
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE:
    case KERN_GMM_REG_NO_CELL_AVAILABLE:
    case KERN_GMM_REG_LIMITED_SERVICE:
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE_MM:
    case KERN_GMM_DEREG_INITIATED:
    case KERN_GMM_RAU_INITIATED:

    case KERN_GMM_REG_RESUMING:
    case KERN_GMM_REG_SUSPENDED:
      kern_mmgmmreg_attach_cnf (GMMREG_AT_COMB, SEARCH_RUNNING, (T_MMGMM_REG_CNF*)mmgmm_lup_accept_ind);
      break;
    default:
      TRACE_ERROR ("Unexpexcted state");   
      break;
  }
  PFREE ( mmgmm_lup_accept_ind );
  GMM_RETURN;  
} /* kern_mmgmm_lup_accept_ind() */
/*
+------------------------------------------------------------------------------
| Function    : kern_mmgmm_lup_accept_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive MMGMM_INFO_IND
|
|          MSC: 3.20.6 MM Information
|
| Parameters  : *mmgmm_info_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_mmgmm_info_ind ( T_MMGMM_INFO_IND *mmgmm_info_ind )
{ 
  GMM_TRACE_FUNCTION( "kern_mmgmm_info_ind" );
  {
    PPASS (mmgmm_info_ind, gmmreg_info_ind, GMMREG_INFO_IND);
    PSEND ( hCommMMI, gmmreg_info_ind );
  }  
  GMM_RETURN;
} /* kern_mmgmm_info_ind() */

/*
+------------------------------------------------------------------------------
| Function    : kern_sim_authentication_cnf
+------------------------------------------------------------------------------
| Description : The function kern_sim_authentication_cnf
|       
| Parameters  : *sim_authentication_cnf - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_sim_authentication_cnf ( T_SIM_AUTHENTICATION_CNF *
                                        sim_authentication_cnf )
{ 
  GMM_TRACE_FUNCTION( "kern_sim_authentication_cnf" );
  
  switch( GET_STATE( KERN ) )
  {
    case KERN_GMM_REG_NORMAL_SERVICE:
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE:
    case KERN_GMM_REG_NO_CELL_AVAILABLE: 
    case KERN_GMM_REG_LIMITED_SERVICE: 
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE_MM:
    case KERN_GMM_REG_INITIATED:
    case KERN_GMM_DEREG_INITIATED:
    case KERN_GMM_DEREG_ATTEMPTING_TO_ATTACH:
    case KERN_GMM_RAU_INITIATED:
    case KERN_GMM_REG_IMSI_DETACH_INITIATED: 
      /*
       * PATCH ubuob 19.7.02: multiple outstanding AUTHENTICATION REQ messages
       * reduce the count of outstanding requests
       */

      /*
       * only use this SIM answer if there are no other outstanding SIM_AUTHENTICATION_REQ messages!
       */

      if (gmm_data->kern.auth_cap.last_auth_req_id == sim_authentication_cnf->req_id)


      if (gmm_data->kern.auth_cap.last_auth_req_id == sim_authentication_cnf->req_id)
      {
         MCAST (authentication_and_ciphering_response,
          AUTHENTICATION_AND_CIPHERING_RESPONSE);
            authentication_and_ciphering_response->a_c_reference_number
            .a_c_reference_number_value 
            = gmm_data->kern.auth_cap.a_c_reference_number;
        
          authentication_and_ciphering_response
           ->v_authentication_parameter_sres = TRUE;
          authentication_and_ciphering_response
            ->authentication_parameter_sres.sres_value.l_sres_value = MAX_SRES*8;
          authentication_and_ciphering_response
            ->authentication_parameter_sres.sres_value.o_sres_value = 0;
          memcpy (authentication_and_ciphering_response->
            authentication_parameter_sres.sres_value.b_sres_value, 
            sim_authentication_cnf->sres, MAX_SRES);  
        
          memcpy ( gmm_data->kern.auth_cap.kc, sim_authentication_cnf->kc,
            MAX_KC ) ;
        
#ifdef GMM_TCS4
        gmm_data->kern.auth_cap.last_auth_req_id = NOT_PRESENT_8BIT;
#endif
          
        PFREE (sim_authentication_cnf);
        /*
         * <R.GMM.AUTH_RES.M.006>
         */
        {
          PALLOC ( llgmm_assign_req, LLGMM_ASSIGN_REQ );
            llgmm_assign_req->old_tlli = LLGMM_TLLI_INVALID;
            llgmm_assign_req->new_tlli = gmm_data->tlli.current;
            if (NO_KEY== gmm_data->kern.auth_cap.cksn)
            {
              memset (llgmm_assign_req->llgmm_kc.key, 0x0, MAX_KC);
            }
            else
            {
              memcpy ( llgmm_assign_req->llgmm_kc.key, gmm_data->kern.auth_cap.kc, MAX_KC );
            }
            llgmm_assign_req->ciphering_algorithm 
              = gmm_data->kern.auth_cap.ciphering_algorithm;
      
          PSEND ( hCommLLC, llgmm_assign_req );
        }
        
        
        if (IMEISV_REQUESTED == gmm_data->kern.auth_cap.imeisv_requested)
        {
          authentication_and_ciphering_response->v_gmobile_identity = TRUE;
          kern_get_imeisv (
            &authentication_and_ciphering_response->gmobile_identity);
        }
        else
        {
          authentication_and_ciphering_response->v_gmobile_identity = FALSE;     
        }
        sig_kern_tx_data_req (CURRENT_TLLI,AUTHENTICATION_AND_CIPHERING_RESPONSE);
      }
      else
      {  
        PFREE (sim_authentication_cnf);
         /* 
          * mutliple  authentication request problem occurred!!!!
          * ignore this SIM answer because another request has already been sent to
          * the SIM again and this overwrote the A&C reference number
          */
         TRACE_0_INFO ("error: Authentication received twice, SIM ignored");
      }
      /* end patch ubuob */
      break;

    default:

      PFREE (sim_authentication_cnf);
      TRACE_1_INFO ("state: %x", GET_STATE( KERN ));
      TRACE_ERROR("A&C RES unexpected");
      break;
  }
  GMM_RETURN;    
} /* kern_sim_authentication_cnf() */


/*
+------------------------------------------------------------------------------
| Function    : kern_mmgmm_nreg_cnf
+------------------------------------------------------------------------------
| Description : Handles the primitive MMGMM_NREG_CNF
|
|          MSC: 3.20.3 IMSI attach/detach
|
| Parameters  : *mmgmm_nreg_cnf - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_mmgmm_nreg_cnf ( T_MMGMM_NREG_CNF *mmgmm_nreg_cnf )
{ 
  UBYTE detach_type;
  GMM_TRACE_FUNCTION( "kern_mmgmm_nreg_cnf" );

  if (gmm_data->kern.attach_cap.mobile_class_changed
  && GMM_GRR_STATE_ON == gmm_data->kern.attach_cap.grr_state)
  {
    gmm_data->kern.attach_cap.mobile_class_changed=FALSE;
    kern_gmmrr_enable();
  }

  switch (mmgmm_nreg_cnf->detach_cause) /* TCS 2.1 */
  {
    case CS_SIM_REM:
      detach_type = GMMREG_DT_SIM_REMOVED;
      TRACE_EVENT ("SIM is removed");
      break;
    case CS_POW_OFF:
      detach_type = GMMREG_DT_POWER_OFF;
      TRACE_EVENT ("Power is switched off");
      break;
    case CS_SOFT_OFF:
      detach_type = GMMREG_DT_SOFT_OFF;
      TRACE_EVENT ("Power is soft switched off");
      break;      
    case CS_DISABLE:
      detach_type = GMMREG_DT_IMSI;
      TRACE_EVENT ("GSM switched off");
      break;
    default:
      detach_type = GMMREG_DT_POWER_OFF;
      TRACE_EVENT ("unknown detach type in MMGMM_NREG_CNF");
      break;
  }
  SET_STATE( MM, GMM_MM_DEREG );
  sig_kern_sync_set_mm_state(MMGMM_CELL_SELECTED);

  if (gmm_data->sim_gprs_invalid && gmm_data->kern.cell_id.lac  == GMMREG_LA_INVALID && mmgmm_nreg_cnf->detach_cause == CS_SIM_REM)
  {
      switch ( GET_STATE (KERN) )
      {
          case KERN_GMM_DEREG_PLMN_SEARCH:
          case KERN_GMM_DEREG_SUSPENDED:
             SET_STATE(KERN,KERN_GMM_DEREG_NO_IMSI);
#ifdef FF_EM_MODE
             EM_GMM_SET_STATE(KERN_GMM_DEREG_NO_IMSI);
#endif
             break;
          case KERN_GMM_REG_IMSI_DETACH_INITIATED:
             kern_enter_reg_normal();
             break;
          case KERN_GMM_DEREG_NO_IMSI:
             SET_STATE(KERN,KERN_GMM_NULL_NO_IMSI);
#ifdef FF_EM_MODE
             EM_GMM_SET_STATE(KERN_GMM_NULL_NO_IMSI);
#endif
             break;
          default:
              break;
      }
      PFREE ( mmgmm_nreg_cnf );
      GMM_RETURN;
  }
  switch ( GET_STATE (KERN) )
  {
    case KERN_GMM_DEREG_PLMN_SEARCH:
      switch(mmgmm_nreg_cnf->detach_cause) /* TCS 2.1 */
      {
        case CS_SOFT_OFF:
        case CS_POW_OFF:
          kern_gmmreg_detach (GMMREG_DT_POWER_OFF,
                              GMMCS_POWER_OFF, /* TCS 2.1 */
                              SEARCH_NOT_RUNNING,
                              GMMCS_POWER_OFF);
          break;
        case CS_SIM_REM:
          SET_STATE(KERN,KERN_GMM_DEREG_NO_IMSI);
#ifdef FF_EM_MODE
          EM_GMM_SET_STATE(KERN_GMM_DEREG_NO_IMSI);
#endif
          kern_gmmreg_detach (GMMREG_DT_SIM_REMOVED,
                              GMMCS_SIM_REMOVED, /* TCS 2.1 */
                              SEARCH_NOT_RUNNING,
                              GMMCS_SIM_REMOVED);
          break;
        default:
          kern_gmmreg_detach (GMMREG_DT_COMB,
                              /* START UBUOB PATCH ISSUE 8381 */
                              GMMCS_NET_CAUSE_NOT_PRESENT, /* TCS 2.1 */
                              /* END UBUOB PATCH ISSUE 8381 */
                              SEARCH_RUNNING,
                              GMMCS_NET_CAUSE_NOT_PRESENT);
          kern_mm_activate_rr();
          break;
      }

      break;

    case KERN_GMM_NULL_NO_IMSI:
    case KERN_GMM_NULL_IMSI:
      /*
       * POWER OFF
       */
      if( CS_DISABLE==mmgmm_nreg_cnf->detach_cause) /* TCS 2.1 */
      {
        detach_type=GMMREG_DT_IMSI;
      }
      kern_gmmreg_detach (detach_type,
                              gmm_data->kern.detach_cap.error_cause, /* TCS 2.1 */
                              SEARCH_NOT_RUNNING,
                              gmm_data->kern.detach_cap.error_cause);                          
          
      break;
    case KERN_GMM_DEREG_SUSPENDED:
      switch(mmgmm_nreg_cnf->detach_cause) /* TCS 2.1 */
      {
        case CS_SOFT_OFF:
          kern_gmmreg_detach (GMMREG_DT_SOFT_OFF,
                              GMMCS_POWER_OFF, /* TCS 2.1 */
                              SEARCH_NOT_RUNNING,
                              GMMCS_POWER_OFF);
          break;
        case CS_POW_OFF:
          kern_gmmreg_detach (GMMREG_DT_POWER_OFF,
                              GMMCS_POWER_OFF, /* TCS 2.1 */
                              SEARCH_NOT_RUNNING,
                              GMMCS_POWER_OFF);
          break;
        case CS_SIM_REM:
          SET_STATE(KERN,KERN_GMM_DEREG_NO_IMSI);
#ifdef FF_EM_MODE
          EM_GMM_SET_STATE(KERN_GMM_DEREG_NO_IMSI);
#endif
          kern_gmmreg_detach (GMMREG_DT_SIM_REMOVED,
                              GMMCS_SIM_REMOVED, /* TCS 2.1 */
                              SEARCH_NOT_RUNNING,
                              GMMCS_SIM_REMOVED);
          break;
        default:
          /* ACI can not handle SEARCH RUNNING to show GSM detached */
          if (GMMRR_NET_MODE_III==gmm_data->kern.sig_cell_info.net_mode
          && GMMREG_CLASS_BG==  gmm_data->kern.attach_cap.mobile_class )
          {
            kern_gmmreg_detach (GMMREG_DT_COMB,
                              GMMCS_NET_CAUSE_NOT_PRESENT, /* TCS 2.1 */
                              SEARCH_RUNNING,
                              GMMCS_NET_CAUSE_NOT_PRESENT);
          }
          else
          { 
            kern_gmmreg_detach (GMMREG_DT_COMB,
                              GMMCS_NET_CAUSE_NOT_PRESENT, /* TCS 2.1 */
                              SEARCH_NOT_RUNNING,
                              GMMCS_NET_CAUSE_NOT_PRESENT);
          }
          break;
      }
      kern_call_undone_mm_proc_der();
      break;
    case KERN_GMM_REG_SUSPENDED:
      switch(mmgmm_nreg_cnf->detach_cause) /* TCS 2.1 */
      {
        case CS_SOFT_OFF:
             kern_gmmreg_detach (GMMREG_DT_SOFT_OFF,
                              GMMCS_POWER_OFF, /* TCS 2.1 */
                              SEARCH_NOT_RUNNING,
                              GMMCS_POWER_OFF);
          break;
        case CS_POW_OFF:
          kern_gmmreg_detach (GMMREG_DT_POWER_OFF,
                              GMMCS_POWER_OFF, /* TCS 2.1 */
                              SEARCH_NOT_RUNNING,
                              GMMCS_POWER_OFF);
          break;
        case CS_DISABLE:
          if (GMMREG_DT_IMSI == gmm_data->kern.detach_cap.detach_type)
          {
             kern_gmmreg_detach (GMMREG_DT_IMSI,
                              GMMCS_NET_CAUSE_NOT_PRESENT, /* TCS 2.1 */
                              SEARCH_NOT_RUNNING,
                              GMMCS_NET_CAUSE_NOT_PRESENT);
             break;
          }
          /*
           * else: wait for gprs detach to be finished
           * FALLTHROUGH
           */
        case CS_SIM_REM:
          if (GMMRR_NET_MODE_III!=gmm_data->kern.sig_cell_info.net_mode)
          {

            /*
             * Wait until GPRS is deregistered
             */
            break;
          }
          /*
           * else NO break;
           * we are ms class BG and have switched off GSM
           */
        default:
          kern_gmmreg_detach (GMMREG_DT_IMSI,
                              GMMCS_NET_CAUSE_NOT_PRESENT, /* TCS 2.1 */
                              SEARCH_NOT_RUNNING,
                              GMMCS_NET_CAUSE_NOT_PRESENT);
          break;
      }
      kern_call_undone_mm_proc_reg();
      break;
    case KERN_GMM_REG_IMSI_DETACH_INITIATED:
       kern_enter_reg_normal();
      if(GMMRR_NET_MODE_I!=gmm_data->kern.sig_cell_info.net_mode)
      /*
       * In NMO I gmmreg_detach_cnf was sent before requesting 
       * mmgmm_net_req
       */
      {
        kern_gmmreg_detach (detach_type,
                            GMMCS_NET_CAUSE_NOT_PRESENT, /* TCS 2.1 */
                            GMMREG_SEARCH_NOT_RUNNING,
                            GMMCS_NET_CAUSE_NOT_PRESENT);
      }
      break;
    case KERN_GMM_REG_NORMAL_SERVICE:
      break;
    case KERN_GMM_DEREG_NO_IMSI:
      SET_STATE(KERN,KERN_GMM_NULL_NO_IMSI);
#ifdef FF_EM_MODE
      EM_GMM_SET_STATE(KERN_GMM_NULL_NO_IMSI);
#endif
      kern_gmmreg_detach (detach_type,
                      GMMCS_INT_NOT_PRESENT, /* TCS 2.1 */
                      GMMREG_SEARCH_NOT_RUNNING,
                      GMMCS_INT_NOT_PRESENT);            
      break;
    default:
        TRACE_ERROR( "MMGMM_NREG_CNF unexpected" );
      break;
  } 
  PFREE ( mmgmm_nreg_cnf );

#ifdef TRACE_FUNC  
#ifdef IDENTATION
  if (gmm_data->deep<=0) {
    /*
     * after power off deep is set to 0 in kern_init so we need here normal 
     * return
     */
    return;
  } 
  else
  {
    GMM_RETURN;
  }
#endif
#endif
} /* kern_mmgmm_nreg_cnf() */

/*
+------------------------------------------------------------------------------
| Function    : kern_mmgmm_auth_rej_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive MMGMM_AUTH_REJ_IND
|
| Parameters  : *mmgmm_auth_rej_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_mmgmm_auth_rej_ind ( T_MMGMM_AUTH_REJ_IND *mmgmm_auth_rej_ind )
{ 
  GMM_TRACE_FUNCTION( "kern_mmgmm_auth_rej_ind" );
  PFREE (mmgmm_auth_rej_ind);

  gmm_data->sim_gprs_invalid = TRUE;
  gmm_data->kern.detach_cap.detach_type = GMMREG_DT_SIM_REMOVED;
  kern_sim_del_locigprs ();
  gmm_data->gu  = GU3_ROAMING_NOT_ALLOWED;
  kern_sim_gmm_update ();
  
  GMM_RETURN;
} /* kern_mmgmm_auth_rej_ind() */

/*
+------------------------------------------------------------------------------
| Function    : kern_mmgmm_t3212_val_ind
|
| Description : Handles the primitive MMGMM_T3212_VAL  _IND
|
| Parameters  : *mmgmm_t3212_val_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_mmgmm_t3212_val_ind ( T_MMGMM_T3212_VAL_IND *mmgmm_t3212_val_ind )
{ 
  GMM_TRACE_FUNCTION( "kern_mmgmm_t3212_val_ind" );

#ifdef REL99
  switch (gmm_data->kern.sig_cell_info.sgsnr_flag)/*!gmm_data->release99 =>old code*/ /* TCS 4.0 */
  {
    default:
    case PS_SGSN_UNKNOWN:
    case PS_SGSN_98_OLDER:
#endif

    /*
     * see GSM04.08 ch. 11.2.2 NOTE 4
     */
    gmm_data->kern.t3302_val = mmgmm_t3212_val_ind->t3212_val;
    /*
     * Boeser Hack wegen Faulheit, signal should be taken
     */
    gmm_data->sync.mm_cell_info.t3212_val = gmm_data->kern.t3302_val;

#ifdef REL99
      break;
    case PS_SGSN_99_ONWARDS:
      break;
  }
#endif

  PFREE ( mmgmm_t3212_val_ind );
  GMM_RETURN;
} /* kern_mmgmm_t3212_val_ind() */

/*
+------------------------------------------------------------------------------
| Function    : kern_mmgmm_cm_establish_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive MMGMM_CM_ESTABLISH_IND
|
| Parameters  : *mmgmm_cm_establish_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_mmgmm_cm_establish_ind ( T_MMGMM_CM_ESTABLISH_IND *mmgmm_cm_establish_ind )
{ 
  GMM_TRACE_FUNCTION( "kern_mmgmm_cm_establish_ind" );

  PFREE ( mmgmm_cm_establish_ind );

  if (gmm_data->kern.gmmrr_resume_sent)
  {
    kern_mm_cm_establish_res (MMGMM_ESTABLISH_REJECT);
    GMM_RETURN;
  }
    
  switch( GET_STATE( KERN ) )
  {
    case KERN_GMM_DEREG_RESUMING:
    case KERN_GMM_REG_RESUMING:
      TRACE_ERROR ("prim should be saved");
      break;
    case KERN_GMM_DEREG_SUSPENDING:
    case KERN_GMM_REG_SUSPENDING:
      gmm_data->kern.suspension_type |= GMM_SUSP_CALL;
      break;

    case KERN_GMM_DEREG_SUSPENDED:
    case KERN_GMM_NULL_IMSI:
      kern_mm_cm_establish_res (MMGMM_ESTABLISH_OK);
      break;
    case KERN_GMM_DEREG_LIMITED_SERVICE:
    case KERN_GMM_DEREG_ATTEMPTING_TO_ATTACH:
    case KERN_GMM_DEREG_NO_CELL_AVAILABLE:
    case KERN_GMM_DEREG_PLMN_SEARCH:
    case KERN_GMM_DEREG_INITIATED:
    case KERN_GMM_REG_INITIATED:
      switch (gmm_data->kern.attach_cap.mobile_class)
      {
        case GMMREG_CLASS_A:
          if (GMMRR_NET_MODE_III == gmm_data->kern.sig_cell_info.net_mode)
          {
             kern_mm_cm_establish_res (MMGMM_ESTABLISH_REJECT);
          }
          else
          {
            kern_mm_cm_establish_res (MMGMM_ESTABLISH_OK);
          }
          break;
        case GMMREG_CLASS_CG:
          kern_mm_cm_establish_res (MMGMM_ESTABLISH_REJECT);
          break; 
        case GMMREG_CLASS_BG:
          if (GMMRR_NET_MODE_III == gmm_data->kern.sig_cell_info.net_mode)
          {
            kern_mm_cm_establish_res (MMGMM_ESTABLISH_REJECT);
            break;
          }
          /* NO break */
        case GMMREG_CLASS_B:
        case GMMREG_CLASS_BC:
          /* START PATCH UBUOB ISSUE 8278 */
          switch (GET_STATE (KERN))
          {
            case KERN_GMM_DEREG_ATTEMPTING_TO_ATTACH:
              gmm_data->kern.attach_cap.enter_attempting_to_update_after_lau = TRUE;
              break;
            default:
              break;
          }
          /* END PATCH UBUOB ISSUE 8278 */

          if (GMM_MM_DEREG != GET_STATE(MM))
          {
            SET_STATE ( KERN, KERN_GMM_DEREG_SUSPENDING );
#ifdef FF_EM_MODE
            EM_GMM_SET_STATE(KERN_GMM_DEREG_SUSPENDING );
#endif
            /* START PATCH UBUOB ISSUE 8781 */
            /* kern_gmmreg_suspend (GMMREG_CS_CALL); */
            /* END PATCH UBUOB ISSUE 8781 */
            kern_gmmrr_suspend (GMMRR_NOT_SUSP_GPRS, GMMRR_ABNORMAL_RELEASE, GMM_SUSP_CALL);
            vsi_t_stop(GMM_handle, kern_T3321);
          }
          else
          {
            SET_STATE( KERN, KERN_GMM_DEREG_SUSPENDING);
#ifdef FF_EM_MODE
            EM_GMM_SET_STATE(KERN_GMM_DEREG_SUSPENDING);
#endif
            /*
             * kern_llgmm_unassign();
             */
            kern_llgmm_suspend(LLGMM_CALL);
            kern_gmmrr_suspend (GMMRR_NOT_SUSP_GPRS, GMMRR_ABNORMAL_RELEASE, GMM_SUSP_CALL);
          }
          break;  
        default:
          TRACE_ERROR( "unknown MS class");
          break;
      }
      break;
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE:
    case KERN_GMM_REG_NO_CELL_AVAILABLE:
    case KERN_GMM_REG_LIMITED_SERVICE:
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE_MM:
    case KERN_GMM_RAU_INITIATED:
      switch (gmm_data->kern.attach_cap.mobile_class)
      {
        case GMMREG_CLASS_A:
          kern_mm_cm_establish_res (MMGMM_ESTABLISH_OK);
          break;
        case GMMREG_CLASS_CG:
          kern_mm_cm_establish_res (MMGMM_ESTABLISH_REJECT);
          break; 
        case GMMREG_CLASS_BG:
          if (GMMRR_NET_MODE_III == gmm_data->kern.sig_cell_info.net_mode)
          {
            kern_mm_cm_establish_res (MMGMM_ESTABLISH_REJECT);
            break;
          }
          /* NO break; */      
        case GMMREG_CLASS_B:
        case GMMREG_CLASS_BC:
          /* START PATCH UBUOB ISSUE 8278 */
          switch (GET_STATE (KERN))
          {
            case KERN_GMM_REG_ATTEMPTING_TO_UPDATE:
              gmm_data->kern.attach_cap.enter_attempting_to_update_after_lau = TRUE;
              break;
            default:
              break;
          }
          /* END PATCH UBUOB ISSUE 8278 */
          SET_STATE ( KERN, KERN_GMM_REG_SUSPENDING );
#ifdef FF_EM_MODE
          EM_GMM_SET_STATE(KERN_GMM_REG_SUSPENDING );
#endif
          kern_llgmm_suspend(LLGMM_CALL);
          kern_gmmrr_suspend (GMMRR_NOT_SUSP_GPRS, GMMRR_ABNORMAL_RELEASE, GMM_SUSP_CALL);
          break;
        default:
          TRACE_ERROR( "unknown MS class");
          break;
      }  
      break;

    case KERN_GMM_REG_NORMAL_SERVICE:
      switch (gmm_data->kern.attach_cap.mobile_class)
      {
        case GMMREG_CLASS_A:
          kern_mm_cm_establish_res (MMGMM_ESTABLISH_OK);
          break;
        case GMMREG_CLASS_CG:
          kern_mm_cm_establish_res (MMGMM_ESTABLISH_REJECT);
          break; 
        case GMMREG_CLASS_BG:
          if (GMMRR_NET_MODE_III == gmm_data->kern.sig_cell_info.net_mode)
          {
            kern_mm_cm_establish_res (MMGMM_ESTABLISH_REJECT);
            break;
          }
          /* NO break; */        
        case GMMREG_CLASS_B:
        case GMMREG_CLASS_BC:
          SET_STATE ( KERN, KERN_GMM_REG_SUSPENDING );
#ifdef FF_EM_MODE
          EM_GMM_SET_STATE(KERN_GMM_REG_SUSPENDING );
#endif
          kern_llgmm_suspend(LLGMM_CALL);
          kern_gmmrr_suspend (GMMRR_SUSP_GPRS, GMMRR_ABNORMAL_RELEASE, GMM_SUSP_CALL);
          break;
        default:
          TRACE_ERROR( "unknown MS class");
          break;
      }  
      break;
    case KERN_GMM_REG_SUSPENDED:
      switch (gmm_data->kern.attach_cap.mobile_class)
      {
        case GMMREG_CLASS_A:
          kern_mm_cm_establish_res (MMGMM_ESTABLISH_OK);
          break;
        case GMMREG_CLASS_CG:
          kern_mm_cm_establish_res (MMGMM_ESTABLISH_REJECT);
          break; 
        case GMMREG_CLASS_BG:
          if (GMMRR_NET_MODE_III == gmm_data->kern.sig_cell_info.net_mode)
          {
            kern_mm_cm_establish_res (MMGMM_ESTABLISH_REJECT);
            break;
          }
          /* NO break; */        
        case GMMREG_CLASS_B:
        case GMMREG_CLASS_BC:
          /*
           * call is rejected because MM may establish the call by itself.
           * When state REG_RESUMING will be defined than GMM will return OK.
           * In case of waiting for GMMRR_CELL_IND after GMMRR_RESUME_REQ
           * the mobile will run into a dead lock situation otherwise.
           */
          kern_mm_cm_establish_res (MMGMM_ESTABLISH_REJECT);
          break;
        default:
          TRACE_ERROR( "unknown MS class");
          break;
      }  
      break;
    case KERN_GMM_DEREG_NO_IMSI:
    case KERN_GMM_NULL_NO_IMSI:
      kern_mm_cm_establish_res (MMGMM_ESTABLISH_REJECT);
      break;
    default:
      kern_mm_cm_establish_res (MMGMM_ESTABLISH_REJECT);
      TRACE_1_INFO ("state: %x", GET_STATE( KERN ));
      TRACE_ERROR( "MMGMM_CM_ESTABLISH_IND unexpected" );
      break;
  }
  GMM_RETURN;
} /* kern_mm_cm_establish_ind() */
/*
+------------------------------------------------------------------------------
| Function    : kern_mmgmm_cm_emergency_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive MMGMM_CM_EMERGENCY_IND
|
| Parameters  : *mmgmm_cm_emergency_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_mmgmm_cm_emergency_ind ( T_MMGMM_CM_EMERGENCY_IND *mmgmm_cm_emergency_ind )
{ 
  GMM_TRACE_FUNCTION( "kern_mmgmm_cm_emeregncy_ind" );

  PFREE ( mmgmm_cm_emergency_ind );

  if (GMMREG_CLASS_A == gmm_data->kern.attach_cap.mobile_class
  && GMMRR_NET_MODE_III != gmm_data->kern.sig_cell_info.net_mode)
  {
     kern_mm_cm_emergency_res (MMGMM_ESTABLISH_OK);
    GMM_RETURN;
  }
  
  switch( GET_STATE( KERN ) )
  {
    case KERN_GMM_DEREG_RESUMING:
    case KERN_GMM_REG_RESUMING:
      TRACE_ERROR ("prim should be saved");
      break;

    case KERN_GMM_DEREG_SUSPENDING:
    case KERN_GMM_REG_SUSPENDING:
      gmm_data->kern.suspension_type |= GMM_SUSP_EM_CALL;
      break;
    case KERN_GMM_DEREG_SUSPENDED:
    case KERN_GMM_REG_SUSPENDED:
    case KERN_GMM_NULL_IMSI:
    case KERN_GMM_NULL_NO_IMSI:
    case KERN_GMM_NULL_PLMN_SEARCH:
    case KERN_GMM_NULL_NO_IMSI_LIMITED_SERVICE_REQ:
    case KERN_GMM_NULL_IMSI_LIMITED_SERVICE_REQ:
      kern_mm_cm_emergency_res (MMGMM_ESTABLISH_OK);
      break;
    case KERN_GMM_DEREG_NO_IMSI:
      if (gmm_data->kern.detach_cap.gmmreg)
      /* 
       * The mobile is switching off this moment. Thera are no power anymore
       */
      {
        kern_mm_cm_emergency_res (MMGMM_ESTABLISH_REJECT);
      }
      break;

    case KERN_GMM_DEREG_PLMN_SEARCH:
    case KERN_GMM_DEREG_LIMITED_SERVICE:
    case KERN_GMM_DEREG_ATTEMPTING_TO_ATTACH:
    case KERN_GMM_DEREG_NO_CELL_AVAILABLE:
    case KERN_GMM_DEREG_INITIATED:
    case KERN_GMM_REG_INITIATED:
      SET_STATE ( KERN, KERN_GMM_DEREG_SUSPENDING );
#ifdef FF_EM_MODE
      EM_GMM_SET_STATE(KERN_GMM_DEREG_SUSPENDING );
#endif
      kern_llgmm_suspend(LLGMM_CALL);
      kern_gmmrr_suspend (GMMRR_NOT_SUSP_GPRS, GMMRR_ABNORMAL_RELEASE, GMM_SUSP_EM_CALL);
      break;
    case KERN_GMM_RAU_WAIT_FOR_NPDU_LIST:
    case KERN_GMM_REG_IMSI_DETACH_INITIATED:
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE:
    case KERN_GMM_REG_NO_CELL_AVAILABLE:
    case KERN_GMM_REG_LIMITED_SERVICE:
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE_MM:
    case KERN_GMM_RAU_INITIATED:
      SET_STATE ( KERN, KERN_GMM_REG_SUSPENDING);
#ifdef FF_EM_MODE
      EM_GMM_SET_STATE(KERN_GMM_REG_SUSPENDING);
#endif
      kern_llgmm_suspend(LLGMM_CALL);
      kern_gmmrr_suspend (GMMRR_NOT_SUSP_GPRS, GMMRR_ABNORMAL_RELEASE, GMM_SUSP_EM_CALL);
      break;
    case KERN_GMM_REG_NORMAL_SERVICE:
      SET_STATE ( KERN, KERN_GMM_REG_SUSPENDING );
#ifdef FF_EM_MODE
      EM_GMM_SET_STATE(KERN_GMM_REG_SUSPENDING );
#endif
      /*
       * kern_llgmm_unassign();
       */
      kern_llgmm_suspend(LLGMM_CALL);
      kern_gmmrr_suspend (GMMRR_SUSP_GPRS, GMMRR_ABNORMAL_RELEASE, GMM_SUSP_EM_CALL);
      break;
    default:
      TRACE_ERROR( "MMGMM_CM_EMERGENCY_IND unexpected" );
      break;
  }
  GMM_RETURN;
} /* kern_mm_cm_emergency_ind() */
/*
+------------------------------------------------------------------------------
| Function    : kern_mmgmm_cm_release_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive MMGMM_CM_RELEASE_IND
|
| Parameters  : *mmgmm_cm_release_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_mmgmm_cm_release_ind ( T_MMGMM_CM_RELEASE_IND *mmgmm_cm_release_ind )
{ 
  GMM_TRACE_FUNCTION( "kern_mmgmm_cm_release_ind" );

  switch( GET_STATE( KERN ) )
  {
    case KERN_GMM_REG_SUSPENDED:
      /*
       * resume with cell_ind
       */
      if (MMGMM_RESUMPTION_OK!=mmgmm_cm_release_ind->resumption)
      {
        TRACE_EVENT("Para: resumption failure: RAU needed");
        SET_STATE(GU,GU_UPDATE_NEEDED);
      }
      else
      {
        TRACE_EVENT("OPar: resumption OK.");
      }
      PFREE ( mmgmm_cm_release_ind );
      kern_call_undone_mm_proc_reg();
      break;
    case KERN_GMM_DEREG_SUSPENDED:
      PFREE ( mmgmm_cm_release_ind );
      kern_call_undone_mm_proc_der();
      break;
    case KERN_GMM_DEREG_SUSPENDING:
    case KERN_GMM_REG_SUSPENDING:
      gmm_data->kern.suspension_type &= ~ (GMM_SUSP_CALL|GMM_SUSP_EM_CALL);

      PFREE ( mmgmm_cm_release_ind );
      break;
    
    default:

      if (GMMREG_CLASS_A!=gmm_data->kern.attach_cap.mobile_class)
      {
        TRACE_ERROR( "MMGMM_CM_RELEASE_IND unexpected" );
      }    
      PFREE ( mmgmm_cm_release_ind );
      break;
  }
  GMM_RETURN;
} /* kern_mm_cm_release_ind() */

/*
+------------------------------------------------------------------------------
| Function    : kern_mmgmm_plmn_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive MMGMM_PLMN_IND
|
|          MSC: 3.20 MM Interface
|          MSC: 3.20.2 Network selection
|
| Parameters  : *mmgmm_plmn_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_mmgmm_plmn_ind ( T_MMGMM_PLMN_IND *mmgmm_plmn_ind )
{ 
  GMM_TRACE_FUNCTION( "kern_mmgmm_plmn_ind" );
  {
    PPASS (mmgmm_plmn_ind, gmmreg_plmn_ind, GMMREG_PLMN_IND);
    PSEND ( hCommMMI, gmmreg_plmn_ind );
  }
  GMM_RETURN;
} /* kern_mm_plmn_ind() */


/*
+------------------------------------------------------------------------------
| Function    : kern_gmmreg_attach_req
+------------------------------------------------------------------------------
| Description : Handles the the primitive GMMREG_ATTACH_REQ
|
|               This signal passed the attach_type from MMI to 
|               the service KERN
|
|         MSC : 3.4 MMI-initiated normal/combined GPRS attach
|
| Parameters  : *gmmreq_attach_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_gmmreg_attach_req ( T_GMMREG_ATTACH_REQ *gmmreg_attach_req ) 
{ 
  U8 attach_type_changed = FALSE;
  GMM_TRACE_FUNCTION( "kern_gmmreg_attach_req" );
  TRACE_EVENT ("hp_gmm_24473#1#24473"); /* TCS 2.1 */

  /* Changes for Boot Time Speedup. GMM will get dummy GMMREG_ATTACH_REQ indicating QUICK_REG. 
   * GMM will send MMGMM_REG_REQ indicating QUICK_REG. No need to process this request as it is 
   * dummy request and GMM will get other GMMREG_ATTACH_REQ indicating NORMAL_REG
   */
  if (gmmreg_attach_req->bootup_act EQ QUICK_REG)
  {
    kern_mm_reg_req (REG_GPRS_INACTIVE, QUICK_REG);
    return ;
  }
  vsi_t_stop  ( GMM_handle, kern_T3302);
  sig_kern_rdy_stop_t3302_req();
  gmm_data->kern.attach_cap.enter_attempting_to_update_after_lau = FALSE;
  /*
   * attach_cap.gmmreg sets the caller of the attach procedure to MMI
   */
  gmm_data->kern.attach_cap.gmmreg = TRUE;
  gmm_data->kern.detach_cap.gmmreg = FALSE;

  gmm_data->kern.attach_cap.service_mode = gmmreg_attach_req->service_mode;
  

  if (SERVICE_MODE_LIMITED == gmm_data->kern.attach_cap.service_mode)
  {
    /*
     * I expect that limited service means emergency call over GSM only
     * CLASS_CC can be set, because service mode is changed via 
     * GMMREG_ATTACH_REQ only, so that the wanted mobile class is giveb again
     */
    gmm_data->kern.attach_cap.mobile_class = GMMREG_CLASS_CC;    
    gmm_data->kern.attach_cap.attach_type = GMMREG_AT_IMSI;

  }
  else
  {
     /*
      *PATCH TB 26/10/02 <
      * Inform GRR of the new MS class
      */
     attach_type_changed = 
        (  gmm_data->kern.attach_cap.mobile_class != gmmreg_attach_req->mobile_class
        || gmm_data->kern.attach_cap.attach_type!= gmmreg_attach_req->attach_type);
     
     if (GMM_GRR_STATE_OFF != gmm_data->kern.attach_cap.grr_state
     && (gmm_data->kern.attach_cap.mobile_class != gmmreg_attach_req->mobile_class))
     {
       if (GMMRR_NET_MODE_III==gmm_data->kern.sig_cell_info.net_mode)
       {
          gmm_data->kern.attach_cap.mobile_class_changed = TRUE;
          gmm_data->kern.attach_cap.mobile_class = gmmreg_attach_req->mobile_class;
          /*
           * inform GRR after switch (mmgmm_nreg_cnf xor kern _enter_der)
           * kern_gmmrr_enable();
           */
       }
       else
       {
          gmm_data->kern.attach_cap.mobile_class = gmmreg_attach_req->mobile_class;
          kern_gmmrr_enable();
       }
     }     
     /* PATCH TB 26/10/02 > */


     gmm_data->kern.attach_cap.mobile_class = gmmreg_attach_req->mobile_class;
      /*
       * if MMI wants to detach, then detach is expected or class change
       */
     gmm_data->kern.attach_cap.attach_type = 
       (GMMREG_CLASS_CG == gmmreg_attach_req->mobile_class?
        GMMREG_AT_GPRS:
        gmm_data->kern.attach_cap.attach_type|gmmreg_attach_req->attach_type);

  }
   

  
  #ifndef NTRACE      
  switch (gmmreg_attach_req->service_mode)
  {
    case SERVICE_MODE_LIMITED:
      TRACE_EVENT ("Limited Service");
      break;
    case SERVICE_MODE_FULL:
      TRACE_EVENT ("Full Service");
      break;
    default:
      TRACE_ERROR ("wrong Service requested");
  }
  switch ( gmm_data->kern.attach_cap.mobile_class )
  {
    case GMMREG_CLASS_CC:
      TRACE_0_PARA("MS class CC");
      break;
    case GMMREG_CLASS_CG:
      TRACE_0_PARA("MS class CG");
      break;
    case GMMREG_CLASS_B:
      TRACE_0_PARA("MS class B");
      break;
    case GMMREG_CLASS_BG:
      TRACE_0_PARA("MS class BG");
      break;
    case GMMREG_CLASS_BC:
      TRACE_0_PARA("MS class BC");
      break;
    case GMMREG_CLASS_A:
      TRACE_0_PARA("MS class A");
      break;
    default:
      TRACE_ERROR ("unknown mobile class");
      break;
  }
  switch (gmm_data->kern.attach_cap.attach_type)
  {
    case GMMREG_AT_IMSI:
      TRACE_0_PARA("GSM only attach");
      break;
    case GMMREG_AT_GPRS:
      TRACE_0_PARA("GPRS attach");
      break;
    case GMMREG_AT_COMB:
      TRACE_0_PARA("GSM and GPRS attach");
      break;
    default:
      TRACE_ERROR ("unknown attach type");
      TRACE_1_PARA("%d",gmm_data->kern.attach_cap.attach_type);
      break;
  }
#endif

  if (gmm_data->sim_gprs_invalid
  && GMMREG_CLASS_CG == gmm_data->kern.attach_cap.mobile_class)
  /* 
   * i.e. error cause 7: gprs not allowed
   */
  {
    PREUSE(gmmreg_attach_req,gmmreg_attach_rej,GMMREG_ATTACH_REJ );
      gmmreg_attach_rej->detach_type = 
        (GMM_MM_REG_NORMAL_SERVICE == GET_STATE(MM)?GMMREG_DT_GPRS:GMMREG_DT_COMB),
      gmmreg_attach_rej->search_running = GMMREG_SEARCH_NOT_RUNNING,
      gmmreg_attach_rej->cause = GMMCS_GPRS_NOT_ALLOWED; /* TCS 2.1 */
      gmmreg_attach_rej->service = GMMCS_GPRS_NOT_ALLOWED;
      gmm_data->kern.attach_cap.gmmreg = FALSE;
      
    PSEND ( hCommMMI, gmmreg_attach_rej );
    GMM_RETURN;
  }
 
  
  PFREE ( gmmreg_attach_req );
  
  switch( GET_STATE( KERN ) )
  {
    /*
     * GMM-DEREGISTERED
     */
    case KERN_GMM_DEREG_RESUMING:
    case KERN_GMM_REG_RESUMING:
      TRACE_ERROR ("prim should be saved");
      break;
 
    case KERN_GMM_DEREG_PLMN_SEARCH:
    case KERN_GMM_DEREG_LIMITED_SERVICE:
    case KERN_GMM_DEREG_NO_CELL_AVAILABLE:
    case KERN_GMM_DEREG_ATTEMPTING_TO_ATTACH:
    
    case KERN_GMM_DEREG_SUSPENDING:
    case KERN_GMM_DEREG_SUSPENDED:
      if (SERVICE_MODE_LIMITED == gmm_data->kern.attach_cap.service_mode)
      {
        SET_STATE ( KERN, KERN_GMM_NULL_IMSI_LIMITED_SERVICE_REQ);
#ifdef FF_EM_MODE
        EM_GMM_SET_STATE(KERN_GMM_NULL_IMSI_LIMITED_SERVICE_REQ);
#endif
        kern_gmmrr_disable();
        kern_mm_activate_mm();
        break;
      }

      if (GMMREG_CLASS_CC==gmm_data->kern.attach_cap.mobile_class
      || GMMREG_AT_IMSI== gmm_data->kern.attach_cap.attach_type)
      {
        SET_STATE ( KERN, KERN_GMM_NULL_IMSI );
#ifdef FF_EM_MODE
        EM_GMM_SET_STATE(KERN_GMM_NULL_IMSI );
#endif
        kern_gmmrr_disable();
        kern_mm_activate_mm();
        if ( GMM_MM_REG_NORMAL_SERVICE == GET_STATE(MM) )
        {
          kern_gmmreg_attach_cnf( GMMREG_AT_IMSI);
        }
        break;
      }

      switch ( gmm_data->kern.attach_cap.mobile_class )
      {
        case GMMREG_CLASS_CG:
          if ( GMM_MM_DEREG != GET_STATE(MM) )
          {
            switch( GET_STATE( KERN ) )
            {
              case KERN_GMM_DEREG_SUSPENDED:
                /*
                 * Case GMMREG_CLASS_CG.
                 * The detach procedure for cs is only possible after receiving
                 * GMMRR_CELL_IND
                 */
                break;
              case KERN_GMM_DEREG_SUSPENDING:
                gmm_data->kern.suspension_type |=GMM_SUSP_IMSI_DETACH;
                break;
              default:
                if (kern_lai_changed() )
                {
                  SET_STATE ( KERN, KERN_GMM_DEREG_SUSPENDING );
#ifdef FF_EM_MODE
                  EM_GMM_SET_STATE(KERN_GMM_DEREG_SUSPENDING );
#endif
                  kern_gmmrr_suspend (GMMRR_NOT_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_LAU);
                }
                else 
                {
                  kern_imsi_detach_der_susp();
                }
                GMM_RETURN;
            }
          }
          break;
        default:
          break;
      }

      switch( GET_STATE( KERN ) )
      {
        case KERN_GMM_DEREG_SUSPENDED:
          gmm_data->kern.attach_cap.mm_lau_attempted=TRUE;
          gmm_data->kern.suspension_type &= ~GMM_SUSP_LAU;
          break;
        case KERN_GMM_DEREG_SUSPENDING:
          gmm_data->kern.suspension_type |=GMM_SUSP_LAU;
          break;
        default:          
          {
            kern_mm_reg_req (REG_CELL_SEARCH_ONLY, NORMAL_REG);
          }
          break;
          
      }
      break;
    case KERN_GMM_NULL_PLMN_SEARCH:
      if (SERVICE_MODE_LIMITED == gmm_data->kern.attach_cap.service_mode)
      {
        SET_STATE (KERN,KERN_GMM_NULL_IMSI_LIMITED_SERVICE_REQ);
#ifdef FF_EM_MODE
        EM_GMM_SET_STATE(KERN_GMM_NULL_IMSI_LIMITED_SERVICE_REQ);
#endif
        kern_mm_activate_mm ();
        break;
      }

      if (GMMREG_CLASS_CC==gmm_data->kern.attach_cap.mobile_class
      || GMMREG_AT_IMSI== gmm_data->kern.attach_cap.attach_type)
      {
        kern_mm_activate_mm ();
        break;
      }


      switch ( gmm_data->kern.attach_cap.mobile_class )
      {
        case GMMREG_CLASS_CG:
          if ( GMM_MM_DEREG != GET_STATE(MM) )
          {
            SET_STATE(KERN, KERN_GMM_DEREG_PLMN_SEARCH);
#ifdef FF_EM_MODE
            EM_GMM_SET_STATE(KERN_GMM_DEREG_PLMN_SEARCH);
#endif
            /*
             * MM enters autmaticly gprs cell search mode so that GMM 
             * need not to send mmgmm_reg_req (cell_search_only)
             */
            kern_mm_imsi_detach();
          }
          else
          {
            SET_STATE ( KERN, KERN_GMM_DEREG_PLMN_SEARCH );
#ifdef FF_EM_MODE
            EM_GMM_SET_STATE(KERN_GMM_DEREG_PLMN_SEARCH );
#endif
            kern_mm_activate_rr();
          }
          break;
        default:
          /*
           * class A and B
           */
          SET_STATE ( KERN, KERN_GMM_DEREG_PLMN_SEARCH );
#ifdef FF_EM_MODE
          EM_GMM_SET_STATE(KERN_GMM_DEREG_PLMN_SEARCH );
#endif
          kern_mm_activate_rr();
        break;
      }
      break;
    /*
     * GPRS will be enabled
     */
    case KERN_GMM_NULL_IMSI:          
      if (SERVICE_MODE_LIMITED == gmm_data->kern.attach_cap.service_mode)
      {
        SET_STATE (KERN,KERN_GMM_NULL_IMSI_LIMITED_SERVICE_REQ);
#ifdef FF_EM_MODE
        EM_GMM_SET_STATE(KERN_GMM_NULL_IMSI_LIMITED_SERVICE_REQ);
#endif
        kern_mm_activate_mm ();
        break;
      }
      if (GMM_MM_DEREG != GET_STATE(MM) 
      && GMMREG_CLASS_CC == gmm_data->kern.attach_cap.mobile_class)
      {
        TRACE_EVENT ("GMMREG_ATTACH_REQ has been come twice and is ignored!");
        /*
         * the funcion is not alled because mm_plmn is used.
         */
       
        kern_gmmreg_attach_cnf(GMMREG_AT_IMSI);
        gmm_data->kern.attach_cap.gmmreg = FALSE;

       break;
      }
      /* NO break */
    case KERN_GMM_NULL_IMSI_LIMITED_SERVICE_REQ:           
      if (SERVICE_MODE_LIMITED != gmm_data->kern.attach_cap.service_mode)
      {
        gmm_data->kern.sig_cell_info.gmm_status = GMMRR_SERVICE_UNKNOWN ; /* Initalize gmm status value */
        switch ( gmm_data->kern.attach_cap.mobile_class )
        {
          case GMMREG_CLASS_CC:
            if (KERN_GMM_NULL_IMSI_LIMITED_SERVICE_REQ==GET_STATE(KERN)
            || GMM_MM_DEREG == GET_STATE(MM))
            {
              SET_STATE ( KERN, KERN_GMM_NULL_PLMN_SEARCH );
#ifdef FF_EM_MODE
              EM_GMM_SET_STATE(KERN_GMM_NULL_PLMN_SEARCH );
#endif
              kern_mm_activate_mm();
            }
            else
            {
              SET_STATE(KERN,KERN_GMM_NULL_IMSI);
#ifdef FF_EM_MODE
              EM_GMM_SET_STATE(KERN_GMM_NULL_IMSI);
#endif
              kern_gmmreg_attach_cnf( GMMREG_AT_IMSI);
            }
            break;
          case GMMREG_CLASS_CG:
            if ( GMM_MM_DEREG != GET_STATE(MM) )
            {
              switch(GET_STATE(KERN))
              {
                case KERN_GMM_NULL_IMSI:
                  SET_STATE ( KERN, KERN_GMM_DEREG_PLMN_SEARCH);
#ifdef FF_EM_MODE
                  EM_GMM_SET_STATE(KERN_GMM_DEREG_PLMN_SEARCH);
#endif
                  /*
                   * MM enters autmaticly gprs cell search mode so that GMM 
                   * need not to send mmgmm_reg_req (cell_search_only)
                   */
                  kern_mm_imsi_detach();
                  break;
                default:
                  break;
              }
            }
            else
            {
              SET_STATE ( KERN, KERN_GMM_DEREG_PLMN_SEARCH );
#ifdef FF_EM_MODE
              EM_GMM_SET_STATE(KERN_GMM_DEREG_PLMN_SEARCH );
#endif
              kern_mm_activate_rr();
            }
            break;
          default:
            /*
             * class A and B
             */
            if (GMMREG_AT_IMSI== gmm_data->kern.attach_cap.attach_type)
            {
              SET_STATE ( KERN, KERN_GMM_NULL_PLMN_SEARCH );
#ifdef FF_EM_MODE
              EM_GMM_SET_STATE(KERN_GMM_NULL_PLMN_SEARCH );
#endif
              kern_mm_activate_mm();
            }
            else
            {
              SET_STATE ( KERN, KERN_GMM_DEREG_PLMN_SEARCH );
#ifdef FF_EM_MODE
              EM_GMM_SET_STATE(KERN_GMM_DEREG_PLMN_SEARCH );
#endif

              kern_mm_activate_rr();
            }
          break;
        }
      }  
      break;
    /* end case KERN_GMM_NULL_IMSI */

    case KERN_GMM_DEREG_NO_IMSI:
      /*
       * this state is not necessary, it is the same as NULL_NO_IMSI
       * and will be romoved soon
       */

      if (SERVICE_MODE_LIMITED == gmm_data->kern.attach_cap.service_mode)
      {
        SET_STATE (KERN,KERN_GMM_NULL_NO_IMSI_LIMITED_SERVICE_REQ);
#ifdef FF_EM_MODE
        EM_GMM_SET_STATE(KERN_GMM_NULL_NO_IMSI_LIMITED_SERVICE_REQ);
#endif
      }
      else
      {
        SET_STATE (KERN,KERN_GMM_NULL_NO_IMSI);
#ifdef FF_EM_MODE
        EM_GMM_SET_STATE(KERN_GMM_NULL_NO_IMSI);
#endif
      }
      kern_gmmrr_disable();
      kern_mm_activate_mm(); 
      break;
     
    case KERN_GMM_NULL_NO_IMSI:
      /*
       * Attach Rejected for the first time to enable POWER_OFF
       */
      if (SERVICE_MODE_LIMITED == gmm_data->kern.attach_cap.service_mode)
      {
        SET_STATE (KERN,KERN_GMM_NULL_NO_IMSI_LIMITED_SERVICE_REQ);
#ifdef FF_EM_MODE
        EM_GMM_SET_STATE(KERN_GMM_NULL_NO_IMSI_LIMITED_SERVICE_REQ);
#endif
      }
      kern_mm_activate_mm();
      break;
    case KERN_GMM_NULL_NO_IMSI_LIMITED_SERVICE_REQ:
      if ( SERVICE_MODE_LIMITED != gmm_data->kern.attach_cap.service_mode)
      {
        SET_STATE (KERN,KERN_GMM_NULL_NO_IMSI);
#ifdef FF_EM_MODE
        EM_GMM_SET_STATE(KERN_GMM_NULL_NO_IMSI);
#endif
      }
      kern_mm_activate_mm();
      break;
    case KERN_GMM_REG_NORMAL_SERVICE:
      if (SERVICE_MODE_LIMITED == gmm_data->kern.attach_cap.service_mode)
      {
        kern_l_sim_rem_reg();
      }
      else
      {
        switch ( gmm_data->kern.attach_cap.mobile_class )
        {
          case GMMREG_CLASS_CC:
            if ( GMM_MM_REG_NORMAL_SERVICE == GET_STATE(MM) )
            {
              kern_gmmreg_attach_cnf ( GMMREG_AT_IMSI );
            }
            kern_disable();
            break;
          case GMMREG_CLASS_CG:
            if (GMM_MM_DEREG != GET_STATE(MM))
            {
              if (kern_lai_changed() )
              {
                switch (gmm_data->kern.sig_cell_info.net_mode)
                {
                  case GMMRR_NET_MODE_I:
                    kern_rau();
                    break;
                  default:
                    SET_STATE ( KERN, KERN_GMM_REG_SUSPENDING );
#ifdef FF_EM_MODE
                    EM_GMM_SET_STATE(KERN_GMM_REG_SUSPENDING );
#endif
                    kern_llgmm_suspend(LLGMM_CALL);
                    kern_gmmrr_suspend (GMMRR_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_LAU);
                    break;
                }
              }
              else 
              {
                switch (gmm_data->kern.sig_cell_info.net_mode)
                {
                  case GMMRR_NET_MODE_I:
                    gmm_data->kern.detach_cap.detach_type = GMMREG_DT_IMSI;
                    kern_detach();
                    break;
                  default:
                    kern_imsi_detach_reg_susp();
                    break;
                }
              }
              GMM_RETURN;
            }
            /*
             * <R.GMM.AGACCEPT.M.027>,
             * <R.GMM.PATTCNF.M.001>
             */
            else
            {
              kern_gmmreg_attach_cnf ( GMMREG_AT_GPRS );
            }
            break;
          default:
            if (attach_type_changed)
            {
              /*
               * class A and B
               */
              if ( GMM_MM_REG_NORMAL_SERVICE == GET_STATE(MM) )
              {
                kern_gmmreg_attach_cnf ( GMMREG_AT_COMB );
              }
              else
              {
                /*
                 * Combined RAU procedure is established
                 * if GPRS is attached and IMSI should attached
                 */
              
                kern_rau();
              }
            }
            else
            {
              kern_mm_reg_req (REG_CELL_SEARCH_ONLY, NORMAL_REG);
            }
            break;
        }
      }
      break;
    case KERN_GMM_REG_SUSPENDED:
    case KERN_GMM_REG_SUSPENDING:
      if (SERVICE_MODE_LIMITED == gmm_data->kern.attach_cap.service_mode)
      {
        kern_l_sim_rem_reg_lim ();
        break;
      }
      /*
       * Attach requests in suspended mode will be handled after resumption
       * by triggering form GRR with GMMRR_CELL_IND
       */
      break;
    
    case KERN_GMM_REG_NO_CELL_AVAILABLE:
    case KERN_GMM_REG_LIMITED_SERVICE:
      if (SERVICE_MODE_LIMITED == gmm_data->kern.attach_cap.service_mode)
      {
        kern_l_sim_rem_reg_lim ();
        break;
      }
      /* NO break */
    case KERN_GMM_RAU_INITIATED:
    case KERN_GMM_REG_INITIATED:
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE_MM:
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE:  
    
      if (SERVICE_MODE_LIMITED == gmm_data->kern.attach_cap.service_mode)
      {
        kern_l_sim_rem_reg ();
      }
      else
      {
        switch ( gmm_data->kern.attach_cap.mobile_class )
        {
          case GMMREG_CLASS_CC:

            gmm_data->kern.detach_cap.detach_type = GMMREG_DT_DISABLE_GPRS;

            kern_local_detach( GMMCS_INT_PROTOCOL_ERROR, FALSE, GMM_LOCAL_DETACH_PROC_ENTER_NULL_IMSI); /* TCS 2.1 */
            if ( GMM_MM_REG_NORMAL_SERVICE == GET_STATE(MM) )
            {
              kern_gmmreg_attach_cnf ( GMMREG_AT_IMSI );
            }
            kern_enter_der();
            break;
          case GMMREG_CLASS_CG:
            if (GMM_MM_DEREG != GET_STATE(MM))
            {
              if (kern_lai_changed() )
              {
                switch(GET_STATE(KERN))
                {
                  case KERN_GMM_REG_NO_CELL_AVAILABLE:
                  case KERN_GMM_RAU_INITIATED:
                  case KERN_GMM_REG_ATTEMPTING_TO_UPDATE_MM:
                  case KERN_GMM_REG_ATTEMPTING_TO_UPDATE:  
                    kern_llgmm_suspend(LLGMM_CALL);
                    break;
                  default:
                    break;
                }

                SET_STATE ( KERN, KERN_GMM_REG_SUSPENDING );
#ifdef FF_EM_MODE
                EM_GMM_SET_STATE(KERN_GMM_REG_SUSPENDING );
#endif

                
                kern_gmmrr_suspend (GMMRR_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_LAU);
              }
              else 
              {
                kern_imsi_detach_reg_susp();
              }
              GMM_RETURN;
            }
            else
            /*
             * <R.GMM.AGACCEPT.M.027>,
             * <R.GMM.PATTCNF.M.001>
             */
            {
              if (KERN_GMM_REG_INITIATED!=GET_STATE(KERN))
              {
                kern_gmmreg_attach_cnf ( GMMREG_AT_GPRS );
              }
            }
            break;
          default:
            
            {
              kern_mm_reg_req (REG_CELL_SEARCH_ONLY, NORMAL_REG);
            }
            break;
          }
        }
      break;  
    default:
      TRACE_ERROR( "GMMREG_ATTACH_REQ unexpected" );
      break;
     
  }
  GMM_RETURN;
} /* kern_gmmreg_attach_req() */


/*
+------------------------------------------------------------------------------
| Function    : kern_gmmsm_sequence_res
+------------------------------------------------------------------------------
| Description : Handles the primitive GMMSM_SEQENCE_RES
|
| Parameters  : *gmmsm_sequence_res - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
#ifdef GMM_TCS4
GLOBAL void kern_gmmsm_sequence_res ( T_MMPM_SEQUENCE_RES *gmmsm_sequence_res )
#else
GLOBAL void kern_gmmsm_sequence_res ( T_GMMSM_SEQUENCE_RES *gmmsm_sequence_res )
#endif
{ 
  GMM_TRACE_FUNCTION( "kern_gmmsm_sequence_res" );

  
  switch( GET_STATE( KERN ) )
  {
    case KERN_GMM_RAU_WAIT_FOR_NPDU_LIST:
    { 
      int i=0;
      MCAST (routing_area_update_complete, ROUTING_AREA_UPDATE_COMPLETE);
      routing_area_update_complete->v_receive_n_pdu_number_list = TRUE;
      routing_area_update_complete->receive_n_pdu_number_list.
        c_receive_n_pdu_number_list_val = gmmsm_sequence_res->c_npdu_list;
      
      for (i=0;i<routing_area_update_complete->receive_n_pdu_number_list.c_receive_n_pdu_number_list_val;i++)
      {
        routing_area_update_complete
          ->receive_n_pdu_number_list.receive_n_pdu_number_list_val[i]
          .nsapi 
          =  gmmsm_sequence_res->npdu_list[i].nsapi;
          
        routing_area_update_complete
          ->receive_n_pdu_number_list.receive_n_pdu_number_list_val[i]
          .receive_n_pdu_number_val
           
          =  gmmsm_sequence_res->npdu_list[i].receive_n_pdu_number_val;

      }
      
      PFREE (gmmsm_sequence_res);
      kern_rau_complete (gmm_data->kern.attach_cap.result_type);
      
      break;
    }
    default:
      PFREE (gmmsm_sequence_res);
      TRACE_ERROR( "GMMSM_SEQUENCE_RES received in an unusual state." );
      break;
  }
  GMM_RETURN;
} /* kern_gmmsm_sequence_res() */

 /*TI DK primitive*/
#ifdef REL99
/*+------------------------------------------------------------------------------
| Function    : kern_gmmsm_pdp_status_req
+------------------------------------------------------------------------------
| Description : Handles the primitive GMMSM_PDP_STATUS_REQ
|
| Parameters  : *gmmsm_pdp_status_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_gmmsm_pdp_status_req ( T_MMPM_PDP_CONTEXT_STATUS_REQ *gmmsm_pdp_status_req )

{ 
  GMM_TRACE_FUNCTION( "kern_gmmsm_pdp_status_req" );

  /* Store the data in context */  
  gmm_data->kern.attach_cap.pdp_context_status = 
                                    gmmsm_pdp_status_req->nsapi_set;
  
  GMM_RETURN;
} /* kern_gmmsm_pdp_status_req() */
#endif

/*
+------------------------------------------------------------------------------
| Function    : kern_mmgmm_ciphering_ind
+------------------------------------------------------------------------------
| Description : forwards the ciphering indicator of GSM to MMI
|
| Parameters  : *mmgmm_ciphering_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_mmgmm_ciphering_ind ( T_MMGMM_CIPHERING_IND *mmgmm_ciphering_ind )
{ 
  GMM_TRACE_FUNCTION( "kern_mmgmm_ciphering_ind" );
  {
    PALLOC (gmmreg_ciphering_ind, GMMREG_CIPHERING_IND);
    gmmreg_ciphering_ind->gsm_ciph = mmgmm_ciphering_ind->ciph;
    /* 
     * do not include the ciphering status of gprs purposely to have the
     * behaviour for GSM and for GPRS (do not remember the status of the other!)
     */
    gmmreg_ciphering_ind->gprs_ciph = CIPH_NA;
    PFREE ( mmgmm_ciphering_ind );
    PSEND ( hCommMMI, gmmreg_ciphering_ind );
  }
  GMM_RETURN;
} /* kern_mmgmm_ciphering_ind() */
/*
+------------------------------------------------------------------------------
| Function    : kern_mmgmm_tmsi_ind
+------------------------------------------------------------------------------
| Description : forwards the TMSI received in MM  to GMM
|
| Parameters  : *mmgmm_tmsi_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_mmgmm_tmsi_ind ( T_MMGMM_TMSI_IND *mmgmm_tmsi_ind )
{ 
  GMM_TRACE_FUNCTION( "kern_mmgmm_tmsi_ind" );
  gmm_data->tmsi = mmgmm_tmsi_ind->tmsi;
  TRACE_1_PARA("TMSI %x", gmm_data->tmsi  );
  PFREE ( mmgmm_tmsi_ind );
  GMM_RETURN;
} /* kern_mmgmm_ciphering_ind() */

/*
+------------------------------------------------------------------------------
| Function    : kern_mmgmm_ahplmn_ind
+------------------------------------------------------------------------------
| Description : forwards the ahplmn indicator to MMI
|
| Parameters  : *mmgmm_ahplmn_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_mmgmm_ahplmn_ind ( T_MMGMM_AHPLMN_IND *mmgmm_ahplmn_ind )
{ 
  GMM_TRACE_FUNCTION( "kern_mmgmm_ahplmn_ind" );
  {
    PPASS (mmgmm_ahplmn_ind, gmmreg_ahplmn_ind, GMMREG_AHPLMN_IND);
    PSEND ( hCommMMI, gmmreg_ahplmn_ind );
  }
  GMM_RETURN;
} /* kern_mmgmm_ahplmn_ind() */

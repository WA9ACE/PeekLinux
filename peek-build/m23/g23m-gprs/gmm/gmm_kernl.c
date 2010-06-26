/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  gmm_kernl.c
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
|             SDL-documentation (KERN-statemachine)
+----------------------------------------------------------------------------- 
*/ 


#ifndef GMM_KERNF_C
#define GMM_KERNF_C
#endif

#define ENTITY_GMM

/*==== INCLUDES =============================================================*/

#include <stdio.h>      
#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "ccdapi.h"     /* to get CCD API */
#include "cnf_gmm.h"    /* to get cnf-definitions */
#include "mon_gmm.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "gmm.h"        /* to get the global entity definitions */

#include "gmm_f.h"      /* to get global functions */
#include "gmm_txs.h"    /* to get some signals */
#include "gmm_rdys.h"    /* to get some signals */
#include "gmm_kernp.h"  /* to get kern_gmmreg_detach_req */
#include "gmm_kernf.h"  /* to get local functions */
#include "gmm_kernl.h"  /* to get local label functions */
#include <string.h>     /* to get memcpy() */
#include "gmm_syncs.h"
#include "gmm_rdyf.h"
#include "gmm_em.h"     /* To get Engineering Mode functions */


/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : kern_attach_1                  
+------------------------------------------------------------------------------
| Description : The label ATTACH_1
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_attach_1 ( void )
{ 
  GMM_TRACE_FUNCTION( "kern_attach_1" );
  {
    T_gmobile_identity gmobile_identity;
    MCAST (attach_request, ATTACH_REQUEST); /* T_ATTACH_REQUEST */

    kern_attach_rau_init();
    
    if ( GMM_LLC_STATE_UNASSIGNED == gmm_data->kern.attach_cap.llc_state )
    {
      /* 
       * If no TLLI has yet been assigned to the lower layers. If a valid P-TMSI
       * is available, GMM derives a foreign TLLI from that P-TMSI, otherwise GMM 
       * generates a random TLLI.
       */
      kern_local_attach( FOREIGN_TLLI, INVALID_TLLI );
    }
    
    kern_ulong2mobile_identity ( gmm_data->ptmsi.current,  &gmobile_identity );

    kern_build_ms_network_capability ( &attach_request->ms_network_capability );

    if (gmm_data->config.cipher_on)
    {
      attach_request->ms_network_capability.gea_1                  = GEA_1_YES;
      TRACE_EVENT ("ciphering on requested"); 
    }
    else
    {
      attach_request->ms_network_capability.gea_1                  = GEA_1_NO;
      TRACE_EVENT ("ciphering off requested");
    }

    attach_request->
    attach_type.type_of_attach    = gmm_data->kern.attach_cap.attach_proc_type;
    attach_request->
        ciphering_key_sequence_number.key_sequence
                                      = gmm_data->kern.auth_cap.cksn;
    attach_request->drx_parameter     = gmm_data->drx_parameter;
    attach_request->gmobile_identity  = gmobile_identity;

    attach_request->routing_area_identification  = gmm_data->kern.attach_cap.rai_accepted;

    /* Internally we work always with 3-digit-MNC, adapt this before coding */
    if (attach_request->routing_area_identification.mnc[2] EQ 0xf)
    {
      attach_request->routing_area_identification.c_mnc = 2;
    }
    else
    {
      attach_request->routing_area_identification.c_mnc = SIZE_MNC;
    }

    rr_csf_get_radio_access_capability( &attach_request->ra_cap );

#ifdef TDSGEN
  attach_request->ra_cap.acc_tech_typ=1;
  attach_request->ra_cap.flag_ra_cap2=1;
  attach_request->ra_cap.v_ra_cap2=1;
  attach_request->ra_cap.ra_cap2.acc_cap.pow_class= 1;
  attach_request->ra_cap.ra_cap2.acc_cap.v_a5_bits=0;
  attach_request->ra_cap.ra_cap2.acc_cap.es_ind=1;
  attach_request->ra_cap.ra_cap2.acc_cap.ps=0;
  attach_request->ra_cap.ra_cap2.acc_cap.vgcs=0;
  attach_request->ra_cap.ra_cap2.acc_cap.vbs=0;
  attach_request->ra_cap.ra_cap2.acc_cap.v_ms_struct=0;
  attach_request->ra_cap.ra_cap2.acc_tech_typ=3;
#endif

    /*
     * The READY_TIMER is negotiated
     *  this parameter is optional and is used, if the MS wants to indicate a
     *  preferred value vor the ready timer
     */
    attach_request->v_ready_timer     = FALSE;
     
     switch (gmm_data->kern.attach_cap.attach_proc_type)
     {
       case AT_WHILE_IMSI_AT:
         /* 
          * 44.2.1.2.8
          */
         if (GMMRR_TMSI_INVALID == gmm_data->tmsi)
         /* 
          * <R.GMM.ACINIT.M.009> 
          */
         {
           attach_request->v_tmsi_status = TRUE;
           attach_request->tmsi_status.tmsi_flag = FALSE;
         }
         else
         {
           attach_request->v_tmsi_status = FALSE;
           attach_request->tmsi_status.tmsi_flag = TRUE;
         }
         break;
        
       case AT_COMB:
         attach_request->v_tmsi_status = TRUE;
         if (GMMRR_TMSI_INVALID == gmm_data->tmsi)
         /* 
          * <R.GMM.ACINIT.M.009> 
          */
         {
           attach_request->tmsi_status.tmsi_flag = FALSE;
         }
         else
         {
           attach_request->tmsi_status.tmsi_flag = TRUE;
         }
         break;
       case AT_GPRS:
       default:
         attach_request->v_tmsi_status = FALSE;
         break;
      }
   
    /*
     * <R.GMM.AGINIT.M.004>
     */
    if ( gmm_data->ptmsi_signature.available )
    {  
      attach_request->v_p_tmsi_signature = TRUE;
      attach_request->  p_tmsi_signature.p_tmsi_signature_value
                             = gmm_data->ptmsi_signature.value;
    }
    else
    {
      attach_request->v_p_tmsi_signature = FALSE;
    }
    /*
     * <R.GMM.AGINIT.M.007>
     */
    TRACE_EVENT ( "Info: START: T3310" );
    sig_kern_rdy_start_timer_req ( kern_T3310, gmm_data->kern.t3310_val );
    /* 
     * increments th counter for T3310
     */
    gmm_data->kern.ct3310++;

    switch(GET_STATE(KERN))
    {
      case KERN_GMM_REG_INITIATED:
        break;
      default:
        kern_gmmrr_attach_started();
        break;
    }
    SET_STATE ( KERN, KERN_GMM_REG_INITIATED );
#ifdef FF_EM_MODE
    EM_GMM_SET_STATE(KERN_GMM_REG_INITIATED );
#endif
    /* 
     * CURRENT_TLLI is used instead of FOREIGN_TLLI, because
     * in kern_local_attach it is possible the the RANDOM TLLI was used 
     */ 
    sig_kern_tx_data_req ( CURRENT_TLLI, ATTACH_REQUEST );
  }
  GMM_RETURN;
} /* kern_attach_1() */

/*
+------------------------------------------------------------------------------
| Function    : kern_resume_grr_der
+------------------------------------------------------------------------------
| Description : The LABEL KERN_RESUME_GRR_DER  sends the resume primitive to GRR
|       
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_resume_grr_der ( void )
{ 
  GMM_TRACE_FUNCTION( "kern_resume_grr_der" );
  {
    PALLOC ( gmmrr_resume_req, GMMRR_RESUME_REQ );
      /*
       * GMM enters state DEGEG_SUSPENDED if it was in DEREG_NO_IMSI
       * So I have to tae careto enter NO_IMSI again.
       */
       if (gmm_data->sim_gprs_invalid)
       {
          SET_STATE (KERN, KERN_GMM_DEREG_NO_IMSI);
#ifdef FF_EM_MODE
          EM_GMM_SET_STATE(KERN_GMM_DEREG_NO_IMSI);
#endif
       }
       else
       {
          SET_STATE (KERN, KERN_GMM_DEREG_RESUMING );
#ifdef FF_EM_MODE
          EM_GMM_SET_STATE(KERN_GMM_DEREG_RESUMING );
#endif
       }
       gmm_data->kern.attach_cap.grr_state = GMM_GRR_STATE_ON;
       gmm_data->kern.gmmrr_resume_sent=TRUE;
    PSEND ( hCommGRR, gmmrr_resume_req );
  }
  gmm_data->kern.suspension_type &= ~GMM_SUSP_LOCAL_DETACH;
  GMM_RETURN;
} /* kern_resume_grr_der() */
/*
+------------------------------------------------------------------------------
| Function    : kern_resume_grr_reg
+------------------------------------------------------------------------------
| Description : The LABEL KERN_RESUME_GRR_DER  sends the resume primitive to GRR
|       
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_resume_grr_reg ( void )
{ 
  GMM_TRACE_FUNCTION( "kern_resume_grr_reg" );
  {
    PALLOC ( gmmrr_resume_req, GMMRR_RESUME_REQ );
       SET_STATE (KERN, KERN_GMM_REG_RESUMING);
#ifdef FF_EM_MODE
       EM_GMM_SET_STATE(KERN_GMM_REG_RESUMING);
#endif
       gmm_data->kern.gmmrr_resume_sent = TRUE;
       gmm_data->kern.attach_cap.grr_state = GMM_GRR_STATE_ON;
    PSEND ( hCommGRR, gmmrr_resume_req );
  }
  GMM_RETURN;
} /* kern_resume_grr_reg() */

/*
+------------------------------------------------------------------------------
| Function    : kern_reg_at_local_disable                  
+------------------------------------------------------------------------------
| Description : The label REG_AT_LOCAL_DISABLE
|               is called, when GMM is in state GMM-REGISTERED.SUSPENDED 
|               and MMI has request GPRS_DISABLE
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_reg_at_local_disable ( void )
{ 
  GMM_TRACE_FUNCTION( "kern_reg_at_local_disable" );

  SET_STATE ( KERN, KERN_GMM_NULL_IMSI );
#ifdef FF_EM_MODE
  EM_GMM_SET_STATE(KERN_GMM_NULL_IMSI );
#endif
  gmm_data->kern.detach_cap.detach_type = GMMREG_DT_DISABLE_GPRS;

  kern_local_detach( GMMCS_INT_PROTOCOL_ERROR,  /* TCS 2.1 */
      FALSE, GMM_LOCAL_DETACH_PROC_ENTER_DEREG );

  if ( GMM_MM_REG_NORMAL_SERVICE == GET_STATE(MM) )
  {
    kern_gmmreg_attach_cnf ( GMMREG_AT_IMSI );
  }
  GMM_RETURN;
} /* kern_reg_at_local_disable () */

/*
+------------------------------------------------------------------------------
| Function    : kern_imsi_detach_der_susp                  
+------------------------------------------------------------------------------
| Description : The label IMSI_DETACH_DER_SUSP
|               suspends GRR and enters the appropriate substate to wait for
|               response from GRR
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_imsi_detach_der_susp ( void )
{ 
  GMM_TRACE_FUNCTION( "kern_imsi_detach_der_susp" );

  SET_STATE ( KERN, KERN_GMM_DEREG_SUSPENDING);
#ifdef FF_EM_MODE
  EM_GMM_SET_STATE(KERN_GMM_DEREG_SUSPENDING);
#endif
  kern_gmmrr_suspend (GMMRR_NOT_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_IMSI_DETACH);
  GMM_RETURN;
} /* kern_imsi_detach_der_susp() */
/*
+------------------------------------------------------------------------------
| Function    : kern_imsi_detach_reg_susp                  
+------------------------------------------------------------------------------
| Description : The label IMSI_DETACH_REG_SUSP
|               suspends GRR and enters the appropriate substate to wait for
|               response from GRR
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_imsi_detach_reg_susp ( void )
{ 
  GMM_TRACE_FUNCTION( "kern_imsi_detach_reg_susp" );

  SET_STATE ( KERN, KERN_GMM_REG_SUSPENDING);
#ifdef FF_EM_MODE
  EM_GMM_SET_STATE(KERN_GMM_REG_SUSPENDING);
#endif
  /*
   * kern_llgmm_unassign();
   */
  kern_llgmm_suspend(LLGMM_CALL);
  kern_gmmrr_suspend (GMMRR_SUSP_GPRS, GMMRR_NORMAL_RELEASE,GMM_SUSP_IMSI_DETACH);
  GMM_RETURN;
} /* kern_imsi_detach_reg_susp() */

/*
+------------------------------------------------------------------------------
| Function    : kern_enter_reg_no_cell_limited                  
+------------------------------------------------------------------------------
| Description : The label ENTER_REG_NO_CELL_LIMITED
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_enter_reg_no_cell_limited ( void )
{ 
  GMM_TRACE_FUNCTION( "kern_enter_reg_no_cell_limited" );

  SET_STATE ( KERN, KERN_GMM_REG_NO_CELL_AVAILABLE );
#ifdef FF_EM_MODE
  EM_GMM_SET_STATE(KERN_GMM_REG_NO_CELL_AVAILABLE );
#endif
  kern_llgmm_suspend(LLGMM_NO_GPRS_SERVICE);
  GMM_RETURN;
} /* kern_enter_reg_no_cell_limited() */
/*
+------------------------------------------------------------------------------
| Function    : kern_rau_susp                  
+------------------------------------------------------------------------------
| Description : The label RAU_SUSP
|               is the continuing of the gmmrr_cell_ind primitive in state
|               GMM_DEREGISTERED_SUSPENDED
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_rau_susp ( void )
{ 
  GMM_TRACE_FUNCTION( "kern_rau_susp" );

  if ( gmm_data->kern.attach_cap.gmmreg )
  {
    switch ( gmm_data->kern.attach_cap.mobile_class )
    { 
      case GMMREG_CLASS_CC:
        kern_reg_at_local_disable();
        GMM_RETURN;
      case GMMREG_CLASS_CG:
        if ( GMM_MM_DEREG != GET_STATE(MM) )
        {
          kern_imsi_detach_reg_susp();
          GMM_RETURN;
        }
        break;
      default:
        break;
    }
  }
  
  

  /*
   * TLLI will be assigned in rau_init
   *
   * kern_llgmm_assign();
   */
  kern_rau();
  GMM_RETURN;
} /* kern_rau_susp() */
/*
+------------------------------------------------------------------------------
| Function    : kern_enter_reg_limited_susp                  
+------------------------------------------------------------------------------
| Description : The label ENTER_REG_LIMITED_SUSP
|               is the continuing of the gmmrr_cell_ind primitive in state
|               GMM_DEREGISTERED_SUSPENDED
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_enter_reg_limited_susp ( void )
{ 
  GMM_TRACE_FUNCTION( "kern_enter_reg_limited_susp" );

  if ( gmm_data->kern.attach_cap.gmmreg )
  {
    switch ( gmm_data->kern.attach_cap.mobile_class )
    {
      case GMMREG_CLASS_CC:
        kern_reg_at_local_disable();
        GMM_RETURN;
      case GMMREG_CLASS_CG:
        if ( GMM_MM_REG_NORMAL_SERVICE != GET_STATE(MM) )
        {
          kern_gmmreg_attach_cnf ( GMMREG_AT_GPRS );
        }
        break;
      default:
        if ( GMM_MM_REG_NORMAL_SERVICE == GET_STATE(MM) )
        {
          kern_gmmreg_attach_cnf ( GMMREG_AT_COMB );
        }
        break;
    }
  }
  kern_enter_reg_limited();      
  GMM_RETURN;
} /* kern_enter_reg_limiied_susp() */

/*
+------------------------------------------------------------------------------
| Function    : kern_enter_reg_normal_susp                  
+------------------------------------------------------------------------------
| Description : The label ENTER_REG_NORMAL_SUSP
|               is the continuing of the gmmrr_cell_ind primitive in state
|               GMM_DEREGISTERED_SUSPENDED
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_enter_reg_normal_susp ( void )
{ 
  GMM_TRACE_FUNCTION( "kern_enter_reg_normal_susp" );

  if ( gmm_data->kern.attach_cap.gmmreg )
  {
    switch ( gmm_data->kern.attach_cap.mobile_class )
    {
      case GMMREG_CLASS_CC:
        kern_disable();
        break;
      case GMMREG_CLASS_CG:
        if ( GMM_MM_DEREG != GET_STATE(MM) )
        {
          kern_imsi_detach_reg_susp();
        }
        else
        {
          kern_enter_reg_normal();
        }
        break;
      default:
        switch ( GET_STATE(MM) )
        {
          case GMM_MM_DEREG:
          case GMM_MM_REG_UPDATE_NEEDED:
            /* Combined RAU
             * MSC 3.4.3 IMSI attach while already GPRS attached
             */
            kern_llgmm_assign ();
            kern_rau ();
            break;
          default:
          case GMM_MM_REG_NORMAL_SERVICE:
          case GMM_MM_REG_NO_CELL_AVAILABLE:
          case GMM_MM_REG_INITATED_VIA_GPRS:
            kern_enter_reg_normal();
            break;
        }
            
        break;
    }
  }
  else
  {
    if ( gmm_data->kern.timeout_t3312 )
    {
      /*
       * <R.GMM.RAUTIMER.M.009>
       */
      kern_periodic_rau();
    }
    else
    {
      kern_gmmrr_stop_waiting_for_transmission();
      kern_enter_reg_normal();
    }

  }
  GMM_RETURN;      
} /* kern_enter_reg_normal_susp() */



/*
+------------------------------------------------------------------------------
| Function    : kern_enter_reg_limited                  
+------------------------------------------------------------------------------
| Description : The label ENTER_REG_LIMITED
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_enter_reg_limited ( void )
{ 
  GMM_TRACE_FUNCTION( "kern_enter_reg_limited" );
  
  SET_STATE ( KERN, KERN_GMM_REG_LIMITED_SERVICE );
#ifdef FF_EM_MODE
  EM_GMM_SET_STATE(KERN_GMM_REG_LIMITED_SERVICE );
#endif
  kern_llgmm_suspend(LLGMM_LIMITED);
  GMM_RETURN;
} /* kern_enter_reg_limited_service() */

/*
+------------------------------------------------------------------------------
| Function    : kern_rau_init
+------------------------------------------------------------------------------
| Description : The label RAU_INIT
|
|               RAU will have been initiated
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/

GLOBAL void kern_rau_init ( void )
{



  GMM_TRACE_FUNCTION( "kern_rau_init" );
  {
    /* T_ROUTING_AREA_UPDATE_REQUEST */
    MCAST (routing_area_update_request, ROUTING_AREA_UPDATE_REQUEST);

    kern_attach_rau_init();

    routing_area_update_request->ciphering_key_sequence_number.key_sequence
      = gmm_data->kern.auth_cap.cksn;
    
    routing_area_update_request->v_drx_parameter = TRUE;
    routing_area_update_request->drx_parameter     
      = gmm_data->drx_parameter;
     
    routing_area_update_request->routing_area_identification 
      = gmm_data->kern.attach_cap.rai_accepted;

    /* Internally we work always with 3-digit-MNC, adapt this before coding */
    if (routing_area_update_request->routing_area_identification.mnc[2] EQ 0xf)
      routing_area_update_request->routing_area_identification.c_mnc = 2;
    else
      routing_area_update_request->routing_area_identification.c_mnc = SIZE_MNC;

    rr_csf_get_radio_access_capability( &routing_area_update_request->ra_cap );

    #ifdef TDSGEN
      routing_area_update_request->ra_cap.acc_tech_typ=1;
      routing_area_update_request->ra_cap.flag_ra_cap2=1;
      routing_area_update_request->ra_cap.v_ra_cap2=1;
      routing_area_update_request->ra_cap.ra_cap2.acc_cap.pow_class= 1;
      routing_area_update_request->ra_cap.ra_cap2.acc_cap.v_a5_bits=0;
      routing_area_update_request->ra_cap.ra_cap2.acc_cap.es_ind=1;
      routing_area_update_request->ra_cap.ra_cap2.acc_cap.ps=0;
      routing_area_update_request->ra_cap.ra_cap2.acc_cap.vgcs=0;
      routing_area_update_request->ra_cap.ra_cap2.acc_cap.vbs=0;
      routing_area_update_request->ra_cap.ra_cap2.acc_cap.v_ms_struct=0;
      routing_area_update_request->ra_cap.ra_cap2.acc_tech_typ=3;
    #endif

    /* 
     * this parameter is optional and is used, if the MS wants to indicate
     * a preferred value vor the ready timer
     */
    routing_area_update_request->v_ready_timer = FALSE;

    if ( gmm_data->ptmsi_signature.available )
    {  
      routing_area_update_request->v_p_tmsi_signature = TRUE;
      routing_area_update_request->p_tmsi_signature.p_tmsi_signature_value
        = gmm_data->ptmsi_signature.value;
    }
    else
    {
      routing_area_update_request->v_p_tmsi_signature = FALSE;
    }

#ifdef REL99
    /* Routing area update request in Release 99 has one more
     * optional fields - PDP context status. 
     * The mobile doesn't change its capabilities while in operation. 
     * So MS network capability need not be sent with the RAU request.
     * The old SGSN will pass the MS network capability sent during attach
     * to the new SGSN. The new SGSN reuses this value.
     */
    routing_area_update_request->v_ms_network_capability = FALSE;  /* TCS 4.0 */
    switch (gmm_data->kern.sig_cell_info.sgsnr_flag)/*!gmm_data->release99 =>old code*/ /* TCS 4.0 */
    {
      default:
      case PS_SGSN_UNKNOWN:
      case PS_SGSN_98_OLDER:
        routing_area_update_request->v_pdp_context_status = FALSE; /* TCS 4.0 */
        routing_area_update_request->v_gmobile_identity = FALSE; /* TCS 4.0 */
        break;
      case PS_SGSN_99_ONWARDS:        
        /*
         * PDP context status
         */
        if (INVALID_PDP_CONTEXT_STATUS == gmm_data->kern.attach_cap.pdp_context_status)
        {
          /*GMM doesn't have valid pdp_context_status. Do not include it in RAU*/
          routing_area_update_request->v_pdp_context_status = FALSE; /* TCS 4.0 */
        } else { /*Valid pdp_context_status present*/
          routing_area_update_request->v_pdp_context_status = TRUE; /* TCS 4.0 */
          routing_area_update_request->pdp_context_status.nsapi_set =  /* TCS 4.0 */
                        gmm_data->kern.attach_cap.pdp_context_status; /* TCS 4.0 */
        }

        /*
         * P-TMSI is required in case of UMTS only
         */
        routing_area_update_request->v_gmobile_identity = FALSE; /* TCS 4.0 */

        break;
    }
#endif

    /*
     * I used T3310 instead of T3330
     */
    TRACE_EVENT ( "Info: START: T3310 (instead of T3330)" );
    sig_kern_rdy_start_timer_req ( kern_T3310, gmm_data->kern.t3310_val );
    /*
     * <R.GMM.RAUTIMER.M.021>
     */
    sig_kern_rdy_start_t3312_req ();
    /* 
     * increments the counter for T3310
     */
    gmm_data->kern.ct3310++;


    /*
     * <R.GMM.TLLIUSE.M.003>
     * if not GMM_PERIODIC_RAU then forein TLLI has o be used
     */
    switch ( gmm_data->tlli.current_type)
    {
      case INVALID_TLLI:
        if (kern_rai_changed())
        {
           kern_local_attach ( FOREIGN_TLLI, OLD_TLLI );
        }
        else
        {
           kern_local_attach ( LOCAL_TLLI, OLD_TLLI );
        }
        break;
      case LOCAL_TLLI:
      case CURRENT_TLLI:
      case OLD_TLLI:
          /*
           * only set to foreign TLLI if it is not done already
           */
        if (kern_rai_changed ())
          /*
           * set to foreign TLLI if RA is crossed or if not updated yet
           */
        {
          kern_local_attach ( FOREIGN_TLLI, OLD_TLLI );
        }
        break;
      case FOREIGN_TLLI:
      case RANDOM_TLLI:
      default:
        break;
    }

    /*
     * #5900
     * PATCH UBUOB 3.4.02: in cases of repetition of RAU, no ATTACH_STARTED_REQ should be
     * sent; therefore check state before sending the primitive
     */   
    switch(GET_STATE(KERN))
    {
      case KERN_GMM_RAU_INITIATED:
        /* do not repeat ATTACH_STARTED_REQ in this state! */
        break;
      default:
        kern_gmmrr_attach_started();
        break;
    }
    /* end patch */
    SET_STATE ( KERN, KERN_GMM_RAU_INITIATED );
#ifdef FF_EM_MODE
    EM_GMM_SET_STATE(KERN_GMM_RAU_INITIATED );
#endif

    sig_kern_tx_data_req (CURRENT_TLLI,ROUTING_AREA_UPDATE_REQUEST);
  }
  GMM_RETURN;
} /* kern_rau_init () */
  
/*
+------------------------------------------------------------------------------
| Function    : kern_periodic_rau
+------------------------------------------------------------------------------
| Description : The label PERIODIC_RAU
|
|               Periodic RAU
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/

GLOBAL void kern_periodic_rau ( void )
{
  GMM_TRACE_FUNCTION( "kern_periodic_rau" );

  gmm_data->kern.attach_cap.rau_initiated = GMM_PERIODIC_RAU;
  gmm_data->kern.attach_cap.update_proc_type = COMBINED_RAU;
  {
    /* T_ROUTING_AREA_UPDATE_REQUEST */
    MCAST (routing_area_update_request, ROUTING_AREA_UPDATE_REQUEST);

    routing_area_update_request->v_gmobile_identity      = FALSE;
    routing_area_update_request->v_ms_network_capability = FALSE;
    routing_area_update_request->v_pdp_context_status    = FALSE;

      if ( GMMREG_AT_GPRS != gmm_data->kern.attach_cap.attach_type
      &&  GMMRR_NET_MODE_I == gmm_data->kern.sig_cell_info.net_mode)
      /* 
       * <R.GMM.RCINIT.M.016>
       *
       * 24.008 9.4.1.3. TMSI Status
       *
       * This IE shall be included if the MS performs a 
       * combined GPRS attach and no valid TMSI is available
      */
      {
        if (GMMRR_TMSI_INVALID == gmm_data->tmsi )
        {
          routing_area_update_request->v_tmsi_status = TRUE;
          routing_area_update_request->tmsi_status.tmsi_flag = FALSE;
        }
        else
        {
          routing_area_update_request->v_tmsi_status = FALSE; 
        }
      }
      else
      {
        routing_area_update_request->v_tmsi_status = FALSE;
      }


      if (gmm_data->kern.timeout_t3312
      && GMMREG_CLASS_CG != gmm_data->kern.attach_cap.mobile_class
      && GMMREG_AT_IMSI != gmm_data->kern.attach_cap.attach_type
      && GMMRR_NET_MODE_I == gmm_data->kern.sig_cell_info.net_mode
      && 
        (gmm_data->kern.attach_cap.mm_lau_attempted 
        || GMMRR_SERVICE_NONE== gmm_data->kern.old_sig_cell_info.gmm_status)
      )
      /*
       * <R.GMM.RAUTIMER.M.018>
       * The network does not know, that the mobile was IMSI attached in 
       * the old wrong cell, so that GMM has to IMSI-attach the MS if entering
       * net mode I
       */
      {
       /*        
        24.008 ch. 4.7.2.2:
        If the MS is both IMSI attached for GPRS and non-GPRS services , and if the MS lost coverage of
        the registered PLMN and timer T3312 expires, then:
        
        a)  if the MS returns to coverage in a cell that supports GPRS and that indicates that the network
            is in network operation mode I, then the MS shall either perform the combined routing area
            update procedure indicating "combined RA/LA updating with IMSI attach"; or
        */
        if (gmm_data->kern.attach_cap.mm_lau_attempted)
        {
          routing_area_update_request->update_type.update_type_value 
          = COMBINED_RAU_IMSI_ATTACH;
        }
        else
        {
          /*
           * if (GMMRR_SERVICE_NONE== gmm_data->kern.old_sig_cell_info.gmm_status)
           * Last cell had no GPRS covarage
           */
          routing_area_update_request->update_type.update_type_value 
          = COMBINED_RAU;
        }        

      }
      else
      {
        routing_area_update_request->update_type.update_type_value
          = PERIODIC_RAU;
      }
      gmm_data->kern.attach_cap.update_proc_type = routing_area_update_request->update_type.update_type_value;

      kern_llgmm_suspend(LLGMM_PERIODIC_RAU);
      kern_rau_init();
    
  }
  GMM_RETURN;
} /* kern_periodic_rau () */

/*
+------------------------------------------------------------------------------
| Function    : kern_enter_reg_normal                  
+------------------------------------------------------------------------------
| Description : The label ENTER_REG_NORMAL
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_enter_reg_normal ( void )
{ 
  GMM_TRACE_FUNCTION( "kern_enter_reg_normal" );

  /*
   * kern_llgmm_assign();
   */
  kern_llgmm_resume ();
  SET_STATE ( KERN, KERN_GMM_REG_NORMAL_SERVICE );
#ifdef FF_EM_MODE
  EM_GMM_SET_STATE(KERN_GMM_REG_NORMAL_SERVICE );
#endif
#ifndef GMM_TCS4
  if ( gmm_data->kern.attach_cap.gmmsm )
  {
    PALLOC (gmmsm_establish_cnf, GMMSM_ESTABLISH_CNF);
      gmm_data->kern.attach_cap.gmmsm = FALSE;
    PSEND ( hCommSM, gmmsm_establish_cnf );
  }
#endif 
    kern_gmmreg_attach_cnf((BYTE)(GMM_MM_REG_NORMAL_SERVICE==GET_STATE(MM)?
      GMMREG_AT_COMB:GMMREG_AT_GPRS));

  GMM_RETURN;
} /* kern_enter_reg_normal() */
/*
+------------------------------------------------------------------------------
| Function    : kern_enter_reg_normal_upon_gmmrr_cell_ind                  
+------------------------------------------------------------------------------
| Description : The label ENTER_REG_NORMAL...
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_enter_reg_normal_upon_gmmrr_cell_ind ( void )
{ 
  GMM_TRACE_FUNCTION( "kern_enter_reg_normal_upon_gmmrr_cell_ind" );

  kern_gmmrr_stop_waiting_for_transmission();
  kern_enter_reg_normal();
  GMM_RETURN;
} /* kern_enter_reg_normal() */

/*
+------------------------------------------------------------------------------
| Function    : kern_normal_attach                  
+------------------------------------------------------------------------------
| Description : The label NORM_ATTACH
|
|         MSC : 3.8 Attach
|
|         MSC : 3.8.1 Normal Attach
|         MSC : 3.8.1.1 Initiation
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_norm_attach ( void )
{ 
  GMM_TRACE_FUNCTION( "kern_norm_attach" );
  
  gmm_data->kern.attach_cap.attach_proc_type = AT_GPRS;
  TRACE_EVENT("Info: Normal ATTACH");
  kern_attach_1 ();
  GMM_RETURN;
} /* kern_norm_attach() */

/*
+------------------------------------------------------------------------------
| Function    : kern_comb_attach                  
+------------------------------------------------------------------------------
| Description : The label COMB_ATTACH
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_comb_attach ( void )
{ 
  GMM_TRACE_FUNCTION( "kern_comb_attach" );

/*
 * 19760: <gprs attach while imsi attached> shall not be used
 *        leave following code in case we need to switch back
 *
  switch (GET_STATE(MM))
  {
    default:
      gmm_data->kern.attach_cap.attach_proc_type = AT_WHILE_IMSI_AT;
      TRACE_EVENT("Info: Comb. ATTACH while IMSI attached");
      break;
    case GMM_MM_DEREG:
    case GMM_MM_REG_INITATED_VIA_GPRS:
    case GMM_MM_REG_UPDATE_NEEDED:
      gmm_data->kern.attach_cap.attach_proc_type = AT_COMB;
      TRACE_EVENT("Info: Comb. ATTACH");
      break;
  }
 *
 */
      
  gmm_data->kern.attach_cap.attach_proc_type = AT_COMB;
  TRACE_EVENT("Info: Comb. ATTACH");

  kern_mm_attach_started ();
  kern_attach_1();
    
 GMM_RETURN;
} /* kern_comb_attach() */

/*
+------------------------------------------------------------------------------
| Function    : kern_attach                  
+------------------------------------------------------------------------------
| Description : The label ATTACH
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_attach ( void )
{ 
  GMM_TRACE_FUNCTION( "kern_attach" );

  /* Set GRR in state to transmit user data if not aready done */
  kern_gmmrr_stop_waiting_for_transmission();
  
  switch (gmm_data->kern.attach_cap.attach_type)
  {
    case GMMREG_AT_IMSI:
      if (gmm_data->sim_gprs_invalid)
      {
        SET_STATE(KERN,KERN_GMM_DEREG_NO_IMSI);
#ifdef FF_EM_MODE
        EM_GMM_SET_STATE(KERN_GMM_DEREG_NO_IMSI);
#endif
      }
      else
      {
        if (GMM_MM_DEREG == GET_STATE(MM)
        &&  MMGMM_LIMITED_SERVICE != gmm_data->kern.sig_cell_info.mm_status)
        {
          SET_STATE ( KERN, KERN_GMM_DEREG_SUSPENDING);
#ifdef FF_EM_MODE
          EM_GMM_SET_STATE(KERN_GMM_DEREG_SUSPENDING);
#endif
          kern_gmmrr_suspend (GMMRR_NOT_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_LAU);
        }
        else
        {
          SET_STATE(KERN,KERN_GMM_DEREG_PLMN_SEARCH);
#ifdef FF_EM_MODE
          EM_GMM_SET_STATE(KERN_GMM_DEREG_PLMN_SEARCH);
#endif
        }
      }
      GMM_RETURN;
    case GMMREG_AT_COMB:
      if (GMM_GRR_STATE_OFF==gmm_data->kern.attach_cap.grr_state)
      {
        kern_gmmrr_enable();
      }
      if (GMMRR_NET_MODE_III==gmm_data->kern.sig_cell_info.net_mode
        && GMMREG_CLASS_BC==  gmm_data->kern.attach_cap.mobile_class
      )
      {
        if( GMM_MM_DEREG == GET_STATE(MM)
        &&  MMGMM_LIMITED_SERVICE != gmm_data->kern.sig_cell_info.mm_status)
        {
          SET_STATE ( KERN, KERN_GMM_DEREG_SUSPENDING );
#ifdef FF_EM_MODE
          EM_GMM_SET_STATE(KERN_GMM_DEREG_SUSPENDING );
#endif
          kern_gmmrr_suspend (GMMRR_NOT_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_LAU);
        }
        else
        {
          SET_STATE(KERN,KERN_GMM_DEREG_PLMN_SEARCH);
#ifdef FF_EM_MODE
          EM_GMM_SET_STATE(KERN_GMM_DEREG_PLMN_SEARCH);
#endif
        }
        GMM_RETURN;
      }
      break;
    default:
      if (GMM_GRR_STATE_OFF==gmm_data->kern.attach_cap.grr_state)
      {
        kern_gmmrr_enable();
      }
      break;
  }

  

  /*
   * Upon some error causes MS has o rettach after RAU_REJ.
   * So procedure should be set to ATTACH
   */
  gmm_data->kern.attach_cap.rau_initiated = GMM_NO_RAU;

  if ( GMMREG_CLASS_CG == gmm_data->kern.attach_cap.mobile_class 
    || GMMREG_AT_GPRS == gmm_data->kern.attach_cap.attach_type)
  {
    kern_norm_attach();
  }
  else
  {
    switch ( gmm_data->kern.sig_cell_info.net_mode )
    {
      case GMMRR_NET_MODE_I:

        kern_comb_attach();
        break;
      case GMMRR_NET_MODE_II:
        if ( GMM_MM_DEREG != GET_STATE(MM) 
        || MMGMM_LIMITED_SERVICE == gmm_data->kern.sig_cell_info.mm_status) 
        {
          kern_norm_attach ();
        }
        else
        {
          SET_STATE ( KERN, KERN_GMM_DEREG_SUSPENDING );
#ifdef FF_EM_MODE
          EM_GMM_SET_STATE(KERN_GMM_DEREG_SUSPENDING );
#endif

          kern_gmmrr_suspend (GMMRR_NOT_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_LAU);
        }
        break;
      case GMMRR_NET_MODE_III:
        switch ( gmm_data->kern.attach_cap.mobile_class)
        {
          case GMMREG_CLASS_BG:
            if ( GMM_MM_DEREG != GET_STATE(MM))
            {
  
              kern_imsi_detach_der_susp ();
            }
            else
            {
  
              kern_norm_attach();
            }
            break;
          case GMMREG_CLASS_BC:
            if ( GMM_MM_DEREG == GET_STATE(MM) 
            &&  MMGMM_LIMITED_SERVICE != gmm_data->kern.sig_cell_info.mm_status) 
            {
              SET_STATE ( KERN, KERN_GMM_DEREG_SUSPENDING );
#ifdef FF_EM_MODE
              EM_GMM_SET_STATE(KERN_GMM_DEREG_SUSPENDING );
#endif
              kern_gmmrr_suspend (GMMRR_NOT_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_LAU);
            }
            else
            {
              SET_STATE(KERN, KERN_GMM_DEREG_PLMN_SEARCH);
#ifdef FF_EM_MODE
              EM_GMM_SET_STATE(KERN_GMM_DEREG_PLMN_SEARCH);
#endif
            }
            break;
          case GMMREG_CLASS_A:
            /*
             * react as in NMO II
             */
            if ( GMM_MM_DEREG != GET_STATE(MM)
            || MMGMM_LIMITED_SERVICE == gmm_data->kern.sig_cell_info.mm_status)  
            {
              kern_norm_attach ();
            }
            else
            {
              SET_STATE ( KERN, KERN_GMM_DEREG_SUSPENDING );
#ifdef FF_EM_MODE
              EM_GMM_SET_STATE(KERN_GMM_DEREG_SUSPENDING );
#endif
              kern_gmmrr_suspend (GMMRR_NOT_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_LAU);
            }
            break;          
          default:
            TRACE_ERROR ("unexpected MS class");
            break;
        }
        break;
      default:
        TRACE_ERROR ("unknown network mode");
        break;
    }
      
  }
  GMM_RETURN;
}
/*
+------------------------------------------------------------------------------
| Function    : kern_disable
+------------------------------------------------------------------------------
| Description : The label DISABLE
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_disable ()
{ 
  GMM_TRACE_FUNCTION( "kern_disable" );
  gmm_data->kern.detach_cap.detach_type = GMMREG_DT_DISABLE_GPRS;
  kern_detach();
  GMM_RETURN;
} 
/*
+------------------------------------------------------------------------------
| Function    : kern_detach
+------------------------------------------------------------------------------
| Description : The label DETACH 
|
|
| Parameters  : void
|               gmm_data->kern.detach_cap.detach_type MUST be set !
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_detach()
{

#ifdef REL99
  T_gmobile_identity gmobile_identity;  /* TCS 4.0 */
#endif  

  GMM_TRACE_FUNCTION( "kern_detach" );
  {
    MCAST ( u_detach_request, U_DETACH_REQUEST ); /* T_U_DETACH_REQUEST */

    u_detach_request->u_detach_type.power_off = PO_NORMAL_DETACH;

    switch ( gmm_data->kern.detach_cap.detach_type )
    {
      case GMMREG_DT_DISABLE_GPRS:
        gmm_data->kern.attach_cap.mobile_class = GMMREG_CLASS_CC;
        /* NO break */
      case GMMREG_DT_GPRS:
        if ( GMM_MM_DEREG != GET_STATE(MM) 
        &&  GMMRR_NET_MODE_I == gmm_data->kern.sig_cell_info.net_mode) 
        {
          /*
           * <R.GMM.DINITM.M.007>
           */
          kern_mm_start_t3212 ();
        }
        /*
         * LABEL NORM_DETACH
         */
        u_detach_request->u_detach_type.u_type_of_detach = DT_GPRS;
        kern_llgmm_suspend(LLGMM_PERIODIC_RAU);

        break;
      
      case GMMREG_DT_COMB:
        gmm_data->kern.attach_cap.mobile_class = GMMREG_CLASS_CC;
        if ( GMM_MM_DEREG == GET_STATE(MM) 
          || GMMRR_NET_MODE_I != gmm_data->kern.sig_cell_info.net_mode) 
        { 
          gmm_data->kern.detach_cap.detach_type = DT_GPRS;
          /*
           * LABEL NORM_DETACH
           */
          u_detach_request->u_detach_type.u_type_of_detach = DT_GPRS;
        }
        else
        {
          /*
           * LABEL COMB_DETACH
           */
          u_detach_request->u_detach_type.u_type_of_detach = DT_COMB;
        }
        kern_llgmm_suspend(LLGMM_PERIODIC_RAU);
        break;
        /* break; */
      case GMMREG_DT_POWER_OFF:
      case GMMREG_DT_SOFT_OFF:
        u_detach_request->u_detach_type.power_off = PO_POWER_OFF;
        /* NO break; */
      case GMMREG_DT_LIMITED_SERVICE:
      case GMMREG_DT_SIM_REMOVED:
        gmm_data->kern.attach_cap.mobile_class = GMMREG_CLASS_CC;
        if ( GMM_MM_DEREG != GET_STATE(MM) 
        && GMMRR_NET_MODE_I==gmm_data->kern.sig_cell_info.net_mode)
        {
          /*
           * LABEL COMB_DETACH
           */
          u_detach_request->u_detach_type.u_type_of_detach = DT_COMB;
        } 
        else
        {
          u_detach_request->u_detach_type.u_type_of_detach = DT_GPRS;
        }
        kern_llgmm_suspend(LLGMM_PERIODIC_RAU);
        break;
      case GMMREG_DT_IMSI:
        u_detach_request->u_detach_type.u_type_of_detach = DT_IMSI;
        break;
      default:
        TRACE_ERROR ( "Unexpected detach type in detach label" );
        break;
    }
    /*
     * <R.GMM.DINITM.M.005>
     */
    gmm_data->kern.ct3321 = 0; //44.2.2.1.3 Richard Byrne

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


    kern_mm_detach_started ();
    
    sig_kern_tx_data_req ( CURRENT_TLLI, U_DETACH_REQUEST );

    
    switch ( gmm_data->kern.detach_cap.detach_type )
    {
      case GMMREG_DT_IMSI:
        /*
         * <R.GMM.DINITM.M.006>,
         * <R.GMM.ORATIMSI.M.001>
         */
        SET_STATE ( KERN, KERN_GMM_REG_IMSI_DETACH_INITIATED );
#ifdef FF_EM_MODE
        EM_GMM_SET_STATE(KERN_GMM_REG_IMSI_DETACH_INITIATED );
#endif
        break;
      case GMMREG_DT_SOFT_OFF:
        /* 
         * if mm_imsi attached is reset than MM is requested in function 
         * mm_imsi_detach_ind to indicate detach_done
         */
        if(GMMRR_NET_MODE_I==gmm_data->kern.sig_cell_info.net_mode)
        {
          SET_STATE ( MM, GMM_MM_DEREG );
        }

        vsi_t_stop ( GMM_handle, kern_T3302);
        sig_kern_rdy_stop_t3302_req();
        vsi_t_stop ( GMM_handle, kern_T3310);       
        vsi_t_stop ( GMM_handle, kern_T3311);       

        sig_kern_rdy_start_timer_req ( kern_TPOWER_OFF, TPOWER_OFF_VALUE );
        SET_STATE ( KERN, KERN_GMM_DEREG_INITIATED );
#ifdef FF_EM_MODE
        EM_GMM_SET_STATE(KERN_GMM_DEREG_INITIATED );
#endif
        break;
      case GMMREG_DT_POWER_OFF:
        /* 
         * if mm_imsi attached is reset than MM is requested in function 
         * mm_imsi_detach_ind to indicate detach_done
         */
        if(GMMRR_NET_MODE_I==gmm_data->kern.sig_cell_info.net_mode)
        {
          SET_STATE ( MM, GMM_MM_DEREG );
        }
        vsi_t_stop ( GMM_handle, kern_T3302);
        sig_kern_rdy_stop_t3302_req();
        vsi_t_stop ( GMM_handle, kern_T3310);       
        vsi_t_stop ( GMM_handle, kern_T3311);       
        sig_kern_rdy_start_timer_req ( kern_TPOWER_OFF, TPOWER_OFF_VALUE );
        SET_STATE ( KERN, KERN_GMM_DEREG_INITIATED );
#ifdef FF_EM_MODE
        EM_GMM_SET_STATE(KERN_GMM_DEREG_INITIATED );
#endif
        break;
      case GMMREG_DT_LIMITED_SERVICE:
        kern_l_sim_rem_reg_lim ();
        break;
      default:
        TRACE_EVENT ("  START T3321" );
        sig_kern_rdy_start_timer_req ( kern_T3321, gmm_data->kern.t3321_val );
  
        SET_STATE ( KERN, KERN_GMM_DEREG_INITIATED );
#ifdef FF_EM_MODE
        EM_GMM_SET_STATE(KERN_GMM_DEREG_INITIATED );
#endif
        break;
    }
  }
  GMM_RETURN;
} /* kern_detach() */

/*
+------------------------------------------------------------------------------
| Function    : kern_rau                  
+------------------------------------------------------------------------------
| Description : The label RAU
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_rau ( void )
{ 
  GMM_TRACE_FUNCTION( "kern_rau" );
  {


    gmm_data->kern.attach_cap.rau_initiated = GMM_RAU;
    switch ( gmm_data->kern.sig_cell_info.net_mode )
    {
      case GMMRR_NET_MODE_I:
        kern_llgmm_suspend(LLGMM_RAU);
        break;
      case GMMRR_NET_MODE_II:
        if (kern_lau_needed()
        && !gmm_data->kern.detach_cap.gmmreg)
        {
          kern_llgmm_suspend(LLGMM_CALL);
        }
        else
        {
          kern_llgmm_suspend(LLGMM_RAU);
        }
        break;
      case GMMRR_NET_MODE_III:
        switch ( gmm_data->kern.attach_cap.mobile_class)
        {
          case GMMREG_CLASS_A:
            kern_llgmm_suspend(LLGMM_RAU);
            break;
          case GMMREG_CLASS_BG:
            if ( GMM_MM_DEREG != GET_STATE(MM)
            && MMGMM_LIMITED_SERVICE != gmm_data->kern.sig_cell_info.mm_status )
            { 
               kern_llgmm_suspend(LLGMM_CALL);
            }
            else
            {
               kern_llgmm_suspend(LLGMM_RAU);
            }
            break;
          case GMMREG_CLASS_BC:
            if (GMMREG_AT_GPRS == gmm_data->kern.attach_cap.attach_type)
            {
               kern_llgmm_suspend(LLGMM_RAU);
            }
            else
            /*
             * GSM attach_type
             */
            {
              if (kern_lau_needed())
              {
                kern_llgmm_suspend(LLGMM_CALL);
              }
              else
              {
                if (kern_rai_changed())
                {
                  kern_llgmm_suspend(LLGMM_RAU);
                }
              }
            }
            break;
          default:
            kern_llgmm_suspend(LLGMM_RAU);
            break;
        }
        break;
      default:
        TRACE_ERROR ("unkown net mode");
        break;
    }
  } /* PALLOC */

  kern_rau_limited ();
  GMM_RETURN;
} /* kern_rau() */
/*
+------------------------------------------------------------------------------
| Function    : kern_aac_over_5
+------------------------------------------------------------------------------
| Description : The label AAC_OVER_5
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
void kern_aac_over_5 ( USHORT cause )
{ 
  GMM_TRACE_FUNCTION( "kern_aac_over_5" );
    
  /* <R.GMM.DDATMATT.M.001> 
   * If entry into this state was caused by b) or d) of procedure <AGABNORM> 
   * with cause 'Retry upon entry into a new cell', GPRS attach shall be
   * performed when a new cell is entered.
   */

/*if ( cause NEQ GMMCS_RETRY_IN_NEW_CELL ) TCS 2.1
  {*/
    /*
     * <R.GMM.AGABNORM.M.026>
     */
    TRACE_EVENT ( "Info: START: T3302" );
    TRACE_1_INFO ("ATTACH or RAU will be retried %.1f minutes later",
        gmm_data->kern.t3302_val/(60000.0));
    
    vsi_t_start ( GMM_handle ,  kern_T3302, gmm_data->kern.t3302_val );
    sig_kern_rdy_start_t3302_req();
  /*}*/
    
  gmm_data->gu = GU2_NOT_UPDATED;
  if (GMM_NO_RAU == gmm_data->kern.attach_cap.rau_initiated)
  {
    kern_sim_del_locigprs ();
  }
  kern_sim_gmm_update();
 
  if ((GMM_NO_RAU == gmm_data->kern.attach_cap.rau_initiated && 
          AT_GPRS != gmm_data->kern.attach_cap.attach_proc_type )
  || (GMM_NO_RAU != gmm_data->kern.attach_cap.rau_initiated && 
          (COMBINED_RAU == gmm_data->kern.attach_cap.update_proc_type )
        || (COMBINED_RAU_IMSI_ATTACH == gmm_data->kern.attach_cap.update_proc_type )))
  {
    if ( GMM_NO_RAU == gmm_data->kern.attach_cap.rau_initiated )
    /* 
     * GMM is in state REG_INITIATED
     * we try to attach for GSM
     */
    {

      gmm_data->kern.attach_cap.enter_attempting_to_update_after_lau=TRUE;

      gmm_data->kern.detach_cap.detach_type = GMMREG_DT_GPRS;

      kern_local_detach( cause, FALSE, 
        GMM_LOCAL_DETACH_PROC_SUSP_LAU);
    }
    else
    /* 
     * GMM is in state RAU_INITIATED
     * we try to start LAU
     */
    {
      gmm_data->kern.attach_cap.enter_attempting_to_update_after_lau=TRUE;
      SET_STATE ( KERN, KERN_GMM_REG_SUSPENDING );
#ifdef FF_EM_MODE
      EM_GMM_SET_STATE(KERN_GMM_REG_SUSPENDING );
#endif
      kern_gmmrr_suspend (GMMRR_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_LAU);
    }
    kern_mm_attach_rej ( GMMCS_AAC_OVER_5); /* TCS 2.1 */
        
  }
  else
  {
    if ( GMM_NO_RAU == gmm_data->kern.attach_cap.rau_initiated )
    {
      /* 
       * Optional: The MS may optionally enter state GMM-DEREG.PLMN-SEARCH 
       * instead of GMM-DEREG.ATTEMPTING-TO-ATTACH. (Version 6.4.0)
       *
       * <R.GMM.AGABNORM.M.027>, <R.GMM.AGABNORM.M.030>
       */
      /* 
       * SET_STATE ( KERN, KERN_GMM_DEREG_ATTEMPTING_TO_ATTACH );
       */
       gmm_data->kern.attach_cap.enter_attempting_to_update_after_lau=TRUE;

       gmm_data->kern.detach_cap.detach_type = GMMREG_DT_GPRS;

       kern_local_detach( cause, FALSE, 
         GMM_LOCAL_DETACH_PROC_RESUME);
    }
    else
    {
      SET_STATE ( KERN, KERN_GMM_REG_ATTEMPTING_TO_UPDATE );
#ifdef FF_EM_MODE
      EM_GMM_SET_STATE(KERN_GMM_REG_ATTEMPTING_TO_UPDATE );
#endif
    }
  }
  GMM_RETURN;
 
} /* kern_aac_over_5() */

/*
+------------------------------------------------------------------------------
| Function    : kern_l_sim_rem_reg_lim
+------------------------------------------------------------------------------
| Description : The label L_SIM_REM_REG_LIM
|               SIM removed in state GMM-REGISTERED in a limited substate
+------------------------------------------------------------------------------
*/
GLOBAL void kern_l_sim_rem_reg_lim ( void )
{ 
  GMM_TRACE_FUNCTION( "kern_l_sim_rem_reg_lim" );
  
  /*
   * local GPRS only detach with any error cause 
   * 2.parameter is only used for attach procedure
   *
   * <R.GMM.DETACH.M.005>
   */
  /* 
   * Optional: The MS may optionally enter state GMM-DEREG.PLMN-SEARCH 
   * instead of GMM-DEREG.ATTEMPTING-TO-ATTACH. (Version 6.4.0)
   *
   * <R.GMM.AGABNORM.M.027>, <R.GMM.AGABNORM.M.030>
   *
   SET_STATE ( KERN,   SET_STATE ( KERN, KERN_GMM_NULL_IMSI_LIMITED_SERVICE_REQ ) );
   */
  gmm_data->kern.detach_cap.detach_type = GMMREG_DT_LIMITED_SERVICE;

  kern_local_detach( GMMCS_SIM_REMOVED, FALSE, /* TCS 2.1 */
              GMM_LOCAL_DETACH_PROC_ENTER_NULL_IMSI_LIMITED_SERVICE_REQ);
  /*
   * <R.GMM.DSUBFANO.M.003>
   */

  GMM_RETURN;
} /* kern_l_sim_rem_reg_lim() */
/*
+------------------------------------------------------------------------------
| Function    : kern_l_sim_rem_reg
+------------------------------------------------------------------------------
| Description : The label L_SIM_REM_REG
|               SIM removed in state GMM-REGISTERED
+------------------------------------------------------------------------------
*/
GLOBAL void kern_l_sim_rem_reg ( void )
{ 
  GMM_TRACE_FUNCTION( "kern_l_sim_rem_reg" );

  SET_STATE ( KERN, KERN_GMM_NULL_IMSI_LIMITED_SERVICE_REQ );
#ifdef FF_EM_MODE
  EM_GMM_SET_STATE(KERN_GMM_NULL_IMSI_LIMITED_SERVICE_REQ );
#endif
  
  gmm_data->kern.detach_cap.detach_type = GMMREG_DT_LIMITED_SERVICE;
  kern_detach( );

  GMM_RETURN;
} /* kern_l_sim_rem_reg() */

/*
+------------------------------------------------------------------------------
| Function    : kern_aac
+------------------------------------------------------------------------------
| Description : The label AAC
|
| Parameters  : error_cause
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_aac ( USHORT cause )
{ 
  GMM_TRACE_FUNCTION( "kern_aac" );
  /* <R.GMM.AGABNORM.M.020> */
  gmm_data->kern.aac ++;
  
  TRACE_1_INFO ("error cause: 0x%x",cause);
  TRACE_1_INFO("   aac: %d", gmm_data->kern.aac );            

  gmm_data->kern.detach_cap.error_cause = cause;
  /*
   * reset the counter for timer T3310
   */
  gmm_data->kern.ct3310 = 0;
  /*
   * <R.GMM.AGABNORM.M.021>
   */
  gmm_data->kern.attach_cap.t3310_value=0;
  vsi_t_stop ( GMM_handle , kern_T3310);
  if (GMM_NO_RAU==gmm_data->kern.attach_cap.rau_initiated)
  {
    kern_gmmreg_detach (  GMMREG_DT_GPRS,  
              cause,
              GMMREG_SEARCH_RUNNING,
              cause);
  }

  kern_gmmrr_attach_finished();
  
  if ( gmm_data->kern.aac < MAX_AAC )
  /*
   * AAC_UNDER_5
   */
  {
    if ( AT_GPRS != gmm_data->kern.attach_cap.attach_proc_type )
    {
      if ( kern_lai_changed () ) 
      {
        /*
         * <R.GMM.ACABNORM.M.006>
         */
        kern_mm_attach_rej ( GMMCS_AAC_UNDER_5 ); /* TCS 2.1 */
      }
    }

    /* <R.GMM.DDATMATT.M.001> 
     * If entry into this state was caused by b) or d) of procedure <AGABNORM> 
     * with cause 'Retry upon entry into a new cell', GPRS attach shall be
     * performed when a new cell is entered.
     */
    /* Richard Byrne 44.2.1.2.8 remove if ( cause NEQ GMMCS_RETRY_IN_NEW_CELL ) */ /* TCS 2.1 */
    {
      /*
       * <R.GMM.AGABNORM.M.022>
       */
      TRACE_EVENT ( "Info: START: T3311" );
      vsi_t_start ( GMM_handle ,  kern_T3311, gmm_data->kern.t3311_val );
    }
    switch ( gmm_data->kern.attach_cap.rau_initiated )
    {
      case GMM_NO_RAU: 
        SET_STATE ( KERN, KERN_GMM_DEREG_ATTEMPTING_TO_ATTACH );
#ifdef FF_EM_MODE
        EM_GMM_SET_STATE(KERN_GMM_DEREG_ATTEMPTING_TO_ATTACH );
#endif
        break;
      case GMM_RAU:
        /*
         * <R.GMM.AGABNORM.M.023>
         */
        SET_STATE ( KERN, KERN_GMM_REG_ATTEMPTING_TO_UPDATE );
#ifdef FF_EM_MODE
        EM_GMM_SET_STATE(KERN_GMM_REG_ATTEMPTING_TO_UPDATE );
#endif
        break;
      case GMM_PERIODIC_RAU:
        /*
         * <R.GMM.RNABNORM.M.022>
         */
        kern_enter_reg_normal();
        break;
      default:
        TRACE_ERROR ("unexcpected RAU case");
        break;
    }
    
  }
  else 
  /* 
   * LABEL AAC_OVER_5 
   * 
   * acc expired 
   */
  {
    kern_aac_over_5 (cause);
  }
  GMM_RETURN;
} /* kern_aac() */


/*
+------------------------------------------------------------------------------
| Function    : kern_err_cause
+------------------------------------------------------------------------------
| Description : The label ERR_CAUSE
|
| Parameters  : error_cause
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_err_cause ( USHORT error_cause, BOOL det_acc_sent )
{
  GMM_TRACE_FUNCTION( "kern_err_cause" );

  gmm_data->kern.detach_cap.error_cause = error_cause;

  TRACE_1_INFO ("error cause: %x",error_cause);

  /*
   * <R.GMM.AGREJECT.M.003>, <R.GMM.AGREJECT.M.004>, 
   * <R.GMM.AGREJECT.M.006>, <R.GMM.DSUBFANO.M.003>, 
   * <R.GMM.AGREJECT.M.011>, <R.GMM.AGREJECT.M.012>,
   * <R.GMM.AGREJECT.M.029>, <R.GMM.AGREJECT.M.028>,
   * <R.GMM.AGREJECT.M.034>, <R.GMM.AGREJECT.M.015>, 
   * <R.GMM.AGREJECT.M.016>, <R.GMM.DSUBFANO.M.013>
   */
  
  switch ( error_cause  )
  {
    case GMMCS_IMSI_UNKNOWN: /* TCS 2.1 */
      /* No SIM upate. GPRS is still available */
      /*
       * <R.GMM.DNACM.M.040>
       */
      /*
       * kern_mm_attach_rej ( error_cause );
       * called in kern_local_detach
       */

       gmm_data->kern.detach_cap.detach_type = GMMREG_DT_IMSI;

       kern_local_detach( error_cause, det_acc_sent, GMM_LOCAL_DETACH_PROC_ENTER_REG_NORMAL);

       break;
    case GMMCS_NO_MS_ID: /* TCS 2.1 */
      /*
       * This case can not be reached in attach_procedure,
       * so I may reset the attach parameters, the mobile
       * is just in state DEREGISTERED.
       */
      kern_sim_del_locigprs();
      gmm_data->gu = GU2_NOT_UPDATED;
      kern_sim_gmm_update();
      /*
       * <R.GMM.RCREJECT.M.016>, <R.GMM.RCREJECT.M.035> 
       */
      gmm_data->kern.detach_cap.detach_type = GMM_DT_RE_ATTACH;

      kern_local_detach ( error_cause, det_acc_sent, GMM_LOCAL_DETACH_PROC_RE_ATTACH);
      break;
    case GMMCS_IMPLICIT_DETACHED: /* TCS 2.1 */
      if ( GMMREG_AT_COMB==gmm_data->kern.attach_cap.attach_proc_type  )
      {
        /*
         * <R.GMM.RCREJECT.M.036> 
         */
        kern_mm_attach_rej ( error_cause );
      
        /*
         * <R.GMM.RCREJECT.M.030>
         */
        kern_gmmreg_detach (GMMREG_DT_COMB,
                     GMMCS_IMPLICIT_DETACHED, /* TCS 2.1 */
                     GMMREG_SEARCH_RUNNING,
                     error_cause);
      }
      else
      {
        kern_gmmreg_detach (GMMREG_DT_GPRS,
                     GMMCS_IMPLICIT_DETACHED, /* TCS 2.1 */
                     GMMREG_SEARCH_RUNNING,
                     error_cause);
      }
      gmm_data->kern.detach_cap.detach_type = GMM_DT_RE_ATTACH;
      kern_local_detach( error_cause, det_acc_sent, GMM_LOCAL_DETACH_PROC_RE_ATTACH);

      break;
    case GMMCS_GPRS_NOT_ALLOWED_IN_PLMN: /* TCS 2.1 */
      memcpy (gmm_data->kern.attach_cap.list_of_forbidden_plmns_for_gprs_service[0].mcc,
              gmm_data->kern.sig_cell_info.env.rai.plmn.mcc, SIZE_MCC);
      memcpy (gmm_data->kern.attach_cap.list_of_forbidden_plmns_for_gprs_service[0].mnc,
              gmm_data->kern.sig_cell_info.env.rai.plmn.mnc, SIZE_MNC);

      gmm_data->kern.sig_cell_info.gmm_status=GMMRR_SERVICE_LIMITED;
        
      kern_sim_del_locigprs();
      gmm_data->gu = GU3_ROAMING_NOT_ALLOWED;
      kern_sim_gmm_update();
      kern_mm_attach_rej ( error_cause );

      
      if ( GMM_NO_RAU == gmm_data->kern.attach_cap.rau_initiated
      &&   GMMREG_AT_COMB == gmm_data->kern.attach_cap.attach_proc_type
      &&   kern_lau_needed()) 
      /*
       * If GMM had tried to comb. attach then if MM is not registered it has 
       * to do so.
       */
      {
        /*
         * <GMM.R.ACREJECT.M.014>
         */
        /*
         * <GMM.R.ACREJECT.M.015>
         * <GMM.R.ACREJECT.M.016>
         */
        gmm_data->kern.suspension_type            |= GMM_SUSP_LAU;
      }
      gmm_data->kern.detach_cap.detach_type = GMMREG_DT_GPRS;
      kern_local_detach( error_cause, det_acc_sent, 
            GMM_LOCAL_DETACH_PROC_UNASSIGN);

      break;

    case GMMCS_GPRS_NOT_ALLOWED: /* TCS 2.1 */
      gmm_data->sim_gprs_invalid  = TRUE;
      kern_sim_del_locigprs();
      gmm_data->gu = GU3_ROAMING_NOT_ALLOWED;
      kern_sim_gmm_update();
      /*
       * LABEL ERRCS_GPRS_NOT_ALLOWED
       *
       * <R.DNACM.M.041>
       */
      /*
       * <GMM.R.RCREJECT.M.033>
       */
      kern_mm_start_t3212();
      /*
       * <R.GMM.RCREJECT.M.011>, <R.GMM.AGREJECT.M.013>
       * <R.GMM.ACREJECT.M.012>
       */
        /*
         * <GMM.R.ACREJECT.M.015>
         * <GMM.R.ACREJECT.M.016>
         */
       /* ANITE TC 44.2.1.2.6 */
      gmm_data->kern.detach_cap.detach_type = GMMREG_DT_GPRS;
      kern_local_detach( error_cause, det_acc_sent, GMM_LOCAL_DETACH_PROC_DISABLE);
      break;
    case GMMCS_GSM_GPRS_NOT_ALLOWED: /* TCS 2.1 */
      gmm_data->sim_gprs_invalid  = TRUE;
      kern_sim_del_locigprs();
      gmm_data->gu = GU3_ROAMING_NOT_ALLOWED;
      kern_sim_gmm_update();
      /*
       * <R.GMM.AGREJECT.M.034>, <R.GMM.ACREJECT.M.009>
       * <R.GMM.DNACM.M.025>, <R.GMM.RCREJECT.M.009>
       */
      /*
       * kern_mm_attach_rej ( error_cause );
       * called in kern_local_detach
       */
      gmm_data->kern.detach_cap.detach_type = GMMREG_DT_COMB;
      kern_local_detach( error_cause, det_acc_sent, 
        GMM_LOCAL_DETACH_PROC_COMB_DISABLE);
      break;
      
    default:
      kern_sim_del_locigprs();
      gmm_data->gu = GU3_ROAMING_NOT_ALLOWED;
      kern_sim_gmm_update();

      /*
       * LABEL ERR_CAUSE_2
       */
      /* NO break; */
      switch ( error_cause )
      {
        case GMMCS_PLMN_NOT_ALLOWED: /* TCS 2.1 */
        case GMMCS_ROAMING_NOT_ALLOWED: /* TCS 2.1 */
          /*
           * <R.GMM.AGREJECT.M.021>, <R.GMM.AGREJECT.M.022>
           */
          /* 
           * MM knows, what is to do 
           * because the cause is forwarded be the mmgmm_attach_rej_req
           * primitive
           */
          /* NO BREAK */
        case GMMCS_LA_NOT_ALLOWED: /* TCS 2.1 */

#ifdef REL99
        case GMMCS_NO_SUITABLE_CELL_IN_LA: /*Cause #15*/  /* TCS 4.0 */
#endif

          /*
           * sometimes GRR responses to GMMRR_RESUME_IND verry fast and does not know
           * that RR is in limited, so GMM will set state before.
           */
          gmm_data->kern.sig_cell_info.mm_status = MMGMM_LIMITED_SERVICE;
          sig_kern_sync_set_mm_state(MMGMM_LIMITED_SERVICE);
          gmm_data->kern.detach_cap.detach_type = GMMREG_DT_COMB;
          kern_local_detach( error_cause, det_acc_sent, 
            GMM_LOCAL_DETACH_PROC_UNASSIGN);
          break;
        case GMMCS_ILLEGAL_ME: /* TCS 2.1 */
        case GMMCS_ILLEGAL_MS: /* TCS 2.1 */
          gmm_data->kern.detach_cap.detach_type = GMMREG_DT_COMB;
          kern_local_detach( error_cause, det_acc_sent,  
            GMM_LOCAL_DETACH_PROC_COMB_DISABLE);
          break;
        default:
          /*
           * <R.GMM.ODNOIMSI.M.001>, <R.GMM.DSUBFANO.M.003>
           */
          gmm_data->kern.detach_cap.detach_type = GMMREG_DT_GPRS;
          kern_local_detach( error_cause, det_acc_sent,  GMM_LOCAL_DETACH_PROC_ENTER_NULL_NO_IMSI);
          break;
      }
      break; 
      /* END of LABEL ERR_CAUSE_2 */
  }
  GMM_RETURN;    
} /* kern_err_cause() */

/*
+------------------------------------------------------------------------------
| Function    : kern_enter_der
+------------------------------------------------------------------------------
| Description : The label ENTER_DER
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_enter_der ( void )
{
  GMM_TRACE_FUNCTION( "kern_enter_der" );

  if (gmm_data->kern.attach_cap.mobile_class_changed
  && GMMREG_CLASS_CC!=gmm_data->kern.attach_cap.mobile_class)
  {
    gmm_data->kern.attach_cap.mobile_class_changed=FALSE;
    kern_gmmrr_enable();
  }

  
  kern_reset_cipher ();


  switch ( gmm_data->kern.detach_cap.detach_type )
  {
    case GMMREG_DT_POWER_OFF:
      SET_STATE ( KERN, KERN_GMM_NULL_NO_IMSI);
#ifdef FF_EM_MODE
      EM_GMM_SET_STATE(KERN_GMM_NULL_NO_IMSI);
#endif
      break;
    case GMMREG_DT_SOFT_OFF:
      if (gmm_data->sim_gprs_invalid)
      {
        SET_STATE ( KERN,KERN_GMM_NULL_NO_IMSI);
#ifdef FF_EM_MODE
        EM_GMM_SET_STATE(KERN_GMM_NULL_NO_IMSI);
#endif
      }
      else
      {
        SET_STATE ( KERN,KERN_GMM_NULL_IMSI);
#ifdef FF_EM_MODE
        EM_GMM_SET_STATE(KERN_GMM_NULL_IMSI);
#endif
      }
      break;
    case GMMREG_DT_SIM_REMOVED:
      SET_STATE ( KERN, KERN_GMM_NULL_NO_IMSI);
#ifdef FF_EM_MODE
      EM_GMM_SET_STATE(KERN_GMM_NULL_NO_IMSI);
#endif
      if (GMMRR_NET_MODE_I==gmm_data->kern.sig_cell_info.net_mode)
      {
        kern_mm_imsi_detach_ind (GMMCS_SIM_REMOVED, MMGMM_PERFORM_DETACH,
                                 GMMREG_DT_SIM_REMOVED ); /* TCS 2.1 */
      }
      break;
    case GMMREG_DT_DISABLE_GPRS:
      if (gmm_data->sim_gprs_invalid)
      {
        SET_STATE ( KERN,KERN_GMM_NULL_NO_IMSI);
#ifdef FF_EM_MODE
        EM_GMM_SET_STATE(KERN_GMM_NULL_NO_IMSI);
#endif
      }
      else
      {
        SET_STATE ( KERN,KERN_GMM_NULL_IMSI);
#ifdef FF_EM_MODE
        EM_GMM_SET_STATE(KERN_GMM_NULL_IMSI);
#endif
      }
      kern_mm_activate_mm ();
      break;
    default:
      if (GMMRR_NET_MODE_III==gmm_data->kern.sig_cell_info.net_mode
      && GMMREG_CLASS_BC==  gmm_data->kern.attach_cap.mobile_class
      && kern_lau_needed())
      {
        /* start attach GSM */
        SET_STATE ( KERN, KERN_GMM_DEREG_SUSPENDING );
#ifdef FF_EM_MODE
        EM_GMM_SET_STATE(KERN_GMM_DEREG_SUSPENDING );
#endif
        kern_gmmrr_suspend (GMMRR_NOT_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_LAU);
        break;
      }
      else
      {
        if (gmm_data->sim_gprs_invalid)
        {
          SET_STATE ( KERN, KERN_GMM_NULL_NO_IMSI);
#ifdef FF_EM_MODE
          EM_GMM_SET_STATE(KERN_GMM_NULL_NO_IMSI);
#endif
        }
        else
        {
          SET_STATE ( KERN, KERN_GMM_NULL_IMSI);
#ifdef FF_EM_MODE
          EM_GMM_SET_STATE(KERN_GMM_NULL_IMSI);
#endif
        }
        /* GPRS DISABLE */        
        kern_gmmrr_disable();
        kern_mm_activate_mm ();
      }
      break;
  }
  /* 
   * The used P-TMSI signature should be deleted 
   */
  gmm_data->ptmsi_signature.available = FALSE;
  gmm_data->ptmsi_signature.value     = INVALID_PTMSI_SIGNATURE;

  GMM_RETURN;
} /* kern_enter_der () */


/*
+------------------------------------------------------------------------------
| Function    : kern_attach_complete_end
+------------------------------------------------------------------------------
| Description : The label ATTACH_COMPLETE_END
|
|               ATTACH accept procedure completion end
|
| Parameters  : result_value - the returned attach type
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_attach_complete_end ( UBYTE result_value )
{
  GMM_TRACE_FUNCTION( "kern_attach_complete_end" );

  kern_gmmrr_stop_waiting_for_transmission();

   /*
    *<R.GMM.AGACCEPT.M.027>, <R.GMM.PATTCNF.M.001>
    */
  switch (result_value)
  {
    case NORMAL_ATTACHED:
      if (GMM_MM_REG_NORMAL_SERVICE != GET_STATE(MM))
      {
        if (GMMREG_AT_COMB==gmm_data->kern.attach_cap.attach_type
        && GMMRR_NET_MODE_III != gmm_data->kern.sig_cell_info.net_mode)
        {
          kern_gmmreg_attach_cnf_sr ( GMMREG_AT_GPRS, SEARCH_RUNNING );
        }
        else
        {
          kern_gmmreg_attach_cnf ( GMMREG_AT_GPRS );
        }
        break;
      }
      /* NO break */
    case COMBINED_ATTACHED:
      kern_gmmreg_attach_cnf ( GMMREG_AT_COMB );
      break;
    default:
      TRACE_ERROR ("unknown result value");
      break;
  }
       
  
#ifndef GMM_TCS4
  if ( gmm_data->kern.attach_cap.gmmsm )
  {
    PALLOC (gmmsm_establish_cnf, GMMSM_ESTABLISH_CNF);
      gmm_data->kern.attach_cap.gmmsm = FALSE;
    PSEND ( hCommSM, gmmsm_establish_cnf );
  }
#endif 
  if (gmm_data->kern.attach_cap.attempting_to_update_mm )
  {
    SET_STATE ( KERN, KERN_GMM_REG_ATTEMPTING_TO_UPDATE_MM );
#ifdef FF_EM_MODE
    EM_GMM_SET_STATE(KERN_GMM_REG_ATTEMPTING_TO_UPDATE_MM );
#endif
    gmm_data->kern.attach_cap.attempting_to_update_mm = FALSE;
  }
  else
  {
    /* gmmrr_cell_res
     */
    SET_STATE(GU,GU_UPDATE_NOT_NEEDED);
    SET_STATE ( KERN, KERN_GMM_REG_NORMAL_SERVICE );
#ifdef FF_EM_MODE
    EM_GMM_SET_STATE(KERN_GMM_REG_NORMAL_SERVICE );
#endif
  }
  GMM_RETURN;
} /* kern_attach_complete_end () */



/*
+------------------------------------------------------------------------------
| Function    : kern_rau_complete_end
+------------------------------------------------------------------------------
| Description : The label RAU_COMPLETE_END
|
|               RAU accept procedure completion end
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_rau_complete_end ( UBYTE result_value )
{
  GMM_TRACE_FUNCTION( "kern_rau_complete_end" );

  /* gmmrr_cell_ind   
   * is sent in kern_attach_complete_end
   */
  kern_llgmm_resume (); 

  gmm_data->kern.attach_cap.rau_initiated = GMM_NO_RAU;

  if( gmm_data->sim_gprs_invalid)
  {
    TRACE_EVENT("Detach because SIM removed");
    gmm_data->kern.detach_cap.detach_type = GMMREG_DT_COMB;
    kern_detach( );
  }
  else if ( gmm_data->kern.detach_cap.gmmreg )
  /*
   * MO DETACH was requested
   */
  {
    TRACE_0_INFO("Detach Request again");
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
            /* 
             * if deatch procedure collision and GSM is already detached we 
             * are finished 
             */
            if (GMM_MM_DEREG != GET_STATE(MM))
            {
              SET_STATE ( KERN, KERN_GMM_REG_SUSPENDING);
#ifdef FF_EM_MODE
              EM_GMM_SET_STATE(KERN_GMM_REG_SUSPENDING);
#endif
              kern_llgmm_suspend(LLGMM_CALL);
              kern_gmmrr_suspend(GMMRR_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_LAU);
            }
            else
            {
              kern_detach( );
            }
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
        kern_local_detach( GMMCS_INT_NOT_PRESENT, FALSE, GMM_LOCAL_DETACH_PROC_ENTER_DEREG); /* TCS 2.1 */
        break;
      case GMMREG_DT_GPRS:
      case GMMREG_DT_POWER_OFF:
      case GMMREG_DT_SOFT_OFF:
        kern_detach( );
        break;
      default:
        TRACE_ERROR ("Unexpected detach type in primitive GMMREG_DETACH_REQ");
        break;
    }
  }
  else if (GMMRR_NET_MODE_III==gmm_data->kern.sig_cell_info.net_mode
  && GMMRR_CLASS_BC == gmm_data->kern.attach_cap.mobile_class)
  {
    TRACE_EVENT("Detach because NMO III and BC");
    gmm_data->kern.detach_cap.detach_type = GMMREG_DT_GPRS;
    kern_detach( );
  }
  else
  {
    kern_attach_complete_end(result_value);
  }
  GMM_RETURN;
} /* kern_rau_complete_end () */

/*
+------------------------------------------------------------------------------
| Function    : kern_rau_complete
+------------------------------------------------------------------------------
| Description : The label RAU_COMPLETE
|
|               RAU accept procedure completion
|
| Parameters  : result_value - the returned attach type
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_rau_complete ( UBYTE result_value )
{
  GMM_TRACE_FUNCTION( "kern_rau_complete" );

  sig_kern_tx_data_req ( CURRENT_TLLI, ROUTING_AREA_UPDATE_COMPLETE );
  
  /*
   * If Cell Notification is set in the RAU ACCEPT message, the
   * RAU COMPLETE if send, can be considered as the initial Cell Update.
   * The Next cell update can use LLC NULL frame. 
   * Reference 3GPP TS 24.008 section 4.7.2.1.1
   */
  if (gmm_data->cell_notification EQ FIRST_CELL_NOTIFY) 
  {
    gmm_data->cell_notification = NOT_FIRST_CELL_NOTIFY; /* TCS 4.0 */
  }

  kern_rau_complete_end(result_value);
  GMM_RETURN;
} /* kern_rau_complete () */

/*
+------------------------------------------------------------------------------
| Function    : kern_attach_complete
+------------------------------------------------------------------------------
| Description : The label ATTACH_COMPLETE
|
|               ATTACH and RAU accept procedure completion
|
| Parameters  : attach_complete         - idicates whether AIR ATTACH-COMPLETE
|                                         message has to sent or not
|               receive_n_pdu_number_list
|                                      
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_attach_complete (
                                   UBYTE  result_value,
                                   BOOL attach_complete,
                                   BOOL v_receive_n_pdu_number_list,
                                   T_receive_n_pdu_number_list * receive_n_pdu_number_list)
{
  GMM_TRACE_FUNCTION( "kern_attach_complete" );
  gmm_data->kern.attach_cap.attach_complete = attach_complete;

  if ( GMM_NO_RAU == gmm_data->kern.attach_cap.rau_initiated )
  /*
   * ATTACH procedure
   */
  {
    if ( attach_complete )
    {
      /*
       * <R.GMM.AGACCEPT.M.018>, <R.GMM.TLLIUSE.M011>
       */
      sig_kern_tx_data_req ( CURRENT_TLLI, ATTACH_COMPLETE );
	  
	  /*
       * If Cell Notification is set in the ATTACH ACCEPT message, the
       * ATTACH COMPLETE if send, can be considered as the initial Cell Update.
       * The Next cell update can use LLC NULL frame. 
       * Reference 3GPP TS 24.008 section 4.7.2.1.1
       */
      if (gmm_data->cell_notification EQ FIRST_CELL_NOTIFY) 
      {
        gmm_data->cell_notification = NOT_FIRST_CELL_NOTIFY; /* TCS 4.0 */
      }

    }
    kern_attach_complete_end( result_value );
    
#ifdef GMM_TCS4 
    {
      PALLOC (mmpm_attach_ind, MMPM_ATTACH_IND);
      PSEND ( hCommSM, mmpm_attach_ind );
    }
#endif 

  }
  else
  /*
   * RAU procedure
   */
  {
    MCAST (routing_area_update_complete, ROUTING_AREA_UPDATE_COMPLETE);
    if (v_receive_n_pdu_number_list )
    {
      int i;
#ifdef GMM_TCS4 
      PALLOC (gmmsm_sequence_ind,MMPM_SEQUENCE_IND);
#else
      PALLOC (gmmsm_sequence_ind,GMMSM_SEQUENCE_IND);
#endif 
        gmmsm_sequence_ind->c_npdu_list 
          = receive_n_pdu_number_list->c_receive_n_pdu_number_list_val;
        for (i=0;i<receive_n_pdu_number_list->c_receive_n_pdu_number_list_val;i++)
        { 
          gmmsm_sequence_ind->npdu_list[i].nsapi 
          = receive_n_pdu_number_list->receive_n_pdu_number_list_val[i].nsapi; 
          gmmsm_sequence_ind->npdu_list[i].receive_n_pdu_number_val 
          = receive_n_pdu_number_list->receive_n_pdu_number_list_val[i].receive_n_pdu_number_val; 
        } 
#ifdef GMM_TCS4   
      PSEND ( hCommUPM, gmmsm_sequence_ind );
#else
      PSEND ( hCommSM, gmmsm_sequence_ind );
#endif 
      SET_STATE ( KERN, KERN_GMM_RAU_WAIT_FOR_NPDU_LIST );
#ifdef FF_EM_MODE
      EM_GMM_SET_STATE(KERN_GMM_RAU_WAIT_FOR_NPDU_LIST );
#endif
      GMM_RETURN;
    }
    else
    {
      routing_area_update_complete->v_receive_n_pdu_number_list = 0;
      if ( attach_complete )
      {
        kern_rau_complete(result_value);
      }
      else
      {
        kern_rau_complete_end(result_value);
      }
    }
  }
  GMM_RETURN;
} /* kern_attach_complete () */



/*
+------------------------------------------------------------------------------
| Function    : kern_attach_accept
+------------------------------------------------------------------------------
| Description : The label ATTACH_ACCEPT
|
|               ATTACH and RAU accept procedure
|
|               This procedure is called when the attach or rau accepüt message
|               is received
|
| Parameters  : message_id - RAU ACCEPT or ATTACH ACCEPT message
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_attach_accept
( 
  T_result_gmm                 * result,
  T_force_to_standby           * force_to_standby,
  T_rau_timer                  * rau_timer,
  /* attach: radio_priority */
  T_routing_area_identification *routing_area_identification,
  BOOL                           v_p_tmsi_signature,
  T_p_tmsi_signature           * p_tmsi_signature,
  BOOL                           v_ready_timer,
  T_ready_timer                * ready_timer,
  BOOL                           v_gmobile_identity,
  T_gmobile_identity           * gmobile_identity,
  BOOL                           v_mobile_identity,
  T_mobile_identity            * mobile_identity,
  /*
   * only RAU
   */
  BOOL                           v_receive_n_pdu_number_list,
  T_receive_n_pdu_number_list  * receive_n_pdu_number_list,
  USHORT                         cause,
  BOOL                           v_eqv_plmn_list, /* TCS 2.1 */
  T_eqv_plmn_list              * eqv_plmn_list,  /* TCS 2.0 */
  BOOL                           v_t3302,
  T_t3302                      * t3302,
  BOOL                           v_cell_notification
)
{
  GMM_TRACE_FUNCTION( "kern_attach_accept" );

  TRACE_2_INFO("ACCEPT.rai.lac %#x rac %#x",
      routing_area_identification->lac,
      routing_area_identification->rac);

  gmm_data->kern.attach_cap.attach_acc_after_po = TRUE;
  gmm_data->kern.timeout_t3312 = FALSE;

  if (v_t3302)
  {
    gmm_data->kern.t3302_val = rdy_get_timer ( (T_rau_timer *)t3302 );  
  }

#ifdef REL99
  /* If cell notification field is present GMM should update its
   * context with the information. 
   * Whenever a LLGMM_TRIGGER_REQ is sent, this field should be 
   * checked. If it indicates that cell notification is supported
   * GMM should inform this to LLC with a special cause value.
   * LLC, on receiving this special cause value can use NULL frame 
   * for cell updates. 
   * The first cell notification shouldn't use LLC NULL frame. The 
   * values FIRST_CELL_NOTIFY and NOT_FIRST_CELL_NOTIFY are introduced
   * to take care of this
   */
#endif

         
  if (v_cell_notification)
  {

#ifdef REL99
     gmm_data->cell_notification = FIRST_CELL_NOTIFY; /* TCS 4.0 */
#else
    TRACE_0_INFO("R99: cell notification supported by network, but not supported by the MS");
#endif

  }

#ifdef REL99
  else /*SGSN doesn't support cell notification*/ /* TCS 4.0 */
  { /* TCS 4.0 */
     gmm_data->cell_notification = NO_CELL_NOTIFY;   /* TCS 4.0 */
  }

/* The following #ifdef is added due to a limitation in TDC.
 * TDC in the current release(ver 14) doesn't support setting of T variables.
 * So until the next release (ver 15) of TDC is available, this fix is needed
 * for running GMM406X test cases.
 */
#ifdef _SIMULATION_ /* TCS 4.0 */
  gmm_data->cell_notification = gmm_data->config.cell_notification;   /* TCS 4.0 */
#endif /*_SIMULATION_*/ /* TCS 4.0 */
#endif /* REL99 */

  /* 
   * remove PLMN from forbiden list
   */
  kern_remove_plmn_from_forbidden_list(gmm_data->kern.sig_cell_info.env.rai.plmn);

  kern_set_rai ( routing_area_identification );

  if (GMM_NO_RAU != gmm_data->kern.attach_cap.rau_initiated)
  {
    result->result_value <<= 1;
    result->result_value  |= 1;
    
  }
 /*
  * the following is needed when calling kern_rau_complete() from 
  * within kern_gmmsm_sequence_res():
  */
  gmm_data->kern.attach_cap.result_type = result->result_value;

  
  /*
   * Reset RAU and ATTACH attempt counter
   */
  if (!IS_CAUSE_INVALID (cause))
  {
    gmm_data->kern.detach_cap.error_cause = cause;

    switch ( cause )
    {
      case GMMCS_MSC_TEMP_NOT_REACHABLE: /* TCS 2.1 */
      case GMMCS_NET_FAIL: /* TCS 2.1 */
      case GMMCS_CONGESTION: /* TCS 2.1 */
        if ( COMBINED_ATTACHED == result->result_value )
        {
          TRACE_0_INFO("#0x10 or #0x11 or #0x16 received together with COMBINED attach. Switched to NORMAL attached");
          result->result_value = NORMAL_ATTACHED;
        }
        /* 04.08 44.7.3.2.3.2 */
        /* special kern_attach_reset */
        gmm_data->kern.aac++;
        gmm_data->kern.ct3310 = 0;
        vsi_t_stop ( GMM_handle, kern_T3310);       
        break;
      default:          
        kern_attach_reset();
        break;
    }
  }
  else
  {
    gmm_data->kern.detach_cap.error_cause = GMMCS_INT_NOT_PRESENT; /* TCS 2.1 */
    kern_attach_reset();
  }

  sig_kern_rdy_force_ie_req
    ( force_to_standby->force_to_standby_value ,
    v_mobile_identity ||  v_gmobile_identity);/*lint !e730 (Info -- Boolean argument to function) */
  
  /*
   * force to standby function has to be called before handling of the ready timer, because cu
   * is not allowed upon force to standby
   */
  sig_kern_rdy_t3314_req
    ( (v_gmobile_identity && ID_TYPE_TMSI==gmobile_identity->type_of_identity)
      || (gmobile_identity && ID_TYPE_TMSI==mobile_identity->type_of_identity)
      || v_receive_n_pdu_number_list, /*lint !e730 (Info -- Boolean argument to function) *//*lint !e730 (Info -- Boolean argument to function) */
      v_ready_timer,
      ready_timer,
      rau_timer,
      v_mobile_identity || v_gmobile_identity);

  /*
   * this procedure transmittes the GSIM_UPDATE_REQ  message
   * and starts the kern_local_attach procedure
   *
   * <R.GMM.PATTCNF.M.001>
   */
    /* Delete old signature if no signatur is included in attach_accept
    or rau_accept primitive */
    if (!v_p_tmsi_signature)
    {
      gmm_data->ptmsi_signature.value = INVALID_PTMSI_SIGNATURE;
      gmm_data->ptmsi_signature.available = FALSE;
    }
    kern_tmsi_negotiated (
    v_mobile_identity,
    mobile_identity,
    v_gmobile_identity,
    gmobile_identity,
    v_p_tmsi_signature,
    p_tmsi_signature);

  
  if (!IS_CAUSE_INVALID (cause))
  /*
   * LABEL TMSI_CAUSE
   */
  /* 
   * it is not required that i have to go to aac if an error cause
   * is received in normal attach/rau mode
   */
  {
    if ( GMM_NO_RAU == gmm_data->kern.attach_cap.rau_initiated
    &&  AT_GPRS == gmm_data->kern.attach_cap.attach_proc_type )
    {
      SET_STATE ( MM, GMM_MM_DEREG );
      kern_aac (cause);
      GMM_RETURN;
    }
    switch ( cause )
    {
      case GMMCS_IMSI_UNKNOWN: /* TCS 2.1 */
      {
        gmm_data->tmsi = GMMRR_TMSI_INVALID;
        gmm_data->kern.sig_cell_info.mm_status = MMGMM_LIMITED_SERVICE;
        sig_kern_sync_set_mm_state(MMGMM_LIMITED_SERVICE);

        kern_mm_attach_rej (cause);
        kern_attach_complete (
          result->result_value,
          v_gmobile_identity && 
          ID_TYPE_TMSI==gmobile_identity->type_of_identity, /*lint !e730 (Info -- Boolean argument to function) */
          v_receive_n_pdu_number_list, receive_n_pdu_number_list );/*lint !e613 (Warning -- Possible use of null pointer 'gmobile_identity' in left argument to operator '->') */
        break;
      }
      case GMMCS_MSC_TEMP_NOT_REACHABLE: /* TCS 2.1 */
      case GMMCS_NET_FAIL: /* TCS 2.1 */
      case GMMCS_CONGESTION: /* TCS 2.1 */
        /*
         * ERRCS_NETWORK_FAILURE
         */

        TRACE_1_INFO("   aac: %d", gmm_data->kern.aac );

        /* this is already done.
         */

        if ( gmm_data->gu == GU1_UPDATED
        &&  !kern_rai_changed()
        &&   gmm_data->kern.aac < MAX_AAC )
        {
          TRACE_EVENT ( "Info: START: T3311" );
          vsi_t_start ( GMM_handle ,  kern_T3311, gmm_data->kern.t3311_val );
          kern_mm_attach_rej (cause);
          gmm_data->kern.attach_cap.attempting_to_update_mm = TRUE;
          kern_attach_complete ( 
                result->result_value,
                (v_gmobile_identity
                && ID_TYPE_TMSI==gmobile_identity->type_of_identity)
                ||  
                (v_mobile_identity 
                &&  ID_TYPE_TMSI == mobile_identity->type_of_identity),/*lint !e730 (Info -- Boolean argument to function) */
                v_receive_n_pdu_number_list, receive_n_pdu_number_list);/*lint !e613 (Warning -- Possible use of null pointer 'gmobile_identity' in left argument to operator '->') */
          gmm_data->kern.attach_cap.rau_initiated=GMM_RAU;
        }
        else
        {
          vsi_t_start ( GMM_handle ,  kern_T3302, gmm_data->kern.t3302_val );
          sig_kern_rdy_start_t3302_req();
          TRACE_EVENT ( "Info: START: T3302" );
          TRACE_1_INFO ( "ATTACH or RAU will retried %.1f minutes later",
              (gmm_data->kern.t3302_val/(60000.0)));
          
          switch ( gmm_data->kern.attach_cap.mobile_class)
          {
            case GMMREG_CLASS_A:
              /*
               * kern_mm_set_state ( MMGMM_ENTER_IDLE_MODE );
               */
              SET_STATE ( MM, GMM_MM_DEREG );
              kern_mm_lau();

              kern_attach_complete ( 
                result->result_value,
                (v_gmobile_identity
                && ID_TYPE_TMSI==gmobile_identity->type_of_identity)
                ||  
                (v_mobile_identity 
                &&  ID_TYPE_TMSI == mobile_identity->type_of_identity),/*lint !e730 (Info -- Boolean argument to function) */
                v_receive_n_pdu_number_list, receive_n_pdu_number_list);/*lint !e613 (Warning -- Possible use of null pointer 'gmobile_identity' in left argument to operator '->') */
              GMM_RETURN;
            default:
              TRACE_ERROR("unexpected MS class");
              /* NO break; */
            case GMMREG_CLASS_BG:
            case GMMREG_CLASS_BC:
            case GMMREG_CLASS_B:
              {
                /* rau_initiated will be reset in attah_complete()*/
                BOOL no_rau=  (GMM_NO_RAU == gmm_data->kern.attach_cap.rau_initiated);
                kern_mm_attach_rej (cause);
           
                gmm_data->kern.attach_cap.attempting_to_update_mm = TRUE;
                                  
                kern_attach_complete ( 
                  result->result_value,
                 (v_gmobile_identity
                  && ID_TYPE_TMSI==gmobile_identity->type_of_identity)
                  ||  
                  (v_mobile_identity 
                  &&  ID_TYPE_TMSI == mobile_identity->type_of_identity),/*lint !e730 (Info -- Boolean argument to function) */
                  v_receive_n_pdu_number_list, receive_n_pdu_number_list);/*lint !e613 (Warning -- Possible use of null pointer 'gmobile_identity' in left argument to operator '->') */

                if ((no_rau && AT_GPRS != gmm_data->kern.attach_cap.attach_proc_type )
                || (!no_rau && 
                        (COMBINED_RAU == gmm_data->kern.attach_cap.update_proc_type )
                      || (COMBINED_RAU_IMSI_ATTACH == gmm_data->kern.attach_cap.update_proc_type )))
                {

                  if (no_rau)
                  {
                    SET_STATE ( KERN, KERN_GMM_DEREG_SUSPENDING );
#ifdef FF_EM_MODE
                    EM_GMM_SET_STATE(KERN_GMM_DEREG_SUSPENDING );
#endif
                  }
                  else
                  {
                   /* 04.08 ch. 4.7..2.3 The MM sublayer shall act as in network operation mode II as long as the 
                     combined GMM procedures are not successful and no new RA is entered. The new 
                     MM state is MM IDLE. */
                    gmm_data->kern.sig_cell_info.net_mode= GMMRR_NET_MODE_II;
                    TRACE_0_INFO ("changed to NMOII");

                    SET_STATE ( KERN, KERN_GMM_REG_SUSPENDING );
#ifdef FF_EM_MODE
                    EM_GMM_SET_STATE(KERN_GMM_REG_SUSPENDING );
#endif
                    kern_llgmm_suspend(LLGMM_CALL);
                  }
                  kern_gmmrr_suspend (GMMRR_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_LAU);
                }
              }
              break;
            case GMMREG_CLASS_CG:
              /*
               * <R.GMM.RCSUGPRS.M.020>
               */
              kern_mm_attach_rej (cause);
           
              gmm_data->kern.attach_cap.attempting_to_update_mm = TRUE;
              kern_attach_complete ( 
                result->result_value,
                SEND_ATTACH_COMPLETE,
                v_receive_n_pdu_number_list, receive_n_pdu_number_list);
              break;
          }

        }
        break;
      default:
        kern_aac (cause);
        break;
    }
    SET_STATE( MM, GMM_MM_DEREG );
  }
  else
  /*
   * LABEL TMSI_NO_CAUSE
   */
  {
    /*
     * <R.GMM.RCSUBOTH.M.019>
     */
    kern_mm_attach_acc ( v_mobile_identity, mobile_identity, /* TCS 2.1 */
                         v_eqv_plmn_list, eqv_plmn_list ); /* TCS 2.1 */

    if ( COMBINED_ATTACHED == result->result_value )
    {
      SET_STATE( MM, GMM_MM_REG_NORMAL_SERVICE );
      /*
       * mm_lau_attempt is reset. It is only set if lau has been performed 
       * before via MM_procedures
       */
      gmm_data->kern.attach_cap.mm_lau_attempted=FALSE;
    }
    else
    {
      if (GMM_MM_REG_INITATED_VIA_GPRS == GET_STATE(MM))
      {
        TRACE_ERROR ("network has sent attach_accept (GPRS) w/o error cause but (COMB) has been requested");
        kern_mm_attach_rej(GMMCS_MESSAGE_INVALID); /* TCS 2.1 */
      }
    }

    kern_attach_complete (
        result->result_value,
        (v_gmobile_identity
        && ID_TYPE_TMSI==gmobile_identity->type_of_identity)
        ||  
        (v_mobile_identity 
        &&  ID_TYPE_TMSI == mobile_identity->type_of_identity),/*lint !e730 (Info -- Boolean argument to function) */
        v_receive_n_pdu_number_list, receive_n_pdu_number_list);/*lint !e613 (Warning -- Possible use of null pointer 'gmobile_identity' in left argument to operator '->') */
  }

  GMM_RETURN;
} /* kern_attach_accept () */


/*
+------------------------------------------------------------------------------
| Function    : kern_norm_rau
+------------------------------------------------------------------------------
| Description : The label NORM_RAU
|
|               Normal RAU
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/

GLOBAL void kern_norm_rau ( void )
{
  GMM_TRACE_FUNCTION( "kern_norm_rau" );
  if (gmm_data->kern.timeout_t3312)
  /*
   * 24.008:
   * If the MS is in other state than GMM-REGISTERED.NORMAL-SERVICE when the timer 
   * expires the periodic routing area updating procedure is delayed until the MS 
   * returns to GMM-REGISTERED.NORMAL-SERVICE.
   * In GSM, if the MS in MS operation mode B is in the state GMM-REGISTERED.
   * SUSPENDED when the timer expires the periodic routing area updating procedure 
   * is delayed until the state is left.
   */
  {
    kern_periodic_rau();
    GMM_RETURN;
  }

  {
    /* T_ROUTING_AREA_UPDATE_REQUEST */
    MCAST (routing_area_update_request,ROUTING_AREA_UPDATE_REQUEST);

    routing_area_update_request->v_gmobile_identity      = FALSE;
    routing_area_update_request->v_ms_network_capability = FALSE;
    routing_area_update_request->v_pdp_context_status    = FALSE;

    gmm_data->kern.attach_cap.update_proc_type = RAU;

    routing_area_update_request->update_type.update_type_value
       = RAU;
    TRACE_EVENT("Info: Normal RAU");

    kern_rau_init();
  }

  GMM_RETURN;
} /* kern_norm_rau () */

 
/*
+------------------------------------------------------------------------------
| Function    : kern_comb_rau
+------------------------------------------------------------------------------
| Description : The label COMB_RAU
|
|               Combined RAU
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/

GLOBAL void kern_comb_rau ( void )
{
  GMM_TRACE_FUNCTION( "kern_comb_rau" );
  if (gmm_data->kern.timeout_t3312)
  /*
   * 24.008:
   * If the MS is in other state than GMM-REGISTERED.NORMAL-SERVICE when the timer 
   * expires the periodic routing area updating procedure is delayed until the MS 
   * returns to GMM-REGISTERED.NORMAL-SERVICE.
   * In GSM, if the MS in MS operation mode B is in the state GMM-REGISTERED.
   * SUSPENDED when the timer expires the periodic routing area updating procedure 
   * is delayed until the state is left.
   */
  {
    kern_periodic_rau();
    GMM_RETURN;
  }

  gmm_data->kern.attach_cap.update_proc_type = COMBINED_RAU;

  {
    /* T_ROUTING_AREA_UPDATE_REQUEST */
    MCAST (routing_area_update_request,ROUTING_AREA_UPDATE_REQUEST);

    routing_area_update_request->v_gmobile_identity      = FALSE;
    routing_area_update_request->v_ms_network_capability = FALSE;
    routing_area_update_request->v_pdp_context_status    = FALSE;

    if ( GMMREG_AT_GPRS != gmm_data->kern.attach_cap.attach_type 
    &&   GMMRR_TMSI_INVALID == gmm_data->tmsi )
    /* 
     * <R.GMM.RCINIT.M.016>
     *
     * 24.008 9.4.1.3. TMSI Status
     *
     * This IE shall be included if the MS performs a 
     * combined GPRS attach and no valid TMSI is available
    */
    {
     routing_area_update_request->v_tmsi_status = TRUE;
     routing_area_update_request->tmsi_status.tmsi_flag = FALSE;
    }
    else
    {
      routing_area_update_request->v_tmsi_status = FALSE;
    }


    if ( GMM_MM_DEREG  == GET_STATE(MM)
    ||  gmm_data->kern.attach_cap.mm_lau_attempted)
    {
      gmm_data->kern.attach_cap.update_proc_type = COMBINED_RAU_IMSI_ATTACH;
      routing_area_update_request->update_type.update_type_value
        = COMBINED_RAU_IMSI_ATTACH;
      TRACE_EVENT("Info: Comb. RAU with IMSI attach");
    }
    else
    {
      gmm_data->kern.attach_cap.update_proc_type = COMBINED_RAU;
      routing_area_update_request->update_type.update_type_value 
        = COMBINED_RAU;
      TRACE_EVENT("Info: Comb. RAU");
    }
  }
  
  kern_mm_attach_started ();
  kern_rau_init();
  GMM_RETURN;
} /* kern_comb_rau () */


/*
+------------------------------------------------------------------------------
| Function    : kern_rau_limited
+------------------------------------------------------------------------------
| Description : The label RAU_LIMITED
|
|               Start of RAU without supention of the rest of the world
|               beacause the rest is already suspended
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_rau_limited ( void )
{
  GMM_TRACE_FUNCTION( "kern_rau_limited" );

  switch ( gmm_data->kern.sig_cell_info.net_mode )
  {
    case GMMRR_NET_MODE_I:
      if (GMM_MM_DEREG != GET_STATE(MM)
      || GMMREG_AT_COMB==gmm_data->kern.attach_cap.attach_type)
      {
        kern_comb_rau();
      }
      else
      {
        kern_norm_rau();
      }
      break;
    case GMMRR_NET_MODE_II:
      if (kern_lau_needed()) 
      /* 
       * if deatch procedure collision and GSM is already detached we can start 
       * rau
       */
      {
        SET_STATE ( KERN, KERN_GMM_REG_SUSPENDING );
#ifdef FF_EM_MODE
        EM_GMM_SET_STATE(KERN_GMM_REG_SUSPENDING );
#endif
        kern_gmmrr_suspend (GMMRR_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_LAU);
      }
      else
      {
        kern_norm_rau();
      }
      break;
    case GMMRR_NET_MODE_III:
      switch ( gmm_data->kern.attach_cap.mobile_class)
      {
        case GMMREG_CLASS_A:
          TRACE_ERROR ("ClassA not supported");
          break;
        case GMMREG_CLASS_BG:
          if ( GMM_MM_DEREG != GET_STATE(MM) )
          { 
            if (kern_lau_needed())  
            /*
             * First update than detach!!!
             */
            {
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
          }
          else
          {
            kern_norm_rau();
          }
          break;
        case GMMREG_CLASS_BC:
          if (GMMREG_AT_GPRS == gmm_data->kern.attach_cap.attach_type)
          {
            kern_norm_rau();
          }
          else
          /*
           * GSM attach_type
           */
          {
            if ( kern_lau_needed())  
            {
              SET_STATE ( KERN, KERN_GMM_REG_SUSPENDING );
#ifdef FF_EM_MODE
              EM_GMM_SET_STATE(KERN_GMM_REG_SUSPENDING );
#endif
              kern_gmmrr_suspend (GMMRR_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_LAU);
            }
            else
            {
              if (kern_rai_changed())
              {
                kern_norm_rau();
              }
              else
              {
                 gmm_data->kern.detach_cap.detach_type = GMMREG_DT_GPRS;
                 kern_detach();
              }
            }
          }
          break;
        case GMMREG_CLASS_CG:
          kern_norm_rau();
          break;
        default:
          break;
      }
        
      break;
    default:
      TRACE_ERROR ("unknown netmode");
      break;
  }
  GMM_RETURN;
} /* kern_rau_limited () */

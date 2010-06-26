/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  gmm_kerns.c
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
|             functions to handles the incoming process internal signals as  
|             described in the SDL-documentation (KERN-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#ifndef GMM_KERNS_C
#define GMM_KERNS_C
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
#include "gmm_em.h"      

#include "gmm_txs.h"    /* to get signal functions */
#include "gmm_f.h"      /* to get global functions */
#include "gmm_rdys.h"   /* to get signal functions */
#include "gmm_rxs.h"   /* to get signal functions */
#include "gmm_syncf.h"   /* to get signal functions */
#include "gmm_kernl.h"  /* to get local functions declared as label */
#include "gmm_kernf.h"  /* to get local functions */
#ifdef REL99
#include "gmm_rdyf.h"
#endif

#include <string.h>    /* to get memset */
/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : kern_indicate_service_loss
+------------------------------------------------------------------------------
| Description : The function indicates loss of service to the MMI
|       
| Parameters  : detach_type: The type of lost service 
|               service    : The service we still have (no or limited)
|               searching  : Another PLMN to try available
|
+------------------------------------------------------------------------------
*/
LOCAL void kern_indicate_service_loss (UBYTE detach_type, 
                                       USHORT service, 
                                       UBYTE searching,
                                       USHORT cause)
{
  GMM_TRACE_FUNCTION ("kern_indicate_service_loss");

  switch (service)
  {
    case NREG_NO_SERVICE:
      kern_gmmreg_detach (detach_type,
                          GMMCS_NO_SERVICE, /* TCS 2.1 */
                          searching,
                          cause);
      break;

    case NREG_LIMITED_SERVICE:
      kern_gmmreg_detach (detach_type,
                          GMMCS_LIMITED_SERVICE, /* TCS 2.1 */
                          searching,
                          cause);
      break;

    default: /* Shall not happen as all defined values caught */
      kern_gmmreg_detach (detach_type,
                          GMMCS_INT_PROTOCOL_ERROR, /* TCS 2.1 */
                          searching,
                          cause);
      break;
  }
  GMM_RETURN;
}

/*
+------------------------------------------------------------------------------
| Function    :kern_cell_update_or_attach_reset
+------------------------------------------------------------------------------
| Description : The function starts cell update or not.
|       
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
LOCAL void kern_cell_update_or_attach_reset (void )
{
  BOOL call_attach_reset;
  GMM_TRACE_FUNCTION ("kern_cell_update_or_attach_reset");

  if (GU_UPDATE_NEEDED EQ GET_STATE(GU))
  {
    call_attach_reset=TRUE;
  }
  else
  {
    switch( GET_STATE( KERN ) )
    {
      case KERN_GMM_DEREG_ATTEMPTING_TO_ATTACH:
      case KERN_GMM_REG_ATTEMPTING_TO_UPDATE:
      case KERN_GMM_REG_ATTEMPTING_TO_UPDATE_MM:
      case KERN_GMM_REG_INITIATED:
      case KERN_GMM_RAU_INITIATED:
        call_attach_reset=kern_ra_crossed();
        break;
      default:
        call_attach_reset=kern_rai_changed();
        break;
    }
  }

  if ( call_attach_reset)
  {
    TRACE_2_INFO("Info: changed to: lac %x rac %x",            
      gmm_data->kern.sig_cell_info.env.rai.lac,
      gmm_data->kern.sig_cell_info.env.rai.rac);
    /*
     * <R.GMM.AGABNORM.M.010>
     */
    /* 
     * NO kern_attach_reset();
     * to do not stop t3310 or T3311 here
     */
    gmm_data->kern.ct3310 = 0;
    gmm_data->kern.aac = 0;
  } 
  else
  {
  /*
   * In state GMM_REG_UPDATE_NEEDED, GMM will start RAU and no cell update
   * is necessary
   */
    switch (gmm_data->kern.detach_cap.error_cause)
    {
     default:
       break;
     case GMMCS_MSC_TEMP_NOT_REACHABLE: /* TCS 2.1 */
     case GMMCS_NET_FAIL: /* TCS 2.1 */
     case GMMCS_CONGESTION: /* TCS 2.1 */
        /* 04.08 ch. 4.7..2.3 The MM sublayer shall act as in network operation mode II as long as the 
        combined GMM procedures are not successful and no new RA is entered. The new 
        MM state is MM IDLE. */
       gmm_data->kern.sig_cell_info.net_mode= GMMRR_NET_MODE_II;
       TRACE_0_INFO ("changed to NMOII");

       break;
    }

    if (GMMRR_SERVICE_FULL EQ gmm_data->kern.sig_cell_info.gmm_status)
    {  
      switch( GET_STATE( KERN ) )
      {
        case KERN_GMM_NULL_NO_IMSI:
        case KERN_GMM_NULL_NO_IMSI_LIMITED_SERVICE_REQ:
        case KERN_GMM_NULL_IMSI_LIMITED_SERVICE_REQ:
        case KERN_GMM_NULL_IMSI:
        case KERN_GMM_NULL_PLMN_SEARCH:

        case KERN_GMM_DEREG_NO_CELL_AVAILABLE:
        case KERN_GMM_DEREG_LIMITED_SERVICE:
        case KERN_GMM_DEREG_NO_IMSI:
        case KERN_GMM_DEREG_PLMN_SEARCH:
        case KERN_GMM_DEREG_SUSPENDING:
        case KERN_GMM_DEREG_SUSPENDED:
          break;

        case KERN_GMM_DEREG_ATTEMPTING_TO_ATTACH:
        case KERN_GMM_REG_INITIATED:

        case KERN_GMM_REG_NO_CELL_AVAILABLE:
        case KERN_GMM_REG_LIMITED_SERVICE:
        case KERN_GMM_REG_ATTEMPTING_TO_UPDATE_MM:
        case KERN_GMM_REG_RESUMING:
        case KERN_GMM_REG_NORMAL_SERVICE:
        case KERN_GMM_REG_IMSI_DETACH_INITIATED:

        case KERN_GMM_DEREG_INITIATED:
          if (kern_cell_changed())
          {
            sig_kern_rdy_cu_req();
          }
          /*
           * sig_kern_rdy_cu_req has to be called allways before 
           * kern_gmmrr_stop_waiting_for_transmission(), because rdy may sent 
           * back the sig_rdy_kern_cu_ind to get the parameter cu_cause for
           * gmmrr_cell_res
           */
          kern_gmmrr_stop_waiting_for_transmission();
          break;

        case KERN_GMM_REG_ATTEMPTING_TO_UPDATE:
        case KERN_GMM_RAU_INITIATED:
        case KERN_GMM_RAU_WAIT_FOR_NPDU_LIST:
          if (kern_cell_changed())
          {
            sig_kern_rdy_cu_req();
            
            switch(GET_STATE(CU))
            {
              case CU_REQUESTED_CELL_RES_SENT:
                SET_STATE (CU, CU_CELL_RES_SENT);
                kern_send_llgmm_trigger_req();
                break;
              case CU_REQUESTED:
                SET_STATE (CU, CU_NOT_REQUESTED);
                kern_send_llgmm_trigger_req();
                break;
              default:
              case CU_NOT_REQUESTED:
              case CU_CELL_RES_SENT:
                break;
            }
          }
          break;

        case KERN_GMM_REG_SUSPENDING:
        case KERN_GMM_REG_SUSPENDED:
        case KERN_GMM_REG_TEST_MODE:
        case KERN_GMM_REG_TEST_MODE_NO_IMSI:
          break;

        default:
          break;
        }

    }
  }
  GMM_RETURN;
}




/*
+------------------------------------------------------------------------------
| Function    : kern_gmmreg_ciphering_ind
+------------------------------------------------------------------------------
| Description : forwards the ciphering indicator of GPRS to MMI. This function is
|               added for resolution of issue 3765
|
| Parameters  : gmm_cipher - GPRS ciphering indicator
|
+------------------------------------------------------------------------------
*/
LOCAL void kern_gmmreg_ciphering_ind ( U8 gmm_cipher )
{ 
  GMM_TRACE_FUNCTION( "kern_gmmreg_ciphering_ind" );
  {
    PALLOC (gmmreg_ciphering_ind, GMMREG_CIPHERING_IND);
    /* 
     * do not include the ciphering status of gsm purposely to have the
     * behaviour for GSM and for GPRS (do not remember the status of the other!)
     */
    gmmreg_ciphering_ind->gsm_ciph = CIPH_NA;
    gmmreg_ciphering_ind->gprs_ciph = gmm_cipher;
    PSEND ( hCommMMI, gmmreg_ciphering_ind );
  }
  GMM_RETURN;
} /* kern_gmmreg_ciphering_ind() */



/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : sig_sync_kern_cell_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_SYNC_KERN_CELL_IND
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_sync_kern_cell_ind ( T_SIG_CELL_INFO  * sig_cell_info )
{ 
  GMM_TRACE_FUNCTION( "sig_sync_kern_cell_ind" );
  TRACE_EVENT ("--- IN:GMMRR_SIG_CELL_IND");

#ifdef REL99
  switch (sig_cell_info->sgsnr_flag)/*!gmm_data->release99 =>old code*/ /* TCS 4.0 */
  {
    default:
    case PS_SGSN_UNKNOWN:
    case PS_SGSN_98_OLDER:
      gmm_data->kern.t3302_val = sig_cell_info->t3212_val;
      break;
    case PS_SGSN_99_ONWARDS:
      break;
  }
#endif

  /*
   * Cel ID
   */
  gmm_data->kern.cell_id.lac  = sig_cell_info->env.rai.lac;
  gmm_data->kern.cell_id.rac  = sig_cell_info->env.rai.rac;
 
  GMM_TRACE_GMM_DATA(GMM_DEBUG_PRINT_MASK_CID);

  if (0xff NEQ gmm_data->config.nmo)
  {
    sig_cell_info->net_mode=gmm_data->config.nmo;
    TRACE_EVENT("NMO changed");
  }
#ifdef GMM_TCS4
  TRACE_11_PARA("%s, %x%x%x, %x%x%x, NMO: %d, lac %x, rac %x, cid %x",  /* TCS 4.0 */
          (RT_GPRS==sig_cell_info->rt?"GPRS":"EDGE"), /* TCS 4.0 */
          sig_cell_info->env.rai.plmn.mcc[0],
          sig_cell_info->env.rai.plmn.mcc[1],
          sig_cell_info->env.rai.plmn.mcc[2],
          sig_cell_info->env.rai.plmn.mnc[0],
          sig_cell_info->env.rai.plmn.mnc[1],
          sig_cell_info->env.rai.plmn.mnc[2],
          sig_cell_info->net_mode+1,
          gmm_data->kern.cell_id.lac,
          gmm_data->kern.cell_id.rac,
          sig_cell_info->env.cid);

#else
  TRACE_10_PARA("%x%x%x, %x%x%x, NMO: %d, lac %x, rac %x, cid %x", 
          sig_cell_info->env.rai.plmn.mcc[0],
          sig_cell_info->env.rai.plmn.mcc[1],
          sig_cell_info->env.rai.plmn.mcc[2],
          sig_cell_info->env.rai.plmn.mnc[0],
          sig_cell_info->env.rai.plmn.mnc[1],
          sig_cell_info->env.rai.plmn.mnc[2],
          sig_cell_info->net_mode+1,
          sig_cell_info->env.rai.lac,
          sig_cell_info->env.rai.rac,
          sig_cell_info->env.cid);
#endif

#ifndef NTRACE
    switch ( sig_cell_info->gmm_status)
    {
      case GMMRR_SERVICE_LIMITED:
        TRACE_0_PARA("GPRS limited");
        break;
      case GMMRR_SERVICE_NONE:
        TRACE_0_PARA("no GPRS");
        break;
      case GMMRR_SERVICE_FULL:
        TRACE_0_PARA("GPRS available");
        break;
      default:
        TRACE_ERROR ("unexpexted gmm_status");
    }
    switch (sig_cell_info->mm_status)
    {
      case MMGMM_LIMITED_SERVICE:

        TRACE_0_PARA("MM limited service");
        break;
      case MMGMM_FULL_SERVICE:
        TRACE_0_PARA("MM full service");
        break;
      case MMGMM_CELL_SELECTED:
        TRACE_0_PARA("MM cell changed");
        break;
      case MMGMM_WAIT_FOR_UPDATE:
        TRACE_0_PARA("MM LUP REQ");
        break;
      default:
        TRACE_ERROR("unknown MM status");
    }

#endif
   
  gmm_data->kern.old_sig_cell_info = gmm_data->kern.sig_cell_info;
  gmm_data->kern.sig_cell_info = *sig_cell_info;

  if (gmm_data->kern.attach_cap.mobile_class_changed
  && sig_cell_info->gmmrr_cell_ind_received)
  {
    gmm_data->kern.attach_cap.mobile_class_changed=FALSE;
    kern_gmmrr_enable();
  }

  /*
   * E/// if we left a cell where NO GPRS has been supported and enter a cell where GPRS is
   * there we have to make RAU. "E/// TC 3.2.5 RAU re-entering into GPRS coverage"
   */
  if (GMMRR_SERVICE_NONE EQ gmm_data->kern.old_sig_cell_info.gmm_status
  &&  MMGMM_FULL_SERVICE EQ gmm_data->kern.old_sig_cell_info.mm_status
  && GMMRR_SERVICE_FULL EQ gmm_data->kern.sig_cell_info.gmm_status
  )
  {
    SET_STATE(GU,GU_UPDATE_NEEDED);
  }

  if ( GMMRR_SERVICE_FULL EQ gmm_data->kern.sig_cell_info.gmm_status 
  && kern_is_cell_forbidden() )
  {
    gmm_data->kern.sig_cell_info.gmm_status = GMMRR_SERVICE_LIMITED;
  }


  if (GMMREG_CLASS_B EQ gmm_data->kern.attach_cap.mobile_class
  &&  GMMRR_NET_MODE_III EQ gmm_data->kern.sig_cell_info.net_mode )
  /*
   * MS class B mobile reacts in NMO III as in NMO II
   * notice that it is not possible to change mobile classes from one to 
   * another for B, BG or BC. MS change are allowed only between A, (one of B|BC|BG), CG 
   * or CC
   */
  {
    TRACE_0_INFO("mobile reacts as in NMO II, because of MS class B");
    gmm_data->kern.sig_cell_info.net_mode = GMMRR_NET_MODE_II;
  }

  /*
   * Inform MMI that cell has changed or that we GSM is in full service again
   */
  kern_send_gmmreg_cell_ind();
  
  if (sig_cell_info->gmmrr_cell_ind_received )
  {
    gmm_data->kern.attach_cap.grr_state = GMM_GRR_STATE_ON;
    SET_STATE(CU, CU_NOT_REQUESTED); 
    gmm_data->kern.gmmrr_resume_sent = FALSE;
  }

  kern_cell_update_or_attach_reset ();


  switch( GET_STATE( KERN ) )
  {
    /*************************************************************************
     *  state GMM-DEREG
     *************************************************************************/
    case KERN_GMM_REG_SUSPENDING:
    case KERN_GMM_DEREG_SUSPENDING:
      /* if GRR reselects the cell in this state GRR has to be suspended again
       *TC 44.2.2.2.2
       */
      if (sig_cell_info->gmmrr_cell_ind_received)
      {
        TRACE_ERROR ("unexpected GMMRR_CELL_IND received in state SUSPENDING");
        TRACE_ERROR("-waiting for GMMRR_SUSPEND_CNF");
        kern_gmmrr_suspend (GMMRR_NOT_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_UNKNOWN);
      }
      break;
    
    case KERN_GMM_DEREG_SUSPENDED:

      if (!sig_cell_info->gmmrr_cell_ind_received
      && MMGMM_WAIT_FOR_UPDATE==sig_cell_info->mm_status)
      /*
       * TACTIVATE has expied so we are in a deadlock situaton and MM waits for 
       * location update request
       */
      {
        TRACE_ERROR("MMGMM_ACTIVATE_IND received in DEREG_SUSPENDED");

        switch (gmm_data->kern.sig_cell_info.net_mode)
        {
          case GMMRR_NET_MODE_II:  
            kern_resume_grr_der();
            break;
          case GMMRR_NET_MODE_III:
            switch (gmm_data->kern.attach_cap.mobile_class)
            {
              case GMMREG_CLASS_BC:
              case GMMREG_CLASS_B:
                kern_resume_grr_der();
                break;
              default:
                break;
            }
            break;
          default:
            break;  
        }
      }
      else
      {
        TRACE_0_INFO("cell_ind ignored");
      }
      break;
    case KERN_GMM_DEREG_RESUMING:
       if ( gmm_data->kern.attach_cap.enter_attempting_to_update_after_lau )
       {
          gmm_data->kern.attach_cap.enter_attempting_to_update_after_lau = FALSE;
          SET_STATE (KERN, KERN_GMM_DEREG_ATTEMPTING_TO_ATTACH);
#ifdef FF_EM_MODE
          EM_GMM_SET_STATE(KERN_GMM_DEREG_ATTEMPTING_TO_ATTACH);
#endif
       }
       else if ( gmm_data->kern.detach_cap.gmmreg ) /* Process Detach if pending */
       {
         TRACE_0_INFO("Stored Detach Request from  KERN_GMM_DEREG_RESUMING");
         /* Already LLC is in suspended state when we are in KERN_GMM_DEREG_RESUMING 
          * give cell_res so that GRR can transmit */
         kern_gmmrr_stop_waiting_for_transmission();
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
               if (GMM_MM_DEREG NEQ GET_STATE(MM))
               {
                 SET_STATE ( KERN, KERN_GMM_REG_SUSPENDING);
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
         GMM_RETURN;
       }
       /* unusual change of states */
       /* NO break; */

    case KERN_GMM_DEREG_NO_IMSI:
      /* TC 44.2.6.2.1 #7 problem */
      /*
       * While suspension GMM is able to receive mobile class changes.
       * This changes will be handled, when GMM is resumed, i. e. here!
       */
      switch ( gmm_data->kern.attach_cap.mobile_class )
      {
        case GMMREG_CLASS_CC:
          /*
           * GMM enters state DEGEG_SUSPENDED if it was in DEREG_NO_IMSI
           * So I have to tae careto enter NO_IMSI again.
           */

          /*
           * GRR is switched off
           */
          kern_gmmrr_disable();
          kern_mm_activate_mm ();

          if (gmm_data->sim_gprs_invalid)
          {
            SET_STATE (KERN, KERN_GMM_NULL_NO_IMSI);
#ifdef FF_EM_MODE
            EM_GMM_SET_STATE(KERN_GMM_NULL_NO_IMSI);
#endif
          }
          else
          {
            SET_STATE (KERN, KERN_GMM_NULL_IMSI );
#ifdef FF_EM_MODE
            EM_GMM_SET_STATE(KERN_GMM_NULL_IMSI );
#endif
            /*
             * MM is switch on as GSM only
             */
            
            if ( GMM_MM_REG_NORMAL_SERVICE EQ GET_STATE(MM) )
            {
              kern_gmmreg_attach_cnf(GMMREG_AT_IMSI);
            }
          }
          
          GMM_RETURN;
        case GMMREG_CLASS_CG:
              if (GMM_MM_DEREG NEQ GET_STATE(MM))
              {
                /*
                 * You are entering dangersome area
                 */
                TRACE_ERROR("signal cell ind, DEREG_NO_IMSI, class CG not covered!"); /* code_cover_not_relevant */

                if (kern_lai_changed() )  /* code_cover_not_relevant */
                {
                  SET_STATE ( KERN, KERN_GMM_DEREG_SUSPENDING ); /* code_cover_not_relevant */
                  kern_gmmrr_suspend (GMMRR_NOT_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_LAU); /* code_cover_not_relevant */
                }
                else /* code_cover_not_relevant */
                {
                  kern_imsi_detach_der_susp(); /* code_cover_not_relevant */
                }
                GMM_RETURN; /* code_cover_not_relevant */
              }
              break;
          /* NO break;
           * goto LABEL cell_der
           */  
        default:
          /* NO return
           * goto LABEL cell_der
           */  
          break;
      }
      /* gmmrr_cell_res */
      /* NO break;*/
      /*
       * LABEL CELL_DER
       */

    case KERN_GMM_DEREG_PLMN_SEARCH:
    case KERN_GMM_DEREG_LIMITED_SERVICE:
    case KERN_GMM_DEREG_NO_CELL_AVAILABLE:
    case KERN_GMM_DEREG_ATTEMPTING_TO_ATTACH:
   
      if (MODE_MAN EQ gmm_data->kern.attach_cap.network_selection_mode
        /* START PATCH UBUOB ISSUE 8784 */
        && gmm_data->kern.attach_cap.gmmreg
        /* END PATCH UBUOB ISSUE 8784 */
        && GMMRR_SERVICE_LIMITED == gmm_data->kern.sig_cell_info.gmm_status )
      {
        TRACE_0_INFO ("forbidden ignored in manual mode");
        gmm_data->kern.sig_cell_info.gmm_status = GMMRR_SERVICE_FULL;        
      }
   
      switch (gmm_data->kern.sig_cell_info.net_mode)
      {
        case GMMRR_NET_MODE_II:                 
          if (kern_lau_needed())
          {
            SET_STATE ( KERN, KERN_GMM_DEREG_SUSPENDING );
#ifdef FF_EM_MODE
            EM_GMM_SET_STATE(KERN_GMM_DEREG_SUSPENDING );
#endif
            kern_gmmrr_suspend (GMMRR_NOT_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_LAU);
            GMM_RETURN;

          }
          break;
        case GMMRR_NET_MODE_III:
          switch (gmm_data->kern.attach_cap.mobile_class)
          {
            case GMMREG_CLASS_BG:
              if (GMM_MM_DEREG NEQ GET_STATE(MM))
              {
                if (kern_lau_needed())
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
              break;
            default:
              break;
          }
          break;
        default:
          break;  
      }
      switch (GET_STATE(KERN))
      {
        case KERN_GMM_DEREG_NO_IMSI:
          /*
           * NO attach if NO IMSI
           * 44.2.1.2.4
           */
          if (gmm_data->sim_gprs_invalid)
          {
            SET_STATE (KERN, KERN_GMM_NULL_NO_IMSI);
#ifdef FF_EM_MODE
            EM_GMM_SET_STATE(KERN_GMM_NULL_NO_IMSI);
#endif
            kern_mm_imsi_detach_ind ( GMMCS_SIM_REMOVED, MMGMM_PERFORM_DETACH,
                                      gmm_data->kern.detach_cap.detach_type); /* TCS 2.1 */
          }
          GMM_RETURN;
        case KERN_GMM_DEREG_ATTEMPTING_TO_ATTACH:
          if (
            (GMMCS_RETRY_IN_NEW_CELL NEQ gmm_data->kern.detach_cap.error_cause /* TCS 2.1 */
              && !kern_ra_crossed())
            || !kern_cell_changed()
          )
          /*  
           * with cause 'Retry upon entry into a new cell', GPRS attach shall be
           * performed when a new cell is entered.
           */
          {
            /*
             * wait untill timer expires
             * 44.2.1.2.8
             */
            T_TIME t3311_value=0;
            T_TIME t3302_value=0;
            if (
              (VSI_OK EQ vsi_t_status ( GMM_handle ,  kern_T3311, &t3311_value )
              && t3311_value>0)
            ||
              (VSI_OK EQ vsi_t_status ( GMM_handle ,  kern_T3302, &t3302_value )
              && t3302_value>0)
            )
            {
              GMM_RETURN;
            }
            else
            /* 
             * if RAU has crossed or rau has changed timer are stopped. attach is needed
             */
            {
              /* both timers stopped, so restart T3311 to make sure we will exit this state */
              TRACE_EVENT ( "Info: START: T3311 not again" );
              SET_STATE(GU,GU_UPDATE_NEEDED);
              //vsi_t_start ( GMM_handle ,  kern_T3311, gmm_data->kern.t3311_val );
              break;
            }
          }
          break;
        default:
          break;
      }

      /*
       * GMM enters state DEGEG_SUSPENDED if it was in DEREG_NO_IMSI
       * So I have to take care to enter NO_IMSI again.
       */
   
      if (gmm_data->sim_gprs_invalid)
      {
        if (kern_lau_needed() )
        {
          SET_STATE ( KERN, KERN_GMM_DEREG_SUSPENDING );
#ifdef FF_EM_MODE
          EM_GMM_SET_STATE(KERN_GMM_DEREG_SUSPENDING );
#endif
          kern_gmmrr_suspend (GMMRR_NOT_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_LAU);
        }
        else
        {
          SET_STATE (KERN, KERN_GMM_NULL_NO_IMSI);
#ifdef FF_EM_MODE
          EM_GMM_SET_STATE(KERN_GMM_NULL_NO_IMSI);
#endif
          kern_mm_imsi_detach_ind ( GMMCS_SIM_REMOVED, MMGMM_PERFORM_DETACH,
                                    gmm_data->kern.detach_cap.detach_type  ); /* TCS 2.1 */
        }
        GMM_RETURN;
      }

      /*
       * ATTACH required
       */
      switch ( gmm_data->kern.sig_cell_info.gmm_status )
      {
        case GMMRR_SERVICE_NONE:
          kern_gmmreg_detach (GMMREG_DT_GPRS,
                        GMMCS_NO_SERVICE, /* TCS 2.1 */
                        GMMREG_SEARCH_NOT_RUNNING,
                        GMMCS_NO_SERVICE);
          SET_STATE(KERN,KERN_GMM_DEREG_NO_CELL_AVAILABLE);
#ifdef FF_EM_MODE
          EM_GMM_SET_STATE(KERN_GMM_DEREG_NO_CELL_AVAILABLE);
#endif
          break;
        case GMMRR_SERVICE_FULL:
          if (gmm_data->kern.attach_cap.enter_attempting_to_update_after_lau)
          /*
           * R&S vtest case 44.2.3.2.7 wants GMM to wait with RAU untill timer 
           * is timed out afe5 5th attempt
           */
          {
            gmm_data->kern.attach_cap.enter_attempting_to_update_after_lau=FALSE;
            SET_STATE(KERN,KERN_GMM_DEREG_ATTEMPTING_TO_ATTACH);
#ifdef FF_EM_MODE
            EM_GMM_SET_STATE(KERN_GMM_DEREG_ATTEMPTING_TO_ATTACH);
#endif
            GMM_RETURN;
          }

          /*
           * MSC: 3.7.2.2.2.1.2 Access allowed
           * MSC: 3.7.2.2.2.1.2.2 Network operates in Mode II or III 
           *
           * call ATTACH procedure
           */
          kern_attach_reset();
          kern_attach ();

          break;
        default:
        case GMMRR_SERVICE_LIMITED:
          kern_gmmreg_detach (GMMREG_DT_GPRS,
                        GMMCS_LIMITED_SERVICE, /* TCS 2.1 */
                        GMMREG_SEARCH_NOT_RUNNING,
                        GMMCS_LIMITED_SERVICE);
          SET_STATE ( KERN, KERN_GMM_DEREG_LIMITED_SERVICE );
#ifdef FF_EM_MODE
          EM_GMM_SET_STATE(KERN_GMM_DEREG_LIMITED_SERVICE );
#endif
          break;
      }
      break;
    case KERN_GMM_DEREG_INITIATED:
      /************************************************************************
       * MSC: 3.9 GPRS detach procedure
       *
       * MCS: 3.9.3 Abnormal cases
       * MSC: 3.9.3.5 e) Change of cell into a new RA
       *                 (before a DETACH ACCEPT message has been received)
       ************************************************************************/
      switch ( gmm_data->kern.sig_cell_info.gmm_status )
      {
        case GMMRR_SERVICE_LIMITED:
        case GMMRR_SERVICE_NONE:
        default:
          /*
           * Local detach, because normal detach does not work
           */
          if (GMMREG_DT_POWER_OFF EQ gmm_data->kern.detach_cap.detach_type)
          {
            kern_local_detach( GMMCS_INT_PROTOCOL_ERROR, FALSE, GMM_LOCAL_DETACH_PROC_POWER_OFF); /* TCS 2.1 */
          }
          else
          {
             gmm_data->kern.detach_cap.detach_type = GMMREG_DT_GPRS;
             kern_local_detach( GMMCS_INT_PROTOCOL_ERROR, FALSE, GMM_LOCAL_DETACH_PROC_DISABLE); /* TCS 2.1 */
          }
          break;
        case GMMRR_SERVICE_FULL:
          if (kern_rai_changed()
          || GU_UPDATE_NEEDED == GET_STATE(GU))
          {
            gmm_data->kern.whilst_detach.rau = TRUE;
            vsi_t_stop ( GMM_handle , kern_T3321);
            kern_rau ();
          } 
          break;
      }
      break;

    case KERN_GMM_REG_IMSI_DETACH_INITIATED:
      /************************************************************************
       * MSC: 3.9 GPRS detach procedure
       *
       * MCS: 3.9.3 Abnormal cases
       * MSC: 3.9.3.5 e) Change of cell into a new RA
       *                 (before a DETACH ACCEPT message has been received)
       ************************************************************************/
      switch ( gmm_data->kern.sig_cell_info.gmm_status )
      {
        case GMMRR_SERVICE_NONE:
          /*
           * Local detach, because normal detach does not work
           */
           gmm_data->kern.detach_cap.detach_type = GMMREG_DT_IMSI;
           kern_local_detach( GMMCS_INT_PROTOCOL_ERROR, FALSE,  /* TCS 2.1 */
                  GMM_LOCAL_DETACH_PROC_NOTHING);
           SET_STATE (KERN, KERN_GMM_REG_NO_CELL_AVAILABLE);

           break;
        default:
        case GMMRR_SERVICE_LIMITED:
          /*
           * Local detach, because normal detach does not work
           */
           gmm_data->kern.detach_cap.detach_type = GMMREG_DT_IMSI;
           kern_local_detach( GMMCS_INT_PROTOCOL_ERROR, FALSE,  /* TCS 2.1 */
                  GMM_LOCAL_DETACH_PROC_NOTHING);
           SET_STATE (KERN, KERN_GMM_REG_LIMITED_SERVICE);
           break;
        case GMMRR_SERVICE_FULL:
          if ( kern_rai_changed()
          || GU_UPDATE_NEEDED EQ GET_STATE(GU))
          {
            gmm_data->kern.whilst_detach.rau = TRUE;
            kern_rau ();
          } 
          break;
      }
      break;
    
    case KERN_GMM_REG_INITIATED:
      if (MODE_MAN EQ gmm_data->kern.attach_cap.network_selection_mode
        /* START PATCH UBUOB ISSUE 8784 */
        && gmm_data->kern.attach_cap.gmmreg
        /* END PATCH UBUOB 8784 */
        && GMMRR_SERVICE_LIMITED == gmm_data->kern.sig_cell_info.gmm_status)
      {
        TRACE_0_INFO ("forbidden ignored in manual mode");
        gmm_data->kern.sig_cell_info.gmm_status = GMMRR_SERVICE_FULL;
      }

      switch (gmm_data->kern.sig_cell_info.net_mode)
      {
        case GMMRR_NET_MODE_II:
          if (kern_lau_needed())
          {
            SET_STATE ( KERN, KERN_GMM_DEREG_SUSPENDING );
#ifdef FF_EM_MODE
            EM_GMM_SET_STATE(KERN_GMM_DEREG_SUSPENDING );
#endif
            kern_gmmrr_suspend (GMMRR_NOT_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_LAU);
            GMM_RETURN;

          }
          break;
        case GMMRR_NET_MODE_III:
          switch (gmm_data->kern.attach_cap.mobile_class)
          {
            case GMMREG_CLASS_BG:
              if (GMM_MM_DEREG NEQ GET_STATE(MM))
              {
                if (kern_lau_needed() )
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
              break;
            default:
              break;
          }
          break;
        default:
          break;  
      }


      /*
       * ATTACH required
       */
      switch ( gmm_data->kern.sig_cell_info.gmm_status )
      {
        case GMMRR_SERVICE_FULL:
          if ( kern_ra_crossed() ) 
          /* 
           * ra changed 
           */
          {
            /*
             * MSC: 3.8.1.4.6. f) Change of cell into a new RA 
             *          (i.e. ATTACH ACCEPT or ATTACH REJECT not yet received)
             *
             * The GPRS attach procedure is aborted and re-initiated immediately.
             *
             * <R.GMM.AGABNORM.M.010>
             */
            kern_attach_reset();
            kern_attach ();
          } 
          else
          /*
           * ra not changed
           */
          /* MSC: 3.8.1.4.5 e) Change of cell within the same RA
           *
           * The GPRS attach procedure is continued.
           * <R.GMM.AGABNORM.M.029>
           */
          {
            if (gmm_data->kern.attach_cap.t3310_value>0)
            /*
             * t3310 has been stopped due to GMMRR_CR_IND
             */
            {
              TRACE_EVENT ( "Info: START: continue T3310 again" );
              /* START PATCH UBUOB ISSUE 8742 */
              sig_kern_rdy_start_timer_req ( kern_T3310, gmm_data->kern.t3310_val );
              /*vsi_t_start ( GMM_handle ,  kern_T3310, gmm_data->kern.t3310_val );*/
              /* END PATCH UBUOB 8742 */
            }
          }
          break;
        case GMMRR_SERVICE_NONE:
          SET_STATE(KERN,KERN_GMM_DEREG_NO_CELL_AVAILABLE);
#ifdef FF_EM_MODE
          EM_GMM_SET_STATE(KERN_GMM_DEREG_NO_CELL_AVAILABLE);
#endif
          break;

        default:
        case GMMRR_SERVICE_LIMITED:
          SET_STATE ( KERN, KERN_GMM_DEREG_LIMITED_SERVICE );
#ifdef FF_EM_MODE
          EM_GMM_SET_STATE(KERN_GMM_DEREG_LIMITED_SERVICE );
#endif
          break;
      }
      gmm_data->kern.attach_cap.t3310_value=0;
      break;
    case KERN_GMM_REG_NO_CELL_AVAILABLE:
      switch (gmm_data->kern.sig_cell_info.net_mode)
      {
        case GMMRR_NET_MODE_II:
          if (kern_lau_needed() )
          {
            SET_STATE ( KERN, KERN_GMM_REG_SUSPENDING );
#ifdef FF_EM_MODE
            EM_GMM_SET_STATE(KERN_GMM_REG_SUSPENDING );
#endif
            kern_llgmm_suspend(LLGMM_CALL);
            kern_gmmrr_suspend (GMMRR_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_LAU);
            GMM_RETURN;

          }
          break;
        case GMMRR_NET_MODE_III:
          switch (gmm_data->kern.attach_cap.mobile_class)
          {
            case GMMREG_CLASS_BG:
              if (GMM_MM_DEREG NEQ GET_STATE(MM))
              {
                if (kern_lau_needed())
                {
                  SET_STATE ( KERN, KERN_GMM_REG_SUSPENDING );
#ifdef FF_EM_MODE
                  EM_GMM_SET_STATE(KERN_GMM_REG_SUSPENDING );
#endif
                  kern_llgmm_suspend(LLGMM_CALL);
                  kern_gmmrr_suspend (GMMRR_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_LAU);
                }
                else 
                {
                  kern_imsi_detach_reg_susp();
                }
                GMM_RETURN;
              }
              break;
            default:
              break;
          }
          break;
        default:
          break;  
      }

      switch ( gmm_data->kern.sig_cell_info.gmm_status )
      {
        case GMMRR_SERVICE_FULL:
          if ( kern_rai_changed()
          || GU_UPDATE_NEEDED EQ GET_STATE(GU) ) 
          {
             kern_rau ();
          }
          else
          {
            kern_enter_reg_normal_upon_gmmrr_cell_ind();
          }
          break;
        case GMMRR_SERVICE_NONE:          
          if (kern_lau_needed() )
          {
            SET_STATE ( KERN, KERN_GMM_REG_SUSPENDING );
#ifdef FF_EM_MODE
            EM_GMM_SET_STATE(KERN_GMM_REG_SUSPENDING );
#endif
            kern_llgmm_suspend(LLGMM_CALL);
            kern_gmmrr_suspend (GMMRR_NOT_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_LAU);
            GMM_RETURN;

          }
          SET_STATE(KERN,KERN_GMM_REG_NO_CELL_AVAILABLE);
#ifdef FF_EM_MODE
          EM_GMM_SET_STATE(KERN_GMM_REG_NO_CELL_AVAILABLE);
#endif
          break;
        default:
        case GMMRR_SERVICE_LIMITED:
          SET_STATE ( KERN, KERN_GMM_REG_LIMITED_SERVICE );
#ifdef FF_EM_MODE
          EM_GMM_SET_STATE(KERN_GMM_REG_LIMITED_SERVICE );
#endif
          kern_llgmm_suspend ( LLGMM_LIMITED );
          break;
      }
      break;

    case KERN_GMM_REG_LIMITED_SERVICE:

      switch (gmm_data->kern.sig_cell_info.net_mode)
      {
        case GMMRR_NET_MODE_II:
          if (kern_lau_needed() )
          {
            SET_STATE ( KERN, KERN_GMM_REG_SUSPENDING );
#ifdef FF_EM_MODE
            EM_GMM_SET_STATE(KERN_GMM_REG_SUSPENDING );
#endif
            kern_gmmrr_suspend (GMMRR_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_LAU);
            GMM_RETURN;

          }
          break;
        case GMMRR_NET_MODE_III:
          switch (gmm_data->kern.attach_cap.mobile_class)
          {
            case GMMREG_CLASS_BG:
              if (GMM_MM_DEREG NEQ GET_STATE(MM))
              {
                if (kern_lau_needed() )
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
                GMM_RETURN;
              }
              break;
            default:
              break;
          }
          break;
        default:
          break;  
      }

      switch ( gmm_data->kern.sig_cell_info.gmm_status )
      {
        case GMMRR_SERVICE_FULL:
          if ( kern_rai_changed()
          || GU_UPDATE_NEEDED EQ GET_STATE(GU) ) 
          { 
            kern_rau ();
          }
          else
          {
            kern_enter_reg_normal_upon_gmmrr_cell_ind();
          }
          break;
        case GMMRR_SERVICE_NONE:
          SET_STATE(KERN,KERN_GMM_REG_NO_CELL_AVAILABLE);
#ifdef FF_EM_MODE
          EM_GMM_SET_STATE(KERN_GMM_REG_NO_CELL_AVAILABLE);
#endif
          break;
        default:
        case GMMRR_SERVICE_LIMITED:
          /* 
           * stay in limited service 
           */
          break;
      }
      break;
    case KERN_GMM_REG_SUSPENDED:
      /* Patch HM 02-Aug-01 >>>
      
       * MMGMM_CELL_SELECTED is used by MM if there was a cell selection 
       * during a location updating procedure and the location area did 
       * not change. The remote controlled updating continues in this case, 
       * as there cannot be any change of network mode if the location 
       * area identification is not changed.
       * If MMGMM_WAIT_FOR_UPDATE is used, MM waits for the update decision
       * of GMM. This is the case in this GMM state if the location area has 
       * changed, maybe the network mode has changed also to I and GMM decides 
       * to continue the procedure with an combinded attach. 
       * Although this patch has still to be checked carefully, the unpatched 
       * code leaded to the situation that GRR was resumed and
       * the GSM updating procedure continued, which will cause a phone crash.
       */
      if ((!sig_cell_info->gmmrr_cell_ind_received
         && MMGMM_WAIT_FOR_UPDATE EQ sig_cell_info->mm_status)
      || (GMM_MM_REG_UPDATE_NEEDED EQ GET_STATE(MM))
       )
      /*
       * TACTIVATE has expied so we are in a deadlock situaton and MM waits for 
       * location update request
       */
      {
        switch (gmm_data->kern.sig_cell_info.net_mode)
        {
          case GMMRR_NET_MODE_II:  
            kern_resume_grr_reg();
            break;
          case GMMRR_NET_MODE_III:
            switch (gmm_data->kern.attach_cap.mobile_class)
            {
              case GMMREG_CLASS_BC:
              case GMMREG_CLASS_B:
                kern_resume_grr_reg();
                break;
              default:
                break;
            }
            break;
          default:
            break;  
        }
      }
      else
      {
        TRACE_0_INFO("cell_ind ignored");
      }
      break;

    case KERN_GMM_REG_RESUMING:
      if (GMM_RAU EQ gmm_data->kern.attach_cap.rau_initiated 
      && !(kern_ra_crossed() || kern_rai_changed()) 
      && !gmm_data->kern.attach_cap.enter_attempting_to_update_after_lau)
      {
        /* RAU is aborted */
        TRACE_0_INFO("RAU aborted");
         /* GMM2410 */
        gmm_data->kern.attach_cap.rau_initiated = GMM_NO_RAU;
        kern_local_attach ( LOCAL_TLLI, OLD_TLLI );
      }


      switch (gmm_data->kern.sig_cell_info.net_mode)
      {
        case GMMRR_NET_MODE_I:
           /*
            * kern_enter_reg_normal_upon_gmmrr_cell_ind();
            * GMM1080
            */
           break;
        case GMMRR_NET_MODE_II:
          switch (gmm_data->kern.attach_cap.attach_type)
          {
            case GMMREG_AT_IMSI:
            case GMMREG_AT_COMB:                    
              if (kern_lau_needed())
              {
                SET_STATE ( KERN, KERN_GMM_REG_SUSPENDING );
#ifdef FF_EM_MODE
                EM_GMM_SET_STATE(KERN_GMM_REG_SUSPENDING );
#endif
                kern_gmmrr_suspend (GMMRR_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_LAU);
                GMM_RETURN;

              }
              break;
            case GMMREG_AT_NOT_KNOWN:
              switch (gmm_data->kern.detach_cap.detach_type)
              {
                case GMMREG_DT_IMSI:
                  break;
                case GMMREG_DT_COMB:
                case GMMREG_DT_SIM_REMOVED:    
                  if (!kern_is_cell_forbidden())
                  {
                    if (kern_lau_needed() )
                    {
                      SET_STATE ( KERN, KERN_GMM_REG_SUSPENDING );
#ifdef FF_EM_MODE
                      EM_GMM_SET_STATE(KERN_GMM_REG_SUSPENDING );
#endif
                      kern_gmmrr_suspend (GMMRR_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_LAU);
                      GMM_RETURN;

                    }
                    if ( kern_rai_changed() 
                    || gmm_data->kern.attach_cap.enter_attempting_to_update_after_lau ) 
                    {
                      break;
                    }
                    else
                    {
                      kern_llgmm_resume ();
                      kern_detach();
                    }
                  }
                  else
                  {
                    kern_local_detach(GMMCS_INT_NOT_PRESENT,  /* TCS 2.1 */
                    FALSE, GMM_LOCAL_DETACH_PROC_ENTER_DEREG);
                  }
                  GMM_RETURN;
                default:
                  break;
              }
              break;
            default:
              break;
          }
          break;
        case GMMRR_NET_MODE_III:
          switch (gmm_data->kern.attach_cap.mobile_class)
          {
            case GMMREG_CLASS_BG:
              if (GMM_MM_DEREG NEQ GET_STATE(MM))
              {
                if (kern_lau_needed())
                {
                  SET_STATE ( KERN, KERN_GMM_REG_SUSPENDING );
#ifdef FF_EM_MODE
                  EM_GMM_SET_STATE(KERN_GMM_REG_SUSPENDING );
#endif
                  kern_gmmrr_suspend (GMMRR_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_LAU);
                }
                else 
                {

                  SET_STATE ( KERN, KERN_GMM_REG_SUSPENDING );
#ifdef FF_EM_MODE
                  EM_GMM_SET_STATE(KERN_GMM_REG_SUSPENDING );
#endif
  

                  /* 
                   * LLC is already suspended
                   */
                  kern_gmmrr_suspend (GMMRR_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_IMSI_DETACH);

                }
                GMM_RETURN;
              }
              break;
            default:
              break;
          }
          break;
        default:
          break;  
      }

      if ( gmm_data->kern.attach_cap.enter_attempting_to_update_after_lau )
      {
        gmm_data->kern.attach_cap.enter_attempting_to_update_after_lau = FALSE;
        SET_STATE (KERN, KERN_GMM_REG_ATTEMPTING_TO_UPDATE);
#ifdef FF_EM_MODE
        EM_GMM_SET_STATE(KERN_GMM_REG_ATTEMPTING_TO_UPDATE);
#endif
        GMM_RETURN;
      }

      /* 
       * Added for the Nortel TC 1.4.2.2.2.1 - RAU Procedure while - MSC 
       * Not Reachable. After the 5th failed attempt, GMM would have set the mode
       * to NMOII. If cell_ind indicates the same cell again for resumption, 
       * preserve the network mode by overwriting the given NMO. 
       */
      if (GMM_NO_RAU EQ gmm_data->kern.attach_cap.rau_initiated
          AND !(kern_ra_crossed() OR kern_rai_changed())
          AND (gmm_data->kern.sig_cell_info.net_mode EQ GMMRR_NET_MODE_I)
          AND (gmm_data->kern.old_sig_cell_info.net_mode EQ GMMRR_NET_MODE_II))
      {
        TRACE_EVENT("NMO : II");
        gmm_data->kern.sig_cell_info.net_mode = GMMRR_NET_MODE_II;
      }

      switch ( gmm_data->kern.sig_cell_info.gmm_status )
      {
        case GMMRR_SERVICE_FULL:

          if ( kern_rai_changed() 
          || GU_UPDATE_NEEDED == GET_STATE(GU) ) 
          {     
            if (gmm_data->kern.attach_cap.enter_attempting_to_update_after_lau)
            /*
             * R&S vtest case 44.2.3.2.7 wants GMM to wait with RAU untill timer 
             * is timed out afe5 5th attempt
             */
            {
              gmm_data->kern.attach_cap.enter_attempting_to_update_after_lau=FALSE;
              SET_STATE(KERN,KERN_GMM_REG_ATTEMPTING_TO_UPDATE);
#ifdef FF_EM_MODE
              EM_GMM_SET_STATE(KERN_GMM_REG_ATTEMPTING_TO_UPDATE);
#endif
              GMM_RETURN;
            }
            else
            {
              kern_rau_susp ();
            }
          }
          else if ( gmm_data->kern.detach_cap.gmmreg )
          /*
           * MO DETACH was requested
           */
          {
             TRACE_0_INFO("Detach Request again from GMM_KERN_DEREG_RESUMING");
             TRACE_EVENT_P2("Netmode %d  MM State %d", gmm_data->kern.sig_cell_info.net_mode, GET_STATE(MM));
             /* Already LLC is in suspended state when we are in KERN_GMM_DEREG_RESUMING 
              * give cell_res so that GRR can transmit */
             kern_gmmrr_stop_waiting_for_transmission();
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
                      if (GMM_MM_DEREG NEQ GET_STATE(MM))
                      { 
                         SET_STATE ( KERN, KERN_GMM_REG_SUSPENDING);
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
          else
          {
             kern_enter_reg_normal_susp();    
          }
          break;
        case GMMRR_SERVICE_NONE:
          SET_STATE(KERN,KERN_GMM_REG_NO_CELL_AVAILABLE);
#ifdef FF_EM_MODE
          EM_GMM_SET_STATE(KERN_GMM_REG_NO_CELL_AVAILABLE);
#endif
          break;
        default:
        case GMMRR_SERVICE_LIMITED:
          kern_enter_reg_limited_susp();
          break;
      }
      break;


    case KERN_GMM_RAU_INITIATED: 
      /*
       * <R.GMM.RNABNORM.M.032>
       */
      if (kern_ra_crossed())
      {
        gmm_data->gu = GU2_NOT_UPDATED;
        kern_sim_gmm_update();
      }
      switch (gmm_data->kern.sig_cell_info.net_mode)
      {
        case GMMRR_NET_MODE_II:
          if (kern_lau_needed())
          {
            SET_STATE ( KERN, KERN_GMM_REG_SUSPENDING );
#ifdef FF_EM_MODE
            EM_GMM_SET_STATE(KERN_GMM_REG_SUSPENDING );
#endif
            kern_llgmm_suspend(LLGMM_CALL);
            kern_gmmrr_suspend ((UBYTE)
              (GMMRR_SERVICE_NONE==gmm_data->kern.sig_cell_info.gmm_status?
                GMMRR_NOT_SUSP_GPRS:GMMRR_SUSP_GPRS), 
                GMMRR_NORMAL_RELEASE,
              GMM_SUSP_LAU);
            GMM_RETURN;

          }
          break;
        case GMMRR_NET_MODE_III:
          switch (gmm_data->kern.attach_cap.mobile_class)
          {
            case GMMREG_CLASS_BG:
              if (GMM_MM_DEREG NEQ GET_STATE(MM))
              {
                if (kern_lau_needed() )
                {
                  SET_STATE ( KERN, KERN_GMM_REG_SUSPENDING );
#ifdef FF_EM_MODE
                  EM_GMM_SET_STATE(KERN_GMM_REG_SUSPENDING );
#endif
                  kern_llgmm_suspend(LLGMM_CALL);
                  kern_gmmrr_suspend ((UBYTE)
                    (GMMRR_SERVICE_NONE==gmm_data->kern.sig_cell_info.gmm_status?
                      GMMRR_NOT_SUSP_GPRS:GMMRR_SUSP_GPRS), 
                      GMMRR_NORMAL_RELEASE,
                    GMM_SUSP_LAU);
                }
                else 
                {
                  kern_imsi_detach_reg_susp();
                }
                GMM_RETURN;
              }

              break;
            default:
              break;
          }
          break;
        default:
          break;  
      }

      switch ( gmm_data->kern.sig_cell_info.gmm_status )
      {
        case GMMRR_SERVICE_FULL:
          if ( kern_ra_crossed()) 
          {
            if (GMM_PERIODIC_RAU EQ gmm_data->kern.attach_cap.rau_initiated)
            {
              kern_rau();
            }
            else
            {
              kern_rau_limited ();
            }
          }
          else 
          {
           /*
            * timer will trigger rau agan
            */
            if (gmm_data->kern.attach_cap.t3310_value>0)
            /*
             * t3310 has been stopped due to GMMRR_CR_IND
             */
            {
              TRACE_EVENT ( "Info: START: continue T3310 again" );
              /* START PATCH UBUOB ISSUE 8742 */
              sig_kern_rdy_start_timer_req ( kern_T3310, gmm_data->kern.t3310_val );
              /*vsi_t_start ( GMM_handle ,  kern_T3310, gmm_data->kern.t3310_val );*/
              /* END PATCH UBUOB 8742 */
            }
          }
          break;
        case GMMRR_SERVICE_NONE:
          SET_STATE(GU,GU_UPDATE_NEEDED);
          if (kern_lau_needed() )
          {
            SET_STATE ( KERN, KERN_GMM_REG_SUSPENDING );
#ifdef FF_EM_MODE
            EM_GMM_SET_STATE(KERN_GMM_REG_SUSPENDING );
#endif
            kern_llgmm_suspend(LLGMM_CALL);
            kern_gmmrr_suspend (GMMRR_NOT_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_LAU);
            GMM_RETURN;
          }
          kern_enter_reg_no_cell_limited();
          break;
        default:
        case GMMRR_SERVICE_LIMITED:
          SET_STATE(GU,GU_UPDATE_NEEDED);
          kern_enter_reg_limited();
          break;
      }
      gmm_data->kern.attach_cap.t3310_value=0;
      break;

    case KERN_GMM_REG_NORMAL_SERVICE:
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE:
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE_MM:
      switch (gmm_data->kern.sig_cell_info.net_mode)
      {
        case GMMRR_NET_MODE_II:
          if (kern_lau_needed() )
          {
            SET_STATE ( KERN, KERN_GMM_REG_SUSPENDING );
#ifdef FF_EM_MODE
            EM_GMM_SET_STATE(KERN_GMM_REG_SUSPENDING );
#endif
            kern_llgmm_suspend(LLGMM_CALL);
            kern_gmmrr_suspend ((UBYTE)
              (GMMRR_SERVICE_NONE==gmm_data->kern.sig_cell_info.gmm_status?
                GMMRR_NOT_SUSP_GPRS:GMMRR_SUSP_GPRS), 
                GMMRR_NORMAL_RELEASE,
              GMM_SUSP_LAU);
            GMM_RETURN;

          }
          break;
        case GMMRR_NET_MODE_III:
          switch (gmm_data->kern.attach_cap.mobile_class)
          {
            case GMMREG_CLASS_BG:
              if (GMM_MM_DEREG NEQ GET_STATE(MM))
              {
                if (kern_lau_needed())
                {
                  SET_STATE ( KERN, KERN_GMM_REG_SUSPENDING );
#ifdef FF_EM_MODE
                  EM_GMM_SET_STATE(KERN_GMM_REG_SUSPENDING );
#endif
                  kern_llgmm_suspend(LLGMM_CALL);
                  kern_gmmrr_suspend ((UBYTE)
                    (GMMRR_SERVICE_NONE==gmm_data->kern.sig_cell_info.gmm_status?
                      GMMRR_NOT_SUSP_GPRS:GMMRR_SUSP_GPRS), 
                      GMMRR_NORMAL_RELEASE,
                    GMM_SUSP_LAU);
                }
                else 
                {
                  kern_imsi_detach_reg_susp();
                }
                GMM_RETURN;
              }
              break;
            default:
              break;
          }
          break;
        default:
          break;  
      }

      switch ( gmm_data->kern.sig_cell_info.gmm_status )
      {
        case GMMRR_SERVICE_FULL:
          if (kern_rai_changed()
          || GU_UPDATE_NEEDED EQ GET_STATE (GU)
          )
          {
            if (kern_cell_changed())
             {
               kern_rau ();
             }
             else
             {
               if (KERN_GMM_REG_ATTEMPTING_TO_UPDATE!=GET_STATE(KERN))
               {
                 kern_gmmrr_stop_waiting_for_transmission();
               }
               TRACE_0_INFO("cell ind ignored");
             }
          }
          else
          {
            if (KERN_GMM_REG_ATTEMPTING_TO_UPDATE!=GET_STATE(KERN))
            {
              kern_gmmrr_stop_waiting_for_transmission();
            }
          }
          break;
        case GMMRR_SERVICE_NONE:
          if (kern_lau_needed() )
          {
            SET_STATE ( KERN, KERN_GMM_REG_SUSPENDING );
#ifdef FF_EM_MODE
            EM_GMM_SET_STATE(KERN_GMM_REG_SUSPENDING );
#endif
            kern_llgmm_suspend(LLGMM_CALL);
            kern_gmmrr_suspend (GMMRR_NOT_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_LAU);
            GMM_RETURN;
          }
          kern_enter_reg_no_cell_limited();
          break;
        default:
        case GMMRR_SERVICE_LIMITED:
          kern_enter_reg_limited();
          break;
      }
            
      break;
    case KERN_GMM_RAU_WAIT_FOR_NPDU_LIST:
      TRACE_ERROR("unexpected state");
      break;
    case KERN_GMM_REG_TEST_MODE:
    case KERN_GMM_REG_TEST_MODE_NO_IMSI:
      TRACE_ERROR("cell change forbidden");
      break;

    case KERN_GMM_NULL_NO_IMSI:
    case KERN_GMM_NULL_NO_IMSI_LIMITED_SERVICE_REQ:
      break;

    case KERN_GMM_NULL_IMSI:
    case KERN_GMM_NULL_IMSI_LIMITED_SERVICE_REQ:
    case KERN_GMM_NULL_PLMN_SEARCH:
      break;
    default:
      switch (gmm_data->kern.sig_cell_info.net_mode)
      {
        case GMMRR_NET_MODE_II:
          if (kern_lau_needed())
          {
            SET_STATE ( KERN, KERN_GMM_DEREG_SUSPENDING );
#ifdef FF_EM_MODE
            EM_GMM_SET_STATE(KERN_GMM_DEREG_SUSPENDING );
#endif
            kern_llgmm_suspend(LLGMM_CALL);
            kern_gmmrr_suspend (GMMRR_NOT_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_LAU);
            GMM_RETURN;

          }

          break;
        case GMMRR_NET_MODE_III:
          switch (gmm_data->kern.attach_cap.mobile_class)
          {
            case GMMREG_CLASS_BG:
              if (GMM_MM_DEREG NEQ GET_STATE(MM))
              {
                if (kern_lau_needed() )
                {
                  SET_STATE ( KERN, KERN_GMM_DEREG_SUSPENDING );
#ifdef FF_EM_MODE
                  EM_GMM_SET_STATE(KERN_GMM_DEREG_SUSPENDING );
#endif
                  kern_llgmm_suspend(LLGMM_CALL);
                  kern_gmmrr_suspend (GMMRR_NOT_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_LAU);
                }
                else 
                {
                  kern_imsi_detach_der_susp();
                }
                GMM_RETURN;
              }
              break;
            default:
              break;
          }
          break;
        default:
          break;  
      }

      TRACE_ERROR( "sig_cell_info unexpected" );
      break;
  }
  GMM_RETURN;
}/*sig_sync_kern_cell_ind*/

/*
+------------------------------------------------------------------------------
| Function    : sig_rx_kern_att_acc_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_RX_KERN_ATT_ACC_IND
|
|         MSC : 3.8 Attach
|
|         MSC : 3.8.1 Normal Attach
|         MSC : 3.8.1.2 GPRS attach accepted by the network
|         MSC : 3.8.1.2.1 MMI-initiated attach accepted
|         MSC : 3.8.1.2.1.1 MMI-initiated attach accepted with implicit P-TMSI reallocation
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_rx_kern_att_acc_ind ( void ) 
{ 
  TRACE_ISIG( "sig_rx_kern_att_acc_ind" );
 
  /*
   * multiple outstanding SIM_AUTHENTICATION_REQ messages
   * for safety reset the counter
   */
  gmm_data->kern.auth_cap.last_auth_req_id = NOT_PRESENT_8BIT;
  switch( GET_STATE( KERN ) )
  {
    case KERN_GMM_DEREG_ATTEMPTING_TO_ATTACH:
    /* START PATCH UBUOB ISSUE 8384 */
      vsi_t_stop (GMM_handle, kern_T3311);
      vsi_t_stop (GMM_handle, kern_T3302);
    /* END PATCH UBUOB ISSUE 8384 */
    case KERN_GMM_REG_NORMAL_SERVICE:
    case KERN_GMM_REG_INITIATED:
    {
      USHORT cause;
      MCAST ( attach_accept, ATTACH_ACCEPT ); /* T_ATTACH_ACCEPT */

      if (attach_accept->routing_area_identification.c_mnc EQ 2) 
      {
        /* Internally we always work with 3-digit MNC */
        attach_accept->routing_area_identification.c_mnc = SIZE_MNC;
        attach_accept->routing_area_identification.mnc[2] = 0xf;
      }

      cause = kern_make_cause (attach_accept->v_gmm_cause,
                               attach_accept->gmm_cause.cause_value); 

#ifndef _TARGET_
      if (memcmp (gmm_data->kern.sig_cell_info.env.rai.plmn.mcc, 
                          attach_accept->routing_area_identification.mcc,
                          SIZE_MCC) != 0
      ||  memcmp (gmm_data->kern.sig_cell_info.env.rai.plmn.mnc, 
                          attach_accept->routing_area_identification.mnc,
                          SIZE_MNC) != 0
      ||  gmm_data->kern.sig_cell_info.env.rai.lac!=attach_accept->routing_area_identification.lac
      ||  gmm_data->kern.sig_cell_info.env.rai.rac!=attach_accept->routing_area_identification.rac)
      {
        TRACE_ERROR ("RAI mismatch");
        vsi_o_ttrace(VSI_CALLER TC_ERROR,"error cell: MCC: %x%x%x, MNC: %x%x, lac %x, rac %x", 
          gmm_data->kern.sig_cell_info.env.rai.plmn.mcc[0],
          gmm_data->kern.sig_cell_info.env.rai.plmn.mcc[1],
          gmm_data->kern.sig_cell_info.env.rai.plmn.mcc[2],
          gmm_data->kern.sig_cell_info.env.rai.plmn.mnc[0],
          gmm_data->kern.sig_cell_info.env.rai.plmn.mnc[1],
          gmm_data->kern.sig_cell_info.env.rai.lac,
          gmm_data->kern.sig_cell_info.env.rai.rac);
        vsi_o_ttrace(VSI_CALLER TC_ERROR,"error ACC: MCC: %x%x%x, MNC: %x%x, lac %x, rac %x", 
          attach_accept->routing_area_identification.mcc[0],
          attach_accept->routing_area_identification.mcc[1],
          attach_accept->routing_area_identification.mcc[2],
          attach_accept->routing_area_identification.mnc[0],
          attach_accept->routing_area_identification.mnc[1],
          attach_accept->routing_area_identification.lac,
          attach_accept->routing_area_identification.rac);
      }
#endif /* _TARGET_ */

      kern_gmmrr_attach_finished();


      gmm_data->kern.attach_cap.sms_radio_priority_level
        = attach_accept->radio_priority.radio_priority_level_value;
      kern_attach_accept (
        &attach_accept->result_gmm,
        &attach_accept->force_to_standby,
        &attach_accept->rau_timer,
        &attach_accept->routing_area_identification,
        attach_accept->v_p_tmsi_signature,
        &attach_accept->p_tmsi_signature,
        attach_accept->v_ready_timer,
        &attach_accept->ready_timer,
        attach_accept->v_gmobile_identity,
        &attach_accept->gmobile_identity,
        attach_accept->v_mobile_identity,
        &attach_accept->mobile_identity,
        FALSE,
        NULL,
        cause,
        attach_accept->v_eqv_plmn_list, /* TCS 2.1 */
        &attach_accept->eqv_plmn_list, /* TCS 2.1 */
        attach_accept->v_t3302,
        &attach_accept->t3302,
        attach_accept->v_cell_notification
      );
      break;
    }
    default:
      sig_kern_rx_gmm_status ( ERRCS_TYPE_INCOMPATIBLE);
      TRACE_ERROR( "SIG_RX_KERN_ATT_ACC_IND unexpected" );
      break;
  }
} /* sig_rx_kern_att_acc_ind() */



/*
+------------------------------------------------------------------------------
| Function    : sig_rx_kern_att_rej_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_RX_KERN_ATT_REJ_IND
|
|               <R.GMM.PATTREJ.M.001>, <R.GMM.PATTREJ.M.002>
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_rx_kern_att_rej_ind (  void ) 
{ 
  TRACE_ISIG( "sig_rx_kern_att_rej_ind" );

  /*
   * multiple outstanding SIM_AUTHENTICATION_REQ messages
   * for safety reset the counter
   */
  gmm_data->kern.auth_cap.last_auth_req_id = NOT_PRESENT_8BIT;

  switch( GET_STATE( KERN ) )
  {
    case KERN_GMM_REG_INITIATED:
    {
      USHORT cause;
      MCAST (attach_reject, ATTACH_REJECT); /* T_ATTACH_REJECT */

      /* In ATTACH REJECT, the cause is a mandatory IE */
      cause = kern_make_cause (TRUE, attach_reject->gmm_cause.cause_value);

      gmm_data->kern.detach_cap.error_cause = cause;

#ifdef REL99     
      if (attach_reject->v_t3302) /* TCS 4.0 */
      { /* TCS 4.0 */
        gmm_data->kern.t3302_val = rdy_get_timer( (T_rau_timer *)&attach_reject->t3302 );   /* TCS 4.0 */
      } /* TCS 4.0 */
#endif      

      /*
       * <R.GMM.PATTREJ.M.001>, <R.GMM.PATTREJ.M.002>
       */
      vsi_t_stop  ( GMM_handle, kern_T3310);  
        
      switch ( cause )
      {
        case GMMCS_ILLEGAL_ME: /* TCS 2.1 */
        case GMMCS_ILLEGAL_MS: /* TCS 2.1 */
        case GMMCS_GPRS_NOT_ALLOWED: /* TCS 2.1 */
        case GMMCS_PLMN_NOT_ALLOWED: /* TCS 2.1 */
        case GMMCS_LA_NOT_ALLOWED: /* TCS 2.1 */
        case GMMCS_ROAMING_NOT_ALLOWED: /* TCS 2.1 */
        case GMMCS_GSM_GPRS_NOT_ALLOWED:  /* TCS 2.1 */
        case GMMCS_GPRS_NOT_ALLOWED_IN_PLMN: /* TCS 2.1 */

#ifdef REL99     
        case GMMCS_NO_SUITABLE_CELL_IN_LA: /*Cause #15*/ /* TCS 4.0 */
#endif

          kern_gmmrr_attach_finished();

          kern_err_cause ( cause, FALSE );
          break;
       
        default:
          /* 
           * <R.GMM.AGREJECT.M026>
           * MSC: 3.8.1.4.4. d) ATTACH REJECT, other causes than 
           * #3, #6, #7, #8, #11, #12, or #13
           */
          kern_aac(cause);
          break;
      }
      break;
    }
    default:
      sig_kern_rx_gmm_status ( ERRCS_TYPE_INCOMPATIBLE);
      TRACE_ERROR( "SIG_RX_KERN_ATT_REJ_IND unexpected" );
      break;
  }
} /* sig_rx_kern_att_rej_ind() */

/*
+------------------------------------------------------------------------------
| Function    : kern_d_detach_default
+------------------------------------------------------------------------------
| Description : The function kern_d_detach_default()
|       
|               This procedure is used for default branch in
|               D_DETACH_REQUEST procedure
|
| Parameters  : detach_type from d_detach_request - AIR message
|               cause GMM cause derived from d_detach_request
|
+------------------------------------------------------------------------------
*/
LOCAL void kern_local_d_detach_default (UBYTE detach_type, USHORT cause)
{ 
  GMM_TRACE_FUNCTION( "kern_local_d_detach_default" );
  switch ( detach_type )
  {
    case DT_RE_ATTACH:
      /*
       * If the MS is attached for GPRS and non-GPRS services and the network
       * operates in network operation mode I, then if in the MS the timer T3212 is 
       * not already running, the timer T3212 shall be set to its initial value and 
       * restarted. (Version 6.7.0)
       *
       * <R.GMM.DNACM.M.039>
       */
      if(GMM_MM_DEREG NEQ GET_STATE(MM)
      && GMMRR_NET_MODE_I EQ gmm_data->kern.sig_cell_info.net_mode)
      {
        kern_mm_start_t3212();
      }
      /* 
       * <R.DNACM.M.035> 
       */
      gmm_data->kern.detach_cap.detach_type = GMM_DT_RE_ATTACH;
      kern_local_detach (cause, TRUE,  GMM_LOCAL_DETACH_PROC_RE_ATTACH);


      break;
    case DT_IMSI_DETACH:
      /*
       * If the detach type IE indicates 'IMSI detach' or 'reattach required', 
       * then the MS shall ignore the cause code if received (GSM04.08v6f0 4.7.4.2.2)
       */
      /* 
       * reattach for IMSI in NMO I
       */
      gmm_data->kern.detach_cap.detach_type = GMMREG_DT_IMSI;
      if (GMMRR_NET_MODE_I EQ gmm_data->kern.sig_cell_info.net_mode
      && GMMREG_AT_COMB EQ gmm_data->kern.attach_cap.attach_type)
      {
        kern_local_detach( GMMCS_INT_NOT_PRESENT, FALSE, GMM_LOCAL_DETACH_PROC_RAU); /* TCS 2.1 */
      }
      else
      {
        kern_local_detach( GMMCS_INT_NOT_PRESENT, FALSE, GMM_LOCAL_DETACH_PROC_ENTER_REG_NORMAL); /* TCS 2.1 */
      }
      break;
        
    default:
      TRACE_ERROR ("unaxpected detach type in AIR message");
      /* NO break;*/
    case DT_NO_RE_ATTACH:
      /* 
       * LABEL DT_NO_REATTACH
       *
       * If the MS is attached for GPRS and non-GPRS services and the network
       * operates in network operation mode I, then if in the MS the timer T3212 is 
       * not already running, the timer T3212 shall be set to its initial value and 
       * restarted. (Version 6.7.0)
       *
       * <R.GMM.DNACM.M.039>
       */
      if(GMM_MM_DEREG NEQ GET_STATE(MM)
      && GMMRR_NET_MODE_I EQ gmm_data->kern.sig_cell_info.net_mode)
      {
        kern_mm_start_t3212();
      }

      vsi_t_stop ( GMM_handle, kern_T3310);       
      vsi_t_stop ( GMM_handle, kern_T3311);       

      if (gmm_data->kern.detach_cap.gmmreg)
      {
         if (GMMREG_DT_IMSI EQ gmm_data->kern.detach_cap.detach_type)
         {
            gmm_data->kern.detach_cap.detach_type = GMMREG_DT_COMB;
         }
         else
         {
            gmm_data->kern.detach_cap.detach_type = GMMREG_DT_GPRS;
         }
      }
      else
      {
         gmm_data->kern.detach_cap.detach_type = GMMREG_DT_GPRS;
      }
      kern_local_detach (cause, TRUE, GMM_LOCAL_DETACH_PROC_ENTER_DEREG);
      /*
       * entering state der is not possible here, because detach accept has to 
       * be sent first. 
       * kern_enter_der();
       *//* GMM9000C */
      break;
  }

  GMM_RETURN;
} /* kern_local_d_detach_default() */



/*
+------------------------------------------------------------------------------
| Function    : sig_rx_kern_det_req_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_RX_KERN_DET_REQ_IND
|
|               Not yet implemented
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_rx_kern_det_req_ind ( void ) 
{ 
  TRACE_ISIG( "sig_rx_kern_det_req_ind" );
  {
    UBYTE detach_type;
    USHORT cause;
    MCAST ( d_detach_request, D_DETACH_REQUEST ); 

    detach_type = d_detach_request->d_detach_type.d_type_of_detach;
    cause = kern_make_cause (d_detach_request->v_gmm_cause, 
                             d_detach_request->gmm_cause.cause_value);

    switch( GET_STATE( KERN ) )
    {
      case KERN_GMM_RAU_INITIATED:
       
        if ( DT_IMSI_DETACH EQ detach_type )
        /* 
         * RAU continued
         */
        {
          /* 
           * <R.GMM.RNABNORM.M.015>, <R.GMM.RNABNORM.M.016> 
           */
          return;
        }
        break;
      case KERN_GMM_REG_INITIATED:
       
        if ( DT_RE_ATTACH EQ detach_type )
        /* 
         * reattach
         */
        {
          /*
           * MSC: 3.8 Attach
           *
           * MSC: 3.8.1 Normal Attach
           * MSC: 3.8.1.4 Abnormal cases
           * MSC: 3.81.4.8 h) Procedure collision
           * MSC: 3.81.4.8.1 Re-attach requested

           * <R.GMM.AGABNORM.M.016>, <R.GMM.AGABNORM.M.017>
           */
          TRACE_0_INFO ("reattach (reset) requested by the network.");
          return;
        }
        else
        {
          kern_gmmrr_attach_finished();
        }
        break;
      
      case KERN_GMM_DEREG_SUSPENDING:
        if ( DT_RE_ATTACH EQ detach_type )
        /* 
         * reattach
         */
        {
          TRACE_0_INFO ("it seems that reattach (reset) has been requested by the network TWICE.");
          TRACE_0_INFO ("DETACH message will be inored.");
          return;
        }
        break;
      default:
        break;
    }
    /* 
     * LABEL DET_REQ_IND
     */
    /* MSC: 3.8 Attach
     *
     * MSC: 3.8.1 Normal Attach
     * MSC: 3.8.1.4 Abnormal cases
     * MSC: 3.81.4.8 h) Procedure collision
     * MSC: 3.81.4.8.2 Re-attach not requested
     *
     * <R.GMM.AGABNORM.M.018>, <R.GMM.AGABNORM.M.019>
     */
    
    sig_kern_tx_data_req ( CURRENT_TLLI, U_DETACH_ACCEPT );


    sig_kern_rdy_force_ie_req ( 
      d_detach_request->force_to_standby.force_to_standby_value,FALSE);
    
    switch (detach_type)
    {
      case DT_IMSI_DETACH:
         kern_local_d_detach_default(detach_type, cause);

        /*
         * If the detach type IE indicates 'IMSI detach' or 'reattach required', 
         * then the MS shall ignore the cause code if received (GSM04.08v6f0 4.7.4.2.2)
         */
        return;
      case DT_RE_ATTACH :
        kern_local_d_detach_default(detach_type, cause);
        /*
         * If the detach type IE indicates 'IMSI detach' or 'reattach required', 
         * then the MS shall ignore the cause code if received (GSM04.08v6f0 4.7.4.2.2)
         */
        return;
      default:
        break;
    }

    switch ( cause )
    {
      case GMMCS_IMSI_UNKNOWN: /* TCS 2.1 */
      case GMMCS_ILLEGAL_ME: /* TCS 2.1 */
      case GMMCS_ILLEGAL_MS: /* TCS 2.1 */
      case GMMCS_GPRS_NOT_ALLOWED: /* TCS 2.1 */
      case GMMCS_PLMN_NOT_ALLOWED: /* TCS 2.1 */
      case GMMCS_LA_NOT_ALLOWED: /* TCS 2.1 */
      case GMMCS_ROAMING_NOT_ALLOWED: /* TCS 2.1 */
      case GMMCS_GSM_GPRS_NOT_ALLOWED: /* TCS 2.1 */
      case GMMCS_GPRS_NOT_ALLOWED_IN_PLMN: /* TCS 2.1 */

#ifdef REL99      
      case GMMCS_NO_SUITABLE_CELL_IN_LA: /*Cause #15*/ /* TCS 4.0 */
#endif

        kern_err_cause (cause, TRUE); /* TCS 2.1 */
        break;
      default:
        kern_local_d_detach_default(detach_type, cause);
        break;
    }
  }
} /* sig_rx_kern_det_req_ind() */



/*
+------------------------------------------------------------------------------
| Function    : sig_rx_kern_det_acc_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_RX_KERN_DET_ACC_IND
|
|               MSC: 3.9 GPRS detach procedure
|    
|               MSC: 3.9.1 MS initiated GPRS detach procedure completion 
|                           (without switching off)
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_rx_kern_det_acc_ind () 
{ 
  TRACE_ISIG( "sig_rx_kern_det_acc_ind" );
  /* 
   * detach type has to be normal detach 
   */
  /*
   * multiple outstanding SIM_AUTHENTICATION_REQ messages
   * for safety reset the counter
   */
  gmm_data->kern.auth_cap.last_auth_req_id = NOT_PRESENT_8BIT;

  switch( GET_STATE( KERN ) )
  {
    case KERN_GMM_DEREG_INITIATED:
     /* 
      * timer has to be stopped here becausekern_local_detach will not be called
      */
     vsi_t_stop ( GMM_handle , kern_T3321);

      {
        MCAST ( d_detach_accept, D_DETACH_ACCEPT);
          
        sig_kern_rdy_force_ie_req (
          d_detach_accept->force_to_standby.force_to_standby_value, FALSE);
      }
      if ( gmm_data->kern.detach_cap.gmmreg
      ||  (gmm_data->kern.attach_cap.gmmreg 
          && GMMREG_CLASS_CC EQ gmm_data->kern.attach_cap.mobile_class)
          )
      {
        kern_local_detach ( GMMCS_INT_NOT_PRESENT, FALSE, GMM_LOCAL_DETACH_PROC_DISABLE); /* TCS 2.1 */
      }
      else
      {
        /* 
         * if the mobile is a class BC mobile and it has to detach for GPRS 
         * because of NMO III than it should be able to look for a good GPRS cell.
         * So GRR must be switched on.
         */
        kern_local_detach ( GMMCS_INT_NOT_PRESENT, FALSE, GMM_LOCAL_DETACH_PROC_UNASSIGN); /* TCS 2.1 */
      }
      
      break;
    case KERN_GMM_REG_IMSI_DETACH_INITIATED:
      kern_local_detach ( GMMCS_INT_NOT_PRESENT, FALSE, GMM_LOCAL_DETACH_PROC_ENTER_REG_NORMAL); /* TCS 2.1 */
      {
        MCAST ( d_detach_accept, D_DETACH_ACCEPT);
          
        sig_kern_rdy_force_ie_req (
          d_detach_accept->force_to_standby.force_to_standby_value, FALSE);
      }
      break;      
    default:
      sig_kern_rx_gmm_status ( ERRCS_TYPE_INCOMPATIBLE);
      break;
  }
} /* sig_rx_kern_det_acc_ind() */



/*
+------------------------------------------------------------------------------
| Function    : sig_rx_kern_rau_acc_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_RX_KERN_RAU_ACC_IND
|
|               MSC: 3.10 RAU
|
|               MSC: 3.10.1 Normal and periodic RAU initiation
|               MSC: 3.10.1.2 Normal and periodic RAU accepted by the network
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_rx_kern_rau_acc_ind ( ) 
{ 
  TRACE_ISIG( "sig_rx_kern_rau_acc_ind" );

  if (GMM_NO_RAU EQ gmm_data->kern.attach_cap.rau_initiated)
  {
    gmm_data->kern.attach_cap.rau_initiated = GMM_RAU;
    TRACE_0_INFO ("RAU ACCEPT receied twice");
  }
  /*
   * multiple outstanding SIM_AUTHENTICATION_REQ messages
   * for safety reset the counter
   */
  gmm_data->kern.auth_cap.last_auth_req_id = NOT_PRESENT_8BIT;

  switch( GET_STATE( KERN ) )
  {
    
    /* START PATCH UBUOB ISSUE 8384 */
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE:
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE_MM:
      vsi_t_stop (GMM_handle, kern_T3311);
      vsi_t_stop (GMM_handle, kern_T3302);
      /* NO break; */
    case KERN_GMM_RAU_INITIATED:
    /* END PATCH UBUOB ISSUE 8384 */
      kern_gmmrr_attach_finished();

      /* NO break; */
    case KERN_GMM_REG_NORMAL_SERVICE:
    {
      USHORT cause;
      MCAST (routing_area_update_accept, ROUTING_AREA_UPDATE_ACCEPT);

      if (routing_area_update_accept->routing_area_identification.c_mnc EQ 2) 
      {
        /* Internally we always work with 3-digit MNC */
        routing_area_update_accept->routing_area_identification.c_mnc = SIZE_MNC;
        routing_area_update_accept->routing_area_identification.mnc[2] = 0xf;
      }

      cause = kern_make_cause (routing_area_update_accept->v_gmm_cause,
                               routing_area_update_accept->gmm_cause.cause_value);

#ifndef _TARGET_
      /* 
       * Check whether the ROUTING AREA UPDATE ACCEPT is for the network 
       * MM sent via MMGMM_ACTIVATE_IND, otherwise we want to see this to 
       * make it possible to revise the simulation testcase.
       */

      if (memcmp (gmm_data->kern.sig_cell_info.env.rai.plmn.mcc, 
                          routing_area_update_accept->routing_area_identification.mcc,
                          SIZE_MCC) != 0
      ||  memcmp (gmm_data->kern.sig_cell_info.env.rai.plmn.mnc, 
                          routing_area_update_accept->routing_area_identification.mnc,
                          SIZE_MNC) != 0
      ||  gmm_data->kern.sig_cell_info.env.rai.lac!=routing_area_update_accept->routing_area_identification.lac
      ||  gmm_data->kern.sig_cell_info.env.rai.rac!=routing_area_update_accept->routing_area_identification.rac)
      {
        TRACE_ERROR ("RAI mismatch");
        vsi_o_ttrace(VSI_CALLER TC_ERROR,"error cell: MCC: %x%x%x, MNC: %x%x, lac %x, rac %x", 
          gmm_data->kern.sig_cell_info.env.rai.plmn.mcc[0],
          gmm_data->kern.sig_cell_info.env.rai.plmn.mcc[1],
          gmm_data->kern.sig_cell_info.env.rai.plmn.mcc[2],
          gmm_data->kern.sig_cell_info.env.rai.plmn.mnc[0],
          gmm_data->kern.sig_cell_info.env.rai.plmn.mnc[1],
          gmm_data->kern.sig_cell_info.env.rai.lac,
          gmm_data->kern.sig_cell_info.env.rai.rac);
        vsi_o_ttrace(VSI_CALLER TC_ERROR,"error ACC: MCC: %x%x%x, MNC: %x%x, lac %x, rac %x", 
          routing_area_update_accept->routing_area_identification.mcc[0],
          routing_area_update_accept->routing_area_identification.mcc[1],
          routing_area_update_accept->routing_area_identification.mcc[2],
          routing_area_update_accept->routing_area_identification.mnc[0],
          routing_area_update_accept->routing_area_identification.mnc[1],
          routing_area_update_accept->routing_area_identification.lac,
          routing_area_update_accept->routing_area_identification.rac);
      }

      //assert (plmn_ok); /* Stop stack in simulation to force a "failed". */
#endif /* WIN32 */

      kern_attach_accept (
        &routing_area_update_accept->result_gmm,
        &routing_area_update_accept->force_to_standby,
        &routing_area_update_accept->rau_timer,
        &routing_area_update_accept->routing_area_identification,
        routing_area_update_accept->v_p_tmsi_signature,
        &routing_area_update_accept->p_tmsi_signature,
        routing_area_update_accept->v_ready_timer,
        &routing_area_update_accept->ready_timer,
        routing_area_update_accept->v_gmobile_identity,
        &routing_area_update_accept->gmobile_identity,
        routing_area_update_accept->v_mobile_identity,
        &routing_area_update_accept->mobile_identity,
        routing_area_update_accept->v_receive_n_pdu_number_list,
        &routing_area_update_accept->receive_n_pdu_number_list,
        cause,
        routing_area_update_accept->v_eqv_plmn_list, /* TCS 2.1 */
        &routing_area_update_accept->eqv_plmn_list, /* TCS 2.1 */
        routing_area_update_accept->v_t3302,
        &routing_area_update_accept->t3302,
        routing_area_update_accept->v_cell_notification
      );
      break;
    }
    default:
      sig_kern_rx_gmm_status ( ERRCS_TYPE_INCOMPATIBLE);
      TRACE_ERROR( "SIG_RX_KERN_RAU_ACC_IND unexpected" );
      break;
  }
} /* sig_rx_kern_rau_acc_ind() */



/*
+------------------------------------------------------------------------------
| Function    : sig_rx_kern_rau_rej_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_RX_KERN_RAU_REJ_IND
|
|          MSC: 3.10 RAU
|
|          MSC: 3.10.1 Normal Attach
|          MSC: 3.10.1.3 RAU not accepted by the network
|
|               <R.GMM.PATTREJ.M.001>, <R.GMM.PATTREJ.M.002>
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_rx_kern_rau_rej_ind ( ) 
{ 
  TRACE_ISIG( "sig_rx_kern_rau_rej_ind" );

  /*
   * multiple outstanding SIM_AUTHENTICATION_REQ messages
   * for safety reset the counter
   */
  gmm_data->kern.auth_cap.last_auth_req_id = NOT_PRESENT_8BIT;

  switch( GET_STATE( KERN ) )
  {
    case KERN_GMM_RAU_INITIATED:
    {
      USHORT cause;
      MCAST (routing_area_update_reject, ROUTING_AREA_UPDATE_REJECT);
      
      SET_STATE(GU,GU_UPDATE_NEEDED);

      // The primitive is also sent in kern_aac(), so for the 
      // default case it is sent twice.
      kern_gmmrr_attach_finished();

      /* In ROUTING AREA UPDATE REJECT, the cause is a mandatory IE */
      cause = kern_make_cause (TRUE, 
                               routing_area_update_reject->gmm_cause.cause_value);
#ifdef REL99     
      if (routing_area_update_reject->v_t3302) /* TCS 4.0 */
      {     /* TCS 4.0 */
         gmm_data->kern.t3302_val = rdy_get_timer ( (T_rau_timer *)&routing_area_update_reject->t3302 );   /* TCS 4.0 */
      } /* TCS 4.0 */
#endif

      /*
       * <R.GMM.PATTREJ.M.001>, <R.GMM.PATTREJ.M.002>
       */
      /* 
       * I used T3310 insted of T3330 
       */
      vsi_t_stop  ( GMM_handle, kern_T3310);  
  
      /* 
       * force to standb does only in RAU_REJ exist
       */
      sig_kern_rdy_force_ie_req (
        routing_area_update_reject->force_to_standby.force_to_standby_value,FALSE);


      switch ( cause )
      {
        case GMMCS_ILLEGAL_MS: /* TCS 2.1 */
        case GMMCS_ILLEGAL_ME: /* TCS 2.1 */
        case GMMCS_GPRS_NOT_ALLOWED: /* TCS 2.1 */
        case GMMCS_NO_MS_ID: /* TCS 2.1 */
        case GMMCS_IMPLICIT_DETACHED: /* TCS 2.1 */
        case GMMCS_PLMN_NOT_ALLOWED: /* TCS 2.1 */
        case GMMCS_LA_NOT_ALLOWED: /* TCS 2.1 */

#ifndef REL99
        case GMMCS_ROAMING_NOT_ALLOWED: /* TCS 2.1 */
#endif

        case GMMCS_GPRS_NOT_ALLOWED_IN_PLMN: /* TCS 2.1 */
        case GMMCS_GSM_GPRS_NOT_ALLOWED: /* TCS 2.1 */
        /*case GMMCS_MSC_TEMP_NOT_REACHABLE:*/ /* TCS 2.1 */
          kern_err_cause ( cause, FALSE );
          break;

#ifdef REL99        
        /* Cause #13 and #15 - Set the GPRS update status to 
         * GU3 ROAMING NOT ALLOWED. Reset the rau attempt counter. 
         * Change state to GMM_REGISTERED.LIMITED_SERVICE
         * kern_err_cause() is not called for this cause 
         * because it conflicts with the handling of attach 
         * reject and NW initiated detach request.
         */
        case GMMCS_ROAMING_NOT_ALLOWED: /* TCS 4.0 */
        case GMMCS_NO_SUITABLE_CELL_IN_LA: /* TCS 4.0 */
          gmm_data->gu = GU3_ROAMING_NOT_ALLOWED; /* TCS 4.0 */
          kern_sim_gmm_update(); /*Update SIM*/ /* TCS 4.0 */
          kern_attach_reset(); /* TCS 4.0 */
          SET_STATE(KERN, KERN_GMM_REG_LIMITED_SERVICE); /* TCS 4.0 */
#ifdef FF_EM_MODE
          EM_GMM_SET_STATE(KERN_GMM_REG_LIMITED_SERVICE); /* TCS 4.0 */
#endif
          /* MM has different handling for Cause 13/15 sent in case of 
           * ATTACH REJ and RAU REJ. In case of ATTACH REJ the original 
           * cause value is passed. In case of RAU REJ a new internal
           * cause value is used. 
           */
          if (GMMCS_ROAMING_NOT_ALLOWED EQ cause) /* TCS 4.0 */
          { /* TCS 4.0 */
            kern_mm_attach_rej(GMMCS_ROAMING_NOT_ALLOWED_WITH_RAU_REJ); /* TCS 4.0 */
          }  /* TCS 4.0 */
          else if (GMMCS_NO_SUITABLE_CELL_IN_LA EQ cause) /* TCS 4.0 */
          { /* TCS 4.0 */
            kern_mm_attach_rej(GMMCS_NO_SUITABLE_CELL_IN_LA_WITH_RAU_REJ);  		   /* TCS 4.0 */
          } /* TCS 4.0 */
          else  /* TCS 4.0 */
          { /* TCS 4.0 */
            TRACE_ERROR("Error in cause value !");	   /* TCS 4.0 */
          } /* TCS 4.0 */
          break; /* TCS 4.0 */
#endif        

        default:
          /* 
           * other causes than #3, #6, #7, #9, #10, #11, #12, #13 (and #15 for TCS4)
           */
          kern_aac(cause);
          break;
      }
    }
      break;
    
    default:
      sig_kern_rx_gmm_status ( ERRCS_TYPE_INCOMPATIBLE);
      TRACE_ERROR( "SIG_RX_KERN_RAU_REJ_IND unexpected" );
      break;
  }
} /* sig_rx_kern_rau_rej_ind() */

/*
+------------------------------------------------------------------------------
| Function    : sig_rx_kern_realloc_cmd_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_RX_PTMS_REALLOC_CMD_IND
|
|              
|               - store RAI, P-TMSI
|               - calculate global TLLI based on new P-TMSI
|               - (store P-TMSI signature, if available)
|               - check force to standby IE
|               - send SIM_GMM_UPDATE_REQ to SIM
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_rx_kern_realloc_cmd_ind ( void ) 
{ 
  TRACE_ISIG( "sig_rx_kern_realloc_cmd_ind" );
  
  switch( GET_STATE( KERN ) )
  {
    case KERN_GMM_DEREG_INITIATED:
     /*************************************************************************************
      * MSC: 3.9 GPRS detach procedure
      *
      * MSC: 3.9.3 Abnormal cases
      * MSC: 3.9.3.4 d) Detach and common procedure collision
      * MSC: 3.9.3.4.3 Receipt of remaining common procedures
      *************************************************************************************/
      break;
    case KERN_GMM_RAU_INITIATED:
     /*************************************************************************************
      * MSC: 3.10.1 Normal and periodic RAU initiation
      * MSC: 3.10.1.4 Abnormal cases
      * MSC: 3.10.1.4.8 h) RAU and P-TMSI procedure collision
      *************************************************************************************/
     /* 
      * <R.GMM.RNABNORM.M.017>, <R.GMM.RNABNORM.M.018> 
      */
      break;
    default:
    {
      MCAST ( p_tmsi_reallocation_command, P_TMSI_REALLOCATION_COMMAND );
      
       /* Internally we work always with 3-digit-MNC */
      if (p_tmsi_reallocation_command->routing_area_identification.c_mnc EQ 2)
      {
        p_tmsi_reallocation_command->routing_area_identification.c_mnc = SIZE_MNC;
        p_tmsi_reallocation_command->routing_area_identification.mnc[2] = 0xf;
      }
      
      /*<R.GMM.REACMPLM.M.001>*/
      kern_set_rai ( &p_tmsi_reallocation_command->routing_area_identification );

      sig_kern_rdy_force_ie_req (
        p_tmsi_reallocation_command->force_to_standby.force_to_standby_value,FALSE );
      /* 
       * <R.GMM.REACMPLM.M.003>, <R.GMM.REACMPLM.M.004>,
       * <R.GMM.REACMPLM.M.005>
       *
       * this procedure transmittes the SIM_GMM_UPDATE_REQ  message
       */
      kern_tmsi_negotiated (
          FALSE, /* tmsi is never required */ 
          NULL, 
          TRUE, /* ptmsi is always required */
          &p_tmsi_reallocation_command->gmobile_identity,
          p_tmsi_reallocation_command->v_p_tmsi_signature,
          &p_tmsi_reallocation_command->p_tmsi_signature);
      
      {
        /*
         * <R.GMM.REACMPLM.M.002>
         */
        sig_kern_tx_data_req ( CURRENT_TLLI, P_TMSI_REALLOCATION_COMPLETE ); 
      }
      break;
    }
  }
} /* sig_rx_kern_realloc_cmd_ind() */



/*
+------------------------------------------------------------------------------
| Function    : sig_rx_kern_auth_req_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_RX_KERN_AUTH_REQ_IND
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_rx_kern_auth_req_ind ( ) 
{ 
  TRACE_ISIG( "sig_rx_kern_auth_req_ind" );
  
  switch( GET_STATE( KERN ) )
  {
    case KERN_GMM_DEREG_LIMITED_SERVICE:
    case KERN_GMM_DEREG_NO_CELL_AVAILABLE:
    case KERN_GMM_DEREG_PLMN_SEARCH: 
      /*
       * L. wants to authentiate while attaching
       * 
       * case KERN_GMM_REG_INITIATED:
       */
      break;

    case KERN_GMM_DEREG_ATTEMPTING_TO_ATTACH:
    case KERN_GMM_REG_INITIATED:
      /*
       * <R.GMM.AGCOMMON.I.001>
       * The network may initiate GMM common procedures, e.g. the GMM identification 
       * and GMM authentication and ciphering procedure, depending on the received 
       * information such as IMSI, CKSN, old RAI, P-TMSI and P-TMSI signature.
       */
    case KERN_GMM_REG_NORMAL_SERVICE:
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE:
    case KERN_GMM_REG_NO_CELL_AVAILABLE: 
    case KERN_GMM_REG_LIMITED_SERVICE: 
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE_MM:
    case KERN_GMM_DEREG_INITIATED:
      /*************************************************************************
       * MSC: 3.9 GPRS detach procedure
       *
       * MSC: 3.9.3 Abnormal cases
       * MSC: 3.9.3.4 d) Detach and common procedure collision
       * MSC: 3.9.3.4.3 Receipt of remaining common procedures
       *************************************************************************/

    case KERN_GMM_RAU_INITIATED:
    case KERN_GMM_REG_IMSI_DETACH_INITIATED:
    {
      MCAST (authentication_and_ciphering_request,
        AUTHENTICATION_AND_CIPHERING_REQUEST);
      
      MCAST (authentication_and_ciphering_response,
          AUTHENTICATION_AND_CIPHERING_RESPONSE);

      gmm_data->kern.auth_cap.imeisv_requested     = authentication_and_ciphering_request->imeisv_request.imeisv_request_value;
      gmm_data->kern.auth_cap.a_c_reference_number = authentication_and_ciphering_request->a_c_reference_number.a_c_reference_number_value;
      gmm_data->kern.auth_cap.ciphering_algorithm  = authentication_and_ciphering_request->ciphering_algorithm.type_of_algorithm;
       
      /*start SIM_AUTHENTICATION_REQ if "RAND" and "cksn" are delivered with A&C_req*/
      if (authentication_and_ciphering_request->v_authentication_parameter_rand EQ TRUE)
      {
#ifdef GMM_TCS4
        if(gmm_data->kern.auth_cap.last_auth_req_id NEQ NOT_PRESENT_8BIT)
        {
          /* 
           * problem occurred!!!! 
           */
          TRACE_1_INFO("Authentication problem occurred %d. time.",
                        gmm_data->kern.auth_cap.last_auth_req_id);
          gmm_data->kern.auth_cap.last_auth_req_id++;
        }
        else
        {
          gmm_data->kern.auth_cap.last_auth_req_id = 0;
        }
#endif        
        kern_sim_authentication_req ( authentication_and_ciphering_request
            ->authentication_parameter_rand.rand_value,
          authentication_and_ciphering_request
            ->ciphering_key_sequence_number.key_sequence);
        gmm_data->kern.auth_cap.cksn = authentication_and_ciphering_request->ciphering_key_sequence_number.key_sequence;                
#ifndef GMM_TCS4
        gmm_data->kern.auth_cap.outstanding_count++;
        if (gmm_data->kern.auth_cap.outstanding_count NEQ 1)
        {
           /* problem occurred!!!! */
           TRACE_1_INFO("Authentication problem occurred %d. time.",gmm_data->kern.auth_cap.outstanding_count);
        }
#endif

      }
      else
      {
        PALLOC ( llgmm_assign_req, LLGMM_ASSIGN_REQ );
            llgmm_assign_req->old_tlli = LLGMM_TLLI_INVALID;
            llgmm_assign_req->new_tlli = gmm_data->tlli.current;
            if (NO_KEY EQ gmm_data->kern.auth_cap.cksn)
            {
              memset (llgmm_assign_req->llgmm_kc.key, 0x0, MAX_KC);
            }
            else
            {
              memcpy ( llgmm_assign_req->llgmm_kc.key, gmm_data->kern.auth_cap.kc, MAX_KC );
            }
            llgmm_assign_req->ciphering_algorithm = authentication_and_ciphering_request->ciphering_algorithm.type_of_algorithm;
                
        PSEND ( hCommLLC, llgmm_assign_req );
        
        authentication_and_ciphering_response->a_c_reference_number.a_c_reference_number_value = authentication_and_ciphering_request->a_c_reference_number.a_c_reference_number_value;
        authentication_and_ciphering_response->v_authentication_parameter_sres = FALSE;
        if (IMEISV_REQUESTED EQ authentication_and_ciphering_request->imeisv_request.imeisv_request_value)
        {
          authentication_and_ciphering_response->v_gmobile_identity = TRUE;
          kern_get_imeisv (&authentication_and_ciphering_response->gmobile_identity);
        }
        else
        {
          authentication_and_ciphering_response->v_gmobile_identity = FALSE;     
        }
        sig_kern_tx_data_req (CURRENT_TLLI,AUTHENTICATION_AND_CIPHERING_RESPONSE);
      }
            
      if ( NO_CIPHERING EQ gmm_data->kern.auth_cap.ciphering_algorithm
      ||   gmm_data->config.cipher_on EQ FALSE )
      {
        TRACE_EVENT ("ciphering OFF");
        gmm_data->cipher = FALSE;
        kern_gmmreg_ciphering_ind (CIPH_OFF);
      }
      else
      {
        TRACE_EVENT ("ciphering ON");
        gmm_data->cipher = TRUE;
        kern_gmmreg_ciphering_ind (CIPH_ON);
      } 
      sig_kern_rdy_force_ie_req (authentication_and_ciphering_request
        ->force_to_standby.force_to_standby_value, FALSE);
      break;
    }
    default:
      TRACE_ERROR("Unexpected A&C");
      TRACE_1_INFO ("state: %x", GET_STATE( KERN ));
      break;
  }
} /* sig_rx_kern_auth_req_ind () */

/*
+------------------------------------------------------------------------------
| Function    : sig_rx_kern_auth_rej_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_RX_KERN_CIPH_REQ_IND
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_rx_kern_auth_rej_ind ( ) 
{ 
  TRACE_ISIG( "sig_rx_kern_auth_rej_ind" );
  

  gmm_data->sim_gprs_invalid = TRUE;
  kern_sim_del_locigprs ();
  gmm_data->gu  = GU3_ROAMING_NOT_ALLOWED;
  /*
   * <R.GMM.AUTHREJECT.M.005> 
   */
  kern_sim_gmm_update ();
  kern_mm_auth_rej ();
  
  gmm_data->kern.detach_cap.detach_type = GMMREG_DT_SIM_REMOVED;
  kern_local_detach ( GMMCS_AUTHENTICATION_REJECTED,  /* TCS 2.1 */
     FALSE, GMM_LOCAL_DETACH_PROC_AUTH_FAILED);
 
} /* sig_rx_kern_auth_rej_ind () */

/*
+------------------------------------------------------------------------------
| Function    : sig_rx_kern_id_req_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_RX_KERN_ID_REQ_IND
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_rx_kern_id_req_ind ( ) 
{ 
  TRACE_ISIG( "sig_rx_kern_id_req_ind" );
  
  switch( GET_STATE( KERN ) )
  {
    case KERN_GMM_DEREG_LIMITED_SERVICE:
    case KERN_GMM_DEREG_ATTEMPTING_TO_ATTACH:
    case KERN_GMM_DEREG_NO_CELL_AVAILABLE:
    case KERN_GMM_DEREG_PLMN_SEARCH: 
      break;
 
    case KERN_GMM_REG_INITIATED:
      /*
       * <R.GMM.AGCOMMON.I.001>
       * The network may initiate GMM common procedures, e.g. the GMM identification 
       * and GMM authentication and ciphering procedure, depending on the received 
       * information such as IMSI, CKSN, old RAI, P-TMSI and P-TMSI signature.
       */
      
    case KERN_GMM_DEREG_INITIATED:
     /*************************************************************************
      * MSC: 3.9 GPRS detach procedure
      *
      * MSC: 3.9.3 Abnormal cases
      * MSC: 3.9.3.4 d) Detach and common procedure collision
      * MSC: 3.9.3.4.3 Receipt of remaining common procedures
      *************************************************************************/
    case KERN_GMM_REG_NORMAL_SERVICE:
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE:
    case KERN_GMM_REG_NO_CELL_AVAILABLE: 
    case KERN_GMM_REG_LIMITED_SERVICE: 
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE_MM:
    case KERN_GMM_RAU_INITIATED:
    {
      MCAST (identity_request, IDENTITY_REQUEST); 
      T_gmobile_identity mobile_identity;
      sig_kern_rdy_force_ie_req (
        identity_request->force_to_standby.force_to_standby_value,FALSE);
        
        kern_get_mobile_identity (
          identity_request->identity_type_2.type_of_identity_2,&mobile_identity);
          {
            MCAST (identity_response, IDENTITY_RESPONSE); 
              identity_response->gmobile_identity = mobile_identity;
            sig_kern_tx_data_req (CURRENT_TLLI,IDENTITY_RESPONSE);
          }
      break;
    }
    default:
      break;
  }
} /* sig_rx_kern_id_req_ind () */

/*
+------------------------------------------------------------------------------
| Function    : sig_rx_kern_status_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_RX_KERN_STATUS_IND
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_rx_kern_status_ind ( ) 
{ 
  TRACE_ISIG( "sig_rx_kern_status_ind" );
  
  switch( GET_STATE( KERN ) )
  {
    case KERN_GMM_DEREG_INITIATED:
     /*************************************************************************************
      * MSC: 3.9 GPRS detach procedure
      *
      * MSC: 3.9.3 Abnormal cases
      * MSC: 3.9.3.4 d) Detach and common procedure collision
      * MSC: 3.9.3.4.3 Receipt of remaining common procedures
      *************************************************************************************/
      break;
    default:
      break;
  }
} /* sig_rx_kern_status_ind () */

/*
+------------------------------------------------------------------------------
| Function    : sig_rx_kern_info_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_RX_KERN_INFO_IND
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_rx_kern_info_ind ( ) 
{ 
  TRACE_ISIG( "sig_rx_kern_info_ind" );
  
  switch( GET_STATE( KERN ) )
  {
    case KERN_GMM_DEREG_INITIATED:
     /*************************************************************************
      * MSC: 3.9 GPRS detach procedure
      *
      * MSC: 3.9.3 Abnormal cases
      * MSC: 3.9.3.4 d) Detach and common procedure collision
      * MSC: 3.9.3.4.3 Receipt of remaining common procedures
      *************************************************************************/
      break;
    case KERN_GMM_DEREG_LIMITED_SERVICE:
    case KERN_GMM_DEREG_ATTEMPTING_TO_ATTACH:
    case KERN_GMM_DEREG_NO_CELL_AVAILABLE:
    case KERN_GMM_DEREG_PLMN_SEARCH: 
    case KERN_GMM_REG_INITIATED:
      break;
    case KERN_GMM_REG_NORMAL_SERVICE:
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE:
    case KERN_GMM_REG_NO_CELL_AVAILABLE: 
    case KERN_GMM_REG_LIMITED_SERVICE: 
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE_MM:
    case KERN_GMM_RAU_INITIATED:
    {
      kern_gmmreg_info_ind ();
      break;
    }
    default:
      break;
  }
} /* sig_rx_kern_info_ind () */

/*
+------------------------------------------------------------------------------
| Function    : sig_rx_kern_tst_cmd_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_RX_KERN_TST_CMD_IND
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_rx_kern_tst_cmd_ind ( ) 
{ 
  TRACE_ISIG( "sig_rx_kern_tst_cmd_ind" );
  
  switch( GET_STATE( KERN ) )
  {
    case KERN_GMM_REG_NORMAL_SERVICE:
      if (OP_NORMAL_SIM NEQ gmm_data->kern.sim_op_mode)
      /* Do not try to test if it is OP_NORMAL_SIM, wich has the value 1
       * SIM supports only comparism for test sims in that way: >=0x80
       */
      {
        MCAST(gprs_test_mode_cmd, GPRS_TEST_MODE_CMD);
        
        PALLOC (cgrlc_test_mode_req, CGRLC_TEST_MODE_REQ); /* TCS 2.1 */
#ifdef GMM_TCS4
         /* cgrlc_test_mode_req->v_no_of_pdus = TRUE;*/
#endif
          cgrlc_test_mode_req->no_of_pdus /* TCS 2.1 */
          = gprs_test_mode_cmd->pdu_description_ie.pdu_description.no_of_pdus;
          cgrlc_test_mode_req->dl_timeslot_offset /* TCS 2.1 */
          = gprs_test_mode_cmd->mode_flag.dl_timeslot_offset;
          cgrlc_test_mode_req->test_mode_flag /* TCS 2.1 */
          = gprs_test_mode_cmd->mode_flag.mode_flag_val;
#ifdef GMM_TCS4
  #ifdef FF_EGPRS
          cgrlc_test_mode_req->v_ul_gmsk_modulation = FALSE;
  #endif /*FF_EGPRS*/
#endif /*GMM_TCS4*/
        PSEND (hCommGRLC, cgrlc_test_mode_req); /* TCS 2.1 */
        SET_STATE (KERN, KERN_GMM_REG_TEST_MODE);
#ifdef FF_EM_MODE
        EM_GMM_SET_STATE(KERN_GMM_REG_TEST_MODE);
#endif
      }
      else
      {
        TRACE_ERROR ("GPRS TEST MODE CMD receives, but no TEST SIM is inserted");
      }
      break;
    default:
      sig_kern_rx_gmm_status ( ERRCS_TYPE_INCOMPATIBLE);
      TRACE_ERROR ("GPRS TEST MODE CMD receives in wrong GMM state");
      break;
  }
} /* sig_rx_kern_status_ind () */

#ifdef GMM_TCS4
#ifdef FF_EGPRS
/*
+------------------------------------------------------------------------------
| Function    : sig_rx_kern_egprs_loopb_cmd_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_RX_KERN_EGPRS_LOOPB_CMD_IND
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_rx_kern_egprs_loopb_cmd_ind ( )
{ 
  TRACE_ISIG( "sig_rx_kern_egprs_loopb_cmd_ind" );
  
  switch( GET_STATE( KERN ) )
  {
    case KERN_GMM_REG_NORMAL_SERVICE:
      if (OP_NORMAL_SIM NEQ gmm_data->kern.sim_op_mode)
      /* Do not try to test if it is OP_NORMAL_SIM, wich has the value 1
       * SIM supports only comparism for test sims in that way: >=0x80
       */
      {
        MCAST(egprs_start_radio_block_loopback_cmd, EGPRS_START_RADIO_BLOCK_LOOPBACK_CMD);
        
        PALLOC (cgrlc_test_mode_req, CGRLC_TEST_MODE_REQ); 
        cgrlc_test_mode_req->dl_timeslot_offset 
        = egprs_start_radio_block_loopback_cmd->mode_flag.dl_timeslot_offset;
        cgrlc_test_mode_req->v_no_of_pdus = FALSE;
        cgrlc_test_mode_req->test_mode_flag = CGRLC_EGPRS_RADIO_LOOP;
        cgrlc_test_mode_req->v_ul_gmsk_modulation = TRUE;
        cgrlc_test_mode_req->ul_gmsk_modulation   
        = egprs_start_radio_block_loopback_cmd->mode_flag.mode_flag_val;
        PSEND (hCommGRLC, cgrlc_test_mode_req); 
        SET_STATE (KERN, KERN_GMM_REG_TEST_MODE);
#ifdef FF_EM_MODE
        EM_GMM_SET_STATE(KERN_GMM_REG_TEST_MODE);
#endif
      }
      else
      {
        TRACE_ERROR ("EGPRS START RADIO BLOCK LOOPBACK CMD received, but no TEST SIM inserted");
      }
      break;
    default:
      sig_kern_rx_gmm_status ( ERRCS_TYPE_INCOMPATIBLE);
      TRACE_ERROR ("EGPRS START RADIO BLOCK LOOPBACK CMD received in wrong GMM state");
      break;
  }
} /* sig_rx_kern_egprs_loopb_cmd_ind () */\

#endif /*FF_EGPRS*/
#endif /*GMM_TCS4*/

/*
+------------------------------------------------------------------------------
| Function    : sig_sync_kern_mmgmm_nreg_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive MMGMM_NREG_IND
|
|          MSC: 3.20.3 IMSI attach/detach
|
| Parameters  : *mmgmm_nreg_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_sync_kern_mmgmm_nreg_ind ( T_MMGMM_NREG_IND *mmgmm_nreg_ind )
{ 
  GMM_TRACE_FUNCTION( "kern_mmgmm_nreg_ind" );
  /*
   * Cel ID
   */
  gmm_data->kern.cell_id.lac  =  GMMREG_LA_INVALID;

  switch (mmgmm_nreg_ind->service) /* TCS 2.1 */
  {
    case NREG_NO_SERVICE:
      TRACE_EVENT ("Para: NO_SERVICE or CELL_SELECTION_FAILED");
      break;
    case NREG_LIMITED_SERVICE:
      TRACE_EVENT ("Para: LIMITED_SERVICE");
      break;
    default:
      break;
  }

  switch ( GET_STATE (KERN) )
  {
    case KERN_GMM_NULL_PLMN_SEARCH:
      SET_STATE (KERN,KERN_GMM_NULL_IMSI);
#ifdef FF_EM_MODE
      EM_GMM_SET_STATE(KERN_GMM_NULL_IMSI);
#endif
      if (!(gmm_data->sim_gprs_invalid && mmgmm_nreg_ind->cause EQ MMCS_SIM_REMOVED))
      {
          kern_indicate_service_loss (GMMREG_DT_COMB, 
                                  mmgmm_nreg_ind->service,  /* TCS 2.1 */
                                  mmgmm_nreg_ind->search_running,
                                  mmgmm_nreg_ind->cause);
      }
      break;   
    case KERN_GMM_DEREG_SUSPENDED:  
      TRACE_ERROR("unexpected MMGMM_NREG_IND");
      if (!(gmm_data->sim_gprs_invalid && mmgmm_nreg_ind->cause EQ MMCS_SIM_REMOVED))
      {
          kern_indicate_service_loss (GMMREG_DT_COMB, 
                                  mmgmm_nreg_ind->service,  /* TCS 2.1 */
                                  mmgmm_nreg_ind->search_running,
                                  mmgmm_nreg_ind->cause);
      }
      kern_call_undone_mm_proc_der();
      break;

    
    case KERN_GMM_DEREG_SUSPENDING:
    case KERN_GMM_DEREG_RESUMING:
      
    case KERN_GMM_NULL_IMSI_LIMITED_SERVICE_REQ:
    case KERN_GMM_NULL_NO_IMSI_LIMITED_SERVICE_REQ:

    case KERN_GMM_NULL_IMSI:
    case KERN_GMM_NULL_NO_IMSI:
    case KERN_GMM_DEREG_NO_IMSI:
    case KERN_GMM_DEREG_LIMITED_SERVICE:
    case KERN_GMM_DEREG_ATTEMPTING_TO_ATTACH:
    case KERN_GMM_DEREG_NO_CELL_AVAILABLE:
    case KERN_GMM_DEREG_PLMN_SEARCH:
    case KERN_GMM_REG_INITIATED:
      if (!(gmm_data->sim_gprs_invalid && mmgmm_nreg_ind->cause EQ MMCS_SIM_REMOVED))
      {
          kern_indicate_service_loss (GMMREG_DT_COMB, 
                                  mmgmm_nreg_ind->service,  /* TCS 2.1 */
                                  mmgmm_nreg_ind->search_running,
                                  mmgmm_nreg_ind->cause);
      }
      break;
    case KERN_GMM_REG_SUSPENDED:
      TRACE_ERROR("unexpected MMGMM_NREG_IND");
      if (!(gmm_data->sim_gprs_invalid && mmgmm_nreg_ind->cause EQ MMCS_SIM_REMOVED))
      {
          kern_indicate_service_loss (GMMREG_DT_COMB, 
                                  mmgmm_nreg_ind->service,  /* TCS 2.1 */
                                  mmgmm_nreg_ind->search_running,
                                  mmgmm_nreg_ind->cause);
      }
      kern_call_undone_mm_proc_der();
      break;
    case KERN_GMM_REG_SUSPENDING:  
    case KERN_GMM_REG_RESUMING:  
    case KERN_GMM_REG_NORMAL_SERVICE:
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE:
    case KERN_GMM_REG_NO_CELL_AVAILABLE:
    case KERN_GMM_REG_LIMITED_SERVICE:
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE_MM:
    case KERN_GMM_DEREG_INITIATED:
    case KERN_GMM_RAU_INITIATED:
      if (!(gmm_data->sim_gprs_invalid && mmgmm_nreg_ind->cause EQ MMCS_SIM_REMOVED))
      {
          kern_indicate_service_loss (GMMREG_DT_IMSI,
                                  mmgmm_nreg_ind->service, /* TCS 2.1 */
                                  mmgmm_nreg_ind->search_running,
                                  mmgmm_nreg_ind->cause);
      }
      break;
    default:
      TRACE_ERROR( "MMGMM_NREG_IND unexpected" );
      break;
  } 
  PFREE ( mmgmm_nreg_ind );

  GMM_RETURN;
} /* sig_sync_mmgmm_nreg_ind() */

/*
+------------------------------------------------------------------------------
| Function    : sig_sync_mmgmm_reg_rej 
+------------------------------------------------------------------------------
| Description : Handles the primitive MMGMM_REG_REJ
|
|          MSC: 3.20.3 IMSI attach/detach
|
| Parameters  : *mmgmm_reg_rej - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_sync_kern_mmgmm_reg_rej ( T_MMGMM_REG_REJ *mmgmm_reg_rej )
{ 
  GMM_TRACE_FUNCTION( "sig_sync_mmgmm_reg_rej" );

  if(mmgmm_reg_rej->cause EQ MMCS_PLMN_NOT_ALLOWED)
  {
    memcpy (gmm_data->kern.attach_cap.list_of_forbidden_plmns_for_gprs_service[0].mcc,
              mmgmm_reg_rej->new_forb_plmn.mcc, SIZE_MCC);
    memcpy (gmm_data->kern.attach_cap.list_of_forbidden_plmns_for_gprs_service[0].mnc,
              mmgmm_reg_rej->new_forb_plmn.mnc, SIZE_MNC);
  }

  switch (mmgmm_reg_rej->service) /* TCS 2.1 */
  {
    case NREG_NO_SERVICE:
      TRACE_EVENT ("Para: NO_SERVICE or CELL_SELECTION_FAILED");
      /* todo state missing */
      gmm_data->kern.sig_cell_info.mm_status = MMGMM_LIMITED_SERVICE;
      break;
    case NREG_LIMITED_SERVICE:
      /*
       * TC 44.2.1.1.4.3.1
       * to remember that LA is changed if mmgmm_acivate_in comes after cell_ind
       * other way arround mmgmm_acivate_ind containspaameter update_needed
       */
      gmm_data->kern.sig_cell_info.mm_status = MMGMM_LIMITED_SERVICE;
      TRACE_EVENT ("Para: LIMITED_SERVICE");
      break;
    default:
      break;
  }

 
  switch ( GET_STATE (KERN) )
  {
     case KERN_GMM_DEREG_NO_IMSI:
       
      SET_STATE (KERN, KERN_GMM_NULL_NO_IMSI);
#ifdef FF_EM_MODE
      EM_GMM_SET_STATE(KERN_GMM_NULL_NO_IMSI);
#endif
      kern_indicate_service_loss (GMMREG_DT_COMB,
                                  mmgmm_reg_rej->service,  /* TCS 2.1 */
                                  mmgmm_reg_rej->search_running,
                                  mmgmm_reg_rej->cause);
      kern_mm_activate_mm();
      break;                                      

    case KERN_GMM_NULL_PLMN_SEARCH:
      SET_STATE (KERN,KERN_GMM_NULL_IMSI);
#ifdef FF_EM_MODE
      EM_GMM_SET_STATE(KERN_GMM_NULL_IMSI);
#endif
      kern_indicate_service_loss (GMMREG_DT_COMB, 
                                  mmgmm_reg_rej->service,  /* TCS 2.1 */
                                  mmgmm_reg_rej->search_running,
                                  mmgmm_reg_rej->cause);

      break;
     
    case KERN_GMM_DEREG_SUSPENDED:  
      kern_indicate_service_loss (GMMREG_DT_COMB, 
                                  mmgmm_reg_rej->service,  /* TCS 2.1 */
                                  mmgmm_reg_rej->search_running,
                                  mmgmm_reg_rej->cause);
      kern_call_undone_mm_proc_der ();
      break;
    case KERN_GMM_DEREG_SUSPENDING:
      
    case KERN_GMM_NULL_IMSI_LIMITED_SERVICE_REQ:
    case KERN_GMM_NULL_NO_IMSI_LIMITED_SERVICE_REQ:

    case KERN_GMM_NULL_IMSI:
    case KERN_GMM_NULL_NO_IMSI:
    case KERN_GMM_DEREG_LIMITED_SERVICE:
    case KERN_GMM_DEREG_ATTEMPTING_TO_ATTACH:
    case KERN_GMM_DEREG_NO_CELL_AVAILABLE:
    case KERN_GMM_DEREG_PLMN_SEARCH:
    case KERN_GMM_REG_INITIATED:
      kern_indicate_service_loss (GMMREG_DT_COMB, 
                                  mmgmm_reg_rej->service,  /* TCS 2.1 */
                                  mmgmm_reg_rej->search_running,
                                  mmgmm_reg_rej->cause);
      break;
  
    case KERN_GMM_REG_SUSPENDED:
      /* Label RESUME_GRR_REG */
      if (MMGMM_RESUMPTION_OK NEQ mmgmm_reg_rej->resumption)
      {
        TRACE_EVENT("Para: resumption failure: RAU needed");
        SET_STATE(GU,GU_UPDATE_NEEDED);
      }
      else
      {
        TRACE_EVENT("OPar: resumption OK.");
      }
      
      kern_indicate_service_loss (GMMREG_DT_IMSI, 
                                  mmgmm_reg_rej->service,  /* TCS 2.1 */
                                  mmgmm_reg_rej->search_running,
                                  mmgmm_reg_rej->cause);
      kern_call_undone_mm_proc_reg ();
      break;

    case KERN_GMM_REG_SUSPENDING:  

    case KERN_GMM_REG_NORMAL_SERVICE:
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE:
    case KERN_GMM_REG_NO_CELL_AVAILABLE:
    case KERN_GMM_REG_LIMITED_SERVICE:
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE_MM:
    case KERN_GMM_DEREG_INITIATED:
    case KERN_GMM_RAU_INITIATED:
      kern_indicate_service_loss (GMMREG_DT_IMSI, 
                                  mmgmm_reg_rej->service,  /* TCS 2.1 */
                                  mmgmm_reg_rej->search_running,
                                  mmgmm_reg_rej->cause);
      break;
    default:
      TRACE_ERROR( "MMGMM_REG_REJ unexpected" );
      break;
  } 
  PFREE ( mmgmm_reg_rej );

  GMM_RETURN;
}/*sig_sync_kern_mmgmm_reg_rej */


/*
+------------------------------------------------------------------------------
| Function    : sig_sync_kern_mmgmm_reg_cnf
+------------------------------------------------------------------------------
| Description : Handles the primitive MMGMM_REG_CNF
|
|          MSC: 3.20.3 IMSI attach/detach
|
| Parameters  : *mmgmm_reg_cnf - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_sync_kern_mmgmm_reg_cnf ( T_MMGMM_REG_CNF *mmgmm_reg_cnf )
{ 
  GMM_TRACE_FUNCTION( "sig_sync_kern_mmgmm_reg_cnf" );

  /*
   * Cel ID should be removed
   */
  gmm_data->kern.cell_id.plmn = mmgmm_reg_cnf->plmn;
  gmm_data->kern.cell_id.lac  = mmgmm_reg_cnf->lac;
  gmm_data->kern.cell_id.cid = mmgmm_reg_cnf->cid;
  switch (mmgmm_reg_cnf->gprs_indicator)
  {
    case MMGMM_GPRS_SUPP_NO:
      gmm_data->kern.sig_cell_info.gmm_status = GMMRR_SERVICE_NONE;      
      break;
    default:
      if (GMMRR_SERVICE_LIMITED NEQ gmm_data->kern.sig_cell_info.gmm_status)
      {
        gmm_data->kern.sig_cell_info.gmm_status = GMMRR_SERVICE_FULL;      
      }
      break;
  }

  if ( gmm_data->kern.attach_cap.mobile_class EQ GMMREG_CLASS_CG )
  {
    PFREE ( mmgmm_reg_cnf );
    TRACE_EVENT ( "reg_cnf ignored because of MS Class CG");
    GMM_RETURN;
  }
  
  SET_STATE( MM, GMM_MM_REG_NORMAL_SERVICE );

  /*
   * GMM TC_1710 rau attemept counter less then 5 in periodic lau.
   * this situation should not occur
   */

  if (GU_UPDATE_NEEDED EQ GET_STATE(GU)
  && GMMCS_IMSI_UNKNOWN==gmm_data->kern.detach_cap.error_cause)
  {
    SET_STATE(GU,  GU_UPDATE_NOT_NEEDED);
  }
  
  switch( GET_STATE( KERN ) )
  {
    case KERN_GMM_DEREG_SUSPENDED:
      /*
       * GMM received the mmgmm_reg_cnf primitve because off requesting LAU
       * before Normal RAU or ATTACH
       */
      if (gmm_data->sim_gprs_invalid
      || 
        (GMMRR_NET_MODE_III==gmm_data->kern.sig_cell_info.net_mode
      &&  GMMREG_CLASS_BC==  gmm_data->kern.attach_cap.mobile_class 
        )
      )
      {
        kern_mmgmmreg_attach_cnf (GMMREG_AT_IMSI, 
          SEARCH_NOT_RUNNING,mmgmm_reg_cnf);
      }
      else
      {
        kern_mmgmmreg_attach_cnf (GMMREG_AT_IMSI, 
          SEARCH_RUNNING,mmgmm_reg_cnf);
      }

      kern_call_undone_mm_proc_der ();
      break;
    case KERN_GMM_REG_SUSPENDED:
      /*
       * LLC is resumed by assigning the last given TLLI. Unassigning LLC is 
       * necessary, because LLC is able to transmit signaling data in 
       * suspended mode, but in GMM_suspended mode its not allowed, or 
       * possible, to transmit any data! 
       */
      kern_mmgmmreg_attach_cnf (GMMREG_AT_COMB, SEARCH_NOT_RUNNING, mmgmm_reg_cnf);
      if (MMGMM_RESUMPTION_OK NEQ mmgmm_reg_cnf->resumption)
      {
        TRACE_EVENT("Para: resumption failure: RAU needed");
        SET_STATE(GU,GU_UPDATE_NEEDED);
      }
      else
      {
        TRACE_EVENT("OPar: resumption OK.");
      }
      kern_call_undone_mm_proc_reg ();
      break;
    case KERN_GMM_NULL_PLMN_SEARCH:
      SET_STATE (KERN,KERN_GMM_NULL_IMSI);
#ifdef FF_EM_MODE
      EM_GMM_SET_STATE(KERN_GMM_NULL_IMSI);
#endif
      /* NO break */
    case KERN_GMM_NULL_IMSI_LIMITED_SERVICE_REQ:
    case KERN_GMM_NULL_NO_IMSI_LIMITED_SERVICE_REQ:
    case KERN_GMM_NULL_NO_IMSI:
    
    case KERN_GMM_NULL_IMSI:
      kern_mmgmmreg_attach_cnf (GMMREG_AT_IMSI, SEARCH_NOT_RUNNING, mmgmm_reg_cnf);
      break;                                      
    case KERN_GMM_DEREG_NO_IMSI:
      SET_STATE (KERN, KERN_GMM_NULL_NO_IMSI);
#ifdef FF_EM_MODE
      EM_GMM_SET_STATE(KERN_GMM_NULL_NO_IMSI);
#endif
      //kern_mm_activate_mm();
      
      kern_mmgmmreg_attach_cnf (GMMREG_AT_IMSI, SEARCH_NOT_RUNNING, mmgmm_reg_cnf);
      
      break;                                      
    case KERN_GMM_DEREG_LIMITED_SERVICE:
    case KERN_GMM_DEREG_ATTEMPTING_TO_ATTACH:
    case KERN_GMM_DEREG_NO_CELL_AVAILABLE:
    case KERN_GMM_DEREG_PLMN_SEARCH:
    case KERN_GMM_REG_INITIATED:
      kern_mmgmmreg_attach_cnf (GMMREG_AT_IMSI, SEARCH_RUNNING, mmgmm_reg_cnf);
      break;                                      
    case KERN_GMM_REG_NORMAL_SERVICE:
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE:
    case KERN_GMM_REG_NO_CELL_AVAILABLE:
    case KERN_GMM_REG_LIMITED_SERVICE:
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE_MM:
    case KERN_GMM_DEREG_INITIATED:
    case KERN_GMM_RAU_INITIATED:
      kern_mmgmmreg_attach_cnf (GMMREG_AT_COMB, SEARCH_NOT_RUNNING, mmgmm_reg_cnf);
      break;
    default:
      TRACE_ERROR ("Unexpexcted state");   
      break;
  }
  PFREE ( mmgmm_reg_cnf );
  GMM_RETURN;  
} /* sig_sync_kern_mmgmm_reg_cnf() */

/*
+------------------------------------------------------------------------------
| Function    : sig_rdy_kern_cu_ind
+------------------------------------------------------------------------------
| Description : Handles the signal sig_rdy_kern_cu_ind
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_rdy_kern_cu_ind ( void )
{ 
  GMM_TRACE_FUNCTION( "sig_rdy_kern_cu_ind" );

  switch (GET_STATE(CU))
  {
    case CU_NOT_REQUESTED:
      SET_STATE(CU,CU_REQUESTED);
      break;
    case CU_CELL_RES_SENT:
      SET_STATE(CU,CU_REQUESTED_CELL_RES_SENT);
      break;
    default:
      break;
  }  
  GMM_RETURN;  
} /* sig_rdy_kern_cu_ind() */



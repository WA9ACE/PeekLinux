/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  gmm_kernf.c
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

#include "gmm_kernf.h"  /* to get some local definitions */
#include "gmm_kernl.h"  /* to get some local definitions */
#include "gmm_f.h"
#include <string.h>     /* to get memset */
#include "pcm.h"
#include "gmm_rxf.h"
#include "gmm_rxs.h"
#include "gmm_txf.h"
#include "gmm_rdyf.h"
#include "gmm_syncs.h"
#include "gmm_kernp.h"
#include "gmm_rdys.h"
#include "gmm_syncf.h"
#include "gmm_em.h"     /* To get Engineering Mode functions */

#ifdef _TARGET_
#include "ffs/ffs.h"
#endif

#include "cl_imei.h"  /* IMEI common library */

#include <stdlib.h>

#include <stdio.h>      /* for sprintf */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/
/*
+------------------------------------------------------------------------------
| Function    : kern_gmmreg_info_ind
+------------------------------------------------------------------------------
| Description : The function kern_gmmreg_info_ind forwards the info comming
|               from the net to the MMI entity
|       
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_gmmreg_info_ind ( void)
{ 
  GMM_TRACE_FUNCTION( "kern_gmmreg_info_ind" );
  {
	  MCAST(gmm_information, GMM_INFORMATION);
    /* Agilent Issue OMAPS00148436 WA */
    if (!gmm_information->v_full_network_name && !gmm_information->v_short_network_name 
        && !gmm_information->v_time_zone && !gmm_information->v_time_zone_and_time)
    { 
      TRACE_EVENT ("GMM_INFORMATION message doesn't contain valid information"); 
      GMM_RETURN; 
    } 
    {

    PALLOC (gmmreg_info_ind, GMMREG_INFO_IND); /* T_GMMREG_INFO_IND */
  
    /* 
     * Set PLMN, this will be used if network name is given
     */
    gmmreg_info_ind->plmn.v_plmn = TRUE;       
    memcpy (gmmreg_info_ind->plmn.mcc, gmm_data->kern.attach_cap.rai_accepted.mcc, SIZE_MCC);
    memcpy (gmmreg_info_ind->plmn.mnc, gmm_data->kern.attach_cap.rai_accepted.mnc, SIZE_MNC);

    /*
     * Set full network name, if present
     */

      if (gmm_information->v_full_network_name) 
      {
        gmmreg_info_ind->full_net_name_gmm.v_name     = TRUE;
        gmmreg_info_ind->full_net_name_gmm.dcs /* TCS 2.1 */
            = gmm_information->full_network_name.code;
        gmmreg_info_ind->full_net_name_gmm.add_ci    
         = gmm_information->full_network_name.add_ci;
        gmmreg_info_ind->full_net_name_gmm.num_spare  = 
          gmm_information->full_network_name.nr_sparebits;
        memset(gmmreg_info_ind->full_net_name_gmm.text, 0, MMR_MAX_TEXT_LEN);
        gmmreg_info_ind->full_net_name_gmm.c_text = 
          MINIMUM (MMR_MAX_TEXT_LEN, gmm_information->full_network_name.c_text_string);
        memcpy (gmmreg_info_ind->full_net_name_gmm.text,
            gmm_information->full_network_name.text_string,
            gmmreg_info_ind->full_net_name_gmm.c_text);
      }
      else
      {
        gmmreg_info_ind->full_net_name_gmm.v_name = FALSE;
      }
      /* Set short network name, if present */
      if (gmm_information->v_short_network_name) 
      {
        gmmreg_info_ind->short_net_name_gmm.v_name    = TRUE;
        gmmreg_info_ind->short_net_name_gmm.dcs       = gmm_information->short_network_name.code;  /* TCS 2.1 */
        gmmreg_info_ind->short_net_name_gmm.add_ci    = gmm_information->short_network_name.add_ci;
        gmmreg_info_ind->short_net_name_gmm.num_spare = 
        gmm_information->short_network_name.nr_sparebits;
        memset(gmmreg_info_ind->short_net_name_gmm.text, 0, MMR_MAX_TEXT_LEN);
        gmmreg_info_ind->short_net_name_gmm.c_text = 
        MINIMUM (MMR_MAX_TEXT_LEN, gmm_information->short_network_name.c_text_string);
        memcpy (gmmreg_info_ind->short_net_name_gmm.text,
          gmm_information->short_network_name.text_string,
          gmmreg_info_ind->short_net_name_gmm.c_text);
      }
      else
      {
        gmmreg_info_ind->short_net_name_gmm.v_name = FALSE;
      }
      /* Set network time zone, if present */
      if (gmm_information->v_time_zone)
      {
        gmmreg_info_ind->net_time_zone.v_time_zone = TRUE;
        gmmreg_info_ind->net_time_zone.time_zone 
            = gmm_information->time_zone.time_zone_value;
      }
      else
      {
        gmmreg_info_ind->net_time_zone.v_time_zone = FALSE;
      }
      /* Set network time zone and time, if present */
      if (gmm_information->v_time_zone_and_time)
      {
        gmmreg_info_ind->net_time_zone.v_time_zone = TRUE;
        gmmreg_info_ind->net_time_zone.time_zone   
            = gmm_information->time_zone_and_time.time_zone_value;
        gmmreg_info_ind->net_time.v_time = TRUE;

        gmmreg_info_ind->net_time.year = 
          10 * gmm_information->time_zone_and_time.year[0] + 
               gmm_information->time_zone_and_time.year[1];
        gmmreg_info_ind->net_time.month = 
          10 * gmm_information->time_zone_and_time.month[0] + 
               gmm_information->time_zone_and_time.month[1];
        gmmreg_info_ind->net_time.day = 
          10 * gmm_information->time_zone_and_time.day[0] +
               gmm_information->time_zone_and_time.day[1];
        gmmreg_info_ind->net_time.hour = 
          10 * gmm_information->time_zone_and_time.hour[0] + 
               gmm_information->time_zone_and_time.hour[1];
        gmmreg_info_ind->net_time.minute =
          10 * gmm_information->time_zone_and_time.minute[0] +
               gmm_information->time_zone_and_time.minute[1];
        gmmreg_info_ind->net_time.second = 
          10 * gmm_information->time_zone_and_time.second[0] +
               gmm_information->time_zone_and_time.second[1];
      }
      else
      {
        gmmreg_info_ind->net_time.v_time = FALSE;
      }

#ifdef REL99
      if (gmm_information->v_daylight_save_time) /* TCS 4.0 */
      { /* TCS 4.0 */
        gmmreg_info_ind->net_daylight_save_time =  /* TCS 4.0 */
          gmm_information->daylight_save_time.save_time_value; /* TCS 4.0 */
      } /* TCS 4.0 */
      else /* TCS 4.0 */
      { /* TCS 4.0 */
        gmmreg_info_ind->net_daylight_save_time = GMMREG_ADJ_NO; /* TCS 4.0 */
      } /* TCS 4.0 */
#endif

    PSEND ( hCommMMI, gmmreg_info_ind);
	}
  }
  GMM_RETURN;
} /* kern_gmmreg_info_ind() */
/*
+------------------------------------------------------------------------------
| Function    : kern_calculate_digits
+------------------------------------------------------------------------------
| Description : Derives tm mobile identiti for the AIR
|
| COPIED FROM : MM    mm_mmf.c : mm_calculate_digits
|               
| Parameters  : mobile_identity   - used in the AIR message
|
+------------------------------------------------------------------------------
*/
LOCAL UBYTE kern_calculate_digits (UBYTE *digits)
{
  UBYTE i = 0;

  GMM_TRACE_FUNCTION ("kern_calculate_digits()");

  while (digits[i] < 0x0A AND i < 16)
    i++;

  GMM_RETURN_ (i);
}
/*
+------------------------------------------------------------------------------
| Function    : kern_mmgmmreg_attach_cnf
+------------------------------------------------------------------------------
| Description : This procedure sends the gmmreg_attach_cnf primitiv to ACI
|
| Parameters  : attach_type - attach type
|               mmgmm_reg_req - the primitiv
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_mmgmmreg_attach_cnf ( UBYTE attach_type, UBYTE search_running,
                                      T_MMGMM_REG_CNF * mmgmm_reg_cnf )
{ 
  GMM_TRACE_FUNCTION( "kern_mmgmmreg_attach_cnf" );

    TRACE_1_INFO ("Info: MM  lac: %x", mmgmm_reg_cnf->lac);

  switch (gmm_data->kern.sig_cell_info.gmm_status)
  {
    case GMMRR_SERVICE_LIMITED:
    case GMMRR_SERVICE_NONE:
      search_running=SEARCH_NOT_RUNNING;
      break;
    default:
    case GMMRR_SERVICE_FULL:
      break;
  }


  gmm_data->kern.mm_cell_env.cid = mmgmm_reg_cnf->cid;
  gmm_data->kern.mm_cell_env.rai.lac = mmgmm_reg_cnf->lac;
  gmm_data->kern.mm_cell_env.rai.plmn = mmgmm_reg_cnf->plmn;

  GMM_TRACE_GMM_DATA(GMM_DEBUG_PRINT_MASK_CID);

  {
    PALLOC ( gmmreg_attach_cnf, GMMREG_ATTACH_CNF );
      gmmreg_attach_cnf->attach_type = attach_type;
      /*
       * gmmreg_attach_cnf->plmn = gmm_data->mm_plmn= mmgmm_reg_cnf->plmn;
       */
      gmmreg_attach_cnf->plmn = mmgmm_reg_cnf->plmn;
      gmmreg_attach_cnf->search_running = search_running;
      gmmreg_attach_cnf->lac = mmgmm_reg_cnf->lac;
      gmmreg_attach_cnf->rac =  gmm_data->kern.sig_cell_info.env.rai.rac;
      //gmmreg_attach_cnf->cid = gmm_data->kern.sig_cell_info.env.cid; // #HM#
      gmmreg_attach_cnf->cid = mmgmm_reg_cnf->cid; // #HM#
      gmmreg_attach_cnf->gprs_indicator = gmm_data->kern.sig_cell_info.gmm_status;
      gmmreg_attach_cnf->bootup_cause = REG_END;
#ifdef GMM_TCS4
      gmmreg_attach_cnf->rt = gmm_data->kern.sig_cell_info.rt; // TCS 4.0
#endif
#ifdef TRACE_EVE
      {
        switch (attach_type)
        {
          case GMMREG_AT_COMB:
            TRACE_EVENT ("MS is combined attached.");
            break;
          case GMMREG_AT_IMSI:
            TRACE_EVENT ("MS is GSM-only attached.");
            break;
          case GMMREG_AT_GPRS:
            TRACE_EVENT ("MS is GPRS-only attached.");
            break;
          default:
            TRACE_ERROR ("MS is attached with wrong attach type.");
            break;
        }
      TRACE_7_PARA("%x%x%x, %x%x%x, lac %x", 
          gmmreg_attach_cnf->plmn.mcc[0],
          gmmreg_attach_cnf->plmn.mcc[1],
          gmmreg_attach_cnf->plmn.mcc[2],
          gmmreg_attach_cnf->plmn.mnc[0],
          gmmreg_attach_cnf->plmn.mnc[1],
          gmmreg_attach_cnf->plmn.mnc[2],
          gmmreg_attach_cnf->lac);
     }
#endif
   
    PSEND ( hCommMMI, gmmreg_attach_cnf );
    
    gmm_data->kern.attach_cap.gmmreg = FALSE;
  }
  GMM_RETURN;
} /* kern_mmgmmreg_attach_cnf () */
/*
+------------------------------------------------------------------------------
| Function    : kern_gmmreg_attach_cnf
+------------------------------------------------------------------------------
| Description : This procedure sends the gmmreg_attach_cnf primitiv to ACI
|
| Parameters  : attach_type - attach type
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_gmmreg_attach_cnf ( UBYTE attach_type)
{ 
  GMM_TRACE_FUNCTION( "kern_gmmreg_attach_cnf" );
  kern_gmmreg_attach_cnf_sr(attach_type,SEARCH_NOT_RUNNING);
  GMM_RETURN;
} /* kern_gmmreg_attach_cnf () */
/*
+------------------------------------------------------------------------------
| Function    : kern_gmmreg_attach_cnf_sr
+------------------------------------------------------------------------------
| Description : This procedure sends the gmmreg_attach_cnf primitiv to ACI
|
| Parameters  : attach_type - attach type
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_gmmreg_attach_cnf_sr ( UBYTE attach_type, UBYTE search_running)
{ 
  GMM_TRACE_FUNCTION( "kern_gmmreg_attach_cnf_sr" );
  TRACE_2_OUT_PARA("%s, %s",
      search_running==SEARCH_RUNNING? "searching":"finished",
      attach_type==GMMREG_AT_COMB?    "comb":
      attach_type==GMMREG_AT_GPRS?"gprs":
      attach_type==GMMREG_AT_IMSI?"gsm":
                                   "unknown tpe"
     );

  {
    PALLOC ( gmmreg_attach_cnf, GMMREG_ATTACH_CNF );
      gmmreg_attach_cnf->attach_type = attach_type;
      gmmreg_attach_cnf->plmn.v_plmn = TRUE;
      gmmreg_attach_cnf->search_running= search_running;
      memcpy ( gmmreg_attach_cnf->plmn.mcc, gmm_data->kern.sig_cell_info.env.rai.plmn.mcc, SIZE_MCC);
      memcpy ( gmmreg_attach_cnf->plmn.mnc, gmm_data->kern.sig_cell_info.env.rai.plmn.mnc, SIZE_MNC);
      gmmreg_attach_cnf->lac = gmm_data->kern.sig_cell_info.env.rai.lac;
      gmmreg_attach_cnf->rac = gmm_data->kern.sig_cell_info.env.rai.rac;
      gmmreg_attach_cnf->cid = gmm_data->kern.sig_cell_info.env.cid;
      gmmreg_attach_cnf->gprs_indicator = gmm_data->kern.sig_cell_info.gmm_status;      
      gmmreg_attach_cnf->bootup_cause = REG_END;
#ifdef GMM_TCS4
      gmmreg_attach_cnf->rt = gmm_data->kern.sig_cell_info.rt; /* TCS 4.0 */
#endif
    PSEND ( hCommMMI, gmmreg_attach_cnf );
    
    gmm_data->kern.attach_cap.gmmreg = FALSE;

    /* 
     * ACI does not realize GSM detach if search running has been set
     * in kern_mmgmm_nreg_cnf 
     */
    if (GMMRR_NET_MODE_III==gmm_data->kern.sig_cell_info.net_mode
    && GMMREG_CLASS_BG==  gmm_data->kern.attach_cap.mobile_class
    && GMMREG_AT_GPRS==attach_type)
    {
      kern_gmmreg_detach (GMMREG_DT_IMSI,
                        GMMCS_LIMITED_SERVICE,  /* TCS 2.1 */
                        SEARCH_NOT_RUNNING,
                        GMMCS_LIMITED_SERVICE);
    }
  }
  GMM_RETURN;
} /* kern_gmmreg_attach_cnf_sr () */
/*
+------------------------------------------------------------------------------
| Function    : kern_gmmreg_detach
+------------------------------------------------------------------------------
| Description : This procedure sends the gmmreg_detach primitiv to ACI
|
| Parameters  : detach_type    - detach type
|                cause           - error cause
|               search_running - cell search still running or not
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_gmmreg_detach ( UBYTE detach_type, 
                                 USHORT service,
                                 UBYTE search_running,
                                 USHORT cause)
{
  UBYTE mm_state;

  GMM_TRACE_FUNCTION( "kern_gmmreg_detach" );

  mm_state = GET_STATE(MM);

  if (GMMREG_DT_GPRS==detach_type
  &&  (  GMM_MM_DEREG==mm_state
      || GMM_MM_REG_NO_CELL_AVAILABLE==mm_state
      || GMM_MM_REG_INITATED_VIA_GPRS==mm_state
      || MMGMM_LIMITED_SERVICE==gmm_data->kern.sig_cell_info.mm_status )
  )
  {
    detach_type = GMMREG_DT_COMB;
  }

  TRACE_3_OUT_PARA("%s, cause: 0x%x, %s",
        detach_type==GMMREG_DT_COMB?    "comb":
        detach_type==GMMREG_DT_IMSI?    "gsm":
        detach_type==GMMREG_DT_GPRS?"gprs":
        detach_type==GMMREG_DT_POWER_OFF?"power off":
        detach_type==GMMREG_DT_SIM_REMOVED?"sim removed":
        detach_type==GMMREG_DT_LIMITED_SERVICE?"limited":
        detach_type==GMMREG_DT_SOFT_OFF?"soft off":
                                            "unknown type"
          , cause,
       (search_running==GMMREG_SEARCH_RUNNING?"searching":"finished"));

  if ( gmm_data->kern.detach_cap.gmmreg )
  {
    PALLOC ( gmmreg_detach_cnf, GMMREG_DETACH_CNF );
      gmmreg_detach_cnf->detach_type = gmm_data->kern.detach_cap.detach_type;
    PSEND ( hCommMMI, gmmreg_detach_cnf );
  }
  else
  {
    if ( gmm_data->kern.attach_cap.gmmreg )
    {
      if (GMMREG_CLASS_CG==gmm_data->kern.attach_cap.mobile_class
      && GMMREG_DT_IMSI==detach_type)
      {
        kern_gmmreg_attach_cnf_sr(GMMREG_AT_GPRS, search_running);
      }
      else
      {
        PALLOC ( gmmreg_attach_rej, GMMREG_ATTACH_REJ );
          gmmreg_attach_rej->detach_type = detach_type;
          gmmreg_attach_rej->cause = cause;
          gmmreg_attach_rej->search_running = search_running;
          gmmreg_attach_rej->service = service;
        PSEND ( hCommMMI, gmmreg_attach_rej );
      }
      if (GMMREG_SEARCH_NOT_RUNNING==search_running)
      {
        gmm_data->kern.attach_cap.gmmreg=FALSE;
      }

    }
    else /* if ! gmmreg */
    {
      /* 
       * If gmmsm == TRUE means, we are in GMM_REGISTERED_INITIATED state.
       * That means, if ACI did not send an attach request ACI need not
       * to be informed, otherwise we are attached in ACI has to be informed
       */
#ifndef GMM_TCS4
      if ( !gmm_data->kern.attach_cap.gmmsm )
#endif 
      {
        PALLOC ( gmmreg_detach_ind, GMMREG_DETACH_IND );
          gmmreg_detach_ind->detach_type = detach_type;
          gmmreg_detach_ind->cause = cause;
          gmmreg_detach_ind->search_running = search_running;
          gmmreg_detach_ind->service = service;
        PSEND ( hCommMMI, gmmreg_detach_ind );
      }
    }
  }
  gmm_data->kern.detach_cap.gmmreg=FALSE;
  /*
   * Clear data before Power Off
   */
  if (GMMREG_DT_POWER_OFF==detach_type)
  {
    /*
     * every servce i reinitilized that the test sysem may run
     * with out real power off
     */
    kern_init();
    rxgmm_init();
    txgmm_init();
    rdy_init();
    sync_gmm_init();

    /*
     * In kern_init the deep is set to 0 for MACRO GMM_RETURN
     */
    return;
  }
  GMM_RETURN;
} /* kern_gmmreg_detach () */

/*
+------------------------------------------------------------------------------
| Function    : kern_llgmm_assign_tlli
+------------------------------------------------------------------------------
| Description : This procedure assigned the given TLLI parameter to LLC 
|
| Parameters  : new_tlli_type - type of new TLLI (random, local ,..)
|               old_tlli_type - type of old TLLI
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_llgmm_assign_tlli ( T_TLLI_TYPE new_tlli_type,
                                     T_TLLI_TYPE old_tlli_type )
{ 
  GMM_TRACE_FUNCTION( "kern_llgmm_assign_tlli" );

  if( GMM_LLC_STATE_UNASSIGNED EQ gmm_data->kern.attach_cap.llc_state )
  {
    gmm_data->kern.attach_cap.llc_state = GMM_LLC_STATE_ASSIGNED;
  }
  {
    PALLOC ( llgmm_assign_req, LLGMM_ASSIGN_REQ );
      llgmm_assign_req->old_tlli = gmm_get_tlli ( old_tlli_type );
      if ( GMMRR_TMSI_INVALID == gmm_data->ptmsi.current )
      {
        /*
         * <R.GMM.TLLIUSE.M.005>
         */
        llgmm_assign_req->new_tlli = gmm_get_tlli (  RANDOM_TLLI );
      }
      else
      {
        llgmm_assign_req->new_tlli = gmm_get_tlli ( new_tlli_type );
      }
      if (NO_KEY== gmm_data->kern.auth_cap.cksn)
      {
        memset (llgmm_assign_req->llgmm_kc.key, 0x0, MAX_KC);
      }
      else
      {
        memcpy ( llgmm_assign_req->llgmm_kc.key, gmm_data->kern.auth_cap.kc, MAX_KC );
      }
      llgmm_assign_req->ciphering_algorithm = LLGMM_CIPHER_NA;
 
      TRACE_2_OUT_PARA("TLLI old: 0x%X, new: 0x%X",  llgmm_assign_req->old_tlli,llgmm_assign_req->new_tlli);

    PSEND ( hCommLLC, llgmm_assign_req );
  }
  GMM_RETURN;
} /* kern_llgmm_assign_tlli() */
/*
+------------------------------------------------------------------------------
| Function    : kern_llgmm_assign
+------------------------------------------------------------------------------
| Description : This procedure assigned the current TLLI parameter to LLC 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_llgmm_assign ( void )
{ 
  GMM_TRACE_FUNCTION( "kern_llgmm_assign" );
  kern_llgmm_assign_tlli ( CURRENT_TLLI, OLD_TLLI );
  GMM_RETURN;
} /* kern_llgmm_assign() */
/*
+------------------------------------------------------------------------------
| Function    : kern_gmmsm_establish_rej
+------------------------------------------------------------------------------
| Description : This procedure informs SM that gprs is off. 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_gmmsm_establich_rej ( void )
{ 
  GMM_TRACE_FUNCTION( "kern_gmmsm_stablich_rej" );

#ifdef GMM_TCS4 

  {
     PALLOC (mmpm_detach_ind, MMPM_DETACH_IND);
     mmpm_detach_ind->ps_cause = kern_make_new_cause();
     /*For reattach, overwrite the cause from kern_make_new_cause()*/
     if (GMM_DT_RE_ATTACH == gmm_data->kern.detach_cap.detach_type) {
       mmpm_detach_ind->ps_cause.ctrl_value = CAUSE_is_from_mm;
       mmpm_detach_ind->ps_cause.value.mm_cause = 
                                 (U16)CAUSE_MM_DETACH_WITH_REATTACH;
     }
     PSEND ( hCommSM, mmpm_detach_ind);
  }
  
#else  /*New TI DK primitive should be sent*/
 if ( gmm_data->kern.attach_cap.gmmsm )
  {
    PALLOC ( gmmsm_establish_rej, GMMSM_ESTABLISH_REJ );
      gmmsm_establish_rej->sm_cause = gmm_data->kern.detach_cap.error_cause; 
    PSEND ( hCommSM, gmmsm_establish_rej );
  }
  else // if !gmmsm 
  {
    
    //  <R.GMM.PRELIND.M.001>
    
    PALLOC ( gmmsm_release_ind, GMMSM_RELEASE_IND);
    PSEND ( hCommSM, gmmsm_release_ind );
  } 

#endif



  GMM_RETURN;
} /* kern_gmmsm_establish_rej() */


/*
+------------------------------------------------------------------------------
| Function    : kern_llgmm_unassign
+------------------------------------------------------------------------------
| Description : This procedure unassigned all TLLIs in LLC 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_llgmm_unassign ( void )
{ 
  GMM_TRACE_FUNCTION( "kern_llgmm_unassign" );

  switch (gmm_data->kern.attach_cap.llc_state)
  {
    case GMM_LLC_STATE_UNASSIGNED:
      GMM_RETURN;
    default:
    case GMM_LLC_STATE_ASSIGNED:
    case GMM_LLC_STATE_SUSPENED:
    case GMM_LLC_STATE_SUSPENED_RAU:
      gmm_data->kern.attach_cap.llc_state = GMM_LLC_STATE_UNASSIGNED;
      break;
  }



  /*
   * PTMSI is still valid for next attach
   * gmm_data->ptmsi.current = GMMRR_TMSI_INVALID;
   */
  gmm_data->ptmsi.new_grr = GMMRR_TMSI_INVALID;

  gmm_data->tlli.old=gmm_data->tlli.current;
  gmm_data->tlli.current_type = INVALID_TLLI;
  gmm_data->tlli.current=GMMRR_TMSI_INVALID;

  TRACE_0_OUT_PARA("Unassign");
  {
    PALLOC ( gmmrr_assign_req, GMMRR_ASSIGN_REQ );
      gmmrr_assign_req->old_tlli  = GMMRR_TLLI_INVALID;
      gmmrr_assign_req->new_tlli  = GMMRR_TLLI_INVALID;
      gmmrr_assign_req->old_ptmsi = GMMRR_TMSI_INVALID;
      gmmrr_assign_req->new_ptmsi = GMMRR_TMSI_INVALID;
      gmmrr_assign_req->imsi  = gmm_data->imsi;
      gmmrr_assign_req->rai.plmn.v_plmn=TRUE;
      memcpy (gmmrr_assign_req->rai.plmn.mcc, gmm_data->kern.attach_cap.rai_accepted.mcc, SIZE_MCC);
      memcpy (gmmrr_assign_req->rai.plmn.mnc, gmm_data->kern.attach_cap.rai_accepted.mnc, SIZE_MNC);
      gmmrr_assign_req->rai.lac = gmm_data->kern.attach_cap.rai_accepted.lac;
      gmmrr_assign_req->rai.rac = gmm_data->kern.attach_cap.rai_accepted.rac;
    PSEND ( hCommGRR, gmmrr_assign_req );
  }
  {
    PALLOC (llgmm_assign_req, LLGMM_ASSIGN_REQ);

    /* gmm_data->tlli.current_type=INVALID_TLLI; */
    
    llgmm_assign_req->old_tlli = LLGMM_TLLI_INVALID;
    llgmm_assign_req->new_tlli = LLGMM_TLLI_INVALID;

    memset (llgmm_assign_req->llgmm_kc.key,0, sizeof (llgmm_assign_req->llgmm_kc.key));
   
    llgmm_assign_req->ciphering_algorithm = LLGMM_CIPHER_NO_ALGORITHM;    
    kern_reset_cipher ();
    
    PSEND ( hCommLLC, llgmm_assign_req );
  }
 
  GMM_RETURN;     
} /* kern_llgmm_unassign() */
/*
+------------------------------------------------------------------------------
| Function    : kern_llgmm_suspend
+------------------------------------------------------------------------------
| Description : This procedure suspends  LLC 
|
| Parameters  : susp_cause - cause off suspension
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_llgmm_suspend ( UBYTE susp_cause)
{ 
  U8 cell_state=GMMREG_CS_CALL;
#ifdef GMM_TCS4 
  U8 mm_cause = CAUSE_MM_DEDICATED_MODE;      
#endif

  GMM_TRACE_FUNCTION( "kern_llgmm_suspend" );
  /*
   * if LLC is in suspended RAU it means that GRR has opned the GMM queue
   * so we have to remember.
   */
  switch (gmm_data->kern.attach_cap.llc_state)
  {
    case GMM_LLC_STATE_SUSPENED_RAU:
      GMM_RETURN;
    case GMM_LLC_STATE_UNASSIGNED:
      TRACE_ERROR ("suspending of LLC in state unassigned ignored");
      GMM_RETURN;
    default:
    case GMM_LLC_STATE_ASSIGNED:
    case GMM_LLC_STATE_SUSPENED:
      break;
  }

  switch (susp_cause)
  {
    case LLGMM_CALL:
      TRACE_0_OUT_PARA("call");
      
      cell_state=GMMREG_CS_CALL;
#ifdef GMM_TCS4
      mm_cause = CAUSE_MM_DEDICATED_MODE;      
#endif
      break;

    case LLGMM_LIMITED:      
      TRACE_0_OUT_PARA("limited");      
      cell_state=GMMREG_LIMITED_SERVICE;
#ifdef GMM_TCS4
      mm_cause = CAUSE_MM_LIMITED_SERVICE;      
#endif
      break;
      
    case LLGMM_NO_GPRS_SERVICE:
      TRACE_0_OUT_PARA("no_service");
      cell_state=GMMREG_GPRS_NOT_SUPPORTED;
#ifdef GMM_TCS4
      mm_cause = CAUSE_MM_NO_SERVICE;      
#endif
      break;

    case LLGMM_RAU:
      cell_state=GMMREG_TRY_TO_UPDATE;
#ifdef GMM_TCS4
      mm_cause = CAUSE_MM_TRY_TO_UPDATE;      
#endif

      if(GET_STATE(CU) EQ CU_CELL_RES_SENT
      || GET_STATE(CU) EQ CU_REQUESTED_CELL_RES_SENT)
      {
        susp_cause= LLGMM_PERIODIC_RAU;
        /* NO break; */
      }
      else
      {
        gmm_data->kern.attach_cap.llc_state=GMM_LLC_STATE_SUSPENED_RAU;
        TRACE_0_OUT_PARA("rau");          
        break;
      }
      /* GS, how would you solve it with V8?*/ 
    case LLGMM_PERIODIC_RAU:
      if (GMM_LLC_STATE_SUSPENED==gmm_data->kern.attach_cap.llc_state)
      {
        GMM_RETURN;
      }
      else
      {
        TRACE_0_OUT_PARA("periodic rau");
        cell_state=GMMREG_TRY_TO_UPDATE;
#ifdef GMM_TCS4
        mm_cause = CAUSE_MM_TRY_TO_UPDATE;      
#endif
      }
      break;
    default:
      TRACE_ERROR("llc_susp unexpected");
      break;
  }
  
  if (GMM_LLC_STATE_SUSPENED_RAU!=gmm_data->kern.attach_cap.llc_state)
  {
    gmm_data->kern.attach_cap.llc_state= GMM_LLC_STATE_SUSPENED;
  }
  
  {
    PALLOC (llgmm_suspend_req, LLGMM_SUSPEND_REQ);
      llgmm_suspend_req->susp_cause = susp_cause;
    PSEND  ( hCommLLC, llgmm_suspend_req );
  }

  {
    PALLOC(gmmreg_suspend_ind, GMMREG_SUSPEND_IND);
      gmmreg_suspend_ind->cell_state= cell_state;
    PSEND(hCommMMI,gmmreg_suspend_ind);
  }
#ifdef GMM_TCS4 
  {
    PALLOC(mmpm_suspend_ind, MMPM_SUSPEND_IND);
      mmpm_suspend_ind->ps_cause.ctrl_value = CAUSE_is_from_mm;      
      mmpm_suspend_ind->ps_cause.value.mm_cause = mm_cause;      
    PSEND(hCommSM,mmpm_suspend_ind);
  }
#endif 
  GMM_RETURN;     
} /* kern_llgmm_suspend() */
/*
+------------------------------------------------------------------------------
| Function    : kern_llgmm_resume
+------------------------------------------------------------------------------
| Description : This procedure resumes  LLC 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_llgmm_resume ( void )
{ 
  GMM_TRACE_FUNCTION( "kern_llgmm_resume" );
 
  switch(gmm_data->kern.attach_cap.llc_state)
  {
    case GMM_LLC_STATE_SUSPENED_RAU:
    case GMM_LLC_STATE_SUSPENED:
      {
        PALLOC (llgmm_resume_req, LLGMM_RESUME_REQ);
          gmm_data->kern.attach_cap.llc_state=GMM_LLC_STATE_ASSIGNED;
        PSEND  ( hCommLLC, llgmm_resume_req );
      }
      {
        PALLOC (gmmreg_resume_ind, GMMREG_RESUME_IND);
        PSEND  ( hCommMMI, gmmreg_resume_ind );
      }
    
#ifdef GMM_TCS4
      {
        PALLOC(mmpm_resume_ind, MMPM_RESUME_IND);
        PSEND(hCommSM,mmpm_resume_ind);
      }
#endif
      break;
    case GMM_LLC_STATE_ASSIGNED:
    default:
      break;
    case GMM_LLC_STATE_UNASSIGNED:
      TRACE_ERROR("resuming LLC in state unassigned");
      kern_llgmm_assign();
      break;
  }
  kern_gmmrr_stop_waiting_for_transmission();
  GMM_RETURN;     
} /* kern_llgmm_suspend() */


/*
+------------------------------------------------------------------------------
| Function    : kern_gmmrr_enable
+------------------------------------------------------------------------------
| Description : The function kern_enable() 
|               disables GRR and activates MM to act as a class CC mobile.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_gmmrr_enable ( void )
{
  GMM_TRACE_FUNCTION ("kern_gmmrr_enable()");
  gmm_data->kern.attach_cap.mobile_class_changed = FALSE;
  {
    PALLOC ( gmmrr_enable_req, GMMRR_ENABLE_REQ );
      TRACE_EVENT ("GRR is on");
      gmm_data->kern.attach_cap.grr_state = GMM_GRR_STATE_ON;
      gmmrr_enable_req->mobile_class = gmm_data->kern.attach_cap.mobile_class;
      gmmrr_enable_req->acc_contr_class = gmm_data->acc_contr_class;
      if (DRX_NOT_USED==gmm_data->drx_parameter.split_pg_cycle_code)
      {
         gmmrr_enable_req->split_pg_cycle = 704/* GMMREG_NO_DRX*/; 
      }
      else if (gmm_data->drx_parameter.split_pg_cycle_code<65)
      {
        gmmrr_enable_req->split_pg_cycle 
          = gmm_data->drx_parameter.split_pg_cycle_code; 
      } 
      else if (gmm_data->drx_parameter.split_pg_cycle_code<=98)
      {
        const USHORT cycle_array[] = 
        {
          71, 72, 74, 75, 77, 79, 80, 83, 86, 88, 90, 92, 96, 101, 103, 107,
          112,116, 118, 128, 141, 144, 150, 160, 171, 176, 192, 214, 224, 235, 
          256, 288, 320, 352 
        };
        
        gmmrr_enable_req->split_pg_cycle
           = cycle_array[gmm_data->drx_parameter.split_pg_cycle_code-65];
      }
      else
      {
        gmmrr_enable_req->split_pg_cycle = 1;
      } 

      if( gmm_data->drx_parameter.split_on_ccch EQ SPLIT_NO )
      {
        gmmrr_enable_req->spgc_ccch_supp = GMMRR_SPGC_CCCH_SUPP_NO;
      }
      else
      {
        gmmrr_enable_req->spgc_ccch_supp = GMMRR_SPGC_CCCH_SUPP_YES;
      }

      gmmrr_enable_req->gmmrr_non_drx_timer = gmm_data->drx_parameter.non_drx_timer;

    PSEND ( hCommGRR, gmmrr_enable_req );
  }
  GMM_RETURN;
} /* kern_gmmrr_ensable () */

/*
+------------------------------------------------------------------------------
| Function    : kern_gmmrr_disable
+------------------------------------------------------------------------------
| Description : The function kern_gmmrr_disable switches off GRR
|
| Parameters  : cm_estasblish_res - OK or NOT to establish the CM
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_gmmrr_disable ( void )
{
  GMM_TRACE_FUNCTION ("kern_gmmrr_disable()");
  TRACE_EVENT ("GRR is off");

  /*gmm_data->kern.attach_cap.mobile_class = GMMREG_CLASS_CC;*/
  gmm_data->kern.attach_cap.attach_type &= ~GMMREG_AT_GPRS;

  gmm_data->kern.attach_cap.grr_state = GMM_GRR_STATE_OFF;
  gmm_data->kern.suspension_type &= ~GMM_SUSP_LOCAL_DETACH;

  /* This code is applicable only for Release 99 */
#ifdef REL99
  gmm_data->ptmsi_signature.available = FALSE;
  gmm_data->ptmsi_signature.value     = INVALID_PTMSI_SIGNATURE;
#endif

  kern_llgmm_unassign();


  {
    PALLOC ( gmmrr_disable_req, GMMRR_DISABLE_REQ );
    PSEND ( hCommGRR, gmmrr_disable_req );
  }
  /* kern_mm_activate_mm (); */
  GMM_RETURN;
} /* kern_gmmrr_disable () */
/*
+------------------------------------------------------------------------------
| Function    : kern_mm_cm_establish_res
+------------------------------------------------------------------------------
| Description : The function mm_cm_establish_res is a response whether a CS call
|                is possible or not
|
| Parameters  : cm_estasblish_res - OK or NOT to establish the CM
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_mm_cm_establish_res ( UBYTE cm_establish_res  )
{
  GMM_TRACE_FUNCTION ("kern_mm_cm_establish_res ()");
  {
    PALLOC ( mmgmm_cm_establish_res, MMGMM_CM_ESTABLISH_RES);
      mmgmm_cm_establish_res->cm_establish_res = cm_establish_res;
      gmm_data->kern.suspension_type &= ~GMM_SUSP_CALL;
    PSEND ( hCommMM, mmgmm_cm_establish_res );
  }
  GMM_RETURN;
} /* kern_mm_establish_res () */
/*
+------------------------------------------------------------------------------
| Function    : kern_mm_cm_emergency_res
+------------------------------------------------------------------------------
| Description : The function mm_cm_emrgency_res is a response whether a
|               emergnecy call is possible or not
|
| Parameters  : cm_estasblish_res - OK or NOT to establish the CM
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_mm_cm_emergency_res ( UBYTE cm_establish_res  )
{
  GMM_TRACE_FUNCTION ("kern_mm_cm_emergency_res ()");
  {
    PALLOC ( mmgmm_cm_emergency_res, MMGMM_CM_EMERGENCY_RES);
      mmgmm_cm_emergency_res->cm_establish_res = cm_establish_res;
    PSEND ( hCommMM, mmgmm_cm_emergency_res );
    gmm_data->kern.suspension_type &= ~GMM_SUSP_EM_CALL;
  }
  GMM_RETURN;
} /* kern_mm_emergency_res () */
/*
+------------------------------------------------------------------------------
| Function    : kern_mm_net_req
+------------------------------------------------------------------------------
| Description : The function kern_mm_net_req sends the promitve to MM
|       
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_mm_net_req ( void )
{ 
  GMM_TRACE_FUNCTION( "kern_mm_net_req" );
  {
    PALLOC ( mmgmm_net_req, MMGMM_NET_REQ );
    PSEND ( hCommMM, mmgmm_net_req);
  }
  GMM_RETURN;
} /* kern_mm_net_req() */
/*
+------------------------------------------------------------------------------
| Function    : kern_mm_start_t3212
+------------------------------------------------------------------------------
| Description : This procedure is called: "If the detach type information
|               element value indicates "GPRS detach without switching off ?and
|               the MS is attached for GPRS and non-GPRS services and the 
|               network operates in network operation mode I, then if in the MS
|               the timer T3212 is not already running, the timer T3212 shall be
|               set to its initial value and restarted after the DETACH REQUEST
|               message has been sent."
|
|               <R.GMM.DINITM.M.007>
|    
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_mm_start_t3212 ( void )
{ 
  GMM_TRACE_FUNCTION( "kern_mm_start_t3212" );
  {  
    PALLOC ( mmgmm_start_t3212_req, MMGMM_START_T3212_REQ );
    PSEND ( hCommMM, mmgmm_start_t3212_req);
  }
  GMM_RETURN;
} /* kern_mm_start_t3212() */
/*
+------------------------------------------------------------------------------
| Function    : kern_ssim_authentication_req
+------------------------------------------------------------------------------
| Description : The function kern_sim_authentication_req
|       
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_sim_authentication_req ( UBYTE *random, UBYTE cksn )
{ 
  GMM_TRACE_FUNCTION( "kern_sim_authentication_req" );
  {
    PALLOC (sim_authentication_req, SIM_AUTHENTICATION_REQ);
      sim_authentication_req->source = SRC_GMM;
#ifdef GMM_TCS4
      sim_authentication_req->req_id = gmm_data->kern.auth_cap.last_auth_req_id;
#endif
      memcpy ( 
        sim_authentication_req->rand, random, MAX_RAND);
        sim_authentication_req->cksn = cksn;
    PSEND (hCommSIM, sim_authentication_req);
  }
  GMM_RETURN;
} /* kern_mm_authentication_req() */
/*
+------------------------------------------------------------------------------
| Function    : kern_mm_attach_started
+------------------------------------------------------------------------------
| Description : The function kern_mm_attach_started sends the primitive 
|                mmgmm_attach_started_req to MM
|       
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_mm_attach_started ( void )
{ 
  GMM_TRACE_FUNCTION( "kern_mm_attach_started" );

  SET_STATE ( MM, GMM_MM_REG_INITATED_VIA_GPRS);
  {  
    PALLOC ( mmgmm_attach_started_req, MMGMM_ATTACH_STARTED_REQ );
    PSEND ( hCommMM, mmgmm_attach_started_req);
  }
  GMM_RETURN;
} /* kern_mm_attach_started() */
/*
+------------------------------------------------------------------------------
| Function    : kern_mm_auth_rej
+------------------------------------------------------------------------------
| Description : The function kern_mm_atuth_rej sends the primitive 
|                mmgmm_auth_req to MM
|       
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_mm_auth_rej ( void )
{ 
  GMM_TRACE_FUNCTION( "kern_mm_auth_rej" );  
  {  
    PALLOC ( mmgmm_auth_rej_req, MMGMM_AUTH_REJ_REQ );
    PSEND ( hCommMM, mmgmm_auth_rej_req);  
  }
  GMM_RETURN;
} /* kern_mm_attach_rej() */
/*
+------------------------------------------------------------------------------
| Function    : kern_mm_attach_rej
+------------------------------------------------------------------------------
| Description : The function kern_mm_attach_rej sends the primitive 
|                mmgmm_attach_rej_req to MM.  The primitive is always sent if 
|               responce from MM is expected, .i.e no MMGMM_NREG_CNF
|       
| Parameters  : error_cause - the error cause given by the net or
|               MMGMM_AAC_OVER_5 if attach_attempt_counter is greater or equal 
|               than 5
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_mm_attach_rej ( USHORT error_cause )
{ 
  GMM_TRACE_FUNCTION( "kern_mm_attach_rej" );

    //if (GMM_MM_REG_INITATED_VIA_GPRS==gmm_data->mm_imsi_attached)
    {
      PALLOC ( mmgmm_attach_rej_req, MMGMM_ATTACH_REJ_REQ );
        /*
         * Anite B2 TC 44.2.1.2.8
         */
      SET_STATE ( MM, GMM_MM_DEREG);

      mmgmm_attach_rej_req-> cause = error_cause;  /* TCS 2.1 */
      PSEND ( hCommMM, mmgmm_attach_rej_req);  
    }
    /*
    else
    {
        kern_mm_imsi_detach_ind ( error_cause, MMGMM_DETACH_DONE);
    }
    */
  GMM_RETURN;
} /* kern_mm_attach_rej() */

/*
+------------------------------------------------------------------------------
| Function    : kern_mm_attach_acc
+------------------------------------------------------------------------------
| Description : The function kern_mm_attach_acc  sends the promitve to MM. 
|       
| Parameters  : mobile_identity -  TMSI given by the network
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_mm_attach_acc ( BOOL                v_mobile_identity,  /* TCS 2.1 */
                                 T_mobile_identity * mobile_identity, /* TCS 2.1 */
                                 BOOL                v_eqv_plmn_list, /* TCS 2.1 */
                                 T_eqv_plmn_list   * eqv_plmn_list /* TCS 2.1 */
                               ) /* TCS 2.1 */
{ 
  GMM_TRACE_FUNCTION( "kern_mm_attach_acc" );
  /* 
   * HM: This catches never...
   * Patch HM 07-Aug-01, make GMM2503 pass >>>
   * GMM thinks it performs a combined attach in network mode I, 
   * but the truth is we came from network mode I and are now 
   * performing the non-combined procedures in network mode II.
   */
  if ( GMM_MM_REG_INITATED_VIA_GPRS == GET_STATE(MM)  )
  /* Patch HM 07-Aug-01, make GMM2503 pass <<< */
  {
    PALLOC ( mmgmm_attach_acc_req, MMGMM_ATTACH_ACC_REQ );

    sig_kern_sync_set_mm_state ( MMGMM_FULL_SERVICE );
    
    gmm_data->kern.mm_cell_env.rai.lac = gmm_data->kern.sig_cell_info.env.rai.lac;
    gmm_data->kern.mm_cell_env.rai.plmn = gmm_data->kern.sig_cell_info.env.rai.plmn;
    mmgmm_attach_acc_req->v_tmsi = v_mobile_identity;
    mmgmm_attach_acc_req->lac = gmm_data->kern.mm_cell_env.rai.lac;
    mmgmm_attach_acc_req->plmn.v_plmn = TRUE;
    memcpy(mmgmm_attach_acc_req->plmn.mnc,gmm_data->kern.mm_cell_env.rai.plmn.mnc,SIZE_MNC);  
    memcpy (mmgmm_attach_acc_req->plmn.mcc, gmm_data->kern.mm_cell_env.rai.plmn.mcc,SIZE_MCC);
    mmgmm_attach_acc_req->tmsi = 
      (v_mobile_identity?gmm_data->tmsi:MMGMM_TMSI_INVALID);
    mmgmm_attach_acc_req->v_equ_plmn_list = v_eqv_plmn_list; /* TCS 2.1 */
    if (v_eqv_plmn_list)  /* TCS 2.1 */
      memcpy(&mmgmm_attach_acc_req->equ_plmn_list, eqv_plmn_list, sizeof(T_equ_plmn_list)); /* TCS 2.1 */
    PSEND ( hCommMM, mmgmm_attach_acc_req);
  }
  else
  {
    PALLOC ( mmgmm_allowed_req, MMGMM_ALLOWED_REQ );
    mmgmm_allowed_req->lac = gmm_data->kern.attach_cap.rai_accepted.lac;
    mmgmm_allowed_req->plmn.v_plmn = TRUE;
    memcpy(mmgmm_allowed_req->plmn.mnc,gmm_data->kern.attach_cap.rai_accepted.mnc,SIZE_MNC);  
    memcpy (mmgmm_allowed_req->plmn.mcc, gmm_data->kern.attach_cap.rai_accepted.mcc,SIZE_MCC);
    mmgmm_allowed_req->v_equ_plmn_list = v_eqv_plmn_list; /* TCS 2.1 */
    if (v_eqv_plmn_list) /* TCS 2.1 */
      memcpy(&mmgmm_allowed_req->equ_plmn_list, eqv_plmn_list, sizeof(T_equ_plmn_list));   /* TCS 2.1 */
    PSEND ( hCommMM, mmgmm_allowed_req);
  }
  GMM_RETURN;
} /* kern_mm_attach_acc() */

/*
+------------------------------------------------------------------------------
| Function    : kern_mm_reg_req
+------------------------------------------------------------------------------
| Description : The function kern_mm_reg_req sends the primitive 
|                MMGMM_REG_REQ to MM
|       
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_mm_reg_req (U8 reg_type, U8 bootup_act)
{ 
  GMM_TRACE_FUNCTION( "kern_mm_reg_req" );  
  {  
    PALLOC ( mmgmm_reg_req, MMGMM_REG_REQ ); /* T_MMGMM_REG_REQ */
    mmgmm_reg_req->reg_type     = reg_type;
    mmgmm_reg_req->service_mode = gmm_data->kern.attach_cap.service_mode;
    mmgmm_reg_req->mobile_class = gmm_data->kern.attach_cap.mobile_class;
    mmgmm_reg_req->bootup_act   = bootup_act;
    PSEND ( hCommMM, mmgmm_reg_req );
  }
  GMM_RETURN;
} /* kern_mm_attach_rej() */

/*
+------------------------------------------------------------------------------
| Function    : kern_mm_activate_rr
+------------------------------------------------------------------------------
| Description : The MMGMM_ACTIVATE_REQ is used to initiate GPRS Cell Selection
|                in RR. Cell selection for CS only is triggered implicitly be 
|                activating MM (i.e. the MS is operates as a class CC mobile) by
|                sending the primitive discribed in function kern_mm_activate_mm
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_mm_activate_rr ( void )
{
  GMM_TRACE_FUNCTION ("kern_mm_activate_rr()");

  if (gmm_data->kern.attach_cap.network_selection_mode EQ MODE_AUTO)
  {
    kern_mm_reg_req (REG_CELL_SEARCH_ONLY, NORMAL_REG);
  }
  else
  {
    PALLOC ( mmgmm_plmn_res, MMGMM_PLMN_RES ); /* T_MMGMM_PLMN_RES */
      mmgmm_plmn_res->plmn         = gmm_data->kern.attach_cap.plmn_requested;
      mmgmm_plmn_res->reg_type     = REG_CELL_SEARCH_ONLY;
      mmgmm_plmn_res->mobile_class = gmm_data->kern.attach_cap.mobile_class;
    PSEND ( hCommMM, mmgmm_plmn_res );
  }

  kern_gmmrr_enable();
  GMM_RETURN;
} /* kern_mm_activate_rr () */

/*
+------------------------------------------------------------------------------
| Function    : kern_mm_activate_mm
+------------------------------------------------------------------------------
| Description : The MM_ACTIVATE_MM_REQ is used to request MM to act as
|               an GSM-only mobile. 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_mm_activate_mm ( void )
{
  GMM_TRACE_FUNCTION ("kern_mm_activate_mm()");
  
  if (GMMREG_CLASS_CG!=gmm_data->kern.attach_cap.mobile_class)
  {
    /*
     * remember lau initiated
     */
     gmm_data->kern.attach_cap.mm_lau_attempted = TRUE;

     if (gmm_data->kern.attach_cap.network_selection_mode EQ MODE_AUTO)
     {
        kern_mm_reg_req (REG_GPRS_INACTIVE, NORMAL_REG);
     }
     else
     {
        PALLOC ( mmgmm_plmn_res, MMGMM_PLMN_RES ); /* T_MMGMM_PLMN_RES */
        mmgmm_plmn_res->plmn         = gmm_data->kern.attach_cap.plmn_requested;
        mmgmm_plmn_res->reg_type     = REG_GPRS_INACTIVE;
        mmgmm_plmn_res->mobile_class = gmm_data->kern.attach_cap.mobile_class;
        PSEND ( hCommMM, mmgmm_plmn_res );
     }
  }
  GMM_RETURN;
} /* kern_mm_activate_rr () */

/*
+------------------------------------------------------------------------------
| Function    : kern_local_detach_open_proc
+------------------------------------------------------------------------------
| Description : The function kern_local_detach_open_proc() calls all open 
|            issues to be done upon receiving GMMRR_SUSPEND_CNF
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_local_detach_open_proc (void )
{
  GMM_TRACE_FUNCTION( "kern_local_detach_open_proc" );

  TRACE_1_INFO ("open %d",gmm_data->kern.detach_cap.local_detach_open_proc);


  switch ( gmm_data->kern.detach_cap.local_detach_open_proc)
  {
    case GMM_LOCAL_DETACH_PROC_RESUME:
      kern_resume_grr_der();
      break;
    case GMM_LOCAL_DETACH_PROC_ENTER_NULL_IMSI:
      SET_STATE (KERN, KERN_GMM_NULL_IMSI);
      EM_GMM_SET_STATE(KERN_GMM_NULL_IMSI);
      /* If GMM entered No Cell Available & after coming back to GSM only N/W, the user does a manual 
       * detach, then it is necessary for GMM to disable GRR & enable GSM only mode through MM */
      kern_gmmrr_disable();
      kern_mm_activate_mm();
      break;
    case GMM_LOCAL_DETACH_PROC_ENTER_NULL_NO_IMSI:
      SET_STATE (KERN, KERN_GMM_NULL_NO_IMSI);
      EM_GMM_SET_STATE(KERN_GMM_NULL_NO_IMSI);

      break;
    case GMM_LOCAL_DETACH_PROC_ENTER_NULL_IMSI_LIMITED_SERVICE_REQ:
      SET_STATE (KERN, KERN_GMM_NULL_IMSI_LIMITED_SERVICE_REQ);
      EM_GMM_SET_STATE(KERN_GMM_NULL_IMSI_LIMITED_SERVICE_REQ);

      break;
    case GMM_LOCAL_DETACH_PROC_ENTER_REG_NORMAL:
      kern_enter_reg_normal();
      break;
    case GMM_LOCAL_DETACH_PROC_ENTER_DEREG:
      kern_enter_der();
      break;
    case GMM_LOCAL_DETACH_PROC_UNASSIGN:
      kern_llgmm_unassign();
      kern_resume_grr_der();
      break;      
    case GMM_LOCAL_DETACH_PROC_SUSP_LAU:
      gmm_data->kern.suspension_type &= ~GMM_SUSP_LOCAL_DETACH;
      kern_mm_lau();
      break;
    case GMM_LOCAL_DETACH_PROC_RE_ATTACH:
    
      TRACE_0_OUT_PARA("Unassign");
      gmm_data->tlli.old=gmm_data->tlli.current;
      gmm_data->tlli.current_type = INVALID_TLLI;
      gmm_data->tlli.current=GMMRR_TMSI_INVALID;
      gmm_data->kern.attach_cap.llc_state = GMM_LLC_STATE_UNASSIGNED;
      {
        PALLOC (llgmm_assign_req, LLGMM_ASSIGN_REQ);

        gmm_data->tlli.current_type=INVALID_TLLI;
          
        llgmm_assign_req->old_tlli = LLGMM_TLLI_INVALID;
        llgmm_assign_req->new_tlli = LLGMM_TLLI_INVALID;

        memset (llgmm_assign_req->llgmm_kc.key,0, sizeof (llgmm_assign_req->llgmm_kc.key));
         
        kern_reset_cipher ();
        llgmm_assign_req->ciphering_algorithm = LLGMM_CIPHER_NO_ALGORITHM;
          
        PSEND ( hCommLLC, llgmm_assign_req );
      }

      kern_resume_grr_der();

      //kern_attach_reset();
      //kern_attach();

      break;
    case GMM_LOCAL_DETACH_PROC_RAU:
      kern_rau();
      break;
    case GMM_LOCAL_DETACH_PROC_DISABLE:
      if (gmm_data->sim_gprs_invalid)
      {
        SET_STATE ( KERN, KERN_GMM_NULL_NO_IMSI);
        EM_GMM_SET_STATE(KERN_GMM_NULL_NO_IMSI);

      }
      else
      {
        SET_STATE ( KERN, KERN_GMM_NULL_IMSI);
        EM_GMM_SET_STATE(KERN_GMM_NULL_IMSI);

      }
      
      kern_gmmrr_disable();
      kern_mm_activate_mm ();
      break;
    case GMM_LOCAL_DETACH_PROC_COMB_DISABLE:
      SET_STATE ( KERN, KERN_GMM_NULL_NO_IMSI);
      EM_GMM_SET_STATE(KERN_GMM_NULL_NO_IMSI);
      gmm_data->kern.attach_cap.attach_type = GMMREG_AT_NOT_KNOWN;
      kern_gmmrr_disable();
      kern_mm_activate_mm ();
      break;
    case GMM_LOCAL_DETACH_PROC_AUTH_FAILED:
      SET_STATE ( KERN, KERN_GMM_DEREG_NO_IMSI);
      EM_GMM_SET_STATE(KERN_GMM_DEREG_NO_IMSI);
      gmm_data->kern.attach_cap.attach_type = GMMREG_AT_NOT_KNOWN;
      kern_gmmrr_disable();
      kern_mm_imsi_detach_ind ( GMMCS_AUTHENTICATION_REJECTED,  /* TCS 2.1 */
        GET_STATE(MM)==GMM_MM_DEREG?
          MMGMM_DETACH_DONE:
          MMGMM_PERFORM_DETACH,			  
          gmm_data->kern.detach_cap.detach_type  );
      break;
    case GMM_LOCAL_DETACH_PROC_SIM_REMOVED:
      SET_STATE ( KERN, KERN_GMM_DEREG_NO_IMSI);
      EM_GMM_SET_STATE(KERN_GMM_DEREG_NO_IMSI);
      gmm_data->kern.attach_cap.attach_type = GMMREG_AT_NOT_KNOWN;
      kern_gmmrr_disable();
      kern_mm_imsi_detach_ind ( GMMCS_SIM_REMOVED,  /* TCS 2.1 */
        GET_STATE(MM)==GMM_MM_DEREG?
          MMGMM_DETACH_DONE:
          MMGMM_PERFORM_DETACH,
          gmm_data->kern.detach_cap.detach_type);

      break;
    case GMM_LOCAL_DETACH_PROC_POWER_OFF:
      SET_STATE (KERN, KERN_GMM_DEREG_NO_IMSI);
      EM_GMM_SET_STATE(KERN_GMM_DEREG_NO_IMSI);
      vsi_t_stop  ( GMM_handle, kern_TPOWER_OFF);
      kern_gmmrr_disable();
      kern_mm_imsi_detach_ind ( GMMCS_POWER_OFF,  /* TCS 2.1 */
        GET_STATE(MM)==GMM_MM_DEREG?
          MMGMM_DETACH_DONE:
          MMGMM_PERFORM_DETACH,
          gmm_data->kern.detach_cap.detach_type);

      break;
    case GMM_LOCAL_DETACH_PROC_SOFT_OFF:
      
      if (gmm_data->sim_gprs_invalid)
      {
        SET_STATE (KERN, KERN_GMM_DEREG_NO_IMSI);
        EM_GMM_SET_STATE(KERN_GMM_DEREG_NO_IMSI);
      }
      else
      {
          SET_STATE (KERN, KERN_GMM_NULL_IMSI);
          EM_GMM_SET_STATE(KERN_GMM_NULL_IMSI);
      }
      vsi_t_stop  ( GMM_handle, kern_TPOWER_OFF);
      kern_gmmrr_disable();
      kern_mm_imsi_detach_ind ( GMMCS_POWER_OFF,  /* TCS 2.1 */
        GET_STATE(MM)==GMM_MM_DEREG?
          MMGMM_DETACH_DONE:
          MMGMM_PERFORM_DETACH,
          gmm_data->kern.detach_cap.detach_type);

      break;

    case GMM_LOCAL_DETACH_PROC_NOT_CHANGED:
    case GMM_LOCAL_DETACH_PROC_NOTHING:
    default:
      if (GMM_GRR_STATE_SUSPENDED==gmm_data->kern.attach_cap.grr_state)
      {
        kern_resume_grr_der();
      }
      break;
  }
  gmm_data->kern.detach_cap.local_detach_open_proc=GMM_LOCAL_DETACH_PROC_NOTHING;
  GMM_RETURN;
} /* kern_kern_local_detach_open_proc () */

/*
+------------------------------------------------------------------------------
| Function    : kern_local_detach
+------------------------------------------------------------------------------
| Description : The function kern_local_detach() .... 
|
| Parameters  : error_cause - error cause
|
|               gmm_data->kern.detach_cap.detach_type MUST be set !
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_local_detach ( USHORT error_cause, BOOL det_acc_sent, T_LOCAL_DETACH_PROC local_detach_open_proc )
{
  /* 
   * compiler cl470.exe test: replace local variable detach_type with 
   *                          gmm_data...detach_type
   * => you may get an assembler error (because switch condition
   *    modified within case statement ??)
   */
  UBYTE detach_type = gmm_data->kern.detach_cap.detach_type;
  UBYTE kern_gmm_save_state = GET_STATE(KERN);

  GMM_TRACE_FUNCTION( "kern_local_detach" );

  if (GMM_LOCAL_DETACH_PROC_NOT_CHANGED!=local_detach_open_proc)
  {
    gmm_data->kern.detach_cap.local_detach_open_proc=local_detach_open_proc;
  }
  else
  {
    if(GMM_LOCAL_DETACH_PROC_NOTHING EQ gmm_data->kern.detach_cap.local_detach_open_proc)
    {
      TRACE_ERROR ("GMM_LOCAL_DETACH_PROC_NOTHING called");
    }
  }
  
  if ( gmm_data->kern.local_detached)
  /*local_detach_done */
  {
    TRACE_0_INFO ("kern_local_detach() called already");
    gmm_data->kern.local_detached = FALSE;
    kern_local_detach_open_proc();
    GMM_RETURN;
  }
  gmm_data->kern.local_detached = TRUE;
  
  switch (detach_type)
  {
    case GMMREG_DT_IMSI:
      break;
    case GMMREG_DT_POWER_OFF:
      SET_STATE ( KERN, KERN_GMM_DEREG_SUSPENDING );
      EM_GMM_SET_STATE(KERN_GMM_DEREG_SUSPENDING );

      kern_gmmsm_establich_rej();

      if (det_acc_sent)
      {
        gmm_data->kern.suspension_type            |= GMM_SUSP_LOCAL_DETACH;
        sig_kern_rdy_start_timer_req( kern_TPOWER_OFF, TPOWER_OFF_VALUE);
      }
      else
      {
        vsi_t_start ( GMM_handle , kern_TPOWER_OFF, TPOWER_OFF_VALUE);
        kern_gmmrr_suspend (GMMRR_NOT_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_LOCAL_DETACH);
      }
      break;
    default:
      SET_STATE ( KERN, KERN_GMM_DEREG_SUSPENDING );
      EM_GMM_SET_STATE(KERN_GMM_DEREG_SUSPENDING );
      kern_gmmsm_establich_rej();

      if (det_acc_sent)
      {
        gmm_data->kern.suspension_type            |= GMM_SUSP_LOCAL_DETACH;
        sig_kern_rdy_start_timer_req( kern_TLOCAL_DETACH, TLOCAL_DETACH_VALUE);
      }
      else
      {
        kern_gmmrr_suspend (GMMRR_NOT_SUSP_GPRS, GMMRR_NORMAL_RELEASE, GMM_SUSP_LOCAL_DETACH);
      }
      break;
  }  
  /*
   * LABEL UNASSIGN_GMMREG
   */

  vsi_t_stop ( GMM_handle , kern_T3321);

  switch ( detach_type )
  {
    case GMM_DT_RE_ATTACH:
      GMM_RETURN;
    case GMMREG_DT_LIMITED_SERVICE:
    case GMMREG_DT_SIM_REMOVED:
      gmm_data->kern.detach_cap.local_detach_open_proc=
        (GMMCS_AUTHENTICATION_REJECTED== error_cause? /* TCS 2.1 */
                   GMM_LOCAL_DETACH_PROC_AUTH_FAILED:
                   GMM_LOCAL_DETACH_PROC_SIM_REMOVED);
      kern_gmmreg_detach (gmm_data->kern.detach_cap.detach_type, error_cause,
                          GMMREG_SEARCH_NOT_RUNNING, error_cause);

      break;
    case GMMREG_DT_POWER_OFF:
    case GMMREG_DT_SOFT_OFF:
     /* If GPRS is already suspended and power switch off is to be done, 
      * kern_local_detach_open_proc() will not be called as GMM will not receive
      * GMMRR_SUSPEND_CNF from GRR, as GRR is already suspended */
      if (kern_gmm_save_state EQ KERN_GMM_REG_SUSPENDED)
      {
          gmm_data->kern.local_detached = FALSE;
          kern_local_detach_open_proc();
      }
      break;
    case GMMREG_DT_IMSI:
      kern_mm_attach_rej ( error_cause );
      kern_gmmreg_detach (gmm_data->kern.detach_cap.detach_type, error_cause,
                          GMMREG_SEARCH_NOT_RUNNING, error_cause);
      gmm_data->kern.local_detached = FALSE;
      kern_local_detach_open_proc();

      break;
    case GMMREG_DT_COMB:
      kern_mm_attach_rej ( error_cause );
      kern_gmmreg_detach (gmm_data->kern.detach_cap.detach_type, error_cause,
                          GMMREG_SEARCH_NOT_RUNNING, error_cause);
      break;
    case GMMREG_DT_DISABLE_GPRS:
      gmm_data->kern.attach_cap.mobile_class = GMMREG_CLASS_CC;
      gmm_data->kern.attach_cap.attach_type &= ~GMMREG_AT_GPRS;
      /* NO break; */
    case GMMREG_DT_GPRS:
      break;
    default:
      TRACE_ERROR ( "Unexpected detach_type in DETACH LABEL" );
      break;
  }

  GMM_RETURN;   
} /* kern_local_detach() */


/*
+------------------------------------------------------------------------------
| Function    : kern_get_imeisv
+------------------------------------------------------------------------------
| Description : The function kern_get_imeisv
|               copied from MM: void csf_read_imei (T_imsi *imei) 
|       
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_get_imeisv (T_gmobile_identity *imei)
{
  UBYTE buf[CL_IMEI_SIZE];


  GMM_TRACE_FUNCTION( "kern_get_imeisv" );
#ifdef TDSGEN
  imei->identity_digit[0]        = 0x04;
  imei->identity_digit[1]        = 0x04;
  imei->identity_digit[2]        = 0x06;
  imei->identity_digit[3]        = 0x00;
  imei->identity_digit[4]        = 0x01;
  imei->identity_digit[5]        = 0x09;
  imei->identity_digit[6]        = 0x01;
  imei->identity_digit[7]        = 0x09;
  imei->identity_digit[8]        = 0x07;
  imei->identity_digit[9]        = 0x05;
  imei->identity_digit[10]       = 0x00;
  imei->identity_digit[11]       = 0x07;
  imei->identity_digit[12]       = 0x05;
  imei->identity_digit[13]       = 0x09;
  imei->identity_digit[14]       = 0x00;
  imei->identity_digit[15]       = 0x00;/* Software Version Number */

#else /* TDSGEN */
  /*
   * Get IMEISV from IMEI common library
   */
  cl_get_imeisv(CL_IMEI_SIZE, buf, CL_IMEI_GET_SECURE_IMEI);
  imei->identity_digit[0]  = (buf [0] >> 4) & 0x0F; /* TAC 8 byte */
  imei->identity_digit[1]  =  buf [0] & 0x0F;
  imei->identity_digit[2]  = (buf [1] >> 4) & 0x0F;
  imei->identity_digit[3]  =  buf [1] & 0x0F;
  imei->identity_digit[4]  = (buf [2] >> 4) & 0x0F;
  imei->identity_digit[5]  =  buf [2] & 0x0F;
  imei->identity_digit[6]  = (buf [3] >> 4) & 0x0F;
  imei->identity_digit[7]  =  buf [3] & 0x0F;
  imei->identity_digit[8]  = (buf [4] >> 4) & 0x0F; /* SNR 6 byte */
  imei->identity_digit[9]  =  buf [4] & 0x0F;
  imei->identity_digit[10] = (buf [5] >> 4) & 0x0F;
  imei->identity_digit[11] =  buf [5] & 0x0F;
  imei->identity_digit[12] = (buf [6] >> 4) & 0x0F;
  imei->identity_digit[13] =  buf [6] & 0x0F;
  imei->identity_digit[14] = (buf [7] >> 4) & 0x0F; /* SV 2 byte */
  imei->identity_digit[15] =  buf [7] & 0x0F;
  TRACE_EVENT_P8("GMM INFO IMEI: TAC %1x%1x%1x%1x%1x%1x%1x%1x",
                  imei->identity_digit[0], imei->identity_digit[1],
                  imei->identity_digit[2], imei->identity_digit[3],
                  imei->identity_digit[4], imei->identity_digit[5],
                  imei->identity_digit[6], imei->identity_digit[7]);
  TRACE_EVENT_P6("GMM INFO IMEI: SNR %1x%1x%1x%1x%1x%1x",
                  imei->identity_digit[8],  imei->identity_digit[9],
                  imei->identity_digit[10], imei->identity_digit[11],
                  imei->identity_digit[12], imei->identity_digit[13]);
  TRACE_EVENT_P2("GMM INFO IMEI: SV  %1x%1x", imei->identity_digit[14],
                                              imei->identity_digit[15]);

#endif /* not defined TDSGEN */

  imei->v_identity_digit = TRUE;
  imei->c_identity_digit =  16;
  
  imei->type_of_identity = ID_TYPE_IMEISV;
  imei->odd_even   = 0;

  GMM_RETURN;
} /* kern_get_imeisv() */
/*
+------------------------------------------------------------------------------
| Function    : kern_get_imei
+------------------------------------------------------------------------------
| Description : The function kern_get_imei
|               copied from MM: void csf_read_imei (T_imsi *imei) 
|       
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_get_imei (T_gmobile_identity *imei)
{   
  GMM_TRACE_FUNCTION( "kern_get_imei" );
  kern_get_imeisv(imei);

  /*
     * get imeisv returns 16 digits, but imei has only 14 digits
     * thus clear last byte:
     */
    imei->identity_digit[14] =  0;
    imei->identity_digit[15] =  0;
    imei->v_identity_digit   = TRUE;
    imei->c_identity_digit   = 15;


  imei->type_of_identity = ID_TYPE_IMEI;
  imei->odd_even   = 1;
  GMM_RETURN;
} /* kern_get_imei() */
/*
+------------------------------------------------------------------------------
| Function    : kern_read_pcm
+------------------------------------------------------------------------------
| Description : The function kern_read_pcm
|       
| Parameters  : void
|
+------------------------------------------------------------------------------
*/

GLOBAL void kern_read_pcm ( void )
{
#ifdef _TARGET_    
  T_GMM_FFS ffsBuffer;
  T_FFS_SIZE size = ffs_fread("/GPRS/gmm", &ffsBuffer, sizeof(ffsBuffer));
  TRACE_FUNCTION( "kern_read_pcm" );

  
  if ( size != sizeof(ffsBuffer) )
  {
    if ( size < 0 )
    {
      TRACE_EVENT_P1("FFS can not read \"/GPRS/gmm\" (%d)", size);
    }
    else
    {
      TRACE_EVENT_P2("FFS contains old file of \"/GPRS/gmm\": %dBytes long, but %dBytes expected",
                      size, sizeof(ffsBuffer));
    }
  }
  else
  {
    gmm_data->config.cipher_on = ffsBuffer.cipher_on;
    
    TRACE_1_INFO("GEA2=%d", (gmm_data->config.cipher_on & 0x02)>0);
    TRACE_1_INFO("GEA3=%d", (gmm_data->config.cipher_on & 0x04)>0);
    TRACE_1_INFO("GEA4=%d", (gmm_data->config.cipher_on & 0x08)>0);
    TRACE_1_INFO("GEA5=%d", (gmm_data->config.cipher_on & 0x10)>0);
    TRACE_1_INFO("GEA6=%d", (gmm_data->config.cipher_on & 0x20)>0);
    TRACE_1_INFO("GEA7=%d", (gmm_data->config.cipher_on & 0x40)>0);
    
  }
#endif /* NOT DEFINED _SIMULATION_ */
  return;
}

/*
+------------------------------------------------------------------------------
| Function    : kern_build_ms_network_capability
+------------------------------------------------------------------------------
| Description : The function kern_build_ms_network_capability() .... 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_build_ms_network_capability (T_ms_network_capability *ms_network_capability)
{
  T_mob_class_2 mob_class_2;
  EXTERN  void rr_csf_get_classmark2 (T_mob_class_2 *mob_class_2);

  GMM_TRACE_FUNCTION ("kern_build_ms_network_capability()");

  rr_csf_get_classmark2 (&mob_class_2);

#ifdef REL99
  // HM 20-Jun-2003 Release 1999 >>>
  // Ensure these IEs are not accidently included in the uplink message
  // until Release 1999 is completely implemented in GMM
  memset (ms_network_capability, 0, sizeof (T_ms_network_capability));
  // HM 20-Jun-2003 Release 1999 <<<
#endif

  if (gmm_data->config.cipher_on)
  {
    ms_network_capability->gea_1                  = GEA_1_YES;
    TRACE_EVENT ("ciphering on requested");
  }
  else
  {
    ms_network_capability->gea_1                  = GEA_1_NO;
    TRACE_EVENT ("ciphering off requested");
  }

  ms_network_capability->ss_screening_indicator = mob_class_2.ss_screen;
  ms_network_capability->sm_capabilities_gsm    = mob_class_2.mt_pp_sms;
  ms_network_capability->sm_capabilities_gprs   = SM_CAP_GPRS_YES;
  ms_network_capability->ucs2_support           = UCS2_YES;
  /* 
   * R99 definitions must be set to 0
   */

    ms_network_capability->solsa_capability       = NO;
#ifdef REL99
    ms_network_capability->rev_level_ind          = REV_LEVEL_IND_99_YES;
    ms_network_capability->pfc_feature_mode       = YES;
#else
	ms_network_capability->rev_level_ind          = REV_LEVEL_IND_99_NO;
    ms_network_capability->pfc_feature_mode       = NO;
#endif
  /* 
   * end of R99 definitions
   */
  ms_network_capability->ext_gea_bits.gea_2     = (gmm_data->config.cipher_on & 0x02)>0;
  ms_network_capability->ext_gea_bits.gea_3     = (gmm_data->config.cipher_on & 0x04)>0;
  ms_network_capability->ext_gea_bits.gea_4     = (gmm_data->config.cipher_on & 0x08)>0;
  ms_network_capability->ext_gea_bits.gea_5     = (gmm_data->config.cipher_on & 0x10)>0;
  ms_network_capability->ext_gea_bits.gea_6     = (gmm_data->config.cipher_on & 0x20)>0;
  ms_network_capability->ext_gea_bits.gea_7     = (gmm_data->config.cipher_on & 0x40)>0;

#ifdef TDSGEN
  ms_network_capability->sm_capabilities_gsm    = SM_CAP_GSM_NO;
  ms_network_capability->ss_screening_indicator = 0;
#endif

  GMM_RETURN;
} /* kern_build_ms_network_capability () */

/*
+------------------------------------------------------------------------------
| Function    : kern_init
+------------------------------------------------------------------------------
| Description : The function kern_init() .... 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_init ( void )
{ 
  int i;
  TRACE_FUNCTION( "kern_init" );

  /*gmm_data->gprs_enabled = TRUE;*/
  /*
   * Initialise service KERN with state KERN_READY.
   */
  INIT_STATE ( KERN, KERN_GMM_NULL_NO_IMSI );

#ifdef TRACE_FUNC
#ifdef IDENTATION
  gmm_data->deep=0;
#endif
#endif
  gmm_data->anite               = FALSE;
  gmm_data->tlli.current        = LLGMM_TLLI_INVALID;
  gmm_data->tlli.old            = LLGMM_TLLI_INVALID;
  gmm_data->kern.local_detached = FALSE; 
  gmm_data->tlli.current_type   = INVALID_TLLI;
  INIT_STATE ( MM, GMM_MM_DEREG);
  gmm_data->gu                  = GU3_ROAMING_NOT_ALLOWED;
  gmm_data->acc_contr_class     = 0;
  gmm_data->kern.gprs_indicator  = GMM_GPRS_SUPP_NO;
#ifdef GMM_TCS4
  gmm_data->sync.sig_cell_info.rt = RT_GSM; /* TCS 4.0 */
#endif
 
  gmm_data->kern.attach_cap.attach_acc_after_po = FALSE;
  TRACE_EVENT ("GRR is off");
  gmm_data->kern.attach_cap.grr_state = GMM_GRR_STATE_OFF;
  /* 
   * set all kc values to 0xff
   */
  memset (gmm_data->kern.auth_cap.kc, 0x00, MAX_KC);

  gmm_data->kern.auth_cap.ciphering_algorithm = LLGMM_CIPHER_NO_ALGORITHM;
  /*
   * multiple outstanding SIM_AUTHENTICATION_REQ messages reset the counter
   */
  gmm_data->kern.auth_cap.last_auth_req_id = NOT_PRESENT_8BIT;
  gmm_data->config.cipher_on = 0x01;
  gmm_data->cipher = FALSE;
  gmm_data->config.preuse_off = FALSE;
  
  gmm_data->ptmsi.new_grr = GMMRR_TMSI_INVALID;
  gmm_data->ptmsi.current  = GMMRR_TMSI_INVALID;
  gmm_data->ptmsi.old  = GMMRR_TMSI_INVALID;
  gmm_data->tmsi  = GMMRR_TMSI_INVALID;
  gmm_data->config.nmo = 0xff;
  /*
   * set the dafault timer vlaues needed in service kern
   */
  gmm_data->kern.t3310_val      = T3310_VALUE;
  gmm_data->kern.t3311_val      = T3311_VALUE;
  gmm_data->kern.t3321_val      = T3321_VALUE;

  /*
   * T3302 is loaded with the same value which is used to load T3212.
   */ 
  gmm_data->kern.t3302_val      = T3302_VALUE;

  gmm_data->kern.timeout_t3312  = FALSE;

  vsi_t_stop  ( GMM_handle, kern_T3310);
  vsi_t_stop  ( GMM_handle, kern_T3311);
  vsi_t_stop  ( GMM_handle, kern_T3321);
  vsi_t_stop  ( GMM_handle, kern_TPOWER_OFF);
  vsi_t_stop  ( GMM_handle, kern_TLOCAL_DETACH);

  /*
   * which kind of attach will be used is unknown this time
   * nobody requested an attach 
   */
  
  gmm_data->kern.attach_cap.mobile_class    = GMMREG_CLASS_BG;
  gmm_data->kern.attach_cap.mobile_class_changed = FALSE;
  gmm_data->kern.attach_cap.attach_type     = GMMREG_AT_NOT_KNOWN;
  gmm_data->kern.attach_cap.service_mode    = SERVICE_MODE_LIMITED;
  gmm_data->kern.attach_cap.gmmreg          = FALSE;
#ifndef GMM_TCS4
  gmm_data->kern.attach_cap.gmmsm           = FALSE;
#endif 
  gmm_data->kern.detach_cap.network         = FALSE;
  gmm_data->kern.detach_cap.gmmreg          = FALSE;        
  gmm_data->kern.detach_cap.local_detach_open_proc=GMM_LOCAL_DETACH_PROC_NOTHING;
  gmm_data->kern.detach_cap.error_cause     = GMMCS_INT_NOT_PRESENT; /* TCS 2.1 */
  gmm_data->kern.suspension_type            = GMM_SUSP_NONE;
  /*
   * the DRX parameter are hardware dependend 
   * TI is not supporting DRX
   * <R.GMM.AGINIT.M.006>
   */

#ifdef _SIMULATION_
  gmm_data->drx_parameter.split_pg_cycle_code = 92 /* 95 DRX_NOT_USED */;
#ifdef TDSGEN
  gmm_data->drx_parameter.split_pg_cycle_code = 16;
#endif

#else  /* #ifdef _SIMULATION_ */
  gmm_data->drx_parameter.split_pg_cycle_code = 16 /* 95 DRX_NOT_USED */;
#endif /* #ifdef _SIMULATION_ */

  gmm_data->drx_parameter.split_on_ccch       = SPLIT_NO;
  gmm_data->drx_parameter.non_drx_timer       = DRX_2_SEC;


  /*
   * resets the attach and rau attempt counter
   */
  gmm_data->kern.aac = 0;
  gmm_data->kern.attach_cap.rau_initiated = GMM_NO_RAU;
  gmm_data->kern.attach_cap.enter_attempting_to_update_after_lau=FALSE;

  gmm_data->kern.sig_cell_info.env.rai.rac = GMMREG_RA_INVALID;
  gmm_data->kern.sig_cell_info.env.rai.lac = GMMRR_LA_INVALID;
  gmm_data->kern.sig_cell_info.gmm_status = GMMRR_SERVICE_UNKNOWN ; /* Initalize gmm status value */
  
#ifdef _SIMULATION_
  gmm_data->kern.mm_cell_env.rai.rac = GMMREG_RA_INVALID;
  gmm_data->kern.mm_cell_env.rai.lac = GMMRR_LA_INVALID;
  gmm_data->kern.detach_cap.detach_type = VAL_DETACH_TYPE___DEF;
#endif

  /*
   * Cel ID
   */
  gmm_data->kern.cell_id.lac  = (USHORT)GMMREG_LA_INVALID;
  gmm_data->kern.cell_id.rac  = (UBYTE)GMMREG_RA_INVALID;


  INIT_STATE (CU,CU_CELL_RES_SENT);
  gmm_data->kern.attach_cap.llc_state=GMM_LLC_STATE_UNASSIGNED;
  gmm_data->kern.gmmrr_resume_sent = FALSE;
  gmm_data->kern.attach_cap.grr_via_llc_suspended=FALSE;
  gmm_data->kern.attach_cap.mm_lau_attempted=FALSE;
  
  for(i=0; i<MAX_LIST_OF_FORBIDDEN_PLMNS_FOR_GPRS_SERVICE; i++)
  {
    memset(gmm_data->kern.attach_cap.list_of_forbidden_plmns_for_gprs_service[i].mcc, 0xFF, SIZE_MCC);
    memset(gmm_data->kern.attach_cap.list_of_forbidden_plmns_for_gprs_service[i].mnc, 0xFF, SIZE_MNC);
  }
  kern_read_pcm();

#ifdef REL99
  /* Used to indicate to LLC if it should use NULL frame for
   * performing cell updates. Cell notification is disabled by
   * default. This field is changed only if the SGSN indicates
   * that it supports cell notification
   */
   gmm_data->cell_notification = NO_CELL_NOTIFY; /* TCS 4.0 */
  /* Initialise all the pdp context status to invalid untill GMM gets a valid status from SM */
  gmm_data->kern.attach_cap.pdp_context_status = INVALID_PDP_CONTEXT_STATUS; /* TCS 4.0 */
#endif

  return;
} /* kern_init() */


/*
+------------------------------------------------------------------------------
| Function    : kern_attach_reset
+------------------------------------------------------------------------------
| Description : This procedure is called in cell updatingn procedure, if RA 
|               was changed an the ATTACH procedurehas to re-initialized. 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_attach_reset ( void )
{ 
  GMM_TRACE_FUNCTION( "kern_attach_reset" );
  
  /*
   * resets the counter for TIMER t3310
   */
  gmm_data->kern.ct3310 = 0;

  /* 
   * I reuse timer T3310 also for T3330
   */
  vsi_t_stop  ( GMM_handle, kern_T3310);
  vsi_t_stop  ( GMM_handle, kern_T3311);


  gmm_data->kern.aac = 0;
  gmm_data->kern.detach_cap.network = FALSE;
  GMM_RETURN; 
} /* kern_attach_reset() */

/*
+------------------------------------------------------------------------------
| Function    : kern_mm_detach_started
+------------------------------------------------------------------------------
| Description : The function kern_mm_detach_started indicates MM that the 
|                GSM detach procedure is started by GMM. MM has to enter state
|               MM-IMSI-DETACH-PENDING.
|       
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_mm_detach_started ( void )
{ 
  GMM_TRACE_FUNCTION( "kern_mm_detach_started" );

  switch( gmm_data->kern.detach_cap.detach_type)
  {
    case GMMREG_DT_SIM_REMOVED :
      if (GMM_MM_DEREG == GET_STATE(MM))
      {
        break;
      }
      /* NO break;*/
    case GMMREG_DT_IMSI:
    case GMMREG_DT_COMB:
      if (GMMRR_NET_MODE_I==gmm_data->kern.sig_cell_info.net_mode)
      {
        PALLOC ( mmgmm_detach_started_req, MMGMM_DETACH_STARTED_REQ);
        PSEND ( hCommMM, mmgmm_detach_started_req ); 
      }
      break;
    default:
      break;
  }
  GMM_RETURN;
} /* kern_detach_started() */

/*
+------------------------------------------------------------------------------
| Function    : kern_mm_imsi_detach_ind
+------------------------------------------------------------------------------
| Description : The function kern_mm_imsi_detach()handles the IMSI detach. 
|      
| Parameters  : detach_done -  indicates whether GMM has already done the IMSI
|                detach or not.
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_mm_imsi_detach_ind ( USHORT error_cause, BOOL detach_done, UBYTE detach_type )
{ 
  GMM_TRACE_FUNCTION( "kern_mm_imsi_detach_ind" );
 
  {
    PALLOC ( mmgmm_nreg_req, MMGMM_NREG_REQ); /* T_MMGMM_NREG_REQ */
      mmgmm_nreg_req->cause = error_cause; /* TCS 2.1 */
      mmgmm_nreg_req->detach_done = detach_done;
      SET_STATE ( MM, GMM_MM_DEREG );

      TRACE_1_OUT_PARA("detach_done %d",detach_done);
      
      switch ( detach_type )
      {
        case GMMREG_DT_SIM_REMOVED:
          mmgmm_nreg_req->detach_cause = CS_SIM_REM; /* TCS 2.1 */
          break;
        case GMMREG_DT_POWER_OFF:
          mmgmm_nreg_req->detach_cause = CS_POW_OFF; /* TCS 2.1 */
          break;
        case GMMREG_DT_SOFT_OFF:
          mmgmm_nreg_req->detach_cause = CS_SOFT_OFF; /* TCS 2.1 */
          break;
        default:
          mmgmm_nreg_req->detach_cause = CS_DISABLE; /* TCS 2.1 */
          break;
      }
      gmm_data->kern.suspension_type &= ~GMM_SUSP_IMSI_DETACH;
    PSEND ( hCommMM, mmgmm_nreg_req );   
  }
  GMM_RETURN;                  
} /* kern_imsi_detach_ind () */
/*
+------------------------------------------------------------------------------
| Function    : kern_mm_imsi_detach
+------------------------------------------------------------------------------
| Description : The function kern_mm_imsi_detach()handles the IMSI detach. 
|      
| Parameters  : detach_done -  indicates whether GMM has already done the IMSI
|                detach or not.
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_mm_imsi_detach ( void )
{ 
  GMM_TRACE_FUNCTION( "kern_mm_imsi_detach" );
  kern_mm_imsi_detach_ind (GMMCS_INT_NOT_PRESENT, MMGMM_PERFORM_DETACH,
                           gmm_data->kern.detach_cap.detach_type); /* TCS 2.1 */
  GMM_RETURN;
} /* kern_imsi_detach() */


/*
+------------------------------------------------------------------------------
| Function    : kern_mm_lau
+------------------------------------------------------------------------------
| Description : This procedure start the LAU procedure in entity MM
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_mm_lau ( void )
{ 
  GMM_TRACE_FUNCTION( "kern_mm_lau" );

  gmm_data->kern.attach_cap.mm_lau_attempted=TRUE;
  gmm_data->kern.suspension_type &= ~GMM_SUSP_LAU;

  if (gmm_data->kern.attach_cap.network_selection_mode EQ MODE_AUTO)
  {
    kern_mm_reg_req (REG_REMOTE_CONTROLLED, NORMAL_REG);
  }
  else
  {
    PALLOC ( mmgmm_plmn_res, MMGMM_PLMN_RES ); /* T_MMGMM_PLMN_RES */
      mmgmm_plmn_res->plmn         = gmm_data->kern.attach_cap.plmn_requested;
      mmgmm_plmn_res->reg_type = REG_REMOTE_CONTROLLED;
      mmgmm_plmn_res->mobile_class = gmm_data->kern.attach_cap.mobile_class;
    PSEND ( hCommMM, mmgmm_plmn_res );
  }

  GMM_RETURN;
} /* kern_mm_lau() */

/*
+------------------------------------------------------------------------------
| Function    : kern_gmmrr_assign
+------------------------------------------------------------------------------
| Description : The function kern_gmmrr_assign() 
|               
|               This procedure assigned the current TLLI parameter to GRR
|               entities 
|
| Parameters  : void
|              
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_gmmrr_assign ( void )
{ 
  GMM_TRACE_FUNCTION( "kern_gmmrr_assign" );
  
  {
    PALLOC ( gmmrr_assign_req, GMMRR_ASSIGN_REQ );
      gmmrr_assign_req->new_tlli  = gmm_get_tlli ( CURRENT_TLLI );
      gmmrr_assign_req->old_tlli  = gmm_get_tlli ( OLD_TLLI );      
      
      if (GMMRR_TLLI_INVALID==gmmrr_assign_req->new_tlli)
      {
        gmmrr_assign_req->old_ptmsi = GMMRR_TMSI_INVALID;
        gmmrr_assign_req->new_ptmsi = GMMRR_TMSI_INVALID;
      }
      else
      {
        gmmrr_assign_req->old_ptmsi = gmm_data->ptmsi.old;
        gmmrr_assign_req->new_ptmsi = gmm_data->ptmsi.new_grr;

      }
    gmmrr_assign_req->imsi  = gmm_data->imsi;
    gmmrr_assign_req->rai.plmn.v_plmn=TRUE;
    memcpy (gmmrr_assign_req->rai.plmn.mcc, gmm_data->kern.attach_cap.rai_accepted.mcc, SIZE_MCC);
    memcpy (gmmrr_assign_req->rai.plmn.mnc, gmm_data->kern.attach_cap.rai_accepted.mnc, SIZE_MNC);
    gmmrr_assign_req->rai.lac = gmm_data->kern.attach_cap.rai_accepted.lac;
    gmmrr_assign_req->rai.rac = gmm_data->kern.attach_cap.rai_accepted.rac;

    TRACE_3_OUT_PARA("PTMSI old: 0x%X new: 0x%X,curent:0x%X ",gmmrr_assign_req->old_ptmsi, 
    gmmrr_assign_req->new_ptmsi, gmm_data->ptmsi.current);
    PSEND ( hCommGRR, gmmrr_assign_req );
  }
  GMM_RETURN;
} /* kern_gmmrr_assign() */

/*
+------------------------------------------------------------------------------
| Function    : kern_local_atach
+------------------------------------------------------------------------------
| Description : The function kern_local_attach() 
|               
|               This procedure assigned the given TLLI parameter to the other
|               entities 
|
| Parameters  : new_tlli_type - type of new TLLI (random, local ,..)
|               old_tlli_type - type of old TLLI
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_local_attach ( T_TLLI_TYPE new_tlli_type, T_TLLI_TYPE old_tlli_type )
{ 
  GMM_TRACE_FUNCTION( "kern_local_attach" );
  
  kern_llgmm_assign_tlli ( new_tlli_type, old_tlli_type );
  kern_gmmrr_assign();
  GMM_RETURN;
} /* kern_local_attach() */

/*
+------------------------------------------------------------------------------
| Function    : kern_ptmsi_negotiated
+------------------------------------------------------------------------------
| Description : This procedure is called if an AIR message is received which 
|               contains an optional PTMSI IE. i.e. in the ATTACH_ACCEPT or 
|               RAU_ACCEPT message. It stores the P-TMSI and the
|               P-TMSI Signature, if available into the SIM and changes the
|               update state to GU1 UPDATED and triggers the cell update
|               procedure by calling LLGMM_TRIGGER_REQ, if needed. Otherwise,
|               if P-TMSI was negotiated CU is triggered outside of this
|               procedure by transmitting CONFIRM message. At the end of this
|               procedure the new genereted TLLI is passed to the other layers
|
| Parameters  : v_mobile_identity  - flag from AIR  if ptmsi is available
|               mobile_identity    - ptmsi from AIR
|               v_ptmsi_signature  - flag from AIR if psignature is available
|               ptmsi_signature   - ptmsi signature from AIR
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_tmsi_negotiated ( BOOL v_tmsi, 
                                   T_mobile_identity *tmsi,
                                   BOOL v_ptmsi, 
                                   T_gmobile_identity *ptmsi,
                                   BOOL v_ptmsi_signature,
                                   T_p_tmsi_signature *p_tmsi_signature
                                 ) 
{
  GMM_TRACE_FUNCTION ("kern_tmsi_negotiated()");

  /*
   * save the available flags
   */
  gmm_data->ptmsi_signature.available = v_ptmsi_signature;
  if ( v_ptmsi )
  {
    if ( !ptmsi->v_tmsi )
    {
      TRACE_ERROR ( "PTMSI IE received from air without ptmsi value, perhaps IMSI reseived" );
      sig_kern_rx_gmm_status(ERRCS_IE_INVALID);
    }
    else
    {
      /*
       * store the given PTMSI
       */
      gmm_data->ptmsi.old     = gmm_data->ptmsi.current,
  
      gmm_data->ptmsi.new_grr 
       = gmm_data->ptmsi.current = kern_get_tmsi ((T_mobile_identity *)ptmsi );

    }
  }
  if ( v_tmsi )
  {
    if ( !tmsi->v_tmsi )
    {
      TRACE_EVENT ( "TMSI IE received from air without ptmsi value, perhaps IMSI reseived" );
      /* 
       * <R.GMM.ACSUBOTH.M.011>
       * If the message contains an IMSI (i.e. the MS is not allocated any TMSI)
       * than the MS shall delete any TMSI.
       */
      gmm_data->tmsi =MMGMM_TMSI_INVALID;

    }
    else
    {
    /*
     * store the given TMSI
     */
      gmm_data->tmsi = kern_get_tmsi ((T_mobile_identity *)tmsi );
    }
  }
  /*
   * After the rocedure the local TLLI have to be used
   *
   * set all TLLIs and saves the current TLLI as old TLLI
   */
  gmm_set_current_tlli ( LOCAL_TLLI ) ;


  if ( v_ptmsi_signature ) 
  {
    /*
     * <R.GMM.AGACCEPT.M.020>, <R.GMM.AGACCEPT.M.021>,
     * <R.GMM.AGACCEPT.M.022>, <R.GMM.RNACCEPT.M.014>
     */
    gmm_data->ptmsi_signature.value 
                           = p_tmsi_signature->p_tmsi_signature_value;
  }
  /*
   * <R.GMM.AGACCEPT.M017>, <R.GMM.AGACCEPT.M019>, 
   * <R.GMM.AGACCEPT.M022>,<R.GMM.AGACCEPT.M023>
   */
  gmm_data->gu                = GU1_UPDATED;

  kern_sim_gmm_update();
  vsi_o_ttrace(VSI_CALLER TC_EVENT,"Info: GU: %i", gmm_data->gu+1);

  
  if ( 
     GMM_PERIODIC_RAU    != gmm_data->kern.attach_cap.rau_initiated
   || ptmsi->v_tmsi
   || tmsi->v_tmsi
  )
  {
    /*
     * in attach and RAU the TLLI as to change from foreign to local tlli
     */
    kern_local_attach ( CURRENT_TLLI, OLD_TLLI );
  }
  else
  {
    /* 
     * AniteB2 44.2.3.1.5
     * RR has to transmit the last assigned RAI with GPRS resumption
     */
    kern_gmmrr_assign();
  }
  GMM_RETURN;

} /* kern_ptmsi_negotiated */

/*
+------------------------------------------------------------------------------
| Function    : kern_sim_del_locigprs
+------------------------------------------------------------------------------
| Description : This procedure resets all location infos on sim
|               
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_sim_del_locigprs ( void ) 
{
  GMM_TRACE_FUNCTION ("kern_sim_del_locigprs()");

  gmm_data->ptmsi.current           = GMMRR_TMSI_INVALID;
  gmm_data->ptmsi.new_grr           = GMMRR_TMSI_INVALID;
  gmm_data->ptmsi.old               = GMMRR_TMSI_INVALID;
  gmm_data->ptmsi_signature.available = FALSE;
  gmm_data->ptmsi_signature.value      = INVALID_PTMSI_SIGNATURE; 

  gmm_data->kern.attach_cap.rai_accepted.lac = 0xfffe;           /* INVALID */
  gmm_data->kern.attach_cap.rai_accepted.rac = 0xff;           /* INVALID */
  memset (gmm_data->kern.auth_cap.kc, 0xFF, MAX_KC);
  gmm_data->kern.auth_cap.cksn       = NO_KEY;
  GMM_RETURN;
} /* kern_sim_del_locigprs */

/*
+------------------------------------------------------------------------------
| Function    : kern_sim_gmm_update
+------------------------------------------------------------------------------
| Description : This procedure sends the primitive SIM_GU_UPDATE_REQ to SIM.
|               The primitive is allocated within this function and the
|               parameters of the primitive are filled in with the actually
|               set global variables of GMM.
|               
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_sim_gmm_update () 
{
  GMM_TRACE_FUNCTION ("kern_sim_gmm_update()");
  {
    PALLOC ( sim_gmm_update_req, SIM_GMM_UPDATE_REQ );
      kern_set_loc_info   (sim_gmm_update_req);
      kern_set_kc_cksn    (sim_gmm_update_req);
      sim_gmm_update_req->att_status 
        = gmm_data->kern.attach_cap.attach_acc_after_po;    
    PSEND ( hCommSIM, sim_gmm_update_req );
  }
  GMM_RETURN;
} /* kern_sim_gmm_update */

/*
+------------------------------------------------------------------------------
| Function    : kern_read_imsi
+------------------------------------------------------------------------------
| Description : This procedure reads the imsi from primitive 
|               sim_gmm_insert_ind
|               
|               copied from mm_regF.c in MM
|               
| Parameters  : sim_gmm_insert_ind - primitiv pointer
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_read_imsi (T_SIM_GMM_INSERT_IND *sim_gmm_insert_ind)
{
  USHORT i;
  UBYTE  digit;
  UBYTE  length;
  ULONG  init_value=25;
#ifdef NEW_FRAME
  T_TIME  time_val;
#else /* NEW_FRAME */
  T_VSI_TVALUE  time_val;
#endif /* NEW_FRAME */

  GMM_TRACE_FUNCTION ("kern_read_imsi ()");

  
  gmm_data->imsi.v_mid    = TRUE;
  gmm_data->imsi.id_type  = TYPE_IMSI;
  gmm_data->imsi.tmsi_dig = 0;


  length = (sim_gmm_insert_ind->imsi_field.c_field-1)*2;
  if (sim_gmm_insert_ind->imsi_field.field[0] & 0x08)
  {
    length++;
  }

  for (i = 0; i < length; i++)
  {
    digit = (i & 1) ?
        sim_gmm_insert_ind->imsi_field.field[(i + 1) / 2] & 0x0f :
       (sim_gmm_insert_ind->imsi_field.field[(i + 1) / 2] & 0xf0) >> 4;
    gmm_data->imsi.id[i] = digit;
    init_value += digit*digit; 
  }
  gmm_data->imsi.id[i] = 0xff;

  vsi_t_time (VSI_CALLER &time_val); 
  srand ((USHORT)(init_value*time_val));
  GMM_RETURN;
} /*kern_read_imsi*/ 
/*
+------------------------------------------------------------------------------
| Function    : kern_get_tmsi
+------------------------------------------------------------------------------
| Description : This procedure translate the timer value given by ACI to
|               an air message struct  
|               
|               
| Parameters  : mobile_identity - PTMSI received from AIR
|
+------------------------------------------------------------------------------
*/

GLOBAL ULONG kern_get_tmsi (T_mobile_identity * mobile_identity )
{ 
  ULONG ptmsi;
  GMM_TRACE_FUNCTION( "kern_get_tmsi" );
  
  ptmsi = (mobile_identity->tmsi.b_tmsi[0] << 24)+
                    (mobile_identity->tmsi.b_tmsi[1] << 16)+
                    (mobile_identity->tmsi.b_tmsi[2] << 8)+
                     mobile_identity->tmsi.b_tmsi[3];
  GMM_RETURN_ (ptmsi);

} /* kern_get_tmsi */

/*
+------------------------------------------------------------------------------
| Function    : kern_get_mobile_identity
+------------------------------------------------------------------------------
| Description : Derives the mobile identiti for the AIR
|
| COPIED FROM : MM    mm_mmf.c : mm_fill_identity
|               
| Parameters  : type_of_identity  - tpe of identity
|                gmobile_identity   - used in the AIR message
|
+------------------------------------------------------------------------------
*/

GLOBAL void kern_get_mobile_identity ( 
  UBYTE type_of_identity,
  T_gmobile_identity* gmobile_identity )
{
  GMM_TRACE_FUNCTION ("kern_get_mobile_identity()");
 
  memset (gmobile_identity, 0, sizeof (T_mobile_identity));

  switch (type_of_identity)
  {
    case ID_TYPE_IMEISV:
      kern_get_imeisv ( gmobile_identity);
      break;
    case ID_TYPE_IMEI:
      kern_get_imei ( gmobile_identity);
      break;
    case ID_TYPE_TMSI:
      kern_ulong2mobile_identity (gmm_data->ptmsi.current, gmobile_identity);
      break;
    default:
      TRACE_ERROR ("undefined type of Identity");
      /*
       * NO break;
       */
    case ID_TYPE_IMSI:
      gmobile_identity->v_tmsi = FALSE;
      gmobile_identity->type_of_identity  = ID_TYPE_IMSI;
      gmobile_identity->v_identity_digit = TRUE;
      gmobile_identity->c_identity_digit 
        = kern_calculate_digits (gmm_data->imsi.id);
      gmobile_identity->odd_even    = gmobile_identity->c_identity_digit & 1;
      memcpy (gmobile_identity->identity_digit, gmm_data->imsi.id, 16);
      break;
  }
  GMM_RETURN;
} /* kern_get_mobile_identity() */
/*
+------------------------------------------------------------------------------
| Function    : kern_read_kc_cksn
+------------------------------------------------------------------------------
| Description : Reads the location information delivered by the SIM card
|               into the registration memory structures.
|
| COPIED FROM : MM    mm_regf.c : reg_read_kc_cksn
|               
| Parameters  : T_loc_info - the the loc_inf field from sim_gmm_inserted_ind
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_read_kc_cksn (T_SIM_GMM_INSERT_IND *sim_gmm_insert_ind)
{
  GMM_TRACE_FUNCTION ("kern_read_kc_cksn ()");
  
  switch (gmm_data->gu)
  {
    case GU2_NOT_UPDATED:
      /*
       * GSM: 04.08, Ch.4.1.3.2 GPRS update status
       * 
       *   GU2: NOT UPDATED
       *     The last GPRS attach or routing area updating attempt failed procedurally,
       *     i.e. no response was received from the network. This includes the cases of
       *     failures or congestion inside the network.
       *     In this case, the SIM may contain the RAI of the routing area (RA) to which
       *     the subscriber was attached, and possibly also a valid P-TMSI,
       *     GPRS GSM ciphering key, GPRS UMTS ciphering key, GPRS UMTS integrity key
       *     and GPRS ciphering key sequence number. For compatibility reasons, all these
       *     fields shall be set to the "deleted" value if the RAI is deleted. However, the
       *     presence of other values shall not be considered an error by the MS. 
       */
      if ( GMMRR_LA_INVALID != gmm_data->kern.attach_cap.rai_accepted.lac) 
      {
        break;
      }
      /* NO break */
    case GU3_ROAMING_NOT_ALLOWED:
    case GU3_PLMN_NOT_ALLOWED:
      /*
       * GSM: 04.08, Ch.4.3.2.4:
       * "When the deletion of the sequence number is described this also means
       * that the associated key shall be considered as invalid."
       * GSM: 04.08, Ch.4.7.7.4:
       * "If the sequence number is deleted,
       *  the associated key shall be considered as invalid."
       */

      gmm_data->kern.auth_cap.cksn = NO_KEY;
      memset (gmm_data->kern.auth_cap.kc, 0xFF, MAX_KC);
      break;
    default:
       gmm_data->kern.auth_cap.cksn = sim_gmm_insert_ind->kc_n.kc[MAX_KC];
      if (NO_KEY == gmm_data->kern.auth_cap.cksn)
      {
        memset (gmm_data->kern.auth_cap.kc, 0xFF, MAX_KC);
      }
      else
      {  
        /*
         * copy parameter from SIM card
         */
        memcpy (gmm_data->kern.auth_cap.kc, sim_gmm_insert_ind->kc_n.kc, MAX_KC);      
      }
      break;
  }
  GMM_RETURN;
}
/*
+------------------------------------------------------------------------------
| Function    : kern_set_kc_cksn
+------------------------------------------------------------------------------
| Description : sets the kc and cksn delivered by the SIM card
|               into the registration memory structures.
|
| COPIED FROM : MM    mm_regf.c : reg_read_kc_cksn
|               
| Parameters  : T_loc_info - the the loc_inf field from sim_gmm_inserted_ind
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_set_kc_cksn (T_SIM_GMM_UPDATE_REQ *sim_gmm_update_req)
{
  GMM_TRACE_FUNCTION ("kern_set_kc_cksn ()");

  sim_gmm_update_req->cksn = gmm_data->kern.auth_cap.cksn;
  memcpy (sim_gmm_update_req->kc, gmm_data->kern.auth_cap.kc, MAX_KC);
  GMM_RETURN;
}

/*
+------------------------------------------------------------------------------
| Function    : kern_read_log_info
+------------------------------------------------------------------------------
| Description : Reads the location information delivered by the SIM card
|               into the registration memory structures.
|
| COPIED FROM : MM    mm_regf.c : reg_read_log_info
|               
| Parameters  : T_loc_info - the the loc_inf field from sim_gmm_inserted_ind
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_read_loc_info (T_SIM_GMM_INSERT_IND *sim_gmm_insert_ind)
{
  GMM_TRACE_FUNCTION ("kern_read_loc_info ()");
  
  gmm_data->tmsi =  (sim_gmm_insert_ind->loc_info.loc[0] << 24) +
                    (sim_gmm_insert_ind->loc_info.loc[1] << 16) +
                    (sim_gmm_insert_ind->loc_info.loc[2] <<  8) +
                      sim_gmm_insert_ind->loc_info.loc[3];

  gmm_data->ptmsi.new_grr =
  gmm_data->ptmsi.current =     (sim_gmm_insert_ind->gprs_loc_info.loc[0] << 24) +
                        (sim_gmm_insert_ind->gprs_loc_info.loc[1] << 16) +
                        (sim_gmm_insert_ind->gprs_loc_info.loc[2] <<  8) +
                         sim_gmm_insert_ind->gprs_loc_info.loc[3];


  gmm_data->ptmsi_signature.value =  
                        (sim_gmm_insert_ind->gprs_loc_info.loc[4] << 16) +
                        (sim_gmm_insert_ind->gprs_loc_info.loc[5] <<  8) +
                         sim_gmm_insert_ind->gprs_loc_info.loc[6];  
  gmm_data->ptmsi_signature.available = TRUE;
   
  gmm_data->kern.attach_cap.rai_accepted.mcc[0] = sim_gmm_insert_ind->gprs_loc_info.loc[7] & 0x0f;
  gmm_data->kern.attach_cap.rai_accepted.mcc[1] = sim_gmm_insert_ind->gprs_loc_info.loc[7] >> 4;
  gmm_data->kern.attach_cap.rai_accepted.mcc[2] = sim_gmm_insert_ind->gprs_loc_info.loc[8] & 0x0f;
  gmm_data->kern.attach_cap.rai_accepted.mnc[2] = sim_gmm_insert_ind->gprs_loc_info.loc[8] >> 4;
  gmm_data->kern.attach_cap.rai_accepted.mnc[0] = sim_gmm_insert_ind->gprs_loc_info.loc[9] & 0x0f;
  gmm_data->kern.attach_cap.rai_accepted.mnc[1] = sim_gmm_insert_ind->gprs_loc_info.loc[9] >> 4;
  gmm_data->kern.attach_cap.rai_accepted.lac    = sim_gmm_insert_ind->gprs_loc_info.loc[10] * 256 +
                         sim_gmm_insert_ind->gprs_loc_info.loc[11];
  gmm_data->kern.attach_cap.rai_accepted.rac    = sim_gmm_insert_ind->gprs_loc_info.loc[12];

  gmm_data->gu        = sim_gmm_insert_ind->gprs_loc_info.loc[13]; 

  if ( GMMRR_LA_INVALID == gmm_data->kern.attach_cap.rai_accepted.lac)
  {
     kern_sim_del_locigprs ();
  } 
  TRACE_9_PARA("TMSI %x, PTMSI %x, lac %x, rac %x, MCC: %x%x%x, MNC: %x%x", 
    gmm_data->tmsi,
    gmm_data->ptmsi.current,          
    gmm_data->kern.attach_cap.rai_accepted.lac,
    gmm_data->kern.attach_cap.rai_accepted.rac,
    gmm_data->kern.attach_cap.rai_accepted.mcc[0],
    gmm_data->kern.attach_cap.rai_accepted.mcc[1],
    gmm_data->kern.attach_cap.rai_accepted.mcc[2],
    gmm_data->kern.attach_cap.rai_accepted.mnc[0],
    gmm_data->kern.attach_cap.rai_accepted.mnc[1]
  );
 
  if (gmm_data->gu > GU3_ROAMING_NOT_ALLOWED)
  {
    gmm_data->gu = GU2_NOT_UPDATED;
  }
  GMM_RETURN;
}

/*
+------------------------------------------------------------------------------
| Function    : kern_set_loc_info
+------------------------------------------------------------------------------
| Description : derives the log_info field for for sim inserted
|
| COPIED FROM : MM    mm_regf.c : reg_set_log_info
|               
| Parameters  : sim_gmm_update_req - the primitive
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_set_loc_info (T_SIM_GMM_UPDATE_REQ *sim_gmm_update_req)
{

  GMM_TRACE_FUNCTION ("kern_set_loc_info ()");
  sim_gmm_update_req->gprs_loc_info.c_loc = MAX_LOCIGPRS;

  sim_gmm_update_req->gprs_loc_info.loc[0]  = (UBYTE)(gmm_data->ptmsi.current >> 24);
  sim_gmm_update_req->gprs_loc_info.loc[1]  = (UBYTE)(gmm_data->ptmsi.current >> 16);
  sim_gmm_update_req->gprs_loc_info.loc[2]  = (UBYTE)(gmm_data->ptmsi.current >> 8);
  sim_gmm_update_req->gprs_loc_info.loc[3]  = (UBYTE)gmm_data->ptmsi.current;
  
  sim_gmm_update_req->gprs_loc_info.loc[4]  = (UBYTE)(gmm_data->ptmsi_signature.value >> 16);
  sim_gmm_update_req->gprs_loc_info.loc[5]  = (UBYTE)(gmm_data->ptmsi_signature.value >> 8);
  sim_gmm_update_req->gprs_loc_info.loc[6]  = (UBYTE)gmm_data->ptmsi_signature.value;
  
  sim_gmm_update_req->gprs_loc_info.loc[7]  = gmm_data->kern.attach_cap.rai_accepted.mcc[1] << 4;
  sim_gmm_update_req->gprs_loc_info.loc[7] += gmm_data->kern.attach_cap.rai_accepted.mcc[0];

  sim_gmm_update_req->gprs_loc_info.loc[8]  = gmm_data->kern.attach_cap.rai_accepted.mnc[2] << 4;
  sim_gmm_update_req->gprs_loc_info.loc[8] += gmm_data->kern.attach_cap.rai_accepted.mcc[2];
  
  sim_gmm_update_req->gprs_loc_info.loc[9]  = gmm_data->kern.attach_cap.rai_accepted.mnc[1] << 4;
  sim_gmm_update_req->gprs_loc_info.loc[9] += gmm_data->kern.attach_cap.rai_accepted.mnc[0];
  sim_gmm_update_req->gprs_loc_info.loc[10]  = gmm_data->kern.attach_cap.rai_accepted.lac >> 8;
  sim_gmm_update_req->gprs_loc_info.loc[11]  = gmm_data->kern.attach_cap.rai_accepted.lac & 0xff;
  sim_gmm_update_req->gprs_loc_info.loc[12]  = gmm_data->kern.attach_cap.rai_accepted.rac;
  
  sim_gmm_update_req->gprs_loc_info.loc[13] = gmm_data->gu;
  GMM_RETURN;
}
/*
+------------------------------------------------------------------------------
| Function    : kern_ulong2mobile_identity
+------------------------------------------------------------------------------
| Description : Derives tm mobile identiti for the AIR
|
| COPIED FROM : MM    mm_mmf.c : mm_fill_identity
|               
| Parameters  : mobile_identity   - used in the AIR message
|
+------------------------------------------------------------------------------
*/

GLOBAL void kern_ulong2mobile_identity ( ULONG ptmsi, T_gmobile_identity* gmobile_identity )
{
  GMM_TRACE_FUNCTION ("kern_ulong2mobile_identity()");
 
  memset (gmobile_identity, 0, sizeof (T_mobile_identity));

  
  
  if ( GMMRR_TMSI_INVALID == gmm_data->ptmsi.current )
  {
    /*
     * IMSI
     * <R.GMM.AGINIT.M.005>
     */
    gmobile_identity->v_tmsi      = FALSE;

    kern_get_mobile_identity ( ID_TYPE_IMSI, gmobile_identity );
  }  
  else /* !ptmsi.available */
  { 
    /* 
     * PTMSI
     * <R.GMM.AGINIT.M.002>
     */
    gmobile_identity->odd_even    = EVEN;
    gmobile_identity->v_tmsi      = TRUE;
    gmobile_identity->type_of_identity = ID_TYPE_TMSI;
    gmobile_identity->v_identity_digit = FALSE;
    gmobile_identity->tmsi.l_tmsi = 32;
  
  
    ccd_codeByte (gmobile_identity->tmsi.b_tmsi,  0, 8, (UBYTE)(ptmsi >> 24));
    ccd_codeByte (gmobile_identity->tmsi.b_tmsi,  8, 8, (UBYTE)(ptmsi >> 16));
    ccd_codeByte (gmobile_identity->tmsi.b_tmsi, 16, 8, (UBYTE)(ptmsi >> 8));
    ccd_codeByte (gmobile_identity->tmsi.b_tmsi, 24, 8, (UBYTE) ptmsi);
  }
  GMM_RETURN; 
  
} /* kern_ulong2mobile_identity() */
/*
+------------------------------------------------------------------------------
| Function    : kern_kern_cell_changed
+------------------------------------------------------------------------------
| Description : Returns TRUE if cell was changed.
|               
| Parameters  : void
|
+------------------------------------------------------------------------------
*/

GLOBAL BOOL kern_cell_changed ( void )
{
  GMM_TRACE_FUNCTION ("kern_cell_changed()");

  if ( gmm_data->kern.sig_cell_info.env.cid != gmm_data->kern.old_sig_cell_info.env.cid)
  {
    TRACE_EVENT("Info: cid changed");
    GMM_RETURN_ (TRUE);
  }
  else 
  {
    GMM_RETURN_ (kern_ra_crossed());
  }
} /* kern_lai_changed() */

/*
+------------------------------------------------------------------------------
| Function    : kern_kern_lai_changed
+------------------------------------------------------------------------------
| Description : Returns TRUE if LA was changed.
|               
| Parameters  : void
|
+------------------------------------------------------------------------------
*/

GLOBAL BOOL kern_lai_changed ( void )
{
  GMM_TRACE_FUNCTION ("kern_lai_changed()");

  if ( gmm_data->kern.attach_cap.rai_accepted.lac != gmm_data->kern.sig_cell_info.env.rai.lac)
  {
    TRACE_3_INFO ("Info: LAI changed: changed lac: %x (%x) -> %x",            
    gmm_data->kern.attach_cap.rai_accepted.lac,
    gmm_data->kern.old_sig_cell_info.env.rai.lac,
    gmm_data->kern.sig_cell_info.env.rai.lac);
    GMM_RETURN_ (TRUE);
  }
  else 
  {
    if (kern_plmn_changed()) 
    {
      GMM_RETURN_ (TRUE);
    }
  }

  GMM_RETURN_ (FALSE);
} /* kern_lai_changed() */

/*
+------------------------------------------------------------------------------
| Function    : kern_kern_lau_needed
+------------------------------------------------------------------------------
| Description : Returns TRUE if LAU is needed
|               
| Parameters  : void
|
+------------------------------------------------------------------------------
*/

GLOBAL BOOL kern_lau_needed ( void )
{
  UBYTE mm_state;

  GMM_TRACE_FUNCTION ("kern_lau_needed()");

  mm_state = GET_STATE(MM);

  TRACE_1_PARA ("sig_cell_info.mm_status %d", gmm_data->kern.sig_cell_info.mm_status);

  switch (gmm_data->kern.attach_cap.attach_type)
  {
    case GMMREG_AT_IMSI:
    case GMMREG_AT_COMB:                    
      GMM_RETURN_ (
        (   GMM_MM_DEREG              == mm_state
          || GMM_MM_REG_UPDATE_NEEDED == mm_state
          || MMGMM_WAIT_FOR_UPDATE    == gmm_data->kern.sig_cell_info.mm_status
             
          ||  gmm_data->kern.sig_cell_info.env.rai.lac != gmm_data->kern.mm_cell_env.rai.lac
          || (memcmp (gmm_data->kern.sig_cell_info.env.rai.plmn.mnc, 
                      gmm_data->kern.mm_cell_env.rai.plmn.mnc, SIZE_MNC) != 0)
          || (memcmp (gmm_data->kern.sig_cell_info.env.rai.plmn.mcc, 
                      gmm_data->kern.mm_cell_env.rai.plmn.mcc, SIZE_MCC) != 0)
        )
        
        &&  ( MMGMM_LIMITED_SERVICE != gmm_data->kern.sig_cell_info.mm_status)
      );
      /* break; */
    default:
      GMM_RETURN_ (FALSE);
      /* break; */
  }
} /* kern_lau_needed() */


/*
+------------------------------------------------------------------------------
| Function    : kern_ra_crossed
+------------------------------------------------------------------------------
| Description : Returns TRUE if RA was crossed.
|               
| Parameters  : void
|
+------------------------------------------------------------------------------
*/

GLOBAL BOOL kern_ra_crossed ( void )
{
  GMM_TRACE_FUNCTION ("kern_ra_crossed?");
  
  if (gmm_data->kern.sig_cell_info.env.rai.rac != gmm_data->kern.old_sig_cell_info.env.rai.rac
    || gmm_data->kern.sig_cell_info.env.rai.lac != gmm_data->kern.old_sig_cell_info.env.rai.lac )
  {
    TRACE_EVENT("Info: RA crossed");
    GMM_RETURN_ (TRUE);
  }
  GMM_RETURN_(FALSE);
} /* kern_rai_changed() */

/*
+------------------------------------------------------------------------------
| Function    : kern_rai_changed
+------------------------------------------------------------------------------
| Description : Returns TRUE if RA was changed.
|               
| Parameters  : void
|
+------------------------------------------------------------------------------
*/

GLOBAL BOOL kern_rai_changed ( void )
{
  GMM_TRACE_FUNCTION ("kern_rai_changed()");
  
  if ( gmm_data->kern.sig_cell_info.env.rai.rac != gmm_data->kern.attach_cap.rai_accepted.rac )
  {
    TRACE_0_INFO("RAI changed");
    GMM_RETURN_ (TRUE);
  }
  GMM_RETURN_ (kern_lai_changed());
} /* kern_rai_changed() */

/*
+------------------------------------------------------------------------------
| Function    : kern_plmn_changed
+------------------------------------------------------------------------------
| Description : Returns TRUE if PLMN was changed.
|               
| Parameters  : void
|
+------------------------------------------------------------------------------
*/

GLOBAL BOOL kern_plmn_changed ( void )
{
  GMM_TRACE_FUNCTION ("kern_plmn_changed()");

  if ( (memcmp (gmm_data->kern.sig_cell_info.env.rai.plmn.mnc, gmm_data->kern.attach_cap.rai_accepted.mnc, SIZE_MNC) != 0)
    || (memcmp (gmm_data->kern.sig_cell_info.env.rai.plmn.mcc, gmm_data->kern.attach_cap.rai_accepted.mcc, SIZE_MCC) != 0)
    )
  {
    TRACE_0_INFO ("PLMN changed");
    GMM_RETURN_ (TRUE);
  }
  else
  {
    GMM_RETURN_ (FALSE);
  } 
} /* kern_plmn_changed() */
/*
+------------------------------------------------------------------------------
| Function    : kern_set_rai
+------------------------------------------------------------------------------
| Description : This Procedure sets the old rai and the new rai.
|               
| Parameters  : rai 
|
+------------------------------------------------------------------------------
*/

GLOBAL void kern_set_rai ( T_routing_area_identification * rai)
{
  GMM_TRACE_FUNCTION ("kern_set_rai()");

  /*
   * gmm_data->kern.attach_cap.rai_accepted has to be always the last valid assigned RAI
   * gmm_data->rai includes the last updated lac and or rac depending
   * on the net mode and the oroiginator:
   * net modeII: lac may only chaged by MM so that we know
   * whether we have to update LA or not either upon cell_ind(lac) or
   * activate_ind(lac) dependinig on what receives first. . 
   * That means gmm_data->rai MUST NOT sent to the  network it
   * can be wrong, i.e. new LAC but old RAC! Use always gmm_data->kern.attach_cap.rai_accepted
   * for transmission o the AIR.
   */
  gmm_data->kern.attach_cap.rai_accepted = *rai;

  if (GMMRR_NET_MODE_I==gmm_data->kern.sig_cell_info.net_mode)
  {
    memcpy (gmm_data->kern.mm_cell_env.rai.plmn.mcc, rai->mcc, SIZE_MCC);
    memcpy (gmm_data->kern.mm_cell_env.rai.plmn.mnc, rai->mnc, SIZE_MNC);

    gmm_data->kern.mm_cell_env.rai.lac     = rai->lac;
    gmm_data->kern.mm_cell_env.rai.rac     = rai->rac;
    gmm_data->kern.mm_cell_env.cid         = gmm_data->kern.sig_cell_info.env.cid;
  }
  GMM_RETURN;
} /* kern_set_rai() */

/*
+------------------------------------------------------------------------------
| Function    : kern_gmmrr_suspend
+------------------------------------------------------------------------------
| Description : The function kern_gmmrr_suspend()
|       
|               This procedure suspends GRR
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_gmmrr_suspend ( UBYTE susp_gprs, UBYTE gmmrr_susp_cause, UBYTE susp_type)
{ 
  GMM_TRACE_FUNCTION( "kern_gmmrr_suspend" );

  gmm_data->kern.suspension_type            |= susp_type;
  TRACE_1_OUT_PARA ("susp_type: %x", gmm_data->kern.suspension_type);

  if (GMM_GRR_STATE_CR==gmm_data->kern.attach_cap.grr_state)
  {
    TRACE_0_INFO("suspension delayed untill cell_ind is received");
  }
  else
  {
    PALLOC ( gmmrr_suspend_req, GMMRR_SUSPEND_REQ);
      gmmrr_suspend_req->susp_gprs = susp_gprs;
      gmmrr_suspend_req->gmmrr_susp_cause= gmmrr_susp_cause;
      /* START PATCH UBUOB ISSUE 8379 */
      gmm_data->kern.gmmrr_resume_sent = FALSE;
      gmm_data->kern.attach_cap.grr_state = GMM_GRR_STATE_SUSPENDING;
      /* END PATCH UBUOB ISSUE 8379 */
    PSEND ( hCommGRR, gmmrr_suspend_req );
  }

  GMM_RETURN;
} /* kern_gmmrr_suspend() */
/*
+------------------------------------------------------------------------------
| Function    : kern_gmmrr_stop_waiting_for_transmission
+------------------------------------------------------------------------------
| Description : The function kern_gmmrr_stop_waiting_for_transmission()
|       
|            Is sent after gmmrr_cell_ind if no RAU is needed.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_gmmrr_stop_waiting_for_transmission (void)
{ 
  GMM_TRACE_FUNCTION( "kern_gmmrr_stop_waiting_for_transmission" );
  switch(GET_STATE(CU))
  {
    case CU_NOT_REQUESTED:
      {
        PALLOC (gmmrr_cell_res, GMMRR_CELL_RES);
          gmmrr_cell_res->cu_cause=GMMRR_RA_DEFAULT;
          SET_STATE (CU, CU_CELL_RES_SENT);
        PSEND (hCommGRR, gmmrr_cell_res);
      }
      break;
    case CU_REQUESTED:
      {
        PALLOC (gmmrr_cell_res, GMMRR_CELL_RES);
          gmmrr_cell_res->cu_cause=GMMRR_RA_CU;
        PSEND (hCommGRR, gmmrr_cell_res);
      }
      /* NO break; */
    case CU_REQUESTED_CELL_RES_SENT:
      SET_STATE (CU, CU_CELL_RES_SENT);
      kern_send_llgmm_trigger_req();
      break;
    default:
    case CU_CELL_RES_SENT:
      break;
  }
  GMM_RETURN;
} /* kern_gmmrr_suspend() */

/*
+------------------------------------------------------------------------------
| Function    : kern_gmmrr_attach_started
+------------------------------------------------------------------------------
| Description : The function kern_attach_started()
|       |
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
          /*
           * GMMRR_ATTACH_STARTED_REQ is used to inform GRR that routing area upodate or 
           * attach procedure is started. This time span is closed with 
           * GMMRR_ATTACH_FINISHED_REQ
           * GSM 04.60, ch. 5.5.1.5 Discontinuous reception (DRX):
           * "When initiating the MM procedures for GPRS attach and routeing area update 
           * defined in 3GPP TS 04.08, the mobile station shall enter the MM non- DRX mode 
           * period. This period ends when either of  the messages GPRS ATTACH ACCEPT, 
           * GPRS ATTACH REJECT, ROUTING AREA UPDATE ACCEPT or ROUTING AREA UPDATE REJECT 
           * is received by the mobile station. This period also ends after timeout 
           * waiting for any of these messages."
           */

GLOBAL void kern_gmmrr_attach_started (void)
{ 
  GMM_TRACE_FUNCTION( "kern_gmmrr_attach_started" );
  {
    PALLOC (gmmrr_attach_started_req, GMMRR_ATTACH_STARTED_REQ);
    gmm_data->kern.attach_cap.gmmrr_attach_finished_sent=FALSE;
    PSEND (hCommGRR, gmmrr_attach_started_req);
  }
  GMM_RETURN;
} /* kern_gmmrr_attach_finished() */

/*
+------------------------------------------------------------------------------
| Function    : kern_gmmrr_attach_finished
+------------------------------------------------------------------------------
| Description : The function kern_attach_finished()
|       
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_gmmrr_attach_finished (void)
{ 
  GMM_TRACE_FUNCTION( "kern_gmmrr_attach_finished" );
  if(!gmm_data->kern.attach_cap.gmmrr_attach_finished_sent )
  {
    PALLOC (gmmrr_attach_finished_req, GMMRR_ATTACH_FINISHED_REQ);
    gmm_data->kern.attach_cap.gmmrr_attach_finished_sent=TRUE;
    PSEND (hCommGRR, gmmrr_attach_finished_req);
  }
  GMM_RETURN;
} /* kern_gmmrr_attach_finished() */

/*
+------------------------------------------------------------------------------
| Function    : kern_is_cell_forbidden
+------------------------------------------------------------------------------
| Description : The function kern_is_cell_forbidden checks whether cell is 
|            forbidden for GPRS or not
|       
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL kern_is_cell_forbidden (void)
{ 
  BOOL forbidden = (GMMRR_SERVICE_LIMITED== gmm_data->kern.sig_cell_info.gmm_status);
  
  int i;
  GMM_TRACE_FUNCTION( "kern_is_cell_forbidden" );

  if (MMGMM_LIMITED_SERVICE == gmm_data->kern.sig_cell_info.mm_status)
  {
    TRACE_0_INFO ("mm is in limited");
    GMM_RETURN_ (TRUE);
  }



  if(MODE_AUTO EQ gmm_data->kern.attach_cap.network_selection_mode
    /* START PATCH UBUOB ISSUE 8781 */
    || !gmm_data->kern.attach_cap.gmmreg)
    /* END PATCH UBUOB ISSUE 8781 */
  {
    for(i=0;!forbidden && (i<MAX_LIST_OF_FORBIDDEN_PLMNS_FOR_GPRS_SERVICE); i++)
    {
      forbidden =
       forbidden
       ||
       (
             (memcmp 
             (gmm_data->kern.attach_cap.list_of_forbidden_plmns_for_gprs_service[i].mcc,
                        gmm_data->kern.sig_cell_info.env.rai.plmn.mcc, SIZE_MCC) 
                        == 0
               )
             &&
              (memcmp 
              (gmm_data->kern.attach_cap.list_of_forbidden_plmns_for_gprs_service[i].mnc,
                        gmm_data->kern.sig_cell_info.env.rai.plmn.mnc, SIZE_MNC) 
                        == 0
               )
        );
    }
  }
  TRACE_1_INFO ("cell %s",forbidden?"forbidden":"allowed");
  
  GMM_RETURN_(forbidden);         
      

} /* kern_gmmrr_attach_finished() */

/*
+------------------------------------------------------------------------------
| Function    : kern_remove_plmn_from_forbidden_list
+------------------------------------------------------------------------------
| Description : The function kern_remove_plmn_from_forbidden_list
|       
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_remove_plmn_from_forbidden_list (T_plmn plmn)
{ 
  int i;
  GMM_TRACE_FUNCTION( "kern_remove_plmn_from_forbidden_list" );
  
  for(i=0; i<MAX_LIST_OF_FORBIDDEN_PLMNS_FOR_GPRS_SERVICE; i++)
  {
    BOOL forbidden =
     ((memcmp 
     (gmm_data->kern.attach_cap.list_of_forbidden_plmns_for_gprs_service[i].mcc,
                plmn.mcc, SIZE_MCC) 
                == 0
       )
     &&
      (memcmp 
      (gmm_data->kern.attach_cap.list_of_forbidden_plmns_for_gprs_service[i].mnc,
                plmn.mnc, SIZE_MNC) 
                == 0
       )
      );
    if (forbidden)
    {
      memset(gmm_data->kern.attach_cap.list_of_forbidden_plmns_for_gprs_service[i].mcc, 0xFF, SIZE_MCC);
      memset(gmm_data->kern.attach_cap.list_of_forbidden_plmns_for_gprs_service[i].mnc, 0xFF, SIZE_MNC);
    }
  }
    
  GMM_RETURN;         
      

} /* kern_gmmrr_attach_finished() */

/*
+------------------------------------------------------------------------------
| Function    : kern_send_gmmreg_cell_ind
+------------------------------------------------------------------------------
| Description : The function kern_send_gmmreg_cell_ind informs MMI 
|            that cell has changed or that we GSM is in full service again
|       
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_send_gmmreg_cell_ind (void)
{ 
  BOOL attach_cnf_to_be_sent;
  GMM_TRACE_FUNCTION( "kern_send_gmmreg_cell_ind" );

  attach_cnf_to_be_sent = (MMGMM_FULL_SERVICE == gmm_data->kern.sig_cell_info.mm_status
      && (
          (
            (
              kern_cell_changed() 
              || gmm_data->kern.attach_cap.gmmreg
            )
            && 
            (
              GMMRR_SERVICE_NONE != gmm_data->kern.sig_cell_info.gmm_status
              || GMMRR_SERVICE_NONE == gmm_data->kern.old_sig_cell_info.gmm_status
            )
          )
                                            /* OLD !!! */
          || MMGMM_FULL_SERVICE != gmm_data->kern.old_sig_cell_info.mm_status
        )
      );
  
  switch( GET_STATE( KERN ) )
  {
    case KERN_GMM_NULL_NO_IMSI:
    case KERN_GMM_NULL_NO_IMSI_LIMITED_SERVICE_REQ:
    case KERN_GMM_NULL_IMSI:
    case KERN_GMM_NULL_IMSI_LIMITED_SERVICE_REQ:
    case KERN_GMM_NULL_PLMN_SEARCH:

    case KERN_GMM_DEREG_NO_IMSI:
    case KERN_GMM_DEREG_PLMN_SEARCH:    
    case KERN_GMM_DEREG_NO_CELL_AVAILABLE:
    case KERN_GMM_DEREG_ATTEMPTING_TO_ATTACH:
    case KERN_GMM_DEREG_LIMITED_SERVICE:
    case KERN_GMM_DEREG_RESUMING:
    case KERN_GMM_DEREG_SUSPENDING:
    case KERN_GMM_DEREG_SUSPENDED:

      if (attach_cnf_to_be_sent)
      {
        if ( GMMRR_NET_MODE_III==gmm_data->kern.sig_cell_info.net_mode
        && GMMREG_CLASS_BC==  gmm_data->kern.attach_cap.mobile_class
        && !kern_lau_needed())
        {
          kern_gmmreg_attach_cnf_sr(GMMREG_AT_IMSI, 
                                SEARCH_NOT_RUNNING);
        }
        else
        {          
          switch (gmm_data->kern.sig_cell_info.gmm_status)
          {
            case GMMRR_SERVICE_LIMITED:
            case GMMRR_SERVICE_NONE:
              kern_gmmreg_attach_cnf_sr(GMMREG_AT_IMSI, 
                    SEARCH_NOT_RUNNING);
              break;
            default:
            case GMMRR_SERVICE_FULL:
              kern_gmmreg_attach_cnf_sr(GMMREG_AT_IMSI, 
                     (UBYTE)(gmm_data->sim_gprs_invalid?
                    SEARCH_NOT_RUNNING:SEARCH_RUNNING));
              break;
          }
        }
      }
      break;

    case KERN_GMM_DEREG_INITIATED:
      break;
    case KERN_GMM_REG_INITIATED:
      if (attach_cnf_to_be_sent)
      {
        kern_gmmreg_attach_cnf_sr(GMMREG_AT_IMSI,SEARCH_RUNNING);
      }
      break;
    default:
      TRACE_ERROR ("Unexpexcted state");
      /* NO break */
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE:
    case KERN_GMM_REG_NO_CELL_AVAILABLE:
    case KERN_GMM_REG_LIMITED_SERVICE:
    case KERN_GMM_REG_ATTEMPTING_TO_UPDATE_MM:
    case KERN_GMM_REG_RESUMING:
    case KERN_GMM_REG_SUSPENDED:
    case KERN_GMM_REG_SUSPENDING:
    case KERN_GMM_REG_IMSI_DETACH_INITIATED:
    
    case KERN_GMM_REG_TEST_MODE_NO_IMSI:
    case KERN_GMM_REG_TEST_MODE:
    case KERN_GMM_RAU_INITIATED:
    case KERN_GMM_RAU_WAIT_FOR_NPDU_LIST:

      if (attach_cnf_to_be_sent)
      {
        kern_gmmreg_attach_cnf(GMMREG_AT_COMB);
      }
      break;
    case KERN_GMM_REG_NORMAL_SERVICE:
      if (attach_cnf_to_be_sent)
      {
        kern_gmmreg_attach_cnf(GMMREG_AT_COMB);

#ifdef REL99
        if (gmm_data->kern.sig_cell_info.sgsnr_flag!= gmm_data->kern.old_sig_cell_info.sgsnr_flag) 
        {
          PALLOC (mmpm_attach_ind, MMPM_ATTACH_IND);
          PSEND ( hCommSM, mmpm_attach_ind );
        }
#endif  

      break;
      }
  }
  GMM_RETURN;
} /* kern_gmmreg_cell_ind */
/*
+------------------------------------------------------------------------------
| Function    : kern_send_llgmm_trigger_req
+------------------------------------------------------------------------------
| Description : Sends trigger request with the correct cause value
|
| Parameters  : None
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_send_llgmm_trigger_req(void)
{
   
  GMM_TRACE_FUNCTION ("kern_send_llgmm_trigger_req()");
  {   
    PALLOC (llgmm_trigger_req, LLGMM_TRIGGER_REQ);

#ifdef REL99
    /* 
     * If SGSN supports cell notification indicate that LLC
     * can use NULL frame for performing cell updates.
     * The first cell notification shouldn't use LLC NULL 
     * frame. The following logic takes care of it. 
     */
     switch (gmm_data->cell_notification) /* TCS 4.0 */
      { /* TCS 4.0 */
        case FIRST_CELL_NOTIFY: /* TCS 4.0 */
          /*
           * Next cell update can use LLC NULL frame
           */
          gmm_data->cell_notification = NOT_FIRST_CELL_NOTIFY; /* TCS 4.0 */
           /*
            * This cell update should not use LLC NULL frame
            */
           llgmm_trigger_req->trigger_cause = LLGMM_TRICS_CELL_UPDATE;
          break; /* TCS 4.0 */
        case NOT_FIRST_CELL_NOTIFY:  /* TCS 4.0 */
          /* 
           * Cell notification supported and this is not the first cell update.
           * So LLC NULL frame can be used
           */
          llgmm_trigger_req->trigger_cause = LLGMM_TRICS_CELL_UPDATE_NULL_FRAME;      /* TCS 4.0 */
          break; /* TCS 4.0 */
        default: /* TCS 4.0 */
          /*
           * cell notification not supported by SGSN 
           */
         llgmm_trigger_req->trigger_cause = LLGMM_TRICS_CELL_UPDATE; /* TCS 4.0 */
     } /* TCS 4.0 */
#else
         llgmm_trigger_req->trigger_cause = LLGMM_TRICS_CELL_UPDATE; /* TCS 4.0 */
#endif

     PSEND ( hCommLLC, llgmm_trigger_req );
  }
  GMM_RETURN;
} /* kern_send_llgmm_trigger_req */
/*
+------------------------------------------------------------------------------
| Function    : kern_attach_rau_init
+------------------------------------------------------------------------------
| Description : The function kern_attach_rau_init initilize RAU and attach procedure
|       
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_attach_rau_init (void)
{ 
  GMM_TRACE_FUNCTION( "kern_attach_rau_init" );
  /*
   * PATCH UBUOB 22.7.07: multiple outstanding SIM_AUTHENTICATION_REQ messages
   * for safety reset the counter whenever a new attachment is started
   */

  gmm_data->kern.auth_cap.last_auth_req_id = NOT_PRESENT_8BIT;


  /* START PATCH UBUOB ISSUE 8284 */
  gmm_data->kern.detach_cap.error_cause = GMMCS_INT_NOT_PRESENT; /* TCS 2.1 */
  /* END PATCH UBUOB ISSUE 8284 */

  gmm_data->kern.attach_cap.t3310_value=0;
  vsi_t_stop ( GMM_handle , kern_T3310);

  /* #5899 */
  /*
   * PATCH UBUOB 25.3.02: Stop timer T3302 when you are trying to ATTACH again
   * otherwise it may timeout in the middle of the procedure. Also this ATTACH
   * procedure may be successful and then T3302 also needs to be stopped.
   */
  vsi_t_stop ( GMM_handle , kern_T3302);
  /* end patch */

  /* 
   * PATCH UBUOB 25.10.02: there are cases where T3311 also needs to be stopped:
   * GMM receives CGRLC_STATUS_IND(access not allowed due to cell change), starts T3311 (TCS 2.1)
   * GMMRR_CELL_IND(cell changed), RAU restarted, but forgot to stop T3311
   * trace 25a_night_cell_change_no_patch
   */
  vsi_t_stop ( GMM_handle , kern_T3311);
  /* end patch ubuob 25.10.02 */
  sig_kern_rdy_stop_t3302_req();

  /* 
   * patch ubuob 23.10.02: flag needs to be cleared when initiating RAU or ATTACH
   * and not at end
   */
  SET_STATE (GU,GU_UPDATE_NOT_NEEDED);
  /* end patch ubuob */

  GMM_RETURN;         
      

} /* kern_attach_rau_init */

/*
+------------------------------------------------------------------------------
| Function    : kern_make_cause
+------------------------------------------------------------------------------
| Description : The function kern_make_cause()
|       
|            An internal 16 bit cause is generated from the cause received
|            from the network. The reject causes indicating a retry in a new
|            cell are translated to a single value. In case no cause was sent 
|            by the network a special internal value is returned to handle this.
|
| Parameters  : valid flag, network cause value
|
+------------------------------------------------------------------------------
*/
GLOBAL USHORT kern_make_cause (BOOL cause_valid, UBYTE network_cause)
{
  GMM_TRACE_FUNCTION ("kern_make_cause");
  
  if (cause_valid)
  {
    if (network_cause >= 0x30 AND /* TCS 2.1 */
        network_cause <= 0x3F) /* TCS 2.1 */
    { /* TCS 2.1 */
      GMM_RETURN_ (GMMCS_RETRY_IN_NEW_CELL); /* TCS 2.1 */
    } /* TCS 2.1 */
    else /* TCS 2.1 */
    { /* TCS 2.1 */
      GMM_RETURN_ (CAUSE_MAKE (DEFBY_STD, /* TCS 2.1 */
                         ORIGSIDE_NET, /* TCS 2.1 */
                         GMM_ORIGINATING_ENTITY, /* TCS 2.1 */
                         network_cause)); /* TCS 2.1 */
    } /* TCS 2.1 */
  }
  else
  {
    GMM_RETURN_ (GMMCS_NET_CAUSE_NOT_PRESENT);
  }
}
#ifdef GMM_TCS4
/*
+------------------------------------------------------------------------------
| Function    : kern_make_new_cause
+------------------------------------------------------------------------------
| Description : The function kern_make_new_cause()
|       
|            An internal 16 bit cause is generated from the cause received
|            from the network. The reject causes indicating a retry in a new
|            cell are translated to a single value. In case no cause was sent 
|            by the network a special internal value is returned to handle this.
|
| Parameters  : valid flag, network cause value
|
+------------------------------------------------------------------------------
*/
GLOBAL T_CAUSE_ps_cause kern_make_new_cause ( void )
{
  T_CAUSE_ps_cause ps_cause;
  GMM_TRACE_FUNCTION ("kern_make_cause");
  ps_cause.ctrl_value = CAUSE_is_from_mm;
    
  switch (gmm_data->kern.detach_cap.error_cause)
  {
    case GMMCS_INT_NOT_PRESENT:
    case GMMCS_NET_CAUSE_NOT_PRESENT    :
    case MMCS_INT_NOT_PRESENT:
      ps_cause.value.mm_cause = CAUSE_MM_NO_ERROR;
      break;
    default:
      ps_cause.ctrl_value = CAUSE_is_from_nwmm;
      ps_cause.value.nwmm_cause = gmm_data->kern.detach_cap.error_cause&0xff; /* TCS 4.0 */
      break;
    case MMCS_UNSPECIFIED:
    case GMMCS_INT_PROTOCOL_ERROR:
      ps_cause.value.mm_cause = CAUSE_MM_PROTOCOL_ERROR;
      break;
    case GMMCS_SUCCESS:
      ps_cause.value.mm_cause = CAUSE_MM_SUCCESS;
      break;
    case GMMCS_AAC_UNDER_5:
    case GMMCS_AAC_OVER_5:
      ps_cause.value.mm_cause = CAUSE_MM_TRY_TO_UPDATE;
      break;
    case MMCS_AUTHENTICATION_REJECTED:
    case GMMCS_AUTHENTICATION_REJECTED:
      ps_cause.value.mm_cause = CAUSE_MM_AUTHENTICATION_REJECTED;
      break;
    case MMCS_SIM_REMOVED:
    case GMMCS_SIM_REMOVED:
      ps_cause.value.mm_cause = CAUSE_MM_SIM_REMOVED;
      break;
    case GMMCS_POWER_OFF:
      ps_cause.value.mm_cause = CAUSE_MM_POWER_OFF;
      break;
    case GMMCS_LIMITED_SERVICE:
      ps_cause.value.mm_cause = CAUSE_MM_LIMITED_SERVICE;
      break;
    case MMCS_SUCCESS:
      ps_cause.value.mm_cause = CAUSE_MM_SUCCESS;
      break;
    case MMCS_NO_REGISTRATION:
      ps_cause.value.mm_cause = CAUSE_MM_NO_REGISTRATION;
      break;      
    case MMCS_TIMER_RECOVERY:
      ps_cause.value.mm_cause = CAUSE_MM_TIMER_RECOVERY;
      break;      
    case MMCS_NO_REESTABLISH:
      ps_cause.value.mm_cause = CAUSE_MM_NO_REESTABLISH;
      break;      
    case MMCS_INT_PREEM:
      ps_cause.value.mm_cause = CAUSE_MM_INT_PREEM;
      break;      
    case MMCS_PLMN_NOT_IDLE_MODE:
      ps_cause.value.mm_cause = CAUSE_MM_PLMN_NOT_IDLE_MODE;
      break;   


  }
  GMM_RETURN_ (ps_cause);
}
#endif

/*
+------------------------------------------------------------------------------
| Function    : kern_call_undone_mm_proc_der 
+------------------------------------------------------------------------------
| Description : This function starts all undone MM procedures after finishing the last 
|            MM procedure
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_call_undone_mm_proc_der ( void )
{ 
  GMM_TRACE_FUNCTION( "kern_call_undone_mm_proc_der" );
  if (!gmm_data->kern.suspension_type)
  /*
   * default branch
   */
  {
    kern_resume_grr_der(); 
    GMM_RETURN;
  }


  if (GMM_SUSP_EM_CALL & gmm_data->kern.suspension_type)
  {
    kern_mm_cm_emergency_res(MMGMM_ESTABLISH_OK);
  }
  else if (GMM_SUSP_LAU & gmm_data->kern.suspension_type)
  {
    kern_mm_lau();
  }
  else if (GMM_SUSP_CALL & gmm_data->kern.suspension_type)
  {
    kern_mm_cm_establish_res(MMGMM_ESTABLISH_OK);
  }
  else if (GMM_SUSP_IMSI_DETACH & gmm_data->kern.suspension_type)
  {
    kern_mm_imsi_detach();
  }
  else if (GMM_SUSP_LOCAL_DETACH & gmm_data->kern.suspension_type)
  {
    kern_local_detach ( GMMCS_INT_NOT_PRESENT, FALSE, GMM_LOCAL_DETACH_PROC_NOT_CHANGED); /* TCS 2.1 */

  }
  GMM_RETURN;
}
/*
+------------------------------------------------------------------------------
| Function    : kern_call_undone_mm_proc_reg 
+------------------------------------------------------------------------------
| Description : This function starts all undone MM procedures after finishing the last 
|            MM procedure
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_call_undone_mm_proc_reg ( void )
{ 
  GMM_TRACE_FUNCTION( "kern_call_undone_mm_proc_reg" );
  if (GMM_SUSP_NONE == gmm_data->kern.suspension_type)
  /*
   * default branch
   */
  {
    kern_resume_grr_reg(); 
    GMM_RETURN;
  }


  if (GMM_SUSP_EM_CALL & gmm_data->kern.suspension_type)
  {
    kern_mm_cm_emergency_res(MMGMM_ESTABLISH_OK);
  }
  else if (GMM_SUSP_LAU & gmm_data->kern.suspension_type)
  {
    kern_mm_lau();
  }
  else if (GMM_SUSP_CALL & gmm_data->kern.suspension_type)
  {
    kern_mm_cm_establish_res(MMGMM_ESTABLISH_OK);
  }
  else if (GMM_SUSP_IMSI_DETACH & gmm_data->kern.suspension_type)
  {
    kern_mm_imsi_detach();
  }
  else if (GMM_SUSP_LOCAL_DETACH & gmm_data->kern.suspension_type)
  {
    kern_local_detach ( GMMCS_INT_NOT_PRESENT, FALSE, GMM_LOCAL_DETACH_PROC_NOT_CHANGED); /* TCS 2.1 */
  }
  GMM_RETURN;
}

/*
+------------------------------------------------------------------------------
| Function    : kern_reset_cipher
+------------------------------------------------------------------------------
| Description : This function deletes the ciphering parameters
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void kern_reset_cipher ( void )
{ 
  GMM_TRACE_FUNCTION( "kern_reset_cipher" );
  
  /*
   * 3GPP,24.008:
   * 4.7.7.2   Authentication and ciphering response by the MS
   *
   * The RAND and RES values stored in the mobile station shall be deleted:
   * - if the mobile station enters the GMM states GMM-DEREGISTERED or GMM-NULL.
   */
  /* 
   * 3GPP,24.008:
   * 4.7.7.4  GPRS ciphering key sequence number
   *
   * If the GPRS ciphering key sequence number is deleted, the associated GPRS GSM 
   * ciphering key , GPRS UMTS ciphering key and GPRS UMTS integrity key shall be deleted 
   * (i.e. the established GSM security context or the UMTS security context is no longer
   * valid).  
   */
  TRACE_EVENT ("ciphering OFF");
  gmm_data->cipher = FALSE;

  GMM_RETURN;
}


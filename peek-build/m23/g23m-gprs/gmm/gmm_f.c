/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  $
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
|  Purpose :  This modul is part of the entity GRR and implements all 
|             procedures and functions as described in the 
|             SDL-documentation (-statemachine)
+----------------------------------------------------------------------------- 
*/ 



#ifndef GMM_F_C
#define GMM_F_C
#endif

#define ENTITY_GMM

/*==== INCLUDES =============================================================*/

#include "typedefs.h"	 /* to get Condat data types */
#include "vsi.h"	/* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"	/* to get a lot of macros */
#include "ccdapi.h"	/* to get CCD API */
#include "cnf_gmm.h"	/* to get cnf-definitions */
#include "mon_gmm.h"	/* to get mon-definitions */
#include "prim.h"	/* to get the definitions of used SAP and directions */
#include "gmm.h"	/* to get the global entity definitions */

#include "gmm_f.h"
#include <stdlib.h>	/* JK, delete warnings: to get rand() */

/*==== CONST ================================================================*/

/*==== DIAGNOSTICS ==========================================================*/

#ifndef TRACE_INFO	 /* only used in local environment */
#  define TRACE_INFO(x)
#endif

#ifndef TRACE_COMMENT	 /* only used in local environment */
#  define TRACE_COMMENT(x)
#endif


/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/
/*
+------------------------------------------------------------------------------
| Function    : gmm_set_current_tlli
+------------------------------------------------------------------------------
| Description : derives the current TLLI with the given TLLI type and saves
|		the old TLLI
|
| Parameters  : tlli_type - the wanted TLLI type
|
+------------------------------------------------------------------------------
*/
GLOBAL void gmm_set_current_tlli ( T_TLLI_TYPE tlli_type ) 
{
  GMM_TRACE_FUNCTION ("gmm_set_current_tlli");

  gmm_data->tlli.old	  = gmm_data->tlli.current,
  gmm_data->tlli.current  = gmm_get_tlli ( tlli_type );
      
  GMM_TRACE_GMM_DATA(GMM_DEBUG_PRINT_MASK_TLLI);

  GMM_RETURN;
} /* gmm_set_current_tlli */


/*
+------------------------------------------------------------------------------
| Function    : gmm_get_tlli
+------------------------------------------------------------------------------
| Description : returns the wanted TLLI
|
| Parameters  : tlli_type - the wanted TLLI type
|
+------------------------------------------------------------------------------
*/
GLOBAL ULONG gmm_get_tlli ( T_TLLI_TYPE tlli_type ) 
{
  GMM_TRACE_FUNCTION ("gmm_get_tlli()");

  /* 
   * the random tlli must be created 
   */
  
  if ( GMMRR_TMSI_INVALID == gmm_data->ptmsi.current
  /* 
   * the random tlli must be created 
   */
  
  &&   tlli_type != RANDOM_TLLI 
  &&   tlli_type != CURRENT_TLLI )
  {
    gmm_data->ptmsi.current= GMMRR_TMSI_INVALID;
    gmm_data->ptmsi.new_grr = GMMRR_TMSI_INVALID;
    gmm_data->ptmsi.old    = GMMRR_TMSI_INVALID;
    gmm_data->tlli.current_type = INVALID_TLLI;
    TRACE_0_OUT_PARA ("first tlli used");
    GMM_RETURN_ ( LLGMM_TLLI_INVALID);
  }
  else
  {
    /* gmm_data->ptmsi.v_mid = TRUE; */
   
    switch ( tlli_type )
    {
      case LOCAL_TLLI:
        gmm_data->tlli.current_type = tlli_type;
    
        gmm_data->tlli.current =  gmm_data->ptmsi.current | LOCAL_TLLI_SET_MASK;
        gmm_data->tlli.current &= LOCAL_TLLI_RES_MASK;
        TRACE_0_OUT_PARA("local tlli used");
        break;
      case FOREIGN_TLLI:
        gmm_data->tlli.current_type = tlli_type;
        
        gmm_data->tlli.current =  gmm_data->ptmsi.current | FOREIGN_TLLI_SET_MASK;
        gmm_data->tlli.current &=  FOREIGN_TLLI_RES_MASK;
        TRACE_0_OUT_PARA ("foreign tlli used");
        break;  
      case RANDOM_TLLI:
        gmm_data->tlli.current_type = CURRENT_TLLI;

        #ifdef _SIMULATION_
          /* This number is a very random number */
          gmm_data->ptmsi.current = 0x06081997L;
        #else
          gmm_data->ptmsi.current = (rand()<<16) +rand();
        #endif

        gmm_data->tlli.current	=  gmm_data->ptmsi.current | RANDOM_TLLI_SET_MASK;
        gmm_data->tlli.current	&= RANDOM_TLLI_RES_MASK;
        gmm_data->ptmsi.current = GMMRR_TMSI_INVALID;
        gmm_data->ptmsi.old    = GMMRR_TMSI_INVALID;

        TRACE_0_OUT_PARA ("random tlli used");
        break;
      case OLD_TLLI:
        GMM_RETURN_ (gmm_data->tlli.old);
      case CURRENT_TLLI:
        GMM_RETURN_ (gmm_data->tlli.current)/*gmm_get_tlli ( gmm_data->tlli.current_type )*/;
      case INVALID_TLLI:
        TRACE_0_OUT_PARA ("first tlli used");
        GMM_RETURN_ (LLGMM_TLLI_INVALID);
      default:
        TRACE_ERROR ("WRONG TLLI TYPE received");
        break;
    }
    GMM_RETURN_ (gmm_data->tlli.current);
  }
} /* gmm_get_tlli */

/*
+------------------------------------------------------------------------------
| Function    : gmm_debug_do_print_gmm_data
+------------------------------------------------------------------------------
| Description : prints gmm data control block
|
| Parameters  : print mask: select type of information to be printed
|
+------------------------------------------------------------------------------
*/
GLOBAL void gmm_debug_do_print_gmm_data(GMM_DEBUG_PRINT_MASK_ENUM_TYPE print_mask)
{
   /*
    * COMMON
    */
   if (print_mask & GMM_DEBUG_PRINT_MASK_COMMON)
   {
      TRACE_1_DATA("* Version : %u", gmm_data->version);
      TRACE_1_DATA("*  Cipher : %u", gmm_data->cipher);
   }
   /*
    * STATE
    */
   if (print_mask & GMM_DEBUG_PRINT_MASK_STATE)
   {
#ifndef NTRACE
      TRACE_1_DATA("* Kern State : %s", gmm_data->kern.state_name);
      TRACE_1_DATA("*   TX State : %s", gmm_data->tx.state_name);
      TRACE_1_DATA("*   RX State : %s", gmm_data->rx.state_name);
      TRACE_1_DATA("*  RDY State : %s", gmm_data->rdy.state_name);
      TRACE_1_DATA("* SYNC State : %s", gmm_data->sync.state_name);
#endif
   }
   /*
    * TLLI
    */
   if (print_mask & GMM_DEBUG_PRINT_MASK_TLLI)
   {
      TRACE_1_DATA("* TLLI current : %#x", gmm_data->tlli.current);
      TRACE_1_DATA("* TLLI old     : %#x", gmm_data->tlli.old);
      switch (gmm_data->tlli.current_type)
      {
      case LOCAL_TLLI:   TRACE_0_DATA("* TLLI type    : LOCAL"); break;
      case FOREIGN_TLLI: TRACE_0_DATA("* TLLI type    : FOREIGN"); break;
      case RANDOM_TLLI:  TRACE_0_DATA("* TLLI type    : RANDOM"); break;
      case CURRENT_TLLI: TRACE_0_DATA("* TLLI type    : CURRENT"); break;
      case OLD_TLLI:     TRACE_0_DATA("* TLLI type    : OLD"); break;
      case INVALID_TLLI: TRACE_0_DATA("* TLLI type    : INVALID"); break;
      default:
          TRACE_1_DATA("* TLLI type    : %u unknown!", gmm_data->tlli.current_type);
      }
   }
   /*
    * IMSI
    */
   if (print_mask & GMM_DEBUG_PRINT_MASK_IMSI)
   {
      TRACE_8_DATA("* IMSI[]: %1d %1d %1d %1d  %1d %1d %1d %1d", 
         gmm_data->imsi.id[0], gmm_data->imsi.id[1],
         gmm_data->imsi.id[2], gmm_data->imsi.id[3],
         gmm_data->imsi.id[4], gmm_data->imsi.id[5],
         gmm_data->imsi.id[6], gmm_data->imsi.id[7]);
      TRACE_8_DATA("* IMSI[]: %1d %1d %1d %1d  %1d %1d %1d %1d", 
         gmm_data->imsi.id[8], gmm_data->imsi.id[9],
         gmm_data->imsi.id[10], gmm_data->imsi.id[11],
         gmm_data->imsi.id[12], gmm_data->imsi.id[13],
         gmm_data->imsi.id[14], gmm_data->imsi.id[15]);
   }
   /*
    * P-TMSI
    */
   if (print_mask & GMM_DEBUG_PRINT_MASK_PTMSI)
   {
      TRACE_1_DATA("* PTMSI current: %#x", gmm_data->ptmsi.current);
      TRACE_1_DATA("* PTMSI old    : %#x", gmm_data->ptmsi.old);
      TRACE_1_DATA("* PTMSI grr    : %#x", gmm_data->ptmsi.new_grr);
   }
   /*
    * CID
    */
   if (print_mask & GMM_DEBUG_PRINT_MASK_CID)
   {
      TRACE_0_DATA("* kern.                     cid    rac    lac mcc mnc");
      TRACE_9_DATA("*                cell_id %#6x %#6x %#6x %1x%1x%1x %1x%1x%1x", 
         gmm_data->kern.cell_id.cid,
         gmm_data->kern.cell_id.rac,
         gmm_data->kern.cell_id.lac,
         gmm_data->kern.cell_id.plmn.mcc[0],
         gmm_data->kern.cell_id.plmn.mcc[1],
         gmm_data->kern.cell_id.plmn.mcc[2],
         gmm_data->kern.cell_id.plmn.mnc[0],
         gmm_data->kern.cell_id.plmn.mnc[1],
         gmm_data->kern.cell_id.plmn.mnc[2]);
      TRACE_9_DATA("*            mm_cell_env %#6x %#6x %#6x %1x%1x%1x %1x%1x%1x",
         gmm_data->kern.mm_cell_env.cid,
         gmm_data->kern.mm_cell_env.rai.rac,
         gmm_data->kern.mm_cell_env.rai.lac,
         gmm_data->kern.mm_cell_env.rai.plmn.mcc[0],
         gmm_data->kern.mm_cell_env.rai.plmn.mcc[1],
         gmm_data->kern.mm_cell_env.rai.plmn.mcc[2],
         gmm_data->kern.mm_cell_env.rai.plmn.mnc[0],
         gmm_data->kern.mm_cell_env.rai.plmn.mnc[1],
         gmm_data->kern.mm_cell_env.rai.plmn.mnc[2]);
      TRACE_9_DATA("*      sig_cell_info.env %#6x %#6x %#6x %1x%1x%1x %1x%1x%1x",
         gmm_data->kern.sig_cell_info.env.cid,
         gmm_data->kern.sig_cell_info.env.rai.rac,
         gmm_data->kern.sig_cell_info.env.rai.lac,
         gmm_data->kern.sig_cell_info.env.rai.plmn.mcc[0],
         gmm_data->kern.sig_cell_info.env.rai.plmn.mcc[1],
         gmm_data->kern.sig_cell_info.env.rai.plmn.mcc[2],
         gmm_data->kern.sig_cell_info.env.rai.plmn.mnc[0],
         gmm_data->kern.sig_cell_info.env.rai.plmn.mnc[1],
         gmm_data->kern.sig_cell_info.env.rai.plmn.mnc[2]);
      TRACE_9_DATA("*  old_sig_cell_info.env %#6x %#6x %#6x %1x%1x%1x %1x%1x%1x",
         gmm_data->kern.old_sig_cell_info.env.cid,
         gmm_data->kern.old_sig_cell_info.env.rai.rac,
         gmm_data->kern.old_sig_cell_info.env.rai.lac,
         gmm_data->kern.old_sig_cell_info.env.rai.plmn.mcc[0],
         gmm_data->kern.old_sig_cell_info.env.rai.plmn.mcc[1],
         gmm_data->kern.old_sig_cell_info.env.rai.plmn.mcc[2],
         gmm_data->kern.old_sig_cell_info.env.rai.plmn.mnc[0],
         gmm_data->kern.old_sig_cell_info.env.rai.plmn.mnc[1],
         gmm_data->kern.old_sig_cell_info.env.rai.plmn.mnc[2]);
      TRACE_0_DATA("* sync.                     cid    rac    lac mcc mnc");
      TRACE_8_DATA("*           mm_cell_info %#6x ------ %#6x %1x%1x%1x %1x%1x%1x",
         gmm_data->sync.mm_cell_info.cid,
         gmm_data->sync.mm_cell_info.lac,
         gmm_data->sync.mm_cell_info.plmn.mcc[0],
         gmm_data->sync.mm_cell_info.plmn.mcc[1],
         gmm_data->sync.mm_cell_info.plmn.mcc[2],
         gmm_data->sync.mm_cell_info.plmn.mnc[0],
         gmm_data->sync.mm_cell_info.plmn.mnc[1],
         gmm_data->sync.mm_cell_info.plmn.mnc[2]);
      TRACE_9_DATA("* grr_cell_info.cell_env %#6x %#6x %#6x %1x%1x%1x %1x%1x%1x", 
         gmm_data->sync.grr_cell_info.cell_env.cid, 
         gmm_data->sync.grr_cell_info.cell_env.rai.rac, 
         gmm_data->sync.grr_cell_info.cell_env.rai.lac,
         gmm_data->sync.grr_cell_info.cell_env.rai.plmn.mcc[0],
         gmm_data->sync.grr_cell_info.cell_env.rai.plmn.mcc[1],
         gmm_data->sync.grr_cell_info.cell_env.rai.plmn.mcc[2],
         gmm_data->sync.grr_cell_info.cell_env.rai.plmn.mnc[0],
         gmm_data->sync.grr_cell_info.cell_env.rai.plmn.mnc[1],
         gmm_data->sync.grr_cell_info.cell_env.rai.plmn.mnc[2]);
      TRACE_9_DATA("*      sig_cell_info.env %#6x %#6x %#6x %1x%1x%1x %1x%1x%1x",
         gmm_data->sync.sig_cell_info.env.cid,
         gmm_data->sync.sig_cell_info.env.rai.rac, 
         gmm_data->sync.sig_cell_info.env.rai.lac,
         gmm_data->sync.sig_cell_info.env.rai.plmn.mcc[0],
         gmm_data->sync.sig_cell_info.env.rai.plmn.mcc[1],
         gmm_data->sync.sig_cell_info.env.rai.plmn.mcc[2],
         gmm_data->sync.sig_cell_info.env.rai.plmn.mnc[0],
         gmm_data->sync.sig_cell_info.env.rai.plmn.mnc[1],
         gmm_data->sync.sig_cell_info.env.rai.plmn.mnc[2]);

      TRACE_0_DATA("* kern.attach_cap.                 rac    lac mcc mnc");
      TRACE_8_DATA("*           rai_accepted        %#6x %#6x %1x%1x%1x %1x%1x%1x", 
         gmm_data->kern.attach_cap.rai_accepted.rac,
         gmm_data->kern.attach_cap.rai_accepted.lac,
         gmm_data->kern.attach_cap.rai_accepted.mcc[0],
         gmm_data->kern.attach_cap.rai_accepted.mcc[1],
         gmm_data->kern.attach_cap.rai_accepted.mcc[2],
         gmm_data->kern.attach_cap.rai_accepted.mnc[0],
         gmm_data->kern.attach_cap.rai_accepted.mnc[1],
         gmm_data->kern.attach_cap.rai_accepted.mnc[2]);
   }
   /*
    * RAI
    */
   /*
    * KERN ATTACH CAPABILITIES
    */
   if (print_mask & GMM_DEBUG_PRINT_MASK_KERN_ATTACH)
   {
   }
}

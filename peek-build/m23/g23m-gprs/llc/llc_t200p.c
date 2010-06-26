/* 
+----------------------------------------------------------------------------- 
|  Project :  
|  Modul   :  
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
|  Purpose :  This modul is part of the entity LLC and implements all 
|             functions to handles the incoming primitives as described in 
|             the SDL-documentation (T200-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#ifndef LLC_T200P_C
#define LLC_T200P_C
#endif

#define ENTITY_LLC

/*==== INCLUDES =============================================================*/

#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "cnf_llc.h"    /* to get cnf-definitions */
#include "mon_llc.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "llc.h"        /* to get the global entity definitions */

#include "llc_t200f.h"  /* to get local T200 functions */
#include "llc_us.h"     /* to get signal interface to U */
#include "llc_f.h"     /* to get llc functions */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : t200_timer_t200
+------------------------------------------------------------------------------
| Description : Handles expiry of timer T200. This function is called from
|               pei_primitive().
|
| Parameters  : sapi - indicates the SAPI for which the timer has expired
|
+------------------------------------------------------------------------------
*/
GLOBAL void t200_timer_t200 (UBYTE sapi)
{
#ifndef LL_DESC 
  T_LL_UNITDATA_REQ *frame;
#else
  T_LL_UNITDESC_REQ *frame;
#endif
  UBYTE             cause;
  T_SERVICE         originator;

  TRACE_FUNCTION( "t200_timer_t200" );

  /*
   * Switch all service incarnations of LLC according to sapi.
   */
  SWITCH_LLC (sapi);
  
  switch (GET_STATE(T200))
  {
    case T200_RUNNING:
      SET_STATE (T200, T200_RESET);

      /*
       * No frame with P bit set to 1 is currently outstanding
       */
      llc_data->sapi->pbit_outstanding = FALSE;

      /*
       * Get associated timer data (frame, cause, originating service)
       * and send T200 expired signal, if originator is set. Otherwise
       * just PFREE the frame.
       */
      t200_get_timer_data (&frame, &cause, &originator);
      if (originator EQ SERVICE_U)
      {
        TRACE_0_INFO("T200 expired");
        sig_t200_u_expired_ind (frame, cause, EXPIRY_TIMED);
      }
      else
      {
        TRACE_0_INFO("T200 expired, no service associated, frame discarded");
#ifdef LL_DESC
        /*
         * The descriptor contents of the primitive structure
         * must be freed as well.
         */
        llc_cl_desc3_free((T_desc3*)llc_data->t200->frame->desc_list3.first);
#endif /* LL_DESC */
        PFREE (frame);
      }
      break;

    default:
      t200_get_timer_data (&frame, &cause, &originator);
      if (frame != NULL)
      {
#ifdef LL_DESC
        /*
         * The descriptor contents of the primitive structure
         * must be freed as well.
         */
        llc_cl_desc3_free((T_desc3*)llc_data->t200->frame->desc_list3.first);
#endif /* LL_DESC */
        PFREE (frame);
      }
      TRACE_ERROR( "T200 unexpected" );
      break;
  }

} /* t200_timer_t200() */


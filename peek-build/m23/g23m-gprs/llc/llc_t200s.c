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
|             functions to handles the incoming process internal signals as  
|             described in the SDL-documentation (T200-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#ifndef LLC_T200S_C
#define LLC_T200S_C
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
| Function    : sig_llme_t200_assign_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_LLME_T200_ASSIGN_REQ
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_llme_t200_assign_req (void)
{ 
  TRACE_ISIG( "sig_llme_t200_assign_req" );
  
  switch (GET_STATE(T200))
  {
    case T200_TLLI_UNASSIGNED:
      SET_STATE (T200, T200_RESET);
      break;
    default:
      TRACE_ERROR( "SIG_LLME_T200_ASSIGN_REQ unexpected" );
      break;
  }
} /* sig_llme_t200_assign_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_llme_t200_unassign_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_LLME_T200_UNASSIGN_REQ
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_llme_t200_unassign_req (void)
{ 
  TRACE_ISIG( "sig_llme_t200_unassign_req" );
  
  switch (GET_STATE(T200))
  {
    case T200_RESET:
      SET_STATE (T200, T200_TLLI_UNASSIGNED);
      break;
    case T200_RUNNING:
      t200_stop();

      /*
       * Indicate that no frame with P bit set to 1 is currently outstanding
       */
      llc_data->sapi->pbit_outstanding = FALSE;

      SET_STATE (T200, T200_TLLI_UNASSIGNED);
      break;
    default:
      TRACE_ERROR( "SIG_LLME_T200_UNASSIGN_REQ unexpected" );
      break;
  }
} /* sig_llme_t200_unassign_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_u_t200_start_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_U_T200_START_REQ
|
| Parameters  : frame - a valid pointer to the frame that shall be associated
|                       with the timer
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_u_t200_start_req 
(
#ifdef LL_DESC
  T_LL_UNITDESC_REQ *frame, 
#else
  T_LL_UNITDATA_REQ *frame, 
#endif
  UBYTE cause
)
{ 
  TRACE_ISIG( "sig_u_t200_start_req" );
  
  switch (GET_STATE(T200))
  {
    case T200_RESET:
      t200_start (frame, cause, SERVICE_U);

      /*
       * Indicate that a frame with P bit set to 1 is currently outstanding
       */
      llc_data->sapi->pbit_outstanding = TRUE;

      SET_STATE (T200, T200_RUNNING);
      break;
    default:
#ifdef LL_DESC
      /*
       * The descriptor contents of the primitive structure
       * must be freed as well.
       */
      llc_cl_desc3_free((T_desc3*)frame->desc_list3.first);
#endif /* LL_DESC */
      PFREE (frame);
      TRACE_ERROR( "SIG_U_T200_START_REQ unexpected" );
      break;
  }
} /* sig_u_t200_start_req() */


/*
+------------------------------------------------------------------------------
| Function    : sig_u_t200_stop_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_U_T200_STOP_REQ
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_u_t200_stop_req (void)
{ 
  TRACE_ISIG( "sig_u_t200_stop_req" );
  
  switch (GET_STATE(T200))
  {
    case T200_RUNNING:
      t200_stop();

      /*
       * Indicate that no frame with P bit set to 1 is currently outstanding
       */
      llc_data->sapi->pbit_outstanding = FALSE;

      SET_STATE (T200, T200_RESET);
      break;
    default:
      break;
  }
} /* sig_u_t200_stop_req() */


/*
+------------------------------------------------------------------------------
| Function    : sig_u_t200_expire_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_U_T200_EXPIRE_REQ
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_u_t200_expire_req (void)
{ 
#ifndef LL_DESC 
  T_LL_UNITDATA_REQ *frame;
#else
  T_LL_UNITDESC_REQ *frame;
#endif
  UBYTE             cause;
  T_SERVICE         originator;

  TRACE_ISIG( "sig_u_t200_expire_req" );
  
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
        sig_t200_u_expired_ind (frame, cause, EXPIRY_REQUESTED);
      }
      else
      {
        TRACE_0_INFO("T200 expired, no service associated, frame discarded");
#ifdef LL_DESC
      /*
       * The descriptor contents of the primitive structure
       * must be freed as well.
       */
      llc_cl_desc3_free((T_desc3*)frame->desc_list3.first);
#endif /* LL_DESC */
        PFREE (frame);
      }
      break;

    default:
      TRACE_ERROR( "SIG_U_T200_EXPIRE_REQ unexpected" );
      break;
  }
} /* sig_u_t200_expire_req() */


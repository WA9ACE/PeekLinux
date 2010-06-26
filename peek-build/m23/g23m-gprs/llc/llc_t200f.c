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
|             procedures and functions as described in the 
|             SDL-documentation (T200-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#ifndef LLC_T200F_C
#define LLC_T200F_C
#endif

#define ENTITY_LLC

/*==== INCLUDES =============================================================*/

#include <string.h>     /* to get memcpy() */

#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "cnf_llc.h"    /* to get cnf-definitions */
#include "mon_llc.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "llc.h"        /* to get the global entity definitions */
#include "llc_f.h"      /* to get the global function */
#include "llc_t200f.h"  /* to get local typedef's and defines */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/


/*
+------------------------------------------------------------------------------
| Function    : t200_init
+------------------------------------------------------------------------------
| Description : This procedure initialises all necessary variables of 
|               timer_t200 for all SAPIs.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void t200_init (void)
{ 
  UBYTE inc;


  TRACE_FUNCTION( "t200_init" );

  /*
   * Initialise timer data structures and initial state of every incarnation.
   * The timer length is not set in this procedure, it is instead initialised
   * by llc_init_parameters().
   */
  for (inc = 0; inc < T200_NUM_INC; inc++)
  {
    SWITCH_SERVICE (llc, t200, inc);

    /*
     * Free old used resources (in case of an LLC restart):
     * memory, stored primitives, running timer.
     */
    t200_stop();

  }

  SWITCH_SERVICE (llc, t200, 0);
  INIT_STATE (T200_0, T200_TLLI_UNASSIGNED);

  SWITCH_SERVICE (llc, t200, 1);
  INIT_STATE (T200_1, T200_TLLI_UNASSIGNED);
  
  SWITCH_SERVICE (llc, t200, 2);
  INIT_STATE (T200_2, T200_TLLI_UNASSIGNED);
  
  SWITCH_SERVICE (llc, t200, 3);
  INIT_STATE (T200_3, T200_TLLI_UNASSIGNED);
  
  SWITCH_SERVICE (llc, t200, 4);
  INIT_STATE (T200_4, T200_TLLI_UNASSIGNED);
  
  SWITCH_SERVICE (llc, t200, 5);
  INIT_STATE (T200_5, T200_TLLI_UNASSIGNED);

  return;
} /* t200_init() */


/*
+------------------------------------------------------------------------------
| Function    : t200_start
+------------------------------------------------------------------------------
| Description : This procedure starts timer T200 for the current sapi and 
|               stores a copy of the given frame and the originator. The 
|               frame address is a pointer to a LL_UNITDATA_REQ primitive 
|               which is associated with T200 and will be freed by the lower 
|               layers after transmission. Therefore a copy has to be stored. 
|               cause is the associated GRLC_DATA_REQ!cause for the frame. 
|               originator is an identifier of the service who started the 
|               timer. The frame is returned to the service (originator) 
|               which started T200 with the corresponding EXPIRED signal.
|
| Parameters  : frame       - a pointer to the frame that shall be associated 
|                             with the timer
|               cause       - RLC/MAC cause of frame
|               originator  - the service which started the timer, must not be
|                             NO_SERVICE
|
+------------------------------------------------------------------------------
*/
GLOBAL void t200_start 
(
#ifdef LL_DESC 
 T_LL_UNITDESC_REQ *frame, 
#else
 T_LL_UNITDATA_REQ *frame, 
#endif
 UBYTE cause,
 T_SERVICE originator
)
{
#ifdef LL_DESC
  USHORT  frame_len, frame_offset;
  UBYTE   *prim_buf, *frame_buf;
  T_desc3 *desc3, *frame_desc3;
#endif /* LL_DESC */

  TRACE_FUNCTION( "t200_start" );

#ifdef LL_DESC
  /*
   * Start timer T200 for the current SAPI.
   */
#ifdef _SIMULATION_
  /*
   * In simulation mode, use always a timer value of 5 seconds.
   */
  vsi_t_start (VSI_CALLER
    (USHORT)(TIMER_T200_BASE + UIMAP(llc_data->current_sapi)),
    5000);
#else
  vsi_t_start (VSI_CALLER
    (USHORT)(TIMER_T200_BASE + UIMAP(llc_data->current_sapi)),
    llc_data->t200->length);
#endif /* _SIMULATION_ */

  {
    /*
     * Allocate copy of frame which shall be stored. Because an exact copy
     * of frame shall be stored, the SDU offset must be added to the used
     * SDU length.
     */
    frame_desc3    = (T_desc3*)frame->desc_list3.first;
    frame_len      = frame->desc_list3.list_len;
    frame_offset   = frame_desc3->offset;
    {    
      PALLOC(ll_unitdesc_req, LL_UNITDESC_REQ);
      desc3 = llc_palloc_desc(frame_len, frame_offset);/* Only one desc3 and buffer are allocated */
      
      /*
       * Copy primitive contents to newly allocated primitive. SDU contents
       * have to be copied separately, because they are allocated dynamically
       * and not of known size in the primitve structure:
       * First copy primitive with SDU, but only sdu.l_buf/o_buf and first octet
       * of sdu.buf.
       * Then memcpy actual SDU content with size defined in sdu.l_buf.
       */
      *ll_unitdesc_req = *frame;
      ll_unitdesc_req->desc_list3.first = (ULONG)desc3;

      prim_buf  = (UBYTE*)desc3->buffer;
      frame_buf = (UBYTE*)frame_desc3->buffer; 

      memcpy (&(prim_buf[desc3->offset]), 
        &(frame_buf[frame_offset]), frame_len);

      /*
       * Store the given frame address, RLC/MAC cause, and the originator 
       * for this timer.
       */
      llc_data->t200->frame       = ll_unitdesc_req;
      llc_data->t200->cause       = cause;
      llc_data->t200->originator  = originator;
    }
  }

  return;

#else /* LL_DESC */
  /*
   * Start timer T200 for the current SAPI.
   */
#ifdef _SIMULATION_
  /*
   * In simulation mode, use always a timer value of 5 seconds.
   */
  vsi_t_start (VSI_CALLER
    (USHORT)(TIMER_T200_BASE + UIMAP(llc_data->current_sapi)),
    5000);
#else
  vsi_t_start (VSI_CALLER
    (USHORT)(TIMER_T200_BASE + UIMAP(llc_data->current_sapi)),
    llc_data->t200->length);
#endif /* _SIMULATION_ */

  {
    /*
     * Allocate copy of frame which shall be stored. Because an exact copy
     * of frame shall be stored, the SDU offset must be added to the used
     * SDU length.
     */
    PALLOC_SDU (ll_unitdata_req, LL_UNITDATA_REQ, 
      (USHORT)(frame->sdu.o_buf + frame->sdu.l_buf));

    /*
     * Copy primitive contents to newly allocated primitive. SDU contents
     * have to be copied separately, because they are allocated dynamically
     * and not of known size in the primitve structure:
     * First copy primitive with SDU, but only sdu.l_buf/o_buf and first octet
     * of sdu.buf.
     * Then memcpy actual SDU content with size defined in sdu.l_buf.
     */
    *ll_unitdata_req = *frame;
    memcpy (&(ll_unitdata_req->sdu.buf[ll_unitdata_req->sdu.o_buf/8]), 
      &(frame->sdu.buf[frame->sdu.o_buf/8]), frame->sdu.l_buf/8);

    /*
     * Store the given frame address, RLC/MAC cause, and the originator 
     * for this timer.
     */
    llc_data->t200->frame       = ll_unitdata_req;
    llc_data->t200->cause       = cause;
    llc_data->t200->originator  = originator;
  }

  return;
#endif /* LL_DESC */
} /* t200_start() */


/*
+------------------------------------------------------------------------------
| Function    : t200_stop
+------------------------------------------------------------------------------
| Description : This procedure stops (resets) timer T200 for the current sapi. 
|               The associated frame address and the originator of T200 are 
|               discarded (i.e. the allocated primitive of the frame is freed).
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void t200_stop (void)
{ 
  TRACE_FUNCTION( "t200_stop" );

  /*
   * Stop timer T200 for the current SAPI.
   */
  vsi_t_stop (VSI_CALLER
    (USHORT)(TIMER_T200_BASE + UIMAP(llc_data->current_sapi)));

  /*
   * Free stored primitive.
   */
  if (llc_data->t200->frame)
  {
#ifdef LL_DESC
    /*
     * The descriptor contents of the primitive structure
     * must be freed as well.
     */
    llc_cl_desc3_free((T_desc3*)llc_data->t200->frame->desc_list3.first);
#endif /* LL_DESC */
    PFREE (llc_data->t200->frame);
  }

  /*
   * Delete timer data of the timer for the given SAPI.
   */
  llc_data->t200->frame       = NULL;
  llc_data->t200->originator  = NO_SERVICE;
  llc_data->t200->cause       = 0;

  return;
} /* t200_stop() */


/*
+------------------------------------------------------------------------------
| Function    : t200_get_timer_data
+------------------------------------------------------------------------------
| Description : This procedure returns frame address, frame cause, and the
|               originator identifier for timer T200 of the current sapi. 
|               (see procedure t200_start)
|
| Parameters  : frame       - will be set to the frame pointer which is stored 
|                             for the timer, must be a valid pointer
|               cause       - will be set to the RLC/MAC frame cause
|               originator  - will be set to the originating service for the
|                             timer, must be a valid pointer
|
+------------------------------------------------------------------------------
*/
GLOBAL void t200_get_timer_data 
(
#ifdef LL_DESC 
  T_LL_UNITDESC_REQ **frame, 
#else
  T_LL_UNITDATA_REQ **frame, 
#endif
                                 UBYTE *cause,
                                 T_SERVICE *originator)
{
  TRACE_FUNCTION( "t200_get_timer_data" );
  
  /*
   * Get timer data of the timer for the given SAPI.
   */
  *frame      = llc_data->t200->frame;
  *cause      = llc_data->t200->cause;
  *originator = llc_data->t200->originator;

  return;
} /* t200_get_timer_data() */

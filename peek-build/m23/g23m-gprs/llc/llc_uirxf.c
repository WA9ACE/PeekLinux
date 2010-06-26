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
|             SDL-documentation (UIRX-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#ifndef LLC_UIRXF_C
#define LLC_UIRXF_C
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

#include "llc_uirxf.h"  /* to compare own prototyes */
/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/


/*
+------------------------------------------------------------------------------
| Function    : uirx_queue_clean 
+------------------------------------------------------------------------------
| Description : This procedure removes all entries from the UIRX queue
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void uirx_queue_clean (void)
{
  T_UIRX_QUEUE** entry = &(llc_data->uirx->queue);
  
  TRACE_FUNCTION( "uirx_queue_clean" );

  while (*entry)
  {
    /*
     * get pointer to next (=first) entry
     */
    T_UIRX_QUEUE* current = *entry;

    /*
     * Free frame, if one is attached to the entry
     */
    if (current->frame != NULL)
    {
      PFREE (current->frame);
    }

    /*
     * remove next entry from the entry (make second to first)
     */
    *entry = current->next;
    
    /*
     * free the removed entry
     */
    MFREE (current);
  }

} /* uirx_queue_clean() */

/*
+------------------------------------------------------------------------------
| Function    : uirx_queue_store
+------------------------------------------------------------------------------
| Description : This procedure stores the primitive to the end of the queue.
|               If the queue is full, an the primitive is freed.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void uirx_queue_store (T_LL_UNITDATA_IND *ll_unitdata_ind)
{
  T_UIRX_QUEUE** entry       = &(llc_data->uirx->queue);
  int            num_entries = 0;

  TRACE_FUNCTION( "uirx_queue_store" );

  /*
   * Skip, but count, already for L3 ready waiting entries.
   * If queue is already full, free this primitive.
   */
  while (*entry)
  {
    num_entries++;
    
    if (num_entries >= UIRX_QUEUE_SIZE)
    {
      PFREE (ll_unitdata_ind);
      TRACE_EVENT ("LL_UNITDATA_IND ignored -> UIRX queue is full");
      return;
    }

    entry = &((*entry)->next);
  }

  /*
   * Allocate management memory
   */
  MALLOC( *entry, sizeof(T_UIRX_QUEUE) );

  if (*entry)
  {
    /*
     * Queue primitive
     */
    (*entry)->next   = NULL;
    (*entry)->frame  = ll_unitdata_ind;

  }
  else
  {
    /*
     * Out of memory
     */
    PFREE (ll_unitdata_ind);
    TRACE_ERROR( "Out of memory in uirx_queue_store()" );
  }

} /* uirx_queue_store() */


/*
+------------------------------------------------------------------------------
| Function    : uirx_queue_retrieve
+------------------------------------------------------------------------------
| Description : Retrieve the next frame, if any. 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL uirx_queue_retrieve (T_LL_UNITDATA_IND **ll_unitdata_ind)
{
  T_UIRX_QUEUE** entry = &(llc_data->uirx->queue);

  TRACE_FUNCTION( "uirx_queue_retrieve" );

  /*
   * Take the first queue entry, if there is any.
   */
  if (*entry)
  {
    /*
     * Store pointer to the entry
     */
    T_UIRX_QUEUE* current = *entry;

    /*
     * Remove entry from the queue (make second to first)
     */
    *entry = current->next;

    *ll_unitdata_ind = current->frame;
  
    /*
     * Free retrieved management entry
     */
    MFREE (current);

    return TRUE;
  }
  else
  {
    /*
     * Set default return values
     */
    *ll_unitdata_ind = NULL;

    return FALSE;
  }

} /* uirx_queue_retrieve() */

/*
+------------------------------------------------------------------------------
| Function    : uirx_init
+------------------------------------------------------------------------------
| Description : This procedure initialises all necessary variables of
|               ui_frames_rx for all SAPIs.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void uirx_init (void)
{
  UBYTE inc;

  TRACE_FUNCTION( "uirx_init" );
  
  /*
   * Initialise every incarnation of UIRX with state TLLI_UNASSIGNED_NOT_READY,
   * except incarnation 0 (= SAPI 1), because we have no flow control for 
   * SAPI 1.
   */
  SWITCH_SERVICE (llc, uirx, 0);
  INIT_STATE (UIRX_0, UIRX_TLLI_UNASSIGNED_READY);

  SWITCH_SERVICE (llc, uirx, 1);
  INIT_STATE (UIRX_1, UIRX_TLLI_UNASSIGNED_NOT_READY);
  
  SWITCH_SERVICE (llc, uirx, 2);
  INIT_STATE (UIRX_2, UIRX_TLLI_UNASSIGNED_NOT_READY);
  
  SWITCH_SERVICE (llc, uirx, 3);
  INIT_STATE (UIRX_3, UIRX_TLLI_UNASSIGNED_NOT_READY);
  
  SWITCH_SERVICE (llc, uirx, 4);
  INIT_STATE (UIRX_4, UIRX_TLLI_UNASSIGNED_NOT_READY);
  
  SWITCH_SERVICE (llc, uirx, 5);
  INIT_STATE (UIRX_5, UIRX_TLLI_UNASSIGNED_NOT_READY);

  /*
   * Initialise the UIRX queue structure
   */
  for (inc = 0; inc < UIRX_NUM_INC; inc++)
  {
    SWITCH_SERVICE (llc, uirx, inc);

    /*
     * Free old used resources (in case of an LLC restart):
     */
    uirx_queue_clean ();
    llc_data->uirx->queue = NULL;
  }

  return;
} /* uirx_init() */


/*
+------------------------------------------------------------------------------
| Function    : uirx_init_sapi
+------------------------------------------------------------------------------
| Description : This procedure initialises all necessary variables of
|               ui_frames_rx for the given SAPI.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void uirx_init_sapi (void)
{ 
  TRACE_FUNCTION( "uirx_init_sapi" );
  
  /*
   * <R.LLC.TLLI_ASS.A.007>, <R.LLC.TLLI_ASS.A.009>
   */
  llc_data->sapi->vur = 0;

  llc_data->uirx->last_32_frames_bitfield = 0L;

  return;
} /* uirx_init_sapi() */



/*
+------------------------------------------------------------------------------
| Function    : uirx_store_nu
+------------------------------------------------------------------------------
| Description : This procedure stores the given n(u) in a list of the last 32 
|               received n(u)s concerning the given SAPI. Therefore the 
|               variable v(ur) is passed to the procedure, which contains 
|               the current value of the unacknowledged receive sequence 
|               number.
|
| Parameters  : nu  - N(U), must be in the range vur-32 <= nu < vur
|               vur - V(UR)
|
+------------------------------------------------------------------------------
*/
GLOBAL void uirx_store_nu (T_FRAME_NUM nu, T_FRAME_NUM vur)
{ 
  USHORT            diff;


  TRACE_FUNCTION( "uirx_store_nu" );
  
  /*
   * Set the bit for the received N(U) in last_32_frames_bitfield. The 
   * relative bit position referring to V(UR) has already been stored 
   * in diff.
   */
  diff = (vur + (MAX_SEQUENCE_NUMBER+1) - nu - 1) % (MAX_SEQUENCE_NUMBER+1);

  llc_data->uirx->last_32_frames_bitfield |= (0x00000001L) << diff;

  return;
} /* uirx_store_nu() */



/*
+------------------------------------------------------------------------------
| Function    : uirx_check_nu
+------------------------------------------------------------------------------
| Description : This procedure checks if the given n(u) has already been 
|               received for the current SAPI within the last 32 frames (see 
|               procedure store_nu).
|
| Parameters  : nu    - N(U), must be in the range vur-32 <= nu < vur
|               vur   - V(UR)
|
| Returns     : TRUE  - if frame was already received.
|               FALSE - else
+------------------------------------------------------------------------------
*/
GLOBAL BOOL uirx_check_nu (T_FRAME_NUM nu, T_FRAME_NUM vur)
{ 
  USHORT diff;


  TRACE_FUNCTION( "uirx_check_nu" );
  
  /*
   * The difference (modulo counted) between V(UR) and N(U) is 
   * calculated as follows:
   * - add 512 (maximum sequence number + 1) to V(UR), in case V(UR)
   *   is smaller than N(U)
   * - subtract N(U)
   * - subtract 1 because V(UR) is not contained within 
   *   last_32_frames_bitfield and thus the bit shift must be one less
   * - afterwards apply modulo 512 to get back into the range of 
   *   sequence numbers, because we added this value initially to
   *   V(UR)
   */
  diff = (vur + (MAX_SEQUENCE_NUMBER+1) - nu - 1) % (MAX_SEQUENCE_NUMBER + 1);

  /*
   * Check if the corresponding bit of N(U) in last_32_frames_bitfield is 
   * set. The bit is stored in last_32_frames_bitfield relative to V(UR).
   */
  return ((llc_data->uirx->last_32_frames_bitfield & (0x00000001L << diff)) != 0L);
} /* uirx_check_nu() */


/*
+------------------------------------------------------------------------------
| Function    : uirx_set_new_vur
+------------------------------------------------------------------------------
| Description : This procedure handles setting vur to a new value. The modulo
|               operation and the handling of the last_32_frames_bitfield
|               is done here.
|
| Parameters  : new_vur - V(UR), must be in range V(UR) < new_vur < V(UR)-32
|
+------------------------------------------------------------------------------
*/
GLOBAL void uirx_set_new_vur (T_FRAME_NUM new_vur)
{ 
  USHORT diff;


  TRACE_FUNCTION( "uirx_set_new_vur" );

  new_vur %= (MAX_SEQUENCE_NUMBER+1);
  
  /*
   * The difference (modulo counted) between V(UR) and the new_vur is 
   * calculated as follows:
   * - add 512 (maximum sequence number + 1) to new_vur
   * - subtract V(UR)
   * - afterwards apply modulo 512 to get back into the range of 
   *   sequence numbers, because we added this value initially
   */
  diff = (new_vur + (MAX_SEQUENCE_NUMBER+1) - llc_data->sapi->vur) 
           % (MAX_SEQUENCE_NUMBER+1);

  /*
   * Now move the bitfield indicating the last 32 frames numbers. If the diff
   * from current to new_vur is to large, the bitfild is automaticly cleard
   * by shifting all bits out.
   */
  llc_data->uirx->last_32_frames_bitfield <<= diff;

  /*
   * Increase oc if necessary. If new_vur is a higher value, no overflow
   * encountered. If new_vur is lower, an retransmission could be possible
   * or we have had an overflow. But this function will not be called in 
   * case of a retransmission!
   */
  if ( new_vur < llc_data->sapi->vur )
  {
    llc_data->sapi->oc_ui_rx += (MAX_SEQUENCE_NUMBER+1);
  }

  /*
   * Set net V(UR) value
   */
  llc_data->sapi->vur = new_vur;

} /* uirx_set_new_vur() */


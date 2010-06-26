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
|             SDL-documentation (IRX-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#ifndef LLC_IRXF_C
#define LLC_IRXF_C
#endif

#define ENTITY_LLC

/*==== INCLUDES =============================================================*/

#include <string.h>

#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "cnf_llc.h"    /* to get cnf-definitions */
#include "mon_llc.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "llc.h"        /* to get the global entity definitions */

#include "llc_itxs.h"   /* to get ITX signal definitions */
#include "llc_irxf.h"   /* to get IRX local functions */


/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : irx_init
+------------------------------------------------------------------------------
| Description : This procedure initialises all necessary variables of
|               i_frames_rx for all SAPIs.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void irx_init (void)
{
  UBYTE inc;

  TRACE_FUNCTION( "irx_init" );

  /*
   * Initialise all 4 SAPIs
   */
  SWITCH_SERVICE (llc, irx, 0);
  INIT_STATE (IRX_0, IRX_TLLI_UNASSIGNED);

  SWITCH_SERVICE (llc, irx, 1);
  INIT_STATE (IRX_1, IRX_TLLI_UNASSIGNED);

  SWITCH_SERVICE (llc, irx, 2);
  INIT_STATE (IRX_2, IRX_TLLI_UNASSIGNED);

  SWITCH_SERVICE (llc, irx, 3);
  INIT_STATE (IRX_3, IRX_TLLI_UNASSIGNED);

  /*
   * Initialise the IRX data structure
   */
  for (inc = 0; inc < IRX_NUM_INC; inc++)
  {
    SWITCH_SERVICE (llc, irx, inc);

    /*
     * Free old used resources (in case of an LLC restart):
     * memory, stored primitives, running timer.
     */
    irx_queue_clean ();

    llc_data->irx->ll_send_ready = FALSE;
    llc_data->irx->last_ns = NS_EQUAL_VR;

    llc_data->irx->queue = NULL;
  }
  
  return;
} /* irx_init() */


/*
+------------------------------------------------------------------------------
| Function    : irx_init_sapi
+------------------------------------------------------------------------------
| Description : This procedure initialises all necessary variables of
|               i_frames_rx for the given SAPI.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void irx_init_sapi (void)
{ 
  TRACE_FUNCTION( "irx_init_sapi" );

  llc_data->sapi->va = 0;
  llc_data->sapi->vs = 0;
  llc_data->sapi->vr = 0;

  llc_data->irx->vf      = 0;
  llc_data->irx->last_ns = NS_EQUAL_VR;
  llc_data->irx->B_rx    = 0;

  return;
} /* irx_init_sapi() */


/*
+------------------------------------------------------------------------------
| Function    : irx_init_abm
+------------------------------------------------------------------------------
| Description : This procedure initialises all necessary variables of
|               i_frames_rx for the given SAPI when switching into ABM.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void irx_init_abm (void)
{ 
  TRACE_FUNCTION( "irx_init_abm" );
  
  llc_data->sapi->va = 0;
  llc_data->sapi->vs = 0;
  llc_data->sapi->vr = 0;

  llc_data->irx->vf      = 0;
  llc_data->irx->last_ns = NS_EQUAL_VR;

  /*
   * Reset OCs for acknowledged transfer.
   */
  llc_data->sapi->oc_i_tx = 0L;
  llc_data->sapi->oc_i_rx = 0L;

  return;
} /* irx_init_abm() */



/*
+------------------------------------------------------------------------------
| Function    : irx_queue_store
+------------------------------------------------------------------------------
| Description : This procedure stores the primitive into the per ns sorted 
|               queue. If the queue is full, is_busy set to TRUE will be 
|               returnd.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void irx_queue_store (T_LL_UNITDATA_IND *ll_unitdata_ind,
                             T_FRAME_NUM       ns,
                             BOOL              *is_busy)
{
  T_IRX_QUEUE** entry = &(llc_data->irx->queue);
  T_IRX_QUEUE*  next;
  T_FRAME_NUM   n     = llc_data->irx->vf;
  ULONG         M     = *(llc_data->md) * 16;

  TRACE_FUNCTION( "irx_queue_store" );

  /*
   * First skip already for L3 ready waiting entries
   */
  while (*entry && (*entry)->ns == n)
  {
    n++;
    n %= (MAX_SEQUENCE_NUMBER+1);

    if ((*entry)->ns == ns)
    {
      /*
       * Ignore duplicate frame
       */
      *is_busy = FALSE;

      TRACE_0_INFO( "Unexpected duplicate frame number" );
      PFREE (ll_unitdata_ind);

      return;
    }

    entry = &((*entry)->next);
  }

  /*
   * Now find the inserting position (queue sorted per ns, 
   * 'lower' frame numbers stored first)
   */
  while (*entry)
  {
    if ((*entry)->ns == ns)
    {
      /*
       * Ignore duplicate frame
       */
      *is_busy = FALSE;

      TRACE_0_INFO( "Duplicate frame number received" );
      PFREE (ll_unitdata_ind);

      return;
    }
    
    /*
     * n <= ns <= (*entry)->ns - 1
     */
    if (FRAME_NUM_VALID( n, ns, (*entry)->ns - 1))
    {
      /*
       * Found a nice place in between
       */
      break;
    }

    entry = &((*entry)->next);
  }

  /*
   * Position found - save ptr to next entry
   */
  next = *entry;

  /*
   * Allocate memory
   */
  MALLOC( *entry, sizeof(T_IRX_QUEUE) );

  if( *entry )
  {
    /*
     * Memory successful allocated. Fill in struct entries.
     */
    (*entry)->next   = next;
    (*entry)->ns     = ns;
    (*entry)->frame  = ll_unitdata_ind;

    /*
     * Increase amount of stored Information in rx queue
     */
    llc_data->irx->B_rx += BYTELEN(ll_unitdata_ind->sdu.l_buf);

    /*
     * Determine 'own receiver busy' condition
     */
    if ((M == 0) || (*(llc_data->n201_i) <= M - llc_data->irx->B_rx))
    {
      *is_busy = FALSE;
    }
    else
    {
      *is_busy = TRUE;
    }
  }
  else
  {
    /*
     * Out of memory
     */
    *is_busy = TRUE;
    TRACE_ERROR( "Out of memory in irx_queue_store()" );
    PFREE (ll_unitdata_ind);
  }

} /* irx_queue_store() */


/* 
+------------------------------------------------------------------------------
| Function    : irx_update_vr
+------------------------------------------------------------------------------
| Description : This procedure increments V(R) if the next following frames
|               are already stored in the queue.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void irx_update_vr (void)
{
  T_IRX_QUEUE* entry = llc_data->irx->queue;

  TRACE_FUNCTION( "irx_update_vr" );

  /*
   * Search the queue from the beginning to entry V(R)
   */
  while (entry)
  {
    if (entry->ns == llc_data->sapi->vr)
    {
      /*
       * Increment V(R) and continue searching
       */
      llc_data->sapi->vr++;

      if (llc_data->sapi->vr > MAX_SEQUENCE_NUMBER)
      {
        llc_data->sapi->oc_i_rx += (MAX_SEQUENCE_NUMBER+1);
        llc_data->sapi->vr       = 0;
      }
    }
    
    entry = entry->next;
  }

} /* irx_update_vr() */


/* 
+------------------------------------------------------------------------------
| Function    : irx_get_last_queued_ns
+------------------------------------------------------------------------------
| Description : This procedure returns the frame number of the last queue entry
|
| Parameters  : *found - set to TRUE if ns found, FALSE otherwise
|
+------------------------------------------------------------------------------
*/
GLOBAL void irx_get_last_queued_ns (BOOL *found, T_FRAME_NUM *num)
{
  T_IRX_QUEUE* entry = llc_data->irx->queue;

  TRACE_FUNCTION( "irx_get_last_queued_ns" );

  /*
   * Search the queue from the beginning to last entry
   */
  while (entry AND entry->next)
    entry = entry->next;

  if (entry)
  {
    *found = TRUE;
    *num   = entry->ns;
  }
  else
  {
    *found = FALSE;
    *num   = 0;
  } 

} /* irx_get_last_queued_ns() */


/*
+------------------------------------------------------------------------------
| Function    : irx_queue_retrieve
+------------------------------------------------------------------------------
| Description : If the next frame in sequence is stored in the queue then this 
|               procedure will return the primitive pointer, sets found to 
|               TRUE and removes the frame from the queue.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void irx_queue_retrieve (T_LL_UNITDATA_IND **ll_unitdata_ind, BOOL *found)
{
  T_IRX_QUEUE** entry = &(llc_data->irx->queue);

  TRACE_FUNCTION( "irx_queue_retrieve" );

  /*
   * Take the first queue entry, if this is the next one (queue is sorted per ns)
   */
  if ((*entry != NULL) && ((*entry)->ns == llc_data->irx->vf))
  {
    /*
     * Store pointer to the entry
     */
    T_IRX_QUEUE* current = *entry;

    /*
     * Remove entry from the queue (make second to first)
     */
    *entry = current->next;

    *ll_unitdata_ind = current->frame;
  
    /*
     * Decrease amount of stored Information in rx queue
     */
    if (llc_data->irx->B_rx >= (ULONG)(BYTELEN((*ll_unitdata_ind)->sdu.l_buf)))
    {
      llc_data->irx->B_rx -= BYTELEN((*ll_unitdata_ind)->sdu.l_buf);
    }
    else
    {
      llc_data->irx->B_rx = 0;
      TRACE_0_INFO("Unexpected SDU lenght handled");
    }

    /*
     * Free retrieved entry
     */
    MFREE (current);

    *found = TRUE;

    /*
     * Increment V(f) (= Next frame number to forward to L3)
     */
    llc_data->irx->vf += 1;
    llc_data->irx->vf %= (MAX_SEQUENCE_NUMBER+1);

    return;
  }

  /*
   * Set default return values
   */
  *ll_unitdata_ind = NULL;
  *found           = FALSE;
}



/*
+------------------------------------------------------------------------------
| Function    : irx_queue_clean 
+------------------------------------------------------------------------------
| Description : This procedure removes all entries from the IRX queue
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void irx_queue_clean (void)
{
  T_IRX_QUEUE** entry = &(llc_data->irx->queue);
  
  TRACE_FUNCTION( "irx_queue_clean" );

  while (*entry)
  {
    /*
     * get pointer to next (=first) entry
     */
    T_IRX_QUEUE* current = *entry;

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

  /*
   * Adjust amount of stored Information in rx queue
   */
  llc_data->irx->B_rx = 0;

} /* irx_queue_clean() */



/*
+------------------------------------------------------------------------------
| Function    : irx_build_sack_bitmap
+------------------------------------------------------------------------------
| Description : This procedure builds a SACK bitmap, depending on current
|               status
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void irx_build_sack_bitmap ( T_SACK_BITMAP *bitmap )
{
  T_IRX_QUEUE* entry    = llc_data->irx->queue;
  int          start_ns = (llc_data->sapi->vr + 1) % (MAX_SEQUENCE_NUMBER+1);
  T_FRAME_NUM  num      = llc_data->irx->vf;
  USHORT       d;


  TRACE_FUNCTION( "irx_build_sack_bitmap" );

  /*
   * Init bitmap to 0 (= frame not successfully received)
   */  
  memset (bitmap, 0, sizeof(T_SACK_BITMAP));

  /*
   * Skip already for L3 ready waiting entries
   */
  while (entry && entry->ns == num)
  {
    num++;
    num %= (MAX_SEQUENCE_NUMBER+1);

    entry = entry->next;
  }


  /*
   * Check the queue for successful received entries
   */
  while (entry)
  {
    /*
     * If frame is successful received, mark it in bitmap
     */
    d = FRAME_NUM_DISTANCE(start_ns, entry->ns);

    if (d < (S_FRAME_SACK_MAX_CTRL_OCTETS * 8))
    {
      bitmap->data[d>>3] |= 0x80>>(d%8);
    }

    entry = entry->next;
  }
} /* irx_build_sack_bitmap () */


/*
+------------------------------------------------------------------------------
| Function    : irx_ack_all_to
+------------------------------------------------------------------------------
| Description : This procedure handles the acknowledgement of transmitted I- 
|               frames from va to n (which is equal to nr -1)
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void irx_ack_all_to (T_FRAME_NUM n)
{
  TRACE_FUNCTION( "irx_ack_all_to" );

  n %= (MAX_SEQUENCE_NUMBER+1);
   
  if (FRAME_WIN_VALID(n, llc_data->sapi->va, *(llc_data->ku)))
  {
    while (llc_data->sapi->va != (n+1)%(MAX_SEQUENCE_NUMBER+1))
    {
      sig_irx_itx_ack_ind (TRUE, llc_data->sapi->va);

      llc_data->sapi->va++;
      llc_data->sapi->va %= (MAX_SEQUENCE_NUMBER+1);
    } 

    /*
     * Sending L3 Data confirmations
     */
    sig_irx_itx_cnf_l3data_req ();
  }

} /* irx_ack_all_to () */


/*
+------------------------------------------------------------------------------
| Function    : irx_handle_sack
+------------------------------------------------------------------------------
| Description : This procedure handles the acknowledge bits of a SACK 
|               supervisory frame and strips the SACK bitmap from the SDU
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void irx_handle_sack (T_FRAME_NUM       nr, 
                             T_LL_UNITDATA_IND *ll_unitdata_ind,
                             T_PDU_TYPE        frame_type)
{
  int            i, n, bytes;
  T_FRAME_NUM    rn = 0;
  T_SACK_BITMAP  *bm;
  
  TRACE_FUNCTION( "irx_handle_sack" );

  /*
   * Extract SACK bitmap and number of Ptr to bits from sdu:
   */
  if (frame_type == I_FRAME)
  {
    bm    = (T_SACK_BITMAP*)&(ll_unitdata_ind->sdu.buf[ 
               (ll_unitdata_ind->sdu.o_buf >> 3) + 1]);
    bytes = ll_unitdata_ind->sdu.buf[ 
               (ll_unitdata_ind->sdu.o_buf >> 3)] & 0xF1;
  }
  else
  {
    bm    = (T_SACK_BITMAP*)&(ll_unitdata_ind->sdu.buf[ 
               (ll_unitdata_ind->sdu.o_buf >> 3)]);
    bytes = ll_unitdata_ind->sdu.l_buf >> 3;
  }

  /*
   * Little Protection
   */
  if ( bytes > S_FRAME_SACK_MAX_CTRL_OCTETS )
  {
    bytes = S_FRAME_SACK_MAX_CTRL_OCTETS;
    TRACE_ERROR ("Illegal SACK bitmap size received");
  }

  /*
   * For each bit, which is set, send an acknowledge to ITX
   */
  for (i = 0; i < bytes; i++)
  {
    for (n = 7; n >= 0; n--)
    {
      /*
       * first increment rn (must begin with 1)
       */
      rn++;

      /*
       * check if bit is set from MSB to LSB
       */
      if ( bm->data[i] & (1 << n) )
      {
        /*
         * set frame acknowledged
         */
        sig_irx_itx_ack_ind ( TRUE, (T_FRAME_NUM)(nr + rn));
      }
      else
      {
        /*
         * mark frame for retransmission
         */
        sig_irx_itx_ack_ind ( FALSE, (T_FRAME_NUM)(nr + rn));
      }
    }
  }

  /*
   * send L3 data confirmation
   */
  sig_irx_itx_cnf_l3data_req ();

  /*
   * Strip SACK bitmap from I frame SDUs to get clean L3 PDU
   */
  if (frame_type == I_FRAME)
  {
    ll_unitdata_ind->sdu.o_buf += bytes << 3;
    ll_unitdata_ind->sdu.l_buf -= bytes << 3;
  }

} /* irx_handle_sack () */


/*
+------------------------------------------------------------------------------
| Function    : irx_send_ack
+------------------------------------------------------------------------------
| Description : This procedure handles the detection of sequence errors and
|               the sending of appropriate acknowledgements
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void irx_send_ack (T_BIT a_bit)
{
  TRACE_FUNCTION( "irx_send_ack" );

  /*
   * Send S frame depending on last N(S) / V(R) condition
   */
  switch (llc_data->irx->last_ns)
  {
    case NS_EQUAL_VR:
      /*
       * Send an RR only, if requested
       */
      if (a_bit)
      {
        sig_irx_itx_send_rr_req (ABIT_NO_REQ);
      }
      else
      {
        /*
         * Trigger ITX to check queue and send frames
         * This is done inside the other signals, too.
         */
        sig_irx_itx_trigger_ind ();
      }
      break;
    
    case NS_EQUAL_VR_PLUS_1:
      /*
       * Sequence error - one missing
       */
      sig_irx_itx_send_ack_req (ABIT_NO_REQ);
      break;
    
    case NS_NO_SEQUENCE_ERROR:
      /*
       * Send an SACK only, if requested
       */
      if (a_bit)
      {
        T_SACK_BITMAP bitmap;

        irx_build_sack_bitmap( &bitmap );
        sig_irx_itx_send_sack_req (ABIT_NO_REQ, &bitmap);
      }
      else
      {
        /*
         * Trigger ITX to check queue and send frames
         * This is done inside the other signals, too.
         */
        sig_irx_itx_trigger_ind ();
      }
      break;

    default:
      /*
       * Sequence error - one or more missing
       */
      {
        T_SACK_BITMAP bitmap;

        irx_build_sack_bitmap( &bitmap );
        sig_irx_itx_send_sack_req (ABIT_NO_REQ, &bitmap);
      }
      break;
  }

} /* irx_send_ack () */


/*
+------------------------------------------------------------------------------
| Function    : irx_send_rnr
+------------------------------------------------------------------------------
| Description : This procedure sends an RNR to ITX
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void irx_send_rnr ()
{
  TRACE_FUNCTION( "irx_send_rnr" );

  sig_irx_itx_send_rnr_req (ABIT_NO_REQ);

} /* irx_send_rnr() */



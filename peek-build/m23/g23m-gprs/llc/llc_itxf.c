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
|             SDL-documentation (ITX-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#ifndef LLC_ITXF_C
#define LLC_ITXF_C
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
#include "llc_f.h"        /* to get the global entity definitions */
#include "llc_txs.h"    /* to get signal interface to TX */
#include "llc_itxt.h"   /* to get signal interface to TX */
#include "llc_llmes.h"  /* to get signal interface to LLME */
#include "llc_itxf.h"   /* to get ITX local functions */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/
LOCAL void itx_send_i_frame (T_ITX_I_QUEUE_ENTRY*  entry,
                                 T_ABIT_REQ_TYPE       tr);

LOCAL void itx_i_queue_get_next (T_IQ_STATUS*          status, 
                                 T_ITX_I_QUEUE_ENTRY** entry,
                                 T_ABIT_REQ_TYPE*      rt);

LOCAL void itx_i_queue_get_retr (T_IQ_STATUS*          status, 
                                 T_ITX_I_QUEUE_ENTRY** entry,
                                 T_ABIT_REQ_TYPE*      rt);

LOCAL void itx_s_queue_retrieve (BOOL*                 found,
                                 T_COMMAND*            sx,
                                 T_ABIT_REQ_TYPE*      rt, 
                                 T_FRAME_NUM*          nr,
                                 T_SACK_BITMAP*        bm, 
                                 USHORT*               bm_byte_size);

LOCAL void itx_send_s_frame     (T_COMMAND             sx,
                                 T_ABIT_REQ_TYPE       rt, 
                                 T_FRAME_NUM           nr,
                                 T_SACK_BITMAP*        bm,
                                 USHORT                bm_byte_size);


/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : itx_init
+------------------------------------------------------------------------------
| Description : This procedure initialises all necessary variables of
|               i_frames_tx for all SAPIs.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void itx_init (void)
{ 
  UBYTE inc;

  TRACE_FUNCTION( "itx_init" );

  /*
   * Initialise all 4 SAPIs with state TLLI_UNASSIGNED.
   */
  SWITCH_SERVICE (llc, itx, 0);
  INIT_STATE (ITX_0, ITX_TLLI_UNASSIGNED);

  SWITCH_SERVICE (llc, itx, 1);
  INIT_STATE (ITX_1, ITX_TLLI_UNASSIGNED);
  
  SWITCH_SERVICE (llc, itx, 2);
  INIT_STATE (ITX_2, ITX_TLLI_UNASSIGNED);
  
  SWITCH_SERVICE (llc, itx, 3);
  INIT_STATE (ITX_3, ITX_TLLI_UNASSIGNED);

  /*
   * Initialise the ITX data structure
   */
  for (inc = 0; inc < ITX_NUM_INC; inc++)
  {
    SWITCH_SERVICE (llc, itx, inc);

    /*
     * Free old used resources (in case of an LLC restart):
     * memory, stored primitives, running timer.
     */
    itx_i_queue_clean();
    itx_s_queue_clean();

    /*
     * Init data area
     */
    llc_data->itx->tx_waiting      = FALSE;
    llc_data->itx->buffer_was_full = FALSE;

    llc_data->itx->i_queue.first   = NULL;
    llc_data->itx->s_queue         = NULL;

    llc_data->itx->n_pb_retr       = 0;
  }

  return;
} /* itx_init() */


/*
+------------------------------------------------------------------------------
| Function    : itx_init_sapi
+------------------------------------------------------------------------------
| Description : This procedure initialises all necessary variables of
|               i_frames_tx for the current SAPI.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void itx_init_sapi (void)
{ 
  TRACE_FUNCTION( "itx_init_sapi" );

  llc_data->itx->buffer_was_full = FALSE;

  llc_data->itx->t201_entry = NULL;
  llc_data->itx->n_pb_retr  = 0;
  llc_data->itx->B_tx       = 0;

  return;
} /* itx_init_sapi() */


/*
+------------------------------------------------------------------------------
| Function    : itx_queue_sequence_check
+------------------------------------------------------------------------------
| Description : This procedure checks for the A bit request conditions "end of
|               a sequence of transmitted I frames"
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void itx_queue_sequence_check (T_IQ_STATUS          status,
                                      T_ITX_I_QUEUE_ENTRY* entry,
                                      T_ABIT_REQ_TYPE*     rt)
{
  TRACE_FUNCTION( "itx_queue_sequence_check" );

  /*
   * Do not overwrite rt, if it is already set
   */
  if (*rt == ABIT_SET_REQ )
  {
    return;
  }

  /*
   * Handle rt depending on status
   */
  switch (status)
  {
    case IQ_NEW_FRAME:
      /*
       * If a next frame is not available, the sequence ends
       */
      if (entry == NULL)
      {
        *rt = ABIT_SET_REQ;
      }
      break;

    case IQ_RETR_FRAME:
      /*
       * If no frame to retransmit is following, the sequence ends
       */
      while (entry)
      {
        if (entry->status == IQ_RETR_FRAME)
        {
          /*
           * Another one is following
           */
          return;
        }
        entry = entry->next;
      }
      *rt = ABIT_SET_REQ;
      break;

    default:
     TRACE_ERROR("Status unexpected");
     break;
  }
}



/*
+------------------------------------------------------------------------------
| Function    : itx_i_queue_get_next
+------------------------------------------------------------------------------
| Description : This procedure gets the next new frame from the I frame queue.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
LOCAL void itx_i_queue_get_next (T_IQ_STATUS*          status,
                                 T_ITX_I_QUEUE_ENTRY** entry,
                                 T_ABIT_REQ_TYPE*      rt )
{
  T_ITX_I_QUEUE_ENTRY* iq_ptr;

  TRACE_FUNCTION( "itx_i_queue_get_next" );

  /*
   * Search for a frame with type equal status
   */
  iq_ptr = llc_data->itx->i_queue.first;
  while (iq_ptr)
  {
    /*
     * check if we have found the entry
     */
    if (iq_ptr->status == IQ_NEW_FRAME)
    {
      *entry  = iq_ptr;
      *status = IQ_NEW_FRAME;
     
      /*
       * check for A bit set condition
       */
      itx_queue_sequence_check (IQ_NEW_FRAME, iq_ptr->next, rt);

      /*
       * next one found
       */
      return;
    }

    iq_ptr = iq_ptr->next;
  }

  /*
   * We have nothing found to transmit
   */
  *entry  = NULL;
  *status = IQ_NO_FRAME;
  *rt     = ABIT_NO_REQ;

} /* itx_i_queue_get_next() */

/*
+------------------------------------------------------------------------------
| Function    : itx_i_queue_get_retr
+------------------------------------------------------------------------------
| Description : This procedure gets the next frame maked for retransmission 
|               from the I frame queue.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
LOCAL void itx_i_queue_get_retr (T_IQ_STATUS*          status,
                                 T_ITX_I_QUEUE_ENTRY** entry,
                                 T_ABIT_REQ_TYPE*      rt )
{
  T_ITX_I_QUEUE_ENTRY* iq_ptr;

  TRACE_FUNCTION( "itx_i_queue_get_retr" );

  /*
   * Search for a frame makred for retransmission
   * Retransmissions can't be stored after new frames
   */
  iq_ptr = llc_data->itx->i_queue.first;
  while (iq_ptr && iq_ptr->status != IQ_NEW_FRAME)
  {
    /*
     * check if we have found the entry
     */
    if (iq_ptr->status == IQ_RETR_FRAME)
    {
      *entry  = iq_ptr;
      *status = IQ_RETR_FRAME;
     
      /*
       * check for A bit set condition
       */
      itx_queue_sequence_check (IQ_RETR_FRAME, iq_ptr->next, rt);

      /*
       * next one found
       */
      return;
    }

    iq_ptr = iq_ptr->next;
  }

  /*
   * We have nothing found to transmit
   */
  *entry  = NULL;
  *status = IQ_NO_FRAME;
  *rt     = ABIT_NO_REQ;

} /* itx_i_queue_get_retr () */


/*
+------------------------------------------------------------------------------
| Function    : itx_send_next_frame
+------------------------------------------------------------------------------
| Description : If LLC is not suspended and TX is ready to receive a frame, 
|               this procedure handles the sending of the next I/S or S frame, 
|               if one is availabel in the I or S queue.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void itx_send_next_frame (T_ABIT_REQ_TYPE req)
{
  T_ITX_I_QUEUE_ENTRY* entry;
  T_IQ_STATUS          status;
  T_ABIT_REQ_TYPE      rt = ABIT_NO_REQ;
  T_SACK_BITMAP        bm;
  T_COMMAND            sx;
  T_FRAME_NUM          nr;
  USHORT               bm_byte_size;
  BOOL                 found;

  TRACE_FUNCTION( "itx_send_next_frame" );

  /*
   * First check, if TX is ready to receive a frame
   */
  if ((llc_data->itx->tx_waiting == FALSE) || (llc_data->suspended == TRUE))
  {
    /*
     * Label ITX_SEND_1
     */

    return;
  }

  /*
   * Try to get a frame marked for retransmission from the I queue
   */
  itx_i_queue_get_retr (&status, &entry, &rt);

  /*
   * If we found none, try to get a new frame
   */
  if (status == IQ_NO_FRAME )
  {
    /*
     * Try it only, if the transmit window is not full => V(S) < V(A) + k
     */
    if ( FRAME_WIN_VALID(llc_data->sapi->vs, llc_data->sapi->va, *(llc_data->ku)) )
    {
      itx_i_queue_get_next (&status, &entry, &rt);
    }
  }

  switch (status)
  {
    case IQ_NO_FRAME:
      /*
       * Label ITX_SEND_2
       */

      /*
       * No I frame to send, look for a S frame
       */
      itx_s_queue_retrieve (&found, &sx, &rt, &nr, &bm, &bm_byte_size);
      /*
       * If an command was found, send it, else leave
       */
      if (found)
      {
        itx_send_s_frame (sx, rt, nr, &bm, bm_byte_size);
      }
      break;

    case IQ_NEW_FRAME:
      /*
       * Set A-bit flag in case of V(S) = V(A) + k after sending
       */
      if( llc_data->sapi->vs == (llc_data->sapi->va + 
                               *(llc_data->ku) - 1) % (MAX_SEQUENCE_NUMBER+1) )
      {
        rt = ABIT_SET_REQ;
      }
      
      /*
       * Set current value of V(S) and OC to the next frame and
       * increment it. Must be done before calling send_i_frame
       * because of possible recursion!
       */
      entry->ns      = llc_data->sapi->vs++;
      entry->oc_i_tx = llc_data->sapi->oc_i_tx;

      /*
       * Handle OC(tx) and MAX_SEQUENCE_NUMBER for acknowledged transfer.
       */
      if (llc_data->sapi->vs > MAX_SEQUENCE_NUMBER)
      {
        llc_data->sapi->vs       = 0;
        llc_data->sapi->oc_i_tx += (MAX_SEQUENCE_NUMBER+1);
      }

      /*
       * Transmit I frame and set waiting for acknowlege
       */
      itx_send_i_frame (entry, rt);
      break;

    case IQ_RETR_FRAME:
      /*
       * Increment retransmission counter and check if 
       * another transmission of this frame is allowed.
       */
      entry->n_retr++;
      if (entry->n_retr <= *(llc_data->n200) )
      {
        /*
         * Retransmit frame and set waiting for acknowlege
         */
        itx_send_i_frame (entry, rt);
      }
      else
      {
        /*
         * Initiate re-establish
         */
        sig_itx_llme_reest_ind (LLGMM_ERRCS_ACK_NO_PEER_RES_REEST);
      }
      break;

    default:
      TRACE_ERROR ("UNEXPECTED RETURN CODE");
      break;
  }

} /* itx_send_next_frame() */


/*
+------------------------------------------------------------------------------
| Function    : itx_s_queue_clean
+------------------------------------------------------------------------------
| Description : This procedure removes all entries from the S frame queue
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void itx_s_queue_clean (void)
{
  T_ITX_S_QUEUE_ENTRY** entry = &(llc_data->itx->s_queue);
  
  TRACE_FUNCTION( "itx_s_queue_clean" );

  while (*entry)
  {
    /*
     * get pointer to next (=first) entry
     */
    T_ITX_S_QUEUE_ENTRY* current = *entry;

    /*
     * remove next entry from the entry (make second to first)
     */
    *entry = current->next;
    
    /*
     * free the removed entry
     */
    MFREE (current);
  }
} /* itx_s_queue_clean() */


/*
+------------------------------------------------------------------------------
| Function    : itx_i_queue_clean
+------------------------------------------------------------------------------
| Description : This procedure removes all entries including the attached L3 
|               data from the I-frame queue.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void itx_i_queue_clean (void)
{
  T_ITX_I_QUEUE_ENTRY** entry = &(llc_data->itx->i_queue.first);
  
  TRACE_FUNCTION( "itx_i_queue_clean" );

  while( *entry )
  {
    /*
     * get pointer to next (=first) entry
     */
    T_ITX_I_QUEUE_ENTRY* current = *entry;

    /*
     * remove next entry from the entry (make second to first)
     */
    *entry = current->next;
    
    /*
     * free L3 primitive data, if attached to the removed entry
     */
    if (current->frame)
    {

      /*
       * Decrease attached counter. If no one is still attached
       * free the primitive memory
       */
      if (current->frame->attached_counter > CCI_NO_ATTACHE)
      {
        current->frame->attached_counter --;

        if (current->frame->attached_counter == CCI_NO_ATTACHE)
        {
#ifdef LL_DESC
          /*
           * The descriptor contents of the primitive structure
           * must be freed as well.
           */
          llc_cl_desc3_free((T_desc3*)current->frame->desc_list3.first);
#endif /* LL_DESC */
          PFREE (current->frame);
        }
        else
        {
          TRACE_0_INFO("LL_DATA_REQ still attached");
        }
      }
    }

    /*
     * free next entry
     */
    MFREE (current);
  }

  /*
   * Adjust number of stored entries in I frame queue
   */
  llc_data->itx->i_queue.entries = 0;

  /*
   * Adjust I frame queue size
   */
  llc_data->itx->B_tx = 0;

  /*
   * If a I frame is associated with T201, also reset this
   */
  TIMERSTOP (T201);
  llc_data->itx->t201_entry = NULL;

} /* itx_i_queue_clean() */


/*
+------------------------------------------------------------------------------
| Function    : itx_handle_ll_ready_ind
+------------------------------------------------------------------------------
| Description : This procedure checks if further storing of L3 data is allowed.
|               If possible, a LL_READY_IND is send.
|
| Parameters  : data_send - TRUE if data sent was the reason to call
|
+------------------------------------------------------------------------------
*/
GLOBAL void itx_handle_ll_ready_ind (BOOL data_send)
{
  ULONG M = *(llc_data->mu) * 16;

  TRACE_FUNCTION( "itx_handle_ll_ready_ind" );

  /*
   * If data_send was the reason to call this function,
   * only send ready, if buffer was full last call.
   */
  if ((data_send == TRUE) && (llc_data->itx->buffer_was_full == FALSE))
  {
    /*
     * Nothing to do
     */
    return;
  }

  /*
   * Check if we already have stored more frames than current ku or if
   * M - if relevant - would exceed if we would add an N201 size frame.
   * If this is true set buffer_was_full to TRUE. Otherwise send out an
   * LL_READY_IND.
   */
  if ((llc_data->itx->i_queue.entries < *(llc_data->ku) + ITX_ADD_QUEUE_SIZE) AND
      ( (M == 0) OR (*(llc_data->n201_i) <= M - llc_data->itx->B_tx) )          )
  {
    PALLOC (ll_ready_ind, LL_READY_IND);

    ll_ready_ind->sapi = llc_data->current_sapi;

    TRACE_1_OUT_PARA("s:%d", ll_ready_ind->sapi);
    PSEND (hCommSNDCP, ll_ready_ind);

    llc_data->itx->buffer_was_full = FALSE;
  }
  else
  {
    llc_data->itx->buffer_was_full = TRUE;

    TRACE_0_INFO("ITX buffer full");
  }
} /* itx_handle_ll_ready_ind() */

/*
+------------------------------------------------------------------------------
| Function    : itx_i_queue_store 
+------------------------------------------------------------------------------
| Description : This procedure tries to store the L3 data request.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void itx_i_queue_store 
(
#ifdef LL_DESC
  T_LL_DESC_REQ* ll_desc_req, BOOL *result
#else
  T_LL_DATA_REQ* ll_desc_req, BOOL *result
#endif
)
{
#ifdef LL_DESC
  T_desc3* desc3;
#endif
  ULONG M = *(llc_data->mu) * 16;

  TRACE_FUNCTION( "itx_i_queue_store" );

  /*
   * Check, if M would exceed if we transmit this frame.
   */
#ifdef LL_DESC
  if ( M == 0 || 
       (ULONG)(ll_desc_req->desc_list3.list_len) <= M - llc_data->itx->B_tx )
#else
  if ( M == 0 || 
       (ULONG)(BYTELEN(ll_desc_req->sdu.l_buf)) <= M - llc_data->itx->B_tx )
#endif
  {
    /*
     * Store data to the end of the queue (queue type: oldies first)
     */
    T_ITX_I_QUEUE_ENTRY** entry = &(llc_data->itx->i_queue.first);
  
    while( *entry )
      entry = &((*entry)->next);

    MALLOC( *entry, sizeof(T_ITX_I_QUEUE_ENTRY) );

    if( *entry )
    {
      /*
       * Memory successful allocated. Fill in struct entries.
       */
      (*entry)->next       = NULL;
      (*entry)->status     = IQ_NEW_FRAME;
      (*entry)->n_retr     = 0;
      (*entry)->ns         = 0;
      (*entry)->frame      = ll_desc_req;

      /*
       * Store primitive header parameter
       */
      (*entry)->ll_qos     = ll_desc_req->ll_qos;
      (*entry)->radio_prio = ll_desc_req->radio_prio;
      (*entry)->reference  = ll_desc_req->reference1;
      (*entry)->seg_pos    = ll_desc_req->seg_pos;

#ifdef REL99 
      /*
       * Fill packet flow identifier with corrrect values if it is from one of the SNDCP SAP.
       * If data request is from GMM fill PFI = LL_PFI_SIGNALING,
       * If data request is from GSMS fill PFI = LL_PFI_SMS   
       * for all other SAPs(if sapi 2 & 8 are supported in future) set pkt_flow_id is to LL_PKT_FLOW_ID_NOT_PRES,
       * until specification are clarified.
       */
      switch(ll_desc_req->sapi)
      {
        case LL_SAPI_1:
         /* 
          * From 24.008 & 23.060 it is interpreted that for all signalling data, a 
          * predefined PFI LL_PFI_SIGNALING shall be used.
          */
  	      (*entry)->pkt_flow_id = LL_PFI_SIGNALING;
          break;

        case LL_SAPI_3:
        case LL_SAPI_5:
        case LL_SAPI_9:
        case LL_SAPI_11:
          (*entry)->pkt_flow_id = (UBYTE)ll_desc_req->pkt_flow_id;
  	      break;

        case LL_SAPI_7:
         /* 
          * From 24.008 & 23.060 it is interpreted that for all SMS data, a 
          * predefined PFI LL_PFI_SMS shall be used.
          */
          (*entry)->pkt_flow_id = LL_PFI_SMS;
          break;

        default:
        /*
         * It is possible when we support llc sapi 2 and 8 are supported.
         * Fill PFI valuse it LL_PKT_FLOW_ID_NOT_PRES
         */
        (*entry)->pkt_flow_id = LL_PKT_FLOW_ID_NOT_PRES;
          break;
      }
#endif /*REL99 */

#ifdef LL_DESC
      desc3 = (T_desc3*)ll_desc_req->desc_list3.first;

      (*entry)->offset  = desc3->offset;
      (*entry)->len   = desc3->len;

      /*
       * Increase attached counter
       */
      ll_desc_req->attached_counter ++;

      /*
       * Increase number of stored entries in I frame queue
       */
      llc_data->itx->i_queue.entries++;

      /*
       * Increase amount of stored Information in I frame queue
       */

      llc_data->itx->B_tx += (ll_desc_req->desc_list3.list_len);

#else
      (*entry)->sdu_o_buf  = ll_desc_req->sdu.o_buf;
      (*entry)->sdu_l_buf  = ll_desc_req->sdu.l_buf;

      /*
       * Increase attached counter
       */
      ll_desc_req->attached_counter ++;

      /*
       * Increase number of stored entries in I frame queue
       */
      llc_data->itx->i_queue.entries++;

      /*
       * Increase amount of stored Information in I frame queue
       */
      llc_data->itx->B_tx += BYTELEN(ll_desc_req->sdu.l_buf);

#endif /*LL_DESC */
      /*
       * Store data succeded
       */
      *result = TRUE;
      return;
    }
    else
    {
      /*
       * Out of memory
       */
      TRACE_ERROR( "Out of memory in itx_i_queue_store()" );
    }
  }

  *result = FALSE;
  return;
} /* itx_i_queue_store() */



/*
+------------------------------------------------------------------------------
| Function    : itx_i_queue_get_ready
+------------------------------------------------------------------------------
| Description : This procedure checks, if the next entry in the queue is
|               already acknowledged. If this is true, the entry will be 
|               removed and the frame reference will be returned.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void itx_i_queue_get_ready (BOOL*           found, 
#ifdef LL_2to1                                   
                                   T_LL_reference1*   reference, 
#else
                                   T_reference1*   reference, 
#endif
                                   UBYTE           state)
{
  T_ITX_I_QUEUE_ENTRY** entry = &(llc_data->itx->i_queue.first);

  TRACE_FUNCTION( "itx_i_queue_get_ready" );

  /*
   * The first entry is the oldest. Only this must be observed.
   */
  if ((*entry) != NULL && (*entry)->status == IQ_IS_ACK_FRAME)
  {
    /*
     * store pointer to the entry
     */
    T_ITX_I_QUEUE_ENTRY* current = *entry;

    /*
     * remove entry from the queue (make second to first)
     */
    *entry = current->next;
    
    /*
     * Decrease number of stored entries in I frame queue
     */
    llc_data->itx->i_queue.entries--;

    if (current->frame)
    {

      /*
       * return original data request reference number
       */
      *reference = current->reference;

      /*
       * decrease amount of stored Information in I frame queue.
       * Take the original value of sdu_len as it was send by L3
       */
#ifdef LL_DESC
      if (llc_data->itx->B_tx >= (ULONG)(current->len))
      {
        llc_data->itx->B_tx -= (ULONG)(current->len);
#else      
      if (llc_data->itx->B_tx >= (ULONG)(BYTELEN(current->sdu_l_buf)))
      {
        llc_data->itx->B_tx -= (ULONG)(BYTELEN(current->sdu_l_buf));
#endif
      }
      else
      {
        llc_data->itx->B_tx = 0;
        TRACE_0_INFO("Unexpected SDU bytelen handled");
      }

      /*
       * decrease attached counter. If no one is still attached
       * free the primitive memory
       */
      if (current->frame->attached_counter > CCI_NO_ATTACHE)
      {
        current->frame->attached_counter --;

        if (current->frame->attached_counter == CCI_NO_ATTACHE)
        {
#ifdef LL_DESC
          llc_cl_desc3_free((T_desc3*)current->frame->desc_list3.first);
#endif /* LL_DESC */
          PFREE (current->frame);
        }
        else
        {
          TRACE_0_INFO("LL_DATA_REQ still attached");
        }
      }
    }
    else
    {
      *found = FALSE;
      TRACE_0_INFO("No LL_DATA_REQ attached to queue entry");
      return;
    }

    /*
     * if the retrieved I frame is associated with T201, reset this
     */
    if (current == llc_data->itx->t201_entry)
    {
      if (state != ITX_ABM_PEER_BUSY)
      {
        /*
         * In this state the timer handles other stuff
         */
        TIMERSTOP (T201);
      }
      
      llc_data->itx->t201_entry = NULL;
    }

    /*
     * free retrieved entry
     */
    MFREE (current);

    *found = TRUE;
  }
  else
  {
    *found = FALSE;
  }
} /* itx_i_queue_get_ready() */



/*
+------------------------------------------------------------------------------
| Function    : itx_i_queue_set_status
+------------------------------------------------------------------------------
| Description : This procedure sets the acknowledge flag to the frame send with
|               N equal to num.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void itx_i_queue_set_status (T_IQ_STATUS status, T_FRAME_NUM num)
{
  T_ITX_I_QUEUE_ENTRY* entry = llc_data->itx->i_queue.first;
  
  TRACE_FUNCTION( "itx_i_queue_set_status" );

  while (entry)
  {
    /*
     * check if we have found the entry
     */
    if (entry->ns == num )
    {
      /*
       * found it - set ack and leave
       */
      entry->status = status;
      return;
    }

    /*
     * goto next entry
     */
    entry = entry->next;
  }
} /* itx_i_queue_set_status() */

     
/*
+-----------------------------------------------------------------------------
| Function    : itx_s_queue_store 
+------------------------------------------------------------------------------
| Description : This procedure stores the command to the end of the S queue
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void itx_s_queue_store (T_COMMAND        command, 
                               T_ABIT_REQ_TYPE  req_type,
                               T_FRAME_NUM      nr,
                               T_SACK_BITMAP*   bitmap)
{
  T_ITX_S_QUEUE_ENTRY** entry = &(llc_data->itx->s_queue);

  TRACE_FUNCTION( "itx_s_queue_store" );

  /*
   * find the end
   */
  while( *entry )
    entry = &((*entry)->next);

  /*
   * allocate memory
   */
  MALLOC( *entry, sizeof(T_ITX_S_QUEUE_ENTRY) );

  if( *entry )
  {
    /*
     * Memory successful allocated. Fill in struct entries.
     */
    (*entry)->next   = NULL;
    (*entry)->sx     = command;
    (*entry)->rt     = req_type;
    (*entry)->nr     = nr;

    /*
     * Copy SACK bitmap
     */
    if (bitmap)
    {
      (*entry)->bitmap = *bitmap;
    }
  }
  else
  {
    /*
     * Out of memory
     */
    TRACE_ERROR( "Out of memory in itx_i_queue_store()" );
  }
} /* itx_s_queue_store() */

      
/*
+------------------------------------------------------------------------------
| Function    : itx_s_queue_retrieve
+------------------------------------------------------------------------------
| Description : This procedure returns the first entry and removes it from the
|               the queue or, if not available returns a RR command with found 
|               set to false.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
LOCAL void itx_s_queue_retrieve (BOOL*            found, 
                                 T_COMMAND*       sx, 
                                 T_ABIT_REQ_TYPE* rt, 
                                 T_FRAME_NUM*     nr,
                                 T_SACK_BITMAP*   bm, 
                                 USHORT*          bm_byte_size)
{
  int i;

  T_ITX_S_QUEUE_ENTRY** entry = &(llc_data->itx->s_queue);

  TRACE_FUNCTION( "itx_s_queue_retrieve" );

  /*
   * Take the next entry, if available
   */
  if (*entry != NULL)
  {
    /*
     * Store pointer to the entry
     */
    T_ITX_S_QUEUE_ENTRY* current = *entry;

    /*
     * Remove entry from the queue (make second to first)
     */
    *entry = current->next;

    *sx = current->sx;
    *rt = current->rt;
    *nr = current->nr;
    *bm = current->bitmap;
    
    /*
     * Free retrieved entry
     */
    MFREE (current);

    *found = TRUE;

    /*
     * Special handling for SACK command
     */
    if(*sx == I_SACK)
    {
      /*
       * Calculate bitmap size
       */
      for(i = S_FRAME_SACK_MAX_CTRL_OCTETS-1; i >= 0; i--)
      {
        if( bm->data[i] != 0)
        {
          /*
           * Return bm size in bits
           */
          *bm_byte_size = i+1;
          return;
        }
      }
    }

    /*
     * No valid bitmap found, return
     */
    *bm_byte_size = 0;
    return;
  }
  else
  {
    /*
     * Set default return values
     */
    *found        = FALSE;
    *sx           = I_RR;
    *nr           = llc_data->sapi->vr;
    *rt           = ABIT_NO_REQ;
    *bm_byte_size = 0;
  }
} /* itx_s_queue_retrieve () */
    

/*
+------------------------------------------------------------------------------
| Function    : itx_send_i_frame
+------------------------------------------------------------------------------
| Description : This procedure gets the next supervisor information to send
|               and build together with the I frame to transmit the final 
|               (expect ciphering and FSC, which will be done by TX) LLC frame.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
LOCAL void itx_send_i_frame (T_ITX_I_QUEUE_ENTRY* entry,
                             T_ABIT_REQ_TYPE      a_req)
{
#ifdef LL_DESC
  T_desc_list3* desc_list3;
  T_desc3*      desc3;
  U8*           desc_buf;
#else
  T_sdu*            sdu;
#endif
  T_COMMAND         sx;
  T_ABIT_REQ_TYPE   rt;
  T_SACK_BITMAP     bm;
  T_FRAME_NUM       nr;
  USHORT            bm_byte_size;
  USHORT            header_size;
  USHORT            offset;
  BOOL              found;

  TRACE_FUNCTION( "itx_send_i_frame" );

#ifdef LL_DESC
  if (entry && entry->frame)
  {
    PALLOC(ll_unitdesc_req, LL_UNITDESC_REQ);

    ll_unitdesc_req->attached_counter = entry->frame->attached_counter;
    ll_unitdesc_req->desc_list3       = entry->frame->desc_list3;

    /*
     * Restore original primitive header parameter of the frame.
     */
    ll_unitdesc_req->sapi       = llc_data->current_sapi;
    ll_unitdesc_req->tlli       = llc_data->tlli_new;
    ll_unitdesc_req->ll_qos     = entry->ll_qos;       
    ll_unitdesc_req->cipher     = LL_CIPHER_ON;
    ll_unitdesc_req->radio_prio = entry->radio_prio;   
    ll_unitdesc_req->seg_pos    = entry->seg_pos;
#ifdef REL99 
    ll_unitdesc_req->pkt_flow_id = entry->pkt_flow_id;
#endif /* REL99 */

    desc_list3 = &(ll_unitdesc_req->desc_list3);

    desc3 = (T_desc3*)(desc_list3->first);

    /*
     * Restore original desc3 offset and length
     */    
    desc3->offset = entry->offset;
    desc3->len = entry->len;

#else
  if (entry && entry->frame)
  {
    PPASS (entry->frame, ll_unitdesc_req, LL_UNITDATA_REQ);

    /*
     * Restore original primitive header parameter of the frame.
     */
    ll_unitdesc_req->sapi       = llc_data->current_sapi;
    ll_unitdesc_req->tlli       = llc_data->tlli_new;
    ll_unitdesc_req->ll_qos     = entry->ll_qos;       
    ll_unitdesc_req->cipher     = LL_CIPHER_ON;
    ll_unitdesc_req->radio_prio = entry->radio_prio;   
    ll_unitdesc_req->seg_pos    = entry->seg_pos;
#ifdef REL99 
    ll_unitdesc_req->pkt_flow_id = entry->pkt_flow_id;
#endif /* REL99 */

    sdu = &(ll_unitdesc_req->sdu);

    /*
     * Restore original SDU offset and length
     */    
    sdu->o_buf = entry->sdu_o_buf;
    sdu->l_buf = entry->sdu_l_buf;
#endif


    /*
     * Get next S-frame. Take the returned RR if not found = FALSE.
     */
    itx_s_queue_retrieve (&found, &sx, &rt, &nr, &bm, &bm_byte_size);

    /*
     * Overwrite A-bit flag request, if neccessary
     */
    if (a_req == ABIT_SET_REQ)
    {
      rt = ABIT_SET_REQ;
    }

#ifndef LL_DESC
    /*
     * header_size is 4 octets + 1 byte SACK header and the size of 
     * SACK bitmap. The SDU length equal bytes more than the original values.
     */
    if (bm_byte_size > 0)
    {
      header_size = I_CTRL_MIN_OCTETS + 1 + bm_byte_size;
      sdu->o_buf -= header_size * 8;
      sdu->l_buf += header_size * 8;
    }
    else
    {
      header_size = I_CTRL_MIN_OCTETS;
      sdu->o_buf -= header_size * 8;
      sdu->l_buf += header_size * 8;
    }

    if (entry->sdu_o_buf < header_size * 8)
    {
      TRACE_ERROR ("ERROR: SDU offset not big enough");
      /* 
       * Now try to give stack a chance to continue
       */
      sdu->o_buf = 0; 
    }

    offset = sdu->o_buf/8;

    /*
     * Build LLC Header:
     * First insert address field (PD and C/R bit are always 0)
     */
    sdu->buf[offset++] = llc_data->current_sapi;
    
    /*
     * Insert 1st octet of control field:
     * Set A bit and 5 most significant bits of N(S)
     */
    if (rt == ABIT_SET_REQ)
    {
      sdu->buf[offset++] = 0x40 | (UBYTE)(entry->ns >> 4);
    }
    else
    {
      sdu->buf[offset++] = (UBYTE)(entry->ns >> 4);
    }

    /*
     * Insert 2nd octet of control field:
     * 4 least significant bits of N(S) and 3 most significant bits of N(R)
     */
    sdu->buf[offset++] = (UBYTE)(((entry->ns << 4) & 0xF0) | (nr >> 6));

    /*
     * Insert 3nd octet of control field:
     * 6 least significant bits of N(R) and the supervisory function bits
     */
    sdu->buf[offset++] = (UBYTE)(((nr << 2) & 0xFC) | sx);

    /*
     * Special SACK handling
     */
    if (sx == I_SACK && bm_byte_size != 0)
    {
      USHORT i;

      /*
       * Insert 1 SACK octet: k - number of SACK bytes
       */
      sdu->buf[offset++] = (UBYTE)bm_byte_size;

      /*
       * Insert SACK bitmap
       */
      for (i = 0; i < bm_byte_size; i++)
      {
        sdu->buf[offset++] = bm.data[i];
      }
    }

    /*
     * Set frame to waiting for ack
     */
    entry->status = IQ_W4ACK_FRAME;
#else /* LL_DESC*/

    /*
     * header_size is 4 octets + 1 byte SACK header and the size of 
     * SACK bitmap. The SDU length equal bytes more than the original values.
     */
    if (bm_byte_size > 0)
    {
      header_size = I_CTRL_MIN_OCTETS + 1 + bm_byte_size;
      desc3->offset -= header_size;
      desc3->len += header_size;
      desc_list3->list_len += header_size;
    }
    else
    {
      header_size = I_CTRL_MIN_OCTETS;
      desc3->offset -= header_size;
      desc3->len += header_size;
      desc_list3->list_len += header_size;
    }

    if (entry->offset < header_size)
    {
      TRACE_ERROR ("ERROR: SDU offset not big enough");
      /* 
       * Now try to give stack a chance to continue
       */
      desc3->offset = 0; 
    }

    offset   = desc3->offset;
    desc_buf = (UBYTE*)desc3->buffer;
    
    if(!desc_buf)
    {
      TRACE_ERROR("desc_buf is NULLPTR !!!");
      return;
    }

    /*
     * Build LLC Header:
     * First insert address field (PD and C/R bit are always 0)
     */
    desc_buf[offset++] = llc_data->current_sapi;
    
    /*
     * Insert 1st octet of control field:
     * Set A bit and 5 most significant bits of N(S)
     */
    if (rt == ABIT_SET_REQ)
    {
      desc_buf[offset++] = 0x40 | (UBYTE)(entry->ns >> 4);
    }
    else
    {
      desc_buf[offset++] = (UBYTE)(entry->ns >> 4);
    }

    /*
     * Insert 2nd octet of control field:
     * 4 least significant bits of N(S) and 3 most significant bits of N(R)
     */
    desc_buf[offset++] = (UBYTE)(((entry->ns << 4) & 0xF0) | (nr >> 6));

    /*
     * Insert 3nd octet of control field:
     * 6 least significant bits of N(R) and the supervisory function bits
     */
    desc_buf[offset++] = (UBYTE)(((nr << 2) & 0xFC) | sx);

    /*
     * Special SACK handling
     */
    if (sx == I_SACK && bm_byte_size != 0)
    {
      USHORT i;

      /*
       * Insert 1 SACK octet: k - number of SACK bytes
       */
      desc_buf[offset++] = (UBYTE)bm_byte_size;

      /*
       * Insert SACK bitmap
       */
      for (i = 0; i < bm_byte_size; i++)
      {
        desc_buf[offset++] = bm.data[i];
      }
    }

    /*
     * Set frame to waiting for ack
     */
    entry->status = IQ_W4ACK_FRAME;

#endif /* LL_DESC */
#ifdef TRACE_EVE
    {
      static char* sf[] = { "RR", "ACK", "RNR", "SACK" };

      if(sx <= 3)
      {
        TRACE_4_INFO("Next I-Frame %s NS:%d NR:%d A:%d", sf[sx], entry->ns, nr, rt);
      }
    }
#endif

    /*
     * reset TX ready flag
     */
    llc_data->itx->tx_waiting = FALSE;

    /*
     * If A bit was set, associate T201 with the transmitted I frame
     * and start T201
     */
    if (rt == ABIT_SET_REQ)
    {
      llc_data->itx->t201_entry = entry;
      TIMERSTART(T201, llc_data->t200->length);
    }

    /*
     * Now send the frame to TX ( do this as last statement - recursion possible)
     */
    sig_itx_tx_data_req( ll_unitdesc_req, I_FRAME, entry->ns, GRLC_DTACS_DEF, header_size,
                         entry->oc_i_tx);
  }
} /* itx_send_i_frame() */


/*
+------------------------------------------------------------------------------
| Function    : itx_send_s_frame
+------------------------------------------------------------------------------
| Description : This procedure sends the supervisor command to TX
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
LOCAL void itx_send_s_frame (T_COMMAND             sx,
                             T_ABIT_REQ_TYPE       rt, 
                             T_FRAME_NUM           nr,
                             T_SACK_BITMAP*        bm,
                             USHORT                bm_byte_size)
{
  USHORT offset;

#ifndef LL_DESC
  T_sdu* sdu;
  USHORT sdu_bitsize;
#else
  T_desc3* desc3; 
  USHORT desc_bytesize;
  U8* buf;  
#endif

  TRACE_FUNCTION( "itx_send_s_frame" );

#ifndef LL_DESC
  /*
   * Calculate SDU size
   */
  sdu_bitsize =  (S_FRAME_MIN_OCTETS_WITHOUT_FCS * 8) + (bm_byte_size * 8);

  /*
   * Build primitive and include supervisory command
   */
  {
    PALLOC_SDU (ll_unitdata_req, LL_UNITDATA_REQ, sdu_bitsize);

    ll_unitdata_req->sapi = llc_data->current_sapi;
    ll_unitdata_req->tlli = llc_data->tlli_new;

    ll_unitdata_req->attached_counter = CCI_NO_ATTACHE;
    /*
     * LLC does not know the QoS profile. S frames should use 
     * LL_NO_REL
     */
    ll_unitdesc_req->ll_qos = llc_data->cur_qos;
    TRACE_EVENT_P1("peak throughput = %d",llc_data->cur_qos.peak);

    /*
     * LLC signalling frames are always sent with highest priority.
     */
    ll_unitdesc_req->radio_prio = llc_data->cur_radio_prio;
    TRACE_EVENT_P1("radio priority = %d",llc_data->cur_radio_prio);


#ifdef REL99 
    /* 
     * From 24.008 & 23.060 it is interpreted that for all signalling data, a 
     * predefined PFI LL_PFI_SIGNALING shall be used.
     */
    ll_unitdesc_req->pkt_flow_id = llc_data->cur_pfi;
    TRACE_EVENT_P1("packet flow id = %d",llc_data->cur_pfi);

#endif  /* REL99 */

    /*
     * Don't worry about L3 reference - TX will ignore it
     */

    /*
     * SDU offset is 0. The SDU length is sdu_bitsize
     */
    sdu = &(ll_unitdata_req->sdu);
    sdu->o_buf = 0;
    sdu->l_buf = sdu_bitsize;

    offset = 0;

    /*
     * Build LLC Header:
     * First insert address field (PD and C/R bit are always 0)
     */
    sdu->buf[offset++] = llc_data->current_sapi;
  
    /*
     * Set A bit and 3 most significant bits of N(R)
     */
    if (rt == ABIT_SET_REQ)
    {
      /* LINTED [following sdu index fits into sdu] */ 
      sdu->buf[offset++] = 0xA0 | (UBYTE)(nr >> 6); 
    }
    else
    {
      /* LINTED [following sdu index fits into sdu] */ 
      sdu->buf[offset++] = 0x80 | (UBYTE)(nr >> 6);
    }

    /* 
     * 6 least significant bits of N(R) and the supervisory function bits
     */
    {
      /* LINTED [following sdu index fits into sdu] */ 
      sdu->buf[offset++] = (UBYTE)(((nr << 2) & 0xFC) | sx);
    }

    /*
     * Special SACK bitmap handling
     */
    if (sx == I_SACK && bm_byte_size != 0 && bm != NULL)
    {
      USHORT i;

      /*
       * Insert SACK bitmap
       */
      for (i = 0; i < bm_byte_size; i++)
      {
        /* LINTED [following sdu indexes are fitting into sdu] */ 
        sdu->buf[offset++] = bm->data[i];
      }
    }

#ifdef TRACE_EVE
    {
      static char* sf[] = { "RR", "ACK", "RNR", "SACK" };

      if(sx <= 3)
      {
        TRACE_3_INFO("Next S-Frame %s NR:%d A:%d", sf[sx], nr, rt);
      }
    }
#endif

    /*
     * Reset TX ready flag (before sending because of possible recursion)
     */
    llc_data->itx->tx_waiting = FALSE;

    /*
     * Now send the frame to TX. header_size and oc are not used and set to zero.
     */
    sig_itx_tx_data_req( ll_unitdata_req, S_FRAME, nr, GRLC_DTACS_DEF, 0, 0);
  }
#else /* LL_DESC */

  /*
   * Calculate DESC size
   */
  desc_bytesize =  (S_FRAME_MIN_OCTETS_WITHOUT_FCS) + (bm_byte_size);

  /*
   * Build primitive and include supervisory command
   */
  {
    PALLOC (ll_unitdesc_req, LL_UNITDESC_REQ);

    desc3 = llc_palloc_desc(desc_bytesize, 0);

    ll_unitdesc_req->desc_list3.first    = (ULONG)desc3;
    ll_unitdesc_req->desc_list3.list_len = desc3->len;

    ll_unitdesc_req->sapi = llc_data->current_sapi;
    ll_unitdesc_req->tlli = llc_data->tlli_new;

    ll_unitdesc_req->attached_counter = CCI_NO_ATTACHE;
    /*
     * LLC does not know the QoS profile. S frames should use 
     * LL_NO_REL
     */
    ll_unitdesc_req->ll_qos.peak     = GRLC_PEAK_SUB;
#ifdef LL_2to1
    ll_unitdesc_req->ll_qos.relclass = PS_NO_REL;
#else
    ll_unitdesc_req->ll_qos.relclass = LL_NO_REL;
#endif

    /*
     * LLC signalling frames are always sent with highest priority.
     */
    ll_unitdesc_req->radio_prio = GRLC_RADIO_PRIO_1;
#ifdef REL99 
    /* 
     * From 24.008 & 23.060 it is interpreted that for all signalling data, a 
     * predefined PFI LL_PFI_SIGNALING shall be used.
     */
    ll_unitdesc_req->pkt_flow_id = LL_PFI_SIGNALING;
#endif  /* REL99 */
    /*
     * Don't worry about L3 reference - TX will ignore it
     */

    /*
     * SDU offset is 0. The SDU length is sdu_bitsize
     */
    desc3->offset = 0;
    desc3->len = desc_bytesize;

    offset = 0;
    buf = (U8*)desc3->buffer;
    /*
     * Build LLC Header:
     * First insert address field (PD and C/R bit are always 0)
     */
    buf[offset++] = llc_data->current_sapi;
  
    /*
     * Set A bit and 3 most significant bits of N(R)
     */
    if (rt == ABIT_SET_REQ)
    {
      /* LINTED [following sdu index fits into sdu] */ 
      buf[offset++] = 0xA0 | (UBYTE)(nr >> 6); 
    }
    else
    {
      /* LINTED [following sdu index fits into sdu] */ 
      buf[offset++] = 0x80 | (UBYTE)(nr >> 6);
    }

    /* 
     * 6 least significant bits of N(R) and the supervisory function bits
     */
    {
      /* LINTED [following sdu index fits into sdu] */ 
      buf[offset++] = (UBYTE)(((nr << 2) & 0xFC) | sx);
    }

    /*
     * Special SACK bitmap handling
     */
    if (sx == I_SACK && bm_byte_size != 0 && bm != NULL)
    {
      USHORT i;

      /*
       * Insert SACK bitmap
       */
      for (i = 0; i < bm_byte_size; i++)
      {
        /* LINTED [following sdu indexes are fitting into sdu] */ 
        buf[offset++] = bm->data[i];
      }
    }

#ifdef TRACE_EVE
    {
      static char* sf[] = { "RR", "ACK", "RNR", "SACK" };

      if(sx <= 3)
      {
        TRACE_3_INFO("Next S-Frame %s NR:%d A:%d", sf[sx], nr, rt);
      }
    }
#endif

    /*
     * Reset TX ready flag (before sending because of possible recursion)
     */
    llc_data->itx->tx_waiting = FALSE;

    /*
     * Now send the frame to TX. header_size and oc are not used and set to zero.
     */
    sig_itx_tx_data_req( ll_unitdesc_req, S_FRAME, nr, GRLC_DTACS_DEF, 0, 0);
  }
#endif /* LL_DESC */
} /* itx_send_s_frame() */



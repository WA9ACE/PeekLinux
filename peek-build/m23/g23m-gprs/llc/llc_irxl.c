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
|  Purpose :  This modul is part of the entity LLC and implements all labels
|             in the SDL-documentation (IRX-statemachine) that are called from
|             more than one diagram/place.
+----------------------------------------------------------------------------- 
*/ 

#ifndef LLC_IRXL_C
#define LLC_IRXL_C
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

#include "llc_irxf.h"   /* to get local IRX functions */
#include "llc_irxl.h"   /* to get local IRX labels */
#include "llc_llmes.h"  /* to get signal interface to LLME */
#include "llc_txs.h"    /* to get signal interface to TX */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/


/*
+------------------------------------------------------------------------------
| Function     : irx_get_next_gap
+------------------------------------------------------------------------------
| Description  : Searches queue with already received frames. 
|
| Parameters   : next_free       - pointer to T_FRAME_NUM, number of the next
|                                  frame that has not been received yet.
|                                  If queue is empty, this parameter will be 
|                                  set to (llc_data->sapi->vr + 1) % 
|                                  (MAX_SEQUENCE_NUMBER + 1).
|                gap_size        - pointer to T_FRAME_NUM, 0, 1 or bigger
| Preconditions: llc_data->sapi points to the affected sapi.
|                There is 1 entry in the queue with vr==ns.
|
+------------------------------------------------------------------------------
*/
GLOBAL void irx_get_next_gap (T_FRAME_NUM* next_free,
                              T_FRAME_NUM* gap_size)
{ 

  T_IRX_QUEUE* entry = llc_data->irx->queue;
  BOOL gap_found = FALSE;
 
  TRACE_FUNCTION("irx_get_next_gap");

  *next_free = (llc_data->sapi->vr + 1) % (MAX_SEQUENCE_NUMBER + 1);  
  *gap_size = 0;

  TRACE_EVENT_P1("*next_free: %d", *next_free);

  
  while ((entry != NULL) && 
          FRAME_NUM_VALID( entry->ns,
                           llc_data->sapi->vr,
                           llc_data->sapi->vr + *(llc_data->ku)) )
  {
    entry = entry->next;
  }
  
  /*
   * If rest of queue is empty, values of next_free and gap_size are ok.
   */
  if (entry == NULL) 
  {
    return;
  } /* if (entry == NULL) */

  /*
   * Queue is not empty. Find the first gap.
   */
  while ((entry != NULL) && !gap_found) 
  {
    T_FRAME_NUM num = entry->ns;

    if (num == *next_free) 
    {
      entry = entry->next;
      *next_free = (*next_free + 1) % (MAX_SEQUENCE_NUMBER + 1);
    }
    else 
    {
      gap_found = TRUE;
      *gap_size = (num - *next_free) % (MAX_SEQUENCE_NUMBER + 1);
    }
  } /* while ((entry != NULL) && !gap_found) */

  TRACE_EVENT_P2("*next_free: %u *gap_size: %u ", *next_free, *gap_size);
 

} /* irx_get_next_gap() */







/*==== PUBLIC FUNCTIONS =====================================================*/


/*
+------------------------------------------------------------------------------
| Function    : irx_label_if
+------------------------------------------------------------------------------
| Description : Describes label IRX_IF
|
| Parameters  : ll_unitdata_ind - a valid pointer to an LL_UNITDATA_IND
|                                 primitive
|               ns              - N(S) of received frame
|               a_bit           - A bit of received frame
|               state           - current state of IRX
|
+------------------------------------------------------------------------------
*/
GLOBAL void irx_label_if (T_LL_UNITDATA_IND *ll_unitdata_ind, 
                          T_FRAME_NUM       ns, 
                          T_BIT             a_bit,
                          UBYTE             state)
{ 
  BOOL        is_busy;
  T_FRAME_NUM vsi;
  BOOL        vsi_found;

  
  TRACE_FUNCTION( "irx_label_if" );

  if (state == IRX_ABM)
  {
    /*
     * Label IRX_IF_ABM
     */
    if ( ns == llc_data->sapi->vr )
    {
      BOOL found = FALSE;
      T_FRAME_NUM num = 0;
      irx_get_last_queued_ns(&found, &num);
      if (!found) 
      {
        /*
         * Store N(S)/V(R) condition
         */
        llc_data->irx->last_ns = NS_EQUAL_VR;
        /*
         * If N(s) = V(r) increment V(r), handle OC(rx)
         */
        llc_data->sapi->vr++;
       
        if (llc_data->sapi->vr > MAX_SEQUENCE_NUMBER)
        {
          llc_data->sapi->oc_i_rx += (MAX_SEQUENCE_NUMBER+1);
          llc_data->sapi->vr       = 0;
        }
      } 
      else
      {

        T_FRAME_NUM next_free = 0;
        T_FRAME_NUM gap_size = 0;
         
        irx_get_next_gap (&next_free, &gap_size);

        /* 
         * vr will be set to next_free. If MAX_SEQUENCE_NUMBER is passed,
         * increment overflow counter.
         */
        if (llc_data->sapi->vr > next_free)
        {
          llc_data->sapi->oc_i_rx += (MAX_SEQUENCE_NUMBER + 1);
        }
         
        llc_data->sapi->vr = next_free;

        if (gap_size == 0) {
          llc_data->irx->last_ns = NS_EQUAL_VR;
        } else if (gap_size == 1) {
          llc_data->irx->last_ns = NS_EQUAL_VR_PLUS_1;
        } else {
          llc_data->irx->last_ns = NS_NO_SEQUENCE_ERROR;
        }

      }

      /*
       * Expected next frame received.
       */
      if ( llc_data->irx->ll_send_ready == TRUE )
      {
        /*
         * Forward the frame to L3
         */
        PPASS (ll_unitdata_ind, ll_data_ind, LL_DATA_IND);
        TRACE_2_OUT_PARA("s:%d len:%d", ll_data_ind->sapi, BYTELEN(ll_data_ind->sdu.l_buf));
        PSEND (hCommSNDCP, ll_data_ind);

        llc_data->irx->ll_send_ready = FALSE;

        /*
         * Increment V(f) (= Next frame number to forward to L3)
         */
        llc_data->irx->vf += 1;
        llc_data->irx->vf %= (MAX_SEQUENCE_NUMBER+1);

         /*
          * Update V(R) if we have the next frames already queued
          */
         irx_update_vr ();

         /*
          * Handle the acknowledgement
          */
         irx_send_ack (a_bit);

         /* SET_STATE (IRX, SAME_STATE); */
       }
       else
       {
         /*
          * store frame and update V(R) if next frames already stored
          */
         irx_queue_store (ll_unitdata_ind, ns, &is_busy);
         irx_update_vr ();
       
         if ( is_busy == TRUE )
         {
           irx_send_rnr ();
           SET_STATE (IRX, IRX_ABM_BUSY);
         }
         else
         {
           irx_send_ack (a_bit);
           /* SET_STATE (IRX, SAME_STATE); */
         }
       }
     }
     else
     {
       /*
        * Not the expected next frame received. Check if it is in receive window.
        */
       if ( FRAME_NUM_VALID (llc_data->sapi->vr + 1, ns, llc_data->sapi->vr + *(llc_data->ku)) )
       {
         /*
          * Store N(S)/V(R) condition
          */
         if (ns == (llc_data->sapi->vr + 1) % (MAX_SEQUENCE_NUMBER+1))
         {
           llc_data->irx->last_ns = NS_EQUAL_VR_PLUS_1;
         }
         else
         {
           /*
            * Check if we have detect a sequence error or not: If ns equals to 
            * ns + 1 of the last queued frame then we have not detect a new error
            */
           irx_get_last_queued_ns (&vsi_found, &vsi);

           if (vsi_found)
           {
             if (ns == (vsi + 1) % (MAX_SEQUENCE_NUMBER+1))
             {
               /*
                * ok, seems to be in sequence
                */
               llc_data->irx->last_ns = NS_NO_SEQUENCE_ERROR;
             }
             else
             {
               /*
                * sequence error found
                */
               llc_data->irx->last_ns = NS_IN_RANGE_VR_KU;
             }
           }
           else
           {
             /*
              * sequence error found
              */
             llc_data->irx->last_ns = NS_IN_RANGE_VR_KU;
           }
         }

         /*
          * store frame and update V(R)
          */
         irx_queue_store (ll_unitdata_ind, ns, &is_busy);
       
         if ( is_busy == TRUE )
         {
           irx_send_rnr ();
           SET_STATE (IRX, IRX_ABM_BUSY);
         }
         else
         {
           irx_send_ack (a_bit);
           /* SET_STATE (IRX, SAME_STATE); */
         }
       }
       else
       {
         /*
          * duplicate frame
          */
         TRACE_0_INFO ("Duplicated frame received");

         PFREE (ll_unitdata_ind);
         irx_send_ack (a_bit);
         /* SET_STATE (IRX, SAME_STATE); */
       }
     }
  }
  else
  {
    /*
     * ABM_BUSY
     */
    irx_send_rnr ();

    /*
     * ignore I frame in state 'own receiver busy'
     */
    PFREE (ll_unitdata_ind);
    /* SET_STATE (IRX, SAME_STATE); */
  }

  return;
} /* irx_label_if() */


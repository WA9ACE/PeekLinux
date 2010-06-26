/* 
+------------------------------------------------------------------------------
|  File:       remu_queue.h
+------------------------------------------------------------------------------
|  Copyright 2003 Texas Instruments Berlin, AG 
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
|  Purpose :  Definitions for the riviera emulator queues.
+----------------------------------------------------------------------------- 
*/ 

#ifndef REMU_QUEUE_H
#define REMU_QUEUE_H

/*==== INCLUDES =============================================================*/

/*==== CONSTANTS ============================================================*/

/*==== TYPES ================================================================*/

/*==== PROTOTYPES ===========================================================*/


/* User buffer queue management.

    This module (currently unimplemented) offers some general queue handling helper functions.

    THIS IS NOT USED FOR COMMUNICATION!!! See module remu_event for that!

    For further information about sending/receiving by queues look at remu_event.c

    Even if implemented, this functions wouldn't  enhance communication  queue handling. They aren't
    used by riviera frame in any way, but by riviera manager to handle lists of unused memory.

    GPF Frame isn't able to bypass a communication-queues order when enqueueing nor can a member
    be dequeued before sending.

*/
 
T_RVF_RET  gsp_enqueue(T_RVF_BUFFER_Q* p_q, void* p_buf);

T_RVF_RET  gsp_enqueue_head(T_RVF_BUFFER_Q* p_q, void* p_buf);

void*    gsp_dequeue (T_RVF_BUFFER_Q* p_q);

T_RVF_BUFFER*  gsp_scan_next(T_RVF_BUFFER_Q*  p_q, T_RVF_BUFFER*  p_buf);

T_RVF_RET  gsp_remove_from_queue(T_RVF_BUFFER_Q*  p_q, T_RVF_BUFFER*  p_buf);

/*==== MACROS ===============================================================*/

#endif /* remu_queue_H */        

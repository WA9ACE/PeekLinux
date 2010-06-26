/*
+------------------------------------------------------------------------------
|  File:       remu_queue.c
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
|  Purpose :  This Modul defines the riviera emulator queue managment.
+-----------------------------------------------------------------------------
*/



#ifndef __REMU_QUEUE_C__
#define __REMU_QUEUE_C__
#endif

/*==== INCLUDES ===================================================*/

#include "remu.h"
#include "../inc/remu_internal.h"
#include "rvf_i.h"
#include "remu_timer.h"


/*==== CONSTANTS ==================================================*/
#define SORRY_NOT_YET_IMPLEMENTED ("blah" == 0)


/*==== TYPES ======================================================*/

/* User buffer queue management.

    This module (currently unimplemented) offers some general queue handling helper functions.

    THIS IS NOT USED FOR SYNCHRONISATION!!! See module remu_event for that!

    For further information about sending/receiving by queues look at remu_event.c

    Even if implemented, this functions wouldn't  enhance communication  queue handling. They aren't
    used by riviera frame in any way, but by riviera manager to handle lists of unused memory.

    GPF Frame isn't able to bypass a communication-queues order when enqueueing nor can a member
    be dequeued before sending.

*/


/*===============================================================*/


/* User buffer queue management*/
/*
+--------------------------------------------------------------------+
NYI
+--------------------------------------------------------------------+
*/
T_RVF_RET  gsp_enqueue(T_RVF_BUFFER_Q* p_q, void* p_buf)
{
    T_REMU_INTERNAL_BUF *p_hdr;
    gsp_get_buf(EXT_MEM_POOL,sizeof(T_REMU_INTERNAL_BUF),(T_RVF_BUFFER**)&p_hdr);
    if (p_hdr==NULL)
		return RVF_MEMORY_ERR;
    p_hdr->p_next=NULL;
    p_hdr->data_ptr=p_buf;
    gsp_disable();	/* enter critical section */

    /* Since the queue is exposed (C vs C++), keep the pointers in exposed format */
	/* p_q->p_last and p_q->p_first point to the user buffer, since p_hdr->p_next points to the T_RVF_INTERNAL_BUF */
    if (p_q->p_first)	/* if the queue is not empty */
    {
		T_REMU_INTERNAL_BUF * p_last_hdr = (T_REMU_INTERNAL_BUF *) ((UINT8 *)p_q->p_last );
        p_last_hdr->p_next = p_hdr;
    }
    else
        p_q->p_first = p_hdr;

    p_q->p_last = p_hdr;
    p_q->count++;
    gsp_enable();		/* exit critical section */
	return RVF_OK;
}

/*
+--------------------------------------------------------------------+
NYI
+--------------------------------------------------------------------+
*/
T_RVF_RET  gsp_enqueue_head(T_RVF_BUFFER_Q* p_q, void* p_buf)
{
  T_RVF_RET returnCode = RVF_OK;
  assert(SORRY_NOT_YET_IMPLEMENTED);
  return returnCode;
}

/*
+--------------------------------------------------------------------+
NYI
+--------------------------------------------------------------------+
*/
void*    gsp_dequeue (T_RVF_BUFFER_Q* p_q)
{
    T_REMU_INTERNAL_BUF    *p_hdr;
    void *data_ptr;

    if (!p_q->count)	/* if the queue is empty */
        return (NULL);

    gsp_disable();	/* enter critical section */

    p_hdr = p_q->p_first ;

    /* Keep buffers such that RVF header is invisible */
    if (p_hdr->p_next)
        p_q->p_first = (UINT8 *)p_hdr->p_next;
    else
    {
        p_q->p_first = NULL;
        p_q->p_last  = NULL;
    }

    p_q->count--;

    p_hdr->p_next = NULL;
    data_ptr=p_hdr->data_ptr;
    gsp_free_buf(p_hdr);
    gsp_enable();		/* exit critical section */
    return (data_ptr);
}

/*
+--------------------------------------------------------------------+
NYI
+--------------------------------------------------------------------+
*/
T_RVF_BUFFER*  gsp_scan_next(T_RVF_BUFFER_Q*  p_q, T_RVF_BUFFER*  p_buf)
{
  T_RVF_BUFFER*  returnBuf = 0;
  assert(SORRY_NOT_YET_IMPLEMENTED);
  return returnBuf;
}

/*
+--------------------------------------------------------------------+
NYI
+--------------------------------------------------------------------+
*/
T_RVF_RET  gsp_remove_from_queue(T_RVF_BUFFER_Q*  p_q, T_RVF_BUFFER*  p_buf)
{
  T_RVF_RET returnCode = RVF_OK;
  assert(SORRY_NOT_YET_IMPLEMENTED);
  return returnCode;
}


/* =============================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found 
*  in the license agreement under which this software has been supplied.
* =========================================================================== */
/**
* @file img_buffer_queue.c
*
* This file implements the buffer related functions for img entity
*
* @path  chipsetsw\services\img\src
*
* @rev  0.2
*
*/
/* ------------------------------------------------------------------------- */
/* =========================================================================
*!
*! Revision History
*! ===================================
*! 31-Jan-2006 Ramesh Anandhi: Initial Release
*! 17-Apr-2006 Ramesh Anandhi; Major Review comments incorporated 
*! 09 - March - 2007 Ramesh, Anandhi; Added buf_addToFront
*!
* ========================================================================= */
#include "img_buffer_queue.h"
#include "common.h"
#include "OMX_Component.h"

/* ========================================================================== */
/** @fn buf_enqueue method is used to push an element to 
* the queue.
*
* @param [in] p_q
*     The queue handle. 
*
* @param [in] p_buf
*     The element to be enqueued.  
*
* @return OMX_ERRORTYPE
*     If the command successfully executes, the return code will be
*     OMX_NoError.  Otherwise the appropriate OMX error will be returned.
*
*  @see  none
*/
/* ========================================================================== */
OMX_ERRORTYPE buf_enqueue(T_BUF_QUEUE *p_q, OMX_PTR p_buf)
{
    OMX_ERRORTYPE ret = OMX_ErrorNone;
    
    if(p_q->nLast < IMG_MAX_QUEUE_ELEMS) {

        p_q->pQueue[p_q->nLast] = p_buf;
        p_q->nLast++;
    }
    else
    ret  = OMX_ErrorOverflow;
    return ret;
    
}

/* ========================================================================== */
/** @fn buf_dequeue method is used to pop an element 
* from the queue.
*
* @param [in] p_q
*     The queue handle. 
*
* @return OMX_PTR
*     The element that was dequeued.   
*
*  @see  none
*/
/* ========================================================================== */
OMX_PTR buf_dequeue(T_BUF_QUEUE *p_q)
{
    OMX_PTR *pBuffer;
    OMX_U8 i = 0;
    
    if(p_q->nLast== 0)
    return (0x0);
    else { 
        
        pBuffer= p_q->pQueue[0];    
        p_q->nLast--;
        while(i < p_q->nLast)
        {
            p_q->pQueue[i] =  p_q->pQueue[i+1];
            i++;
        };
        p_q->pQueue[p_q->nLast] =0x0;        
        return  (pBuffer);
    }        
}

/* ========================================================================== */
/** @fn buf_addToFront method is used to pop an element 
* from the queue.
*
* @param [in] p_q
*     The queue handle. 
*
* @return OMX_PTR
*     The element that was dequeued.   
*
*  @see  none
*/
/* ========================================================================== */
void buf_addToFront(T_BUF_QUEUE *p_q, OMX_PTR p_buf)
{
    OMX_PTR *pBuffer;
    OMX_U8 i =  p_q->nLast;
    if(p_q->nLast < IMG_MAX_QUEUE_ELEMS) 
    {
        while(i > 0)
        p_q->pQueue[i] =  p_q->pQueue[i-1];
        i--;
    }
    p_q->pQueue[0] = p_buf;
    p_q->nLast++;
    return;
}


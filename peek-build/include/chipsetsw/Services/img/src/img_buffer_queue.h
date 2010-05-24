/** 
file img_core.h

handle message 

**/
#include "common.h"
#include "OMX_Types.h"
#include "OMX_Component.h"

#define IMG_MAX_QUEUE_ELEMS 4

typedef struct buf_queue
{
	OMX_PTR pQueue[IMG_MAX_QUEUE_ELEMS];
	OMX_U8	nLast;
	
}T_BUF_QUEUE;

#define IS_BUF_QUEUE_EMPTY(p_q) (p_q.nLast == 0)

/* User buffer queue management*/
OMX_ERRORTYPE	buf_enqueue (T_BUF_QUEUE *p_q, OMX_PTR p_buf);
OMX_PTR          buf_dequeue  (T_BUF_QUEUE *p_q);
void buf_addToFront(T_BUF_QUEUE *p_q, OMX_PTR p_buf);
 

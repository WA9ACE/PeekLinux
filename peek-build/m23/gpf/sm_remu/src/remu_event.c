/*
+------------------------------------------------------------------------------
|  File:       remu_event.c
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
+---------------------------------------------------------------------------------------
|  Purpose :  This Modul defines the riviera emulator event managment and communication queue handling
+---------------------------------------------------------------------------------------
*/


/*
    Original Riviera Queues lacks any names or handles, they are defined by a non-null heading pointer(p_first).

    Riviera queues are constituted in a queue struct. It consists of an element counter and two pointers to the heading and the
    trailing queue Entries, which are in fact the payload sections of riviera messages. The queue sequence linkage is
    done in the riviera messages internal header, which has a "p_next" pointer to the following T_RVF_INTERNAL_BUF.

    4 Riviera Queues per task are stored in a global task ID table array consisting of two pointers per queue,
    refering the first and last element. GSPTaskIdTable[task_id]->OSTaskQFirst[mbox] refers to the internal header of the
    "tid" tasks FIRST message of the "mbox" indexed queue. The 4 queues of a task are called it's "mboxes".

    By convention, mbox 0 is used for normal messages and mbox 1 for high-prioritymessages (rvf_send_priority_msg).

    No other mbox than 0/1 is used by rivieras send_msg mechanisms. Riviera-application tasks may use the two remaing queue entries,
    but it's unclear by now if riviera offers a binding mechanism for that or if it's hard-coded in an application.

    rvf_send_msg() uses a given address id(handle) to write to receiver's queue (mbox) 0, rvf_read_mbox()
    uses it's own task id. So, a queue, as long treated as mbox, is always in possession of the reader, like in GPF-Frame.

    Riviera queues are created on the fly by sending to a specified mbox of a task. Again: here are no explicit bindings on
    riviera frame level, may be on manager level.

    Riviera send/await functions are much like the general purpose enqueue/dequeue functions outlined in module remu_queue,
    but bound to specific mboxes (0,1).  Original rivierra send/await functions do issue/handle a queue event to enable flow
    control / handshake, the enqueue/dequeue functions of module remu_queue does NOT!.

    The (currently unimplemented) functions of module remu_queue shouldn't be mixed with the local ones.

*/



#ifndef __REMU_EVENT_C__
#define __REMU_EVENT_C__
#endif

//#define MULTIPLE_MBOX 1
/*==== INCLUDES ===================================================*/

#include "../inc/remu_internal.h"
#include "rvf_i.h"
#include "rvm_use_id_list.h"


/*==== CONSTANTS ==================================================*/
#define SORRY_NOT_YET_IMPLEMENTED ("blah" == 0)
#define  mbox_non_empty(T,M)   (ComTable[GSPTaskIdTable[T]->Queue[M]].UsedSemCB.sm_semaphore_count != 0)
#define RVM_TRACE_DEBUG_HIGH(string)\
	rvf_send_trace (string,(sizeof(string)-1),NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )

/*==== TYPES ======================================================*/

LONG ObtainSemaphoreCB ( NU_SEMAPHORE *SemCB, ULONG Timeout, USHORT wait_check);
LONG ReleaseSemaphoreCB (NU_SEMAPHORE *SemCB);

/*===============================================================*/


/*
+--------------------------------------------------------------------+
 read from an own mbox
+--------------------------------------------------------------------+
*/
void* gsp_read_mbox (UINT8 mbox)
{
  T_HANDLE entityHandle;

  entityHandle = e_running[os_MyHandle()];
  return gsp_read_addr_mbox((T_RVF_G_ADDR_ID) entityHandle , mbox);
}



/*
+--------------------------------------------------------------------+
  read from a qualified task's mbox
+--------------------------------------------------------------------+
*/
void*    gsp_read_addr_mbox(T_RVF_G_ADDR_ID  task_id, UINT8 mbox)
{
  OS_QDATA OS_Msg;  /* fix for omaps00085721 issue */
  T_HANDLE entityHandle;

  entityHandle = e_running[os_MyHandle()];


  if ((mbox >= 0) && (mbox <= 3))
  {
    if ( mbox_non_empty(task_id, mbox) )
    {
      if ((os_ReceiveFromQueue(entityHandle, GSPTaskIdTable[task_id]->Queue[mbox], &OS_Msg, 0xffffffff)) == OS_OK )
      {
        if ( (OS_Msg.data16 == MSG_PRIMITIVE)  || (OS_Msg.data16 == MSG_SIGNAL))
        {
         return P2D(OS_Msg.ptr); 
        }
	else 
        {
            return ((void *) NULL);
        }
      }
    }
  }
  return NULL;
}


/*
+--------------------------------------------------------------------+
  send an event to an event group
+--------------------------------------------------------------------+
*/
UINT8   gsp_send_event(T_RVF_G_ADDR_ID task_id, UINT16 event)
{
  if ((task_id >= MaxEntities) ||(task_id <= 0) || (!GSPTaskIdTable[task_id]) )
  {
       RVM_TRACE_DEBUG_HIGH("gsp_send_event  Failed: Invalid address id");
	return RV_INVALID_PARAMETER;
  }
  os_SetEvents(GSPTaskIdTable[task_id]->pOSEvtGrp, (unsigned) event);
  return 0;
}

/*
+--------------------------------------------------------------------+
NYI. Does anybody need that pre-read function? Frame does not support it!
+--------------------------------------------------------------------+
*/
T_RVF_BUFFER*  gsp_wait_for_specific_msg(UINT16 msg_code, UINT8 mbox, UINT32 timeout)
{
  T_RVF_BUFFER*  returnBuf = 0;
  assert(SORRY_NOT_YET_IMPLEMENTED);
  return returnBuf;
}

/*
+--------------------------------------------------------------------+
 To send buffers and events to task addr_id. Do the real work.
+--------------------------------------------------------------------+
*/

#ifdef MULTIPLE_MBOX
T_RVF_RET  gsp_send_msg_to_queue (T_RVF_G_ADDR_ID addr_id, void*  msg, UINT8 mbox)
{
  int result;
  T_HANDLE entityHandle;
  T_RVF_RET returnCode = RVF_OK;
  OS_QDATA Msg;

  entityHandle = e_running[os_MyHandle()];

  memset( &Msg, 0, sizeof(Msg) );

  Msg.data16 = MSG_PRIMITIVE;
  Msg.ptr = msg;

  result = os_SendToQueue (entityHandle, GSPTaskIdTable[addr_id]->Queue[mbox], OS_NORMAL, OS_SUSPEND, &Msg);

  if (result == OS_OK)
  {
    returnCode = RVF_OK;
    if(rvf_send_event(addr_id,(UINT16) (EVENT_MASK(mbox)) ) != RVF_OK)
    {
       RVM_TRACE_DEBUG_HIGH("rvf_send_event Failed in gsp_send_msg_to_queue ");
    }
  }
  else
  {
    returnCode = result;
    RVM_TRACE_DEBUG_HIGH("os_SendToQueue Failed in gsp_send_msg_to_queue ");
  }
  return returnCode;
}

/*
+--------------------------------------------------------------------+
 To send buffers and events to task addr_id, mbox 0
+--------------------------------------------------------------------+
*/
T_RVF_RET  gsp_send_msg  (T_RVF_G_ADDR_ID addr_id, void*  msg)
{
  char buf[128] = "";

  sprintf( buf, "msg= %u", msg);
  RVM_TRACE_DEBUG_HIGH(buf);

  ((T_RV_HDR *)msg)->dest_addr_id = addr_id;
  if (GSPTaskIdTable[addr_id])
  {
    return gsp_send_msg_to_queue(addr_id, msg, 0);
  }
  return RVF_INTERNAL_ERR;
}

/*
+--------------------------------------------------------------------+
 To send buffers and events to task addr_id, mbox 1
+--------------------------------------------------------------------+
*/
T_RVF_RET gsp_send_priority_msg (T_RVF_G_ADDR_ID addr_id, void *msg)
{
  if (GSPTaskIdTable[addr_id])
  {
    return gsp_send_msg_to_queue(addr_id, msg, 1);
  }
  return RVF_INTERNAL_ERR;
}

#else

/*
+--------------------------------------------------------------------+
 To send buffers and events to task addr_id, mbox 0
+--------------------------------------------------------------------+
*/


T_RVF_RET  gsp_send_msg   (T_RVF_G_ADDR_ID addr_id, void * p_msg)
{
  T_RVF_RET returnCode = RVF_OK;
  int result;

  if ( (addr_id <= 0) ||  (addr_id >= MaxEntities))
  	{
       RVM_TRACE_DEBUG_HIGH("gsp_send_msg  Failed: Invalid address id");
	return RV_INVALID_PARAMETER;
  	} 	
  
  ((T_RV_HDR *)p_msg)->dest_addr_id = addr_id;
//  result = vsi_c_psend((T_HANDLE)GSPTaskIdTable[addr_id]->com_handle, (T_VOID_STRUCT *) p_msg FILE_LINE_MACRO);
  result = vsi_c_psend((T_HANDLE)addr_id, (T_VOID_STRUCT *) p_msg FILE_LINE_MACRO);
//  result = vsi_c_psend((T_HANDLE)GSPTaskIdTable[addr_id]->Queue[0], (T_VOID_STRUCT *) p_msg FILE_LINE_MACRO);
  if (result == VSI_OK)
  {
    if (!(pf_TaskTable[addr_id].Flags & PASSIVE_BODY)) /* we are talking to an active entity */
    {
      if (rvf_send_event(addr_id,(UINT16) (EVENT_MASK(0))) != RVF_OK) /* propagate an mbox event */
      {
         RVM_TRACE_DEBUG_HIGH("rvf_send_event Failed in gsp_send_msg_to_queue ");
         returnCode = RVF_INTERNAL_ERR;
      }
    }
  }
  else
  {
    returnCode = RVF_INTERNAL_ERR;
    RVM_TRACE_DEBUG_HIGH("os_SendToQueue Failed in gsp_send_msg_to_queue ");
  }
  return returnCode;
}

#endif

/*
+--------------------------------------------------------------------+
wait for a masked event of own group and tell if there is something in the own mbox
+--------------------------------------------------------------------+
*/
UINT16 gsp_wait(UINT16 flag, UINT32 timeout)
{
  T_RVF_G_ADDR_ID rtask = gsp_get_taskid();
  return gsp_evt_wait(rtask, flag, timeout);
}

/*
+--------------------------------------------------------------------+
wait for a masked (mbox) event of a task's event group and tell if there is something
in that task's mbox
+--------------------------------------------------------------------+
*/
#ifdef MULTIPLE_MBOX
UINT16 gsp_evt_wait(T_RVF_G_ADDR_ID task_id, UINT16 flag, UINT32 timeout)
{
  UINT16    mbxEvt = 0;
  UNSIGNED  evt   = 0;
  UNSIGNED  timEvt   = 0;
  UNSIGNED  clear = 0;
  STATUS    status_ret;

  /* Check if anything in any of the mailboxes. Possible race condition. */

  if (task_id >= MaxEntities || !GSPTaskIdTable[task_id])
  /* MAX_ENTITIES -> MAX_RVF_TASKS later on, when clumsy GSPTaskIdTable implementation is improved */
  {
    RVM_TRACE_WARNING_PARAM("RVF: Illegal MBOX or MBOX not ready!", task_id);
    return (UINT16) 0;
  }

  if (GSPTaskIdTable[task_id]->Queue[0])
  {
    if (mbox_non_empty(task_id, 0))
    {
     mbxEvt |= RVF_TASK_MBOX_0_EVT_MASK;
    }
  }
  
  if (GSPTaskIdTable[task_id]->Queue[1])
  {
    if (mbox_non_empty(task_id, 1))
    {
     mbxEvt |= RVF_TASK_MBOX_1_EVT_MASK;
    }
  }
  
  if (GSPTaskIdTable[task_id]->Queue[2])
  {
    if (mbox_non_empty(task_id, 2))
    {
     mbxEvt |= RVF_TASK_MBOX_2_EVT_MASK;
    }
  }
  
  if (GSPTaskIdTable[task_id]->Queue[3])
  {
    if (mbox_non_empty(task_id, 3))
    {
     mbxEvt |= RVF_TASK_MBOX_3_EVT_MASK;
    }
  }
  
  /* If any valid event if pending, return immediately */
  if (mbxEvt & flag)
  {
     /* Return only those bits which user wants... */
     evt = (UINT16) (mbxEvt & flag);

   /* clear the nucleus event(s) for mailboxes */
     if (mbxEvt & 0x000F ) /* a mailbox event is signaled*/
     {
       os_RetrieveEvents(GSPTaskIdTable[task_id]->pOSEvtGrp,
                         ((unsigned) mbxEvt & 0x000F), (char) NU_AND_CONSUME,
                         (unsigned *)&clear, (unsigned)NU_NO_SUSPEND);

     }
     return ((UINT16) evt); /* just tell that a mailbox event occured */
  }

  if (timEvt)
  {
    return ((UINT16) (timEvt & flag)); /* just tell that a timer event occured */
  }
  else
  {
    if (!timeout) timeout = 0xFFFFFFFFL;
    status_ret = os_RetrieveEvents(GSPTaskIdTable[task_id]->pOSEvtGrp,
                                   (unsigned) flag, (char)NU_OR_CONSUME,
                                   (unsigned *)&evt, (unsigned)timeout );

    if ( status_ret == NU_SUCCESS)
    {
      return (UINT16) evt;
    }
  }
  return (UINT16) 0;
}

#else

UINT16 gsp_evt_wait(T_RVF_G_ADDR_ID task_id, UINT16 flag, UINT32 timeout)
{
  UINT16    mbxEvt = 0;
  UNSIGNED  evt   = 0;
  UNSIGNED  timEvt   = 0;
  UNSIGNED  clear = 0;
  STATUS    status_ret;

  /* Check if anything in any of the mailboxes. Possible race condition. */

  if (task_id >= MaxEntities || !GSPTaskIdTable[task_id])
  /* MAX_ENTITIES -> MAX_RVF_TASKS later on, when clumsy GSPTaskIdTable implementation is improved */
  {
    RVM_TRACE_WARNING_PARAM("RVF: Illegal MBOX or MBOX not ready!", task_id);
    return (UINT16) 0;
  }

  if (GSPTaskIdTable[task_id]->Queue[0])
  {
    if (mbox_non_empty(task_id, 0))
    {
     mbxEvt |= RVF_TASK_MBOX_0_EVT_MASK;
    }
  }
  
  /* If any valid event if pending, return immediately */
  if (mbxEvt & flag)
  {
     /* Return only those bits which user wants... */
     evt = (UINT16) (mbxEvt & flag);

   /* clear the nucleus event(s) for mailboxes */
     if (mbxEvt & 0x000F ) /* a mailbox event is signaled*/
     {
       os_RetrieveEvents(GSPTaskIdTable[task_id]->pOSEvtGrp,
                         ((unsigned) mbxEvt & 0x000F), (char) NU_AND_CONSUME,
                         (unsigned *)&clear, (unsigned)NU_NO_SUSPEND);

     }
     return ((UINT16) evt); /* just tell that a mailbox event occured */
  }

  if (timEvt)
  {
    return ((UINT16) (timEvt & flag)); /* just tell that a timer event occured */
  }
  else
  {
    if (!timeout) timeout = 0xFFFFFFFFL;
    status_ret = os_RetrieveEvents(GSPTaskIdTable[task_id]->pOSEvtGrp,
                                   (unsigned) flag, (char)NU_OR_CONSUME,
                                   (unsigned *)&evt, (unsigned)timeout );

    if ( status_ret == NU_SUCCESS)
    {
      return (UINT16) evt;
    }
  }
  return (UINT16) 0;
}

#endif


/*
+------------------------------------------------------------------------------
|  File:       remu_event.h
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
|  Purpose :  Definitions for the riviera emulator events.
+-----------------------------------------------------------------------------
*/

#ifndef REMU_EVENT_H
#define REMU_EVENT_H

/*==== INCLUDES =============================================================*/

/*==== CONSTANTS ============================================================*/

/*==== TYPES ================================================================*/

/*==== PROTOTYPES ===========================================================*/


/* To send buffers and events between tasks*/
void*    gsp_read_mbox (UINT8 mbox);

void*    gsp_read_addr_mbox(T_RVF_G_ADDR_ID  task_id, UINT8 mbox);

UINT8   gsp_send_event(T_RVF_G_ADDR_ID task_id, UINT16 event);

/* specific function */
T_RVF_BUFFER*  gsp_wait_for_specific_msg(UINT16 msg_code, UINT8 mbox, UINT32 timeout);

T_RVF_RET  gsp_send_msg  (T_RVF_G_ADDR_ID addr_id, void*  p_msg);

T_RVF_RET gsp_send_priority_msg (T_RVF_G_ADDR_ID addr_id, void *msg);

UINT16   gsp_evt_wait(T_RVF_G_ADDR_ID rtask, UINT16 flag, UINT32 timeout);


#endif /* remu_event_H */

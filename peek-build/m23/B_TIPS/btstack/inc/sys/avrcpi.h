/****************************************************************************
 *
 * File:
 *     $Workfile:avrcpi.h$ for iAnywhere AV SDK, version 1.4
 *     $Revision:19$
 *
 * Description: This file contains internal definitions for the Audio/Video
 *     Remote Control Profile (AVRCP).
 *             
 * Created:     Mar 11, 2004
 *
 * Copyright 2004 - 2005 Extended Systems, Inc.
 * Portions copyright 2005-2006 iAnywhere Solutions, Inc.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any means, 
 * or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions, 
 * Inc.  This work contains confidential and proprietary information of 
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#ifndef __AVRCPI_H_
#define __AVRCPI_H_

#include "avrcp.h"

/* SDP query info */
extern const U8 AvrcpServiceSearchAttribReq[AVRCP_NUM_SEARCH_ATTRIBUTES];

/* The BtAvrcpContext structure contains the global context of AVDTP. */
typedef struct _BtAvrcpContext {
    SdpRecord       ctSdpRecord;
    SdpAttribute    ctSdpAttrib[AVRCP_NUM_ATTRIBUTES];
    U16             ctChannelCount;
    SdpRecord       tgSdpRecord;
    SdpAttribute    tgSdpAttrib[AVRCP_NUM_ATTRIBUTES];
    U16             tgChannelCount;
} BtAvrcpContext;

#if XA_CONTEXT_PTR == XA_ENABLED
extern BtAvrcpContext *avrcpContext;
#define AVRCP(s) (avrcpContext->s)
#else /* XA_CONTEXT_PTR == XA_ENABLED */
extern BtAvrcpContext avrcpContext;
#define AVRCP(s) (avrcpContext.s)
#endif /* XA_CONTEXT_PTR */

BOOL AvrcpAlloc(void);
BtStatus AvrcpRegisterSdp(AvrcpChannel *chnl);
BtStatus AvrcpDeregisterSdp(AvrcpChannel *chnl);
BtStatus AvrcpStartServiceQuery(AvrcpChannel *chnl, SdpQueryMode mode);
void AvrcpSdpCallback(const BtEvent *event);
void AvrcpReportFailedConnect(AvrcpChannel *chnl);

#define AVRCP_PANEL_STATE_NONE       0

#define AVRCP_PANEL_STATE_C_IDLE     1
#define AVRCP_PANEL_STATE_C_PRESS    2
#define AVRCP_PANEL_STATE_C_PRESS_R  3
#define AVRCP_PANEL_STATE_C_RELEASE  4
#define AVRCP_PANEL_STATE_C_SKIP     5

#define AVRCP_PANEL_STATE_T_IDLE     101
#define AVRCP_PANEL_STATE_T_PRESS    102
#define AVRCP_PANEL_STATE_T_HOLD     103

/* Need an invalid operation code for logic to work properly */
#define AVRCP_POP_INVALID 0x00FF

/* Macros for acting on the Controller's operation queue */
   
/* Returns the length of the op queue */
#define AVRCP_OpQueueLen(c) \
 ((AVRCP_SUBUNIT_OP_QUEUE_MAX + (c)->panel.ct.opWrite - (c)->panel.ct.opRead) \
  % AVRCP_SUBUNIT_OP_QUEUE_MAX)

/* Returns the number of bytes free in the op queue */
#define AVRCP_OpQueueFree(c) \
 (AVRCP_SUBUNIT_OP_QUEUE_MAX - AVRCP_OpQueueLen(c) - 1)

/* Pushes an operation byte onto the back of the op queue */
#define AVRCP_OpQueuePush(c,o)  do { \
 (c)->panel.ct.opQueue[(c)->panel.ct.opWrite++] = o;   \
 (c)->panel.ct.opWrite %= AVRCP_SUBUNIT_OP_QUEUE_MAX; \
 } while(0)

/* Returns the p'th element from the front of the operation queue */
#define AVRCP_OpQueueGet(c,p) \
 (c)->panel.ct.opQueue[((c)->panel.ct.opRead + (p)) % AVRCP_SUBUNIT_OP_QUEUE_MAX]

#if TI_CHANGES == XA_ENABLED
/* Advances the queue to the p'th next element from the top */
#define AVRCP_OpQueueAdvance(c,p) \
 (c)->panel.ct.opRead = (U8)(((c)->panel.ct.opRead + (p)) % AVRCP_SUBUNIT_OP_QUEUE_MAX)
#else
/* Advances the queue to the p'th next element from the top */
#define AVRCP_OpQueueAdvance(c,p) \
 (c)->panel.ct.opRead = ((c)->panel.ct.opRead + (p)) % AVRCP_SUBUNIT_OP_QUEUE_MAX
#endif /* TI_CHANGES == XA_ENABLED */
#define AVRCP_POP_RELEASE 0x0080

#endif /* __AVRCPI_H_ */


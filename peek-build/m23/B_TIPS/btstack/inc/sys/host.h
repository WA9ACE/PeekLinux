#ifndef __HOST_H
#define __HOST_H

/***************************************************************************
 *
 * File:
 *     $Workfile:host.h$ for iAnywhere Blue SDK, Version 2.1.2
 *     $Revision:67$
 *
 * Description:
 *     This file contains internal definitions for the Host
 *     Controller Interface.
 *
 * Created:
 *     September 20, 1999
 *
 * Copyright 1999-2005 Extended Systems, Inc.
 * Portions copyright 2005-2006 iAnywhere Solutions, Inc.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions, 
 * Inc.  This work contains confidential and proprietary information of 
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#include "eventmgr.h"
#include "sys/hci.h"
#include "sys/rxbuff.h"
#include "hcitrans.h"
#include "prescan.h"

/*--------------------------------------------------------------------------*
 * Typedefs and Data structures used internally by HCI                      *
 *--------------------------------------------------------------------------*/

/* SCO Support in HCI */
#define HCI_SCO_ENABLED ((NUM_SCO_CONNS != 0) && (BT_SCO_HCI_DATA == XA_ENABLED))

/* HCI States */
#define HCI_STATE_SHUTDOWN        0
#define HCI_STATE_INIT            1
#define HCI_STATE_DEINIT          2
#define HCI_STATE_READY           4

/* HCI Flags */
#define HCI_FLAG_DATA_QUEUED      0x01
#define HCI_FLAG_PACKETS_COMPLETE 0x02
#define HCI_FLAG_ACTIVE_BC        0x04
#define HCI_FLAG_PICONET_BC       0x08

/* HCI Transmit states */
#define HCI_TX_STATE_SEND_HEADER  0
#define HCI_TX_STATE_SEND_DATA    1
#define HCI_TX_STATE_SEND_TAIL    2
#define HCI_TX_STATE_SEND_FCS     3

/* HCI Connection Flags */
typedef U8 HciConFlags;

#define HCI_ACTIVE_CON         0x01
#define HCI_DATA_REQ           0x02
#define HCI_DATA_TYPE_ACL      0x04
#define HCI_DATA_TYPE_SCO      0x08

/* HCI Connection Table */
typedef struct _HciConnection {
    HciHandle     hciHandle;
    HciConFlags   conFlags;
#if (TI_CHANGES == XA_ENABLED && HCI_QOS == XA_ENABLED)
	U16			  devTableIndex;
#endif /* TI_CHANGES == XA_ENABLED && HCI_QOS == XA_ENABLED */

    /* Transmit state variables */
    ListEntry     btPacketList;
    BtPacket     *btPacket;
    U16           remaining;
    U16           fragRemain;
    const U8     *buffPtr;
    U8            txState;

    /* Flow control variables */
    U16           packetsSent;
    
#if HCI_HOST_FLOW_CONTROL == XA_ENABLED
    U16           packetsComplete;
#endif /* HCI_HOST_FLOW_CONTROL == XA_ENABLED */

#if ((TI_CHANGES == XA_ENABLED) && (L2CAP_PACKET_PRIORITIZATION == XA_ENABLED))
		U8 						priority;
#endif
} HciConnection;

/* HCI Context */
typedef struct _BtHciContext {

    /* HCI Queues */
    ListEntry       rxBuffList;        /* Received HCI buffers         */
    ListEntry       sentPacketList;    /* Handled HCI TX packets       */
    ListEntry       commandList;       /* Commands queued for transmit */
    ListEntry       deferredList;      /* Commands queued during init  */
    ListEntry       sentCommandList;   /* Commands sent to the radio   */
    ListEntry       hciPacketPool;     /* Avaialable HCI TX packets    */

    /* HCI Transmit and flow control variables */
    HciPacket       hciPackets[HCI_NUM_PACKETS + HCI_NUM_COMMANDS]; 
    U16             hciPacketsLeft;
    U16             aclTxPacketLen;
    U16             aclTxPacketsLeft;
    U8              scoTxPacketLen;
    U16             scoTxPacketsLeft;
#if HCI_HOST_FLOW_CONTROL == XA_ENABLED
    U16             aclRxPacketsLeft;

#if HCI_SCO_ENABLED && (HCI_SCO_FLOW_CONTROL == XA_ENABLED)
    U16             scoRxPacketsLeft;
#endif /* HCI_SCO_ENABLED */

#endif /* HCI_HOST_FLOW_CONTROL == XA_ENABLED */
    
    /* Internal command packet */
    HciCommand      cmd;

    /* Command flow control variable */
    U8              maxCommands;

    /* HCI connection table and state */
    HciConnection   con[HCI_NUM_HANDLES];
    U16             activeAclCons;
#if ((TI_CHANGES == XA_ENABLED) && (L2CAP_PACKET_PRIORITIZATION == XA_ENABLED))
		U16							curTxConHighPriority;
		U16							curTxConLowPriority;
#else
    U16             txCon;
#endif
		

    /* Other HCI state variables */
    U8              state;
    U8              flags;
    U8              conReqs;
    U8              transportError;

    /* Dynamic broadcast handles */
    HciHandle       piconetBcHandle;
    HciHandle       activeBcHandle;

#if HCI_ALLOW_PRESCAN == XA_ENABLED
    PrescanContext  psc;
#endif /* HCI_ALLOW_PRESCAN == XA_ENABLED */
    
    /* Reset delay and retry timers */
    EvmTimer        resetTimer;
    EvmTimer        retryTimer;

    /* Transport driver entry points */
    TranEntry       tranEntry;

    /* Radio handler entry point */
    RadioHandlerCallback radioEntry;

} BtHciContext;

/*--------------------------------------------------------------------------*
 * Functions used internally by the driver                                  *
 *--------------------------------------------------------------------------*/

/* Internal references to HCI entry points */
void HciTransportError(void);
void HciBufferInd(HciBuffer *buffer, BtStatus status);
void HciPacketSent(HciPacket *packet);
BtStatus HciSendCommand(U16 opCode, U8 parmLen, HciCommand *cmd, BOOL internal);

/* HCI utility functions */
U16 HciGetTableEntry(HciHandle hciHandle);
HciHandle HciAddConnection(HciHandle hciHandle, U8 linkType);
BtStatus HciDeleteConnection(HciHandle hciHandle);
void HciSetNewBcHandle(HciHandle *handle);
U8 HCI_GetState(void);
#define HCI_GetState() (HCC(state))

/* HCI event processing functions */
void HciProcessEvent(HciBuffer *hciBuffer);

/* Prescan initialization  */
void HciInitPrescan(void);

#endif /* __HOST_H */

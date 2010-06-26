/****************************************************************************
 *
 * File:
 *     $Workfile:avdtpcon.h$ for iAnywhere AV SDK, version 1.4
 *     $Revision:19$
 *
 * Description: This file contains the header file for the L2CAP/ACL 
 *              connection state machine for AVDTP connections.
 *             
 * Created:     Mar 31, 2004
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

#ifndef __AVDTPCON_H_
#define __AVDTPCON_H_

#include "l2cap.h"

/* Connection Flags */
#define AVDTP_FLAG_LINK_ACL       0x01

/* Conn Events */
#define AVDTP_CONN_EVENT_CONNECT     1
#define AVDTP_CONN_EVENT_CONNECT_IND 2
#define AVDTP_CONN_EVENT_DISCONNECT  3
#define AVDTP_CONN_EVENT_DATA_SENT   4
#define AVDTP_CONN_EVENT_DATA_IND    5

/* Connection States */
#define AVDTP_CONN_NUM_STATES     6

#define AVDTP_STATE_DISCONNECTED   0
#define AVDTP_STATE_CONN_PENDING   1
#define AVDTP_STATE_CONN_INCOMING  2
#define AVDTP_STATE_DISC_PENDING   3
#define AVDTP_STATE_DISC_INCOMING  4
#define AVDTP_STATE_CONNECTED      5

/* Forward references */
typedef struct _AvdtpConnCallbackParms AvdtpConnCallbackParms;
typedef struct _AvdtpConn AvdtpConn;

/* State machine function prototype */                     
typedef void (*AvdtpConnState)(AvdtpConn *Conn, L2capCallbackParms *Parms);

/* Conn callback function */
typedef void (*AvdtpConnCallback)(AvdtpConn *Conn, AvdtpConnCallbackParms *Parms);

/* Conn calback parameters */
struct _AvdtpConnCallbackParms {
    U8         event;
    BtStatus   status;
    U16        dataLen;
    union {
        BtRemoteDevice *remDev;
        U8             *data;
        BtPacket       *packet;
    } ptrs;
};

/* Connection State */
struct _AvdtpConn {
    L2capChannelId    l2ChannelId;
    U8                state;
    AvdtpConnCallback callback;
    BtRemoteDevice   *remDev;
};

/* State machine functions */
void AvdtpConnInitStateMachine(void);
void AvL2Callback(L2capChannelId L2ChannelId, L2capCallbackParms *Parms);

/* Function Prototypes */
void AvdtpInitConn(AvdtpConn *Conn, AvdtpConnCallback Callback);
BtStatus AvdtpConnect(AvdtpConn *Conn, BtRemoteDevice *RemDev, BOOL reliable);
BtStatus AvdtpConnectRsp(AvdtpConn *Conn, BOOL Accept);
BtStatus AvdtpDisconnect(AvdtpConn *Conn);
BOOL AvdtpIsConnected(AvdtpConn *Conn);

#endif /* __AVDTPCON_H_ */


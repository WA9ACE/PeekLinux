/****************************************************************************
 *
 * File:
 *     $Workfile:hfgi.h$ for iAnywhere Blue SDK, Version 2.1.2
 *     $Revision:13$
 *
 * Description: This file contains the internal function prototypes and data
 *              structures for the hands free profile
 *             
 * Created:     March 15, 2005
 *
 * Copyright 2001-2005 Extended Systems, Inc.
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

#ifndef __HFGI_H_
#define __HFGI_H_

#include "hfg.h"

/* Channel States */
#define HFG_STATE_CLOSED         0
#define HFG_STATE_CONN_PENDING   1
#define HFG_STATE_CONN_INCOMING  2
#define HFG_STATE_OPEN           3

/* Link Flags */
#define HFG_LINK_ACL              0x01
#define HFG_LINK_HANDSFREE        0x08
#define HFG_LINK_REMOVE_HF        0x10
#define HFG_LINK_HS_CONNECTING    0x20
#define HFG_LINK_HS_DISCONNECTING 0x40
#define HFG_LINK_REM_DISC         0x80

/* Internal State Machine Events */
#define EVENT_OPEN_IND         0x80
#define EVENT_OPEN             0x81
#define EVENT_AT_DATA          0x82
#define EVENT_CLOSED           0x83
#define EVENT_RF_PACKET_SENT   0x85

/* Channel Flags */
#define CHANNEL_FLAG_OUTGOING            0x0001
#define CHANNEL_FLAG_TX_IN_PROGRESS      0x0002
#define CHANNEL_FLAG_SEND_OK             0x0004
#define CHANNEL_FLAG_NEGOTIATE           0x0008
#define CHANNEL_FLAG_NEG_DONE            0x0010
#define CHANNEL_FLAG_FEATURES_SENT       0x0020
#define CHANNEL_FLAG_SNIFFING            0x0040

/* SDP Query States */
#define SDP_QUERY_FLAG_PROTOCOL  0x01
#define SDP_QUERY_FLAG_PROFILE   0x02
#define SDP_QUERY_FLAG_FEATURES  0x08
#define SDP_QUERY_FLAG_HF        0x10

/** Indicators */
#define AT_IND_CALL             0
#define AT_IND_SIGNAL           1
#define AT_IND_SERVICE          2
#define AT_IND_ROAM             3
#define AT_IND_SMS              4
#define AT_IND_BATTCHG          5
#define AT_IND_SETUP            6
#define AT_IND_CALLHELD         7

/* Unknown AT data received */
#define AT_UNKNOWN 0xFF

/* State machine function prototype */                     
typedef void (*HfgState)(HfgChannel *Channel, U8 Event, AtCommands *Command);

/* General utility functions */
void HfgCloseChannel(HfgChannel *Channel);
BtStatus HfgParseRfcommData(HfgChannel *channel, RfCallbackParms *parms, U16 *offset, AtCommands *atParms);
void HfgAppCallback(HfgChannel *Channel, HfgEvent Event, BtStatus Status, U32 data);
HfgChannel * HfgFindChannel(BtRemoteDevice *remDev);
HfgChannel * HfgGetClosedChannel(void);

/* SDP utility functions */
BtStatus HfgRegisterSdpServices(HfgChannel *Channel);
BtStatus HfgDeregisterSdpServices(void);
BtStatus HfgStartServiceQuery(HfgChannel *Channel, SdpQueryMode mode);
BtStatus HfgVerifySdpQueryRsp(HfgChannel *Channel, SdpQueryToken *token);
void HfgReportServiceConnected(HfgChannel *Channel);
void HfgSetupIndicatorTestRsp(HfgChannel *Channel);
void HfgSetupIndicatorReadRsp(HfgChannel *Channel);
void HfgSetupCallHoldReadRsp(HfgChannel *Channel);

/* Handsfree functions */
void HfgRfCallback(RfChannel *Channel, RfCallbackParms *Parms);
void HfgInitStateMachine(void);

/* At send functions */
BtStatus AtSendResults(HfgChannel *Channel, AtResults *Results);
BtStatus HfgAtSendRfPacket(HfgChannel *Channel, BtPacket *Packet);
BtStatus AtSendComplete(HfgChannel *Channel, BtPacket *Packet);
void AtParseCommand(HfgChannel *Channel, U8 *input, 
                    U16 inputLen, AtCommands *Command);

#endif /* __HFGI_H_ */


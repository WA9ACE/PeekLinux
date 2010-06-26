/*******************************************************************************\
*                                                                           	*
*  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION			*
*																				*
*  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE			*
*	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE		*
*	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO		*
*	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT		*
*	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL			*
*	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.			*
*																				*
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      hsgi.h
*
*   DESCRIPTION:    This file declares internal functions and constants
*					for the headset audio gateway.
*
*   AUTHOR:         Itay Klein.
*
\*******************************************************************************/

#ifndef __HSGI_H_
#define __HSGI_H_

#include <hsg.h>

/* Channel States */
#define HSG_STATE_CLOSED         0
#define HSG_STATE_CONN_PENDING   1
#define HSG_STATE_CONN_INCOMING  2
#define HSG_STATE_OPEN           3

/* Link Flags */
#define HSG_LINK_ACL			0x01
#define HSG_LINK_HEADSET		0x08
#define HSG_LINK_REMOVE_HEADSET		0x10
#define HSG_LINK_HS_CONNECTING		0x20
#define HSG_LINK_HS_DISCONNECTING	0x40
#define HSG_LINK_REM_DISC		0x80

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
#define SDP_QUERY_FLAG_HS        0x10

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
typedef void (*HsgState)(HsgChannel *Channel, U8 Event, AtCommands *Command);

/* General utility functions */
void HsgCloseChannel(HsgChannel *Channel);
BtStatus HsgParseRfcommData(HsgChannel *channel, RfCallbackParms *parms, U16 *offset, AtCommands *atParms);
void HsgAppCallback(HsgChannel *channel, HsgEvent Event, BtStatus Status, U32 data);
HsgChannel* HsgFindChannel(BtRemoteDevice *remDev);
HsgChannel* HsgGetClosedChannel(void);

/* SDP utility functions */
BtStatus HsgRegisterSdpServices(HsgChannel *Channel);
BtStatus HsgDeregisterSdpServices(void);
BtStatus HsgStartServiceQuery(HsgChannel *Channel, SdpQueryMode mode);
BtStatus HsgVerifySdpQueryRsp(HsgChannel *Channel, SdpQueryToken *token);
void HsgReportServiceConnected(HsgChannel *Channel);

/* Headset Audio gateway functions */
void HsgRfCallback(RfChannel *channel, RfCallbackParms *parms);
void HsgInitStateMachine(void);

/* At send functions */
BtStatus HsgAtSendResults(HsgChannel *channel, AtResults *results);
BtStatus HsgAtSendRfPacket(HsgChannel *channel, BtPacket *packet);
BtStatus HsgAtSendComplete(HsgChannel *channel, BtPacket *packet);
void HsgAtParseCommand(HsgChannel *Channel, U8 *input, 
                    U16 inputLen, AtCommands *Command);
#endif /* __HSGI_H_ */


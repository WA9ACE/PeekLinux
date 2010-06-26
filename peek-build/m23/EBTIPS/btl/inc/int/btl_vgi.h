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
*   FILE NAME:		btl_vgi.h
*
*   DESCRIPTION:	Internal VG Header file for sharing internal API
*
*   AUTHOR:			Itay Klein
*
\*******************************************************************************/

#ifndef __BTL_VGI_H
#define __BTL_VGI_H

/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/

#include "hsg.h"
#include "hfg.h"
#include "btl_pool.h"
#include "btl_vg.h"
#include "bthal_mc.h"
#include "bthal_vc.h"

/********************************************************************************
 *
 * Constants
 *
 *******************************************************************************/

/*------------------------------------------------------------------------------
 * BTL_VG_MAXIMUM_NUMBER_OF_CHANNELS constant
 *
 *	The maximal number of available VG Channels.
 *	Generaly more then one connections is not possible,
 *	only during a handover 2 connections exist.
 */
#define BTL_VG_MAXIMUM_NUMBER_OF_CHANNELS 2

/*------------------------------------------------------------------------------
 * BTL_VG_MAXIMUM_NUMBER_OF_AVAILABLE_RESPONSES constant
 *
 *	The maximal number of available structures of type "AtResults",
  *	this structure is used for sending AT results and responses.
 */
#define BTL_VG_MAXIMUM_NUMBER_OF_AVAILABLE_RESPONSES	30

/*------------------------------------------------------------------------------
 * BTL_VG_MAXIMUM_NUMBER_OF_AVAILABLE_INCOMING_EVENTS constant
 *
 *	the maximal number of "BtlVgIncomingEvent" available for allocation.
 *
 *	TODO:	Reduce the amount of BtlVgIncomingEvent structued allocated
 *			This number is used because when registering within BTHAL MC
 *			for every HFG indicator an event is sent to the newly registered user.
 */
#define BTL_VG_MAXIMUM_NUMBER_OF_AVAILABLE_INCOMING_EVENTS	10

/*------------------------------------------------------------------------------
 * BTL_VG_MAXIMUM_NUMBER_OF_ME_COMMAND_TOKENS constant
 *
 *	the maximal number of "MeCommandToken" available for allocation within the VG.
 */
#define BTL_VG_MAXIMUM_NUMBER_OF_ME_COMMAND_TOKENS	2

/*------------------------------------------------------------------------------
 * BTL_VG_MAXIMUM_NUMBER_OF_BTL_VG_STRINGS constant
 *
 *	the maximal number of "BtlVgString" available for allocation within the VG.
 */
#define BTL_VG_MAXIMUM_NUMBER_OF_BTL_VG_STRINGS		30

/*------------------------------------------------------------------------------
 * BTL_VG_MAX_STR_LEN Constant
 *
 *	The maximal string length in the BTL VG Module.
 *	Strings are used for AT commands, AT results, phone numbers, and etc.
 */
#define BTL_VG_MAX_STR_LEN		60

/*------------------------------------------------------------------------------
 * BTL_VG_SECURITY_SETTINGS Constant
 *
 *	The default security level of the VG Module.
 */
#define BTL_VG_SECURITY_SETTINGS	BSL_DEFAULT

/*------------------------------------------------------------------------------
 * BTL_VG_NUM_OF_CONTEXT_SM_STATES Constant
 *
 *	The number of states in the BtlVgContext state machine
 */
#define BTL_VG_NUM_OF_CONTEXT_SM_STATES	3

/*------------------------------------------------------------------------------
 * BTL_VG_NUM_OF_INCOMING_EVENTS_SOURCES Constant
 *
 *	The number of sources for incoming events
 */
#define BTL_VG_NUM_OF_INCOMING_EVENTS_SOURCES	4

/*------------------------------------------------------------------------------
 * BTL_VG_NUM_OF_HANDSFREE_INDICATORS Constant
 *
 *	The number of HF Indicators type that can be sent to the HF Unit.
 */
#define BTL_VG_NUM_OF_HANDSFREE_INDICATORS 8

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/
/*-------------------------------------------------------------------------------
 * BtlVgEventSource type
 */
typedef U32 BtlVgEventSource;

/* Event received from the BlueSDK module HFG
 */
#define BTL_VG_EVENT_SOURCE_HFG		0

/* Event received from the BlueSDK module HSG
 */
#define BTL_VG_EVENT_SOURCE_HSG		1

/* Event received from the BTHAL module MC
 */
#define BTL_VG_EVENT_SOURCE_MC		2

/* Event received from the BTHAL module VC
 */
#define BTL_VG_EVENT_SOURCE_VC		3

/*-------------------------------------------------------------------------------
 * BtlVgModuleState type
 *
 *	BtlVgModuleState represents different states of the VG Module.
 */
typedef enum _BtlVgModuleState
{
	BTL_VG_MODULE_STATE_NOT_INITIALIZED,
	BTL_VG_MODULE_STATE_INITIALIZED
}BtlVgModuleState;


/*-------------------------------------------------------------------------------
 * BtlVgContextState type
 *
 *	BtlVgContextState represents different states of the BTL VG Context.
 */
typedef enum _BtlVgContextState
{
	BTL_VG_CONTEXT_STATE_DISABLED,		/* The context is disabled; Call BTL_VG_Enable() to enable it. */
	BTL_VG_CONTEXT_STATE_DISABLING,	/* The context is being enabled */
	BTL_VG_CONTEXT_STATE_ENABLED,		/* The contet is enabled and Ready to accpet incoming/outgoing connections. */
	BTL_VG_CONTEXT_STATE_DESTROYED	/* The context destroyed call BTL_VG_Create to create it  */
}BtlVgContextState;

/*-------------------------------------------------------------------------------
 * BtlVgChannelState type
 *
 *	BtlVgChannelState represents different states of a BTL VG Channel.
 */
typedef enum _BtlVgChannelState
{
	BTL_VG_CHANNEL_STATE_UNKNOWN,
	BTL_VG_CHANNEL_STATE_IDLE,
	BTL_VG_CHANNEL_STATE_CHECKING_SUPPORTED_SERVICES,
	BTL_VG_CHANNEL_STATE_CONNECTING,
	BTL_VG_CHANNEL_STATE_DISCONNECTING,
	BTL_VG_CHANNEL_STATE_CONNECTED,
	BTL_VG_CHANNEL_STATE_CONNECTING_AUDIO,
	BTL_VG_CHANNEL_STATE_DISCONNECTING_AUDIO,
	BTL_VG_CHANNEL_STATE_AUDIO_CONNECTED
} BtlVgChannelState;

/*-------------------------------------------------------------------------------
 * BtlVgChannelType type
 *
 *	A BtlVgChannel type.
 */
typedef U32 BtlVgChannelType;

#define BTL_VG_UNDEFINED_CHANNEL 0xffffffff

#define BTL_VG_HANDSFREE_CHANNEL	0x00000001

#define BTL_VG_HEADSET_CHANNEL	0x00000002

/*-------------------------------------------------------------------------------
 * BtlVgHeadsetCallState type
 *
 *	Holding the current call state for a Headset connection.
 */
typedef enum _BtlVgHeadsetCallState
{
	BTL_VG_HEADSET_CALL_STATE_NOCALL,
	BTL_VG_HEADSET_CALL_STATE_OUTGOING,
	BTL_VG_HEADSET_CALL_STATE_INCOMING,
	BTL_VG_HEADSET_CALL_STATE_ACTIVE
} BtlVgHeadsetCallState;

/*-------------------------------------------------------------------------------
 * BtlVgCccConnState type
 *     
 *     State of connection establishing used in possible cancellation of
 *     creating connection
 */
typedef enum _BtlVgCccConnState
{
	BTL_VG_CCC_CONN_STATE_NONE,
	BTL_VG_CCC_CONN_STATE_CONNECTING_ACL,
	BTL_VG_CCC_CONN_STATE_SDP_QUERY,
	BTL_VG_CCC_CONN_STATE_CONNECTING
} BtlVgCccConnState;

/* Forward declaration */
typedef struct _BtlVgIncomingEvent BtlVgIncomingEvent;

/*-------------------------------------------------------------------------------
 * BtlVgSmFunction type
 *
 *	The function prototype used in the BTL VG Context state machine.
 */
typedef void (*BtlVgSmFunction)(BtlVgIncomingEvent *event);

/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BtlVgString structure
 *
 *	A structure for a VG Strings, used for AT Commands.
 */
typedef struct _BtlVgString
{
	char		string[BTL_VG_MAX_STR_LEN];

	U32		length;

} BtlVgString;

/*-------------------------------------------------------------------------------
 * BtlVgHfgEvent structure
 */
typedef struct _BtlVgHfgEvent
{
	HfgEvent    type;
	BtStatus    status;
	BtErrorCode errCode;
	HfgChannel *channel;

	union
	{
		BtRemoteDevice		*remDev;
		AtAgFeatures			features;
		BOOL				enabled;
		BtlVgString			phoneNumber;
		BtlVgString			memory;        
		U32					dtmf;
		U32					gain;
		HfgHold				hold;
		HfgHandsFreeVersion	version;
		HfgResponse			*response;
		BtlVgString			atString;

#if HFG_USE_RESP_HOLD == XA_ENABLED
		HfgResponseHold			respHold;
#endif
		HfgPhonebookRead		phonebookRead;
		BtlVgString			phonebookFindString;
		BtlVgString			charSetType;
		AtPbStorageType		phonebookStorage;
	}p;

}BtlVgHfgEvent;

/*-------------------------------------------------------------------------------
 * BtlVgHsgEvent structure
 */
typedef struct _BtlVgHsgEvent
{
	HsgEvent	type;
	BtStatus	status;
	BtErrorCode	errCode;
	HsgChannel *channel;

	union
	{
		BtRemoteDevice	*remDev;
		U32				gain;
		HsgResponse		*response;
		BtlVgString		atString;
	}p;

}BtlVgHsgEvent;

/*-------------------------------------------------------------------------------
 * BtlVgConnectionHandler structure
 *
 *	This connection handler is used to establish a connection to a remote device and query its SDP data
 *	base to know if HFP or HSP are supported and establish the appropriate connection (HS or HF).
 */
 typedef struct _BtlVgConnectionHandler
 {
	CmgrHandler cmgrHandler;			/* For establishing an ACL for an SDP Query. */

	SdpQueryToken sdpQueryToken;		/* For making an SDP Query */

	BtlVgChannelType channelTypeMask;	/* Used to indicates what kind of Audio profiles (HS or HF) 
											are supported by a remote device */
 }BtlVgConnectionHandler;


/*-------------------------------------------------------------------------------
 * BtlVgChannel structure
 */
typedef struct _BtlVgChannel
{
	BOOL audioRequested;

	BtlVgChannelState state;

	/* Flag and connection establishing state used for cancellation */
	BOOL cancelCreateConn;
	BtlVgCccConnState cccConnState;

	BtlVgChannelType type;

	union
	{
		HfgChannel *handsfreeChannel;

		HsgChannel *headsetChannel;

		void *channel;
	}c;

	BtlVgConnectionHandler connectionHandler;

	CmgrAudioParms audioParms;

	BtScoAudioSettings audioSettings;

	BOOL pendingBthalVcCommand;
    
	/* Hands-free event was received, appropriate action was initiated using
	 * BTHAL MC or BTHAL VC, and result is pending */
	U16 bthalMcCommandId;                
	U16 bthalVcCommandId;                

	/* Used to mark indicators changed during establishing of HFP SLC and need
	 * to be updated */
	U8 handsfreeIndicatorsUpdate;                
    
}BtlVgChannel;

/*-------------------------------------------------------------------------------
 * BtlVgIncomingEvent structure
 */
struct _BtlVgIncomingEvent
{
	ListEntry node;

	BtlVgEventSource source;

	BtlVgChannel *channel;

	union
	{
		BthalMcEvent mcEvent;

		BthalVcEvent vcEvent;

		BtlVgHfgEvent hfgEvent;

		BtlVgHsgEvent hsgEvent;
	}e;

};

/*-------------------------------------------------------------------------------
 * BtlVgContext structure
 */
struct _BtlVgContext
{
	BtlContext base;

	BtlVgCallback callback;

	BtlVgContextState state;

	BOOL handover;

	BOOL handoverReject;

	BtlVgChannel *handoverSource;

	BtlVgChannel *handoverTarget;

	BtlVgChannel *handoverRequest;

	BtlVgChannel channels[BTL_VG_MAXIMUM_NUMBER_OF_CHANNELS];

	HfgChannel hfgChannels[BTL_HFAG_MAX_CHANNELS];

	HsgChannel hsgChannels[BTL_HSAG_MAX_CHANNELS];

	U32 numChannels;

	BtlVgSmFunction stateMachine[BTL_VG_NUM_OF_CONTEXT_SM_STATES][BTL_VG_NUM_OF_INCOMING_EVENTS_SOURCES];

	BthalMcContext *modem;

	BthalVcContext *voice;

	BtlVgHeadsetCallState hsCallState;

	HfgIndicator indicators[BTL_VG_NUM_OF_HANDSFREE_INDICATORS];

};

/*-------------------------------------------------------------------------------
 * BtlVgModuleData structure
 */
typedef struct _BtlVgModuleData
{
	/* The current state of the VG Module */
	BtlVgModuleState state;

	/* The response pool name */
	const char *responsesPoolName;

	/* Incoming events pool name */
	const char *incomingEventsPoolName;

	/* "MeCommandToken" pool name */
	const char *commandTokensPoolName;

	/* "BtlVgString" pool name */
	const char *btlVgStringsPoolName;

	BTL_POOL_DECLARE_POOL(responsesPool, responsesMemory, BTL_VG_MAXIMUM_NUMBER_OF_AVAILABLE_RESPONSES, sizeof(AtResults));

	BTL_POOL_DECLARE_POOL(incomingEventsPool, incomingEventsMemory, BTL_VG_MAXIMUM_NUMBER_OF_AVAILABLE_INCOMING_EVENTS, sizeof(BtlVgIncomingEvent));

	BTL_POOL_DECLARE_POOL(commandTokensPool, commandTokensMemory, BTL_VG_MAXIMUM_NUMBER_OF_ME_COMMAND_TOKENS, sizeof(MeCommandToken));

	BTL_POOL_DECLARE_POOL(btlVgStringsPool, btlVgStringsMemory, BTL_VG_MAXIMUM_NUMBER_OF_BTL_VG_STRINGS, sizeof(BtlVgString));

	ListEntry	incomingEventsQue;

	BtlVgContext context;

	/* Indicates whether RADIO OFF notification was received */
	BOOL isRadioOffNotified;

	/* Hands-free event was received, appropriate action was initiated using
	 * BTHAL MC or BTHAL VC, and result is pending. In order to send response only
	 * to channel, which had sent appropriate command, identifiers of commands and
	 * identifiers of responses are stored in members below (0 is invalid value). */
	U16 bthalMcCommandId;                
	U16 bthalMcResponseId;                
	U16 bthalVcCommandId;                
	U16 bthalVcResponseId;                
    
} BtlVgModuleData;

/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/
/*-------------------------------------------------------------------------------
 * voiceGatewayModuleData global
 */
extern BtlVgModuleData voiceGatewayModuleData;

/********************************************************************************
 *
 * Function Reference
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BtlVgSmInitStateMachine()
 */
void BtlVgSmInitStateMachine(void);

/*-------------------------------------------------------------------------------
 * BtlVgUtilsInit()
 */
BtStatus BtlVgUtilsInit(void);

/*-------------------------------------------------------------------------------
 * BtlVgUtilsDeinit()
 */
BtStatus BtlVgUtilsDeinit(void);

/*-------------------------------------------------------------------------------
 * BtlVgUtilsCreate()
 */
BtStatus BtlVgUtilsCreate(BtlAppHandle *handle,
						  BtlVgCallback callback,
						  const BtSecurityLevel *secLevel);

/*-------------------------------------------------------------------------------
 * BtlVgUtilsDestroy()
 */
BtStatus BtlVgUtilsDestroy(void);

/*-------------------------------------------------------------------------------
 * BtlVgUtilsChannelIdToChannel()
 *
 *	Returns the VG Channel with the given channelId.
 */
BtlVgChannel* BtlVgUtilsChannelIdToChannel(BtlVgChannelId channelId);

/*-----------------------------------------------------------------------------------
 * BtlVgUtilsGetChannelIdFromChannel()
 */
BtlVgChannelId BtlVgUtilsChannelToChannelId(const BtlVgChannel *channel);

/*-----------------------------------------------------------------------------------
 * BtlVgUtilsGetChannelType()
 */
BtlVgChannelType BtlVgUtilsGetChannelType(const BtlVgChannel *channel);

/*-----------------------------------------------------------------------------------
 * BtlVgUtilsGetDeviceAddress()
 *
 *	Get the BD Address of the remote device connected via the VG Channel.
 */
BD_ADDR* BtlVgUtilsGetDeviceAddress(BtlVgChannel *channel);

/*-----------------------------------------------------------------------------------
 * BtlVgUtilsGetCmgrHandler()
 *
 *	Get to connection manager handler associated with the VG Channel.
 */
CmgrHandler* BtlVgUtilsGetCmgrHandler(BtlVgChannel *channel);

/*-------------------------------------------------------------------------------
 * BtlVgUtilsSendResults()
 *
 *	Used to send AT Results or Unsolicited result code to remote audio units.
 *	Reffer to HFG_SendHfResults or HSG_SendHsResults for arguments description.
 */
BtStatus BtlVgUtilsSendResults(BtlVgChannel *channel,
						  AtCommand command,
						  U32 parms,
						  U16 parmLen);

/*----------------------------------------------------------------------------------
 * BtlVgUtilsSendEventToUser()
 *
 *	Notifies the VG User of VG Events.
 */
void BtlVgUtilsSendEventToUser(BtlVgChannel *channel,
						  BtlVgEventType,
						  BtStatus status,
						  BtErrorCode errCode,
						  const void *data);

/*----------------------------------------------------------------------------------
 * BtlVgUtilsHfgCallback()
 *
 *	The callback registered with HFG Channels
 */
void BtlVgUtilsHfgCallback(HfgChannel *channel,
					  HfgCallbackParms *parms);

/*----------------------------------------------------------------------------------
 * BtlVgUtilsHsgCallback()
 *
 *	The callback registered with HSG Channels
 */
void BtlVgUtilsHsgCallback(HsgChannel *channel,
					  HsgCallbackParms *parms);

/*----------------------------------------------------------------------------------
 * BtlVgUtilsBthalMcCallback()
 *
 *	The callback registered with the BTHAL MC Module.
 */
void BtlVgUtilsBthalMcCallback(const BthalMcEvent *mcEvent);

/*----------------------------------------------------------------------------------
 * BtlVgUtilsBthalVcCallback()
 *
 *	The callback registered with the BTHAL VC Module.
 */
void BtlVgUtilsBthalVcCallback(const BthalVcEvent *vcEvent);

#if BT_SECURITY == XA_ENABLED
/*----------------------------------------------------------------------------------
 * BtlVgUtilsSetSecurityLevel()
 *
 *	Set security level for the HFG and HSG Modules.
 */
BtStatus BtlVgUtilsSetSecurityLevel(const BtSecurityLevel *secLevel);
#endif /* BT_SECURITY == XA_ENABLED */

/*----------------------------------------------------------------------------------
 * BtlVgUtilsEnableAudioProfiles()
 *
 *	Enables and registers with the HFG and HSG modules
 */
BtStatus BtlVgUtilsEnableAudioProfiles(const AtAgFeatures *features,
								  const char *serviceName);

/*----------------------------------------------------------------------------------
 * BtlVgUtilsDisableAudioProfiles()
 *
 *	Disables and deregisters with the HFG and HSG modules.
 */
BtStatus BtlVgUtilsDisableAudioProfiles(void);

/*-----------------------------------------------------------------------------------
 * BtlVgUtilsFindFreeChannel()
 *
 *	Returns a pointer to an unused BtlVgChannel,
 *	i.e. the channel has no active connection
 *
 */
BtlVgChannel* BtlVgUtilsFindFreeChannel(void);

/*-----------------------------------------------------------------------------------
 * BtlVgUtilsRouteAudioToUnit
 */
BtStatus BtlVgUtilsRouteAudioToChannel(BtlVgChannel *channel);

/*-----------------------------------------------------------------------------------
 * BtlVgUtilsRouteAudioToGateway
 */
BtStatus BtlVgUtilsRouteAudioToGateway(BtlVgChannel *channel);

/*-----------------------------------------------------------------------------------
 * BtlVgUtilsRegister
 */
BtStatus BtlVgUtilsRegister(BtlVgChannel *channel);

/*-----------------------------------------------------------------------------------
 * BtlVgUtilsDeregister
 */
BtStatus BtlVgUtilsDeregister(BtlVgChannel *channel);

/*-----------------------------------------------------------------------------------
 * BtlVgUtilsCreateServiceLink
 */
BtStatus BtlVgUtilsCreateServiceLink(BD_ADDR *bdAddr,
									BtlVgChannel *channel,
									BtlVgChannelType type);

/*-----------------------------------------------------------------------------------
 * BtlVgUtilsDisconnectServiceLink
 */
BtStatus BtlVgUtilsDisconnectServiceLink(BtlVgChannel *channel);

/*-----------------------------------------------------------------------------------
 * BtlVgUtilsConnectAudio()
 *
 *	Establishes an audio connection to an audio BT device.
 */
BtStatus BtlVgUtilsConnectAudio(BtlVgChannel *channel);

/*-----------------------------------------------------------------------------------
 * BtlVgUtilsDisconnectAudio()
 *
 *	Releases an audio connection to an audio BT device.
 */
BtStatus BtlVgUtilsDisconnectAudio(BtlVgChannel *channel);

/*-----------------------------------------------------------------------------------
 * BtlVgUtilsConnect()
 *
 *	Starts a connection to a remote device.
 *	Perfroms an SDP Query and establishes an HS or HF connection
 *	based upon the SDP Query result.
 */
BtStatus BtlVgUtilsConnect(BD_ADDR *addr, BtlVgChannel *channel);

/*-----------------------------------------------------------------------------------
 * BtlVgUtillQueryEscoSupport()
 *
 *	Queries the remote device for support of eSCO connections.
 */
BtStatus BtlVgUtilsQueryEscoSupport(BtRemoteDevice *remDev);

/*-----------------------------------------------------------------------------------
 * BtlVgUtilsAcceptIncomingChannel()
 */
BtStatus BtlVgUtilsAcceptIncomingChannel(BtlVgChannel *channel, BD_ADDR *bdAddr);

/*-----------------------------------------------------------------------------------
 * BtlVgUtilsRejectIncomingChannel()
 */
BtStatus BtlVgUtilsRejectIncomingChannel(BtlVgChannel *channel);

/*-----------------------------------------------------------------------------------
 * BtlVgUtilsIsInbandRingEnabled()
 */
BOOL BtlVgUtilsIsInbandRingEnabled(BtlVgChannel *channel);

/*-----------------------------------------------------------------------------------
 * BtlVgSmClearAtResult()
 */
BtStatus BtlVgUtilsClearAtResult(AtResults *res);

/*-----------------------------------------------------------------------------------
 * BtlVgSmBtlVgUtilsCStringToBtlVgString()
 *
 *	Returns BT_STATUS_SUCCESS on succesful convertion,
 *	BT_STATUS_NO_RESOURCES is returned if the "cString" argument is too long,
 *	In that case only BTL_VG_MAX_STR_LEN are copied.
 */
BtStatus BtlVgUtilsCStringToBtlVgString(BtlVgString *vgString,
								const char *cString);

/*-----------------------------------------------------------------------------------
 * BtlVgUtilsBtlVgStringCreate()
 */
BtStatus BtlVgUtilsBtlVgStringCreate(const char *cString, BtlVgString **vgString);

/*-----------------------------------------------------------------------------------
 * BtlVgSmUpdateAllIndicators()
 */
void BtlVgSmUpdateAllIndicators(BtlVgChannel *channel);

/*-----------------------------------------------------------------------------------
 * BtlVgUtilsDeregisterServices()
 */
BtStatus BtlVgUtilsDeregisterServices(void);

#if (0)
/*-----------------------------------------------------------------------------------
 * BtlVgUtilsSetFmOverBt()
 */
BtStatus BtlVgUtilsSetFmOverBt(BtlVgChannel *channel, BOOL enable);

#endif /* BTL_CONFIG_FM_STACK == BTL_CONFIG_ENABLED */

/*-----------------------------------------------------------------------------------
 * BtlVgUtilsHfgEventName()
 */
const char *BtlVgUtilsHfgEventName(HfgEvent event);

/*-----------------------------------------------------------------------------------
 * BtlVgUtilsHsgEventName()
 */
const char *BtlVgUtilsHsgEventName(HsgEvent event);

/*-----------------------------------------------------------------------------------
 * BtlVgUtilsBthalMcEventName()
 */
const char *BtlVgUtilsBthalMcEventName(BthalMcEventType event);

/*-----------------------------------------------------------------------------------
 * BtlVgUtilsBthalVcEventName()
 */
const char *BtlVgUtilsBthalVcEventName(BthalVcEventType event);

/*-----------------------------------------------------------------------------------
 * BtlVgUtilsIncrementCmdRspId()
 */
U16 BtlVgUtilsIncrementCmdRspId(U16 id);

#endif /* __BTL_VGI_H */


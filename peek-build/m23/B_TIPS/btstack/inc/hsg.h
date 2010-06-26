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
*   FILE NAME:      hsg.h
*
*   DESCRIPTION:    This file defines the API of the headset audio gateway.
*
*   AUTHOR:         Itay Klein
*
\*******************************************************************************/

#ifndef __HSG_H
#define __HSG_H

#include "rfcomm.h"
#include "atp.h"
#include "conmgr.h"

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HSG_TX_BUFFER_SIZE constant
 *
 *	Defines the maximum size of AT response data that can be transmitted.
 *	The default is large enough to handle all AT responses supported by
 *	the headset SDK If raw AT commands are sent that are larger than the
 *	default value, then this number must be increased.
 */
#ifndef HSG_TX_BUFFER_SIZE
#define HSG_TX_BUFFER_SIZE	32
#endif

/*---------------------------------------------------------------------------
 * HSG_RX_BUFFER_SIZE constant
 *
 *	Defines the maximum size of AT command data that can be received.
 *	The default is large enough to handle all AT commands supported by
 *	the headset profile.
 */
#ifndef HSG_RX_BUFFER_SIZE
#define HSG_RX_BUFFER_SIZE	32
#endif

/*---------------------------------------------------------------------------
 * HSG_SNIFF_TIMER constant
 *
 *	Enable sniff mode after a defined timeout. Sniff mode is entered
 *	when a service link is active, but no audio connection is up.
 *	This is a power saving feature. If this value is defined to -1, then
 *	sniff mode is disabled.
 */
#ifndef HSG_SNIFF_TIMER
#define HSG_SNIFF_TIMER	2000
#endif

/*---------------------------------------------------------------------------
 * HSG_SNIFF_MIN_INTERVAL constant
 *
 *	Miniumum interval for sniff mode if enabled (see HSG_SNIFF_TIMER).
 */
#ifndef HSG_SNIFF_MIN_INTERVAL
#define HSG_SNIFF_MIN_INTERVAL 800
#endif

/*---------------------------------------------------------------------------
 * HSG_SNIFF_MAX_INTERVAL constant
 *
 *	Maximum interval for sniff mode if enabled (see HSG_SNIFF_TIMER).
 */
#ifndef HSG_SNIFF_MAX_INTERVAL
#define HSG_SNIFF_MAX_INTERVAL 8000
#endif

/*---------------------------------------------------------------------------
 * HSG_SNIFF_ATTEMPT constant
 *
 *	Sniff attempts for sniff mode if enabled (see HSG_SNIFF_TIMER).
 */
#ifndef HSG_SNIFF_ATTEMPT
#define HSG_SNIFF_ATTEMPT 1600
#endif

/*---------------------------------------------------------------------------
 * HSG_SNIFF_TIMEOUT constant
 *
 *	Sniff timeout for sniff mode if enabled (see HSG_SNIFF_TIMER).
 */
#ifndef HSG_SNIFF_TIMEOUT
#define HSG_SNIFF_TIMEOUT 1600
#endif

#if HSG_DEFAULT_VOLUME > HSG_MAX_VOLUME
#error HSG_DEFAULT_VOLUME must be less than HSG_MAX_VOLUME
#endif

/*---------------------------------------------------------------------------
 * HSG_SECURITY_SETTINGS constant
 *
 *	The default headset audio gateway security settings.
 */
#define HSG_SECURITY_SETTINGS (BSL_AUTHENTICATION_IN | BSL_AUTHENTICATION_OUT)

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HsgEvent type.
 *
 *	All indications and confirmations are sent through a callback
 *	function. Depending on the event, different elements of
 *	HsgCallbackParms "parms" union will be valid as described below.
 */
typedef U32 HsgEvent;

/** An incoming service level connection is being established.
 *	This happens when the headset device establishes the service connection.
 *  The data connection is not available yet for sending to the headset 
 *  device.  When the HSG_EVENT_SERVICE_CONNECTED event is received, the 
 *  channel is available for sending.
 *  When this callback is received, the "HsgCallbackParms.p.remDev" field 
 *  contains a pointer to the remote device context.
 */
#define HSG_EVENT_SERVICE_CONNECT_REQ	0

/** A service level connection has been established.  
 *	This can happen as the result of a call to HSG_CreateServiceLink,
 *	or if the headset device establishes the service connection.
 *	When this event has been received,
 *	a data connection is available for sending to the headset device.
 *
 *  This event can also occur when an attempt to create a service level 
 *  connection (HSG_CreateServiceLink) fails.
 *
 *  When this callback is received, the "HsgCallbackParms.p.remDev" field 
 *  contains a pointer to the remote device context.
 *	In addition, the "HsgCallbackParms.errCode" fields contains the reason for disconnect.
 */
#define HSG_EVENT_SERVICE_CONNECTED		1

/** The service level connection has been released.
 *	This can happen as the result of a call to HSG_DisconnectServiceLink,
 *	or if the headset device releases the service connection.
 *
 *	This event can also occur when an attempt to create a service level 
 *	connection (HSG_CreateServiceLink) fails.
 *
 *	When this callback is received, the "HsgCallbackParms.p.remDev" field 
 *	contains a pointer to the remote device context.  In addition, the
 *	"HsgCallbackParms.errCode" fields contains the reason for disconnect.
 */
#define HSG_EVENT_SERVICE_DISCONNECTED	2

/** A request to establish audio connection has been recieved in the audio gateway.
 *  Call HSG_AccpetAudioRequest() to accept the connection
 *  or HSG_RejectAudioRequest() to reject the connection.
 */
#define HSG_EVENT_AUDIO_CONNECT_REQ		3

/** An audio connection has been established.
 *	This event occurs whenever the audio channel (SCO) comes up,
 *	When this callback is received, the "HsgCallbackParms.p.remDev" field 
 *	contains a pointer to the remote device context.
 */
#define HSG_EVENT_AUDIO_CONNECTED		4

/** An audio connection has been released.
 *	This event occurs whenever the audio channel (SCO) goes down,
 *	
 *  When this callback is received, the "HsgCallbackParms.p.remDev" field 
 *  contains a pointer to the remote device context.  In addition, the
 *  "HsgCallbackParms.errCode" fields contains the reason for disconnect.
 */
#define HSG_EVENT_AUDIO_DISCONNECTED	5


/** The headset button was pressed,
 *	AT+CKPD=200 recieved from the headset device.
 */
#define HSG_EVENT_BUTTON_PRESSED		6

/** The headset device has informed the audio gateway of its microphone volume 
 *  level. 
 *  The "HsgCallbackParms.p.gain" parameter is valid.
 */
#define HSG_EVENT_REPORT_MIC_VOLUME		7

/** The headset device has informed the audio gateway of its speaker volume 
 *  level. 
 *  The "HsgCallbackParms.p.gain" parameter is valid.
 */
#define HSG_EVENT_REPORT_SPK_VOLUME		8

/** An unsupported AT command has been received from the audio gateway.
 *	This event is received for AT commands that are not handled by the internal
 *  headset AT parser.  The application must make an appropriate response
 *  and call HSG_SendOK() to complete the response.
 *
 *  When this callback is received, the "HsgCallbackParms.p.data" field 
 *  contains the AT command data.
 */
#define HSG_EVENT_AT_COMMAND_DATA		9

/** Whenever a response has been set to the remote device, this event is
 *  received to confirm that the repsonse was sent.
 * 
 *  When this event is received, the "HsgCallbackParms.p.response" field
 *  contains a pointer to the response structure that was used to send
 *  the reponse.
 */
#define HSG_EVENT_RESPONSE_COMPLETE		10

#if BT_SCO_HCI_DATA == XA_ENABLED
/** Only valid if BT_SCO_HCI_DATA is set to XA_ENABLED.  Audio data has been 
 *  received from the remote device.  The data is only valid during the
 *  callback.
 *
 *  When this callback is received, the "HsgCallbackParms.p.audioData" field 
 *  contains the audio data.
 */
#define HSG_EVENT_AUDIO_DATA			11

/** Only valid if BT_SCO_HCI_DATA is set to XA_ENABLED.  Audio data has been 
 *  sent to the remote device.  This event is received by the application when
 *  the data sent by HSG_SendAudioData has been successfully sent.
 *
 *  When this callback is received, the "HsgCallbackParms.p.audioPacket" field 
 *  contains the result.
 */
#define HSG_EVENT_AUDIO_DATA_SENT		12
#endif

/*	End of HsgEvent */

/*---------------------------------------------------------------------------
 * HsgSniffExitPolicy type
 *
 *	Defines the policy for exiting sniff mode.  While it is possible to both
 *	send data and create SCO connections in sniff mode, it may be desired to
 *	exit sniff mode for performance or compatibility reasons.
 */
typedef U8 HsgSniffExitPolicy;

/** Exit sniff mode whenever there is data to send.
 */
#define HSG_SNIFF_EXIT_ON_SEND  CMGR_SNIFF_EXIT_ON_SEND

/** Exit sniff mode whenever an audio link is being created.
 */
#define HSG_SNIFF_EXIT_ON_AUDIO CMGR_SNIFF_EXIT_ON_AUDIO

/* End of HsgSniffExitPolicy */

/*---------------------------------------------------------------------------
 * HsgResponse type
 *    
 *	Structure used to send AT responses.
 */
typedef AtResults HsgResponse;

/* End of HsgResponse */

#if TI_CHANGES == XA_ENABLED
/*---------------------------------------------------------------------------
 * HfgCccConnState type
 *     
 *     State of connection establishing used in possible cancellation of
 *     creating connection
 */
typedef U8 HsgCccConnState;

#define HSG_CCC_CONN_STATE_NONE            0x00
#define HSG_CCC_CONN_STATE_CONNECTING_ACL  0x01
#define HSG_CCC_CONN_STATE_SDP_QUERY       0x02
#define HSG_CCC_CONN_STATE_CONNECTING      0x03
#define HSG_CCC_CONN_STATE_CONNECTED       0x04

/* End of HsgCccConnState */
#endif /* TI_CHANGES == XA_ENABLED */

/* Forward References */
typedef struct _HsgCallbackParms HsgCallbackParms;
typedef struct _HsgChannel HsgChannel;

/*---------------------------------------------------------------------------
 * HsgCallback type
 *
 *	A function of this type is called to indicate headset events to
 *	the application.
 */
typedef void (*HsgCallback)(HsgChannel *channel, HsgCallbackParms *parms);

/* End of HsgCallback */

#if BT_SCO_HCI_DATA == XA_ENABLED
/*---------------------------------------------------------------------------
 * HsgAudioData type
 *
 *	This type is only available when BT_SCO_HCI_DATA is set to XA_ENABLED.
 *	Contains audio data received from the remote device.
 */
typedef CmgrAudioData HsgAudioData;

/* End of HsgAudioData */
#endif

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/
/*---------------------------------------------------------------------------
 * HsgAtData structure
 *
 *	Defines the structure containing raw AT data.
 */
typedef struct _HsgAtData {
    U8         *rawData;

	U32			dataLen;

} HsgAtData;

/*---------------------------------------------------------------------------
 * HsgChannel structure
 *
 *	The Audio gateway channel.
 *	This structure is used to identify a connection to the headset device.
 */
struct _HsgChannel {

	/* === Internal use only === */

    /* Registration variables */
    ListEntry		node;
    HsgCallback		callback;		/* Application callback function    */
    RfChannel		rfChannel;		/* RFCOMM channel used for audio
									 * gateway connection.
									 */
    /* Transmit Queue */
    ListEntry		rspQueue;		/* List of AT responses             */

    /* Connection State Variables */
    U8				state;			/* Current connection state         */
    U16				flags;			/* Current connection flags         */
    U8				linkFlags;		/* Levels of service connected      */

    /* SDP variables for client */
    SdpQueryToken	sdpQueryToken;	/* Used to query the service        */
    U8				queryFlags;		/* Defines which SDP entries were
									 * parsed from the gateway.
									 */
    U8				rfServerChannel;/* When connecting AG client        */

    /* Channel Resources */
    CmgrHandler		cmgrHandler;
    AtResults		atResults;
    AtResults		*currentAtResults;
    AtResults		*lastAtResults;
    XaBufferDesc	atBuffer;
    BtPacket		atTxPacket;
    U8				atTxData[HSG_TX_BUFFER_SIZE];
    U16				bytesToSend;
    U8				atRxBuffer[HSG_RX_BUFFER_SIZE];
    U16				atRxLen;

#if BT_SECURITY == XA_ENABLED
	BtSecurityRecord	securityRecord;
	BOOL				isSecurityRegistered;

#if TI_CHANGES == XA_ENABLED
    /* Flag and connection establishing state used for cancellation */
    BOOL                cancelCreateConn;
    HsgCccConnState     cccConnState; 
#endif /* TI_CHANGES == XA_ENABLED */
#endif

};

/*---------------------------------------------------------------------------
 * HsgCallbackParms structure
 *
 * This structure is sent to the application's callback to notify it of
 * any state changes.
 */
struct _HsgCallbackParms {
	HsgEvent	event;	/* Event associated with the callback       */

	BtStatus	status;	/* Status of the callback event             */
	BtErrorCode	errCode;/* Error code (reason) on disconnect events */

	/* For certain events, a single member of this union will be valid.
	 * See HsgEvent documentation for more information.
	 */
	union {
		void			*ptr;
		BtRemoteDevice	*remDev;
		U8				gain;
        HsgResponse		*response;

#if BT_SCO_HCI_DATA == XA_ENABLED
        HsgAudioData	*audioData;
        BtPacket		*audioPacket;
#endif
        HsgAtData		*data;
    }p;

};

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HSG_Init()
 *
 *	Initialize the Audio Gateway SDK.  This function should only be called
 *	once, normally at sytem initialization time.  The calling of this 
 *	function can be specified in overide.h using the XA_LOAD_LIST macro
 *	(i.e. #define XA_LOAD_LIST XA_MODULE(HSG)).
 *
 * Returns:
 *	TRUE - Initialization was successful
 *
 *	FALSE - Initialization failed.
 */
BOOL HSG_Init(void);

/*---------------------------------------------------------------------------
 * HSG_Register()
 *
 *	Registers and initializes a channel for use in creating and receiving
 *	service level connections.  Registers the headset profile audio
 *	gateway with SDP.  The application callback function is also bound
 *	to the channel.
 *
 * Parameters:
 *	channel - Contains a pointer to the channel structure that will be
 *		initialized and registered.
 *
 *	Callback - The application callback function that will receive events.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *	BT_STATUS_IN_USE - The operation failed because the channel has already
 *		been initialized. 
 *
 *	BT_STATUS_FAILED - The operation failed because either the RFCOMM
 *		channel or the SDP record could not be registered.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *		initialized (XA_ERROR_CHECK only).
 */
BtStatus HSG_Register(HsgChannel *channel, HsgCallback callback);

/*---------------------------------------------------------------------------
 * HSG_Deregister()
 *
 *	Deregisters the channel.  The channel becomes unbound from RFCOMM and
 *	SDP, and can no longer be used for creating service level connections.
 *
 * Parameters:
 *	Channel - Contains a pointer to the channel structure that will be
 *		deregistered.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *	BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *	BT_STATUS_BUSY - The operation failed because a service level connection
 *		is still open to the audio gateway.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *		initialized (XA_ERROR_CHECK only).
 */
BtStatus HSG_Deregister(HsgChannel *Channel);

/*---------------------------------------------------------------------------
 * HSG_CreateServiceLink()
 *
 *	Creates a service level connection with the headset unit.
 *	This includes performing SDP Queries to find the appropriate service
 *	and opening an RFCOMM channel.  The success of the operation is indicated 
 *	by the HSG_EVENT_SERVICE_CONNECTED event.  If the connection fails, the
 *	application is notified by the HSG_EVENT_SERVICE_DISCONNECTED event. 
 *
 *	If an ACL link does not exist to the audio gateway, one will be 
 *	created first.  If desired, however, the ACL link can be established 
 *	prior to calling this function.
 *
 * Parameters:
 *
 *	Channel - Pointer to a registered channel structure.
 *
 *	Addr - The Bluetooth address of the remote device.
 *
 * Returns:
 *	BT_STATUS_PENDING - The operation has started, the application will be 
 *		notified when the connection has been created (via the callback 
 *		function registered by HSG_Register).
 *
 *	BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *	BT_STATUS_BUSY - The operation failed because a connection is already
 *		open to the remote device, or a new connection is being created.
 *
 *	BT_STATUS_FAILED - The channel has not been registered.
 *
 *	BT_STATUS_CONNECTION_FAILED - The connection failed.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *		initialized (XA_ERROR_CHECK only).
 */
BtStatus HSG_CreateServiceLink(HsgChannel *Channel, BD_ADDR *Addr);

/*---------------------------------------------------------------------------
 * HSG_DisconnectServiceLink()
 *
 *	Releases the service level connection with the headset unit. This will 
 *	close the RFCOMM channel and will also close the SCO and ACL links if no 
 *	other services are active, and no other link handlers are in use 
 *	(ME_CreateLink).  When the operation is complete the application will be 
 *	notified by the HSG_EVENT_SERVICE_DISCONNECTED event.
 *
 * Parameters:
 *	Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *	BT_STATUS_PENDING - The operation has started, the application will be 
 *		notified when the service level connection is down (via the callback 
 *		function registered by HSG_Register).
 *
 *	BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *	BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *		does not exist to the audio gateway.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *		initialized (XA_ERROR_CHECK only).
 */
BtStatus HSG_DisconnectServiceLink(HsgChannel *Channel);

/*---------------------------------------------------------------------------
 * HSG_AcceptAudioLink()
 *
 *	Accpets or rejects an incoming request to establish an audio connection.
 *	Called in response to HSG_EVENT_AUDIO_CONNECTION_REQ.
 *
 * Parameters:
 *	Channel - Pointer to a registered channel structure,
 *
 *	Error - Set to BEC_NO_ERROR to accept the connection,
 *		otherwise the connection is rejected.
 *
 * Returns:
 *	BT_STATUS_PENDING - The connection will be accepted.
 *		When the connection is up, the event HSG_EVENT_AUDIO_CONNECTED 
 *		will be sent.
 *
 *	BT_STATUS_NOT_FOUND - Could not create audio link,
 *		because the HsgChannel is not registered.
 *
 *	BT_STATUS_INVALID_PARM - Invalid parameter or handlers did not
 *		match (XA_ERROR_CHECK only).
 */
BtStatus HSG_AcceptAudioLink(HsgChannel *Channel, BtErrorCode Error);
#define HSG_AcceptAudioLink(c,e) CMGR_AcceptAudioLink(&(c)->cmgrHandler, (e))

/*---------------------------------------------------------------------------
 * HSG_CreateAudioLink()
 *
 *	Creates an audio (SCO) link to the headset unit. The success of the 
 *	operation is indicated by the HSG_EVENT_AUDIO_CONNECTED event.  If the 
 *	connection fails, the application is notified by the 
 *	HSG_EVENT_AUDIO_DISCONNECTED event.  
 *
 * Parameters:
 *	channel - Pointer to a registered channel structure.
 *
 * Returns:
 *	BT_STATUS_PENDING - The operation has started, the application will be 
 *		notified when the audio link has been established (via the callback 
 *		function registered by HSG_Register).
 *
 *	BT_STATUS_SUCCESS - The audio (SCO) link already exists.
 *
 *	BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *	BT_STATUS_NO_CONNECTION - The operation failed because a service level 
 *		connection does not exist to the audio gateway.
 *
 *	BT_STATUS_FAILED - An audio connection already exists.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *		initialized (XA_ERROR_CHECK only).
 */
BtStatus HSG_CreateAudioLink(HsgChannel *Channel);

/*---------------------------------------------------------------------------
 * HSG_DisconnectAudioLink()
 *
 *	Releases the audio connection with the headset unit.  When the 
 *	operation is complete, the application will be notified by the 
 *	HSG_EVENT_SERVICE_DISCONNECTED event.
 *
 * Parameters:
 *	Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *	BT_STATUS_PENDING - The operation has started, the application will be 
 *		notified when the audio connection is down (via the callback 
 *		function registered by HSG_Register).
 *
 *	BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *	BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *		does not exist to the audio gateway.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *		initialized (XA_ERROR_CHECK only).
 */
BtStatus HSG_DisconnectAudioLink(HsgChannel *Channel);

#if BT_SCO_HCI_DATA == XA_ENABLED
/*---------------------------------------------------------------------------
 * HSG_SendAudioData()
 *
 *	Sends the specified audio data on the audio link.
 *
 * Parameters:
 *	channel - The Channel over which to send the audio data.
 *
 *	packet - The packet of data to send. After this call, the Headset
 *		SDK owns the packet. When the packet has been transmitted
 *		to the host controller, HSG_EVENT_AUDIO_DATA_SENT is sent to the
 *		application
 *
 * Returns:
 *	BT_STATUS_PENDING - The packet was queued successfully.
 *
 *	BT_STATUS_NO_CONNECTION - No audio connection exists.
 *
 *	BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus HSG_SendAudioData(HsgChannel *Channel, BtPacket *Packet);
#define HSG_SendAudioData(c, p) SCO_SendData((c)->cmgrHandler.scoConnect, p)
#endif

/* Forward Reference used by several functions defined below */
BtStatus HSG_SendHsResults(HsgChannel *Channel, AtCommand Command, 
                           U32 Parms, U16 ParmLen, HsgResponse *Response,
                           BOOL done);

/*---------------------------------------------------------------------------
 * HSG_SendOK
 *	Sends an OK response to the headset device.  This function must
 *	be called after receiving several events (see the description of each
 *	event).
 *
 * Parameters:
 *	Channel - Pointer to a registered channel structure.
 *
 *	Response - A response structure to be used for transmitting the
 *		response.
 *
 * Returns:
 *	BT_STATUS_PENDING - The result code has been sent to the headset.
 *
 *	BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *	BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *		initialized (XA_ERROR_CHECK only).
 */
BtStatus HSG_SendOK(HsgChannel *Channel, HsgResponse *Response);
#define HSG_SendOK(c, r) HSG_SendHsResults(c, AT_OK, 0, 0, r, FALSE)

/*---------------------------------------------------------------------------
 * HSG_SendError()
 *
 *	Sends an ERROR result code to the headset.
 *
 * Parameters:
 *	channel - Pointer to a registered channel structure.
 *
 *	response - A response structure to be used for transmitting the
 *		response.
 *
 * Returns:
 *	BT_STATUS_PENDING - The result code has been sent to the headset.
 *
 *	BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *	BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *		initialized (XA_ERROR_CHECK only).
 */
BtStatus HSG_SendError(HsgChannel *Channel, HsgResponse *response);
#define HSG_SendError(c, r) HSG_SendHsResults(c, AT_ERROR, 0, 0, r, FALSE)

/*---------------------------------------------------------------------------
 * HSG_SendRing()
 *
 *	Notifies the headset of an incoming call.
 *	Sends a single "RING" to the headset unit.
 *
 *
 * Parameters:
 *	Channel - Pointer to a registered channel structure.
 *
 *	Response - A response structure to be used for transmitting the
 *		response.
 *
 * Returns:
 *	BT_STATUS_PENDING - The result code has been sent to the headset.
 *
 *	BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *	BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *		initialized (XA_ERROR_CHECK only).
 */
BtStatus HSG_SendRing(HsgChannel *Channel, HsgResponse *Response);
#define HSG_SendRing(c, r) HSG_SendHsResults(c, AT_RING, 0, 0, r, FALSE)

/*---------------------------------------------------------------------------
 * HSG_SendMicVolume()
 *
 *	Notifies the headset of the AG's current microphone volume level. 
 *
 *	When the AT response has been sent, the HSG_EVENT_RESPONSE_COMPLETE
 *	event will be received by the application's callback function.
 *
 * Parameters:
 *	channel - Pointer to a registered channel structure.
 *
 *	gain - current volume level.
 *
 *	response - A response structure to be used for transmitting the
 *		response.
 *
 * Returns:
 *	BT_STATUS_PENDING - The speaker volume level has been sent to the headset.
 *
 *	BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *	BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *		initialized (XA_ERROR_CHECK only).
 */
BtStatus HSG_SendMicVolume(HsgChannel *Channel, U8 Gain, HsgResponse *Response);
#define HSG_SendMicVolume(c, g, r)  HSG_SendHsResults(c, AT_MICROPHONE_GAIN, (U32)(g), 0, r, FALSE)

/*---------------------------------------------------------------------------
 * HSG_SendSpeakerVolume()
 *
 *	Notifies the headset of the AG's current speaker volume level.
 *
 * Parameters:
 *	Channel - Pointer to a registered channel structure.
 *
 *	gain - current volume level.
 *
 *	Response - A response structure to be used for transmitting the
 *		response.
 *
 * Returns:
 *	BT_STATUS_PENDING - The speaker volume level has been sent to the headset.
 *
 *	BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *	BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *		initialized (XA_ERROR_CHECK only).
 */
BtStatus HSG_SendSpeakerVolume(HsgChannel *Channel, U8 Gain, HsgResponse *Response);
#define HSG_SendSpeakerVolume(c, g, r)  HSG_SendHsResults(c, AT_SPEAKER_GAIN, (U32)(g), 0, r, FALSE)

/*---------------------------------------------------------------------------
 * HSG_SendAtResponse()
 *
 *	Sends any AT response.  The 'AtString' parameter must be initialized,
 *	and the AT response must be formatted properly.  It is not necessary
 *	to add CR/LF at the beginning and end of the string.
 *
 *	When the AT response has been sent, the HSG_EVENT_RESPONSE_COMPLETE
 *	event will be received by the application's callback function.
 *
 * Parameters:
 *	Channel - Pointer to a registered channel structure.
 *
 *	AtString - An properly formated AT response.
 *   
 *	Response - A response structure to be used for transmitting the
 *		response.
 *
 * Returns:
 *	BT_STATUS_PENDING - The operation has started, the application will be 
 *		notified when the response has been sent (via the callback function 
 *		registered by HSG_Register).
 *
 *	BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *	BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *		does not exist to the audio gateway.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *		initialized (XA_ERROR_CHECK only).
 */
BtStatus HSG_SendAtResponse(HsgChannel *Channel, const char *AtString,
                            HsgResponse *Response);
#define HSG_SendAtResponse(c, s, r)  HSG_SendHsResults(c, AT_RAW, (U32)s, sizeof(s), r, TRUE)

#if HSG_SNIFF_TIMER >= 0
/*---------------------------------------------------------------------------
 * HSG_EnableSniffMode
 *
 *	Enables/Disables placing link into sniff mode.
 *
 * Requires:
 *     HSG_SNIFF_TIMER >= 0.
 *
 * Parameters:
 *	Channel - Pointer to a registered channel structure.
 *
 *	Enabled - If TRUE, sniff mode will be used, otherwise sniff mode is
 *		disabled
 */
BtStatus HSG_EnableSniffMode(HsgChannel *Channel, BOOL Enable);

/*---------------------------------------------------------------------------
 * HSG_IsSniffModeEnabled
 *
 *	Returns TRUE when sniff mode is enabled on the specified handler.
 *
 * Parameters:
 *	Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *	TRUE if sniff mode is enabled.
 *
 *	FALSE if sniff mode is not enabled.
 */
BOOL HSG_IsSniffModeEnabled(HsgChannel *Channel);
#define HSG_IsSniffModeEnabled(c) (CMGR_GetSniffTimer(&((c)->cmgrHandler)) > 0)
#endif

/*---------------------------------------------------------------------------
 * HSG_SetSniffExitPolicy()
 *
 *	Sets the policy for exiting sniff mode on the specified channel.  The 
 *	policy can be set to HSG_SNIFF_EXIT_ON_SEND or HSG_SNIFF_EXIT_ON_AUDIO_LINK.
 *	These values can also be OR'd together to enable both (See 
 *	HsgSniffExitPolicy).
 *
 * Parameters:
 *	Channel - Pointer to a registered channel structure.
 *
 *	Policy - Bitmask that defines the policy for exiting sniff mode.
 *
 * Returns:
 *
 *	BT_STATUS_SUCCESS - The policy was set successfully.
 *
 *	BT_STATUS_NOT_FOUND - Could not set the sniff policy, because  
 *		Handler is not registered.
 */
BtStatus HSG_SetSniffExitPolicy(HsgChannel *Channel, HsgSniffExitPolicy Policy);
#define HSG_SetSniffExitPolicy(c, p) CMGR_SetSniffExitPolicy(&((c)->cmgrHandler), (p));

/*---------------------------------------------------------------------------
 * HSG_GetSniffExitPolicy()
 *
 *	Gets the policy for exiting sniff mode on the specified channel.
 *
 * Parameters:
 *	Channel - Pointer to a registered channel structure.
 * 
 * Returns:
 *
 *     HsgSniffExitPolicy
 */
HsgSniffExitPolicy HSG_GetSniffExitPolicy(HsgChannel *Channel);
#define HSG_GetSniffExitPolicy(c)  CMGR_GetSniffExitPolicy(&((c)->cmgrHandler))

#if BT_SECURITY == XA_ENABLED
/*---------------------------------------------------------------------------
 * HSG_SetSecurityLevel()
 *
 *	Sets the security level for the HSG service
 *	(incoming connections).
 *
 * Parameters:
 *	Level - The security level.
 * 
 * Returns:
 */
void HSG_SetSecurityLevel(BtSecurityLevel Level);

/*---------------------------------------------------------------------------
 * HSG_GetSecurityLevel()
 *
 *	Gets the current security level for the HSG service
 *	(incoming connections).
 *
 * Parameters:
 * 
 * Returns:
 *	The current security level
 */
BtSecurityLevel HSG_GetSecurityLevel(void);
#endif /* BT_SECURITY == XA_ENABLED */

/*---------------------------------------------------------------------------
 * HSG_AcceptIncomingSLC()
 *
 *	Accept an incoming SLC
 *
 * Parameters:
 * 
 * Returns:
 */
BtStatus HSG_AcceptIncomingSLC(HsgChannel *channel, BD_ADDR *addr);

/*---------------------------------------------------------------------------
 * HSG_RejectIncomingSLC()
 *
 *	Reject an incoming SLC
 *
 * Parameters:
 * 
 * Returns:
 */
BtStatus HSG_RejectIncomingSLC(HsgChannel *channel);

#endif /* __HSG_H */

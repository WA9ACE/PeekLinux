/*******************************************************************************\
##                                                                           	*
##  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION			*
##																				*
##  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE			*
##	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE		*
##	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO		*
##	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT		*
##	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL			*
##	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.			*
##																				*
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      btl_ag.h
*
*   BRIEF:			This file defines the API of the BTL Audio Gateway.
*
*	DESCRIPTION:	The BTL AG is an implementation of the Audio Gateway role
*					of the HFP and HSP.
*
*					The same API is used for the HFP part and the HSP part
*					where applicable.
*					Using an API intended for HFP (for example sending AT commands undefineded by the HSP)
*					for a HSP connection results in an error.
*					
*					Support for AT commands is given in the form of parsing incoming
*					AT commands and encoding AT results or responses.
*
*   AUTHOR:         Itay Klein
*
\*******************************************************************************/

#ifndef __BTL_AG_H
#define __BTL_AG_H

/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/

#include "bttypes.h"
#include "hfg.h"
#include "hsg.h"
#include "btl_common.h"
#include "btl_config.h"

/********************************************************************************
 *
 * Constants.
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BTL_AG_CHANNEL_ID_NONE constant.
 *
 *	A BtlAgChannelId value used to indicate that a BtlAgChannelId variable
 *	doesn't not reference a AG channel.
 */
#define BTL_AG_CHANNEL_ID_NONE 0xffffffff


/*-------------------------------------------------------------------------------
 * BTL_AG_MAX_VOLUME constant
 *
 *	The maximum value for speaker of microphone volume.
 */
#define BTL_AG_MAX_VOLUME	15

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/*------------------------------------------------------------------------------
 * BtlAgEventType type
 *
 *	A type for describing BTL AG Events type.
 */
typedef U32 BtlAgEventType;

/* Incoming SLC from a remote device.
 */
#define BTL_AG_EVENT_SERVICE_CONNECT_REQ		0

/* 
 * SLC with a remote device is established.
 */
#define BTL_AG_EVENT_SERVICE_CONNECTED			1

/* 
 * SLC with a remote device is released.
 */
#define BTL_AG_EVENT_SERVICE_DISCONNECTED		2

/* 
 * Received a request for an audio connection
 */
#define BTL_AG_EVENT_AUDIO_CONNECT_REQ			3

/* 
 * Audio connection established
 */
#define BTL_AG_EVENT_AUDIO_CONNECTED			4

/* 
 * Audio connection lost.
 */
#define BTL_AG_EVENT_AUDIO_DISCONNECTED			5

/* 
 * Received the features of a remote handsfree unit.
 * "BtlAgEvent.p.hfgParms.p.features" is valid.
 */
#define BTL_AG_EVENT_HANDSFREE_FEATURES			6


#if BT_SCO_HCI_DATA == XA_ENABLED
/* 
 *	This event is inteded for VOHCI support and is not yet implemented
 */
#define BTL_AG_EVENT_AUDIO_DATA					7

/* 
 *	This event is inteded for VOHCI support and is not yet implemented
 */
#define BTL_AG_EVENT_AUDIO_DATA_SENT			8
#endif

/* 
 * The handsfree unit requested the AG to
 * answer the incoming call.
 */
#define BTL_AG_EVENT_ANSWER_CALL				9

/* 
 * The handsfree unit has requested the AG to dial a number.
 * "BtlAgEvent.p.hfgParms.p.phoneNumber" is valid.
 */
#define BTL_AG_EVENT_DIAL_NUMBER				10

/* 
 * The handsfree unit has requested the AG to dial a number
 * from a local memory location.
 * "BtlAgEvent.p.hfgParms.p.memory" is valid.
 */
#define BTL_AG_EVENT_MEMORY_DIAL				11

/* 
 * The handsfree unit has requested the AG to redial
 * the last number.
 *
 */
#define BTL_AG_EVENT_REDIAL						12

/* 
 * The handsfree unit has requested the AG to place the current call on hold
 */
#define BTL_AG_EVENT_CALL_HOLD					13

#if HFG_USE_RESP_HOLD == XA_ENABLED

/* 
 * The handsfree unit requests the current Response and hold state
 * of the audio gateway.
 */
#define BTL_AG_EVENT_QUERY_RESPONSE_HOLD		14

/* 
 * The handsfree unit has requested the AG to set the current response and hold state.
 * "BtlAgEvent.p.hfgParms.p.respHold" is valid.
 */
#define BTL_AG_EVENT_RESPONSE_HOLD				15

#endif /* HFG_USE_RESP_HOLD == XA_ENABLED */

/* 
 * The handsfree unit has requested the AG to hang up current call.
 */
#define BTL_AG_EVENT_HANGUP						16

/* 
 * The handsfree unit has requested a list of the current calls
 * in the AG.
 */
#define BTL_AG_EVENT_LIST_CURRENT_CALLS			17

/* 
 * The handsfree unit has requested the AG to enable/disable
 * Calling Line Identification Presentation.
 * "BtlAgEvent.p.hfgParms.p.enable" is valid.
 */
#define BTL_AG_EVENT_ENABLE_CALLER_ID			18

/* 
 * The handsfree unit has requested the AG to enable/disable
 * call waiting notification.
 * "BtlAgEvent.p.hfgParms.p.enable" is valid. 
 */
#define BTL_AG_EVENT_ENABLE_CALL_WAITING		19

/* 
 * The handsfree unit has requested the AG to generate
 * a DTMF.
 * BtlAgEvent.p.hfgParms.p.dtmf" is valid.
 */
#define BTL_AG_EVENT_GENERATE_DTMF				20

/* 
 * The handsfree unit has requested a voice tag.
 */
#define BTL_AG_EVENT_GET_LAST_VOICE_TAG			21

/* 
 * The handsfree unit has requested voice recognition to 
 * be enabled or disabled.
 * "BtlAgEvent.p.hfgParms.p.enabled" is valid.
 */
#define	BTL_AG_EVENT_ENABLE_VOICE_RECOGNITION	22

/* 
 * The handsfree unit has requested to disable Echo Cancelling 
 * and Noise Reduction within the AG.
 */
#define BTL_AG_EVENT_DISABLE_NREC				23

/* 
 * The remote audio unit reports its current microphone volume.
 * "BtlAgEvent.p.hfgParms.p.gain" or "BtlAgEvent.p.hsgParms.p.gain"
 * is valid.
 */
#define BTL_AG_EVENT_REPORT_MIC_VOLUME			24

/* 
 * The remote audio unit reports its current speaker volume.
 * "BtlAgEvent.p.hfgParms.p.gain" or "BtlAgEvent.p.hsgParms.p.gain"
 * is valid.
 */
#define BTL_AG_EVENT_REPORT_SPK_VOLUME			25

/* 
 * The handsfree units has requested the network operator.
 */
#define BTL_AG_EVENT_QUERY_NETWORK_OPERATOR		26

/* 
 * The handsfree unit has requested the subscriber number.
 */
#define BTL_AG_EVENT_QUERY_SUBSCRIBER_NUMBER	27

/* 
 * The hands free unit has requested extended errors to be enabled.
 */
#define BTL_AG_EVENT_ENABLE_EXTENDED_ERRORS		28

/* 
 * An unsupported AT command has be received.
 * "BtlAgEvent.p.hfgParms.p.data" or "BtlAgEvent.p.hsgParms.p.data"
 * holds the AT command.
 */
#define BTL_AG_EVENT_AT_COMMAND_DATA			29

/* 
 * An AT command has been sent to the remote audio unit.
 */
#define BTL_AG_EVENT_RESPONSE_COMPLETE			30

/* 
 * The user pressed the remote headset button:
 * AT+CKPD=200
 */
#define BTL_AG_EVENT_BUTTON_PRESSED				31

/* 
 * The AG context is disabled.
 * received, when BTL_AG_Disable() operation is asynchronous.
 */
#define BTL_AG_EVENT_CONTEXT_DISABLED			32

/* 
 * the FM over Bluetooth audio path is enable .
 */
#define BTL_AG_EVENT_FM_OVER_BT_ENABLE			33

/* 
 * the FM over Bluetooth audio path is disable .
 */
#define BTL_AG_EVENT_FM_OVER_BT_DISABLE			34

/* Forward declaration */
typedef struct _BtlAgEvent BtlAgEvent;
typedef struct _BtlAgContext BtlAgContext;

/*-------------------------------------------------------------------------------
 * BtlAgChannelId type
 *
 *	An ID for identifying a channel within an AG context.
 */
typedef U32 BtlAgChannelId;

/*-------------------------------------------------------------------------------
 * BtlAgChannelState type
 *
 *	A type defining the different states of a channel.
 */
typedef enum _BtlAgChannelState
{
	BTL_AG_CHANNEL_STATE_UNKNOWN,				/* The channel was not created */
	BTL_AG_CHANNEL_STATE_IDLE,					/* The channel is ready for accepting or initiating SLC */
	BTL_AG_CHANNEL_STATE_CONNECTING_SLC,		/* SLC is being established */
	BTL_AG_CHANNEL_STATE_DISCONNECTING_SLC,		/* SLC is being disconnected */
	BTL_AG_CHANNEL_STATE_SLC_CONNECTED,			/* SLC is established */
	BTL_AG_CHANNEL_STATE_CONNECTING_AUDIO,		/* Audio connection is being established */
	BTL_AG_CHANNEL_STATE_DISCONNECTING_AUDIO,	/* Audio is being disconnected */
	BTL_AG_CHANNEL_STATE_AUDIO_CONNECTED		/* Audio connection is established */
	
}BtlAgChannelState;

/*-------------------------------------------------------------------------------
 * BtlAgService type
 *
 *	A bit mask representing active services within the AG.
 */
typedef U32 BtlAgService;

#define BTL_AG_SERVICE_HFAG		0x00000001

#define BTL_AG_SERVICE_HSAG		0x00000002

/*-------------------------------------------------------------------------------
 * BtlAgCallBack type
 *
 *	A function of this type is called to indicate BTL AG events.
 */
typedef void (*BtlAgCallback)(const BtlAgEvent *event);


/*-------------------------------------------------------------------------------
 * BtlAgEvent structure
 *
 *	Represents a BTL AG Event.
 */
struct _BtlAgEvent
{
	BtlAgContext *context;		/* The event's relevant AG context */

	BtlAgChannelId channelId;	/* The event's relevant channel Id */

	BtlAgService service;		/* The service type of the channel */

	BtlAgEventType type;		/* The type of the event */

	union
	{
		HfgCallbackParms *hfgParms;	/* Event's parameters when service is BTL_AG_SERVICE_HFAG */

		HsgCallbackParms *hsgParms; /* Event's parameters when service is BTL_AG_SERVICE_HSAG */
	}p;
};

/* Sniff Parameters */
typedef struct _HfgSniffParams
{
	TimeT sniffTimer;
	U16 minInterval;
	U16 maxInterval;
	U16 attempt;
	U16 timeout;
} HfgSniffParams;

/********************************************************************************
 *
 * Function Reference
 *
 *******************************************************************************/

 /*---------------------------------------------------------------------------
 * BTL_AG_Init()
 *
 * Brief:
 *	Initializes the AG Module.
 *
 * Description:
 *	This function initialize the BTL AG module and 
 *	must be called before any other API.
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	void
 *
 * Returns:
 *	BT_STATUS_SUCCESS - Initialization succeeded.
 *
 *	BT_STATUS_FAILED - Module already initialized.
 *
 *	BT_STATUS_INTERNAL_ERROR - Error allocating resources.
 */
BtStatus BTL_AG_Init(void);


/*---------------------------------------------------------------------------
 * BTL_AG_Deinit()
 *
 * Brief:
 *	Deinitializes the AG Module
 *
 * Description:
 *	This function deinitialize the BTL AG module.
 *	After this function is called no other AG functions can be called
 *	(Except BTL_AG_Init()).
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	void
 *
 * Returns:
 *	BT_STATUS_SUCCESS - Deinitialization succeeded.
 *
 *	BT_STATUS_FAILED - Module is not initialized.
 *
 *	BT_STATUS_BUSY - There are still AG contexts active. 
 */
BtStatus BTL_AG_Deinit(void);

/*---------------------------------------------------------------------------
 * BTL_AG_Create()
 *
 * Brief:
 *	Creates an AG Context.
 *
 * Description:
 *	Creates a AG context.
 *	This function must be called before any other BTL AG API function.
 *	The allocated context should be supplied in subsequent BTL AG API calls.
 *	The caller must also provide a callback function, which will be called 
 *	on AG events.
 *	The caller can provide an application handle (previously allocated 
 *	with BTL_RegisterApp), in order to link between different modules.
 *
 * Type:
 *      Synchronous.
 *
 * Parameters:
 *	appHandle [in] - Application handle, if there's no need for an application
 *		handle, set appHandle to 0.
 *
 *	callback [in] - All AG events will be sent to this callback.
 *
 *  secLevel [in] - The required security level.
 *		if NULL value is passed, the default security level is used.
 *
 *	context [out] - Allocated AG context.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *	BT_STATUS_FAILED - The operation failed because 
 *		the AG module was not initialized.
 *
 *	BT_STATUS_NO_RESOURCES - The operation failed because there
 *		are no resources for a new context.
 *
 *	BT_STATUS_INTERNAL_ERROR - Failed to register with the application handle.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly
 *		initialized.
 */
BtStatus BTL_AG_Create(BtlAppHandle *appHandle,
					   BtlAgCallback callback,
					   BtSecurityLevel *secLevel,
					   BtlAgContext **context);

/*---------------------------------------------------------------------------
 * BTL_AG_Destroy()
 *
 * Brief:
 *	Destroys an AG Context.
 *
 * Description:
 *	Destroys an AG context (previously created with BTL_AG_Create),
 *	An application should call this function when it completes using 
 * 	a AG context services.
 *
 * Type:
 *	 Synchronous.
 *
 * Parameters:
 *	context [in/out] - A AG context, the AG context is set to null in
 *		order to prevent the application from an illegal 
 *		attempt to keep using it.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *	BT_STATUS_BUSY - The operation failed because the context is still enabled.
 *
 *	BT_STATUS_INTERANL_ERROR - Internal error.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *		initialized.
 */
BtStatus BTL_AG_Destroy(BtlAgContext **context);



/*-------------------------------------------------------------------------------
 * BTL_AG_Enable()
 *
 * Brief:
 *	Enables an AG Context.
 *
 * Description:
 *	Enables an AG context, needs to be called to allow 
 *	incoming and outgoing connections.
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	context [in] - The context to enable.
 *
 *	serviceName [in] - A service name for the AG service.
 *		if null, the default service name is used.
 *
 *	features [in] - The AG features, relevant only for the handsfree service.
 *		if null, default features are used.
 *
 *	service [in] - A bit mask defining which services are active in the AG
 *		(hands free, headset or both).
 *
 * Returns:
 *	BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *	BT_STATUS_FAILED - Operation failed, either in registering a SDP 
 *		service or registering a RFCOMM channel.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *                         initialized.      
 */
BtStatus BTL_AG_Enable(BtlAgContext *context,
					   const char *serviceName,
					   AtAgFeatures *features,
					   BtlAgService service);


/*-------------------------------------------------------------------------------
 * BTL_AG_Disable()
 *
 * Brief:
 *	Disables an AG Context.
 *
 * Description:
 *	Disables an AG context, if there are existing connections
 *	to other devices they shall be released.
 *      
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context [in] - The context to disable.
 *
 * Generated Events:
 *	BTL_AG_EVENT_CONTEXT_DISABLED.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *	BT_STATUS_PENDING - Operation started successfully.
 *		BTL_AG_EVENT_CONTEXT_DISABLED is received when
 *		disabling has finished.
 *
 *	BT_STATUS_BUSY - The channel has an active SLC,
 *		disconnect the SLC before disabling.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly
 *		initialized (XA_ERROR_CHECK only).      
 */
BtStatus BTL_AG_Disable(BtlAgContext *context);

#if BT_SECURITY == XA_ENABLED
/*-------------------------------------------------------------------------------
 * BTL_AG_SetSecurityLevel()
 *
 * Brief:
 *	Sets an AG Context security level.
 *
 * Description:
 *	Sets an AG Context security level.
 *
 * Type:
 *	Synchronous
 *
 * Parameters:
 *	context [in] - An AG context.
 *
 *	secLevel [in] - the required security level, If NULL value is passed 
 *		the default security level is used.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - The operation succeeded.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *		initialized.
 */
BtStatus BTL_AG_SetSecurityLevel(BtlAgContext *context,
								 BtSecurityLevel *secLevel);

/*-------------------------------------------------------------------------------
 * BTL_AG_GetSecurityLevel()
 *
 * Brief:
 *	Gets an AG Context security level.
 *
 * Description:
 *	Gets an AG Context security level.
 *
 * Type:
 *	Synchronous
 *
 * Parameters:
 *	context [in] - An AG context.
 *
 *	secLevel [out] - the current security level
 *
 * Returns:
 *	BT_STATUS_SUCCESS - The operation succeeded.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly
 *     	initialized.
 */
BtStatus BTL_AG_GetSecurityLevel(BtlAgContext *context,
								 BtSecurityLevel *secLevel);

#endif /* BT_SECURITY == XA_ENABLED */


/*-------------------------------------------------------------------------------
 * BTL_AG_GetService()
 *
 * Brief:
 *	Gets the active service(s) within the AG Context.
 *
 * Description:
 *	Gets the active services within the AG context.
 *	the active services can be: Hands-free AG, Headset AG or both.
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	context [in] - An AG Context.
 *
 *	service [out] - Indicates which services are active in the AG
 *		see "BtlAgService" description for further details.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - The operation finished successfully.
 *		'service' is valid.
 *
 *	BT_STATUS_INVALID_PARM - Invalid parameter.
 */
BtStatus BTL_AG_GetService(BtlAgContext *context,
						   BtlAgService *service);



/*---------------------------------------------------------------------------
 * BTL_AG_SetFeatures()
 *
 * Brief:
 *	Set an AG Context features.
 *
 * Description:
 *	Sets the features supported by the AG.
 *	The AG features are used to inform the HF Unit of the supported
 *	capabilities within the AG.
 *	And is updated in the SDP record and also used with the "+BRSF" response.
 *	This API is relevant only for the hands-free part of the AG.
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	context [in] - An AG context.
 *
 *	features [in] - a bit mask defining a set of features, 
 *		constructed using a bitwise OR of the required features.
 *		If NULL is passed, the default features value,
 *		HFG_DEFAULT_FEATURES is used.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *	BT_STATUS_FAILED - The operation failed.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly
 *		initialized.
 */
BtStatus BTL_AG_SetFeatures(BtlAgContext *context,
							const AtAgFeatures *features);

/*---------------------------------------------------------------------------
 * BTL_AG_GetFeatures()
 *
 * Brief:
 *	Gets the AG Context features.
 *
 * Description:
 *	Gets the AG Context features.
 *	See BTL_AG_SetFeatures() for further details.
 *	This API is relevant only for the handsfree part of the AG.
 *      
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	context [in] - An AG context.
 *
 *	features [out] - A bit mask defining the set of features.
 *
 * Retures:
 *	BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly
 *		initialized.
 */
BtStatus BTL_AG_GetFeatures(const BtlAgContext *context,
							AtAgFeatures *features);



/*---------------------------------------------------------------------------
 * BTL_AG_GetNumOfChannels()
 *
 * Brief:
 *	Gets the number of active channels within the AG Context.
 *
 * Description:
 *	Gets the number of active channels within the AG Context.
 *	Active channel means established SLC connection to a HF Unit
 *	are an established RFCOMM connection with a HS Unit.
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	context [in] - An AG context.
 *
 *	numChannels [out] - the number of channels within the context.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - operation successful.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly
 *     	initialized.
 */
BtStatus BTL_AG_GetNumOfChannels(BtlAgContext *context,
								 U32 *numChannels);

/*---------------------------------------------------------------------------
 * BTL_AG_ConnectSLC()
 *
 * Brief:
 *	Establishes a connection to a hands-free or a headset unit.
 *
 * Description:
 *	Establishes an SLC with the hands-free/headset unit.
 *	This includes performing SDP Queries to find the appropriate service
 *	and opening an RFCOMM channel.  The success of the operation is 
 *	indicated by the BTL_AG_EVENT_SERVICE_CONNECTED event.
 *	The field channelId in BtlAgEvent is an identifier for the new connection,
 *	The channelId is used to identify the connection when sending AT commands,
 *	establishing audio and etc.
 *
 *	If the connection fails, the application is notified by the
 *	BTL_AG_EVENT_SERVICE_DISCONNECTED event.
 *
 *	If an ACL link does not exist to the audio gateway, one will be
 *	created first.  If desired, however, the ACL link can be established
 *	prior to calling this function.
 *
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context [in] - a AG context.
 *
 *	bdAddr [in] - The Bluetooth address of the remote device.
 *
 * Generated Events:
 *	BTL_AG_EVENT_SERVICE_CONNETED.
 *	BTL_AG_EVENT_SERVICE_DISCONNECTED.
 *
 * Returns:
 *	BT_STATUS_PENDING - The operation has started, the application will be
 *		notified when the connection has been created (via
 *		the callback function supplied in BTL_AG_Create).
 *
 *	BT_STATUS_BUSY - The operation failed because a SLC is already
 *		open to the remote device, or a new SLC is being
 *		created.
 *
 *	BT_STATUS_CONNECTION_FAILED - The SLC establishment failed.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *		initialized.
 */
BtStatus BTL_AG_ConnectSLC(BtlAgContext *context,
						   BD_ADDR *bdAddr);

/*---------------------------------------------------------------------------
 * BTL_AG_ConnectSLCAndGetChannelId()
 *
 * Brief:
 *	Establishes a connection to a hands-free or a headset unit and immediately
 *  returns ID of a channel which will hold the connection and which should be
 *  used in all subsequent API calls in order to control this connection.
 *
 * Description:
 *	Establishes an SLC with the hands-free/headset unit.
 *	This includes performing SDP Queries to find the appropriate service
 *	and opening an RFCOMM channel.  The success of the operation is 
 *	indicated by the BTL_AG_EVENT_SERVICE_CONNECTED event.
 *	The field channelId in BtlAgEvent is an identifier for the new connection,
 *	The channelId is used to identify the connection when sending AT commands,
 *	establishing audio and etc.
 *
 *	If the connection fails, the application is notified by the
 *	BTL_AG_EVENT_SERVICE_DISCONNECTED event.
 *
 *	If an ACL link does not exist to the audio gateway, one will be
 *	created first.  If desired, however, the ACL link can be established
 *	prior to calling this function.
 *
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context [in] - a AG context.
 *
 *	bdAddr [in] - The Bluetooth address of the remote device.
 *
 *	channelId [out] - the Id of the channel that will hold the connection.
 *
 * Generated Events:
 *	BTL_AG_EVENT_SERVICE_CONNETED.
 *	BTL_AG_EVENT_SERVICE_DISCONNECTED.
 *
 * Returns:
 *	BT_STATUS_PENDING - The operation has started, the application will be
 *		notified when the connection has been created (via
 *		the callback function supplied in BTL_AG_Create).
 *
 *	BT_STATUS_BUSY - The operation failed because a SLC is already
 *		open to the remote device, or a new SLC is being
 *		created.
 *
 *	BT_STATUS_CONNECTION_FAILED - The SLC establishment failed.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *		initialized.
 */
BtStatus BTL_AG_ConnectSLCAndGetChannelId(BtlAgContext *context,
						                  BD_ADDR *bdAddr,
                                          BtlAgChannelId *channelId);

/*---------------------------------------------------------------------------
 * BTL_AG_DisconnectSLC()
 *
 * Brief:
 *	Disconnects a connection with a hands-free or a headset unit.
 *
 * Description:
 *	Releases the SLC with the hands-free unit. This will 
 *	close the RFCOMM channel and will also close the SCO and ACL links
 *	if no other services are active, and no other contexts are in use.
 *	When the operation is complete the application will be notified by the
 *  BTL_AG_EVENT_AUDIO_DISCONNECTED (if audio connection was active) and by the
 *  BTL_AG_EVENT_SERVICE_DISCONNECTED event.
 *
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context [in] - The AG context.
 *
 *	channelId [in] - The Id of the channel holding the SLC
 *
 * Generated Events:
 *	BTL_AG_EVENT_AUDIO_DISCONNECTED (if audio connection was active),
 *	BTL_AG_EVENT_SERVICE_DISCONNECTED.
 *
 * Returns:
 *	BT_STATUS_PENDING - The operation has started, the application 
 *		will be notified when the SLC is down (via the
 *		callback function given in BTL_AG_Create).
 *
 *	BT_STATUS_FAILED - SLC is not connected.
 *
 *	BT_STATUS_BUSY - SLC disconnection in progress.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *		initialized.
 */
BtStatus BTL_AG_DisconnectSLC(BtlAgContext *context,
							  BtlAgChannelId channelId);

/*---------------------------------------------------------------------------
 * BTL_AG_GetChannelState()
 *
 * Brief:
 *	Gets the current state of an AG Channel.
 *
 * Description:
 *	Gets the current state of an AG Channel.
 *	See the "BtlAgChannelState" description for further details.
 *  
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	context [in] - An AG context.
 *
 *	channelId [in] - The channel Id.
 *
 *	channelState [out] - The channel's state.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - operation successful.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *     	initialized.
 */
BtStatus BTL_AG_GetChannelState(BtlAgContext *context,
								BtlAgChannelId channelId,
								BtlAgChannelState *state);

/*---------------------------------------------------------------------------
 * BTL_AG_SetAudioParms()
 *
 * Brief:
 *	Sets audio connection parameters for an AG Channel.
 *
 * Description:
 *	Sets audio connection parameters for the specified channel.
 *	See 'CmgrAudioParms' description in conmgr.h, fur further details.
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	context [in] - An AG context.
 *
 *	channelId [in] - A AG channel id.
 *
 *	parms [in] - The audio parameters, if NULL then the default value is used.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - Operation completed successfully.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly initialized.
 */
BtStatus BTL_AG_SetAudioParms(BtlAgContext *context,
							  BtlAgChannelId channelId,
							  CmgrAudioParms *parms);

/*---------------------------------------------------------------------------
 * BTL_AG_GetAudioParms()
 *
 * Brief:
 *	Gets the audio connection parameters of an AG Channel.
 *
 * Description:
 *	Gets audio connection parameters of the specified channel.
 *	See 'CmgrAudioParms' description in conmgr.h, fur further details.
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	context [in] - An AG context.
 *
 *	channelId [in] - A AG channel id.
 *
 *	parms [out] - The audio connection parameters.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - Operation completed successfully.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly initialized.
 */
BtStatus BTL_AG_GetAudioParms(BtlAgContext *context,
							  BtlAgChannelId channelId,
							  CmgrAudioParms *parms);

/*---------------------------------------------------------------------------
 * BTL_AG_SetAudioFormat()
 *
 * Brief:
 *	Sets the air and input audio format of an AG Channel.
 *
 * Description:
 *	Sets the air and input audio format the specified channel.
 *	See 'BtScoAudioSettings' in mesco.h for further details.
 *
 *	If the audio parameters are 
 *	CMGR_AUDIO_PARMS_S1, CMGR_AUDIO_PARMS_S2 or CMGR_AUDIO_PARMS_S3
 *	Then the air format is already defined and cannot be set.
 *	
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	context [in] - An AG context.
 *
 *	channelId [in] - A AG channel id.
 *
 *	format [in] - The audio format, if NULL then the default value is used.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - Operation completed successfully.
 *
 *	BT_STATUS_FAILED - Format includes air format while audio parameters
 *		are: CMGR_AUDIO_PARMS_S1, CMGR_AUDIO_PARMS_S2 or CMGR_AUDIO_PARMS_S3.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly initialized.
 */
BtStatus BTL_AG_SetAudioFormat(BtlAgContext *context,
							   BtlAgChannelId channelId,
							   BtScoAudioSettings *format);

/*---------------------------------------------------------------------------
 * BTL_AG_GetAudioFormat()
 *
 * Brief:
 *	Gets the air and input audio format of an AG Channel.
 *
 * Description:
 *	Gets the audio format of the specified channel.
 *	See 'BtScoAudioSettings' in mesco.h for further details.
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	context [in] - AG context.
 *
 *	channelId [in] - An AG channel id.
 *
 *	format [out] - The audio format.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - Operation completed successfully.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly initialized.
 */
BtStatus BTL_AG_GetAudioFormat(BtlAgContext *context,
							   BtlAgChannelId channelId,
							   BtScoAudioSettings *format);

/*---------------------------------------------------------------------------
 * BTL_AG_ConnectAudio()
 *
 * Brief:
 *	Connect audio to a remote device.
 *
 * Description: 
 *	Establishes an audio link to the hands-free unit.
 *	The success of the operation is indicated by the 
 *	BTL_AG_EVENT_AUDIO_CONNECTED event.
 *	If the connection fails, the application is notified by the
 *	BK_AG_EVENT_AUDIO_DISCONNECTED event.
 *	Establishing an audio connection requires that a SLC connection
 *	will be already established.
 *
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context [in] - AG context.
 *
 *	channelId [in] - the channel holding for the audio connection.
 *
 * Generated Events:
 *	BTL_AG_EVENT_AUDIO_CONNECTED.
 *	BTL_AG_EVENT_AUDIO_DISCONNECTED.
 *
 * Returns:
 *	BT_STATUS_PENDING - The operation has started, the application will be
 *		notified when the audio link has been established.
 *
 *	BT_STATUS_SUCCESS - The audio (SCO) connection already exists.
 *
 *	BT_STATUS_NO_CONNECTION - The operation failed because a service level 
 *		connection does not exist to the audio gateway.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly initialized.
 */
BtStatus BTL_AG_ConnectAudio(BtlAgContext *context,
							 BtlAgChannelId channelId);

/*---------------------------------------------------------------------------
 * BTL_AG_DisconnectAudio()
 *
 * Brief:
 *	Disconnect an audio connection with a remote device.
 *
 * Description:
 *	Releases the audio connection with the hands-free unit.  When the
 *	operation is complete, the application will be notified by the
 *	BTL_AG_EVENT_AUDIO_DISCONNECTED event.
 *
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context [in] - AG context.
 *
 *	channelId [in] - The channel holding the audio connection.
 *
 * Generated Events:
 *	BTL_AG_EVENT_AUDIO_DISCONNECTED.
 *
 * Returns:
 *	BT_STATUS_PENDING - The operation has started, the application will be
 *		notified when the audio connection is down.
 *
 *	BT_STATUS_NO_CONNECTION - The operation failed because an audio
 *		connection does not exist.
 *
 *	BT_STATUS_FAILED - The operation failed because a SLC does not exist.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly
 *		initialized.
 */
BtStatus BTL_AG_DisconnectAudio(BtlAgContext *context,
								BtlAgChannelId channelId);

/*---------------------------------------------------------------------------
 * BTL_AG_GetConnectedDevice()
 *
 * Brief:
 *	Gets the BD address of a connected device.
 *
 * Description:
 *	Gets the BD address of a connected device.
 *
 * Type:
 *	Synchronous.
 *
 * Parameter:
 *	context [in] - The AG context.
 *
 *	channelId [in] - The channel representing the connection to the remote device.
 *
 *	bdAddr [out] - The address of the connected device, valid if BT_STATUS_SUCCESS
 *		is returned.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - Operation is successful.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid.
 *
 *	BT_STATUS_NO_CONNECTION - The channel is not used.
 */
BtStatus BTL_AG_GetConnectedDevice(BtlAgContext *context,
								   BtlAgChannelId channelId,
								   BD_ADDR *bdAddr);

#if BT_SCO_HCI_DATA == XA_ENABLED
/*---------------------------------------------------------------------------
 * BTL_AG_SendAudioData()
 *
 * Brief:
 *	This function is intended for VOHCI support and is not yet implemented.
 *
 * Description:
 *	Send audio connection data over the host controller,
 *	"Voice over HCI".
 *
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context [in] - AG context.
 *
 *	channelId [in] - The channel ID
 *
 *	packet [in] - The packet of data to send.
 *
 * Generated Events:
 *	BTL_AG_EVENT_AUDIO_DATA_SENT.
 *
 * Returns:
 *	BT_STATUS_PENDING - Operation started successfully.
 *
 *	BT_STATUS_NO_CONNECTION - No audio connection exists.
 *
 *	BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus BTL_AG_SendAudioData(BtlAgContext *context,
							  BtlAgChannelId channelId,
							  BtPacket *packet);
#endif

/*---------------------------------------------------------------------------
 * BTL_AG_SendOK()
 *
 * Brief:
 *	Sends "OK" AT response to a remote device.
 *
 * Description:
 *	Sends "OK" AT response to a remote device.
 *
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context [in] - AG context.
 *
 *	channelId [in] - The channel ID.
 *
 * Returns:
 *	BT_STATUS_PENDING - The operation has started.
 *
 *	BT_STATUS_NO_CONNECTION - A SLC does not exist.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *		initialized.
 */
BtStatus BTL_AG_SendOK(BtlAgContext *context,
					   BtlAgChannelId channelId);

/*---------------------------------------------------------------------------
 * BTL_AG_SendError()
 *
 * Brief:
 *	Sends error AT response to a remote device.
 *
 * Description:
 *	Sends error AT response to a remote device.
 *	If the remote device is a HS Unit or a HF Unit with extended errors
 *	disabled, "ERROR" is sent.
 *	If the remote device is a HF Unit with extended errors enabled.
 *	+CME ERROR: <n> is sent to the HF Unit where <n> is the error code
 *	supplied by the user.
 *
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context [in] - AG context.
 *
 *	channelId [in] - The channel id
 *
 *	error [in] - Extended error to be sent (if enabled).
 *
 * Generated Events:
 *	BTL_AG_EVENT_RESPONSE_COMPLETE.
 *
 * Returns:
 *	BT_STATUS_PENDING - The operation has started.
 *
 *	BT_STATUS_NO_CONNECTION - A SLC does not exist.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *		initialized.
 */
BtStatus BTL_AG_SendError(BtlAgContext *context,
						  BtlAgChannelId channelId,
						  HfgCmeError error);

/*---------------------------------------------------------------------------
 * BTL_AG_SendRing()
 *
 * Brief:
 *	Send "RING" to the remote device.
 *
 * Description:
 *	Send "RING" to the remote device.
 *
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context [in] - AG context.
 *
 *	channelId [in] - The channel Id.
 *
 * Generated Events:
 *	BTL_AG_EVENT_RESPONSE_COMPLETE.
 *
 * Returns:
 *	BT_STATUS_PENDING - The operation has started.
 *
 *	BT_STATUS_NO_CONNECTION - A SLC does not exist.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *		initialized.
 */
BtStatus BTL_AG_SendRing(BtlAgContext *context,
						 BtlAgChannelId channelId);

/*---------------------------------------------------------------------------
 * BTL_AG_SendMicVolume()
 *
 * Brief:
 *	Updates the microphone volume of the remote device.
 *
 * Description:
 *	Updates the microphone volume of the remote device.
 *
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context [in] - AG context.
 * 
 *	channelId [in] - The channel ID.
 *
 *	gain [in] - current volume level, must be in the range of 0 to 15.
 *
 * Generated Events:
 *	BTL_AG_EVENT_RESPONSE_COMPLETE.
 *
 * Returns:
 *	BT_STATUS_PENDING - The operation has started.
 *
 *	BT_STATUS_NO_CONNECTION - A SLC does not exist.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly
 *		initialized.
 */
BtStatus BTL_AG_SendMicVolume(BtlAgContext *context,
							  BtlAgChannelId,
							  U32 gain);

/*---------------------------------------------------------------------------
 * BTL_AG_SendSpeakerVolume()
 *
 * Brief:
 *	Updates the speaker volume of the remote device.
 *
 * Description:
 *	Updates the speaker volume of the remote device.
 *
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context [in] - AG context.
 *
 *	channelId [in] - The channel Id.
 *
 *	gain [in] - current volume level, must be in the range of 0 to 15.
 *
 * Generated Events:
 *	BTL_AG_EVENT_RESPONSE_COMPLETE.
 *
 * Returns:
 *	BT_STATUS_PENDING - The operation has started.
 *
 *	BT_STATUS_NO_CONNECTION - A SLC does not exist.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *		initialized.
 */BtStatus BTL_AG_SendSpeakerVolume(BtlAgContext *context,
								  BtlAgChannelId,
								  U32 gain);

/*---------------------------------------------------------------------------
 * BTL_AG_SendAtResult()
 *
 * Brief:
 *	Sends an AT command to the remote device.
 *
 * Description:
 *	Sends any AT result.  The 'AtString' parameter must be initialized,
 *	and the AT result must be formatted properly.  It is not necessary
 *	to add CR/LF at the beginning and end of the string.
 *
 *	When the AT result has been sent, the BTL_AG_EVENT_RESPONSE_COMPLETE
 *	event will be received by the application's callback function.
 *
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context [in] - AG context.
 *
 *	channelId [in] - The channel ID.
 *
 *	atString [in] - A properly formatted AT command response.
 *
 * Generated Events:
 *	BTL_AG_EVENT_RESPONSE_COMPLETE.
 *   
 * Returns:
 *	BT_STATUS_PENDING - The operation has started, the application will be
 *		notified when the response has been sent.
 *
 *	BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *		does not exist to the audio gateway.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly
 *		initialized.
 */
BtStatus BTL_AG_SendAtResult(BtlAgContext *context,
							 BtlAgChannelId channelId,
							 const char *atString);


/*---------------------------------------------------------------------------
 *
 *	The following functions are relevant only for HF connections.
 *	Calling those functions with an HS connection results in error.
 *
 *---------------------------------------------------------------------------/


 *---------------------------------------------------------------------------
 * BTL_AG_SetIndicatorValue()
 *
 * Brief:
 *	Sets an indicator's value.
 *
 * Description:
 *	Sets the current value for an indicator.  If a SLC
 *	is active and indicator reporting is currently enabled, the state 
 *	of the modified indicator is reported to the hands-free device. 
 *	If no SLC exists, the current value is changed and will
 *	be reported during the establishment of the SLC.
 *	If indicator reporting is disabled, the value of the indicator will
 *	only be reported when requested by the hands-free unit (AT+CIND).
 *
 *	Upon registration of an Audio Gateway (BTL_AG_Enable()),
 *	all indicators are initialized to 0.  To properly initialize all 
 *	indicators, this function must be called once for each indicator 
 *	prior to establishing a SLC.
 *
 *	Indicators must be sent to the hands-free device as specified by the
 *	hands-free v1.5 specification.  Indicators are sent in the order that
 *	calls are made to this function.
 *
 * Type:
 *	Asynchronous or Synchronous.
 *  
 * Parameters:
 *	context [in] - AG context.
 *
 *	indicator [in] - Indicator type.
 *
 *	value [in] - The value of the indicator.
 *
 * Generated Events:
 *	BTL_AG_EVENT_RESPONSE_COMPLETE.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - The indicator value was set.
 *
 *	BT_STATUS_PENDING - The indicator value is set and being sent to the 
 *		handsfree unit. When the response has been sent,
 *		the BTL_AG_EVENT_RESPONSE_COMPLETE event will be received 
 *		by the application.
 *
 *	BT_STATUS_NO_CONNECTION - A SLC does not exist.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly
 *		initialized.
 */
BtStatus BTL_AG_SetIndicatorValue(BtlAgContext *context,
								  HfgIndicator indicator, 
								  U8 value);

/*---------------------------------------------------------------------------
 * BTL_AG_GetIndicatorValue()
 *
 * Brief:
 *	Gets the value of an indicator.
 *
 * Description:
 *	Gets the value of an indicator.
 *
 * Type:
 *	Synchronous
 *
 * Parameters:
 *	context [in] - AG context.
 *
 *	indicator [in] - Indicator type.
 *
 *	value [out] - Receives the value of the indicator.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - operation successful.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly
 *		initialized.
 */					  
BtStatus BTL_AG_GetIndicatorValue(BtlAgContext *context,
								  HfgIndicator indicator, 
								  U8 *value);

/*---------------------------------------------------------------------------
 * BTL_AG_SendCallListRsp()
 *
 * Brief:
 *	Send a list of the current calls in the AG.
 *
 * Description:
 *	Sends the call listing response to the hands-free device (see
 *	BTL_AG_EVENT_LIST_CURRENT_CALLS).  This function should be called for each
 *	line supported on the audio gateway with the state of any call set
 *	appropriately.  If no call is active on the specified line, a response 
 *	(+CLCC) will not be sent.  If a call is any state besides 
 *	BTL_AG_CALL_STATUS_NONE, then a response will be sent.  On the final call
 *	to this function, FinalFlag should be set.  This will send an OK 
 *	response in addtion to +CLCC (if sent).
 *
 *	If it is known that no call exists on any line, it is acceptable to 
 *	call BTL_AG_SendOK() instead of calling this function.
 *
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context [in] - AG context.
 *
 *	channelId [in] - The channel Id.
 *
 *	parms [in] - A structure containing the call status information for the
 *		specified line.
 *
 *	finalFlag [in] -  Set to TRUE when the final call is make to this
 *		function.
 *
 * Generated Events:
 *	BTL_AG_EVENT_RESPONSE_COMPLETE.
 *
 * Returns:
 *	BT_STATUS_PENDING - The operation has started.
 *
 *	BT_STATUS_NO_CONNECTION - A SLC does not exist.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly
 *		initialized.
 */
BtStatus BTL_AG_SendCallListRsp(BtlAgContext *context,
								BtlAgChannelId channelId,
								const HfgCallListParms *parms, 

								BOOL finalFlag);
/*---------------------------------------------------------------------------
 * BTL_AG_SendCallerId()
 *
 * Brief:
 *	Sends the calling line Id (+CLIP).
 *
 * Description:
 *	Sends a Calling Line Identification AT command result code containing
 *	the phone number and type of the incoming call.  This function should
 *	be called immediately after BTL_AG_SendRing() if 
 *	Calling Line Identification Notification has been enabled by the HF.
 *	If caller ID notification has been disabled by the remote device,
 *	no notification will be sent even if this function is called.
 *
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context [in] - AG context.
 *
 *	channelId [in] - The channel ID.
 *
 *	number [in] - Phone number of the caller.
 *
 *	type [in] - Type of the incoming call.
 *
 * Generated Events:
 *	BTL_AG_EVENT_RESPONSE_COMPLETE
 *
 * Returns:
 *	BT_STATUS_PENDING - The operation has started.
 *
 *	BT_STATUS_NOT_SUPPORTED - Caller ID notification is disabled.
 *
 *	BT_STATUS_NO_CONNECTION - A SLC does not exist.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly
 *		initialized.
 */
BtStatus BTL_AG_SendCallerId(BtlAgContext *context,
							 BtlAgChannelId channelId,
							 const char *number,
							 U8 type);

/*---------------------------------------------------------------------------
 * BTL_AG_SendSubscriberNumberRsp()
 *
 * Brief:
 *	Sends the subscriber number.
 *
 * Description:
 *	This function is called in response to a request for the subscriber
 *	number (see BTL_AG_EVENT_QUERY_SUBSCRIBER_NUMBER).  It is not necessary
 *	to call BTL_AG_SendOK() after calling this function.
 * 
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context [in] - AG context.
 *
 *	channelId [in] - The channel Id.
 *
 *	number [in] - phone number of format specified by type.
 *
 *	type [in] - phone number format (see atp.h).
 *
 *	service [in] - service related to the phone number.
 *
 * Generated Events:
 *	BTL_AG_EVENT_RESPONSE_COMPLETE
 *
 * Returns:
 *	BT_STATUS_PENDING - The operation has started.
 *
 *	BT_STATUS_NO_CONNECTION - A SLC does not exist.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly
 *		initialized.
 */
BtStatus BTL_AG_SendSubscriberNumberRsp(BtlAgContext *context,
										BtlAgChannelId channelId,
										HfgSubscriberNum *number);

/*---------------------------------------------------------------------------
 * BTL_AG_SendNetworkOperatorRsp()
 *
 * Brief:
 *	Sends the network operator information.
 *
 * Description:
 *	This function is called in response to a request for the network
 *	operator information (see BTL_AG_EVENT_QUERY_NETWORK_OPERATOR).
 *	It is not necessary to call BTL_AG_SendOK() after calling this function.
 *
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context [in] - AG context.
 *
 *	channelId [in] - The channel Id.
 *
 *	operator [in] - A structure representing the operator name.
 *
 * Generated Events:
 *	BTL_AG_EVENT_RESPONSE_COMPETE.
 *
 * Returns:
 *	BT_STATUS_PENDING - The operation has started.
 *
 *	BT_STATUS_NO_CONNECTION - A SLC does not exist.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly
 *		initialized.
 */
BtStatus BTL_AG_SendNetworkOperatorRsp(BtlAgContext *context,
									   BtlAgChannelId channelId,
									   HfgNetworkOperator *oper);

/*---------------------------------------------------------------------------
 * BTL_AG_EnableInbandRingTone()
 *
 *	Notifies the HF of in-band ring tone status
 *	and enables inband ring tone for a AG channel.
 *
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context [in] - AG context.
 *
 *	channelId [in] - The channel ID.
 *
 *	enabled [in] - TRUE if in-band ring tone enabled, FALSE otherwise.
 *
 * Generated Events:
 *	BTL_AG_EVENT_RESPONSE_COMPLETE.
 *
 * Returns:
 *	BT_STATUS_PENDING - The operation has started.
 *
 *	BT_STATUS_NO_CONNECTION - A SLC does not exist.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly
 *		initialized.
 */
BtStatus BTL_AG_EnableInbandRingTone(BtlAgContext *context,
									 BtlAgChannelId channelId,
									 BOOL enabled);
#if HFG_USE_RESP_HOLD == XA_ENABLED
/*---------------------------------------------------------------------------
 * BTL_AG_SendResponseHoldState()
 *
 * Brief:
 *	Sends the current response and hold state.
 *
 * Description:
 *	Notifies the HF of state of Response and Hold.  This function is called
 *	to report the Response and Hold state in response to a request by
 *	the hands-free unit (see HFG_RESPONSE_HOLD), or upon an action taken on
 *	the audio gateway.
 *
 *	This function is also called in response to a query for the Response
 *	and Hold state from the hands-free unit (see BTL_AG_QUERY_RESPONSE_HOLD).
 *	This function should be called with the 'state' parameter set to 
 *	HFG_RESP_HOLD_STATE_HOLD if the audio gateway is in the Response and
 *	Hold state, followed by a call to BTL_AG_SendOK().  Otherwise, the
 *	application should simply call BTL_AG_SendOK().
 *
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context [in] - AG context.
 *
 *	channelId [in] - The channel Id.
 *
 *	state [in] - The current Response and Hold state,
 *		see type declaration for possible values.
 *
 * Generated Events:
 *	BTL_AG_EVENT_RESPONSE_COMPLETE.
 *
 * Returns:
 *	BT_STATUS_PENDING - The operation has started.
 *
 *	BT_STATUS_NO_CONNECTION - A SLC does not exist.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *		initialized.
 */
BtStatus BTL_AG_SendResponseHoldState(BtlAgContext *context,
									  BtlAgChannelId channelId,
									  HfgResponseHold state);
#endif /* HFG_USE_RESP_HOLD == XA_ENABLED */

/*---------------------------------------------------------------------------
 * BTL_AG_SendCallWaiting()
 *
 * Brief:
 *	Sends call waiting notification (+CCWA)
 *
 * Description:
 *	Sends call waiting notification (+CCWA)
 *
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context [in] - AG context.
 *
 *	channelId [in] - the channel ID.
 *
 *	number [in] - Phone number of the waiting call.
 *
 *	classMap [in] - voice parameters.
 *
 *	type [in] - Type of address.
 *
 * Generated Events:
 *	BTL_AG_EVENT_RESPONSE_COMPLETE.
 *
 * Returns:
 *	BT_STATUS_PENDING - The operation has started.
 *
 *	BT_STATUS_NO_CONNECTION - A SLC does not exist.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *		initialized.
 */
BtStatus BTL_AG_SendCallWaiting(BtlAgContext *context,
								BtlAgChannelId channelId,
								HfgCallWaitParms *parms);

/*---------------------------------------------------------------------------
 * BTL_AG_SendEnableVoiceRec()
 *
 * Brief:
 *	Notifes the HF Unit about the current status of Voice Recognition.
 *
 * Description:
 *	Notifies the HF that voice recognition has been enabled/disabled 
 *	(if the HF has activated the voice recognition functionality in the AG)
 *
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context [in] - AG context.
 *
 *	channelId [in] - The channel Id.
 *
 *	enabled - TRUE if voice recognition is active, otherwise FALSE.
 *
 * Generated Events:
 *	BTL_AG_EVENT_RESPONSE_COMPLETE.
 *
 * Returns:
 *	BT_STATUS_PENDING - The operation has started.
 *
 *	BT_STATUS_NO_CONNECTION - A SLC does not exist.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *		initialized.
 */
BtStatus BTL_AG_SendEnableVoiceRec(BtlAgContext *context,
								   BtlAgChannelId channelId,
								   BOOL enabled);

/*---------------------------------------------------------------------------
 * BTL_AG_SendVoiceTagRsp()
 *
 * Brief:
 *	Sends a number that is associated with a voice tag.
 *
 * Description:
 *	Called by the app to return the phone number associated with the
 *	VoiceTag request to the HF.  It is not necessary to call 
 *	BTL_AG_SendOK() after calling this function.
 *
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context [in] - AG context.
 *
 *	channelId [in] - The channel Id.
 *
 *	number [in] - A structure containing the phone number associated 
 *		with the last voice tag.
 *
 * Generated Events:
 *	BTL_AG_EVENT_RESPONSE_COMPLETE.
 *
 * Returns:
 *	BT_STATUS_PENDING - The operation has started.
 *
 *	BT_STATUS_NO_CONNECTION - A SLC does not exist.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *		initialized.
 */
BtStatus BTL_AG_SendVoiceTagRsp(BtlAgContext *context,
								BtlAgChannelId channelId,
								const char *number);

/*---------------------------------------------------------------------------
 * BTL_AG_SendNoCarrierRsp()
 *
 * Brief:
 *	Sends "NO CARRIER".
 *	
 * Description:
 *	Called by the app to send the "NO CARRIER" response to the HF.  This
 *	response can be sent in addition to the "+CME ERROR:" response.
 *
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context [in] - AG context.
 *
 *	channelId [in] - The channel Id.
 *
 * Generated Events:
 *	BTL_AG_EVENT_RESPONSE_COMPLETE.
 *
 * Returns:
 *	BT_STATUS_PENDING - The operation has started.
 *
 *	BT_STATUS_NO_CONNECTION - A SLC does not exist.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *		initialized.
 */
BtStatus BTL_AG_SendNoCarrierRsp(BtlAgContext *context,
								 BtlAgChannelId channelId);

/*---------------------------------------------------------------------------
 * BTL_AG_SendBusyRsp()
 *
 * Brief:
 *	Sends "BUSY".
 *
 * Description:
 *	Called by the app to send the "BUSY" response to the HF.  This
 *	response can be sent in addition to the "+CME ERROR:" response.
 *
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context [in] - AG context.
 *
 *	channelId [in] - The channel Id.
 *
 * Generated Events:
 *	BTL_AG_EVENT_RESPONSE_COMPLETE.
 *
 * Returns:
 *	BT_STATUS_PENDING - The operation has started.
 *
 *	BT_STATUS_NO_CONNECTION - A SLC does not exist.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly
 *		initialized.
 */
BtStatus BTL_AG_SendBusyRsp(BtlAgContext *context,
							BtlAgChannelId channelId);

/*---------------------------------------------------------------------------
 * BTL_AG_SendNoAnswerRsp()
 *
 * Brief:
 *	Sends "NO ANSWER".
 *
 * Description:
 *	Called by the app to send the "NO ANSWER" response to the HF.  This
 *	response can be sent in addition to the "+CME ERROR:" response.
 *
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context [in] - AG context.
 *
 *	channelId [in] - The channel Id.
 *
 * Generated Events:
 *	BTL_AG_EVENT_RESPONSE_COMPLETE.
 *
 * Returns:
 *	BT_STATUS_PENDING - The operation has started.
 *
 *	BT_STATUS_NO_CONNECTION - A SLC does not exist.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly
 *		initialized.
 */
BtStatus BTL_AG_SendNoAnswerRsp(BtlAgContext *context,
								BtlAgChannelId channelId);

/*---------------------------------------------------------------------------
 * BTL_AG_SendDelayedRsp()
 *
 * Brief:
 *	Sends "DELAYED".
 *
 * Description:
 *	Called by the app to send the "DELAYED" response to the HF.  This
 *	response can be sent in addition to the "+CME ERROR:" response.
 *
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context [in] - AG context.
 *
 *	channelId [in] - The channel Id.
 *
 * Generated Events:
 *	BTL_AG_EVENT_RESPONSE_COMPLETE.
 *
 * Returns:
 *	BT_STATUS_PENDING - The operation has started.
 *
 *	BT_STATUS_NO_CONNECTION - A SLC does not exist.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *		initialized.
 */
BtStatus BTL_AG_SendDelayedRsp(BtlAgContext *context,
							   BtlAgChannelId channelId);

/*---------------------------------------------------------------------------
 * BTL_AG_SendBlacklistedRsp()
 *
 * Brief:
 *	Send "BLACKLISTED".
 *
 * Description:
 *	Called by the app to send the "BLACKLISTED" response to the HF.
 *	This response can be sent in addition to the "+CME ERROR:" response.
 *
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context [in] - AG context.
 *
 *	channelId [in] - The channel Id.
 *
 * Generated Events:
 *	BTL_AG_EVENT_RESPONSE_COMPLETE.
 *
 * Returns:
 *	BT_STATUS_PENDING - The operation has started.
 *
 *	BT_STATUS_NO_CONNECTION - A SLC does not exist.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *		initialized.
 */
BtStatus BTL_AG_SendBlacklistedRsp(BtlAgContext *context,
								   BtlAgChannelId channelId);

/*---------------------------------------------------------------------------
 * BTL_AG_IsNoiseReductionEnabled()
 *
 * Brief:
 *	Queries if noise reduction is enabled.
 *
 * Descriptoin:
 *	Returns an answer saying whether noise reduction is enabled
 *	or not.
 *
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context [in] - AG context.
 *
 *	ans [out] - TRUE if noise reduction is enabled, FALSE otherwise
 *
 * Returns:
 *	BT_STATUS_SUCCESS - operation successful.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *		initialized.
 */
BtStatus BTL_AG_IsNoiseReductionEnabled(BtlAgContext *context,
										BtlAgChannelId channelId,
										BOOL *enabled);

/*---------------------------------------------------------------------------
 * BTL_AG_IsInbandRingEnabled()
 *
 * Brief:
 *	Queries if inband ring tone is enabled.
 *
 * Description:
 *	Returns an answer saying whether inband ringing is enabled
 *	or not.
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	context [in] - AG context.
 *
 *	ans [out] - TRUE if inband ringing is enabled, otherwise FALSE.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - operation successful.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *		initialized.
 */
BtStatus BTL_AG_IsInbandRingEnabled(BtlAgContext *context,
									BtlAgChannelId channelId,
									BOOL *enabled);

/*---------------------------------------------------------------------------
 * BTL_AG_IsCallIdNotifyEnabled()
 *
 *
 *	Returns an answer saying whether call id notify is enabled
 *	or not.
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	context [in] - AG context.
 *
 *	ans [out] - TRUE if call id notify is enabled, otherwise FALSE.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - operation successful.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *		initialized.
 */
BtStatus BTL_AG_IsCallIdNotifyEnabled(BtlAgContext *context,
							 BtlAgChannelId channelId,
							 BOOL *enabled);

/*---------------------------------------------------------------------------
 * BTL_AG_IsVoiceRecEnabled()
 *
 * Brief:
 *	Queries if voice recognition is enabled.
 *
 * Description:
 *	Returns an answer saying whether voice recognition is enabled
 *	or not.
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	context [in] - AG context.
 *
 *	ans [out] - TRUE if voice recognition is enabled, otherwise FALSE.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - operation successful.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *		initialized.
 */
BtStatus BTL_AG_IsVoiceRecEnabled(BtlAgContext *context,
								  BtlAgChannelId channelId,
								  BOOL *enabled);

/*---------------------------------------------------------------------------
 * BTL_AG_IsCallWaitingEnabled()
 *
 * Brief:
 *	Queries if call waiting is enabled.
 *
 * Description:
 *	Returns an answer saying whether call waiting is enabled
 *	or not.
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	context [in] - AG context.
 *
 *	ans [out] - TRUE if call waiting is enabled, otherwise FALSE.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - operation successful.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *		initialized.
 */
BtStatus BTL_AG_IsCallWaitingEnabled(BtlAgContext *context,
									 BtlAgChannelId channelId,
									 BOOL *enabled);


#if HFG_SNIFF_TIMER >= 0

/*---------------------------------------------------------------------------
 * BTL_AG_SetSniffTimer()
 *
 * Brief:
 *	Sets the sniff timer.
 *
 * Description:
 *	DEPRICATED function.
 * 	It is strongly recomended to use BTL_AG_SetSniffParams instead
 *
 *	Sets the Sniff timer, the sniff timer is the time passed
 *	until sniff mode is entered (when sniff mode is enabled).
 *	This value is applied to all AG channels.
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	context [in] - The AG context.
 *
 *	time [in] - The new value of sniff timer in milliseconds.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - The new sniff timer value has been set.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly
 *		initialized.	
 */
BtStatus BTL_AG_SetSniffTimer(BtlAgContext *context,
							  TimeT time);

/*---------------------------------------------------------------------------
 * BTL_AG_GetSniffTimer
 *
 * Brief:
 *	Gets the sniff timer.
 *
 * Description:
 *	DUPRICATED function.
 *	It is strongly recomended to use BTL_AG_GetSniffParams instead
 *	Get the current value of the sniff timer.
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	context [in] - The AG context;
 *
 *	time [out] - The sniff timer value in milliseconds.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - Operation successful, 'time' is valid.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly
 *		initialized.	
 */
BtStatus BTL_AG_GetSniffTimer(BtlAgContext *context,
							  TimeT *time);

/*---------------------------------------------------------------------------
 * BTL_AG_SetSniffParams()
 *
 * Brief:
 *	Sets sniff parameters.
 *
 * Description:
 *	Sets sniff parameters, which includes:
 *	min interval, max interval, attempt & timeout.
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	context [in] - The AG context.
 *
 *	sniffParams [in] - The Sniff Parameters.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - The new sniff timer value has been set.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly
 *		initialized.	
 */
BtStatus BTL_AG_SetSniffParams(BtlAgContext * context, HfgSniffParams * sniffParams);

/*---------------------------------------------------------------------------
 * BTL_AG_GetSniffParams()
 *
 * Brief:
 *	Gets the sniff parameters.
 *
 * Description:
 *	Gets the current value of the sniff parameters (not including timer).
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	context [in] - The AG context;
 *
 *	sniffParams [out] - The Sniff Parameters.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - Operation successful, 'time' is valid.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly
 *		initialized.	
 */
BtStatus BTL_AG_GetSniffParams(BtlAgContext *context, HfgSniffParams * sniffParams);

/*---------------------------------------------------------------------------
 * BTL_AG_GetSniffMode()
 *
 * Brief:
 *	Gets the sniff mode.
 *
 * Description:
 *	Gets the current sniff mode, if the sniff mode is TRUE then sniff is enabled.
 *	otherwise sniff is disabled.
 *	Requires HFG_SNIFF_TIMER >= 0.
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	context [in] - AG context.
 *
 *	channelId [in] - the channel Id
 *
 *	res [out] - if TRUE sniff mode is enabled otherwise 
 *		sniff mode is disabled.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - operation successful.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly
 *		initialized.
 */
BtStatus BTL_AG_GetSniffMode(BtlAgContext *context,
							 BtlAgChannelId channelId,
							 BOOL *enabled);

/*---------------------------------------------------------------------------
 * BTL_AG_SetSniffMode()
 *
 * Brief:
 *	Sets the sniff mode.
 *
 * Description:
 *	Sets the sniff mode, if sniff mode is set to TRUE.
 *	the sniff is activated, otherwise sniff is disabled.
 *	Requires HFG_SNIFF_TIMER >= 0.
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	context [in] - AG context.
 *
 *	channelId [in] - The channel ID.
 *
 *	enable [in] - If TRUE sniff mode is enabled otherwise disabled.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - operation successful.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly
 *		initialized.
 */
BtStatus BTL_AG_SetSniffMode(BtlAgContext *context,
							 BtlAgChannelId channelId,
							 BOOL enabled);
#endif /* HFG_SNIFF_TIMER >= 0 */

/*---------------------------------------------------------------------------
 * BTL_AG_GetSniffExitPolicy()
 *
 * Brief:
 *	Gets the sniff exit policy.
 *
 * Description:
 *	Gets the policy for exiting sniff mode on the specified channel.
 *	See the description of "HfgSniffExitPolicy".
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	context [in] - AG context.
 *
 *	channelId [in] - The channel Id
 * 
 *	policy [out] - Bitmask that describes the policy for exiting sniff mode.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - The policy was set successfully.
 *
 *	BT_STATUS_NOT_FOUND - Could not get the sniff policy, because  
 *		handler is not registered.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *		initialized.
 */
BtStatus BTL_AG_GetSniffExitPolicy(BtlAgContext *context,
								   BtlAgChannelId channelId,
								   HfgSniffExitPolicy *policy);

/*---------------------------------------------------------------------------
 * BTL_AG_SetSniffExitPolicy()
 *
 * Brief:
 *	Sets the sniff exit policy.
 *
 * Description:
 *	Sets the policy for exiting sniff mode on the specified channel.
 *	The policy can be set to HFG_SNIFF_EXIT_ON_SEND or 
 *	HFG_SNIFF_EXIT_ON_AUDIO_LINK. These values can also be OR'd together
 *	to enable both (See HfgSniffExitPolicy).
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	context [in] - AG context.
 *
 *	channelId [in] - The channel Id.
 * 
 *	policy [in] - Bitmask that defines the policy for exiting sniff mode.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - The policy was set successfully.
 *
 *	BT_STATUS_NOT_FOUND - Could not set the sniff policy, because  
 *		handler is not registered.
 *
 *	BT_STATUS_INVALID_PARM - A parameter is invalid or not properly
 *		initialized.
 */
BtStatus BTL_AG_SetSniffExitPolicy(BtlAgContext *context,
								   BtlAgChannelId channelId,
								   HfgSniffExitPolicy policy);


/*---------------------------------------------------------------------------
 * BTL_AG_SetFmOverBtMode()
 *
 * Brief:
 *	send vendor specific that enables\disables FM over BT.
 *
 * Description:
 *	send vendor specific that enables\disables FM over BT.	
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	context [in] - AG context.
 *
 *	channelId [in] - The channel Id.
 *
 *	enableFMoBT [in] - Disable/Enable FM over BT .
  *
 * Returns:
 *	BT_STATUS_PENDING - The operation has started.
 *
 *	BT_STATUS_FAILED - the operation failed.
 *
 */
BtStatus BTL_AG_SetFmOverBtMode(BtlAgContext *context,
								BtlAgChannelId channelId,
								BOOL enableFMoBT);



/*---------------------------------------------------------------------------
 * BTL_AG_SetCallHolfFlags()
 *
 * Brief:
 *	Set call hold flags to profile.
 *
 * Description:
 *	Set call hold flags to profile.
 *	The call hold flags will be sent to the HF Unit when requested (AT+CHLD?).
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	context [in] - AG context.
 *
 *	callHoldFlags [in] - The call hold flags to be set in the profile.
 *
 * Returns:
 *	BT_STATUS_PENDING - The operation has started.
 *
 *	BT_STATUS_FAILED - the operation failed.
 */
BtStatus BTL_AG_SetCallHolfFlags(BtlAgContext *context, 
								 AtHoldFlag callHoldFlags);




#endif /* __BTL_AG_H */


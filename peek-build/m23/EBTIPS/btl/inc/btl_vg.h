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
*   FILE NAME:		btl_vg.h
*
*	BRIEF:			This file defines the API of the BTL Voice Gateway.
*
*   DESCRIPTION:	The BTL_VG is an implementation of the AG role of the HFP and HSP.
*					The current role of the BTL_VG (i.e. Headset or Hands-free AG) is handled
*					Internally by the module.
*
*					Audio is established and routed to the remote device (HS or HF)
*					according to the current call state (incoming/outgoing and etc.)
*					
*					Support for audio routing and AT Commands handling is given
*					using the BTHAL MC & VC Modules (Modem Control and Voice Control respectively).
*					All call/modem related AT Commands are passed to the modem using the 
*					BTHAL MC Module and Voice/audio related AT Commands
*					(e.g. AT+BVRA for voice recognition activation/deactivation)
*					are passed to the BTHAL MC Module
*
*   AUTHOR:			Itay Klein
*
\*******************************************************************************/

#ifndef __BTL_VG_H
#define __BTL_VG_H
/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/

#include "bttypes.h"
#include "atp.h"
#include "at_hf.h"
#include "btl_common.h"
#include "btl_config.h"
#include "sec.h"

#if BTL_VG_BT_EXTENDED_EVENTS == XA_ENABLED

#include "hfg.h"

#endif 

/********************************************************************************
 *
 * Constants.
 *
 *******************************************************************************/
/*-------------------------------------------------------------------------------
 * BTL_VG_CHANNEL_ID_NONE constant.
 *
 *	A BtlVgChannelId value used to indicate that a BtlVgChannelId variable
 *	does not reference a VG Channel.
 */
#define BTL_VG_CHANNEL_ID_NONE 0xffffffff

/*-------------------------------------------------------------------------------
 * BTL_VG_MAX_VOLUME constant.
 *
 *	The maximum volume value a speaker or a microphone can have.
 */
#define BTL_VG_MAX_VOLUME 15

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

 
/*-------------------------------------------------------------------------------
 * BtlVgEventType type
 *
 *	Indicating the type of an event sent by the VG.
 */
typedef U32 BtlVgEventType;

/* 
 * A remote audio device has initiated a connection to the VG.
 * call BTL_VG_AcceptConnectReq().
 * The field "BtlVgEvent.bdAddr" contains the address of the remote device.
 */
#define BTL_VG_EVENT_SLC_REQUEST				0

/* 
 * A service level connection has been established.
 * A successful operation of BTL_VG_Connect() or a successful incoming connection
 * from a remote audio unit results in this event
 * This event can also occur when an attempt to create a service level 
 * connection (BTL_VG_Connect) fails.
 *
 * In addition, if appropriate, the "BtlVgEvent.errCode"
 * fields contains the reason for disconnect
 */
#define BTL_VG_EVENT_SLC_CONNECTED				1

/* 
 * The service level connection has been released.
 * This can happen as the result of a call to BTL_VG_Disconnect() or
 * if the remote audio unit releases the service connection.
 */
#define BTL_VG_EVENT_SLC_DISCONNECTED			2

/* 
 * An audio connection has been established.
 * This event occurs whenever the audio connection comes up,
 * whether it is initiated by the VG or the remote audio unit.
 */
#define BTL_VG_EVENT_AUDIO_CONNECTED			3

/* 
 * An audio connection has been released.
 * This event occurs whenever the audio connection is disconnected
 * Whether it is initiated by the VG or the remote audio unit.
 */
#define BTL_VG_EVENT_AUDIO_DISCONNECTED			4

/* 
 * The remote unit has requested a phone number to attach to a voice tag (AT+BINP=1).
 * The user must call BTL_VG_AcceptAttachNumber() to accept the request and send a number
 * or call BTL_VG_RejectAttachNumber() to reject the request.
 */
#define BTL_VG_EVENT_PHONE_NUMBER_REQUESTED		5

/* 
 * The remote audio units report its current microphone volume.
 * 
 * When this event is received, the "BtlVgEvent.p.volume" field
 * contains the reported microphone volume.
 */
#define BTL_VG_EVENT_MICROPHONE_VOLUME			6

/* 
 * The remote audio unit report its current speaker volume.
 *
 * When this event is received, the "BtlVgEvent.p.volume" field
 * contains the reported speaker volume.
 */
#define BTL_VG_EVENT_SPEAKER_VOLUME				7

/* 
 * The remote audio unit has enabled or disabled voice recognition
 * in the VG.
 *
 * When this event is received, the "BtlVgEvent.p.enabled" field
 * indicates the current status of voice recognition.
 * if it equals TRUE then voice recognition is enabled, otherwise
 * it is disabled.
 */
#define BTL_VG_EVENT_VOICE_RECOGNITION_STATUS	8

/* 
 * Inband ring tone has been enabled/disabled within the VG.
 *
 * When this event is received, the "BtlVgEvent.p.enabled" field
 * indicates whether in-band ring tone has been enabled or disabled
 * if the field equals TRUE then in-band ring tone has been enabled
 * otherwise, disabled.
 */
#define BTL_VG_EVENT_INBAND_RING_STATUS			9

/* 
 * The VG is disabled.
 * This event is received only when the BTL_VG_Disable operation is asynchronous,
 * i.e. the VG is connected to remote audio units.
 */
#define BTL_VG_EVENT_VG_CONTEXT_DISABLED		10

/* 
 * The VG is enabled.
 * This event is received when the enabling process (BTL_VG_Enable)
 * has finished.
 */
#define BTL_VG_EVENT_VG_CONTEXT_ENABLED			11

/* 
 * Unrecognized AT Command received from the audio unit.
 * When this event is received "BtlVgEvent.p.atCommand" contains
 * The AT Command.
 */
#define BTL_VG_EVENT_UNRECOGNIZED_AT_COMMAND	12

/* 
 * Remote device has initiated a connection to the VG while another device
 * is connected.
 * call BTL_VG_AcceptConnectReq() or BTL_VG_PerformHandover().
 * The field "BtlVgEvent.bdAddr" contains the address of the remote device.
 */
 #define BTL_VG_EVENT_ADDITIONAL_SERVICE_CONNECT_REQ	13

/* 
 * The handover process is successfully completed.
 * The new VG connection is established and the previous connection is released.
 */
#define BTL_VG_EVENT_HANDOVER_COMPLETED			14

/* 
 * The handover process failed. It may happen, whether an application or a peer
 * device initiated the handover but it was not successful, or when an application,
 * being already connected to one peer device, intentionally rejected request for
 * connection from additional device.
 */
#define BTL_VG_EVENT_HANDOVER_FAILED                            15

#if (0)

/* The FM over Bluetooth audio path is enabled.
 */
#define BTL_VG_EVENT_FM_OVER_BT_ENABLED                     16

/* The FM over Bluetooth audio path is disabled.
 */
#define BTL_VG_EVENT_FM_OVER_BT_DISABLED                    17

#endif /* BTL_CONFIG_FM_STACK == BTL_CONFIG_ENABLED */

#if BTL_VG_BT_EXTENDED_EVENTS == XA_ENABLED

#define BTL_VG_BT_EXTENDED_EVENTS_BASE					1000

/* 
 * This event is received when the audio units reports its supported features.
 * When this is event is received. the "BtlVgEvent.p.features" field is valid.
 */
#define BTL_VG_EVENT_UNIT_SUPPORTED_FEATURES			(1 + BTL_VG_BT_EXTENDED_EVENTS_BASE)

/* 
 * This event is received when the remote unit requests the current call 
 * to be terminated.
 */
#define BTL_VG_EVENT_CALL_TERMINATED					(2 + BTL_VG_BT_EXTENDED_EVENTS_BASE)

/* 
 * This event is received when the remote audio unit requests a number to be dialed
 * in the VG.
 * When this event is received, the "BtlVgEvent.p.number" field is valid.
 */
#define BTL_VG_EVENT_NUMBER_DIALED						(3 + BTL_VG_BT_EXTENDED_EVENTS_BASE)

/* 
 * This event is received when the remote audio unit has changed the hold
 * status of one or more calls.
 *
 * When this event is received, the "BtlVgEvent.p.action" field, describes
 * the action requested by the remote unit.
 */
#define BTL_VG_EVENT_CALL_HOLD_MULTIPARTY_HANDLING		(4 + BTL_VG_BT_EXTENDED_EVENTS_BASE)

/* 
 * This event is received when the remote audio unit has requested
 * call waiting notification to be enabled or disabled. 
 *
 * When this event is received the "BtlVgEvent.p.enabled" field, describes
 * the request, if it equals TRUE then the request if to enable otherwise
 * is to disable.
 */
#define BTL_VG_EVENT_CALL_WAITING_NOTIFICATION_ENABLED	(5 + BTL_VG_BT_EXTENDED_EVENTS_BASE)

/* 
 * This event is received when the remote unit has requested the VG to 
 * turn off any active noise reduction or echo cancelling.
 */
#define BTL_VG_EVENT_NREC_TURNED_OFF					(6 + BTL_VG_BT_EXTENDED_EVENTS_BASE)

/* 
 * This event is received when the remote unit has requested the VG to
 * enable or disable calling line identification.
 *
 * When this event is received the "BtlVgEvent.p.enabled" field is valid.
 * If TRUE the request is to enable calling line identification.
 * Otherwise the request is to disable.
 */
#define BTL_VG_EVENT_CALLING_LINE_ID_ENABLED			(7 + BTL_VG_BT_EXTENDED_EVENTS_BASE)

/* 
 * This event is received when the remote audio unit has requested the VG
 * to transmit a specified DTMF code to the network.
 *
 * When this event is received, the "BtlVgEvent.p.dtmf" field
 * describes the DTMF code requested.
 */
#define BTL_VG_EVENT_TRANSMIT_DTMF						(8 + BTL_VG_BT_EXTENDED_EVENTS_BASE)

/* 
 * This event is received when the remote audio unit send AT+CKPD=200 to the VG
 * indicating that a button was pressed.
 */
#define BTL_VG_EVENT_BUTTON_PRESSED						(9 + BTL_VG_BT_EXTENDED_EVENTS_BASE)

/* 
 * This event is received when RING has been sent to the remote audio unit.
 */
#define BTL_VG_EVENT_RING_SENT							(10+ BTL_VG_BT_EXTENDED_EVENTS_BASE)

#endif /* BTL_VG_BT_EXTENDED_EVENTS == XA_ENABLED */

#if BTL_VG_MC_EXTENDED_EVENTS == XA_ENABLED

/* The following events originate from the BTHAL_MC module. */

#define BTL_VG_MC_EXTENDED_EVENTS_BASE					2000

/* 
 * This event is received when a call setup status indication
 * is received from the modem..
 */
#define BTL_VG_MC_EVENT_CALL_SETUP_STATUS				(1 + BTL_VG_MC_EXTENDED_EVENTS_BASE)

/* 
 * This event is received when a call status indication
 * is received from the modem.
 */
#define BTL_VG_MC_EVENT_CALL_STATUS						(2 + BTL_VG_MC_EXTENDED_EVENTS_BASE)

/* 
 * This event is received when a call hold indication is
 * received from the modem.
 */
#define BTL_VG_MC_EVENT_CALL_HOLD_STATUS				(3 + BTL_VG_MC_EXTENDED_EVENTS_BASE)

/* 
 * This event is received when a notification on call waiting
 * is received from the modem.
 */
#define BTL_VG_MC_EVENT_CALL_WAITING					(4 + BTL_VG_MC_EXTENDED_EVENTS_BASE)

/* 
 * This event is received when extended errors usage is enabled or disabled in the modem.
 *
 * When this event is received, the "BtlVgEvent.p.enabled" is valid.
 * if it equals TRUE then extended error are enabled,
 * else they are disabled.
 */
#define BTL_VG_MC_EVENT_EXTENDED_ERROR					(5 + BTL_VG_MC_EXTENDED_EVENTS_BASE)

/* 
 * Unsolicited result code RING has been received from the modem
 */
#define BTL_VG_MC_EVENT_RING_RECEIVED					(6 + BTL_VG_MC_EXTENDED_EVENTS_BASE)

/* 
 * Calling line identification has been received from the modem.
 */
#define BTL_VG_MC_EVENT_CALLING_LINE_ID_RECEIVED		(7 + BTL_VG_MC_EXTENDED_EVENTS_BASE)

#endif /* BTL_VG_MC_EXTENDED_EVENTS == XA_ENABLED */

#if BTL_VG_VC_EXTENDED_EVENTS == XA_ENABLED

/* The following events originate from the BTHAL_VC module */
#define BTL_VG_VC_EXTENDED_EVENTS_BASE					3000

/* 
 * The audio paths has been changes from VG to an audio unit,
 * from an audio unit to the VG or to both.
 *
 * When this event is received, the "BtlVgEvent.p.path" indicates
 * where the audio/voice is routed.
 */
#define BTL_VG_VC_EVENT_VOICE_PATH_CHANGED				(1 + BTL_VG_VC_EXTENDED_EVENTS_BASE)

/* 
 * This events indicates whether noise reduction and echo cancelling are
 * enabled or disabled in the VG.
 *
 * When this event is received, the "BtlVgEvent.p.enabled" field
 * Indicates whether noise reduction and echo cancelling are
 * enabled or not.
 */
#define BTL_VG_VC_EVENT_NREC_ENABLED					(2 + BTL_VG_VC_EXTENDED_EVENTS_BASE)

/* 
 * This events indicates whether voice recognition is enabled or
 * disabled in the VG.
 *
 * When this event is received, the "BtlVgEvent.p.enabled" field
 * Indicates whether voice recognition is enabled or not.
 */
#define BTL_VG_VC_EVENT_VR_ENABLED						(3 + BTL_VG_VC_EXTENDED_EVENTS_BASE)

#endif /* BTL_VG_VC_EXTENDED_EVENTS == XA_ENABLED */


/*-------------------------------------------------------------------------------
 * BtlVgChannelId type
 *
 *	An ID for identifying a VG Channel.
 *	A VG Channel is an entity used to describe an VG Connection between
 *	the VG and an audio unit.
 */
typedef U32 BtlVgChannelId;

/*-------------------------------------------------------------------------------
 * BtlVgHeadsetFunction type
 *
 *	Define the action taken when a headset connects to the VG.
 *	Possible actions depends on the audio source used for the connection.
 */
typedef U32 BtlVgHeadsetAction;

/* 
 * Perform redial last number,
 * valid only when audio source is voice calls.
 */
#define BTL_VG_HEADSET_ACTION_REDIAL	1

/* 
 * Activate voice recognition and dial the number supplied.
 * valid only when audio source is voice calls.
 */
#define BTL_VG_HEADSET_ACTION_VR		2

/* 
 * Used for describing an action for unavailable audio source
 * In the structure: BtlVgAudioSourcesAction.
 */
#define BTL_VG_HEADSET_ACTION_INVALID	0xffffffff


/*-------------------------------------------------------------------------------
 * BtlVgAudioSource type
 *
 *	A type describing an audio source of a VG Channel.
 */
typedef U32 BtlVgAudioSource;

/* 
 * Audio source is voice calls.
 */
#define BTL_VG_AUDIO_SOURCE_MODEM				1

/* 
 * Audio source is the FM radio.
 */
#define BTL_VG_AUDIO_SOURCE_FM_RADIO			2

/* 
 * Audio source is from electronic books
 */
#define BTL_VG_AUDIO_SOURCE_MULTIMEDIA_STACK	3

#define BTL_VG_AUDIO_SOURCE_LAST_SOURCE			BTL_VG_AUDIO_SOURCE_MULTIMEDIA_STACK

/*-------------------------------------------------------------------------------
 * BtlVgAudioPath type
 *
 *	This type indicates the audio path to set.
 *	The audio can be directed to the Handset or to the bluetooth remote unit
 */
typedef U32 BtlVgVoicePath;

/* 
 * Route the audio to the VG
 */
#define BTL_VG_ROUTE_VOICE_TO_VG	1

/* 
 * Route the audio to the bluetooth audio unit.
 */
#define BTL_VG_ROUTE_VOICE_TO_UNIT	2

/* Forward declaration: */
typedef struct _BtlVgEvent BtlVgEvent;

/*-------------------------------------------------------------------------------
 * BtlVgCallback type
 *
 *	A function of this type is called to indicate BTL VG events.
 */
typedef void (*BtlVgCallback)(const BtlVgEvent *event);

/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/

/* forward declaration */
typedef struct _BtlVgContext BtlVgContext;


/*-------------------------------------------------------------------------------
 * BtlVgAudioSourcesAction structure.
 *
 *	Describes actions taken for each audio source.
 *	Those actions are relevant when the VG is connected to a HS Unit,
 *	And the user pressed the headset button (AT+CKPD=200 recieved).
 *
 *	For each audio source ,the relevant entry in 'actions' shall hold the
 *	required action.
 *	If the audio source is unavailable for the VG set the action to
 *	BTL_VG_HEADSET_ACTION_INVALID
 *
 *	For example, if the desired action, when a Headset audio source is the modem,
 *	is redial.
 *	and the modem is the only available resource in the system.
 *
 *	then set:
 *
 *	BtlVgAudioSourcesAction.actions[BTL_VG_AUDIO_SOURCE_MODEM] = BTL_VG_HEADSET_ACTION_REDIAL;
 *
 *	BtlVgAudioSourcesAction.actions[BTL_VG_AUDIO_SOURCE_FM_RADIO] = BTL_VG_HEADSET_ACTION_INVALID;
 *
 *	BtlVgAudioSourcesAction.actions[BTL_VG_AUDIO_SOURCE_MULTIMEDIA_STACK] = BTL_VG_HEADSET_ACTION_INVALID;
 */
typedef struct _BtlVgAudioSourcesAction
{
	BtlVgHeadsetAction actions[BTL_VG_AUDIO_SOURCE_LAST_SOURCE+1];
} BtlVgAudioSourcesAction;

/*-------------------------------------------------------------------------------
 * BtlVgAtCommand structure
 *
 *	Describes an AT command in text format.
 */
typedef struct _BtlVgAtCommand
{
	const char *atCommand;	/* The AT Command string */

	U32 commandLength;		/* The AT Command string length */

} BtlVgAtCommand;

/*-------------------------------------------------------------------------------
 * BtlVgEvent structure
 *
 *	This structure describes an event sent from the VG to the user
 */
struct _BtlVgEvent
{
	BtlVgEventType type;		/* The type of the event */

	BtStatus status;			/* The status of the event */

	BtErrorCode errCode;		/* Error code if applicable */

	BtlVgContext *context;		/* The VG Context */

	BtlVgChannelId channelId;	/* The channelId of the relevant VG channel */
	 
	union
	{
		BD_ADDR *bdAddr;
		
		U32 volume;		
		 
		BOOL enabled;

		const BtlVgAtCommand *atCommand;

#if BTL_VG_BT_EXTENDED_EVENTS == XA_ENABLED

		HfgHandsFreeFeatures features;

		const char *number;

		HfgHoldAction action;

		U32 dtmf;
#endif

#if BTL_VG_MC_EXTENDED_EVENTS == XA_ENABLED
		/* TBD */
#endif

#if BTL_VG_VC_EXTENDED_EVENTS == XA_ENABLED

		BtlVgAudioPath path;
#endif
		/* For internal use only */
		const void *ptr;	
	}p;	/* See the list of VG Events for a description on the various union fields. */
};

/********************************************************************************
 *
 * Function Reference
 *
 *******************************************************************************/
/*---------------------------------------------------------------------------
 * BTL_VG_Init()
 *
 * Brief:
 *	Initializes the VG Module.
 *
 * Description:
 *	This function initialize the BTL VG module and must be called
 *	before any other function in the API.
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	void.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - Initialization succeeded.
 *
 *	BT_STATUS_FAILED - The VG Module is already initialized.
 *
 *	BT_STATUS_INTERNAL_ERROR - Internal error, bad memory initialization.
 */
BtStatus BTL_VG_Init(void);

/*---------------------------------------------------------------------------
 * BTL_VG_Deinit()
 *
 * Brief:
 *	Deinitializes the VG Module.
 *
 * Description:
 *	This function deinitialize the BTL VG module.
 *	After this function is called no API function except BTL_VG_Init
 *	can be called.
 *	If BTL_VG_Create() has been called, call BTL_VG_Destroy() for the operation
 *	to succeed.
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
 *	BT_STATUS_FAILED - The module is not initialized
 *		or a VG Context is still active.
 *
 *	BT_STATUS_INTERNAL_ERROR - Failed to release memory used
 *		by the module.
 */
BtStatus BTL_VG_Deinit(void);

/*---------------------------------------------------------------------------
 * BTL_VG_Create()
 *
 * Brief:
 *	Creates a VG Context.
 *
 * Description:
 *	This function creates a VG Context instance.
 *	A VG Context is needed for all VG Operations (except init and deinit).
 *	BTL_VG_Create() should be called after BTL_VG_Init().
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	handle [in] - Application handle in order to link between different modules,
 *		If there's no need for an application handle set handle to 0.
 *
 *	callback [in] - A user supplied callback, used to notify the user 
 *		of VG events.
 *
 *	secLevel [in] - The contexts security level, if set to 0 the default
 *		security level is used.
 *
 *	context [out] - A reference to the created VG Context.
 *		
 * Returns:
 *	BT_STATUS_SUCCESS - The VG Context has been successfully created.
 *
 *	BT_STATUS_FAILED - The VG Module is not initialized
 *
 *	BT_STATUS_INVALID_PARM - Null callback parameter, the context is not destroyed
 *		or an invalid security level.
 *
 *	BT_STATUS_INTERNAL_ERROR - An internal error has occurred.
 */
BtStatus BTL_VG_Create(BtlAppHandle *handle,
					   BtlVgCallback callback,
					   const BtSecurityLevel *secLevel,
					   BtlVgContext **context);
/*---------------------------------------------------------------------------
 * BTL_VG_Destroy()
 *
 * Brief:
 *	Destroys a VG Context.
 *
 * Description:
 *	This function is called to destroy the VG.
 *	If the VG is enabled, BTL_VG_ENABLED must be called previously.
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	context [out] - The VG Context to destroy,
 *		By the end of the operation the context argument is set to 0.
 *	
 * Returns:
 *	BT_STATUS_SUCCESS - The VG Context is destroyed.
 *
 *	BT_STATUS_INVALID_PARM - The context parameter is in illegal state.
 *		the context was not created or it is still enabled.
 *
 *	BT_STATUS_INTERANL_ERROR - An internal error has occurred.
 */
BtStatus BTL_VG_Destroy(BtlVgContext **context);

/*---------------------------------------------------------------------------
 * BTL_VG_Enable()
 *
 * Brief:
 *	Enables the VG Context.
 *
 * Description:
 *	Enables the VG Context.
 *	after this function has been called connection to and from the VG can be established.
 *	BTL_VG_Create() must be called before BTL_VG_Enable() for the operation
 *	To succeed.
 *	The event BTL_VG_EVENT_CONTEXT_ENABLED is sent when the operation finishes.
 *
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context [in] - The VG Context.
 *
 *	serviceName [in] - The service name of the VG.
 *		this name is published in the SDP database.
 *		If 0, the default name is used.
 *
 *	features [in] - A bit mask describing the features supported by the VG.
 *		If 0, the default features are used.
 *
 *	action [in] - Defines the actions taken for the various audio sources.
 *		see "BtlVgAudioSourcesAction" description for further details.
 *
 * Generated Events:
 *	BTL_VG_EVENT_CONTEXT_ENABLED.
 *
 * Returns:
 *	BT_STATUS_PENDING - The operation started successfully.
 *
 *	BT_STATUS_INVALID_PARM - The context is in illegal state,
 *
 *	BT_STATUS_FAILED - Failed to enable the VG Context.
 */
BtStatus BTL_VG_Enable(BtlVgContext *context,
					   const char *serviceName,
					   const AtAgFeatures *features,
					   const BtlVgAudioSourcesAction *action);

/*---------------------------------------------------------------------------
 * BTL_VG_Disable()
 *
 * Brief:
 *	Disables the VG Context.
 *
 * Description:
 *	Disables The VG, if there are connections to remote devices
 *	They are disconnected first.
 *	If connections exists BTL_VG_EVENT_CONTEXT_DISABLED is sent
 *	When the operation is done.
 *
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context [in] - The VG Context.
 *
 * Generated Events:
 *	BTL_VG_EVENT_CONTEXT_DISABLED.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - The VG Context is disabled.
 *
 *	BT_STATUS_PENDING - The VG Context is being disabled,
 *		BTL_VG_EVENT_CONTEXT_DISABLED will be sent when the operation is finished.
 *
 *	BT_STATUS_INVALID_PARM - The  context is in illegal state,
 */
BtStatus BTL_VG_Disable(BtlVgContext *context);

/*---------------------------------------------------------------------------
 * BTL_VG_SetAudioSourcesAction()
 *
 * Brief:
 *	Set the action for "Button pressed" for HS Connections.
 *
 * Description:
 *	Set the action taken for each audio source.
 *	look at "BtlVgAudioSourcesAction" description for further details.
 *	The new actions will be applied for future connections.
 *	current connections are unaffected.
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	context [in] - The VG Context.
 *
 *	action [in] - Describing actions for audio sources.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - Operation finished successfully.
 *
 *	BT_STATUS_FAILED - Operation failed.
 *
 *	BT_STATUS_INVALID_PARMS - Invalid parameter.
 */
BtStatus BTL_VG_SetAudioSourcesAction(BtlVgContext *context,
									  const BtlVgAudioSourcesAction *action);

/*---------------------------------------------------------------------------
 * BTL_VG_GetAudioSourcesAction()
 *
 * Brief:
 *	Gets the current audio sources actions in the VG.
 *
 * Description:
 *	Gets the current audio sources actions in the VG.
 *	See BTL_VG_SetAudioSourcesAction() and BtlVgAudioSourcesAction
 *	for further details.
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	context[in] - The VG Context.
 *
 *	action [out] - The audio sources actions.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - Operation finished successfully,
 *		'action' is valid.
 *
 *	BT_STATUS_FAILED - Operation failed.
 *
 *	BT_STATUS_INVALID_PARMS - Invalid parameter.
 */
BtStatus BTL_VG_GetAudioSourcesAction(const BtlVgContext *context,
									  BtlVgAudioSourcesAction *action);

#if BT_SECURITY == XA_ENABLED
/*---------------------------------------------------------------------------
 * BTL_VG_SetSecurityLevel()
 *
 * Brief:
 *	Sets the Security level of the VG Context.
 *
 * Description:
 *	Set the security level of the VG context.
 *	The new settings shall affects new connections and not existing ones.
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	context [in] - The VG context.
 *
 *	secLevel [in] - The desired security level,
 *		If 0, the default security level is used.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - The new security level has been successfully set.
 *
 *	BT_STATUS_INVALID_PARM - Invalid security settings.
 */
BtStatus BTL_VG_SetSecurityLevel(BtlVgContext *context,
								 const BtSecurityLevel *secLevel);

/*---------------------------------------------------------------------------
 * BTL_VG_GetSecurityLevel()
 *
 * Brief:
 *	Gets the security level of the VG Context.
 *
 * Description:
 *	Gets the security level of the VG Context.
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	context [in] - The VG context.
 *
 *	secLevel [out] - The current VG's security level.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - Operation finished successfully,
 *		secLevel is valid.
 *
 *	BT_STATUS_INVALID_PARM - secLevel argument is null.
 */
BtStatus BTL_VG_GetSecurityLevel(const BtlVgContext *context,
								 BtSecurityLevel *secLevel);
#endif /* BT_SECURITY == XA_ENABLED */

/*---------------------------------------------------------------------------
 * BTL_VG_Connect()
 *
 * Brief:
 *	Establishes a connection to a remote device.
 *
 * Description:
 *	Requests a connection to be established to a remote device.
 *	The event: BTL_VG_EVENT_SLC_CONNECTED 
 *	notifies the user about the results of the request.
 *
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context [in] - The VG context. 
 *
 *	bdAddr [in] - The Bluetooth Device Address of the device
 *		to connect to.
 *
 *	source [in] - The source of audio for the created connection.
 *		valid only if the remote device is a HS Unit.
 *		see "BtlVgAudioSource" and "BtlVgAudioSourcesAction"
 *		fur further details.
 *
 * Generated Events:
 *	BTL_VG_EVENT_SLC_CONNECTED.
 *	BTL_VG_EVENT_SLC_DISCONNECTED.
 *
 * Returns:
 *	BT_STATUS_PENDING - The operation has started successfully.
 *
 *	BT_STATUS_IN_USE - An SLC to a remote device already exists.
 *		use BTL_VG_PerformHandover or BTL_VG_Disconnect to establish a new connection.
 *
 *	BT_STATUS_FAILED - The operation has failed to start.
 *
 *	BT_STATUS_INVALID_PARM - Invalid parameter.
 *
 *	BT_STATUS_INTERNAL_ERROR - An internal error as occurred,
 *		failed to allocate memory.
 */
BtStatus BTL_VG_Connect(BtlVgContext *context,
						BD_ADDR *bdAddr,
						BtlVgAudioSource source);

/*---------------------------------------------------------------------------
 * BTL_VG_ConnectAndGetChannelId()
 *
 * Brief:
 *	Establishes a connection to a remote device.
 *
 * Description:
 *	Requests a connection to be established to a remote device.
 *	The event: BTL_VG_EVENT_SLC_CONNECTED 
 *	notifies the user about the results of the request.
 *	Unlike BTL_VG_Connect() this functions also returns a channelId for to new connection
 *	To allow calling BTL_VG_Disconnect before the is connection established and cancel
 *	the connect procedure.
 *	
 *
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context [in] - The VG context. 
 *
 *	bdAddr [in] - The Bluetooth Device Address of the device
 *		to connect to.
 *
 *	source [in] - The source of audio for the created connection.
 *		valid only if the remote device is a HS Unit.
 *		see "BtlVgAudioSource" and "BtlVgAudioSourcesAction"
 *		fur further details.
 *
 *	channelId [out] - The channel Id of the new connection.
 *
 * Generated Events:
 *	BTL_VG_EVENT_SLC_CONNECTED.
 *	BTL_VG_EVENT_SLC_DISCONNECTED. 
 *
 * Returns:
 *	BT_STATUS_PENDING - The operation has started successfully.
 *
 *	BT_STATUS_IN_USE - An SLC to a remote device already exists.
 *		use BTL_VG_PerformHandover or BTL_VG_Disconnect to establish a new connection.
 *
 *	BT_STATUS_FAILED - The operation has failed to start.
 *
 *	BT_STATUS_INVALID_PARM - Invalid parameter.
 *
 *	BT_STATUS_INTERNAL_ERROR - An internal error as occurred,
 *		failed to allocate memory.
 */
BtStatus BTL_VG_ConnectAndGetChannelId(BtlVgContext *context,
									BD_ADDR *bdAddr,
									BtlVgAudioSource source,
									BtlVgChannelId *channelId);

/*---------------------------------------------------------------------------
 * BTL_VG_Disconnect()
 *
 * Brief:
 *	Disconnects a connection to a remote device.
 *
 * Description:
 *	Disconnects a connection to a remote device. This will close the RFCOMM
 *  channel and will also close the SCO and ACL links, if no other services
 *  are active, and no other contexts are in use.
 *	When the operation is complete the application will be notified by the
 *  BTL_VG_EVENT_AUDIO_DISCONNECTED (if audio connection was active) and by
 *  the BTL_VG_EVENT_SERVICE_DISCONNECTED event.

 *
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context [in] - The VG context.
 *
 *	channelId [in] - The Id of the VG Channel with the active connection.
 *
 * Generated Events:
 *	BTL_VG_EVENT_AUDIO_DISCONNECTED (if audio connection was active),
 *	BTL_VG_EVENT_SLC_DISCONNECTED.
 *
 * Returns:
 *	BT_STATUS_PENDING - The operation has started successfully.
 *		The event BTL_VG_EVENT_SLC_DISCONNECTED, will be sent to notify
 *		on the results of the operation.
 *
 *	BT_STATUS_FAILED - Operation failed.
 *
 *	BT_STATUS_INVALID_PARM - Invalid channelId.
 */
BtStatus BTL_VG_Disconnect(BtlVgContext *context,
						   BtlVgChannelId channelId);

/*---------------------------------------------------------------------------
 * BTL_VG_GetConnectedDevice()
 *
 * Brief:
 *	Gets the BD Address of a connected device.
 *
 * Description:
 *	Get the Bluetooth device address of a device connected to the VG.
 *	The connection is identified by the channelId parameter.
 *	Only VG connections can be used with this API.
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	context [in] - The VG context.
 *
 *	channelId [in] - The VG Channel representing the connection.
 *
 *	bdAddr [out] - The BD Address of the connected device.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - Operation completed successfully.
 *
 *	BT_STATUS_FAILED - The operation failed.
 *
 *	BT_STATUS_INVALID_PARM - Invalid parameter.
 */
BtStatus BTL_VG_GetConnectedDevice(const BtlVgContext *context,
								   BtlVgChannelId channelId,
								   BD_ADDR *bdAddr);

/*---------------------------------------------------------------------------
 * BTL_VG_PerformHandover()
 *
 * Brief:
 *	Handover an existing VG Connection to a different remote device.
 *
 * Description:
 *	Call this function to handover a VG connection from one device to another.
 *	A VG connection and a connection request from the user or a remote device must already
 *	exist.
 *	If audio is established, an audio connection shall be opened to the handover target device.
 *	"BTL_VG_EVENT_HANDOVER_COMPLETED" or "BTL_VG_EVENT_HANDOVER_FAILED" notify
 *	the user when the operation completes.
 *
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context [in] - The VG Context.
 *
 *	source [in] – The channelId of the current connection.
 *
 *	target [in] - The BD address of the handover target.
 *
 *	accept[in] – If TRUE the handover is performed, otherwise the handover is rejected.
 *
 * Generated Events:
 *	BTL_VG_EVENT_HANDOVER_COMPLETED,
 *	BTL_VG_EVENT_HANDOVER_FAILED.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - Operation completed successfully.
 *
 *	BT_STATUS_PENDING - Operation started successfully.
 *		The event BTL_VG_EVENT_HANDOVER_COMPLETED is sent
 *		when the operation is finished.
 *
 *	BT_STATUS_FAILED - Operation failed.
 *
 *	BT_STATUS_INVALID_PARM - Invalid parameter.
 */
BtStatus BTL_VG_PerformHandover(BtlVgContext *context,
							BtlVgChannelId source,
							BD_ADDR *target,
							BOOL accept);

/*---------------------------------------------------------------------------
 * BTL_VG_AcceptAttachNumber()
 *
 * Brief:
 *	Send a phone number attached to a voice tag.
 *
 * Description:
 *	Send a phone number attached to a voice tag.
 *	Called by the user to accept a request to attach a phone number
 *	to a voice tag (see: BTL_VG_EVENT_PHONE_NUMBER_REQUESTED).
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	context [in] - The VG context.
 *
 *	channelId [in] - The channelId of the VG Channel that represents the connection.
 *
 *	number [in] - The phone number to attach to the voice tag.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - Operation completed successfully.
 *
 *	BT_STATUS_FAILED - Operation failed.
 *
 *	BT_STATUS_NOT_SUPPORTED - The remote device doesn't support "Voice Tag" attachment.
 *
 *	BT_STATUS_INVALID_PARM - Invalid parameter.
 */
BtStatus BTL_VG_AcceptAttachNumber(const BtlVgContext *context,
								   BtlVgChannelId channelId,
								   const char *number);
/*---------------------------------------------------------------------------
 * BTL_VG_RejectAttachNumber()
 *
 * Brief:
 *	Reject a request to attach a number to a voice tag
 *
 * Description:
 *	Reject a request to attach a number to a voice tag
 *	Called by the user to reject a request to attach a phone number to 
 *	a voice tag (see: BTL_VG_EVENT_PHONE_NUMBER_REQUESTED)
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	context [in] - The VG context.
 *
 *	channelId [in] - The channelId of the VG Channel that represents the connection.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - Operation completed successfully.
 *
 *	BT_STATUS_FAILED - Operation failed.
 *
 *	BT_STATUS_INVALID_PARM - Invalid parameter
 */
BtStatus BTL_VG_RejectAttachNumber(const BtlVgContext *context,
								   BtlVgChannelId channelId);

/*---------------------------------------------------------------------------
 * BTL_VG_EnableInbandRingTone()
 *
 * Brief:
 *	Enables/Disables inband ring tone.
 *
 * Description:
 *	Enables/Disables inband ring tone.
 *	If inband ring tone is enabled, upon incoming call an audio connection
 *	to the remote device shall be opened and the phone's ring tone is passed
 *	to the remote device.
 *	If the remote device is a HF Unit, +BSIR: 0 or +BSIR: 1 is sent.
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	context [in] - The VG context.
 *
 *	channelId [in] - The channelId of the VG Channel that represents the connection.
 *
 *	enable [in] - If TRUE then the request is to enable in-band ring tone,
 *		otherwise, the request is to disable it.
 *
 * Returns:
 *	BT_STATUS_PENDING - Operation started successfully.
 *
 *	BT_STATUS_FAILED - Operation failed.
 *
 *	BT_STATUS_INVALID_PARM - Invalid parameter.
 */
BtStatus BTL_VG_EnableInbandRingTone(BtlVgContext *context,
									 BtlVgChannelId channelId,
									 BOOL enable);
/*---------------------------------------------------------------------------
 * BTL_VG_IsInbandRingToneEnabled()
 *
 * Brief:
 *	Queries the VG if inband ring tone is enabled.
 *
 * Description:
 *	Queries the VG if inband ring tone is enabled.
 *	See BTL_VG_EnableInbandRingTone() for further details.
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	context [in] - The VG context.
 *
 *	channelId [in] - The channelId of the VG Channel that represents the connection.
 *
 *	enable [out] - If TRUE, inband ring tone is enabled, otherwise,
 *		it is disabled.
 *
 * Returns:
 *	BT_STATUS_SUCCESSS - Operation succeeded.
 *
 *	BT_STATUS_INVALID_PARM - Invalid parameter.
 */
BtStatus BTL_VG_IsInbandRingToneEnabled(const BtlVgContext *context,
										BtlVgChannelId channelId,
										BOOL *enable);
										
/*---------------------------------------------------------------------------
 * BTL_VG_EnableVoiceRecognition()
 *
 * Brief:
 *	Enables or disables voice recognition.
 *
 * Description:
 *	Request to enables/disables voice recognition within the VG.
 *	If the remote device is a HF Unit, AT+BVRA: 1 or AT+BVRA: 0 is sent
 *	respectively.
 *
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context [in] - The VG Context.
 *
 *	channelId [in] - The channelId of the VG Channel that represents the connection.
 *
 *	enable [in] - If TRUE then the request is to enable voice recognition,
 *		otherwise, the request is to disable it.
 *
 * Generated Events:
 *	BTL_VG_EVENT_VOICE_RECOGNITION_STATUS.
 *
 * Returns:
 *	BT_STATUS_PENDING - Operation started successfully. Event
 *	BTL_VG_EVENT_VOICE_RECOGNITION_STATUS will be sent, when the operation is
 *	finished.
 *
 *	BT_STATUS_FAILED - Operation failed.
 *
 *	BT_STATUS_INVALID_PARM - Invalid parameter.
 */
BtStatus BTL_VG_EnableVoiceRecognition(BtlVgContext *context,
									   BtlVgChannelId channelId,
									   BOOL enable);
/*---------------------------------------------------------------------------
 * BTL_VG_IsVoiceRecognitionEnabled()
 *
 * Brief:
 *	Queries the VG if voice recognition is activated.
 *
 * Description:
 *	Queries the VG if voice recognition is activated.
 *	See BTL_VG_EnableVoiceRecognition() for further details.
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	context [in] - The VG Context.
 *
 *	channelId [in] - The channelId of the VG Channel that represents the connection.
 *
 *	enable [out] - If TRUE, voice recognition is enabled, otherwise,
 *		it is disabled.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - Operation finished successfully,
 *		"enable" is valid.
 *
 *	BT_STATUS_INVALID_PARM - Invalid parameter
 */
BtStatus BTL_VG_IsVoiceRecognitionEnabled(const BtlVgContext *context,
										  BtlVgChannelId channelId,
										  BOOL *enable);

/*---------------------------------------------------------------------------
 * BTL_VG_SendMicrophoneVolume()
 *
 * Brief:
 *	Update the microphone volume on the remote device.
 *
 * Description.
 *	Update the microphone volume on the remote device.
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	context [in] - The VG Context.
 *
 *	channelId [in] - The channelId of the VG Channel that represents the connection.
 *
 *	volume [in] - The volume value to sent, this value must be less then or equals
 *		BTL_VG_MAX_VOLUME.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - Operation finished successfully.
 *		"volume" sent to the remote unit.
 *
 *	BT_STATUS_FAILED - Operation failed.
 *
 *	BT_STATUS_INVALID_PARM - Invalid parameter.
 */
BtStatus BTL_VG_SendMicrophoneVolume(const BtlVgContext *context,
									 BtlVgChannelId,
									 U32 volume);
/*---------------------------------------------------------------------------
 * BTL_VG_SendSpeakerVolume()
 *
 * Brief:
 *	Update the microphone volume on the remote device.
 *
 * Description.
 *	Update the microphone volume on the remote device.
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	context [in] - The VG Context.
 *
 *	channelId [in] - The channelId of the VG Channel that represents the connection.
 *
 *	volume [in] - The volume value to sent, this value must be less then or equals
 *		BTL_VG_MAX_VOLUME.
 * Returns:
 *	BT_STATUS_SUCCESS - Operation finished successfully.
 *		"volume" sent to the remote unit.
 *
 *	BT_STATUS_FAILED - Operation failed.
 *
 *	BT_STATUS_INVALID_PARM - Invalid parameter.
 */
BtStatus BTL_VG_SendSpeakerVolume(const BtlVgContext *context,
								  BtlVgChannelId channelId,
								  U32 volume);

/*---------------------------------------------------------------------------
 * BTL_VG_SendAtResult()
 *
 * Brief:
 *	Sends an AT result/response.
 
 * Description:
 *	Sends an AT result/response.
 *	Used to response to unrecognized AT commands.
 *	See BTL_VG_EVENT_UNRECOGNIZED_AT_COMMAND description.
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	context [in] - The VG Context.
 *
 *	channelId [in] - The channelId of the VG Channel that represents the connection.
 *
 *	atString [in] - AT Command in string format, no need to add <CR> & <LF>.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - Operation finished successfully.
 *		"atString" sent to the remote unit.
 *
 *	BT_STATUS_FAILED - Operation failed.
 *
 *	BT_STATUS_INVALID_PARM - Invalid parameter.
 */
BtStatus BTL_VG_SendAtResult(const BtlVgContext *context,
						BtlVgChannelId channelId,
						const char *atString);

/*---------------------------------------------------------------------------
 * BTL_VG_SentAtError()
 *
 * Brief:
 *	Sends ERROR to the remote unit.
 *
 * Description:
 *	Sends ERROR to the remote unit.
 *	Used to response with an ERROR to unrecognized AT commands.
 *	See BTL_VG_EVENT_UNRECOGNIZED_AT_COMMAND description.
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	context [in] - The VG Context.
 *
 *	channelId [in] - The channelId of the VG Channel that represents the connection.
 *
 *	err [in] - The err description to send, if EXTENDED ERRORS are not enabled
 *		"err" is ignored.
 *
 * Returns:
 *	BT_STATUS_SUCCESS - Operation finished successfully,
 *		ERROR sent to the remote unit.
 *
 *	BT_STATUS_FAILED - Operation failed.
 *
 *	BT_STATUS_INVALID_PARM - Invalid parameter.
 */
BtStatus BTL_VG_SentAtError(const BtlVgContext *context,
						BtlVgChannelId channelId,
						AtError err);
						
/*---------------------------------------------------------------------------
 * BTL_VG_ConnectAudio()
 *
 * Brief:
 *	Establishes audio connection to a remote device.
 *
 * Description:
 *	Establishes audio connection to a remote device
 *	A VG Connection must already exist.
 *	
 *
 * Type:
 *	Asynchronous or Synchronous
 *
 * Parameters:
 *	context [in] - The VG Context.
 *
 *	channelId [in] - A VG channel Id, audio will be established for
 *		the channel represented by the channelId.
 *
 * Generated Events:
 *	BTL_VG_EVENT_AUDIO_CONNECTED.
 *
 * Returns:
 *	BT_STATUS_PENDING - Operation started successfully.
 *
 *	BT_STATUS_FAILED - Operation failed to start.
 *
 *	BT_STATUS_INVALID_PARM - Invalid Parameter.
 */
BtStatus BTL_VG_ConnectAudio(BtlVgContext *context,
							 BtlVgChannelId channelId);

/*---------------------------------------------------------------------------
 * BTL_VG_DisconnectAudio()
 *
 * Brief:
 *	Disconnects an audio connection.
 *
 * Description:
 *	Disconnect an audio connection between the VG and a remote device.
 *
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context [in] - The VG Context.
 *
 *	channelId [in] - A channel Id, audio will be disconnected for the channel
 *		represented by the channel Id.
 *
 * Generated Events:
 *	BTL_VG_EVENT_AUDIO_DISCONNECTED.
 *
 * Returns:
 *	BT_STATUS_PENDING - Operation started successfully.
 *
 *	BT_STATUS_FAILED - Operation failed to start.
 *
 *	BT_STATUS_INVALID_PARM - Invalid Parameter.
 */
BtStatus BTL_VG_DisconnectAudio(BtlVgContext *context,
								BtlVgChannelId channelId);

#if (0)
/*---------------------------------------------------------------------------
 * BTL_VG_SetFmOverBtMode()
 *
 *		send vendor specific that enble \ disable FM over BT.	
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	context [in] - VG context.
 *
 *	channelId [in] - The channel Id.
 *
 *	enableFMoBT [in] - Disable/Enable FM over BT .
 *
 * Generated Events:
 *	BTL_VG_EVENT_FM_OVER_BT_ENABLED.
 *	BTL_VG_EVENT_FM_OVER_BT_DISABLED.
 *
 * Returns:
 *	BT_STATUS_PENDING - The operation has been started.
 *
 *	BT_STATUS_FAILED - the operation failed.
 *
 */
BtStatus BTL_VG_SetFmOverBtMode(BtlVgContext *context,
								BtlVgChannelId channelId,
								BOOL enableFMoBT);
#endif /* BTL_CONFIG_FM_STACK == BTL_CONFIG_ENABLED */

#endif /* __BTL_VG_H */


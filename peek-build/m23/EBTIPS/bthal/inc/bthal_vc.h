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
*   FILE NAME:		bthal_vc.h
*
*   BRIEF:			This file defines the API of the BTHAL Voice Control.
*
*   DESCRIPTION:		The BTHAL_VC Module is an adaptation layer to the audio resources
*					of the local system.
*					The supported operation includes: audio routing to the bluetooth chip,
*					Activation/Deactivation of voice recognition and noise reduction and echo cancelling.
*					
*					Using the API requires a BTHAL VC Context entity.
*					Multiple users are supported.
*
*   AUTHOR:			Itay Klein
*
\*******************************************************************************/

#ifndef __BTHAL_VC_H
#define __BTHAL_VC_H

/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/

#include "bthal_types.h"
#include "bthal_common.h"

#define BTHAL_VC_MAXIMAL_NUMBER_OF_USERS 3

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/
 
/*-------------------------------------------------------------------------------
 * BthalVcVoicePath type
 *
 *	Describes an audio path.
 */
typedef BTHAL_U32 BthalVcAudioPath;

#define BTHAL_VC_AUDIO_PATH_HANDSET		0x00000001

#define BTHAL_VC_AUDIO_PATH_BLUETOOTH	       0x00000002

/*-------------------------------------------------------------------------------
 * BthalVcAudioSource type
 *
 *	Describes an audio source,
 */
typedef BTHAL_U32 BthalVcAudioSource;

#define BTHAL_VC_AUDIO_SOURCE_MODEM				1

#define BTHAL_VC_AUDIO_SOURCE_FM_RADIO			2

#define BTHAL_VC_AUDIO_SOURCE_MULTIMEDIA_STACK	3

/*-------------------------------------------------------------------------------
 * BthalVcFmRxAudioPath type
 *
 *	Describes FM Rx audio path.
 */
typedef BTHAL_U32 BthalVcFmRxAudioPath;

#define BTHAL_VC_FM_RX_ANALOG_AUDIO_PATH_OFF	    (0)
#define BTHAL_VC_FM_RX_ANALOG_AUDIO_PATH_HEADSET    (1)
#define BTHAL_VC_FM_RX_ANALOG_AUDIO_PATH_HANDSET    (2)

/*-------------------------------------------------------------------------------
 * BthalVcEventType type.
 *
 *	Indicates the type of a VC Event.
 */
typedef BTHAL_I32 BthalVcEventType;

/* 
 * Indicates that the voice paths has been changed.
 * The "BthalVcEvent.p.route" is valid.
 */
#define BTHAL_VC_EVENT_VOICE_PATHS_CHANGED							0

/* 
 * Indicates that voice recognition feature has been enabled or
 * disabled.
 * The "BthalVcEvent.p.enabled" is valid.
 */
#define BTHAL_VC_EVENT_VOICE_RECOGNITION_STATUS						1

/* 
 * Indicates that the noise reduction and echo cancelling feature 
 * has been enabled or disabled within the VG.
 * The "BthalVcEvent.p.enabled" is valid.
 */
#define BTHAL_VC_EVENT_NOISE_REDUCTION_ECHO_CANCELLING_STATUS		2

/* 
 * Indicates successful registration with the VC Module
 */
#define BTHAL_VC_EVENT_LINK_ESTABLISHED								3

/* 
 * Indicates that the link with the VC Module is lost
 */
#define BTHAL_VC_EVENT_LINK_RELEASED								4

/* 
 * Indicates that the audio paths has been changed.
 * The "BthalVcEvent.p.route" is valid.
 */
#define BTHAL_VC_EVENT_FM_RX_AUDIO_PATH_CHANGED						5

/* Forward declaration */
typedef struct _BthalVcEvent BthalVcEvent;

/*-------------------------------------------------------------------------------
 * BthalVcCallback type
 *
 *	A function of this type is called to indicate BTHAL VC Events.
 */
typedef void (*BthalVcCallback)(const BthalVcEvent *event);

/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/

/* Forward declaration: */

typedef struct _BthalVcContext BthalVcContext;

struct _BthalVcEvent
{
	BthalVcEventType type;		/* The type of the event. */

	BthalStatus status;			/* The status of the event. */

	void *userData;

	BthalVcContext *context;		/* The event's context */

	union
	{
		BTHAL_BOOL enabled;

		struct
		{
			BthalVcAudioSource source;

			BthalVcAudioPath path;

			BthalVcFmRxAudioPath audioPath;

		} route;

	}p; /* The field 'event' defines which union field is valid,
		   look at the list of MC events for description on the various fields. */
};

/********************************************************************************
 *
 * Function Reference
 *
 *******************************************************************************/

/*---------------------------------------------------------------------------
 * BTHAL_VC_Init()
 *
 * Brief:
 *	Initializes the BTHAL Voice Control module.
 *
 * Description:
 *	Initializes the BTHAL Voice Control module.
 *	Must be called before any other BTHAL VC function.
 *
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	callback [in] - A callback for notifying on successful initialization.
 *
 * Returns:
 *	BTHAL_STATUS_SUCCESS - The operation succeeded.
 *
 *	BTHAL_STATUS_PENDING - Operation started successfully. Completion will be signaled 
 *								via an event to the callback
 *
 *	BTHAL_STATUS_FAILED - The operation failed.
 */
BthalStatus BTHAL_VC_Init(BthalCallBack	callback);

/*---------------------------------------------------------------------------
 * BTHAL_VC_Deinit()
 *
 * Brief:
 *	Deinitializes the BTHAL Voice Control module.
 *
 * Description:
 *	Deinitializes the BTHAL Voice Control module.
 *	Only BTHAL_VC_Init() can be called after this function.
 * 
 * Type:
 *	Synchronous.
 *
 * Parameters:
 *	void.
 *
 * Returns:
 *	BTHAL_STATUS_SUCCESS - The operation succeeded.
 *
 *	BTHAL_STATUS_FAILED - The operation failed.
 */
BthalStatus BTHAL_VC_Deinit(void);

/*---------------------------------------------------------------------------
 * BTHAL_VC_Register()
 *
 * Brief:
 *	Registers a BthalVcContext.
 *
 * Description:
 *	Register and allocates a BTHAL VC Context.
 *	A context is needed for interacting with the BTHAL VC Module.
 *	the event: BTHAL_VC_EVENT_LINK_ESTABLISHED is sent
 *	when the registration is completed.
 * 
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	callback [in] - BTHAL VC Events will be sent to this callback
 *
 *	userData [in] - 'userData' will be returned to the user upon every BTHAL VC Event.
 *
 *	context[out] - A BTHAL VC Context.
 *
 * Generated Events:
 *	BTHAL_VC_EVENT_LINK_ESTABLISHED.
 *
 * Returns:
 *	BTHAL_STATUS_PENDING - The operation started successfully.
 *
 *	BTHAL_STATUS_FAILED - The operation failed.
 *
 *	BTHAL_STATUS_INVALID_PARM - The 'callback' argument is invalid.
 */
BthalStatus BTHAL_VC_Register(BthalVcCallback callback,
							void *userData,
							BthalVcContext **context);

/*---------------------------------------------------------------------------
 * BTHAL_VC_Unregister()
 *
 * Brief:
 *	Unregister a BTHAL VC Context.
 *
 * Description:
 *	Unregisters and deallocate a VC Context previously registered by
 *	BTHAL_VC_Register().
 *	The event BTHAL_VC_EVENT_LINK_RELEASED will be sent to the user
 *	upon the completion of the operation.
 * 
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context[in] - A BTHAL VC Context, when BTHAL_VC_Unregister() returns
 *		context is set to 0.
 *
 * Generated Events:
 *	BTHAL_VC_EVENT_LINK_RELEASED.
 *
 * Returns:
 *	BTHAL_STATUS_PENDING - The operation started successfully.
 *
 *	BTHAL_STATUS_FAILED - The operation failed.
 *
 *	BTHAL_STATUS_INVALID_PARM - The 'callback' argument is invalid.
 */
BthalStatus BTHAL_VC_Unregister(BthalVcContext **context);

/*---------------------------------------------------------------------------
 * BTHAL_VC_SetVoicePath()
 *
 * Brief:
 *	Sets the voice path.
 *
 * Description:
 *	Sets the voice path within the system.
 *	the voice path define where the audio is routed (bluetooth, handset and etc.).
 *
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context [in] - A BTHAL VC Context.
 *
 *	audioPath [in] - Where to route the audio.
 *
 * Generated Events:
 *	BTHAL_VC_EVENT_VOICE_PATHS_CHANGED.
 *
 * Returns:
 *	BTHAL_STATUS_PENDING - The operation has started successfully,
 *		The event BTHAL_VC_EVENT_VOICE_PATHS_CHANGED will inform on the operation result.
 *		if the "BthalVcEvent.status" equals BTHAL_STATUS_SUCCESS then the operation is successful.
 *		otherwise, the operation failed.
 *
 *	BTHAL_STATUS_FAILED - The operation failed to start.
 *
 *	BTHAL_STATUS_INVALID_PARM - The 'path' argument is  invalid.
 */
BthalStatus BTHAL_VC_SetVoicePath(BthalVcContext *context,
								  BthalVcAudioSource source,
								  BthalVcAudioPath path);

/*---------------------------------------------------------------------------
 * BTHAL_VC_SetVoiceRecognition()
 *
 * Brief:
 *	Enables/Disables voice recognition.
 *
 * Description:
 *	Enables/Disables voice recognition.
 *
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context [in] - A BTHAL VC Context.
 *
 *	enable [in] - If TRUE, voice recognition is enabled,
 *		otherwise, it is disabled.
 *
 * Generated Events:
 *	BTHAL_VC_EVENT_VOICE_RECOGNITION_STATUS.
 *
 * Returns:
 *	BTHAL_STATUS_PENDING - The operation has started successfully,
 *		The event BTHAL_VC_EVENT_VOICE_RECOGNITION_STATUS will inform on the operation result.
 *		if the "BthalVcEvent.status" equals BTHAL_STATUS_SUCCESS then the operation is successful.
 *		otherwise, the operation has failed.
 *
 *	BTHAL_STATUS_FAILED - The operation failed to start.
 */
BthalStatus BTHAL_VC_SetVoiceRecognition(BthalVcContext *context,
										 BTHAL_BOOL enable);

/*---------------------------------------------------------------------------
 * BTHAL_VC_SetNoiseReductionAndEchoCancelling()
 *
 * Brief:
 *	Enables/Disables noise reduction and echo cancelling.
 *
 * Description:
 *	Enables/Disables noise reduction and echo cancelling.
 *
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context [in] - A BTHAL VC Context.
 *
 *	enable [in] - If TRUE, noise reduction and echo cancelling is enabled,
 *		otherwise, it is disabled.
 *
 * Generated Events:
 *	BTHAL_VC_EVENT_NOISE_REDUCTION_ECHO_CANCELLING_STATUS.
 *
 * Returns:
 *	BTHAL_STATUS_PENDING - The operation has started successfully,
 *		The event BTHAL_VC_EVENT_NOISE_REDUCTION_ECHO_CANCELLING_STATUS
 *		will inform on the operation result, if the "BthalVcEvent.status" equals BTHAL_STATUS_SUCCESS,
 *		then the operation is successful, otherwise, the operation has failed.
 *
 *	BTHAL_STATUS_FAILED - The operation failed to start.
 */
BthalStatus BTHAL_VC_SetNoiseReductionAndEchoCancelling(BthalVcContext *context,
														BTHAL_BOOL enable);

#if (0)
/*---------------------------------------------------------------------------
 * BTHAL_VC_FmRxSetAudioPath()
 *
 * Brief:
 *	Sets the FM Rx audio path.
 *
 * Description:
 *	Sets the FM Rx audio path within the system: handset, wired headset or off.
 *
 * Type:
 *	Asynchronous or Synchronous.
 *
 * Parameters:
 *	context [in] - A BTHAL VC Context.
 *
 *	audioPath [in] - Where to route the audio.
 *
 * Generated Events:
 *	BTHAL_VC_EVENT_FM_RX_AUDIO_PATH_CHANGED.
 *
 * Returns:
 *	BTHAL_STATUS_SUCCESS - The operation has successfully finished.
 *
 *	BTHAL_STATUS_PENDING - The operation has started successfully,
 *		The event BTHAL_VC_EVENT_FM_RX_AUDIO_PATH_CHANGED will inform on the operation
 *           result.
 *		if the "BthalVcEvent.status" equals BTHAL_STATUS_SUCCESS, the operation is successful,
 *		otherwise, the operation failed.
 *
 *	BTHAL_STATUS_FAILED - The operation failed to start.
 *
 *	BTHAL_STATUS_INVALID_PARM - The 'audioPath' argument is  invalid.
 */
BthalStatus BTHAL_VC_FmRxSetAudioPath(BthalVcContext *context,
								      BthalVcFmRxAudioPath audioPath);
#endif /* BTL_CONFIG_FM_STACK == BTL_CONFIG_ENABLED */

#endif


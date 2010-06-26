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
*   FILE NAME:      btl_a2dp.h
*
*   BRIEF:          This file defines the API of the BTL A2DP Source role.
*                  
*
*   DESCRIPTION:    General
*
*		The A2DP soure sends high quality audio to A2DP sink (headset).
*		The BTL A2DP API enables applications to create/configure/control/close audio
*            streams with A2DP headsets.
*
*            Audio streams can be from SBC or MP3 types. In MP3 case the MP3 encoder should
*            be external to the stack (e.g. MP3 files provided from the MM). In SBC case the encoding
*            can be either built-in (in the stack), or external (at the MM).
*
*            Multiple sinks are supported (currently up to two). They must be from the same stream
*            type, e.g. same SBC audio can be streamed (on two separate streams) to two different
*            sinks simultaneously.
*
*   AUTHOR:         Keren Ferdman
*
\*******************************************************************************/

#ifndef __BTL_A2DP_H
#define __BTL_A2DP_H


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include <bttypes.h>
#include <btl_common.h>
#include <a2dp.h>
#include <sec.h>
#include "btl_unicode.h"


/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/* Forward declaration */
typedef struct _BtlA2dpEvent BtlA2dpEvent;
typedef struct _BtlA2dpContext BtlA2dpContext;


/*-------------------------------------------------------------------------------
 * BtlA2dpCallBack type
 *
 *     A function of this type is called to indicate BTL A2DP events.
 */
typedef void (*BtlA2dpCallBack)(const BtlA2dpEvent *event);


/*-------------------------------------------------------------------------------
 * BtlA2dpStreamId type
 *
 *     An ID for identifying a stream within a A2DP context.
 */
typedef U32 BtlA2dpStreamId;


/*-------------------------------------------------------------------------------
 * BtlA2dpSupportedFeaturesMask type
 *
 *     Defines which features are supported in the A2DP SRC, can be ORed together.
 */
typedef U8 BtlA2dpSupportedFeaturesMask;

#define BTL_A2DP_SUPPORTED_FEATURES_PLAYER				(0x01)
#define BTL_A2DP_SUPPORTED_FEATURES_MICROPHONE			(0x02)	
#define BTL_A2DP_SUPPORTED_FEATURES_TUNER				(0x04)	
#define BTL_A2DP_SUPPORTED_FEATURES_MIXER				(0x08)


/*-------------------------------------------------------------------------------
 * BtlA2dpCodec type
 *
 *     Defines the codec type of the A2DP stream.
 */
typedef U8 BtlA2dpCodec;

#define BTL_A2DP_CODEC_SBC								(0x00)
#define BTL_A2DP_CODEC_MPEG1_2_AUDIO					(0x01)	


/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/


/*-------------------------------------------------------------------------------
 * BtlA2dpEvent structure
 *
 *     Represents BTL A2DP event.
 */
struct _BtlA2dpEvent
{
	/* Associated context */
	BtlA2dpContext 		*a2dpContext;

	/* Associated channel id */
	BtlA2dpStreamId  		streamId;

	/* A2DP SRC event */
	A2dpCallbackParms 	*callbackParms;
};


/********************************************************************************
 *
 * Function declarations
 *
 *******************************************************************************/


/*-------------------------------------------------------------------------------
 * BTL_A2DP_Init()
 *
 * Brief:  
 *          Initialize the A2DP module.
 *      
 * Description:
 *		Initialize the A2DP module.  This function should only be called once, 
 *		normally at initialization time.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		void.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 *		
 *		BT_STATUS_INTERNAL_ERROR - Internal error has occur.
 */
BtStatus BTL_A2DP_Init(void);


/*-------------------------------------------------------------------------------
 * BTL_A2DP_Deinit()
 *
 * Brief:  
 *		Deinit the A2DP module.
 *      
 * Description:
 *		Deinit the A2DP module.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		void.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 *		
 *		BT_STATUS_INTERNAL_ERROR - Internal error has occur.
 */
BtStatus BTL_A2DP_Deinit(void);


/*-------------------------------------------------------------------------------
 * BTL_A2DP_Create()
 *
 * Brief:  
 *		Allocates a unique A2DP context.
 *      
 * Description:
 *		This function must be called before any other A2DP API function.
 *		The allocated context should be supplied in subsequent A2DP API calls.
 *		The caller must also provide a callback function, which will be called 
 *		on A2DP events.
 *		The caller can provide an application handle (previously allocated 
 *		with BTL_RegisterApp), in order to link between different modules.
 *		If there is no need to link between different modules, set appHandle to 0.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		appHandle [in] - application handle, can be 0.
 *
 *		a2dpCallback [in] - all A2DP events will be sent to this callback.
 *
 *		securityLevel [in] - level of security which should be applied,	when
 *			connecting to a remote A2DP controller.
 *			If NULL is passed, default value is used.
 *		
 *		a2dpContext [out] - allocated A2DP context.	
 *
 * Returns:
 *		BT_STATUS_SUCCESS - A2DP context was created successfully.
 *
 *		BT_STATUS_FAILED -  Failed to create A2DP context.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *		BT_STATUS_NO_RESOURCES - No more resources.
 *
 *		BT_STATUS_INTERNAL_ERROR - Internal error has occur.
 */
BtStatus BTL_A2DP_Create(BtlAppHandle *appHandle,
							const BtlA2dpCallBack a2dpCallback,
							const BtSecurityLevel *securityLevel,
							BtlA2dpContext **a2dpContext);


/*-------------------------------------------------------------------------------
 * BTL_A2DP_Destroy()
 *
 * Brief:  
 *           Releases a A2DP context
 *      
 * Description:
 *		Releases a A2DP context (previously allocated with BTL_A2DP_Create).
 *		An application should call this function when it completes using A2DP services.
 *		Upon completion, the A2DP context is set to null in order to prevent 
 *		the application from an illegal attempt to keep using it.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		a2dpContext [in/out] - A2DP context.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - A2DP context was destroyed successfully.
 *
 *		BT_STATUS_IN_USE -  A2DP context is in use.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *		BT_STATUS_INTERNAL_ERROR - Internal error has occur.
 */
BtStatus BTL_A2DP_Destroy(BtlA2dpContext **a2dpContext);


/*-------------------------------------------------------------------------------
 * BTL_A2DP_Enable()
 *
 * Brief:  
 *           Enable A2DP and register A2DP SDP record
 *      
 * Description:
 *		Enable A2DP and register A2DP SDP record, called after 
 *		BTL_A2DP_Create.
 *		After calling this function, A2DP is ready for usage.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		a2dpContext [in] - A2DP context.
 *
 *		serviceName [in] - null-terminated service name which will be written into 
 *			SDP service record and will be discovered by the peer device.
 *			If NULL is passed, default value is used.
 *			The pointer is free after the function returns.
 *
 *		supportedFeatures [in] - The features supported by the SRC, 
 *			which will be written into SDP service record and will be discovered 
 *			by the peer device (see BtlA2dpSupportedFeaturesMask).
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_IN_USE - The context is already enabled.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *		BT_STATUS_FAILED - The specified context is already enabled.
 */
BtStatus BTL_A2DP_Enable(BtlA2dpContext *a2dpContext,
						const BtlUtf8 *serviceName,
						const BtlA2dpSupportedFeaturesMask supportedFeatures);


/*-------------------------------------------------------------------------------
 * BTL_A2DP_Disable()
 *
 * Brief:  
 *          Disable the A2DP context
 *      
 * Description:
 *		Disable A2DP context, called before BTL_A2DP_Destroy.
 *		If any connections exist, they will be disconnected automatically.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		a2dpContext [in] - A2DP context.
 *
 * Generated Events:
 *       A2DP_EVENT_STREAM_CLOSED
 *       A2DP_EVENT_DISABLED
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *		
 *		BT_STATUS_PENDING - At least one stream is connected. The disable process has 
 *			been successfully started. On each connected stream, the application callback 
 *			will receive A2DP_EVENT_STREAM_CLOSED event.
 *			When the disable process is complete, the application callback will 
 *			receive the A2DP_EVENT_DISABLED event, indicating the context is disabled.
 */
BtStatus BTL_A2DP_Disable(BtlA2dpContext *a2dpContext);


/*---------------------------------------------------------------------------
 * BTL_A2DP_ConnectStream()
 *
 * Brief:  
 *      Tries to establish a stream to a given remote device using a given codec.
 *      
 * Description:
 *    This function searches for a stream with capabilities matching those 
 *	  that were registered.  
 *	  The application will receive the A2DP_EVENT_GET_CONFIG_IND event.
 *	  When this event has been received , the stream is in idle state.
 *
 *	  Afterwards, the application should call BTL_A2DP_OpenStream in order to 
 *	  open the stream.
 *
 *    BTL_A2DP_AbortStream() can be called to close the stream to abort this 
 *	  process if necessary.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *
 *     a2dpContext [in] - A2dp context
 *
 *     bdAddr [in] - pointer to 48-bit address of the device.
 *
 *	   codec [in] - type of codec of the A2DP stream.
 *
 *	   streamId [out] - Identifies the ID that corresponds to this stream.
 *
 * Generated Events:
 *       A2DP_EVENT_GET_CONFIG_IND
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The connect operation was started 
 *         successfully.
 *
 *	   BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *     BT_STATUS_IN_USE - The specified stream is already in use.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel or stream is not registered.
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus BTL_A2DP_ConnectStream(BtlA2dpContext *a2dpContext,
								BD_ADDR *bdAddr, 
								BtlA2dpCodec codec,
								BtlA2dpStreamId *streamId);


/*---------------------------------------------------------------------------
 * BTL_A2DP_OpenStream()
 *
 * Brief:  
 *    Opens a stream on the remote device.
 *      
 * Description:
 *    This function opens a stream on the remote device.  
 *	  The application will receive the A2DP_STREAM_OPEN event.
 *	  When this event has been received with no errors, the stream is open.  
 *	  If no streams with matching capabilities can be found, the stream is closed.
 *
 *    BTL_A2DP_AbortStream() can be called to close the stream to abort this process
 *    if necessary.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *
 *     a2dpContext [in] - A2dp context
 *
 *	   streamId [in] - the stream ID
 *
 * Generated Events:
 *      A2DP_EVENT_STREAM_OPEN
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The Stream config operation was started 
 *         successfully.  The stream open process will continue.  Once the
 *         stream is open, the A2DP_EVENT_STREAM_OPEN event will be received.
 *
 *	   BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *     BT_STATUS_FAILED - The specified stream is not in the correct state.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel or stream is not registered.
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus BTL_A2DP_OpenStream(BtlA2dpContext *a2dpContext,
							BtlA2dpStreamId streamId);


/*---------------------------------------------------------------------------
 * BTL_A2DP_OpenStreamResponse()
 *
 * Brief:  
 *    Responds to an request from a sink to open a connection.
 *      
 * Description:
 *    Responds to an request to open a connection with the remote device 
 *    (See A2DP_EVENT_STREAM_OPEN_IND).  The open request is accepted by the 
 *    application if this function is called with A2DP_ERR_NO_ERROR.  Any other 
 *    error code rejects the request.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *
 *     	a2dpContext [in] - A2dp context
 *
 *		streamId [in] - the stream ID
 *
 *     	error [in] - If the request to open the connection is granted then
 *      	A2DP_ERR_NO_ERROR is passed in this parameter, otherwise another
 *          appropriate error code should be used.
 *
 * Generated Events:
 *       A2DP_EVENT_STREAM_OPEN
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The Open Stream Response operation was started 
 *         successfully.  An A2DP_EVENT_STREAM_OPEN event will be received 
 *         when the stream is open.
 *	
 *	   BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *     BT_STATUS_NO_CONNECTION - The specified stream did not request a 
 *         connection.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel or stream is not registered.
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus BTL_A2DP_OpenStreamResponse(BtlA2dpContext *a2dpContext,
									BtlA2dpStreamId streamId, 
									A2dpError error);


/*---------------------------------------------------------------------------
 * BTL_A2DP_CloseStream()
 *
 * Brief:  
 *      Initiate the closing an open stream.
 *
 * Description:
 *     Initiate the closing an open stream.
 *	   The lower layers (L2CAP and ACL) will be disconnected.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *
 *		a2dpContext [in] - A2dp context
 *
 *		streamId [in] - the stream ID
 *
 * Generated Events:
 *      A2DP_EVENT_STREAM_CLOSED
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The Close Stream operation was started 
 *         successfully.  An A2DP_EVENT_STREAM_CLOSED event will be received 
 *         when the stream is closed.
 *
 *     BT_STATUS_FAILED - The stream is not in the open or active state.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel or stream is not registered.
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus BTL_A2DP_CloseStream(BtlA2dpContext *a2dpContext,
							BtlA2dpStreamId streamId);


/*---------------------------------------------------------------------------
 * BTL_A2DP_IdleStream()
 *
 * Brief:  
 *     Put a stream into the Idle state. 
 *      
 * Description:
 *     Put the stream into the Idle state.  This differs from closing the
 *     stream, because the lower layers (L2CAP and ACL) stay connected.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *
 *		a2dpContext [in] - A2dp context
 *
 *		streamId [in] - the stream ID
 *
 * Generated Events:
 *       A2DP_EVENT_STREAM_IDLE
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The Idle Stream operation was started 
 *         successfully.  An A2DP_EVENT_STREAM_IDLE event will be received 
 *         when the stream is closed.
 *
 *     BT_STATUS_FAILED - The stream is not in the open or active state.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel or stream is not registered.
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus BTL_A2DP_IdleStream(BtlA2dpContext *a2dpContext,
							BtlA2dpStreamId streamId);


/*---------------------------------------------------------------------------
 * BTL_A2DP_StartStream()
 *
 * Brief:  
 *     Initiates streaming on the open stream.
 *      
 * Description:
 *     Initiates streaming on the open stream.  Calling this function puts the
 *     stream into an active state, which allows media packets to be sent on 
 *     the stream.
 *	   NOTE: This function should be called when the stream is in 'open' state 
 *	   after the event A2DP_EVENT_STREAM_CONFIGURED was received, meaning the 
 *	   stream is configured properly.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *
 *		a2dpContext [in] - A2dp context
 *
 *		streamId [in] - the stream ID
 *
 * Generated Events:
 *       A2DP_EVENT_STREAM_STARTED
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The operation was initiated successfully.  
 *         An A2DP_EVENT_STREAM_STARTED event will be received when the stream
 *         is ready for streaming.
 *
 *     BT_STATUS_FAILED - The stream is not in the open state.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *     BT_STATUS_IN_USE - One of the specified streams is already in use.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel or stream is not registered.
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus BTL_A2DP_StartStream(BtlA2dpContext *a2dpContext,
							BtlA2dpStreamId streamId);


/*---------------------------------------------------------------------------
 * BTL_A2DP_StartStreamResponse()
 *
 * Brief:  
 *     Respond to a request from the sink to start a stream
 *      
 * Description:
 *     Called in response to an A2DP_EVENT_STREAM_START_IND event.  Calling this
 *     function will either accept the streaming request or reject it.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *
 *		a2dpContext [in] - A2dp context
 *
 *		streamId [in] - the stream ID
 *
 *      error - If the streaming request is accepted, this parameter must be
 *         set to A2DP_ERR_NO_ERROR.  If the streaming request is rejected,
 *         this parameter must be set to the appropriate error defined by
 *         A2dpError.
 *
 * Generated Events:
 *       A2DP_EVENT_STREAM_STARTED 
 *       A2DP_EVENT_STREAM_SUSPENDED
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The Start Streams Response operation was started 
 *         successfully.  An A2DP_EVENT_STREAM_STARTED event will be received 
 *         when the stream has been started.  If the start stream request was 
 *         rejected, the A2DP_EVENT_STREAM_SUSPENDED event will be received.
 *
 *     BT_STATUS_FAILED - The stream is not in the open state.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *     BT_STATUS_IN_USE - One of the specified streams is already in use.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel or stream is not registered.
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus BTL_A2DP_StartStreamResponse(BtlA2dpContext *a2dpContext,
									BtlA2dpStreamId streamId, 
									A2dpError error);


/*---------------------------------------------------------------------------
 * BTL_A2DP_SuspendStream()
 *
 * Brief:  
 *     Suspends a stream.
 *      
 * Description:
 *     Suspends a stream that is currently active.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *
 *		a2dpContext [in] - A2dp context
 *
 *		streamId [in] - the stream ID
 *
 * Generated Events:
 *       A2DP_EVENT_STREAM_SUSPENDED
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The Suspend Streams Response operation was started 
 *         successfully.  An A2DP_EVENT_STREAM_SUSPENDED event will be received 
 *         when the stream has been suspended.
 *
 *     BT_STATUS_FAILED - The stream is not in an active state.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel or stream is not registered.
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus BTL_A2DP_SuspendStream(BtlA2dpContext *a2dpContext,
								BtlA2dpStreamId streamId);


/*---------------------------------------------------------------------------
 * BTL_A2DP_AbortStream()
 *
 * Brief:  
 *     Aborts any open or active stream.
 *      
 * Description:
 *     Aborts any open or active stream.  Once aborted, an open stream will
 *     be in a closed state.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *
 *     a2dpContext [in] - A2dp context
 *
 *		streamId [in] - the stream ID
 *
 * Generated Events:
 *       A2DP_EVENT_STREAM_ABORTED
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The Abort Stream operation was started 
 *         successfully.  An A2DP_EVENT_STREAM_ABORTED event will be received 
 *         when the stream has been aborted.
 *
 *     BT_STATUS_FAILED - The stream is not in the correct state.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel or stream is not registered.
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus BTL_A2DP_AbortStream(BtlA2dpContext *a2dpContext,
								BtlA2dpStreamId streamId);


/*-------------------------------------------------------------------------------
 * BTL_A2DP_GetConnectedDevice()
 *
 * Brief:  
 *		Gets the connected device.
 *      
 * Description:
 *		This function returns the connected device.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		a2dpContext [in] - A2DP context.
 *
 *		streamId [in] - associated stream id.
 *
 *		bdAddr [out] - pointer to 48-bit address of the connected device.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *		BT_STATUS_NO_CONNECTION -  The target is not connected.
 */
BtStatus BTL_A2DP_GetConnectedDevice(BtlA2dpContext *a2dpContext, 
										BtlA2dpStreamId streamId, 
										BD_ADDR *bdAddr);


/*-------------------------------------------------------------------------------
 * BTL_A2DP_SetSecurityLevel()
 *
 * Brief:  
 *     	Sets security level for the given A2DP context.
 *      
 * Description:
 *     	Sets security level for the given A2DP context.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *     	a2dpContext [in] - pointer to the A2DP context.
 *
 *		secLevel [in] - level of security which should be applied,	when
 *			connecting to a remote HS. For default value, pass null.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_A2DP_SetSecurityLevel(BtlA2dpContext *a2dpContext,
								const BtSecurityLevel *securityLevel);


/*-------------------------------------------------------------------------------
 * BTL_A2DP_GetSecurityLevel()
 *
 * Brief:  
 *     	Gets security level for the given A2DP context.
 *
 * Description:
 *     	Gets security level for the given A2DP context.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *     	a2dpContext [in] - pointer to the A2DP context.
 *
 *		secLevel [out] - level of security which should be applied, when
 *			connecting to a remote HS.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_A2DP_GetSecurityLevel(BtlA2dpContext *a2dpContext,
								BtSecurityLevel *securityLevel);

#endif /* __BTL_A2DP_H */


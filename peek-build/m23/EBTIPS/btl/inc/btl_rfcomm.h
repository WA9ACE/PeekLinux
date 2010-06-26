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
*   FILE NAME:      btl_rfcomm.h
*
*	BRIEF:          This file defines the API of the BTL RFCOMM protocol client 
*					and server roles.
*
*   DESCRIPTION:    General
*
*					RFCOMM is a protocol that emulates a number of serial ports over
*     				the L2CAP protocol. RFCOMM is based on another standard (ETSI's
*     				TS 07.10 standard, version 6.3.0) and also contains some
*     				Bluetooth-specific adaptations.
*
*     				RFCOMM supports up to 60 multiplexed ports (each of which is
*     				known as an RFCOMM channel) for each device connection. However,
*     				the total number of channels is limited by this implementation
*     				to conserve RAM. (See the NUM_RF_CHANNELS configuration.)
*
*     				RFCOMM may also be used to emulate RS-232 control signals and
*     				baud rate changes.
*
*   AUTHOR:         Ilan Elias
*
\*******************************************************************************/


#ifndef __BTL_RFCOMM_H
#define __BTL_RFCOMM_H


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include <rfcomm.h>
#include <sdp.h>
#include <sec.h>
#include <btl_common.h>


/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/* Forward declarations */
typedef struct _BtlRfcommEvent 		BtlRfcommEvent;
typedef struct _BtlRfcommContext 	BtlRfcommContext;


/*-------------------------------------------------------------------------------
 * BtlRfcommChannelId type
 *
 *     Defines the type of an RFCOMM channel id.
 */
typedef U32 BtlRfcommChannelId;

 
/*-------------------------------------------------------------------------------
 * BtlRfcommCallBack type
 *
 *     A function of this type is called to indicate BTL RFCOMM events.
 */
typedef void (*BtlRfcommCallBack)(const BtlRfcommEvent *event);


/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BtlRfcommEvent structure
 *
 *     Represents BTL RFCOMM event.
 */
struct _BtlRfcommEvent 
{
	/* Associated context */
	BtlRfcommContext		*rfcommContext;

	/* Associated channel id */
	BtlRfcommChannelId		channelId;

	/* RFCOMM event */
	RfCallbackParms			*rfcommEvent;
};


/********************************************************************************
 *
 * Function declarations
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_Init()
 *
 *	Brief:  
 *		Init the BTL RFCOMM module.
 *
 *	Description:
 *		Init the BTL RFCOMM module.
 *		Allocates resources needed for the BTL RFCOMM module.
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
BtStatus BTL_RFCOMM_Init(void);


/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_Deinit()
 *
 *	Brief:  
 *		Deinit the BTL RFCOMM module.
 *
 *	Description:
 *		Deinit the BTL RFCOMM module.
 *		Deallocates resources needed for the BTL RFCOMM module.
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
BtStatus BTL_RFCOMM_Deinit(void);

 
/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_Create()
 *
 *	Brief:  
 *		Allocates a unique RFCOMM context.
 *
 *	Description:
 *		Allocates a unique RFCOMM context.
 *		This function must be called before any other RFCOMM API function.
 *		The allocated context should be supplied in subsequent RFCOMM API calls.
 *		The caller must also provide a callback function, which will be called 
 *		on RFCOMM events.
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
 *		rfcommCallback [in] - all RFCOMM events will be sent to this callback.
 *
 *		securityLevel [in] - level of security which should be applied,	when
 *			connecting to a remote RFCOMM channel.
 *			If NULL is passed, default value is used.
 *
 *		service [in] -  service which is on top of this 
 *			RFCOMM channel (used later when BTEVENT_AUTHORIZATION_REQ is generated 
 *			to identify the service which caused the event).
 *		
 *		rfcommContext [out] - allocated RFCOMM context.	
 *
 * Returns:
 *		BT_STATUS_SUCCESS - RFCOMM context was created successfully.
 *
 *		BT_STATUS_FAILED -  Failed to create RFCOMM context.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *		BT_STATUS_NO_RESOURCES - No more resources.
 *
 *		BT_STATUS_INTERNAL_ERROR - Internal error has occur.
 */
BtStatus BTL_RFCOMM_Create(BtlAppHandle *appHandle,
							const BtlRfcommCallBack rfcommCallback,
							const BtSecurityLevel *securityLevel,
							const SdpServicesMask service,
							BtlRfcommContext **rfcommContext);


/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_Destroy()
 *
 *	Brief:  
 *		Releases a RFCOMM context (previously allocated with BTL_RFCOMM_Create).
 *
 *	Description:
 *		Releases a RFCOMM context (previously allocated with BTL_RFCOMM_Create).
 *		An application should call this function when it completes using RFCOMM services.
 *		Upon completion, the RFCOMM context is set to null in order to prevent 
 *		the application from an illegal attempt to keep using it.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		rfcommContext [in/out] - RFCOMM context.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - RFCOMM context was destroyed successfully.
 *
 *		BT_STATUS_IN_USE -  RFCOMM context is in use.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *		BT_STATUS_INTERNAL_ERROR - Internal error has occur.
 */
BtStatus BTL_RFCOMM_Destroy(BtlRfcommContext **rfcommContext);


/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_RegisterServerChannel()
 *
 *	Brief:  
 *		Register RFCOMM server channel.
 *
 *	Description:
 *		Register RFCOMM server channel.
 *		After this call, a remote device may attempt to connect to the service.
 *
 *		When registering the service, it's a good idea to add a corresponding 
 *		SDP entry.
 *
 *		RFCOMM allows connections from multiple remote devices to the same
 *     	service. To achieve this, the RFCOMM user must call this function
 *     	repeatedly to associate a set of RFCOMM channels with the same service.
 *
 *		After calling this function, RFCOMM server channel is ready for usage.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		rfcommContext [in] - RFCOMM context.
 *
 *		maxFrameSize [in] - Frame size for the channel. This value must be between
 * 			RF_MIN_FRAME_SIZE and RF_MAX_FRAME_SIZE.
 *
 *		credit [in] - This parameter is used to specify an initial amount of credit 
 *         	that will be granted to a client upon connection.  The amount of 
 *         	credit that is advanced tells the client how many RFCOMM packets it 
 *         	can send before flow control is shut off from client to server.  
 *         	In order to allow the client to send more data, BTL_RFCOMM_AdvanceCredit 
 *			must be called.  If the remote device does not support credit-based flow 
 *         	control, then the flow control is controlled automatically by the FC 
 *         	bit in the modem status command.
 *
 *		channelId [out] - Identifies the ID that corresponds to this channel.
 *			Each time this function is called, a new channel will be registered.
 *
 *		serviceId [out] - Identifies the ID that corresponds to this service.
 *			After the service has been registered, it contains an RFCOMM ID.
 *			Register this ID with SDP so that remote devices can locate
 * 			this RFCOMM service.
 *			The same serviceId will be returned when calling this function several 
 *			times with the same context (for creating several channels with the 
 *			same serviceId).
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Service was successfully registered.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *		BT_STATUS_NO_RESOURCES - The system has run out of resources
 *         and cannot register the channel. 
 *
 *		BT_STATUS_FAILED - Service failed to register because the number
 *         	of servers or channels would exceed the maximum.
 */
BtStatus BTL_RFCOMM_RegisterServerChannel(BtlRfcommContext *rfcommContext, 
											U16 maxFrameSize,
											U8 credit,
											BtlRfcommChannelId *channelId,
											U8 *serviceId);


/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_DeregisterServerChannel()
 *
 *	Brief:  
 *		Deregisters a channel from an RFCOMM service.
 *
 *	Description:
 *		Deregisters a channel from an RFCOMM service.
 *		If a connection exists, it will be disconnected automatically.
 *
 *		When deregistering all channels from the service, it's a good idea to 
 *		remove the corresponding SDP entry.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		rfcommContext [in] - RFCOMM context.
 *
 *		channelId [in] - Channel to deregister from the service.
 *
 * Generated Events:
 *      RFEVENT_CLOSED
 *
 * Returns:
 *		BT_STATUS_SUCCESS - The channel was successfully deregistered.
 *
 *		BT_STATUS_FAILED - The channel could not be found, or it is in ivalid state.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *		
 *		BT_STATUS_PENDING - The link is connected. The disconnect process has 
 *			been successfully started. When the disconnect process is complete, the
 *         	application callback will receive the RFEVENT_CLOSED event,
 *			which indicates the RFCOMM channel is deregistered.
 */
BtStatus BTL_RFCOMM_DeregisterServerChannel(BtlRfcommContext *rfcommContext, 
											BtlRfcommChannelId channelId);


/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_ConnectResponse()
 *
 *	Brief:  
 *		Responds to a connection request from the remote device.
 *
 *	Description:
 *     	Responds to a connection request from the remote device, indicated by
 *		the event RFEVENT_OPEN_IND.
 *		This event occurs when a remote client attempts to connect to a
 *     	local RFCOMM server channel.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *     	rfcommContext [in] - pointer to the RFCOMM context.
 *
 *		channelId [in] - associated channel id, received in RFEVENT_OPEN_IND event.
 *
 *		acceptCon [in] - TRUE accepts the connection or FALSE rejects the connection.
 *
 * Generated Events:
 *      RFEVENT_OPEN
 *		RFEVENT_CLOSED
 *
 * Returns:
 *		BT_STATUS_PENDING - The accept (reject) message will be sent. The application
 *         	will receive a RFEVENT_OPEN (RFEVENT_CLOSED) when the accept (reject) message 
 *			has been sent and the channel is open (closed).
 *
 *		BT_STATUS_FAILED - The specified channel did not have a pending
 *         	connection request or the stack has run out of resources.
 *
 *     	BT_STATUS_NO_CONNECTION - No L2CAP connection exists.
 *
 *     	BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_RFCOMM_ConnectResponse(BtlRfcommContext *rfcommContext, 
									BtlRfcommChannelId channelId,
									BOOL acceptCon);


/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_ConnectClientChannel()
 *
 *	Brief:  
 *		Attempts to establish a channel with a remote service.
 *
 *	Description:
 *     	Attempts to establish a channel with a remote service. The RFCOMM
 *     	user must identify the remote service by its BD address and remote
 *     	service ID.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *     	rfcommContext [in] - pointer to the RFCOMM context.
 *
 *		bdAddr [in] - pointer to 48-bit address of the device.
 *
 *		serviceId [in] - Identifies the RFCOMM server ID on the remote device.
 *         	The server ID can be determined by querying the remote device's
 *         	SDP server.
 *
 *		maxFrameSize [in] - Frame size for the channel. This value must be between
 * 			RF_MIN_FRAME_SIZE and RF_MAX_FRAME_SIZE.
 *
 *		priority [in] - Priority of the channel. Channels with higher priority have smaller
 * 			"priority" values. Generally, data on higher priority channels is
 * 			sent before lower priority channels. The priority must be between
 * 			RF_DEFAULT_PRIORITY (0) and RF_LOWEST_PRIORITY (63).
 *
 *		credit [in] - This parameter is used to specify an initial amount of credit 
 *         	that will be granted to a server upon connection.  The amount of 
 *         	credit that is advanced tells the server how many RFCOMM packets it 
 *         	can send before flow control is shut off from server to client.  
 *         	In order to allow the server to send more data, BTL_RFCOMM_AdvanceCredit 
 *			must be called.  If the remote device does not support credit-based flow 
 *         	control, then the flow control is controlled automatically by the FC 
 *         	bit in the modem status command.
 *
 *		channelId [out] - Identifies the ID that corresponds to this channel.
 *
 * Generated Events:
 *      RFEVENT_OPEN
 *		RFEVENT_CLOSED
 *
 * Returns:
 *		BT_STATUS_PENDING - The request to open the channel was sent.
 *         If the remote device accepts the request, RFCOMM will
 *         generate an RFEVENT_OPEN event. If the channel is rejected, RFCOMM
 *         will generate an RFEVENT_CLOSED event.
 *
 *		BT_STATUS_FAILED - The L2CAP connection is not ready for RFCOMM
 *         data.
 *
 *     	BT_STATUS_NO_RESOURCES - The system has run out of resources
 *         and cannot open the channel.  A channel must be closed before a
 *         new channel can be opened.
 *
 *     	BT_STATUS_RESTRICTED - The channel failed a security check.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_RFCOMM_ConnectClientChannel(BtlRfcommContext *rfcommContext,
										BD_ADDR *bdAddr,
										U8 serviceId,
										U16 maxFrameSize,
										U8 priority,
										U8 credit, 
										BtlRfcommChannelId *channelId);


/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_Disconnect()
 *
 *	Brief:  
 *		Closes a connected channel between two devices.
 *
 *	Description:
 *     	Closes a connected channel between two devices.  When the channel
 *     	is closed, the RFCOMM user will receive an RFEVENT_CLOSED event.
 *
 *     	If outstanding transmit packets exist when a channel is closed,
 *     	they will be returned to the RFCOMM user with RFEVENT_PACKET_HANDLED
 *     	events. The "status" field for these events will be set to
 *     	BT_STATUS_NO_CONNECTION.  All packets are returned to the application
 *     	before the RFEVENT_CLOSED event is received.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *     	rfcommContext [in] - pointer to the RFCOMM context.
 *
 *		channelId [in] - associated channel id.
 *
 * Generated Events:
 *		RFEVENT_CLOSED
 *
 * Returns:
 *		BT_STATUS_PENDING - The request to close the channel will be sent.
 *         The application will receive an RFEVENT_CLOSED event when the
 *         channel is closed.
 *
 *		BT_STATUS_IN_PROGRESS - RFCOMM is already in the process of closing.
 *
 *		BT_STATUS_FAILED - The channel is not in the correct state.
 *
 *     	BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_RFCOMM_Disconnect(BtlRfcommContext *rfcommContext, 
								BtlRfcommChannelId channelId);


/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_GetConnectedDevice()
 *
 *	Brief:  
 *		This function returns the connected device.
 *
 *	Description:
 *		This function returns the connected device.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		rfcommContext [in] - RFCOMM context.
 *
 *		channelId [in] - associated channel id.
 *
 *		bdAddr [out] - pointer to 48-bit address of the connected device.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *		BT_STATUS_NO_CONNECTION -  The channel is not connected.
 */
BtStatus BTL_RFCOMM_GetConnectedDevice(BtlRfcommContext *rfcommContext, 
										BtlRfcommChannelId channelId, 
										BD_ADDR *bdAddr);


/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_SendData()
 *
 *	Brief:  
 *		Sends data on the specified channel.
 *
 *	Description:
 *     	Sends data on the specified channel. When data transmission is
 *     	complete, RFCOMM generates an RFEVENT_PACKET_HANDLED event for the
 *     	channel. The "status" field of the RfCallbackParms structure
 *     	will indicate whether the transmission was successful.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *     	rfcommContext [in] - pointer to the RFCOMM context.
 *
 *		channelId [in] - Identifies the channel upon which the data is sent.
 *
 *		data [in] - Points to a buffer of user data. This pointer is owned by 
 *			RFCOMM until the BtPacket (returned as last argument) is returned 
 *			with the RFEVENT_PACKET_HANDLED event.
 *
 *		dataLen [in] - Indicates the length of "data" in bytes.
 *
 *		packet [out] - The BtPacket used to send the data. This pointer is returned
 *         	with the RFEVENT_PACKET_HANDLED event, and only then the "data"
 *			pointer is freed.
 *
 * Generated Events:
 *		RFEVENT_PACKET_HANDLED
 *
 * Returns:
 *		BT_STATUS_PENDING - The data has been queued. If the data is sent
 *         successfully, an RFEVENT_PACKET_HANDLED event will arrive with
 *         a "status" of BT_STATUS_SUCCESS.  If the transmission fails, the
 *         same event will be received with a status specifying the reason.
 *
 *     	BT_STATUS_FAILED - The channel is not in the correct state.
 *
 *		BT_STATUS_NO_CONNECTION -  The channel is not connected.
 *
 *     	BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_RFCOMM_SendData(BtlRfcommContext *rfcommContext,
							BtlRfcommChannelId channelId,
							U8 *data,
							U16 dataLen,
							BtPacket **packet);


#if RF_SEND_CONTROL == XA_ENABLED

/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_SetModemStatus()
 *
 *	Brief:  
 *		Sends a modem status command to the remote device.
 *
 *	Description:
 *     	Sends a modem status command to the remote device.  This function
 *     	allows the application to send flow control and V.24 signals to the
 *     	remote device.
 *
 * Requires:
 *     	RF_SEND_CONTROL enabled.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *     	rfcommContext [in] - pointer to the RFCOMM context.
 *
 *		channelId [in] - Identifies the connected channel for this action.
 *
 *		modemStatus [in] - The modem status structure.  The "signals" field should
 *         	be initialized with the V.24 bits and/or flow control bit.
 *         	The "breakLen" field should contain a value between 0 and 15,
 *         	signifying the length of a break signal in 200 ms units.
 *         	If "breakLen" is zero, no break signal is sent.
 *
 *         	The FC bit of the modem status is used for flow control of the
 *         	channel.  Applications do not need to set this bit, however,
 *         	because flow control is automatically managed using credits.
 *
 * Generated Events:
 *		RFEVENT_MODEM_STATUS_CNF
 *		RFEVENT_CLOSED
 *
 * Returns:
 *		BT_STATUS_PENDING - The modem status will be sent to the remote
 *         	device.  If the remote device receives the status, the
 *         	channel's callback function will receive an RFEVENT_MODEM_STATUS_CNF
 *         	event. If the operation times out, the callback function will
 *         	receive an RFEVENT_CLOSED event.
 *
 *     	BT_STATUS_FAILED - The channel is not open.
 *
 *		BT_STATUS_IN_PROGRESS -  An RFCOMM control channel request is already 
 *         	outstanding for this channel.  Wait for the status event associated 
 *         	with the request before sending a new request. The functions that 
 *         	make control channel requests are:  BTL_RFCOMM_SetModemStatus, 
 *         	BTL_RFCOMM_SetLineStatus, BTL_RFCOMM_RequestPortSettings, 
 *			BTL_RFCOMM_RequestPortStatus and BTL_RFCOMM_SendTest.
 *
 *     	BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_RFCOMM_SetModemStatus(BtlRfcommContext *rfcommContext,
									BtlRfcommChannelId channelId,
									RfModemStatus *modemStatus);


/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_SetLineStatus()
 *
 *	Brief:  
 *		Sends the line status command to the remote device.
 *
 *	Description:
 *     	Sends the line status command to the remote device.  This function
 *     	allows the RFCOMM user to communicate overrun, framing, and parity
 *     	errors to the remote device.
 *
 * Requires:
 *     	RF_SEND_CONTROL enabled.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *     	rfcommContext [in] - pointer to the RFCOMM context.
 *
 *		channelId [in] - Identifies the connected channel for this action.
 *
 *		lineStatus [in] - The line status bit mask.  This value should be
 *         	initialized with the appropriate line status bits set.
 *
 * Generated Events:
 *		RFEVENT_LINE_STATUS_CNF
 *		RFEVENT_CLOSED
 *
 * Returns:
 *		BT_STATUS_PENDING - The line status will be sent to the remote
 *         	device.  When the remote device receives the status, RFCOMM
 *         	will generate an RFEVENT_LINE_STATUS_CNF event. If the operation
 *         	times out, RFCOMM will generate RFEVENT_CLOSED.
 *
 *     	BT_STATUS_FAILED - The channel is not open.
 *
 *     	BT_STATUS_IN_PROGRESS -  An RFCOMM control channel request is already 
 *         	outstanding for this channel.  Wait for the status event associated 
 *         	with the request before sending a new request. The functions that 
 *         	make control channel requests are:  BTL_RFCOMM_SetModemStatus, 
 *         	BTL_RFCOMM_SetLineStatus, BTL_RFCOMM_RequestPortSettings, 
 *			BTL_RFCOMM_RequestPortStatus and BTL_RFCOMM_SendTest.
 *
 *     	BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_RFCOMM_SetLineStatus(BtlRfcommContext *rfcommContext,
									BtlRfcommChannelId channelId,
									RfLineStatus lineStatus);


/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_RequestPortSettings()
 *
 *	Brief:  
 *		Requests a change to the current port settings.
 *
 *	Description:
 *     	Requests a change to the current port settings. The caller may set
 *     	only a subset of the fields in the "portSettings" parameter. The
 *     	"parmMask" field of the structure identifies which fields are
 *     	important.
 *
 *     	After a successful call to this function, RFCOMM exchanges
 *     	the parameters with the remote device on the channel. After the
 *     	remote device responds, RFCOMM generates an RFEVENT_PORT_NEG_CNF event
 *     	to indicate which settings were accepted or rejected. Bits in
 *     	the "parmMask" field indicate whether the parameter was accepted
 *     	(bit set) or rejected (bit clear).
 *
 *     	If the remote device rejects some of the parameters, subsequent
 *     	requests can be sent with modified parameters until a final
 *     	agreement is reached.
 *
 * Requires:
 *     	RF_SEND_CONTROL enabled.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *     	rfcommContext [in] - pointer to the RFCOMM context.
 *
 *		channelId [in] - Identifies the connected channel for this action.
 *
 *		portSettings [in] - A pointer to an RfPortSettings structure, initialized
 *         	with the desired port settings.
 *
 * Generated Events:
 *		RFEVENT_PORT_NEG_CNF
 *		RFEVENT_CLOSED
 *
 * Returns:
 *		BT_STATUS_PENDING - The port negotiation will be sent to the
 *         	remote device.  If the remote device responds to the request,
 *         	RFCOMM sends an RFEVENT_PORT_NEG_CNF event. If the operation times
 *         	out, RFCOMM sends an RFEVENT_CLOSED event.
 *
 *     	BT_STATUS_FAILED - The channel is not open.
 *
 *     	BT_STATUS_IN_PROGRESS -  An RFCOMM control channel request is already 
 *         	outstanding for this channel.  Wait for the status event associated 
 *         	with the request before sending a new request. The functions that 
 *         	make control channel requests are:  BTL_RFCOMM_SetModemStatus, 
 *         	BTL_RFCOMM_SetLineStatus, BTL_RFCOMM_RequestPortSettings, 
 *			BTL_RFCOMM_RequestPortStatus and BTL_RFCOMM_SendTest.
 *
 *     	BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_RFCOMM_RequestPortSettings(BtlRfcommContext *rfcommContext,
										BtlRfcommChannelId channelId,
										RfPortSettings *portSettings);


/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_RequestPortStatus()
 *
 *	Brief:  
 *		Requests the status of the port settings for the remote device.
 *
 *	Description:
 *     	Requests the status of the port settings for the remote device.
 *
 *     	After a successful call to this function, the remote device responds
 *     	with its current port settings. 
 *
 * Requires:
 *     	RF_SEND_CONTROL enabled.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *     	rfcommContext [in] - pointer to the RFCOMM context.
 *
 *		channelId [in] - Identifies the connected channel for this action.
 *
 * Generated Events:
 *		RFEVENT_PORT_STATUS_CNF
 *		RFEVENT_CLOSED
 *
 * Returns:
 *		BT_STATUS_PENDING - The request will be sent to the remote device.  
 *         	If the remote device responds to the request, RFCOMM sends an 
 *         	RFEVENT_PORT_STATUS_CNF event. If the operation times out, RFCOMM 
 *         	sends an RFEVENT_CLOSED event.  
 *
 *     	BT_STATUS_FAILED - The channel is not open.
 *
 *     	BT_STATUS_IN_PROGRESS -  An RFCOMM control channel request is already 
 *         	outstanding for this channel.  Wait for the status event associated 
 *         	with the request before sending a new request. The functions that 
 *         	make control channel requests are:  BTL_RFCOMM_SetModemStatus, 
 *         	BTL_RFCOMM_SetLineStatus, BTL_RFCOMM_RequestPortSettings, 
 *			BTL_RFCOMM_RequestPortStatus and BTL_RFCOMM_SendTest.
 *
 *     	BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_RFCOMM_RequestPortStatus(BtlRfcommContext *rfcommContext,
										BtlRfcommChannelId channelId);


/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_AcceptPortSettings()
 *
 *	Brief:  
 *		Accepts or rejects the port settings received during an
 *     	RFEVENT_PORT_NEG_IND event.
 *
 *	Description:
 *     	Accepts or rejects the port settings received during an
 *     	RFEVENT_PORT_NEG_IND event.
 *
 *     	This function must be called during the RFEVENT_PORT_NEG_IND callback,
 *     	unless processing of the Port Negotiation has been delayed by calling
 *     	BTL_RFCOMM_DelayPortRsp.  If called during the callback, the 
 *     	RfPortSettings structure provided in the "ptrs.portSettings" field can 
 *     	be modified and used for the "PortSettings" parameter.  Only the 
 *     	"parmMask" field of the RfPortSettings structure should be modified to 
 *     	indicate whether a parameter is accepted or rejected.
 *
 *     	Processing of the Port Negotiation request can be delayed by calling 
 *     	BTL_RFCOMM_DelayPortRsp.  This allows the application to accept the
 *     	port settings outside the callback context.  If processing is
 *     	delayed, it is important for the application to respond within 10 seconds,
 *     	otherwise, a link disconnect may result.  The application will also
 *     	be required to allocate an RfPortSettings structure, copy the port
 *     	settings from the "ptrs.portSettings" field receive during the
 *     	RFEVENT_PORT_NEG_IND event into that data structure, set the response
 *     	mask to the appropriate value, and provide a pointer to this structure 
 *     	in the "PortSettings" parameter.
 *
 *     	If neither this function nor BTL_RFCOMM_DelayPortRsp are called during the
 *     	callback, the stack will automatically respond to the 
 *     	RFEVENT_PORT_NEG_IND by accepting the requested parameters.
 *
 * Requires:
 *     	RF_SEND_CONTROL enabled.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *     	rfcommContext [in] - pointer to the RFCOMM context.
 *
 *		channelId [in] - Identifies the connected channel for this action.
 *
 *		portSettings [in] - Contains port settings received in the RFEVENT_PORT_NEG_IND
 *         event.  The "parmMask" field should be set with bits that indicate 
 *         which parameters are valid.  To accept a parameter, set its bit
 *         to 1. To reject a parameter, set its bit to 0.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - The port settings have been accepted or rejected
 *         	as indicated, and an indication will be sent to the remote
 *         	device.
 *
 *     	BT_STATUS_FAILED - The channel is not in the correct state or a request
 *         	is already outstanding.
 *
 *     	BT_STATUS_NO_RESOURCES - There are no available resources for sending
 *         	this response.  When resources are free, an RFEVENT_RESOURCE_FREE
 *         	event will be sent to the application callback function.
 *
 *     	BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_RFCOMM_AcceptPortSettings(BtlRfcommContext *rfcommContext,
										BtlRfcommChannelId channelId,
										RfPortSettings *portSettings);


/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_SendPortStatus()
 *
 *	Brief:  
 *		Sends the current port settings (specified by "portSettings") when they
 *     	have been requested by the RFEVENT_PORT_STATUS_IND event. 
 *
 *	Description:
 *     	Sends the current port settings (specified by "portSettings") when they
 *     	have been requested by the RFEVENT_PORT_STATUS_IND event. 
 *
 *     	This function must be called during the RFEVENT_PORT_NEG_IND callback,
 *     	unless processing of the Port Status request has been delayed by calling
 *     	BTL_RFCOMM_DelayPortRsp. If called during the callback, the 
 *     	RfPortSettings structure provided in the "ptrs.portSettings" field can 
 *     	be modified and used for the "PortSettings" parameter.  Each field should
 *     	be set to the current state of each port parameter.
 *
 *     	Processing of the Port Status request can be delayed by calling 
 *     	BTL_RFCOMM_DelayPortRsp.  This allows the application to report the port status 
 *     	outside the callback context.  If processing is delayed, it is 
 *     	important for the application to respond within 10 seconds, otherwise, a 
 *     	link disconnect may result.  Also, the application is required to 
 *     	allocate an RfPortSettings structure, copy the port status into that 
 *     	structure, and provide a pointer to this structure in the "PortSettings" 
 *     	parameter.
 *
 *     	If neither this function nor BTL_RFCOMM_DelayPortRsp are called during the
 *     	callback, the stack will automatically respond to the 
 *     	RFEVENT_PORT_STATUS_IND by sending the default Serial Port Profile
 *     	port settings of:  9600 baud, 8 data bits, 1 stop bit, no parity.
 *
 * Requires:
 *     	RF_SEND_CONTROL enabled.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *     	rfcommContext [in] - pointer to the RFCOMM context.
 *
 *		channelId [in] - Identifies the connected channel for this action.
 *
 *		portSettings [in] - Contains the current port settings maintained by the 
 *         application or port entity.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - The port settings have been accepted or rejected
 *         	as indicated.
 *
 *     	BT_STATUS_FAILED - The channel is not in the correct state or a request
 *         	is already outstanding.
 *
 *     	BT_STATUS_NO_RESOURCES - There are no available resources for sending
 *         	this response.  When resources are free, an RFEVENT_RESOURCE_FREE
 *         	event will be sent to the application callback function.
 *
 *     	BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_RFCOMM_SendPortStatus(BtlRfcommContext *rfcommContext,
									BtlRfcommChannelId channelId,
									RfPortSettings *portSettings);


/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_DelayPortRsp()
 *
 *	Brief:  
 *		Delays the processing of an RFEVENT_PORT_NEG_IND or an
 *     	RFEVENT_PORT_STATUS_IND event so that a response may be sent outside
 *     	the context of the callback.
 *
 *	Description:
 *     	Delays the processing of an RFEVENT_PORT_NEG_IND or an
 *     	RFEVENT_PORT_STATUS_IND event so that a response may be sent outside
 *     	the context of the callback.
 * 
 *     	This function must be called during the RFEVENT_PORT_NEG_IND or 
 *     	RFEVENT_PORT_STATUS_IND callbacks.  When this function is called during 
 *     	the callback, no response will be sent to the remote device until
 *     	the application calls RF_AcceptPortSettings or RF_SendPortStatus.
 *     	It is important for the application to respond within 10 seconds,
 *     	otherwise, a link disconnect may result.
 *
 * Requires:
 *     	RF_SEND_CONTROL enabled.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *     	rfcommContext [in] - pointer to the RFCOMM context.
 *
 *		channelId [in] - Identifies the connected channel for this action.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - The port response will be delayed.
 *
 *     	BT_STATUS_FAILED - The channel is not in the correct state.
 *
 *     	BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_RFCOMM_DelayPortRsp(BtlRfcommContext *rfcommContext,
									BtlRfcommChannelId channelId);

#endif /* RF_SEND_CONTROL == XA_ENABLED */


#if RF_SEND_TEST == XA_ENABLED

/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_SendTest()
 *
 *	Brief:  
 *		Sends a TEST frame.
 *
 *	Description:
 *     	Sends a TEST frame. When the test result is received, RFCOMM generates
 *     	an RFEVENT_TEST_CNF event. During the event, the "status" field of the
 *     	RfCallbackParms structure contains the result of the test.
 *
 *     	If the status is set to BT_STATUS_SUCCESS, the test was successful
 *     	(a valid test response was received). If the status is
 *     	BT_STATUS_FAILED, the data transmitted did not match the received
 *     	data.
 *
 * Requires:
 *     	RF_SEND_TEST enabled.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *     	rfcommContext [in] - pointer to the RFCOMM context.
 *
 *		channelId [in] - Identifies the channel for this action.
 *
 * Generated Events:
 *      RFEVENT_TEST_CNF
 *      AVRCP_EVENT_DISCONNECT
 *
 * Returns:
 *		BT_STATUS_PENDING - The test operation was started successfully. The
 *         	result will be returned in the RFEVENT_TEST_CNF event. If the
 *         	connection times out before the response is detected, RFEVENT_CLOSED
 *         	is indicated instead.
 *
 *     	BT_STATUS_FAILED - The channel is not open.
 *
 *     	BT_STATUS_IN_PROGRESS -  An RFCOMM control channel request is already 
 *         	outstanding for this channel.  Wait for the status event associated 
 *         	with the request before sending a new request. The functions that 
 *         	make control channel requests are:  BTL_RFCOMM_SetModemStatus, 
 *        	BTL_RFCOMM_SetLineStatus, BTL_RFCOMM_RequestPortSettings, 
 *			BTL_RFCOMM_RequestPortStatus and BTL_RFCOMM_SendTest.
 *
 *     	BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_RFCOMM_SendTest(BtlRfcommContext *rfcommContext,
							BtlRfcommChannelId channelId);

#endif /* RF_SEND_TEST == XA_ENABLED */


/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_GetFrameSize()
 *
 *	Brief:  
 *		Returns the maximum size allowed for a transmit or receive data.
 *
 *	Description:
 *     	Returns the maximum size allowed for a transmit or receive data.  The 
 *     	value returned is only valid for an active channel. The maximum size 
 *     	will vary for each channel and is based on negotiated channel parameters.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *     	rfcommContext [in] - pointer to the RFCOMM context.
 *
 *		channelId [in] - Identifies the channel for this action.
 *
 *		frameSize [out] - The maximum number of bytes allowed for a transmit packet.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_NO_CONNECTION - no RFCOMM connection has been established yet.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_RFCOMM_GetFrameSize(BtlRfcommContext *rfcommContext,
								BtlRfcommChannelId channelId,
								U16 *frameSize);


/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_AdvanceCredit()
 *
 *	Brief:  
 *		Advances transmit credit to the remote device.
 *
 *	Description:
 *     	Advances transmit credit to the remote device.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *     	rfcommContext [in] - pointer to the RFCOMM context.
 *
 *		channelId [in] - Identifies the channel for this action. The channel must
 *         	be open for credit to be advanced.
 *
 *		credit [in] - Each credit value represents one RFCOMM packet.  Advancing n
 *         	credits allows the remote device to send n packets.  Once those
 *         	packets have been sent, the remote device can no longer send (flow
 *         	is off).  Subsequent calls to BTL_RFCOMM_AdvanceCredit will allow the
 *         	remote device to send again (flow is on).  Credits are additive, 
 *         	so calling this function once with 3 credits and then with 2 credits 
 *         	will grant a total of 5 credits to the remote device, allowing the
 *         	remote device to send 5 RFCOMM packets.
 *
 *         	It is important to know that there are two types of flow control 
 *         	at the RFCOMM protocol layer.  Newer devices use "credit" based 
 *         	flow control, and older devices use the FC bit in the modem status 
 *         	command to control flow.  RFCOMM will always try to negotiate
 *         	credit-based flow control with a remote device during the connection
 *         	process. If that negotiation is successful, then RFCOMM provides
 *         	a completely reliable link.  If the negotiation is not successful,
 *         	the link is automatically managed by RFCOMM using MSC commands, 
 *         	based on the credits granted by the application.  It is never 
 *         	necessary for the application to send an MSC command to control
 *         	the flow of data.
 *
 *         	Since RFCOMM is running over packet-based protocols, it may not 
 *         	flow the link off with great precision when forced to use MSC
 *         	commands.  In this case, RFCOMM's flow control is unreliable.  
 *         	It is possible for an application to receive more data packets
 *         	than the credits advanced.
 * 
 *         	Because of this unreliability, some buffer overflow will need to 
 *         	be allocated for data that is received after the FC bit is sent. 
 *         	The amount of buffer space to allocate is difficult to calculate, 
 *         	so some experimentation may be required.  An application, if possible, 
 *         	should use some higher layer flow control mechanism if greater 
 *         	precision is required.
 *
 *         	Only Bluetooth 1.0B devices (and earlier) will not negotiate
 *         	credit flow control.  These devices are fairly scarce.  All 
 *         	Bluetooth 1.1 compliant devices must support credit flow control.
 *
 *         	Call BTL_RFCOMM_IsCreditFlowEnabled to determine what type of flow 
 *			control	has been negotiated.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Credit was advanced successfully.
 *
 *		BT_STATUS_FAILED - Credit could not be advanced.  Either the maximum
 *          amount of credit was exceeded, or credit flow control is not
 *          enabled.
 *
 *      BT_STATUS_NO_CONNECTION - no RFCOMM connection has been established 
 *          yet.  A connection must exist to advance credit.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_RFCOMM_AdvanceCredit(BtlRfcommContext *rfcommContext,
								BtlRfcommChannelId channelId,
								U8 credit);


/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_IsCreditFlowEnabled()
 *
 *	Brief:  
 *		Identifies whether credit based flow control has been negotiated 
 *     	for the current RFCOMM session or not.
 *
 *	Description:
 *     	Identifies whether credit based flow control has been negotiated 
 *     	for the current RFCOMM session or not. Credit flow control is
 *     	negotiated in RFCOMM. If credit flow control is enabled locally and
 *     	the remote device is capable of credit flow control, then this function 
 *     	will return TRUE. It must be called only after the channel is open
 *     	(RFEVENT_OPEN is received).
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *     	rfcommContext [in] - pointer to the RFCOMM context.
 *
 *		channelId [in] - Identifies the channel for this action.
 *
 *		flowEnabled [out] - credit flow state.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_NO_CONNECTION - no RFCOMM connection has been established yet.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_RFCOMM_IsCreditFlowEnabled(BtlRfcommContext *rfcommContext,
								  		BtlRfcommChannelId channelId,
								  		BOOL *flowEnabled);


#if BT_SECURITY == XA_ENABLED

/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_SetSecurityLevel()
 *
 *	Brief:  
 *		Sets security level for the given RFCOMM context.
 *
 *	Description:
 *     	Sets security level for the given RFCOMM context.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *     	rfcommContext [in] - pointer to the RFCOMM context.
 *
 *		securityLevel [in] - level of security which should be applied,	when 
 *			a connection to a remote device is established. 
 *			For default value, pass null.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_RFCOMM_SetSecurityLevel(BtlRfcommContext *rfcommContext,
								  	const BtSecurityLevel *securityLevel);


/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_GetSecurityLevel()
 *
 *	Brief:  
 *		Gets security level for the given RFCOMM context.
 *
 *	Description:
 *     	Gets security level for the given RFCOMM context.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *     	rfcommContext [in] - pointer to the RFCOMM context.
 *
 *		securityLevel [out] - level of security which should be applied, when 
 *			a connection to a remote device is established.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_RFCOMM_GetSecurityLevel(BtlRfcommContext *rfcommContext,
								  	BtSecurityLevel *securityLevel);

#endif	/* BT_SECURITY == XA_ENABLED */


#endif /* __BTL_RFCOMM_H */



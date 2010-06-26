/*******************************************************************************\
##                                                                           	*
##  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION			*
##																				*
##  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE			*
##	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE		*
##	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO		*
##	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT		*
##	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL			*
##	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC          *
##																				*
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:       btl_l2cap.h
*
*   BRIEF:          This file defines the API of the BTL L2CAP protocol layer.
*                   It does support only the L2CAP functionality as defined
*                   in the Bluetooth specification 1.1, excluding group calls
*                   (connectionless communication).
*
*   DESCRIPTION:    General
*
*                   The BTL_L2CAP supports both a client role (initiating 
*                   a connection) and a server role (accepting a connection).
*                   Although L2CAP does not define these roles, it can be
*                   useful to distinguish these 2 roles fora better
*                   understanding of the API.
*
*                   With these 2 roles in mind, the API functions can be divided
*                   into 4 sections:
*
*                   1. Management functions:
*                   These are the functions that can be found in all/most of
*                   the BTL modules as a consistent API:
*                   - BTL_L2CAP_Init
*                   - BTL_L2CAP_Deinit
*                   - BTL_L2CAP_Create
*                   - BTL_L2CAP_Destroy
*                   - BTL_L2CAP_GetConnectedDevice
*                   - BTL_L2CAP_SetSecurityLevel
*                   - BTL_L2CAP_GetSecurityLevel
*                   The remaining functions are more specific for L2CAP
*
*                   2. Server functions:
*                   The server specific functions are:
*                   - BTL_L2CAP_RegisterPsm
*                   - BTL_L2CAP_DeregisterPsm
*                   - BTL_L2CAP_ConnectResponse
*         
*                   3. Client functions:
*                   The client specific functions are:
*                   - BTL_L2CAP_Connect
*
*                   4. Common functions:
*                   The remaining functions are used by both client and server
*                   type of roles.
*
*                   Limitations:
*                   This version of BTL_L2CAP does support only the basic
*                   functionality of L2CAP. This means that the following
*                   functionality is not supported:
*                   - CLT (Connectionless Traffic),
*                   - BT 1.2 functionality like Flow control.
*
*                   Restrictions:
*                   Part of the API is prepared for further extension which
*                   might give some confusion. These future extensions are
*                   embedded in compiler directives. This means that some compiler
*                   directives must be fixed as stated in the list below:
*                   - L2CAP_FLOW_CONTROL XA_DISABLED:
*                     This BT 1.2 feature is not available yet.
*                   - L2CAP_FLEXIBLE_CONFIG XA_ENABLED
*                     This version only supports the automatic configuration of
*                     an L2CAP channel. So BTL_L2CAP will take care of the
*                     configuration process.
*                   
*   AUTHOR:         Gerrit Slot
*
\*******************************************************************************/

#ifndef __BTL_L2CAP_H
#define __BTL_L2CAP_H


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include <l2cap.h>
#include <sdp.h>
#include <sec.h>
#include <btl_common.h>

/********************************************************************************
 *
 * Macros
 *
 *******************************************************************************/

/* My own defined events */
#define L2CAP_EVENT_DISABLED ((U8) BTL_L2CAP_LAST_EVENT + 1)

/* L2CAP connection establishment failed */
#define L2EVENT_CONNECTION_FAILURE      ((U8) BTL_L2CAP_LAST_EVENT + 2)

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/* Forward declarations */
typedef struct _BtlL2capEvent           BtlL2capEvent;
typedef struct _BtlL2capContext         BtlL2capContext;

#if L2CAP_FLOW_CONTROL == XA_ENABLED
/*---------------------------------------------------------------------------
 * BtlFlowChannelSettings
 *
 *    Flow control settings for an L2CAP connection (channel).
 *    It contains selected parameters from the 'L2capPsm' structure
 *    from the ESI stack.
 */
typedef struct _BtlFlowChannelSettings
{
    /* 
     * Requested operating modes for channels opened for this PSM. For
     * maximum compatibility, this should include L2MODE_BASIC. If basic is
     * not included, connections to Bluetooth v1.1 devices will fail. An
     * L2CAP Information request may be issued to retrieve the extended
     * features mask to ascertain the remote devices capabilities. If the
     * PSM does not permit use the flow and error control extensions added
     * in Bluetooth v1.2, set this parameter to L2MODE_BASIC.
     *
     * The inbound and outbound link modes can be independently configured.
     * So, for example inbound packets can use retransmission mode while
     * outbound data uses only flow control. However, if the link is
     * configured to use basic mode in one direction, it must use basic
     * mode in the other direction also.
     */

    /* Specify the acceptable link modes for incoming data. */
    L2capLinkMode   inLinkMode;

    /* Specify the acceptable link modes for outgoing data. */
    L2capLinkMode   outLinkMode;

    /*
     *The maximum number of times the remote L2CAP can transmit a packet
     * before considering the channel lost. The minimum value of 1, indicates
     * no retransmissions shall be sent. Note that if the retransmit timer
     * expires and the transmitMax value has been reached, the channel will
     * be disconnected. This value is not used by basic mode connections.
     */
    U8              inTransmitMax;

    /*
     * The retransmission timeout to be used by the remote device when 
     * retransmission mode is enabled. This timeout should be set fairly
     * high in order to avoid unnecessary retransmissions due to delayed
     * acknowledgements. Suitable values, specified in milliseconds, could
     * be 100’s of milliseconds or higher. This value is not used by basic
     * mode connections.
     */
    U16             inRetransTimeout;

    /*
     * The monitor timeout to be used by the remote device when no activity
     * is present on the channel. It serves to ensure that lost acknowledgements
     * are retransmitted. Suitable values, specified in milliseconds, could
     * be 100’s of milliseconds or higher. This value is not used by basic
     * mode connections.
     */
    U16             inMonitorTimeout;

    /*
     * Number of bytes that can be received from the remote device before
     * an acknowledgement must be sent. This value represents the buffering
     * capability internal to the L2CAP User. The minimum value is Psm.localMtu.
     */
    U32             inBufferSize;
} BtlFlowChannelSettings;
#endif /* L2CAP_FLOW_CONTROL == XA_ENABLED */

/*---------------------------------------------------------------------------
 * BtlL2capChannelSettings
 *
 *    Settings for an L2CAP connection (channel)
 *    It contains selected parameters from the 'L2capPsm' structure
 *    from the ESI stack.
 *    The following parameters specify the connection settings used
     * to configure channels opened on this PSM. L2CAP will automatically
     * negotiate the connection configuration with the remote peer. Channels
     * that require control over their configuration process should use
     * the L2CAP_ConfigReq and L2CAP_ConfigRsp functions to execute their own
 *    configuration process (enable L2CAP_FLEXIBLE_CONFIG).
 *    For the configuration parameters that are not defined here, the
 *    default parameters as defined in the L2CAP specification are used.
     */
typedef struct _BtlL2capChannelSettings
{
    /*
     * Maximum receive MTU for this service. Should be equal to or less 
     * than L2CAP_MAXIMUM_MTU. 
     */
    U16  localMtu;

    /*
     * Minimum acceptable value for remote device's receive MTU size. Aka, 
     * local transmit MTU. Should be no less than L2CAP_MINIMUM_MTU. 
     */
    U16  minRemoteMtu;

#if L2CAP_FLOW_CONTROL == XA_ENABLED
    BtlFlowChannelSettings flowControl;
#endif /* L2CAP_FLOW_CONTROL == XA_ENABLED */
} BtlL2capChannelSettings;

/*---------------------------------------------------------------------------
 * BtlL2capChannelConfig
 *
 *    Configuration for an L2CAP connection (channel)
 */
typedef struct _BtlL2capChannelConfig
{
    /* MTU for transmission (remote MTU)*/
    U16 txMtu;
} BtlL2capChannelConfig;

/*---------------------------------------------------------------------------
 * BTLL2capChannelId
 *
 *    Identification of a connection. It is not the same as the L2CAP CID,
 *    but has a 1:1 relation to it.
 */
typedef U16 BtlL2capChannelId;

/* Non existing channelId. E.g. can be used to mark an admin as being 'not used'.*/
#define BTL_L2CAP_ILLEGAL_CHANNEL_ID ((BtlL2capChannelId) 0xFFFF)

/*-------------------------------------------------------------------------------
 * BtlL2capCallBack type
 *
 *    A function of this type is called to indicate BTL L2CAP events.
 */
typedef void (*BtlL2capCallBack)(const BtlL2capEvent *event);    

/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BtlL2capEvent structure
 *
 *    Represents BTL L2CAP event.
 */
struct _BtlL2capEvent 
{
	  /* Associated context */
    BtlL2capContext     *l2capContext;
    /* Unique connection ID. */
    BtlL2capChannelId   channelId;
    /* Event data */
    L2capCallbackParms  *l2capEvent;
};

/********************************************************************************
 *
 * Function declarations
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BTL_L2CAP_Init()
 *
 * Brief:  
 *    Init the L2CAP module.
 *
 * Description:
 *    Init the L2CAP module.
 *
 * Type:
 *    Synchronous
 *
 * Parameters:
 *    void.
 *
 * Returns:
 *    BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 *		
 *		BT_STATUS_INTERNAL_ERROR - Internal error has occur.
 */
BtStatus BTL_L2CAP_Init(void);


/*-------------------------------------------------------------------------------
 * BTL_L2CAP_Deinit()
 *
 * Brief:  
 *    Deinit the L2CAP module.
 *
 * Description:
 *    Deinit the L2CAP module.
 *
 * Type:
 *    Synchronous
 *
 * Parameters:
 *    void.
 *
 * Returns:
 *    BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 *		
 *		BT_STATUS_INTERNAL_ERROR - Internal error has occur.
 */
BtStatus BTL_L2CAP_Deinit(void);

 
/*-------------------------------------------------------------------------------
 * BTL_L2CAP_Create()
 *
 * Brief:  
 *    Allocates a unique L2CAP context.
 *
 * Description:
 *    Allocates a unique L2CAP context.
 *    This function must be called before any other L2CAP API function.
 *    The allocated context should be supplied in subsequent L2CAP API calls.
 *    The caller must also provide a callback function, which will be called 
 *    on L2CAP events.
 *    This function is necessary as a start for both server type and client type
 *    of applications.
 *
 *    The caller can provide an application handle (previously allocated 
 *    with BTL_RegisterApp), in order to link between different modules.
 *    If there is no need to link between different modules, set appHandle to 
 *    NULL.
 *
 * Type:
 *    Synchronous
 *
 * Parameters:
 *    appHandle [in] - application handle, can be NULL.
 *
 *    l2capCallback [in] - all L2CAP events will be sent to this callback.
 *
 *    securityLevel [in] - level of security which should be applied, when
 *        connecting to a remote L2CAP channel.
 *        If NULL is passed, default value (BSL_DEFAULT) is used.
 *
 *		service [in] - service which is on top of this L2CAP channel
 *			  (used later when BTEVENT_AUTHORIZATION_REQ is generated 
 *			  to identify the service which caused the event).
 *
 *    l2capContext [out] - allocated L2CAP context.	
 *
 * Returns:
 *    BT_STATUS_SUCCESS - L2CAP context was created successfully.
 *
 *		BT_STATUS_FAILED -  Failed to create L2CAP context.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *    BT_STATUS_NO_RESOURCES - No more resources.
 *
 *		BT_STATUS_INTERNAL_ERROR - Internal error has occur.
 *
 */
BtStatus BTL_L2CAP_Create(BtlAppHandle *appHandle,
                          const BtlL2capCallBack l2capCallback,
                          const BtSecurityLevel *securityLevel,
                          const SdpServicesMask service,
                          BtlL2capContext **l2capContext);


/*-------------------------------------------------------------------------------
 * BTL_L2CAP_Destroy()
 *
 * Brief:  
 *		Releases an L2CAP context (previously allocated with BTL_L2CAP_Create).
 *
 * Description:
 *    Releases an L2CAP context (previously allocated with BTL_L2CAP_Create).
 *    An application should call this function when it completes using L2CAP 
 *    services.
 *    Upon completion, the L2CAP context is set to NULL in order to prevent 
 *    the application from an illegal attempt to keep using it.
 *
 * Type:
 *    Synchronous
 *
 * Parameters:
 *    l2capContext [in/out] - Pointer to the L2CAP context.
 *
 * Returns:
 *    BT_STATUS_SUCCESS - L2CAP context was destroyed successfully.
 *
 *		BT_STATUS_IN_USE -  L2CAP context is in use.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *		BT_STATUS_INTERNAL_ERROR - Internal error has occur.
 *
 */
BtStatus BTL_L2CAP_Destroy(BtlL2capContext **l2capContext);

#if BT_SECURITY == XA_ENABLED
/*-------------------------------------------------------------------------------
 * BTL_L2CAP_SetSecurityLevel()
 *
 * Brief:  
 *    Sets security level for the given L2CAP context.
 *
 * Description:
 *    Sets security level for the given L2CAP context.
 *
 * Type:
 *    Synchronous
 *
 * Parameters:
 *    l2capContext [in] - pointer to the L2CAP context.
 *
 *    securityLevel [in] - level of security which should be applied,	when 
 *        a connection to a remote device is established. 
 *        For default value, pass NULL.
 *
 * Returns:
 *    BT_STATUS_SUCCESS - Operation is successful.
 *
 *    BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_L2CAP_SetSecurityLevel(BtlL2capContext *l2capContext,
			                					  	const BtSecurityLevel *securityLevel);


/*-------------------------------------------------------------------------------
 * BTL_L2CAP_GetSecurityLevel()
 *
 * Brief:  
 *    Gets security level for the given L2CAP context.
 *
 * Description:
 *    Gets security level for the given L2CAP context.
 *
 * Type:
 *    Synchronous
 *
 * Parameters:
 *    l2capContext [in] - pointer to the L2CAP context.
 *
 *    securityLevel [out] - level of security which should be applied, when 
 *        a connection to a remote device is established.
 *
 * Returns:
 *    BT_STATUS_SUCCESS - Operation is successful.
 *
 *    BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_L2CAP_GetSecurityLevel(BtlL2capContext *l2capContext,
			                					  	BtSecurityLevel *securityLevel);

#endif /* BT_SECURITY == XA_ENABLED */

/*-------------------------------------------------------------------------------
 * BTL_L2CAP_GetConnectedDevice()
 *
 * Brief:
 *    This function returns the connected device.
 *      
 * Description:
 *    This function returns the connected device.
 *
 * Type:
 *    Synchronous
 *
 * Parameters:
 *    l2capContext [in] - Pointer to the L2CAP context.
 *
 *    channelId [in] - Identification of the L2CAP channel
 *
 *    bdAddr [out] - pointer to 48-bit address of the connected device.
 *
 * Returns:
 *    BT_STATUS_SUCCESS - Operation is successful.
 *
 *    BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *    BT_STATUS_NO_CONNECTION -  The channel is not connected.
 */
BtStatus BTL_L2CAP_GetConnectedDevice(BtlL2capContext *l2capContext, 
                                      BtlL2capChannelId   channelId,
			          							        BD_ADDR *bdAddr);

/*---------------------------------------------------------------------------
 * BTL_L2CAP_RegisterPsm()
 *
 * Brief:
 *    Registers a protocol ('psm') to L2CAP.
 *      
 * Description:
 *    Registers a protocol ('psm') to L2CAP. The 'channelConfig' contains
 *    the channel configuration that will be used during the establishment
 *    of an incoming connection.
 *    Once registered, the protocol can receive connection requests
 *    from remote clients (L2EVENT_CONNECT_IND event).
 *
 * Type:
 *    Synchronous
 *
 * Parameters:
 *    l2capContext [in] - Pointer to the L2CAP context.
 *
 *    channelSettings [in] - Configuration of the channel to be negotiated during
 *        the L2CAP channel configuration process, when a remote device does
 *        set-up a connection with this 'psm'.
 *
 *    psm [in/out] - PSM of this protocol.
 *        When set to BT_DYNAMIC_PSM, the value will be changed at function
 *        return to a random PSM value in the range as allocated for the
 *        dynamic PSMs by the L2CAP specification.
 *
 * Returns:
 *    BT_STATUS_SUCCESS - PSM was successfully registered with L2CAP.
 *
 *    BT_STATUS_NO_RESOURCES - Maximum number of registered PSMs reached.
 *
 *    BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_L2CAP_RegisterPsm(BtlL2capContext *l2capContext,
                           BtlL2capChannelSettings *channelSettings,
                           L2capPsmValue *psm);

#if L2CAP_DEREGISTER_FUNC == XA_ENABLED
/*---------------------------------------------------------------------------
 * BTL_L2CAP_DeregisterPsm()
 *
 * Brief:
 *    Removes a registered PSM (server) from the L2CAP protocol.
 *      
 * Description:
 *    Removes a registered PSM (server) from the L2CAP protocol.
 *    This server is identified by 'l2capContext'.
 *    If there are still one or more connections established for this
 *    PSM, they will be disconnected first automatically.
 *
 * Type:
 *    Synchronous/Asynchronous
 *
 * Requires:
 *    L2CAP_DEREGISTER_FUNC enabled.
 *
 * Parameters:
 *    l2capContext [in] - Pointer to the L2CAP context.
 *
 * Returns:
 *    BT_STATUS_SUCCESS - The protocol has been deregistered.
 *
 *    BT_STATUS_NOT_FOUND - The protocol was not previously registered.
 * 
 *		BT_STATUS_PENDING - There are 1 or more connections related to this
 *      'l2capContext'
 *      The disconnect process has been successfully started for all
 *      connections related to this 'l2capContext' When a link is
 *      disconnected, the application will receive the disconnect event
 *      (L2EVENT_DISCONNECTED) for each disconnected channel.
 *      When the disconnect process is complete, the L2CAP_EVENT_DISABLED will
 *      be sent to the application.
 */
BtStatus BTL_L2CAP_DeregisterPsm(BtlL2capContext *l2capContext);

#endif /* L2CAP_DEREGISTER_FUNC == XA_ENABLED */

/*---------------------------------------------------------------------------
 * BTL_L2CAP_Connect()
 *
 * Brief:
 *    Connects to a PSM service on the remote device.
 *      
 * Description:
 *    Request the establishment of an L2CAP channel to the specified 
 *    protocol service ('remotePsm') on the specified remote
 *    device ('bdAddr').
 *    If there was no ACL connection yet to the remote device, it will
 *    be established first.
 *    Via the 'settings' some channel parameters like 'automatic
 *    configuration' are under control.
 *    If this function returns BT_STATUS_PENDING, the request was successfully 
 *    initiated, and 'channelId' does contain a unique ID for this connection.
 *    All connection oriented functions and events do contains this 'channelId'.
 *    Connection status events will be sent to the callback
 *    function belonging to this 'l2capContext'.
 *
 *    By default the configuration is set to the auto mode
 *    (L2CAP_FLEXIBLE_CONFIG), and this module will automatically
 *    manage/negotiate the channel configuration with the remote side.
 *               
 * Type:
 *    Asynchronous.
 *
 * Parameters:
 *    l2capContext [in] - L2CAP context as created (BTL_L2CAP_Create) before.
 *
 *    bdAddr [in] - 48 bit BD-address of the remote device.
 *
 *    remotePsm [in] - PSM value of protocol to connect to on the remote device. 
 *
 *    connectSettings [in] - Some connection specific settings. E.g. it tells if
 *        L2CAP should take care of the configuration process (see next parameter)
 *        or not. Details on the individual parameters are defined in the 
 *        structure definition.
 *        This parameter is for future use and is ignored now because of the fixed
 *        compiler directive settings for L2CAP_FLOW_CONTROL and
 *        L2CAP_FLEXIBLE_CONFIG as described before in the restrictions.
 *        It is allowed to use NULL for this parameter.
 *
 *    channelSettings [in] - Configuration of the channel to be negotiated during
 *        the L2CAP channel configuration process.
 *    
 *    channelId [out] - Channel identifier. If this function 
 *        returns BT_STATUS_PENDING, this parameter is filled with a 
 *        channel ID assigned to this connection. This ID is not the same as
 *        the L2CAP CID, but it has a 1:1 relation with an L2CAP CID.
 *
 * Generated Events:
 *      L2EVENT_CONNECTED: succeeded
 *      L2EVENT_DISCONNECTED:failed
 *
 * Returns:
 *    BT_STATUS_PENDING - The connection process has been successfully
 *        initialized. When the connection process is completed, the
 *        registered callback will be notified with
 *        L2EVENT_CONNECTED (succeeded) or
 *        L2EVENT_DISCONNECTED (failed).
 *
 *    BT_STATUS_NO_RESOURCES - L2CAP could not allocate a channel
 *        structure to initiate the connection.
 *
 *    BT_STATUS_RESTRICTED - The PSM is not authorized to establish
 *        outbound connections.
 *
 *    BT_STATUS_IN_USE - This context is already in use. This function was
 *        called before with the same context, or this context was registered
 *        before as a server via L2CAP_RegisterPsm.
 *
 *    BT_STATUS_INVALID_PARM -A parameter is not correct according the definitions
 *        above. It can also mean that a second attempt is made to connect to
 *        a 'remotePsm' that is already connected to this device, which is not allowed.
 */
BtStatus BTL_L2CAP_Connect(BtlL2capContext *l2capContext,
                           BD_ADDR *bdAddr,
                           L2capPsmValue remotePsm,
                           L2capConnSettings *connectSettings,
                           BtlL2capChannelSettings *channelSettings,
                           BtlL2capChannelId *channelId);

/*---------------------------------------------------------------------------
 * BTL_L2CAP_ConnectResponse()
 *
 * Brief:
 *    Accept or reject an incoming connection request (L2EVENT_CONNECT_IND).
 *      
 * Description:
 *    Send a response on an L2CAP Connect Request for anL2CAP channel. The 
 *    response can 'accept', 'delay'(pending) or 'reject' an incoming 
 *    connection(indicated by L2EVENT_CONNECT_IND).
 *
 * Type:
 *    Asynchronous.
 *
 * Parameters:
 *    l2capContext [in] - Pointer to the L2CAP context.
 *         
 *    channelId [in] - Channel ID assigned by L2CAP to this connection. This
 *        ID must match the one provided by the L2EVENT_CONNECT_IND event
 *        callback.
 *
 *    responseCode [in] - Response code to indicate the status of the requested 
 *        connection. This value indicates L2CAP_ACCEPTED, L2CAP_PENDING,
 *        L2CAP_REJECT_NO_RECOURCES or L2CAP_REJECT_SECURITY_BLOCK.
 *
 *    connectSettings [in] - Channel-specific connection settings. This
 *         parameter may be NULL if no settings are relevant.
 * 
 * Generated Events:
 *      L2EVENT_CONNECTED: succeeded
 *      L2EVENT_DISCONNECTED: failed
 *      L2EVENT_COMPLETE: connection was in progress, now this function can be called.
 *
 * Returns:
 *    BT_STATUS_PENDING - The connection response packet has been initiated
 *        successfully. One of the following events will arrive to the
 *        application to indicate completion:
 *        - L2EVENT_CONNECTED, if the connection was established successfully.
 *        - L2EVENT_DISCONNECTED, if the connection failed. Check the 
 *          callback info's "status" parameter for the exact reason.
 *
 *    BT_STATUS_IN_PROGRESS - A response is already in progress. Wait for
 *        L2EVENT_COMPLETE before calling L2CAP_ConnectRsp.
 *
 *    BT_STATUS_NO_CONNECTION - The ACL link associated with this 
 *        channel is no longer active.
 */
BtStatus BTL_L2CAP_ConnectResponse(BtlL2capContext *l2capContext, 
					        				         BtlL2capChannelId channelId,
									                 L2capConnStatus responseCode,
                                   L2capConnSettings *connectSettings);

/*---------------------------------------------------------------------------
 * BTL_L2CAP_Disconnect()
 *
 * Brief:
 *    Disconnect an L2CAP channel between two devices.
 *      
 * Description:
 *    Disconnect an L2CAP channel ('channelId') between two devices.
 *    When the channel is closed the application will receive an
 *    L2EVENT_DISCONNECTED.event 
 *
 * Type:
 *    Asynchronous
 *
 * Parameters:
 *    l2capContext [in] - Pointer to the L2CAP context.
 *
 *    channelId [in] - Identification of the L2CAP channel to disconnect. 
 *
 * Generated Events:
 *      L2EVENT_DISCONNECTED: succeeded
 *
 * Returns: 
 *    BT_STATUS_PENDING - The disconnect request has been initiated
 *        successfully. When the disconnect is complete, the
 *        L2EVENT_DISCONNECTED event is provided to the application.
 *
 *    BT_STATUS_FAILED - The channel is invalid or could not be
 *        disconnected.
 *
 *    BT_STATUS_NO_CONNECTION - The ACL link associated with this 
 *        channel is no longer active.
 */
 BtStatus BTL_L2CAP_Disconnect(BtlL2capContext *l2capContext,
                               BtlL2capChannelId channelId);


/*---------------------------------------------------------------------------
 * BTL_L2CAP_SendData()
 *
 * Brief:
 *    Sends data on the specified 'channelId'.
 *      
 * Description:
 *    Sends data on the specified 'channelId'. The 'data' and 'dataLen' are 
 *    used to send a data buffer to the other side. When data transmission is 
 *    complete, L2CAP generates an L2EVENT_PACKET_HANDLED event for the 
 *    channel with the returned 'packet'. The "status" field of the 
 *    L2CallbackParms structure will indicate whether the transmission was 
 *    successful  or not
 *
 * Type:
 *    Asynchronous
 *
 * Parameters:
 *    l2capContext [in] - Pointer to the L2CAP context.
 *
 *    channelId [in] - Channel ID assigned by L2CAP, identifying the channel
 *        for data transmission.
 *
 *    data [in] - Points to a buffer of user data. This pointer is owned by 
 *        L2CAP until the BtPacket (returned as last argument) is returned 
 *        with the L2EVENT_PACKET_HANDLED event.
 *
 *    dataLen [in] - Indicates the length of 'data' in bytes.
 *
 *    packet [out] - The BtPacket (container) used to send data.
 *        This pointer is returned  with the L2EVENT_PACKET_HANDLED event,
 *        and only then the 'data' pointer can be released.
 *        The 'data' should stay valid during this lifetime.
 *          
 * Generated Events:
 *      L2EVENT_PACKET_HANDLED: finished
 *          
 * Returns:
 *    BT_STATUS_PENDING - Packet was successfully queued for transmission on
 *        the channel. The completion of the send request is indicated by an
 *        L2EVENT_PACKET_HANDLED event.
 *
 *    BT_STATUS_FAILED - The channel is invalid or is not connected.
 *
 *    BT_STATUS_NO_CONNECTION - The ACL link associated with this 
 *        channel is no longer active.
 */
BtStatus BTL_L2CAP_SendData(BtlL2capContext *l2capContext,
                            BtlL2capChannelId channelId,
                            U8 *data, 
                            U16 dataLen,
                            BtPacket **packet);

#if L2CAP_PING_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 * BTL_L2CAP_Ping()
 *
 * Brief:
 *    Sends an L2CAP Echo Request to the remote device.
 *      
 * Description:
 *    Sends an Echo Request to the L2CAP protocol on the specified remote
 *    device. The data length should not exceed the default L2CAP
 *    signaling MTU (44 bytes).
 *
 * Type:
 *    Asynchronous.
 *
 * Requires:
 *    L2CAP_PING_SUPPORT enabled.
 *
 * Parameters:
 *    l2capContext [in] - Pointer to the L2CAP context.
 *
 *    bdAddr [in] - Bluetooth address of device to ping.
 *
 *    data [in] - Data to send to the remote device.
 *
 *    dataLen [in] - Length of "data" buffer in bytes. Must be less than 44.
 *
 * Generated Events:
 *      L2EVENT_COMPLETE: succeeded
 *
 * Returns:
 *    BT_STATUS_PENDING - Ping request was successfully queued for
 *        transmit. 
 *
 *    BT_STATUS_NO_RESOURCES - L2CAP could not allocate a signaling
 *        channel or packet.
 * 
 *    BT_STATUS_NO_CONNECTION - The ACL link to the specified device
 *        is not active.
 */
BtStatus BTL_L2CAP_Ping(BtlL2capContext *l2capContext,
                        BD_ADDR *bdAddr,
                        const U8 *data, 
                        U16 dataLen);
#endif /* L2CAP_PING_SUPPORT */


#if L2CAP_GET_INFO_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 * BTL_L2CAP_GetInfo()
 *
 * Brief:
 *    Sends an L2CAP Info Request to the remote device.
 *      
 * Description:
 *    Sends an Info Request to the L2CAP protocol on the specified remote
 *    device. Info requests are used to exchange implementation-specific
 *    information regarding L2CAP's capabilities.
 *
 * Type:
 *    Asynchronous
 *
 * Requires:
 *    L2CAP_GET_INFO_SUPPORT enabled.
 *
 * Parameters:
 *    l2capContext [in] - Pointer to the L2CAP context.
 *
 *    bdAddr [in] - Bluetooth address of the device to query.
 *
 *    type [in] - Type of information to request(Connectionless MTU or 
 *        Extended features).
 *
 * Generated Events:
 *      L2EVENT_COMPLETE: succeeded
 *
 * Returns:
 *    BT_STATUS_PENDING - Sending of an Information Request has been
 *        successfully initiated. 
 *
 *    BT_STATUS_NO_RESOURCES - L2CAP could not allocate a signaling
 *        channel or packet.
 * 
 *    BT_STATUS_NO_CONNECTION - The ACL link to the specified device
 *        is not active.
 */
BtStatus BTL_L2CAP_GetInfo(BtlL2capContext *l2capContext,
                           BD_ADDR *bdAddr, 
                           L2capInfoType type);
#endif /* L2CAP_GET_INFO_SUPPORT */

/*---------------------------------------------------------------------------
 * BTL_L2CAP_GetChannelConfig()
 *
 * Brief:
 *    Gets the channel configuration for this channel.
 *      
 * Description:
 *    Get the channel configuration for a specific connection ('channelId')
 *    as it was negotiated during the connection setup.
 *
 * Type:
 *    Synchronous
 *
 * Parameters:
 *    l2capContext [in] - L2CAP context.
 *
 *    channelId [in] - ID of an existing connection.
 * 
 *    channelConfig [out] - actual channel configuration.
 *   
 * Returns:
 *    BT_STATUS_PENDING - Packet was successfully queued for transmission
 *        on the channel. The completion of the send request is indicated
 *        by an L2EVENT_PACKET_HANDLED event.
 *
 *    BT_STATUS_FAILED - The channel is invalid or is not connected.
 *
 *    BT_STATUS_NO_CONNECTION - This channel is not connected.
 */
BtStatus BTL_L2CAP_GetChannelConfig(BtlL2capContext *l2capContext,
                                    BtlL2capChannelId channelId, 
                                    BtlL2capChannelConfig *channelConfig);

#endif /* __BTL_L2CAP_H */

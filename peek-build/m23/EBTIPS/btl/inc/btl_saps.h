/*******************************************************************************\
##                                                                             *
##  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION          *
##                                                                             *
##  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE        *
##	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE      *
##	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO      *
##	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT       *
##	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL        *
##	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC          *
##                                                                             *
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      btl_saps.h
*
*   BRIEF:          This file defines the API for the BTL Sim Access Profile
*                   (SAP) for the server role only.
*
*   DESCRIPTION:    General
*
*                   Via this API the application can manage the SAP server
*                   profile functionality. The API can be divided into 2
*                   groups.
*
*                   1. Management functions:
*                   These are the functions that can be found in all/most of
*                   the BTL modules as a consistent API:
*                   - BTL_SAPS_Init
*                   - BTL_SAPS_Deinit
*                   - BTL_SAPS_Create
*                   - BTL_SAPS_Destroy
*                   - BTL_SAPS_Enable
*                   - BTL_SAPS_Disable
*                   - BTL_SAPS_GetConnectedDevice
*                   - BTL_SAPS_SetSecurityLevel
*                   - BTL_SAPS_GetSecurityLevel
*                   The remaining function is more specific for SAP
*
*                   2. SAP server functions:
*                   - BTL_SAPS_RespondOpenInd:
*                     accept or decline an incoming connection.
*
*                   Via the management functions, the application can start-up
*                   and shutdown the application.
*                   Via the 'BTL_SAPS_RespondOpenInd' it can dynamically react
*                   on an incoming connection request.
*
*                   When the connection is established, then all data transfer
*                   over the SAP connection is relayed directly to the
*                   BTHAL_SIM interface, representing an abstraction to the
*                   SIM-card. (see bthal_sim.h for details).
*                   
*   AUTHOR:         Gerrit Slot
*
\*******************************************************************************/

#ifndef __BTL_SAPS_H
#define __BTL_SAPS_H

/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "xatypes.h" /* for types such as U8, U16, S8, S16,... */
#include "sim.h"
#include "btl_common.h"
#include "btl_unicode.h"

/* Forward declarations */
typedef struct _BtlSapsEvent 	  BtlSapsEvent;
typedef struct _BtlSapsContext 	BtlSapsContext;

/********************************************************************************
 *
 * Macros
 *
 *******************************************************************************/
/* Use the last defined event from the SIM profile a offset for my own events. */
#define BTL_SAPS_LAST_SIM_EVENT SAEVENT_SERVER_STATUS_SENT

/* My own defined events */
#define SAPS_EVENT_DISABLED ((U8) BTL_SAPS_LAST_SIM_EVENT + 1)

/*-------------------------------------------------------------------------------
 * BtlSapsCallBack type
 *
 *     A function of this type is called to indicate BTL SAP events.
 */
typedef void (*BtlSapsCallBack)(BtlSapsEvent *event);

/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/
/*-------------------------------------------------------------------------------
 * BtlSapsEvent structure
 *
 *     Represents a BTL SAP event.
 */
struct _BtlSapsEvent 
{
	/* Associated context */
	BtlSapsContext   *context;
	/* Event ID */
  SimEvent          event;
  /* Event specific Data/Parameters */
	SimCallbackParms *parms;
};

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/********************************************************************************
 *
 * Function declarations
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BTL_SAPS_Init()
 *
 * Brief:
 *    Init the SAPS module.
 *
 * Description:
 *    Init the SAPS module.
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
 *
 *    BT_STATUS_IMPROPER_STATE - Module is in a wrong state, most likely
 *        because it was already initialized before.
 */
BtStatus BTL_SAPS_Init(void);


/*-------------------------------------------------------------------------------
 * BTL_SAPS_Deinit()
 *
 * Brief:  
 *    Deinit the SAPS module.
 *
 * Description:
 *    Deinit the SAPS module.
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
 *		BT_STATUS_FAILED - There is still a SAPS context created. It should be
 *      destroyed before.
 *
 *		BT_STATUS_INTERNAL_ERROR - Internal error did occur 
 */
BtStatus BTL_SAPS_Deinit(void);

 
/*-------------------------------------------------------------------------------
 * BTL_SAPS_Create()
 *
 * Brief:  
 *    Allocates a unique SAPS context.
 *
 * Description:
 *		Allocates a unique SAPS context.
 *		This function must be called before any other SAPS API function.
 *		The allocated context should be supplied in subsequent SAPS API calls.
 *		The caller must also provide a callback function, which will be called 
 *		on SAPS events.
 *		The caller can provide an application handle (previously allocated 
 *		with BTL_RegisterApp), in order to link between different modules.
 *		If there is no need to link between different modules, set appHandle
 *    to NULL.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		appHandle [in] - application handle, can be NULL.
 *
 *		callback [in] - all SAPS events will be sent to this callback.
 *
 *		securityLevel [in] - level of security which should be applied,	when
 *			connecting to a remote SAP client. 
 *			This parameter is not supported yet, and should be filled with NULL.
 *      This means that the default security level as defined by the underlying
 *      stack module is used.
 *		
 *		context [out] - allocated SAPS context.	
 *
 * Returns:
 *		BT_STATUS_SUCCESS - SAPS context was created successfully.
 *
 *		BT_STATUS_FAILED -  Failed to create SAPS context.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *		BT_STATUS_NO_RESOURCES - No more resources.
 *
 *		BT_STATUS_INTERNAL_ERROR - Internal error has occur.
 */
BtStatus BTL_SAPS_Create(BtlAppHandle           *appHandle,
                         const BtlSapsCallBack   callback,
                         const BtSecurityLevel  *securityLevel,
                         BtlSapsContext        **sapsContext);


/*-------------------------------------------------------------------------------
 * BTL_SAPS_Destroy()
 *
 * Brief:  
 *    Releases a SAPS context (previously allocated with BTL_SAPS_Create).
 *
 * Description:
 *		Releases a SAPS context (previously allocated with BTL_SAPS_Create).
 *		An application should call this function when it completes using SAPS services.
 *    Before calling this function, the SAPS context should have been disabled.
 *		Upon completion, the SAPS context is set to NULL in order to prevent 
 *		the application from an illegal attempt to keep using it.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		context [in/out] - SAPS context.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - SAPS context was destroyed successfully.
 *
 *		BT_STATUS_IN_USE - SAPS context is still in use. It should
 *      be disabled first.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *		BT_STATUS_INTERNAL_ERROR - Internal error has occur.
 */
BtStatus BTL_SAPS_Destroy(BtlSapsContext **sapsContext);


/*-------------------------------------------------------------------------------
 * BTL_SAPS_Enable()
 *
 * Brief:  
 *    Enable SAPS and register SAP server SDP record.
 *
 * Description:
 *		Enable SAPS and register SAP server SDP record, called after 
 *		BTL_SAPS_Create.
 *		After calling this function, SAP server is ready for usage.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		context [in] - SAPS context.
 *
 *		serviceName [in] - 0-terminated service name which will be written into 
 *			SDP service record and will be discovered by the peer device.
 *			If NULL is passed, the default name ("SIM Access") is used.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *    BT_STATUS_IN_USE - This 'context' was already in use (enabled)
 * 
 *		BT_STATUS_FAILED - The server could not be registered, probably 
 *      because the serviceName is too long or due to an internal error.
 *
 *    BT_STATUS_NO_RESOURCES - Could not register a channel with RFCOMM.
 */
BtStatus BTL_SAPS_Enable(BtlSapsContext *sapsContext, 
                         const BtlUtf8  *serviceName);


/*-------------------------------------------------------------------------------
 * BTL_SAPS_Disable()
 *
 * Brief:
 *    Disable SAPS. Remove the registration of this server (from the SDP)
 *
 * Description:
 *		Disable SAPS. Remove the registration of this server (from the SDP),
 *    so this server is not visible anymore from a remote side.
 *		If a connection exists, it will be disconnected automatically.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		context [in] - SAPS context.
 *
 * Generated Events:
 *    SAEVENT_SERVER_CLOSED
 *    SAPS_EVENT_DISABLED
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_PENDING - The link is connected. The disconnect process has 
 *			been successfully started. When the link is disconnected, the application
 *      will receive the disconnect event (SAEVENT_SERVER_CLOSED).
 *      When the disconnect process is complete, the SAPS_EVENT_DISABLED will
 *      be sent to the application.
 */
BtStatus BTL_SAPS_Disable(BtlSapsContext *sapsContext);

/*-------------------------------------------------------------------------------
 * BTL_SAPS_Disconnect()
 *
 * Brief:
 *    Initiates the disconnection of the server transport connection.
 *
 * Description:
 *		Initiates the disconnection of the server transport connection.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		context [in] - SAPS context.
 *
 *    immediate [in] -
 *      TRUE = terminates the connection immediately.  No response is required
 *        from the client.  The GSM connection should be terminated immediately.
 *      FALSE = a graceful disconnect allows the client to continue sending
 *        APDU's until while the GSM connection is terminated.
 *        The client will then initiate the disconnect process. 
 *        Until the client initiates the disconnect process, the connection
 *        stays open.    
 *
 * Generated Events:
 *    SAEVENT_SERVER_CLOSED
 * 
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_PENDING - Operation was successfully started.
 *       Completion will be signaled via an event (SAEVENT_SERVER_CLOSED) to
 *       the application callback..
 *
 *    BT_STATUS_BUSY - The server is already in the process of disconnecting.
 *
 *    BT_STATUS_NO_CONNECTION - Operation was not started because
 *      there is no remote service connected.
 *
 *    BT_STATUS_INVALID_PARM - Invalid parameter.
 */
BtStatus BTL_SAPS_Disconnect(BtlSapsContext *sapsContext,
                             BOOL            immediate);


/*-------------------------------------------------------------------------------
 * BTL_SAPS_GetConnectedDevice()
 *
 * Brief:
 *    This function returns the connected device.
 *      
 * Description:
 *		This function returns the connected device.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		context [in] - SAPS context.
 *
 *		bdAddr [out] - pointer to 48-bit address of the connected device.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_NO_CONNECTION -  The server is not connected.
 *
 *    BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *    BT_STATUS_INTERNAL_ERROR - Internal administration conflict. There
 *      is a connection, but the 'bdAddr' is not administrated.
 * 
 */
BtStatus BTL_SAPS_GetConnectedDevice(BtlSapsContext *sapsContext, 
                                     BD_ADDR        *bdAddr);

/*-------------------------------------------------------------------------------
 * BTL_SAPS_SetSecurityLevel()
 *
 * Brief:
 *    Sets security level for the given SAPS context.
 *
 * Description:
 *    Sets security level for the given SAPS context.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *    context [in] - pointer to the SAPS context.
 *
 *		securityLevel [in] - level of security which should be applied,	when
 *			connecting to a remote SAP client. For default value, pass NULL.
 *      As default, the authentication + encryption is enabled.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */
BtStatus BTL_SAPS_SetSecurityLevel(BtlSapsContext        *sapsContext,
                                   const BtSecurityLevel *securityLevel);


/*-------------------------------------------------------------------------------
 * BTL_SAPS_GetSecurityLevel()
 *
 * Brief:
 *    Gets security level for the given SAPS context.
 *
 * Description:
 *    Gets security level for the given SAPS context.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *    sapsContext [in] - pointer to the SAPS context.
 *
 *		securityLevel [out] - level of security which should be applied, when
 *			connecting to a remote SAP client.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */
BtStatus BTL_SAPS_GetSecurityLevel(BtlSapsContext  *sapsContext,
                                   BtSecurityLevel *securityLevel);


/*---------------------------------------------------------------------------
 * BTL_SAPS_RespondOpenInd()
 *
 * Brief:
 *    Accepts or rejects an incoming connection.
 *
 * Description:
 *    Accepts or rejects an incoming connection in response to an 
 *    SAEVENT_SERVER_OPEN_IND event. This event occurs when a remote client 
 *    attempts to connect to a registered server. This function must be 
 *    used to respond to the connection request.
 *
 * Type:
 *		Synchronous/Asynchronous.
 *
 * Parameters:
 *    context[in] - Identifies the server that is accepting the connection.  This 
 *         server is provided to the callback function as a parameter during the
 *         SAEVENT_SERVER_OPEN_IND event.
 *
 *    accept[in] - When TRUE, then the server is ready to provide SIM access
 *         by the Client. When the server does contain a SIM module, it should
 *         this SIM module first before calling this function. When the connection
 *         is established, the SAEVENT_SERVER_OPEN event will be 
 *         received.  Previous to the SAEVENT_SERVER_OPEN event, a 
 *         SAVEVENT_SERVER_STATUS_REQ event will be received.  The application 
 *         must respond to this by sending the current card status by calling 
 *         SIM_ServerSendStatus.  The application may also receive a 
 *         SAEVENT_SERVER_ATR_REQ event.  If this event is received, the 
 *         application must call SIM_SendAtrRsp with the ATR response.
 *         When accept = FALSE, then the connection is rejected and the connection
 *         remains closed.
 *
 * Generated Events:
 *    SAEVENT_SERVER_OPEN: succeeded
 *
 *
 * Returns:
 *    BT_STATUS_PENDING - The accept message will be sent. The application
 *         will receive a SAEVENT_SERVER_OPEN when the accept message has been 
 *         sent and the channel is open.
 *
 *    BT_STATUS_BUSY - A response is already in progress.
 *
 *    BT_STATUS_NO_CONNECTION - No connection to accept.
 */
BtStatus BTL_SAPS_RespondOpenInd(BtlSapsContext *sapsContext,
                                 BOOL            accept);

#endif /* __BTL_SAPS_H */

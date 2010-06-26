/*******************************************************************************\
##                                                                           	*
##  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION			*
##																				*
## LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE			*
##	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE		*
##	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO		*
##	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT		*
##	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL			*
##	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.			*
##																				*
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      btl_pbaps.h
*
*   BRIEF:          This file defines the API of the BTL phonebook access profile
*                   server role.
*
*	DESCRIPTION:    General
*
*					The BTL_PBAPS defines the API of the BTL phonebook access profile
*                   server role, for the use of the application. Being a server API,  
*					the main use of the it is basic initialization and connectivity 
*					operations, whereas the device's phonebook access implementation 
*					is defined in bthal_pb.h .
*
*
*					Usage Models
*					
*					The BTL_PBAP server representation is a PBAP context (BtlPbapsContext).
*					The context has to created and enabled in order to receive remote 
*					connections and requests. (BTL_PBAPS_Create, BTL_PBAPS_Enable).
*					
*					The server handles the requests initiated by the remote client,
*					and is also capable of initiating a disconnect operation, or an 
*					abort operation. (BTL_PBAPS_Disconnect, BTL_PBAPS_Abort).
*					
*					The application will be notified by events regarding the on-going
*					operations. See below list of events. The events callback function        
*					should be registered using the BTL_PBAPS_Enable function.
*					
*					Additional API: set/get security, configure OBEX authentication,
*					get remote device BD address.
*
*
*                   Generated Events List
*
*					See pbap.h for full events description.
*                   PBAP_EVENT_TP_CONNECTED
* 					PBAP_EVENT_TP_DISCONNECTED
* 					PBAP_EVENT_START
* 					PBAP_EVENT_COMPLETE           
* 					PBAP_EVENT_ABORTED
*					PBAP_EVENT_AUTH_RESULT
* 					PBAP_EVENT_AUTH_CHALLENGE
*
*
*					Execution Flow
*
*					The PBAP remote client initiates an operation. The applicaition 
*					is notified by an event. The request is handled internally in
*					the BTL layer by accessing the BTHAL_PB. At the end of the operation
*					the application is also notified by an event.
*
*   AUTHOR:         Itay Klein
*
\*******************************************************************************/

#ifndef __BTL_PBAPS_H
#define __BTL_PBAPS_H

/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/

#include <bttypes.h>
#include <pbap.h>
#include <btl_common.h>
#include "btl_unicode.h"


/********************************************************************************
 *
 * Event handling
 * 
 * The following PBAP events are passed to application: 
 * 
 *
 *******************************************************************************/
 
  
/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/* forward declarations: */

typedef struct _BtlPbapsContext BtlPbapsContext;
typedef struct _BtlPbapsEvent BtlPbapsEvent;
 
/*---------------------------------------------------------------------------
 *  BtlPbapsCallback type
 *
 *      A function of this type is called to indicate pbap server events.
 */
typedef void (*BtlPbapsCallBack)(const BtlPbapsEvent *event);

/*---------------------------------------------------------------------------
 *  RepositoriesMask type.
 *
 *      A bit mask representing phone book repositories.
 */
typedef U8 BtlPbapsRepositoriesMask;

#define BTL_PBAPS_LOCAL_PHONEBOOK   0x01
#define BTL_PBAPS_SIM_CARD          0x02


/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/

/*---------------------------------------------------------------------------
 *  BtlPbapsEvent structure
 *
 *      Represents a PBAP server event.
 */
struct _BtlPbapsEvent
{
	/*	The PBAP server context. */
    BtlPbapsContext *pbapsContext;

	/*	A structure holding parameters regarding the incoming event.
		The field event of type PbapEvent describes the event type.
		When data request is received from the client the application
		will fill the relevant fields in callbackParms.
		for example, when receiving the event PBAP_EVENT_OBJECT_LENGTH_REQ
		the field callbackParms.u.objectReq.objectLen needs to be assigned.

		Before the result is actually sent to the client the event:
		PBAP_EVENT_PRECOMPLETE will be sent to the application
		to provide an opportunity to complete the operation. */
    PbapServerCallbackParms *callbackParms;
};

/********************************************************************************
 *
 * Function Reference
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BTL_PBAPS_Init()
 *
 * Brief:  
 *		Init the PBAPS module.
 *
 * Description:
 *      Init the PBAPS module.
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
BtStatus BTL_PBAPS_Init();

/*-------------------------------------------------------------------------------
 * BTL_PBAPS_Deinit()
 *
 * Brief:  
 *		Deinit the PBAPS module.
 *
 * Description:
 *      Deinit the PBAPS module.
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
BtStatus BTL_PBAPS_Deinit();

/*---------------------------------------------------------------------------
 * BTL_PBAPS_Create
 *
 *  Brief:  
 *      Allocates a unique PBAP server context.
 *
 *  Description:
 *		Allocates a PBAP server context.
 *		This function must be called before any other BTL PBAPS API function.
 *		The allocated context should be supplied in subsequent
 *      BTL PBAPS API calls.
 *		The caller must also provide a callback function,
 *      which will be called on PBAP server events.
 *		The caller can provide an application handle
 *      (previously allocated with BTL_RegisterApp),
 *      in order to link between different modules.
 *
 *  Type:
 *      Synchronous.
 *
 *  Parameters:
 *
 *		appHandle [in] - Application handle, If there's no need for 
 *                       an application handle set appHandle to 0.
 *
 *		pbapsCallback [in] - All PBAPS events will be sent to this callback.
 *
 *      secLevel [in] - The required security level.
 *                      If NULL value is passed,
 *                      the default security level is used.
 *
 *		pbapsContext [out] - Allocated PBAPS context.
 *
 *  Returns:
 *      BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *      BT_STATUS_IN_USE - The operation failed because the PBAPS context
 *                         has already been created. 
 *
 *      BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *                               initialized (XA_ERROR_CHECK only).
 */
BtStatus BTL_PBAPS_Create(BtlAppHandle *appHandle,
                          BtlPbapsCallBack pbapsCallback,
                          BtSecurityLevel *secLevel,
                          BtlPbapsContext **pbapsContext);

/*---------------------------------------------------------------------------
 * BTL_PBAPS_Destory()
 *
 *  Brief:  
 *		Releases a PBAP server context (previously allocated with BTL_PBAPS_Create).
 *
 *  Description:
 *      Releases a PBAP server context,
 *		An application should call this function 
 *      when it completes using a PBAPS context services.
 *
 *  Type:
 *      Synchronous.
 *
 *  Parameters:
 *      pbapsContext [in/out] - A PBAPS context, the PBAPS context is set to 
 *                              null in order to prevent the application
 *                              from an illegal attempt to keep using it.
 *
 *  Returns:
 *      BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *      BT_STATUS_NOT_FOUND - The context has not been created.
 *
 *      BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *                               initialized (XA_ERROR_CHECK only).
 */
BtStatus BTL_PBAPS_Destroy(BtlPbapsContext **pbapsContext);


/*-------------------------------------------------------------------------------
 *  BTL_PBAPS_Enable()
 *
 *  Brief:  
 *      Enable PBAPS, called after BTL_PBAPS_Create.
 *
 *  Description:
 *      Enables the PBAP server service,
 *      called to allow incoming and outgoing connections.
 *      Publishes a SDP record and registers within the OBEX.
 *		Should be after BTL_PBAPS_Create.
 *
 *  Type:
 *      Synchronous.
 *
 *  Parameters:
 *      pbapsContext [in] - The context to enable.
 *
 *      serviceName [in] - A name given to the service
 *                         If NULL value is passed the default service name,
 *                         "Phonebook Access PSE", is used.
 *
 *      reposMask [in] - A bit mask defining the active phonebook repositories.
 *
 *  Returns:
 *      BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *      BT_STATUS_FAILED - Operation failed, either in registering a SDP service
 *                         Or registering a RFCOMM channel.
 *
 *      BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *                               initialized (XA_ERROR_CHECK only).      
 */
BtStatus BTL_PBAPS_Enable(BtlPbapsContext *pbapsContext,
                          BtlUtf8 *serviceName,
                          BtlPbapsRepositoriesMask *reposMask);

/*-------------------------------------------------------------------------------
 *  BTL_PBAPS_Disable()
 *
 *  Brief:  
 *		Disable PBAPS, called before BTL_PBAPS_Destory.
 *
 *  Description:
 *      Removes the service SDP record and unregisters with OBEX.
 *      Disconnects transport connections if exist.
 *		Should be called before BTL_PBAPS_Destory.
 *      
 *  Type:
 *      Asynchronous or Synchronous.
 *
 *  Parameters:
 *      pbapsContext - The context to disable.
 *
 *  Generated Events:
 *      PBAP_EVENT_DISABLED
 *      PBAP_EVENT_TP_DISCONNECTED
 *      PBAP_EVENT_COMPLETE
 *
 *  Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_BUSY - The server is currently executing an operation.
 *
 *		BT_STATUS_PENDING - The link is connected. The disconnect process has 
 *			been successfully started. When the disconnect process is complete, the
 *         	application callback will receive the PBAP_EVENT_TP_DISCONNECTED event,
 *			which indicates the PBAPS is disabled.
 *			When the disable process is complete, the application callback will 
 *			receive the PBAP_EVENT_DISABLED event, indicating the context is disabled.
 *
 *      BT_STATUS_FAILED - The operation failed.
 *
 *      BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *                               initialized (XA_ERROR_CHECK only).      
 */
BtStatus BTL_PBAPS_Disable(BtlPbapsContext *pbapsContext);

/*-------------------------------------------------------------------------------
 * BTL_PBAPS_SetSecurityLevel()
 *
 * Brief:  
 *     	Sets security level for the given PBAPS context.
 *
 * Description:
 *     	Sets security level for the given PBAPS context.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		pbapsContext [in] - The server context.
 *
 *		secLevel [in] - the required security level, if NULL value is
 *                      passed the default security level is used.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - The operation succeeded.
 *
 *      BT_STATUS_NOT_FOUND - The context has not been created.
 *
 *      BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *                               initialized (XA_ERROR_CHECK only).
 */
BtStatus BTL_PBAPS_SetSecurityLevel(BtlPbapsContext *pbapsContext,
                                    BtSecurityLevel *secLevel);

/*-------------------------------------------------------------------------------
 * BTL_PBAPS_GetSecurityLevel()
 *
 *  Brief:  
 *     	Gets security level for the given PBAPS context.
 *
 *  Description:
 *     	Gets security level for the given PBAPS context.
 *
 *  Type:
 *		Synchronous
 *
 *  Parameters:
 *		pbapsContext [in] - The server context.
 *
 *		secLevel [out] - the current security level
 *
 *  Returns:
 *		BT_STATUS_SUCCESS - The operation succeeded.
 *
 *      BT_STATUS_NOT_FOUND - The context has not been created.
 *
 *      BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *                               initialized (XA_ERROR_CHECK only).
 */
 BtStatus BTL_PBAPS_GetSecurityLevel(BtlPbapsContext *pbapsContext,
                                     BtSecurityLevel *secLevel);

/*---------------------------------------------------------------------------
 * BTL_PBAPS_Abort()
 *
 * Brief:  
 *		Aborts the current server operation
 *
 * Description:
 *      Aborts the current server operation. The completion event will signal
 *     	the status of the operation in progress, either COMPLETE or ABORTED.
 *  
 * Type:
 *     Synchronous.
 * 
 * Parameters:
 *     pbapsContext [in] - The server context.
 *
 *     resp [in]- The response code to send to the client 
 *                expressing the reason for the abort.
 *
 * Generated Events:
 *      PBAP_EVENT_ABORTED 
 *
 * Returns:
 *     BT_STATUS_PENDING - The abort was successfully requested.
 *
 *     BT_STATUS_FAILED - The abort operation failed.
 *            
 *     BT_STATUS_NO_CONNECTION - No transport connection exists.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameters (XA_ERROR_CHECK only).
 */ 
BtStatus BTL_PBAPS_Abort(BtlPbapsContext *pbapsContext,
                         PbapRespCode resp);

/*-------------------------------------------------------------------------------
 * BTL_PBAPS_GetConnectedDevice()
 *
 * Brief:  
 *      This function returns the connected device.
 *      
 * Description:
 *      This function returns the connected device.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		pbapsContext [in] - PBAP context.
 *
 *		bdAddr [out] - pointer to 48-bit address of the connected device.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *		BT_STATUS_NO_CONNECTION -  The server is not connected.
 */
BtStatus BTL_PBAPS_GetConnectedDevice(BtlPbapsContext *pbapsContext, 
									  BD_ADDR *bdAddr);

/*-------------------------------------------------------------------------------
 * BTL_PBAPS_ConfigInitiatingObexAuthentication()
 *
 * Brief:
 * 		This function configures the OBEX authentication procedure of the PBAP server.
 *
 * Description:  
 *		Enable or disable server initiating OBEX authentication, when PBAP client 
 *		is connecting.
 *		In both cases, the server will still respond to OBEX authentication 
 *		initiated by the client (via BTL_PBAPS_ObexAuthenticationResponse()).
 *		Server initiating OBEX authentication is disabled by default.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		pbapsContext [in] - PBAPS context.
 *
 *		enableObexAuth [in] - enable initiating OBEX authentication.
 *
 *		realm [in] - null-terminated string used with OBEX authentication.
 *			This string should be displayed in the peer device when authentication 
 *			challenge is sent to the peer. The realm is indicating which userid 
 *			and/or password to use. This parameter is ignored if enableObexAuth 
 *			is FALSE. Realm is optional, thus NULL can be passed.
 *
 *		userId [in] - null-terminated string used with OBEX authentication 
 *			representing the user id. If NULL is passed, the user id is not 
 *			verified. This parameter is ignored if enableObexAuth is FALSE.
 *
 *		password [in] - null-terminated string used with OBEX authentication 
 *			representing the password. This parameter is ignored if 
 *			enableObexAuth is FALSE.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */
BtStatus BTL_PBAPS_ConfigInitiatingObexAuthentication(BtlPbapsContext *pbapsContext, 
													const BOOL enableObexAuth,
													const BtlUtf8 *realm, 
													const BtlUtf8 *userId, 
													const BtlUtf8 *password);

/*-------------------------------------------------------------------------------
 * BTL_PBAPS_ObexAuthenticationResponse()
 *
 * Brief:
 * 		This function issues an OBEX authentication challenge response.
 * 
 * Description: 
 *		This function must be called after receiving OBEX authentication 
 *		challenge from the client.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		pbapsContext [in] - PBAPS context.
 *
 *		userId [in] - null-terminated string used with OBEX authentication 
 *			representing the user id. If the argument is not needed by the 
 *			client (see in event), NULL can be passed.
 *
 *		password [in] - null-terminated string used with OBEX authentication 
 *			representing the password. Pass NULL to deny authentication.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */
BtStatus BTL_PBAPS_ObexAuthenticationResponse(BtlPbapsContext *pbapsContext,
												const BtlUtf8 *userId, 
												const BtlUtf8 *password);

/*---------------------------------------------------------------------------
 * BTL_PBAPS_Disconnect()
 *
 * Brief: 
 *      This function is used by the server to issue a transport disconnection.
 *
 * Description:
 *		This function is used by the server to issue a transport disconnection.
 *		If the ACL link is unused, it is disconnected as well.
 *
 * Type:
 *     Asynchronous
 *
 * Parameters:
 *     pbapsContext [in] - The server context.
 *
 * Generated Events:
 *      PBAP_EVENT_TP_DISCONNECTED
 *
 * Returns:
 *     OB_STATUS_PENDING - Operation was successfully started.
 *         Completion will be signaled with the PBAP_EVENT_TP_DISCONNECTED 
 *         event to the application callback.
 *
 *     OB_STATUS_NO_CONNECT - Operation was not started because
 *         the server is not connected.
 */
BtStatus BTL_PBAPS_Disconnect(BtlPbapsContext *pbapsContext);

#endif /* __BTL_PBAPS_H */


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
*   FILE NAME:      btl_oppc.h
*
*   BRIEF:          This file defines the API of the BTL Object Push Profile 
*                   client role.
*
*   DESCRIPTION:    General
*
*                   The BTL_OPPC defines the API of the Object Push profile (OPP) client role, 
*                   for the use of the application. Being a client API,	the main use of it 
*                   is basic initialization, connectivity, and support push/pull operations 
*                   to/from the remote server.
*
*					Usage Models
*
*                   The BTL_OPP client representation is an OPPC context (BtlOppcContext).
*					The context has to be created and enabled in order to send remote 
*					connections and requests. (BTL_OPPC_Create, BTL_OPPC_Enable).
*					
*					The OPP client sends the requests to the remote server,
*					and is also capable of initiating a disconnect operation, or an 
*					abort operation. (BTL_OPPC_Disconnect, BTL_OPPC_Abort).
*					
*					The application will be notified by events regarding the on-going
*					operations. See below list of events. The events callback function        
*					should be registered using the BTL_OPPC_Enable function.
*					
*					Additional API: set/get security, basic and encapsulated operations,
*					get remote device BD address.
*
*
*					Scenarios
*
*                   A. Basic operations (OPP client is already connected):
*
*                   1. Object Push - push an object to the inbox remote server. 
*	                2. Object Pull - pull a business card from remote server.
*                   3. Object Exchange - push follows by pull from remote server.
*
*                   B. Encapsulated operations (OPP client is not connected):
*
*                   1. Connect-Push-Disconnect 
*                   2. Connect-Pull-Disconnect 
*                   3. Connect-Exchange-Disconnect
*				
*                   Operational Modes
*
*					The BTL_OPPC supports the following modes:
*                   1. File system mode - all operations are handled by The BTHAL_FS to store or retrieve
*                      the business card.
*                   2. Memory mode - all operations are handled by OBSTORE to store or retrieve the business 
*                      card in memory.
*				
*
*                   Generated Events List
*
*                   OPPC_EVENT_START
* 					OPPC_EVENT_COMPLETE
* 					OPPC_EVENT_ABORTED
* 					OPPC_EVENT_TP_CONNECTED           
* 					OPPC_EVENT_TP_DISCONNECTED
*					OPPC_EVENT_DISCOVERY_FAILED
* 					OPPC_EVENT_NO_SERVICE_FOUND
*                   OPPC_EVENT_OBJECT_PROGRESS
*                   OPPC_EVENT_DISABLED
*
*					Execution Flow
*
*					All OPP Client operations (basic and encapsulated) are asynchronous.
*					At the beginning of each operation, a call to is made to start the operation
*					and consecutives generated events (e.g. OPPC_EVENT_COMPLETE) are received by the 
*                   BtlOppcCallback() to further support the operation.
*                   
*   AUTHOR:         Ilan Elias
*
\*******************************************************************************/

#ifndef __BTL_OPPC_H
#define __BTL_OPPC_H


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "opush.h"
#include "goep.h"
#include "btl_common.h"
#include "btl_unicode.h"

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/* Forward declarations */
typedef struct _OppClientEvent 	OppClientEvent;
typedef struct _BtlOppcEvent 	BtlOppcEvent;
typedef struct _BtlOppcContext	BtlOppcContext;

 
/*-------------------------------------------------------------------------------
 * BtlOppcCallBack type
 *
 *     A function of this type is called to indicate BTL OPPC events.
 */
typedef void (*BtlOppcCallBack)(const BtlOppcEvent *event);


/*-------------------------------------------------------------------------------
 * BtlOppcSupportedFormatsMask type
 *
 * Defines which formats are supported in the OPPC, can be ORed together.
 */
typedef U8 BtlOppcSupportedFormatsMask;

#define BTL_OPPC_SUPPORTED_FORMATS_VCARD21		(0x01)	/* vCard 2.1 */
#define BTL_OPPC_SUPPORTED_FORMATS_VCARD30		(0x02)	/* vCard 3.0 */
#define BTL_OPPC_SUPPORTED_FORMATS_VCAL			(0x04)	/* vCal 1.0 */
#define BTL_OPPC_SUPPORTED_FORMATS_ICAL			(0x08)	/* iCal 2.0 */
#define BTL_OPPC_SUPPORTED_FORMATS_VNOTE		(0x10)	/* vNote */
#define BTL_OPPC_SUPPORTED_FORMATS_VMSG			(0x20)	/* vMessage */
#define BTL_OPPC_SUPPORTED_FORMATS_ANY			(0xFF)	/* Any type of object */




/*-------------------------------------------------------------------------------
 * OppcObjectRequest type
 *
 *     	Defines the type of object request.
 */
typedef U8 OppcObjectRequest;

#define OPPC_OBJECT_REQUEST_NONE		(0x00)	/* No request (internal usage) */
#define OPPC_OBJECT_REQUEST_PUSH		(0x01)	/* Client push an object */
#define OPPC_OBJECT_REQUEST_PULL		(0x02)	/* Client pull an object */
#define OPPC_OBJECT_REQUEST_DELETE		(0x03)	/* Client delete an object */


/*---------------------------------------------------------------------------
 * OppcEvent type
 *
 *     The OppcEvent type defines the events that may be indicated to
 *     the OPP Server application.
 */
typedef U8 OppcEvent;

/* 
 * Indicates the start of a new operation. In the case of an Abort
 * operation, this event may occur during another pending operation (Put or Get).
 */
#define OPPC_EVENT_START            GOEP_EVENT_START

/* 
 * Indicates that the operation has completed successfully.
 */
#define OPPC_EVENT_COMPLETE         GOEP_EVENT_COMPLETE

/* 
 * Indicates that the current operation has failed.
 */
#define OPPC_EVENT_ABORTED          GOEP_EVENT_ABORTED

/* 
 * A transport layer connection has been established. There is no
 * operation associated with this event.
 */
#define OPPC_EVENT_TP_CONNECTED     GOEP_EVENT_TP_CONNECTED 

/* 
 * The transport layer connection has been disconnected. There is no
 * operation associated with this event.
 */
#define OPPC_EVENT_TP_DISCONNECTED  GOEP_EVENT_TP_DISCONNECTED

/* 
 * The transport layer discovery process has failed. (Client only)
 */
#define OPPC_EVENT_DISCOVERY_FAILED GOEP_EVENT_DISCOVERY_FAILED

/* 
 * The transport layer OBEX service cannot be found. (Client only)
 */
#define OPPC_EVENT_NO_SERVICE_FOUND GOEP_EVENT_NO_SERVICE_FOUND

/* 
 * Delivered to applications on each progress change.
 */
#define OPPC_EVENT_OBJECT_PROGRESS  15

/*
 * This event is received when all connections have been closed.
 */
#define OPPC_EVENT_DISABLED			16


/* End of OppcEvent */


/*---------------------------------------------------------------------------
 * OppcOperation type
 *
 *     The OppcOperation type defines operations based on the Generic
 *     Object Exchange Profile's operations.
 */
typedef U8 OppcOperation;

/* 
 * This value is used when indicating events which do not occur during
 * an operation, such as TP Connect and TP Disconnect.
 */
#define OPPC_OPER_NONE              GOEP_OPER_NONE

/* 
 * A Connect operation is in progress.
 */
#define OPPC_OPER_CONNECT           GOEP_OPER_CONNECT

/* 
 * A Disconnect operation is in progress. 
 */
#define OPPC_OPER_DISCONNECT        GOEP_OPER_DISCONNECT

/* 
 * A Push operation is in progress.
 */
#define OPPC_OPER_PUSH              GOEP_OPER_PUSH

/* 
 * A Pull operation is in progress.
 */
#define OPPC_OPER_PULL              GOEP_OPER_PULL

/* 
 * An abort operation is in progress. This event will be delivered while
 * another operation is in progress, if the ABORT is sent to stop that
 * operation.
 */
#define OPPC_OPER_ABORT             GOEP_OPER_ABORT

/* End of OppcOperation */


/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/


/*---------------------------------------------------------------------------
 * OppcProgressInd structure
 * 
 *     This structure is used during progress indications.
 */
typedef struct _OppcProgressInd 
{
	/* Name of the object, null-terminated. */
	BtlUtf8*   name;

	/* Length (in bytes) of the 'name' field. */
	U16     nameLen;

	/* Current position (in bytes) of the operation. */
    U32     currPos;

	/* Max position (in bytes) of the operation. */
	U32     maxPos;
} OppcProgressInd;


/*-------------------------------------------------------------------------------
 * OppClientEvent structure
 *
 *     Represents OPP Client event.
 */
struct _OppClientEvent
{
	/* OPPC_EVENT_xxx */
	OppcEvent           event;

	/* OPPC_OPER_xxx */
	OppcOperation       oper;

	union 
	{
		OppcProgressInd progress;  		/* Use during OPPC_EVENT_OBJECT_PROGRESS */
	} info;
};


/*-------------------------------------------------------------------------------
 * BtlOppcEvent structure
 *
 *     Represents BTL OPPC event.
 */
struct _BtlOppcEvent 
{
	BtlOppcContext      *oppcContext;
	OppClientEvent      *oppcEvent;
};


/********************************************************************************
 *
 * Function declarations
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BTL_OPPC_Init()
 *
 * Brief:  
 *		Init the OPPC module.
 *
 * Description:
 *      Init the OPPC module.
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
BtStatus BTL_OPPC_Init(void);


/*-------------------------------------------------------------------------------
 * BTL_OPPC_Deinit()
 *
 * Brief:  
 *		Deinit the OPPC module.
 *
 * Description:
 *		Deinit the OPPC module.
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
BtStatus BTL_OPPC_Deinit(void);

 
/*-------------------------------------------------------------------------------
 * BTL_OPPC_Create()
 *
 * Brief:  
 *      Allocates a unique OPPC context.
 *
 * Description:
 *	    Allocates a unique OPPC context.
 *		This function must be called before any other OPPC API function.
 *		The allocated context should be supplied in subsequent OPPC API calls.
 *		The caller must also provide a callback function, which will be called 
 *		on OPPC events.
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
 *		oppcCallback [in] - all OPPC events will be sent to this callback.
 *
 *		securityLevel [in] - level of security which should be applied,	when
 *			connecting to a remote OPP server. For default value, pass null.
 *		
 *		oppcContext [out] - allocated OPPC context.	
 *
 * Returns:
 *		BT_STATUS_SUCCESS - OPPC context was created successfully.
 *
 *		BT_STATUS_FAILED -  Failed to create OPPC context.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *		BT_STATUS_NO_RESOURCES - No more resources.
 *
 *		BT_STATUS_INTERNAL_ERROR - Internal error has occur.
 */
BtStatus BTL_OPPC_Create(BtlAppHandle *appHandle,
                        const BtlOppcCallBack oppcCallback,
                        const BtSecurityLevel *securityLevel,
                        BtlOppcContext **oppcContext);


/*-------------------------------------------------------------------------------
 * BTL_OPPC_Destroy()
 *
 * Brief:  
 *		Releases a OPPC context (previously allocated with BTL_OPPC_Create).
 *
 * Description:
 *      Releases a OPPC context (previously allocated with BTL_OPPC_Create).
 *		An application should call this function when it completes using OPPC services.
 *		Upon completion, the OPPC context is set to null in order to prevent 
 *		the application from an illegal attempt to keep using it.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		oppcContext [in/out] - OPPC context.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - OPPC context was destroyed successfully.
 *
 *		BT_STATUS_IN_USE -  OPPC context is in use.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *		BT_STATUS_INTERNAL_ERROR - Internal error has occur.
 */
BtStatus BTL_OPPC_Destroy(BtlOppcContext **oppcContext);


/*-------------------------------------------------------------------------------
 * BTL_OPPC_Enable()
 *
 * Brief:  
 *      Enable OPPC, called after BTL_OPPC_Create.
 *
 * Description:
 *      Enable OPPC, called after BTL_OPPC_Create.
 *		After calling this function, OPP client is ready for usage.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		oppcContext [in] - OPPC context.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *		BT_STATUS_FAILED - The specified context is already enabled.
 */
BtStatus BTL_OPPC_Enable(BtlOppcContext *oppcContext);


/*-------------------------------------------------------------------------------
 * BTL_OPPC_Disable()
 *
 * Brief:  
 *		Disable OPPC, called before BTL_OPPC_Destroy.
 *
 * Description:
 *		Disable OPPC, called before BTL_OPPC_Destroy.
 *		If a connection exists, it will be disconnected automatically.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		oppcContext [in] - OPPC context.
 *
 * Generated Events:
 *      OPPC_EVENT_DISABLED
 *      OPPC_EVENT_TP_DISCONNECTED
 *      OPPC_EVENT_COMPLETE
 *      OPPC_EVENT_ABORTED   
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 *
 *		BT_STATUS_PENDING - The link is connected. The disconnect process has 
 *			been successfully started. When the disconnect process is complete, the
 *         	application callback will receive the disconnect event,
 *			which indicates the OPPC is disabled.
 */
BtStatus BTL_OPPC_Disable(BtlOppcContext *oppcContext);


/*-------------------------------------------------------------------------------
 * BTL_OPPC_Connect()
 *
 * Brief:  
 *      This function is used to create a transport connection.
 *
 * Description:
 *		This function is used by the client to create a transport connection
 *     	to the specified device and issue an OBEX Connect Request.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		oppcContext [in] - OPPC context.
 *
 *		bdAddr [in] - pointer to 48-bit address of the device.
 *
 * Generated Events:
 *      OPPC_EVENT_DISCOVERY_FAILED
 *      OPPC_EVENT_NO_SERVICE_FOUND
 *      OPPC_EVENT_TP_DISCONNECTED
 *      OPPC_EVENT_COMPLETE
 *
 * Returns:
 *		BT_STATUS_PENDING - Connection was successfully started. Completion
 *         	will be signaled via a call to the application callback.
 *
 *			If the connection is successful, a OPPC_EVENT_COMPLETE event for
 *         	the GOEP_OPER_CONNECT operation will be signaled.
 *
 *			If the transport connection is successful, but the OBEX Connect
 *         	failed, the completion event will be OPPC_EVENT_TP_DISCONNECTED
 *         	for the operation GOEP_OPER_CONNECT. At this point the transport
 *         	is DISCONNECTED.
 *
 *			If the transport connection is unsuccessful, the completion event
 *         	will be OPPC_EVENT_TP_DISCONNECTED, OPPC_EVENT_DISCOVERY_FAILED, or
 *         	OPPC_EVENT_NO_SERVICE_FOUND.
 *
 *		BT_STATUS_SUCCESS - The client is now connected.
 *
 *		BT_STATUS_FAILED -  Unable to start the operation.
 *
 *		BT_STATUS_BUSY - Operation was not started because
 *         the client is currently executing another operation.
 */
BtStatus BTL_OPPC_Connect(BtlOppcContext *oppcContext, 
                            const BD_ADDR *bdAddr);


/*-------------------------------------------------------------------------------
 * BTL_OPPC_Disconnect()
 *
 * Brief: 
 *      This function is used to issue an OBEX Disconnect Request.
 *
 * Description:
 *      This function is used by the client to issue an OBEX Disconnect Request.
 *     	When the OBEX Disconnect is completed, the transport connection will
 *     	be disconnected automatically.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *		oppcContext [in] - OPPC context.
 *
 * Generated Events:
 *      OPPC_EVENT_TP_DISCONNECTED
 *      
 * Returns:
 *      BT_STATUS_PENDING - Operation was successfully started.
 *         Completion will be signaled with the OPPC_EVENT_TP_DISCONNECTED 
 *         event to the application callback.
 *
 *      BT_STATUS_BUSY - Operation was not started because
 *         the client is currently executing another operation.
 *
 *		BT_STATUS_NO_CONNECTION -  Operation was not started because
 *         the client is not connected.
 */
BtStatus BTL_OPPC_Disconnect(BtlOppcContext *oppcContext);


/*-------------------------------------------------------------------------------
 * BTL_OPPC_GetConnectedDevice()
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
 *		oppcContext [in] - OPPC context.
 *
 *		bdAddr [out] - pointer to 48-bit address of the connected device.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *		BT_STATUS_NO_CONNECTION -  The client is not connected.
 */
BtStatus BTL_OPPC_GetConnectedDevice(BtlOppcContext *oppcContext, 
                                        BD_ADDR *bdAddr);


/*-------------------------------------------------------------------------------
 * BTL_OPPC_Push()
 *
 * Brief:  
 *      Initiates the OBEX "Put" operation to send an object to the remote server.
 *
 * Description:
 *      Initiates the OBEX "Put" operation to send an object to the remote server.
 *		This function can be called only after connecting to the server via 
 *		BTL_OPPC_Connect().
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *		oppcContext [in] - OPPC context.
 *
 *		objToPush [in] - Object to push (see BtlObject for details).
 *			The objToPush pointer is free after the function returns.
 *
 * Generated Events:
 *      OPPC_EVENT_COMPLETE
 *      OPPC_EVENT_OBJECT_PROGRESS
 *
 * Returns:
 *		BT_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via OPPC_EVENT_COMPLETE event to the 
 *         application callback.
 *
 *		BT_STATUS_BUSY - Operation was not started because
 *         the client is currently executing another operation.
 *
 *		BT_STATUS_NO_CONNECTION -  Operation was not started because
 *         the client is not connected.
 *
 *		BT_STATUS_FAILED -  Unable to start the operation.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_OPPC_Push(BtlOppcContext *oppcContext, 
                        const BtlObject *objToPush);


/*-------------------------------------------------------------------------------
 * BTL_OPPC_Pull()
 *
 * Brief:  
 *      Initiates basic pull operation.
 *
 * Description:
 *      Initiates the OBEX "Get" operation to retrieve an object from
 *     	the remote server.  
 *		This function can be called only after connecting to the server via 
 *		BTL_OPPC_Connect().
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *		oppcContext [in] - OPPC context.
 *
 *		objToPull [in] - Object to pull (see BtlObject for details).
 *			The objToPull pointer is free after the function returns.
 *
 * Generated Events:
 *      OPPC_EVENT_COMPLETE
 *      OPPC_EVENT_OBJECT_PROGRESS
 *
 * Returns:
 *		BT_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via OPPC_EVENT_COMPLETE event to the application callback.
 *
 *		BT_STATUS_BUSY - Operation was not started because
 *         the client is currently executing another operation.
 *
 *		BT_STATUS_NO_CONNECTION -  Operation was not started because
 *         the client is not connected.
 *
 *		BT_STATUS_FAILED -  Unable to start the operation.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_OPPC_Pull(BtlOppcContext *oppcContext, 
                       const BtlObject *objToPull);


/*-------------------------------------------------------------------------------
 * BTL_OPPC_Exchange()
 *
 * Brief: 
 *      Initiates basic exchange operation.
 *
 * Description:
 *      Initiates the OBEX "Put" operation to send an object to the remote server.
 *		Afterwards, it initiates the OBEX "Get" operation to retrieve an object 
 *		from the remote server.
 *		This function can be called only after connecting to the server via 
 *		BTL_OPPC_Connect().
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *		oppcContext [in] - OPPC context.
 *
 *		objToPush [in] - Object to push (see BtlObject for details).
 *			The objToPush pointer is free after the function returns.
 *
 *		objToPull [in] - Object to pull (see BtlObject for details).
 *			The objToPull pointer is free after the function returns.
 *
 * Generated Events:
 *      OPPC_EVENT_COMPLETE
 *      OPPC_EVENT_ABORTED      
 *      OPPC_EVENT_OBJECT_PROGRESS
 *   
 * Returns:
 *		BT_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via OPPC_EVENT_COMPLETE event to the application callback.
 *
 *		BT_STATUS_BUSY - Operation was not started because
 *         the client is currently executing another operation.
 *
 *		BT_STATUS_NO_CONNECTION -  Operation was not started because
 *         the client is not connected.
 *
 *		BT_STATUS_FAILED -  Unable to start the operation.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_OPPC_Exchange(BtlOppcContext *oppcContext, 
                            const BtlObject *objToPush,
                            const BtlObject *objToPull);


/*-------------------------------------------------------------------------------
 * BTL_OPPC_ConnectPushDisconnect()
 *
 * Brief:  
 *      Initiates encapsulated push operation.
 *		
 * Description:
 *      This function is used by the client to create a transport connection
 *     	to the specified device and issue an OBEX Connect Request.
 *		Then, It initiates the OBEX "Put" operation to send an object to the remote server.
 *		After the object is transmitted, it issue an OBEX Disconnect Request.
 *     	When the OBEX Disconnect is completed, the transport connection will
 *     	be disconnected automatically.
 *		This function is a combined operation of 3 functions: 
 *		BTL_OPPC_Connect(), BTL_OPPC_Push() and BTL_OPPC_Disconnect().
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *		oppcContext [in] - OPPC context.
 *
 *		bdAddr [in] - pointer to 48-bit address of the device.
 *
 *		objToPush [in] - Object to push (see BtlObject for details).
 *			The objToPush pointer is free after the function returns.
 *
 * Generated Events:
 *      OPPC_EVENT_DISCOVERY_FAILED
 *      OPPC_EVENT_NO_SERVICE_FOUND
 *      OPPC_EVENT_TP_DISCONNECTED
 *      OPPC_EVENT_COMPLETE
 *      OPPC_EVENT_ABORTED      
 *      OPPC_EVENT_OBJECT_PROGRESS
 *
 * Returns:
 *		BT_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via an event to the application callback.
 *
 *		BT_STATUS_BUSY - Operation was not started because
 *         the client is currently executing another operation.
 *
 *		BT_STATUS_FAILED -  Unable to start the operation.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_OPPC_ConnectPushDisconnect(BtlOppcContext *oppcContext, 
                                        const BD_ADDR *bdAddr, 
                                        const BtlObject *objToPush);


/*-------------------------------------------------------------------------------
 * BTL_OPPC_ConnectPullDisconnect()
 *
 * Brief:  
 *      Initiates encapsulated pull operation.
 *		
 * Description:
 *      This function is used by the client to create a transport connection
 *     	to the specified device and issue an OBEX Connect Request.
 *		Then, it initiates the OBEX "Get" operation to retrieve an object from
 *     	the remote server.
 *		After the object is transmitted, it issue an OBEX Disconnect Request.
 *     	When the OBEX Disconnect is completed, the transport connection will
 *     	be disconnected automatically.
 *		This function is a combined operation of 3 functions: 
 *		BTL_OPPC_Connect(), BTL_OPPC_Pull() and BTL_OPPC_Disconnect().
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *		oppcContext [in] - OPPC context.
 *
 *		bdAddr [in] - pointer to 48-bit address of the device.
 *
 *		objToPull [in] - Object to pull (see BtlObject for details).
 *			The objToPull pointer is free after the function returns.
 *
 * Generated Events:
 *      OPPC_EVENT_DISCOVERY_FAILED
 *      OPPC_EVENT_NO_SERVICE_FOUND
 *      OPPC_EVENT_TP_DISCONNECTED
 *      OPPC_EVENT_COMPLETE
 *      OPPC_EVENT_ABORTED      
 *      OPPC_EVENT_OBJECT_PROGRESS
 *
 * Returns:
 *		BT_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via OPPC_EVENT_TP_DISCONNECTED event to the application callback.
 *
 *		BT_STATUS_BUSY - Operation was not started because
 *         the client is currently executing another operation.
 *
 *		BT_STATUS_FAILED -  Unable to start the operation.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_OPPC_ConnectPullDisconnect(BtlOppcContext *oppcContext, 
                                        const BD_ADDR *bdAddr, 
                                        const BtlObject *objToPull);


/*-------------------------------------------------------------------------------
 * BTL_OPPC_ConnectExchangeDisconnect()
 *
 * Brief:  
 *      Initiates encapsulated exchange operation.
 *		
 * Description:
 *      This function is used by the client to create a transport connection
 *     	to the specified device and issue an OBEX Connect Request.
 *		Then, it initiates the OBEX "Put" operation to send an object to the remote server.
 *		Then, it initiates the OBEX "Get" operation to retrieve an object from
 *     	the remote server.
 *		After the objects are transmitted, it issue an OBEX Disconnect Request.
 *     	When the OBEX Disconnect is completed, the transport connection will
 *     	be disconnected automatically.
 *		This function is a combined operation of 4 functions: 
 *		BTL_OPPC_Connect(), BTL_OPPC_Push(), BTL_OPPC_Pull() and BTL_OPPC_Disconnect().
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *		oppcContext [in] - OPPC context.
 *
 *		bdAddr [in] - pointer to 48-bit address of the device.
 *
 *		objToPush [in] - Object to push (see BtlObject for details).
 *			The objToPush pointer is free after the function returns.
 *
 *		objToPull [in] - Object to pull (see BtlObject for details).
 *			The objToPull pointer is free after the function returns.
 *
 * Generated Events:
 *      OPPC_EVENT_DISCOVERY_FAILED
 *      OPPC_EVENT_NO_SERVICE_FOUND
 *      OPPC_EVENT_TP_DISCONNECTED
 *      OPPC_EVENT_COMPLETE
 *      OPPC_EVENT_ABORTED      
 *      OPPC_EVENT_OBJECT_PROGRESS
 *
 * Returns:
 *		BT_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via OPPC_EVENT_TP_DISCONNECTED event to the application callback.
 *
 *		BT_STATUS_BUSY - Operation was not started because
 *         the client is currently executing another operation.
 *
 *		BT_STATUS_FAILED -  Unable to start the operation.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_OPPC_ConnectExchangeDisconnect(BtlOppcContext *oppcContext, 
                                            const BD_ADDR *bdAddr, 
                                            const BtlObject *objToPush,
                                            const BtlObject *objToPull);


/*-------------------------------------------------------------------------------
 * BTL_OPPC_Abort()
 *
 * Brief:  
 *		Aborts the current client operation
 *
 * Description:
 *      Aborts the current client operation. The completion event will signal
 *     	the status of the operation in progress, either COMPLETE or ABORTED.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		oppcContext [in] - OPPC context.
 *
 * Generated Events:
 *      OPPC_EVENT_ABORTED      
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation was successful.
 *
 *		BT_STATUS_FAILED - Operation failed to start.
 *
 *		BT_STATUS_NO_CONNECTION -  Operation was not started because
 *         the client is not connected.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_OPPC_Abort(BtlOppcContext *oppcContext);


/*-------------------------------------------------------------------------------
 * BTL_OPPC_SetSecurityLevel()
 *
 * Brief:  
 *     	Sets security level for the given OPPC context.
 *
 * Description:
 *     	Sets security level for the given OPPC context.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	oppcContext [in] - pointer to the OPPC context.
 *
 *		securityLevel [in] - level of security which should be applied,	when
 *			connecting to a remote OPP server. For default value, pass null.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */
BtStatus BTL_OPPC_SetSecurityLevel(BtlOppcContext *oppcContext,
                                    const BtSecurityLevel *securityLevel);


/*-------------------------------------------------------------------------------
 * BTL_OPPC_GetSecurityLevel()
 *
 * Brief:  
 *     	Gets security level for the given OPPC context.
 *
 * Description:
 *     	Gets security level for the given OPPC context.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	oppcContext [in] - pointer to the OPPC context.
 *
 *		securityLevel [out] - level of security which should be applied, when
 *			connecting to a remote OPP server.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */
BtStatus BTL_OPPC_GetSecurityLevel(BtlOppcContext *oppcContext,
                                    BtSecurityLevel *securityLevel);


#endif /* __BTL_OPPC_H */



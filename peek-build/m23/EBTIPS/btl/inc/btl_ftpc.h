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
*   FILE NAME:      btl_ftpc.h
*
*   BRIEF:          This file defines the API of the BTL File Transfer Profile 
*					client role.
*
*   DESCRIPTION:    General
*
*                   The BTL_FTPC defines the API of the File Transfer profile (FTP) client role, 
*                   for the use of the application. Being a client API,	the main use of it 
*                   is basic initialization, connectivity, and support push/pull operations 
*                   to/from the remote server.
*
*					Usage Models
*
*                   The BTL_FTP client representation is an FTPC context (BtlFtpcContext).
*					The context has to be created and enabled in order to send remote 
*					connections and requests. (BTL_FTPC_Create, BTL_FTPC_Enable).
*					
*					The FTP client create connection and sends the requests to the remote server,
*					and is also capable of initiating a disconnect operation, or an 
*					abort operation. (BTL_FTPC_Connect,BTL_FTPC_Disconnect, BTL_FTPC_Abort).
*					
*					The application will be notified by events regarding the on-going
*					operations. See below list of events. The events callback function        
*					should be registered using the BTL_FTPC_Enable function.
*					
*					Additional API: set/get security, basic and encapsulated operations,
*					get remote device BD address.
*
*
*					Scenarios
*
*	                1. Object Push - push an object to remote server. 
*	                2. Object Pull - pull an object from remote server. 
*                   3. Object Delete - delete an object from remote server.
*					4. Browse server folders.
*					5. Create folders.
*                  
*                   Generated Events List
*
*                   FTPC_EVENT_AUTH_CHALLENGE
* 					FTPC_EVENT_COMPLETE
* 					FTPC_EVENT_ABORTED
* 					FTPC_EVENT_TP_CONNECTED           
* 					FTPC_EVENT_TP_DISCONNECTED
*					FTPC_EVENT_DISCOVERY_FAILED
* 					FTPC_EVENT_NO_SERVICE_FOUND
*                   FTPC_EVENT_OBJECT_PROGRESS
*                   FTPC_EVENT_DISABLED
*
*					Execution Flow
*
*					All FTP Client operations are asynchronous.
*					At the beginning of each operation, a call to is made to start the operation
*					and consecutive generated events (e.g. FTPC_EVENT_COMPLETE) are received by the 
*                   BtlFtpcCallback() to further support the operation.
*                            
*   AUTHOR:         Yoni Shavit
*
\*******************************************************************************/


#ifndef __BTL_FTPC_H
#define __BTL_FTPC_H


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "ftp.h"
#include "btl_common.h"
#include "ftpcstore.h"
#include "btl_unicode.h"

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/* Forward declarations */
typedef struct _FtpClientEvent  FtpClientEvent;
typedef struct _BtlFtpcEvent 	BtlFtpcEvent;
typedef struct _BtlFtpcContext 	BtlFtpcContext;


/*-------------------------------------------------------------------------------
 * Max file and path lengths 
 *
 *     Define the maximum file name and path length
 */
#define  FTPC_MAX_FILE_NAME_LENGTH  	BTHAL_FS_MAX_FILE_NAME_LENGTH
#define  FTPC_MAX_PATH_LENGTH 		 	BTHAL_FS_MAX_PATH_LENGTH

/*-------------------------------------------------------------------------------
 * BtlFtpcCallBack type
 *
 *     A function of this type is called to indicate BTL FTPC events.
 */
typedef void (*BtlFtpcCallBack)(const BtlFtpcEvent *event);

/*---------------------------------------------------------------------------
 * FtpcEvent type
 *
 *     The FtpcEvent type defines the events that may be indicated to
 *     the FTP client application.
 */
typedef U8 FtpcEvent;

#if OBEX_AUTHENTICATION == XA_ENABLED

/* 
 * Indicates that an authentication challenge request has
 * been received and is available in the event challenge field. If
 * the application intends on responding to this challenge it should do
 * so during this event notification. This is because there is a risk
 * that the peer may send more than one challenge and the GOEP layer
 * only tracks the last challenge received. However, it is not required
 * that the application respond during the event, as long as the
 * application always responds to the last challenge received.
 */
#define FTPC_EVENT_AUTH_CHALLENGE   GOEP_EVENT_AUTH_CHALLENGE

#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

/* 
 * Indicates that the operation has completed successfully.
 */
#define FTPC_EVENT_COMPLETE         GOEP_EVENT_COMPLETE

/* 
 * Indicates that the current operation has failed.
 */
#define FTPC_EVENT_ABORTED          GOEP_EVENT_ABORTED

/* 
 * A transport layer connection has been established. There is no
 * operation associated with this event.
 */
#define FTPC_EVENT_TP_CONNECTED     GOEP_EVENT_TP_CONNECTED

/* 
 * The transport layer connection has been disconnected. There is no
 * operation associated with this event.
 */
#define FTPC_EVENT_TP_DISCONNECTED  GOEP_EVENT_TP_DISCONNECTED

#define FTPC_EVENT_DISCOVERY_FAILED	GOEP_EVENT_DISCOVERY_FAILED

/* 
 * The transport layer OBEX service cannot be found. (Client only)
 */
#define FTPC_EVENT_NO_SERVICE_FOUND GOEP_EVENT_NO_SERVICE_FOUND

/* 
 * Delivered to applications on each progress change.
 */
#define FTPC_EVENT_OBJECT_PROGRESS	15

/*
 * This event is received when all streams have been closed.
 */
#define FTPC_EVENT_DISABLED			16

/* End of FtpcEvent */

/*---------------------------------------------------------------------------
 * FtpsOperation type
 *
 *     The FtpsOperation type defines operations based on the Generic
 *     Object Exchange Profile's operations.
 */
typedef U8 FtpcOperation;

/* 
 * This value is used when indicating events which do not occur during
 * an operation, such as TP Connect and TP Disconnect.
 */
#define FTPC_OPER_NONE              GOEP_OPER_NONE

/* 
 * A Connect operation is in progress. 
 */
#define FTPC_OPER_CONNECT           GOEP_OPER_CONNECT

/* 
 * A Disconnect operation is in progress. 
 */
#define FTPC_OPER_DISCONNECT        GOEP_OPER_DISCONNECT

/* 
 * A Push operation is in progress. 
 */
#define FTPC_OPER_PUSH              GOEP_OPER_PUSH

/* 
 * A Delete operation is in progress. 
 */
#define FTPC_OPER_DELETE            GOEP_OPER_DELETE

/* 
 * A Pull operation is in progress. 
 */
#define FTPC_OPER_PULL              GOEP_OPER_PULL

/* 
 * A Set folder operation is in progress. 
 */
#define FTPC_OPER_SETFOLDER         GOEP_OPER_SETFOLDER

/* 
 * An abort operation is in progress. This event will be delivered while
 * another operation is in progress, if the ABORT is sent to stop that
 * operation.
 */
#define FTPC_OPER_ABORT             GOEP_OPER_ABORT


/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/

/*---------------------------------------------------------------------------
 * FtpcProgressInd structure
 * 
 *     This structure is used during progress indications.
 */
typedef struct _FtpcProgressInd 
{
	/* Name of the object, null-terminated. */
	BtlUtf8					*name;

	/* Length (in bytes) of the 'name' field. */
	U16						nameLen;

	/* Current position (in bytes) of the operation. */
    U32						currPos;

	/* Max position (in bytes) of the operation. */
	U32						maxPos;
} FtpcProgressInd;


/*-------------------------------------------------------------------------------
 * FtpClientEvent structure
 *
 *     Represents FTP Client event.
 */
struct _FtpClientEvent
{
	/* FTPC EVENT */
	FtpcEvent           		event;

	/* FTPC OPER */
	FtpcOperation       		oper;

	/* FTPC response code. Used in case of the server aborted the operation */
	ObexRespCode 				reason;

	union 
	{
		FtpcProgressInd      	progress;  		/* Use during FTPC_EVENT_OBJECT_PROGRESS */
#if OBEX_AUTHENTICATION == XA_ENABLED
	    ObexAuthChallengeInfo   challenge;  	/* Use during FTPC_EVENT_AUTH_CHALLENGE */
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
	} info;
};

/*-------------------------------------------------------------------------------
 * BtlFtpcEvent structure
 *
 *     Represents BTL FTPC event.
 */
struct _BtlFtpcEvent 
{
	BtlFtpcContext		*ftpcContext;	
	FtpClientEvent		*ftpcEvent;	
};


/********************************************************************************
 *
 * Function declarations
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BTL_FTPC_Init()
 *
 * Brief:  
 *		Init the FTPC module.
 *
 * Description:
 *		Init the FTPC module.
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
BtStatus BTL_FTPC_Init(void);


/*-------------------------------------------------------------------------------
 * BTL_FTPC_Deinit()
 *
 * Brief:  
 *		Deinit the FTPC module.
 *
 * Description:
 *		Deinit the FTPC module.
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
BtStatus BTL_FTPC_Deinit(void);

 
/*-------------------------------------------------------------------------------
 * BTL_FTPC_Create()
 *
 * Brief:  
 *		Allocates a unique FTPC context.
 *
 * Description:
 *		Allocates a unique FTPC context.
 *		This function must be called before any other FTPC API function.
 *		The allocated context should be supplied in subsequent FTPC API calls.
 *		The caller must also provide a callback function, which will be called 
 *		on FTPC events.
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
 *		ftpcCallback [in] - all FTPC events will be sent to this callback.
 *
 *		securityLevel [in] - level of security which should be applied,	when
 *			connecting to a remote FTP server. For default value, pass null.
 *		
 *		ftpcContext [out] - allocated FTPC context.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - FTPC context was created successfully.
 *
 *		BT_STATUS_FAILED -  Failed to create FTPC context.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *		BT_STATUS_NO_RESOURCES - No more resources.
 *
 *		BT_STATUS_INTERNAL_ERROR - Internal error has occur.
 */
BtStatus BTL_FTPC_Create(BtlAppHandle *appHandle,
						const BtlFtpcCallBack ftpcCallback,
						const BtSecurityLevel *securityLevel,
						BtlFtpcContext **ftpcContext);


/*-------------------------------------------------------------------------------
 * BTL_FTPC_Destroy()
 *
 * Brief:  
 *		Releases a FTPC context (previously allocated with BTL_FTPC_Create).
 *
 * Description:
 *		Releases a FTPC context (previously allocated with BTL_FTPC_Create).
 *		An application should call this function when it completes using FTPC services.
 *		Upon completion, the FTPC context is set to null in order to prevent 
 *		the application from an illegal attempt to keep using it.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		ftpcContext [in/out] - FTPC context.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - FTPC context was destroyed successfully.
 *
 *		BT_STATUS_IN_USE -  FTPC context is in use.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *		BT_STATUS_INTERNAL_ERROR - Internal error has occur.
 */
BtStatus BTL_FTPC_Destroy(BtlFtpcContext **ftpcContext);


/*-------------------------------------------------------------------------------
 * BTL_FTPC_Enable()
 *
 * Brief:  
 *		Enable FTPC, called after BTL_FTPC_Create.
 *
 * Description:
 *		Enable FTPC, called after BTL_FTPC_Create.
 *		After calling this function, FTP client is ready for usage.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		ftpcContext [in] - FTPC context.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */
BtStatus BTL_FTPC_Enable(BtlFtpcContext *ftpcContext);


/*-------------------------------------------------------------------------------
 * BTL_FTPC_Disable()
 *
 * Brief:  
 *		Disable FTPC, called before BTL_FTPC_Destroy.
 *
 * Description:
 *		Disable FTPC, called before BTL_FTPC_Destroy.
 *		If a connection exists, it will be disconnected automatically.
 *		
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		ftpcContext [in] - FTPC context.
 *
 * Generated Events:
 *      FTPC_EVENT_DISCONNECT
 *      FTPC_EVENT_DISABLED
 *      FTPC_EVENT_COMPLETE
 *		FTPC_EVENT_ABORTED
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 *
 *		BT_STATUS_PENDING - At least one client is connected. The disable process has 
 *			been successfully started. On each connected client, the application callback 
 *			will receive FTPC_EVENT_DISCONNECT event.
 *			When the disable process is complete, the application callback will 
 *			receive the FTPC_EVENT_DISABLED event, indicating the context is disabled.
 */
BtStatus BTL_FTPC_Disable(BtlFtpcContext *ftpcContext);


/*-------------------------------------------------------------------------------
 * BTL_FTPC_Connect()
 *
 * Brief:  
 *		This function is used by the client to create a transport connection.
 *
 * Description:
 *		This function is used by the client to create a transport connection
 *     	to the specified device and issue an OBEX Connect Request.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *		ftpcContext [in] - FTPC context.
 *
 *		bdAddr [in] - pointer to 48-bit address of the device.
 *
 * Generated Events:
 *      FTPC_EVENT_ABORTED 
 *      FTPC_EVENT_TP_DISCONNECTED
 *      FTPC_EVENT_DISCOVERY_FAILED
 *		FTPC_EVENT_NO_SERVICE_FOUND
 *      FTPC_EVENT_COMPLETE
 *
 * Returns:
 *		BT_STATUS_PENDING - Connection was successfully started. Completion
 *         will be signaled by an event to the application callback. 
 *
 *         If the connection is successful, a FTPC_EVENT_COMPLETE event for
 *         the FTPC_OPER_CONNECT operation will be signaled.
 *
 *         If the transport connection is successful, but the OBEX Connect
 *         failed, the completion event will be FTPC_EVENT_ABORTED for the
 *         operation FTPC_OPER_CONNECT. At this point the transport is
 *         CONNECTED but a successful OBEX Connect has not been sent.
 *         Additional calls to this function will generate new OBEX Connect
 *         requests. 
 * 
 *         If the transport connection is unsuccessful, the completion event
 *         will be FTPC_EVENT_TP_DISCONNECTED, FTPC_EVENT_DISCOVERY_FAILED, or
 *         FTPC_EVENT_NO_SERVICE_FOUND.
 *
 *		BT_STATUS_SUCCESS - The client is now connected.
 *
 *		BT_STATUS_FAILED -  Unable to start the operation because the client
 *         is in the middle of starting up a connection.
 *
 *		BT_STATUS_BUSY - The client is currently executing an operation.
 */
BtStatus BTL_FTPC_Connect(BtlFtpcContext *ftpcContext, 
							const BD_ADDR *bdAddr);


/*-------------------------------------------------------------------------------
 * BTL_FTPC_Disconnect()
 *
 * Brief:  
 *		This function is used by the client to issue an OBEX Disconnect Request.
 *
 * Description:
 *		This function is used by the client to issue an OBEX Disconnect Request.
 *     	When the OBEX Disconnect is completed, the transport connection will
 *     	be disconnected automatically.
 *		
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *		ftpcContext [in] - FTPC context.
 *
 * Generated Events:
 *      FTPC_EVENT_TP_DISCONNECTED
 *    
 * Returns:
 *		BT_STATUS_PENDING - Operation was successfully started.
 *         Completion will be signaled with the FTPC_EVENT_TP_DISCONNECTED 
 *         event to the application callback.
 *
 *		BT_STATUS_BUSY - Operation was not started because
 *         the client is currently executing another operation.
 *
 *		BT_STATUS_NO_CONNECTION - Operation was not started because
 *         the client is not connected.
 */
BtStatus BTL_FTPC_Disconnect(BtlFtpcContext *ftpcContext);


/*-------------------------------------------------------------------------------
 * BTL_FTPC_GetConnectedDevice()
 *
 * Brief:  
 *		This function returns the connected device.
 *
 * Description:
 *		This function returns the connected device.
 *		
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		ftpcContext [in] - FTPC context.
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
BtStatus BTL_FTPC_GetConnectedDevice(BtlFtpcContext *ftpcContext, 
										BD_ADDR *bdAddr);


/*-------------------------------------------------------------------------------
 * BTL_FTPC_Push()
 *
 * Brief:  
 *		Initiates the OBEX "Put" operation to send an object to the remote server.
 *
 * Description:
 *		Initiates the OBEX "Put" operation to send an object to the remote server.
 *		This function can be called only after connecting to the server via 
 *		BTL_FTPC_Connect().
 *		
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *		ftpcContext [in] - FTPC context.
 *
 *		objToPush [in] - Object to push (see BtlObject for details).
 *			The objToPush pointer is free after the function returns.
 *
 * Generated Events:
 *      FTPC_EVENT_COMPLETE
 *      FTPC_EVENT_OBJECT_PROGRESS
 *
 * Returns:
 *		BT_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via an event to the application callback.
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
BtStatus BTL_FTPC_Push(BtlFtpcContext *ftpcContext,
						const BtlObject *objToPush);


/*-------------------------------------------------------------------------------
 * BTL_FTPC_Pull()
 *
 * Brief:  
 *		Initiates the OBEX "Get" operation to retrieve an object from the remote server.
 *
 * Description:
 *		Initiates the OBEX "Get" operation to retrieve an object from the remote server.
 *     	This function can be called only after connecting to the server via 
 *		BTL_FTPC_Connect().
 *		
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *		ftpcContext [in] - FTPC context.
 *
 *		objToPull [in] - Object to pull (see BtlObject for details).
 *			The objToPull pointer is free after the function returns.
 *
 * Generated Events:
 *      FTPC_EVENT_COMPLETE
 *      FTPC_EVENT_OBJECT_PROGRESS
 *
 * Returns:
 *		BT_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via an event to the application callback.
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
BtStatus BTL_FTPC_Pull(BtlFtpcContext *ftpcContext, 
						const BtlObject *objToPull);


/*-------------------------------------------------------------------------------
 * BTL_FTPC_Delete()
 *
 * Brief:  
 *		Initiates the OBEX "Put" operation to delete an object on the remote server.
 *
 * Description:
 *		Initiates the OBEX "Put" operation to delete an object on the remote server.
 *     	This function can be called only after connecting to the server via 
 *		BTL_FTPC_Connect().
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *		ftpcContext [in] - FTPC context.
 *
 *		objToDelete [in] - Object to delete.
 *			The objToDelete pointer is free after the function returns.
 *
 * Generated Events:
 *      FTPC_EVENT_COMPLETE
 *
 * Returns:
 *		BT_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via an event to the application callback.
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
BtStatus BTL_FTPC_Delete(BtlFtpcContext *ftpcContext, 
							const BtlUtf8 *objToDelete);



/*-------------------------------------------------------------------------------
 * BTL_FTPC_PullListFolder()
 *
 * Brief:  
 *		Initiates the OBEX "Get" operation to retrieve the contents of the given 
 *		folder from	the remote server.
 *
 * Description:
 *		Initiates the OBEX "Get" operation to retrieve the contents of the given 
 *		folder from	the remote server.
 *		The event will provide the XML data representing the contents of the 
 *		folder, which can be parsed via BTL_FTPC_GetParsedFolderListingFirst() and
 *		BTL_FTPC_GetParsedFolderListingNext.
 *     	This function can be called only after connecting to the server via 
 *		BTL_FTPC_Connect().
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *		ftpcContext [in] - FTPC context.
 *
 *		folderListing [in] - Determines the server folder path which its contents will be retrieved, 
 *							 and the memory location for saving the received list. (see BtlObject 
 *							 for details). 
 *
 * Generated Events:
 *      FTPC_EVENT_COMPLETE
 *      FTPC_EVENT_OBJECT_PROGRESS
 *
 * Returns:
 *		BT_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via an event to the application callback.
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
BtStatus BTL_FTPC_PullListFolder(BtlFtpcContext *ftpcContext, 
								const BtlObject *folderListing);


/*-------------------------------------------------------------------------------
 * BTL_FTPC_SetFolder()
 *
 * Brief:  
 *		Performs the OBEX "SetPath" operation to a new folder.
 *
 * Description:
 *		Performs the OBEX "SetPath" operation to a new folder.
 *		If the given parameter 'allowCreate' is TRUE, then this function creates
 *		a new folder.
 *     	This function can be called only after connecting to the server via 
 *		BTL_FTPC_Connect().
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *		ftpcContext [in] - FTPC context.
 *
 *		folderName [in] - The name of the child folder to change to/to create.
 *			The folderName pointer is free after the function returns.
 *
 *		allowCreate [in] - Set to TRUE to allow the server to create the folder
 *          if it does not exist (like a create folder function).
 *
 * Generated Events:
 *      FTPC_EVENT_COMPLETE
 *
 * Returns:
 *		BT_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via an event to the application callback.
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
BtStatus BTL_FTPC_SetFolder(BtlFtpcContext *ftpcContext, 
							const BtlUtf8 *folderName, 
							BOOL allowCreate);


/*-------------------------------------------------------------------------------
 * BTL_FTPC_SetFolderBackUp()
 *
 * Brief:  
 *		Performs the OBEX "SetPath" operation to backup folder.
 *
 * Description:
 *		Performs the OBEX "SetPath" operation to backup folder.
 *     	This function can be called only after connecting to the server via 
 *		BTL_FTPC_Connect().
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *		ftpcContext [in] - FTPC context.
 *
 * Generated Events:
 *      FTPC_EVENT_COMPLETE
 *
 * Returns:
 *		BT_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via an event to the application callback.
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
BtStatus BTL_FTPC_SetFolderBackUp(BtlFtpcContext *ftpcContext);


/*-------------------------------------------------------------------------------
 * BTL_FTPC_SetFolderRoot()
 *
 * Brief:  
 *		Performs the OBEX "SetPath" operation to change to the root folder.
 *
 * Description:
 *		Performs the OBEX "SetPath" operation to change to the root folder.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *		ftpcContext [in] - FTPC context.
 *
 * Generated Events:
 *      FTPC_EVENT_COMPLETE
 *
 * Returns:
 *		BT_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via an event to the application callback.
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
BtStatus BTL_FTPC_SetFolderRoot(BtlFtpcContext *ftpcContext);


/*-------------------------------------------------------------------------------
 * BTL_FTPC_Abort()
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
 *		ftpcContext [in] - FTPC context.
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
BtStatus BTL_FTPC_Abort(BtlFtpcContext *ftpcContext);


/*-------------------------------------------------------------------------------
 * BTL_FTPC_ConfigInitiatingObexAuthentication()
 *
 * Brief:  
 *		Enable or disable client initiating OBEX authentication.
 *
 * Description:
 *		Enable or disable client initiating OBEX authentication, when connecting 
 *		to FTP server.
 *		In both cases, the client will still respond to OBEX authentication 
 *		initiated by the server (via BTL_FTPC_ObexAuthenticationResponse()).
 *		Client initiating OBEX authentication is disabled by default.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		ftpcContext [in] - FTPC context.
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
BtStatus BTL_FTPC_ConfigInitiatingObexAuthentication(BtlFtpcContext *ftpcContext, 
													const BOOL		enableObexAuth,
													const BtlUtf8	*realm, 
													const BtlUtf8	*userId, 
													const BtlUtf8	*password);


/*-------------------------------------------------------------------------------
 * BTL_FTPC_ObexAuthenticationResponse()
 *
 * Brief:  
 *		This function must be called after receiving OBEX authentication 
 *		challenge from the server.
 *
 * Description:
 *		This function must be called after receiving OBEX authentication 
 *		challenge from the server.
 *		the client create connection with the authentication response.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		ftpcContext [in] - FTPC context.
 *
 *		userId [in] - null-terminated string used with OBEX authentication 
 *			representing the user id. If the argument is not needed by the 
 *			server (see in event), NULL can be passed.
 *
 *		password [in] - null-terminated string used with OBEX authentication 
 *			representing the password. Pass NULL to deny authentication.
 *
 * Generated Events:
 *      FTPC_EVENT_ABORTED 
 *      FTPC_EVENT_TP_DISCONNECTED
 *      FTPC_EVENT_DISCOVERY_FAILED
 *		FTPC_EVENT_NO_SERVICE_FOUND
 *      FTPC_EVENT_COMPLETE
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - No OBEX auth challenge was received.
 *
 *		BT_STATUS_INTERNAL_ERROR - Failed building authentication response header.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_FTPC_ObexAuthenticationResponse(BtlFtpcContext *ftpcContext, 
												const BtlUtf8 *userId, 
												const BtlUtf8 *password);


/*-------------------------------------------------------------------------------
 * BTL_FTPC_SetSecurityLevel()
 *
 * Brief:  
 *     	Sets security level for the given FTPC context.
 *
 * Description:
 *     	Sets security level for the given FTPC context.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	ftpcContext [in] - pointer to the FTPC context.
 *
 *		securityLevel [in] - level of security which should be applied,	when
 *			connecting to a remote FTP server. For default value, pass null.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */
BtStatus BTL_FTPC_SetSecurityLevel(BtlFtpcContext *ftpcContext,
								  	const BtSecurityLevel *securityLevel);


/*-------------------------------------------------------------------------------
 * BTL_FTPC_GetSecurityLevel()
 *
 * Brief:  
 *     	Gets security level for the given FTPC context.
 *
 * Description:
 *     	Gets security level for the given FTPC context.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	ftpcContext [in] - pointer to the FTPC context.
 *
 *		securityLevel [out] - level of security which should be applied, when
 *			connecting to a remote FTP server.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */
BtStatus BTL_FTPC_GetSecurityLevel(BtlFtpcContext *ftpcContext,
								  	BtSecurityLevel *securityLevel);


#endif /* __BTL_FTPC_H */



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
*   FILE NAME:      btl_ftps.h
*
*   BRIEF:          This file defines the API of the BTL File Transfer Profile 
*					server role.
*
*   DESCRIPTION:    General
*
*                   The BTL_FTPS defines the API of the File Transfer profile (FTP) server role, 
*                   for the use of the application. Being a server API,	the main use of it 
*                   is basic initialization, connectivity, and support push/pull operations initiated
*                   by remote client.
*
*					Usage Models
*
*                   The BTL_FTP server representation is an FTPS context (BtlFtpsContext).
*					The context has to be created and enabled in order to receive remote 
*					connections and requests. (BTL_FTPS_Create, BTL_FTPS_Enable).
*					
*					The FTP server handles the requests initiated by the remote client,
*					and is also capable of initiating a disconnect operation, or an 
*					abort operation. (BTL_FTPS_Disconnect, BTL_FTPS_Abort).
*					
*					The application will be notified by events regarding the on-going
*					operations. See below list of events. The events callback function        
*					should be registered using the BTL_FTPS_Enable function.
*
*					Additional API: set/get security, configure OBEX authentication,
*					get remote device BD address.
*
*					Supported Features
*
*                   1. Service Discovery - Provide information on supported content types on SDP request.
*                   2. User Accept/Reject notifications for Pulled Object.
*                   3. User Accept/Reject notifications Pushed Object.
*                   4. User Accept/Reject notifications Delete Object.
*                   5. Enbale/Disable Read only mode.
*                   6. Enbale/Disable Hide objects.
*  
*    
*                   Generated Events List
*
*                   FTPS_EVENT_START
* 					FTPS_EVENT_AUTH_CHALLENGE
* 					FTPS_EVENT_COMPLETE
* 					FTPS_EVENT_ABORTED           
* 					FTPS_EVENT_TP_CONNECTED
*					FTPS_EVENT_TP_DISCONNECTED
* 					FTPS_EVENT_OBJECT_PROGRESS
*                   FTPS_EVENT_OBJECT_REQUEST
*					FTPS_EVENT_DISABLED
*  
*					Execution Flow
*
*                   The FTP remote client initiates an operation. The application 
*					is notified by an event. The request is handled internally in
*					the BTL layer by accessing the BTHAL_FS. At the end of the operation
*					the application is also notified by an event.
*                   
*   AUTHOR:         Ilan Elias
*
\*******************************************************************************/

#ifndef __BTL_FTPS_H
#define __BTL_FTPS_H


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "ftp.h"
#include "goep.h"
#include "btl_common.h"
#include "btl_unicode.h"

/********************************************************************************
 *
 * Constants
 *
 *******************************************************************************/


/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/* Forward declarations */
typedef struct _FtpServerEvent 	FtpServerEvent;
typedef struct _BtlFtpsEvent 	BtlFtpsEvent;
typedef struct _BtlFtpsContext 	BtlFtpsContext;
 
/*-------------------------------------------------------------------------------
 * BtlFtpsCallBack type
 *
 *     A function of this type is called to indicate BTL FTPS events.
 */
typedef void (*BtlFtpsCallBack)(const BtlFtpsEvent *event);


/*-------------------------------------------------------------------------------
 * BtlFtpsObjectRequestMask type
 *
 *     	Defines which object requests are delivered to the application callback, 
 *		can be ORed together.
 */
typedef U8 BtlFtpsObjectRequestMask;

#define BTL_FTPS_OBJECT_REQUEST_NONE			(0x00)	/* No request is delivered to app */
#define BTL_FTPS_OBJECT_REQUEST_PUSH			(0x01)	/* Client push an object */
#define BTL_FTPS_OBJECT_REQUEST_PULL			(0x02)	/* Client pull an object */
#define BTL_FTPS_OBJECT_REQUEST_DELETE			(0x04)	/* Client delete an object */
#define BTL_FTPS_OBJECT_REQUEST_CREATE_FOLDER	(0x08)	/* Client create a new folder */
#define BTL_FTPS_OBJECT_REQUEST_ALL				(0xFF)	/* All requests are delivered to app */

/*-------------------------------------------------------------------------------
 * BtlFtpsObjectRequestType type
 *
 *     	Defines which object delete type are delivered to the application callback, 
 */
typedef U8 BtlFtpsObjectType;

#define BTL_FTPS_OBJECT_FOLDER			(0x01)	/* folder request */
#define BTL_FTPS_OBJECT_FILE			(0x02)	/* file request */


/*---------------------------------------------------------------------------
 * FtpsEvent type
 *
 *     The FtpsEvent type defines the events that may be indicated to
 *     the FTP Server application.
 */
typedef U8 FtpsEvent;

/* 
 * Indicates the start of a new operation. In the case of an Abort
 * operation, this event may occur during another pending operation (Put or Get).
 */
#define FTPS_EVENT_START            GOEP_EVENT_START

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
#define FTPS_EVENT_AUTH_CHALLENGE   GOEP_EVENT_AUTH_CHALLENGE

#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

/* 
 * Indicates that the operation has completed successfully.
 */
#define FTPS_EVENT_COMPLETE         GOEP_EVENT_COMPLETE

/* 
 * Indicates that the current operation has failed.
 */
#define FTPS_EVENT_ABORTED          GOEP_EVENT_ABORTED

/* 
 * A transport layer connection has been established. There is no
 * operation associated with this event.
 */
#define FTPS_EVENT_TP_CONNECTED     GOEP_EVENT_TP_CONNECTED

/* 
 * The transport layer connection has been disconnected. There is no
 * operation associated with this event.
 */
#define FTPS_EVENT_TP_DISCONNECTED  GOEP_EVENT_TP_DISCONNECTED

/* 
 * Delivered to applications on each progress change.
 */
#define FTPS_EVENT_OBJECT_PROGRESS	15

/* 
 * Delivered to applications on each client request.
 * Configure which requests are delivered to the application via 
 * BTL_FTPS_ConfigObjectRequest.
 */
#define FTPS_EVENT_OBJECT_REQUEST	16

/*
 * This event is received when all streams have been closed.
 */
#define FTPS_EVENT_DISABLED			17

/* End of FtpsEvent */


/*---------------------------------------------------------------------------
 * FtpsOperation type
 *
 *     The FtpsOperation type defines operations based on the Generic
 *     Object Exchange Profile's operations.
 */
typedef U8 FtpsOperation;

/* 
 * This value is used when indicating events which do not occur during
 * an operation, such as TP Connect and TP Disconnect.
 */
#define FTPS_OPER_NONE              GOEP_OPER_NONE

/* 
 * A Connect operation is in progress.
 */
#define FTPS_OPER_CONNECT           GOEP_OPER_CONNECT

/* 
 * A Disconnect operation is in progress. 
 */
#define FTPS_OPER_DISCONNECT        GOEP_OPER_DISCONNECT

/* 
 * A Push operation is in progress.
 */
#define FTPS_OPER_PUSH              GOEP_OPER_PUSH

/* 
 * A Delete operation is in progress.
 */
#define FTPS_OPER_DELETE            GOEP_OPER_DELETE

/* 
 * A Pull operation is in progress.
 */
#define FTPS_OPER_PULL              GOEP_OPER_PULL

/* 
 * A Set folder operation is in progress.
 */
#define FTPS_OPER_SETFOLDER         GOEP_OPER_SETFOLDER

/* 
 * An abort operation is in progress. This event will be delivered while
 * another operation is in progress, if the ABORT is sent to stop that
 * operation.
 */
#define FTPS_OPER_ABORT             GOEP_OPER_ABORT

/* End of FtpsOperation */


/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/

/*---------------------------------------------------------------------------
 * FtpsRequestInd structure
 * 
 *     This structure is used during request indications.
 */
typedef struct _FtpsRequestInd 
{	
	/* Name of the object, null-terminated. */
	BtlUtf8					*name;

	/* Length (in bytes) of the 'name' field. */
	U16						nameLen;

	/* Type of the object, null-terminated. */
	char					*type;

	/* Length (in bytes) of the 'type' field. */
	U16						typeLen;

	/* Length (in bytes) of the requested object (relevant in pull and push requests only) */
	U32						objectLen;

	/* Object type File / Folder */
	BtlFtpsObjectType		objectType;
	
} FtpsRequestInd;


/*---------------------------------------------------------------------------
 * FtpsProgressInd structure
 * 
 *     This structure is used during progress indications.
 */
typedef struct _FtpsProgressInd 
{
	/* Name of the object, null-terminated. */
	BtlUtf8					*name;

	/* Length (in bytes) of the 'name' field. */
	U16						nameLen;

	/* Current position (in bytes) of the operation. */
    U32						currPos;

	/* Max position (in bytes) of the operation. */
	U32						maxPos;
} FtpsProgressInd;


/*-------------------------------------------------------------------------------
 * FtpServerEvent structure
 *
 *     Represents FTP Server event.
 */
struct _FtpServerEvent
{
	/* FTPS_EVENT_xxx */
	FtpsEvent           		event;

	/* FTPS_OPER_xxx */
	FtpsOperation       		oper;

	union 
	{
		FtpsRequestInd    		request;  		/* Use during FTPS_EVENT_OBJECT_REQUEST */
		FtpsProgressInd    		progress;  		/* Use during FTPS_EVENT_OBJECT_PROGRESS */
#if OBEX_AUTHENTICATION == XA_ENABLED
	    ObexAuthChallengeInfo   challenge;  	/* Use during FTPS_EVENT_AUTH_CHALLENGE */
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
    } info;
};


/*-------------------------------------------------------------------------------
 * BtlFtpsEvent structure
 *
 *     Represents BTL FTPS event.
 */
struct _BtlFtpsEvent 
{
	/* Associated context */
	BtlFtpsContext				*ftpsContext;

	/* FTP Server event */
	FtpServerEvent				*ftpsEvent;
};

/********************************************************************************
 *
 * Function declarations
 *
 *******************************************************************************/


/*-------------------------------------------------------------------------------
 * BTL_FTPS_Init()
 *
 * Brief:  
 *		Init the FTPS module.
 *
 * Description:
 *      Init the FTPS module.
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
BtStatus BTL_FTPS_Init(void);


/*-------------------------------------------------------------------------------
 * BTL_FTPS_Deinit()
 *
 * Brief:  
 *		Deinit the FTPS module.
 *
 * Description:
 *      Deinit the FTPS module.
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
BtStatus BTL_FTPS_Deinit(void);

 
/*-------------------------------------------------------------------------------
 * BTL_FTPS_Create()
 *
 * Brief:  
 *		Allocates a unique FTPS context.
 *
 * Description:
 *		Allocates a unique FTPS context.
 *		This function must be called before any other FTPS API function.
 *		The allocated context should be supplied in subsequent FTPS API calls.
 *		The caller must also provide a callback function, which will be called 
 *		on FTPS events.
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
 *		ftpsCallback [in] - all FTPS events will be sent to this callback.
 *
 *		securityLevel [in] - level of security which should be applied,	when
 *			connecting to a remote FTP client. 
 *			If NULL is passed, default value is used.
 *		
 *		ftpsContext [out] - allocated FTPS context.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - FTPS context was created successfully.
 *
 *		BT_STATUS_FAILED -  Failed to create FTPS context.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *		BT_STATUS_NO_RESOURCES - No more resources.
 *
 *		BT_STATUS_INTERNAL_ERROR - Internal error has occur.
 */
BtStatus BTL_FTPS_Create(BtlAppHandle *appHandle,
						const BtlFtpsCallBack ftpsCallback,
						const BtSecurityLevel *securityLevel,
						BtlFtpsContext **ftpsContext);


/*-------------------------------------------------------------------------------
 * BTL_FTPS_Destroy()
 *
 * Brief:  
 *		Releases a FTPS context (previously allocated with BTL_FTPS_Create).
 *
 * Description:
 *		Releases a FTPS context (previously allocated with BTL_FTPS_Create).
 *		An application should call this function when it completes using FTPS services.
 *		Upon completion, the FTPS context is set to null in order to prevent 
 *		the application from an illegal attempt to keep using it.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		ftpsContext [in/out] - FTPS context.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - FTPS context was destroyed successfully.
 *
 *		BT_STATUS_IN_USE -  FTPS context is in use.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *		BT_STATUS_INTERNAL_ERROR - Internal error has occur.
 */
BtStatus BTL_FTPS_Destroy(BtlFtpsContext **ftpsContext);


/*-------------------------------------------------------------------------------
 * BTL_FTPS_Enable()
 *
 * Brief:  
 *		Enable FTPS and register FTP server SDP record, called after 
 *		BTL_FTPS_Create.
 *
 * Description:
 *		Enable FTPS and register FTP server SDP record, called after 
 *		BTL_FTPS_Create.
 *		After calling this function, FTP server is ready for usage.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		ftpsContext [in] - FTPS context.
 *
 *		serviceName [in] - null- terminated service name which will be written into 
 *			SDP service record and will be discovered by the peer device.
 *			If NULL is passed, default value is used.
 *
 *		rootPath [in] - null- terminated string, representing the path of the 
 *							  root folder. Max root path length is PATHMGR_MAX_PATH.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */
BtStatus BTL_FTPS_Enable(BtlFtpsContext *ftpsContext, 
							const BtlUtf8 *serviceName,
							const BtlUtf8 *rootPath);


/*-------------------------------------------------------------------------------
 * BTL_FTPS_Disable()
 *
 * Brief:  
 *		Disable FTPS, called before BTL_FTPS_Destroy.
 *
 * Description:
 *		Disable FTPS, called before BTL_FTPS_Destroy.
 *		If a connection exists, it will be disconnected automatically.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		ftpsContext [in] - FTPS context.
 *
 * Generated Events:
 *      FTPS_EVENT_DISCONNECT
 *      FTPS_EVENT_DISABLED
 *      FTPS_EVENT_COMPLETE
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 *
 *		BT_STATUS_BUSY - The server is currently executing an operation.
 *
 *		BT_STATUS_PENDING - At least one server is connected. The disable process has 
 *			been successfully started. On each connected server, the application callback 
 *			will receive FTPS_EVENT_DISCONNECT event.
 *			When the disable process is complete, the application callback will 
 *			receive the FTPS_EVENT_DISABLED event, indicating the context is disabled.
 *
 */
BtStatus BTL_FTPS_Disable(BtlFtpsContext *ftpsContext);


/*-------------------------------------------------------------------------------
 * BTL_FTPS_Disconnect()
 *
 * Brief:  
 *		Initiates the disconnection of the server transport connection.
 *
 * Description:
 *		Initiates the disconnection of the server transport connection.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *		ftpsContext [in] - FTPS context.
 *
 * Generated Events:
 *      FTPS_EVENT_TP_DISCONNECTED
 *
 * Returns:
 *		BT_STATUS_BUSY - The server is currently executing an operation.
 
 *		BT_STATUS_PENDING - Operation was successfully started.
 *         Completion will be signaled via an event to the application
 *         callback.
 *
 *		BT_STATUS_NO_CONNECTION - Operation failed because there is
 *         no server connection.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */
BtStatus BTL_FTPS_Disconnect(BtlFtpsContext *ftpsContext);


/*-------------------------------------------------------------------------------
 * BTL_FTPS_GetConnectedDevice()
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
 *		ftpsContext [in] - FTPS context.
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
BtStatus BTL_FTPS_GetConnectedDevice(BtlFtpsContext *ftpsContext, 
										BD_ADDR *bdAddr);

/*-------------------------------------------------------------------------------
 * BTL_FTPS_ConfigObjectRequest()
 *
 * Brief:  
 *		This function configure the objects requests which will be delivered
 *		to the application callback.
 *
 * Description:
 *		This function configure the objects requests which will be delivered
 *		to the application callback.
 *		The application will respond with BTL_FTPS_AcceptObjectRequest() in order
 *		to accept or reject the object request.
 *		Default value is BTL_FTPS_OBJECT_REQUEST_NONE, which means all object
 *		requests are automatically accepted.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		ftpsContext [in] - FTPS context.
 *
 *		objectRequestMask [in] - defines the objects requests which will be delivered
 *			to the application callback.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_FTPS_ConfigObjectRequest(BtlFtpsContext *ftpsContext,
										const BtlFtpsObjectRequestMask objectRequestMask);


/*-------------------------------------------------------------------------------
 * BTL_FTPS_AcceptObjectRequest()
 *
 * Brief:  
 *		This function must be called after receiving object request from the client,
 *		in order to accept or reject the request.
 *
 * Description:
 *		This function must be called after receiving object request from the client,
 *		in order to accept or reject the request.
 *		The object requests which will be delivered to the application callback can
 *		be defined via BTL_FTPS_ConfigObjectRequest().
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *		ftpsContext [in] - FTPS context.
 *
 *		accept [in] - accept or reject pending object request.
 *
 * Generated Events:
 *      FTPS_EVENT_ABORTED   
 *      FTPS_EVENT_OBJECT_PROGRESS 
 *
 * Returns:
 *		BT_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via an event to the application callback.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *		BT_STATUS_FAILED - The operation failed.
 *
 *		BT_STATUS_NO_CONNECTION -  The server is not connected.
 */
BtStatus BTL_FTPS_AcceptObjectRequest(BtlFtpsContext *ftpsContext, 
										const BOOL accept);


/*-------------------------------------------------------------------------------
 * BTL_FTPS_SetReadOnlyMode()
 *
 * Brief:  
 *		This function Enable or Disable the Set read only mode.
 *
 * Description:
 *		This function Enable or Disable the Read only mode.
 *		Read only mode is disabled by default.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		ftpsContext [in] - FTPS context.
 *
 *		readOnly [in] - read only mode.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_FTPS_SetReadOnlyMode(BtlFtpsContext *ftpsContext, 
									const BOOL readOnly);


/*-------------------------------------------------------------------------------
 * BTL_FTPS_SetRootPath()
 *
 * Brief:  
 *		This function Set the root path of the root folder.
 *
 * Description:
 *		This function Set the root path of the root folder.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		ftpsContext [in] - FTPS context.
 *
 *		rootPath [in] - null-terminated string, representing the path of the 
 *			root folder.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */
BtStatus BTL_FTPS_SetRootPath(BtlFtpsContext *ftpsContext, 
								const BtlUtf8 *rootPath);


/*-------------------------------------------------------------------------------
 * BTL_FTPS_GetRootPath()
 *
 * Brief:  
 *		This function Get the root path of the root folder.
 *
 * Description:
 *		This function Get the root path of the root folder.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		ftpsContext [in] - FTPS context.
 *
 *		rootPath [out] - null-terminated string, representing the path of the 
 *			root folder.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_FTPS_GetRootPath(BtlFtpsContext *ftpsContext, 
								BtlUtf8 **rootPath);


/*-------------------------------------------------------------------------------
 * BTL_FTPS_GetCurrentPath()
 *
 * Brief:  
 *		This function Get the current path of the server.
 *
 * Description:
 *		This function Get the current path of the server.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		ftpsContext [in] - FTPS context.
 *
 *		currentPath [out] - null-terminated string, representing the current path
 *			of the server.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_FTPS_GetCurrentPath(BtlFtpsContext *ftpsContext, 
									BtlUtf8 **currentPath);

/*-------------------------------------------------------------------------------
 * BTL_FTPS_SetHiddenStatus()
 *
 * Brief:  
 *		This function configure the status of object hidden or not.
 *
 * Description:
 *		This function configure the status of object hidden or not for XML builder.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		ftpsContext [in] - FTPS context.
 *
 *		objName [in] - FileName or Directory Name
 *			
 *		setHide [in] - hide mode True / False 
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_FTPS_SetHiddenStatus(BtlFtpsContext *ftpsContext,const BtlUtf8 *objName,BOOL hideStat);

/*-------------------------------------------------------------------------------
 * BTL_FTPS_GetHiddenStatus()
 *
 * Brief:  
 *		This function get the status of object hidden or not.
 *
 * Description:
 *		This function get the status of object hidden or not.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		ftpsContext [in] - FTPS context.
 *
 *		objName [in] - FileName or Directory Name
 *			
 *		setHide [in] - hide mode True / False 
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_FTPS_GetHiddenStatus(BtlFtpsContext *ftpsContext,const BtlUtf8 *objName,BOOL *hideStat);

/*-------------------------------------------------------------------------------
 * BTL_FTPS_EnableHiddenMode()
 *
 * Brief:  
 *		This function Enable hidden mode.
 *
 * Description:
 *		This function Enable hidden mode.
 *		Hidden mode is Enable by default.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		ftpsContext [in] - FTPS context.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_FTPS_EnableHiddenMode(BtlFtpsContext *ftpsContext);

/*-------------------------------------------------------------------------------
 * BTL_FTPS_DisableHiddenMode()
 *
 * Brief:  
 *		This function Enable hidden mode.
 *
 * Description:
 *		This function Disable hidden mode.
 *		Hidden mode is Enable by default.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		ftpsContext [in] - FTPS context.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_FTPS_DisableHiddenMode(BtlFtpsContext *ftpsContext);

/*-------------------------------------------------------------------------------
 * BTL_FTPS_ClearHiddenList()
 *
 * Brief:  
 *		This function clear hidden list.
 *
 * Description:
 *		This function clear hidden list.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		ftpsContext [in] - FTPS context.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_FTPS_ClearHiddenList(BtlFtpsContext *ftpsContext);

/*-------------------------------------------------------------------------------
 * BTL_FTPS_Abort()
 *
 * Brief:  
 *		This function Aborts the current server operation.
 *
 * Description:
 *		This function Aborts the current server operation.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		ftpsContext [in] - FTPS context.
 *
 *		respCode [in] - The response code to send to the client expressing the reason
 *         for the abort.
 *
 * Generated Events:
 *      FTPS_EVENT_ABORTED   
 *
 * Returns:
 *		BT_STATUS_SUCCESS - The abort was successfully requested.
 *
 *		BT_STATUS_FAILED - The abort operation failed.
 *
 *		BT_STATUS_NO_CONNECTION -  No transport connection exists.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_FTPS_Abort(BtlFtpsContext *ftpsContext, ObexRespCode respCode);


/*-------------------------------------------------------------------------------
 * BTL_FTPS_ConfigInitiatingObexAuthentication()
 *
 * Brief:  
 *		This function Enable or disable server initiating OBEX authentication, 
 * 		when FTP client is connecting.
 *
 * Description:
 *		Enable or disable server initiating OBEX authentication, when FTP client 
 *		is connecting.
 *		In both cases, the server will still respond to OBEX authentication 
 *		initiated by the client (via BTL_FTPS_ObexAuthenticationResponse()).
 *		Server initiating OBEX authentication is disabled by default.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		ftpsContext [in] - FTPS context.
 *
 *		enableObexAuth [in] - enable initiating OBEX authentication.
 *
 *		realm [in] - null-terminated string used with OBEX authentication.
 *			This string should be displayed in the peer device when authentication 
 *			challenge is sent to the peer. The realm is indicating which userid 
 *			and/or password to use. This parameter is ignored if enableObexAuth 
 *			is FALSE. Realm is optional, thus NULL can be passed.
 *			Max len is OBEX_MAX_REALM_LEN.
 *			The pointer is freed after the function returns.
 *
 *		userId [in] - null-terminated string used with OBEX authentication 
 *			representing the user id. If NULL is passed, the user id is not 
 *			verified. This parameter is ignored if enableObexAuth is FALSE.
 *			Max len is OBEX_MAX_USERID_LEN.
 *			The pointer is freed after the function returns.
 *
 *		password [in] - null-terminated string used with OBEX authentication 
 *			representing the password. This parameter is ignored if 
 *			enableObexAuth is FALSE.
 *			Max len is OBEX_MAX_PASSWORD_LEN.
 *			The pointer is freed after the function returns.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_FTPS_ConfigInitiatingObexAuthentication(BtlFtpsContext *ftpsContext, 
													const BOOL enableObexAuth,
													const BtlUtf8 *realm, 
													const BtlUtf8 *userId, 
													const BtlUtf8 *password);


/*-------------------------------------------------------------------------------
 * BTL_FTPS_ObexAuthenticationResponse()
 *
 * Brief:  
 *		This function must be called after receiving OBEX authentication 
 *		challenge from the client.
 *
 * Description:
 *		This function must be called after receiving OBEX authentication 
 *		challenge from the client.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		ftpsContext [in] - FTPS context.
 *
 *		userId [in] - null-terminated string used with OBEX authentication 
 *			representing the user id. If the argument is not needed by the 
 *			client (see in event), NULL can be passed.
 *			Max len is OBEX_MAX_USERID_LEN.
 *			The pointer is freed after the function returns.
 *
 *		password [in] - null-terminated string used with OBEX authentication 
 *			representing the password. Pass NULL to deny authentication.
 *			Max len is OBEX_MAX_PASSWORD_LEN.
 *			The pointer is freed after the function returns.
 *
 * Generated Events:
 *      FTPS_EVENT_ABORTED   
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_FTPS_ObexAuthenticationResponse(BtlFtpsContext *ftpsContext, 
												const BtlUtf8 *userId, 
												const BtlUtf8 *password);


/*-------------------------------------------------------------------------------
 * BTL_FTPS_SetSecurityLevel()
 *
 * Brief:  
 *     	Sets security level for the given FTPS context.
 *
 * Description:
 *     	Sets security level for the given FTPS context.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	ftpsContext [in] - pointer to the FTPS context.
 *
 *		securityLevel [in] - level of security which should be applied,	when
 *			connecting to a remote FTP client. For default value, pass null.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_FTPS_SetSecurityLevel(BtlFtpsContext *ftpsContext,
								  	const BtSecurityLevel *securityLevel);


/*-------------------------------------------------------------------------------
 * BTL_FTPS_GetSecurityLevel()
 *
 * Brief:  
 *     	Gets security level for the given FTPS context.
 *
 * Description:
 *     	Gets security level for the given FTPS context.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	ftpsContext [in] - pointer to the FTPS context.
 *
 *		securityLevel [out] - level of security which should be applied, when
 *			connecting to a remote FTP client.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_FTPS_GetSecurityLevel(BtlFtpsContext *ftpsContext,
								  	BtSecurityLevel *securityLevel);


#endif /* __BTL_FTPS_H */



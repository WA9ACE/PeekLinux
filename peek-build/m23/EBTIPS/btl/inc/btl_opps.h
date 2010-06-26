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
*   FILE NAME:      btl_opps.h
*
*   BRIEF:          This file defines the API of the BTL Object Push Profile 
*                   server role
*
*   DESCRIPTION:    General
*
*                   The BTL_OPPS defines the API of the Object Push profile (OPP) server role, 
*                   for the use of the application. Being a server API,	the main use of the it 
*                   is basic initialization, connectivity, and support push/pull operations initiated
*                   by remote client.
*
*					Usage Models
*
*                   The BTL_OPP server representation is an OPPS context (BtlOppsContext).
*					The context has to be created and enabled in order to receive remote 
*					connections and requests. (BTL_OPPS_Create, BTL_OPPS_Enable).
*					
*					The OPP server handles the requests initiated by the remote client,
*					and is also capable of initiating a disconnect operation, or an 
*					abort operation. (BTL_OPPS_Disconnect, BTL_OPPS_Abort).
*					
*					The application will be notified by events regarding the on-going
*					operations. See below list of events. The events callback function        
*					should be registered using the BTL_OPPS_Enable function.
*					
*					Additional API: set/get security, accept/reject object request,
*					get remote device BD address.
*
*                   Operational Modes
*
*					The BTL_OPPS supports the following modes:
*                   1. File system mode - all operations are handled by The BTHAL_FS to store or retrieve
*                      the business card.
*                   2. Memory mode - all operations are handled by OBSTORE to store or retrieve the business 
*                      card in memory.
*				
*                   Supported Features
*
*                   1. All pushed file types are supported (e.g. jpeg,vCard,vNote,vMessage etc)
*                   2. Service Discovery - Provide information on supported content types on SDP request
*                   3. User Accept/Reject notifications for Pulled Object
*                   4. User Accept/Reject notifications Pushed Object
*  
*                   Generated Events List
*
*                   OPPS_EVENT_START
* 					OPPS_EVENT_COMPLETE
* 					OPPS_EVENT_ABORTED
* 					OPPS_EVENT_TP_CONNECTED           
* 					OPPS_EVENT_TP_DISCONNECTED
*					OPPS_EVENT_OBJECT_PROGRESS
* 					OPPS_EVENT_OBJECT_REQUEST
*                   OPPS_EVENT_DISABLED
*  
*					Execution Flow
*
*                   The OPP remote client initiates an operation. The application 
*					is notified by an event. The request is handled internally in
*					the BTL layer by accessing the BTHAL_FS. At the end of the operation
*					the application is also notified by an event.
*                   
*   AUTHOR:         Ilan Elias
*
\*******************************************************************************/

#ifndef __BTL_OPPS_H
#define __BTL_OPPS_H


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
typedef struct _OppServerEvent	OppServerEvent;
typedef struct _BtlOppsEvent 	BtlOppsEvent;
typedef struct _BtlOppsContext 	BtlOppsContext;

 
/*-------------------------------------------------------------------------------
 * BtlOppsCallBack type
 *
 *     A function of this type is called to indicate BTL OPPS events.
 */
typedef void (*BtlOppsCallBack)(const BtlOppsEvent *event);


/*-------------------------------------------------------------------------------
 * BtlOppsSupportedFormatsMask type
 *
 *     Defines which formats are supported in the OPPS, can be ORed together.
 */
typedef U8 BtlOppsSupportedFormatsMask;

#define BTL_OPPS_SUPPORTED_FORMATS_VCARD21		(0x01)	/* vCard 2.1 */
#define BTL_OPPS_SUPPORTED_FORMATS_VCARD30		(0x02)	/* vCard 3.0 */
#define BTL_OPPS_SUPPORTED_FORMATS_VCAL			(0x04)	/* vCal 1.0 */
#define BTL_OPPS_SUPPORTED_FORMATS_ICAL			(0x08)	/* iCal 2.0 */
#define BTL_OPPS_SUPPORTED_FORMATS_VNOTE		(0x10)	/* vNote */
#define BTL_OPPS_SUPPORTED_FORMATS_VMSG			(0x20)	/* vMessage */
#define BTL_OPPS_SUPPORTED_FORMATS_ANY			(0xFF)	/* Any type of object */


/*---------------------------------------------------------------------------
 * OppsEvent type
 *
 *     The OppsEvent type defines the events that may be indicated to
 *     the OPP Server application.
 */
typedef U8 OppsEvent;

/* 
 * Indicates the start of a new operation. In the case of an Abort
 * operation, this event may occur during another pending operation (Put or Get).
 */
#define OPPS_EVENT_START            GOEP_EVENT_START

/* 
 * Indicates that the operation has completed successfully.
 */
#define OPPS_EVENT_COMPLETE         GOEP_EVENT_COMPLETE

/* 
 * Indicates that the current operation has failed.
 */
#define OPPS_EVENT_ABORTED          GOEP_EVENT_ABORTED

/* 
 * A transport layer connection has been established. There is no
 * operation associated with this event.
 */
#define OPPS_EVENT_TP_CONNECTED     GOEP_EVENT_TP_CONNECTED 

/* 
 * The transport layer connection has been disconnected. There is no
 * operation associated with this event.
 */
#define OPPS_EVENT_TP_DISCONNECTED  GOEP_EVENT_TP_DISCONNECTED 

/* 
 * Delivered to applications on each progress change.
 */
#define OPPS_EVENT_OBJECT_PROGRESS	15

/* 
 * Delivered to applications on each client request.
 */
#define OPPS_EVENT_OBJECT_REQUEST   16

/*
 * This event is received when all connections have been closed.
 */
#define OPPS_EVENT_DISABLED			17

/* End of OppsEvent */


/*---------------------------------------------------------------------------
 * OppsOperation type
 *
 *     The OppsOperation type defines operations based on the Generic
 *     Object Exchange Profile's operations.
 */
typedef U8 OppsOperation;

/* 
 * This value is used when indicating events which do not occur during
 * an operation, such as TP Connect and TP Disconnect.
 */
#define OPPS_OPER_NONE              GOEP_OPER_NONE

/* 
 * A Connect operation is in progress.
 */
#define OPPS_OPER_CONNECT           GOEP_OPER_CONNECT

/* 
 * A Disconnect operation is in progress. 
 */
#define OPPS_OPER_DISCONNECT        GOEP_OPER_DISCONNECT

/* 
 * A Push operation is in progress.
 */
#define OPPS_OPER_PUSH              GOEP_OPER_PUSH

/* 
 * A Pull operation is in progress.
 */
#define OPPS_OPER_PULL              GOEP_OPER_PULL

/* 
 * An abort operation is in progress. This event will be delivered while
 * another operation is in progress, if the ABORT is sent to stop that
 * operation.
 */
#define OPPS_OPER_ABORT             GOEP_OPER_ABORT

/* End of OppsOperation */


/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/

/*---------------------------------------------------------------------------
 * OppsRequestInd structure
 * 
 *     This structure is used during request indications.
 */
typedef struct _OppsRequestInd 
{
	/* Name of the object, null-terminated. */
	BtlUtf8					*name;

	/* Length (in bytes) of the 'name' field. */
	U16						nameLen;

	/* Type of the object, null-terminated. */
	BtlUtf8					*type;

	/* Length (in bytes) of the 'type' field. */
	U16						typeLen;

	/* Object size (in bytes)  */
	U32						objectLen;
} OppsRequestInd;


/*---------------------------------------------------------------------------
 * OppsProgressInd structure
 * 
 *     This structure is used during progress indications.
 */
typedef struct _OppsProgressInd 
{
	/* Name of the object, null-terminated. */
	BtlUtf8					*name;

	/* Length (in bytes) of the 'name' field. */
	U16						nameLen;

	/* Current position (in bytes) of the operation. */
    U32						currPos;

	/* Max position (in bytes) of the operation. */
	U32						maxPos;
} OppsProgressInd;


/*-------------------------------------------------------------------------------
 * OppServerEvent structure
 *
 *     Represents OPP Server event.
 */
struct _OppServerEvent
{
	/* OPPS_EVENT_xxx */
	OppsEvent               event;

	/* OPPS_OPER_xxx */
	OppsOperation           oper;

	union 
	{
		OppsRequestInd      request;        /* Use during OPPS_EVENT_OBJECT_REQUEST */
		OppsProgressInd    	progress;       /* Use during OPPS_EVENT_OBJECT_PROGRESS */
    } info;
};


/*-------------------------------------------------------------------------------
 * BtlOppsEvent structure
 *
 *     Represents BTL OPPS event.
 */
struct _BtlOppsEvent 
{
	BtlOppsContext		*oppsContext;
	OppServerEvent		*oppsEvent;
};


/********************************************************************************
 *
 * Function declarations
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BTL_OPPS_Init()
 *
 * Brief:  
 *		Init the OPPS module.
 *
 * Description:
 *
 *		Init the OPPS module.
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
BtStatus BTL_OPPS_Init(void);


/*-------------------------------------------------------------------------------
 * BTL_OPPS_Deinit()
 *
 * Brief:  
 *		Deinit the OPPS module.
 *
 * Description:
 *
 *		Deinit the OPPS module.
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
BtStatus BTL_OPPS_Deinit(void);

 
/*-------------------------------------------------------------------------------
 * BTL_OPPS_Create()
 *
 * Brief:  
 *		Allocates a unique OPPS context.
 *
 * Description:
 *		Allocates a unique OPPS context.
 *		This function must be called before any other OPPS API function.
 *		The allocated context should be supplied in subsequent OPPS API calls.
 *		The caller must also provide a callback function, which will be called 
 *		on OPPS events.
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
 *		oppsCallback [in] - all OPPS events will be sent to this callback.
 *
 *		securityLevel [in] - level of security which should be applied,	when
 *			connecting to a remote OPP client. 
 *			If NULL is passed, default value is used.
 *		
 *		oppsContext [out] - allocated OPPS context.	
 *
 * Returns:
 *		BT_STATUS_SUCCESS - OPPS context was created successfully.
 *
 *		BT_STATUS_FAILED -  Failed to create OPPS context.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *		BT_STATUS_NO_RESOURCES - No more resources.
 *
 *		BT_STATUS_INTERNAL_ERROR - Internal error has occur.
 */
BtStatus BTL_OPPS_Create(BtlAppHandle *appHandle,
                        const BtlOppsCallBack oppsCallback,
                        const BtSecurityLevel *securityLevel,
                        BtlOppsContext **oppsContext);


/*-------------------------------------------------------------------------------
 * BTL_OPPS_Destroy()
 *
 * Brief:  
 *		Releases a OPPS context (previously allocated with BTL_OPPS_Create).
 *
 * Description:
 *		Releases a OPPS context (previously allocated with BTL_OPPS_Create).
 *		An application should call this function when it completes using OPPS services.
 *		Upon completion, the OPPS context is set to null in order to prevent 
 *		the application from an illegal attempt to keep using it.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		oppsContext [in/out] - OPPS context.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - OPPS context was destroyed successfully.
 *
 *		BT_STATUS_IN_USE -  OPPS context is in use.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *		BT_STATUS_INTERNAL_ERROR - Internal error has occur.
 */
BtStatus BTL_OPPS_Destroy(BtlOppsContext **oppsContext);


/*-------------------------------------------------------------------------------
 * BTL_OPPS_Enable()
 *
 * Brief:  
 *		Enable OPPS and register OPP server SDP record.
 *
 * Description:
 *		Enable OPPS and register OPP server SDP record, called after 
 *		BTL_OPPS_Create.
 *		After calling this function, OPP server is ready for usage.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		oppsContext [in] - OPPS context.
 *
 *		serviceName [in] - null-terminated service name which will be written into 
 *			SDP service record and will be discovered by the peer device.
 *			If NULL is passed, default value is used.
 *
 *		supportedFormats [in] - The object formats supported by the server, 
 *			which will be written into SDP service record and will be discovered 
 *			by the peer device (see BtlOppsSupportedFormatsMask).
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */
BtStatus BTL_OPPS_Enable(BtlOppsContext *oppsContext, 
                         const BtlUtf8 *serviceName,
                         const BtlOppsSupportedFormatsMask supportedFormats);


/*-------------------------------------------------------------------------------
 * BTL_OPPS_Disable()
 *
 * Brief:  
 *		Disable OPPS, called before BTL_OPPS_Destroy.
 *
 * Description:
 *		Disable OPPS, called before BTL_OPPS_Destroy.
 *		If a connection exists, it will be disconnected automatically.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		oppsContext [in] - OPPS context.
 *
 * Generated Events:
 *      OPPS_EVENT_ABORTED   
 *      OPPS_EVENT_TP_DISCONNECTED
 *      OPPS_EVENT_COMPLETE
 *      OPPS_EVENT_DISABLED
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 *
 *		BT_STATUS_PENDING - The link is connected. The disconnect process has 
 *			been successfully started. When the disconnect process is complete, the
 *         	application callback will receive the disconnect event,
 *			which indicates the OPPS is disabled.
 */
BtStatus BTL_OPPS_Disable(BtlOppsContext *oppsContext);


/*-------------------------------------------------------------------------------
 * BTL_OPPS_Disconnect()
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
 *		oppsContext [in] - OPPS context.
 *
 * Generated Events:
 *      OPPS_EVENT_TP_DISCONNECTED
 *
 * Returns:
 *		BT_STATUS_BUSY - The server is currently executing an operation.
 *
 *		BT_STATUS_PENDING - Operation was successfully started.
 *         Completion will be signaled via an event to the application
 *         callback.
 *
 *		BT_STATUS_NO_CONNECTION - Operation failed because there is
 *         no server connection.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */
BtStatus BTL_OPPS_Disconnect(BtlOppsContext *oppsContext);


/*-------------------------------------------------------------------------------
 * BTL_OPPS_GetConnectedDevice()
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
 *		oppsContext [in] - OPPS context.
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
BtStatus BTL_OPPS_GetConnectedDevice(BtlOppsContext *oppsContext, 
                                        BD_ADDR *bdAddr);


/*-------------------------------------------------------------------------------
 * BTL_OPPS_AcceptObjectRequest()
 *
 * Brief:  
 *		This function must be called after receiving object request from the client,
 *		in order to accept or reject the request.
 *
 * Description:
 *      This function must be called after receiving object request from the client,
 *		in order to accept or reject the request.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *      oppsContext [in] - OPPS context.
 *
 *      accept [in] - accept or reject pending object request.
 *
 *      object [in] - Object to accept (see BtlObject for details).
 *          If accept is FALSE, this parameter is ignored.
 *          The object pointer is free after the function returns.
 *          Used just in Push operation. 
 *          In Pull operation this field is unused. 
 *          Use BTL_OPPS_SetDefaultObject() function to set pulled object.
 *
 * Generated Events:
 *      OPPS_EVENT_ABORTED   
 *      OPPS_EVENT_OBJECT_PROGRESS 
 *
 * Returns:
 *      BT_STATUS_PENDING - Operation was successfully started. Completion
 *      will be signaled via an event to the application callback.
 *
 *      BT_STATUS_FAILED - The operation failed.
 */
BtStatus BTL_OPPS_AcceptObjectRequest(BtlOppsContext *oppsContext, 
                                      const BOOL accept, 
                                      const BtlObject *object);


/*-------------------------------------------------------------------------------
 * BTL_OPPS_Abort()
 *
 * Brief:  
 *		Aborts the current server operation.
 *
 * Description:
 *		Aborts the current server operation.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		oppsContext [in] - OPPS context.
 *
 *		respCode [in] - The response code to send to the client expressing the reason
 *         for the abort.
 *
 * Generated Events:
 *      OPPS_EVENT_ABORTED   
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
BtStatus BTL_OPPS_Abort(BtlOppsContext *oppsContext, const ObexRespCode respCode);


/*-------------------------------------------------------------------------------
 * BTL_OPPS_SetSecurityLevel()
 *
 * Brief:  
 *     	Sets security level for the given OPPS context.
 *
 * Description:
 *     	Sets security level for the given OPPS context.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	oppsContext [in] - pointer to the OPPS context.
 *
 *		securityLevel [in] - level of security which should be applied,	when
 *			connecting to a remote OPP client. For default value, pass null.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */
BtStatus BTL_OPPS_SetSecurityLevel(BtlOppsContext *oppsContext,
                                   const BtSecurityLevel *securityLevel);


/*-------------------------------------------------------------------------------
 * BTL_OPPS_GetSecurityLevel()
 *
 * Brief:  
 *     	Gets security level for the given OPPS context.
 *
 * Description:
 *     	Gets security level for the given OPPS context.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	oppsContext [in] - pointer to the OPPS context.
 *
 *		securityLevel [out] - level of security which should be applied, when
 *			connecting to a remote OPP client.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */
BtStatus BTL_OPPS_GetSecurityLevel(BtlOppsContext *oppsContext,
                                   BtSecurityLevel *securityLevel);


/*-------------------------------------------------------------------------------
 * BTL_OPPS_SetDefaultObject()
 *
 * Brief:  
 *     	Set default object absolute location (drive, path and file name)
 *
 * Description:
 *     	Set default object absolute location (drive, path and file name)
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	*defaultObject [in] - pointer to the default object, what will be pulled (if object is located in File System).
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_INVALID_PARM - Wrong parameter. The operation failed.
 */
BtStatus BTL_OPPS_SetDefaultObject(const BtlObject *defaultObject);


#endif /* __BTL_OPPS_H */



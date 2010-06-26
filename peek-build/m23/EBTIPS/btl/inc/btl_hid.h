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
*   FILE NAME:      btl_hidh.h
*
*   BRIEF:          This file defines the API of the BTL HID host role.
*                   
*	DESCRIPTION:    General
*
*					The BTL_HIDH defines the API of the BTL Human Interface Device Host
*                   role, for the use of the application. The BTL HOST is actually a
*					pipe connecting lower transport layer to the application, enabling 
*					sending and receiving data.  
*
*
*					Usage Model
*					
*					The BTL_PBAP server representation is a PBAP context (BtlPbapsContext).
*					The context has to created and enabled in order to initiate or receive  
*					connections or data transmissions. (BTL_HIDH_Create, BTL_HIDH_Enable).
*					
*					The BTL_HIDH_HostQueryDevice function is used to query the remote 
*					device about his SDP database entry, whenever the device is connect 
*					or not. An SDP query  will be sent automatically to the device at 
*					connection request (BTL_HIDH_Connect). Note that some devices are 
*					not capable of responding  to an SDP query while there's a leaving 
*					connection. 
*
*					The application can send a transaction or an interrupt using one of the 
*					API functions. Examples for transaction - device unplug or device reset.
*					It will be notified by events to the callback function (registered previously 
*					in BTL_HIDH_Enable function) whenever a transaction or an interrupt was
*					received.
*
*					Additional API: set/get security, get remote device BD address.
*
*
*					Data Allocation and Release
*
*					It is the responsibility of the application to allocate and free
*					the data it wishes to send. At data sending (transaction or interrupt) 
*					using an API function, a 'cookie' with a reference pointer will be returned 
*					by the called function. The data pointer should be valid until a 'complete'
*					event with the previously given cookie will be sent to the application, which 
*					can then free the data.
*
*   AUTHOR:         Avi Hamu
*
\*******************************************************************************/

#ifndef __BTL_HID_H
#define __BTL_HID_H


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include <hid.h>
#include <sec.h>
#include <btl_common.h>
#include "btl_unicode.h"

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/* Forward declarations */

typedef struct _BtlHidEvent 	BtlHidEvent;
typedef struct _BtlHidContext 	BtlHidContext;


/*-------------------------------------------------------------------------------
 * BtlHidChannelId type
 *
 *     Defines the type of an HID channel id.
 */
 
typedef U32 BtlHidChannelId;

 
/*-------------------------------------------------------------------------------
 * BtlHidCallBack type
 *
 *     A function of this type is called to indicate BTL HID events.
 */

typedef void (*BtlHidCallBack)(const BtlHidEvent *event);


/*-------------------------------------------------------------------------------
 * BtlHidSupportedCategoriesMask type
 *
 *     Defines which type of device are supported in the HID.
 */
typedef U8 BtlHidSupportedRole;

#define BTL_HID_HOST		(0x01)
#define BTL_HID_DEVICE   	(0x02)	

/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BtlHidEvent structure
 *
 *     Represents BTL HID event.
 */

struct _BtlHidEvent 
{
	/* Associated context */
	BtlHidContext		*hidContext;

	/* Associated channel id */
	BtlHidChannelId		channelId;

	/* HID event */
	HidCallbackParms		*hidEvent;
};



/********************************************************************************
 *
 * Function declarations
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BTL_HIDH_Init()
 *
 * Brief:  
 *		Initialize the HID Module.
 *
 * Description:
 *      Initialize the HID Module. This function should be the 
 *      first function called.
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
BtStatus BTL_HIDH_Init(void);

/*-------------------------------------------------------------------------------
 * BTL_HIDH_Deinit()
 *
 * Brief:
 *		Deinit the HID module.  
 *
 * Description:
 *		Deinit the HID module. 	
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
BtStatus BTL_HIDH_Deinit(void);

/*-------------------------------------------------------------------------------
 * BTL_HIDH_Create()
 *
 * Brief:  
 *      Allocates a unique HID host context.
 *
 * Description:
 *		Allocates a unique HID context.
 *		This function must be called before any other HID API function.
 *		The allocated context should be supplied in subsequent HID API calls.
 *		The caller must also provide a callback function, which will be called 
 *		on HID events.
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
 *		hidCallback [in] - all HID events will be sent to this callback.
 *
 *		securityLevel [in] - level of security which should be applied,	when
 *			connecting to a remote HID device.
 *			If NULL is passed, default value is used.
 *		
 *		hidContext [out] - allocated HID context.	
 *
 * Returns:
 *		BT_STATUS_SUCCESS - HID context was created successfully.
 *
 *		BT_STATUS_FAILED -  Failed to create HID context.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *		BT_STATUS_NO_RESOURCES - No more resources.
 *
 *		BT_STATUS_INTERNAL_ERROR - Internal error has occur.
 */
BtStatus BTL_HIDH_Create(BtlAppHandle *appHandle,
							const BtlHidCallBack hidCallback,
							const BtSecurityLevel *securityLevel,
							BtlHidContext **hidContext);

/*-------------------------------------------------------------------------------
 * BTL_HIDH_Destroy()
 *
 * Brief:  
 *      Releases a HID context.
 *
 * Description:
 *		Releases a HID context (previously allocated with BTL_HIDH_Create).
 *		An application should call this function when it completes using HID services.
 *		Upon completion, the HID context is set to null in order to prevent 
 *		the application from an illegal attempt to keep using it.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
  *		hidContext [in/out] - HID context.
 *
 * Returns:
  *		BT_STATUS_SUCCESS - HID context was destroyed successfully.
 *
 *		BT_STATUS_IN_USE -  HID context is in use.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *		BT_STATUS_INTERNAL_ERROR - Internal error has occur.
 */
BtStatus BTL_HIDH_Destroy(BtlHidContext **hidContext);

/*-------------------------------------------------------------------------------
 * BTL_HIDH_Enable()
 *
 * Brief:  
 *      Enable the HID module, called after BTL_HIDH_Create.
 *
 * Description:
 *		Enable the HID module, called after BTL_HIDH_Create
 *		After calling this function, HID host is ready for usage.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *     	hidContext [in] - pointer to the HID context.
 *
 *		serviceName [in] - A name given to the service
 *                  If NULL value is passed the default service name,
 *                  "HID Host App", is used.
 *
 *		supportedRole [in] - The role supported by the device.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *		BT_STATUS_FAILED - The specified context is already enabled.
 */
BtStatus BTL_HIDH_Enable(BtlHidContext *hidContext,
								const BtlUtf8 *serviceName,
								const BtlHidSupportedRole supportedRole);



/*-------------------------------------------------------------------------------
 * BTL_HIDH_Disable()
 *
 * Brief:  
 *		Disable the HID module, called before BTL_HIDH_Destroy.
 *
 *  Description:
 *		Disable the HID module, called before BTL_HIDH_Destroy.
 *		If any connections exist, they will be disconnected automatically.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *		hidContext [in] - HID context.
 * 
 * Generated Events:
 *	    HIDEVENT_DISABLED
 *      HIDEVENT_CLOSED
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *		
 *		BT_STATUS_PENDING - The link is connected. The disconnect process has 
 *			been successfully started. When the disconnect process is complete, the
 *         		application callback will receive the HIDEVENT_CLOSED event,
 *			which indicates that HID is disabled.
 *			When the disable process is complete, the application callback will 
 *			receive the HIDEVENT_DISABLED event, indicating the context is disabled.
 *
 */
BtStatus BTL_HIDH_Disable(BtlHidContext *hidContext);


/*-------------------------------------------------------------------------------
 * BTL_HIDH_Connect()
 *
 * Brief:  
 *		Initiates a connection to a remote HID Device.
 *
 * Description:
 *     	Initiates a connection to a remote HID Device. This function is 
 *     	used to establish the lower layer connection (L2CAP), which allows
 *     	sending transactions and interrupts.
 *
 *		The HID Host send a SDP query before establishing a connection, and 
 *		when the event HIDEVENT_QUERY_CNF arrives, only then we try to establish a connection.
 *		
 *     	If the connection attempt is successful, the HIDEVENT_OPEN event
 *     	will be received.  If the connection attempt is unsuccessful, the
 *     	HIDEVENT_CLOSED event will be received.
 *           
 *      This function is in charge to perform a reconnection to the device if the connection has been lost. 
 *		(If the relevant attributes of the SDP query require to do a reconnection)
 *
 * Type:
 *		Asynchronous
 *
 * Generated Events:
 *	    HIDEVENT_OPEN
 *      HIDEVENT_CLOSED
 *
 * Parameters:
 *     	hidContext [in] - pointer to the HID context.
 *
 *		bdAddr [in] - BD_ADDR of the remote device.
 *
 * Returns:
 *		BT_STATUS_PENDING - The connection process has been successfully
 *         		started. When the connection process is complete, the
 *         		application callback will receive either the HIDEVENT_OPEN or 
 *         		HIDEVENT_CLOSED event.
 *
 *		BT_STATUS_FAILED - The operation failed.
 *
 *     	BT_STATUS_IN_USE - All context channels are already connected or are in the
 *         		process of connecting.
 *
 *     	BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *     	Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus BTL_HIDH_Connect(BtlHidContext *hidContext,
										BD_ADDR *bdAddr);


/*-------------------------------------------------------------------------------
 * BTL_HIDH_GetConnectedDevice()
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
 *		hidContext [in] - HID context.
 *
 *		channelId [in] - associated channel id.
 *
 *		bdAddr [out] - pointer to 48-bit address of the connected device.
 *
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *		BT_STATUS_NO_CONNECTION -  The target is not connected.
 */
BtStatus BTL_HIDH_GetConnectedDevice(BtlHidContext *hidContext, 
										BtlHidChannelId channelId, 
										BD_ADDR *bdAddr);
 
/*-------------------------------------------------------------------------------
 * BTL_HIDH_Disconnect()
 *
 * Brief:  
 *		Closes an HID connection between two devices.
 *
 * Description:
 *     	Closes an HID connection between two devices.  When the connection
 *     	is closed, the application will receive an HIDEVENT_CLOSED event.
 *
 *     	If there are outstanding transactions or interrupts when a connection is 
 *     	closed, an event will be received by the application for each one.  The 
 *     	"status" field for these events will be set to BT_STATUS_NO_CONNECTION.
 *
 *		After disconnection completed the Host will try to reconnect to the device if 
 *		the relevant attributes of the SDP query require to do a reconnection.
 *		The host won't do a reconnection, also if the disconnection is the result of
 *		the HID_CTRL_VIRTUAL_CABLE_UNPLUG control operation
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *     	hidContext [in] - pointer to the HID context.
 *
 *		channelId [in] - associated channel id.
 *
 * Generated Events:
 *	    HIDEVENT_CLOSED
 *
 * Returns:
 *		BT_STATUS_PENDING - The disconnect process has been successfully
 *         		started. When the disconnect process is complete, the
 *         		application callback will receive the HIDEVENT_CLOSED event.
 *
 *     	BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *     	BT_STATUS_NO_CONNECTION - No connection exists on the specified context.
 *
 *     	Other - It is possible to receive other error codes, depending on the 
 *         		lower layer service in use (L2CAP or Management Entity).
 */
BtStatus BTL_HIDH_Disconnect(BtlHidContext *hidContext,
									BtlHidChannelId channelId);


/*-------------------------------------------------------------------------------
 * BTL_HIDH_SendInterrupt()
 *
 * Brief:  
 *	   Sends an interrupt (report) to the remote device.
 *
 * Description:
 *	   Sends an interrupt (report) to the remote device (Host or HID Device).  
 *     The Interrupt parameters should be initialized with the appropriate 
 *     information.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *     	hidContext [in] - pointer to the HID context.
 *
 *		channelId [in] - associated channel id.
 *
 *		data [in] - pointer to interrupt data. This pointer is owned by 
 *			BTL_HID until the HidInterrupt (returned as last argument) is returned 
 *			with the HIDEVENT_INTERRUPT_COMPLETE event.
 *
 *		dataLen [in] - length of interrupt data. 
 *
 *		reportType [in] - Report type (input, output, or feature).
 *		
 *		cookie [out] - The cookie used to send the transaction. This pointer is returned
 *         	with the HIDEVENT_INTERRUPT_COMPLETE event (in the field 'ptrs.intrData'), and only then the 
 *		'data' pointer can be freed.
 * 
 * Generated Events:
 *	    HIDEVENT_INTERRUPT_COMPLETE
 *
 * Returns:
 *     BT_STATUS_PENDING - The request has been queued. If sent successfully, 
 *         an HIDEVENT_INTERRUPT_COMPLETE event will arrive with a "status" of 
 *         BT_STATUS_SUCCESS.  If the transmission fails, the same event will be 
 *         received with a status specifying the reason.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel was not found (XA_ERROR_CHECK 
 *         only).
 *
 *     BT_STATUS_NO_CONNECTION - No connection exists for transmitting.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter.
 *
 *     BT_STATUS_IN_USE - The channel is already in use.
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus BTL_HIDH_SendInterrupt(BtlHidContext *hidContext,
										BtlHidChannelId channelId,
										U8 *data,
										U16 dataLen,
										HidReportType reportType,
										HidInterrupt **cookie);



/*---------------------------------------------------------------------------
 * HID_HostQueryDevice()
 *
 * Brief:  
 *	   Queries the Device for its SDP database entry.
 *
 * Description:
 *     Queries the Device for its SDP database entry. The query
 *     information will be returned to the application with a
 *     HIDEVENT_QUERY_CNF event. The application can save the
 *     pointer to the SDP data and reference it as long as the channel
 *     is still registered.  The data is no longer valid when the channel is
 *     deregistered. It is possible that the query will fail, because some devices 
 *     limited in memory will not allow an SDP query while the HID channel is open.  
 *     It is suggested that the host query the device before opening a 
 *     connection.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *     hidContext [in] - pointer to the HID context.
 *
 *	channelId [in] - associated channel id.
 *
 *	bdAddr [in] - BD_ADDR of the remote device.
 *
 * Generated Events:
 *	    HIDEVENT_QUERY_CNF
 *
 * Returns:
 *     BT_STATUS_PENDING - The request to query the SDP entry was sent.
 *         When a response is received from the device, an HIDEVENT_QUERY_CNF
 *         event will be received.
 *
 *     BT_STATUS_BUSY - The connection is already in the process of opening.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel was not found.
 *        
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter.
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (SDP, L2CAP or Management Entity).
 */
BtStatus BTL_HIDH_HostQueryDevice(BtlHidContext *hidContext,
										BtlHidChannelId channelId,
										BD_ADDR *bdAddr);




/*-------------------------------------------------------------------------------
 * BTL_HID_SetSecurityLevel()
 *
 * Brief:  
 *	   Sets security level for the given HID context.
 *
 * Description:
 *     	Sets security level for the given HID context.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *     	hidContext [in] - pointer to the HID context.
 *
 *		securityLevel [in] - level of security which should be applied,	when
 *			connecting to a remote HID device. For default value, pass null.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_HID_SetSecurityLevel(BtlHidContext *hidContext,
								  		const BtSecurityLevel *securityLevel);



/*-------------------------------------------------------------------------------
 * BTL_HID_GetSecurityLevel()
 *
 * Brief:  
 *	    Gets security level for the given HID context.
 *
 * Description:
 *     	Gets security level for the given HID context.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *     	hidContext [in] - pointer to the HID context.
 *
 *		securityLevel [out] - level of security which should be applied, when
 *			connecting to a remote HID device.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_HID_GetSecurityLevel(BtlHidContext *hidContext,
								  		BtSecurityLevel *securityLevel);



/*-------------------------------------------------------------------------------
 * BTL_HIDH_HardReset()
 *
 * Brief:  
 *	    Sends a "HardReset" operation control to the remote device (HID Device).
 *
 * Description:
 *		Sends a "HardReset" operation control to the remote device (HID Device).  
 *           channelId - Identifies the channel on which to send the request.
 *		cookie - A pointer to the transaction, which describes the control
 *           operation.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *     	hidContext [in] - pointer to the HID context.
 *
 *		channelId [in] - associated channel id.
 *
 *		cookie [out] - The cookie is used to send the transaction. This pointer is returned
 *         	with the HIDEVENT_TRANSACTION_COMPLETE event (in the field 'ptrs.trans'), and only then the "transaction"
 *		pointer can be freed.
 *
 * Generated Events:
 *	    HIDEVENT_TRANSACTION_COMPLETE
 *
 * Returns:
 *     BT_STATUS_PENDING - The response has been queued.  
 *           A HIDEVENT_TRANSACTION_COMPLETE event will arrive with a "status" of
 *           BT_STATUS_SUCCESS upon successful transmission.  If the transmission 
 *           fails, the same event will be received with a status specifying the
 *           reason.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel was not found. 
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter .
 *
 *           Other - It is possible to receive other error codes, depending on the 
 *           lower layer service in use (L2CAP or Management Entity).
 */
BtStatus BTL_HIDH_HardReset(BtlHidContext *hidContext,
									BtlHidChannelId channelId,
									HidTransaction**cookie);

/*-------------------------------------------------------------------------------
 * BTL_HIDH_SoftReset()
 *
 * Brief:  
 *	    Sends a "SoftReset" operation control to the remote device (HID Device). 
 *
 * Description:
 *		Sends a "SoftReset" operation control to the remote device (HID Device).  
 *           channelId - Identifies the channel on which to send the request.
 *		cookie - A pointer to the transaction, which describes the control
 *           operation.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *     	hidContext [in] - pointer to the HID context.
 *
 *		channelId [in] - associated channel id.
 *
 *		cookie [out] - The cookie is used to send the transaction. This pointer is returned
 *         	with the HIDEVENT_TRANSACTION_COMPLETE event (in the field 'ptrs.trans'), and only then the "transaction"
 *		pointer can be freed.
 * 
 * Generated Events:
 *	    HIDEVENT_TRANSACTION_COMPLETE
 *
 * Returns:
 *     BT_STATUS_PENDING - The response has been queued.  
 *           A HIDEVENT_TRANSACTION_COMPLETE event will arrive with a "status" of
 *           BT_STATUS_SUCCESS upon successful transmission.  If the transmission 
 *           fails, the same event will be received with a status specifying the
 *           reason.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel was not found. 
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter .
 *
 *          Other - It is possible to receive other error codes, depending on the 
 *          lower layer service in use (L2CAP or Management Entity).
 */
BtStatus BTL_HIDH_SoftReset(BtlHidContext *hidContext,
									BtlHidChannelId channelId,
									HidTransaction**cookie);

/*-------------------------------------------------------------------------------
 * BTL_HIDH_Suspend()
 *
 * Brief:  
 *	    Sends a "Suspend" operation control to the remote device (HID Device). 
 *
 * Description:
 *		Sends a "Suspend" operation control to the remote device (HID Device).  
 *           channelId - Identifies the channel on which to send the request.
 *		cookie - A pointer to the transaction, which describes the control
 *           operation.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *     	hidContext [in] - pointer to the HID context.
 *
 *		channelId [in] - associated channel id.
 *
 *		cookie [out] - The cookie is used to send the transaction. This pointer is returned
 *         	with the HIDEVENT_TRANSACTION_COMPLETE event (in the field 'ptrs.trans'), and only then the "transaction"
 *		pointer can be freed.
 *
 * Generated Events:
 *	    HIDEVENT_TRANSACTION_COMPLETE
 *
 * Returns:
 *     BT_STATUS_PENDING - The response has been queued.  
 *           A HIDEVENT_TRANSACTION_COMPLETE event will arrive with a "status" of
 *           BT_STATUS_SUCCESS upon successful transmission.  If the transmission 
 *           fails, the same event will be received with a status specifying the
 *           reason.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel was not found. 
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter .
 *
 *          Other - It is possible to receive other error codes, depending on the 
 *          lower layer service in use (L2CAP or Management Entity).
 */
BtStatus BTL_HIDH_Suspend(BtlHidContext *hidContext,
									BtlHidChannelId channelId,
									HidTransaction**cookie);

/*-------------------------------------------------------------------------------
 * BTL_HIDH_ExitSuspend()
 *
 * Brief:  
 *	    Sends an "Exit Suspend" operation control to the remote device (HID Device).
 *
 * Description:
 *		Sends an "Exit Suspend" operation control to the remote device (HID Device).  
 *           channelId - Identifies the channel on which to send the request.
 *		cookie - A pointer to the transaction, which describes the control
 *           operation.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *     	hidContext [in] - pointer to the HID context.
 *
 *		channelId [in] - associated channel id.
 *
 *		cookie [out] - The cookie is used to send the transaction. This pointer is returned
 *         	with the HIDEVENT_TRANSACTION_COMPLETE event (in the field 'ptrs.trans'), and only then the "transaction"
 *		pointer can be freed.
 *
 * Generated Events:
 *	    HIDEVENT_TRANSACTION_COMPLETE
 *
 * Returns:
 *     BT_STATUS_PENDING - The response has been queued.  
 *           A HIDEVENT_TRANSACTION_COMPLETE event will arrive with a "status" of
 *           BT_STATUS_SUCCESS upon successful transmission.  If the transmission 
 *           fails, the same event will be received with a status specifying the
 *           reason.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel was not found. 
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter .
 *
 *           Other - It is possible to receive other error codes, depending on the 
 *           lower layer service in use (L2CAP or Management Entity).
 */
BtStatus BTL_HIDH_ExitSuspend(BtlHidContext *hidContext,
									BtlHidChannelId channelId,
									HidTransaction**cookie);

/*-------------------------------------------------------------------------------
 * BTL_HIDH_Unplug()
 *
 * Brief:  
 *	    Sends a "Unplug" operation control to the remote device (HID Device). 
 *
 * Description:
 *		Sends a "Unplug" operation control to the remote device (HID Device).  
 *           channelId - Identifies the channel on which to send the request.
 *		cookie - A pointer to the transaction, which describes the control
 *           operation.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *     	hidContext [in] - pointer to the HID context.
 *
 *		channelId [in] - associated channel id.
 *
 *		cookie [out] - The cookie is used to send the transaction. This pointer is returned
 *         	with the HIDEVENT_TRANSACTION_COMPLETE event (in the field 'ptrs.trans'), and only then the "transaction"
 *		pointer can be freed.
 *
 * Generated Events:
 *	    HIDEVENT_TRANSACTION_COMPLETE
 *
 * Returns:
 *     BT_STATUS_PENDING - The response has been queued.  
 *           A HIDEVENT_TRANSACTION_COMPLETE event will arrive with a "status" of
 *           BT_STATUS_SUCCESS upon successful transmission.  If the transmission 
 *           fails, the same event will be received with a status specifying the
 *           reason.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel was not found. 
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter .
 *
 *           Other - It is possible to receive other error codes, depending on the 
 *           lower layer service in use (L2CAP or Management Entity).
 */
BtStatus BTL_HIDH_Unplug(BtlHidContext *hidContext,
									BtlHidChannelId channelId,
									HidTransaction**cookie);

/*-------------------------------------------------------------------------------
 * BTL_HIDH_GetReport()
 *
 * Brief:  
 *	    Sends an report request to the HID device. 
 *
 * Description:
 *     	Sends an report request to the HID device. Requesting an input report 
 *		causes the device to respond with the instantaneous state of fields in the 
 *		requested input report.
 *     	Requesting an output report causes the device to respond with the last
 *     	output report received on the interrupt channel.  If no output report
 *     	has been received, default values will be returned. Requesting a feature
 *     	report causes the device to return the default values or instantaneous
 *     	state of the feature report fields.
 *		If useId is TRUE both reportType and reportId will determine the report type, 
 *		otherwise only reportType is relevant.
 *
 *		Note: This transaction is costly in terms of time and overhead, and shall be 
 *		avoided whenever possible.
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *     	hidContext [in] - pointer to the HID context.
 *
 *		channelId [in] -  associated channel id.
 *
 *		bufferSize [in] - The amount of report data to be returned 
 * 					   (optional). Set this value to 0 if the 
 * 					   entire report should be returned. Note that
 * 					   this value must be increased by 1 to 
 * 					   receive the Report ID in Boot Mode.
 *
 *
 *		reportType [in] - Report type (input, output, or feature).
 *
 *		useId [in] - Set to TRUE if reportId should be used.
 *
 *		reportId [in] -The report ID (optional).
 *
 *		cookie [out] - The cookie used to send the transaction. This pointer is returned
 *         	with the HIDEVENT_TRANSACTION_COMPLETE event (in the field 'ptrs.trans'), and only then the "transaction"
 *		pointer can freed.
 *
 * Generated Events:
 *	    HIDEVENT_TRANSACTION_RSP
 *      HIDEVENT_TRANSACTION_COMPLETE
 *
 * Returns:
 *     BT_STATUS_PENDING - The request has been queued. If sent successfully, 
 *           at lease one HIDEVENT_TRANSACTION_RSP event will be receive with
 *           report data.  A HIDEVENT_TRANSACTION_COMPLETE event will arrive with
 *           a "status" of BT_STATUS_SUCCESS upon successful completion.
 *           If the transmission fails, the same event will be received with a
 *           status specifying the reason.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel was not found.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter.
 *
 *           Other - It is possible to receive other error codes, depending on the 
 *           lower layer service in use (L2CAP or Management Entity).
 */
BtStatus BTL_HIDH_GetReport(BtlHidContext *hidContext,
										BtlHidChannelId channelId,
										U16 bufferSize,
										HidReportType reportType,
										BOOL useId,
										U8 reportId,
										HidTransaction**cookie);

/*-------------------------------------------------------------------------------
 * BTL_HIDH_SetReport()
 *
 * Brief:  
 *	    Sends a report to the HID device. 
 *
 * Description:
 *     	Sends a report to the HID device.  The input parameters should be 
 *		initialized with the appropriate report information.
 *		
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *     	hidContext [in] - pointer to the HID context.
 *
 *		channelId [in] - associated channel id.
 *
 *		data [in] - Report data.This pointer is owned by 
 *		BTL_HID until the HidTransaction (returned as last argument) is returned 
 *
 *		dataLen [in] - Length of the report data.
 *
 *		reportType [in] - Report type (input, output, or feature).
 *
 *		cookie [out] - The cookie used to send the transaction. This pointer is returned
 *         	with the HIDEVENT_TRANSACTION_COMPLETE event (in the field 'ptrs.trans'), and only then the 'data'
 *		pointer can be freed.
 *
 * Generated Events:
 *      HIDEVENT_TRANSACTION_COMPLETE
 *
 * Returns:
 *     BT_STATUS_PENDING - The request has been queued. If sent successfully, 
 *           an HIDEVENT_TRANSACTION_COMPLETE event will arrive with a "status" of 
 *           BT_STATUS_SUCCESS.  If the transmission fails, the same event will be 
 *           received with a status specifying the reason.
 *
 *     BT_STATUS_NO_CONNECTION - No connection exists for transmitting.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter .
 *
 *           Other - It is possible to receive other error codes, depending on the 
 *           lower layer service in use (L2CAP or Management Entity).
 */
BtStatus BTL_HIDH_SetReport(BtlHidContext *hidContext,
										BtlHidChannelId channelId,
										U8 *data,
										U16 dataLen,
										HidReportType reportType,
										HidTransaction **cookie);

/*-------------------------------------------------------------------------------
 * BTL_HIDH_GetProtocol()
 *
 * Brief:  
 *	    Sends a protocol request to the HID device. 
 *
 * Description:
 *		Sends a protocol request to the HID device. No initialization is need.
 *		cookie -  A pointer to the transaction structure.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *     	hidContext [in] - pointer to the HID context.
 *
 *		channelId [in] - associated channel id.
 *
 *		cookie [out] - The cookie used to send the transaction. This pointer is returned
 *         	with the HIDEVENT_TRANSACTION_COMPLETE event (in the field 'ptrs.trans'), and only then the "transaction"
 *		pointer can be freed.
 *
 * Generated Events:
 *      HIDEVENT_TRANSACTION_COMPLETE
 *
 * Returns:
 *     BT_STATUS_PENDING - The request has been queued. If sent successfully, 
 *           an HIDEVENT_TRANSACTION_RSP event will be receive with protocol data.
 *           An HIDEVENT_TRANSACTION_COMPLETE event will arrive with a "status" of 
 *           BT_STATUS_SUCCESS upon successful complete.  If the transaction
 *           fails, the same event will be received with a status specifying the
 *           reason.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel was not found.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter .
 *
 *           Other - It is possible to receive other error codes, depending on the 
 *           lower layer service in use (L2CAP or Management Entity).
 */
BtStatus BTL_HIDH_GetProtocol(BtlHidContext *hidContext,
									BtlHidChannelId channelId,
									HidTransaction**cookie);

/*-------------------------------------------------------------------------------
 * BTL_HIDH_SetProtocol()
 *
 * Brief:  
 *	    Sends the current protocol to the HID device.
 *
 * Description:
 *		Sends the current protocol to the HID device.
 *      
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *     	hidContext [in] - pointer to the HID context.
 *
 *		channelId [in] - associated channel id.
 *
 *		reportProtocol [in] - Contains the current protocol.
 *
 *		cookie [out] - The cookie used to send the transaction. This pointer is returned
 *         	with the HIDEVENT_TRANSACTION_COMPLETE event (in the field 'ptrs.trans'), and only then the "transaction"
 *		pointer can be freed.
 *
 *  Generated Events:
 *      HIDEVENT_TRANSACTION_COMPLETE
 *
 * Returns:
 *     BT_STATUS_PENDING - The request has been queued. If sent successfully, 
 *           an HIDEVENT_TRANSACTION_COMPLETE event will arrive with a "status" of 
 *           BT_STATUS_SUCCESS.  If the transmission fails, the same event will be 
 *           received with a status specifying the reason.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel was not found .
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter .
 *
 *           Other - It is possible to receive other error codes, depending on the 
 *           lower layer service in use (L2CAP or Management Entity).
 */
BtStatus BTL_HIDH_SetProtocol(BtlHidContext *hidContext,
										BtlHidChannelId channelId,
										HidProtocol reportProtocol,
										HidTransaction**cookie);

/*-------------------------------------------------------------------------------
 * BTL_HIDH_GetIdleRate()
 *
 * Brief:  
 *	    Sends an idle rate status request to the HID device.
 *
 * Description:
 *		Sends an idle rate status request to the HID device. No initialization is need.
 *		
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *     	hidContext [in] - pointer to the HID context.
 *
 *		channelId [in] - associated channel id.
 *
 *		cookie [out] - The cookie used to send the transaction. This pointer is returned
 *         	with the HIDEVENT_TRANSACTION_COMPLETE event (in the field 'ptrs.trans'), and only then the "transaction"
 *		pointer can be freed.
 *
 * Generated Events:
 *      HIDEVENT_TRANSACTION_COMPLETE
 *
 * Returns:
 *     BT_STATUS_PENDING - The request has been queued. If sent successfully, 
 *           an HIDEVENT_TRANSACTION_RSP event will be receive with "idleRate" data.
 *           An HIDEVENT_TRANSACTION_COMPLETE event will arrive with a "status" of 
 *           BT_STATUS_SUCCESS upon successful complete.  If the transaction
 *           fails, the same event will be received with a status specifying the
 *           reason.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel was not found.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter.
 *
 *          Other - It is possible to receive other error codes, depending on the 
 *          lower layer service in use (L2CAP or Management Entity).
 */
BtStatus BTL_HIDH_GetIdleRate(BtlHidContext *hidContext,
									BtlHidChannelId channelId,
									HidTransaction**cookie);

/*-------------------------------------------------------------------------------
 * BTL_HIDH_SetIdleRate()
 *
 * Brief:  
 *	    Sends the idle rate to the HID device.
 *
 * Description:
 *		Sends the idle rate to the HID device. The "reportIdleRate" parameter
 *		should be initialized with the appropriate idle rate.
 *		cookie - A pointer to the transaction, which describes idle rate.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *     	hidContext [in] - pointer to the HID context.
 *
 *		channelId [in] - associated channel id.
 *
 *		reportIdleRate [in] - Contains the idle rate.
 *
 *		cookie [out] - The cookie used to send the transaction. This pointer is returned
 *         	with the HIDEVENT_TRANSACTION_COMPLETE event (in the field 'ptrs.trans'), and only then the "transaction"
 *		pointer can be freed.
 *
 * Generated Events:
 *      HIDEVENT_TRANSACTION_COMPLETE
 *
 * Returns:
 *     BT_STATUS_PENDING - The idle rate has been queued. If sent successfully, 
 *         an HIDEVENT_TRANSACTION_COMPLETE event will arrive with a "status" of 
 *         BT_STATUS_SUCCESS.  If the transmission fails, the same event will be 
 *         received with a status specifying the reason.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel was not found.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter.
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus BTL_HIDH_SetIdleRate(BtlHidContext *hidContext,
										BtlHidChannelId channelId,
										HidIdleRate reportIdleRate,
										HidTransaction**cookie);


#endif /* __BTL_HID_H */




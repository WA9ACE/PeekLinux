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
*   FILE NAME:		btl_spp.h
*
*   BRIEF:        	This file defines the API of the BTL Srerial Port Profile 
*                     	client and server role.
*
*   DESCRIPTION:	The BTL SPP provides abstraction layer for DUN ,FAX
*                   		and SPP applications. This layer provides the procedures  
*                   		used by the applications of the Serial Port Peofile usage model.
*
*
*   AUTHOR:			V. Abram
*
\*******************************************************************************/

#ifndef __BTL_SPP_H
#define __BTL_SPP_H

 
/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "bttypes.h"
#include "sec.h"
#include "spp.h"
#include "btl_apphandle.h"
#include "btl_common.h"
#include "btl_unicode.h"


/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BtlSppServiceType type
 *
 *	    Type of services which may be used by connecting SPP ports.
 */
typedef U8 BtlSppServiceType;

/* Dial-up networking */
#define  BTL_SPP_SERVICE_TYPE_DUN           (0x01)

/* Fax */	
#define  BTL_SPP_SERVICE_TYPE_FAX           (0x02)

/* Serial port */	
#define  BTL_SPP_SERVICE_TYPE_SPP           (0x03)

/*-------------------------------------------------------------------------------
 * BtlSppPortSettings type
 *
 *     Defines the BTL SPP port settings.
 */
typedef struct _BtlSppPortSettings
{
    /* Type of serial port - SPP_SERVER_PORT or SPP_CLIENT_PORT */
    SppPortType             portType;
    
    /* Type of service which will be used with serial port */
    BtlSppServiceType       serviceType;

    /* Types of Tx and Rx data paths - synchronous or asynchronous */
    SppPortDataPathType     dataPathTypes;

    /* 
    *		Max number of Rx Sync packets - this value will be given to peer serial
    * 		port as amount of credits 
    */
    U8                      maxNumOfRxSyncPackets;

    /* Communication settings of the port */
    SppComSettings          comSettings;

} BtlSppPortSettings;


/* Forward declarations */
typedef struct _BtlSppPortEvent BtlSppPortEvent;
typedef struct _BtlSppPortContext BtlSppPortContext;

/*-------------------------------------------------------------------------------
 * BtlSppCallBack type
 *
 *     A function of this type is called to indicate BTL SPP events.
 */
typedef void (*BtlSppPortCallback)(const BtlSppPortEvent *event);

/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/
 
/*-------------------------------------------------------------------------------
 * BtlSppEvent structure
 *
 *     Represents BTL SPP event.
 */
struct _BtlSppPortEvent 
{
	/* Context of port event received from */
	BtlSppPortContext	*portContext;
	
	/* SPP event information */
	SppCallbackParms	*parms;
};

/********************************************************************************
 *
 * Function declarations
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BTL_SPP_Init()
 *
 * Brief:  
 *		Init the SPP module.
 *
 * Description:
 *		Initializes BTL SPP module and allocates required resources.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	void.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - if the module was successfully initialized.
 *
 *		BT_STATUS_FAILED - or any specific error defined in BtStatus type,
 *			if the initialization failed.
 */
BtStatus BTL_SPP_Init(void);

/*-------------------------------------------------------------------------------
 * BTL_SPP_Deinit()
 *
 * Brief:   
 *		Deinit the SPP module.
 *
 * Description:
 *		Deinitializes BTL SPP module and frees allocated resources.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	None.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - if the module was successfully deinitialized.
 *
 *		BT_STATUS_FAILED - or any specific error defined in BtStatus type,
 *			if the initialization failed.
 */
BtStatus BTL_SPP_Deinit(void);

/*-------------------------------------------------------------------------------
 * BTL_SPP_Create()
 *
  * Brief:  
 *      Creates either SPP client or SPP server.
 *
 * Description:
 *     	Creates context for either SPP client or SPP server port. 
 *		Callback function will be registered for delivering SPP events 
 *		to  the application.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	appHandle [in] - handle of the application calling the function.
 *
 *     	portCallback [in] - callback funtion for receiving port events.
 *
 *		securityLevel [in] - pointer to level of security which should be applied,
 *			when a remote device will try to connect to the server port's service
 *			or when a local device will try to connect the client port to the
 *			remote SPP port.
 *			NULL may be passed, if application does not want to overwrite a default
 *			value set by a profile or a default value BSL_DEFAULT of the entire
 *			program, if BT_DEFAULT_SECURITY is enabled.
 *
 *     	portContext [out] - pointer to the created port's context, returned by
 *			the	function.
 * Returns:
 *		BT_STATUS_SUCCESS - if the port was successfully created.
 *
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type,
 *			if the creation failed.
 */
BtStatus BTL_SPP_Create(BtlAppHandle *appHandle,
					  	const BtlSppPortCallback portCallback,
						const BtSecurityLevel *securityLevel,
						BtlSppPortContext **portContext);

/*-------------------------------------------------------------------------------
 * BTL_SPP_Destroy()
 *
 * Brief:  
 *		Releases a SPP context (previously allocated with BTL_SPP_Create).
 *
 * Description:
 *     	Removes previously created SPP client or SPP server.
 *		If the connection to the modem exists, it will be disconnected.
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	sppPortContext [in/out] - SPP port context.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - if a port was destroyed successfully.
 *
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type,
 *			if the operation failed.
 */
BtStatus BTL_SPP_Destroy(BtlSppPortContext **portContext);
       
/*-------------------------------------------------------------------------------
 * BTL_SPP_Enable()
 *
 * Brief:
 *		Enables SPP, called after BTL_SPP_Create.

 *
 * Description:
 *     	Enables previously created SPP port.
 *		In case of a server port, specified service record will be added to
 *		services database.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	sppPortContext [in] - pointer to the created port's context, returned at
 *			the port creation.
 *
 *		serviceName [in] - 
 *			1. In case of a server port, service name which will be written into
 *			services database and which may be discovered by the peer device. If
 *			NULL is	passed,	default values "COM1", "COM2", etc. will be used.
 *			2. In case of a client port and service type is BTL_SPP_SERVICE_TYPE_SPP, 
 *			the service name will be matched during establishing
 *			a connection to the remote server device.
 *
 *	 	portSettings [in] - pointer to a structure with settings of the port to
 *         		 be created.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - if the port wass successfully open.
 *
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type,
 *			if the operation failed.
 */
BtStatus BTL_SPP_Enable(BtlSppPortContext *portContext,
						const BtlUtf8 *serviceName,
						const BtlSppPortSettings *portSettings);

/*-------------------------------------------------------------------------------
 * BTL_SPP_Disable()
 *
 * Brief:  
 *     	Disables previously enabled SPP port.
 *
 * Description:
 *     	Disables previously enabled SPP port.
 *		In case of a server port, service record will be removed from the services
 *		database.
 *		If connection to the remote port exists, it will be	disconnected.
 *     
 * Type:
 *		Asynchronous or synchronous.
 *
 * Parameters:
 *     	sppPortContext [in] - pointer to the created port's context, returned at
 *			the port creation.
 *
  * Generated Events:
 *	 	SPP_EVENT_DISABLED
 *
 * Returns:
 *		BT_STATUS_SUCCESS - if the port was successfully disabled.
 *
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type,
 *			if the operation failed.
 *
 *		BT_STATUS_PENDING - if the disabling has been successfully started.
 *			SPP_EVENT_DISABLED event will be received upon successful port disable.
 */
BtStatus BTL_SPP_Disable(BtlSppPortContext *portContext);

/*-------------------------------------------------------------------------------
 * BTL_SPP_Connect()
 *
 * Brief:  
 *      	This function is used to the enabled client to remote device.
 *
 * Description:
 *     	Connects previously created and opened SPP client port to requested
 *		service in requested remote device.
 *
 * Type:
 *		Asynchronous or synchronous.
 *
 * Parameters:
 *     	sppPortContext [in] - pointer to the created port's context, returned at
 *			the port creation.
 *
 *		bdAddr [in] - pointer to 48-bit address of the remote device, 
 *
  * Generated Events:
 *      	SPP_EVENT_SDP_QUERY_FAILED
 *	  	SPP_EVENT_ACL_CREATION_FAILED
 *      	SPP_EVENT_OPEN
 *
 * Returns:
 *		BT_STATUS_SUCCESS - if the port was already connected.
 *
 *		BT_STATUS_PENDING - if the connection process has been successfully
 *			started. The application will be notified by event about a result of
 *			the connection establishment, i.e. by event	SPP_EVENT_OPEN in
 *			successful case and, i.e. by event SPP_EVENT_SDP_QUERY_FAILED, if
 *			requested service was not found in the requested remote device.
 *			The application will receive SPP_EVENT_ACL_CREATION_FAILED event,
 *			when establishing of ACL link for SPP client port to server port failed.
 *
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type,
 *			if the operation failed to start.
 */
BtStatus BTL_SPP_Connect(BtlSppPortContext *portContext,
						 const BD_ADDR *bdAddr);

/*-------------------------------------------------------------------------------
 * BTL_SPP_Disconnect()
 *
 *
 * Brief: 
 *     	Disconnects either client or server SPP port from the SPP port in the
 *			remote device.
 *
 * Description:
 *		Disconnects either client or server SPP port from the SPP port in the
 *		remote device.
 *		All data in receive and transmit buffers will be discarded.
 *		In case of a client port, it will be also closed.
 *		In case of a server port, it will continue to listen.
 *     
 * Type:
 *		Asynchronous or Synchronous.
 *
 * Parameters:
 *     	sppPortContext [in] - pointer to the created port's context, returned at
 *			the port creation.
 *
 * Generated Events:
 *      	SPP_EVENT_CLOSED
 * Returns:
 *		BT_STATUS_PENDING - if disconnection process has been successfully started.
 *			The application will be notified by event SPP_EVENT_CLOSED, when the
 *			disconnection is completed and the port is closed.
 *
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type,
 *			if the operation failed to start.
 */
BtStatus BTL_SPP_Disconnect(BtlSppPortContext *portContext);

/*-------------------------------------------------------------------------------
 * BTL_SPP_GetConnectedDevice()
 *
  * Brief:  
 *      This function returns the connected device.
 *    
 * Description:
 *     	Gets BD ADDR of the connected to the local SPP port remote device.
 *     
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	sppPortContext [in] - pointer to the created port's context, returned at
 *			the port creation.
 *
 *		bdAddr [out] - pointer to get 48-bit address of the connected remote
 *			device.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - if BD ADDR of the remote device was successfully
 *			returned.
 *
 *		BT_STATUS_NO_CONNECTION - if the port is not connected.
 *
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type,
 *			if the operation failed to start.
 */
BtStatus BTL_SPP_GetConnectedDevice(BtlSppPortContext *portContext,
									BD_ADDR *bdAddr);

/*-------------------------------------------------------------------------------
 * BTL_SPP_ReadAsync()
 *
 * Brief:  
 *		Reads data Asyncroniosly.
 *
 * Description:
 *     	Reads data from previously opened SPP port.
 *      	Requires that the port was opened with Rx data path type
 *          	BTL_SPP_PORT_DATA_PATH_RX_ASYNC.
 *      	Received data was previously read by SPP profile into its interim Rx
 *          	buffer.
 *     
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	sppPortContext [in] - pointer to the created port's context, returned at
 *			the port creation.
 *
 *     	buffer [in] - buffer in which to receive data,
 *
 *     	maxBytes [in]	- maximum number of bytes to place in buffer,
 *
 *     	readBytes [out] - amount of actually read bytes.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - if read operation was successful.
 *
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type,
 *			if the operation failed.
 */
BtStatus BTL_SPP_ReadAsync(BtlSppPortContext *portContext,
					       U8 *buffer,
					       U16 maxBytes,
					       U16 *readBytes);
 
/*-------------------------------------------------------------------------------
 * BTL_SPP_WriteSync()
 *
 * Brief:  
 * 		Writes data to an open port synchroniosly.
 *
 * Description:
 *     	Writes data into previously opened SPP port.
 *      	Requires that the port was opened with Tx data path type
 *          	BTL_SPP_PORT_DATA_PATH_TX_SYNC.
 *     
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	sppPortContext [in] - pointer to the created port's context, returned at
 *			the port creation.
 *
 *     	buffer [in] - buffer containing data to write.
 *
 *     	maxBytes [in]	- number of bytes to write.
 *
 *     	writtenBytes [out] - amount of actually written bytes.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - if write operation was successful.
 *
 *		BT_STATUS_NO_RESOURCES - if there is no room in write buffer (zero will be
 *			returned in 'writtenBytes' parameter).
 *
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type,
 *			if the operation failed.
 */
BtStatus BTL_SPP_WriteSync(BtlSppPortContext *portContext,
					       const U8 *buffer,
					       U16 nBytes,
					       U16 *writtenBytes);
 
/*-------------------------------------------------------------------------------
 * BTL_SPP_WriteAsync()
 *
 *     	Writes data into previously opened SPP port directly into RFCOMM layer.
 *      Requires that the port was opened with Tx data path type
 *          BTL_SPP_PORT_DATA_PATH_TX_ASYNC.
 *      The application will receive SPP_EVENT_TX_DATA_COMPLETE event, when all
 *          the data is sent.
 *
 * Type:
 *		Asynchronous.
 *
 * Parameters:
 *     	sppPortContext [in] - pointer to the created port's context, returned at
 *			the port creation.
 *
 *     	buffer [in] - buffer containing data to write.
 *
 *     	maxBytes [in]	- number of bytes to write.
 *
 * Generated Events:
 *      SPP_EVENT_TX_DATA_COMPLETE
 *
 * Returns:
 *		BT_STATUS_PENDING - if write operation successfully started, event
 *			SPP_EVENT_TX_DATA_COMPLETE will be delivered asynchronously.
 *
 *		BT_STATUS_NO_RESOURCES - if there is no room in write buffer (zero will be
 *			returned in 'writtenBytes' parameter).
 *
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type,
 *			if the operation failed.
 */
BtStatus BTL_SPP_WriteAsync(BtlSppPortContext *portContext,
					        const U8 *buffer,
					        U16 nBytes);
 
/*-------------------------------------------------------------------------------
 * BTL_SPP_GetRxAsyncDataSize()
 *
 * Brief:  
 *		Gets a size of data received in port's read buffer.
 *
 * Description:
 *		Gets a size of data received in port's read buffer.
 *      	Requires that the port was opened with Rx data path type
 *          	BTL_SPP_PORT_DATA_PATH_RX_ASYNC.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	sppPortContext [in] - pointer to the created port's context, returned at
 *			the port creation.
 *
 *	 	dataReadSize [out] - pointer to a size of data received in the port's
 *			read buffer.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - if a size of data received in the port's read buffer
 *			was successfully returned.
 *
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type,
 *			if the operation failed.
 */
BtStatus BTL_SPP_GetRxAsyncDataSize(BtlSppPortContext *portContext,
								    U16 *dataReadSize);

/*-------------------------------------------------------------------------------
 * BTL_SPP_GetTxSyncDataSize()
 *
 * Brief:  
 *		Gets a size of data which is still in port's write buffer.
 *
 * Description:
 *		Gets a size of data which is still in port's write buffer.
 *    		Requires that the port was opened with Tx data path type
 *          	BTL_SPP_PORT_DATA_PATH_TX_ASYNC.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	sppPortContext [in] - pointer to the created port's context, returned at
 *			the port creation.
 *
 *	 	dataWriteSize [out] - pointer to a size of data which is still in the
 *			port's write buffer.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - if a size of free space in the port's write buffer
 *			was successfully returned.
 *
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type,
 *			if the operation failed.
 */
BtStatus BTL_SPP_GetTxSyncDataSize(BtlSppPortContext *portContext,
								   U16 *dataWriteSize);

/*-------------------------------------------------------------------------------
 * BTL_SPP_GetFreeRxAsyncSize()
 *
 * Brief:  
 *		Gets a size of free space in port's read buffer.
 *
 * Description:
 *		Gets a size of free space in port's read buffer.
 *      	Requires that the port was opened with Rx data path type
 *          BTL_SPP_PORT_DATA_PATH_RX_ASYNC.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	sppPortContext [in] - pointer to the created port's context, returned at
 *			the port creation.
 *
 *	 	freeReadSize [out] - pointer to a size of free space in the port's read
 *			buffer.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - if a size of free space in the port's read buffer
 *			was successfully returned.
 *
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type,
 *			if the operation failed.
 */
BtStatus BTL_SPP_GetFreeRxAsyncSize(BtlSppPortContext *portContext,
								    U16 *freeReadSize);

/*-------------------------------------------------------------------------------
 * BTL_SPP_GetFreeTxSyncSize()
 *
 * Brief:  
 *		Gets a size of free space in port's write buffer.
 *
 * Description:
 *		Gets a size of free space in port's write buffer.
 *      Requires that the port was opened with Tx data path type
 *          BTL_SPP_PORT_DATA_PATH_TX_ASYNC.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	sppPortContext [in] - pointer to the created port's context, returned at
 *			the port creation.
 *
 *	 	freeWriteSize [out] - type of buffers to which flush will be performed.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - if a size of free space in the port's write buffer
 *			was successfully returned.
 *
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type,
 *			if the operation failed.
 */
BtStatus BTL_SPP_GetFreeTxSyncSize(BtlSppPortContext *portContext,
								   U16 *freeWriteSize);

/*-------------------------------------------------------------------------------
 * BTL_SPP_Flush()
 *
 * Brief:  
 *		Flushes required types of data path.
 *
 * Description:
 *		Flushes required types of data path.
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	sppPortContext [in] - pointer to the created port's context, returned at
 *			the port creation.
 *
 *	 	flushType [in] - type of buffers to which flush will be performed.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - if flush was successfully performed.
 *
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type,
 *			if the flush operation failed.
 */
BtStatus BTL_SPP_Flush(BtlSppPortContext *portContext,
					   SppFlushType flushType);

/*-------------------------------------------------------------------------------
 * BTL_SPP_SetSecurityLevel()
 *
 * Brief:  
 *      Sets security level.
 *
 * Description:
 *     	Sets security level for previously created SPP port.
 *      	Requires compilation switch BT_SECURITY is enabled.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	sppPortContext [in] - pointer to the created port's context, returned at
 *			the port creation.
 *
 *		securityLevel [in] - level of security which should be applied,	when
 *			connection process to or from the remote SPP port is started.
 *			NULL may be passed, if application wants to return to a default
 *			value defined for a specific profile.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - if the security level was successfully changed.
 *
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type,
 *			if the operation failed.
 */
BtStatus BTL_SPP_SetSecurityLevel(BtlSppPortContext *portContext,
								  const BtSecurityLevel *securityLevel);

/*-------------------------------------------------------------------------------
 * BTL_SPP_GetSecurityLevel()
 *
* Brief:  
 *      Gets security level.
 *
 * Description:
 *     	Gets security level for previously created SPP  port.
 *      	Requires compilation switch BT_SECURITY is enabled.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	sppPortContext [in] - pointer to the created port's context, returned at
 *			the port creation.
 *
 *		securityLevel [out] - level of security which should be applied, when
 *			connection process to or from the remote SPP port is started.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - if the security level was successfully returned.
 *
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type,
 *			if the operation failed.
 */
BtStatus BTL_SPP_GetSecurityLevel(BtlSppPortContext *portContext,
								  BtSecurityLevel *securityLevel);

/*-------------------------------------------------------------------------------
 * BTL_SPP_SetComSettings()
 *
 * Brief:  
 *		Sets either SPP client or SPP server port's communication settings.
 *
 * Description:
 *		Sets either SPP client or SPP server port's communication settings.
 *		An application should assure that the communication settings are
 *		correctly set prior sending data.
 *
 * Type:
 *		Asynchronous or Synchronous.
 *
 * Parameters:
 *     	sppPortContext [in] - pointer to the created port's context, returned at
 *			the port creation.
 *
 *	 	comSettings [in] - pointer to a structure with communication settings
 *			of the port to be created (baudrate, data format... originally defined
 *			in file rfcomm.h).
 *
 * Returns:
 *		BT_STATUS_PENDING - if write operation successfully started. Event
 *			SPP_EVENT_PORT_STATUS_CNF will be delivered asynchronously, when a
 *			response will be received from the remote port.
 *
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type,
 *			if the creation failed.
 */
BtStatus BTL_SPP_SetComSettings(BtlSppPortContext *portContext,
								const SppComSettings *comSettings);

/*-------------------------------------------------------------------------------
 * BTL_SPP_GetComSettings()
 *
* Brief:  
 *      Gets communication settings.
 *
 * Description:
 *		Gets SPP's server or client communication settings.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	sppPortContext [in] - pointer to the created port's context, returned at
 *			the port creation.
 *
 *	 	comSettings [out] - pointer to a structure with returned communication
 *			settings of	the port (baudrate, data format... originally defined
 *			in file rfcomm.h).
 *
 * Returns:
 *		BT_STATUS_SUCCESS - if the port's properties were successfully returned.
 *
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type,
 *			if the creation failed.
 */
BtStatus BTL_SPP_GetComSettings(BtlSppPortContext *portContext,
								SppComSettings *comSettings);

/*-------------------------------------------------------------------------------
 * BTL_SPP_SetControlSignals()
 *
 * Brief: 
 *		Sets either SPP client or SPP server port's control signals.
 *
 * Description:
 *		Sets either SPP client or SPP server port's control signals.
 *
 * Type:
 *		Asynchronous or Synchronous.
 *
 * Generated Events:
 *		SPP_EVENT_MODEM_STATUS_CNF
 * Parameters:
 *     	sppPortContext [in] - pointer to the created port's context, returned at
 *			the port creation.
 *
 *	 	controlSignals [in] - pointer to a structure with required control
 *			signals to be set in the port.
 *
 * Returns:
 *		BT_STATUS_PENDING - if sending control signals to the remote port was
 *			successful. Event SPP_EVENT_MODEM_STATUS_CNF will be delivered
 *			asynchronously, when a response will be received from the remote port.
 *
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type,
 *			if the operation failed to start.
 */
BtStatus BTL_SPP_SetControlSignals(BtlSppPortContext *portContext,
								   const SppControlSignals *controlSignals);

/*-------------------------------------------------------------------------------
 * BTL_SPP_GetControlSignals()
 *
 * Brief: 
 *		Gets either SPP client or SPP server port's control signals.
 *
 * Description:
 *		Gets either SPP client or SPP server created port's control signals .
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	sppPortContext [in] - pointer to the created port's context, returned at
 *			the port creation.
 *
 *	 	controlSignals [out] - pointer to a structure with returned port's
 *			control signals.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - if the port's control signals were successfully
 *			returned.
 *
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type,
 *			if the creation failed.
 */
BtStatus BTL_SPP_GetControlSignals(BtlSppPortContext *portContext,
								   SppControlSignals *controlSignals);

/*-------------------------------------------------------------------------------
 * BTL_SPP_SetLineStatus()
 *
 * Brief: 
 *		Sets either SPP client or SPP server port's control signals.
 *
 * Description:
 *		Sets either SPP client or SPP server port's control signals.
 *
 * Type:
 *		Asynchronous or Synchronous.
 *
 * Parameters:
 *     	sppPortContext [in] - pointer to the created port's context, returned at
 *			the port creation.
 *
 *	 	lineStatus [in] - line status to be set.
 *
 * Generated Events:
 *		SPP_EVENT_LINE_STATUS_CNF
 *
 * Returns:
 *		BT_STATUS_PENDING - if sending line status to the remote port was
 *			successful. Event SPP_EVENT_LINE_STATUS_CNF will be delivered
 *			asynchronously, when a response will be received from the remote port.
 *
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type,
 *			if the operation failed to start.
 */
BtStatus BTL_SPP_SetLineStatus(BtlSppPortContext *portContext,
							   SppLineStatus lineStatus);
/*-------------------------------------------------------------------------------
 * BTL_SPP_SetServiceName()
 *
 * Brief: 
 *		Sets either SPP client or SPP server port's service name.
 *
 * Description:
 *		Sets either SPP client or SPP server port's service name.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *		sppPortContext [in] - pointer to the created port's context, returned at
 *			the port creation.
 *
 *		service name [in] - service name.
 *
 *
 * Returns:
 *
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type,
 *			if the operation failed to start.
 */

BtStatus BTL_SPP_SetServiceName(BtlSppPortContext *portContext,
							   const BtlUtf8 *serviceName);
/*-------------------------------------------------------------------------------
 * BTL_SPP_GetServiceName()
 *
 * Brief: 
 *		Gets either SPP client or SPP server port's service name.
 *
 * Description:
 *		Gets either SPP client or SPP server port's service name.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *		sppPortContext [in] - pointer to the created port's context, returned at
 *			the port creation.
 *
 *		service name [out] - the returned service name.
 *
 *
 * Returns:
 *
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type,
 *			if the operation failed to start.
 */

BtStatus BTL_SPP_GetServiceName(BtlSppPortContext *portContext,
							    BtlUtf8 *serviceName);



#endif /* __BTL_SPP_H */





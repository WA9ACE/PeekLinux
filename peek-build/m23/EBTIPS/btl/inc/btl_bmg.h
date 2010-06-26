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
*   FILE NAME:      btl_bmg.h
*
*   BRIEF:          This file defines the API of the BTL Bluetooth manager.
*
*   DESCRIPTION:    General
*
*					The BTL_BMG is a bluetooth manager abstraction that provides the procedures  
*                   used by the applications of general BT managment.
*
*					Usage Models
*
*					The processes covered by the BTL_BMG are the following:
*
*					A. BT configuration - 
*							control over the system variables (read and write) such as accessible modes, security modes 
*							local class of device, local name, send HCI command etc.
*					B. BT device discovery - 
*							provides the application the ability to aply search (and to cancel it) 
*							for BT devices in the 
*							environment with additional data fetching such as SDP service discovery or 
*							name request and various filters.
*					C. Fatching remote device characteristics -
*							Gets information such as name, suported services ,class of device etc.
*					D. SDP Processes abstruction - 
*							apply SDP queries for remote device
*					E. DSP server abstraction -
*							provide the ability to register, delete and update SDP records.	
*					F. Data Base services
*							provides the ability to save remote devices data in a non volatile memory.
*					G. Security processes API - 
*							Such as: bond/unbond a remote device, pin reply, authorize a remote device 
*							set connection encrypted etc.
*
*                   
*   AUTHOR:   Gili Friedman
*
\*******************************************************************************/


#ifndef __BTL_BMG_H
#define __BTL_BMG_H


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "bttypes.h"
#include "me.h"
#include "medev.h"
#include "sec.h"
#include "sdp.h"
#include "btl_common.h"
#include "btl_config.h"
#include "btips_retrieve_srv.h"


/********************************************************************************
 *
 * Constants
 *
 *******************************************************************************/

/*
	The maximum allowed length of the local name
*/
#define BTL_BMG_MAX_LOCAL_NAME_LEN			(248)

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/* Forward declarations */
typedef struct _BtlBmgEvent 	BtlBmgEvent;
typedef struct _BtlBmgContext 	BtlBmgContext;

 
/*-------------------------------------------------------------------------------
 * BtlBmgCallBack type
 *
 *     A function of this type is called to indicate BTL BMG events.
 */
typedef void (*BtlBmgCallBack)(const BtlBmgEvent *event);


/*---------------------------------------------------------------------------
 * BtlBmgBtVersion type
 *
 *     Defines the BT version.
 */
typedef U8 BtlBmgBtVersion;

#define BTL_BMG_BT_VERSION_1_0			(0)		/* BT v1.0 */
#define BTL_BMG_BT_VERSION_1_1			(1)		/* BT v1.1 */
#define BTL_BMG_BT_VERSION_1_2			(2)		/* BT v1.2 */
#define BTL_BMG_BT_VERSION_2_0			(3)		/* BT v2.0 */


/*-------------------------------------------------------------------------------
 * BtlBmgEventForwardingMode type
 *
 *     Each BMG callback has the ability to mask all or some of the events, 
 *	   according the mode given in BTL_BMG_SetEventForwardingMode().
 */
typedef U8 BtlBmgEventForwardingMode;

#define BTL_BMG_EVENT_FORWARDING_ALL_EVENTS			(0x00)	
#define BTL_BMG_EVENT_FORWARDING_MY_EVENTS_ONLY		(0x01)	

#define BTL_BMG_DFLT_EVENT_FORWARDING_MODE				\
			(BTL_BMG_EVENT_FORWARDING_MY_EVENTS_ONLY)

/*---------------------------------------------------------------------------
 * BtlBmgSearchMethod type
 *
 *     Defines the search method.
 *
 *		BTL_BMG_SEARCH_REGULAR - perform the following stages:
 *				1) perform inquiry.
 *				2) perform name request on each found device (if needed).
 *				3) perform SDP service discovery on each found device (if needed).
 *
 *		BTL_BMG_SEARCH_DEVICE_ORIENTED - perform the following stages:
 *				1) perform inquiry.
 *				2) if a new device is not found, quit.
 *				3) cancel inquiry once first NEW device is found.
 *				4) perform name request on new found device (if needed).
 *				5) perform SDP service discovery on new found device (if needed).
 *				6) go to stage 1.
 */
typedef U8 BtlBmgSearchMethod;

#define BTL_BMG_SEARCH_REGULAR						(0x00)
#define BTL_BMG_SEARCH_DEVICE_ORIENTED				(0x01)


/*---------------------------------------------------------------------------
 * BtlBmgSearchCodFilterMask type
 *
 *     Defines which COD field to use when filtering inquiry results.
 *	   Can be ORed together.
 */
typedef U8 BtlBmgSearchCodFilterMask;

#define BTL_BMG_SEARCH_COD_FILTER_NONE				(0x00)
#define BTL_BMG_SEARCH_COD_FILTER_SERVICE_CLS		(0x01)
#define BTL_BMG_SEARCH_COD_FILTER_MAJOR_DEV_CLS		(0x02)
#define BTL_BMG_SEARCH_COD_FILTER_MINOR_DEV_CLS		(0x04)

/* 
 *				type			 | In BTL_BMG_AuthorizeDeviceReply		        |   In BTL_BMG_AuthorizeServiceReply
 *=======================================================================================================================
 * BTL_BMG_NOT_AUTHORIZED        | Do not authorize							 	| Do not authorize
 *-----------------------------------------------------------------------------------------------------------------------
 * BTL_BMG_AUTHORIZED_NOT_TRUSTED| Authorize device for the remaining of 		| Authorize service one time (next 
 *								 | this connection but it is not trusted		| service that require authorization 
 *								 |												| will ask the application again for 
 *								 |												| authorization) and it is not trusted.
 *-----------------------------------------------------------------------------------------------------------------------
 * BTL_BMG_AUTHORIZED_TRUSTED    | Authorize device this time and it is trusted | Authorize service this time and it is 
 *								 |												| trusted for this device
*/										
typedef U8 BtlBmgAuthorizationType;			
#define BTL_BMG_NOT_AUTHORIZED        0x00 	
#define BTL_BMG_AUTHORIZED_NOT_TRUSTED 0x01 
#define BTL_BMG_AUTHORIZED_TRUSTED     0x02 

/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BtlBmgEvent structure
 *
 *     Represents BTL BMG event.
 */
struct _BtlBmgEvent 
{
	BtlBmgContext	*bmgContext;
	const BtEvent		*event;
};


/*-------------------------------------------------------------------------------
 * BtlBmgCodFilter structure
 *
 *     Represents a class of device filter.
 */
typedef struct _BtlBmgCodFilter 
{
	/* Defines which COD fields to use when filtering inquiry results, 
	can be ORed together. */
	BtlBmgSearchCodFilterMask	codFilterMask;	

	/* Service Classes to filter in. Can be ORed together.
	Used only if the BTL_BMG_SEARCH_COD_FILTER_SERVICE_CLS is set in codFilterMask. */
	BtClassOfDevice 			serviceClass;

	/* Major Device Classes to filter in (Select one).
	Used only if the BTL_BMG_SEARCH_COD_FILTER_MAJOR_DEV_CLS is set in codFilterMask. */
	BtClassOfDevice 			majorDeviceClass;

	/* Minor Device Classes to filter in (Select one, according to Major Device Class).
	Used only if the BTL_BMG_SEARCH_COD_FILTER_MINOR_DEV_CLS is set in codFilterMask. */
	BtClassOfDevice 			minorDeviceClass;

} BtlBmgCodFilter;

typedef struct _BtlBmgDeviceRecord
{
	BD_ADDR         bdAddr;
	BOOL            trusted;
	SdpServicesMask trustedPerService;
	U8              linkKey[16];
	BtLinkKeyType   keyType; 
	U8							pinLen;	/* length of the pin used for the last pairing.*/

	
	BtClassOfDevice 		classOfDevice; /* updated in BTL_BMG_GetRemDevCod.*/
	SdpServicesMask	services;	/* updated in BTL_BMG_DiscoverServices response event.*/
	U8					name[BT_MAX_REM_DEV_NAME + 1]; /* updated in BTL_BMG_GetRemoteDeviceName.*/
	
	psiToSave psi;	/* updated after inquiry.*/
	
	U8	userData[BTL_BMG_DDB_CONFIG_DEVICE_REC_USER_DATA_SIZE];	
	
} BtlBmgDeviceRecord;


/********************************************************************************
 *
 * Function declarations
 *
 *******************************************************************************/
/*-------------------------------------------------------------------------------
 * BTL_BMG_Init()
 *
 * Brief:  
 *		Initializes BTL BMG module.
 *
 * Description:
 *    This function must be called by the system before any other BMG API function.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		void.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - BTL was initialized successfully.
 *
 *		BT_STATUS_FAILED -  BTL failed initialization.
 */
BtStatus BTL_BMG_Init(void);

/*-------------------------------------------------------------------------------
 * BTL_Deinit()
 *
 * Brief:  
 *		Deinitializes BTL BMG module.
 *
 * Description:
 *		After calling this function, no BMG API function can be called.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		void
 *
 * Returns:
 *		BT_STATUS_SUCCESS - BTL was deinitialized successfully.
 *
 *		BT_STATUS_FAILED -  BTL failed deinitialization.
 */
BtStatus BTL_BMG_Deinit(void);
 
/*-------------------------------------------------------------------------------
 * BTL_BMG_Create()
 *
 * Brief:  
 *		Allocates a unique BMG context.
 *
 * Description:
 *		This function must be called before any other BMG API function.
 *		The allocated context should be supplied in subsequent BMG API calls.
 *		The caller must also provide a callback function, which will be called 
 *		on BMG events.
 *		The caller can provide an application handle (previously allocated 
 *		with BTL_RegisterApp), in order to link between different modules.
 *		If there is no need to link between different modules, set appHandle to 0.
 *		Default event forwarding mode is BTL_BMG_EVENT_FORWARDING_ALL_EVENTS.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		appHandle [in] - application handle, can be 0.
 *
 *		bmgCallback [in] - all BMG events will be sent to this callback.
 *		
 *		bmgContext [out] - allocated BMG context.	
 *
 * Returns:
 *		BT_STATUS_SUCCESS - BMG context was created successfully.
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 *		
 *		BT_STATUS_INTERNAL_ERROR - Error allocating context 
 *
 *		BT_STATUS_NO_RESOURCES - No resources for the context
 */
BtStatus BTL_BMG_Create(BtlAppHandle *appHandle,
						const BtlBmgCallBack bmgCallback, 
						BtlBmgContext **bmgContext);


/*-------------------------------------------------------------------------------
 * BTL_BMG_Destroy()
 *
 * Brief:  
 *		Releases a BMG context (previously allocated with BTL_BMG_Create).
 *
 * Description:
 *		An application should call this function when it completes using BMG services.
 *		Upon completion, the BMG context is set to null in order to prevent 
 *		the application from an illegal attempt to keep using it.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		bmgContext [in/out] - BMG context.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - BMG context was destroyed successfully.
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 *		
 *		BT_STATUS_FAILED -  Failed to destroy BMG context.
 */
BtStatus BTL_BMG_Destroy(BtlBmgContext **bmgContext);


/*-------------------------------------------------------------------------------
 * BTL_BMG_SetEventForwardingMode()
 *
 * Brief:  
 *		Set the BMG event forwarding mode for the given BMG context.
 *
 * Description:
 *		Determines whether all system events will be forwarded to the application or only
 *		ones that were initiated by the given context. When working with one context only it is
 *		highly recommended to use BTL_BMG_EVENT_FORWARDING_ALL_EVENTS in order not to miss desirable events.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *		
 *		mode [in] - BMG event forwarding mode.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - BMG event forwarding mode was set successfully.
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 *		
 *		BT_STATUS_FAILED -  Failed to set BMG event forwarding mode.
 */
BtStatus BTL_BMG_SetEventForwardingMode(BtlBmgContext *bmgContext, 
										const BtlBmgEventForwardingMode mode);

/*-------------------------------------------------------------------------------
 * BTL_BMG_SearchByCod()
 *
 * Brief:  
 *		Searches for Bluetooth devices in the surrounding environment that pass the given COD filter.
 *
 * Description:
 *		searches for BT devices and filter results by class of device, if needed.
 *		This function first performs an inquiry and filter the results according 
 *		to the defined COD filter (See BtlBmgCodFilter for details).
 *		Then, if needed, it performs a name request for each filtered in device.
 *		Afterwards, if needed, if performs SDP query on each filtered in device, 
 *		in order to discover whether the device supports the specified services.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 *		method [in] - Defines the search method (see BtlBmgSearchMethod). 
 *		
 *		lap [in] - LAP used for the Inquiry (General or Limited).
 *
 *		length [in] - Maximum amount of time before the Inquiry is halted.
 *			Range is 0x01 to 0x30. Time is length * 1.28 sec.
 *			The Generic Access profile specifies using the value BT_INQ_TIME_GAP100.
 *
 *		maxResp [in] - The maximum number of filtered in responses. 
 *			Specify zero to receive an unlimited number of responses.
 *	
 *		filter [in] - Defines the class of device filter on inquiry results (see BtlBmgCodFilter).
 *			If no filtering is needed, set to 0.
 *		
 *		performNameRequest [in] - Performs name request on each discovered device 
 *			(which was not filtered out).
 *
 *		sdpServicesMask [in] - Performs service discovery on the specified set of SDP 
 *			services on each discovered device (which was not filtered out).
 *			Set SDP_SERVICE_NONE when no service discovery is needed.
 *
 * Generated events:
 *		BTEVENT_INQUIRY_RESULT
 *		BTEVENT_INQUIRY_COMPLETE
 *		BTEVENT_NAME_RESULT
 *		BTEVENT_DISCOVER_SERVICES_RESULT
 *		BTEVENT_SEARCH_COMPLETE
 *
 * Returns:
 *		BT_STATUS_PENDING - The Search process is started results
 *         will be sent via the BMG handler. A BTEVENT_HCI_FATAL_ERROR event
 *         indicates a fatal radio or HCI transport error and that all
 *         pending operations have failed.
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 *
 *		BT_STATUS_NO_RESOURCES - Not enough resources for execution
 *
 *    BT_STATUS_IN_PROGRESS - An Search process is already in 
 *         progress. Only one Search can be in progress at a time.  
 *         Keep track of the BMG events to get the results from 
 *         the current Search or to see when it ends. If it has just
 *         ended then a cancel is also in progress so wait for
 *         the cancel to complete to start another search.
 *
 *    BT_STATUS_FAILED - The operation failed. 
 *
 *    BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the BMG events to
 *         be notified when the error has been corrected.
 */
BtStatus BTL_BMG_SearchByCod(BtlBmgContext 				*bmgContext, 
									const BtlBmgSearchMethod	 	method,
									const BtIac 					lap, 
									const U8 						length, 
									const U8 						maxResp, 
									const BtlBmgCodFilter 			*filter,
									const BOOL 					performNameRequest,
									const SdpServicesMask 	sdpServicesMask);


/*-------------------------------------------------------------------------------
 * BTL_BMG_SearchByDevices()
 *
 * Brief:  
 *		Searches for Bluetooth devices in the surrounding environment that pass the given devices addrs filter.
 *
 * Description:
 *		Search for Bluetooth devices, filter results by given list of devices.
 *		This function first performs an inquiry and filter the results according 
 *		to the given list of devices.
 *		Then, if needed, it performs a name request for each filtered in device.
 *		Afterwards, if needed, if performs SDP query on each filtered in device, 
 *		in order to discover whether the device supports the specified services.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 *		method [in] - Defines the search method (see BtlBmgSearchMethod). 
 *		
 *		lap [in] - LAP used for the Inquiry (General or Limited).
 *
 *		length [in] - Maximum amount of time before the Inquiry is halted.
 *			Range is 0x01 to 0x30. Time is length * 1.28 sec.
 *			The Generic Access profile specifies using the value BT_INQ_TIME_GAP100.
 *
 *		listLength [in] - Length of devices list. There must be at least 1 device in the list.
 *	
 *		devicesList [in] - The list of devices to filter in.
 *		
 *		performNameRequest [in] - Performs name request on each discovered device 
 *			(which was not filtered out).
 *
 *		sdpServicesMask [in] - Performs service discovery on the specified set of SDP 
 *			services on each discovered device (which was not filtered out).
 *			Set SDP_SERVICE_NONE when no service discovery is needed.
 *
 * Generated events:
 *		BTEVENT_INQUIRY_RESULT
 *		BTEVENT_INQUIRY_COMPLETE
 *		BTEVENT_NAME_RESULT
 *		BTEVENT_DISCOVER_SERVICES_RESULT
 *		BTEVENT_SEARCH_COMPLETE
 *
 * Returns:
 *		BT_STATUS_PENDING - The Search process is started results
 *         will be sent via the BMG handler. A BTEVENT_HCI_FATAL_ERROR event
 *         indicates a fatal radio or HCI transport error and that all
 *         pending operations have failed.
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 *
 *		BT_STATUS_NO_RESOURCES - Not enough resources for execution
 *
 *     	BT_STATUS_IN_PROGRESS - An Search process is already in 
 *         progress. Only one Search can be in progress at a time.  
 *         Keep track of the BMG events to get the results from 
 *         the current Search or to see when it ends. If it has just
 *         ended then a cancel is also in progress so wait for
 *         the cancel to complete to start another search.
 *
 *     	BT_STATUS_FAILED - The operation failed. 
 *
 *     	BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the BMG events to
 *         be notified when the error has been corrected.
 */
BtStatus BTL_BMG_SearchByDevices(BtlBmgContext *bmgContext,
								const BtlBmgSearchMethod method,
								const BtIac lap, 
								const U8 length, 
								const U8 listLength, 
								const BD_ADDR *devicesList,
								const BOOL performNameRequest,
								const SdpServicesMask sdpServicesMask);


/*-------------------------------------------------------------------------------
 * BTL_BMG_CancelSearch()
 *
 * Brief: 
 *	Cancels the running search.
 *
 * Description:
 *		Request cancellation of a pending BTL_BMG_SearchByCod() or 
 *		BTL_BMG_SearchByDevices() request.
 *
 * Type:
 *		Asynchronous
 *
 * Generated events:
 *		BTEVENT_INQUIRY_CANCELED
 *		BTEVENT_DISCOVER_SERVICES_CANCELLED
 *		BTEVENT_SEARCH_CANCELLED
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 * Returns:
 *		BT_STATUS_PENDING - The cancel operation was started
 *         successfully. The results will be sent to the BMG handler.
 *		   A BTEVENT_HCI_FATAL_ERROR event indicates a fatal radio or 
 *		   HCI transport error and that all pending operations have failed.
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 *
 *     	BT_STATUS_SUCCESS - The search process was canceled
 *         immediately. It actually never was started.
 *
 *     	BT_STATUS_IN_PROGRESS - A cancel search is already in
 *         progress. Only one cancel can be in progress at a time.
 *         Keep track of the BMG events to see when the cancel
 *         is complete. 
 * 
 *     	BT_STATUS_FAILED - The operation failed because an search
 *         operation was not in progress.
 */
BtStatus BTL_BMG_CancelSearch(BtlBmgContext *bmgContext);


/*-------------------------------------------------------------------------------
 * BTL_BMG_GetRemoteDeviceName()
 *
 * Brief: 
 *	   	Get the name of the remote device. 
 *
 * Description:
 *			If an ACL connection does not exist then a temporary one will be created to
 *     	get the name. If this function returns BT_STATUS_PENDING
 *     	then the result will be returned via the BMG callback with the 
 *			BTEVENT_NAME_RESULT event. The "errCode" field 
 *     	indicates the status of the operation. The output field of
 *     	token "p.meToken" contains the results if the status indicates success.
 *
 *     	If a request to the same remote device is already in progress
 *     	only one request will be made. This token will receive the
 *     	results of the request in progress.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 *		bdAddr [in] - pointer to 48-bit address of the device.
 *
 * Generated events:
 *		BTEVENT_NAME_RESULT
 *
 * Returns:
 *		BT_STATUS_PENDING - Operation is started and results will
 *         be sent to the BMG callback.
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 *
 *		BT_STATUS_NO_RESOURCES - Not enough resources for execution
 *
 *     	BT_STATUS_FAILED - operation failed.
 *
 *		BT_STATUS_IN_PROGRESS - Operation to the same BD address from the same 
 *			context already in progress. 
 *
 *     	BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the BMG events to
 *         be notified when the error has been corrected.
 */
BtStatus BTL_BMG_GetRemoteDeviceName(BtlBmgContext *bmgContext, 
										const BD_ADDR *bdAddr);


/*-------------------------------------------------------------------------------
 * BTL_BMG_CancelGetRemoteDeviceName()
 *
 * Brief: 
 *	  Request cancellation of a pending BTL_BMG_GetRemoteDeviceName request.
 *
 * Description:
 *    If this function returns BT_STATUS_SUCCESS, then no further events will 
 *		be returned. If this function returns BT_STATUS_PENDING, then the 
 *		result will be returned	via the BMG callback in "p.meToken" with the 
 *		BTEVENT_NAME_RESULT event.
 *    The "errCode" field will indicate status BEC_REQUEST_CANCELLED
 *    if canceled or BEC_NO_ERROR if the name was returned before the
 *    cancel completed.
 *
 * Type:
 *		Asynchronous or Synchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 * Generated events:
 *		BTEVENT_NAME_RESULT
 *
 *		bdAddr [in] - pointer to 48-bit address of the device, 
 *			which was previously used in BTL_BMG_GetRemoteDeviceName().
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - the operation has completed successfully.
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 *
 *		BT_STATUS_NO_RESOURCES - Not enough resources for execution
 *
 *     	BT_STATUS_PENDING - Operation is started and results will
 *         be sent to the BMG callback.
 *
 *     	BT_STATUS_FAILED - operation failed because the get remote
 *         device name request is no longer in progress.
 */
BtStatus BTL_BMG_CancelGetRemoteDeviceName(BtlBmgContext *bmgContext, 
											const BD_ADDR *bdAddr);


/*-------------------------------------------------------------------------------
 * BTL_BMG_SetLocalDeviceName()
 *
 * Brief:
 *		Sets the local device name. 
 *
 * Description:
 *		This is the name that other devices will get when performing a GetRemoteDeviceName 
 *		to this device. The name is a UTF-8 encoded string. The name can be up to 
 *		BTL_BMG_MAX_LOCAL_NAME_LEN bytes (248) in length.
 *
 *    If the name is less than BTL_BMG_MAX_LOCAL_NAME_LEN bytes it must be 
 *		null-terminated.
 *
 *		Default name is zero length ("").
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 *		name [in] - pointer to a UTF-8 encoded string. 
 *
 *    len [in] - indicates the number of bytes in the name including the 
 *			null termination byte if the name is less than 248 bytes.
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - Operation successful.
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 *
 *		BT_STATUS_NO_RESOURCES - Not enough resources for execution
 *
 *		BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *        		 an initialization error. Monitor the BMG events to
 *         		be notified when the error has been corrected
 *
 *		BT_STATUS_INTERNAL_ERROR - An internal, unrecoverable error has occuured. The caller
 *			should restart BTL package before using the BTL again
 */
BtStatus BTL_BMG_SetLocalDeviceName(BtlBmgContext *bmgContext, 
									const U8 *name, 
									const U8 len);


/*-------------------------------------------------------------------------------
 * BTL_BMG_GetLocalDeviceName()
 *
 * Brief: 
 *		Get the local device name.
 *
 * Description: 
 *		Get the local device name.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 *		name [out] - pointer to which the function copies the returned name.
 *					name must have a length of at least BTL_BMG_MAX_LOCAL_NAME_LEN
 *					bytes
 *
 *    len [out] - indicates the number of bytes in the name including the 
 *			null termination byte if the name is less than 248 bytes.
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - The operation is successful.
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 *
 *		BT_STATUS_FAILED - Operation failed.
 */
BtStatus BTL_BMG_GetLocalDeviceName(BtlBmgContext *bmgContext, 
											U8 *name, 
											U8 *len);


/*-------------------------------------------------------------------------------
 * BTL_BMG_ReadLocalBdAddr()
 *
 * Brief: 
 *	Get the 48-bit Bluetooth device address of the local device. 
 *
 * Description:
 *		This request cannot be issued until the radio has been initialized. 
 *		If it's not initialized
 *    wait for the BTEVENT_HCI_INITIALIZED event on a BMG handler.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 *		Addr [out] - Pointer to a BD_ADDR structure to receive the address.
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - Operation was successful.
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 *
 *    BT_STATUS_FAILED - Operation failed because the radio is not
 *         initialized yet. Monitor the BMG events to be notified
 *         when the radio is initialized (BTEVENT_HCI_INITIALIZED).
 *
 *    BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the BMG events to
 *         be notified when the error has been corrected.
 */
BtStatus BTL_BMG_ReadLocalBdAddr(BtlBmgContext *bmgContext, BD_ADDR *Addr);


/*-------------------------------------------------------------------------------
 * BTL_BMG_SetClassOfDevice()
 *
 * Brief: 
 *		Set the class of device. 
 *
 * Description:
 *		The class of device is sent out as part of
 *    the response to inquiry. The actual bit pattern that is sent out is
 *    a composite of the value set using this function and the service
 *    class information found in registered SDP records.
 *		the classOfDevice value will be ORed to the device current classOfDevice.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 *		classOfDevice [in] - A 32-bit integer where the lower 3 bytes 
 *			represents the class of device. The most significant 8 bits are ignored.
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 */
BtStatus BTL_BMG_SetClassOfDevice(BtlBmgContext *bmgContext,
									const BtClassOfDevice classOfDevice);


/*-------------------------------------------------------------------------------
 * BTL_BMG_GetClassOfDevice()
 *
 * Brief: 
 *		Get the class of device (that was set both by user and by the profiles).
 *
 * Description: 
 *		Get the class of device (that was set both by user and by the profiles).
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *		
 *		classOfDevice [out] - A 32-bit integer where the lower 3 bytes 
 *			represents the class of device.
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 */
BtStatus BTL_BMG_GetClassOfDevice(BtlBmgContext *bmgContext, BtClassOfDevice *classOfDevice);

/*-------------------------------------------------------------------------------
 * BTL_BMG_GetCurrentRole()
 *
 * Brief: 
 *		Get the current role played by the local device. 
 *
 * Description: 
 *		Get the current role played by the local device. 
 *
 * Description:
 *    The value is meaningless if a connection does not exist to
 *    the remote device. When the role is currently being discovered,
 *    the role BCR_UNKNOWN will be returned. When the role discovery
 *    completes, the BTEVENT_ROLE_CHANGE event will be indicated.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 *		bdAddr [in] - pointer to 48-bit address of the device.
 *
 *		role [out] - The current role if a link exists otherwise the 
 *			value is meaningless.
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 *
 *		BT_STATUS_DEVICE_NOT_FOUND - device not found.
 */
BtStatus BTL_BMG_GetCurrentRole(BtlBmgContext *bmgContext, 
								const BD_ADDR *bdAddr, 
								BtConnectionRole *role);


/*-------------------------------------------------------------------------------
 * BTL_BMG_SwitchRole()
 *
 * Brief: 
 *		Switch the current role the device is performing for the ACL link
 *    specified by bdAddr. 
 *
 * Description:
 *		If the current role is slave then switch to
 *    master. If the current role is master then switch to slave. The
 *    current role can be found via BTL_BMG_GetCurrentRole. The result of the 
 *    operation will be returned via the BTEVENT_ROLE_CHANGE event.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 *		bdAddr [in] - pointer to 48-bit address of the device.
 *		
 * Generated events:
 *		BTEVENT_ROLE_CHANGE
 *
 * Returns:
 *		BT_STATUS_PENDING - the operation was started successfully.
 *       BMG handler will receive the BTEVENT_ROLE_CHANGE event. 
 *		   The "errCode" field of the BtEvent will indicate the success or 
 *		   failure of the role change event. 
 *		   The "p.roleChange" field indicates for which remote 
 *       Device the change has occurred along with the new role.  It is 
 *       possible that link is disconnected before the role change has 
 *       occurred. In that case the BMG handler will not receive 
 *       BTEVENT_ROLE_CHANGE but instead will receive
 *       BTEVENT_LINK_DISCONNECT.
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 *
 *    BT_STATUS_IN_PROGRESS - the operation failed because a mode
 *         change or disconnect operation is already in progress.
 *
 *    BT_STATUS_FAILED - the operation failed. 
 *
 *		BT_STATUS_DEVICE_NOT_FOUND - device not found.
 *
 *    BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the BMG events to
 *         be notified when the error has been corrected.
 */
BtStatus BTL_BMG_SwitchRole(BtlBmgContext *bmgContext, 
							const BD_ADDR *bdAddr);


/*-------------------------------------------------------------------------------
 * BTL_BMG_SetAccessibleMode()
 *
 * Brief: 
 *		Set the accessibility mode to be used when the device is not 
 *    connected and when the device is connected to one or more devices. 
 *
 * Description:
 *		If the modeNC is set to a value other than BAM_NOT_ACCESSIBLE and 
 *		there are no connections then the Bluetooth radio will enter inquiry 
 *		and/or page scan mode using the information passed in infoNC. 
 *		If infoNC is 0 or the values in infoNC are set to defaults 
 *		(BT_DEFAULT_SCAN_INTERVAL and BT_DEFAULT_SCAN_WINDOW) the radio 
 *		module default values are used. It is assumed that the macro defaults
 *    match the radio defaults (see BT_DEFAULT_PAGE_SCAN_WINDOW
 *    documentation.) So, the first call to BTL_BMG_SetAccessibleMode
 *    with infoNC set to 0 will not change the settings as the radio has
 *    already been initialized to its default settings. If there is
 *    a connection or a connection is in the process  of being created
 *    then modeNC and infoNC are saved and applied when all connections are 
 *    disconnected.
 *
 *		If the modeC is set to a value other than BAM_NOT_ACCESSIBLE and 
 *		there is a connection to one or more devices then the Bluetooth radio 
 *    will enter inquiry and/or page scan mode using the 
 *    information passed in infoC. If infoC is 0 or the values in 
 *    infoC are set to defaults (BT_DEFAULT_SCAN_INTERVAL and 
 *    BT_DEFAULT_SCAN_WINDOW) the radio module default values 
 *    are used. It is assumed that the macro defaults
 *    match the radio defaults (see BT_DEFAULT_PAGE_SCAN_WINDOW
 *    documentation.) So, the first call to BTL_BMG_SetAccessibleMode
 *    with infoC set to 0 will not change the settings as the radio has
 *    already been initialized to its default settings. If there are
 *    no active connections then modeC and infoC are saved and applied
 *    when the first connection comes up.
 *
 *    To keep other devices from finding and connecting to this 
 *    device, set the modeNC/modeC to BAM_NOT_ACCESSIBLE.
 *		When the device is not connected, the default mode when
 *    the stack is first loaded and initialized is controlled by
 *    BT_DEFAULT_ACCESS_MODE_NC.
 *		When the device is connected, the default mode when the stack is first 
 *		loaded and initialized is controlled by BT_DEFAULT_ACCESS_MODE_C.
 *
 *    In setting the values for infoNC/infoC. Both "inqWindow" and
 *    "inqInterval" must be set to defaults or to legitimate
 *    values. The range for values is 0x0012 to 0x1000. The time
 *    is calculated by taking the value * 0.625ms. It is an error
 *    if one field (interval/window) is a default and the other is 
 *    not. This also true for "pageInterval" and "pageWindow".
 *
 *    Any time the scan interval or window is different from 
 *    the current settings in the radio, the radio will be 
 *    instructed to change to the new settings. This means that 
 *    if there are different settings for the connected state
 *    versus the non-connected state, the radio module will be 
 *    instructed to change the settings when the first connection 
 *    comes up and when the last connection goes down
 *    (automatically). This also means that if different values
 *    for window and interval are set when going from any setting
 *    of accessible to non-accessible then the radio will be 
 *    instructed to change. In most cases it is best to use
 *    the radio defaults. In this way the radio is never told
 *    to change the scan interval or window.
 *
 * Requires:
 *     	BT_ALLOW_SCAN_WHILE_CON must be enabled for setting accessible mode 
 *			while at least one connection exists.
 *
 * Type:
 *		Asynchronous or Synchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 *		modeNC [in] - desired accessibility mode while no connection exists.
 *
 *     	infoNC [in] - pointer to structure containing the inquiry and page
 *         	scan interval and window to use while no connection exists. 
 *			If infoNC is 0 then the defaults set by the radio module are used.
 *
 *		modeC [in] - desired accessibility mode while at least one connection 
 *			exists.
 *
 *     	infoC [in] - pointer to structure containing the inquiry and page
 *         	scan interval and window to use while at least one connection exists. 
 *			If infoNC is 0 then the defaults set by the radio module are used.
 *		
 * Generated events:
 *		BTEVENT_ACCESSIBLE_CHANGE
 *
 * Returns:
 *		BT_STATUS_PENDING - the mode is being set. BMG callback will 
 *         receive BTEVENT_ACCESSIBLE_CHANGE event when the mode change 
 *         actually takes affect or an error has occurred. The "errCode"
 *         field of the BtEvent indicates the status of the operation. 
 *         If the operation is successful the "aMode" field of BtEvent
 *         indicates the new mode.  A BTEVENT_HCI_FATAL_ERROR event
 *         indicates a fatal radio or HCI transport error and that all
 *         pending operations have failed.
 *
 *     BT_STATUS_SUCCESS - Accessible mode is set. No event
 *         is sent out. This is returned if the values are only saved or 
 *		   info already matches the current setting.
 *
 *     BT_STATUS_IN_PROGRESS - operation failed because a change
 *         is already in progress. Monitor the BMG events to 
 *         determine when the change has taken place.
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the BMG events to
 *         be notified when the error has been corrected.
 */
BtStatus BTL_BMG_SetAccessibleMode(BtlBmgContext *bmgContext, 
									const BtAccessibleMode *modeNC,
									const BtAccessModeInfo *infoNC,
									const BtAccessibleMode *modeC, 
									const BtAccessModeInfo *infoC);


/*-------------------------------------------------------------------------------
 * BTL_BMG_GetAccessibleMode()
 *
 * Brief: 
 *		Returns the current accessibility mode
 *
 * Description:
 *		Returns the current accessibility mode that is used when the device is not 
 *    connected and when the device is connected to one or more devices. 
 *		This is not necessarily the current mode.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 *		modeNC [out] - pointer to memory to receive accessibility mode 
 *			when no connections exist. If modeNC is 0 then mode is not returned. 
 *			If the accessible modeNC has not yet been set, and therefore unknown, 
 *			0xff will be returned.
 *
 *     	infoNC [out] - pointer to structure to receive accessibility info 
 *			when no connections exist.
 *         	If infoNC is 0 then info is not returned.
 *
 *		modeC [out] - pointer to memory to receive accessibility mode 
 *			when connections exist. If modeC is 0 then mode is not returned. 
 *			If the accessible modeC has not yet been set, and therefore unknown, 
 *			0xff will be returned.
 *
 *     	infoC [out] - pointer to structure to receive accessibility info 
 *			when connections exist.
 *         	If infoC is 0 then info is not returned.
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - operation was successful
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 *
 *     	BT_STATUS_IN_PROGRESS - operation failed because a change
 *         is in progress. Monitor the BMG events to determine when
 *         the change is finished.
 *
 *     	BT_STATUS_FAILED - operation failed.
 */
BtStatus BTL_BMG_GetAccessibleMode(BtlBmgContext *bmgContext, 
									BtAccessibleMode *modeNC,
									BtAccessModeInfo *infoNC,
									BtAccessibleMode *modeC, 
									BtAccessModeInfo *infoC);


/*-------------------------------------------------------------------------------
 * BTL_BMG_SendHciCommand()
 *
 * Brief: 
 *		This function is used to send HCI commands not handled directly by the 
 *		BMG and radio specific user defined commands. 
 *
 * Description:
 *		The command will 
 *    block other HCI commands until it is complete. Use BTL_BMG_SendHciCommand
 *    for HCI operations that return with the "Command Complete" event
 *    or local operations that do no require communication with a remote device.
 *    The "Command Status" event with an error will also cause the operation 
 *    to complete. If the function returns BT_STATUS_PENDING then the result 
 *    will be returned via the BMG callback with the BTEVENT_COMMAND_COMPLETE event.
 *
 *    The "out" fields of the MeCommandToken will contain the result
 *    of the operation. The "errCode" field of the BtEvent is not valid
 *    since the "errCode" is part of the event and its location in "parms"
 *    varies.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 *		hciCommand [in] - HCI command must contain the desired command.
 *			The command can be one of the HCI commands defined 
 *			in hci.h that is not supported by the BMG or user defined commands.
 *
 *    parmLen [in] - Length of the command parameters.
 *
 *		parms [in] - Pointer to command parameters.
 *
 *    event [in] - Main event which signals end of operation.
 *			It must contain the event that is expected to signal completion 
 *			of the operation. The most common event is "Command Complete". 
 *			See hci.h for a list of events. The "Command Status" event is 
 *			always checked. If a "Command Status" event is received with 
 *			an error then the operation is considered complete.
 *         	If a "Command Status" is received without an error then the command
 *         	will finish when the event matches "event".
 *		
 * Generated events:
 *		BTEVENT_SEND_HCI_COMMAND_RESULT
 *
 * Returns:
 *		BT_STATUS_PENDING - Operation is started and results will
 *         	be sent to the BMG callback. A BTEVENT_HCI_FATAL_ERROR event
 *         	indicates a fatal radio or HCI transport error and all
 *         	pending operations have failed.
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 *
 *		BT_STATUS_NO_RESOURCES - Not enough resources for execution
 *
 *     	BT_STATUS_FAILED - operation failed (XA_ERROR_CHECK only).
 *
 *     	BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         	an initialization error. Monitor the BMG events to
 *         	be notified when the error has been corrected.
 */
BtStatus BTL_BMG_SendHciCommand(	BtlBmgContext 	*bmgContext, 
											const U16 		hciCommand,
											const U8 			parmsLen, 
											const U8 			*parms, 
											const U8 			event);


/*-------------------------------------------------------------------------------
 * BTL_BMG_SetDefaultLinkPolicy()
 *
 * Brief: 
 *	 	Set the default link policy used on outgoing and incoming ACL 
 *    connections. 
 *
 * Description:
 *		The link policy determines the behavior of the local
 *    link manager when it receives a request from a remote device or it
 *    determines itself to change the master-slave role or to enter hold,
 *    sniff, or park mode. The default if this function is not called 
 *    is to disable all modes.
 *
 *    Policy for incoming ACL connections is set independent of policy
 *    for outgoing connections. The default policy is applied immediately 
 *    after the link comes up. An higher level Management Entity should
 *    use this function, not applications. It should be called before any
 *    ACL connections exist and does not effect existing
 *    ACL connections.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 *		inACL [in] - default link policy applied to all incoming ACL connections.
 *         Incoming ACL connections are those initiated by a remote device.
 *
 *    outACL [in] - default link policy applied to all outgoing ACL connections.
 *         Outgoing ACL connections are those initiated by the local device.
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - operation was successful.
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 *
 *    BT_STATUS_FAILED - operation failed because the policy settings are
 *         not valid (error checking only).
 */
BtStatus BTL_BMG_SetDefaultLinkPolicy(BtlBmgContext *bmgContext, 
										const BtLinkPolicy inACL, 
										const BtLinkPolicy outACL);


/*-------------------------------------------------------------------------------
 * BTL_BMG_GetDefaultLinkPolicy()
 *
 * Brief: 
 *	 	Get the default link policy used on outgoing and incoming ACL 
 *   	connections. 
 *
 * Description:
 *		The link policy determines the behavior of the local
 *    link manager when it receives a request from a remote device or it
 *    determines itself to change the master-slave role or to enter hold,
 *    sniff, or park mode.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 *		inACL [out] - pointer to receive the default link policy applied to 
 *			all incoming ACL connections.
 *
 *     	outACL [out] - pointer to receive the default link policy applied to 
 *			all outgoing ACL connections.
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - operation was successful.
 *
 *    BT_STATUS_INVALID_PARM - Operation failed because one of the parameters is not valid
 */
BtStatus BTL_BMG_GetDefaultLinkPolicy(BtlBmgContext *bmgContext, 
										BtLinkPolicy *inACL, 
										BtLinkPolicy *outACL);


/*-------------------------------------------------------------------------------
 * BTL_BMG_GetRemDevState()
 *
 * Brief: 
 *		Returns the current state of the link for the given device.
 *
 * Description: 
 *		Returns the current state of the link for the given device.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 *		bdAddr [in] - pointer to 48-bit address of the device.
 *
 *		state [out] - pointer to receive the current state of the link.
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - operation was successful.
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 *
 *    BT_STATUS_FAILED - operation failed.
 *
 *		BT_STATUS_DEVICE_NOT_FOUND - device not found.
 */
BtStatus BTL_BMG_GetRemDevState(BtlBmgContext *bmgContext, 
								const BD_ADDR *bdAddr, 
								BtRemDevState *state);


/*-------------------------------------------------------------------------------
 * BTL_BMG_GetRemDevCod()
 *
 * Brief: 
 *		Returns the class of device of the given device.
 *
 * Description: 
 *		Returns the class of device of the given device.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 *		bdAddr [in] - pointer to 48-bit address of the device.
 *
 *		cod [out] - pointer to receive the class of device of the given device.
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - operation was successful.
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 *
 *    BT_STATUS_FAILED - operation failed.
 *
 *		BT_STATUS_DEVICE_NOT_FOUND - device not found.
 */
BtStatus BTL_BMG_GetRemDevCod(BtlBmgContext *bmgContext, 
								const BD_ADDR *bdAddr, 
								BtClassOfDevice *cod);


/*-------------------------------------------------------------------------------
 * BTL_BMG_GetBtVersion()
 *
 * Brief: 
 *		Returns the minimal supported Bluetooth version between the local radio and the stack.
 *
 * Description: 
 *		Returns the minimal supported Bluetooth version between the local radio and the stack.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 *		version [out] - pointer to receive the BT version.
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - operation was successful.
 *
 *    BT_STATUS_INVALID_PARM - invalid parameter.
 */
BtStatus BTL_BMG_GetBtVersion(BtlBmgContext *bmgContext, BtlBmgBtVersion *version);


/*-------------------------------------------------------------------------------
 * BTL_BMG_GetBtFeatures()
 *
 * Brief: 
 *		Returns BT features bit mask.
 *
 * Description:
 *		Gets the value of the specified byte of the Bluetooth features
 *		bitmask from the local radio. See the Bluetooth specification for
 *		a description of the features bitmask.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 *		byteIdx [in] - index of byte to retrieve. Must be between 0 and 7.
 *		
 *		byteVal [out] - value of requested byte.
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - operation was successful.
 *
 *    BT_STATUS_INVALID_PARM - invalid parameter.
 */
BtStatus BTL_BMG_GetBtFeatures(BtlBmgContext *bmgContext, const U8 byteIdx, U8 *byteVal);


/*-------------------------------------------------------------------------------
 * BTL_BMG_EnableTestMode()
 *
 * Brief: 
 *	  Enable test mode.
 *
 * Decription: 
 *	  Enable test mode.
 *
 * Type:
 *		Asynchronous or Synchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *	
 * Generated events:
 *		BTEVENT_ENABLE_TEST_MODE_RESULT
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Test mode already enabled.
 *
 *		BT_STATUS_PENDING - operation started successfully.
 *			The application will be notified when operation is complete with
 *			the event BTEVENT_ENABLE_TEST_MODE.
 *
 *    BT_STATUS_FAILED - operation failed.
 *
 *		BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error.
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 *
 *		BT_STATUS_NO_RESOURCES - Not enough resources for execution
 *
 */
BtStatus BTL_BMG_EnableTestMode(BtlBmgContext *bmgContext);


/*-------------------------------------------------------------------------------
 * BTL_BMG_AuthorizeDeviceReply()
 *
 * Brief: 
 *		Authorizes remote device as a result to BTEVENT_AUTHORIZATION_REQ previously sent.
 *
 * Description:
 *		remote device it authorized according to authorizationType
 *    If authorizationType is set to BTL_BMG_AUTHORIZED_TRUSTED The 
 *    trust holds beyond this connection. In this case the device is 
 *		automatically inserted to the persistent DB.
 *    
 *		The BMG handler calls this function after 
 *    receiving a BTEVENT_AUTHORIZATION_REQ when "errCode" is
 *    BEC_NO_ERROR.
 *		The service associated with	a BTEVENT_AUTHORIZATION_REQ event
 *		is also contained within the event. BMG handler might use this to 
 *		determine what service or channel is being authorized.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 *		bdAddr [in] - pointer to 48-bit address of the device.
 *
 *		authorizationType [in] - 
 *					BTL_BMG_NOT_AUTHORIZED  - Do not authorize the device
 *					BTL_BMG_AUTHORIZED_NOT_TRUSTED - Authorize device for the remaining of this connection but it is not trusted
 *					BTL_BMG_AUTHORIZED_TRUSTED - Authorize device this time and it is also be trusted in future
 * Returns:
 *		BT_STATUS_SUCCESS - operation successful
 *
 *     	BT_STATUS_FAILED - operation failed. The remote device is
 *         not in a state to accept authorization.
 *
 *     	BT_STATUS_DEVICE_NOT_FOUND - the authorization was successful
 *         but the device data base write failed because the device was
 *         not found in the database or the write operation to the 
 *         database failed.
 *
 *     	BT_STATUS_INVALID_PARM - invalid parameter.
 *
 *     	BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the BMG events to
 *         be notified when the error has been corrected.
 */
BtStatus BTL_BMG_AuthorizeDeviceReply(BtlBmgContext *bmgContext, 
								const BD_ADDR *bdAddr, 
								BtlBmgAuthorizationType authorizationType);

/*-------------------------------------------------------------------------------
 * BTL_BMG_AuthorizeServiceReply()
 *
 * Brief: 
 *	  BTL_BMG_AuthorizeServiceReply is similar to BTL_BMG_AuthorizeDeviceReply, 
 *		but authorizes a device FOR A SPECIFIC SERVICE OR CHANNEL.
 *
 * Description:
 *		Remote device it authorized for the specific service 
 *		according to authorizationType.
 *    If authorizationType is set to BTL_BMG_AUTHORIZED_TRUSTED The 
 *    trust holds beyond this connection. In this case the device is 
 *		automatically inserted to the persistent DB.
 *    
 *		The BMG handler calls this function after 
 *    receiving a BTEVENT_AUTHORIZATION_REQ when "errCode" is
 *    BEC_NO_ERROR.
 *		The service associated with	a BTEVENT_AUTHORIZATION_REQ event
 *		is also contained within the event. BMG handler might use this to 
 *		determine what service or channel is being authorized.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 *		bdAddr [in] - pointer to 48-bit address of the device.
 *
 *     	isAuthorized [in] - TRUE indications authorization is granted.
 *         FALSE indicates authorization is denied.
 *		authorizationType [in] - 
 *				BTL_BMG_NOT_AUTHORIZED  - Do not authorize the service
 *				BTL_BMG_AUTHORIZED_NOT_TRUSTED - Authorize service one time (next service that require 
 * 																			authorization will ask the application again for authorization ) 
 *																			and it is not trusted
 *				BTL_BMG_AUTHORIZED_TRUSTED - Authorize service this time and it is trusted for this device. 
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - operation successful
 *
 *     	BT_STATUS_FAILED - operation failed. The remote device is
 *         not in a state to accept authorization.
 *
 *     	BT_STATUS_DEVICE_NOT_FOUND - the authorization was successful
 *         but the device data base write failed because the device was
 *         not found in the database or the write operation to the 
 *         database failed.
 *
 *     	BT_STATUS_INVALID_PARM - operation failed because the remote
 *         device pointer is 0 (XA_ERROR_CHECK only).
 *
 *     	BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus BTL_BMG_AuthorizeServiceReply(BtlBmgContext *bmgContext, 
										const BD_ADDR *bdAddr, 
										BtlBmgAuthorizationType authorizationType);

/*-------------------------------------------------------------------------------
 * BTL_BMG_PinReply()
 *
 * Brief: 
 *		Set the pin value for the device specified by rm. 
 *
 * Description:
 *    This function is called as a response to the BTEVENT_PIN_REQ
 *    event when "errCode" is BEC_NO_ERROR. Setting pin to 0
 *    rejects the pairing operation.
 *		Note that BPT_SAVE_NOT_TRUSTED, BPT_SAVE_TRUSTED and BPT_NOT_SAVE_TRUSTED
 *		pairing types automatically add device record to the non volatile memory.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 *		bdAddr [in] - pointer to 48-bit address of the device.
 *
 *		pin [in] - pointer to pin. Pin set to 0 indicates that
 *         the pairing operation should be rejected. 
 *
 *    len [in] - number of bytes in the pin.
 *
 *    type [in] - indicates how the link key and future trust should be handled.
 *		
 * Generated events:
 *		BTEVENT_ENABLE_TEST_MODE_RESULT
 *
 * Returns:
 *		BT_STATUS_PENDING - operation is started successful. The
 *         BMG handler will be called with the BTEVENT_PAIRING_COMPLETE
 *         event when pairing is complete.
 *
 *     	BT_STATUS_FAILED - operation failed because the remote
 *         device structure is not in a state where a pin is needed.
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 *
 *     	BT_STATUS_NO_RESOURCES - operation failed because there are
 *         too many security operations on the queue. Try again
 *         in a little while.
 *
 *		BT_STATUS_DEVICE_NOT_FOUND - device not found.
 *
 *     	BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the BMG events to
 *         be notified when the error has been corrected.
 */
BtStatus BTL_BMG_PinReply(BtlBmgContext *bmgContext, 
							const BD_ADDR *bdAddr, 
							const U8 *pin, 
							const U8 len, 
							const BtPairingType type);


/*-------------------------------------------------------------------------------
 * BTL_BMG_Bond()
 *
 * Brief: 
 *	  Initiates bonding procedure with a remote device.
 *
 * Description:
 *		If a device is not bonded (use BTL_BMG_GetDeviceBondingState) then bonding will be attempted.
 *		A connection is created, and then closed after the the bonding is complete.
 *		bonding is not allowed with an already connected device.
 *		If pin code is not known when this function is called, set the pin to 0,
 *		which indicates that the pin code will be provided later once the event 
 *		BTEVENT_PIN_REQ occurs.
 *		BTEVENT_AUTHENTICATE_CNF will be sent to the BMG handler.
 *    The "errCode" field of the BtEvent specifies if the operation 
 *    was successful or not.
 *		Bond for bonded device or connected device is not allowed.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 *		bdAddr [in] - pointer to 48-bit address of the device to bond with.
 *
 *		pin [in] - pointer to pin. Pin set to 0 indicates that the pin code will 
 *			be provided later once the event BTEVENT_PIN_REQ occurs.
 *
 *     	len [in] - number of bytes in the pin (relevant only if pin is not 0).
 *
 *     	type [in] - indicates how the link key should be handled 
 *			(relevant only if pin is not 0).
 *			Possible values:
 *			BPT_NOT_SAVED - Do not save the link key (one time authentication).
 *			BPT_SAVE_NOT_TRUSTED - Save the link key but make not trusted.
 *			BPT_SAVE_TRUSTED - Save the link key and make trusted.
 *		
 * Generated events:
 *		BTEVENT_PIN_REQ
 *		BTEVENT_AUTHENTICATE_CNF
 *		BTEVENT_BOND_RESULT
 *
 * Returns:
 *		BT_STATUS_PENDING - operation has started successfully
 *         result will be sent to BMG handler.
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 *
 *		BT_STATUS_NO_RESOURCES - Not enough resources for execution
 *
 *     	BT_STATUS_NO_RESOURCES - the operation failed because 
 *         the maximum number of connections already exist.
 *
 *     	BT_STATUS_FAILED - operation failed because the device is bonded or connected.
 *
 *     	BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the BMG events to
 *         be notified when the error has been corrected.
 */
BtStatus BTL_BMG_Bond(BtlBmgContext *bmgContext,
						const BD_ADDR *bdAddr,
						const U8 *pin, 
						const U8 len, 
						const BtPairingType type);

/*-------------------------------------------------------------------------------
 * BTL_BMG_GetDeviceBondingState()
 *
 * Brief: 
 *	Returns TRUE if the device with the given BD addr is bonded.
 *
 * Description: 
 *	Returns TRUE if the device with the given BD addr is bonded.
 *
 *	Type:
 *		Synchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 *		bdAddr [in] - pointer to 48-bit address of device.
 *
 *		answer[out] - TRUE if the device with the given BD addr is bonded
 *
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - operation is successful.
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 */
BtStatus BTL_BMG_GetDeviceBondingState(BtlBmgContext *bmgContext, BD_ADDR *bdAddr, BOOL *answer);


/*-------------------------------------------------------------------------------
 * BTL_BMG_UnbondDevice()
 *
 * Brief: 
 * 		Unbonds a device with the given BD addr.
 *
 * Description:
 *		Deletes device security info from the DB
 *		Note - this command will not remove the device from DB. It only erases security info.
 *		To remove the device also from DB use: BTL_BMG_DeleteDeviceRecord
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 *		bdAddr [in] - pointer to 48-bit address of device unbond.
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - operation is successful.
 *
 *    BT_STATUS_FAILED - operation failed.
 *
 *		BT_STATUS_DEVICE_NOT_FOUND - device not found.
 *
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 */
BtStatus BTL_BMG_UnbondDevice(BtlBmgContext *bmgContext, 
											const BD_ADDR *bdAddr);

/*-------------------------------------------------------------------------------
 * BTL_BMG_UnbondAll()
 *
 * Brief: 
 * 		Unbonds all devices in the data base.
 *
 * Description:
 *		Deletes devices security info from the DB
 *		Note - this command will not remove the devices from DB. It only erases security info.
 *		To remove the devices also from DB use: BTL_BMG_CleanDeviceDB.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - operation is successful.
 *
 *    BT_STATUS_FAILED - operation failed.
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 */
BtStatus BTL_BMG_UnbondAll(BtlBmgContext *bmgContext);


/*-------------------------------------------------------------------------------
 * BTL_BMG_CancelBond()
 *
 * Brief: 
 *		Cancel pending bond procedure.
 *
 * Description:
 *    If the function returns BT_STATUS_PENDING then 
 *    BTEVENT_BOND_CANCELLED will be sent to the BMG handler. The "errCode"
 *    field will indicate the result.
 *
 * Type:
 *		Asynchronous or Synchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 *		bdAddr [in] - pointer to 48-bit address of the device to cancel bond with.
 *	
 * Generated events:
 *		BTEVENT_BOND_CANCELLED
 *		
 * Returns:
 *		BT_STATUS_PENDING - operation has started successfully,
 *         result will be sent to BMG handler.
 *
 *     	BT_STATUS_SUCCESS - the operation is successful.
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 *
 *     	BT_STATUS_FAILED - the operation failed because 
 *         no bonding procedure is ongoing.
 *
 *     	BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the BMG events to
 *         be notified when the error has been corrected.
 */
BtStatus BTL_BMG_CancelBond(BtlBmgContext *bmgContext, 
							const BD_ADDR *bdAddr);


/*-------------------------------------------------------------------------------
 * BTL_BMG_SetLinkEncryption()
 *
 * Brief: 
 *		Enable or disable link level encryption for the give link.
 *
 * Description:
 *    If the function returns BT_STATUS_PENDING then 
 *    BTEVENT_ENCRYPT_COMPLETE will be sent to the BMG handler. The "errCode"
 *    field will indicate the result.
 *    
 *    The link must be active (not in hold, sniff, park or in the
 *    process of being disconnected). Also it must not already
 *    be in the process of performing an encryption operation.
 *
 * Type:
 *		Asynchronous or Synchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 *		bdAddr [in] - pointer to 48-bit address of device record to delete.
 *
 *		encryptMode [in] - desired encryption mode (enabled or disabled).
 *
 * Generated events:
 *		BTEVENT_ENCRYPT_COMPLETE
 *
 * Returns:
 *		BT_STATUS_PENDING - the operation was started successfully.
 *         When the operation is complete the BMG handler will be
 *         called with the BTEVENT_ENCRYPT_COMPLETE event.
 *         A BTEVENT_HCI_FATAL_ERROR event indicates a fatal radio
 *         or HCI transport error and all pending operations have failed.
 *
 *     	BT_STATUS_SUCCESS - the link is already in the desired mode.
 *         No events will be generated.
 *
 *     	BT_STATUS_FAILED - the operation failed because the link
 *         is not in a state to perform encryption. The link must
 *         be authenticated before encryption is allowed.
 *
 *     	BT_STATUS_INVALID_PARM - invalid parameter(XA_ERROR_CHECK only).
 *
 *     	BT_STATUS_NO_RESOURCES - operation failed because there are
 *         too many security operations on the queue. Try again
 *         in a little while.
 *
 *		BT_STATUS_DEVICE_NOT_FOUND - device not found.
 *
 *     	BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the BMG events to
 *         be notified when the error has been corrected.
 */
BtStatus BTL_BMG_SetLinkEncryption(BtlBmgContext *bmgContext, 
									const BD_ADDR *bdAddr, 
									const BtEncryptMode encryptMode);


/*-------------------------------------------------------------------------------
 * BTL_BMG_GetLinkEncryption()
 *
 * Brief: 
 *		Get the encryption state of the remote device.
 *
 * Description: 
 *		Get the encryption state of the remote device.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 *		bdAddr [in] - pointer to 48-bit address of device record to delete.
 *
 *		encryptMode [out] - current encryption mode (enabled or disabled).
 *		
 * Returns:
 *     	BT_STATUS_SUCCESS - the operation is successful.
 *
 *     	BT_STATUS_FAILED - the operation failed.
 *
 *			BT_STATUS_DEVICE_NOT_FOUND - device not found.
 *
 *     	BT_STATUS_INVALID_PARM - invalid parameter.
 */
BtStatus BTL_BMG_GetLinkEncryption(BtlBmgContext *bmgContext, 
									const BD_ADDR *bdAddr, 
									BtEncryptMode *encryptMode);


/*-------------------------------------------------------------------------------
 * BTL_BMG_SetSecurityMode()
 *
 * Brief: 
 *	  Set the security mode, existing connections are not affected.
 *
 * Description:
 *		Possible security modes are:
 *
 *		BSM_SEC_DISABLED - All (incoming and outgoing) security procedures will fail.
 *
 *		BSM_SEC_LEVEL_1  - Mode 1 is a non-secure mode in which the Bluetooth 
 *			device initiates no security procedures, although it may respond to them.
 *
 *		BSM_SEC_LEVEL_2  - Mode 2 allows security to be enforced at the service level. 
 *			That is, each service may determine what level of security is required to 
 *			access that service.
 *
 *		BSM_SEC_LEVEL_3  - Mode 3 allows security to be enforced at the link level.
 *			Authentication will be enforced when any Bluetooth link (incoming and outgoing) 
 *			is opened. The security Mode 2 security records are still checked and 
 *			authorization may still be required.
 *
 *		BSM_SEC_ENCRYPT  - Same as BSM_SEC_LEVEL_3, with encryption.
 *
 *		If the operation is successful then the BMG handler will receive 
 *		BTEVENT_SECURITY_CHANGE event.
 *		
 *
 * Type:
 *		Asynchronous or Synchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 *		securityMode [in] - desired security mode.
 *		
 * Generated events:
 *		BTEVENT_SECURITY_CHANGE
 *
 * Returns:
 *		BT_STATUS_PENDING - operation has started successfully.
 *         When the operation is complete the BMG handler will be
 *         called with the BTEVENT_SECURITY_CHANGE event.
 *         A BTEVENT_HCI_FATAL_ERROR event indicates a fatal radio
 *         or HCI transport error and all pending operations have failed.
 *
 *     	BT_STATUS_SUCCESS - the operation is successful, since the given
 *         security mode is already set. No events will be generated.
 *
 *     	BT_STATUS_NO_RESOURCES - operation failed because there are
 *         too many security operations on the queue. Try again
 *         in a little while.
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 *
 *     	BT_STATUS_FAILED - operations failed because change of
 *         security mode is already in progress.
 *
 *     	BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the BMG events to
 *         be notified when the error has been corrected.
 */
BtStatus BTL_BMG_SetSecurityMode(BtlBmgContext *bmgContext, 
								const BtSecurityMode securityMode);


/*-------------------------------------------------------------------------------
 * BTL_BMG_GetSecurityMode()
 *
 * Brief: 
 *	  Get the current security mode.
 *
 * Description:
 *		Possible security modes are:
 *
 *		BSM_SEC_DISABLED - All (incoming and outgoing) security procedures will fail.
 *
 *		BSM_SEC_LEVEL_1  - Mode 1 is a non-secure mode in which the Bluetooth 
 *			device initiates no security procedures, although it may respond to them.
 *
 *		BSM_SEC_LEVEL_2  - Mode 2 allows security to be enforced at the service level. 
 *			That is, each service may determine what level of security is required to 
 *			access that service.
 *
 *		BSM_SEC_LEVEL_3  - Mode 3 allows security to be enforced at the link level.
 *			Authentication will be enforced when any Bluetooth link (incoming and outgoing) 
 *			is opened. The security Mode 2 security records are still checked and 
 *			authorization may still be required.
 *
 *		BSM_SEC_ENCRYPT  - Same as BSM_SEC_LEVEL_3, with encryption.
 *		
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 *		securityMode [out] - current security mode.
 *		
 * Returns:
 *     	BT_STATUS_SUCCESS - the operation is successful.
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 *
 *     	BT_STATUS_FAILED - operations failed.
 */
BtStatus BTL_BMG_GetSecurityMode(BtlBmgContext *bmgContext, 
								BtSecurityMode *securityMode);


/*-------------------------------------------------------------------------------
 * BTL_BMG_DeleteDeviceRecord()
 *
 * Brief: 
 *		Deletes a record with the given "bdAddr" from the device	database.
 *		
 * Description:
 *		Erases the device record from the non volatile memory.
 *		Note that this call will clean all device security information (bonding info) as well
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 *		bdAddr [in] - pointer to 48-bit address of device record to delete.
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - operation is successful.
 *
 *    BT_STATUS_FAILED - operation failed.
 *
 *		BT_STATUS_DEVICE_NOT_FOUND - device not found.
 *
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 */
BtStatus BTL_BMG_DeleteDeviceRecord(BtlBmgContext *bmgContext, 
											const BD_ADDR *bdAddr);


/*-------------------------------------------------------------------------------
 * BTL_BMG_CleanDeviceDB()
 *
 * Brief: 
 *		Deletes all records from the device database.
 * 
 * Description:
 *		Erases the device records from the non volatile memory.
 *		Note that this call will clean all security information as well
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - operation is successful.
 *
 *    BT_STATUS_FAILED - operation failed.
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 */
BtStatus BTL_BMG_CleanDeviceDB(BtlBmgContext *bmgContext);

/*-------------------------------------------------------------------------------
 * BTL_BMG_InitDeviceRecord()
 *
 * Brief: 
 *		Initializes records fields. 
 *
 * Description:
 *		When adding a new record, this function should be 
 *		called to reset record's values, then user can set records fields and add the 
 *		record. the purpose of this function is to reset fields that are not known to 
 *		the user to their default values.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 *		record [in] - pointer to record for init.
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - record was initialized.
 *
 *    BT_STATUS_INVALID_PARM - operation failed because record or bmgContext
 *         is invalid
 */
BtStatus BTL_BMG_InitDeviceRecord(BtlBmgContext *bmgContext, BtlBmgDeviceRecord* record);


/*-------------------------------------------------------------------------------
 * BTL_BMG_SetDeviceRecord()
 *
 * Brief: 
 *		Adds/Updates a record to the device database. 
 *
 * Description:
 *		A pointer to a record is passed to the function . The contents of the
 *    record is copied into the device database. If a record
 *    with the same BD_ADDR already exists then the existing
 *    record is changed. 
 *		This function can be used to preload the device database. 
 *		For changing only a set of fields use BTL_BMG_FindDeviceRecord, 
 *		change the relevant fields and set the changed record using BTL_BMG_SetDeviceRecord.
 *		If the device does not exist in the DB, use BTL_BMG_InitDeviceRecord before setting desired fields.
 *		Note that a record can be changed only using this function.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 *		record [in] - pointer to record which is to be copied into the database.
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - record written successfully.
 *
 *    BT_STATUS_FAILED - record was not written.
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 */
BtStatus BTL_BMG_SetDeviceRecord(BtlBmgContext *bmgContext, 
										const BtlBmgDeviceRecord *record);


/*-------------------------------------------------------------------------------
 * BTL_BMG_FindDeviceRecord()
 *
 * Brief: 
 *		Find the device record with the given bdAddr. 
 *
 * Description:
 *		The record is copied to the location pointed to by the record parameter.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 *		bdAddr [in] - pointer to 48-bit address of device record to find.
 *
 *    record [out] - pointer to location to receive record.
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - operation is successful.
 *
 *    BT_STATUS_FAILED - operation failed.
 *
 *		BT_STATUS_DEVICE_NOT_FOUND - device not found.
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 */
BtStatus BTL_BMG_FindDeviceRecord(BtlBmgContext *bmgContext, 
											const BD_ADDR *bdAddr, 
											BtlBmgDeviceRecord *record);


/*-------------------------------------------------------------------------------
 * BTL_BMG_GetNumberOfDeviceRecords()
 *
 * Brief: 
 *		Returns number of device records.
 *
 * Description: 
 *		Returns number of device records.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 *		size [out] - number of device security records.
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - operation is successful.
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 *
 *    BT_STATUS_FAILED - operation failed.
 */
BtStatus BTL_BMG_GetNumberOfDeviceRecords(BtlBmgContext *bmgContext, 
													U32 *size);


/*-------------------------------------------------------------------------------
 * BTL_BMG_EnumDeviceRecords()
 *
 * Brief: 
 *		Enumerate the device database. 
 * Description:
 *		The first record is at index 0. The intent of this function is to 
 *		enumerate all records in the device database in a loop. 
 *		If records are added or deleted in between calls
 *    to this function then all records may not be returned or duplicate
 *    records could be returned.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 *		idx [in] - index of desired record. The first record is at index 0.
 *
 *     	record [out] - pointer to location to receive record.
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - operation is successful. Record is returned.
 *
 *    BT_STATUS_FAILED - operation failed. No record is
 *         returned. This occurs if the index is greater than
 *         the number of records or an error occurred in reading
 *         the database.
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 */
BtStatus BTL_BMG_EnumDeviceRecords(BtlBmgContext *bmgContext, 
											const U32 idx, 
											BtlBmgDeviceRecord *record);


/*-------------------------------------------------------------------------------
 * BTL_BMG_ControlIncomingConnectionRequests()
 *
 * Brief: 
 *		Register for BTEVENT_LINK_CONNECT_REQ events.
 *
 * Description:
 *		when enable = TRUE the application will be responsible for accepting 
 *    connections by calling BTL_BMG_IncomingConnectionRequestReply upon receiving BTEVENT_LINK_CONNECT_REQ.
 *    when disabled, the stack will accept all incoming connections automatically. 
 *
 * Type:
 *		Synchronous
 *
 * Generated events:
 *		BTEVENT_LINK_CONNECT_REQ
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - operation is successful. 
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 *
 *		BT_STATUS_NO_RESOURCES - Not enough resources for execution
 */
BtStatus BTL_BMG_ControlIncomingConnectionRequests(BtlBmgContext *bmgContext, BOOL enable);


/*-------------------------------------------------------------------------------
 * BTL_BMG_GetControlIncomingConnectionRequests()
 *
 * Brief: 
 *		Returns TRUE if bmgContext user is registered for BTEVENT_LINK_CONNECT_REQ events.
 *
 * Description:
 *		Returns TRUE if bmgContext user is registered for BTEVENT_LINK_CONNECT_REQ events.
 *
 *	Type:
 *		Synchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 *		answer[out] - TRUE if bmgContext user is registered for BTEVENT_LINK_CONNECT_REQ events.
 *
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - operation is successful.
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 */
BtStatus BTL_BMG_GetControlIncomingConnectionRequests(BtlBmgContext *bmgContext, BOOL *answer);

/*-------------------------------------------------------------------------------
 * BTL_BMG_IncomingConnectionRequestReply()
 *
 * Brief: 
 *		Accepts/rejects the incoming connection. 
 *
 * Description:
 *		The BMG handler calls this function upon receiving a BTEVENT_LINK_CONNECT_REQ event.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *
 *    bdAddr [in] - pointer to 48-bit address of device.
 *
 *		accept[in] - TRUE - accept incoming connection, FALSE - reject incoming connection
 *
 *		rejectErrorCode - reason for the rejection in case of rejection in case accept = TRUE, this parameter is ignored.
 *    The only reasons allowed are as follows:
 *
 *     BEC_LIMITED_RESOURCE - Host rejected due to limited resources
 *
 *     BEC_SECURITY_ERROR   - Host rejected due to security reasons
 *
 *     BEC_PERSONAL_DEVICE  - Host rejected (remote is personal device)
 *
 *		this field is ignored when accept = FALSE.
 *		
 * Returns:
 *		BT_STATUS_PENDING - operation started successfully.
 *
 *		BT_STATUS_INVALID_PARM - operation failed because bmgContext or BD addr is 0.
 *
 *    BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the BMG events to
 *         be notified when the error has been corrected.
 *
 *    BT_STATUS_FAILED  - remote device is not in state for dealing with incoming connections.
 *
 */
BtStatus BTL_BMG_IncomingConnectionRequestReply(BtlBmgContext *bmgContext,
											 BD_ADDR* bdAddr,
											 BOOL accept,
											 BtErrorCode rejectErrorCode);

/*-------------------------------------------------------------------------------
 *	BTL_BMG_DiscoverServices()
 *
 * Brief: 
 *		Discover remote device supported services.
 *
 * Description:
 *		Discover which of the services specified by sdpServiceMask are supported 
 *		in the device specified by bdAddr.
 *		An event BTEVENT_DISCOVER_SERVICES_RESULT will be sent.
 *		A BtlBmgSdpServiceMask variable will specify which of the given services
 *		are supported.
 *
 *	Type:
 *		Asynchronous.
 *
 *	Parameters:
 *      bmgContext [in] - BMG context.
 *      
 *      bdAddr [in] - pointer to 48-bit address of device.
 *
 *      sdpServiceMask [in] - a set of SDP services.
 *
 *  Generated events:
 *		BTEVENT_DISCOVER_SERVICES_RESULT
 *
 *  Returns:
 *      BT_STATUS_PENDING - operation started successfully.
 *
 *      BT_STATUS_FAILED  - operation failed to start.
 *
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 *
 *		BT_STATUS_NO_RESOURCES - Not enough resources for execution
 */
BtStatus BTL_BMG_DiscoverServices(BtlBmgContext *bmgContext,
								  const BD_ADDR *bdAddr,
								  const SdpServicesMask sdpServicesMask);


/*-------------------------------------------------------------------------------
 * BTL_BMG_CancelDiscoverServices()
 *
 * Brief: 
 *		Cancel an ongoing discover services request.
 *
 * Description: 
 *		Cancel an ongoing discover services request.
 *
 *  Type:
 *      Asynchronous\Synchronous.
 *
 *  Parameters:
 *      bmgContext [in] - BMG context.
 *
 *	Generated events:
 *			BTEVENT_SERVICE_SEARCH_ATTRIBUTES_CANCELLED if BT_STATUS_PENDING was returned.
 *  Returns:
 *      BT_STATUS_SUCCESS - operation completed successfully.
 *
 *      BT_STATUS_PENDING - operation started successfully.
 *
 *      BT_STATUS_FAILED  - operation failed to start.
 *
 *      BT_STATUS_INVALID_PARM - invalid parameter.
 *          (XA_ERROR_CHECK only).
 */
BtStatus BTL_BMG_CancelDiscoverServices(BtlBmgContext *bmgContext);

/*-------------------------------------------------------------------------------
 * BTL_BMG_SetUuid16()
 *     
 * Brief: 
 *		Utility function for setting uuids to BtlBmgUuid structure in order to be passed in
 *		BTL_BMG_ServiceSearchRequest & BTL_BMG_ServiceSearchAttributeRequest uuid parameter.
 *
 * Description: 
 *		Utility function for setting uuids to BtlBmgUuid structure in order to be passed in
 *		BTL_BMG_ServiceSearchRequest & BTL_BMG_ServiceSearchAttributeRequest uuid parameter.
 *
 *  Parameters:
 *    sourceUuid [in] - required uuid in host endianity.
 *
 *		destUuid [out] - 128 bit converted uuid in Big endian order.
 *      
 *  Returns:
 *      BT_STATUS_PENDING - operation started successfully.
 *
 *      BT_STATUS_INVALID_PARM - invalid parameter.
 */
BtStatus BTL_BMG_SetUuid16(U16 sourceUuid, BtlBmgUuid *destUuid);


/*-------------------------------------------------------------------------------
 * BTL_BMG_SetUuid32()
 *  
 * Brief: 
 *		Utility function for setting uuids to BtlBmgUuid structure in order to be passed in
 *		BTL_BMG_ServiceSearchRequest & BTL_BMG_ServiceSearchAttributeRequest uuid parameter.
 *
 * Description: 
 *		Utility function for setting uuids to BtlBmgUuid structure in order to be passed in
 *		BTL_BMG_ServiceSearchRequest & BTL_BMG_ServiceSearchAttributeRequest uuid parameter.
 *
 *  Parameters:
 *    sourceUuid [in] - required uuid in host endianity.
 *
 *		destUuid [out] - 128 bit converted uuid in Big endian order.
 *      
 *  Returns:
 *      BT_STATUS_PENDING - operation started successfully.
 *
 *      BT_STATUS_INVALID_PARM - invalid parameter.
 */
BtStatus BTL_BMG_SetUuid32(U32 sourceUuid, BtlBmgUuid *destUuid);


/*-------------------------------------------------------------------------------
 * BTL_BMG_SetUuid128()
 *  
 * Brief: 
 *		Utility function for setting uuids to BtlBmgUuid structure in order to be passed in
 *		BTL_BMG_ServiceSearchRequest & BTL_BMG_ServiceSearchAttributeRequest uuid parameter.
 *
 * Description: 
 *		Utility function for setting uuids to BtlBmgUuid structure in order to be passed in
 *		BTL_BMG_ServiceSearchRequest & BTL_BMG_ServiceSearchAttributeRequest uuid parameter.
 *
 *  Parameters:
 *    sourceUuid [in] - required uuid in Big endian order.
 *
 *		destUuid [out] - 128 bit converted uuid in Big endian order.
 *      
 *  Returns:
 *      BT_STATUS_PENDING - operation started successfully.
 *
 *      BT_STATUS_INVALID_PARM - invalid parameter.
 */
BtStatus BTL_BMG_SetUuid128(U8* sourceUuid, BtlBmgUuid *destUuid);



/*-------------------------------------------------------------------------------
 * BTL_BMG_ServiceSearchRequest()
 *
 * Brief: 
 *		Get record handle of services that contain all uuids that appear in uuidList.
 *
 * Description: 
 * 		Only one service discovery request to a remote device can be active,
 * 		A second service search request while the first request is running will fail.
 * 
 * 		service record handle that contains ALL of the UUIDs given by the user will be returned
 *
 * 		As specified in bluetooth SIG spec. the maximal number of UUIDs
 * 		for a service discovery operation is 12.
 *
 * Type:
 *      Asynchronous.
 *
 * Parameters:
 *		bmgContext [in] - BMG context.
 *      
 *    bdAddr [in] - pointer to 48-bit address of device.
 *
 *	 	uuidList[in] - UUID values for the service discovery operation - 128 bit UUID in big endian - 
 *									this value can be computed using BTL_BMG_SetUuid128, BTL_BMG_SetUuid32 and BTL_BMG_SetUuid16 utility functions. 
 *
 *		numOfUuids[in] - Length of uuidList
 *
 *		maxServiceRecords[in] - maximal number of service records to retrieve after validation
 *
 *	requestId [out] - a service request Id generated by the service search operation,
 *				each ongoing service discovery request has a unique request ID.
 *
 *	Generated events: BTEVENT_SERVICE_SEARCH_COMPLETE
 *  Returns:
 *      BT_STATUS_PENDING - operation started successfully.
 *
 *      BT_STATUS_FAILED  - operation failed to start.
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 *
 *		BT_STATUS_NO_RESOURCES - Not enough resources for execution
 */
BtStatus BTL_BMG_ServiceSearchRequest(BtlBmgContext *bmgContext,
																			BD_ADDR *addr,
																			BtlBmgUuid* uuidList,
																			U8  numOfUuids,
																			U16	maxServiceRecords,
																			U32 *requestId);    


/*-------------------------------------------------------------------------------
 * BTL_BMG_CancelServiceSearchRequest()
 *
 * Brief: 
 *		Cancel an ongoing service search request.
 *
 * Description: 
 *		Cancel an ongoing service search request.
 *
 *  Type:
 *      Asynchronous\Synchronous.
 *
 *  Parameters:
 *      bmgContext [in] - BMG context.
 *
 *		requestId [in] - a request Id generated by a previous service search request.
 *      
 *	Generated events:
 *			BTEVENT_SERVICE_SEARCH_ATTRIBUTES_CANCELLED if BT_STATUS_PENDING was returned.
 *  Returns:
 *      BT_STATUS_SUCCESS - operation completed successfully.
 *
 *      BT_STATUS_PENDING - operation started successfully.
 *
 *      BT_STATUS_FAILED  - operation failed to start.
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 */
BtStatus BTL_BMG_CancelServiceSearchRequest(BtlBmgContext *bmgContext, const U32 requestId);

/*-------------------------------------------------------------------------------
 * BTL_BMG_ServiceAttributeRequest()
 *
 * Brief: 
 *		Get attributes of services defined in attributeSearchParams.
 *
 * Description:
 *		retrieves all attributes values of attributeIdList within the given recordHandle.
 *		Only one service attribute request to a remote device can be active,
 * 		A second service attribute request while the first request is running will fail.
 *
 *		Only values of attribute IDs that appear in the service record will be returned.
 *      
 *  Type:
 *      Asynchronous.
 *
 *  Parameters:
 *      bmgContext [in] - BMG context.
 *      
 *      bdAddr [in] - pointer to 48-bit address of device.
 *
 *			attributeIdList[in] - list of attributes IDs to fetch
 *
 *			attributeIdListLen[in] - Length of attributeIdList must not be greter then 
 *															BTL_CONFIG_BMG_MAX_NUM_OF_ATTRIBUTES_TO_SEARCH
 *
 *			requestId [out] - request Id generated by the attributes search operation,
 *				each ongoing attribute request has a unique request ID.
 *
 *  Generated events: BTEVENT_SERVICE_ATTRIBUTES_COMPLETE
 *
 *  Returns:
 *      BT_STATUS_PENDING - operation started successfully.
 *
 *      BT_STATUS_FAILED  - operation failed to start.
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 *
 *		BT_STATUS_NO_RESOURCES - Not enough resources for execution
 *
 */
BtStatus BTL_BMG_ServiceAttributeRequest(BtlBmgContext *bmgContext,
									BD_ADDR *addr,
									U32 recordHandle,
									SdpAttributeId *attributeIdList,
									U16 attributeIdListLen,
									U32 *requestId);

/*-------------------------------------------------------------------------------
 * BTL_BMG_CancelServiceAttributeRequest()
 *
 * Brief: 
 *		Cancel an ongoing service search attributes request.
 *
 * Description: 
 *		Cancel an ongoing service search attributes request.
 *
 *  Type:
 *      Asynchronous\Synchronous.
 *
 *  Parameters:
 *      bmgContext [in] - BMG context.
 *
 *		requestId [in] - a request Id generated by a previous service attributes search request.
 *  
 *	Generated events:
 *			BTEVENT_SERVICE_SEARCH_ATTRIBUTES_CANCELLED if BT_STATUS_PENDING was returned.
 *  Returns:
 *      BT_STATUS_SUCCESS - operation completed successfully.
 *
 *      BT_STATUS_PENDING - operation started successfully.
 *
 *      BT_STATUS_FAILED  - operation failed to start.
 *
 *      BT_STATUS_INVALID_PARM - invalid parameter.
 */
BtStatus BTL_BMG_CancelServiceAttributeRequest(BtlBmgContext *bmgContext, const U32 requestId);


/*-------------------------------------------------------------------------------
 * BTL_BMG_ServiceSearchAttributeRequest()
 *
 * Brief: 
 *		Get attributes of services defined in discoveryParams.
 *    
 * Description:
 *		An event BTEVENT_SERVICE_SEARCH_ATTRIBUTES_RESULT will be sent for every service 
 *		record discovered.
 *		Only one service discovery request to a remote device can be active,
 * 		A second service search attribute request while the first request is running will fail.
 *      
 *		For each service record that contains ALL of the UUIDs given by the user:
 *
 *	  Service search requests the value of the attributes given in attributeIdList.
 *
 *		Only values of attribute IDs that appear in the service record will be returned.
 *      
 *    As specified in bluetooth SIG spec. the maximal number of UUIDs
 *    for a service discovery operation is 12.
 *
 *  Type:
 *      Asynchronous.
 *
 *  Parameters:
 *      bmgContext [in] - BMG context.
 *      
 *      bdAddr [in] - pointer to 48-bit address of device.
 *
 *	 		uuidList[in] - UUID values for the service discovery operation - 128 bit UUID in big endian - 
 *									this value can be computed using BTL_BMG_SetUuid128, BTL_BMG_SetUuid32 and BTL_BMG_SetUuid16 utility functions.
 *
 *			numOfUuids[in] - Length of uuidList
 *
 *			maxServiceRecords[in] - maximal number of service records to retrieve after validation
 *			
 *			requestId [out] - a service request Id generated by the service search attributes operation,
 *				each ongoing service discovery request has a unique request ID.
 *
 *			attributeIdList[in] - list of attributes IDs to fetch
 *
 *			attributeIdListLen[in] - Length of attributeIdList must not be greater then 
 *															BTL_CONFIG_BMG_MAX_NUM_OF_ATTRIBUTES_TO_SEARCH
 *
 *	Generated events:
 *			BTEVENT_SERVICE_SEARCH_ATTRIBUTES_RESULT
 *			BTEVENT_SERVICE_SEARCH_ATTRIBUTES_COMPLETE
 *
 *  Returns:
 *      BT_STATUS_PENDING - operation started successfully.
 *
 *      BT_STATUS_FAILED  - operation failed to start.
 *
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 *
 *		BT_STATUS_NO_RESOURCES - Not enough resources for execution
 */
BtStatus BTL_BMG_ServiceSearchAttributeRequest(BtlBmgContext *bmgContext,
                                  			const BD_ADDR *bdAddr,
																				BtlBmgUuid* uuidList,
																				U8  numOfUuids,
																				U16	maxServiceRecords,
																				SdpAttributeId *attributeIdList,
																				U16 attributeIdListLen,
																				U32 *requestId);


/*-------------------------------------------------------------------------------
 * BTL_BMG_CancelServiceSearchAttributeRequest()
 *
 * Brief: 
 *		Cancel an ongoing service discovery request.
 *
 * Description: 
 *		Cancel an ongoing service discovery request.
 *
 *  Type:
 *      Asynchronous\Synchronous.
 *
 *  Parameters:
 *      bmgContext [in] - BMG context.
 *
 *			requestId [in] - a request Id generated by a previous service discovery request.
 *      
 *	Generated events:
 *			BTEVENT_SERVICE_SEARCH_ATTRIBUTES_CANCELLED if BT_STATUS_PENDING was returned.
 *
 *  Returns:
 *      BT_STATUS_SUCCESS - operation completed successfully.
 *
 *      BT_STATUS_PENDING - operation started successfully.
 *
 *      BT_STATUS_FAILED  - operation failed to start.
 *
 *      BT_STATUS_INVALID_PARM - invalid parameter.
 */
BtStatus BTL_BMG_CancelServiceSearchAttributeRequest(BtlBmgContext *bmgContext, 
													const U32 requestId);


/*-------------------------------------------------------------------------------
 * BTL_BMG_SetServiceRecord()
 *
 * Brief: 
 *		Adds/updates the service record specified by the record handle.
 *
 * Description:
 *		Sets record in the local SDP server.
 *		note that only records that were added by the user can be updated. 
 *		records that were added by the profiles can not be updated.
 *		If *recordHandle == 0 then a new service record will be allocated,
 *		the new handle value will be assigned to recordHandle.
 *
 *		Attributes with null value will be removed if they appear in the service record.
 *    Attributes that already appear in the service record will be updated.
 *    Attributes that do not appear in the service record will be added to the record.
 *
 * Type:
 *      Synchronous.
 *
 * Parameters:
 *      bmgContext in] - [BMG context.
 *
 *      recordHandle [in/out] - the service record handle.
 *
 *	  classOfDevice [in] - The service class fields of the class of device. Use the values defined
 *						 in me.h. The device class portion is ignored.
 *						 this parameter is ignored when updating a service record.
 *
 *      attributeList [in] - an array of attributes.
 *
 *      attributeListLen [in] - the length of attribute list.
 *
 *  Returns:
 *      BT_STATUS_SUCCESS - The service record has been updated.
 *
 *      BT_STATUS_FAILED - An ongoing service discovery from a remote device prevents changing the record.
 *
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 *
 *		BT_STATUS_NO_RESOURCES - Not enough resources for execution
 */
BtStatus BTL_BMG_SetServiceRecord(BtlBmgContext *bmgContext, 
									U32 *recordHandle, 
									BtClassOfDevice classOfDevice,
									const SdpAttribute *attributeList, 
									const U32 attributeListLen);


/*---------------------------------------------------------------------------
 * BTL_BGM_RemoveServiceRecord()
 *
 * Brief: 
 *		Removes a service record from the SDP database.
 *
 * Description:
 *		Removes a record from the local SDP server.
 *		note that only records that were added by the user can be removed. records that were added
 *		by the profiles can not be removed.
 *
 * Type:
 *      Synchronous.
 *
 * Parameters:
 *     bmgContext [in] - BMG context.
 *
 *     recordHandle [in] - The record's handle.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - record is removed successfully.
 *
 *		BT_STATUS_INVALID_PARM -  Invalid parameter.
 *
 *     BT_STATUS_FAILED - record does not exist in database.
 */
BtStatus BTL_BMG_RemoveServiceRecord(BtlBmgContext *bmgContext, U32 recordHandle);


/*-------------------------------------------------------------------------------
 * BTL_BMG_EnumServiceRecords()
 *
 * Brief: 
 *		Enumerates all local service records.
 *	
 * Description: 
 *		Cancel an ongoing service discovery request.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *    bmgContext [in] - BMG context.
 *
 *		recordHandle [in/out] - A pointer to a record handle. The first time
 *         this function is called, recordHandle points to NULL (*recordHandle == 0) . Upon return,
 *         "recordHandle" will hold the record handle of the first element in the list of local
 *         service records.
 *         On subsequent calls, "recordHandle" will hold the record handle of the following element
 *         of the one provided by the BTL_BMG_EnumServiceRecords call.
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - The next device has been stored in "recordHandle".
 *
 *    BT_STATUS_FAILED - The service record referenced to by recordHandle is not on the
 *         service record list.
 *
 *    BT_STATUS_NOT_FOUND - There are no more service records on the list.
 *
 *    BT_STATUS_INVALID_PARM - The recordHandle parameter was invalid.
 */
BtStatus BTL_BMG_EnumServiceRecords(BtlBmgContext *bmgContext, U32 *recordHandle);

/*-------------------------------------------------------------------------------
 * BTL_BMG_GetServiceRecord()
 *
 * Brief: 
 *		returns the service record content.
 *
 * Description: 
 *		returns the service record content.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *      bmgContext [in] - BMG context.
 *
 *     	recordHandle [in] - The record's handle.
 *	
 *			BtClassOfDevice[out] - record's class of device
 *
 *			SdpAttribute[out] - pointer array of attributes
 *
 *			attributeListLen[out] - the length of attribute list.
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - The next device has been stored in "recordHandle".
 *
 *     	BT_STATUS_FAILED - The service record referenced to by recordHandle is not on the
 *         service record list.
 *
 *     	BT_STATUS_INVALID_PARM - The recordHandle parameter is invalid or one of bmgContext/cod/attributeList/attributeList is 0.
 */
BtStatus BTL_BMG_GetServiceRecord(BtlBmgContext *bmgContext, 
								const U32 recordHandle, 
								BtClassOfDevice *cod, 
								SdpAttribute **attributeList,
								U32 *attributeListLen);

/*-------------------------------------------------------------------------------
 * BTL_BMG_GetNumberOfServiceRecords()
 *
 * Brief: 
 *		Returns the number of SDP service records. 
 *
 * Description:
 *		The returned number of service records includes both records that
 *		were inserted by the user and ones that were inserted by the profiles internally.
 *		 
 * Type:
 *		Synchronous
 *
 * Parameters:
 *      bmgContext [in] - BMG context.
 *	
 *	numOfServiceRecords[out] - number of SDP service records
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - The number has been stored in "numOfServiceRecords".
 *
 *     	BT_STATUS_INVALID_PARM - bmgContext or numOfServiceRecords is 0.
 */
BtStatus BTL_BMG_GetNumberOfServiceRecords(BtlBmgContext *bmgContext, U16 *numOfServiceRecords);

#endif /* __BTL_BMG_H */



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
*   FILE NAME:		btl_mdg.h
*
*   BRIEF:          This file defines the API of the BTL Modem Data Gateway -  
*                   Gateway roles of the Dial-up Networking and Fax Profiles.
*
*   DESCRIPTION:	The BTL MDG provides abstraction layer for DUN or FAX
*                   applications. This layer hides interface to the Bluetooth
*                   Serial Port Profile and, thus, simplifies application's code.
*
*   AUTHOR:			V. Abram
*
\*******************************************************************************/

#ifndef __BTL_MDG_H
#define __BTL_MDG_H
 
/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "bttypes.h"
#include "btl_spp.h"
#include "bthal_types.h"
#include "bthal_common.h"
#include "bthal_md.h"


 /********************************************************************************
 *
 * Types
 *
 *******************************************************************************/
 
/*---------------------------------------------------------------------------
 * BtlMdgEvent type
 *
 *	   Represents possible types of events which may be received by application
 *	   using the MDG services.
 */
typedef U8 BtlMdgServerEvent;

/* MDG server is initialized */
#define BTL_MDG_SERVER_EVENT_INITIALIZED	    (1)

/* MDG server is deinitialized */
#define BTL_MDG_SERVER_EVENT_DEINITIALIZED		(2)

/* MDG server is enabled */
#define BTL_MDG_SERVER_EVENT_ENABLED		    (3)

/* MDG server is disabled */
#define BTL_MDG_SERVER_EVENT_DISABLED			(4)

/* MDG server is connected to the data terminal device */
#define BTL_MDG_SERVER_EVENT_DT_CONNECTED		(5)

/* MDG server is disconnected from a data terminal device */
#define BTL_MDG_SERVER_EVENT_DT_DISCONNECTED	(6)

/* MDG server is connected to the modem */
#define BTL_MDG_SERVER_EVENT_MODEM_CONNECTED	(7)

/* MDG server is disconnected from the modem */
#define BTL_MDG_SERVER_EVENT_MODEM_DISCONNECTED	(8)


/* Forward declarations */
typedef struct _BtlMdgServerContext BtlMdgServerContext;
typedef struct _BtlMdgCallbackParms BtlMdgCallbackParms;

/*-------------------------------------------------------------------------------
 * BtlMdgCallBack type
 *
 *     A function of this type is called to indicate BTL MDG events.
 */
typedef void (*BtlMdgServerCallBack)(const BtlMdgCallbackParms *parms);


/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/
 
/*-------------------------------------------------------------------------------
 * BtlMdgEvent structure
 *
 *     Includes information passed along with the callback function
 */
struct _BtlMdgCallbackParms 
{
	/* BTL MDG event */
	BtlMdgServerEvent	event;
	
	/* Context of a server event received from */
	BtlMdgServerContext	*serverContext;
	
	/* BD ADDR of the remote device */
	BD_ADDR				bdAddr;
};

/*-------------------------------------------------------------------------------
 * BtlMdgServerSettings structure
 *
 *	   Structure for passing parameters, when creating a EDG server.
 */
typedef struct _BtlMdgServerSettings
{
	/* Type of MDG server: DUN or FAX */
	BtlSppServiceType		    serviceType;
	
	const BtlUtf8  			    serviceName[SPP_SERVICE_NAME_MAX_SIZE];

    const BtlSppPortSettings    portSettings;

} BtlMdgServerSettings;


/********************************************************************************
 *
 * Function declarations
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BTL_MDG_Init()
 *
 * Brief:  
 *      Initializes BTL MDG module.
 *
 * Description:
 *		Initializes software structures and allocates required resources.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	None.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - if the module was successfully initialized.
 *
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type,
 *			if the initialization failed.
 */
BtStatus BTL_MDG_Init(void);

/*-------------------------------------------------------------------------------
 * BTL_MDG_Deinit()
 *
 * Brief:  
 *      Deinitializes BTL MDG module.
 *
 * Description:
 *		Releases resources allocated by the BTL MDG module.
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
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type,
 *			if the initialization failed.
 */
BtStatus BTL_MDG_Deinit(void);

/*-------------------------------------------------------------------------------
 * BTL_MDG_Create()
 *
 * Brief:  
 *      Creates a Modem Data Gateway server.
 *
 * Description:
 *     	Creates context for the MDG server and context for the underlying SPP
 *      port. Callback function will be registered for delivering MDG events to
 *      the application.
 *      
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	appHandle [in] - handle of the application creating the MDG server.
 *
 *     	dgServerCallback [in] - callback function for receiving MDG events.
 *
 *		securityLevel [in] - pointer to level of security which should be applied,
 *			when a remote device will try to connect to the server's service or
 *			when a local device will try to connect the client port to the
 *			remote SPP port.
 *			NULL may be passed, if application does not want to overwrite a default
 *			value set by a profile or a default value BSL_DEFAULT of the entire
 *			program, if BT_DEFAULT_SECURITY is enabled.
 *
 *     	serverContext [out] - pointer to the created server's context, returned
 *			by the function.
 * Returns:
 *		BT_STATUS_SUCCESS - if the creation of the server was successful.
 *
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type, if the
 *			operation failed.
 */
BtStatus BTL_MDG_Create(const BtlAppHandle *appHandle,
						const BtlMdgServerCallBack serverCallback,
						const BtSecurityLevel *securityLevel,
						BtlMdgServerContext **serverContext);

/*-------------------------------------------------------------------------------
 * BTL_MDG_Destroy()
 *
 * Brief:  
 *      Destroys a Modem Data Gateway server.
 *
 * Description:
 *     	Removes previously created Modem Data Gateway server.
 *		If the connection to the remote client device exists, it will be
 *			disconnected.
 *		If the connection to the modem exists, it will be disconnected.
 * Type:
 *		Asynchronous or synchronous.
 *
 * Parameters:
 *     	serverContext [in] - pointer to the created server's context, returned
 *			at the server creation.
 *
 * Generated Events:
 *      BTL_MDG_EVENT_CLOSED
 *      BTL_MDG_EVENT_MODEM_DISCONNECTED
 *
 * Returns:
 *		BT_STATUS_SUCCESS - if the server was successfully destroyed.
 *
 *		BT_STATUS_PENDING - if the operation has been successfully started.
 *
 *			The following events will be delivered via the callback function
 *          registered with BTL_MDG_Create() function:
 *           - BTL_MDG_EVENT_CLOSED upon successful close of the SPP port used by
 *              the server.
 *           - BTL_MDG_EVENT_MODEM_DISCONNECTED upon successful disconnection
 *              from the modem.
 *
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type,
 *			if the operation failed.
 */
BtStatus BTL_MDG_Destroy(BtlMdgServerContext **serverContext);
       
/*-------------------------------------------------------------------------------
 * BTL_MDG_Enable()
 *
 * Brief:  
 *      Enables a Modem Data Gateway server for DUN or FAX.
 *
 * Description:
 *     	Enables SPP port, which will register required service in the SDP server
 *      database, and registers with the modem's data service.
 *      
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	serverContext [in] - pointer to the created server's context, returned
 *			at the server creation.
 *
 *     	serverSettings [in] - pointer to a structure with parameters required to
 *			create the server.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - if the server was successfully enabled.
 *
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type,
 *			if the operation failed.
 */
BtStatus BTL_MDG_Enable(BtlMdgServerContext *serverContext,
						const BtlMdgServerSettings *serverSettings);

/*-------------------------------------------------------------------------------
 * BTL_MDG_Disable()
 *
 * Brief:  
 *      Disables previously enabled Modem Data Gateway server.
 *
 * Description:
 *		If connections to the remote client device and to the modem exist, they
 *      will be disconnected.
 *      
 * Type:
 *		Asynchronous or synchronous.
 *
 * Parameters:
 *     	serverContext [in] - pointer to the created server's context, returned
 *			at the server creation.
 *
 * Generated Events:
 *      BTL_MDG_SERVER_EVENT_DISABLED
 *
 * Returns:
 *		BT_STATUS_SUCCESS - if the server was successfully enabled.
 *
 *		BT_STATUS_PENDING - if the disabling has been successfully started.
 *
 *          Event BTL_MDG_SERVER_EVENT_DISABLED will be delivered via the
 *          callback function registered with function BTL_MDG_Create(), when the
 *          disabling process is finished.
 *
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type,
 *			if the operation failed.
 */
BtStatus BTL_MDG_Disable(BtlMdgServerContext *serverContext);
			    
/*-------------------------------------------------------------------------------
 * BTL_MDG_GetConnectedDevice()
 *
 * Brief:  
 *      Gets connected BT device.
 *
 * Description:
 *     	Gets BD ADDR of the connected to the server BT device.
 *     
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	serverContext [in] - pointer to the created server's context, returned
 *			at the server creation.
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
BtStatus BTL_MDG_GetConnectedDevice(BtlMdgServerContext *serverContext,
									BD_ADDR *bdAddr);

#if BT_SECURITY == XA_ENABLED
/*-------------------------------------------------------------------------------
 * BTL_MDG_SetSecurityLevel()
 *
 * Brief:  
 *      Sets security level.
 *
 * Description:
 *     	Sets security level for previously created MDG server port.
 *      Requires compilation switch BT_SECURITY is enabled.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	serverContext [in] - pointer to the created server's context, returned
 *			at the server creation.
 *
 *		securityLevel [in] - level of security which should be applied, when
 *			a BT device will try to connect to the server.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - if the security level was successfully changed.
 *
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type,
 *			if the operation failed.
 */
BtStatus BTL_MDG_SetSecurityLevel(BtlMdgServerContext *serverContext,
								  const BtSecurityLevel *securityLevel);

/*-------------------------------------------------------------------------------
 * BTL_MDG_GetSecurityLevel()
 *
 * Brief:  
 *      Gets security level.
 *
 * Description:
 *     	Gets security level for previously created MDG server port.
 *      Requires compilation switch BT_SECURITY is enabled.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	serverContext [in] - pointer to the created server's context, returned
 *			at the server creation.
 *
 *		securityLevel [out] - server's security level.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - if the security level was successfully returned.
 *
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type,
 *			if the operation failed.
 */
BtStatus BTL_MDG_GetSecurityLevel(BtlMdgServerContext *serverContext,
								  BtSecurityLevel *securityLevel);
#endif /* BT_SECURITY == XA_ENABLED */

/*-------------------------------------------------------------------------------
 * BTL_MDG_SetComSettings()
 *
 * Brief:  
 *      Sets communication settings.
 *
 * Description:
 *		Sets server's communication settings.
 *
 * Type:
 *		Asynchronous or synchronous.
 *
 * Parameters:
 *     	serverContext [in] - pointer to the created server's context, returned
 *			at the server creation.
 *
 *	 	comSettings [in] - pointer to a structure with communication settings
 *			of the server (baudrate, data format... originally defined in file
 *			rfcomm.h).
 *
 * Generated Events:
 *      SPP_EVENT_PORT_STATUS_CNF
 *
 * Returns:
 *		BT_STATUS_PENDING - if operation was successful.
 *
 *          Event SPP_EVENT_PORT_STATUS_CNF will be delivered via the callback
 *          function registered with function BTL_MDG_Create(), when a response
 *          will be received from the remote BT device.
 *
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type,
 *			if the creation failed.
 */
BtStatus BTL_MDG_SetComSettings(BtlMdgServerContext *serverContext,
								const SppComSettings *comSettings);

/*-------------------------------------------------------------------------------
 * BTL_MDG_GetComSettings()
 *
 * Brief:  
 *      Gets communication settings.
 *
 * Description:
 *		Gets MDG server's communication settings.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	serverContext [in] - pointer to the created server's context, returned
 *			at the server creation.
 *
 *	 	comSettings [out] - pointer to a structure with returned communication
 *			settings of	the server (baudrate, data format... originally defined
 *			in file rfcomm.h).
 *
 * Returns:
 *		BT_STATUS_SUCCESS - if the port's properties were successfully returned.
 *
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type,
 *			if the creation failed.
 */
BtStatus BTL_MDG_GetComSettings(BtlMdgServerContext *serverContext,
								SppComSettings *comSettings);

/*-------------------------------------------------------------------------------
 * BTL_MDG_TerminateDataCall()
 *
 * Brief:  
 *      Terminates data call.
 *
 * Description:
 *     	Terminates data call originated via the Modem Data Gateway server.
 *		If a connection to the data terminal device exists, it will be
 *			disconnected.
 *		If a connection to the modem exists, it will be disconnected.
 *      
 * Type:
 *		Asynchronous or synchronous.
 *
 * Parameters:
 *     	serverContext [in] - pointer to the server's context returned at a time
 *			of its creation.
 *
 * Generated Events:
 *      BTL_MDG_EVENT_DT_DISCONNECTED
 *      BTL_MDG_EVENT_MODEM_DISCONNECTED
 *
 * Returns:
 *		BT_STATUS_SUCCESS - if the data call was already terminated before and
 *			both the data terminal and the modem were already disconnected.
 *
 *		BT_STATUS_PENDING - if the operation has been successfully started.
 *
 *			The following events will be delivered via the callback function
 *          registered with BTL_MDG_Create() function:
 *			 - BTL_MDG_EVENT_DT_DISCONNECTED upon successful disconnection from
 *              the data terminal device.
 *			 - BTL_MDG_EVENT_MODEM_DISCONNECTED upon successful disconnection
 *              from the modem.
 *
 *		BT_STATUS_FAILED or any specific error defined in BtStatus type,
 *			if the operation failed.
 */
BtStatus BTL_MDG_TerminateDataCall(BtlMdgServerContext *serverContext);


#endif /* __BTL_MDG_H */


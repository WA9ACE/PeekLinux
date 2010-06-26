/*******************************************************************************\
*                                                                           	*
*  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION			*
*																				*
*  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE			*
*	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE		*
*	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO		*
*	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT		*
*	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL			*
*	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.			*
*																				*
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      btl_spp.c
*
*   DESCRIPTION:    Implementation of BTL SPP API.
*
*   AUTHOR:         V. Abram
*
\*******************************************************************************/


#include "btl_config.h"
#include "btl_defs.h"
#include "btl_spp.h"

BTL_LOG_SET_MODULE(BTL_MODULE_TYPE_SPP);

#if BTL_CONFIG_SPP == BTL_CONFIG_ENABLED

/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "spp.h"
#include "sppbuf.h"
#include "btl_commoni.h"
#include "btl_pool.h"

/********************************************************************************
 *
 * Constants 
 *
 *******************************************************************************/


 /********************************************************************************
 *
 * Internal Types
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BtlSppState type
 *
 *     Defines the BTL SPP states.
 */
typedef U8 BtlSppState;

#define BTL_SPP_STATE_NOT_INTIALIZED		(0x00)
#define BTL_SPP_STATE_INITIALIZED			(0x01)

/*-------------------------------------------------------------------------------
 * BtlSppPortState type
 *
 *     Defines the BTL SPP port states.
 */
typedef U8 BtlSppPortState;

#define BTL_SPP_PORT_STATE_IDLE				(0x00)
#define BTL_SPP_PORT_STATE_DISCONNECTED		(0x01)
#define BTL_SPP_PORT_STATE_CONNECTING		(0x02)
#define BTL_SPP_PORT_STATE_CONNECT_IND		(0x03)
#define BTL_SPP_PORT_STATE_DISCONNECTING	(0x04)
#define BTL_SPP_PORT_STATE_CONNECTED		(0x05)
#define BTL_SPP_PORT_STATE_DISABLING		(0x06)


/********************************************************************************
 *
 * Internal Data Structures
 *
 *******************************************************************************/
 
/*-------------------------------------------------------------------------------
 * SppPortContext structure
 *
 *	   Represents BTL SPP context. A pointer to this structure is returned
 *	   by the BTL, when an SPP port is created.
 *	   This pointer	is used by a caller of BTL SPP API as the SPP port's handle.
 */
struct _BtlSppPortContext
{
	/* Base BTL context which includes handle of application  using SPP port */
	BtlContext			base;
	
	/* Port's state */
	BtlSppPortState		state;

	/* SPP device structure used by SPP profile with this port */
	SppDev				sppDev;
	
	/* Callback for receiving all BTL SPP events */
	BtlSppPortCallback	callback;
	
	/* Service name, published, in case of a server port or requested to be
	   connected to, in case of a client port */
	BtlUtf8				serviceName[SPP_SERVICE_NAME_MAX_SIZE];

	/* Connected remote device, null if not connected  */
    BtRemoteDevice		*remDev;

} ;

/*-------------------------------------------------------------------------------
 * BtlSppData structure
 *
 *	   Includes internal data required for BTL SPP functioning.
 */
typedef struct _BtlSppData
{
	/* Pool of SPP ports contexts */
	BTL_POOL_DECLARE_POOL(contextsPool, btlSppPortsContextsMemory, BTL_CONFIG_SPP_MAX_NUM_OF_PORTS_CONTEXTS, sizeof(BtlSppPortContext));

	/* List of active SPP ports contexts */
	ListEntry	contextsList;

    /* Indicates whether RADIO OFF notification was received */
    BOOL        isRadioOffNotified;

} BtlSppData;


/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * btlSppPortComSettingsDefault
 *
 *     Represents the default communication settings of the SPP port.
 */
static const SppComSettings btlSppPortComSettingsDefault =
{
		SPP_COM_SETTING_BAUDRATE_DEFAULT,
		SPP_COM_SETTING_DATA_FORMAT_DEFAULT,
		SPP_COM_SETTING_FLOW_CONTROL_DEFAULT,
		SPP_COM_SETTING_XON_CHAR_DEFAULT,
		SPP_COM_SETTING_XOFF_CHAR_DEFAULT,
		SPP_COM_SETTINGS_PARM_MASK_DEFAULT
};

/*-------------------------------------------------------------------------------
 * btlSppPortsContextsPoolName
 *
 *     Represents the name of the SPP ports contexts pool.
 */
static const char btlSppPortsContextsPoolName[] = "SppPortsContexts";

/*-------------------------------------------------------------------------------
 * btlSppState
 *
 *     Represents the current state of the BTL SPP module.
 */
static BtlSppState btlSppState = BTL_SPP_STATE_NOT_INTIALIZED;

/*-------------------------------------------------------------------------------
 * btlSppData
 *
 *     Represents the internal data of the BTL SPP module.
 */
static BtlSppData btlSppData;


/********************************************************************************
 *
 * Internal function prototypes
 *
 *******************************************************************************/
static void BtlSppEventsCallback(SppDev *dev, SppCallbackParms *parms);
BtStatus BtlSppGetUserType(const BtlSppPortSettings *btlPortSettings,
                           SppUserType *sppUserType);

static BtStatus BtlSppProcessRadioOffNotification(void);
static BtStatus BtlSppBtlNotificationsCb(BtlModuleNotificationType notificationType);

/********************************************************************************
 *
 * Function definitions
 *
 *******************************************************************************/
 
/*-------------------------------------------------------------------------------
 * BTL_SPP_Init()
 *
 *		Initializes BTL SPP module and allocates required resources.
 */
BtStatus BTL_SPP_Init()
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_COMMON("BTL_SPP_Init");

	BTL_VERIFY_ERR((BTL_SPP_STATE_NOT_INTIALIZED == btlSppState),
				   BT_STATUS_FAILED,
				   ("BTL_SPP: BTL_SPP_Init was already called"));
	
	/* Create pool for SPP port contexts */
	status = BTL_POOL_Create(&btlSppData.contextsPool,
							 btlSppPortsContextsPoolName,
							 btlSppData.btlSppPortsContextsMemory, 
							 BTL_CONFIG_SPP_MAX_NUM_OF_PORTS_CONTEXTS,
							 sizeof(BtlSppPortContext));

	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status),
					 BT_STATUS_INTERNAL_ERROR,
					 ("BTL_SPP: SPP ports contexts pool creation failed"));
	
	InitializeListHead(&btlSppData.contextsList);

	BTL_RegisterForCommonNotifications(BTL_MODULE_TYPE_SPP, BtlSppBtlNotificationsCb);

    btlSppData.isRadioOffNotified = FALSE;
	btlSppState = BTL_SPP_STATE_INITIALIZED;
		
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

/*-------------------------------------------------------------------------------
 * BTL_SPP_Deinit()
 *
 *		Deinitializes BTL SPP module and frees allocated resources.
 */
BtStatus BTL_SPP_Deinit()
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_COMMON("BTL_SPP_Deinit");

	BTL_DeRegisterFromCommonNotifications(BTL_MODULE_TYPE_SPP);

	BTL_VERIFY_ERR((BTL_SPP_STATE_INITIALIZED == btlSppState),
				   BT_STATUS_FAILED,
				   ("BTL_SPP: BTL_SPP_Deinit was already called"));
	
	BTL_VERIFY_ERR((IsListEmpty(&btlSppData.contextsList)), 
				   BT_STATUS_FAILED,
				   ("BTL_SPP: SPP ports contexts are still active"));
	
	/* Destroy pool which was used for SPP ports' contexts */
	status = BTL_POOL_Destroy(&btlSppData.contextsPool);

	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status),
					 BT_STATUS_INTERNAL_ERROR,
					 ("BTL_SPP: SPP ports contexts pool destruction failed"));
	
	btlSppState = BTL_SPP_STATE_NOT_INTIALIZED;
		
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

/*-------------------------------------------------------------------------------
 * BTL_SPP_Create()
 *
 *		Creates either SPP client or SPP server port with required properties.
 */
BtStatus BTL_SPP_Create(BtlAppHandle *appHandle,
					  	const BtlSppPortCallback portCallback,
						const BtSecurityLevel *securityLevel,
						BtlSppPortContext **portContext)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_SPP_Create");

	BTL_VERIFY_ERR((0 != portCallback),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_SPP: Null portCallback"));

	BTL_VERIFY_ERR((0 != portContext),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_SPP: Null portContext"));

#if BT_SECURITY == XA_ENABLED

	if (0 != securityLevel)
	{
		status = BTL_VerifySecurityLevel(*securityLevel);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status),
					   status,
					   ("BTL_SPP: Invalid SPP port securityLevel"));
	}

#endif	/* BT_SECURITY == XA_ENABLED */
	
	/* Allocate memory for the new port's context */
	status = BTL_POOL_Allocate(&btlSppData.contextsPool, (void **)portContext);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status),
				   status,
				   ("BTL_SPP: Failed allocating SPP port's context"));

	status = BTL_HandleModuleInstanceCreation(appHandle,
											  BTL_MODULE_TYPE_SPP,
											  &(*portContext)->base);
	if (BT_STATUS_SUCCESS != status)
	{
		/* portContext must be freed before we exit */
		BtStatus retVal = BTL_POOL_Free(&btlSppData.contextsPool, (void **)portContext);
		BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == retVal),
							 ("BTL_SPP: Failed freeing SPP port's context"));

		/* This verify will fail and will handle the exception gracefully */
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status),
					   status,
					   ("BTL_SPP: Failed handling SPP port's creation"));
	}
	
	/* Save the application's callback */
	(*portContext)->callback = portCallback;

	/* Insert context into the contexts list */
	InsertTailList(&btlSppData.contextsList, &((*portContext)->base.node));
	
#if BT_SECURITY == XA_ENABLED

	/* Create the security record and partially fill it with given security
	 * level or with its default value and with protocol ID */
	if (securityLevel == 0)
	{
		(*portContext)->sppDev.secRec.level =
                                (BtSecurityLevel)SPP_SECURITY_SETTINGS_DEFAULT;
	}
	else
	{
		(*portContext)->sppDev.secRec.level = (BtSecurityLevel)*securityLevel;
	}
	
	(*portContext)->sppDev.secRec.id = SEC_RFCOMM_ID;

#endif	/* BT_SECURITY == XA_ENABLED */

	/* Fill callback to receive SPP events */
	(*portContext)->sppDev.callback = &BtlSppEventsCallback;
	
	/* Init SPP port's state */
	(*portContext)->state = BTL_SPP_PORT_STATE_IDLE;

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

/*-------------------------------------------------------------------------------
 * BTL_SPP_Destroy()
 *
 *     	Removes previously created SPP port.
 */
BtStatus BTL_SPP_Destroy(BtlSppPortContext **portContext)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BOOL isAllocated = FALSE;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_SPP_Destroy");

	BTL_VERIFY_ERR((0 != portContext),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_SPP: Null portContext"));

	BTL_VERIFY_ERR((0 != *portContext),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_SPP: Null *portContext"));

	BTL_VERIFY_ERR(((BTL_SPP_PORT_STATE_IDLE == (*portContext)->state) ||
					(BTL_SPP_PORT_STATE_DISABLING == (*portContext)->state)),
				   BT_STATUS_IN_USE,
				   ("BTL_SPP: SPP port's context is in use"));

	status = BTL_POOL_IsElelementAllocated(&btlSppData.contextsPool,
										   *portContext,
										   &isAllocated);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status),
				   status,
				   ("BTL_SPP: Failed locating given SPP port's context"));
	BTL_VERIFY_ERR((TRUE == isAllocated),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_SPP: Invalid SPP port's context"));

	/* Remove the context from the list of all SPP ports contexts */
	RemoveEntryList(&((*portContext)->base.node));

	status = BTL_HandleModuleInstanceDestruction(&(*portContext)->base);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status),
				   status,
				   ("BTL_SPP: Failed handling SPP port destruction"));

	status = BTL_POOL_Free(&btlSppData.contextsPool, (void **)portContext);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status),
					 BT_STATUS_INTERNAL_ERROR,
					 ("BTL_SPP: Failed freeing SPP port's context"));

	/* Set the SPP port's context to NULL */
	*portContext = 0;

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

/*-------------------------------------------------------------------------------
 * BTL_SPP_Enable()
 *
 *     	Enables previously created SPP port.
 *		In case of a server port, specified service record will be added to
 *			services database.
 */
BtStatus BTL_SPP_Enable(BtlSppPortContext *portContext,
						const BtlUtf8 *serviceName,
						const BtlSppPortSettings *btlPortSettings)
{
	BtStatus status = BT_STATUS_SUCCESS;
	SppUserType sppUserType;
	U16 len = 0;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_SPP_Enable");

	BTL_VERIFY_ERR((0 != portContext),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_SPP: Null portContext"));

	BTL_VERIFY_ERR((0 != btlPortSettings),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_SPP: Null btlPortSettings"));

	BTL_VERIFY_ERR((!((btlPortSettings->dataPathTypes & SPP_PORT_DATA_PATH_TX_SYNC) &&
                      (btlPortSettings->dataPathTypes & SPP_PORT_DATA_PATH_TX_ASYNC)) &&
                    !((btlPortSettings->dataPathTypes & SPP_PORT_DATA_PATH_RX_SYNC) &&
                      (btlPortSettings->dataPathTypes & SPP_PORT_DATA_PATH_RX_ASYNC))),
					BT_STATUS_INVALID_PARM,
					("BTL_SPP: Invalid data paths configuration"));

	BTL_VERIFY_ERR(((BTL_SPP_PORT_STATE_IDLE == portContext->state)||
					(BTL_SPP_PORT_STATE_DISABLING == portContext->state)),
					BT_STATUS_FAILED,
					("BTL_SPP: SPP port is already opened"));

	if ((serviceName != 0) && ((len = OS_StrLen((const char *)serviceName)) != 0))
	{
		/* Service name plus 1 null-terminated char, plus 2 bytes for SDP_TEXT_8BIT */
		BTL_VERIFY_ERR((len <= (SPP_SERVICE_NAME_MAX_SIZE-3)), BT_STATUS_FAILED, ("SPP service name is too long!"));
	}

	/* Check 'serviceType' and 'portType' parameters and convert them to
	 * SppUserType type which is used in underlined SPPDRV module */
	status = BtlSppGetUserType(btlPortSettings, &sppUserType);
	
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status),
				   status,
				   ("BTL_SPP: Incorrect serviceType or portType in portSettings"));

	/* Save checked parameters and other data in the port's context */
	portContext->sppDev.portType = btlPortSettings->portType;
	portContext->sppDev.userType = sppUserType;
    portContext->sppDev.dataPathTypes = btlPortSettings->dataPathTypes;
    portContext->sppDev.portSettings = btlPortSettings->comSettings;

    if (btlPortSettings->dataPathTypes & SPP_PORT_DATA_PATH_RX_SYNC)
    {
        portContext->sppDev.rxMaxNumOfPackets = btlPortSettings->maxNumOfRxSyncPackets;
    }

	/* Update serviceName */
	if (len > 0)
	{
		OS_StrCpy((char *)portContext->serviceName, (const char *)serviceName);
	}
	else
	{
		portContext->serviceName[0] = '\0';
	}
	
	/* Initialize SPP port (service record will be added to the server's
	 * database) and check whether it was done correctly */
	status = SPP_InitDevice(&portContext->sppDev);
	
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status),
				   status,
				   ("BTL_SPP: SPP_InitDevice() failed"));
	
	/* In case of a server port, open the port and start listening */
	if (SPP_SERVER_PORT == btlPortSettings->portType)
	{
		if (len > 0)
		{
			/* The first 2 bytes are for SDP_TEXT_8BIT */
			portContext->sppDev.type.sppService.name[0] = DETD_TEXT + DESD_ADD_8BITS;
			portContext->sppDev.type.sppService.name[1] = (U8)(len + 1);	/* Includes '\0' */
			
			OS_MemCopy((portContext->sppDev.type.sppService.name + 2), serviceName, len);
			portContext->sppDev.type.sppService.name[(len + 2)] = '\0';

			portContext->sppDev.type.sppService.sdpRecord.attribs[5].len = (U16)(len + 3);
			portContext->sppDev.type.sppService.sdpRecord.attribs[5].value = (const U8 *)portContext->sppDev.type.sppService.name;
		}
	}
	else
	{
		/* Save service name */
		if (len > 0)
        {
			portContext->sppDev.type.client.serviceName = portContext->serviceName;
        }
		else
        {
			portContext->sppDev.type.client.serviceName = 0;
        }
	}

	status = SPP_Enable(&portContext->sppDev);

	if (BT_STATUS_SUCCESS == status)
    {
	    /* Set port's state */
	    portContext->state = BTL_SPP_PORT_STATE_DISCONNECTED;

    }
	else
    {
		/* Deinitialize SPP port */
		SPP_DeinitDevice(&portContext->sppDev);

		BTL_LOG_DEBUG(("BTL_SPP: SPP_Enable() failed"));
	}
	
	BTL_FUNC_END_AND_UNLOCK();
	
	return status; 
}

/*-------------------------------------------------------------------------------
 * BTL_SPP_Disable()
 *
 *     	Disables previously enabled SPP port.
 *		In case of a server port, service record will be removed from the services
 *			database.
 *		If connection to the remote port exists, it will be	disconnected.
 */
BtStatus BTL_SPP_Disable(BtlSppPortContext *portContext)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_SPP_Disable");

	BTL_VERIFY_ERR((0 != portContext),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_SPP: Null portContext"));

	BTL_VERIFY_ERR((BTL_SPP_PORT_STATE_IDLE != portContext->state),
				   BT_STATUS_FAILED,
				   ("BTL_SPP: SPP port is not enabled"));

	BTL_VERIFY_ERR((!(BTL_SPP_PORT_STATE_DISABLING == portContext->state)),
				   BT_STATUS_IN_PROGRESS,
				   ("BTL_SPP: SPP port is already in disabling state"));

	if ((BTL_SPP_PORT_STATE_CONNECTING == portContext->state) ||
        (BTL_SPP_PORT_STATE_CONNECTED == portContext->state))
	{
	    status = SPP_Disconnect(&portContext->sppDev);

   	    BTL_VERIFY_ERR((BT_STATUS_FAILED != status),
				       BT_STATUS_FAILED,
    			       ("BTL_SPP: SPP_Disconnect() failed"));
        
        if (BT_STATUS_SUCCESS == status)
        {
            /* Set port's state */
		    portContext->state = BTL_SPP_PORT_STATE_DISCONNECTED;
        }
        else
        {
            /* Set port's state */
		    portContext->state = BTL_SPP_PORT_STATE_DISABLING;

            /* Continuation of the port disabling will be done upon receiving event
		     * SPP_EVENT_CLOSED or other SPP events in case the port is in collision
             * of establishing connection */
        }
	}
    
	if (BTL_SPP_PORT_STATE_DISCONNECTED == portContext->state)
	{
        status = SPP_Disable(&portContext->sppDev);

	    if (BT_STATUS_SUCCESS == status)
	    {
		    /* Closing of the port successfully finished  - release its resources */
		    status = SPP_DeinitDevice(&portContext->sppDev);

		    /* Update port's state */
		    portContext->state = BTL_SPP_PORT_STATE_IDLE;

		    BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status),
					       status,
					       ("BTL_SPP: SPP_DeinitDevice() failed"));
	    }
	}

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

/*-------------------------------------------------------------------------------
 * BTL_SPP_Connect()
 *
 *     	Connects previously created and opened SPP client port to requested
 *		service in requested remote device.
 */
BtStatus BTL_SPP_Connect(BtlSppPortContext *portContext,
						 const BD_ADDR *bdAddr)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK("BTL_SPP_Connect");

	BTL_VERIFY_ERR((0 != portContext),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_SPP: Null portContext"));

	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("BTL_SPP: Null bdAddr"));

	BTL_VERIFY_ERR((BTL_SPP_PORT_STATE_DISCONNECTED == portContext->state),
				   BT_STATUS_FAILED,
				   ("BTL_SPP: SPP port is not disconnected"));

	BTL_VERIFY_ERR((SPP_CLIENT_PORT == portContext->sppDev.portType),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_SPP: Not a client port type"));

	/* Start connecting client port to a server */
	status = SPP_Connect(&portContext->sppDev, (BD_ADDR *)bdAddr);

	BTL_VERIFY_ERR(((BT_STATUS_SUCCESS == status) || (BT_STATUS_PENDING == status)),
				   status,
				   ("BTL_SPP: SPP_Connect() failed"));

    /* Update port's state */
    portContext->state = BTL_SPP_PORT_STATE_CONNECTING;
	
	BTL_FUNC_END_AND_UNLOCK();
	
	return status; 
}

/*-------------------------------------------------------------------------------
 * BTL_SPP_Disconnect()
 *
 *     	Disconnects either client or server SPP port from the SPP port in the
 *			remote device.
 *		All data in receive and transmit buffers will be discarded.
 *		In case of a client port, it will be also closed.
 *		In case of a server port, it will continue to listen.
 */
BtStatus BTL_SPP_Disconnect(BtlSppPortContext *portContext)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK("BTL_SPP_Disconnect");

	BTL_VERIFY_ERR((0 != portContext),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_SPP: Null portContext"));

	BTL_VERIFY_ERR(((BTL_SPP_PORT_STATE_CONNECTED == portContext->state) ||
                    (BTL_SPP_PORT_STATE_CONNECTING == portContext->state)),
				   BT_STATUS_NO_CONNECTION,
				   ("BTL_SPP: SPP port is not connected"));

	/* Disconnect port */
	status = SPP_Disconnect(&portContext->sppDev);

	BTL_VERIFY_ERR((BT_STATUS_FAILED != status),
				   status,
				   ("BTL_SPP: SPP_Disconnect() failed"));
	
	/* Update port's state */
    if (BT_STATUS_SUCCESS == status)
    {
	    portContext->state = BTL_SPP_PORT_STATE_DISCONNECTED;
    }
    else
    {
	    portContext->state = BTL_SPP_PORT_STATE_DISCONNECTING;
    }

	BTL_FUNC_END_AND_UNLOCK();

	return status; 
}

/*-------------------------------------------------------------------------------
 * BTL_SPP_GetConnectedDevice()
 */
BtStatus BTL_SPP_GetConnectedDevice(BtlSppPortContext *portContext, 
									BD_ADDR *bdAddr)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK("BTL_SPP_GetConnectedDevice");

	BTL_VERIFY_ERR((0 != portContext),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_SPP: Null portContext"));

	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("BTL_SPP: Null bdAddr"));

	BTL_VERIFY_ERR((BTL_SPP_PORT_STATE_CONNECTED == portContext->state),
				   BT_STATUS_NO_CONNECTION,
				   ("BTL_SPP: Port is not connected"));

	BTL_ASSERT(0 != portContext->remDev);

	OS_MemCopy((U8 *)bdAddr,
			   (const U8 *)(&(portContext->remDev->bdAddr)),
			   sizeof(BD_ADDR));

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

/*-------------------------------------------------------------------------------
 * BTL_SPP_ReadAsync()
 *
 *     	Reads data from previously opened SPP port.
 */
BtStatus BTL_SPP_ReadAsync(BtlSppPortContext *portContext,
					       U8 *buffer,
					       U16 maxBytes,
					       U16 *readBytes)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK("BTL_SPP_ReadAsync");

	*readBytes = 0;

	BTL_VERIFY_ERR((0 != portContext),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_SPP: Null portContext"));

	BTL_VERIFY_ERR((0 != buffer), BT_STATUS_INVALID_PARM, ("BTL_SPP: Null buffer"));

	BTL_VERIFY_ERR((0 != maxBytes), BT_STATUS_INVALID_PARM, ("BTL_SPP: Null maxBytes"));

	/* Read required amount of bytes from SPP port */
	status = SPP_ReadAsync(&portContext->sppDev, buffer, &maxBytes);

	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status),
				   status,
				   ("BTL_SPP: SPP_Read() failed"));
	
	/* Fill output parameter - number of actually read bytes */
	*readBytes = maxBytes;

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}
 
/*-------------------------------------------------------------------------------
 * BTL_SPP_WriteSync()
 *
 *     	Synchronously writes data into previously opened SPP port.
 */
BtStatus BTL_SPP_WriteSync(BtlSppPortContext *portContext,
					       const U8 *buffer,
					       U16 nBytes,
					       U16 *writtenBytes)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK("BTL_SPP_WriteSync");

	*writtenBytes = 0;

	BTL_VERIFY_ERR((0 != portContext),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_SPP: Null portContext"));

	BTL_VERIFY_ERR((0 != buffer), BT_STATUS_INVALID_PARM, ("BTL_SPP: Null buffer"));

	BTL_VERIFY_ERR((0 != nBytes), BT_STATUS_INVALID_PARM, ("BTL_SPP: Null nBytes"));

	BTL_VERIFY_ERR((portContext->state == BTL_SPP_PORT_STATE_CONNECTED),
					BT_STATUS_INVALID_PARM,
				   	("BTL_SPP: Port not connected"));

	/* Write required amount of bytes into SPP port */
	status = SPP_Write(&portContext->sppDev, buffer, &nBytes);

	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status),
				   status,
				   ("BTL_SPP: SPP_Write() failed"));
	
	/* Fill output parameter - number of written bytes */
	*writtenBytes = nBytes;

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

/*-------------------------------------------------------------------------------
 * BTL_SPP_WriteAsync()
 *
 *     	Asynchronously writes data into previously opened SPP port.
 */
BtStatus BTL_SPP_WriteAsync(BtlSppPortContext *portContext,
					        const U8 *buffer,
					        U16 nBytes)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK("BTL_SPP_WriteAsync");

	BTL_VERIFY_ERR((0 != portContext),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_SPP: Null portContext"));

	BTL_VERIFY_ERR((0 != buffer), BT_STATUS_INVALID_PARM, ("BTL_SPP: Null buffer"));

	BTL_VERIFY_ERR((0 != nBytes), BT_STATUS_INVALID_PARM, ("BTL_SPP: Null nBytes"));

	BTL_VERIFY_ERR((portContext->state == BTL_SPP_PORT_STATE_CONNECTED),
					BT_STATUS_INVALID_PARM,
				   	("BTL_SPP: Port not connected"));

	/* Write required amount of bytes into SPP port */
	status = SPP_Write(&portContext->sppDev, buffer, &nBytes);

	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status),
				   status,
				   ("BTL_SPP: SPP_Write() failed"));
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

/*-------------------------------------------------------------------------------
 * BTL_SPP_GetRxAsyncDataSize()
 *
 *		Gets a size of data received in port's read buffer.
 */
BtStatus BTL_SPP_GetRxAsyncDataSize(BtlSppPortContext *portContext,
								    U16 *dataReadSize)
{
	BtStatus status = BT_STATUS_SUCCESS;
	SppBuf *sppBuf = portContext->sppDev.sppBuf;
	
	BTL_FUNC_START_AND_LOCK("BTL_SPP_GetDataReadSize");

	BTL_VERIFY_ERR((0 != portContext),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_SPP: Null portContext"));

	BTL_VERIFY_ERR((portContext->sppDev.dataPathTypes & SPP_PORT_DATA_PATH_RX_ASYNC),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_SPP: Function is used only with Rx Async data path"));

	*dataReadSize = SPPBUF_RxBytes(sppBuf);

	BTL_FUNC_END_AND_UNLOCK();

	return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 * BTL_SPP_GetTxSyncDataSize()
 *
 *		Gets a size of data which is still in port's write buffer.
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
								   U16 *dataWriteSize)
{
	BtStatus status = BT_STATUS_SUCCESS;
	SppBuf *sppBuf = portContext->sppDev.sppBuf;
	
	BTL_FUNC_START_AND_LOCK("BTL_SPP_GetDataWriteSize");

	BTL_VERIFY_ERR((0 != portContext),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_SPP: Null portContext"));

	BTL_VERIFY_ERR((portContext->sppDev.dataPathTypes & SPP_PORT_DATA_PATH_TX_SYNC),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_SPP: Function is used only with Tx Sync data path"));

	*dataWriteSize = SPPBUF_TxBytes(sppBuf);

	BTL_FUNC_END_AND_UNLOCK();

	return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 * BTL_SPP_GetFreeRxAsyncSize()
 *
 *		Gets a size of free space in port's read buffer.
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
								    U16 *freeReadSize)
{
	BtStatus status = BT_STATUS_SUCCESS;
	SppBuf *sppBuf = portContext->sppDev.sppBuf;
	
	BTL_FUNC_START_AND_LOCK("BTL_SPP_GetDataWriteSize");

	BTL_VERIFY_ERR((0 != portContext),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_SPP: Null portContext"));

	BTL_VERIFY_ERR((portContext->sppDev.dataPathTypes & SPP_PORT_DATA_PATH_RX_ASYNC),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_SPP: Function is used only with Rx Async data path"));

    *freeReadSize = SPPBUF_RxFree(sppBuf);

	BTL_FUNC_END_AND_UNLOCK();

	return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 * BTL_SPP_GetFreeTxSyncSize()
 *
 *		Gets a size of free space in port's write buffer.
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
								   U16 *freeWriteSize)
{
	BtStatus status = BT_STATUS_SUCCESS;
	SppBuf *sppBuf = portContext->sppDev.sppBuf;
	
	BTL_FUNC_START_AND_LOCK("BTL_SPP_GetDataWriteSize");

	BTL_VERIFY_ERR((0 != portContext),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_SPP: Null portContext"));

	BTL_VERIFY_ERR((portContext->sppDev.dataPathTypes & SPP_PORT_DATA_PATH_TX_SYNC),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_SPP: Function is used only with Tx Sync data path"));

	*freeWriteSize = SPPBUF_TxFree(sppBuf);

	BTL_LOG_DEBUG(("BTL_SPP: free space %d",*freeWriteSize));

	BTL_FUNC_END_AND_UNLOCK();

	return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 * BTL_SPP_Flush()
 *
 *		Flushes required SPP port buffers.
 *
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
					   SppFlushType flushType)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK("BTL_SPP_Flush");

	BTL_VERIFY_ERR((0 != portContext),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_SPP: Null portContext"));
    
    status = SPP_Flush(&portContext->sppDev, flushType);

	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status),
				   status,
				   ("BTL_SPP: SPP_Flush() failed"));
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

#if BT_SECURITY == XA_ENABLED
/*-------------------------------------------------------------------------------
 * BTL_SPP_SetSecurityLevel()
 *
 *     	Sets security level for previously created SPP server port.
 */
BtStatus BTL_SPP_SetSecurityLevel(BtlSppPortContext *portContext,
								  const BtSecurityLevel *securityLevel)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_SPP_SetSecurityLevel");

	BTL_VERIFY_ERR((0 != portContext),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_SPP: Null portContext"));

	if (0 != securityLevel)
	{
		status = BTL_VerifySecurityLevel(*securityLevel);
		
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status),
					   status,
					   ("BTL_SPP: Invalid SPP port's securityLevel"));
	}
	
	/* Partially fill the security record with given security level or with its
	 * default value and with protocol ID */
	if (securityLevel == 0)
	{
		portContext->sppDev.secRec.level =
                                (BtSecurityLevel)SPP_SECURITY_SETTINGS_DEFAULT;
	}
	else
	{
		portContext->sppDev.secRec.level = (BtSecurityLevel)*securityLevel;
	}
	
	portContext->sppDev.secRec.id = SEC_RFCOMM_ID;

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

/*-------------------------------------------------------------------------------
 * BTL_SPP_GetSecurityLevel()
 *
 *     	Gets security level for previously created SPP server port.
 */
BtStatus BTL_SPP_GetSecurityLevel(BtlSppPortContext *portContext,
								  BtSecurityLevel *securityLevel)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_SPP_GetSecurityLevel");

	BTL_VERIFY_ERR((0 != portContext),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_SPP: Null portContext"));

	BTL_VERIFY_ERR((0 != securityLevel),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_SPP: Null securityLevel"));

	*securityLevel = portContext->sppDev.secRec.level;
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}
#endif	/* BT_SECURITY == XA_ENABLED */

/*-------------------------------------------------------------------------------
 * BTL_SPP_SetComSettings()
 *
 *		Sets either SPP client or SPP server port's communication settings.
 *		An application should assure that the communication settings are
 *		correctly set prior sending data.
 */
BtStatus BTL_SPP_SetComSettings(BtlSppPortContext *portContext,
								const SppComSettings *comSettings)
{
	BtStatus status = BT_STATUS_SUCCESS;
    	
	BTL_FUNC_START_AND_LOCK("BTL_SPP_SetComSettings");

	BTL_VERIFY_ERR((0 != portContext),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_SPP: Null portContext"));
	
	/* Save communication settings, if they are passed */
	if (0 != comSettings)
	{
	    /* Write new communication settings into the port */
	    status = SPP_SetComSettings(&portContext->sppDev, comSettings);
	}
	else
	{
		/* Use default communication settings */
	    status = SPP_SetComSettings(&portContext->sppDev,
                                    &btlSppPortComSettingsDefault);
	}

	BTL_VERIFY_ERR((BT_STATUS_FAILED != status),
				   status,
				   ("BTL_SPP: Fail setting port's communication settings"));
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

/*-------------------------------------------------------------------------------
 * BTL_SPP_GetComSettings()
 *
 *		Gets either SPP client or SPP server port's properties.
 */
BtStatus BTL_SPP_GetComSettings(BtlSppPortContext *portContext,
								SppComSettings *comSettings)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_SPP_GetComSettings");

	BTL_VERIFY_ERR((0 != portContext),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_SPP: Null portContext"));

	BTL_VERIFY_ERR((0 != comSettings),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_SPP: Null comSettings"));
	
	/* Read new communication settings */
	status = SPP_GetComSettings(&portContext->sppDev, comSettings);

	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status),
				   status,
				   ("BTL_SPP: Fail getting port's communication settings"));

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

/*-------------------------------------------------------------------------------
 * BTL_SPP_SetControlSignals()
 *
 *		Sets either SPP client or SPP server port's control signals.
 */
BtStatus BTL_SPP_SetControlSignals(BtlSppPortContext *portContext,
								   const SppControlSignals *controlSignals)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK("BTL_SPP_SetComSettings");

	BTL_VERIFY_ERR((0 != portContext),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_SPP: Null portContext"));
	
	BTL_VERIFY_ERR((0 != controlSignals),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_SPP: Null controlSignals"));

	/* Save control signals */
	portContext->sppDev.lModemStatus = *controlSignals;
	
	/* Write new communication settings into the port */
	status = SPP_SetModemControl(&portContext->sppDev, controlSignals);

	BTL_VERIFY_ERR((BT_STATUS_FAILED != status),
				   status,
				   ("BTL_SPP: Fail setting port's control signals"));
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

/*-------------------------------------------------------------------------------
 * BTL_SPP_GetControlSignals()
 *
 *		Gets either SPP client or SPP server port's control signals.
 */
BtStatus BTL_SPP_GetControlSignals(BtlSppPortContext *portContext,
								   SppControlSignals *controlSignals)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK("BTL_SPP_GetComSettings");

	BTL_VERIFY_ERR((0 != portContext),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_SPP: Null portContext"));
    
	BTL_VERIFY_ERR((0 != controlSignals),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_SPP: Null controlSignals"));
	
	/* Read new control signals */
	status = SPP_GetModemControl(&portContext->sppDev, controlSignals);

	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status),
				   status,
				   ("BTL_SPP: Fail getting port's control signals"));

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

/*-------------------------------------------------------------------------------
 * BTL_SPP_SetLineStatus()
 *
 *		Sets either SPP client or SPP server port's control signals.
 */
BtStatus BTL_SPP_SetLineStatus(BtlSppPortContext *portContext,
							   SppLineStatus lineStatus)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK("BTL_SPP_SetLineStatus");

	BTL_VERIFY_ERR((0 != portContext),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_SPP: Null portContext"));
	
	/* Sends line status to the peer device */
	status = SPP_SetLineStatus(&portContext->sppDev, lineStatus);

	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status),
				   status,
				   ("BTL_SPP: Fail sending line status"));

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}
/*-------------------------------------------------------------------------------
 * BTL_SPP_SetServiceName()
 *
 *		Sets either SPP client or SPP server port's service name.
 */
BtStatus BTL_SPP_SetServiceName(BtlSppPortContext *portContext,
							   const BtlUtf8 *serviceName)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U16 len = 0;
	BTL_FUNC_START_AND_LOCK("BTL_SPP_SetServiceName");

	BTL_VERIFY_ERR((0 != portContext),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_SPP: Null portContext"));
	
	
	if ((serviceName != 0) && ((len = OS_StrLen((const char *)serviceName)) != 0))
		{
			/* Service name plus 1 null-terminated char, plus 2 bytes for SDP_TEXT_8BIT */
			BTL_VERIFY_ERR((len <= (SPP_SERVICE_NAME_MAX_SIZE-3)), BT_STATUS_FAILED, ("SPP service name is too long!"));
		}
	if (len > 0)
	{
		OS_StrCpy((char *)portContext->serviceName, (const char *)serviceName);
	}
	else
	{
		portContext->serviceName[0] = '\0';
	}
	if (SPP_SERVER_PORT == portContext->sppDev.portType)
	{
		if (len > 0)
		{
			/* The first 2 bytes are for SDP_TEXT_8BIT */
			portContext->sppDev.type.sppService.name[0] = DETD_TEXT + DESD_ADD_8BITS;
			portContext->sppDev.type.sppService.name[1] = (U8)(len + 1);	/* Includes '\0' */
			
			OS_MemCopy((portContext->sppDev.type.sppService.name + 2), serviceName, len);
			portContext->sppDev.type.sppService.name[(len + 2)] = '\0';

			portContext->sppDev.type.sppService.sdpRecord.attribs[5].len = (U16)(len + 3);
			portContext->sppDev.type.sppService.sdpRecord.attribs[5].value = (const U8 *)portContext->sppDev.type.sppService.name;
		}
	}
	else
	{
		/* Save service name */
		if (len > 0)
        {
			portContext->sppDev.type.client.serviceName = portContext->serviceName;
        }
		else
        {
        		BTL_VERIFY_ERR(0,
				   BT_STATUS_INVALID_PARM,
				   ("BTL_SPP: Client Service Name was not inserted"));
			/*currently we are do not allow client to connect without spesifying the service name*/	
			/*portContext->sppDev.type.client.serviceName = 0;*/
        }
	}
	

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}
/*-------------------------------------------------------------------------------
 * BTL_SPP_GetServiceName()
 *
 *		Gets either SPP client or SPP server port's service name.
 */
BtStatus BTL_SPP_GetServiceName(BtlSppPortContext *portContext,
								BtlUtf8 *serviceName)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK("BTL_SPP_GetServiceName");
	
	BTL_VERIFY_ERR((0 != portContext),
					   BT_STATUS_INVALID_PARM,
					   ("BTL_SPP: Null portContext"));

	OS_StrCpy((char *)serviceName, (const char *)portContext->serviceName);

	
	BTL_FUNC_END_AND_UNLOCK();
	
	return status;
}

/*-------------------------------------------------------------------------------
 * BtlSppEventsCallback()
 *
 *		Callback function for receiving SPP events.
 */
static void BtlSppEventsCallback(SppDev *dev, SppCallbackParms *sppCbParms)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtlSppPortContext *portContext;
	BtlSppPortEvent event;
	SppCallbackParms parms;
    BOOL processDisablingState = FALSE;

	BTL_FUNC_START("BtlSppEventsCallback");
	
	/* Find context according to given SPP device */
	portContext = ContainingRecord(dev, BtlSppPortContext, sppDev);

	/* Set the context in the event passed to app */
	event.portContext = portContext;

	/* Set the internal event in the event passed to app */
	event.parms = sppCbParms;

	/* Set port's state according to event */
	switch(sppCbParms->event)
	{
		case SPP_EVENT_ACL_CREATION_FAILED:
		case SPP_EVENT_SDP_QUERY_FAILED:
            /* Check whether the port is in DISABLING state */
            if (BTL_SPP_PORT_STATE_DISABLING == portContext->state)
            {
                processDisablingState = TRUE;
            }
            else
            {
			    /* Update port's state */
			    portContext->state = BTL_SPP_PORT_STATE_DISCONNECTED;
            }
			break;

		case SPP_EVENT_OPEN_IND:
            /* If the port is in the DISABLING state, wait it to be connected
             * or disconnected and then continue processing the disabling */
            if (BTL_SPP_PORT_STATE_DISABLING != portContext->state)
            {
			    /* Update port's state */
			    portContext->state = BTL_SPP_PORT_STATE_CONNECT_IND;
            }
			break;

		case SPP_EVENT_OPEN:
            /* If the port is in DISABLING state, start disconnection and wait
             * for CLOSED event to continue disabling */
            if (BTL_SPP_PORT_STATE_DISABLING == portContext->state)
            {
                status = SPP_Disconnect(&portContext->sppDev);

	            /* Update port's state */
                if (BT_STATUS_FAILED == status)
                {
	                BTL_LOG_ERROR(("BTL_SPP: SPP_Disconnect() failed"));
                }
            }
            else
            {
                /* Update port's state */
			    portContext->state = BTL_SPP_PORT_STATE_CONNECTED;
            }
			break;

		case SPP_EVENT_CLOSED:
			/* Continue disabling the port */
			if (BTL_SPP_PORT_STATE_DISABLING == portContext->state)
			{
				processDisablingState = TRUE;
			}
			else
			{
				/* Update port's state */
				portContext->state = BTL_SPP_PORT_STATE_DISCONNECTED;
			}
			break;

		default:
			break;
	}

	/* Pass the event to application */
	portContext->callback(&event);

    if (TRUE == processDisablingState)
    {

        status = SPP_Disable(&portContext->sppDev);

	    if (BT_STATUS_SUCCESS == status)
	    {
		    /* Closing of the port successfully finished  - release its resources */
		    status = SPP_DeinitDevice(&portContext->sppDev);

		    BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status),
					       status,
					       ("BTL_SPP: SPP_DeinitDevice() failed"));
		    
		    /* Update port's state */
		    portContext->state = BTL_SPP_PORT_STATE_IDLE;

       	    /* Send DISABLED event to app */
	        event.portContext = portContext;
            event.parms = &parms;
            parms.event = SPP_EVENT_DISABLED;
	        portContext->callback(&event);
	    }
    }
    
    if (TRUE == btlSppData.isRadioOffNotified)
    {
        /* Check whether the last connection was closed. If not, disconnect
         * all connections, stop all connections in establishment and any
         * other process in progress in all contexts and send
         * BTL_MODULE_NOTIFICATION_COMPLETION_EVENT_TYPE_RADIO_OFF, if all
         * connections are closed */
        status = BtlSppProcessRadioOffNotification();

        if (BT_STATUS_SUCCESS == status)
        {
            BtlModuleNotificationCompletionEvent moduleCompletionEvent;
    
            moduleCompletionEvent.type =
                    BTL_MODULE_NOTIFICATION_COMPLETION_EVENT_TYPE_RADIO_OFF;
            moduleCompletionEvent.status = BT_STATUS_SUCCESS;
    
            BTL_ModuleCompleted(BTL_MODULE_TYPE_SPP, &moduleCompletionEvent);

            btlSppData.isRadioOffNotified = FALSE;
        }
    }

	BTL_FUNC_END();
}
							   
/*-------------------------------------------------------------------------------
 * BtlSppGetUserType()
 *
 *		Returns SppUserType of the SPP port used in SPP profile.
 *		In case of bad parameters, returns BT_STATUS_NOT_FOUND.
 */
BtStatus BtlSppGetUserType(const BtlSppPortSettings *portSettings,
                           SppUserType *sppUserType)
{
	BtStatus status = BT_STATUS_SUCCESS;

	switch(portSettings->portType)
	{
		case SPP_SERVER_PORT:
			switch (portSettings->serviceType)
			{
				case BTL_SPP_SERVICE_TYPE_DUN:
					*sppUserType = SPP_USER_TYPE_DIALUP_SERVER;
					break;

				case BTL_SPP_SERVICE_TYPE_FAX:
					*sppUserType = SPP_USER_TYPE_FAX_SERVER;
					break;

				case BTL_SPP_SERVICE_TYPE_SPP:
					*sppUserType = SPP_USER_TYPE_SERIAL_SERVER;
					break;

				default:
					status = BT_STATUS_NOT_SUPPORTED;
					break;
			}
			break;

		case SPP_CLIENT_PORT:
			switch (portSettings->serviceType)
			{
				case BTL_SPP_SERVICE_TYPE_DUN:
					*sppUserType = SPP_USER_TYPE_DIALUP_CLIENT;
					break;

				case BTL_SPP_SERVICE_TYPE_FAX:
					*sppUserType = SPP_USER_TYPE_FAX_CLIENT;
					break;

				case BTL_SPP_SERVICE_TYPE_SPP:
					*sppUserType = SPP_USER_TYPE_SERIAL_CLIENT;
					break;

				default:
					status = BT_STATUS_NOT_SUPPORTED;
					break;
			}
			break;

		default:
			status = BT_STATUS_NOT_SUPPORTED;
			break;
	}

	return status;
}

	
/*-------------------------------------------------------------------------------
 * BtlSppProcessRadioOffNotification()
 *
 *		Checks all ports and disconnects them, if any connection procedures are
 *      active.
 */
static BtStatus BtlSppProcessRadioOffNotification()
{
	BtStatus status = BT_STATUS_SUCCESS;
    BtlSppPortContext *portContext;
	BtlContext *base;
    BOOL radioOffResponsePending = FALSE;

    IterateList(btlSppData.contextsList, base, BtlContext *)
    {
        portContext = (BtlSppPortContext *)base;
        status = BT_STATUS_SUCCESS;

        switch(portContext->state)
        {
            case BTL_SPP_PORT_STATE_CONNECT_IND:
            case BTL_SPP_PORT_STATE_DISCONNECTING:
            case BTL_SPP_PORT_STATE_DISABLING:
                /* Wait to finish current process and then continue */
                status = BT_STATUS_PENDING;
                break;

            case BTL_SPP_PORT_STATE_CONNECTING:
            case BTL_SPP_PORT_STATE_CONNECTED:
                /* Disconnect the connection to peer serial port */
                status = SPP_Disconnect(&portContext->sppDev);

                /* Update port's state */
                if (BT_STATUS_SUCCESS == status)
                {
                    portContext->state = BTL_SPP_PORT_STATE_DISCONNECTED;
                }
                else if (BT_STATUS_FAILED == status)
                {
                    BTL_LOG_ERROR(("BTL_SPP: SPP_Disconnect() failed"));
                }
                else
                {
                    portContext->state = BTL_SPP_PORT_STATE_DISCONNECTING;
                }
                break;

            default:
                /* Nothing to do with port in IDLE or DISCONNECTED
                 * states */
                break;
        }

        if (status != BT_STATUS_SUCCESS)
        {
            radioOffResponsePending = TRUE;
        }
    }

    /* Check whether RADIO OFF response could be sent */
    if (TRUE == radioOffResponsePending)
    {
        status = BT_STATUS_PENDING;
    }

    return (status);
}

BtStatus BtlSppBtlNotificationsCb(BtlModuleNotificationType notificationType)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BtlSppBtlNotificationsCb");
	
	BTL_LOG_DEBUG(("BTL_SPP: Notification Type: %d", notificationType));
	
	switch (notificationType)
	{
		case BTL_MODULE_NOTIFICATION_RADIO_ON:
			/* Do nothing */
			break;

		case BTL_MODULE_NOTIFICATION_RADIO_OFF:

            /* Set Radio Off Notification flag */
            btlSppData.isRadioOffNotified = TRUE;
                
			/* Disconnect all connections, stop all connections in establishment
             * and any other process in progress in all contexts and send
             * BTL_MODULE_NOTIFICATION_COMPLETION_EVENT_TYPE_RADIO_OFF, if all
             * connections are closed */
            status = BtlSppProcessRadioOffNotification();
			break;

		default:
			BTL_ERR(BT_STATUS_INTERNAL_ERROR,
                    ("BTL_SPP: Invalid notification (%d)",
                    notificationType));

	};

	BTL_FUNC_END();
	
	return (status);
}


#else /*BTL_CONFIG_SPP ==   BTL_CONFIG_ENABLED*/

/*-------------------------------------------------------------------------------
 * BTL_SPP_Init() - When  BTL_CONFIG_SPP is disabled.
 */
BtStatus BTL_SPP_Init(void)
{
    
   BTL_LOG_INFO(("BTL_SPP_Init()  -  BTL_CONFIG_SPP Disabled"));
  

    return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 * BTL_SPP_Deinit() - When  BTL_CONFIG_SPP is disabled.
 */
BtStatus BTL_SPP_Deinit(void)
{
    BTL_LOG_INFO(("BTL_SPP_Deinit() -  BTL_CONFIG_SPP Disabled"));

    return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------*/

#endif /*BTL_CONFIG_SPP ==   BTL_CONFIG_ENABLED*/




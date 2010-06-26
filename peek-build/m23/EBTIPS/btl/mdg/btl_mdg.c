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
*   FILE NAME:      btl_mdg.c
*
*   DESCRIPTION:    Implementation of BTL Modem Data Gateway component.
*					This component implements functionality of the Bluetooth
*						Data Gateway for DUN or FAX profiles.
*					It is assumed that only one of these profiles may use modem
*						data services at a time.
*					Processing of events and data from both the SPP port and
*						from the modem data service should be done in separate
*						context, hence, it is a responsibility of callback
*						functions to deliver events from the SPP and from the
*						modem to BTL MDG context.
*
*   AUTHOR:         V. Abram
*
\*******************************************************************************/


#include "btl_config.h"
#include "btl_defs.h"

BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_MDG);

#if BTL_CONFIG_MDG == BTL_CONFIG_ENABLED

/********************************************************************************/


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "osapi.h"
#include "debug.h"
#include "spp.h"
#include "btl_log_modules.h"
#include "btl_pool.h"
#include "btl_spp.h"
#include "btl_mdg.h"
#include "bthal_types.h"
#include "bthal_common.h"
#include "bthal_config.h"
#include "bthal_os.h"
#include "bthal_md.h"


/********************************************************************************
 *
 * Constants
 *
 *******************************************************************************/

/* Size of a pool for receiving events from the SPP and MD */
#define BTL_MDG_EVENTS_POOL_SIZE            (80)

/* Definition of the MDG state machine's execution context */
#define BTL_MDG_EXECUTION_CONTEXT           (BTHAL_OS_TASK_HANDLE_STACK)

/********************************************************************************
 *
 * Internal Types
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BtlMdgState type
 *
 *     Defines the BTL MDG states.
 */
typedef U8 BtlMdgState;

#define BTL_MDG_STATE_NOT_INTIALIZED		(0x00)
#define BTL_MDG_STATE_INITIALIZED			(0x01)

/*-------------------------------------------------------------------------------
 * BtlMdgServerState type
 *
 *     Defines the BTL MDG server states.
 */
typedef U8 BtlMdgServerState;

#define BTL_MDG_SERVER_STATE_IDLE			    (0x00)
#define BTL_MDG_SERVER_STATE_ENABLED		    (0x01)
#define BTL_MDG_SERVER_STATE_DISABLING		    (0x02)
#define BTL_MDG_SERVER_STATE_DISCONNECTED	    (0x03)
#define BTL_MDG_SERVER_STATE_DT_CONNECTED	    (0x04)
#define BTL_MDG_SERVER_STATE_MODEM_CONNECTED    (0x05)
#define BTL_MDG_SERVER_STATE_CONNECTED		    (0x06)

/*-------------------------------------------------------------------------------
 * BtlMdgEventSource type
 *
 *     Defines the source of event sent to the MDG state machine.
 */
typedef U8 BtlMdgEventSource;

#define BTL_MDG_EVENT_SOURCE_SPP    		(0x01)
#define BTL_MDG_EVENT_SOURCE_MD 			(0x02)


/********************************************************************************
 *
 * Internal Data Structures
 *
 *******************************************************************************/
 
/*-------------------------------------------------------------------------------
 * BtlMdgServerContext structure
 *
 *		Represents BTL Modem Data Gateway server context. A pointer to this
 *		structure is returned by the BTL, when the MDG server is created. This
 *		pointer	is used by a caller of BTL MDG API as the MDG server's handle.
 */
struct _BtlMdgServerContext
{
	/* Base BTL context which includes handle of application  using SPP port */
	BtlContext				base;

	/* Port's state */
	BtlMdgServerState		state;

	/* Context of SPP port used for the server */
	BtlSppPortContext		*sppPortContext;

	/* Callback for receiving all BTL MDG events */
	BtlMdgServerCallBack    callback;

	/* Security level of the server */
	BtSecurityLevel			securityLevel;

    /* Settings of the server's SPP port */
    BtlSppPortSettings      portSettings;

	/* Info about connected device: BD ADDR, max frame size */
    SppPortOpenInfo         openInfo;

	/* --- Download path --- */

	/* Size of data to be read from modem */
	BTHAL_INT		        downloadReadSize;

	/* Size of data was not written to BT because of missing BT buffers */
	BTHAL_INT  				downloadWriteSize;

#if BTHAL_MD_DOWNLOAD_BUF_OWNER_MODEM == BTL_CONFIG_ENABLED

	/* Pointer to buffer, its length, and offset to the last partially written
	 * to BT data */
	U8				        *mdDownloadBuf;
	BTHAL_INT		        mdDownloadBufDataOffset;

#else

	/*  Buffer for downloaded data */
	U8    				    downloadBuf[BTL_CONFIG_MDG_DOWNLOAD_BUF_SIZE];

#endif /* BTHAL_MD_DOWNLOAD_BUF_OWNER_MODEM == BTL_CONFIG_ENABLED */
	
	/* --- Upload path --- */

	/* Size of data to be read from BT */
	BTHAL_INT  				uploadReadSize;

#if BTHAL_MD_UPLOAD_BUF_OWNER_MODEM == BTL_CONFIG_DISABLED

	/* Pointer to buffer, its length, and offset to the last partially written
	 * to modem data */
	U8				        uploadBuf[BTL_MDG_UPLOAD_BUF_SIZE];
	BTHAL_INT  				uploadBufDataLen;
	BTHAL_INT  				uploadBufDataOffset;

	/* Size of data to be written to modem */
	BTHAL_INT  				uploadWriteSize;

#else


#endif /* BTHAL_MD_UPLOAD_BUF_OWNER_MODEM == BTL_CONFIG_DISABLED */
	 BTHAL_BOOL btControlSignalsChanged;
} ;

/*-------------------------------------------------------------------------------
 * BtlMdgSppEvent structure
 *
 *	   Represents SPP event used by the MDG state machine.
 */
typedef struct _BtlMdgSppEvent
{
	/* MDG server's context */
	BtlMdgServerContext	*serverContext;

	/* Parameters get with BTL SPP callback */
	SppCallbackParms	parms;

} BtlMdgSppEvent;

/*-------------------------------------------------------------------------------
 * BtlMdgEvent structure
 *
 *	   Represents event used by the MDG state machine.
 */
typedef struct _BtlMdgEvent
{
    /* Info for managing list of these structures */
    ListEntry           node;

	/* Event's source */
	BtlMdgEventSource	source;

	/* Events from SPP and MD */
	union
    {
        BtlMdgSppEvent  sppEvent;
        BthalMdEvent    mdEvent;
    }p;

} BtlMdgEvent;

/*-------------------------------------------------------------------------------
 * BtlMdgData structure
 *
 *	   Includes internal data required for BTL MDG functioning.
 */
typedef struct _BtlMdgData
{
	/* Pool of MDG servers contexts */
	BTL_POOL_DECLARE_POOL(contextsPool,
                          serversContextsMemory,
                          BTL_CONFIG_MDG_MAX_NUM_OF_CONTEXTS,
                          sizeof(BtlMdgServerContext));

	/* List of active MDG servers contexts */
	ListEntry	            contextsList;
    
    /* Pool for MDG events to be sent from the SPP and MD to the MDG */
	BTL_POOL_DECLARE_POOL(eventsPool,
                          eventsMemory,
                          BTL_MDG_EVENTS_POOL_SIZE,
                          sizeof(BtlMdgEvent));
    
    /* List of events received by the MDG */
    ListEntry               eventsList;

    /* Semaphore for accessing events pool and list */
    BthalOsSemaphoreHandle  eventsSemaphore;

    /* Indicates whether RADIO OFF notification was received */
    BOOL        isRadioOffNotified;

} BtlMdgData;


/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * btlMdgServersContextsPoolName
 *
 *     Represents the name of the MDG servers contexts pool.
 */
static const char btlMdgServersContextsPoolName[] = "MdgServersContexts";

/*-------------------------------------------------------------------------------
 * btlMdgEventsPoolName
 *
 *     Represents the name of the MDG servers contexts pool.
 */
static const char btlMdgEventsPoolName[] = "MdgEvents";

/*-------------------------------------------------------------------------------
 * btlMdgState
 *
 *     Represents the current state of the BTL MDG module.
 */
static BtlMdgState btlMdgState = BTL_MDG_STATE_NOT_INTIALIZED;

/*-------------------------------------------------------------------------------
 * btlMdgData
 *
 *     Represents the internal data of the BTL MDG module.
 */
static BtlMdgData btlMdgData;

/********************************************************************************
 *
 * Function prototypes
 *
 *******************************************************************************/

/* This function should be called from the main loop of the context in which
 * the MDG state machine is processed */
void BTL_MDG_ProcessEvents(void);

/********************************************************************************
 *
 * Internal function prototypes
 *
 *******************************************************************************/

static void BtlMdgProcessBtEvent(BtlMdgEvent *mdgEvent);
static void BtlMdgProcessModemDataEvent(BtlMdgEvent *mdgEvent);
static void BtlMdgSendEvent(BtlMdgEvent *mdgEvent, BtlMdgEventSource eventSource);
static void BtlMdgSppEventsCallback(const BtlSppPortEvent *event);
static void BtlMdgBthalMdEventsCallback(const BthalMdEvent *mdEvent);
static void BtlMdgUpload(BtlMdgServerContext *serverContext);
static void BtlMdgDownload(BtlMdgServerContext *serverContext);
static BtlMdgServerContext *BtlMdgFindActiveServer(void);
static BtStatus BtlMdgProcessRadioOffNotification(void);
static BtStatus BtlMdgBtlNotificationsCb(BtlModuleNotificationType notificationType);
const char *BtlMdgBthalMdEventName(BthalMdEventType event);

/********************************************************************************
 *
 * Function definitions
 *
 *******************************************************************************/
 
/*-------------------------------------------------------------------------------
 * BTL_MDG_Init()
 *
 *		Initializes BTL MDG module and allocates required resources.
 */
BtStatus BTL_MDG_Init()
{
	BtStatus status = BT_STATUS_SUCCESS;
    BthalStatus bthalStatus;

	BTL_FUNC_START_AND_LOCK_COMMON("BTL_MDG_Init");

	BTL_VERIFY_ERR((BTL_MDG_STATE_NOT_INTIALIZED == btlMdgState),
				   BT_STATUS_FAILED,
				   ("BTL_MDG: BTL_MDG_Init() was already called"));
	
	/* Create pool for MDG server contexts */
	status = BTL_POOL_Create(&btlMdgData.contextsPool,
							 btlMdgServersContextsPoolName,
							 btlMdgData.serversContextsMemory, 
							 BTL_CONFIG_MDG_MAX_NUM_OF_CONTEXTS,
							 sizeof(BtlMdgServerContext));

	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status),
					 BT_STATUS_INTERNAL_ERROR,
					 ("BTL_MDG: MDG servers contexts pool creation failed"));
	
	InitializeListHead(&btlMdgData.contextsList);

	/* Create buffers pool for events to be sent from the SPP and MD */
	status = BTL_POOL_Create(&btlMdgData.eventsPool,
							 btlMdgEventsPoolName,
							 btlMdgData.eventsMemory, 
							 BTL_MDG_EVENTS_POOL_SIZE,
							 sizeof(BtlMdgEvent));

	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status),
					 BT_STATUS_INTERNAL_ERROR,
					 ("BTL_MDG: MDG events pool creation failed"));
	
	InitializeListHead(&btlMdgData.eventsList);

	/* Create semaphore for accessing events pool and list */
    bthalStatus = BTHAL_OS_CreateSemaphore("BTL_MDG_sem", &btlMdgData.eventsSemaphore); 
    
    BTL_VERIFY_FATAL((BTHAL_STATUS_SUCCESS == bthalStatus),
				     BT_STATUS_FAILED,
				     ("BTL_MDG: BTHAL_OS_CreateSemaphore() status: %s",
                     BTHAL_StatusName(bthalStatus)));

	BTL_RegisterForCommonNotifications(BTL_MODULE_TYPE_MDG, BtlMdgBtlNotificationsCb);

    btlMdgData.isRadioOffNotified = FALSE;
    btlMdgState = BTL_MDG_STATE_INITIALIZED;
		
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

/*-------------------------------------------------------------------------------
 * BTL_MDG_Deinit()
 *
 *		Deinitializes BTL MDG module and frees allocated resources.
 */
BtStatus BTL_MDG_Deinit()
{
	BtStatus status = BT_STATUS_SUCCESS;
    BthalStatus bthalStatus;

	BTL_FUNC_START_AND_LOCK_COMMON("BTL_MDG_Deinit");

	BTL_VERIFY_ERR((BTL_MDG_STATE_INITIALIZED == btlMdgState),
				   BT_STATUS_FAILED,
				   ("BTL_MDG: BTL_MDG_Deinit() was already called"));
	
	BTL_VERIFY_ERR((IsListEmpty(&btlMdgData.contextsList)), 
				   BT_STATUS_FAILED,
				   ("BTL_MDG: MDG servers contexts are still active"));
	
    /* Destroy semaphore which was used for accessing events pool and list */
    bthalStatus = BTHAL_OS_DestroySemaphore(btlMdgData.eventsSemaphore);

    BTL_VERIFY_FATAL((BTHAL_STATUS_SUCCESS == bthalStatus),
				     BT_STATUS_FAILED,
				     ("BTL_MDG: BTHAL_OS_DestroySemaphore() status: %s",
                     BTHAL_StatusName(bthalStatus)));

    /* Destroy pool which was used for MDG servers' contexts */
	status = BTL_POOL_Destroy(&btlMdgData.contextsPool);

	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status),
					 BT_STATUS_INTERNAL_ERROR,
					 ("BTL_MDG: MDG servers contexts pool destruction failed"));
	
	/* Destroy pool which was used for MDG events */
	status = BTL_POOL_Destroy(&btlMdgData.eventsPool);

	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status),
					 BT_STATUS_INTERNAL_ERROR,
					 ("BTL_MDG: MDG events pool destruction failed"));

    btlMdgState = BTL_MDG_STATE_NOT_INTIALIZED;
		
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

/*-------------------------------------------------------------------------------
 * BTL_MDG_Create()
 *
 *     	Creates a Modem Data Gateway server.
 */
BtStatus BTL_MDG_Create(const BtlAppHandle *appHandle,
						const BtlMdgServerCallBack serverCallback,
						const BtSecurityLevel *securityLevel,
						BtlMdgServerContext **serverContext)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_MDG_Create");

	BTL_VERIFY_ERR((0 != serverCallback),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_MDG: Null serverCallback"));

	BTL_VERIFY_ERR((0 != serverContext),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_MDG: Null serverContext"));

#if BT_SECURITY == XA_ENABLED

	if (0 != securityLevel)
	{
		status = BTL_VerifySecurityLevel(*securityLevel);
		
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status),
					   status,
					   ("BTL_MDG: Invalid MDG server securityLevel"));
	}

#endif	/* BT_SECURITY == XA_ENABLED */
	
	/* Allocate memory for the new server's context */
	status = BTL_POOL_Allocate(&btlMdgData.contextsPool,
                               (void **)serverContext);
	
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status),
				   status,
				   ("BTL_MDG: Failed allocating MDG server's context"));

	status = BTL_HandleModuleInstanceCreation((BtlAppHandle *)appHandle,
											  BTL_MODULE_TYPE_MDG,
											  &(*serverContext)->base);
	if (BT_STATUS_SUCCESS != status)
	{
		/* serverContext must be freed before we exit */
		BtStatus retVal = BTL_POOL_Free(&btlMdgData.contextsPool,
                                        (void **)serverContext);
		
		BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == retVal),
							 ("BTL_MDG: Failed freeing MDG server's context"));

		/* This verify will fail and will handle the exception gracefully */
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status),
					   status,
					   ("BTL_MDG: Failed handling MDG server's creation"));
	}
	
	/* Save the application's callback */
	(*serverContext)->callback = serverCallback;

	/* Insert context into the contexts list */
	InsertTailList(&btlMdgData.contextsList, &((*serverContext)->base.node));

#if BT_SECURITY == XA_ENABLED

	/* Save security level, if it was passed */
	if (securityLevel != 0)
	{
		(*serverContext)->securityLevel = *securityLevel;
	}

#endif	/* BT_SECURITY == XA_ENABLED */

	/* Create context for the SPP port to be used by the server */
	status = BTL_SPP_Create((BtlAppHandle *)appHandle,
				  			BtlMdgSppEventsCallback,
							securityLevel,
							&((*serverContext)->sppPortContext));
	
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status),
				   status,
				   ("BTL_MDG: Failed creating SPP port for the MDG server"));

	/* Init MDG server's state */
	(*serverContext)->state = BTL_MDG_SERVER_STATE_IDLE;

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

/*-------------------------------------------------------------------------------
 * BTL_MDG_Destroy()
 *
 *     	Removes previously created Modem Data Gateway server.
 */
BtStatus BTL_MDG_Destroy(BtlMdgServerContext **serverContext)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BOOL isAllocated = FALSE;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_MDG_Destroy");

	BTL_VERIFY_ERR((0 != serverContext),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_MDG: Null serverContext"));
	
	BTL_VERIFY_ERR((0 != *serverContext),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_MDG: Null *serverContext"));
	
	BTL_VERIFY_ERR((BTL_MDG_SERVER_STATE_IDLE == (*serverContext)->state),
				   BT_STATUS_IN_USE,
				   ("BTL_MDG: MDG server's context is in use"));

	status = BTL_POOL_IsElelementAllocated(&btlMdgData.contextsPool,
										   *serverContext,
										   &isAllocated);
	
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status),
				   status,
				   ("BTL_MDG: Failed locating given MDG server's context"));
	
	BTL_VERIFY_ERR((TRUE == isAllocated),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_MDG: Invalid MDG server's context"));

	/* Destroy context of the BTL SPP port used by the server */
	status = BTL_SPP_Destroy(&((*serverContext)->sppPortContext));
	
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status),
				   status,
				   ("BTL_MDG: Failed destroying BTL SPP context"));

	/* Remove the context from the list of all MDG servers contexts */
	RemoveEntryList(&((*serverContext)->base.node));

	status = BTL_HandleModuleInstanceDestruction(&(*serverContext)->base);
	
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status),
				   status,
				   ("BTL_MDG: Failed handling MDG server destruction"));

	status = BTL_POOL_Free(&btlMdgData.contextsPool, (void **)serverContext);
	
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status),
					 BT_STATUS_INTERNAL_ERROR,
					 ("BTL_MDG: Failed freeing MDG server's context"));

	/* Set the MDG server's context to NULL */
	*serverContext = 0;

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}
       
/*-------------------------------------------------------------------------------
 * BTL_MDG_Enable()
 *
 *     	Enables a Modem Data Gateway server for DUN or FAX.
 */
BtStatus BTL_MDG_Enable(BtlMdgServerContext *serverContext,
						const BtlMdgServerSettings *serverSettings)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BthalStatus mdStatus;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_MDG_Enable");

	BTL_VERIFY_ERR((0 != serverContext),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_MDG: Null serverContext"));

	BTL_VERIFY_ERR((BTL_MDG_SERVER_STATE_IDLE == serverContext->state),
					BT_STATUS_FAILED,
					("BTL_MDG: MDG server is already opened"));

	/* Save the communication settings */
    serverContext->portSettings = serverSettings->portSettings;
    
    /* Open SPP server port, register service record and start listening */
	status = BTL_SPP_Enable(serverContext->sppPortContext,
							serverSettings->serviceName,
							&serverSettings->portSettings);

	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status),
				   status,
				   ("BTL_MDG: Failed enabling SPP port for MDG server"));
	
    /* Register with the BTHAL MD module, if this is the 1st server to be
     * enabled */
	if (NULL == BtlMdgFindActiveServer())
	{
        mdStatus = BTHAL_MD_Register(BtlMdgBthalMdEventsCallback,
                                     (const SppComSettings *)&serverSettings->portSettings.comSettings);

	    BTL_VERIFY_ERR((BTHAL_STATUS_SUCCESS == mdStatus),
				       BT_STATUS_FAILED,
				       ("BTL_MDG: BTHAL_MD_Register() status: %s",
						BTHAL_StatusName(mdStatus)));
	}

    /* Update state of the server */
	serverContext->state = BTL_MDG_SERVER_STATE_ENABLED;

	BTL_FUNC_END_AND_UNLOCK();
	
	return (status); 
}

/*-------------------------------------------------------------------------------
 * BTL_MDG_Disable()
 *
 *     	Disables previously enabled Modem Data Gateway server.
 */
BtStatus BTL_MDG_Disable(BtlMdgServerContext *serverContext)
{
    BtStatus status = BT_STATUS_SUCCESS;
    BthalStatus mdStatus;
    BtlMdgCallbackParms parms;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_MDG_Disable");

	BTL_VERIFY_ERR((0 != serverContext),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_MDG: Null portContext"));
	BTL_VERIFY_ERR((BTL_MDG_SERVER_STATE_IDLE != serverContext->state),
				   BT_STATUS_FAILED,
				   ("BTL_MDG: MDG server is not enabled"));

	/* Close SPP port used by the server */
	status = BTL_SPP_Disable(serverContext->sppPortContext);

	BTL_VERIFY_ERR((BT_STATUS_FAILED != status),
				   status,
				   ("BTL_MDG: Failed disabling SPP port for MDG server"));

	if (BT_STATUS_SUCCESS == status)
	{
		/* Closing of the port successfully finished */
		serverContext->state = BTL_MDG_SERVER_STATE_IDLE;
        
        /* Send DISABLED event to the application */
        parms.event = BTL_MDG_SERVER_EVENT_DISABLED;
        serverContext->callback(&parms);
	}
	else
	{
		/* Continuation of the port disabling will be done upon receiving event
		 * SPP_EVENT_CLOSED */
        serverContext->state = BTL_MDG_SERVER_STATE_DISABLING;
	}

    /* Deregister from the BTHAL MD module, if this is the last server to be
     * disabled */
    if (NULL == BtlMdgFindActiveServer())
    {
        mdStatus = BTHAL_MD_Deregister();

	    BTL_VERIFY_ERR((BTHAL_STATUS_SUCCESS == mdStatus),
				       BT_STATUS_FAILED,
				       ("BTL_MDG: BTHAL_MD_Deregister() status: %s",
						BTHAL_StatusName(mdStatus)));
    }
    
    BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

/*-------------------------------------------------------------------------------
 * BTL_MDG_GetConnectedDevice()
 */
BtStatus BTL_MDG_GetConnectedDevice(BtlMdgServerContext *serverContext, 
									BD_ADDR *bdAddr)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK("BTL_MDG_GetConnectedDevice");

	BTL_VERIFY_ERR((0 != serverContext),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_MDG: Null serverContext"));
	BTL_VERIFY_ERR((0 != bdAddr),
                   BT_STATUS_INVALID_PARM,
                   ("BTL_MDG: Null bdAddr"));
	BTL_VERIFY_ERR((BTL_MDG_SERVER_STATE_CONNECTED == serverContext->state),
				   BT_STATUS_NO_CONNECTION,
				   ("BTL_MDG: Server is not connected"));

	/* Copy BD_ADDR structure */
    *bdAddr = serverContext->openInfo.bdAddr;

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

#if BT_SECURITY == XA_ENABLED
/*-------------------------------------------------------------------------------
 * BTL_MDG_SetSecurityLevel()
 *
 *     	Sets security level for previously created MDG server.
 */
BtStatus BTL_MDG_SetSecurityLevel(BtlMdgServerContext *serverContext,
								  const BtSecurityLevel *securityLevel)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_MDG_SetSecurityLevel");

	BTL_VERIFY_ERR((0 != serverContext),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_MDG: Null serverContext"));

	if (0 != securityLevel)
	{
		status = BTL_VerifySecurityLevel(*securityLevel);
		
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status),
					   status,
					   ("BTL_MDG: Invalid MDG server's securityLevel"));
	}
	
	/* Save security level, if it was passed, and set it in SPP port used by
	 * the server */
	if (securityLevel != 0)
	{
		serverContext->securityLevel = *securityLevel;

		status = BTL_SPP_SetSecurityLevel(serverContext->sppPortContext,
										  securityLevel);
		
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status),
					   status,
					   ("BTL_MDG: Fail setting MDG server's securityLevel"));
	}

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

/*-------------------------------------------------------------------------------
 * BTL_MDG_GetSecurityLevel()
 *
 *     	Gets security level for previously created MDG server.
 */
BtStatus BTL_MDG_GetSecurityLevel(BtlMdgServerContext *serverContext,
								  BtSecurityLevel *securityLevel)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_MDG_GetSecurityLevel");

	BTL_VERIFY_ERR((0 != serverContext),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_MDG: Null serverContext"));
	BTL_VERIFY_ERR((0 != securityLevel),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_MDG: Null securityLevel"));

	*securityLevel = serverContext->securityLevel;
	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}
#endif	/* BT_SECURITY == XA_ENABLED */

/*-------------------------------------------------------------------------------
 * BTL_MDG_SetComSettings()
 *
 *		Sets MDG server's communication settings.
 */
BtStatus BTL_MDG_SetComSettings(BtlMdgServerContext *serverContext,
								const SppComSettings *comSettings)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK("BTL_MDG_SetComSettings");

	BTL_VERIFY_ERR((0 != serverContext),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_MDG: Null serverContext"));
	
	/* Set new server's communication settings */
	if (0 != comSettings)
	{
		status = BTL_SPP_SetComSettings(serverContext->sppPortContext,
										comSettings);

		BTL_VERIFY_ERR((BT_STATUS_FAILED != status),
					   status,
					   ("BTL_MDG: Fail setting server's communication settings"));

		/* Event SPP_EVENT_PORT_STATUS_CNF will be received in confirmation
		 * that new communication settings were acknowledged by the remote
		 * BT device */
	}
	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

/*-------------------------------------------------------------------------------
 * BTL_MDG_GetComSettings()
 *
 *		Gets MDG server's communication settings.
 */
BtStatus BTL_MDG_GetComSettings(BtlMdgServerContext *serverContext,
								SppComSettings *comSettings)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_MDG_GetComSettings");

	BTL_VERIFY_ERR((0 != serverContext),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_MDG: Null serverContext"));
	BTL_VERIFY_ERR((0 != comSettings),
				   BT_STATUS_INVALID_PARM,
				   ("BTL_MDG: Null comSettings"));
	
	/* Read server's communication settings */
	status = BTL_SPP_GetComSettings(serverContext->sppPortContext, comSettings);

	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status),
				   status,
				   ("BTL_MDG: Fail getting server's communication settings"));

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

/*-------------------------------------------------------------------------------
 * BTL_MDG_TerminateDataCall()
 *
 *     	Terminates data call originated via the Modem Data Gateway server.
 */
BtStatus BTL_MDG_TerminateDataCall(BtlMdgServerContext *serverContext)
{
	UNUSED_PARAMETER(serverContext);

    return (BT_STATUS_SUCCESS);
}

/*-------------------------------------------------------------------------------
 * BTL_MDG_ProcessEvents()
 *
 *     	Processes events received from the BT SPP port and from the MD.
 */
void BTL_MDG_ProcessEvents()
{
    BtStatus status;
    BthalStatus bthalStatus;
    BtlMdgEvent *mdgEvent;

   	BTL_FUNC_START("BtlMdgProcessEvents");

    /* Get semaphore for accessing events pool and list */
    bthalStatus = BTHAL_OS_LockSemaphore(btlMdgData.eventsSemaphore, 0);
    
    BTL_VERIFY_ERR_NO_RETVAR((BTHAL_STATUS_SUCCESS == bthalStatus),
                             ("BTL_MDG: BTHAL_OS_LockSemaphore() status: %s",
                             BTHAL_StatusName(bthalStatus)));

    while (!IsListEmpty(&btlMdgData.eventsList))
    {
        /* Get received event */
        mdgEvent = (BtlMdgEvent *)RemoveHeadList(&btlMdgData.eventsList);

        /* Process received event according to its source */
        switch(mdgEvent->source)
        {
            case BTL_MDG_EVENT_SOURCE_SPP:
                BtlMdgProcessBtEvent(mdgEvent);
                break;

            case BTL_MDG_EVENT_SOURCE_MD:
                BtlMdgProcessModemDataEvent(mdgEvent);
                break;
            
            default:
                BTL_LOG_DEBUG(("Unknown source of event %d", mdgEvent->source));
                break;
        }
    
        /* Put the buffer back on the available list */
	    status = BTL_POOL_Free(&btlMdgData.eventsPool, (void **)&mdgEvent);

	    BTL_VERIFY_ERR_NO_RETVAR((BT_STATUS_SUCCESS == status),
				                 ("BTL_MDG: Failed freeing MDG event"));
    }

    /* Release semaphore for accessing events pool and list */
    bthalStatus = BTHAL_OS_UnlockSemaphore(btlMdgData.eventsSemaphore);
    
    BTL_VERIFY_ERR_NO_RETVAR((BTHAL_STATUS_SUCCESS == bthalStatus),
                             ("BTL_MDG: BTHAL_OS_UnlockSemaphore() status: %s",
                             BTHAL_StatusName(bthalStatus)));

	BTL_FUNC_END();
}

/*-------------------------------------------------------------------------------
 * BtlMdgProcessBtEvent()
 *
 *     	Processes event received from the BT SPP port used by the MDG server.
 */
static void BtlMdgProcessBtEvent(BtlMdgEvent *mdgEvent)
{
    BtStatus btStatus;
    BthalStatus mdStatus;
    BtlMdgCallbackParms parms;
    BtlMdgSppEvent *mdgSppEvent = &mdgEvent->p.sppEvent;
    BtlMdgServerContext *serverContext = mdgSppEvent->serverContext;
	
	BTL_FUNC_START("BtlMdgProcessBtEvent");

    /* Fill server's context to be passed in event to application */
    parms.serverContext = serverContext;

    /* Process event */
	switch (mdgSppEvent->parms.event)
	{
		/* BT service level connection is up  - connect to modem data service */ 
		case SPP_EVENT_OPEN:
            /* Reset internal variables for data downloading */
			serverContext->downloadReadSize = 0;
			serverContext->downloadWriteSize = 0;

#if BTHAL_MD_DOWNLOAD_BUF_OWNER_MODEM == BTL_CONFIG_ENABLED

			serverContext->mdDownloadBuf = 0;
			serverContext->mdDownloadBufDataOffset = 0;

#endif /* BTHAL_MD_DOWNLOAD_BUF_OWNER_MODEM == BTL_CONFIG_ENABLED */

            /* Reset internal variables for data uploading */
			serverContext->uploadReadSize = 0;

#if BTHAL_MD_UPLOAD_BUF_OWNER_MODEM == BTL_CONFIG_DISABLED

			serverContext->uploadBufDataLen = 0;
			serverContext->uploadBufDataOffset = 0;
			serverContext->uploadWriteSize = 0;

#endif /* BTHAL_MD_UPLOAD_BUF_OWNER_MODEM == BTL_CONFIG_DISABLED */

            /* Store info about the connected remote device: BD ADDR, max frame
             * size */
            serverContext->openInfo = mdgSppEvent->parms.p.openInfo;

            /* Pass event to the application */
            parms.event = BTL_MDG_SERVER_EVENT_DT_CONNECTED;
            OS_MemCopy(parms.bdAddr.addr,
                       (const U8 *)mdgSppEvent->parms.p.openInfo.bdAddr.addr,
                       BD_ADDR_SIZE);
            serverContext->callback(&parms);
            serverContext->btControlSignalsChanged = FALSE;
            /* Connect data path to the modem */
			mdStatus = BTHAL_MD_Connect();

			if ((BTHAL_STATUS_SUCCESS != mdStatus) &&
				(BTHAL_STATUS_PENDING != mdStatus))
			{
				BTL_LOG_INFO(("BTL_MDG: BTHAL_MD_Connect() status: %s",
							  BTHAL_StatusName(mdStatus)));
			}

            /* If modem is successfully connected, pass event to the
             * application */
            if (BTHAL_STATUS_SUCCESS == mdStatus)
            {
                parms.event = BTL_MDG_SERVER_EVENT_MODEM_CONNECTED;
                serverContext->callback(&parms);
                serverContext->state = BTL_MDG_SERVER_STATE_CONNECTED;
            
            } 
            else
            {
                serverContext->state = BTL_MDG_SERVER_STATE_DT_CONNECTED;
            }
			break;
		
		case SPP_EVENT_CLOSED:
            /* Pass event to the application */
            parms.event = BTL_MDG_SERVER_EVENT_DT_DISCONNECTED;
            serverContext->callback(&parms);

		    /* BT service level connection is down  - disconnect from the
             * modem */
		    mdStatus = BTHAL_MD_Disconnect();

		    if ((BTHAL_STATUS_SUCCESS != mdStatus) &&
			    (BTHAL_STATUS_PENDING != mdStatus) &&
                (BTHAL_STATUS_NO_CONNECTION != mdStatus))
		    {
			    BTL_LOG_INFO(("BTL_MDG: BTHAL_MD_Disconnect() status: %s",
						      BTHAL_StatusName(mdStatus)));
		    }

            /* If modem is successfully disconnected, pass event to the
             * application */
            if (BTHAL_STATUS_SUCCESS == mdStatus)
            {
                parms.event = BTL_MDG_SERVER_EVENT_MODEM_DISCONNECTED;
                serverContext->callback(&parms);
            }

            if ((BTHAL_STATUS_SUCCESS == mdStatus) ||
                (BTHAL_STATUS_NO_CONNECTION == mdStatus))
            {
                /* If the server was in the DISABLING state, send DISABLED
                 * event */
                if (BTL_MDG_SERVER_STATE_DISABLING == serverContext->state)
                {
                    serverContext->state = BTL_MDG_SERVER_STATE_IDLE;

                    parms.event = BTL_MDG_SERVER_EVENT_DISABLED;
                    serverContext->callback(&parms);

                    /* Deregister from the BTHAL MD module, if this is the last
                     * server to be disabled */
                    if (NULL == BtlMdgFindActiveServer())
                    {
                        mdStatus = BTHAL_MD_Deregister();

	                    BTL_VERIFY_ERR_NO_RETVAR((BTHAL_STATUS_SUCCESS == mdStatus),
				                                 ("BTL_MDG: BTHAL_MD_Deregister() status: %s",
						                          BTHAL_StatusName(mdStatus)));
                    }
                }
                else
                {
                    /* Update server's state */
                    serverContext->state = BTL_MDG_SERVER_STATE_DISCONNECTED;
                }

                if (TRUE == btlMdgData.isRadioOffNotified)
                {
                    /* Check whether the last connection was closed. If not, disconnect
                     * all connections, stop all connections in establishment and any
                     * other process in progress in all contexts and send
                     * BTL_MODULE_NOTIFICATION_COMPLETION_EVENT_TYPE_RADIO_OFF, if all
                     * connections are closed */
                    btStatus = BtlMdgProcessRadioOffNotification();

                    if (BT_STATUS_SUCCESS == btStatus)
                    {
                        BtlModuleNotificationCompletionEvent moduleCompletionEvent;
    
                        moduleCompletionEvent.type =
                                BTL_MODULE_NOTIFICATION_COMPLETION_EVENT_TYPE_RADIO_OFF;
                        moduleCompletionEvent.status = BT_STATUS_SUCCESS;
    
                        BTL_ModuleCompleted(BTL_MODULE_TYPE_MDG, &moduleCompletionEvent);

                        btlMdgData.isRadioOffNotified = FALSE;
                    }
                }
            }
            else
            {
                if (BTL_MDG_SERVER_STATE_CONNECTED == serverContext->state)
                {
                    serverContext->state = BTL_MDG_SERVER_STATE_MODEM_CONNECTED;
                }
            }
            break;
		
		case SPP_EVENT_DISABLED:
            /* DISABLED event and update of the server's state were done upon
             * receiving of SPP_EVENT_CLOSED event or will be done upon
             * receiving of BTHAL_MD_EVENT_DISCONNECTED event, if disconnection
             * from the modem will be finished asynchronously */
            break;

        /* Data received from BT */ 
		case SPP_EVENT_RX_DATA_IND:
			/* Update amount of data to be read from BT and to be written to the
			 * modem */
			serverContext->uploadReadSize += mdgSppEvent->parms.p.rxDataInfo.rxDataLen;
			
			if (serverContext->uploadReadSize)
			{
				BtlMdgUpload(serverContext);
			
			}
			break;

		/* Line status is got from BT */
		case SPP_EVENT_LINE_STATUS_IND:
			/* Check received line status for errors */
			if (SPP_LINE_STATUS_NO_ERROR != mdgSppEvent->parms.lineStatus)
			{
				BTL_LOG_INFO(("BTL_MDG: Received line status error: 0x%x",
							  mdgSppEvent->parms.lineStatus));
			}
			break;

		/* Tx buffer is got from BT */
		case SPP_EVENT_TX_DATA_COMPLETE:
			if (serverContext->downloadReadSize || serverContext->downloadWriteSize)
			{
				/* Read data from the modem and write it to the BT SPP port */
				BtlMdgDownload(serverContext);
			}
			break;

		/* Modem status is got from BT */
		case SPP_EVENT_MODEM_STATUS_IND:
			/* Translate control signals to modem rules */
			mdStatus = BTHAL_MD_TranslateBtSignalsToModem(&mdgSppEvent->parms.p.controlSignals,
                                                          BTHAL_FALSE);

			if (BTHAL_STATUS_SUCCESS != mdStatus)
			{
				BTL_LOG_INFO(("BTL_MDG: BTHAL_MD_TranslateBtSignalsToModem() status: %s",
							  BTHAL_StatusName(mdStatus)));
			}
            else
            {
                /* Pass control signals to modem */
				serverContext->btControlSignalsChanged = TRUE;		
				BtlMdgUpload(serverContext);
            }
			break;

        /* The remote device has requested negotiation of port settings and they
         * were automatically accepted by SPP profile layer */
		case SPP_EVENT_PORT_NEG_IND:
        {
            SppComSettings *comSettings = &mdgSppEvent->parms.p.portSettings;

			/* Try to establish  communication settings, if received from peer
             * differ from requested for MDG server */
			if ((comSettings->baudRate != serverContext->portSettings.comSettings.baudRate) ||
                (comSettings->dataFormat != serverContext->portSettings.comSettings.dataFormat) ||
                (comSettings->flowControl != serverContext->portSettings.comSettings.flowControl))
			{
                btStatus = BTL_SPP_SetComSettings(serverContext->sppPortContext,
                                                  &serverContext->portSettings.comSettings);
            
			    BTL_VERIFY_ERR_NO_RETVAR((BT_STATUS_FAILED != btStatus),
						                 ("BTL_SPP_SetComSettings() failed"));
			}
			break;
        }

		default:
			break;
	}

	BTL_FUNC_END();
}

/*-------------------------------------------------------------------------------
 * BtlMdgProcessModemDataEvent()
 *
 *     	Processes event received from the modem data and used by the MDG server.
 */
static void BtlMdgProcessModemDataEvent(BtlMdgEvent *mdgEvent)
{
	BthalStatus mdStatus;
	BtStatus status = BT_STATUS_SUCCESS;
    BtlMdgCallbackParms parms;
    BtlMdgServerContext *serverContext = NULL;
	SppControlSignals btControlSignals;
    BthalMdEvent *mdEvent = &mdgEvent->p.mdEvent;

	
	BTL_FUNC_START("BtlMdgProcessModemDataEvent");

	BTL_LOG_DEBUG(("BTL_MDG: %s", BtlMdgBthalMdEventName(mdEvent->eventType)));

    /* Find active server event to be passed to */
	serverContext = BtlMdgFindActiveServer();
	
	BTL_VERIFY_ERR_NO_RETVAR((NULL != serverContext),
				   			 ("BTL_MDG: Active server not found"));

    /* Fill server's context to be passed in event to application */
    parms.serverContext = serverContext;

    /* Process event */
	switch (mdEvent->eventType)
	{
		/* Modem is connected */ 
		case BTHAL_MD_EVENT_CONNECTED:
            /* Pass event to the application */
            parms.event = BTL_MDG_SERVER_EVENT_MODEM_CONNECTED;
            serverContext->callback(&parms);

            /* Update server's state */
            if (BTL_MDG_SERVER_STATE_DT_CONNECTED == serverContext->state)
            {
                serverContext->state = BTL_MDG_SERVER_STATE_CONNECTED;
            }
            else
            {
                serverContext->state = BTL_MDG_SERVER_STATE_MODEM_CONNECTED;
            }
			break;

		/* Modem is disconnected */ 
		case BTHAL_MD_EVENT_DISCONNECTED:
            /* Pass event to the application */
            parms.event = BTL_MDG_SERVER_EVENT_MODEM_DISCONNECTED;
            serverContext->callback(&parms);
            
            /* If the server was in the DISABLING state, send DISABLED
             * event */
            if (BTL_MDG_SERVER_STATE_DISABLING == serverContext->state)
            {
                serverContext->state = BTL_MDG_SERVER_STATE_IDLE;

                parms.event = BTL_MDG_SERVER_EVENT_DISABLED;
                serverContext->callback(&parms);

                /* Deregister from the BTHAL MD module, if this is the last
                 * server to be disabled */
                if (NULL == BtlMdgFindActiveServer())
                {
                    mdStatus = BTHAL_MD_Deregister();

                    BTL_VERIFY_ERR((BTHAL_STATUS_SUCCESS == mdStatus),
                                   BT_STATUS_FAILED,
                                   ("BTL_MDG: BTHAL_MD_Deregister() status: %s",
	                                BTHAL_StatusName(mdStatus)));
                }
            }
            else
            {
                /* Update server's state */
                serverContext->state = BTL_MDG_SERVER_STATE_DISCONNECTED;
            }

            if (TRUE == btlMdgData.isRadioOffNotified)
            {
                /* Check whether the last connection was closed. If not, disconnect
                 * all connections, stop all connections in establishment and any
                 * other process in progress in all contexts and send
                 * BTL_MODULE_NOTIFICATION_COMPLETION_EVENT_TYPE_RADIO_OFF, if all
                 * connections are closed */
                status = BtlMdgProcessRadioOffNotification();

                if (BT_STATUS_SUCCESS == status)
                {
                    BtlModuleNotificationCompletionEvent moduleCompletionEvent;

                    moduleCompletionEvent.type =
                            BTL_MODULE_NOTIFICATION_COMPLETION_EVENT_TYPE_RADIO_OFF;
                    moduleCompletionEvent.status = BT_STATUS_SUCCESS;

                    BTL_ModuleCompleted(BTL_MODULE_TYPE_MDG, &moduleCompletionEvent);

                    btlMdgData.isRadioOffNotified = FALSE;
                }
            }
            break;

		/* Data received from modem */ 
		case BTHAL_MD_EVENT_DOWNLOAD_DATA:
			/* Update amount of data to be read from modem  and to be written to
			 * the SPP port */
			serverContext->downloadReadSize += mdEvent->downloadDataLen;

			/* Read the data and write it to BT */
			BtlMdgDownload(serverContext);
			break;

		/* New control signals are got from modem */
		case BTHAL_MD_EVENT_CONTROL_SIGNALS:
			/* Get new modem control signals */
			mdStatus = BTHAL_MD_TranslateModemSignalsToBt(&btControlSignals);

			if (BTHAL_STATUS_SUCCESS == mdStatus)
			{
				status = BTL_SPP_SetControlSignals(serverContext->sppPortContext,
												   (const SppControlSignals *)&btControlSignals);

				if (BT_STATUS_SUCCESS != status)
				{
					BTL_LOG_DEBUG(("BTL_MDG: BTL_SPP_SetControlSignals() status: %s",
								   pBT_Status(status)));
				}
			}
			else
			{
				BTL_LOG_DEBUG(("BTL_MDG: BTHAL_MD_TranslateModemSignalsToBt() status: %s",
							   BTHAL_StatusName(mdStatus)));
			}
			break;

#if BTHAL_MD_UPLOAD_BUF_OWNER_MODEM == BTL_CONFIG_ENABLED

		/* Buffer for data to be uploaded is got from modem */
		case BTHAL_MD_EVENT_UPLOAD_BUF:
			if (serverContext->uploadReadSize || serverContext->btControlSignalsChanged)
			{
				/* Read the data from the BT SPP port and write it to the modem  
					or pass the stored modem control signal
				*/
				BtlMdgUpload(serverContext);
			}
			break;

#endif /* BTHAL_MD_DOWNLOAD_BUF_OWNER_MODEM == BTL_CONFIG_ENABLED */

		default:
			BTL_LOG_ERROR(("BTL_MDG: Unknown event from modem: %d",
						   mdEvent->eventType));
			break;
	}

	BTL_FUNC_END();
}

/*-------------------------------------------------------------------------------
 * BtlMdgSendSppEvent()
 *
 *     	Sends event received from the SPP or MD to the MDG state machine.
 */
static void BtlMdgSendEvent(BtlMdgEvent *mdgEvent, BtlMdgEventSource eventSource)
{
    BthalStatus status = BTHAL_STATUS_SUCCESS;

    BTL_FUNC_START("BtlMdgSendEvent");

    /* Fill source of the event */
    mdgEvent->source = eventSource;

    /* Pass event to the MDG state machine */
    InsertTailList(&btlMdgData.eventsList, &mdgEvent->node);

    /* Notify execution context about the event */
	status = BTHAL_OS_SendEvent(BTL_MDG_EXECUTION_CONTEXT,
                                OS_EVENT_STACK_TASK_MDG);

    BTL_VERIFY_ERR_NO_RETVAR((BTHAL_STATUS_SUCCESS == status),
		                     ("BTL_MDG: BthalNotifyContext() status: %s",
				             BTHAL_StatusName(status)));

    BTL_FUNC_END();
}

/*-------------------------------------------------------------------------------
 * BtlMdgSppEventsCallback()
 *
 *     	Processes events received from the BTL SPP.
 */
static void BtlMdgSppEventsCallback(const BtlSppPortEvent *event)
{
    BtlMdgEvent *mdgEvent;
    BtlMdgServerContext *server = NULL;
    BtlContext *base;
    BtStatus status;
    BthalStatus bthalStatus;
    BOOL activeServerFound = FALSE;

	BTL_FUNC_START("BtlMdgSppEventsCallback");
	

	/* Send to the MDG state machine only selected events */
	switch(event->parms->event)
	{
		case SPP_EVENT_OPEN:
		case SPP_EVENT_CLOSED:
		case SPP_EVENT_RX_DATA_IND:
		case SPP_EVENT_LINE_STATUS_IND:
		case SPP_EVENT_TX_DATA_COMPLETE:
		case SPP_EVENT_MODEM_STATUS_IND:
		case SPP_EVENT_PORT_NEG_IND:

            /* Get semaphore for accessing events pool and list */
            bthalStatus = BTHAL_OS_LockSemaphore(btlMdgData.eventsSemaphore, 0);
    
            BTL_VERIFY_ERR_NO_RETVAR((BTHAL_STATUS_SUCCESS == bthalStatus),
                                     ("BTL_MDG: BTHAL_OS_LockSemaphore() status: %s",
                                     BTHAL_StatusName(bthalStatus)));

	        /* Allocate memory for the new event */
	        status = BTL_POOL_Allocate(&btlMdgData.eventsPool,
                                       (void **)&mdgEvent);
	        
	        BTL_VERIFY_ERR_NO_RETVAR((BT_STATUS_SUCCESS == status),
				                     ("BTL_MDG: Failed allocating MDG event"));

	        /* Find server context according to the given SPP port context */
	        IterateList(btlMdgData.contextsList, base, BtlContext *)
	        {
                server = (BtlMdgServerContext *)base;

	            if (server->sppPortContext == event->portContext)
	            {
	                activeServerFound = TRUE;
                    break;
	            }
	        }

	        if (FALSE == activeServerFound)
	        {
                BTL_LOG_ERROR(("BTL_MDG: BTL SPP event contains invalid portContext"));
                break;
	        }
            
	        /* Set the SPP event in the event passed to the MDG state machine */
		    mdgEvent->p.sppEvent.serverContext = server;
            mdgEvent->p.sppEvent.parms = *(event->parms);

            /* Pass event to the MDG state machine */
            BtlMdgSendEvent(mdgEvent, BTL_MDG_EVENT_SOURCE_SPP);

            /* Release semaphore for accessing events pool and list */
            bthalStatus = BTHAL_OS_UnlockSemaphore(btlMdgData.eventsSemaphore);
    
            BTL_VERIFY_ERR_NO_RETVAR((BTHAL_STATUS_SUCCESS == bthalStatus),
                                     ("BTL_MDG: BTHAL_OS_UnlockSemaphore() status: %s",
                                     BTHAL_StatusName(bthalStatus)));
		    break;

		default:
			break;
	}
	
	BTL_FUNC_END();
}

/*-------------------------------------------------------------------------------
 * BtlMdgBthalMdEventsCallback()
 *
 *     	Processes events received from the BTHAL MD.
 */
static void BtlMdgBthalMdEventsCallback(const BthalMdEvent *mdEvent)
{
    BtlMdgEvent *mdgEvent;
    BtStatus status;
    BthalStatus bthalStatus;

	BTL_FUNC_START("BtlMdgBthalMdEventsCallback");
	
    /* Get semaphore for accessing events pool and list */
    bthalStatus = BTHAL_OS_LockSemaphore(btlMdgData.eventsSemaphore, 0);
    
    BTL_VERIFY_ERR_NO_RETVAR((BTHAL_STATUS_SUCCESS == bthalStatus),
                             ("BTL_MDG: BTHAL_OS_LockSemaphore() status: %s",
                             BTHAL_StatusName(bthalStatus)));

    /* Allocate memory for the new event */
    status = BTL_POOL_Allocate(&btlMdgData.eventsPool, (void **)&mdgEvent);

    BTL_VERIFY_ERR_NO_RETVAR((BT_STATUS_SUCCESS == status),
	                         ("BTL_MDG: Failed allocating MDG event"));

    /* Fill event's information */
    mdgEvent->p.mdEvent = *mdEvent;
    
    /* Pass event to the MDG state machine */
	BtlMdgSendEvent(mdgEvent, BTL_MDG_EVENT_SOURCE_MD);

    /* Release semaphore for accessing events pool and list */
    bthalStatus = BTHAL_OS_UnlockSemaphore(btlMdgData.eventsSemaphore);
    
    BTL_VERIFY_ERR_NO_RETVAR((BTHAL_STATUS_SUCCESS == bthalStatus),
                             ("BTL_MDG: BTHAL_OS_UnlockSemaphore() status: %s",
                             BTHAL_StatusName(bthalStatus)));

	BTL_FUNC_END();
}

/*-------------------------------------------------------------------------------
 * BtlMdgUpload()
 *
 *     	Reads data or control signals from BT SPP port used by the MDG server and
 *      writes it to the modem.
 */
static void BtlMdgUpload(BtlMdgServerContext *serverContext)
{
	U8 *data = NULL;
	BTHAL_INT num = 0;
	BTHAL_INT len = 0;
    	BOOL endOfData;
	BthalStatus mdStatus = BTHAL_STATUS_SUCCESS;
	BtStatus btStatus = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BtlMdgUpload");

   
    /* Loop on reading data from BT and writing it to modem */
	while ((serverContext->uploadReadSize  || serverContext->btControlSignalsChanged

#if BTHAL_MD_UPLOAD_BUF_OWNER_MODEM == BTL_CONFIG_DISABLED

		 || serverContext->uploadWriteSize

#endif /* BTHAL_MD_UPLOAD_BUF_OWNER_MODEM == BTL_CONFIG_DISABLED */

	   ) && (BTHAL_STATUS_SUCCESS == mdStatus)
         && (BT_STATUS_SUCCESS == btStatus))
	{

#if BTHAL_MD_UPLOAD_BUF_OWNER_MODEM == BTL_CONFIG_ENABLED

		/* Get size and pointer to buffer to upload data */
		mdStatus = BTHAL_MD_GetUploadBuf(serverContext->uploadReadSize,
                                         (BTHAL_U8 **)&data,
                                         &len);

		if (BTHAL_STATUS_SUCCESS != mdStatus)
		{
			BTL_LOG_DEBUG(("BTL_MDG: BTHAL_MD_GetUploadBuf() status: %s",
						   BTHAL_StatusName(mdStatus)));
			break;
		}

        /* Check whether we get buffer with size more, than needed */
        if (len > serverContext->uploadReadSize)
        {
            len = serverContext->uploadReadSize;
        }

#else /* Buffer is allocated by MDG */
	
		/* Check whether some previously read from BT data has to be written
		 * to the modem */
		if (0 != serverContext->uploadWriteSize)
		{
			data = serverContext->uploadBuf + serverContext->uploadBufDataOffset;
			len = serverContext->uploadWriteSize;
			
			/* Write remained from previous time data to the modem */
			mdStatus = BTHAL_MD_Write((const BTHAL_S8 *)data, len, &num);

			if (BTHAL_STATUS_SUCCESS == mdStatus)
			{
				/* Update amount of data to be written to the modem */
				serverContext->uploadWriteSize -= num;
				
				/* Check whether all data was written */
				if (num == len)
				{
					/* Reset offset value */
					serverContext->uploadBufDataOffset = 0;
				}
				else
				{
					/* Update offset value for future use and exit
					 * loop */
					serverContext->uploadBufDataOffset += num;
					break;
				}
			}
			else
			{
				BTL_LOG_DEBUG(("BTL_MDG: BTHAL_MD_Write() status: %s",
							   BTHAL_StatusName(mdStatus)));
				break;
			}
		}

		/* All previously read data was written to the modem. Thus, set pointer
		 * to buffer and length of data to read from BT */
		data = serverContext->uploadBuf;
		len = serverContext->uploadReadSize;

#endif /* BTHAL_MD_UPLOAD_BUF_OWNER_MODEM == BTL_CONFIG_ENABLED */

		if (len > 0)
		{
                BTL_VERIFY_ERR_NO_RETVAR((serverContext->portSettings.dataPathTypes & SPP_PORT_DATA_PATH_RX_ASYNC),
						   				 ("BTL_MDG: Only Rx Async data path type is implemented in BTL_MDG"));

            /* Read data from the SPP port */
			btStatus = BTL_SPP_ReadAsync(serverContext->sppPortContext,
									     (U8 *)data,
									     (U16)len,
									     (U16 *)&num);
           
            		if (BT_STATUS_SUCCESS == btStatus)
			{
				switch (num)
				{
					case 1:
						BTL_LOG_DEBUG(("BTL_MDG: Read 1 byte from BT - 0x%02x", *data));
						break;
					case 2:
						BTL_LOG_DEBUG(("BTL_MDG: Read 2 bytes from BT - 0x%02x 0x%02x", *data, *(data+1)));
						break;
					case 3:
						BTL_LOG_DEBUG(("BTL_MDG: Read 3 bytes from BT - 0x%02x 0x%02x 0x%02x", *data, *(data+1), *(data+2)));
						break;
					default:
						BTL_LOG_DEBUG(("BTL_MDG: Read %d bytes from BT - 0x%02x 0x%02x 0x%02x 0x%02x", num, *data, *(data+1), *(data+2), *(data+3)));
						break;
				}
				
				if ((len == serverContext->uploadReadSize) && (num < len))
				{
					BTL_LOG_DEBUG(("BTL_MDG: Not all data was read from BT"));
				}
                
                /* Update amount of read from BT bytes */
				if (serverContext->uploadReadSize >= num)
				{
                    serverContext->uploadReadSize -= num;
				}
		                else
		                {
		                    BTL_LOG_FATAL(("BTL_MDG: uploadReadSize %d < read bytes %d in BtlMdgUpload()",
		                                   serverContext->uploadReadSize,
		                                   num));
		                }

#if BTHAL_MD_UPLOAD_BUF_OWNER_MODEM == BTL_CONFIG_ENABLED

				/* Notify modem that buffer for uploading is filled with data,
				 * pass actual length of the data and mark, if required, end of
                 * data */
		                if (!serverContext->uploadReadSize)
		                {
		                    endOfData = BTHAL_TRUE;
		                }
		                else
		                {
		                    endOfData = BTHAL_FALSE;
		                }
				mdStatus = BTHAL_MD_UploadDataReady((BTHAL_U8 *)data,
                                                    num,
                                                    endOfData);				
				if (BTHAL_STATUS_SUCCESS != mdStatus)
				{
					BTL_LOG_DEBUG(("BTL_MDG: BTHAL_MD_UploadDataReady() status: %s",
								   BTHAL_StatusName(mdStatus)));
				}
				else if(serverContext->btControlSignalsChanged)
				{
					serverContext->btControlSignalsChanged = FALSE;
				}

#else /* Buffer is allocated by MDG */

				/* Update amount of data to be written to the modem */
				serverContext->uploadWriteSize += num;

#endif /* BTHAL_MD_UPLOAD_BUF_OWNER_MODEM == BTL_CONFIG_ENABLED */

			}

			else
			{
				BTL_LOG_DEBUG(("BTL_MDG: BTL_SPP_Read() status: %s",
							   pBT_Status(btStatus)));
			}
		}
		else if(serverContext->btControlSignalsChanged)
		{
			mdStatus = BTHAL_MD_UploadDataReady(NULL,
		                                                    0,
		                                                    FALSE);
			if (BTHAL_STATUS_SUCCESS != mdStatus)
			{
				BTL_LOG_DEBUG(("BTL_MDG: BTHAL_MD_UploadDataReady() status: %s",
							   BTHAL_StatusName(mdStatus)));
			}
			else 
			{
				serverContext->btControlSignalsChanged = FALSE;
			}
		}		
	}

	BTL_FUNC_END();
}

/*-------------------------------------------------------------------------------
 * BtlMdgDownload()
 *
 *     	Reads data from the modem and writes it into BT SPP port used by the MDG
 *		server.
 */
static void BtlMdgDownload(BtlMdgServerContext *serverContext)
{
	U8 *data = NULL;
	BTHAL_INT len = 0;
	BTHAL_INT num = 0;
	BthalStatus mdStatus = BTHAL_STATUS_SUCCESS;
	BtStatus btStatus = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BtlMdgDownload");

	/* Loop on reading data from modem and writing it to BT */
	while ((serverContext->downloadReadSize ||
			serverContext->downloadWriteSize) &&
		   (BTHAL_STATUS_SUCCESS == mdStatus) &&
		   (BT_STATUS_SUCCESS == btStatus))
	{

#if BTHAL_MD_DOWNLOAD_BUF_OWNER_MODEM == BTL_CONFIG_ENABLED

		/* Check whether some data was not written to BT because of missing BT
		 * buffers */
		if (0 != serverContext->downloadWriteSize)
		{
			data = serverContext->mdDownloadBuf +
				   serverContext->mdDownloadBufDataOffset;
			len = serverContext->downloadWriteSize;
		}
		else
		{
			/* Get length and pointer to data already downloaded by modem */
			mdStatus = BTHAL_MD_GetDownloadBuf((BTHAL_U8 **)&data, &len);

			if (BTHAL_STATUS_SUCCESS != mdStatus)
			{
				BTL_LOG_DEBUG(("BTL_MDG: BTHAL_MD_GetDownloadBuf() status: %s",
							   BTHAL_StatusName(mdStatus)));
				break;
			}

			/* Save them for possible future use */
			serverContext->mdDownloadBuf = data;

			/* Update amount of data read from the modem */
			serverContext->downloadReadSize -= len;
			/* and amount of data should be written to BT */
			serverContext->downloadWriteSize = len;
		}

#else /* Buffer is allocated by MDG */

		/* Check size of buffer available in BT SPP port */
		btStatus = BTL_SPP_GetFreeWriteSize(serverContext->sppPortContext, &len);

		if (BT_STATUS_SUCCESS != btStatus)
		{
			BTL_LOG_DEBUG(("BTL_MDG: BTL_SPP_GetFreeWriteSize() status: %s",
						   pBT_Status(btStatus)));
			break;
		}

		/* Adjust length of data to be read and initialize pointer to buffer */
		if (len >= serverContext->downloadReadSize)
		{
			len = serverContext->downloadReadSize;
		}
		data = serverContext->downloadBuf;

		/* Read downloaded data from modem */
		mdStatus = BTHAL_MD_Read(data, len, &num);

		if (BTHAL_STATUS_SUCCESS != mdStatus)
		{
			BTL_LOG_DEBUG(("BTL_MDG: BTHAL_MD_Read() status: %s",
						   BTHAL_StatusName(mdStatus)));
			break;
		}

		/* Update amount of data read from the modem */
		serverContext->downloadReadSize -= num;
		/* and amount of data should be written to BT */
		serverContext->downloadWriteSize += num;
		len = num;
			
#endif /* BTHAL_MD_DOWNLOAD_BUF_OWNER_MODEM */
		
		if (len > 0)
		{
			BTL_VERIFY_ERR_NO_RETVAR((serverContext->portSettings.dataPathTypes & SPP_PORT_DATA_PATH_TX_SYNC),
					   				 ("BTL_MDG: Only Tx Sync data path type is implemented in BTL_MDG"));

			/* Write downloaded data to SPP */
			btStatus = BTL_SPP_WriteSync(serverContext->sppPortContext,
									     data,
									     (U16)len,
									     (U16 *)&num);

			switch (num)
			{
				case 0:
					BTL_LOG_DEBUG(("BTL_MDG: Written to BT 0 bytes"));
					break;
				case 1:
					BTL_LOG_DEBUG(("BTL_MDG: Written to BT 1 byte - 0x%x", *data));
					break;
				case 2:
					BTL_LOG_DEBUG(("BTL_MDG: Written to BT 2 bytes - 0x%x 0x%x", *data, *(data+1)));
					break;
				case 3:
					BTL_LOG_DEBUG(("BTL_MDG: Written to BT 3 bytes - 0x%x 0x%x 0x%x", *data, *(data+1), *(data+2)));
					break;
				default:
					BTL_LOG_DEBUG(("BTL_MDG: Written %d bytes to BT - 0x%x 0x%x 0x%x 0x%x", num, *data, *(data+1), *(data+2), *(data+3)));
					break;
			}
			
			if (BT_STATUS_SUCCESS == btStatus)
			{
				/* Update amount of data was not written to BT because of missing
				 * buffers */
				if (serverContext->downloadWriteSize > 0)
				{
					serverContext->downloadWriteSize -= num;
				}
				
#if BTHAL_MD_DOWNLOAD_BUF_OWNER_MODEM == BTL_CONFIG_ENABLED

				/* Return even part of received buffer to modem */
				mdStatus = BTHAL_MD_ReturnDownloadBuf((BTHAL_U8 *)data, num);

					if (BTHAL_STATUS_SUCCESS != mdStatus)
					{
						BTL_LOG_DEBUG(("BTL_MDG: BTHAL_MD_ReturnDownloadBuf() status: %s",
									   BTHAL_StatusName(mdStatus)));
					}
				
				/* Check whether all data was written */
				if (0 == serverContext->downloadWriteSize)
				{
					/* Reset pointer and offset to data to be written later */
					serverContext->mdDownloadBuf = NULL;
					serverContext->mdDownloadBufDataOffset = 0;
				}
				/* There is not enough room for all data in BT buffers */
				else
				{
					BTL_LOG_DEBUG(("BTL_MDG: Not all data was written to BT"));

					/* Save offset to data to be written later */
					serverContext->mdDownloadBufDataOffset += num;

					/* Exit from the loop */
					break;
				}

#endif /* BTHAL_MD_DOWNLOAD_BUF_OWNER_MODEM */

			}
			else
			{
				BTL_LOG_DEBUG(("BTL_MDG: BTL_SPP_Write() status: %s",
							   pBT_Status(btStatus)));
			}
		}
	}

	BTL_FUNC_END();
}

/*-------------------------------------------------------------------------------
 * BtlMdgFindActiveServer()
 *
 *     	Checks if any server was already enabled and, if found, returns pointer
 *      to its context.
 */
static BtlMdgServerContext *BtlMdgFindActiveServer()
{
    BtlMdgServerContext *server;
    BtlMdgServerContext *foundServer = NULL;
    BtlContext *base;

	BTL_FUNC_START("BtlMdgFindActiveServer");

    /* Check whether another server is already enabled */
	IterateList(btlMdgData.contextsList, base, BtlContext *)
    {
        server = (BtlMdgServerContext *)base;

        if (server->state >= BTL_MDG_SERVER_STATE_ENABLED)
        {
            foundServer = server;
            break;
        }
    }

	BTL_FUNC_END();

    return (foundServer);
}

/*-------------------------------------------------------------------------------
 * BtlMdgProcessRadioOffNotification()
 *
 *		Checks all servers and disconnects them, if any connection procedures are
 *      active.
 */
static BtStatus BtlMdgProcessRadioOffNotification()
{
	BtStatus status = BT_STATUS_SUCCESS;
    BtlMdgServerContext *server;
	BtlContext *base;
    BOOL radioOffResponsePending = FALSE;

	BTL_FUNC_START("BtlMdgProcessRadioOffNotification");

    IterateList(btlMdgData.contextsList, base, BtlContext *)
    {
        server = (BtlMdgServerContext *)base;
        status = BT_STATUS_SUCCESS;

        switch(server->state)
        {
            case BTL_MDG_SERVER_STATE_DISABLING:
                /* Wait to finish current process and then continue */
                status = BT_STATUS_PENDING;
                break;

            case BTL_MDG_SERVER_STATE_DT_CONNECTED:
            case BTL_MDG_SERVER_STATE_CONNECTED:
                /* Disconnect the connection to the peer serial port */
                status = BTL_SPP_Disconnect(server->sppPortContext);

                /* Update port's state */
                if (BT_STATUS_SUCCESS == status)
                {
                    server->state = BTL_MDG_SERVER_STATE_DISCONNECTED;
                }
                else if (BT_STATUS_FAILED == status)
                {
                    BTL_LOG_ERROR(("BTL_MDG: BTL_SPP_Disconnect() failed"));
                }
                break;

            default:
                /* Nothing to do with port in IDLE, ENABLED or DISCONNECTED
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

	BTL_FUNC_END();

    return (status);
}

BtStatus BtlMdgBtlNotificationsCb(BtlModuleNotificationType notificationType)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BtlMdgBtlNotificationsCb");
	
	BTL_LOG_DEBUG(("BTL_MDG: Notification Type: %d", notificationType));
	
	switch (notificationType)
	{
		case BTL_MODULE_NOTIFICATION_RADIO_ON:
			/* Do nothing */
			break;

		case BTL_MODULE_NOTIFICATION_RADIO_OFF:

            /* Set Radio Off Notification flag */
            btlMdgData.isRadioOffNotified = TRUE;
                
			/* Disconnect all connections, stop all connections in establishment
             * and any other process in progress in all contexts and send
             * BTL_MODULE_NOTIFICATION_COMPLETION_EVENT_TYPE_RADIO_OFF, if all
             * connections are closed */
            status = BtlMdgProcessRadioOffNotification();
			break;

		default:
			BTL_ERR(BT_STATUS_INTERNAL_ERROR,
                    ("BTL_MDG: Invalid notification (%d)",
                    notificationType));

	};

	BTL_FUNC_END();
	
	return (status);
}

const char *BtlMdgBthalMdEventName(BthalMdEventType event)
{
	switch(event)
	{
		case BTHAL_MD_EVENT_INITIALIZED:
			return "BTHAL_MD_EVENT_INITIALIZED";
			
		case BTHAL_MD_EVENT_REGISTERED:
			return "BTHAL_MD_EVENT_REGISTERED";
			
		case BTHAL_MD_EVENT_CONFIGURED:
			return "BTHAL_MD_EVENT_CONFIGURED";
			
		case BTHAL_MD_EVENT_CONNECTED:
			return "BTHAL_MD_EVENT_CONNECTED";
			
		case BTHAL_MD_EVENT_DISCONNECTED:
			return "BTHAL_MD_EVENT_DISCONNECTED";
			
		case BTHAL_MD_EVENT_DOWNLOAD_DATA:
			return "BTHAL_MD_EVENT_DOWNLOAD_DATA";
			
		case BTHAL_MD_EVENT_UPLOAD_BUF:
			return "BTHAL_MD_EVENT_UPLOAD_BUF";
			
		case BTHAL_MD_EVENT_CONTROL_SIGNALS:
			return "BTHAL_MD_EVENT_CONTROL_SIGNALS";
			
		default:
			return "Unknown BTHAL MD event";
	}
}



#else /*BTL_CONFIG_MDG ==   BTL_CONFIG_ENABLED*/

/*-------------------------------------------------------------------------------
 * BTL_MDG_Init() - When  BTL_CONFIG_MDG is disabled.
 */
BtStatus BTL_MDG_Init(void)
{
    
   BTL_LOG_INFO(("BTL_MDG_Init()  -  BTL_CONFIG_MDG Disabled"));
  

    return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 * BTL_MDG_Deinit() - When  BTL_CONFIG_MDG is disabled.
 */
BtStatus BTL_MDG_Deinit(void)
{
    BTL_LOG_INFO(("BTL_MDG_Deinit() -  BTL_CONFIG_MDG Disabled"));

    return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------*/



#endif /*BTL_CONFIG_MDG ==   BTL_CONFIG_ENABLED*/




/*******************************************************************************\
*                                                                              *
*  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION           *
*                                                                              *
*  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE         *
*	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE        *
*	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO        *
*	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT         *
*	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL          *
*	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.           *
*                                                                              *
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:   btl_saps.c
*
*   DESCRIPTION: This file contains the BTL SAP Server role implementation.
*
*   AUTHOR:      Gerrit Slot
*
\*******************************************************************************/

#include "btl_config.h"
#include "btl_defs.h"

BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_SAPS);

#if BTL_CONFIG_SAPS ==   BTL_CONFIG_ENABLED
/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "xatypes.h" /* for types such as U8, U16, S8, S16,... */
#include "debug.h"
#include "sim.h"
#include "sec.h"
#include "btl_log_modules.h"
#include "btl_commoni.h"
#include "btl_pool.h"
#include "btl_unicode.h"
#include "btl_saps.h"
#include "bthal_sim.h"

/********************************************************************************
 *
 * Macros
 *
 *******************************************************************************/
#define DEFAULT_SECURITY_LEVEL (BSL_AUTHENTICATION_IN | BSL_ENCRYPTION_IN)
 
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

/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/
/*-------------------------------------------------------------------------------
 * BtlSapsInitState type
 *
 *     Defines the SAPS init state.
 */
typedef enum _BtlSapsInitState
{
    BTL_SAPS_INIT_STATE_NOT_INITIALIZED,
    BTL_SAPS_INIT_STATE_INITIALIZED,
    BTL_SAPS_INIT_STATE_INITIALIZATION_FAILED
} BtlSapsInitState;

/*-------------------------------------------------------------------------------
 * BtlSapsState type
 *
 *     Defines the SAPS state of a specific context (instance)
 */
typedef enum _BtlSapsState
{
     BTL_SAPS_STATE_IDLE,          /* Not in use, waiting for an Enable.       */
     BTL_SAPS_STATE_DISCONNECTED,  /* No connection, waiting for a connection. */
     BTL_SAPS_STATE_CONNECTED,     /* Connected to remote client.              */
     BTL_SAPS_STATE_DISCONNECTING, /* In process of disconnecting.             */
     BTL_SAPS_STATE_DISABLING,     /* In process of disabling.                 */
     BTL_SAPS_STATE_CONNECTING     /* In process of connecting.                */
} BtlSapsState;

/*-------------------------------------------------------------------------------
 * BthalSimState type
 *
 *     Defines the state of teh context in the communication to the BTHAL_SIM.
 */
typedef enum _BthalSimState
{
    BTHAL_SIM_STATE_IDLE,
    BTHAL_SIM_STATE_W4_APDU_RSP,
    BTHAL_SIM_STATE_W4_SIM_ON_RSP,
    BTHAL_SIM_STATE_W4_SIM_OFF_RSP,
    BTHAL_SIM_STATE_W4_SIM_RESET_RSP
} BthalSimState;

/*-------------------------------------------------------------------------------
 * BtlSapsContext structure
 *
 *     Represents BTL SAPS context.
 */
typedef struct _BtlSapsContext 
{
   
    BtlContext       base;        /* Must be first field!                    */

    BtlSapsState     state;       /* State of this context.                  */
    BthalSimState    bthalState;  /* State of the communication to the BTHAL */
    SimCardStatus    cardStatus;  /* Status of the SIM card.                 */
    BOOL             w4cardStatusSent; /* TRUE = waiting for the             */
                                       /*  SAEVENT_SERVER_STATUS_SENT event  */
                                       /*  from the SIM profile. Now no new  */
                                       /*  card status may be sent.          */
                                       /* FALSE = card status can be sent.   */
    BOOL             sendCardStatus;   /* TRUE = sending a CardStatus is     */                                             
                                       /*  waiting. It was blocked because   */
                                       /*  another SendStatus is in process  */
                                       /*  (see also w4cardStatusSent)       */
    U8               nrOfConnections;  /* Number of connections for this     */
                                       /*  context.                          */
    BtlUtf8          serviceName[BTL_CONFIG_SAPS_MAX_SERVICE_NAME+2+1];
                                  /* Friendly name for this service.         */
                                  /*  Stored in SDP format (extra 2 bytes)   */
                                  /*  Including 0-termination.               */
    BtlSapsCallBack  callback;    /* APP Callback for events from me.        */
    SimServer        serverAdmin; /* Admin of the server instance,           */
                                  /*  used by BlueSDK                        */
} BtlSapsontext;


/*-------------------------------------------------------------------------------
 * BtlSapsData structure
 *
 *     Represents the data of the BTL SAPS module.
 */
typedef struct _BtlSapsData
{
    /* Pool of SAPS contexts */
	BTL_POOL_DECLARE_POOL(contextsPool, contextsMemory, BTL_CONFIG_SAPS_MAX_NUM_OF_CONTEXTS, sizeof(BtlSapsContext));
    
    /* List of active SAPS contexts */
    ListEntry  contextsList;

} BtlSapsData;

/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * btlSapsInitState
 *
 *     Represents the current init state of Saps module.
 */
static BtlSapsInitState btlSapsInitState = BTL_SAPS_INIT_STATE_NOT_INITIALIZED;

/*-------------------------------------------------------------------------------
 * btlSapsData
 *
 *     Global data for this SAPS module, containing all context administrations.
 */
static BtlSapsData btlSapsData;

/*-------------------------------------------------------------------------------
 * btlSapsContextsPoolName
 *
 *     Represents the name of the Saps contexts pool.
 */
static const char btlSapsContextsPoolName[] = "SapsContexts";

/********************************************************************************
 *
 * Internal function prototypes
 *
 *******************************************************************************/
static BOOL HandleOpenInd(BtlSapsEvent *sapsEvent);
static BOOL HandleOpen(BtlSapsEvent *sapsEvent);
static BOOL HandleCloseInd(BtlSapsEvent *sapsEvent);
static BOOL HandleClosed(BtlSapsEvent *sapsEvent);
static BOOL HandleAtrReq(BtlSapsEvent *sapsEvent);
static BOOL HandleApduReq(BtlSapsEvent *sapsEvent);
static BOOL HandleStatusReq(BtlSapsEvent *sapsEvent);
static BOOL HandleSimOnReq(BtlSapsEvent *sapsEvent);
static BOOL HandleSimOffReq(BtlSapsEvent *sapsEvent);
static BOOL HandleResetSimReq(BtlSapsEvent *sapsEvent);
static BOOL HandleCardReaderStatusReq(BtlSapsEvent *sapsEvent);
static void SendCardStatus(BtlSapsContext *sapsContext);
static void SapsCallback(SimServer *server,
                         SimCallbackParms *parms);
static void AfterProcessing(BtlSapsEvent *sapsEvent);
static void BthalCallback(BthalSimEvent event,
                          SimResultCode result);
static BtlSapsContext *GetContext(void);
static void SetBthalState(BtlSapsContext *sapsContext,
                           BthalSimState state);
static void SetContextState(BtlSapsContext *sapsContext,
                           BtlSapsState state);

/* Some debug logging features. */
#if (XA_DEBUG == XA_ENABLED) || (XA_DEBUG_PRINT == XA_ENABLED)
static void LogContextState(BtlSapsState state);
static void LogBthalState(BthalSimState state);
#define BTL_SAPS_LOG_CONTEXT_STATE(state) LogContextState(state)
#define BTL_SAPS_LOG_BTHAL_STATE(state) LogBthalState(state) 
#else
/* Debugging disabled --> define empty macros */
#define BTL_SAPS_LOG_CONTEXT_STATE(state)
#define BTL_SAPS_LOG_BTHAL_STATE(state)
#endif /* (XA_DEBUG == XA_ENABLED) || (XA_DEBUG_PRINT == XA_ENABLED) */

BtStatus BtlSapsBtlNotificationsCb(BtlModuleNotificationType notificationType);

/********************************************************************************
 *
 * Function definitions
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------*
 *  EXTERNAL FUNCTIONS
 *-------------------------------------------------------------------------*/
 
/*-------------------------------------------------------------------------------
 * BTL_SAPS_Init()
 */
BtStatus BTL_SAPS_Init(void)
{
    BtStatus status = BT_STATUS_SUCCESS;

    BTL_FUNC_START_AND_LOCK_COMMON("BTL_SAPS_Init");
    
    BTL_VERIFY_ERR((BTL_SAPS_INIT_STATE_INITIALIZATION_FAILED != btlSapsInitState),
                   BT_STATUS_IMPROPER_STATE,
                   ("SAPS initialization failed before, please de-init before retrying "));
    BTL_VERIFY_ERR((BTL_SAPS_INIT_STATE_NOT_INITIALIZED == btlSapsInitState),
                   BT_STATUS_IMPROPER_STATE,
                   ("SAPS module is already initialized"));
    
    btlSapsInitState = BTL_SAPS_INIT_STATE_INITIALIZATION_FAILED;

    status = BTL_POOL_Create(&btlSapsData.contextsPool,
                             btlSapsContextsPoolName,
                             btlSapsData.contextsMemory, 
                             BTL_CONFIG_SAPS_MAX_NUM_OF_CONTEXTS,
                             sizeof(BtlSapsContext));
                             
    BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status),
                     BT_STATUS_INTERNAL_ERROR,
                     ("SAPS contexts pool creation failed"));
    
    InitializeListHead(&btlSapsData.contextsList);

	BTL_RegisterForCommonNotifications(BTL_MODULE_TYPE_SAPS, BtlSapsBtlNotificationsCb);
        
    btlSapsInitState = BTL_SAPS_INIT_STATE_INITIALIZED;

    BTL_FUNC_END_AND_UNLOCK();
    
    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_SAPS_Deinit()
 */
BtStatus BTL_SAPS_Deinit(void)
{
    BtStatus status = BT_STATUS_SUCCESS;

    BTL_FUNC_START_AND_LOCK_COMMON("BTL_SAPS_Deinit");

	BTL_DeRegisterFromCommonNotifications(BTL_MODULE_TYPE_SAPS);
    
    BTL_VERIFY_ERR((BTL_SAPS_INIT_STATE_NOT_INITIALIZED != btlSapsInitState),
                   BT_STATUS_IMPROPER_STATE,
                   ("SAPS module is not initialized"));

    BTL_VERIFY_ERR((IsListEmpty(&btlSapsData.contextsList)),
                           BT_STATUS_FAILED,
                           ("SAPS contexts are still active"));

    status = BTL_POOL_Destroy(&btlSapsData.contextsPool);
    
    BTL_VERIFY_FATAL_NORET((BT_STATUS_SUCCESS == status),
                           ("SAPS contexts pool destruction failed"));
        
    btlSapsInitState = BTL_SAPS_INIT_STATE_NOT_INITIALIZED;

    BTL_FUNC_END_AND_UNLOCK();
    
    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_SAPS_Create()
 */
BtStatus BTL_SAPS_Create(BtlAppHandle           *appHandle,
                         const BtlSapsCallBack   sapsCallback,
                         const BtSecurityLevel  *securityLevel,
                         BtlSapsContext        **sapsContext)
{
    BtStatus status = BT_STATUS_SUCCESS;

    BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_SAPS_Create");

    BTL_VERIFY_ERR((NULL != sapsCallback),
                   BT_STATUS_INVALID_PARM,
                   ("Null sapsCallback"));
    BTL_VERIFY_ERR((NULL != sapsContext),
                   BT_STATUS_INVALID_PARM,
                   ("Null sapsContext"));

#if BT_SECURITY == XA_ENABLED

    if (NULL != securityLevel)
    {
        /* securityLevel functionality is not implemented yet.*/
        BTL_FATAL(BT_STATUS_INVALID_PARM, ("Security level not supported"));
        
#if 0
        /* This code should be added when security functionality is implemented */
        status = BTL_VerifySecurityLevel(*securityLevel);
        BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status),
                       status,
                       ("Invalid SAPS securityLevel"));
#endif
    }

#endif  /* BT_SECURITY == XA_ENABLED */

    /* Allocate memory for a unique new SAPS context */
    status = BTL_POOL_Allocate(&btlSapsData.contextsPool, (void**)sapsContext);
    BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status),
                   status,
                   ("Failed allocating SAPS context"));

    status = BTL_HandleModuleInstanceCreation(appHandle,
                                              BTL_MODULE_TYPE_SAPS,
                                              &(*sapsContext)->base);
    
    if (BT_STATUS_SUCCESS != status)
    {
        /* sapsContext must be freed before we exit */
        BtStatus freeResult = BTL_POOL_Free(&btlSapsData.contextsPool, (void**)sapsContext);
        BTL_VERIFY_FATAL_NORET((BT_STATUS_SUCCESS == freeResult),
                               ("Failed freeing SAPS context (%s)",
                               pBT_Status(freeResult)));

        BTL_ERR(status,
                ("Failed handling SAPS instance creation (%s)",
                pBT_Status(status)));
    }

    /* Add the new SAPS context to the active contexts list */
    InsertTailList(&btlSapsData.contextsList, &((*sapsContext)->base.node));

    /* Save the given callback */
    (*sapsContext)->callback = sapsCallback;

    /* Init the further context administration with valid start-up values.*/
    (*sapsContext)->state  = BTL_SAPS_STATE_IDLE;

    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_SAPS_Destroy()
 */
BtStatus BTL_SAPS_Destroy(BtlSapsContext **sapsContext)
{
    BtStatus status      = BT_STATUS_SUCCESS;
    BOOL     isAllocated = FALSE;

    BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_SAPS_Destroy");

    BTL_VERIFY_ERR((NULL != sapsContext),
                   BT_STATUS_INVALID_PARM,
                   ("Null sapsContext"));
    BTL_VERIFY_ERR((NULL != *sapsContext),
                   BT_STATUS_INVALID_PARM,
                   ("Null *sapsContext"));
    BTL_VERIFY_ERR((BTL_SAPS_STATE_IDLE == (*sapsContext)->state),
                   BT_STATUS_IN_USE,
                   ("SAPS context is in use"));

    status = BTL_POOL_IsElelementAllocated(&btlSapsData.contextsPool, *sapsContext, &isAllocated);
    BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status),
                   status,
                   ("Failed locating given SAPS context"));
    BTL_VERIFY_ERR((TRUE == isAllocated),
                   BT_STATUS_INVALID_PARM,
                   ("Invalid SAPS context"));

    /* Remove the context from the list of all SAPS contexts */
    RemoveEntryList(&((*sapsContext)->base.node));

    status = BTL_HandleModuleInstanceDestruction(&(*sapsContext)->base);
    BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status),
                   BT_STATUS_INTERNAL_ERROR,
                   ("Failed handling SAPS instance destruction (%s)", pBT_Status(status)));

    status = BTL_POOL_Free(&btlSapsData.contextsPool, (void**)sapsContext);
    BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status),
                     BT_STATUS_INTERNAL_ERROR,
                     ("Failed freeing SAPS context (%s)", pBT_Status(status)));

    /* Clean the SAPS context for the caller. */
    *sapsContext = NULL;

    BTL_FUNC_END_AND_UNLOCK();

    return (status);
}

/*-------------------------------------------------------------------------------
 * BTL_SAPS_Enable()
 */
BtStatus BTL_SAPS_Enable(BtlSapsContext *sapsContext,
                        const BtlUtf8   *serviceName)
{
    BtStatus status = BT_STATUS_SUCCESS;
    U16      len = 0;

    BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_SAPS_Enable");

    BTL_VERIFY_ERR((NULL != sapsContext),
                   BT_STATUS_INVALID_PARM,
                   ("Null sapsContext"));
    BTL_VERIFY_ERR((BTL_SAPS_STATE_IDLE == sapsContext->state),
                   BT_STATUS_IN_USE, 
                   ("context is already enabled"));

    /* Non default serviceName? --> get it.*/        
    if (serviceName != NULL)
    {
        len = OS_StrLen((char*)serviceName);

        /* length too long? --> reply it. */
        BTL_VERIFY_ERR((len <= BTL_CONFIG_SAPS_MAX_SERVICE_NAME), BT_STATUS_FAILED, ("serviceName is too long!"));
    }

    /* Register as a SAP server, and register SapsCallback */
    status = SIM_RegisterServer(&(sapsContext->serverAdmin), SapsCallback);

    BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status),
                   status, 
                   ("Failed to register SAP Server (%s)", pBT_Status(status)));

    /* There is a valid user defined servicName? --> overide the default one.*/
    if (len != 0)
    {
        /* Prepare and store the name in SDP format. */
        /* The first 2 bytes are for SDP_TEXT_8BIT */
        sapsContext->serviceName[0] = DETD_TEXT + DESD_ADD_8BITS;
        sapsContext->serviceName[1] = (U8)(len + 1); /* Includes '\0' */        
        OS_MemCopy(&(sapsContext->serviceName[2]), serviceName, len);
        sapsContext->serviceName[(len + 2)] = '\0';

        /* Overide the serviceName in the SDP record.*/
        sapsContext->serverAdmin.sdpAttribute[4].len   = (U16)(len+3);
        sapsContext->serverAdmin.sdpAttribute[4].value = sapsContext->serviceName;
    }

    BTL_LOG_INFO(("SAP Server is enabled."));
    
    /* Register to the BTHAL_SIM */
    (void) BTHAL_SIM_Init(BthalCallback);
    
    /* Init the context admin. */
    SetContextState(sapsContext, BTL_SAPS_STATE_DISCONNECTED);
    sapsContext->bthalState       = BTHAL_SIM_STATE_IDLE;
    sapsContext->w4cardStatusSent = FALSE;
    sapsContext->sendCardStatus   = FALSE;
    sapsContext->nrOfConnections  = 0;
    (void) BTHAL_SIM_GetStatus(&(sapsContext->cardStatus));
    
    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_SAPS_Disable()
 */
BtStatus BTL_SAPS_Disable(BtlSapsContext *sapsContext)
{
    BtStatus status = BT_STATUS_SUCCESS;
    
    BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_SAPS_Disable");

    BTL_VERIFY_ERR((NULL != sapsContext),
                   BT_STATUS_INVALID_PARM,
                   ("Null sapsContext"));

    /* Context was enabled? --> process the disabling. */
    if (sapsContext->state != BTL_SAPS_STATE_IDLE)
    {
        /* Is there an open connection? --> disconnect them first. */
        if (sapsContext->nrOfConnections != 0)
        {
            /* Change to context state. */
            SetContextState(sapsContext, BTL_SAPS_STATE_DISABLING);

            /* Disconnect the link immediate. */
            status = SIM_ServerClose(&sapsContext->serverAdmin,
                                SIM_DISCONNECT_IMMEDIATE);

            /* Verify if it was indeed a success, otherwise make a note. */
            BTL_VERIFY_ERR((BT_STATUS_PENDING == status),
                           status, ("Disconnect during the SAPS disable failed: (%s)",
                           pBT_Status(status)));
        }
        /* All links are gone! --> remove registration. */
        else
        {
            /* Reset the context state. */
            SetContextState(sapsContext, BTL_SAPS_STATE_IDLE);

            /* Remove SDP registration. */
            status = SIM_DeregisterServer(&sapsContext->serverAdmin);


            /* Verify if it was indeed a success, otherwise make a note. */
            BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status),
                           status, ("Disable of SAPS failed: (%s)",
                           pBT_Status(status)));
        }
    
    }
    /* Context was already disabled! --> just make a note. */
    else
    {
        BTL_LOG_INFO(("SAP Server was already disabled."));
    }

	BTHAL_SIM_Deinit();

    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

#if BT_SECURITY == XA_ENABLED
/*-------------------------------------------------------------------------------
 * BTL_SAPS_SetSecurityLevel()
 */
BtStatus BTL_SAPS_SetSecurityLevel(BtlSapsContext        *sapsContext,
                                   const BtSecurityLevel *securityLevel)
{
    BtStatus status = BT_STATUS_SUCCESS;
    BtSecurityLevel secLevel;
    
    BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_SAPS_SetSecurityLevel");

	UNUSED_PARAMETER(sapsContext);

    /* No specific security level defined? --> fill it with default value. */
    if( NULL == securityLevel )
    {
        secLevel = DEFAULT_SECURITY_LEVEL;
    }
    /* Defined security level value! --> fill it.*/
    else
    {
        secLevel = *securityLevel;
    }

    /* Authentication should be enabled for this service. */
    BTL_VERIFY_ERR(((secLevel & BSL_AUTHENTICATION_IN) != 0), BT_STATUS_INVALID_PARM, ("Authentication in must be ON"));

    /* Verify if settings are valid. */
    status = BTL_VerifySecurityLevel(secLevel);
    BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Invalid SAPS securityLevel"));

    /* Everything OK: update the security level. */
    SIM_SetSecurityLevel(&secLevel);

    BTL_FUNC_END_AND_UNLOCK();
    
    return status;    
}

/*-------------------------------------------------------------------------------
 * BTL_SAPS_GetSecurityLevel()
 */
BtStatus BTL_SAPS_GetSecurityLevel(BtlSapsContext  *sapsContext,
                                   BtSecurityLevel *securityLevel)
{
    BtStatus status = BT_STATUS_SUCCESS;
    
    BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_SAPS_GetSecurityLevel");

	UNUSED_PARAMETER(sapsContext);
    
    SIM_GetSecurityLevel(securityLevel);

    BTL_FUNC_END_AND_UNLOCK();
    
    return status;
}
#endif  /* BT_SECURITY == XA_ENABLED */

/*-------------------------------------------------------------------------------
 * BTL_SAPS_GetConnectedDevice()
 */
BtStatus BTL_SAPS_GetConnectedDevice(BtlSapsContext *sapsContext,
                                     BD_ADDR        *bdAddr)
{
    BtStatus status = BT_STATUS_SUCCESS;
    
    BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_SAPS_GetConnectedDevice");

    BTL_VERIFY_ERR((NULL != sapsContext),
                   BT_STATUS_INVALID_PARM,
                   ("Null sapsContext"));
    BTL_VERIFY_ERR((NULL != bdAddr),
                    BT_STATUS_INVALID_PARM,
                    ("Null bdAddr"));

    /* Connected? --> return BD-Address */
    if (BTL_SAPS_STATE_CONNECTED == sapsContext->state)
    {
        BTL_VERIFY_ERR((NULL != sapsContext->serverAdmin.remDev),
                       BT_STATUS_INTERNAL_ERROR,
                       ("Unable to find connected remote device"));

        /* Read BDAddress from internal Blue-SDK administration. */    
        *bdAddr = sapsContext->serverAdmin.remDev->bdAddr;
    }
    /* Not connected! --> tell it the caller. */
    else
    {
        status = BT_STATUS_NO_CONNECTION;
    }

    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_SAPS_Disconnect()
 */
BtStatus BTL_SAPS_Disconnect(BtlSapsContext *sapsContext,
                             BOOL            immediate)
{
    BtStatus status = BT_STATUS_SUCCESS;
    
    BTL_FUNC_START_AND_LOCK("BTL_SAPS_Disconnect");

    BTL_VERIFY_ERR((NULL != sapsContext),
                   BT_STATUS_INVALID_PARM,
                   ("Null sapsContext"));
    BTL_VERIFY_ERR((BTL_SAPS_STATE_CONNECTED == sapsContext->state),
                   BT_STATUS_NO_CONNECTION, 
                   ("context must be connected"));

    if (TRUE == immediate)
    {
        status = SIM_ServerClose(&sapsContext->serverAdmin,
                                 SIM_DISCONNECT_IMMEDIATE);
    }
    else
    {
        status = SIM_ServerClose(&sapsContext->serverAdmin,
                                 SIM_DISCONNECT_GRACEFUL);
    }

    /* Pending? --> update state. */
    if (BT_STATUS_PENDING == status)
    {
        /* SAPS state is now in the process of disconnecting */
        SetContextState(sapsContext, BTL_SAPS_STATE_DISCONNECTING);
    }
    /* Finished? --> update state to DISCONNECTED */
    else if (BT_STATUS_SUCCESS == status)
    {
        SetContextState(sapsContext, BTL_SAPS_STATE_DISCONNECTED);
    }
    /* Other reasons? --> do nothing; keep the same state as before. */
    else
    {
    }

    BTL_FUNC_END_AND_UNLOCK();

    return status;
}


/*-------------------------------------------------------------------------------
 * BTL_SAPS_RespondOpenInd()
 */
BtStatus BTL_SAPS_RespondOpenInd(BtlSapsContext *sapsContext,
                                 BOOL            accept)
{
    BtStatus status = BT_STATUS_SUCCESS;

    BTL_FUNC_START_AND_LOCK("BTL_SAPS_RespondOpenInd");

    BTL_VERIFY_ERR((NULL != sapsContext),
                   BT_STATUS_INVALID_PARM,
                   ("Null sapsContext"));
    BTL_VERIFY_ERR((BTL_SAPS_STATE_CONNECTING == sapsContext->state),
                   BT_STATUS_NO_CONNECTION, 
                   ("context must be connected"));

    if (TRUE == accept)
    {
        status = SIM_ServerRespondOpenInd(&sapsContext->serverAdmin,
                                          SIM_CONN_STATUS_OK);
    }
    else
    {
        status = SIM_ServerRespondOpenInd(&sapsContext->serverAdmin,
                                          SIM_CONN_STATUS_UNABLE);
    }

    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*-------------------------------------------------------------------------*
 *  LOCAL FUNCTIONS
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------
 * HandleOpenInd()
 *
 *    Process the SAEVENT_SERVER_OPEN_IND from the SIM module in the Blue SDK
 *
 *    Client has requested a connection to the server. During processing of 
 *    this event, call SIM_ServerRespondOpenInd to indicate whether the connection 
 *    should be allowed. When this callback is received, the 
 *    "SimCallbackparms.ptrs.remDev" field contains a pointer to the remote device.
 *
 * Parameters:
 *		sapsEvent [in/out] - All relevant data (Event data, SapsContext) for
 *        processing the event. It can be adpated as well for passing extra info
 *        to the APP.
 *
 * Returns:
 *		TRUE - sapsEvent should be passed to the APP.
 *
 *		FALSE - sapsEvent is completely handled inside. It does not need to
 *        be passed to the APP.
 */
static BOOL HandleOpenInd(BtlSapsEvent *sapsEvent)
{
    BOOL passEventToApp = TRUE; /* Function return. */

    BTL_FUNC_START("HandleOpenInd");

    /* Process event in correct state. */
    switch (sapsEvent->context->state)
    {
        /* Legal state? --> change state + notify APP */
        case BTL_SAPS_STATE_DISCONNECTED:
            SetContextState(sapsEvent->context, BTL_SAPS_STATE_CONNECTING);
            break;

        /* Illegal state? --> make a note and don't notify the APP. */
        case BTL_SAPS_STATE_CONNECTED:
        case BTL_SAPS_STATE_CONNECTING:
        case BTL_SAPS_STATE_IDLE:
			      BTL_LOG_ERROR(("Illegal state detected."));

        /* FALL THROUGH!*/
        
        /* In process of disabling/disconnecting?      */
        /* --> refuse request; don't involve the APP.  */
        case BTL_SAPS_STATE_DISABLING:
        case BTL_SAPS_STATE_DISCONNECTING:
            /* NOTE: What does the SIM_xxx function return in this case?? */
            (void) SIM_ServerRespondOpenInd(&(sapsEvent->context->serverAdmin),
                                            SIM_CONN_STATUS_UNABLE);
            passEventToApp = FALSE;
            break;

        /* Unknown state? -->  error notification +              */
        /*  keep the machine running by giving a negative reply. */
        default:
			      BTL_LOG_ERROR(("Unknown state detected."));
            (void) SIM_ServerRespondOpenInd(&(sapsEvent->context->serverAdmin),
                                            SIM_CONN_STATUS_UNABLE);
            passEventToApp = FALSE;
            break;
    }

    BTL_FUNC_END();

    return passEventToApp;
}

/*-------------------------------------------------------------------------------
 * HandleOpen()
 *
 *    Process the SAEVENT_SERVER_OPEN from the SIM module in the Blue SDK
 *
 *    A connection is open.  This can come as a result of a call to 
 *    SIM_ServerRespondOpenInd.  When this event has been received, the  
 *    connection is available for receiving requests and sending responses.
 *    When this callback is received, the "SimCallbackParms.ptrs.remDev" field 
 *    contains a pointer to the remote device.
 *
 * Parameters:
 *		sapsEvent [in/out] - All relevant data (Event data, SapsContext) for
 *        processing the event. It can be adapted as well for passing extra info
 *        to the APP.
 *
 * Returns:
 *		TRUE - sapsEvent should be passed to the APP.
 *
 *		FALSE - sapsEvent is completely handled inside. It does not need to
 *        be passed to the APP.
 */
static BOOL HandleOpen(BtlSapsEvent *sapsEvent)
{
    BOOL passEventToApp = TRUE; /* Function return. */

    BTL_FUNC_START("HandleOpen");

    /* Process event in correct state. */
    switch (sapsEvent->context->state)
    {
        /* Legal state? --> change state + notify APP */
        case BTL_SAPS_STATE_CONNECTING:
            SetContextState(sapsEvent->context, BTL_SAPS_STATE_CONNECTED);
            sapsEvent->context->nrOfConnections++;
            break;

        /* Illegal state? --> make a note and don't notify the APP. */
        case BTL_SAPS_STATE_DISCONNECTING:
        case BTL_SAPS_STATE_DISCONNECTED:
        case BTL_SAPS_STATE_CONNECTED:
        case BTL_SAPS_STATE_IDLE:
        case BTL_SAPS_STATE_DISABLING:
			      BTL_LOG_ERROR(("Illegal state detected."));
            passEventToApp = FALSE;
            break;

        /* Unknown state? -->  error notification */
        default:
			      BTL_LOG_ERROR(("Unknown state detected."));
            passEventToApp = FALSE;
            break;
    }

    BTL_FUNC_END();

    return passEventToApp;
}

/*-------------------------------------------------------------------------------
 * HandleCloseInd()
 *
 *    Process the SAEVENT_SERVER_CLOSE_IND from the SIM module in the Blue SDK
 *
 *    The remote device is closing the connection.  Once the connection is closed, 
 *    a SAEVENT_SERVER_CLOSED event will be received.  Requests will not be 
 *    received and responses cannot be sent in this state.
 *    When this callback is received, the "SimCallbackParms.ptrs.remDev" field 
 *    contains a pointer to the remote device.
 *
 * Parameters:
 *		sapsEvent [in/out] - All relevant data (Event data, SapsContext) for
 *        processing the event. It can be adpated as well for passing extra info
 *        to the APP.
 *
 * Returns:
 *		FALSE - sapsEvent is completely handled inside. It does not need to
 *        be passed to the APP.
 */
static BOOL HandleCloseInd(BtlSapsEvent *sapsEvent)
{
    BTL_FUNC_START("HandleCloseInd");

    /* Process event in correct state. */
    switch (sapsEvent->context->state)
    {
        /* Legal state? --> change state + do not notify the APP:              */
        /*  it will be followed by SAEVENT_SERVER_CLOSED for the APP later on. */
        case BTL_SAPS_STATE_CONNECTED:
            SetContextState(sapsEvent->context, BTL_SAPS_STATE_DISCONNECTING);
            break;

        /* Illegal state? --> make a note and don't notify the APP. */
        case BTL_SAPS_STATE_DISCONNECTED:
        case BTL_SAPS_STATE_CONNECTING:
        case BTL_SAPS_STATE_IDLE:
        case BTL_SAPS_STATE_DISABLING:
        case BTL_SAPS_STATE_DISCONNECTING:
			      BTL_LOG_ERROR(("Illegal state detected."));
            break;

        /* Unknown state? -->  error notification +              */
        /*  keep the machine running by giving a negative reply. */
        default:
			      BTL_LOG_ERROR(("Unknown state detected."));
            break;
    }

    BTL_FUNC_END();

    return FALSE;
}

/*-------------------------------------------------------------------------------
 * HandleClosed()
 *
 *    Process the SAEVENT_SERVER_CLOSED from the SIM module in the Blue SDK
 *
 *    The connection is closed.  This can come as a result of calling 
 *    SIM_ServerClose, if the remote device has closed the connection, or if an 
 *    incoming connection is rejected by a call to SIM_ServerRespondOpenInd.  
 *    Requests will not be received and responses cannot be sent in this state.  
 *    When this callback is received, the "SimCallbackParms.ptrs.remDev" field 
 *    contains a pointer to the remote device.
 *
 * Parameters:
 *		sapsEvent [in/out] - All relevant data (Event data, SapsContext) for
 *        processing the event. It can be adpated as well for passing extra info
 *        to the APP.
 *
 * Returns:
 *		TRUE - sapsEvent should be passed to the APP.
 *
 *		FALSE - sapsEvent is completely handled inside. It does not need to
 *        be passed to the APP.
 */
static BOOL HandleClosed(BtlSapsEvent *sapsEvent)
{
    BOOL passEventToApp = TRUE; /* Function return. */

    BTL_FUNC_START("HandleClosed");

    /* Process event in correct state. */
    switch (sapsEvent->context->state)
    {
        /* Legal state? --> change state + notify APP */
        case BTL_SAPS_STATE_DISCONNECTING:
        case BTL_SAPS_STATE_CONNECTED:  /* to be robust. */
            SetContextState(sapsEvent->context, BTL_SAPS_STATE_DISCONNECTED);

        /* FALL TROUGH! */

        /* Disconnect as result of a disable? --> keep the same state.    */
        /*  After notifying the application of the last disconnection,    */
        /*  ths application will be notified and the state is put to IDLE */
        case BTL_SAPS_STATE_DISABLING:
            sapsEvent->context->nrOfConnections--;
            break;

        /* Illegal state? --> make a note and don't notify the APP. */
        case BTL_SAPS_STATE_DISCONNECTED:
        case BTL_SAPS_STATE_CONNECTING:
        case BTL_SAPS_STATE_IDLE:
			      BTL_LOG_ERROR(("Illegal state detected."));
            passEventToApp = FALSE;
            break;

        /* Unknown state? -->  error notification */
        default:
			      BTL_LOG_ERROR(("Unknown state detected."));
            passEventToApp = FALSE;
            break;
    }

    BTL_FUNC_END();

    return passEventToApp;
}

/*-------------------------------------------------------------------------------
 * HandleAtrReq()
 *
 *    Process the SAEVENT_SERVER_ATR_REQ from the SIM module in the Blue SDK
 *
 *    The ATR is being requested by the client.  This can happen at any time
 *    during a connection (after SAEVENT_SERVER_OPEN is received).  It can also
 *    happen before the connection (after SAEVENT_SERVER_OPEN_IND is received).
 *    The server must respond by calling SIM_ServerAtrRsp with a valid ATR
 *    as specified by the GSM spec, or with an error code describing the reason
 *    the response cannot be made.  During this callback, the
 *    "SimCallbackParms.ptrs" field is not valid.
 *
 * Parameters:
 *		sapsEvent [in/out] - All relevant data (Event data, SapsContext) for
 *        processing the event. It can be adpated as well for passing extra info
 *        to the APP.
 *
 * Returns:
 *		FALSE - sapsEvent is completely handled inside. It does not need to
 *        be passed to the APP.
 */
static BOOL HandleAtrReq(BtlSapsEvent *sapsEvent)
{
    BtStatus       btStatus;
    U16            len;
    U8            *atr;
    SimResultCode  simResultCode;

    BTL_FUNC_START("HandleAtrReq");

    /* CONNECTED or CONNECTING? --> process it via the BTHAL. */
    if ((sapsEvent->context->state == BTL_SAPS_STATE_CONNECTED) ||
        (sapsEvent->context->state == BTL_SAPS_STATE_CONNECTING))
    {
        /* Read ATR from the BTHAL, return will always be OK. */
        (void) BTHAL_SIM_GetAtr(&len, &atr, &simResultCode);

       /* Reply ATR to the remote side. */
        btStatus = SIM_ServerAtrRsp(&(sapsEvent->context->serverAdmin),
                                    len,
                                    atr,
                                    simResultCode);
    }
    else
    {
		    BTL_LOG_ERROR(("Not connected."));
    }

    BTL_FUNC_END();

    return FALSE;
}

/*-------------------------------------------------------------------------------
 * HandleApduReq()
 *
 *    Process the SAEVENT_SERVER_APDU_REQ from the SIM module in the Blue SDK
 *
 *    An APDU has been received from the client.  This can happen at any time
 *    during a connection (after SAEVENT_SERVER_OPEN is received).
 *    The server must respond by calling SIM_ServerApduRsp with a valid APDU
 *    as specified by the GSM spec, or with an error code describing the reason
 *    the response cannot be made.  During this callback, the 
 *    "SimCallbackParms.ptrs.msg.parms.apdu" field points to the command APDU.
=*
 * Parameters:
 *		sapsEvent [in/out] - All relevant data (Event data, SapsContext) for
 *        processing the event. It can be adpated as well for passing extra info
 *        to the APP.
 *
 * Returns:
 *		FALSE - sapsEvent is completely handled inside. It does not need to
 *        be passed to the APP.
 */
static BOOL HandleApduReq(BtlSapsEvent *sapsEvent)
{
    BthalStatus  bthalStatus;

    BTL_FUNC_START("HandleApduReq");

    /* CONNECTED? --> process it via the BTHAL. */
    if (sapsEvent->context->state == BTL_SAPS_STATE_CONNECTED)
    {
        /* Send it to the SIM and wait for the response in the BthalCallBack. */
        bthalStatus = BTHAL_SIM_WriteApduReq(sapsEvent->parms->ptrs.msg->parm.apdu.dataLen,
                                             sapsEvent->parms->ptrs.msg->parm.apdu.data);

        /* Failed, because APDU request is too big?          */
        /* --> give immediate negative reply to remote side. */
        if (bthalStatus == BTHAL_STATUS_FAILED)
        {
            (void) SIM_ServerApduRsp(&(sapsEvent->context->serverAdmin),0,NULL,SIM_RESULT_NO_REASON);
        }
        /* Pending? --> wait for the response in the BthalCallBack. */
        else if (bthalStatus == BTHAL_STATUS_PENDING)
        {            
            SetBthalState(sapsEvent->context, BTHAL_SIM_STATE_W4_APDU_RSP);
        }
        /* Success? --> finished: response is already sent via the BthalCallBack. */
        else
        {            
        }
    }
    
    else
    {
		    BTL_LOG_ERROR(("Not connected.."));
    }

    BTL_FUNC_END();

    return FALSE;
}

/*-------------------------------------------------------------------------------
 * HandleStatusReq()
 *
 *    Process the SAEVENT_SERVER_STATUS_REQ from the SIM module in the Blue SDK
 *
 *    During the connect process, the server must send the status of the SIM
 *    card. The application will be prompted to send the status with this
 *    event. During the processing of this event, the application must call 
 *    SIM_ServerSendStatus.  During this callback, the "SimCallbackParms.ptrs" 
 *    field is not valid.
 *
 *
 * Parameters:
 *		sapsEvent [in/out] - All relevant data (Event data, SapsContext) for
 *        processing the event. It can be adapted as well for passing extra info
 *        to the APP.
 *
 * Returns:
 *		FALSE - sapsEvent is completely handled inside. It does not need to
 *        be passed to the APP.
 */
static BOOL HandleStatusReq(BtlSapsEvent *sapsEvent)
{
    BTL_FUNC_START("HandleStatusReq");

    SendCardStatus(sapsEvent->context);

    BTL_FUNC_END();

    return FALSE;
}

/*-------------------------------------------------------------------------------
 * HandleSimOnReq()
 *
 *    Process the SAEVENT_SERVER_SIM_ON_REQ from the SIM module in the Blue SDK
 *
 *    When the client requests that the SIM card be turned on, the server will
 *    receive the SAEVENT_SERVER_SIM_ON_REQ event. The server should respond
 *    to this request by calling SIM_ServerSimOnRsp with the result of the
 *    action (see SimResultCode). During this callback, the "SimCallbackParms.ptrs" 
 *    field is not valid.
 *
 * Parameters:
 *		sapsEvent [in/out] - All relevant data (Event data, SapsContext) for
 *        processing the event. It can be adpated as well for passing extra info
 *        to the APP.
 *
 * Returns:
 *		FALSE - sapsEvent is completely handled inside. It does not need to
 *        be passed to the APP.
 */
static BOOL HandleSimOnReq(BtlSapsEvent *sapsEvent)
{
    BthalStatus  bthalStatus;

    BTL_FUNC_START("HandleSimOnReq");

    /* CONNECTED? --> process it via the BTHAL. */
    if (sapsEvent->context->state == BTL_SAPS_STATE_CONNECTED)
    {
        /* Next state is waiting for the reply on next request.*/
        SetBthalState(sapsEvent->context,BTHAL_SIM_STATE_W4_SIM_ON_RSP);
    
        /* Send it to the SIM and wait for the response in the BthalCallBack. */
        bthalStatus = BTHAL_SIM_PowerOn();

        /* Pending? --> wait for the response in the BthalCallBack. */
        if (bthalStatus == BTHAL_STATUS_PENDING)
        {
            SetBthalState(sapsEvent->context, BTHAL_SIM_STATE_W4_SIM_ON_RSP);
        }
        /* Success? --> finished: response is already sent via the BthalCallBack. */
        else
        {            
        }
    }
    
    else
    {
		    BTL_LOG_ERROR(("Not connected.."));
    }

    BTL_FUNC_END();

    return FALSE;
}

/*-------------------------------------------------------------------------------
 * HandleSimOffReq()
 *
 *    Process the SAEVENT_SERVER_SIM_OFF_REQ from the SIM module in the Blue SDK
 *
 *    When the client requests that the SIM card be turned off, the server will
 *    receive the SAEVENT_SERVER_SIM_OFF_REQ event. The server should respond
 *    to this request by calling SIM_ServerSimOffRsp with the result of the
 *    action (see SimResultCode). During this callback, the "SimCallbackParms.ptrs" 
 *    field is not valid.
 *
 * Parameters:
 *		sapsEvent [in/out] - All relevant data (Event data, SapsContext) for
 *        processing the event. It can be adpated as well for passing extra info
 *        to the APP.
 *
 * Returns:
 *		FALSE - sapsEvent is completely handled inside. It does not need to
 *        be passed to the APP.
 */
static BOOL HandleSimOffReq(BtlSapsEvent *sapsEvent)
{
    BthalStatus  bthalStatus;

    BTL_FUNC_START("HandleSimOffReq");

    /* CONNECTED? --> process it via the BTHAL. */
    if (sapsEvent->context->state == BTL_SAPS_STATE_CONNECTED)
    {
        /* Next state is waiting for the reply on next request.*/
        SetBthalState(sapsEvent->context,BTHAL_SIM_STATE_W4_SIM_OFF_RSP);
    
        /* Send it to the SIM and wait for the response in the BthalCallBack. */
        bthalStatus = BTHAL_SIM_PowerOff();

        /* Pending? --> wait for the response in the BthalCallBack. */
        if (bthalStatus == BTHAL_STATUS_PENDING)
        {            
            SetBthalState(sapsEvent->context, BTHAL_SIM_STATE_W4_SIM_OFF_RSP);
        }
        /* Success? --> finished: response is already sent via the BthalCallBack. */
        else
        {            
        }
    }
    
    else
    {
		    BTL_LOG_ERROR(("Not connected.."));
    }

    BTL_FUNC_END();

    return FALSE;
}

/*-------------------------------------------------------------------------------
 * HandleResetSimReq()
 *
 *    Process the SAEVENT_SERVER_RESET_SIM_REQ from the SIM module in the Blue SDK
 *
 *    When the client requests that the SIM card be reset, the server will
 *    receive the SAEVENT_SERVER_RESET_SIM_REQ event. The server should respond
 *    to this request by calling SIM_ServerResetSimRsp with the result of the
 *    action (see SimResultCode). During this callback, the "SimCallbackParms.ptrs" 
 *    field is not valid.
 *
 * Parameters:
 *		sapsEvent [in/out] - All relevant data (Event data, SapsContext) for
 *        processing the event. It can be adpated as well for passing extra info
 *        to the APP.
 *
 * Returns:
 *		FALSE - sapsEvent is completely handled inside. It does not need to
 *        be passed to the APP.
 */
static BOOL HandleResetSimReq(BtlSapsEvent *sapsEvent)
{
    BthalStatus bthalStatus;

    BTL_FUNC_START("HandleSimOffReq");

    /* CONNECTED? --> process it via the BTHAL. */
    if (sapsEvent->context->state == BTL_SAPS_STATE_CONNECTED)
    {
        /* Next state is waiting for the reply on next request.*/
        SetBthalState(sapsEvent->context,BTHAL_SIM_STATE_W4_SIM_RESET_RSP);
    
        /* Send it to the SIM and wait for the response in the BthalCallBack. */
        bthalStatus = BTHAL_SIM_Reset();

        /* Pending? --> wait for the response in the BthalCallBack. */
        if (bthalStatus == BTHAL_STATUS_PENDING)
        {            
            SetBthalState(sapsEvent->context, BTHAL_SIM_STATE_W4_SIM_RESET_RSP);
        }
        /* Success? --> finished: response is already sent via the BthalCallBack. */
        else
        {            
        }
    }
    
    else
    {
		    BTL_LOG_ERROR(("Not connected.."));
    }

    BTL_FUNC_END();

    return FALSE;
}

/*-------------------------------------------------------------------------------
 * HandleCardReaderStatusReq()
 *
 *    Process the SAEVENT_SERVER_CARD_READER_STATUS_REQ from the SIM module in the Blue SDK
 *
 *    When the client requests the status of the card reader, the server will
 *    receive the SAEVENT_SERVER_CARD_READER_STATUS_REQ event.  The server should 
 *    respond to this request by calling SIM_ServerCardReaderStatusRsp with the 
 *    result of the action (see SimResultCode and SimCardReaderStatus).
 *    During this callback, the "SimCallbackParms.ptrs" field is not valid.
 *
 * Parameters:
 *		sapsEvent [in/out] - All relevant data (Event data, SapsContext) for
 *        processing the event. It can be adapted as well for passing extra info
 *        to the APP.
 *
 * Returns:
 *		TRUE - sapsEvent should be passed to the APP.
 *
 *		FALSE - sapsEvent is completely handled inside. It does not need to
 *        be passed to the APP.
 */
static BOOL HandleCardReaderStatusReq(BtlSapsEvent *sapsEvent)
{
    BtStatus            btStatus;
    SimResultCode       simResultCode;
    SimCardReaderStatus cardReaderStatus;

    BTL_FUNC_START("HandleAtrReq");

    /* CONNECTED? --> process it via the BTHAL. */
    if (sapsEvent->context->state == BTL_SAPS_STATE_CONNECTED)
    {
        /* Read CardReaderStatus from the BTHAL, return will always be OK. */
        (void) BTHAL_SIM_GetCardReaderStatus(&cardReaderStatus,&simResultCode);

        /* Reply result to the remote side. */
        btStatus = SIM_ServerCardReaderStatusRsp(&(sapsEvent->context->serverAdmin),
                                                 cardReaderStatus,
                                                 simResultCode);
    }
    else
    {
		    BTL_LOG_ERROR(("Not connected.."));
    }

    BTL_FUNC_END();

    return FALSE;
}

/*-------------------------------------------------------------------------------
 * SendCardStatus()
 *
 *    Send a new CardStatus to the remote client.
 *
 * Parameters:
 *		sapsContext [in] - Admin with all info for sending the CardStatus
 *
 * Returns:
 *    void
 */
static void SendCardStatus(BtlSapsContext *sapsContext)
{
    BTL_FUNC_START("SendCardStatus");

    /* SIM profile is blocked for sending a new status?            */
    /*  (It is waiting for SAEVENT_SERVER_STATUS_SENT event)       */
    /*  --> put it in the queue and send it when it is free again. */
    if(TRUE == sapsContext->w4cardStatusSent)
    {
        sapsContext->sendCardStatus = TRUE;
    }
    /* I can send the status! --> do it. */
    else
    {
        /* Succeeded in sending the status?                    */
        /* --> expect a SAEVENT_SERVER_STATUS_SENT event back. */
        if (SIM_ServerSendStatus(&(sapsContext->serverAdmin),
                                 sapsContext->cardStatus) == BT_STATUS_PENDING)
        {
            sapsContext->w4cardStatusSent = TRUE;
        }
        
        /* I have sent a CardStatus to the remote side, so reset admin. */
        sapsContext->sendCardStatus = FALSE;
    }
    
    BTL_FUNC_END();    
}

/*-------------------------------------------------------------------------------
 * SapsCallback()
 *
 *    Callback function for events from the SIM profile in the Blue SDK
 *
 * Parameters:
 *		server [in] - Identification of the srever instance.
 *
 *    parms [in] - Parameters of the event.
 *
 * Returns:
 *    void
 */
static void SapsCallback(SimServer        *server,
                         SimCallbackParms *parms)
{
    BtlSapsEvent    sapsEvent;
    BtlSapsContext *sapsContext;
	  BOOL            passEventToApp = TRUE;

    BTL_FUNC_START("SapsCallback");

    /* Get correct SapsContext.
     * Subtract the offset of the field BtlSapsContext->serverAdmin from the address
     * of 'server' to get the address of BtlBipContext.
     * BtlSapsContext->serverAdmin is given as parameter to the SIM_xxx functions which
     * is the 'server' in the callback.
     */
    sapsContext = ContainingRecord(server, BtlSapsContext, serverAdmin);

    /* Prepare event for the APP. */
    sapsEvent.context = sapsContext;
    sapsEvent.event   = parms->event;
    sapsEvent.parms   = parms;
    
    switch (parms->event)
    {
        /** A client has requested a connection to the server.  During processing of 
         *  this event, call SIM_ServerRespondOpenInd to indicate whether the connection 
         *  should be allowed.  When this callback is received, the 
         *  "SimCallbackparms.ptrs.remDev" field contains a pointer to the remote device.
         */
        case SAEVENT_SERVER_OPEN_IND:
            BTL_LOG_INFO(("SAEVENT_SERVER_OPEN_IND"));
            passEventToApp = HandleOpenInd(&sapsEvent);
            break;

        /** A connection is open.  This can come as a result of a call to 
         *  SIM_ServerRespondOpenInd.  When this event has been received, the  
         *  connection is available for receiving requests and sending responses.
         *  When this callback is received, the "SimCallbackParms.ptrs.remDev" field 
         *  contains a pointer to the remote device.
         */
        case SAEVENT_SERVER_OPEN:
            BTL_LOG_INFO(("SAEVENT_SERVER_OPEN"));
            passEventToApp = HandleOpen(&sapsEvent);
            break;

        /** The remote device is closing the connection.  Once the connection is closed, 
         *  a SAEVENT_SERVER_CLOSED event will be received.  Requests will not be 
         *  received and responses cannot be sent in this state.
         *  When this callback is received, the "SimCallbackParms.ptrs.remDev" field 
         *  contains a pointer to the remote device.
         */
        case SAEVENT_SERVER_CLOSE_IND:
            BTL_LOG_INFO(("SAEVENT_SERVER_CLOSE_IND"));
            passEventToApp = HandleCloseInd(&sapsEvent);
            break;

        /** The connection is closed.  This can come as a result of calling 
         *  SIM_ServerClose, if the remote device has closed the connection, or if an 
         *  incoming connection is rejected by a call to SIM_ServerRespondOpenInd.  
         *  Requests will not be received and responses cannot be sent in this state.  
         *  When this callback is received, the "SimCallbackParms.ptrs.remDev" field 
         *  contains a pointer to the remote device.
         */
        case SAEVENT_SERVER_CLOSED:
            BTL_LOG_INFO(("SAEVENT_SERVER_CLOSED"));
            passEventToApp = HandleClosed(&sapsEvent);
            break;

        /** The ATR is being requested by the client.  This can happen at any time
         *  during a connection (after SAEVENT_SERVER_OPEN is received).  It can also
         *  happen before the connection (after SAEVENT_SERVER_OPEN_IND is received).
         *  The server must respond by calling SIM_ServerAtrRsp with a valid ATR
         *  as specified by the GSM spec, or with an error code describing the reason
         *  the response cannot be made.  During this callback, the
         *  "SimCallbackParms.ptrs" field is not valid.
         */
        case SAEVENT_SERVER_ATR_REQ:
            BTL_LOG_INFO(("SAEVENT_SERVER_ATR_REQ"));
            passEventToApp = HandleAtrReq(&sapsEvent);
            break;

        /** An APDU has been received from the client.  This can happen at any time
         *  during a connection (after SAEVENT_SERVER_OPEN is received).
         *  The server must respond by calling SIM_ServerApduRsp with a valid APDU
         *  as specified by the GSM spec, or with an error code describing the reason
         *  the response cannot be made.  During this callback, the 
         *  "SimCallbackParms.ptrs.msg.parms.apdu" field points to the command APDU.
         */
        case SAEVENT_SERVER_APDU_REQ:
            BTL_LOG_INFO(("SAEVENT_SERVER_APDU_REQ"));
            passEventToApp = HandleApduReq(&sapsEvent);
            break;

        /** During the connect process, the server must send the status of the SIM
         *  card.  The application will be prompted to send the status with this
         *  event.  During the processing of this event, the application must call 
         *  SIM_ServerSendStatus.  During this callback, the "SimCallbackParms.ptrs" 
         *  field is not valid.
         */
        case SAEVENT_SERVER_STATUS_REQ:
            BTL_LOG_INFO(("SAEVENT_SERVER_STATUS_REQ"));
            passEventToApp = HandleStatusReq(&sapsEvent);
            break;

        /** When the client requests that the SIM card be turned on, the server will
         *  receive the SAEVENT_SERVER_SIM_ON_REQ event.  The server should respond
         *  to this request by calling SIM_ServerSimOnRsp with the result of the
         *  action (see SimResultCode).  During this callback, the "SimCallbackParms.ptrs" 
         *  field is not valid.
         */
        case SAEVENT_SERVER_SIM_ON_REQ:
            BTL_LOG_INFO(("SAEVENT_SERVER_SIM_ON_REQ"));
            passEventToApp = HandleSimOnReq(&sapsEvent);
            break;

        /** When the client requests that the SIM card be turned off, the server will
         *  receive the SAEVENT_SERVER_SIM_OFF_REQ event.  The server should respond
         *  to this request by calling SIM_ServerSimOffRsp with the result of the
         *  action (see SimResultCode).  During this callback, the "SimCallbackParms.ptrs" 
         *  field is not valid.
         */
        case SAEVENT_SERVER_SIM_OFF_REQ:
            BTL_LOG_INFO(("SAEVENT_SERVER_SIM_OFF_REQ"));
            passEventToApp = HandleSimOffReq(&sapsEvent);
            break;

        /** When the client requests that the SIM card be reset, the server will
         *  receive the SAEVENT_SERVER_RESET_SIM_REQ event.  The server should respond
         *  to this request by calling SIM_ServerResetSimRsp with the result of the
         *  action (see SimResultCode).  During this callback, the "SimCallbackParms.ptrs" 
         *  field is not valid.
         */
        case SAEVENT_SERVER_RESET_SIM_REQ:
            BTL_LOG_INFO(("SAEVENT_SERVER_RESET_SIM_REQ"));
            passEventToApp = HandleResetSimReq(&sapsEvent);
            break;

        /** When the client requests the status of the card reader, the server will
         *  receive the SAEVENT_SERVER_CARD_READER_STATUS_REQ event.  The server should 
         *  respond to this request by calling SIM_ServerCardReaderStatusRsp with the 
         *  result of the action (see SimResultCode and SimCardReaderStatus).
         *  During this callback, the "SimCallbackParms.ptrs" field is not valid.
         */
        case SAEVENT_SERVER_CARD_READER_STATUS_REQ:
            BTL_LOG_INFO(("SAEVENT_SERVER_CARD_READER_STATUS_REQ"));
            passEventToApp = HandleCardReaderStatusReq(&sapsEvent);
            break;

        /** When the server sends the status of the SIM card, this event will indicate
         *  that the status has been sent successfully.  The SIM card status is the
         *  only unsolicited message that the server can send, therefore, it is 
         *  important to know when it is safe to send the command.  After receiving this
         *  event, the application can send another SIM card status if necessary.
         */
        case SAEVENT_SERVER_STATUS_SENT:
            BTL_LOG_INFO(("SAEVENT_SERVER_STATUS_SENT"));
            passEventToApp = FALSE;

            /* Amdinistrate that I have received this event, so a new CardStatus can be sent. */
            sapsContext->w4cardStatusSent = FALSE;

            /* Another transmission for sending a CardStatus is waiting? --> send it. */
            if(TRUE == sapsContext->sendCardStatus)
            {
                SendCardStatus(sapsContext);
            }
            break;

        /* Other (unexpected) event? --> do nothing. */
        default:
            BTL_LOG_INFO(("SapsCallback: unknown event from SIM profile"));
            break;
    }

    /* APP should be notified? --> do it. */
  	if (TRUE == passEventToApp)
  	{
        BTL_LOG_INFO(("SapsCallback: event passed to APP"));
        sapsContext->callback(&sapsEvent);

        /* Check if some after processing is necessary. */
        AfterProcessing(&sapsEvent);
  	}

    BTL_FUNC_END();
}

/*-------------------------------------------------------------------------------
 * AfterProcessing()
 *
 *    Check if there is some processing necessary for this 'sapsContext' due to
 *    the latest 'event' sent to the application.
 *
 * Parameters:
 *		sapsEvent [in] - Latest event, sent to the application.
 *
 * Returns:
 *    void
 */
static void AfterProcessing(BtlSapsEvent *sapsEvent)
{
    BtStatus status;

    BTL_FUNC_START("AfterProcessing");

    switch(sapsEvent->context->state)
    {
    case BTL_SAPS_STATE_DISABLING:
      /* Last disconnect event to the APP during disabling?                        */
      /*  --> notify APP that the disabling is finished + reset the context state. */
      if((sapsEvent->event == SAEVENT_SERVER_CLOSED) && (0 == sapsEvent->context->nrOfConnections))
      {
          /* Remove the registration from the SIM profile. */
          status = SIM_DeregisterServer(&(sapsEvent->context->serverAdmin));
        
          /* Verify if it was indeed a success, otherwise make a note. */
          BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status),
                         status, ("Disable of SAPS failed: (%s)",
                         pBT_Status(status)));

          SetContextState(sapsEvent->context, BTL_SAPS_STATE_IDLE);

          sapsEvent->event = SAPS_EVENT_DISABLED;
          sapsEvent->context->callback(sapsEvent);
      }
      break;
    }

    BTL_FUNC_END();
}

/*-------------------------------------------------------------------------------
 * BthalCallback()
 *
 *    Callback function for events from the BTHAL_SIM interface to the SIM card.
 *
 * Parameters:
 *		event [in] - ID of an event from the BTHAL_SIM.
 *
 *    result [in] - Result of the executed action on the SIM card.
 *
 * Returns:
 *    void
 */
static void BthalCallback(BthalSimEvent event,
                          SimResultCode result)
{
    BtlSapsContext *context;
    SimCardStatus   cardStatus;
    U16             len;
    U8             *data;
    
    BTL_FUNC_START("BthalCallback");

    /* Get the context for this event.*/
    context = GetContext();   

    switch (event)
    {
    case BTHAL_SIM_EVENT_APDU_RSP:
        BTL_LOG_INFO(("BTHAL_SIM_EVENT_APDU_RSP"));

        /* Valid APDU RSP present? --> read it */
        if(SIM_RESULT_OK == result)
        {
            /* Read the APDU response from the BTHAL. */
            (void) BTHAL_SIM_ReadApduRsp(&len, &data);
        }
        /* No APDU RSP data! --> make empty APDU RSP */
        else
        {
            BTL_LOG_INFO(("No valid APDU RSP; SIM result = %d",result));
            len = 0;
            data = NULL;

            /* ATTENTION: Workaround.                             */
            /* The BlueSDK has a problem with the values above.   */
            /* (See routine SIM_ServerApduRsp that will be called)*/
            /* A problem report has been made, in the meantime    */
            /* the next workaround is made.                       */
            (void) BTHAL_SIM_ReadApduRsp(&len, &data);
        }
        
        /* Send APDU response to the client.*/
        (void) SIM_ServerApduRsp(&(context->serverAdmin),len,data,result);                   
        break;
    case BTHAL_SIM_EVENT_STATUS_CHANGED:
        BTL_LOG_INFO(("BTHAL_SIM_EVENT_APDU_RSP"));
        /* Get new status. */
        (void) BTHAL_SIM_GetStatus(&cardStatus);

        /* Remote side is waiting for a response? --> send it.*/
        switch(context->bthalState)
        {
            case BTHAL_SIM_STATE_W4_SIM_ON_RSP:
                SetBthalState(context, BTHAL_SIM_STATE_IDLE);
                (void) SIM_ServerSimOnRsp(&(context->serverAdmin),result);
                break;
            case BTHAL_SIM_STATE_W4_SIM_OFF_RSP:
                SetBthalState(context, BTHAL_SIM_STATE_IDLE);
                (void) SIM_ServerSimOffRsp(&(context->serverAdmin),result);
                break;
            case BTHAL_SIM_STATE_W4_SIM_RESET_RSP:
                SetBthalState(context, BTHAL_SIM_STATE_IDLE);
                (void) SIM_ServerResetSimRsp(&(context->serverAdmin),result);
                break;
            /* No specific state? --> notify remote side for change. */
            case BTHAL_SIM_STATE_IDLE:
              
                break;

            /* Unknown state? --> do nothing */
            default:
                break;
        }

        /* Status was indeed changed? --> notify remote client. */
        if (context->cardStatus != cardStatus)
        {        
            /* Store new status. */
            context->cardStatus = cardStatus;

            /* Notify remote client.*/
            SendCardStatus(context);
        }

        break;

    /* Undefined event? --> do nothing. */               
    default:
        BTL_LOG_INFO(("BthalCallback: unknown event from SIM-card driver"));
        break;
    }

    BTL_FUNC_END();
}

/*-------------------------------------------------------------------------------
 * GetContext()
 *
 *    Get the SapsContext for this event from the SIM.
 *
 *    NOTE: only 1 SIM and 1 context (server) is supported.
 *    If it is necessary to support more the 1 SIM or that mulitple client can
 *    access the same SIM server in parallel, then this routine needs to be changed.
 *
 * Parameters:
 *
 * Returns:
 */
static BtlSapsContext *GetContext(void)
{   
    BTL_FUNC_START("GetContext");

    BTL_FUNC_END();

    /* Get the context for this event.                               */
    /* It is now a simple return of the first created context.       */
    /* So a second context is not able to communicate to the SIM yet.*/
    return ((BtlSapsContext*) &(btlSapsData.contextsMemory[0]));
}

/*-------------------------------------------------------------------------------
 * SetBthalState()
 *
 *    Fill the 'bthalState' parameter in the 'sapsContext' with the new 'state'.
 *
 * Parameters:
 *
 * Returns:
 */
static void SetBthalState(BtlSapsContext *sapsContext,
                          BthalSimState   state)
{   
    BTL_FUNC_START("SetBthalState");

    sapsContext->bthalState = (BthalSimState)state;

    /* DEBUG: do some logging. */
    BTL_SAPS_LOG_BTHAL_STATE(state);
    
    BTL_FUNC_END();
}

/*-------------------------------------------------------------------------------
 * SetContextState()
 *
 *    Fill the 'state' parameter in the 'sapsContext' with the new 'state'.
 *
 * Parameters:
 *
 * Returns:
 */
static void SetContextState(BtlSapsContext *sapsContext,
                            BtlSapsState    state)
{   
    BTL_FUNC_START("SetContextState");

    sapsContext->state = state;
    
    /* DEBUG: do some logging. */
    BTL_SAPS_LOG_CONTEXT_STATE(state);

    BTL_FUNC_END();
}

/* Some debug logging routines. */
#if (XA_DEBUG == XA_ENABLED) || (XA_DEBUG_PRINT == XA_ENABLED)
/*-------------------------------------------------------------------------------
 * LogContextState()
 *
 *    Display the 'state' variable in the logging window as readable text.
 *
 * Parameters:
 *    state[in] - the state of the context to be displayed.
 *
 * Returns:
 */
static void LogContextState(BtlSapsState state)
{
    switch (state)
    {
        case BTL_SAPS_STATE_IDLE: BTL_LOG_INFO(("BthalState changed: IDLE"));break;
        case BTL_SAPS_STATE_DISCONNECTED: BTL_LOG_INFO(("BthalState changed: DISCONNECTED"));break;
        case BTL_SAPS_STATE_CONNECTED: BTL_LOG_INFO(("BthalState changed: CONNECTED"));break;
        case BTL_SAPS_STATE_DISCONNECTING: BTL_LOG_INFO(("BthalState changed: DISCONNECTING"));break;
        case BTL_SAPS_STATE_DISABLING: BTL_LOG_INFO(("BthalState changed: DISABLING"));break;
        case BTL_SAPS_STATE_CONNECTING: BTL_LOG_INFO(("BthalState changed: CONNECTING"));break;
    }
}

/*-------------------------------------------------------------------------------
 * LogBthalState()
 *
 *    Display the 'state' variable in the logging window as readable text.
 *
 * Parameters:
 *    state[in] - the state of the communication to BTHAL_SIM to be displayed.
 *
 * Returns:
 */
static void LogBthalState(BthalSimState state)
{
    switch (state)
    {
        case BTHAL_SIM_STATE_IDLE: BTL_LOG_INFO(("BthalState changed: IDLE"));break;
        case BTHAL_SIM_STATE_W4_APDU_RSP: BTL_LOG_INFO(("BthalState changed: W4_APDU_RSP"));break;
        case BTHAL_SIM_STATE_W4_SIM_ON_RSP: BTL_LOG_INFO(("BthalState changed: W4_SIM_ON_RSP"));break;
        case BTHAL_SIM_STATE_W4_SIM_OFF_RSP: BTL_LOG_INFO(("BthalState changed: W4_SIM_OFF_RSP"));break;
        case BTHAL_SIM_STATE_W4_SIM_RESET_RSP: BTL_LOG_INFO(("BthalState changed: W4_SIM_RESET_RSP"));break;
    }
}
#endif /* (XA_DEBUG == XA_ENABLED) || (XA_DEBUG_PRINT == XA_ENABLED) */


	
BtStatus BtlSapsBtlNotificationsCb(BtlModuleNotificationType notificationType)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BtlSapsBtlNotificationsCb");
	
	BTL_LOG_DEBUG(("Notification Type: %d", notificationType));
	
	switch (notificationType)
	{
		case BTL_MODULE_NOTIFICATION_RADIO_ON:

			/* Do something if necessary */
		
			break;

		case BTL_MODULE_NOTIFICATION_RADIO_OFF:

			/* Disconnect all connections , stop all connections in establishment, and any other process in progress 
				in all contexts
			*/
			
			break;

		default:

			BTL_ERR(BT_STATUS_INTERNAL_ERROR, ("Invalid notification (%d)", notificationType));
			


	};

	BTL_FUNC_END();
	
	return status;
}



#else /*BTL_CONFIG_SAPS ==   BTL_CONFIG_ENABLED*/

/*-------------------------------------------------------------------------------
 * BTL_SAPS_Init() - When  BTL_CONFIG_SAPS is disabled.
 */
BtStatus BTL_SAPS_Init(void)
{
    
   BTL_LOG_INFO(("BTL_SAPS_Init()  -  BTL_CONFIG_SAPS Disabled"));
  

    return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 * BTL_SAPS_Deinit() - When  BTL_CONFIG_SAPS is disabled.
 */
BtStatus BTL_SAPS_Deinit(void)
{
    BTL_LOG_INFO(("BTL_SAPS_Deinit() -  BTL_CONFIG_SAPS Disabled"));

    return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------*/



#endif /*BTL_CONFIG_SAPS ==   BTL_CONFIG_ENABLED*/





/*******************************************************************************\
*                                                                              *
*  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION           *
*                                                                              *
*  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE         *
*  UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE       *
*  APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO       *
*  BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT        *
*  OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL         *
*  DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.          *
*                                                                              *
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      btl_l2cap.c
*
*   DESCRIPTION:    This file contains the BTL L2CAP implementation.
*
*   AUTHOR:         Gerrit SLot
*
\*******************************************************************************/

#include "btl_l2cap.h"
#include "btl_config.h"
#include "btl_defs.h"

BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_L2CAP);

#if BTL_CONFIG_BTL_L2CAP == BTL_CONFIG_ENABLED

/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/

#include "btl_pool.h"
#include "btl_commoni.h"
#include "l2cap.h"
#include "conmgr.h"


/********************************************************************************
 *
 * Constants
 *
 *******************************************************************************/

#define MAX_CHANNELS BTL_CONFIG_L2CAP_MAX_NUM_CHANNELS

/*-------------------------------------------------------------------------------
 * BSL_L2CAP_DEFAULT constant
 *
 *     Represents default security level for L2CAP
 */
#define BSL_L2CAP_DEFAULT  BSL_NO_SECURITY

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/* Forward declarations */
typedef struct _BtlL2capChannel  BtlL2capChannel;
typedef struct _BtlL2capData     BtlL2capData;

/*-------------------------------------------------------------------------------
 * BtlL2capInitState type
 *
 *     Defines the state of this complete module, whether it is initialized or not.
 */
typedef enum _BtlL2capInitState
{
    BTL_L2CAP_INIT_STATE_NOT_INTIALIZED,
    BTL_L2CAP_INIT_STATE_INITIALIZED,
    BTL_L2CAP_INIT_STATE_INITIALIZATION_FAILED,
    BTL_L2CAP_INIT_STATE_DEINITIALIZATION_FAILED
} BtlL2capInitState;

/*-------------------------------------------------------------------------------
 * BtlL2capContextState type
 *
 *     Defines the L2CAP state of a specific context (instance)
 */
typedef enum _BtlL2capContextState
{
    BTL_L2CAP_CONTEXT_STATE_IDLE,       /* Not in use, Waiting for a register (server) or connect (client) */
    BTL_L2CAP_CONTEXT_STATE_CLIENT,     /* In use as a client                    */
    BTL_L2CAP_CONTEXT_STATE_SERVER,     /* In use.as a server                    */
    BTL_L2CAP_CONTEXT_STATE_DISABLING   /* In process of deregistration (server) */
} BtlL2capContextState;

/*-------------------------------------------------------------------------------
 * BtlL2capChannelState type
 *
 *     Defines the state of a specific L2CAP channel.
 */
typedef enum _BtlL2capChannelState
{
    BTL_L2CAP_CHANNEL_STATE_DISCONNECTED,
    BTL_L2CAP_CHANNEL_STATE_DISCONNECTING,
    BTL_L2CAP_CHANNEL_STATE_CONNECT_IND,
    BTL_L2CAP_CHANNEL_STATE_CONNECTING,
    BTL_L2CAP_CHANNEL_STATE_CONNECTED
} BtlL2capChannelState;
 
/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BtlL2capChannel structure
 *
 *     Represents BTL L2CAP channel.
 */
struct _BtlL2capChannel
{
    /* L2CAP channel as exported in the API of BTL_L2CAP. */
    BtlL2capChannelId channelId; 

    /* L2CAP CID of this channel (see L2CAP protocol spec.). */
    L2capChannelId cid;

    /* Current state of the channel */
    BtlL2capChannelState state;

    /* Connection Manager Handler */
    CmgrHandler cmgrHandler;

    /* pointer to L2cap context */
    BtlL2capContext *l2capContext;
};

/*-------------------------------------------------------------------------------
 * BtlL2capContext structure
 *
 *     Represents BTL L2CAP context.
 */
struct _BtlL2capContext
{
    /* Must be first field */
    BtlContext base;
  
    /* Unique ID for this context. */
    U16 contextId;

    /* Current state of this context.  */
    BtlL2capContextState state;

    /* Associated callback with this context */
    BtlL2capCallBack callback;

    /* Registered PSM information (including connection configuration as well) */
    L2capPsm l2capPsm;

    /* Connection Settings */
    L2capConnSettings settings;

#if BT_SECURITY == XA_ENABLED
    /* Registered server security record (used only with the server) */
    BtSecurityRecord securityRecord;
#endif  /* BT_SECURITY == XA_ENABLED */
};

/*-------------------------------------------------------------------------------
 * BtlL2capData structure
 *
 *     Represents the data of the BTL L2CAP module.
 */
struct _BtlL2capData
{
    /* Pool of L2CAP contexts */
	  BTL_POOL_DECLARE_POOL(contextsPool, contextsMemory, BTL_CONFIG_L2CAP_MAX_NUM_OF_CONTEXTS, sizeof(BtlL2capContext));

    /* List of active L2CAP contexts */
    ListEntry contextsList;

	  BTL_POOL_DECLARE_POOL(packetsPool, packetsMemory, BTL_CONFIG_L2CAP_MAX_NUM_OF_TX_PACKETS, sizeof(BtPacket));

    /* Event passed to the application */
    BtlL2capEvent event;
};

/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * btlL2capInitState
 *
 *     Represents the current init state of L2CAP module.
 */
static BtlL2capInitState btlL2capInitState = BTL_L2CAP_INIT_STATE_NOT_INTIALIZED;

/*-------------------------------------------------------------------------------
 * btlL2capData
 *
 *     Represents the data of L2CAP module.
 */
static BtlL2capData btlL2capData;

/*-------------------------------------------------------------------------------
 * btlL2capContextsPoolName
 *
 *     Represents the name of the L2CAP contexts pool.
 */
static const char btll2capContextsPoolName[] = "l2capContexts";

/*-------------------------------------------------------------------------------
 * btlL2capPacketsPoolName
 *
 *     Represents the name of the L2CAP packets pool.
 */
static const char btlL2capPacketsPoolName[] = "L2capPackets";

/*-------------------------------------------------------------------------------
 * btlChannelAdmin
 *
 *     Mapping of the channelId (array index) to a specific channel administration
 *     When the 'state' parameter in this admin is
 *     BTL_L2CAP_CHANNEL_STATE_DISCONNECTED then this channel admin is not in use.
 */
static BtlL2capChannel btlChannelAdmin[MAX_CHANNELS];

/*-------------------------------------------------------------------------------
 * btlContextsAdmin
 *
 *     Mapping of the contextId (array index) to a specific context.
 *     This 'contextId' is stored in the psm administration of the ESI stack
 *     (L2capPsm) in order to make it possible to select the correct context
 *     when receiving specific (non-CID related) events from L2CAP.
 */
static BtlL2capContext* btlContextsAdmin[BTL_CONFIG_L2CAP_MAX_NUM_OF_CONTEXTS];

/********************************************************************************
 *
 * Internal function prototypes
 *
 *******************************************************************************/
static U16 GetChannelIndex(BtlL2capContext *l2capContext,
                           BtlL2capChannel *btlChannel);
static BtStatus DeregisterPsm(BtlL2capContext *l2capContext);
static BtStatus ConnectL2cap(BtlL2capContext *l2capContext,
                             BtlL2capChannel *btlChannel);
static void BtlL2capCmgrCallback(CmgrHandler *Handler,
                                 CmgrEvent Event,
                                 BtStatus Status);
static void BtlL2capCallback(L2capChannelId channelId,
                             L2capCallbackParms *parms);
static BOOL AllocContext(BtlL2capContext *l2capContext);
static BtlL2capContext *GetContext(L2capCallbackParms *parms);
static BOOL FreeContext(BtlL2capContext *l2capContext);
static BtlL2capChannel *GetChannelAdmin(L2capChannelId cid);
static BtlL2capChannel *GetFreeChannelAdmin(void);
static void FillChannelId(L2capChannelId cid);
static void SetChannelState(BtlL2capChannel *btlChannel,
                            BtlL2capChannelState  state);
static void SetContextState(BtlL2capContext *l2capContext,
                            BtlL2capContextState  state);
static BOOL HandlePacketHandled(L2capChannelId cid,
                                L2capCallbackParms *parms);
static BOOL HandleDisconnected(BtlL2capContext *l2capContext,
                               L2capChannelId cid,
                               L2capCallbackParms *parms);
static BOOL HandleDisconInd(BtlL2capContext *l2capContext,
                            L2capChannelId cid,
                            L2capCallbackParms *parms);
static BOOL HandleConnectInd(BtlL2capContext *l2capContext,
                             L2capChannelId cid,
                             L2capCallbackParms *parms);
static BOOL HandleConnected(L2capChannelId cid,
                            L2capCallbackParms *parms);
static BOOL HandleComplete(L2capCallbackParms *parms);
static BtStatus RegisterPsm(BtlL2capContext *l2capContext);
static void SendL2capDisabled(BtlL2capContext *l2capContext);
static void AfterProcessing(BtlL2capEvent *l2capEvent);
static BtStatus BtlL2capBtlNotificationsCb(BtlModuleNotificationType notificationType);


/* Some debug logging features. */
#if (XA_DEBUG == XA_ENABLED) || (XA_DEBUG_PRINT == XA_ENABLED)
static void LogChannelState(BtlL2capChannelState state);
#define BTL_L2CAP_LOG_CHANNEL_STATE(state) LogChannelState(state)
static void LogContextState(BtlL2capContextState state);
#define BTL_L2CAP_LOG_CONTEXT_STATE(state) LogContextState(state)
#else
/* Debugging disabled --> define empty macros */
#define BTL_L2CAP_LOG_CONTEXT_STATE(state)
#endif /* (XA_DEBUG == XA_ENABLED) || (XA_DEBUG_PRINT == XA_ENABLED) */

/********************************************************************************
 *
 * Function definitions
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BTL_L2CAP_Init()
 */
BtStatus BTL_L2CAP_Init(void)
{
    BtStatus status = BT_STATUS_SUCCESS;
    U16      idx;

    BTL_FUNC_START_AND_LOCK_COMMON("BTL_L2CP_Init");

    BTL_VERIFY_ERR((BTL_L2CAP_INIT_STATE_NOT_INTIALIZED == btlL2capInitState),
                    BT_STATUS_FAILED,
                    ("L2CAP module is already initialized"));

    btlL2capInitState = BTL_L2CAP_INIT_STATE_INITIALIZATION_FAILED;

    status = BTL_POOL_Create(&btlL2capData.contextsPool,
                             btll2capContextsPoolName,
                             btlL2capData.contextsMemory,
                             BTL_CONFIG_L2CAP_MAX_NUM_OF_CONTEXTS,
                             sizeof(BtlL2capContext));
    BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status),
                      BT_STATUS_INTERNAL_ERROR,
                      ("L2CAP contexts pool creation failed"));

    InitializeListHead(&btlL2capData.contextsList);

    status = BTL_POOL_Create(&btlL2capData.packetsPool,
                             btlL2capPacketsPoolName,
                             btlL2capData.packetsMemory,
                             BTL_CONFIG_L2CAP_MAX_NUM_OF_TX_PACKETS,
                             sizeof(BtPacket));
    BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status),
                     BT_STATUS_INTERNAL_ERROR,
                     ("L2CAP packets pool creation failed"));

    btlL2capInitState = BTL_L2CAP_INIT_STATE_INITIALIZED;

    /* Clear administrations. */
    for (idx = 0; idx < BTL_CONFIG_L2CAP_MAX_NUM_OF_CONTEXTS; idx++)
    {
        btlContextsAdmin[idx] = NULL;
    }

    /* Init L2CAP channel administrations. */
    for (idx = 0; idx < MAX_CHANNELS; idx++)
    {
        btlChannelAdmin[idx].channelId = idx; /* for reading my own channelId. */
        btlChannelAdmin[idx].state = BTL_L2CAP_CHANNEL_STATE_DISCONNECTED;
        btlChannelAdmin[idx].cid = L2CID_INVALID;
    }

    BTL_RegisterForCommonNotifications(BTL_MODULE_TYPE_L2CAP, BtlL2capBtlNotificationsCb);

    BTL_FUNC_END_AND_UNLOCK();

    return (status);
}

/*-------------------------------------------------------------------------------
 * BTL_L2CAP_Deinit()
 */
BtStatus BTL_L2CAP_Deinit(void)
{
    BtStatus status = BT_STATUS_SUCCESS;

    BTL_FUNC_START_AND_LOCK_COMMON("BTL_L2CAP_Deinit");

    BTL_VERIFY_ERR((BTL_L2CAP_INIT_STATE_INITIALIZED == btlL2capInitState),
                   BT_STATUS_FAILED,
                   ("L2CAP module is not initialized"));

    btlL2capInitState = BTL_L2CAP_INIT_STATE_DEINITIALIZATION_FAILED;

    BTL_VERIFY_ERR((IsListEmpty(&btlL2capData.contextsList)),
                   BT_STATUS_FAILED,
                   ("L2CAP contexts are still active"));

    status = BTL_POOL_Destroy(&btlL2capData.contextsPool);
    BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status),
                     BT_STATUS_INTERNAL_ERROR,
                     ("L2CAP contexts pool destruction failed"));

    status = BTL_POOL_Destroy(&btlL2capData.packetsPool);
    BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status),
                     BT_STATUS_INTERNAL_ERROR,
                     ("L2CAP packets pool destruction failed"));

    btlL2capInitState = BTL_L2CAP_INIT_STATE_NOT_INTIALIZED;

    BTL_FUNC_END_AND_UNLOCK();

    return (status);
}

/*-------------------------------------------------------------------------------
 * BTL_L2CAP_Create()
 */
BtStatus BTL_L2CAP_Create(BtlAppHandle *appHandle,
                          const BtlL2capCallBack l2capCallback,
                          const BtSecurityLevel *securityLevel,
						              const SdpServicesMask service,
                          BtlL2capContext **l2capContext)
{
    BtStatus        status = BT_STATUS_SUCCESS;
    BOOL            result;
    BtSecurityLevel secLevel = BSL_L2CAP_DEFAULT;

    BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_L2CAP_Create");

    BTL_VERIFY_ERR((NULL != l2capCallback), BT_STATUS_INVALID_PARM, ("Null l2capCallback"));
    BTL_VERIFY_ERR((NULL != l2capContext), BT_STATUS_INVALID_PARM, ("Null l2capContext"));

#if BT_SECURITY == XA_ENABLED
    /* Security level defined? --> overrule default. */
    if (NULL != securityLevel)
    {
        secLevel = *securityLevel;
        status = BTL_VerifySecurityLevel(secLevel);
        BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Invalid L2CAP securityLevel"));
    }
#endif  /* BT_SECURITY == XA_ENABLED */

    /* Allocate memory for a unique new L2CAP context */
    status = BTL_POOL_Allocate(&btlL2capData.contextsPool, (void **)l2capContext);
    BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed allocating L2CAP context"));

    /* Create some common (applicable to all contexts) context administration. */
    status = BTL_HandleModuleInstanceCreation(appHandle, BTL_MODULE_TYPE_L2CAP, &(*l2capContext)->base);

    /* Creation failed? --> clean up. */
    if (BT_STATUS_SUCCESS != status)
    {
        BtStatus retVal = BTL_POOL_Free(&btlL2capData.contextsPool, (void **)l2capContext);
        BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == retVal), ("Failed freeing L2CAP context"));

        /* This will handle the exception gracefully */
        BTL_ERR(status, ("Failed handling L2CAP instance creation"));
    }

    /* Save the given callback */
    (*l2capContext)->callback = l2capCallback;

    /* Add the new L2CAP context to the active contexts list */
    InsertTailList(&btlL2capData.contextsList, &((*l2capContext)->base.node));

    /* Add context to my own index administration. */
    result = AllocContext(*l2capContext);
    BTL_VERIFY_ERR((TRUE == result), BT_STATUS_NO_RESOURCES, ("Context administration full."));

#if BT_SECURITY == XA_ENABLED
    /* Create the security record */
    (*l2capContext)->securityRecord.id = SEC_L2CAP_ID;
    (*l2capContext)->securityRecord.service = service;

    /* Update the security level. */
    (*l2capContext)->securityRecord.level = secLevel;
#endif  /* BT_SECURITY == XA_ENABLED */

    /* Further inits' of this context. */
    SetContextState(*l2capContext, BTL_L2CAP_CONTEXT_STATE_IDLE);

    BTL_FUNC_END_AND_UNLOCK();

    return (status);
}

/*-------------------------------------------------------------------------------
 * BTL_L2CAP_Destroy()
 */
BtStatus BTL_L2CAP_Destroy(BtlL2capContext **l2capContext)
{
    BtStatus status;
    BOOL     result;
    BOOL     isAllocated = FALSE;

    BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_L2CAP_Destroy");

    BTL_VERIFY_ERR((NULL != l2capContext), BT_STATUS_INVALID_PARM, ("Null l2capContext"));
    BTL_VERIFY_ERR((NULL != *l2capContext), BT_STATUS_INVALID_PARM, ("Null *l2capContext"));

    status = BTL_POOL_IsElelementAllocated(&btlL2capData.contextsPool, *l2capContext, &isAllocated);
    BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed locating given L2CAP context"));
    BTL_VERIFY_ERR((TRUE == isAllocated), BT_STATUS_INVALID_PARM, ("Invalid L2CAP context"));

    /* Remove context from my own index administration. */
    result = FreeContext(*l2capContext);
    BTL_VERIFY_ERR((TRUE != result), BT_STATUS_INVALID_PARM, ("Context was not present in administration."));

    /* Remove the context from the list of all L2CAP contexts */
    RemoveEntryList(&((*l2capContext)->base.node));

    status = BTL_HandleModuleInstanceDestruction(&(*l2capContext)->base);
    BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed handling L2CAP instance destruction"));

    status = BTL_POOL_Free(&btlL2capData.contextsPool, (void **)l2capContext);
    BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("Failed freeing L2CAP context"));

    /* Set the L2CAP context to NULL */
    *l2capContext = NULL;

    BTL_FUNC_END_AND_UNLOCK();

    return (status);
}

#if BT_SECURITY == XA_ENABLED
/*-------------------------------------------------------------------------------
 * BTL_L2CAP_SetSecurityLevel()
 */
BtStatus BTL_L2CAP_SetSecurityLevel(BtlL2capContext *l2capContext,
                                    const BtSecurityLevel *securityLevel)

{
    BtStatus status = BT_STATUS_SUCCESS;

    BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_L2CAP_SetSecurityLevel");

    BTL_VERIFY_ERR((NULL != l2capContext), BT_STATUS_INVALID_PARM, ("Null l2capContext"));

    if (NULL != securityLevel)
    {
        status = BTL_VerifySecurityLevel(*securityLevel);
        BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Invalid L2CAP securityLevel"));
    }

    /* Security level present? --> fill it. */
    if (NULL != securityLevel)
    {
        l2capContext->securityRecord.level = *securityLevel;
    }
    /* Security level absent! --> fill default value. */
    else
    {
        l2capContext->securityRecord.level = BSL_L2CAP_DEFAULT;
    }
  
    BTL_FUNC_END_AND_UNLOCK();

    return (status);
}

/*-------------------------------------------------------------------------------
 * BTL_L2CAP_GetSecurityLevel()
 */
BtStatus BTL_L2CAP_GetSecurityLevel(BtlL2capContext *l2capContext,
                                    BtSecurityLevel *securityLevel)
{
  
    BtStatus status = BT_STATUS_SUCCESS;

    BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_L2CAP_GetSecurityLevel");

    BTL_VERIFY_ERR((NULL != l2capContext), BT_STATUS_INVALID_PARM, ("Null l2capContext"));
    BTL_VERIFY_ERR((NULL != securityLevel), BT_STATUS_INVALID_PARM, ("Null securityLevel"));

    *securityLevel = l2capContext->securityRecord.level;

    BTL_FUNC_END_AND_UNLOCK();

    return (status);
}
#endif  /* BT_SECURITY == XA_ENABLED */

/*-------------------------------------------------------------------------------
 * BTL_L2CAP_GetConnectedDevice()
 */			                					  	
BtStatus BTL_L2CAP_GetConnectedDevice(BtlL2capContext *l2capContext, 
                                      BtlL2capChannelId channelId,
			          							        BD_ADDR *bdAddr)
{
    BtStatus         status = BT_STATUS_SUCCESS;
    BtlL2capChannel *btlChannel;
    BtRemoteDevice  *remDev = 0;

    BTL_FUNC_START_AND_LOCK("BTL_l2CAP_GetConnectedDevice");

    BTL_VERIFY_ERR((NULL != l2capContext), BT_STATUS_INVALID_PARM, ("Null l2capContext"));
    BTL_VERIFY_ERR((NULL != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));
    BTL_VERIFY_ERR((channelId < MAX_CHANNELS), BT_STATUS_INVALID_PARM, ("Invalid channelId"));

    btlChannel = &(btlChannelAdmin[channelId]);
  
    remDev = CMGR_GetRemoteDevice(&(btlChannel->cmgrHandler));
    BTL_VERIFY_ERR((0 != remDev), BT_STATUS_NO_CONNECTION, ("ACL link is not connected!"));
  
    BTL_VERIFY_ERR(((BTL_L2CAP_CHANNEL_STATE_CONNECTED == btlChannel->state) ),
                   BT_STATUS_NO_CONNECTION,
                   ("Channel %d is not connected", channelId));
  
    /* If connected return the BdAddr from the remote device*/
    *bdAddr = remDev->bdAddr;
    
    BTL_VERIFY_FATAL((0 != remDev),
                     BT_STATUS_NO_CONNECTION,
                     ("Unable to find connected remote device"));
  
    BTL_FUNC_END_AND_UNLOCK();

    return (status);
}

/*-------------------------------------------------------------------------------
 * BTL_L2CAP_RegisterPsm()
 */
BtStatus BTL_L2CAP_RegisterPsm(BtlL2capContext *l2capContext,
                               BtlL2capChannelSettings *channelSettings,
                               L2capPsmValue *psm)
{
    BtStatus status = BT_STATUS_SUCCESS;
 
    BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_L2CAP_RegisterPsm");

    BTL_VERIFY_ERR((NULL != l2capContext), BT_STATUS_INVALID_PARM, ("Null l2capContext"));
    BTL_VERIFY_ERR((NULL != channelSettings), BT_STATUS_INVALID_PARM, ("Null channelSettings"));

    /* Context acts as a server */
    SetContextState(l2capContext, BTL_L2CAP_CONTEXT_STATE_SERVER);

    /* Set the internal L2CAP channel fields */
    l2capContext->l2capPsm.callback = BtlL2capCallback;
    l2capContext->l2capPsm.psm = *psm;
    l2capContext->l2capPsm.localMtu = channelSettings->localMtu;
    l2capContext->l2capPsm.minRemoteMtu = channelSettings->minRemoteMtu;
    l2capContext->l2capPsm.contextId = l2capContext->contextId;

    /* Do the necessary registration to the ESI stack. */
    status = RegisterPsm(l2capContext);

		if((BT_STATUS_SUCCESS == status) && (*psm == BT_DYNAMIC_PSM))
		{
			*psm = l2capContext->l2capPsm.psm;
		}
		  
    BTL_FUNC_END_AND_UNLOCK();

    return (status);
}

/*-------------------------------------------------------------------------------
 * BTL_L2CAP_DeregisterPsm()
 */
BtStatus BTL_L2CAP_DeregisterPsm(BtlL2capContext *l2capContext)
{
    BtStatus status = BT_STATUS_PENDING;

    BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_L2CAP_DeregisterPsm");
  
    BTL_VERIFY_ERR((NULL != l2capContext), BT_STATUS_INVALID_PARM, ("Null l2capContext"));

    /* I am busy with the deregistration process. */
    SetContextState(l2capContext, BTL_L2CAP_CONTEXT_STATE_DISABLING);

    /* Remove registration from L2CAP. */
	  status = DeregisterPsm(l2capContext);

    /* Finished? --> notify APP with L2CAP_EVENT_DISABLED */
    if (BT_STATUS_SUCCESS == status)
    {
        /* I am finished with the deregistration process. */
        SetContextState(l2capContext, BTL_L2CAP_CONTEXT_STATE_IDLE);

        SendL2capDisabled(l2capContext);
    }
   
    BTL_FUNC_END_AND_UNLOCK();

    return (status);
}

/*-------------------------------------------------------------------------------
 * BTL_L2CAP_Connect()
 */
BtStatus BTL_L2CAP_Connect(BtlL2capContext *l2capContext,
                           BD_ADDR *bdAddr,
                           L2capPsmValue remotePsm,
                           L2capConnSettings *connectSettings,
                           BtlL2capChannelSettings *channelSettings,
                           BtlL2capChannelId *channelId)
{
    BtStatus        status = BT_STATUS_PENDING;
    BtStatus        retVal;
    BtlL2capChannel *btlChannel;

    BTL_FUNC_START_AND_LOCK("BTL_L2CAP_Connect");

    BTL_VERIFY_ERR((NULL != l2capContext), BT_STATUS_INVALID_PARM, ("Null l2capContext"));
    BTL_VERIFY_ERR((NULL != bdAddr), BT_STATUS_INVALID_PARM, ("Null remoteDevice"));
    BTL_VERIFY_ERR((NULL != connectSettings), BT_STATUS_INVALID_PARM, ("Null connectSettings"));
    BTL_VERIFY_ERR((BTL_L2CAP_CONTEXT_STATE_IDLE == l2capContext->state), BT_STATUS_IN_USE, ("Context already in use."));

    /* Get first available channelAdmin */
    btlChannel = GetFreeChannelAdmin();
    BTL_VERIFY_ERR((NULL != btlChannel), BT_STATUS_NO_RESOURCES, ("Null btlChannel"));

    /* Context acts as a client */
    SetContextState(l2capContext, BTL_L2CAP_CONTEXT_STATE_CLIENT);
  
    /* Fill channelAdmin. */
    btlChannel->l2capContext = l2capContext;

    /* Copy connect info into this admin. */
    l2capContext->l2capPsm.callback = BtlL2capCallback;
/* gesl: TODO: INVESTIGATE: which one is correct below?  l2capContext->l2capPsm.psm = BT_CLIENT_ONLY_PSM; */
    l2capContext->l2capPsm.psm = remotePsm;
    l2capContext->l2capPsm.localMtu = channelSettings->localMtu;
    l2capContext->l2capPsm.minRemoteMtu = channelSettings->minRemoteMtu;
    l2capContext->l2capPsm.contextId = l2capContext->contextId;
    if (NULL != connectSettings)
    {
      l2capContext->settings = *connectSettings;
    }

    /* Register to the ESI stack.                                                             */
    /*  This action is included in this Connect function because it is not logical from a API */
    /*  perspective to register a PSM before setting up a connection as a client.             */
    status = RegisterPsm(l2capContext);
    BTL_VERIFY_ERR((status == BT_STATUS_SUCCESS), status, ("Internal (client) PSM registration failed, status = ", status));
		  
    /* Register CMGR handler */
    status = CMGR_RegisterHandler(&btlChannel->cmgrHandler, BtlL2capCmgrCallback);
    BTL_VERIFY_ERR((status == BT_STATUS_SUCCESS), status, ("Failed registering CMGR handler, status = ", status));

    /* Create ACL link */
    status = CMGR_CreateDataLink(&btlChannel->cmgrHandler, bdAddr);

    /* Immediate success, so there was already an ACL link? --> open L2CAP channel.*/
    if (status == BT_STATUS_SUCCESS)
    {
        status = ConnectL2cap(l2capContext, btlChannel);

        /* Failed? --> cleanup */
        if (status != BT_STATUS_PENDING)
        {
            /* Clear Context admin for next use. */
            SetContextState(l2capContext, BTL_L2CAP_CONTEXT_STATE_IDLE);

            /* ACL link was established? --> remove it. */
            if (CMGR_IsLinkUp(&btlChannel->cmgrHandler) == TRUE)
            {
                retVal = CMGR_RemoveDataLink(&btlChannel->cmgrHandler);
                if (retVal != BT_STATUS_SUCCESS)
                {
                    BTL_LOG_ERROR(("Failed removing data link, status = ", retVal));
                }
            }

            /* Release CMGR handler */
            retVal = CMGR_DeregisterHandler(&btlChannel->cmgrHandler);
            if (retVal != BT_STATUS_SUCCESS)
            {
                BTL_LOG_ERROR(("Failed deregistering CMGR handler, status = ", retVal));
            }
        }
/* gesl: redundant code below */
          /* PENDING! --> I am in a connection process now.*/
/*        else
          {
              SetChannelState(btlChannel,BTL_L2CAP_CHANNEL_STATE_CONNECTING);
          } */
    }

    /* Setup of ACL link has failed? --> cleanup. */
    else if (status != BT_STATUS_PENDING)
    {
        /* Release CMGR handler */
        (void)CMGR_DeregisterHandler(&btlChannel->cmgrHandler);
        BTL_LOG_ERROR(("Failed creating outgoing ACL link, status = %d", status));
    }

 	  /* Return channelId to app */
    *channelId = btlChannel->channelId;

    /* Busy setting up an ACL link? --> update channel state */
    if (status == BT_STATUS_PENDING)
    {
        SetChannelState(btlChannel,BTL_L2CAP_CHANNEL_STATE_CONNECTING);
    }

    BTL_FUNC_END_AND_UNLOCK();

    return (status);
}

/*-------------------------------------------------------------------------------
 * BTL_L2CAP_ConnectResponse()
 */
BtStatus BTL_L2CAP_ConnectResponse(BtlL2capContext *l2capContext, 
					        				         BtlL2capChannelId channelId,
									                 L2capConnStatus responseCode,
                                   L2capConnSettings *connectSettings)
{
    BtStatus         status = BT_STATUS_PENDING;
    BtlL2capChannel *btlChannel;

    BTL_FUNC_START_AND_LOCK("BTL_L2CAP_ConnectResponse"); 

    BTL_VERIFY_ERR((NULL != l2capContext), BT_STATUS_INVALID_PARM, ("Null l2capContext"));
    BTL_VERIFY_ERR((channelId < MAX_CHANNELS), BT_STATUS_INVALID_PARM, ("Invalid channelId"));

    /* Get correct channelAdmin */
    btlChannel = &(btlChannelAdmin[channelId]);
  
    BTL_VERIFY_ERR((BTL_L2CAP_CHANNEL_STATE_CONNECT_IND == btlChannel->state),
                   BT_STATUS_FAILED,
                   ("Server channel %d is not connect indicated", channelId));

    /* Tell L2CAP what the APP has decided on thsi incoming connection.*/
    status = L2CAP_ConnectRsp(btlChannel->cid, responseCode, connectSettings);

    /* Connection not accepted? --> cleanup channelAdmin for later re-use.*/
    if ((L2CONN_ACCEPTED != responseCode) && (L2CONN_PENDING != responseCode))
    {
        btlChannel->state = BTL_L2CAP_CHANNEL_STATE_DISCONNECTED;
        btlChannel->cid = L2CID_INVALID;
    }

    BTL_VERIFY_ERR((BT_STATUS_PENDING == status),
                   status,
                   ("Failed responding to connection indication %d", channelId));
 
    BTL_FUNC_END_AND_UNLOCK();

    return (status);
}

/*-------------------------------------------------------------------------------
 * BTL_L2CAP_Disconnect()
 */
BtStatus BTL_L2CAP_Disconnect(BtlL2capContext *l2capContext,
                              BtlL2capChannelId channelId)
{
    BtStatus         status = BT_STATUS_PENDING;
    BtlL2capChannel *btlChannel;

    BTL_FUNC_START_AND_LOCK("BTL_L2CAP_Disconnect");

    BTL_VERIFY_ERR((NULL != l2capContext), BT_STATUS_INVALID_PARM, ("Null l2capContext"));
    BTL_VERIFY_ERR((channelId < MAX_CHANNELS), BT_STATUS_INVALID_PARM, ("Invalid channelId"));

    /* Get correct channelAdmin */
    btlChannel = &(btlChannelAdmin[channelId]);

    BTL_VERIFY_ERR(((BTL_L2CAP_CHANNEL_STATE_CONNECTED == btlChannel->state)),
                   BT_STATUS_FAILED,
                   ("Channel %d is not connected", channelId));

    status = L2CAP_DisconnectReq(btlChannel->cid);
    BTL_VERIFY_ERR((BT_STATUS_PENDING == status), status, ("Failed closing channel %d", channelId));

    SetChannelState(btlChannel,BTL_L2CAP_CHANNEL_STATE_DISCONNECTING);
  
    BTL_FUNC_END_AND_UNLOCK();

    return (status);
}

/*-------------------------------------------------------------------------------
 * BTL_L2CAP_SendData()
 */
BtStatus BTL_L2CAP_SendData(BtlL2capContext *l2capContext,
                            BtlL2capChannelId channelId,
                            U8 *data, 
                            U16 dataLen,
                            BtPacket **packet)
{
    BtStatus         status = BT_STATUS_PENDING;
    BtStatus         retVal;
    BtlL2capChannel *btlChannel;

    BTL_FUNC_START_AND_LOCK("BTL_L2CAP_SendData");

    BTL_VERIFY_ERR((NULL != l2capContext), BT_STATUS_INVALID_PARM, ("Null l2capContext"));
    BTL_VERIFY_ERR((channelId < MAX_CHANNELS), BT_STATUS_INVALID_PARM, ("Invalid channelId"));
    BTL_VERIFY_ERR((NULL != data), BT_STATUS_INVALID_PARM, ("Null data"));
    BTL_VERIFY_ERR((NULL != packet), BT_STATUS_INVALID_PARM, ("Null packet"));

    /* Get correct channeAdmin. */
    btlChannel = &(btlChannelAdmin[channelId]);

    BTL_VERIFY_ERR(((BTL_L2CAP_CHANNEL_STATE_CONNECTED == btlChannel->state)),
                   BT_STATUS_NO_CONNECTION,
                   ("Channel %d is not connected", channelId));

    /* Allocate memory for a unique new L2CAP TX packet */
    status = BTL_POOL_Allocate(&btlL2capData.packetsPool, (void **)packet);
    BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed allocating TX packet"));

    /* Set up the TX packet */
    (*packet)->headerLen = 0;
    (*packet)->data = data;
    (*packet)->dataLen = dataLen;
    (*packet)->flags = BTP_FLAG_NONE;

    /* Send packet via L2CAP */
    status = L2CAP_SendData(btlChannel->cid, *packet);
    
    /* Sending failed? --> free the packet */
    if (status != BT_STATUS_PENDING)
    {
        retVal = BTL_POOL_Free(&btlL2capData.packetsPool, (void **)packet);
        BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == retVal), BT_STATUS_INTERNAL_ERROR, ("Failed freeing TX packet"));
    }

    BTL_FUNC_END_AND_UNLOCK();

    return (status);
}

#if L2CAP_PING_SUPPORT == XA_ENABLED
/*-------------------------------------------------------------------------------
 * BTL_L2CAP_Ping()
 */
BtStatus BTL_L2CAP_Ping(BtlL2capContext *l2capContext,
                        BD_ADDR *bdAddr,
                        const U8 *data, 
                        U16 dataLen)
{
    BtStatus        status = BT_STATUS_PENDING;
    BtRemoteDevice *remDev;
  
    BTL_FUNC_START_AND_LOCK("BTL_L2CAP_Ping");

    BTL_VERIFY_ERR((NULL != l2capContext), BT_STATUS_INVALID_PARM, ("Null l2capContext"));
    BTL_VERIFY_ERR((NULL != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));
    BTL_VERIFY_ERR((NULL != data), BT_STATUS_INVALID_PARM, ("Null data"));

    remDev = ME_FindRemoteDeviceP((U8*) bdAddr);
    status = L2CAP_Ping(&l2capContext->l2capPsm, remDev, data, dataLen);
  
    BTL_VERIFY_ERR((status == BT_STATUS_PENDING), status, ("Failed Ping"));
    BTL_VERIFY_ERR((status == BT_STATUS_NO_RESOURCES), status, ("Failed Ping, no recources"));
    BTL_VERIFY_ERR((status == BT_STATUS_NO_CONNECTION), status, ("Failed Ping, No connection"));
 
    BTL_FUNC_END_AND_UNLOCK();

    return (status);
}
#endif

#if L2CAP_GET_INFO_SUPPORT == XA_ENABLED
/*-------------------------------------------------------------------------------
 * BTL_L2CAP_GetInfo()
 */
BtStatus BTL_L2CAP_GetInfo(BtlL2capContext *l2capContext,
                           BD_ADDR *bdAddr, 
                           L2capInfoType type)
{
	  BtStatus        status = BT_STATUS_SUCCESS;
    BtRemoteDevice *remDev = NULL;
	  
	  BTL_FUNC_START_AND_LOCK("BTL_L2CAP_GetInfo");

	  BTL_VERIFY_ERR((NULL != l2capContext), BT_STATUS_INVALID_PARM, ("Null l2capContext"));
    BTL_VERIFY_ERR((NULL != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));
	  BTL_VERIFY_ERR((0 != type), BT_STATUS_INVALID_PARM, ("Null type"));

    remDev = ME_FindRemoteDeviceP((U8*) bdAddr);
	  status = L2CAP_GetInfo(&l2capContext->l2capPsm, remDev, type);

    BTL_FUNC_END_AND_UNLOCK();
	  
	  return (status);
}
#endif /* L2CAP_GET_INFO_SUPPORT */

/*-------------------------------------------------------------------------------
 * BTL_L2CAP_GetChannelConfig()
 */
BtStatus BTL_L2CAP_GetChannelConfig(BtlL2capContext *l2capContext,
                                    BtlL2capChannelId channelId, 
                                    BtlL2capChannelConfig *channelConfig)
{
	  BtStatus         status = BT_STATUS_NO_CONNECTION;
	  BtlL2capChannel *btlChannel;
	  
    BTL_FUNC_START_AND_LOCK("BTL_L2CAP_GetChannelConfig");

    BTL_VERIFY_ERR((NULL != l2capContext), BT_STATUS_INVALID_PARM, ("Null l2capContext"));
	  
    /* Get correct channelAdmin */
    btlChannel = &(btlChannelAdmin[channelId]);
	  
	  BTL_VERIFY_ERR(((BTL_L2CAP_CHANNEL_STATE_CONNECTED == btlChannel->state)),
                   BT_STATUS_NO_CONNECTION,
                   ("Channel %d is not connected", channelId));

	  /* Get the configuration settings */
    channelConfig->txMtu = L2CAP_GetTxMtu(btlChannel->cid);

    /* L2CAP admin tells that the CID is not open? --> return error. */
	  if (0 == channelConfig->txMtu)
    { 
  		  status = BT_STATUS_FAILED;
    }
    /* Internal admin is OK as well! --> return success. */
	  else
    { 
		    status = BT_STATUS_SUCCESS;
    }
    
    BTL_FUNC_END_AND_UNLOCK();
  
	  return (status);
}

/*******************************************************************************************************
*
*								Internal functions
*
*********************************************************************************************************/

/*-------------------------------------------------------------------------------
 * RegisterPsm()
 */
/*-------------------------------------------------------------------------------
 * RegisterPsm()
 *
 *    Do the necessary registrations to the ESI stack.
 *    It registers the l2capPsm data + it connects a security handler
 *    to this connection when it is enabled.
 *
 * Parameters:
 *		l2capContext [in] - context for processing this event
 *
 * Returns:
 *		status
 */
static BtStatus RegisterPsm(BtlL2capContext *l2capContext)
{
    BtStatus status = BT_STATUS_SUCCESS;
 
    BTL_FUNC_START("RegisterPsm");

    BTL_VERIFY_ERR((NULL != l2capContext), BT_STATUS_INVALID_PARM, ("Null l2capContext"));

    status = L2CAP_RegisterPsm(&(l2capContext->l2capPsm));
    BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed registering psm %d", l2capContext->l2capPsm.psm));

#if BT_SECURITY == XA_ENABLED
    /* Set the 'channel' field to serviceId on incoming connections */
    l2capContext->securityRecord.channel = l2capContext->l2capPsm.psm;

    /* Register server security record */
    status = SEC_Register(&l2capContext->securityRecord);
  
    /* Registration failed? --> clean up.*/
    if (BT_STATUS_SUCCESS != status)
    {
        /* Remove PSM registration. */
        status = L2CAP_DeregisterPsm(&(l2capContext->l2capPsm));
        /* Deregistration failed? --> just make a note.*/
        if (status != BT_STATUS_SUCCESS)
        {
            BTL_LOG_ERROR(("Failed deregistering psm %d", l2capContext->l2capPsm.psm));
        }

        /* This will handle the exception gracefully */
        BTL_ERR(status, ("Failed registering server security record"));
    }
#endif  /* BT_SECURITY == XA_ENABLED */
		  
    BTL_LOG_INFO(("PSM %d is registered ", l2capContext->l2capPsm.psm));

    BTL_FUNC_END();

    return (status);
}


/*-------------------------------------------------------------------------------
 * DeregisterPsm()
 *
 *		Internal function for deregistering Psm.
 *    It tries to remove the registration which can only fail if there are still
 *    channels connected. If there are still connection available, the first one
 *    will be disconnected..
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		l2capContext [in] - L2CAP context.
 *
 * Returns:
 *		BtStatus.
 */
static BtStatus DeregisterPsm(BtlL2capContext *l2capContext)
{
	  BtStatus         status;
    BtlL2capChannel *btlChannel;
    U16              idx;
	  
	  BTL_FUNC_START("DeregisterPsm");

    /* Remove registration from L2CAP. */
	  status = L2CAP_DeregisterPsm(&l2capContext->l2capPsm);

    /* Succeeded? --> remove security record + notify APP. */
    if (BT_STATUS_SUCCESS == status)
    {
  
#if BT_SECURITY == XA_ENABLED
		    status = SEC_Unregister(&l2capContext->securityRecord);
		    if (BT_STATUS_SUCCESS != status)
        {  
  			    BTL_LOG_ERROR(("Failed deregistering server security record!, status = %d", status));
        }
#endif	/* BT_SECURITY == XA_ENABLED */

    	  BTL_LOG_INFO(("PSM %d is deregistered.", l2capContext->l2capPsm.psm));
    }

    /* Still channels connected? --> disconnect the in sequence */
    else if (BT_STATUS_BUSY == status)
    {
        /* Check all channels for this context if they are connected */
        for (idx = 0; idx < MAX_CHANNELS; idx++)
        {
  	        btlChannel = &(btlChannelAdmin[idx]);
      
            /* Channel is owned by this context? --> close it if it is connected. */
            if (btlChannel->l2capContext == l2capContext)
            {
                /* Channel is connected? --> disconnect it. */
    	          switch (btlChannel->state)
    	          {
    	          case BTL_L2CAP_CHANNEL_STATE_CONNECTED:
  			            status = L2CAP_DisconnectReq(btlChannel->cid);
  			            BTL_VERIFY_ERR((status == BT_STATUS_PENDING),
                                   status,
                                   ("Failed closing channel %d", idx));
                    
  			            SetChannelState(btlChannel,BTL_L2CAP_CHANNEL_STATE_DISCONNECTING);

                    /* force a break outside the loop. */
                    idx = MAX_CHANNELS;
                    break;
                }
            }
        }
    }
    /* other results! --> internal error. */
    else
    {
			  BTL_LOG_ERROR(("Failed deregistering PSM!, status = %d", status));
    }
  
	  BTL_FUNC_END();

	  return (status);
}

/*-------------------------------------------------------------------------------
 * ConnectL2cap()
 *
 *    Internal function for opening client channel, when ACL link already exists.
 *
 * Type:
 *    Asynchronous
 *
 * Parameters:
 *    l2capContext [in] - L2CAP context.
 *
 *    btlChannel [in] - channel admin for this connection.
 *
 * Returns:
 *    BtStatus.
 */
static BtStatus ConnectL2cap(BtlL2capContext *l2capContext,
                             BtlL2capChannel *btlChannel)
{
    BtStatus        status = BT_STATUS_PENDING;
    BtRemoteDevice *remDev;
  
    BTL_FUNC_START("ConnectL2cap");

    remDev = CMGR_GetRemoteDevice(&(btlChannel->cmgrHandler));
    BTL_VERIFY_ERR((NULL != remDev), BT_STATUS_NO_CONNECTION, ("ACL link is not connected!"));

    status = L2CAP_ConnectReq(&l2capContext->l2capPsm,
                              l2capContext->l2capPsm.psm, remDev,
                              &l2capContext->settings,
                              &(btlChannel->cid));

    BTL_FUNC_END();

    return (status);
}

/*-------------------------------------------------------------------------------
 * BtlL2capCmgrCallback()
 *
 *    Internal callback for handling CMGR events in this layer.
 *
 * Type:
 *    Synchronous
 *
 * Parameters:
 *    handler [in] - Internal CMGR handler.
 *
 *    event [in] - Internal CMGR event.
 *
 *    cmgrStatus [in] - indicate status.
 *
 * Returns:
 *    void.
 */
static void BtlL2capCmgrCallback(CmgrHandler *handler,
                                 CmgrEvent event,
                                 BtStatus cmgrStatus)
{
    BtStatus            status = BT_STATUS_SUCCESS;
    BOOL                cleanUpAndReport = FALSE;
    char                addr[BDADDR_NTOA_SIZE];
    BtlL2capContext    *l2capContext;
    BtlL2capChannel    *btlChannel;
    L2capCallbackParms  info;

    BTL_FUNC_START("BtlL2capCmgrCallback");

    /* Find BTL L2CAP channel according to given CMGR handler */
    btlChannel = ContainingRecord(handler, BtlL2capChannel, cmgrHandler);
  
    /* Find context according L2CAP channelId */
    l2capContext = (btlChannel->l2capContext);

    switch (event)
    {
	      case (CMEVENT_DATA_LINK_CON_CNF):
  	        /* An outgoing ACL connection request has been completed. */
  	        BTL_LOG_INFO(("ACL link connect confirmation from %s, status = %d.",
  			                 bdaddr_ntoa(&(handler->bdc->addr), addr),
                         cmgrStatus));

            /* New ACL link present? --> create L2CAP link now. */
            if (BT_STATUS_SUCCESS == cmgrStatus)
            {
                status = ConnectL2cap(l2capContext, btlChannel);
                if (BT_STATUS_PENDING != status)
                {
                    cleanUpAndReport = TRUE;
                }
            }
            else
            {
                cleanUpAndReport = TRUE;
            }
            break;
    
        case (CMEVENT_DATA_LINK_CON_IND):
            /* A remote device has established an ACL connection with this device? */
            /* --> nice to know but I do nothing with it, just log it.             */
            BTL_LOG_INFO(("ACL link connect indication from %s.",
                         bdaddr_ntoa(&(handler->remDev->bdAddr), addr)));
            break;
      
        case (CMEVENT_DATA_LINK_DIS):
            /* The ACL link has been disconnected?                    */
            /* --> nice to know but I do nothing with it, just log it.*/
            BTL_LOG_INFO(("ACL link has been disconnected from %s, status = %d, reason = %d.",
                         bdaddr_ntoa(&(handler->remDev->bdAddr), addr),
                         cmgrStatus,
                         handler->errCode));
            break;
    }

    /* Failure in set-up up the ACL link or L2CAP link?        */
    /* --> cleanup + notify the APP with L2EVENT_DISCONNECTED. */
    if (TRUE == cleanUpAndReport)
    {
        /* ACL link present? --> remove my channel realtion from it. */
        if (CMGR_IsLinkUp(&btlChannel->cmgrHandler) == TRUE)
        {
            /* Remove my cmgrHandler from this ACL link. */
            status = CMGR_RemoveDataLink(&btlChannel->cmgrHandler);
            if (status != BT_STATUS_SUCCESS)
            {
                BTL_LOG_ERROR(("Failed removing data link, status = ", status));
            }
        }

        /* Release CMGR handler */
        status = CMGR_DeregisterHandler(&btlChannel->cmgrHandler);
        if (status != BT_STATUS_SUCCESS)
        {
            BTL_LOG_ERROR(("Failed deregistering CMGR handler, status = ", status));
        }

        /* channelAdmin is free to use again. */
        SetChannelState(btlChannel,BTL_L2CAP_CHANNEL_STATE_DISCONNECTED);

        /* Indicate the failure to the application */
        info.event = L2EVENT_CONNECTION_FAILURE;
        info.status = (U16) cmgrStatus;
				info.aclLink = handler->remDev;

        /* Fill the event data. */
        btlL2capData.event.l2capContext = l2capContext;
        btlL2capData.event.channelId = btlChannel->channelId;
        btlL2capData.event.l2capEvent = &info;

        /* Pass the event to app */
        l2capContext->callback(&btlL2capData.event);
    }

    BTL_FUNC_END();
}

/*-------------------------------------------------------------------------------
 * BtlL2capCallback()
 *
 *    Internal callback for handling L2CAP events in this layer.
 *    This function forwards relevant events to the registered application callback.
 *
 * Type:
 *    Synchronous
 *
 * Parameters:
 *    cid [in] - CID of the L2CAP channel.
 *
 *    parms [in] - Internal L2CAP event.
 *
 * Returns:
 *    void.
 */
static void BtlL2capCallback(L2capChannelId cid,
                             L2capCallbackParms *parms)
{
    BOOL             passEventToApp = TRUE;
    BtlL2capContext *l2capContext;

    BTL_FUNC_START("BtlL2capCallback");

    /* Get the correct context for this event. */
    l2capContext = GetContext(parms);

    /* Handle event in the correct context/channel admin. */
    switch (parms->event)
    {  
  /*----------------------------
   * Channel related events
   */
        case L2EVENT_DATA_IND:
            /*BTL_LOG_INFO(("L2EVENT_DATA_IND"));*/
            /* Only the CID needs to be translated to the channelId in the event. */
            FillChannelId(cid);      
            break;
        case L2EVENT_PACKET_HANDLED:
            /*BTL_LOG_INFO(("L2EVENT_PACKET_HANDLED"));*/
            passEventToApp = HandlePacketHandled(cid, parms);
            break;
        case L2EVENT_DISCONNECTED:
            BTL_LOG_INFO(("L2EVENT_DISCONNECTED"));
            passEventToApp = HandleDisconnected(l2capContext, cid, parms);
            break;
        case L2EVENT_DISCON_IND:
            BTL_LOG_INFO(("L2EVENT_DISCON_IND"));
            /* Do nothing, just notify the APP. It is just a notification         */
            /*  that the link is in the disconnection process.                    */
            /*  It will be followed by a L2EVENT_DISCONNECTED anyway.             */
            /* Only the CID needs to be translated to the channelId in the event. */
            FillChannelId(cid);      
            break;
  /*----------------------------
   * Context related events
   */
        case L2EVENT_CONNECT_IND:
            BTL_LOG_INFO(("L2EVENT_CONNECT_IND"));
            passEventToApp = HandleConnectInd(l2capContext, cid, parms);
            break;
        case L2EVENT_CONNECTED:
            BTL_LOG_INFO(("L2EVENT_CONNECTED"));
            passEventToApp = HandleConnected(cid, parms);
            break;
	      case L2EVENT_COMPLETE:
            BTL_LOG_INFO(("L2EVENT_COMPLETE"));
            passEventToApp = HandleComplete(parms);
            break;

        /* Unknown event? --> make a note. */      
        default:
            BTL_LOG_ERROR(("Unknown event from L2CAP."));
            break;
        }

      /* APP should be notified? --> do it. */
    if (TRUE == passEventToApp)
    {
        /* Set the context in the event passed to app */
        btlL2capData.event.l2capContext = l2capContext;

        /* Setting the channelId in the event passed to app is already done. */

        /* Set the internal event in the event passed to app */
        btlL2capData.event.l2capEvent = parms;

        /* Pass the event to app */
        l2capContext->callback(&btlL2capData.event);

        /* Check if some after processing is necessary. */
        AfterProcessing(&(btlL2capData.event));
    }

    BTL_FUNC_END();
}

/*-------------------------------------------------------------------------------
 * BtlSapsBtlNotificationsCb()
 *
 *    blabla
 *
 * Type:
 *    Synchronous
 *
 * Parameters:
 *    notificationType [in] - blabla
 *
 * Returns:
 *    status
 */
static BtStatus BtlL2capBtlNotificationsCb(BtlModuleNotificationType notificationType)
{
/* gesl TODO: fill in the required behaviour; it is now a framework only. */
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BtlL2capBtlNotificationsCb");
	
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

/*-------------------------------------------------------------------------------
 * FillChannelId()
 *
 *    Find the channelId for the 'CID' and fill it in the global event admin.
 *
 */
/* gesl: REMARK: lot of work, just for replying a channelId!! replying CID would be more efficient. */
static void FillChannelId(L2capChannelId cid)
{
    BtlL2capChannel *btlChannel;

    BTL_FUNC_START("FillChannelId");

    /* Find the correct connectAdmin. */
    btlChannel = GetChannelAdmin(cid);

    /* No admin found? --> make a note. */
    if (NULL == btlChannel)
    {
        BTL_LOG_ERROR(("No CID %d not found",cid));
        btlL2capData.event.channelId = BTL_L2CAP_ILLEGAL_CHANNEL_ID;
    }
    else
    {
        btlL2capData.event.channelId = btlChannel->channelId;
    }

    BTL_FUNC_END();
}

/*-------------------------------------------------------------------------------
 * AllocContext()
 *
 *    Store the 'l2capContext' on the first free location in the 'contextAdmin'.
 *    and fill the 'contextId'.in this 'l2capContext'.
 *    If TRUE is returned, the allocation is succeeded.
 *    If FALSE is returned, the administration is full.
 *
 */
static BOOL AllocContext(BtlL2capContext *l2capContext)
{
    BOOL result=FALSE; /* function return. */
    U16  idx;

    BTL_FUNC_START("AllocContext");

    /* Find free spot in the admin. */
    for (idx = 0; idx < BTL_CONFIG_L2CAP_MAX_NUM_OF_CONTEXTS; idx++)
    {
        /* Found? --> store it +  loop. */
        if (NULL == btlContextsAdmin[idx])
        {
           btlContextsAdmin[idx] = l2capContext;
           l2capContext->contextId = idx;
           result = TRUE;
           break;
        }
    }

    BTL_FUNC_END();

    return result;
}

/*-------------------------------------------------------------------------------
 * GetContext()
 *
 *		Return the specific l2capContext that is identified with the 'contextId'
 *    as stored inside the 'psm' admin deeply inside 'parms'.
 *
 */
static BtlL2capContext *GetContext(L2capCallbackParms *parms)
{
    BTL_FUNC_START("GetContext");
    BTL_FUNC_END();

    return(btlContextsAdmin[parms->owner.psm->contextId]);
}

/*-------------------------------------------------------------------------------
 * FreeContext()
 *
 *    Store the 'l2capContext' from the 'contextAdmin'.
 *    If TRUE is returned, the release is succeeded.
 *    If FALSE is returned, the 'l2capContext' was not found.
 *
 */
static BOOL FreeContext(BtlL2capContext *l2capContext)
{
    BOOL result=FALSE; /* function return. */
    U16  idx;

    BTL_FUNC_START("L2capCallback");

    /* Find context in the admin. */
    for (idx = 0; idx < BTL_CONFIG_L2CAP_MAX_NUM_OF_CONTEXTS; idx++)
    {
        /* Found? --> clear entry + exit loop. */
        if (l2capContext == btlContextsAdmin[idx])
        {
             btlContextsAdmin[idx] = NULL;
             result = TRUE;
             break;
        }
    }

    BTL_FUNC_END();

    return result;
}

/*-------------------------------------------------------------------------------
 * GetChannelAdmin()
 *
 *    Find the channelAdmin that is identified with the L2CAP 'CID' and return
 *    it. When it cannot be found, NULL will be returned.
 *
 * Parameters:
 *    cid [in] - L2CAP CID
 *
 */
static BtlL2capChannel *GetChannelAdmin(L2capChannelId cid)
{
    BtlL2capChannel *btlChannel=NULL; /* function return. */
    U16              idx;
  
    BTL_FUNC_START("GetChannelAdmin");

    for (idx = 0; idx < MAX_CHANNELS; idx++)
    {
        /* Found it? --> exit loop.*/
        if (cid ==btlChannelAdmin[idx].cid)
        {
            btlChannel = &(btlChannelAdmin[idx]);
            break;
        }
    }
    
    BTL_FUNC_END();

    return(btlChannel);
}

/*-------------------------------------------------------------------------------
 * GetFreeChannelAdmin()
 *
 *    Return a free channel administration.
 *    If there is no free administration, NULL will be returned.
 *
 */
static BtlL2capChannel *GetFreeChannelAdmin()
{
    BtlL2capChannel *btlChannel=NULL; /* function return. */
    U16 idx;
  
    BTL_FUNC_START("GetFreeChannelAdmin");

    for (idx = 0; idx < MAX_CHANNELS; idx++)
    {
        /* Found it? --> exit loop.*/
        if (BTL_L2CAP_CHANNEL_STATE_DISCONNECTED ==btlChannelAdmin[idx].state)
        {
            btlChannel = &(btlChannelAdmin[idx]);
            break;
        }
    }
    
    BTL_FUNC_END();

    return(btlChannel);
}

/*-------------------------------------------------------------------------------
 * SetChannelState()
 *
 *    Fill the 'state' parameter in the 'btlChannel' with the new 'state'.
 *
 */
static void SetChannelState(BtlL2capChannel *btlChannel,
                            BtlL2capChannelState state)
{   
    BTL_FUNC_START("SetChannelState");

    btlChannel->state = state;
    
    /* DEBUG: do some logging. */
    BTL_L2CAP_LOG_CHANNEL_STATE(state);

    BTL_FUNC_END();
}

/*-------------------------------------------------------------------------------
 * SetContextState()
 *
 *    Fill the 'state' parameter in the 'l2capContext' with the new 'state'.
 *
 */
static void SetContextState(BtlL2capContext *l2capContext,
                            BtlL2capContextState state)
{   
    BTL_FUNC_START("SetContextState");

    l2capContext->state = state;
    
    /* DEBUG: do some logging. */
    BTL_L2CAP_LOG_CONTEXT_STATE(state);

    BTL_FUNC_END();
}

/*-------------------------------------------------------------------------------
 * HandlePacketHandled()
 *
 *    Process the L2EVENT_PACKET_HANDLED from the L2CAP module in the Blue SDK
 *    A packet has been transmitted. This routine will release the
 *    L2CAP packet and notify the APP.
 *
 * Parameters:
 *
 *    cid [in] - L2CAP CID for this channel
 *
 *    parms [in] - event data
 *
 * Returns:
 *		TRUE - (event should be passed to the APP)
 */
static BOOL HandlePacketHandled(L2capChannelId cid,
                                L2capCallbackParms *parms)
{
	  BtStatus  status;
    BtPacket *packet;

    BTL_FUNC_START("HandlePacketHandled");

    /*BTL_LOG_INFO(("Tx-packet processed on CID %d. Status = %d.", cid, parms->status)); */

    /* Save pointer locally, since it is set to NULL when calling BTL_POOL_Free  */
    /*  The 'packet' in the event needs to stay alive, because it identified the */
    /*  txPacket ID for the APP.                                         .       */
    packet = parms->ptrs.packet;

    /* Release the 'packet'. */
    status = BTL_POOL_Free(&btlL2capData.packetsPool, (void **)(&packet));
    BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status),
                     BT_STATUS_INTERNAL_ERROR,
                     ("Failed freeing TX packet"));

    /* Fill correct channelId in the APP event */
    FillChannelId(cid);

    BTL_FUNC_END();

    return TRUE;
}

/*-------------------------------------------------------------------------------
 * HandleDisconnected()
 *
 *    Process the L2EVENT_DISCONNECTED from the L2CAP module in the Blue SDK
 *    An L2CAP link has been disconnected now.
 *
 * Parameters:
 *		l2capContext [in] - context for processing this event
 *
 *    cid [in] - assigned CID for this L2CAP channel
 *
 *    parms [in] - event data
 *
 * Returns:
 *		TRUE - event should be passed to the APP.
 *
 *		FALSE - event is completely handled inside. It does not need to
 *        be passed to the APP.
 */
static BOOL HandleDisconnected(BtlL2capContext *l2capContext,
                               L2capChannelId cid,
                               L2capCallbackParms *parms)
{
    BOOL             passEventToApp = TRUE; /* Function return. */
	  BtStatus         status = BT_STATUS_SUCCESS;
    BtlL2capChannel *btlChannel;
	  char             addr[BDADDR_NTOA_SIZE];

    BTL_FUNC_START("HandleDisconnected");

    /* Get btlChannel in the admin. */
    btlChannel = GetChannelAdmin(cid);

    BTL_VERIFY_ERR_SET_RETVAR((btlChannel != NULL), (passEventToApp = FALSE),
					("No channelAdmin found for (%x)", cid));

    /* Fill correct channelId in the event for the APP. */
    btlL2capData.event.channelId = btlChannel->channelId;
  
    /* Clear channelAdmin for next usage. */
  	SetChannelState(btlChannel,BTL_L2CAP_CHANNEL_STATE_DISCONNECTED);
    btlChannel->cid = L2CID_INVALID;

    /* Was it a client channel? --> remove the PSM registration from the ESI stack as well.*/
    if (BTL_L2CAP_CONTEXT_STATE_CLIENT == l2capContext->state)
    {
        /* Context is free to use again. */
        SetContextState(l2capContext, BTL_L2CAP_CONTEXT_STATE_IDLE);

        /* Remove PSM registration fro this client. */
        status = L2CAP_DeregisterPsm(&l2capContext->l2capPsm);
        BTL_VERIFY_ERR(((status == BT_STATUS_SUCCESS) || (status == BT_STATUS_BUSY)),
                         status,
                         ("Internal (client) PSM deregistration failed, status = ", status));

#if BT_SECURITY == XA_ENABLED
        /* Remove security handler registration as well for this client. */
    		status = SEC_Unregister(&l2capContext->securityRecord);
    		if (BT_STATUS_SUCCESS != status)
        {  
    			BTL_LOG_ERROR(("Failed deregistering client security record!, status = %d", status));
        }
#endif	/* BT_SECURITY == XA_ENABLED */
    }
	
/* gesl: TODO:INVESTIGATE: Is next check necessary; is it not covered in CMGR_RemoveDataLink?? */
  	/* ACL link still present? --> remove it, or at least my relation to this link. */
  	if (CMGR_IsLinkUp(&btlChannel->cmgrHandler) == TRUE)
  	{
    	  /* Remove my channel link from this ACL link.*/
    	  status = CMGR_RemoveDataLink(&btlChannel->cmgrHandler);
    	  if (status != BT_STATUS_SUCCESS)
        { 
    			  BTL_LOG_ERROR(("Failed removing ACL link, status = ", status));
        }
  	}

  	/* Release CMGR handler */
  	status = CMGR_DeregisterHandler(&btlChannel->cmgrHandler);
  	if (BT_STATUS_SUCCESS != status)
    { 
  		  BTL_LOG_ERROR(("Failed deregistering CMGR handler, status = ", status));
    }
  	
  	BTL_LOG_INFO(("Channel %d is disconnected from %s.", cid, bdaddr_ntoa(&(parms->aclLink->bdAddr), addr)));

    BTL_FUNC_END();

    return passEventToApp;
}

/*-------------------------------------------------------------------------------
 * HandleConnectInd()
 *
 *    Process the L2EVENT_CONNECT_IND from the L2CAP module in the Blue SDK
 *
 * Parameters:
 *		l2capContext [in] - context for processing this event
 *
 *    cid [in] - assigned CID for this L2CAP channel
 *
 *    parms [in] - event data
 *
 * Returns:
 *		TRUE - event should be passed to the APP.
 *
 *		FALSE - event is completely handled inside. It does not need to
 *        be passed to the APP.
 */
static BOOL HandleConnectInd(BtlL2capContext *l2capContext,
                             L2capChannelId cid,
                             L2capCallbackParms *parms)
{
    BOOL             passEventToApp = TRUE; /* Function return. */
    BtlL2capChannel *btlChannel;
	  char             addr[BDADDR_NTOA_SIZE];
 
    BTL_FUNC_START("HandleConnectInd");
    
    BTL_LOG_INFO(("Channel %d received connect indication from %s.", cid, bdaddr_ntoa(&(parms->aclLink->bdAddr), addr)));

  	/* Get a free channelAdmin  */
	  btlChannel = GetFreeChannelAdmin();

    /* No free channel? --> refuse it with reason: No Resources */
    if (btlChannel == NULL)
    {
        passEventToApp = FALSE;

        /* Send negative reply (no resources) to the remote side.*/
        (void) L2CAP_ConnectRsp(cid, L2CONN_REJECT_NO_RESOURCES, NULL);
    }
    
    /* free channel found? --> update admin + notify. */
    else
    {
        btlChannel->l2capContext = l2capContext;
        btlChannel->cid = cid; 
        SetChannelState(btlChannel,BTL_L2CAP_CHANNEL_STATE_CONNECT_IND);
        
        /* Fill correct channelId in the event for the APP. */
        btlL2capData.event.channelId = btlChannel->channelId;
    }

    BTL_FUNC_END();

    return passEventToApp;
}

/*-------------------------------------------------------------------------------
 * HandleConnected()
 *
 *    Process the L2EVENT_CONNECTED from the L2CAP module in the Blue SDK
 *
 * Parameters:
 *    cid [in] - assigned CID for this L2CAP channel
 *
 *    parms [in] - event data
 *
 * Returns:
 *		TRUE - event should be passed to the APP.
 *
 *		FALSE - event is completely handled inside. It does not need to
 *        be passed to the APP.
 */
static BOOL HandleConnected(L2capChannelId cid,
                            L2capCallbackParms *parms)
{
    BOOL             passEventToApp = TRUE; /* Function return. */
    BtlL2capChannel *btlChannel;
	  BtStatus         status = BT_STATUS_SUCCESS;
	  char             addr[BDADDR_NTOA_SIZE];

    BTL_FUNC_START("HandleConnected");

    /* Get channel Admin for this connection. */
	  btlChannel = GetChannelAdmin(cid);

    /* No free channel? --> there is an (internal) problem */
    if (NULL == btlChannel)
    {
        passEventToApp = FALSE;
        BTL_LOG_ERROR(("Could not find channel admin for connection with CID %d", cid));
    }

    /* Channel found? --> continue and notify the APP.*/
    else
    { 
        /* Incoming connection? --> need to associate CMGR handler with the incoming link */
        if (btlChannel->state == BTL_L2CAP_CHANNEL_STATE_CONNECT_IND)
        {
            status = CMGR_RegisterHandler(&btlChannel->cmgrHandler, BtlL2capCmgrCallback);
            if (status == BT_STATUS_SUCCESS)
            {
                /* Associate this L2CAP connection to the (existing) ACL link as well. */
                status = CMGR_CreateDataLink(&btlChannel->cmgrHandler, &(parms->aclLink->bdAddr));
                /* Not succeeded immediately? --> internal error */
                if (status != BT_STATUS_SUCCESS)
                {
                    /* Release CMGR handler */
                    (void)CMGR_DeregisterHandler(&btlChannel->cmgrHandler);
                    BTL_LOG_ERROR(("Failed associating CMGR handler to incoming connection, status = %d", status));
                }
          }
          else
          {
              BTL_LOG_ERROR(("Failed registering CMGR handler to incoming connection, status = %d", status));
          }
        }

        SetChannelState(btlChannel,BTL_L2CAP_CHANNEL_STATE_CONNECTED);

        /* Fill correct channelId in the event for the APP. */
        btlL2capData.event.channelId = btlChannel->channelId;

        BTL_LOG_INFO(("Channel %d is connected to %s.", cid, bdaddr_ntoa(&(parms->aclLink->bdAddr), addr)));
    }
    
    BTL_FUNC_END();

    return passEventToApp;
}

/*-------------------------------------------------------------------------------
 * HandleComplete()
 *
 *    Process the L2EVENT_COMPLETE from the L2CAP module in the Blue SDK
 *    It signals that the BTL_L2CAP_Ping or BTL_L2CAP_GetInfo action has
 *    finished. These actions do not require an L2CAP channel,
 *
 * Parameters:
 *    parms [in] - event data
 *
 * Returns:
 *		TRUE - event should be passed to the APP.
 *
 *		FALSE - event is completely handled inside. It does not need to
 *        be passed to the APP.
 */
static BOOL HandleComplete(L2capCallbackParms *parms)
{
    BOOL passEventToApp = TRUE; /* Function return. */
	
    BTL_FUNC_START("HandleComplete");

    BTL_LOG_INFO(("Received complete event from %d, status = %d.",parms->aclLink->bdAddr, parms->status));

    /* Give channelId in the event for the APP a defined value. */
    btlL2capData.event.channelId = BTL_L2CAP_ILLEGAL_CHANNEL_ID;
    
    BTL_FUNC_END();

    return passEventToApp;
}

/*-------------------------------------------------------------------------------
 * SendL2capDisabled()
 *
 *    Send L2CAP_EVENT_DISABLED to the APP that owns (has created) the
 *    indicated 'l2capContext'.
 *
 * Parameters:
 *		l2capContext [in] - context identifying the APP.
 *
 * Returns:
 *		void
 */
static void SendL2capDisabled(BtlL2capContext *l2capContext)
{
    L2capCallbackParms info;

    BTL_FUNC_START("SendL2capDisabled");

    /* Create data field in the event */
    btlL2capData.event.l2capEvent = &info;

    /* Set the internal event in the event passed to app */
    btlL2capData.event.l2capEvent->event = L2CAP_EVENT_DISABLED;

    /* Set the context in the event passed to app */
    btlL2capData.event.l2capContext = l2capContext;

    /* Pass the event to app */
    l2capContext->callback(&btlL2capData.event);

    BTL_FUNC_END();
}

/*-------------------------------------------------------------------------------
 * AfterProcessing()
 *
 *    Check if there is some processing necessary for this 'l2capContext' due to
 *    the latest 'event' sent to the application.
 *
 * Parameters:
 *		l2capEvent [in] - Latest event, sent to the application.
 *
 * Returns:
 *    void
 */
static void AfterProcessing(BtlL2capEvent *l2capEvent)
{
    BtStatus status;

    BTL_FUNC_START("AfterProcessing");

    switch(l2capEvent->l2capContext->state)
    {
        /* In the process of de registration. */
        case BTL_L2CAP_CONTEXT_STATE_DISABLING:
            /* Last disconnect event to the APP during disabling?                        */
            /*  --> notify APP that the disabling is finished + reset the context state. */
            if(L2EVENT_DISCONNECTED == l2capEvent->l2capEvent->event)
            {
                /* Try to remove the PSM administration now.*/      
                status = DeregisterPsm(l2capEvent->l2capContext);

                /* Succeeded? --> last connection is gone; notify the APP. */
                if (BT_STATUS_SUCCESS == status)
                {
                    /* I am finished with the deregistration process. */
                    SetContextState(l2capEvent->l2capContext, BTL_L2CAP_CONTEXT_STATE_IDLE);

                    l2capEvent->l2capEvent->event = L2CAP_EVENT_DISABLED;
                    l2capEvent->l2capContext->callback(l2capEvent);
                }
            }
            break;
    }

    BTL_FUNC_END();
}

/* Some debug logging routines. */
#if (XA_DEBUG == XA_ENABLED) || (XA_DEBUG_PRINT == XA_ENABLED)
/*-------------------------------------------------------------------------------
 * LogChannelState()
 *
 *    Display the 'state' variable in the logging window as readable text.
 *
 * Parameters:
 *    state[in] - the state of the channel to be displayed.
 *
 * Returns:
 */
static void LogChannelState(BtlL2capChannelState state)
{
    switch (state)
    {
        case BTL_L2CAP_CHANNEL_STATE_DISCONNECTED: BTL_LOG_INFO(("Channel state changed: DISCONNECTED"));break;
        case BTL_L2CAP_CHANNEL_STATE_CONNECTED: BTL_LOG_INFO(("Channel state changed: CONNECTED"));break;
        case BTL_L2CAP_CHANNEL_STATE_DISCONNECTING: BTL_LOG_INFO(("Channel state changed: DISCONNECTING"));break;
        case BTL_L2CAP_CHANNEL_STATE_CONNECTING: BTL_LOG_INFO(("Channel state changed: CONNECTING"));break;
        case BTL_L2CAP_CHANNEL_STATE_CONNECT_IND: BTL_LOG_INFO(("Channel state changed: CONNECT_IND"));break;
    }
}

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
static void LogContextState(BtlL2capContextState state)
{
    switch (state)
    {
        case BTL_L2CAP_CONTEXT_STATE_IDLE: BTL_LOG_INFO(("Context state changed: IDLE"));break;
        case BTL_L2CAP_CONTEXT_STATE_CLIENT: BTL_LOG_INFO(("Context state changed: CLIENT"));break;
        case BTL_L2CAP_CONTEXT_STATE_SERVER: BTL_LOG_INFO(("Context state changed: SERVER"));break;
        case BTL_L2CAP_CONTEXT_STATE_DISABLING: BTL_LOG_INFO(("Context state changed: DISABLING"));break;
    }
}
#endif /* (XA_DEBUG == XA_ENABLED) || (XA_DEBUG_PRINT == XA_ENABLED) */



#else /*BTL_CONFIG_BTL_L2CAP == BTL_CONFIG_ENABLED*/

/*-------------------------------------------------------------------------------
 * BTL_L2CAP_Init() - When  BTL_CONFIG_BTL_L2CAP is disabled.
 */
BtStatus BTL_L2CAP_Init(void)
{
    
   BTL_LOG_INFO(("BTL_L2CAP_Init()  -  BTL_CONFIG_L2CAP Disabled"));
  

    return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 *BTL_L2CAP_Deinit() - When  BTL_CONFIG_BTL_RFCOMM is disabled.
 */
BtStatus BTL_L2CAP_Deinit(void)
{
    BTL_LOG_INFO(("BTL_L2CAP_Deinit() -  BTL_CONFIG_L2CAP Disabled"));

    return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------*/



#endif /*BTL_CONFIG_BTL_L2CAP == BTL_CONFIG_ENABLED*/



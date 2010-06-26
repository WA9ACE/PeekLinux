/*******************************************************************************\
*                                                                               *
*  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION            *
*                                                                               *
*  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE          *
*   UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE       *
*   APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO       *
*   BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT        *
*   OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL         *
*   DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.          *
*                                                                               *
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      btl_bip.c
*
*   DESCRIPTION:    This file contians the implementation of the BTL Basic Image Profile
*
*   AUTHOR:         Arnoud van Riessen
*
\*******************************************************************************/

/*gesl: TODO: clean up code*/
/*gesl: TODO: check all global variables (and its elements) for usage, etc.*/
/*gesl: TODO: add more comments*/
/*gesl: TODO: align code format*/
/*gesl: TODO: solve all remaining //gesl issues/remarks*/
/*gesl: TODO: check/correct imagePush (extended) functionality.*/

/********************************************************************************/

#include "btl_config.h"
#include "btl_log.h"
#include "btl_defs.h"
#include "btl_bipint.h"

BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_BIPINT);

#if BTL_CONFIG_BIP ==   BTL_CONFIG_ENABLED

/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "osapi.h"
#include "bttypes.h"
#include "medev.h"
#include "obex.h"
#include "goep.h"
#include "debug.h"
#include "utils.h"
#include "pathmgr.h"
#include "bthal_fs.h"

#include "btl_unicode.h"


#include "btl_commoni.h"
#include "btl_pool.h"
#include "btl_utils.h"
#include "btl_debug.h"

#include "btl_obex_utils.h"
#include "bip.h"
#include "btl_bip_common.h"
#include "btl_bip_xml.h"



/********************************************************************************
 *
 * Constants
 *
 *******************************************************************************/
#define BTL_BIPINT_MAX_FSPATH_LEN  BTHAL_FS_MAX_PATH_LENGTH

#define MAX_XML_DESCR_LEN ((U16) 1000) /* Max length of an XML Descriptor to be build */
/*gesl: TODO: tune this size to smaller value.*/

/*-------------------------------------------------------------------------------
 * BIP_SERVICE_NAME_MAX_LEN constant
 *
 *     Represents max length of service name for BIP.
 */
#define BIP_SERVICE_NAME_MAX_LEN  (32)

/*-------------------------------------------------------------------------------
 * requestType
 *
 *     Type of OBEX request (PUT or GET) to be send.
 */
typedef U8 requestType;
#define REQUEST_PUT ((requestType) 0)
#define REQUEST_GET ((requestType) 2)

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * ObexAuthFlags type
 *
 *     Defines the OBEX authentication flags of a specific context.
 */
typedef U8 ObexAuthFlags;

#define OBEX_AUTH_RECEIVED_CHAL  					(0x02)
#define OBEX_AUTH_DURING_CALLBACK					(0x10)
#define OBEX_AUTH_INFO_SET							(0x40)


/*gesl: TODO: check usage of this state-machine.*/
typedef enum _BtlBipImagePushState
{
    BTL_BIPINT_STATE_IMAGEPUSH_IDLE,
    BTL_BIPINT_STATE_IMAGEPUSH_CONNECTING,
    BTL_BIPINT_STATE_IMAGEPUSH_CONNECTED,
    BTL_BIPINT_STATE_IMAGEPUSH_PENDING,
    BTL_BIPINT_STATE_IMAGEPUSH_DISCONNECT,
    BTL_BIPINT_STATE_IMAGEPUSH_DISCONNECTING
} BtlBipImagePushState;

/*-------------------------------------------------------------------------------
 * BtlBipintRadioOffState type
 *
 *     Defines the BIPINT Radio Off state of a specific context.
 */
typedef enum _BtlBipintRadioOffState
{
	 BTL_BIPINT_RADIO_OFF_STATE_IDLE,
 	 BTL_BIPINT_RADIO_OFF_STATE_DISCONNECTING,
	 BTL_BIPINT_RADIO_OFF_STATE_DONE
} BtlBipintRadioOffState;

typedef enum _BtlBipintdRadioOffEvent
{
	 BTL_BIPINT_RADIO_OFF_EVENT_START,
 	 BTL_BIPINT_RADIO_OFF_EVENT_CONTEXT_COMPLETED
} BtlBipintRadioOffEvent;

/*-------------------------------------------------------------------------------
 * BtlBipintContextRadioOffState type
 *
 *     Defines the BIPINT Radio Off state of a specific context.
 */
typedef enum _BtlBipintContextRadioOffState
{
	 BTL_BIPINT_CONTEXT_RADIO_OFF_STATE_IDLE,
 	 BTL_BIPINT_CONTEXT_RADIO_OFF_STATE_DISCONNECTING,
	 BTL_BIPINT_CONTEXT_RADIO_OFF_STATE_DONE
} BtlBipintContextRadioOffState;

/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/
typedef struct _BipImagePush
{
    BtlBipEncoding  bipEncoding;
    U16             width;
    U16             height;
    U32             size;
} BipImagePush;

/*-------------------------------------------------------------------------------
 * BtlBipintContext structure
 *
 *     Represents BTL BIP context (for both Initiator and Responder).
 */
struct _BtlBipintContext
{
    /* Must be first field */
    BtlContext              base;

    BtlBipContextState      state;        /* State of this context. */
    BtlBipintCallBack       callback;     /* Application (Initiator) callabck for events from this module.. */
    BtlBipChannelState      channelState; /* Initiator has max 1 connection, so it can be kept in the context admin. */

    BipData                 bipData;              /* Object info (XML and Image requests or responses) as being exchanged with the BIP module.   */
    BtlBipObjStore          objStore;             /* Object store, where to read (sending) or write (receiving) object info                      */
                                                   /*  Used as an ID (ocx) to the BIP module for the identification of the object in the response */
/*gesl: QUESTION: why not use BtlObject type here? This type is also needed for ImagePush (see further on in this struct).*/

    /* Server specific */
    BipObexClient           *bipObexClient;
/*gesl: INVESTIGATE: this parameter (above) is only needed for the BIP_InitiatorAccept call. Which is not clear why touse it.*/
    BipInitiatorSession     sessionInitiator;

    /* Entries for ImagePush */
/*gesl: TODO: check usage of fields belwo when busy with imagePush function.*/
    BtlObject               bipObject;
    BtlBipImagePushState    imagePushState;
    BipImagePush            bipImagePush;
    BthalFsDirDesc          dirDesc;
    BthalFsStat             filestat;

    /* Service name plus 1 null-terminated char, plus 2 bytes for SDP_TEXT_8BIT */
    BtlUtf8                      serviceName[BIP_SERVICE_NAME_MAX_LEN + 3];
/*gesl: QUESTION: should this become a UTF-8 type?*/

    BtlUtf8                 xmlDescr[MAX_XML_DESCR_LEN]; /* 0-terminated UTF-8 string area, used for building an XML Descriptor for a BIP request to be send.*/
    U16                     nrLatestReturnedhandles; /* received number of image handles from response GetImageList  */

/* udir: Why not just record the BD Address? */
	ObexTpAddr 				responderAddress;
	BipService  			requestedService;

#if OBEX_AUTHENTICATION == XA_ENABLED
	/* OBEX Authentication flags */
	ObexAuthFlags 			obexAuthFlags;
#endif	/* OBEX_AUTHENTICATION == XA_ENABLED */

	BOOL						pendingDisconnection;
	BtlBipintContextRadioOffState	radioOffState;
	BOOL						asynchronousRadioOff;

	BOOL						deleteFileIfAborted;
	BtlUtf8          					pulledFileName[BTHAL_FS_MAX_PATH_LENGTH + 1];

	U32							objLen;

	BOOL						requestInProgress;
};

/*-------------------------------------------------------------------------------
 * BtlBipData structure
 *
 *     Represents the data of the whole BTL Bip module.
 */
typedef struct _BtlBipData
{
	/* Pool of Bip contexts */
	BTL_POOL_DECLARE_POOL(contextsPool, contextsMemory, BTL_CONFIG_BIPINT_MAX_NUM_OF_CONTEXTS, sizeof(BtlBipintContext));

	/* List of active Bip contexts */
	ListEntry            		contextsList;

	BtlBipintRadioOffState	radioOffState;
	U32					numOfDisconnectingContexts;
	BOOL				asynchronousRadioOff;

	BtlObjProgressInd    	progressIndInfo;

	BipCallbackParms     	selfGeneratedCallbackParms;

    /* Event passed to the application */
	BtlBipintEvent 		eventToApp;
} BtlBipData;

/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * btlBipInitState
 *
 *     Represents the current init state of Bip module.
 */
static BtlBipInitState btlBipInitState = BTL_BIP_INIT_STATE_NOT_INITIALIZED;


/*-------------------------------------------------------------------------------
 * btlBipData
 *
 *     Represents the data of Bip module.
 */
static BtlBipData btlBipData;


/*-------------------------------------------------------------------------------
 * btlBipContextsPoolName
 *
 *     Represents the name of the Bip contexts pool.
 */
static const char btlBipContextsPoolName[] = "BipInitiatorContexts";


/********************************************************************************
 *
 * Internal function prototypes
 *
 *******************************************************************************/
static BtlBipintContext *GetContextFromBipCallbackParms(BipCallbackParms* parms);
static void InitOperationVariables(BtlBipintContext* context);
static void BipintCallback(BipCallbackParms* parms);
static void AfterProcessing(BtlBipintContext* context, BtlBipintEvent *bipEvent);
static BtStatus ObexRequest(BtlBipintContext *bipContext,
                              requestType  operation,
                              BtlObject *object);
static void SetContextState(BtlBipintContext *bipContext,
                            BtlBipContextState    state);
static void SetChannelState(BtlBipintContext *bipContext,
                            BtlBipChannelState    state);
static U8  *LocateDataInBipData (BipData *bipData);

static BtStatus BtlBipintDisconnect(BtlBipintContext *bipContext);
static BtStatus BtlBipintBtlNotificationsCb(BtlModuleNotificationType notificationType);
static BtStatus BtlBipintContextRadioOffProcessor(BtlBipintContext *bipContext);
static BtStatus BtlBipintRadioOffProcessor(BtlBipintRadioOffEvent event);


/*gesl: TODO: check this extended functionality (lowest prio).*/
#ifdef SKIP
static BtStatus ImagePush(BtlBipintContext *bipContext);
#endif /* SKIP */


/* Some debug logging features. */
#if (XA_DEBUG == XA_ENABLED) || (XA_DEBUG_PRINT == XA_ENABLED)
static void LogChannelState(BtlBipChannelState state);
#define BTL_BIP_LOG_CHANNEL_STATE(state) LogChannelState(state)
static void LogContextState(BtlBipContextState state);
#define BTL_BIP_LOG_CONTEXT_STATE(state) LogContextState(state)
#else
/* Debugging disabled --> define empty macros */
#define BTL_BIP_LOG_CONTEXT_STATE(state)
#endif /* (XA_DEBUG == XA_ENABLED) || (XA_DEBUG_PRINT == XA_ENABLED) */

static void BtlBipintSendProgressNotificationToApp(BtlBipintContext *context, BipCallbackParms* parms, U32 currPos);
static void BtlBipintSendEventToApp(BtlBipintContext *context, BipCallbackParms *parms);
static void BtlBipintInitRequestVars(BtlBipintContext *bipContext);

/********************************************************************************
 *
 * Function implementation, externals.
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BTL_BIPINT_Init()
 */
BtStatus BTL_BIPINT_Init(void)
{
    BtStatus status = BT_STATUS_SUCCESS;

    BTL_FUNC_START_AND_LOCK_COMMON("BTL_BIPINT_Init");

    BTL_VERIFY_ERR((BTL_BIP_INIT_STATE_INITIALIZATION_FAILED != btlBipInitState), BT_STATUS_IMPROPER_STATE,
                    ("BIP initialization failed before, please de-init before retrying "));
    BTL_VERIFY_ERR((BTL_BIP_INIT_STATE_NOT_INITIALIZED == btlBipInitState), BT_STATUS_IMPROPER_STATE,
                    ("BIP module is already initialized"));

    btlBipInitState = BTL_BIP_INIT_STATE_INITIALIZATION_FAILED;

    status = BTL_POOL_Create(&btlBipData.contextsPool,
                            btlBipContextsPoolName,
                            (U32 *)btlBipData.contextsMemory,
                            BTL_CONFIG_BIPINT_MAX_NUM_OF_CONTEXTS,
                            sizeof(BtlBipintContext));
    BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("BIP contexts pool creation failed"));

    InitializeListHead(&btlBipData.contextsList);

	btlBipData.radioOffState = BTL_BIPINT_RADIO_OFF_STATE_IDLE;
	btlBipData.numOfDisconnectingContexts = 0;
	btlBipData.asynchronousRadioOff = FALSE;

	BTL_RegisterForCommonNotifications(BTL_MODULE_TYPE_BIPINT, BtlBipintBtlNotificationsCb);

    btlBipInitState = BTL_BIP_INIT_STATE_INITIALIZED;

    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPINT_Deinit()
 */
BtStatus BTL_BIPINT_Deinit(void)
{
    BtStatus status = BT_STATUS_SUCCESS;

    BTL_FUNC_START_AND_LOCK_COMMON("BTL_BIPINT_Deinit");

    BTL_VERIFY_ERR((BTL_BIP_INIT_STATE_NOT_INITIALIZED != btlBipInitState), BT_STATUS_FAILED,
                    ("BIP module is not initialized"));

    BTL_VERIFY_ERR((IsListEmpty(&btlBipData.contextsList)),
                           BT_STATUS_FAILED,
                           ("BIP Initiator contexts are still active"));


  	BTL_DeRegisterFromCommonNotifications(BTL_MODULE_TYPE_BIPINT);

    RemoveHeadList(&btlBipData.contextsList);

    status = BTL_POOL_Destroy(&btlBipData.contextsPool);
    BTL_VERIFY_FATAL_NORET((BT_STATUS_SUCCESS == status), ("BIP contexts pool destruction failed"));

    btlBipInitState = BTL_BIP_INIT_STATE_NOT_INITIALIZED;

    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPINT_Create()
 */
BtStatus BTL_BIPINT_Create(BtlAppHandle            *appHandle,
                           BtlBipintCallBack bipCallback,
                           BtSecurityLevel         *securityLevel,
                           BtlBipintContext  **bipContext)
{
    BtStatus status = BT_STATUS_SUCCESS;

    BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BIPINT_Create");

    BTL_VERIFY_ERR((NULL != bipCallback), BT_STATUS_INVALID_PARM, ("Null bipCallback"));
    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));

#if BT_SECURITY == XA_ENABLED

    if (NULL != securityLevel)
    {
        status = BTL_VerifySecurityLevel(*securityLevel);
        BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Invalid BIP securityLevel"));
    }

#endif  /* BT_SECURITY == XA_ENABLED */

    /* Allocate memory for a unique new BIP context */
    status = BTL_POOL_Allocate(&btlBipData.contextsPool, (void **)bipContext);
    BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed allocating BIP context"));

    status = BTL_HandleModuleInstanceCreation(appHandle, BTL_MODULE_TYPE_BIPINT, &(*bipContext)->base);

    if (BT_STATUS_SUCCESS != status)
    {
        /* bipContext must be freed before we exit */
        BtStatus freeResult = BTL_POOL_Free(&btlBipData.contextsPool, (void **)bipContext);
        BTL_VERIFY_FATAL_NORET((BT_STATUS_SUCCESS == freeResult), ("Failed freeing BIP context (%s)", pBT_Status(freeResult)));

        BTL_ERR(status, ("Failed handling BIP instance creation (%s)", pBT_Status(status)));
    }

    /* Init operation variables */
    InitOperationVariables(*bipContext);

    /* Save the given callback */
    (*bipContext)->callback = bipCallback;

    /* Add the new BIP context to the active contexts list */
    InsertTailList(&btlBipData.contextsList, &((*bipContext)->base.node));

#if BT_SECURITY == XA_ENABLED
    /* Save the given security level, or use default */
    if (NULL != securityLevel)
    {
       (*bipContext)->sessionInitiator.primary.cApp.secRecord.level = *securityLevel;
    }
    else
    {
      (*bipContext)->sessionInitiator.primary.cApp.secRecord.level = BSL_NO_SECURITY;
    }
#endif  /* BT_SECURITY == XA_ENABLED */

    /* Init BIP context state */
    SetContextState(*bipContext, BTL_BIP_CONTEXT_STATE_IDLE);

#if OBEX_AUTHENTICATION == XA_ENABLED
	(*bipContext)->obexAuthFlags = 0;
#endif

	(*bipContext)->pendingDisconnection = FALSE;
	(*bipContext)->radioOffState = BTL_BIPINT_CONTEXT_RADIO_OFF_STATE_IDLE;
	(*bipContext)->asynchronousRadioOff = FALSE;

	(*bipContext)->deleteFileIfAborted = FALSE;
	OS_StrCpyUtf8((*bipContext)->pulledFileName, (BtlUtf8*)"");

	(*bipContext)->objLen = 0;
	
    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPINT_Destroy()
 */
BtStatus BTL_BIPINT_Destroy(BtlBipintContext **bipContext)
{
    BtStatus status = BT_STATUS_SUCCESS;
    BOOL isAllocated = FALSE;

    BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BIPINT_Destroy");

    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));
    BTL_VERIFY_ERR((NULL != *bipContext), BT_STATUS_INVALID_PARM, ("Null *bipContext"));
    BTL_VERIFY_ERR((BTL_BIP_CONTEXT_STATE_IDLE == (*bipContext)->state), BT_STATUS_IN_USE, ("BIP context is in use"));

    status = BTL_POOL_IsElelementAllocated(&btlBipData.contextsPool, *bipContext, &isAllocated);
    BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed locating given BIP context"));
    BTL_VERIFY_ERR((TRUE == isAllocated), BT_STATUS_INVALID_PARM, ("Invalid BIP context"));

    /* Remove the context from the list of all BIP contexts */
    RemoveEntryList(&((*bipContext)->base.node));

    status = BTL_HandleModuleInstanceDestruction(&(*bipContext)->base);
    BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("Failed handling BIP instance destruction (%s)", pBT_Status(status)));

    status = BTL_POOL_Free(&btlBipData.contextsPool, (void **)bipContext);
    BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("Failed freeing BIP context (%s)", pBT_Status(status)));

    /* Set the BIP context to NULL */
    *bipContext = NULL;

    BTL_FUNC_END_AND_UNLOCK();

    return (status);
}

/*-------------------------------------------------------------------------------
 * BTL_BIPINT_Enable()
 */
BtStatus BTL_BIPINT_Enable(BtlBipintContext *bipContext)
{
    BtStatus status = BT_STATUS_SUCCESS;
    BtStatus deregisterStatus;

    BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BIPINT_Enable");

    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));
    BTL_VERIFY_ERR((BTL_BIP_CONTEXT_STATE_IDLE == bipContext->state), BT_STATUS_IMPROPER_STATE,
                    ("context is already enabled"));

    /* Register as a BIP-Initiator, and register initiator callback (bipContext->callback) */
    status = BIP_RegisterInitiator(&bipContext->sessionInitiator,BipintCallback);
    BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status,
                    ("Failed to register BIP Initiator (%s)", pBT_Status(status)));

#if BT_SECURITY == XA_ENABLED

	/* Register security record */
	status = GOEP_RegisterClientSecurityRecord(&(bipContext->sessionInitiator.primary.cApp),
	                                               bipContext->sessionInitiator.primary.cApp.secRecord.level);

  /* Failed? --> cleanup BIP registration. */
	if (BT_STATUS_SUCCESS != status)
	{
        deregisterStatus = BIP_DeregisterInitiator(&bipContext->sessionInitiator);

    		BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == deregisterStatus), ("Failed deregistering BIP Initiator"));

    		/* This verify will fail and will handle the exception gracefully */
    		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed registering BIP Initiator security record"));
	}
  
#endif	/* BT_SECURITY == XA_ENABLED */

    SetContextState(bipContext, BTL_BIP_CONTEXT_STATE_IN_USE);

    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPINT_Disable()
 */
BtStatus BTL_BIPINT_Disable(BtlBipintContext *bipContext)
{
    BtStatus status = BT_STATUS_SUCCESS;

    BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BIPINT_Disable");

    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));
    BTL_VERIFY_ERR((BTL_BIP_CONTEXT_STATE_IN_USE == bipContext->state), BT_STATUS_IMPROPER_STATE,
                    ("context must be enabled"));

#if BT_SECURITY == XA_ENABLED

  	/* First, try to unregister security record */
  	status = GOEP_UnregisterClientSecurityRecord(&bipContext->sessionInitiator.primary.cApp);
  	BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == status), ("Failed unregistering BIP Initiator security record"));

#endif	/* BT_SECURITY == XA_ENABLED */

    /* Still connected? --> disconnect it first */
    if( BTL_BIP_CHANNEL_STATE_CONNECTED == bipContext->channelState )
    {
        SetContextState(bipContext, BTL_BIP_CONTEXT_STATE_DISABLING);
        status = BTL_BIPINT_Disconnect(bipContext);
    }
    /* No connection! --> deregister directly. */
    else
    {
        SetContextState(bipContext, BTL_BIP_CONTEXT_STATE_IDLE);
        status = BIP_DeregisterInitiator(&bipContext->sessionInitiator);

        BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed De-Registering BIP Initiator (%s)", pBT_Status(status)));
    }

#if OBEX_AUTHENTICATION == XA_ENABLED
	bipContext->obexAuthFlags = 0;
#endif

    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

#if BT_SECURITY == XA_ENABLED
/*-------------------------------------------------------------------------------
 * BTL_BIPINT_SetSecurityLevel()
 */
BtStatus BTL_BIPINT_SetSecurityLevel(BtlBipintContext         *bipContext,
                                     const BtSecurityLevel *securityLevel)
{
    BtStatus status = BT_STATUS_SUCCESS;

    BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BIPINT_SetSecurityLevel");

    if (NULL != securityLevel)
    {
        status = BTL_VerifySecurityLevel(*securityLevel);
        BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Invalid BIP securityLevel"));
    }

    /* Save the given security level, or use default */
    if (NULL != securityLevel)
    {
       bipContext->sessionInitiator.primary.cApp.secRecord.level = *securityLevel;
    }
    else
    {
      bipContext->sessionInitiator.primary.cApp.secRecord.level = BSL_NO_SECURITY;
    }

    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPINT_GetSecurityLevel()
 */
BtStatus BTL_BIPINT_GetSecurityLevel(BtlBipintContext   *bipContext,
                                     BtSecurityLevel *securityLevel)
{
    BtStatus status = BT_STATUS_SUCCESS;

    BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BIPINT_GetSecurityLevel");

    *securityLevel = bipContext->sessionInitiator.primary.cApp.secRecord.level;

    BTL_FUNC_END_AND_UNLOCK();

    return status;
}
#endif  /* BT_SECURITY == XA_ENABLED */

/*-------------------------------------------------------------------------------
 * BTL_BIPINT_GetConnectedDevice()
 */
BtStatus BTL_BIPINT_GetConnectedDevice(BtlBipintContext *bipContext,  BD_ADDR *bdAddr)
{
    BtStatus status = BT_STATUS_SUCCESS;
    BOOL retVal;
    ObexTpConnInfo tpInfo;

    BTL_FUNC_START_AND_LOCK("BTL_BIPINT_GetConnectedDevice");

    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));
    BTL_VERIFY_ERR((NULL != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));
    BTL_VERIFY_ERR((BTL_BIP_CHANNEL_STATE_CONNECTED == bipContext->channelState), BT_STATUS_NO_CONNECTION,
                    ("context must be connected"));

    tpInfo.size = sizeof(ObexTpConnInfo);
    tpInfo.tpType = OBEX_TP_BLUETOOTH;
    tpInfo.remDev = NULL;

    retVal = GOEP_ClientGetTpConnInfo(&bipContext->sessionInitiator.primary.cApp, &tpInfo);
    BTL_VERIFY_ERR((TRUE == retVal), BT_STATUS_NO_CONNECTION, ("Failed getting connection info failed"));

    BTL_VERIFY_ERR((NULL != tpInfo.remDev), BT_STATUS_NO_CONNECTION, ("Unable to find connected remote device"));

    *bdAddr = tpInfo.remDev->bdAddr;

    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPINT_Connect()
 */
BtStatus BTL_BIPINT_Connect(BtlBipintContext *bipContext,
                             BD_ADDR       *bdAddr,
							               BipService  service)
{
    BtStatus         status   = BT_STATUS_SUCCESS;
    ObStatus         obStatus = OB_STATUS_SUCCESS;
    BtDeviceContext *bdc;
	  BtDeviceContext  tmplt;

    BTL_FUNC_START_AND_LOCK("BTL_BIPINT_Connect");

    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));
    BTL_VERIFY_ERR((NULL != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));
    BTL_VERIFY_ERR((BTL_BIP_CONTEXT_STATE_IN_USE == bipContext->state), BT_STATUS_IMPROPER_STATE,
                    ("context must be enabled"));
    BTL_VERIFY_ERR((BTL_BIP_CHANNEL_STATE_DISCONNECTED == bipContext->channelState), BT_STATUS_IMPROPER_STATE,
                    ("context must be disconnected"));

    bipContext->responderAddress.type = OBEX_TP_BLUETOOTH;
    bipContext->responderAddress.proto.bt.addr = *bdAddr;

    /* Find a device in DS */
    bdc = DS_FindDevice(bdAddr);

    /* Device found? --> copy psi info from it. */
    if (NULL != bdc)
    {
        bipContext->responderAddress.proto.bt.psi = bdc->psi;
    }
    /* Device not in DeviceSelection table! --> add it with default psi settings.*/
    else
    {
        /* Add this device.*/
/*gesl: QUESTION: why is this necessary?        */
    		OS_MemSet((U8 *)&tmplt, 0, (U32)sizeof(BtDeviceContext));
    		OS_MemCopy((U8 *)&tmplt.addr, (U8 *)bdAddr, sizeof(BD_ADDR));
    		DS_AddDevice(&tmplt, 0);	

        /* Set default psi values for this remote device. */
        OS_MemSet((U8*)&bipContext->responderAddress.proto.bt.psi, 0, sizeof(bipContext->responderAddress.proto.bt.psi));      
    }

	/* Save requested service for later usage */
	bipContext->requestedService = service;

#if OBEX_AUTHENTICATION == XA_ENABLED
	bipContext->obexAuthFlags  &= ~OBEX_AUTH_INFO_SET;
#endif	/* OBEX_AUTHENTICATION == XA_ENABLED */
	
    obStatus = BIP_Connect(&bipContext->sessionInitiator, &bipContext->responderAddress, service);

    if (OB_STATUS_PENDING == obStatus)
    {
        BTL_LOG_DEBUG(("Waiting for connection to establish"));

        /* BIP state is now in the process of connecting */
        SetChannelState(bipContext, BTL_BIP_CHANNEL_STATE_CONNECTING);

        status = BT_STATUS_PENDING;
    }
    else if (OB_STATUS_SUCCESS == status)
    {
        BTL_LOG_INFO(("Connected To Responder (%s)", BTL_UTILS_LockedBdaddrNtoa(bdAddr)));

        SetChannelState(bipContext, BTL_BIP_CHANNEL_STATE_CONNECTED);

        status = BT_STATUS_SUCCESS;
    }
    else
    {
        BTL_ERR(BTL_UTILS_ObStatus2BtStatus(obStatus),
                    ("Failed connecting to Printer (%s), Status: %s", BTL_UTILS_LockedBdaddrNtoa(bdAddr),  BTL_DEBUG_pObStatus(obStatus)));

    }

    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPINT_Disconnect()
 */
BtStatus BTL_BIPINT_Disconnect(BtlBipintContext *bipContext)
{
/*gesl: QUESTION: should the BTL_BIPINT not do an Abort in this case, when an OBEX operation is busy?*/
/*gesl: DECIDE: Under discussion now.*/

    BtStatus    status = BT_STATUS_SUCCESS;

    BTL_FUNC_START_AND_LOCK("BTL_BIPINT_Disconnect");

    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));
    BTL_VERIFY_ERR((BTL_BIP_CHANNEL_STATE_CONNECTED == bipContext->channelState), BT_STATUS_NO_CONNECTION,
                    ("context must be connected"));

	status = BtlBipintDisconnect(bipContext);
	
    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPINT_PutImage()
 */
BtStatus BTL_BIPINT_PutImage(BtlBipintContext *bipContext,
                             BtlBipEncoding         encoding,
                             U16                    width,
                             U16                    height,
                             U32                    size,
                             BtlBipTranformation    transformation,
                             BtlObject              *objToSend)
{
    BtStatus    status = BT_STATUS_SUCCESS;
    BtlBipPixel pixelRange;

    BTL_FUNC_START_AND_LOCK("BTL_BIPINT_PutImage");

    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));
	BTL_VERIFY_ERR((bipContext->requestInProgress == FALSE), BT_STATUS_BUSY, ("Another operation in progress"));

	BtlBipintInitRequestVars(bipContext);
	
	/* udir: Add verifications that:
		1. encoding is not BTL_BIP_ENCODING_ALL
	*/
	
    /* Prepare 'pixel' attribute as fixed width + heigth. */
    pixelRange.widthSmall = width;
    pixelRange.widthLarge = width;
    pixelRange.heightSmall = height;
    pixelRange.heightLarge = height;

    /* Create XML 'Image Descriptor', where 'maxSize' attribute is not relevant (0). */
    btl_bip_XmlBuildDescrImage((S8*)bipContext->xmlDescr,encoding,&pixelRange,size,0,transformation);

    /* Prepare data for BIP (bipData) with PutImage specific data. */
    bipContext->bipData.bipOp = BIPOP_PUT_IMAGE;
    bipContext->bipData.r.rImg.imageDescriptor = bipContext->xmlDescr;
    bipContext->bipData.r.rImg.imageDescriptorLen = OS_StrLenUtf8(bipContext->xmlDescr);
    OS_StrCpyUtf8(bipContext->bipData.r.rImg.name, objToSend->objectName);

    /* Now start the OBEX operation. */
    status = ObexRequest(bipContext, REQUEST_PUT, objToSend);

    BTL_FUNC_END_AND_UNLOCK();

    return status;
}


/*-------------------------------------------------------------------------------
 * BTL_BIPINT_PutLinkedThumbnail()
 */
BtStatus BTL_BIPINT_PutLinkedThumbnail(BtlBipintContext *bipContext,
                                       BipImgHandle          imgHandle,
                                       BtlObject              *objToSend)
{
    BtStatus status = BT_STATUS_SUCCESS;

    BTL_FUNC_START_AND_LOCK("BTL_BIPINT_PutLinkedThumbnail");

    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));
	BTL_VERIFY_ERR(	(btl_bip_IsHandleValid(imgHandle) == TRUE), BT_STATUS_INVALID_PARM,
						("Invalid Image Handle (%s)", (char*)imgHandle));
	BTL_VERIFY_ERR((bipContext->requestInProgress == FALSE), BT_STATUS_BUSY, ("Another operation in progress"));
		
	BtlBipintInitRequestVars(bipContext);
	
    /* Prepare data for BIP (bipData) with PutLinkedThumbnail specific data. */
    bipContext->bipData.bipOp = BIPOP_PUT_LINKED_THUMBNAIL;
    OS_MemCopy(bipContext->bipData.r.rThm.imgHandle, imgHandle, BTL_BIP_IMAGE_HANDLE_LEN + 1);

    /* Now start the OBEX operation. */
    status = ObexRequest(bipContext, REQUEST_PUT, objToSend);

    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPINT_GetCapabilities()
 */
BtStatus BTL_BIPINT_GetCapabilities(BtlBipintContext *bipContext,
                             BtlObject              *responseObj)
{
    BtStatus status = BT_STATUS_SUCCESS;

    BTL_FUNC_START_AND_LOCK("BTL_BIPINT_GetCapabilities");

    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));
	BTL_VERIFY_ERR((bipContext->requestInProgress == FALSE), BT_STATUS_BUSY, ("Another operation in progress"));

	BtlBipintInitRequestVars(bipContext);
    
    /* Prepare data for BIP (bipData) with GetCapabilities specific data. */
    bipContext->bipData.bipOp = BIPOP_GET_CAPABILITIES;

    /* Now start the OBEX operation. */
    status = ObexRequest(bipContext, REQUEST_GET, responseObj);

    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPINT_GetImageList()
 */
BtStatus BTL_BIPINT_GetImagesList(BtlBipintContext *bipContext,
                                  BtlBipFilter     *filter,
                                  BOOL                 latestCapturedImages,
                                  U16                  listStartOffset,
                                  U16                  nbReturnedHandles,
                                  BtlObject           *responseObj)
/* gesl: TODO: process  added parameters 'filter' (replacing ''startdate' + 'enddate')*/
/*       + 'responseObj'. Handle different parameter order (APP)*/
{
    BtStatus status = BT_STATUS_SUCCESS;

    BTL_FUNC_START_AND_LOCK("BTL_BIPINT_GetImagesList");

    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));
	BTL_VERIFY_ERR((bipContext->requestInProgress == FALSE), BT_STATUS_BUSY, ("Another operation in progress"));
    
	BtlBipintInitRequestVars(bipContext);
	
    /* Create XML request. */
    btl_bip_XmlBuildDescrImageHandles((S8*)bipContext->xmlDescr, filter);

    /* Prepare data for BIP (bipData) with GetImagesList specific data. */
    bipContext->bipData.bipOp = BIPOP_GET_IMAGES_LIST;
    bipContext->bipData.r.rList.nbReturnedHandles = nbReturnedHandles;
    bipContext->bipData.r.rList.listStartOffset = listStartOffset;
    if( TRUE == latestCapturedImages )
    {
        bipContext->bipData.r.rList.latestCapturedImages = (U16)1;
    }
    else
    {
        bipContext->bipData.r.rList.latestCapturedImages = (U16)0;
    }
    bipContext->bipData.r.rList.handlesDescriptor = bipContext->xmlDescr;
    bipContext->bipData.r.rList.handlesDescriptorLen = OS_StrLenUtf8(bipContext->xmlDescr);
    bipContext->bipData.totLen = bipContext->bipData.r.rList.handlesDescriptorLen;

    /* Now start the OBEX operation. */
    status = ObexRequest(bipContext, REQUEST_GET, responseObj);

    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPINT_GetImageProperties()
 */
BtStatus BTL_BIPINT_GetImageProperties(BtlBipintContext *bipContext,
                                       BipImgHandle        imgHandle,
                                       BtlObject              *responseObj)
{
    BtStatus status = BT_STATUS_SUCCESS;

    BTL_FUNC_START_AND_LOCK("BTL_BIPINT_GetImageProperties");

    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));
	BTL_VERIFY_ERR(	(btl_bip_IsHandleValid(imgHandle) == TRUE), BT_STATUS_INVALID_PARM,
						("Invalid Image Handle (%s)", (const char*)imgHandle));
	BTL_VERIFY_ERR((bipContext->requestInProgress == FALSE), BT_STATUS_BUSY, ("Another operation in progress"));
    
	BtlBipintInitRequestVars(bipContext);
	
    /* Prepare data for BIP (bipData) with GetImageProperties specific data. */
    bipContext->bipData.bipOp = BIPOP_GET_IMAGE_PROPERTIES;
    OS_MemCopy(bipContext->bipData.r.rProp.imgHandle, imgHandle, BTL_BIP_IMAGE_HANDLE_LEN + 1);

    /* Now start the OBEX operation. */
    status = ObexRequest(bipContext, REQUEST_GET, responseObj);

    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPINT_GetImage()
 */
 BtStatus BTL_BIPINT_GetImage(BtlBipintContext *bipContext,
                             BipImgHandle       imgHandle,
                             BtlBipEncoding         encoding,
                             BtlBipPixel           *pixelRange,
                             U32                    maxSize,
                             BtlBipTranformation    transformation,
                             BtlObject              *responseObj)
{
    BtStatus status = BT_STATUS_SUCCESS;

    BTL_FUNC_START_AND_LOCK("BTL_BIPINT_GetImage");

    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));
	BTL_VERIFY_ERR(	(btl_bip_IsHandleValid(imgHandle) == TRUE), BT_STATUS_INVALID_PARM,
						("Invalid Image Handle (%s)", (const char*)imgHandle));
	BTL_VERIFY_ERR((bipContext->requestInProgress == FALSE), BT_STATUS_BUSY, ("Another operation in progress"));
    
	BtlBipintInitRequestVars(bipContext);
	
    /* Create XML 'Image Descriptor', where 'size' attribute is not relevant (0) */
    btl_bip_XmlBuildDescrImage((S8*)bipContext->xmlDescr,encoding,pixelRange,0,maxSize,transformation);

    /* Prepare data for BIP (bipData) with GetImage specific data. */
    bipContext->bipData.bipOp = BIPOP_GET_IMAGE;
    bipContext->bipData.r.rImg.imageDescriptor = bipContext->xmlDescr;
    bipContext->bipData.r.rImg.imageDescriptorLen = OS_StrLenUtf8(bipContext->xmlDescr);
    OS_MemCopy(bipContext->bipData.r.rImg.imgHandle, imgHandle, BTL_BIP_IMAGE_HANDLE_LEN + 1);

    /* Now start the OBEX operation. */
    status = ObexRequest(bipContext, REQUEST_GET, responseObj);

    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPINT_GetLinkedThumbnail()
 */
BtStatus BTL_BIPINT_GetLinkedThumbnail(BtlBipintContext *bipContext,
                                       BipImgHandle        imgHandle,
                                       BtlObject              *responseObj)
{
    BtStatus status = BT_STATUS_SUCCESS;

    BTL_FUNC_START_AND_LOCK("BTL_BIPINT_GetLinkedThumbnail");

    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));
    	BTL_VERIFY_ERR(	(btl_bip_IsHandleValid(imgHandle) == TRUE), BT_STATUS_INVALID_PARM,
						("Invalid Image Handle (%s)", (const char*)imgHandle));
	BTL_VERIFY_ERR((bipContext->requestInProgress == FALSE), BT_STATUS_BUSY, ("Another operation in progress"));

	BtlBipintInitRequestVars(bipContext);
	
    /* Prepare data for BIP (bipData) with GetLinkedThumbnail specific data. */
    bipContext->bipData.bipOp = BIPOP_GET_LINKED_THUMBNAIL;
    OS_MemCopy(bipContext->bipData.r.rThm.imgHandle, imgHandle, BTL_BIP_IMAGE_HANDLE_LEN + 1);

    /* Now start the OBEX operation. */
    status = ObexRequest(bipContext, REQUEST_GET, responseObj);

    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPINT_GetMonitoringImage()
 */
BtStatus BTL_BIPINT_GetMonitoringImage(BtlBipintContext *bipContext,
                                       BOOL                    storeFlag,
                                       BtlObject              *responseObj)
{
    BtStatus  status = BT_STATUS_SUCCESS;

    BTL_FUNC_START_AND_LOCK("BTL_BIPINT_GetMonitoringImage");

    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));
	BTL_VERIFY_ERR((bipContext->requestInProgress == FALSE), BT_STATUS_BUSY, ("Another operation in progress"));
    
	BtlBipintInitRequestVars(bipContext);
	
    /* Prepare data for BIP (bipData) with GetMonitoringImage specific data. */
    bipContext->bipData.bipOp = BIPOP_GET_MONITORING_IMAGE;
    if (TRUE == storeFlag)
    {
        bipContext->bipData.r.rMon.storeFlag = 0x01;
    }
    else
    {
        bipContext->bipData.r.rMon.storeFlag = 0x00;
    }

    /* Now start the OBEX operation. */
    status = ObexRequest(bipContext, REQUEST_GET, responseObj);

    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPINT_Abort()
 *
 */
BtStatus BTL_BIPINT_Abort(BtlBipintContext *bipContext)
{
    BtStatus  status = BT_STATUS_SUCCESS;

    BTL_FUNC_START_AND_LOCK("BTL_BIPINT_Abort");

    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));

    BIP_Abort(&bipContext->sessionInitiator, BIPCH_INITIATOR_PRIMARY);

    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPINT_ParseGetCapabilitiesResponse()
 */
BtStatus BTL_BIPINT_ParseGetCapabilitiesResponse(BtlBipintContext *bipContext,
                                      			const BtlObject 				*responseObj,
                                      			BtlBipintCapabilitiesElements		*elementsData,
                                      			BtlBipintCapabilitiesElementsMask	*elementsMask)
{
	BtStatus		status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK("BTL_BIPINT_ParseGetCapabilitiesResponse");

	BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));
	BTL_VERIFY_ERR((NULL != responseObj), BT_STATUS_INVALID_PARM, ("Null responseObj"));
	BTL_VERIFY_ERR(NULL != elementsData, BT_STATUS_INVALID_PARM, ("Null elementsData"));
	BTL_VERIFY_ERR(NULL != elementsMask, BT_STATUS_INVALID_PARM, ("Null elementsMask"));

	status = btl_bip_XmlParseGetCapabilitiesResponse(
							responseObj,
							elementsData,
							elementsMask );

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPINT_ParseGetImagePropertiesResponse()
 */
BtStatus BTL_BIPINT_ParseGetImagePropertiesResponse(BtlBipintContext *bipContext,
                                      			const BtlObject 				*responseObj,
                                      			BtlBipintImgPropertiesElements		*elementsData,
                                      			BtlBipintImgPropertiesElementsMask	*elementsMask)
{

	BtStatus		status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK("BTL_BIPINT_ParseGetImagePropertiesResponse");

	BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));
	BTL_VERIFY_ERR((NULL != responseObj), BT_STATUS_INVALID_PARM, ("Null responseObj"));
	BTL_VERIFY_ERR(NULL != elementsData, BT_STATUS_INVALID_PARM, ("Null elementsData"));
	BTL_VERIFY_ERR(NULL != elementsMask, BT_STATUS_INVALID_PARM, ("Null elementsMask"));

	status = btl_bip_XmlParseGetImagePropertiesResponse(
							responseObj,
							elementsData,
							elementsMask );

	BTL_FUNC_END_AND_UNLOCK();

	return status;

}

/*-------------------------------------------------------------------------------
 * BTL_BIPINT_ParseGetImagesListResponse()
 */
BtStatus BTL_BIPINT_ParseGetImagesListResponse(BtlBipintContext *bipContext,
                                      			const BtlObject 				*responseObj,
                                      			BtlBipintImagesListMetaData     *elementsMetaData,
                                      			BtlBipintImagesListElementsMask	*elementsMask)
{
	BtStatus		status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK("BTL_BIPINT_ParseGetImagesListResponse");

	BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));
	BTL_VERIFY_ERR((NULL != responseObj), BT_STATUS_INVALID_PARM, ("Null responseObj"));
	BTL_VERIFY_ERR(NULL != elementsMetaData, BT_STATUS_INVALID_PARM, ("Null elementsData"));
	BTL_VERIFY_ERR(NULL != elementsMask, BT_STATUS_INVALID_PARM, ("Null elementsMask"));

	status = btl_bip_XmlParseGetImagesListResponse(bipContext->nrLatestReturnedhandles,
							responseObj,
							elementsMetaData,
							elementsMask );

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}


/*-------------------------------------------------------------------------------
 * BTL_BIPINT_ParseNextFromImagesList()
 */
BtStatus BTL_BIPINT_ParseNextFromImagesList(BtlBipintImagesListMetaData *elementsMetaData, BtlBipImagesListing *imageInfo)
{
	BtStatus		status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK("BTL_BIPINT_ParseNextFromImagesList");

	BTL_VERIFY_ERR((NULL != elementsMetaData), BT_STATUS_INVALID_PARM, ("Null elementsMetaData"));
	BTL_VERIFY_ERR((NULL != imageInfo), BT_STATUS_INVALID_PARM, ("Null imageInfo"));
	BTL_VERIFY_ERR(NULL != elementsMetaData->xmlImgListObjectOffset, BT_STATUS_INVALID_PARM, ("Null elementsMetaData->xmlImgListObjectOffset"));

    /* parse the imagelist now for handles */
    status = btl_bip_XmlParseNextFromImagesList(elementsMetaData,imageInfo);

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

/*-------------------------------------------------------------------------*
 *  LOCAL FUNCTIONS
 *-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 *            bipContext->callback
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Callback function for BIP Initiator Events from the BIP module
 *
 * Return:    void
 *
 */
static void BipintCallback(BipCallbackParms* parms)
{
    BtStatus           status = BT_STATUS_SUCCESS;
    BtlBipintEvent     bipEvent;
    BOOL               bCloseFile;
    BtlBipintContext  *bipContext;
    BOOL               passEventToApp = TRUE;

    BTL_FUNC_START("BipintCallback");

    /* Get correct context from the event received. */
    bipContext = GetContextFromBipCallbackParms(parms);

    /* Prepare event for the APP. */
    bipEvent.context = bipContext;
    bipEvent.parms   = parms;

    /* gesl: Why is this done? */
    if(NULL == bipContext->bipObexClient)
    {
        bipContext->bipObexClient = parms->obex.client;
    }

	BTL_LOG_INFO(("BipintCallback: Event: %s, Op: %s, Data Type: %s", 
					btl_bip_GetBipEventString(parms->event), 
					btl_bip_GetBipOpString(parms->data->bipOp),
					btl_bip_GetBipDataTypeString(parms->data->dataType)));
	
    /* Switch first on event. */
    switch (parms->event)
    {
        /* Repsonder wants to have data from me? --> provide it.*/
        case BIPINITIATOR_DATA_REQ:
			
		BtlBipintSendProgressNotificationToApp(bipContext,  parms, parms->data->offset + parms->data->len);
		
            if (NULL != parms->data->ocx )
            {
                /* Copy the file into memory */
                status = btl_bip_DataRead(parms->data);
				
                /* Error detected while reading? --> abort OBEX session. */
                if( BT_STATUS_SUCCESS != status )
                {
                    BIP_Abort(&bipContext->sessionInitiator, parms->channel);
                }
                
                /* Event is completely handled inside. */
                passEventToApp = FALSE;
            }
            else
            {
                /*App_Report("Initiator error writing image, channel = %x", parms->channel);*/
                parms->data->len = 0;
            }
            break;

        case BIPINITIATOR_DATA_IND:
		
		if (parms->data->offset == 0)
		{
			BTL_LOG_INFO(("BipintCallback: Offset is 0 => setting objLen to 0"));
			bipContext->objLen = 0;
		}
				
            /* Store retrieved info in the 'responsObj' as defined by the Get*** function. */
		if ( 	(BIPIND_IMAGE                == parms->data->dataType) ||
			(BIPIND_THUMBNAIL            == parms->data->dataType) ||
			(BIPIND_IMG_LISTING_XML      == parms->data->dataType) ||
			(BIPIND_IMG_CAPABILITIES_XML == parms->data->dataType) ||
			(BIPIND_IMG_PROPERTIES_XML   == parms->data->dataType))
		{
			if (parms->data->offset < parms->data->totLen)
			{
				bipContext->objLen += parms->data->len;

				BtlBipintSendProgressNotificationToApp(bipContext,  parms, bipContext->objLen);
			}		

			/* Store in file? --> add data to the file. */
			/*gesl: QUESTION: check on limits?*/

			BTL_LOG_INFO(("BipintCallback: Writing to File / Memory"));		
			status = btl_bip_DataWrite(parms->data);

			if (status != BT_STATUS_SUCCESS)
			{
				BIP_Abort(&bipContext->sessionInitiator, parms->channel);
				BTL_ERR_NO_RETVAR(("BipintCallback: btl_bip_DataWrite Failed (%s)", pBT_Status(status))); 
			}
		}
		else
		{
			BTL_LOG_INFO(("BipintCallback: Ignore Indication - Not writing to File / Memory"));
		}
		
            break;

        case BIPINITIATOR_RESPONSE:
            /* Parse the response */
            switch (parms->data->bipOp)
            {
                case BIPOP_GET_IMAGE:
                case BIPOP_GET_LINKED_THUMBNAIL:
                case BIPOP_GET_MONITORING_IMAGE:
                     /* Nothing has to be done here since file is already opened
                      * or we need to write to memory
                      */
/*gesl: QUESTION: BIP_InitiatorAccept needed as below?*/

                    break;

                case BIPOP_GET_IMAGES_LIST:
                case BIPOP_GET_CAPABILITIES:
                case BIPOP_GET_IMAGE_PROPERTIES:
                    /* Received an XML response, the data will be indicated
                     * in  BIPINITIATOR_DATA_IND event
                     */
                    BIP_InitiatorAccept(bipContext->bipObexClient,parms->data);
/*gesl  QUESTION: is BIP_InitiatorAccept used elsewhere?*/
/*gesl: INVESTIGATE: what is exactly the purpose of this function. I cannot see it being used in the ESI example APP or elsewhere.*/
                    break;
                default:
                    BIP_Abort(bipContext, parms->channel);
                    break;
            }
            break;

        case BIPINITIATOR_COMPLETE:
            bCloseFile = FALSE;
		bipEvent.parms->data->totLen = bipContext->objLen;
		bipContext->requestInProgress = FALSE;

            switch (parms->data->bipOp)
            {
                case BIPOP_CONNECT:
                    /* It was indeed a new connection? --> update admin + notify the APP.*/
                    if (parms->status == OB_STATUS_SUCCESS)
                    {
                        /* Update my channel state-machine */
                        SetChannelState(bipContext, BTL_BIP_CHANNEL_STATE_CONNECTED);
                        /* Change event to make it clear for the APP. */
                        parms->event = BIPINITIATOR_CONNECTED;
                    }
                    /* It was a disconnection? --> update admin + notify the APP.*/
                    else if (parms->status == OB_STATUS_DISCONNECT)
                    {
                        /* Update my channel state-machine */
                        SetChannelState(bipContext, BTL_BIP_CHANNEL_STATE_DISCONNECTED);
                        /* Change event to make it clear for the APP. */
                        parms->event = BIPINITIATOR_DISCONNECTED;

#if OBEX_AUTHENTICATION == XA_ENABLED
				bipContext->obexAuthFlags = 0;
#endif
                    }

                    break;
                case BIPOP_GET_IMAGE:
                case BIPOP_PUT_IMAGE:
                    if (parms->data->r.rImg.imageDescriptor != NULL)
                    {
                        parms->data->r.rImg.imageDescriptor = NULL;
                        parms->data->r.rImg.imageDescriptorLen = 0;
                    }
                    bCloseFile = TRUE;

                    break;
                case BIPOP_PUT_LINKED_THUMBNAIL:
                case BIPOP_GET_LINKED_THUMBNAIL:
                case BIPOP_GET_MONITORING_IMAGE:
                    bCloseFile = TRUE;
                    break;

			case BIPOP_GET_CAPABILITIES:
			case BIPOP_GET_IMAGE_PROPERTIES:

				bCloseFile = TRUE;
				break;

                case BIPOP_GET_IMAGES_LIST:
                    if (parms->data->r.rList.handlesDescriptor != NULL)
                    {
                        parms->data->r.rList.handlesDescriptor = NULL;
                        parms->data->r.rList.handlesDescriptorLen = 0;
                    }

			bCloseFile = TRUE;
			
                    break;
                default:
                    break;
            }

		if( TRUE == bCloseFile )
		{
			if (parms->data->ocx != NULL)
			{
				BtlBipObjStore		*objStore = parms->data->ocx;
				
				if (objStore->type == BTL_BIP_OBJ_STORE_TYPEHANDLE)
				{
					(void) btl_bip_FileClose((BthalFsFileDesc *)&(objStore->location.fileHandle));
					objStore->type = BTL_BIP_OBJ_STORE_TYPENONE;

					bipContext->deleteFileIfAborted = FALSE;
					OS_StrCpyUtf8(bipContext->pulledFileName, (BtlUtf8*)"");

					/* Clear admin. */
					parms->data->ocx = NULL;
				}
			}
		}
		
            break;

        case BIPINITIATOR_ABORT:
		if (bipContext->channelState == BTL_BIP_CHANNEL_STATE_CONNECTING)
		{
			/* Change event to make it clear for the APP. */
			parms->data->bipOp = BIPOP_CONNECT;

	        SetChannelState(bipContext, BTL_BIP_CHANNEL_STATE_DISCONNECTED);

			passEventToApp = TRUE;

			GOEP_TpDisconnect(&parms->obex.client->cApp);
		}
		else
		{
			/*gesl: TODO: handle this exception (see e-mail from Yaniv), by executiong a disconnection now?*/
			/*            In other words: when executing a Disconnect, should it be preceeded by an Abort first?*/

			if (bipContext->objStore.type == BTL_BIP_OBJ_STORE_TYPEHANDLE)
			{
		            /* File was open for reading or writing? --> close it. */
		           (void) btl_bip_FileClose((BthalFsFileDesc *)&(bipContext->objStore.location.fileHandle));
			}

			if (bipContext->deleteFileIfAborted == TRUE)
			{
				BTL_LOG_INFO(	("Operation Aborted, Deleting File Data That was received so far (%s)", 
								(char*)bipContext->pulledFileName));
			
				btl_bip_FileDelete(bipContext->pulledFileName);

				bipContext->deleteFileIfAborted = FALSE;
				OS_StrCpyUtf8(bipContext->pulledFileName, (BtlUtf8*)"");
			}

	           bipContext->objStore.type = BTL_BIP_OBJ_STORE_TYPENONE;

#if OBEX_AUTHENTICATION == XA_ENABLED
				if (bipContext->state == BTL_BIPINT_STATE_IMAGEPUSH_CONNECTING)
					bipContext->obexAuthFlags &= ~OBEX_AUTH_INFO_SET;
#endif

			bipContext->bipData.bipOp = BIPOP_NOP;

	            break;

#if OBEX_AUTHENTICATION == XA_ENABLED
			case BIPINITIATOR_AUTH_CHALLENGE_RCVD:

				if ( ! (bipContext->obexAuthFlags & OBEX_AUTH_INFO_SET) )
				{	
					bipContext->obexAuthFlags |= OBEX_AUTH_RECEIVED_CHAL | OBEX_AUTH_DURING_CALLBACK;

					bipContext->obexAuthFlags |= OBEX_AUTH_INFO_SET;

					bipContext->callback(&bipEvent);
					
					bipContext->obexAuthFlags &= ~OBEX_AUTH_DURING_CALLBACK;

					/* Event is already passed to app */
					passEventToApp = FALSE;
				}
			}
		
			break;
#endif

	case BIPINITIATOR_FAILED:

		if (bipContext->channelState == BTL_BIP_CHANNEL_STATE_CONNECTING)
		{
			parms->data->bipOp = BIPOP_CONNECT;

	        SetChannelState(bipContext, BTL_BIP_CHANNEL_STATE_DISCONNECTED);

			passEventToApp = TRUE;
			
			GOEP_TpDisconnect(&parms->obex.client->cApp);
		}
		
		break;
		
        default:
        break;
    }

    /* APP should be notified? --> do it. */
  	if (TRUE == passEventToApp)
  	{
        BTL_LOG_INFO(("BipCallback: event passed to APP"));
        bipContext->callback(&bipEvent);

        /* Check if some after processing is necessary. */
        AfterProcessing(bipContext, &bipEvent);
  	}

    BTL_FUNC_END();
}

/*-------------------------------------------------------------------------------
 * AfterProcessing()
 *
 *    Check if there is some processing necessary for this 'bipContext' due to
 *    the latest 'event' sent to the application.
 *
 * Parameters:
 *		bipEvent [in] - Latest event, sent to the application.
 *
 * Returns:
 *    void
 */
static void AfterProcessing(BtlBipintContext* bipContext, BtlBipintEvent *bipEvent)
{
    BtStatus status;

    BTL_FUNC_START("AfterProcessing");

    switch(bipEvent->context->state)
    {
    case BTL_BIP_CONTEXT_STATE_DISABLING:
      /* Last disconnect event to the APP during disabling?                        */
      /*  --> notify APP that the disabling is finished + reset the context state. */
      if(bipEvent->parms->event == BIPINITIATOR_DISCONNECTED)
      {
          /* Remove the registration from the BIP profile. */
          status = BIP_DeregisterInitiator(&(bipEvent->context->sessionInitiator));

          /* Verify if it was indeed a success, otherwise make a note. */
          BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status),
                         status, ("Disable of BIP Initiator failed: (%s)",
                         pBT_Status(status)));

          SetContextState(bipEvent->context, BTL_BIP_CONTEXT_STATE_IDLE);

          /* Notify APP with this new event. */
          bipEvent->parms->event = BIPINITIATOR_DISABLED;
          bipEvent->context->callback(bipEvent);

		 return;
      }
      break;
    }

	if ((bipEvent->parms->event == BIPINITIATOR_CONNECTED) && (bipContext->pendingDisconnection == TRUE))
	{
		bipContext->pendingDisconnection = FALSE;
		
		status = BtlBipintDisconnect(bipContext);

		if (status == BT_STATUS_SUCCESS)
		{
			bipEvent->parms->event = BIPINITIATOR_DISCONNECTED;
		}
	}

	if (	(bipEvent->parms->event == BIPINITIATOR_DISCONNECTED) && 
		(bipContext->radioOffState != BTL_BIPINT_RADIO_OFF_STATE_IDLE))
	{
		BtlBipintContextRadioOffProcessor(bipContext);
	}

    BTL_FUNC_END();
}


/*-------------------------------------------------------------------------------
 * ObexRequest()
 *
 *    This routine will start an OBEX session by sending an OBEX request
 *    to the remote Responder as connected to this Initiator ('bipContext').
 *    The specific data for this OBEX request is already prepared
 *    in the 'bipContext' administration and 'object'.
 *
 *    The 'operation' indicates a Put or Get operation
 *    - 'REQUEST_PUT' = Put operation: when the 'object' is stored in a file,
 *                      it will be openend for reading for transmitting
 *                      the 'object'.
 *
 *    - 'REQUEST_GET' = get operation: when the 'object' should be stored
 *                      in a file, it will be created for writing when
 *                      receiving the 'object' data from the remote
 *                      Responder
 */
static BtStatus ObexRequest(BtlBipintContext *bipContext,
                              requestType  operation,
                              BtlObject *object)
{
    BtStatus        status = BT_STATUS_SUCCESS; /* function return. */
    BtlBipObjStore *objStore;

    BTL_FUNC_START("ObexRequest");

    objStore = &bipContext->objStore;

    /* Clear data for BIP. */
    bipContext->bipData.len    = 0;
    bipContext->bipData.offset = 0;

    /* Store object in the file-system? --> open/create the file. */
    if( BTL_OBJECT_LOCATION_FS == object->objectLocation )
    {
        objStore->type = BTL_BIP_OBJ_STORE_TYPEHANDLE;
        bipContext->bipData.ocx = (void*) objStore;

/*gesl: TODO: add ASSERT check on ( NULL != object->location.fsLocation.fsPath )*/

        /* PUT operation? --> open file for reading. */
        if (REQUEST_PUT == operation)
        {
            status =  btl_bip_FileOpen(FALSE,
			                               	object->location.fsLocation.fsPath,
								NULL, /* path should also contain filename. */
								&(bipContext->objStore.location.fileHandle),
								&bipContext->bipData);
		BTL_VERIFY_ERR(status == BT_STATUS_SUCCESS, status, ("btl_bip_FileOpen Failed (%s)", pBT_Status(status)));
        }
        /* GET operation! --> create file for writing. */
        else
        {
            status =  btl_bip_FileOpen(TRUE,
                              				object->location.fsLocation.fsPath,
                               				NULL, /* path should also contain filename. */
                              				&(bipContext->objStore.location.fileHandle),
                              				&bipContext->bipData);
		BTL_VERIFY_ERR(status == BT_STATUS_SUCCESS, status, ("btl_bip_FileOpen Failed (%s)", pBT_Status(status)));

		BTL_VERIFY_FATAL(bipContext->deleteFileIfAborted == FALSE, BT_STATUS_INTERNAL_ERROR, 
							("deleteFileIfAborted flag already TRUE"));
		BTL_VERIFY_FATAL(OS_StrCmpUtf8(bipContext->pulledFileName, (BtlUtf8*)"") == 0, BT_STATUS_INTERNAL_ERROR, 
							("pulledFileName Is not Empty (%s)", (char*)bipContext->pulledFileName));
		
		bipContext->deleteFileIfAborted = TRUE;
		OS_StrCpyUtf8(bipContext->pulledFileName, object->location.fsLocation.fsPath);

        }
    }
    /* Store object in memory! --> fill necessary settings*/
    else
    {
        objStore->type = BTL_BIP_OBJ_STORE_TYPEMEMPTR;
        objStore->location.mem = (void*)object->location.memLocation.memAddr;
        bipContext->bipData.ocx = (void*)objStore;
        bipContext->bipData.totLen = object->location.memLocation.size;
    }

    /* Everything still OK? -> start the OBEX operation. */
    if( BT_STATUS_SUCCESS == status )
    {
    	BTL_LOG_INFO(("ObexRequest: Calling BIP_SendRequest"));
        status = BIP_SendRequest(&bipContext->sessionInitiator, &bipContext->bipData);
    }

    BTL_FUNC_END();

    return (status);
}

/*-------------------------------------------------------------------------------
 * GetContextFromBipCallbackParms()
 *
 *     Extract the unique context from the event parameters ('parms')
 */
static BtlBipintContext *GetContextFromBipCallbackParms(BipCallbackParms* parms)
{
    BtlBipintContext *bipContext = NULL;

    BTL_FUNC_START("BTL_BIPRSP_GetContextFromBipCallbackParms");

    switch (parms->event)
    {
        case BIPINITIATOR_DATA_REQ:
        case BIPINITIATOR_DATA_IND:
        case BIPINITIATOR_RESPONSE:
        case BIPINITIATOR_COMPLETE:
        case BIPINITIATOR_FAILED:
        case BIPINITIATOR_ABORT:
            /* Subtract the offset of the field BtlBipintContext->sessionInitiator from the address
             * of parms->obex.client to get the address of BtlBipintContext.
             * BtlBipintContext->sessionInitiator is given as parameter to the BIP_xxx functions which
             * is the parms->obex.client in the callback.
             */
            bipContext = ContainingRecord(parms->obex.client, BtlBipintContext, sessionInitiator);
            break;
/*gesl: QUESTION: is this not always the case? Can the 'ContainingRecord' not simply be called directly?*/
        default:
            bipContext = NULL;
            break;
    };

    BTL_FUNC_END();

    return bipContext;
}

/*-------------------------------------------------------------------------------
 * InitOperationVariables()
 */
static void InitOperationVariables(BtlBipintContext* context)
{
    context->bipData.bipOp              = BIPOP_NOP;
    context->bipData.dataType           = BIPIND_NO_DATA;
    context->bipData.len                = 0;
    context->bipData.ocx                = NULL;
    context->bipData.offset             = 0;
    context->bipData.totLen             = 0;
    context->bipObexClient              = NULL;
    context->objStore.type              = BTL_BIP_OBJ_STORE_TYPENONE;
    context->objStore.location.fileHandle = BTHAL_FS_INVALID_FILE_DESC;
    context->callback                   = NULL;
    context->imagePushState             = BTL_BIPINT_STATE_IMAGEPUSH_IDLE;
    context->xmlDescr[0]                = 0;
    context->serviceName[0]             = 0;

    SetContextState(context, BTL_BIP_CONTEXT_STATE_IDLE);
    SetChannelState(context, BTL_BIP_CHANNEL_STATE_DISCONNECTED);
}

/*-------------------------------------------------------------------------------
 * BTL_BIPINT_Disconnect()
 */
BtStatus BtlBipintDisconnect(BtlBipintContext *bipContext)
{
/*gesl: QUESTION: should the BTL_BIPINT not do an Abort in this case, when an OBEX operation is busy?*/
/*gesl: DECIDE: Under discussion now.*/

    BtStatus    status = BT_STATUS_SUCCESS;
    ObStatus obStatus;

    BTL_FUNC_START("BtlBipintDisconnect");
		
	switch (bipContext->channelState)
	{
	case BTL_BIP_CHANNEL_STATE_DISCONNECTED:

		BTL_LOG_DEBUG(("Channel already Disconnected"));
		BTL_RET(BT_STATUS_SUCCESS);
		
	
	case BTL_BIP_CHANNEL_STATE_CONNECTING:

		BTL_LOG_DEBUG(("Channel in in the process of connecting, disconnection will occur upon connection establishment"));
		
		bipContext->pendingDisconnection = TRUE;
		
		BTL_RET(BT_STATUS_PENDING);

	
	case BTL_BIP_CHANNEL_STATE_CONNECTED:

		/* Actually Disconnect - Code continues after switch Case */

	break;
	
	case BTL_BIP_CHANNEL_STATE_DISCONNECTING:

		/* Do nothing, wait for disconnection to complete */

		BTL_RET(BT_STATUS_PENDING);

	}

	/* Channel currently connected - attempt to disconnect */
	
	obStatus = BIP_Disconnect(&bipContext->sessionInitiator);
		
	if (OB_STATUS_PENDING == obStatus)
	{
		/* BIP state is now in the process of disconnecting */
		SetChannelState(bipContext, BTL_BIP_CHANNEL_STATE_DISCONNECTING);
	}
	else if (OB_STATUS_SUCCESS == obStatus)
	{
		SetChannelState(bipContext, BTL_BIP_CHANNEL_STATE_DISCONNECTED);
	}
	else if (OB_STATUS_BUSY == obStatus)
	{
		obStatus = GOEP_TpDisconnect(&bipContext->sessionInitiator.primary.cApp);
		BTL_VERIFY_FATAL((OB_STATUS_PENDING == obStatus) || (OB_STATUS_SUCCESS == obStatus), BT_STATUS_INTERNAL_ERROR, 
							("Unexpected Result (%s) from GOEP_TpDisconnect", BTL_UTILS_ObStatus2BtStatus(obStatus)));

		if (OB_STATUS_SUCCESS == obStatus)
		{
			SetChannelState(bipContext, BTL_BIP_CHANNEL_STATE_DISCONNECTED);
		}
		else
		{
			/* BIP state is now in the process of disconnecting */
			SetChannelState(bipContext, BTL_BIP_CHANNEL_STATE_DISCONNECTING);
		}
	}
	else
	{
		BTL_ERR(BTL_UTILS_ObStatus2BtStatus(obStatus), ("BIP_Disconnect Failed (%s)", BTL_DEBUG_pObStatus(obStatus)));
	}

	status = BTL_UTILS_ObStatus2BtStatus(obStatus);
	
	BTL_FUNC_END();

	return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPINT_RadioOffProcessor()
 */
BtStatus BtlBipintContextRadioOffProcessor(BtlBipintContext *bipContext)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BOOL	keepProcessing = TRUE;
	
	BTL_FUNC_START("BtlBipintContextRadioOffProcessor");

	while (keepProcessing == TRUE)
	{
		keepProcessing = FALSE;

		switch (bipContext->radioOffState)
		{
			case BTL_BIPINT_CONTEXT_RADIO_OFF_STATE_IDLE:

				BTL_VERIFY_ERR((BTL_BIP_CONTEXT_STATE_DISABLING != bipContext->state), BT_STATUS_IN_PROGRESS, 
								("Disabling already in progress"));

				bipContext->asynchronousRadioOff = FALSE;
				bipContext->radioOffState = BTL_BIPINT_CONTEXT_RADIO_OFF_STATE_DISCONNECTING;
				
				status = BtlBipintDisconnect(bipContext);
				BTL_VERIFY_FATAL((status == BT_STATUS_SUCCESS) || (BT_STATUS_PENDING == status), status, (""));
				
				if (status == BT_STATUS_SUCCESS)
				{
					bipContext->radioOffState = BTL_BIPINT_CONTEXT_RADIO_OFF_STATE_DONE;
					keepProcessing = TRUE;
				}
				else
				{
					bipContext->asynchronousRadioOff = TRUE;
				}

			break;
			
 	 		case BTL_BIPINT_CONTEXT_RADIO_OFF_STATE_DISCONNECTING:

				if (bipContext->channelState == BTL_BIP_CHANNEL_STATE_DISCONNECTED)
				{				
					bipContext->radioOffState = BTL_BIPINT_CONTEXT_RADIO_OFF_STATE_DONE;
					keepProcessing = TRUE;
				}

			break;
			 
			case BTL_BIPINT_CONTEXT_RADIO_OFF_STATE_DONE:
				
				if (bipContext->asynchronousRadioOff == TRUE)
				{
					BtlBipintRadioOffProcessor(BTL_BIPINT_RADIO_OFF_EVENT_CONTEXT_COMPLETED);	
				}

				bipContext->radioOffState = BTL_BIPINT_CONTEXT_RADIO_OFF_STATE_IDLE;
				
			break;

			default:

				BTL_FATAL(BT_STATUS_INTERNAL_ERROR, ("Unexpected Radio Off (%d)", bipContext->radioOffState));
				
		};
	}
	
	BTL_FUNC_END();

	return status;
}

BtStatus BtlBipintRadioOffProcessor(BtlBipintRadioOffEvent event)
{
	BtStatus 		status = BT_STATUS_SUCCESS;
	BtlContext	*context = 0;
	BOOL		keepProcessing = FALSE;

	BTL_FUNC_START("BtlBipintRadioOffProcessor");

	keepProcessing = TRUE;

	while (keepProcessing == TRUE)
	{

		keepProcessing = FALSE;

		switch (btlBipData.radioOffState)
		{
			case BTL_BIPINT_RADIO_OFF_STATE_IDLE:

				BTL_VERIFY_FATAL(event == BTL_BIPINT_RADIO_OFF_EVENT_START, BT_STATUS_INTERNAL_ERROR, ("Unexpected Event (%d), event"));

				btlBipData.asynchronousRadioOff = FALSE;
				btlBipData.numOfDisconnectingContexts = 0;
				btlBipData.radioOffState = BTL_BIPINT_RADIO_OFF_STATE_DISCONNECTING;
				
				IterateList(btlBipData.contextsList, context, BtlContext*)
				{
					status = BtlBipintContextRadioOffProcessor((BtlBipintContext*)context);

					BTL_VERIFY_FATAL_NORET((status == BT_STATUS_SUCCESS) || (status == BT_STATUS_PENDING),
												("BtlBipintContextRadioOffProcessor Returned an unexpected status (%s)", pBT_Status(status)));

					if (status == BT_STATUS_PENDING)
					{
						++btlBipData.numOfDisconnectingContexts;
					}
				}

				if (btlBipData.numOfDisconnectingContexts == 0)
				{
					status = BT_STATUS_SUCCESS;
					btlBipData.radioOffState = BTL_BIPINT_RADIO_OFF_STATE_DONE;
					keepProcessing = TRUE;
				}
				else
				{
					status = BT_STATUS_PENDING;
					btlBipData.asynchronousRadioOff = TRUE;
				}

			break;

			case BTL_BIPINT_RADIO_OFF_STATE_DISCONNECTING:

				BTL_VERIFY_FATAL(event == BTL_BIPINT_RADIO_OFF_EVENT_CONTEXT_COMPLETED, BT_STATUS_INTERNAL_ERROR, ("Unexpected Event (%d), event"));
				BTL_VERIFY_FATAL(btlBipData.numOfDisconnectingContexts > 0, BT_STATUS_INTERNAL_ERROR, ("Not expecting any contexts to disconnect"));

				--btlBipData.numOfDisconnectingContexts;

				if (btlBipData.numOfDisconnectingContexts == 0)
				{
					btlBipData.radioOffState = BTL_BIPINT_RADIO_OFF_STATE_DONE;
					keepProcessing = TRUE;
				}

			break;
			
			case BTL_BIPINT_RADIO_OFF_STATE_DONE:

				if (btlBipData.asynchronousRadioOff == TRUE)
				{
					BtlModuleNotificationCompletionEvent	moduleCompletedEvent;
					moduleCompletedEvent.type = BTL_MODULE_NOTIFICATION_COMPLETION_EVENT_TYPE_RADIO_OFF;
					moduleCompletedEvent.status = BT_STATUS_SUCCESS;
						
					 BTL_ModuleCompleted(BTL_MODULE_TYPE_BIPINT , &moduleCompletedEvent);
				}

				btlBipData.radioOffState = BTL_BIPINT_RADIO_OFF_STATE_IDLE;

			break;
		};
	};

	BTL_FUNC_END();

	return status;
}

/*-------------------------------------------------------------------------------
 * BtlBipintBtlNotificationsCb()
 */
BtStatus BtlBipintBtlNotificationsCb(BtlModuleNotificationType notificationType)
{
  	BtStatus status = BT_STATUS_SUCCESS;

  	BTL_FUNC_START("BtlBipintBtlNotificationsCb");

  	BTL_LOG_DEBUG(("Notification Type: %d", notificationType));

  	switch (notificationType)
  	{
    		case BTL_MODULE_NOTIFICATION_RADIO_ON:

      			/* Do something if necessary */

      			break;

    		case BTL_MODULE_NOTIFICATION_RADIO_OFF:

			status = BtlBipintRadioOffProcessor(BTL_BIPINT_RADIO_OFF_EVENT_START);

      			break;

    		default:

      			BTL_ERR(BT_STATUS_INTERNAL_ERROR, ("Invalid notification (%d)", notificationType));



  	};

  	BTL_FUNC_END();

  	return status;
}

/*-------------------------------------------------------------------------------
 * SetContextState()
 *
 *    Fill the 'state' parameter in the 'bipContext' with the new 'state'.
 *
 * Parameters:
 *
 * Returns:
 */
static void SetContextState(BtlBipintContext *bipContext,
                            BtlBipContextState    state)
{
    BTL_FUNC_START("SetContextState");

    bipContext->state = state;

    /* DEBUG: do some logging. */
    BTL_BIP_LOG_CONTEXT_STATE(state);

    BTL_FUNC_END();
}

/*-------------------------------------------------------------------------------
 * SetChannelState()
 *
 *    Fill the 'channelState' parameter in the 'bipContext' with the new 'state'.
 *
 * Parameters:
 *
 * Returns:
 */
static void SetChannelState(BtlBipintContext *bipContext,
                            BtlBipChannelState    state)
{
    BTL_FUNC_START("SetContextState");

    bipContext->channelState = state;

    /* DEBUG: do some logging. */
    BTL_BIP_LOG_CHANNEL_STATE(state);

    BTL_FUNC_END();
}

/* udir - Why isn't there a GetChannelState? */

#if OBEX_AUTHENTICATION == XA_ENABLED
BtStatus BTL_BIPINT_ObexAuthenticationResponse(BtlBipintContext *bipContext,
													const BtlUtf8	*userId,
													const BtlUtf8	*password)
{
 	BipAuthInfo	authInfo;
	BtStatus status = BT_STATUS_SUCCESS;
	static const U8 empty[1] = {0};

	BTL_FUNC_START_AND_LOCK("BTL_BIPINT_ObexAuthenticationResponse");

	BTL_VERIFY_ERR((0 != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));
	BTL_VERIFY_ERR((0 != password), BT_STATUS_INVALID_PARM, ("Null password"));
	BTL_VERIFY_ERR((bipContext->obexAuthFlags & OBEX_AUTH_RECEIVED_CHAL), BT_STATUS_FAILED, ("No OBEX auth challenge was received"));

	authInfo.password = password;
	authInfo.passwordLen = OS_StrLen((char*) password);

	if ( NULL != userId )
	{
		authInfo.userId = userId;
		authInfo.userIdLen = OS_StrLen((char*) userId);
	}
	else /* empty / NULL userId is legal*/
	{
		authInfo.userId = empty;
		authInfo.userIdLen = 0;
	}

	authInfo.realm = empty;
	authInfo.realmLen = 0;
	
	bipContext->obexAuthFlags &= (~OBEX_AUTH_RECEIVED_CHAL);

	status = BIP_ClientSetAuthInfo(&bipContext->sessionInitiator.primary, &authInfo);
	BTL_VERIFY_ERR((status == OB_STATUS_SUCCESS), status, ("BIP_ClientSetAuthInfo failed"));
	BTL_LOG_DEBUG(("BIP_ClientSetAuthInfo success"));

	bipContext->obexAuthFlags  |= OBEX_AUTH_INFO_SET;
	/* BIP_ClientSetAuthInfo takes care of checking that string lengths don't exeed allowed maximum */
	if ( bipContext->obexAuthFlags & OBEX_AUTH_DURING_CALLBACK )
	{
	}
	else
	/*if ( (BT_STATUS_SUCCESS == status) && !(bipContext->obexAuthFlags & OBEX_AUTH_DURING_CALLBACK) )*/
	{
		status = BIP_Connect(&bipContext->sessionInitiator, &bipContext->responderAddress, bipContext->requestedService);
		if ( BT_STATUS_PENDING == status )
		{
			bipContext->channelState = BTL_BIP_CHANNEL_STATE_CONNECTING;
			status = BT_STATUS_SUCCESS;
		}
	}
		/*BIP_ServerContinue(&bipContext->sessionInitiator.primary);*/ /*BipObexServer */
	
	BTL_FUNC_END_AND_UNLOCK();
	
	return 	status;
}
#endif


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
static void LogChannelState(BtlBipChannelState state)
{
    switch (state)
    {
        case BTL_BIP_CHANNEL_STATE_DISCONNECTED: BTL_LOG_INFO(("Channel state changed: DISCONNECTED"));break;
        case BTL_BIP_CHANNEL_STATE_CONNECTED: BTL_LOG_INFO(("Channel state changed: CONNECTED"));break;
        case BTL_BIP_CHANNEL_STATE_DISCONNECTING: BTL_LOG_INFO(("Channel state changed: DISCONNECTING"));break;
        case BTL_BIP_CHANNEL_STATE_CONNECTING: BTL_LOG_INFO(("Channel state changed: CONNECTING"));break;
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
static void LogContextState(BtlBipContextState state)
{
    switch (state)
    {
        case BTL_BIP_CONTEXT_STATE_IDLE: BTL_LOG_INFO(("Context state changed: IDLE"));break;
        case BTL_BIP_CONTEXT_STATE_IN_USE: BTL_LOG_INFO(("Context state changed: IN USE"));break;
        case BTL_BIP_CONTEXT_STATE_DISABLING: BTL_LOG_INFO(("Context state changed: DISABLING"));break;
    }
}
#endif /* (XA_DEBUG == XA_ENABLED) || (XA_DEBUG_PRINT == XA_ENABLED) */


void BtlBipintSendProgressNotificationToApp(BtlBipintContext *context, BipCallbackParms* parms, U32 currPos)
{
	BTL_FUNC_START("BtlBipintSendProgressNotificationToApp");

	btlBipData.progressIndInfo.name = NULL;
	btlBipData.progressIndInfo.currPos = currPos;
	btlBipData.progressIndInfo.maxPos = parms->data->totLen;
	
	btlBipData.selfGeneratedCallbackParms = *parms;

	btlBipData.selfGeneratedCallbackParms.event = BIPINITIATOR_PROGRESS;
	btlBipData.selfGeneratedCallbackParms.data->progressInfo = &btlBipData.progressIndInfo;

	BtlBipintSendEventToApp(context, &btlBipData.selfGeneratedCallbackParms);
		
	BTL_FUNC_END();
}

void BtlBipintSendEventToApp(BtlBipintContext *context, BipCallbackParms *parms)
{
	BTL_FUNC_START("BtlBipintSendEventToApp");

	btlBipData.eventToApp.context = context;
	btlBipData.eventToApp.parms = parms;

	(context->callback)(&btlBipData.eventToApp);
	
	BTL_FUNC_END();
}


static void BtlBipintInitRequestVars(BtlBipintContext *bipContext)
{
	bipContext->objLen = 0;
	bipContext->deleteFileIfAborted = FALSE;
	OS_StrCpyUtf8(bipContext->pulledFileName, (BtlUtf8*)"");
	bipContext->requestInProgress = FALSE;
}



#else /* BTL_CONFIG_BIP ==   BTL_CONFIG_ENABLED*/

/*-------------------------------------------------------------------------------
 * BTL_BIPINT_Init()
 */
BtStatus BTL_BIPINT_Init(void)
{
    
   BTL_LOG_INFO(("BTL_BIPINT_Init() -  BTL_CONFIG_BIP Disabled"));
  

    return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPINT_Deinit()
 */
BtStatus BTL_BIPINT_Deinit(void)
{
    BTL_LOG_INFO(("BTL_BIPINT_Deinit() -  BTL_CONFIG_BIP Disabled"));

    return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------*/



#endif /* BTL_CONFIG_BIP==   BTL_CONFIG_ENABLED*/







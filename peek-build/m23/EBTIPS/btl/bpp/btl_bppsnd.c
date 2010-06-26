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
*   FILE NAME:      btl_bppsnd.c
*
*   DESCRIPTION:    Implementation of the BTL Basic Printing Profile Sender role
*
*   AUTHOR:         Udi Ron
*
\*******************************************************************************/

#include "btl_config.h"
#include "btl_defs.h"
#include "btl_log.h"
#include "btl_bppsnd.h"

BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_BPPSND);

#if BTL_CONFIG_BPPSND == BTL_CONFIG_ENABLED

/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "bttypes.h"
#include "medev.h"
#include "obex.h"
#include "debug.h"
#include "utils.h"
#include "btl_commoni.h"
#include "btl_pool.h"
#include "btl_utils.h"
#include "btl_obex_utils.h"
#include "btl_debug.h"
#include "bthal_fs.h"
#include "string.h"
#include "btl_bppsnd_soapinterface.h"


/********************************************************************************
 *
 * Constants
 *
 *******************************************************************************/
#define BTL_BPPSND_MAX_FSPATH_LEN	BTHAL_FS_MAX_PATH_LENGTH

#define BSL_BPPSND_DEFAULT  ((U8)(BSL_NO_SECURITY))

#define MAX_REQ_MEM_OBJ  (2000)


/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BtlBppsndInitState type
 *
 *     Defines the BPPSND init state.
 */
typedef enum _BtlBppsndInitState
{
	BTL_BPPSND_INIT_STATE_NOT_INITIALIZED,
	BTL_BPPSND_INIT_STATE_INITIALIZED,
	BTL_BPPSND_INIT_STATE_INITIALIZATION_FAILED
} BtlBppsndInitState;

/*-------------------------------------------------------------------------------
 * BtlBppsndState type
 *
 *     Defines the BPPSND state of a specific context.
 */
typedef enum _BtlBppsndState
{
	 BTL_BPPSND_STATE_IDLE,
	 BTL_BPPSND_STATE_DISCONNECTED,
	 BTL_BPPSND_STATE_CONNECTED,
	 BTL_BPPSND_STATE_DISCONNECTING,
	 BTL_BPPSND_STATE_CONNECTING,
	 BTL_BPPSND_STATE_PRINTING,
	 BTL_BPPSND_STATE_SENT_REQUEST
} BtlBppsndState;

/*-------------------------------------------------------------------------------
 * BtlBppsndAbortState type
 *
 *     Defines the BPPSND Abort state of a specific context.
 */
typedef enum _BtlBppsndAbortState
{
	 BTL_BPPSND_ABORT_STATE_IDLE,
 	 BTL_BPPSND_ABORT_STATE_INTERNAL_ABORT_IN_PROGRESS,
	 BTL_BPPSND_ABORT_STATE_USER_ABORT_IN_PROGRESS
} BtlBppsndAbortState;


/*-------------------------------------------------------------------------------
 * BtlBppsndDisablingState type
 *
 *     Defines the BPPSND Disabling state of a specific context.
 */
typedef enum _BtlBppsndDisablingState
{
	 BTL_BPPSND_DISABLING_STATE_IDLE,
 	 BTL_BPPSND_DISABLING_STATE_DISCONNECTING,
 	 BTL_BPPSND_DISABLING_STATE_DEREGISTER,
	 BTL_BPPSND_DISABLING_STATE_DONE
} BtlBppsndDisablingState;

/*-------------------------------------------------------------------------------
 * BtlBppsndContextRadioOffState type
 *
 *     Defines the BPPSND Radio Off state of a specific context.
 */
typedef enum _BtlBppsndContextRadioOffState
{
	 BTL_BPPSND_CONTEXT_RADIO_OFF_STATE_IDLE,
 	 BTL_BPPSND_CONTEXT_RADIO_OFF_STATE_DISCONNECTING,
	 BTL_BPPSND_CONTEXT_RADIO_OFF_STATE_DONE
} BtlBppsndContextRadioOffState;

/*-------------------------------------------------------------------------------
 * BtlBppsndRadioOffState type
 *
 *     Defines the BPPSND Radio Off state of a specific context.
 */
typedef enum _BtlBppsndRadioOffState
{
	 BTL_BPPSND_RADIO_OFF_STATE_IDLE,
 	 BTL_BPPSND_RADIO_OFF_STATE_DISCONNECTING,
	 BTL_BPPSND_RADIO_OFF_STATE_DONE
} BtlBppsndRadioOffState;

typedef enum _BtlBppsndRadioOffEvent
{
	 BTL_BPPSND_RADIO_OFF_EVENT_START,
 	 BTL_BPPSND_RADIO_OFF_EVENT_CONTEXT_COMPLETED
} BtlBppsndRadioOffEvent;

/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * ObexAuthFlags type
 *
 *     Defines the OBEX authentication flags of a specific context.
 */
typedef U8 ObexAuthFlags;

#define OBEX_AUTH_RECEIVED_CHAL  					(0x02)
#define OBEX_AUTH_DURING_CALLBACK				(0x10)
#define OBEX_AUTH_INFO_SET						(0x40)


/*-------------------------------------------------------------------------------
 * BtlBppsndContext structure
 *
 *     Represents BTL BPP Sender context.
 */
struct _BtlBppsndContext 
{
	/* Must be first field */
	BtlContext 			base;

	BtlBppsndState		state;
	BtlBppsndCallBack		callback;
	ObexAbortReason		failureReason;
	BppSndSession   		session;
	BppData				sndData;
	
	BOOL				printingMemory; /* whether current object (main or referenced) is in memory or in file */
	BOOL				mainDocIsMemObj; /* whether object of main document (is in memory or in file) */

	BppPrintJob			printJob;
	BtlUtf8				fsPath[BTL_BPPSND_MAX_FSPATH_LEN + 1];
	ObexTpAddr 			printerAddress;
	BppObexServer   		*obexServer;

#if BT_SECURITY == XA_ENABLED
	BtSecurityLevel		secLevel;
#endif

#if OBEX_AUTHENTICATION == XA_ENABLED
	/* OBEX Authentication flags */
	ObexAuthFlags obexAuthFlags;
#endif	/* OBEX_AUTHENTICATION == XA_ENABLED */

	BtlObject objToPrint;	/* used for ConnectSimplePushPrintDisconnect */

	/* members for job based transfer: */
	BtlObject responseObj;
	U32		jobId;
	
	BtlBppsndState		complexApiState; /* state machine of complex operation Connect-Push-Disconnect*/

	BOOL						disconnectionPending;
	BtlBppsndAbortState			abortState;
	BtlBppsndDisablingState			disablingState;
	BOOL						asynchronousDisable;
	BtlBppsndContextRadioOffState	radioOffState;
	BOOL						asynchronousRadioOff;

#if BTL_CONFIG_BPPSND_DISCONNECT_TIMEOUT > 0
	BOOL						disconnectionTimerPending;
	EvmTimer					disconnectionTimer;
#endif /* BTL_CONFIG_BPPSND_DISCONNECT_TIMEOUT > 0 */

};

/*-------------------------------------------------------------------------------
 * BtlBppsndData structure
 *
 *     Represents the data of th BTL Bppsnd module.
 */
typedef struct _BtlBppsndData
{
	/* Pool of Bppsnd contexts */
	BTL_POOL_DECLARE_POOL(contextsPool, contextsMemory, BTL_CONFIG_BPPSND_MAX_NUM_OF_CONTEXTS, sizeof(BtlBppsndContext));

	/* List of active Bppsnd contexts */
	ListEntry 				contextsList;

	BtlObjProgressInd		progressIndInfo;
	
	BppCallbackParms		selfGeneratedCallbackParms;
	
	/* Event passed to the application */
	BtlBppsndEvent 		eventToApp;

	BtlBppsndRadioOffState	radioOffState;
	U32					numOfDisconnectingContexts;
	BOOL				asynchronousRadioOff;
} BtlBppsndData;

/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * btlBppsndInitState
 *
 *     Represents the current init state of Bppsnd module.
 */
static BtlBppsndInitState btlBppsndInitState = BTL_BPPSND_INIT_STATE_NOT_INITIALIZED;


/*-------------------------------------------------------------------------------
 * btlBppsndData
 *
 *     Represents the data of Bppsnd module.
 */
static BtlBppsndData btlBppsndData;


/*-------------------------------------------------------------------------------
 * btlBppsndContextsPoolName
 *
 *     Represents the name of the Bppsnd contexts pool.
 */
static const char btlBppsndContextsPoolName[] = "BppsndContexts";


static U8 soapReqMem[MAX_REQ_MEM_OBJ];

/********************************************************************************
 *
 * Internal function prototypes
 *
 *******************************************************************************/
static void BTL_BPPSND_InitOperationVariables(BtlBppsndContext* context);
static void BTL_BPPSND_Callback(BppCallbackParms* parms);
static BtlBppsndContext *BTL_BPPSND_GetContextFromBppCallbackParms(BppCallbackParms* parms);

static BtStatus BTL_BPPSND_HandleClientDataRequest(BtlBppsndContext *bppsndContext, BppCallbackParms* parms);
static BtStatus BTL_BPPSND_HandleConnected(BtlBppsndContext *bppsndContext, BppCallbackParms* parms);
static BtStatus BTL_BPPSND_HandleDisconnected(BtlBppsndContext *bppsndContext, BppCallbackParms* parms);
static BtStatus Btl_Bppsnd_HandleChallengeRcvd(BtlBppsndContext *bppsndContext, BppCallbackParms* parms);
static BtStatus Btl_Bppsnd_HandlePushOrSendCompleted(BtlBppsndContext *bppsndContext, BppCallbackParms* parms);
static BtStatus Btl_Bppsnd_HandleSoapRequestCompleted(BtlBppsndContext *bppsndContext, BppCallbackParms* parms);
static BtStatus	Btl_Bppsnd_HandleIncomingSoapResponse(BtlBppsndContext *bppsndContext, BppCallbackParms* parms);
static BtStatus BTL_BPPSND_HandleFailure(BtlBppsndContext *bppsndContext, BppCallbackParms* parms);
static BtStatus BTL_BPPSND_HandleOperationAborted(BtlBppsndContext *bppsndContext, BppCallbackParms* parms);

static BtStatus Btl_Bppsnd_Disconnect(BtlBppsndContext *bppsndContext);
	
static BtStatus Btl_Bppsnd_HandleStartGetRefObject(BtlBppsndContext *bppsndContext, BppCallbackParms* parms);
static BtStatus Btl_Bppsnd_HandleStartGetResolvedRefObject(BtlBppsndContext *bppsndContext, 
	BppCallbackParms* parms, const BtlObject *responseObj);

static BtStatus BTL_BPPSND_HandletGetRefObjectData(BppCallbackParms* parms);
static BtStatus BTL_BPPSND_HandletGetRefObjectDataCompleted(BtlBppsndContext *bppsndContext, BppCallbackParms* parms);
static BOOL BTL_BPPSND_GetReferencedObjectFullFileName(	const BtlUtf8 	*printedFileFullName,
																	const BtlUtf8	*referencedFileName, 
																	BtlUtf8			*referencedFileFullName);

static void BTL_BPPSND_GetFolderNameFromFullName(const BtlUtf8* fullName, BtlUtf8* folderName);

static BtStatus Btl_Bppsnd_SendObexAbort(BtlBppsndContext *bppsndContext, BOOL userAbort);
static void BTL_BPPSND_SendProgressNotificationToApp(BtlBppsndContext *bppsndContext, BppCallbackParms *parms);
static void BTL_BPPSND_SendEventToApp(BtlBppsndContext *bppsndContext, BppCallbackParms *parms);

static BOOL BTL_BPPSND_IsConnected(BtlBppsndContext *bppsndContext);
static BOOL BTL_BPPSND_IsConnecting(BtlBppsndContext *bppsndContext);

static BtStatus BTL_BPPSND_OpenDocument(const BtlObject *objToPrint, BtlBppsndContext *bppsndContext, BOOL read);
static BtStatus BTL_BPPSND_CloseDocument(BppData *data, BtlBppsndContext *bppsndContext);
static BtStatus BTL_BPPSND_GetDocumentSize(const BtlObject *objToPrint, BtlBppsndContext *bppsndContext);
static BtStatus BTL_BPPSND_ReadDocument(BppData *data, BtlBppsndContext *bppsndContext);
static BtStatus Btl_Bppsnd_WriteDocument(BppData *data, BtlBppsndContext *bppsndContext);
static BtStatus BTL_BPPSND_SeekDocument(BppData *data, BtlBppsndContext *bppsndContext);
static BtStatus BTL_BPPSND_OpenFile(const BtlUtf8* name, BppData *data, BOOL read);
static BtStatus BTL_BPPSND_CloseFile(BppData *data);
static BtStatus BTL_BPPSND_GetFileSize(const BtlUtf8 *fileFullName, BppData* data);
static BtStatus BTL_BPPSND_ReadFile(BppData *data);
static BtStatus Btl_Bppsnd_WriteFile(BppData *data);
static BtStatus BTL_BPPSND_SeekFile(BppData *data);

/*static const char* BTL_BPPSND_pInitState(BtlBppsndInitState state);*/
static const char* BTL_BPPSND_pState(BtlBppsndState state);
static BtStatus BTL_BPPSND_ReadMem(BppData *data);
static BtStatus Btl_Bppsnd_WriteMem(BppData *data);
static BOOL Btl_Bppsnd_IsAbsoluteName( const BtlUtf8 *fileName );
static BtStatus Btl_Bppsnd_GetReferencedObjectFromApp(BtlBppsndContext *bppsndContext, BppCallbackParms* parms);
static BtStatus Btl_Bppsnd_PushOrSendDocument(
									BtlBppsndContext 	*bppsndContext,
									const BtlObject 	*objToPrint,
									BppOp			bppOpp,
									BtlBppsndJobId	jobId,
									char			*callerFuncName);

static BtStatus Btl_bppsd_CheckObjectValidity(const BtlObject 	*btlObj);
static void Btl_Bppsnd_HandleMainStateMachine( BtlBppsndContext *bppsndContext, BppCallbackParms* parms, BppEvent bppEvent);
static void Btl_Bppsnd_HandleComplexStateMachine( BtlBppsndContext *bppsndContext, BppEvent bppEvent );
static BtStatus BtlBppsndBtlNotificationsCb(BtlModuleNotificationType notificationType);

static BtStatus Btl_Bppsnd_DisablingProcessor(BtlBppsndContext *bppsndContext, BppCallbackParms* parms);
static BtStatus Btl_Bppsnd_ContextRadioOffProcessor(BtlBppsndContext *bppsndContext);
static BtStatus Btl_Bppsnd_RadioOffProcessor(BtlBppsndRadioOffEvent event);

#if BTL_CONFIG_BPPSND_DISCONNECT_TIMEOUT > 0
static void BtlBppsndDisconnectionTimeout(EvmTimer *timer);
#endif /* BTL_CONFIG_BPPSND_DISCONNECT_TIMEOUT > 0 */


/********************************************************************************
 *
 * Function declarations
 *
 *******************************************************************************/
 
/*-------------------------------------------------------------------------------
 * BTL_BPPSND_Init()
 */
BtStatus BTL_BPPSND_Init(void)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_COMMON("BTL_BPPSND_Init");
	
	BTL_VERIFY_ERR((BTL_BPPSND_INIT_STATE_INITIALIZATION_FAILED != btlBppsndInitState), BT_STATUS_IMPROPER_STATE,
					("BPPSND initialization failed before, please de-init before retrying "));
	BTL_VERIFY_ERR((BTL_BPPSND_INIT_STATE_NOT_INITIALIZED == btlBppsndInitState), BT_STATUS_IMPROPER_STATE,
					("BPPSND module is already initialized"));
	
	btlBppsndInitState = BTL_BPPSND_INIT_STATE_INITIALIZATION_FAILED;

	status = BTL_POOL_Create(&btlBppsndData.contextsPool,
							btlBppsndContextsPoolName,
							btlBppsndData.contextsMemory, 
							BTL_CONFIG_BPPSND_MAX_NUM_OF_CONTEXTS,
							sizeof(BtlBppsndContext));
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("BPPSND contexts pool creation failed"));
	
	InitializeListHead(&btlBppsndData.contextsList);
	btlBppsndData.radioOffState = BTL_BPPSND_RADIO_OFF_STATE_IDLE;
		
	BTL_RegisterForCommonNotifications(BTL_MODULE_TYPE_BPPSND, BtlBppsndBtlNotificationsCb);

	btlBppsndInitState = BTL_BPPSND_INIT_STATE_INITIALIZED;

	BTL_FUNC_END_AND_UNLOCK();
	
	return status;
}


/*-------------------------------------------------------------------------------
 * BTL_BPPSND_Deinit()
 */
BtStatus BTL_BPPSND_Deinit(void)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_COMMON("BTL_BPPSND_Deinit");

	BTL_DeRegisterFromCommonNotifications(BTL_MODULE_TYPE_BPPSND);

	BPP_Deinit();
	
	BTL_VERIFY_ERR((BTL_BPPSND_INIT_STATE_NOT_INITIALIZED != btlBppsndInitState), BT_STATUS_IMPROPER_STATE,
					("BPPSND module is not initialized"));

	BTL_VERIFY_FATAL_NORET((IsListEmpty(&btlBppsndData.contextsList)), ("BPPSND contexts are still active"));

	status = BTL_POOL_Destroy(&btlBppsndData.contextsPool);
	BTL_VERIFY_FATAL_NORET((BT_STATUS_SUCCESS == status), ("BPPSND contexts pool destruction failed"));
		
	btlBppsndInitState = BTL_BPPSND_INIT_STATE_NOT_INITIALIZED;

	BTL_FUNC_END_AND_UNLOCK();
	
	return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BPPSND_Create()
 */
BtStatus BTL_BPPSND_Create(	BtlAppHandle 				*appHandle,
									const BtlBppsndCallBack 	bppsndCallback,
									const BtSecurityLevel 		*securityLevel,
									BtlBppsndContext 			**bppsndContext)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BPPSND_Create");

	BTL_VERIFY_ERR((0 != bppsndCallback), BT_STATUS_INVALID_PARM, ("Null bppsndCallback"));
	BTL_VERIFY_ERR((0 != bppsndContext), BT_STATUS_INVALID_PARM, ("Null bppsndContext"));

#if BT_SECURITY == XA_ENABLED
	if (0 != securityLevel)
	{
		status = BTL_VerifySecurityLevel(*securityLevel);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Invalid BPPSND securityLevel"));
	}
#endif	/* BT_SECURITY == XA_ENABLED */

	/* Allocate memory for a unique new BPPSND context */
	status = BTL_POOL_Allocate(&btlBppsndData.contextsPool, (void **)bppsndContext);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed allocating BPPSND context"));

	status = BTL_HandleModuleInstanceCreation(appHandle, BTL_MODULE_TYPE_BPPSND, &(*bppsndContext)->base);
	
	if (BT_STATUS_SUCCESS != status)
	{
		/* bppsndContext must be freed before we exit */
		BtStatus freeResult = BTL_POOL_Free(&btlBppsndData.contextsPool, (void **)bppsndContext);
		BTL_VERIFY_FATAL_NORET((BT_STATUS_SUCCESS == freeResult), ("Failed freeing BPPSND context (%s)", pBT_Status(freeResult)));

		BTL_ERR(status, ("Failed handling BPPSND instance creation (%s)", pBT_Status(status)));
	}

	/* Save the given callback */
	(*bppsndContext)->callback = bppsndCallback;

	(*bppsndContext)->fsPath[0] = (*bppsndContext)->fsPath[BTL_BPPSND_MAX_FSPATH_LEN] = '\0';
	(*bppsndContext)->printingMemory = (*bppsndContext)->mainDocIsMemObj = FALSE;
	(*bppsndContext)->jobId = 0;
	(*bppsndContext)->disconnectionPending = FALSE;
	(*bppsndContext)->abortState = BTL_BPPSND_ABORT_STATE_IDLE;
	(*bppsndContext)->disablingState = BTL_BPPSND_DISABLING_STATE_IDLE;
	(*bppsndContext)->radioOffState = BTL_BPPSND_CONTEXT_RADIO_OFF_STATE_IDLE;
	
	/* Add the new BPPSND context to the active contexts list */
	InsertTailList(&btlBppsndData.contextsList, &((*bppsndContext)->base.node));

#if BT_SECURITY == XA_ENABLED
	if (securityLevel == 0)
	{
		(*bppsndContext)->secLevel = BSL_BPPSND_DEFAULT;
	}
	else
	{
		(*bppsndContext)->secLevel = *securityLevel;
	}
#endif	/* BT_SECURITY == XA_ENABLED */

	/* Init BPPSND context state */
	(*bppsndContext)->state = BTL_BPPSND_STATE_IDLE;
	(*bppsndContext)->complexApiState = BTL_BPPSND_STATE_IDLE;

#if OBEX_AUTHENTICATION == XA_ENABLED
	(*bppsndContext)->obexAuthFlags = 0;
#endif

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BPPSND_Destroy()
 */
BtStatus BTL_BPPSND_Destroy(BtlBppsndContext **bppsndContext)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BOOL isAllocated = FALSE;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BPPSND_Destroy");

	BTL_VERIFY_ERR((0 != bppsndContext), BT_STATUS_INVALID_PARM, ("Null bppsndContext"));
	BTL_VERIFY_ERR((0 != *bppsndContext), BT_STATUS_INVALID_PARM, ("Null *bppsndContext"));
	BTL_VERIFY_ERR((BTL_BPPSND_STATE_IDLE == (*bppsndContext)->state), BT_STATUS_IN_USE, ("BPPSND context is in use"));

	status = BTL_POOL_IsElelementAllocated(&btlBppsndData.contextsPool, *bppsndContext, &isAllocated);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed locating given BPPSND context"));
	BTL_VERIFY_ERR((TRUE == isAllocated), BT_STATUS_INVALID_PARM, ("Invalid BPPSND context"));

	/* Remove the context from the list of all BPPSND contexts */
	RemoveEntryList(&((*bppsndContext)->base.node));

	status = BTL_HandleModuleInstanceDestruction(&(*bppsndContext)->base);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("Failed handling BPPSND instance destruction (%s)", pBT_Status(status)));

	status = BTL_POOL_Free(&btlBppsndData.contextsPool, (void **)bppsndContext);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("Failed freeing BPPSND context (%s)", pBT_Status(status)));

	/* Set the BPPSND context to NULL */
	*bppsndContext = 0;

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

/*-------------------------------------------------------------------------------
 * BTL_BPPSND_Enable()
 */
BtStatus BTL_BPPSND_Enable(BtlBppsndContext *bppsndContext)
{
   	BtStatus status = BT_STATUS_SUCCESS;
	ObStatus obStatus;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BPPSND_Enable");

	BTL_VERIFY_ERR((0 != bppsndContext), BT_STATUS_INVALID_PARM, ("Null bppsndContext"));
	BTL_VERIFY_ERR((BTL_BPPSND_STATE_IDLE == bppsndContext->state), BT_STATUS_IMPROPER_STATE, 
					("context is already enabled"));

#if BT_SECURITY == XA_ENABLED
	/* Save the given security level, or use default */
	bppsndContext->session.sndJob.cApp.secRecord.level = bppsndContext->secLevel;
#endif	/* BT_SECURITY == XA_ENABLED */

	
	/* Register as a BPP-Sender, and register sender callback (bppsndContext->allback) */
	status = BPP_RegisterSender(&bppsndContext->session, BTL_BPPSND_Callback);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, 
					("Failed to register BPP Sender (%s)", pBT_Status(status)));

	/* Initialize BPP Sender Job channel (the only channel used for Simple Push) */
	status = BPP_InitSenderChannel(&(bppsndContext->session), BPPCH_SND_JOB);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed to Initialize BPP Sender Job channel (%s)", pBT_Status(status)));

#if BPP_PRINTING_STATUS == XA_ENABLED
    /* Initialize BPP Sender Status channel */
	status = BPP_InitSenderChannel(&(bppsndContext->session), BPPCH_SND_STATUS);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed to Initialize BPP Sender Status channel (%s)", pBT_Status(status)));
#endif	/* BPP_PRINTING_STATUS == XA_ENABLE */

	/* ^^Uzi add more channels for job based*/
		
#if BT_SECURITY == XA_ENABLED

	/* Register security record */
	status = GOEP_RegisterClientSecurityRecord(
		&bppsndContext->session.sndJob.cApp, bppsndContext->session.sndJob.cApp.secRecord.level);
	if (BT_STATUS_SUCCESS != status)
	{
		obStatus = BPP_DeregisterSender(&bppsndContext->session);
		BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == obStatus), ("Failed deregistering BPPSND client"));

		/* This verify will fail and will handle the exception gracefully */
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed registering BPPSND security record"));
	}
	
#endif	/* BT_SECURITY == XA_ENABLED */

#if BTL_CONFIG_BPPSND_DISCONNECT_TIMEOUT > 0

	bppsndContext->disconnectionTimerPending = FALSE;

	/* Initialize disconnection timer */
	bppsndContext->disconnectionTimer.context = (void*)bppsndContext;
    bppsndContext->disconnectionTimer.func = BtlBppsndDisconnectionTimeout;
	
#endif /* BTL_CONFIG_BPPSND_DISCONNECT_TIMEOUT > 0 */

	BTL_LOG_INFO(("BTL_BPPSND: Sender is enabled."));

	bppsndContext->state = BTL_BPPSND_STATE_DISCONNECTED;
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;

}


/*-------------------------------------------------------------------------------
 * BTL_BPPSND_Disable()
 */
BtStatus Btl_Bppsnd_DisablingProcessor(BtlBppsndContext *bppsndContext, BppCallbackParms* parms)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BOOL	keepProcessing = TRUE;
	
	BTL_FUNC_START("Btl_Bppsnd_DisablingProcessor");

	while (keepProcessing == TRUE)
	{
		keepProcessing = FALSE;

		switch (bppsndContext->disablingState)
		{
			case BTL_BPPSND_DISABLING_STATE_IDLE:

				bppsndContext->asynchronousDisable = FALSE;
				bppsndContext->disablingState = BTL_BPPSND_DISABLING_STATE_DISCONNECTING;
				
				status = Btl_Bppsnd_Disconnect(bppsndContext);
				BTL_VERIFY_FATAL((status == BT_STATUS_SUCCESS) || (BT_STATUS_PENDING == status), status, (""));
				
				if (status == BT_STATUS_SUCCESS)
				{
					keepProcessing = TRUE;
					bppsndContext->disablingState = BTL_BPPSND_DISABLING_STATE_DEREGISTER;
				}
				else
				{
					bppsndContext->asynchronousDisable = TRUE;
				}

			break;
			
 	 		case BTL_BPPSND_DISABLING_STATE_DISCONNECTING:

				if (bppsndContext->state == BTL_BPPSND_STATE_DISCONNECTED)
				{				
					keepProcessing = TRUE;
					bppsndContext->disablingState = BTL_BPPSND_DISABLING_STATE_DEREGISTER;
				}

			break;
			
			case BTL_BPPSND_DISABLING_STATE_DEREGISTER:

#if BT_SECURITY == XA_ENABLED
				/* First, try to unregister security record */
				status = GOEP_UnregisterClientSecurityRecord(&bppsndContext->session.sndJob.cApp);
				BTL_VERIFY_FATAL_NORET((BT_STATUS_SUCCESS == status), ("Failed unregistering BPPSND security record"));
#endif	/* BT_SECURITY == XA_ENABLED */

				status = BPP_DeregisterSender(&bppsndContext->session);
				BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), status, ("Failed De-Registering Sender (%s)", pBT_Status(status)));
	
#if OBEX_AUTHENTICATION == XA_ENABLED
				bppsndContext->obexAuthFlags = 0;
#endif	

				keepProcessing = TRUE;
				bppsndContext->disablingState = BTL_BPPSND_DISABLING_STATE_DONE;
				
			break;

			case BTL_BPPSND_DISABLING_STATE_DONE:

				bppsndContext->state = BTL_BPPSND_STATE_IDLE;
				
				if (bppsndContext->asynchronousDisable == TRUE)
				{
					btlBppsndData.selfGeneratedCallbackParms = *parms;
					btlBppsndData.selfGeneratedCallbackParms.event = BPPCLIENT_DISABLED;
					btlBppsndData.selfGeneratedCallbackParms.status = OB_STATUS_SUCCESS;

					BTL_BPPSND_SendEventToApp(bppsndContext, &btlBppsndData.selfGeneratedCallbackParms);
				}

				bppsndContext->disablingState = BTL_BPPSND_DISABLING_STATE_IDLE;
				
			break;

			default:

				BTL_FATAL(BT_STATUS_INTERNAL_ERROR, ("Unexpected Disabling State (%d)", bppsndContext->disablingState));
				
		};
	}
	
	BTL_FUNC_END();

	return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BPPSND_Disable()
 */
BtStatus BTL_BPPSND_Disable(BtlBppsndContext *bppsndContext)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BPPSND_Disable");

	BTL_VERIFY_ERR((0 != bppsndContext), BT_STATUS_INVALID_PARM, ("Null bppsndContext"));
	BTL_VERIFY_ERR((BTL_BPPSND_DISABLING_STATE_IDLE == bppsndContext->disablingState), BT_STATUS_IN_PROGRESS, 
					("Disabling already in progress"));
	BTL_VERIFY_ERR((BTL_BPPSND_ABORT_STATE_IDLE == bppsndContext->abortState), BT_STATUS_IN_PROGRESS, 
					("Operation Abort already in progress"));
	BTL_VERIFY_ERR((BTL_BPPSND_RADIO_OFF_STATE_IDLE == btlBppsndData.radioOffState), BT_STATUS_IMPROPER_STATE, 
					("Radio Off already in progress"));

	status = 	Btl_Bppsnd_DisablingProcessor(bppsndContext, 0);
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}


/*-------------------------------------------------------------------------------
 * BTL_BPPSND_Connect()
 */
BtStatus BTL_BPPSND_Connect(	BtlBppsndContext 	*bppsndContext, 
									const BD_ADDR 	*bdAddr)
{
	BtStatus 			status = BT_STATUS_SUCCESS;
	ObStatus			obStatus = OB_STATUS_SUCCESS;
	BtDeviceContext * bdc;
	
	BtDeviceContext 	tmplt;

	BTL_FUNC_START_AND_LOCK("BTL_BPPSND_Connect");

	BTL_VERIFY_ERR((0 != bppsndContext), BT_STATUS_INVALID_PARM, ("Null bppsndContext"));
	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));
	BTL_VERIFY_ERR((BTL_BPPSND_STATE_DISCONNECTED == bppsndContext->state), BT_STATUS_IMPROPER_STATE,
					("context must be enabled and disconnected"));
	
	/* Init operation variables */
	BTL_BPPSND_InitOperationVariables(bppsndContext);
	
	bppsndContext->printerAddress.type = OBEX_TP_BLUETOOTH;
	bppsndContext->printerAddress.proto.bt.addr = *bdAddr;

	/* Find a device in DS */
	bdc = DS_FindDevice(bdAddr);

	/* [@ToDO][Udi] Check why this is necessary */
	if ( 0 == bdc ) 
	{
		OS_MemSet((U8 *)&tmplt, 0, (U32)sizeof(BtDeviceContext));
		OS_MemCopy((U8 *)&tmplt.addr, (U8 *)bdAddr, sizeof(BD_ADDR));
		DS_AddDevice(&tmplt, 0);	
       	OS_MemSet((U8*)&bppsndContext->printerAddress.proto.bt.psi, 0, sizeof(bppsndContext->printerAddress.proto.bt.psi));
	}
	else
	{
		bppsndContext->printerAddress.proto.bt.psi = bdc->psi;
	}

	bppsndContext->obexAuthFlags  &= ~OBEX_AUTH_INFO_SET;

	obStatus = BPP_Connect(&bppsndContext->session, &bppsndContext->printerAddress, BPPSVC_DPS);

	if (OB_STATUS_PENDING == obStatus)
	{
		BTL_LOG_INFO(("Waiting for connection to establish Job channel"));
		
		/* BPPSND state is now in the process of connecting */
		bppsndContext->state = BTL_BPPSND_STATE_CONNECTING;

		status = BT_STATUS_PENDING;
	}
	else if (OB_STATUS_SUCCESS == status)
	{
		BTL_LOG_INFO(("Connected To Printer Job channel (%s)", BTL_UTILS_LockedBdaddrNtoa(bdAddr)));
		
		bppsndContext->state = BTL_BPPSND_STATE_CONNECTED;

		status = BT_STATUS_SUCCESS;

#if BPP_PRINTING_STATUS == XA_ENABLED

    	obStatus = BPP_Connect(&bppsndContext->session, &bppsndContext->printerAddress, BPPSVC_STS);

		if (OB_STATUS_PENDING == obStatus)
		{
			BTL_LOG_INFO(("Waiting for connection to establish Status channel"));

			/* BPPSND state is now in the process of connecting */
			bppsndContext->state = BTL_BPPSND_STATE_CONNECTING;

			status = BT_STATUS_PENDING;
		}
		else if (OB_STATUS_SUCCESS == status)
		{
			BTL_LOG_INFO(("Connected To Printer Status channel (%s)", BTL_UTILS_LockedBdaddrNtoa(bdAddr)));		
		}
		else
		{
			BTL_ERR(BTL_UTILS_ObStatus2BtStatus(obStatus), 
						("Failed connecting to Printer Status channel (%s), Status: %s", BTL_UTILS_LockedBdaddrNtoa(bdAddr),  BTL_DEBUG_pObStatus(obStatus)));
		}

#endif	/* BPP_PRINTING_STATUS == XA_ENABLE */

		}
		else
		{
			BTL_ERR(BTL_UTILS_ObStatus2BtStatus(obStatus), 
					("Failed connecting to Printer Job channel (%s), Status: %s", BTL_UTILS_LockedBdaddrNtoa(bdAddr),  BTL_DEBUG_pObStatus(obStatus)));

	}

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BPPSND_Disconnect()
 */
BtStatus BTL_BPPSND_Disconnect(BtlBppsndContext *bppsndContext)
{
	BtStatus 	status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK("BTL_BPPSND_Disconnect");

	BTL_VERIFY_ERR((0 != bppsndContext), BT_STATUS_INVALID_PARM, ("Null bppsndContext"));
	BTL_VERIFY_ERR(((TRUE == BTL_BPPSND_IsConnected(bppsndContext)) ||
                    (TRUE == BTL_BPPSND_IsConnecting(bppsndContext))),
                    BT_STATUS_IMPROPER_STATE, 
				   ("Context must be connected or connecting"));

	status = Btl_Bppsnd_Disconnect(bppsndContext);

	BTL_BPPSND_InitOperationVariables(bppsndContext);
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

/*-------------------------------------------------------------------------------
 * BTL_OPPS_GetConnectedDevice()
 */
BtStatus BTL_BPPSND_GetConnectedDevice(BtlBppsndContext *bppsndContext,  BD_ADDR *bdAddr)
{
#if OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED

	BtStatus status = BT_STATUS_SUCCESS;
	BOOL retVal;
	ObexTpConnInfo tpInfo;
	
	BTL_FUNC_START_AND_LOCK("BTL_BPPSND_GetConnectedDevice");

	BTL_VERIFY_ERR((0 != bppsndContext), BT_STATUS_INVALID_PARM, ("Null bppsndContext"));
	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));
	BTL_VERIFY_ERR(TRUE == BTL_BPPSND_IsConnected(bppsndContext), BT_STATUS_IMPROPER_STATE, 
					("context is not connected"));

	tpInfo.size = sizeof(ObexTpConnInfo);
	tpInfo.tpType = OBEX_TP_BLUETOOTH;
	tpInfo.remDev = 0;

	retVal = GOEP_ClientGetTpConnInfo(&(bppsndContext->session.sndJob.cApp), &tpInfo);
	BTL_VERIFY_ERR((TRUE == retVal), BT_STATUS_NO_CONNECTION, ("Failed getting connection info failed"));

	BTL_VERIFY_ERR((0 != tpInfo.remDev), BT_STATUS_INTERNAL_ERROR, ("Unable to find connected remote device"));
	
	*bdAddr = tpInfo.remDev->bdAddr;
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;
	
#else

	return BT_STATUS_NOT_SUPPORTED;

#endif
}

/*-------------------------------------------------------------------------------
 * BTL_BPPSND_SimplePushPrint()
 */
BtStatus BTL_BPPSND_SimplePushPrint(	BtlBppsndContext 	*bppsndContext, 
											const BtlObject 	*objToPrint)
{
	BtStatus		status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK("BTL_BPPSND_SimplePushPrint");

	status = Btl_Bppsnd_PushOrSendDocument(bppsndContext, objToPrint, BPPOP_SIMPLEPUSH, 0, "SimplePushPrint");

	BTL_FUNC_END_AND_UNLOCK();
	
	return status;
}


BtStatus BTL_BPPSND_Abort(BtlBppsndContext *bppsndContext)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK("BTL_BPPSND_Abort");

	BTL_VERIFY_ERR((BTL_BPPSND_ABORT_STATE_IDLE == bppsndContext->abortState), BT_STATUS_IN_PROGRESS, 
					("User Abort already in progress"));
	BTL_VERIFY_ERR((BTL_BPPSND_DISABLING_STATE_IDLE == bppsndContext->disablingState), BT_STATUS_IMPROPER_STATE, 
					("Radio Off already in progress"));
	BTL_VERIFY_ERR((BTL_BPPSND_RADIO_OFF_STATE_IDLE == btlBppsndData.radioOffState), BT_STATUS_IMPROPER_STATE, 
					("Radio Off already in progress"));

	/* Printing in progress, just abort as requested */
	Btl_Bppsnd_SendObexAbort(bppsndContext, TRUE);
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_BPPSND_GetReferenceObjectResponse(	BtlBppsndContext 	*bppsndContext,
															const BtlObject	*responseObj)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BppCallbackParms parms = { 0 };
	BppData               bppData = { 0 };

	BTL_FUNC_START_AND_LOCK("BTL_BPPSND_GetReferenceObjectResponse");
	
	BTL_VERIFY_ERR((0 != bppsndContext), BT_STATUS_INVALID_PARM, ("Null bppsndContext"));
	BTL_VERIFY_ERR((0 != responseObj), BT_STATUS_INVALID_PARM, ("Null responseObj"));
	BTL_VERIFY_ERR(0 != responseObj->objectMimeType, BT_STATUS_INVALID_PARM, ("Null objectMimeType"));
	BTL_VERIFY_ERR(OS_StrLen(responseObj->objectMimeType) <= BTL_BPPSND_MAX_MIME_TYPE_LEN,
					BT_STATUS_INVALID_PARM, 
					("MIME type too long (%d), %d is the limit", 
					OS_StrLen(responseObj->objectMimeType), BTL_BPPSND_MAX_MIME_TYPE_LEN));
	parms.data = &bppData;
	switch ( responseObj->objectLocation )
	{
		case BTL_OBJECT_LOCATION_FS:
			/*if object location is on fs then its path must not be null*/
			BTL_VERIFY_ERR((0 != responseObj->location.fsLocation.fsPath), BT_STATUS_INVALID_PARM,
							("Null file system path to object location"));
			/*if object location is on fs then its path len must not be too long*/
			BTL_VERIFY_ERR((OS_StrLen((const char*)responseObj->location.fsLocation.fsPath) <= BTL_BPPSND_MAX_FSPATH_LEN),
							BT_STATUS_INVALID_PARM, 
							(("File System Path too long (%d), %d is the limit"), 
							OS_StrLen((const char*)responseObj->location.fsLocation.fsPath), BTL_BPPSND_MAX_FSPATH_LEN));
			/*if object location is on fs then it must have an abolute path name*/
			BTL_VERIFY_ERR( (Btl_Bppsnd_IsAbsoluteName(responseObj->location.fsLocation.fsPath)),
							BT_STATUS_INVALID_PARM, 
							(("Referenced file must have an absolute path name (name = %s)"), 
							responseObj->location.fsLocation.fsPath));
			parms.data->buff =  (U8 *) responseObj->location.fsLocation.fsPath;
			break;

		case  BTL_OBJECT_LOCATION_MEM:
			/*if object location is on mem then its address must not be null.*/
			/*should be changed if target architecture might allow user data to reside on address 0x0000*/
			BTL_VERIFY_ERR((0 != responseObj->location.memLocation.memAddr)
							, BT_STATUS_INVALID_PARM, 
							("Null memory address object given"));
			BTL_VERIFY_ERR((0 < responseObj->location.memLocation.size),
							BT_STATUS_INVALID_PARM, 
							("Size of given memory object should be larger than zero (%d given)", 
							responseObj->location.memLocation.size));
			break;

		default:
			BTL_ERR(BT_STATUS_INVALID_PARM, ("Object location not supported"));
				
	}

	parms.data->offset = bppsndContext->sndData.offset;
	parms.data->len = bppsndContext->sndData.len;
	parms.obex.server = bppsndContext->obexServer;

	status = Btl_Bppsnd_HandleStartGetResolvedRefObject(bppsndContext, &parms, responseObj);

	BTL_FUNC_END_AND_UNLOCK();
	
	return status;

}

void BTL_BPPSND_InitOperationVariables(BtlBppsndContext* context)
{	
	context->failureReason = OB_STATUS_SUCCESS;
}

BtStatus Btl_Bppsnd_PushOrSendDocument(
									BtlBppsndContext 	*bppsndContext,
									const BtlObject 	*objToPrint,
									BppOp			bppOpp,
									BtlBppsndJobId	jobId,
									char				*callerFuncName)
{
	BtStatus		status = BT_STATUS_SUCCESS;
	ObStatus		obStatus = OB_STATUS_INVALID_PARM;
	BOOL		simplePush = bppOpp != BPPOP_SENDDOCUMENT;
	
	BTL_FUNC_START(callerFuncName);

	/* ^^ add check for valid jobId*/
	BTL_VERIFY_ERR((0 != bppsndContext), BT_STATUS_INVALID_PARM, ("Null bppsndContext"));
	BTL_VERIFY_ERR((BTL_BPPSND_STATE_CONNECTED == bppsndContext->state), BT_STATUS_IMPROPER_STATE, 
					("Must be connected but not already printing"));
	BTL_VERIFY_ERR(callerFuncName != 0, BT_STATUS_INVALID_PARM, ("Null callerFuncName"));

	if ( simplePush )
	{
		BTL_VERIFY_ERR((0 == bppsndContext->jobId),  BT_STATUS_BUSY, ("cannot push during active job"));
	}
	else
	{
		BTL_VERIFY_ERR((bppsndContext->jobId == jobId),  BT_STATUS_INVALID_PARM, 
			("invalid jobId %d (our current is %d)", jobId, bppsndContext->jobId ));
	}

	status = Btl_bppsd_CheckObjectValidity(objToPrint);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), BT_STATUS_INVALID_PARM, ("invalid  object to print"));

	BTL_BPPSND_InitOperationVariables(bppsndContext);
	
	/* record file name & type to be used throught the process */

	OS_StrCpy((char*)bppsndContext->printJob.name, (const char*)objToPrint->objectName);	
	OS_StrCpy((char*)bppsndContext->printJob.docFormat, (const char*)objToPrint->objectMimeType);
	
	if(BTL_OBJECT_LOCATION_FS == objToPrint->objectLocation) 
	{
		OS_StrCpy((char*)bppsndContext->fsPath, (const char*)objToPrint->location.fsLocation.fsPath);
		bppsndContext->printingMemory = FALSE;
		bppsndContext->mainDocIsMemObj = FALSE;
	}
	/* must be memory-based location, since we verified above that the location is one of the two */
	else
	{
		OS_StrCpy((char*)bppsndContext->fsPath, (const char*)objToPrint->objectName);
		/*bppsndContext->fsPath[0] = '\0';*/
		bppsndContext->printingMemory = TRUE;
		bppsndContext->mainDocIsMemObj = TRUE;
	}
	
	BTL_LOG_INFO(("Printing: |%s, Type: |%s|",
		bppsndContext->printingMemory ? objToPrint->objectName : bppsndContext->fsPath, objToPrint->objectMimeType));

	status = BTL_BPPSND_OpenDocument(objToPrint, bppsndContext, TRUE);	

	BTL_VERIFY_ERR(BT_STATUS_SUCCESS == status, status, ("Failed opening document (%s)", pBT_Status(status)));

	status = BTL_BPPSND_GetDocumentSize(objToPrint, bppsndContext);
	
	if (BT_STATUS_SUCCESS != status)
	{
		BTL_BPPSND_CloseDocument(&bppsndContext->sndData, bppsndContext);
		
		BTL_ERR(status, ("Failed Getting File Size (%s)", pBT_Status(status)));
	}

	BTL_LOG_DEBUG(("Object Size: %d", bppsndContext->sndData.totLen));

	if ( simplePush )
	{
		obStatus = BPP_SimplePush(&bppsndContext->session, &bppsndContext->sndData, &bppsndContext->printJob);
	}
	else
	{
		bppsndContext->sndData.jobId = jobId;
		obStatus = BPP_SendDocument(&bppsndContext->session, &bppsndContext->sndData, &bppsndContext->printJob);
	}
		
	if (OB_STATUS_PENDING== obStatus)
	{
		bppsndContext->state = BTL_BPPSND_STATE_PRINTING;

		status = BT_STATUS_PENDING;
	}
	else if (OB_STATUS_SUCCESS== obStatus)
	{
		BTL_BPPSND_CloseDocument(&bppsndContext->sndData,bppsndContext);
		
		bppsndContext->state = BTL_BPPSND_STATE_CONNECTED;

		status = BT_STATUS_SUCCESS;		
	}
	else
	{
		BTL_BPPSND_CloseDocument(&bppsndContext->sndData,bppsndContext);
		
		BTL_ERR(BTL_UTILS_ObStatus2BtStatus(obStatus), 
			("ERROR: %s (%s)", simplePush ? "BPP_SimplePush" : "BPP_SendDocument", BTL_DEBUG_pObStatus(obStatus)));
	}
	
	BTL_FUNC_END();
	
	return status;

}

/*---------------------------------------------------------------------------
 *            bppsndContext->allback
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Callback function for BPP Sender Events
 *
 * Return:    void
 *
 */
void BTL_BPPSND_Callback(BppCallbackParms* parms)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BtlBppsndContext *bppsndContext = BTL_BPPSND_GetContextFromBppCallbackParms(parms);
	
	BTL_FUNC_START("BTL_BPPSND_Callback");

	BTL_LOG_INFO(("Sender State: %s, channel: %d", BTL_BPPSND_pState(bppsndContext->state), parms->channel));
	BTL_LOG_DEBUG(("Event: %s, Op: %s, status: %s", 
					BTL_DEBUG_pBppEvent(parms->event), BTL_DEBUG_pBppOp(parms->data->bppOp),
					BTL_DEBUG_pObStatus(parms->status)));

	BTL_LOG_DEBUG(("Event: %d, Op: %d, status: %d",  /* ???*/
					parms->event, parms->data->bppOp, parms->status));

	switch (parms->event)
	{
	case BPPCLIENT_DATA_REQ:

		/* Request to send next chunk of data to be printed from file */
		BTL_BPPSND_HandleClientDataRequest(bppsndContext, parms);
				
	break;

	case BPPCLIENT_COMPLETE:

		switch (parms->data->bppOp)
		{
		case BPPOP_CONNECT:

			if (parms->status == OB_STATUS_SUCCESS)
			{	
			
#if BPP_PRINTING_STATUS == XA_ENABLED

				/* Successfully connected to printer */
				if (parms->channel == BPPCH_SND_JOB)
				{
					ObStatus obStatus;

					BTL_LOG_INFO(("Printer Job channel is connected."));

			    	obStatus = BPP_Connect(&bppsndContext->session, &bppsndContext->printerAddress, BPPSVC_STS);

					if (OB_STATUS_PENDING == obStatus)
					{
						BTL_LOG_INFO(("Waiting for connection to establish Status channel"));
					}
					else if (OB_STATUS_SUCCESS == status)
					{
						BTL_LOG_INFO(("Connected To Printer Status channel"));	

						/* Let the app know that the job channel is up only after the status channel is up */
						BTL_BPPSND_HandleConnected(bppsndContext, parms);
					}
					else
					{
						BTL_LOG_ERROR(("Failed connecting to Printer Status channel"));

						/* Let the app know that the job channel is up even if the status channel is down */
						BTL_BPPSND_HandleConnected(bppsndContext, parms);
					}
				}
				else if (parms->channel == BPPCH_SND_STATUS)
				{
					BTL_LOG_INFO(("Printer Status channel is connected."));

					/* We indicate to app that the Job channel is established */
					parms->channel = BPPCH_SND_JOB;

					/* Let the app know that the job channel is up only after the status channel is up */
					BTL_BPPSND_HandleConnected(bppsndContext, parms);
				}
#else

				/* Successfully connected to printer */
				BTL_BPPSND_HandleConnected(bppsndContext, parms);

#endif	/* BPP_PRINTING_STATUS == XA_ENABLE */

			}
			
			else if (parms->status == OB_STATUS_DISCONNECT)
			{
			
#if BPP_PRINTING_STATUS == XA_ENABLED

				/* Successfully disconnected from printer */
				if (parms->channel == BPPCH_SND_JOB)
				{
					BTL_LOG_INFO(("Printer Job channel is disconnected."));
					
					BTL_BPPSND_HandleDisconnected(bppsndContext, parms);
				}
				else if (parms->channel == BPPCH_SND_STATUS)
				{
					BTL_LOG_INFO(("Printer Status channel is disconnected."));
				}

#else

				/* Successfully disconnected from printer */
				BTL_BPPSND_HandleDisconnected(bppsndContext, parms);

#endif	/* BPP_PRINTING_STATUS == XA_ENABLE */

			}
			else
			{
				BTL_ERR_NORET(("Invalid or Unexpected parms->status (%s) in case of BPPCLIENT_COMPLETE / BPPOP_CONNECT", 
									BTL_DEBUG_pObStatus(parms->status)));
			}
			
		break;

		case BPPOP_NOP:
			
			/* FALL THROUGH */
			
		case BPPOP_SIMPLEPUSH:
		case BPPOP_SENDDOCUMENT:

			Btl_Bppsnd_HandlePushOrSendCompleted(bppsndContext, parms);

		break;
		
		case BPPOP_CREATEJOB:
		case BPPOP_GETJOBATTR:
		case BPPOP_GETPRTATTR:
		case BPPOP_CANCELJOB:
		case BPPOP_GETEVENT :
		case BPPOP_SOAP:

			Btl_Bppsnd_HandleSoapRequestCompleted(bppsndContext, parms);

		break;
			
			
		default:
			
			BTL_ERR(BT_STATUS_INTERNAL_ERROR, 
					("Invalid or Unexpected BPP Op (%s), in case of BPPCLIENT_COMPLETE", 
					BTL_DEBUG_pBppOp(parms->data->bppOp)));
						

		}
	
	break;

	case BPPCLIENT_FAILED:

		BTL_BPPSND_HandleFailure(bppsndContext, parms);

	break;
	
	case BPPCLIENT_ABORT:

		if (parms->status == OBRC_UNAUTHORIZED)
		{
			/* Security failed disconnect transport */
			GOEP_TpDisconnect(&parms->obex.client->cApp);
		}

		BTL_BPPSND_HandleOperationAborted(bppsndContext, parms);
		
	break;

	case BPPSERVER_REQUEST:
		switch (parms->data->bppOp)
		{
		case BPPOP_GETREFOBJ:

			Btl_Bppsnd_HandleStartGetRefObject(bppsndContext, parms);
			
		break;

		default:
			
			BTL_ERR(BT_STATUS_INTERNAL_ERROR, 
					("Invalid or Unexpected BPP Op (%s), in case of BPPSERVER_REQUEST", 
					BTL_DEBUG_pBppOp(parms->data->bppOp)));


			
		}
		
	break;

	case BPPSERVER_DATA_REQ:

		BTL_BPPSND_HandletGetRefObjectData(parms);
		
	break;

	case BPPSERVER_COMPLETE:
		
		switch (parms->data->bppOp)
		{
			case BPPOP_CONNECT:
				if (parms->status == OB_STATUS_SUCCESS)
				{
					BTL_LOG_INFO(("Ref Obj Chnl Connect Complete, channel = %x", parms->channel));
					
#if BTL_CONFIG_BPPSND_DISCONNECT_TIMEOUT > 0

					/* The printer open an object channel for referenced objects.
					Cancel disconnection timer, if exists. */
					if (TRUE == bppsndContext->disconnectionTimerPending)
					{
						BTL_LOG_INFO(("Delaying ACL disconnection...", parms->channel));
						
						EVM_CancelTimer(&(bppsndContext->disconnectionTimer));
					}		

#endif /* BTL_CONFIG_BPPSND_DISCONNECT_TIMEOUT > 0 */

					BTL_BPPSND_SendEventToApp(bppsndContext, parms);

				}
				else if (parms->status == OB_STATUS_DISCONNECT)
				{
					BTL_LOG_INFO(("Ref Obj Chnl Disconnect Complete, channel = %x", parms->channel));
					
#if BTL_CONFIG_BPPSND_DISCONNECT_TIMEOUT > 0

					if (TRUE == bppsndContext->disconnectionTimerPending)
					{
						BTL_LOG_INFO(("Now we disconnect ACL...", parms->channel));

						bppsndContext->disconnectionTimerPending = FALSE;
						
						status = Btl_Bppsnd_Disconnect( bppsndContext );
						BTL_BPPSND_InitOperationVariables(bppsndContext);
						if (BT_STATUS_PENDING != status)
						{
							/* bppsndContext->state = BTL_BPPSND_STATE_DISCONNECTED; */
							bppsndContext->complexApiState = BTL_BPPSND_STATE_IDLE;
						}
					}		

#endif /* BTL_CONFIG_BPPSND_DISCONNECT_TIMEOUT > 0 */

					btlBppsndData.selfGeneratedCallbackParms = *parms;
					btlBppsndData.selfGeneratedCallbackParms.data->bppOp = BPPOP_DISCONNECT;
					btlBppsndData.selfGeneratedCallbackParms.status = OB_STATUS_SUCCESS;

					BTL_BPPSND_SendEventToApp(bppsndContext, &btlBppsndData.selfGeneratedCallbackParms);

				}
			break;

			case BPPOP_GETREFOBJ:

				BTL_BPPSND_HandletGetRefObjectDataCompleted(bppsndContext, parms);

			break;

			default:
						
				BTL_ERR(BT_STATUS_INTERNAL_ERROR, 
						("Invalid or Unexpected BPP Op (%s), in case of BPPSERVER_COMPLETE", 
						BTL_DEBUG_pBppOp(parms->data->bppOp)));


		}
		
	break;

	case BPPSERVER_FAILED:
		
		BTL_LOG_ERROR(("Ref Obj Chnl Session Failed status = %d", parms->status));
		
	break;

	case BPPSERVER_ABORT:
		
		BTL_LOG_ERROR(("Ref Obj Chnl Aborted status = %d", parms->status));
		
	break;

	case BPPSERVER_CONTINUE:
				
		/* Indicate ready to connect or receive more data
		* On connect indication, parms->data->bppOp = BPPOP_CONNECT.
		*/
		BPP_ServerContinue(parms->obex.server);
		
	break;

#if OBEX_AUTHENTICATION == XA_ENABLED
	case BPPCLIENT_AUTH_CHALLENGE_RCVD:
		
		Btl_Bppsnd_HandleChallengeRcvd(bppsndContext, parms);
#endif
	break;

	case BPPCLIENT_DATA_IND:

		Btl_Bppsnd_HandleIncomingSoapResponse(bppsndContext, parms);
				
	break;

	case BPPCLIENT_RESPONSE:
		/* finished receiving SOAP response:*/
		BTL_LOG_DEBUG(("BPPCLIENT_RESPONSE %s channel=%x, len=%d, offset=%d, totLen=%d, buff=0x%x", 
			BTL_DEBUG_pBppOp(parms->data->bppOp), 
			parms->channel, parms->data->len, parms->data->offset, parms->data->totLen, parms->data->buff));
		
	break;
	
	default:
		
		BTL_ERR(BT_STATUS_INTERNAL_ERROR, ("Invalid or Unexpected BPP Event (%s)", BTL_DEBUG_pBppEvent(parms->event)));
			

	}

	BTL_FUNC_END();
}

BtlBppsndContext *BTL_BPPSND_GetContextFromBppCallbackParms(BppCallbackParms* parms)
{
	BtlBppsndContext *bppsndContext = 0;
	
	BTL_FUNC_START("");
		
	switch (parms->event)
	{
		case BPPCLIENT_DATA_REQ:
		case BPPCLIENT_DATA_IND:
		case BPPCLIENT_RESPONSE:
		case BPPCLIENT_COMPLETE:
		case BPPCLIENT_FAILED:
		case BPPCLIENT_ABORT:
		case BPPCLIENT_PROGRESS:
		case BPPCLIENT_AUTH_CHALLENGE_RCVD:

#if BPP_PRINTING_STATUS == XA_ENABLED

			if (parms->channel == BPPCH_SND_JOB)
				bppsndContext = ContainingRecord(parms->obex.client, BtlBppsndContext, session.sndJob);
			else	/* BPPCH_SND_STATUS */
				bppsndContext = ContainingRecord(parms->obex.client, BtlBppsndContext, session.sndStatus);

#else

			bppsndContext = ContainingRecord(parms->obex.client, BtlBppsndContext, session.sndJob);

#endif	/* BPP_PRINTING_STATUS == XA_ENABLE */

		break;

		case BPPSERVER_DATA_REQ:
		case BPPSERVER_DATA_IND:
		case BPPSERVER_REQUEST:
		case BPPSERVER_COMPLETE:
		case BPPSERVER_FAILED:
		case BPPSERVER_ABORT:
		case BPPSERVER_CONTINUE:

			bppsndContext = ContainingRecord(parms->obex.client, BtlBppsndContext, session.sndObject);
				
		break;
		
		default:

			BTL_FATAL_SET_RETVAR(bppsndContext = 0, ("Unexpected BPP Callback Event Type (%d)", parms->event));
			
	};

	BTL_FUNC_END();
	
	return bppsndContext;
}

BtStatus BTL_BPPSND_HandleClientDataRequest(BtlBppsndContext *bppsndContext, BppCallbackParms* parms)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BTL_BPPSND_HandleClientDataRequest");

	BTL_LOG_DEBUG(("Data Requested: Offset: %d, Len : %d", parms->data->offset, parms->data->len));

	if (	( BTL_BPPSND_STATE_PRINTING == bppsndContext->complexApiState ) &&
		( BPPOP_NOP == parms->data->bppOp) )
	{
		parms->data->bppOp = BPPOP_SIMPLEPUSH;
	}

	status = BTL_BPPSND_ReadDocument(parms->data, bppsndContext);

	if (BT_STATUS_SUCCESS == status)
	{
		BTL_BPPSND_SendProgressNotificationToApp(bppsndContext, parms);
	}
	else
	{
		/* Setting length to 0 causes completion of operation */
		parms->data->len = 0;

		/* Printing failed, set fields for UI notification at the end */
		bppsndContext->failureReason = OBRC_CLIENT_RW_ERROR;

		Btl_Bppsnd_SendObexAbort(bppsndContext, FALSE);
		
		BTL_LOG_ERROR(("Failed reading %s", bppsndContext->fsPath));
		BTL_RET(status);
	}
	
	BTL_FUNC_END();

	return status;
}

/*---------------------------------------------------------------------------
 *            BppHandleConnected
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Handles change of the complex operation state meachine.
 *                 It MUST be called after the 'state' field has already received its new value
 *
 */
void Btl_Bppsnd_HandleMainStateMachine( BtlBppsndContext *bppsndContext, BppCallbackParms* parms, BppEvent bppEvent)
{		
	BTL_FUNC_START("Btl_Bppsnd_HandleMainStateMachine");

	if (bppsndContext->disablingState != BTL_BPPSND_DISABLING_STATE_IDLE)
	{
		Btl_Bppsnd_DisablingProcessor(bppsndContext, parms);
	}
	else if (btlBppsndData.radioOffState != BTL_BPPSND_RADIO_OFF_STATE_IDLE)
	{
		Btl_Bppsnd_ContextRadioOffProcessor(bppsndContext);
	}
	else
	{
		Btl_Bppsnd_HandleComplexStateMachine(bppsndContext, bppEvent);
	}
	
	BTL_FUNC_END();
}

/*---------------------------------------------------------------------------
 *            BppHandleConnected
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Handles change of the complex operation state meachine.
 *                 It MUST be called after the 'state' field has already received its new value
 *
 */
static void Btl_Bppsnd_HandleComplexStateMachine( BtlBppsndContext *bppsndContext, BppEvent bppEvent)
{
	BtStatus 		status = BT_STATUS_SUCCESS;
	BtlBppsndState	prevComplexState;
		
	BTL_FUNC_START("Btl_Bppsnd_HandleComplexStateMachine");

	prevComplexState = bppsndContext->complexApiState;
	
	switch ( bppsndContext->complexApiState )
	{
		case BTL_BPPSND_STATE_CONNECTING:
			if (BTL_BPPSND_STATE_CONNECTED == bppsndContext->state)
			{	
				/* If disconnection is pending, don't start printing and wait for disconnection */
				if (bppsndContext->disconnectionPending == FALSE)
				{
					/* we arrived connected state - start the simple push print*/
					status = Btl_Bppsnd_PushOrSendDocument(bppsndContext, &(bppsndContext->objToPrint), BPPOP_SIMPLEPUSH, 0, "ConnectPushDisco");
					if ( BT_STATUS_PENDING == status )
					{	 /* started sending the object to printer*/
						bppsndContext->complexApiState = BTL_BPPSND_STATE_PRINTING;
					}
					else
					{	/* either failed to start or started and already completed*/
						status = Btl_Bppsnd_Disconnect(bppsndContext);
						BTL_BPPSND_InitOperationVariables(bppsndContext);
						bppsndContext->complexApiState = BTL_BPPSND_STATE_DISCONNECTING;
					}
				}

				break;
			}
			if ( BPPCLIENT_ABORT == bppEvent ||  BPPCLIENT_FAILED == bppEvent )
			{
				bppsndContext->complexApiState = BTL_BPPSND_STATE_IDLE;
			}
			else
			{
				BTL_LOG_ERROR(("ERROR: in complex-state CONNECTING, but state did not become CONNECTED nor ABORTED !!"));
			}
			break;

		case BTL_BPPSND_STATE_PRINTING:
			if ( BTL_BPPSND_STATE_CONNECTED == bppsndContext->state )
			{	/* we arrived connected state - it means the simple push has completed, */
				/* or failed/aborted from some reason.  but in any case it is over*/
				bppsndContext->complexApiState = BTL_BPPSND_STATE_DISCONNECTING;

#if BTL_CONFIG_BPPSND_DISCONNECT_TIMEOUT > 0

				/* The printer might open an object channel for referenced objects.
				So, before we disconnect ACL, we wait for BTL_CONFIG_BPPSND_DISCONNECT_TIMEOUT ms
				for the printer to establish the object channel.
				If such channel is estabished, we disconnect only after the printer disconnect
				this object channel.
				If no such channel is established, we disconnect when the timer expired. */

				BTL_LOG_INFO(("Starting ACL disconnection timer to let the printer get referenced objects..."));

				bppsndContext->disconnectionTimerPending = TRUE;

				EVM_StartTimer(&(bppsndContext->disconnectionTimer), BTL_CONFIG_BPPSND_DISCONNECT_TIMEOUT);

#else
	
				status = Btl_Bppsnd_Disconnect(bppsndContext);
				BTL_BPPSND_InitOperationVariables(bppsndContext);
				if ( (BT_STATUS_SUCCESS != status) && (BT_STATUS_PENDING != status) )
					bppsndContext->complexApiState = BTL_BPPSND_STATE_IDLE;

#endif /* BTL_CONFIG_BPPSND_DISCONNECT_TIMEOUT > 0 */
				
			}
			else
			{
				/* ^^ ??? shouldn't get here*/
				BTL_LOG_ERROR(("ERROR: invalid state in complex-state PRINTING, but state did not become CONNECTED !!"));
				if ( BPPCLIENT_COMPLETE != bppEvent )
				{
					bppsndContext->complexApiState = BTL_BPPSND_STATE_IDLE;
				}
			}
			break;

		case BTL_BPPSND_STATE_DISCONNECTING:
			if ( BTL_BPPSND_STATE_DISCONNECTED == bppsndContext->state )
			{	/* disconnecting has completed, */
				bppsndContext->complexApiState = BTL_BPPSND_STATE_IDLE;
			}
			break;

		default:
			BTL_LOG_DEBUG(("HandleComplexStateMachine, state=%s, SM N/A", 
				BTL_BPPSND_pState(bppsndContext->complexApiState)));
			break;
				
	}
	BTL_LOG_DEBUG(("HandleComplexStateMachine: change state from %s to %s",
		BTL_BPPSND_pState(prevComplexState), BTL_BPPSND_pState(bppsndContext->complexApiState)));
	
	BTL_FUNC_END();
		
}

/*---------------------------------------------------------------------------
 *            BppHandleConnected
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Handles Successful connection established event from the stack
 *
 */
BtStatus BTL_BPPSND_HandleConnected(BtlBppsndContext *bppsndContext, BppCallbackParms* parms)
{
	BtStatus 		status = BT_STATUS_SUCCESS;
	BD_ADDR	bdAddress;

	BTL_FUNC_START("BTL_BPPSND_HandleConnected");

	BTL_BPPSND_SendEventToApp(bppsndContext, parms);

	bppsndContext->state = BTL_BPPSND_STATE_CONNECTED;
	
	BTL_BPPSND_GetConnectedDevice(bppsndContext, &bdAddress);
	BTL_LOG_INFO(("Connected to Printer (%s)", BTL_UTILS_LockedBdaddrNtoa(&bdAddress)));

	Btl_Bppsnd_HandleMainStateMachine( bppsndContext, parms, BPPCLIENT_COMPLETE);
	BTL_FUNC_END();

	return status;
}

BtStatus BTL_BPPSND_HandleDisconnected(BtlBppsndContext *bppsndContext, BppCallbackParms* parms)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START("BTL_BPPSND_HandleDisconnected");
	
	BTL_BPPSND_CloseDocument(parms->data,bppsndContext);

	bppsndContext->state = BTL_BPPSND_STATE_DISCONNECTED;
	bppsndContext->jobId = 0;
	bppsndContext->abortState = BTL_BPPSND_ABORT_STATE_IDLE;
	bppsndContext->disconnectionPending = FALSE;
	
#if OBEX_AUTHENTICATION == XA_ENABLED
	bppsndContext->obexAuthFlags = 0;
#endif

	bppsndContext->complexApiState = BTL_BPPSND_STATE_IDLE;

#if BTL_CONFIG_BPPSND_DISCONNECT_TIMEOUT > 0

	/* Cancel disconnection timer, if exists. */
	EVM_CancelTimer(&(bppsndContext->disconnectionTimer));	

	bppsndContext->disconnectionTimerPending = FALSE;

#endif /* BTL_CONFIG_BPPSND_DISCONNECT_TIMEOUT > 0 */

	btlBppsndData.selfGeneratedCallbackParms = *parms;
	btlBppsndData.selfGeneratedCallbackParms.data->bppOp = BPPOP_DISCONNECT;
	btlBppsndData.selfGeneratedCallbackParms.status = OB_STATUS_SUCCESS;

	BTL_BPPSND_SendEventToApp(bppsndContext, &btlBppsndData.selfGeneratedCallbackParms);

	Btl_Bppsnd_HandleMainStateMachine(  bppsndContext, parms, BPPCLIENT_COMPLETE );

	BTL_FUNC_END();

	return status;
	
}

#if OBEX_AUTHENTICATION == XA_ENABLED
BtStatus Btl_Bppsnd_HandleChallengeRcvd(BtlBppsndContext *bppsndContext, BppCallbackParms* parms)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START("Btl_Bppsnd_HandleChallengeRcvd");

	if ( ! (bppsndContext->obexAuthFlags & OBEX_AUTH_INFO_SET) )
	{
		btlBppsndData.selfGeneratedCallbackParms = *parms;
		
		bppsndContext->obexAuthFlags |= OBEX_AUTH_RECEIVED_CHAL | OBEX_AUTH_DURING_CALLBACK;

		bppsndContext->obexAuthFlags  |= OBEX_AUTH_INFO_SET;
		
		BTL_BPPSND_SendEventToApp(bppsndContext, &btlBppsndData.selfGeneratedCallbackParms);
		
		bppsndContext->obexAuthFlags &= ~OBEX_AUTH_DURING_CALLBACK;
	}
	BTL_FUNC_END();

	return status;
	
}
#endif

/*---------------------------------------------------------------------------
 *            BppHandleSimplePushCompleted
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Handles Successful Simple Push completed event from the stack
 *
 */
BtStatus Btl_Bppsnd_HandlePushOrSendCompleted(BtlBppsndContext *bppsndContext, BppCallbackParms* parms)
{	
	BtStatus 		status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START("Btl_Bppsnd_HandlePushOrSendCompleted");
	
	BTL_BPPSND_CloseDocument(parms->data,bppsndContext);

	btlBppsndData.selfGeneratedCallbackParms = *parms;
	btlBppsndData.selfGeneratedCallbackParms.data->bppOp = parms->data->bppOp;
	btlBppsndData.selfGeneratedCallbackParms.status = bppsndContext->failureReason;

	BTL_BPPSND_SendEventToApp(bppsndContext, &btlBppsndData.selfGeneratedCallbackParms);

	BTL_LOG_INFO(("%s of %s completed successfully", 
		parms->data->bppOp == BPPOP_SIMPLEPUSH ? "Simple Push" : "Send Document", bppsndContext->fsPath));
	
	bppsndContext->state = BTL_BPPSND_STATE_CONNECTED;

	if ( BPPOP_SIMPLEPUSH == parms->data->bppOp )
	{
		Btl_Bppsnd_HandleMainStateMachine(  bppsndContext, parms, BPPCLIENT_COMPLETE );
	}

	BTL_FUNC_END();

	return status;
}

/*---------------------------------------------------------------------------
 *            Btl_Bppsnd_HandleSoapRequestCompleted
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Handles Successful SOAP request event from the stack
 *
 */
BtStatus Btl_Bppsnd_HandleSoapRequestCompleted(BtlBppsndContext *bppsndContext, BppCallbackParms* parms)
{	
	BtStatus 		status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START("Btl_Bppsnd_HandleSoapRequestCompleted");

		/*switch (parms->data->bppOp)*/

	BTL_BPPSND_CloseDocument(&bppsndContext->sndData, bppsndContext);

	btlBppsndData.selfGeneratedCallbackParms = *parms;
	btlBppsndData.selfGeneratedCallbackParms.data->bppOp = parms->data->bppOp;
	btlBppsndData.selfGeneratedCallbackParms.status = bppsndContext->failureReason;

	if ( OB_STATUS_SUCCESS == bppsndContext->failureReason )
	{
		BTL_LOG_INFO(("%s completed successfully", BTL_DEBUG_pBppOp(parms->data->bppOp)));
	}
	else
	{
		BTL_LOG_ERROR(("%s failed due to insufficient memory for SOAP response", BTL_DEBUG_pBppOp(parms->data->bppOp)));
	}

	bppsndContext->state = BTL_BPPSND_STATE_CONNECTED;

	BTL_BPPSND_SendEventToApp(bppsndContext, &btlBppsndData.selfGeneratedCallbackParms);

	BTL_FUNC_END();

	return status;
}

BtStatus	Btl_Bppsnd_HandleIncomingSoapResponse(BtlBppsndContext *bppsndContext, BppCallbackParms* parms)
{
	BtStatus 		status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START("Btl_Bppsnd_HandleIncomingSoapResponse");

	BTL_LOG_DEBUG(("BPPCLIENT_DATA_IND channel=%x, len=%d, offset=%d, totLen=%d, buff=0x%x", 
			parms->channel, parms->data->len, parms->data->offset, parms->data->totLen, parms->data->buff));

	parms->data->ocx = bppsndContext->sndData.ocx;
		
	status = BTL_BPPSND_SeekDocument(parms->data, bppsndContext);
	BTL_VERIFY_ERR(BT_STATUS_SUCCESS == status, status, 
		("Failed to seek in SOAP response Obj (offset=%d, status=%s)", parms->data->offset, pBT_Status(status)));

	if ( BTL_OBJECT_LOCATION_MEM == bppsndContext->responseObj.objectLocation )
	{
		/* check  that the buffer in the response object is large enough for totLen*/
		if ( (parms->data->offset + parms->data->len) < bppsndContext->responseObj.location.memLocation.size )
		{
			((U8*)bppsndContext->sndData.ocx)[(parms->data->offset + parms->data->len)] = '\0';
		}
		else
		{
			bppsndContext->failureReason = OB_STATUS_NO_RESOURCES;
			BTL_ERR(BT_STATUS_NO_RESOURCES, 
				("SOAP response too large for Obj (totLen=%d, max mem size=%d)",
				parms->data->totLen, bppsndContext->responseObj.location.memLocation.size));
		}
	}

	/* finally copy the newly arrived data into the response object (file or mem)*/
	Btl_Bppsnd_WriteDocument(parms->data, bppsndContext);

	BTL_FUNC_END();

	return status;
}		

/*---------------------------------------------------------------------------
 *            BppHandlePrintingFailure
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Handles some Failure during Printing process event from the stack
 *
 */
BtStatus BTL_BPPSND_HandleFailure(BtlBppsndContext *bppsndContext, BppCallbackParms* parms)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BTL_BPPSND_HandleFailure");
	
	BTL_LOG_DEBUG(("Session Failed status = %d. Printing state: %s", 
					parms->status, BTL_BPPSND_pState(bppsndContext->state)));

	/*
		The action to be taken depends on the current state.
		If an operation is in progress, abort it.
		In the end, disconnect
	*/
	switch (bppsndContext->state)
	{
	case BTL_BPPSND_STATE_CONNECTING:

		btlBppsndData.selfGeneratedCallbackParms = *parms;
		btlBppsndData.selfGeneratedCallbackParms.data->bppOp = BPPOP_CONNECT;
		btlBppsndData.selfGeneratedCallbackParms.status = parms->status;

		BTL_BPPSND_SendEventToApp(bppsndContext, &btlBppsndData.selfGeneratedCallbackParms);

		bppsndContext->state = BTL_BPPSND_STATE_DISCONNECTED;

	break;
	
	case BTL_BPPSND_STATE_PRINTING:

		BTL_BPPSND_CloseDocument(parms->data,bppsndContext);

		btlBppsndData.selfGeneratedCallbackParms = *parms;
		btlBppsndData.selfGeneratedCallbackParms.data->bppOp = BPPOP_SIMPLEPUSH;
		btlBppsndData.selfGeneratedCallbackParms.status = parms->status;

		BTL_BPPSND_SendEventToApp(bppsndContext, &btlBppsndData.selfGeneratedCallbackParms);

		bppsndContext->state = BTL_BPPSND_STATE_CONNECTED;

	break;

	case BTL_BPPSND_STATE_DISCONNECTING:

		btlBppsndData.selfGeneratedCallbackParms = *parms;
		btlBppsndData.selfGeneratedCallbackParms.data->bppOp = BPPOP_DISCONNECT;
		btlBppsndData.selfGeneratedCallbackParms.status = parms->status;

		BTL_BPPSND_SendEventToApp(bppsndContext, &btlBppsndData.selfGeneratedCallbackParms);

		bppsndContext->state = BTL_BPPSND_STATE_CONNECTED;

	break;

	case BTL_BPPSND_STATE_SENT_REQUEST:

		btlBppsndData.selfGeneratedCallbackParms = *parms;
		btlBppsndData.selfGeneratedCallbackParms.status = parms->status;

		BTL_BPPSND_SendEventToApp(bppsndContext, &btlBppsndData.selfGeneratedCallbackParms);
		
		bppsndContext->state = BTL_BPPSND_STATE_CONNECTED;

		break;

	default:

		BTL_ERR(BT_STATUS_INTERNAL_ERROR, ("Invalid or Unexpectd State: %d", bppsndContext->state));
			

	};

	Btl_Bppsnd_HandleMainStateMachine( bppsndContext, parms, BPPCLIENT_FAILED);

	BTL_FUNC_END();

	return status;
}

/*---------------------------------------------------------------------------
 *            BppHandlePrintingAborted
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Handles disconncted completed event from the stack
 *
 */
BtStatus BTL_BPPSND_HandleOperationAborted(BtlBppsndContext *bppsndContext, BppCallbackParms* parms)
{
	BtStatus	status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BTL_BPPSND_HandleOperationAborted");
	
	BTL_LOG_INFO(("Operation Aborted. Sender State: %d, Status: %d channel = %d", 
					bppsndContext->state, parms->status, parms->channel));

	bppsndContext->abortState = BTL_BPPSND_ABORT_STATE_IDLE;
	
	/* 
		If connection establishment fails, for example, due to a failed OBEX authentication,
		the stack aborts on behalf of the BPP application. In that case an aborted event is received.
	*/
	switch (bppsndContext->state)
	{
	case BTL_BPPSND_STATE_CONNECTING:

		btlBppsndData.selfGeneratedCallbackParms = *parms;
		btlBppsndData.selfGeneratedCallbackParms.data->bppOp = BPPOP_CONNECT;
		btlBppsndData.selfGeneratedCallbackParms.status = parms->status;

		bppsndContext->obexAuthFlags &= ~OBEX_AUTH_INFO_SET;
		
		BTL_BPPSND_SendEventToApp(bppsndContext, &btlBppsndData.selfGeneratedCallbackParms);

		bppsndContext->state = BTL_BPPSND_STATE_DISCONNECTED;

	break;
	
	case BTL_BPPSND_STATE_PRINTING:

		BTL_BPPSND_CloseDocument(parms->data,bppsndContext);

		btlBppsndData.selfGeneratedCallbackParms = *parms;
		btlBppsndData.selfGeneratedCallbackParms.data->bppOp = BPPOP_SIMPLEPUSH; /*  ^^ or senddoc ???*/

		if (BTL_BPPSND_ABORT_STATE_USER_ABORT_IN_PROGRESS == bppsndContext->abortState)
		{
			btlBppsndData.selfGeneratedCallbackParms.status = OBRC_SESSION_USER_ABORT;
		}
		else
		{
			btlBppsndData.selfGeneratedCallbackParms.status = parms->status;
		}

		BTL_BPPSND_SendEventToApp(bppsndContext, &btlBppsndData.selfGeneratedCallbackParms);

		bppsndContext->state = BTL_BPPSND_STATE_CONNECTED;

	break;

	case BTL_BPPSND_STATE_DISCONNECTING:

		btlBppsndData.selfGeneratedCallbackParms = *parms;
		btlBppsndData.selfGeneratedCallbackParms.data->bppOp = BPPOP_DISCONNECT;
		btlBppsndData.selfGeneratedCallbackParms.status = parms->status;

		BTL_BPPSND_SendEventToApp(bppsndContext, &btlBppsndData.selfGeneratedCallbackParms);

	break;

	case BTL_BPPSND_STATE_SENT_REQUEST:

		btlBppsndData.selfGeneratedCallbackParms = *parms;
		btlBppsndData.selfGeneratedCallbackParms.status = parms->status;

		BTL_BPPSND_SendEventToApp(bppsndContext, &btlBppsndData.selfGeneratedCallbackParms);
		
		bppsndContext->state = BTL_BPPSND_STATE_CONNECTED;

	break;
	
	
	default:

		BTL_ERR(BT_STATUS_INTERNAL_ERROR, ("Invalid or Unexpected bppsndContext->state: %d", bppsndContext->state));
		

	};

	Btl_Bppsnd_HandleMainStateMachine( bppsndContext, parms, BPPCLIENT_ABORT );

	BTL_FUNC_END();
	
	return status;
}

/*-------------------------------------------------------------------------------
 * Btl_Bppsnd_Disconnect()
 */
BtStatus Btl_Bppsnd_Disconnect(BtlBppsndContext *bppsndContext)
{
	BtStatus 	status = BT_STATUS_SUCCESS;
	ObStatus obStatus;
	
	BTL_FUNC_START("Btl_Bppsnd_Disconnect");

	if (bppsndContext->state == BTL_BPPSND_STATE_CONNECTING)
	{
		BTL_LOG_INFO(("Cancelling the connection"));
		
		bppsndContext->disconnectionPending = TRUE;
	}

	if ((BTL_BPPSND_IsConnected(bppsndContext) == FALSE) &&
        (BTL_BPPSND_IsConnecting(bppsndContext) == FALSE))
	{
			BTL_RET(BT_STATUS_SUCCESS);
	}

	bppsndContext->state = BTL_BPPSND_STATE_DISCONNECTING;
	
	obStatus = BPP_Disconnect(&bppsndContext->session);

#if 0
	if (OB_STATUS_SUCCESS == obStatus)
	{
		bppsndContext->state = BTL_BPPSND_STATE_DISCONNECTED;
		/*XXXbppsndContext->complexApiState = BTL_BPPSND_STATE_IDLE;*/

		status = BT_STATUS_SUCCESS;
	}
#endif

	if (OB_STATUS_PENDING == obStatus)
	{
		status = BT_STATUS_PENDING;		
	}
	else
	{
		BTL_VERIFY_FATAL(OB_STATUS_BUSY == obStatus, BT_STATUS_INTERNAL_ERROR, 
							("Unexpected Result (%s) from BPP_Disconnect", BTL_UTILS_ObStatus2BtStatus(obStatus)));
		
		BTL_LOG_INFO(("Calling GOEP_TpDisconnect to disconnect operation before disconnecting"));

		obStatus = GOEP_TpDisconnect(&bppsndContext->session.sndJob.cApp);
		BTL_VERIFY_FATAL((OB_STATUS_PENDING == obStatus) || (OB_STATUS_SUCCESS == obStatus), BT_STATUS_INTERNAL_ERROR, 
							("Unexpected Result (%s) from GOEP_TpDisconnect", BTL_UTILS_ObStatus2BtStatus(obStatus)));

		if (OB_STATUS_SUCCESS == obStatus)
		{
	        bppsndContext->state = BTL_BPPSND_STATE_DISCONNECTED;
            status = BT_STATUS_SUCCESS;
		}
        else
        {
            status = BT_STATUS_PENDING;
        }
	}

	BTL_FUNC_END();

	return status;
}

/* 
*   Btl_Bppsnd_IsAbsoluteName
*
*   return true if given file name has an absolute path . 
*   it actually should be implemented in BTHAL.
*
*/
BOOL Btl_Bppsnd_IsAbsoluteName( const BtlUtf8 *fileName )
{
	BOOL result = FALSE;

	BtFsStatus status;

	status = BTHAL_FS_IsAbsoluteName( fileName, &result );

	BTL_LOG_DEBUG(("Is Absolute File Name(%s) returns %d, status=%d" , ((const char*)fileName) ? ((const char*)fileName) : " null", result, status));
	return result;
}

/* 
*   Btl_Bppsnd_GetReferencedObjectFromApp
*
*   make a callback to the app asking it to call BTL_BPPSND_GetReferenceObjectResponse 
*   in order to resolve the relative path of a file referenced by a memory object.
*
*/static BtStatus Btl_Bppsnd_GetReferencedObjectFromApp(BtlBppsndContext *bppsndContext, BppCallbackParms* parms)
{
	BTL_FUNC_START("Btl_Bppsnd_GetReferencedObjectFromApp");
	/* save all the aprameters needed when will call BTL_BPPSND_GetReferenceObjectResponse*/
	bppsndContext->sndData.len = 	parms->data->len;
	bppsndContext->sndData.offset= 	parms->data->offset;
	bppsndContext->obexServer = 	parms->obex.server;
	
	BTL_BPPSND_SendEventToApp(bppsndContext, parms);
	
	BTL_FUNC_END();
	
	return BT_STATUS_SUCCESS;
}

BtStatus Btl_Bppsnd_HandleStartGetRefObject(BtlBppsndContext *bppsndContext, BppCallbackParms* parms)
{
	BtStatus 	status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("Btl_Bppsnd_HandleStartGetRefObject");

	BTL_LOG_INFO(("Printer requsets referenced object. Name: |%s, Offset: %d, Count: %d",
					parms->data->buff, parms->data->offset, parms->data->len));

	/* if printing from memory and we deal with refrenced object without full path - call app first*/
	if ( !Btl_Bppsnd_IsAbsoluteName( parms->data->buff ) && bppsndContext->mainDocIsMemObj )
	{
		status = Btl_Bppsnd_GetReferencedObjectFromApp(bppsndContext, parms);
	}
	else
	{
		status = Btl_Bppsnd_HandleStartGetResolvedRefObject( bppsndContext,  parms, NULL);
	}
	BTL_FUNC_END();

	return status;
}

BtStatus Btl_Bppsnd_HandleStartGetResolvedRefObject(BtlBppsndContext *bppsndContext, 
															BppCallbackParms* parms, 
															const BtlObject *responseObj)
{
	BtStatus 	status = BT_STATUS_SUCCESS;
	BOOL	result;
	BtlUtf8 	refFileFullName[BTL_BPPSND_MAX_FSPATH_LEN + 1];
	BtlBppsndContext refBppsndContext = {0};
	BtlObject refObjToPrint;

	BTL_FUNC_START("Btl_Bppsnd_HandleStartGetResolvedRefObject");

	/* Set up temporary objects needed for OpenDocument */
	refObjToPrint.location.fsLocation.fsPath = refFileFullName;
	/* if main object was from file then we assume that referenced objects are file names too*/
	/* when responseObj is NULL it means we didn't have to call back the app to resolve a relative*/
	/* path, which implies the referenced object must be a file*/
	refBppsndContext.printingMemory = 
	bppsndContext->printingMemory = responseObj && (BTL_OBJECT_LOCATION_MEM == responseObj->objectLocation);
	/* Assume Failure */
	refBppsndContext.sndData.totLen = 0;
	refBppsndContext.sndData.len = 0;

	if ( ! bppsndContext->printingMemory )
	{
		result = BTL_BPPSND_GetReferencedObjectFullFileName(bppsndContext->fsPath,
														(BtlUtf8*)parms->data->buff, 
														refFileFullName);
		BTL_VERIFY_ERR(TRUE == result, BT_STATUS_FAILED, ("Failed obtaining ref obj full file name"));
		BTL_LOG_DEBUG(("Opening Ref Obj: |%s|", refFileFullName));

		refObjToPrint.objectLocation = BTL_OBJECT_LOCATION_FS;
	}
	else
	{
		OS_StrCpy((char*)refFileFullName,  "refMemObj");
		OS_MemCopy((U8*) &refObjToPrint, (U8*) responseObj, sizeof(BtlObject));
	}


	/* Open the referenced object */
	status = BTL_BPPSND_OpenDocument(&refObjToPrint, &refBppsndContext, TRUE);
	BTL_VERIFY_ERR(BT_STATUS_SUCCESS == status, status, 
					("Failed Opening Ref Obj File Name (%s, %s)", refFileFullName, pBT_Status(status)));

	status = BTL_BPPSND_GetDocumentSize(&refObjToPrint, &refBppsndContext);
	BTL_VERIFY_ERR(BT_STATUS_SUCCESS == status, status, 
					("Failed Obtaining Ref Obj File Size (%s, %s)", refFileFullName, pBT_Status(status)));

	BTL_LOG_DEBUG(("FileSize = %d", refBppsndContext.sndData.totLen));

	/* Set the length of response data to amount requested */
	refBppsndContext.sndData.len = min(parms->data->len, refBppsndContext.sndData.totLen - parms->data->offset);

	BTL_LOG_DEBUG(("data.len = %d, data.totLen = %d", refBppsndContext.sndData.len, refBppsndContext.sndData.totLen));
 
	/* Send response */
	status = BPP_SendReferencedObjectRsp(parms->obex.server, &(refBppsndContext.sndData));	
	BTL_VERIFY_ERR(BT_STATUS_SUCCESS == status, status, 
					("BPP_SendReferencedObjectRsp Failed (%s)", pBT_Status(status)));
	
	BTL_FUNC_END();

	return status;
}

BOOL BTL_BPPSND_GetReferencedObjectFullFileName(const BtlUtf8 	*printedFileFullName,
															const BtlUtf8	*referencedFileName, 
															BtlUtf8 		*referencedFileFullName)
{
	BOOL	result = TRUE;
	
	BTL_FUNC_START("BTL_BPPSND_GetReferencedObjectFullFileName");

	OS_StrCpy((char*)referencedFileFullName, "");
	
	/* Relative file names are not supported */
	BTL_VERIFY_ERR_SET_RETVAR(	referencedFileName[0] != '.', 
									result = FALSE,
									("References using '.' or '..' are not supported (|%s|)", referencedFileName));

	if ( Btl_Bppsnd_IsAbsoluteName(referencedFileName) )
	{
		/* Absolute file name was specified => use it as is */
		
		BTL_VERIFY_ERR_SET_RETVAR(	OS_StrLen((const char*)referencedFileName) <= BTL_BPPSND_MAX_FSPATH_LEN,
										result = FALSE,
										("Referenced File Name (%s) is TOO LONG. Max Len: %d",
										referencedFileName, BTL_BPPSND_MAX_FSPATH_LEN));

		OS_StrCpy((char*)referencedFileFullName, (const char*)referencedFileName);
	}
	else 
	{
		BtlUtf8 printedFileFolderName[BTL_BPPSND_MAX_FSPATH_LEN + 1];
	
		/* We do not yet support printing referenced objects that has relative path
		 * when the original object was memory based (thus doesn't have a base path).
		 * We check that by making sure we have a base path for our referenced object */
		BTL_VERIFY_ERR_SET_RETVAR(	printedFileFullName[0] != '\0',
										result = FALSE,
										("Referenced File Name (%s) - relative names not supported yet for memory object..",
										referencedFileName));
	
		/* file name is in the same folder of the printed file, just append the name to the printed file's folder */

		BTL_BPPSND_GetFolderNameFromFullName(printedFileFullName, printedFileFolderName);
		
		OS_StrCpy((char*)referencedFileFullName,(const char*)printedFileFolderName);

		BTL_VERIFY_ERR_SET_RETVAR(
			(OS_StrLen((const char*)referencedFileFullName) + OS_StrLen((const char*)referencedFileName) <= BTL_BPPSND_MAX_FSPATH_LEN),
			result = FALSE,
			("Combined Len of Printed File Folder Name (\"%s\") + Ref File Name (\"%s\") is TOO LONG", 
			referencedFileFullName, referencedFileName));
		
		OS_StrCat((char*)referencedFileFullName, (const char*)referencedFileName);
	}
	
	BTL_LOG_DEBUG(("referencedFileFullName: \"%s\"", referencedFileFullName));

	BTL_FUNC_END();
	
	return result;
}


void BTL_BPPSND_GetFolderNameFromFullName(const BtlUtf8* fullName, BtlUtf8* folderName)
{		
	/* Find the last folder separator ('/') in the file name */
	char* lastFolderSeparatorPos = OS_StrrChr((const char*)fullName, BTHAL_FS_PATH_DELIMITER);

	BTL_FUNC_START("BTL_BPPSND_GetFolderNameFromFullName");

	if (lastFolderSeparatorPos != 0)
	{
		/*  Calculate the length of the folder name */
		U32 folderNameLen = lastFolderSeparatorPos -(const char*)fullName + 1;

		/* Copy the folder name, including the trailing folder separator ('/') */
		OS_StrnCpy((char*)folderName, (const char*)fullName, folderNameLen);

		folderName[folderNameLen] = '\0';
	}
	else
	{
		/* No folder name separator found, return a null string */
		OS_StrCpy((char*)folderName, "");
	}

	BTL_LOG_DEBUG(("fullName: \"%s\", folderName: \"%s\"", fullName, folderName));

	BTL_FUNC_END();
}


BtStatus BTL_BPPSND_HandletGetRefObjectData(BppCallbackParms* parms)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BtlBppsndContext *bppsndContext = BTL_BPPSND_GetContextFromBppCallbackParms(parms);

	BTL_FUNC_START("BppHandletGetRefObjectData");

	/*
		The printer may request for any portion of the referenced object file. Therefore, we need to seek
		to the requested offset, so reading starts from there
	*/

	BTL_LOG_DEBUG(("Seeking to offset %d", parms->data->offset));

	status = BTL_BPPSND_SeekDocument(parms->data, bppsndContext);
	if (BT_STATUS_SUCCESS != status)
	{
		parms->data->len = 0;

		BTL_ERR(status, ("Failed to seek in the referenced object document (%s)", pBT_Status(status)));
	}
	
	status = BTL_BPPSND_ReadDocument(parms->data, bppsndContext);
	BTL_VERIFY_ERR(BT_STATUS_SUCCESS == status, status, ("Failed Reading Referenced Object File Data (%s)", pBT_Status(status)));

	BTL_FUNC_END();

	return status;
}

BtStatus BTL_BPPSND_HandletGetRefObjectDataCompleted(BtlBppsndContext *bppsndContext, BppCallbackParms* parms)
{
	BTL_BPPSND_CloseDocument(parms->data,bppsndContext);
	
	BTL_LOG_INFO(("Ref Obj Chnl Operation Complete status = %d", parms->status));

	return BT_STATUS_SUCCESS;
}

BtStatus Btl_Bppsnd_SendObexAbort(BtlBppsndContext *bppsndContext, BOOL userAbort)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("Btl_Bppsnd_SendObexAbort");

	BTL_VERIFY_FATAL(bppsndContext->abortState ==  BTL_BPPSND_ABORT_STATE_IDLE, BT_STATUS_INTERNAL_ERROR,
						("Attempt to abort while an abort is in progress"));
	
	if (userAbort == TRUE)
	{
		bppsndContext->abortState =  BTL_BPPSND_ABORT_STATE_USER_ABORT_IN_PROGRESS;
	}
	else
	{
		bppsndContext->abortState =  BTL_BPPSND_ABORT_STATE_INTERNAL_ABORT_IN_PROGRESS;
	}
	
	/* OBEX Abort */	
	BPP_Abort(&bppsndContext->session, BPPCH_SND_JOB);

	BTL_FUNC_END();
	
	return status;
}

void BTL_BPPSND_SendProgressNotificationToApp(BtlBppsndContext *bppsndContext, BppCallbackParms *parms)
{
	BTL_FUNC_START("BTL_BPPSND_SendProgressNotificationToApp");

	btlBppsndData.progressIndInfo.name = bppsndContext->fsPath;
	btlBppsndData.progressIndInfo.currPos = parms->data->offset;
	btlBppsndData.progressIndInfo.maxPos = parms->data->totLen;
	
	btlBppsndData.selfGeneratedCallbackParms = *parms;

	btlBppsndData.selfGeneratedCallbackParms.event = BPPCLIENT_PROGRESS;
	btlBppsndData.selfGeneratedCallbackParms.data->progressInfo = &btlBppsndData.progressIndInfo;

	BTL_BPPSND_SendEventToApp(bppsndContext, &btlBppsndData.selfGeneratedCallbackParms);
		
	BTL_FUNC_END();
}

void BTL_BPPSND_SendEventToApp(BtlBppsndContext *bppsndContext, BppCallbackParms *parms)
{
	BTL_FUNC_START("BTL_BPPSND_SendEventToApp");

	btlBppsndData.eventToApp.bppsndContext = bppsndContext;
	btlBppsndData.eventToApp.bppsndCallbackParms = parms;

	(bppsndContext->callback)(&btlBppsndData.eventToApp);
	
	BTL_FUNC_END();
}

BOOL BTL_BPPSND_IsConnected(BtlBppsndContext *bppsndContext)
{
	if (	(BTL_BPPSND_STATE_CONNECTED	== bppsndContext->state)	|| 
		(BTL_BPPSND_STATE_PRINTING		== bppsndContext->state)	||
		(BTL_BPPSND_STATE_SENT_REQUEST	== bppsndContext->state))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

static BOOL BTL_BPPSND_IsConnecting(BtlBppsndContext *bppsndContext)
{
	if ((BTL_BPPSND_STATE_CONNECTING == bppsndContext->state))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL Btl_Bppsnd_IsOperationOnMemoryObj(BppOp curOp, BtlBppsndContext *bppsndContext)
{
	switch ( curOp )
	{
		case BPPOP_SIMPLEPUSH:
		case BPPOP_GETREFOBJ:
		case BPPOP_SENDDOCUMENT:
		case BPPOP_SENDREFERENCE:
		case BPPOP_SIMPLEREFERENCE:
			return bppsndContext->printingMemory;

		case BPPOP_CREATEJOB:
		case BPPOP_GETJOBATTR:
		case BPPOP_GETPRTATTR:
		case BPPOP_CANCELJOB:
		case BPPOP_GETEVENT:
		case BPPOP_SOAP:
		case BPPOP_GETMARGINS:
		case BPPOP_CREATEPRECISE:	
			return bppsndContext->responseObj.objectLocation == BTL_OBJECT_LOCATION_MEM;
			
		default: 
			return FALSE;
			
	}
}

BtStatus BTL_BPPSND_OpenDocument(const BtlObject 	*objToPrint, BtlBppsndContext *bppsndContext, BOOL read)
{
	BtStatus status = BT_STATUS_SUCCESS;

	if ( objToPrint->objectLocation == BTL_OBJECT_LOCATION_FS )
		status = BTL_BPPSND_OpenFile(objToPrint->location.fsLocation.fsPath, &bppsndContext->sndData, read);
	else
		bppsndContext->sndData.ocx = (BtlUtf8*)objToPrint->location.memLocation.memAddr;

	return status;
}

BtStatus BTL_BPPSND_CloseDocument(BppData *data, BtlBppsndContext *bppsndContext)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_LOG_DEBUG(("BTL_BPPSND_CloseDocument(totLen=%d, offset=%d, len=%d, op=%s)", 
		data->totLen, data->offset, data->len, BTL_DEBUG_pBppOp(data->bppOp)));

	if ( FALSE  == Btl_Bppsnd_IsOperationOnMemoryObj(data->bppOp, bppsndContext ) )
		status = BTL_BPPSND_CloseFile(data);

	/* bppsndContext->fsPath[0] = '\0';*/

	return status;
}

BtStatus BTL_BPPSND_GetDocumentSize(const BtlObject 	*objToPrint, BtlBppsndContext *bppsndContext)
{
	BtStatus status = BT_STATUS_SUCCESS;

	if ( FALSE  == bppsndContext->printingMemory )
		status = BTL_BPPSND_GetFileSize(objToPrint->location.fsLocation.fsPath,  &(bppsndContext->sndData));
	else
		bppsndContext->sndData.totLen = objToPrint->location.memLocation.size;

	return status;
}

BtStatus BTL_BPPSND_ReadDocument(BppData *data, BtlBppsndContext *bppsndContext)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BTL_LOG_DEBUG(("BTL_BPPSND_ReadDocument(totLen=%d, offset=%d, len=%d, op=%s)", 
		data->totLen, data->offset, data->len, BTL_DEBUG_pBppOp(data->bppOp)));

	if ( TRUE  == Btl_Bppsnd_IsOperationOnMemoryObj(data->bppOp, bppsndContext ) )
		status = BTL_BPPSND_ReadMem(data);
	else
		status = BTL_BPPSND_ReadFile(data);
	
	return status;
}


BtStatus Btl_Bppsnd_WriteDocument(BppData *data, BtlBppsndContext *bppsndContext)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BTL_LOG_DEBUG(("Btl_Bppsnd_WriteDocument(totLen=%d, offset=%d, len=%d, op=%s)", 
		data->totLen, data->offset, data->len, BTL_DEBUG_pBppOp(data->bppOp)));

	if ( FALSE  == Btl_Bppsnd_IsOperationOnMemoryObj(data->bppOp, bppsndContext ) )
		status = Btl_Bppsnd_WriteFile(data);
	else
		status = Btl_Bppsnd_WriteMem(data);
	
	return status;
}

BtStatus BTL_BPPSND_SeekDocument(BppData *data, BtlBppsndContext *bppsndContext)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_LOG_DEBUG(("BTL_BPPSND_SeekDocument(totLen=%d, offset=%d, len=%d, op=%s)", 
		data->totLen, data->offset, data->len, BTL_DEBUG_pBppOp(data->bppOp)));

	if ( FALSE  == Btl_Bppsnd_IsOperationOnMemoryObj(data->bppOp, bppsndContext ) )
		status = BTL_BPPSND_SeekFile(data);
	/*else*/
	/*	no need to seek since seek is only used in referenced object,*/
	/*   and data->offset already has the correct offset when we are called to read.*/
	
	return status;
}

/*---------------------------------------------------------------------------
 *            BTL_BPPSND_OpenFile
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Wrapper for FFS Open File
 *
 * Return:    TRUE for success, FALSE otherwise
 *
 */
BtStatus BTL_BPPSND_OpenFile(const BtlUtf8* name, BppData *data, BOOL read)
{
	BtStatus 			status = BT_STATUS_SUCCESS;
	BthalFsFileDesc	fd;
	BtFsStatus 		fsStatus;
	BthalFsOpenFlags	flags = BTHAL_FS_O_BINARY |
							(read ? BTHAL_FS_O_RDONLY :
							BTHAL_FS_O_WRONLY|BTHAL_FS_O_CREATE|BTHAL_FS_O_TRUNC);

	BTL_FUNC_START("BTL_BPPSND_OpenFile");

	/* open file */
 	fsStatus = BTHAL_FS_Open(name, flags , &fd);

	if (BT_STATUS_HAL_FS_SUCCESS != fsStatus)
	{
		data->ocx = (BppOcx)BTHAL_FS_INVALID_FILE_DESC;
		
		BTL_ERR(BT_STATUS_FFS_ERROR, ("Failed Opening %s, Reason: %d", name, fsStatus));
	}
	else
	{
		data->ocx = (void*)fd;
	}
	BTL_LOG_DEBUG(("BTL_BPPSND_OpenFile(%s) returned fd=%d", name, (int) fd));

	BTL_FUNC_END();

	return status;
}


/*---------------------------------------------------------------------------
 *            BTL_BPPSND_CloseFile
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Wrapper for FFS Open File
 *
 * Return:    TRUE for success, FALSE otherwise
 *
 */
BtStatus BTL_BPPSND_CloseFile(BppData *data)
{
	BtStatus 			status = BT_STATUS_SUCCESS;
	BthalFsFileDesc	fd = (BthalFsFileDesc)data->ocx;
	
	BTL_FUNC_START("BTL_BPPSND_CloseFile");
	BTL_LOG_DEBUG(("BTL_BPPSND_CloseFile(fd=%d)", fd));

	if (BTHAL_FS_INVALID_FILE_DESC != fd)
	{		
		BtFsStatus fileCloseResult = BTHAL_FS_Close(fd);
		
		data->ocx = (BppOcx)BTHAL_FS_INVALID_FILE_DESC;

		BTL_VERIFY_ERR(BT_STATUS_HAL_FS_SUCCESS == fileCloseResult, BT_STATUS_FFS_ERROR, 
						("Failed closing File, Reason: %d", fileCloseResult));
	}
	
	BTL_FUNC_END();

	return status;
}

/*---------------------------------------------------------------------------
 *            BppGetFileSize
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Wrapper for FFS Get File Size
 *
 * Return:    TRUE for success, FALSE otherwise
 *
 */
BtStatus BTL_BPPSND_GetFileSize(const BtlUtf8 *name, BppData* data)
{
	BtStatus 			status = BT_STATUS_SUCCESS;
	BtFsStatus			fileStatResult;
	BthalFsStat 		fileStat;

	BTL_FUNC_START("BTL_BPPSND_GetFileSize");

	fileStatResult = BTHAL_FS_Stat(name, &fileStat);

	if (BT_STATUS_HAL_FS_SUCCESS != fileStatResult)
	{
		data->totLen = 0;

		BTL_ERR(BT_STATUS_FFS_ERROR, ("BTHAL_FS_Stat Failed, Reason: %d", fileStatResult));
	}
	else
	{
		/* get file size from meta-data */
		data->totLen = fileStat.size;
	}

	BTL_FUNC_END();

	return status;
}

/*---------------------------------------------------------------------------
 *            BppReadFile
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Wrapper for FFS Read File
 *
 * Return:    TRUE for success, FALSE otherwise
 *
 */
BtStatus BTL_BPPSND_ReadFile(BppData *data)
{
	BtStatus 			status = BT_STATUS_SUCCESS;
	BthalFsFileDesc	fd = (BthalFsFileDesc)data->ocx;
	BtFsStatus		fileReadResult;
	BTHAL_U32		numRead = 0;

	BTL_FUNC_START("BTL_BPPSND_ReadFile");

	fileReadResult = BTHAL_FS_Read (fd, data->buff, (int)data->len, &numRead);
	BTL_LOG_DEBUG(("BTL_BPPSND_ReadFile(fd=%d, len=%d)=%d", fd,(int)data->len, numRead));

	if (BT_STATUS_HAL_FS_SUCCESS != fileReadResult)
	{
		data->len = 0;

		BTL_ERR(BT_STATUS_FFS_ERROR, ("Failed reading from file, Reason: %d", fileReadResult));
	}
	else
	{
		/* Reading was successful, read result indicates the actual number of bytes read */
		data->len = numRead;
	}

	BTL_FUNC_END();

	return status;
}

static BtStatus Btl_Bppsnd_WriteFile(BppData *data)
{
	BtStatus 			status = BT_STATUS_SUCCESS;
	BthalFsFileDesc	fd = (BthalFsFileDesc)data->ocx;
	BtFsStatus		fileWriteResult;
	BTHAL_U32		numWritten = 0;

	BTL_FUNC_START("Btl_Bppsnd_WriteFile");

	fileWriteResult = BTHAL_FS_Write (fd, data->buff, (int)data->len, &numWritten);
	BTL_LOG_DEBUG(("Btl_Bppsnd_WriteFile(fd=%d, len=%d)=%d", fd, (int)data->len, numWritten));

	if (BT_STATUS_HAL_FS_SUCCESS != fileWriteResult)
	{
		data->len = 0;

		BTL_ERR(BT_STATUS_FFS_ERROR, ("Failed writing to file, Reason: %d", fileWriteResult));
	}
	else
	{
		/* writing was successful, write result indicates the actual number of bytes written */
		data->len = numWritten;
	}

	BTL_FUNC_END();

	return status;
}


/*---------------------------------------------------------------------------
 *            BppReadMem
 *---------------------------------------------------------------------------
 *
 * Synopsis:  reading memory objects
 *
 * Note: probably should be implemented elsewhere (osapi or hal for example)
 *
 * Return:    TRUE for success, FALSE otherwise
 *
 */
static BtStatus BTL_BPPSND_ReadMem(BppData *data)
{
	BtStatus 		status = BT_STATUS_SUCCESS;
	U32				howMuch = 0;
	U32				leftToRead = 0;

	BTL_FUNC_START("BTL_BPPSND_ReadMem");

	leftToRead = data->totLen - data->offset;
	howMuch = (data->len <= leftToRead)?data->len:leftToRead;
	BTL_LOG_DEBUG(("BTL_BPPSND_ReadMem(totLen=%d, offset=%d, len=%d, howMuch=%d)", 
	data->totLen, data->offset, data->len, howMuch));

	OS_MemCopy(data->buff, ((const U8 *)data->ocx)+data->offset, howMuch);

	data->len = howMuch;

	BTL_FUNC_END();

	return status;
}

/*---------------------------------------------------------------------------
 *            Btl_Bppsnd_WriteMem
 *---------------------------------------------------------------------------
 *
 * Synopsis: writing memory objects
 *
 * Return:    TRUE for success, FALSE otherwise
 *
 */
BtStatus Btl_Bppsnd_WriteMem(BppData *data)
{
	BtStatus 			status = BT_STATUS_SUCCESS;
	char				*memAddr = (char *)data->ocx;
	U32				howMuch = 0;
	/*U32				leftToWrite;*/

	BTL_FUNC_START("Btl_Bppsnd_WriteMem");

	/*leftToWrite = data->totLen - data->offset;*/
	howMuch = data->len; /*(data->len <= leftToWrite) ? data->len : leftToWrite;*/
	BTL_LOG_DEBUG(("Btl_Bppsnd_WriteMem(totLen=%d, offset=%d, len=%d, howMuch=%d)", 
		data->totLen, data->offset, data->len, howMuch));

	OS_MemCopy((U8*)memAddr+data->offset, data->buff, howMuch);

	data->len = howMuch;

	BTL_FUNC_END();

	return status;

}

	
/*---------------------------------------------------------------------------
 *            BppSeekFile
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Wrapper for FFS Read File
 *
 * Return:    TRUE for success, FALSE otherwise
 *
 */
BtStatus BTL_BPPSND_SeekFile(BppData *data)
{
	BtStatus 			status = BT_STATUS_SUCCESS;
	BthalFsFileDesc	fd = (BthalFsFileDesc)data->ocx;
	BtFsStatus		fileSeekResult;

	BTL_FUNC_START("BTL_BPPSND_SeekFile");

	fileSeekResult = BTHAL_FS_Seek(fd, data->offset, BTHAL_FS_START);
	BTL_LOG_DEBUG(("BTL_BPPSND_SeekFile(fd=%d, offset=%d)", fd, data->offset));
	
	BTL_VERIFY_ERR(BT_STATUS_HAL_FS_SUCCESS == fileSeekResult, BT_STATUS_FFS_ERROR, 
					("Failed Seeking in file to %d, Reason: %d", data->offset, fileSeekResult));

	BTL_FUNC_END();

	return status;
}

#if 0
const char* BTL_BPPSND_pInitState(BtlBppsndInitState state)
{
	switch (state)
	{
		case BTL_BPPSND_INIT_STATE_NOT_INITIALIZED: return "BTL_BPPSND_INIT_STATE_NOT_INITIALIZED";
		case BTL_BPPSND_INIT_STATE_INITIALIZED: return "BTL_BPPSND_INIT_STATE_INITIALIZED";
		case BTL_BPPSND_INIT_STATE_INITIALIZATION_FAILED: return "BTL_BPPSND_INIT_STATE_INITIALIZATION_FAILED";
		default: return "UNKNOWN";
	};
} 
#endif

const char* BTL_BPPSND_pState(BtlBppsndState state)
{
	switch (state)
	{
		case BTL_BPPSND_STATE_IDLE: return "BTL_BPPSND_STATE_IDLE";
		case BTL_BPPSND_STATE_DISCONNECTED: return "BTL_BPPSND_STATE_DISCONNECTED";
		case BTL_BPPSND_STATE_CONNECTED: return "BTL_BPPSND_STATE_CONNECTED";
		case BTL_BPPSND_STATE_DISCONNECTING: return "BTL_BPPSND_STATE_DISCONNECTING";
		case BTL_BPPSND_STATE_CONNECTING: return "BTL_BPPSND_STATE_CONNECTING";
		case BTL_BPPSND_STATE_PRINTING: return "BTL_BPPSND_STATE_PRINTING";
		case BTL_BPPSND_STATE_SENT_REQUEST: return "BTL_BPPSND_STATE_SENT_REQUEST";
		default: return "UNKNOWN";
	};
}

#if 0
const char* BTL_BPPSND_pDisablingState(BtlBppsndDisablingState state)
{
	switch (state)
	{
		case BTL_BPPSND_DISABLING_STATE_IDLE: return "IDLE";
	 	case BTL_BPPSND_DISABLING_STATE_PENDING: return "PENDING";
 	 	case BTL_BPPSND_DISABLING_STATE_IN_PROGRESS: return "IN PROGRESS";
		default: return "UNKNOWN";
	};
}
#endif

#if OBEX_AUTHENTICATION == XA_ENABLED
BtStatus BTL_BPPSND_ObexAuthenticationResponse(	BtlBppsndContext 	*bppsndContext, 
														const BtlUtf8 			*userId, 
														const BtlUtf8 			*password)
{
 	BppAuthInfo	authInfo;
	const U8	nullString[1] = {0};
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK("BTL_BPPSND_ObexAuthenticationResponse");

	BTL_VERIFY_ERR((0 != bppsndContext), BT_STATUS_INVALID_PARM, ("Null bppsndContext"));
	BTL_VERIFY_ERR((0 != password), BT_STATUS_INVALID_PARM, ("Null password"));
	BTL_VERIFY_ERR((bppsndContext->obexAuthFlags & OBEX_AUTH_RECEIVED_CHAL), BT_STATUS_FAILED, ("No OBEX auth challenge was received"));

	authInfo.password = password;
	authInfo.passwordLen = OS_StrLen((const char*)password);

	if ( NULL != userId )
	{
		authInfo.userId = userId;
		authInfo.userIdLen = OS_StrLen((const char*)userId) ;
	}
	else /* empty / NULL userId is legal*/
	{
		authInfo.userId = nullString;
		authInfo.userIdLen = 0;
	}

	authInfo.realm = nullString;
	authInfo.realmLen = 0;
	
	bppsndContext->obexAuthFlags &= (~OBEX_AUTH_RECEIVED_CHAL);

	status = BPP_ClientSetAuthInfo(&bppsndContext->session.sndJob, &authInfo);
	BTL_VERIFY_ERR((status == OB_STATUS_SUCCESS), status, ("BPP_ClientSetAuthInfo failed"));
	BTL_LOG_DEBUG(("BPP_ClientSetAuthInfo success"));

	bppsndContext->obexAuthFlags  |= OBEX_AUTH_INFO_SET;
	/* BPP_ClientSetAuthInfo takes care of checking that string lengths don't exeed allowed maximum */
	if ( bppsndContext->obexAuthFlags & OBEX_AUTH_DURING_CALLBACK )
	{
	}
	else
	/*if ( (BT_STATUS_SUCCESS == status) && !(bppsndContext->obexAuthFlags & OBEX_AUTH_DURING_CALLBACK) )*/
	{
		status = BPP_Connect(&bppsndContext->session, &bppsndContext->printerAddress, BPPSVC_DPS);
		if ( BT_STATUS_PENDING == status )
		{
			bppsndContext->state = BTL_BPPSND_STATE_CONNECTING;
			status = BT_STATUS_SUCCESS;
		}

#if BPP_PRINTING_STATUS == XA_ENABLED

		else if ( BT_STATUS_SUCCESS == status )
		{
			ObStatus obStatus = BPP_Connect(&bppsndContext->session, &bppsndContext->printerAddress, BPPSVC_STS);

			if (OB_STATUS_PENDING == obStatus)
			{
				BTL_LOG_INFO(("Waiting for connection to establish Status channel"));
			}
			else if (OB_STATUS_SUCCESS == status)
			{
				BTL_LOG_INFO(("Connected To Printer Status channel"));		
			}
			else
			{
				BTL_LOG_ERROR(("Failed connecting To Printer Status channel"));
			}
		}

#endif	/* BPP_PRINTING_STATUS == XA_ENABLE */

	}
	/*BPP_ServerContinue(&bppsndContext->session.sndObject);//	BppObexServer*/
	
	BTL_FUNC_END_AND_UNLOCK();
	
	return 	status;
}
#endif

#if BT_SECURITY == XA_ENABLED
BtStatus BTL_BPPSND_SetSecurityLevel(BtlBppsndContext *bppsndContext,
								  		const BtSecurityLevel *securityLevel)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BPPSND_SetSecurityLevel");

	BTL_VERIFY_ERR((0 != bppsndContext), BT_STATUS_INVALID_PARM, ("Null bppsndContext"));

	if (0 != securityLevel)
	{
		status = BTL_VerifySecurityLevel(*securityLevel);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Invalid BPP securityLevel"));
	}

	/* Apply the new security level */
	bppsndContext->session.sndJob.cApp.secRecord.level = (BtSecurityLevel)((securityLevel == 0) ? (BSL_BPPSND_DEFAULT) : (*securityLevel));

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

BtStatus BTL_BPPSND_GetSecurityLevel(	BtlBppsndContext 	*bppsndContext,
								  			BtSecurityLevel 	*securityLevel)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK("BTL_BPPSND_GetSecurityLevel");

	BTL_VERIFY_ERR((0 != bppsndContext), BT_STATUS_INVALID_PARM, ("Null bppsndContext"));
	BTL_VERIFY_ERR((0 != securityLevel), BT_STATUS_INVALID_PARM, ("Null securityLevel"));

	*securityLevel = bppsndContext->session.sndJob.cApp.secRecord.level;
	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}
#endif	/* BT_SECURITY == XA_ENABLED */


/*		 J O B     B A S E D    C O M M A N D S*/
/*		=========================*/


BtStatus BTL_BPPSND_SendDocument(	BtlBppsndContext 	*bppsndContext,
											BtlBppsndJobId	jobId,
											const BtlObject 	*objToPrint)
{
	BtStatus		status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK("BTL_BPPSND_SendDocument");
	
	status = Btl_Bppsnd_PushOrSendDocument(bppsndContext, objToPrint, BPPOP_SENDDOCUMENT, jobId, "SendDocument");
	
	BTL_FUNC_END_AND_UNLOCK();
	
	return status;

}

 /* 
 * 	Following two functions are helper routines which hold the commnon code to all SOAP request API calls
 *	
 */

BtStatus Btl_bppsd_CheckObjectValidity(const BtlObject 	*btlObj)
 {
 	BtStatus		status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START("Btl_bppsd_CheckObjectValidity");
	BTL_VERIFY_ERR((0 != btlObj), BT_STATUS_INVALID_PARM, ("Null btlObj"));
	BTL_VERIFY_ERR(0 != btlObj->objectName, BT_STATUS_INVALID_PARM, ("Null objectName"));
	BTL_VERIFY_ERR(0 != btlObj->objectMimeType, BT_STATUS_INVALID_PARM, ("Null objectMimeType"));
	BTL_VERIFY_ERR(OS_StrLen((const char*)btlObj->objectMimeType) <= BTL_BPPSND_MAX_MIME_TYPE_LEN,
					BT_STATUS_INVALID_PARM, 
					("MIME type too long (%d), %d is the limit", 
					OS_StrLen((const char*)btlObj->objectMimeType), BTL_BPPSND_MAX_MIME_TYPE_LEN));
	BTL_VERIFY_ERR(OS_StrLen((const char*)btlObj->objectName) <= GOEP_MAX_UNICODE_LEN,
					BT_STATUS_INVALID_PARM, 
					("objectName too long (%d), %d is the limit", 
					OS_StrLen((const char*)btlObj->objectName), GOEP_MAX_UNICODE_LEN));

	switch ( btlObj->objectLocation )
	{
		case BTL_OBJECT_LOCATION_FS:
			/*if object location is on fs then its path must not be null*/
			BTL_VERIFY_ERR((0 != btlObj->location.fsLocation.fsPath), BT_STATUS_INVALID_PARM,
							("Null file system path to object location"));
			/*if object location is on fs then its path len must not be too long*/
			BTL_VERIFY_ERR((OS_StrLen((const char*)btlObj->location.fsLocation.fsPath) <= BTL_BPPSND_MAX_FSPATH_LEN),
							BT_STATUS_INVALID_PARM, 
							(("File System Path too long (%d), %d is the limit"), 
							OS_StrLen((const char*)btlObj->location.fsLocation.fsPath), BTL_BPPSND_MAX_FSPATH_LEN));
			break;

		case  BTL_OBJECT_LOCATION_MEM:
			/*if object location is on mem then its address must not be null.*/
			/*should be changed if target architecture might allow user data to reside on address 0x0000*/
			BTL_VERIFY_ERR((0 != btlObj->location.memLocation.memAddr)
							, BT_STATUS_INVALID_PARM, 
							("Null memory address object given"));
			BTL_VERIFY_ERR((0 < btlObj->location.memLocation.size),
							BT_STATUS_INVALID_PARM, 
							("Size of given memory object should be larger than zero (%d given)", 
							btlObj->location.memLocation.size));
			break;

		default:
			BTL_ERR(BT_STATUS_INVALID_PARM, ("Object location not supported"));
				
	}

	BTL_FUNC_END();
	
	return status;
	
}
 
BtStatus Btl_bppsd_StartSendingSoapRequest(
			BtlBppsndContext 				*bppsndContext,
			const BtlObject 				*responseObj,
			BtlObject 					*requestObj,
			char 						*callerFuncName)
{
	BtStatus		status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START(callerFuncName);

	BTL_VERIFY_ERR((0 != bppsndContext), BT_STATUS_INVALID_PARM, ("Null bppsndContext"));
	BTL_VERIFY_ERR(((BTL_BPPSND_STATE_CONNECTED == bppsndContext->state) || 
					(BTL_BPPSND_STATE_PRINTING == bppsndContext->state)), 
					BT_STATUS_FAILED, 
					("Must be in Connected state or Printing state to send SOAP request"));
	BTL_VERIFY_ERR(callerFuncName != 0, BT_STATUS_INVALID_PARM, ("Null callerFuncName"));

	status = Btl_bppsd_CheckObjectValidity(responseObj);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), BT_STATUS_INVALID_PARM, ("invalid response object"));

	BTL_BPPSND_InitOperationVariables(bppsndContext);
	
	OS_MemCopy( (U8*) &bppsndContext->responseObj,  (U8*) responseObj, sizeof(BtlObject) );
	requestObj->objectLocation = BTL_OBJECT_LOCATION_MEM;
	requestObj->location.memLocation.size = MAX_REQ_MEM_OBJ;
	requestObj->location.memLocation.memAddr = (char*)soapReqMem;
	soapReqMem[0] = 0;

	BTL_FUNC_END();
	
	return status;
}

BtStatus Btl_bppsd_FinishSendingSoapRequest(
			BtlBppsndContext 				*bppsndContext,
			const BtlObject 				*responseObj,
			U32							jobId,
			BppOp						bppOp,
			U8							*soapBuf,
			U16 							totSize,
			char 						*callerFuncName)
{
	BtStatus		status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START(callerFuncName);

	status = BTL_BPPSND_OpenDocument(responseObj, bppsndContext, FALSE); /* open for write	*/

	BTL_VERIFY_ERR(BT_STATUS_SUCCESS == status, status, ("Failed opening document (%s)", pBT_Status(status)));
	BTL_VERIFY_ERR(callerFuncName != 0, BT_STATUS_INVALID_PARM, ("Null callerFuncName"));

	bppsndContext->sndData.jobId = jobId;
	bppsndContext->sndData.bppOp = bppOp;
	bppsndContext->sndData.totLen = totSize;
	bppsndContext->sndData.buff = soapBuf;
	bppsndContext->sndData.len = 0;
	bppsndContext->sndData.offset = 0;

 	status = BPP_SendRequest(&bppsndContext->session, &bppsndContext->sndData);

	BTL_LOG_DEBUG(("BPP_SendRequest returned %s (%d)", BTL_DEBUG_pObStatus(status) ,  status));
	BTL_VERIFY_ERR((OB_STATUS_SUCCESS == status) || (OB_STATUS_PENDING == status), 
		BT_STATUS_INVALID_PARM, ("sending SOAP request failed"));

	bppsndContext->state = BTL_BPPSND_STATE_SENT_REQUEST;

	BTL_FUNC_END();

	return status;

}


U32 os_StrNICmp(const BtlUtf8 *Str1, const BtlUtf8 *Str2, U32 n)
{
	U32 idx;
	
	for (idx = 0; idx < n; ++idx) {
		if (ToUpper(Str1[idx]) != ToUpper(Str2[idx])) {
			return 1;
		}
		/* The first idx characters are already equal,
		   so test only if Str1's end has been reached. */
		if (Str1[idx] == '\0') {
			return 0;
		}
	}
	return 0;
}

/**/
/* parse a SOAP request message to find the requested operation and the jobID */
/* jobID is searched and filled only for operations that require a jobId*/
/* return TRUE if succeeded*/
/**/

BOOL Btl_bppsd_ParseSoapRequest(U8 *soapBuf, U16 bufSize, BppOp *bppOp, BtlBppsndJobId *jobId)
{
	int i, j, k;
	BOOL status = TRUE;
	BtlUtf8 *startOp;
	int opLen;
	static char getPrinterAttributesStr[]	= "GetPrinterAttributes";
	static char createJob[]				= "CreateJob";
	static char cancelJobStr[]			= "CancelJob";
	static char getJobAttributesStr[]	= "GetJobAttributes";
	static char getEventStr[]			= "GetEvent";
	static char createPreciseJobStr[]	= "CreatePreciseJob";
	static char getMarginsStr[]			= "GetMargins";
	static BppOp op[] = {
		BPPOP_GETPRTATTR, BPPOP_CREATEJOB, BPPOP_CANCELJOB, BPPOP_GETJOBATTR,
		BPPOP_GETEVENT, BPPOP_CREATEPRECISE, BPPOP_GETMARGINS };
	static BtlUtf8 *opStr[] = {
		(BtlUtf8*)getPrinterAttributesStr, (BtlUtf8*)createJob, (BtlUtf8*)cancelJobStr, (BtlUtf8*)getJobAttributesStr, 
		(BtlUtf8*)getEventStr, (BtlUtf8*)createPreciseJobStr, (BtlUtf8*)getMarginsStr };
	static BOOL jobidRequired[] = { FALSE, FALSE, TRUE, TRUE, TRUE, FALSE, FALSE };

	/* find the BPP operation first*/
	BTL_FUNC_START("Btl_bppsd_ParseSoapRequest");
	for ( i = 0 ; i <  bufSize ; i++ )
	{
		if ( soapBuf[i] == '#' )
			break;
	}
	BTL_VERIFY_ERR((soapBuf[i] == '#'), FALSE, ("'#' not found in SOAP request"));

	for ( j = i + 1 ; j <  bufSize ; j++ )
	{
		if ( soapBuf[j] == '\"' )
			break;
	}
	BTL_VERIFY_ERR((soapBuf[j] == '\"'), FALSE, ("'\"' not found in SOAP request"));

	opLen = j - i - 1;
	startOp = &soapBuf[i+1];

	for ( k = 0 ; k < sizeof(op) / sizeof(BppOp) ; k++ )
	{
		if ( os_StrNICmp(startOp, opStr[k], opLen) == 0 )
		{
			*bppOp = op[k];	/* success to find the operation !*/
			BTL_LOG_DEBUG(("found BPP operation = '%s'", opStr[k]));
			break;
		}
	}
	BTL_VERIFY_ERR((k < sizeof(op) / sizeof(BppOp) ), FALSE, ("legal BPP operation not found in SOAP request"));

	if ( jobidRequired[k] )
	{
		/* now try to find the jobId*/
		for ( i = j + 1 ; i <  bufSize ; i++ )
		{
			if ( ( soapBuf[i] == '<' ) && (os_StrNICmp( (BtlUtf8*)&soapBuf[i], (BtlUtf8*)("<JobId>"), 7) == 0 ) )
				break;
		}
		BTL_VERIFY_ERR((i <  bufSize), FALSE, ("<JobId> not found in SOAP request"));

		*jobId = OS_AtoU32((const char*)&soapBuf[i+7]);
		BTL_LOG_DEBUG(("found jobId = %d", *jobId));
	}

	BTL_FUNC_END();
	return status;

}

BtStatus BTL_BPPSND_SendSoapRequest(	BtlBppsndContext 	*bppsndContext,
												const BtlObject 	*requestObj,
												const BtlObject	*responseObj)
{
	BtStatus		status = BT_STATUS_SUCCESS;
	BtlObject 	dummyRequestObj;
	BtlBppsndJobId	jobId;
	U16 			totSize;
	BppOp		bppOp;
	U8			*soapBuf;
	
	BTL_FUNC_START_AND_LOCK("BTL_BPPSND_SendSoapRequest");
	
	/*BTL_FATAL_SET_RETVAR(status = BT_STATUS_FAILED, ("BTL_BPPSND_SendSoapRequest not supported yet"));*/
		
	status = Btl_bppsd_StartSendingSoapRequest(
		bppsndContext, responseObj, 	&dummyRequestObj, "\"StartSendSoapRequest\"" );
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), 	status, ("failed on StartSendSoapRequest"));

	/* check the request object*/
	status = Btl_bppsd_CheckObjectValidity(requestObj);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), 	status, ("invalid request object"));

	if ( BTL_OBJECT_LOCATION_FS == requestObj->objectLocation )
	{
		/* if the object is in file - read into memory*/
		BthalFsFileDesc fd;
		BTHAL_U32 numRead = 0;
		BtFsStatus fsStatus = BTHAL_FS_Open(
			requestObj->location.fsLocation.fsPath , BTHAL_FS_O_RDONLY |BTHAL_FS_O_BINARY, &fd);
		BTL_VERIFY_ERR((BT_STATUS_HAL_FS_SUCCESS == fsStatus), 	BT_STATUS_FFS_ERROR, ("failed to open SOAP request file"));
		fsStatus = BTHAL_FS_Read(fd, soapReqMem, sizeof(soapReqMem) -1, &numRead);
		BTHAL_FS_Close(fd);
		BTL_VERIFY_ERR((numRead > 0),  BT_STATUS_FFS_ERROR, ("failed to read from SOAP request file"));
		soapReqMem[numRead] = 0;
		totSize = (U16)numRead;
		soapBuf = soapReqMem;	
	}
	else
	{
		totSize = (U16) requestObj->location.memLocation.size;
		soapBuf = (U8	 *) requestObj->location.memLocation.memAddr;
	}

	BTL_VERIFY_ERR( Btl_bppsd_ParseSoapRequest(soapBuf, totSize, &bppOp, &jobId),  
		BT_STATUS_FAILED, ("failed to parse SOAP request for operation and jobId"));

	BTL_VERIFY_ERR((BPPOP_GETEVENT != bppOp), 	BT_STATUS_FAILED, ("failed because GetEvent not supported"));
	
	status = Btl_bppsd_FinishSendingSoapRequest(
			bppsndContext,
			responseObj,
			jobId,
			bppOp,
			soapBuf,
			totSize,
			"\"FinishSendSoapRequest\"" );
	BTL_VERIFY_ERR((BT_STATUS_PENDING == status), 	BT_STATUS_FAILED, ("failed on FinishSendSoapRequest"));
	
	BTL_FUNC_END_AND_UNLOCK();
	
	return status;
}

BtStatus BTL_BPPSND_GetPrinterAttributes(
			BtlBppsndContext 				*bppsndContext,
			BtlBppsndPrinterAttributesMask	attributesMask,											
			const BtlObject 				*responseObj)
{
	BtStatus		status = BT_STATUS_SUCCESS;
	BtlObject 	requestObj;
	U16 			totSize = 0;
	
	BTL_FUNC_START_AND_LOCK("BTL_BPPSND_GetPrinterAttributes");

	status = Btl_bppsd_StartSendingSoapRequest(
		bppsndContext, responseObj, 	&requestObj, "\"StartGetPrinterAttributes\"" );
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), 	BT_STATUS_FAILED, ("failed on StartGetPrinterAttributes"));

	status = Btl_BppSnd_BuildGetPrinterAttributes(attributesMask, &requestObj, &totSize);

	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), 	BT_STATUS_FAILED, ("failed to build SOAP request"));
	
	status = Btl_bppsd_FinishSendingSoapRequest(
			bppsndContext,
			responseObj,
			0, /* 	jobId,*/
			BPPOP_GETPRTATTR,
			soapReqMem,
			totSize,
			"\"FinishGetPrinterAttributes\"" );
	BTL_VERIFY_ERR((BT_STATUS_PENDING == status), 	BT_STATUS_FAILED, ("failed on FinishGetPrinterAttributes"));

	BTL_FUNC_END_AND_UNLOCK();
	
	return status;

}

BtStatus BTL_BPPSND_ParseGetPrinterAttributesResponse(
			BtlBppsndContext 				*bppsndContext, 
			const BtlObject 				*responseObj, 
			BtlBppsndPrinterAttributes		*attributesData,
			BtlBppsndPrinterAttributesMask	*attributesMask)
{
	BtStatus		status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK("BTL_BPPSND_ParseGetPrinterAttributesResponse");

	BTL_VERIFY_ERR((0 != bppsndContext), BT_STATUS_INVALID_PARM, ("Null bppsndContext"));
	BTL_VERIFY_ERR((0 != responseObj), BT_STATUS_INVALID_PARM, ("Null responseObj"));
	BTL_VERIFY_ERR(0 != attributesData, BT_STATUS_INVALID_PARM, ("Null attributesData"));
	BTL_VERIFY_ERR(0 != attributesMask, BT_STATUS_INVALID_PARM, ("Null attributesMask"));

	status = Btl_BppSnd_ParseGetPrinterAttributesResponse(
							responseObj,
							attributesData,
							BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_ALL,
							attributesMask );

	BTL_FUNC_END_AND_UNLOCK();

	return status;

}


BtStatus BTL_BPPSND_CreateJob(BtlBppsndContext 						*bppsndContext,
									const BtlBppsndCreateJobRequestData		*attributesData,
									const BtlBppsndCreateJobAttributesMask	*attributesMask,
									const BtlObject 						*responseObj)
{
	BtStatus		status = BT_STATUS_SUCCESS;
	BtlObject 	requestObj;
	U16 			totSize = 0;
	
	BTL_FUNC_START_AND_LOCK("BTL_BPPSND_CreateJob");

	status = Btl_bppsd_StartSendingSoapRequest(
		bppsndContext, responseObj, 	&requestObj, "\"StartCreateJob\"" );
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), 	BT_STATUS_FAILED, ("failed on StartCreateJob"));

	status = Btl_BppSnd_BuildCreateJob(attributesData, *attributesMask, &requestObj, &totSize);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), 	BT_STATUS_FAILED, ("failed to build SOAP request"));

	status = Btl_bppsd_FinishSendingSoapRequest(
			bppsndContext,
			responseObj,
			0, /* 	jobId,*/
			BPPOP_CREATEJOB,
			soapReqMem,
			totSize,
			"\"FinishCreateJob\"" );
	BTL_VERIFY_ERR((BT_STATUS_PENDING == status), 	BT_STATUS_FAILED, ("failed on FinishCreateJob"));

	bppsndContext->jobId = 0;
	
	BTL_FUNC_END_AND_UNLOCK();
	
	return status;

}

BtStatus BTL_BPPSND_CancelJob(	BtlBppsndContext 	*bppsndContext,
										BtlBppsndJobId	jobId,
										const BtlObject 	*responseObj)
{
	BtStatus		status = BT_STATUS_SUCCESS;
	BtlObject 	requestObj;
	U16 			totSize = 0;
	
	BTL_FUNC_START_AND_LOCK("BTL_BPPSND_CancelJob");

	status = Btl_bppsd_StartSendingSoapRequest(
		bppsndContext, responseObj, 	&requestObj, "\"StartCancelJob\"" );
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), 	BT_STATUS_FAILED, ("failed on StartCancelJob"));

	BTL_VERIFY_ERR((bppsndContext->jobId == jobId),  BT_STATUS_INVALID_PARM, 
		("invalid jobId %d (our current is %d)", jobId, bppsndContext->jobId ));

	status = Btl_BppSnd_BuildCancelJob(jobId, &requestObj, &totSize);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), 	BT_STATUS_FAILED, ("failed to build SOAP request"));

	status = Btl_bppsd_FinishSendingSoapRequest(
			bppsndContext,
			responseObj,
			jobId,
			BPPOP_CANCELJOB,
			soapReqMem,
			totSize,
			"\"FinishCancelJob\"" );
	BTL_VERIFY_ERR((BT_STATUS_PENDING == status), 	BT_STATUS_FAILED, ("failed on FinishCancelJob"));

	BTL_FUNC_END_AND_UNLOCK();
	
	return status;

}

BtStatus BTL_BPPSND_GetJobAttributes(	BtlBppsndContext 					*bppsndContext,
											BtlBppsndJobId					jobId,
											const BtlBppsndGetJobAttributesMask	*attributesMask,
											const BtlObject 					*responseObj)
{
	BtStatus		status = BT_STATUS_SUCCESS;
	BtlObject 	requestObj;
	U16 			totSize = 0;
	
	BTL_FUNC_START_AND_LOCK("BTL_BPPSND_GetJobAttributes");

	BTL_VERIFY_ERR((0 != jobId),  BT_STATUS_INVALID_PARM, ("jobId must not be zero"));

	status = Btl_bppsd_StartSendingSoapRequest(
		bppsndContext, responseObj, 	&requestObj, "\"StartGetJobAttributes\"" );
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), 	BT_STATUS_FAILED, ("failed on StartGetJobAttributes"));

	BTL_VERIFY_ERR((bppsndContext->jobId == jobId),  BT_STATUS_INVALID_PARM, 
		("invalid jobId %d (our current is %d)", jobId, bppsndContext->jobId ));

	status = Btl_BppSnd_BuildGetJobAttributes(jobId, *attributesMask, &requestObj, &totSize);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), 	BT_STATUS_FAILED, ("failed to build SOAP request"));

	status = Btl_bppsd_FinishSendingSoapRequest(
			bppsndContext,
			responseObj,
			jobId,
			BPPOP_GETJOBATTR,
			soapReqMem,
			totSize,
			"\"FinishGetJobAttributes\"" );
	BTL_VERIFY_ERR((BT_STATUS_PENDING == status), 	BT_STATUS_FAILED, ("failed on FinishGetJobAttributes"));

	BTL_FUNC_END_AND_UNLOCK();
	
	return status;

}

BtStatus BTL_BPPSND_ParseCreateJobResponse(
			BtlBppsndContext 					*bppsndContext,
			const BtlObject 					*responseObj,
			BtlBppsndCreateJobResponseData	*attributesData)
{
	BtStatus		status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK("BTL_BPPSND_ParseCreateJobResponse");

	BTL_VERIFY_ERR((0 != bppsndContext), BT_STATUS_INVALID_PARM, ("Null bppsndContext"));
	BTL_VERIFY_ERR((0 != responseObj), BT_STATUS_INVALID_PARM, ("Null responseObj"));
	BTL_VERIFY_ERR(0 != attributesData, BT_STATUS_INVALID_PARM, ("Null attributesData"));

	status = Btl_BppSnd_ParseCreateJobResponse(
							responseObj, attributesData);

	BTL_VERIFY_ERR((0 != attributesData->jobId), BT_STATUS_INVALID_PARM, ("jobId 0 was returned"));

	bppsndContext->jobId = attributesData->jobId;
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;

}

BtStatus BTL_BPPSND_ParseCancelJobResponse(
			BtlBppsndContext 					*bppsndContext,
			const BtlObject 					*responseObj,
			BtlBppsndCancelJobResponseData	*attributesData)
{
	BtStatus		status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK("BTL_BPPSND_ParseCancelJobResponse");

	BTL_VERIFY_ERR((0 != bppsndContext), BT_STATUS_INVALID_PARM, ("Null bppsndContext"));
	BTL_VERIFY_ERR((0 != responseObj), BT_STATUS_INVALID_PARM, ("Null responseObj"));
	BTL_VERIFY_ERR(0 != attributesData, BT_STATUS_INVALID_PARM, ("Null attributesData"));

	status = Btl_BppSnd_ParseCancelJobResponse(
							responseObj, attributesData);

	bppsndContext->jobId = 0;

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_BPPSND_ParseGetJobAttributesResponse(
			BtlBppsndContext 						*bppsndContext, 
			const BtlObject 						*responseObj, 
			BtlBppsndGetJobAttributesResponseData	*attributesData,
			BtlBppsndGetJobAttributesMask			*attributesMask)
{
	BtStatus		status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK("BTL_BPPSND_ParseGetJobAttributesResponse");

	BTL_VERIFY_ERR((0 != bppsndContext), BT_STATUS_INVALID_PARM, ("Null bppsndContext"));
	BTL_VERIFY_ERR((0 != responseObj), BT_STATUS_INVALID_PARM, ("Null responseObj"));
	BTL_VERIFY_ERR(0 != attributesData, BT_STATUS_INVALID_PARM, ("Null attributesData"));
	BTL_VERIFY_ERR(0 != attributesMask, BT_STATUS_INVALID_PARM, ("Null attributesMask"));

	status = Btl_BppSnd_ParseGetJobAttributesResponse(
							responseObj,
							attributesData,
							BTL_BPPSND_GET_JOB_ATTRIBUTE_MASK_ALL,
							attributesMask );

	BTL_FUNC_END_AND_UNLOCK();

	return status;

}




/*   E N H A N C E D     M O D E     A P I*/
/* ---------------------------------------*/

BtStatus BTL_BPPSND_ConnectSimplePushPrintDisconnect(
			BtlBppsndContext 	*bppsndContext, 
			const BD_ADDR 	*bdAddr, 
			const BtlObject 	*objToPrint)
{
	BtStatus		status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK("BTL_BPPSND_ConnectSimplePushPrintDisconnect");
	
/* the following is NOT the real implementaion of the function ... ^^Uzi*/

	BTL_VERIFY_ERR((0 != bppsndContext), BT_STATUS_INVALID_PARM, ("Null bppsndContext"));
	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));
	BTL_VERIFY_ERR((BTL_BPPSND_STATE_DISCONNECTED == bppsndContext->state), BT_STATUS_IMPROPER_STATE,
					("context must be enabled and disconnected"));

	status = Btl_bppsd_CheckObjectValidity(objToPrint);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), BT_STATUS_INVALID_PARM, ("invalid  object to print"));

	OS_MemCopy((U8*) &(bppsndContext->objToPrint), (U8*) objToPrint, sizeof(BtlObject));

	bppsndContext->complexApiState = BTL_BPPSND_STATE_CONNECTING;
	BTL_BPPSND_Connect(bppsndContext, bdAddr);

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}


/*-------------------------------------------------------------------------------
 * BTL_BPPSND_RadioOffProcessor()
 */
BtStatus Btl_Bppsnd_ContextRadioOffProcessor(BtlBppsndContext *bppsndContext)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BOOL	keepProcessing = TRUE;
	
	BTL_FUNC_START("Btl_Bppsnd_RadioOffProcessor");

	while (keepProcessing == TRUE)
	{
		keepProcessing = FALSE;

		switch (bppsndContext->radioOffState)
		{
			case BTL_BPPSND_CONTEXT_RADIO_OFF_STATE_IDLE:

				BTL_VERIFY_FATAL((BTL_BPPSND_DISABLING_STATE_IDLE == bppsndContext->disablingState), BT_STATUS_IN_PROGRESS, 
								("Disabling already in progress"));
				BTL_VERIFY_FATAL((BTL_BPPSND_ABORT_STATE_IDLE == bppsndContext->abortState), BT_STATUS_IN_PROGRESS, 
								("Operation Abort already in progress"));
				
				bppsndContext->asynchronousRadioOff = FALSE;
				bppsndContext->radioOffState = BTL_BPPSND_CONTEXT_RADIO_OFF_STATE_DISCONNECTING;
				
				status = Btl_Bppsnd_Disconnect(bppsndContext);
				BTL_VERIFY_FATAL((status == BT_STATUS_SUCCESS) || (BT_STATUS_PENDING == status), status, (""));
				
				if (status == BT_STATUS_SUCCESS)
				{
					keepProcessing = TRUE;
					bppsndContext->radioOffState = BTL_BPPSND_CONTEXT_RADIO_OFF_STATE_DONE;
				}
				else
				{
					bppsndContext->asynchronousRadioOff = TRUE;
				}

			break;
			
 	 		case BTL_BPPSND_CONTEXT_RADIO_OFF_STATE_DISCONNECTING:

				if (bppsndContext->state == BTL_BPPSND_STATE_DISCONNECTED)
				{				
					keepProcessing = TRUE;
					bppsndContext->radioOffState = BTL_BPPSND_CONTEXT_RADIO_OFF_STATE_DONE;
				}

			break;
			 
			case BTL_BPPSND_CONTEXT_RADIO_OFF_STATE_DONE:
				
				if (bppsndContext->asynchronousRadioOff == TRUE)
				{
					Btl_Bppsnd_RadioOffProcessor(BTL_BPPSND_RADIO_OFF_EVENT_CONTEXT_COMPLETED);	
				}

				bppsndContext->radioOffState = BTL_BPPSND_CONTEXT_RADIO_OFF_STATE_IDLE;
				
			break;

			default:

				BTL_FATAL(BT_STATUS_INTERNAL_ERROR, ("Unexpected Disabling State (%d)", bppsndContext->disablingState));
				
		};
	}
	
	BTL_FUNC_END();

	return status;
}

BtStatus Btl_Bppsnd_RadioOffProcessor(BtlBppsndRadioOffEvent event)
{
	BtStatus 		status = BT_STATUS_SUCCESS;
	BtlContext	*context = 0;
	BOOL		keepProcessing = FALSE;

	BTL_FUNC_START("Btl_Bppsnd_RadioOffProcessor");

	keepProcessing = TRUE;

	while (keepProcessing == TRUE)
	{

		keepProcessing = FALSE;

		switch (btlBppsndData.radioOffState)
		{
			case BTL_BPPSND_RADIO_OFF_STATE_IDLE:

				BTL_VERIFY_FATAL(event == BTL_BPPSND_RADIO_OFF_EVENT_START, BT_STATUS_INTERNAL_ERROR, ("Unexpected Event (%d), event"));

				btlBppsndData.asynchronousRadioOff = FALSE;
				btlBppsndData.numOfDisconnectingContexts = 0;
				btlBppsndData.radioOffState = BTL_BPPSND_RADIO_OFF_STATE_DISCONNECTING;
				
				IterateList(btlBppsndData.contextsList, context, BtlContext*)
				{
					status = Btl_Bppsnd_ContextRadioOffProcessor((BtlBppsndContext*)context);

					BTL_VERIFY_FATAL_NORET((status == BT_STATUS_SUCCESS) || (status == BT_STATUS_PENDING),
												("Btl_Bppsnd_ContextRadioOffProcessor Returned an unexpected status (%s)", pBT_Status(status)));

					if (status == BT_STATUS_PENDING)
					{
						++btlBppsndData.numOfDisconnectingContexts;
					}
				}

				if (btlBppsndData.numOfDisconnectingContexts == 0)
				{
					status = BT_STATUS_SUCCESS;
					btlBppsndData.radioOffState = BTL_BPPSND_RADIO_OFF_STATE_DONE;
					keepProcessing = TRUE;
				}
				else
				{
					status = BT_STATUS_PENDING;
					btlBppsndData.asynchronousRadioOff = TRUE;
				}

			break;

			case BTL_BPPSND_RADIO_OFF_STATE_DISCONNECTING:

				BTL_VERIFY_FATAL(event == BTL_BPPSND_RADIO_OFF_EVENT_CONTEXT_COMPLETED, BT_STATUS_INTERNAL_ERROR, ("Unexpected Event (%d), event"));
				BTL_VERIFY_FATAL(btlBppsndData.numOfDisconnectingContexts > 0, BT_STATUS_INTERNAL_ERROR, ("Not expecting any contexts to disconnect"));

				--btlBppsndData.numOfDisconnectingContexts;

				if (btlBppsndData.numOfDisconnectingContexts == 0)
				{
					btlBppsndData.radioOffState = BTL_BPPSND_RADIO_OFF_STATE_DONE;
					keepProcessing = TRUE;
				}

			break;
			
			case BTL_BPPSND_RADIO_OFF_STATE_DONE:

				if (btlBppsndData.asynchronousRadioOff == TRUE)
				{
					BtlModuleNotificationCompletionEvent	moduleCompletedEvent;
					moduleCompletedEvent.type = BTL_MODULE_NOTIFICATION_COMPLETION_EVENT_TYPE_RADIO_OFF;
					moduleCompletedEvent.status = BT_STATUS_SUCCESS;
						
					 BTL_ModuleCompleted(BTL_MODULE_TYPE_BPPSND , &moduleCompletedEvent);
				}

				btlBppsndData.radioOffState = BTL_BPPSND_RADIO_OFF_STATE_IDLE;

			break;
		};
	};

	BTL_FUNC_END();

	return status;
}

BtStatus BtlBppsndBtlNotificationsCb(BtlModuleNotificationType notificationType)
{
	BtStatus 		status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BtlBppsndBtlNotificationsCb");
	
	BTL_LOG_DEBUG(("Notification Type: %d", notificationType));
	
	switch (notificationType)
	{
		case BTL_MODULE_NOTIFICATION_RADIO_ON:

			/* Do something if necessary */
		
			break;

		case BTL_MODULE_NOTIFICATION_RADIO_OFF:

			status = Btl_Bppsnd_RadioOffProcessor(BTL_BPPSND_RADIO_OFF_EVENT_START);
			
			break;

		default:

			BTL_ERR(BT_STATUS_INTERNAL_ERROR, ("Invalid notification (%d)", notificationType));
			


	};

	BTL_FUNC_END();
	
	return status;
}


#if BTL_CONFIG_BPPSND_DISCONNECT_TIMEOUT > 0

static void BtlBppsndDisconnectionTimeout(EvmTimer *timer)
{
	BtStatus status;
	BtlBppsndContext *bppsndContext;

	BTL_FUNC_START("BtlBppsndDisconnectionTimeout");

	BTL_LOG_INFO(("Disconnection timer expired, now we disconnect ACL..."));

	bppsndContext = (BtlBppsndContext *)(timer->context);

	bppsndContext->disconnectionTimerPending = FALSE;

	status = Btl_Bppsnd_Disconnect(bppsndContext);
	BTL_BPPSND_InitOperationVariables(bppsndContext);
	if (BT_STATUS_PENDING != status)
	{
		/* bppsndContext->state = BTL_BPPSND_STATE_DISCONNECTED; */
		bppsndContext->complexApiState = BTL_BPPSND_STATE_IDLE;
	}

	BTL_FUNC_END();
}

#endif /* BTL_CONFIG_BPPSND_DISCONNECT_TIMEOUT > 0 */


  #else /* BTL_CONFIG_BPPSND ==   BTL_CONFIG_ENABLED*/

/*-------------------------------------------------------------------------------
 * BTL_BPPSND_Init() - When  BTL_CONFIG_BPPSND is disabled.
 */
BtStatus BTL_BPPSND_Init(void)
{
    
   BTL_LOG_INFO(("BTL_BPPSND_Init() -  BTL_CONFIG_BPPSND Disabled"));
  

    return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 * BTL_BPPSND_Deinit() - When  BTL_CONFIG_BPPSND is disabled.
 */
BtStatus BTL_BPPSND_Deinit(void)
{
    BTL_LOG_INFO(("BTL_BPPSND_Deinit() -  BTL_CONFIG_BPPSND Disabled"));

    return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------*/



#endif /* BTL_CONFIG_BIP==   BTL_CONFIG_ENABLED*/
  

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
*   DESCRIPTION:    This file defines the implementation of the BTL Basic Image Profile
*
*   AUTHOR:         Arnoud van Riessen
*
\*******************************************************************************/

#include "btl_config.h"
#include "btl_log.h"
#include "btl_defs.h"
#include "btl_biprsp.h"


BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_BIPRSP);

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
#include "debug.h"
#include "utils.h"
#include "bthal_fs.h"
#include "sdp.h"
#include "bip.h"
#include "goep.h"

#include "btl_commoni.h"
#include "btl_log_modules.h"
#include "btl_pool.h"
#include "btl_utils.h"
#include "btl_obex_utils.h"
#include "btl_debug.h"
#include "btl_unicode.h"
#include "btl_bip_common.h"
#include "btl_bip_xml.h"
#include "btl_biprsp.h"



/********************************************************************************
 *
 * Constants
 *
 *******************************************************************************/
#define BTL_BIPRSP_MAX_FSPATH_LEN BTHAL_FS_MAX_PATH_LENGTH

/* Max number of channels supported by this Responcer (server).      */
/* Now fixed to 1 because multi-instance is not supported yet.       */
/* When multi-instamce is added, it should become a config parameter */
#define MAX_CHANNELS 1

/*-------------------------------------------------------------------------------
 * BIP_SERVICE_NAME_MAX_LEN constant
 *
 *     Represents max length of service name for BIP.
 */
#define BIP_SERVICE_NAME_MAX_LEN              (32)

#define U32_DEC_LENGTH                        (10)

#define XML_IMAGELIST_START                     "<images-listing version=\"1.0\">\n"
#define XML_IMAGELIST_ENTRY1                    "<image handle=\""
#define XML_IMAGELIST_ENTRY2                    "\" created=\""
#define XML_IMAGELIST_ENTRY3                    "\" modified=\""
#define XML_IMAGELIST_ENTRY4                    "\"/>\n"
#define XML_IMAGELIST_END                       "</images-listing>\n"

#define BTL_BIP_IMAGE_CAPABILITIES_START        "<imaging-capabilities version=\"1.0\">\n"
#define BTL_BIP_IMAGE_CAPABILITIES_START2       "\">\n"
#define BTL_BIP_IMAGE_CAPABILITIES_PREF_ENC     "<preferred-format encoding=\""
#define BTL_BIP_IMAGE_CAPABILITIES_PREF_PIXEL   "\" pixel=\""
#define BTL_BIP_IMAGE_CAPABILITIES_PREF_SIZE    "\" maxsize=\""
#define BTL_BIP_IMAGE_CAPABILITIES_PREF_END     "\"/>\n"

#define BTL_BIP_IMAGE_CAPABILITIES_IFMT_ENC     "<image-formats encoding=\""
#define BTL_BIP_IMAGE_CAPABILITIES_IFMT_PIXEL   "\" pixel=\""
#define BTL_BIP_IMAGE_CAPABILITIES_IFMT_SIZE    "\" size=\""
#define BTL_BIP_IMAGE_CAPABILITIES_IFMT_END     "\"/>\n"

#define BTL_BIP_IMAGE_CAPABILITIES_ATTF_TYPE    "<attachment-formats content-type=\""
#define BTL_BIP_IMAGE_CAPABILITIES_ATTF_CHARS   "\" charset=\""
#define BTL_BIP_IMAGE_CAPABILITIES_ATTF_END     "\"/>\n"

#define BTL_BIP_IMAGE_CAPABILITIES_FILTER_CR    "<filtering-parameters "
#define BTL_BIP_IMAGE_CAPABILITIES_FILTER_MOD   "\" modified=\""
#define BTL_BIP_IMAGE_CAPABILITIES_FILTER_END   "\"/>\n"
#define BTL_BIP_IMAGE_CAPABILITIES_END          "</imaging-capabilities>"

#define BTL_BIP_IMAGE_PROPERTIES_START          "<image-properties version=\"1.0\" "
#define BTL_BIP_IMAGE_PROPERTIES_HANDLE         "handle=\""
#define BTL_BIP_IMAGE_PROPERTIES_FRIENDLY_NAME "friendly-name=\""
#define BTL_BIP_IMAGE_PROPERTIES_START2         "\">\n"

#define BTL_BIP_IMAGE_PROPERTIES_NENC_ENC       "<native encoding=\""
#define BTL_BIP_IMAGE_PROPERTIES_NENC_PIXEL     "pixel=\""
#define BTL_BIP_IMAGE_PROPERTIES_NENC_SIZE      "size=\""
#define BTL_BIP_IMAGE_PROPERTIES_NENC_END       "/>\n"

#define BTL_BIP_IMAGE_PROPERTIES_VENC_ENC       "<variant encoding=\""
#define BTL_BIP_IMAGE_PROPERTIES_VENC_PIXEL     "\" pixel=\""
#define BTL_BIP_IMAGE_PROPERTIES_VENC_SIZE      "\" size=\""
#define BTL_BIP_IMAGE_PROPERTIES_VENC_END       "\"/>\n"

#define BTL_BIP_IMAGE_PROPERTIES_ATTCH_TYPE     "<attachment content-type=\""
#define BTL_BIP_IMAGE_PROPERTIES_ATTCH_NAME     "\" name=\""
#define BTL_BIP_IMAGE_PROPERTIES_ATTCH_SIZE     "\" size=\""
#define BTL_BIP_IMAGE_PROPERTIES_ATTCH_END      "\"/>\n"

#define BTL_BIP_IMAGE_PROPERTIES_END            "</image-properties>"

#define XML_ATTR_HANDLE                           "handle=\""
#define XML_ATTR_PIXEL                            "pixel=\""
#define XML_ATTR_SIZE                             "size=\""
#define XML_ATTR_MAXSIZE                          "maxsize=\""
#define XML_ATTR_NAME                             "name=\""
#define XML_ATTR_ENCODING                         "encoding=\""
#define XML_ATTR_TRANSFORMATION                   "transformation=\""
#define XML_ATTR_CHARSET                          "charset=\""
#define XML_ATTR_CREATED                          "created=\""
#define XML_ATTR_MODIFIED                         "modified=\""
#define XML_NON_EMPTY_ELEMENT_END      ">\n"
#define XML_ELEMENT_END                           "/>\n"

#define MAX_XML_DESCR_LEN ((U16) 1000) /* Max length of an XML Descriptor to be build */

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BtlBipInitState type
 *
 *     Defines the BIP init state.
 */
typedef enum _BtlBiprspInitState
{
    BTL_BIPRSP_INIT_STATE_NOT_INITIALIZED,
    BTL_BIPRSP_INIT_STATE_INITIALIZED,
    BTL_BIPRSP_INIT_STATE_INITIALIZATION_FAILED
} BtlBiprspInitState;

/*-------------------------------------------------------------------------------
 * BtlBipState type
 *
 *     Defines the BIP state of a specific context.
 */
typedef enum _BtlBiprspState
{
     BTL_BIPRSP_STATE_IDLE,
     BTL_BIPRSP_STATE_DISCONNECTED,
     BTL_BIPRSP_STATE_CONNECTED,
     BTL_BIPRSP_STATE_DISCONNECTING,
     BTL_BIPRSP_STATE_DISABLING,
     BTL_BIPRSP_STATE_CONNECTING
} BtlBiprspState;

typedef enum _BtlBiprspAbortState
{
     BTL_BIPRSP_ABORT_STATE_IDLE,
     BTL_BIPRSP_ABORT_STATE_USER_ABORT,
     BTL_BIPRSP_ABORT_STATE_INTERNAL_ABORT,
     BTL_BIPRSP_ABORT_STATE_ABORTED
} BtlBiprspAbortState;

/*-------------------------------------------------------------------------------
 * ObjectRequestState type
 *
 *     	Defines the state of the object request being processed by the APP.
 *      A specific 'bipOp' is send to the APP, this context is waiting for
 *      a specific call from the APP as mentioned between brackets.
 */
typedef enum _ObjectRequestState
{
    OBJECT_REQUEST_STATE_IDLE,                   /* No outstanding requests being handled by the APP.*/
    OBJECT_REQUEST_STATE_PUT_IMAGE,	            /* Wait for BTL_BIPRSP_RespondPutImage */
    OBJECT_REQUEST_STATE_PUT_LINKED_THUMBNAIL, /* Wait for BTL_BIPRSP_RespondPutLinkedThumbnail*/
    OBJECT_REQUEST_STATE_GET_IMAGE,              /* Wait for BTL_BIPRSP_RespondGetImage */
    OBJECT_REQUEST_STATE_GET_LINKED_THUMBNAIL, /* Wait for BTL_BIPRSP_RespondGetLinkedThumbnail */
    OBJECT_REQUEST_STATE_GET_MONITORING_IMAGE, /* Wait for BTL_BIPRSP_RespondGetMonitoringImage */
    OBJECT_REQUEST_STATE_GET_IMAGES_LIST,       /* Wait for BTL_BIPRSP_RespondGetImagesList */
    OBJECT_REQUEST_STATE_GET_IMAGE_PROPERTIES, /* Wait for BTL_BIPRSP_RespondGetImageProperties */
    OBJECT_REQUEST_STATE_GET_CAPABILITIES       /* Wait for BTL_BIPRSP_RespondGetCapabilities */
} ObjectRequestState;

/*-------------------------------------------------------------------------------
 * BtlBiprspRadioOffState type
 *
 *     Defines the BIPRSP Radio Off state of a specific context.
 */
typedef enum _BtlBiprspRadioOffState
{
	 BTL_BIPRSP_RADIO_OFF_STATE_IDLE,
 	 BTL_BIPRSP_RADIO_OFF_STATE_DISCONNECTING,
	 BTL_BIPRSP_RADIO_OFF_STATE_DONE
} BtlBiprspRadioOffState;

typedef enum _BtlBiprspdRadioOffEvent
{
	 BTL_BIPRSP_RADIO_OFF_EVENT_START,
 	 BTL_BIPRSP_RADIO_OFF_EVENT_CONTEXT_COMPLETED
} BtlBiprspRadioOffEvent;

/*-------------------------------------------------------------------------------
 * BtlBiprspContextRadioOffState type
 *
 *     Defines the BIPRSP Radio Off state of a specific context.
 */
typedef enum _BtlBiprspContextRadioOffState
{
	 BTL_BIPRSP_CONTEXT_RADIO_OFF_STATE_IDLE,
 	 BTL_BIPRSP_CONTEXT_RADIO_OFF_STATE_DISCONNECTING,
	 BTL_BIPRSP_CONTEXT_RADIO_OFF_STATE_DONE
} BtlBiprspContextRadioOffState;

/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BtlBipChannel structure
 *
 *     Represents BTL BIP channel.
 */
typedef struct _BtlBipChannel
{
/*gesl: TBD for multi-instance */
      /* BIP channel as exported in the API of BTL_BIP. */
/*    BtlBipChannelId channelId; */

    /* Channel ID as exported by the BIP stack module. */
    BipChannel channel;

    /* Current state of the channel */
    BtlBipChannelState state;

    /* pointer to BIP Responder context */
    BtlBiprspContext *context;
} BtlBipChannel;

/*-------------------------------------------------------------------------------
 * BtlBiprspContext structure
 *
 *     Represents BTL BIP context (for both Initiator and Responder).
 */
struct _BtlBiprspContext 
{
    /* Must be first field */
    BtlContext              base;

    BtlBipContextState      state; /*gesl: parameters is not used*/
    BtlBiprspCallBack       callback;
    ObexAbortReason         failureReason;
    BOOL                    userRequestedAbort;
    BipObexServer           *bipObexServer;
    BipResponderSession     sessionResponder;
    BipData                 bipData;
    BtlBipObjStore          objStore;
    BipData                 *eventBipData;
    BtlObject               bipObject;
    BthalFsDirDesc          dirDesc;
    BthalFsStat             filestat;
    BOOL                    bAccept;
    BOOL                    autoRequestLinkedThumbnail; /* TRUE = This mdoule will automatically ask teh Initiator     */
                                                           /*         for a thumbnail when a PutImage request is received. */
    BtlUtf8                 fsPath[BTL_BIPRSP_MAX_FSPATH_LEN + 1]; /* 0-terminate string of the full path name. */

    /* Updated SDP record info*/
    BtlUtf8                 serviceName[BIP_SERVICE_NAME_MAX_LEN + 3]; /* include 2 bytes for SDP_TEXT_8BIT */
    U8                      supFeatures[3]; /* Overrules default supported features (SDP translation of 'service' param in Enable function) */

    /* XML object build data as started (inititalized) with the several BTL_BIPRSP_Build***Start functions.*/
    U16                     xmlObjMaxLen;
    U16                     xmlObjLen; /* Actual Length (bytes) of teh next string.         */
    U8                     *xmlObj;    /* XML object 0-terminated string under construction. */
	
    BtlUtf8                 xmlDescr[MAX_XML_DESCR_LEN]; /* 0-terminated UTF-8 string area, used for building an XML Descriptor for a BIP request to be send.*/
	
/*gesl: TODO: SPEED optimization for Build: use the end of xmlObj as input for a Build function. Then it finds the end (0-character) faster*/
/*gesl: TODO: Add a *len parameter to the Build functions:*/
/*      Input is space left in the 'xmlObj' string before the build.*/
/*      Output = space left after the build (0 (or65535) = out of range)*/
/*      len = NULL: don't care. (see XmlStrCat example routines in this module as example (not used yet).*/

	BOOL			deleteFileIfAborted;
	BtlUtf8          		pushedFileName[BTHAL_FS_MAX_PATH_LENGTH + 1];

	BOOL						pendingDisconnection;
	BtlBiprspContextRadioOffState	radioOffState;
	BOOL						asynchronousRadioOff;

	U32				objLen;

	BipImgHandle		imgHandleAssignedToPushedImage;

	BOOL			suspendContinue;
	BOOL			continuePending;
	BtlUtf8			desiredPushedFileName[BTHAL_FS_MAX_PATH_LENGTH + 1];
	
	BtlBiprspAbortState	abortState;
};

/*-------------------------------------------------------------------------------
 * BtlBipData structure
 *
 *     Represents the data of th BTL Bip module.
 */
typedef struct _BtlBipData
{
    /* Pool of Bip contexts */
	BTL_POOL_DECLARE_POOL(contextsPool, contextsMemory, BTL_CONFIG_BIPRSP_MAX_NUM_OF_CONTEXTS, sizeof(BtlBiprspContext));
    
    /* List of active Bip contexts */
    ListEntry               contextsList;

    BtlObjProgressInd       progressIndInfo;
    
    BipCallbackParms        selfGeneratedCallbackParms;
    
    /* Event passed to the application */
    BtlBiprspEvent    eventToApp;

	BtlBiprspRadioOffState	radioOffState;
	U32					numOfDisconnectingContexts;
	BOOL				asynchronousRadioOff;
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
static BtlBiprspInitState btlBipInitState = BTL_BIPRSP_INIT_STATE_NOT_INITIALIZED;


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
static const char btlBipContextsPoolName[] = "BipResponderContexts";


/*-------------------------------------------------------------------------------
 * btlChannelAdmin
 *
 *     Mapping of the channel (from BIP) to a specific channel administration
 *     When the 'state' parameter in this admin is
 *     BTL_BIP_CHANNEL_STATE_DISCONNECTED then this channel admin is not in use.
 */
static BtlBipChannel btlChannelAdmin[MAX_CHANNELS];


/********************************************************************************
 *
 * Internal function prototypes
 *
 *******************************************************************************/
static BOOL IsConnected(BtlBiprspContext *bipContext);
static BtlBiprspContext *GetContextFromBipCallbackParms(BipCallbackParms* parms);
static void InitOperationVariables(BtlBiprspContext* context);
static void BiprspCallback(BipCallbackParms* parms);
static void AfterProcessing(BtlBiprspContext* context, BtlBiprspEvent *bipEvent);

#ifdef DEBUG
static S8* pInitState(BtlBiprspInitState state);
static S8* pState(BtlBiprspState state);
#endif

static char bip_ntoc( U8 Nr );
static char *bip_itoa( U32 Nr, S8 *AddrString,BOOL bZeroFill, U8 nrOfChar);

static void *getDataPtrFromBipDataOp   (BipData *bipData);
static void *getDataPtrFromBipDataType (BipData *bipData);
static BtStatus BtlBiprspBtlNotificationsCb(BtlModuleNotificationType notificationType);
static void XmlBuildAttrBool(U8 *xmlStr, BOOL value);

static BtlBipChannel *GetChannelAdmin(BipChannel channel);
static BtlBipChannel *GetFreeChannelAdmin(void);
static void SetContextState(BtlBiprspContext *bipContext,
                            BtlBipContextState  state);
static void SetChannelState(BtlBipChannel *bipChannel,
                            BtlBipChannelState state);

static BtStatus BtlBiprspDisconnect(BtlBiprspContext *bipContext);
static BtStatus BtlBiprspBtlNotificationsCb(BtlModuleNotificationType notificationType);
static BtStatus BtlBiprspContextRadioOffProcessor(BtlBiprspContext *bipContext);
static BtStatus BtlBiprspRadioOffProcessor(BtlBiprspRadioOffEvent event);

/* Some debug logging features. */
#if (XA_DEBUG == XA_ENABLED) || (XA_DEBUG_PRINT == XA_ENABLED)
static void LogContextState(BtlBipContextState state);
static void LogChannelState(BtlBipChannelState state);
#define BTL_BIP_LOG_CONTEXT_STATE(state) LogContextState(state)
#define BTL_BIP_LOG_CHANNEL_STATE(state) LogChannelState(state) 
#else
/* Debugging disabled --> define empty macros */
#define BTL_BIP_LOG_CONTEXT_STATE(state)
#define BTL_BIP_LOG_CHANNEL_STATE(state)
#endif /* (XA_DEBUG == XA_ENABLED) || (XA_DEBUG_PRINT == XA_ENABLED) */

static void BtlBiprspSendProgressNotificationToApp(BtlBiprspContext*context, BipCallbackParms* parms, U32 currPos);
static void BtlBiprspSendEventToApp(BtlBiprspContext*context, BipCallbackParms *parms);
static BtStatus BtlBiprspCopyTempFileToMem(	BtlBiprspContext 	*context, 
													const BtlUtf8 		*tempFilePath, 
													BtlBipObjStore 	*objStore, 
													void 			*memAddress);

static BtStatus BtlBiprspRenameFile(const BtlUtf8 *srcFullFileName, const BtlUtf8 *destFullFileName);
static void BtlBiprspInitRequestVars(BtlBiprspContext *bipContext);


/********************************************************************************
 *
 * Function declarations
 *
 *******************************************************************************/
 
/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_Init()
 */
BtStatus BTL_BIPRSP_Init(void)
{
    BtStatus status = BT_STATUS_SUCCESS;

    BTL_FUNC_START_AND_LOCK_COMMON("BTL_BIPRSP_Init");
    
    BTL_VERIFY_ERR((BTL_BIPRSP_INIT_STATE_INITIALIZATION_FAILED != btlBipInitState), BT_STATUS_IMPROPER_STATE,
                    ("BIP initialization failed before, please de-init before retrying "));
    BTL_VERIFY_ERR((BTL_BIPRSP_INIT_STATE_NOT_INITIALIZED == btlBipInitState), BT_STATUS_IMPROPER_STATE,
                    ("BIP module is already initialized"));
    
    btlBipInitState = BTL_BIPRSP_INIT_STATE_INITIALIZATION_FAILED;

    status = BTL_POOL_Create(&btlBipData.contextsPool,
                            btlBipContextsPoolName,
                            (U32 *)btlBipData.contextsMemory, 
                            BTL_CONFIG_BIPRSP_MAX_NUM_OF_CONTEXTS,
                            sizeof(BtlBiprspContext));
    BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("BIP contexts pool creation failed"));
    
    InitializeListHead(&btlBipData.contextsList);
        
	BTL_RegisterForCommonNotifications(BTL_MODULE_TYPE_BIPRSP, BtlBiprspBtlNotificationsCb);

	btlBipData.radioOffState = BTL_BIPRSP_RADIO_OFF_STATE_IDLE;
	btlBipData.numOfDisconnectingContexts = 0;
	btlBipData.asynchronousRadioOff = FALSE;

    btlBipInitState = BTL_BIPRSP_INIT_STATE_INITIALIZED;

    BTL_FUNC_END_AND_UNLOCK();
    
    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_Deinit()
 */
BtStatus BTL_BIPRSP_Deinit(void)
{
    BtStatus status = BT_STATUS_SUCCESS;

    BTL_FUNC_START_AND_LOCK_COMMON("BTL_BIPRSP_Deinit");

	BTL_DeRegisterFromCommonNotifications(BTL_MODULE_TYPE_BIPRSP);
    
    BTL_VERIFY_ERR((BTL_BIPRSP_INIT_STATE_NOT_INITIALIZED != btlBipInitState), BT_STATUS_FAILED,
                    ("BIP module is not initialized"));

    RemoveHeadList(&btlBipData.contextsList);
    BTL_VERIFY_FATAL_NORET((IsListEmpty(&btlBipData.contextsList)), ("BIP contexts are still active"));

    status = BTL_POOL_Destroy(&btlBipData.contextsPool);
    BTL_VERIFY_FATAL_NORET((BT_STATUS_SUCCESS == status), ("BIP contexts pool destruction failed"));
        
    btlBipInitState = BTL_BIPRSP_INIT_STATE_NOT_INITIALIZED;

    BTL_FUNC_END_AND_UNLOCK();
    
    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_Create()
 */
BtStatus BTL_BIPRSP_Create(BtlAppHandle             *appHandle,
                           BtlBiprspCallBack  bipCallback,
                           const BtSecurityLevel    *securityLevel,
                           BtlBiprspContext            **bipContext)
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

    status = BTL_HandleModuleInstanceCreation(appHandle, BTL_MODULE_TYPE_BIPRSP, &(*bipContext)->base);
    
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

    (*bipContext)->fsPath[0] = 0;
    (*bipContext)->fsPath[BTL_BIPRSP_MAX_FSPATH_LEN] = 0;

    /* Add the new BIP context to the active contexts list */
    InsertTailList(&btlBipData.contextsList, &((*bipContext)->base.node));

#if BT_SECURITY == XA_ENABLED
    /* Save the given security level, or use default */
    if (NULL != securityLevel)
    {
      (*bipContext)->sessionResponder.primary.sApp.secRecord.level = *securityLevel;
    }
    else
    {
      (*bipContext)->sessionResponder.primary.sApp.secRecord.level = BSL_NO_SECURITY;
    }
#endif  /* BT_SECURITY == XA_ENABLED */

	(*bipContext)->pendingDisconnection = FALSE;
	(*bipContext)->radioOffState = BTL_BIPRSP_CONTEXT_RADIO_OFF_STATE_IDLE;
	(*bipContext)->asynchronousRadioOff = FALSE;

	BtlBiprspInitRequestVars(*bipContext);
	
    /* Init BIP context state */
    SetContextState(*bipContext, BTL_BIP_CONTEXT_STATE_IDLE);

    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_Destroy()
 */
BtStatus BTL_BIPRSP_Destroy(BtlBiprspContext **bipContext)
{
    BtStatus status = BT_STATUS_SUCCESS;
    BOOL isAllocated = FALSE;

    BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BIPRSP_Destroy");

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
    *bipContext = 0;

    BTL_FUNC_END_AND_UNLOCK();

    return (status);
}

/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_Enable()
 */
BtStatus BTL_BIPRSP_Enable(BtlBiprspContext *bipContext,
                           BtlUtf8             *servicename,
                           BipService          *service)
{
    BtStatus status = BT_STATUS_SUCCESS;
    BtStatus deregisterStatus;
    U16      len = 0;   

    BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BIPRSP_Enable");

    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));
    BTL_VERIFY_ERR((BTL_BIP_CONTEXT_STATE_IDLE == bipContext->state), BT_STATUS_IMPROPER_STATE, 
                    ("context is already enabled"));

    if ((servicename != NULL) && ((len = OS_StrLen((char*)servicename)) != 0))
    {
        BTL_VERIFY_ERR((len <= BIP_SERVICE_NAME_MAX_LEN), BT_STATUS_FAILED, ("BIPRSP service name is too long!"));
    }

    /* Register as a BIP-Initiator, and register sender callback (bipContext->allback) */
    status = BIP_RegisterResponder(&bipContext->sessionResponder,BiprspCallback);
    BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, 
                    ("Failed to register BIP Initiator (%s)", pBT_Status(status)));

    /* Update serviceName */
    if (len > 0)
    {
        /* The first 2 bytes are for SDP_TEXT_8BIT */
        bipContext->serviceName[0] = DETD_TEXT + DESD_ADD_8BITS;
        bipContext->serviceName[1] = (U8)(len + 1); /* Includes '\0' */
        
        OS_MemCopy((bipContext->serviceName + 2), servicename, len);
        bipContext->serviceName[(len + 2)] = '\0';

        bipContext->sessionResponder.responderAttrib[4].len = (U16)(len+3);
        bipContext->sessionResponder.responderAttrib[4].value = bipContext->serviceName;
    }

    /* Service present? --> update Supported features  in the SDP record. */
    if (NULL != service)
    {
        /* Supported features field in SDP record always starts with this byte .*/
        /* (Inspired by SDP_UINT_16BIT macro.                                   */
        bipContext->supFeatures[0] = DETD_UINT + DESD_2BYTES;

        /* Store this 'service' as the new supported features in the SDP record.      */
        /*  SDP record information should be stored in big-endian format.             */
        StoreBE16(&(bipContext->supFeatures[1]), *service);
        
        /* Overrule the default attribute by pointing to this new attribute now. */
        bipContext->sessionResponder.responderAttrib[6].value = bipContext->supFeatures;
    }

#if BT_SECURITY == XA_ENABLED

	/* Register security record */
	status = GOEP_RegisterServerSecurityRecord(&bipContext->sessionResponder.primary.sApp,
	                                               bipContext->sessionResponder.primary.sApp.secRecord.level);

  /* Failed? --> cleanup BIP registration. */
	if (BT_STATUS_SUCCESS != status)
	{
        deregisterStatus = BIP_DeregisterResponder(&bipContext->sessionResponder);

    		BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == deregisterStatus), ("Failed deregistering BIP Responder"));

    		/* This verify will fail and will handle the exception gracefully */
    		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed registering BIP Responder security record"));
	}
  
#endif	/* BT_SECURITY == XA_ENABLED */

    SetContextState(bipContext, BTL_BIP_CONTEXT_STATE_IN_USE);
        
    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_Disable()
 */
BtStatus BTL_BIPRSP_Disable(BtlBiprspContext *bipContext)
{
    BtStatus status = BT_STATUS_SUCCESS;
    
    BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BIPRSP_Disable");

    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));
    BTL_VERIFY_ERR((BTL_BIP_CONTEXT_STATE_IN_USE == bipContext->state), BT_STATUS_IMPROPER_STATE,
                    ("context must be enabled"));

	SetContextState(bipContext, BTL_BIP_CONTEXT_STATE_DISABLING);

#if BT_SECURITY == XA_ENABLED

  	/* First, try to unregister security record */
  	status = GOEP_UnregisterServerSecurityRecord(&bipContext->sessionResponder.primary.sApp);
  	BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == status), ("Failed unregistering BIP Responder security record"));

#endif	/* BT_SECURITY == XA_ENABLED */

	status = BtlBiprspDisconnect(bipContext);

	if (status == BT_STATUS_SUCCESS)
	{
		status = BIP_DeregisterResponder(&bipContext->sessionResponder);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed De-Registering BIP Responder (%s)", pBT_Status(status)));
		
		SetContextState(bipContext, BTL_BIP_CONTEXT_STATE_IDLE);
    }

    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

#if BT_SECURITY == XA_ENABLED
/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_SetSecurityLevel()
 */
BtStatus BTL_BIPRSP_SetSecurityLevel(BtlBiprspContext *bipContext,
                                     BtSecurityLevel        *securityLevel)
{
    BtStatus status = BT_STATUS_SUCCESS;

    BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BIPRSP_SetSecurityLevel");

    if (NULL != securityLevel)
    {
        status = BTL_VerifySecurityLevel(*securityLevel);
        BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Invalid BIP securityLevel"));
    }

    /* Update the given security level, or use default */
    if (NULL != securityLevel)
    {
      bipContext->sessionResponder.primary.sApp.secRecord.level = *securityLevel;
    }
    else
    {
      bipContext->sessionResponder.primary.sApp.secRecord.level = BSL_NO_SECURITY;
    }

    BTL_FUNC_END_AND_UNLOCK();
    
    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_GetSecurityLevel()
 */
BtStatus BTL_BIPRSP_GetSecurityLevel(BtlBiprspContext *bipContext,
                                     BtSecurityLevel        *securityLevel)
{
    BtStatus status = BT_STATUS_SUCCESS;

    BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BIPRSP_GetSecurityLevel");

    *securityLevel = bipContext->sessionResponder.primary.sApp.secRecord.level;

    BTL_FUNC_END_AND_UNLOCK();
    
    return status;
}
#endif  /* BT_SECURITY == XA_ENABLED */

/*-------------------------------------------------------------------------------
 * BTL_OPPS_GetConnectedDevice()
 */
BtStatus BTL_BIPRSP_GetConnectedDevice(BtlBiprspContext *bipContext,
                                       BD_ADDR                *bdAddr)
{
    BtStatus status = BT_STATUS_SUCCESS;
    BOOL retVal;
    ObexTpConnInfo tpInfo;
    
    BTL_FUNC_START_AND_LOCK("BTL_BIPRSP_GetConnectedDevice");

    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));
    BTL_VERIFY_ERR((NULL != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));
    BTL_VERIFY_ERR(TRUE == IsConnected(bipContext), BT_STATUS_IMPROPER_STATE, 
                    ("context is not connected"));

    if( (NULL == bipContext) || (NULL == bdAddr) )
        return BT_STATUS_INVALID_PARM;

    tpInfo.size = sizeof(ObexTpConnInfo);
    tpInfo.tpType = OBEX_TP_BLUETOOTH;
    tpInfo.remDev = 0;

    retVal = GOEP_ServerGetTpConnInfo(&bipContext->sessionResponder.primary.sApp, &tpInfo);
    BTL_VERIFY_ERR((TRUE == retVal), BT_STATUS_NO_CONNECTION, ("Failed getting connection info failed"));

    BTL_VERIFY_ERR((NULL != tpInfo.remDev), BT_STATUS_INTERNAL_ERROR, ("Unable to find connected remote device"));
    
    *bdAddr = tpInfo.remDev->bdAddr;
    
    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_SetDefaultImagePath()
 */
BtStatus BTL_BIPRSP_SetDefaultImagePath(BtlBiprspContext *bipContext,
                                        BtlUtf8          *directory )
{
    BtStatus status = BT_STATUS_SUCCESS;

    BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_BIPRSP_SetDefaultImagePath");

    if( (NULL == bipContext) || (NULL == directory) )
    {
        status = BT_STATUS_INVALID_PARM;
    }
    else
    {
    	BTL_LOG_INFO(("Default Image Path set to: |%s|", directory));
        OS_StrCpyUtf8(bipContext->fsPath, directory);
    }

    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_SetAutoRequestLinkedThumbnail()
 */
BtStatus BTL_BIPRSP_SetAutoRequestLinkedThumbnail(BtlBiprspContext *bipContext, BOOL enable)
{
    BtStatus status = BT_STATUS_SUCCESS;

    BTL_FUNC_START_AND_LOCK("BTL_BIPRSP_SetAutoRequestLinkedThumbnail");

	BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));
	
    bipContext->autoRequestLinkedThumbnail = enable;

    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_GetAutoRequestLinkedThumbnail()
 */
BtStatus BTL_BIPRSP_GetAutoRequestLinkedThumbnail(BtlBiprspContext *bipContext, BOOL *enable)
{
    BtStatus status = BT_STATUS_SUCCESS;

    BTL_FUNC_START_AND_LOCK("BTL_BIPRSP_GetAutoRequestLinkedThumbnail");

	BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));
	
    *enable = bipContext->autoRequestLinkedThumbnail;

    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

static BtStatus BtlBiprspCompleteInitiatorPutRequest(BtlBiprspContext *bipContext, ObexRespCode responseCode)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BtlBiprspCompleteInitiatorPutRequest");

	BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));

	BIP_ResponderAccept(bipContext->bipObexServer, responseCode, &bipContext->bipData);
	 
	bipContext->suspendContinue = FALSE;

	if (bipContext->continuePending == TRUE)
	{
		BIP_ResponderContinue(bipContext->bipObexServer);
		
		bipContext->continuePending = FALSE;
	}

	BTL_FUNC_END();

	return status;
}

static BtStatus BtlBiprspCompleteAcceptedInitiatorRequest(BtlBiprspContext *bipContext)
{
	BtStatus	status = BT_STATUS_SUCCESS;
	ObStatus obStatus;

	BTL_FUNC_START("BtlBiprspCompleteAcceptedInitiatorRequest");
	
	obStatus = BIP_SendResponse(bipContext->bipObexServer, &bipContext->bipData);
	BTL_VERIFY_FATAL(obStatus == OB_STATUS_SUCCESS, BT_STATUS_INTERNAL_ERROR, ("BIP_SendResponse Failed (%d)",  obStatus)); 
	
	obStatus = GOEP_SetObexObjLen(	&bipContext->bipObexServer->sApp, 
									BIPOBS_GetObjectLen(bipContext->bipObexServer->object));    

	obStatus = GOEP_ServerAccept(&bipContext->bipObexServer->sApp, bipContext->bipObexServer->object);
	BTL_VERIFY_FATAL(obStatus == OB_STATUS_SUCCESS, BT_STATUS_INTERNAL_ERROR, 
						("GOEP_ServerAccept Failed (%)", obStatus)); 

	bipContext->suspendContinue = FALSE;

	if (bipContext->continuePending == TRUE)
	{
		BIP_ResponderContinue(bipContext->bipObexServer);
		
		bipContext->continuePending = FALSE;
	}

	BTL_FUNC_END();

	return status;
}

static BtStatus BtlBiprspCompleteAbortedInitiatorRequest(BtlBiprspContext *bipContext)
{
	BtStatus	status = BT_STATUS_SUCCESS;
	ObStatus obStatus;
	
	BTL_FUNC_START("BtlBiprspCompleteAbortedInitiatorRequest");
	
	obStatus = GOEP_ServerAbort(&bipContext->bipObexServer->sApp, OBRC_FORBIDDEN);
	BTL_VERIFY_FATAL(obStatus == OB_STATUS_SUCCESS, BT_STATUS_INTERNAL_ERROR, ("GOEP_ServerAbort Failed (%d)", obStatus)); 

	bipContext->suspendContinue = FALSE;

	if (bipContext->continuePending == TRUE)
	{
		BIP_ResponderContinue(bipContext->bipObexServer);
		
		bipContext->continuePending = FALSE;
	}	

	BTL_FUNC_END();

	return status;
}

static void BtlBiprspHandleTempResponseForPutOperations(BtlBiprspContext *bipContext)
{
    	BtStatus 				status = BT_STATUS_SUCCESS;	
	ObStatus 			obStatus = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BtlBiprspHandleTempResponseForPutOperations");
	
	bipContext->bipData.r.rImg.noImgHandle = FALSE;
			
	    /* Store the imgHandle in the reply. */
	    OS_StrCpyUtf8(bipContext->bipData.r.rImg.imgHandle, (BtlUtf8*)"");

	bipContext->objStore.type = BTL_BIP_OBJ_STORE_TYPEHANDLE;

        /* Prepare for reception of an image on the file-system. */
        bipContext->bipData.ocx = &bipContext->objStore;
	
	status =  btl_bip_FileOpen(	TRUE,
								(BtlUtf8*)BTHAL_FS_BIPRSP_TEMP_PUSHED_IMAGE_PATH,
								NULL,
								&(bipContext->objStore.location.fileHandle),
								&bipContext->bipData);
	BTL_VERIFY_FATAL_NORET(	status == BT_STATUS_SUCCESS, 
								("Failed Creating Temp File (%s)", BTHAL_FS_BIPRSP_TEMP_PUSHED_IMAGE_PATH));
		
	bipContext->deleteFileIfAborted = TRUE;
	OS_StrCpyUtf8(bipContext->pushedFileName, (BtlUtf8*)BTHAL_FS_BIPRSP_TEMP_PUSHED_IMAGE_PATH);
	
	obStatus = GOEP_ServerAccept(&bipContext->bipObexServer->sApp, bipContext->bipObexServer->object);
	BTL_VERIFY_FATAL(obStatus == OB_STATUS_SUCCESS, BT_STATUS_INTERNAL_ERROR, 
						("GOEP_ServerAccept Failed (%d)", obStatus)); 
    
    BTL_FUNC_END();
}

/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_RespondPutImage()
 */
BtStatus BTL_BIPRSP_RespondPutImage(BtlBiprspContext *bipContext,
                                           BOOL 			accept,
                                           BipImgHandle 	imgHandle)
{
    BtStatus status = BT_STATUS_SUCCESS;

    BTL_FUNC_START_AND_LOCK("BTL_BIPRSP_RespondPutImage");

    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));
    BTL_VERIFY_ERR((OS_StrLenUtf8(imgHandle) <= BTL_BIP_IMAGE_HANDLE_LEN), BT_STATUS_INVALID_PARM, ("imgHandle too long"));

	bipContext->bipData.r.rImg.noImgHandle = FALSE;
	
    /* Accepted to respond the requested image? --> prepare for it */
    if (TRUE == accept)
    {		
	    /* Store the imgHandle in the reply. */
	    OS_StrCpyUtf8(bipContext->bipData.r.rImg.imgHandle, imgHandle);
	
        /* Prepare for reception of an image on the file-system. */
        bipContext->bipData.ocx = &bipContext->objStore;
        bipContext->objStore.type = BTL_BIP_OBJ_STORE_TYPEHANDLE;

/*gesl: QUESTION: why only to disk and not to memory as well?*/
        status =  btl_bip_FileOpen(TRUE,
                                    bipContext->fsPath,
                                   bipContext->eventBipData->r.rImg.name,
                                   &(bipContext->objStore.location.fileHandle),
                                   &bipContext->bipData);

        /* Failed to create a file? --> abort the OBEX session + make a note. */
        if (BT_STATUS_SUCCESS != status)
        {
            BIP_Abort(&bipContext->sessionResponder, bipContext->bipObexServer->channel);

            /* Make  note + exit function. */
            BTL_ERR(status, ("Failed to create a file."));
        }

	bipContext->deleteFileIfAborted = TRUE;
	OS_StrCpyUtf8(bipContext->pushedFileName, bipContext->fsPath);
	OS_StrCatUtf8(bipContext->pushedFileName, bipContext->eventBipData->r.rImg.name);

	BTL_LOG_INFO(("Accepting Image Pushed and saving it in |%s|", (char*)bipContext->pushedFileName));
	
        /* Thumbnail is needed as well? --> tell it the Initiator. */
        if(TRUE == bipContext->autoRequestLinkedThumbnail)
        {			
		status = BtlBiprspCompleteInitiatorPutRequest(	bipContext, OBRC_PARTIAL_CONTENT);		
        }
        else
        {
		status = BtlBiprspCompleteInitiatorPutRequest(	bipContext, OBRC_ACCEPTED);
        }    

	BTL_VERIFY_ERR(status == BT_STATUS_SUCCESS, status, ("BtlBiprspCompleteInitiatorPutRequest Failed (%s)", pBT_Status(status)));
    }

    /* Not accepted to respond the requested image! --> refuse it with reason OBRC_FORBIDDEN*/
    else
    {
		status = BtlBiprspCompleteInitiatorPutRequest(bipContext, OBRC_FORBIDDEN);
		BTL_VERIFY_ERR(status == BT_STATUS_SUCCESS, status, ("BtlBiprspCompleteInitiatorPutRequest Failed (%s)", pBT_Status(status)));
    }
    
    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_RespondPutImageWithObj()
 */
BtStatus BTL_BIPRSP_RespondPutImageWithObj(BtlBiprspContext *bipContext,
                                           BOOL 			accept,
                                           BipImgHandle 	imgHandle,
                                           BtlObject		*objToReceive)
{
    BtStatus status = BT_STATUS_SUCCESS;

    BTL_FUNC_START_AND_LOCK("BTL_BIPRSP_RespondPutImageWithObj");

    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));

	bipContext->bipData.r.rImg.noImgHandle = FALSE;
	
    /* Accepted to respond the requested image? --> prepare for it */
    if (TRUE == accept)
    {		
	    BTL_VERIFY_ERR((OS_StrLenUtf8(imgHandle) <= BTL_BIP_IMAGE_HANDLE_LEN), BT_STATUS_INVALID_PARM, ("imgHandle too long"));
	    BTL_VERIFY_ERR((NULL != objToReceive), BT_STATUS_INVALID_PARM, ("Null objToReceive"));
		
	    /* Store the imgHandle in the reply. */
	    OS_StrCpyUtf8(bipContext->bipData.r.rImg.imgHandle, imgHandle);

		/* When saving image in a file, we rename the temporary file at the (successful) end of the operation */
		if (objToReceive->objectLocation == BTL_OBJECT_LOCATION_FS)
		{
			OS_StrCpyUtf8(bipContext->desiredPushedFileName, objToReceive->location.fsLocation.fsPath);
		}
		/* When saving the image in memory, stop using the file (switch to memory now) */
		else
		{
			status = BtlBiprspCopyTempFileToMem(	bipContext,
													(const BtlUtf8*)BTHAL_FS_BIPRSP_TEMP_PUSHED_IMAGE_PATH,
													&bipContext->objStore,
													objToReceive->location.memLocation.memAddr);
			BTL_VERIFY_ERR(status == BT_STATUS_SUCCESS, status, ("BtlBiprspCopyTempFileToMem Failed (%s)", pBT_Status(status)));									
		}

        /* Thumbnail is needed as well? --> tell it the Initiator. */
        if(TRUE == bipContext->autoRequestLinkedThumbnail)
        {
        	/* Save the handle just allocated to the pushed image. Use it to verify the handle of the thumbnail */
		OS_MemCopy(bipContext->imgHandleAssignedToPushedImage, imgHandle, BTL_BIP_IMAGE_HANDLE_LEN + 1);

		status = BtlBiprspCompleteInitiatorPutRequest(	bipContext, OBRC_PARTIAL_CONTENT );		
        }
        else
        {
		status = BtlBiprspCompleteInitiatorPutRequest(	bipContext, OBRC_ACCEPTED);		
        }    
		BTL_VERIFY_ERR(status == BT_STATUS_SUCCESS, status, ("BtlBiprspCompleteInitiatorPutRequest Failed (%s)", pBT_Status(status)));
    }
    /* Not accepted to respond the requested image! --> refuse it with reason OBRC_FORBIDDEN*/
    else
    {
		status = BtlBiprspCompleteInitiatorPutRequest(bipContext, OBRC_FORBIDDEN);
		BTL_VERIFY_ERR(status == BT_STATUS_SUCCESS, status, ("BtlBiprspCompleteInitiatorPutRequest Failed (%s)", pBT_Status(status)));
    }
    
    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_RespondPutLinkedThumbnailWithObj()
 */
BtStatus BTL_BIPRSP_RespondPutLinkedThumbnailWithObj(
									BtlBiprspContext 	*bipContext,
                                                     	BOOL 			accept,
                                                     	BtlObject			*objToReceive)
{
    BtStatus status = BT_STATUS_SUCCESS;
    
    BTL_FUNC_START_AND_LOCK("BTL_BIPRSP_RespondPutLinkedThumbnailWithObj");

    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));
	
    /* Accepted to respond the requested image? --> prepare for it */
    if (TRUE == accept)
    {
	    BTL_VERIFY_ERR((NULL != objToReceive), BT_STATUS_INVALID_PARM, ("Null objToReceive"));

		/* When saving image in a file, we rename the temporary file at the (successful) end of the operation */
		if (objToReceive->objectLocation == BTL_OBJECT_LOCATION_FS)
		{
			OS_StrCpyUtf8(bipContext->desiredPushedFileName, objToReceive->location.fsLocation.fsPath);
		}
		/* When saving the image in memory, stop using the file (switch to memory now) */
		else
		{
			status = BtlBiprspCopyTempFileToMem(	bipContext,
													(BtlUtf8*)BTHAL_FS_BIPRSP_TEMP_PUSHED_IMAGE_PATH,
													&bipContext->objStore,
													objToReceive->location.memLocation.memAddr);
			BTL_VERIFY_ERR(status == BT_STATUS_SUCCESS, status, ("BtlBiprspCopyTempFileToMem Failed (%s)", pBT_Status(status)));									
		}

	status = BtlBiprspCompleteInitiatorPutRequest(bipContext, OBRC_ACCEPTED);
	BTL_VERIFY_ERR(status == BT_STATUS_SUCCESS, status, ("BtlBiprspCompleteInitiatorPutRequest Failed (%s)", pBT_Status(status)));
    }    

    /* Not accepted to respond the requested image! --> refuse it with reason OBRC_FORBIDDEN*/
    else
    {
		status = BtlBiprspCompleteInitiatorPutRequest(bipContext, OBRC_FORBIDDEN);
		BTL_VERIFY_ERR(status == BT_STATUS_SUCCESS, status, ("BtlBiprspCompleteInitiatorPutRequest Failed (%s)", pBT_Status(status)));
    }
    
    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_RespondPutLinkedThumbnail()
 */
BtStatus BTL_BIPRSP_RespondPutLinkedThumbnail(BtlBiprspContext *bipContext,
                                                     BOOL accept,
                                                     BtlUtf8* fileName)
{
    BtStatus status = BT_STATUS_SUCCESS;
    
    BTL_FUNC_START_AND_LOCK("BTL_BIPRSP_RespondPutLinkedThumbnail");

    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));

    /* Accepted to respond the requested image? --> prepare for it */
    if (TRUE == accept)
    {
	  BTL_VERIFY_ERR((NULL != fileName), BT_STATUS_INVALID_PARM, ("Null fileName"));
        BTL_VERIFY_ERR((OS_StrLenUtf8(fileName) != 0), BT_STATUS_INVALID_PARM, ("Empty fileName"));

        bipContext->bipData.ocx = &bipContext->objStore;
        bipContext->objStore.type = BTL_BIP_OBJ_STORE_TYPEHANDLE;

        /* Give the thumbail 'fileName' the mandatory jpg extension. */
        /* XXXX OS_StrCat((char*)fileName, ".jpg"); */

        /* Create file for the thumbnail that we will get. */
        status =  btl_bip_FileOpen(TRUE,
                                    bipContext->fsPath,
                                    fileName,
                                   &(bipContext->objStore.location.fileHandle),
                                   &bipContext->bipData);

        /* Failed to create a file? --> abort the OBEX session + make a note. */
        if (BT_STATUS_SUCCESS != status)
        {
		status = BtlBiprspCompleteInitiatorPutRequest(bipContext, OBRC_INTERNAL_SERVER_ERR);
		BTL_VERIFY_ERR(status == BT_STATUS_SUCCESS, status, ("BtlBiprspCompleteInitiatorPutRequest Failed (%s)", pBT_Status(status)));

            /* Make  note + exit function. */
            BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed to create a file."));
        }

	bipContext->deleteFileIfAborted = TRUE;
	OS_StrCpyUtf8(bipContext->pushedFileName, bipContext->fsPath);
	OS_StrCatUtf8(bipContext->pushedFileName, fileName);

	status = BtlBiprspCompleteInitiatorPutRequest(	bipContext, OBRC_ACCEPTED);	
	BTL_VERIFY_ERR(status == BT_STATUS_SUCCESS, status, ("BtlBiprspCompleteInitiatorPutRequest Failed (%s)", pBT_Status(status)));
    }    

    /* Not accepted to respond the requested image! --> refuse it with reason OBRC_FORBIDDEN*/
    else
    {
		status = BtlBiprspCompleteInitiatorPutRequest(bipContext, OBRC_FORBIDDEN);
		BTL_VERIFY_ERR(status == BT_STATUS_SUCCESS, status, ("BtlBiprspCompleteInitiatorPutRequest Failed (%s)", pBT_Status(status)));
    }
    
    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_RespondGetCapabilities()
 */
BtStatus BTL_BIPRSP_RespondGetCapabilities(BtlBiprspContext *bipContext,
                                            BOOL accept,
                                            BtlObject *responseObj)
{
    BtStatus status = BT_STATUS_SUCCESS;

    BTL_FUNC_START_AND_LOCK("BTL_BIPRSP_RespondGetCapabilities");

    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));

    /* Accepted? --> respond capabilities */
    if (TRUE == accept)
    {
        bipContext->bipData.bipOp = BIPOP_GET_CAPABILITIES;
	 bipContext->bipData.ocx = &bipContext->objStore;
	 
	if (responseObj->objectLocation == BTL_OBJECT_LOCATION_MEM)
	{
	      bipContext->objStore.type = BTL_BIP_OBJ_STORE_TYPEMEMPTR;
	      	bipContext->objStore.location.mem = (U8*)responseObj->location.memLocation.memAddr;
      		bipContext->bipData.totLen = responseObj->location.memLocation.size;
	}
	else
	{
            BTL_VERIFY_ERR((NULL != responseObj->location.fsLocation.fsPath), BT_STATUS_INVALID_PARM, ("Empty path in object"));

            bipContext->objStore.type = BTL_BIP_OBJ_STORE_TYPEHANDLE;

            /* Open the file for reading. */
            status =  btl_bip_FileOpen(FALSE,
                               				responseObj->location.fsLocation.fsPath,
                               				NULL,
                               				&(bipContext->objStore.location.fileHandle),
                               				&bipContext->bipData);

            /* Failed to open the indicated file? --> abort the OBEX session + make a note. */
            if (BT_STATUS_SUCCESS != status)
            {
			status = BtlBiprspCompleteAbortedInitiatorRequest(bipContext);
			BTL_VERIFY_ERR(status == BT_STATUS_SUCCESS, status, ("BtlBiprspCompleteAcceptedInitiatorRequest Failed (%s)", pBT_Status(status)));
	
                /* Make  note + exit function. */
                BTL_ERR(status, ("Failed to open file.%s", responseObj->objectName));
            }
	}

	status = BtlBiprspCompleteAcceptedInitiatorRequest(bipContext);
	BTL_VERIFY_ERR(status == BT_STATUS_SUCCESS, status, ("BtlBiprspCompleteAcceptedInitiatorRequest Failed (%s)", pBT_Status(status)));
    }
    
    /* Not accepted! --> refuse it with reason OBRC_FORBIDDEN*/
    else
    {
		status = BtlBiprspCompleteAbortedInitiatorRequest(bipContext);
		BTL_VERIFY_ERR(status == BT_STATUS_SUCCESS, status, ("BtlBiprspCompleteAcceptedInitiatorRequest Failed (%s)", pBT_Status(status)));
    }
    
    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_RespondGetImagesList()
 */
BtStatus BTL_BIPRSP_RespondGetImagesList(BtlBiprspContext *bipContext,
                                          BOOL                    accept,
                                          U16                     nbOfReturnedHandles,
                                          BtlBipFilter        *filter,
                                          BtlObject              *responseObj)
{
/*gesl: TODO: 'filter' param is  not used.*/
    BtStatus status = BT_STATUS_SUCCESS;

    BTL_FUNC_START_AND_LOCK("BTL_BIPRSP_RespondGetImagesList");

    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));
	BTL_VERIFY_ERR((filter == NULL), BT_STATUS_NOT_SUPPORTED, ("Filter must be NULL (Not supported at the moment)"));
	
    /* Accepted? --> respond capabilities */
    if (TRUE == accept)
    {
        bipContext->bipData.bipOp = BIPOP_GET_IMAGES_LIST;
	 bipContext->bipData.ocx = &bipContext->objStore;

        /* Support for memory destination only. */
	if (responseObj->objectLocation == BTL_OBJECT_LOCATION_MEM)
	{
      		bipContext->objStore.type = BTL_BIP_OBJ_STORE_TYPEMEMPTR;
	      bipContext->objStore.location.mem = (U8*)responseObj->location.memLocation.memAddr;
		bipContext->bipData.totLen = (U16)responseObj->location.memLocation.size;
	}
	else
	{
            BTL_VERIFY_ERR((NULL != responseObj->location.fsLocation.fsPath), BT_STATUS_INVALID_PARM, ("Empty path in object"));

            bipContext->objStore.type = BTL_BIP_OBJ_STORE_TYPEHANDLE;

            /* Open the file for reading. */
            status =  btl_bip_FileOpen(FALSE,
                               				responseObj->location.fsLocation.fsPath,
                               				NULL,
                               				&(bipContext->objStore.location.fileHandle),
                               				&bipContext->bipData);

            /* Failed to open the indicated file? --> abort the OBEX session + make a note. */
            if (BT_STATUS_SUCCESS != status)
            {
			status = BtlBiprspCompleteAbortedInitiatorRequest(bipContext);
			BTL_VERIFY_ERR(status == BT_STATUS_SUCCESS, status, ("BtlBiprspCompleteAcceptedInitiatorRequest Failed (%s)", pBT_Status(status)));

                /* Make  note + exit function. */
                BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed to open file.%s", responseObj->objectName));
            }
	}
	
        bipContext->bipData.ocx = &bipContext->objStore;
/*gesl: QUESTION: should length of nbOfReturnedHandles not be included? totLen = total object length?*/

/*    bipContext->bipData.r.rList.handlesDescriptor    = (U8*)xmlDescription;*/
/*    bipContext->bipData.r.rList.handlesDescriptorLen = OS_StrLen(xmlDescription);*/
/*gesl: code above replaced by next code (ik snap er geen zak van)*/
/*gesl: INVESTIGATE: is this correct? --> debugging*/

	    /* Create XML request. */
	    btl_bip_XmlBuildDescrImageHandles((S8*)bipContext->xmlDescr, filter);

	    bipContext->bipData.r.rList.handlesDescriptor = bipContext->xmlDescr;
	    bipContext->bipData.r.rList.handlesDescriptorLen = OS_StrLenUtf8(bipContext->xmlDescr);
	
        bipContext->bipData.r.rList.nbReturnedHandles    = nbOfReturnedHandles;
       
	status = BtlBiprspCompleteAcceptedInitiatorRequest(bipContext);
	BTL_VERIFY_ERR(status == BT_STATUS_SUCCESS, status, ("BtlBiprspCompleteAcceptedInitiatorRequest Failed (%s)", pBT_Status(status)));
    }
    
    /* Not accepted! --> refuse it with reason OBRC_FORBIDDEN*/
    else
    {
		status = BtlBiprspCompleteAbortedInitiatorRequest(bipContext);
		BTL_VERIFY_ERR(status == BT_STATUS_SUCCESS, status, ("BtlBiprspCompleteAcceptedInitiatorRequest Failed (%s)", pBT_Status(status)));
    }

    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_RespondGetImageProperties()
 */
BtStatus BTL_BIPRSP_RespondGetImageProperties(BtlBiprspContext *bipContext,
                                               BOOL                    accept,
                                               BtlObject              *responseObj)
{
    BtStatus status = BT_STATUS_SUCCESS;

    BTL_FUNC_START_AND_LOCK("BTL_BIPRSP_RespondGetImageProperties");

    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));

    /* Accepted? --> respond capabilities */
    if (TRUE == accept)
    {
        bipContext->bipData.bipOp = BIPOP_GET_IMAGE_PROPERTIES;
        bipContext->bipData.ocx = &bipContext->objStore;

        /* Support for memory destination only. */
	if (responseObj->objectLocation == BTL_OBJECT_LOCATION_MEM)
	{
      		bipContext->objStore.type = BTL_BIP_OBJ_STORE_TYPEMEMPTR;
	      bipContext->objStore.location.mem = (U8*)responseObj->location.memLocation.memAddr;
		bipContext->bipData.totLen = (U16)responseObj->location.memLocation.size;
	}
	else
	{
            BTL_VERIFY_ERR((NULL != responseObj->location.fsLocation.fsPath), BT_STATUS_INVALID_PARM, ("Empty path in object"));

            bipContext->objStore.type = BTL_BIP_OBJ_STORE_TYPEHANDLE;

            /* Open the file for reading. */
            status =  btl_bip_FileOpen(FALSE,
			                               responseObj->location.fsLocation.fsPath,
			                               NULL,
                   				            &(bipContext->objStore.location.fileHandle),
			                               &bipContext->bipData);

            /* Failed to open the indicated file? --> abort the OBEX session + make a note. */
            if (BT_STATUS_SUCCESS != status)
            {
			status = BtlBiprspCompleteAbortedInitiatorRequest(bipContext);
			BTL_VERIFY_ERR(status == BT_STATUS_SUCCESS, status, ("BtlBiprspCompleteAcceptedInitiatorRequest Failed (%s)", pBT_Status(status)));

                /* Make  note + exit function. */
                BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed to open file.%s", responseObj->objectName));
            }
	}

	status = BtlBiprspCompleteAcceptedInitiatorRequest(bipContext);
	BTL_VERIFY_ERR(status == BT_STATUS_SUCCESS, status, ("BtlBiprspCompleteAcceptedInitiatorRequest Failed (%s)", pBT_Status(status)));
    }
    
    /* Not accepted! --> refuse it with reason OBRC_FORBIDDEN*/
    else
    {
		status = BtlBiprspCompleteAbortedInitiatorRequest(bipContext);
		BTL_VERIFY_ERR(status == BT_STATUS_SUCCESS, status, ("BtlBiprspCompleteAcceptedInitiatorRequest Failed (%s)", pBT_Status(status)));
    }

    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_RespondGetImage()
 */
BtStatus BTL_BIPRSP_RespondGetImage(BtlBiprspContext *bipContext,
                                     BOOL                    accept,
                                     BtlObject              *responseObj)
{
    BtStatus   status = BT_STATUS_SUCCESS;
    BtlBipObjStore *objStore;

    BTL_FUNC_START_AND_LOCK("BTL_BIPRSP_RespondGetImage");

    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));

    /* Accepted? --> respond capabilities */
    if (TRUE == accept)
    {
        BTL_VERIFY_ERR((NULL != responseObj), BT_STATUS_INVALID_PARM, ("Null objToSend"));

        bipContext->bipData.bipOp = BIPOP_GET_IMAGE;
        
        if( BTL_OBJECT_LOCATION_FS == responseObj->objectLocation )
        {
            BTL_VERIFY_ERR((NULL != responseObj->location.fsLocation.fsPath), BT_STATUS_INVALID_PARM, ("Empty path in object"));

            objStore = &bipContext->objStore;
            bipContext->bipData.ocx = objStore;
            objStore->type = BTL_BIP_OBJ_STORE_TYPEHANDLE;

            /* Open the file for reading. */
            status =  btl_bip_FileOpen(FALSE,
                               				responseObj->location.fsLocation.fsPath,
			                   			NULL,
                   				          	&(bipContext->objStore.location.fileHandle),
                              				&bipContext->bipData);

            /* Failed to open the indicated file? --> abort the OBEX session + make a note. */
            if (BT_STATUS_SUCCESS != status)
            {
		status = BtlBiprspCompleteAbortedInitiatorRequest(bipContext);
		BTL_VERIFY_ERR(status == BT_STATUS_SUCCESS, status, ("BtlBiprspCompleteAcceptedInitiatorRequest Failed (%s)", pBT_Status(status)));

                /* Make  note + exit function. */
                BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed to open file.%s", responseObj->objectName));
            }
        }

        else if( BTL_OBJECT_LOCATION_MEM == responseObj->objectLocation )
        {
            objStore = &bipContext->objStore;
            bipContext->bipData.ocx = objStore;
            objStore->type = BTL_BIP_OBJ_STORE_TYPEMEMPTR;
            objStore->location.mem = responseObj->location.memLocation.memAddr;
            bipContext->bipData.totLen = responseObj->location.memLocation.size;
        }
        else
        {
            return BT_STATUS_INVALID_PARM;
        }

	status = BtlBiprspCompleteAcceptedInitiatorRequest(bipContext);
	BTL_VERIFY_ERR(status == BT_STATUS_SUCCESS, status, ("BtlBiprspCompleteAcceptedInitiatorRequest Failed (%s)", pBT_Status(status)));
    }
    
    /* Not accepted! --> refuse it with reason OBRC_FORBIDDEN*/
    else
    {
		status = BtlBiprspCompleteAbortedInitiatorRequest(bipContext);
		BTL_VERIFY_ERR(status == BT_STATUS_SUCCESS, status, ("BtlBiprspCompleteAcceptedInitiatorRequest Failed (%s)", pBT_Status(status)));
    }

    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_RespondGetLinkedThumbnail()
 */
BtStatus BTL_BIPRSP_RespondGetLinkedThumbnail(BtlBiprspContext *bipContext,
                                               BOOL                    accept,
                                               BtlObject              *responseObj)
{
    BtStatus status = BT_STATUS_SUCCESS;
    BtlBipObjStore *objStore;

    BTL_FUNC_START_AND_LOCK("BTL_BIPRSP_RespondGetLinkedThumbnail");

    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));

    /* Accepted? --> respond capabilities */
    if (TRUE == accept)
    {
        BTL_VERIFY_ERR((NULL != responseObj), BT_STATUS_INVALID_PARM, ("Null responseObj"));

        bipContext->bipData.bipOp = BIPOP_GET_LINKED_THUMBNAIL;

        if( BTL_OBJECT_LOCATION_FS == responseObj->objectLocation )
        {
            BTL_VERIFY_ERR((NULL != responseObj->location.fsLocation.fsPath), BT_STATUS_INVALID_PARM, ("Empty path in object"));

            objStore = &bipContext->objStore;
            bipContext->bipData.ocx = objStore;
            bipContext->bipData.bipOp = BIPOP_GET_LINKED_THUMBNAIL;
            objStore->type = BTL_BIP_OBJ_STORE_TYPEHANDLE;

            /* Open the file for reading. */
            status =  btl_bip_FileOpen(FALSE,
                               responseObj->location.fsLocation.fsPath,
                               NULL,
                               &(bipContext->objStore.location.fileHandle),
                               &bipContext->bipData);

            /* Failed to open the indicated file? --> abort the OBEX session + make a note. */
            if (BT_STATUS_SUCCESS != status)
            {
		status = BtlBiprspCompleteAbortedInitiatorRequest(bipContext);
		BTL_VERIFY_ERR(status == BT_STATUS_SUCCESS, status, ("BtlBiprspCompleteAcceptedInitiatorRequest Failed (%s)", pBT_Status(status)));

                /* Make  note + exit function. */
                BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed to open file.%s", responseObj->objectName));
            }
        }

        else if( BTL_OBJECT_LOCATION_MEM == responseObj->objectLocation )
        {
            objStore = &bipContext->objStore;
            bipContext->bipData.ocx = objStore;
            bipContext->bipData.bipOp = BIPOP_GET_LINKED_THUMBNAIL;
            objStore->type = BTL_BIP_OBJ_STORE_TYPEMEMPTR;
            objStore->location.mem = responseObj->location.memLocation.memAddr;
            bipContext->bipData.totLen = responseObj->location.memLocation.size;
        }

        else
        {
            return BT_STATUS_INVALID_PARM;
        }

	status = BtlBiprspCompleteAcceptedInitiatorRequest(bipContext);
	BTL_VERIFY_ERR(status == BT_STATUS_SUCCESS, status, ("BtlBiprspCompleteAcceptedInitiatorRequest Failed (%s)", pBT_Status(status)));
    }
    
    /* Not accepted! --> refuse it with reason OBRC_FORBIDDEN*/
    else
    {
		status = BtlBiprspCompleteAbortedInitiatorRequest(bipContext);
		BTL_VERIFY_ERR(status == BT_STATUS_SUCCESS, status, ("BtlBiprspCompleteAcceptedInitiatorRequest Failed (%s)", pBT_Status(status)));
    }

    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_RespondGetMonitoringImage()
 */
BtStatus BTL_BIPRSP_RespondGetMonitoringImage(BtlBiprspContext *bipContext,
                                                    BOOL accept,
                                                    BipImgHandle imgHandle,
                                                    BtlObject *responseObj)
{
    BtStatus status = BT_STATUS_SUCCESS;
    BtlBipObjStore *objStore;

    BTL_FUNC_START_AND_LOCK("BTL_BIPRSP_RespondGetMonitoringImage");
	
    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));

    /* Accepted? --> respond capabilities */
    if (TRUE == accept)
    {
        BTL_VERIFY_ERR((NULL != responseObj), BT_STATUS_INVALID_PARM, ("Null responseObj"));
          
        bipContext->bipData.bipOp = BIPOP_GET_MONITORING_IMAGE;

	if (imgHandle != NULL)
	{
		/* Store the imgHandle in the reply. */
	    OS_StrCpyUtf8(bipContext->bipData.r.rImg.imgHandle, imgHandle);

		bipContext->bipData.r.rImg.noImgHandle = FALSE;
	}
	else
	{
		bipContext->bipData.r.rImg.imgHandle[0] = (U8)'\0';
		bipContext->bipData.r.rImg.noImgHandle = TRUE;
	}
	
        if( BTL_OBJECT_LOCATION_FS == responseObj->objectLocation )
        {
            BTL_VERIFY_ERR((NULL != responseObj->location.fsLocation.fsPath), BT_STATUS_INVALID_PARM, ("Empty path in object"));

            objStore = &bipContext->objStore;
            bipContext->bipData.ocx = objStore;
            bipContext->bipData.bipOp = BIPOP_GET_MONITORING_IMAGE;
            objStore->type = BTL_BIP_OBJ_STORE_TYPEHANDLE;

            /* Open the file for reading. */
            status =  btl_bip_FileOpen(FALSE,
				                          responseObj->location.fsLocation.fsPath,
								NULL,
                               				&(bipContext->objStore.location.fileHandle),
								&bipContext->bipData);

            /* Failed to open the indicated file? --> abort the OBEX session + make a note. */
            if (BT_STATUS_SUCCESS != status)
            {
		status = BtlBiprspCompleteAbortedInitiatorRequest(bipContext);
		BTL_VERIFY_ERR(status == BT_STATUS_SUCCESS, status, ("BtlBiprspCompleteAcceptedInitiatorRequest Failed (%s)", pBT_Status(status)));

                /* Make  note + exit function. */
                BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed to open file.%s", responseObj->objectName));
            }
        }

        else if( BTL_OBJECT_LOCATION_MEM == responseObj->objectLocation )
        {
            objStore = &bipContext->objStore;
            bipContext->bipData.ocx = objStore;
            bipContext->bipData.bipOp = BIPOP_GET_MONITORING_IMAGE;
					
            objStore->type = BTL_BIP_OBJ_STORE_TYPEMEMPTR;
            objStore->location.mem = responseObj->location.memLocation.memAddr;
            bipContext->bipData.totLen = responseObj->location.memLocation.size;
        }

        else
        {
            return BT_STATUS_INVALID_PARM;
        }

	status = BtlBiprspCompleteAcceptedInitiatorRequest(bipContext);
	BTL_VERIFY_ERR(status == BT_STATUS_SUCCESS, status, ("BtlBiprspCompleteAcceptedInitiatorRequest Failed (%s)", pBT_Status(status)));
    }
    
    /* Not accepted! --> refuse it with reason OBRC_FORBIDDEN*/
    else
    {
		status = BtlBiprspCompleteAbortedInitiatorRequest(bipContext);
		BTL_VERIFY_ERR(status == BT_STATUS_SUCCESS, status, ("BtlBiprspCompleteAcceptedInitiatorRequest Failed (%s)", pBT_Status(status)));
    }

    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_Abort()
 *
 */
BtStatus BTL_BIPRSP_Abort(BtlBiprspContext *bipContext)
{
    BtStatus  status = BT_STATUS_SUCCESS;

    BTL_FUNC_START_AND_LOCK("BTL_BIPRSP_Abort");

    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));

/*    BIP_Abort(&bipContext->sessionResponder, BIPCH_RESPONDER_PRIMARY);*/
    BIP_Abort(&bipContext->sessionResponder, bipContext->bipObexServer->channel);

    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*-------------------------------------------------------------------------*
 *  EXPORTED XML BUILD FUNCTIONS
 *-------------------------------------------------------------------------*/
/*gesl: TODO: move this functionality to btl_bip_xml */

/*gesl: some boundary check functionality below. Not introduced yet.*/
/*gesl: QUESTION: Add these kind of checks?*/
static void btl_bip_XmlStrCat(U8 *dest, U8 *src, U16 *len, U16 maxLen);
static void btl_bip_XmlStrCatCheck(U8 *dest, U8 *src, U16 *len, U16 maxLen);
static void btl_bip_XmlStrCatNoCheck(U8 *dest, U8 *src, U16 *len, U16 maxLen);

/* Some debug logging features. */
#if (XA_DEBUG == XA_ENABLED) || (XA_DEBUG_PRINT == XA_ENABLED)
#define btl_bip_XmlStrCat(dest, src, len, maxLen) btl_bip_XmlStrCatCheck(dest, src, len, maxLen)
#else
/* Debugging disabled --> exclude check on memory boundary */
#define btl_bip_XmlStrCat(dest, src, len, maxLen) btl_bip_XmlStrCatNoCheck(dest, src, len, maxLen)
/* Next option is also possible */
/* #define btl_bip_XmlStrCat(dest, src, len, maxLen) OS_StrCat(dest, src) */
#endif /* (XA_DEBUG == XA_ENABLED) || (XA_DEBUG_PRINT == XA_ENABLED) */

#if 0
/*------------------------------------------------------------------------------
 * btl_bip_XmlStrCatCheck()
 *
 *    This routine executes an OS_StrCat operation with boundary check
 *    When len returns 0: memory overflow detected.
 */
static void btl_bip_XmlStrCatCheck(U8 *dest, U8 *src, U16 *len, U16 maxLen)
{
   U16 srcLen;

   /* What's the length of the string to be added? */
   srcLen = OS_StrLen((char*)src);
   
   /* src will not fit in remaining part of strDest? --> refuse + reply error*/
   if (srcLen > (maxLen - *len))
   {
      /* reply overflow. */
      *len = 0;
      /* Display error message. */
   }
   else
   {
       OS_StrCat((char*)dest, (char*)src);
       *len = (U16) (*len + srcLen);
   }
}

/*------------------------------------------------------------------------------
 * btl_bip_XmlStrCatNoCheck()
 *
 *    This routine executes an OS_StrCat operation without boundary check
 *    This will execute a bit faster.
 */
static void btl_bip_XmlStrCatNoCheck(U8 *dest, U8 *src, U16 *len, U16 maxLen)
{
	UNUSED_PARAMETER(maxLen);
	
    OS_StrCat((char*)dest, (char*)src);
    *len = (U16) (*len + OS_StrLen((char*)src));
}
#endif

/*------------------------------------------------------------------------------
 * BTL_BIPRSP_BuildPropertiesStart()
 */
BtStatus BTL_BIPRSP_BuildPropertiesStart(BtlBiprspContext *bipContext,
                                               U8 *xmlObj,
                                               U16 xmlObjMaxLen,
                                               BipImgHandle imgHandle,
                                               BtlUtf8 *friendlyName)                                                  
{
    BtStatus status = BT_STATUS_SUCCESS;

    BTL_FUNC_START_AND_LOCK("BTL_BIPRSP_BuildPropertiesStart");

    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));
    BTL_VERIFY_ERR((NULL != xmlObj), BT_STATUS_INVALID_PARM, ("Null xmlObj"));
    BTL_VERIFY_ERR((NULL == bipContext->xmlObj), BT_STATUS_FAILED, ("Another XML build session active."));
    BTL_VERIFY_ERR((NULL != imgHandle), BT_STATUS_INVALID_PARM, ("Null imgHandle"));

    /* Init XML build admin in the Context admin. */
    bipContext->xmlObj       = xmlObj;
    bipContext->xmlObjMaxLen = xmlObjMaxLen;
    bipContext->xmlObjLen    = 0;

    /* Create start of object.*/
    OS_StrCpy((char*)xmlObj, BTL_BIP_IMAGE_PROPERTIES_START);

    /* Add imgHandle attribute. */
    OS_StrCat((char*)xmlObj, BTL_BIP_IMAGE_PROPERTIES_HANDLE);
    OS_StrCat((char*)xmlObj, (char*)imgHandle);
    /* Close this attribute */
    OS_StrCat((char*)xmlObj, "\"");

    /* friendlyName present? --> add this attribute as well */
    if (NULL != friendlyName)
    {
        OS_StrCat((char*)xmlObj, BTL_BIP_IMAGE_PROPERTIES_FRIENDLY_NAME);
        OS_StrCat((char*)xmlObj, (char*)friendlyName);
        /* Close this attribute */
        OS_StrCat((char*)xmlObj, "\"");
    }

    /* Close this element.. */
    OS_StrCat((char*)xmlObj, XML_NON_EMPTY_ELEMENT_END );
    
    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_BuildPropertiesAddNativeEncoding()
 */
BtStatus BTL_BIPRSP_BuildPropertiesAddNativeEncoding(BtlBiprspContext *bipContext,
                                                  BtlBipNativeEncoding *encoding)
{
    BtStatus  status = BT_STATUS_SUCCESS;
    U8       *xmlObj;
    S8        str[20];
    
    BTL_FUNC_START_AND_LOCK("BTL_BIPRSP_BuildPropertiesAddNativeEncoding");

    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));
    BTL_VERIFY_ERR((NULL != encoding), BT_STATUS_INVALID_PARM, ("Null encoding"));
    BTL_VERIFY_ERR((NULL != bipContext->xmlObj), BT_STATUS_FAILED, ("Build session not started before."));

    /* Let xmlObj point to end of XML string under construction for faster StrCat execution.*/
    xmlObj = &(bipContext->xmlObj[bipContext->xmlObjLen]);

    /* Start native encoding element. */
    OS_StrCat((char*)xmlObj, BTL_BIP_IMAGE_PROPERTIES_NENC_ENC);

    /* Add 'encoding' attribute */
    btl_bip_GetBipEncodingString( encoding->encoding, str);
    OS_StrCat((char*)xmlObj, (char*)str );
    OS_StrCat((char*)xmlObj, "\" "); /* Close this attribute */

    /* Add 'pixel' attribute. */
    OS_StrCat((char*)xmlObj, XML_ATTR_PIXEL);
    btl_bip_XmlBuildAttrPixel((S8*)xmlObj,&encoding->pixel);
    OS_StrCat((char*)xmlObj, "\" "); /* Close this attribute */

    /* size attribute present? --> add it*/
    if (0 != encoding->size)
    {
        OS_StrCat((char*)xmlObj, XML_ATTR_SIZE);
        bip_itoa( encoding->size, str, FALSE, 0 );
        OS_StrCat((char*)xmlObj, (char*)str );
        OS_StrCat((char*)xmlObj, "\""); /* Close this attribute */
    }

    /* Close this element. */
    OS_StrCat((char*)bipContext->xmlObj, XML_ELEMENT_END);

    BTL_FUNC_END_AND_UNLOCK();

    return status;
}


/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_BuildPropertiesAddVariantEncoding()
 */
BtStatus BTL_BIPRSP_BuildPropertiesAddVariantEncoding(BtlBiprspContext *bipContext,
                                                  BtlBipVariantEncoding *encoding)
{
    BtStatus  status = BT_STATUS_SUCCESS;
    U8       *xmlObj;
    S8        str[20];
    
    BTL_FUNC_START_AND_LOCK("BTL_BIPRSP_BuildPropertiesAddVariantEncoding");

    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));
    BTL_VERIFY_ERR((NULL != encoding), BT_STATUS_INVALID_PARM, ("Null encoding"));
    BTL_VERIFY_ERR((NULL != bipContext->xmlObj), BT_STATUS_FAILED, ("Build session not started before."));

    /* Let xmlObj point to end of XML string under construction for faster StrCat execution.*/
    xmlObj = &(bipContext->xmlObj[bipContext->xmlObjLen]);

    /* Start native encoding element. */
    OS_StrCat((char*)xmlObj, BTL_BIP_IMAGE_PROPERTIES_VENC_ENC);

    /* Add 'encoding' attribute */
    btl_bip_GetBipEncodingString( encoding->encoding, str);
    OS_StrCat((char*)xmlObj, (char*)str );
    OS_StrCat((char*)xmlObj, "\" "); /* Close this attribute */

    /* Add 'pixel' attribute. */
    OS_StrCat((char*)xmlObj, XML_ATTR_PIXEL);
    btl_bip_XmlBuildAttrPixel((S8*)xmlObj,&encoding->pixel);
    OS_StrCat((char*)xmlObj, "\" "); /* Close this attribute */

    /* 'maxsize' attribute present? --> add it*/
    if (0 != encoding->maxSize)
    {
        OS_StrCat((char*)xmlObj, XML_ATTR_SIZE);
        bip_itoa(encoding->maxSize, str, FALSE, 0 );
        OS_StrCat((char*)xmlObj, (char*)str );
        OS_StrCat((char*)xmlObj, "\""); /* Close this attribute */
    }

    /* Close this element. */
    OS_StrCat((char*)bipContext->xmlObj, XML_ELEMENT_END);

    BTL_FUNC_END_AND_UNLOCK();

    return status;
}


/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_BuildPropertiesAddAttachment()
 */
BtStatus BTL_BIPRSP_BuildPropertiesAddAttachment(BtlBiprspContext *bipContext,
                                                  BtlBipAttachment *attachment)
{
    BtStatus  status = BT_STATUS_SUCCESS;
    U8       *xmlObj;
    S8        str[20];
    
    BTL_FUNC_START_AND_LOCK("BTL_BIPRSP_BuildPropertiesAddAttachment");

    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));
    BTL_VERIFY_ERR((NULL != attachment), BT_STATUS_INVALID_PARM, ("Null attachment"));
    BTL_VERIFY_ERR((NULL != bipContext->xmlObj), BT_STATUS_FAILED, ("Build session not started before."));

    /* Let xmlObj point to end of XML string under construction for faster StrCat execution.*/
    xmlObj = &(bipContext->xmlObj[bipContext->xmlObjLen]);

    /* Start attachment element. */
    OS_StrCat((char*)xmlObj, BTL_BIP_IMAGE_PROPERTIES_ATTCH_TYPE);

    /* Add (mandatory) 'content-type' attribute */
    OS_StrCat((char*)xmlObj, (char*)attachment->contentType );
    OS_StrCat((char*)xmlObj, "\""); /* Close this attribute */

    /* 'charset' attribute present? --> add it*/
    if (OS_StrLen((char*)attachment->charset) != 0)
    {
        OS_StrCat((char*)xmlObj, XML_ATTR_CHARSET);
        OS_StrCat((char*)xmlObj, (char*)attachment->charset);
        OS_StrCat((char*)xmlObj, "\""); /* Close this attribute */
    }

    /* Add (mandatory) 'name' attribute */
    OS_StrCat((char*)xmlObj, XML_ATTR_NAME);
    OS_StrCat((char*)xmlObj, (char*)attachment->name );
    OS_StrCat((char*)xmlObj, "\""); /* Close this attribute */

    /* 'size' attribute present? --> add it*/
    if (0 != attachment->size)
    {
        OS_StrCat((char*)xmlObj, XML_ATTR_SIZE);
        bip_itoa( attachment->size, str, FALSE, 0 );
        OS_StrCat((char*)xmlObj, (char*)str );
        OS_StrCat((char*)xmlObj, "\""); /* Close this attribute */
    }

    /* 'created' attribute present? --> add it*/
    if (TRUE != attachment->filledCreated)
    {
        OS_StrCat((char*)xmlObj, XML_ATTR_CREATED);
        BTL_UTILS_XmlBuildAttrDateAndTime (&attachment->created, (U8*)str);
        OS_StrCat((char*)xmlObj, (char*)str );
        OS_StrCat((char*)xmlObj, "\""); /* Close this attribute */
    }

    /* 'modified' attribute present? --> add it*/
    if (TRUE != attachment->filledCreated)
    {
        OS_StrCat((char*)xmlObj, XML_ATTR_MODIFIED);
        BTL_UTILS_XmlBuildAttrDateAndTime (&attachment->modified, (U8*)str);
        OS_StrCat((char*)xmlObj, (char*)str );
        OS_StrCat((char*)xmlObj, "\""); /* Close this attribute */
    }

    /* Close this element. */
    OS_StrCat((char*)bipContext->xmlObj, XML_ELEMENT_END);

    BTL_FUNC_END_AND_UNLOCK();

    return status;
}


/*------------------------------------------------------------------------------
 * BTL_BIPRSP_BuildPropertiesFinish()
 */
BtStatus BTL_BIPRSP_BuildPropertiesFinish(BtlBiprspContext *bipContext,
                                                 U8 **xmlObj)
{
    BtStatus status = BT_STATUS_SUCCESS;

    BTL_FUNC_START_AND_LOCK("BTL_BIPRSP_BuildPropertiesFinish");

    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));
    BTL_VERIFY_ERR((NULL != bipContext->xmlObj), BT_STATUS_FAILED, ("Build session not started before."));

    /* return the XML result.*/
    *xmlObj = bipContext->xmlObj;

    /* Close this object. */
    OS_StrCat((char*) *xmlObj, BTL_BIP_IMAGE_PROPERTIES_END);

    /* Clear admin for next XML build. */
    bipContext->xmlObj = NULL;
    
    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_BuildCapabilitiesStart()
 */
BtStatus BTL_BIPRSP_BuildCapabilitiesStart(BtlBiprspContext *bipContext,
                                               U8 *xmlObj,
                                               U16 xmlObjMaxLen,
                                               BtlBipPreferredFormat *preferredFormat)
{
    BtStatus status = BT_STATUS_SUCCESS;
    S8       str[20];

    BTL_FUNC_START_AND_LOCK("BTL_BIPRSP_BuildCapabilitiesStart");

    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));
    BTL_VERIFY_ERR((NULL != xmlObj), BT_STATUS_INVALID_PARM, ("Null xmlObj"));
    BTL_VERIFY_ERR((NULL == bipContext->xmlObj), BT_STATUS_FAILED, ("Another XML build session active."));
 
    /* Init XML build admin in the Context admin. */
    bipContext->xmlObj    = xmlObj;
    bipContext->xmlObjLen = xmlObjMaxLen;
    bipContext->xmlObjLen = 0;

    /* Create start of object.*/
    OS_StrCpy((char*)xmlObj, BTL_BIP_IMAGE_CAPABILITIES_START);

    /* preferredFormat present? --> add this element */
    if (NULL != preferredFormat)
    {
        /* Add 'encoding' attribute*/
        OS_StrCat((char*)xmlObj, BTL_BIP_IMAGE_CAPABILITIES_PREF_ENC);
        btl_bip_GetBipEncodingString(preferredFormat->encoding, str );
        OS_StrCat((char*)xmlObj, (char*)str);
        OS_StrCat((char*)xmlObj, "\""); /* Close this attribute */

	if (preferredFormat->filledPixel == TRUE)
	{
		/* Add 'pixel' attribute. */
		OS_StrCat((char*)xmlObj, " ");
        OS_StrCat((char*)xmlObj, XML_ATTR_PIXEL);
        btl_bip_XmlBuildAttrPixel((S8*)xmlObj,&preferredFormat->pixel);
        OS_StrCat((char*)xmlObj, "\""); /* Close this attribute */
	}
#if 0
        /* Add 'transformation' attribute. */
		OS_StrCat((char*)xmlObj, " ");
        OS_StrCat((char*)xmlObj, XML_ATTR_TRANSFORMATION);
        btl_bip_GetBipEncodingString( preferredFormat->transformation, str);
        OS_StrCat((char*)xmlObj, str );
        OS_StrCat((char*)xmlObj, "\""); /* Close this attribute */

        /* Add 'maxsize' attribute. */
		OS_StrCat((char*)xmlObj, " ");
        OS_StrCat((char*)xmlObj, XML_ATTR_MAXSIZE);
        bip_itoa(preferredFormat->maxSize, str, FALSE, 0);
        OS_StrCat((char*)xmlObj, (char*)str );
        OS_StrCat((char*)xmlObj, "\""); /* Close this attribute */
#endif
    }

    /* Close this element. */
    OS_StrCat((char*)bipContext->xmlObj, XML_ELEMENT_END);

    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_BuildCapabilitiesAddImgFormat()
 */
BtStatus BTL_BIPRSP_BuildCapabilitiesAddImgFormat(BtlBiprspContext *bipContext,
                                                  BtlBipImageFormats *format)
{
    BtStatus  status = BT_STATUS_SUCCESS;
    U8       *xmlObj;
    S8        str[20];
    
    BTL_FUNC_START_AND_LOCK("BTL_BIPRSP_BuildCapabilitiesAddImgFormat");

    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));
    BTL_VERIFY_ERR((NULL != format), BT_STATUS_INVALID_PARM, ("Null image format"));
    BTL_VERIFY_ERR((NULL != bipContext->xmlObj), BT_STATUS_FAILED, ("Build session not started before."));

    /* Let xmlObj point to end of XML string under construction for faster StrCat execution.*/
    xmlObj = &(bipContext->xmlObj[bipContext->xmlObjLen]);

    /* Start image formats encoding element. */
    OS_StrCat((char*)xmlObj, BTL_BIP_IMAGE_CAPABILITIES_IFMT_ENC);

    /* Add (mandatory) 'encoding' attribute */
    btl_bip_GetBipEncodingString(format->encoding, str);
    OS_StrCat((char*)xmlObj, (char*)str);
    OS_StrCat((char*)xmlObj, "\""); /* Close this attribute */

    /* 'pixel' attribute present? --> add it. */
    if (TRUE != format->filledPixel)
    {
    	OS_StrCat((char*)xmlObj, " ");
        OS_StrCat((char*)xmlObj, XML_ATTR_PIXEL);
        btl_bip_XmlBuildAttrPixel((S8*)xmlObj,&format->pixel);
        OS_StrCat((char*)xmlObj, "\""); /* Close this attribute */
    }

    /* 'maxsize' attribute present? --> add it.*/
    if (0 != format->maxSize)
    {
    	OS_StrCat((char*)xmlObj, " ");
        OS_StrCat((char*)xmlObj, XML_ATTR_MAXSIZE);
        bip_itoa( format->maxSize, str, FALSE, 0);
        OS_StrCat((char*)xmlObj, (char*)str );
        OS_StrCat((char*)xmlObj, "\""); /* Close this attribute */
    }

    /* Close this element. */
    OS_StrCat((char*)bipContext->xmlObj, XML_ELEMENT_END);

    BTL_FUNC_END_AND_UNLOCK();

    return status;
}


/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_BuildCapabilitiesAddAttachmentFormat()
 */
BtStatus BTL_BIPRSP_BuildCapabilitiesAddAttachmentFormat(BtlBiprspContext *bipContext,
                                                  BtlBipAttachmentFormats *format)
{
    BtStatus  status = BT_STATUS_SUCCESS;
    U8       *xmlObj;
    
    BTL_FUNC_START_AND_LOCK("BTL_BIPRSP_BuildCapabilitiesAddAttachmentFormat");

    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));
    BTL_VERIFY_ERR((NULL != format), BT_STATUS_INVALID_PARM, ("Null attachment format"));
    BTL_VERIFY_ERR((NULL != bipContext->xmlObj), BT_STATUS_FAILED, ("Build session not started before."));

    /* Let xmlObj point to end of XML string under construction for faster StrCat execution.*/
    xmlObj = &(bipContext->xmlObj[bipContext->xmlObjLen]);

    /* Start image-attachments encoding element. */
    OS_StrCat((char*)xmlObj, BTL_BIP_IMAGE_CAPABILITIES_ATTF_TYPE);

    /* Add (mandatory) 'content-type' attribute */
    OS_StrCat((char*)xmlObj, (char*)format->contentType );
    OS_StrCat((char*)xmlObj, "\""); /* Close this attribute */

    /* 'charset' attribute present? --> add it*/
    if (TRUE == format->filledCharset)
    {
        OS_StrCat((char*)xmlObj, XML_ATTR_CHARSET);
        OS_StrCat((char*)xmlObj, (char*)format->charset);
        OS_StrCat((char*)xmlObj, "\""); /* Close this attribute */
    }

    /* Close this element. */
    OS_StrCat((char*)bipContext->xmlObj, XML_ELEMENT_END);

    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*------------------------------------------------------------------------------
 * BTL_BIPRSP_BuildCapabilitiesFinish()
 */
BtStatus BTL_BIPRSP_BuildCapabilitiesFinish(BtlBiprspContext *bipContext,
                                             BtlBipFilteringParameters *filterParms,
                                             U8 **xmlObj)
{
    BtStatus status = BT_STATUS_SUCCESS;

    BTL_FUNC_START_AND_LOCK("BTL_BIPRSP_BuildCapabilitiesFinish");

    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));
    BTL_VERIFY_ERR((NULL != bipContext->xmlObj), BT_STATUS_FAILED, ("Build session not started before."));

    /* return the XML result.*/
    *xmlObj = bipContext->xmlObj;

    /* filterParms present? --> add this element */
    if (NULL != filterParms)
    {
        /* Start 'filter' element. */
        OS_StrCat((char*)*xmlObj, BTL_BIP_IMAGE_CAPABILITIES_FILTER_CR );

        /* Add 'created' attribute*/
        OS_StrCat((char*)*xmlObj, XML_ATTR_CREATED);
        XmlBuildAttrBool(*xmlObj, filterParms->created);
		OS_StrCat((char*)*xmlObj, " ");

        /* Add 'modified' attribute*/
        OS_StrCat((char*)*xmlObj, XML_ATTR_MODIFIED);
        XmlBuildAttrBool(*xmlObj, filterParms->modified);
		OS_StrCat((char*)*xmlObj, " ");
        
        /* Add 'encoding' attribute*/
        OS_StrCat((char*)*xmlObj, XML_ATTR_ENCODING);
        XmlBuildAttrBool(*xmlObj, filterParms->encoding);
		OS_StrCat((char*)*xmlObj, " ");

        /* Add 'pixel' attribute. */
        OS_StrCat((char*)*xmlObj, XML_ATTR_PIXEL);
        XmlBuildAttrBool(*xmlObj, filterParms->pixel);
		OS_StrCat((char*)*xmlObj, " ");

        /* Close this element.. */
        OS_StrCat((char*)*xmlObj, XML_ELEMENT_END );
    }

    /* Close this object.. */
    OS_StrCat((char*)*xmlObj, BTL_BIP_IMAGE_CAPABILITIES_END);

    /* Clear admin for next XML build. */
    bipContext->xmlObj = NULL;
    
    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_BuildListingStart()
 */
BtStatus BTL_BIPRSP_BuildListingStart(BtlBiprspContext *bipContext,
                                        U8 *xmlObj,
                                        U16 xmlObjMaxLen)
{
    BtStatus status = BT_STATUS_SUCCESS;

    BTL_FUNC_START_AND_LOCK("BTL_BIPRSP_BuildListingStart");

    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));
    BTL_VERIFY_ERR((NULL != xmlObj), BT_STATUS_INVALID_PARM, ("Null xmlObj"));
    BTL_VERIFY_ERR((NULL == bipContext->xmlObj), BT_STATUS_FAILED, ("Another XML build session active."));
 
    /* Init XML build admin in the Context admin. */
    bipContext->xmlObj    = xmlObj;
    bipContext->xmlObjLen = xmlObjMaxLen;
    bipContext->xmlObjLen = 0;

    /* Create start of object.*/
    OS_StrCpy((char*)xmlObj, XML_IMAGELIST_START);
   
    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*------------------------------------------------------------------------------
 * BTL_BIPRSP_BuildListingAddImg()
 */
BtStatus BTL_BIPRSP_BuildListingAddImg(BtlBiprspContext *bipContext,
                                           BipImgHandle      imgHandle,
                                           BtlDateAndTimeStruct *created,
                                           BtlDateAndTimeStruct *modified)
{
    BtStatus  status = BT_STATUS_SUCCESS;
    U8       *xmlObj;
    S8        str[20];
    
    BTL_FUNC_START_AND_LOCK("BTL_BIPRSP_BuildListingAddImg");

    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));
    BTL_VERIFY_ERR((NULL != imgHandle), BT_STATUS_INVALID_PARM, ("Null imgHandle"));
    BTL_VERIFY_ERR((NULL != bipContext->xmlObj), BT_STATUS_FAILED, ("Build session not started before."));

    /* Let xmlObj point to end of XML string under construction for faster StrCat execution.*/
    xmlObj = &(bipContext->xmlObj[bipContext->xmlObjLen]);

    /* Start (mandatory) 'handle' element. */
    OS_StrCat((char*)xmlObj, XML_IMAGELIST_ENTRY1);
    OS_StrCat((char*)xmlObj, (char*)imgHandle);
    OS_StrCat((char*)xmlObj, "\""); /* Close this attribute */

    /* 'created' attribute present? --> add it*/
    if (NULL != created)
    {
        OS_StrCat((char*)xmlObj, XML_ATTR_CREATED);
        BTL_UTILS_XmlBuildAttrDateAndTime (created, (U8*)str);
        OS_StrCat((char*)xmlObj, (char*)str );
        OS_StrCat((char*)xmlObj, "\""); /* Close this attribute */
    }

    /* 'modified' attribute present? --> add it*/
    if (NULL != modified)
    {
        OS_StrCat((char*)xmlObj, XML_ATTR_MODIFIED);
        BTL_UTILS_XmlBuildAttrDateAndTime (modified, (U8*)str);
        OS_StrCat((char*)xmlObj, (char*)str );
        OS_StrCat((char*)xmlObj, "\""); /* Close this attribute */
    }

    /* Close this element. */
    OS_StrCat((char*)bipContext->xmlObj, XML_ELEMENT_END);

    BTL_FUNC_END_AND_UNLOCK();

    return status;
}

/*------------------------------------------------------------------------------
 * BTL_BIPRSP_BuildListingFinish()
 */
BtStatus BTL_BIPRSP_BuildListingFinish(BtlBiprspContext *bipContext,
                                                 U8 **xmlObj)
{
    BtStatus status = BT_STATUS_SUCCESS;

    BTL_FUNC_START_AND_LOCK("BTL_BIPRSP_BuildListingFinish");

    BTL_VERIFY_ERR((NULL != bipContext), BT_STATUS_INVALID_PARM, ("Null bipContext"));
    BTL_VERIFY_ERR((NULL != bipContext->xmlObj), BT_STATUS_FAILED, ("Build session not started before."));

    /* return the XML result.*/
    *xmlObj = bipContext->xmlObj;

    /* Close this object.. */
    OS_StrCat((char*)*xmlObj, XML_IMAGELIST_END);

    /* Clear admin for next XML build. */
    bipContext->xmlObj = NULL;
    
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
 * Synopsis:  Callback function for BIP Responder Events
 *
 * Return:    void
 *
 */
void BiprspCallback(BipCallbackParms *parms)
{
    BOOL            passEventToApp = FALSE;
    BtlBipObjStore *objStore;
    BtlBiprspEvent  bipEvent;
    U8             *pData;
    BtlBipChannel  *channelAdmin;

    BtlBiprspContext *bipContext = GetContextFromBipCallbackParms(parms);
	BOOL			success;
/*gesl: TODO: check on NULL*/

    BTL_FUNC_START("BiprspCallback");

    bipEvent.context = bipContext;
    bipEvent.parms   = parms;
    
/*gesl: REMARK: next check on NULL removed, because it cannot make sense; it is never reste to NULL.*/
/*    if( NULL == bipContext->bipObexServer)*/
        bipContext->bipObexServer = parms->obex.server;
/*gesl: QUESTION: Why is this done? --> add good comment.*/
    
    objStore = (BtlBipObjStore*)(parms->data->ocx);
/*gesl: TODO: add comments that parms->obex.server identifies an individual connection +*/

/*gesl: REMARK: bipContext->state is never used.*/

	BTL_LOG_INFO(("BiprspCallback: Event: %s, Op: %s", 
					btl_bip_GetBipEventString(parms->event), btl_bip_GetBipOpString(parms->data->bipOp)));

    switch(parms->event)
    {
        case BIPRESPONDER_DATA_REQ:
			
		BtlBiprspSendProgressNotificationToApp(bipContext,  parms, parms->data->offset + parms->data->len);
		
		pData = getDataPtrFromBipDataOp(parms->data);

		success = FALSE;
		
		if( (NULL != objStore) && (NULL != pData) )
		{
			if (BTL_BIP_OBJ_STORE_TYPEHANDLE == objStore->type)
			{
				if (BTHAL_FS_INVALID_FILE_DESC != objStore->location.fileHandle)
				{
					/* Error detected while reading from the file? --> abort the OBEX session.*/
					if( BT_STATUS_SUCCESS == btl_bip_DataRead(parms->data) )
					{
						success = TRUE;
					}
				}
				else
				{
					BTL_LOG_ERROR(("BiprspCallback: Invalid objStore->location.fileHandle"));
				}
			} 
			else if (BTL_BIP_OBJ_STORE_TYPEMEMPTR == objStore->type)
			{
				if (NULL != objStore->location.mem)
				{
					success = TRUE;
					OS_MemCopy(pData,&((char*)objStore->location.mem)[parms->data->offset],parms->data->len);
				}
				else
				{
					BTL_LOG_ERROR(("BiprspCallback: Null objStore->location.mem"));
				}
			}
		}

		if (success == FALSE)		
		{
			BTL_LOG_ERROR(("BiprspCallback: Failed handling BIPRESPONDER_DATA_REQ, Aborting operation", pData));
				
			parms->data->len = 0;
			BIP_Abort(&bipContext->sessionResponder, parms->channel);
		}
		
	break;
			
        case BIPRESPONDER_DATA_IND:

		bipContext->objLen += parms->data->len;
		
		BtlBiprspSendProgressNotificationToApp(bipContext,  parms, bipContext->objLen);
		
            /* Set the buff pointer */
            pData = getDataPtrFromBipDataType(parms->data);
            if( NULL == pData )
                BIP_Abort(&bipContext->sessionResponder, parms->channel);

			BTL_LOG_INFO(("Bip Data Type: %d", parms->data->dataType));
			
            switch(parms->data->dataType)
            {
                case BIPIND_IMAGE:
                case BIPIND_THUMBNAIL:
                    /* Write the image */

                    (void) btl_bip_DataWrite(parms->data);
/*gesl: TODO: add error handling?*/
                    break;

                case BIPIND_HANDLES_DESCRIPTOR:
                case BIPIND_IMAGE_DESCRIPTOR:
                    /* Received an XML response, parse it here */
                    passEventToApp = TRUE;
                    break;

                default:
			BTL_FATAL_NORET(("Invalid Data Type (%d)", parms->data->dataType));
			
                    BIP_Abort(&bipContext->sessionResponder, parms->channel);
                    break;
            }
            break;
        case BIPRESPONDER_REQUEST:

		BtlBiprspInitRequestVars(bipContext);

		bipContext->suspendContinue = TRUE;
		
		if (parms->data->bipOp == BIPOP_PUT_LINKED_THUMBNAIL)
		{
			if (btl_bip_IsHandleValid(parms->data->r.rThm.imgHandle) == FALSE)
			{
				BTL_LOG_ERROR(("Invalid Image Handle (%s) in Put Linked Thumbnail Initiator Request", (char*)parms->data->r.rThm.imgHandle));
				BIP_ResponderAccept(parms->obex.server, OBRC_BAD_REQUEST, parms->data);
				break;
			}

			/* Verify that the handle of the linked thumbnail matches the handle previously allocated to the linked image */
			if (bipContext->imgHandleAssignedToPushedImage[0] != 0)
			{
				if (OS_MemCmp(&bipContext->imgHandleAssignedToPushedImage, 8, parms->data->r.rThm.imgHandle, 8) == FALSE)
				{
					BTL_LOG_ERROR(("Image Handle (%s) in Put Linked Thumbnail Doesn't match the linked thumbnail handle (%s)", 
									(char*)parms->data->r.rThm.imgHandle, (char*)bipContext->imgHandleAssignedToPushedImage));
					BIP_ResponderAccept(parms->obex.server, OBRC_BAD_REQUEST, parms->data);
					break;
				}
			}
		}

		if ((parms->data->bipOp == BIPOP_PUT_LINKED_THUMBNAIL) || (parms->data->bipOp == BIPOP_PUT_IMAGE))
		{
			BtlBiprspHandleTempResponseForPutOperations(bipContext);
		}

            passEventToApp = TRUE;
            bipContext->eventBipData = parms->data;

            break;
        case BIPRESPONDER_COMPLETE:

		if (	(parms->data->bipOp != BIPOP_PUT_IMAGE) || 
			((parms->data->bipOp == BIPOP_PUT_IMAGE) && (bipContext->autoRequestLinkedThumbnail == FALSE)))
		{
			OS_MemSet(bipContext->imgHandleAssignedToPushedImage, 0, 8);
		}
		
            if( (BTL_BIP_OBJ_STORE_TYPEHANDLE == bipContext->objStore.type) &&
                (NULL != bipContext->bipData.ocx) )
            {
                (void) btl_bip_FileClose((BthalFsFileDesc *)&(bipContext->objStore.location.fileHandle));
/*gesl: TODO: Add error handling?*/

                bipContext->bipData.ocx   = NULL;
                bipContext->objStore.type = BTL_BIP_OBJ_STORE_TYPENONE;
            }
/*gesl: add comments what is done here above.            */

            /* Check for new connection. */
            if (BIPOP_CONNECT == parms->data->bipOp)
            {
                /* It was indeed a new connection? --> update admin + notify the APP.*/
                if (parms->status == OB_STATUS_SUCCESS)
                {
                    /* get free channelAdmin.*/
                    channelAdmin = GetFreeChannelAdmin();
                    /* found it? -> initchannel admin */
                    if (NULL != channelAdmin)
                    {
                        SetChannelState(channelAdmin, BTL_BIP_CHANNEL_STATE_CONNECTED);
                        channelAdmin->context = bipContext;
                        channelAdmin->channel = parms->channel;
                        /* Change event to make it clear for the APP. */
                        parms->event = BIPRESPONDER_CONNECTED;
                    }
                    /* Admin full? --> disconnect again */
                    else
                    {
/*gesl: TODO: add nice refusal to remote because I am running out of resources.*/
/*      now the APP simply gets this event.*/
                    }
                }
                /* It was a disconnection? --> update admin + notify the APP.*/
                else if (parms->status == OB_STATUS_DISCONNECT)
                {
                    /* Get correct channelAdmin. */
                    channelAdmin = GetChannelAdmin(parms->channel);
                    /* Update my channel state-machine */
                    SetChannelState(channelAdmin, BTL_BIP_CHANNEL_STATE_DISCONNECTED);
                    channelAdmin->context = NULL;
                    /* Change event to make it clear for the APP. */
                    parms->event = BIPRESPONDER_DISCONNECTED;
                }
            }
		else if ((parms->data->bipOp == BIPOP_PUT_IMAGE) || (parms->data->bipOp == BIPOP_PUT_LINKED_THUMBNAIL))
		{
			if (bipContext->abortState == BTL_BIPRSP_ABORT_STATE_IDLE)
			{
				BtStatus status = BtlBiprspRenameFile(bipContext->pushedFileName, bipContext->desiredPushedFileName);
				BTL_VERIFY_ERR_NORET((status == BT_STATUS_SUCCESS), ("BtlBiprspRenameFile Failed (%s)", pBT_Status(status)));
			}
			
			bipEvent.parms->data->totLen = bipContext->objLen;
			bipContext->objLen = 0;
		}

            passEventToApp = TRUE;
            break;
        case BIPRESPONDER_FAILED:
		OS_MemSet(bipContext->imgHandleAssignedToPushedImage, 0, 8);
            passEventToApp = TRUE;
            break;
        case BIPRESPONDER_ABORT:
			
		OS_MemSet(bipContext->imgHandleAssignedToPushedImage, 0, 8);

		if (bipContext->objStore.type == BTL_BIP_OBJ_STORE_TYPEHANDLE)
		{
	             /* File was open for reading or writing? --> close it. */
	            (void) btl_bip_FileClose((BthalFsFileDesc *)&(bipContext->objStore.location.fileHandle));

			if (bipContext->deleteFileIfAborted == TRUE)
			{
				BTL_LOG_INFO(("Operation Aborted, Deleting File Data That was received so far (%s)", (char*)bipContext->pushedFileName));
				
				btl_bip_FileDelete(bipContext->pushedFileName);
			}
		}

            bipContext->objStore.type = BTL_BIP_OBJ_STORE_TYPENONE;
		bipContext->suspendContinue = FALSE;
		bipContext->continuePending = FALSE;

		bipContext->abortState = BTL_BIPRSP_ABORT_STATE_ABORTED;

            passEventToApp = TRUE;
            break;
        case BIPRESPONDER_CONTINUE:
		if (bipContext->suspendContinue == FALSE)
		{
	            BIP_ResponderContinue(bipContext->bipObexServer);
		}
		else
		{
			bipContext->continuePending = TRUE;			
		}	
		
            break;
        default:
            break;
    }
    
    /* APP should be notified? --> do it. */
    if( TRUE == passEventToApp )
    {
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
static void AfterProcessing(BtlBiprspContext* bipContext, BtlBiprspEvent *bipEvent)
{
    BtStatus status;

    BTL_FUNC_START("AfterProcessing");

    switch(bipEvent->context->state)
    {
    case BTL_BIP_CONTEXT_STATE_DISABLING:
      /* Last disconnect event to the APP during disabling?                        */
      /*  --> notify APP that the disabling is finished + reset the context state. */
      if(bipEvent->parms->event == BIPRESPONDER_DISCONNECTED)
      {
          /* Remove the registration from the BIP profile. */
	    status = BIP_DeregisterResponder(&bipEvent->context->sessionResponder);
          BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Disable of BIP Responder failed: (%s)", pBT_Status(status)));

          SetContextState(bipEvent->context, BTL_BIP_CONTEXT_STATE_IDLE);

          /* Notify APP with this new event. */
          bipEvent->parms->event = BIPRESPONDER_DISABLED;
          bipEvent->context->callback(bipEvent);

		 return;
      }
      break;
    }

	if ((bipEvent->parms->event == BIPRESPONDER_DISCONNECTED) && (bipContext->pendingDisconnection == TRUE))
	{
		bipContext->pendingDisconnection = FALSE;
		
		status = BtlBiprspDisconnect(bipContext);

		if (status == BT_STATUS_SUCCESS)
		{
			bipEvent->parms->event = BIPRESPONDER_DISCONNECTED;
		}
	}

	if (	(bipEvent->parms->event == BIPRESPONDER_DISCONNECTED) && 
		(bipContext->radioOffState != BTL_BIPRSP_RADIO_OFF_STATE_IDLE))
	{
		BtlBiprspContextRadioOffProcessor(bipContext);
	}

    BTL_FUNC_END();
}

/*---------------------------------------------------------------------------
 *            IsConnected
 *---------------------------------------------------------------------------
 *
 * Synopsis:  
 *
 */
static BOOL IsConnected(BtlBiprspContext *bipContext)
{
	UNUSED_PARAMETER(bipContext);

    if( BTL_BIP_CHANNEL_STATE_CONNECTED == GetChannelAdmin(BIPCH_RESPONDER_PRIMARY)->state)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*-------------------------------------------------------------------------------
 * GetContextFromBipCallbackParms()
 */
static BtlBiprspContext *GetContextFromBipCallbackParms(BipCallbackParms* parms)
{
    BtlBiprspContext *bipContext = 0;
    
    BTL_FUNC_START("GetContextFromBipCallbackParms");

    switch (parms->event)
    {
        case BIPRESPONDER_DATA_REQ:
        case BIPRESPONDER_DATA_IND:
        case BIPRESPONDER_REQUEST:
        case BIPRESPONDER_COMPLETE:
        case BIPRESPONDER_FAILED:
        case BIPRESPONDER_ABORT:
        case BIPRESPONDER_CONTINUE:
            /* Subtract the offset of the field BtlBipContext->sessionResponder from the address
             * of parms->obex.client to get the address of BtlBipContext.
             * BtlBipContext->sessionResponder is given as parameter to the BIP_xxx functions which
             * is the parms->obex.server in the callback.
             */
            bipContext = ContainingRecord(parms->obex.server, BtlBiprspContext, sessionResponder);
            break;
        default:
            bipContext = NULL;
            break;
    };

    BTL_FUNC_END();

    return bipContext;
}

/*---------------------------------------------------------------------------
 *            InitOperationVariables
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize some variables
 *
 */
static void InitOperationVariables(BtlBiprspContext* context)
{   
    context->bipData.bipOp                = BIPOP_NOP;
    context->bipData.dataType             = BIPIND_NO_DATA;
    context->bipData.len                  = 0;
    context->bipData.ocx                  = NULL;
    context->bipData.offset               = 0;
    context->bipData.totLen               = 0;
    context->bipObexServer                = NULL;
    context->objStore.type                = BTL_BIP_OBJ_STORE_TYPENONE;
    context->objStore.location.fileHandle = BTHAL_FS_INVALID_FILE_DESC;
    context->autoRequestLinkedThumbnail   = FALSE;
    context->failureReason                = OB_STATUS_SUCCESS;
    context->callback                     = NULL;
    context->eventBipData                 = NULL;
    context->userRequestedAbort           = FALSE;
    context->xmlObj                       = NULL;
    context->xmlObjLen                    = NULL;
}

#ifdef DEBUG
static S8* pInitState(BtlBiprspInitState state)
{
    switch (state)
    {
        case BTL_BIPRSP_INIT_STATE_NOT_INITIALIZED:       return (S8*)"BTL_BIP_INIT_STATE_NOT_INITIALIZED";
        case BTL_BIPRSP_INIT_STATE_INITIALIZED:           return (S8*)"BTL_BIP_INIT_STATE_INITIALIZED";
        case BTL_BIPRSP_INIT_STATE_INITIALIZATION_FAILED: return (S8*)"BTL_BIP_INIT_STATE_INITIALIZATION_FAILED";
        default: return "UNKNOWN";
    };
} 

static S8* pState(BtlBiprspState state)
{
    switch (state)
    {
        case BTL_BIPRSP_STATE_IDLE:         return (S8*)"BTL_BIP_STATE_IDLE";
        case BTL_BIPRSP_STATE_DISCONNECTED: return (S8*)"BTL_BIP_STATE_DISCONNECTED";
        case BTL_BIPRSP_STATE_CONNECTED:    return (S8*)"BTL_BIP_STATE_CONNECTED";
        case BTL_BIPRSP_STATE_DISCONNECTING:return (S8*)"BTL_BIP_STATE_DISCONNECTING";
        case BTL_BIPRSP_STATE_DISABLING:    return (S8*)"BTL_BIP_STATE_DISABLING";
        case BTL_BIPRSP_STATE_CONNECTING:   return (S8*)"BTL_BIP_STATE_CONNECTING";
        default: return "UNKNOWN";
    };
}
#endif

/*gesl: TODO: share function below with INT side: --> 1 implementation*/
static char bip_ntoc( U8 Nr )
{
    S8  c;

    if( Nr > 10 )
        return 0;
        
    c = (S8)(0x30 + Nr);
    return c;
}

/*gesl: TODO: share function below with INT side: --> 1 implementation*/
static char *bip_itoa( U32 Nr, S8 *AddrString, BOOL bZeroFill, U8 nrOfChar)
{
    U8           u, l, t;

    AddrString[U32_DEC_LENGTH] = 0;

    for( u=0; u < U32_DEC_LENGTH; u++ )
    {
        AddrString[u] = bip_ntoc((S8)(Nr%10));
        Nr = Nr / 10;
    }
    u--;
    if( FALSE == bZeroFill )
    {
        while( (AddrString[u] == '0') && u!= 0 )
        {
            AddrString[u] = 0;
            u--;
        }
    }
    else
    {
        if( nrOfChar < U32_DEC_LENGTH )
        {
            AddrString[nrOfChar] = 0;
        }
    }
    l = (U8)(OS_StrLen((char*)AddrString)-1);
    u = 0;
    while( u < l )
    {
        t = AddrString[u];
        AddrString[u] = AddrString[l];
        AddrString[l] = t;
        u++;
        l--;
    }
    return (char*)AddrString;
}

/*---------------------------------------------------------------------------
 *            getDataPtrFromBipDataOp
 *---------------------------------------------------------------------------
 *
 * Synopsis:
 *
 * Return:    Pointer to the data corresponding to the operation
 */
static void *getDataPtrFromBipDataOp(BipData *bipData)
{
    void *p;
    switch(bipData->bipOp)
    {
        case BIPOP_GET_IMAGE:
        case BIPOP_GET_MONITORING_IMAGE:
            p = bipData->r.rImg.buff;
            break;

        case BIPOP_GET_LINKED_THUMBNAIL:
            p = bipData->r.rThm.buff;
            break;

        case BIPOP_GET_CAPABILITIES:
            p = bipData->r.rCapa.buff;
            break;

        case BIPOP_GET_IMAGES_LIST:
            p = bipData->r.rList.buff;
            break;

        case BIPOP_GET_IMAGE_PROPERTIES:
            p = bipData->r.rProp.buff;
            break;
        default:
            p = NULL;
            break;
    }
    return p;    
}

/*---------------------------------------------------------------------------
 *            getDataPtrFromBipDataType
 *---------------------------------------------------------------------------
 *
 * Synopsis:
 *
 * Return:    Pointer to the data corresponding to the datatype
 */
static void *getDataPtrFromBipDataType(BipData *bipData)
{
    void *pData;
    switch (bipData->dataType)
    {
        case BIPIND_IMAGE:
        case BIPOP_GET_MONITORING_IMAGE:
            pData = bipData->i.iImg.buff;
            break;
        case BIPIND_THUMBNAIL:
            pData = bipData->i.iThm.buff;
            break;
        case BIPIND_HANDLES_DESCRIPTOR:
            pData = bipData->i.iHDsc.buff;
            break;
        case BIPIND_IMAGE_DESCRIPTOR:
            pData = bipData->i.iIDsc.buff;
            break;
        case BIPIND_IMG_LISTING_XML:
            pData = bipData->i.iList.buff;
            break;
        case BIPIND_IMG_CAPABILITIES_XML:
            pData = bipData->i.iCapa.buff;
            break;
        case BIPIND_IMG_PROPERTIES_XML:
            pData = bipData->i.iProp.buff;
            break;
        default:
            pData = NULL;
            break;
    }

    return pData;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_Disconnect()
 */
BtStatus BtlBiprspDisconnect(BtlBiprspContext *bipContext)
{

	BtStatus    	status = BT_STATUS_SUCCESS;
	ObStatus 	obStatus;
	BtlBipChannel	*bipChannel = GetChannelAdmin(BIPCH_RESPONDER_PRIMARY);

    BTL_FUNC_START("BtlBiprspDisconnect");
		
	switch (bipChannel->state)
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

	obStatus = GOEP_ServerTpDisconnect(&bipContext->sessionResponder.primary.sApp);
	BTL_VERIFY_FATAL(	(OB_STATUS_PENDING == obStatus) || (OB_STATUS_SUCCESS == obStatus), BT_STATUS_INTERNAL_ERROR, 
						("Unexpected Result (%s) from GOEP_TpDisconnect", BTL_UTILS_ObStatus2BtStatus(obStatus)));

	if (OB_STATUS_SUCCESS == obStatus)
	{
		SetChannelState(bipChannel, BTL_BIP_CHANNEL_STATE_DISCONNECTED);
	}
	else
	{
		/* BIP state is now in the process of disconnecting */
		SetChannelState(bipChannel, BTL_BIP_CHANNEL_STATE_DISCONNECTING);
	}

	status = BTL_UTILS_ObStatus2BtStatus(obStatus);
	
	BTL_FUNC_END();

	return status;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_RadioOffProcessor()
 */
BtStatus BtlBiprspContextRadioOffProcessor(BtlBiprspContext *bipContext)
{
	BtStatus 		status = BT_STATUS_SUCCESS;
	BOOL		keepProcessing = TRUE;
	BtlBipChannel	*bipChannel = GetChannelAdmin(BIPCH_RESPONDER_PRIMARY);
	
	BTL_FUNC_START("BtlBiprspContextRadioOffProcessor");

	while (keepProcessing == TRUE)
	{
		keepProcessing = FALSE;

		switch (bipContext->radioOffState)
		{
			case BTL_BIPRSP_CONTEXT_RADIO_OFF_STATE_IDLE:

				BTL_VERIFY_ERR((BTL_BIP_CONTEXT_STATE_DISABLING != bipContext->state), BT_STATUS_IN_PROGRESS, 
								("Disabling already in progress"));

				bipContext->asynchronousRadioOff = FALSE;
				bipContext->radioOffState = BTL_BIPRSP_CONTEXT_RADIO_OFF_STATE_DISCONNECTING;
				
				status = BtlBiprspDisconnect(bipContext);
				BTL_VERIFY_FATAL((status == BT_STATUS_SUCCESS) || (BT_STATUS_PENDING == status), status, (""));
				
				if (status == BT_STATUS_SUCCESS)
				{
					bipContext->radioOffState = BTL_BIPRSP_CONTEXT_RADIO_OFF_STATE_DONE;
					keepProcessing = TRUE;
				}
				else
				{
					bipContext->asynchronousRadioOff = TRUE;
				}

			break;
			
 	 		case BTL_BIPRSP_CONTEXT_RADIO_OFF_STATE_DISCONNECTING:

				if (bipChannel->state == BTL_BIP_CHANNEL_STATE_DISCONNECTED)
				{				
					bipContext->radioOffState = BTL_BIPRSP_CONTEXT_RADIO_OFF_STATE_DONE;
					keepProcessing = TRUE;
				}

			break;
			 
			case BTL_BIPRSP_CONTEXT_RADIO_OFF_STATE_DONE:
				
				if (bipContext->asynchronousRadioOff == TRUE)
				{
					BtlBiprspRadioOffProcessor(BTL_BIPRSP_RADIO_OFF_EVENT_CONTEXT_COMPLETED);	
				}

				bipContext->radioOffState = BTL_BIPRSP_CONTEXT_RADIO_OFF_STATE_IDLE;
				
			break;

			default:

				BTL_FATAL(BT_STATUS_INTERNAL_ERROR, ("Unexpected Radio Off (%d)", bipContext->radioOffState));
				
		};
	}
	
	BTL_FUNC_END();

	return status;
}

BtStatus BtlBiprspRadioOffProcessor(BtlBiprspRadioOffEvent event)
{
	BtStatus 		status = BT_STATUS_SUCCESS;
	BtlContext	*context = 0;
	BOOL		keepProcessing = FALSE;

	BTL_FUNC_START("BtlBiprspRadioOffProcessor");

	keepProcessing = TRUE;

	while (keepProcessing == TRUE)
	{

		keepProcessing = FALSE;

		switch (btlBipData.radioOffState)
		{
			case BTL_BIPRSP_RADIO_OFF_STATE_IDLE:

				BTL_VERIFY_FATAL(event == BTL_BIPRSP_RADIO_OFF_EVENT_START, BT_STATUS_INTERNAL_ERROR, ("Unexpected Event (%d), event"));

				btlBipData.asynchronousRadioOff = FALSE;
				btlBipData.numOfDisconnectingContexts = 0;
				btlBipData.radioOffState = BTL_BIPRSP_RADIO_OFF_STATE_DISCONNECTING;
				
				IterateList(btlBipData.contextsList, context, BtlContext*)
				{
					status = BtlBiprspContextRadioOffProcessor((BtlBiprspContext*)context);

					BTL_VERIFY_FATAL_NORET((status == BT_STATUS_SUCCESS) || (status == BT_STATUS_PENDING),
												("BtlBiprspContextRadioOffProcessor Returned an unexpected status (%s)", pBT_Status(status)));

					if (status == BT_STATUS_PENDING)
					{
						++btlBipData.numOfDisconnectingContexts;
					}
				}

				if (btlBipData.numOfDisconnectingContexts == 0)
				{
					status = BT_STATUS_SUCCESS;
					btlBipData.radioOffState = BTL_BIPRSP_RADIO_OFF_STATE_DONE;
					keepProcessing = TRUE;
				}
				else
				{
					status = BT_STATUS_PENDING;
					btlBipData.asynchronousRadioOff = TRUE;
				}

			break;

			case BTL_BIPRSP_RADIO_OFF_STATE_DISCONNECTING:

				BTL_VERIFY_FATAL(event == BTL_BIPRSP_RADIO_OFF_EVENT_CONTEXT_COMPLETED, BT_STATUS_INTERNAL_ERROR, ("Unexpected Event (%d), event"));
				BTL_VERIFY_FATAL(btlBipData.numOfDisconnectingContexts > 0, BT_STATUS_INTERNAL_ERROR, ("Not expecting any contexts to disconnect"));

				--btlBipData.numOfDisconnectingContexts;

				if (btlBipData.numOfDisconnectingContexts == 0)
				{
					btlBipData.radioOffState = BTL_BIPRSP_RADIO_OFF_STATE_DONE;
					keepProcessing = TRUE;
				}

			break;
			
			case BTL_BIPRSP_RADIO_OFF_STATE_DONE:

				if (btlBipData.asynchronousRadioOff == TRUE)
				{
					BtlModuleNotificationCompletionEvent	moduleCompletedEvent;
					moduleCompletedEvent.type = BTL_MODULE_NOTIFICATION_COMPLETION_EVENT_TYPE_RADIO_OFF;
					moduleCompletedEvent.status = BT_STATUS_SUCCESS;
						
					 BTL_ModuleCompleted(BTL_MODULE_TYPE_BIPRSP , &moduleCompletedEvent);
				}

				btlBipData.radioOffState = BTL_BIPRSP_RADIO_OFF_STATE_IDLE;

			break;
		};
	};

	BTL_FUNC_END();

	return status;
}

BtStatus BtlBiprspBtlNotificationsCb(BtlModuleNotificationType notificationType)
{
  	BtStatus status = BT_STATUS_SUCCESS;

  	BTL_FUNC_START("BtlBiprspBtlNotificationsCb");
  	
  	BTL_LOG_DEBUG(("Notification Type: %d", notificationType));
  	
  	switch (notificationType)
  	{
    		case BTL_MODULE_NOTIFICATION_RADIO_ON:

      			/* Do something if necessary */
      		
      			break;

    		case BTL_MODULE_NOTIFICATION_RADIO_OFF:

			status = BtlBiprspRadioOffProcessor(BTL_BIPRSP_RADIO_OFF_EVENT_START);
     			
      			break;

    		default:

      			BTL_ERR(BT_STATUS_INTERNAL_ERROR, ("Invalid notification (%d)", notificationType));
      			


  	};

  	BTL_FUNC_END();
  	
  	return status;
}


/*-------------------------------------------------------------------------------
 * GetChannelAdmin()
 *
 *    Find the channelAdmin that is identified with the BIP 'channel' and return
 *    it. When it cannot be found, NULL will be returned.
 *
 * Parameters:
 *    channel[in] - BIP channel
 *
 */
static BtlBipChannel *GetChannelAdmin(BipChannel channel)
{
	UNUSED_PARAMETER(channel);
	
    /* It is now a simple function (parameter ignored) because multi-instance is not supported yet.*/
    return(&(btlChannelAdmin[0]));
}

/*-------------------------------------------------------------------------------
 * GetFreeChannelAdmin()
 *
 *    Return a free channel administration.
 *    If there is no free administration, NULL will be returned.
 *
 */
static BtlBipChannel *GetFreeChannelAdmin()
{
    BtlBipChannel *btlChannel=NULL; /* function return. */
  
    /* It is now a simple function (parameter ignored) because multi-instance is not supported yet.*/
    if (BTL_BIP_CHANNEL_STATE_DISCONNECTED ==btlChannelAdmin[0].state)
    {
        btlChannel = &(btlChannelAdmin[0]);
    }

    return(btlChannel);
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
static void SetContextState(BtlBiprspContext *bipContext,
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
 *    Fill the 'channelState' parameter in the 'bipChannel' with the new 'state'.
 *
 * Parameters:
 *
 * Returns:
 */
static void SetChannelState(BtlBipChannel *bipChannel,
                            BtlBipChannelState state)
{   
    BTL_FUNC_START("SetContextState");

    bipChannel->state = state;
    
    /* DEBUG: do some logging. */
    BTL_BIP_LOG_CHANNEL_STATE(state);

    BTL_FUNC_END();
}

/*------------------------------------------------------------------------------
 * XmlBuildAttrBool()
 */
static void XmlBuildAttrBool(U8 *xmlStr, BOOL value)
{
    if (TRUE == value)
    {
        OS_StrCat((char*)xmlStr, "1");
    }
    else
    {
        OS_StrCat((char*)xmlStr, "0");
    }

    OS_StrCat((char*)xmlStr, "\""); /* Close this attribute */
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

void BtlBiprspSendProgressNotificationToApp(BtlBiprspContext *context, BipCallbackParms* parms, U32 currPos)
{
	BTL_FUNC_START("BtlBiprspSendProgressNotificationToApp");

	btlBipData.progressIndInfo.name = context->fsPath;
	btlBipData.progressIndInfo.currPos = currPos;
	btlBipData.progressIndInfo.maxPos = parms->data->totLen;
	
	btlBipData.selfGeneratedCallbackParms = *parms;

	btlBipData.selfGeneratedCallbackParms.event = BIPRESPONDER_PROGRESS;
	btlBipData.selfGeneratedCallbackParms.data->progressInfo = &btlBipData.progressIndInfo;

	BtlBiprspSendEventToApp(context, &btlBipData.selfGeneratedCallbackParms);
		
	BTL_FUNC_END();
}

void BtlBiprspSendEventToApp(BtlBiprspContext *context, BipCallbackParms *parms)
{
	BTL_FUNC_START("BtlBiprspSendEventToApp");

	btlBipData.eventToApp.context = context;
	btlBipData.eventToApp.parms = parms;

	(context->callback)(&btlBipData.eventToApp);
	
	BTL_FUNC_END();
}

/*---------------------------------------------------------------------------
 * BtlBiprspCopyTempFileToMem()
 *
 * We pushed received data to FS object and now user requests Memory object, 
 * therefore we need to move all data from temp FS to Memory object.
 *
 */
static BtStatus BtlBiprspCopyTempFileToMem(
				BtlBiprspContext	*context, 
				const BtlUtf8 		*tempFilePath, 
				BtlBipObjStore 	*objStore,
				void 			*memAddress)
{
	BtStatus			status = BT_STATUS_SUCCESS;
	BtFsStatus		fsStatus;
	BTHAL_U32 		pNumRead;
	BipData			tempBipData;

	BTL_FUNC_START("BtlBiprspCopyTempFileToMem");
	
	BTL_VERIFY_FATAL(objStore->type == BTL_BIP_OBJ_STORE_TYPEHANDLE, BT_STATUS_INTERNAL_ERROR,
						("Invalid Obj Store Type (%d)", objStore->type));
	BTL_VERIFY_FATAL(BTHAL_FS_INVALID_FILE_DESC != objStore->location.fileHandle, BT_STATUS_INTERNAL_ERROR,
						("Null File Handle"));

	/* Close the temporary file */
	status = btl_bip_FileClose(&(objStore->location.fileHandle));
	BTL_VERIFY_ERR(status == BT_STATUS_SUCCESS, BT_STATUS_FFS_ERROR, ("Failed Closing Temp File (%s)", tempFilePath));
	
	status =  btl_bip_FileOpen(FALSE, tempFilePath, NULL, &objStore->location.fileHandle, &tempBipData);
	BTL_VERIFY_ERR(status == BT_STATUS_SUCCESS, BT_STATUS_FFS_ERROR, ("Failed Opening Temp File (%s)", tempFilePath));

	fsStatus = BTHAL_FS_Read(objStore->location.fileHandle, memAddress, tempBipData.totLen, &pNumRead);
	BTL_VERIFY_ERR(fsStatus == BT_STATUS_HAL_FS_SUCCESS, BT_STATUS_FFS_ERROR, 
					("BTHAL_FS_Read Failed (%d) for %s", fsStatus, tempFilePath));
	BTL_VERIFY_ERR((pNumRead == tempBipData.totLen), BT_STATUS_FFS_ERROR, 
					("Read %d bytes from %s, Expected %d", pNumRead, tempFilePath, tempBipData.totLen));

	/* Close the temporary file */
	status = btl_bip_FileClose(&(objStore->location.fileHandle));
	BTL_VERIFY_ERR(status == BT_STATUS_SUCCESS, BT_STATUS_FFS_ERROR, ("Failed Closing Temp File (%s)", tempFilePath));

	objStore->type = BTL_BIP_OBJ_STORE_TYPEMEMPTR;
	objStore->location.mem = memAddress;

	context->deleteFileIfAborted = FALSE;
	OS_StrCpyUtf8(context->pushedFileName, (BtlUtf8*)"");
				
	fsStatus = BTHAL_FS_Remove((const BTHAL_U8 *)tempFilePath);
	BTL_VERIFY_ERR(status == BT_STATUS_SUCCESS, BT_STATUS_FFS_ERROR, ("Failed Closing Temp File (%s)", tempFilePath));
	
	BTL_FUNC_END();
	
	return status;
}

static void BtlBiprspExtractFolder(const BtlUtf8 *fileFullName, BtlUtf8 *folderName)
{
	int len;

	len = OS_StrLenUtf8(fileFullName);
	len--;

	for ( ;(fileFullName[len] != BTHAL_FS_PATH_DELIMITER) && (len >= 0); len--)
	{
	    /* do-nothing */
	}
		
	OS_StrnCpyUtf8(folderName, fileFullName, len);
	
	*(folderName+len) = '\0';
}

static BtStatus BtlBiprspRenameFile(const BtlUtf8 *srcFullFileName, const BtlUtf8 *destFullFileName)
{

#define BTL_BIPRSP_RENAME_FILE_BUF_SIZE	128

	BtStatus			status = BT_STATUS_SUCCESS;
	BtlUtf8 			srcFolderName[BTHAL_FS_MAX_PATH_LENGTH + 1];
	BtlUtf8 			destFolderName[BTHAL_FS_MAX_PATH_LENGTH + 1];
	BthalFsStat 		fileStat;
	BtFsStatus		fsStatus;
	BthalFsFileDesc	srcFd = BTHAL_FS_INVALID_FILE_DESC;
	BthalFsFileDesc	destFd = BTHAL_FS_INVALID_FILE_DESC;
	BTHAL_U32		leftToCopy;
	U8				buff[BTL_BIPRSP_RENAME_FILE_BUF_SIZE];
	BTHAL_U32		numToRead;
	BTHAL_U32 		pNumRead;
	BTHAL_U32 		pNumWritten;
	
	BTL_FUNC_START("BtlBiprspMoveFile");
	
	BTL_VERIFY_FATAL((srcFullFileName != NULL), BT_STATUS_INTERNAL_ERROR, ("Null srcFullFileName"));
	BTL_VERIFY_FATAL((destFullFileName != NULL), BT_STATUS_INTERNAL_ERROR, ("Null destFullFileName"));
	
	if (0 != OS_StrCmpUtf8(srcFullFileName, destFullFileName))
	{
		/* Check if the two files are on the same directory */
		BtlBiprspExtractFolder(srcFullFileName, srcFolderName);
		BtlBiprspExtractFolder(destFullFileName, destFolderName);

		if (0 == OS_StrCmpUtf8(srcFolderName, destFolderName))
		{
			fsStatus = BTHAL_FS_Remove((const BTHAL_U8 *)destFullFileName);
			BTL_VERIFY_ERR(((fsStatus == BT_STATUS_HAL_FS_SUCCESS) || (fsStatus == BT_STATUS_HAL_FS_ERROR_NOTFOUND)), 
								BT_STATUS_FFS_ERROR, 
								("Failed Removing Src File (%s)", destFullFileName));

			/* If they are, then we can use BTHAL_FS_Rename */
			fsStatus = BTHAL_FS_Rename((const BTHAL_U8 *)srcFullFileName, (const BTHAL_U8 *)destFullFileName);
			BTL_VERIFY_ERR((fsStatus == BT_STATUS_HAL_FS_SUCCESS), BT_STATUS_FFS_ERROR, 
							("Failed Renaming %s to %s", srcFullFileName, destFullFileName));
		}
		/* Not in same folder => must copy content */
		else
		{			
			fsStatus = BTHAL_FS_Stat((const BTHAL_U8 *)srcFullFileName, &fileStat); 
			BTL_VERIFY_ERR((fsStatus == BT_STATUS_HAL_FS_SUCCESS), BT_STATUS_FFS_ERROR, 
							("BTHAL_FS_Stat Failed for %s", srcFullFileName));

			leftToCopy = fileStat.size;
			
			fsStatus = BTHAL_FS_Open(	(const BTHAL_U8 *)srcFullFileName, 
								            	BTHAL_FS_O_RDONLY | BTHAL_FS_O_BINARY, 
		                    					&srcFd);
			BTL_VERIFY_ERR((fsStatus == BT_STATUS_HAL_FS_SUCCESS), BT_STATUS_FFS_ERROR, 
							("Failed Opening Src File (%s)", srcFullFileName));
		    
			fsStatus = BTHAL_FS_Open(	(const BTHAL_U8 *)destFullFileName, 
								            	BTHAL_FS_O_CREATE | BTHAL_FS_O_TRUNC | BTHAL_FS_O_WRONLY | BTHAL_FS_O_BINARY, 
		                    					&destFd);
			BTL_VERIFY_ERR((fsStatus == BT_STATUS_HAL_FS_SUCCESS), BT_STATUS_FFS_ERROR, 
							("Failed Opening Dest File (%s)", destFullFileName));

			while (leftToCopy > 0)
			{
				if (leftToCopy <= BTL_BIPRSP_RENAME_FILE_BUF_SIZE)
				{
					numToRead = leftToCopy;
				}
				else
				{
					numToRead = BTL_BIPRSP_RENAME_FILE_BUF_SIZE;
				}
				
				fsStatus = BTHAL_FS_Read(srcFd, buff, numToRead, &pNumRead);
				BTL_VERIFY_ERR((fsStatus == BT_STATUS_HAL_FS_SUCCESS), BT_STATUS_FFS_ERROR, 
								("Failed Reading From Src File (%s)", srcFullFileName));

				fsStatus = BTHAL_FS_Write(destFd, buff, pNumRead, &pNumWritten);
				BTL_VERIFY_ERR((fsStatus == BT_STATUS_HAL_FS_SUCCESS), BT_STATUS_FFS_ERROR, 
								("Failed Writing To Dest File (%s)", destFullFileName));
				BTL_VERIFY_ERR((pNumWritten == pNumRead), BT_STATUS_FFS_ERROR, 
								("Wrote %d Bytes To Dest File (%s), Expected %d", pNumWritten, destFullFileName, pNumRead));

				leftToCopy -= pNumWritten;
			}

			fsStatus = BTHAL_FS_Remove((const BTHAL_U8 *)srcFullFileName);
			BTL_VERIFY_ERR((fsStatus == BT_STATUS_HAL_FS_SUCCESS), BT_STATUS_FFS_ERROR, 
							("Failed Removing Src File (%s)", srcFullFileName));
		}    

		BTL_LOG_INFO(("%s Successfully Renamed To %s", srcFullFileName, destFullFileName));
	}
	else
	{
		/* Src is identical to Dest => Do-nothing */
	}

	BTL_FUNC_END();

	if (srcFd != BTHAL_FS_INVALID_FILE_DESC)
	{
		BTHAL_FS_Close(srcFd);
	}

	if (destFd != BTHAL_FS_INVALID_FILE_DESC)
	{
		BTHAL_FS_Close(destFd);
	}

	return status;
}

static void BtlBiprspInitRequestVars(BtlBiprspContext *bipContext)
{
	bipContext->objStore.type = BTL_BIP_OBJ_STORE_TYPENONE;
	bipContext->objLen = 0;
	OS_MemSet(&bipContext->imgHandleAssignedToPushedImage, 0, 8);
	bipContext->suspendContinue = FALSE;
	bipContext->continuePending = FALSE;
	bipContext->abortState = BTL_BIPRSP_ABORT_STATE_IDLE;

	OS_StrCpyUtf8(bipContext->desiredPushedFileName, (BtlUtf8*)"");

	bipContext->deleteFileIfAborted = FALSE;	
}





#else /* BTL_CONFIG_BIP ==   BTL_CONFIG_ENABLED*/

/*-------------------------------------------------------------------------------
 * BTL_BIPINT_Init()
 */
BtStatus BTL_BIPRSP_Init(void)
{
    
   BTL_LOG_INFO(("BTL_BIPRSP_Init() -  BTL_CONFIG_BIP Disabled"));
  

    return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 * BTL_BIPINT_Deinit()
 */
BtStatus BTL_BIPRSP_Deinit(void)
{
    BTL_LOG_INFO(("BTL_BIPRSP_Deinit() -  BTL_CONFIG_BIP Disabled"));

    return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------*/



#endif /* BTL_CONFIG_BIP==   BTL_CONFIG_ENABLED*/



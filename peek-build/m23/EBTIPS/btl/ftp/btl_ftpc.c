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
*   FILE NAME:      btl_ftpc.c
*
*   DESCRIPTION:    This file contains the BTL FTP client implementation.
*
*   AUTHOR:         Yoni Shavit , Yaniv Rabin
*
\*******************************************************************************/


#include "btl_config.h"
#include "btl_defs.h"

BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_FTPC);

#if BTL_CONFIG_FTPC ==   BTL_CONFIG_ENABLED

/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "btl_ftpc.h"
#include "btl_pool.h"


#include "btl_commoni.h"
#include "config.h"
#include "ftp.h"
#include "goep.h"
#include "ftpcstore.h"
#include "osapi.h"
#include "utils.h"
#include "medev.h"
#include "bthal_fs.h"



/********************************************************************************
 *
 * Constants 
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BSL_FTPC_DEFAULT constant
 *
 *     Represents default security level for FTPC.
 */
#define BSL_FTPC_DEFAULT  (BSL_AUTHENTICATION_IN)


/*---------------------------------------------------------------------------
 *
 * Object Store Function call table
 */
static const ObStoreFuncTable ftpcFuncTable = {
    FTPCSTORE_Read, FTPCSTORE_Write, FTPCSTORE_GetObjectLen,
};

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/* Forward declarations */
typedef struct _BtlFtpcData BtlFtpcData;


/*-------------------------------------------------------------------------------
 * BtlFtpcInitState type
 *
 *     Defines the FTPC init state.
 */
typedef U8 BtlFtpcInitState;

#define BTL_FTPC_INIT_STATE_NOT_INTIALIZED					(0x00)
#define BTL_FTPC_INIT_STATE_INITIALIZED						(0x01)
#define BTL_FTPC_INIT_STATE_INITIALIZATION_FAILED			(0x02)
#define BTL_FTPC_INIT_STATE_DEINITIALIZATION_FAILED			(0x03)


/*-------------------------------------------------------------------------------
 * BtlFtpcState type
 *
 *     Defines the FTPC state of a specific context.
 */
typedef U8 BtlFtpcState;

#define BTL_FTPC_STATE_IDLE									(0x00)
#define BTL_FTPC_STATE_DISCONNECTED							(0x01)
#define BTL_FTPC_STATE_CONNECTED							(0x02)
#define BTL_FTPC_STATE_DISCONNECTING						(0x03)
#define BTL_FTPC_STATE_DISABLING							(0x04)
#define BTL_FTPC_STATE_CONNECTING							(0x05)

/*-------------------------------------------------------------------------------
 * BtlFtpcDisableStateMask type
 *
 *     Defines the FTPC disable state mask.
 */
typedef U8 BtlFtpcDisableStateMask;

#define BTL_FTPC_DISABLE_STATE_MASK_NONE				(0x00)
#define BTL_FTPC_DISABLE_STATE_MASK_DISABLE_ACTIVE		(0x01)
#define BTL_FTPC_DISABLE_STATE_MASK_RADIO_OFF_ACTIVE	(0x02)

/*-------------------------------------------------------------------------------
 * ObexAuthFlags type
 *
 *     Defines the OBEX authentication flags of a specific context.
 */
typedef U8 ObexAuthFlags;

#define OBEX_AUTH_NONE                                      (0x00)
#define OBEX_AUTH_RECEIVED_CHAL  							(0x02)
#define OBEX_AUTH_RECEIVED_CHAL_CONT						(0x04)
#define OBEX_AUTH_AUTHENTICATED  							(0x08)


/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * _BtlFtpcContext structure
 *
 *     Represents BTL FTPC context.
 */
struct _BtlFtpcContext 
{
	/* Must be first field */
	BtlContext base;

	/* Internal FTPC channel */
	GoepClientApp client;

	/* Associated callback with this context */
	BtlFtpcCallBack callback;

	/* Current FTPC state of the context */
	BtlFtpcState state;

	/* Request parameters */
	GoepObjectReq pushpull;

	/* Current GOEP operation */
	FtpcOperation currOper;

	/* File name for push/pull operations */
	BtlUtf8 operFileName[BTHAL_FS_MAX_FILE_NAME_LENGTH + 1];

	ObexTpAddr Target;

#if OBEX_AUTHENTICATION == XA_ENABLED
	
	/* Defines if server is initiating OBEX authentication */
	BOOL initiateObexAuth;

	/* OBEX Authentication flags */
	ObexAuthFlags obexAuthFlags;

	/*used by the Client when sending an OBEX Connect*/
	GoepConnectReq	req;

	/*encapsulates the information necessary to build an authentication challenge */
	ObexAuthChallenge	challenge;

	/*encapsulates the information necessary to build an authentication response */
	ObexAuthResponse	response;

	/* Null-terminated string representing the OBEX authentication realm */
	BtlUtf8 obexRealm[BTL_CONFIG_OBEX_MAX_REALM_LEN+ 2];

	/* Null-terminated string representing the OBEX authentication password */
	BtlUtf8 obexPassword[BTL_CONFIG_OBEX_MAX_PASS_LEN+ 1];

	/* Null-terminated string representing the OBEX authentication user Id */
	BtlUtf8 obexUserId[BTL_CONFIG_OBEX_MAX_USER_ID_LEN+ 1];

	/* The challenge string. This value is used to build the challenge nonce */
	U8	chalStr[BTL_CONFIG_OBEX_MAX_PASS_LEN + 7];

	/* This flag indicates the disable state */
	BtlFtpcDisableStateMask disableState;
	

#endif	/* OBEX_AUTHENTICATION == XA_ENABLED */

	
};


/*-------------------------------------------------------------------------------
 * BtlFtpcData structure
 *
 *     Represents the data of th BTL FTPC module.
 */
struct _BtlFtpcData
{
	/* Pool of FTPC contexts */
	BTL_POOL_DECLARE_POOL(contextsPool, contextsMemory, BTL_CONFIG_FTPC_MAX_NUM_OF_CONTEXTS, sizeof(BtlFtpcContext));

	/* List of active FTPC contexts */
	ListEntry contextsList;
	
	/* Event passed to the application */
	BtlFtpcEvent event;

	/* Internal event generated in BTL_FTPC layer */
	FtpClientEvent ftpcEvent;
};

/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * btlFtpcInitState
 *
 *     Represents the current init state of FTPC module.
 */
static BtlFtpcInitState btlFtpcInitState = BTL_FTPC_INIT_STATE_NOT_INTIALIZED;


/*-------------------------------------------------------------------------------
 * btlFtpcData
 *
 *     Represents the data of FTPC module.
 */
static BtlFtpcData btlFtpcData;


/*-------------------------------------------------------------------------------
 * btlFtpcContextsPoolName
 *
 *     Represents the name of the FTPC contexts pool.
 */
static const char btlFtpcContextsPoolName[] = "FtpcContexts";

#if XA_CONTEXT_PTR == XA_ENABLED
extern FtpExtensionData  *Ftp;
#else /* XA_CONTEXT_PTR == XA_ENABLED */
extern FtpExtensionData   Ftp;
#endif /* XA_CONTEXT_PTR == XA_ENABLED */


/*-------------------------------------------------------------------------------
 * obexFolderListingMimeType
 *
 *     Represents the MIME type of the pull folder  listing object
 */
static const char obexFolderListingMimeType[] = {"x-obex/folder-listing"};

/********************************************************************************
 *
 * Internal function prototypes
 *
 *******************************************************************************/
static BtStatus BtlFtpcDisable(BtlFtpcContext *ftpcContext);
#if OBEX_AUTHENTICATION == XA_ENABLED
static void BtlFtpcInitiateObexAuth(BtlFtpcContext *ftpcContext,ObexAuthChallenge *Challenge);
#endif	/* OBEX_AUTHENTICATION == XA_ENABLED */
static void BtlFtpcCallback(GoepClientEvent *Event);
static void BtlFtpcFtpcstoreCallback(FtpcStoreEntry *obs);
static ObStatus BtlFtpcBuildPushReq(BtlFtpcContext *ftpcContext, const BtlObject *objToPush);
static BtStatus BtlFtpcBuildPullReq(BtlFtpcContext *FtpcContext, const BtlObject *objToPull);
static  BtStatus BtlFtpcBuildPullFolderReq(BtlFtpcContext *ftpcContext, const BtlObject *objToPull);
static const char *BtlpObexHeaderType(ObexHeaderType type);
static const char *BtlFtpcGoepOpName(GoepOperation Op);
static BtStatus BtlFtpcBtlNotificationsCb(BtlModuleNotificationType notificationType);
static const char *ClientAbortMsg(ObexRespCode Reason);


/********************************************************************************
 *
 * Function definitions
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BTL_FTPC_Init()
 */
BtStatus BTL_FTPC_Init(void)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_COMMON("BTL_FTPC_Init");
	
	BTL_VERIFY_ERR((BTL_FTPC_INIT_STATE_NOT_INTIALIZED == btlFtpcInitState), 
		BT_STATUS_FAILED, ("FTPC module is already initialized"));
	
	btlFtpcInitState = BTL_FTPC_INIT_STATE_INITIALIZATION_FAILED;

	status = BTL_POOL_Create(&btlFtpcData.contextsPool,
							btlFtpcContextsPoolName,
							btlFtpcData.contextsMemory, 
							BTL_CONFIG_FTPC_MAX_NUM_OF_CONTEXTS,
							sizeof(BtlFtpcContext));
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_FAILED, 
		("FTPC contexts pool creation failed"));
	
	InitializeListHead(&btlFtpcData.contextsList);
		
	BTL_RegisterForCommonNotifications(BTL_MODULE_TYPE_FTPC, BtlFtpcBtlNotificationsCb);
	
	btlFtpcInitState = BTL_FTPC_INIT_STATE_INITIALIZED;

	BTL_FUNC_END_AND_UNLOCK();
	
	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_FTPC_Deinit()
 */
BtStatus BTL_FTPC_Deinit(void)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_COMMON("BTL_FTPC_Deinit");

	BTL_DeRegisterFromCommonNotifications(BTL_MODULE_TYPE_FTPC);
	
	BTL_VERIFY_ERR((BTL_FTPC_INIT_STATE_INITIALIZED == btlFtpcInitState), 
		BT_STATUS_FAILED, ("FTPC module is not initialized"));

	btlFtpcInitState = BTL_FTPC_INIT_STATE_DEINITIALIZATION_FAILED;

	BTL_VERIFY_ERR((IsListEmpty(&btlFtpcData.contextsList)), 
		BT_STATUS_FAILED, ("FTPC contexts are still active"));

	status = BTL_POOL_Destroy(&btlFtpcData.contextsPool);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_FAILED, 
		("FTPC contexts pool destruction failed"));
		
	btlFtpcInitState = BTL_FTPC_INIT_STATE_NOT_INTIALIZED;

	BTL_FUNC_END_AND_UNLOCK();
	
	return (status);
}

 
/*-------------------------------------------------------------------------------
 * BTL_FTPC_Create()
 */
BtStatus BTL_FTPC_Create(BtlAppHandle *appHandle,
						const BtlFtpcCallBack ftpcCallback,
						const BtSecurityLevel *securityLevel,
						BtlFtpcContext **ftpcContext)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_FTPC_Create");

	BTL_VERIFY_ERR((0 != ftpcCallback), BT_STATUS_INVALID_PARM, ("Null ftpcCallback"));
	BTL_VERIFY_ERR((0 != ftpcContext), BT_STATUS_INVALID_PARM, ("Null ftpcContext"));

#if BT_SECURITY == XA_ENABLED

	if (0 != securityLevel)
	{
		status = BTL_VerifySecurityLevel(*securityLevel);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Invalid FTPC securityLevel"));
	}

#endif	/* BT_SECURITY == XA_ENABLED */

	/* Allocate memory for a unique new FTPC context */
	status = BTL_POOL_Allocate(&btlFtpcData.contextsPool, (void **)ftpcContext);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed allocating FTPC context"));

	status = BTL_HandleModuleInstanceCreation(appHandle, BTL_MODULE_TYPE_FTPC, &(*ftpcContext)->base);
	if (BT_STATUS_SUCCESS != status)
	{
		/* ftpcContext must be freed before we exit */
		BtStatus retVal = BTL_POOL_Free(&btlFtpcData.contextsPool,(void **)ftpcContext);
		BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == retVal), ("Failed freeing FTPC context"));

		/* This verify will fail and will handle the exception gracefully */
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed handling FTPC instance creation"));
	}

	/* Save the given callback */
	(*ftpcContext)->callback = ftpcCallback;

	/* Add the new FTPC context to the active contexts list */
	InsertTailList(&btlFtpcData.contextsList, &((*ftpcContext)->base.node));

#if BT_SECURITY == XA_ENABLED

	/* Save the given security level, or use default */
	(*ftpcContext)->client.secRecord.level = (BtSecurityLevel)((securityLevel == 0) ? (BSL_FTPC_DEFAULT) : (*securityLevel));

#endif	/* BT_SECURITY == XA_ENABLED */

	/* Init FTPC context state */
	(*ftpcContext)->state = BTL_FTPC_STATE_IDLE;

	(*ftpcContext)->pushpull.object = 0;

	(*ftpcContext)->currOper = FTPC_OPER_NONE;

	(*ftpcContext)->disableState = BTL_FTPC_DISABLE_STATE_MASK_NONE;
	
#if OBEX_AUTHENTICATION == XA_ENABLED

	OS_MemSet(&(*ftpcContext)->req, 0, sizeof(GoepConnectReq));
	
	/* Server does not initiate OBEX authentication by default */
	(*ftpcContext)->initiateObexAuth = FALSE;

	(*ftpcContext)->obexAuthFlags = 0;

	(*ftpcContext)->req.challenge = &(*ftpcContext)->challenge;

	(*ftpcContext)->req.response = &(*ftpcContext)->response;

	/* The first byte of the string is the character set. The ASCII character set is value 0 */
	(*ftpcContext)->obexRealm[0] = '\0';
	(*ftpcContext)->obexRealm[1] = '\0';
	
	(*ftpcContext)->obexPassword[0] = '\0';
	
	(*ftpcContext)->obexUserId[0] = '\0';

#endif	/* OBEX_AUTHENTICATION == XA_ENABLED */

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_FTPC_Destroy()
 */
BtStatus BTL_FTPC_Destroy(BtlFtpcContext **ftpcContext)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BOOL isAllocated = FALSE;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_FTPC_Destroy");

	BTL_VERIFY_ERR((0 != ftpcContext), BT_STATUS_INVALID_PARM, ("Null ftpcContext"));
	BTL_VERIFY_ERR((0 != *ftpcContext), BT_STATUS_INVALID_PARM, ("Null *ftpcContext"));
	BTL_VERIFY_ERR((BTL_FTPC_STATE_IDLE == (*ftpcContext)->state), BT_STATUS_IN_USE, ("FTPC context is in use"));

	status = BTL_POOL_IsElelementAllocated(&btlFtpcData.contextsPool, *ftpcContext, &isAllocated);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed locating given FTPC context"));
	BTL_VERIFY_ERR((TRUE == isAllocated), BT_STATUS_INVALID_PARM, ("Invalid FTPC context"));

	/* Remove the context from the list of all FTPC contexts */
	RemoveEntryList(&((*ftpcContext)->base.node));

	status = BTL_HandleModuleInstanceDestruction(&(*ftpcContext)->base);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed handling FTPC instance destruction"));

	status = BTL_POOL_Free(&btlFtpcData.contextsPool,(void **)ftpcContext);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_FAILED, ("Failed freeing FTPC context"));

	/* Set the FTPC context to NULL */
	*ftpcContext = 0;

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_FTPC_Enable()
 */
BtStatus BTL_FTPC_Enable(BtlFtpcContext *ftpcContext) 
{
	BtStatus status = BT_STATUS_SUCCESS;
	ObStatus obStatus;
	BOOL retVal;
		
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_FTPC_Enable");

	BTL_VERIFY_ERR((0 != ftpcContext), BT_STATUS_INVALID_PARM, ("Null ftpcContext"));
	BTL_VERIFY_ERR((BTL_FTPC_STATE_IDLE == ftpcContext->state), BT_STATUS_FAILED, ("FTPC context is already enabled"));

	/* Setup callback, Object Store function table and register FTP server */
	ftpcContext->client.callback = BtlFtpcCallback;
	ftpcContext->client.connFlags = GOEP_NEW_CONN;
	obStatus = FTP_RegisterClient(&ftpcContext->client, &ftpcFuncTable);
	BTL_VERIFY_ERR((OB_STATUS_SUCCESS == obStatus), BT_STATUS_FAILED, ("Failed to register FTP client"));

	/* Init OBSTORE Module */
	retVal = FTPCSTORE_Init();

	BTL_VERIFY_ERR((retVal), BT_STATUS_FAILED, ("FTPCSTORE init Failed"));

#if BT_SECURITY == XA_ENABLED

	/* Register security record */
	status = GOEP_RegisterClientSecurityRecord(&ftpcContext->client, ftpcContext->client.secRecord.level);
	if (BT_STATUS_SUCCESS != status)
	{
		obStatus = FTP_DeregisterClient(&ftpcContext->client);
		BTL_VERIFY_ERR_NORET((OB_STATUS_SUCCESS == obStatus), ("Failed deregistering FTP client"));

		/* This verify will fail and will handle the exception gracefully */
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed registering FTP security record"));
	}
	
#endif	/* BT_SECURITY == XA_ENABLED */

	/* FTPC state is now enabled */
	ftpcContext->state = BTL_FTPC_STATE_DISCONNECTED;

	BTL_LOG_INFO(("BTL_FTPC: client is enabled."));

	BTL_FUNC_END_AND_UNLOCK();
	
	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_FTPC_Disable()
 */
BtStatus BTL_FTPC_Disable(BtlFtpcContext *ftpcContext)
{
	BtStatus status = BT_STATUS_PENDING;
	ObStatus obStatus;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_FTPC_Disable");

	BTL_VERIFY_ERR((0 != ftpcContext), BT_STATUS_INVALID_PARM, ("Null ftpcContext"));
	BTL_VERIFY_ERR((BTL_FTPC_STATE_IDLE != ftpcContext->state), BT_STATUS_FAILED, ("FTPC context is not enabled"));

	if (ftpcContext->state == BTL_FTPC_STATE_DISABLING)
	{
		ftpcContext->disableState |= BTL_FTPC_DISABLE_STATE_MASK_DISABLE_ACTIVE;

		BTL_RET(BT_STATUS_PENDING);
	}

	switch (ftpcContext->state)
	{
		case (BTL_FTPC_STATE_DISCONNECTING):
		{
			/* FTPC state is now in the process of disabling */
			ftpcContext->state = BTL_FTPC_STATE_DISABLING;
			break;
		}

		case (BTL_FTPC_STATE_CONNECTING):
		{
			/* In process of connecting, let it finish and then close the channel */

			/* FTPC state is now in the process of disabling */
			ftpcContext->state = BTL_FTPC_STATE_DISABLING;
			break;
		}
		case (BTL_FTPC_STATE_CONNECTED):
		{
			obStatus = FTP_Disconnect(&ftpcContext->client);
			if (OB_STATUS_BUSY == obStatus)
			{
				obStatus = GOEP_TpDisconnect(&ftpcContext->client);
			}

			if (OB_STATUS_PENDING == status)
			{
				/* FTPC state is now in the process of disabling */
				ftpcContext->state = BTL_FTPC_STATE_DISABLING;
				break;
			}
			
			/* No connection, try to deregister. Pass through to next case... */	
		}
		case (BTL_FTPC_STATE_DISCONNECTED):
		{
			break;
		}
		
		default:
		{
			/* This verify will fail and will handle the exception gracefully */
			BTL_VERIFY_ERR((BTL_FTPC_STATE_DISCONNECTED == ftpcContext->state), BT_STATUS_FAILED, ("Disable failed, invalid FTPC state"));
			break;
		}
	}
	
	if (BTL_FTPC_STATE_DISABLING != ftpcContext->state)
	{
		status = BtlFtpcDisable(ftpcContext);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed disabling FTPC context"));
	}
	else
	{
		ftpcContext->disableState |= BTL_FTPC_DISABLE_STATE_MASK_DISABLE_ACTIVE;
	}
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

/*-------------------------------------------------------------------------------
 * BtlFtpcDisable()
 *
 *		Internal function for deregistering security record and FTP client.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		ftpcContext [in] - pointer to the FTPC context.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */
static BtStatus BtlFtpcDisable(BtlFtpcContext *ftpcContext)
{
	BtStatus status = BT_STATUS_SUCCESS;
	ObStatus obStatus;
		
	BTL_FUNC_START("BtlFtpcDisable");

	/* Clean up any active object */
	ftpcContext->currOper = FTPC_OPER_NONE;

	/* Deinit FTPCSTORE Module */
	FTPCSTORE_Deinit();

#if BT_SECURITY == XA_ENABLED

	/* First, try to unregister security record */
	status = GOEP_UnregisterClientSecurityRecord(&ftpcContext->client);
	BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == status), ("Failed unregistering FTPC security record"));
			
#endif	/* BT_SECURITY == XA_ENABLED */

	/* Second, try to deregister server */
	obStatus = FTP_DeregisterClient(&ftpcContext->client);
	BTL_VERIFY_ERR((OB_STATUS_SUCCESS == status), BT_STATUS_FAILED, ("Failed deregistering FTP client"));

	/* FTPC state is now idle */
	ftpcContext->state = BTL_FTPC_STATE_IDLE;

	BTL_LOG_INFO(("BTL_FTPC: Client is disabled."));

	BTL_FUNC_END();

	return (status);
}

/*-------------------------------------------------------------------------------
 * BTL_FTPC_Connect()
 */
BtStatus BTL_FTPC_Connect(BtlFtpcContext *ftpcContext, 
							const BD_ADDR *bdAddr)
{
	BtStatus status = BT_STATUS_PENDING;
	BtDeviceContext * bdc;
	BtDeviceContext tmplt;
	
	BTL_FUNC_START_AND_LOCK("BTL_FTPC_Connect");

	BTL_VERIFY_ERR((0 != ftpcContext), BT_STATUS_INVALID_PARM, ("Null ftpcContext"));
	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));
	BTL_VERIFY_ERR((BTL_FTPC_STATE_DISCONNECTED == ftpcContext->state), BT_STATUS_FAILED, ("FTPC context is not disconnected"));

	ftpcContext->Target.type = OBEX_TP_BLUETOOTH;
	ftpcContext->Target.proto.bt.addr = *bdAddr;
		
	/* Find a device in DS */
	bdc = DS_FindDevice(bdAddr);
	if (!bdc) 
	{
		OS_MemSet(&tmplt, 0, (U32)sizeof(BtDeviceContext));
		OS_MemCopy(&tmplt.addr, (U8 *)bdAddr, sizeof(BD_ADDR));
		DS_AddDevice(&tmplt, 0);	
		ftpcContext->Target.proto.bt.psi.psRepMode = 1;
	 	ftpcContext->Target.proto.bt.psi.psMode = 0;
		ftpcContext->Target.proto.bt.psi.clockOffset = 0;
	}
	else
	{
		ftpcContext->Target.proto.bt.psi = bdc->psi;
	}

#if OBEX_AUTHENTICATION == XA_ENABLED
	ftpcContext->req.response = 0;
	ftpcContext->req.challenge = 0;

	/* Only authenticate the client if the we should and if we haven't already */
	if ((TRUE == ftpcContext->initiateObexAuth) && !(ftpcContext->obexAuthFlags & OBEX_AUTH_AUTHENTICATED))
	{
		OS_MemSet(&ftpcContext->challenge,0, sizeof(ObexAuthChallenge));
		BtlFtpcInitiateObexAuth(ftpcContext,&ftpcContext->challenge);
		ftpcContext->req.challenge = &ftpcContext->challenge;
	}
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */ 	
	
	status = FTP_Connect(&ftpcContext->client, &ftpcContext->Target,&ftpcContext->req );
	
	BTL_VERIFY_ERR((OB_STATUS_PENDING == status), status, ("Failed connecting FTP client"));

	/* FTPC state is now in the process of connecting */
	ftpcContext->state = BTL_FTPC_STATE_CONNECTING;

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_FTPC_Disconnect()
 */
BtStatus BTL_FTPC_Disconnect(BtlFtpcContext *ftpcContext)
{
	BtStatus status = BT_STATUS_PENDING;
	ObStatus obStatus;
	
	BTL_FUNC_START_AND_LOCK("BTL_FTPC_Disconnect");

	BTL_VERIFY_ERR((0 != ftpcContext), BT_STATUS_INVALID_PARM, ("Null ftpcContext"));
	BTL_VERIFY_ERR((BTL_FTPC_STATE_CONNECTING == ftpcContext->state) || (BTL_FTPC_STATE_CONNECTED == ftpcContext->state), 
					BT_STATUS_NO_CONNECTION, 
					("FTPC context is not connected and is not connecting"));

	obStatus = OB_STATUS_BUSY;
		
	/* FTPC state is now in the process of disconnecting */
	ftpcContext->state = BTL_FTPC_STATE_DISCONNECTING;

	obStatus = FTP_Disconnect(&ftpcContext->client);
	/*	BTL_VERIFY_ERR((OB_STATUS_BUSY != obStatus), BT_STATUS_BUSY, ("FTPC is currently executing an operation"));*/

	if (OB_STATUS_BUSY == obStatus)
	{
		obStatus = GOEP_TpDisconnect(&ftpcContext->client);
	}

	BTL_VERIFY_ERR((OB_STATUS_SUCCESS == obStatus) || (OB_STATUS_PENDING == obStatus),
                   BT_STATUS_FAILED,  ("Failed disconnecting FTP client"));
	
	if (OB_STATUS_SUCCESS == obStatus)
	{
		ftpcContext->state = BTL_FTPC_STATE_DISCONNECTED;
		status = BT_STATUS_SUCCESS;
	}

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_FTPC_GetConnectedDevice()
 */
BtStatus BTL_FTPC_GetConnectedDevice(BtlFtpcContext *ftpcContext, 
										BD_ADDR *bdAddr)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BOOL retVal;
	ObexTpConnInfo tpInfo;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_FTPC_GetConnectedDevice");

	BTL_VERIFY_ERR((0 != ftpcContext), BT_STATUS_INVALID_PARM, ("Null ftpcContext"));
	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));
	BTL_VERIFY_ERR((BTL_FTPC_STATE_CONNECTED == ftpcContext->state), BT_STATUS_NO_CONNECTION, ("FTPC context is not connected"));

	tpInfo.size = sizeof(ObexTpConnInfo);
	tpInfo.tpType = OBEX_TP_BLUETOOTH;
	tpInfo.remDev = 0;

	retVal = GOEP_ClientGetTpConnInfo(&ftpcContext->client, &tpInfo);
	BTL_VERIFY_ERR((TRUE == retVal), BT_STATUS_NO_CONNECTION, ("FTPC get connection info failed"));

	BTL_VERIFY_ERR((0 != tpInfo.remDev), BT_STATUS_FAILED, ("Unable to find connected remote device"));
	
	OS_MemCopy(bdAddr, &(tpInfo.remDev->bdAddr), sizeof(BD_ADDR));

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_FTPC_Push()
 */
BtStatus BTL_FTPC_Push(BtlFtpcContext *ftpcContext,
						const BtlObject *objToPush)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtStatus ret;
	
	BTL_FUNC_START_AND_LOCK("BTL_FTPC_Push");

	BTL_VERIFY_ERR((0 != ftpcContext), BT_STATUS_INVALID_PARM, ("Null ftpcContext"));
	BTL_VERIFY_ERR((BTL_FTPC_STATE_CONNECTED == ftpcContext->state), BT_STATUS_NO_CONNECTION, ("FTPC context is not connected"));
	BTL_VERIFY_ERR((FTPC_OPER_NONE == ftpcContext->currOper), BT_STATUS_BUSY, ("FTPC context is in progress"));

	ret = BtlFtpcBuildPushReq(ftpcContext, objToPush);
	
	BTL_VERIFY_ERR((ret != BT_STATUS_FAILED), BT_STATUS_FAILED, ("Client build push request for file %s", objToPush->objectName));

	status = FTP_Push(&ftpcContext->client, &ftpcContext->pushpull);
	
	BTL_VERIFY_ERR((OB_STATUS_PENDING == status), BT_STATUS_FAILED, ("Client push falied"));

	/* FTPC state is now in the process of push */
	ftpcContext->currOper = FTPC_OPER_PUSH;

	BTL_FUNC_END_AND_UNLOCK();

	return (status);

}

/*-------------------------------------------------------------------------------
 * BTL_FTPC_Pull()
 */
 
BtStatus BTL_FTPC_Pull(BtlFtpcContext *ftpcContext, 
						const BtlObject *objToPull)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtStatus ret;
	
	BTL_FUNC_START_AND_LOCK("BTL_FTPC_Pull");

	BTL_VERIFY_ERR((0 != ftpcContext), BT_STATUS_INVALID_PARM, ("Null ftpcContext"));
	BTL_VERIFY_ERR((BTL_FTPC_STATE_CONNECTED == ftpcContext->state), BT_STATUS_NO_CONNECTION, ("FTPC context is not connected"));
	BTL_VERIFY_ERR((FTPC_OPER_NONE == ftpcContext->currOper), BT_STATUS_BUSY, ("FTPC context is in progress"));

	ret = BtlFtpcBuildPullReq(ftpcContext, objToPull);

	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == ret), BT_STATUS_INVALID_PARM, ("Client Failed building pull request for file %s", objToPull->objectName));

	status = FTP_Pull(&ftpcContext->client, &ftpcContext->pushpull, FALSE);
	
	if (status != OB_STATUS_PENDING)
	{
		FTPCSTORE_Delete((FtpcStoreHandle*)(&(ftpcContext->pushpull.object)));
	}		
	
	BTL_VERIFY_ERR((OB_STATUS_PENDING == status), BT_STATUS_FAILED, ("Client pull failed"));

	/* FTPC state is now in the process of pull */
	ftpcContext->currOper = FTPC_OPER_PULL;

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

/*-------------------------------------------------------------------------------
 * BTL_FTPC_PullListFolder()
 */
BtStatus BTL_FTPC_PullListFolder(BtlFtpcContext *ftpcContext, 
								const BtlObject *folderListing)
{
	BtStatus    status = BT_STATUS_SUCCESS;
	BtStatus	ret;

	BTL_FUNC_START_AND_LOCK("BTL_FTPC_PullListFolder");

	BTL_VERIFY_ERR((0 != ftpcContext), BT_STATUS_INVALID_PARM, ("Null ftpcContext"));
	BTL_VERIFY_ERR((BTL_FTPC_STATE_CONNECTED == ftpcContext->state), BT_STATUS_NO_CONNECTION, ("FTPC context is not connected"));
	BTL_VERIFY_ERR((FTPC_OPER_NONE == ftpcContext->currOper), BT_STATUS_BUSY, ("FTPC context is in progress"));

	ret = BtlFtpcBuildPullFolderReq(ftpcContext, folderListing);

	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == ret), BT_STATUS_INVALID_PARM, ("Failed to build folder pull request"));

	status = FTP_Pull(&ftpcContext->client, &ftpcContext->pushpull, FALSE);
	
	if (BT_STATUS_PENDING !=  status)
	{
		FTPCSTORE_Delete((FtpcStoreHandle *)(&(ftpcContext->pushpull.object)));
	}	
   
	BTL_VERIFY_ERR((BT_STATUS_PENDING == status), BT_STATUS_FAILED, ("Client pull failed"));

	/* FTPC state is now in the process of pull */
	ftpcContext->currOper = FTPC_OPER_PULL;
	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}
	
/*-------------------------------------------------------------------------------
 * BTL_FTPC_Delete()
 */
BtStatus BTL_FTPC_Delete(BtlFtpcContext *ftpcContext, 
							const BtlUtf8 *objToDelete)
{
	BtStatus status = BT_STATUS_PENDING;
	ObStatus obStatus;
	
	BTL_FUNC_START_AND_LOCK("BTL_FTPC_Delete");

	BTL_VERIFY_ERR((0 != ftpcContext), BT_STATUS_INVALID_PARM, ("Null ftpcContext"));
	BTL_VERIFY_ERR((0 != objToDelete), BT_STATUS_INVALID_PARM, ("Null objToDelete"));
	BTL_VERIFY_ERR((FTPC_OPER_NONE == ftpcContext->currOper), BT_STATUS_BUSY, ("FTPC context is in progress"));

	obStatus = FTP_Delete(&ftpcContext->client, (GoepUniType *)objToDelete);
	
	BTL_VERIFY_ERR((obStatus == OB_STATUS_PENDING), BT_STATUS_FAILED, ("Failed deleting remote file" ));

	/* FTPC state is now in the process of delete */
	ftpcContext->currOper = FTPC_OPER_DELETE;

	BTL_FUNC_END_AND_UNLOCK();

	return (status);

}


/*-------------------------------------------------------------------------------
 * BTL_FTPC_Abort()
 */
BtStatus BTL_FTPC_Abort(BtlFtpcContext *ftpcContext)
{
	BtStatus status = BT_STATUS_SUCCESS;
	ObStatus obStatus;

	BTL_FUNC_START_AND_LOCK("BTL_FTPC_Abort");

	BTL_VERIFY_ERR((0 != ftpcContext), BT_STATUS_INVALID_PARM, ("Null ftpcContext"));
	BTL_VERIFY_ERR((BTL_FTPC_STATE_CONNECTED == ftpcContext->state), BT_STATUS_NO_CONNECTION, ("FTPC context is not connected"));
    BTL_VERIFY_ERR((FTPC_OPER_NONE != ftpcContext->currOper), BT_STATUS_FAILED, ("No Operation in progress"));
	
    obStatus = FTP_ClientAbort(&ftpcContext->client);
	BTL_VERIFY_ERR((OB_STATUS_SUCCESS == obStatus), BT_STATUS_FAILED, ("client abort falied"));

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


#if OBEX_AUTHENTICATION == XA_ENABLED
/*-------------------------------------------------------------------------------
 * BTL_FTPC_ConfigInitiatingObexAuthentication()
 *
 */
BtStatus BTL_FTPC_ConfigInitiatingObexAuthentication(BtlFtpcContext *ftpcContext, 
													const BOOL enableObexAuth,
													const BtlUtf8 *realm, 
													const BtlUtf8 *userId, 
													const BtlUtf8 *password)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U16 len;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_FTPC_ConfigInitiatingObexAuthentication");

	BTL_VERIFY_ERR((0 != ftpcContext), BT_STATUS_INVALID_PARM, ("Null ftpcContext"));
	
	if (enableObexAuth == TRUE)
	{
		BTL_VERIFY_ERR((0 != password), BT_STATUS_INVALID_PARM, ("Null password"));
		if (realm != 0)
		{
			/* Check valid realm */
			len = OS_StrLenUtf8(realm);
			BTL_VERIFY_ERR((len <= BTL_CONFIG_OBEX_MAX_REALM_LEN), BT_STATUS_INVALID_PARM, ("Too long realm"));

			/* Now safely copy the realm, starting from the second byte */
			OS_StrnCpyUtf8((ftpcContext->obexRealm + 1), realm, BTL_CONFIG_OBEX_MAX_REALM_LEN);
			ftpcContext->obexRealm[BTL_CONFIG_OBEX_MAX_REALM_LEN] = '\0';
		}
		else
		{
			/* The first byte of the string is the character set. The ASCII character set is value 0 */
			ftpcContext->obexRealm[1] = '\0';
		}

		ftpcContext->challenge.realm = ftpcContext->obexRealm;

		if (userId != 0)
		{
			/* Check valid userId */
			len = OS_StrLenUtf8(userId);
			BTL_VERIFY_ERR((len <= BTL_CONFIG_OBEX_MAX_USER_ID_LEN), BT_STATUS_INVALID_PARM, ("Too long userId"));

			/* Now safely copy the userId */
			OS_StrnCpyUtf8(ftpcContext->obexUserId, userId, BTL_CONFIG_OBEX_MAX_USER_ID_LEN);
			ftpcContext->obexUserId[BTL_CONFIG_OBEX_MAX_USER_ID_LEN] = '\0';
		}
		else
		{
			ftpcContext->obexUserId[0] = '\0';
		}
		ftpcContext->response.userId = ftpcContext->obexUserId;
		
		if (password != 0)
		{
			/* Check valid password */
			len = OS_StrLenUtf8(password);
			BTL_VERIFY_ERR((len <= BTL_CONFIG_OBEX_MAX_PASS_LEN), BT_STATUS_INVALID_PARM, ("Too long password"));

			/* Now safely copy the password */
			OS_StrnCpyUtf8(ftpcContext->obexPassword, password, BTL_CONFIG_OBEX_MAX_PASS_LEN);
			ftpcContext->obexPassword[BTL_CONFIG_OBEX_MAX_PASS_LEN] = '\0';
	
		}
		else
		{
			ftpcContext->obexPassword[0] = '\0';
		}		

		ftpcContext->response.password = ftpcContext->obexPassword;
	}	
		

	ftpcContext->initiateObexAuth = enableObexAuth;

	BTL_FUNC_END_AND_UNLOCK();
	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_FTPC_ObexAuthenticationResponse()
 */
BtStatus BTL_FTPC_ObexAuthenticationResponse(BtlFtpcContext *ftpcContext, 
												const BtlUtf8 *userId, 
												const BtlUtf8 *password)
{
	BtStatus status = BT_STATUS_SUCCESS;
	ObStatus obStatus;
	U16 len;

	BTL_FUNC_START_AND_LOCK("BTL_FTPC_ObexAuthenticationResponse");

	BTL_VERIFY_ERR((0 != ftpcContext), BT_STATUS_INVALID_PARM, ("Null ftpcContext"));
	BTL_VERIFY_ERR((ftpcContext->obexAuthFlags & OBEX_AUTH_RECEIVED_CHAL), BT_STATUS_FAILED, ("No OBEX auth challenge was received"));

	ftpcContext->req.challenge = 0;
	
	if(password == 0){
		/* Respond to the challenge */
		ftpcContext->obexAuthFlags &= (~OBEX_AUTH_RECEIVED_CHAL);
		BTL_LOG_INFO(("BTL_FTPC: receiving NULL password and deny authentication."));
		ftpcContext->state = BTL_FTPC_STATE_CONNECTED;
		BTL_FTPC_Disconnect(ftpcContext);		
		BTL_RET(status);
	}
			
	if (userId != 0)
	{
		/* Check valid userId */
		len = OS_StrLenUtf8(userId);
		BTL_VERIFY_ERR((len <= BTL_CONFIG_OBEX_MAX_USER_ID_LEN), BT_STATUS_INVALID_PARM, ("Too long userId"));

		/* Now safely copy the userId */
		OS_StrnCpyUtf8(ftpcContext->obexUserId, userId, BTL_CONFIG_OBEX_MAX_USER_ID_LEN);
		ftpcContext->obexUserId[BTL_CONFIG_OBEX_MAX_USER_ID_LEN] = '\0';

		ftpcContext->response.userId = ftpcContext->obexUserId;
		ftpcContext->response.userIdLen = (U8)len;		
	}
	else
	{
		ftpcContext->response.userId = 0;		
		ftpcContext->response.userIdLen = 0;		
	}
	
	/* Check valid password */
	len = OS_StrLenUtf8(password);
	BTL_VERIFY_ERR((len <= BTL_CONFIG_OBEX_MAX_PASS_LEN), BT_STATUS_INVALID_PARM, ("Too long Password"));
	
	/* Now safely copy the password */
	OS_StrnCpyUtf8(ftpcContext->obexPassword, password, BTL_CONFIG_OBEX_MAX_PASS_LEN);
	ftpcContext->obexPassword[BTL_CONFIG_OBEX_MAX_PASS_LEN] = '\0';

	ftpcContext->response.password = ftpcContext->obexPassword;
	ftpcContext->response.passwordLen = (U8)len;	

	/* Respond to the challenge */
	ftpcContext->obexAuthFlags &= (~OBEX_AUTH_RECEIVED_CHAL);
	
    ftpcContext->req.response = &ftpcContext->response;
	
	obStatus = FTP_Connect(&ftpcContext->client,&ftpcContext->Target, &ftpcContext->req);
	
	BTL_VERIFY_ERR((OB_STATUS_PENDING == obStatus), BT_STATUS_INTERNAL_ERROR, ("Failed building authentication response header"));

	BTL_LOG_INFO(("BTL_FTPC: Built OBEX Authentication response header."));

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}
/*-------------------------------------------------------------------------------
 * BtlFtpcInitiateObexAuth()
 *
 *		Internal function for initiating OBEX authentication.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		ftpcContext [in] - pointer to the FTPS context.
 *
 * Returns:
 *		void.
 */
static void BtlFtpcInitiateObexAuth(BtlFtpcContext *ftpcContext,ObexAuthChallenge *Challenge)
{
	TimeT time;
	U16 len;

	BTL_FUNC_START("BtlFtpcInitiateObexAuth");

	Challenge->challenge = ftpcContext->chalStr;
    
	time = OS_GetSystemTime();
	ftpcContext->chalStr[0] = (U8)(time);
	ftpcContext->chalStr[1] = (U8)(time >> 8);
	ftpcContext->chalStr[2] = (U8)(time >> 16);
	ftpcContext->chalStr[3] = (U8)(time >> 24);
	ftpcContext->chalStr[4] = ':';

	OS_MemCopy(ftpcContext->chalStr+5, ftpcContext->response.password, OS_StrLenUtf8(ftpcContext->response.password));
	ftpcContext->chalStr[OS_StrLenUtf8(ftpcContext->response.password)+5] = '\0';	
	Challenge->challengeLen = (U8)(OS_StrLenUtf8(ftpcContext->response.password)+6);

	if (OS_StrLenUtf8(ftpcContext->response.userId) > 0)
	{
		Challenge->options = AUTH_OPT_SEND_USERID;  	/* UserId must be sent in response */
	}		
	else
	{
		Challenge->options = AUTH_OPT_DEFAULT;  		/* No options specified */
	}		

	if ((len = OS_StrLenUtf8((ftpcContext->obexRealm + 1))) > 0)
	{
		Challenge->realm = ftpcContext->obexRealm;
		Challenge->realmLen = (U8)(len + 1);	/* realmLen include also first '\0' BtlUtf8 */
	}
	else
	{
		Challenge->realm = 0;
		Challenge->realmLen = 0;
	}
	
	
	BTL_LOG_INFO(("BTL_FTPC: Built OBEX Authentication challenge header, realm \"%s\", userId \"%s\", password \"%s\".", 
		(ftpcContext->challenge.realm +1), ftpcContext->response.userId, ftpcContext->response.password));

	BTL_FUNC_END();
}
#endif	/* OBEX_AUTHENTICATION == XA_ENABLED */


#if BT_SECURITY == XA_ENABLED
/*-------------------------------------------------------------------------------
 * BTL_FTPC_SetSecurityLevel()
 */
BtStatus BTL_FTPC_SetSecurityLevel(BtlFtpcContext *ftpcContext,
								  	const BtSecurityLevel *securityLevel)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_FTPC_SetSecurityLevel");

	BTL_VERIFY_ERR((0 != ftpcContext), BT_STATUS_INVALID_PARM, ("Null ftpcContext"));

	if (0 != securityLevel)
	{
		status = BTL_VerifySecurityLevel(*securityLevel);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Invalid FTPC securityLevel"));
	}

	/* Apply the new security level */
	ftpcContext->client.secRecord.level = (BtSecurityLevel)((securityLevel == 0) ? (BSL_FTPC_DEFAULT) : (*securityLevel));

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_FTPC_GetSecurityLevel()
 */
BtStatus BTL_FTPC_GetSecurityLevel(BtlFtpcContext *ftpcContext,
								  	BtSecurityLevel *securityLevel)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_FTPC_GetSecurityLevel");

	BTL_VERIFY_ERR((0 != ftpcContext), BT_STATUS_INVALID_PARM, ("Null ftpcContext"));
	BTL_VERIFY_ERR((0 != securityLevel), BT_STATUS_INVALID_PARM, ("Null securityLevel"));

	*securityLevel = ftpcContext->client.secRecord.level;
	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}
#endif	/* BT_SECURITY == XA_ENABLED */

/*-------------------------------------------------------------------------------
 * BTL_FTPC_SetFolder()
 */
BtStatus BTL_FTPC_SetFolder(BtlFtpcContext *ftpcContext, 
							const BtlUtf8 *folderName, 
							BOOL allowCreate)
{
	BtStatus status = BT_STATUS_SUCCESS;
	ObStatus obStatus;

	BTL_FUNC_START_AND_LOCK("BTL_FTPC_SetFolder");

	BTL_VERIFY_ERR((0 != ftpcContext), BT_STATUS_INVALID_PARM, ("Null ftpcContext"));
	BTL_VERIFY_ERR((0 != folderName), BT_STATUS_INVALID_PARM, ("Null folderName"));
	BTL_VERIFY_ERR((FTPC_OPER_NONE == ftpcContext->currOper), BT_STATUS_BUSY, ("FTPC context is in progress"));
	
	obStatus = FTP_SetFolderFwd(&ftpcContext->client, (GoepUniType *)folderName, allowCreate);

	BTL_VERIFY_ERR((OB_STATUS_PENDING == obStatus), BT_STATUS_FAILED, ("Failed setting folder"));

	/* FTPC state is now in the process of set folder */
	ftpcContext->currOper = FTPC_OPER_SETFOLDER;

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

/*-------------------------------------------------------------------------------
 * BTL_FTPC_SetFolderBackUp()
 */
BtStatus BTL_FTPC_SetFolderBackUp(BtlFtpcContext *ftpcContext)
{
	BtStatus status = BT_STATUS_SUCCESS;
	ObStatus obStatus;

	BTL_FUNC_START_AND_LOCK("BTL_FTPC_SetFolderBackup");

	BTL_VERIFY_ERR((0 != ftpcContext), BT_STATUS_INVALID_PARM, ("Null ftpcContext"));
	BTL_VERIFY_ERR((FTPC_OPER_NONE == ftpcContext->currOper), BT_STATUS_BUSY, ("FTPC context is in progress"));

	obStatus = FTP_SetFolderBkup(&ftpcContext->client);

	BTL_VERIFY_ERR((OB_STATUS_PENDING == obStatus), BT_STATUS_FAILED, ("Failed setting folder"));

	/* FTPC state is now in the process of set folder */
	ftpcContext->currOper = FTPC_OPER_SETFOLDER;

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
	
}

/*-------------------------------------------------------------------------------
 * BTL_FTPC_SetFolderRoot()
 */
BtStatus BTL_FTPC_SetFolderRoot(BtlFtpcContext *ftpcContext)
{
	BtStatus status = BT_STATUS_SUCCESS;
	ObStatus obStatus;

	BTL_FUNC_START_AND_LOCK("BTL_FTPC_SetFolderRoot");

	BTL_VERIFY_ERR((0 != ftpcContext), BT_STATUS_INVALID_PARM, ("Null ftpcContext"));
	BTL_VERIFY_ERR((FTPC_OPER_NONE == ftpcContext->currOper), BT_STATUS_BUSY, ("FTPC context is in progress"));

	obStatus = FTP_SetFolderRoot(&ftpcContext->client);

	BTL_VERIFY_ERR((OB_STATUS_PENDING == obStatus), BT_STATUS_FAILED, ("Failed setting folder"));

	/* FTPC state is now in the process of set folder */
	ftpcContext->currOper = FTPC_OPER_SETFOLDER;
	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);

}


/*-------------------------------------------------------------------------------
 * BtlFtpcCallback()
 *
 *		Internal callback for handling FTP events in BTL layer.
 *		This function forwards events to registered application callback.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		Event [in] - Internal FTPC event.
 *
 * Returns:
 *		void.
 */
static void BtlFtpcCallback(GoepClientEvent *Event)
{
	BtStatus status = BT_STATUS_SUCCESS;
	ObStatus obStatus;
	ObexRespCode rcode;
	BtlFtpcContext *ftpcContext;
	BOOL passEventToApp = TRUE;
	BOOL sendDisableEvent = FALSE;
	BOOL notifyRadioOffComplete = FALSE;

	BTL_FUNC_START("BtlFtpcCallback");
	
	/* Find context according to given FTP event */
	ftpcContext = ContainingRecord(Event->handler, BtlFtpcContext, client);

	/* Setup the internal event */
	btlFtpcData.ftpcEvent.event = Event->event;
	btlFtpcData.ftpcEvent.oper = Event->oper;

	/* First handle special case of disabling */
	if (ftpcContext->state == BTL_FTPC_STATE_DISABLING)
	{
		switch (Event->event)
		{
			case (GOEP_EVENT_TP_DISCONNECTED):
			{

#if OBEX_AUTHENTICATION == XA_ENABLED
				ftpcContext->obexAuthFlags = 0;
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

				/* Clean up any active object */
				ftpcContext->state = BTL_FTPC_STATE_DISCONNECTED;
				
				/* The transport layer connection has been disconnected. */
				BTL_LOG_INFO(("BTL_FTPC: Transport layer connection has been disconnected."));

				if (ftpcContext->disableState & BTL_FTPC_DISABLE_STATE_MASK_DISABLE_ACTIVE)
				{				
					BtlFtpcDisable(ftpcContext);
					sendDisableEvent = TRUE;
				}

				if (ftpcContext->disableState & BTL_FTPC_DISABLE_STATE_MASK_RADIO_OFF_ACTIVE)
				{
					BtlContext *base;
					BtlFtpcContext *context;

					notifyRadioOffComplete = TRUE;

					IterateList(btlFtpcData.contextsList, base, BtlContext *)
					{
						context = (BtlFtpcContext *)base;
						if ((context->state != BTL_FTPC_STATE_DISCONNECTED) && (context->state != BTL_FTPC_STATE_IDLE))
								notifyRadioOffComplete = FALSE;			
					}
				}

				ftpcContext->currOper = FTPC_OPER_NONE;

				ftpcContext->disableState = BTL_FTPC_DISABLE_STATE_MASK_NONE;

				/* Pass event to app */
				break;
			}

			case (GOEP_EVENT_ABORTED):
			{
				/* Indicates that the current operation aborted. */
				BTL_LOG_INFO(("BTL_FTPC: GOEP %s The Operation was aborted, \"%s\".", BtlFtpcGoepOpName(Event->oper), ClientAbortMsg(Event->reason)));

				if (GOEP_OPER_CONNECT == Event->oper)
				{
					if (Event->reason != OBRC_SUCCESS)
					{
					    /* Security failed disconnect transport */
					    GOEP_TpDisconnect(&ftpcContext->client);
					}
				}
                
				if (Event->oper == GOEP_OPER_PUSH && ftpcContext->pushpull.object)
				{
					rcode = FTPCSTORE_Close((FtpcStoreHandle*)(&(ftpcContext->pushpull.object)));
				
				}
				/* if pull operation aborted - remove all files, associated with last pull session */
				else if (Event->oper == GOEP_OPER_PULL && ftpcContext->pushpull.object)  
				{
					FTPCSTORE_Delete((FtpcStoreHandle *)(&(ftpcContext->pushpull.object)));
				}

				ftpcContext->currOper = FTPC_OPER_NONE;

				btlFtpcData.ftpcEvent.reason = Event->reason;

				/* Pass event to app */
				break;
			}
			
			case (GOEP_EVENT_COMPLETE):
			{
				if (Event->oper == GOEP_OPER_CONNECT) 
				{				
					/* A new transport layer connection is established while disabling. */
					BTL_LOG_INFO(("BTL_FTPC: Transport layer connection has come up while disabling."));
					
					obStatus = FTP_Disconnect(&ftpcContext->client);
					if (OB_STATUS_BUSY == obStatus)
					{
						obStatus = GOEP_TpDisconnect(&ftpcContext->client);
					}

					if (OB_STATUS_PENDING == status)
					{
						/* FTPC state is now in the process of disabling */
						ftpcContext->state = BTL_FTPC_STATE_DISCONNECTING;
						break;
					}
					
					ftpcContext->currOper = FTPC_OPER_NONE;

					/* Do not pass event to app, since we are closing the connection... */
					passEventToApp = FALSE;
				}
				break;
				
			}
			
			default:
			{
				BTL_LOG_ERROR(("BTL_FTPC: Received unexpected event %d while disabling!", Event->event));	
				
				/* Do not pass event to app. */
				passEventToApp = FALSE;
				break;
			}
		}
	}
	else
	{			
		switch (Event->event)
		{
			case (GOEP_EVENT_DISCOVERY_FAILED):
   		   	{
				BTL_LOG_INFO(("BTL_FTPC: Client %s Discovery Failure.", BtlFtpcGoepOpName(Event->oper)));
       			ftpcContext->state = BTL_FTPC_STATE_DISCONNECTED;
   				/* Pass event to app */
				break;
   		   	}
   
       		case GOEP_EVENT_NO_SERVICE_FOUND:
   			{
   				BTL_LOG_INFO(("BTL_FTPC: Client %s No OBEX Service Found.", BtlFtpcGoepOpName(Event->oper)));
				ftpcContext->state = BTL_FTPC_STATE_DISCONNECTED;
				/* Pass event to app */
				break;
       		}
   
			case GOEP_EVENT_HEADER_RX:
			{
				if (Event->header.remainLen == 0) 
   				{
					/* Delivered to the application when it is time the pull file length */ 
					if ((Event->oper == GOEP_OPER_PULL) && (Event->header.type == OBEXH_LENGTH))
					{
						if (Event->info.pull.objectLen) 
						{
							FTPCSTORE_SetObjectLen( (FtpcStoreHandle)ftpcContext->pushpull.object, 
																Event->info.pull.objectLen);
			                
						}
					}

					/* Only indicate the last header indication.  This prevents  reporting the same header multiple times */
					BTL_LOG_INFO(("BTL_FTPC: Client %s Receiving an OBEX header: %s", BtlFtpcGoepOpName(Event->oper), 
               														BtlpObexHeaderType(Event->header.type)));
				}
				passEventToApp = FALSE;
				break;
			}
			
			case (GOEP_EVENT_TP_CONNECTED):
			{
				/* A new transport layer connection is established. */
				BTL_LOG_INFO(("BTL_FTPC: Transport layer connection has come up."));
				
				ftpcContext->state = BTL_FTPC_STATE_CONNECTED;
				
				/* Pass event to app */
				break;
			}
			
			case (GOEP_EVENT_TP_DISCONNECTED):
			{
				/* The transport layer connection has been disconnected. */
				BTL_LOG_INFO(("BTL_FTPC: Transport layer connection has been disconnected."));
				
#if OBEX_AUTHENTICATION == XA_ENABLED
				ftpcContext->obexAuthFlags = OBEX_AUTH_NONE;
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

				/* Clean up any active object */
				ftpcContext->currOper = FTPC_OPER_NONE;
				ftpcContext->state = BTL_FTPC_STATE_DISCONNECTED;

				/* Pass event to app */
				break;
			}
			
			case (GOEP_EVENT_CONTINUE):
			{
				/* Delivered to the application when it is time to issue another response. */ 
				/* During a Pull operation, keep looking for the server
				 * to send back the file length information. 
				 */
				if (Event->oper == GOEP_OPER_PULL)
				{
					if (Event->info.pull.objectLen) 
					{
						FTPCSTORE_SetObjectLen( (FtpcStoreHandle)ftpcContext->pushpull.object, 
															Event->info.pull.objectLen);
		                
					}
				}

				/* Always call continue to keep the requests flowing. */
				obStatus = FTP_ClientContinue(Event->handler);
				BTL_VERIFY_ERR((OB_STATUS_SUCCESS == obStatus), BT_STATUS_FAILED, ("The client is not expecting a continue"));
				
				passEventToApp = FALSE;
				break;
			}
			
			
#if OBEX_AUTHENTICATION == XA_ENABLED
			case (GOEP_EVENT_AUTH_CHALLENGE):
			{

				/* Authentication challenge request has been received */
				BTL_LOG_INFO(("BTL_FTPC: Received OBEX authentication challenge for %s operation.", BtlFtpcGoepOpName(Event->oper)));

#if OBEX_MAX_REALM_LEN > 0
				BTL_LOG_INFO(("BTL_FTPC: Realm: %s, Charset %d , Options %d.", 
				Event->challenge.realm+1, 
				Event->challenge.realm[0], 
				Event->challenge.options));
#endif /* OBEX_MAX_REALM_LEN > 0 */
				
				ftpcContext->obexAuthFlags |= OBEX_AUTH_RECEIVED_CHAL;

				/* Copy challenge to new event */
				btlFtpcData.ftpcEvent.info.challenge = Event->challenge;
				
				passEventToApp = FALSE;
				break;
			}
            
			case (GOEP_EVENT_AUTH_RESPONSE):
			{
				/* Authentication response has been received */
				BTL_LOG_INFO(("BTL_FTPC: Received OBEX authentication response for %s operation.", BtlFtpcGoepOpName(Event->oper)));
				
				if (FTP_ClientVerifyAuthResponse(Event->handler, (U8 *)ftpcContext->response.password, (U8)OS_StrLenUtf8(ftpcContext->response.password)) == TRUE) 
				{
					ftpcContext->obexAuthFlags |= OBEX_AUTH_AUTHENTICATED;
					BTL_LOG_INFO(("BTL_FTPS: UserId: %s, verified.", Event->response.userId));
				}
				else 
				{
					BTL_LOG_INFO(("BTL_FTPC: UserId: %s, failed verification.", Event->response.userId));
					
				}
	
				passEventToApp = FALSE;
				break;
			}
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
		
			case (GOEP_EVENT_ABORTED):
			{
				/* Indicates that the current operation failed or aborted. */
				BTL_LOG_INFO(("BTL_FTPC: GOEP %s The Operation was aborted, \"%s\".", BtlFtpcGoepOpName(Event->oper), ClientAbortMsg(Event->reason)));

				
				if (GOEP_OPER_CONNECT == Event->oper)
				{
					if (Event->reason != OBRC_SUCCESS)
					{
#if OBEX_AUTHENTICATION == XA_ENABLED		
						if (ftpcContext->obexAuthFlags & OBEX_AUTH_RECEIVED_CHAL)
						{
							/* Do-nothing */
						}
						else
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */					
						{
							/* Security failed disconnect transport */
							GOEP_TpDisconnect(&ftpcContext->client);
						}
					}
				}

#if OBEX_AUTHENTICATION == XA_ENABLED		
				if (ftpcContext->obexAuthFlags & OBEX_AUTH_RECEIVED_CHAL)
				{
					/* Setup the internal event */
					btlFtpcData.ftpcEvent.event = GOEP_EVENT_AUTH_CHALLENGE;
					btlFtpcData.ftpcEvent.oper = GOEP_OPER_CONNECT;
				
					/* Pass event to app */
				}
				else
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */					
				if (Event->oper == GOEP_OPER_PUSH && ftpcContext->pushpull.object)
				{
					rcode = FTPCSTORE_Close((FtpcStoreHandle*)(&(ftpcContext->pushpull.object)));
				
				}
				/* if pull operation aborted - remove all files, associated with last pull session */
				else if (Event->oper == GOEP_OPER_PULL && ftpcContext->pushpull.object)  
				{
					FTPCSTORE_Delete((FtpcStoreHandle *)(&(ftpcContext->pushpull.object)));
				}

				ftpcContext->currOper = FTPC_OPER_NONE;

				btlFtpcData.ftpcEvent.reason = Event->reason;
				/* Pass event to app */
				break;
			}
			
			case GOEP_EVENT_COMPLETE:

				/* Indicates that the operation has completed successfully. */
				BTL_LOG_INFO(("BTL_FTPC: GOEP %s operation complete.", BtlFtpcGoepOpName(Event->oper)));
				
#if OBEX_AUTHENTICATION == XA_ENABLED
				if((TRUE == ftpcContext->initiateObexAuth) && !(ftpcContext->obexAuthFlags & OBEX_AUTH_AUTHENTICATED)){
					status = FTP_Disconnect(&ftpcContext->client);
					break;
				}
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */	

				if (ftpcContext->pushpull.object) 
				{  
					/* only GET still needs to return the object */
					rcode = FTPCSTORE_Close((FtpcStoreHandle *)(&(ftpcContext->pushpull.object)));
					BTL_VERIFY_ERR((OBRC_SUCCESS == rcode), BT_STATUS_FAILED, ("Failed closing object"));
				}

				if (Event->oper == GOEP_OPER_CONNECT) 
				{
					/*	Transport and OBEX connection are established. */
					ftpcContext->state = BTL_FTPC_STATE_CONNECTED;
				} 
				else if ( Event->oper != GOEP_OPER_DISCONNECT) 
				{
        				/* Do-nothing */
				}

				ftpcContext->currOper = FTPC_OPER_NONE;
				break;
				
			default:
			{
				BTL_LOG_INFO(("BTL_FTPC: Client %s Unexpected event: %i", Event->event));
				passEventToApp = FALSE;
				break;
			}
		}

	}
	if (TRUE == passEventToApp)
	{
		/* Set the context in the event passed to app */
		btlFtpcData.event.ftpcContext = ftpcContext;

		/* Set the internal event in the event passed to app */
		btlFtpcData.event.ftpcEvent = &btlFtpcData.ftpcEvent;

		/* Pass the event to app */
		ftpcContext->callback(&btlFtpcData.event);
	}

	if (TRUE == sendDisableEvent)
	{
		/* Change the event to indicate context was disabled */
		 btlFtpcData.event.ftpcEvent->event = FTPC_EVENT_DISABLED;
		
		/* Pass the event to app */
		ftpcContext->callback(&btlFtpcData.event);
	}

	if (TRUE == notifyRadioOffComplete)
	{
		BtlModuleNotificationCompletionEvent moduleCompletionEvent;
		moduleCompletionEvent.type = BTL_MODULE_NOTIFICATION_COMPLETION_EVENT_TYPE_RADIO_OFF;
		moduleCompletionEvent.status = BT_STATUS_SUCCESS;
		
		BTL_ModuleCompleted(BTL_MODULE_TYPE_FTPC, &moduleCompletionEvent);
	}

	BTL_FUNC_END();
}

/*-------------------------------------------------------------------------------
 * BtlFtpcFtpcstoreCallback()
 *
 *		Handles OBSTORE progress events.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		obs [in] - related OBSTORE object.
 *
 * Returns:
 *		void.
 */
static void BtlFtpcFtpcstoreCallback(FtpcStoreEntry *obs)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtlContext *context;
	BtlFtpcContext *ftpcContext=0;

	BTL_FUNC_START("BtlFtpcFtpcstoreCallback");

	/* Find FTPCcontext according to given FTPCSTORE object */
	IterateList(btlFtpcData.contextsList, context, BtlContext *)
	{
		ftpcContext = (BtlFtpcContext *)context;
       	if (ftpcContext->pushpull.object == obs)
			break;
	}

	BTL_VERIFY_ERR((ftpcContext->pushpull.object == obs), BT_STATUS_FAILED, ("Failed to find FTPC context in progress callback"));

	/* Setup the internal event */
	btlFtpcData.ftpcEvent.event = FTPC_EVENT_OBJECT_PROGRESS;
	btlFtpcData.ftpcEvent.oper = ftpcContext->currOper;
	btlFtpcData.ftpcEvent.info.progress.name = obs->name;
	btlFtpcData.ftpcEvent.info.progress.nameLen = obs->nameLen;
	btlFtpcData.ftpcEvent.info.progress.currPos = obs->amount;
	btlFtpcData.ftpcEvent.info.progress.maxPos = obs->fileLen;

	/* Set the context in the event passed to app */
	btlFtpcData.event.ftpcContext = ftpcContext;

	/* Set the internal event in the event passed to app */
	btlFtpcData.event.ftpcEvent = &btlFtpcData.ftpcEvent;

	/* Pass the event to app */
	ftpcContext->callback(&btlFtpcData.event);

	BTL_FUNC_END();
}


/*-------------------------------------------------------------------------------
 * BtlFtpcBuildPushReq()
 *
 *		Creates and initializes Object Store entry which describes
 * 		a file that GOEP is to Push. 
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		ftpcContext [in] - The FTP client context.
 *
 *		ObPathAndName [in] - A string containing local file path and name.
 *   
 *      ObType [in] - File type. 
 *
 * Returns:
 *
 *		BT_STATUS_SUCCESS - FTPCSTORE creation and initialization succeeded. 
 *
 *		BT_STATUS_FAILED -  FTPCSTORE creation and initialization failed.
 */
static BtStatus BtlFtpcBuildPushReq(BtlFtpcContext *ftpcContext, const BtlObject *objToPush)
{
	U16     len = 0;
	ObexRespCode rcode = OBRC_SUCCESS;
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START("BtlPbapsBuildPushReq");

	BTL_VERIFY_ERR((objToPush), BT_STATUS_FAILED, ("BTL_FTPC: BtlFtpcBuildPullReq fails. No object.")); 
	len = OS_StrLenUtf8(objToPush->objectName);
	BTL_VERIFY_ERR(((objToPush->objectName) &&  (len > 0) && (len < FTPC_MAX_FILE_NAME_LENGTH)), BT_STATUS_FAILED, ("BTL_FTPC: Push failed. No remote file name"));
	len = OS_StrLenUtf8(objToPush->location.fsLocation.fsPath);
	BTL_VERIFY_ERR((objToPush->location.fsLocation.fsPath && (len > 0) && (len < FTPC_MAX_PATH_LENGTH)), BT_STATUS_FAILED, ("No path "));
	
	/* Get a new object store entry and set the name. */
	ftpcContext->pushpull.object = FTPCSTORE_New();

	BTL_VERIFY_ERR((ftpcContext->pushpull.object), BT_STATUS_FAILED, ("BTL_FTPC: BtlPbapsBuildPushReq fails. No object."));

	FTPCSTORE_AppendNameAscii((FtpcStoreHandle)ftpcContext->pushpull.object, objToPush->location.fsLocation.fsPath);
	
	rcode = FTPCSTORE_Open((FtpcStoreHandle)ftpcContext->pushpull.object);
	
	BTL_VERIFY_ERR(OBRC_SUCCESS == rcode, BT_STATUS_FAILED, ("BTL_FTPC: BtlFtpcBuildPullReq fails. Create file failed"));

	ftpcContext->pushpull.objectLen = FTPCSTORE_GetObjectLen((FtpcStoreHandle)ftpcContext->pushpull.object);

	/* Copy the applicaiton given paramenters to the btl_ftpc layer */
	OS_StrCpyUtf8(ftpcContext->operFileName, objToPush->objectName);
	
	/* Setting request parameters */
	ftpcContext->pushpull.name = objToPush->objectName;
	ftpcContext->pushpull.type = 0;

	/* Setting callback */
	FTPCSTORE_SetFtpcstoreCallback( (FtpcStoreHandle)ftpcContext->pushpull.object, BtlFtpcFtpcstoreCallback );
	
	BTL_FUNC_END();

	return (status);
}

/*-------------------------------------------------------------------------------
 * BtlFtpcBuildPullReq()
 *
 *		Creates and initializes Object Store entry which describes
 * 		a file that GOEP is to Pull. 
 *
 * Type:
 *		Synchronous
 *
 * Parameters: 
 *		ftpcContext [in] - The FTP client context.
 *
 *		objToPull [in] - includes File name & File type.     
 *
 * Returns:
 *
 *		BT_STATUS_SUCCESS - FTPCSTORE creation and initialization succeeded. 
 *
 *		BT_STATUS_FAILED -  FTPCSTORE creation and initialization failed.
 *
 */
static BtStatus BtlFtpcBuildPullReq(BtlFtpcContext *ftpcContext, const BtlObject *objToPull)
{
	U16     len = 0;
	ObexRespCode rcode = OBRC_SUCCESS;
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BtlFtpcBuildPullReq");
	
	/* Create an object store item for writing and set it's name */
	ftpcContext->pushpull.object = FTPCSTORE_New();
	
	BTL_VERIFY_ERR((NULL != objToPull), BT_STATUS_FAILED, ("BTL_FTPC: BtlFtpcBuildPullReq fails. No object."));
	len = OS_StrLenUtf8(objToPull->objectName);

	BTL_VERIFY_ERR(((0 != objToPull->objectName) && (len > 0) && (len < FTPC_MAX_FILE_NAME_LENGTH)), BT_STATUS_FAILED, ("BTL_FTPC: Push failed. No remote file name"));
	len = OS_StrLenUtf8(objToPull->location.fsLocation.fsPath);
	BTL_VERIFY_ERR((objToPull->location.fsLocation.fsPath && (len > 0) && (len < FTPC_MAX_PATH_LENGTH)), BT_STATUS_FAILED, ("No path "));
	
	/*A file name exists so put into the freshly created object store entry.*/
	FTPCSTORE_AppendNameAscii((FtpcStoreHandle)ftpcContext->pushpull.object, objToPull->location.fsLocation.fsPath);

	/* Copy the application given parameters to the btl_ftpc layer */
	OS_StrCpyUtf8(ftpcContext->operFileName, objToPull->objectName);
		
	/* Setting request parameters */
	ftpcContext->pushpull.name = ftpcContext->operFileName;
	ftpcContext->pushpull.type = 0; 

	rcode = FTPCSTORE_Create((FtpcStoreHandle)ftpcContext->pushpull.object);

	if (rcode != OBRC_SUCCESS) 
	{
	    FTPCSTORE_Close((FtpcStoreHandle *)&(ftpcContext->pushpull.object));
	}
	
	BTL_VERIFY_ERR(OBRC_SUCCESS == rcode, BT_STATUS_FAILED, ("BTL_FTPC: BtlFtpcBuildPullReq fails. Create file failed"));

	if ( rcode == OBRC_SUCCESS )
	{
		FTPCSTORE_SetFtpcstoreCallback( (FtpcStoreHandle)ftpcContext->pushpull.object, BtlFtpcFtpcstoreCallback );
	}

	BTL_FUNC_END();

	return (status);

}


/*-------------------------------------------------------------------------------
 * BtlFtpcBuildPullFolderReq()
 *
 *		Creates and initializes Object Store entry which describes
 * 		an XML folder listing that GOEP is to Pull. 
 *
 * Type:
 *		Synchronous
 *
 * Parameters: 
 *		ftpcContext [in] - The FTP client context.
 *
 *		objToPull [in] - includes File name & File type.     
 *
 * Returns:
 *
 *		BT_STATUS_SUCCESS - FTPCSTORE creation and initialization succeeded. 
 *
 *		BT_STATUS_FAILED -  FTPCSTORE creation and initialization failed.
 *
 */
static BtStatus BtlFtpcBuildPullFolderReq(BtlFtpcContext *ftpcContext, const BtlObject *objToPull)
{

	U16     len = 0;
	ObexRespCode rcode = OBRC_SUCCESS;
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BtlFtpcBuildPullReq");
	
	/* Create an object store item for writing and set it's name */
	ftpcContext->pushpull.object = FTPCSTORE_New();
	
	BTL_VERIFY_ERR((NULL != objToPull), BT_STATUS_FAILED, ("Failed to create FTPCSTORE object."));

	if (objToPull->objectLocation == BTL_OBJECT_LOCATION_FS)
	{
		((FtpcStoreHandle)(ftpcContext->pushpull.object))->object_location = BTL_OBJECT_LOCATION_FS;

		len = OS_StrLenUtf8(objToPull->location.fsLocation.fsPath);
		BTL_VERIFY_ERR((objToPull->location.fsLocation.fsPath && (len > 0) && (len < FTPC_MAX_PATH_LENGTH)), BT_STATUS_FAILED, ("No path "));

		/* A file name exists so put into the freshly created object store entry. */
		FTPCSTORE_AppendNameAscii((FtpcStoreHandle)ftpcContext->pushpull.object, objToPull->location.fsLocation.fsPath);

		/* Setting request parameters */		
		ftpcContext->pushpull.type = (U8*)obexFolderListingMimeType;

		ftpcContext->pushpull.name = 0;

		rcode = FTPCSTORE_Create((FtpcStoreHandle)ftpcContext->pushpull.object);

		if (rcode != OBRC_SUCCESS) 
		{
		    FTPCSTORE_Close((FtpcStoreHandle *)&(ftpcContext->pushpull.object));
		}
	}
	else
	{
		((FtpcStoreHandle)(ftpcContext->pushpull.object))->object_location = BTL_OBJECT_LOCATION_MEM;
		
		/* Setting request parameters */		
		ftpcContext->pushpull.type = (U8*)obexFolderListingMimeType;

		ftpcContext->pushpull.name = 0;
		
		rcode = FTPCSTORE_CreateMem((FtpcStoreHandle)ftpcContext->pushpull.object,
										objToPull->location.memLocation.memAddr,
										objToPull->location.memLocation.size);
	
	}
	

	BTL_VERIFY_ERR(OBRC_SUCCESS == rcode, BT_STATUS_FAILED, ("BTL_FTPC: BtlFtpcBuildPullReq fails. Create file failed"));
	
	if ( rcode == OBRC_SUCCESS )
	{
		FTPCSTORE_SetFtpcstoreCallback( (FtpcStoreHandle)ftpcContext->pushpull.object, BtlFtpcFtpcstoreCallback );
	}

	BTL_FUNC_END();

	return (status);
}

/*-------------------------------------------------------------------------------
 * BtlFtpcGoepOpName()
 *
 *		Return a pointer to the name of the current server operation.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		Op [in] - GOEP operation.
 *
 * Returns:
 *		ASCII String pointer representing the given GOEP operation.
 */
static const char *BtlFtpcGoepOpName(GoepOperation Op)
{
   switch (Op) {
   case GOEP_OPER_PUSH:
       return "Push";
   case GOEP_OPER_PULL:
       return "Pull";
   case GOEP_OPER_SETFOLDER:
       return "Set Folder";
   case GOEP_OPER_CONNECT:
       return "Connect";
   case GOEP_OPER_DISCONNECT:
       return "Disconnect";
   case GOEP_OPER_DELETE:
       return "Delete";
   case GOEP_OPER_ABORT:
       return "Abort";
   }
   return "Unknown";
}

/*-------------------------------------------------------------------------------
 * BtlpObexHeaderType()
 *
 *		Return a pointer to the name of the current server operation.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		type [in] - OBEX header type
 *
 * Returns:
 *		ASCII String pointer representing the OBEX header type.
 */
static const char *BtlpObexHeaderType(ObexHeaderType type)
{
    switch (type) {
    case OBEXH_COUNT:
        return "Count";
    case OBEXH_NAME:
        return "Name";
    case OBEXH_TYPE:
        return "Type";
    case OBEXH_LENGTH:
        return "Length";
    case OBEXH_TIME_ISO:
    case OBEXH_TIME_COMPAT:
        return "Time";
    case OBEXH_DESCRIPTION:
        return "Description";
    case OBEXH_TARGET:
        return "Target";
    case OBEXH_HTTP:
        return "HTTP";
    case OBEXH_WHO:
        return "Who";
    case OBEXH_CONNID:
        return "Conn ID";
    case OBEXH_APP_PARAMS:
        return "App Params";
    case OBEXH_AUTH_CHAL:
        return "Auth Challenge";
    case OBEXH_AUTH_RESP:
        return "Auth Resposne";
    case OBEXH_CREATOR_ID:
        return "Creator ID";
    case OBEXH_WAN_UUID:
        return "WAN UUID";
    case OBEXH_OBJ_CLASS:
        return "Object Class";   
    }
    return "UNKNOWN";
}

/*---------------------------------------------------------------------------
 *            ClientAbortMsg
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Return a pointer to a description of the OBEX Abort reason.
 *
 * Return:    ASCII String pointer.
 *
 */
static const char *ClientAbortMsg(ObexRespCode Reason)
{
    switch (Reason) {
    case OBRC_CLIENT_RW_ERROR:
        return "Client Internal R/W Error";
    case OBRC_LINK_DISCONNECT:
        return "Transport Link Disconnected";
    case OBRC_NOT_FOUND:
        return "Object Not Found";
    case OBRC_FORBIDDEN:
        return "Operation Forbidden";
    case OBRC_UNAUTHORIZED:
        return "Unauthorized Operation";
    case OBRC_NOT_ACCEPTABLE:
        return "Not Acceptable";
    case OBRC_CONFLICT:
        return "Conflict";
    case OBRC_USER_ABORT:
        return "User Aborted";
	case OBRC_PRECONDITION_FAILED:
		return "Precondition Failed";
    }
    return "Other";
}

BtStatus BtlFtpcBtlNotificationsCb(BtlModuleNotificationType notificationType)
{
	BtStatus		status = BT_STATUS_SUCCESS;
	ObStatus    	obStatus = OB_STATUS_SUCCESS;
	BtlFtpcContext	*ftpcContext;
	BtlContext		*base;
	
	BTL_FUNC_START("BtlFtpcBtlNotificationsCb");
	
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

			IterateList(btlFtpcData.contextsList, base, BtlContext *)
			{
				ftpcContext = (BtlFtpcContext *)base;

				switch (ftpcContext->state)
				{
					case (BTL_FTPC_STATE_DISCONNECTING):
					{
						/* FTPC state is now in the process of disabling */
						ftpcContext->state = BTL_FTPC_STATE_DISABLING;
						break;
					}
					case (BTL_FTPC_STATE_CONNECTED):
					{							
						obStatus = FTP_Disconnect(&ftpcContext->client);
						if (OB_STATUS_BUSY == obStatus)
						{
							obStatus = GOEP_TpDisconnect(&ftpcContext->client);
						}
											
						if (OB_STATUS_PENDING == obStatus)
						{
							/* FTPC state is now in the process of disabling */
							ftpcContext->state = BTL_FTPC_STATE_DISABLING;
							break;
						}
						
						break;
					}
					case (BTL_FTPC_STATE_CONNECTING):
					{
						/* In process of connecting, let it finish and then close the stream */

						/* FTPC state is now in the process of disabling */
						ftpcContext->state = BTL_FTPC_STATE_DISABLING;
						break;
					}
				}

				if (ftpcContext->state == BTL_FTPC_STATE_DISABLING)
				{
					ftpcContext->disableState |= BTL_FTPC_DISABLE_STATE_MASK_RADIO_OFF_ACTIVE;
					status = BT_STATUS_PENDING;
				}
				
			}/*IterateList(btlFtpcData.contextsList, base, BtlContext *)*/
		
			break; /*case BTL_MODULE_NOTIFICATION_RADIO_OFF:*/

		default:

			BTL_ERR(BT_STATUS_INTERNAL_ERROR, ("Invalid notification (%d)", notificationType));
			


	}; /*switch (notificationType)*/

	BTL_FUNC_END();
	
	return status;
}



#else /*BTL_CONFIG_FTPC ==   BTL_CONFIG_ENABLED*/

/*-------------------------------------------------------------------------------
 * BTL_FTPC_Init() - When  BTL_CONFIG_FTPC is disabled.
 */
BtStatus BTL_FTPC_Init(void)
{
    
   BTL_LOG_INFO(("BTL_FTPC_Init()  -  BTL_CONFIG_FTPC Disabled"));
  

    return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 * BTL_FTPC_Deinit() - When  BTL_CONFIG_FTPC is disabled.
 */
BtStatus BTL_FTPC_Deinit(void)
{
    BTL_LOG_INFO(("BTL_FTPC_Deinit() -  BTL_CONFIG_FTPC Disabled"));

    return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------*/



#endif /*BTL_CONFIG_FTPC ==   BTL_CONFIG_ENABLED*/





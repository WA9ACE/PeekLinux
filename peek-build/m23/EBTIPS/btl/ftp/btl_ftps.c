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
*   FILE NAME:      btl_ftps.c
*
*   DESCRIPTION:    This file contains the BTL FTP server implementation.
*
*   AUTHOR:         Ilan Elias
*
\*******************************************************************************/


#include "btl_config.h"
#include "btl_defs.h"

BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_FTPS);
#if BTL_CONFIG_FTPS ==   BTL_CONFIG_ENABLED

/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "btl_ftps.h"
#include "btl_pool.h"
#include "btl_commoni.h"
#include "config.h"
#include "ftp.h"
#include "goep.h"
#include "obmgr.h"
#include "osapi.h"
#include "utils.h"



/********************************************************************************
 *
 * Constants 
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BSL_FTPS_DEFAULT constant
 *
 *     Represents default security level for FTPS.
 */
#define BSL_FTPS_DEFAULT  (BSL_AUTHENTICATION_IN)


/*-------------------------------------------------------------------------------
 * FTPS_SERVICE_NAME_MAX_LEN constant
 *
 *     Represents max length of service name for FTPS.
 */
#define FTPS_SERVICE_NAME_MAX_LEN  (32)


/*---------------------------------------------------------------------------
 *
 * Object Store Function call table
 */
static const ObStoreFuncTable ftpsFuncTable = {
    OBM_Read, OBM_Write, OBM_GetObjectLen,
#if OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED
    OBM_ReadFlex,
#endif /* OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED */
};


/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/* Forward declarations */
typedef struct _BtlFtpsData BtlFtpsData;


/*-------------------------------------------------------------------------------
 * BtlFtpsInitState type
 *
 *     Defines the FTPS init state.
 */
typedef U8 BtlFtpsInitState;

#define BTL_FTPS_INIT_STATE_NOT_INTIALIZED					(0x00)
#define BTL_FTPS_INIT_STATE_INITIALIZED						(0x01)
#define BTL_FTPS_INIT_STATE_INITIALIZATION_FAILED			(0x02)
#define BTL_FTPS_INIT_STATE_DEINITIALIZATION_FAILED			(0x03)


/*-------------------------------------------------------------------------------
 * BtlFtpsState type
 *
 *     Defines the FTPS state of a specific context.
 */
typedef U8 BtlFtpsState;

#define BTL_FTPS_STATE_IDLE									(0x00)
#define BTL_FTPS_STATE_DISCONNECTED							(0x01)
#define BTL_FTPS_STATE_CONNECTED							(0x02)
#define BTL_FTPS_STATE_DISCONNECTING						(0x03)
#define BTL_FTPS_STATE_DISABLING							(0x04)

/*-------------------------------------------------------------------------------
 * BtlFtpsDisableStateMask type
 *
 *     Defines the FTPS disable state mask.
 */
typedef U8 BtlFtpsDisableStateMask;

#define BTL_FTPS_DISABLE_STATE_MASK_NONE				(0x00)
#define BTL_FTPS_DISABLE_STATE_MASK_DISABLE_ACTIVE		(0x01)
#define BTL_FTPS_DISABLE_STATE_MASK_RADIO_OFF_ACTIVE	(0x02)

/*-------------------------------------------------------------------------------
 * ObjectRequestState type
 *
 *     	Defines the state of the object request.
 */
typedef U8 ObjectRequestState;

#define OBJECT_REQUEST_STATE_IDLE				(0x00)
#define OBJECT_REQUEST_STATE_PUSH				(0x01)	
#define OBJECT_REQUEST_STATE_PULL				(0x02)	
#define OBJECT_REQUEST_STATE_DELETE				(0x03)	
#define OBJECT_REQUEST_STATE_CREATE_FOLDER		(0x04)	


/*-------------------------------------------------------------------------------
 * ObexAuthFlags type
 *
 *     Defines the OBEX authentication flags of a specific context.
 */
typedef U8 ObexAuthFlags;

#define OBEX_AUTH_NONE              								(0x00)
#define OBEX_AUTH_SENT_CHAL              						(0x01)
#define OBEX_AUTH_RECEIVED_CHAL  							(0x02)
#define OBEX_AUTH_RECEIVED_CHAL_CONT						(0x04)
#define OBEX_AUTH_AUTHENTICATED  							(0x08)

/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * _BtlFtpsContext structure
 *
 *     Represents BTL FTPS context.
 */
struct _BtlFtpsContext 
{
	/* Must be first field */
	BtlContext base;
	
	/* Internal FTPS server */
	GoepServerApp server;

	/* Associated callback with this context */
	BtlFtpsCallBack callback;

	/* Current FTPS state of the context */
	BtlFtpsState state;

	/* Defines which object requets should be delivered to callback */
	BtlFtpsObjectRequestMask objectRequestMask;

	/* Defines if read-only mode is enabled */
	BOOL readOnly;

	/* Null-terminated string representing current root path */
	BtlUtf8 rootPath[PATHMGR_MAX_PATH + 1];

	/* Current Object's handle */
	ObStoreMgr *object;

	/* Current GOEP operation */
	GoepOperation currOper;

	/* Current object request state */
	ObjectRequestState requestState;

	/* array that contain all the hide object  */
	U8 hideObject[BTL_CONFIG_FTPS_MAX_HIDE_OBJECT][BTHAL_FS_MAX_PATH_LENGTH +1];

	/* Defines if hidden mode is enable */
	BOOL	hideMode;

#if OBEX_AUTHENTICATION == XA_ENABLED

	/* Defines if server is initiating OBEX authentication */
	BOOL initiateObexAuth;

	/* OBEX Authentication flags */
	ObexAuthFlags obexAuthFlags;

	/* Null-terminated string representing the OBEX authentication realm */
	BtlUtf8 obexRealm[BTL_CONFIG_OBEX_MAX_REALM_LEN+ 2];

	/* Null-terminated string representing the OBEX authentication password */
	BtlUtf8 obexPassword[BTL_CONFIG_OBEX_MAX_PASS_LEN+ 1];

	/* Null-terminated string representing the OBEX authentication user Id */
	BtlUtf8 obexUserId[BTL_CONFIG_OBEX_MAX_USER_ID_LEN+ 1];

	/* Null-terminated string representing the OBEX authentication password needed by the client */
	BtlUtf8 obexPasswordClient[BTL_CONFIG_OBEX_MAX_PASS_LEN + 1];

	/* Null-terminated string representing the OBEX authentication user Id needed by the client */
	BtlUtf8 obexUserIdClient[BTL_CONFIG_OBEX_MAX_USER_ID_LEN + 1];

#endif	/* OBEX_AUTHENTICATION == XA_ENABLED */

	/* Service name plus 1 null-terminated char, plus 2 bytes for SDP_TEXT_8BIT */
	U8 serviceName[FTPS_SERVICE_NAME_MAX_LEN + 3];

	/* This flag indicates the disable state */
	BtlFtpsDisableStateMask disableState;
};


/*-------------------------------------------------------------------------------
 * BtlFtpsgData structure
 *
 *     Represents the data of th BTL FTPS module.
 */
struct _BtlFtpsData
{
	/* Pool of FTPS contexts */
	BTL_POOL_DECLARE_POOL(contextsPool, contextsMemory, BTL_CONFIG_FTPS_MAX_NUM_OF_CONTEXTS, sizeof(BtlFtpsContext));

	/* List of active FTPS contexts */
	ListEntry contextsList;

	/* Event passed to the application */
	BtlFtpsEvent event;

	/* Internal event generated in BTL_FTPS layer */
	FtpServerEvent ftpsEvent;
};



/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BtlFtpsInitState
 *
 *     Represents the current init state of FTPS module.
 */
static BtlFtpsInitState btlFtpsInitState = BTL_FTPS_INIT_STATE_NOT_INTIALIZED;


/*-------------------------------------------------------------------------------
 * btlFtpsData
 *
 *     Represents the data of FTPS module.
 */
static BtlFtpsData btlFtpsData;


/*-------------------------------------------------------------------------------
 * btlFtpsContextsPoolName
 *
 *     Represents the name of the FTPS contexts pool.
 */
static const char btlFtpsContextsPoolName[] = "FtpsContexts";


#if XA_CONTEXT_PTR == XA_ENABLED
extern FtpExtensionData  *Ftp;
#else /* XA_CONTEXT_PTR == XA_ENABLED */
extern FtpExtensionData   Ftp;
#endif /* XA_CONTEXT_PTR == XA_ENABLED */


/********************************************************************************
 *
 * Internal function prototypes
 *
 *******************************************************************************/
static BtStatus BtlFtpsDisable(BtlFtpsContext *ftpsContext);
static void BtlFtpsInitiateObexAuth(BtlFtpsContext *ftpsContext);
static void BtlFtpsObjectRequest(GoepServerEvent *Event);
static void BtlFtpsCallback(GoepServerEvent *Event);
static void BtlFtpsObmgrCallback(ObStoreMgr *obm);
static const char *BtlFtpsGoepOpName(GoepOperation Op);
static BOOL BtlFtpsIsFolderCreationNeeded(GoepServerEvent *Event);
static BtStatus BtlFtpsBtlNotificationsCb(BtlModuleNotificationType notificationType);


/********************************************************************************
 *
 * Function definitions
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BTL_FTPS_Init()
 */
BtStatus BTL_FTPS_Init(void)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_COMMON("BTL_FTPS_Init");
	
	BTL_VERIFY_ERR((BTL_FTPS_INIT_STATE_NOT_INTIALIZED == btlFtpsInitState), 
		BT_STATUS_FAILED, ("FTPS module is already initialized"));
	
	btlFtpsInitState = BTL_FTPS_INIT_STATE_INITIALIZATION_FAILED;

	status = BTL_POOL_Create(&btlFtpsData.contextsPool,
							btlFtpsContextsPoolName,
							btlFtpsData.contextsMemory, 
							BTL_CONFIG_FTPS_MAX_NUM_OF_CONTEXTS,
							sizeof(BtlFtpsContext));
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, 
		("FTPS contexts pool creation failed"));
	
	InitializeListHead(&btlFtpsData.contextsList);

	BTL_RegisterForCommonNotifications(BTL_MODULE_TYPE_FTPS, BtlFtpsBtlNotificationsCb);
		
	btlFtpsInitState = BTL_FTPS_INIT_STATE_INITIALIZED;

	BTL_FUNC_END_AND_UNLOCK();
	
	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_FTPS_Deinit()
 */
BtStatus BTL_FTPS_Deinit(void)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_COMMON("BTL_FTPS_Deinit");

	BTL_DeRegisterFromCommonNotifications(BTL_MODULE_TYPE_FTPS);
	
	BTL_VERIFY_ERR((BTL_FTPS_INIT_STATE_INITIALIZED == btlFtpsInitState), 
		BT_STATUS_FAILED, ("FTPS module is not initialized"));

	btlFtpsInitState = BTL_FTPS_INIT_STATE_DEINITIALIZATION_FAILED;

	BTL_VERIFY_ERR((IsListEmpty(&btlFtpsData.contextsList)), 
		BT_STATUS_FAILED, ("FTPS contexts are still active"));

	status = BTL_POOL_Destroy(&btlFtpsData.contextsPool);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, 
		("FTPS contexts pool destruction failed"));
		
	btlFtpsInitState = BTL_FTPS_INIT_STATE_NOT_INTIALIZED;

	BTL_FUNC_END_AND_UNLOCK();
	
	return (status);
}

 
/*-------------------------------------------------------------------------------
 * BTL_FTPS_Create()
 */
BtStatus BTL_FTPS_Create(BtlAppHandle *appHandle,
						const BtlFtpsCallBack ftpsCallback,
						const BtSecurityLevel *securityLevel,
						BtlFtpsContext **ftpsContext)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_FTPS_Create");

	BTL_VERIFY_ERR((0 != ftpsCallback), BT_STATUS_INVALID_PARM, ("Null ftpsCallback"));
	BTL_VERIFY_ERR((0 != ftpsContext), BT_STATUS_INVALID_PARM, ("Null ftpsContext"));

#if BT_SECURITY == XA_ENABLED

	if (0 != securityLevel)
	{
		status = BTL_VerifySecurityLevel(*securityLevel);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Invalid FTPS securityLevel"));
	}

#endif	/* BT_SECURITY == XA_ENABLED */

	/* Allocate memory for a unique new FTPS context */
	status = BTL_POOL_Allocate(&btlFtpsData.contextsPool, (void **)ftpsContext);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed allocating FTPS context"));

	status = BTL_HandleModuleInstanceCreation(appHandle, BTL_MODULE_TYPE_FTPS, &(*ftpsContext)->base);
	if (BT_STATUS_SUCCESS != status)
	{
		/* ftpsContext must be freed before we exit */
		BtStatus retVal = BTL_POOL_Free(&btlFtpsData.contextsPool, (void **)ftpsContext);
		BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == retVal), ("Failed freeing FTPS context"));

		/* This verify will fail and will handle the exception gracefully */
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed handling FTPS instance creation"));
	}

	/* Save the given callback */
	(*ftpsContext)->callback = ftpsCallback;

	/* Add the new FTPS context to the active contexts list */
	InsertTailList(&btlFtpsData.contextsList, &((*ftpsContext)->base.node));

#if BT_SECURITY == XA_ENABLED

	/* Save the given security level, or use default */
	(*ftpsContext)->server.secRecord.level = (BtSecurityLevel)((securityLevel == 0) ? (BSL_FTPS_DEFAULT) : (*securityLevel));

#endif	/* BT_SECURITY == XA_ENABLED */

	/* Init FTPS context state */
	(*ftpsContext)->state = BTL_FTPS_STATE_IDLE;

	/* All object requests are automatically accepted by default */
	(*ftpsContext)->objectRequestMask = BTL_FTPS_OBJECT_REQUEST_NONE;

	/* Read-only mode is disabled by default */
	(*ftpsContext)->readOnly = FALSE;

	(*ftpsContext)->rootPath[0] = '\0';

	(*ftpsContext)->object = 0;

	(*ftpsContext)->currOper = GOEP_OPER_NONE;

	(*ftpsContext)->requestState = OBJECT_REQUEST_STATE_IDLE;

	OS_MemSet((*ftpsContext)->hideObject,0, sizeof((*ftpsContext)->hideObject));

	/* Hidden mode is Enable by default */
	(*ftpsContext)->hideMode = TRUE;

#if OBEX_AUTHENTICATION == XA_ENABLED

	/* Server does not initiate OBEX authentication by default */
	(*ftpsContext)->initiateObexAuth = FALSE;

	(*ftpsContext)->obexAuthFlags = 0;

	/* The first byte of the string is the character set. The ASCII character set is value 0 */
	(*ftpsContext)->obexRealm[0] = '\0';
	(*ftpsContext)->obexRealm[1] = '\0';
	
	(*ftpsContext)->obexPassword[0] = '\0';
	
	(*ftpsContext)->obexUserId[0] = '\0';

#endif	/* OBEX_AUTHENTICATION == XA_ENABLED */

	(*ftpsContext)->disableState = BTL_FTPS_DISABLE_STATE_MASK_NONE;

	(*ftpsContext)->serviceName[0] = '\0';

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_FTPS_Destroy()
 */
BtStatus BTL_FTPS_Destroy(BtlFtpsContext **ftpsContext)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BOOL isAllocated = FALSE;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_FTPS_Destroy");

	BTL_VERIFY_ERR((0 != ftpsContext), BT_STATUS_INVALID_PARM, ("Null ftpsContext"));
	BTL_VERIFY_ERR((0 != *ftpsContext), BT_STATUS_INVALID_PARM, ("Null *ftpsContext"));
	BTL_VERIFY_ERR((BTL_FTPS_STATE_IDLE == (*ftpsContext)->state), BT_STATUS_IN_USE, ("FTPS context is in use"));

	status = BTL_POOL_IsElelementAllocated(&btlFtpsData.contextsPool, *ftpsContext, &isAllocated);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed locating given FTPS context"));
	BTL_VERIFY_ERR((TRUE == isAllocated), BT_STATUS_INVALID_PARM, ("Invalid FTPS context"));

	/* Remove the context from the list of all FTPS contexts */
	RemoveEntryList(&((*ftpsContext)->base.node));

	status = BTL_HandleModuleInstanceDestruction(&(*ftpsContext)->base);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed handling FTPS instance destruction"));

	status = BTL_POOL_Free(&btlFtpsData.contextsPool, (void **)ftpsContext);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("Failed freeing FTPS context"));

	/* Set the FTPS context to NULL */
	*ftpsContext = 0;

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_FTPS_Enable()
 */
BtStatus BTL_FTPS_Enable(BtlFtpsContext *ftpsContext, 
							const BtlUtf8 *serviceName,
							const BtlUtf8 *rootPath)
{
	BtStatus status = BT_STATUS_SUCCESS;
	ObStatus obStatus;
	BOOL retVal;
	U16 len = 0;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_FTPS_Enable");

	BTL_VERIFY_ERR((0 != ftpsContext), BT_STATUS_INVALID_PARM, ("Null ftpsContext"));
	BTL_VERIFY_ERR((BTL_FTPS_STATE_IDLE == ftpsContext->state), BT_STATUS_FAILED, ("FTPS context is already enabled"));

	if ((serviceName != 0) && ((len = OS_StrLenUtf8(serviceName)) != 0))
	{
		BTL_VERIFY_ERR((len <= FTPS_SERVICE_NAME_MAX_LEN), BT_STATUS_FAILED, ("FTPS service name is too long!"));
	}

	/* Try to set root path */
	status = BTL_FTPS_SetRootPath(ftpsContext, rootPath);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed setting root path"));
	ftpsContext->server.connFlags = GOEP_NEW_CONN;
	/* Setup callback, Object Store function table and register FTP server */
	ftpsContext->server.callback = BtlFtpsCallback;
	obStatus = FTP_RegisterServer(&ftpsContext->server, &ftpsFuncTable);
	BTL_VERIFY_ERR((OB_STATUS_SUCCESS == obStatus), BT_STATUS_FAILED, ("Failed to register FTP server"));

	/* Update serviceName */
	if (len > 0)
	{
		/* The first 2 bytes are for SDP_TEXT_8BIT */
		ftpsContext->serviceName[0] = DETD_TEXT + DESD_ADD_8BITS;
		ftpsContext->serviceName[1] = (U8)(len + 1);	/* Includes '\0' */
		
		OS_MemCopy((ftpsContext->serviceName + 2), serviceName, len);
		ftpsContext->serviceName[(len + 2)] = '\0';

		FTP(sdp)[ftpsContext->server.connId].attributes[4].len = (U16)(len + 3);
		FTP(sdp)[ftpsContext->server.connId].attributes[4].value = ftpsContext->serviceName;
	}

#if BT_SECURITY == XA_ENABLED

	/* Register security record */
	status = GOEP_RegisterServerSecurityRecord(&ftpsContext->server, ftpsContext->server.secRecord.level);
	if (BT_STATUS_SUCCESS != status)
	{
		obStatus = FTP_DeregisterServer(&ftpsContext->server);
		BTL_VERIFY_ERR_NORET((OB_STATUS_SUCCESS == obStatus), ("Failed deregistering FTP server"));

		/* This verify will fail and will handle the exception gracefully */
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed registering FTP security record"));
	}
	
#endif	/* BT_SECURITY == XA_ENABLED */

	/* Now initialize the Object Store Component */
	retVal = OBM_Init();
	if (TRUE != retVal)
	{
#if BT_SECURITY == XA_ENABLED

		/* First, try to unregister security record */
		status = GOEP_UnregisterServerSecurityRecord(&ftpsContext->server);
		BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == status), ("Failed unregistering FTPS security record"));
			
#endif	/* BT_SECURITY == XA_ENABLED */

		/* Second, try to deregister server */
		obStatus = FTP_DeregisterServer(&ftpsContext->server);
		BTL_VERIFY_ERR_NORET((OB_STATUS_SUCCESS == obStatus), ("Failed deregistering FTP server"));

		/* This verify will fail and will handle the exception gracefully */
		BTL_VERIFY_ERR((TRUE == retVal), BT_STATUS_FAILED, ("Object Store initialization failed!"));
	}

	/* FTPS state is now enabled */
	ftpsContext->state = BTL_FTPS_STATE_DISCONNECTED;

	BTL_LOG_INFO(("BTL_FTPS: Server is enabled."));

	BTL_FUNC_END_AND_UNLOCK();
	
	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_FTPS_Disable()
 */
BtStatus BTL_FTPS_Disable(BtlFtpsContext *ftpsContext)
{
	BtStatus status = BT_STATUS_PENDING;
	ObStatus obStatus;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_FTPS_Disable");

	BTL_VERIFY_ERR((0 != ftpsContext), BT_STATUS_INVALID_PARM, ("Null ftpsContext"));
	BTL_VERIFY_ERR((BTL_FTPS_STATE_IDLE != ftpsContext->state), BT_STATUS_FAILED, ("FTPS context is not enabled"));

	if (ftpsContext->state == BTL_FTPS_STATE_DISABLING)
	{
		ftpsContext->disableState |= BTL_FTPS_DISABLE_STATE_MASK_DISABLE_ACTIVE;

		BTL_RET(BT_STATUS_PENDING);
	}
	
	switch (ftpsContext->state)
	{
		case (BTL_FTPS_STATE_DISCONNECTING):
		{
			/* FTPS state is now in the process of disabling */
			ftpsContext->state = BTL_FTPS_STATE_DISABLING;
			break;
		}
		case (BTL_FTPS_STATE_CONNECTED):
		{
			obStatus = GOEP_ServerTpDisconnect(&ftpsContext->server);
			BTL_VERIFY_ERR((OB_STATUS_BUSY != obStatus), BT_STATUS_BUSY, ("FTPS is currently executing an operation"));
			
			if (OB_STATUS_PENDING == status)
			{
				/* FTPS state is now in the process of disabling */
				ftpsContext->state = BTL_FTPS_STATE_DISABLING;
				break;
			}
			
			/* No connection, try to deregister. Pass through to next case... */
		}
		case (BTL_FTPS_STATE_DISCONNECTED):
		{
			break;
		}
		default:
		{
			/* This verify will fail and will handle the exception gracefully */
			BTL_VERIFY_ERR((BTL_FTPS_STATE_DISCONNECTED == ftpsContext->state), BT_STATUS_FAILED, ("Disable failed, invalid FTPS state"));
			break;
		}
	}

	if (BTL_FTPS_STATE_DISABLING != ftpsContext->state)
	{
		status = BtlFtpsDisable(ftpsContext);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed disabling FTPS context"));
	}
	else
	{
		ftpsContext->disableState |= BTL_FTPS_DISABLE_STATE_MASK_DISABLE_ACTIVE;
	}
	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_FTPS_Disconnect()
 */
BtStatus BTL_FTPS_Disconnect(BtlFtpsContext *ftpsContext)
{
	BtStatus status = BT_STATUS_PENDING;
	ObStatus obStatus;
	
	BTL_FUNC_START_AND_LOCK("BTL_FTPS_Disconnect");

	BTL_VERIFY_ERR((0 != ftpsContext), BT_STATUS_INVALID_PARM, ("Null ftpsContext"));
	BTL_VERIFY_ERR((BTL_FTPS_STATE_CONNECTED == ftpsContext->state), BT_STATUS_NO_CONNECTION, ("FTPS context is not connected"));

	obStatus = GOEP_ServerTpDisconnect(&ftpsContext->server);
	BTL_VERIFY_ERR((OB_STATUS_BUSY != obStatus), BT_STATUS_BUSY, ("FTPS is currently executing an operation"));
	BTL_VERIFY_ERR((OB_STATUS_PENDING == obStatus), BT_STATUS_FAILED, ("Failed disconnecting FTP server"));
	
	/* FTPS state is now in the process of disconnecting */
	ftpsContext->state = BTL_FTPS_STATE_DISCONNECTING;

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}
/*-------------------------------------------------------------------------------
 * BTL_FTPS_GetConnectedDevice()
 */
BtStatus BTL_FTPS_GetConnectedDevice(BtlFtpsContext *ftpsContext, 
										BD_ADDR *bdAddr)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BOOL retVal;
	ObexTpConnInfo tpInfo;
	
	BTL_FUNC_START_AND_LOCK("BTL_FTPS_GetConnectedDevice");

	BTL_VERIFY_ERR((0 != ftpsContext), BT_STATUS_INVALID_PARM, ("Null ftpsContext"));
	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));
	BTL_VERIFY_ERR((BTL_FTPS_STATE_CONNECTED == ftpsContext->state), BT_STATUS_NO_CONNECTION, ("FTPS context is not connected"));

	tpInfo.size = sizeof(ObexTpConnInfo);
	tpInfo.tpType = OBEX_TP_BLUETOOTH;
	tpInfo.remDev = 0;

	retVal = GOEP_ServerGetTpConnInfo(&ftpsContext->server, &tpInfo);
	BTL_VERIFY_ERR((TRUE == retVal), BT_STATUS_NO_CONNECTION, ("FTPS get connection info failed"));

	BTL_VERIFY_ERR((0 != tpInfo.remDev), BT_STATUS_INTERNAL_ERROR, ("Unable to find connected remote device"));
	
	OS_MemCopy(bdAddr, (const U8 *)(&(tpInfo.remDev->bdAddr)), sizeof(BD_ADDR));

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_FTPS_ConfigObjectRequest()
 */
BtStatus BTL_FTPS_ConfigObjectRequest(BtlFtpsContext *ftpsContext,
										const BtlFtpsObjectRequestMask objectRequestMask)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_FTPS_ConfigObjectRequest");

	BTL_VERIFY_ERR((0 != ftpsContext), BT_STATUS_INVALID_PARM, ("Null ftpsContext"));

	ftpsContext->objectRequestMask = objectRequestMask;

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_FTPS_AcceptObjectRequest()
 */
BtStatus BTL_FTPS_AcceptObjectRequest(BtlFtpsContext *ftpsContext, 
										const BOOL accept)
{
	BtStatus status = BT_STATUS_PENDING;
	ObStatus obStatus;
	ObexRespCode rcode;

	BTL_FUNC_START_AND_LOCK("BTL_FTPS_AcceptObjectRequest");

	BTL_VERIFY_ERR((0 != ftpsContext), BT_STATUS_INVALID_PARM, ("Null ftpsContext"));
	BTL_VERIFY_ERR((BTL_FTPS_STATE_CONNECTED == ftpsContext->state), BT_STATUS_NO_CONNECTION, ("FTPS context is not connected"));
	BTL_VERIFY_ERR((OBJECT_REQUEST_STATE_IDLE != ftpsContext->requestState), BT_STATUS_FAILED, ("No pending request!"));

	switch (ftpsContext->requestState)
	{
		case (OBJECT_REQUEST_STATE_PUSH):
		{
			if (accept == FALSE)
			{
				/* Delete the file we created */
                if (ftpsContext->object)
                    OBM_Delete(&ftpsContext->object);

				obStatus = FTP_ServerAbort(&ftpsContext->server, OBRC_FORBIDDEN);
				BTL_VERIFY_ERR((OB_STATUS_SUCCESS == obStatus), BT_STATUS_FAILED, ("The server is unable to abort push!"));
			}
			break;
		}
		case (OBJECT_REQUEST_STATE_PULL):
		{
			if (accept == FALSE)
			{
				if (ftpsContext->object)
                	OBM_Close(&ftpsContext->object);

				obStatus = FTP_ServerAbort(&ftpsContext->server, OBRC_FORBIDDEN);
				BTL_VERIFY_ERR((OB_STATUS_SUCCESS == obStatus), BT_STATUS_FAILED, ("The server is unable to abort pull!"));
			}
			break;
		}
		case (OBJECT_REQUEST_STATE_DELETE):
		{
			if (accept == FALSE)
			{
				/* User rejected the object delete request, now actually reject delete object... */
				if (ftpsContext->object)
                	OBM_Close(&ftpsContext->object);

				obStatus = FTP_ServerAbort(&ftpsContext->server, OBRC_FORBIDDEN);
				BTL_VERIFY_ERR((OB_STATUS_SUCCESS == obStatus), BT_STATUS_FAILED, ("The server is unable to abort delete!"));
			}
			else
			{
				/* User accepted the object delete request, now actually delete object... */
				rcode = OBM_Delete(&ftpsContext->object);
				if (rcode != OBRC_SUCCESS)
			    	FTP_ServerAbort(&ftpsContext->server, rcode);
			}
			break;
		}
		case (OBJECT_REQUEST_STATE_CREATE_FOLDER):
		{
			if (accept == FALSE)
			{
				obStatus = FTP_ServerAbort(&ftpsContext->server, OBRC_FORBIDDEN);
				BTL_VERIFY_ERR((OB_STATUS_SUCCESS == obStatus), BT_STATUS_FAILED, ("The server is unable to abort set folder!"));
			}
			break;
		}
	}

	ftpsContext->requestState = OBJECT_REQUEST_STATE_IDLE;

	obStatus = FTP_ServerContinue(&ftpsContext->server);
	BTL_VERIFY_ERR((OB_STATUS_SUCCESS == obStatus), BT_STATUS_FAILED, ("The server is not expecting a continue"));

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_FTPS_SetReadOnlyMode()
 */
BtStatus BTL_FTPS_SetReadOnlyMode(BtlFtpsContext *ftpsContext, 
									const BOOL readOnly)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_FTPS_SetReadOnlyMode");

	BTL_VERIFY_ERR((0 != ftpsContext), BT_STATUS_INVALID_PARM, ("Null ftpsContext"));

	ftpsContext->readOnly = readOnly;

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_FTPS_SetRootPath()
 */
BtStatus BTL_FTPS_SetRootPath(BtlFtpsContext *ftpsContext, 
								const BtlUtf8 *rootPath)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BOOL retVal;
	U16 len;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_FTPS_SetRootPath");

	BTL_VERIFY_ERR((0 != ftpsContext), BT_STATUS_INVALID_PARM, ("Null ftpsContext"));
	BTL_VERIFY_ERR((0 != rootPath), BT_STATUS_INVALID_PARM, ("Null rootPath"));

	/* Check valid root path */
	len = OS_StrLenUtf8(rootPath);
	BTL_VERIFY_ERR((len <= PATHMGR_MAX_PATH), BT_STATUS_INVALID_PARM, ("Too long rootPath"));

	retVal = PATHMGR_SetRootFolder(rootPath);
	BTL_VERIFY_ERR((TRUE == retVal), BT_STATUS_FAILED, ("Failed setting root path"));

	/* Now safely copy the root path */
	OS_StrnCpyUtf8(ftpsContext->rootPath, rootPath, PATHMGR_MAX_PATH);
	ftpsContext->rootPath[PATHMGR_MAX_PATH] = '\0';

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_FTPS_GetRootPath()
 */
BtStatus BTL_FTPS_GetRootPath(BtlFtpsContext *ftpsContext, 
								BtlUtf8 **rootPath)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_FTPS_GetRootPath");

	BTL_VERIFY_ERR((0 != ftpsContext), BT_STATUS_INVALID_PARM, ("Null ftpsContext"));
	BTL_VERIFY_ERR((0 != rootPath), BT_STATUS_INVALID_PARM, ("Null rootPath"));

	*rootPath = ftpsContext->rootPath;

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_FTPS_GetCurrentPath()
 */
BtStatus BTL_FTPS_GetCurrentPath(BtlFtpsContext *ftpsContext, 
									BtlUtf8 **currentPath)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK("BTL_FTPS_GetCurrentPath");

	BTL_VERIFY_ERR((0 != ftpsContext), BT_STATUS_INVALID_PARM, ("Null ftpsContext"));
	BTL_VERIFY_ERR((0 != currentPath), BT_STATUS_INVALID_PARM, ("Null currentPath"));

	*currentPath = (BtlUtf8 *)PATHMGR_GetCurrentPath();

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

/*-------------------------------------------------------------------------------
 * BTL_FTPS_SetHiddenStatus()
 */
BtStatus BTL_FTPS_SetHiddenStatus(BtlFtpsContext *ftpsContext,const BtlUtf8 *objName
									,BOOL hideStat)

{
	BtStatus status = BT_STATUS_SUCCESS;
	U8	index;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_FTPS_SetHiddenStatus");

	BTL_VERIFY_ERR((0 != ftpsContext), BT_STATUS_INVALID_PARM, ("Null ftpsContext"));
	BTL_VERIFY_ERR((0 != objName), BT_STATUS_INVALID_PARM, ("Null objName"));

	if(TRUE == hideStat)
	{
		/* find the first free cell	*/
		for(index = 0;index < BTL_CONFIG_FTPS_MAX_HIDE_OBJECT;index++)
		{
			if( 0 == ftpsContext->hideObject[index][0])
				break;
		}

		BTL_VERIFY_ERR((BTL_CONFIG_FTPS_MAX_HIDE_OBJECT > index), BT_STATUS_INVALID_PARM, ("Can't find the free cell in enable operation"));
		
		/* set the object into the array	*/
		OS_StrCpyUtf8(ftpsContext->hideObject[index],objName);	
	}
	else
	{
		/* find the cell that contain the object	*/
		for(index = 0;index < BTL_CONFIG_FTPS_MAX_HIDE_OBJECT;index++)
		{
			if(OS_StrCmpUtf8(ftpsContext->hideObject[index],objName) == 0)
				break;
		}

		BTL_VERIFY_ERR((BTL_CONFIG_FTPS_MAX_HIDE_OBJECT > index), BT_STATUS_INVALID_PARM, ("Can't find the object in disable operation"));
		
		/* Clear the object from the array	*/
		OS_MemSet(ftpsContext->hideObject[index],0, sizeof(ftpsContext->hideObject[index]));
	}

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

/*-------------------------------------------------------------------------------
 * BTL_FTPS_GetHiddenStatus()
 */
BtStatus BTL_FTPS_GetHiddenStatus(BtlFtpsContext *ftpsContext,const BtlUtf8 *objName,
									BOOL *hideStat)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U8	index;


	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_FTPS_GetHiddenStatus");

	BTL_VERIFY_ERR((0 != ftpsContext), BT_STATUS_INVALID_PARM, ("Null ftpsContext"));
	BTL_VERIFY_ERR((0 != objName), BT_STATUS_INVALID_PARM, ("Null objName"));

	*hideStat = FALSE;
	
	/* find the cell that contain the object	*/
	for(index = 0;index < BTL_CONFIG_FTPS_MAX_HIDE_OBJECT;index++)
	{
		if(OS_StrCmpUtf8(ftpsContext->hideObject[index],objName) == 0)
			*hideStat = TRUE;
	}

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

/*-------------------------------------------------------------------------------
 * BTL_FTPS_EnableHiddenMode()
 */
BtStatus BTL_FTPS_EnableHiddenMode(BtlFtpsContext *ftpsContext)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_FTPS_EnableHiddenMode");

	BTL_VERIFY_ERR((0 != ftpsContext), BT_STATUS_INVALID_PARM, ("Null ftpsContext"));

	ftpsContext->hideMode = TRUE;

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
	
}

/*-------------------------------------------------------------------------------
 * BTL_FTPS_DisableHiddenMode()
 */
BtStatus BTL_FTPS_DisableHiddenMode(BtlFtpsContext *ftpsContext)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_FTPS_DisableHiddenMode");

	BTL_VERIFY_ERR((0 != ftpsContext), BT_STATUS_INVALID_PARM, ("Null ftpsContext"));

	ftpsContext->hideMode = FALSE;

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
	
}

/*-------------------------------------------------------------------------------
 * BTL_FTPS_ClearHiddenList()
 */
BtStatus BTL_FTPS_ClearHiddenList(BtlFtpsContext *ftpsContext)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_FTPS_ClearHiddenList");

	BTL_VERIFY_ERR((0 != ftpsContext), BT_STATUS_INVALID_PARM, ("Null ftpsContext"));

	OS_MemSet(ftpsContext->hideObject,0, sizeof(ftpsContext->hideObject));

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
	
}

/*-------------------------------------------------------------------------------
 * BTL_FTPS_Abort()
 */
BtStatus BTL_FTPS_Abort(BtlFtpsContext *ftpsContext, ObexRespCode respCode)
{
	BtStatus status = BT_STATUS_SUCCESS;
	ObStatus obStatus;

	BTL_FUNC_START_AND_LOCK("BTL_FTPS_Abort");

	BTL_VERIFY_ERR((0 != ftpsContext), BT_STATUS_INVALID_PARM, ("Null ftpsContext"));
	BTL_VERIFY_ERR((BTL_FTPS_STATE_CONNECTED == ftpsContext->state), BT_STATUS_NO_CONNECTION, ("FTPS context is not connected"));
	BTL_VERIFY_ERR((OBJECT_REQUEST_STATE_IDLE == ftpsContext->requestState), BT_STATUS_FAILED, ("Unable to abort while a request is pending!"));

	obStatus = FTP_ServerAbort(&ftpsContext->server, respCode);
	BTL_VERIFY_ERR((OB_STATUS_SUCCESS == obStatus), BT_STATUS_FAILED, ("Server abort falied"));

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


#if OBEX_AUTHENTICATION == XA_ENABLED
/*-------------------------------------------------------------------------------
 * BTL_FTPS_ConfigInitiatingObexAuthentication()
 *
 */
BtStatus BTL_FTPS_ConfigInitiatingObexAuthentication(BtlFtpsContext *ftpsContext, 
													const BOOL enableObexAuth,
													const BtlUtf8 *realm, 
													const BtlUtf8 *userId, 
													const BtlUtf8 *password)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U16 len;

	BTL_FUNC_START_AND_LOCK("BTL_FTPS_ConfigInitiatingObexAuthentication");

	BTL_VERIFY_ERR((0 != ftpsContext), BT_STATUS_INVALID_PARM, ("Null ftpsContext"));
	
	if (enableObexAuth == TRUE)
	{
		BTL_VERIFY_ERR((0 != password), BT_STATUS_INVALID_PARM, ("Null password"));
		if (realm != 0)
		{
			/* Check valid realm */
			len = OS_StrLenUtf8(realm);
			BTL_VERIFY_ERR((len <= BTL_CONFIG_OBEX_MAX_REALM_LEN), BT_STATUS_INVALID_PARM, ("Too long realm"));

			/* Now safely copy the realm, starting from the second byte */
			OS_StrnCpyUtf8((ftpsContext->obexRealm + 1), realm, BTL_CONFIG_OBEX_MAX_REALM_LEN);
			ftpsContext->obexRealm[BTL_CONFIG_OBEX_MAX_REALM_LEN] = '\0';
		}
		else
		{
			/* The first byte of the string is the character set. The ASCII character set is value 0 */
			ftpsContext->obexRealm[1] = '\0';
		}

		if (userId != 0)
		{
			/* Check valid userId */
			len = OS_StrLenUtf8(userId);
			BTL_VERIFY_ERR((len <= BTL_CONFIG_OBEX_MAX_USER_ID_LEN), BT_STATUS_INVALID_PARM, ("Too long userId"));

			/* Now safely copy the userId */
			OS_StrnCpyUtf8(ftpsContext->obexUserId, userId, BTL_CONFIG_OBEX_MAX_USER_ID_LEN);
			ftpsContext->obexUserId[BTL_CONFIG_OBEX_MAX_USER_ID_LEN] = '\0';
		}
		else
		{
			ftpsContext->obexUserId[0] = '\0';
		}
		
		/* Check valid password */
		len = OS_StrLenUtf8(password);
		BTL_VERIFY_ERR((len <= BTL_CONFIG_OBEX_MAX_PASS_LEN), BT_STATUS_INVALID_PARM, ("Too long password"));

		/* Now safely copy the password */
		OS_StrnCpyUtf8(ftpsContext->obexPassword, password, BTL_CONFIG_OBEX_MAX_PASS_LEN);
		ftpsContext->obexPassword[BTL_CONFIG_OBEX_MAX_PASS_LEN] = '\0';
	}

	ftpsContext->initiateObexAuth = enableObexAuth;

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_FTPS_ObexAuthenticationResponse()
 */
BtStatus BTL_FTPS_ObexAuthenticationResponse(BtlFtpsContext *ftpsContext, 
												const BtlUtf8 *userId, 
												const BtlUtf8 *password)
{
	BtStatus status = BT_STATUS_SUCCESS;
	ObStatus obStatus;
	ObexAuthResponse rsp;
	U16 len;

	BTL_FUNC_START_AND_LOCK("BTL_FTPS_ObexAuthenticationResponse");

	BTL_VERIFY_ERR((0 != ftpsContext), BT_STATUS_INVALID_PARM, ("Null ftpsContext"));
	BTL_VERIFY_ERR((0 != password), BT_STATUS_INVALID_PARM, ("Null password"));
	BTL_VERIFY_ERR((ftpsContext->obexAuthFlags & OBEX_AUTH_RECEIVED_CHAL), BT_STATUS_FAILED, ("No OBEX auth challenge was received"));

	if (userId != 0)
	{
		/* Check valid userId */
		len = OS_StrLenUtf8(userId);
		BTL_VERIFY_ERR((len <= BTL_CONFIG_OBEX_MAX_USER_ID_LEN), BT_STATUS_INVALID_PARM, ("Too long userId"));

		/* Now safely copy the userId */
		OS_StrnCpyUtf8(ftpsContext->obexUserIdClient, userId, BTL_CONFIG_OBEX_MAX_USER_ID_LEN);
		ftpsContext->obexUserIdClient[BTL_CONFIG_OBEX_MAX_USER_ID_LEN] = '\0';

		rsp.userId = ftpsContext->obexUserIdClient;
    	rsp.userIdLen = (U8)len;
	}
	else
	{
		rsp.userId = 0;
    	rsp.userIdLen = 0;
	}

	len = OS_StrLenUtf8(password);

	/* Now safely copy the password */
	OS_StrnCpyUtf8(ftpsContext->obexPasswordClient, password, BTL_CONFIG_OBEX_MAX_PASS_LEN);
	ftpsContext->obexPasswordClient[BTL_CONFIG_OBEX_MAX_PASS_LEN] = '\0';

	rsp.password = ftpsContext->obexPasswordClient;
	rsp.passwordLen = (U8)len;

	/* Respond to the challenge */
	ftpsContext->obexAuthFlags &= (~OBEX_AUTH_RECEIVED_CHAL);

    obStatus = FTP_ServerAuthenticate(&ftpsContext->server, &rsp, 0);
	BTL_VERIFY_ERR((OB_STATUS_SUCCESS == obStatus), BT_STATUS_INTERNAL_ERROR, ("Failed building authentication response header"));

	BTL_LOG_INFO(("BTL_FTPS: Built OBEX Authentication response header."));

	if (ftpsContext->obexAuthFlags & OBEX_AUTH_RECEIVED_CHAL_CONT)
	{
		/* Continue event occured, need to call server continue */
		ftpsContext->obexAuthFlags &= (~OBEX_AUTH_RECEIVED_CHAL_CONT);

		/* Always call continue to keep the requests flowing */
	    obStatus = FTP_ServerContinue(&ftpsContext->server);
		BTL_VERIFY_ERR((OB_STATUS_SUCCESS == obStatus), BT_STATUS_INTERNAL_ERROR, ("The server is not expecting a continue"));
	}

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BtlFtpsInitiateObexAuth()
 *
 *		Internal function for initiating OBEX authentication.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		ftpsContext [in] - pointer to the FTPS context.
 *
 * Returns:
 *		void.
 */
static void BtlFtpsInitiateObexAuth(BtlFtpsContext *ftpsContext)
{
	BtStatus status = BT_STATUS_SUCCESS;
    ObexAuthChallenge chal;
    U8 chalStr[20];
    ObStatus obStatus;
    TimeT time;
	U16 len;

	BTL_FUNC_START("BtlFtpsInitiateObexAuth");

    /* Only authenticate the client if the we should and if we haven't already */
    if ((TRUE == ftpsContext->initiateObexAuth) && !(ftpsContext->obexAuthFlags & OBEX_AUTH_AUTHENTICATED)) 
	{
        chal.challenge = chalStr;
        chal.challengeLen = 15;

        time = OS_GetSystemTime();
        chalStr[0] = (U8)(time);
        chalStr[1] = (U8)(time >> 8);
        chalStr[2] = (U8)(time >> 16);
        chalStr[3] = (U8)(time >> 24);
        chalStr[4] = ':';
        OS_MemCopy(chalStr+5, ftpsContext->obexPassword, OS_StrLenUtf8(ftpsContext->obexPassword));

		if (OS_StrLenUtf8(ftpsContext->obexUserId) > 0)
	        chal.options = AUTH_OPT_SEND_USERID;  	/* UserId must be sent in response */
		else
			chal.options = AUTH_OPT_DEFAULT;  		/* No options specified */

		if ((len = OS_StrLenUtf8((ftpsContext->obexRealm + 1))) > 0)
		{
			chal.realm = ftpsContext->obexRealm;
        	chal.realmLen = (U8)(len + 1);	/* realmLen include also first '\0' char */
		}
		else
		{
			chal.realm = 0;
        	chal.realmLen = 0;
		}

		/* Send challenge */
        obStatus = FTP_ServerAuthenticate(&ftpsContext->server, 0, &chal);
		BTL_VERIFY_ERR((OB_STATUS_SUCCESS == obStatus), BT_STATUS_INTERNAL_ERROR, ("Failed building authentication challenge header"));
		
        ftpsContext->obexAuthFlags |= OBEX_AUTH_SENT_CHAL;
        
        BTL_LOG_INFO(("BTL_FTPS: Built OBEX Authentication challenge header, realm \"%s\", userId \"%s\", password \"%s\".", 
			(ftpsContext->obexRealm+1), ftpsContext->obexUserId, ftpsContext->obexPassword));
    }

	BTL_FUNC_END();
}
#endif	/* OBEX_AUTHENTICATION == XA_ENABLED */


#if BT_SECURITY == XA_ENABLED
/*-------------------------------------------------------------------------------
 * BTL_FTPS_SetSecurityLevel()
 */
BtStatus BTL_FTPS_SetSecurityLevel(BtlFtpsContext *ftpsContext,
								  	const BtSecurityLevel *securityLevel)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_FTPS_SetSecurityLevel");

	BTL_VERIFY_ERR((0 != ftpsContext), BT_STATUS_INVALID_PARM, ("Null ftpsContext"));

	if (0 != securityLevel)
	{
		status = BTL_VerifySecurityLevel(*securityLevel);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Invalid FTPS securityLevel"));
	}

	/* Apply the new security level */
	ftpsContext->server.secRecord.level = (BtSecurityLevel)((securityLevel == 0) ? (BSL_FTPS_DEFAULT) : (*securityLevel));

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_FTPS_GetSecurityLevel()
 */
BtStatus BTL_FTPS_GetSecurityLevel(BtlFtpsContext *ftpsContext,
								  	BtSecurityLevel *securityLevel)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_FTPS_GetSecurityLevel");

	BTL_VERIFY_ERR((0 != ftpsContext), BT_STATUS_INVALID_PARM, ("Null ftpsContext"));
	BTL_VERIFY_ERR((0 != securityLevel), BT_STATUS_INVALID_PARM, ("Null securityLevel"));

	*securityLevel = ftpsContext->server.secRecord.level;
	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}
#endif	/* BT_SECURITY == XA_ENABLED */


/*-------------------------------------------------------------------------------
 * BtlFtpsDisable()
 *
 *		Internal function for deregistering security record and FTP server.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		ftpsContext [in] - pointer to the FTPS context.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */
static BtStatus BtlFtpsDisable(BtlFtpsContext *ftpsContext)
{
	BtStatus status = BT_STATUS_SUCCESS;
	ObStatus obStatus;

	BTL_FUNC_START("BtlFtpsDisable");

	/* Clean up any active object */
	ftpsContext->currOper = GOEP_OPER_NONE;
	ftpsContext->requestState = OBJECT_REQUEST_STATE_IDLE;
	if (ftpsContext->object)
		OBM_Close(&ftpsContext->object);

#if BT_SECURITY == XA_ENABLED

	/* First, try to unregister security record */
	status = GOEP_UnregisterServerSecurityRecord(&ftpsContext->server);
	BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == status), ("Failed unregistering FTPS security record"));
			
#endif	/* BT_SECURITY == XA_ENABLED */

	/* Second, try to deregister server */
	obStatus = FTP_DeregisterServer(&ftpsContext->server);
	BTL_VERIFY_ERR((OB_STATUS_SUCCESS == status), BT_STATUS_FAILED, ("Failed deregistering FTP server"));

	/* Now deinitialize the Object Store Component */
	OBM_Deinit();

	/* FTPS state is now idle */
	ftpsContext->state = BTL_FTPS_STATE_IDLE;

	BTL_LOG_INFO(("BTL_FTPS: Server is disabled."));

	BTL_FUNC_END();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BtlFtpsObjectRequest()
 *
 *		Internal function which handles object requests.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		Event [in] - Internal FTPS event.
 *
 * Returns:
 *		void.
 */
static void BtlFtpsObjectRequest(GoepServerEvent *Event)
{
	BtStatus status = BT_STATUS_SUCCESS;
	ObexRespCode rcode = OBRC_SUCCESS;
	ObStatus obStatus;
	BtlFtpsContext *ftpsContext;
	U8	index;

	BTL_FUNC_START("BtlFtpsObjectRequest");
	
	/* Find context according to given FTP event */
	ftpsContext = ContainingRecord(Event->handler, BtlFtpsContext, server);

	if (Event->info.pushpull.nameLen)
		BTL_LOG_INFO(("BTL_FTPS: Object request name \"%s\".", Event->info.pushpull.name));

	if (Event->info.pushpull.typeLen)
		BTL_LOG_INFO(("BTL_FTPS: Object request type \"%s\".", Event->info.pushpull.type));

	/* First check if the operation is allowed */
	if ((ftpsContext->readOnly == TRUE) && (Event->oper != GOEP_OPER_PULL))
	{
		/* Read only mode */
		obStatus = FTP_ServerAbort(Event->handler, OBRC_UNAUTHORIZED);
		BTL_VERIFY_ERR_NORET((obStatus == OB_STATUS_SUCCESS), ("Failed aborting object request!"));
	}
	else
	{
		ftpsContext->object = OBM_New();
		
		BTL_VERIFY_FATAL((ftpsContext->object), BT_STATUS_INTERNAL_ERROR, ("Failed allocating new object"));

	    OBM_AppendNameAscii(ftpsContext->object, Event->info.pushpull.name, Event->info.pushpull.nameLen);

	    OBM_AppendType(ftpsContext->object, (char*)Event->info.pushpull.type, Event->info.pushpull.typeLen);
	
		switch (Event->oper)
		{
		    case (GOEP_OPER_DELETE):
			{
                if (ftpsContext->objectRequestMask & BTL_FTPS_OBJECT_REQUEST_DELETE)
				{
					/* Need to ask user before performing delete operation */
					BTL_VERIFY_FATAL((ftpsContext->requestState == OBJECT_REQUEST_STATE_IDLE), 
						BT_STATUS_INTERNAL_ERROR, ("Already pending object request!"));

					/* When GOEP_EVENT_CONTINUE event will be generated, 
					this flag will be checked and the user will be notified of the request */
					ftpsContext->requestState = OBJECT_REQUEST_STATE_DELETE;
				}
				else
				{
					/* Delete object automatically */
					rcode = OBM_Delete(&ftpsContext->object);
			        if (rcode != OBRC_SUCCESS)
                    {
                        FTP_ServerAbort(Event->handler, rcode);
                    }
				}
		        break;
		    }
            
		    case (GOEP_OPER_PULL):
			{	
				/* Open for reading */
				/* update hide mode field	*/
				for(index = 0;index < BTL_CONFIG_FTPS_MAX_HIDE_OBJECT;index++)
				{
					ftpsContext->object->hideObjects[index] = ftpsContext->hideObject[index];
				}
				ftpsContext->object->hideMode = ftpsContext->hideMode;
				
		        rcode = OBM_Open(ftpsContext->object);
		        if (rcode == OBRC_SUCCESS) 
				{
					/* Check if the client is pulling a file */
					if (OBM_GetObjectLen(ftpsContext->object) != UNKNOWN_OBJECT_LENGTH)
					{
						/* Be nice, build a LENGTH header for the response */
		            	Event->info.pushpull.objectLen = OBM_GetObjectLen(ftpsContext->object);

						if (ftpsContext->objectRequestMask & BTL_FTPS_OBJECT_REQUEST_PULL)
						{
							/* Need to ask user before performing pull operation */
							BTL_VERIFY_FATAL((ftpsContext->requestState == OBJECT_REQUEST_STATE_IDLE), 
								BT_STATUS_INTERNAL_ERROR, ("Already pending object request!"));

							/* When GOEP_EVENT_CONTINUE event will be generated, 
							this flag will be checked and the user will be notified of the request */
							ftpsContext->requestState = OBJECT_REQUEST_STATE_PULL;
						}
					}
		        }
		        break;
		    }
            
		    case (GOEP_OPER_PUSH):
			{	
				/* Open for writing */
		        rcode = OBM_Create(ftpsContext->object);
				if (rcode == OBRC_SUCCESS)
				{
					OBM_SetObjectLen(ftpsContext->object, Event->info.pushpull.objectLen);

					if (ftpsContext->objectRequestMask & BTL_FTPS_OBJECT_REQUEST_PUSH)
					{
						/* Need to ask user before performing push operation */
						BTL_VERIFY_FATAL((ftpsContext->requestState == OBJECT_REQUEST_STATE_IDLE), 
							BT_STATUS_INTERNAL_ERROR, ("Already pending object request!"));

						/* When GOEP_EVENT_CONTINUE event will be generated, 
						this flag will be checked and the user will be notified of the request */
						ftpsContext->requestState = OBJECT_REQUEST_STATE_PUSH;
					}
				}
		        break;
		    }
	    }

		if (rcode != OBRC_SUCCESS)
        {
            BTL_LOG_INFO(("BTL_FTPS: Object request failed!"));
        }

		/* With delete, no need to accept or deleting killed the object pointer. */
		if ((Event->oper != GOEP_OPER_DELETE) && (rcode == OBRC_SUCCESS))
		{
            OBM_SetObmgrCallback(ftpsContext->object, BtlFtpsObmgrCallback);
		    obStatus = FTP_ServerAccept(Event->handler, ftpsContext->object);
			BTL_VERIFY_ERR_NORET((obStatus == OB_STATUS_SUCCESS), ("Failed accepting object request!"));
        }
}

	BTL_FUNC_END();
}


/*-------------------------------------------------------------------------------
 * BtlFtpsCallback()
 *
 *		Internal callback for handling FTP events in BTL layer.
 *		This function forwards events to registered application callback.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		Event [in] - Internal FTPS event.
 *
 * Returns:
 *		void.
 */
static void BtlFtpsCallback(GoepServerEvent *Event)
{
	BtStatus status = BT_STATUS_SUCCESS;
	ObStatus obStatus;
	BOOL retVal;
	ObexRespCode rcode;
	BtlFtpsContext *ftpsContext;
	U32	objectLen; 
	BOOL passEventToApp = TRUE;
	BOOL sendDisableEvent = FALSE;
	BOOL notifyRadioOffComplete = FALSE;
    
	BTL_FUNC_START("BtlFtpsCallback");
	
	/* Find context according to given FTP event */
	ftpsContext = ContainingRecord(Event->handler, BtlFtpsContext, server);

	/* Setup the internal event */
	btlFtpsData.ftpsEvent.event = Event->event;
	btlFtpsData.ftpsEvent.oper = Event->oper;

	/* First handle special case of disabling */
	if (ftpsContext->state == BTL_FTPS_STATE_DISABLING)
	{
		switch (Event->event)
		{
			case (GOEP_EVENT_TP_DISCONNECTED):
			{
				/* The transport layer connection has been disconnected. */
				BTL_LOG_INFO(("BTL_FTPS: Transport layer connection has been disconnected."));

#if OBEX_AUTHENTICATION == XA_ENABLED
				ftpsContext->obexAuthFlags = OBEX_AUTH_NONE;
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

				/* Clean up any active object */
				ftpsContext->currOper = GOEP_OPER_NONE;
				ftpsContext->requestState = OBJECT_REQUEST_STATE_IDLE;
				if (ftpsContext->object)
					OBM_Close(&ftpsContext->object);

				ftpsContext->state = BTL_FTPS_STATE_DISCONNECTED;

				if (ftpsContext->disableState & BTL_FTPS_DISABLE_STATE_MASK_DISABLE_ACTIVE)
				{				
					BtlFtpsDisable(ftpsContext);
					sendDisableEvent = TRUE;
				}
				if (ftpsContext->disableState & BTL_FTPS_DISABLE_STATE_MASK_RADIO_OFF_ACTIVE)
				{
					BtlContext *base;
					BtlFtpsContext *context;

					notifyRadioOffComplete = TRUE;

					IterateList(btlFtpsData.contextsList, base, BtlContext *)
					{
						context = (BtlFtpsContext *)base;
						if ((context->state != BTL_FTPS_STATE_DISCONNECTED) && (context->state != BTL_FTPS_STATE_IDLE))
								notifyRadioOffComplete = FALSE;			
					}
				}

				ftpsContext->disableState = BTL_FTPS_DISABLE_STATE_MASK_NONE;

				/* Pass event to app */
				break;
			}
			case (GOEP_EVENT_ABORTED):
			{
				/* Indicates that the current operation failed or aborted. */
				BTL_LOG_INFO(("BTL_FTPS: GOEP %s operation failed or aborted.", BtlFtpsGoepOpName(Event->oper)));
		        switch (Event->oper) 
				{
		    	case GOEP_OPER_SETFOLDER:
		            /* Cleanup any path changes that were started */
		            PATHMGR_FlushNewPath();
		            break;

				case GOEP_OPER_PUSH:
	                /* Delete the file we created */
	                if (ftpsContext->object)
	                    OBM_Delete(&ftpsContext->object);
                	break;

            	case GOEP_OPER_PULL:
                	if (ftpsContext->object)
                    	OBM_Close(&ftpsContext->object);
		            break;
		        }

				/* Pass event to app */
		        break;
			}
			default:
			{
				BTL_LOG_ERROR(("BTL_FTPS: Received unexpected event %d while disabling!", Event->event));	
				
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
			case (GOEP_EVENT_TP_CONNECTED):
			{
				/* A new transport layer connection is established. */
				BTL_LOG_INFO(("BTL_FTPS: Transport layer connection has come up."));
				
		        /* Reset to the root folder is for the Folder Browsing service. */
				retVal = PATHMGR_Switch2Root();
				BTL_VERIFY_FATAL((TRUE == retVal), BT_STATUS_INTERNAL_ERROR, ("Failed switching to root folder"));

				ftpsContext->state = BTL_FTPS_STATE_CONNECTED;
				
				/* Pass event to app */
				break;
			}

			case (GOEP_EVENT_TP_DISCONNECTED):
			{
				/* The transport layer connection has been disconnected. */
				BTL_LOG_INFO(("BTL_FTPS: Transport layer connection has been disconnected."));
				
#if OBEX_AUTHENTICATION == XA_ENABLED
        		ftpsContext->obexAuthFlags = 0;
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

				/* Clean up any active object */
				ftpsContext->currOper = GOEP_OPER_NONE;
				ftpsContext->requestState = OBJECT_REQUEST_STATE_IDLE;
				if (ftpsContext->object)
					OBM_Close(&ftpsContext->object);

				ftpsContext->state = BTL_FTPS_STATE_DISCONNECTED;

				/* Pass event to app */
				break;
			}

			case (GOEP_EVENT_CONTINUE):
			{
				/* Delivered to the application when it is time to issue another response. */

#if OBEX_AUTHENTICATION == XA_ENABLED
		        /* Initiate OBEX authentication if necessary. */
		        BtlFtpsInitiateObexAuth(ftpsContext);

				if (ftpsContext->obexAuthFlags & OBEX_AUTH_RECEIVED_CHAL)
				{
					/* Server received a challenge, and now a continue event */
					ftpsContext->obexAuthFlags |= OBEX_AUTH_RECEIVED_CHAL_CONT;
					passEventToApp = FALSE;
				}
				else 
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
				
                if (ftpsContext->requestState != OBJECT_REQUEST_STATE_IDLE)
				{
                    /* Here, we have a pending request (Push/Pull/Delete/Create Folder) */
                    
                    /* Check that the file to be deleted does exists.*/
                    if (ftpsContext->requestState == OBJECT_REQUEST_STATE_DELETE)
                    {
                        if (TRUE != PATHMGR_IsFileExists(ftpsContext->object->name))
                        {
                            rcode = OBM_Delete(&ftpsContext->object);

                            ftpsContext->requestState = OBJECT_REQUEST_STATE_IDLE;
                            
                            /* Always call continue to keep the requests flowing. */
			                obStatus = FTP_ServerContinue(Event->handler);
					        BTL_VERIFY_ERR((OB_STATUS_SUCCESS == obStatus), BT_STATUS_INTERNAL_ERROR, ("The server is not expecting a continue"));
					        passEventToApp = FALSE;
                            break;
                        }
                    } 
                    
                    /* Setup the internal event */
					btlFtpsData.ftpsEvent.event = FTPS_EVENT_OBJECT_REQUEST;
					btlFtpsData.ftpsEvent.info.request.name = ftpsContext->object->name;
					btlFtpsData.ftpsEvent.info.request.nameLen = ftpsContext->object->nameLen;
					btlFtpsData.ftpsEvent.info.request.type = ftpsContext->object->type;
					btlFtpsData.ftpsEvent.info.request.typeLen = ftpsContext->object->typeLen;
					btlFtpsData.ftpsEvent.info.request.objectType = BTL_FTPS_OBJECT_FILE;
					
                    if (ftpsContext->requestState == OBJECT_REQUEST_STATE_PUSH)
					{
						btlFtpsData.ftpsEvent.info.request.objectLen = ftpsContext->object->objectLen;
					}
					else if (ftpsContext->requestState == OBJECT_REQUEST_STATE_DELETE)
					{
						btlFtpsData.ftpsEvent.info.request.objectLen = 0;
						
                        if(TRUE == PATHMGR_IsFolderExists(ftpsContext->object->name))
                        {
                            btlFtpsData.ftpsEvent.info.request.objectType = BTL_FTPS_OBJECT_FOLDER;
                        }
                        else
                        {
                            btlFtpsData.ftpsEvent.info.request.objectType = BTL_FTPS_OBJECT_FILE;
                        }
					}	
					else
					{
						objectLen = OBM_GetObjectLen(ftpsContext->object);
						if (UNKNOWN_OBJECT_LENGTH == objectLen)
                        {
                            btlFtpsData.ftpsEvent.info.request.objectLen = 0;
                        }
						else
                        {
                        	btlFtpsData.ftpsEvent.info.request.objectLen = objectLen;
                        }
					}					
					
                    /* Pass event to app */
				}
				else if ((ftpsContext->objectRequestMask & BTL_FTPS_OBJECT_REQUEST_CREATE_FOLDER) && 
					(BtlFtpsIsFolderCreationNeeded(Event) == TRUE))
				{
					/* Need to ask user before performing create folder operation */
					ftpsContext->requestState = OBJECT_REQUEST_STATE_CREATE_FOLDER;

					/* Setup the internal event */
					btlFtpsData.ftpsEvent.event = FTPS_EVENT_OBJECT_REQUEST;
					btlFtpsData.ftpsEvent.info.request.name = Event->info.setfolder.name;
					btlFtpsData.ftpsEvent.info.request.nameLen = Event->info.setfolder.nameLen;
					btlFtpsData.ftpsEvent.info.request.type = 0;
					btlFtpsData.ftpsEvent.info.request.typeLen = 0;
					btlFtpsData.ftpsEvent.info.request.objectLen = 0;			
					
                    /* Pass event to app */
				}
				else
				{
                    /* Always call continue to keep the requests flowing. */
			        obStatus = FTP_ServerContinue(Event->handler);
					BTL_VERIFY_ERR((OB_STATUS_SUCCESS == obStatus), BT_STATUS_INTERNAL_ERROR, ("The server is not expecting a continue"));
					passEventToApp = FALSE;
				}
				break;
			}

#if OBEX_AUTHENTICATION == XA_ENABLED
			case (GOEP_EVENT_AUTH_CHALLENGE):
			{
				/* Authentication challenge request has been received */
				BTL_LOG_INFO(("BTL_FTPS: Received OBEX authentication challenge for %s operation.", BtlFtpsGoepOpName(Event->oper)));

#if OBEX_MAX_REALM_LEN > 0
				BTL_LOG_INFO(("BTL_FTPS: Realm: %s, Charset %d , Options %d.", 
				Event->challenge.realm+1, 
				Event->challenge.realm[0], 
				Event->challenge.options));
#endif /* OBEX_MAX_REALM_LEN > 0 */
				
		        ftpsContext->obexAuthFlags |= OBEX_AUTH_RECEIVED_CHAL;

				/* Copy challenge to new event */
				btlFtpsData.ftpsEvent.info.challenge = Event->challenge;

				/* Pass event to app */
				break;
			}
            
			case (GOEP_EVENT_AUTH_RESPONSE):
			{
				/* Authentication response has been received */
				BTL_LOG_INFO(("BTL_FTPS: Received OBEX authentication response for %s operation.", BtlFtpsGoepOpName(Event->oper)));
				
		        if (FTP_ServerVerifyAuthResponse(Event->handler, ftpsContext->obexPassword, (U8)OS_StrLenUtf8(ftpsContext->obexPassword)) == TRUE) 
		        {
			        ftpsContext->obexAuthFlags |= OBEX_AUTH_AUTHENTICATED;
					BTL_LOG_INFO(("BTL_FTPS: UserId: %s, verified.", Event->response.userId));
		    	}
		        else 
				{
			        FTP_ServerAbort(Event->handler, OBRC_UNAUTHORIZED);
					BTL_LOG_INFO(("BTL_FTPS: UserId: %s, failed verification.", Event->response.userId));
		    	}

				passEventToApp = FALSE;
				break;
			}
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

			case (GOEP_EVENT_START):
			{
				/* Indicates the start of a new operation */
				BTL_LOG_INFO(("BTL_FTPS: Starting GOEP %s operation.", BtlFtpsGoepOpName(Event->oper)));

				if ((Event->oper == GOEP_OPER_ABORT) && (ftpsContext->requestState != OBJECT_REQUEST_STATE_IDLE))
				{	
					/* Clean up any active object */
					if (ftpsContext->object)
					{
						if (ftpsContext->requestState == OBJECT_REQUEST_STATE_PUSH)
                        			{
                            			OBM_Delete(&ftpsContext->object);
                        			}
						else
                        			{
                           				 OBM_Close(&ftpsContext->object);
                        			}
					}

					ftpsContext->requestState = OBJECT_REQUEST_STATE_IDLE;

					obStatus = FTP_ServerContinue(&ftpsContext->server);
					BTL_VERIFY_ERR((OB_STATUS_SUCCESS == obStatus), BT_STATUS_FAILED, ("The server is not expecting a continue"));
				}

				/* Save current operation for this context */
				ftpsContext->currOper = Event->oper;

				/* Pass event to app */
				break;
			}
            
			case (GOEP_EVENT_DELETE_OBJECT):
			case (GOEP_EVENT_PROVIDE_OBJECT):
			{
				/* Instructs the server to provide or to delete an object. */
				BTL_LOG_INFO(("BTL_FTPS: Ongoing GOEP %s operation.", BtlFtpsGoepOpName(Event->oper)));
				
		        	if (Event->oper == GOEP_OPER_PUSH || Event->oper == GOEP_OPER_PULL ||
		            		Event->oper == GOEP_OPER_DELETE) 
		        	{
		            		/* Need Object to complete this request. */
		            		BtlFtpsObjectRequest(Event);
		        	}

				passEventToApp = FALSE;
				break;
			}
            
			case (GOEP_EVENT_ABORTED):
			{
				/* Indicates that the current operation failed or aborted. */
				BTL_LOG_INFO(("BTL_FTPS: GOEP %s operation failed or aborted.", BtlFtpsGoepOpName(Event->oper)));
		        
				switch (Event->oper) 
				{
					case GOEP_OPER_SETFOLDER:
						/* Cleanup any path changes that were started */
						PATHMGR_FlushNewPath();

						/* Pass event to app */
						break;

					case GOEP_OPER_PUSH:
						/* Delete the file we created */
						if (ftpsContext->object)
						{
							OBM_Delete(&ftpsContext->object);
						}
						/* Pass event to app */
						break;

					case GOEP_OPER_PULL:
						if (ftpsContext->object)
						{
							OBM_Close(&ftpsContext->object);
						}
						/* Pass event to app */
						break;

#if OBEX_AUTHENTICATION == XA_ENABLED
					case GOEP_OPER_CONNECT:
						if (ftpsContext->obexAuthFlags & OBEX_AUTH_SENT_CHAL)
						{
							ftpsContext->obexAuthFlags &= ~OBEX_AUTH_SENT_CHAL;

							/* Don't Pass event to app */
							passEventToApp = FALSE;                            
						}
						break;
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
				}

		 		break;
			}

			case (GOEP_EVENT_PRECOMPLETE):
			{	
				/* Delivered to server applications before the last response is going to be sent.
				   We want to process the Set Folder and Push commands now. 
				   This way we can report the status of the request back to the client. */
				if (Event->oper == GOEP_OPER_SETFOLDER) 
				{
	                /* Now apply the new path. */
	                if (Event->info.setfolder.reset == FALSE)
					{
						
	                    PATHMGR_AppendNewAscii(Event->info.setfolder.name, 
	                                           Event->info.setfolder.nameLen);
	                    PATHMGR_SetFlags(Event->info.setfolder.flags);

	                    if (Event->info.setfolder.flags & OSPF_BACKUP)
							BTL_LOG_INFO(("BTL_FTPS: Folder Backup requested."));
						
	                    if (Event->info.setfolder.nameLen)
	                        BTL_LOG_INFO(("BTL_FTPS: Switching to Folder: %s.", PATHMGR_GetNewPath()));

						
	                    switch (PATHMGR_Switch2NewFolder(ftpsContext->readOnly))
						{
							case 0 : 
								FTP_ServerAbort(Event->handler, OBRC_NOT_FOUND); 
								break;
							
                            case 1 : 
								FTP_ServerAbort(Event->handler, OBRC_UNAUTHORIZED);
								break;
							
                            case 2 : 
								FTP_ServerAbort(Event->handler, OBRC_NOT_FOUND); 
								break;
							
                            case 3 : 
								break;
							
                            default:
								/* Not supposed to get here */
								BTL_LOG_INFO(("BTL_FTPS: Wrong response code from function PATHMGR_Switch2NewFolder" ));
								break;
	                    }
	                } 
					else 
					{
	                    /* Reset to Root */
	                    BTL_LOG_INFO(("BTL_FTPS: Folder Reset requested."));
	                    if (PATHMGR_Switch2Root() == FALSE) 
						{
	                        /* Change dir failed, return error to client */
	                        FTP_ServerAbort(Event->handler, OBRC_NOT_FOUND);
	                    }
	                }
	            }
	            else if (Event->oper == GOEP_OPER_PUSH && ftpsContext->object) 
				{
	                /* See if we can successfully close the file we have received */
	                rcode = OBM_Close(&ftpsContext->object);
	                if (rcode != OBRC_SUCCESS)
                    {
                        FTP_ServerAbort(Event->handler, rcode);
                    }
	            }
				
				passEventToApp = FALSE;
				break;
			}
            
			case (GOEP_EVENT_COMPLETE):
			{
				/* Indicates that the operation has completed successfully. */
				BTL_LOG_INFO(("BTL_FTPS: GOEP %s operation complete.", BtlFtpsGoepOpName(Event->oper)));

				if (ftpsContext->object)
				{  
					/* only GET still needs to return the object */
		            rcode = OBM_Close(&ftpsContext->object);
					BTL_VERIFY_ERR((OBRC_SUCCESS == rcode), BT_STATUS_INTERNAL_ERROR, ("Failed closing object"));
		        }

				/* Pass event to app */
				break;
			}
            
			default:
			{
				passEventToApp = FALSE;
				break;
			}
		}
	}

	if (TRUE == passEventToApp)
	{
		/* Set the context in the event passed to app */
		btlFtpsData.event.ftpsContext = ftpsContext;

		/* Set the internal event in the event passed to app */
		btlFtpsData.event.ftpsEvent = &btlFtpsData.ftpsEvent;

		/* Pass the event to app */
		ftpsContext->callback(&btlFtpsData.event);
	}

	if (TRUE == sendDisableEvent)
	{
		/* Change the event to indicate context was disabled */
		 btlFtpsData.event.ftpsEvent->event = FTPS_EVENT_DISABLED;
		
		/* Pass the event to app */
		ftpsContext->callback(&btlFtpsData.event);
	}

	if (TRUE == notifyRadioOffComplete)
	{
		BtlModuleNotificationCompletionEvent moduleCompletionEvent;
		moduleCompletionEvent.type = BTL_MODULE_NOTIFICATION_COMPLETION_EVENT_TYPE_RADIO_OFF;
		moduleCompletionEvent.status = BT_STATUS_SUCCESS;
		
		BTL_ModuleCompleted(BTL_MODULE_TYPE_FTPS, &moduleCompletionEvent);
	}



	BTL_FUNC_END();
}


/*-------------------------------------------------------------------------------
 * BtlFtpsObmgrCallback()
 *
 *		Handles OBMGR progress events.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		obm [in] - related OBMGR object.
 *
 * Returns:
 *		void.
 */
static void BtlFtpsObmgrCallback(ObStoreMgr *obm)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtlContext *context;
	BtlFtpsContext *ftpsContext=0;

	BTL_FUNC_START("BtlFtpsObmgrCallback");
	
	/* Find FTPS context according to given OBMGR object */
	IterateList(btlFtpsData.contextsList, context, BtlContext *)
	{
		ftpsContext = (BtlFtpsContext *)context;
        if (ftpsContext->object == obm)
    	{
			break;
    	}
    }

	BTL_VERIFY_ERR((ftpsContext->object == obm), BT_STATUS_FAILED, ("Failed to find FTPS context in progress callback"));

	if(ftpsContext->requestState == OBJECT_REQUEST_STATE_IDLE)
    {
		/* Setup the internal event */
		btlFtpsData.ftpsEvent.event = FTPS_EVENT_OBJECT_PROGRESS;
		btlFtpsData.ftpsEvent.oper = ftpsContext->currOper;
		btlFtpsData.ftpsEvent.info.progress.name = obm->name;
		btlFtpsData.ftpsEvent.info.progress.nameLen = obm->nameLen;
		btlFtpsData.ftpsEvent.info.progress.currPos = obm->amount;
		btlFtpsData.ftpsEvent.info.progress.maxPos = obm->objectLen;

		/* Set the context in the event passed to app */
		btlFtpsData.event.ftpsContext = ftpsContext;

		/* Set the internal event in the event passed to app */
		btlFtpsData.event.ftpsEvent = &btlFtpsData.ftpsEvent;

		/* Pass the event to app */
		ftpsContext->callback(&btlFtpsData.event);
	}
    
	BTL_FUNC_END();
}


/*-------------------------------------------------------------------------------
 * BtlFtpsIsFolderCreationNeeded()
 *
 *		Returns TRUE if folder creation is needed.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		Event [in] - Internal FTPS event.
 *
 * Returns:
 *		TRUE if folder creation is needed.
 */
static BOOL BtlFtpsIsFolderCreationNeeded(GoepServerEvent *Event)
{
	BOOL retVal = FALSE;
	BtlFtpsContext *ftpsContext;

	BTL_FUNC_START("BtlFtpsIsFolderCreationNeeded");
	
	/* Find context according to given FTP event */
	ftpsContext = ContainingRecord(Event->handler, BtlFtpsContext, server);
	
	if ((Event->oper == GOEP_OPER_SETFOLDER) && 
		(Event->info.setfolder.reset == FALSE) && 
		(!(Event->info.setfolder.flags & OSPF_BACKUP)) && 
		(!(Event->info.setfolder.flags & OSPF_DONT_CREATE)) &&
		(Event->info.setfolder.nameLen) && 
		(ftpsContext->readOnly == FALSE) && 
		(PATHMGR_IsFolderExists(Event->info.setfolder.name) == FALSE))
	{
    	 retVal = TRUE;
    }

	BTL_FUNC_END();

	return (retVal);
}

/*-------------------------------------------------------------------------------
 * BtlFtpsGoepOpName()
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
static const char *BtlFtpsGoepOpName(GoepOperation Op)
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



BtStatus BtlFtpsBtlNotificationsCb(BtlModuleNotificationType notificationType)
{
	BtStatus		status = BT_STATUS_SUCCESS;
	ObStatus    	obStatus = OB_STATUS_SUCCESS;
	BtlFtpsContext	*ftpsContext;
	BtlContext		*base;
	

	BTL_FUNC_START("BtlFtpsBtlNotificationsCb");
	
	BTL_LOG_DEBUG(("Notification Type: %d", notificationType));
	
	switch (notificationType)
	{
		case BTL_MODULE_NOTIFICATION_RADIO_ON:

			/* Do nothing */
		
			break;

		case BTL_MODULE_NOTIFICATION_RADIO_OFF:
			/* Disconnect all connections , stop all connections in establishment, and any other process in progress 
				in all contexts
			*/

			IterateList(btlFtpsData.contextsList, base, BtlContext *)
			{
				ftpsContext = (BtlFtpsContext *)base;

				switch (ftpsContext->state)
				{
					case (BTL_FTPS_STATE_DISCONNECTING):
					{
						/* FTPS state is now in the process of disabling */
						ftpsContext->state = BTL_FTPS_STATE_DISABLING;
						break;
					}
					case (BTL_FTPS_STATE_CONNECTED):
					{							
						obStatus = GOEP_ServerTpDisconnect(&ftpsContext->server);
						BTL_VERIFY_ERR((OB_STATUS_BUSY != obStatus), BT_STATUS_BUSY, ("FTPS is currently executing an operation"));
						
						if (OB_STATUS_PENDING == obStatus)
						{
							/* FTPS state is now in the process of disabling */
							ftpsContext->state = BTL_FTPS_STATE_DISABLING;
							break;
						}
						
						break;
					}
				}
				if (ftpsContext->state == BTL_FTPS_STATE_DISABLING)
				{
					ftpsContext->disableState |= BTL_FTPS_DISABLE_STATE_MASK_RADIO_OFF_ACTIVE;
					status = BT_STATUS_PENDING;
				}
				
			}/*IterateList(btlFtpsData.contextsList, base, BtlContext *)*/
		
			break; /*case BTL_MODULE_NOTIFICATION_RADIO_OFF:*/

		default:

			BTL_ERR(BT_STATUS_INTERNAL_ERROR, ("Invalid notification (%d)", notificationType));
			
	}; /*switch (notificationType)*/

	BTL_FUNC_END();
	
	return status;
}


#else /*BTL_CONFIG_FTPS ==   BTL_CONFIG_ENABLED*/

/*-------------------------------------------------------------------------------
 * BTL_FTPS_Init() - When  BTL_CONFIG_FTPS is disabled.
 */
BtStatus BTL_FTPS_Init(void)
{
    
   BTL_LOG_INFO(("BTL_FTPS_Init()  -  BTL_CONFIG_FTPS Disabled"));
  

    return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 * BTL_FTPS_Deinit() - When  BTL_CONFIG_FTPS is disabled.
 */
BtStatus BTL_FTPS_Deinit(void)
{
    BTL_LOG_INFO(("BTL_FTPS_Deinit() -  BTL_CONFIG_FTPS Disabled"));

    return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------*/



#endif /*BTL_CONFIG_FTPS ==   BTL_CONFIG_ENABLED*/





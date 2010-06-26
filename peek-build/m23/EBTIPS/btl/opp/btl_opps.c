/*******************************************************************************\
*                                                  								*
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
*   FILE NAME:     	btl_opps.c
*
*   DESCRIPTION:	This file contains the BTL OPP Server role implementation.
*
*   AUTHOR:        	Alexander Udler
*
\*******************************************************************************/

#include "btl_config.h"
#include "btl_log.h"
#include "btl_defs.h"
#include "btl_opps.h"
#include "obstore.h"


BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_OPPS);

#if BTL_CONFIG_OPP == BTL_CONFIG_ENABLED

/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "btl_pool.h"
#include "btl_commoni.h"
#include "config.h"
#include "opush.h"
#include "goep.h"
#include "osapi.h"
#include "utils.h"
#include "obex.h"
#include "obstore.h"

/******************************************************************************** 
 *
 * Constants 
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BSL_OPPS_DEFAULT constant
 *
 *     Represents default security level for OPPS.
 */
#define BSL_OPPS_DEFAULT  (BSL_AUTHENTICATION_IN)


/*-------------------------------------------------------------------------------
 * OPPS_SERVICE_NAME_MAX_LEN constant
 *
 *     Represents max length of service name for OPPS.
 */
#define OPPS_SERVICE_NAME_MAX_LEN  (32)


#define BTL_OPPS_CHECK_FS_PATH(object)                                                \
          (OS_StrLenUtf8(object->location.fsLocation.fsPath) <                       \
              (BTHAL_FS_MAX_PATH_LENGTH + BTHAL_FS_MAX_FILE_NAME_LENGTH))



/********************************************************************************
 *
 * Internal Types
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BtlOppsInitState type
 *
 *     Defines the OPPS init state.
 */
typedef U8 BtlOppsInitState;

#define BTL_OPPS_INIT_STATE_NOT_INITIALIZED				(0x00)
#define BTL_OPPS_INIT_STATE_INITIALIZED					(0x01)
#define BTL_OPPS_INIT_STATE_INITIALIZATION_FAILED		(0x02)
#define BTL_OPPS_INIT_STATE_DEINITIALIZATION_FAILED		(0x03)


/*-------------------------------------------------------------------------------
 * BtlOppsState type
 *
 *     Defines the OPPS state of a specific context.
 */
typedef U8 BtlOppsState;

#define BTL_OPPS_STATE_DISABLED                         (0x00)
#define BTL_OPPS_STATE_DISCONNECTED                     (0x01)
#define BTL_OPPS_STATE_CONNECTED                        (0x02)
#define BTL_OPPS_STATE_DISCONNECTING                    (0x03)
#define BTL_OPPS_STATE_DISABLING                        (0x04)


/*-------------------------------------------------------------------------------
 * BtlOppsDisableStateMask type
 *
 *     Defines the OPPS disable state mask.
 */
typedef U8 BtlOppsDisableStateMask;

#define BTL_OPPS_DISABLE_STATE_MASK_NONE                (0x00)
#define BTL_OPPS_DISABLE_STATE_MASK_DISABLE_ACTIVE		(0x01)
#define BTL_OPPS_DISABLE_STATE_MASK_RADIO_OFF_ACTIVE    (0x02)


/*-------------------------------------------------------------------------------
 * ObjectRequestState type
 *
 *     	Defines the state of the object request.
 */
typedef U8 ObjectRequestState;

#define OBJECT_REQUEST_STATE_IDLE       				(0x00)
#define OBJECT_REQUEST_STATE_PUSH           			(0x01)	
#define OBJECT_REQUEST_STATE_PULL       				(0x02)	



/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/


/*-------------------------------------------------------------------------------
 * BtlOppsContext structure
 *
 *     Represents BTL OPPS context.
 */
struct _BtlOppsContext 
{
	/* Must be first field */
	BtlContext base;

	/* Internal OPPS channel */
	GoepServerApp server;

	/* Associated callback with this context */
	BtlOppsCallBack callback;

	/* Current OPPS state of the context */
	BtlOppsState state;

	/* Current Object's handle */
	OppObStoreEntry *object; 

	/* Current GOEP operation */
	GoepOperation currBasicOper;

	/* Saved pushedObjectLen when parsing header */
	BTHAL_U32 pushedObjectLen;

	/* Current object request state */
	ObjectRequestState requestState;

	/* Service name plus 1 null-terminated char, plus 2 bytes for SDP_TEXT_8BIT */
	U8 serviceName[OPPS_SERVICE_NAME_MAX_LEN + 3];

	/* Holds which formats are supported in the OPPS.*/
	U8 supportedFormats[4];

	/* This flag indicates the disable state */
	BtlOppsDisableStateMask disableState;
};


/*-------------------------------------------------------------------------------
 * BtlOppsData structure
 *
 *     Represents the data of th BTL OPPS module.
 */
struct _BtlOppsData
{
	/* Pool of OPPS contexts */
	BTL_POOL_DECLARE_POOL(contextsPool, contextsMemory, BTL_CONFIG_OPPS_MAX_NUM_OF_CONTEXTS, sizeof(BtlOppsContext));

	/* List of active OPPS contexts */
	ListEntry contextsList;

	/* Event passed to the application */
	BtlOppsEvent event;

	/* Internal event generated in BTL_OPPS layer */
	OppServerEvent oppsEvent;
};

typedef struct _BtlOppsData BtlOppsData;


/********************************************************************************
 *
 * Globals
 *


 *******************************************************************************/

/*---------------------------------------------------------------------------
 *
 * Default Object 
 * 
 * Note: this instance object may be configured different in OPP Client
 *        and OPP server (i.e. Memory vs. FS location)
 */
BtlObject OBS_DefaultObject;   


/*---------------------------------------------------------------------------
 *
 * Default Object fs storage
 */
static BtlUtf8 defaultFsPathBuf[BTHAL_FS_MAX_PATH_LENGTH + BTHAL_FS_MAX_FILE_NAME_LENGTH] = {'\0'};

 


/*------------------------------------------------------------------------
 *
 * Default fsPath (i.e. C:\QbInbox\default_object)
 */
BtlUtf8 OBS_DefaultDir[BTHAL_FS_MAX_PATH_LENGTH];


/*---------------------------------------------------------------------------
 *
 * Object Store Function fsPath
 */
static const ObStoreFuncTable oppsFuncTable = {
    OBSTORE_Read, OBSTORE_Write, OBSTORE_GetObjectLen
};
 
 /*-------------------------------------------------------------------------------
 * btlOppsInitState
 *
 *     Represents the current init state of OPPS module.
 */
static BtlOppsInitState btlOppsInitState = BTL_OPPS_INIT_STATE_NOT_INITIALIZED;


/*-------------------------------------------------------------------------------
 * btlOppsData
 *
 *     Represents the data of OPPS module.
 */
static BtlOppsData btlOppsData;


/*-------------------------------------------------------------------------------
 * btlOppsContextsPoolName
 *
 *     Represents the name of the OPPS contexts pool.
 */
static const char btlOppsContextsPoolName[] = "OppsContexts";


#if XA_CONTEXT_PTR == XA_ENABLED
extern OPushExtensionData  *OPush;
#else /* XA_CONTEXT_PTR == XA_ENABLED */
extern OPushExtensionData   OPush;
#endif /* XA_CONTEXT_PTR == XA_ENABLED */


/********************************************************************************
 *
 * Internal function prototypes
 *
 *******************************************************************************/
static ObexRespCode BtlOppsObjectRequest(GoepServerEvent *Event);
static BtStatus BtlOppsDisable(BtlOppsContext *oppsContext);
static void BtlOppsCallback(GoepServerEvent *Event);
static void BtlOppsObstoreCallback(OppObStoreEntry *obs);
static const char *BtlOppsGoepOpName(GoepOperation Op);
static const char *BtlpObexHeaderType(ObexHeaderType type);
static void BtlOppsUpdateFileAndType(OppObStoreHandle obs, GoepServerEvent *Event);
static BtStatus BtlOppsBtlNotificationsCb(BtlModuleNotificationType notificationType);


/********************************************************************************
 *
 * Function definitions
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BTL_OPPS_Init()
 *
 *		Init the OPPS module.
 */
BtStatus BTL_OPPS_Init(void)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_COMMON("BTL_OPPS_Init");
	
	BTL_VERIFY_ERR((BTL_OPPS_INIT_STATE_NOT_INITIALIZED == btlOppsInitState), 
		BT_STATUS_FAILED, ("OPPS module is already initialized"));
	
	btlOppsInitState = BTL_OPPS_INIT_STATE_INITIALIZATION_FAILED;

	status = BTL_POOL_Create(&btlOppsData.contextsPool,
							btlOppsContextsPoolName,
							btlOppsData.contextsMemory, 
							BTL_CONFIG_OPPS_MAX_NUM_OF_CONTEXTS,
							sizeof(BtlOppsContext));
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, 
          ("OPPS contexts pool creation failed"));
	
	InitializeListHead(&btlOppsData.contextsList);
		
	BTL_RegisterForCommonNotifications(BTL_MODULE_TYPE_OPPS, BtlOppsBtlNotificationsCb);
	
	btlOppsInitState = BTL_OPPS_INIT_STATE_INITIALIZED;

	BTL_FUNC_END_AND_UNLOCK();
	
	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_OPPS_Deinit()
 *
 *		Deinit the OPPS module.
 */
BtStatus BTL_OPPS_Deinit(void)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_COMMON("BTL_OPPS_Deinit");

	BTL_DeRegisterFromCommonNotifications(BTL_MODULE_TYPE_OPPS);
	
	BTL_VERIFY_ERR((BTL_OPPS_INIT_STATE_INITIALIZED == btlOppsInitState), 
        BT_STATUS_FAILED, ("OPPS module is not initialized"));

	btlOppsInitState = BTL_OPPS_INIT_STATE_DEINITIALIZATION_FAILED;

	BTL_VERIFY_ERR((IsListEmpty(&btlOppsData.contextsList)), BT_STATUS_FAILED, 
        ("OPPS contexts are still active"));

	status = BTL_POOL_Destroy(&btlOppsData.contextsPool);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, 
        ("OPPS contexts pool destruction failed"));
		
	btlOppsInitState = BTL_OPPS_INIT_STATE_NOT_INITIALIZED;

	BTL_FUNC_END_AND_UNLOCK();
	
	return (status);
}

 
/*-------------------------------------------------------------------------------
 * BTL_OPPS_Create()
 *
 *		Allocates a unique OPPS context.
 */
BtStatus BTL_OPPS_Create(BtlAppHandle *appHandle,
						const BtlOppsCallBack oppsCallback,
						const BtSecurityLevel *securityLevel,
						BtlOppsContext **oppsContext)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_OPPS_Create");

	BTL_VERIFY_ERR((0 != oppsCallback), BT_STATUS_INVALID_PARM, 
        ("Null oppsCallback"));

	BTL_VERIFY_ERR((0 != oppsContext), BT_STATUS_INVALID_PARM, 
        ("Null oppsContext"));

#if BT_SECURITY == XA_ENABLED

	if (0 != securityLevel)
	{
		status = BTL_VerifySecurityLevel(*securityLevel);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, 
               ("Invalid OPPS securityLevel"));
	}

#endif	/* BT_SECURITY == XA_ENABLED */

	/* Allocate memory for a unique new OPPS context */
	status = BTL_POOL_Allocate(&btlOppsData.contextsPool, (void **)oppsContext);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, 
                ("Failed allocating OPPS context"));

	status = BTL_HandleModuleInstanceCreation(appHandle, BTL_MODULE_TYPE_OPPS, 
                                                &(*oppsContext)->base);
	if (BT_STATUS_SUCCESS != status)
	{
		/* oppsContext must be freed before we exit */
		BtStatus retVal = BTL_POOL_Free(&btlOppsData.contextsPool, (void **)oppsContext);
		BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == retVal), 
              ("Failed freeing OPPS context"));

		/* This verify will fail and will handle the exception gracefully */
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, 
              ("Failed handling OPPS instance creation"));
	}

	/* Save the given callback */
	(*oppsContext)->callback = oppsCallback;

	/* Add the new OPPS context to the active contexts list */
	InsertTailList(&btlOppsData.contextsList, &((*oppsContext)->base.node));

#if BT_SECURITY == XA_ENABLED

	/* Save the given security level, or use default */
	(*oppsContext)->server.secRecord.level = (BtSecurityLevel)((securityLevel == 0) ? 
                                    (BSL_OPPS_DEFAULT) : (*securityLevel));

#endif	/* BT_SECURITY == XA_ENABLED */

	/* Init OPPS context state */
	(*oppsContext)->state = BTL_OPPS_STATE_DISABLED;

	(*oppsContext)->object = 0;

	(*oppsContext)->currBasicOper = GOEP_OPER_NONE;

	(*oppsContext)->requestState = OBJECT_REQUEST_STATE_IDLE;

	(*oppsContext)->disableState = BTL_OPPS_DISABLE_STATE_MASK_NONE;
    
	(*oppsContext)->serviceName[0] = '\0';

	(*oppsContext)->supportedFormats[0] = '\0';

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_OPPS_Destroy()
 *
 *		Releases a OPPS context (previously allocated with BTL_OPPS_Create).
 */
BtStatus BTL_OPPS_Destroy(BtlOppsContext **oppsContext)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BOOL isAllocated = FALSE;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_OPPS_Destroy");

	BTL_VERIFY_ERR((0 != oppsContext), BT_STATUS_INVALID_PARM, 
        ("Null oppsContext"));

	BTL_VERIFY_ERR((0 != *oppsContext), BT_STATUS_INVALID_PARM, 
        ("Null *oppsContext"));

	BTL_VERIFY_ERR((BTL_OPPS_STATE_DISABLED == (*oppsContext)->state), 
        BT_STATUS_IN_USE, ("OPPS context is in use"));

	status = BTL_POOL_IsElelementAllocated(&btlOppsData.contextsPool, 
        *oppsContext, &isAllocated);

	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, 
        ("Failed locating given OPPS context"));

	BTL_VERIFY_ERR((TRUE == isAllocated), BT_STATUS_INVALID_PARM, 
        ("Invalid OPPS context"));

	/* Remove the context from the list of all OPPS contexts */
	RemoveEntryList(&((*oppsContext)->base.node));

	status = BTL_HandleModuleInstanceDestruction(&(*oppsContext)->base);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, 
        ("Failed handling OPPS instance destruction"));

	status = BTL_POOL_Free(&btlOppsData.contextsPool, (void **)oppsContext);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, 
        ("Failed freeing OPPS context"));

	/* Set the OPPS context to NULL */
	*oppsContext = 0;

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_OPPS_Enable()
 *
 *		Enable OPPS and register OPP server SDP record, called after 
 *		BTL_OPPS_Create.
 */
BtStatus BTL_OPPS_Enable(BtlOppsContext *oppsContext, 
                        const BtlUtf8 *serviceName,
                        const BtlOppsSupportedFormatsMask supportedFormats)
{
	BtStatus status = BT_STATUS_SUCCESS;
	ObStatus obStatus;
	BOOL retVal;
	BTHAL_U32 len = 0;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_OPPS_Enable");

	BTL_VERIFY_ERR((0 != oppsContext), BT_STATUS_INVALID_PARM, 
        ("Null oppsContext"));

	BTL_VERIFY_ERR((BTL_OPPS_STATE_DISABLED == oppsContext->state), 
        BT_STATUS_FAILED, ("OPPS context is already enabled"));

	if ((NULL != serviceName) && ((len = OS_StrLenUtf8(serviceName)) != 0))
	{
		BTL_VERIFY_ERR((len <= OPPS_SERVICE_NAME_MAX_LEN), BT_STATUS_FAILED, ("OPPS service name is too long!"));
	}
    
	/* Setup callback, Object Store function table and register OPP server */
	oppsContext->server.callback = BtlOppsCallback;
	oppsContext->server.connFlags = GOEP_NEW_CONN;
	obStatus = OPUSH_RegisterServer(&oppsContext->server, &oppsFuncTable);
	
	BTL_VERIFY_ERR((OB_STATUS_SUCCESS == obStatus), BT_STATUS_FAILED, 
		("Failed to register OPP server"));

	/* Update serviceName */
	if (len > 0)
	{
		/* The first 2 bytes are for SDP_TEXT_8BIT */
		oppsContext->serviceName[0] = DETD_TEXT + DESD_ADD_8BITS;
		oppsContext->serviceName[1] = (U8)(len + 1);	/* Includes '\0' */
		
		OS_MemCopy((oppsContext->serviceName + 2), serviceName, len);
		oppsContext->serviceName[(len + 2)] = '\0';

		OPP(sdp)[oppsContext->server.connId].attributes[4].len = (U16)(len + 3);
		OPP(sdp)[oppsContext->server.connId].attributes[4].value = oppsContext->serviceName;
	}


	/* Update the supported pushed object formats */	
	oppsContext->supportedFormats[0] = DETD_SEQ + DESD_ADD_8BITS;
	oppsContext->supportedFormats[1] = 2;
	oppsContext->supportedFormats[2] = DETD_UINT + DESD_1BYTE;
	oppsContext->supportedFormats[3] = supportedFormats;

	OPP(sdp)[oppsContext->server.connId].attributes[5].value = oppsContext->supportedFormats;
	

#if BT_SECURITY == XA_ENABLED

	/* Register security record */
	status = GOEP_RegisterServerSecurityRecord(&oppsContext->server, 
                    oppsContext->server.secRecord.level);
	
    if (BT_STATUS_SUCCESS != status)
	{
		obStatus = OPUSH_DeregisterServer(&oppsContext->server);
		
		BTL_VERIFY_ERR_NORET((OB_STATUS_SUCCESS == obStatus), 
            ("Failed deregistering OPP server"));

		/* This verify will fail and will handle the exception gracefully */
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, 
            ("Failed registering OPP security record"));
	}
	
#endif	/* BT_SECURITY == XA_ENABLED */

	/* Now initialize the Object Store Component */
	retVal = OBSTORE_Init();
	
	if (TRUE != retVal)
	{

#if BT_SECURITY == XA_ENABLED

		/* First, try to unregister security record */
		status = GOEP_UnregisterServerSecurityRecord(&oppsContext->server);
		BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == status), 
			("Failed unregistering OPPS security record"));
			
#endif	/* BT_SECURITY == XA_ENABLED */

		/* Second, try to deregister server */
		obStatus = OPUSH_DeregisterServer(&oppsContext->server);
		BTL_VERIFY_ERR_NORET((OB_STATUS_SUCCESS == obStatus), 
			("Failed deregistering OPP server"));

		/* This verify will fail and will handle the exception gracefully */
		BTL_VERIFY_ERR((TRUE == retVal), BT_STATUS_FAILED, 
			("Object Store initialization failed!"));
	}

	/* OPPS state is now enabled */
	oppsContext->state = BTL_OPPS_STATE_DISCONNECTED;

	BTL_LOG_INFO(("Server is enabled."));

	BTL_FUNC_END_AND_UNLOCK();
	
	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_OPPS_Disable()
 *
 *		Disable OPPS, called before BTL_OPPS_Destroy.
 */
BtStatus BTL_OPPS_Disable(BtlOppsContext *oppsContext)
{
	BtStatus status = BT_STATUS_PENDING;
	ObStatus obStatus;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_OPPS_Disable");

	BTL_VERIFY_ERR((0 != oppsContext), BT_STATUS_INVALID_PARM, 
        ("Null oppsContext"));

	BTL_VERIFY_ERR((BTL_OPPS_STATE_DISABLED != oppsContext->state), 
        BT_STATUS_FAILED, ("OPPS context is not enabled"));
    

	if (oppsContext->state == BTL_OPPS_STATE_DISABLING)
	{
		oppsContext->disableState |= BTL_OPPS_DISABLE_STATE_MASK_DISABLE_ACTIVE;

		BTL_RET(BT_STATUS_PENDING);
	}
    
	switch (oppsContext->state)
	{
		
		case (BTL_OPPS_STATE_DISCONNECTING):
		{
			/* OPPS state is now in the process of disabling */
			oppsContext->state = BTL_OPPS_STATE_DISABLING;
			break;
		}
		
		case (BTL_OPPS_STATE_CONNECTED):
		{
			obStatus = GOEP_ServerTpDisconnect(&oppsContext->server);
            
			BTL_VERIFY_ERR((OB_STATUS_BUSY != obStatus), BT_STATUS_BUSY, 
                ("OPPS is currently executing an operation"));
			
			if (OB_STATUS_PENDING == status)
			{
				/* OPPS state is now in the process of disabling */
				oppsContext->state = BTL_OPPS_STATE_DISABLING;
				break;
			}
			
			/* No connection, try to deregister. Pass through to next case... */
		}
		
		case (BTL_OPPS_STATE_DISCONNECTED):
		{
			break;
		}
		
		default:
		{
			/* This verify will fail and will handle the exception gracefully */
			BTL_VERIFY_ERR((BTL_OPPS_STATE_DISCONNECTED == oppsContext->state), 
				 BT_STATUS_FAILED, ("Disable failed, invalid OPPS state"));
			break;
		}
	}

	if (BTL_OPPS_STATE_DISABLING != oppsContext->state)
	{
		status = BtlOppsDisable(oppsContext);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed disabling OPPS context"));
	}
	else
	{
		oppsContext->disableState |= BTL_OPPS_DISABLE_STATE_MASK_DISABLE_ACTIVE;
	}
	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_OPPS_Disconnect()
 *
 *		Initiates the disconnection of the server transport connection.
 */
BtStatus BTL_OPPS_Disconnect(BtlOppsContext *oppsContext)
{
	BtStatus status = BT_STATUS_PENDING;
	ObStatus obStatus;
	
	BTL_FUNC_START_AND_LOCK("BTL_OPPS_Disconnect");

	BTL_VERIFY_ERR((0 != oppsContext), BT_STATUS_INVALID_PARM, 
        ("Null oppsContext"));

	BTL_VERIFY_ERR((BTL_OPPS_STATE_CONNECTED == oppsContext->state), 
        BT_STATUS_NO_CONNECTION, ("OPPS context is not connected"));

	obStatus = GOEP_ServerTpDisconnect(&oppsContext->server);
	BTL_VERIFY_ERR((OB_STATUS_BUSY != obStatus), BT_STATUS_BUSY, 
        ("OPPS is currently executing an operation"));

	BTL_VERIFY_ERR((OB_STATUS_PENDING == obStatus), BT_STATUS_FAILED, 
        ("Failed disconnecting OPP server"));
	
	/* OPPS state is now in the process of disconnecting */
	oppsContext->state = BTL_OPPS_STATE_DISCONNECTING;

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_OPPS_GetConnectedDevice()
 *
 *		This function returns the connected device.
 */
BtStatus BTL_OPPS_GetConnectedDevice(BtlOppsContext *oppsContext, 
                                     BD_ADDR *bdAddr)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BOOL retVal;
	ObexTpConnInfo tpInfo;
	
	BTL_FUNC_START_AND_LOCK("BTL_OPPS_GetConnectedDevice");

	BTL_VERIFY_ERR((0 != oppsContext), BT_STATUS_INVALID_PARM, 
        ("Null oppsContext"));

	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, 
        ("Null bdAddr"));

	BTL_VERIFY_ERR((BTL_OPPS_STATE_CONNECTED == oppsContext->state), 
        BT_STATUS_NO_CONNECTION, ("OPPS context is not connected"));

	tpInfo.size = sizeof(ObexTpConnInfo);
	tpInfo.tpType = OBEX_TP_BLUETOOTH;
	tpInfo.remDev = 0;

	retVal = GOEP_ServerGetTpConnInfo(&oppsContext->server, &tpInfo);
	BTL_VERIFY_ERR((TRUE == retVal), BT_STATUS_NO_CONNECTION, 
        ("OPPS get connection info failed"));

	BTL_VERIFY_ERR((0 != tpInfo.remDev), BT_STATUS_INTERNAL_ERROR, 
        ("Unable to find connected remote device"));
	
	OS_MemCopy(bdAddr, (&(tpInfo.remDev->bdAddr)), sizeof(BD_ADDR));

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_OPPS_AcceptObjectRequest()
 *
 *		This function is called after receiving object request from the client,
 *		accept - whether to accept or reject the request.
 *      object - holds Object Parameters (i.e. ObjectName, Location, Size)
 */
BtStatus BTL_OPPS_AcceptObjectRequest(BtlOppsContext *oppsContext, 
										const BOOL accept,
										const BtlObject *object)
{
	BtStatus status = BT_STATUS_PENDING;
	ObStatus obStatus;
	ObexRespCode rcode;
    		    
	BTL_FUNC_START_AND_LOCK("BTL_OPPS_AcceptObjectRequest");

	BTL_VERIFY_ERR((0 != oppsContext), BT_STATUS_INVALID_PARM, 
        ("Null oppsContext"));

	BTL_VERIFY_ERR((BTL_OPPS_STATE_CONNECTED == oppsContext->state), 
        BT_STATUS_NO_CONNECTION, ("OPPS context is not connected"));

	BTL_VERIFY_ERR((OBJECT_REQUEST_STATE_IDLE != oppsContext->requestState), 
        BT_STATUS_FAILED, ("No pending request!"));

	switch (oppsContext->requestState)
	{
		case OBJECT_REQUEST_STATE_PUSH:
			if (accept == TRUE)
			{
				BTL_VERIFY_ERR((NULL != object), BT_STATUS_FAILED, ("object is NULL!"));
			}
			break;

		case OBJECT_REQUEST_STATE_PULL: 
			if (accept == TRUE)
			{
				BTL_VERIFY_ERR((NULL != object), BT_STATUS_FAILED, ("object is NULL!"));
			}
			break;
	}
    
	switch (oppsContext->requestState)
	{
		case OBJECT_REQUEST_STATE_PUSH:
		{
			if (accept == FALSE)
			{
				/* Push Request Rejected */
                
				obStatus = OPUSH_ServerAbort(&oppsContext->server, OBRC_FORBIDDEN);
				
				BTL_VERIFY_ERR((OB_STATUS_SUCCESS == obStatus), BT_STATUS_FAILED, 
					("The server is unable to abort!"));

				break;
			}
			else 
			{
				/* Push Request Accepted */
                
				if (object->objectLocation == BTL_OBJECT_LOCATION_MEM)
				{
					/* Push Object to Memory */
                    
					BTL_VERIFY_ERR(((NULL != object->location.memLocation.memAddr) && 
				                        (0 !=object->location.memLocation.size)),
								BT_STATUS_FAILED, ("Memory adress or/and size equal to '0'."));

					oppsContext->object->mem_ocx = (void*)object->location.memLocation.memAddr;
					oppsContext->object->objectSize = object->location.memLocation.size;

					/* Since we started with FS object and now user requests MEM object, 
						we need to move all received data from FS to Memory
		                     */
					if (oppsContext->object->object_location == BTL_OBJECT_LOCATION_FS)
					{
						obStatus = OBSTORE_CopyTempFsToMem(oppsContext->object);
						
						if (OBRC_SUCCESS != obStatus)
						{
							obStatus = OPUSH_ServerAbort(&oppsContext->server, OBRC_FORBIDDEN);
							BTL_VERIFY_ERR((OB_STATUS_SUCCESS == obStatus), BT_STATUS_FAILED, 
								("Server is unable to abort!"));
		                            	break;
						}
                  
						if(oppsContext->pushedObjectLen > oppsContext->object->objectSize)
						{
							obStatus = OPUSH_ServerAbort(&oppsContext->server, OBRC_FORBIDDEN);
							BTL_VERIFY_ERR((OB_STATUS_SUCCESS == obStatus), BT_STATUS_FAILED, 
			                                   ("The server is unable to abort!"));
							
							break;
						}
					
						/* Update to the new object location */
						oppsContext->object->object_location = object->objectLocation;
					 }
				}
				else
				{
					/* Push Object to FS */
                    
					/* Save the Push Object user's settings */

					BTL_VERIFY_ERR((0 != object->location.fsLocation.fsPath[0]), BT_STATUS_FAILED, 
						("fsPath must be set by the user."));
                    
					rcode = OBSTORE_PushFsSaveUserSettings(oppsContext->object, object);	
                        
					if (OBRC_SUCCESS != rcode)
					{  
						obStatus = OPUSH_ServerAbort(&oppsContext->server, rcode);
						BTL_VERIFY_ERR((OB_STATUS_SUCCESS == obStatus), BT_STATUS_FAILED, 
							("Server is unable to abort!"));
						
						break;
					}
				}
			}
			
	            break;
		}
		
		case OBJECT_REQUEST_STATE_PULL: 
		{
			if (accept == FALSE)
			{
				/* Pull Request Rejected */
                
				obStatus = OPUSH_ServerAbort(&oppsContext->server, OBRC_FORBIDDEN);
				BTL_VERIFY_ERR((OB_STATUS_SUCCESS == obStatus), BT_STATUS_FAILED, 
					("The server is unable to abort!"));

				break;
			}
			else 
			{
				/* Pull Request Accepted */

				/* Fill the Object location field */
				oppsContext->object->object_location = object->objectLocation;
                
				if (object->objectLocation == BTL_OBJECT_LOCATION_FS)
				{
					/* Pull Object from FS */

					/* Close previous opened default vcard, and reopen a new default vcard file */
					rcode = OBSTORE_FileCloseAndReopen(&oppsContext->object, object);
	                
					/* Update the new default vcard object size */
					obStatus = GOEP_SetObexObjLen(&oppsContext->server, OBSTORE_GetObjectLen(oppsContext->object));    
                    
					if ((OBRC_SUCCESS != rcode) || (OB_STATUS_SUCCESS != obStatus))
					{
						obStatus = OPUSH_ServerAbort(&oppsContext->server, OBRC_FORBIDDEN);

						BTL_LOG_ERROR(("Failed to open Default vcard file!"));

						BTL_VERIFY_ERR((OB_STATUS_SUCCESS == obStatus), BT_STATUS_FAILED, 
							("The server is unable to abort!"));						
						break;
					}
				}
				else 
				{
					/* Pull Object from Memory */

					oppsContext->object->objectSize = object->location.memLocation.size;
                    
					if ((NULL == object->location.memLocation.memAddr) || 
						(0 == object->location.memLocation.size))
					{
						obStatus = OPUSH_ServerAbort(&oppsContext->server, OBRC_FORBIDDEN);
						BTL_LOG_ERROR(("MemorySize or MemoryAddress is invalid.!"));

						BTL_VERIFY_ERR((OB_STATUS_SUCCESS == obStatus), BT_STATUS_FAILED, 
							 ("The server is unable to abort!"));
						break;
					}
				}

				obStatus = OPUSH_ServerAccept(&oppsContext->server, oppsContext->object);

				BTL_VERIFY_ERR_NORET((obStatus == OB_STATUS_SUCCESS), 
					("Failed accepting object request!"));
			}
			
			break;
		}
	}

	/* Now, OPPS_EVENT_OBJECT_PROGRESS events will be forwarded to Application */
	oppsContext->requestState = OBJECT_REQUEST_STATE_IDLE;

	obStatus = OPUSH_ServerContinue(&oppsContext->server);
	BTL_VERIFY_ERR((OB_STATUS_SUCCESS == obStatus), BT_STATUS_FAILED, 
            ("The server is not expecting a continue"));

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_OPPS_Abort()
 *
 *		Aborts the current server operation.
 */
BtStatus BTL_OPPS_Abort(BtlOppsContext *oppsContext, 
                        const ObexRespCode respCode)
{
	BtStatus status = BT_STATUS_SUCCESS;
	ObStatus obStatus;

	BTL_FUNC_START_AND_LOCK("BTL_OPPS_Abort");

	BTL_VERIFY_ERR((0 != oppsContext), BT_STATUS_INVALID_PARM, 
        ("Null oppsContext"));

	BTL_VERIFY_ERR((BTL_OPPS_STATE_CONNECTED == oppsContext->state), 
        BT_STATUS_NO_CONNECTION, ("OPPS context is not connected"));

	obStatus = OPUSH_ServerAbort(&oppsContext->server, respCode);
	BTL_VERIFY_ERR((OB_STATUS_SUCCESS == obStatus), BT_STATUS_FAILED, 
         ("Server abort falied"));

	oppsContext->requestState = OBJECT_REQUEST_STATE_IDLE;

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


#if BT_SECURITY == XA_ENABLED
/*-------------------------------------------------------------------------------
 * BTL_OPPS_SetSecurityLevel()
 *
 *		Sets security level for the given OPPS context.
 */
BtStatus BTL_OPPS_SetSecurityLevel(BtlOppsContext *oppsContext,
								  	const BtSecurityLevel *securityLevel)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_OPPS_SetSecurityLevel");

	BTL_VERIFY_ERR((0 != oppsContext), BT_STATUS_INVALID_PARM, 
        ("Null oppsContext"));

	if (0 != securityLevel)
	{
		status = BTL_VerifySecurityLevel(*securityLevel);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, 
            ("Invalid OPPS securityLevel"));
	}

	/* Apply the new security level */
	oppsContext->server.secRecord.level = (BtSecurityLevel)((securityLevel == 0) ? 
		(BSL_OPPS_DEFAULT) : (*securityLevel));

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_OPPS_GetSecurityLevel()
 *
 *		Gets security level for the given OPPS context.
 */
BtStatus BTL_OPPS_GetSecurityLevel(BtlOppsContext *oppsContext,
								  	BtSecurityLevel *securityLevel)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_OPPS_GetSecurityLevel");

	BTL_VERIFY_ERR((0 != oppsContext), BT_STATUS_INVALID_PARM, 
        ("Null oppsContext"));

	BTL_VERIFY_ERR((0 != securityLevel), BT_STATUS_INVALID_PARM, 
        ("Null securityLevel"));

	*securityLevel = oppsContext->server.secRecord.level;
	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}
#endif	/* BT_SECURITY == XA_ENABLED */


/*-------------------------------------------------------------------------------
 * BTL_OPPS_SetDefaultObject()
*
 *		Set default object absolute location (drive, path and file name)
*/
BtStatus BTL_OPPS_SetDefaultObject(const BtlObject* object)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_OPPS_SetDefaultObject");

	OBS_DefaultObject.objectLocation = object->objectLocation;

	if (object->objectLocation == BTL_OBJECT_LOCATION_FS)
	{
		/* A null-terminated string representing the full path of the object in the FS.*/
		BTL_VERIFY_ERR((NULL != object->location.fsLocation.fsPath), BT_STATUS_FAILED, ("Null fsPath"));
               
		/* Check fsPath length */
		BTL_VERIFY_ERR((TRUE == BTL_OPPS_CHECK_FS_PATH(object)), BT_STATUS_FAILED, ("fsPath too long."));
        
		/* Update default path and default file name (i.e. C:\QbInbox\default_object\mycard.vcf)*/
		OS_StrCpyUtf8(defaultFsPathBuf, object->location.fsLocation.fsPath);
		OBS_DefaultObject.location.fsLocation.fsPath = defaultFsPathBuf;

		/* Update OBS_DefaultDir path */
		if (TRUE != OBSTORE_SetInboxDir(OBS_DefaultObject.location.fsLocation.fsPath))
		{
			BTL_LOG_ERROR(("OBS_DefaultDir is too long.\n"));
			return FALSE;
		}
	}
	else 
	{
		OBS_DefaultObject.location.memLocation.memAddr = object->location.memLocation.memAddr;
		OBS_DefaultObject.location.memLocation.size = object->location.memLocation.size;

		BTL_VERIFY_ERR((NULL != OBS_DefaultObject.location.memLocation.memAddr), 
		BT_STATUS_INVALID_PARM, ("No DefaultObject"));

		BTL_VERIFY_ERR((0 != OBS_DefaultObject.location.memLocation.size), 
		BT_STATUS_INVALID_PARM, ("No DefaultObject"));
	}
	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BtlOppsDisable()
 *
 *		Function for deregistering security record and OPP server.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		oppsContext [in] - pointer to the OPPS context.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */
static BtStatus BtlOppsDisable(BtlOppsContext *oppsContext)
{
	BtStatus status = BT_STATUS_SUCCESS;
	ObStatus obStatus;

	BTL_FUNC_START("BtlOppsDisable");

	/* Clean up any active object */
	oppsContext->currBasicOper = GOEP_OPER_NONE;
	oppsContext->requestState = OBJECT_REQUEST_STATE_IDLE;
	
	if (oppsContext->object)
	{
		OBSTORE_Close(&oppsContext->object);
	}

#if BT_SECURITY == XA_ENABLED

	/* First, try to unregister security record */
	status = GOEP_UnregisterServerSecurityRecord(&oppsContext->server);
	BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == status), 
		("Failed unregistering OPPS security record"));
			
#endif	/* BT_SECURITY == XA_ENABLED */

	/* Second, try to deregister server */
	obStatus = OPUSH_DeregisterServer(&oppsContext->server);
	
	BTL_VERIFY_ERR((OB_STATUS_SUCCESS == obStatus), BT_STATUS_FAILED, 
        ("Failed deregistering OPP server"));

	/* Now deinitialize the Object Store Component */
	OBSTORE_Deinit();

	/* OPPS state is now idle */
	oppsContext->state = BTL_OPPS_STATE_DISABLED;

	BTL_LOG_INFO(("Server is disabled."));

	BTL_FUNC_END();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BtlOppsObjectRequest()
 *
 *		Function which handles object requests. (GOEP_EVENT_PROVIDE_OBJECT)
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		Event [in] - Internal OPPS event.
 *
 * Returns:
 *		void.
 */
static ObexRespCode BtlOppsObjectRequest(GoepServerEvent *Event)
{
	BtStatus status = BT_STATUS_SUCCESS;
	ObexRespCode rcode = OBRC_SUCCESS;
	ObStatus obStatus;
	BtlOppsContext *oppsContext;
	OppObStoreHandle   object;      
	
	BTL_FUNC_START("BtlOppsObjectRequest");

	/* Find context according to given OPP event */
	oppsContext = ContainingRecord(Event->handler, BtlOppsContext, server);

	if (Event->info.pushpull.nameLen)
	{
		BTL_LOG_INFO(("Object request name \"%s\".", Event->info.pushpull.name));
	}

	if (Event->info.pushpull.typeLen)
	{
		BTL_LOG_INFO(("Object request type \"%s\".", Event->info.pushpull.type));
	}

	/* Create a ObStore object (note: object->object_location is FS) */
	object = OBSTORE_New();


	BTL_VERIFY_FATAL((object), BT_STATUS_INTERNAL_ERROR, ("Failed allocating new object"));


	/* Saved new ObStore object */
	oppsContext->object = object;

	if (Event->oper == GOEP_OPER_PULL)
	{
		/* In pull operation, we shall take the entire Object from either
			Memory or FS location */
		object->object_location = OBS_DefaultObject.objectLocation;
	}

	if ((Event->oper == GOEP_OPER_PULL) || (Event->oper == GOEP_OPER_PUSH))
	{
		if (object->object_location == BTL_OBJECT_LOCATION_MEM)
		{
			/* Requested Object is located in Memory */
        
			if (Event->oper == GOEP_OPER_PULL)
			{
				/* Note: obs->mem_ocx & obs->objectSize are updated in OBSTORE_Open() */
			}
			else if (Event->oper == GOEP_OPER_PUSH)
			{
				/* Push operation saves initial data to FS file. 
				    Later, when BTL_OPPS_AcceptObjectRequest() is called, the user
				    can decide where the pushed object will be saved, in Memory or FS */
			}
		}
		else 
		{
			/* Requested Object is located in FS */
        
			/* Fill object->fsPath, object->type, and object->name */
			BtlOppsUpdateFileAndType(object, Event);
		}
	}
    
    
	switch (Event->oper)
	{
		case (GOEP_OPER_PULL):
			
			/* Open the default file for Pull operation */
			rcode = OBSTORE_Open(object, OPPS_PULL_OPERATION);
	        
			if (rcode == OBRC_SUCCESS) 
			{
				/* OPPS build a length header response */
				/* This is a MUST - do not remove */
				Event->info.pushpull.objectLen = OBSTORE_GetObjectLen(object);

				/* Need to ask user before performing pull operation */
				BTL_VERIFY_FATAL((oppsContext->requestState == OBJECT_REQUEST_STATE_IDLE), 
								BT_STATUS_INTERNAL_ERROR, ("Already pending object request!"));

				/* When GOEP_EVENT_CONTINUE event will be generated, 
				this flag will be checked and the user will be notified of the request */
				oppsContext->requestState = OBJECT_REQUEST_STATE_PULL;
			}

			break;

		case (GOEP_OPER_PUSH):
		
			/* Create a new file for Push operation */
			rcode = OBSTORE_Create(object, OPPS_PUSH_OPERATION);
			
			if (rcode == OBRC_SUCCESS)
			{
				OBSTORE_SetObjectLen(object, Event->info.pushpull.objectLen);

				/* Need to ask user before performing push operation */
				BTL_VERIFY_FATAL((oppsContext->requestState == OBJECT_REQUEST_STATE_IDLE), 
							BT_STATUS_INTERNAL_ERROR, 
                            ("Already pending object request!"));

				/* When GOEP_EVENT_CONTINUE event will be generated, 
				this flag will be checked and the user will be notified of the request */
				oppsContext->requestState = OBJECT_REQUEST_STATE_PUSH;
			}
			
			break;
	}

	if (rcode != OBRC_SUCCESS)
	{
		BTL_LOG_INFO(("Object request failed!"));

       	 if (oppsContext->object)
		{
			OBSTORE_Delete(&oppsContext->object);
		}
	}

	if (rcode == OBRC_SUCCESS)
	{
		OBSTORE_SetObstoreCallback(object, BtlOppsObstoreCallback);
		
		if (Event->oper == GOEP_OPER_PULL)
		{
			/* Do-nothing */
			/* OPUSH_ServerAccept() will be called in BTL_OPPS_AcceptObjectRequest(). */
		}
		else if (Event->oper == GOEP_OPER_PUSH)
		{
			obStatus = OPUSH_ServerAccept(Event->handler, oppsContext->object);

			BTL_VERIFY_ERR_NORET((obStatus == OB_STATUS_SUCCESS), 
				 ("Failed accepting object request!"));
		}
	}

	BTL_FUNC_END();

	return rcode;
}


/*-------------------------------------------------------------------------------
 * BtlOppsCallback()
 *
 *		Internal callback for handling OPP events in BTL layer.
 *		This function forwards events to registered application callback.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		Event [in] - Internal OPPS event.
 *
 * Returns:
 *		void.
 */
static void BtlOppsCallback(GoepServerEvent *Event)
{
	BtStatus status = BT_STATUS_SUCCESS;
	ObStatus obStatus;
	ObexRespCode rcode;
	BtlOppsContext *oppsContext;
	OppObStoreHandle object;  
	BOOL passEventToApp = TRUE;
	BOOL sendDisableEvent = FALSE;
	BOOL notifyRadioOffComplete = FALSE;

	BTL_FUNC_START("BtlOppsCallback");
	
	/* Find context according to given OPP event */
	oppsContext = ContainingRecord(Event->handler, BtlOppsContext, server);

	/* object is valid only after OBSTORE_Create() is called */
	object = oppsContext->object;

	/* Setup the internal event */
	btlOppsData.oppsEvent.event = Event->event;
	btlOppsData.oppsEvent.oper = Event->oper;

	/* First handle special case of disabling */
	if (oppsContext->state == BTL_OPPS_STATE_DISABLING)
	{
		switch (Event->event)
		{
			case (GOEP_EVENT_TP_DISCONNECTED):
			{
				/* The transport layer connection has been disconnected. */
				BTL_LOG_INFO(("Transport layer connection has been disconnected."));

				/* Clean up any active object */
				oppsContext->currBasicOper = GOEP_OPER_NONE;
				oppsContext->requestState = OBJECT_REQUEST_STATE_IDLE;
				
				if (oppsContext->object)
				{
					OBSTORE_Close(&oppsContext->object);
				}

				oppsContext->state = BTL_OPPS_STATE_DISCONNECTED;

				if (oppsContext->disableState & BTL_OPPS_DISABLE_STATE_MASK_DISABLE_ACTIVE)
				{				
					BtlOppsDisable(oppsContext);
					sendDisableEvent = TRUE;
				}

				if (oppsContext->disableState & BTL_OPPS_DISABLE_STATE_MASK_RADIO_OFF_ACTIVE)
				{
					BtlContext *base;
					BtlOppsContext *context;

					notifyRadioOffComplete = TRUE;

					IterateList(btlOppsData.contextsList, base, BtlContext *)
					{
						context = (BtlOppsContext *)base;
						if ((context->state != BTL_OPPS_STATE_DISCONNECTED) && 
							(context->state != BTL_OPPS_STATE_DISABLED))
						{
							notifyRadioOffComplete = FALSE;			
						}                             
					}
				}

				oppsContext->disableState = BTL_OPPS_DISABLE_STATE_MASK_NONE;

				/* Pass event to app */
				break;
			}
			
			case (GOEP_EVENT_ABORTED):
			{
				/* Indicates that the current operation failed or aborted. */
				BTL_LOG_INFO(("BTL_OPPS: GOEP %s operation failed or aborted.", 
				BtlOppsGoepOpName(Event->oper)));

				switch (Event->oper) 
				{
					case GOEP_OPER_PUSH:

						/* Delete the file we created */
						if (oppsContext->object)
						{
							OBSTORE_Delete(&oppsContext->object);
						}
						break;

					case GOEP_OPER_PULL:

						if (oppsContext->object)
						{
							OBSTORE_Close(&oppsContext->object);
						}
						break;
				}

				/* Pass event to app */
				break;
			}

			default:
			{
				BTL_LOG_ERROR(("Received unexpected event %d while disabling!",
		                    Event->event));	
				
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
				BTL_LOG_INFO(("Transport layer connection has come up."));
				
				oppsContext->state = BTL_OPPS_STATE_CONNECTED;

				/* Pass event to app */
				break;
			}

			case (GOEP_EVENT_TP_DISCONNECTED):
			{
				/* The transport layer connection has been disconnected. */
				BTL_LOG_INFO(("Transport layer connection has been disconnected."));

				/* Clean up any active object */
				oppsContext->currBasicOper = GOEP_OPER_NONE;
				oppsContext->requestState = OBJECT_REQUEST_STATE_IDLE;
				
				if (oppsContext->object)
				{
					OBSTORE_Close(&oppsContext->object);
				}

				oppsContext->state = BTL_OPPS_STATE_DISCONNECTED;

				/* Pass event to app */
				break;
			}
			
			case (GOEP_EVENT_CONTINUE):
			{
				/* Delivered to the application when it is time to issue another response. */
				if (oppsContext->requestState != OBJECT_REQUEST_STATE_IDLE)
				{
					/* Setup the internal event */
					btlOppsData.oppsEvent.event = OPPS_EVENT_OBJECT_REQUEST;
					btlOppsData.oppsEvent.info.request.name = object->objectName;
					btlOppsData.oppsEvent.info.request.nameLen = object->nameLen;
					btlOppsData.oppsEvent.info.request.type = object->type;
					btlOppsData.oppsEvent.info.request.typeLen = object->typeLen;
					btlOppsData.oppsEvent.info.request.objectLen = OBSTORE_GetObjectLen(object);
					
					/* Pass event to app */
				}
				else
				{
					/* Always call continue to keep the requests flowing. */
					obStatus = OPUSH_ServerContinue(Event->handler);
					
					BTL_VERIFY_ERR((OB_STATUS_SUCCESS == obStatus), BT_STATUS_INTERNAL_ERROR, 
						("The server is not expecting a continue"));

					/* Do not pass event to app. */
					passEventToApp = FALSE;
				}
				break;
			}
			
			case (GOEP_EVENT_START):
			{
				/* Indicates the start of a new operation */
				BTL_LOG_INFO(("Starting GOEP %s operation.", BtlOppsGoepOpName(Event->oper)));

				if ((Event->oper == GOEP_OPER_ABORT) && 
					(oppsContext->requestState != OBJECT_REQUEST_STATE_IDLE))
				{
					/* Clean up any active object */
					if (oppsContext->object)
					{
						if (oppsContext->requestState == OBJECT_REQUEST_STATE_PUSH)
						{
							OBSTORE_Delete(&oppsContext->object);
						}
						else
						{
							OBSTORE_Close(&oppsContext->object);
						}
					}
					
					oppsContext->requestState = OBJECT_REQUEST_STATE_IDLE;
					
					obStatus = OPUSH_ServerContinue(&oppsContext->server);
					BTL_VERIFY_ERR((OB_STATUS_SUCCESS == obStatus), BT_STATUS_FAILED, 
						("The server is not expecting a continue"));
				}

				/* Save current operation for this context */
				oppsContext->currBasicOper = Event->oper;

				/* Pass event to app */
				break;
			}

			case GOEP_EVENT_HEADER_RX:
			{
				/* OPPS received a push request, header file received first */

				if (Event->oper == GOEP_OPER_PUSH) 
		              {
					if (Event->header.remainLen == 0) 
					{
					    /* Only indicate the last header indication.  
		                           This prevents  reporting the same header multiple times 
		                         */
						BTL_LOG_INFO(("Server %s Receiving an OBEX header: %s", 
						BtlOppsGoepOpName(Event->oper), 
						BtlpObexHeaderType(Event->header.type)));
					}

					if (Event->header.type == OBEXH_NAME)	
					{
						OBSTORE_SetFileName(object , (const BtlUtf8 *)(Event->header.buffer));
					}
					else if (Event->header.type == OBEXH_LENGTH)
					{
						/* We have not created OBSTORE_Create() yet, therefore we shall 
							postpone object length checking for later (save length for future use) */
						oppsContext->pushedObjectLen = Event->info.pushpull.objectLen;
					}
				}
				else
				{
					/* should not reach here */
					BTL_FATAL_NO_RETVAR(("Invalid HEADER_RX Event received during PULL operation"));
				}
					
				/* Do not pass event to app. */
				passEventToApp = FALSE;
				break;
			}

			case (GOEP_EVENT_PROVIDE_OBJECT):
			{
				/* Instructs the server to provide or to delete an object. */
				BTL_LOG_INFO(("Ongoing GOEP %s operation.", BtlOppsGoepOpName(Event->oper)));
				
		       	if ((Event->oper == GOEP_OPER_PUSH) || (Event->oper == GOEP_OPER_PULL)) 
				{
		       		/* Need Object to complete this request. */
					rcode = BtlOppsObjectRequest(Event);

					if (rcode != OBRC_SUCCESS)
					{
						OPUSH_ServerAbort(Event->handler, rcode);
                        
						if (Event->oper == GOEP_OPER_PUSH)
						{
							BTL_LOG_ERROR(("Unable to create Push object!"));	
						}
						else if (Event->oper == GOEP_OPER_PULL) 
						{
			                            BTL_LOG_ERROR(("Pull object not found!"));	
			                     }
					}
				}
				
				/* Do not pass event to app. */
				passEventToApp = FALSE;
				break;
			}

	            case (GOEP_EVENT_ABORTED):
				{
					/* Indicates that the current operation failed or aborted. */
					BTL_LOG_INFO(("BTL_OPPS: GOEP %s operation failed or aborted.", 
	                        BtlOppsGoepOpName(Event->oper)));
			        
			        switch (Event->oper) 
					{
						case GOEP_OPER_PUSH:
		                	/* Delete the file we created */
		                	if (oppsContext->object)
							{
		                    	OBSTORE_Delete(&oppsContext->object);
							}
	                		break;

	            		case GOEP_OPER_PULL:
	                		if (oppsContext->object)
							{
	                    		OBSTORE_Close(&oppsContext->object);
							}
			            	break;
			        }

					/* Pass event to app */
			        break;
				}
			
	            case (GOEP_EVENT_PRECOMPLETE):
				{
					/* Delivered to server applications before the last response is 
	                  		 going to be sent. We want to process the Push commands now. 
					This way we can report the status of the request back to the client. */
					if ((Event->oper == GOEP_OPER_PUSH) && object) 
					{
		              		/* See if we can successfully close the file we've received */
		              		rcode = OBSTORE_Close(&oppsContext->object);
		              		if (rcode != OBRC_SUCCESS)
						{
		              			OPUSH_ServerAbort(Event->handler, rcode);
						}
	                		}
					
					/* Do not pass event to app. */
					passEventToApp = FALSE;
					break;
				}
			
	            case (GOEP_EVENT_COMPLETE):
				{
					/* Indicates that the operation has completed successfully. */
					BTL_LOG_INFO(("GOEP %s operation complete.", 
		                    BtlOppsGoepOpName(Event->oper)));

					if (oppsContext->object) 
					{  
						/* only GET still needs to return the object */
				            rcode = OBSTORE_Close(&oppsContext->object);
						BTL_VERIFY_ERR((OBRC_SUCCESS == rcode), BT_STATUS_INTERNAL_ERROR, 
		                        ("Failed closing object"));
				        }

					/* Pass event to app */
					break;
				}
				
			default:
			{
				/* Do not pass event to app. */
				passEventToApp = FALSE;
				break;
			}
		
		} /* switch end */		
	}

	if (TRUE == passEventToApp)
	{
		/* Set the context in the event passed to app */
		btlOppsData.event.oppsContext = oppsContext;

		/* Set the internal event in the event passed to app */
		btlOppsData.event.oppsEvent = &btlOppsData.oppsEvent;

		/* Pass the event to app */
		oppsContext->callback(&btlOppsData.event);
	}

	if (TRUE == sendDisableEvent)
	{
		/* Change the event to indicate context was disabled */
		btlOppsData.event.oppsEvent->event = OPPS_EVENT_DISABLED;
		
		/* Pass the event to app */
		oppsContext->callback(&btlOppsData.event);
	}

	if (TRUE == notifyRadioOffComplete)
	{
		BtlModuleNotificationCompletionEvent moduleCompletionEvent;
		moduleCompletionEvent.type = BTL_MODULE_NOTIFICATION_COMPLETION_EVENT_TYPE_RADIO_OFF;
		moduleCompletionEvent.status = BT_STATUS_SUCCESS;
		
		BTL_ModuleCompleted(BTL_MODULE_TYPE_OPPS, &moduleCompletionEvent);
	}

	BTL_FUNC_END();
}


/*-------------------------------------------------------------------------------
 * BtlOppsObstoreCallback()
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
static void BtlOppsObstoreCallback(OppObStoreEntry *obs)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtlContext *context;
	BtlOppsContext *oppsContext = NULL;

	BTL_FUNC_START("BtlOppsObstoreCallback");

	/* Find OPPS context according to given OBSTORE object */
	IterateList(btlOppsData.contextsList, context, BtlContext *)
	{
		oppsContext = (BtlOppsContext *)context;
       	if (oppsContext->object == obs)
	    	{
			break;
       	}
	}

	BTL_VERIFY_ERR((NULL != oppsContext), BT_STATUS_FAILED, 
        ("Failed to find OPPS context in progress callback"));

	BTL_VERIFY_ERR((oppsContext->object == obs), BT_STATUS_FAILED, 
        ("Failed to find OPPS context in progress callback"));

	/* Forward OPPS_EVENT_OBJECT_PROGRESS events only after user accept the request */
	if (oppsContext->requestState == OBJECT_REQUEST_STATE_IDLE)
	{
		/* Setup the internal event */
		btlOppsData.oppsEvent.event = OPPS_EVENT_OBJECT_PROGRESS;
		btlOppsData.oppsEvent.oper = oppsContext->currBasicOper;
		btlOppsData.oppsEvent.info.progress.name = obs->objectName;
		btlOppsData.oppsEvent.info.progress.nameLen = obs->nameLen;
		btlOppsData.oppsEvent.info.progress.currPos = obs->amount;
		btlOppsData.oppsEvent.info.progress.maxPos = obs->objectSize;

		/* Set the context in the event passed to app */
		btlOppsData.event.oppsContext = oppsContext;

		/* Set the internal event in the event passed to app */
		btlOppsData.event.oppsEvent = &btlOppsData.oppsEvent;

		/* Pass the event to app */
		oppsContext->callback(&btlOppsData.event);
	}

	BTL_FUNC_END();
}


/*-------------------------------------------------------------------------------
 * BtlOppsObjectUpdateFileAndType()
 *
 *		Update Full file name and type fields
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		obs [in] - handle of object store entry.
 *      Event [in] - GOEP Event
 *
 * Returns:
 *		void.
 */
static void BtlOppsUpdateFileAndType(OppObStoreHandle obs, GoepServerEvent *Event)
{
	static BtlUtf8 fsPath[BTHAL_FS_MAX_PATH_LENGTH + BTHAL_FS_MAX_FILE_NAME_LENGTH];
	U16 len;
    
	if (0 != Event->info.pushpull.name[0])
	{
		/* Build full path with file name */
		OS_StrCpyUtf8(fsPath, OBS_DefaultDir);
		len = OS_StrLenUtf8(fsPath);		
		fsPath[len] = BTHAL_FS_PATH_DELIMITER;
		fsPath[++len] = '\0';
		OS_StrCatUtf8(fsPath, (const char *)Event->info.pushpull.name);

		/* Fill obs->fsPath and obs->nameLen fields */
		OBSTORE_AppendNameAscii(obs, fsPath);

		/* Fill obs->name field */
		OBSTORE_ExtractObjName(fsPath, obs->objectName);

		/* Fill the nameLen field */
		obs->nameLen = OS_StrLenUtf8(obs->objectName);
	}
    	    
	OBSTORE_AppendType(obs, Event->info.pushpull.type, Event->info.pushpull.typeLen);
}


/*-------------------------------------------------------------------------------
 * BtlOppsGoepOpName()
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
static const char *BtlOppsGoepOpName(GoepOperation Op)
{
	switch (Op) 
	{
        case GOEP_OPER_PUSH:
            return "Push";
            
        case GOEP_OPER_PULL:
            return "Pull";

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


BtStatus BtlOppsBtlNotificationsCb(BtlModuleNotificationType notificationType)
{
	BtStatus		status = BT_STATUS_SUCCESS;
	ObStatus    	obStatus = OB_STATUS_SUCCESS;
	BtlOppsContext	*oppsContext;
	BtlContext		*base;

	BTL_FUNC_START("BtlOppsBtlNotificationsCb");
	
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
			
			IterateList(btlOppsData.contextsList, base, BtlContext *)
			{
				oppsContext = (BtlOppsContext *)base;

				switch (oppsContext->state)
				{
					case (BTL_OPPS_STATE_DISCONNECTING):
					{
						/* OPPS state is now in the process of disabling */
						oppsContext->state = BTL_OPPS_STATE_DISABLING;
						break;
					}
					
					case (BTL_OPPS_STATE_CONNECTED):
					{							
						obStatus = GOEP_ServerTpDisconnect(&oppsContext->server);
						BTL_VERIFY_ERR((OB_STATUS_BUSY != obStatus), BT_STATUS_BUSY, 
							("OPPS is currently executing an operation"));
						
						if (OB_STATUS_PENDING == obStatus)
						{
							/* OPPS state is now in the process of disabling */
							oppsContext->state = BTL_OPPS_STATE_DISABLING;
							break;
						}
						
						break;
					}
				}

				if (oppsContext->state == BTL_OPPS_STATE_DISABLING)
				{
					oppsContext->disableState |= BTL_OPPS_DISABLE_STATE_MASK_RADIO_OFF_ACTIVE;
					status = BT_STATUS_PENDING;
				}
				
			}

			break;

		default:

			BTL_ERR(BT_STATUS_INTERNAL_ERROR, ("Invalid notification (%d)", notificationType));
			


	};

	BTL_FUNC_END();
	
	return status;
}




#else /* BTL_CONFIG_OPP ==   BTL_CONFIG_ENABLED*/

/*-------------------------------------------------------------------------------
 * BTL_OPPS_Init()
 */
BtStatus BTL_OPPS_Init(void)
{
    
   BTL_LOG_INFO(("BTL_OPPS_Init() -  BTL_CONFIG_OPP Disabled"));
  

    return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 * BTL_OPPS_Deinit()
 */
BtStatus BTL_OPPS_Deinit(void)
{
    BTL_LOG_INFO(("BTL_OPPS_Deinit() -  BTL_CONFIG_OPP Disabled"));

    return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------*/



#endif /* BTL_CONFIG_BIP==   BTL_CONFIG_ENABLED*/


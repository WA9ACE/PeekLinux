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
*   FILE NAME:      btl_pbaps.c
*
*   DESCRIPTION:    This file contains the BTL PBAP target implementation.
*
*   AUTHOR:         Yoni Shavit
*
\*******************************************************************************/




#include "btl_defs.h"
#include "btl_config.h"

BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_PBAPS);

#if BTL_CONFIG_PBAPS == BTL_CONFIG_ENABLED

/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
 
#include <btl_pbaps.h>
#include <btl_pool.h>
#include <btl_pbaps_pbi.h>
/********************************************************************************
 *
 * Constants 
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BSL_PBAPS_DEFAULT constant
 *
 *     Represents default security level for PBAPS.
 */
#define BSL_PBAPS_DEFAULT  (BSL_NO_SECURITY)

/*-------------------------------------------------------------------------------
 * PBAPS_SERVICE_NAME_MAX_LEN constant
 *
 *     Represents max length of service name for PBAPS.
 */
#define PBAPS_SERVICE_NAME_MAX_LEN  (32)

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/
 
typedef struct _BtlPbapsData    BtlPbapsData;

/*-------------------------------------------------------------------------------
 * BtlPbapsInitState type
 *
 *     Defines the PBAPS init state.
 */
typedef U8 BtlPbapsInitState;

#define BTL_PBAPS_INIT_STATE_NOT_INTIALIZED					(0x00)
#define BTL_PBAPS_INIT_STATE_INITIALIZED					(0x01)
#define BTL_PBAPS_INIT_STATE_INITIALIZATION_FAILED			(0x02)
#define BTL_PBAPS_INIT_STATE_DEINITIALIZATION_FAILED		(0x03)

/*-------------------------------------------------------------------------------
 * BtlPbapsState type
 *
 *     Defines the PBAPS state of a specific context.
 */
typedef U8 BtlPbapsState;

#define BTL_PBAPS_STATE_IDLE									(0x00)
#define BTL_PBAPS_STATE_DISCONNECTED							(0x01)
#define BTL_PBAPS_STATE_CONNECTED 								(0x02)
#define BTL_PBAPS_STATE_OPER_ON_GOING							(0x03)
#define BTL_PBAPS_STATE_DISCONNECTING							(0x04)
#define BTL_PBAPS_STATE_DISABLING								(0x05)

/*-------------------------------------------------------------------------------
 * BtlPbapsDisableStateMask type
 *
 *     Defines the PBAPS disable state mask.
 */
typedef U8 BtlPbapsDisableStateMask;

#define BTL_PBAPS_DISABLE_STATE_MASK_NONE				(0x00)
#define BTL_PBAPS_DISABLE_STATE_MASK_DISABLE_ACTIVE		(0x01)
#define BTL_PBAPS_DISABLE_STATE_MASK_RADIO_OFF_ACTIVE	(0x02)

/*-------------------------------------------------------------------------------
 * ObexAuthFlags type
 *
 *     Defines the OBEX authentication flags of a specific context.
 */
typedef U8 ObexAuthFlags;

#define OBEX_AUTH_RECEIVED_CHAL  							(0x02)
#define OBEX_AUTH_RECEIVED_CHAL_CONT						(0x04)
#define OBEX_AUTH_AUTHENTICATED  							(0x08)

 /********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BtlPbapsContext structure
 *
 *     Represents BTL PBAPS context.
 */
struct _BtlPbapsContext 
{
	/* Must be first field */
	BtlContext base;

	/* Internal PBAPS server */
	PbapServerSession server;

	/* Associated callback with this context */
	BtlPbapsCallBack callback;

	/* Current PBAPS state of the context */
	BtlPbapsState state;

	/* This flag indicates the disable state */
	BtlPbapsDisableStateMask disableState;
	
	/* Service name plus 1 null-terminated char, plus 2 bytes for SDP_TEXT_8BIT */
	BtlUtf8 serviceName[PBAPS_SERVICE_NAME_MAX_LEN + 3];

	/* 
 	* Supported Repositories - cell 0 contains constant SDP Data Element Types.
 	*                          cell 1 contains the supported repositories as defined in 
 	*						   BtlPbapsRepositoriesMask
 	*/
    U8 supportedRepositories[2];
	
#if OBEX_AUTHENTICATION == XA_ENABLED

	/* Defines if server is initiating OBEX authentication */
	BOOL initiateObexAuth;

	/* OBEX Authentication flags */
	ObexAuthFlags obexAuthFlags;

#endif	/* OBEX_AUTHENTICATION == XA_ENABLED */

};

/*-------------------------------------------------------------------------------
 * BtlPbapsData structure
 *
 *     Represents the data of th BTL PBAPS module.
 */
struct _BtlPbapsData
{
	/* Pool of PBAPS contexts */
	BTL_POOL_DECLARE_POOL(contextsPool, contextsMemory, BTL_CONFIG_PBAPS_MAX_NUM_OF_CONTEXTS, sizeof(BtlPbapsContext));

	/* List of active PBAPS contexts */
	ListEntry contextsList;

	/* Event passed to the application */
	BtlPbapsEvent event;
		
};


/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/
 
/*-------------------------------------------------------------------------------
 * btlPbapsInitState
 *
 *     Represents the current init state of PBAPS module.
 */
static BtlPbapsInitState btlPbapsInitState = BTL_PBAPS_INIT_STATE_NOT_INTIALIZED;

/*-------------------------------------------------------------------------------
 * btlPbapsData
 *
 *     Represents the data of PBAPS module.
 */
static BtlPbapsData btlPbapsData;

/*-------------------------------------------------------------------------------
 * btlPbapsContextsPoolName
 *
 *     Represents the name of the PBAPS contexts pool.
 */
static const char btlPbapsContextsPoolName[] = "PbapsContexts";

/********************************************************************************
 *
 * Internal function prototypes
 *
 *******************************************************************************/
 
static BtStatus BtlPbapsDisable(BtlPbapsContext *pbapsContext);
static void BtlPbapsCallback(PbapServerCallbackParms *Parms);
static const char *BtlPbapsOpName(PbapOp Op);
static BtStatus BtlPbapsBtlNotificationsCb(BtlModuleNotificationType notificationType);

/********************************************************************************
 *
 * Extrnal  function 
 *
 *******************************************************************************/
extern void	BtlPbapResetEntryName(void);

/********************************************************************************
 *
 * Function definitions
 *
 *******************************************************************************/
 
/*-------------------------------------------------------------------------------
 * BTL_PBAPS_Init()
 */
BtStatus BTL_PBAPS_Init()
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_COMMON("BTL_PBAPS_Init");
	
	BTL_VERIFY_ERR((BTL_PBAPS_INIT_STATE_NOT_INTIALIZED == btlPbapsInitState), 
		BT_STATUS_FAILED, ("PBAP module is already initialized"));
	
	btlPbapsInitState = BTL_PBAPS_INIT_STATE_INITIALIZATION_FAILED;

	status = BTL_POOL_Create(&btlPbapsData.contextsPool,
							btlPbapsContextsPoolName,
							btlPbapsData.contextsMemory, 
							BTL_CONFIG_PBAPS_MAX_NUM_OF_CONTEXTS,
							sizeof(BtlPbapsContext));
	
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, 
		("PBAP contexts pool creation failed"));
	
	InitializeListHead(&btlPbapsData.contextsList);

	BTL_RegisterForCommonNotifications(BTL_MODULE_TYPE_PBAPS, BtlPbapsBtlNotificationsCb);
		
	btlPbapsInitState = BTL_PBAPS_INIT_STATE_INITIALIZED;

	/* Init the PBHAL_PB module */
	status = BTHAL_PB_Init();
	BTL_VERIFY_ERR(BT_STATUS_SUCCESS == status, BT_STATUS_FAILED, ("BTHAL_PB_Init failed"));

	BTL_FUNC_END_AND_UNLOCK();
	
	return (status);
}

/*-------------------------------------------------------------------------------
 * BTL_PBAPS_Deinit()
 */
BtStatus BTL_PBAPS_Deinit(void)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_COMMON("BTL_PBAPS_Deinit");
	
	BTL_VERIFY_ERR((BTL_PBAPS_INIT_STATE_INITIALIZED == btlPbapsInitState), 
		BT_STATUS_FAILED, ("PBAPS module is not initialized"));

	btlPbapsInitState = BTL_PBAPS_INIT_STATE_INITIALIZATION_FAILED;

	BTL_DeRegisterFromCommonNotifications(BTL_MODULE_TYPE_PBAPS);

	BTL_VERIFY_ERR((IsListEmpty(&btlPbapsData.contextsList)), 
		BT_STATUS_FAILED, ("PBAPS contexts are still active"));

	status = BTL_POOL_Destroy(&btlPbapsData.contextsPool);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, 
		("PBAPS contexts pool destruction failed"));
		
	btlPbapsInitState = BTL_PBAPS_INIT_STATE_NOT_INTIALIZED;

	BTL_FUNC_END_AND_UNLOCK();
	
	return (status);
}

/*-------------------------------------------------------------------------------
 * BTL_PBAPS_Create()
 */
BtStatus BTL_PBAPS_Create(BtlAppHandle *appHandle,
                          BtlPbapsCallBack pbapsCallback,
                          BtSecurityLevel *secLevel,
                          BtlPbapsContext **pbapsContext)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_PBAPS_Create");

	BTL_VERIFY_ERR((0 != pbapsCallback), BT_STATUS_INVALID_PARM, ("Null pbapsCallback"));
	BTL_VERIFY_ERR((0 != pbapsContext), BT_STATUS_INVALID_PARM, ("Null pbapsContext"));

#if BT_SECURITY == XA_ENABLED

	if (0 != secLevel)
	{
		status = BTL_VerifySecurityLevel(*secLevel);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Invalid PBAPS securityLevel"));
	}

#endif	/* BT_SECURITY == XA_ENABLED */

	/* Allocate memory for a unique new PBAPS context */
	status = BTL_POOL_Allocate(&btlPbapsData.contextsPool, (void**)pbapsContext);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed allocating PBAPS context"));
	
	status = BTL_HandleModuleInstanceCreation(appHandle, BTL_MODULE_TYPE_PBAPS, &(*pbapsContext)->base);
	
	if (BT_STATUS_SUCCESS != status)
	{
		/* pbapsContext must be freed before we exit */
		BtStatus retVal = BTL_POOL_Free(&btlPbapsData.contextsPool, (void**)pbapsContext);
		BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == retVal), ("Failed freeing PBAPS context"));

		/* This verify will fail and will handle the exception gracefully */
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed handling PBAPS instance creation"));
	}

	/* Save the given callback */
	(*pbapsContext)->callback = pbapsCallback;

	/* Add the new PBAPS context to the active contexts list */
	InsertTailList(&btlPbapsData.contextsList, &((*pbapsContext)->base.node));

#if BT_SECURITY == XA_ENABLED

	/* Set the security record & save the given security level, or use default */
	(*pbapsContext)->server.secLevel = (U8)((secLevel == 0) ? (BSL_PBAPS_DEFAULT) : (*secLevel));

#endif	/* BT_SECURITY == XA_ENABLED */

	/* Init PBAPS context state */
	(*pbapsContext)->state = BTL_PBAPS_STATE_IDLE;

	(*pbapsContext)->disableState = BTL_PBAPS_DISABLE_STATE_MASK_NONE;
#if OBEX_AUTHENTICATION == XA_ENABLED

	/* Server does not initiate OBEX authentication by default */
	(*pbapsContext)->initiateObexAuth = FALSE;

	(*pbapsContext)->obexAuthFlags = 0;
	
#endif	/* OBEX_AUTHENTICATION == XA_ENABLED */


	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

/*-------------------------------------------------------------------------------
 * BTL_PBAPS_Destroy()
 */	
BtStatus BTL_PBAPS_Destroy(BtlPbapsContext **pbapsContext)
{

	BtStatus status = BT_STATUS_SUCCESS;
	BOOL isAllocated = FALSE;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_PBAPS_Destroy");

	BTL_VERIFY_ERR((0 != pbapsContext), BT_STATUS_INVALID_PARM, ("Null pbapsContext"));
	BTL_VERIFY_ERR((0 != *pbapsContext), BT_STATUS_INVALID_PARM, ("Null *pbapsContext"));

	BTL_VERIFY_ERR((BTL_PBAPS_STATE_IDLE == (*pbapsContext)->state), BT_STATUS_IN_USE, ("PBAPS context is in use"));

	status = BTL_POOL_IsElelementAllocated(&btlPbapsData.contextsPool, *pbapsContext, &isAllocated);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed locating given PBAPS context"));
	BTL_VERIFY_ERR((TRUE == isAllocated), BT_STATUS_INVALID_PARM, ("Invalid PBAPS context"));

	/* Remove the context from the list of all PBAPS contexts */
	RemoveEntryList(&((*pbapsContext)->base.node));

	status = BTL_HandleModuleInstanceDestruction(&(*pbapsContext)->base);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed handling PBAPS instance destruction"));

	status = BTL_POOL_Free(&btlPbapsData.contextsPool, (void**)pbapsContext);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("Failed freeing PBAPS context"));

	/* Set the PBAPS context to NULL */
	*pbapsContext = 0;

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

/*-------------------------------------------------------------------------------
 *  BTL_PBAPS_Enable()
 */
BtStatus BTL_PBAPS_Enable(BtlPbapsContext *pbapsContext,
                          BtlUtf8 *serviceName,
                          BtlPbapsRepositoriesMask *reposMask)
{
	BtStatus status = BT_STATUS_SUCCESS;
	ObStatus obStatus;
	U16 len = 0;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_PBAPS_Enable");

	BTL_VERIFY_ERR((0 != pbapsContext), BT_STATUS_INVALID_PARM, ("Null pbapsContext"));
	BTL_VERIFY_ERR((BTL_PBAPS_STATE_IDLE == pbapsContext->state), BT_STATUS_FAILED, ("PBAPS context is already enabled"));

	if ((serviceName != 0) && ((len = OS_StrLenUtf8(serviceName)) != 0))
	{
		BTL_VERIFY_ERR((len <= PBAPS_SERVICE_NAME_MAX_LEN), BT_STATUS_INVALID_PARM, ("PBAPS service name is too long!"));
	}
	BTL_VERIFY_ERR((0 != reposMask), BT_STATUS_INVALID_PARM, ("PBAPS null reposMask"));

	obStatus = PBAP_RegisterServer(&pbapsContext->server, BtlPbapsCallback);
	BTL_VERIFY_ERR((OB_STATUS_SUCCESS == obStatus), BT_STATUS_FAILED, ("Failed to register PBAPS channel"));

	/* Set the PBAP server session in the BTL_PBAPS_PB module */
	status = BtlPbapsPbInit(&pbapsContext->server);
	BTL_VERIFY_ERR(BT_STATUS_SUCCESS == status, BT_STATUS_FAILED, ("BtlPbapsPbInit failed"));
	
	/* Update serviceName */
	if (len > 0)
	{
		/* The first 2 bytes are for SDP_TEXT_8BIT */
		pbapsContext->serviceName[0] = DETD_TEXT + DESD_ADD_8BITS;
		pbapsContext->serviceName[1] = (U8)(len + 1);	/* Includes '\0' */
		
		OS_MemCopy((pbapsContext->serviceName + 2), serviceName, len);
		pbapsContext->serviceName[(len + 2)] = '\0';

		pbapsContext->server.attributes[5].len = (U16)(len + 3);
		pbapsContext->server.attributes[5].value = pbapsContext->serviceName;
	}

	/* Update Phonebook Repositories */
	pbapsContext->supportedRepositories[0] = DETD_UINT + DESD_1BYTE;
	pbapsContext->supportedRepositories[1] = *reposMask;
	pbapsContext->server.attributes[6].value = pbapsContext->supportedRepositories;


#if BT_SECURITY == XA_ENABLED

	/* In case the security level is BSL_NO_SECURITY, ESI register fucntion doesn't register security 
	 * record. Therefore,in that case, security registration is done below 
	 */
	if (pbapsContext->server.secLevel == BSL_NO_SECURITY) 
	{
        
        obStatus = GOEP_RegisterServerSecurityRecord(&pbapsContext->server.sApp, pbapsContext->server.secLevel);
		BTL_VERIFY_ERR((OB_STATUS_SUCCESS == obStatus), BT_STATUS_FAILED, ("Failed to register PBAPS channel"));
	}

#endif	/* BT_SECURITY == XA_ENABLED */
	
	/* PBAPS state is now enabled */
	pbapsContext->state = BTL_PBAPS_STATE_DISCONNECTED;

	BTL_LOG_INFO(("BTL_PBAPS: Target is enabled."));

	BTL_FUNC_END_AND_UNLOCK();
	
	return (status); 
}


/*-------------------------------------------------------------------------------
 * BTL_PBAPS_Disable()
 */
BtStatus BTL_PBAPS_Disable(BtlPbapsContext *pbapsContext)
{
	BtStatus status = BT_STATUS_PENDING;
	ObStatus obStatus;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_PBAPS_Disable");

	BTL_VERIFY_ERR((0 != pbapsContext), BT_STATUS_INVALID_PARM, ("Null pbapsContext"));
	BTL_VERIFY_ERR((BTL_PBAPS_STATE_IDLE != pbapsContext->state), BT_STATUS_FAILED, ("PBAPS context is not enabled"));

	if (pbapsContext->state == BTL_PBAPS_STATE_DISABLING)
	{
		pbapsContext->disableState |= BTL_PBAPS_DISABLE_STATE_MASK_DISABLE_ACTIVE;

		BTL_RET(BT_STATUS_PENDING);
	}

	switch (pbapsContext->state)
	{
		case (BTL_PBAPS_STATE_DISCONNECTING):
		{
			/* PBAPS state is now in the process of disabling */
			pbapsContext->state = BTL_PBAPS_STATE_DISABLING;
			break;
		}

		case (BTL_PBAPS_STATE_OPER_ON_GOING):
			/* Abort the phonebook and OBEX operation */
			BtlPbapsPbAbortOperation(FALSE);
			PBAP_ServerAbort(&pbapsContext->server, PBRC_SERVICE_UNAVAILABLE);
			/* Drop into next case */
		case (BTL_PBAPS_STATE_CONNECTED):
		{
			obStatus = PBAP_ServerTpDisconnect(&pbapsContext->server);
						
			if (OB_STATUS_PENDING == obStatus)
			{
				status = BT_STATUS_PENDING; 
				/* PBAPS state is now in the process of disabling */
				pbapsContext->state = BTL_PBAPS_STATE_DISABLING;
				break;
			}
			/* No connection, try to deregister (later after the switch case) */
			
		}
		case (BTL_PBAPS_STATE_DISCONNECTED):
		{
			break;
		}
		default:
		{
			/* This verify will fail and will handle the exception gracefully */
			BTL_VERIFY_ERR((BTL_PBAPS_STATE_DISCONNECTED == pbapsContext->state), BT_STATUS_FAILED, ("Disable failed, invalid PBAPS state"));
			break;
		}
	}

	if (BTL_PBAPS_STATE_DISABLING != pbapsContext->state)
	{
		status = BtlPbapsDisable(pbapsContext);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed disabling PBAPS context"));
	}
	else
	{
		pbapsContext->disableState |= BTL_PBAPS_DISABLE_STATE_MASK_DISABLE_ACTIVE;
	}
	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

#if BT_SECURITY == XA_ENABLED
/*-------------------------------------------------------------------------------
 * BTL_PBAPS_SetSecurityLevel()
 */
BtStatus BTL_PBAPS_SetSecurityLevel(BtlPbapsContext *pbapsContext,
                                    BtSecurityLevel *secLevel)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_PBAPS_SetSecurityLevel");

	BTL_VERIFY_ERR((0 != pbapsContext), BT_STATUS_INVALID_PARM, ("Null pbapsContext"));

	if (0 != secLevel)
	{
		status = BTL_VerifySecurityLevel(*secLevel);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Invalid PBAPS securityLevel"));
	}

	/* Apply the new security level */
	pbapsContext->server.sApp.secRecord.level = (U8)((secLevel == 0) ? (BSL_PBAPS_DEFAULT) : (*secLevel));

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

/*-------------------------------------------------------------------------------
 * BTL_PBAPS_GetSecurityLevel()
 */
 BtStatus BTL_PBAPS_GetSecurityLevel(BtlPbapsContext *pbapsContext,
                                     BtSecurityLevel *secLevel)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_PBAPS_GetSecurityLevel");

	BTL_VERIFY_ERR((0 != pbapsContext), BT_STATUS_INVALID_PARM, ("Null pbapsContext"));
	BTL_VERIFY_ERR((0 != secLevel), BT_STATUS_INVALID_PARM, ("Null securityLevel"));

	*secLevel = pbapsContext->server.sApp.secRecord.level;
	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}
#endif	/* BT_SECURITY == XA_ENABLED */


/*---------------------------------------------------------------------------
 * BTL_PBAPS_Abort()
 */ 
BtStatus BTL_PBAPS_Abort(BtlPbapsContext *pbapsContext,
                         PbapRespCode resp)
{

	BtStatus status = BT_STATUS_SUCCESS;
	ObStatus obStatus;

	BTL_FUNC_START_AND_LOCK("BTL_PBAPS_Abort");

	BTL_VERIFY_ERR((0 != pbapsContext), BT_STATUS_INVALID_PARM, ("Null pbapsContext"));
	BTL_VERIFY_ERR((BTL_PBAPS_STATE_CONNECTED == pbapsContext->state) || 
					(BTL_PBAPS_STATE_OPER_ON_GOING == pbapsContext->state)
					, BT_STATUS_NO_CONNECTION, ("PBAPS context is not connected"));

	if (BTL_PBAPS_STATE_OPER_ON_GOING == pbapsContext->state)
	{
		BtlPbapsPbAbortOperation(FALSE);
	}
	
	obStatus = PBAP_ServerAbort(&pbapsContext->server, resp);
	BTL_VERIFY_ERR((OB_STATUS_INVALID_PARM != obStatus), BT_STATUS_INVALID_PARM, ("Invalid param"));
	BTL_VERIFY_ERR((OB_STATUS_SUCCESS == obStatus), BT_STATUS_FAILED, ("Server abort falied"));

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

/*-------------------------------------------------------------------------------
 * BTL_PBAPS_GetConnectedDevice()
 */
BtStatus BTL_PBAPS_GetConnectedDevice(BtlPbapsContext *pbapsContext, 
										BD_ADDR *bdAddr)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BOOL retVal;
	ObexTpConnInfo tpInfo;
	
	BTL_FUNC_START_AND_LOCK("BTL_PBAPS_GetConnectedDevice");

	BTL_VERIFY_ERR((0 != pbapsContext), BT_STATUS_INVALID_PARM, ("Null pbapsContext"));
	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));
	BTL_VERIFY_ERR((BTL_PBAPS_STATE_CONNECTED == pbapsContext->state) ||
				   (BTL_PBAPS_STATE_OPER_ON_GOING == pbapsContext->state), 
			        BT_STATUS_NO_CONNECTION, ("PBAPS context is not connected"));

	tpInfo.size = sizeof(ObexTpConnInfo);
	tpInfo.tpType = OBEX_TP_BLUETOOTH;
	tpInfo.remDev = 0;

	retVal = GOEP_ServerGetTpConnInfo(&pbapsContext->server.sApp, &tpInfo);
	BTL_VERIFY_ERR((TRUE == retVal), BT_STATUS_NO_CONNECTION, ("PBAPS get connection info failed"));

	BTL_VERIFY_ERR((0 != tpInfo.remDev), BT_STATUS_INTERNAL_ERROR, ("Unable to find connected remote device"));
	
	OS_MemCopy((U8 *)bdAddr, (const U8 *)(&(tpInfo.remDev->bdAddr)), sizeof(BD_ADDR));

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*---------------------------------------------------------------------------
 * BTL_PBAPS_Disconnect()
 */
BtStatus BTL_PBAPS_Disconnect(BtlPbapsContext *pbapsContext)
{
	BtStatus status = BT_STATUS_PENDING;
	ObStatus obStatus;
	
	BTL_FUNC_START_AND_LOCK("BTL_PBAPS_Disconnect");

	BTL_VERIFY_ERR((0 != pbapsContext), BT_STATUS_INVALID_PARM, ("Null pbapsContext"));
	BTL_VERIFY_ERR((BTL_PBAPS_STATE_CONNECTED == pbapsContext->state) ||
				   (BTL_PBAPS_STATE_OPER_ON_GOING == pbapsContext->state), 
					BT_STATUS_NO_CONNECTION, ("PBAPS context is not connected"));
	
	/* Close the phonebook if an operation is on going */
	if (BTL_PBAPS_STATE_OPER_ON_GOING == pbapsContext->state)
	{
		BtlPbapsPbAbortOperation(FALSE);
	}
	obStatus = PBAP_ServerTpDisconnect(&pbapsContext->server);
	BTL_VERIFY_ERR((OB_STATUS_BUSY != obStatus), BT_STATUS_BUSY, ("PBAPS is currently executing an operation"));
	BTL_VERIFY_ERR((OB_STATUS_PENDING == obStatus), BT_STATUS_FAILED, ("Failed disconnecting PBAS server"));
	
	/* PBAPS state is now in the process of disconnecting */
	pbapsContext->state = BTL_PBAPS_STATE_DISCONNECTING;

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

#if OBEX_AUTHENTICATION == XA_ENABLED
/*-------------------------------------------------------------------------------
 * BTL_PBAPS_ConfigInitiatingObexAuthentication()
 */
BtStatus BTL_PBAPS_ConfigInitiatingObexAuthentication(BtlPbapsContext *pbapsContext, 
													const BOOL enableObexAuth,
													const BtlUtf8 *realm, 
													const BtlUtf8 *userId, 
													const BtlUtf8 *password)
{
	ObStatus obStatus;
	BtStatus status = BT_STATUS_SUCCESS;
	PbapAuthInfo pbapAuthInfo;
	BtlUtf8 obexRealm[BTL_CONFIG_OBEX_MAX_REALM_LEN+ 2];
	U16 len;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_PBAPS_ConfigInitiatingObexAuthentication");

	BTL_VERIFY_ERR((0 != pbapsContext), BT_STATUS_INVALID_PARM, ("Null pbapsContext"));
	
	if (enableObexAuth == TRUE)
	{
		BTL_VERIFY_ERR((0 != password), BT_STATUS_INVALID_PARM, ("Null password"));
		
		if (realm != 0)
		{
			/* Check valid realm */
			len = OS_StrLenUtf8(realm);
			BTL_VERIFY_ERR((len <= BTL_CONFIG_OBEX_MAX_REALM_LEN), BT_STATUS_INVALID_PARM, ("Too long realm"));

			/* Now safely copy the realm, starting from the second byte */
			obexRealm[0] = '\0';
			OS_StrnCpyUtf8((obexRealm + 1), realm, BTL_CONFIG_OBEX_MAX_REALM_LEN);
			pbapAuthInfo.realm = obexRealm;
			pbapAuthInfo.realmLen = (U16)(len + 1);
		}
		else
		{
			/* The first byte of the string is the character set. The ASCII character set is value 0 */
			obexRealm[0] = '\0';
			obexRealm[1] = '\0';
			pbapAuthInfo.realm = obexRealm;
			pbapAuthInfo.realmLen = 1;
		}

		if (userId != 0)
		{
			/* Check valid userId */
			len = OS_StrLenUtf8(userId);
			BTL_VERIFY_ERR((len <= BTL_CONFIG_OBEX_MAX_USER_ID_LEN), BT_STATUS_INVALID_PARM, ("Too long userId"));

			/* Now safely copy the userId */
			pbapAuthInfo.userId = userId;
			pbapAuthInfo.userIdLen = len ;
		}
		else
		{
			pbapAuthInfo.userId = 0;
			pbapAuthInfo.userIdLen = 0;
		}
		
		/* Check valid password */
		len = OS_StrLenUtf8(password);
		BTL_VERIFY_ERR((len <= BTL_CONFIG_OBEX_MAX_PASS_LEN), BT_STATUS_INVALID_PARM, ("Too long password"));

		/* Now safely copy the password */
		pbapAuthInfo.password = password; 
		pbapAuthInfo.passwordLen = len;

		/* The lower level PBAP profile copies the content of pbapAuthInfo so we 
		 * can give it as an input argument 
		 */
		obStatus = PBAP_ServerSetAuthInfo(&pbapsContext->server, &pbapAuthInfo);
		BTL_VERIFY_ERR((OB_STATUS_SUCCESS == obStatus), OB_STATUS_FAILED, ("Failed setting auth info"));
		
	}

	pbapsContext->initiateObexAuth = enableObexAuth;

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}
#endif	/* OBEX_AUTHENTICATION == XA_ENABLED */

#if OBEX_AUTHENTICATION == XA_ENABLED
/*-------------------------------------------------------------------------------
 * BTL_PBAPS_ObexAuthenticationResponse()
 */
BtStatus BTL_PBAPS_ObexAuthenticationResponse(BtlPbapsContext *pbapsContext,
												const BtlUtf8 *userId, 
												const BtlUtf8 *password)
{
	BtStatus status = BT_STATUS_SUCCESS;
	ObStatus obStatus;
	ObexAuthResponse rsp;
	U16 len;

	BTL_FUNC_START_AND_LOCK("BTL_PBAPS_ObexAuthenticationResponse");

	BTL_VERIFY_ERR((0 != pbapsContext), BT_STATUS_INVALID_PARM, ("Null pbapsContext"));
	BTL_VERIFY_ERR((0 != password), BT_STATUS_INVALID_PARM, ("Null password"));
	BTL_VERIFY_ERR((pbapsContext->obexAuthFlags & OBEX_AUTH_RECEIVED_CHAL), BT_STATUS_FAILED, ("No OBEX auth challenge was received"));

	if (userId != 0)
	{
		/* Check valid userId */
		len = OS_StrLenUtf8(userId);
		BTL_VERIFY_ERR((len <= BTL_CONFIG_OBEX_MAX_USER_ID_LEN), BT_STATUS_INVALID_PARM, ("Too long userId"));

		/* Now safely copy the userId */
		OS_StrnCpyUtf8(pbapsContext->server.userId, userId, BTL_CONFIG_OBEX_MAX_USER_ID_LEN);
		pbapsContext->server.userId[len] = '\0';

		rsp.userId = pbapsContext->server.userId;
    	rsp.userIdLen = (U8)len;
	}
	else
	{
		rsp.userId = 0;
    	rsp.userIdLen = 0;
	}

	len = OS_StrLenUtf8(password);

	/* Now safely copy the password */
	OS_StrnCpyUtf8(pbapsContext->server.password, password, BTL_CONFIG_OBEX_MAX_PASS_LEN);
	pbapsContext->server.password[len] = '\0';

	rsp.password = pbapsContext->server.password;
	rsp.passwordLen = (U8)len;

	pbapsContext->server.resp = rsp;

	/* Respond to the challenge */
	pbapsContext->obexAuthFlags &= (~OBEX_AUTH_RECEIVED_CHAL);

 	if (pbapsContext->obexAuthFlags & OBEX_AUTH_RECEIVED_CHAL_CONT)
	{
		/* Continue event occured, need to call server continue */
		pbapsContext->obexAuthFlags &= (~OBEX_AUTH_RECEIVED_CHAL_CONT);

		obStatus = PBAP_ServerContinue(&pbapsContext->server);
		BTL_VERIFY_ERR((OB_STATUS_SUCCESS == obStatus), BT_STATUS_INTERNAL_ERROR, ("The server is not expecting a continue"));
	}

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}
#endif	/* OBEX_AUTHENTICATION == XA_ENABLED */


/*-------------------------------------------------------------------------------
 * BtlPbapsDisable()
 *
 *		Internal function for deregistering security record.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		avrcptgContext [in] - pointer to the PBAPS context.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 * 
 * 		BT_STATUS_BUSY - Server busy
 */
static BtStatus BtlPbapsDisable(BtlPbapsContext *pbapsContext)
{
	BtStatus status = BT_STATUS_SUCCESS;
	ObStatus obStatus;
	
	BTL_FUNC_START("BtlPbaps");
 
	/* First, try to deregister channel */
	obStatus = PBAP_DeregisterServer(&pbapsContext->server);
	BTL_VERIFY_ERR((OB_STATUS_BUSY != obStatus), BT_STATUS_BUSY, ("PBAPS is currently executing an operation"));
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), BT_STATUS_FAILED, ("Failed deregistering PBAPS context"));

#if BT_SECURITY == XA_ENABLED

	/* In case the security level is BSL_NO_SECURITY, ESI deregister fucntion doesn't register security 
	 * record. Therefore, in that case, security deregistration is done below 
	 */
	if (pbapsContext->server.secLevel == BSL_NO_SECURITY) 
	{
        
        obStatus = GOEP_UnregisterServerSecurityRecord(&pbapsContext->server.sApp);
		BTL_VERIFY_ERR((OB_STATUS_SUCCESS == obStatus), BT_STATUS_FAILED, ("Failed deregistering PBAPS context"));
	}
#endif	/* BT_SECURITY == XA_ENABLED */
	
	pbapsContext->state = BTL_PBAPS_STATE_IDLE;

	BTL_LOG_INFO(("BTL_PBAPS: Target is disabled."));

	BTL_FUNC_END();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BtlPbapsCallback()
 *
 *		Internal callback for handling PBAPS events in BTL layer.
 *		This function forwards events to registered application callback.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		
 *		Parms [in] - Internal PBAPS event.
 *
 * Returns:
 *		void.
 */

static void BtlPbapsCallback(PbapServerCallbackParms *Parms)
{
	BtStatus 							  status = BT_STATUS_SUCCESS;
	ObStatus 					  		  obStatus;
	BtlPbapsContext						 *pbapsContext;
	BOOL 								  passEventToApp = TRUE;
	BtlPbapsPbSetPathFlags				  setPathFlags;
	static BOOL                    		  provideParams = FALSE;
	BOOL 								  sendDisableEvent = FALSE;
	BOOL 								  notifyRadioOffComplete = FALSE;
	static BOOL							  abortOperation;
	
	
	BTL_FUNC_START("BtlPbapsCallback");
	
	/* Find context according to given PBAP event */
	pbapsContext = ContainingRecord(Parms->server, BtlPbapsContext, server);
	
	/* Handle special case of disabling */
	if (pbapsContext->state == BTL_PBAPS_STATE_DISABLING)
	{	
		switch (Parms->event)
		{
			case (PBAP_EVENT_TP_DISCONNECTED):
			{
				/* The transport layer connection has been disconnected. */
				BTL_LOG_INFO(("BTL_PBAPS: Transport layer connection has been disconnected."));
#if OBEX_AUTHENTICATION == XA_ENABLED
        		pbapsContext->obexAuthFlags = 0;
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */			

				pbapsContext->state = BTL_PBAPS_STATE_DISCONNECTED;

				if (pbapsContext->disableState & BTL_PBAPS_DISABLE_STATE_MASK_DISABLE_ACTIVE)
				{				
					BtlPbapsDisable(pbapsContext);
					sendDisableEvent = TRUE;
				}
				if (pbapsContext->disableState & BTL_PBAPS_DISABLE_STATE_MASK_RADIO_OFF_ACTIVE)
				{
					BtlContext *base;
					BtlPbapsContext *context;

					notifyRadioOffComplete = TRUE;
					IterateList(btlPbapsData.contextsList, base, BtlContext *)
					{
						context = (BtlPbapsContext *)base;
						if ((context->state != BTL_PBAPS_STATE_DISCONNECTED) && (context->state != BTL_PBAPS_STATE_IDLE))
								notifyRadioOffComplete = FALSE;			
					}
				}

				pbapsContext->disableState = BTL_PBAPS_DISABLE_STATE_MASK_NONE;
				/* Pass event to app */
				break;
			}
			case PBAP_EVENT_ABORTED:
			{
		        BTL_LOG_INFO(("Server: PBAP %s aborted.", BtlPbapsOpName(Parms->oper))); 
		        break;
				/* Pass event to app. */
		    }

				
			
			default:
			{
				BTL_LOG_ERROR(("BTL_PBAPS: Received unexpected event %d while disabling!", Parms->event));	
				
				/* Do not pass event to app. */
				passEventToApp = FALSE;
				break;
			}
		}
	}
	else
	{			
		switch (Parms->event)
		{
			case (PBAP_EVENT_TP_DISCONNECTED):
			{
				/* The transport layer connection has been disconnected. */
				BTL_LOG_INFO(("BTL_PBAP_EVENT_TP_DISCONNECTED: Transport layer connection has been disconnected."));
				
#if OBEX_AUTHENTICATION == XA_ENABLED
        		pbapsContext->obexAuthFlags = 0;
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */			

				pbapsContext->state = BTL_PBAPS_STATE_DISCONNECTED;

				/* Pass event to app */
				break;
			}
			case PBAP_EVENT_TP_CONNECTED:
			{
 			    pbapsContext->state = BTL_PBAPS_STATE_CONNECTED;
				
		        BTL_LOG_INFO(("Server: PBAP Transport Connection has come up."));

#if OBEX_PROVIDE_SDP_RESULTS == XA_ENABLED
		        if (0 != Parms->u.connect.profileVersion) 
				{
		            BTL_LOG_INFO(("Server: PBAP Client profile version %x.%x", (U8)(Parms->u.connect.profileVersion >> 8), 
		                       (U8)(Parms->u.connect.profileVersion)) );
		        }
#endif /* OBEX_PROVIDE_SDP_RESULTS == XA_ENABLED */

				/* Reset the 'changed' status of the phonebook*/
				BtlPbapsPbSetChangedStatus(FALSE);
		
				/* Reset the phonebook path for the new connection */
				status = BtlPbapsPbSetPbPath(0, BTL_PBAPS_PB_SET_PATH_RESET);
				
				if (BT_STATUS_FAILED == status)
					{
					    /* Set Phonebook Path failed */
						BTL_LOG_INFO(("BtlPbapsPbSetPbPath Failed"));
						obStatus = PBAP_ServerAbort(&pbapsContext->server, PBRC_NOT_ACCEPTABLE);
						BTL_VERIFY_ERR_NO_RETVAR((OB_STATUS_SUCCESS == obStatus), ("Server abort falied"));
		            }
		        break;
				/* Pass event to app. */
		    }
#if OBEX_AUTHENTICATION == XA_ENABLED
		    case PBAP_EVENT_AUTH_RESULT:
			{
				if (TRUE == Parms->u.auth.result)
				{
					pbapsContext->obexAuthFlags |= OBEX_AUTH_AUTHENTICATED;
					BTL_LOG_INFO(("BTL_PBAPS: Received a positive OBEX authentication response"));			
		            BTL_LOG_INFO(("Server: PBAP Authentication succeeded."));
		            BTL_VERIFY_ERR(Parms->u.auth.reason == PBAP_AUTH_NO_FAILURE, BT_STATUS_FAILED, ("Server: PBAP wrong OBEX authentication reason"));
		        } 
				else 
		        {
		            if (Parms->u.auth.reason == PBAP_AUTH_NO_RCVD_AUTH_RESP) 
					{
		                BTL_LOG_INFO(("Server: PBAP Authentication failed: No Auth Resp Received."));
		            } 
					else if (Parms->u.auth.reason == PBAP_AUTH_INVALID_CREDENTIALS) 
					{
		                BTL_LOG_INFO(("Server: PBAP Authentication failed: Invalid Credentials."));
		            } 
					else 
					{
						BTL_ERR_NORET(("Server: Invalid OBEX authentication result reason"));
		            }
		            BTL_LOG_INFO(("Server: PBAP Client must retry Connect."));
		        }
				/* Pass event to app. */
		        break;
		    }
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

		    case PBAP_EVENT_PARAMS_RX:
			{
		        BTL_LOG_INFO(("Server: PBAP Received App Parameters."));

		        if (PBAPOP_PULL_PHONEBOOK == Parms->oper) 
				{
					BtlPbapsPbSetPullPbParams(Parms);
		        } 
				else if (PBAPOP_PULL_VCARD_LISTING == Parms->oper) 
		        {	
		        	BtlPbapsPbSetPullListParams(Parms);
		        } 
				else if (PBAPOP_PULL_VCARD_ENTRY == Parms->oper) 
				{	
					BtlPbapsPbSetPullEntryParams(Parms);
		        }
				/* Do not pass event to app. */
				passEventToApp = FALSE;
		        break;
		    }
		    case PBAP_EVENT_PROVIDE_PARAMS:
			{
		        BTL_LOG_INFO(("Server: PBAP Provide Parameters Event."));
		        
				provideParams = TRUE;
				
				/* Phonebook Size  - Pull Phonebook and Pull Vcard Listing operations only */
		        if ((PBAPOP_PULL_PHONEBOOK != Parms->oper) && (PBAPOP_PULL_VCARD_LISTING != Parms->oper))
				{
				    BTL_ERR(BT_STATUS_INTERNAL_ERROR, ("Wrong operatiopn type"));
		        }
		       	Parms->u.provideParams.asynchronous = TRUE;			
				/* Do not pass event to app. */
				passEventToApp = FALSE;
		        break;
		    }

		    case PBAP_EVENT_OBJECT_LENGTH_REQ:
			{
		        BTL_LOG_INFO(("Server: PBAP Object Length Request."));
				BtlPbapsPbUpdateNameParam(Parms->oper, (char *)Parms->u.objectReq.name);
		        Parms->u.objectReq.objectLen = 0;
				/* Do not provide a length in order avoid complex and long operations with the device's phonebook
				 * In this case we route PBAP to use the PBAPOBS_ReadFlex routine
				 */
		        if ((PBAPOP_PULL_PHONEBOOK == Parms->oper) || (PBAPOP_PULL_VCARD_LISTING == Parms->oper)  
														  || (PBAPOP_PULL_VCARD_ENTRY == Parms->oper))
				{
#if OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED
		            /* Unknown object length, the folder listing will be created dynamically */
		            Parms->u.objectReq.objectLen = PBAP_UNKNOWN_OBJECT_LENGTH;
#endif /* OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED */
		        } 
				else 
				{
		            /* Unknown PBAP operation */
					obStatus = PBAP_ServerAbort(&pbapsContext->server, PBRC_NOT_ACCEPTABLE);
					BTL_VERIFY_ERR_NO_RETVAR((OB_STATUS_SUCCESS == obStatus), ("Server abort falied"));
		            BTL_LOG_INFO(("Server: Invalid Pull operation. Aborting..."));
		        }
				/* Do not pass event to app. */
				passEventToApp = FALSE;
				break;
		    }
			
		    case PBAP_EVENT_DATA_REQ:
			{
#if OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED

		        BTL_LOG_INFO(("Server: PBAP Data Request Event."));
				/* Expect only the use of PBAPOBS_ReadFlex for this operation, since we
	             * provided an unknown object length during PBAP_EVENT_OBJECT_LENGTH_REQ.
	             * This is done since we generate the XML folder listing dynamically as
	             * the data requests are passed to the application.
	             */
	            BTL_VERIFY_ERR(Parms->u.dataReq.type == PBAP_DATAREQ_READ_FLEX, BT_STATUS_INTERNAL_ERROR, ("Wrong use of data request")); 
				
		        if (  (PBAPOP_PULL_PHONEBOOK == Parms->oper) || 
					  (PBAPOP_PULL_VCARD_LISTING == Parms->oper) ||
					  (PBAPOP_PULL_VCARD_ENTRY == Parms->oper)      )
					
				{
		            status = BtlPbapsPbGetData(Parms->u.dataReq.buffer, Parms->u.dataReq.flex.len,
		                              									  Parms->u.dataReq.flex.more);

					if (BT_STATUS_FAILED == status)
					{
						/* Get phonebook data failed */
						BTL_LOG_INFO(("BtlPbapsPbGetData Failed"));
						obStatus = PBAP_ServerAbort(&pbapsContext->server, PBRC_SERVICE_UNAVAILABLE);
						BTL_VERIFY_ERR_NO_RETVAR((OB_STATUS_SUCCESS == obStatus), ("Server abort falied"));

						
					}
		            
		        } 
				else 
				{
		            BTL_ERR(BT_STATUS_INTERNAL_ERROR, ("Wrong operatiopn type"));
		        }
#endif /* OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED */
				/* Do not pass event to app. */
				passEventToApp = FALSE;
		        break;
		    }
		    case PBAP_EVENT_START:
			{
				/* reset abort operation flag */
				abortOperation = FALSE;
				
		        if (PBAPOP_NONE == Parms->oper) 
				{
		            /* We don't know the exact operation type yet. We will need to get
		             * more information from the headers to determine this.
		             */
		            BTL_LOG_INFO(("Server: PBAP Receiving an operation"));
					
		        } 
				else if ( (PBAPOP_PULL == Parms->oper) || (PBAPOP_SET_PHONEBOOK == Parms->oper) )
					 
				{
					BtlPbapsPbStartOper();
					
					pbapsContext->state = BTL_PBAPS_STATE_OPER_ON_GOING;
					
				}
				else if ( (PBAPOP_PULL_PHONEBOOK == Parms->oper) || (PBAPOP_PULL_VCARD_LISTING == Parms->oper) || 
					      (PBAPOP_PULL_VCARD_ENTRY == Parms->oper) )
				{
					/* An abort operation starts. (actually 'Parms->oper' will contain the value  
					 * of the on going operation, for example - PBAPOP_PULL_PHONEBOOK. But that is 
					 * probably inaccuracy of ESI)
					 */
		            BTL_LOG_INFO(("Server: PBAP Receiving an %s operation",BtlPbapsOpName(Parms->oper)));
					abortOperation = TRUE;
										
				}
				else
				{
					BTL_LOG_INFO(("Server: PBAP Receiving an %s operation",BtlPbapsOpName(Parms->oper)));
				}

				break;
				/* Pass event to app. */
		    }

		    case PBAP_EVENT_ABORTED:
			{
		        BTL_LOG_INFO(("Server: PBAP %s aborted.", BtlPbapsOpName(Parms->oper))); 
		        break;
				/* Pass event to app. */
		    }
		    case PBAP_EVENT_CONTINUE:
			{
#if OBEX_AUTHENTICATION == XA_ENABLED
				/* Dilling with authentication initialization by server */
				if ((TRUE == pbapsContext->initiateObexAuth) && (PBAPOP_CONNECT == Parms->oper) &&
					!(pbapsContext->obexAuthFlags & OBEX_AUTH_AUTHENTICATED) ) 
				{
					/* Setup a challenge to be sent out during PBAP's OBEX connection */
					BTL_LOG_INFO(("Server: Setting up authentication challenge")); 
					obStatus = PBAP_ServerAuthenticate(Parms->server);
					BTL_VERIFY_ERR(OB_STATUS_SUCCESS == obStatus, BT_STATUS_INTERNAL_ERROR, ("Server: PBAP initiation auth failure"));
					
				}

				/* Dilling with authentication response (initiated by client) */
				if (pbapsContext->obexAuthFlags & OBEX_AUTH_RECEIVED_CHAL)
				{
					/* Server received a challenge, and now a continue event */
					pbapsContext->obexAuthFlags |= OBEX_AUTH_RECEIVED_CHAL_CONT;
				}
				else 
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
				if ( (TRUE == provideParams) && (TRUE == abortOperation) )
				{
				/* In that case we got an abort even before the operation has started. Just call continue 
				 * to free the stack
				 */
				 	obStatus = PBAP_ServerContinue(Parms->server);
					BTL_VERIFY_ERR(OB_STATUS_SUCCESS == obStatus, BT_STATUS_INTERNAL_ERROR, ("Server: PBAP_ServerContinue failure"));
				}
				else if (TRUE == provideParams)
				/* Need to provide the application parameters now */
				{
					if (PBAPOP_PULL_PHONEBOOK == Parms->oper) 
					{
						status = BtlPbapsPbGetPbParams(TRUE, TRUE);
						
						if ((BT_STATUS_FAILED == status)|| (BT_STATUS_BUSY == status) )
						{
							/* Get phonebook params failed -
							 * Abort stack  
							 */
							obStatus = PBAP_ServerAbort(&pbapsContext->server, PBRC_NOT_FOUND);
							BTL_VERIFY_ERR_NO_RETVAR((OB_STATUS_SUCCESS == obStatus), ("Server abort falied"));

							/* Call continue to keep the event flowing */
							obStatus = PBAP_ServerContinue(Parms->server);
							BTL_VERIFY_ERR(OB_STATUS_SUCCESS == obStatus, BT_STATUS_INTERNAL_ERROR, ("Server: PBAP_ServerContinue failure"));
							
						}
						else if (BT_STATUS_INVALID_PARM == status)
						{
							/* Get phonebook params failed */
							BTL_LOG_DEBUG(("BtlPbapsPbGetPbParams Failed"));
							/* Abort stack  */
							obStatus = PBAP_ServerAbort(&pbapsContext->server, PBRC_BAD_REQUEST);
							BTL_VERIFY_ERR_NO_RETVAR((OB_STATUS_SUCCESS == obStatus), ("Server abort falied"));

							/* Call continue to keep the event flowing */
							obStatus = PBAP_ServerContinue(Parms->server);
							BTL_VERIFY_ERR(OB_STATUS_SUCCESS == obStatus, BT_STATUS_FAILED, ("Server: PBAP_ServerContinue failure"));
						}
						else if (BT_STATUS_NO_RESOURCES == status)
						{
							/* Get phonebook params failed */
							BTL_LOG_DEBUG(("BtlPbapsPbGetPbParams Failed"));
							/* Abort stack  */
							obStatus = PBAP_ServerAbort(&pbapsContext->server, PBRC_NOT_ACCEPTABLE);
							BTL_VERIFY_ERR_NO_RETVAR((OB_STATUS_SUCCESS == obStatus), ("Server abort falied"));

							/* Call continue to keep the event flowing */
							obStatus = PBAP_ServerContinue(Parms->server);
							BTL_VERIFY_ERR(OB_STATUS_SUCCESS == obStatus, BT_STATUS_FAILED, ("Server: PBAP_ServerContinue failure"));
						}
			        } 
					else if (PBAPOP_PULL_VCARD_LISTING == Parms->oper) 
			        {
			            status = BtlPbapsPbGetPbParams(FALSE, TRUE);
						if ( (BT_STATUS_FAILED == status) || (BT_STATUS_BUSY == status) )
						{
							/* Get phonebook params failed */
							BTL_LOG_DEBUG(("BtlPbapsPbGetPbParams Failed"));
							/* Abort stack  */
							obStatus = PBAP_ServerAbort(&pbapsContext->server, PBRC_SERVICE_UNAVAILABLE);
							BTL_VERIFY_ERR_NO_RETVAR((OB_STATUS_SUCCESS == obStatus), ("Server abort falied"));

							/* Call continue to keep the event flowing */
							obStatus = PBAP_ServerContinue(Parms->server);
							BTL_VERIFY_ERR(OB_STATUS_SUCCESS == obStatus, BT_STATUS_FAILED, ("Server: PBAP_ServerContinue failure"));
						}
						else if (BT_STATUS_INVALID_PARM == status)
						{
							/* Get phonebook params failed */
							BTL_LOG_DEBUG(("BtlPbapsPbGetPbParams Failed"));
							/* Abort stack  */
							obStatus = PBAP_ServerAbort(&pbapsContext->server, PBRC_BAD_REQUEST);
							BTL_VERIFY_ERR_NO_RETVAR((OB_STATUS_SUCCESS == obStatus), ("Server abort falied"));

							/* Call continue to keep the event flowing */
							obStatus = PBAP_ServerContinue(Parms->server);
							BTL_VERIFY_ERR(OB_STATUS_SUCCESS == obStatus, BT_STATUS_FAILED, ("Server: PBAP_ServerContinue failure"));
						}
						else if (BT_STATUS_NO_RESOURCES == status)
						{
							/* Get phonebook params failed */
							BTL_LOG_DEBUG(("BtlPbapsPbGetPbParams Failed"));
							/* Abort stack  */
							obStatus = PBAP_ServerAbort(&pbapsContext->server, PBRC_NOT_ACCEPTABLE);
							BTL_VERIFY_ERR_NO_RETVAR((OB_STATUS_SUCCESS == obStatus), ("Server abort falied"));

							/* Call continue to keep the event flowing */
							obStatus = PBAP_ServerContinue(Parms->server);
							BTL_VERIFY_ERR(OB_STATUS_SUCCESS == obStatus, BT_STATUS_FAILED, ("Server: PBAP_ServerContinue failure"));
						}
						else if (BT_STATUS_IMPROPER_STATE == status)
						{
							/* Get phonebook params failed */
							BTL_LOG_DEBUG(("BtlPbapsPbGetPbParams Failed"));
							/* Abort stack  */
							obStatus = PBAP_ServerAbort(&pbapsContext->server, PBRC_PRECONDITION_FAILED);
							BTL_VERIFY_ERR_NO_RETVAR((OB_STATUS_SUCCESS == obStatus), ("Server abort falied"));

							/* Call continue to keep the event flowing */
							obStatus = PBAP_ServerContinue(Parms->server);
							BTL_VERIFY_ERR(OB_STATUS_SUCCESS == obStatus, BT_STATUS_FAILED, ("Server: PBAP_ServerContinue failure"));
						}
			        } 
					provideParams = FALSE;
				}
				else if (TRUE == abortOperation)
				{
					BtlPbapsPbAbortOperation(TRUE);
				}
				else if (PBAPOP_PULL_PHONEBOOK == Parms->oper) 
				{
					status = BtlPbapsPbBuildPbData(TRUE);
					if (BT_STATUS_FAILED == status)
					{
						BTL_LOG_DEBUG(("BtlPbapsPbBuildPbData Failed"));
						/* Abort both stack and phonebook */
						obStatus = PBAP_ServerAbort(&pbapsContext->server, PBRC_NOT_ACCEPTABLE);
						BTL_VERIFY_ERR_NO_RETVAR((OB_STATUS_SUCCESS == obStatus), ("Server abort falied"));

						BtlPbapsPbAbortOperation(TRUE);
						
					}
					
		        } 
				else if (PBAPOP_PULL_VCARD_LISTING == Parms->oper) 
				{
					status = BtlPbapsPbBuildFolderListingData(TRUE);
					if (BT_STATUS_FAILED == status)
					{
						BTL_LOG_INFO(("BtlPbapsPbBuildFolderListingData Failed"));
						/* Abort both stack and phonebook */
						obStatus = PBAP_ServerAbort(&pbapsContext->server, PBRC_SERVICE_UNAVAILABLE);
						BTL_VERIFY_ERR_NO_RETVAR((OB_STATUS_SUCCESS == obStatus), ("Server abort falied"));

						BtlPbapsPbAbortOperation(TRUE);
						
					}
		        } 
				else if (PBAPOP_PULL_VCARD_ENTRY == Parms->oper) 
				{
					status = BtlPbapsPbBuildEntryData(TRUE);

					if (BT_STATUS_IMPROPER_STATE == status)
					{
						/* The phonebook wasn't opened yet. No need to abort its operation */
						obStatus = PBAP_ServerAbort(&pbapsContext->server, PBRC_PRECONDITION_FAILED);
						BTL_VERIFY_ERR_NO_RETVAR((OB_STATUS_SUCCESS == obStatus), ("Server abort falied"));

						/* Call continue to send the abort operation */
						obStatus = PBAP_ServerContinue(Parms->server);
						BTL_VERIFY_ERR(BT_STATUS_FAILED != status, BT_STATUS_FAILED, ("PBAP_ServerContinue failed"));
					}
					if (BT_STATUS_NOT_FOUND == status)
					{
						/* Abort both stack and phonebook */
						obStatus = PBAP_ServerAbort(&pbapsContext->server, PBRC_NOT_FOUND);
						BTL_VERIFY_ERR_NO_RETVAR((OB_STATUS_SUCCESS == obStatus), ("Server abort falied"));

						/* Call continue to send the abort operation */
						obStatus = PBAP_ServerContinue(Parms->server);
						BTL_VERIFY_ERR(BT_STATUS_FAILED != status, BT_STATUS_FAILED, ("PBAP_ServerContinue failed"));
				

					}
					else if (BT_STATUS_FAILED == status)
					{
						BTL_LOG_DEBUG(("BtlPbapsPbBuildEntryData Failed"));
						/* Abort both stack and phonebook */
						obStatus = PBAP_ServerAbort(&pbapsContext->server, PBRC_NOT_ACCEPTABLE);
						BTL_VERIFY_ERR_NO_RETVAR((OB_STATUS_SUCCESS == obStatus), ("Server abort falied"));
						
						BtlPbapsPbAbortOperation(TRUE);
						
					}
					else if (BT_STATUS_BUSY == status)
					{
						/* Abort both stack and phonebook */
						obStatus = PBAP_ServerAbort(&pbapsContext->server, PBRC_SERVICE_UNAVAILABLE);
						BTL_VERIFY_ERR_NO_RETVAR((OB_STATUS_SUCCESS == obStatus), ("Server abort falied"));
						
					}
				}
				else
				{
					/* Call continue to keep the event flowing */
					obStatus = PBAP_ServerContinue(Parms->server);
					BTL_VERIFY_ERR(OB_STATUS_SUCCESS == obStatus, BT_STATUS_INTERNAL_ERROR, ("Server: PBAP initiation auth failure"));
				}
				
				/* Do not pass event to app. */
				passEventToApp = FALSE;
			    break;
		    }
	 		
#if OBEX_AUTHENTICATION == XA_ENABLED
			case PBAP_EVENT_AUTH_CHALLENGE:
			{
				/* Authentication challenge request has been received */
				BTL_LOG_INFO(("BTL_PBAPS: Received OBEX authentication challenge for %s operation.", BtlPbapsOpName(Parms->oper)));

#if OBEX_MAX_REALM_LEN > 0
				BTL_LOG_INFO(("BTL_PBAPS: Realm: %s, Charset %d , Options %d.", 
				Parms->u.challenge.realm+1, 
				Parms->u.challenge.realm[0], 
				Parms->u.challenge.options));
#endif /* OBEX_MAX_REALM_LEN > 0 */
				
		        pbapsContext->obexAuthFlags |= OBEX_AUTH_RECEIVED_CHAL;

				/* Pass event to app */
				break;
			}
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

		    case PBAP_EVENT_PRECOMPLETE:
			{
		        BTL_LOG_INFO( ("Server: PBAP %s precomplete.", BtlPbapsOpName(Parms->oper)) );
				if (PBAPOP_SET_PHONEBOOK == Parms->oper) 
				{
					/* Setting flags for the set path operation */
					if (TRUE == Parms->u.info.setPb.reset)
						setPathFlags = BTL_PBAPS_PB_SET_PATH_RESET;
					else if (Parms->u.info.setPb.flags & PBAP_SETPB_BACKUP)
						setPathFlags = BTL_PBAPS_PB_SET_PATH_BACKUP;
					else
						setPathFlags = BTL_PBAPS_PB_SET_PATH_LOCAL;

					status = BtlPbapsPbSetPbPath((char*)Parms->u.info.setPb.name, setPathFlags);
					if (BT_STATUS_FAILED == status)
					{
					    /* Set Phonebook Path failed */
						BTL_LOG_INFO(("BtlPbapsPbSetPbPath Failed"));
		                BTL_PBAPS_Abort(pbapsContext, PBRC_BAD_REQUEST);
		            }
				}
				/* Do not pass event to app. */
				passEventToApp = FALSE;
		        break;
		    }
		    case PBAP_EVENT_COMPLETE:
			{
		        BTL_LOG_DEBUG(("Server: PBAP %s complete.", BtlPbapsOpName(Parms->oper)));

				/* In order to be consistent with the PBAP_EVENT_START operation name, 
				 * change the PBAP operation name
				 */
				if ( (PBAPOP_PULL_PHONEBOOK == Parms->oper) || (PBAPOP_PULL_VCARD_LISTING == Parms->oper) || 
					 (PBAPOP_PULL_VCARD_ENTRY == Parms->oper) )
				{
					Parms->oper = PBAPOP_PULL;
				}
				/* Need to reset the Entry name before the next pull request*/
				BtlPbapResetEntryName();
				pbapsContext->state = BTL_PBAPS_STATE_CONNECTED;
	            break;
				/* Pass event to app. */
		    }
		    default:
			{
		        BTL_ERR(BT_STATUS_INTERNAL_ERROR, ("Server: PBAP Unexpected event"));

		    }
		}
	}

	if (TRUE == passEventToApp)
	{
		/* Set the context in the event passed to app */
		btlPbapsData.event.pbapsContext = pbapsContext;

		/* Set the event passed to app */
		btlPbapsData.event.callbackParms = Parms;
		
		/* Pass the event to app */
		pbapsContext->callback(&btlPbapsData.event);
	}

	if (TRUE == sendDisableEvent)
	{
		Parms->event = PBAP_EVENT_DISABLED;
		
		/* Pass the event to app */
		pbapsContext->callback(&btlPbapsData.event);
	}

	if (TRUE == notifyRadioOffComplete)
	{	
		BtlModuleNotificationCompletionEvent moduleCompletionEvent;
				
		moduleCompletionEvent.type = BTL_MODULE_NOTIFICATION_COMPLETION_EVENT_TYPE_RADIO_OFF;
		moduleCompletionEvent.status = BT_STATUS_SUCCESS;
		
		BTL_ModuleCompleted(BTL_MODULE_TYPE_PBAPS, &moduleCompletionEvent);
	}
	

	BTL_FUNC_END();
}

/*---------------------------------------------------------------------------
 *            PbapOpName
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Return a pointer to the name of the current server operation.
 *
 * Return:    ASCII String pointer.
 *
 */
static const char *BtlPbapsOpName(PbapOp Op)
{
    switch (Op) {
    case PBAPOP_NONE:
        return "None";
    case PBAPOP_CONNECT:
        return "Connect";
    case PBAPOP_DISCONNECT:
        return "Disconnect";
    case PBAPOP_PULL:
        return "Pull";
    case PBAPOP_PULL_PHONEBOOK:
        return "Pull Phonebook";
    case PBAPOP_SET_PHONEBOOK:
        return "Set Phonebook";
    case PBAPOP_PULL_VCARD_LISTING:
        return "Pull vCard Listing";
    case PBAPOP_PULL_VCARD_ENTRY:
        return "Pull vCard Entry";
    }
    return "UNKNOWN";
}


BtStatus BtlPbapsBtlNotificationsCb(BtlModuleNotificationType notificationType)
{
	BtStatus 		 status = BT_STATUS_SUCCESS;
	ObStatus    	 obStatus = OB_STATUS_SUCCESS;
	BtlPbapsContext *pbapsContext;
	BtlContext	    *base;

	BTL_FUNC_START("BtlPbapsBtlNotificationsCb");
	
	BTL_LOG_DEBUG(("Notification Type: %d", notificationType));
	
	switch (notificationType)
	{
		case BTL_MODULE_NOTIFICATION_RADIO_ON:

			/* Do Nothing */
		
			break;

		case BTL_MODULE_NOTIFICATION_RADIO_OFF:

			/* Disconnect all connections , stop all connections in establishment, and any other process in progress 
				in all contexts
			*/

			IterateList(btlPbapsData.contextsList, base, BtlContext *)
			{
				pbapsContext = (BtlPbapsContext *)base;

				switch (pbapsContext->state)
				{
					case (BTL_PBAPS_STATE_DISCONNECTING):
					{
						/* PBAPS state is now in the process of disabling */
						pbapsContext->state = BTL_PBAPS_STATE_DISABLING;
						break;
					}
					case (BTL_PBAPS_STATE_OPER_ON_GOING):
						/* Abort the phonebook and OBEX operation */
						BtlPbapsPbAbortOperation(FALSE);
						obStatus = PBAP_ServerAbort(&pbapsContext->server, PBRC_SERVICE_UNAVAILABLE);
						BTL_VERIFY_ERR_NO_RETVAR((OB_STATUS_SUCCESS == obStatus), ("Server abort falied"));
						
						/* Drop into next case */
					case (BTL_PBAPS_STATE_CONNECTED):
					{							
						obStatus = PBAP_ServerTpDisconnect(&pbapsContext->server);
						BTL_VERIFY_ERR((OB_STATUS_PENDING == obStatus), BT_STATUS_FAILED, ("PBAPS is disconnect failed"));
						
						if (OB_STATUS_PENDING == obStatus)
						{
							/* PBAPS state is now in the process of disabling */
							pbapsContext->state = BTL_PBAPS_STATE_DISABLING;
							BTL_LOG_INFO(("BTL_PBAPS: pbapsContext->state = BTL_PBAPS_STATE_DISABLING;")); /* reomve */
							break;
						}
						
						break;
					}
				}
				if (pbapsContext->state == BTL_PBAPS_STATE_DISABLING)
				{
					pbapsContext->disableState |= BTL_PBAPS_DISABLE_STATE_MASK_RADIO_OFF_ACTIVE;
					status = BT_STATUS_PENDING;
				}
				
			}/*IterateList(btlPbapsData.contextsList, base, BtlContext *)*/
		
			break; /*case BTL_MODULE_NOTIFICATION_RADIO_OFF:*/

			


		default:

			BTL_ERR(BT_STATUS_INTERNAL_ERROR, ("Invalid notification (%d)", notificationType));
			


	};

	BTL_FUNC_END();
	
	return status;
}

#else /* BTL_CONFIG_PBAPS ==   BTL_CONFIG_ENABLED*/

/*-------------------------------------------------------------------------------
 * BTL_PBAPS_Init() - When BTL_CONFIG_PBAPS  disabled.
 */
BtStatus BTL_PBAPS_Init(void)
{
    
   BTL_LOG_INFO(("BTL_PBAPS_Init() -  BTL_CONFIG_PBAPS Disabled"));
  

    return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 * BTL_PBAPS_Deinit() - When BTL_CONFIG_PBAPS  disabled.
 */
BtStatus BTL_PBAPS_Deinit(void)
{
    BTL_LOG_INFO(("BTL_PBAPS_Deinit() -  BTL_CONFIG_PBAPS Disabled"));

    return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------*/



#endif /* BTL_CONFIG_PBAPS ==   BTL_CONFIG_ENABLED*/




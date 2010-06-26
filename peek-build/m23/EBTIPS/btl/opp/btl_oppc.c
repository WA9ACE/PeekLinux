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
*   FILE NAME:     	btl_oppc.c
*
*   DESCRIPTION:	This file contains the BTL OPP Client role implementation.
*
*   AUTHOR:        	Alexander Udler
*
\*******************************************************************************/


#include "btl_config.h"
#include "btl_log.h"
#include "btl_defs.h"
#include "btl_oppc.h"

BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_OPPC);

#if BTL_CONFIG_OPP == BTL_CONFIG_ENABLED


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/

#include "btl_pool.h"
#include "obstore.h"
#include "btl_commoni.h"
#include "config.h"
#include "goep.h"
#include "osapi.h"
#include "utils.h"
#include "obex.h"

#include "medev.h"
#include "btl_obex_utils.h"


#define OPP_PTS_FIX

/********************************************************************************
 *
 * Constants 
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BSL_OPPC_DEFAULT 
 *
 *     The default security level for OPPC.
 */
#define BSL_OPPC_DEFAULT  (BSL_AUTHENTICATION_OUT)


#define BTL_OPPC_CHECK_FS_PATH(object)                                                \
          (OS_StrLenUtf8(object->location.fsLocation.fsPath) <                   \
              (BTHAL_FS_MAX_PATH_LENGTH + BTHAL_FS_MAX_FILE_NAME_LENGTH))

 /********************************************************************************
 *
 * Internal Types
 *
 *******************************************************************************/


/*-------------------------------------------------------------------------------
 * BtlOppcInitState type
 *
 *     Defines the OPPC init state.
 */
typedef U8 BtlOppcInitState;

#define BTL_OPPC_INIT_STATE_NOT_INTIALIZED              (0x00)
#define BTL_OPPC_INIT_STATE_INITIALIZED                 (0x01)
#define BTL_OPPC_INIT_STATE_INITIALIZATION_FAILED       (0x02)
#define BTL_OPPC_INIT_STATE_DEINITIALIZATION_FAILED     (0x03)


/*-------------------------------------------------------------------------------
 * BtlOppcState type
 *
 *     Defines the OPPC state of a specific context.
 */
typedef U8 BtlOppcState;

#define BTL_OPPC_STATE_DISABLED                         (0x00)
#define BTL_OPPC_STATE_DISCONNECTED                     (0x01)
#define BTL_OPPC_STATE_CONNECTED                        (0x02)
#define BTL_OPPC_STATE_DISCONNECTING                    (0x03)
#define BTL_OPPC_STATE_DISABLING                        (0x04)
#define BTL_OPPC_STATE_CONNECTING                       (0x05)


/*-------------------------------------------------------------------------------
 * BtlOppcDisableStateMask type
 *
 *     Defines the OPPC disable state mask.
 */
typedef U8 BtlOppcDisableStateMask;

#define BTL_OPPC_DISABLE_STATE_MASK_NONE                (0x00)
#define BTL_OPPC_DISABLE_STATE_MASK_DISABLE_ACTIVE      (0x01)
#define BTL_OPPC_DISABLE_STATE_MASK_RADIO_OFF_ACTIVE    (0x02)


 /*-------------------------------------------------------------------------------
 * BtlOppcCurrEncOper type
 *
 *     Defines the OPPC current encapsulated operation of of 
 *     a specific context.
 */
typedef U8 BtlOppcCurrEncOper;

#define BTL_OPPC_OPER_ENC_NONE                          (0x00)
#define BTL_OPPC_OPER_ENC_CON_PUSH_DISCON               (0x01)
#define BTL_OPPC_OPER_ENC_CON_PULL_DISCON               (0x02)
#define BTL_OPPC_OPER_ENC_CON_EXCHANGE_DISCON           (0x03)


/*-------------------------------------------------------------------------------
 * BtlOppcCurrOper type
 *
 *     Defines the OPPC current operation of a specific context.
 */
typedef U8 BtlOppcCurrOper;

#define BTL_OPPC_OPER_BASIC_NONE                        (0x00)
#define BTL_OPPC_OPER_BASIC_PUSH                        (0x01)
#define BTL_OPPC_OPER_BASIC_PULL                        (0x02)
#define BTL_OPPC_OPER_BASIC_EXCHANGE                    (0x03)

/*-------------------------------------------------------------------------------
 * BtlOppcOpStateMask type
 *
 *     Defines the OPPC operation state mask.
 */
typedef U8 BtlOppcOpStateMask;

#define BTL_OPPC_OP_STATE_MASK_NONE                     (0x00)
#define BTL_OPPC_OP_STATE_MASK_PULL_SUCCESS             (0x01)
#define BTL_OPPC_OP_STATE_MASK_PUSH_SUCCESS             (0x02)



/********************************************************************************
 *
 * Internal Data Structures
 *
 *******************************************************************************/


 /*-------------------------------------------------------------------------------
 * BtlOppcContext structure
 *
 *     Represents BTL OPPC context.
 */
struct _BtlOppcContext 
{
	/* Must be first field */
	BtlContext  base;

	/* Internal OPPC channel */
	GoepClientApp   client;

	/* Associated callback with this context */
	BtlOppcCallBack callback;

	/* Current OPPC state of the context */
	BtlOppcState    state;

	/* Current BTL Basic operation */
	BtlOppcCurrOper currBasicOper;

	/* Current BTL encapsulate operation */
	BtlOppcCurrEncOper  currEncapOper;

	/* Next object to be pulled in encapsulated operation */
	OppObStoreHandle   nextObjToPull;

	/* Push or Pull Request parameters */
	GoepObjectReq   objReqParams;
    
	/* is pull object initialized? */
	BTHAL_BOOL isPullObjInitialized;

	/* is push object initialized? */
	BTHAL_BOOL isPushObjInitialized;

	/* This flag indicates the push/pull success state */
	BtlOppcOpStateMask opState;
	
	/* This flag indicates the disable state */
	BtlOppcDisableStateMask disableState;

#if BTL_CONFIG_OPPC_PUSH_PULL_TIMER > 0
	EvmTimer obexOperTimer;
	BOOL    isObexTimerstarted;
#endif

};


/*-------------------------------------------------------------------------------
 * BtlOppcData structure
 *
 *     Represents the data of th BTL OPPC module.
 */
struct _BtlOppcData
{
	/* Pool of OPPC contexts */
	BTL_POOL_DECLARE_POOL(contextsPool, contextsMemory, BTL_CONFIG_OPPC_MAX_NUM_OF_CONTEXTS, sizeof(BtlOppcContext));

	/* List of active OPPC contexts */
	ListEntry   contextsList;

	/* Event passed to the application */
	BtlOppcEvent    event;

	/* Internal event generated in BTL_OPPC layer */
	OppClientEvent  oppcEvent;

};


typedef struct _BtlOppcData BtlOppcData;


/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/

/*---------------------------------------------------------------------------
 *
 * Object Store Function call table
 */
static const ObStoreFuncTable oppcFuncTable = {
    OBSTORE_Read, OBSTORE_Write, OBSTORE_GetObjectLen
};


/*-------------------------------------------------------------------------------
 * btlOppcInitState
 *
 *     Represents the current init state of OPPC module.
 */
static BtlOppcInitState btlOppcInitState = BTL_OPPC_INIT_STATE_NOT_INTIALIZED;


/*-------------------------------------------------------------------------------
 * btlOppcData
 *
 *     Represents the data of OPPC module.
 */
static BtlOppcData btlOppcData;


/*-------------------------------------------------------------------------------
 * btlOppcContextsPoolName
 *
 *     Represents the name of the OPPC contexts pool.
 */
static const char btlOppcContextsPoolName[] = "OppcContexts";


/********************************************************************************
 *
 * Internal function prototypes
 *
 *******************************************************************************/
static void     BtlOppcSetInitState(BtlOppcContext *oppcContext);
static BtStatus BtlOppcDisable(BtlOppcContext *oppcContext);
static void     BtlOppcCallback(GoepClientEvent *Event);
static BtStatus BtlOppcBuildPushReq(BtlOppcContext *oppcContext, const BtlObject *objToPush);
static BtStatus BtlOppcBuildPullReq(BtlOppcContext *oppcContext, const BtlObject *objToPull);
static void     BtlOppcObstoreCallback(OppObStoreEntry *obs);
static const char *BtlOppcObexHeaderType(ObexHeaderType type);
static const char *BtlOppcAbortMsg(ObexRespCode Reason);
static const char *BtlOppcGoepOpName(GoepOperation Op);
static BtStatus BtlOppcBtlNotificationsCb(BtlModuleNotificationType notificationType);
static BtStatus BtlOppcPush(BtlOppcContext *oppcContext, const BtlObject *objToPush);
static BtStatus BtlOppcPull(BtlOppcContext *oppcContext, const BtlObject *objToPull);
static BtStatus BtlOppcConnect(BtlOppcContext *oppcContext, const BD_ADDR *bdAddr);   
static BtStatus BtlOppcDisconnect(BtlOppcContext *oppcContext);                        
static void BtlOppcHandleAbort(BtlOppcContext *oppcContext, GoepClientEvent *Event);
static void BtlOppcCloseObjHandle(BtlOppcContext *oppcContext);
static void BtlOppcTimeoutHandler(EvmTimer *timer);



/********************************************************************************
 *
 * Function definitions
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BTL_OPPC_Init()
 *
 *      Init the OPPC module.
 */
BtStatus BTL_OPPC_Init(void)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_COMMON("BTL_OPPC_Init");
	
	BTL_VERIFY_ERR((BTL_OPPC_INIT_STATE_NOT_INTIALIZED == btlOppcInitState), 
		BT_STATUS_FAILED, ("OPPC module is already initialized"));
	
	btlOppcInitState = BTL_OPPC_INIT_STATE_INITIALIZATION_FAILED;

	status = BTL_POOL_Create(&btlOppcData.contextsPool,
							btlOppcContextsPoolName,
							btlOppcData.contextsMemory, 
							BTL_CONFIG_OPPC_MAX_NUM_OF_CONTEXTS,
							sizeof(BtlOppcContext));

	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("OPPC contexts pool creation failed"));
	
	InitializeListHead(&btlOppcData.contextsList);
		
	BTL_RegisterForCommonNotifications(BTL_MODULE_TYPE_OPPC, BtlOppcBtlNotificationsCb);
	
	btlOppcInitState = BTL_OPPC_INIT_STATE_INITIALIZED;

	BTL_FUNC_END_AND_UNLOCK();
	
	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_OPPC_Deinit()
 *
 *      Deinit the OPPC module.
 */
BtStatus BTL_OPPC_Deinit(void)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_COMMON("BTL_OPPC_Deinit");

	BTL_DeRegisterFromCommonNotifications(BTL_MODULE_TYPE_OPPC);
	
	BTL_VERIFY_ERR((BTL_OPPC_INIT_STATE_INITIALIZED == btlOppcInitState), BT_STATUS_FAILED, 
        ("OPPC module is not initialized"));

	btlOppcInitState = BTL_OPPC_INIT_STATE_DEINITIALIZATION_FAILED;

	BTL_VERIFY_ERR((IsListEmpty(&btlOppcData.contextsList)), BT_STATUS_FAILED, 
        ("OPPC contexts are still active"));

	status = BTL_POOL_Destroy(&btlOppcData.contextsPool);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, 
        ("OPPC contexts pool destruction failed"));
		
	btlOppcInitState = BTL_OPPC_INIT_STATE_NOT_INTIALIZED;

	BTL_FUNC_END_AND_UNLOCK();
	
	return (status);
}

 
/*-------------------------------------------------------------------------------
 * BTL_OPPC_Create()
 *
 *      Allocates a unique OPPC context.
 */
BtStatus BTL_OPPC_Create(BtlAppHandle *appHandle,
						const BtlOppcCallBack oppcCallback,
						const BtSecurityLevel *securityLevel,
						BtlOppcContext **oppcContext)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_OPPC_Create");

	BTL_VERIFY_ERR((0 != oppcCallback), BT_STATUS_INVALID_PARM, ("Null oppcCallback"));
	BTL_VERIFY_ERR((0 != oppcContext), BT_STATUS_INVALID_PARM, ("Null oppcContext"));

#if BT_SECURITY == XA_ENABLED

	if (0 != securityLevel)
	{
		status = BTL_VerifySecurityLevel(*securityLevel);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, 
            ("Invalid OPPC securityLevel"));
	}

#endif	/* BT_SECURITY == XA_ENABLED */

	/* Allocate memory for a unique new OPPC context */
	status = BTL_POOL_Allocate(&btlOppcData.contextsPool, (void **)oppcContext);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed allocating OPPC context"));

	status = BTL_HandleModuleInstanceCreation(appHandle, BTL_MODULE_TYPE_OPPC, 
                                                &(*oppcContext)->base);

	if (BT_STATUS_SUCCESS != status)
	{
		/* oppcContext must be freed before we exit */
		BtStatus retVal = BTL_POOL_Free(&btlOppcData.contextsPool, (void **)oppcContext);
		BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == retVal), ("Failed freeing OPPC context"));

		/* This verify will fail and will handle the exception gracefully */
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, 
            ("Failed handling OPPC instance creation"));
	}

	/* Save the given callback */
	(*oppcContext)->callback = oppcCallback;

	/* Add the new OPPC context to the active contexts list */
	InsertTailList(&btlOppcData.contextsList, &((*oppcContext)->base.node));

#if BT_SECURITY == XA_ENABLED

	/* Save the given security level, or use default */
	(*oppcContext)->client.secRecord.level = (BtSecurityLevel)((securityLevel == 0) ? 
		(BSL_OPPC_DEFAULT) : (*securityLevel));

#endif	/* BT_SECURITY == XA_ENABLED */

	/* Init OPPC context state */
	(*oppcContext)->state = BTL_OPPC_STATE_DISABLED;

	(*oppcContext)->disableState = BTL_OPPC_DISABLE_STATE_MASK_NONE;

	BtlOppcSetInitState(*oppcContext);
    
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_OPPC_Destroy()
 *
 *      Releases a OPPC context (previously allocated with BTL_OPPC_Create).
 */
BtStatus BTL_OPPC_Destroy(BtlOppcContext **oppcContext)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BOOL isAllocated = FALSE;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_OPPC_Destroy");

	BTL_VERIFY_ERR((0 != oppcContext), BT_STATUS_INVALID_PARM, 
        ("Null oppcContext"));

	BTL_VERIFY_ERR((0 != *oppcContext), BT_STATUS_INVALID_PARM, 
        ("Null *oppcContext"));

	BTL_VERIFY_ERR((BTL_OPPC_STATE_DISABLED == (*oppcContext)->state), 
        BT_STATUS_IN_USE, ("OPPC context is in use"));

	status = BTL_POOL_IsElelementAllocated(&btlOppcData.contextsPool, 
        *oppcContext, &isAllocated);

	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, 
        ("Failed locating given OPPC context"));

	BTL_VERIFY_ERR((TRUE == isAllocated), BT_STATUS_INVALID_PARM, 
        ("Invalid OPPC context"));

	/* Remove the context from the list of all OPPC contexts */
	RemoveEntryList(&((*oppcContext)->base.node));

	status = BTL_HandleModuleInstanceDestruction(&(*oppcContext)->base);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, 
        ("Failed handling OPPC instance destruction"));

	status = BTL_POOL_Free(&btlOppcData.contextsPool, (void **)oppcContext);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, 
        ("Failed freeing OPPC context"));

	/* Set the OPPC context to NULL */
	*oppcContext = 0;

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_OPPC_Enable()
 *
 *      Enable OPPC, called after BTL_OPPC_Create.
 *      After calling this function, OPP client is ready for usage.
 */
BtStatus BTL_OPPC_Enable(BtlOppcContext *oppcContext)
{
	BtStatus status = BT_STATUS_SUCCESS;
	ObStatus obStatus;
	BOOL retVal;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_OPPC_Enable");

	BTL_VERIFY_ERR((0 != oppcContext), BT_STATUS_INVALID_PARM, 
                        ("Null oppcContext"));

	BTL_VERIFY_ERR((BTL_OPPC_STATE_DISABLED == oppcContext->state), 
                        BT_STATUS_FAILED, ("OPPC context is already enabled"));

	/* Setup callback, Object Store function table and register OPP client */
	oppcContext->client.callback = BtlOppcCallback;
	oppcContext->client.connFlags = GOEP_NEW_CONN;
	obStatus = OPUSH_RegisterClient(&oppcContext->client, &oppcFuncTable);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == obStatus), BT_STATUS_FAILED, 
        ("Failed to register OPP client"));

#if BT_SECURITY == XA_ENABLED

	/* Register security record */
	status = GOEP_RegisterClientSecurityRecord(&oppcContext->client, 
	oppcContext->client.secRecord.level);

	if (BT_STATUS_SUCCESS != status)
	{
		obStatus = OPUSH_DeregisterClient(&oppcContext->client);
		BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == obStatus), 
            ("Failed deregistering OPP client"));

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
		status = GOEP_UnregisterClientSecurityRecord(&oppcContext->client);
		BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == status), 
			("Failed unregistering OPPC security record"));
			
#endif	/* BT_SECURITY == XA_ENABLED */

		/* Second, try to deregister client */
		obStatus = OPUSH_DeregisterClient(&oppcContext->client);
		BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == obStatus), 
			("Failed deregistering OPP client"));

		/* This verify will fail and will handle the exception gracefully */
		BTL_VERIFY_ERR((TRUE == retVal), BT_STATUS_FAILED, 
			("Object Store initialization failed!"));
	}

	/* OPPC state is now enabled */
	oppcContext->state = BTL_OPPC_STATE_DISCONNECTED;

#if BTL_CONFIG_OPPC_PUSH_PULL_TIMER > 0
	oppcContext->isObexTimerstarted = FALSE;
#endif
	

	BTL_LOG_INFO(("Client is enabled."));

	BTL_FUNC_END_AND_UNLOCK();
	
	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_OPPC_Disable()
 *
 *      Disable OPPC, called before BTL_OPPC_Destroy.
 *      If a connection exists, it will be disconnected automatically.
 */
BtStatus BTL_OPPC_Disable(BtlOppcContext *oppcContext)
{
	BtStatus status = BT_STATUS_PENDING;
	ObStatus obStatus;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_OPPC_Disable");

	BTL_VERIFY_ERR((0 != oppcContext), BT_STATUS_INVALID_PARM, 
                        ("Null oppcContext"));

	BTL_VERIFY_ERR((BTL_OPPC_STATE_DISABLED != oppcContext->state), 
        BT_STATUS_FAILED, ("OPPC context is not enabled"));

	if (oppcContext->state == BTL_OPPC_STATE_DISABLING)
	{
		oppcContext->disableState |= BTL_OPPC_DISABLE_STATE_MASK_DISABLE_ACTIVE;

		BTL_RET(BT_STATUS_PENDING);
	}

	switch (oppcContext->state)
	{
		case (BTL_OPPC_STATE_DISCONNECTING):
		{
			/* OPPC state is now in the process of disabling */
			oppcContext->state = BTL_OPPC_STATE_DISABLING;
			break;
		}
		
		case (BTL_OPPC_STATE_CONNECTING):
		{
			/* In process of connecting, let it finish and then close the channel */

			/* OPPC state is now in the process of disabling */
			oppcContext->state = BTL_OPPC_STATE_DISABLING;
			break;
		}

		case (BTL_OPPC_STATE_CONNECTED):
		{
			obStatus = GOEP_TpDisconnect(&oppcContext->client);

			BTL_VERIFY_ERR((OB_STATUS_BUSY != obStatus), BT_STATUS_BUSY, 
				("OPPC is currently executing an operation"));
			
			if (OB_STATUS_PENDING == status)
			{
				/* OPPC state is now in the process of disabling */
				oppcContext->state = BTL_OPPC_STATE_DISABLING;
				break;
			}
			
			/* No connection, try to deregister. Pass through to next case... */
		}

		case (BTL_OPPC_STATE_DISCONNECTED):
		{
			break;
		}
		
		default:
		{
			/* This verify will fail and will handle the exception gracefully */
			BTL_VERIFY_ERR((BTL_OPPC_STATE_DISCONNECTED == oppcContext->state), 
                        BT_STATUS_FAILED, ("Disable failed, invalid OPPC state"));
			break;
		}
	}

	if (BTL_OPPC_STATE_DISABLING != oppcContext->state)
	{
		status = BtlOppcDisable(oppcContext);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed disabling OPPC context"));
	}
	else
	{
		oppcContext->disableState |= BTL_OPPC_DISABLE_STATE_MASK_DISABLE_ACTIVE;
	}

	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_OPPC_Connect()
 *
 *      This function is used by the client to create a transport connection
 *     	to the specified device and issue an OBEX Connect Request.
 */
BtStatus BTL_OPPC_Connect(BtlOppcContext *oppcContext,
                            const BD_ADDR *bdAddr)
{
	BtStatus status;
	
	BTL_FUNC_START_AND_LOCK("BTL_OPPC_Connect");

	status = BtlOppcConnect(oppcContext, bdAddr);

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_OPPC_Disconnect()
 *
 *      This function is used by the client to issue an OBEX Disconnect Request.
 *     	When the OBEX Disconnect is completed, the transport connection will
 *     	be disconnected automatically.
 */
BtStatus BTL_OPPC_Disconnect(BtlOppcContext *oppcContext)
{
	BtStatus status;
		
	BTL_FUNC_START_AND_LOCK("BTL_OPPC_Disconnect");

	status = BtlOppcDisconnect(oppcContext);
    
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_OPPC_GetConnectedDevice()
 *
 *      This function returns the connected device.
 */
BtStatus BTL_OPPC_GetConnectedDevice(BtlOppcContext *oppcContext, 
                                    BD_ADDR *bdAddr)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BOOL retVal;
	ObexTpConnInfo tpInfo;
	
	BTL_FUNC_START_AND_LOCK("BTL_OPPC_GetConnectedDevice");

	BTL_VERIFY_ERR((0 != oppcContext), BT_STATUS_INVALID_PARM, 
        ("Null oppcContext"));

	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, 
        ("Null bdAddr"));

	BTL_VERIFY_ERR((BTL_OPPC_STATE_CONNECTED == oppcContext->state), 
        BT_STATUS_NO_CONNECTION, ("OPPC context is not connected"));

	tpInfo.size = sizeof(ObexTpConnInfo);
	tpInfo.tpType = OBEX_TP_BLUETOOTH;
	tpInfo.remDev = 0;

	retVal = GOEP_ClientGetTpConnInfo(&oppcContext->client, &tpInfo);
	BTL_VERIFY_ERR((TRUE == retVal), BT_STATUS_NO_CONNECTION, 
        ("OPPC get connection info failed"));

	BTL_VERIFY_ERR((0 != tpInfo.remDev), BT_STATUS_INTERNAL_ERROR, 
        ("Unable to find connected remote device"));
	
	OS_MemCopy(bdAddr, (&(tpInfo.remDev->bdAddr)), sizeof(BD_ADDR));

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_OPPC_Push()
 *
 *      Initiates the OBEX "Put" operation to send an object to the remote server.
 */
BtStatus BTL_OPPC_Push(BtlOppcContext *oppcContext, 
                    const BtlObject *objToPush)
{
	BtStatus status = BT_STATUS_SUCCESS;
		
	BTL_FUNC_START_AND_LOCK("BTL_OPPC_Push");

	status = BtlOppcPush(oppcContext, objToPush);
    
	BTL_VERIFY_ERR((OB_STATUS_PENDING == status), status, ("Client push falied"));

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_OPPC_Pull()
 *
 *      Initiates the OBEX "Get" operation to retrieve an object from
 *      the remote server. 
 */
BtStatus BTL_OPPC_Pull(BtlOppcContext *oppcContext, 
                        const BtlObject *objToPull)
{
	BtStatus status = BT_STATUS_SUCCESS;
		
	BTL_FUNC_START_AND_LOCK("BTL_OPPC_Pull");

	status = BtlOppcPull(oppcContext, objToPull);
	
	BTL_VERIFY_ERR((OB_STATUS_PENDING == status), status, ("Client pull falied"));

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_OPPC_Abort()
 *
 *      Aborts the current client operation. The completion event will signal
 *      the status of the operation in progress, either COMPLETE or ABORTED.
 */
BtStatus BTL_OPPC_Abort(BtlOppcContext *oppcContext)
{
	BtStatus status = BT_STATUS_SUCCESS;
	ObStatus obStatus;

	BTL_FUNC_START_AND_LOCK("BTL_OPPC_Abort");

	BTL_VERIFY_ERR((0 != oppcContext), BT_STATUS_INVALID_PARM, 
        ("Null oppcContext"));

	BTL_VERIFY_ERR((BTL_OPPC_STATE_CONNECTED == oppcContext->state), 
        BT_STATUS_NO_CONNECTION, ("OPPC context is not connected"));

	obStatus = OPUSH_ClientAbort(&oppcContext->client);
	
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == obStatus), BT_STATUS_FAILED, 
		("Client abort falied"));

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


#if BT_SECURITY == XA_ENABLED
/*-------------------------------------------------------------------------------
 * BTL_OPPC_SetSecurityLevel()
 *
 *      Sets security level for the given OPPC context.
 */
BtStatus BTL_OPPC_SetSecurityLevel(BtlOppcContext *oppcContext,
                                    const BtSecurityLevel *securityLevel)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_OPPC_SetSecurityLevel");

	BTL_VERIFY_ERR((0 != oppcContext), BT_STATUS_INVALID_PARM, ("Null oppcContext"));

	if (0 != securityLevel)
	{
		status = BTL_VerifySecurityLevel(*securityLevel);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Invalid OPPC securityLevel"));
	}

	/* Apply the new security level */
	oppcContext->client.secRecord.level = (BtSecurityLevel)((securityLevel == 0) ? 
		(BSL_OPPC_DEFAULT) : (*securityLevel));

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_OPPC_GetSecurityLevel()
 *
 *      Gets security level for the given OPPC context.
 */
BtStatus BTL_OPPC_GetSecurityLevel(BtlOppcContext *oppcContext,
                                    BtSecurityLevel *securityLevel)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_OPPC_GetSecurityLevel");

	BTL_VERIFY_ERR((0 != oppcContext), BT_STATUS_INVALID_PARM, 
        ("Null oppcContext"));

	BTL_VERIFY_ERR((0 != securityLevel), BT_STATUS_INVALID_PARM, 
        ("Null securityLevel"));

	*securityLevel = oppcContext->client.secRecord.level;
	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}
#endif	/* BT_SECURITY == XA_ENABLED */


/*-------------------------------------------------------------------------------
 * BTL_OPPC_Exchange()
 *
 *      Initiates the OBEX "Put" operation to send an object to the remote server.
 *      Afterwards, it initiates the OBEX "Get" operation to retrieve an object 
 *      from the remote server.
 */
BtStatus BTL_OPPC_Exchange(BtlOppcContext *oppcContext, 
                            const BtlObject *objToPush,
                            const BtlObject *objToPull)
{
	BtStatus status = BT_STATUS_SUCCESS;
	ObStatus ret;
	
	BTL_FUNC_START_AND_LOCK("BTL_OPPC_Exchange");

	BTL_VERIFY_ERR((0 != oppcContext), BT_STATUS_INVALID_PARM, ("Null oppcContext"));
	BTL_VERIFY_ERR((0 != objToPush), BT_STATUS_INVALID_PARM, ("Null objToPush"));
	BTL_VERIFY_ERR((0 != objToPull), BT_STATUS_INVALID_PARM, ("Null objToPull"));
	BTL_VERIFY_ERR((BTL_OPPC_STATE_CONNECTED == oppcContext->state), 
        BT_STATUS_NO_CONNECTION, ("OPPC context is not connected"));
    
	ret = BtlOppcBuildPullReq(oppcContext, objToPull);	
	
	if (BT_STATUS_FAILED == ret)
	{
		OBSTORE_Close((OppObStoreHandle *)&(oppcContext->objReqParams.object));
	}

	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == ret), BT_STATUS_INVALID_PARM, 
		("Failed to build object pull request."));

	oppcContext->nextObjToPull = oppcContext->objReqParams.object; 	
	
	/* Prepare push object */
	ret = BtlOppcBuildPushReq(oppcContext, objToPush);	

	if (BT_STATUS_FAILED == ret)
	{
		OBSTORE_Close((OppObStoreHandle *)&(oppcContext->objReqParams.object));
		OBSTORE_Delete((OppObStoreHandle *)&(oppcContext->nextObjToPull));
	}
    
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == ret), BT_STATUS_INVALID_PARM, 
            ("Failed to build object push request."));

	/* Start push operation */
	status = OPUSH_Push(&oppcContext->client, &oppcContext->objReqParams);
	
	BTL_VERIFY_ERR((OB_STATUS_PENDING == status), BT_STATUS_FAILED, 
            ("Client push falied"));

	oppcContext->currBasicOper = BTL_OPPC_OPER_BASIC_EXCHANGE;	
	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_OPPC_ConnectPushDisconnect()
 *
 *      This function is a combined operation of 3 functions: 
 *      BTL_OPPC_Connect(), BTL_OPPC_Push() and BTL_OPPC_Disconnect().
 */
BtStatus BTL_OPPC_ConnectPushDisconnect(BtlOppcContext *oppcContext, 
                                        const BD_ADDR *bdAddr, 
                                        const BtlObject *objToPush)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtStatus ret;

	BTL_FUNC_START_AND_LOCK("BTL_OPPC_ConnectPushDisconnect");

	BTL_VERIFY_ERR((0 != oppcContext), BT_STATUS_INVALID_PARM, ("Null oppcContext"));
	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));
	BTL_VERIFY_ERR((0 != objToPush), BT_STATUS_INVALID_PARM, ("Null objToPush"));
	BTL_VERIFY_ERR((BTL_OPPC_STATE_DISCONNECTED == oppcContext->state), 
       	 BT_STATUS_BUSY, ("OPPC context is busy"));
    
	ret = BtlOppcBuildPushReq(oppcContext, objToPush);
	
	if (BT_STATUS_FAILED == ret)
	{
		OBSTORE_Close((OppObStoreHandle *)&(oppcContext->objReqParams.object));
	}

	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == ret), BT_STATUS_INVALID_PARM, 
		("Failed to build object push request."));

	status = BtlOppcConnect(oppcContext, bdAddr);

	BTL_VERIFY_ERR((BT_STATUS_PENDING == status), status, 
		("Failed connecting OPP client"));

	oppcContext->currEncapOper = BTL_OPPC_OPER_ENC_CON_PUSH_DISCON;	
	
	BTL_FUNC_END_AND_UNLOCK();
	
	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_OPPC_ConnectPullDisconnect()
 *
 *      This function is a combined operation of 3 functions: 
 *      BTL_OPPC_Connect(), BTL_OPPC_Pull() and BTL_OPPC_Disconnect().
 */
BtStatus BTL_OPPC_ConnectPullDisconnect(BtlOppcContext *oppcContext, 
                                        const BD_ADDR *bdAddr, 
                                        const BtlObject *objToPull)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtStatus ret;

	BTL_FUNC_START_AND_LOCK("BTL_OPPC_ConnectPullDisconnect");

	BTL_VERIFY_ERR((0 != oppcContext), BT_STATUS_INVALID_PARM, ("Null oppcContext"));
	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));
	BTL_VERIFY_ERR((BTL_OPPC_STATE_DISCONNECTED == oppcContext->state),
		BT_STATUS_BUSY, ("OPPC context is busy"));

	ret = BtlOppcBuildPullReq(oppcContext, objToPull);	

	if (BT_STATUS_FAILED == ret)
	{
		OBSTORE_Close((OppObStoreHandle *)&(oppcContext->objReqParams.object));
	}	    	

	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == ret), BT_STATUS_INVALID_PARM, 
		("Failed to build object pull request."));

	status = BtlOppcConnect(oppcContext, bdAddr);

	BTL_VERIFY_ERR((OB_STATUS_PENDING == status), status, 
        ("Failed connecting OPP client"));

	oppcContext->currEncapOper = BTL_OPPC_OPER_ENC_CON_PULL_DISCON;	

	BTL_FUNC_END_AND_UNLOCK();
	
	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_OPPC_ConnectExchangeDisconnect()
 *
 *      This function is a combined operation of 4 functions: 
 *      BTL_OPPC_Connect(), BTL_OPPC_Push(), BTL_OPPC_Pull() and BTL_OPPC_Disconnect().
 */
BtStatus BTL_OPPC_ConnectExchangeDisconnect(BtlOppcContext *oppcContext, 
                                            const BD_ADDR *bdAddr, 
                                            const BtlObject *objToPush,
                                            const BtlObject *objToPull)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtStatus ret;


	BTL_FUNC_START_AND_LOCK("BTL_OPPC_ConnectExchangeDisconnect");

	BTL_VERIFY_ERR((0 != oppcContext), BT_STATUS_INVALID_PARM, ("Null oppcContext"));
	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));
	BTL_VERIFY_ERR((0 != objToPush), BT_STATUS_INVALID_PARM, ("Null objToPush"));
	BTL_VERIFY_ERR((0 != objToPull), BT_STATUS_INVALID_PARM, ("Null objToPull"));

	BTL_VERIFY_ERR((BTL_OPPC_STATE_DISCONNECTED == oppcContext->state), 
		BT_STATUS_BUSY, ("OPPC context is busy"));

	ret = BtlOppcBuildPullReq(oppcContext, objToPull);	

	if (BT_STATUS_FAILED == ret)
	{
		OBSTORE_Close((OppObStoreHandle *)&(oppcContext->objReqParams.object));
	}	

	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == ret), BT_STATUS_INVALID_PARM, 
		("Failed to build object pull request."));

	oppcContext->nextObjToPull = oppcContext->objReqParams.object; 	    

	ret = BtlOppcBuildPushReq(oppcContext, objToPush);	

	if (BT_STATUS_FAILED == ret)
	{
		OBSTORE_Close((OppObStoreHandle *)&(oppcContext->objReqParams.object));
		OBSTORE_Delete((OppObStoreHandle *)&(oppcContext->nextObjToPull));
	}

	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == ret), BT_STATUS_INVALID_PARM, 
		("Failed to build object push request."));
    
	status = BtlOppcConnect(oppcContext, bdAddr);

	BTL_VERIFY_ERR((BT_STATUS_PENDING == status), status, 
        ("Failed connecting OPP client"));

	oppcContext->currEncapOper = BTL_OPPC_OPER_ENC_CON_EXCHANGE_DISCON;	
	
	BTL_FUNC_END_AND_UNLOCK();
	
	return (status);
}


/*-------------------------------------------------------------------------------
 * BtlOppcDisable()
 *
 *		Internal function for deregistering security record and OPP client.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		oppcContext [in] - pointer to the OPPC context.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */
static BtStatus BtlOppcDisable(BtlOppcContext *oppcContext)
{
	ObStatus obStatus;
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START("BtlOppcDisable");

	BtlOppcCloseObjHandle(oppcContext);

	BtlOppcSetInitState(oppcContext);

#if BT_SECURITY == XA_ENABLED

	/* First, try to unregister security record */
	status = GOEP_UnregisterClientSecurityRecord(&oppcContext->client);
	BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == status), 
        ("Failed unregistering OPPC security record"));
			
#endif /* BT_SECURITY == XA_ENABLED */

	/* Second, try to deregister client */
	obStatus = OPUSH_DeregisterClient(&oppcContext->client);
	BTL_VERIFY_ERR((OB_STATUS_SUCCESS == obStatus), BT_STATUS_FAILED, 
        ("Failed deregistering OPP client"));

	/* Now deinitialize the Object Store Component */
	OBSTORE_Deinit();

	/* OPPC state is now disabled */
	oppcContext->state = BTL_OPPC_STATE_DISABLED;

	BTL_LOG_INFO(("Client is disabled."));

	BTL_FUNC_END();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BtlOppcSetInitState()
 *
 *		Function for clearing client context transaction
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		oppcContext [in] - pointer to the OPPC context.
 *
 * Returns:
 *		void
 */
static void BtlOppcSetInitState(BtlOppcContext *oppcContext)
{
	BTL_FUNC_START("BtlOppcSetInitState");

	oppcContext->currBasicOper = BTL_OPPC_OPER_BASIC_NONE;
	oppcContext->currEncapOper = BTL_OPPC_OPER_ENC_NONE;

	oppcContext->isPushObjInitialized = FALSE;
	oppcContext->isPullObjInitialized = FALSE;

	oppcContext->nextObjToPull = NULL;
	oppcContext->objReqParams.object = NULL;

	oppcContext->opState = BTL_OPPC_OP_STATE_MASK_NONE;

    BTL_FUNC_END();
}


/*---------------------------------------------------------------------------
 * BtlOppcBuildPushReq()
 *
 * Synopsis:  Creates and initializes Object Store entry which describes
 *            object that GOEP is to Push.
 *
 * Return:	BT_STATUS_FAILED - Invalid parameters, Failed to open file, or
 *               out of resources. The user is responsible to call OBSTORE_close()
 *               in case of failure.     
 *			BT_STATUS_SUCCESS - Operation started Successfully.
 */
static BtStatus BtlOppcBuildPushReq(BtlOppcContext *oppcContext,
                                    const BtlObject *objToPush)
{
	ObexRespCode    rcode = OBRC_SUCCESS;
	BtStatus        status = BT_STATUS_SUCCESS;
	OppObStoreHandle   obs;
            
	BTL_FUNC_START("BtlOppcBuildPushReq");

	/* Memory and FS Objects must provide objectName */
	BTL_VERIFY_ERR((NULL != objToPush->objectName), BT_STATUS_FAILED, ("Null objectName"));

	/* Memory and FS Objects must provide objectMimeType */
	BTL_VERIFY_ERR((NULL != objToPush->objectMimeType), BT_STATUS_FAILED, ("Null objectMimeType"));

       
	/* Create a new OBSTORE object */
	obs = OBSTORE_New();

	BTL_VERIFY_FATAL((0 != obs), BT_STATUS_FAILED, ("Failed to create obstore object."));

   	/* Saved new object */
	oppcContext->objReqParams.object = (void *)obs;

	/* Fill Object Location field */
	obs->object_location = objToPush->objectLocation;

	/* Fill Object Type field */    
	OBSTORE_AppendType(obs, (const U8 *)objToPush->objectMimeType, OS_StrLen(objToPush->objectMimeType));
    
	/* Common to both FS and Memory */
	OBSTORE_SetFileName(obs, objToPush->objectName);

	/* Fill Object Name field */
	if (objToPush->objectLocation == BTL_OBJECT_LOCATION_FS)
	{
		/* A null-terminated string representing the full path of the object in the FS.*/
		BTL_VERIFY_ERR((NULL != objToPush->location.fsLocation.fsPath), BT_STATUS_FAILED, ("Null fsPath"));

		/* Check fsPath length */
		BTL_VERIFY_ERR((TRUE == BTL_OPPC_CHECK_FS_PATH(objToPush)), BT_STATUS_FAILED, ("fsPath too long."));
		        
		/* Fill obs->fsPath and obs->nameLen fields */
		OBSTORE_AppendNameAscii(obs, objToPush->location.fsLocation.fsPath);
	}
    
	/* Fill Object Length field */
	if (objToPush->objectLocation == BTL_OBJECT_LOCATION_FS)
	{
		/* Open object, and update object->fileLen field */
		rcode = OBSTORE_Open((OppObStoreHandle)oppcContext->objReqParams.object, 
	                                        OPPC_PUSH_OPERATION);

		if (rcode != OBRC_SUCCESS) 
		{
			BTL_VERIFY_ERR((rcode == OBRC_SUCCESS), BT_STATUS_FAILED, 
				("Failed to open push file.!"));
		}
	}
	else
	{
		obs->mem_ocx = (void*)objToPush->location.memLocation.memAddr;

		/* In TX operation, this is the actual size of the transferred object in memory. */
		obs->objectSize = objToPush->location.memLocation.size;
	}


	/* Update objectLen field */
	oppcContext->objReqParams.objectLen = OBSTORE_GetObjectLen(obs);
	
	/* Update type field */
	oppcContext->objReqParams.type = (const U8 *)OBSTORE_GetTypeAscii(obs) ;

	oppcContext->objReqParams.name = (const GoepUniType *)OBSTORE_GetNameAscii(obs); 

	if (rcode == OBRC_SUCCESS)
	{
		OBSTORE_SetObstoreCallback(obs, BtlOppcObstoreCallback);
	}

	oppcContext->isPushObjInitialized = TRUE;
    
	BTL_FUNC_END();

	return (status);
}


/*---------------------------------------------------------------------------
 * BtlOppcBuildPullReq()
 *
 * Synopsis:  Creates and initializes Object Store entry which describes
 *            file that OBEX is to get.
 *
 * Return:	BT_STATUS_FAILED - Invalid parameters, Failed to create temp file, or
 *          	out of resources. The user is responsible to call OBSTORE_close()
 *              in case of failure. 
 *			BT_STATUS_SUCCESS - Operation Started Successfully.
 *
 */
static BtStatus BtlOppcBuildPullReq(BtlOppcContext *oppcContext,
                                    const BtlObject *objToPull)
{
	ObexRespCode    rcode = OBRC_SUCCESS;
	OppObStoreHandle   obs;      
	BtStatus        status = BT_STATUS_SUCCESS;
		    
	BTL_FUNC_START("BtlOppcBuildPullReq");

	/* Memory and FS Objects must provide objectMimeType */
	BTL_VERIFY_ERR((NULL != objToPull->objectMimeType), BT_STATUS_FAILED, ("Null objectMimeType"));

	/* Create a new OBSTORE object */
	obs = OBSTORE_New();

	BTL_VERIFY_FATAL((0 != obs), BT_STATUS_FAILED, ("Failed to create obstore object."));

   	/* Saved new object */
	oppcContext->objReqParams.object = (void *)obs;

	/* Fill Object Location field */
	obs->object_location = objToPull->objectLocation;

	if (objToPull->objectName)
	{
		OS_StrCpyUtf8(obs->objectName, objToPull->objectName); 
	}
    
	if (objToPull->objectMimeType)
	{
		/* Fill Object Type field */    
		OBSTORE_AppendType(obs, (const U8 *)objToPull->objectMimeType, OS_StrLen(objToPull->objectMimeType));
    
		/* This field must be present */
		oppcContext->objReqParams.type = (const U8 *)&obs->type[0];
	}

	/* This field must not be present */
	oppcContext->objReqParams.name = NULL;

	/* Fill Object Name field */
	if (objToPull->objectLocation == BTL_OBJECT_LOCATION_FS)
	{
		/* A null-terminated string representing the full path of the object in the FS.*/
		BTL_VERIFY_ERR((NULL != objToPull->location.fsLocation.fsPath), BT_STATUS_FAILED, ("Null fsPath."));
        
		/* Check fsPath length */
		BTL_VERIFY_ERR((TRUE == BTL_OPPC_CHECK_FS_PATH(objToPull)), BT_STATUS_FAILED, ("fsPath too long."));
        
		/* Fill obs->fsPath and obs->nameLen fields */
		OBSTORE_AppendNameAscii(obs, objToPull->location.fsLocation.fsPath);
	}
    
	/* Fill Object Length field */
	if (objToPull->objectLocation == BTL_OBJECT_LOCATION_FS)
	{
		rcode = OBSTORE_Create((OppObStoreHandle)oppcContext->objReqParams.object, 
                                OPPC_PULL_OPERATION);

		if (rcode != OBRC_SUCCESS) 
		{
		    BTL_VERIFY_ERR((rcode == OBRC_SUCCESS), BT_STATUS_FAILED, 
			("Failed to create tmp file."));
		}
	}
	else
	{
		/* In RX operation, this is the max available space in memory in which the object */
		obs->objectSize = objToPull->location.memLocation.size;

		obs->mem_ocx = (void*)objToPull->location.memLocation.memAddr;
	}

	if (rcode == OBRC_SUCCESS)
	{
		OBSTORE_SetObstoreCallback(obs, BtlOppcObstoreCallback);
	}

	oppcContext->isPullObjInitialized = TRUE;
    
	BTL_FUNC_END();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BtlOppcCallback()
 *
 *		Internal callback for handling OPP events in BTL layer.
 *		This function forwards events to registered application callback.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		Event [in] - Internal OPPC event.
 *
 * Returns:
 *		void.
 */
static void BtlOppcCallback(GoepClientEvent *Event)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtlOppcContext *oppcContext;
	ObexRespCode rcode;
	ObStatus obStatus;
	BtlObject voidBtlObject;
	OppObStoreHandle object;  
	BOOL passEventToApp = TRUE;
	BOOL sendDisableEvent = FALSE;
	BOOL notifyRadioOffComplete = FALSE;


	BTL_FUNC_START("BtlOppcCallback");
	
	/* Find context according to given OPP channel */
	oppcContext = ContainingRecord(Event->handler, BtlOppcContext, client);

	object = oppcContext->objReqParams.object;

	/* Setup the internal event */
	btlOppcData.oppcEvent.event = Event->event;
	btlOppcData.oppcEvent.oper = Event->oper;

	/* First handle special case of disabling */
	if (oppcContext->state == BTL_OPPC_STATE_DISABLING)
	{
		switch (Event->event)
		{
			case (GOEP_EVENT_TP_DISCONNECTED):
			{
				/* The transport layer connection has been disconnected. */
				BTL_LOG_INFO(("Transport layer connection has been disconnected."));

				BtlOppcCloseObjHandle(oppcContext);

				BtlOppcSetInitState(oppcContext);

				oppcContext->state = BTL_OPPC_STATE_DISCONNECTED;

				if (oppcContext->disableState & BTL_OPPC_DISABLE_STATE_MASK_DISABLE_ACTIVE)
				{				
					BtlOppcDisable(oppcContext);
					sendDisableEvent = TRUE;
				}

				if (oppcContext->disableState & BTL_OPPC_DISABLE_STATE_MASK_RADIO_OFF_ACTIVE)
				{
					BtlContext *base;
					BtlOppcContext *context;

					notifyRadioOffComplete = TRUE;

					IterateList(btlOppcData.contextsList, base, BtlContext *)
					{
						context = (BtlOppcContext *)base;
						if ((context->state != BTL_OPPC_STATE_DISCONNECTED) && (context->state != BTL_OPPC_STATE_DISABLED))
						{
							notifyRadioOffComplete = FALSE;			
						}
					}
				}

				oppcContext->disableState = BTL_OPPC_DISABLE_STATE_MASK_NONE;

				/* Pass event to app */  
				break;
			}

			case (GOEP_EVENT_ABORTED):
			{
				 /* Indicates that the current operation failed or aborted. */
				BTL_LOG_INFO(("BTL_OPPC: GOEP %s operation failed or aborted, \"%s\".", 
							BtlOppcGoepOpName(Event->oper), BtlOppcAbortMsg(Event->reason)));

				/* Close or delete any intermediate files */
				BtlOppcHandleAbort(oppcContext, Event);

				/* Pass event to app */
		        break;
			}
			
			case (GOEP_EVENT_COMPLETE):
			{
				if (Event->oper == GOEP_OPER_CONNECT) 
				{				
					/* A new transport layer connection is established while disabling. */
					BTL_LOG_INFO(("BTL_OPPC: Transport layer connection has come up while disabling."));
					
					obStatus = GOEP_TpDisconnect(&oppcContext->client);
					
					BTL_VERIFY_ERR((OB_STATUS_BUSY != obStatus), BT_STATUS_BUSY, 
						("OPPC is currently executing an operation"));
					
					if (OB_STATUS_PENDING == status)
					{
						/* OPPC state is now in the process of disabling */
						oppcContext->state = BTL_OPPC_STATE_DISCONNECTING;
						break;
					}
					
					/* Do not pass event to app. */
					passEventToApp = FALSE;
				}
				
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
			case GOEP_EVENT_DISCOVERY_FAILED:
        			BTL_LOG_INFO(("Client %s Discovery Failure.", 
				BtlOppcGoepOpName(Event->oper)));
        			
				oppcContext->state = BTL_OPPC_STATE_DISCONNECTED;
        			
				BtlOppcCloseObjHandle(oppcContext);
                    
				BtlOppcSetInitState(oppcContext);

				/* Pass event to app */ 
				break;

			case GOEP_EVENT_NO_SERVICE_FOUND:
        			BTL_LOG_INFO(("Client %s No OBEX Service Found.", 
				BtlOppcGoepOpName(Event->oper)));
        			oppcContext->state = BTL_OPPC_STATE_DISCONNECTED;

				BtlOppcCloseObjHandle(oppcContext);

				BtlOppcSetInitState(oppcContext);
             					
				/* Pass event to app */ 
        			break;

			case GOEP_EVENT_HEADER_RX:
        		
				/* OPPC initiated a pull request, and received header file */

				if (Event->oper == GOEP_OPER_PULL) 
				{
				    if (Event->header.type == OBEXH_NAME)	
				    {
				        /* Pulled object was opened with a default name. */
				        /* But, the pulled object will eventually be saved with this name. */
				        OBSTORE_SetFileName(object, (const BtlUtf8*)(Event->header.buffer));

				    }
				    else if (Event->header.type == OBEXH_LENGTH)
				    {
				        if (object->object_location == BTL_OBJECT_LOCATION_FS)
				        {
				            /* FS Object */

				            /* Fill object->objectSize field */
				            OBSTORE_SetObjectLen(object, Event->info.pull.objectLen);
				        }
				        else
				        {    
				            /* Memory Object */

				            if (Event->info.pull.objectLen > object->objectSize)
				            {
				                rcode = OBRC_UNAUTHORIZED;
				                obStatus = OPUSH_ClientAbort(&oppcContext->client);

				                BTL_VERIFY_ERR((BT_STATUS_SUCCESS == obStatus), BT_STATUS_FAILED, 
				                ("Client abort falied"));
				            }
				        }
				    }
				    else if(Event->header.type == OBEXH_TYPE)
				    {
				        /* Only indicate the last header indication.  
				           This prevents  reporting the same header multiple times 
				         */
				        BTL_LOG_INFO(("Client %s Receiving an OBEX header: %s", 
				            BtlOppcGoepOpName(Event->oper), 
						    BtlOppcObexHeaderType(Event->header.type)));
				    }
				}

				/* Do not pass event to app. */
				passEventToApp = FALSE;
				break;

    		case GOEP_EVENT_TP_CONNECTED:
        		
				oppcContext->state = BTL_OPPC_STATE_CONNECTED;
        		
				BTL_LOG_INFO(("Transport Layer connection has been established."));

				/* Pass event to app */ 
				break;

			case GOEP_EVENT_COMPLETE:

				/* Indicates that the operation has completed successfully. */
				BTL_LOG_INFO(("GOEP %s operation complete.", BtlOppcGoepOpName(Event->oper)));

				/* First, close obs->fp if needed */
				if (((oppcContext->currEncapOper != BTL_OPPC_OPER_ENC_NONE) ||
		                     (oppcContext->currBasicOper != BTL_OPPC_OPER_BASIC_NONE)) && 
					((Event->oper == GOEP_OPER_PUSH) || (Event->oper == GOEP_OPER_PULL)))
				{  
					
					if (Event->oper == GOEP_OPER_PUSH)
					{
						oppcContext->opState |= BTL_OPPC_OP_STATE_MASK_PUSH_SUCCESS;
					}
					else if (Event->oper == GOEP_OPER_PULL)
					{
						oppcContext->opState |= BTL_OPPC_OP_STATE_MASK_PULL_SUCCESS;
					}

					rcode = OBSTORE_Close((OppObStoreHandle *)&(oppcContext->objReqParams.object));
					BTL_VERIFY_ERR((OBRC_SUCCESS == rcode), BT_STATUS_INTERNAL_ERROR, 
						 ("Failed closing object"));
				}


				/* Now, issue the next execution operation required in the following:
				   Encapsulated operations: PUSH, PULL, EXCHANGE
				   Basic operation: EXCHANGE 

				   note: Basic operations such as PUSH or PULL are simple, and don't have
				         a next execution operation.   
				*/
                
				if (Event->oper == GOEP_OPER_CONNECT) 
				{
					/* Transport and OBEX connection are established. */
					oppcContext->state = BTL_OPPC_STATE_CONNECTED;
					
					if (oppcContext->currEncapOper == BTL_OPPC_OPER_ENC_CON_PUSH_DISCON)
					{
						status = BtlOppcPush(oppcContext, &voidBtlObject);
						BTL_VERIFY_ERR((OB_STATUS_PENDING == status), BT_STATUS_INTERNAL_ERROR,
										("Failed to Push object in CON_PUSH_DISCON operation"));
					}
					else if (oppcContext->currEncapOper == BTL_OPPC_OPER_ENC_CON_PULL_DISCON)
					{
						status = BtlOppcPull(oppcContext, &voidBtlObject);
						BTL_VERIFY_ERR((OB_STATUS_PENDING == status), BT_STATUS_INTERNAL_ERROR,
										("Failed to Pull object in CON_PULL_DISCON operation"));
					}
					else if (oppcContext->currEncapOper == BTL_OPPC_OPER_ENC_CON_EXCHANGE_DISCON)
					{
						status = BtlOppcPush(oppcContext, &voidBtlObject);
						BTL_VERIFY_ERR((OB_STATUS_PENDING == status), BT_STATUS_INTERNAL_ERROR,
										("Failed to Push object in CON_EXCHANGE_DISCON operation"));
					}
					else if (oppcContext->currBasicOper == BTL_OPPC_OPER_BASIC_EXCHANGE)
					{
						/* Do-nothing */
					}
				} 
				else if (Event->oper == GOEP_OPER_PUSH) 
				{
					if (oppcContext->currEncapOper == BTL_OPPC_OPER_ENC_CON_PUSH_DISCON)
					{
						status = BtlOppcDisconnect(oppcContext);
						BTL_VERIFY_ERR((OB_STATUS_PENDING == status), BT_STATUS_INTERNAL_ERROR,
										("Failed to Disconnect in CON_PUSH_DISCON operation"));
					}
					else if (oppcContext->currEncapOper == BTL_OPPC_OPER_ENC_CON_EXCHANGE_DISCON)
					{
						/* Retrieve next object to pull */
						oppcContext->objReqParams.object = oppcContext->nextObjToPull; 	
						oppcContext->objReqParams.name = NULL;
						oppcContext->objReqParams.type = (const U8 *)oppcContext->nextObjToPull->type;
						
						status = BtlOppcPull(oppcContext, &voidBtlObject);
						BTL_VERIFY_ERR((OB_STATUS_PENDING == status), BT_STATUS_INTERNAL_ERROR,
										("Failed to Pull object in CON_EXCHANGE_DISCON operation"));
					}
					else if (oppcContext->currBasicOper == BTL_OPPC_OPER_BASIC_EXCHANGE)
					{
						/* Retrieve next object to pull */
						oppcContext->objReqParams.object = oppcContext->nextObjToPull; 	
						oppcContext->objReqParams.name = NULL;
						oppcContext->objReqParams.type = (const U8 *)oppcContext->nextObjToPull->type;
						
						status = BtlOppcPull(oppcContext, &voidBtlObject);
						BTL_VERIFY_ERR((OB_STATUS_PENDING == status), BT_STATUS_INTERNAL_ERROR,
									("Failed to Pull object in EXCHANGE operation"));
					}
					else if (oppcContext->currEncapOper == BTL_OPPC_OPER_ENC_CON_PULL_DISCON)
					{
					    /* should not reach here */
					    BTL_FATAL_NO_RETVAR(("Invalid OPPC_PUSH Event received during Encapsulated PULL operation"));

					}
					else if (oppcContext->currBasicOper == BTL_OPPC_OPER_BASIC_PUSH)
					{
					     /* Bring client context to its init state */
					     BtlOppcSetInitState(oppcContext);
					}
				} 
				else if (Event->oper == GOEP_OPER_PULL) 
				{
					if (oppcContext->currEncapOper == BTL_OPPC_OPER_ENC_CON_PULL_DISCON)
					{
						status = BtlOppcDisconnect(oppcContext);
						BTL_VERIFY_ERR((OB_STATUS_PENDING == status), BT_STATUS_INTERNAL_ERROR,
										("Failed to Disconnect in CON_PULL_DISCON operation"));
					}
					else if (oppcContext->currEncapOper == BTL_OPPC_OPER_ENC_CON_EXCHANGE_DISCON)
					{
						status = BtlOppcDisconnect(oppcContext);
						BTL_VERIFY_ERR((OB_STATUS_PENDING == status), BT_STATUS_INTERNAL_ERROR,
										("Failed to Disconnect in CON_EXCHANGE_DISCON operation"));
					}
					else if (oppcContext->currEncapOper == BTL_OPPC_OPER_ENC_CON_PUSH_DISCON)
					{
						/* should not reach here */
						BTL_FATAL_NO_RETVAR(("Invalid OPPC_PULL Event received during Encapsulated PUSH operation"));
					}
					else if (oppcContext->currBasicOper == BTL_OPPC_OPER_BASIC_EXCHANGE)
					{
						BtlOppcSetInitState(oppcContext);
					}
					else if (oppcContext->currBasicOper == BTL_OPPC_OPER_BASIC_PULL)
					{

						BtlOppcSetInitState(oppcContext);
					}
				} 
				
				/* Pass event to app */ 
				break;					

			case GOEP_EVENT_TP_DISCONNECTED:
				
				/* The transport layer connection has been disconnected. */
				BTL_LOG_INFO(("Transport layer connection has been disconnected."));
				
				BtlOppcCloseObjHandle(oppcContext);
                
				BtlOppcSetInitState(oppcContext);

				oppcContext->state = BTL_OPPC_STATE_DISCONNECTED;

#if BTL_CONFIG_OPPC_PUSH_PULL_TIMER > 0
    
				if (TRUE == oppcContext->isObexTimerstarted)
				{
					BTL_OBEX_UTILS_CancelTimer(&oppcContext->obexOperTimer);
					oppcContext->isObexTimerstarted = FALSE;
				}

#endif /* BTL_CONFIG_OPPC_PUSH_PULL_TIMER > 0 */

				/* Pass event to app */ 
				break;

			case GOEP_EVENT_ABORTED:
				
				/* Indicates that the current operation failed or aborted. */
				BTL_LOG_INFO(("GOEP %s operation failed or aborted, \"%s\".", 
					BtlOppcGoepOpName(Event->oper), BtlOppcAbortMsg(Event->reason)));

				/* First, close/delete intermediate files */
				BtlOppcHandleAbort(oppcContext, Event);

				/* Second, close the connection (if necessary) */
				if ( ((oppcContext->currEncapOper != BTL_OPPC_OPER_ENC_NONE) &&
				   ((Event->oper == GOEP_OPER_PUSH) || (Event->oper == GOEP_OPER_PULL))) ) 
				{

#ifdef OPP_PTS_FIX
					if (!
						((Event->oper == GOEP_OPER_PUSH)&&
					 	(oppcContext->currEncapOper == BTL_OPPC_OPER_ENC_CON_EXCHANGE_DISCON))
						)
					{

						status = BtlOppcDisconnect(oppcContext);
						BTL_VERIFY_ERR((OB_STATUS_PENDING == status), BT_STATUS_INTERNAL_ERROR,
						("Failed to Disconnect afret Aborting "));
					}
#else //OPP_PTS_FIX
					status = BtlOppcDisconnect(oppcContext);
					BTL_VERIFY_ERR((OB_STATUS_PENDING == status), BT_STATUS_INTERNAL_ERROR, 
						("Failed to Disconnect afret Aborting "));
#endif //OPP_PTS_FIX
				}
				else
				{
					/* Bring client context to its init state */
#ifdef OPP_PTS_FIX
					//reset only if its not a case of abort of push as part of an exchange
					if (!
						((Event->oper == GOEP_OPER_PUSH)&&

						((oppcContext->currEncapOper == BTL_OPPC_OPER_ENC_CON_EXCHANGE_DISCON)||
						(oppcContext->currBasicOper == BTL_OPPC_OPER_BASIC_EXCHANGE))
						))
					{
					BtlOppcSetInitState(oppcContext);
				}

#else //OPP_PTS_FIX
					BtlOppcSetInitState(oppcContext);
#endif //OPP_PTS_FIX
				}

				/* Pass event to app */ 
				break;

			case GOEP_EVENT_CONTINUE:
				
				/* During a Pull operation, keep looking for the server
				   to send back the file length information. */
				if (Event->oper == GOEP_OPER_PULL) 
				{
					if (Event->info.pull.objectLen) 
					{
						OBSTORE_SetObjectLen((OppObStoreHandle)oppcContext->objReqParams.object, 
							Event->info.pull.objectLen);
					}
				}
				
				/* Always call continue to keep the requests flowing. */
				obStatus = OPUSH_ClientContinue(Event->handler);
				BTL_VERIFY_ERR((BT_STATUS_SUCCESS == obStatus), BT_STATUS_INTERNAL_ERROR, 
												("The client is not expecting a continue"));
				
				/* Do not pass event to app. */
				passEventToApp = FALSE;
				break;

			default:
				BTL_LOG_INFO(("Client %s Unexpected event: %i", Event->event));
			
				/* Do not pass event to app. */
				passEventToApp = FALSE;
				break;
		
		} /* switch end */		
	}

	if (TRUE == passEventToApp)
	{
		/* Set the context in the event passed to app */
		btlOppcData.event.oppcContext = oppcContext;

		/* Set the internal event in the event passed to app */
		btlOppcData.event.oppcEvent = &btlOppcData.oppcEvent;

		/* Pass the event to app */
		oppcContext->callback(&btlOppcData.event);
	}

	if (TRUE == sendDisableEvent)
	{
		/* Change the event to indicate context was disabled */
        btlOppcData.event.oppcEvent->event = OPPC_EVENT_DISABLED;
		
		/* Pass the event to app */
		oppcContext->callback(&btlOppcData.event);
	}

	if (TRUE == notifyRadioOffComplete)
	{
		BtlModuleNotificationCompletionEvent moduleCompletionEvent;
		moduleCompletionEvent.type = BTL_MODULE_NOTIFICATION_COMPLETION_EVENT_TYPE_RADIO_OFF;
		moduleCompletionEvent.status = BT_STATUS_SUCCESS;
		
		BTL_ModuleCompleted(BTL_MODULE_TYPE_OPPC, &moduleCompletionEvent);
	}

	BTL_FUNC_END(); 
}


/*-------------------------------------------------------------------------------
 * BtlOppcObstoreCallback()
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
static void BtlOppcObstoreCallback(OppObStoreEntry *obs)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtlContext *context;
	BtlOppcContext *oppcContext = NULL;

	BTL_FUNC_START("BtlOppcObstoreCallback");

	/* Find OPPC context according to given OBSTORE object */
	IterateList(btlOppcData.contextsList, context, BtlContext *)
	{
		oppcContext = (BtlOppcContext *)context;
		if (oppcContext->objReqParams.object == obs)
		{
			break;
		}
	}

	BTL_VERIFY_ERR((oppcContext->objReqParams.object == obs), BT_STATUS_FAILED, 
        ("Failed to find OPPC context in progress callback"));


#if BTL_CONFIG_OPPC_PUSH_PULL_TIMER > 0
    
	if (TRUE == oppcContext->isObexTimerstarted)
	{
		BTL_OBEX_UTILS_CancelTimer(&oppcContext->obexOperTimer);
		oppcContext->isObexTimerstarted = FALSE;
	}

#endif /* BTL_CONFIG_OPPC_PUSH_PULL_TIMER > 0 */

	/* Setup the internal event */
	btlOppcData.oppcEvent.event = OPPC_EVENT_OBJECT_PROGRESS;
	btlOppcData.oppcEvent.info.progress.name = obs->objectName;
	btlOppcData.oppcEvent.info.progress.nameLen = obs->nameLen;
	btlOppcData.oppcEvent.info.progress.currPos = obs->amount;
	btlOppcData.oppcEvent.info.progress.maxPos = obs->objectSize;

	/* Set the context in the event passed to app */
	btlOppcData.event.oppcContext = oppcContext;

	/* Set the internal event in the event passed to app */
	btlOppcData.event.oppcEvent = &btlOppcData.oppcEvent;

	/* Pass the event to app */
	oppcContext->callback(&btlOppcData.event);

	BTL_FUNC_END();
}


/*-------------------------------------------------------------------------------
 * BtlOppcGoepOpName()
 *
 *		Return a pointer to the name of the current client operation.
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
static const char *BtlOppcGoepOpName(GoepOperation Op)
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
 * BtlOppcObexHeaderType()
 *
 * Synopsis:  Return a pointer to a description of the OBEX header type.
 *
 * Return:    ASCII String pointer.
 *
 */
static const char *BtlOppcObexHeaderType(ObexHeaderType type)
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
 * BtlOppcAbortMsg()
 *
 * Synopsis:  Return a pointer to a description of the OBEX Abort reason.
 *
 * Return:    ASCII String pointer.
 *
 */
static const char *BtlOppcAbortMsg(ObexRespCode Reason)
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
    }
    return "Other";
}


static BtStatus BtlOppcBtlNotificationsCb(BtlModuleNotificationType notificationType)
{
	BtStatus		status = BT_STATUS_SUCCESS;
	ObStatus    	obStatus = OB_STATUS_SUCCESS;
	BtlOppcContext	*oppcContext;
	BtlContext		*base;

	BTL_FUNC_START("BtlOppcBtlNotificationsCb");
	
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

			IterateList(btlOppcData.contextsList, base, BtlContext *)
			{
				oppcContext = (BtlOppcContext *)base;

				switch (oppcContext->state)
				{
					case (BTL_OPPC_STATE_DISCONNECTING):
					{
						/* OPPC state is now in the process of disabling */
						oppcContext->state = BTL_OPPC_STATE_DISABLING;
						break;
					}

					case (BTL_OPPC_STATE_CONNECTED):
					{							
						obStatus = GOEP_TpDisconnect(&oppcContext->client);
						
						BTL_VERIFY_ERR((OB_STATUS_BUSY != obStatus), BT_STATUS_BUSY, 
							("OPPC is currently executing an operation"));
						
						if (OB_STATUS_PENDING == obStatus)
						{
							/* OPPC state is now in the process of disabling */
							oppcContext->state = BTL_OPPC_STATE_DISABLING;
							break;
						}
						
						break;
					}

					case (BTL_OPPC_STATE_CONNECTING):
					{
						/* In process of connecting, let it finish and then close the stream */

						/* OPPC state is now in the process of disabling */
						oppcContext->state = BTL_OPPC_STATE_DISABLING;
						break;
					}
				}

				if (oppcContext->state == BTL_OPPC_STATE_DISABLING)
				{
					oppcContext->disableState |= BTL_OPPC_DISABLE_STATE_MASK_RADIO_OFF_ACTIVE;
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


static void BtlOppcHandleAbort(BtlOppcContext *oppcContext, GoepClientEvent *Event)
{
    ObexRespCode    rcode = OBRC_SUCCESS;

#ifdef OPP_PTS_FIX
	BtStatus status;
	BtlObject voidBtlObject;
#endif //OPP_PTS_FIX

    BTL_FUNC_START("BtlOppcHandleAbort");
    
    if (Event->oper == GOEP_OPER_PUSH)  
    {
        if (oppcContext->currBasicOper == BTL_OPPC_OPER_BASIC_PUSH)  
        {
            rcode = OBSTORE_Close((OppObStoreHandle *)&(oppcContext->objReqParams.object));

        }
        else if (oppcContext->currBasicOper == BTL_OPPC_OPER_BASIC_EXCHANGE) 
        {


            rcode = OBSTORE_Close((OppObStoreHandle *)&(oppcContext->objReqParams.object));

#ifdef OPP_PTS_FIX
			oppcContext->opState |= BTL_OPPC_OP_STATE_MASK_PUSH_SUCCESS;

			oppcContext->objReqParams.object = oppcContext->nextObjToPull;
			oppcContext->objReqParams.name = NULL;
			oppcContext->objReqParams.type = (const U8 *)oppcContext->nextObjToPull->type;

			status = BtlOppcPull(oppcContext, &voidBtlObject);
			BTL_VERIFY_ERR((OB_STATUS_PENDING == status), BT_STATUS_INTERNAL_ERROR,
						("Failed to Pull object in EXCHANGE operation"));

#else //OPP_PTS_FIX

            OBSTORE_Delete((OppObStoreHandle *)&(oppcContext->nextObjToPull));
        
#endif //OPP_PTS_FIX

        }
        else if (oppcContext->currEncapOper == BTL_OPPC_OPER_ENC_CON_EXCHANGE_DISCON)
        {
            rcode = OBSTORE_Close((OppObStoreHandle *)&(oppcContext->objReqParams.object));

#ifdef OPP_PTS_FIX

			oppcContext->opState |= BTL_OPPC_OP_STATE_MASK_PUSH_SUCCESS;

			oppcContext->objReqParams.object = oppcContext->nextObjToPull;
			oppcContext->objReqParams.name = NULL;
			oppcContext->objReqParams.type = (const U8 *)oppcContext->nextObjToPull->type;

			status = BtlOppcPull(oppcContext, &voidBtlObject);
			BTL_VERIFY_ERR((OB_STATUS_PENDING == status), BT_STATUS_INTERNAL_ERROR,
						("Failed to Pull object in EXCHANGE operation"));

#else //OPP_PTS_FIX
            OBSTORE_Delete((OppObStoreHandle *)&(oppcContext->nextObjToPull));
#endif //OPP_PTS_FIX
        
        }
        else if (oppcContext->currEncapOper == BTL_OPPC_OPER_ENC_CON_PUSH_DISCON)
        {
            rcode = OBSTORE_Close((OppObStoreHandle *)&(oppcContext->objReqParams.object));
        }
        else if (oppcContext->currEncapOper == BTL_OPPC_OPER_ENC_CON_PULL_DISCON)
        {
            /* should not reach here */
            BTL_FATAL_NO_RETVAR(("Invalid OPPC_PUSH Event received during Encapsulated PULL operation"));
        }
    }
    else if (Event->oper == GOEP_OPER_PULL) 
    {
        if (oppcContext->currBasicOper == BTL_OPPC_OPER_BASIC_PULL)  
        {
            OBSTORE_Delete((OppObStoreHandle *)&(oppcContext->objReqParams.object));

        }
        else if (oppcContext->currBasicOper == BTL_OPPC_OPER_BASIC_EXCHANGE) 
        {
            OBSTORE_Delete((OppObStoreHandle *)&(oppcContext->objReqParams.object));
            oppcContext->nextObjToPull = NULL;
        
        }
        else if (oppcContext->currEncapOper == BTL_OPPC_OPER_ENC_CON_EXCHANGE_DISCON)
        {
            OBSTORE_Delete((OppObStoreHandle *)&(oppcContext->objReqParams.object));
            oppcContext->nextObjToPull = NULL;

        }                        
        else if (oppcContext->currEncapOper == BTL_OPPC_OPER_ENC_CON_PULL_DISCON)
        {
            OBSTORE_Delete((OppObStoreHandle *)&(oppcContext->objReqParams.object));
        }
        else if (oppcContext->currEncapOper == BTL_OPPC_OPER_ENC_CON_PUSH_DISCON)
        {
            /* should not reach here */
            BTL_FATAL_NO_RETVAR(("Invalid OPPC_PULL Event received during Encapsulated PUSH operation"));
        }
    }

#if BTL_CONFIG_OPPC_PUSH_PULL_TIMER > 0
    
    if (TRUE == oppcContext->isObexTimerstarted)
    {
		BTL_OBEX_UTILS_CancelTimer(&oppcContext->obexOperTimer);
		oppcContext->isObexTimerstarted = FALSE;
    }

#endif /* BTL_CONFIG_OPPC_PUSH_PULL_TIMER > 0 */

	BTL_FUNC_END();
}


static void BtlOppcCloseObjHandle(BtlOppcContext *oppcContext)
{
    if (oppcContext->objReqParams.object)
    {
        OBSTORE_Close((OppObStoreHandle *)&(oppcContext->objReqParams.object));
    }

    if (!(oppcContext->opState & BTL_OPPC_OP_STATE_MASK_PULL_SUCCESS) && (oppcContext->nextObjToPull))
    {
        OBSTORE_Delete((OppObStoreHandle *)&(oppcContext->nextObjToPull));
    }
}


static BtStatus BtlOppcPush(BtlOppcContext *oppcContext, const BtlObject *objToPush)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtStatus ret;
    
	BTL_FUNC_START("BtlOppcPush");
    
	BTL_VERIFY_ERR((NULL != oppcContext), BT_STATUS_INVALID_PARM, ("Null oppcContext"));

	BTL_VERIFY_ERR((BTL_OPPC_STATE_CONNECTED == oppcContext->state), 
		BT_STATUS_NO_CONNECTION, ("OPPC context is not connected"));

	if (TRUE != oppcContext->isPushObjInitialized)
	{
		ret = BtlOppcBuildPushReq(oppcContext, objToPush);
        	
		if (BT_STATUS_FAILED == ret)
		{
			OBSTORE_Close((OppObStoreHandle *)&(oppcContext->objReqParams.object));
		}
        	
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == ret), BT_STATUS_INVALID_PARM, 
			("Failed to build object push request."));

		oppcContext->currBasicOper = BTL_OPPC_OPER_BASIC_PUSH;
	}

	status = OPUSH_Push(&oppcContext->client, &oppcContext->objReqParams);

#if BTL_CONFIG_OPPC_PUSH_PULL_TIMER > 0

	BTL_VERIFY_ERR((FALSE == oppcContext->isObexTimerstarted),  status,
		("OPPC push timer is already started."));
	
	BTL_OBEX_UTILS_StartTimer(&oppcContext->obexOperTimer, (TimeT)BTL_CONFIG_OPPC_PUSH_PULL_TIMER * 1000, 
            BtlOppcTimeoutHandler, oppcContext);

	oppcContext->isObexTimerstarted = TRUE;

#endif /* BTL_CONFIG_OPPC_PUSH_PULL_TIMER > 0 */

	BTL_FUNC_END();

	return status;
}


static BtStatus BtlOppcPull(BtlOppcContext *oppcContext, const BtlObject *objToPull)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtStatus ret;
    
	BTL_FUNC_START("BtlOppcPull");
    
	BTL_VERIFY_ERR((NULL != oppcContext), BT_STATUS_INVALID_PARM, ("Null oppcContext"));

	BTL_VERIFY_ERR((BTL_OPPC_STATE_CONNECTED == oppcContext->state),
       	 BT_STATUS_NO_CONNECTION, ("OPPC context is not connected"));
    
	if (TRUE != oppcContext->isPullObjInitialized)
	{
		ret = BtlOppcBuildPullReq(oppcContext, objToPull);
		
		if (BT_STATUS_FAILED == ret)
		{
			OBSTORE_Close((OppObStoreHandle *)&(oppcContext->objReqParams.object));
		}
        
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == ret), BT_STATUS_INVALID_PARM, 
			("Failed to build object pull request."));

		oppcContext->currBasicOper = BTL_OPPC_OPER_BASIC_PULL;
	}

	status = OPUSH_Pull(&oppcContext->client, &oppcContext->objReqParams, FALSE);

	if (OB_STATUS_PENDING != status)
	{
		OBSTORE_Delete((OppObStoreHandle *)&(oppcContext->objReqParams.object));
	}
#if BTL_CONFIG_OPPC_PUSH_PULL_TIMER > 0
	else
	{
		BTL_VERIFY_ERR((FALSE == oppcContext->isObexTimerstarted),  status,
			("OPPC pull timer is already started."));

		BTL_OBEX_UTILS_StartTimer(&oppcContext->obexOperTimer, (TimeT)BTL_CONFIG_OPPC_PUSH_PULL_TIMER * 1000, 
			BtlOppcTimeoutHandler, oppcContext);

		oppcContext->isObexTimerstarted = TRUE;
	}
#endif /* BTL_CONFIG_OPPC_PUSH_PULL_TIMER > 0 */

	BTL_FUNC_END();

    return status;
}


static BtStatus BtlOppcConnect(BtlOppcContext *oppcContext,
                            const BD_ADDR *bdAddr)                           
{
	BtStatus status = BT_STATUS_PENDING;
	ObexTpAddr Target;
	BtDeviceContext * bdc;
	BtDeviceContext tmplt;
	
	BTL_FUNC_START("BtlOppcConnect");

	BTL_VERIFY_ERR((0 != oppcContext), BT_STATUS_INVALID_PARM, ("Null oppcContext"));
	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));
	
	BTL_VERIFY_ERR((BTL_OPPC_STATE_DISCONNECTED == oppcContext->state), 
       	BT_STATUS_FAILED, ("OPPC context is not disconnected"));

	Target.type = OBEX_TP_BLUETOOTH;
	Target.proto.bt.addr = *bdAddr;

	/* Find a device in DS */
	bdc = DS_FindDevice(bdAddr);
	if (!bdc) 
	{
		OS_MemSet(&tmplt, 0, (U32)sizeof(BtDeviceContext));
		OS_MemCopy(&tmplt.addr, bdAddr, sizeof(BD_ADDR));
		DS_AddDevice(&tmplt, 0);
		Target.proto.bt.psi.psRepMode = 1;
	 	Target.proto.bt.psi.psMode = 0;
		Target.proto.bt.psi.clockOffset = 0;
	}
	else
	{
		Target.proto.bt.psi = bdc->psi;
	}

	status = OPUSH_Connect(&oppcContext->client, &Target);

	BTL_VERIFY_ERR((OB_STATUS_PENDING == status), status, 
                        ("Failed connecting OPP client"));

	/* OPPC state is now in the process of connecting */
	oppcContext->state = BTL_OPPC_STATE_CONNECTING;

	BTL_FUNC_END();

	return (status);
}


static BtStatus BtlOppcDisconnect(BtlOppcContext *oppcContext)
{
	BtStatus status = BT_STATUS_PENDING;
	ObStatus obStatus;
	
	BTL_FUNC_START("BtlOppcDisconnect");

	BTL_VERIFY_ERR((0 != oppcContext), BT_STATUS_INVALID_PARM, ("Null oppcContext"));
 	
	BTL_VERIFY_ERR(((BTL_OPPC_STATE_CONNECTED == oppcContext->state) ||
                    (BTL_OPPC_STATE_CONNECTING == oppcContext->state)), 
                   BT_STATUS_NO_CONNECTION,
                   ("OPPC context is not connected"));

	obStatus = GOEP_Disconnect(&oppcContext->client);
	if (OB_STATUS_BUSY == obStatus)
	{
		/* OPPC is currently executing an operation */
	obStatus = GOEP_TpDisconnect(&oppcContext->client);
	}

	BTL_VERIFY_ERR(((OB_STATUS_SUCCESS == obStatus) ||
                    (OB_STATUS_PENDING == obStatus)),
                   BT_STATUS_FAILED, 
                   ("Failed disconnecting OPP client"));
	
	if (OB_STATUS_SUCCESS == obStatus)
	{
		oppcContext->state = BTL_OPPC_STATE_DISCONNECTED;
		status = BT_STATUS_SUCCESS;
	}
	else
	{
		/* OPPC state is now in the process of disconnecting */
		oppcContext->state = BTL_OPPC_STATE_DISCONNECTING;
	}

	BTL_FUNC_END();

	return (status);
}


#if BTL_CONFIG_OPPC_PUSH_PULL_TIMER > 0
static void BtlOppcTimeoutHandler(EvmTimer *timer)
{
	BtlOppcContext	*oppcContext;
	ObStatus obStatus;
   	BtStatus status = BT_STATUS_SUCCESS;
    
	BTL_FUNC_START("BtlOppcTimeoutHandler");
    
	oppcContext = (BtlOppcContext *)timer->context;

	BTL_VERIFY_FATAL((BTL_OPPC_STATE_CONNECTED == oppcContext->state), BT_STATUS_FAILED, 
				 ("OPPC is not connected"));
    
	BTL_VERIFY_FATAL((BTL_OPPC_OPER_BASIC_NONE != oppcContext->currBasicOper) 
       	  || (BTL_OPPC_OPER_ENC_NONE != oppcContext->currEncapOper), BT_STATUS_FAILED, 
				 ("OPPC is not currently executing any operation"));
    
	obStatus = GOEP_TpDisconnect(&oppcContext->client);
			
	BTL_VERIFY_ERR((OB_STATUS_BUSY != obStatus), BT_STATUS_BUSY, 
			 ("OPPC is currently executing an operation"));
			
	BTL_FUNC_END();
}
#endif





#else /* BTL_CONFIG_OPP ==   BTL_CONFIG_ENABLED */

/*-------------------------------------------------------------------------------
 * BTL_OPPC_Init() - When BTL_CONFIG_OPP Disabled
 */
BtStatus BTL_OPPC_Init(void)
{
    
	BTL_LOG_INFO(("BTL_OPPC_Init() -  BTL_CONFIG_OPP Disabled"));

	return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 * BTL_OPPC_Deinit() - When BTL_CONFIG_OPP Disabled
 */
BtStatus BTL_OPPC_Deinit(void)
{
	BTL_LOG_INFO(("BTL_OPPC_Deinit() -  BTL_CONFIG_OPP Disabled"));

	return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------*/



#endif /* BTL_CONFIG_BIP==   BTL_CONFIG_ENABLED */



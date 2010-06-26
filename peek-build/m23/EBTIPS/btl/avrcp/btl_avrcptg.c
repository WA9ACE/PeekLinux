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
*   FILE NAME:      btl_avrcptg.c
*
*   DESCRIPTION:    This file contains the BTL AVRCP target implementation.
*
*   AUTHOR:         Ilan Elias
*
\*******************************************************************************/

#include "btl_config.h"
#include "btl_defs.h"

BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_AVRCPTG);


#if BTL_CONFIG_AVRCPTG == BTL_CONFIG_ENABLED

/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "btl_avrcptg.h"
#include "btl_pool.h"
#include "btl_commoni.h"
#include "btl_common.h"
#include "btl_bsc.h"
#include "l2cap.h"
#include "avrcpi.h"
#include "config.h"



/********************************************************************************
 *
 * Constants 
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BSL_AVRCPTG_DEFAULT constant
 *
 *     Represents default security level for AVRCPTG.
 */
#define BSL_AVRCPTG_DEFAULT  (BSL_AUTHENTICATION_IN | BSL_AUTHENTICATION_OUT)


/*-------------------------------------------------------------------------------
 * AVRCP_SERVICE_NAME_MAX_LEN constant
 *
 *     Represents max length of service name for AVRCP.
 */
#define AVRCPTG_SERVICE_NAME_MAX_LEN  						(32)


/*-------------------------------------------------------------------------------
 * AVRCPTG_SUPPORTED_CATEGORIES_ARRAY_SIZE constant
 *
 *     Represents supported categories array size for AVRCP.
 */
#define AVRCPTG_SUPPORTED_CATEGORIES_ARRAY_SIZE  			(3)


/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/* Forward declarations */
typedef struct _BtlAvrcptgChannel 	BtlAvrcptgChannel;
typedef struct _BtlAvrcptgData 		BtlAvrcptgData;


/*-------------------------------------------------------------------------------
 * BtlAvrcptgInitState type
 *
 *     Defines the AVRCPTG init state.
 */
typedef U8 BtlAvrcptgInitState;

#define BTL_AVRCPTG_INIT_STATE_NOT_INTIALIZED				(0x00)
#define BTL_AVRCPTG_INIT_STATE_INITIALIZED					(0x01)
#define BTL_AVRCPTG_INIT_STATE_INITIALIZATION_FAILED		(0x02)
#define BTL_AVRCPTG_INIT_STATE_DEINITIALIZATION_FAILED		(0x03)


/*-------------------------------------------------------------------------------
 * BtlAvrcptgState type
 *
 *     Defines the AVRCPTG state of a specific context.
 */
typedef U8 BtlAvrcptgState;

#define BTL_AVRCPTG_STATE_DISABLED							(0x00)
#define BTL_AVRCPTG_STATE_ENABLED							(0x01)
#define BTL_AVRCPTG_STATE_DISABLING							(0x02)


/*-------------------------------------------------------------------------------
 * BtlAvrcptgState type
 *
 *     Defines the AVRCPTG state of a specific context.
 */
typedef U8 BtlAvrcptgChannelState;

#define BTL_AVRCPTG_CHANNEL_STATE_IDLE						(0x00)
#define BTL_AVRCPTG_CHANNEL_STATE_DISCONNECTED				(0x01)
#define BTL_AVRCPTG_CHANNEL_STATE_CONNECTING				(0x02)
#define BTL_AVRCPTG_CHANNEL_STATE_CONNECT_IND				(0x03)
#define BTL_AVRCPTG_CHANNEL_STATE_CONNECTED					(0x04)
#define BTL_AVRCPTG_CHANNEL_STATE_DISCONNECTING				(0x05)


/*-------------------------------------------------------------------------------
 * BtlAvrcptgDisableStateMask type
 *
 *     Defines the AVRCPTG disable state mask.
 */
typedef U8 BtlAvrcptgDisableStateMask;

#define BTL_AVRCPTG_DISABLE_STATE_MASK_NONE					(0x00)
#define BTL_AVRCPTG_DISABLE_STATE_MASK_DISABLE_ACTIVE		(0x01)
#define BTL_AVRCPTG_DISABLE_STATE_MASK_RADIO_OFF_ACTIVE		(0x02)
#define BTL_AVRCPTG_DISABLE_STATE_MASK_ENABLED				(0x04)


/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/


/*-------------------------------------------------------------------------------
 * BtlAvrcptgChannel structure
 *
 *     Represents BTL AVRCPTG channel.
 */
struct _BtlAvrcptgChannel
{	
	/* Internal AVRCPTG channel */
	AvrcpChannel channel;

	/* Current AVRCPTG state of the channel */
	BtlAvrcptgChannelState state;

	/* Connected remote device, Null if not connected  */
    BtRemoteDevice *remDev;

	/* Associated context */
	BtlAvrcptgContext *context;
};


/*-------------------------------------------------------------------------------
 * BtlAvrcptgContext structure
 *
 *     Represents BTL AVRCPTG context.
 */
struct _BtlAvrcptgContext 
{
	/* Must be first field */
	BtlContext base;
	
	/* Internal AVRCPTG channels */
	BtlAvrcptgChannel channels[BTL_CONFIG_AVRCPTG_MAX_NUM_CHANNELS_PER_CONTEXT];

	/* Associated callback with this context */
	BtlAvrcptgCallBack callback;

	/* Current AVRCPTG state of the context */
	BtlAvrcptgState state;

#if BT_SECURITY == XA_ENABLED

	/* Registered security record */
	BtSecurityRecord securityRecord;

#endif	/* BT_SECURITY == XA_ENABLED */

	/* Service name plus 1 null-terminated char, plus 2 bytes for SDP_TEXT_8BIT */
	U8 serviceName[AVRCPTG_SERVICE_NAME_MAX_LEN + 3];

	/* 3 bytes for SDP_UINT_16BIT */
	U8 supportedCategories[AVRCPTG_SUPPORTED_CATEGORIES_ARRAY_SIZE];

	/* This flag indicates the disable state */
	BtlAvrcptgDisableStateMask disableState;
};


/*-------------------------------------------------------------------------------
 * BtlAvrcptgData structure
 *
 *     Represents the data of th BTL AVRCPTG module.
 */
struct _BtlAvrcptgData
{
	/* Pool of AVRCPTG contexts */
	BTL_POOL_DECLARE_POOL(contextsPool, contextsMemory, BTL_CONFIG_AVRCPTG_MAX_NUM_OF_CONTEXTS, sizeof(BtlAvrcptgContext));

	/* List of active AVRCPTG contexts */
	ListEntry contextsList;

	/* Event passed to the application */
	BtlAvrcptgEvent event;
};


/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * btlAvrcptgInitState
 *
 *     Represents the current init state of AVRCPTG module.
 */
static BtlAvrcptgInitState btlAvrcptgInitState = BTL_AVRCPTG_INIT_STATE_NOT_INTIALIZED;


/*-------------------------------------------------------------------------------
 * btlAvrcptgData
 *
 *     Represents the data of AVRCPTG module.
 */
static BtlAvrcptgData btlAvrcptgData;


/*-------------------------------------------------------------------------------
 * btlAvrcptgContextsPoolName
 *
 *     Represents the name of the AVRCPTG contexts pool.
 */
static const char btlAvrcptgContextsPoolName[] = "AvrcptgContexts";


/********************************************************************************
 *
 * Internal function prototypes
 *
 *******************************************************************************/
static BtStatus BtlAvrcptgDeregisterAll(BtlAvrcptgContext *avrcptgContext);
static BtStatus BtlAvrcptgDisable(BtlAvrcptgContext *avrcptgContext);
static void BtlAvrcptgCallback(AvrcpChannel *chnl, AvrcpCallbackParms *Parms);
static const char *BtlAvrcptgCtype(U8 msgCtype);
static const char *BtlAvrcptgOpCode(U8 msgOpCode);
static const char *BtlAvrcptgResponse(U8 msgResponse);
static const char *BtlAvrcptgPanelOperation(AvrcpPanelOperation msgPanelOperation);
static const char *BtlAvrcptgSubunitType(U8 msgSubunitType);
static const char *BtlAvrcptgSubunitId(U8 msgSubunitId);
static BtStatus BtlAvrcptgBtlNotificationsCb(BtlModuleNotificationType notificationType);
static void BtlAvrcptgSendEventToBsc(BtlBscEventType eventType,
                                     BD_ADDR *bdAddr,
                                     BtlAvrcptgContext *avrcptgContext);
static const char *BtlAvrcptgRecommendedEventName(AvrcpMetadataEventId event);


/********************************************************************************
 *
 * Function definitions
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BTL_AVRCPTG_Init()
 */
BtStatus BTL_AVRCPTG_Init(void)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_COMMON("BTL_AVRCPTG_Init");
	
	BTL_VERIFY_ERR((BTL_AVRCPTG_INIT_STATE_NOT_INTIALIZED == btlAvrcptgInitState), 
		BT_STATUS_FAILED, ("AVRCPTG module is already initialized"));
	
	btlAvrcptgInitState = BTL_AVRCPTG_INIT_STATE_INITIALIZATION_FAILED;

	status = BTL_POOL_Create(&btlAvrcptgData.contextsPool,
							btlAvrcptgContextsPoolName,
							btlAvrcptgData.contextsMemory, 
							BTL_CONFIG_AVRCPTG_MAX_NUM_OF_CONTEXTS,
							sizeof(BtlAvrcptgContext));
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, 
		("AVRCPTG contexts pool creation failed"));
	
	InitializeListHead(&btlAvrcptgData.contextsList);
		
	BTL_RegisterForCommonNotifications(BTL_MODULE_TYPE_AVRCPTG, BtlAvrcptgBtlNotificationsCb);
		
	btlAvrcptgInitState = BTL_AVRCPTG_INIT_STATE_INITIALIZED;

	BTL_FUNC_END_AND_UNLOCK();
	
	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_AVRCPTG_Deinit()
 */
BtStatus BTL_AVRCPTG_Deinit(void)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_COMMON("BTL_AVRCPTG_Deinit");
	
	BTL_VERIFY_ERR((BTL_AVRCPTG_INIT_STATE_INITIALIZED == btlAvrcptgInitState), 
		BT_STATUS_FAILED, ("AVRCPTG module is not initialized"));

	btlAvrcptgInitState = BTL_AVRCPTG_INIT_STATE_DEINITIALIZATION_FAILED;

	BTL_DeRegisterFromCommonNotifications(BTL_MODULE_TYPE_AVRCPTG);
	
	BTL_VERIFY_ERR((IsListEmpty(&btlAvrcptgData.contextsList)), 
		BT_STATUS_FAILED, ("AVRCPTG contexts are still active"));

	status = BTL_POOL_Destroy(&btlAvrcptgData.contextsPool);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, 
		("AVRCPTG contexts pool destruction failed"));
		
	btlAvrcptgInitState = BTL_AVRCPTG_INIT_STATE_NOT_INTIALIZED;

	BTL_FUNC_END_AND_UNLOCK();
	
	return (status);
}

 
/*-------------------------------------------------------------------------------
 * BTL_AVRCPTG_Create()
 */
BtStatus BTL_AVRCPTG_Create(BtlAppHandle *appHandle,
							const BtlAvrcptgCallBack avrcptgCallback,
							const BtSecurityLevel *securityLevel,
							BtlAvrcptgContext **avrcptgContext)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U32 idx;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_AVRCPTG_Create");

	BTL_VERIFY_ERR((0 != avrcptgCallback), BT_STATUS_INVALID_PARM, ("Null avrcptgCallback"));
	BTL_VERIFY_ERR((0 != avrcptgContext), BT_STATUS_INVALID_PARM, ("Null avrcptgContext"));

#if BT_SECURITY == XA_ENABLED

	if (0 != securityLevel)
	{
		status = BTL_VerifySecurityLevel(*securityLevel);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Invalid AVRCPTG securityLevel"));
	}

#endif	/* BT_SECURITY == XA_ENABLED */

	/* Allocate memory for a unique new AVRCPTG context */
	status = BTL_POOL_Allocate(&btlAvrcptgData.contextsPool, (void **)avrcptgContext);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed allocating AVRCPTG context"));

	status = BTL_HandleModuleInstanceCreation(appHandle, BTL_MODULE_TYPE_AVRCPTG, &(*avrcptgContext)->base);
	if (BT_STATUS_SUCCESS != status)
	{
		/* avrcptgContext must be freed before we exit */
		BtStatus retVal = BTL_POOL_Free(&btlAvrcptgData.contextsPool, (void **)avrcptgContext);
		BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == retVal), ("Failed freeing AVRCPTG context"));

		/* This verify will fail and will handle the exception gracefully */
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed handling AVRCPTG instance creation"));
	}

	/* Save the given callback */
	(*avrcptgContext)->callback = avrcptgCallback;

	/* Add the new AVRCPTG context to the active contexts list */
	InsertTailList(&btlAvrcptgData.contextsList, &((*avrcptgContext)->base.node));

#if BT_SECURITY == XA_ENABLED

	/* Set the security record & save the given security level, or use default */
	(*avrcptgContext)->securityRecord.id = SEC_L2CAP_ID;
	(*avrcptgContext)->securityRecord.channel = BT_PSM_AVCTP;
	if(securityLevel == 0)
		(*avrcptgContext)->securityRecord.level = BSL_AVRCPTG_DEFAULT;
	else
		(*avrcptgContext)->securityRecord.level = (*securityLevel);
	(*avrcptgContext)->securityRecord.service = SDP_SERVICE_AVRCP;

#endif	/* BT_SECURITY == XA_ENABLED */

	(*avrcptgContext)->disableState = BTL_AVRCPTG_DISABLE_STATE_MASK_NONE;

	/* Init AVRCPTG context state */
	(*avrcptgContext)->state = BTL_AVRCPTG_STATE_DISABLED;

	/* Init AVRCPTG channels state to idle */
	for (idx = 0; idx<BTL_CONFIG_AVRCPTG_MAX_NUM_CHANNELS_PER_CONTEXT; idx++)
	{
		(*avrcptgContext)->channels[idx].remDev = 0;
		(*avrcptgContext)->channels[idx].state = BTL_AVRCPTG_CHANNEL_STATE_IDLE;
		(*avrcptgContext)->channels[idx].context = (*avrcptgContext);
	}

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_AVRCPTG_Destroy()
 */
BtStatus BTL_AVRCPTG_Destroy(BtlAvrcptgContext **avrcptgContext)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BOOL isAllocated = FALSE;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_AVRCPTG_Destroy");

	BTL_VERIFY_ERR((0 != avrcptgContext), BT_STATUS_INVALID_PARM, ("Null avrcptgContext"));
	BTL_VERIFY_ERR((0 != *avrcptgContext), BT_STATUS_INVALID_PARM, ("Null *avrcptgContext"));
	BTL_VERIFY_ERR((BTL_AVRCPTG_STATE_DISABLED == (*avrcptgContext)->state), BT_STATUS_IN_USE, ("AVRCPTG context is in use"));

	status = BTL_POOL_IsElelementAllocated(&btlAvrcptgData.contextsPool, *avrcptgContext, &isAllocated);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed locating given AVRCPTG context"));
	BTL_VERIFY_ERR((TRUE == isAllocated), BT_STATUS_INVALID_PARM, ("Invalid AVRCPTG context"));

	/* Remove the context from the list of all AVRCPTG contexts */
	RemoveEntryList(&((*avrcptgContext)->base.node));

	status = BTL_HandleModuleInstanceDestruction(&(*avrcptgContext)->base);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed handling AVRCPTG instance destruction"));

	status = BTL_POOL_Free(&btlAvrcptgData.contextsPool, (void **)avrcptgContext);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("Failed freeing AVRCPTG context"));

	/* Set the AVRCPTG context to NULL */
	*avrcptgContext = 0;

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_AVRCPTG_Enable()
 */
BtStatus BTL_AVRCPTG_Enable(BtlAvrcptgContext *avrcptgContext, 
							const BtlUtf8 *serviceName,
							const BtlAvrcptgSupportedCategoriesMask supportedCategories)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U32 idx;
	U16 len = 0;
		
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_AVRCPTG_Enable");

	BTL_VERIFY_ERR((0 != avrcptgContext), BT_STATUS_INVALID_PARM, ("Null avrcptgContext"));
	BTL_VERIFY_ERR((BTL_AVRCPTG_STATE_DISABLED == avrcptgContext->state), BT_STATUS_FAILED, ("AVRCPTG context is already enabled"));

	if ((serviceName != 0) && ((len = OS_StrLen((char *)serviceName)) != 0))
	{
		BTL_VERIFY_ERR((len <= AVRCPTG_SERVICE_NAME_MAX_LEN), BT_STATUS_FAILED, ("AVRCPTG service name is too long!"));
	}

	for (idx = 0; idx<BTL_CONFIG_AVRCPTG_MAX_NUM_CHANNELS_PER_CONTEXT; idx++)
	{
		status = AVRCP_Register(&(avrcptgContext->channels[idx].channel), BtlAvrcptgCallback, AVRCP_TG);

		if (status != BT_STATUS_SUCCESS)
		{
			/* Deregister all registered channels */
			BtlAvrcptgDeregisterAll(avrcptgContext);

			/* This verify will fail and will handle the exception gracefully */
			BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed to register AVRCP channel %d", idx));
		}

		avrcptgContext->channels[idx].state = BTL_AVRCPTG_CHANNEL_STATE_DISCONNECTED;
	}

	/* Update serviceName */
 	if (len > 0)
 	{
  		/* The first 2 bytes are for SDP_TEXT_8BIT */
    	avrcptgContext->serviceName[0] = DETD_TEXT + DESD_ADD_8BITS;
    	avrcptgContext->serviceName[1] = (U8)(len + 1);	/* Includes '\0' */
    	
		OS_MemCopy((U8 *)(avrcptgContext->serviceName + 2), (const U8 *)serviceName, len);
   		avrcptgContext->serviceName[(len + 2)] = '\0';
		
  		/* now write the values into avrcpContext.tgAttrib[]*/
         AVRCP(tgSdpAttrib)[4].value = avrcptgContext->serviceName;
         AVRCP(tgSdpAttrib)[4].len = (U16)(len + 3);
 	}
	
	/* Update supportedCategories */
	avrcptgContext->supportedCategories[0] = DETD_UINT + DESD_2BYTES;
	avrcptgContext->supportedCategories[1] = (U8)(((supportedCategories) & 0xff00) >> 8);
	avrcptgContext->supportedCategories[2] = (U8)((supportedCategories) & 0x00ff);

    AVRCP(tgSdpAttrib)[5].value = avrcptgContext->supportedCategories;
    AVRCP(tgSdpAttrib)[5].len = sizeof(avrcptgContext->supportedCategories);
	

#if BT_SECURITY == XA_ENABLED

	/* Register security record */
	if (BT_STATUS_SUCCESS != (status = SEC_Register(&avrcptgContext->securityRecord)))
	{		
		/* Deregister all registered channels */
		BtlAvrcptgDeregisterAll(avrcptgContext);

		/* This verify will fail and will handle the exception gracefully */
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed registering AVRCPTG security record"));
	}
	
#endif	/* BT_SECURITY == XA_ENABLED */

	/* AVRCPTG state is now enabled */
	avrcptgContext->state = BTL_AVRCPTG_STATE_ENABLED;

	BTL_LOG_INFO(("BTL_AVRCPTG: Target is enabled with %d channels.", BTL_CONFIG_AVRCPTG_MAX_NUM_CHANNELS_PER_CONTEXT));

	BTL_FUNC_END_AND_UNLOCK();
	
	return (status); 
}


/*-------------------------------------------------------------------------------
 * BTL_AVRCPTG_Disable()
 */
BtStatus BTL_AVRCPTG_Disable(BtlAvrcptgContext *avrcptgContext)
{
	BtStatus status = BT_STATUS_PENDING;
	U32 idx;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_AVRCPTG_Disable");

	BTL_VERIFY_ERR((0 != avrcptgContext), BT_STATUS_INVALID_PARM, ("Null avrcptgContext"));
	BTL_VERIFY_ERR((BTL_AVRCPTG_STATE_DISABLED != avrcptgContext->state), BT_STATUS_FAILED, ("AVRCPTG context is already disabled!"));


	if (avrcptgContext->state == BTL_AVRCPTG_STATE_DISABLING)
	{
		avrcptgContext->disableState |= BTL_AVRCPTG_DISABLE_STATE_MASK_DISABLE_ACTIVE;

		BTL_RET(BT_STATUS_PENDING);
	}

	for (idx = 0; idx<BTL_CONFIG_AVRCPTG_MAX_NUM_CHANNELS_PER_CONTEXT; idx++)
	{
		switch (avrcptgContext->channels[idx].state)
		{
			case (BTL_AVRCPTG_CHANNEL_STATE_DISCONNECTING):
			{
				/* AVRCPTG state is now in the process of disabling */
				avrcptgContext->state = BTL_AVRCPTG_STATE_DISABLING;
				break;
			}
			case (BTL_AVRCPTG_CHANNEL_STATE_CONNECTED):
			{
				status = AVRCP_Disconnect(&(avrcptgContext->channels[idx].channel));
				
				if (BT_STATUS_PENDING == status)
				{
					avrcptgContext->channels[idx].state = BTL_AVRCPTG_CHANNEL_STATE_DISCONNECTING;
					
					/* AVRCPTG state is now in the process of disabling */
					avrcptgContext->state = BTL_AVRCPTG_STATE_DISABLING;
				}
				
				break;
			}
			case (BTL_AVRCPTG_CHANNEL_STATE_CONNECTING):
			{
				/* In process of connecting, let it finish and then close the channel */

				/* AVRCPTG state is now in the process of disabling */
				avrcptgContext->state = BTL_AVRCPTG_STATE_DISABLING;
				break;
			}
			case (BTL_AVRCPTG_CHANNEL_STATE_CONNECT_IND):
			{
				/* Reject incoming open request, and wait for close event */
				status = AVRCP_ConnectRsp(&(avrcptgContext->channels[idx].channel), FALSE);
				BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == status), ("Failed responding to open indication while disabling."));

				if (BT_STATUS_PENDING == status)
					avrcptgContext->state = BTL_AVRCPTG_STATE_DISABLING;
				
				break;
			}
		}
	}

	if (BTL_AVRCPTG_STATE_DISABLING != avrcptgContext->state)
	{
		/* All channels are disconnected */
		status = BtlAvrcptgDisable(avrcptgContext);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed disabling AVRCPTG context"));
	}
	else
	{
		avrcptgContext->disableState |= BTL_AVRCPTG_DISABLE_STATE_MASK_DISABLE_ACTIVE;
	}
	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_AVRCPTG_Connect()
 */
BtStatus BTL_AVRCPTG_Connect(BtlAvrcptgContext *avrcptgContext,
								BD_ADDR *bdAddr, 
								BtlAvrcptgChannelId *channelId)
{
	BtStatus status = BT_STATUS_PENDING;
	U32 idx;
	
	BTL_FUNC_START_AND_LOCK("BTL_AVRCPTG_Connect");

	BTL_VERIFY_ERR((0 != avrcptgContext), BT_STATUS_INVALID_PARM, ("Null avrcptgContext"));
	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));
	BTL_VERIFY_ERR((BTL_AVRCPTG_STATE_ENABLED == avrcptgContext->state), BT_STATUS_FAILED, ("AVRCPTG context is not enabled"));

	for (idx = 0; idx<BTL_CONFIG_AVRCPTG_MAX_NUM_CHANNELS_PER_CONTEXT; idx++)
	{
		if (avrcptgContext->channels[idx].state == BTL_AVRCPTG_CHANNEL_STATE_DISCONNECTED)
		{
			/* Found a free channel */
			break;
		}
	}

	BTL_VERIFY_ERR((idx != BTL_CONFIG_AVRCPTG_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_IN_USE, ("No free AVRCP channel"));

	status = AVRCP_Connect(&(avrcptgContext->channels[idx].channel), bdAddr);
	BTL_VERIFY_ERR((BT_STATUS_PENDING == status), status, ("Failed connecting AVRCP channel %d", idx));

	/* AVRCPTG state is now in the process of connecting */
	avrcptgContext->channels[idx].state = BTL_AVRCPTG_CHANNEL_STATE_CONNECTING;
	
	/* Return channelId to app */
	*channelId = idx;
	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_AVRCPTG_ConnectResponse()
 */
BtStatus BTL_AVRCPTG_ConnectResponse(BtlAvrcptgContext *avrcptgContext,
									BtlAvrcptgChannelId channelId,
									BOOL acceptCon)
{
	BtStatus status = BT_STATUS_PENDING;
	
	BTL_FUNC_START_AND_LOCK("BTL_AVRCPTG_ConnectResponse");

	BTL_VERIFY_ERR((0 != avrcptgContext), BT_STATUS_INVALID_PARM, ("Null avrcptgContext"));
	BTL_VERIFY_ERR((channelId < BTL_CONFIG_AVRCPTG_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid channelId"));
	BTL_VERIFY_ERR((BTL_AVRCPTG_CHANNEL_STATE_CONNECT_IND == avrcptgContext->channels[channelId].state), BT_STATUS_FAILED, ("AVRCP channel %d is not connect indicated", channelId));

	status = AVRCP_ConnectRsp(&(avrcptgContext->channels[channelId].channel), acceptCon);
	BTL_VERIFY_ERR((BT_STATUS_PENDING == status), status, ("Failed responding to connection indication %d", channelId));
	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_AVRCPTG_Disconnect()
 */
BtStatus BTL_AVRCPTG_Disconnect(BtlAvrcptgContext *avrcptgContext, BtlAvrcptgChannelId channelId)
{
	BtStatus status = BT_STATUS_PENDING;
	
	BTL_FUNC_START_AND_LOCK("BTL_AVRCPTG_Disconnect");

	BTL_VERIFY_ERR((0 != avrcptgContext), BT_STATUS_INVALID_PARM, ("Null avrcptgContext"));
	BTL_VERIFY_ERR((channelId < BTL_CONFIG_AVRCPTG_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid channelId"));
	BTL_VERIFY_ERR(((BTL_AVRCPTG_CHANNEL_STATE_CONNECTED == avrcptgContext->channels[channelId].state) ||
                   (BTL_AVRCPTG_CHANNEL_STATE_CONNECTING == avrcptgContext->channels[channelId].state)),
                   	BT_STATUS_NO_CONNECTION,
                  	("AVRCP channel %d is not connected", channelId));

	status = AVRCP_Disconnect(&(avrcptgContext->channels[channelId].channel));
	BTL_VERIFY_ERR(((BT_STATUS_SUCCESS == status) || (BT_STATUS_PENDING == status)),
                   status,
                   ("Failed disconnecting AVRCP channel %d", channelId));

	if (BT_STATUS_SUCCESS == status)
	{
		avrcptgContext->channels[channelId].state = BTL_AVRCPTG_CHANNEL_STATE_DISCONNECTED;
	}
	else if (BT_STATUS_PENDING == status)
	{
	/* AVRCPTG state is now in the process of disconnecting */
	avrcptgContext->channels[channelId].state = BTL_AVRCPTG_CHANNEL_STATE_DISCONNECTING;
	}
	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_AVRCPTG_GetConnectedDevice()
 */
BtStatus BTL_AVRCPTG_GetConnectedDevice(BtlAvrcptgContext *avrcptgContext, 
										BtlAvrcptgChannelId channelId, 
										BD_ADDR *bdAddr)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_AVRCPTG_GetConnectedDevice");

	BTL_VERIFY_ERR((0 != avrcptgContext), BT_STATUS_INVALID_PARM, ("Null avrcptgContext"));
	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));
	BTL_VERIFY_ERR((channelId < BTL_CONFIG_AVRCPTG_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid channelId"));
	BTL_VERIFY_ERR((BTL_AVRCPTG_CHANNEL_STATE_CONNECTED == avrcptgContext->channels[channelId].state), BT_STATUS_NO_CONNECTION, ("AVRCP channel %d is not connected", channelId));

	BTL_VERIFY_FATAL((0 != avrcptgContext->channels[channelId].remDev), BT_STATUS_INTERNAL_ERROR, ("Unable to find connected remote device"));

	OS_MemCopy((U8 *)bdAddr, (const U8 *)(&(avrcptgContext->channels[channelId].remDev->bdAddr)), sizeof(BD_ADDR));

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_AVRCPTG_PanelOperationResponse()
 */
BtStatus BTL_AVRCPTG_PanelOperationResponse(BtlAvrcptgContext *avrcptgContext,
											BtlAvrcptgChannelId channelId,
											const AvrcpResponse rsp)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK("BTL_AVRCPTG_PanelOperationResponse");

	BTL_VERIFY_ERR((0 != avrcptgContext), BT_STATUS_INVALID_PARM, ("Null avrcptgContext"));
	BTL_VERIFY_ERR((channelId < BTL_CONFIG_AVRCPTG_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid channelId"));
	BTL_VERIFY_ERR((BTL_AVRCPTG_CHANNEL_STATE_CONNECTED == avrcptgContext->channels[channelId].state), BT_STATUS_FAILED, ("AVRCP channel %d is not connected", channelId));

	if ((AVRCP_RESPONSE_NOT_IMPLEMENTED == rsp) || 
		(AVRCP_RESPONSE_REJECTED == rsp))
	{
		AVRCP_RejectPanelOperation(&(avrcptgContext->channels[channelId].channel), rsp);
	}
	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_AVRCPTG_SendResponse()
 */
BtStatus BTL_AVRCPTG_SendResponse(BtlAvrcptgContext *avrcptgContext,
									BtlAvrcptgChannelId channelId,
									AvrcpRspFrame *rspFrame)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK("BTL_AVRCPTG_SendResponse");

	BTL_VERIFY_ERR((0 != avrcptgContext), BT_STATUS_INVALID_PARM, ("Null avrcptgContext"));
	BTL_VERIFY_ERR((0 != rspFrame), BT_STATUS_INVALID_PARM, ("Null rspFrame"));
	BTL_VERIFY_ERR((channelId < BTL_CONFIG_AVRCPTG_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid channelId"));
	BTL_VERIFY_ERR((BTL_AVRCPTG_CHANNEL_STATE_CONNECTED == avrcptgContext->channels[channelId].state), BT_STATUS_FAILED, ("AVRCP channel %d is not connected", channelId));

	status = AVRCP_SendResponse(&(avrcptgContext->channels[channelId].channel), rspFrame);
	BTL_VERIFY_ERR((BT_STATUS_PENDING == status), status, ("Failed sending response"));

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


#if BT_SECURITY == XA_ENABLED
/*-------------------------------------------------------------------------------
 * BTL_AVRCPTG_SetSecurityLevel()
 */
BtStatus BTL_AVRCPTG_SetSecurityLevel(BtlAvrcptgContext *avrcptgContext,
								  		const BtSecurityLevel *securityLevel)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_AVRCPTG_SetSecurityLevel");

	BTL_VERIFY_ERR((0 != avrcptgContext), BT_STATUS_INVALID_PARM, ("Null avrcptgContext"));

	if (0 != securityLevel)
	{
		status = BTL_VerifySecurityLevel(*securityLevel);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Invalid AVRCPTG securityLevel"));
	}
	
	/* Apply the new security level */
	if(securityLevel == 0)
		avrcptgContext->securityRecord.level = BSL_AVRCPTG_DEFAULT;
	else
		avrcptgContext->securityRecord.level = (*securityLevel);


	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_AVRCPTG_GetSecurityLevel()
 */
BtStatus BTL_AVRCPTG_GetSecurityLevel(BtlAvrcptgContext *avrcptgContext,
								  		BtSecurityLevel *securityLevel)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_AVRCPTG_GetSecurityLevel");

	BTL_VERIFY_ERR((0 != avrcptgContext), BT_STATUS_INVALID_PARM, ("Null avrcptgContext"));
	BTL_VERIFY_ERR((0 != securityLevel), BT_STATUS_INVALID_PARM, ("Null securityLevel"));

	*securityLevel = avrcptgContext->securityRecord.level;
	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}
#endif	/* BT_SECURITY == XA_ENABLED */


/*-------------------------------------------------------------------------------
 * BtlAvrcptgDisable()
 *
 *		Internal function for deregistering security record and AVRCP channel.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		avrcptgContext [in] - pointer to the AVRCPTG context.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */
static BtStatus BtlAvrcptgDisable(BtlAvrcptgContext *avrcptgContext)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BtlAvrcptgDisable");

	/* First, try to deregister all channels */
	status = BtlAvrcptgDeregisterAll(avrcptgContext);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), BT_STATUS_FAILED, ("Failed deregistering all AVRCPTG channels"));

#if BT_SECURITY == XA_ENABLED

	/* Second, try to unregister security record */
	status = SEC_Unregister(&avrcptgContext->securityRecord);
	BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == status), ("Failed unregistering AVRCPTG security record"));
			
#endif	/* BT_SECURITY == XA_ENABLED */
	
	avrcptgContext->state = BTL_AVRCPTG_STATE_DISABLED;

	BTL_LOG_INFO(("BTL_AVRCPTG: Target is disabled."));

	BTL_FUNC_END();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BtlAvrcptgDeregisterAll()
 *
 *		Internal function for deregistering all AVRCP channels.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		avrcptgContext [in] - pointer to the AVRCPTG context.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */
static BtStatus BtlAvrcptgDeregisterAll(BtlAvrcptgContext *avrcptgContext)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U32 idx;

	BTL_FUNC_START("BtlAvrcptgDeregisterAll");

	/* Try to deregister all channels */
	for (idx = 0; idx<BTL_CONFIG_AVRCPTG_MAX_NUM_CHANNELS_PER_CONTEXT; idx++)
	{
		status = AVRCP_Deregister(&(avrcptgContext->channels[idx].channel));

		if (status == BT_STATUS_SUCCESS)
		{
			avrcptgContext->channels[idx].state = BTL_AVRCPTG_CHANNEL_STATE_IDLE;	
		}
		else
		{
			BTL_LOG_ERROR(("Failed deregistering AVRCP channel %d", idx));
			status = BT_STATUS_FAILED;
		}
	}

	BTL_FUNC_END();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BtlAvrcptgCallback()
 *
 *		Internal callback for handling AVRCP events in BTL layer.
 *		This function forwards events to registered application callback.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		chnl [in] - Internal AVRCP channel.
 *
 *		Parms [in] - Internal AVRCP event.
 *
 * Returns:
 *		void.
 */
static void BtlAvrcptgCallback(AvrcpChannel *chnl, AvrcpCallbackParms *Parms)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BOOL passEventToApp = TRUE;
	char addr[BDADDR_NTOA_SIZE];
	BtlAvrcptgContext *avrcptgContext;
	BtlAvrcptgChannel *btlChannel;
	U32 idx;
	BOOL sendDisableEvent = FALSE;
	BOOL notifyRadioOffComplete = FALSE;

	BTL_FUNC_START("BtlAvrcptgCallback");
	
	/* Find BTL AVRCPTG channel according to given AVRCP channel */
	btlChannel = ContainingRecord(chnl, BtlAvrcptgChannel, channel);

	/* Find context according to BTL AVRCPTG channel */
	avrcptgContext = btlChannel->context;

	/* Find channel index in the context */
	for (idx = 0; idx<BTL_CONFIG_AVRCPTG_MAX_NUM_CHANNELS_PER_CONTEXT; idx++)
	{
		if ((&(avrcptgContext->channels[idx])) == btlChannel)
			break;
	}

	BTL_VERIFY_ERR((idx != BTL_CONFIG_AVRCPTG_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_FAILED, ("No channel found for received event"));

	/* First handle special case of disabling */
	if (avrcptgContext->state == BTL_AVRCPTG_STATE_DISABLING)
	{
		switch (Parms->event)
		{
			case (AVRCP_EVENT_DISCONNECT):
			{
				U32 i;
				
				BTL_LOG_INFO(("BTL_AVRCPTG: Target channel %d is disconnected from %s.", idx, bdaddr_ntoa(&(Parms->p.remDev->bdAddr), addr)));

				btlChannel->state = BTL_AVRCPTG_CHANNEL_STATE_DISCONNECTED;

				/* Check that all channels are disconnected */
				for (i=0; i<BTL_CONFIG_AVRCPTG_MAX_NUM_CHANNELS_PER_CONTEXT; i++)
				{
					if (avrcptgContext->channels[i].state != BTL_AVRCPTG_CHANNEL_STATE_DISCONNECTED)
						break;
				}
				
				if (i == BTL_CONFIG_AVRCPTG_MAX_NUM_CHANNELS_PER_CONTEXT)
				{
					/* All channels are disconnected */
					
					if (avrcptgContext->disableState & BTL_AVRCPTG_DISABLE_STATE_MASK_DISABLE_ACTIVE)
					{				

					BtlAvrcptgDisable(avrcptgContext);
					sendDisableEvent = TRUE;
				}
					else if (avrcptgContext->disableState & BTL_AVRCPTG_DISABLE_STATE_MASK_ENABLED)
					{
						avrcptgContext->state = BTL_AVRCPTG_STATE_ENABLED;
					}
					
					if (avrcptgContext->disableState & BTL_AVRCPTG_DISABLE_STATE_MASK_RADIO_OFF_ACTIVE)
					{
						/* Verify all channels are disconnected */
						BtlContext *base;
						BtlAvrcptgContext *context;

						notifyRadioOffComplete = TRUE;

						IterateList(btlAvrcptgData.contextsList, base, BtlContext *)
						{
							context = (BtlAvrcptgContext *)base;

							for (i = 0; i < BTL_CONFIG_AVRCPTG_MAX_NUM_CHANNELS_PER_CONTEXT; i++)
							{
								if (context->channels[i].state != BTL_AVRCPTG_CHANNEL_STATE_DISCONNECTED)
									notifyRadioOffComplete = FALSE;
							}
						}
					}

					avrcptgContext->disableState = BTL_AVRCPTG_DISABLE_STATE_MASK_NONE;
				}

                /* Pass event to the BSC module */
                BtlAvrcptgSendEventToBsc(BTL_BSC_EVENT_SLC_DISCONNECTED,
                                         &Parms->p.remDev->bdAddr,
                                         avrcptgContext);

				/* Pass event to app. */
				break;
			}

			case (AVRCP_EVENT_CONNECT_IND):
			{
				/* Transport lower layer connection (L2CAP) request has been received */
				BTL_LOG_INFO(("BTL_AVRCPTG: Target channel %d received connect indication from %s indication while disabling, rejecting request...", idx, bdaddr_ntoa(&(Parms->p.remDev->bdAddr), addr)));

				status = AVRCP_ConnectRsp(&(avrcptgContext->channels[idx].channel), FALSE);
				BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == status), ("Failed responding to open indication while disabling."));

				/* Do not pass event to app, since we already rejected the incoming open indication. */
				passEventToApp = FALSE;
				break;
			}

			case (AVRCP_EVENT_CONNECT):
			{
				/* Transport lower layer connection (L2CAP) has been established */
				BTL_LOG_INFO(("BTL_AVRCPTG: Target channel %d is connected to %s while disabling, closing channel...", idx, bdaddr_ntoa(&(Parms->p.remDev->bdAddr), addr)));

				status = AVRCP_Disconnect(&(avrcptgContext->channels[idx].channel));
				BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == status), ("Failed disconnecting channel %d while disabling.", idx));

				if (BT_STATUS_PENDING == status)
					avrcptgContext->state = BTL_AVRCPTG_CHANNEL_STATE_DISCONNECTING;

				/* Do not pass event to app, since we are closing the connection... */
				passEventToApp = FALSE;

                /* Pass event to the BSC module */
                BtlAvrcptgSendEventToBsc(BTL_BSC_EVENT_SLC_CONNECTED,
                                         &Parms->p.remDev->bdAddr,
                                         avrcptgContext);
				break;
			}
			default:
			{
				BTL_LOG_ERROR(("BTL_AVRCPTG: Received unexpected event %d while disabling!", Parms->event));	
				
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
			case (AVRCP_EVENT_CONNECT):
			{
				/* Transport lower layer connection (L2CAP) has been established */
				btlChannel->state = BTL_AVRCPTG_CHANNEL_STATE_CONNECTED;
				btlChannel->remDev = Parms->p.remDev;
				BTL_LOG_INFO(("BTL_AVRCPTG: Target channel %d is connected to %s.", idx, bdaddr_ntoa(&(Parms->p.remDev->bdAddr), addr)));
                
                /* Pass event to the BSC module */
                BtlAvrcptgSendEventToBsc(BTL_BSC_EVENT_SLC_CONNECTED,
                                         &Parms->p.remDev->bdAddr,
                                         avrcptgContext);
				
				break;
			}
			case (AVRCP_EVENT_CONNECT_IND):
			{
				/* Transport lower layer connection (L2CAP) request has been received */
				btlChannel->state = BTL_AVRCPTG_CHANNEL_STATE_CONNECT_IND;
				BTL_LOG_INFO(("BTL_AVRCPTG: Target channel %d received connect indication from %s.", idx, bdaddr_ntoa(&(Parms->p.remDev->bdAddr), addr)));
				break;
			}
			case (AVRCP_EVENT_DISCONNECT):
			{
				/* Transport lower layer connection (L2CAP) has been disconnected */
				btlChannel->state = BTL_AVRCPTG_CHANNEL_STATE_DISCONNECTED;
				btlChannel->remDev = 0;
				BTL_LOG_INFO(("BTL_AVRCPTG: Target channel %d is disconnected from %s.", idx, bdaddr_ntoa(&(Parms->p.remDev->bdAddr), addr)));

                /* Pass event to the BSC module */
                BtlAvrcptgSendEventToBsc(BTL_BSC_EVENT_SLC_DISCONNECTED,
                                         &Parms->p.remDev->bdAddr,
                                         avrcptgContext);
                
				break;
			}
			case (AVRCP_EVENT_COMMAND):
			{
				/* AV/C Command frame was received */
				BTL_LOG_INFO(("BTL_AVRCPTG: Target channel %d received AV/C Command, Ctype: \"%s\", OpCode: \"%s\".", 
					idx,
					BtlAvrcptgCtype(Parms->p.cmdFrame->ctype), 
					BtlAvrcptgOpCode(Parms->p.cmdFrame->opcode)));

				BTL_LOG_INFO(("BTL_AVRCPTG: Subunit Type: \"%s\", Subunit ID: \"%s\".", 
					BtlAvrcptgSubunitType(Parms->p.cmdFrame->subunitType), 
					BtlAvrcptgSubunitId(Parms->p.cmdFrame->subunitId)));
				break;
			}
			case (AVRCP_EVENT_TX_DONE):
			{
				/* AV/C Response frame was sent */
				BTL_LOG_INFO(("BTL_AVRCPTG: Target channel %d sent AV/C Response, response: \"%s\", OpCode: \"%s\".", 
					idx,
					BtlAvrcptgResponse(Parms->p.rspFrame->response), 
					BtlAvrcptgOpCode(Parms->p.rspFrame->opcode)));

				BTL_LOG_INFO(("BTL_AVRCPTG: Subunit Type: \"%s\", Subunit ID: \"%s\".", 
					BtlAvrcptgSubunitType(Parms->p.rspFrame->subunitType), 
					BtlAvrcptgSubunitId(Parms->p.rspFrame->subunitId)));
				break;
			}
			case (AVRCP_EVENT_PANEL_PRESS):
			{
				/* Key has been pressed on the CT */
				BTL_LOG_INFO(("BTL_AVRCPTG: Target channel %d, key \"%s\" has been pressed.", 
					idx,
					BtlAvrcptgPanelOperation(Parms->p.panelInd.operation)));
				break;
			}
			case (AVRCP_EVENT_PANEL_HOLD):
			{
				/* Key has been held down on the CT */
				BTL_LOG_INFO(("BTL_AVRCPTG: Target channel %d, key \"%s\" has been held down.", 
					idx,
					BtlAvrcptgPanelOperation(Parms->p.panelInd.operation)));
				break;
			}
			case (AVRCP_EVENT_PANEL_RELEASE):
			{
				/* Key has been released on the CT */
				BTL_LOG_INFO(("BTL_AVRCPTG: Target channel %d, key \"%s\" has been released.", 
					idx,
					BtlAvrcptgPanelOperation(Parms->p.panelInd.operation)));
				break;
			}
		}	
	}

	if (TRUE == passEventToApp)
	{
		/* Set the context in the event passed to app */
		btlAvrcptgData.event.avrcptgContext = avrcptgContext;

		/* Set the channel index in the event passed to app */
		btlAvrcptgData.event.channelId = idx;

		/* Set the internal event in the event passed to app */
		btlAvrcptgData.event.avrcptgEvent = Parms;

		/* Pass the event to app */
		avrcptgContext->callback(&btlAvrcptgData.event);
	}

	if (TRUE == sendDisableEvent)
	{
		Parms->event = AVRCP_EVENT_DISABLED;
		
		/* Pass the event to app */
		avrcptgContext->callback(&btlAvrcptgData.event);
	}


	if (TRUE == notifyRadioOffComplete)
	{
		BtlModuleNotificationCompletionEvent moduleCompletionEvent;
		moduleCompletionEvent.type = BTL_MODULE_NOTIFICATION_COMPLETION_EVENT_TYPE_RADIO_OFF;
		moduleCompletionEvent.status = BT_STATUS_SUCCESS;
		
		BTL_ModuleCompleted(BTL_MODULE_TYPE_AVRCPTG, &moduleCompletionEvent);
	}


	BTL_FUNC_END();
}


/*-------------------------------------------------------------------------------
 * BtlAvrcptgCtype()
 *
 *		Return a pointer to a description of the AV/C Frame Ctype.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		msgCtype [in] - AV/C Frame Ctype.
 *
 * Returns:
 *		ASCII String pointer.
 */
static const char *BtlAvrcptgCtype(U8 msgCtype)
{
    switch (msgCtype) 
	{
    case AVRCP_CTYPE_CONTROL:
        return "Control";
    case AVRCP_CTYPE_STATUS:
        return "Status";
    case AVRCP_CTYPE_SPECIFIC_INQUIRY:
        return "Specific Inquiry";
    case AVRCP_CTYPE_NOTIFY:
        return "Notify";
    case AVRCP_CTYPE_GENERAL_INQUIRY:
        return "General Inquiry";
    }
    return "UNKNOWN";
}


/*-------------------------------------------------------------------------------
 * BtlAvrcptgOpCode()
 *
 *		Return a pointer to a description of the AV/C Frame OpCode.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		msgOpCode [in] - AV/C Frame OpCode.
 *
 * Returns:
 *		ASCII String pointer.
 */
static const char *BtlAvrcptgOpCode(U8 msgOpCode)
{
    switch (msgOpCode) 
	{
    case AVRCP_OPCODE_VENDOR_DEPENDENT:
        return "Vendor Dependent";
    case AVRCP_OPCODE_UNIT_INFO:
        return "Unit Info";
    case AVRCP_OPCODE_SUBUNIT_INFO:
        return "Subunit Info";
    case AVRCP_OPCODE_PASS_THROUGH:
        return "Pass Through";
    }
    return "UNKNOWN";
}


/*-------------------------------------------------------------------------------
 * BtlAvrcptgResponse()
 *
 *		Return a pointer to a description of the AV/C Frame Response.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		msgResponse [in] - AV/C Frame Response.
 *
 * Returns:
 *		ASCII String pointer.
 */
static const char *BtlAvrcptgResponse(U8 msgResponse)
{
    switch (msgResponse) 
	{
    case AVRCP_RESPONSE_NOT_IMPLEMENTED:
        return "Not Implemented";
    case AVRCP_RESPONSE_ACCEPTED:
        return "Accepted";
    case AVRCP_RESPONSE_REJECTED:
        return "Rejected";
    case AVRCP_RESPONSE_IN_TRANSITION:
        return "In Transition";
    case AVRCP_RESPONSE_IMPLEMENTED_STABLE:
        return "Implemented Stable";
	case AVRCP_RESPONSE_CHANGED:
        return "Changed";
    case AVRCP_RESPONSE_INTERIM:
        return "Interim";
    }
    return "UNKNOWN";
}


/*-------------------------------------------------------------------------------
 * BtlAvrcptgPanelOperation()
 *
 *		Return a pointer to a description of the panel operation.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		msgPanelOperation [in] - panel operation.
 *
 * Returns:
 *		ASCII String pointer.
 */
static const char *BtlAvrcptgPanelOperation(AvrcpPanelOperation msgPanelOperation)
{
    switch (msgPanelOperation) 
	{
	case AVRCP_POP_0:
        return "0";
    case AVRCP_POP_1:
        return "1";
    case AVRCP_POP_2:
        return "2";
    case AVRCP_POP_3:
        return "3";
    case AVRCP_POP_4:
        return "4";
	case AVRCP_POP_5:
        return "5";
    case AVRCP_POP_6:
        return "6";
	case AVRCP_POP_7:
        return "7";
    case AVRCP_POP_8:
        return "8";
    case AVRCP_POP_9:
        return "9";
    case AVRCP_POP_DOT:
        return "DOT";
    case AVRCP_POP_ENTER:
        return "ENTER";
	case AVRCP_POP_CLEAR:
        return "CLEAR";
		
    case AVRCP_POP_POWER:
        return "POWER";
    case AVRCP_POP_VOLUME_UP:
        return "VOLUME UP";
    case AVRCP_POP_VOLUME_DOWN:
        return "VOLUME DOWN";
    case AVRCP_POP_MUTE:
        return "MUTE";
    case AVRCP_POP_PLAY:
        return "PLAY";
	case AVRCP_POP_STOP:
        return "STOP";
    case AVRCP_POP_PAUSE:
        return "PAUSE";
	case AVRCP_POP_RECORD:
        return "RECORD";
    case AVRCP_POP_REWIND:
        return "REWIND";
    case AVRCP_POP_FAST_FORWARD:
        return "FAST FORWARD";
    case AVRCP_POP_EJECT:
        return "EJECT";
    case AVRCP_POP_FORWARD:
        return "FORWARD";
	case AVRCP_POP_BACKWARD:
        return "BACKWARD";

	case AVRCP_POP_F1:
        return "F1";
    case AVRCP_POP_F2:
        return "F2";
    case AVRCP_POP_F3:
        return "F3";
    case AVRCP_POP_F4:
        return "F4";
	case AVRCP_POP_F5:
        return "F5";
    }
    return "UNKNOWN";
}


/*-------------------------------------------------------------------------------
 * BtlAvrcptgSubunitType()
 *
 *		Return a pointer to a description of the AV/C Command Subunit Type.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		msgSubunitType [in] - AV/C Command Subunit Type.
 *
 * Returns:
 *		ASCII String pointer.
 */
static const char *BtlAvrcptgSubunitType(U8 msgSubunitType)
{
    switch (msgSubunitType) 
	{
    case SUBUNIT_TYPE_MONITOR:
        return "Monitor";
    case SUBUNIT_TYPE_AUDIO:
        return "Audio";
    case SUBUNIT_TYPE_PRINTER:
        return "Printer";
    case SUBUNIT_TYPE_DISC:
        return "Disc";
    case SUBUNIT_TYPE_TAPE_RECORDER_PLAYER:
        return "Tape recorder/player";
	case SUBUNIT_TYPE_TUNER:
        return "Tuner";
    case SUBUNIT_TYPE_CA:
        return "CA";
    case SUBUNIT_TYPE_CAMERA:
        return "Camera";
    case SUBUNIT_TYPE_PANEL:
        return "Panel";
    case SUBUNIT_TYPE_BULLETIN_BOARD:
        return "Bulletin board";
	case SUBUNIT_TYPE_CAMERA_STORAGE:
        return "Camera storage";
    case SUBUNIT_TYPE_VENDOR_UNIQUE:
        return "Vendor unique";
    case SUBUNIT_TYPE_ALL_SUBUNIT_TYPES:
        return "All subunit types";
    case SUBUNIT_TYPE_EXTENDED_TO_NEXT_BYTE:
        return "Extended to next byte";
    case SUBUNIT_TYPE_UNIT:
        return "Unit";
    }
    return "UNKNOWN";
}


/*-------------------------------------------------------------------------------
 * BtlAvrcptgSubunitId()
 *
 *		Return a pointer to a description of the AV/C Command Subunit Id.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		msgSubunitId [in] - AV/C Command Subunit Id.
 *
 * Returns:
 *		ASCII String pointer.
 */
static const char *BtlAvrcptgSubunitId(U8 msgSubunitId)
{
    switch (msgSubunitId) 
	{
    case SUBUNIT_ID_INSTANCE_0:
        return "Instance 0";
    case SUBUNIT_ID_INSTANCE_1:
        return "Instance 1";
    case SUBUNIT_ID_INSTANCE_2:
        return "Instance 2";
    case SUBUNIT_ID_INSTANCE_3:
        return "Instance 3";
    case SUBUNIT_ID_INSTANCE_4:
        return "Instance 4";
	case SUBUNIT_ID_EXTENDED_TO_NEXT_BYTE:
        return "Extended to next byte";
    case SUBUNIT_ID_ALL_INSTANCES:
        return "All instances";
    case SUBUNIT_ID_IGNORE:
        return "Ignore (addressing a unit)";
    }
    return "UNKNOWN";
}



BtStatus BtlAvrcptgBtlNotificationsCb(BtlModuleNotificationType notificationType)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U32 idx;
	BtlAvrcptgContext *avrcptgContext;
	BtlContext *base;

	BTL_FUNC_START("BtlAvrcptgBtlNotificationsCb");
	
	BTL_LOG_DEBUG(("Notification Type: %d", notificationType));
 
	switch (notificationType)
	{
		case BTL_MODULE_NOTIFICATION_RADIO_ON:

			/* Do nothing */
		
			break;

		case BTL_MODULE_NOTIFICATION_RADIO_OFF:

			IterateList(btlAvrcptgData.contextsList, base, BtlContext *)
			{
				avrcptgContext = (BtlAvrcptgContext *)base;

				switch (avrcptgContext->state)
				{
					case BTL_AVRCPTG_STATE_ENABLED:

 						for (idx = 0; idx< BTL_CONFIG_AVRCPTG_MAX_NUM_CHANNELS_PER_CONTEXT; idx++)
 						{
 							switch (avrcptgContext->channels[idx].state)
 							{
 								case (BTL_AVRCPTG_CHANNEL_STATE_DISCONNECTING):
 								{
 									/* AVRCPTG state is now in the process of disabling */
  									avrcptgContext->state = BTL_AVRCPTG_STATE_DISABLING;
 									break;
 								}
 								case (BTL_AVRCPTG_CHANNEL_STATE_CONNECTED):
 								{	
 									
  									status = AVRCP_Disconnect(&(avrcptgContext->channels[idx].channel));
 
 									if (BT_STATUS_PENDING == status)
 									{
  										avrcptgContext->channels[idx].state = BTL_AVRCPTG_CHANNEL_STATE_DISCONNECTING;
 										
 										/* AVRCPTG state is now in the process of disabling */
  										avrcptgContext->state = BTL_AVRCPTG_STATE_DISABLING;
 									}
 									
 									break;
 								}
 								case (BTL_AVRCPTG_CHANNEL_STATE_CONNECTING):
 								{
 									/* In process of connecting, let it finish and then close the stream */
 
 									/* AVRCPTG state is now in the process of disabling */
  									avrcptgContext->state = BTL_AVRCPTG_STATE_DISABLING;
 									break;
 								}
 								case (BTL_AVRCPTG_CHANNEL_STATE_CONNECT_IND):
 								{
 									/* Reject incoming open request, and wait for close event */
 
  									status = AVRCP_ConnectRsp(&(avrcptgContext->channels[idx].channel), FALSE);
  									BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == status), ("Failed responding to open indication while disabling."));
 
 									if (BT_STATUS_PENDING == status)
  										avrcptgContext->state = BTL_AVRCPTG_STATE_DISABLING;
 									
 									break;
 								}							
 							}
 						}
 
 						if (BTL_AVRCPTG_STATE_DISABLING != avrcptgContext->state)
 						{
 							/* All streams are disconnected */
 							break;
 						}

						/* Remember that context was enabled */
						avrcptgContext->disableState |= BTL_AVRCPTG_DISABLE_STATE_MASK_ENABLED;
						
						/* Wait for disconnect event, pass through to next case... */

					case BTL_AVRCPTG_STATE_DISABLING:
						
 						avrcptgContext->disableState |= BTL_AVRCPTG_DISABLE_STATE_MASK_RADIO_OFF_ACTIVE;
 						status = BT_STATUS_PENDING;
						break;
				}
			}/*IterateList(btlAvrcptgData.contextsList, base, BtlContext *)*/
		
			break; /*case BTL_MODULE_NOTIFICATION_RADIO_OFF:*/

		default:

			BTL_ERR(BT_STATUS_INTERNAL_ERROR, ("Invalid notification (%d)", notificationType));
			


	}; /*switch (notificationType)*/

	BTL_FUNC_END();
	
	return (status);
}

#if AVRCP_METADATA_TARGET == XA_ENABLED

/*-------------------------------------------------------------------------------
 * BTL_AVRCPTG_MDA_SetEventMask()
 */
BtStatus BTL_AVRCPTG_MDA_SetEventMask(BtlAvrcptgContext *avrcptgContext, 
										AvrcpMetadataEventMask EventMask)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U32 channelId;
	
	BTL_FUNC_START_AND_LOCK("BTL_AVRCPTG_MDA_SetEventMask");

	BTL_VERIFY_ERR((0 != avrcptgContext), BT_STATUS_INVALID_PARM, ("Null avrcptgContext"));

	for (channelId = 0; channelId<BTL_CONFIG_AVRCPTG_MAX_NUM_CHANNELS_PER_CONTEXT; channelId++)
	{
		status = AVRCP_TgSetMdaEventMask(&(avrcptgContext->channels[channelId].channel), EventMask);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed setting EventMask on channel %d", channelId));
	}


	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

/*---------------------------------------------------------------------------
 * BTL_AVRCPTG_MDA_SetPlayerSettingsMask()
 */
BtStatus BTL_AVRCPTG_MDA_SetPlayerSettingsMask(BtlAvrcptgContext *avrcptgContext, 
                                          		AvrcpPlayerAttrIdMask PlayerAttrIdmask)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U32 channelId;
	
	BTL_FUNC_START_AND_LOCK("BTL_AVRCPTG_MDA_SetPlayerSettingsMask");

	BTL_VERIFY_ERR((0 != avrcptgContext), BT_STATUS_INVALID_PARM, ("Null avrcptgContext"));

	for (channelId = 0; channelId<BTL_CONFIG_AVRCPTG_MAX_NUM_CHANNELS_PER_CONTEXT; channelId++)
	{
		status = AVRCP_TgSetMdaPlayerSettingsMask(&(avrcptgContext->channels[channelId].channel), PlayerAttrIdmask);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed setting PlayerSettingsMask on channel %d", channelId));
	}

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*---------------------------------------------------------------------------
 * BTL_AVRCPTG_MDA_SetMediaAttributesMask()
 */
BtStatus BTL_AVRCPTG_MDA_SetMediaAttributesMask(BtlAvrcptgContext *avrcptgContext, 
                                          		AvrcpMediaAttrIdMask mediaAttrIdmask)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U32 channelId;
	
	BTL_FUNC_START_AND_LOCK("BTL_AVRCPTG_MDA_SetMediaAttributesMask");

	BTL_VERIFY_ERR((0 != avrcptgContext), BT_STATUS_INVALID_PARM, ("Null avrcptgContext"));

	for (channelId = 0; channelId<BTL_CONFIG_AVRCPTG_MAX_NUM_CHANNELS_PER_CONTEXT; channelId++)
	{
		status = AVRCP_TgSetMdaMediaAttributesMask(&(avrcptgContext->channels[channelId].channel), mediaAttrIdmask);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed setting MediaAttributesMask on channel %d", channelId));
	}

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

/*---------------------------------------------------------------------------
 * BTL_AVRCPTG_MDA_SetPlayerSetting()
 */
BtStatus BTL_AVRCPTG_MDA_SetPlayerSetting(BtlAvrcptgContext *avrcptgContext, 
                                     		AvrcpPlayerSetting *setting) 
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtStatus retVal;
	U32 channelId;
	BOOL channelExists = FALSE;
	
	BTL_FUNC_START_AND_LOCK("BTL_AVRCPTG_MDA_SetPlayerSetting");

	BTL_VERIFY_ERR((0 != avrcptgContext), BT_STATUS_INVALID_PARM, ("Null avrcptgContext"));
	BTL_VERIFY_ERR((0 != setting), BT_STATUS_INVALID_PARM, ("Null PlayerSetting"));

	for (channelId = 0; channelId<BTL_CONFIG_AVRCPTG_MAX_NUM_CHANNELS_PER_CONTEXT; channelId++)
	{
		if (avrcptgContext->channels[channelId].state == BTL_AVRCPTG_CHANNEL_STATE_CONNECTED)
		{
			channelExists = TRUE;
			retVal = AVRCP_TgSetMdaPlayerSetting(&(avrcptgContext->channels[channelId].channel), setting);
			BTL_VERIFY_ERR(((BT_STATUS_PENDING == retVal) || (BT_STATUS_SUCCESS == retVal)), retVal, ("Failed setting PlayerSetting on channel %d", channelId));
			if (retVal == BT_STATUS_PENDING)
			{
				status = BT_STATUS_PENDING; 
			}
		}
	}
	
	if (channelExists == FALSE)
	{
		status = BT_STATUS_FAILED;
	}

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

/*---------------------------------------------------------------------------
 * BTL_AVRCPTG_MDA_SetPlayerStrings()
  */
BtStatus BTL_AVRCPTG_MDA_SetPlayerStrings(BtlAvrcptgContext *avrcptgContext,
                                     AvrcpPlayerAttrId attrId,
                                     AvrcpPlayerStrings *strings)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U32 channelId;
	
	BTL_FUNC_START_AND_LOCK("BTL_AVRCPTG_MDA_SetPlayerStrings");

	BTL_VERIFY_ERR((0 != avrcptgContext), BT_STATUS_INVALID_PARM, ("Null avrcptgContext"));
	BTL_VERIFY_ERR((0 != strings), BT_STATUS_INVALID_PARM, ("Null strings"));

	for (channelId = 0; channelId<BTL_CONFIG_AVRCPTG_MAX_NUM_CHANNELS_PER_CONTEXT; channelId++)
	{
		status = AVRCP_TgSetMdaPlayerStrings(&(avrcptgContext->channels[channelId].channel), attrId, strings);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed setting PlayerStrings on channel %d", channelId));
	}

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

/*---------------------------------------------------------------------------
 * BTL_AVRCPTG_MDA_SetMediaInfo()
 */
BtStatus BTL_AVRCPTG_MDA_SetMediaInfo(BtlAvrcptgContext *avrcptgContext,
									  U8 numElements,
									  AvrcpMediaInfo *info)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U32 channelId;
	
	BTL_FUNC_START_AND_LOCK("BTL_AVRCPTG_MDA_SetMediaInfo");

	BTL_VERIFY_ERR((0 != avrcptgContext), BT_STATUS_INVALID_PARM, ("Null avrcptgContext"));
	BTL_VERIFY_ERR((0 != info), BT_STATUS_INVALID_PARM, ("Null info"));

	for (channelId = 0; channelId<BTL_CONFIG_AVRCPTG_MAX_NUM_CHANNELS_PER_CONTEXT; channelId++)
	{
		status = AVRCP_TgSetMdaMediaInfo(&(avrcptgContext->channels[channelId].channel), numElements, info);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed setting MediaInfo on channel %d", channelId));
	}

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

/*---------------------------------------------------------------------------
 * BTL_AVRCPTG_MDA_SetPlayStatus()
 */
BtStatus BTL_AVRCPTG_MDA_SetPlayStatus(BtlAvrcptgContext *avrcptgContext,
									   AvrcpMediaStatus mediaStatus)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U32 channelId;
	BtStatus retVal;
	BOOL channelExists = FALSE;

	BTL_FUNC_START_AND_LOCK("BTL_AVRCPTG_MDA_SetPlayStatus");

	BTL_VERIFY_ERR((0 != avrcptgContext), BT_STATUS_INVALID_PARM, ("Null avrcptgContext"));

	for (channelId = 0; channelId<BTL_CONFIG_AVRCPTG_MAX_NUM_CHANNELS_PER_CONTEXT; channelId++)
	{
		if (avrcptgContext->channels[channelId].state == BTL_AVRCPTG_CHANNEL_STATE_CONNECTED)
		{
			channelExists = TRUE;
			retVal = AVRCP_TgSetMdaPlayStatus(&(avrcptgContext->channels[channelId].channel), mediaStatus);
			BTL_VERIFY_ERR(((BT_STATUS_PENDING == retVal) || (BT_STATUS_SUCCESS == retVal)), retVal, ("Failed setting PlayStatus on channel %d", channelId));
			if (retVal == BT_STATUS_PENDING)
			{
				status = BT_STATUS_PENDING; 
			}
		}
	}
	
	if (channelExists == FALSE)
	{
		status = BT_STATUS_FAILED;
	}

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

/*---------------------------------------------------------------------------
 * BTL_AVRCPTG_MDA_SetPlayPosition()
 */
BtStatus BTL_AVRCPTG_MDA_SetPlayPosition(BtlAvrcptgContext *avrcptgContext,
										 U32 position,
										 U32 length)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtStatus retVal;
	U32 channelId;
	BOOL channelExists = FALSE;
	
	BTL_FUNC_START_AND_LOCK("BTL_AVRCPTG_MDA_SetPlayPosition");

	BTL_VERIFY_ERR((0 != avrcptgContext), BT_STATUS_INVALID_PARM, ("Null avrcptgContext"));

	for (channelId = 0; channelId<BTL_CONFIG_AVRCPTG_MAX_NUM_CHANNELS_PER_CONTEXT; channelId++)
	{
		if (avrcptgContext->channels[channelId].state == BTL_AVRCPTG_CHANNEL_STATE_CONNECTED)
		{
			channelExists = TRUE;
			retVal = AVRCP_TgSetMdaPlayPosition(&(avrcptgContext->channels[channelId].channel), position, length);
			BTL_VERIFY_ERR(((BT_STATUS_PENDING == retVal) || (BT_STATUS_SUCCESS == retVal)), retVal, ("Failed setting PlayPosition on channel %d", channelId));
			if (retVal == BT_STATUS_PENDING)
			{
				status = BT_STATUS_PENDING; 
			}
		}
	}
	
	if (channelExists == FALSE)
	{
		status = BT_STATUS_FAILED;
	}

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

/*---------------------------------------------------------------------------
 * BTL_AVRCPTG_MDA_SetBattStatus()
 */
BtStatus BTL_AVRCPTG_MDA_SetBattStatus(BtlAvrcptgContext *avrcptgContext,
									   AvrcpBatteryStatus battStatus)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtStatus retVal;
	U32 channelId;
	BOOL channelExists = FALSE;
	
	BTL_FUNC_START_AND_LOCK("BTL_AVRCPTG_MDA_SetBattStatus");

	BTL_VERIFY_ERR((0 != avrcptgContext), BT_STATUS_INVALID_PARM, ("Null avrcptgContext"));

	for (channelId = 0; channelId<BTL_CONFIG_AVRCPTG_MAX_NUM_CHANNELS_PER_CONTEXT; channelId++)
	{
		if (avrcptgContext->channels[channelId].state == BTL_AVRCPTG_CHANNEL_STATE_CONNECTED)
		{
			channelExists = TRUE;
			retVal = AVRCP_TgSetMdaBattStatus(&(avrcptgContext->channels[channelId].channel), battStatus);
			BTL_VERIFY_ERR(((BT_STATUS_PENDING == retVal) || (BT_STATUS_SUCCESS == retVal)), retVal,("Failed setting BattStatus on channel %d", channelId));
			if (retVal == BT_STATUS_PENDING)
			{
				status = BT_STATUS_PENDING; 
			}
		}
	}
	
	if (channelExists == FALSE)
	{
		status = BT_STATUS_FAILED;
	}

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

/*---------------------------------------------------------------------------
 * BTL_AVRCPTG_MDA_SetSystemStatus()
  */
BtStatus BTL_AVRCPTG_MDA_SetSystemStatus(BtlAvrcptgContext *avrcptgContext, AvrcpSystemStatus sysStatus)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtStatus retVal;
	U32 channelId;
	BOOL channelExists = FALSE;
	
	BTL_FUNC_START_AND_LOCK("BTL_AVRCPTG_MDA_SetSystemStatus");

	BTL_VERIFY_ERR((0 != avrcptgContext), BT_STATUS_INVALID_PARM, ("Null avrcptgContext"));

	for (channelId = 0; channelId<BTL_CONFIG_AVRCPTG_MAX_NUM_CHANNELS_PER_CONTEXT; channelId++)
	{
		if (avrcptgContext->channels[channelId].state == BTL_AVRCPTG_CHANNEL_STATE_CONNECTED)
		{
			channelExists = TRUE;
			retVal = AVRCP_TgSetMdaSystemStatus(&(avrcptgContext->channels[channelId].channel), sysStatus);
			BTL_VERIFY_ERR(((BT_STATUS_PENDING == retVal) || (BT_STATUS_SUCCESS == retVal)), retVal, ("Failed setting SystemStatus on channel %d", channelId));
			if (retVal == BT_STATUS_PENDING)
			{
				status = BT_STATUS_PENDING; 
			}
		}
	}
	
	if (channelExists == FALSE)
	{
		status = BT_STATUS_FAILED;
	}

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

/*-------------------------------------------------------------------------------
 * BTL_AVRCPTG_MDA_SetTrack()
 */
BtStatus BTL_AVRCPTG_MDA_SetTrack(BtlAvrcptgContext *avrcptgContext,U32 track)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtStatus retVal;
 	U32 channelId = 0;
	BOOL channelExists = FALSE;
	
	BTL_FUNC_START_AND_LOCK("BTL_AVRCPTG_MDA_SetTrack");

	BTL_VERIFY_ERR((0 != avrcptgContext), BT_STATUS_INVALID_PARM, ("Null avrcptgContext"));

	for (channelId = 0; channelId<BTL_CONFIG_AVRCPTG_MAX_NUM_CHANNELS_PER_CONTEXT; channelId++)
	{
		if (avrcptgContext->channels[channelId].state == BTL_AVRCPTG_CHANNEL_STATE_CONNECTED)
		{
			channelExists = TRUE;
			retVal = AVRCP_TgSetMdaTrack(&(avrcptgContext->channels[channelId].channel), track);
			BTL_VERIFY_ERR(((BT_STATUS_PENDING == retVal) || (BT_STATUS_SUCCESS == retVal)), retVal, ("Failed setting track on channel %d", channelId));
			if (retVal == BT_STATUS_PENDING)
			{
				status = BT_STATUS_PENDING; 
			}
		}
	}
	
	if (channelExists == FALSE)
	{
		status = BT_STATUS_FAILED;
	}

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

/*---------------------------------------------------------------------------
 * BTL_AVRCPTG_MDA_SignalTrackStart()
  */
BtStatus BTL_AVRCPTG_MDA_SignalTrackStart(BtlAvrcptgContext *avrcptgContext)

{
	BtStatus status = BT_STATUS_SUCCESS;
	BtStatus retVal;
	U32 channelId;
	BOOL channelExists = FALSE;
	
	BTL_FUNC_START_AND_LOCK("BTL_AVRCPTG_MDA_SignalTrackStart");

	BTL_VERIFY_ERR((0 != avrcptgContext), BT_STATUS_INVALID_PARM, ("Null avrcptgContext"));

	for (channelId = 0; channelId<BTL_CONFIG_AVRCPTG_MAX_NUM_CHANNELS_PER_CONTEXT; channelId++)
	{
		if (avrcptgContext->channels[channelId].state == BTL_AVRCPTG_CHANNEL_STATE_CONNECTED)
		{
			channelExists = TRUE;
			retVal = AVRCP_TgSignalMdaTrackStart(&(avrcptgContext->channels[channelId].channel));
			BTL_VERIFY_ERR(((BT_STATUS_PENDING == retVal) || (BT_STATUS_SUCCESS == retVal)), retVal, ("Failed Signaling Track Start on channel %d", channelId));
			if (retVal == BT_STATUS_PENDING)
			{
				status = BT_STATUS_PENDING; 
			}
		}
	}
	
	if (channelExists == FALSE)
	{
		status = BT_STATUS_FAILED;
	}

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}
  
/*---------------------------------------------------------------------------
 * BTL_AVRCPTG_MDA_SignalTrackEnd()
 */
BtStatus BTL_AVRCPTG_MDA_SignalTrackEnd(BtlAvrcptgContext *avrcptgContext)

{
	BtStatus status = BT_STATUS_SUCCESS;
	BtStatus retVal;
	U32 channelId;
	BOOL channelExists = FALSE;
	
	BTL_FUNC_START_AND_LOCK("BTL_AVRCPTG_MDA_SignalTrackEnd");

	BTL_VERIFY_ERR((0 != avrcptgContext), BT_STATUS_INVALID_PARM, ("Null avrcptgContext"));

	for (channelId = 0; channelId<BTL_CONFIG_AVRCPTG_MAX_NUM_CHANNELS_PER_CONTEXT; channelId++)
	{
		if (avrcptgContext->channels[channelId].state == BTL_AVRCPTG_CHANNEL_STATE_CONNECTED)
		{
			channelExists = TRUE;
			retVal = AVRCP_TgSignalMdaTrackEnd(&(avrcptgContext->channels[channelId].channel));
			BTL_VERIFY_ERR(((BT_STATUS_PENDING == retVal) || (BT_STATUS_SUCCESS == retVal)), retVal, ("Failed Signaling Track End on channel %d", channelId));
			if (retVal == BT_STATUS_PENDING)
			{
				status = BT_STATUS_PENDING; 
			}
		}
	}
	
	if (channelExists == FALSE)
	{
		status = BT_STATUS_FAILED;
	}

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

/*---------------------------------------------------------------------------
 * BTL_AVRCPTG_MDA_SetCurrentCharSet()
 */
BtStatus BTL_AVRCPTG_MDA_SetCurrentCharSet(BtlAvrcptgContext *avrcptgContext, U16 charSet)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U32 channelId;
	
	BTL_FUNC_START_AND_LOCK("BTL_AVRCPTG_MDA_SetCurrentCharSet");

	BTL_VERIFY_ERR((0 != avrcptgContext), BT_STATUS_INVALID_PARM, ("Null avrcptgContext"));

	for (channelId = 0; channelId<BTL_CONFIG_AVRCPTG_MAX_NUM_CHANNELS_PER_CONTEXT; channelId++)
	{
		status = AVRCP_TgSetCurrentMdaCharSet(&(avrcptgContext->channels[channelId].channel), charSet);
 		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed Setting Current char Set on channel %d", channelId));
	}

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

/*---------------------------------------------------------------------------
 * BTL_AVRCPTG_MDA_GetCurrentCharSet()
 */
BtStatus BTL_AVRCPTG_MDA_GetCurrentCharSet(BtlAvrcptgContext *avrcptgContext,U16 *CharSet)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U32 channelId;
	
	BTL_FUNC_START_AND_LOCK("BTL_AVRCPTG_MDA_GetCurrentCharSet");

	BTL_VERIFY_ERR((0 != avrcptgContext), BT_STATUS_INVALID_PARM, ("Null avrcptgContext"));
	BTL_VERIFY_ERR((0 != CharSet), BT_STATUS_INVALID_PARM, ("Null CharSet"));

	/* Since CharSet is a context related parameter, it is identical for all channels*/
	channelId = 0;
    *CharSet = AVRCP_TgGetCurrentMdaCharSet(&(avrcptgContext->channels[channelId].channel));

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

/*---------------------------------------------------------------------------
 * BTL_AVRCPTG_MDA_SetResponseBuffer()
  */
BtStatus BTL_AVRCPTG_MDA_SetResponseBuffer(BtlAvrcptgContext *avrcptgContext, U8 *buffer, U16 bufLen)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U32 channelId;
	
	BTL_FUNC_START_AND_LOCK("BTL_AVRCPTG_MDA_SetResponseBuffer");

	BTL_VERIFY_ERR((0 != avrcptgContext), BT_STATUS_INVALID_PARM, ("Null avrcptgContext"));
	BTL_VERIFY_ERR((0 != buffer), BT_STATUS_INVALID_PARM, ("Null buffer"));

	for (channelId = 0; channelId<BTL_CONFIG_AVRCPTG_MAX_NUM_CHANNELS_PER_CONTEXT; channelId++)
	{
		status = AVRCP_SetMdaResponseBuffer(&(avrcptgContext->channels[channelId].channel), buffer, bufLen);
 		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed Setting Response Buffer on channel %d", channelId));
	}

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}
#endif /* AVRCP_METADATA_TARGET == XA_ENABLED */


/*---------------------------------------------------------------------------
 * BTL_AVRCPTG_BSC_SendRecommendedEvent()
 *
 *      Send recommended event to the AVRCP application
 */
BtStatus BTL_AVRCPTG_BSC_SendRecommendedEvent(BtlAvrcptgContext *avrcptgContext,
                                              AvrcpEvent recommendedEvent)
{
	AvrcpCallbackParms avrcptgEvent;
   	BtlAvrcptgEvent event;

    BTL_LOG_INFO(("BTL_AVRCPTG_BSC: Event %s sent",
                  BtlAvrcptgRecommendedEventName(recommendedEvent)));

	/* Set the internal type of event */
    avrcptgEvent.event = recommendedEvent;
    
    /* Set the context in the event passed to app */
	event.avrcptgContext = avrcptgContext;

	/* Set the internal event in the event passed to app */
	event.avrcptgEvent = &avrcptgEvent;

	/* Pass the event to app */
	avrcptgContext->callback(&event);

    return (BT_STATUS_SUCCESS);
}

/*-------------------------------------------------------------------------------
 * BtlAvrcptgSendEventToBsc()
 *
 *		Sends event to BTL_BSC module.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		eventType [in] - type of event to be sent.
 *
 *      bdAddr [in] - pointer to BD_ADDR structure.
 *
 *      avrcptgContext [in] - pointer to BtlAvrcptgContext structure.
 *
 * Returns:
 *		none
 */
static void BtlAvrcptgSendEventToBsc(BtlBscEventType eventType,
                                     BD_ADDR *bdAddr,
                                     BtlAvrcptgContext *avrcptgContext)
{
    BtlBscEvent bscEvent;

	BTL_FUNC_START("BtlAvrcptgSendEventToBsc");

    /* Fill event's data */
    bscEvent.type = eventType;
    bscEvent.source = BTL_BSC_EVENT_SOURCE_AVRCPTG;
    bscEvent.isBdAddrValid = TRUE;
    bscEvent.info.bdAddr = *bdAddr;
    bscEvent.info.context = avrcptgContext;

    BTL_BSC_SendEvent((const BtlBscEvent *)&bscEvent);

	BTL_FUNC_END();
}

/*-------------------------------------------------------------------------------
 * BtlAvrcptgRecommendedEventName()
 *
 *		Gets event's name.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	event - recommended event.
 *
 * Returns:
 *		String with event's name.
 */
static const char *BtlAvrcptgRecommendedEventName(AvrcpEvent event)
{
    switch (event)
    {
        case AVRCP_BSC_EVENT_RECOMMENDED_PAUSE:
            return "AVRCP_BSC_EVENT_RECOMMENDED_PAUSE";

        case AVRCP_BSC_EVENT_RECOMMENDED_RESUME_PLAYING:
            return "AVRCP_BSC_EVENT_RECOMMENDED_RESUME_PLAYING";

        default:
            return "AVRCP BSC unknown recommended event";
    }
}




#else /*BTL_CONFIG_AVRCPTG == BTL_CONFIG_ENABLED*/

/*-------------------------------------------------------------------------------
 * BTL_AVRCPTG_Init() - When  BTL_CONFIG_AVRCPTG is disabled.
 */
BtStatus BTL_AVRCPTG_Init(void)
{
    
   BTL_LOG_INFO(("BTL_AVRCPTG_Init()  -  BTL_CONFIG_AVRCPTG Disabled"));
  

    return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 * BTL_AVRCPTG_Deinit() - When  BTL_CONFIG_FTPC is disabled.
 */
BtStatus BTL_AVRCPTG_Deinit(void)
{
    BTL_LOG_INFO(("BTL_AVRCPTG_Deinit() -  BTL_CONFIG_AVRCPTG Disabled"));

    return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------*/



#endif /*BTL_CONFIG_AVRCPTG ==   BTL_CONFIG_ENABLED*/



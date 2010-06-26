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
*   FILE NAME:		btl_ag.c
*
*   DESCRIPTION:	This file contains the implementation of the BTL AG API.
*
*   AUTHOR:			Itay Klein
*
\*******************************************************************************/

#include "btl_config.h"
#include "btl_defs.h"
#include "btl_ag.h"

BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_AG);

#if BTL_CONFIG_AG == BTL_CONFIG_ENABLED

/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/


#include "btl_pool.h"
#include "btl_commoni.h"
#include "btl_bsc.h"
#include "debug.h"
#include "hci.h"



/********************************************************************************
 *
 * Constants 
 *
 *******************************************************************************/

#define FM_OVER_BT_MODE						(0xFD61)

/*-------------------------------------------------------------------------------
 * BTL_AG_MAX_CONTEXTS constant
 *
 *	The maximal number of AG Contexts.
 */
#define BTL_AG_MAX_CONTEXTS		2

/*-------------------------------------------------------------------------------
 * BTL_AG_MAX_RESPONSES constant
 *
 *	The maximal number of available responses.
 */
#define BTL_AG_MAX_RESPONSES	10

/*-------------------------------------------------------------------------------
 * BTL_AG_MAX_GAIN constant
 *
 *	The maximal gain the speaker or the microphone can have.
 */
#define BTL_AG_MAX_GAIN 15

/*-------------------------------------------------------------------------------
 * BTL_AG_DEFAULT_SECURITY_LEVEL constant
 *
 *	The default security level of the AG context.
 */
#define BTL_AG_DEFAULT_SECURITY_LEVEL	BSL_DEFAULT

/*-------------------------------------------------------------------------------
 * BTL_AG_NUMBER_OF_INDICATORS constant
 *
 *	The number of different indicator types as defined by the HFP spec.
 */
#define BTL_AG_NUMBER_OF_INDICATORS 8

/*-------------------------------------------------------------------------------
 * BTL_AG_MIN_INDICATOR constant
 *
 *	The minimum value a variable of type HfgIndicator can have
 */
#define BTL_AG_MIN_INDICATOR 1

/*-------------------------------------------------------------------------------
 * BTL_AG_MAX_INDICATOR constant
 *
 *	The maximum value a variable of type HfgIndicator can have
 */
#define BTL_AG_MAX_INDICATOR 7

/*---------------------------------------------------------------------------
 * HFG_BTL_SNIFF_TIMER constant
 *     Enable sniff mode after a defined timeout.  Sniff mode is entered
 *     when a service link is active, but no audio connection is up.  This
 *     is a power saving feature.  If this value is defined to -1, then
 *     sniff mode is disabled.
 *
 *	   This value is only the default value for the sniff timer, if HFG_BTL_SNIFF_TIMER >= 0
 *	   Then a sniff timer value can be set for each HFG channel.
 */
#define HFG_BTL_SNIFF_TIMER 2000

/*---------------------------------------------------------------------------
 * HFG_BTL_SNIFF_MIN_INTERVAL constant
 *     Miniumum interval for sniff mode if enabled (see HFG_BTL_SNIFF_TIMER).
 *
 */
#define HFG_BTL_SNIFF_MIN_INTERVAL 800

/*---------------------------------------------------------------------------
 * HFG_BTL_SNIFF_MAX_INTERVAL constant
 *     Maximum interval for sniff mode if enabled (see HFG_BTL_SNIFF_TIMER).
 *
 */
#define HFG_BTL_SNIFF_MAX_INTERVAL 8000

/*---------------------------------------------------------------------------
 * HFG_BTL_SNIFF_ATTEMPT constant
 *     Sniff attempts for sniff mode if enabled (see HFG_BTL_SNIFF_TIMER).
 *
 */
#define HFG_BTL_SNIFF_ATTEMPT 1600

/*---------------------------------------------------------------------------
 * HFG_BTL_SNIFF_TIMEOUT constant
 *     Sniff timeout for sniff mode if enabled (see HFG_BTL_SNIFF_TIMER).
 *
 */
#define HFG_BTL_SNIFF_TIMEOUT 1600

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BtlAgModuleState type
 *
 *	BtlAgModuleState represents different states the AG Module.
 */
typedef enum _BtlAgModuleState
{
	BTL_AG_MODULE_STATE_NOT_INITIALIZED,
	BTL_AG_MODULE_STATE_INITIALIZED
}BtlAgModuleState;


/*-------------------------------------------------------------------------------
 * BtlAgContextState type
 *
 *	BtlAgContextState represents different states a BtlAgContext.
 */
typedef enum _BtlAgContextState
{
	BTL_AG_CONTEXT_STATE_DISABLED,	/* Properly initialized and ready to be enabled */
	BTL_AG_CONTEXT_STATE_ENABLED,	/* Ready to accpet incoming/outgoing connections */
	BTL_AG_CONTEXT_STATE_DISABLING	/* Disabling the context */
}BtlAgContextState;


typedef enum _BtlAgChannelType
{
	BTL_AG_CHANNEL_TYPE_HANDSFREE,
	BTL_AG_CHANNEL_TYPE_HEADSET
} BtlAgChannelType;

/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BtlAgChannel structure.
 *
 *	Representing a AG Channel.
 */
typedef struct _BtlAgChannel
{
	/* The current state of the channel. */
	BtlAgChannelState state;

	/* The type of the channel's audio connection */
	CmgrAudioParms audioParms;

	/* The audio format of the channel. */
	BtScoAudioSettings audioFormat;

	BtlAgChannelType type;

	/* The containing context */
	BtlAgContext *context;

	union
	{
		HfgChannel hfgChannel;

		HsgChannel hsgChannel;
	}c;

} BtlAgChannel;

/*-------------------------------------------------------------------------------
 * BtlAgContext structure.
 *
 *	Definition of BtlAgContext.
 */
struct _BtlAgContext
{
	/* Base context */
	BtlContext base;

	/* The context's callback */
	BtlAgCallback callback;

	/* The state of the context */
	BtlAgContextState state;

	/* Used to hold handles to active HFG Channels */
	BtlAgChannel channels[BTL_HFAG_MAX_CHANNELS + BTL_HSAG_MAX_CHANNELS];

	/* The active services within the AG*/
	BtlAgService service;

	/* The context security level */
	BtSecurityLevel secLevel;
	
	U32 numChannels;

	U8 indicatorsArray[BTL_AG_NUMBER_OF_INDICATORS];
};


typedef struct _BtlAgModuleData
{
	const char *responsesPoolName;

	const char *contextsPoolName;

	/* tokens for HCI commands */
	MeCommandToken		hciCmdToken;

	BTL_POOL_DECLARE_POOL(contextsPool, contextsMemory, BTL_AG_MAX_CONTEXTS, sizeof(BtlAgContext));

	BTL_POOL_DECLARE_POOL(responsesPool, responsesMemory, BTL_AG_MAX_RESPONSES, sizeof(AtResults));

	ListEntry contextsList;

	BtlAgModuleState state;

	BtlAgService activeServices;

	HfgSniffParams sniffParams;

	BOOL isRadioOffNotified;
	
} BtlAgModuleData;

/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/

BtlAgModuleData agModuleData = {"AG responses pool", "AG contexts pool", 0};
 
/********************************************************************************
 *
 * Internal function prototypes
 *
 *******************************************************************************/

static BtlAgChannelType BtlAgGetChannelType(const BtlAgChannel *channel);

static BtStatus BtlAgUpdateChannelIndicators(BtlAgContext *context, BtlAgChannel *channel);

static BtStatus BtlAgRegisterChannel(BtlAgChannel *channel);

static BtStatus BtlAgDeregisterChannel(BtlAgChannel *channel);

static BtStatus BtlAgDisconnectServiceLink(BtlAgChannel *channel);

static BtStatus BtlAgCreateAudioLink(BtlAgChannel *channel);

static BtStatus BtlAgDisconnectAudioLink(BtlAgChannel *channel);

static BtStatus BtlAgCreateServiceLink(BtlAgChannel *channel,
									   BD_ADDR *bdAddr);

static BtlAgChannel* BtlAgFindFreeChannel(BtlAgContext *context);

static void BtlAgHfCallback(HfgChannel *hfgChannel, HfgCallbackParms *parms);

static void BtlAgHsCallback(HsgChannel *hsgChannel, HsgCallbackParms *parms);

static BtStatus BtlAgSendResults(BtlAgChannel *channel,
								 AtCommand command,
								 U32 parms,
								 U16 parmLen,
								 BOOL done);

static void BtlAgEnabledHfagEvent(BtlAgChannel *channel, HfgCallbackParms *parms);

static void BtlAgDisablingHfagEvent(BtlAgChannel *channel, HfgCallbackParms *parms);

static void BtlAgEnabledHsagEvent(BtlAgChannel *channel, HsgCallbackParms *parms);

static void BtlAgDisablingHsagEvent(BtlAgChannel *channel, HsgCallbackParms *parms);

static void BtlAgSendEventToBsc(BtlBscEventType eventType,
                                BtlAgChannel *agChannel,
                                BD_ADDR *bdAddr);

static BtStatus BtlAgSendHciCommand(U16	hciCommand,U8  parmsLen,U8 *parms);

static void BtlAgSendHciCommandCB(const BtEvent *event);

static BtStatus BtlAgBtlNotificationsCb(BtlModuleNotificationType notificationType);

static BtStatus BtlAgProcessRadioOffNotification(void);

static BtStatus BtlAgRemoveAllConnections(BtlAgContext *context);



/********************************************************************************
 *
 * Function definitions
 *
 *******************************************************************************/

BtStatus BTL_AG_Init(void)
{
	BtStatus status;

	BTL_FUNC_START_AND_LOCK_COMMON("BTL_AG_Init");
	
	BTL_VERIFY_ERR((BTL_AG_MODULE_STATE_NOT_INITIALIZED == agModuleData.state), 
		BT_STATUS_FAILED, ("illegal module state"));
	
	status = BTL_POOL_Create(&agModuleData.responsesPool,
		agModuleData.responsesPoolName,
		agModuleData.responsesMemory,
		BTL_AG_MAX_RESPONSES,
		sizeof(AtResults));
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, 
		("BTL_POOL_Create failed"));

	status = BTL_POOL_Create(&agModuleData.contextsPool,
		agModuleData.contextsPoolName,
		agModuleData.contextsMemory,
		BTL_AG_MAX_CONTEXTS,
		sizeof(BtlAgContext));
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, 
		("BTL_POOL_Create failed"));

	InitializeListHead(&agModuleData.contextsList);

	agModuleData.activeServices = 0;
	agModuleData.sniffParams.sniffTimer = HFG_BTL_SNIFF_TIMER;
	agModuleData.sniffParams.minInterval= HFG_BTL_SNIFF_MIN_INTERVAL;
	agModuleData.sniffParams.maxInterval= HFG_BTL_SNIFF_MAX_INTERVAL;
	agModuleData.sniffParams.attempt= HFG_BTL_SNIFF_ATTEMPT;
	agModuleData.sniffParams.timeout= HFG_BTL_SNIFF_TIMEOUT;
	agModuleData.isRadioOffNotified = FALSE;
	agModuleData.state = BTL_AG_MODULE_STATE_INITIALIZED;

	BTL_RegisterForCommonNotifications(BTL_MODULE_TYPE_AG, BtlAgBtlNotificationsCb);

	BTL_FUNC_END_AND_UNLOCK();
	
	return status;
}

BtStatus BTL_AG_Deinit()
{
	BtStatus status;

	BTL_FUNC_START_AND_LOCK_COMMON("BTL_AG_Deinit");
	
	BTL_VERIFY_ERR((BTL_AG_MODULE_STATE_INITIALIZED == agModuleData.state), 
		BT_STATUS_FAILED, ("illegal module state"));

	BTL_VERIFY_ERR((FALSE == IsListEmpty(&agModuleData.contextsList)), BT_STATUS_BUSY,
		("There are still active AG contexts"));
	 
	status = BTL_POOL_Destroy(&agModuleData.responsesPool);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR,
		("BTL_POOL_Destroy failed"));

	status = BTL_POOL_Destroy(&agModuleData.contextsPool);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR,
		("BTL_POOL_Destroy failed"));

	BTL_DeRegisterFromCommonNotifications(BTL_MODULE_TYPE_AG);

	agModuleData.state = BTL_AG_MODULE_STATE_NOT_INITIALIZED;;

	BTL_FUNC_END_AND_UNLOCK();
	
	return status;
}

BtStatus BTL_AG_Create(BtlAppHandle *appHandle,
					   BtlAgCallback callback,
					   BtSecurityLevel *secLevel,
					   BtlAgContext **context)
{
	BtStatus status;
	BtlAgChannel *channel;
	U32 nChannels;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_AG_Create");

	BTL_VERIFY_ERR((BTL_AG_MODULE_STATE_INITIALIZED == agModuleData.state), BT_STATUS_FAILED,
		("illegal module state"));

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM, ("Null context"));

	BTL_VERIFY_ERR((0 != callback), BT_STATUS_INVALID_PARM, ("callback is null"));

#if BT_SECURITY == XA_ENABLED
	if (0 != secLevel)
	{
		status = BTL_VerifySecurityLevel(*secLevel);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), BT_STATUS_INVALID_PARM,
			("Invalid security level"));
	}
#endif	/* BT_SECURITY == XA_ENABLED */
		
	status = BTL_POOL_Allocate(&agModuleData.contextsPool, (void**)context);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("BTL_POOL_Allocate Failed"));

	/* TODO: create BTL_MODULE_TYPE_AG */
	status = BTL_HandleModuleInstanceCreation(appHandle, BTL_MODULE_TYPE_AG, &(*context)->base);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR,
			("BTL_HandleModuleInstanceCreation() failed"));

	InsertTailList(&agModuleData.contextsList, &((*context)->base.node));

	channel = (*context)->channels;
	for (nChannels = 0; nChannels < BTL_HFAG_MAX_CHANNELS + BTL_HSAG_MAX_CHANNELS; ++nChannels)
	{
		channel->state = BTL_AG_CHANNEL_STATE_UNKNOWN;
		channel->type = nChannels < BTL_HFAG_MAX_CHANNELS ? BTL_AG_CHANNEL_TYPE_HANDSFREE: BTL_AG_CHANNEL_TYPE_HEADSET;
		++channel;
	}

	(*context)->callback = callback;
	(*context)->state = BTL_AG_CONTEXT_STATE_DISABLED;
	(*context)->secLevel = (BtSecurityLevel)(0 == secLevel ? BTL_AG_DEFAULT_SECURITY_LEVEL: *secLevel);
	(*context)->numChannels = 0;

	/* Set all the indicators to 0 */
	OS_MemSet((*context)->indicatorsArray, 0, sizeof((*context)->indicatorsArray[0])*BTL_AG_NUMBER_OF_INDICATORS);
	
	BTL_FUNC_END_AND_UNLOCK();
	
	return status;
}

BtStatus BTL_AG_Destroy(BtlAgContext **context)
{
	BtStatus status;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_AG_Destroy");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM, ("null context"));

	BTL_VERIFY_ERR((0 != *context), BT_STATUS_INVALID_PARM, ("context invalid"));

	BTL_VERIFY_ERR((BTL_AG_CONTEXT_STATE_DISABLED == (*context)->state), BT_STATUS_BUSY,
		("context is enabled"));

	RemoveEntryList(&(*context)->base.node);

	status = BTL_HandleModuleInstanceDestruction(&(*context)->base);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR,
		("BTL_HandleModuleInstanceDestruction() failed"));

	BTL_POOL_Free(&agModuleData.contextsPool, (void**)context);

	*context = 0;

	BTL_FUNC_END_AND_UNLOCK();
	
	return status;
}

BtStatus BTL_AG_Enable(BtlAgContext *context,
					   const char *serviceName,
					   AtAgFeatures *features,
					   BtlAgService service)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U32 idx, end;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_AG_Enable");

	/* Service name registration in SDP is not yet supported */
	UNUSED_PARAMETER(serviceName);

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM, ("null context"));

	BTL_VERIFY_ERR((BTL_AG_CONTEXT_STATE_DISABLED == context->state), BT_STATUS_FAILED, ("Context is not disabled"));

	BTL_VERIFY_ERR((0 == (agModuleData.activeServices & service)), BT_STATUS_FAILED,
		("specified services are already active"));

	BTL_VERIFY_ERR((0 !=  (service & (BTL_AG_SERVICE_HFAG|BTL_AG_SERVICE_HSAG))),
		BT_STATUS_INVALID_PARM,("service invalid"));

	/* Register channels that belong to the services specified */
	idx = (0 != (service & BTL_AG_SERVICE_HFAG)) ? 0 : BTL_HFAG_MAX_CHANNELS;
	end = (0 != (service & BTL_AG_SERVICE_HSAG)) ? BTL_HFAG_MAX_CHANNELS + BTL_HSAG_MAX_CHANNELS : BTL_HFAG_MAX_CHANNELS;
	for (; end-idx > 0; ++idx)
	{		
		status = BtlAgRegisterChannel(&context->channels[idx]);
		if (BTL_AG_CHANNEL_TYPE_HANDSFREE == BtlAgGetChannelType(&context->channels[idx]))
		{
			context->channels[idx].c.hfgChannel.sniffTimer = agModuleData.sniffParams.sniffTimer;
			context->channels[idx].c.hfgChannel.minInterval = agModuleData.sniffParams.minInterval;
			context->channels[idx].c.hfgChannel.maxInterval = agModuleData.sniffParams.maxInterval;
			context->channels[idx].c.hfgChannel.attempt = agModuleData.sniffParams.attempt;
			context->channels[idx].c.hfgChannel.timeout = agModuleData.sniffParams.timeout;

			status = BtlAgUpdateChannelIndicators(context, &context->channels[idx]);
			BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status,
				("BtlAgUpdateChannelIndicators failed: %s", pBT_Status(status)));
		}
		
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), BT_STATUS_FAILED, ("BtlAgRegisterChannel failed"));
		context->channels[idx].state = BTL_AG_CHANNEL_STATE_IDLE;
		context->channels[idx].context = context;
	}
	if (service & BTL_AG_SERVICE_HFAG)
	{
		HFG_SetSecurityLevel(context->secLevel);
		status = HFG_SetFeatures(0 == features ? HFG_DEFAULT_FEATURES : *features);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), BT_STATUS_FAILED, ("HFG_SetFeatures failed"));
	}
	if (service & BTL_AG_SERVICE_HSAG)
	{
		HSG_SetSecurityLevel(context->secLevel);
	}
	context->service = service;
	agModuleData.activeServices |= service;

	context->state = BTL_AG_CONTEXT_STATE_ENABLED;

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_AG_Disable(BtlAgContext *context)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U32 nChannels;
	BOOL isPending = FALSE;
	BtlAgChannel *channel;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_AG_Disable");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM, ("null context"));

	BTL_VERIFY_ERR((BTL_AG_CONTEXT_STATE_ENABLED == context->state), BT_STATUS_FAILED, ("context not enabled"));

	context->state = BTL_AG_CONTEXT_STATE_DISABLING;

	channel = context->channels;
	for (nChannels = BTL_HFAG_MAX_CHANNELS + BTL_HSAG_MAX_CHANNELS; nChannels > 0; --nChannels)
	{
		if (BTL_AG_CHANNEL_STATE_IDLE == channel->state)
		{
			status = BtlAgDeregisterChannel(channel);
			BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), status, ("BtlAgDeregisterChannel() failed"));
		}
		else if (BTL_AG_CHANNEL_STATE_SLC_CONNECTED <= channel->state)
		{
			isPending = TRUE;
			status = BtlAgDisconnectServiceLink(channel);
			if (BT_STATUS_PENDING == status)
			{
				channel->state = BTL_AG_CHANNEL_STATE_DISCONNECTING_SLC;
			}
			else if (BT_STATUS_SUCCESS == status)
			{
				channel->state = BTL_AG_CHANNEL_STATE_IDLE;
			}

			BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status || BT_STATUS_PENDING == status), status, ("BtlAgDisconnectServiceLink() failed"));
		} 
		else if (BTL_AG_CHANNEL_STATE_UNKNOWN != channel->state)
		{
			isPending = TRUE;
		}
		++channel;
	}
	if (isPending)
	{
		status = BT_STATUS_PENDING;
	}
	else
	{
		context->state = BTL_AG_CONTEXT_STATE_DISABLED;
		agModuleData.activeServices &= ~context->service;
	}
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

#if BT_SECURITY == XA_ENABLED
BtStatus BTL_AG_SetSecurityLevel(BtlAgContext *context,
								 BtSecurityLevel *secLevel)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_AG_SetSecurityLevel");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM, ("null context"));

	if (0 != secLevel)
	{
		status = BTL_VerifySecurityLevel(*secLevel);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), BT_STATUS_INVALID_PARM, ("Invalid security level"));
		context->secLevel = *secLevel;
	}
	else
	{
		context->secLevel = BTL_AG_DEFAULT_SECURITY_LEVEL;
	}

	if (BTL_AG_SERVICE_HFAG & context->service)
	{
		HFG_SetSecurityLevel(context->secLevel);
	}
	if (BTL_AG_SERVICE_HSAG & context->service)
	{
		HSG_SetSecurityLevel(context->secLevel);
	}
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_AG_GetSecurityLevel(BtlAgContext *context,
								 BtSecurityLevel *secLevel)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_HFAG_GetSecurityLevel");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM, ("null context"));
	
	BTL_VERIFY_ERR((0 != secLevel), BT_STATUS_INVALID_PARM, ("secLevel is null"));

	*secLevel = context->secLevel;
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}
#endif /* BT_SECURITY == XA_ENABLED */

BtStatus BTL_AG_GetService(BtlAgContext *context,
						   BtlAgService *service)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_AG_GetService");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM, ("null context"));
	
	BTL_VERIFY_ERR((0 != service), BT_STATUS_INVALID_PARM, ("null service"));

	*service = context->service;

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_AG_SetFeatures(BtlAgContext *context,
							const AtAgFeatures *features)
{
	BtStatus status;
	AtAgFeatures agFeatures;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_AG_SetFeatures");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM, ("null context"));

	BTL_VERIFY_ERR((BTL_AG_SERVICE_HFAG & context->service), BT_STATUS_INVALID_PARM,
		("HFAG service is not active"));

	if (BTL_AG_CONTEXT_STATE_DISABLED == context->state)
	{
		agFeatures = (0 == features ? HFG_DEFAULT_FEATURES : *features);
		status = HFG_SetFeatures(agFeatures);
	}
	else
	{
		status = BT_STATUS_FAILED;
	}

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_AG_GetFeatures(const BtlAgContext *context,
							AtAgFeatures *features)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_AG_GetFeatures");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((BTL_AG_SERVICE_HFAG & context->service), BT_STATUS_INVALID_PARM,
		("HFAG service is not active"));

	BTL_VERIFY_ERR((0 != features),BT_STATUS_INVALID_PARM, ("null features"));

	*features = HFG_GetFeatures();
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_AG_ConnectSLC(BtlAgContext *context,
						   BD_ADDR *bdAddr)
{
	BtStatus status;
	BtlAgChannel *channel;

	BTL_FUNC_START_AND_LOCK("BTL_AG_ConnectSLC");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM,("null bdAddr"));

	channel = BtlAgFindFreeChannel(context);
	BTL_VERIFY_ERR((0 != channel), BT_STATUS_NO_RESOURCES, ("no free channels"));

    status = BtlAgCreateServiceLink(channel, bdAddr);

	if (BT_STATUS_PENDING == status)
	{
		channel->state = BTL_AG_CHANNEL_STATE_CONNECTING_SLC;
	}
	else if(BT_STATUS_SUCCESS == status)
	{
		channel->state = BTL_AG_CHANNEL_STATE_SLC_CONNECTED;
	}

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_AG_ConnectSLCAndGetChannelId(BtlAgContext *context,
						                  BD_ADDR *bdAddr,
                                          BtlAgChannelId *channelId)
{
	BtStatus status;
	BtlAgChannel *channel;

	BTL_FUNC_START_AND_LOCK("BTL_AG_ConnectSLC");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM,("null bdAddr"));

	channel = BtlAgFindFreeChannel(context);
	BTL_VERIFY_ERR((0 != channel), BT_STATUS_NO_RESOURCES, ("no free channels"));

	/* Calculate channel ID to be returned */
    *channelId = (BtlAgChannelId)(channel - channel->context->channels);
    
    status = BtlAgCreateServiceLink(channel, bdAddr);

	if (BT_STATUS_PENDING == status)
	{
		channel->state = BTL_AG_CHANNEL_STATE_CONNECTING_SLC;
	}
	else if(BT_STATUS_SUCCESS == status)
	{
		channel->state = BTL_AG_CHANNEL_STATE_SLC_CONNECTED;
	}

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_AG_DisconnectSLC(BtlAgContext *context,
							  BtlAgChannelId channelId)
{
	BtStatus status;
	BtlAgChannel *channel;

	BTL_FUNC_START_AND_LOCK("BTL_AG_DisconnectSLC");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((BTL_HFAG_MAX_CHANNELS + BTL_HSAG_MAX_CHANNELS > channelId),
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	channel = &context->channels[channelId];
	status = BtlAgDisconnectServiceLink(channel);

	if (BT_STATUS_PENDING == status)
	{
		channel->state = BTL_AG_CHANNEL_STATE_DISCONNECTING_SLC;
	}
	else if (BT_STATUS_SUCCESS == status)
	{
		channel->state = BTL_AG_CHANNEL_STATE_IDLE;
	}

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_AG_GetChannelState(BtlAgContext *context,
								BtlAgChannelId channelId,
								BtlAgChannelState *state)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK("BTL_AG_GetChannelState");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((BTL_HFAG_MAX_CHANNELS + BTL_HSAG_MAX_CHANNELS > channelId),
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	BTL_VERIFY_ERR((0 != state), BT_STATUS_INVALID_PARM,("null state"));

	*state = context->channels[channelId].state;

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_AG_SetAudioParms(BtlAgContext *context,
							  BtlAgChannelId channelId,
							  CmgrAudioParms *parms)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtlAgChannel *channel;

	BTL_FUNC_START_AND_LOCK("BTL_AG_SetAudioParms");

	BTL_VERIFY_ERR((0 != context), status,("null context"));

	BTL_VERIFY_ERR((BTL_HFAG_MAX_CHANNELS + BTL_HSAG_MAX_CHANNELS > channelId),
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	channel = &context->channels[channelId];
	channel->audioParms = (CmgrAudioParms)(0 == parms ? BTL_HFAG_DEFAULT_AUDIO_PARMS : *parms);

	if (CMGR_AUDIO_PARMS_S1 <= channel->audioParms && CMGR_AUDIO_PARMS_S3 >= channel->audioParms)
	{
		/* When audio parameters are S1, S2 or S3 the air format must be CVSD */
		channel->audioFormat &= ~(BSAS_ALAW | BSAS_ULAW);
		channel->audioFormat |= BSAS_CVSD;	
	}

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_AG_GetAudioParms(BtlAgContext *context,
							  BtlAgChannelId channelId,
							  CmgrAudioParms *parms)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK("BTL_AG_GetAudioParms");

	BTL_VERIFY_ERR((0 != context), status,("null context"));

	BTL_VERIFY_ERR((BTL_HFAG_MAX_CHANNELS + BTL_HSAG_MAX_CHANNELS > channelId),
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	BTL_VERIFY_ERR((0 != parms), status,("null parms"));

	*parms = context->channels[channelId].audioParms;

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_AG_SetAudioFormat(BtlAgContext *context,
							   BtlAgChannelId channelId,
							   BtScoAudioSettings *format)
{
	BtStatus status;
	BtScoAudioSettings airFormat;
	BtlAgChannel *channel;

	BTL_FUNC_START_AND_LOCK("BTL_AG_SetAudioFormat");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((BTL_HFAG_MAX_CHANNELS + BTL_HSAG_MAX_CHANNELS > channelId),
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	channel = &context->channels[channelId];

	if (BTL_AG_CHANNEL_TYPE_HANDSFREE == BtlAgGetChannelType(channel))
	{
		airFormat = (BtScoAudioSettings)(0 == format ? BTL_HFAG_DEFAULT_AUDIO_FORMAT : *format);
	}
	else
	{
		airFormat = (BtScoAudioSettings)(0 == format ? BTL_HSAG_DEFAULT_AUDIO_FORMAT : *format);
	}

	if (CMGR_AUDIO_PARMS_S1 <= channel->audioParms &&
		CMGR_AUDIO_PARMS_S3 >= channel->audioParms &&
		~(BSAS_ALAW & airFormat || BSAS_ULAW & airFormat))
	{
		/* If audio settings are S1, S2 and S3  the air format must be CVSD */
		status = BT_STATUS_FAILED;
	}
	else
	{
		channel->audioFormat = airFormat;
		status = BT_STATUS_SUCCESS;
	}

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_AG_GetAudioFormat(BtlAgContext *context,
							   BtlAgChannelId channelId,
							   BtScoAudioSettings *format)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK("BTL_AG_SetAudioFormat");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((0 != format), BT_STATUS_INVALID_PARM,("null format"));

	BTL_VERIFY_ERR((BTL_HFAG_MAX_CHANNELS + BTL_HSAG_MAX_CHANNELS > channelId),
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	*format = context->channels[channelId].audioFormat;

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_AG_ConnectAudio(BtlAgContext *context,
							 BtlAgChannelId channelId)
{
	BtStatus status;
	BtlAgChannel *channel;

	BTL_FUNC_START_AND_LOCK("BTL_AG_ConnectAudio");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((BTL_HFAG_MAX_CHANNELS + BTL_HSAG_MAX_CHANNELS > channelId),
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	channel = &context->channels[channelId];

	status = BtlAgCreateAudioLink(channel);
	if (BT_STATUS_PENDING == status)
	{
		channel->state = BTL_AG_CHANNEL_STATE_CONNECTING_AUDIO;
	}
	else if (BT_STATUS_SUCCESS == status)
	{
		channel->state = BTL_AG_CHANNEL_STATE_AUDIO_CONNECTED;
	}

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_AG_DisconnectAudio(BtlAgContext *context,
								BtlAgChannelId channelId)
{
	BtStatus status;
	BtlAgChannel *channel;

	BTL_FUNC_START_AND_LOCK("BTL_AG_DisconnectAudio");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((BTL_HFAG_MAX_CHANNELS + BTL_HSAG_MAX_CHANNELS > channelId),
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	channel = &context->channels[channelId];

	status = BtlAgDisconnectAudioLink(channel);
	if (BT_STATUS_PENDING == status)
	{
		channel->state = BTL_AG_CHANNEL_STATE_DISCONNECTING_AUDIO;
	}
	else if (BT_STATUS_SUCCESS == status)
	{
		channel->state = BTL_AG_CHANNEL_STATE_SLC_CONNECTED;
	}

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_AG_GetConnectedDevice(BtlAgContext *context,
								   BtlAgChannelId channelId,
								   BD_ADDR *bdAddr)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtlAgChannel *channel;
	BD_ADDR *devAddr = 0;

	BTL_FUNC_START_AND_LOCK("BTL_AG_DisconnectAudio");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM,("null bdAddr"));

	BTL_VERIFY_ERR((BTL_HFAG_MAX_CHANNELS + BTL_HSAG_MAX_CHANNELS > channelId),
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	channel = &context->channels[channelId];
	if (BTL_AG_CHANNEL_TYPE_HANDSFREE == BtlAgGetChannelType(channel))
	{
		devAddr = &channel->c.hfgChannel.cmgrHandler.remDev->bdAddr;
	}
	else
	{
		devAddr = &channel->c.hsgChannel.cmgrHandler.remDev->bdAddr;
	}

	OS_MemCopy((U8*)bdAddr, (U8*)devAddr, sizeof(BD_ADDR));

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

#if BT_SCO_HCI_DATA == XA_ENABLED

BtStatus BTL_AG_SendAudioData(BtlAgContext *context,
							  BtlAgChannelId channelId,
							  BtPacket *packet)
{
	BtStatus status;
	BtlAgChannel *channel;

	BTL_FUNC_START_AND_LOCK("BTL_AG_SendAudioData");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((0 != packet), BT_STATUS_INVALID_PARM,("null packet"));

	BTL_VERIFY_ERR((BTL_HFAG_MAX_CHANNELS + BTL_HSAG_MAX_CHANNELS > channelId),
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	channel = &context->channels[channelId];

	if (BTL_AG_CHANNEL_TYPE_HANDSFREE == BtlAgGetChannelType(channel))
	{
		status = HFG_SendAudioData(&channel->c.hfgChannel, packet);
	}
	else
	{
		status = HSG_SendAudioData(&channel->c.hsgChannel, packet);
	}

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

#endif

BtStatus BTL_AG_SendOK(BtlAgContext *context,
					   BtlAgChannelId channelId)
{
	BtStatus status;

	BTL_FUNC_START_AND_LOCK("BTL_AG_SendOK");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((BTL_HFAG_MAX_CHANNELS + BTL_HSAG_MAX_CHANNELS > channelId),
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	status = BtlAgSendResults(&context->channels[channelId], AT_OK,0,0,FALSE);

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_AG_SendError(BtlAgContext *context,
						  BtlAgChannelId channelId,
						  HfgCmeError error)
{
	BtStatus status;

	BTL_FUNC_START_AND_LOCK("BTL_AG_SendError");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((BTL_HFAG_MAX_CHANNELS + BTL_HSAG_MAX_CHANNELS > channelId),
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	status = BtlAgSendResults(&context->channels[channelId], AT_ERROR, (U32)error,0,FALSE);

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_AG_SendRing(BtlAgContext *context,
						 BtlAgChannelId channelId)
{
	BtStatus status;

	BTL_FUNC_START_AND_LOCK("BTL_AG_SendRing");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((BTL_HFAG_MAX_CHANNELS + BTL_HSAG_MAX_CHANNELS > channelId),
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	status = BtlAgSendResults(&context->channels[channelId], AT_RING,0,0,FALSE);

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_AG_SendMicVolume(BtlAgContext *context,
							  BtlAgChannelId channelId,
							  U32 gain)
{
	BtStatus status;

	BTL_FUNC_START_AND_LOCK("BTL_AG_SendMicVolume");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((BTL_AG_MAX_VOLUME >= gain), BT_STATUS_INVALID_PARM, ("invalid gain"));

	BTL_VERIFY_ERR((BTL_HFAG_MAX_CHANNELS + BTL_HSAG_MAX_CHANNELS > channelId),
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	status = BtlAgSendResults(&context->channels[channelId], AT_MICROPHONE_GAIN, gain, 0, FALSE);

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_AG_SendSpeakerVolume(BtlAgContext *context,
								  BtlAgChannelId channelId,
								  U32 gain)
{
	BtStatus status;

	BTL_FUNC_START_AND_LOCK("BTL_AG_SendSpeakerVolume");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((BTL_AG_MAX_VOLUME >= gain), BT_STATUS_INVALID_PARM, ("invalid gain"));

	BTL_VERIFY_ERR((BTL_HFAG_MAX_CHANNELS + BTL_HSAG_MAX_CHANNELS > channelId),
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	status = BtlAgSendResults(&context->channels[channelId], AT_SPEAKER_GAIN, gain, 0, FALSE);

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_AG_SendAtResult(BtlAgContext *context,
							 BtlAgChannelId channelId,
							 const char *atString)
{
	BtStatus status;

	BTL_FUNC_START_AND_LOCK("BTL_AG_SendAtResult");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((BTL_HFAG_MAX_CHANNELS + BTL_HSAG_MAX_CHANNELS > channelId),
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	status = BtlAgSendResults(&context->channels[channelId], AT_RAW, (U32)atString, sizeof(atString), TRUE);

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_AG_SetIndicatorValue(BtlAgContext *context,
								  HfgIndicator indicator, 
								  U8 value)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U32 nChannels;
	BtlAgChannel *channel;
	AtResults *response;

	BTL_FUNC_START_AND_LOCK("BTL_AG_SetIndicatorValue");

	BTL_VERIFY_ERR((0 != context), status,("null context"));

	BTL_VERIFY_ERR((BTL_AG_SERVICE_HFAG & context->service), BT_STATUS_INVALID_PARM,
		("HFAG Service is not active"));

	BTL_VERIFY_ERR((BTL_AG_MIN_INDICATOR <= indicator && BTL_AG_MAX_INDICATOR >= indicator),
		BT_STATUS_INVALID_PARM, ("Illegal value for 'indicator' argument"));

	context->indicatorsArray[indicator] = value;

	if (BTL_AG_CONTEXT_STATE_ENABLED != context->state)
	{
		BTL_RET(BT_STATUS_SUCCESS);
	}

	channel = context->channels;
	for (nChannels = BTL_HFAG_MAX_CHANNELS; 0 < nChannels; --nChannels)
	{
        BOOL sendEventToBsc = FALSE;
        BtlBscEventType bscEvent = 0;

		if (BTL_AG_CHANNEL_TYPE_HANDSFREE == BtlAgGetChannelType(channel) &&
			BTL_AG_CHANNEL_STATE_IDLE <= channel->state)
		{
			status = BTL_POOL_Allocate(&agModuleData.responsesPool, (void**)&response);
			BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), BT_STATUS_NO_RESOURCES,
				("BTL_POOL_Allocate failed"));

			status = HFG_SetIndicatorValue(&channel->c.hfgChannel, indicator, value, response);

			if (BT_STATUS_PENDING != status)
			{
				status = BTL_POOL_Free(&agModuleData.responsesPool, (void**)&response);
				BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR,
					("BTL_POOL_Free failed"));
			}

            if (BTL_AG_CHANNEL_STATE_SLC_CONNECTED <= channel->state)
            {
                switch(indicator)
                {
                    case HFG_IND_CALL_SETUP:
			            if (HFG_CALL_SETUP_IN == value)
			            {
                            bscEvent = BTL_BSC_EVENT_INCOMING_CALL;
                            sendEventToBsc = TRUE;
			            }
                        break;

                    case HFG_IND_CALL:
                        if (0 == value)
                        {
                            bscEvent = BTL_BSC_EVENT_CALL_FINISHED;
                            sendEventToBsc = TRUE;
                        }
                        break;

                    default:
                        break;
                }

                if (TRUE == sendEventToBsc)
                {
				    /* Pass event to the BSC module */
                    BtlAgSendEventToBsc(bscEvent, channel, NULL);
                }
            }
		}
		++channel;
	}
    
    BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_AG_GetIndicatorValue(BtlAgContext *context,
								  HfgIndicator indicator, 
								  U8 *value)
{
	BtStatus status = BT_STATUS_FAILED;
	BtlAgChannel *channel;
	U32 nChannels;

	BTL_FUNC_START_AND_LOCK("BTL_AG_GetIndicatorValue");

	BTL_VERIFY_ERR((0 != context), status,("null context"));

	BTL_VERIFY_ERR((BTL_AG_SERVICE_HFAG & context->service), BT_STATUS_INVALID_PARM,
		("HFAG Service is not active"));

	channel = context->channels;
	for (nChannels = BTL_HFAG_MAX_CHANNELS; nChannels > 0; --nChannels)
	{
		if (BTL_AG_CHANNEL_STATE_IDLE == channel->state)
		{
			status = HFG_GetIndicatorValue(&channel->c.hfgChannel, indicator, value);
			break;
		}
		++channel;
	}

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}


BtStatus BTL_AG_SendCallListRsp(BtlAgContext *context,
								BtlAgChannelId channelId,
								const HfgCallListParms *parms,
								BOOL finalFlag)
{
	BtStatus status;

	BTL_FUNC_START_AND_LOCK("BTL_AG_SendCallListRsp");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((BTL_AG_SERVICE_HFAG & context->service), BT_STATUS_INVALID_PARM,
		("HFAG Service is not active"));

	BTL_VERIFY_ERR((0 != parms), BT_STATUS_INVALID_PARM,("null parms"));

	BTL_VERIFY_ERR((BTL_HFAG_MAX_CHANNELS > channelId),
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	status = BtlAgSendResults(&context->channels[channelId], AT_LIST_CURRENT_CALLS, (U32)parms,
		sizeof(HfgCallListParms), finalFlag);

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}


BtStatus BTL_AG_SendCallerId(BtlAgContext *context,
							 BtlAgChannelId channelId,
							 const char *number,
							 U8 type)
{
	BtStatus status;
	HfgCallerIdParms parms;

	BTL_FUNC_START_AND_LOCK("BTL_AG_SendCallerId");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((0 != number), BT_STATUS_INVALID_PARM,("null number"));

	BTL_VERIFY_ERR((BTL_AG_SERVICE_HFAG & context->service), BT_STATUS_INVALID_PARM,
		("HFAG Service is not active"));

	BTL_VERIFY_ERR((BTL_HFAG_MAX_CHANNELS > channelId),
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	parms.number = number;
	parms.type = type;

	status = BtlAgSendResults(&context->channels[channelId], AT_CALL_ID,
		(U32)&parms, sizeof(HfgCallerIdParms), FALSE);

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_AG_SendSubscriberNumberRsp(BtlAgContext *context,
										BtlAgChannelId channelId,
										HfgSubscriberNum *number)
{
	BtStatus status;

	BTL_FUNC_START_AND_LOCK("BTL_AG_SendSubscriberNumberRsp");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((BTL_AG_SERVICE_HFAG & context->service), BT_STATUS_INVALID_PARM,
		("HFAG Service is not active"));

	BTL_VERIFY_ERR((BTL_HFAG_MAX_CHANNELS > channelId),
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	status = BtlAgSendResults(&context->channels[channelId], AT_SUBSCRIBER_NUM, (U32)number,
		sizeof(HfgSubscriberNum),TRUE);

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_AG_SendNetworkOperatorRsp(BtlAgContext *context,
									   BtlAgChannelId channelId,
									   HfgNetworkOperator *oper)
{
	BtStatus status;

	BTL_FUNC_START_AND_LOCK("BTL_AG_SendNetworkOperatorRsp");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((0 != oper), BT_STATUS_INVALID_PARM,("null oper"));

	BTL_VERIFY_ERR((BTL_AG_SERVICE_HFAG & context->service), BT_STATUS_INVALID_PARM,
		("HFAG Service is not active"));

	BTL_VERIFY_ERR((BTL_HFAG_MAX_CHANNELS > channelId),
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	status = BtlAgSendResults(&context->channels[channelId], AT_NETWORK_OPERATOR | AT_READ, (U32)oper,
		sizeof(HfgNetworkOperator), TRUE);

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_AG_EnableInbandRingTone(BtlAgContext *context,
									 BtlAgChannelId channelId,
									 BOOL enabled)
{
	BtStatus status;

	BTL_FUNC_START_AND_LOCK("BTL_AG_EnableInbandRingTone");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((BTL_AG_SERVICE_HFAG & context->service), BT_STATUS_INVALID_PARM,
		("HFAG Service is not active"));

	BTL_VERIFY_ERR((BTL_HFAG_MAX_CHANNELS > channelId),
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	status = BtlAgSendResults(&context->channels[channelId], AT_IN_BAND_RING_TONE, (U32)enabled, 0, FALSE);


	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

#if HFG_USE_RESP_HOLD == XA_ENABLED
BtStatus BTL_AG_SendResponseHoldState(BtlAgContext *context,
									  BtlAgChannelId channelId,
									  HfgResponseHold state)
{
	BtStatus status;

	BTL_FUNC_START_AND_LOCK("BTL_AG_SendResponseHoldState");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((BTL_AG_SERVICE_HFAG & context->service), BT_STATUS_INVALID_PARM,
		("HFAG Service is not active"));

	BTL_VERIFY_ERR((BTL_HFAG_MAX_CHANNELS > channelId),
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	status = BtlAgSendResults(&context->channels[channelId], AT_RESPONSE_AND_HOLD, (U32)state,
		sizeof(HfgResponseHold), FALSE);

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

#endif /* HFG_USE_RESP_HOLD == XA_ENABLED */


BtStatus BTL_AG_SendCallWaiting(BtlAgContext *context,
								BtlAgChannelId channelId,
								HfgCallWaitParms *parms)
{
	BtStatus status;

	BTL_FUNC_START_AND_LOCK("BTL_AG_SendCallWaiting");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((0 != parms), BT_STATUS_INVALID_PARM,("null parms"));
	
	BTL_VERIFY_ERR((BTL_AG_SERVICE_HFAG & context->service), BT_STATUS_INVALID_PARM,
		("HFAG Service is not active"));

	BTL_VERIFY_ERR((BTL_HFAG_MAX_CHANNELS > channelId),
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	status = BtlAgSendResults(&context->channels[channelId], AT_CALL_WAIT_NOTIFY, (U32)parms,
		sizeof(HfgCallWaitParms), FALSE);
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_AG_SendEnableVoiceRec(BtlAgContext *context,
								   BtlAgChannelId channelId,
								   BOOL enabled)
{
	BtStatus status;

	BTL_FUNC_START_AND_LOCK("BTL_AG_SendEnableVoiceRec");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((BTL_AG_SERVICE_HFAG & context->service), BT_STATUS_INVALID_PARM,
		("HFAG Service is not active"));

	BTL_VERIFY_ERR((BTL_HFAG_MAX_CHANNELS > channelId),
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	status = BtlAgSendResults(&context->channels[channelId], AT_VOICE_RECOGNITION, (U32)enabled, 0, FALSE);
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}


BtStatus BTL_AG_SendVoiceTagRsp(BtlAgContext *context,
								BtlAgChannelId channelId,
								const char *number)
{
	BtStatus status;

	BTL_FUNC_START_AND_LOCK("BTL_AG_SendVoiceTagRsp");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((0 != number), BT_STATUS_INVALID_PARM,("null number"));

	BTL_VERIFY_ERR((BTL_AG_SERVICE_HFAG & context->service), BT_STATUS_INVALID_PARM,
		("HFAG Service is not active"));

	BTL_VERIFY_ERR((BTL_HFAG_MAX_CHANNELS > channelId),
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	status = BtlAgSendResults(&context->channels[channelId], AT_VOICE_TAG, (U32)number, sizeof(number), TRUE);

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}


BtStatus BTL_AG_SendNoCarrierRsp(BtlAgContext *context,
								 BtlAgChannelId channelId)
{
	BtStatus status;

	BTL_FUNC_START_AND_LOCK("BTL_AG_SendNoCarrierRsp");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((BTL_AG_SERVICE_HFAG & context->service), BT_STATUS_INVALID_PARM,
		("HFAG Service is not active"));

	BTL_VERIFY_ERR((BTL_HFAG_MAX_CHANNELS > channelId),
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	status = BtlAgSendResults(&context->channels[channelId], AT_NO_CARRIER, 0, 0, FALSE);

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}


BtStatus BTL_AG_SendBusyRsp(BtlAgContext *context,
							BtlAgChannelId channelId)
{
	BtStatus status;

	BTL_FUNC_START_AND_LOCK("BTL_AG_SendBusyRsp");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((BTL_AG_SERVICE_HFAG & context->service), BT_STATUS_INVALID_PARM,
		("HFAG Service is not active"));

	BTL_VERIFY_ERR((BTL_HFAG_MAX_CHANNELS > channelId),
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	status = BtlAgSendResults(&context->channels[channelId], AT_BUSY, 0, 0, FALSE);

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}


BtStatus BTL_AG_SendNoAnswerRsp(BtlAgContext *context,
								BtlAgChannelId channelId)
{
	BtStatus status;

	BTL_FUNC_START_AND_LOCK("BTL_AG_SendNoAnswerRsp");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((BTL_AG_SERVICE_HFAG & context->service), BT_STATUS_INVALID_PARM,
		("HFAG Service is not active"));

	BTL_VERIFY_ERR((BTL_HFAG_MAX_CHANNELS > channelId),
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	status = BtlAgSendResults(&context->channels[channelId], AT_NO_ANSWER, 0, 0, FALSE);
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}


BtStatus BTL_AG_SendDelayedRsp(BtlAgContext *context,
							   BtlAgChannelId channelId)
{
	BtStatus status;

	BTL_FUNC_START_AND_LOCK("BTL_AG_SendDelayedRsp");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((BTL_AG_SERVICE_HFAG & context->service), BT_STATUS_INVALID_PARM,
		("HFAG Service is not active"));

	BTL_VERIFY_ERR((BTL_HFAG_MAX_CHANNELS > channelId),
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	status = BtlAgSendResults(&context->channels[channelId], AT_DELAYED, 0, 0, FALSE);

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_AG_SendBlacklistedRsp(BtlAgContext *context,
								   BtlAgChannelId channelId)

{
	BtStatus status;

	BTL_FUNC_START_AND_LOCK("BTL_AG_SendBlacklistedRsp");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((BTL_AG_SERVICE_HFAG & context->service), BT_STATUS_INVALID_PARM,
		("HFAG Service is not active"));

	BTL_VERIFY_ERR((BTL_HFAG_MAX_CHANNELS > channelId),
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	status = BtlAgSendResults(&context->channels[channelId], AT_BLACKLISTED, 0, 0, FALSE);

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_AG_IsNoiseReductionEnabled(BtlAgContext *context,
										BtlAgChannelId channelId,
										BOOL *enabled)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK("BTL_AG_IsNoiseReductionEnabled");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((BTL_AG_SERVICE_HFAG & context->service), BT_STATUS_INVALID_PARM,
		("HFAG Service is not active"));

	BTL_VERIFY_ERR((BTL_HFAG_MAX_CHANNELS > channelId),
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	*enabled = HFG_IsNRECEnabled(&context->channels[channelId].c.hfgChannel);

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_AG_IsInbandRingEnabled(BtlAgContext *context,
									BtlAgChannelId channelId,
									BOOL *enabled)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK("BTL_AG_IsInbandRingEnabled");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((0 != enabled), BT_STATUS_INVALID_PARM,("null enabled"));

	BTL_VERIFY_ERR((BTL_AG_SERVICE_HFAG & context->service), BT_STATUS_INVALID_PARM,
		("HFAG Service is not active"));

	BTL_VERIFY_ERR((BTL_HFAG_MAX_CHANNELS  > channelId),
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	*enabled = HFG_IsInbandRingEnabled(&context->channels[channelId].c.hfgChannel);

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}


BtStatus BTL_AG_IsCallIdNotifyEnabled(BtlAgContext *context,
							 BtlAgChannelId channelId,
							 BOOL *enabled)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK("BTL_AG_IsCallIdNotifyEnabled");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((0 != enabled), BT_STATUS_INVALID_PARM,("null enabled"));

	BTL_VERIFY_ERR((BTL_AG_SERVICE_HFAG & context->service), BT_STATUS_INVALID_PARM,
		("HFAG Service is not active"));

	BTL_VERIFY_ERR((BTL_HFAG_MAX_CHANNELS > channelId),
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	*enabled = HFG_IsCallIdNotifyEnabled(&context->channels[channelId].c.hfgChannel);

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}


BtStatus BTL_AG_IsVoiceRecEnabled(BtlAgContext *context,
								  BtlAgChannelId channelId,
								  BOOL *enabled)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK("BTL_AG_IsVoiceRecEnabled");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((0 != enabled), BT_STATUS_INVALID_PARM,("null enabled"));
	
	BTL_VERIFY_ERR((BTL_AG_SERVICE_HFAG & context->service), BT_STATUS_INVALID_PARM,
		("HFAG Service is not active"));

	BTL_VERIFY_ERR((BTL_HFAG_MAX_CHANNELS > channelId),
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	*enabled = HFG_IsVoiceRecActive(&context->channels[channelId].c.hfgChannel);

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}


BtStatus BTL_AG_IsCallWaitingEnabled(BtlAgContext *context,
									 BtlAgChannelId channelId,
									 BOOL *enabled)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK("BTL_AG_IsCallWaitingEnabled");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((0 != enabled), BT_STATUS_INVALID_PARM,("null enabled"));
	
	BTL_VERIFY_ERR((BTL_AG_SERVICE_HFAG & context->service), BT_STATUS_INVALID_PARM,
		("HFAG Service is not active"));

	BTL_VERIFY_ERR((BTL_HFAG_MAX_CHANNELS > channelId),
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	*enabled = HFG_IsCallWaitingActive(&context->channels[channelId].c.hfgChannel);

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}


#if HFG_SNIFF_TIMER >= 0

BtStatus BTL_AG_SetSniffTimer(BtlAgContext *context,
							  TimeT time)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U32 nChannels;
	BtlAgChannel *channel;

	BTL_FUNC_START_AND_LOCK("BTL_AG_SetSniffTimer");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((BTL_AG_SERVICE_HFAG & context->service), BT_STATUS_INVALID_PARM,
		("HFAG Service is not active"));

	channel = context->channels;

	for (nChannels = BTL_HFAG_MAX_CHANNELS; 0 < nChannels; --nChannels)
	{
		if (BTL_AG_CHANNEL_STATE_IDLE == channel->state)
		{
			channel->c.hfgChannel.sniffTimer = time;
			if (TRUE == HFG_IsSniffModeEnabled(&channel->c.hfgChannel))
			{
				status = HFG_EnableSniffMode(&channel->c.hfgChannel,FALSE);
				BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("HFG_EnableSniffMode failed"));

				status = HFG_EnableSniffMode(&channel->c.hfgChannel, TRUE);
				BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("HFG_EnableSniffMode failed"));
			}
		}
		++channel;
	}
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_AG_GetSniffTimer(BtlAgContext *context,
							  TimeT *time)
{
	BtStatus status = BT_STATUS_FAILED;
	U32 nChannels;
	BtlAgChannel *channel;


	BTL_FUNC_START_AND_LOCK("BTL_AG_GetSniffTimer");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((0 != time), BT_STATUS_INVALID_PARM,("null time"));

	BTL_VERIFY_ERR((BTL_AG_SERVICE_HFAG & context->service), BT_STATUS_INVALID_PARM,
		("HFAG Service is not active"));

	channel = context->channels;

	for (nChannels = BTL_HFAG_MAX_CHANNELS; 0 < nChannels; --nChannels)
	{
		if (BTL_AG_CHANNEL_STATE_IDLE == channel->state)
		{
			*time = channel->c.hfgChannel.sniffTimer;
			status = BT_STATUS_SUCCESS;
			break;
		}
		++channel;
	}

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_AG_SetSniffParams(BtlAgContext *context, HfgSniffParams * sniffParams)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U32 nChannels;
	BtlAgChannel *channel;

	BTL_FUNC_START_AND_LOCK("BTL_AG_SetSniffParams");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((BTL_AG_SERVICE_HFAG & context->service), BT_STATUS_INVALID_PARM,
		("HFAG Service is not active"));

	agModuleData.sniffParams = *sniffParams;
	
	channel = context->channels;

	for (nChannels = BTL_HFAG_MAX_CHANNELS; 0 < nChannels; --nChannels)
	{
		if (channel->state >= BTL_AG_CHANNEL_STATE_IDLE)
		{
			channel->c.hfgChannel.sniffTimer = sniffParams->sniffTimer;
			channel->c.hfgChannel.minInterval = sniffParams->minInterval;
			channel->c.hfgChannel.maxInterval = sniffParams->maxInterval;
			channel->c.hfgChannel.attempt = sniffParams->attempt;
			channel->c.hfgChannel.timeout = sniffParams->timeout;
				
			if (TRUE == HFG_IsSniffModeEnabled(&channel->c.hfgChannel))
			{
				status = HFG_EnableSniffMode(&channel->c.hfgChannel,FALSE);
				BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("HFG_EnableSniffMode failed"));

				status = HFG_EnableSniffMode(&channel->c.hfgChannel, TRUE);
				BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("HFG_EnableSniffMode failed"));
			}
		}
		++channel;
	}
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_AG_GetSniffParams(BtlAgContext *context, HfgSniffParams * sniffParams)
{
	BtStatus status = BT_STATUS_FAILED;

	BTL_FUNC_START_AND_LOCK("BTL_AG_GetSniffParams");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));
	BTL_VERIFY_ERR((0 != sniffParams), BT_STATUS_INVALID_PARM,("null sniffParams"));

	BTL_VERIFY_ERR((BTL_AG_SERVICE_HFAG & context->service), BT_STATUS_INVALID_PARM,
		("HFAG Service is not active"));
	
	*sniffParams = agModuleData.sniffParams;

	BTL_FUNC_END_AND_UNLOCK();

	return BT_STATUS_SUCCESS;
}

BtStatus BTL_AG_GetSniffMode(BtlAgContext *context,
							 BtlAgChannelId channelId,
							 BOOL *enabled)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK("BTL_AG_GetSniffMode");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((0 != enabled), BT_STATUS_INVALID_PARM,("null enabled"));

	BTL_VERIFY_ERR((BTL_AG_SERVICE_HFAG & context->service), BT_STATUS_INVALID_PARM,
		("HFAG Service is not active"));

	BTL_VERIFY_ERR((BTL_HFAG_MAX_CHANNELS > channelId),
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	*enabled = HFG_IsSniffModeEnabled(&context->channels[channelId].c.hfgChannel);

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}


BtStatus BTL_AG_SetSniffMode(BtlAgContext *context,
							 BtlAgChannelId channelId,
							 BOOL enabled)
{
	BtStatus status;

	BTL_FUNC_START_AND_LOCK("BTL_AG_SetSniffMode");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((BTL_AG_SERVICE_HFAG & context->service), BT_STATUS_INVALID_PARM,
		("HFAG Service is not active"));

	BTL_VERIFY_ERR((BTL_HFAG_MAX_CHANNELS > channelId),
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	status = HFG_EnableSniffMode(&context->channels[channelId].c.hfgChannel, enabled);

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}
#endif /* HFG_SNIFF_TIMER >= 0 */

BtStatus BTL_AG_GetSniffExitPolicy(BtlAgContext *context,
								   BtlAgChannelId channelId,
								   HfgSniffExitPolicy *policy)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK("BTL_AG_GetSniffExitPolicy");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((0 != policy), BT_STATUS_INVALID_PARM,("null policy"));

	BTL_VERIFY_ERR((BTL_AG_SERVICE_HFAG & context->service), BT_STATUS_INVALID_PARM,
		("HFAG Service is not active"));

	BTL_VERIFY_ERR((BTL_HFAG_MAX_CHANNELS > channelId),
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	*policy = HFG_GetSniffExitPolicy(&context->channels[channelId].c.hfgChannel);

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}


BtStatus BTL_AG_SetSniffExitPolicy(BtlAgContext *context,
								   BtlAgChannelId channelId,
								   HfgSniffExitPolicy policy)
{
	BtStatus status;

	BTL_FUNC_START_AND_LOCK("BTL_AG_SetSniffExitPolicy");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	BTL_VERIFY_ERR((BTL_AG_SERVICE_HFAG & context->service), BT_STATUS_INVALID_PARM,
		("HFAG Service is not active"));

	BTL_VERIFY_ERR((BTL_HFAG_MAX_CHANNELS > channelId),
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	status = HFG_SetSniffExitPolicy(&context->channels[channelId].c.hfgChannel, policy);

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}


static BtlAgChannelType BtlAgGetChannelType(const BtlAgChannel *channel)
{
	BtlAgChannelType type;

	BTL_FUNC_START("BtlAgGetChannelType");

	type = channel->type;

	BTL_FUNC_END();

	return type;
}

static BtStatus BtlAgRegisterChannel(BtlAgChannel *channel)
{
	BtStatus status;

	BTL_FUNC_START("BtlAgRegisterChannel");

	if (BTL_AG_CHANNEL_TYPE_HANDSFREE == BtlAgGetChannelType(channel))
	{	
		channel->audioFormat = BTL_HFAG_DEFAULT_AUDIO_FORMAT;
		channel->audioParms = BTL_HFAG_DEFAULT_AUDIO_PARMS;
		status = HFG_Register(&channel->c.hfgChannel, BtlAgHfCallback);
	}
	else
	{
		channel->audioFormat = BTL_HSAG_DEFAULT_AUDIO_FORMAT;
		channel->audioParms = BTL_HSAG_DEFAULT_AUDIO_PARMS;
		status = HSG_Register(&channel->c.hsgChannel, BtlAgHsCallback);
	}

	BTL_FUNC_END();

	return status;
}

static BtStatus BtlAgDeregisterChannel(BtlAgChannel *channel)
{
	BtStatus status;

	BTL_FUNC_START("BtlAgDeregisterChannel");

	if (BTL_AG_CHANNEL_TYPE_HANDSFREE == BtlAgGetChannelType(channel))
	{
		status = HFG_Deregister(&channel->c.hfgChannel);
	}
	else
	{
		status = HSG_Deregister(&channel->c.hsgChannel);
	}

	BTL_FUNC_END();

	return status;
}

static BtStatus BtlAgDisconnectServiceLink(BtlAgChannel *channel)
{
	BtStatus status;

	BTL_FUNC_START("BtlAgDisconnectServiceLink");

	if (BTL_AG_CHANNEL_TYPE_HANDSFREE == BtlAgGetChannelType(channel))
	{
		status = HFG_DisconnectServiceLink(&channel->c.hfgChannel);
	}
	else
	{
		status = HSG_DisconnectServiceLink(&channel->c.hsgChannel);
	}

	BTL_FUNC_END();

	return status;
}


static BtStatus BtlAgCreateAudioLink(BtlAgChannel *channel)
{
	BtStatus status;

	BTL_FUNC_START("BtlAgCreateAudioLink");

	CMGR_SetAudioDefaultParms(channel->audioParms);
	CMGR_SetAudioVoiceSettings(channel->audioFormat);

	if (BTL_AG_CHANNEL_TYPE_HANDSFREE == BtlAgGetChannelType(channel))
	{
		status = HFG_CreateAudioLink(&channel->c.hfgChannel);
	}
	else
	{
		status = HSG_CreateAudioLink(&channel->c.hsgChannel);
	}

	BTL_FUNC_END();

	return status;
}


static BtStatus BtlAgDisconnectAudioLink(BtlAgChannel *channel)
{
	BtStatus status;

	BTL_FUNC_START("BtlAgDisconnectAudioLink");

	if (BTL_AG_CHANNEL_TYPE_HANDSFREE == BtlAgGetChannelType(channel))
	{
		status = HFG_DisconnectAudioLink(&channel->c.hfgChannel);
	}
	else
	{
		status = HSG_DisconnectAudioLink(&channel->c.hsgChannel);
	}

	BTL_FUNC_END();

	return status;
}

static BtStatus BtlAgCreateServiceLink(BtlAgChannel *channel,
									   BD_ADDR *bdAddr)
{
	BtStatus status;

	BTL_FUNC_START("BtlAgCreateServiceLink");

	if (BTL_AG_CHANNEL_TYPE_HANDSFREE == BtlAgGetChannelType(channel))
	{
		status = HFG_CreateServiceLink(&channel->c.hfgChannel, bdAddr);
	}
	else
	{
		status = HSG_CreateServiceLink(&channel->c.hsgChannel, bdAddr);
	}

	BTL_FUNC_END();

	return status;
}

static BtlAgChannel* BtlAgFindFreeChannel(BtlAgContext *context)
{
	BtlAgChannel *channel;
	U32 nChannels;

	BTL_FUNC_START("BtlAgFindFreeChannel");

	channel = context->channels;
	for (nChannels = BTL_HFAG_MAX_CHANNELS + BTL_HSAG_MAX_CHANNELS; nChannels > 0; --nChannels)
	{
		if (BTL_AG_CHANNEL_STATE_IDLE == channel->state)
		{
			break;
		}
		++channel;
	}
	if (0 == nChannels)
	{
		channel = 0;
	}

	BTL_FUNC_END();

	return channel;
}

static BtStatus BtlAgSendResults(BtlAgChannel *channel,
							 AtCommand command,
							 U32 parms,
							 U16 parmLen,
							 BOOL done)
{
	BtStatus status;
	AtResults *response;

	BTL_FUNC_START("BtlAgSendResults");

	status = BTL_POOL_Allocate(&agModuleData.responsesPool, (void**)&response);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), BT_STATUS_NO_RESOURCES,
		("BTL_POOL_Allocate failed"));

	if (BTL_AG_CHANNEL_TYPE_HEADSET == BtlAgGetChannelType(channel) &&
		(AT_OK == command || AT_RING == command || AT_ERROR == command ||
		 AT_SPEAKER_GAIN == command || AT_MICROPHONE_GAIN == command))
	{
		status = HSG_SendHsResults(&channel->c.hsgChannel, command, parms, parmLen, response, done);

	}
	else if (BTL_AG_CHANNEL_TYPE_HANDSFREE == BtlAgGetChannelType(channel))
	{
		status = HFG_SendHfResults(&channel->c.hfgChannel, command, parms, parmLen, response, done);
	}
	else
	{
		status = BT_STATUS_FAILED;
	}

	if (BT_STATUS_PENDING != status)
	{
		BtStatus tempStatus;
		tempStatus = BTL_POOL_Free(&agModuleData.responsesPool, (void**)&response);
		BTL_VERIFY_FATAL_NORET((BT_STATUS_SUCCESS == tempStatus),	("BTL_POOL_Free failed"));
	}

	BTL_FUNC_END();

	return status;
}

static void BtlAgHfCallback(HfgChannel *hfgChannel,
							HfgCallbackParms *parms)
{
	BtlAgChannel *channel;
	BtlAgContext *context;

	BTL_FUNC_START("BtlAgHfCallback");

	channel = ContainingRecord(hfgChannel, BtlAgChannel, c.hfgChannel);
	context = channel->context;

	switch (context->state)
	{
	case BTL_AG_CONTEXT_STATE_ENABLED:
		BtlAgEnabledHfagEvent(channel, parms);
		break;

	case BTL_AG_CONTEXT_STATE_DISABLING:
		BtlAgDisablingHfagEvent(channel, parms);
		break;

	default:
		BTL_FATAL_NO_RETVAR(("Illegal context state"));
	}

	BTL_FUNC_END();
}

static void BtlAgHsCallback(HsgChannel *hsgChannel,
							HsgCallbackParms *parms)
{
	BtlAgChannel *channel;
	BtlAgContext *context;

	BTL_FUNC_START("BtlAgHfCallback");

	channel = ContainingRecord(hsgChannel, BtlAgChannel, c.hsgChannel);
	context = channel->context;

	switch (context->state)
	{
	case BTL_AG_CONTEXT_STATE_ENABLED:
		BtlAgEnabledHsagEvent(channel, parms);
		break;

	case BTL_AG_CONTEXT_STATE_DISABLING:
		BtlAgDisablingHsagEvent(channel, parms);
		break;

	default:
		BTL_FATAL_NO_RETVAR(("Illegal context state"));
	}

	BTL_FUNC_END();
}


static void BtlAgEnabledHfagEvent(BtlAgChannel *channel,
								  HfgCallbackParms *parms)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtlAgEvent event;
	HfgAtData atData;

	BTL_FUNC_START("BtlHfagHandleEventWhenEnabled");

	event.channelId = channel - channel->context->channels;
	event.context = channel->context;
	event.service = BTL_AG_SERVICE_HFAG;
	event.p.hfgParms = parms;
	
	switch (parms->event)
	{
	case HFG_EVENT_SERVICE_CONNECT_REQ:
		if (TRUE == agModuleData.isRadioOffNotified)
		{
			status = HFG_RejectIncomingSLC(&channel->c.hfgChannel);
			BTL_VERIFY_ERR_NO_RETVAR((BT_STATUS_PENDING == status),
				("HFG_RejectIncomingSLC() failed: %s", pBT_Status(status)));
			channel->state = BTL_AG_CHANNEL_STATE_DISCONNECTING_SLC;
			/*	The incoming channel is rejected no need to notify the user about 
			 *	the SLC request.
			 */
			BTL_RET_NO_RETVAR();
		}
		
		status = HFG_AcceptIncomingSLC(&channel->c.hfgChannel, &parms->p.remDev->bdAddr);
		BTL_VERIFY_ERR_NO_RETVAR((BT_STATUS_PENDING == status),
			("HFG_AcceptIncomingSLC() failed: %s", pBT_Status(status)));
		channel->state = BTL_AG_CHANNEL_STATE_CONNECTING_SLC;
		event.type = BTL_AG_EVENT_SERVICE_CONNECT_REQ;
		break;

 	case HFG_EVENT_SERVICE_CONNECTED:
		if (TRUE == agModuleData.isRadioOffNotified)
		{
			status = BTL_AG_DisconnectSLC(channel->context, 
				(BtlAgChannelId)(channel - channel->context->channels));
			BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == status),
				("BTL_AG_DisconnectSLC() failed: %s", pBT_Status(status)));
			/*	Since we are in the process of turning the radio off
			 *	no need to inform the user about new connections.
			 */				
			BTL_RET_NO_RETVAR();
		}
		
		event.type = BTL_AG_EVENT_SERVICE_CONNECTED;
		if (BEC_NO_ERROR == parms->errCode)
		{
			++channel->context->numChannels;
			channel->state = BTL_AG_CHANNEL_STATE_SLC_CONNECTED;
		}
		else
		{
			channel->state = BTL_AG_CHANNEL_STATE_IDLE;
		}

		/* Pass event to the BSC module */
		BtlAgSendEventToBsc(BTL_BSC_EVENT_SLC_CONNECTED,
			channel, &event.p.hfgParms->p.remDev->bdAddr);
		break;

	case HFG_EVENT_SERVICE_DISCONNECTED:
		event.type = BTL_AG_EVENT_SERVICE_DISCONNECTED;
		/* The event may arrive after cancellation, before the channel is
		 * established */
		if (channel->c.hfgChannel.cancelCreateConn == FALSE)
		{
			--channel->context->numChannels;
		}
		channel->state = BTL_AG_CHANNEL_STATE_IDLE;

		/* Pass event to the BSC module */
		BtlAgSendEventToBsc(BTL_BSC_EVENT_SLC_DISCONNECTED, channel, NULL);

		if (TRUE == agModuleData.isRadioOffNotified && 0 == channel->context->numChannels)
		{
			BtlModuleNotificationCompletionEvent moduleCompletionEvent;

			moduleCompletionEvent.type = BTL_MODULE_NOTIFICATION_COMPLETION_EVENT_TYPE_RADIO_OFF;
			moduleCompletionEvent.status = BT_STATUS_SUCCESS;
			BTL_ModuleCompleted(BTL_MODULE_TYPE_AG, &moduleCompletionEvent);
			agModuleData.isRadioOffNotified = FALSE;
			BTL_RET_NO_RETVAR();
		}
		break;

	case HFG_EVENT_AUDIO_CONNECT_REQ:
		if (BT_STATUS_SUCCESS == parms->status)
		{
			CMGR_SetAudioVoiceSettings(channel->audioFormat);
			CMGR_SetAudioDefaultParms(channel->audioParms);
			status = HFG_AcceptAudioLink(&channel->c.hfgChannel, BEC_NO_ERROR);
			BTL_VERIFY_FATAL_NORET((BT_STATUS_PENDING == status), ("HFG_AcceptAudioLink() failed"));
			channel->state = BTL_AG_CHANNEL_STATE_CONNECTING_AUDIO;
		}
		event.type = BTL_AG_EVENT_AUDIO_CONNECT_REQ;
		break;
		
	case HFG_EVENT_AUDIO_CONNECTED:
		if (BEC_NO_ERROR == parms->errCode)
		{
			channel->state = BTL_AG_CHANNEL_STATE_AUDIO_CONNECTED;
		}
		event.type = BTL_AG_EVENT_AUDIO_CONNECTED;
		break;
		
	case HFG_EVENT_AUDIO_DISCONNECTED:
		channel->state = BTL_AG_CHANNEL_STATE_SLC_CONNECTED;
		event.type = BTL_AG_EVENT_AUDIO_DISCONNECTED;
		break;
		
	case HFG_EVENT_RESPONSE_COMPLETE:
		status = BTL_POOL_Free(&agModuleData.responsesPool, (void**)&parms->p.response);
		BTL_VERIFY_FATAL_NO_RETVAR((BT_STATUS_SUCCESS == status), ("BTL_POOL_Free() failed"));
		event.type = BTL_AG_EVENT_RESPONSE_COMPLETE;
		break;

	case HFG_EVENT_AT_COMMAND_DATA:
		atData.rawData = channel->c.hfgChannel.atBuffer.buff;
		atData.dataLen = channel->c.hfgChannel.atBuffer.buffSize;
		parms->p.data = &atData;
		event.type = BTL_AG_EVENT_AT_COMMAND_DATA;
		break;

	case HFG_EVENT_HANDSFREE_FEATURES:
		event.type = BTL_AG_EVENT_HANDSFREE_FEATURES;
		break;

#if (BT_SCO_HCI_DATA == XA_ENABLED)

	case HFG_EVENT_AUDIO_DATA:
		event.type = BTL_AG_EVENT_AUDIO_DATA;
		break;

	case HFG_EVENT_AUDIO_DATA_SENT:
		event.type = BTL_AG_EVENT_AUDIO_DATA_SENT;
		break;
		
#endif /* BT_SCO_HCI_DATA == XA_ENABLED */

	case HFG_EVENT_ANSWER_CALL:
		event.type = BTL_AG_EVENT_ANSWER_CALL;
		break;

	case HFG_EVENT_DIAL_NUMBER:
		event.type = BTL_AG_EVENT_DIAL_NUMBER;
		break;

	case HFG_EVENT_MEMORY_DIAL:
		event.type = BTL_AG_EVENT_MEMORY_DIAL;
		break;

	case HFG_EVENT_REDIAL:
		event.type = BTL_AG_EVENT_REDIAL;
		break;

	case HFG_EVENT_CALL_HOLD:
		event.type = BTL_AG_EVENT_CALL_HOLD;
		break;

#if HFG_USE_RESP_HOLD == XA_ENABLED
	case HFG_QUERY_RESPONSE_HOLD:
		event.type = BTL_AG_EVENT_QUERY_RESPONSE_HOLD;
		break;

	case HFG_RESPONSE_HOLD:
		event.type = BTL_AG_EVENT_RESPONSE_HOLD;
		break;
#endif /* HFG_USE_RESP_HOLD == XA_ENABLED */

	case HFG_EVENT_HANGUP:
		event.type = BTL_AG_EVENT_HANGUP;
		break;

	case HFG_EVENT_LIST_CURRENT_CALLS:
		event.type = BTL_AG_EVENT_LIST_CURRENT_CALLS;
		break;

	case HFG_EVENT_ENABLE_CALLER_ID:
		event.type = BTL_AG_EVENT_ENABLE_CALLER_ID;
		break;

	case HFG_EVENT_ENABLE_CALL_WAITING:
		event.type = BTL_AG_EVENT_ENABLE_CALL_WAITING;
		break;

	case HFG_EVENT_GENERATE_DTMF:
		event.type = BTL_AG_EVENT_GENERATE_DTMF;
		break;

	case HFG_EVENT_GET_LAST_VOICE_TAG:
		event.type = BTL_AG_EVENT_GET_LAST_VOICE_TAG;
		break;

	case HFG_EVENT_ENABLE_VOICE_RECOGNITION:
		event.type = BTL_AG_EVENT_ENABLE_VOICE_RECOGNITION;
		break;

	case HFG_EVENT_DISABLE_NREC:
		event.type = BTL_AG_EVENT_DISABLE_NREC;
		break;

	case HFG_EVENT_REPORT_MIC_VOLUME:
		event.type = BTL_AG_EVENT_REPORT_MIC_VOLUME;
		break;

	case HFG_EVENT_REPORT_SPK_VOLUME:
		event.type = BTL_AG_EVENT_REPORT_SPK_VOLUME;
		break;

	case HFG_EVENT_QUERY_NETWORK_OPERATOR:
		event.type = BTL_AG_EVENT_QUERY_NETWORK_OPERATOR;
		break;

	case HFG_EVENT_QUERY_SUBSCRIBER_NUMBER:
		event.type = BTL_AG_EVENT_QUERY_SUBSCRIBER_NUMBER;
		break;

	case HFG_EVENT_ENABLE_EXTENDED_ERRORS:
		event.type = BTL_AG_EVENT_ENABLE_EXTENDED_ERRORS;
		break;

	default:
		BTL_FATAL_NORET(("Unexpected HFG Event"));
	}
	channel->context->callback(&event);
	
	BTL_FUNC_END();
}

static void BtlAgDisablingHfagEvent(BtlAgChannel *channel,
									HfgCallbackParms *parms)
{
	BtStatus status;

	BTL_FUNC_START("BtlAgDisablingHfagEvent");

	switch (parms->event)
	{
	case HFG_EVENT_SERVICE_CONNECT_REQ:
		status = HFG_RejectIncomingSLC(&channel->c.hfgChannel);
		BTL_VERIFY_ERR_NO_RETVAR((BT_STATUS_PENDING == status),
			("HFG_RejectIncomingSLC() failed: %s", pBT_Status(status)));
		break;

	case HFG_EVENT_SERVICE_CONNECTED:
		++channel->context->numChannels;
		status = HFG_DisconnectServiceLink(&channel->c.hfgChannel);
		BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == status || BT_STATUS_PENDING == status),
			("HFG_DisconnectServiceLink() failed: %s",pBT_Status(status)));
		break;

	case HFG_EVENT_SERVICE_DISCONNECTED:
		--channel->context->numChannels;
		status = HFG_Deregister(&channel->c.hfgChannel);
		BTL_VERIFY_FATAL_NO_RETVAR((BT_STATUS_SUCCESS == status), ("HFG_Deregister() failed"));

		if (0 == channel->context->numChannels)
		{
			BtlAgEvent event;

			channel->context->state = BTL_AG_CONTEXT_STATE_DISABLED;
			agModuleData.activeServices &= ~channel->context->service;
			event.channelId = channel - channel->context->channels;
			event.context = channel->context;
			event.service = channel->context->service;
			event.type = BTL_AG_EVENT_CONTEXT_DISABLED;
			channel->context->callback(&event);
		}
		break;

	case HFG_EVENT_AUDIO_CONNECT_REQ:
		/* Reject the audio request */
		HFG_AcceptAudioLink(&channel->c.hfgChannel, BEC_USER_TERMINATED);
		break;

	case HFG_EVENT_RESPONSE_COMPLETE:
		status = BTL_POOL_Free(&agModuleData.responsesPool, (void**)&parms->p.response);
		BTL_VERIFY_FATAL_NO_RETVAR((BT_STATUS_SUCCESS == status), ("BTL_POOL_Free failed"));
		break;

	default:
		break;
	}

	BTL_FUNC_END();
}

static void BtlAgEnabledHsagEvent(BtlAgChannel *channel, HsgCallbackParms *parms)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtlAgEvent event;

	BTL_FUNC_START("BtlAgEnabledHsagEvent");

	event.channelId = channel - channel->context->channels;
	event.context = channel->context;
	event.service = BTL_AG_SERVICE_HSAG;
	event.p.hsgParms = parms;

	switch (parms->event)
	{
	case HSG_EVENT_SERVICE_CONNECT_REQ:
		if (TRUE == agModuleData.isRadioOffNotified)
		{
			status = HSG_RejectIncomingSLC(&channel->c.hsgChannel);
			BTL_VERIFY_ERR_NO_RETVAR((BT_STATUS_PENDING == status),
				("HSG_RejectIncomingSLC() failed: %s", pBT_Status(status)));
			channel->state = BTL_AG_CHANNEL_STATE_DISCONNECTING_SLC;
			/*	The incoming channel is rejected no need to notify the user about 
			 *	the SLC request.
			 */
			BTL_RET_NO_RETVAR();
		}

		status = HSG_AcceptIncomingSLC(&channel->c.hsgChannel, &parms->p.remDev->bdAddr);
		BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == status),
			("HSG_AcceptIncomingSLC() failed: %s", pBT_Status(status)));
		channel->state = BTL_AG_CHANNEL_STATE_CONNECTING_SLC;
		event.type = BTL_AG_EVENT_SERVICE_CONNECT_REQ;
		break;

 	case HSG_EVENT_SERVICE_CONNECTED:

		if (TRUE == agModuleData.isRadioOffNotified)
		{
			status = BTL_AG_DisconnectSLC(channel->context, 
				(BtlAgChannelId)(channel - channel->context->channels));
			BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == status),
				("BTL_AG_DisconnectSLC() failed: %s", pBT_Status(status)));
			/*	Since we are in the process of turning the radio off
			 *	no need to inform the user about new connections.
			 */				
			BTL_RET_NO_RETVAR();
		}
		
		event.type = BTL_AG_EVENT_SERVICE_CONNECTED;
		if (BEC_NO_ERROR == parms->errCode)
		{
			++channel->context->numChannels;
			channel->state = BTL_AG_CHANNEL_STATE_SLC_CONNECTED;
		}
		else
		{
			channel->state = BTL_AG_CHANNEL_STATE_IDLE;
		}
		break;

	case HSG_EVENT_SERVICE_DISCONNECTED:
		event.type = BTL_AG_EVENT_SERVICE_DISCONNECTED;
		/*	The event may arrive after cancellation, before the channel is
		 *	established.
		 */
		if (channel->c.hsgChannel.cancelCreateConn == FALSE) {
			--channel->context->numChannels;
		}
		channel->state = BTL_AG_CHANNEL_STATE_IDLE;

		if (TRUE == agModuleData.isRadioOffNotified && 0 == channel->context->numChannels)
		{
			BtlModuleNotificationCompletionEvent moduleCompletionEvent;

			moduleCompletionEvent.type = BTL_MODULE_NOTIFICATION_COMPLETION_EVENT_TYPE_RADIO_OFF;
			moduleCompletionEvent.status = BT_STATUS_SUCCESS;
			BTL_ModuleCompleted(BTL_MODULE_TYPE_AG, &moduleCompletionEvent);
			agModuleData.isRadioOffNotified = FALSE;
			BTL_RET_NO_RETVAR();
		}
		break;

	case HSG_EVENT_AUDIO_CONNECT_REQ:
		if (BT_STATUS_SUCCESS == parms->status)
		{
			CMGR_SetAudioVoiceSettings(channel->audioFormat);
			CMGR_SetAudioDefaultParms(channel->audioParms);
			status = HSG_AcceptAudioLink(&channel->c.hsgChannel, BEC_NO_ERROR);
			BTL_VERIFY_FATAL_NORET((BT_STATUS_PENDING == status), ("HSG_AcceptAudioLink() failed"));
			channel->state = BTL_AG_CHANNEL_STATE_CONNECTING_AUDIO;
		}
		event.type = BTL_AG_EVENT_AUDIO_CONNECT_REQ;
		break;

	case HSG_EVENT_AUDIO_CONNECTED:
		channel->state = BTL_AG_CHANNEL_STATE_AUDIO_CONNECTED;
		event.type = BTL_AG_EVENT_AUDIO_CONNECTED;
		break;
		
	case HSG_EVENT_AUDIO_DISCONNECTED:
		channel->state = BTL_AG_CHANNEL_STATE_SLC_CONNECTED;
		event.type = BTL_AG_EVENT_AUDIO_DISCONNECTED;
		break;
		
	case HSG_EVENT_RESPONSE_COMPLETE:
		status = BTL_POOL_Free(&agModuleData.responsesPool, (void**)&parms->p.response);
		BTL_VERIFY_FATAL_NORET((BT_STATUS_SUCCESS == status), ("BTL_POOL_Free() failed"));
		event.type = BTL_AG_EVENT_RESPONSE_COMPLETE;
		break;

	case HSG_EVENT_BUTTON_PRESSED:
		event.type = BTL_AG_EVENT_BUTTON_PRESSED;
		break;

	case HSG_EVENT_REPORT_MIC_VOLUME:
		event.type = BTL_AG_EVENT_REPORT_MIC_VOLUME;
		break;

	case HSG_EVENT_REPORT_SPK_VOLUME:
		event.type = BTL_AG_EVENT_REPORT_SPK_VOLUME;
		break;

	case HSG_EVENT_AT_COMMAND_DATA:
		event.type = BTL_AG_EVENT_AT_COMMAND_DATA;
		break;

#if (BT_SCO_HCI_DATA == XA_ENABLED)

	case HSG_EVENT_AUDIO_DATA:
		event.type = BTL_AG_EVENT_AUDIO_DATA;
		break;

	case HSG_EVENT_AUDIO_DATA_SENT:
		event.type = BTL_AG_EVENT_AUDIO_DATA_SENT;
		break;

#endif /* BT_SCO_HCI_DATA == XA_ENABLED */

	default:
		BTL_FATAL_NORET(("Unexpected HSG Event"));
	}
	channel->context->callback(&event);

	BTL_FUNC_END();
}

static void BtlAgDisablingHsagEvent(BtlAgChannel *channel, HsgCallbackParms *parms)
{
	BtStatus status;

	BTL_FUNC_START("BtlAgDisablingHsagEvent");

	switch (parms->event)
	{
	case HSG_EVENT_SERVICE_CONNECT_REQ:
		status = HSG_RejectIncomingSLC(&channel->c.hsgChannel);
		BTL_VERIFY_ERR_NO_RETVAR((BT_STATUS_PENDING == status),
			("HSG_RejectIncomingSLC() failed: %s", pBT_Status(status)));
		break;

	case HSG_EVENT_SERVICE_CONNECTED:
		++channel->context->numChannels;
		status = HSG_DisconnectServiceLink(&channel->c.hsgChannel);
		BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == status || BT_STATUS_PENDING == status),
			("HSG_DisconnectServiceLink() failed: %s",pBT_Status(status)));
		break;

	case HSG_EVENT_SERVICE_DISCONNECTED:
		--channel->context->numChannels;
		status = HSG_Deregister(&channel->c.hsgChannel);
		BTL_VERIFY_FATAL_NO_RETVAR((BT_STATUS_SUCCESS == status), ("HSG_Deregister() failed"));

		if (0 == channel->context->numChannels)
		{
			BtlAgEvent event;

			channel->context->state = BTL_AG_CONTEXT_STATE_DISABLED;
			agModuleData.activeServices &= ~channel->context->service;
			event.channelId = channel - channel->context->channels;
			event.context = channel->context;
			event.service = channel->context->service;
			event.type = BTL_AG_EVENT_CONTEXT_DISABLED;
			channel->context->callback(&event);
		}
		break;

	case HSG_EVENT_AUDIO_CONNECT_REQ:
		/* Reject the audio request */
		HSG_AcceptAudioLink(&channel->c.hsgChannel, BEC_USER_TERMINATED);
		break;

	case HSG_EVENT_RESPONSE_COMPLETE:
		status = BTL_POOL_Free(&agModuleData.responsesPool, (void**)&parms->p.response);
		BTL_VERIFY_FATAL_NO_RETVAR((BT_STATUS_SUCCESS == status), ("BTL_POOL_Free() failed"));
		break;

	default:
		break;
	}

	BTL_FUNC_END();
}

/*-------------------------------------------------------------------------------
 * BtlAgSendEventToBsc()
 *
 *		Sends event to BTL_BSC module.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		eventType [in] - type of event to be sent.
 *
 *      agChannel [in] - pointer to BtlAgChannel structure.
 *
 *      bdAddr [in] - pointer to BD_ADDR structure or NULL.
 *
 * Returns:
 *		none
 */
static void BtlAgSendEventToBsc(BtlBscEventType eventType,
                                BtlAgChannel *agChannel,
                                BD_ADDR *bdAddr)
{
    BtlBscEvent bscEvent;

	BTL_FUNC_START("BtlAgSendEventToBsc");

    /* Fill event's data */
    bscEvent.type = eventType;
    bscEvent.source = BTL_BSC_EVENT_SOURCE_AG;
    bscEvent.info.channel = (U32)agChannel;
    bscEvent.isBdAddrValid = TRUE;
        
	if (BTL_AG_CHANNEL_TYPE_HANDSFREE == BtlAgGetChannelType(agChannel))
	{
		if (NULL != bdAddr)
		{
            bscEvent.info.bdAddr = *bdAddr;
		}
        else if (NULL != agChannel->c.hfgChannel.cmgrHandler.bdc)
        {
            bscEvent.info.bdAddr = agChannel->c.hfgChannel.cmgrHandler.bdc->link->bdAddr;
        }
        else
        {
            bscEvent.isBdAddrValid = FALSE;
        }
	}
	else
	{
		if (NULL != bdAddr)
		{
            bscEvent.info.bdAddr = *bdAddr;
		}
        else if (NULL != agChannel->c.hsgChannel.cmgrHandler.bdc)
        {
		    bscEvent.info.bdAddr = agChannel->c.hsgChannel.cmgrHandler.bdc->link->bdAddr;
        }
        else
        {
            bscEvent.isBdAddrValid = FALSE;
        }
	}
    
    BTL_BSC_SendEvent((const BtlBscEvent *)&bscEvent);

	BTL_FUNC_END();
}


static void BtlAgSendHciCommandCB(const BtEvent *event)
{
	BTL_FUNC_START("BtlAgSendHciCommandCB");
	
	if (event->p.meToken->p.general.out.status == BT_STATUS_SUCCESS)
	{
		if ((HCE_COMMAND_COMPLETE != event->p.meToken->p.general.out.event) &&
			 (HCE_COMMAND_STATUS != event->p.meToken->p.general.out.event))
		{
			BTL_LOG_ERROR(("HCI Command Failed, HCI Event: %d",
							event->p.meToken->p.general.out.event));
		}
		else if (HCE_COMMAND_COMPLETE == event->p.meToken->p.general.out.event)
		{
			if (event->p.meToken->p.general.out.parms[3] != 0)
			{
				BTL_LOG_ERROR(("HCI Command Failed, status: %x",
								event->p.meToken->p.general.out.parms[3]));
			}
			else
			{
				BTL_LOG_ERROR(("HCI Command sent, successfully"));
			}
		}
	}	
	else
	{
		BTL_LOG_ERROR(("HCI Command Failed, status: %x",event->p.meToken->p.general.out.status));
	}
	BTL_FUNC_END();
}

BtStatus BTL_AG_SetFmOverBtMode(BtlAgContext *context,BtlAgChannelId channelId,BOOL enableFMoBT)
{
	static U8 vendorSpecificparms[5] =
	{
		0x01,0x01, 	/* handle  */
		0x01,		/* Enable FoB */
		0x00,		/* PCM IO while in FoB  */
		0x00 		/* I2S IO while in FoB */
	};
	
	BtStatus status = BT_STATUS_SUCCESS;
	BtRemoteDevice	*remDev;
	U16				connectionHandle;
	BtlAgChannel 	*channel;

	BTL_FUNC_START_AND_LOCK("BTL_AG_SetFmOverBtMode");

	channel = &context->channels[channelId];
	remDev = channel->c.hfgChannel.cmgrHandler.bdc->link;

	if (remDev)
	{
		connectionHandle = ME_GetHciConnectionHandle(remDev);

		/* add sco handle to acl handle */
		connectionHandle |=  0x0100; 

 		StoreLE16(&vendorSpecificparms[0], connectionHandle);
	}

	if (TRUE == enableFMoBT)
	{
		vendorSpecificparms[2] = 0x01;
	}
	else
	{
		vendorSpecificparms[2] = 0x00;
	}
	
	status = BtlAgSendHciCommand(FM_OVER_BT_MODE,sizeof(vendorSpecificparms),vendorSpecificparms);
	Assert(status == XA_STATUS_PENDING);	

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

/*-------------------------------------------------------------------------------
 * BtlAgSendHciCommand()
 *
 *		send a command to HCI
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *
 *		hciCommand [in] - the HCI comamnd
 *
 *           parmsLen [in] - length in bytes of the parameter buffer
 *
 *           parms [in] - the parameter buffer
 *
 * Returns:
 *		BT_STATUS_PENDING - the command was successfully sent,
 *           elase various failures
 */
static BtStatus BtlAgSendHciCommand(U16 	hciCommand,
									U8 		parmsLen, 
									U8 		*parms)
{
	BtStatus 			status = BT_STATUS_SUCCESS;
	MeCommandToken		*token;


	BTL_FUNC_START("BtlAgSendHciCommand");

	token = &agModuleData.hciCmdToken;
	
	OS_MemSet((U8*) token, 0, sizeof(MeCommandToken));
	
	token->callback = BtlAgSendHciCommandCB;
	token->p.general.in.hciCommand = hciCommand;
	token->p.general.in.parmLen = parmsLen;
	token->p.general.in.parms = parms;
	token->p.general.in.event = HCE_COMMAND_COMPLETE;
	

	status = ME_SendHciCommandAsync(token);

	BTL_VERIFY_ERR((BT_STATUS_PENDING == status),
			status, ("ME_SendHciCommandSync Failed, Status = %s", pBT_Status(status)));

	BTL_FUNC_END();

	return status;

}
 
BtStatus BTL_AG_SetCallHolfFlags(BtlAgContext *context, 
								 AtHoldFlag callHoldFlags)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK("BTL_AG_SetCallHolfFlags");

	BTL_VERIFY_ERR((0 != context), BT_STATUS_INVALID_PARM,("null context"));

	status = HFG_SetCallHolfFlags(callHoldFlags);
    
    BTL_FUNC_END_AND_UNLOCK();

	return status;
}

static BtStatus BtlAgUpdateChannelIndicators(BtlAgContext *context, BtlAgChannel *channel)
{
	BtStatus status = BT_STATUS_FAILED;
	HfgIndicator indicator;
	HfgChannel *hfgChannel;
	HfgResponse dummy;
	
	BTL_FUNC_START("BtlAgUpdateChannelIndicators");

	BTL_VERIFY_ERR((BTL_AG_CONTEXT_STATE_DISABLED == context->state), BT_STATUS_INVALID_PARM,
		("AG Context must be disabled when calling this function"));

	BTL_VERIFY_ERR((BTL_AG_CHANNEL_TYPE_HANDSFREE == BtlAgGetChannelType(channel)),
		BT_STATUS_INVALID_PARM, ("'channel' argument is a headset channel"));

	hfgChannel = &channel->c.hfgChannel;

	for (indicator = BTL_AG_MAX_INDICATOR; indicator >= BTL_AG_MIN_INDICATOR; --indicator)
	{
		status = HFG_SetIndicatorValue(hfgChannel, indicator, context->indicatorsArray[indicator], &dummy);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR,
			("HFG_SetIndicatorValue returned: %s", pBT_Status(status)));
	}
	
	BTL_FUNC_END();

	return status;
}

static BtStatus BtlAgBtlNotificationsCb(BtlModuleNotificationType notificationType)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START(("BtlAgBtlNotificationsCb"));
	
	BTL_LOG_DEBUG(("BTL_AG: Notification Type: %d", notificationType));
	
	switch (notificationType)
	{
	case BTL_MODULE_NOTIFICATION_RADIO_ON:
		break;

	case BTL_MODULE_NOTIFICATION_RADIO_OFF:
		status = BtlAgProcessRadioOffNotification();
		break;

	default:
		BTL_ERR(BT_STATUS_INTERNAL_ERROR, ("BTL_AG: Invalid notification (%d)", notificationType));
	}

	BTL_FUNC_END();

	return status;
}

static BtStatus BtlAgProcessRadioOffNotification(void)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtlContext *base;

	BTL_FUNC_START(("BtlAgProcessRadioOffNotification"));

	agModuleData.isRadioOffNotified = TRUE;

	IterateList(agModuleData.contextsList, base, BtlContext *)
	{
		BtlAgContext *context;

		context = (BtlAgContext*)base;
		switch (context->state)
		{
		case BTL_AG_CONTEXT_STATE_DISABLED:
			break;

		case BTL_AG_CONTEXT_STATE_DISABLING:
			status = BT_STATUS_PENDING;
			break;

		case BTL_AG_CONTEXT_STATE_ENABLED:
			status = BtlAgRemoveAllConnections(context);
			break;

		default:
			BTL_LOG_FATAL(("BTL AG: Illegal context state: %d", (int)context->state));
			status = BT_STATUS_INTERNAL_ERROR;
			break;
		}
	}

	if (BT_STATUS_PENDING != status)
	{
		agModuleData.isRadioOffNotified = TRUE;	
	}

	BTL_FUNC_END();

	return status;
}

static BtStatus BtlAgRemoveAllConnections(BtlAgContext *context)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtStatus discStatus;
	BtlAgChannel *channel;
	U32 nChannels;

	BTL_FUNC_START(("BtlAgRemoveAllConnections"));

	channel = context->channels;
	for (nChannels = BTL_HFAG_MAX_CHANNELS + BTL_HSAG_MAX_CHANNELS; nChannels > 0; --nChannels)
	{
		switch (channel->state)
		{
		case BTL_AG_CHANNEL_STATE_AUDIO_CONNECTED:
		case BTL_AG_CHANNEL_STATE_CONNECTING_AUDIO:
		case BTL_AG_CHANNEL_STATE_DISCONNECTING_AUDIO:
		case BTL_AG_CHANNEL_STATE_SLC_CONNECTED:
			discStatus = BTL_AG_DisconnectSLC(context, (BtlAgChannelId)(channel - context->channels));
			if (BT_STATUS_PENDING == discStatus)
			{
				status = BT_STATUS_PENDING;
			}
			else
			{
				BTL_LOG_ERROR(("BTL_AG_DisconnectSLC() failed: %s", pBT_Status(discStatus)));
			}
			break;

		case BTL_AG_CHANNEL_STATE_CONNECTING_SLC:
		case BTL_AG_CHANNEL_STATE_DISCONNECTING_SLC:
			/*	If SLC is in the middle of disconnecting, the operation will be finished when the SLC is removed.
			 *	If the SLC is in the middle of connection the operation will be finished when the new SLC
			 *	is disconnected
			 */				
			status = BT_STATUS_PENDING;
			break;
			
		case BTL_AG_CHANNEL_STATE_IDLE:
			break;

		default:
			BTL_LOG_FATAL(("BTL AG: Illegal BtlAGChannel state: %d",(int)channel->state));
			break;
		}
		++channel;
	}

	BTL_FUNC_END();

	return status;
}

#else /*BTL_CONFIG_AG ==   BTL_CONFIG_ENABLED*/

/*-------------------------------------------------------------------------------
 * BTL_AG_Init() - When  BTL_CONFIG_AG is disabled.
 */
BtStatus BTL_AG_Init(void)
{
    
   BTL_LOG_INFO(("BTL_AG_Init()  -  BTL_CONFIG_AG Disabled"));
  

    return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 * BTL_AG_Deinit() - When  BTL_CONFIG_AG is disabled.
 */
BtStatus BTL_AG_Deinit(void)
{
    BTL_LOG_INFO(("BTL_AG_Deinit() -  BTL_CONFIG_AG Disabled"));

    return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------*/



#endif /*BTL_CONFIG_AG ==   BTL_CONFIG_ENABLED*/




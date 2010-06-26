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
*   FILE NAME:      btl_rfcomm.c
*
*   DESCRIPTION:    This file contains the BTL RFCOMM implementation.
*
*   AUTHOR:         Ilan Elias
*
\*******************************************************************************/


#include "btl_rfcomm.h"
#include "btl_config.h"
#include "btl_defs.h"

BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_RFCOMM);

#if BTL_CONFIG_BTL_RFCOMM == BTL_CONFIG_ENABLED

/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/

#include <btl_pool.h>
#include <btl_commoni.h>
#include <rfcomm.h>
#include <conmgr.h>





/********************************************************************************
 *
 * Constants 
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BSL_RFCOMM_DEFAULT constant
 *
 *     Represents default security level for RFCOMM.
 */
#define BSL_RFCOMM_DEFAULT  									(BSL_NO_SECURITY)


/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/* Forward declarations */
typedef struct _BtlRfcommChannel 	BtlRfcommChannel;
typedef struct _BtlRfcommData 		BtlRfcommData;


/*-------------------------------------------------------------------------------
 * BtlRfcommInitState type
 *
 *     Defines the RFCOMM init state.
 */
typedef U8 BtlRfcommInitState;

#define BTL_RFCOMM_INIT_STATE_NOT_INTIALIZED				(0x00)
#define BTL_RFCOMM_INIT_STATE_INITIALIZED					(0x01)
#define BTL_RFCOMM_INIT_STATE_INITIALIZATION_FAILED			(0x02)
#define BTL_RFCOMM_INIT_STATE_DEINITIALIZATION_FAILED		(0x03)


/*-------------------------------------------------------------------------------
 * BtlRfcommChannelState type
 *
 *     Defines the RFCOMM state of a specific context.
 */
typedef U8 BtlRfcommChannelState;

#define BTL_RFCOMM_CHANNEL_STATE_IDLE						(0x00)	/* Client & Server */

#define BTL_RFCOMM_CHANNEL_STATE_SERVER_DISCONNECTED		(0x01)	/* Server only */
#define BTL_RFCOMM_CHANNEL_STATE_SERVER_DISCONNECTING		(0x02)
#define BTL_RFCOMM_CHANNEL_STATE_SERVER_DISCONNECT_IND		(0x03)
#define BTL_RFCOMM_CHANNEL_STATE_SERVER_CONNECT_IND			(0x04)
#define BTL_RFCOMM_CHANNEL_STATE_SERVER_CONNECTED			(0x05)
#define BTL_RFCOMM_CHANNEL_STATE_SERVER_DEREGISTERING		(0x06)
#define BTL_RFCOMM_CHANNEL_STATE_SERVER_MASK				(0x0F)	/* Server mask */

#define BTL_RFCOMM_CHANNEL_STATE_CLIENT_DISCONNECTING		(0x10)	/* Client only */
#define BTL_RFCOMM_CHANNEL_STATE_CLIENT_DISCONNECT_IND		(0x20)
#define BTL_RFCOMM_CHANNEL_STATE_CLIENT_CONNECTING			(0x30)	
#define BTL_RFCOMM_CHANNEL_STATE_CLIENT_CONNECTED			(0x40)
#define BTL_RFCOMM_CHANNEL_STATE_CLIENT_DEREGISTERING		(0x50)
#define BTL_RFCOMM_CHANNEL_STATE_CLIENT_MASK				(0xF0)	/* Client mask */


/*-------------------------------------------------------------------------------
 * BtlRfcommDeregisterStateMask type
 *
 *     Defines the RFCOMM deregister state mask.
 */
typedef U8 BtlRfcommDeregisterStateMask;

#define BTL_RFCOMM_DEREGISTER_STATE_MASK_NONE				(0x00)
#define BTL_RFCOMM_DEREGISTER_STATE_MASK_DEREGISTER_ACTIVE	(0x01)
#define BTL_RFCOMMP_DEREGISTER_STATE_MASK_RADIO_OFF_ACTIVE	(0x02)


/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/


/*-------------------------------------------------------------------------------
 * BtlRfcommChannel structure
 *
 *     Represents BTL RFCOMM channel.
 */
struct _BtlRfcommChannel
{	
	/* Internal RFCOMM channel */
	RfChannel channel;

	/* Current state of the channel */
	BtlRfcommChannelState state;

	/* Connection Manager Handler */
    CmgrHandler	cmgrHandler;

	/* Saved parameters for opening client connection */
	U8 savedServiceId;
	U8 savedCredit;

#if BT_SECURITY == XA_ENABLED

	/* Registered client security record, used only if this channel is a client channel */
	BtSecurityRecord clientSecurityRecord;

#endif	/* BT_SECURITY == XA_ENABLED */

#if RF_SEND_CONTROL == XA_ENABLED

	RfModemStatus modemStatus;
	BOOL modemStatusUsed;

	RfPortSettings portSettings;
	BOOL portSettingsUsed;

#endif /* RF_SEND_CONTROL == XA_ENABLED */

	BtlRfcommChannelId channelId;

	/* This flag indicates the deregister state */
	BtlRfcommDeregisterStateMask deregisterState;
};


/*-------------------------------------------------------------------------------
 * BtlRfcommContext structure
 *
 *     Represents BTL RFCOMM context.
 */
struct _BtlRfcommContext 
{
	/* Must be first field */
	BtlContext base;
	
	/* Internal RFCOMM channels */
	BtlRfcommChannel channels[BTL_CONFIG_RFCOMM_MAX_NUM_CHANNELS_PER_CONTEXT];

	/* Associated service with this context (used only with the server) */
	RfService service;

	/* Associated callback with this context */
	BtlRfcommCallBack callback;

	/* Client channels counter */
	U16 clientCounter;

	/* Server channels counter */
	U16 serverCounter;

#if BT_SECURITY == XA_ENABLED

	/* Registered server security record (used only with the server) */
	BtSecurityRecord serverSecurityRecord;

#endif	/* BT_SECURITY == XA_ENABLED */
};


/*-------------------------------------------------------------------------------
 * BtlRfcommData structure
 *
 *     Represents the data of th BTL RFCOMM module.
 */
struct _BtlRfcommData
{
	/* Pool of RFCOMM contexts */
	BTL_POOL_DECLARE_POOL(contextsPool, contextsMemory, BTL_CONFIG_RFCOMM_MAX_NUM_OF_CONTEXTS, sizeof(BtlRfcommContext));

	/* List of active RFCOMM contexts */
	ListEntry contextsList;

	/* Pool of TX BtPackets */
	BTL_POOL_DECLARE_POOL(packetsPool, packetsMemory, BTL_CONFIG_RFCOMM_MAX_NUM_OF_TX_PACKETS, sizeof(BtPacket));

	/* Event passed to the application */
	BtlRfcommEvent event;
};


/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * btlRfcommInitState
 *
 *     Represents the current init state of RFCOMM module.
 */
static BtlRfcommInitState btlRfcommInitState = BTL_RFCOMM_INIT_STATE_NOT_INTIALIZED;


/*-------------------------------------------------------------------------------
 * btlRfcommData
 *
 *     Represents the data of RFCOMM module.
 */
static BtlRfcommData btlRfcommData;


/*-------------------------------------------------------------------------------
 * btlRfcommContextsPoolName
 *
 *     Represents the name of the RFCOMM contexts pool.
 */
static const char btlRfcommContextsPoolName[] = "RfcommContexts";


/*-------------------------------------------------------------------------------
 * btlRfcommPacketsPoolName
 *
 *     Represents the name of the RFCOMM packets pool.
 */
static const char btlRfcommPacketsPoolName[] = "RfcommPackets";


/********************************************************************************
 *
 * Internal function prototypes
 *
 *******************************************************************************/
static BtStatus BtlOpenClientChannel(BtlRfcommContext *rfcommContext, BtlRfcommChannelId channelId);
static BtStatus BtlDeregisterServerChannel(BtlRfcommContext *rfcommContext, BtlRfcommChannelId channelId);
static void BtlRfcommCmgrCallback(CmgrHandler *Handler, CmgrEvent Event, BtStatus Status);
static void BtlRfcommCallback(RfChannel *chnl, RfCallbackParms *parms);
static BtStatus BtlRfcommBtlNotificationsCb(BtlModuleNotificationType notificationType);


/********************************************************************************
 *
 * Function definitions
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_Init()
 */
BtStatus BTL_RFCOMM_Init(void)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_COMMON("BTL_RFCOMM_Init");
	
	BTL_VERIFY_ERR((BTL_RFCOMM_INIT_STATE_NOT_INTIALIZED == btlRfcommInitState), 
		BT_STATUS_FAILED, ("RFCOMM module is already initialized"));
	
	btlRfcommInitState = BTL_RFCOMM_INIT_STATE_INITIALIZATION_FAILED;

	status = BTL_POOL_Create(&btlRfcommData.contextsPool,
							btlRfcommContextsPoolName,
							btlRfcommData.contextsMemory, 
							BTL_CONFIG_RFCOMM_MAX_NUM_OF_CONTEXTS,
							sizeof(BtlRfcommContext));
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, 
		("RFCOMM contexts pool creation failed"));
	
	InitializeListHead(&btlRfcommData.contextsList);

	status = BTL_POOL_Create(&btlRfcommData.packetsPool,
							btlRfcommPacketsPoolName,
							btlRfcommData.packetsMemory, 
							BTL_CONFIG_RFCOMM_MAX_NUM_OF_TX_PACKETS,
							sizeof(BtPacket));
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, 
		("RFCOMM packets pool creation failed"));

	BTL_RegisterForCommonNotifications(BTL_MODULE_TYPE_RFCOMM, BtlRfcommBtlNotificationsCb);
		
	btlRfcommInitState = BTL_RFCOMM_INIT_STATE_INITIALIZED;

	BTL_FUNC_END_AND_UNLOCK();
	
	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_Deinit()
 */
BtStatus BTL_RFCOMM_Deinit(void)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK_COMMON("BTL_RFCOMM_Deinit");
	
	BTL_VERIFY_ERR((BTL_RFCOMM_INIT_STATE_INITIALIZED == btlRfcommInitState), 
		BT_STATUS_FAILED, ("RFCOMM module is not initialized"));

	btlRfcommInitState = BTL_RFCOMM_INIT_STATE_DEINITIALIZATION_FAILED;

	BTL_DeRegisterFromCommonNotifications(BTL_MODULE_TYPE_RFCOMM);

	BTL_VERIFY_ERR((IsListEmpty(&btlRfcommData.contextsList)), 
		BT_STATUS_FAILED, ("RFCOMM contexts are still active"));

	status = BTL_POOL_Destroy(&btlRfcommData.contextsPool);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, 
		("RFCOMM contexts pool destruction failed"));

	status = BTL_POOL_Destroy(&btlRfcommData.packetsPool);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, 
		("RFCOMM packets pool destruction failed"));
		
	btlRfcommInitState = BTL_RFCOMM_INIT_STATE_NOT_INTIALIZED;

	BTL_FUNC_END_AND_UNLOCK();
	
	return (status);
}

 
/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_Create()
 */
BtStatus BTL_RFCOMM_Create(BtlAppHandle *appHandle,
							const BtlRfcommCallBack rfcommCallback,
							const BtSecurityLevel *securityLevel,
							const SdpServicesMask service,
							BtlRfcommContext **rfcommContext)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U32 idx;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_RFCOMM_Create");

	BTL_VERIFY_ERR((0 != rfcommCallback), BT_STATUS_INVALID_PARM, ("Null rfcommCallback"));
	BTL_VERIFY_ERR((0 != rfcommContext), BT_STATUS_INVALID_PARM, ("Null rfcommContext"));

#if BT_SECURITY == XA_ENABLED

	if (0 != securityLevel)
	{
		status = BTL_VerifySecurityLevel(*securityLevel);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Invalid RFCOMM securityLevel"));
	}

#endif	/* BT_SECURITY == XA_ENABLED */

	/* Allocate memory for a unique new RFCOMM context */
	status = BTL_POOL_Allocate(&btlRfcommData.contextsPool, (void **)rfcommContext);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed allocating RFCOMM context"));

	status = BTL_HandleModuleInstanceCreation(appHandle, BTL_MODULE_TYPE_RFCOMM, &(*rfcommContext)->base);
	if (BT_STATUS_SUCCESS != status)
	{
		/* rfcommContext must be freed before we exit */
		BtStatus retVal = BTL_POOL_Free(&btlRfcommData.contextsPool, (void **)rfcommContext);
		BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == retVal), ("Failed freeing RFCOMM context"));

		/* This will handle the exception gracefully */
		BTL_ERR(status, ("Failed handling RFCOMM instance creation"));
	}

	/* Save the given callback */
	(*rfcommContext)->callback = rfcommCallback;

	/* Add the new RFCOMM context to the active contexts list */
	InsertTailList(&btlRfcommData.contextsList, &((*rfcommContext)->base.node));

#if BT_SECURITY == XA_ENABLED

	/* Set the server security record */
	(*rfcommContext)->serverSecurityRecord.id = SEC_RFCOMM_ID;
	if (securityLevel == 0)
		(*rfcommContext)->serverSecurityRecord.level = BSL_RFCOMM_DEFAULT;
	else
		(*rfcommContext)->serverSecurityRecord.level = (*securityLevel);
	/* Security on outgoing connection is not used with the RFCOMM server */
	(*rfcommContext)->serverSecurityRecord.level &= ~(BSL_AUTHENTICATION_OUT | BSL_AUTHORIZATION_OUT | BSL_ENCRYPTION_OUT);
	(*rfcommContext)->serverSecurityRecord.service = service;

#endif	/* BT_SECURITY == XA_ENABLED */

	/* Init RFCOMM channel counters to zero */
	(*rfcommContext)->clientCounter = 0;
	(*rfcommContext)->serverCounter = 0;

	/* Init RFCOMM channels */
	for (idx = 0; idx<BTL_CONFIG_RFCOMM_MAX_NUM_CHANNELS_PER_CONTEXT; idx++)
	{
		(*rfcommContext)->channels[idx].state = BTL_RFCOMM_CHANNEL_STATE_IDLE;

#if BT_SECURITY == XA_ENABLED

		/* Set the client security record */
		(*rfcommContext)->channels[idx].clientSecurityRecord.id = SEC_RFCOMM_ID;
		if (securityLevel == 0)
			(*rfcommContext)->channels[idx].clientSecurityRecord.level = BSL_RFCOMM_DEFAULT;
		else
			(*rfcommContext)->channels[idx].clientSecurityRecord.level = (*securityLevel);
		/* Security on incoming connection is not used with the RFCOMM client */
		(*rfcommContext)->channels[idx].clientSecurityRecord.level &= ~(BSL_AUTHENTICATION_IN | BSL_AUTHORIZATION_IN | BSL_ENCRYPTION_IN);
		(*rfcommContext)->channels[idx].clientSecurityRecord.service = service;

#endif	/* BT_SECURITY == XA_ENABLED */

#if RF_SEND_CONTROL == XA_ENABLED

		(*rfcommContext)->channels[idx].modemStatusUsed = FALSE;
		(*rfcommContext)->channels[idx].portSettingsUsed = FALSE;

#endif /* RF_SEND_CONTROL == XA_ENABLED */

		(*rfcommContext)->channels[idx].channelId = idx;

		(*rfcommContext)->channels[idx].deregisterState = BTL_RFCOMM_DEREGISTER_STATE_MASK_NONE;
	}

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_Destroy()
 */
BtStatus BTL_RFCOMM_Destroy(BtlRfcommContext **rfcommContext)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BOOL isAllocated = FALSE;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_RFCOMM_Destroy");

	BTL_VERIFY_ERR((0 != rfcommContext), BT_STATUS_INVALID_PARM, ("Null rfcommContext"));
	BTL_VERIFY_ERR((0 != *rfcommContext), BT_STATUS_INVALID_PARM, ("Null *rfcommContext"));
	BTL_VERIFY_ERR((0 == (*rfcommContext)->clientCounter), BT_STATUS_IN_USE, ("Client channels are still used"));
	BTL_VERIFY_ERR((0 == (*rfcommContext)->serverCounter), BT_STATUS_IN_USE, ("Server channels are still used"));

	status = BTL_POOL_IsElelementAllocated(&btlRfcommData.contextsPool, *rfcommContext, &isAllocated);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed locating given RFCOMM context"));
	BTL_VERIFY_ERR((TRUE == isAllocated), BT_STATUS_INVALID_PARM, ("Invalid RFCOMM context"));

	/* Remove the context from the list of all RFCOMM contexts */
	RemoveEntryList(&((*rfcommContext)->base.node));

	status = BTL_HandleModuleInstanceDestruction(&(*rfcommContext)->base);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed handling RFCOMM instance destruction"));

	status = BTL_POOL_Free(&btlRfcommData.contextsPool, (void **)rfcommContext);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("Failed freeing RFCOMM context"));

	/* Set the RFCOMM context to NULL */
	*rfcommContext = 0;

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_RegisterServerChannel()
 */
BtStatus BTL_RFCOMM_RegisterServerChannel(BtlRfcommContext *rfcommContext, 
											U16 maxFrameSize,
											U8 credit,
											BtlRfcommChannelId *channelId,
											U8 *serviceId)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U32 idx;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_RFCOMM_RegisterServerChannel");

	BTL_VERIFY_ERR((0 != rfcommContext), BT_STATUS_INVALID_PARM, ("Null rfcommContext"));
	BTL_VERIFY_ERR((0 != channelId), BT_STATUS_INVALID_PARM, ("Null channelId"));
	BTL_VERIFY_ERR((0 != serviceId), BT_STATUS_INVALID_PARM, ("Null serviceId"));

	for (idx = 0; idx<BTL_CONFIG_RFCOMM_MAX_NUM_CHANNELS_PER_CONTEXT; idx++)
	{
		if (rfcommContext->channels[idx].state == BTL_RFCOMM_CHANNEL_STATE_IDLE)
		{
			break;
		}
	}

	BTL_VERIFY_ERR((idx != BTL_CONFIG_RFCOMM_MAX_NUM_CHANNELS_PER_CONTEXT), 
		BT_STATUS_NO_RESOURCES, ("Number of channels would exceed the maximum"));

	/* Set the internal RFCOMM channel fields */
	rfcommContext->channels[idx].channel.callback = BtlRfcommCallback;
	rfcommContext->channels[idx].channel.maxFrameSize = maxFrameSize;
	rfcommContext->channels[idx].channel.userContext = (void*)rfcommContext;

	if (rfcommContext->serverCounter == 0)
	{
		/* For the first server channel, init RFCOMM serviceId to zero */
		rfcommContext->service.serviceId = 0;
	}

	status = RF_RegisterServerChannel(&rfcommContext->channels[idx].channel, &rfcommContext->service, credit);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed registering server channel %d", idx));

#if BT_SECURITY == XA_ENABLED

	/* Register only one security record per service */
	if (rfcommContext->serverCounter == 0)
	{
		/* Set the 'channel' field to serviceId on incoming connections */
		rfcommContext->serverSecurityRecord.channel = rfcommContext->service.serviceId;

		/* Register server security record */
		if (BT_STATUS_SUCCESS != (status = SEC_Register(&rfcommContext->serverSecurityRecord)))
		{		
			/* Deregister registered RFCOMM channel */
			if (RF_DeregisterServerChannel(&rfcommContext->channels[idx].channel, &rfcommContext->service) != BT_STATUS_SUCCESS)
				BTL_LOG_ERROR(("Failed deregistering server channel %d", idx));

			/* This will handle the exception gracefully */
			BTL_ERR(status, ("Failed registering server security record"));
		}
	}
	
#endif	/* BT_SECURITY == XA_ENABLED */

	/* Return channelId & serviceId to app */
	*channelId = idx;
	*serviceId = rfcommContext->service.serviceId;

	rfcommContext->channels[idx].state = BTL_RFCOMM_CHANNEL_STATE_SERVER_DISCONNECTED;

	rfcommContext->serverCounter++;

	BTL_LOG_INFO(("Server channel %d is registered to serviceId %d.", idx, rfcommContext->service.serviceId));

	BTL_FUNC_END_AND_UNLOCK();
	
	return (status); 
}


/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_DeregisterServerChannel()
 */
BtStatus BTL_RFCOMM_DeregisterServerChannel(BtlRfcommContext *rfcommContext, 
											BtlRfcommChannelId channelId)
{
	BtStatus status = BT_STATUS_PENDING;
	BtlRfcommChannel *btlChannel;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_RFCOMM_DeregisterServerChannel");

	BTL_VERIFY_ERR((0 != rfcommContext), BT_STATUS_INVALID_PARM, ("Null rfcommContext"));
	BTL_VERIFY_ERR((channelId < BTL_CONFIG_RFCOMM_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid channelId"));

	btlChannel = &(rfcommContext->channels[channelId]);

	switch (btlChannel->state)
	{
		case (BTL_RFCOMM_CHANNEL_STATE_SERVER_DISCONNECTED):
		{
			/* Server channel is disconnected, so it can be deregistered */
			status = BtlDeregisterServerChannel(rfcommContext, channelId);
			break;
		}
		case (BTL_RFCOMM_CHANNEL_STATE_SERVER_DISCONNECTING):
		case (BTL_RFCOMM_CHANNEL_STATE_SERVER_DISCONNECT_IND):
		{
			/* Server channel state is now in the process of deregistering */
			btlChannel->state = BTL_RFCOMM_CHANNEL_STATE_SERVER_DEREGISTERING;
			break;
		}
		case (BTL_RFCOMM_CHANNEL_STATE_SERVER_CONNECTED):
		{
			status = RF_CloseChannel(&btlChannel->channel);
			BTL_VERIFY_ERR((status == BT_STATUS_PENDING), status, ("Failed closing server channel %d", channelId));

			if (BT_STATUS_PENDING == status)
				btlChannel->state = BTL_RFCOMM_CHANNEL_STATE_SERVER_DEREGISTERING;
			break;
		}
		case (BTL_RFCOMM_CHANNEL_STATE_SERVER_CONNECT_IND):
		{
			/* Reject incoming open request, and wait for close event */
			
			status = RF_RejectChannel(&btlChannel->channel);
			BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == status), ("Failed responding to open indication while deregistering."));

			if (BT_STATUS_PENDING == status)
				btlChannel->state = BTL_RFCOMM_CHANNEL_STATE_SERVER_DEREGISTERING;
			
			break;
		}
		case (BTL_RFCOMM_CHANNEL_STATE_SERVER_DEREGISTERING):
		{
			/* Server is already deregistering */
			break;
		}
		default:
		{
			BTL_ERR(BT_STATUS_FAILED, ("Failed deregistering server channel %d, invalid state %d", channelId, btlChannel->state));
			break;
		}
	}

	if (btlChannel->state == BTL_RFCOMM_CHANNEL_STATE_SERVER_DEREGISTERING)
		btlChannel->deregisterState |= BTL_RFCOMM_DEREGISTER_STATE_MASK_DEREGISTER_ACTIVE;		
	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_ConnectResponse()
 */
BtStatus BTL_RFCOMM_ConnectResponse(BtlRfcommContext *rfcommContext, 
									BtlRfcommChannelId channelId,
									BOOL acceptCon)
{
	BtStatus status = BT_STATUS_PENDING;
	BtlRfcommChannel *btlChannel;
	
	BTL_FUNC_START_AND_LOCK("BTL_RFCOMM_ConnectResponse");

	BTL_VERIFY_ERR((0 != rfcommContext), BT_STATUS_INVALID_PARM, ("Null rfcommContext"));
	BTL_VERIFY_ERR((channelId < BTL_CONFIG_RFCOMM_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid channelId"));

	btlChannel = &(rfcommContext->channels[channelId]);
	
	BTL_VERIFY_ERR((BTL_RFCOMM_CHANNEL_STATE_SERVER_CONNECT_IND == btlChannel->state), BT_STATUS_FAILED, ("Server channel %d is not connect indicated", channelId));

	if (acceptCon == TRUE)
		status = RF_AcceptChannel(&btlChannel->channel);
	else
		status = RF_RejectChannel(&btlChannel->channel);
	
	BTL_VERIFY_ERR((BT_STATUS_PENDING == status), status, ("Failed responding to connection indication %d", channelId));
	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_ConnectClientChannel()
 */
BtStatus BTL_RFCOMM_ConnectClientChannel(BtlRfcommContext *rfcommContext,
										BD_ADDR *bdAddr,
										U8 serviceId,
										U16 maxFrameSize,
										U8 priority,
										U8 credit, 
										BtlRfcommChannelId *channelId)
{
	BtStatus status = BT_STATUS_PENDING;
	BtStatus retVal;
	U32 idx;
	BtlRfcommChannel *btlChannel;
	
	BTL_FUNC_START_AND_LOCK("BTL_RFCOMM_ConnectClientChannel");

	BTL_VERIFY_ERR((0 != rfcommContext), BT_STATUS_INVALID_PARM, ("Null rfcommContext"));
	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));
	BTL_VERIFY_ERR((0 != channelId), BT_STATUS_INVALID_PARM, ("Null channelId"));

	for (idx = 0; idx<BTL_CONFIG_RFCOMM_MAX_NUM_CHANNELS_PER_CONTEXT; idx++)
	{
		if (rfcommContext->channels[idx].state == BTL_RFCOMM_CHANNEL_STATE_IDLE)
		{
			break;
		}
	}

	BTL_VERIFY_ERR((idx != BTL_CONFIG_RFCOMM_MAX_NUM_CHANNELS_PER_CONTEXT), 
		BT_STATUS_NO_RESOURCES, ("Number of channels would exceed the maximum"));

	btlChannel = &(rfcommContext->channels[idx]);

	/* Set the internal RFCOMM channel fields */
	btlChannel->channel.callback = BtlRfcommCallback;
	btlChannel->channel.maxFrameSize = maxFrameSize;
	btlChannel->channel.priority = priority;
	btlChannel->channel.userContext = (void*)rfcommContext;
	btlChannel->savedServiceId = serviceId;
	btlChannel->savedCredit = credit;

	/* Register CMGR handler */
	status = CMGR_RegisterHandler(&btlChannel->cmgrHandler, BtlRfcommCmgrCallback);
	BTL_VERIFY_ERR((status == BT_STATUS_SUCCESS), status, ("Failed registering CMGR handler, status = ", status));

	/* Create ACL link */
	status = CMGR_CreateDataLink(&btlChannel->cmgrHandler, bdAddr);
	if (status == BT_STATUS_SUCCESS)
	{
		/* A connection already exists and the handler is now associated with that link, 
		now we can open RFCOMM channel */
		status = BtlOpenClientChannel(rfcommContext, idx);
		if (status != BT_STATUS_PENDING)
		{
			/* Remove ACL link, if exists */
			if (CMGR_IsLinkUp(&btlChannel->cmgrHandler) == TRUE)
			{
				retVal = CMGR_RemoveDataLink(&btlChannel->cmgrHandler);
				if (retVal != BT_STATUS_SUCCESS)
		        	BTL_LOG_ERROR(("Failed removing data link, status = ", retVal));
			}
			
		    /* Release CMGR handler */
			retVal = CMGR_DeregisterHandler(&btlChannel->cmgrHandler);
			if (retVal != BT_STATUS_SUCCESS)
	        	BTL_LOG_ERROR(("Failed deregistering CMGR handler, status = ", retVal));
		}
	}
	else if (status != BT_STATUS_PENDING)
	{
		/* Release CMGR handler */
		(void)CMGR_DeregisterHandler(&btlChannel->cmgrHandler);
		BTL_LOG_ERROR(("Failed creating outgoing ACL link, status = %d", status));
	}

	if (status == BT_STATUS_PENDING)
	{
		/* Return channelId to app */
		*channelId = idx;
		
		/* Client is now in the process of connecting */
		btlChannel->state = BTL_RFCOMM_CHANNEL_STATE_CLIENT_CONNECTING;
		rfcommContext->clientCounter++;
	}
	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_Disconnect()
 */
BtStatus BTL_RFCOMM_Disconnect(BtlRfcommContext *rfcommContext, 
								BtlRfcommChannelId channelId)
{
	BtStatus status = BT_STATUS_PENDING;
	BtlRfcommChannel *btlChannel;
	
	BTL_FUNC_START_AND_LOCK("BTL_RFCOMM_Disconnect");

	BTL_VERIFY_ERR((0 != rfcommContext), BT_STATUS_INVALID_PARM, ("Null rfcommContext"));
	BTL_VERIFY_ERR((channelId < BTL_CONFIG_RFCOMM_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid channelId"));

	btlChannel = &(rfcommContext->channels[channelId]);

	BTL_VERIFY_ERR(((BTL_RFCOMM_CHANNEL_STATE_SERVER_CONNECTED == btlChannel->state) || 
		(BTL_RFCOMM_CHANNEL_STATE_CLIENT_CONNECTED == btlChannel->state)), 
		BT_STATUS_FAILED, ("Channel %d is not connected", channelId));	

	status = RF_CloseChannel(&btlChannel->channel);
	BTL_VERIFY_ERR((BT_STATUS_PENDING == status), status, ("Failed closing channel %d", channelId));

	/* Channel state is now in the process of disconnecting */
	if (btlChannel->state & BTL_RFCOMM_CHANNEL_STATE_SERVER_MASK)
	{
		btlChannel->state = BTL_RFCOMM_CHANNEL_STATE_SERVER_DISCONNECTING;
	}
	else
	{
		btlChannel->state = BTL_RFCOMM_CHANNEL_STATE_CLIENT_DISCONNECTING;
	}
	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_GetConnectedDevice()
 */
BtStatus BTL_RFCOMM_GetConnectedDevice(BtlRfcommContext *rfcommContext, 
										BtlRfcommChannelId channelId, 
										BD_ADDR *bdAddr)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtlRfcommChannel *btlChannel;
	BtRemoteDevice *remDev;
	
	BTL_FUNC_START_AND_LOCK("BTL_RFCOMM_GetConnectedDevice");

	BTL_VERIFY_ERR((0 != rfcommContext), BT_STATUS_INVALID_PARM, ("Null rfcommContext"));
	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));
	BTL_VERIFY_ERR((channelId < BTL_CONFIG_RFCOMM_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid channelId"));

	btlChannel = &(rfcommContext->channels[channelId]);

	BTL_VERIFY_ERR(((BTL_RFCOMM_CHANNEL_STATE_SERVER_CONNECTED == btlChannel->state) || 
		(BTL_RFCOMM_CHANNEL_STATE_CLIENT_CONNECTED == btlChannel->state)), 
		BT_STATUS_NO_CONNECTION, ("Channel %d is not connected", channelId));	

	remDev = RF_RemoteDevice(&(btlChannel->channel));

	BTL_VERIFY_FATAL((0 != remDev), BT_STATUS_NO_CONNECTION, ("Unable to find connected remote device"));

	OS_MemCopy((U8 *)bdAddr, (const U8 *)(&(remDev->bdAddr)), sizeof(BD_ADDR));

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_SendData()
 */
BtStatus BTL_RFCOMM_SendData(BtlRfcommContext *rfcommContext,
							BtlRfcommChannelId channelId,
							U8 *data,
							U16 dataLen,
							BtPacket **packet)
{
	BtStatus status = BT_STATUS_PENDING;
	BtStatus retVal;
	BtlRfcommChannel *btlChannel;
	
	BTL_FUNC_START_AND_LOCK("BTL_RFCOMM_SendData");

	BTL_VERIFY_ERR((0 != rfcommContext), BT_STATUS_INVALID_PARM, ("Null rfcommContext"));
	BTL_VERIFY_ERR((channelId < BTL_CONFIG_RFCOMM_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid channelId"));
	BTL_VERIFY_ERR((0 != data), BT_STATUS_INVALID_PARM, ("Null data"));
	BTL_VERIFY_ERR((0 != packet), BT_STATUS_INVALID_PARM, ("Null packet"));

	btlChannel = &(rfcommContext->channels[channelId]);

	BTL_VERIFY_ERR(((BTL_RFCOMM_CHANNEL_STATE_SERVER_CONNECTED == btlChannel->state) || 
		(BTL_RFCOMM_CHANNEL_STATE_CLIENT_CONNECTED == btlChannel->state)), 
		BT_STATUS_NO_CONNECTION, ("Channel %d is not connected", channelId));	

	/* Allocate memory for a unique new RFCOMM TX packet */
	status = BTL_POOL_Allocate(&btlRfcommData.packetsPool, (void **)packet);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed allocating TX packet"));

	/* Set up the TX packet */
	(*packet)->data = data;
	(*packet)->dataLen = dataLen;
	(*packet)->flags = BTP_FLAG_NONE;

	status = RF_SendData(&btlChannel->channel, (*packet));
	if (status != BT_STATUS_PENDING)
	{
		/* Sending failed, free the packet */
		retVal = BTL_POOL_Free(&btlRfcommData.packetsPool, (void **)packet);
		BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == retVal), BT_STATUS_INTERNAL_ERROR, ("Failed freeing TX packet"));
	}	

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


#if RF_SEND_CONTROL == XA_ENABLED

/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_SetModemStatus()
 */
BtStatus BTL_RFCOMM_SetModemStatus(BtlRfcommContext *rfcommContext,
									BtlRfcommChannelId channelId,
									RfModemStatus *modemStatus)
{
	BtStatus status = BT_STATUS_PENDING;
	BtlRfcommChannel *btlChannel;
	
	BTL_FUNC_START_AND_LOCK("BTL_RFCOMM_SetModemStatus");

	BTL_VERIFY_ERR((0 != rfcommContext), BT_STATUS_INVALID_PARM, ("Null rfcommContext"));
	BTL_VERIFY_ERR((0 != modemStatus), BT_STATUS_INVALID_PARM, ("Null modemStatus"));
	BTL_VERIFY_ERR((channelId < BTL_CONFIG_RFCOMM_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid channelId"));

	btlChannel = &(rfcommContext->channels[channelId]);

	BTL_VERIFY_ERR(((BTL_RFCOMM_CHANNEL_STATE_SERVER_CONNECTED == btlChannel->state) || 
		(BTL_RFCOMM_CHANNEL_STATE_CLIENT_CONNECTED == btlChannel->state)), 
		BT_STATUS_FAILED, ("Channel %d is not connected", channelId));

	BTL_VERIFY_ERR((btlChannel->modemStatusUsed == FALSE), BT_STATUS_IN_PROGRESS, ("modemStatus is in use"));

	/* Setup local copy of modemStatus, which will be owned by RFCOMM */
	btlChannel->modemStatus = *modemStatus;
	
	status = RF_SetModemStatus(&btlChannel->channel, &btlChannel->modemStatus);
	BTL_VERIFY_ERR((status == BT_STATUS_PENDING), status, ("Failed setting modem status"));
	
	btlChannel->modemStatusUsed = TRUE;

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_SetLineStatus()
 */
BtStatus BTL_RFCOMM_SetLineStatus(BtlRfcommContext *rfcommContext,
									BtlRfcommChannelId channelId,
									RfLineStatus lineStatus)
{
	BtStatus status = BT_STATUS_PENDING;
	BtlRfcommChannel *btlChannel;
	
	BTL_FUNC_START_AND_LOCK("BTL_RFCOMM_SetLineStatus");

	BTL_VERIFY_ERR((0 != rfcommContext), BT_STATUS_INVALID_PARM, ("Null rfcommContext"));
	BTL_VERIFY_ERR((channelId < BTL_CONFIG_RFCOMM_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid channelId"));

	btlChannel = &(rfcommContext->channels[channelId]);

	BTL_VERIFY_ERR(((BTL_RFCOMM_CHANNEL_STATE_SERVER_CONNECTED == btlChannel->state) || 
		(BTL_RFCOMM_CHANNEL_STATE_CLIENT_CONNECTED == btlChannel->state)), 
		BT_STATUS_FAILED, ("Channel %d is not connected", channelId));
	
	status = RF_SetLineStatus(&btlChannel->channel, lineStatus);

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_RequestPortSettings()
 */
BtStatus BTL_RFCOMM_RequestPortSettings(BtlRfcommContext *rfcommContext,
										BtlRfcommChannelId channelId,
										RfPortSettings *portSettings)
{
	BtStatus status = BT_STATUS_PENDING;
	BtlRfcommChannel *btlChannel;
	
	BTL_FUNC_START_AND_LOCK("BTL_RFCOMM_RequestPortSettings");

	BTL_VERIFY_ERR((0 != rfcommContext), BT_STATUS_INVALID_PARM, ("Null rfcommContext"));
	BTL_VERIFY_ERR((0 != portSettings), BT_STATUS_INVALID_PARM, ("Null portSettings"));
	BTL_VERIFY_ERR((channelId < BTL_CONFIG_RFCOMM_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid channelId"));

	btlChannel = &(rfcommContext->channels[channelId]);

	BTL_VERIFY_ERR(((BTL_RFCOMM_CHANNEL_STATE_SERVER_CONNECTED == btlChannel->state) || 
		(BTL_RFCOMM_CHANNEL_STATE_CLIENT_CONNECTED == btlChannel->state)), 
		BT_STATUS_FAILED, ("Channel %d is not connected", channelId));

	BTL_VERIFY_ERR((btlChannel->portSettingsUsed == FALSE), BT_STATUS_IN_PROGRESS, ("portSettings is in use"));

	/* Setup local copy of portSettings, which will be owned by RFCOMM */
	btlChannel->portSettings = *portSettings;
	
	status = RF_RequestPortSettings(&btlChannel->channel, &btlChannel->portSettings);
	BTL_VERIFY_ERR((status == BT_STATUS_PENDING), status, ("Failed requesting port settings"));
	
	btlChannel->portSettingsUsed = TRUE;

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_RequestPortStatus()
 */
BtStatus BTL_RFCOMM_RequestPortStatus(BtlRfcommContext *rfcommContext,
										BtlRfcommChannelId channelId)
{
	BtStatus status = BT_STATUS_PENDING;
	BtlRfcommChannel *btlChannel;
	
	BTL_FUNC_START_AND_LOCK("BTL_RFCOMM_RequestPortStatus");

	BTL_VERIFY_ERR((0 != rfcommContext), BT_STATUS_INVALID_PARM, ("Null rfcommContext"));
	BTL_VERIFY_ERR((channelId < BTL_CONFIG_RFCOMM_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid channelId"));

	btlChannel = &(rfcommContext->channels[channelId]);

	BTL_VERIFY_ERR(((BTL_RFCOMM_CHANNEL_STATE_SERVER_CONNECTED == btlChannel->state) || 
		(BTL_RFCOMM_CHANNEL_STATE_CLIENT_CONNECTED == btlChannel->state)), 
		BT_STATUS_FAILED, ("Channel %d is not connected", channelId));
	
	status = RF_RequestPortStatus(&btlChannel->channel);

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_AcceptPortSettings()
 */
BtStatus BTL_RFCOMM_AcceptPortSettings(BtlRfcommContext *rfcommContext,
										BtlRfcommChannelId channelId,
										RfPortSettings *portSettings)
{
	BtStatus status = BT_STATUS_PENDING;
	BtlRfcommChannel *btlChannel;
	
	BTL_FUNC_START_AND_LOCK("BTL_RFCOMM_AcceptPortSettings");

	BTL_VERIFY_ERR((0 != rfcommContext), BT_STATUS_INVALID_PARM, ("Null rfcommContext"));
	BTL_VERIFY_ERR((0 != portSettings), BT_STATUS_INVALID_PARM, ("Null portSettings"));
	BTL_VERIFY_ERR((channelId < BTL_CONFIG_RFCOMM_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid channelId"));

	btlChannel = &(rfcommContext->channels[channelId]);

	BTL_VERIFY_ERR(((BTL_RFCOMM_CHANNEL_STATE_SERVER_CONNECTED == btlChannel->state) || 
		(BTL_RFCOMM_CHANNEL_STATE_CLIENT_CONNECTED == btlChannel->state)), 
		BT_STATUS_FAILED, ("Channel %d is not connected", channelId));
	
	status = RF_AcceptPortSettings(&btlChannel->channel, portSettings);

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_SendPortStatus()
 */
BtStatus BTL_RFCOMM_SendPortStatus(BtlRfcommContext *rfcommContext,
									BtlRfcommChannelId channelId,
									RfPortSettings *portSettings)
{
	BtStatus status = BT_STATUS_PENDING;
	BtlRfcommChannel *btlChannel;
	
	BTL_FUNC_START_AND_LOCK("BTL_RFCOMM_SendPortStatus");

	BTL_VERIFY_ERR((0 != rfcommContext), BT_STATUS_INVALID_PARM, ("Null rfcommContext"));
	BTL_VERIFY_ERR((0 != portSettings), BT_STATUS_INVALID_PARM, ("Null portSettings"));
	BTL_VERIFY_ERR((channelId < BTL_CONFIG_RFCOMM_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid channelId"));

	btlChannel = &(rfcommContext->channels[channelId]);

	BTL_VERIFY_ERR(((BTL_RFCOMM_CHANNEL_STATE_SERVER_CONNECTED == btlChannel->state) || 
		(BTL_RFCOMM_CHANNEL_STATE_CLIENT_CONNECTED == btlChannel->state)), 
		BT_STATUS_FAILED, ("Channel %d is not connected", channelId));
	
	status = RF_SendPortStatus(&btlChannel->channel, portSettings);

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_DelayPortRsp()
 */
BtStatus BTL_RFCOMM_DelayPortRsp(BtlRfcommContext *rfcommContext,
									BtlRfcommChannelId channelId)
{
	BtStatus status = BT_STATUS_PENDING;
	BtlRfcommChannel *btlChannel;
	
	BTL_FUNC_START_AND_LOCK("BTL_RFCOMM_DelayPortRsp");

	BTL_VERIFY_ERR((0 != rfcommContext), BT_STATUS_INVALID_PARM, ("Null rfcommContext"));
	BTL_VERIFY_ERR((channelId < BTL_CONFIG_RFCOMM_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid channelId"));

	btlChannel = &(rfcommContext->channels[channelId]);

	BTL_VERIFY_ERR(((BTL_RFCOMM_CHANNEL_STATE_SERVER_CONNECTED == btlChannel->state) || 
		(BTL_RFCOMM_CHANNEL_STATE_CLIENT_CONNECTED == btlChannel->state)), 
		BT_STATUS_FAILED, ("Channel %d is not connected", channelId));
	
	status = RF_DelayPortRsp(&btlChannel->channel);

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

#endif /* RF_SEND_CONTROL == XA_ENABLED */


#if RF_SEND_TEST == XA_ENABLED

/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_SendTest()
 */
BtStatus BTL_RFCOMM_SendTest(BtlRfcommContext *rfcommContext,
							BtlRfcommChannelId channelId)
{
	BtStatus status = BT_STATUS_PENDING;
	BtlRfcommChannel *btlChannel;
	
	BTL_FUNC_START_AND_LOCK("BTL_RFCOMM_SendTest");

	BTL_VERIFY_ERR((0 != rfcommContext), BT_STATUS_INVALID_PARM, ("Null rfcommContext"));
	BTL_VERIFY_ERR((channelId < BTL_CONFIG_RFCOMM_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid channelId"));

	btlChannel = &(rfcommContext->channels[channelId]);

	BTL_VERIFY_ERR(((BTL_RFCOMM_CHANNEL_STATE_SERVER_CONNECTED == btlChannel->state) || 
		(BTL_RFCOMM_CHANNEL_STATE_CLIENT_CONNECTED == btlChannel->state)), 
		BT_STATUS_FAILED, ("Channel %d is not connected", channelId));
	
	status = RF_SendTest(&btlChannel->channel);

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

#endif /* RF_SEND_TEST == XA_ENABLED */


/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_GetFrameSize()
 */
BtStatus BTL_RFCOMM_GetFrameSize(BtlRfcommContext *rfcommContext,
								BtlRfcommChannelId channelId,
								U16 *frameSize)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtlRfcommChannel *btlChannel;
	
	BTL_FUNC_START_AND_LOCK("BTL_RFCOMM_GetFrameSize");

	BTL_VERIFY_ERR((0 != rfcommContext), BT_STATUS_INVALID_PARM, ("Null rfcommContext"));
	BTL_VERIFY_ERR((channelId < BTL_CONFIG_RFCOMM_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid channelId"));
	BTL_VERIFY_ERR((0 != frameSize), BT_STATUS_INVALID_PARM, ("Null frameSize"));

	btlChannel = &(rfcommContext->channels[channelId]);

	BTL_VERIFY_ERR(((BTL_RFCOMM_CHANNEL_STATE_SERVER_CONNECTED == btlChannel->state) || 
		(BTL_RFCOMM_CHANNEL_STATE_CLIENT_CONNECTED == btlChannel->state)), 
		BT_STATUS_NO_CONNECTION, ("Channel %d is not connected", channelId));
	
	*frameSize = RF_FrameSize(&btlChannel->channel);

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_AdvanceCredit()
 */
BtStatus BTL_RFCOMM_AdvanceCredit(BtlRfcommContext *rfcommContext,
								BtlRfcommChannelId channelId,
								U8 credit)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtlRfcommChannel *btlChannel;
	
	BTL_FUNC_START_AND_LOCK("BTL_RFCOMM_AdvanceCredit");

	BTL_VERIFY_ERR((0 != rfcommContext), BT_STATUS_INVALID_PARM, ("Null rfcommContext"));
	BTL_VERIFY_ERR((channelId < BTL_CONFIG_RFCOMM_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid channelId"));

	btlChannel = &(rfcommContext->channels[channelId]);

	BTL_VERIFY_ERR(((BTL_RFCOMM_CHANNEL_STATE_SERVER_CONNECTED == btlChannel->state) || 
		(BTL_RFCOMM_CHANNEL_STATE_CLIENT_CONNECTED == btlChannel->state)), 
		BT_STATUS_NO_CONNECTION, ("Channel %d is not connected", channelId));
	
	status = RF_AdvanceCredit(&btlChannel->channel, credit);

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_IsCreditFlowEnabled()
 */
BtStatus BTL_RFCOMM_IsCreditFlowEnabled(BtlRfcommContext *rfcommContext,
								  		BtlRfcommChannelId channelId,
								  		BOOL *flowEnabled)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtlRfcommChannel *btlChannel;
	
	BTL_FUNC_START_AND_LOCK("BTL_RFCOMM_IsCreditFlowEnabled");

	BTL_VERIFY_ERR((0 != rfcommContext), BT_STATUS_INVALID_PARM, ("Null rfcommContext"));
	BTL_VERIFY_ERR((channelId < BTL_CONFIG_RFCOMM_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_INVALID_PARM, ("Invalid channelId"));
	BTL_VERIFY_ERR((0 != flowEnabled), BT_STATUS_INVALID_PARM, ("Null flowEnabled"));

	btlChannel = &(rfcommContext->channels[channelId]);

	BTL_VERIFY_ERR(((BTL_RFCOMM_CHANNEL_STATE_SERVER_CONNECTED == btlChannel->state) || 
		(BTL_RFCOMM_CHANNEL_STATE_CLIENT_CONNECTED == btlChannel->state)), 
		BT_STATUS_NO_CONNECTION, ("Channel %d is not connected", channelId));
	
	*flowEnabled = RF_CreditFlowEnabled(&btlChannel->channel);

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


#if BT_SECURITY == XA_ENABLED

/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_SetSecurityLevel()
 */
BtStatus BTL_RFCOMM_SetSecurityLevel(BtlRfcommContext *rfcommContext,
								  	const BtSecurityLevel *securityLevel)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U32 idx;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_RFCOMM_SetSecurityLevel");

	BTL_VERIFY_ERR((0 != rfcommContext), BT_STATUS_INVALID_PARM, ("Null rfcommContext"));

	if (0 != securityLevel)
	{
		status = BTL_VerifySecurityLevel(*securityLevel);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Invalid RFCOMM securityLevel"));
	}

	/* Set the server security level */
	if (securityLevel == 0)
		rfcommContext->serverSecurityRecord.level = BSL_RFCOMM_DEFAULT;
	else
		rfcommContext->serverSecurityRecord.level = (*securityLevel);
	/* Security on outgoing connection is not used with the RFCOMM server */
	rfcommContext->serverSecurityRecord.level &= ~(BSL_AUTHENTICATION_OUT | BSL_AUTHORIZATION_OUT | BSL_ENCRYPTION_OUT);

	/* Set the client security levels */
	for (idx = 0; idx<BTL_CONFIG_RFCOMM_MAX_NUM_CHANNELS_PER_CONTEXT; idx++)
	{
		if (securityLevel == 0)
			rfcommContext->channels[idx].clientSecurityRecord.level = BSL_RFCOMM_DEFAULT;
		else
			rfcommContext->channels[idx].clientSecurityRecord.level = (*securityLevel);

		/* Security on incoming connection is not used with the RFCOMM client */
		rfcommContext->channels[idx].clientSecurityRecord.level &= ~(BSL_AUTHENTICATION_IN | BSL_AUTHORIZATION_IN | BSL_ENCRYPTION_IN);
	}

	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_GetSecurityLevel()
 */
BtStatus BTL_RFCOMM_GetSecurityLevel(BtlRfcommContext *rfcommContext,
								  	BtSecurityLevel *securityLevel)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_RFCOMM_GetSecurityLevel");

	BTL_VERIFY_ERR((0 != rfcommContext), BT_STATUS_INVALID_PARM, ("Null rfcommContext"));
	BTL_VERIFY_ERR((0 != securityLevel), BT_STATUS_INVALID_PARM, ("Null securityLevel"));

	*securityLevel = (BtSecurityLevel)(rfcommContext->serverSecurityRecord.level | rfcommContext->channels[0].clientSecurityRecord.level);
	
	BTL_FUNC_END_AND_UNLOCK();

	return (status);
}

#endif	/* BT_SECURITY == XA_ENABLED */


/*-------------------------------------------------------------------------------
 * BtlOpenClientChannel()
 *
 *		Internal function for opening client channel, when ACL link already exists.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *		rfcommContext [in] - RFCOMM context.
 *
 *		channelId [in] - BTL RFCOMM channelId.
 *
 * Returns:
 *		BtStatus.
 */
static BtStatus BtlOpenClientChannel(BtlRfcommContext *rfcommContext, BtlRfcommChannelId channelId)
{
	BtStatus status = BT_STATUS_PENDING;
	BtStatus retVal;
	BtlRfcommChannel *btlChannel;
	BtRemoteDevice *remDev;

	BTL_FUNC_START("BtlOpenClientChannel");

	btlChannel = &(rfcommContext->channels[channelId]);

	remDev = CMGR_GetRemoteDevice(&(btlChannel->cmgrHandler));
	BTL_VERIFY_ERR((0 != remDev), BT_STATUS_NO_CONNECTION, ("ACL link is not connected!"));

#if BT_SECURITY == XA_ENABLED

	/* Register security record before opening RFCOMM channel */

	/* The 'channel' field is set to the address of the RfChannel structure on outgoing connections */
	btlChannel->clientSecurityRecord.channel = (U32)(&(btlChannel->channel));

	/* Register client security record */
	status = SEC_Register(&btlChannel->clientSecurityRecord);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("Failed registering client security record"));
	
#endif	/* BT_SECURITY == XA_ENABLED */

	status = RF_OpenClientChannel(remDev, btlChannel->savedServiceId, &btlChannel->channel, btlChannel->savedCredit);

#if BT_SECURITY == XA_ENABLED
	if (status != BT_STATUS_PENDING)
	{
		/* Failed in opening RFCOMM channel, need to Deregister the security record */
		retVal = SEC_Unregister(&btlChannel->clientSecurityRecord);
		if (retVal != BT_STATUS_SUCCESS)
			BTL_LOG_ERROR(("Failed deregistering client security record!, status = %d", retVal));	
	}
#endif	/* BT_SECURITY == XA_ENABLED */
	
	BTL_FUNC_END();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BtlDeregisterServerChannel()
 *
 *		Internal function for deregistering server channel.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		rfcommContext [in] - RFCOMM context.
 *
 *		channelId [in] - BTL RFCOMM channelId.
 *
 * Returns:
 *		BtStatus.
 */
static BtStatus BtlDeregisterServerChannel(BtlRfcommContext *rfcommContext, BtlRfcommChannelId channelId)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtlRfcommChannel *btlChannel;

	BTL_FUNC_START("BtlDeregisterServerChannel");

	BTL_VERIFY_ERR((rfcommContext->serverCounter > 0), BT_STATUS_INTERNAL_ERROR, ("wrong serverCounter!"));

	btlChannel = &(rfcommContext->channels[channelId]);

	status = RF_DeregisterServerChannel(&btlChannel->channel, &rfcommContext->service);
	BTL_VERIFY_ERR((status == BT_STATUS_SUCCESS), status, ("Failed deregistering server channel"));

	btlChannel->state = BTL_RFCOMM_CHANNEL_STATE_IDLE;

	rfcommContext->serverCounter--;

	BTL_LOG_INFO(("Server channel %d is deregistered from serviceId %d.", channelId, rfcommContext->service.serviceId));

	/* If this is the last server channel, deregister service and security record */
	if (rfcommContext->serverCounter == 0)
	{
		status = RF_DeregisterService(&rfcommContext->service);
		if (BT_STATUS_SUCCESS == status)
			BTL_LOG_INFO(("Service is deregistered, serviceId %d.", rfcommContext->service.serviceId));
		else
			BTL_LOG_ERROR(("Failed deregistering service!, status = %d", status));

#if BT_SECURITY == XA_ENABLED

		/* Deregister the security record only if it's the last server channel */
		status = SEC_Unregister(&rfcommContext->serverSecurityRecord);
		if (BT_STATUS_SUCCESS != status)
			BTL_LOG_ERROR(("Failed deregistering server security record!, status = %d", status));
	
#endif	/* BT_SECURITY == XA_ENABLED */

	}

	BTL_FUNC_END();

	return (status);
}


/*-------------------------------------------------------------------------------
 * BtlCmgrCallback()
 *
 *		Internal callback for handling CMGR events in BTL layer.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		Handler [in] - Internal CMGR handler.
 *
 *		Event [in] - Internal CMGR event.
 *
 *		Status [in] - indicate status.
 *
 * Returns:
 *		void.
 */
static void BtlRfcommCmgrCallback(CmgrHandler *Handler, CmgrEvent Event, BtStatus Status)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BOOL cleanUpAndReport = FALSE;
	char addr[BDADDR_NTOA_SIZE];
	BtlRfcommContext *rfcommContext;
	BtlRfcommChannel *btlChannel;
	U32 idx;
	RfCallbackParms info;

	BTL_FUNC_START("BtlRfcommCmgrCallback");

	/* Find BTL RFCOMM channel according to given CMGR handler */
	btlChannel = ContainingRecord(Handler, BtlRfcommChannel, cmgrHandler);

	/* Find context according to RFCOMM channel */
	rfcommContext = (BtlRfcommContext*)(btlChannel->channel.userContext);

	/* Find channel index in the context */
	for (idx = 0; idx<BTL_CONFIG_RFCOMM_MAX_NUM_CHANNELS_PER_CONTEXT; idx++)
	{
		if ((&(rfcommContext->channels[idx])) == btlChannel)
			break;
	}

	BTL_VERIFY_ERR((idx != BTL_CONFIG_RFCOMM_MAX_NUM_CHANNELS_PER_CONTEXT), BT_STATUS_FAILED, ("No channel found for received CMGR event"));

	switch (Event) 
	{
    	case (CMEVENT_DATA_LINK_CON_CNF):
		{
			/* An outgoing ACL connection request has been completed. */
			BTL_LOG_INFO(("Received outgoing ACL link connect confirmation to %s, status = %d.", 
							bdaddr_ntoa(&(Handler->bdc->addr), addr), Status));

			/* If ACL link was created, open RFCOMM client channel */
	        if ((Status != BT_STATUS_SUCCESS) || 
				(BT_STATUS_PENDING != BtlOpenClientChannel(rfcommContext, idx)))
				cleanUpAndReport = TRUE;

	        break;
    	}
		case (CMEVENT_DATA_LINK_CON_IND):
		{
	        /* A remote device has established an ACL connection with this device. */
			BTL_LOG_INFO(("Received incoming ACL link connect indication from %s.", bdaddr_ntoa(&(Handler->remDev->bdAddr), addr)));
	        break;
    	}
		case (CMEVENT_DATA_LINK_DIS):
		{
	        /* The ACL link has been disconnected. */
			BTL_LOG_INFO(("ACL link has been disconnected from %s, status = %d, reason = %d.", 
						bdaddr_ntoa(&(Handler->remDev->bdAddr), addr), Status, Handler->errCode));	
	        break;
    	}
    }

	if (TRUE == cleanUpAndReport)
	{
		/* Remove ACL link, if exists */
		if (CMGR_IsLinkUp(&btlChannel->cmgrHandler) == TRUE)
		{
			status = CMGR_RemoveDataLink(&btlChannel->cmgrHandler);
			if (status != BT_STATUS_SUCCESS)
	        	BTL_LOG_ERROR(("Failed removing data link, status = ", status));
		}
		
	    /* Release CMGR handler */
		status = CMGR_DeregisterHandler(&btlChannel->cmgrHandler);
		if (status != BT_STATUS_SUCCESS)
        	BTL_LOG_ERROR(("Failed deregistering CMGR handler, status = ", status));

		btlChannel->state = BTL_RFCOMM_CHANNEL_STATE_IDLE;
		rfcommContext->clientCounter--;
				
		/* Indicate the failure to the application */
	    info.event = RFEVENT_CLOSED;
	    info.status = Status;
		info.ptrs.remDev = 0;
				
		/* Set the context in the event passed to app */
		btlRfcommData.event.rfcommContext = rfcommContext;

		/* Set the channel index in the event passed to app */
		btlRfcommData.event.channelId = idx;

		/* Set the internal event in the event passed to app */
		btlRfcommData.event.rfcommEvent = &info;

		/* Pass the event to app */
		rfcommContext->callback(&btlRfcommData.event);
	}

	BTL_FUNC_END();
}


/*-------------------------------------------------------------------------------
 * BtlRfcommCallback()
 *
 *		Internal callback for handling RFCOMM events in BTL layer.
 *		This function forwards events to registered application callback.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		chnl [in] - Internal RFCOMM channel.
 *
 *		Parms [in] - Internal RFCOMM event.
 *
 * Returns:
 *		void.
 */
static void BtlRfcommCallback(RfChannel *chnl, RfCallbackParms *parms)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BOOL passEventToApp = TRUE;
	BOOL notifyRadioOffComplete = FALSE;
	char addr[BDADDR_NTOA_SIZE];
	BtlRfcommContext *rfcommContext;
	BtlRfcommChannel *btlChannel;
	U32 idx,idx2;
	char *channelType;
	BtPacket *packet;
	
	/* Find BTL RFCOMM channel according to given RFCOMM channel */
	btlChannel = ContainingRecord(chnl, BtlRfcommChannel, channel);

	/* Find context according to RFCOMM channel */
	rfcommContext = (BtlRfcommContext*)(chnl->userContext);

	/* Find channel index in the context */
	idx = btlChannel->channelId;

	channelType = (((btlChannel->state & BTL_RFCOMM_CHANNEL_STATE_SERVER_MASK)) ? ("Server") : ("Client"));	

	/* Check if this channel is deregistering */
	if ((btlChannel->state == BTL_RFCOMM_CHANNEL_STATE_SERVER_DEREGISTERING) || 
		(btlChannel->state == BTL_RFCOMM_CHANNEL_STATE_CLIENT_DEREGISTERING))
	{
		switch (parms->event)
		{
			case (RFEVENT_CLOSED):
			{
				/* The channel is closed. */
				
				/* Remove ACL link, if exists */
				if (CMGR_IsLinkUp(&btlChannel->cmgrHandler) == TRUE)
				{
					status = CMGR_RemoveDataLink(&btlChannel->cmgrHandler);
					if (status != BT_STATUS_SUCCESS)
			        	BTL_LOG_ERROR(("Failed removing ACL link, status = ", status));
				}

				/* Release CMGR handler */
				status = CMGR_DeregisterHandler(&btlChannel->cmgrHandler);
				if (status != BT_STATUS_SUCCESS)
		        	BTL_LOG_ERROR(("Failed deregistering CMGR handler, status = ", status));

				if (btlChannel->state & BTL_RFCOMM_CHANNEL_STATE_SERVER_MASK)
				{
					btlChannel->state = BTL_RFCOMM_CHANNEL_STATE_SERVER_DISCONNECTED;
				}
				else
				{
				
#if BT_SECURITY == XA_ENABLED

					/* Deregister the security record */
					status = SEC_Unregister(&btlChannel->clientSecurityRecord);
					if (status != BT_STATUS_SUCCESS)
						BTL_LOG_ERROR(("Failed deregistering client security record!, status = %d", status));
	
#endif	/* BT_SECURITY == XA_ENABLED */

					btlChannel->state = BTL_RFCOMM_CHANNEL_STATE_IDLE;
					rfcommContext->clientCounter--;
				}

				btlChannel->modemStatusUsed = FALSE;				
				btlChannel->portSettingsUsed = FALSE;

				if (btlChannel->deregisterState & BTL_RFCOMM_DEREGISTER_STATE_MASK_DEREGISTER_ACTIVE)
				{
					/* Server channel is disconnected, so it can be deregistered */
					status = BtlDeregisterServerChannel(rfcommContext, idx);
				}

				if (btlChannel->deregisterState & BTL_RFCOMMP_DEREGISTER_STATE_MASK_RADIO_OFF_ACTIVE)
				{
					/* Verify all channels are idle or disconnected */
					BtlContext *base;
					BtlRfcommContext *context;

					notifyRadioOffComplete = TRUE;

					IterateList(btlRfcommData.contextsList, base, BtlContext *)
					{
						context = (BtlRfcommContext *)base;

						for (idx2 = 0; idx2 < BTL_CONFIG_RFCOMM_MAX_NUM_CHANNELS_PER_CONTEXT; idx2++)
						{
							if ((context->channels[idx2].state != BTL_RFCOMM_CHANNEL_STATE_IDLE) && 
								(context->channels[idx2].state != BTL_RFCOMM_CHANNEL_STATE_SERVER_DISCONNECTED))
								notifyRadioOffComplete = FALSE;
						}
					}
				}

				btlChannel->deregisterState = BTL_RFCOMM_DEREGISTER_STATE_MASK_NONE;

				BTL_LOG_INFO(("%s channel %d is disconnected from %s.", channelType, idx, bdaddr_ntoa(&(parms->ptrs.remDev->bdAddr), addr)));		

				/* Pass event to app, to indicate channel closed */
				break;
			}
			case (RFEVENT_PACKET_HANDLED):
			{
				/* RFCOMM is finished with the data packet provided in "ptrs.packet". */

				/* Save locally the pointer, since it is set to null when calling BTL_POOL_Free */
				packet = parms->ptrs.packet;			

				status = BTL_POOL_Free(&btlRfcommData.packetsPool, (void **)&packet);
				BTL_VERIFY_FATAL_NORET((BT_STATUS_SUCCESS == status), ("Failed freeing TX packet"));			

				BTL_LOG_INFO(("%s channel %d received packet handled indication, status = %d.", channelType, idx, parms->status));

				/* Pass event to app, so data pointer can be freed by app */
				break;
			}
			case (RFEVENT_MODEM_STATUS_CNF):
			{
				btlChannel->modemStatusUsed = FALSE;
		
				/* Do not pass event to app. */
				passEventToApp = FALSE;
				break;
			}
			case (RFEVENT_PORT_NEG_CNF):
			{
				btlChannel->portSettingsUsed = FALSE;

				/* Do not pass event to app. */
				passEventToApp = FALSE;
				break;
			}
			case (RFEVENT_OPEN_IND):
			{
				/* A remote device has requested a connection to a local RFCOMM service. */
				BTL_LOG_INFO(("%s channel %d received connect indication from %s while deregistering, rejecting request...", channelType, idx, bdaddr_ntoa(&(parms->ptrs.remDev->bdAddr), addr)));

				status = RF_RejectChannel(&btlChannel->channel);
				BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == status), ("Failed responding to open indication while deregistering."));				

				/* Do not pass event to app, since we already rejected the incoming open indication. */
				passEventToApp = FALSE;
				break;
			}
			case (RFEVENT_OPEN):
			{
				/* A channel is now open. */
				
				status = RF_CloseChannel(&btlChannel->channel);
				BTL_VERIFY_ERR_NORET((status == BT_STATUS_PENDING), ("Failed closing %s channel %d while deregistering", channelType, idx));

				if (BT_STATUS_PENDING == status)
				{
					if (btlChannel->state & BTL_RFCOMM_CHANNEL_STATE_SERVER_MASK)
						btlChannel->state = BTL_RFCOMM_CHANNEL_STATE_SERVER_DISCONNECTING;
					else
						btlChannel->state = BTL_RFCOMM_CHANNEL_STATE_CLIENT_DISCONNECTING;
				}

				/* Do not pass event to app, since we are closing the channel... */
				passEventToApp = FALSE;
				break;
			}
			default:
			{
				BTL_LOG_ERROR(("%s channel %d received unexpected event %d while deregistering!", channelType, idx, parms->event));	
				
				/* Do not pass event to app. */
				passEventToApp = FALSE;
				break;
			}
		}
	}
	else
	{
		switch (parms->event)
		{
			case (RFEVENT_PACKET_HANDLED):
			{
				/* RFCOMM is finished with the data packet provided in "ptrs.packet". */

				/* Save locally the pointer, since it is set to null when calling BTL_POOL_Free */
				packet = parms->ptrs.packet;			

				status = BTL_POOL_Free(&btlRfcommData.packetsPool, (void **)&packet);
				BTL_VERIFY_FATAL_NORET((BT_STATUS_SUCCESS == status), ("Failed freeing TX packet"));			
				
				BTL_LOG_INFO(("%s channel %d received packet handled indication, status = %d.", channelType, idx, parms->status));		
				break;
			}
			case (RFEVENT_OPEN_IND):
			{
				/* A remote device has requested a connection to a local RFCOMM service. */
				
				btlChannel->state = BTL_RFCOMM_CHANNEL_STATE_SERVER_CONNECT_IND;
				BTL_LOG_INFO(("%s channel %d received connect indication from %s.", channelType, idx, bdaddr_ntoa(&(parms->ptrs.remDev->bdAddr), addr)));
				break;
			}
			case (RFEVENT_OPEN):
			{
				/* A channel is now open and ready for data exchange. */
				
				if (btlChannel->state & BTL_RFCOMM_CHANNEL_STATE_SERVER_MASK)
				{
					/* Incoming connection, need to associate CMGR handler with the incoming link */
					status = CMGR_RegisterHandler(&btlChannel->cmgrHandler, BtlRfcommCmgrCallback);
					if (status == BT_STATUS_SUCCESS)
					{
						status = CMGR_CreateDataLink(&btlChannel->cmgrHandler, &(parms->ptrs.remDev->bdAddr));
						if (status != BT_STATUS_SUCCESS)
						{
							/* Release CMGR handler */
							(void)CMGR_DeregisterHandler(&btlChannel->cmgrHandler);
							BTL_LOG_ERROR(("Failed associating CMGR handler to incoming connection, status = %d", status));
						}
					}
					else
						BTL_LOG_ERROR(("Failed registering CMGR handler, status = ", status));

					btlChannel->state = BTL_RFCOMM_CHANNEL_STATE_SERVER_CONNECTED;
				}
				else
				{
					/* Outgoing connection */
					btlChannel->state = BTL_RFCOMM_CHANNEL_STATE_CLIENT_CONNECTED;
				}

				BTL_LOG_INFO(("%s channel %d is connected to %s.", channelType, idx, bdaddr_ntoa(&(parms->ptrs.remDev->bdAddr), addr)));
				
				break;
			}
			case (RFEVENT_CLOSE_IND):
			{
				/* A request to close a channel was received. */

				if (btlChannel->state & BTL_RFCOMM_CHANNEL_STATE_SERVER_MASK)
				{
					btlChannel->state = BTL_RFCOMM_CHANNEL_STATE_SERVER_DISCONNECT_IND;					
				}
				else
				{
					btlChannel->state = BTL_RFCOMM_CHANNEL_STATE_CLIENT_DISCONNECT_IND;
				}

				BTL_LOG_INFO(("%s channel %d received disconnect indication from %s.", channelType, idx, bdaddr_ntoa(&(parms->ptrs.remDev->bdAddr), addr)));
				
				break;
			}
			case (RFEVENT_CLOSED):
			{
				/* The channel is closed. */
				
				/* Remove ACL link, if exists */
				if (CMGR_IsLinkUp(&btlChannel->cmgrHandler) == TRUE)
				{
					status = CMGR_RemoveDataLink(&btlChannel->cmgrHandler);
					if (status != BT_STATUS_SUCCESS)
			        	BTL_LOG_ERROR(("Failed removing ACL link, status = ", status));
				}

				/* Release CMGR handler */
				status = CMGR_DeregisterHandler(&btlChannel->cmgrHandler);
				if (status != BT_STATUS_SUCCESS)
		        	BTL_LOG_ERROR(("Failed deregistering CMGR handler, status = ", status));

				if (btlChannel->state & BTL_RFCOMM_CHANNEL_STATE_SERVER_MASK)
				{
					btlChannel->state = BTL_RFCOMM_CHANNEL_STATE_SERVER_DISCONNECTED;
				}
				else
				{
				
#if BT_SECURITY == XA_ENABLED

					/* Deregister the security record */
					status = SEC_Unregister(&btlChannel->clientSecurityRecord);
					if (status != BT_STATUS_SUCCESS)
						BTL_LOG_ERROR(("Failed deregistering client security record!, status = %d", status));
	
#endif	/* BT_SECURITY == XA_ENABLED */

					btlChannel->state = BTL_RFCOMM_CHANNEL_STATE_IDLE;
					rfcommContext->clientCounter--;
				}

				btlChannel->modemStatusUsed = FALSE;				
				btlChannel->portSettingsUsed = FALSE;

				BTL_LOG_INFO(("%s channel %d is disconnected from %s.", channelType, idx, bdaddr_ntoa(&(parms->ptrs.remDev->bdAddr), addr)));		
		        
				break;
			}
			case (RFEVENT_DATA_IND):
			{
				/* Data was received from the remote device. */
				
				BTL_LOG_INFO(("%s channel %d received data indication of %d bytes.", channelType, idx, parms->dataLen));
				break;
			}
			case (RFEVENT_MODEM_STATUS_CNF):
			{
				btlChannel->modemStatusUsed = FALSE;
				break;
			}
			case (RFEVENT_PORT_NEG_CNF):
			{
				btlChannel->portSettingsUsed = FALSE;
				break;
			}
		}
	}

	if (TRUE == passEventToApp)
	{
		/* Set the context in the event passed to app */
		btlRfcommData.event.rfcommContext = rfcommContext;

		/* Set the channel index in the event passed to app */
		btlRfcommData.event.channelId = idx;

		/* Set the internal event in the event passed to app */
		btlRfcommData.event.rfcommEvent = parms;

		/* Pass the event to app */
		rfcommContext->callback(&btlRfcommData.event);
	}

	if (TRUE == notifyRadioOffComplete)
	{
		BtlModuleNotificationCompletionEvent moduleCompletionEvent;
		moduleCompletionEvent.type = BTL_MODULE_NOTIFICATION_COMPLETION_EVENT_TYPE_RADIO_OFF;
		moduleCompletionEvent.status = BT_STATUS_SUCCESS;
		
		BTL_ModuleCompleted(BTL_MODULE_TYPE_RFCOMM, &moduleCompletionEvent);
	}
}


	
BtStatus BtlRfcommBtlNotificationsCb(BtlModuleNotificationType notificationType)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtStatus retVal;
	BtlContext *base;
	BtlRfcommContext *rfcommContext;
	BtlRfcommChannel *btlChannel;
	U32 idx;

	BTL_FUNC_START("BtlRfcommBtlNotificationsCb");
	
	BTL_LOG_DEBUG(("Notification Type: %d", notificationType));
	
	switch (notificationType)
	{
		case BTL_MODULE_NOTIFICATION_RADIO_ON:

			/* Do nothing */
		
			break;

		case BTL_MODULE_NOTIFICATION_RADIO_OFF:

			IterateList(btlRfcommData.contextsList, base, BtlContext *)
			{
				rfcommContext = (BtlRfcommContext *)base;

				for (idx = 0; idx < BTL_CONFIG_RFCOMM_MAX_NUM_CHANNELS_PER_CONTEXT; idx++)
				{
					btlChannel = &(rfcommContext->channels[idx]);

					switch (btlChannel->state)
					{
						case (BTL_RFCOMM_CHANNEL_STATE_SERVER_DISCONNECTING):
						case (BTL_RFCOMM_CHANNEL_STATE_SERVER_DISCONNECT_IND):
						{
							btlChannel->state = BTL_RFCOMM_CHANNEL_STATE_SERVER_DEREGISTERING;
							break;
						}
						case (BTL_RFCOMM_CHANNEL_STATE_CLIENT_DISCONNECTING):
						case (BTL_RFCOMM_CHANNEL_STATE_CLIENT_DISCONNECT_IND):
						{
							btlChannel->state = BTL_RFCOMM_CHANNEL_STATE_CLIENT_DEREGISTERING;
							break;
						}
						case (BTL_RFCOMM_CHANNEL_STATE_SERVER_CONNECT_IND):
						{
							/* Reject incoming open request, and wait for close event */
							
							retVal = RF_RejectChannel(&btlChannel->channel);
							BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == retVal), ("Failed responding to open indication while deregistering."));

							if (BT_STATUS_PENDING == retVal)
								btlChannel->state = BTL_RFCOMM_CHANNEL_STATE_SERVER_DEREGISTERING;
							
							break;
						}
						case (BTL_RFCOMM_CHANNEL_STATE_SERVER_CONNECTED):
						{
							/* Close channel and wait for close event */
							
							retVal = RF_CloseChannel(&btlChannel->channel);
							BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == retVal), ("Failed closing channel %d", idx));

							if (BT_STATUS_PENDING == retVal)
								btlChannel->state = BTL_RFCOMM_CHANNEL_STATE_SERVER_DEREGISTERING;
							
							break;
						}
						case (BTL_RFCOMM_CHANNEL_STATE_CLIENT_CONNECTED):
						{
							/* Close channel and wait for close event */
							
							retVal = RF_CloseChannel(&btlChannel->channel);
							BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == retVal), ("Failed closing channel %d", idx));

							if (BT_STATUS_PENDING == retVal)
								btlChannel->state = BTL_RFCOMM_CHANNEL_STATE_CLIENT_DEREGISTERING;
							
							break;
						}
						case (BTL_RFCOMM_CHANNEL_STATE_CLIENT_CONNECTING):
						{
							/* In process of connecting, let it finish and then close the channel */

							btlChannel->state = BTL_RFCOMM_CHANNEL_STATE_CLIENT_DEREGISTERING;
							
							break;
						}
					}

					if ((btlChannel->state == BTL_RFCOMM_CHANNEL_STATE_SERVER_DEREGISTERING) || 
						(btlChannel->state == BTL_RFCOMM_CHANNEL_STATE_CLIENT_DEREGISTERING))
					{
						btlChannel->deregisterState |= BTL_RFCOMMP_DEREGISTER_STATE_MASK_RADIO_OFF_ACTIVE;
						status = BT_STATUS_PENDING;
					}
				}
			}
			
			break;

		default:

			BTL_ERR(BT_STATUS_INTERNAL_ERROR, ("Invalid notification (%d)", notificationType));
			
			break;
	}

	BTL_FUNC_END();
	
	return (status);
}



#else /*BTL_CONFIG_BTL_RFCOMM == BTL_CONFIG_ENABLED*/

/*-------------------------------------------------------------------------------
 * BTL_RFCOMM_Init() - When  BTL_CONFIG_BTL_RFCOMM is disabled.
 */
BtStatus BTL_RFCOMM_Init(void)
{
    
   BTL_LOG_INFO(("BTL_RFCOMM_Init()  -  BTL_CONFIG_BTL_RFCOMM Disabled"));
  

    return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 *BTL_RFCOMM_Deinit() - When  BTL_CONFIG_BTL_RFCOMM is disabled.
 */
BtStatus BTL_RFCOMM_Deinit(void)
{
    BTL_LOG_INFO(("BTL_RFCOMM_Deinit() -  BTL_CONFIG_BTL_RFCOMM Disabled"));

    return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------*/



#endif /*BTL_CONFIG_BTL_RFCOMM == BTL_CONFIG_ENABLED*/


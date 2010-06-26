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
*   FILE NAME:		btl_vg_utils.c
*
*   DESCRIPTION:	Implementation of internal shared API.
*
*   AUTHOR:			Itay Klein
*
\*******************************************************************************/


#include "btl_config.h"
#if BTL_CONFIG_VG == BTL_CONFIG_ENABLED

/*******************************************************************************
 *
 *	Include files
 *
 ******************************************************************************/

#include "debug.h"
#include "btl_defs.h"
#include "btl_vgi.h"
#include "bthal_os.h"
#include "hci.h"
#include "hfgi.h"
#include "hsgi.h"


/*******************************************************************************
 *
 *	Constant
 *
 ******************************************************************************/

/*-----------------------------------------------------------------------------
 * FEATURES_MASK_OFFSET Constant
 *
 *	The features mask offset in bytes in the data received with the HCI Event:
 *	HCE_READ_REMOTE_FEATURES_COMPLETE.
 */
 #define BTL_VG_UTILS_FEATURES_MASK_OFFSET	3

/*-----------------------------------------------------------------------------
 * FEATURES_MASK_SIZE Constant
 *
 *	The size in bytes of a feature mask.
 */
 #define BTL_VG_UTILS_FEATURES_MASK_SIZE		8


/*-----------------------------------------------------------------------------
 * BTL_VG_MAX_BTL_VG_STRINGS_TO_FREE Constant
 *
 *	When issuing an AT response BtlVgStrings may be allocated to hold the data.
 *	Those BtlVgStrings are needed to be free when the AT Response has been sent.
 *	This is the maximal number of BtlVgStrings to free for a response.
 */
 #define BTL_VG_MAX_BTL_VG_STRINGS_TO_FREE	3

#if (0)
/*-----------------------------------------------------------------------------
 * FM_OVER_BT_MODE Constant
 *
 *	Opcode of HCI Vendor Specific command for BRF6350.
 */
#define FM_OVER_BT_MODE		                                (0xFD61)

#endif /* BTL_CONFIG_FM_STACK == BTL_CONFIG_ENABLED */


/*******************************************************************************
 *
 *	Globals
 *
 ******************************************************************************/

static const U8 BtlVgServiceSearchAttribReq[] = 
{
	/* First parameter is the search pattern in data element format. It
	 * is a list of 3 UUIDs. 
	 */
	SDP_ATTRIB_HEADER_8BIT(3),     /* Data Element Sequence, 3 bytes */ 

	SDP_UUID_16BIT(SC_HANDSFREE),	/* UUID for Handsfree in Big Endian */

	/* The second parameter is the maximum number of bytes that can be
	 * be received for the attribute list
	 */
	0x00, 0x64,	/* Max number of bytes for attribute is 100 */

	SDP_ATTRIB_HEADER_8BIT(3),
	SDP_UINT_16BIT(AID_SERVICE_CLASS_ID_LIST)
};



BtlVgModuleData voiceGatewayModuleData = {BTL_VG_MODULE_STATE_NOT_INITIALIZED,
										  "BTL VG Responses pool",
										  "BTL VG Incoming events pool",
										  "BTL VG MeCommandToken pool",
										  "BTL VG BtlVgString pool",
										  0};

/*******************************************************************************
 *
 *	Internal functions.
 *
 ******************************************************************************/

static BtStatus BtlVgUtilsCallChannelFunction(BtlVgChannel *channel,
											  BtStatus (*hfgFunc)(HfgChannel *channel),
											  BtStatus (*hsgFunc)(HsgChannel *channel));

static void BtlVgUtilsCmgrCallback(CmgrHandler *cmgrHandler,
									CmgrEvent event,
									BtStatus status);

static void BtlVgUtilsSdpCallback(const BtEvent *event);

static void BtlVgUtilsQueryEscoCallback(const BtEvent *event);

static BtStatus BtlVgUtilsSdpQuery(BtlVgConnectionHandler *handler,
								SdpQueryMode mode);

static void BtlVgUtilsHandleSdpQueryFinished(BtlVgConnectionHandler *handler,
                                             BOOL result);

static BtStatus BtlVgUtilsVerifySdpQueryRsp(BtlVgConnectionHandler *handler,
											SdpQueryToken *token);

static void BtlVgUtilsSetAudioParms(const BtRemoteDevice *remDev,
									CmgrAudioParms audioParms);

static BtStatus BtlVgUtilsRegisterServices(void);

static BtlVgChannel* BtlVgUtilsContainingVgChannel(const void *channel,
													BtlVgChannelType type);

static HfgChannel* BtlVgUtilsGetFreeHfgChannel(void);

static HsgChannel* BtlVgUtilsGetFreeHsgChannel(void);

static BtStatus BtlVgUtilsBtlNotificationsCb(BtlModuleNotificationType notificationType);

static BtStatus BtlVgUtilsProcessRadioOffNotification(void);

static BtStatus BtlVgUtilsRemoveAllConnections(void);

#if (0)

BtStatus BtlVgUtilsSetFmOverBt(BtlVgChannel *channel, BOOL enable);

static void BtlVgUtilsSetFmOverBtCallback(const BtEvent *event);

#endif /* BTL_CONFIG_FM_STACK == BTL_CONFIG_ENABLED */

/*******************************************************************************
 *
 *	Macros
 *
 ******************************************************************************/
/*-----------------------------------------------------------------------------
 * BTL_VG_UTILS_EXTENDED_SCO_LINK_BIT Macro
 *
 *	Extract from a features mask the relevant bit for extended SCO link support (EV3 packets)
 */
#define BTL_VG_UTILS_EXTENDED_SCO_LINK_BIT(FEATURES) ((FEATURES)[3] & 0x80)


/*-----------------------------------------------------------------------------
 * BTL_VG_UTILS_EDR_ESCO_2_MBPS_MODE_BIT Macro
 *
 *	Extract from a features mask the relevant bit for EDR in eSCO 2-MPS mode support
 */
#define BTL_VG_UTILS_EDR_ESCO_2_MBPS_MODE_BIT(FEATURES) ((FEATURES)[5] & 0x20)

/*******************************************************************************
 *
 *	Functions definitions
 *
 ******************************************************************************/

BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_VG);

BtStatus BtlVgUtilsConnect(BD_ADDR *addr, BtlVgChannel *channel)
{
	BtStatus status;
	BtlVgConnectionHandler *handler;
	
	BTL_FUNC_START("BtlVgUtilsConnect");

	handler = &channel->connectionHandler;
	
	handler->channelTypeMask = 0;

	channel->cancelCreateConn = FALSE;
	channel->cccConnState = BTL_VG_CCC_CONN_STATE_NONE;
    
	status = CMGR_RegisterHandler(&handler->cmgrHandler, BtlVgUtilsCmgrCallback);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("CMGR_RegisterHandler failed: %s", pBT_Status(status)));

	channel->state = BTL_VG_CHANNEL_STATE_CHECKING_SUPPORTED_SERVICES;
	channel->cccConnState = BTL_VG_CCC_CONN_STATE_CONNECTING_ACL;

	status = CMGR_CreateDataLink(&handler->cmgrHandler, addr);
	if (BT_STATUS_SUCCESS == status)
	{
		channel->cccConnState = BTL_VG_CCC_CONN_STATE_SDP_QUERY;
    
		status = BtlVgUtilsSdpQuery(handler, BSQM_FIRST);
		if (BT_STATUS_PENDING != status)
		{
			BTL_LOG_ERROR(("BtlVgUtilsSdpQuery failed: %s", pBT_Status(status)));
			status = CMGR_RemoveDataLink(&handler->cmgrHandler);
			BTL_LOG_INFO(("CMGR_RemoveDataLink returned: %s",pBT_Status(status)));
			channel->cccConnState = BTL_VG_CCC_CONN_STATE_NONE;
		}
	}
	else if (BT_STATUS_PENDING != status)
	{
		channel->state = BTL_VG_CHANNEL_STATE_IDLE;
		BTL_LOG_ERROR(("CMGR_CreateDataLink failed: %s",pBT_Status(status)));
		status = CMGR_DeregisterHandler(&handler->cmgrHandler);
		BTL_LOG_INFO(("CMGR_DeregisterHandler returned: %s", pBT_Status(status)));
		channel->cccConnState = BTL_VG_CCC_CONN_STATE_NONE;
		BTL_RET(BT_STATUS_FAILED);
	}

	BTL_FUNC_END();

	return status;
}

static BtStatus BtlVgUtilsSdpQuery(BtlVgConnectionHandler *handler, SdpQueryMode mode)
{
	BtStatus status;
	
	BTL_FUNC_START("BtlVgUtilsSdpQuery");

	handler->sdpQueryToken.parms = BtlVgServiceSearchAttribReq;
	handler->sdpQueryToken.plen = sizeof(BtlVgServiceSearchAttribReq);
	handler->sdpQueryToken.type = BSQT_SERVICE_SEARCH_ATTRIB_REQ;
	handler->sdpQueryToken.callback = BtlVgUtilsSdpCallback;
	handler->sdpQueryToken.rm = handler->cmgrHandler.bdc->link;

	status = SDP_Query(&handler->sdpQueryToken, mode);
    
	BTL_FUNC_END();

	return status;
}

static void BtlVgUtilsSdpCallback(const BtEvent *event)
{
	BtStatus status;
	BtlVgConnectionHandler *handler;

	BTL_FUNC_START("BtlVgUtilsSdpCallback");

	handler = ContainingRecord(event->p.token, BtlVgConnectionHandler, sdpQueryToken);

	switch (event->eType)
	{
	case SDEVENT_QUERY_RSP:
		status = BtlVgUtilsVerifySdpQueryRsp(handler, event->p.token);
		if (BT_STATUS_SDP_CONT_STATE == status)
		{
			status = BtlVgUtilsSdpQuery(handler, BSQM_CONTINUE);
			BTL_VERIFY_ERR((BT_STATUS_PENDING == status), status, ("Could not continue query: %s", pBT_Status(status)));
		}
		else
		{
			BtlVgUtilsHandleSdpQueryFinished(handler, TRUE);
		}
		break;

	case SDEVENT_QUERY_ERR:
	case SDEVENT_QUERY_FAILED:
		BTL_LOG_INFO(("BtlVgUtilsSdpCallback, sdp failed"));
		BtlVgUtilsHandleSdpQueryFinished(handler, FALSE);
		break;
        
        default:
        	break;
	}

	BTL_FUNC_END();
}

static void BtlVgUtilsCmgrCallback(CmgrHandler *cmgrHandler, CmgrEvent event, BtStatus status)
{
	BtlVgConnectionHandler *handler;
	BtlVgChannel *channel;
	BOOL closeChannel = TRUE;
	BtlVgContext *context;
	BtErrorCode errCode;

	BTL_FUNC_START("BtlVgUtilsCmgrCallback");

	handler = ContainingRecord(cmgrHandler, BtlVgConnectionHandler, cmgrHandler);
	channel = ContainingRecord(handler, BtlVgChannel, connectionHandler);	

	switch (event)
	{
	case CMEVENT_DATA_LINK_CON_CNF:
		if ((BT_STATUS_SUCCESS == status) && (FALSE == voiceGatewayModuleData.isRadioOffNotified))
		{
			BtlVgUtilsQueryEscoSupport(handler->cmgrHandler.remDev);

			channel->cccConnState = BTL_VG_CCC_CONN_STATE_SDP_QUERY;
            
			status = BtlVgUtilsSdpQuery(handler, BSQM_FIRST);
			if (BT_STATUS_PENDING != status)
			{
				BTL_LOG_ERROR(("BtlVgUtilsSdpQuery failed: %s", pBT_Status(status)));
				status = CMGR_RemoveDataLink(cmgrHandler);
				BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == status || BT_STATUS_PENDING == status),
					("CMGR_RemoveDataLink failed: %s", pBT_Status(status)));
				errCode = BEC_SDP_QUERY_FAILED;
			}
			else
			{
				closeChannel = FALSE;
			}
		}
		else
		{
			/* If status is not BT_STATUS_SUCCESS then go to
			 * deregister and free the handler for future use */
			errCode = BEC_NO_CONNECTION;
		}
		break;
		
	case CMEVENT_DATA_LINK_DIS:
		errCode = BEC_NO_CONNECTION;
		break;

	default:
		closeChannel = FALSE;
		break; 
	}

	if (TRUE == closeChannel)
	{
		status = CMGR_DeregisterHandler(cmgrHandler);
		BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == status),
			("CMGR_DeregisterHandler failed: %s", pBT_Status(status)));
		channel->state = BTL_VG_CHANNEL_STATE_IDLE;
		channel->type = BTL_VG_UNDEFINED_CHANNEL;
		channel->c.channel = 0;
		channel->cccConnState = BTL_VG_CCC_CONN_STATE_NONE;
        
		context = &voiceGatewayModuleData.context;
        
		if (TRUE == context->handover)
		{
		    BtlVgUtilsSendEventToUser(channel, BTL_VG_EVENT_HANDOVER_FAILED, status, errCode, 0);
		}
		else
		{
		    BtlVgUtilsSendEventToUser(channel, BTL_VG_EVENT_SLC_DISCONNECTED, status, errCode, 0);
		}
                
		context->handover = FALSE;
		context->handoverTarget = 0;
		context->handoverSource = 0;
	}

	if (TRUE == voiceGatewayModuleData.isRadioOffNotified && 0 == context->numChannels)
	{
		/*	All SLCs are removed, 
		 *	notify that VG is ready for radio shutdown
		 */
		BtlModuleNotificationCompletionEvent event;
		
		event.type = BTL_MODULE_NOTIFICATION_COMPLETION_EVENT_TYPE_RADIO_OFF;
		event.status = BT_STATUS_SUCCESS;
		BTL_ModuleCompleted(BTL_MODULE_TYPE_VG, &event);
              voiceGatewayModuleData.isRadioOffNotified = FALSE;
	}
    
	BTL_FUNC_END();
}

static void BtlVgUtilsHandleSdpQueryFinished(BtlVgConnectionHandler *handler,
                                             BOOL result)
{
	BtStatus status = BT_STATUS_FAILED;
	BtlVgChannel *channel;
	BtlVgContext *context;
    BtErrorCode errCode = BEC_CONNECTION_FAILED;
	
	BTL_FUNC_START("BtlVgUtilsHandleSdpQueryFinished");

	channel = ContainingRecord(handler, BtlVgChannel, connectionHandler);
    
	/* Do not establish SLC in case of cancellation */
	if (FALSE == channel->cancelCreateConn)
	{
        /* Create SLC, if SDP query has been successfully finished */
        if ((TRUE == result) && (FALSE == voiceGatewayModuleData.isRadioOffNotified))
        {
        	if (handler->channelTypeMask & BTL_VG_HANDSFREE_CHANNEL)
        	{
        		status = BtlVgUtilsCreateServiceLink(&(handler->cmgrHandler.bdc->link->bdAddr), channel, BTL_VG_HANDSFREE_CHANNEL);
        	}
        	else
        	{
        		status = BtlVgUtilsCreateServiceLink(&(handler->cmgrHandler.bdc->link->bdAddr), channel, BTL_VG_HEADSET_CHANNEL);
        	}

        	if (BT_STATUS_PENDING == status)
        	{
        		channel->cccConnState = BTL_VG_CCC_CONN_STATE_CONNECTING;
        	}
        }
        	else
        	{
            errCode = BEC_SDP_QUERY_FAILED;
        }

        /* SDP query failed or SLC creation wasn't successfully started */
        if ((FALSE == result) || (BT_STATUS_PENDING != status))
        {
        		channel->state = BTL_VG_CHANNEL_STATE_IDLE;
        		channel->type = BTL_VG_UNDEFINED_CHANNEL;
        		channel->c.handsfreeChannel = 0;
        		channel->cccConnState = BTL_VG_CCC_CONN_STATE_NONE;

        		context = &voiceGatewayModuleData.context;
		
        		if (TRUE == context->handover)
        		{
	            BtlVgUtilsSendEventToUser(channel, BTL_VG_EVENT_HANDOVER_FAILED, status, errCode, 0);
        		}
        		else
        		{
	            BtlVgUtilsSendEventToUser(channel, BTL_VG_EVENT_SLC_DISCONNECTED, status, errCode, 0);
        		}
                
        		context->handover = FALSE;
        		context->handoverSource = 0;
        		context->handoverTarget = 0;
        	}
	}
	else
	{
		BTL_LOG_INFO(("BTL VG: Cancel create connection after SDP query"));
		channel->state = BTL_VG_CHANNEL_STATE_IDLE;
		channel->type = BTL_VG_UNDEFINED_CHANNEL;
		channel->c.handsfreeChannel = 0;
        	channel->cccConnState = BTL_VG_CCC_CONN_STATE_NONE;
	}
	
	status = CMGR_RemoveDataLink(&handler->cmgrHandler);
	BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == status || BT_STATUS_SUCCESS == status),
		("CMGR_RemoveDataLink failed: %s", pBT_Status(status)));
	(void) CMGR_DeregisterHandler(&handler->cmgrHandler);

	if (TRUE == voiceGatewayModuleData.isRadioOffNotified && 0 == context->numChannels)
	{
		/*	All SLCs are removed, 
		 *	notify that VG is ready for radio shutdown
		 */
		BtlModuleNotificationCompletionEvent event;
		
		event.type = BTL_MODULE_NOTIFICATION_COMPLETION_EVENT_TYPE_RADIO_OFF;
		event.status = BT_STATUS_SUCCESS;
		BTL_ModuleCompleted(BTL_MODULE_TYPE_VG, &event);
              voiceGatewayModuleData.isRadioOffNotified = FALSE;
	}
    
	BTL_FUNC_END();
}

static BtStatus BtlVgUtilsVerifySdpQueryRsp(BtlVgConnectionHandler *handler,
									SdpQueryToken *token)
{
	BtStatus status;

	BTL_FUNC_START("BtlVgUtilsVerifySdpQueryRsp");

	token->attribId = AID_SERVICE_CLASS_ID_LIST;
	token->uuid = SC_HANDSFREE;
	token->mode = BSPM_BEGINNING;

	status = SDP_ParseAttributes(token);
	if (BT_STATUS_SDP_CONT_STATE == status)
	{
		token->mode = BSPM_CONT_STATE;
	}
	else	if (BT_STATUS_SUCCESS == status)
	{
		BTL_LOG_INFO(("HF Service class found"));
		handler->channelTypeMask |= BTL_VG_HANDSFREE_CHANNEL;
	}
	else
	{
		/*HF service not found */
		status = BT_STATUS_SUCCESS;
	}
	
	BTL_FUNC_END();

	return status;
}


BtlVgChannel* BtlVgUtilsChannelIdToChannel(BtlVgChannelId channelId)
{
	BtlVgChannel *channel;

	BTL_FUNC_START("BtlVgUtilsChannelIdToChannel");

 	channel = voiceGatewayModuleData.context.channels + channelId;

	BTL_FUNC_END();

	return channel;
}

BtlVgChannelId BtlVgUtilsChannelToChannelId(const BtlVgChannel *channel)
{
	BtlVgChannelId channelId;

	BTL_FUNC_START("BtlVgUtilsChannelToChannelId");
	
	channelId =  channel - voiceGatewayModuleData.context.channels;

	BTL_FUNC_END();

	return channelId;
}

BtlVgChannelType BtlVgUtilsGetChannelType(const BtlVgChannel *channel)
{
	BtlVgChannelType type;

	BTL_FUNC_START("BtlVgUtilsGetChannelType");

	type = channel->type;

	BTL_FUNC_END();
	
	return type;
}

BD_ADDR* BtlVgUtilsGetDeviceAddress(BtlVgChannel *channel)
{
	BD_ADDR *bdAddr = 0;
	CmgrHandler *handler;
	const BtRemoteDevice *remDev;

	BTL_FUNC_START("BtlVgUtilsGetDeviceAddress");

	handler = BtlVgUtilsGetCmgrHandler(channel);
	BTL_VERIFY_ERR_SET_RETVAR((0 != handler),bdAddr = 0, ("BtlVgUtilsGetCmgrHandler() returned 0"));
	
	remDev = CMGR_GetRemoteDevice(handler) ;
	BTL_VERIFY_ERR_SET_RETVAR((0 != remDev), bdAddr = 0, ("CMGR_GetRemoteDevice() returned 0"));
	
	bdAddr = (BD_ADDR*)(0 != remDev ? &remDev->bdAddr : 0);

	BTL_FUNC_END();

	return bdAddr;
}

CmgrHandler* BtlVgUtilsGetCmgrHandler(BtlVgChannel *channel)
{
	CmgrHandler *handler = 0;
	BtlVgChannelType type;

	BTL_FUNC_START("BtlVgUtilsGetCmgrHandler");

	type = BtlVgUtilsGetChannelType(channel);

	switch (type)
	{
	case BTL_VG_HANDSFREE_CHANNEL:
		handler = &channel->c.handsfreeChannel->cmgrHandler;
		break;

	case BTL_VG_HEADSET_CHANNEL:
		handler = &channel->c.headsetChannel->cmgrHandler;
		break;
		
	case BTL_VG_UNDEFINED_CHANNEL:
		handler = &channel->connectionHandler.cmgrHandler;
		break;
		
	default:
		BTL_FATAL_SET_RETVAR(handler = 0, ("Illegal BtlVgChannel type: %d",(int)type));
	}

	BTL_FUNC_END();

	return handler;
}

BtStatus BtlVgUtilsInit(void)
{
	BtStatus status;

	BTL_FUNC_START("BtlVgUtilsInit");

	status = BTL_POOL_Create(&voiceGatewayModuleData.responsesPool,
		voiceGatewayModuleData.responsesPoolName,
		voiceGatewayModuleData.responsesMemory, 
		BTL_VG_MAXIMUM_NUMBER_OF_AVAILABLE_RESPONSES,
		sizeof(AtResults));

	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, 
		("pool creation failed"));

	status = BTL_POOL_Create(&voiceGatewayModuleData.incomingEventsPool,
		voiceGatewayModuleData.incomingEventsPoolName,
		voiceGatewayModuleData.incomingEventsMemory, 
		BTL_VG_MAXIMUM_NUMBER_OF_AVAILABLE_INCOMING_EVENTS,
		sizeof(BtlVgIncomingEvent));

	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR,
		("pool creation failed"));

	status = BTL_POOL_Create(&voiceGatewayModuleData.commandTokensPool,
		voiceGatewayModuleData.commandTokensPoolName,
		voiceGatewayModuleData.commandTokensMemory,
		BTL_VG_MAXIMUM_NUMBER_OF_ME_COMMAND_TOKENS,
		sizeof(MeCommandToken));

	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR,
		("pool creation failed"));

	status = BTL_POOL_Create(&voiceGatewayModuleData.btlVgStringsPool,
		voiceGatewayModuleData.btlVgStringsPoolName,
		voiceGatewayModuleData.btlVgStringsMemory,
		BTL_VG_MAXIMUM_NUMBER_OF_BTL_VG_STRINGS,
		sizeof(BtlVgString));

	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR,
		("pool creation failed"));
	
	InitializeListHead(&voiceGatewayModuleData.incomingEventsQue);

	voiceGatewayModuleData.isRadioOffNotified = FALSE;
	voiceGatewayModuleData.state = BTL_VG_MODULE_STATE_INITIALIZED;
	voiceGatewayModuleData.context.state = BTL_VG_CONTEXT_STATE_DESTROYED;

	/* Reset identifiers of commands sent to BTHAL MC and BTHAL VC and their responses
	 * with invalid value = 0 */
	voiceGatewayModuleData.bthalMcCommandId = 0;
	voiceGatewayModuleData.bthalMcResponseId = 0;
	voiceGatewayModuleData.bthalVcCommandId = 0;
	voiceGatewayModuleData.bthalVcResponseId = 0;
    

	BTL_RegisterForCommonNotifications(BTL_MODULE_TYPE_VG, BtlVgUtilsBtlNotificationsCb);	

	BTL_FUNC_END();

	return status;
}

BtStatus BtlVgUtilsDeinit(void)
{
	BtStatus status;

	BTL_FUNC_START("BtlVgUtilsDeinit");

	BTL_DeRegisterFromCommonNotifications(BTL_MODULE_TYPE_VG);

	status = BTL_POOL_Destroy(&voiceGatewayModuleData.responsesPool);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR,
		("pool destruction failed"));

	status = BTL_POOL_Destroy(&voiceGatewayModuleData.incomingEventsPool);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR,
		("pool destruction failed"));

	status = BTL_POOL_Destroy(&voiceGatewayModuleData.commandTokensPool);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR,
		("pool destruction failed"));

	status = BTL_POOL_Destroy(&voiceGatewayModuleData.btlVgStringsPool);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR,
		("pool destruction failed"));

	voiceGatewayModuleData.state = BTL_VG_MODULE_STATE_NOT_INITIALIZED;;

	BTL_FUNC_END();

	return status;
}

BtStatus BtlVgUtilsCreate(BtlAppHandle *handle,
						  BtlVgCallback callback,
						  const BtSecurityLevel *secLevel)
{
	BtStatus status;
	BtlVgContext *context;

	BTL_FUNC_START("BtlVgUtilsCreate");

	context = &voiceGatewayModuleData.context;

	status = BTL_HandleModuleInstanceCreation(handle, BTL_MODULE_TYPE_VG, &context->base);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("failed handling VG Instance creation"));

#if BT_SECURITY == XA_ENABLED
	status = BtlVgUtilsSetSecurityLevel(secLevel);
#endif

	context->callback = callback;
	context->state = BTL_VG_CONTEXT_STATE_DISABLED;
	context->handover = FALSE;
	context->handoverSource = 0;
	context->handoverTarget = 0;
	context->handoverRequest = 0;

	BtlVgSmInitStateMachine();

	BTL_FUNC_END();

	return status;
}

BtStatus BtlVgUtilsDestroy(void)
{
	BtStatus status;
	BtlVgContext *context;

	BTL_FUNC_START("BtlVgUtilsDestroy");

	context = &voiceGatewayModuleData.context;

	status = BTL_HandleModuleInstanceDestruction(&context->base);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), status, ("context destruction failed"));

	context->state = BTL_VG_CONTEXT_STATE_DESTROYED;

	BTL_FUNC_END();

	return status;
}


BtStatus BtlVgUtilsSendResults(BtlVgChannel *channel,
							   AtCommand command,
							   U32 parms,
							   U16 parmLen)
{
	BtStatus status;
	AtResults *response;
	BtlVgChannelType type;

	BTL_FUNC_START("BtlVgUtilsSendResults");

	status = BTL_POOL_Allocate(&voiceGatewayModuleData.responsesPool, (void**)&response);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), status, ("Mem alloc failed"));

	type = BtlVgUtilsGetChannelType(channel);
	switch (type)
	{
	case BTL_VG_HANDSFREE_CHANNEL:
		status = HFG_SendHfResults(channel->c.handsfreeChannel, command, parms, parmLen, response, FALSE);
		break;

	case BTL_VG_HEADSET_CHANNEL:
		status = HSG_SendHsResults(channel->c.headsetChannel, command, parms, parmLen, response, FALSE);
		break;

	case BTL_VG_UNDEFINED_CHANNEL:
	default:
		BTL_FATAL(BT_STATUS_INTERNAL_ERROR, ("Illegal BtlVgChannel type: %d",(int)type));
	}

	if (BT_STATUS_PENDING != status)
	{
		BtStatus temp;

		temp = BTL_POOL_Free(&voiceGatewayModuleData.responsesPool, (void**)&response);
		BTL_VERIFY_FATAL_NORET((BT_STATUS_SUCCESS == temp),("Mem free failed"));
	} 
	else
	{
		status = BT_STATUS_SUCCESS;
	}

	BTL_FUNC_END();

	return status;
}

void BtlVgUtilsSendEventToUser(BtlVgChannel *channel,
							   BtlVgEventType type,
							   BtStatus status,
							   BtErrorCode errCode,
							   const void *data)
{
	BtlVgEvent event;
	BtlVgContext *context;

	BTL_FUNC_START("BtlVgUtilsSendEventToUser");

	context = &voiceGatewayModuleData.context;

	event.channelId = (0 == channel) ? BTL_VG_CHANNEL_ID_NONE :BtlVgUtilsChannelToChannelId(channel);
	event.type = type;
	event.status = status;
	event.errCode = errCode;
	event.p.ptr = data;
	context->callback(&event);
	
	BTL_FUNC_END();
}


#if BT_SECURITY == XA_ENABLED
BtStatus BtlVgUtilsSetSecurityLevel(const BtSecurityLevel *secLevel)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtSecurityLevel level;

	BTL_FUNC_START("BtlVgUtilsSetSecurityLevel");

	if (0 != secLevel)
	{
		status = BTL_VerifySecurityLevel(*secLevel);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), BT_STATUS_INVALID_PARM, ("Invalid security level"));
		level = *secLevel;
	}
	else
	{
		level = BTL_VG_SECURITY_SETTINGS;		
	}
	HSG_SetSecurityLevel(level);
	HFG_SetSecurityLevel(level);

	BTL_FUNC_END();

	return status;
}
#endif /* BT_SECURITY == XA_ENABLED */

BtStatus BtlVgUtilsEnableAudioProfiles(const AtAgFeatures *features,
									   const char *serviceName)
{
	BtStatus status;
	U32 nChannels;
	BtlVgChannel *channel;
	BtlVgContext *context;

	BTL_FUNC_START("BtlVgUtilsEnableAudioProfiles");

	/* Setting the HF/HS AG Service name is not supported */
	UNUSED_PARAMETER(serviceName);

	context = &voiceGatewayModuleData.context;

	context->hsCallState = BTL_VG_HEADSET_CALL_STATE_NOCALL;

	channel = context->channels;
	for (nChannels = BTL_VG_MAXIMUM_NUMBER_OF_CHANNELS; nChannels > 0; --nChannels)
	{
		channel->audioRequested = FALSE;
		channel->state = BTL_VG_CHANNEL_STATE_IDLE;
		channel->type = BTL_VG_UNDEFINED_CHANNEL;
		channel->c.channel = 0;
		channel->pendingBthalVcCommand = FALSE;
        
		/* Reset pending command IDs, which were sent to BTHAL MC and BTHAL VC, with
		 * invalid ID = 0 */
		channel->bthalMcCommandId = 0;
		channel->bthalVcCommandId = 0;
        
		++channel;
	}

	status = BtlVgUtilsRegisterServices();
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status,
		("BtlVgUtilsRegisterServices() failed: %s", pBT_Status(status)));

	status = HFG_SetFeatures(0 == features ? HFG_DEFAULT_FEATURES : *features);
	BTL_VERIFY_FATAL_NORET((BT_STATUS_SUCCESS == status), ("Couldn't lock the HFG service record"));

	BTL_FUNC_END();

	return status;
}

BtStatus BtlVgUtilsDisableAudioProfiles(void)
{
	BtStatus status;

	BTL_FUNC_START("BtlVgUtilsDisableAudioProfiles");

	voiceGatewayModuleData.context.state = BTL_VG_CONTEXT_STATE_DISABLING;

	status = BtlVgUtilsRemoveAllConnections();

	if (BT_STATUS_SUCCESS == status)
	{
		/*	If there are no connections we can safly
		 *	deregister the HF and HS Services.
		 */
		status = BtlVgUtilsDeregisterServices();
	}

	BTL_FUNC_END();

	return status;
}

BtlVgChannel* BtlVgUtilsFindFreeChannel(void)
{
	U32 nChannels;
	BtlVgChannel *channel;

	BTL_FUNC_START("BtlVgUtilsFindFreeChannel");

	channel = voiceGatewayModuleData.context.channels;

	for (nChannels = BTL_VG_MAXIMUM_NUMBER_OF_CHANNELS; nChannels > 0; --nChannels)
	{
		if (BTL_VG_CHANNEL_STATE_IDLE == channel->state)
		{
			break;
		}
		++channel;
	}

	BTL_VERIFY_ERR_SET_RETVAR((0 != nChannels), (channel = 0), ("No free VG Channel"));

	BTL_FUNC_END();

	return channel;
}

void BtlVgUtilsHfgCallback(HfgChannel *channel,
						   HfgCallbackParms *parms)
{
	BtlVgIncomingEvent *event;
	BtlVgHfgEvent *hfgEvent;
	BtStatus status;
	BthalStatus halStatus;
 	
	BTL_FUNC_START("BtlVgUtilsHfgCallback");

	status = BTL_POOL_Allocate(&voiceGatewayModuleData.incomingEventsPool, (void**)&event);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), status, ("Mem alloc failed"));

	event->source = BTL_VG_EVENT_SOURCE_HFG;

	if (parms->event != HFG_EVENT_SERVICE_CONNECT_REQ)
	{
		event->channel = BtlVgUtilsContainingVgChannel((void*)channel, BTL_VG_HANDSFREE_CHANNEL);
		BTL_VERIFY_FATAL_NO_RETVAR((0 != event->channel), ("BtlVgUtilsContainingVgChannel returned 0"));
	}
	else
	{
		event->channel = 0;
	}

	hfgEvent = &event->e.hfgEvent;
	hfgEvent->type = parms->event;
	hfgEvent->status = parms->status;
	hfgEvent->errCode = parms->errCode;
	hfgEvent->channel = channel;

	switch (parms->event)
	{
	case HFG_EVENT_SERVICE_CONNECT_REQ:
	case HFG_EVENT_SERVICE_CONNECTED:
	case HFG_EVENT_SERVICE_DISCONNECTED:
	case HFG_EVENT_AUDIO_CONNECTED:
	case HFG_EVENT_AUDIO_DISCONNECTED:
		hfgEvent->p.remDev = parms->p.remDev;
		break;

	case HFG_EVENT_HANDSFREE_FEATURES:
		hfgEvent->p.features = parms->p.features;
		break;

	case HFG_EVENT_DIAL_NUMBER:
 		status = BtlVgUtilsCStringToBtlVgString(&hfgEvent->p.phoneNumber, parms->p.phoneNumber);
		BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == status),
			("BtlVgUtilsCStringToBtlVgString failed: %s",pBT_Status(status)));
		break;

	case HFG_EVENT_MEMORY_DIAL:
		status = BtlVgUtilsCStringToBtlVgString(&hfgEvent->p.memory, parms->p.memory);
		BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == status),
			("BtlVgUtilsCStringToBtlVgString failed: %s",pBT_Status(status)));
		break;

	case HFG_EVENT_CALL_HOLD:
		hfgEvent->p.hold.action = parms->p.hold->action;
		hfgEvent->p.hold.index = parms->p.hold->index;
		break;
		
	case HFG_EVENT_ENABLE_CALLER_ID:
	case HFG_EVENT_ENABLE_CALL_WAITING:
	case HFG_EVENT_ENABLE_VOICE_RECOGNITION:
		hfgEvent->p.enabled = parms->p.enabled;
		break;

	case HFG_EVENT_GENERATE_DTMF:
		hfgEvent->p.dtmf = parms->p.dtmf;
		break;

	case HFG_EVENT_REPORT_MIC_VOLUME:
	case HFG_EVENT_REPORT_SPK_VOLUME:
		hfgEvent->p.gain = parms->p.gain;
		break;

	case HFG_EVENT_AT_COMMAND_DATA:
		hfgEvent->p.atString.length = channel->atBuffer.buffSize;
		OS_MemCopy((U8*)hfgEvent->p.atString.string, channel->atBuffer.buff, channel->atBuffer.buffSize);
		break;

	case HFG_EVENT_RESPONSE_COMPLETE:
		hfgEvent->p.response = parms->p.response;
		break;

#if BT_SCO_HCI_DATA == XA_ENABLED
	case HFG_EVENT_AUDIO_DATA:
		/* TODO */
		break;

	case HFG_EVENT_AUDIO_DATA_SENT:
		/* TODO */
		break;
#endif

#if HFG_USE_RESP_HOLD == XA_ENABLED
	case HFG_RESPONSE_HOLD:
		hfgEvent->p.respHold = parms->p.respHold;
		break;
#endif

	case HFG_EVENT_SET_CHAR_SET:
		status = BtlVgUtilsCStringToBtlVgString(&hfgEvent->p.charSetType, parms->p.charSet);
		BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == status),
			("BtlVgUtilsCStringToBtlVgString failed: %s",pBT_Status(status)));

		break;

	case HFG_EVENT_SET_PHONEBOOK:
		hfgEvent->p.phonebookStorage = parms->p.phonebookStorage;
		break;

	case HFG_EVENT_READ_PHONEBOOK_ENTRIES:
		hfgEvent->p.phonebookRead.first = parms->p.phonebookRead->first;
		hfgEvent->p.phonebookRead.last = parms->p.phonebookRead->last;
		break;

	case HFG_EVENT_FIND_PHONEBOOK_ENTRIES:
		status = BtlVgUtilsCStringToBtlVgString(&hfgEvent->p.phonebookFindString, parms->p.phonebookFindText);
		BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == status),
			("BtlVgUtilsCStringToBtlVgString failed: %s",pBT_Status(status)));

		break;

	case HFG_EVENT_SELECTED_CHAR_SET:
	case HFG_EVENT_SELECTED_PHONEBOOK_INFO:
	case HFG_EVENT_READ_PHONEBOOK_ENTRIES_RANGE:
	case HFG_EVENT_SUPPORTED_PHONEBOOK_LIST:
	default:
		break;
	}
	
	InsertTailList(&voiceGatewayModuleData.incomingEventsQue, (ListEntry*)event);
	
	halStatus = BTHAL_OS_SendEvent(BTHAL_OS_TASK_HANDLE_STACK, OS_EVENT_STACK_TASK_VG);
	BTL_VERIFY_ERR_NO_RETVAR((BTHAL_STATUS_SUCCESS == halStatus), ("BTHAL_OS_SendEvent failed"));

	BTL_FUNC_END();
}

void BtlVgUtilsHsgCallback(HsgChannel *channel,
						   HsgCallbackParms *parms)
{
	BtlVgIncomingEvent *event;
	BtlVgHsgEvent *hsgEvent;
	BtStatus status;
	BthalStatus halStatus;

	BTL_FUNC_START("BtlVgUtilsHsgCallback");

	status = BTL_POOL_Allocate(&voiceGatewayModuleData.incomingEventsPool, (void**)&event);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), status, ("Mem alloc failed"));

	event->source = BTL_VG_EVENT_SOURCE_HSG;

	if (parms->event != HSG_EVENT_SERVICE_CONNECT_REQ)
	{
		event->channel = BtlVgUtilsContainingVgChannel((void*)channel, BTL_VG_HEADSET_CHANNEL);
		BTL_VERIFY_FATAL_NO_RETVAR((0 != event->channel), ("BtlVgUtilsContainingVgChannel returned 0"));
	}
	else
	{
		event->channel = 0;
	}

	hsgEvent = &event->e.hsgEvent;
	hsgEvent->type = parms->event;
	hsgEvent->status = parms->status;
	hsgEvent->errCode = parms->errCode;
	hsgEvent->channel = channel;

	switch (parms->event)
	{
	case HSG_EVENT_SERVICE_CONNECT_REQ:
	case HSG_EVENT_SERVICE_CONNECTED:
	case HSG_EVENT_SERVICE_DISCONNECTED:
	case HSG_EVENT_AUDIO_CONNECT_REQ:
	case HSG_EVENT_AUDIO_CONNECTED:
	case HSG_EVENT_AUDIO_DISCONNECTED:
		hsgEvent->p.remDev = parms->p.remDev;
		break;
		
	case HSG_EVENT_REPORT_MIC_VOLUME:
	case HSG_EVENT_REPORT_SPK_VOLUME:
		hsgEvent->p.gain = parms->p.gain;
		break;
		
	case HSG_EVENT_AT_COMMAND_DATA:
		hsgEvent->p.atString.length = parms->p.data->dataLen;
		OS_MemCopy((U8*)hsgEvent->p.atString.string,
			parms->p.data->rawData, parms->p.data->dataLen);
		break;
		
	case HSG_EVENT_RESPONSE_COMPLETE:
		hsgEvent->p.response = parms->p.response;
		break;

	case HSG_EVENT_BUTTON_PRESSED:
		break;

#if BT_SCO_HCI_DATA == XA_ENABLED
	case HSG_EVENT_AUDIO_DATA:
		/* TODO */
		break;

	case HSG_EVENT_AUDIO_DATA_SENT:
		/* TODO */
		break;
#endif

	default:
		BTL_ERR_NORET(("Unrecognized HSG event"));
	}
	InsertTailList(&voiceGatewayModuleData.incomingEventsQue, (ListEntry*)event);
	
	halStatus = BTHAL_OS_SendEvent(BTHAL_OS_TASK_HANDLE_STACK, OS_EVENT_STACK_TASK_VG);
	BTL_VERIFY_ERR_NO_RETVAR((BTHAL_STATUS_SUCCESS == halStatus), ("BTHAL_OS_SendEvent failed"));

	BTL_FUNC_END();
}

void BtlVgUtilsBthalMcCallback(const BthalMcEvent *mcEvent)
{
	BtlVgIncomingEvent *event;
	BtStatus status;
	BthalStatus halStatus;

	BTL_FUNC_START("BtlVgUtilsBthalMcCallback");

	status = BTL_POOL_Allocate(&voiceGatewayModuleData.incomingEventsPool, (void**)&event);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), status, ("Mem alloc failed"));

	event->source = BTL_VG_EVENT_SOURCE_MC;
	event->channel = 0;
	
	OS_MemCopy((void*)&event->e.mcEvent, (void*)mcEvent, sizeof(*mcEvent));

	InsertTailList(&voiceGatewayModuleData.incomingEventsQue, (ListEntry*)event);

	halStatus = BTHAL_OS_SendEvent(BTHAL_OS_TASK_HANDLE_STACK, OS_EVENT_STACK_TASK_VG);
	BTL_VERIFY_ERR_NO_RETVAR((BTHAL_STATUS_SUCCESS == halStatus), ("BTHAL_OS_SendEvent failed"));

	BTL_FUNC_END();
}

void BtlVgUtilsBthalVcCallback(const BthalVcEvent *vcEvent)
{
	BtlVgIncomingEvent *event;
	BtStatus status;
	BthalStatus halStatus;

	BTL_FUNC_START("BtlVgUtilsBthalVcCallback");

	status = BTL_POOL_Allocate(&voiceGatewayModuleData.incomingEventsPool, (void**)&event);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), status, ("Mem alloc failed"));

	event->source = BTL_VG_EVENT_SOURCE_VC;
	event->channel = 0;
	
	OS_MemCopy((void*)&event->e.vcEvent, (void*)vcEvent, sizeof(*vcEvent));

	InsertTailList(&voiceGatewayModuleData.incomingEventsQue, (ListEntry*)event);

	halStatus = BTHAL_OS_SendEvent(BTHAL_OS_TASK_HANDLE_STACK, OS_EVENT_STACK_TASK_VG);
	BTL_VERIFY_ERR_NO_RETVAR((BTHAL_STATUS_SUCCESS == halStatus), ("BTHAL_OS_SendEvent failed"));

	BTL_FUNC_END();
}

BtStatus BtlVgUtilsRouteAudioToChannel(BtlVgChannel *channel)
{
	BtStatus status;
	BthalStatus halStatus;
	
	BTL_FUNC_START("BtlVgUtilsRouteAudioToChannel");

	UNUSED_PARAMETER(channel);

 	halStatus = BTHAL_VC_SetVoicePath(voiceGatewayModuleData.context.voice,
		BTHAL_VC_AUDIO_SOURCE_MODEM, BTHAL_VC_AUDIO_PATH_BLUETOOTH);
	
	BTL_VERIFY_ERR((BTHAL_STATUS_PENDING == halStatus), BT_STATUS_FAILED, 	("BTHAL_VC_SetVoicePath failed"));

	status = BT_STATUS_PENDING;

	BTL_FUNC_END();

	return status;
}

BtStatus BtlVgUtilsRouteAudioToGateway(BtlVgChannel *channel)
{
	BtStatus status;
	BthalStatus halStatus;

	BTL_FUNC_START("BtlVgUtilsRouteAudioToGateway");

	UNUSED_PARAMETER(channel);

	halStatus = BTHAL_VC_SetVoicePath(voiceGatewayModuleData.context.voice,
		BTHAL_VC_AUDIO_SOURCE_MODEM, BTHAL_VC_AUDIO_PATH_HANDSET);

	BTL_VERIFY_ERR((BTHAL_STATUS_PENDING == halStatus), BT_STATUS_FAILED, 	("BTHAL_VC_SetVoicePath failed")); 

	status = BT_STATUS_PENDING;

	BTL_FUNC_END();

	return status;
}

BtStatus BtlVgUtilsConnectAudio(BtlVgChannel *channel)
{
	BtStatus status = BT_STATUS_FAILED;

	BTL_FUNC_START("BtlVgUtilsConnectAudio");

	BTL_LOG_INFO(("Setting audio parameters for the audio connection: audio parms = %d", (int)channel->audioParms));
	CMGR_SetAudioDefaultParms(channel->audioParms);
	CMGR_SetAudioVoiceSettings(channel->audioSettings);

	if (TRUE == channel->audioRequested)
	{
		BTL_LOG_INFO(("BTL VG: Accepting audio request"));
		/* HS audio requests are always accepted  */
		status = HFG_AcceptAudioLink(channel->c.handsfreeChannel, BEC_NO_ERROR);
		channel->audioRequested = FALSE;
	}
	else
	{
		BTL_LOG_INFO(("BTL VG: Establishing audio connection"));
		status = BtlVgUtilsCallChannelFunction(channel, HFG_CreateAudioLink, HSG_CreateAudioLink);
	}

	if (BT_STATUS_PENDING == status)
	{
		channel->state = BTL_VG_CHANNEL_STATE_CONNECTING_AUDIO;
	}
	
	BTL_FUNC_END();

	return status;
}

BtStatus BtlVgUtilsDisconnectAudio(BtlVgChannel *channel)
{
	BtStatus status;

	BTL_FUNC_START("BtlVgUtilsDisconnectAudio");

	if (TRUE == channel->audioRequested)
	{
		/* HS audio requests are always rejected  */
		status = HFG_AcceptAudioLink(channel->c.handsfreeChannel, BEC_USER_TERMINATED);
		channel->audioRequested = FALSE;
	}
	else
	{
		status = BtlVgUtilsCallChannelFunction(channel, HFG_DisconnectAudioLink, HSG_DisconnectAudioLink);
	}

	if (BT_STATUS_PENDING == status)
	{
		channel->state = BTL_VG_CHANNEL_STATE_DISCONNECTING_AUDIO;
	}
	
	BTL_FUNC_END();

	return status;
}

BtStatus BtlVgUtilsCreateServiceLink(BD_ADDR *bdAddr,
									BtlVgChannel *channel,
									BtlVgChannelType type)
{
	BtStatus status = BT_STATUS_FAILED; 

	BTL_FUNC_START("BtlVgUtilsCreateServiceLink");

	BTL_VERIFY_FATAL((0 != channel && 0 != bdAddr), BT_STATUS_INVALID_PARM,
		("BtlVgUtilsCreateServiceLink(): Null argument"));

	switch (type)
	{
	case BTL_VG_HANDSFREE_CHANNEL:
		channel->c.handsfreeChannel = BtlVgUtilsGetFreeHfgChannel();
		if (0 == channel->c.handsfreeChannel)
		{
			BTL_FATAL(BT_STATUS_NO_RESOURCES, ("BtlVgUtilsGetFreeHfgChannel() returned 0"));
		}
		channel->type = BTL_VG_HANDSFREE_CHANNEL;
		BtlVgSmUpdateAllIndicators(channel);
		status = HFG_CreateServiceLink(channel->c.handsfreeChannel, bdAddr);
		break;

	case BTL_VG_HEADSET_CHANNEL:
		channel->c.headsetChannel = BtlVgUtilsGetFreeHsgChannel();
		if (0 == channel->c.headsetChannel)
		{
			BTL_FATAL(BT_STATUS_NO_RESOURCES, ("BtlVgUtilsGetFreeHsgChannel() failed"));
		}
		
		channel->type = BTL_VG_HEADSET_CHANNEL;
		/* SCO is always used for headset regardsless of eSCO support */
		channel->audioParms = CMGR_AUDIO_PARMS_SCO;
		status = HSG_CreateServiceLink(channel->c.headsetChannel, bdAddr);
		break;

	case BTL_VG_UNDEFINED_CHANNEL:
		channel->state = BTL_VG_CHANNEL_STATE_IDLE;		
		BTL_FATAL(BT_STATUS_INTERNAL_ERROR, ("Channel type is undefined"));

	default:
		channel->state = BTL_VG_CHANNEL_STATE_IDLE;
		BTL_FATAL(BT_STATUS_INTERNAL_ERROR, ("Illegal BtlVgChannelType value: %d",(int)type));
	}

	if (BT_STATUS_PENDING == status)
	{
		++voiceGatewayModuleData.context.numChannels;
		channel->state = BTL_VG_CHANNEL_STATE_CONNECTING;
	}

	BTL_FUNC_END();

	return status;
}

BtStatus BtlVgUtilsDisconnectServiceLink(BtlVgChannel *channel)
{
	BtStatus status;

	BTL_FUNC_START("BtlVgUtilsDisconnectServiceLink");

	status = BtlVgUtilsCallChannelFunction(channel, HFG_DisconnectServiceLink, HSG_DisconnectServiceLink);

	if (BT_STATUS_PENDING == status)
	{
		channel->state = BTL_VG_CHANNEL_STATE_DISCONNECTING;
	}

	BTL_FUNC_END();

	return status;
}

BtStatus BtlVgUtilsAcceptIncomingChannel(BtlVgChannel *channel, BD_ADDR *bdAddr)
{
	BtStatus status;
	BtlVgChannelType type;
	BtRemoteDevice *remDev;

	BTL_FUNC_START("BtlVgUtilsAcceptIncomingChannel");

	type = BtlVgUtilsGetChannelType(channel);
	switch (type)
	{
	case BTL_VG_HANDSFREE_CHANNEL:
		remDev = ME_FindRemoteDevice(bdAddr);
		BTL_VERIFY_ERR((0 != remDev), BT_STATUS_INTERNAL_ERROR, ("No BtRemoteDevice that corresponds to bdAddr found"));
		
		status = BtlVgUtilsQueryEscoSupport(remDev);
		BTL_VERIFY_ERR((BT_STATUS_PENDING == status), status, ("BtlVgUtilsQueryEscoSupport() failed: %s", pBT_Status(status)));
		
		status = HFG_AcceptIncomingSLC(channel->c.handsfreeChannel, bdAddr);
		break;

	case BTL_VG_HEADSET_CHANNEL:
		status = HSG_AcceptIncomingSLC(channel->c.headsetChannel, bdAddr);
		break;

	case BTL_VG_UNDEFINED_CHANNEL:
	default:
		BTL_FATAL(BT_STATUS_INTERNAL_ERROR, ("Illegal BtlVgChannelType value: %x",(unsigned)type));
	}

	if (BT_STATUS_PENDING == status)
	{
		channel->state = BTL_VG_CHANNEL_STATE_CONNECTING;
	}

	BTL_FUNC_END();

	return status;
}

BtStatus BtlVgUtilsRejectIncomingChannel(BtlVgChannel *channel)
{
	BtStatus status;

	BTL_FUNC_START("BtlVgUtilsRejectIncomingChannel");

	status = BtlVgUtilsCallChannelFunction(channel, HFG_RejectIncomingSLC, HSG_RejectIncomingSLC);

	BTL_FUNC_END();

	return status;
}


BtStatus BtlVgUtilsQueryEscoSupport(BtRemoteDevice *remDev)
{
	BtStatus status;
	MeCommandToken *token;

	BTL_FUNC_START("BtlVgUtilsQueryEscoSupp");

	status = BTL_POOL_Allocate(&voiceGatewayModuleData.commandTokensPool, (void**)&token);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("BTL_POOL_Allocate failed: %s",
		pBT_Status(status)));

	token->callback = BtlVgUtilsQueryEscoCallback;
	token->p.general.in.hciCommand = HCC_READ_REMOTE_FEATURES;
	token->p.general.in.parmLen = sizeof(remDev->hciHandle);
	token->p.general.in.event = HCE_READ_REMOTE_FEATURES_COMPLETE;
	token->p.general.in.parms = (U8*)&remDev->hciHandle;

	status = ME_SendHciCommandAsync(token);

	if (BT_STATUS_PENDING != status)
	{
		BtStatus tempStatus;
		tempStatus = BTL_POOL_Free(&voiceGatewayModuleData.commandTokensPool, (void**)&token);
		BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS != tempStatus), ("BTL_POOL_Free failed"));
	}

	BTL_FUNC_END();

	return status;
}

static void BtlVgUtilsQueryEscoCallback(const BtEvent *event)
{
	BtStatus status;
	MeCommandToken *token;
	U8 *featuresMask;
	CmgrAudioParms audioParms = CMGR_AUDIO_PARMS_S3;
	BtRemoteDevice *remDev;

	BTL_FUNC_START("BtlVgUtilsQueryEscoCallback");

	BTL_VERIFY_ERR_NO_RETVAR((BTEVENT_COMMAND_COMPLETE == event->eType),
		("Illegal BtEventType: %u", (unsigned)event->eType));

	token = event->p.meToken;
	
	if(HCE_READ_REMOTE_FEATURES_COMPLETE != token->p.general.out.event)
	{
		BTL_LOG_INFO(("Unexpected HCI Event received: %u", (unsigned)token->p.general.out.event));
		status = BTL_POOL_Free(&voiceGatewayModuleData.commandTokensPool, (void**)&token);
		BTL_VERIFY_ERR_NO_RETVAR((BT_STATUS_SUCCESS == status), ("BTL_POOL_Free failed"));
		
	}

	if (BT_STATUS_SUCCESS != token->p.general.out.status)
	{
		status = BTL_POOL_Free(&voiceGatewayModuleData.commandTokensPool, (void**)&token);
		BTL_VERIFY_ERR_NO_RETVAR((BT_STATUS_SUCCESS == status), ("BTL_POOL_Free failed"));
	}

	featuresMask = &token->p.general.out.parms[BTL_VG_UTILS_FEATURES_MASK_OFFSET];


	/* Check supported audio paramaters of the remote device */
	if (BTL_VG_UTILS_EXTENDED_SCO_LINK_BIT(featuresMask))
	{
		if (BTL_VG_UTILS_EDR_ESCO_2_MBPS_MODE_BIT(featuresMask))
		{
			audioParms = CMGR_AUDIO_PARMS_S3;
			BTL_LOG_DEBUG(("BTL VG: Setting audio to S3"));
		}
		else
		{
			audioParms = CMGR_AUDIO_PARMS_S1;
			BTL_LOG_DEBUG(("BTL VG: Setting audio to S1"));
		}
	}
	else
	{
		audioParms = CMGR_AUDIO_PARMS_SCO;
		BTL_LOG_DEBUG(("BTL VG: Setting audio to SCO"));
	}
	
	remDev = ContainingRecord(token->p.general.in.parms, BtRemoteDevice , hciHandle);
	BtlVgUtilsSetAudioParms(remDev, audioParms);

	status = BTL_POOL_Free(&voiceGatewayModuleData.commandTokensPool, (void**)&token);
	BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == status), ("BTL_POOL_Free failed"));

	BTL_FUNC_END();
}

static BtStatus BtlVgUtilsCallChannelFunction(BtlVgChannel *channel,
											  BtStatus (*hfgFunc)(HfgChannel *channel),
											  BtStatus (*hsgFunc)(HsgChannel *channel))
{
	BtStatus status = BT_STATUS_FAILED;
	BtlVgChannelType type;

	BTL_FUNC_START("BtlVgUtilsCallChannelFunction");

	type = BtlVgUtilsGetChannelType(channel);
	switch (type)
	{
	case BTL_VG_HANDSFREE_CHANNEL:
		status = hfgFunc(channel->c.handsfreeChannel);
		break;

	case BTL_VG_HEADSET_CHANNEL:
		status = hsgFunc(channel->c.headsetChannel);
		break;

	case BTL_VG_UNDEFINED_CHANNEL:
	default:
		BTL_FATAL(BT_STATUS_INTERNAL_ERROR, ("Illegal BtlVgChannelType value: %x",(unsigned)type));
	}

	BTL_FUNC_END();

	return status;
}

BtStatus BtlVgUtilsCStringToBtlVgString(BtlVgString *vgString,
								const char *cString)
{
	U32 length;
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BtlVgUtilsCStringToBtlVgString");

	length = OS_StrLen(cString);
	if (BTL_VG_MAX_STR_LEN < length + 1)
	{
		status = BT_STATUS_NO_RESOURCES;
		length = BTL_VG_MAX_STR_LEN - 1;
	}

	vgString->length = length;
	OS_StrnCpy(vgString->string, cString, length+1);

	BTL_FUNC_END();

	return status;
}

BtStatus BtlVgUtilsBtlVgStringCreate(const char *cString,
								BtlVgString **vgString)
{
	BtStatus status;
	U32 nElements;
	
	BTL_FUNC_START("BtlVgUtilsBtlVgStringCreate");

	/* TODO: Remove the following traces when BtlVgString allocation and release are stable */
	status = BTL_POOL_GetNumOfAllocatedElements(&voiceGatewayModuleData.btlVgStringsPool, &nElements);
	if (BT_STATUS_SUCCESS == status)
	{
		BTL_LOG_DEBUG(("Num of allocated BtlVgStrings: %d", (int)nElements));
	}
	else
	{
		BTL_LOG_DEBUG(("BTL_POOL_GetNumOfAllocatedElements() failed: %s", pBT_Status(status)));
	}
	

	status = BTL_POOL_Allocate(&voiceGatewayModuleData.btlVgStringsPool, (void**)vgString);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status,
		("BTL_POOL_Allocate() for BtlVgString failed: %s", pBT_Status(status)));

	status = BtlVgUtilsCStringToBtlVgString(*vgString, cString);
	if (BT_STATUS_SUCCESS != status)
	{
		BtStatus poolFreeStatus;

		poolFreeStatus = BTL_POOL_Free(&voiceGatewayModuleData.btlVgStringsPool, (void**)vgString);
		BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == poolFreeStatus),
			("BTL_POOL_Free() for BtlVgString faile: %s", pBT_Status(poolFreeStatus)));

		BTL_ERR(status, ("BtlVgUtilsCStringToBtlVgString() failed: %s", pBT_Status(status)));
	}

	BTL_FUNC_END();

	return status;
}

static void BtlVgUtilsSetAudioParms(const BtRemoteDevice *remDev,
								   CmgrAudioParms audioParms)
{
	BtlVgChannel *channel;
	U32 nChannels;

	BTL_FUNC_START("BtlVgUtilsSetAudioParms");

	BTL_VERIFY_ERR_NO_RETVAR((0 != remDev), ("remDev argument is 0"));

	channel = voiceGatewayModuleData.context.channels;

	for (nChannels = BTL_VG_MAXIMUM_NUMBER_OF_CHANNELS; nChannels > 0; --nChannels)
	{
		CmgrHandler *handler = BtlVgUtilsGetCmgrHandler(channel);

		BTL_VERIFY_ERR_NO_RETVAR((0 != handler), ("BtlVgUtilsGetCmgrHandler() returned 0"));
		
		if (remDev == handler->remDev)
		{
			channel->audioParms = audioParms;
		}
		++channel;
	}

	BTL_FUNC_END();
}

BOOL BtlVgUtilsIsInbandRingEnabled(BtlVgChannel *channel)
{
	BOOL inbandEnabled = FALSE;

	BTL_FUNC_START("BtlVgUtilsIsInbandRingEnabled");

	if (BTL_VG_HANDSFREE_CHANNEL == BtlVgUtilsGetChannelType(channel))
	{
		inbandEnabled = HFG_IsInbandRingEnabled(channel->c.handsfreeChannel);
	}
	else
	{
		inbandEnabled = FALSE;
	}

	BTL_FUNC_END();

	return inbandEnabled;
}

BtStatus BtlVgUtilsClearAtResult(AtResults *res)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtStatus poolFreeStatus;
	const char *str[BTL_VG_MAX_BTL_VG_STRINGS_TO_FREE+1] = {0};
	BtlVgString *vgStr = 0;
	I32 idx;

	BTL_FUNC_START("BtlVgSmClearAtResult");

	switch (res->type)
	{
	case AT_CALL_WAIT_NOTIFY:
		str[0] = res->p.hf.wait.number;
		break;

	case AT_CALL_ID:
		str[0] = res->p.hf.callId.number;
		break;

	case AT_VOICE_TAG:
		str[0] = res->p.hf.voiceTag.number;
		break;

	case AT_SUBSCRIBER_NUM:
		str[0] = res->p.hf.subscribeNum.number;
		break;

	case AT_LIST_CURRENT_CALLS:
		str[0] = res->p.hf.currentCalls.number;
		break;

	case AT_NETWORK_OPERATOR | AT_READ:
		str[0] = res->p.hf.networkOper_read.oper;
		break;
		
	case AT_RAW:
		str[0] = (const char*)res->p.data;
		break;

	case AT_READ_PHONEBOOK_ENTRY:
		str[0] = res->p.pb.read.number;
		str[1] = res->p.pb.read.text;
		break;

	case AT_FIND_PHONEBOOK_ENTRY:
		str[0] = res->p.pb.read.number;
		str[1] = res->p.pb.read.text;
		break;

	case AT_READ_PHONEBOOK_ENTRY | AT_TEST:
		str[0] = res->p.pb.read_test.range;

	default:
		break;
	}

	for (idx = 0; 0 != str[idx] && idx < BTL_VG_MAX_BTL_VG_STRINGS_TO_FREE; ++idx)
	{
		U32 nElements;
		BtStatus tempStatus;
	
		vgStr = ContainingRecord(str[idx], BtlVgString, string);
		status = BTL_POOL_Free(&voiceGatewayModuleData.btlVgStringsPool, (void**)&vgStr);
		BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == status),
			("BTL_POOL_Free() on BtlVgString failed: %s", pBT_Status(status)));

		/* TODO: Remove the following traces when BtlVgString allocation and release are stable */
		tempStatus = BTL_POOL_GetNumOfAllocatedElements(&voiceGatewayModuleData.btlVgStringsPool, &nElements);
		if (BT_STATUS_SUCCESS == tempStatus)
		{
			BTL_LOG_DEBUG(("Num of allocated BtlVgStrings: %d", (int)nElements));
		}
		else
		{
			BTL_LOG_DEBUG(("BTL_POOL_GetNumOfAllocatedElements failed: %s", pBT_Status(tempStatus)));
		}
	}
	BTL_VERIFY_ERR_NORET((idx < BTL_VG_MAX_BTL_VG_STRINGS_TO_FREE),
		("idx reached BTL_VG_MAX_BTL_VG_STRINGS_TO_FREE"));

	poolFreeStatus = BTL_POOL_Free(&voiceGatewayModuleData.responsesPool, (void**)&res);
	BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == poolFreeStatus),
		("BTL_POOL_Free() on AtResults failed: %s", pBT_Status(poolFreeStatus)));

	if (BT_STATUS_SUCCESS != poolFreeStatus || BT_STATUS_SUCCESS != status)
	{
		status = BT_STATUS_INTERNAL_ERROR;
	}

	BTL_FUNC_END();

	return status;
}


static BtStatus BtlVgUtilsRegisterServices(void)
{
	BtStatus status = BT_STATUS_FAILED;
	U32 nChannels;
	HfgChannel *hfgChannel;
	HsgChannel *hsgChannel;

	BTL_FUNC_START("BtlVgUtilsRegisterServices");

	hfgChannel = voiceGatewayModuleData.context.hfgChannels;
	for (nChannels = BTL_HFAG_MAX_CHANNELS; nChannels > 0; --nChannels)
	{
		status = HFG_Register(hfgChannel, BtlVgUtilsHfgCallback);
		BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR,
			("HFG_Register() failed: %s", pBT_Status(status)));
		++hfgChannel;
	}

	hsgChannel = voiceGatewayModuleData.context.hsgChannels;
	for (nChannels = BTL_HSAG_MAX_CHANNELS; nChannels > 0; --nChannels)
	{
		status = HSG_Register(hsgChannel, BtlVgUtilsHsgCallback);
		BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR,
			("HSG_Register() failed: %s", pBT_Status(status)));
		++hsgChannel;
	}

	BTL_FUNC_END();

	return status;
}

BtStatus BtlVgUtilsDeregisterServices(void)
{
	BtStatus status = BT_STATUS_SUCCESS;
	U32 nChannels;
	HfgChannel *hfgChannel;
	HsgChannel *hsgChannel;

	BTL_FUNC_START("BtlVgUtilsDeregisterServices");

	hfgChannel = voiceGatewayModuleData.context.hfgChannels;
	for (nChannels = BTL_HFAG_MAX_CHANNELS; nChannels > 0; --nChannels)
	{
		status = HFG_Deregister(hfgChannel);
		BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR,
			("HFG_Deregister() failed: %s", pBT_Status(status)));
		++hfgChannel;
	}

	hsgChannel = voiceGatewayModuleData.context.hsgChannels;
	for (nChannels = BTL_HSAG_MAX_CHANNELS; nChannels > 0; --nChannels)
	{
		status = HSG_Deregister(hsgChannel);
		BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR,
			("HSG_Deregister() failed: %s", pBT_Status(status)));
		++hsgChannel;
	}

	BTL_FUNC_END();

	return status;
}

static BtlVgChannel* BtlVgUtilsContainingVgChannel(const void *channel, 
													BtlVgChannelType type)
{
	BtlVgChannel *vgChannel = 0;
	U32 nChannels;

	BTL_FUNC_START("BtlVgUtilsContainingVgChannel");

	if (BTL_VG_HANDSFREE_CHANNEL != type && BTL_VG_HEADSET_CHANNEL != type)
	{
		BTL_FATAL_SET_RETVAR((vgChannel = 0), ("Illegal BtlVgChannelType value: %d",(int)type));
	}

	vgChannel = voiceGatewayModuleData.context.channels;
	for (nChannels = BTL_VG_MAXIMUM_NUMBER_OF_CHANNELS; nChannels > 0; --nChannels)
	{
		
		if (BTL_VG_HANDSFREE_CHANNEL == type && vgChannel->c.handsfreeChannel == channel)
		{
			break;
		}
		else if (BTL_VG_HEADSET_CHANNEL == type && vgChannel->c.headsetChannel == channel)
		{
			break;
		}
		++vgChannel;
	}
	BTL_VERIFY_FATAL_SET_RETVAR((0 != nChannels), (vgChannel = 0), ("No containning BtlVgChannel found"));

	BTL_FUNC_END();

	return vgChannel;
}

static HfgChannel* BtlVgUtilsGetFreeHfgChannel(void)
{
	U32 nChannels;
	HfgChannel *channel = 0;

	BTL_FUNC_START("BtlVgUtilsGetFreeHfgChannel");

	channel = voiceGatewayModuleData.context.hfgChannels;
	for (nChannels = BTL_HFAG_MAX_CHANNELS; nChannels > 0; --nChannels)
	{
		if (HFG_STATE_CLOSED == channel->state)
		{
			break;
		}
		++channel;
	}

	BTL_VERIFY_ERR_SET_RETVAR((0 != channel), (channel = 0), ("No free HfgChannel"));

	BTL_FUNC_END();

	return channel;
}

static HsgChannel* BtlVgUtilsGetFreeHsgChannel(void)
{
	U32 nChannels;
	HsgChannel *channel = 0;

	BTL_FUNC_START("BtlVgUtilsGetFreeHsgChannel");

	channel = voiceGatewayModuleData.context.hsgChannels;
	for (nChannels = BTL_HFAG_MAX_CHANNELS; nChannels > 0; --nChannels)
	{
		if (HSG_STATE_CLOSED == channel->state)
		{
			break;
		}
		++channel;
	}

	BTL_VERIFY_ERR_SET_RETVAR((0 != channel), (channel = 0), ("No free HsgChannel"));

	BTL_FUNC_END();

	return channel;
}

static BtStatus BtlVgUtilsBtlNotificationsCb(BtlModuleNotificationType notificationType)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START(("BtlVgUtilsBtlNotificationsCb"));

	BTL_LOG_DEBUG(("BTL_VG: Notification Type: %d", (int)notificationType));	
	
	switch (notificationType)
	{
	case BTL_MODULE_NOTIFICATION_RADIO_ON:
		break;

	case BTL_MODULE_NOTIFICATION_RADIO_OFF:
		status = BtlVgUtilsProcessRadioOffNotification();
		break;

	default:
		BTL_ERR(BT_STATUS_INTERNAL_ERROR, ("BTL_VG: Invalid notification (%d)", (int)notificationType));
	}

	BTL_FUNC_END();

	return status;
}

static BtStatus BtlVgUtilsProcessRadioOffNotification(void)
{
	BtStatus status;
	BtlVgContext *context;
	
	BTL_FUNC_START(("BtlVgUtilsProcessRadioOffNotification"));

	voiceGatewayModuleData.isRadioOffNotified = TRUE;
	context = &voiceGatewayModuleData.context;

	/*	Halt the handover operation if exists;
	 *	Both connections will be terminated.
	 */
	context->handover = FALSE;
	context->handoverSource = 0;
	context->handoverTarget = 0;
	context->handoverRequest = 0;

	switch (context->state)
	{
	case BTL_VG_CONTEXT_STATE_DESTROYED:
	case BTL_VG_CONTEXT_STATE_DISABLED:
		status = BT_STATUS_SUCCESS;
		break;

	case BTL_VG_CONTEXT_STATE_ENABLED:
		status = BtlVgUtilsRemoveAllConnections();
		break;

	case BTL_VG_CONTEXT_STATE_DISABLING:
		status = BT_STATUS_PENDING;
		break;

	default:
		status = BT_STATUS_INTERNAL_ERROR;
		BTL_LOG_FATAL(("BTL VG: Illegal BtlVgContext state: %d", context->state));
		break;
	}

	if (BT_STATUS_PENDING != status)
	{
		voiceGatewayModuleData.isRadioOffNotified = FALSE;		
	}

	BTL_FUNC_END();

	return status;
}

static BtStatus BtlVgUtilsRemoveAllConnections(void)
{
	U32	nChannels;
	BtlVgChannel *channel;
	BtStatus status = BT_STATUS_SUCCESS;
	BtStatus discStatus;

	BTL_FUNC_START(("BtlVgUtilsRemoveAllConnections"));

	channel = voiceGatewayModuleData.context.channels;

	for (nChannels = BTL_VG_MAXIMUM_NUMBER_OF_CHANNELS; nChannels > 0; --nChannels)
	{
		switch (channel->state)
		{
		case BTL_VG_CHANNEL_STATE_CONNECTED:
			/* BtlVgUtilsDisconnectServiceLink also handles the audio link. */
			discStatus = BtlVgUtilsDisconnectServiceLink(channel);
			if (BT_STATUS_PENDING == discStatus)
			{
				status = BT_STATUS_PENDING;			
			}
			else
			{
				/*	Since the channel state indicates that an SLC exist,
				 *	every status other then pending is erroneous.
				 */
				BTL_LOG_ERROR(("BtlVgUtilsDisconnectServiceLink() returned: %s", pBT_Status(discStatus)));
			}
			break;

		case BTL_VG_CHANNEL_STATE_AUDIO_CONNECTED:
			discStatus = BtlVgUtilsDisconnectAudio(channel);
			if (BT_STATUS_PENDING == discStatus)
			{
				status = BT_STATUS_PENDING;			
			}
			else
			{
				/*	Since the channel state indicates that audio is connected,
				 *	every status other then pending is erroneous.
				 */
				BTL_LOG_ERROR(("BtlVgUtilsDisconnectAudio() returned: %s", pBT_Status(discStatus)));
			}
			break;
		
		case BTL_VG_CHANNEL_STATE_CHECKING_SUPPORTED_SERVICES:
		case BTL_VG_CHANNEL_STATE_CONNECTING:
		case BTL_VG_CHANNEL_STATE_DISCONNECTING:
		case BTL_VG_CHANNEL_STATE_CONNECTING_AUDIO:
		case BTL_VG_CHANNEL_STATE_DISCONNECTING_AUDIO:
			/*	If in the middle of establishing or disconnecting SLC or AUDIO let
			 *	the operation finish, those states will be treated later.
			 */
			status = BT_STATUS_PENDING;
			break;

		case BTL_VG_CHANNEL_STATE_IDLE:
			break;		
			
		default:
			BTL_LOG_FATAL(("BTL VG: Illegal BtlVgChannel state: %d", (int)channel->state));
			break;

		}
			
		++channel;
	}

	BTL_FUNC_END();

	return status;
}

#if (0)

BtStatus BtlVgUtilsSetFmOverBt(BtlVgChannel *channel, BOOL enable)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtRemoteDevice *remDev;
	U16 connectionHandle;
	MeCommandToken *token;
	CmgrHandler *cmgrHandler;
    
	static U8 vendorSpecificParms[5] =
	{
		0x01, 0x01, 	/* handle  */
		0x01,		/* Enable FoB */
		0x00,		/* PCM IO while in FoB  */
		0x00 		/* I2S IO while in FoB */
	};

	BTL_FUNC_START("BtlVgUtilsSetFmOverBt");

	BTL_VERIFY_ERR(((BTL_VG_HANDSFREE_CHANNEL == channel->type) || (BTL_VG_HEADSET_CHANNEL == channel->type)), BT_STATUS_INVALID_PARM, ("BtlVgUtilsSetFmOverBt(): Invalid channel type"));

	cmgrHandler = BtlVgUtilsGetCmgrHandler(channel);
    
	BTL_VERIFY_ERR((0 != cmgrHandler), BT_STATUS_INTERNAL_ERROR, ("BtlVgUtilsGetCmgrHandler() returned 0"));
 	BTL_VERIFY_ERR((0 != cmgrHandler->bdc), BT_STATUS_NO_CONNECTION, ("BtlVgUtilsSetFmOverBt(): No connection"));

 	remDev = cmgrHandler->bdc->link;

 	BTL_VERIFY_ERR((0 != remDev), BT_STATUS_INTERNAL_ERROR, ("BtlVgUtilsSetFmOverBt(): NULL remDev"));
   
	connectionHandle = ME_GetHciConnectionHandle(remDev);

	/* Add SCO handle to ACL handle */
	connectionHandle |=  0x0100; 

	StoreLE16(&vendorSpecificParms[0], connectionHandle);

	if (TRUE == enable)
	{
		vendorSpecificParms[2] = 0x01;
	}
	else
	{
		vendorSpecificParms[2] = 0x00;
	}
	
	status = BTL_POOL_Allocate(&voiceGatewayModuleData.commandTokensPool, (void**)&token);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status),
                                    status,
                                    ("BTL_POOL_Allocate failed: %s", pBT_Status(status)));

	OS_MemSet((U8*) token, 0, sizeof(MeCommandToken));
	
	token->callback = BtlVgUtilsSetFmOverBtCallback;
	token->p.general.in.hciCommand = FM_OVER_BT_MODE;
	token->p.general.in.parmLen = sizeof(vendorSpecificParms);
	token->p.general.in.parms = vendorSpecificParms;
	token->p.general.in.event = HCE_COMMAND_COMPLETE;

	status = ME_SendHciCommandAsync(token);

	if (BT_STATUS_PENDING != status)
	{
		BtStatus tempStatus;
		tempStatus = BTL_POOL_Free(&voiceGatewayModuleData.commandTokensPool, (void**)&token);
		BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS != tempStatus), ("BTL_POOL_Free failed"));
	}

	BTL_FUNC_END();

	return status;
}

static void BtlVgUtilsSetFmOverBtCallback(const BtEvent *event)
{
	MeCommandToken *token;
 	BtStatus	status = BT_STATUS_SUCCESS;
   
	BTL_FUNC_START("BtlVgUtilsSetFmOverBtCallback");
	
	BTL_VERIFY_ERR_NO_RETVAR((BTEVENT_COMMAND_COMPLETE == event->eType),
		("Illegal BtEventType: %u", (unsigned)event->eType));
    
	token = event->p.meToken;
    
	if (BT_STATUS_SUCCESS != token->p.general.out.status)
	{
		status = BTL_POOL_Free(&voiceGatewayModuleData.commandTokensPool, (void**)&token);
		BTL_VERIFY_ERR_NO_RETVAR((BT_STATUS_SUCCESS == status), ("BTL_POOL_Free failed"));
	}
    
	if ((HCE_COMMAND_COMPLETE != token->p.general.out.event) &&
	    (HCE_COMMAND_STATUS != token->p.general.out.event))
	{
		BTL_LOG_ERROR(("HCI Command Failed, HCI Event: %d",
						token->p.general.out.event));
	}
	else if (HCE_COMMAND_COMPLETE == token->p.general.out.event)
	{
		if (event->p.meToken->p.general.out.parms[3] != 0)
		{
			BTL_LOG_ERROR(("HCI Command Failed, status: %x",
							token->p.general.out.parms[3]));
		}
		else
		{
			BTL_LOG_ERROR(("HCI Command sent, successfully"));
		}
	}
    
	status = BTL_POOL_Free(&voiceGatewayModuleData.commandTokensPool, (void**)&token);
	BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == status), ("BTL_POOL_Free failed"));
    
	BTL_FUNC_END();

	return;
}

#endif /* BTL_CONFIG_FM_STACK == BTL_CONFIG_ENABLED */

const char *BtlVgUtilsHfgEventName(HfgEvent event)
{
    const char *eventName;

    switch(event)
    {
        case HFG_EVENT_SERVICE_CONNECT_REQ:
            eventName = "HFG_EVENT_SERVICE_CONNECT_REQ";
            break;
        case HFG_EVENT_SERVICE_CONNECTED:
            eventName = "HFG_EVENT_SERVICE_CONNECTED";
            break;
        case HFG_EVENT_SERVICE_DISCONNECTED:
            eventName = "HFG_EVENT_SERVICE_DISCONNECTED";
            break;
        case HFG_EVENT_AUDIO_CONNECTED:
            eventName = "HFG_EVENT_AUDIO_CONNECTED";
            break;
        case HFG_EVENT_AUDIO_DISCONNECTED:
            eventName = "HFG_EVENT_AUDIO_DISCONNECTED";
            break;
        case HFG_EVENT_HANDSFREE_FEATURES:
            eventName = "HFG_EVENT_HANDSFREE_FEATURES";
            break;
#if BT_SCO_HCI_DATA == XA_ENABLED
        case HFG_EVENT_AUDIO_DATA:
            eventName = "HFG_EVENT_AUDIO_DATA";
            break;
        case HFG_EVENT_AUDIO_DATA_SENT:
            eventName = "HFG_EVENT_AUDIO_DATA_SENT";
            break;
#endif /* BT_SCO_HCI_DATA == XA_ENABLED */
        case HFG_EVENT_ANSWER_CALL:
            eventName = "HFG_EVENT_ANSWER_CALL";
            break;
        case HFG_EVENT_DIAL_NUMBER:
            eventName = "HFG_EVENT_DIAL_NUMBER";
            break;
        case HFG_EVENT_MEMORY_DIAL:
            eventName = "HFG_EVENT_MEMORY_DIAL";
            break;
        case HFG_EVENT_REDIAL:
            eventName = "HFG_EVENT_REDIAL";
            break;
        case HFG_EVENT_CALL_HOLD:
            eventName = "HFG_EVENT_CALL_HOLD";
            break;
#if HFG_USE_RESP_HOLD == XA_ENABLED
        case HFG_QUERY_RESPONSE_HOLD:
            eventName = "HFG_QUERY_RESPONSE_HOLD";
            break;
        case HFG_RESPONSE_HOLD:
            eventName = "HFG_RESPONSE_HOLD";
            break;
#endif /* HFG_USE_RESP_HOLD == XA_ENABLED */
        case HFG_EVENT_HANGUP:
            eventName = "HFG_EVENT_HANGUP";
            break;
        case HFG_EVENT_LIST_CURRENT_CALLS:
            eventName = "HFG_EVENT_LIST_CURRENT_CALLS";
            break;
        case HFG_EVENT_ENABLE_CALLER_ID:
            eventName = "HFG_EVENT_ENABLE_CALLER_ID";
            break;
        case HFG_EVENT_ENABLE_CALL_WAITING:
            eventName = "HFG_EVENT_ENABLE_CALL_WAITING";
            break;
        case HFG_EVENT_GENERATE_DTMF:
            eventName = "HFG_EVENT_GENERATE_DTMF";
            break;
        case HFG_EVENT_GET_LAST_VOICE_TAG:
            eventName = "HFG_EVENT_GET_LAST_VOICE_TAG";
            break;
        case HFG_EVENT_ENABLE_VOICE_RECOGNITION:
            eventName = "HFG_EVENT_ENABLE_VOICE_RECOGNITION";
            break;
        case HFG_EVENT_DISABLE_NREC:
            eventName = "HFG_EVENT_DISABLE_NREC";
            break;
        case HFG_EVENT_REPORT_MIC_VOLUME:
            eventName = "HFG_EVENT_REPORT_MIC_VOLUME";
            break;
        case HFG_EVENT_REPORT_SPK_VOLUME:
            eventName = "HFG_EVENT_REPORT_SPK_VOLUME";
            break;
        case HFG_EVENT_QUERY_NETWORK_OPERATOR:
            eventName = "HFG_EVENT_QUERY_NETWORK_OPERATOR";
            break;
        case HFG_EVENT_QUERY_SUBSCRIBER_NUMBER:
            eventName = "HFG_EVENT_QUERY_SUBSCRIBER_NUMBER";
            break;
        case HFG_EVENT_ENABLE_EXTENDED_ERRORS:
            eventName = "HFG_EVENT_ENABLE_EXTENDED_ERRORS";
            break;
        case HFG_EVENT_AT_COMMAND_DATA:
            eventName = "HFG_EVENT_AT_COMMAND_DATA";
            break;
        case HFG_EVENT_RESPONSE_COMPLETE:
            eventName = "HFG_EVENT_RESPONSE_COMPLETE";
            break;
        case HFG_EVENT_AUDIO_CONNECT_REQ:
            eventName = "HFG_EVENT_AUDIO_CONNECT_REQ";
            break;
#if AT_PHONEBOOK == XA_ENABLED
        case HFG_EVENT_SUPPORTED_PHONEBOOK_LIST:
            eventName = "HFG_EVENT_SUPPORTED_PHONEBOOK_LIST";
            break;
        case HFG_EVENT_SELECTED_PHONEBOOK_INFO:
            eventName = "HFG_EVENT_SELECTED_PHONEBOOK_INFO";
            break;
        case HFG_EVENT_READ_PHONEBOOK_ENTRIES_RANGE:
            eventName = "HFG_EVENT_READ_PHONEBOOK_ENTRIES_RANGE";
            break;
        case HFG_EVENT_READ_PHONEBOOK_ENTRIES:
            eventName = "HFG_EVENT_READ_PHONEBOOK_ENTRIES";
            break;
        case HFG_EVENT_FIND_PHONEBOOK_ENTRIES:
            eventName = "HFG_EVENT_FIND_PHONEBOOK_ENTRIES";
            break;
        case HFG_EVENT_FIND_PHONEBOOK_ENTRIES_SIZE:
            eventName = "HFG_EVENT_FIND_PHONEBOOK_ENTRIES_SIZE";
            break;
        case HFG_EVENT_WRITE_PHONEBOOK_ENTRY_LOCATION_RANGE:
            eventName = "HFG_EVENT_WRITE_PHONEBOOK_ENTRY_LOCATION_RANGE";
            break;
        case HFG_EVENT_SELECTED_CHAR_SET:
            eventName = "HFG_EVENT_SELECTED_CHAR_SET";
            break;
        case HFG_EVENT_SUPPORTED_CHAR_SET:
            eventName = "HFG_EVENT_SUPPORTED_CHAR_SET";
            break;
        case HFG_EVENT_SET_CHAR_SET:
            eventName = "HFG_EVENT_SET_CHAR_SET";
            break;
        case HFG_EVENT_SET_PHONEBOOK:
            eventName = "HFG_EVENT_SET_PHONEBOOK";
            break;
#endif /* AT_PHONEBOOK == XA_ENABLED */
        default:
            eventName = "UNKNOWN";
            break;
    }

    return (eventName);
}

const char *BtlVgUtilsHsgEventName(HsgEvent event)
{
    const char *eventName;

    switch(event)
    {
        case HSG_EVENT_SERVICE_CONNECT_REQ:
            eventName = "HSG_EVENT_SERVICE_CONNECT_REQ";
            break;
        case HSG_EVENT_SERVICE_CONNECTED:
            eventName = "HSG_EVENT_SERVICE_CONNECTED";
            break;
        case HSG_EVENT_SERVICE_DISCONNECTED:
            eventName = "HSG_EVENT_SERVICE_DISCONNECTED";
            break;
        case HSG_EVENT_AUDIO_CONNECT_REQ:
            eventName = "HSG_EVENT_AUDIO_CONNECT_REQ";
            break;
        case HSG_EVENT_AUDIO_CONNECTED:
            eventName = "HSG_EVENT_AUDIO_CONNECTED";
            break;
        case HSG_EVENT_AUDIO_DISCONNECTED:
            eventName = "HSG_EVENT_AUDIO_DISCONNECTED";
            break;
        case HSG_EVENT_BUTTON_PRESSED:
            eventName = "HSG_EVENT_BUTTON_PRESSED";
            break;
        case HSG_EVENT_REPORT_MIC_VOLUME:
            eventName = "HSG_EVENT_REPORT_MIC_VOLUME";
            break;
        case HSG_EVENT_REPORT_SPK_VOLUME:
            eventName = "HSG_EVENT_REPORT_SPK_VOLUME";
            break;
        case HSG_EVENT_AT_COMMAND_DATA:
            eventName = "HSG_EVENT_AT_COMMAND_DATA";
            break;
        case HSG_EVENT_RESPONSE_COMPLETE:
            eventName = "HSG_EVENT_RESPONSE_COMPLETE";
            break;
#if BT_SCO_HCI_DATA == XA_ENABLED
        case HSG_EVENT_AUDIO_DATA:
            eventName = "HSG_EVENT_AUDIO_DATA";
            break;
        case HSG_EVENT_AUDIO_DATA_SENT:
            eventName = "HSG_EVENT_AUDIO_DATA_SENT";
            break;
#endif /* BT_SCO_HCI_DATA == XA_ENABLED */
        default:
            eventName = "UNKNOWN";
            break;
    }
    
    return (eventName);
}

const char *BtlVgUtilsBthalMcEventName(BthalMcEventType event)
{
    const char *eventName;

    switch(event)
    {
        case BTHAL_MC_EVENT_OK:
            eventName = "BTHAL_MC_EVENT_OK";
            break;
        case BTHAL_MC_EVENT_ERROR:
            eventName = "BTHAL_MC_EVENT_ERROR";
            break;
        case BTHAL_MC_EVENT_NO_CARRIER:
            eventName = "BTHAL_MC_EVENT_NO_CARRIER";
            break;
        case BTHAL_MC_EVENT_BUSY:
            eventName = "BTHAL_MC_EVENT_BUSY";
            break;
        case BTHAL_MC_EVENT_NO_ANSWER:
            eventName = "BTHAL_MC_EVENT_NO_ANSWER";
            break;
        case BTHAL_MC_EVENT_DELAYED:
            eventName = "BTHAL_MC_EVENT_DELAYED";
            break;
        case BTHAL_MC_EVENT_BLACKLISTED:
            eventName = "BTHAL_MC_EVENT_BLACKLISTED";
            break;
        case BTHAL_MC_EVENT_RING:
            eventName = "BTHAL_MC_EVENT_RING";
            break;
        case BTHAL_MC_EVENT_EXTENDED_ERROR:
            eventName = "BTHAL_MC_EVENT_EXTENDED_ERROR";
            break;
        case BTHAL_MC_EVENT_CALL_WAIT_NOTIFY:
            eventName = "BTHAL_MC_EVENT_CALL_WAIT_NOTIFY";
            break;
        case BTHAL_MC_EVENT_CURRENT_CALLS_LIST_RESPONSE:
            eventName = "BTHAL_MC_EVENT_CURRENT_CALLS_LIST_RESPONSE";
            break;
        case BTHAL_MC_EVENT_CALLING_LINE_ID_NOTIFY:
            eventName = "BTHAL_MC_EVENT_CALLING_LINE_ID_NOTIFY";
            break;
         case BTHAL_MC_EVENT_INDICATOR_EVENT:
            eventName = "BTHAL_MC_EVENT_INDICATOR_EVENT";
            break;
         case BTHAL_MC_EVENT_SUBSCRIBER_NUMBER_RESPONSE:
            eventName = "BTHAL_MC_EVENT_SUBSCRIBER_NUMBER_RESPONSE";
            break;
         case BTHAL_MC_EVENT_CALL_HOLD_AND_MULTIPARTY_OPTIONS:
            eventName = "BTHAL_MC_EVENT_CALL_HOLD_AND_MULTIPARTY_OPTIONS";
            break;
         case BTHAL_MC_EVENT_CURRENT_INDICATORS_STATUS:
            eventName = "BTHAL_MC_EVENT_CURRENT_INDICATORS_STATUS";
            break;
         case BTHAL_MC_EVENT_INDICATORS_RANGE:
            eventName = "BTHAL_MC_EVENT_INDICATORS_RANGE";
            break;
         case BTHAL_MC_EVENT_NETWORK_OPERATOR_RESPONSE:
            eventName = "BTHAL_MC_EVENT_NETWORK_OPERATOR_RESPONSE";
            break;
         case BTHAL_MC_EVENT_READ_RESPONSE_AND_HOLD_RES:
            eventName = "BTHAL_MC_EVENT_READ_RESPONSE_AND_HOLD_RES";
            break;
         case BTHAL_MC_EVENT_SET_RESPONSE_AND_HOLD_RES:
            eventName = "BTHAL_MC_EVENT_SET_RESPONSE_AND_HOLD_RES";
            break;
         case BTHAL_MC_EVENT_MODEM_LINK_ESTABLISHED:
            eventName = "BTHAL_MC_EVENT_MODEM_LINK_ESTABLISHED";
            break;
         case BTHAL_MC_EVENT_MODEM_LINK_RELEASED:
            eventName = "BTHAL_MC_EVENT_MODEM_LINK_RELEASED";
            break;
         case BTHAL_MC_EVENT_SELECTED_PHONEBOOK:
            eventName = "BTHAL_MC_EVENT_SELECTED_PHONEBOOK";
            break;
         case BTHAL_MC_EVENT_SUPPORTED_PHONEBOOKS:
            eventName = "BTHAL_MC_EVENT_SUPPORTED_PHONEBOOKS";
            break;
         case BTHAL_MC_EVENT_READ_PHONEBOOK_ENTRIES_RES:
            eventName = "BTHAL_MC_EVENT_READ_PHONEBOOK_ENTRIES_RES";
            break;
         case BTHAL_MC_EVENT_TST_READ_PHONEBOOK_ENTRIES_RES:
            eventName = "BTHAL_MC_EVENT_TST_READ_PHONEBOOK_ENTRIES_RES";
            break;
         case BTHAL_MC_EVENT_FIND_PHONEBOOK_ENTRIES_RES:
            eventName = "BTHAL_MC_EVENT_FIND_PHONEBOOK_ENTRIES_RES";
            break;
         case BTHAL_MC_EVENT_SELECTED_CHAR_SET:
            eventName = "BTHAL_MC_EVENT_SELECTED_CHAR_SET";
            break;
       default:
            eventName = "UNKNOWN";
            break;
    }
    
    return (eventName);
}

const char *BtlVgUtilsBthalVcEventName(BthalVcEventType event)
{
    const char *eventName;

    switch(event)
    {
        case BTHAL_VC_EVENT_VOICE_PATHS_CHANGED:
            eventName = "BTHAL_VC_EVENT_VOICE_PATHS_CHANGED";
            break;
        case BTHAL_VC_EVENT_VOICE_RECOGNITION_STATUS:
            eventName = "BTHAL_VC_EVENT_VOICE_RECOGNITION_STATUS";
            break;
        case BTHAL_VC_EVENT_NOISE_REDUCTION_ECHO_CANCELLING_STATUS:
            eventName = "BTHAL_VC_EVENT_NOISE_REDUCTION_ECHO_CANCELLING_STATUS";
            break;
        case BTHAL_VC_EVENT_LINK_ESTABLISHED:
            eventName = "BTHAL_VC_EVENT_LINK_ESTABLISHED";
            break;
        case BTHAL_VC_EVENT_LINK_RELEASED:
            eventName = "BTHAL_VC_EVENT_LINK_RELEASED";
            break;
        default:
            eventName = "UNKNOWN";
            break;
    }
    
    return (eventName);
}

U16 BtlVgUtilsIncrementCmdRspId(U16 id)
{
    U16 incrementedId = (U16)(id + 1);

    /* 0 is invalid ID */
    if (0 == incrementedId)
    {
        incrementedId = 1;
    }
    
    return (incrementedId);
}


#endif /*BTL_CONFIG_VG == BTL_CONFIG_ENABLED*/


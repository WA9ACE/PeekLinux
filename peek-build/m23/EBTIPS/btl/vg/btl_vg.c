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
*   FILE NAME:		btl_vg.c
*
*   DESCRIPTION:	This file implements the API of the BTL Voice Gateway.
*
*   AUTHOR:			Itay Klein
*
\*******************************************************************************/


#include "btl_config.h"
#include "btl_defs.h"
#include "btl_vg.h"

BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_VG);

#if BTL_CONFIG_VG == BTL_CONFIG_ENABLED

/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/

#include "debug.h"
#include "btl_commoni.h"
#include "btl_vgi.h"


/********************************************************************************
 *
 * Internal functions declarations
 *
 *******************************************************************************/

static BtStatus BtlVgUtilsSendVolume(BtlVgChannelId channelId,
									 U32 volume,
									 AtCommand cmd);

/********************************************************************************
 *
 * Function definitions
 *
 *******************************************************************************/



BtStatus BTL_VG_Init(void)
{
	BtStatus status;

	BTL_FUNC_START_AND_LOCK_COMMON("BTL_VG_Init");

	BTL_VERIFY_ERR((BTL_VG_MODULE_STATE_NOT_INITIALIZED == voiceGatewayModuleData.state), BT_STATUS_FAILED, 
		("Module initialized"));
	
	status = BtlVgUtilsInit();

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_VG_Deinit(void)
{
	BtStatus status;

	BTL_FUNC_START_AND_LOCK_COMMON("BTL_VG_Deinit");
	
	BTL_VERIFY_ERR((BTL_VG_MODULE_STATE_INITIALIZED == voiceGatewayModuleData.state),
		BT_STATUS_FAILED, ("Module not initialized"));

	BTL_VERIFY_ERR((BTL_VG_CONTEXT_STATE_DESTROYED == voiceGatewayModuleData.context.state),
		BT_STATUS_FAILED, ("Context is active"));

	status = BtlVgUtilsDeinit();

	BTL_FUNC_END_AND_UNLOCK();
	
	return status;
}

BtStatus BTL_VG_Create(BtlAppHandle *handle,
					   BtlVgCallback callback,
					   const BtSecurityLevel *secLevel,
					   BtlVgContext **context)
{
	BtStatus status;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_VG_Create");

	UNUSED_PARAMETER(context);

	BTL_VERIFY_ERR((0 != callback), BT_STATUS_INVALID_PARM, ("null callback"));

	BTL_VERIFY_ERR((BTL_VG_MODULE_STATE_INITIALIZED == voiceGatewayModuleData.state),
		BT_STATUS_FAILED, ("BTL_VG_Init() was not called"));

	BTL_VERIFY_ERR((BTL_VG_CONTEXT_STATE_DESTROYED == voiceGatewayModuleData.context.state),
		BT_STATUS_INVALID_PARM, ("Invalid context state"));

	status = BtlVgUtilsCreate(handle, callback, secLevel);

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_VG_Destroy(BtlVgContext **context)
{
	BtStatus status;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_VG_Destroy");

	UNUSED_PARAMETER(context);

	BTL_VERIFY_ERR((BTL_VG_CONTEXT_STATE_DISABLED == voiceGatewayModuleData.context.state),
		BT_STATUS_INVALID_PARM, ("Invalid context state"));

	status = BtlVgUtilsDestroy();

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_VG_Enable(BtlVgContext *context,
					   const char *serviceName,
					   const AtAgFeatures *features,
					   const BtlVgAudioSourcesAction *action)
{
	BtStatus status;
	BthalStatus halStatus;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_VG_Enable");

	UNUSED_PARAMETER(action);

	context = &voiceGatewayModuleData.context;

	BTL_VERIFY_ERR((BTL_VG_CONTEXT_STATE_DISABLED == context->state), BT_STATUS_INVALID_PARM,
		("context is not disabled"));

	status = BtlVgUtilsEnableAudioProfiles(features, serviceName);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), BT_STATUS_FAILED, ("Enable failed"));

	halStatus = BTHAL_MC_Register(BtlVgUtilsBthalMcCallback, (void*)context, &context->modem);
	BTL_VERIFY_ERR((BTHAL_STATUS_PENDING == halStatus), BT_STATUS_FAILED,
		("BTHAL_MC_Register failed: %s", BTHAL_StatusName(halStatus)));

	halStatus = BTHAL_VC_Register(BtlVgUtilsBthalVcCallback, (void*)context, &context->voice);
	BTL_VERIFY_ERR((BTHAL_STATUS_PENDING == halStatus), BT_STATUS_FAILED,
		("BTHAL_VC_Register failed: %s", BTHAL_StatusName(halStatus)));

	context->state = BTL_VG_CONTEXT_STATE_ENABLED;

	BtlVgUtilsSendEventToUser(0, BTL_VG_EVENT_VG_CONTEXT_ENABLED, BT_STATUS_SUCCESS, BEC_NO_ERROR, 0);

	/*
	 * 	TODO: Save the actions given from the user
	 */

	status = BT_STATUS_PENDING;

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_VG_Disable(BtlVgContext *context)
{
	BtStatus status;
	BthalStatus halStatus;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_VG_Disable");

	context = &voiceGatewayModuleData.context;

	BTL_VERIFY_ERR((BTL_VG_CONTEXT_STATE_ENABLED == context->state), BT_STATUS_INVALID_PARM,
		("context is not enabled"));

	context->state = BTL_VG_CONTEXT_STATE_DISABLING;

	status = BtlVgUtilsDisableAudioProfiles();
	BTL_VERIFY_ERR((BT_STATUS_PENDING == status || BT_STATUS_SUCCESS == status), status,
		("BtlVgDisableAudioProfiles failed", pBT_Status(status)));

	if (BT_STATUS_SUCCESS == status)
	{
		/*	All audio connections are needed to be closed before
		 *	Unregistering with the voice control module.
		 */
		halStatus = BTHAL_VC_Unregister(&context->voice);
		BTL_VERIFY_ERR((BTHAL_STATUS_PENDING == halStatus), BT_STATUS_FAILED,
			("BTHAL_VC_Register failed: %s", BTHAL_StatusName(halStatus)));
	}

	halStatus = BTHAL_MC_Unregister(&context->modem);
	BTL_VERIFY_ERR((BTHAL_STATUS_PENDING == halStatus), BT_STATUS_FAILED,
		("BTHAL_MC_Register failed: %s", BTHAL_StatusName(halStatus)));

	
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

#if BT_SECURITY == XA_ENABLED
BtStatus BTL_VG_SetSecurityLevel(BtlVgContext *context,
								 const BtSecurityLevel *secLevel)
{
	BtStatus status;
	
	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_VG_SetSecurityLevel");

	UNUSED_PARAMETER(context);

	status = BtlVgUtilsSetSecurityLevel(secLevel);

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_VG_GetSecurityLevel(const BtlVgContext *context,
								 BtSecurityLevel *secLevel)
{
	BtStatus status;

	BTL_FUNC_START_AND_LOCK_RADIO_OFF("BTL_VG_GetSecurityLevel");

	UNUSED_PARAMETER(context);

	BTL_VERIFY_ERR((0 != secLevel), BT_STATUS_INVALID_PARM, ("null secLevel"));

	*secLevel = HFG_GetSecurityLevel();

	status = BT_STATUS_SUCCESS;

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}
#endif /* BT_SECURITY == XA_ENABLED */

BtStatus BTL_VG_Connect(BtlVgContext *context,
						BD_ADDR *bdAddr,
						BtlVgAudioSource source)
{
	BtStatus status;
	BtlVgChannel *channel;	

	BTL_FUNC_START_AND_LOCK("BTL_VG_Connect");

	UNUSED_PARAMETER(context);
	UNUSED_PARAMETER(source);

	context = &voiceGatewayModuleData.context;

	BTL_VERIFY_ERR((0 == context->numChannels), BT_STATUS_IN_USE, ("BTL_VG_Connect: An SLC to a remote device already exists"));

	channel = BtlVgUtilsFindFreeChannel();
	BTL_VERIFY_ERR((0 != channel), BT_STATUS_NO_RESOURCES, ("BtlVgUtilsFindFreeChannel() failed: no resources"));

	status = BtlVgUtilsConnect(bdAddr, channel);

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_VG_ConnectAndGetChannelId(BtlVgContext *context,
									BD_ADDR *bdAddr,
									BtlVgAudioSource source,
									BtlVgChannelId *channelId)
{
	BtStatus status;
	BtlVgChannel *channel;

	BTL_FUNC_START_AND_LOCK("BTL_VG_ConnectAndGetChannelId");

	UNUSED_PARAMETER(source);

	context = &voiceGatewayModuleData.context;

	BTL_VERIFY_ERR((0 == context->numChannels), BT_STATUS_IN_USE, ("BTL_VG_Connect: An SLC to a remote device already exists"));

	channel = BtlVgUtilsFindFreeChannel();
	BTL_VERIFY_ERR((0 != channel), BT_STATUS_NO_RESOURCES, ("BtlVgUtilsFindFreeChannel() failed: no resources"));

	*channelId = BtlVgUtilsChannelToChannelId(channel);
	
	status = BtlVgUtilsConnect(bdAddr, channel);

	BTL_FUNC_END_AND_UNLOCK();

	return status;

}


BtStatus BTL_VG_Disconnect(BtlVgContext *context,
						   BtlVgChannelId channelId)
{
	BtStatus status;
	BtlVgChannel *channel;

	BTL_FUNC_START_AND_LOCK("BTL_VG_Disconnect");

	UNUSED_PARAMETER(context);

	BTL_VERIFY_ERR((BTL_VG_MAXIMUM_NUMBER_OF_CHANNELS > channelId), BT_STATUS_INVALID_PARM,
		("Invalid channelId"));

	channel = BtlVgUtilsChannelIdToChannel(channelId);

	if (BTL_VG_CCC_CONN_STATE_CONNECTING_ACL == channel->cccConnState)
	{
		BTL_LOG_INFO(("BTL VG: Cancel create connection during ACL establishment"));
		status = CMGR_RemoveDataLink(&channel->connectionHandler.cmgrHandler);
		BTL_LOG_INFO(("CMGR_RemoveDataLink returned: %s",pBT_Status(status)));
		(void) CMGR_DeregisterHandler(&channel->connectionHandler.cmgrHandler);
		channel->cccConnState = BTL_VG_CCC_CONN_STATE_NONE;
		channel->state = BTL_VG_CHANNEL_STATE_IDLE;
		channel->type = BTL_VG_UNDEFINED_CHANNEL;
		channel->c.handsfreeChannel = 0;
	}
	else if (BTL_VG_CCC_CONN_STATE_SDP_QUERY == channel->cccConnState)
	{
		channel->cancelCreateConn = TRUE;
		status = BT_STATUS_PENDING;
	}
	else
	{
		channel->cancelCreateConn = TRUE;
		status = BtlVgUtilsDisconnectServiceLink(channel);
	}

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_VG_PerformHandover(BtlVgContext *context,
							BtlVgChannelId source,
							BD_ADDR *target,
							BOOL accept)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK("BTL_VG_PerformHandover");

	context = &voiceGatewayModuleData.context;
	
	BTL_VERIFY_ERR((FALSE == context->handover),
                   XA_STATUS_IN_USE,
                   ("Handover is already in progress"));

	if (FALSE == accept)
	{
		context->handover = TRUE;
		context->handoverReject = TRUE;
		context->handoverSource = 0;
        
		if (0 != context->handoverRequest)
		{
			status = BtlVgUtilsRejectIncomingChannel(context->handoverRequest);
			BTL_VERIFY_ERR((BT_STATUS_PENDING == status), status,
				("BtlVgUtilsRejectIncomingChannel() returned: %s", pBT_Status(status)));
		}
	}
	else
	{
		BTL_VERIFY_ERR((0 != target), BT_STATUS_INVALID_PARM, ("null argument: target"));
		BTL_VERIFY_ERR((BTL_VG_MAXIMUM_NUMBER_OF_CHANNELS > source), BT_STATUS_INVALID_PARM,
			("Invalid channelId"));

		context->handover = TRUE;
		context->handoverReject = FALSE;
		context->handoverSource = BtlVgUtilsChannelIdToChannel(source);
        
		if (0 != context->handoverRequest)
		{
			status = BtlVgUtilsAcceptIncomingChannel(context->handoverRequest, target);

			if (BT_STATUS_PENDING == status)
			{
			    context->handoverTarget = context->handoverRequest;
			}
			else
			{
			    context->handoverRequest = 0;
			    BTL_LOG_ERROR(("BtlVgUtilsAcceptIncomingChannel() returned %s",
                                                    pBT_Status(status)));
			}
		}
		else
		{
			context->handoverTarget = BtlVgUtilsFindFreeChannel();
			BTL_VERIFY_ERR((0 != context->handoverTarget), BT_STATUS_NO_RESOURCES,
				("No BtlVgChannel available"));

			status = BtlVgUtilsConnect(target, context->handoverTarget);
			BTL_VERIFY_ERR((BT_STATUS_PENDING == status), status,
				("BtlVgUtilsConnect() failed: %s", pBT_Status(status)));
		}
	}

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}


BtStatus BTL_VG_GetConnectedDevice(const BtlVgContext *context,
										BtlVgChannelId channelId,
										BD_ADDR *bdAddr)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BD_ADDR *devAddr;

	BTL_FUNC_START_AND_LOCK("BTL_VG_GetConnectedDevice");

	UNUSED_PARAMETER(context);

	BTL_VERIFY_ERR((0 != bdAddr), BT_STATUS_INVALID_PARM, ("null argumnet"));
	BTL_VERIFY_ERR((BTL_VG_MAXIMUM_NUMBER_OF_CHANNELS > channelId), BT_STATUS_INVALID_PARM, ("invalid channelId"));

	devAddr = BtlVgUtilsGetDeviceAddress(BtlVgUtilsChannelIdToChannel(channelId));
	OS_MemCopy((U8*)bdAddr, (const U8*)devAddr, sizeof(BD_ADDR));

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_VG_AcceptAttachNumber(const BtlVgContext *context,
								   BtlVgChannelId channelId,
								   const char *number)
{
	BtStatus status;
	BtlVgChannel *channel;
	BtlVgString *vgString;

	BTL_FUNC_START_AND_LOCK("BTL_VG_AcceptAttachNumber");

	UNUSED_PARAMETER(context);

	BTL_VERIFY_ERR((BTL_VG_MAXIMUM_NUMBER_OF_CHANNELS > channelId), 
		BT_STATUS_INVALID_PARM, ("invalid channelId"));
	BTL_VERIFY_ERR((0 != number), BT_STATUS_INVALID_PARM, ("Null argument"));

	channel = BtlVgUtilsChannelIdToChannel(channelId);
	
	BTL_VERIFY_ERR((BTL_VG_HANDSFREE_CHANNEL == BtlVgUtilsGetChannelType(channel)),
		BT_STATUS_NOT_SUPPORTED, ("Unsupported feature"));

	status = BtlVgUtilsBtlVgStringCreate(number, &vgString);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("BtlVgUtilsBtlVgStringCreate() failed: %s", pBT_Status(status)));

	status = BtlVgUtilsSendResults(channel, AT_VOICE_TAG, (U32)vgString->string, sizeof(char*));
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("BtlVgUtilsSendResults() failed: %s", pBT_Status(status)));
	
	status = BtlVgUtilsSendResults(channel, AT_OK, 0, 0);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("BtlVgUtilsSendResults() failed: %s", pBT_Status(status)));	

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_VG_RejectAttachNumber(const BtlVgContext *context,
								   BtlVgChannelId channelId)
{
	BtStatus status;
	BtlVgChannel *channel;

	BTL_FUNC_START_AND_LOCK("BTL_VG_RejectAttachNumber");

	UNUSED_PARAMETER(context);

	BTL_VERIFY_ERR((BTL_VG_MAXIMUM_NUMBER_OF_CHANNELS > channelId), 
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	channel = BtlVgUtilsChannelIdToChannel(channelId);
	
	BTL_VERIFY_ERR((BTL_VG_HANDSFREE_CHANNEL == BtlVgUtilsGetChannelType(channel)),
		BT_STATUS_NOT_SUPPORTED, ("Unsupported feature"));

	status = BtlVgUtilsSendResults(BtlVgUtilsChannelIdToChannel(channelId), AT_ERROR, (U32)HFG_CME_OP_NOT_ALLOWED, 0);

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_VG_EnableInbandRingTone(BtlVgContext *context,
									 BtlVgChannelId channelId,
									 BOOL enable)
{
	BtStatus status = BTHAL_STATUS_FAILED;
	BtlVgChannel *channel;

	BTL_FUNC_START_AND_LOCK("BTL_VG_EnableInbandRingTone");

	UNUSED_PARAMETER(context);

	BTL_VERIFY_ERR((BTL_VG_MAXIMUM_NUMBER_OF_CHANNELS > channelId), 
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	channel = BtlVgUtilsChannelIdToChannel(channelId);

	if ((BTL_VG_HANDSFREE_CHANNEL == BtlVgUtilsGetChannelType(channel)) &&
		(HFG_GetFeatures() & HFG_FEATURE_RING_TONE))
	{
		status = BtlVgUtilsSendResults(channel, AT_IN_BAND_RING_TONE, (U32)enable, 0);
	}
	else
	{
		Report(("BTL_VG: Inband ringing is not supported"));
	}
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_VG_IsInbandRingToneEnabled(const BtlVgContext *context,
										BtlVgChannelId channelId,
										BOOL *enable)
{
	BtStatus status;
	BtlVgChannel *channel;

	BTL_FUNC_START_AND_LOCK("BTL_VG_IsInbandRingToneEnabled");

	UNUSED_PARAMETER(context);

	BTL_VERIFY_ERR((BTL_VG_MAXIMUM_NUMBER_OF_CHANNELS > channelId), 
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	BTL_VERIFY_ERR((0 != enable), BT_STATUS_INVALID_PARM, ("null enable"));

	channel = BtlVgUtilsChannelIdToChannel(channelId);
	
	if ((BTL_VG_HANDSFREE_CHANNEL == BtlVgUtilsGetChannelType(channel)) &&
		(HFG_GetFeatures() & HFG_FEATURE_RING_TONE))
	{
		*enable = HFG_IsInbandRingEnabled(channel->c.handsfreeChannel);

		status = BT_STATUS_SUCCESS;
	}
	else
	{
		Report(("BTL_VG: Inband ringing is not supported"));
		
		status = BT_STATUS_FAILED;
	}

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_VG_EnableVoiceRecognition(BtlVgContext *context,
									   BtlVgChannelId channelId,
									   BOOL enable)
{
	BtStatus status = BT_STATUS_PENDING;
	BthalStatus halStatus;

	BTL_FUNC_START_AND_LOCK("BTL_VG_EnableVoiceRecognition");

	context = &voiceGatewayModuleData.context;

	BTL_VERIFY_ERR((BTL_VG_MAXIMUM_NUMBER_OF_CHANNELS > channelId), 
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	halStatus = BTHAL_VC_SetVoiceRecognition(context->voice, enable);
	BTL_VERIFY_ERR((BTHAL_STATUS_PENDING == halStatus), BT_STATUS_FAILED,
		("BTHAL_VC_SetVoiceRecognition failed"));

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_VG_IsVoiceRecognitionEnabled(const BtlVgContext *context,
										  BtlVgChannelId channelId,
										  BOOL *enable)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtlVgChannel *channel;

	BTL_FUNC_START_AND_LOCK("BTL_VG_IsVoiceRecognitionEnabled");

	BTL_VERIFY_ERR((BTL_VG_MAXIMUM_NUMBER_OF_CHANNELS > channelId), 
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	BTL_VERIFY_ERR((0 != enable), BT_STATUS_INVALID_PARM, ("null enable"));

	UNUSED_PARAMETER(context);

	channel = BtlVgUtilsChannelIdToChannel(channelId);

	BTL_VERIFY_ERR((BTL_VG_HANDSFREE_CHANNEL == BtlVgUtilsGetChannelType(channel)),
		BT_STATUS_INVALID_PARM, ("Currently VR is supported only for HF Units"));

	*enable = HFG_IsVoiceRecActive(channel->c.handsfreeChannel);
	
	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_VG_SendMicrophoneVolume(const BtlVgContext *context,
									 BtlVgChannelId channelId,
									 U32 volume)
{
	UNUSED_PARAMETER(context);

	return BtlVgUtilsSendVolume(channelId, volume, AT_MICROPHONE_GAIN);
}

BtStatus BTL_VG_SendSpeakerVolume(const BtlVgContext *context,
								  BtlVgChannelId channelId,
								  U32 volume)
{
	UNUSED_PARAMETER(context);

	return BtlVgUtilsSendVolume(channelId, volume, AT_SPEAKER_GAIN);
}

BtStatus BTL_VG_SendAtResult(const BtlVgContext *context,
						BtlVgChannelId channelId,
						const char *atString)
{
	BtStatus status;
	BtlVgString *vgStr;
	
	BTL_FUNC_START_AND_LOCK("BTL_VG_SendAtResult");

	UNUSED_PARAMETER(context);

	BTL_VERIFY_ERR((BTL_VG_MAXIMUM_NUMBER_OF_CHANNELS > channelId), 
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	BTL_VERIFY_ERR((0 != atString), BT_STATUS_INVALID_PARM, ("null atString"));

	status = BtlVgUtilsBtlVgStringCreate(atString, &vgStr);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status,
		("BtlVgUtilsBtlVgStringCreate() failed: %s", pBT_Status(status)));
	
	status = BtlVgUtilsSendResults(BtlVgUtilsChannelIdToChannel(channelId), AT_RAW, (U32)atString, sizeof(atString));

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_VG_SentAtError(const BtlVgContext *context,
						BtlVgChannelId channelId,
						AtError err)
{
	BtStatus status;
	
	BTL_FUNC_START_AND_LOCK("BTL_VG_SentAtError");

	UNUSED_PARAMETER(context);

	BTL_VERIFY_ERR((BTL_VG_MAXIMUM_NUMBER_OF_CHANNELS > channelId), 
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	status = BtlVgUtilsSendResults(BtlVgUtilsChannelIdToChannel(channelId), AT_ERROR, (U32)err, 0);

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BtlVgUtilsSendVolume(BtlVgChannelId channelId,
						 U32 volume,
						 AtCommand cmd)
{
	BtStatus status;

	BTL_FUNC_START_AND_LOCK("BtlVgSendVolume");

	BTL_VERIFY_ERR((BTL_VG_MAXIMUM_NUMBER_OF_CHANNELS > channelId), 
		BT_STATUS_INVALID_PARM, ("invalid channelId"));

	BTL_VERIFY_ERR((BTL_VG_MAX_VOLUME >= volume), BT_STATUS_INVALID_PARM, ("Illegal volume"));

	status = BtlVgUtilsSendResults(BtlVgUtilsChannelIdToChannel(channelId), cmd, volume, 0);

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_VG_ConnectAudio(BtlVgContext *context,
							 BtlVgChannelId channelId)
{
	BtStatus status;
 	
	BTL_FUNC_START_AND_LOCK("BTL_VG_ConnectAudio");

	UNUSED_PARAMETER(context);

 	BTL_VERIFY_ERR((BTL_VG_MAXIMUM_NUMBER_OF_CHANNELS > channelId), BT_STATUS_INVALID_PARM, ("Invalid channelId"));

	status = BtlVgUtilsConnectAudio(BtlVgUtilsChannelIdToChannel(channelId));

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

BtStatus BTL_VG_DisconnectAudio(BtlVgContext *context,
								BtlVgChannelId channelId)
{
	BtStatus status;

	BTL_FUNC_START_AND_LOCK("BTL_VG_DisconnectAudio");

	UNUSED_PARAMETER(context);

	status = BtlVgUtilsDisconnectAudio(BtlVgUtilsChannelIdToChannel(channelId));

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

#if (0)

BtStatus BTL_VG_SetFmOverBtMode(BtlVgContext *context,
								BtlVgChannelId channelId,
								BOOL enableFMoBT)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START_AND_LOCK("BTL_VG_SetFmOverBtMode");

	UNUSED_PARAMETER(context);

	status = BtlVgUtilsSetFmOverBt(BtlVgUtilsChannelIdToChannel(channelId), enableFMoBT);

	BTL_VERIFY_ERR((BT_STATUS_PENDING == status),
                                    status,
                                    ("BtlVgUtilsSetFmOverBt() failed: %s", pBT_Status(status)));

	BTL_FUNC_END_AND_UNLOCK();

	return status;
}

#endif /* BTL_CONFIG_FM_STACK == BTL_CONFIG_ENABLED */


#else /*BTL_CONFIG_VG ==   BTL_CONFIG_ENABLED*/

/*-------------------------------------------------------------------------------
 * BTL_VG_Init() - When  BTL_CONFIG_VG is disabled.
 */
BtStatus BTL_VG_Init(void)
{
    
   BTL_LOG_INFO(("BTL_VG_Init()  -  BTL_CONFIG_VG Disabled"));
  

    return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 * BTL_VG_Deinit() - When  BTL_CONFIG_FTPS is disabled.
 */
BtStatus BTL_VG_Deinit(void)
{
    BTL_LOG_INFO(("BTL_VG_Deinit() -  BTL_CONFIG_VG Disabled"));

    return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------*/



#endif /*BTL_CONFIG_VG ==   BTL_CONFIG_ENABLED*/




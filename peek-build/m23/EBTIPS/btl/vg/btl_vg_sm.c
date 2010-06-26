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
*   DESCRIPTION:	This file contains the VG state machine 
*
*   AUTHOR:			Itay Klein
*
\*******************************************************************************/


#include "btl_config.h"
#if BTL_CONFIG_VG == BTL_CONFIG_ENABLED

/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/

#include "debug.h"
#include "me.h"
#include "btl_defs.h"
#include "btl_vgi.h"
#include "btl_bsc.h"
#include "bthal_mc.h"
#include "bthal_vc.h"
#include "atpi.h"
#include "hfgi.h"


/********************************************************************************
 *
 * Static functions declaration
 *
 *******************************************************************************/

static void BtlVgSmEnabledProcessHandsFreeEvent(BtlVgIncomingEvent *event);
static void BtlVgSmDisablingProcessHandsFreeEvent(BtlVgIncomingEvent *event);
static void BtlVgSmDisabledProcessHandsFreeEvent(BtlVgIncomingEvent *event);

static void BtlVgSmEnabledProcessHeadSetEvent(BtlVgIncomingEvent *event);
static void BtlVgSmDisablingProcessHeadSetEvent(BtlVgIncomingEvent *event);
static void BtlVgSmDisabledProcessHeadSetEvent(BtlVgIncomingEvent *event);

static void BtlVgSmEnabledProcessModemControlEvent(BtlVgIncomingEvent *event);
static void BtlVgSmDisablingProcessModemControlEvent(BtlVgIncomingEvent *event);
static void BtlVgSmDisabledProcessModemControlEvent(BtlVgIncomingEvent *event);

static void BtlVgSmEnabledProcessVoiceControlEvent(BtlVgIncomingEvent *event);
static void BtlVgSmDisablingProcessVoiceControlEvent(BtlVgIncomingEvent *event);
static void BtlVgSmDisabledProcessVoiceControlEvent(BtlVgIncomingEvent *event);


static void BtlVgSmStateMachine(BtlVgIncomingEvent *event);

static void BtlVgSmHandleSlcRequest(BtlVgChannel *channel,
									BtRemoteDevice *remDev,
									BtStatus status,
									BtErrorCode errCode);
									
static void BtlVgSmHandleSlcConnected(BtlVgChannel *channel,
									  BtStatus status,
									  BtErrorCode errCode);

static void BtlVgSmHandleSlcDisconnected(BtlVgChannel *channel,
										 BtStatus status,
										 BtErrorCode errCode);

static void BtlVgSmHandleAudioConnected(BtlVgChannel *channel,
										BtStatus status,
										BtErrorCode errCode);

static void BtlVgSmHandleAudioDisconnected(BtlVgChannel *channel,
										   BtStatus status,
										   BtErrorCode errCode);

static BthalStatus BtlVgSmHandleAnswerCall(BtlVgChannel *channel);

static void BtlVgSmHandleAtCommandData(BtlVgChannel *channel,
									   const BtlVgString *string,
									   BtStatus status,
									   BtErrorCode errCode);

static BthalStatus BtlVgSmHandleHangup(BtlVgChannel *channel);

static void BtlVgSmHandleVoicePathsChanged(BthalVcAudioPath path);

static void BtlVgSmProcessModemEventHandsfree(BtlVgChannel *channel,
											  const BthalMcEvent *event);

static void BtlVgSmProcessModemEventHeadset(BtlVgChannel *channel,
											const BthalMcEvent *event);


static BtStatus BtlVgSmCallWaitNotify(BtlVgChannel *channel,
									  const BthalMcEvent *event);

static BtStatus BtlVgSmSendCallList(BtlVgChannel *channel,
									const BthalMcEvent *event);

static BtStatus BtlVgSmClipNotify(BtlVgChannel *channel,
								  const BthalMcEvent *event);

static BtStatus BtlVgSmHandleIndicator(BtlVgChannel *channel,
									   const BthalMcEvent *event);

static BtStatus BtlVgSmReportIndicator(BtlVgChannel *channel,
									   HfgIndicator ind,
									   U8 val);

static BtStatus BtlVgSmHandleCallAudio(BtlVgChannel *channel,
								  HfgIndicator indicator,
								  U8 value);

static BtStatus BtlVgSmHeadsetSmIndicator(HfgIndicator ind,
											U8 val);

static BtStatus BtlVgSmSendSubscriber(BtlVgChannel *channel,
									  const BthalMcEvent *event);

static BtStatus BtlVgSmSendOperator(BtlVgChannel *channel,
									const BthalMcEvent *event);

static BtStatus BtlVgSmSendSelectedPhonebook(BtlVgChannel *channel,
										const BthalMcEvent *event);

static BtStatus BtlVgSmSendSupportedPhonebooks(BtlVgChannel *channel,
										const BthalMcEvent *event);

static BtStatus BtlVgSmSendReadPhonebookRes(BtlVgChannel *channel,
												const BthalMcEvent *event);

static BtStatus BtlVgSmSendFindPhonebookRes(BtlVgChannel *channel,
												const BthalMcEvent *event);

static BtStatus BtlVgSmSendSelectedCharSet(BtlVgChannel *channel,
											const BthalMcEvent *event);

static BtStatus BtlVgSmSendTstReadPhonebookRes(BtlVgChannel *channel,
													const BthalMcEvent *event);

static void BtlVgSmHandleHsButtonPressed(BtlVgChannel *channel);

static void BtlVgSmPassModemEventToChannels(const BthalMcEvent *event);

static void BtlVgSmPassVoiceEventToChannels(const BthalVcEvent *event);

static void BtlVgSendEventToBsc(BtlBscEventType eventType,
                                				   BtlVgChannel *vgChannel,
                                				   BD_ADDR *bdAddr);

static BtStatus BtlVgSmHandleVRStatusChanged(const BthalVcEvent *event,
									BtlVgChannel *channel);

static BtStatus BtlVgSmHandleNRECStatusChanged(const BthalVcEvent *event, 
											BtlVgChannel *channel);

/********************************************************************************
 *
 * Functions definitions
 *
 *******************************************************************************/

BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_VG);

void BTL_VG_ProcessEvents(void)
{
	BtStatus status;
	ListEntry *list;

	BTL_FUNC_START_AND_LOCK_COMMON("BTL_VG_ProcessEvents");
	
	list = &voiceGatewayModuleData.incomingEventsQue;
	while (FALSE == IsListEmpty(list))
	{
		BtlVgIncomingEvent *event = (BtlVgIncomingEvent*)RemoveHeadList(list);
		BtlVgSmStateMachine(event);
		status = BTL_POOL_Free(&voiceGatewayModuleData.incomingEventsPool, (void**)&event);
		BTL_VERIFY_FATAL_NO_RETVAR((BT_STATUS_SUCCESS == status),("BTL_POOL_Free failed"));
	}

	BTL_FUNC_END_AND_UNLOCK();
}

static void BtlVgSmStateMachine(BtlVgIncomingEvent *event)
{
	BtlVgContext *context = &voiceGatewayModuleData.context;
	
	BTL_FUNC_START("BtlVgSmStateMachine");

	context->stateMachine[context->state][event->source](event);
	
	BTL_FUNC_END();
}

void BtlVgSmInitStateMachine(void)
{
	BtlVgContext *context;

	BTL_FUNC_START("BtlVgUtilsInitStateMachine");
	
	context = &voiceGatewayModuleData.context;

	context->stateMachine[BTL_VG_CONTEXT_STATE_DISABLED][BTL_VG_EVENT_SOURCE_HFG] = BtlVgSmDisabledProcessHandsFreeEvent;
	context->stateMachine[BTL_VG_CONTEXT_STATE_DISABLED][BTL_VG_EVENT_SOURCE_HSG] = BtlVgSmDisabledProcessHeadSetEvent;
	context->stateMachine[BTL_VG_CONTEXT_STATE_DISABLED][BTL_VG_EVENT_SOURCE_MC] = BtlVgSmDisabledProcessModemControlEvent;
	context->stateMachine[BTL_VG_CONTEXT_STATE_DISABLED][BTL_VG_EVENT_SOURCE_VC] = BtlVgSmDisabledProcessVoiceControlEvent;
	
	context->stateMachine[BTL_VG_CONTEXT_STATE_DISABLING][BTL_VG_EVENT_SOURCE_HFG] = BtlVgSmDisablingProcessHandsFreeEvent;
	context->stateMachine[BTL_VG_CONTEXT_STATE_DISABLING][BTL_VG_EVENT_SOURCE_HSG] = BtlVgSmDisablingProcessHeadSetEvent;
	context->stateMachine[BTL_VG_CONTEXT_STATE_DISABLING][BTL_VG_EVENT_SOURCE_MC] = BtlVgSmDisablingProcessModemControlEvent;
	context->stateMachine[BTL_VG_CONTEXT_STATE_DISABLING][BTL_VG_EVENT_SOURCE_VC] = BtlVgSmDisablingProcessVoiceControlEvent;
	
	context->stateMachine[BTL_VG_CONTEXT_STATE_ENABLED][BTL_VG_EVENT_SOURCE_HFG] = BtlVgSmEnabledProcessHandsFreeEvent;
	context->stateMachine[BTL_VG_CONTEXT_STATE_ENABLED][BTL_VG_EVENT_SOURCE_HSG] = BtlVgSmEnabledProcessHeadSetEvent;
	context->stateMachine[BTL_VG_CONTEXT_STATE_ENABLED][BTL_VG_EVENT_SOURCE_MC] = BtlVgSmEnabledProcessModemControlEvent;
	context->stateMachine[BTL_VG_CONTEXT_STATE_ENABLED][BTL_VG_EVENT_SOURCE_VC] = BtlVgSmEnabledProcessVoiceControlEvent;

	BTL_FUNC_END();
}


static void BtlVgSmEnabledProcessHandsFreeEvent(BtlVgIncomingEvent *event)
{
	BtStatus status;
	BthalStatus halStatus;
	BtlVgChannel *channel;
	BtlVgHfgEvent *hfgEvent;
	BtlVgContext *context;
	BOOL pendingBthalMcCommand = TRUE;
	
	BTL_FUNC_START("BtlVgSmEnabledProcessHandsFreeEvent");

	halStatus = BTHAL_STATUS_SUCCESS;
	hfgEvent = &event->e.hfgEvent;
	channel = event->channel;
	context = &voiceGatewayModuleData.context;

	BTL_LOG_INFO(("BTL VG: %s", BtlVgUtilsHfgEventName(hfgEvent->type)));
	
	switch (hfgEvent->type)
	{
	case  HFG_EVENT_SERVICE_CONNECT_REQ:
		channel = BtlVgUtilsFindFreeChannel();
		if (0 != channel)
		{
			channel->type = BTL_VG_HANDSFREE_CHANNEL;
			channel->c.handsfreeChannel = hfgEvent->channel;
			BtlVgSmHandleSlcRequest(channel, hfgEvent->p.remDev, hfgEvent->status,
				hfgEvent->errCode);
		}
		else
		{
			BTL_LOG_ERROR(("event HFG_EVENT_SERVICE_CONNECT_REQ, no free BtlVgChannel"));
			status = HFG_RejectIncomingSLC(hfgEvent->channel);
			BTL_VERIFY_ERR_NO_RETVAR((BT_STATUS_PENDING == status || BT_STATUS_SUCCESS == status),
				("HFG_RejectIncomingSLC() returned: %s", pBT_Status(status)));
		}
		break;

	case HFG_EVENT_SERVICE_CONNECTED:
		/* Pass event to the BSC module */
		BtlVgSendEventToBsc(BTL_BSC_EVENT_SLC_CONNECTED, channel, &hfgEvent->p.remDev->bdAddr);
		BtlVgSmHandleSlcConnected(channel, hfgEvent->status, hfgEvent->errCode);
		break;

	case HFG_EVENT_SERVICE_DISCONNECTED:
		BtlVgSmHandleSlcDisconnected(channel, hfgEvent->status, hfgEvent->errCode);
		break;

	case HFG_EVENT_AUDIO_CONNECT_REQ:
		BTL_LOG_INFO(("BTL VG: Incoming audio request"));
		channel->audioRequested = TRUE;
		status = BtlVgUtilsConnectAudio(channel);
		BTL_VERIFY_ERR_NO_RETVAR((BT_STATUS_PENDING == status),("BtlVgUtilsRouteAudioToChannel failed"));
		break;

	case HFG_EVENT_AUDIO_CONNECTED:
		BtlVgSmHandleAudioConnected(channel, hfgEvent->status, hfgEvent->errCode);
		break;

	case HFG_EVENT_AUDIO_DISCONNECTED:
		BtlVgSmHandleAudioDisconnected(channel,hfgEvent->status, hfgEvent->errCode);
		break;
		
	case HFG_EVENT_REPORT_MIC_VOLUME:
		BtlVgUtilsSendEventToUser(channel, BTL_VG_EVENT_MICROPHONE_VOLUME,
			hfgEvent->status, hfgEvent->errCode, (void*)hfgEvent->p.gain);
		status = BtlVgUtilsSendResults(channel, AT_OK, 0, 0);
		BTL_VERIFY_ERR_NO_RETVAR((BT_STATUS_SUCCESS==status), ("BtlVgUtilsSendResults failed: %s", pBT_Status(status)));
		break;

	case HFG_EVENT_REPORT_SPK_VOLUME:
		BtlVgUtilsSendEventToUser(channel, BTL_VG_EVENT_SPEAKER_VOLUME,
			hfgEvent->status, hfgEvent->errCode, (void*)hfgEvent->p.gain);
		status = BtlVgUtilsSendResults(channel, AT_OK, 0, 0);
		BTL_VERIFY_ERR_NO_RETVAR((BT_STATUS_SUCCESS==status), ("BtlVgUtilsSendResults failed: %s", pBT_Status(status)));
		break;


	case HFG_EVENT_GET_LAST_VOICE_TAG:
		if (HFG_GetFeatures() & HFG_FEATURE_VOICE_TAG)
		{
			BtlVgUtilsSendEventToUser(channel, BTL_VG_EVENT_PHONE_NUMBER_REQUESTED, hfgEvent->status, hfgEvent->errCode, 0);
		}
		else
		{
			status = BtlVgUtilsSendResults(channel, AT_ERROR, (U32)HFG_CME_OP_NOT_SUPPORTED, 0);
			BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == status), ("BtlVgUtilsSendResults returned: %s", pBT_Status(status)));
		}
		break;

	case HFG_EVENT_AT_COMMAND_DATA:
		BtlVgSmHandleAtCommandData(channel, &hfgEvent->p.atString, hfgEvent->status, hfgEvent->errCode);
		break;

#if BT_SCO_HCI_DATA == XA_ENABLED

	case HFG_EVENT_AUDIO_DATA:
		/* TODO */
		break;

	case HFG_EVENT_AUDIO_DATA_SENT:
		/* TODO */
		break;
#endif

	case HFG_EVENT_HANDSFREE_FEATURES:
		break;

	case HFG_EVENT_ANSWER_CALL:
		halStatus = BtlVgSmHandleAnswerCall(channel);
		break;

	case HFG_EVENT_DIAL_NUMBER:
		halStatus = BTHAL_MC_DialNumber(context->modem, (BTHAL_U8*)hfgEvent->p.phoneNumber.string, (U8)hfgEvent->p.phoneNumber.length);
		break;

	case HFG_EVENT_MEMORY_DIAL:
		halStatus = BTHAL_MC_DialMemory(context->modem, (BTHAL_U8*)hfgEvent->p.memory.string, (U8)hfgEvent->p.memory.length);
		break;

	case HFG_EVENT_REDIAL:
		halStatus = BTHAL_MC_DialLastNumber(context->modem);
		break;

	case HFG_EVENT_CALL_HOLD:
		if (HFG_GetFeatures() & HFG_FEATURE_THREE_WAY_CALLS)
		{
			halStatus = BTHAL_MC_HandleCallHoldAndMultiparty(context->modem, &hfgEvent->p.hold);
		}
		else
		{
			status = BtlVgUtilsSendResults(channel, AT_ERROR, (U32)HFG_CME_OP_NOT_SUPPORTED, 0);
			BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == status), ("BtlVgUtilsSendResults failed: %s", pBT_Status(status)));
		}
		break;

#if HFG_USE_RESP_HOLD == XA_ENABLED

	case HFG_QUERY_RESPONSE_HOLD:
		halStatus = BTHAL_MC_RequestResponseAndHoldStatus(context->modem);
		break;

	case HFG_RESPONSE_HOLD:
		halStatus = BTHAL_MC_SetResponseAndHold(context->modem, hfgEvent->p.respHold);
		break;
#endif

	case HFG_EVENT_HANGUP:
		halStatus = BtlVgSmHandleHangup(channel);
		break;

	case HFG_EVENT_LIST_CURRENT_CALLS:
		halStatus = BTHAL_MC_RequestCurrentCallsList(context->modem);
		break;

	case HFG_EVENT_ENABLE_CALLER_ID:
		halStatus = BTHAL_MC_SetClipNotification(context->modem, (BTHAL_BOOL)hfgEvent->p.enabled);
		break;

	case HFG_EVENT_ENABLE_CALL_WAITING:
		/*The AT command is not send to the network, Call Waiting Activation enable
		   *  and disable is handled on VG side itself.It is assumed that call waiting service
		   *  is already active in the network
		   */
		status = BtlVgUtilsSendResults(channel, AT_OK, 0, 0);
		BTL_VERIFY_ERR_NO_RETVAR((BT_STATUS_SUCCESS==status), ("BtlVgUtilsSendResults failed: %s", pBT_Status(status)));
		break;

	case HFG_EVENT_GENERATE_DTMF:
		halStatus = BTHAL_MC_GenerateDTMF(context->modem, (BTHAL_I32)hfgEvent->p.dtmf);
		break;

	case HFG_EVENT_ENABLE_VOICE_RECOGNITION:
		halStatus = BTHAL_VC_SetVoiceRecognition(context->voice, hfgEvent->p.enabled);		
		BTL_VERIFY_ERR_NO_RETVAR((BTHAL_STATUS_PENDING == halStatus),("BTHAL_VC_SetVoiceRecognition failed"));
		channel->pendingBthalVcCommand = TRUE;
		pendingBthalMcCommand = FALSE;
		break;

	case HFG_EVENT_DISABLE_NREC:
		halStatus = BTHAL_VC_SetNoiseReductionAndEchoCancelling(context->voice, FALSE);
		BTL_VERIFY_ERR_NO_RETVAR((BTHAL_STATUS_PENDING == halStatus),
			("BTHAL_VC_SetNoiseReductionAndEchoCancelling failed"));
		channel->pendingBthalVcCommand = TRUE;
		pendingBthalMcCommand = FALSE;
		break;

	case HFG_EVENT_QUERY_NETWORK_OPERATOR:
		halStatus = BTHAL_MC_RequestNetworkOperatorString(context->modem);
		break;
 
	case HFG_EVENT_QUERY_SUBSCRIBER_NUMBER:
		halStatus = BTHAL_MC_RequestSubscriberNumberInformation(context->modem);
		break;

	case HFG_EVENT_ENABLE_EXTENDED_ERRORS:
		halStatus = BTHAL_MC_SetExtendedErrors(context->modem, 	BTHAL_TRUE);
		break;

	case HFG_EVENT_SET_CHAR_SET:
		halStatus = BTHAL_MC_SetCharSet(context->modem, hfgEvent->p.charSetType.string);
		break;

	case HFG_EVENT_SELECTED_CHAR_SET:
		halStatus = BTHAL_MC_RequestSelectedCharSet(context->modem);
		break;

	case HFG_EVENT_SET_PHONEBOOK:
		halStatus = BTHAL_MC_SetPhonebook(context->modem, hfgEvent->p.phonebookStorage);
		break;

	case HFG_EVENT_SELECTED_PHONEBOOK_INFO:
		halStatus = BTHAL_MC_RequestSelectedPhonebook(context->modem);
		break;

	case HFG_EVENT_SUPPORTED_PHONEBOOK_LIST:
		halStatus = BTHAL_MC_RequestSupportedPhonebooks(context->modem);
		break;

	case HFG_EVENT_READ_PHONEBOOK_ENTRIES:
		halStatus = BTHAL_MC_ReadPhonebook(context->modem, hfgEvent->p.phonebookRead.first,
			hfgEvent->p.phonebookRead.last);
		break;

	case HFG_EVENT_READ_PHONEBOOK_ENTRIES_RANGE:
		halStatus = BTHAL_MC_RequestPhonebookSupportedIndices(context->modem);
		break;

	case HFG_EVENT_FIND_PHONEBOOK_ENTRIES:
		halStatus = BTHAL_MC_FindPhonebook(context->modem, hfgEvent->p.phonebookFindString.string);
		break;

	case HFG_EVENT_RESPONSE_COMPLETE:
		status = BtlVgUtilsClearAtResult(hfgEvent->p.response);
		BTL_VERIFY_FATAL_NORET((BT_STATUS_SUCCESS == status), ("BtlVgUtilsClearAtResult() failed: %s", pBT_Status(status)));
		break;

	default:
		BTL_FATAL_NORET(("BTL VG: Unrecognized event: %d", hfgEvent->type));
	}
	
	if (BTHAL_STATUS_PENDING == halStatus)
	{
	    /* A command was sent to the BTHAL MC or to the BTHAL VC and response is pending */
	    if (TRUE == pendingBthalMcCommand)
	    {
        	    /* Increment command ID, which is sent to the BTHAL MC, and store it
        	     * in channel's database */
        	    voiceGatewayModuleData.bthalMcCommandId =
                            BtlVgUtilsIncrementCmdRspId(voiceGatewayModuleData.bthalMcCommandId);
        	    channel->bthalMcCommandId = voiceGatewayModuleData.bthalMcCommandId;
                
        	    BTL_LOG_DEBUG(("BTL_VG: bthalMcCommandId %d",
                                                channel->bthalMcCommandId));
	    }
	    else
	    {
        	    /* Increment command ID, which is sent to the BTHAL VC, and store it
        	     * in channel's database */
        	    voiceGatewayModuleData.bthalVcCommandId =
                            BtlVgUtilsIncrementCmdRspId(voiceGatewayModuleData.bthalVcCommandId);
        	    channel->bthalVcCommandId = voiceGatewayModuleData.bthalVcCommandId;
                
        	    BTL_LOG_DEBUG(("BTL_VG: bthalVcCommandId %d",
                                                channel->bthalVcCommandId));
	    }

	}
	/* Function BtlVgSmHandleAnswerCall() may return BTHAL_STATUS_IMPROPER_STATE,
	 * in this case, BtlVgUtilsSendResults() was already called with another error type.
	 * In case no command to the modem was needed, the status remains unchanged
	 * BTHAL_STATUS_SUCCESS 
	 *Function BtlVgSmHandleHangup() may return  BTHAL_STATUS_NOT_SUPPORTED
	 * in this case, BtlVgUtilsSendResults() was already called with another error type.
	 */
	else if ((BTHAL_STATUS_IMPROPER_STATE != halStatus) &&
	            (BTHAL_STATUS_SUCCESS != halStatus) &&
	            (BTHAL_STATUS_NOT_SUPPORTED != halStatus))
	{
		BTL_LOG_ERROR(("BTL VG: Failed processing event type %d, status: %s", hfgEvent->type, BTHAL_StatusName(halStatus)));
		status = BtlVgUtilsSendResults(channel, AT_ERROR, (U32)HFG_CME_UNKNOWN, 0);
		BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == status), ("BtlVgUtilsSendResults failed: %s", pBT_Status(status)));
	}

	BTL_FUNC_END();
}

static void BtlVgSmDisablingProcessHandsFreeEvent(BtlVgIncomingEvent *event)
{
	BtStatus status;
	BtlVgChannel *channel;
	BtlVgHfgEvent *hfgEvent;
	BtlVgContext *context;

	BTL_FUNC_START("BtlVgSmDisablingProcessHandsFreeEvent");

	channel = event->channel;
	hfgEvent = &event->e.hfgEvent;

	BTL_LOG_INFO(("BTL VG: %s", BtlVgUtilsHfgEventName(hfgEvent->type)));

	switch (hfgEvent->type)
	{
	case HFG_EVENT_SERVICE_CONNECT_REQ:
		status = BtlVgUtilsRejectIncomingChannel(channel);
		BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == status),
			("BtlVgUtilsRejectIncomingChannel failed :%s", pBT_Status(status)));
		break;		

	case HFG_EVENT_SERVICE_CONNECTED:
		/* Pass event to the BSC module */
		BtlVgSendEventToBsc(BTL_BSC_EVENT_SLC_CONNECTED, channel, &hfgEvent->p.remDev->bdAddr);
		BtlVgSmHandleSlcConnected(channel, hfgEvent->status, hfgEvent->errCode);
		break;

	case HFG_EVENT_SERVICE_DISCONNECTED:
		BtlVgSmHandleSlcDisconnected(channel, hfgEvent->status, hfgEvent->errCode);

		/*	If this event was received, it means that an SLC existed
		 *	When BTL_VG_Disable was called.
		 *	So now we need to call BTHAL_VC_Unregister().
		 */
		context = &voiceGatewayModuleData.context;
		if (0 == context->numChannels)
		{
			BthalStatus halStatus;
			
			status = BtlVgUtilsDeregisterServices();
			BTL_VERIFY_FATAL_NO_RETVAR((BT_STATUS_SUCCESS == status),
				("BtlVgUtilsDeregisterServices() failed: %s", pBT_Status(status)));

			halStatus = BTHAL_VC_Unregister(&context->voice);
			BTL_VERIFY_ERR_NO_RETVAR((BTHAL_STATUS_PENDING == halStatus),
				("BTHAL_VC_Unregister() failed: %s", BTHAL_StatusName(halStatus)));
		}
		break;

	case HFG_EVENT_AUDIO_CONNECT_REQ:
		/* Reject the audio request */
		HFG_AcceptAudioLink(channel->c.handsfreeChannel, BEC_USER_TERMINATED);
		break;

	case HFG_EVENT_AUDIO_DISCONNECTED:
		BtlVgSmHandleAudioDisconnected(channel, hfgEvent->status, hfgEvent->errCode);
		break;

	case HFG_EVENT_AUDIO_CONNECTED:
		BtlVgSmHandleAudioConnected(channel, hfgEvent->status, hfgEvent->errCode);
		break;

	case HFG_EVENT_RESPONSE_COMPLETE:
		status = BtlVgUtilsClearAtResult(hfgEvent->p.response);
		BTL_VERIFY_FATAL_NORET((BT_STATUS_SUCCESS == status), ("BtlVgUtilsClearAtResult() failed: %s", pBT_Status(status)));
		break;

	default:
		break;
	}

	BTL_FUNC_END();
}

static void BtlVgSmDisabledProcessHandsFreeEvent(BtlVgIncomingEvent *event)
{
	BtStatus status;
	BtlVgChannel *channel;
	BtlVgHfgEvent *hfgEvent;
	
	BTL_FUNC_START("BtlVgSmDisabledProcessHandsFreeEvent");

	channel = event->channel;
	hfgEvent = &event->e.hfgEvent;

	BTL_LOG_INFO(("BTL VG: %s", BtlVgUtilsHfgEventName(hfgEvent->type)));

	switch (hfgEvent->type)
	{
	case HFG_EVENT_SERVICE_CONNECT_REQ:
		BTL_LOG_ERROR(("BTL VG: SLC Reqest while disabled"));
		
		status = BtlVgUtilsRejectIncomingChannel(channel);
		BTL_VERIFY_ERR_NO_RETVAR((BT_STATUS_PENDING == status),
			("BtlVgUtilsRejectIncomingChannel() failed: %s", pBT_Status(status)))
		break;

	case HFG_EVENT_SERVICE_CONNECTED:
		BTL_LOG_ERROR(("BTL VG: SLC established while disabled"));

		/* Pass event to the BSC module */
		BtlVgSendEventToBsc(BTL_BSC_EVENT_SLC_CONNECTED, channel, &hfgEvent->p.remDev->bdAddr);
		
		BtlVgSmHandleSlcConnected(channel, hfgEvent->status, hfgEvent->errCode);
		status = BtlVgUtilsDisconnectServiceLink(channel);
		BTL_VERIFY_FATAL_NO_RETVAR((status == BT_STATUS_PENDING),
			("BtlVgUtilsDisconnectServiceLink() failed: %s", pBT_Status(status)));
		break;

	case HFG_EVENT_SERVICE_DISCONNECTED:
		BTL_LOG_INFO(("BTL VG: SLC released"));
		BtlVgSmHandleSlcDisconnected(channel, hfgEvent->status, hfgEvent->errCode);
		break;

	case HFG_EVENT_AUDIO_CONNECT_REQ:
		BTL_LOG_ERROR(("BTL VG: Audio connect request received while disabled"));
		/* Reject the audio request */
		HFG_AcceptAudioLink(channel->c.handsfreeChannel, BEC_USER_TERMINATED);
		break;

	case HFG_EVENT_AUDIO_DISCONNECTED:
		BtlVgSmHandleAudioDisconnected(channel, hfgEvent->status, hfgEvent->errCode);
		break;

	default:
		break;
	}

	BTL_FUNC_END();
}


static void BtlVgSmEnabledProcessHeadSetEvent(BtlVgIncomingEvent *event)
{
	BtStatus btStatus;
	BtlVgChannel *channel;
	BtlVgHsgEvent *hsgEvent;

	BTL_FUNC_START("BtlVgSmEnabledProcessHeadSetEvent");

	hsgEvent = &event->e.hsgEvent;
	channel = event->channel;

	BTL_LOG_INFO(("BTL VG: %s", BtlVgUtilsHsgEventName(hsgEvent->type)));
	
	switch (hsgEvent->type)
	{
	case HSG_EVENT_SERVICE_CONNECT_REQ:
		channel = BtlVgUtilsFindFreeChannel();
		if (0 != channel)
		{
			channel->type = BTL_VG_HEADSET_CHANNEL;
			channel->c.headsetChannel = hsgEvent->channel;
			channel->audioParms = CMGR_AUDIO_PARMS_SCO;
			BtlVgSmHandleSlcRequest(channel, hsgEvent->p.remDev,
				hsgEvent->status, hsgEvent->errCode);
		}
		else
		{
			BTL_LOG_ERROR(("event HSG_EVENT_SERVICE_CONNECT_REQ, no free BtlVgChannel"));
			btStatus = HSG_RejectIncomingSLC(hsgEvent->channel);
			BTL_VERIFY_ERR_NO_RETVAR((BT_STATUS_PENDING == btStatus || BT_STATUS_SUCCESS == btStatus),
				("HSG_RejectIncomingSLC() returned: %s", pBT_Status(btStatus)));
		}
		break;

	case HSG_EVENT_SERVICE_CONNECTED:
		BtlVgSmHandleSlcConnected(channel, hsgEvent->status, hsgEvent->errCode);
		break;

	case HSG_EVENT_SERVICE_DISCONNECTED:
		BtlVgSmHandleSlcDisconnected(channel, hsgEvent->status, hsgEvent->errCode);
		break;

	case HSG_EVENT_AUDIO_CONNECT_REQ:
		btStatus = HSG_AcceptAudioLink((channel->c.headsetChannel), BEC_LOCAL_TERMINATED);
		BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == btStatus), ("HSG_AcceptAudioLink failed: %s", pBT_Status(btStatus)));
		break;

	case HSG_EVENT_AUDIO_CONNECTED:
		BtlVgSmHandleAudioConnected(channel, hsgEvent->status, hsgEvent->errCode);
		break;

	case HSG_EVENT_AUDIO_DISCONNECTED:
		BtlVgSmHandleAudioDisconnected(channel, hsgEvent->status, hsgEvent->errCode);
		break;

	case HSG_EVENT_BUTTON_PRESSED:
		BtlVgSmHandleHsButtonPressed(channel);
		break;

	case HSG_EVENT_REPORT_MIC_VOLUME:
		BtlVgUtilsSendEventToUser(channel, BTL_VG_EVENT_MICROPHONE_VOLUME, hsgEvent->status, hsgEvent->errCode, (void*)hsgEvent->p.gain);
		btStatus = BtlVgUtilsSendResults(channel, AT_OK, 0, 0);
		BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == btStatus), ("BtlVgUtilsSendResults failed :%s", btStatus));
		break;

	case HSG_EVENT_REPORT_SPK_VOLUME:
		BtlVgUtilsSendEventToUser(channel, BTL_VG_EVENT_SPEAKER_VOLUME, hsgEvent->status, hsgEvent->errCode, (void*)hsgEvent->p.gain);
		btStatus = BtlVgUtilsSendResults(channel, AT_OK, 0, 0);
		BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == btStatus), ("BtlVgUtilsSendResults failed :%s", btStatus));
		break;

	case HSG_EVENT_AT_COMMAND_DATA:
		BtlVgSmHandleAtCommandData(channel, &hsgEvent->p.atString, hsgEvent->status, hsgEvent->errCode);
		break;

#if BT_SCO_HCI_DATA == XA_ENABLED
	case HSG_EVENT_AUDIO_DATA:
		/* TODO */
		break;

	case HSG_EVENT_AUDIO_DATA_SENT:
		/* TODO */
		break;
#endif

	case HSG_EVENT_RESPONSE_COMPLETE:
		btStatus = BtlVgUtilsClearAtResult(hsgEvent->p.response);
		BTL_VERIFY_FATAL_NORET((BT_STATUS_SUCCESS == btStatus), ("BtlVgUtilsClearAtResult() failed: %s", pBT_Status(btStatus)));
		break;

	default:
		BTL_FATAL_NORET(("BTL VG: Unrecognized headset event: %d",hsgEvent->type));
	}

	BTL_FUNC_END();
}

static void BtlVgSmDisablingProcessHeadSetEvent(BtlVgIncomingEvent *event)
{
	BtStatus status;
	BtlVgChannel *channel;
	BtlVgHsgEvent *hsgEvent;
	BtlVgContext *context;

	BTL_FUNC_START("BtlVgSmDisablingProcessHeadSetEvent");

	channel = event->channel;
	hsgEvent = &event->e.hsgEvent;

	BTL_LOG_INFO(("BTL VG: %s", BtlVgUtilsHsgEventName(hsgEvent->type)));
    
	switch (hsgEvent->type)
	{
	case HSG_EVENT_SERVICE_CONNECT_REQ:
		status = BtlVgUtilsRejectIncomingChannel(channel);
		BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == status),
			("BtlVgUtilsRejectIncomingChannel failed :%s", pBT_Status(status)));
		break;		

	case HSG_EVENT_SERVICE_CONNECTED:
		BtlVgSmHandleSlcConnected(channel, hsgEvent->status, hsgEvent->errCode);
		break;

	case HSG_EVENT_SERVICE_DISCONNECTED:
		BtlVgSmHandleSlcDisconnected(channel, hsgEvent->status, hsgEvent->errCode);

		context = &voiceGatewayModuleData.context;
		if (0 == context->numChannels)
		{
			BthalStatus halStatus;
			
			status = BtlVgUtilsDeregisterServices();
			BTL_VERIFY_FATAL_NO_RETVAR((BT_STATUS_SUCCESS == status),
				("BtlVgUtilsDeregisterServices() failed: %s", pBT_Status(status)));

			halStatus = BTHAL_VC_Unregister(&context->voice);
			BTL_VERIFY_ERR_NO_RETVAR((BTHAL_STATUS_PENDING == halStatus),
				("BTHAL_VC_Unregister() failed: %s", BTHAL_StatusName(halStatus)));
		}
		break;

	case HSG_EVENT_AUDIO_CONNECT_REQ:
		status = HSG_AcceptAudioLink((channel->c.headsetChannel), BEC_LOCAL_TERMINATED);
		BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == status),
			("HSG_AcceptAudioLink failed: %s", pBT_Status(status)));
		break;

	case HSG_EVENT_AUDIO_DISCONNECTED:
		BtlVgSmHandleAudioDisconnected(channel, hsgEvent->status, hsgEvent->errCode);
		break;

	case HSG_EVENT_AUDIO_CONNECTED:
		BtlVgSmHandleAudioConnected(channel, hsgEvent->status, hsgEvent->errCode);
		break;

	case HSG_EVENT_RESPONSE_COMPLETE:
		status = BtlVgUtilsClearAtResult(hsgEvent->p.response);
		BTL_VERIFY_FATAL_NORET((BT_STATUS_SUCCESS == status), ("BtlVgUtilsClearAtResult() failed: %s", pBT_Status(status)));
		break;

	default:
		break;
	}

	BTL_FUNC_END();
}

static void BtlVgSmDisabledProcessHeadSetEvent(BtlVgIncomingEvent *event)
{
	BtStatus status;
	BtlVgChannel *channel;
	BtlVgHsgEvent *hsgEvent;

	BTL_FUNC_START("BtlVgSmDisabledProcessHeadSetEvent");
	
	channel = event->channel;
	hsgEvent = &event->e.hsgEvent;

	BTL_LOG_INFO(("BTL VG: %s", BtlVgUtilsHsgEventName(hsgEvent->type)));
    
	switch (hsgEvent->type)
	{
	case HSG_EVENT_SERVICE_CONNECT_REQ:
		BTL_LOG_ERROR(("BTL VG: Headset connection request while disabled"));
		status = BtlVgUtilsRejectIncomingChannel(channel);
		BTL_VERIFY_ERR_NO_RETVAR((BT_STATUS_PENDING == status),
			("BtlVgUtilsRejectIncomingChannel() failed: %s",pBT_Status(status)));
		break;


	case HSG_EVENT_SERVICE_CONNECTED:
		BTL_LOG_ERROR(("BTL VG: Headset connection established while disabled"));
		BtlVgSmHandleSlcConnected(channel, hsgEvent->status, hsgEvent->errCode);
		status = BtlVgUtilsDisconnectServiceLink(channel);
		BTL_VERIFY_FATAL_NO_RETVAR((BT_STATUS_PENDING == status),
			("BtlVgUtilsDisconnectServiceLink() failed: %s", pBT_Status(status)));
		break;

	case HSG_EVENT_SERVICE_DISCONNECTED:
		BtlVgSmHandleSlcDisconnected(channel, hsgEvent->status, hsgEvent->errCode);
		break;

	case HSG_EVENT_RESPONSE_COMPLETE:
		status = BTL_POOL_Free(&voiceGatewayModuleData.responsesPool, (void**)&hsgEvent->p.response);
		BTL_VERIFY_FATAL_NO_RETVAR((BT_STATUS_SUCCESS == status), ("BTL_POOL_Free failed"));
		break;

	case HSG_EVENT_AUDIO_DISCONNECTED:
		BtlVgSmHandleAudioDisconnected(channel, hsgEvent->status, hsgEvent->errCode);
		break;

	default:
		break;
	}

	BTL_FUNC_END();
}

static void BtlVgSmHandleHsButtonPressed(BtlVgChannel *channel)
{
	BtStatus btStatus;
	BthalStatus halStatus;
	BtlVgContext *context;
	
	BTL_FUNC_START("BtlVgSmHandleHsButtonPressed");

	context = &voiceGatewayModuleData.context;
	switch (context->hsCallState)
	{
	case BTL_VG_HEADSET_CALL_STATE_NOCALL:
		/* TODO: Act according to headset action */
		
		btStatus = BtlVgUtilsSendResults(channel, AT_OK, 0, 0);
		BTL_VERIFY_ERR_NO_RETVAR((BT_STATUS_SUCCESS == btStatus),
			("BtlVgUtilsSendResults failed: %s", pBT_Status(btStatus)));

		if (BTL_VG_CHANNEL_STATE_CONNECTED == channel->state)
		{
			btStatus = BtlVgUtilsConnectAudio(channel);
		}
		else if (BTL_VG_CHANNEL_STATE_AUDIO_CONNECTED == channel->state)
		{
			btStatus = BtlVgUtilsDisconnectAudio(channel);
		}
		BTL_VERIFY_ERR_NO_RETVAR((BT_STATUS_PENDING == btStatus || BT_STATUS_SUCCESS == btStatus),
			("BtlVgUtilsConnectAudio failed: %s", pBT_Status(btStatus)));
		break;
		
	case BTL_VG_HEADSET_CALL_STATE_INCOMING:
		halStatus = BTHAL_MC_AnswerCall(context->modem);
		BTL_VERIFY_ERR_NO_RETVAR((BTHAL_STATUS_PENDING == halStatus),
			("BTHAL_MC_AnswerCall failed: %s", BTHAL_StatusName(halStatus)));
		break;

	case BTL_VG_HEADSET_CALL_STATE_OUTGOING:
	case BTL_VG_HEADSET_CALL_STATE_ACTIVE:
		if (BTL_VG_CHANNEL_STATE_CONNECTED == channel->state)
		{
			btStatus = BtlVgUtilsSendResults(channel, AT_OK, 0, 0);
			BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == btStatus),
				("BtlVgUtilsSendResults failed: %s", pBT_Status(btStatus)));

			btStatus = BtlVgUtilsConnectAudio(channel);
			BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == btStatus),
				("BtlVgUtilsConnectAudio failed: %s", pBT_Status(btStatus)));
		}
		else if (BTL_VG_CHANNEL_STATE_AUDIO_CONNECTED == channel->state)
		{
			halStatus = BTHAL_MC_HangupCall(context->modem);
			BTL_VERIFY_ERR_NO_RETVAR((BTHAL_STATUS_PENDING == halStatus),
				("BTHAL_MC_HangupCall failed: %s", BTHAL_StatusName(halStatus)));
		}
		break;

	default:
		BtlVgUtilsSendResults(channel, AT_ERROR, 0, 0);
		BTL_LOG_ERROR(("Undefined BtlVgHeadsetCallState value: %d", (int)context->hsCallState));
		break;
	}

	BTL_FUNC_END();
}

static void BtlVgSmEnabledProcessVoiceControlEvent(BtlVgIncomingEvent *event )
{
	const BthalVcEvent *vcEvent;

	BTL_FUNC_START("BtlVgSmEnabledProcessVoiceControlEvent");
	
	vcEvent = &event->e.vcEvent;

	BTL_LOG_INFO(("BTL VG: %s", BtlVgUtilsBthalVcEventName(vcEvent->type)));

	switch (vcEvent->type)
	{
        	case BTHAL_VC_EVENT_VOICE_PATHS_CHANGED:
        		BtlVgSmHandleVoicePathsChanged(vcEvent->p.route.path);
        		break;

        	case BTHAL_VC_EVENT_LINK_ESTABLISHED:
        		break;

        	case BTHAL_VC_EVENT_LINK_RELEASED:
        		BTL_FATAL_NORET(("Voice control link lost while enabled"));
        		break;

        	case BTHAL_VC_EVENT_VOICE_RECOGNITION_STATUS:
        		BtlVgUtilsSendEventToUser(0,
                                          BTL_VG_EVENT_VOICE_RECOGNITION_STATUS,
                                          (BtStatus)vcEvent->status,
                                          BEC_NO_ERROR,
                                          (void*)vcEvent->p.enabled);
        		BtlVgSmPassVoiceEventToChannels(vcEvent);
        		break;
                
        	case BTHAL_VC_EVENT_NOISE_REDUCTION_ECHO_CANCELLING_STATUS:
        		BtlVgSmPassVoiceEventToChannels(vcEvent);
        		break;
        	
        	default:
        		BTL_LOG_ERROR(("BTL VG: Unrecognized VC event type - %d", vcEvent->type));
        		break;
	}

	BTL_FUNC_END();
}

static void BtlVgSmDisablingProcessVoiceControlEvent(BtlVgIncomingEvent *event)
{
	BtlVgContext *context;
	const BthalVcEvent *vcEvent;
	
	BTL_FUNC_START("BtlVgSmDisablingProcessVoiceControlEvent");

	vcEvent = &event->e.vcEvent;
	context = &voiceGatewayModuleData.context;

	BTL_LOG_INFO(("BTL VG: %s", BtlVgUtilsBthalVcEventName(vcEvent->type)));
    
	switch (vcEvent->type)
	{
	case BTHAL_VC_EVENT_LINK_RELEASED:
		if (0 == context->modem && 0 == context->numChannels)
		{
			context->state = BTL_VG_CONTEXT_STATE_DISABLED;
			BtlVgUtilsSendEventToUser(0, BTL_VG_EVENT_VG_CONTEXT_DISABLED,
				BT_STATUS_SUCCESS, BEC_NO_ERROR, 0);
		}
		break;

	case BTHAL_VC_EVENT_VOICE_PATHS_CHANGED:
		/*	Voice path might be changed when disabling,
		 *	If audio connection exists the audio is needed to be routed
		 *	to the handset
		 */
		BtlVgSmHandleVoicePathsChanged(vcEvent->p.route.path);
		break;
		
	default:
		BTL_FATAL_NORET(("Received event from voice control while disabling, event: %d", (int)vcEvent->type));
	}

	BTL_FUNC_END();
}

static void BtlVgSmDisabledProcessVoiceControlEvent(BtlVgIncomingEvent *event)
{
	const BthalVcEvent *vcEvent;
	
	BTL_FUNC_START("BtlVgSmDisabledProcessVoiceControlEvent");

	vcEvent = &event->e.vcEvent;

	BTL_LOG_INFO(("BTL VG: %s", BtlVgUtilsBthalVcEventName(vcEvent->type)));
    
	switch (vcEvent->type)
	{
	case BTHAL_VC_EVENT_LINK_ESTABLISHED:
		BTL_LOG_INFO(("Event received: BTHAL_VC_EVENT_LINK_ESTABLISHED"));		
		break;

	case BTHAL_VC_EVENT_LINK_RELEASED:
		BTL_LOG_INFO(("Event received: BTHAL_VC_EVENT_LINK_RELEASED"));
		break;

	default:
		BTL_LOG_ERROR(("Received event from voice control while disabled, event: %d", (int)vcEvent->type));
		break;
	}

	BTL_FUNC_END();
}

static void BtlVgSmEnabledProcessModemControlEvent(BtlVgIncomingEvent *event)
{
	BtStatus status;
	BthalStatus halStatus;
	const BthalMcEvent *mcEvent;
	BtlVgContext *context;
	BOOL passOn = TRUE;

	BTL_FUNC_START("BtlVgSmEnabledProcessModemControlEvent");

	mcEvent = &event->e.mcEvent;
	context = &voiceGatewayModuleData.context;

	BTL_LOG_INFO(("BTL VG: %s", BtlVgUtilsBthalMcEventName(mcEvent->type)));

	switch (mcEvent->type)
	{
	case BTHAL_MC_EVENT_MODEM_LINK_ESTABLISHED:
		passOn = FALSE;
		BTL_LOG_INFO(("Event received: BTHAL_MC_EVENT_MODEM_LINK_ESTABLISHED"));
		break;

	case BTHAL_MC_EVENT_MODEM_LINK_RELEASED:
		passOn = FALSE;
		BTL_FATAL_NORET(("Modem link lost while enabled"));
		break;

	case BTHAL_MC_EVENT_INDICATOR_EVENT:
		status = BtlVgSmHeadsetSmIndicator(mcEvent->p.indicator.ind, mcEvent->p.indicator.val);
		BTL_VERIFY_ERR_NO_RETVAR((BT_STATUS_SUCCESS == status),
			("BtlVgSmHeadsetSmIndicator failed: %s", pBT_Status(status)));
		context->indicators[mcEvent->p.indicator.ind] = mcEvent->p.indicator.val;
		break;

	case BTHAL_MC_EVENT_CURRENT_INDICATORS_STATUS:
		context->indicators[HFG_IND_SERVICE] = (U8)mcEvent->p.indicatorsValue.service;
		context->indicators[HFG_IND_CALL] = (U8)mcEvent->p.indicatorsValue.call;
		context->indicators[HFG_IND_CALL_SETUP] = (U8)mcEvent->p.indicatorsValue.setup;
		context->indicators[HFG_IND_CALL_HELD] = (U8)mcEvent->p.indicatorsValue.hold;
		context->indicators[HFG_IND_BATTERY] = (U8)mcEvent->p.indicatorsValue.battery;
		context->indicators[HFG_IND_SIGNAL] = (U8)mcEvent->p.indicatorsValue.signal;
		context->indicators[HFG_IND_ROAMING] = (U8)mcEvent->p.indicatorsValue.roaming;
		break;

	case BTHAL_MC_EVENT_RING:
		halStatus = BTHAL_MC_ScheduleRingClipEvent();
		BTL_VERIFY_ERR_NORET((BTHAL_STATUS_SUCCESS == halStatus),
			("BTHAL_MC_ScheduleRingClipEvent() failed: %s", BTHAL_StatusName(halStatus)));
		break;

	default:
		break;
	}

	if (TRUE == passOn)
	{
		BtlVgSmPassModemEventToChannels(mcEvent);
	}

	BTL_FUNC_END();
}

static void BtlVgSmDisablingProcessModemControlEvent(BtlVgIncomingEvent *event)
{
	const BthalMcEvent *mcEvent;
	BtlVgContext *context;

	
	BTL_FUNC_START("BtlVgSmDisablingProcessModemControlEvent");

	mcEvent = &event->e.mcEvent;
	context = &voiceGatewayModuleData.context;

	BTL_LOG_INFO(("BTL VG: %s", BtlVgUtilsBthalMcEventName(mcEvent->type)));

	switch (mcEvent->type)
	{
	case BTHAL_MC_EVENT_MODEM_LINK_RELEASED:
		if (0 == context->voice && 0 == context->numChannels)
		{
			context->state = BTL_VG_CONTEXT_STATE_DISABLED;
			BtlVgUtilsSendEventToUser(0, BTL_VG_EVENT_VG_CONTEXT_DISABLED,
				BT_STATUS_SUCCESS, BEC_NO_ERROR, 0);
			
		}
		break;

	default:
		BTL_FATAL_NORET(("Received event from Modem Control while disabling, event: %d", (int)mcEvent->type));
	}

	BTL_FUNC_END();
}

static void BtlVgSmDisabledProcessModemControlEvent(BtlVgIncomingEvent *event)
{
	const BthalMcEvent *mcEvent;
	
	BTL_FUNC_START("BtlVgSmDisablingProcessModemControlEvent");

	mcEvent = &event->e.mcEvent;

	BTL_LOG_INFO(("BTL VG: %s", BtlVgUtilsBthalMcEventName(mcEvent->type)));

	switch (mcEvent->type)
	{
	case BTHAL_MC_EVENT_MODEM_LINK_ESTABLISHED:
		BTL_LOG_INFO(("Event received: BTHAL_MC_EVENT_MODEM_LINK_ESTABLISHED"));		
		break;

	case BTHAL_MC_EVENT_MODEM_LINK_RELEASED:
		BTL_LOG_INFO(("Event received: BTHAL_MC_EVENT_MODEM_LINK_RELEASED"));
		break;

	default:
		BTL_FATAL_NORET(("Received event from Modem Control while disabled, event: %d", (int)mcEvent->type));
	}

	BTL_FUNC_END();
}

static void BtlVgSmProcessModemEventHeadset(BtlVgChannel *channel,
											const BthalMcEvent *event)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BtlVgSmProcessModemEventHeadset");

	switch (event->type)
	{
	case BTHAL_MC_EVENT_OK:
		status = BtlVgUtilsSendResults(channel, AT_OK, 0, 0);
		break;

	case BTHAL_MC_EVENT_EXTENDED_ERROR:
	case BTHAL_MC_EVENT_ERROR:
		status = BtlVgUtilsSendResults(channel, AT_ERROR, 0, 0);
		break;

	case BTHAL_MC_EVENT_RING:
		status = BtlVgUtilsSendResults(channel, AT_RING, 0, 0);
		break;

	case BTHAL_MC_EVENT_INDICATOR_EVENT:
		status = BtlVgSmHandleIndicator(channel, event);
		break;

	default:
		break;
	}

	BTL_VERIFY_ERR_NO_RETVAR((BT_STATUS_SUCCESS == status), ("Operation failed: %s", pBT_Status(status)));

	BTL_FUNC_END();
}

static void BtlVgSmProcessModemEventHandsfree(BtlVgChannel *channel,
											  const BthalMcEvent *event)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BOOL atResponse = FALSE;
	U16 bthalMcResponseIdInc;

	BTL_FUNC_START("BtlVgSmProcessModemEventHandsfree");

    
	/* Process all modem unsolicited results (notifications) but responses only
	 * for channel which sent appropriate command */
	if ((BTHAL_MC_EVENT_RING != event->type) &&
	    (BTHAL_MC_EVENT_CALL_WAIT_NOTIFY != event->type) &&
	    (BTHAL_MC_EVENT_CALLING_LINE_ID_NOTIFY != event->type) &&
	    (BTHAL_MC_EVENT_INDICATOR_EVENT != event->type) &&
	    (BTHAL_MC_EVENT_CURRENT_INDICATORS_STATUS != event->type))
	{
        	atResponse = TRUE;

        	bthalMcResponseIdInc =
                        BtlVgUtilsIncrementCmdRspId(voiceGatewayModuleData.bthalMcResponseId);

        	BTL_LOG_DEBUG(("BTL_VG: bthalMcCommandId %d, bthalMcResponseId %d",
                                        channel->bthalMcCommandId,
                                        bthalMcResponseIdInc));
	}
    
	if ((FALSE == atResponse) || (bthalMcResponseIdInc == channel->bthalMcCommandId))
	{
        	BtlVgChannelType channelType = BtlVgUtilsGetChannelType(channel);
            
        	/* Increment response ID only, if response is a final message: OK, ERROR... */
        	if ((TRUE == atResponse) &&
        	    ((BTHAL_MC_EVENT_OK == event->type) ||
        	     (BTHAL_MC_EVENT_ERROR == event->type) |  
        	     (BTHAL_MC_EVENT_NO_CARRIER == event->type) ||  
        	     (BTHAL_MC_EVENT_BUSY == event->type) ||  
        	     (BTHAL_MC_EVENT_NO_ANSWER == event->type) ||
        	     (BTHAL_MC_EVENT_DELAYED == event->type) ||  
        	     (BTHAL_MC_EVENT_BLACKLISTED == event->type)))
        	{
                	voiceGatewayModuleData.bthalMcResponseId = bthalMcResponseIdInc;
        	}
            
        	switch (event->type)
        	{
                	case BTHAL_MC_EVENT_OK:
                		status = BtlVgUtilsSendResults(channel, AT_OK, 0, 0);
                		break;

                	case BTHAL_MC_EVENT_ERROR:
                		status = BtlVgUtilsSendResults(channel, AT_ERROR, (U32)HFG_CME_UNKNOWN, 0);
                		break;

                	case BTHAL_MC_EVENT_NO_CARRIER:
                		status = BtlVgUtilsSendResults(channel, AT_NO_CARRIER, 0, 0);
                		break;

                	case BTHAL_MC_EVENT_BUSY:
                		status = BtlVgUtilsSendResults(channel, AT_BUSY, 0, 0);
                		break;

                	case BTHAL_MC_EVENT_NO_ANSWER:
                		status = BtlVgUtilsSendResults(channel, AT_NO_ANSWER, 0, 0);
                		break;

                	case BTHAL_MC_EVENT_DELAYED:
                		status = BtlVgUtilsSendResults(channel, AT_DELAYED, 0, 0);
                		break;

                	case BTHAL_MC_EVENT_BLACKLISTED:
                		status = BtlVgUtilsSendResults(channel, AT_BLACKLISTED, 0, 0);
                		break;

                	case BTHAL_MC_EVENT_RING:
                		status = BtlVgUtilsSendResults(channel, AT_RING, 0, 0);
                		break;

                	case BTHAL_MC_EVENT_EXTENDED_ERROR:
                		status = BtlVgUtilsSendResults(channel, AT_ERROR, event->p.err, 0);
                		break;

                	case BTHAL_MC_EVENT_CALL_WAIT_NOTIFY:
                		if ((BTL_VG_HANDSFREE_CHANNEL == channelType) &&
                		    (TRUE == HFG_IsCallWaitingActive(channel->c.handsfreeChannel)))
                		{
                		    status = BtlVgSmCallWaitNotify(channel, event);	
                		}
                		break;

                	case BTHAL_MC_EVENT_CURRENT_CALLS_LIST_RESPONSE:
                		status = BtlVgSmSendCallList(channel, event);
                		break;

                	case BTHAL_MC_EVENT_CALLING_LINE_ID_NOTIFY:
                		if ((BTL_VG_HANDSFREE_CHANNEL == channelType) &&
                		    (TRUE == HFG_IsCallIdNotifyEnabled(channel->c.handsfreeChannel)))
                		{
                		    status = BtlVgSmClipNotify(channel, event);	
                		}
                		break;

                	case BTHAL_MC_EVENT_INDICATOR_EVENT:
                		if (BTL_VG_CHANNEL_STATE_CONNECTED <= channel->state)
                		{
                			BOOL sendEventToBsc = FALSE;
                			BtlBscEventType bscEvent = 0;

                			switch(event->p.indicator.ind)
                			{
                				case HFG_IND_CALL_SETUP:
                					 if (HFG_CALL_SETUP_IN == event->p.indicator.val)
                					{
                						bscEvent = BTL_BSC_EVENT_INCOMING_CALL;
                						sendEventToBsc = TRUE;
                					}
                					break;

                				case HFG_IND_CALL:
                					 if (0 == event->p.indicator.val)
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
                				BtlVgSendEventToBsc(bscEvent, channel, NULL);
                			}
                		}
                		
                		status = BtlVgSmHandleIndicator(channel, event);
                		break;

                	case BTHAL_MC_EVENT_SUBSCRIBER_NUMBER_RESPONSE:
                		status = BtlVgSmSendSubscriber(channel, event);
                		break;

                	case BTHAL_MC_EVENT_CURRENT_INDICATORS_STATUS:
                		BtlVgSmUpdateAllIndicators(channel);
                		break;

                	case BTHAL_MC_EVENT_CALL_HOLD_AND_MULTIPARTY_OPTIONS:
                	case BTHAL_MC_EVENT_INDICATORS_RANGE:
                		/* TODO */
                		break;

                	case BTHAL_MC_EVENT_NETWORK_OPERATOR_RESPONSE:
                		status = BtlVgSmSendOperator(channel, event);
                		break;

                	case BTHAL_MC_EVENT_SELECTED_PHONEBOOK:
                		status = BtlVgSmSendSelectedPhonebook(channel, event);
                		break;
                		
                	case BTHAL_MC_EVENT_SUPPORTED_PHONEBOOKS	:
                		status = BtlVgSmSendSupportedPhonebooks(channel, event);
                		break;
                		
                	case BTHAL_MC_EVENT_READ_PHONEBOOK_ENTRIES_RES:
                		status = BtlVgSmSendReadPhonebookRes(channel, event);
                		break;
                		
                	case BTHAL_MC_EVENT_TST_READ_PHONEBOOK_ENTRIES_RES:
                		status = BtlVgSmSendTstReadPhonebookRes(channel, event);
                		break;
                		
                	case BTHAL_MC_EVENT_FIND_PHONEBOOK_ENTRIES_RES:
                		status = BtlVgSmSendFindPhonebookRes(channel, event);
                		break;
                		
                	case BTHAL_MC_EVENT_SELECTED_CHAR_SET:
                		status = BtlVgSmSendSelectedCharSet(channel, event);
                		break;

#if HFG_USE_RESP_HOLD == XA_ENABLED
                	case BTHAL_MC_EVENT_READ_RESPONSE_AND_HOLD_RES:
                		status = BtlVgUtilsSendResults(channel, AT_RESPONSE_AND_HOLD,
                			HFG_RESP_HOLD_STATE_HOLD, sizeof(HfgResponseHold));
                		break;

                	case BTHAL_MC_EVENT_SET_RESPONSE_AND_HOLD_RES:
                		status = BtlVgUtilsSendResults(channel, AT_RESPONSE_AND_HOLD,
                			event->p.respHold, sizeof(HfgResponseHold));
                		break;
#endif

                	default:
                		BTL_FATAL_NO_RETVAR(("Unrecognized event: %d", event->type));

        	}
	}

	BTL_VERIFY_ERR_NO_RETVAR((BT_STATUS_PENDING == status ||
							    BT_STATUS_SUCCESS == status ||
							    BT_STATUS_NO_CONNECTION == status),
							    ("Operation failed: %s", pBT_Status(status)));

	BTL_FUNC_END();
}

static BtStatus BtlVgSmCallWaitNotify(BtlVgChannel *channel,
									  const BthalMcEvent *event)
{
	BtStatus status;
	HfgCallWaitParms parms;
	BtlVgString *vgString;

	BTL_FUNC_START(("BtlVgSmCallWaitNotify"));

	status = BtlVgUtilsBtlVgStringCreate(event->p.callwait.number, &vgString);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("BtlVgUtilsBtlVgStringCreate() failed: %s", pBT_Status(status)));
	
	parms.number = vgString->string;
	parms.classmap = event->p.callwait.classmap;
	parms.type = event->p.callwait.type;

	status = BtlVgUtilsSendResults(channel, AT_CALL_WAIT_NOTIFY, (U32)&parms, sizeof(HfgCallWaitParms));

	BTL_FUNC_END();

	return status;
}

static BtStatus BtlVgSmSendCallList(BtlVgChannel *channel,
									const BthalMcEvent *event)
{
	BtStatus status;
	HfgCallListParms parms;
	BtlVgString *vgString;

	BTL_FUNC_START("BtlVgSmSendCallList");
	
	status = BtlVgUtilsBtlVgStringCreate(event->p.calllist.number, &vgString);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("BtlVgUtilsBtlVgStringCreate() failed: %s", pBT_Status(status)));

	parms.number = vgString->string;
	parms.index = event->p.calllist.index;
	parms.dir = event->p.calllist.dir;
	parms.state = event->p.calllist.state;
	parms.mode = event->p.calllist.mode;
	parms.multiParty = event->p.calllist.multiParty;
	parms.type = event->p.calllist.type;

	status = BtlVgUtilsSendResults(channel, AT_LIST_CURRENT_CALLS, (U32)&parms, sizeof(HfgCallListParms));

	BTL_FUNC_END();

	return status;
}

static BtStatus BtlVgSmClipNotify(BtlVgChannel *channel,
								  const BthalMcEvent *event)
{
	BtStatus status;
	HfgCallerIdParms parms;
	BtlVgString *vgString;

	BTL_FUNC_START("BtlVgSmClipNotify");

	status = BtlVgUtilsBtlVgStringCreate(event->p.number.number, &vgString);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("BtlVgUtilsBtlVgStringCreate() failed: %s", pBT_Status(status)));

	parms.number = vgString->string;
	parms.type = event->p.number.type;

	status = BtlVgUtilsSendResults(channel, AT_CALL_ID, (U32)&parms, sizeof(HfgCallerIdParms));

	BTL_FUNC_END();

	return status;
}

static BtStatus BtlVgSmHandleIndicator(BtlVgChannel *channel,
									   const BthalMcEvent *event)
{
	BtStatus status = BT_STATUS_SUCCESS;
	HfgIndicator indicator;
	U8 value;

	BTL_FUNC_START("BtlVgSmHandleIndicator");

	indicator = event->p.indicator.ind;
	value = event->p.indicator.val;

	if (BTL_VG_CHANNEL_STATE_CONNECTED <= channel->state)
	{
		/* Establish or release audio according to the curret call state */
		status = BtlVgSmHandleCallAudio(channel, indicator, value);
		BTL_VERIFY_ERR((BT_STATUS_PENDING == status || BT_STATUS_SUCCESS == status),
			status, ("BtlVgSmHandleCallAudio failed: %s", pBT_Status(status)));
	}

	/* Report the indicator to the handsfree unit */
	if (BTL_VG_HANDSFREE_CHANNEL == BtlVgUtilsGetChannelType(channel))
	{
		status = BtlVgSmReportIndicator(channel, indicator, value);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status || BT_STATUS_PENDING == status),
			status, ("BtlVgReportIndicator failed: %s", pBT_Status(status)));

		/* If indicator was not sent to Hands-free device because the SLC has not
		 * been established yet, mark this indicator in order to be sent later */
		if (BT_STATUS_PENDING != status)
		{
		    HfgIndicator ind;
		    U8 mask;

		    for (ind=HFG_IND_SERVICE, mask=1; ind<=HFG_IND_ROAMING; ind++)
		    {
		        if (ind == indicator)
		        {
		            channel->handsfreeIndicatorsUpdate |= mask;
		            break;
		        }
		        else
		        {
		            mask = (U8)(mask * 2);
		        }
		    }
		}
	}
   
	BTL_FUNC_END();

	return status;
}

/*----------------------------------------------------------------------------------------
 * BtlVgSmHandleCallAudio()
 *
 *	Function handles audio connection according to the current call or call setup state.
 *	Establishing and releasing the audio link when needed.
 *	The "indicator" and "value" argument indicate the call or call setup states.
 */
static BtStatus BtlVgSmHandleCallAudio(BtlVgChannel *channel,
								  HfgIndicator indicator,
								  U8 value)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BtlVgSmHandleCallAudio");
	
	if (HFG_IND_CALL_SETUP == indicator && HFG_CALL_SETUP_OUT == value
		||
		(HFG_IND_CALL_SETUP == indicator && HFG_CALL_SETUP_IN == value && 
		 BTL_VG_HANDSFREE_CHANNEL == BtlVgUtilsGetChannelType(channel) &&
		 HFG_FEATURE_RING_TONE & HFG_GetFeatures()					&&
		 BtlVgUtilsIsInbandRingEnabled(channel)))
	{
		status = BtlVgUtilsConnectAudio(channel);
		BTL_VERIFY_ERR((BT_STATUS_PENDING == status || BT_STATUS_SUCCESS == status),
			status, ("BtlVgUtilsConnectAudio failed"));
	}
	else if (HFG_IND_CALL == indicator && TRUE == value)
	{
		if (BTL_VG_CHANNEL_STATE_AUDIO_CONNECTED != channel->state)
		{
			status = BtlVgUtilsConnectAudio(channel);
			BTL_VERIFY_ERR((BT_STATUS_PENDING == status || BT_STATUS_SUCCESS == status),
			    status, ("BtlVgUtilsConnectAudio failed"));
		}
	}
	else if (HFG_IND_CALL == indicator && FALSE == value)
	{
		if (BTL_VG_CHANNEL_STATE_AUDIO_CONNECTED == channel->state)
		{
        			status = BtlVgUtilsDisconnectAudio(channel);
        			BTL_VERIFY_ERR((BT_STATUS_PENDING == status || BT_STATUS_SUCCESS == status),	status,
        				("BtlVgUtilsDisconnectAudio failed"));
			}
		}
	else if (HFG_IND_CALL_SETUP == indicator && HFG_CALL_SETUP_NONE == value)
	{
		U8 call;
              BtlVgChannelType type;
		BtlVgContext *context = &voiceGatewayModuleData.context;

		type = BtlVgUtilsGetChannelType(channel);
		if(BTL_VG_HANDSFREE_CHANNEL == type)
		{
		status  = HFG_GetIndicatorValue(channel->c.handsfreeChannel, HFG_IND_CALL, &call);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status,
			("HFG_GetIndicatorValue failed: %s", pBT_Status(status)));
		}
		else if(BTL_VG_HEADSET_CHANNEL == type)
		{
			if(BTL_VG_HEADSET_CALL_STATE_NOCALL == context->hsCallState)
		{
				call = 0;
			}
		}
		else
			{
			BTL_VERIFY_ERR((BTL_VG_UNDEFINED_CHANNEL != type), BT_STATUS_INVALID_PARM, 
				("Invalid channel type"));
			}
                      
		if (!call && BTL_VG_CHANNEL_STATE_AUDIO_CONNECTED ==  channel->state)
		{
			/* There's no active call and audio is connected */
			status = BtlVgUtilsDisconnectAudio(channel);
			BTL_VERIFY_ERR((BT_STATUS_PENDING == status || BT_STATUS_SUCCESS == status),
				status, ("BtlVgUtilsDisconnectAudio failed"));
		}
	}
		    
	BTL_FUNC_END();

	return status;
}


static BtStatus BtlVgSmReportIndicator(BtlVgChannel *channel,
									   HfgIndicator ind,
									   U8 val)
{
	BtStatus status;
	AtResults *response;

	BTL_FUNC_START("BtlVgSmReportIndicator");

	status = BTL_POOL_Allocate(&voiceGatewayModuleData.responsesPool, (void**)&response);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), status, ("Mem alloc failed"));

	status = HFG_SetIndicatorValue(channel->c.handsfreeChannel, ind, val, response);

	if (BT_STATUS_PENDING != status)
	{
		BtStatus temp;

		temp = BTL_POOL_Free(&voiceGatewayModuleData.responsesPool, (void**)&response);
		BTL_VERIFY_FATAL_NORET((BT_STATUS_SUCCESS == temp),("Mem free failed"));
	}

	BTL_FUNC_END();

	return status;
}

static BtStatus BtlVgSmHeadsetSmIndicator(HfgIndicator ind,
											U8 val)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BtlVgContext *context = &voiceGatewayModuleData.context;

	BTL_FUNC_START("BtlVgSmHeadsetSmIndicator");

	if (ind ==  HFG_IND_CALL_SETUP)
	{
		if (HFG_CALL_SETUP_IN == val)
		{
			context->hsCallState = BTL_VG_HEADSET_CALL_STATE_INCOMING;
		}
		else if (HFG_CALL_SETUP_OUT == val)
		{
			context->hsCallState = BTL_VG_HEADSET_CALL_STATE_OUTGOING;
		}
		else if (HFG_CALL_SETUP_NONE == val && BTL_VG_HEADSET_CALL_STATE_ACTIVE != context->hsCallState)
		{
			context->hsCallState = BTL_VG_HEADSET_CALL_STATE_NOCALL;
		}
	}
	else if (ind ==  HFG_IND_CALL)
	{
		if (TRUE == val)
		{
			context->hsCallState = BTL_VG_HEADSET_CALL_STATE_ACTIVE;
		}
		else
		{
			context->hsCallState = BTL_VG_HEADSET_CALL_STATE_NOCALL;
		}
	}

	BTL_FUNC_END();

	return status;
}


static BtStatus BtlVgSmSendSubscriber(BtlVgChannel *channel,
									  const BthalMcEvent *event)
{
	BtStatus status;
	HfgSubscriberNum parms;
	BtlVgString *vgString;

	BTL_FUNC_START("BtlVgSmSendSubscriber");

	status = BtlVgUtilsBtlVgStringCreate(event->p.subscriber.number, &vgString);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("BtlVgUtilsBtlVgStringCreate() failed: %s", pBT_Status(status)));

	parms.number = vgString->string;
	parms.service = event->p.subscriber.service;
	parms.type = event->p.subscriber.type;

	status = BtlVgUtilsSendResults(channel, AT_SUBSCRIBER_NUM, (U32)&parms, sizeof(HfgSubscriberNum));

	BTL_FUNC_END();

	return status;
}

BtStatus BtlVgSmSendOperator(BtlVgChannel *channel,
							 const BthalMcEvent *event)
{
	BtStatus status;
	HfgNetworkOperator parms;
	BtlVgString *vgString;

	BTL_FUNC_START("BtlVgSmSendOperator");

	status = BtlVgUtilsBtlVgStringCreate(event->p.oper.name, &vgString);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("BtlVgUtilsBtlVgStringCreate() failed: %s", pBT_Status(status)));

	parms.oper = vgString->string;
	parms.mode = event->p.oper.mode;
	parms.format = event->p.oper.format;

	status = BtlVgUtilsSendResults(channel, AT_NETWORK_OPERATOR | AT_READ, (U32)&parms, sizeof(HfgNetworkOperator));

	BTL_FUNC_END();

	return status;
}

static BtStatus BtlVgSmSendSelectedPhonebook(BtlVgChannel *channel,
										const BthalMcEvent *event)
{
	BtStatus status;
	AtResults *response;

	BTL_FUNC_START("BtlVgSmSendSelectedPhonebook");

	status = BTL_POOL_Allocate(&voiceGatewayModuleData.responsesPool, (void**)&response);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), status, ("Mem alloc failed"));

	response->type = AT_SELECT_PHONEBOOK_STORAGE | AT_READ;
	response->p.pb.storage_read.selected = event->p.selectedPhonebook.selected;
	response->p.pb.storage_read.used = event->p.selectedPhonebook.used;
	response->p.pb.storage_read.total = event->p.selectedPhonebook.total;

	status = AtSendResults(channel->c.handsfreeChannel , response);
	
	if (BT_STATUS_PENDING != status)
	{
		BtStatus temp;

		temp = BTL_POOL_Free(&voiceGatewayModuleData.responsesPool, (void**)&response);
		BTL_VERIFY_FATAL_NORET((BT_STATUS_SUCCESS == temp),("Mem free failed"));
	}

	BTL_FUNC_END();

	return status;
}

static BtStatus BtlVgSmSendSupportedPhonebooks(BtlVgChannel *channel,
										const BthalMcEvent *event)
{
	BtStatus status;
	AtResults *response;

	BTL_FUNC_START("BtlVgSmSendSupportedPhonebooks");

	status = BTL_POOL_Allocate(&voiceGatewayModuleData.responsesPool, (void**)&response);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), status, ("Mem alloc failed"));

	response->type = AT_SELECT_PHONEBOOK_STORAGE | AT_TEST;
	response->p.pb.storage_test.supported = event->p.supportedPhonebooks;

	status = AtSendResults(channel->c.handsfreeChannel , response);
	
	if (BT_STATUS_PENDING != status)
	{
		BtStatus temp;

		temp = BTL_POOL_Free(&voiceGatewayModuleData.responsesPool, (void**)&response);
		BTL_VERIFY_FATAL_NORET((BT_STATUS_SUCCESS == temp),("Mem free failed"));
	}

	BTL_FUNC_END();

	return status;
}

static BtStatus BtlVgSmSendReadPhonebookRes(BtlVgChannel *channel,
												const BthalMcEvent *event)
{
	BtStatus status;
	AtResults *response;
	BtlVgString *number;
	BtlVgString *text;

	BTL_FUNC_START("BtlVgSmSendReadPhonebookRes");

	status = BTL_POOL_Allocate(&voiceGatewayModuleData.responsesPool, (void**)&response);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), status, ("Mem alloc failed"));

	status = BtlVgUtilsBtlVgStringCreate(event->p.phonebookEntry.number, &number);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("BtlVgUtilsBtlVgStringCreate() failed: %s", pBT_Status(status)));

	status = BtlVgUtilsBtlVgStringCreate(event->p.phonebookEntry.text, &text);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("BtlVgUtilsBtlVgStringCreate() failed: %s", pBT_Status(status)));

	response->type = AT_READ_PHONEBOOK_ENTRY;
	response->p.pb.read.index = event->p.phonebookEntry.index;
	response->p.pb.read.type = event->p.phonebookEntry.type;
	response->p.pb.read.number = number->string;
	response->p.pb.read.text = text->string;

	status = AtSendResults(channel->c.handsfreeChannel , response);
	
	if (BT_STATUS_PENDING != status)
	{
		BtStatus temp;

		temp = BTL_POOL_Free(&voiceGatewayModuleData.responsesPool, (void**)&response);
		BTL_VERIFY_FATAL_NORET((BT_STATUS_SUCCESS == temp),("Mem free failed"));

		temp = BTL_POOL_Free(&voiceGatewayModuleData.btlVgStringsPool, (void**)&number);
		BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == status), ("Mem free failed"));

		temp = BTL_POOL_Free(&voiceGatewayModuleData.btlVgStringsPool, (void**)&text);
		BTL_VERIFY_FATAL_NORET((BT_STATUS_SUCCESS == temp),("Mem free failed"));
	}

	BTL_FUNC_END();

	return status;
}


static BtStatus BtlVgSmSendFindPhonebookRes(BtlVgChannel *channel,
												const BthalMcEvent *event)
{
	BtStatus status;
	AtResults *response;
	BtlVgString *number;
	BtlVgString *text;

	BTL_FUNC_START("BtlVgSmSendFindPhonebookRes");

	status = BTL_POOL_Allocate(&voiceGatewayModuleData.responsesPool, (void**)&response);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), status, ("Mem alloc failed"));

	status = BtlVgUtilsBtlVgStringCreate(event->p.phonebookEntry.number, &number);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("BtlVgUtilsBtlVgStringCreate() failed: %s", pBT_Status(status)));

	status = BtlVgUtilsBtlVgStringCreate(event->p.phonebookEntry.text, &text);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("BtlVgUtilsBtlVgStringCreate() failed: %s", pBT_Status(status)));

	response->type = AT_FIND_PHONEBOOK_ENTRY;
	response->p.pb.read.index = event->p.phonebookEntry.index;
	response->p.pb.read.type = event->p.phonebookEntry.type;
	response->p.pb.read.number = number->string;
	response->p.pb.read.text = text->string;

	status = AtSendResults(channel->c.handsfreeChannel , response);
	
	if (BT_STATUS_PENDING != status)
	{
		BtStatus temp;

		temp = BTL_POOL_Free(&voiceGatewayModuleData.responsesPool, (void**)&response);
		BTL_VERIFY_FATAL_NORET((BT_STATUS_SUCCESS == temp),("Mem free failed"));

		temp = BTL_POOL_Free(&voiceGatewayModuleData.btlVgStringsPool, (void**)&number);
		BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == status), ("Mem free failed"));

		temp = BTL_POOL_Free(&voiceGatewayModuleData.btlVgStringsPool, (void**)&text);
		BTL_VERIFY_FATAL_NORET((BT_STATUS_SUCCESS == temp),("Mem free failed"));
	}

	BTL_FUNC_END();

	return status;
}
												

static BtStatus BtlVgSmSendSelectedCharSet(BtlVgChannel *channel,
											const BthalMcEvent *event)
{
	BtStatus status = BT_STATUS_FAILED;
	AtResults *response;
	BtlVgString *text;
	U16 len;

	BTL_FUNC_START("BtlVgSmSendSelectedCharSet");

	status = BTL_POOL_Allocate(&voiceGatewayModuleData.responsesPool, (void**)&response);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), status, ("Mem alloc failed"));

	status = BtlVgUtilsBtlVgStringCreate((const char *)event->p.charsetType, &text);
	BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, ("BtlVgUtilsBtlVgStringCreate() failed: %s", pBT_Status(status)));

	response->type = AT_SELECT_CHARACTER_SET | AT_READ;
	response->p.data = (U8 *)text->string;

	status = AtSendResults(channel->c.handsfreeChannel , response);
	
	if (BT_STATUS_PENDING != status)
	{
		BtStatus temp;

		temp = BTL_POOL_Free(&voiceGatewayModuleData.responsesPool, (void**)&response);
		BTL_VERIFY_FATAL_NORET((BT_STATUS_SUCCESS == temp),("Mem free failed"));
	}

	BTL_FUNC_END();

	return status;
}

static BtStatus BtlVgSmSendTstReadPhonebookRes(BtlVgChannel *channel,
													const BthalMcEvent *event)
{
	BtStatus status;
	AtResults *response;
	BtlVgString *range;

	BTL_FUNC_START("BtlVgSmSendTstReadPhonebookRes");

	status = BTL_POOL_Allocate(&voiceGatewayModuleData.responsesPool, (void**)&response);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), status, ("Mem alloc failed"));

	status = BTL_POOL_Allocate(&voiceGatewayModuleData.btlVgStringsPool, (void**)&range);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), status, ("Mem alloc failed"));	

	range->length = 0;
	range->length += AtItoA((U8*)range->string, event->p.phonebookData.firstIndex);
	range->string[range->length++] = '-';
	range->length += AtItoA((U8*)&range->string[range->length], event->p.phonebookData.lastIndex);
	range->string[range->length] = '\0';

	response->type = AT_READ_PHONEBOOK_ENTRY | AT_TEST;
	response->p.pb.read_test.numLength = event->p.phonebookData.numberLength;
	response->p.pb.read_test.textLength = event->p.phonebookData.textLength;
	response->p.pb.read_test.range = range->string;

	status = AtSendResults(channel->c.handsfreeChannel ,response);
	
	if (BT_STATUS_PENDING != status)
	{
		BtStatus temp;

		temp = BTL_POOL_Free(&voiceGatewayModuleData.responsesPool, (void**)&response);
		BTL_VERIFY_FATAL_NORET((BT_STATUS_SUCCESS == temp),("Mem free failed"));

		temp = BTL_POOL_Free(&voiceGatewayModuleData.btlVgStringsPool, (void**)&range);
		BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == status), ("Mem free failed"));
	}

	BTL_FUNC_END();

	return status;
}

static void BtlVgSmHandleSlcRequest(BtlVgChannel *channel,
									BtRemoteDevice *remDev,
									BtStatus status,
									BtErrorCode errCode)
{
	BtStatus btStatus;
	BtlVgContext *context;
	
	BTL_FUNC_START("BtlVgSmHandleSlcRequest");

	context = &voiceGatewayModuleData.context;

	BTL_VERIFY_ERR_NO_RETVAR((BEC_NO_ERROR == errCode), ("incoming SLC error, err code: %d",errCode));

	BtlVgSmUpdateAllIndicators(channel);	

	if (TRUE == voiceGatewayModuleData.isRadioOffNotified)
	{
		/*	The BT radio is about to shutdown,
		 *	all incoming connections are rejected.
		 */
		btStatus = BtlVgUtilsRejectIncomingChannel(channel);
		BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == status), 
			("BtlVgUtilsRejectIncomingChannel() failed: %s",pBT_Status(btStatus)));
		BTL_RET_NO_RETVAR();
	}

	++context->numChannels;
	if (1 == context->numChannels) /* This is the first connection */
	{
		btStatus = BtlVgUtilsAcceptIncomingChannel(channel, &remDev->bdAddr);
		BTL_VERIFY_ERR_NO_RETVAR((BT_STATUS_PENDING == btStatus || BT_STATUS_SUCCESS == btStatus),
			("RF_AcceptChannel failed: %s", pBT_Status(btStatus)));
		BtlVgUtilsSendEventToUser(channel, BTL_VG_EVENT_SLC_REQUEST,
			status, errCode, (void*)&remDev->bdAddr);
	}
	else
	{
		context->handoverRequest = channel;
		BtlVgUtilsSendEventToUser(channel, BTL_VG_EVENT_ADDITIONAL_SERVICE_CONNECT_REQ,
			status, errCode, &remDev->bdAddr);
	}

	BTL_FUNC_END();
}


static void BtlVgSmHandleSlcConnected(BtlVgChannel *channel,
									  BtStatus status,
									  BtErrorCode errCode)
{
	BtlVgContext *context;
	BtStatus btStatus;
	BOOL checkIndicatorsUpdate = TRUE;
	
	BTL_FUNC_START("BtlVgSmHandleSlcConnected");

	BTL_VERIFY_ERR_NO_RETVAR((BEC_NO_ERROR == errCode), ("error code is: %d",(int)errCode));

	context = &voiceGatewayModuleData.context;
	
	channel->state = BTL_VG_CHANNEL_STATE_CONNECTED;

	if (TRUE == voiceGatewayModuleData.isRadioOffNotified || BTL_VG_CONTEXT_STATE_DISABLING == context->state)
	{
		/*	Radio is about to shutdown;	terminate all established connections
		 *	Or we are in the middle of disabling the VG
		 */
		btStatus = BtlVgUtilsDisconnectServiceLink(channel);
		BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == btStatus), 
			("BtlVgUtilsDisconnectServiceLink() failed: %s", pBT_Status(btStatus)));

		checkIndicatorsUpdate = FALSE;
	}
	else if (TRUE == context->handover)
	{
		BTL_VERIFY_ERR_NO_RETVAR((0 != context->handoverSource), ("context->handoverSource is null"));
		
		if (BTL_VG_CHANNEL_STATE_AUDIO_CONNECTED == context->handoverSource->state)
		{
			status = BtlVgUtilsDisconnectAudio(context->handoverSource);
			BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == status), 
				("BtlVgUtilsDisconnectAudio returned :%s", pBT_Status(status)))
		}
		/* Check whether a call was not dropped during handover */
		else if (TRUE == context->indicators[HFG_IND_CALL])
		{
			if (0 != context->handoverTarget)
			{
        			btStatus = BtlVgUtilsConnectAudio(context->handoverTarget);
        			BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == btStatus),
        			        ("BtlVgUtilsConnectAudio() failed: %s", pBT_Status(btStatus)));
			}
			else
			{
        			BTL_LOG_ERROR(("context->handoverTarget is null"));
			}
		}
		else
		{
			status = BtlVgUtilsDisconnectServiceLink(context->handoverSource);
			BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == status || BT_STATUS_SUCCESS == status),
				("BtlVgUtilsDisconnectServiceLink returned: %s", pBT_Status(status)));
		}
	}
	else
	{
		BtlVgUtilsSendEventToUser(channel, BTL_VG_EVENT_SLC_CONNECTED, status, errCode, 0);
	}

	if (TRUE == checkIndicatorsUpdate && channel->handsfreeIndicatorsUpdate)
	{
		/* If indicator was not sent to Hands-free device because the SLC has not
		 * been established yet, sent it now */
		HfgIndicator indicator;
		U8 mask;

		for (indicator=HFG_IND_SERVICE, mask=1; indicator<=HFG_IND_ROAMING; indicator++)
		{
		    if (channel->handsfreeIndicatorsUpdate & mask)
		    {
		        BTL_LOG_INFO(("BTL VG: Update indicator changed during establishment of SLC"));
		        btStatus = BtlVgSmReportIndicator(channel, indicator, context->indicators[indicator]);
		        BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == btStatus || BT_STATUS_PENDING == btStatus),
		            ("BtlVgReportIndicator failed: %s", pBT_Status(btStatus)));
		    }
            
		    mask = (U8)(mask * 2);
		}
        
		/* Reset flags of possible Handsfree indicators update */
		channel->handsfreeIndicatorsUpdate = 0;
	}

	BTL_FUNC_END();
}

static void BtlVgSmHandleSlcDisconnected(BtlVgChannel *channel,
										 BtStatus status,
										 BtErrorCode errCode)
{
	BtlVgContext *context;
	BtStatus btStatus;
	
	BTL_FUNC_START("BtlVgSmHandleSlcDisconnected");

	context = &voiceGatewayModuleData.context;

	if (0 < context->numChannels)
	{
		--context->numChannels;
	}
	else
	{
		BTL_LOG_ERROR(("context->numChannels is invalid"));
	}
	
	/* Pass event to the BSC module */
	BtlVgSendEventToBsc(BTL_BSC_EVENT_SLC_DISCONNECTED, channel, NULL);

	channel->state = BTL_VG_CHANNEL_STATE_IDLE;
	channel->type = BTL_VG_UNDEFINED_CHANNEL;
	channel->c.channel = 0;

	/* Reset pending command IDs, which were sent to BTHAL MC and BTHAL VC, with
	 * invalid ID = 0 */
	channel->bthalMcCommandId = 0;
	channel->bthalVcCommandId = 0;

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
	else if (TRUE == context->handover)
	{
		BOOL handoverEnd = TRUE;
    
		/* If requested SLC was rejected */
		if ((TRUE == context->handoverReject) && (channel == context->handoverRequest))
		{
		    BtlVgUtilsSendEventToUser(context->handoverRequest, BTL_VG_EVENT_HANDOVER_FAILED, status, errCode, 0);
		}
		/* If  SLC to handover target was not successfully established by any reason */
		else if (channel == context->handoverTarget)
		{
		    BtlVgUtilsSendEventToUser(context->handoverTarget, BTL_VG_EVENT_HANDOVER_FAILED, status, errCode, 0);
		}
		else if (channel == context->handoverSource)
		{
		    /* SLC to handover source was dropped before we started audio to handover target */
		    if (BTL_VG_CHANNEL_STATE_CONNECTED == context->handoverTarget->state)
		    {
        		    /* Check whether a call was not dropped during handover */
        		    if (TRUE == context->indicators[HFG_IND_CALL])
        		    {
                		    btStatus = BtlVgUtilsConnectAudio(context->handoverTarget);
                		    BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == btStatus),
                			    ("BtlVgUtilsConnectAudio() failed: %s", pBT_Status(btStatus)));
                		    handoverEnd = FALSE;
        		    }
        		    else
        		    {
                		    BtlVgUtilsSendEventToUser(context->handoverTarget, BTL_VG_EVENT_HANDOVER_COMPLETED, status, errCode, 0);
        		    }
		    }
		    else if (BTL_VG_CHANNEL_STATE_AUDIO_CONNECTED == context->handoverTarget->state)
		    {
        		    BtlVgUtilsSendEventToUser(context->handoverTarget, BTL_VG_EVENT_HANDOVER_COMPLETED, status, errCode, 0);
                
        		    /* Check, if audio was established during handover procedure but a call was dropped in the middle.
        		     * In this case, remove the audio */
        		    if (FALSE == context->indicators[HFG_IND_CALL])
        		    {
        		        btStatus = BtlVgUtilsDisconnectAudio(context->handoverTarget);
        		        BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == btStatus), 
        				("BtlVgUtilsDisconnectAudio returned :%s", pBT_Status(btStatus)))
        		        handoverEnd = FALSE;
        		    }
		    }
		}

		if (TRUE == handoverEnd)
		{
        		context->handover = FALSE;
        		context->handoverSource = 0;
        		context->handoverTarget = 0;
        		context->handoverRequest = 0;
		}
	}
	else
	{
		/* It might be unanswered handover request disconnected by peer after timeout */ 
		context->handoverRequest = 0;
        
		BtlVgUtilsSendEventToUser(channel, BTL_VG_EVENT_SLC_DISCONNECTED, status, errCode, 0);
	}

	BTL_FUNC_END();
}

static void BtlVgSmHandleAtCommandData(BtlVgChannel *channel,
									   const BtlVgString *string,
									   BtStatus status,
									   BtErrorCode errCode)
{
	BtlVgAtCommand command;
	
	BTL_FUNC_START("BtlVgSmHandleAtCommandData");

	command.atCommand = string->string;
	command.commandLength = string->length;
	BtlVgUtilsSendEventToUser(channel, BTL_VG_EVENT_UNRECOGNIZED_AT_COMMAND, status, errCode, &command);

	BTL_FUNC_END();
}

static void BtlVgSmHandleAudioConnected(BtlVgChannel *channel,
										BtStatus status,
										BtErrorCode errCode)
{
	BtStatus btStatus;
	BtlVgContext *context;

	BTL_FUNC_START("BtlVgSmHandleAudioConnected");

	if ((BEC_NO_ERROR == errCode && BT_STATUS_SUCCESS == status))
	{
	    channel->state = BTL_VG_CHANNEL_STATE_AUDIO_CONNECTED;
        
	    btStatus = BtlVgUtilsRouteAudioToChannel(channel);
	    BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == btStatus), ("BtlVgUtilsRouteAudioToChannel() failed"));
	}
	else
	{
	    BTL_LOG_ERROR(("Audio connection failed: %s, error code: %x",
                                        pBT_Status(status),
                                        (unsigned)errCode));
        
	    /* Route audio to phone */
	    btStatus = BtlVgUtilsRouteAudioToGateway(channel);
	    BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == btStatus),("BtlVgRouteAudioToGateway() failed: %s", pBT_Status(btStatus)));
	}

	context = &voiceGatewayModuleData.context;

	if (TRUE == voiceGatewayModuleData.isRadioOffNotified || BTL_VG_CONTEXT_STATE_DISABLING == context->state)
	{
		if (BTL_VG_CHANNEL_STATE_AUDIO_CONNECTED == channel->state)
		{
        		btStatus = BtlVgUtilsDisconnectAudio(channel);
        		BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == btStatus), ("BtlVgUtilsDisconnectAudio() failed: %s", pBT_Status(btStatus)));
		}
		else
		{
        		btStatus = BtlVgUtilsDisconnectServiceLink(channel);
        		BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == btStatus), 
        			("BtlVgUtilsDisconnectServiceLink() failed: %s", pBT_Status(btStatus)));
		}
	}
	else if (TRUE == context->handover)
	{
		BOOL handoverEnd = TRUE;
    
		if (BTL_VG_CHANNEL_STATE_AUDIO_CONNECTED == channel->state)
		{
        		BTL_VERIFY_ERR_NO_RETVAR((channel == context->handoverTarget), ("channel != handoverTarget"));
        		BTL_VERIFY_ERR_NO_RETVAR((0 != context->handoverSource), ("context->handoverSource is null"));

        		/* SLC connection to handover source may have been dropped at this time */
        		if ((BTL_VG_UNDEFINED_CHANNEL != context->handoverSource->type) &&
                          (BTL_VG_CHANNEL_STATE_CONNECTED == context->handoverSource->state))
        		{
        		    btStatus = BtlVgUtilsDisconnectServiceLink(context->handoverSource);
        		    BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == btStatus || BT_STATUS_SUCCESS == btStatus), 
                			("BtlVgUtilsDisconnectServiceLink() failed: %s", pBT_Status(btStatus)));
                    
        		    if (BT_STATUS_PENDING == btStatus)
        		    {
        		        handoverEnd = FALSE;
        		    }
        		}
		}
        
		/* SCL to handover source was already, somehow, disconnected by some reason or
		 * audio connection establishment to handover target failed - indicate that the handover
		 * is completed and reset the handover state machine */
		if (TRUE == handoverEnd)
		{
		    BtlVgUtilsSendEventToUser(context->handoverTarget, BTL_VG_EVENT_HANDOVER_COMPLETED, status, errCode, 0);
            
		    context->handover = FALSE;
		    context->handoverSource = 0;
		    context->handoverTarget = 0;
		    context->handoverRequest = 0;
		}
	}
	else
	{
		BtlVgUtilsSendEventToUser(channel, BTL_VG_EVENT_AUDIO_CONNECTED, status, errCode, 0);
	}

	BTL_FUNC_END();
}

static void BtlVgSmHandleAudioDisconnected(BtlVgChannel *channel,
										   BtStatus status,
										   BtErrorCode errCode)
{
	BtStatus btStatus;
	BtlVgContext *context;

	BTL_FUNC_START("BtlVgSmHandleAudioDisconnected");

	channel->state = BTL_VG_CHANNEL_STATE_CONNECTED;
	context = &voiceGatewayModuleData.context;

	if (TRUE == voiceGatewayModuleData.isRadioOffNotified || BTL_VG_CONTEXT_STATE_DISABLING == context->state)
	{
		btStatus = BtlVgUtilsRouteAudioToGateway(channel);
		BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == btStatus),("BtlVgRouteAudioToGateway() failed: %s", pBT_Status(btStatus)));
		btStatus = BtlVgUtilsDisconnectServiceLink(channel);
		BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == btStatus),("BtlVgUtilsDisconnectServiceLink() failed: %s", pBT_Status(btStatus)));
	}
	else if (TRUE == context->handover)
	{
		BTL_VERIFY_ERR_NO_RETVAR((channel == context->handoverSource), ("channel != handoverSource"));
		BTL_VERIFY_ERR_NO_RETVAR((0 != context->handoverTarget), ("context->handoverTarget is null"));
        
		/* Audio may be disconnected as a result of a call drop before SLC to handover target
		 * is established */
		if (BTL_VG_CHANNEL_STATE_CONNECTED == context->handoverTarget->state)
		{
    		    /* Check whether a call was not dropped during handover */
    		    if (TRUE == context->indicators[HFG_IND_CALL])
    		    {
        		    btStatus = BtlVgUtilsConnectAudio(context->handoverTarget);
        		    BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == btStatus),
        			    ("BtlVgUtilsConnectAudio() failed: %s", pBT_Status(btStatus)));
    		    }
    		    else
    		    {
        		    btStatus = BtlVgUtilsRouteAudioToGateway(channel);
        		    BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == btStatus),("BtlVgRouteAudioToGateway failed"));
        		    btStatus = BtlVgUtilsDisconnectServiceLink(context->handoverSource);
        		    BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == btStatus), 
        			    ("BtlVgUtilsDisconnectServiceLink() failed: %s", pBT_Status(btStatus)));
    		    }
		}
	}
	else
	{
		btStatus = BtlVgUtilsRouteAudioToGateway(channel);
		BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == btStatus),("BtlVgRouteAudioToGateway failed"));
		BtlVgUtilsSendEventToUser(channel, BTL_VG_EVENT_AUDIO_DISCONNECTED, status, errCode, 0);
	}

	BTL_FUNC_END();
}


static BthalStatus BtlVgSmHandleHangup(BtlVgChannel *channel)
{
	BthalStatus halStatus;
	BtStatus btStatus;
	AtAgFeatures features;
	HfgCallSetupState setupState;
	
	BTL_FUNC_START("BtlVgSmHandleHangup");

	features = HFG_GetFeatures();
	btStatus = HFG_GetIndicatorValue(channel->c.handsfreeChannel, HFG_IND_CALL_SETUP, &setupState);
	BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == btStatus), ("HFG_GetIndicatorValue() failed: %s", pBT_Status(btStatus)));

	if ( !(features & HFG_FEATURE_REJECT) && HFG_CALL_SETUP_IN == setupState)
	{
		halStatus = BTHAL_STATUS_NOT_SUPPORTED;
		btStatus = BtlVgUtilsSendResults(channel, AT_ERROR, HFG_CME_OP_NOT_SUPPORTED ,0);
		BTL_VERIFY_ERR_NORET((BT_STATUS_PENDING == btStatus), ("BtlVgUtilsSendResults() failed: %s", pBT_Status(btStatus)));
	}
	else
	{
		halStatus = BTHAL_MC_HangupCall(voiceGatewayModuleData.context.modem);
		BTL_VERIFY_ERR_NORET((BTHAL_STATUS_PENDING == halStatus), ("BTHAL_MC_HangupCall() failed: %s", BTHAL_StatusName(halStatus)));
	}

	BTL_FUNC_END();

	return halStatus;
}

static BthalStatus BtlVgSmHandleAnswerCall(BtlVgChannel *channel)
{
	BtStatus status;
	U8  setupInd;
	BthalStatus halStatus = BTHAL_STATUS_FAILED;
	
	BTL_FUNC_START("BtlVgSmHandleAnswerCall");

	/*The code has been edited so that ATA command will answer also a 
	*  waiting call(active call exist)*/
	status = HFG_GetIndicatorValue(channel->c.handsfreeChannel, HFG_IND_CALL_SETUP, &setupInd);
	BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == status), ("HFG_GetIndicatorValue failed: %s", pBT_Status(status)));

	if (HFG_CALL_SETUP_IN == setupInd)
	{
		/* Answers a call (incoming/waiting) */
		halStatus = BTHAL_MC_AnswerCall(voiceGatewayModuleData.context.modem);
	}
	else
	{
		/* Cannot answer call */
		halStatus = BTHAL_STATUS_IMPROPER_STATE;
		status = BtlVgUtilsSendResults(channel, AT_ERROR, (U32)HFG_CME_OP_NOT_ALLOWED, 0);
		BTL_VERIFY_ERR_NORET((BT_STATUS_SUCCESS == status), ("BtlVgUtilsSendResults failed: %s", pBT_Status(status)));
	}
	
	BTL_FUNC_END();
    
	return halStatus;
}

static void BtlVgSmHandleVoicePathsChanged(BthalVcAudioPath path)
{
	BTL_FUNC_START("BtlVgSmHandleVoicePathsChanged");

	if (BTHAL_VC_AUDIO_PATH_HANDSET == path)
	{
		BTL_LOG_INFO(("BTL VG: Audio path is set to handset"));
 	}
	else if (BTHAL_VC_AUDIO_PATH_BLUETOOTH == path)
	{
		BTL_LOG_INFO(("BTL VG: Audio path is set to Bluetooth"));
 	}
	else
	{
		BTL_FATAL_NORET(("Illegal BthalVgAudioPath: %x", (unsigned)path));
	}

	BTL_FUNC_END();
}

void BtlVgSmUpdateAllIndicators(BtlVgChannel *channel)
{
	BtStatus status;
	BtlVgContext *context;

	BTL_FUNC_START("BtlVgSmUpdateAllIndicators");

	context = &voiceGatewayModuleData.context;

	if (BTL_VG_HANDSFREE_CHANNEL == BtlVgUtilsGetChannelType(channel))
	{
		status = BtlVgSmReportIndicator(channel, HFG_IND_SERVICE, context->indicators[HFG_IND_SERVICE]);
		BTL_VERIFY_FATAL_NO_RETVAR((BT_STATUS_SUCCESS == status),
			("BtlVgSmReportIndicator returned: %s", pBT_Status(status)));

		status = BtlVgSmReportIndicator(channel, HFG_IND_CALL, context->indicators[HFG_IND_CALL]);
		BTL_VERIFY_FATAL_NO_RETVAR((BT_STATUS_SUCCESS == status),
			("BtlVgSmReportIndicator returned: %s", pBT_Status(status)));

		status = BtlVgSmReportIndicator(channel, HFG_IND_CALL_SETUP, context->indicators[HFG_IND_CALL_SETUP]);
		BTL_VERIFY_FATAL_NO_RETVAR((BT_STATUS_SUCCESS == status),
			("BtlVgSmReportIndicator returned: %s", pBT_Status(status)));

		status = BtlVgSmReportIndicator(channel, HFG_IND_CALL_HELD, context->indicators[HFG_IND_CALL_HELD]);
		BTL_VERIFY_FATAL_NO_RETVAR((BT_STATUS_SUCCESS == status),
			("BtlVgSmReportIndicator returned: %s", pBT_Status(status)));

		status = BtlVgSmReportIndicator(channel, HFG_IND_BATTERY, context->indicators[HFG_IND_BATTERY]);
		BTL_VERIFY_FATAL_NO_RETVAR((BT_STATUS_SUCCESS == status),
			("BtlVgSmReportIndicator returned: %s", pBT_Status(status)));

		status = BtlVgSmReportIndicator(channel, HFG_IND_SIGNAL, context->indicators[HFG_IND_SIGNAL]);
		BTL_VERIFY_FATAL_NO_RETVAR((BT_STATUS_SUCCESS == status),
			("BtlVgSmReportIndicator returned: %s", pBT_Status(status)));

		status = BtlVgSmReportIndicator(channel, HFG_IND_ROAMING, context->indicators[HFG_IND_ROAMING]);
		BTL_VERIFY_FATAL_NO_RETVAR((BT_STATUS_SUCCESS == status),
			("BtlVgSmReportIndicator returned: %s", pBT_Status(status)));

		/* Reset flags of possible Handsfree indicators update */
		channel->handsfreeIndicatorsUpdate = 0;
        
	}

	BTL_FUNC_END();
}

static void BtlVgSmPassVoiceEventToChannels(const BthalVcEvent *event)
{
	BtStatus status;
	U32 nChannels;
	BtlVgChannel *channel;
	
	BTL_FUNC_START("BtlVgSmPassVoiceEventToChannels");

	channel = voiceGatewayModuleData.context.channels;
	for (nChannels = BTL_VG_MAXIMUM_NUMBER_OF_CHANNELS; nChannels > 0; --nChannels)
	{
		BtlVgChannelType type = BtlVgUtilsGetChannelType(channel);

		if (BTL_VG_CHANNEL_STATE_CONNECTED <= channel->state && BTL_VG_HANDSFREE_CHANNEL == type)
		{
			switch (event->type)
			{
        			case BTHAL_VC_EVENT_VOICE_RECOGNITION_STATUS:
        				status = BtlVgSmHandleVRStatusChanged(event, channel);
        				BTL_VERIFY_ERR_NO_RETVAR((BT_STATUS_PENDING == status || BT_STATUS_SUCCESS == status),
        					("BtlVgSmHandleVRStatusChanged() failed: %s", pBT_Status(status)));
        				break;

        			case  BTHAL_VC_EVENT_NOISE_REDUCTION_ECHO_CANCELLING_STATUS:
        				status = BtlVgSmHandleNRECStatusChanged(event, channel);
        				BTL_VERIFY_ERR_NO_RETVAR((BT_STATUS_PENDING == status || BT_STATUS_SUCCESS == status),
        					("BtlVgSmHandleNRECStatusChanged() failed: %s", pBT_Status(status)));
        				break;

        			default:
        				BTL_LOG_ERROR(("BtlVgSmPassVoiceEventToChannels :Received voice event: %d",(int)event->type));
        				break;
			}
		}	
		++channel;
	}

	BTL_FUNC_END();
}
	

static void BtlVgSmPassModemEventToChannels(const BthalMcEvent *event)
{
	U32 nChannels;
	BtlVgChannel *channel;
	
	BTL_FUNC_START("BtlVgSmPassModemEventToChannels");

	channel = voiceGatewayModuleData.context.channels;
	for (nChannels = BTL_VG_MAXIMUM_NUMBER_OF_CHANNELS; nChannels > 0; --nChannels)
	{
		BtlVgChannelType type = BtlVgUtilsGetChannelType(channel);

		/* Indicator's update should be saved even if a channel is not connected yet */
		if ((channel->state >= BTL_VG_CHANNEL_STATE_CONNECTED) ||
		    ((BTHAL_MC_EVENT_INDICATOR_EVENT == event->type) &&
		     (BTL_VG_HANDSFREE_CHANNEL == type)))          
		{
        		switch (type)
        		{
        		case BTL_VG_HANDSFREE_CHANNEL:
        			BtlVgSmProcessModemEventHandsfree(channel, event);
        			break;

        		case BTL_VG_HEADSET_CHANNEL:
        			BtlVgSmProcessModemEventHeadset(channel, event);
        			break;

        		case BTL_VG_UNDEFINED_CHANNEL:
        			/* Channel is not associated with an active connection, do nothing */
        			break;
        			
        		default:
        			BTL_FATAL_NO_RETVAR(("In BtlVgSmPassModemEventToChannels(): Illegal channel type: %d", (int)type));
        		}
		}
		
		++channel;
	}

	BTL_FUNC_END();
}

static BtStatus BtlVgSmHandleVRStatusChanged(const BthalVcEvent *event,
									BtlVgChannel *channel)
{
	BtStatus status = BT_STATUS_SUCCESS;
	BOOL connectAudio = FALSE;
	
	BTL_FUNC_START("BtlVgSmHandleVRStatusChanged");

	if (TRUE == channel->pendingBthalVcCommand)
	{
        	U16 bthalVcResponseIdInc =
                    BtlVgUtilsIncrementCmdRspId(voiceGatewayModuleData.bthalVcResponseId);
            
        	BTL_LOG_DEBUG(("BTL_VG: bthalVcCommandId %d, bthalVcResponseId %d",
                                         channel->bthalVcCommandId,
                                         bthalVcResponseIdInc));

		/* Process responses only for channel which sent the command */
		if (bthalVcResponseIdInc == channel->bthalVcCommandId)
		{
			voiceGatewayModuleData.bthalVcResponseId = bthalVcResponseIdInc;
            
        		if (BTHAL_STATUS_SUCCESS == event->status)
        		{
        			status = BtlVgUtilsSendResults(channel, AT_OK, 0, 0);
        			BTL_VERIFY_ERR((BT_STATUS_SUCCESS==status), status,
        				("BtlVgUtilsSendResults failed: %s", pBT_Status(status)));
        			connectAudio = TRUE;
        		}
        		else
        		{
        			status = BtlVgUtilsSendResults(channel, AT_ERROR, (U32)HFG_CME_UNKNOWN, 0);
        			BTL_VERIFY_ERR_NO_RETVAR((BT_STATUS_SUCCESS==status),
        				("BtlVgUtilsSendResults failed: %s", pBT_Status(status)));
        		}
		}
        
		channel->pendingBthalVcCommand = FALSE;
	}
	else if (BTHAL_STATUS_SUCCESS == event->status)
	{
		status = BtlVgUtilsSendResults(channel, AT_VOICE_RECOGNITION, event->p.enabled,0);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS==status), status,
			("BtlVgUtilsSendResults failed: %s", pBT_Status(status)));
		connectAudio = TRUE;
	}

	if (TRUE == connectAudio && TRUE == event->p.enabled)
	{
		status = BtlVgUtilsConnectAudio(channel);
		BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status || BT_STATUS_PENDING == status),
			status, ("BtlVgUtilsConnectAudio() failed: %s", pBT_Status(status)));
	}
	
	BTL_FUNC_END();

	return status;
}

static BtStatus BtlVgSmHandleNRECStatusChanged(const BthalVcEvent *event, 
											BtlVgChannel *channel)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BtlVgSmHandleNRECStatusChanged");

	if (TRUE == channel->pendingBthalVcCommand)
	{
        	U16 bthalVcResponseIdInc =
                    BtlVgUtilsIncrementCmdRspId(voiceGatewayModuleData.bthalVcResponseId);
            
        	BTL_LOG_DEBUG(("BTL_VG: bthalVcCommandId %d, bthalVcResponseId %d",
                                         channel->bthalVcCommandId,
                                         bthalVcResponseIdInc));

		/* Process responses only for channel which sent the command */
		if (bthalVcResponseIdInc == channel->bthalVcCommandId)
		{
			voiceGatewayModuleData.bthalVcResponseId = bthalVcResponseIdInc;
            
        		if (BTHAL_STATUS_SUCCESS == event->status)
			{
        			status = BtlVgUtilsSendResults(channel, AT_OK, 0, 0);
        			BTL_VERIFY_ERR((BT_STATUS_SUCCESS==status), status,
        				("BtlVgUtilsSendResults failed: %s", pBT_Status(status)));
        		}
        		else
        		{
        			status = BtlVgUtilsSendResults(channel, AT_ERROR, (U32)HFG_CME_UNKNOWN, 0);
        			BTL_VERIFY_ERR_NO_RETVAR((BT_STATUS_SUCCESS == status),
        				("BtlVgUtilsSendResults failed: %s", pBT_Status(status)));
        		}
		}
        
		channel->pendingBthalVcCommand = FALSE;
	}
	
	BTL_FUNC_END();

	return status;
}


/*-------------------------------------------------------------------------------
 * BtlVgSendEventToBsc()
 *
 *		Sends event to BTL_BSC module.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		eventType [in] - type of event to be sent.
 *
 *      vgChannel [in] - pointer to BtlVgChannel structure.
 *
 *      bdAddr [in] - pointer to BD_ADDR structure or NULL.
 *
 * Returns:
 *		none
 */
static void BtlVgSendEventToBsc(BtlBscEventType eventType,
                                BtlVgChannel *vgChannel,
                                BD_ADDR *bdAddr)
{
	BtlBscEvent bscEvent;

	BTL_FUNC_START("BtlVgSendEventToBsc");

	/* Fill event's data */
	bscEvent.type = eventType;
	bscEvent.source = BTL_BSC_EVENT_SOURCE_VG;
	bscEvent.info.channel = (U32)vgChannel;
	bscEvent.isBdAddrValid = TRUE;
        
	if (BTL_VG_HANDSFREE_CHANNEL == BtlVgUtilsGetChannelType(vgChannel))
	{
		if (NULL != bdAddr)
		{
			bscEvent.info.bdAddr = *bdAddr;
		}
		else if (NULL != vgChannel->c.handsfreeChannel->cmgrHandler.bdc)
		{
			bscEvent.info.bdAddr = vgChannel->c.handsfreeChannel->cmgrHandler.bdc->link->bdAddr;
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
		else if (NULL != vgChannel->c.headsetChannel->cmgrHandler.bdc)
		{
			bscEvent.info.bdAddr = vgChannel->c.headsetChannel->cmgrHandler.bdc->link->bdAddr;
		}
		else
		{
			bscEvent.isBdAddrValid = FALSE;
		}
	}
    
	BTL_BSC_SendEvent((const BtlBscEvent *)&bscEvent);

	BTL_FUNC_END();
}


#endif /*BTL_CONFIG_VG == BTL_CONFIG_ENABLED*/


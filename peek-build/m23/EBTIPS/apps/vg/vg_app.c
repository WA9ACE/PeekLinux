/*******************************************************************************\
*                                                                       		*
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
*   FILE NAME:      vg_app.c
*
*   DESCRIPTION:	Sample application for the VG Module.
*
*   AUTHOR:         Itay Klein
*
\*******************************************************************************/
#include "btl_config.h"
#include "osapi.h"
#if BTL_CONFIG_VG == BTL_CONFIG_ENABLED

#include <stdlib.h>
#include "conmgr.h"
#include "btl_vg.h"
#include "debug.h"
#include "lineparser.h"
#include "hfg.h"



typedef BOOL (*VgFuncType)(void);

typedef struct _VgCommand
{
	char	funcName[LINE_PARSER_MAX_STR_LEN];
	
	VgFuncType funcPtr;
} VgCommand;

typedef struct _VgEnableFuncParameter
{
	char name[LINE_PARSER_MAX_STR_LEN];

	BOOL value;
} VgEnableFuncParameter;

static BOOL VG_APP_Init(void);
static BOOL VG_APP_Deinit(void);
static BOOL VG_APP_Create(void);
static BOOL VG_APP_Destroy(void);
static BOOL VG_APP_Enable(void);
static BOOL VG_APP_Disable(void);
static BOOL VG_APP_Connect(void);
static BOOL VG_APP_Disconnect(void);
static BOOL VG_APP_ConnectAudio(void);
static BOOL VG_APP_DisconnectAudio(void);
static BOOL VG_APP_EnableInbandRing(void);
static BOOL VG_APP_IsInbandRingEnabled(void);
static BOOL VG_APP_SendMicVolume(void);
static BOOL VG_APP_SendSpkVolume(void);
static BOOL VG_APP_SetSecurityLevel(void);
static BOOL VG_APP_GetSecurityLevel(void);
static BOOL VG_APP_PerformHandover(void);
static BOOL VG_APP_RejectHandover(void);
static BOOL VG_APP_AttachNumber(void);
static BOOL VG_APP_RejectAttachNumber(void);
static BOOL VG_APP_EnableVoiceRecognition(void);
#if (0)
static BOOL VG_APP_EnableFmOverBt(void);
#endif

static void VGA_Callback(const BtlVgEvent *event);

static BOOL VgAppSendVolume(BtStatus (*sendVolFunc)(const BtlVgContext*, BtlVgChannelId, U32));
static BOOL VG_APP_SendBatteryStatus(void);
static const char *VgAppBtlVgEventName(BtlVgEventType event);

extern int  BthalMcSendBatteryStatus(U32 batteryStatus);

static const VgCommand vgAppCommands[] = 
{
	{"init", VG_APP_Init},
	{"Deinit", VG_APP_Deinit},
	{"create", VG_APP_Create},
	{"destroy", VG_APP_Destroy},
	{"enable", VG_APP_Enable},
	{"disable", VG_APP_Disable},
	{"connect", VG_APP_Connect},
	{"disconnect", VG_APP_Disconnect},
	{"enable_inbandring", VG_APP_EnableInbandRing},
	{"is_inbandring_enabled", VG_APP_IsInbandRingEnabled},
	{"send_mic_volume", VG_APP_SendMicVolume},
	{"send_spk_volume", VG_APP_SendSpkVolume},
	{"connect_audio", VG_APP_ConnectAudio},
	{"disconnect_audio", VG_APP_DisconnectAudio},
	{"set_security", VG_APP_SetSecurityLevel},
	{"get_security", VG_APP_GetSecurityLevel},
	{"perform_handover", VG_APP_PerformHandover},
	{"reject_handover", VG_APP_RejectHandover},
	{"attach_number",VG_APP_AttachNumber},
	{"reject_attach_number", VG_APP_RejectAttachNumber},
	{"voice_rec", VG_APP_EnableVoiceRecognition},
	{"send_battery_status", VG_APP_SendBatteryStatus}
#if (0)
	,
	{"enable_fm_over_bt", VG_APP_EnableFmOverBt}
#endif
};

static const VgEnableFuncParameter enableParameters[] =
{
	{"enable", TRUE},
	{"disable", FALSE}
};

static const BtSecurityLevel noSecurity = BSL_NO_SECURITY;
static const BtlVgAudioSourcesAction noActions = {0};
static AtAgFeatures features =	HFG_FEATURE_REJECT |
							HFG_FEATURE_RING_TONE |
							HFG_FEATURE_THREE_WAY_CALLS |
							HFG_FEATURE_ENHANCED_CALL_STATUS |
							HFG_FEATURE_EXTENDED_ERRORS |
							HFG_FEATURE_ECHO_NOISE |
							HFG_FEATURE_VOICE_RECOGNITION |
							HFG_FEATURE_VOICE_TAG |
							HFG_FEATURE_ENHANCED_CALL_CTRL;

static BOOL closingVoiceGateway = FALSE;

static BtlVgChannelId channelId = BTL_VG_CHANNEL_ID_NONE;
static BtlVgContext *context =0;

#define VG_APP_NUM_OF_COMMANDS (sizeof(vgAppCommands) /sizeof(VgCommand))
#define VG_APP_ENABLE_PARAM_NUM_OF_VALS (sizeof (enableParameters) / sizeof(VgEnableFuncParameter))
#define VG_APP_AT_STRING_LEN 100

void VGA_Init(void)
{
	if (BT_STATUS_SUCCESS != BTL_VG_Create(0, VGA_Callback, &noSecurity, &context))
	{
		Report(("VG APP: BTL_VG_Create failed"));
	}

	if (BT_STATUS_PENDING != BTL_VG_Enable(context, 0, &features, &noActions))
	{
		Report(("VG APP: BTL_VG_Enable failed"));
	}
}

void VGA_Deinit(void)
{

	BtStatus status;

	status = BTL_VG_Disable(context);
	if (BT_STATUS_SUCCESS == status)
	{
		status = BTL_VG_Destroy(&context);
		Report(("BTL_VG_Destroy() returned: %s", pBT_Status(status)));
	}
	else if (BT_STATUS_PENDING == status)
	{
		closingVoiceGateway = TRUE;
	}
	else
	{
		Report(("BTL_VG_Disable() failed: %s", pBT_Status(status)));
	}
}

void VGA_ProcessUserAction(U8 *msg)
{
	U32 idx;
	char command[LINE_PARSER_MAX_STR_LEN];
	LINE_PARSER_STATUS status;
	BOOL success = FALSE;

	status = LINE_PARSER_GetNextStr((U8*)command, LINE_PARSER_MAX_STR_LEN);
	if (LINE_PARSER_STATUS_SUCCESS != status)
	{
		Report(("VG APP: command parsing failed"));
		return;
	}

	for(idx = 0; idx < VG_APP_NUM_OF_COMMANDS; ++idx)
	{
		if (0 == OS_StrCmp(command, vgAppCommands[idx].funcName))
		{
			success = vgAppCommands[idx].funcPtr();
		}
	}
	if (FALSE == success)
	{
		Report(("VG APP: user action failed"));
	}
}

static void VGA_Callback(const BtlVgEvent *event)
{
	char str[VG_APP_AT_STRING_LEN];
	U32 strLen;
	char bdAddrStr[100];
	BtStatus status;
	

	Report(("VG APP: %s", VgAppBtlVgEventName((int)event->type)));
	
	switch (event->type)
	{
	case BTL_VG_EVENT_SLC_REQUEST:
		Report(("VG APP: SLC request from device: %s", bdaddr_ntoa(event->p.bdAddr, bdAddrStr)));
		channelId = event->channelId;
		break;

	case BTL_VG_EVENT_ADDITIONAL_SERVICE_CONNECT_REQ:
		Report(("VG APP: Addtional SLC request received from device: %s", bdaddr_ntoa(event->p.bdAddr, bdAddrStr)));
		break;

	case BTL_VG_EVENT_SLC_CONNECTED:
		if (channelId != event->channelId)
		{
			Report(("VG APP: app's channel id is different form the event's channel Id"));
		}
		else 
		{
			Report(("VG APP: Connection established"));
			/* turn off inband ring tone */
			if (BT_STATUS_SUCCESS != BTL_VG_EnableInbandRingTone(context, event->channelId, FALSE))
			{
				Report(("VG_APP: Error disabling inband ring tone after SLC established"));
			}
		}
		break;

	case BTL_VG_EVENT_SLC_DISCONNECTED:
		if (event->channelId == channelId)
		{
			channelId = BTL_VG_CHANNEL_ID_NONE;
			Report(("VG APP: Connection terminated"));
		}
		break;
		
	case BTL_VG_EVENT_HANDOVER_COMPLETED:
		channelId = event->channelId;
		Report(("VG APP: Handover completed"));
		break;
		
	case BTL_VG_EVENT_HANDOVER_FAILED:
		Report(("VG APP: Handover failed"));
		break;
		
	case BTL_VG_EVENT_AUDIO_CONNECTED:
		Report(("VG APP: Audio connection established"));
		break;

	case BTL_VG_EVENT_AUDIO_DISCONNECTED:
		Report(("VG APP: Audio conenction disconnected"));
		break;

	case BTL_VG_EVENT_MICROPHONE_VOLUME:
		Report(("VG APP: microphone volume: %u", event->p.volume));
		break;

	case BTL_VG_EVENT_SPEAKER_VOLUME:
		Report(("VG APP: speaker volume: %u", event->p.volume));
		break;

	case BTL_VG_EVENT_PHONE_NUMBER_REQUESTED:
		Report(("VG APP: A number for a voice tag is requested"));
		break;

	case BTL_VG_EVENT_VOICE_RECOGNITION_STATUS:
		Report(("VG APP: voice recognition is :%s", (event->p.enabled ? "enabled" : "disabled")));
		/*In case, the process of outgoing call establishing has been successfully started, 
		audio connection may be kept in order to be used in this call. At present audio is 
		disconnected on disabling voice recognition indication is received*/
		if(!event->p.enabled)
		{
			status = BTL_VG_DisconnectAudio(context, channelId);
			if(BT_STATUS_PENDING != status && BT_STATUS_SUCCESS != status)
			{
				Report(("VG APP: BTL_VG_DisconnectAudio failed"));
			}
		}
		break;
		
#if HFG_FEATURE_RING_TONE & HFG_SDK_FEATURES

	case BTL_VG_EVENT_INBAND_RING_STATUS:
		Report(("VG APP: inband ringing is: %s", (event->p.enabled ? "enabled" : "disabled")));
		break;
#endif

	case BTL_VG_EVENT_UNRECOGNIZED_AT_COMMAND:
		strLen = event->p.atCommand->commandLength;
		if (strLen > LINE_PARSER_MAX_STR_LEN - 1 )
		{
			Report(("VG APP: AT Command string is too long"));
			strLen = LINE_PARSER_MAX_STR_LEN - 1;
		}
		OS_MemCopy((U8*)str, (const U8*) event->p.atCommand->atCommand, strLen);
		str[strLen] = '\0';
		Report(("VG APP: unrecognized AT command: %s", str));
		if (BT_STATUS_SUCCESS != BTL_VG_SentAtError(event->context, event->channelId, ATCME_OP_NOT_SUPPORTED))
		{
			Report(("VG APP: BTL_VG_SentAtError failed"));
		}
		break;
		
	case BTL_VG_EVENT_VG_CONTEXT_ENABLED:
		Report(("VG APP: VG is enabled"));
		break;

	case BTL_VG_EVENT_VG_CONTEXT_DISABLED:
		Report(("VG APP: VG is disabled"));
		if (TRUE == closingVoiceGateway)
		{
			BtStatus status;

			status = BTL_VG_Destroy(&context);
			Report(("BTL_VG_Destroy returned: %s", pBT_Status(status)));
			closingVoiceGateway = FALSE;
		}
		break;

	default:
		Report(("VG APP: Unexpected VG event: %u", event->type));
		break;
	}
	
}

static BOOL VG_APP_Init(void)
{
	return BT_STATUS_SUCCESS == BTL_VG_Init();
}

static BOOL VG_APP_Deinit(void)
{
	return BT_STATUS_SUCCESS == BTL_VG_Deinit();
}

static BOOL VG_APP_Create(void)
{
	return BT_STATUS_SUCCESS == BTL_VG_Create(0,VGA_Callback, &noSecurity, &context);
}

static BOOL VG_APP_Destroy(void)
{
	return BT_STATUS_SUCCESS == BTL_VG_Destroy(&context);
}

static BOOL VG_APP_Enable(void)
{
	return BT_STATUS_PENDING == BTL_VG_Enable(context, 0, &features, &noActions);
}

static BOOL VG_APP_Disable(void)
{
	return BT_STATUS_PENDING == BTL_VG_Disable(context);
}

static BOOL VG_APP_Connect(void)
{
	BtStatus btStatus;
	BD_ADDR addr;
	char addrString[LINE_PARSER_MAX_STR_LEN];
	LINE_PARSER_STATUS parserStatus;

	parserStatus = LINE_PARSER_GetNextStr((U8*)addrString, LINE_PARSER_MAX_STR_LEN);
	if (LINE_PARSER_STATUS_SUCCESS != parserStatus)
	{
		Report(("VG APP: BD Address parsing failed"));
		return FALSE;
	}
	addr = bdaddr_aton(addrString);

	btStatus = BTL_VG_ConnectAndGetChannelId(context, &addr, BTL_VG_AUDIO_SOURCE_MODEM, &channelId);

	if (BT_STATUS_IN_USE == btStatus)
	{
		Report(("VG APP: An SLC to a remote device already exists"));
		Report(("VG APP: Disconnect or perform handover to connect to a new device"));
	}

	return BT_STATUS_PENDING == btStatus;
}

static BOOL VG_APP_Disconnect(void)
{
	BtStatus btStatus = BTL_VG_Disconnect(context, channelId);
    
	/* In case of cancellation during ACL connection establishment, BT_STATUS_SUCCESS
	 * may be returned */
	return ((BT_STATUS_SUCCESS == btStatus) || (BT_STATUS_PENDING == btStatus));
}

static BOOL VG_APP_EnableInbandRing(void)
{
	U32 idx;
	char param[LINE_PARSER_MAX_STR_LEN];
	BOOL enable = FALSE;
	LINE_PARSER_STATUS parserStatus;

	parserStatus = LINE_PARSER_GetNextStr((U8*)param, LINE_PARSER_MAX_STR_LEN);
	if (LINE_PARSER_STATUS_SUCCESS != parserStatus)
	{
		Report(("VG APP: failed to parse parameter"));
		return FALSE;
	}

	for (idx = 0; idx < VG_APP_ENABLE_PARAM_NUM_OF_VALS; ++idx)
	{
		if (0 == OS_StrCmp(param, enableParameters[idx].name))
		{
			enable = enableParameters[idx].value;
			break;
		}
	}
	if (idx == VG_APP_ENABLE_PARAM_NUM_OF_VALS)
	{
		Report(("VG APP: enable_inband ring - illegal parameter: %s", param));
		return FALSE;
	}

	return (BT_STATUS_SUCCESS == BTL_VG_EnableInbandRingTone(context, channelId, enable));
}

static BOOL VG_APP_IsInbandRingEnabled(void)
{
	BOOL enabled;

	if (BT_STATUS_SUCCESS != BTL_VG_IsInbandRingToneEnabled(context, channelId, &enabled))
	{
		Report(("VG APP: BTL_VG_IsInbandRingToneEnabled failed"));
		return FALSE;
	}

	if (TRUE == enabled)
	{
		Report(("VG APP: Inband ring tone is enabled"));
	}
	else
	{
		Report(("VG APP: Inband ring tone is disabled"));
	}

	return TRUE;
}

static BOOL VG_APP_SendMicVolume(void)
{
	return VgAppSendVolume(BTL_VG_SendMicrophoneVolume);
}

static BOOL VG_APP_SendSpkVolume(void)
{
	return VgAppSendVolume(BTL_VG_SendSpeakerVolume);	
}

static BOOL VgAppSendVolume(BtStatus (*sendVolFunc)(const BtlVgContext*, BtlVgChannelId, U32))
{
	char param[LINE_PARSER_MAX_STR_LEN];
	U32 vol;
	LINE_PARSER_STATUS parserStatus;

	parserStatus = LINE_PARSER_GetNextStr((U8*)param, LINE_PARSER_MAX_STR_LEN);
	if (LINE_PARSER_STATUS_SUCCESS != parserStatus)
	{
		Report(("VG APP: failed to parse parameter"));
		return FALSE;
	}

	vol = atoi(param);

	return BT_STATUS_SUCCESS == sendVolFunc(context, channelId, vol);
}

static BOOL VG_APP_ConnectAudio(void)
{
	BtStatus status;

 	status = BTL_VG_ConnectAudio(context, channelId);

	return (BT_STATUS_PENDING == status || BT_STATUS_SUCCESS == status);
}

static BOOL VG_APP_DisconnectAudio(void)
{
	BtStatus status;

	status = BTL_VG_DisconnectAudio(context, channelId);

	return (BT_STATUS_PENDING == status || BT_STATUS_SUCCESS == status);
}

static BOOL VG_APP_SetSecurityLevel(void)
{
	BtStatus btStatus;
	BtSecurityLevel secLevel;
	LINE_PARSER_STATUS parserStatus;

	parserStatus= LINE_PARSER_GetNextU8(&secLevel, FALSE);
	
	if (LINE_PARSER_STATUS_SUCCESS != parserStatus)
	{
		Report(("VG APP: LINE_PARSER_GetNextU8 failed, LINE_PARSER_STATUS: %d ", parserStatus));
		return FALSE;
	}

	btStatus = BTL_VG_SetSecurityLevel(context, &secLevel);

	Report(("BTL_VG_SetSecurityLevel() returned %s.", pBT_Status(btStatus)));

	return BT_STATUS_SUCCESS == btStatus;
}

static BOOL VG_APP_AttachNumber(void)
{
	BtStatus status;
	char number[LINE_PARSER_MAX_STR_LEN];
	LINE_PARSER_STATUS parserStatus;

	parserStatus = LINE_PARSER_GetNextStr((U8*)number, LINE_PARSER_MAX_STR_LEN);
	if (LINE_PARSER_STATUS_SUCCESS != parserStatus)
	{
		Report(("VG APP: Failed to parse number"));
		return FALSE;
	}
	status = BTL_VG_AcceptAttachNumber(context, channelId, number);
	Report(("BTL_VG_AcceptAttachNumber() returned: %s", pBT_Status(status)));

	return BT_STATUS_PENDING == status;
}

static BOOL VG_APP_RejectAttachNumber(void)
{
	BtStatus status;
	
	Report(("VG APP: Reject request for a number to be attached for a voice tag"));

	status = BTL_VG_RejectAttachNumber(context, channelId);

	Report(("BTL_VG_RejectAttachNumber() returned: %s", pBT_Status(status)));

	return BT_STATUS_PENDING == status;
}


static BOOL VG_APP_PerformHandover(void)
{
	BtStatus btStatus;
	BD_ADDR addr;
	char addrString[LINE_PARSER_MAX_STR_LEN];
	LINE_PARSER_STATUS parserStatus;

	Report(("VG APP: performing handover"));

	Report(("VG APP: getting the BD Addr string"));

	parserStatus = LINE_PARSER_GetNextStr((U8*)addrString, LINE_PARSER_MAX_STR_LEN);
	if (LINE_PARSER_STATUS_SUCCESS != parserStatus)
	{
		Report(("VG APP: BD Address parsing failed"));
		return FALSE;
	}
	addr = bdaddr_aton(addrString);

	Report(("VG APP: doing handover from channel ID:%d to address:%s", (int)channelId, addrString));

	btStatus = BTL_VG_PerformHandover(context, channelId, &addr, TRUE);
	Report(("BTL_VG_PerformHandover returned: %s", pBT_Status(btStatus)));
	
	return BT_STATUS_PENDING == btStatus;
}

static BOOL VG_APP_RejectHandover(void)
{
	Report(("VG APP: rejecting handover"));
    
	return BT_STATUS_PENDING == BTL_VG_PerformHandover(context, BTL_VG_CHANNEL_ID_NONE, 0, FALSE);
}

static BOOL VG_APP_EnableVoiceRecognition(void)
{
	BtStatus status;
	char str[LINE_PARSER_MAX_STR_LEN];
	BOOL enable = FALSE;

	if (LINE_PARSER_STATUS_SUCCESS != LINE_PARSER_GetNextStr((U8*)str, LINE_PARSER_MAX_STR_LEN))
	{
		Report(("VG APP: LINE_PARSER_GetNextStr() failed"));
	}

	if (0 == strcmp(str, "enable"))
	{
		enable = TRUE;
	}
	else if (0 == strcmp(str,"disable"))
	{
		enable = FALSE;
	}
	else
	{
		Report(("VG APP: illegal string parameter"));
		return FALSE;
	}

	status = BTL_VG_EnableVoiceRecognition(context, channelId, enable);

	return BT_STATUS_PENDING == status;
}

#if (0)
static BOOL VG_APP_EnableFmOverBt(void)
{
	BtStatus status;
	char str[LINE_PARSER_MAX_STR_LEN];
	BOOL enable = FALSE;

	if (LINE_PARSER_STATUS_SUCCESS != LINE_PARSER_GetNextStr((U8*)str, LINE_PARSER_MAX_STR_LEN))
	{
		Report(("VG APP: LINE_PARSER_GetNextStr() failed"));
	}

	if (0 == strcmp(str, "enable"))
	{
		enable = TRUE;
	}
	else if (0 == strcmp(str,"disable"))
	{
		enable = FALSE;
	}
	else
	{
		Report(("VG APP: illegal string parameter"));
		return FALSE;
	}

	status = BTL_VG_SetFmOverBtMode(context, channelId, enable);

	return (BT_STATUS_PENDING == status);
}
#endif /* BTL_CONFIG_FM_STACK == BTL_CONFIG_ENABLED */

static BOOL VG_APP_GetSecurityLevel(void)
{
	BtStatus status;
	BtSecurityLevel secLevel;

	status = BTL_VG_GetSecurityLevel(context, &secLevel);
	Report(("VG APP: BTL_VG_GetSecurityLevel() returned: %s", pBT_Status(status)));
	if ( BT_STATUS_SUCCESS != status)
	{
		return FALSE;
	}
	switch (secLevel)
	{
	case 0:
		Report(("VG APP: Security level: BSL_NO_SECURITY"));
		break;

	case 1:
		Report(("VG APP: Security level: BSL_AUTHENTICATION_IN"));
		break;

	case 2:
		Report(("VG APP: Security level: BSL_AUTHORIZATION_IN"));
		break;

	case 4:
		Report(("VG APP: Security level: BSL_ENCRYPTION_IN"));
		break;

		case 3:
		Report(("VG APP: Security level: BSL_AUTHENTICATION_IN and BSL_AUTHORIZATION_IN"));
		break;

	case 5:
		Report(("VG APP: security level: BSL_AUTHENTICATION_IN and BSL_ENCRYPTION_IN"));
		break;

	case 6:
		Report(("VG APP: Security level: BSL_AUTHORIZATION_IN and BSL_ENCRYPTION_IN"));
		break;

	case 7:
		Report(("VG APP: Security level: BSL_AUTHENTICATION_IN"));
		Report(("and BSL_AUTHORIZATION_IN"));
		Report(("and BSL_ENCRYPTION_IN"));
		break;

	default:
		Report(("VG APP: Error: Wrong security level"));
		break;
	}

	return TRUE;
}

static BOOL VG_APP_SendBatteryStatus(void)
{
	char param[LINE_PARSER_MAX_STR_LEN];
	U32 batStatus;
	BtStatus status;
	LINE_PARSER_STATUS parserStatus;

	parserStatus = LINE_PARSER_GetNextStr((U8*)param, LINE_PARSER_MAX_STR_LEN);
	if (LINE_PARSER_STATUS_SUCCESS != parserStatus)
	{
		Report(("VG APP: failed to parse parameter"));
		return FALSE;
	}
	
	batStatus = atoi(param);
	if(batStatus > 5) /*Battery charge value ranges from 0 to 5*/
	{
		return FALSE;
	}
	
	status = BthalMcSendBatteryStatus(batStatus);

	return status;	
}

static const char *VgAppBtlVgEventName(BtlVgEventType event)
{
    const char *eventName;

    switch(event)
    {
        case BTL_VG_EVENT_SLC_REQUEST:
            eventName = "BTL_VG_EVENT_SLC_REQUEST";
            break;
        case BTL_VG_EVENT_SLC_CONNECTED:
            eventName = "BTL_VG_EVENT_SLC_CONNECTED";
            break;
        case BTL_VG_EVENT_SLC_DISCONNECTED:
            eventName = "BTL_VG_EVENT_SLC_DISCONNECTED";
            break;
        case BTL_VG_EVENT_AUDIO_CONNECTED:
            eventName = "BTL_VG_EVENT_AUDIO_CONNECTED";
            break;
        case BTL_VG_EVENT_AUDIO_DISCONNECTED:
            eventName = "BTL_VG_EVENT_AUDIO_DISCONNECTED";
            break;
        case BTL_VG_EVENT_PHONE_NUMBER_REQUESTED:
            eventName = "BTL_VG_EVENT_PHONE_NUMBER_REQUESTED";
            break;
        case BTL_VG_EVENT_MICROPHONE_VOLUME:
            eventName = "BTL_VG_EVENT_MICROPHONE_VOLUME";
            break;
        case BTL_VG_EVENT_SPEAKER_VOLUME:
            eventName = "BTL_VG_EVENT_SPEAKER_VOLUME";
            break;
        case BTL_VG_EVENT_VOICE_RECOGNITION_STATUS:
            eventName = "BTL_VG_EVENT_VOICE_RECOGNITION_STATUS";
            break;
        case BTL_VG_EVENT_INBAND_RING_STATUS:
            eventName = "BTL_VG_EVENT_INBAND_RING_STATUS";
            break;
        case BTL_VG_EVENT_VG_CONTEXT_DISABLED:
            eventName = "BTL_VG_EVENT_VG_CONTEXT_DISABLED";
            break;
        case BTL_VG_EVENT_VG_CONTEXT_ENABLED:
            eventName = "BTL_VG_EVENT_VG_CONTEXT_ENABLED";
            break;
        case BTL_VG_EVENT_UNRECOGNIZED_AT_COMMAND:
            eventName = "BTL_VG_EVENT_UNRECOGNIZED_AT_COMMAND";
            break;
        case BTL_VG_EVENT_ADDITIONAL_SERVICE_CONNECT_REQ:
            eventName = "BTL_VG_EVENT_ADDITIONAL_SERVICE_CONNECT_REQ";
            break;
        case BTL_VG_EVENT_HANDOVER_COMPLETED:
            eventName = "BTL_VG_EVENT_HANDOVER_COMPLETED";
            break;
        case BTL_VG_EVENT_HANDOVER_FAILED:
            eventName = "BTL_VG_EVENT_HANDOVER_FAILED";
            break;
#if 0
        case BTL_VG_EVENT_FM_OVER_BT_ENABLED:
            eventName = "BTL_VG_EVENT_FM_OVER_BT_ENABLED";
            break;
        case BTL_VG_EVENT_FM_OVER_BT_DISABLED:
            eventName = "BTL_VG_EVENT_FM_OVER_BT_DISABLED";
            break;
#endif /* BTL_CONFIG_FM_STACK == BTL_CONFIG_ENABLED */
#if (BTL_VG_BT_EXTENDED_EVENTS == XA_ENABLED)
        case BTL_VG_EVENT_UNIT_SUPPORTED_FEATURES:
            eventName = "BTL_VG_EVENT_UNIT_SUPPORTED_FEATURES";
            break;
        case BTL_VG_EVENT_CALL_TERMINATED:
            eventName = "BTL_VG_EVENT_CALL_TERMINATED";
            break;
        case BTL_VG_EVENT_NUMBER_DIALED:
            eventName = "BTL_VG_EVENT_NUMBER_DIALED";
            break;
        case BTL_VG_EVENT_CALL_HOLD_MULTIPARTY_HANDLING:
            eventName = "BTL_VG_EVENT_CALL_HOLD_MULTIPARTY_HANDLING";
            break;
        case BTL_VG_EVENT_CALL_WAITING_NOTIFICATION_ENABLED:
            eventName = "BTL_VG_EVENT_CALL_WAITING_NOTIFICATION_ENABLED";
            break;
        case BTL_VG_EVENT_NREC_TURNED_OFF:
            eventName = "BTL_VG_EVENT_NREC_TURNED_OFF";
            break;
        case BTL_VG_EVENT_CALLING_LINE_ID_ENABLED:
            eventName = "BTL_VG_EVENT_CALLING_LINE_ID_ENABLED";
            break;
        case BTL_VG_EVENT_TRANSMIT_DTMF:
            eventName = "BTL_VG_EVENT_TRANSMIT_DTMF";
            break;
        case BTL_VG_EVENT_BUTTON_PRESSED:
            eventName = "BTL_VG_EVENT_BUTTON_PRESSED";
            break;
        case BTL_VG_EVENT_RING_SENT:
            eventName = "BTL_VG_EVENT_RING_SENT";
            break;
#endif /* BTL_VG_BT_EXTENDED_EVENTS == XA_ENABLED */
#if BTL_VG_MC_EXTENDED_EVENTS == XA_ENABLED
        case BTL_VG_MC_EVENT_CALL_SETUP_STATUS:
            eventName = "BTL_VG_MC_EVENT_CALL_SETUP_STATUS";
            break;
        case BTL_VG_MC_EVENT_CALL_STATUS:
            eventName = "BTL_VG_MC_EVENT_CALL_STATUS";
            break;
        case BTL_VG_MC_EVENT_CALL_HOLD_STATUS:
            eventName = "BTL_VG_MC_EVENT_CALL_HOLD_STATUS";
            break;
        case BTL_VG_MC_EVENT_CALL_WAITING:
            eventName = "BTL_VG_MC_EVENT_CALL_WAITING";
            break;
        case BTL_VG_MC_EVENT_EXTENDED_ERROR:
            eventName = "BTL_VG_MC_EVENT_EXTENDED_ERROR";
            break;
        case BTL_VG_MC_EVENT_RING_RECEIVED:
            eventName = "BTL_VG_MC_EVENT_RING_RECEIVED";
            break;
        case BTL_VG_MC_EVENT_CALLING_LINE_ID_RECEIVED:
            eventName = "BTL_VG_MC_EVENT_CALLING_LINE_ID_RECEIVED";
            break;
#endif /* BTL_VG_MC_EXTENDED_EVENTS == XA_ENABLED */
#if BTL_VG_VC_EXTENDED_EVENTS == XA_ENABLED
        case BTL_VG_VC_EVENT_VOICE_PATH_CHANGED:
            eventName = "BTL_VG_VC_EVENT_VOICE_PATH_CHANGED";
            break;
        case BTL_VG_VC_EVENT_NREC_ENABLED:
            eventName = "BTL_VG_VC_EVENT_NREC_ENABLED";
            break;
        case BTL_VG_VC_EVENT_VR_ENABLED:
            eventName = "BTL_VG_VC_EVENT_VR_ENABLED";
            break;
#endif /* BTL_VG_VC_EXTENDED_EVENTS == XA_ENABLED */
        default:
            eventName = "UNKNOWN";
            break;
    }
    
    return (eventName);
}


#else /* BTL_CONFIG_VG == BTL_CONFIG_ENABLED */

void VGA_Init(void)
{
	Report(("VGA_Init -BTL_CONFIG_VG is disabled "));
}
void VGA_Deinit(void)
{
	Report(("VGA_Deinit  - BTL_CONFIG_VG is disabled"));
}

void VGA_ProcessUserAction(U8 *msg)
{
	msg=msg;
	Report(("VG_APP is disabled via BTL_CONFIG."));

}






#endif /* BTL_CONFIG_VG == BTL_CONFIG_ENABLED */


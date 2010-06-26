/* =========================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* ========================================================== */
/*
 $Project name: Basic Bluetooth MMI
 $Project code:
 $Module:   Bluetooth BMG MFW
 $File:       Mfw_Btips.c
 $Revision:   1.0
 $Author:   Texas Instruments
 $Date:       26/06/07

********************************************************************************

 Description:

    This module provides the BTIPS BMG APPlication functionality.

********************************************************************************
 $History: Mfw_Btips.c

  26/06/07 Sasken original version

 $End

*******************************************************************************/




/*******************************************************************************

                                Include files

*******************************************************************************/

#define ENTITY_MFW
/* includes */

#include <string.h>

#if defined (NEW_FRAME)

#include "typedefs.h"
#include "vsi.h"
#include "pei.h"
#include "custom.h"
#include "gsm.h"
#include "prim.h"

#else

#include "STDDEFS.H"
#include "custom.h"
#include "gsm.h"
#include "vsi.h"

#endif

#ifdef FF_MMI_BTIPS_APP

#include "osapi.h"

/* GPF Includes */
#include "typedefs.h"
#include "vsi.h"
//#include "../p_btt.h"

#if BT_STACK == XA_ENABLED
#include <me.h>
#include <bttypes.h>
#include <sec.h>
#endif

#include "btl_common.h"
#include "btl_bmg.h"
#include "debug.h"

#include "bthal_fs.h"
#include "mfw_mfw.h"
#include "mfw_win.h"
#include "mfw_icn.h"	/* included for mfw_mnu.h */
#include "mfw_mnu.h"
#include "mfw_tim.h"
#include "mfw_kbd.h"
#include "mfw_sat.h" /* included for MmiDummy.h, included for MmiMenu.h */

/* BTL inclides */
#include "hfg.h"
#include "btl_vg.h"
#include "btl_vgi.h"
#include "Mfw_Btips.h"
#include "Mfw_BtipsBmg.h"
#include "mfw_BtipsVg.h"

static void btips_vgCallback(const BtlVgEvent *event);

static const BtSecurityLevel noSecurity = BSL_NO_SECURITY;
static const BtlVgAudioSourcesAction noActions = {0};
static AtAgFeatures features = HFG_FEATURE_REJECT | HFG_FEATURE_RING_TONE;

static BOOL closingVoiceGateway = FALSE;

static BtlVgChannelId mfwBtipsVgChannelId = BTL_VG_CHANNEL_ID_NONE;
static BtlVgContext *mfwBtipsVgContext =0;
static int audio_on_hands_free = 0;

/*---------------------------------------------------------------------------
 *            mfw_btips_vgInit
 *---------------------------------------------------------------------------
 *
 * Synopsis: VG application initialization.		
 *
 * Return:    NONE.
 *
 */

void mfw_btips_vgInit ( )
{
	if (BT_STATUS_SUCCESS != BTL_VG_Create(0, btips_vgCallback, &noSecurity, &mfwBtipsVgContext))
	{
		TRACE_FUNCTION("BTL_VG_Create failed");
	}

	if (BT_STATUS_PENDING != BTL_VG_Enable(mfwBtipsVgContext, 0, &features, &noActions))
	{
		TRACE_FUNCTION("BTL_VG_Enable failed");
	}
}

BOOL mfw_btips_vgGetConnectedDevice (BD_ADDR *bdAddr, U8 *name)
{
	BtStatus status;

	TRACE_FUNCTION("mfw_btips_vgGetConnectedDevice");

	status = BTL_VG_GetConnectedDevice (mfwBtipsVgContext, mfwBtipsVgChannelId, bdAddr);
	
	if (status == BT_STATUS_SUCCESS)
	{
		mfw_btips_bmgGetDeviceName (bdAddr, name);
		return TRUE;
	}
	return FALSE;
}

BOOL mfw_btips_vgIsDeviceConnected (BD_ADDR bdAddr)
{
	BtRemDevState state;

	TRACE_FUNCTION("mfw_btips_vgIsDeviceConnected");

	if (mfwBtipsVgChannelId != BTL_VG_CHANNEL_ID_NONE)
	{
#if 0
		BD_ADDR connectedDev;
		BTL_VG_GetConnectedDevice (mfwBtipsVgContext, mfwBtipsVgChannelId, &connectedDev)
#endif
		mfw_btips_bmgGetDeviceState  (&bdAddr, &state);
		if (state == BDS_CONNECTED)
		{
			return TRUE;
		}
	}
	return FALSE;

}
	
BOOL mfw_btips_vgConnect (BD_ADDR bdAddr)
{
	BtStatus btStatus;

	TRACE_FUNCTION("mfw_btips_vgConnect");

	btStatus = BTL_VG_Connect (mfwBtipsVgContext, &bdAddr, BTL_VG_AUDIO_SOURCE_MODEM);

	if (BT_STATUS_IN_USE == btStatus)
	{
		TRACE_FUNCTION("VG APP: An SLC to a remote device already exists");
		TRACE_FUNCTION("VG APP: Disconnect or perform handover to connect to a new device");
	}
	return mfw_btips_checkStatus(btStatus);
}

BOOL mfw_btips_vgDisconnect (BD_ADDR bdAddr)
{
	BtStatus btStatus;
	
	TRACE_FUNCTION("mfw_btips_vgDisconnect");
	
	btStatus = BTL_VG_Disconnect (mfwBtipsVgContext, mfwBtipsVgChannelId);
	return mfw_btips_checkStatus(btStatus);
}

BOOL mfw_btips_vgTransferAudioToPhone (void)
{
	BtStatus btStatus;
	
	TRACE_FUNCTION("mfw_btips_vgTransferAudioToPhone");
	
	if ((mfwBtipsVgChannelId != BTL_VG_CHANNEL_ID_NONE) && (audio_on_hands_free == 1))
	{
		btStatus = BTL_VG_DisconnectAudio (mfwBtipsVgContext, mfwBtipsVgChannelId);
	}
	return mfw_btips_checkStatus(btStatus);
}

BOOL mfw_btips_vgTransferAudioToHF (void)
{
	BtStatus btStatus;

	TRACE_FUNCTION_P1("mfw_btips_vgTransferAudioToHF audio_on_hands_free=%d", audio_on_hands_free);
	
	if ((mfwBtipsVgChannelId != BTL_VG_CHANNEL_ID_NONE) && (audio_on_hands_free == 0))
	{
		btStatus = BTL_VG_ConnectAudio (mfwBtipsVgContext, mfwBtipsVgChannelId);
	}
	return mfw_btips_checkStatus(btStatus);
}

BOOL mfw_btips_vgIsAudioOnPhone (void)
{

	TRACE_FUNCTION("mfw_btips_vgIsAudioOnPhone");
	if (audio_on_hands_free == 0)
	{
		return TRUE;
	}
	else if (mfwBtipsVgChannelId != BTL_VG_CHANNEL_ID_NONE)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

BOOL mfw_btips_vgIsAudioOnHF (void)
{
	if (audio_on_hands_free == 1)
	{
		return TRUE;
	}
	else if (mfwBtipsVgChannelId != BTL_VG_CHANNEL_ID_NONE)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

BOOL mfw_btips_vgIsHandOverPossible (void)
{
	if (audio_on_hands_free == 1)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void btips_vgCallback(const BtlVgEvent *event)
{
	T_BTIPS_MMI_IND	btips_mmi_ind;
	BtStatus status;
	char str[MFW_VG_APP_AT_STR_LEN];
	U16 strLen;
	U16 scoHandle = 0;

	TRACE_FUNCTION_P1("btips_vgCallback %d", event->type);

	btips_mmi_ind.mfwBtipsEventType = MFW_BTIPS_VG_EVENT;

	switch (event->type)
	{
	case BTL_VG_EVENT_SLC_REQUEST:
		TRACE_EVENT("VG APP: SLC request from device:");
		break;

	case BTL_VG_EVENT_ADDITIONAL_SERVICE_CONNECT_REQ:
		TRACE_EVENT("VG APP: Addtional SLC request received from device:");
		break;

	case BTL_VG_EVENT_SLC_CONNECTED:
		mfwBtipsVgChannelId = event->channelId;
		TRACE_EVENT("VG APP: Connection established");
		/* turn off inband ring tone */
		if (BT_STATUS_SUCCESS != BTL_VG_EnableInbandRingTone(mfwBtipsVgContext, event->channelId, FALSE))
		{
			TRACE_EVENT("VG_APP: Error disabling inband ring tone after SLC established");
		}
		btips_mmi_ind.data.vgBtEvent.event = MFW_BTIPS_VG_CONNECT_EVENT;
		
		status = BTL_VG_GetConnectedDevice (mfwBtipsVgContext, event->channelId, 
							&(btips_mmi_ind.data.vgBtEvent.bdAddr));
		if (status == BT_STATUS_SUCCESS)
		{
			mfw_btips_bmgGetDeviceName (&(btips_mmi_ind.data.vgBtEvent.bdAddr), 
							btips_mmi_ind.data.vgBtEvent.deviceName);

			btipsNotifyMFW (&btips_mmi_ind);
		}
									
		break;

	case BTL_VG_EVENT_SLC_DISCONNECTED:
		mfwBtipsVgChannelId = BTL_VG_CHANNEL_ID_NONE;
		TRACE_EVENT("VG APP: Connection lost");
		btips_mmi_ind.data.vgBtEvent.event = MFW_BTIPS_VG_DISCONNECT_EVENT;
		#ifdef FF_MMI_FMRADIO_APP
		if(mfw_fmRadio_isFmOverBT())
		{
			/*Inform FM module of the SLC disconnection*/
			mfw_fmRadio_listenViaBT(FALSE);		
			mfw_fmRadio_setFmOverBT(FALSE, 0);
		}
		#endif

		btipsNotifyMFW (&btips_mmi_ind);

		break;
		
	case BTL_VG_EVENT_HANDOVER_COMPLETED:
		mfwBtipsVgChannelId = event->channelId;
		TRACE_EVENT("VG APP: Handover completed");
		break;
		
	case BTL_VG_EVENT_AUDIO_CONNECTED:
	{
		HfgChannel *handsfreeChannel;
		BtlVgChannel *vgChannel;
		 CmgrHandler         cmgrHandler;
		scoHandle = 0x101;
		btips_mmi_ind.data.vgBtEvent.event = MFW_BTIPS_VG_AUDIO_CONNECT_EVENT;
#if 0
		vgChannel = &(event->context->channels[event->channelId]);
		handsfreeChannel = &(vgChannel->c.handsfreeChannel);
		cmgrHandler = handsfreeChannel->cmgrHandler;
		scoHandle = cmgrHandler.scoConnect->scoHciHandle;

		TRACE_EVENT_P1("VG APP: Audio connection established, scoHandle = %x", scoHandle);
#endif
		audio_on_hands_free = 1;

#ifdef FF_MMI_FMRADIO_APP
		if(mfw_fmRadio_isFmOverBT())
		{
			/*Send VS commands to BRF for routing FM audio to BT headset*/
			mfw_fmRadio_setFmOverBT(TRUE, scoHandle);
		}
#endif
		btipsNotifyMFW (&btips_mmi_ind);

	}	break;

	case BTL_VG_EVENT_AUDIO_DISCONNECTED:
		TRACE_EVENT("VG APP: Audio conenction lost");
		btips_mmi_ind.data.vgBtEvent.event = MFW_BTIPS_VG_AUDIO_DISCONNECT_EVENT;

		audio_on_hands_free = 0;
#ifdef FF_MMI_FMRADIO_APP
		if(mfw_fmRadio_isFmOverBT())
		{
			/*Inform FM module of the SLC disconnection*/
			mfw_fmRadio_listenViaBT(FALSE);		
			mfw_fmRadio_setFmOverBT(FALSE, 0);
		}
#endif

		btipsNotifyMFW (&btips_mmi_ind);

		break;

	case BTL_VG_EVENT_PHONE_NUMBER_REQUESTED:
		TRACE_EVENT("VG APP: unsupported event: BTL_VG_EVENT_PHONE_NUMBER_REQUESTED");
		break;

	case BTL_VG_EVENT_MICROPHONE_VOLUME:
		TRACE_EVENT_P1("VG APP: microphone volume: %u", event->p.volume);
		break;

	case BTL_VG_EVENT_SPEAKER_VOLUME:
		TRACE_EVENT_P1("VG APP: speaker volume: %u", event->p.volume);
		break;

	case BTL_VG_EVENT_VOICE_RECOGNITION_STATUS:
		TRACE_EVENT_P1("VG APP: voice recognition is :%s", (event->p.enabled ? "enabled" : "disabled"));
		break;
		
#if HFG_FEATURE_RING_TONE & HFG_SDK_FEATURES

	case BTL_VG_EVENT_INBAND_RING_STATUS:
		TRACE_EVENT_P1("VG APP: inband ringing is: %s", (event->p.enabled ? "enabled" : "disabled"));
		break;
#endif

	case BTL_VG_EVENT_UNRECOGNIZED_AT_COMMAND:
		strLen = event->p.atCommand->commandLength;
		if (strLen > MFW_VG_APP_AT_STR_LEN )
		{
			TRACE_EVENT("VG APP: AT Command string is too long");
			strLen = MFW_VG_APP_AT_STR_LEN - 1;
		}
		OS_MemCopy((U8*)str, (const U8*) event->p.atCommand->atCommand, strLen);
		str[strLen] = '\0';
		TRACE_EVENT_P1("VG APP: unrecognized AT command: %s", str);
		if (BT_STATUS_SUCCESS != BTL_VG_SentAtError(event->context, event->channelId, ATCME_OP_NOT_SUPPORTED))
		{
			TRACE_EVENT("VG APP: BTL_VG_SentAtError failed");
		}
		break;
		
	case BTL_VG_EVENT_VG_CONTEXT_ENABLED:
		TRACE_EVENT("VG APP: VG is enabled");
		break;

	case BTL_VG_EVENT_VG_CONTEXT_DISABLED:
		TRACE_EVENT("VG APP: VG is disabled");
		if (TRUE == closingVoiceGateway)
		{
			BtStatus status;

			status = BTL_VG_Destroy(&mfwBtipsVgContext);
			TRACE_EVENT_P1("BTL_VG_Destroy returned: %s", pBT_Status(status));
			closingVoiceGateway = FALSE;
		}
		break;

	default:
		TRACE_EVENT_P1("VG APP: Unexpected VG event: %u", event->type);
		break;
	}
	
}
void mfw_btips_vgCallback (T_BTIPS_MMI_IND * mmiInd)
{
	MfwBtVgEvent vgBtEvent = mmiInd->data.vgBtEvent;

	TRACE_FUNCTION_P1("mfw_btips_vgCallback: Event %d", vgBtEvent.event);
	
	switch (vgBtEvent.event)
	{
		case MFW_BTIPS_VG_CONNECT_EVENT:
	#ifdef FF_MMI_FMRADIO_APP		
			if(mfw_fmRadio_isFmOverBT())
			{
				/*Automatically create the SCO for FM Radio routing
				In this case Modem Audio will not be routed to the MSCI port, hence
				all GSM calls will be answered on the Phone Handset ONLY*/
				mfw_btips_vgTransferAudioToHF();
			}
	#endif		
			mfw_btips_signal(E_BTIPS_VG_CONNECT_EVENT, mmiInd);
		break;
		
		case MFW_BTIPS_VG_DISCONNECT_EVENT:			
			mfw_btips_signal(E_BTIPS_VG_DISCONNECT_EVENT, mmiInd);
		break;
		
		case MFW_BTIPS_VG_AUDIO_CONNECT_EVENT:			
			mfw_btips_signal(E_BTIPS_VG_AUDIO_CONNECT_EVENT, mmiInd);
		break;
		
		case MFW_BTIPS_VG_AUDIO_DISCONNECT_EVENT:			
			mfw_btips_signal(E_BTIPS_VG_AUDIO_DISCONNECT_EVENT, mmiInd);
		break;
	}
}

#endif


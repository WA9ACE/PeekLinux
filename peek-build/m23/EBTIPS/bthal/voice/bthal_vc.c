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
*   FILE NAME:		bthal_vc.c
*
*   DESCRIPTION:	Implementation of the BTHAL VC Module.
*
*   AUTHOR:			Itay Klein
*
\*******************************************************************************/


#include "osapi.h"
#include "btl_config.h"
#include "bthal_common.h"

#if (BTL_CONFIG_VG == BTL_CONFIG_ENABLED) 

/*******************************************************************************
 *
 * Include files
 *
 ******************************************************************************/
#include "nucleus.h"
#include "l1_types.h"
#include "l1_confg.h"
#include "rv/rv_general.h"
#include "rvm/rvm_gen.h"
#include "../../../chipsetsw/Services/Audio/audio_api.h"
#include "../../../chipsetsw/riviera/rv/rv_general.h"
#include <armio.h>

#include "utils.h"
#include "bthal_vc.h"
#include "bthal_log.h"
#include "hcitrans.h"

#include "bthal_log_modules.h"

#define BTHAL_VOICE_LOG_ERROR(msg)			BTHAL_LOG_ERROR(__FILE__, __LINE__, BTHAL_LOG_MODULE_TYPE_VG, msg)
#define BTHAL_VOICE_LOG_INFO(msg)			BTHAL_LOG_INFO(__FILE__, __LINE__, BTHAL_LOG_MODULE_TYPE_VG, msg)
#define BTHAL_VOICE_LOG_FUNCTION(msg)		BTHAL_LOG_FUNCTION(__FILE__, __LINE__, BTHAL_LOG_MODULE_TYPE_VG, msg)

#define BTHAL_VC_MAXIMAL_NUMBER_OF_COMMANDS 20

typedef enum _BthalVcCommandType
{
	BTHAL_VC_CMD_CHANGE_AUDIO_PATH,
	BTHAL_VC_CMD_SET_VOICE_RECOGNITION,
	BTHAL_VC_CMD_SET_NREC
}BthalVcCommandType;

struct _BthalVcContext
{
	BTHAL_BOOL used;
	
	BthalVcCallback callback;

	void *userData;
};

typedef struct _BthalVcCommand
{
	ListEntry node;

	BthalVcContext *context;

	BthalVcCommandType type;

	union
	{
		BthalVcAudioPath path;

		BTHAL_BOOL enableVR;

		BTHAL_BOOL enableNREC;
	} p;

}BthalVcCommand;


typedef struct _BthalVcModuleData
{
	BthalVcAudioPath path;

	BthalVcContext contextsMem[BTHAL_VC_MAXIMAL_NUMBER_OF_USERS];

	BthalVcCommand commandsMem[BTHAL_VC_MAXIMAL_NUMBER_OF_COMMANDS];

	ListEntry freeCommands;

	ListEntry commandsQue;

	BTHAL_BOOL busy;

}BthalVcModuleData;

/*******************************************************************************
 *
 * Globals
 *
 ******************************************************************************/

static BthalVcModuleData moduleData = {0};

/*******************************************************************************
 *
 * Internal functions declarations
 *
 ******************************************************************************/

static void BthalVcConfirmSettingVoicePath(U32 primId, U32 *primPtr);

static void BthalVcProcessCommands(void);

static void BthalVcSetVoicePath(BthalVcCommand *cmd);

static void BthalVcSetVoiceRecognition(BthalVcCommand *cmd);

static void BthalVgSetNrec(BthalVcCommand *cmd);

static void BthalVcFmRxAudioPathCallback(void *msg);

/*******************************************************************************
 *
 * Functions definition
 *
 ******************************************************************************/

BthalStatus BTHAL_VC_Init(BthalCallBack	callback)
{
	BTHAL_U32 nContexts, nCommands;
	BthalVcContext *context;
	BthalVcCommand *cmd;

	BTHAL_VOICE_LOG_FUNCTION(("BTHAL_VC_Init"));

	if (0 == callback)
	{
		return BTHAL_STATUS_INVALID_PARM;
	}
	
	context = moduleData.contextsMem;
	for (nContexts = BTHAL_VC_MAXIMAL_NUMBER_OF_USERS; nContexts > 0; --nContexts)
	{
		context->used = BTHAL_FALSE;
		++context;
	}

	cmd = moduleData.commandsMem;
	InitializeListHead(&moduleData.freeCommands);
	for (nCommands = BTHAL_VC_MAXIMAL_NUMBER_OF_COMMANDS; nCommands > 0; --nCommands)
	{
		InitializeListEntry(&cmd->node);
		InsertHeadList(&moduleData.freeCommands, &cmd->node);
		++cmd;
	}
	moduleData.busy = BTHAL_FALSE;	
	InitializeListHead(&moduleData.commandsQue);
	
	return BTHAL_STATUS_SUCCESS;	
}

BthalStatus BTHAL_VC_Deinit(void)
{
	BTHAL_VOICE_LOG_FUNCTION(("BTHAL_VC_Deinit"));
	
	return BTHAL_STATUS_SUCCESS;
}

BthalStatus BTHAL_VC_Register(BthalVcCallback callback,
							  void *userData,
							  BthalVcContext **context)
{
	BthalVcEvent event;
	BTHAL_U32 nContext;
	BthalVcContext *vcContext;

	BTHAL_VOICE_LOG_FUNCTION(("BTHAL_VC_Register"));

	if (0 == callback || 0 == context)
	{
		return BTHAL_STATUS_INVALID_PARM;
	}

	vcContext = moduleData.contextsMem;
	for (nContext = BTHAL_VC_MAXIMAL_NUMBER_OF_USERS; nContext > 0; --nContext)
	{
		if (BTHAL_FALSE == vcContext->used)
		{
			break;
		}
		++vcContext;
	}
	if (0 == nContext)
	{
		*context = 0;
		return BTHAL_STATUS_FAILED;
	}

	vcContext->used = BTHAL_TRUE;
	vcContext->callback = callback;
	vcContext->userData = userData;

	event.status = BTHAL_STATUS_SUCCESS;
	event.type = BTHAL_VC_EVENT_LINK_ESTABLISHED;
	event.context = vcContext;
	event.userData = userData;
	callback(&event);

	*context = vcContext;

	return BTHAL_STATUS_PENDING;
}

BthalStatus BTHAL_VC_Unregister(BthalVcContext **context)
{
	BthalVcEvent event;

	BTHAL_VOICE_LOG_FUNCTION(("BTHAL_VC_Unregister"));

	if (0 == context)
	{
		return BTHAL_STATUS_INVALID_PARM;
	}

	event.type= BTHAL_VC_EVENT_LINK_RELEASED;
	event.status = BTHAL_STATUS_SUCCESS;
	event.context = *context;
	event.userData = (*context)->userData;

	(*context)->callback(&event);
	(*context)->used = BTHAL_FALSE;
	*context = 0;

	return BTHAL_STATUS_PENDING;
}

#if BTL_CONFIG_VG == BTL_CONFIG_ENABLED

BthalStatus BTHAL_VC_SetVoicePath(BthalVcContext *context,
								  BthalVcAudioSource source,
								  BthalVcAudioPath path)
{
	BthalVcCommand *cmd;

	BTHAL_VOICE_LOG_FUNCTION(("BTHAL_VC_SetVoicePath"));

	if (0 == context)
	{
		return BTHAL_STATUS_INVALID_PARM;
	}

	if (IsListEmpty(&moduleData.freeCommands))
	{
		return BTHAL_STATUS_NO_RESOURCES;
	}
	cmd = (BthalVcCommand*)GetHeadList(&moduleData.freeCommands);
	RemoveEntryList(&cmd->node);
	
	cmd->context = context;
	cmd->type = BTHAL_VC_CMD_CHANGE_AUDIO_PATH;
	cmd->p.path = path;
	InsertTailList(&moduleData.commandsQue, &cmd->node);

	BthalVcProcessCommands();

	return BTHAL_STATUS_PENDING;
}


BthalStatus BTHAL_VC_SetVoiceRecognition(BthalVcContext *context,
										 BTHAL_BOOL enable)
{
	BthalVcCommand *cmd;

	BTHAL_VOICE_LOG_FUNCTION(("BTHAL_VC_SetVoiceRecognition"));

	if (0 == context)
	{
		return BTHAL_STATUS_INVALID_PARM;
	}

	if (IsListEmpty(&moduleData.freeCommands))
	{
		return BTHAL_STATUS_NO_RESOURCES;
	}
	cmd = (BthalVcCommand*)GetHeadList(&moduleData.freeCommands);
	RemoveEntryList(&cmd->node);
	
	cmd->context = context;
	cmd->type = BTHAL_VC_CMD_SET_VOICE_RECOGNITION;
	cmd->p.enableVR = enable;
	InsertTailList(&moduleData.commandsQue, &cmd->node);

	BthalVcProcessCommands();

	return BTHAL_STATUS_PENDING;

}

BthalStatus BTHAL_VC_SetNoiseReductionAndEchoCancelling(BthalVcContext *context,
														BTHAL_BOOL enable)
{
	BthalVcCommand *cmd;

	BTHAL_VOICE_LOG_FUNCTION(("BTHAL_VC_SetNoiseReductionAndEchoCancelling"));

	if (context == 0)
	{
		return BTHAL_STATUS_INVALID_PARM;
	}

	if (IsListEmpty(&moduleData.freeCommands))
	{
		return BTHAL_STATUS_NO_RESOURCES;
	}
	cmd = (BthalVcCommand*)GetHeadList(&moduleData.freeCommands);
	RemoveEntryList(&cmd->node);
	cmd->context = context;
	cmd->type = BTHAL_VC_CMD_SET_NREC;
	cmd->p.enableNREC = enable;
	InsertTailList(&moduleData.commandsQue, &cmd->node);
	BthalVcProcessCommands();

	return BTHAL_STATUS_PENDING;
}

static void BthalVcSetVoiceRecognition(BthalVcCommand *cmd)
{
	BthalVcEvent event;

	BTHAL_VOICE_LOG_FUNCTION(("BthalVcSetVoiceRecognition"));

	BTHAL_VOICE_LOG_INFO(("BTHAL VC: Voice Recognition is: %s", (const char*)(cmd->p.enableVR? "enabled":"disabled")));

	event.type = BTHAL_VC_EVENT_VOICE_RECOGNITION_STATUS;
	event.status = BTHAL_STATUS_SUCCESS;
	event.context = cmd->context;
	event.userData = cmd->context->userData;
	event.p.enabled = cmd->p.enableVR;
	cmd->context->callback(&event);

	RemoveHeadList(&moduleData.commandsQue);
	InsertTailList(&moduleData.freeCommands, &cmd->node);
	moduleData.busy = BTHAL_FALSE;
	BthalVcProcessCommands();
}

static void BthalVcSetNrec(BthalVcCommand *cmd)
{
	BthalVcEvent event;

	BTHAL_VOICE_LOG_FUNCTION(("BthalVcSetNrec"));

	BTHAL_VOICE_LOG_INFO(("BTHAL VC: Noise Reduction and Echo Cancelling are: %s",
		(const char*)(cmd->p.enableNREC? "enabled":"disabled")));	

	event.type = BTHAL_VC_EVENT_NOISE_REDUCTION_ECHO_CANCELLING_STATUS;
	event.status = BTHAL_STATUS_SUCCESS;
	event.context = cmd->context;
	event.userData = cmd->context->userData;
	event.p.enabled = cmd->p.enableNREC;
	cmd->context->callback(&event);
	
	RemoveHeadList(&moduleData.commandsQue);
	InsertTailList(&moduleData.freeCommands, &cmd->node);
	moduleData.busy = BTHAL_FALSE;
	BthalVcProcessCommands();
}
														

static void BthalVcSetVoicePath(BthalVcCommand *cmd)
{
	BTHAL_I32 route = 0;
	T_AUDIO_FULL_ACCESS_WRITE audio_path_param;
	T_AUDIO_VOICE_PATH_SETTING hsg_voice_path_mode;
	T_RV_RETURN return_path;
	T_AUDIO_RET status = AUDIO_ERROR;
	
	BTHAL_VOICE_LOG_FUNCTION(("BthalVcSetVoicePath"));

	switch (cmd->p.path)
	{
	case BTHAL_VC_AUDIO_PATH_HANDSET:
		/* return_path.addr_id  = btui_task_id; */
		return_path.addr_id  = rvf_get_taskid();
		return_path.callback_func = NULL;

		/*	Next, switch the audio path. 	*/

		hsg_voice_path_mode = AUDIO_GSM_VOICE_PATH;
		audio_path_param.variable_indentifier = AUDIO_PATH_USED;
		audio_path_param.data = &hsg_voice_path_mode;
		status = audio_full_access_write(&audio_path_param , return_path);
		break;
	case BTHAL_VC_AUDIO_PATH_BLUETOOTH:
		l1_select_mcsi_port(1);

		/* return_path.addr_id  = btui_task_id; */
		return_path.addr_id  = rvf_get_taskid();
		return_path.callback_func = NULL;

		/* Next, switch the audio path. */

		hsg_voice_path_mode = AUDIO_BLUETOOTH_HEADSET;
		audio_path_param.variable_indentifier = AUDIO_PATH_USED;
		audio_path_param.data = &hsg_voice_path_mode;
		status = audio_full_access_write(&audio_path_param , return_path);
		break;
	case BTHAL_VC_AUDIO_PATH_HANDSET | BTHAL_VC_AUDIO_PATH_BLUETOOTH:
		/* audio is to be routed to both bluetooth and the handset */
		break;
	default:
		BTHAL_VOICE_LOG_ERROR(("BTHAL VC: invalid path value"));
		break;
	}
	if (AUDIO_OK != status)
	{
		BTHAL_VOICE_LOG_ERROR(("BTHAL VC: setting new voice path failed!"));
	}
	else
	{
		BthalVcConfirmSettingVoicePath(0,0);
	}
}

static void BthalVcConfirmSettingVoicePath(U32 primId, U32 *primPtr)
{
	BthalVcEvent event;
	BthalVcCommand *cmd;

	BTHAL_VOICE_LOG_FUNCTION(("BthalVcConfirmSettingVoicePath"));

	cmd = (BthalVcCommand*)GetHeadList(&moduleData.commandsQue);

	event.status = BTHAL_STATUS_SUCCESS;
	event.type = BTHAL_VC_EVENT_VOICE_PATHS_CHANGED;
	event.p.route.source = BTHAL_VC_AUDIO_SOURCE_MODEM;
	event.p.route.path = cmd->p.path;
	event.userData = cmd->context->userData;
	event.context = cmd->context;

	BTHAL_VOICE_LOG_INFO(("BTHAL VC: New voice path is set, path value: %u", moduleData.path));

	cmd->context->callback(&event);
	RemoveHeadList(&moduleData.commandsQue);
	InsertTailList(&moduleData.freeCommands, &cmd->node);
	moduleData.busy = BTHAL_FALSE;
	BthalVcProcessCommands();
}

static void BthalVcProcessCommands(void)
{
	BthalVcCommand *cmd;

	BTHAL_VOICE_LOG_FUNCTION(("BthalVcProcessCommands"));

	if (IsListEmpty(&moduleData.commandsQue) || BTHAL_TRUE == moduleData.busy)
	{
		return;
	}

	cmd = (BthalVcCommand*)GetHeadList(&moduleData.commandsQue);
	moduleData.busy = BTHAL_TRUE;
	
	switch(cmd->type)
	{
	case BTHAL_VC_CMD_CHANGE_AUDIO_PATH:
		BthalVcSetVoicePath(cmd);
		break;

	case BTHAL_VC_CMD_SET_VOICE_RECOGNITION:
		BthalVcSetVoiceRecognition(cmd);
		break;

	case BTHAL_VC_CMD_SET_NREC:
		BthalVcSetNrec(cmd);
		break;
		
	default:
		BTHAL_VOICE_LOG_ERROR(("BTHAL_MC: Unrecognized command type: %d", cmd->type));
	}	
}

#endif /* BTL_CONFIG_VG == BTL_CONFIG_ENABLED */

#if (0)

BthalStatus BTHAL_VC_FmRxSetAudioPath(BthalVcContext *context,
								      BthalVcFmRxAudioPath audioPath)
{
	BthalStatus halStatus = BTHAL_STATUS_PENDING;

	T_AUDIO_FULL_ACCESS_WRITE  audio_path_param_1;
	T_AUDIO_FULL_ACCESS_WRITE  audio_path_param_2;

	T_AUDIO_FM_PATH_SETTING	audio_fm_path_param;
	T_AUDIO_MODE_ONOFF 		audio_mode_on_off_param;

	T_RV_RETURN return_path;
	T_AUDIO_RET status = AUDIO_ERROR;
	
	BTHAL_VOICE_LOG_FUNCTION(("BTHAL_VC_FmRxSetAudioPath"));

	if (0 == context)
	{
		return BTHAL_STATUS_INVALID_PARM;
	}
    
	switch (audioPath)
	{
		case BTHAL_VC_FM_RX_ANALOG_AUDIO_PATH_OFF:

			/* audio is to be routed to both bluetooth and the handset */

			/* Init script (Headset - OFF, Handset - OFF)

				1. auw 32 0 (Disable FM to handset and headset)
			*/
			    

			/*	1. Disable FM to handset and headset. 	*/

			BTHAL_VOICE_LOG_INFO(("1. AUDIO_FM_SPEAKER_MODE: start"));
			
			return_path.addr_id  = rvf_get_taskid();
			return_path.callback_func = NULL;

			audio_path_param_1.variable_indentifier = AUDIO_FM_SPEAKER_MODE;
			audio_path_param_1.data = &audio_fm_path_param;
			audio_fm_path_param =  AUDIO_FM_SPEAKER_NONE;
			status = audio_full_access_write(&audio_path_param_1, return_path);

			if (status == AUDIO_ERROR)
			{
				BTHAL_VOICE_LOG_ERROR(("1. ERROR: setting AUDIO_FM_SPEAKER_MODE failed"));
				halStatus = BTHAL_STATUS_FAILED;
			}	
			else
			{
				BTHAL_VOICE_LOG_INFO(("1. AUDIO_FM_SPEAKER_MODE: end"));			
			}
			break;

		case BTHAL_VC_FM_RX_ANALOG_AUDIO_PATH_HEADSET:

			/* audio is to be routed to both bluetooth and the handset */

			/* Init script (Headset - ON)

				1. auw 32 1 (Configure FM to handset)
				2. auw 19 3 (DL&UL On)
				3. auw 1  4(Configures input path to FML & FMR) 
			*/
			    

			/*	1. Configure FM to Headset. 	*/

			BTHAL_VOICE_LOG_INFO(("1. AUDIO_FM_SPEAKER_MODE: start"));
			
			return_path.addr_id  = rvf_get_taskid();
			return_path.callback_func = NULL;

			audio_path_param_1.variable_indentifier = AUDIO_FM_SPEAKER_MODE;
			audio_path_param_1.data = &audio_fm_path_param;
			audio_fm_path_param =  AUDIO_FM_STEREO_SPEAKER_HEADSET;
			status = audio_full_access_write(&audio_path_param_1, return_path);

			if (status == AUDIO_ERROR)
			{
				BTHAL_VOICE_LOG_ERROR(("1. ERROR: setting AUDIO_FM_SPEAKER_MODE failed"));
				halStatus = BTHAL_STATUS_FAILED;
			}	
			else
			{
				BTHAL_VOICE_LOG_INFO(("1. AUDIO_FM_SPEAKER_MODE: end"));
			}
			

			/* 2. DL&UL On. 	*/

			BTHAL_VOICE_LOG_INFO(("2. AUDIO_ONOFF: start"));

			return_path.addr_id  = rvf_get_taskid();
			return_path.callback_func = BthalVcFmRxAudioPathCallback;

			audio_path_param_2.variable_indentifier = AUDIO_ONOFF;
			audio_path_param_2.data = &audio_mode_on_off_param;

			((T_AUDIO_MODE_ONOFF *)audio_path_param_2.data)->vdl_onoff = AUDIO_VDL_ON;
			((T_AUDIO_MODE_ONOFF *)audio_path_param_2.data)->vul_onoff = AUDIO_VUL_ON;
			status = audio_full_access_write(&audio_path_param_2 , return_path);

			if (status == AUDIO_ERROR)
			{
				BTHAL_VOICE_LOG_ERROR(("2. ERROR: setting AUDIO_ONOFF failed"));
				halStatus = BTHAL_STATUS_FAILED;
			}	
			else
			{
				BTHAL_VOICE_LOG_INFO(("2. AUDIO_ONOFF: end"));
			}
			break;
				
		case BTHAL_VC_FM_RX_ANALOG_AUDIO_PATH_HANDSET:
			
			/* audio is to be routed to both bluetooth and the handset */

			/* Init script (Handset - ON)

				1. auw 32 2 (Configure FM to handset)
				2. auw 19 3 (DL&UL On)
				3. auw 1  4(Configures input path to FML & FMR) 
			*/
			    

			/*	1. Configure FM to Handset. 	*/

			BTHAL_VOICE_LOG_INFO(("1. AUDIO_FM_SPEAKER_MODE: start"));
			
			return_path.addr_id  = rvf_get_taskid();
			return_path.callback_func = NULL;

			audio_path_param_1.variable_indentifier = AUDIO_FM_SPEAKER_MODE;
			audio_path_param_1.data = &audio_fm_path_param;
			audio_fm_path_param =  AUDIO_FM_MONO_SPEAKER_HANDFREE;
			status = audio_full_access_write(&audio_path_param_1, return_path);

			if (status == AUDIO_ERROR)
			{
				BTHAL_VOICE_LOG_ERROR(("1. ERROR: setting AUDIO_FM_SPEAKER_MODE failed"));
				halStatus = BTHAL_STATUS_FAILED;
			}	
			else
			{
				BTHAL_VOICE_LOG_INFO(("1. AUDIO_FM_SPEAKER_MODE: end"));			
			}

			/* 2. DL&UL On. 	*/

			BTHAL_VOICE_LOG_INFO(("2. AUDIO_ONOFF: start"));			
			
			return_path.addr_id  = rvf_get_taskid();
			return_path.callback_func = BthalVcFmRxAudioPathCallback;

			audio_path_param_2.variable_indentifier = AUDIO_ONOFF;
			audio_path_param_2.data = &audio_mode_on_off_param;
			
			((T_AUDIO_MODE_ONOFF *)audio_path_param_2.data)->vdl_onoff = AUDIO_VDL_ON;
			((T_AUDIO_MODE_ONOFF *)audio_path_param_2.data)->vul_onoff = AUDIO_VUL_ON;
			status = audio_full_access_write(&audio_path_param_2, return_path);

			if (status == AUDIO_ERROR)
			{
				BTHAL_VOICE_LOG_ERROR(("2. ERROR: setting AUDIO_ONOFF failed"));
				halStatus = BTHAL_STATUS_FAILED;
			}	
			else
			{
				BTHAL_VOICE_LOG_INFO(("2. AUDIO_ONOFF: end"));				
			}
			break;
            
		default:
			BTHAL_VOICE_LOG_ERROR(("BTHAL_VC_FmRxSetAudioPath(): unknown audio path %d",
                                                                audioPath));
			halStatus = BTHAL_STATUS_INVALID_PARM;
			break;
	}

       return (halStatus);
}

static void BthalVcFmRxAudioPathCallback(void *msg)
{
	T_AUDIO_FULL_ACCESS_WRITE  audio_path_param;
	BTHAL_U8 					audio_microphone_param;

	T_RV_RETURN return_path;
	T_AUDIO_RET status = AUDIO_ERROR;

	UNUSED_PARAMETER(msg);
    
	/* Init script
	
		3. auw 1  4(Configures input path to FML & FMR) 
	*/

	/* 3. Configures input path to FML & FMR.  */

	return_path.addr_id  = rvf_get_taskid();
	return_path.callback_func = NULL;

	audio_path_param.variable_indentifier = AUDIO_MICROPHONE_MODE;
	audio_path_param.data = &audio_microphone_param;
	audio_microphone_param = AUDIO_MICROPHONE_FM;
	status = audio_full_access_write(&audio_path_param, return_path);

	if (status == AUDIO_ERROR)
	{
		BTHAL_VOICE_LOG_ERROR(("3. ERROR: setting AUDIO_MICROPHONE_MODE failed"));
	}	
}
#endif /* BTL_CONFIG_FM_STACK == BTL_CONFIG_ENABLED */

#else /* ((BTL_CONFIG_VG == BTL_CONFIG_ENABLED) || (BTL_CONFIG_FM_STACK == BTL_CONFIG_ENABLED)) */

BthalStatus BTHAL_VC_Init(BthalCallBack	callback)

{
	callback = callback;
	Report(("BTHAL_VC_Init -BTL_CONFIG_VG is disabled."));
	return BTHAL_STATUS_SUCCESS;
}

BthalStatus BTHAL_VC_Deinit(void)
{
	Report(("BTHAL_VC_Deinit -BTL_CONFIG_VG is disabled."));
	return BTHAL_STATUS_SUCCESS;

}



#endif /* ((BTL_CONFIG_VG == BTL_CONFIG_ENABLED) || (BTL_CONFIG_FM_STACK == BTL_CONFIG_ENABLED)) */



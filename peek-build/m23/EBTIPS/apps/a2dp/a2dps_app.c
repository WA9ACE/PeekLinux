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
*   FILE NAME:      a2dps_app.c
*
*   DESCRIPTION:    This file contains the A2DP source application in the neptune
*					or locosto platform.
*
*
*   AUTHOR:         Keren Ferdman
*   AUTHOR:         Rene Kuiken 
*                   - Porting to Locosto
*                   - Added Pcm Simulation
\*******************************************************************************/

#include "btl_config.h"
#include "osapi.h"


#if BTL_CONFIG_A2DP ==   BTL_CONFIG_ENABLED
 

/* GPF Includes */
#include "typedefs.h"
#include "vsi.h"
#include "../p_btt.h"

#if BT_STACK == XA_ENABLED
#include <me.h>
#include <bttypes.h>
#include <sec.h>
#endif

#include "btl_common.h"
#include "debug.h"

#include "../app_main.h"
#include "overide.h"

/* BTL includes */
#include "btl_a2dp.h"
#include "btl_config.h"
#include "bthal_mm.h"
#include "bthal_mm_debug.h"
#include "lineParser.h"

#include "avrcp.h"

#include "btl_log.h"

static const BtlLogModuleType btlLogModuleType = 13;

BtlA2dpContext	*appA2dpContext = 0;
BtSecurityLevel btlA2dpSecurityLevel = BSL_NO_SECURITY;

#if SBC_ENCODER == XA_ENABLED
static BOOL externalSbcEncoder = FALSE;
#else
static BOOL externalSbcEncoder = TRUE;
#endif /* SBC_ENCODER == XA_ENABLED */


static BOOL pcmStreaming = FALSE;
static BOOL sbcStream = TRUE;

int avrcpChannelId2A2dpStreamId[BTL_CONFIG_AVRCPTG_MAX_NUM_CHANNELS_PER_CONTEXT] = {(-1)};

/* variables for interfacing BMI: */

#if BTHAL_A2DP_MM_PCM_SUPPORT == BTL_CONFIG_ENABLED

/* include of BMI interface */
#include "mfw_bt_api.h"
void BT_NotifyBmiEvent(BmiAvEvent event);

#else

#define BMI_SwitchAudioToBtHeadset(b) 0
#define BMI_RegisterEventCallback(p)
#define BMI_NotifyBtEvent(op)

#endif

/*-------------------------------------------------------------------------------
 * A2DP_MAX_NUM_STREAMS_PER_CONTEXT constant
 *
 *     Represents max number of streams per context.
 */
#define A2DP_MAX_NUM_STREAMS_PER_CONTEXT  (BTL_CONFIG_A2DP_MAX_NUM_SBC_STREAMS_PER_CONTEXT + \
											BTL_CONFIG_A2DP_MAX_NUM_MPEG1_2_AUDIO_STREAMS_PER_CONTEXT)

/* whether an SBC stream is in Open state or above */
static BOOL isOpenStream[A2DP_MAX_NUM_STREAMS_PER_CONTEXT] = {FALSE};
/* whether BMI approves that the streaming will go to the A2DP headset */
static BOOL a2dpStreamingGrantedByBmi = TRUE;

/* whether an SBC stream received a Start_Ind request from the sink */
static BOOL startIndReceived[A2DP_MAX_NUM_STREAMS_PER_CONTEXT] = {FALSE};

/* whether there is a pendign Start command on this stream */
static BOOL pendingStart[A2DP_MAX_NUM_STREAMS_PER_CONTEXT] = {FALSE};

/* in case configuration needed and stream is already started, stream will be suspend */
/* and theis flag will be TRUE. 													  */
BOOL isStreamNeedsToBeStarted[A2DP_MAX_NUM_STREAMS_PER_CONTEXT] = {FALSE};


#define IS_SBC_STREAM(s) ((s) < BTL_CONFIG_A2DP_MAX_NUM_SBC_STREAMS_PER_CONTEXT )
#define OTHER_SBC_STREAM(s) ((s) == 0 ? 1 : 0)

/* app streamId to bdadd */
static BD_ADDR streamIdToBdadd[A2DP_MAX_NUM_STREAMS_PER_CONTEXT];

/* This constant enables reading PCM file directly from FFS - need to define also in bthal_mm.c file */
/* #define BTHAL_PCM_FILE */


/****************************************************************************
 *
 * Internal Function prototypes
 *
 ***************************************************************************/

void A2DPA_ProcessUserAction(U8 *msg);
void A2DPA_Init(void);
void A2DPA_Deinit(void);

BOOL APP_A2DPS_ConnectMp3Stream(void);
BOOL APP_A2DPS_ConnectSbcStream(void);
BOOL APP_A2DPS_OpenStream(void);
BOOL APP_A2DPS_CloseStream(void);
BOOL APP_A2DPS_IdleStream(void);
BOOL APP_A2DPS_StartStream(void);
BOOL APP_A2DPS_SuspendStream(void);
BOOL APP_A2DPS_Abort(void);
BOOL APP_A2DPS_PlayMp3Audio(void);
BOOL APP_A2DPS_StopMp3Audio(void);
BOOL APP_A2DP_OpenMp3File(void);
BOOL APP_A2DP_CloseMp3File(void);

#ifdef BTHAL_PCM_FILE
BOOL APP_A2DPS_PlayPcmAudio(void);
BOOL APP_A2DPS_StopPcmAudio(void);
BOOL APP_A2DP_OpenPcmFile(void);
BOOL APP_A2DP_ClosePcmFile(void);
#endif	/* BTHAL_PCM_FILE */

BOOL APP_A2DPS_SetSecurityLevel(void);
BOOL APP_A2DPS_GetSecurityLevel(void);
BOOL APP_A2DPS_Enable(void);
BOOL APP_A2DPS_Disable(void);
BOOL APP_A2DPS_Create(void);
BOOL APP_A2DPS_Destroy(void);
BOOL APP_A2DPS_Debug(void);
BOOL APP_A2DPS_PlaySbcAudio(void);
BOOL APP_A2DPS_StopSbcAudio(void);
BOOL APP_A2DP_OpenSbcFile(void);
BOOL APP_A2DP_CloseSbcFile(void);
void APP_A2DP_StartPcmStream(BTHAL_U32 streamId);

void APP_A2DP_SendAvrcpOperation(U32 channelId, AvrcpPanelOperation avrcpPanelOp);
void APP_A2DP_UpdateAvrcpConnection(U32 channelId, BD_ADDR *bdAdd, BOOL isChannelConnected);

static void checkForStartStream(BtlA2dpStreamId streamId);

static void APP_A2DPS_CallBack(const BtlA2dpEvent *event);
extern void bthalMmSetPcmPlatformSupportedCapabilities(BthalMmSbcInfo *supportedCap);
extern void bthalMmSetMp3PlatformSupportedCapabilities(BthalMmMpeg1_2_audioInfo *supportedCap);
extern BOOL AppMp3OpenFile(const char *FileName);
extern void AppMp3CloseFile(void);
extern void AppMp3PlayFile(void);
extern void AppStopAudioFile(void);
extern BOOL AppSbcOpenFile(const char *FileName);
extern void AppSbcCloseFile(void);
extern void AppSbcPlayFile(void);

#ifdef BTHAL_PCM_FILE
extern BOOL AppPcmOpenFile(const char *FileName, BTHAL_U16 sampleFreq, BTHAL_U8 numChannels);
extern void AppPcmCloseFile(void);
extern void AppPcmPlayFile(void);
#endif	/* BTHAL_PCM_FILE */

#ifdef BTHAL_PCM_SIMULATOR
extern BOOL AppPcmSimOpenFile(const char *FileName);
extern void AppPcmSimCloseFile(void);
extern void AppPcmSimPlayFile(void);
extern void AppPcmSimStopFile(void);
BOOL APP_A2DPS_PlayPcmSimAudio(void);
BOOL APP_A2DPS_StopPcmSimAudio(void);
BOOL APP_A2DP_OpenPcmSimFile(void);
BOOL APP_A2DP_ClosePcmSimFile(void);
#endif /* BTHAL_PCM_SIMULATOR */

extern void bthalMmSetExternalSbcEncoder(BOOL externalEncoder);
extern void bthalMmStreamStateIsConnected(U32 streamId, BOOL isConnected);
extern void APP_AVRCPTG_UpdateAvrcpConnection(U32 streamId, BD_ADDR *bdAdd, BOOL isChannelConnected);

#if SBC_ENCODER == XA_ENABLED
extern void bthalMmReportHsConnectionState(BTHAL_U8 streamId, BOOL streamIsOpen);
extern void bthalMmReportHsStartingState(BTHAL_U8 streamId, BOOL streamIsStarted);
#endif /* SBC_ENCODER == XA_ENABLED */






/****************************************************************************
 *
 * RAM data
 *
 ****************************************************************************/
 
/*
#ifdef BTHAL_PCM_SIMULATOR
#define A2DP_COMMANDS_NUM 27 
#else
#define A2DP_COMMANDS_NUM 27 
#endif */ /* BTHAL_PCM_SIMULATOR */


#undef BMI_STUB

#ifdef BMI_STUB
BOOL APP_A2DPS_BmiEventPlay(void);
BOOL APP_A2DPS_BmiEventStop(void);
BOOL APP_A2DPS_BmiEventPause(void);
BOOL APP_A2DPS_BmiEventResume(void);
BOOL APP_A2DPS_BmiEventTrackStart(void);
BOOL APP_A2DPS_BmiEventTrackEnd(void);
BOOL APP_A2DPS_l1AudioCfg(void);
extern void l1audio_CallConfigCallback(int sampleRate);

#endif  /* BMI_STUB */

typedef BOOL (*FuncType)(void);

typedef struct
{
	char 		funcName[LINE_PARSER_MAX_STR_LEN];
	FuncType    funcPtr;
} _BtlA2dpCommands;

/* A2DP commands array */
static const  _BtlA2dpCommands btlA2dpCommands[] =  {   
														{"connectmp3stream",	APP_A2DPS_ConnectMp3Stream},
														{"connectsbcstream",	APP_A2DPS_ConnectSbcStream},
														{"open",				APP_A2DPS_OpenStream},
														{"close",				APP_A2DPS_CloseStream},
														{"idle",				APP_A2DPS_IdleStream},
														{"start",				APP_A2DPS_StartStream},
														{"suspend",				APP_A2DPS_SuspendStream},
														{"abort",       		APP_A2DPS_Abort},
														{"playsbcaudio",		APP_A2DPS_PlaySbcAudio},
														{"StopsbcAudio",		APP_A2DPS_StopSbcAudio},
														{"playmp3audio",		APP_A2DPS_PlayMp3Audio},
														{"Stopmp3Audio",		APP_A2DPS_StopMp3Audio},
														{"openmp3file",			APP_A2DP_OpenMp3File},
														{"closemp3file",		APP_A2DP_CloseMp3File},
														{"opensbcfile",			APP_A2DP_OpenSbcFile},
														{"closesbcfile",		APP_A2DP_CloseSbcFile},
														{"setsec",      		APP_A2DPS_SetSecurityLevel},   
														{"getsec",      		APP_A2DPS_GetSecurityLevel},   
														{"enable",      		APP_A2DPS_Enable}, 		    
														{"disable",     		APP_A2DPS_Disable},		     	 
														{"create",      		APP_A2DPS_Create},			    
														{"destroy",     		APP_A2DPS_Destroy}, 
														{"debug",     		APP_A2DPS_Debug}, //???
#ifdef BTHAL_PCM_FILE
														{"playpcmaudio",		APP_A2DPS_PlayPcmAudio},
														{"StoppcmAudio",		APP_A2DPS_StopPcmAudio},
														{"openpcmfile",			APP_A2DP_OpenPcmFile},
														{"closepcmfile",		APP_A2DP_ClosePcmFile},
#endif	/* BTHAL_PCM_FILE */
#ifdef BTHAL_PCM_SIMULATOR
														{"playpcmsimaudio",		APP_A2DPS_PlayPcmSimAudio},
														{"stoppcmsimAudio",		APP_A2DPS_StopPcmSimAudio},
														{"openpcmsimfile",		APP_A2DP_OpenPcmSimFile},
														{"closepcmsimfile",		APP_A2DP_ClosePcmSimFile},
#endif
#ifdef BMI_STUB
														{"bmiPlay",			APP_A2DPS_BmiEventPlay},
														{"bmiStop",			APP_A2DPS_BmiEventStop},
														{"bmiPause",			APP_A2DPS_BmiEventPause},
														{"bmiResume",		APP_A2DPS_BmiEventResume},
														{"bmiTrackStart",		APP_A2DPS_BmiEventTrackStart},
														{"bmiTrackEnd",		APP_A2DPS_BmiEventTrackEnd},
														{"l1AudioCfg",			APP_A2DPS_l1AudioCfg},
#endif  /* BMI_STUB */
														{"init",        		(FuncType)A2DPA_Init} 
											        };

 #define A2DP_COMMANDS_NUM  (sizeof(btlA2dpCommands) / sizeof(_BtlA2dpCommands))

BOOL APP_A2DPS_Debug(void)
{
	LINE_PARSER_STATUS  ParserStatus;
    U32                 tMask;
	
	ParserStatus = LINE_PARSER_GetNextU32(&tMask, TRUE);

	if (ParserStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("A2DP wrong debugMask."));
		//return FALSE;
	}
	else
	{
        tDbgBthalMm.uDebugMask = tMask;
	}

    /* print BTAHL MM debug statistics */
    BTHAL_MM_DEBUG_Print();

	return TRUE;
}


/*******************************************************************************
 * 
 * A2DP PCO commands 
 *
 *******************************************************************************
 *
 * connectmp3stream <bdadd>		- open signaling channel for mp3 streaming
 *
 * connectsbcstream <bdadd>		- open signaling channel for sbc streaming
 *
 * open <streamId>				- open data l2cap channel
 *
 * close <streamId>				- close data and signaling channel
 *
 * idle <streamId>				- change stream state to idle (only signaling channel exist)
 *
 * abort <streamId>		    	- abort a streaming/open stream (change stream state to idle)
 *
 * suspend <streamId>			- change stream state from streaming to open
 *
 * start <streamId>				- change stream state from open to start
 *
 * playsbcaudio  				- start playing sbc audio
 *
 * StopsbcAudio 				- stop playing sbc audio
 *
 * playmp3audio  				- start playing mp3 audio
 *
 * Stopmp3Audio 				- stop playing mp3 audio
 *
 * openmp3file <fullpath>		- open mp3 file. Input parameter is the full path of the file 
 *								  include file name in the FFS
 *
 * closemp3file					- close mp3 file
 *
 * opensbcfile <fullpath>		- open sbc file. Input parameter is the full path of the file 
 *								  include file name in the FFS
 *
 * closesbcfile					- close sbc file
 *
 * setsec <seclevel> 			- Set secutiry level. 
 *							 	 The <seclevel> parameter is a 10 base number representing the security level. 
 *							    (relevant only in security mode 2) For example:
 *					 	         3 - means BSL_AUTHENTICATION_IN and BSL_AUTHORIZATION_IN
 *						         25 - means BSL_AUTHENTICATION_IN and BSL_AUTHENTICATION_OUT and BSL_AUTHORIZATION_OUT which is 
 *						        (0x01 + 0x08 + 0x10) = (1 + 8 + 16)
 *
 *							 	0x01 :  BSL_AUTHENTICATION_IN  -  Authentication on incoming required 
 *							 	0x02 :  BSL_AUTHORIZATION_IN   -  Authorization on incoming required 
 *								0x04:  BSL_ENCRYPTION_IN      -  Encryption on incoming required 
 *
 *								0x08:  BSL_AUTHENTICATION_OUT  -  Authentication on outgoing required 
 *								0x10:  BSL_AUTHORIZATION_OUT   -  Authorization on outgoing required 
 *								0x20:  BSL_ENCRYPTION_OUT      -  Encryption on outgoing required 
 *								0x40:  BSL_BROADCAST_IN        -  Reception of broadcast packets allowed 
 *
 *							    To configure BSL_NO_SECURITY, insert 0.	
 *
 * getsec 						- Get security level
 *
 * enable <externalEncoder(TURE/FALSE)>	- enable A2DP profile. externalEncoder = FALSE if ESI encoder is in use
 *
 * disable 						- disable A2DP profile
 *
 * create 						- allocate A2DP resources. 
 * 
 * destroy 						- free A2DP resources
 *
 * PCM SIMULATOR only :
 * playpcmsimaudio  			- start playing pcm audio
 *
 * stoppcmsimAudio 				- stop playing pcm audio
 *
 * openpcmsimfile <fullpath>	- open pcm file. Input parameter is the full path of the file 
 *								  include file name in the FFS
 *
 * closepcmsimfile				- close pcm file from FFS/RAM
 *
 *******************************************************************************/


/*******************************************************************************
*  FUNCTION:	A2DPA_ProcessUserAction
*                                                                         
*  PARAMETERS:	msg - Parameters of actions.
*                                                                         
*  DESCRIPTION:	Processes primitives for the A2DP application
*                                                                         
*  RETURNS:		None.
*******************************************************************************/
void A2DPA_ProcessUserAction(U8 *msg)
{
	U8 i;
	char command[LINE_PARSER_MAX_STR_LEN];
	LINE_PARSER_STATUS status;

	Report(("A2DP primitive: '%s' ", msg));

	status = LINE_PARSER_GetNextStr((U8*)command, LINE_PARSER_MAX_STR_LEN);

	if (status != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("Wrong A2DP command. Can't parse line"));
		return;
	}

	/* Look for the correct function */
	for(i = 0; i < A2DP_COMMANDS_NUM; i++)
	{
		if (OS_StrCmp(command, btlA2dpCommands[i].funcName) == 0)
		{
			btlA2dpCommands[i].funcPtr();
			return;
		}
	}
	Report(("Wrong A2DP command"));
	
}

/*---------------------------------------------------------------------------
 *            A2DPA_Init
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Init the A2DP server.		
 *
 * Return:    void
 *
 */
void A2DPA_Init(void)
{
	BtStatus status;
	BthalMmSbcInfo supportedSbcCap;
	BthalMmMpeg1_2_audioInfo supportedMp3Cap;
	int streamId;

	BMI_RegisterEventCallback(NULL);
	
	status = BTL_A2DP_Create(0, APP_A2DPS_CallBack, &btlA2dpSecurityLevel, &appA2dpContext);

	Report(("BTL_A2DP_Create() returned %s.",pBT_Status(status)));

	if(status != BT_STATUS_SUCCESS)
		return;

#ifdef BTHAL_PCM_FILE
	/*  We do NOT support 16KHz & 32KHz in order to test the SRC */
	supportedSbcCap.samplingFreq = (BTHAL_MM_SBC_SAMPLING_FREQ_44100 | BTHAL_MM_SBC_SAMPLING_FREQ_48000);
#else
	supportedSbcCap.samplingFreq = (BTHAL_MM_SBC_SAMPLING_FREQ_16000 | BTHAL_MM_SBC_SAMPLING_FREQ_32000 | BTHAL_MM_SBC_SAMPLING_FREQ_44100 | BTHAL_MM_SBC_SAMPLING_FREQ_48000);
#endif	/* BTHAL_PCM_FILE */
	supportedSbcCap.channelMode = (BTHAL_MM_SBC_CHANNEL_MODE_MONO | BTHAL_MM_SBC_CHANNEL_MODE_DUAL_CHANNEL | BTHAL_MM_SBC_CHANNEL_MODE_STEREO | BTHAL_MM_SBC_CHANNEL_MODE_JOINT_STEREO);
	supportedSbcCap.blockLength = (BTHAL_MM_SBC_BLOCK_LENGTH_4 | BTHAL_MM_SBC_BLOCK_LENGTH_8 | BTHAL_MM_SBC_BLOCK_LENGTH_12 | BTHAL_MM_SBC_BLOCK_LENGTH_16);
	supportedSbcCap.subbands = (BTHAL_MM_SBC_SUBBANDS_4 | BTHAL_MM_SBC_SUBBANDS_8);
	supportedSbcCap.allocationMethod = (BTHAL_MM_SBC_ALLOCATION_METHOD_SNR | BTHAL_MM_SBC_ALLOCATION_METHOD_LOUDNESS);
	supportedSbcCap.maxBitpoolValue = 32;
	supportedSbcCap.minBitpoolValue = 2;
	
	bthalMmSetPcmPlatformSupportedCapabilities(&supportedSbcCap);

	supportedMp3Cap.bitRate = 0xFFFF;
	supportedMp3Cap.channelMode = (BTHAL_MM_MPEG1_2_AUDIO_CHANNEL_MODE_MONO | BTHAL_MM_MPEG1_2_AUDIO_CHANNEL_MODE_DUAL_CHANNEL | BTHAL_MM_MPEG1_2_AUDIO_CHANNEL_MODE_STEREO | BTHAL_MM_MPEG1_2_AUDIO_CHANNEL_MODE_JOINT_STEREO);
	supportedMp3Cap.crcProtection = BTHAL_MM_MPEG1_2_AUDIO_CRC_PROTECTION_SUPPORTED;
	supportedMp3Cap.layer = (BTHAL_MM_MPEG1_2_AUDIO_LAYER_1 | BTHAL_MM_MPEG1_2_AUDIO_LAYER_2 | BTHAL_MM_MPEG1_2_AUDIO_LAYER_3);
	supportedMp3Cap.mpf2 = BTHAL_MM_MPEG1_2_AUDIO_MPF2_SUPPORTED;
	supportedMp3Cap.samplingFreq = (BTHAL_MM_MPEG1_2_AUDIO_SAMPLING_FREQ_16000 | BTHAL_MM_MPEG1_2_AUDIO_SAMPLING_FREQ_22050 | BTHAL_MM_MPEG1_2_AUDIO_SAMPLING_FREQ_24000 | BTHAL_MM_MPEG1_2_AUDIO_SAMPLING_FREQ_32000 | BTHAL_MM_MPEG1_2_AUDIO_SAMPLING_FREQ_44100 | BTHAL_MM_MPEG1_2_AUDIO_SAMPLING_FREQ_48000);
	supportedMp3Cap.vbr = BTHAL_MM_MPEG1_2_AUDIO_VBR_SUPPORTED;

	bthalMmSetMp3PlatformSupportedCapabilities(&supportedMp3Cap);

	bthalMmSetExternalSbcEncoder(externalSbcEncoder);

	status = BTL_A2DP_Enable(appA2dpContext, 0, (BTL_A2DP_SUPPORTED_FEATURES_PLAYER | BTL_A2DP_SUPPORTED_FEATURES_MIXER));

	if (status == BT_STATUS_SUCCESS)
	{
		BMI_RegisterEventCallback(BT_NotifyBmiEvent);
	}
	
	for(streamId=0; streamId < A2DP_MAX_NUM_STREAMS_PER_CONTEXT; streamId++)
		OS_MemSet(&(streamIdToBdadd[streamId]), 0, sizeof(BD_ADDR));
	
   	Report(("BTL_A2DP_Enable() returned %s.",pBT_Status(status)));	
}

/*---------------------------------------------------------------------------
 *            A2DPA_Deinit
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deinitialize and Destroy the A2DP server.
 *
 * Return:    void
 *
 */
void A2DPA_Deinit(void)
{
	BtStatus status;

	BMI_RegisterEventCallback(NULL);

	status = BTL_A2DP_Disable(appA2dpContext);
	
    Report(("BTL_A2DP_Disable() returned %s.",pBT_Status(status)));
	
	status =  BTL_A2DP_Destroy(&appA2dpContext);
	
    Report(("BTL_A2DP_Destroy() returned %s.",pBT_Status(status)));
}

/*---------------------------------------------------------------------------
 *            APP_A2DPS_ConnectSbcStream
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Open signaling channel for SBC stream		
 *
 * Return:    void
 *
 */
BOOL APP_A2DPS_ConnectSbcStream(void)
{

	U8 					BdArray[17];
	BtStatus    		status;
	BD_ADDR 			BdAddr;
	BtlA2dpStreamId     streamId;
	LINE_PARSER_STATUS  ParserStatus;
	ParserStatus = LINE_PARSER_GetNextStr(BdArray, 17);

	if (ParserStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("A2DP wrong BD addr. Can't connect SBC stream."));
		return FALSE;
	}
	
	BdAddr = bdaddr_aton((const char *)(BdArray));

	status = BTL_A2DP_ConnectStream(appA2dpContext,&BdAddr, BTL_A2DP_CODEC_SBC, &streamId);
	Report(("BTL_A2DP_ConnectStream returned %s, streamId = %d", pBT_Status(status), streamId));

	return ((status == BT_STATUS_SUCCESS) ? (TRUE) : (FALSE));
	
}

/*---------------------------------------------------------------------------
 *            APP_A2DPS_ConnectMp3Stream
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Open signaling channel for MP3 stream		
 *
 * Return:    void
 *
 */
BOOL APP_A2DPS_ConnectMp3Stream(void)
{

	U8 					BdArray[17];
	BtStatus    		status;
	BD_ADDR 			BdAddr;
	LINE_PARSER_STATUS  ParserStatus;
	BtlA2dpStreamId     streamId;
	ParserStatus = LINE_PARSER_GetNextStr(BdArray, 17);

	if (ParserStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("A2DP wrong BD addr. Can't connect MP3 stream."));
		return FALSE;
	}
	
	BdAddr = bdaddr_aton((const char *)(BdArray));

	status = BTL_A2DP_ConnectStream(appA2dpContext, &BdAddr, BTL_A2DP_CODEC_MPEG1_2_AUDIO, &streamId);
	Report(("BTL_A2DP_ConnectStream returned %s, streamId = %d", pBT_Status(status), streamId));
	
	return ((status == BT_STATUS_SUCCESS) ? (TRUE) : (FALSE));
	
}

/*---------------------------------------------------------------------------
 *            APP_A2DPS_OpenStream
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Open data channel		
 *
 * Return:    void
 *
 */
BOOL APP_A2DPS_OpenStream(void)
{
	U32 				StreamId;
	BtStatus    		status;
	LINE_PARSER_STATUS  ParserStatus;
	
	ParserStatus = LINE_PARSER_GetNextU32(&StreamId, FALSE);

	if (ParserStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("A2DP wrong stream ID. Can't open stream."));
		return FALSE;
	}

	status = BTL_A2DP_OpenStream(appA2dpContext, (BtlA2dpStreamId)(StreamId));
	Report(("BTL_A2DP_OpenStream returned %s", pBT_Status(status)));

	return ((status == BT_STATUS_SUCCESS) ? (TRUE) : (FALSE));
}

/*---------------------------------------------------------------------------
 *            APP_A2DPS_CloseStream
 *---------------------------------------------------------------------------
 *
 * Synopsis:  close existing connection		
 *
 * Return:    void
 *
 */
BOOL APP_A2DPS_CloseStream(void)
{
	U32 				StreamId;
	BtStatus    		status;
	LINE_PARSER_STATUS  ParserStatus;
	
	ParserStatus = LINE_PARSER_GetNextU32(&StreamId, FALSE);

	if (ParserStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("A2DP wrong stream ID. Can't close stream."));
		return FALSE;
	}
	
	status = BTL_A2DP_CloseStream(appA2dpContext, (BtlA2dpStreamId)(StreamId));
	Report(("BTL_A2DP_CloseStream returned %s", pBT_Status(status)));

	return ((status == BT_STATUS_SUCCESS) ? (TRUE) : (FALSE));
}


/*---------------------------------------------------------------------------
 *            APP_A2DPS_IdleStream
 *---------------------------------------------------------------------------
 *
 * Synopsis:  idle existing connection		
 *
 * Return:    void
 *
 */
BOOL APP_A2DPS_IdleStream(void)
{
	U32 				StreamId;
	BtStatus    		status;
	LINE_PARSER_STATUS  ParserStatus;
	
	ParserStatus = LINE_PARSER_GetNextU32(&StreamId, FALSE);

	if (ParserStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("A2DP wrong stream ID. Can't idle stream."));
		return FALSE;
	}

	status = BTL_A2DP_IdleStream(appA2dpContext, (BtlA2dpStreamId)(StreamId));
	Report(("BTL_A2DP_IdleStream returned %s", pBT_Status(status)));
	
	return ((status == BT_STATUS_SUCCESS) ? (TRUE) : (FALSE));
}


/*---------------------------------------------------------------------------
 *            APP_A2DPS_Abort
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Abort existing connection		
 *
 * Return:    void
 *
 */
BOOL APP_A2DPS_Abort(void)
{
	U32 				StreamId;
	BtStatus    		status;
	LINE_PARSER_STATUS  ParserStatus;
	
	ParserStatus = LINE_PARSER_GetNextU32(&StreamId, FALSE);

	if (ParserStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("A2DP wrong stream ID. Can't abort stream."));
		return FALSE;
	}

	status = BTL_A2DP_AbortStream(appA2dpContext, (BtlA2dpStreamId)(StreamId));
	Report(("BTL_A2DP_AbortStream returned %s", pBT_Status(status)));

	return ((status == BT_STATUS_SUCCESS) ? (TRUE) : (FALSE));
}


/*---------------------------------------------------------------------------
 *            APP_A2DPS_SuspendStream
 *---------------------------------------------------------------------------
 *
 * Synopsis:  suspend streaming stream		
 *
 * Return:    void
 *
 */
BOOL APP_A2DPS_SuspendStream(void)
{
	U32 				StreamId;
	BtStatus    		status;
	LINE_PARSER_STATUS  ParserStatus;
	
	ParserStatus = LINE_PARSER_GetNextU32(&StreamId, FALSE);

	if (ParserStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("A2DP wrong stream ID. Can't suspend stream."));
		return FALSE;
	}

	status = BTL_A2DP_SuspendStream(appA2dpContext, (BtlA2dpStreamId)(StreamId));
	Report(("BTL_A2DP_SuspendStream returned %s", pBT_Status(status)));

	return ((status == BT_STATUS_SUCCESS) ? (TRUE) : (FALSE));
}


/*---------------------------------------------------------------------------
 *            APP_A2DPS_StartStream
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Change stream state from open to start		
 *
 * Return:    void
 *
 */
BOOL APP_A2DPS_StartStream(void)
{
	U32 				StreamId;
	LINE_PARSER_STATUS  ParserStatus;
	
	ParserStatus = LINE_PARSER_GetNextU32(&StreamId, FALSE);

	if (ParserStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("A2DP wrong stream ID. Can't start stream."));
		return FALSE;
	}
	
	checkForStartStream((BtlA2dpStreamId)(StreamId));

	return TRUE;
}


/*---------------------------------------------------------------------------
 *            APP_A2DPS_PlayMp3Audio
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Play MP3 file from FFS		
 *
 * Return:    void
 *
 */
BOOL APP_A2DPS_PlayMp3Audio(void)
{
	AppMp3PlayFile();
	return TRUE;
}


/*---------------------------------------------------------------------------
 *            APP_A2DPS_StopMp3Audio
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Stop playing MP3 file from FFS		
 *
 * Return:    void
 *
 */
BOOL APP_A2DPS_StopMp3Audio(void)
{
	AppStopAudioFile();
	return TRUE;
}


/*---------------------------------------------------------------------------
 *            APP_A2DP_OpenMp3File
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Open MP3 file from FFS		
 *
 * Return:    void
 *
 */
BOOL APP_A2DP_OpenMp3File(void)
{
	BOOL 				status;
	LINE_PARSER_STATUS	lineStatus;
	char				fullPathFile[256];
	
	lineStatus = LINE_PARSER_GetNextStr((U8*)fullPathFile, LINE_PARSER_MAX_STR_LEN);

	if (lineStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("A2DP: Can't read full path file to push. Ignoring open file command"));
		return FALSE;
	}
	
	status = AppMp3OpenFile(fullPathFile);

	if(status != TRUE)
	{
		Report(("A2DP: open MP3 file failed"));
		return FALSE;
	}

	Report(("Open Mp3 file finished"));

	return TRUE;
}


/*---------------------------------------------------------------------------
 *            APP_A2DP_CloseMp3File
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Close MP3 file		
 *
 * Return:    void
 *
 */
BOOL APP_A2DP_CloseMp3File(void)
{
	AppMp3CloseFile();
	Report(("Closing Mp3 file finished"));

	return TRUE;
}

#ifdef BTHAL_PCM_FILE

/*---------------------------------------------------------------------------
 *            APP_A2DPS_PlayPcmAudio
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Play Pcm file from FFS		
 *
 * Return:    void
 *
 */
BOOL APP_A2DPS_PlayPcmAudio(void)
{
	AppPcmPlayFile();
	return TRUE;
}

/*---------------------------------------------------------------------------
 *            APP_A2DPS_StopPcmAudio
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Stop playing PCM file from FFS		
 *
 * Return:    void
 *
 */
BOOL APP_A2DPS_StopPcmAudio(void)
{
	AppStopAudioFile();
	return TRUE;
}

/*---------------------------------------------------------------------------
 *            APP_A2DP_OpenPcmFile
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Open PCM file from FFS		
 *
 * Return:    void
 *
 */
BOOL APP_A2DP_OpenPcmFile(void)
{
	BOOL 				status;
	LINE_PARSER_STATUS	lineStatus;
	char				fullPathFile[256];
	U16					sampleFreq;
	U8					numChannels;
	
	lineStatus = LINE_PARSER_GetNextStr((U8*)fullPathFile, LINE_PARSER_MAX_STR_LEN);
	if (lineStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("A2DP: Can't read full path file to push. Ignoring open file command"));
		return FALSE;
	}

	lineStatus = LINE_PARSER_GetNextU16( &sampleFreq, FALSE);
	if (lineStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("A2DP: Can't read sample frequency. Ignoring open file command"));
		return FALSE;
	}

	lineStatus = LINE_PARSER_GetNextU8( &numChannels, FALSE);
	if (lineStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("A2DP: Can't read number of channels. Ignoring open file command"));
		return FALSE;
	}
	
	status = AppPcmOpenFile(fullPathFile, sampleFreq, numChannels);

	if(status != TRUE)
	{
		Report(("A2DP: open PCM file failed"));
		return FALSE;
	}

	Report(("Open PCM file finished"));

	return TRUE;
}

/*---------------------------------------------------------------------------
 *            APP_A2DP_ClosePcmFile
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Close PCM file		
 *
 * Return:    void
 *
 */
BOOL APP_A2DP_ClosePcmFile(void)
{
	AppPcmCloseFile();
	Report(("Closing PCM file finished"));

	return TRUE;
}

#endif	/* BTHAL_PCM_FILE */


/*---------------------------------------------------------------------------
 *            APP_A2DPS_PlaySbcAudio
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Play SBC file from FFS		
 *
 * Return:    void
 *
 */
BOOL APP_A2DPS_PlaySbcAudio(void)
{
	AppSbcPlayFile();
	return TRUE;
}


/*---------------------------------------------------------------------------
 *            APP_A2DPS_StopSbcAudio
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Stop playing SBC file from FFS		
 *
 * Return:    void
 *
 */
BOOL APP_A2DPS_StopSbcAudio(void)
{
	AppStopAudioFile();
	return TRUE;
}


/*---------------------------------------------------------------------------
 *            APP_A2DP_OpenSbcFile
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Open SBC file from FFS		
 *
 * Return:    void
 *
 */
BOOL APP_A2DP_OpenSbcFile(void)
{
	BOOL 				status;
	LINE_PARSER_STATUS	lineStatus;
	char				fullPathFile[256];
	
	lineStatus = LINE_PARSER_GetNextStr((U8*)fullPathFile, LINE_PARSER_MAX_STR_LEN);

	if (lineStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("A2DP: Can't read full path file to push. Ignoring open file command"));
		return FALSE;
	}
	
	status = AppSbcOpenFile(fullPathFile);

	if(status != TRUE)
	{
		Report(("A2DP: open SBC file failed"));
		return FALSE;
	}

	Report(("Open Sbc audio file finished"));

	return TRUE;
}


/*---------------------------------------------------------------------------
 *            APP_A2DP_CloseSbcFile
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Close SBC file		
 *
 * Return:    void
 *
 */
BOOL APP_A2DP_CloseSbcFile(void)
{
	AppSbcCloseFile();
	Report(("Close Sbc audio file finished"));

	return TRUE;
}

/*---------------------------------------------------------------------------
 *            APP_A2DPS_SetSecurityLevel
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Sets security level for the given A2DP context.
 *
 * Return:    void
 *
 */
BOOL APP_A2DPS_SetSecurityLevel(void)
{
	BtStatus status;
	BtSecurityLevel newSecurityLevel;
	LINE_PARSER_STATUS parserStatus;
	
	parserStatus = LINE_PARSER_GetNextU8(&newSecurityLevel, TRUE);
	
	if (parserStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("PBAP: Can't read security level. Ignoring command"));
		return FALSE;
	}
	

	status = BTL_A2DP_SetSecurityLevel(appA2dpContext, &newSecurityLevel);
	
    Report(("BTL_A2DP_SetSecurityLevel() returned %s.",pBT_Status(status)));

	return ((status == BT_STATUS_SUCCESS) ? (TRUE) : (FALSE));
}

/*---------------------------------------------------------------------------
 *            APP_A2DPS_GetSecurityLevel
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Gets security level for the given A2DP context.
 *
 * Return:    void
 *
 */
BOOL APP_A2DPS_GetSecurityLevel(void)
{
	BtStatus status;
	BtSecurityLevel level;
	
	status = BTL_A2DP_GetSecurityLevel(appA2dpContext, &level);
	
    Report(("BTL_A2DP_GetSecurityLevel() returned %s.",pBT_Status(status)));

	if(level & BSL_AUTHENTICATION_IN)
	{
			Report(("Security level: BSL_AUTHENTICATION_IN"));
	}
	if(level & BSL_AUTHORIZATION_IN)
	{
			Report(("Security level: BSL_AUTHORIZATION_IN"));
	}
	if(level & BSL_ENCRYPTION_IN)
	{
			Report(("Security level: BSL_ENCRYPTION_IN"));
	}
	if(level & BSL_AUTHENTICATION_OUT)
	{
		Report(("Security level: BSL_AUTHENTICATION_OUT"));
	}
	if(level & BSL_AUTHORIZATION_OUT)
	{
		Report(("Security level: BSL_AUTHORIZATION_OUT"));
	}
	if(level & BSL_ENCRYPTION_OUT)
	{
		Report(("Security level: BSL_ENCRYPTION_OUT"));
	}
	if(level & BSL_BROADCAST_IN)
	{
		Report(("Security level: BSL_BROADCAST_IN"));
	}

	return ((status == BT_STATUS_SUCCESS) ? (TRUE) : (FALSE));
}


/*---------------------------------------------------------------------------
 *            APP_A2DPS_Enable
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Enable the A2DP profile.
 *
 * Return:    void
 *
 */
BOOL APP_A2DPS_Enable(void)
{
	BtStatus status;
	BOOL ExternalEncoder;
		
	if (LINE_PARSER_GetNextBool(&ExternalEncoder) == LINE_PARSER_STATUS_SUCCESS)
	{
#if SBC_ENCODER == XA_ENABLED
		bthalMmSetExternalSbcEncoder(ExternalEncoder);
		externalSbcEncoder = ExternalEncoder;
#else
		if ( ! ExternalEncoder )
			BTL_LOG_INFO(("NOTE: built-in SBC encoder not supported, using external SBC encoder"));
#endif /* SBC_ENCODER == XA_ENABLED */
		status = BTL_A2DP_Enable(appA2dpContext, 0, (BTL_A2DP_SUPPORTED_FEATURES_PLAYER | BTL_A2DP_SUPPORTED_FEATURES_MIXER));
		Report(("BTL_A2DP_Enable() returned %s.",pBT_Status(status)));
	}
	else
	{
		Report(("Invalid external encoder value"));
		status = FALSE;
	}	

	return ((status == BT_STATUS_SUCCESS) ? (TRUE) : (FALSE));
}

/*---------------------------------------------------------------------------
 *            APP_A2DPS_Disable
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Disable the A2DP profile.
 *
 * Return:    void
 *
 */
BOOL APP_A2DPS_Disable(void)
{
	BtStatus status;
	
	status = BTL_A2DP_Disable(appA2dpContext);
	
    Report(("BTL_A2DP_Disable() returned %s.",pBT_Status(status)));

	return ((status == BT_STATUS_SUCCESS) ? (TRUE) : (FALSE));
}


/*---------------------------------------------------------------------------
 *            APP_A2DPS_Create
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Create the A2DP server.		
 *
 * Return:    void
 *
 */
BOOL APP_A2DPS_Create(void)
{
	BtStatus status;
	
	status = BTL_A2DP_Create(0, APP_A2DPS_CallBack, &btlA2dpSecurityLevel, &appA2dpContext);

	Report(("BTL_A2DP_Create() returned %s.", pBT_Status(status)));

	return ((status == BT_STATUS_SUCCESS) ? (TRUE) : (FALSE));
}

/*---------------------------------------------------------------------------
 *            APP_A2DPS_Destroy
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Destroy the A2DP server.		
 *
 * Return:    void
 *
 */
BOOL APP_A2DPS_Destroy(void)
{
	BtStatus status;
	
	status = BTL_A2DP_Destroy(&appA2dpContext);

	Report(("BTL_A2DP_Destroy() returned %s.",pBT_Status(status)));

	return ((status == BT_STATUS_SUCCESS) ? (TRUE) : (FALSE));
}

/*---------------------------------------------------------------------------
 *            APP_A2DP_StartPcmStream
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Start a PCm stream (called from BTHAL_MM)		
 *
 * Return:    void
 *
 */
void APP_A2DP_StartPcmStream(BTHAL_U32 streamId)
{
	checkForStartStream(streamId);
}

/*---------------------------------------------------------------------------
 *            checkForStartStream
 *---------------------------------------------------------------------------
 *
 * Synopsis:  check if Starting a stream is possible and Start it if yes.		
 *
 * Return:    void
 *
 */
static void checkForStartStream(BtlA2dpStreamId streamId)
{
	BtStatus    		status;

	if ( startIndReceived[streamId] )
	{
		pendingStart[streamId] = TRUE;
	}
	else
	{
		status = BTL_A2DP_StartStream(appA2dpContext, streamId);
		Report(("BTL_A2DP_StartStream returned %s", pBT_Status(status)));
	}

}


/*---------------------------------------------------------------------------
 *            switchAudioToDefault
 *---------------------------------------------------------------------------
 *
 * Synopsis:  checks and if necessary switches audio output to default audio (handset)	
 *                 instead of to A2DP headset
 *
 * Return:    void
 *
 */
 void switchAudioToDefault(BtlA2dpStreamId streamId)
{
	BTL_LOG_INFO(("switchAudioToDefault(%d): pcmStreaming = %d ", streamId, pcmStreaming));
	if (pcmStreaming)
	{
		if (TRUE == isOpenStream[streamId])
		{
			isOpenStream[streamId] = FALSE;
			if (FALSE == isOpenStream[OTHER_SBC_STREAM(streamId)])
			{
				BMI_SwitchAudioToBtHeadset(FALSE);
			}
		}
	}
	else
	{
		isOpenStream[streamId] = FALSE;
	}
}

/*---------------------------------------------------------------------------
 *            APP_A2DPS_CallBack
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function processes OBEX server protocol events.
 *
 * Return:    void
 *
 */
static void APP_A2DPS_CallBack(const BtlA2dpEvent *event)
{
	A2dpCallbackParms 		*Parms;
	BtStatus    			status;

	Parms = event->callbackParms;
	
    switch (event->callbackParms->event) 
	{
		case A2DP_EVENT_STREAM_OPEN_IND:
			UI_DISPLAY(("UI_EVENT: A2DP_EVENT_STREAM_OPEN_IND streamId %d.",event->streamId));
			BTL_A2DP_OpenStreamResponse(appA2dpContext, event->streamId, A2DP_ERR_NO_ERROR);
			UI_DISPLAY(("Incomming connection accepted"));
	        break;

		case A2DP_EVENT_GET_CONFIG_IND:
			bthalMmStreamStateIsConnected(event->streamId, TRUE);
			pcmStreaming = IS_SBC_STREAM(event->streamId) && !externalSbcEncoder;
			startIndReceived[event->streamId] = FALSE;
			pendingStart[event->streamId] = FALSE;
			UI_DISPLAY(("UI_EVENT: A2DP_EVENT_GET_CONFIG_IND streamId %d.", event->streamId));
			UI_DISPLAY(("stream state = IDLE, please send 'open' command "));
			break;

		case A2DP_EVENT_STREAM_OPEN:
			
			bthalMmStreamStateIsConnected(event->streamId, TRUE);
			pcmStreaming = IS_SBC_STREAM(event->streamId) && !externalSbcEncoder;
			sbcStream = IS_SBC_STREAM(event->streamId);
			startIndReceived[event->streamId] = FALSE;
			pendingStart[event->streamId] = FALSE;
#if SBC_ENCODER == XA_ENABLED
			bthalMmReportHsConnectionState(event->streamId, TRUE);
			bthalMmReportHsStartingState(event->streamId, FALSE);
#endif /* SBC_ENCODER == XA_ENABLED */

			UI_DISPLAY(("UI_EVENT: A2DP_EVENT_STREAM_OPEN streamId %d.", event->streamId));
		
			if ( IS_SBC_STREAM(event->streamId) )
			{
				if ( FALSE == isOpenStream[event->streamId] )
				{
					isOpenStream[event->streamId] = TRUE;
					if (!externalSbcEncoder)
					{
						if (FALSE == (a2dpStreamingGrantedByBmi = BMI_SwitchAudioToBtHeadset(TRUE)))
						{
							BTL_LOG_ERROR(("BMI refused to play the audio on the A2DP headset :-((( "));
						}
					}
				}
			}
			else
			{
				isOpenStream[event->streamId] = TRUE;
			}

			status = BTL_A2DP_GetConnectedDevice(appA2dpContext, event->streamId, &(streamIdToBdadd[event->streamId]));

			if(status == BT_STATUS_SUCCESS)
			{
				APP_AVRCPTG_UpdateAvrcpConnection(event->streamId, &(streamIdToBdadd[event->streamId]), TRUE);
			}
			
			break;

		case A2DP_EVENT_STREAM_IDLE:

#if SBC_ENCODER == XA_ENABLED
			bthalMmReportHsStartingState(event->streamId, FALSE);
			bthalMmReportHsConnectionState(event->streamId, FALSE);
#endif /* SBC_ENCODER == XA_ENABLED */

				UI_DISPLAY(("UI_EVENT: A2DP_EVENT_STREAM_IDLE streamId %d.", event->streamId));

				switchAudioToDefault(event->streamId);

			APP_AVRCPTG_UpdateAvrcpConnection(event->streamId, &(streamIdToBdadd[event->streamId]), FALSE);
			OS_MemSet(&(streamIdToBdadd[event->streamId]), 0, sizeof(BD_ADDR));

			break;

		case A2DP_EVENT_STREAM_CLOSED:

				bthalMmStreamStateIsConnected(event->streamId, FALSE);

#if SBC_ENCODER == XA_ENABLED
				bthalMmReportHsStartingState(event->streamId, FALSE);
				bthalMmReportHsConnectionState(event->streamId, FALSE);
#endif /* SBC_ENCODER == XA_ENABLED */

				UI_DISPLAY(("UI_EVENT: A2DP_EVENT_STREAM_CLOSED streamId %d.", event->streamId));

				switchAudioToDefault(event->streamId);

			APP_AVRCPTG_UpdateAvrcpConnection(event->streamId, &(streamIdToBdadd[event->streamId]), FALSE);
			OS_MemSet(&(streamIdToBdadd[event->streamId]), 0, sizeof(BD_ADDR));

			break;

		case A2DP_EVENT_STREAM_START_IND:
	        UI_DISPLAY(("UI_EVENT: A2DP_EVENT_STREAM_START_IND streamId %d.", event->streamId));
			startIndReceived[event->streamId] = TRUE;
			BTL_A2DP_StartStreamResponse(appA2dpContext, event->streamId, A2DP_ERR_NO_ERROR);
			UI_DISPLAY(("Incoming start accepted"));
        	break;

		case A2DP_EVENT_STREAM_STARTED:

			if (startIndReceived[event->streamId])
			{
				UI_DISPLAY(("Suspending stream when Started (because headset started the stream..)"));
				BTL_A2DP_SuspendStream(appA2dpContext, event->streamId);
				break;
			}
#if SBC_ENCODER == XA_ENABLED
			bthalMmReportHsStartingState(event->streamId, TRUE);
#endif /* SBC_ENCODER == XA_ENABLED */

			UI_DISPLAY(("UI_EVENT: A2DP_EVENT_STREAM_STARTED streamId %d.", event->streamId));
			break;

		 case A2DP_EVENT_STREAM_SUSPENDED:
			if ( startIndReceived[event->streamId] )
			{
				startIndReceived[event->streamId] = FALSE;
				if ( pendingStart[event->streamId] )
				{
					UI_DISPLAY(("Starting stream after it was suspended (because headset started the stream..)"));
					pendingStart[event->streamId] = FALSE;
					status = BTL_A2DP_StartStream(appA2dpContext, event->streamId);
					Report(("BTL_A2DP_StartStream returned %s", pBT_Status(status)));
				}
			}

#if SBC_ENCODER == XA_ENABLED
		 	bthalMmReportHsConnectionState(event->streamId, TRUE);
			bthalMmReportHsStartingState(event->streamId, FALSE);
#endif /* SBC_ENCODER == XA_ENABLED */

			UI_DISPLAY(("UI_EVENT: A2DP_EVENT_STREAM_SUSPENDED streamId %d.", event->streamId));
			if (Parms->error != AVDTP_ERR_NO_ERROR)
			{
				UI_DISPLAY(("Stream Suspend Failed"));
			}
			break;

		case A2DP_EVENT_STREAM_ABORTED:

#if SBC_ENCODER == XA_ENABLED
			bthalMmReportHsStartingState(event->streamId, FALSE);
			bthalMmReportHsConnectionState(event->streamId, FALSE);
#endif /* SBC_ENCODER == XA_ENABLED */

			UI_DISPLAY(("UI_EVENT: A2DP_EVENT_STREAM_ABORTED streamId %d.", event->streamId));
	        if (Parms->error != AVDTP_ERR_NO_ERROR) {
	            UI_DISPLAY(("UI_EVENT: Stream Abort Failed"));
			}
			switchAudioToDefault(event->streamId);

			APP_AVRCPTG_UpdateAvrcpConnection(event->streamId, &(streamIdToBdadd[event->streamId]), FALSE);
			OS_MemSet(&(streamIdToBdadd[event->streamId]), 0, sizeof(BD_ADDR));
			
			break;

		case A2DP_EVENT_DISABLED:
			UI_DISPLAY(("UI_EVENT: A2DP_EVENT_DISABLED"));
			break;

		case A2DP_EVENT_STREAM_CONFIGURED:
			UI_DISPLAY(("UI_EVENT: A2DP_EVENT_STREAM_CONFIGURED"));
			if (isStreamNeedsToBeStarted[event->streamId] == TRUE)
			{
				BTL_A2DP_StartStream(appA2dpContext, event->streamId);
				isStreamNeedsToBeStarted[event->streamId] = FALSE;
			}
			
			break;

		case A2DP_EVENT_STREAM_CONFIGURATION_NEEDED:
			UI_DISPLAY(("UI_EVENT: A2DP_EVENT_STREAM_CONFIGURATION_NEEDED"));
			status = BTL_A2DP_SuspendStream(appA2dpContext, event->streamId);
			if(status == BT_STATUS_PENDING)
				isStreamNeedsToBeStarted[event->streamId] = TRUE;
			break;

    default:
        UI_DISPLAY(("A2DP Unexpected event: %i", event->callbackParms->event));
        break;
    }
}


#ifdef BTHAL_PCM_SIMULATOR
/*---------------------------------------------------------------------------
 *            APP_A2DPS_PlayPcmSimAudio
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Play PCM file from FFS		
 *
 * Return:    void
 *
 */
BOOL APP_A2DPS_PlayPcmSimAudio(void)
{
	AppPcmSimPlayFile();
	Report(("Start playing Pcm audio file"));

	return TRUE;
}

/*---------------------------------------------------------------------------
 *            APP_A2DPS_StopPcmSimAudio
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Stop playing SBC file from FFS		
 *
 * Return:    void
 *
 */
BOOL APP_A2DPS_StopPcmSimAudio(void)
{
	AppPcmSimStopFile();
	Report(("Stop playing Pcm audio file"));

	return TRUE;
}


/*---------------------------------------------------------------------------
 *            APP_A2DP_OpenPcmSimFile
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Open PCM file from FFS		
 *
 * Return:    void
 *
 */
BOOL APP_A2DP_OpenPcmSimFile(void)
{
	BOOL 				status;
	LINE_PARSER_STATUS	lineStatus;
	char				fullPathFile[256];
	
	lineStatus = LINE_PARSER_GetNextStr((U8*)fullPathFile, LINE_PARSER_MAX_STR_LEN);

	if (lineStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("A2DP: Can't read full path file to push. Ignoring open file command"));
		return FALSE;
	}
	 
	status = AppPcmSimOpenFile(fullPathFile);

	if(status != TRUE)
	{
		Report(("A2DP: open PCM file failed"));
		return FALSE;
	}

	Report(("Open PCM audio file finished"));

	return TRUE;
}


/*---------------------------------------------------------------------------
 *            APP_A2DP_ClosePcmSimFile
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Close PCM file		
 *
 * Return:    void
 *
 */
BOOL APP_A2DP_ClosePcmSimFile(void)
{
	AppPcmSimCloseFile();
	Report(("Close Pcm audio file finished"));

	return TRUE;
}
#endif /* BTHAL_PCM_SIMULATOR*/


/*

     handle API to BMI app

*/



static char *eventName[] = 
{
	"",
	"USER_PLAY",
	"USER_STOP",
	"USER_PAUSE",
	"USER_RESUME",
	"TRACK_START",
	"TRACK_END",
	"UNKNOWN EVENT !!!"
};

static void a2dpsStartStreams()
{
	U32 i;
	BtStatus   status;

	if ( pcmStreaming && ! a2dpStreamingGrantedByBmi )
	{
		BTL_LOG_ERROR(("exit a2dpsStartStreams because audio is not routed to A2DP"));
		return;
	}

	for ( i = 0; i < A2DP_MAX_NUM_STREAMS_PER_CONTEXT ; i++)
	{
		if ( isOpenStream[i] )
		{
			checkForStartStream(i);
		}
	}
}

static void a2dpsStopStreams()
{
	U32 i;
	BtStatus   status;

	for ( i = 0; i < A2DP_MAX_NUM_STREAMS_PER_CONTEXT ; i++)
	{
		if ( isOpenStream[i] )
		{
			status = BTL_A2DP_IdleStream(appA2dpContext, i);
			BTL_LOG_INFO(("BTL_A2DP_IdleStream returned %s for stream %d", pBT_Status(status), i));
		}
	}
}

static void a2dpsSuspendStreams()
{
	U32 i;
	BtStatus   status;

	for ( i = 0; i < A2DP_MAX_NUM_STREAMS_PER_CONTEXT ; i++)
	{
		if ( isOpenStream[i] )
		{
			status = BTL_A2DP_SuspendStream(appA2dpContext, i);
			BTL_LOG_INFO(("BTL_A2DP_SuspendStream returned %s for stream %d", pBT_Status(status), i));
		}
	}
}

/*---------------------------------------------------------------------------
 *            BT_NotifyBmiEvent
 *---------------------------------------------------------------------------
 *
 * Synopsis:  process events from BMI		
 *
 *     A callback function that will be called by BMI to send events
 *     (of type BmiAvEvent) to BT. BMI will be able to send events
 *     after BT has registered this callback function
 *
 * Parameters:
 *		event [in] - the event sent from BMI to BT
 *
 * Return:    void
 *
 */
#if BTHAL_A2DP_MM_PCM_SUPPORT == BTL_CONFIG_ENABLED
void BT_NotifyBmiEvent(BmiAvEvent event)
{

	if (event > BMI_EVENT_TRACK_END)
		event = BMI_EVENT_TRACK_END + 1; /* just for error reporting */

	BTL_LOG_INFO(("BT_NotifyBmiEvent ( %s )", eventName[event]));

	if ( ! pcmStreaming )
		return;

	switch (event)
	{
		case BMI_EVENT_USER_RESUME:
		case BMI_EVENT_USER_PLAY:		
			a2dpsStartStreams();
			break;
		case BMI_EVENT_USER_STOP:
		case BMI_EVENT_TRACK_END:
			/*a2dpsStopStreams();
			break;*/
		case BMI_EVENT_USER_PAUSE:
			a2dpsSuspendStreams();
			break;
		case BMI_EVENT_TRACK_START:
			/* ??? */
			break;
		default:
			;
	}

}
#endif

#ifdef BMI_STUB

static BmiEventCallback btEventCallback = NULL;
BOOL a2dpConnected = FALSE;

void BMI_RegisterEventCallback(BmiEventCallback eventCallback)
{
	Report(("BMI_RegisterEventCallback: eventCallback=0x%x ", eventCallback));
	btEventCallback = eventCallback;
}

BOOL BMI_SwitchAudioToBtHeadset(BOOL connected)
{
	Report(("BMI_SwitchAudioToBtHeadset: connected=%d ", connected));
	a2dpConnected = connected;
	return TRUE;
}

void BMI_NotifyBtEvent(BtAppEvent event)
{
	switch (event)
	{
		case BT_EVENT_USER_PLAY				: Report(("user pressed Play on BT headset"));			break;
		case BT_EVENT_USER_STOP			: Report(("user pressed Stop on BT headset"));			break;
		case BT_EVENT_USER_PAUSE			: Report(("user pressed Pause on BT headset"));		break;
		/*case BT_EVENT_USER_VOLUME_UP		: Report(("user pressed Volume-Up on BT headset"));	break;
		case BT_EVENT_USER_VOLUME_DOWN	: Report(("user pressed Volume-Down on BT headset"));	break;
		case BT_EVENT_USER_BACKWARD		: Report(("user pressed Backward on BT headset"));		break;
		case BT_EVENT_USER_FORWARD		: Report(("user pressed Forward on BT headset"));		break;
		case BT_EVENT_USER_FAST_FORWARD	: Report(("user pressed Fast-Forward on BT headset"));	break;
		*/
		default:
			Report(("Unknown BT headset event"));
	}
}

void bmiSendCallbackToBt(BmiAvEvent event)
{
	if (/*a2dpConnected &&*/ (NULL != btEventCallback))
	{
		Report(("BMI - sending to BT event %s", eventName[event]));
		btEventCallback(event);
	}
}

BOOL APP_A2DPS_BmiEventPlay(void)
{
	bmiSendCallbackToBt(BMI_EVENT_USER_PLAY);
	return TRUE;
}
BOOL APP_A2DPS_BmiEventStop(void)
{
	bmiSendCallbackToBt(BMI_EVENT_USER_STOP);
	return TRUE;
}
BOOL APP_A2DPS_BmiEventPause(void)
{
	bmiSendCallbackToBt(BMI_EVENT_USER_PAUSE);
	return TRUE;
}
BOOL APP_A2DPS_BmiEventResume(void)
{
	bmiSendCallbackToBt(BMI_EVENT_USER_RESUME);
	return TRUE;
}
BOOL APP_A2DPS_BmiEventTrackStart(void)
{
	bmiSendCallbackToBt(BMI_EVENT_TRACK_START);
	return TRUE;
}
BOOL APP_A2DPS_BmiEventTrackEnd(void)
{
	bmiSendCallbackToBt(BMI_EVENT_TRACK_END);
	return TRUE;
}

BOOL APP_A2DPS_l1AudioCfg(void)
{
	LINE_PARSER_STATUS  ParserStatus;
	U32 sampleRate;
	
	ParserStatus = LINE_PARSER_GetNextU32(&sampleRate, FALSE);

	if (ParserStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		Report(("A2DP can't parse sample rate !"));
		return FALSE;
	}

	Report(("A2DP sample rate %d", sampleRate));

	l1audio_CallConfigCallback(sampleRate);
	
	return TRUE;
}


#endif /* BMI_STUB */


/*---------------------------------------------------------------------------
 *            APP_A2DP_SendAvrcpOperation
 *---------------------------------------------------------------------------
 *
 * Synopsis:  send an AVRCP panel operation to A2DP application	
 *
 *     when AVRCP applicaiton receives events from the headset
 *     (e.g. Play, Stop, Pause, ...), it sends them to A2DP
 *     using this function. A2DP application will react upon them
 *     (e.g. to start/stop/suspend playing the current track)
 *
 * Return:    void
 *
 */
void APP_A2DP_SendAvrcpOperation(U32 channelId, AvrcpPanelOperation avrcpPanelOp)
{
	BtStatus status;
	int streamId;

	streamId = avrcpChannelId2A2dpStreamId[channelId];
	
	if(streamId < 0)
	{
		BTL_LOG_INFO(("No A2DP stream connected"));
		return;
	}

	// ??? have to add support for multi streams. assume only one stream for now !!!
	// ??? (e.g. we should tell BMI to pause only if all streams asked to pause)

	/* send the event to BMI */
	if ( pcmStreaming )
	{
		switch (avrcpPanelOp)
		{
			case AVRCP_POP_PLAY :
			case AVRCP_POP_STOP:
			case AVRCP_POP_PAUSE:
			case AVRCP_POP_VOLUME_UP:
			case AVRCP_POP_VOLUME_DOWN:
			case AVRCP_POP_FORWARD:
			case AVRCP_POP_BACKWARD:
			case AVRCP_POP_FAST_FORWARD:
				/* here we send the op to BMI as-is, because we rely on the
				    identical definition of AvrcpPanelOperation and BtAppEvent */
					BMI_NotifyBtEvent((BtAppEvent) avrcpPanelOp); 
				break;
			default:
				BTL_LOG_ERROR(("unsupported AVRCP panel operation 0x%x", avrcpPanelOp));
		}
	}
	else /*  it is SBC or MP3 file */
	{
		/* apply the event to A2DP: */
		switch (avrcpPanelOp)
		{
			case AVRCP_POP_PLAY :
				checkForStartStream((BtlA2dpStreamId)(streamId));
				break;
			case AVRCP_POP_STOP:
			case AVRCP_POP_PAUSE:
				status = BTL_A2DP_SuspendStream(appA2dpContext, (BtlA2dpStreamId)(streamId));
				break;
			case AVRCP_POP_FORWARD:
			case AVRCP_POP_BACKWARD:
			case AVRCP_POP_FAST_FORWARD:
				break;
			default: 	;
		}
	}
}

/*---------------------------------------------------------------------------
 *            APP_A2DP_UpdateAvrcpConnection
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Update A2DP on AVRCP connection / Disconnection	
 *
 *      when AVRCP applicaiton receives events AVRCP_EVENT_CONNECT and
 *		AVRCP_EVENT_DISCONNECT it will update A2DP app on status change and
 *		mapp the AVRCP channel ID with A2DP stream ID
 *
 * Return:    void
 *
 */
void APP_A2DP_UpdateAvrcpConnection(U32 channelId, BD_ADDR *bdAdd, BOOL isChannelConnected)
{
	U32 streamId;
	BD_ADDR currBdAdd;
	BtStatus status;

	if(isChannelConnected == FALSE)
	{
		/* channel was disconnected */
		avrcpChannelId2A2dpStreamId[channelId] = (-1);  
		return;
	}

	for(streamId = 0; streamId < A2DP_MAX_NUM_STREAMS_PER_CONTEXT; streamId++)
	{
		status = BTL_A2DP_GetConnectedDevice(appA2dpContext, streamId, &currBdAdd);

		if(status == BT_STATUS_SUCCESS)
		{
			/* connected device was found, compare BD address */
			if(OS_MemCmp(&currBdAdd, sizeof(BD_ADDR), bdAdd, sizeof(BD_ADDR)))
				break;  
		}   
	}

	if(streamId == A2DP_MAX_NUM_STREAMS_PER_CONTEXT)
	{
		BTL_LOG_INFO(("Warning: matching A2DP stream wasn't found"));
		return;
	}

	avrcpChannelId2A2dpStreamId[channelId] = streamId;
}



#else /* BTL_CONFIG_A2DP ==   BTL_CONFIG_ENABLED */

void A2DPA_Init(void)
{
	Report(("A2DPA_Init -BTL_CONFIG_A2DP is disabled "));
}
void A2DPA_Deinit(void)
{
	Report(("A2DPA_Deinit  - BTL_CONFIG_A2DP is disabled"));
}

void A2DPA_ProcessUserAction(U8 *msg)
{
	msg=msg;
	Report(("A2DP_APP is disabled via BTL_CONFIG."));

}



#endif /* BTL_CONFIG_A2DP ==   BTL_CONFIG_ENABLED */



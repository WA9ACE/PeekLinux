
#include <stdio.h>
#include "fms.h"
#include "fm_os.h"
#include "fms_api.h"
#include "fm_drivers.h"
#include "fms_transport.h"
#include "bthal_os.h"
#include "fm_tiinit.h"
#include "fm_config.h"
#include "btl_config.h"

#if 0

extern FmContext fm_context;
FmGeneral fm;

static void Fm_copyCurrentOpParam(TIFM_U8 opType);
static void startReadInt(void);
static void handleReadInt(void);
static void FmHandleFmInterrupt(void);
static void FmHandleGenInterrupts(void);
static void fm_init();
static void fm_deinit();
static void fm_param_init(void);
static void fm_reset_station_params(TIFM_U32 freq);
static void fm_reset_rds_params(void);
static void fm_memory_init(void);
static TIFM_BOOL checkUpperEvent(void);

static void FmOperationEnd(void);

static void FM_FinishPowerOn(TIFM_U8 status);
static TIFM_BOOL fmLoadInitScript(TIFM_U16 firm_version);
static void fmSendInitScript(void);
static void fmUnLoadInitScript(void);

static void send_fm_event_cmd_done(TIFM_U8 cmd, FmStatus status, TIFM_U32 value);
static void send_fm_event_radio_tuned(TIFM_U8 status, TIFM_U32 frequency);
static void send_fm_event_ps_changed(TIFM_U32 freq, TIFM_U8 *data);
static void send_fm_event_af_list_changed(TIFM_U16 pi, TIFM_U8 afListSize, TIFM_U32 *afList);
static void send_fm_event_af_jump(TIFM_U8 status, TIFM_U16 Pi, TIFM_U32 oldFreq, TIFM_U32 newFreq);
static void send_fm_event_radio_text(TIFM_BOOL changed, TIFM_U8 length, TIFM_U8 *radioText);
static void send_fm_event_most_mode_changed(TIFM_U8 mode);
static void send_fm_event_raw_rds(TIFM_U16 len, TIFM_U8 *data);
static FmStatus fmWriteCommand(TIFM_U8 fm_opcode, TIFM_U16 data);
static FmStatus fmPowerModeCommand(TIFM_U8 mode);
static FmStatus fmSendInitCmd(TIFM_U16 hci_opcode, TIFM_U8 len, TIFM_U8 *data_ptr);
static FmStatus fmReadStatus(TIFM_U8 fm_opcode);
static FmStatus fmReadRds(void);
static FmStatus fmReadInt(TIFM_U8 fm_opcode);

/* Handlers prototypes */
static void HandlePowerOnWakeupFm(void);
static void HandlePowerOnReadVersion(void);
static void HandlePowerOnStartInitScript(void);
static void Fm_Init_Process(void);
static void HandlePowerOnFinish(void);
static void HandlePowerOffStart(void);
static void HandlePowerOffFinish(void);
static void HandleMoStSetStart(void);
static void HandleMoStSetFinish(void);
static void HandleBandSetStart(void);
static void HandleBandSetFinish(void);
static void HandleMuteStart(void);
static void HandleMuteFinish(void);
static void HandleVolumeSetStart(void);
static void HandleVolumeSetFinish(void);
/*static void HandleRdsSetStart(void);	*/
static void HandleRdsOffFinishOnFlushFifo(void);
static void HandleRdsSetThreshold(void);
static void HandleRdsSetClearFlag(void);
static void HandleRdsSetEnableInt(void);
static void HandleRdsSetFinish(void);
static void HandleTuneSetFreq(void);
static void HandleTuneClearFlag(void);
static void HandleTuneEnableInterrupts(void);
static void HandleTuneStartTuning(void);
static void HandleTuneWaitStartTuneCmdComplete(void);
static void HandleTuneFinishedReadFreq(void);
static void HandleTuneFinishedEnableDefaultInts(void);
static void HandleTuneFinish(void);
static void HandleRssiGetStart(void);
static void HandleRssiGetFinish(void);
static void HandleSeekMain(void);
static void HandleSeekStart(void);
static void HandleSeekSetFreq(void);
static void HandleSeekSetDir(void);
static void HandleSeekClearFlag(void);
static void HandleSeekEnableInterrupts(void);
static void HandleSeekStartTuning(void);
static void HandleSeekWaitStartTuneCmdComplete(void);
static void HandleStopSeekStart(void);
static void HandleStopSeekWaitCmdCompleteOrInt(void);
static void HandleSeekStopSeekFinishedReadFreq(void);
static void HandleSeekStopSeekFinishedEnableDefaultInts(void);
static void HandleSeekStopSeekFinish(void);
static void HandleStopSeekBeforeSeekStarted(void);
static void HandleSetAFStart(void);
static void HandleSetAfFinish(void);
static void HandleSetStereoBlendStart(void);
static void HandleSetStereoBlendFinish(void);
static void HandleSetDeemphasisModeStart(void);
static void HandleSetDeemphasisModeFinish(void);
static void HandleSetRssiSearchLevelStart(void);
static void HandleSetRssiSearchLevelFinish(void);
static void HandleSetPauseLevelStart(void);
static void HandleSetPauseLevelFinish(void);
static void HandleSetPauseDurationStart(void);
static void HandleSetPauseDurationFinish(void);
static void HandleSetRdsRbdsStart(void);
static void HandleSetRdsRbdsFinish(void);
static void HandleMoStGetStart(void);
static void HandleMoStGetFinish(void);
static void HandleAudioEnableStart(void);
static void HandleAudioEnableFinish(void);
static void HandleGenIntMal(void);
static void HandleGenIntStereoChange(void);
static void HandleGenIntRds(void);
static void HandleGenIntLowRssi(void);
static void HandleGenIntEnableInt(void);
static void HandleGenIntFinish(void);
static void HandleReadRdsStart(void);
static void HandleReadRdsAnalyze(void);
static void GetRDSBlock(void);
static void handleRdsGroup(void);
static void	handleRdsGroup0(void);
static TIFM_BOOL checkNewAf(TIFM_U8 af);
static void	handleRdsGroup2(void);
static void FmHandleRdsRx(void);
static void FmHandleAfJump(void);
static void FmHandleStereoChange(void);
static void FmHandleMalfunction(void);
static TIFM_BOOL isAfJumpValid(void);
static void initAfJumpParams(void);
static void HandleAfJumpStartSetPi(void);
static void HandleAfJumpSetPiMask(void);
static void HandleAfJumpSetAfFreq(void);
static void HandleAfJumpEnableInt(void);
static void HandleAfJumpStartAfJump(void);
static void HandleAfJumpWaitCmdComplete(void);
static void HandleAfJumpReadFreq(void);
static void HandleAfJumpFinished(void);
static void HandleLowRssiStartFinish(void);
static void HandleStereoChangeStart(void);
static void HandleStereoChangeFinish(void);
static void HandleHwMalStartFinish(void);
static void HandleTimeoutStart(void);
static void HandleTimeoutFinish(void);

static FmEvent fmEvent;

/* Handlers for FM_CMD_POWER_ON */
FmOpCurHandler powerOnHandler[NUM_HANDLERS_POWER_ON] = {HandlePowerOnWakeupFm,
														HandlePowerOnReadVersion,
														HandlePowerOnStartInitScript,
														Fm_Init_Process,
														HandlePowerOnFinish};

/* Handlers for FM_CMD_POWER_OFF */
FmOpCurHandler powerOffHandler[NUM_HANDLERS_POWER_OFF] = {HandlePowerOffStart,
															HandlePowerOffFinish};

/* Handlers for FM_CMD_MOST_SET */
FmOpCurHandler mostSetHandler[NUM_HANDLERS_MOST_SET] = {HandleMoStSetStart,
														  HandleMoStSetFinish};

/* Handlers for FM_CMD_BAND_SET */
FmOpCurHandler bandSetHandler[NUM_HANDLERS_BAND_SET] = {HandleBandSetStart,
														HandleBandSetFinish};

/* Handlers for FM_CMD_MUTE */
FmOpCurHandler muteHandler[NUM_HANDLERS_MUTE] = {HandleMuteStart,
												 HandleMuteFinish};

/* Handlers for FM_CMD_VOLUME_SET */
FmOpCurHandler volumeSetHandler[NUM_HANDLERS_VOLUME_SET] = {HandleVolumeSetStart,
															HandleVolumeSetFinish};

/* Handlers for FM_CMD_RDS_SET */
FmOpCurHandler rdsSetHandler[NUM_HANDLERS_RDS_SET] = {
													  HandleRdsOffFinishOnFlushFifo,
													  HandleRdsSetThreshold,
													  HandleRdsSetClearFlag,
													  HandleRdsSetEnableInt,
													  HandleRdsSetFinish};

/* Handlers for FM_CMD_RDS_SET */
/*FmOpCurHandler rdsSetHandler[NUM_HANDLERS_RDS_SET] = {HandleRdsSetStart,
													  HandleRdsOffFinishOnFlushFifo,
													  HandleRdsSetThreshold,
													  HandleRdsSetClearFlag,
													  HandleRdsSetEnableInt,
													  HandleRdsSetFinish};
*/
/* Handlers for FM_CMD_TUNE */
FmOpCurHandler tuneHandler[NUM_HANDLERS_TUNE] = {HandleTuneSetFreq,
												   HandleTuneClearFlag,
												   HandleTuneEnableInterrupts,
												   HandleTuneStartTuning,
												   HandleTuneWaitStartTuneCmdComplete,
												   HandleTuneFinishedReadFreq,
												   HandleTuneFinishedEnableDefaultInts,
												   HandleTuneFinish};

/* Handlers for FM_CMD_RSSI_GET */
FmOpCurHandler getRssiHandler[NUM_HANDLERS_RSSI_GET] = {HandleRssiGetStart,
															HandleRssiGetFinish};

/* Handlers for FM_CMD_SEEK (and STOP SEEK) */
FmOpCurHandler seekMainHandler[NUM_HANDLERS_MAIN_SEEK] = {HandleSeekMain};

/****************************************************************************/
/* Internal seek / stop seek handlers. Called by seekMainHandler			*/
FmOpCurHandler seekHandler[10] = {HandleSeekStart,
									HandleSeekSetFreq,
									HandleSeekSetDir,
									HandleSeekClearFlag,
									HandleSeekEnableInterrupts,
									HandleSeekStartTuning,
									HandleSeekWaitStartTuneCmdComplete,
									HandleSeekStopSeekFinishedReadFreq,
									HandleSeekStopSeekFinishedEnableDefaultInts,
									HandleSeekStopSeekFinish};


FmOpCurHandler stopSeekHandler[7] = {HandleStopSeekStart,
										HandleStopSeekWaitCmdCompleteOrInt,													
										HandleSeekStopSeekFinishedReadFreq,
										HandleSeekStopSeekFinishedEnableDefaultInts,
										HandleSeekStopSeekFinish,
										HandleSeekStopSeekFinishedEnableDefaultInts, /*It appears twice on purpose */
										HandleStopSeekBeforeSeekStarted};

/****************************************************************************/

/* Handlers for FM_CMD_SET_AF */
FmOpCurHandler setAfHandler[NUM_HANDLERS_SET_AF] = {HandleSetAFStart,
													HandleSetAfFinish};

/* Handlers for FM_CMD_SET_STEREO_BLEND */
FmOpCurHandler setStereoBlendHandler[NUM_HANDLERS_SET_STEREO_BLEND] = {HandleSetStereoBlendStart,
																		HandleSetStereoBlendFinish};

/* Handlers for FM_CMD_SET_DEEMPHASIS_MODE */
FmOpCurHandler setDeemphasisModeHandler[NUM_HANDLERS_SET_DEEMPHASIS_MODE] = {HandleSetDeemphasisModeStart,
																			 HandleSetDeemphasisModeFinish};

/* Handlers for FM_CMD_SET_RSSI_SEARCH_LEVEL */
FmOpCurHandler setRssiSearchLevelHandler[NUM_HANDLERS_SET_RSSI_SEARCH_LEVEL] = {HandleSetRssiSearchLevelStart,
																				HandleSetRssiSearchLevelFinish};

/* Handlers for FM_CMD_SET_PAUSE_LEVEL */
FmOpCurHandler setPauseLevelHandler[NUM_HANDLERS_SET_PAUSE_LEVEL] = {HandleSetPauseLevelStart,
																	 HandleSetPauseLevelFinish};

/* Handlers for FM_CMD_SET_PAUSE_DURATION */
FmOpCurHandler setPauseDurationHandler[NUM_HANDLERS_SET_PAUSE_DURATION] = {HandleSetPauseDurationStart,
																			HandleSetPauseDurationFinish};

/* Handlers for FM_CMD_SET_RDS_RBDS_MODE */
FmOpCurHandler setRdsRbdsModeHandler[NUM_HANDLERS_SET_RDS_RBDS_MODE] = {HandleSetRdsRbdsStart,
																		HandleSetRdsRbdsFinish};

/* Handlers for FM_CMD_MOST_GET */
FmOpCurHandler mostGetHandler[NUM_HANDLERS_MOST_GET] = {HandleMoStGetStart,
														HandleMoStGetFinish};

/* Handlers for FM_CMD_AUDIO_ENABLE */
FmOpCurHandler audioEnableHandler[NUM_HANDLERS_AUDIO_ENABLE] = {HandleAudioEnableStart,
																HandleAudioEnableFinish};

/* Handlers for FM_HANDLE_GEN_INT */
FmOpCurHandler genIntHandler[NUM_HANDLERS_GEN_INT] = {HandleGenIntMal,
														HandleGenIntStereoChange,
														HandleGenIntRds,
														HandleGenIntLowRssi,
														HandleGenIntEnableInt,
														HandleGenIntFinish};

/* Handlers for FM_READ_RDS */
FmOpCurHandler readRdsHandler[NUM_HANDLERS_READ_RDS] = {HandleReadRdsStart,
														HandleReadRdsAnalyze};

/* Handlers for FM_HANDLE_AF_JUMP */
FmOpCurHandler afJumpHandler[NUM_HANDLERS_AF_JUMP] = {HandleAfJumpStartSetPi,
														HandleAfJumpSetPiMask,
														HandleAfJumpSetAfFreq,
														HandleAfJumpEnableInt,
														HandleAfJumpStartAfJump,
														HandleAfJumpWaitCmdComplete,														
														HandleAfJumpReadFreq,
														HandleAfJumpFinished};

/* Handlers for FM_HANDLE_STEREO_CHANGE */
FmOpCurHandler stereoChangedHandler[NUM_HANDLERS_STEREO_CHANGED] = {HandleStereoChangeStart,
																	HandleStereoChangeFinish};

/* Handlers for FM_HANDLE_HW_MAL */
FmOpCurHandler hwMalHandler[NUM_HANDLERS_HW_MAL] = {HandleHwMalStartFinish};

/* Handlers for FM_HANDLE_TIMEOUT */
FmOpCurHandler timeoutHandler[NUM_HANDLERS_TIMEOUT] = {HandleTimeoutStart,
													   HandleTimeoutFinish};
/* Operation Handlers array */
FmOpHandlerArray fmOpAllHandlersArray[NUM_OP_HANDLERS] = {powerOnHandler,					/* FM_CMD_POWER_ON				*/
														  powerOffHandler,					/* FM_CMD_POWER_OFF				*/
														  mostSetHandler,					/* FM_CMD_MOST_SET				*/
														  bandSetHandler,					/* FM_CMD_BAND_SET				*/
														  muteHandler,						/* FM_CMD_MUTE					*/
														  volumeSetHandler,					/* FM_CMD_VOLUME_SET			*/
														  rdsSetHandler,					/* FM_CMD_RDS_SET				*/
														  tuneHandler,						/* FM_CMD_TUNE					*/
														  getRssiHandler,					/* FM_CMD_RSSI_GET				*/
														  seekMainHandler,					/* FM_CMD_SEEK					*/
														  setAfHandler,						/* FM_CMD_SET_AF				*/
														  setStereoBlendHandler,			/* FM_CMD_SET_STEREO_BLEND		*/
														  setDeemphasisModeHandler,			/* FM_CMD_SET_DEEMPHASIS_MODE	*/
														  setRssiSearchLevelHandler,		/* FM_CMD_SET_RSSI_SEARCH_LEVEL */
														  setPauseLevelHandler,				/* FM_CMD_SET_PAUSE_LEVEL		*/
														  setPauseDurationHandler,			/* FM_CMD_SET_PAUSE_DURATION	*/
														  setRdsRbdsModeHandler,			/* FM_CMD_SET_RDS_RBDS_MODE		*/
														  mostGetHandler,					/* FM_CMD_MOST_GET				*/
														  audioEnableHandler,				/* FM_CMD_AUDIO_ENABLE			*/
														  genIntHandler,					/* FM_HANDLE_GEN_INT			*/
														  readRdsHandler,					/* FM_READ_RDS					*/
														  afJumpHandler,					/* FM_HANDLE_AF_JUMP			*/
														  stereoChangedHandler,				/* FM_HANDLE_STEREO_CHANGE		*/
														  hwMalHandler, 					/* FM_HANDLE_HW_MAL				*/
														  timeoutHandler};					/* FM_HANDLE_TIMEOUT				*/


void FMS_Init(void)
{
	fm_memory_init();

}

static void fm_init()
{
	fm_param_init();

	fm_radio_init();
}

static void fm_deinit()
{

}

void FMS_Deinit(void)
{
}


static void fm_param_init(void)
{
	fm_reset_station_params(NO_STATION_FREQ);
}

static void fm_reset_station_params(TIFM_U32 freq)
{
	BTHAL_OS_CancelTimer(FMC(fmTimerHandle));
	FMC(curStationParams).freq = freq;
	fm_reset_rds_params();

	/* If AF feature is on, enable low level RSSI interrupt in global parameter */
	if(TIFM_TRUE == FMC(AFOn))
	{
		FMC(fmDriver).gen_int_mask |= MASK_LEV;
	}
}

static void fm_reset_rds_params(void)
{
	FMC(rdsParams).last_block_index = RDS_BLOCK_INDEX_UNKNOWN;
	FMC(rdsParams).nextPsIndex = RDS_NEXT_PS_INDEX_RESET;
	FMC(curStationParams).piCode = NO_PI_CODE;
	FMC(curStationParams).afListSize = 0;
	BTHAL_UTILS_MemCopy(FMC(curStationParams).psName, (TIFM_U8*)("\0"), RDS_PS_NAME_SIZE);
	FMC(rdsParams).nextRtIndex = RDS_NEXT_RT_INDEX_RESET;
	FMC(rdsParams).prevABFlag = RDS_PREV_RT_AB_FLAG_RESET;
}

static void fm_memory_init(void)
{
    TIFM_U8* ptr; 

    /* Fill memory with 0. */
    ptr = (TIFM_U8*)&fm;
	
    BTHAL_UTILS_MemSet(ptr, 0, (TIFM_U32)sizeof(FmGeneral));

    /* Initialize mute variables */
    FMC(muteState) = MUTE_UNMUTE_MODE;
    FMC(rfMuteOn) = TIFM_TRUE;

	TIFM_InitializeListHead(&(FMC(opList)));	
}


void Fm_Process(void)
{
	/*********************************************************************
		KEEP THIS ORDER. WHEN HANDLING FIRST THE INTERRUPTS IT CAN 
		MAKE A DEADLOCK FOR THE COMMANDS. WHEN RECEIVING INTERRUPT
		ALL THE TIME (LIKE LOW_RSSI WHEN AF_ON AND WE ARE ON BAD CHANNEL) 
		AND TRYING TO SEND COMMAND IT MIGHT TAKE VERY LONG UNTIL IT WILL 
		BE DONE.
	*********************************************************************/

	/* If we are not in the middle of first handling of the interrupt
	   (Read mask/flag and handle if it's operation-specific */
	if(FMC(readIntState) != INT_STATE_READ_INT)
	{
		Fm_Events_Process();
		Fm_Commands_Process();
	}
	/* Make sure we will handle the events after exiting the read interrupt state */
	else
	{
		NotifyFms();
	}

	Fm_Interrupts_Process();
}
/* Handle received interrupts: Read the flag and mask in order to figure
   out if this is a general interrupt or a specific-operation interrupt.
   A general interrupt will be handled after the operation is finished */
void Fm_Interrupts_Process(void)
{
	/* An interrupt was received */
	if(FMC(interruptInd))
	{
		/* If we are waiting for cmd complete - wait to receive it before handling
		   the interrupt */
		if(FMC(waitCmdCmplt))
		{
			FMC(readIntState) = INT_STATE_WAIT_FOR_CMD_CMPLT;
		}
		/* Not waiting for cmd complete - can start handling the interrupt */
		else
		{
			startReadInt();
		}
	}

	/* This event is set when we receive a cmd complete for the read flag
	   and read mask commands - we can proceed in handling the interrupt */
	if(FMC(intReadEvt))
	{
		FMC(intReadEvt) = TIFM_FALSE;
		
		handleReadInt();
	}

}

void Fm_Timer_Process(void)
{
	TIFM_Report(("AF suspension timeout finished"));

	/* Check if the command is not already waiting in the queue */
	if(!TIFM_IsNodeAvailable(&(FMC(timeoutOp).op.node)))
	{
		TIFM_Report(("Timeout operation is already pending"));
	}
	else
	{
		FMC(timeoutOp).op.opType = FM_HANDLE_TIMEOUT;

		/* Lock the stack when inserting the operation into the queue */
		OS_LockFmStack();
		FmAddOperation(&FMC(timeoutOp));
		OS_UnlockFmStack();

		/* Notify FM stack about the operation */
		NotifyFms();
	}
}

/*---------------------------------------------------------------------------
 *            Fm_Commands_Process()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  If possible start the next operation on the queue. 
 *
 * Return:    void
 */
void Fm_Commands_Process(void)
{
	/* Start the next command only if there is no current command handled */
	if (FMC(curOp) == 0) 
	{
		/* There is no active operation so start the next one on the queue */
		if (!TIFM_IsListEmpty(&FMC(opList))) 
		{
			/* Lock the stack when changing thecurOp */
			OS_LockFmStack();
			FMC(curOp) = (FmOperation*)TIFM_RemoveHeadList(&FMC(opList));
			/* We copy the param in order to allow the application to send another 
			   command of the same type without overriding the current performed operation */
			Fm_copyCurrentOpParam(FMC(curOp)->opType);
			TIFM_InitializeListNode(&(FMC(curOp)->node));
			OS_UnlockFmStack();
		
			FMC(fmOpCurStage) = 0;
			FMC(fmOpHandler) = (fmOpAllHandlersArray[FMC(curOp)->opType])[FMC(fmOpCurStage)];

			FMC(callReason) = CALL_REASON_START;
			FMC(fmOpHandler)();
		}
    }
}

static void Fm_copyCurrentOpParam(TIFM_U8 opType)
{
	switch(opType)
	{
		case FM_CMD_MOST_SET:
			FMC(fmCurOpParam).param.mode = FMC(moStOp).mode;
			break;
		case FM_CMD_BAND_SET:
			FMC(fmCurOpParam).param.mode = FMC(bandSetOp).mode;
			break;
		case FM_CMD_MUTE:
			FMC(fmCurOpParam).param.mode = FMC(muteOp).mode;
			break;
		case FM_CMD_VOLUME_SET:
			FMC(fmCurOpParam).param.gain = FMC(volumeSetOp).gain;
			break;
		case FM_CMD_RDS_SET:
			FMC(fmCurOpParam).param.mode = FMC(rdsSetOp).mode;
			break;
		case FM_CMD_TUNE:
			FMC(fmCurOpParam).param.freq = FMC(tuneOp).freq;
			break;
		case FM_CMD_SET_AF:
			FMC(fmCurOpParam).param.mode = FMC(setAFOp).mode;
			break;
		case FM_CMD_SET_STEREO_BLEND:
			FMC(fmCurOpParam).param.mode = FMC(setStereoBlendOp).mode;
			break;
		case FM_CMD_SET_DEEMPHASIS_MODE:
			FMC(fmCurOpParam).param.mode = FMC(setDeemphasisModeOp).mode;
			break;
		case FM_CMD_SET_RSSI_SEARCH_LEVEL:
			FMC(fmCurOpParam).param.rssi_level = FMC(setRssiSearchLevelOp).rssi_level;
			break;
		case FM_CMD_SET_PAUSE_LEVEL:
			FMC(fmCurOpParam).param.pause_level = FMC(setPauseLevelOp).pause_level;
			break;
		case FM_CMD_SET_PAUSE_DURATION:
			FMC(fmCurOpParam).param.pause_duration = FMC(setPauseDurationOp).pause_duration;
			break;
		case FM_CMD_SET_RDS_RBDS_MODE:
			FMC(fmCurOpParam).param.mode = FMC(setRdsRbdsModeOp).mode;
			break;
	}
}


#define INT_READ_MASK		0
#define INT_READ_FLAG		1
#define INT_HANDLE_INT		2

static void startReadInt(void)
{
	/* Start handling the interrupt - Clear the flag */
	OS_LockFmStack();
	FMC(interruptInd) = TIFM_FALSE;
	OS_UnlockFmStack();

    /* Do nothing, if we are going power down or already OFF */
    if ((FMC(initState) == INIT_STATE_FM_OFF) ||
        (FMC(initState) == INIT_STATE_FM_OFF_PENDING))
    {
	    FMC(readIntState) = INT_STATE_NONE;
		NotifyFms();
    }
    else
    {
	    FMC(readIntState) = INT_STATE_READ_INT;
	    handleReadInt();
   }
}

static void handleReadInt(void)
{
	static TIFM_U8 int_state = INT_READ_MASK;
	
	if(int_state == INT_READ_MASK)
	{
		int_state = INT_READ_FLAG;
		TIFM_Assert(fmReadInt(INT_MASK_SET_GET) == FM_STATUS_PENDING);
	}
	else if(int_state == INT_READ_FLAG)
	{
		int_state = INT_HANDLE_INT;
		TIFM_Assert(fmReadInt(FLAG_GET) == FM_STATUS_PENDING);
	}
	else
	{
		int_state = INT_READ_MASK;
		FMC(readIntState) = INT_STATE_NONE;
		FmHandleFmInterrupt();
	}
}

/* This function is called after reading the interrupt flag. 
	it can be either a general interrupt or a specific interrupt that
	the operation enabled */
static void FmHandleFmInterrupt(void)
{
	TIFM_U16 intSetBits = (TIFM_U16)(FMC(fmFlag) & FMC(fmMask));

	/* If one of the bits of the opHandler interrupt occurred - call it */
	if(intSetBits & (FMC(fmDriver).opHandler_int_mask))
	{
		FMC(opHandlerIntSetBits) = intSetBits;
		FMC(callReason) = CALL_REASON_INTERRUPT;
		FMC(fmOpHandler)();
	}
	/* If it's one of the general interrupts - add it to the queue and 
	   handle it after the current operation is done */
	/* Note: A scenario can happen that an interrupt occur and the flag and mask are read
	   right after we clear the flag in the middle of an operation (like seek). in 
	   this situation we will read flag 0 - so we will handle it like a general int
	   just in order to enable the interrupts again. */
	else
	{
		FMC(genIntSetBits) = intSetBits;
		FmHandleGenInterrupts();		
/*		if(intSetBits == 0)
		{
			TIFM_Report("ERROR INTERRUPT SITUATION");
		}*/
	}

}
static void FmHandleGenInterrupts(void)
{
    /* If we already have a general interrupt in the queue - no need to add another one.
	   Note: This situation can happen when there is a general interrupt in the queue, then we
	   start an operation with interrupt (like seek) and then after receiving the interrupt
	   and enabling again the general interrupt - we will receive an interrupt again and try
	   to add it to the queue */
	if(!TIFM_IsNodeAvailable(&(FMC(genIntOp).op.node)))
	{		
		return;
	}

	FMC(genIntOp).op.opType = FM_HANDLE_GEN_INT;


	OS_LockFmStack();
	/* Add the general interrupts handling to the beginning of the list to generate it before other operations */
    FmAddOperationFront(&FMC(genIntOp));
	NotifyFms();
	OS_UnlockFmStack();
}

/*---------------------------------------------------------------------------
 *            Fm_Events_Process()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  If possible start the next operation on the queue. 
 *
 * Return:    void
 */
void Fm_Events_Process(void)
{
	/* If a Command Complete event was received */
    if (FMC(curEvent).cmdCmpltEvent) 
	{
		/* An interrupt was received before but we had to wait for a cmd_complete event
		   to arrive before we can start handling it. Now event arrived and we can start 
		   handling the interrupt */
		if(FMC(readIntState) == INT_STATE_WAIT_FOR_CMD_CMPLT)
		{
			startReadInt();
		}
		/* No interrupt was waiting - now we can handle the cmd complete event.
		   If the upper event waited for cmd complete -
		   call the operation handler only with upper event reason,
		   Otherwise - call it with cmd complete reason. */
		else 
		{

			FMC(waitCmdCmplt) = TIFM_FALSE;
			FMC(curEvent).cmdCmpltEvent = TIFM_FALSE;

			/* If the upper event waited for the cmd complete - call the 
			   Operation handler with upper event and ignore the cmd complete */
			if(FMC(upperEventWait))
			{
				FMC(upperEventWait) = TIFM_FALSE;
				
				/* Only if the current event fits the current running operation.
				   It will only be sent in that case but this check is to prevent 
				   race condition */
				if(checkUpperEvent())
				{
					FMC(callReason) = CALL_REASON_UPPER_EVT;
				}
				else
				{
					FMC(callReason) = CALL_REASON_CMD_CMPLT_EVT;
				}
				/* Resetting the upperEvent must appear after the checkUpperEvent */
				FMC(upperEvent) = UPPER_EVENT_NONE;
				FMC(fmOpHandler)();
			}
			else
			{
				/* Call the handler with callReason cmd_complete event */
				FMC(callReason) = CALL_REASON_CMD_CMPLT_EVT;
				FMC(fmOpHandler)();
			}
		}
	}

	/* If an event from the upper API received - handle it */
	if(FMC(upperEvent) != UPPER_EVENT_NONE)
	{
		if(FMC(waitCmdCmplt))
		{
			FMC(upperEventWait) = TIFM_TRUE;
		}
		else
		{			
			/* Only if the current event fits the current running operation.
			   It will only be sent in that case but this check is to prevent 
			   race condition */
			if(checkUpperEvent())
			{
				FMC(callReason) = CALL_REASON_UPPER_EVT;
				FMC(fmOpHandler)();
			}
			/* Resetting the upperEvent must appear after the checkUpperEvent */
			FMC(upperEvent) = UPPER_EVENT_NONE;

		}
	}
}

static TIFM_BOOL checkUpperEvent(void)
{
	switch(FMC(upperEvent))
	{
		case UPPER_EVENT_STOP_SEEK:
			if((FMC(curOp) != 0) && (FMC(curOp)->opType == FM_CMD_SEEK))
				return TIFM_TRUE;
			else
				return TIFM_FALSE;
		default:
			return TIFM_FALSE;
	}
}
static void Fm_Init_Process(void)
{
	if(FMC(initState) == INIT_STATE_CMD_COMPLETE_DONE_ERROR)
	{
		TIFM_Assert(TIFM_FALSE);
		FM_FinishPowerOn(FM_STATUS_FAILED);
	}
	else
	{
		fmSendInitScript();
	}
}

/*---------------------------------------------------------------------------
 *            FmOperationEnd()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  End the current operation and start a new one if one
 *            exists.
 *
 * Return:    void
 */
static void FmOperationEnd(void)
{
	
	OS_LockFmStack();
	FMC(fmOpHandler) = 0;
	FMC(curOp) = 0;
	/* wakeup the stack to check whether more commands are available */
	NotifyFms();
	OS_UnlockFmStack();
}

static void prepareNextStage(TIFM_U8 wait, TIFM_U8 nextStage)
{
	/* Update the operation handler */
	if(nextStage == INCREMENT_STAGE)
	{
		FMC(fmOpCurStage)++;
	}
	else
	{
		FMC(fmOpCurStage) = nextStage; 
	}
	FMC(fmOpHandler) = (fmOpAllHandlersArray[FMC(curOp)->opType])[FMC(fmOpCurStage)];

	if(wait == WAIT_FOR_CMD_COMPLETE)
	{
		FMC(waitCmdCmplt) = TIFM_TRUE;
	}
}

static void HandlePowerOnWakeupFm(void)
{
	TIFM_U8 status = FM_STATUS_FAILED;

	if(FMC(initState) == INIT_STATE_FM_OFF)
	{
		fm_init();

		/* Update to next handler */
		prepareNextStage(WAIT_FOR_CMD_COMPLETE, INCREMENT_STAGE);

		/* Wakeup the FM module.
		   In UART mode - this command mux the I2C to work internally and wakeup the FM module.
		   In I2C mode - Power_Set command needs to be sent in order to wakeup the FM. 
		   No Ack should be received. */
		TIFM_Assert(fmPowerModeCommand(FM_POWER_MODE_ENABLE) == FM_STATUS_PENDING);

		/* Must wait 20msec before starting to send commands to the FM. As it may
         * take time to stack to send this command to the chip, we will start delay
         * after receiving Command Complete Event */
	}
	else
	{
		if(FMC(initState) == INIT_STATE_FM_ON)
		{
			status = FM_STATUS_FM_ALREADY_ON;
		}

		FM_FinishPowerOn(status);
	}
}

static void HandlePowerOnReadVersion(void)
{	
	/* Must wait 20msec before starting to send commands to the FM after command
     * FM_POWER_MODE_ENABLE was sent. */
	BTHAL_OS_Sleep(FM_CONFIG_WAKEUP_TIMEOUT_MS);

	/* Update to next handler */
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, INCREMENT_STAGE);

	/* Send read version command */
	TIFM_Assert(fmReadStatus(FIRM_VER_GET) == FM_STATUS_PENDING);
}

static void HandlePowerOnStartInitScript(void)
{
	TIFM_U8 status;
	TIFM_U16 firm_version;

	firm_version = FMC(curEvent).read_param;

	/* If didn't manage to load the fm init script */
	if(fmLoadInitScript(firm_version) == TIFM_FALSE)
	{
		status = FM_STATUS_FAILED;
		FM_FinishPowerOn(status);
		TIFM_Assert(TIFM_FALSE);
	}
	else
	{
		prepareNextStage(WAIT_FOR_CMD_COMPLETE, INCREMENT_STAGE);		
		fmSendInitScript();
	}
}

static void HandlePowerOnFinish(void)
{
	FM_FinishPowerOn(FM_STATUS_SUCCESS);
}

static void FM_FinishPowerOn(TIFM_U8 status)
{
	send_fm_event_cmd_done(FM_CMD_POWER_ON, status, FM_NO_VALUE);

	FmOperationEnd();
}

static void HandlePowerOffStart(void)
{
	FMC(initState) = INIT_STATE_FM_OFF_PENDING;
	fm_deinit();
	
	/* Update to next handler */
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, INCREMENT_STAGE);

	/* Send power off command */
	TIFM_Assert(fmPowerModeCommand(FM_POWER_MODE_DISABLE) == FM_STATUS_PENDING);			
	
}
static void HandlePowerOffFinish(void)
{
	FMC(initState) = INIT_STATE_FM_OFF;
	fm_param_init();

	fm_transport_off();

	/* Send event to the applicatoin */
	send_fm_event_cmd_done(FMC(curOp)->opType, FM_STATUS_SUCCESS, FM_NO_VALUE);
	FmOperationEnd();
}

static void HandleMoStSetStart(void)
{
	/* Update to next handler */
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, INCREMENT_STAGE);

	/* Send Set_MoSt command */
	TIFM_Assert(fmWriteCommand(MOST_MODE_SET_GET, FMC(fmCurOpParam).param.mode) == FM_STATUS_PENDING);
}
static void HandleMoStSetFinish(void)
{
	/* Send event to the applicatoin */
	send_fm_event_cmd_done(FMC(curOp)->opType, FM_STATUS_SUCCESS, FM_NO_VALUE);
	FmOperationEnd();
}

static void HandleBandSetStart(void)
{
	/* Update to next handler */
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, INCREMENT_STAGE);

	/* Send Set_MoSt command */
	TIFM_Assert(fmWriteCommand(BAND_SET_GET, FMC(fmCurOpParam).param.mode) == FM_STATUS_PENDING);			
}
static void HandleBandSetFinish(void)
{
	FMC(band) = FMC(fmCurOpParam).param.mode;
	/* Send event to the applicatoin */
	send_fm_event_cmd_done(FMC(curOp)->opType, FM_STATUS_SUCCESS, FM_NO_VALUE);
	FmOperationEnd();
}

static void HandleMuteStart(void)
{
	TIFM_U8 muteMode = MUTE_UNMUTE_MODE;

	/* Update to next handler */
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, INCREMENT_STAGE);

	switch(FMC(fmCurOpParam).param.mode)
	{
		case FM_MUTE_VOICE:
			muteMode = MUTE_AC_MUTE_MODE;
			break;
		case FM_REDUCE_VOICE:
			muteMode = MUTE_SOFT_MUTE_FORCE_MODE;
			break;
		case FM_UNMUTE_VOICE:
			muteMode = MUTE_UNMUTE_MODE;
			break;
		default:
			TIFM_Report(("HandleMuteStart muteMode not define"));
			TIFM_Assert(0);			
	}

	/* Update the mute mode with RF dependent bit */
    if (TIFM_TRUE == FMC(rfMuteOn))
    {
	    muteMode |= MUTE_RF_DEP_MODE;
    }

	/* Send Set_MoSt command */
	TIFM_Assert(fmWriteCommand(MUTE_STATUS_SET_GET, muteMode) == FM_STATUS_PENDING);			
}
static void HandleMuteFinish(void)
{
	/* Send event to the applicatoin */
	send_fm_event_cmd_done(FMC(curOp)->opType, FM_STATUS_SUCCESS, FM_NO_VALUE);
	FmOperationEnd();
}

static void HandleVolumeSetStart(void)
{
	/* Update to next handler */
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, INCREMENT_STAGE);

	/* Send Set_MoSt command */
	TIFM_Assert(fmWriteCommand(VOLUME_SET_GET, FMC(fmCurOpParam).param.gain) == FM_STATUS_PENDING);			

}
static void HandleVolumeSetFinish(void)
{
	/* Send event to the applicatoin */
	send_fm_event_cmd_done(FMC(curOp)->opType, FM_STATUS_SUCCESS, FM_NO_VALUE);
	FmOperationEnd();
}

#if 0
static void HandleRdsSetStart(void)
{
	TIFM_U8 mode;

	/* Update to next handler */
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, INCREMENT_STAGE);

	if(FMC(fmCurOpParam).param.mode == FM_RDS_ON)
	{
		mode = POWER_SET_FM_AND_RDS_ON;
	}
	else
	{
		mode = POWER_SET_FM_ON_RDS_OFF;
	}
	/* Send Set_MoSt command */
	TIFM_Assert(fmWriteCommand(POWER_SET_GET, mode) == FM_STATUS_PENDING);			

}
#endif

static void HandleRdsOffFinishOnFlushFifo(void)
{
	/* If RDS is turned off - finished. Send event and end operation */
	if(FMC(fmCurOpParam).param.mode == FM_RDS_OFF)
	{
		fm_reset_rds_params();
		FMC(fmDriver).gen_int_mask &= ~(MASK_RDS);
		send_fm_event_cmd_done(FMC(curOp)->opType, FM_STATUS_SUCCESS, FMC(fmCurOpParam).param.mode);
		FmOperationEnd();
	}
	/* RDS on requires more commands to be sent */
	else
	{
		/* Update to next handler */
		prepareNextStage(WAIT_FOR_CMD_COMPLETE, INCREMENT_STAGE);

		/* Send Set_MoSt command */
		TIFM_Assert(fmWriteCommand(RDS_CNTRL_SET, RDS_FLUSH_FIFO) == FM_STATUS_PENDING);			
	}

}

static void HandleRdsSetThreshold(void)
{
	/* Update to next handler */
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, INCREMENT_STAGE);

	/* Send Set_MoSt command */
	TIFM_Assert(fmWriteCommand(RDS_MEM_SET_GET, RDS_THRESHOLD) == FM_STATUS_PENDING);			

}

static void HandleRdsSetClearFlag(void)
{
	/* Update to next handler */
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, INCREMENT_STAGE);

	/* Read the flag to clear status */
	TIFM_Assert(fmReadStatus(FLAG_GET) == FM_STATUS_PENDING);
}

static void HandleRdsSetEnableInt(void)
{
	/* Update to next handler */
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, INCREMENT_STAGE);

	/* Update global parameter int_mask to enable also RDS interrupt */
	FMC(fmDriver).gen_int_mask |= MASK_RDS;
	/* Enable the interrupts */
	TIFM_Assert(fmWriteCommand(INT_MASK_SET_GET, FMC(fmDriver).gen_int_mask) == FM_STATUS_PENDING);

}

static void HandleRdsSetFinish(void)
{
	/* Send event to the applicatoin */
	send_fm_event_cmd_done(FMC(curOp)->opType, FM_STATUS_SUCCESS, FMC(fmCurOpParam).param.mode);
	FmOperationEnd();
}

/***********************
 * HANDLE TUNE FUNCTIONS
 ***********************/

static void HandleTuneSetFreq(void)
{	
	TIFM_U32 freq = FMC(fmCurOpParam).param.freq;
	TIFM_U16 index;
	TIFM_U8 status = FM_STATUS_SUCCESS;

	if(FMC(band) == FM_EUROPE_US_BAND)
	{
		if((freq < BASE_FREQ_US_EUROPE) || (freq > LAST_FREQ_US_EUROPE))
			status = FM_STATUS_INVALID_PARM;
	}
	/* Japan band */
	else
	{
		if((freq < BASE_FREQ_JAPAN) || (freq > LAST_FREQ_JAPAN))
			status = FM_STATUS_INVALID_PARM;
	}

	if(status == FM_STATUS_INVALID_PARM)
	{
		send_fm_event_radio_tuned(status, FMC(curStationParams).freq);	
		FmOperationEnd();
	}
	else
	{
		index = FmFreqToIndex(freq);
		/* Update to next handler */
		prepareNextStage(WAIT_FOR_CMD_COMPLETE, INCREMENT_STAGE);

		/* Send Set_Frequency command */	
		TIFM_Assert(fmWriteCommand(FREQ_SET_GET, index) == FM_STATUS_PENDING);
	}


}

static void HandleTuneClearFlag(void)
{	
	/* Update to next handler */
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, INCREMENT_STAGE);

	/* Read the flag to clear status */
	TIFM_Assert(fmReadStatus(FLAG_GET) == FM_STATUS_PENDING);
}

static void HandleTuneEnableInterrupts(void)
{	
	/* Update to next handler */
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, INCREMENT_STAGE);

	FMC(fmDriver).opHandler_int_mask = MASK_FR;
	/* Enable FR interrupt */
	TIFM_Assert(fmWriteCommand(INT_MASK_SET_GET, FMC(fmDriver).opHandler_int_mask) == FM_STATUS_PENDING);
}

static void HandleTuneStartTuning(void)
{	
	/* Update to next handler */
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, INCREMENT_STAGE);

	/* Start tuning */
	TIFM_Assert(fmWriteCommand(TUNER_MODE_SET, TUNER_MODE_PRESET_MODE) == FM_STATUS_PENDING);
}

static void HandleTuneWaitStartTuneCmdComplete(void)
{	
	/* Update to next handler */
	prepareNextStage(DONT_WAIT, INCREMENT_STAGE);

	TIFM_Assert(FMC(callReason) == CALL_REASON_CMD_CMPLT_EVT);

/*	TIFM_Report("FMFMFM expected 1 - got %d", FMC(callReason));*/
	/* Got command complete - Just wait for interrupt */
}

static void HandleTuneFinishedReadFreq(void)
{	
	TIFM_Assert(FMC(callReason) == CALL_REASON_INTERRUPT);
/*	TIFM_Report("FMFMFM expected 2 - got %d", FMC(callReason));*/

	/* Update to next handler */
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, INCREMENT_STAGE);

	/* Verify we got an interrupt - the tuning is finished */
	TIFM_Assert(FMC(opHandlerIntSetBits) & MASK_FR);

	/* Read frequency to notify the application */
	TIFM_Assert(fmReadStatus(FREQ_SET_GET) == FM_STATUS_PENDING);
}

static void HandleTuneFinishedEnableDefaultInts(void)
{	
	/* Update to next handler */
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, INCREMENT_STAGE);

	FMC(fmDriver).opHandler_int_mask = 0;
	/* Disable FR and enable the default interrupts */
	TIFM_Assert(fmWriteCommand(INT_MASK_SET_GET, FMC(fmDriver).gen_int_mask) == FM_STATUS_PENDING);

}
static void HandleTuneFinish(void)
{	
	TIFM_U32 freq;

	freq = FmIndexToFreq(FMC(curEvent).read_param);
	
	fm_reset_station_params(freq);

	send_fm_event_radio_tuned(FM_STATUS_SUCCESS, freq);

	FmOperationEnd();
}

static void HandleRssiGetStart(void)
{
	/* Update to next handler */
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, INCREMENT_STAGE);

	TIFM_Assert(fmReadStatus(RSSI_LEVEL_GET) == FM_STATUS_PENDING);			

}
static void HandleRssiGetFinish(void)
{
	/* Send event to the applicatoin */
	send_fm_event_cmd_done(FMC(curOp)->opType, FM_STATUS_SUCCESS, FMC(curEvent).read_param);
	FmOperationEnd();
}

static void HandleSeekMain(void)
{
	static FmOpHandlerArray handlerArray;

	/* If we start the command update the handler and stage */
	if(FMC(callReason) == CALL_REASON_START)
	{
		FMC(seekOp).curStage = 0;
		handlerArray = seekHandler;
	}
	/* If an upper event was received update the handler to stop seek */
	else if(FMC(callReason) == CALL_REASON_UPPER_EVT)
	{
		/* The seek is already finished - just finish the operation
			Call the next seek stage with cmd complete because upper
			event always wait for the cmd complete event */
		if((FMC(seekOp).curStage) > 7)
		{
			FMC(callReason) = CALL_REASON_CMD_CMPLT_EVT;
		}
		else
		{
			FMC(seekOp).seekStageBeforeStop = FMC(seekOp).curStage;
			FMC(seekOp).curStage = 0;
			handlerArray = stopSeekHandler;
		}
	}

	/* Call the handler and update to next stage */
	handlerArray[FMC(seekOp).curStage]();
	FMC(seekOp).curStage++;
}

static void HandleSeekStart(void)
{
	/* Update to next handler */
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, 0);

	/* Read frequency to know which frequency to start at */
	TIFM_Assert(fmReadStatus(FREQ_SET_GET) == FM_STATUS_PENDING);	
}
static void HandleSeekSetFreq(void)
{
	TIFM_U16 index;
	TIFM_U16 new_index;

	/* Update to next handler */
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, 0);

	index = FMC(curEvent).read_param;
	
	new_index = findNextIndex(FMC(seekOp).dir, index);
	
	/* Send Set_Frequency command */	
	TIFM_Assert(fmWriteCommand(FREQ_SET_GET, new_index) == FM_STATUS_PENDING);
}

static void HandleSeekSetDir(void)
{

	/* Update to next handler */
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, 0);
	
	/* Send Set_Frequency command */	
	TIFM_Assert(fmWriteCommand(SEARCH_DIR_SET_GET, FMC(seekOp).dir) == FM_STATUS_PENDING);
}
static void HandleSeekClearFlag(void)
{	
	/* Update to next handler */
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, 0);
	/* Read the flag to clear status */
	TIFM_Assert(fmReadStatus(FLAG_GET) == FM_STATUS_PENDING);
}

static void HandleSeekEnableInterrupts(void)
{	
	/* Update to next handler */
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, 0);

	FMC(fmDriver).opHandler_int_mask = MASK_FR | MASK_BL;
	/* Enable FR and BL interrupts */
	TIFM_Assert(fmWriteCommand(INT_MASK_SET_GET, FMC(fmDriver).opHandler_int_mask) == FM_STATUS_PENDING);
}
static void HandleSeekStartTuning(void)
{	
	/* Update to next handler */
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, 0);

	/* Start tuning */
	TIFM_Assert(fmWriteCommand(TUNER_MODE_SET, TUNER_MODE_SEARCH_MODE) == FM_STATUS_PENDING);
}

static void HandleSeekWaitStartTuneCmdComplete(void)
{	

	TIFM_Assert(FMC(callReason) == CALL_REASON_CMD_CMPLT_EVT);
	
	/* Update to next handler */
	prepareNextStage(DONT_WAIT, 0);

}


static void HandleStopSeekStart(void)
{
	/* If the seek didn't start yet, no need to stop.
	   Just go to the last stages in order to finish properly */
	if(FMC(seekOp).seekStageBeforeStop <= 5)
	{
		FMC(seekOp).status = FM_STATUS_SEEK_STOPPED;
		FMC(seekOp).curStage = 5;
		stopSeekHandler[FMC(seekOp).curStage]();	
	}
	else
	{
		/* Update to next handler */
		prepareNextStage(WAIT_FOR_CMD_COMPLETE, 0);

		/* Stop the seek */
		TIFM_Assert(fmWriteCommand(TUNER_MODE_SET, TUNER_MODE_STOP_SEARCH) == FM_STATUS_PENDING);
	}
}

static void HandleStopSeekWaitCmdCompleteOrInt(void)
{	
	/* Update to next handler */
	prepareNextStage(DONT_WAIT, 0);

	/* If we have received cmd complete event first then the stop seek command
	   was sent. Now wait for the interrupt to be received */
	if(FMC(callReason) == CALL_REASON_CMD_CMPLT_EVT)
	{
		FMC(seekOp).status = FM_STATUS_STOP_SEEK;
	}
	/* If we got an interrupt although we expected a cmd complete event
	   then the interrupt was received on the seek operation and not on
	   the stop seek.
	   Still we will have to wait for the cmd complete event */
	else if(FMC(callReason) == CALL_REASON_INTERRUPT)
	{
		TIFM_Assert( FMC(opHandlerIntSetBits) & (MASK_FR|MASK_BL));
		
		if(FMC(opHandlerIntSetBits) & MASK_BL)
		{
			FMC(seekOp).status = FM_STATUS_SEEK_REACHED_BAND_LIMIT;
		}
		else
		{
			FMC(seekOp).status = FM_STATUS_SEEK_SUCCESS;
		}
	}
}

/* Got interrupt either from seek or stop seek operation */
static void HandleSeekStopSeekFinishedReadFreq(void)
{	
	/* Update to next handler */
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, 0);
	
	/* We could have got a cmd complete in a case of stop seek
	   when we first got interrupt and now we get the cmd complete
	   for the stop seek. */
	if(FMC(callReason) == CALL_REASON_INTERRUPT)
	{
		TIFM_Assert( FMC(opHandlerIntSetBits) & (MASK_FR|MASK_BL));

		if(FMC(opHandlerIntSetBits) & MASK_BL)
		{
			FMC(seekOp).status = FM_STATUS_SEEK_REACHED_BAND_LIMIT;
		}
		else
		{
			if(FMC(seekOp).status == FM_STATUS_STOP_SEEK)
			{
				FMC(seekOp).status = FM_STATUS_SEEK_STOPPED;
			}
			else
			{
				FMC(seekOp).status = FM_STATUS_SEEK_SUCCESS;
			}
		}
	}

	/* Read frequency to notify the application */
	TIFM_Assert(fmReadStatus(FREQ_SET_GET) == FM_STATUS_PENDING);
}


static void HandleSeekStopSeekFinishedEnableDefaultInts(void)
{	
	/* Update to next handler */
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, 0);

	FMC(fmDriver).opHandler_int_mask = 0;
	/* Disable FR and enable the default interrupts */
	TIFM_Assert(fmWriteCommand(INT_MASK_SET_GET, FMC(fmDriver).gen_int_mask) == FM_STATUS_PENDING);

}
static void HandleSeekStopSeekFinish(void)
{	
	TIFM_U32 freq;

	freq = FmIndexToFreq(FMC(curEvent).read_param);

	fm_reset_station_params(freq);

	send_fm_event_cmd_done(FMC(curOp)->opType, FMC(seekOp).status, freq);
	
	FmOperationEnd();
}

static void HandleStopSeekBeforeSeekStarted(void)
{
	send_fm_event_cmd_done(FMC(curOp)->opType, FMC(seekOp).status, FMC(curStationParams).freq);
	
	FmOperationEnd();
}
static void HandleSetAFStart(void)
{
	/* Update to next handler */
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, INCREMENT_STAGE);
	
	FMC(AFOn) = FMC(fmCurOpParam).param.mode;

	if(FMC(AFOn) == FM_AF_ON)
	{
		/* Update global parameter gen_int_mask to enable also low rssi interrupt */
		FMC(fmDriver).gen_int_mask |= MASK_LEV;
	}
	else
	{
		/* Cancel inactivity timer started at the beginning of AF jumps */
		BTHAL_OS_CancelTimer(FMC(fmTimerHandle));
        
		/* Update global parameter gen_int_mask to disable low rssi interrupt */
		FMC(fmDriver).gen_int_mask &= ~(MASK_LEV);
	}

	/* Enable the interrupts */
	TIFM_Assert(fmWriteCommand(INT_MASK_SET_GET, FMC(fmDriver).gen_int_mask) == FM_STATUS_PENDING);
}
static void HandleSetAfFinish(void)
{
	/* Send event to the applicatoin */
	send_fm_event_cmd_done(FMC(curOp)->opType, FM_STATUS_SUCCESS, FMC(fmCurOpParam).param.mode);
	FmOperationEnd();
}

static void HandleSetStereoBlendStart(void)
{
	/* Update to next handler */
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, INCREMENT_STAGE);
		
	/* Send Set_MoSt command */
	TIFM_Assert(fmWriteCommand(MOST_BLEND_SET_GET, FMC(fmCurOpParam).param.mode) == FM_STATUS_PENDING);			

}
static void HandleSetStereoBlendFinish(void)
{
	/* Send event to the applicatoin */
	send_fm_event_cmd_done(FMC(curOp)->opType, FM_STATUS_SUCCESS, FM_NO_VALUE);
	FmOperationEnd();
}

static void HandleSetDeemphasisModeStart(void)
{
	/* Update to next handler */
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, INCREMENT_STAGE);

	/* Send Set_MoSt command */
	TIFM_Assert(fmWriteCommand(DEMPH_MODE_SET_GET, FMC(fmCurOpParam).param.mode) == FM_STATUS_PENDING);			

}
static void HandleSetDeemphasisModeFinish(void)
{
	/* Send event to the applicatoin */
	send_fm_event_cmd_done(FMC(curOp)->opType, FM_STATUS_SUCCESS, FM_NO_VALUE);
	FmOperationEnd();
}
static void HandleSetRssiSearchLevelStart(void)
{
	/* Update to next handler */
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, INCREMENT_STAGE);

	/* Send Set_MoSt command */
	TIFM_Assert(fmWriteCommand(SEARCH_LVL_SET_GET, FMC(fmCurOpParam).param.rssi_level) == FM_STATUS_PENDING);			

}
static void HandleSetRssiSearchLevelFinish(void)
{
	/* Send event to the applicatoin */
	send_fm_event_cmd_done(FMC(curOp)->opType, FM_STATUS_SUCCESS, FM_NO_VALUE);
	FmOperationEnd();
}
static void HandleSetPauseLevelStart(void)
{
	/* Update to next handler */
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, INCREMENT_STAGE);

	/* Send Set_MoSt command */
	TIFM_Assert(fmWriteCommand(RDS_PAUSE_LVL_SET_GET, FMC(fmCurOpParam).param.pause_level) == FM_STATUS_PENDING);			

}
static void HandleSetPauseLevelFinish(void)
{
	/* Send event to the applicatoin */
	send_fm_event_cmd_done(FMC(curOp)->opType, FM_STATUS_SUCCESS, FM_NO_VALUE);
	FmOperationEnd();
}

static void HandleSetPauseDurationStart(void)
{
	/* Update to next handler */
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, INCREMENT_STAGE);

	/* Send Set_MoSt command */
	TIFM_Assert(fmWriteCommand(RDS_PAUSE_DUR_SET_GET, FMC(fmCurOpParam).param.pause_duration) == FM_STATUS_PENDING);			

}
static void HandleSetPauseDurationFinish(void)
{
	/* Send event to the applicatoin */
	send_fm_event_cmd_done(FMC(curOp)->opType, FM_STATUS_SUCCESS, FM_NO_VALUE);
	FmOperationEnd();
}

static void HandleSetRdsRbdsStart(void)
{
	/* Update to next handler */
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, INCREMENT_STAGE);

	/* Send Set_MoSt command */
	TIFM_Assert(fmWriteCommand(RDS_SYSTEM_SET_GET, FMC(fmCurOpParam).param.mode) == FM_STATUS_PENDING);
}

static void HandleSetRdsRbdsFinish(void)
{
	/* Send event to the applicatoin */
	send_fm_event_cmd_done(FMC(curOp)->opType, FM_STATUS_SUCCESS, FM_NO_VALUE);
	FmOperationEnd();
}

static void HandleMoStGetStart(void)
{
	/* Update to next handler */
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, INCREMENT_STAGE);

	TIFM_Assert(fmReadStatus(MOST_MODE_SET_GET) == FM_STATUS_PENDING);			

}
static void HandleMoStGetFinish(void)
{
	/* Send event to the applicatoin */
	send_fm_event_cmd_done(FMC(curOp)->opType, FM_STATUS_SUCCESS, FMC(curEvent).read_param);
	FmOperationEnd();
}

static void HandleAudioEnableStart(void)
{
	/* Update to next handler */
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, INCREMENT_STAGE);

	TIFM_Assert(fmWriteCommand(AUDIO_ENABLE_SET_GET, 3) == FM_STATUS_PENDING);			

}
static void HandleAudioEnableFinish(void)
{
	FmOperationEnd();
}


static void HandleGenIntMal(void)
{

	if((FMC(genIntSetBits)) & (FMC(fmDriver).gen_int_mask) & MASK_MAL)
	{
		FmHandleMalfunction();
	}
	else
	{
		genIntHandler[GEN_INT_AFTER_MAL_STAGE]();
	}
}
static void HandleGenIntStereoChange(void)
{
	/* Handle specific interrupts */
	if((FMC(genIntSetBits)) & (FMC(fmDriver).gen_int_mask) & MASK_STIC)
	{
		FmHandleStereoChange();
	}
	else
	{
		genIntHandler[GEN_INT_AFTER_STEREO_CHANGE_STAGE]();
	}
}

static void HandleGenIntRds(void)
{
	/* Handle specific interrupts */
	if((FMC(genIntSetBits)) & (FMC(fmDriver).gen_int_mask) & MASK_RDS)
	{
		FmHandleRdsRx();
	}
	else
	{
		genIntHandler[GEN_INT_AFTER_RDS_STAGE]();
	}
}

static void HandleGenIntLowRssi(void)
{
	TIFM_BOOL go_to_next = TIFM_FALSE;
	BthalStatus bthalStatus;
	
	/* If low RSSI interrupt occurred  */
	if((FMC(genIntSetBits)) & (FMC(fmDriver).gen_int_mask) & MASK_LEV)
	{
		bthalStatus = BTHAL_OS_ResetTimer(FMC(fmTimerHandle),
                                              BTHAL_OS_MS_TO_TICKS(FM_CONFIG_AF_TIMER_MS),
                                              OS_EVENT_FM_TIMER_EXPIRED);
		TIFM_Assert(bthalStatus == BTHAL_STATUS_SUCCESS);
            
		/* Update global parameter gen_int_mask to disable low RSSI interrupt -
		 * we do not need it during FM_CONFIG_AF_TIMER_MS timeout */
		FMC(fmDriver).gen_int_mask &= ~MASK_LEV;
        
		if(isAfJumpValid())
		{
			FmHandleAfJump();
		}
		else
		{
			go_to_next = TIFM_TRUE;
		}
	}
	else
	{
		go_to_next = TIFM_TRUE;
	}
	
	if(go_to_next)
	{
		genIntHandler[GEN_INT_AFTER_LOW_RSSI_STAGE]();
	}
}

static void HandleGenIntEnableInt(void)
{
	/* Update operation back to general interrupts */
	FMC(curOp)->opType = FM_HANDLE_GEN_INT;
	
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, GEN_INT_AFTER_FINISH_STAGE);

	/* After handling all interrupts write to the mask again to enable interrupts (otherwise it is still disabled) */
	/* Must be the last thing done in this operation. otherwise another general interrupt possible in parallel */
	TIFM_Assert(fmWriteCommand(INT_MASK_SET_GET, FMC(fmDriver).gen_int_mask) == FM_STATUS_PENDING);
	
}

static void HandleGenIntFinish(void)
{
	FmOperationEnd();
}

static void FmHandleRdsRx(void)
{
	OS_LockFmStack();
	FMC(curOp)->opType = FM_READ_RDS;
	OS_UnlockFmStack();

	prepareNextStage(WAIT_FOR_CMD_COMPLETE, 0);

	FMC(fmOpHandler)();
}

static void FmHandleAfJump(void)
{
	OS_LockFmStack();
	FMC(curOp)->opType = FM_HANDLE_AF_JUMP;
	OS_UnlockFmStack();
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, 0);

	initAfJumpParams();

	FMC(fmOpHandler)();
}

static void FmHandleStereoChange(void)
{
	OS_LockFmStack();
	FMC(curOp)->opType = FM_HANDLE_STEREO_CHANGE;
	OS_UnlockFmStack();

	prepareNextStage(WAIT_FOR_CMD_COMPLETE, 0);

	FMC(fmOpHandler)();
}

static void FmHandleMalfunction(void)
{
	OS_LockFmStack();
	FMC(curOp)->opType = FM_HANDLE_HW_MAL;
	OS_UnlockFmStack();

	prepareNextStage(WAIT_FOR_CMD_COMPLETE, 0);

	FMC(fmOpHandler)();
}

static TIFM_BOOL isAfJumpValid(void)
{
	TIFM_U8 isValid = TIFM_TRUE;

	/* If AF feature is off - do nothing */
	if(FMC(AFOn) == TIFM_FALSE)
	{
		isValid = TIFM_FALSE;
	}

	/* If we are not tuned or the AF list is empty - do nothing */
	if((FMC(curStationParams).freq == NO_STATION_FREQ) || (FMC(curStationParams).afListSize == 0))
	{
		isValid = TIFM_FALSE;
	}

	return isValid;
}

static void initAfJumpParams(void)
{
	/* Update parameters before starting the jump */
	FMC(curAfJumpIndex) = 0;
	/* Save the frequency before the jump to compare later if a jump was done */
	FMC(freqBeforeJump) = FMC(curStationParams).freq;
}

static void HandleAfJumpStartSetPi(void)
{
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, INCREMENT_STAGE);

	/* Set PI code - must be updated if the af list is not empty */	
	TIFM_Assert(fmWriteCommand(RDS_PI_SET_GET, FMC(curStationParams).piCode) == FM_STATUS_PENDING);
}

static void HandleAfJumpSetPiMask(void)
{
	TIFM_U16 piMask = 0xFFFF;

	prepareNextStage(WAIT_FOR_CMD_COMPLETE, INCREMENT_STAGE);

	/* Set PI code - must be updated if the af list is not empty */	
	TIFM_Assert(fmWriteCommand(RDS_PI_MASK_SET_GET, piMask) == FM_STATUS_PENDING);
}

static void HandleAfJumpSetAfFreq(void)
{
	TIFM_U16 freqIndex;
	
	freqIndex = FmFreqToIndex(FMC(curStationParams).afList[FMC(curAfJumpIndex)]);

	prepareNextStage(WAIT_FOR_CMD_COMPLETE, INCREMENT_STAGE);

	/* Send Set_Frequency command */	
	TIFM_Assert(fmWriteCommand(AF_FREQ_SET_GET, freqIndex) == FM_STATUS_PENDING);
}

static void HandleAfJumpEnableInt(void)
{
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, INCREMENT_STAGE);

	FMC(fmDriver).opHandler_int_mask = MASK_FR;
	/* Enable FR interrupt */
	TIFM_Assert(fmWriteCommand(INT_MASK_SET_GET, FMC(fmDriver).opHandler_int_mask) == FM_STATUS_PENDING);
}

static void HandleAfJumpStartAfJump(void)
{
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, INCREMENT_STAGE);

	/* Start tuning */
	TIFM_Assert(fmWriteCommand(TUNER_MODE_SET, TUNER_MODE_AF_JUMP_MODE) == FM_STATUS_PENDING);
}

static void HandleAfJumpWaitCmdComplete(void)
{
	TIFM_Assert(FMC(callReason) == CALL_REASON_CMD_CMPLT_EVT);

	/* Update to next handler */
	prepareNextStage(DONT_WAIT, INCREMENT_STAGE);

	/* Got command complete - Just wait for interrupt */
}

static void HandleAfJumpReadFreq(void)
{
	TIFM_Assert(FMC(callReason) == CALL_REASON_INTERRUPT);

	/* Update to next handler */
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, INCREMENT_STAGE);
	
	/* Verify we got an interrupt - the tuning is finished */
	TIFM_Assert(FMC(opHandlerIntSetBits) & MASK_FR);

	/* Read frequency to notify the application */
	TIFM_Assert(fmReadStatus(FREQ_SET_GET) == FM_STATUS_PENDING);
}

static void HandleAfJumpFinished(void)
{	
	TIFM_U32 read_freq, jumped_freq;
	TIFM_U16 curPi = FMC(curStationParams).piCode;

	/* No need to enable all interrupt again - it will be done at the end of handling the general interrupts */

	read_freq = FmIndexToFreq(FMC(curEvent).read_param);

	jumped_freq = FMC(curStationParams).afList[FMC(curAfJumpIndex)];

	/* If the frequency was changed the jump succeeded */
	if(read_freq != FMC(freqBeforeJump))
	{	
		/* There was a jump - make sure it was to the frequency we set */
		TIFM_Assert(jumped_freq == read_freq);

		fm_reset_station_params(read_freq);
		FMC(curStationParams).piCode = curPi; /* The PI should stay the same */
		
		send_fm_event_af_jump(AF_JUMP_SUCCESS, curPi, FMC(freqBeforeJump), read_freq);

		/* Reset the int_mask */
		FMC(fmDriver).opHandler_int_mask = 0;
		
		/* Call the next stage of general interrupts handler to handle other interrupts */
		genIntHandler[GEN_INT_AFTER_LOW_RSSI_STAGE]();
	}
	/* Tried to jump but jumped back to the original frequency - jump to the next freq in the af list */
	else
	{
		FMC(curAfJumpIndex)++;	/* Go to next index in the af list */
		
		/* If we reached the end of the list - stop searching */
		if(FMC(curAfJumpIndex) >= FMC(curStationParams).afListSize)
		{
			send_fm_event_af_jump(AF_JUMP_FAILED_LIST_FINISHED, curPi, read_freq, jumped_freq);

			/* Reset the int_mask */
			FMC(fmDriver).opHandler_int_mask = 0;

			/* Call the next stage of general interrupts handler to handle other interrupts */
			genIntHandler[GEN_INT_AFTER_LOW_RSSI_STAGE]();
		}
		/* List is not over - try next one */
		else
		{
			send_fm_event_af_jump(AF_JUMP_FAILED_LIST_NOT_FINISHED, curPi, read_freq, jumped_freq);

			/* Call the first handler again */
			prepareNextStage(DONT_WAIT, 0);
		
			FMC(fmOpHandler)();	
		}	
	}
}

static void HandleStereoChangeStart(void)
{
	/* Update to next handler */
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, INCREMENT_STAGE);

	TIFM_Assert(fmReadStatus(STEREO_GET) == FM_STATUS_PENDING);			
}

static void HandleStereoChangeFinish(void)
{
	TIFM_U8	monoMode;

	/* 0 = Mono; 1 = Stereo */
	if ((TIFM_U8)FMC(curEvent).read_param == 0)
	{
		monoMode = 1;
	}
	else
	{
		monoMode = 0;
	}
	
	/* Send event to the applicatoin */
	send_fm_event_most_mode_changed(monoMode);

	/* Finished analyzing - call the next stage of general interrupts handler to handle other interrupts */
	genIntHandler[GEN_INT_AFTER_STEREO_CHANGE_STAGE]();
}

static void HandleHwMalStartFinish(void)
{
	TIFM_Report(("Hw MAL interrupt received - do nothing"));
	/* Finished analyzing - call the next stage of general interrupts handler to handle other interrupts */
	genIntHandler[GEN_INT_AFTER_MAL_STAGE]();
}

static void HandleReadRdsStart(void)
{
	/* Update to next handler */
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, INCREMENT_STAGE);

	/* Send Read RDS command */	
	TIFM_Assert(fmReadRds() == FM_STATUS_PENDING);
}

static void HandleReadRdsAnalyze(void)
{
	GetRDSBlock();

	/* Finished analyzing - call the next stage of general interrupts handler to handle other interrupts */
	genIntHandler[GEN_INT_AFTER_RDS_STAGE]();
}

static void GetRDSBlock(void)
{
	TIFM_U16 len = FMC(genIntOp).readRdsOp.len;
	TIFM_U8 status, type, index;
	TIFM_U8 *data = FMC(genIntOp).readRdsOp.rdsData;

    /* Zero length of Raw RDS data event */
    FMC(rawRdsEvt).len = 0;
	
	/* Parse the RDS data */
	while (len >= RDS_BLOCK_SIZE)
	{
		status = data[2];

		/* Get the type:
		 0=A, 1=B, 2=C, 3=C', 4=D, 5=E */
		type = (TIFM_U8)(status & 0x07);

		/* Transform the block type into an index sequence (0, 1, 2, 3, 4) */
		index =  (TIFM_U8)(type <= RDS_BLOCK_C ? type : (type - 1));

/*		TIFM_Report("Block index = %d", index);*/
		/* Is it block A or is it a sequence block after the previous one? */
		if (index == RDS_BLOCK_INDEX_A ||
			(index == FMC(rdsParams).last_block_index + 1 &&
			 index <= RDS_BLOCK_INDEX_D))
		{
			/* Copy it and save the index	*/
			BTHAL_UTILS_MemCopy(&FMC(rdsParams).rdsGroup[index * RDS_BLOCK_SIZE], data, RDS_BLOCK_SIZE);
			FMC(rdsParams).last_block_index = index;

			/* If completed a whole group then handle it */
			if (index == RDS_BLOCK_INDEX_D)
			{
/*				TIFM_Report("Block good - handle group");*/
				handleRdsGroup();
			}
		}
		else
		{
			TIFM_Report(("Block sequence mismatch\n"));
			FMC(rdsParams).last_block_index = RDS_BLOCK_INDEX_UNKNOWN;
		}

		len -= RDS_BLOCK_SIZE;
		data += RDS_BLOCK_SIZE;
	}

    /* Send Raw RDS event only, if its length is not 0 */
    if (FMC(rawRdsEvt).len)
    {
	    /* Send the Raw RDS data to the application */
	    send_fm_event_raw_rds(FMC(rawRdsEvt).len, FMC(rawRdsEvt).data);
    }
}


static void handleRdsGroup(void)
{
	TIFM_U8 groupType;
	TIFM_U16 piIndex;
	TIFM_BOOL goodPi = TIFM_FALSE;
    TIFM_U8 *readRdsData;
    TIFM_U8 idx1, idx2;

	piIndex = TIFM_BigEtoHost16(&FMC(rdsParams).rdsGroup[RDS_BLOCK_A_PI_INDEX]);

	/* If PI is unknown yet - read it */
	if ((FMC(rdsParams).rdsGroup[RDS_BLOCK_A_STATUS_INDEX] & RDS_STATUS_ERROR_MASK) == 0)
	{
		if(FMC(curStationParams).piCode == NO_PI_CODE)
		{
			FMC(curStationParams).piCode = piIndex;
			goodPi = TIFM_TRUE;
/*			TIFM_Report(("PI = 0x%x\n", piIndex));*/
		}
		else
		{
			if(piIndex == FMC(curStationParams).piCode)
			{
/*				TIFM_Report("Good PI");*/
				goodPi = TIFM_TRUE;
			}
			else
			{
				TIFM_Report(("different PI 0x%x\n", piIndex));
			}
		}
	}
	else
	{
/*		TIFM_Report("ERROR IN BLOCK A");*/
	}

	if(!goodPi)
	{
		return;
	}

	/* Before reading group type check if block B has no errors */
	if ((FMC(rdsParams).rdsGroup[RDS_BLOCK_B_STATUS_INDEX] & RDS_STATUS_ERROR_MASK) == 0)
	{
        readRdsData = &FMC(rdsParams).rdsGroup[0];
        
        /* Copy group to Raw RDS event buffer block by block without error checking bytes */
		for (idx1=0; idx1<RDS_BLOCKS_IN_GROUP; idx1++)
        {
		    for (idx2=0; idx2<RDS_BLOCK_SIZE-1; idx2++)
            {
                FMC(rawRdsEvt).data[FMC(rawRdsEvt).len++] = *readRdsData++;
            }

            /* Skip error checking byte */
            readRdsData++;
        }
        
        groupType = (TIFM_U8)((FMC(rdsParams).rdsGroup[RDS_BLOCK_B_GROUP_TYPE_INDEX] & RDS_BLOCK_B_GROUP_TYPE_MASK) >> 4);

		/* If it's group type 0 - handle AF and PS */
		if (groupType == RDS_GROUP_TYPE_0)
		{
/*			TIFM_Report("Handle group 0");*/
			handleRdsGroup0();
		}
		else if(groupType == RDS_GROUP_TYPE_2)
		{
/*			TIFM_Report("Handle group 2");*/
			handleRdsGroup2();
		}
		else
		{
			TIFM_Report(("Not handling group type %d.\n", groupType));
		}
	}
	else
	{
		TIFM_Report(("Ignore - error in block B.\n"));
	}
}

static void	handleRdsGroup0(void)
{
	TIFM_U8 psIndex;
	TIFM_U16 dataB, dataA;
	TIFM_BOOL changed1, changed2;

	dataB = TIFM_BigEtoHost16(&FMC(rdsParams).rdsGroup[RDS_BLOCK_B_GROUP_TYPE_INDEX]);

	/* Check if block D that contains the PS has no error */
	if((FMC(rdsParams).rdsGroup[RDS_BLOCK_D_STATUS_INDEX] & RDS_STATUS_ERROR_MASK) == 0)
	{
		psIndex = (TIFM_U8)(dataB & RDS_BLOCK_B_PS_INDEX_MASK);
/*		TIFM_Report("PS index = %d", psIndex);*/
		/* Check if the name index is expected	*/
		if (FMC(rdsParams).nextPsIndex == psIndex ||
			psIndex == 0)
		{
			FMC(rdsParams).nextPsIndex = (TIFM_U8)(psIndex + 1);

			FMC(rdsParams).psName[psIndex*2] = FMC(rdsParams).rdsGroup[RDS_BLOCK_D_PS1_INDEX];
			FMC(rdsParams).psName[psIndex*2+1] = FMC(rdsParams).rdsGroup[RDS_BLOCK_D_PS2_INDEX];

			/* Is this is the last index?	*/
			if (psIndex + 1 >= RDS_PS_NAME_LAST_INDEX)
			{
				/* If ps name was changed or it's the first time - update */
				if(!BTHAL_UTILS_MemCmp((TIFM_U8*)(FMC(rdsParams).psName), RDS_PS_NAME_SIZE, FMC(curStationParams).psName, RDS_PS_NAME_SIZE))
				{
					BTHAL_UTILS_MemCopy(FMC(curStationParams).psName, (TIFM_U8*)(FMC(rdsParams).psName), RDS_PS_NAME_SIZE);
					send_fm_event_ps_changed(FMC(curStationParams).freq, FMC(curStationParams).psName);
				}
				else
				{
/*					TIFM_Report("PS name NOT changed\n");					*/
				}
			}
		}
		else 
		{
			/* Reset the expected index to prevent accidentally receive the correct index of the next ps name */
			FMC(rdsParams).nextPsIndex = RDS_NEXT_PS_INDEX_RESET;
/*			TIFM_Report("PS Name not in sequence - ignored.\n");*/
		}			
	}
	else
	{
		/* Reset the expected index to prevent accidentally receive the correct index of the next ps name */
		FMC(rdsParams).nextPsIndex = RDS_NEXT_PS_INDEX_RESET;
/*		TIFM_Report("Ignore - error in block D.\n");*/
	}


/*****************************
 * Check Alternate Frequencies
 *****************************/
 
	/* Check that we work with 0A group (and not 0B) that contains AF 
		Check no errors in block C (contains AF) */
	if (0 == (dataB & RDS_BLOCK_B_AB_BIT_MASK) &&			
		0 == (FMC(rdsParams).rdsGroup[RDS_BLOCK_C_STATUS_INDEX] & RDS_STATUS_ERROR_MASK))
	{
		dataA = TIFM_BigEtoHost16(&FMC(rdsParams).rdsGroup[RDS_BLOCK_A_PI_INDEX]);

		
		changed1 = checkNewAf(FMC(rdsParams).rdsGroup[RDS_BLOCK_C_AF1]);
		changed2 = checkNewAf(FMC(rdsParams).rdsGroup[RDS_BLOCK_C_AF2]);

		if (changed1 || changed2)
		{
			send_fm_event_af_list_changed(FMC(curStationParams).piCode, FMC(curStationParams).afListSize, FMC(curStationParams).afList);
		}
	}
}

static TIFM_BOOL checkNewAf(TIFM_U8 af)
{
	TIFM_U32 freq, base_freq;
	TIFM_U8 index;

	TIFM_BOOL changed = TIFM_FALSE;
	
	/* First AF indicates the number of AF follows. Reset the list */
	if((af >= RDS_1_AF_FOLLOWS) && (af <= RDS_25_AF_FOLLOWS))
	{
		FMC(curStationParams).afListCurMaxSize = (TIFM_U8)(af - RDS_1_AF_FOLLOWS + 1);
		FMC(curStationParams).afListSize = 0;
		changed = TIFM_TRUE;
	}
	else if (((af >= RDS_MIN_AF) && (FMC(band) == FM_EUROPE_US_BAND) && (af <= RDS_MAX_AF)) ||
			((af >= RDS_MIN_AF) && (FMC(band) == FM_JAPAN_BAND) && (af <= RDS_MAX_AF_JAPAN)))
	{
		
		base_freq = FMC(band) ? BASE_FREQ_JAPAN : BASE_FREQ_US_EUROPE;
		freq = base_freq + (af * 100);

		/* If the af frequency is the same as the tuned station frequency - ignore it */
		if(freq == FMC(curStationParams).freq)
			return changed;

		for(index=0; ((index<FMC(curStationParams).afListSize) && (index < FMC(curStationParams).afListCurMaxSize)); index++)
		{
			if(FMC(curStationParams).afList[index] == freq)
				break;
		}
		
		/* Reached the limit of the list - ignore the next AF */
		if(index == FMC(curStationParams).afListCurMaxSize)
		{
/*			TIFM_Report("Error - af list is longer than expected.\n");*/
			return changed;
		}
		/*If we reached the end of the list then this AF is not in the list - add it */
		if(index == FMC(curStationParams).afListSize)
		{
			changed = TIFM_TRUE;
			FMC(curStationParams).afList[index] = freq;
			FMC(curStationParams).afListSize++;
		}
	}

	return changed;
}

static void	handleRdsGroup2(void)
{
	TIFM_U8 rtIndex, index, max_bytes, length = 0;
	TIFM_U16 dataB;
	TIFM_BOOL isAGroup, isError, isTextEnd = TIFM_FALSE, abFlag;

	dataB = TIFM_BigEtoHost16(&FMC(rdsParams).rdsGroup[RDS_BLOCK_B_GROUP_TYPE_INDEX]);

	/* Check if it's A or B group type */
	if (0 == (dataB & RDS_BLOCK_B_AB_BIT_MASK))
	{
		isAGroup = TIFM_TRUE;
		isError = (((FMC(rdsParams).rdsGroup[RDS_BLOCK_C_STATUS_INDEX] & RDS_STATUS_ERROR_MASK) != 0)
			|| ((FMC(rdsParams).rdsGroup[RDS_BLOCK_D_STATUS_INDEX] & RDS_STATUS_ERROR_MASK) != 0));
	}
	else
	{
		isAGroup = TIFM_FALSE;
		isError = ((FMC(rdsParams).rdsGroup[RDS_BLOCK_D_STATUS_INDEX] & RDS_STATUS_ERROR_MASK) != 0);
	}
 
	if(!isError)
	{

		rtIndex = (TIFM_U8)(dataB & RDS_BLOCK_B_RT_INDEX_MASK);
/*		TIFM_Report("RT index = %d", rtIndex);*/

		/* Start a new radio text - check the text A/B flag */
		if(rtIndex == 0)
		{
			abFlag = ((dataB & RDS_BLOCK_B_AB_FLAG_MASK) != 0);
			if(abFlag != FMC(rdsParams).prevABFlag)
			{
				FMC(rdsParams).changed = TIFM_TRUE;
				FMC(rdsParams).prevABFlag = (TIFM_U8)(abFlag);
			}
			else
			{
				FMC(rdsParams).changed = TIFM_FALSE;
			}
		}				

		/* Check if the RT index is expected */
		if (FMC(rdsParams).nextRtIndex == rtIndex ||
			rtIndex == 0)
		{
/*			if((rtIndex == 0) && (FMC(rdsParams).nextRtIndex >=5) && (FMC(rdsParams).nextRtIndex < RDS_NEXT_RT_INDEX_RESET))
			{
				return;
			}*/
			FMC(rdsParams).nextRtIndex = (TIFM_U8)(rtIndex + 1);

			if(isAGroup)
			{
				max_bytes = 4;
				FMC(rdsParams).radioText[rtIndex*max_bytes] = FMC(rdsParams).rdsGroup[RDS_BLOCK_C_RT1_INDEX]; 
				FMC(rdsParams).radioText[rtIndex*max_bytes+1] = FMC(rdsParams).rdsGroup[RDS_BLOCK_C_RT2_INDEX];
				FMC(rdsParams).radioText[rtIndex*max_bytes+2] = FMC(rdsParams).rdsGroup[RDS_BLOCK_D_RT1_INDEX];
				FMC(rdsParams).radioText[rtIndex*max_bytes+3] = FMC(rdsParams).rdsGroup[RDS_BLOCK_D_RT2_INDEX];

			}
			else
			{
				max_bytes = 2;
				FMC(rdsParams).radioText[rtIndex*max_bytes] = FMC(rdsParams).rdsGroup[RDS_BLOCK_D_RT1_INDEX];
				FMC(rdsParams).radioText[rtIndex*max_bytes+1] = FMC(rdsParams).rdsGroup[RDS_BLOCK_D_RT2_INDEX];
			}

			/* If we reached the last valid index */
			if(rtIndex + 1 >= RDS_RT_LAST_INDEX)
			{
				isTextEnd = TIFM_TRUE;
				length = (TIFM_U8)((rtIndex + 1) * max_bytes);
			}

			/* Check if the RT is finished before the maximum characters received */
			for(index = 0; index < max_bytes; index++)
			{
				if(FMC(rdsParams).radioText[rtIndex*max_bytes+index] == RDS_RT_END_CHARACTER)
				{
					isTextEnd = TIFM_TRUE;
					length = (TIFM_U8)(rtIndex*max_bytes+index);
					break;
				}
			}

			/* Is this the last index? */
			if (isTextEnd)
			{
				/* Reset the expected index to prevent accidentally receive the correct index of the next rt */
				FMC(rdsParams).nextRtIndex = RDS_NEXT_RT_INDEX_RESET;
/*				TIFM_Report("Show Radio Text");*/
				TIFM_Assert(length != 0);
				send_fm_event_radio_text(FMC(rdsParams).changed, length, (TIFM_U8*)(FMC(rdsParams).radioText));
			}
			else
			{
/*				TIFM_Report("Radio Text NOT END YET");*/
			}
		}
		else 
		{
			FMC(rdsParams).nextRtIndex = RDS_NEXT_RT_INDEX_RESET;
/*			TIFM_Report("Radio Text not in sequence - ignored.\n");*/
		}			
	}
	else
	{
		/* Reset the expected index to prevent accidentally receive the correct index of the next rt */
		FMC(rdsParams).nextRtIndex = RDS_NEXT_RT_INDEX_RESET;
/*		TIFM_Report("Ignore - error in block C or D.\n");*/
	}
}

static void HandleTimeoutStart(void)
{
	/* Update to next handler */
	prepareNextStage(WAIT_FOR_CMD_COMPLETE, INCREMENT_STAGE);

    /* Update global parameter gen_int_mask to enable low RSSI interrupt -
     * it was disabled during FM_CONFIG_AF_TIMER_MS timeout */
    FMC(fmDriver).gen_int_mask |= MASK_LEV;
        
    /* Enable the interrupts */
    TIFM_Assert(fmWriteCommand(INT_MASK_SET_GET, FMC(fmDriver).gen_int_mask) ==
        	                                                FM_STATUS_PENDING);
}

static void HandleTimeoutFinish(void)
{
	FmOperationEnd();
}

		

void fm_radio_init(void)
{

	fmDriverInit();

}
static TIFM_BOOL fmLoadInitScript(TIFM_U16 firm_version)
{
    TIFM_BOOL isSuccess = TIFM_TRUE;
	char version[30];

	/*clear the string */
	version[0] = 0;

	sprintf(version, "FMInit_%d.bts", firm_version);

	if (FM_STATUS_SUCCESS != FM_bts_LoadFmScript(version))
	{
		isSuccess = TIFM_FALSE;
	}

	return isSuccess;
}
static void fmSendInitScript(void)
{
	FmStatus status = FM_STATUS_PENDING;
	TIFM_S8 abBuffer[300];
	TIFM_U32 nSize;
	TIFM_U16 wType = 0;
    TIFM_BOOL bCmdExecuted = TIFM_FALSE;
	
	do
	{
		/* Process the init script */
		nSize = FM_bts_GetNextAction((TIFM_U8*)(&abBuffer[0]), sizeof(abBuffer), &wType);
	
		if (nSize > 0)
		{
			switch(wType)
			{
				/* We have an HCI command to execute here */
				 case FM_ACTION_SEND_COMMAND:
				 {
                     TIFM_U8 parmLen = abBuffer[3];
					 TIFM_U16 hci_opcode = TIFM_LittleEtoHost16((TIFM_U8*)(&abBuffer[1])); 
				     						
  					 status = fmSendInitCmd(hci_opcode, parmLen, (TIFM_U8*)(abBuffer + 4));
					 FMC(initState) = INIT_STATE_WAIT_CMD_COMPLETE;
				}
					break;
					
				case FM_ACTION_WAIT_EVENT:
						status = FM_STATUS_PENDING;
						bCmdExecuted = TIFM_TRUE;
					
					break;
				default:
					break;
			}
		}
		else
		{
            TIFM_Report(("FM: End of FM Init Script\n"));
			status = FM_STATUS_SUCCESS;
		}

	} while ((bCmdExecuted == TIFM_FALSE) && (nSize > 0) && (status != FM_STATUS_FAILED));

	/* Init script finished */		
	if(status != FM_STATUS_PENDING)
	{
		fmUnLoadInitScript();
		if(nSize == 0)
		{
			/* Update to next handler */
			prepareNextStage(WAIT_FOR_CMD_COMPLETE, INCREMENT_STAGE);

			TIFM_Assert(fmWriteCommand(INT_MASK_SET_GET, FMC(fmDriver).gen_int_mask) == FM_STATUS_PENDING);			
			FMC(initState) = INIT_STATE_FM_ON;
		}
		else
		{
		
			FMC(initState) = INIT_STATE_FM_ERROR;
			FM_FinishPowerOn(FM_STATUS_FAILED);
		}
	}
}

static void fmUnLoadInitScript(void)
{
    /* Close init script file */
    FM_bts_UnloadScript();
}

static void send_fm_event_cmd_done(TIFM_U8 cmd, FmStatus status, TIFM_U32 value)
{
	fmEvent.type = FMEVENT_CMD_DONE;
	fmEvent.p.cmd_done.cmd = cmd;
	fmEvent.p.cmd_done.status = status;
	fmEvent.p.cmd_done.value = value;

	fm_context.fmCallcack(&fmEvent);
}

static void send_fm_event_radio_tuned(TIFM_U8 status, TIFM_U32 frequency)
{
	fmEvent.type = FMEVENT_RADIO_TUNED;
	fmEvent.p.radio_tuned.status = status;
	fmEvent.p.radio_tuned.frequency = frequency;

	fm_context.fmCallcack(&fmEvent);
}

static void send_fm_event_ps_changed(TIFM_U32 freq, TIFM_U8 *data)
{
	fmEvent.type = FMEVENT_PS_CHANGED;
	fmEvent.p.ps_changed.frequency = freq;
	fmEvent.p.ps_changed.psName = data;

	fm_context.fmCallcack(&fmEvent);
}

static void send_fm_event_af_list_changed(TIFM_U16 pi, TIFM_U8 afListSize, TIFM_U32 *afList)
{
	fmEvent.type = FMEVENT_AF_LIST_CHANGED;
	fmEvent.p.af_list_changed.pi = pi;
	fmEvent.p.af_list_changed.afListSize = afListSize;
	fmEvent.p.af_list_changed.afList = afList;

	fm_context.fmCallcack(&fmEvent);
}

static void send_fm_event_af_jump(TIFM_U8 status, TIFM_U16 pi, TIFM_U32 oldFreq, TIFM_U32 newFreq)
{
	fmEvent.type = FMEVENT_AF_JUMP;
	fmEvent.p.af_jump.status = status;
	fmEvent.p.af_jump.pi = pi;
	fmEvent.p.af_jump.oldFreq = oldFreq;
	fmEvent.p.af_jump.newFreq = newFreq;

	fm_context.fmCallcack(&fmEvent);
}

static void send_fm_event_radio_text(TIFM_BOOL changed, TIFM_U8 length, TIFM_U8 *radioText)
{
	fmEvent.type = FMEVENT_RADIO_TEXT;
	fmEvent.p.radio_text.changed = changed;
	fmEvent.p.radio_text.length = length;
	fmEvent.p.radio_text.radioText = radioText;

	fm_context.fmCallcack(&fmEvent);
}

static void send_fm_event_most_mode_changed(TIFM_U8 mode)
{
	fmEvent.type = FMEVENT_MOST_MODE_CHANGED;
	fmEvent.p.most_mode_changed.mode = mode;

	fm_context.fmCallcack(&fmEvent);
}

static void send_fm_event_raw_rds(TIFM_U16 len, TIFM_U8 *data)
{
	fmEvent.type = FMEVENT_RAW_RDS;
	fmEvent.p.raw_rds.len = len;
	fmEvent.p.raw_rds.data = data;

	fm_context.fmCallcack(&fmEvent);
}

/*---------------------------------------------------------------------------
 *            fmWriteCommand()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Send an HCI vendor-specific write command to the FM
 *
 * Return:    status of operation. 
 */
static FmStatus fmWriteCommand(TIFM_U8 fm_opcode, TIFM_U16 data)
{
	FMC(commandParams[0]) = fm_opcode;
	TIFM_StoreLittleE16((FMC(commandParams)) + WR_CMD_LENGTH_LOCATION, WR_CMD_LENGTH);
	TIFM_StoreBigE16(((FMC(commandParams)) + (WR_CMD_PARAM_LOCATION)), data);
		
	return fm_transport_sendWriteCommand();
}

static FmStatus fmPowerModeCommand(TIFM_U8 mode)
{
	FMC(commandParams[0]) = mode;
	
	return fm_transport_sendPowerModeCommand();
}

static FmStatus fmSendInitCmd(TIFM_U16 hci_opcode, TIFM_U8 len, TIFM_U8 *data_ptr)
{
	BTHAL_UTILS_MemCopy(FMC(commandParams), data_ptr, len);

	return fm_transport_sendInitCmd(hci_opcode, len);
}

/*---------------------------------------------------------------------------
 *            FmReadStatus()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Send an HCI vendor-specific that reads FM's status
 *
 * Return:    status of operation. 
 */
static FmStatus fmReadStatus(TIFM_U8 fm_opcode)
{
	FMC(commandParams[0]) = fm_opcode;
	TIFM_StoreLittleE16((FMC(commandParams)) + RD_CMD_LENGTH_LOC, RD_CMD_LENGTH);

	return fm_transport_sendReadStatus();
}

static FmStatus fmReadRds(void)
{
	FMC(commandParams[0]) = RDS_DATA_GET;
	TIFM_StoreLittleE16((FMC(commandParams)) + RD_CMD_LENGTH_LOC, RDS_THRESHOLD*3);

	return fm_transport_sendFmReadRds();
}

/* This command is only for reading the flag when an interrupt is received.
   When we only want to clear the flag we will use the regular readStatus command */
static FmStatus fmReadInt(TIFM_U8 fm_opcode)
{
	FMC(commandParams[0]) = fm_opcode;
	TIFM_StoreLittleE16((FMC(commandParams)) + RD_CMD_LENGTH_LOC, RD_CMD_LENGTH);

	return fm_transport_sendReadInt(fm_opcode);
}


void fm_recvd_initCmdCmplt(TIFM_BOOL isValid)
{	
	OS_LockFmStack();
	
	if(isValid)
	{
		FMC(initState) = INIT_STATE_CMD_COMPLETE_DONE;
    }
	else
	{
		FMC(initState) = INIT_STATE_CMD_COMPLETE_DONE_ERROR;
		
		TIFM_Report(("Error in FM Init.\n"));
	}
	
	FMC(curEvent).cmdCmpltEvent = TIFM_TRUE;		

	NotifyFms();
	OS_UnlockFmStack();
	
}

void fm_recvd_readFlagCmdCmplt(TIFM_U8 *data)
{	
	OS_LockFmStack();
	FMC(intReadEvt) = TIFM_TRUE;
	FMC(fmFlag) = TIFM_BigEtoHost16(data);

	NotifyFms();
	OS_UnlockFmStack();
}

void fm_recvd_readMaskCmdCmplt(TIFM_U8 *data)
{	
	OS_LockFmStack();
	FMC(intReadEvt) = TIFM_TRUE;
	FMC(fmMask) = TIFM_BigEtoHost16(data);

	NotifyFms();
	OS_UnlockFmStack();
}

void fm_recvd_readCmdCmplt(TIFM_U8 *data)
{
	OS_LockFmStack();
	FMC(curEvent).cmdCmpltEvent = TIFM_TRUE;
	FMC(curEvent).read_param = TIFM_BigEtoHost16(data);

	NotifyFms();
	OS_UnlockFmStack();
}

void fm_recvd_readRdsCmdCmplt(TIFM_U8 len, TIFM_U8 *data)
{		
	OS_LockFmStack();
	FMC(curEvent).cmdCmpltEvent = TIFM_TRUE;
	FMC(genIntOp).readRdsOp.len = (TIFM_U8)(len-4);  /* 4 general bytes */
	BTHAL_UTILS_MemCopy(FMC(genIntOp).readRdsOp.rdsData, data, FMC(genIntOp).readRdsOp.len);

	NotifyFms();
	OS_UnlockFmStack();
}

void fm_recvd_writeCmdCmplt(void)
{	
	OS_LockFmStack();
	FMC(curEvent).cmdCmpltEvent = TIFM_TRUE;		

	NotifyFms();
	OS_UnlockFmStack();
}


void fm_recvd_intterupt(void)
{
	OS_LockFmStack();
	FMC(interruptInd) = TIFM_TRUE;
	NotifyFms();
	OS_UnlockFmStack();	
}

#endif /*BTL_CONFIG_FM_STACK == BTL_CONFIG_ENABLED*/



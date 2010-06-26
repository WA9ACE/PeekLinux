#include "fms_api.h"
#include "fms.h"
#include "fm_os.h"
#include "fms_transport.h"
#include "btl_config.h"
#include "bthal_vc.h"

#if 0

static TIFM_BOOL chipInit(void);
static FmStatus FM_SetPower(TIFM_U8 mode);
void FmsApiBthalVcCallback(const BthalVcEvent *vcEvent);

FmContext fm_context;

FmStatus FM_Init(void)
{
	/* Initialize FM stack and radio */
	if (TIFM_TRUE != FM_OsInit())
	{
		TIFM_Report(("FM: Fatal error - could not initialize the FM stack"));
		return FM_STATUS_FAILED;
	}
	else
	{
		fm_transport_init(transportStatusCallback);
		return FM_STATUS_SUCCESS;
	}
}

FmStatus FM_Deinit()
{
	if(FM_OsDeinit() == TIFM_FALSE)
		return FM_STATUS_FAILED;
	else
		return FM_STATUS_SUCCESS;
}

FmStatus FM_Create(FmAppHandle 			*appHandle,
							const FmCallBack 	fmCallback, 
							FmContext 		**fmContext)
{
	FmStatus			status = FM_STATUS_SUCCESS;
	
	if(fm_context.isAllocated)
	{
		return FM_STATUS_INVALID_PARM;
	}

	*fmContext = &fm_context;	
	(*fmContext)->fmCallcack = fmCallback;
	(*fmContext)->isAllocated = TIFM_TRUE;
	(*fmContext)->appHandle = appHandle;

	return status;
}

FmStatus FM_Destroy(FmContext **fmContext)
{
	FmStatus	status = FM_STATUS_SUCCESS;

	if((*fmContext)->isAllocated)
	{
		(*fmContext)->fmCallcack = NULL;
		(*fmContext)->isAllocated = TIFM_FALSE;
	}
	else
	{
		status = FM_STATUS_INVALID_PARM;
	}

	*fmContext = 0;
	
	return status;
}

FmStatus FM_Enable(FmContext *fmContext)
{
	FmStatus status = FM_STATUS_PENDING;
	BthalStatus halStatus;

	if(fmContext->isAllocated == TIFM_FALSE)
	{
		return FM_STATUS_INVALID_PARM;
	}

	/* Lock the stack */
	OS_LockFmStack();

	/* Register FM stack with BTHAL VC */
	halStatus = BTHAL_VC_Register(FmsApiBthalVcCallback,
                                                         (void*)fmContext,
                                                         &fmContext->vcContext);

	if (BTHAL_STATUS_PENDING != halStatus)
	{
		TIFM_Report(("FM: Fatal error - BTHAL_VC_Register failed: %s",
                                    BTHAL_StatusName(halStatus)));
	}
    
	/* If chip Init finished already - start FM On 
	Otherwise - wait for the chip init to finish, which
	will call the callback and only then power on the FM*/ 
	if ((fm_transport_getState() != TI_TRANSPORT_STATE_FM_ON) && (fm_transport_getState() != TI_TRANSPORT_STATE_ALL_ON))
	{	
		if (fm_transport_on() == FM_STATUS_SUCCESS)
		{
			status = FM_SetPower(FM_POWER_ON);
		}
		/* Else - need to wait for the FM On to finish and call the callback */
	}

	/* Unlock the stack */
    OS_UnlockFmStack();

	return status;
}

void FmsApiBthalVcCallback(const BthalVcEvent *vcEvent)
{
	TIFM_Report(("FmsApiBthalVcCallback: recevied BTHAL VC event %d, status %d",
                             vcEvent->type,
                             vcEvent->status));

	switch (vcEvent->type)
	{
	    case BTHAL_VC_EVENT_LINK_ESTABLISHED:
                break;
                
	    case BTHAL_VC_EVENT_LINK_RELEASED:
                break;
                
	    case BTHAL_VC_EVENT_FM_RX_AUDIO_PATH_CHANGED:
                break;
                
            default:
                break;
	}
}

FmStatus FM_Disable(FmContext *fmContext)
{
	BthalStatus halStatus;
    
	if(fmContext->isAllocated == TIFM_FALSE)
	{
		return FM_STATUS_INVALID_PARM;
	}
		
	/* Unregister FM stack from BTHAL VC */
	halStatus = BTHAL_VC_Unregister( &fmContext->vcContext);

	if (BTHAL_STATUS_PENDING != halStatus)
	{
		TIFM_Report(("FM: Fatal error - BTHAL_VC_Unregister failed: %s",
                                    BTHAL_StatusName(halStatus)));
	}

	return FM_SetPower(FM_POWER_OFF);
}

/* This callback is called only after FM called the chipManager */
void transportStatusCallback(TiTransportFmNotificationType notification)
{
    /* Lock the stack */
    OS_LockFmStack();

	if(notification == TI_TRANSPORT_FM_NOTIFICATION_FM_ON_COMPLETE)
	{
		FM_SetPower(FM_POWER_ON);	
	}

    /* Unlock the stack */
    OS_UnlockFmStack();
}
/*---------------------------------------------------------------------------
 *            FM_SetPower()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Set FM on/off.
 * 
 */
static FmStatus FM_SetPower(TIFM_U8 mode)
{
	FmStatus status = FM_STATUS_PENDING;

    /* Lock the stack */
    OS_LockFmStack();

	if(mode == FM_POWER_ON)
	{
		if(!TIFM_IsNodeAvailable(&(FMC(powerOnOp).op.node)))
		{
			status = FM_STATUS_FAILED_ALREADY_PENDING;
		}
		else
		{
			FMC(powerOnOp).op.opType = FM_CMD_POWER_ON;

			FmAddOperation(&FMC(powerOnOp));
		}

	}
	else /* Power-off */
	{
		if(!TIFM_IsNodeAvailable(&(FMC(powerOffOp).op.node)))
		{
			status = FM_STATUS_FAILED_ALREADY_PENDING;
		}
		else
		{
			FMC(powerOffOp).op.opType = FM_CMD_POWER_OFF;

			FmAddOperation(&FMC(powerOffOp));
		}
	}

	if(status == FM_STATUS_PENDING)
	{
		/*Handle the operation in the fms task context	*/
		NotifyFms();
	}

    /* Unlock the stack */
    OS_UnlockFmStack();
	
	return status;
}



/*---------------------------------------------------------------------------
 *            FM_SetMoSt()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Set Mono/Stereo
 *
 */
FmStatus FM_SetMoSt(FmContext *fmContext, TIFM_U8 mode)
{
	FmStatus status = FM_STATUS_PENDING;

    /* Lock the stack */
    OS_LockFmStack();

	if(fmContext->isAllocated == TIFM_FALSE)
	{
		status = FM_STATUS_INVALID_PARM;
	}
	else if(FMC(initState != INIT_STATE_FM_ON))
	{
		status = FM_STATUS_FAILED_FM_NOT_ON;
	}
	/* If the command is waiting in the queue only update 
	   the param and send the relevant event */
	else if(!TIFM_IsNodeAvailable(&(FMC(moStOp).op.node)))
	{
		FMC(moStOp).mode = mode;
		status = FM_STATUS_PENDING_UPDATE_CMD_PARAMS;
	}
	else
	{
		FMC(moStOp).op.opType = FM_CMD_MOST_SET;
		FMC(moStOp).mode = mode;

		FmAddOperation(&FMC(moStOp));

		/*Handle the operation in the fms task context	*/
		NotifyFms();
	}

    /* Unlock the stack */
    OS_UnlockFmStack();

	return status;
}

/*---------------------------------------------------------------------------
 *            FM_SetBand()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Set Band
 *
 */
FmStatus FM_SetBand(FmContext *fmContext, TIFM_U8 mode)
{
	FmStatus status = FM_STATUS_PENDING;

    /* Lock the stack */
    OS_LockFmStack();

	if(fmContext->isAllocated == TIFM_FALSE)
	{
		status = FM_STATUS_INVALID_PARM;
	}
	else if(FMC(initState != INIT_STATE_FM_ON))
	{
		status = FM_STATUS_FAILED_FM_NOT_ON;
	}
	/* If the command is waiting in the queue only update 
	   the param and send the relevant event */
	else if(!TIFM_IsNodeAvailable(&(FMC(bandSetOp).op.node)))
	{
		FMC(bandSetOp).mode = mode;
		status = FM_STATUS_PENDING_UPDATE_CMD_PARAMS;
	}
	else
	{
		FMC(bandSetOp).op.opType = FM_CMD_BAND_SET;
		FMC(bandSetOp).mode = mode;

		FmAddOperation(&FMC(bandSetOp));

		/*Handle the operation in the fms task context	*/
		NotifyFms();
	}
    /* Unlock the stack */
    OS_UnlockFmStack();

	return status;
}

/*---------------------------------------------------------------------------
 *            FM_MuteRadio()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Mute Radio
 *
 */
FmStatus FM_MuteRadio(FmContext *fmContext, TIFM_U8 mode)
{
	FmStatus status = FM_STATUS_PENDING;

    /* Lock the stack */
    OS_LockFmStack();

	if(fmContext->isAllocated == TIFM_FALSE)
	{
		status = FM_STATUS_INVALID_PARM;
	}
	else if(FMC(initState != INIT_STATE_FM_ON))
	{
		status = FM_STATUS_FAILED_FM_NOT_ON;
	}
	/* If the command is waiting in the queue only update 
	   the param and send the relevant event */
	else if(!TIFM_IsNodeAvailable(&(FMC(muteOp).op.node)))
	{
		FMC(muteOp).mode = mode;
		status = FM_STATUS_PENDING_UPDATE_CMD_PARAMS;

        /* Store the last used mute mode */
        FMC(muteState) = mode;
	}
	else
	{
		FMC(muteOp).op.opType = FM_CMD_MUTE;
		FMC(muteOp).mode = mode;

        /* Store the last used mute mode */
        FMC(muteState) = mode;

		FmAddOperation(&FMC(muteOp));

		/* Handle the operation in the fms task context	*/
		NotifyFms();
	}

    /* Unlock the stack */
    OS_UnlockFmStack();
	
	return status;
}

/*---------------------------------------------------------------------------
 *            FM_RfDependentMute()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Enable or Disable the RF dependent mute feature
 *
 */
FmStatus FM_RfDependentMute(FmContext *fmContext, TIFM_U8 mode)
{
    /* Lock the stack */
    OS_LockFmStack();

	FMC(rfMuteOn) = mode;

    /* Unlock the stack */
    OS_UnlockFmStack();
	
	/* The mute state didn't change, just the RF dependent state */
	return FM_MuteRadio(fmContext, FMC(muteState));
}

/*---------------------------------------------------------------------------
 *            FM_SetVolume()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Set volume
 *
 */
FmStatus FM_SetVolume(FmContext *fmContext, TIFM_U16 gain)
{
	FmStatus status = FM_STATUS_PENDING;

    /* Lock the stack */
    OS_LockFmStack();

	if(fmContext->isAllocated == TIFM_FALSE)
	{
		status = FM_STATUS_INVALID_PARM;
	}	
	else if(FMC(initState != INIT_STATE_FM_ON))
	{
		status = FM_STATUS_FAILED_FM_NOT_ON;
	}
	/* If the command is waiting in the queue only update 
	   the param and send the relevant event */
	else if(!TIFM_IsNodeAvailable(&(FMC(volumeSetOp).op.node)))
	{
		FMC(volumeSetOp).gain = gain;
		status = FM_STATUS_PENDING_UPDATE_CMD_PARAMS;
	}
	else
	{
		FMC(volumeSetOp).gain = gain;
		FMC(volumeSetOp).op.opType = FM_CMD_VOLUME_SET;

		FmAddOperation(&FMC(volumeSetOp));

		/*Handle the operation in the fms task context	*/
		NotifyFms();
	}
    /* Unlock the stack */
    OS_UnlockFmStack();

	return status;
}

/*---------------------------------------------------------------------------
 *            FM_SetRDS()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Mute Radio
 *
 */
FmStatus FM_SetRDS(FmContext *fmContext, TIFM_U8 mode)
{
	FmStatus status = FM_STATUS_PENDING;

    /* Lock the stack */
    OS_LockFmStack();

	if(fmContext->isAllocated == TIFM_FALSE)
	{
		status = FM_STATUS_INVALID_PARM;
	}
	else if(FMC(initState != INIT_STATE_FM_ON))
	{
		status = FM_STATUS_FAILED_FM_NOT_ON;
	}
	/* If the command is waiting in the queue only update 
	   the param and send the relevant event */
	else if(!TIFM_IsNodeAvailable(&(FMC(rdsSetOp).op.node)))
	{
		FMC(rdsSetOp).mode = mode;
		status = FM_STATUS_PENDING_UPDATE_CMD_PARAMS;
	}
	else
	{
		FMC(rdsSetOp).op.opType = FM_CMD_RDS_SET;
		FMC(rdsSetOp).mode = mode;

		FmAddOperation(&FMC(rdsSetOp));

		/*Handle the operation in the fms task context	*/
		NotifyFms();
	}
    /* Unlock the stack */
    OS_UnlockFmStack();

	return status;
}

/*---------------------------------------------------------------------------
 *            FM_Tune()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Tune Radio
 *
 */
FmStatus FM_Tune(FmContext *fmContext, TIFM_U32 freq)
{
	FmStatus status = FM_STATUS_PENDING;

    /* Lock the stack */
    OS_LockFmStack();

	if(fmContext->isAllocated == TIFM_FALSE)
	{
		status = FM_STATUS_INVALID_PARM;
	}
	else if(FMC(initState != INIT_STATE_FM_ON))
	{
		status = FM_STATUS_FAILED_FM_NOT_ON;
	}
	/* If the command is waiting in the queue only update 
	   the param and send the relevant event */
	else if(!TIFM_IsNodeAvailable(&(FMC(tuneOp).op.node)))
	{
		FMC(tuneOp).freq = freq;
		status = FM_STATUS_PENDING_UPDATE_CMD_PARAMS;
	}
	else
	{
		FMC(tuneOp).freq = freq;
		FMC(tuneOp).op.opType = FM_CMD_TUNE;

		FmAddOperation(&FMC(tuneOp));

		/*Handle the operation in the fms task context	*/
		NotifyFms();
	}
    /* Unlock the stack */
    OS_UnlockFmStack();

	return status;
}

/*---------------------------------------------------------------------------
 *            FM_RssiGet()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Get Current RSSI level
 *
 */
FmStatus FM_RssiGet(FmContext *fmContext)
{
	TIFM_BOOL isNodeOccupied, isCurOp;
	FmStatus status = FM_STATUS_PENDING;

    /* Lock the stack */
    OS_LockFmStack();

	isNodeOccupied = !(TIFM_IsNodeAvailable(&(FMC(rssiGetOp).op.node)));
	isCurOp = (FMC(curOp) != 0) && (FMC(curOp)->opType == FM_CMD_RSSI_GET);

	if(fmContext->isAllocated == TIFM_FALSE)
	{
		status = FM_STATUS_INVALID_PARM;
	}
	else if(FMC(initState != INIT_STATE_FM_ON))
	{
		status = FM_STATUS_FAILED_FM_NOT_ON;
	}
	/* If the command is in the queue or currently running no need to run it again */
	else if(isNodeOccupied || isCurOp)
	{
		status = FM_STATUS_FAILED_ALREADY_PENDING;
	}
	else
	{
		FMC(rssiGetOp).op.opType = FM_CMD_RSSI_GET;

		FmAddOperation(&FMC(rssiGetOp));

		/*	Handle the operation in the fms task context	*/
		NotifyFms();
	}
    /* Unlock the stack */
    OS_UnlockFmStack();

	return status;
}

/*---------------------------------------------------------------------------
 *            FM_Seek()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Seek
 *
 */
FmStatus FM_Seek(FmContext *fmContext, TIFM_U8 mode, TIFM_U8 direction)
{
	TIFM_BOOL isNodeOccupied, isCurOp;
	FmStatus status = FM_STATUS_PENDING;
	TIFM_U8 notify = TIFM_FALSE;

    /* Lock the stack */
    OS_LockFmStack();

	isNodeOccupied = !(TIFM_IsNodeAvailable(&(FMC(seekOp).op.node)));
	isCurOp = (FMC(curOp) != 0) && (FMC(curOp)->opType == FM_CMD_SEEK);

	/* Stop seek only if seek operation is currently running.
	   if it's on the queue cancel the seek operation */
	if(fmContext->isAllocated == TIFM_FALSE)
	{
		status = FM_STATUS_INVALID_PARM;
	}
	else if(FMC(initState != INIT_STATE_FM_ON))
	{
		status = FM_STATUS_FAILED_FM_NOT_ON;
	}
	else if(mode == FM_STOP_SEEK)
	{
		/* If operation is not on the list then either it is currently running and need to be stopped or
			or there is no seek operation available - no need to do stop seek */
		if(!isNodeOccupied)
		{
			/* If seek is currently running - update mode to stop seek*/
			if(isCurOp)
			{
				/* Send upper event that indicate to the fms that the seek operation should be stopped */
				FMC(upperEvent) = UPPER_EVENT_STOP_SEEK;
				notify = TIFM_TRUE;
			}
			else
			{
				/* There is no seek to stop so return a success event */
				status = FM_STATUS_SUCCESS;
			}
		}
		/* Seek operation is in the list - remove it before it starts */
		else
		{
			/* Seek operation is in the queue and not running currently - cancel the seek operation */
			TIFM_RemoveNodeList(&(FMC(seekOp).op.node));
			status = FM_STATUS_SUCCESS;
		}
	}
	/* Mode = Seek */
	else
	{
		/* seek command already sent and not finished (maybe not started yet) */
		if(isNodeOccupied || isCurOp)
		{
			status = FM_STATUS_FAILED_ALREADY_PENDING;
		}
		else
		{
			FMC(seekOp).dir = direction;
			FMC(seekOp).op.opType = FM_CMD_SEEK;

			FmAddOperation(&FMC(seekOp));

			notify = TIFM_TRUE;

		}

	}


    /* Unlock the stack */
    OS_UnlockFmStack();

	if(notify)
	{
		/* Handle the operation in the fms task context */
		NotifyFms();
	}

	return status;
}


/*---------------------------------------------------------------------------
 *            FM_SetAF()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Set Altervative Frequencies feature on/off
 *
 */
FmStatus FM_SetAF(FmContext *fmContext, TIFM_U8 mode)
{
	FmStatus status = FM_STATUS_PENDING;

    /* Lock the stack */
    OS_LockFmStack();

	if(fmContext->isAllocated == TIFM_FALSE)
	{
		status = FM_STATUS_INVALID_PARM;
	}
	else if(FMC(initState != INIT_STATE_FM_ON))
	{
		status = FM_STATUS_FAILED_FM_NOT_ON;
	}
	/* If the command is waiting in the queue only update 
	the param and send the relevant event */
	else if(!TIFM_IsNodeAvailable(&(FMC(setAFOp).op.node)))
	{
		FMC(setAFOp).mode = mode;
		status = FM_STATUS_PENDING_UPDATE_CMD_PARAMS;
	}
	else
	{
		FMC(setAFOp).mode = mode;
		FMC(setAFOp).op.opType = FM_CMD_SET_AF;

		FmAddOperation(&FMC(setAFOp));

		/*Handle the operation in the fms task context	*/
		NotifyFms();
	}
    /* Unlock the stack */
    OS_UnlockFmStack();

	return status;
}

/*---------------------------------------------------------------------------
 *            FM_GetCurrentFreq()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Get current frequency
 *
 * Return:	  current frequency
 */
FmStatus FM_GetCurrentFreq(FmContext *fmContext, TIFM_U32 *freq)
{
	FmStatus status = FM_STATUS_SUCCESS;

    OS_LockFmStack();
	if(fmContext->isAllocated == TIFM_FALSE)
	{
		status = FM_STATUS_INVALID_PARM;
	}
	else if(FMC(initState != INIT_STATE_FM_ON))
	{
		status = FM_STATUS_FAILED_FM_NOT_ON;
	}
	else
	{
		*freq = FMC(curStationParams).freq;
	}
    OS_UnlockFmStack();

	return status;
}

/*---------------------------------------------------------------------------
 *            FM_GetCurrentPi()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Get current pi
 *
 * Return:	  current pi
 */
FmStatus FM_GetCurrentPi(FmContext *fmContext, TIFM_U16 *pi)
{
	FmStatus status = FM_STATUS_SUCCESS;

    OS_LockFmStack();
	if(fmContext->isAllocated == TIFM_FALSE)
	{
		status = FM_STATUS_INVALID_PARM;
	}
	else if(FMC(initState != INIT_STATE_FM_ON))
	{
		status = FM_STATUS_FAILED_FM_NOT_ON;
	}
	else if(FMC(rdsSetOp).mode !=  FM_RDS_ON)
	{
		status = FM_STATUS_FAIL_RDS_OFF;
	}	
	else
	{
		if (FMC(curStationParams).piCode == NO_PI_CODE)
			status = FM_STATUS_FAIL_NO_VALUE_AVAILABLE;
		else
			*pi = FMC(curStationParams).piCode;
	}
    OS_UnlockFmStack();

	return status;
}

/*---------------------------------------------------------------------------
 *            FM_AudioEnable()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Internal stack function
 *
 */
FmStatus FM_AudioEnable(FmContext *fmContext)
{
	TIFM_BOOL isNodeOccupied, isCurOp;
	FmStatus status = FM_STATUS_PENDING;

    /* Lock the stack */
    OS_LockFmStack();

	isNodeOccupied = !(TIFM_IsNodeAvailable(&(FMC(audioEnableOp).op.node)));
	isCurOp = (FMC(curOp) != 0) && (FMC(curOp)->opType == FM_CMD_AUDIO_ENABLE);

	if(fmContext->isAllocated == TIFM_FALSE)
	{
		status = FM_STATUS_INVALID_PARM;
	}
	else if(FMC(initState != INIT_STATE_FM_ON))
	{
		status = FM_STATUS_FAILED_FM_NOT_ON;
	}
	/* If the command is waiting in the queue only update 
	   the param and send the relevant event */
	else if(isNodeOccupied || isCurOp)
	{
		status = FM_STATUS_FAILED_ALREADY_PENDING;
	}
	else
	{
		FMC(audioEnableOp).op.opType = FM_CMD_AUDIO_ENABLE;

		FmAddOperation(&FMC(audioEnableOp));

		/*Handle the operation in the fms task context	*/
		NotifyFms();
	}
    /* Unlock the stack */
    OS_UnlockFmStack();

	return status;
}

/*---------------------------------------------------------------------------
 *            FM_SetStereoBlend()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Set Stereo Blend
 *
 */
FmStatus FM_SetStereoBlend(FmContext *fmContext, TIFM_U8 mode)
{
	FmStatus status = FM_STATUS_PENDING;

    /* Lock the stack */
    OS_LockFmStack();

	if(fmContext->isAllocated == TIFM_FALSE)
	{
		status = FM_STATUS_INVALID_PARM;
	}
	else if(FMC(initState != INIT_STATE_FM_ON))
	{
		status = FM_STATUS_FAILED_FM_NOT_ON;
	}
	/* If the command is waiting in the queue only update 
	   the param and send the relevant event */
	else if(!TIFM_IsNodeAvailable(&(FMC(setStereoBlendOp).op.node)))
	{
		FMC(setStereoBlendOp).mode = mode;
		status = FM_STATUS_PENDING_UPDATE_CMD_PARAMS;
	}
	else
	{
		FMC(setStereoBlendOp).mode = mode;
		FMC(setStereoBlendOp).op.opType = FM_CMD_SET_STEREO_BLEND;

		FmAddOperation(&FMC(setStereoBlendOp));

		/*Handle the operation in the fms task context	*/
		NotifyFms();
	}
    /* Unlock the stack */
    OS_UnlockFmStack();

	return status;
}

/*---------------------------------------------------------------------------
 *            FM_SetDeemphasisMode()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Set de-emphasis mode
 *
 */
FmStatus FM_SetDeemphasisMode(FmContext *fmContext, TIFM_U8 mode)
{
	FmStatus status = FM_STATUS_PENDING;

    /* Lock the stack */
    OS_LockFmStack();

	if(fmContext->isAllocated == TIFM_FALSE)
	{
		status = FM_STATUS_INVALID_PARM;
	}
	else if(FMC(initState != INIT_STATE_FM_ON))
	{
		status = FM_STATUS_FAILED_FM_NOT_ON;
	}
	/* If the command is waiting in the queue only update 
	   the param and send the relevant event */
	else if(!TIFM_IsNodeAvailable(&(FMC(setDeemphasisModeOp).op.node)))
	{
		FMC(setDeemphasisModeOp).mode = mode;
		status = FM_STATUS_PENDING_UPDATE_CMD_PARAMS;
	}
	else
	{
		FMC(setDeemphasisModeOp).mode = mode;
		FMC(setDeemphasisModeOp).op.opType = FM_CMD_SET_DEEMPHASIS_MODE;

		FmAddOperation(&FMC(setDeemphasisModeOp));

		/*Handle the operation in the fms task context	*/
		NotifyFms();
	}
    /* Unlock the stack */
    OS_UnlockFmStack();

	return status;
}

/*---------------------------------------------------------------------------
 *            FM_RssiSearchLevel()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Set rssi search level
 *
 */
FmStatus FM_RssiSearchLevel(FmContext *fmContext, TIFM_S8 rssi_level)
{
	FmStatus status = FM_STATUS_PENDING;

    /* Lock the stack */
    OS_LockFmStack();

	if(fmContext->isAllocated == TIFM_FALSE)
	{
		status = FM_STATUS_INVALID_PARM;
	}
	else if(FMC(initState != INIT_STATE_FM_ON))
	{
		status = FM_STATUS_FAILED_FM_NOT_ON;
	}
	/* If the command is waiting in the queue only update 
	   the param and send the relevant event */
	else if(!TIFM_IsNodeAvailable(&(FMC(setRssiSearchLevelOp).op.node)))
	{
		FMC(setRssiSearchLevelOp).rssi_level = rssi_level;
		status = FM_STATUS_PENDING_UPDATE_CMD_PARAMS;
	}
	else
	{
		FMC(setRssiSearchLevelOp).rssi_level = rssi_level;
		FMC(setRssiSearchLevelOp).op.opType = FM_CMD_SET_RSSI_SEARCH_LEVEL;

		FmAddOperation(&FMC(setRssiSearchLevelOp));

		/*Handle the operation in the fms task context	*/
		NotifyFms();
	}
    /* Unlock the stack */
    OS_UnlockFmStack();

	return status;
}

/*---------------------------------------------------------------------------
 *            FM_SetPauseLevel()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Set pause level
 *
 */
FmStatus FM_SetPauseLevel(FmContext *fmContext, TIFM_U8 pause_level)
{
	FmStatus status = FM_STATUS_PENDING;

    /* Lock the stack */
    OS_LockFmStack();

	if(fmContext->isAllocated == TIFM_FALSE)
	{
		status = FM_STATUS_INVALID_PARM;
	}
	else if(FMC(initState != INIT_STATE_FM_ON))
	{
		status = FM_STATUS_FAILED_FM_NOT_ON;
	}
	/* If the command is waiting in the queue only update 
	   the param and send the relevant event */
	else if(!TIFM_IsNodeAvailable(&(FMC(setPauseLevelOp).op.node)))
	{
		FMC(setPauseLevelOp).pause_level = pause_level;
		status = FM_STATUS_PENDING_UPDATE_CMD_PARAMS;
	}
	else
	{
		FMC(setPauseLevelOp).pause_level = pause_level;
		FMC(setPauseLevelOp).op.opType = FM_CMD_SET_PAUSE_LEVEL;

		FmAddOperation(&FMC(setPauseLevelOp));

		/*Handle the operation in the fms task context	*/
		NotifyFms();
	}
    /* Unlock the stack */
    OS_UnlockFmStack();

	return status;
}

/*---------------------------------------------------------------------------
 *            FM_SetPauseDuration()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Set pause duration
 *
 */
FmStatus FM_SetPauseDuration(FmContext *fmContext, TIFM_U8 pause_duration)
{
	FmStatus status = FM_STATUS_PENDING;

    /* Lock the stack */
    OS_LockFmStack();

	if(fmContext->isAllocated == TIFM_FALSE)
	{
		status = FM_STATUS_INVALID_PARM;
	}
	else if(FMC(initState != INIT_STATE_FM_ON))
	{
		status = FM_STATUS_FAILED_FM_NOT_ON;
	}
	/* If the command is waiting in the queue only update 
	   the param and send the relevant event */
	else if(!TIFM_IsNodeAvailable(&(FMC(setPauseDurationOp).op.node)))
	{
		FMC(setPauseDurationOp).pause_duration = pause_duration;
		status = FM_STATUS_PENDING_UPDATE_CMD_PARAMS;
	}
	else
	{
		FMC(setPauseDurationOp).pause_duration = pause_duration;
		FMC(setPauseDurationOp).op.opType = FM_CMD_SET_PAUSE_DURATION;

		FmAddOperation(&FMC(setPauseDurationOp));

		/*Handle the operation in the fms task context	*/
		NotifyFms();
	}
    /* Unlock the stack */
    OS_UnlockFmStack();

	return status;
}

/*---------------------------------------------------------------------------
 *            FM_SetRdsRbds()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Choose between rds and rbds mode
 *
 */
FmStatus FM_SetRdsRbds(FmContext *fmContext, TIFM_U8 mode)
{
	FmStatus status = FM_STATUS_PENDING;

    /* Lock the stack */
    OS_LockFmStack();

	if(fmContext->isAllocated == TIFM_FALSE)
	{
		status = FM_STATUS_INVALID_PARM;
	}
	else if(FMC(initState != INIT_STATE_FM_ON))
	{
		status = FM_STATUS_FAILED_FM_NOT_ON;
	}
	/* If the command is waiting in the queue only update 
	   the param and send the relevant event */
	else if(!TIFM_IsNodeAvailable(&(FMC(setRdsRbdsModeOp).op.node)))
	{
		FMC(setRdsRbdsModeOp).mode = mode;
		status = FM_STATUS_PENDING_UPDATE_CMD_PARAMS;
	}
	else
	{
		FMC(setRdsRbdsModeOp).mode = mode;
		FMC(setRdsRbdsModeOp).op.opType = FM_CMD_SET_RDS_RBDS_MODE;

		FmAddOperation(&FMC(setRdsRbdsModeOp));

		/*Handle the operation in the fms task context	*/
		NotifyFms();
	}
    /* Unlock the stack */
    OS_UnlockFmStack();

	return status;
}

/*---------------------------------------------------------------------------
 *            FM_GetMoSt()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Get mono/stereo mode
 *
 */
FmStatus FM_GetMoSt(FmContext *fmContext)
{
	TIFM_BOOL isNodeOccupied, isCurOp;
	FmStatus status = FM_STATUS_PENDING;

    /* Lock the stack */
    OS_LockFmStack();

	isNodeOccupied = !(TIFM_IsNodeAvailable(&(FMC(moStGetOp).op.node)));
	isCurOp = (FMC(curOp) != 0) && (FMC(curOp)->opType == FM_CMD_MOST_GET);

	if(fmContext->isAllocated == TIFM_FALSE)
	{
		status = FM_STATUS_INVALID_PARM;
	}
	else if(FMC(initState != INIT_STATE_FM_ON))
	{
		status = FM_STATUS_FAILED_FM_NOT_ON;
	}
	else if(isNodeOccupied || isCurOp)
	{
		status = FM_STATUS_FAILED_ALREADY_PENDING;
	}
	else
	{
		FMC(moStGetOp).op.opType = FM_CMD_MOST_GET;

		FmAddOperation(&FMC(moStGetOp));

		/*Handle the operation in the fms task context	*/
		NotifyFms();
	}
    /* Unlock the stack */
    OS_UnlockFmStack();

	return status;
}

/*---------------------------------------------------------------------------
 *            FM_SetRxAudioPath()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Sets FM Rx audio path
 *
 */
FmStatus FM_SetRxAudioPath(FmContext *fmContext,
                           FmRxAudioPath audioPath)
{
	TIFM_BOOL isNodeOccupied, isCurOp;
	FmStatus status = FM_STATUS_PENDING;
	BthalStatus halStatus = BTHAL_STATUS_FAILED;
	BthalVcFmRxAudioPath vcAudioPath = BTHAL_VC_FM_RX_ANALOG_AUDIO_PATH_OFF;

	/* Lock the stack */
	OS_LockFmStack();

	if(fmContext->isAllocated == TIFM_FALSE)
	{
		status = FM_STATUS_INVALID_PARM;
	}
	else if(FMC(initState != INIT_STATE_FM_ON))
	{
		status = FM_STATUS_FAILED_FM_NOT_ON;
	}
	else
	{
            switch (audioPath)
            {
                case FM_RX_AUDIO_PATH_OFF:
                    vcAudioPath = BTHAL_VC_FM_RX_ANALOG_AUDIO_PATH_OFF;
                    break;

                case FM_RX_AUDIO_PATH_HEADSET:
                    vcAudioPath = BTHAL_VC_FM_RX_ANALOG_AUDIO_PATH_HEADSET;
                    break;

                case FM_RX_AUDIO_PATH_HANDSET:
                    vcAudioPath = BTHAL_VC_FM_RX_ANALOG_AUDIO_PATH_HANDSET;
                    break;

                default:
                    TIFM_Report(("FM_SetRxAudioPath: unknown audio path type %d",
                                 audioPath));
                    status = FM_STATUS_INVALID_PARM;
                    break;
            }

            halStatus = BTHAL_VC_FmRxSetAudioPath(fmContext->vcContext,
        								              vcAudioPath);

            if (BTHAL_STATUS_PENDING != halStatus)
            {
                TIFM_Report(("FM_SetRxAudioPath() returned status %d", halStatus));
                status = FM_STATUS_FAILED;
            }
	}
    
	/* Unlock the stack */
	OS_UnlockFmStack();

	return status;
}

#else  /*BTL_CONFIG_FM_STACK == BTL_CONFIG_ENABLED*/

FmStatus FM_Init(void)
{
	
		TIFM_Report(("FM_Init ()  -  BTL_CONFIG_FM_STACK Disabled "));

		return FM_STATUS_SUCCESS;
	
}

FmStatus FM_Deinit()
{
		TIFM_Report((" FM_Deinit() -  BTL_CONFIG_FM_STACK Disabled "));

		return FM_STATUS_SUCCESS;
}


#endif /*BTL_CONFIG_FM_STACK == BTL_CONFIG_ENABLED*/






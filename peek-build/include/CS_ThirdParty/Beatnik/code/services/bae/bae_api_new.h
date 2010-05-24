
/**
 * @file    bae_api.h
 *
 * API Definition for BAE SWE.
 *
 * @author  Richard Powell (richard@beatnik.com)
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author                  Modification
 *  -------------------------------------------------------------------
 *  12/23/2004  Ravi Tatavarthi (ravi@beatnik.com)      Created
 *  12/23/2004  Ravi Tatavarthi (ravi@beatnik.com)      Added NEW synchronous BAE API functions.
 *
 * (C) Copyright 2003 by Beatnik, Inc., All Rights Reserved
 */

#ifndef __BAE_API_NEW_H
#define __BAE_API_NEW_H

#include "bae/bae_i.h"
#include "bae/bae_api.h"
#include "mobileBAE.h"

#define baeGlobalSystem  \
        (bae_env_ctrl_blk_p->system)

#define baeEngageOutputHardware()  \
        mbEngageOutputHardware(baeGlobalSystem)

#define baeDisengageOutputHardware()  \
        mbDisengageOutputHardware(baeGlobalSystem)

#define baeCreateAndLoadPlayerFromFile(A,B,C)  \
        mbCreateAndLoadPlayerFromFile(bae_env_ctrl_blk_p->system, (A), (B), (C))

#define baeCreateAndLoadPlayerFromMemory(A,B,C,D)  \
        mbCreateAndLoadPlayerFromMemory(bae_env_ctrl_blk_p->system, (A), (B), (C), (D))

#define baeDestroy(A) \
	    mbDestroy((A))

#define baeSetRendererVoiceLimit(A, B) \
	    mbSetRendererVoiceLimit(bae_env_ctrl_blk_p->system, (A), (B))

#define baeGetRendererVoiceLimit(A, B) \
	    mbGetRendererVoiceLimit(bae_env_ctrl_blk_p->system, (A), (B))

#define baeSetOutputHardwareChannelCount(A) \
	    mbSetOutputHardwareChannelCount(bae_env_ctrl_blk_p->system, (A))

#define baeGetOutputHardwareChannelCount(A) \
	    mbGetOutputHardwareChannelCount(bae_env_ctrl_blk_p->system, (A))

#define baeSetOutputHardwareSampleRate(A) \
	    mbSetOutputHardwareSampleRate(bae_env_ctrl_blk_p->system, (A))

#define baeGetOutputHardwareSampleRate(A) \
	    mbGetOutputHardwareSampleRate(bae_env_ctrl_blk_p->system, (A))

#define baeAddBankFromFile(A,B,C) \
	    mbAddBankFromFile(bae_env_ctrl_blk_p->system, (A), (B), (C))

#define baeAddBankFromMemory(A,B,C,D)  \
        mbAddBankFromMemory(bae_env_ctrl_blk_p->system, (A), (B), (C), (D))

#define baeGetBankCount(A) \
		mbGetBankCount(bae_env_ctrl_blk_p->system, (A))

#define baeRemoveBank(A) \
		mbRemoveBank(bae_env_ctrl_blk_p->system, (A))
		
#define baeSetMixerPulseCallback(A,B,C) \
		mbSetMixerPulseCallback(bae_env_ctrl_blk_p->system, (A), (B), (C))
		
#define baeGetMixerPulseCallback(A) \
		mbGetMixerPulseCallback(bae_env_ctrl_blk_p->system, (A))

#define baeGetBAEVersion(A) \
		bae_get_version((A))

#define baePreroll(A) \
	    mbPreroll((A))

static inline mbResult baeStart(mbObjectID playerID)
{	
	mbResult result;
	mbBool engaged = mbIsOutputHardwareEngaged(bae_env_ctrl_blk_p->system, &result);
	if ((result == mbNo_Err) && (!engaged))
		result = mbEngageOutputHardware(bae_env_ctrl_blk_p->system);
	if (result == mbNo_Err) 
		result = mbStart(playerID);
	return result;
}

#define baeStop(A, B) \
	    mbStop((A), (B))

#define baeIsPlaying(A, B) \
	    mbIsPlaying((A), (B))

#define baeIsStopped(A, B) \
	    mbIsStopped((A), (B))

#define baePause(A) \
	    mbPause((A))

#define baeResume(A) \
	    mbResume((A))

#define baeIsPaused(A, B) \
	    mbIsPaused((A), (B))

#define baeEnableLooping(A, B) \
	    mbEnableLooping((A), (B))

#define baeIsLoopingEnabled(A, B) \
	    mbIsLoopingEnabled((A), (B))

#define baeGetCurrentLoopNum(A, B) \
	    mbGetCurrentLoopNum((A), (B))

#define baeSetLoopCount(A, B) \
	    mbSetLoopCount((A), (B))

#define baeGetLoopCount(A, B) \
	    mbGetLoopCount((A), (B))

#define baeGetDuration(A, B) \
	    mbGetDuration((A), (B))

#define baeSetPosition(A, B) \
	    mbSetPosition((A), (B))

#define baeGetPosition(A, B) \
	    mbGetPosition((A), (B))

#define baeSetPlayerTerminationCallback(A,B,C,D) \
	    mbSetPlayerTerminationCallback((A), (B), (C), (D))

#define baeGetPlayerTerminationCallback(A,B) \
	    mbGetPlayerTerminationCallback((A), (B))

#define baeAddPlayerPositionCallback(A,B,C,D,E) \
	    mbAddPlayerPositionCallback((A), (B), (C), (D), (E))

#define baeRemovePlayerPositionCallback(A,B) \
	    mbRemovePlayerPositionCallback((A), (B))

#define baeSetVolume(A, B) \
	    mbSetVolume((A), (B))

#define baeGetVolume(A, B) \
	    mbGetVolume((A), (B))

#define baeEngageDRC(A, B) \
	    mbEngageDRC((A), (B))

#define baeIsDRCEngaged(A, B) \
	    mbIsDRCEngaged((A), (B))

#define baeSetDRCParameters(A, B) \
	    mbSetDRCParameters((A), (B))

#define baeGetDRCParameters(A, B) \
	    mbGetDRCParameters((A), (B))

#define baeEnableMidiNormalizer(A) \
	    mbEnableMidiNormalizer(bae_env_ctrl_blk_p->system, (A))


#define baeIsFeatureSupported(A) \
	    mbIsFeatureSupported((A))

#endif //__BAE_API_NEW_H
